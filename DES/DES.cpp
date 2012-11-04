// DES.cpp : Defines the entry point for the console application.
// Handled verification and normalization of arguments to the application.
//
// Author: Nick Caley
// Date: 10/20/2012
//

#include "stdafx.h"
#include "DESEncrypter.h"

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

struct dataNodeHead {
	unsigned __int64 fileInfo;
	int maxBlock;
	struct dataNode *next;
};

struct dataNode {
	unsigned __int64 data;
	struct dataNode *next;
};


void debugDES(DESEncrypter des) {
	/************  TEST  ************/
	unsigned __int64 test64 = 0;
	char *input = "The Quick Brown Fox Jumped Over The Lazy Dog";
	int i;
	for (i = 0; i < strlen(input); i++) {
		test64  = test64 | *(input+i);
		test64 = test64 << 8;
	}

	unsigned __int64 test2 = test64;
	printf("test2 = %llx\n", test2);
	char *result = (char *) std::malloc(sizeof(char)*8);
	for (i = 0; i < 8; i++) {
		char c = test2;
		*(result+i) = c;
		test2 = test2 >> 8;
	}
	printf("FINAL RESULT = %s\n", result);
	
	printf("Original Message in hex: %llx\n", test64);
	unsigned __int64 e = des.encryptBlock(test64);
	printf("Encrpyted Message in hex: %llx\n", e);
	unsigned __int64 d = des.decryptBlock(e);
	printf("Decrypted Message in hex: %llx\n", d);

	result = (char *) std::malloc(sizeof(char)*8);
	for (i = 0; i < 8; i++) {
		char c = d;
		*(result+i) = c;
		d = d >> 8;
	}
	printf("FINAL RESULT = %s\n", result);
	/************  TEST  ************/
}

/* 
 * Reads blocks of 64bit data from a file and stores them in a linked
 * list struct. returns the head. the first node contains the file size
 */
struct dataNodeHead readFile(char *filepath) {
	// get file size
	struct __stat64 fileStat;  
	int err = _stat64(filepath, &fileStat); 
	if (0 != err) {
		printf("FILEPATH: %s\n", filepath);
		perror("The following error occured");
		exit(1);			// non-zero err indicates an error
	}
	__int64 fileSize = (int) (fileStat.st_size & 0x000000007fffffff);
	printf("FILESIZE: %llx\n", fileSize);

	// open file for reading
	FILE *file;
	fopen_s(&file, filepath, "rb");

	if (file == NULL) {
		printf("Could not open file at %s\n", filepath);
		exit(1);
	}
	struct dataNodeHead result;
	struct dataNode *head = NULL, 
					*tmp = NULL, 
					*current = NULL;
	
	__int64 MAX_BLOCK;
	if (fileSize % 8 != 0) {
		MAX_BLOCK = (fileSize % 8) + 1;
	}
	else {
		MAX_BLOCK = (fileSize % 8);
	}
	printf("MAX BLOCK 1: %d\n", MAX_BLOCK);

	unsigned __int64 currentData = 0, currentBlock = 0;
	while (currentBlock <= MAX_BLOCK) {
		fread(&currentData, 8, 1, file);	// read into buffer
		printf("READ DATA FROM FILE: %llx\n", currentData);

		current = (struct dataNode *) malloc(sizeof(struct dataNode));
		current->data = currentData;
		current->next = NULL;

		if (head == NULL) {
			head = current;
		}
		else if (tmp != NULL) {
			tmp->next = current;
		}
		tmp = current;
		currentBlock++;
	}
	fclose(file);

	result.fileInfo = fileSize;
	result.next = head;
	result.maxBlock = MAX_BLOCK;

	printf("Done reading file\n");
	return result;
}

/* 
 * Writes out blocks of 64bit data to a file
 */
void writeFile(char *filepath, struct dataNodeHead start) {
	printf("Writing file %s\n", filepath);
	FILE *file;
	fopen_s(&file, filepath, "w+b");

	if (file == NULL) {
		printf("Could not open file for writing at %s\n", filepath);
		perror("The following error occured");
		exit (1);
	}
	size_t wrote = fwrite(&(start.fileInfo), 8, 1, file);
	printf("WROTE %u bytes\n", wrote);

	int MAX_BLOCK = start.maxBlock,
		currentBlock = 0;
	printf("MAX %d\n", start.maxBlock);
	struct dataNode *current = start.next;
	while (currentBlock <= MAX_BLOCK) {


		printf("___data: %llx\n", current->data);
		fwrite(&(current->data), 8, 1, file);

		if (currentBlock < MAX_BLOCK) {
			current = current->next;
		}
		currentBlock++;
	}
	printf("Done writing file\n");

	fclose(file);
	perror("The following error occured");
}

/*
 * Handles encryption/decryption
 */
void desStuff(DESEncrypter des, char action, char *inputFile, char *outputFile) {

	struct dataNodeHead plainText = readFile(inputFile),
						*newHead = (struct dataNodeHead *) malloc(sizeof(struct dataNodeHead));
	struct dataNode *current, 
					*newCurrent, 
					*tmp = NULL;
	current = plainText.next;

	if (action == 'e') {
		// TODO - adjust current->data to contain garbage on one side
		newHead->fileInfo = des.encryptBlock(current->data);
		newHead->maxBlock = plainText.maxBlock;
		newHead->next = NULL;

		current = current->next;

		while (current != NULL) {
			unsigned __int64 encryptedMessage = des.encryptBlock(current->data);
			newCurrent = (struct dataNode *) malloc(sizeof(struct dataNode));
			newCurrent->next = NULL;
			newCurrent->data = encryptedMessage;

			// create new LL of encrypted data from LL of read-in data
			if (newHead->next == NULL) {
				newHead->next = newCurrent;
			}
			else if (tmp != NULL) {
				tmp->next = newCurrent;
			}
			tmp = newCurrent;

			current = current->next;
		}
	}
	else if (action == 'd') {
		// TODO - adjust current->data to contain garbage on one side
		newHead->fileInfo = des.decryptBlock(current->data);
		newHead->next = NULL;

		current = current->next;

		while (current != NULL) {
			unsigned __int64 encryptedMessage = des.decryptBlock(current->data);
			newCurrent = (struct dataNode *) malloc(sizeof(struct dataNode));

			// create new LL of encrypted data from LL of read-in data
			if (newHead->next == NULL) {
				newHead->next = newCurrent;
			}
			else if (tmp != NULL) {
				tmp->next = newCurrent;
			}
			tmp = newCurrent;

			current = current->next;
		}
	}
	printf("Done with action\n");
	writeFile(outputFile, *newHead);
}

/* Verifies that the key is legal. If it is surrounded in single quotes then it 
 * may contain any 8 ASCII characters. If it is not surrounded in quotes, then 
 * the key must contain 16 hex characters.
 */
int verifyKey(char *key) {    // should this be const?
	char *c;
	int size, result = 0, i;

	size = strlen(key);
	c = key;
	// check to see if we have an 8 byte string
	if (size == 10) {
		// if we do, it should be surrounded in single quotes
		if (*c == 39 && *(c + size - 1) == 39) {
			// check if the values are valid ASCII
		}
	}
	// or a 16 hex string
	else if (size == 16) {
		// check if all values are hex - and convert string to direct hex vals
		for (c; c < (c + size); c++) {
			*c = (*c) - 30;		// convert to hex from ASCII
			// is the hex value valid?
			if (*c <= 16 && *c >= 0) {
				// good - should we do anything?
			}
			else {
				// we have a non-hex value when we should have hex
				result = 1;
				break;
			}
		}
		
	}
	// otherwise - invalid key size
	else {
		result = 1;
	}

	return result;
}


/* Verifies that the action given is a valid one based on the ASCII value 
 * of the char. Also, if 'action' was capitalized, it reduces its value to 
 * lowercase.
 */
int verifyAction(int *action) {
	int result = 0;
	switch (*action) {
	case 'E': 
		*action = *action + 32;
	case 'e':
		printf("Encrypting...\n");
		break;
	case 'D':
		*action = *action + 32;
	case 'd':
		printf("Decrypting...\n");
		break;
	default:
		printf("Unknown action specified (%c)\n", action);
		result = 1;
	}
	return result;
}

// TODO - uses char(ASCII) instead of _TCHAR(UNICODE)
int _tmain(int argc, char *argv[])
{
	const char *helpMsg = "Usage: des -<action> <16HEXchars/'8chars'> <input-file> <output-file>";
	const int expectedArgs = 4;
	
	char *inputFile;
	char *outputFile;
	char *key;
	__int64 desKey = 0x133457799BBCDFF1;
	int action;

	// Check to make sure we have enough arguments to continue
	if (argc < expectedArgs+1) {
		printf("Expected %u arguments, found %u\n%s", expectedArgs, argc-1, helpMsg);
		return 1;
	}
	else {
		printf("Found %u args\n", argc-1);
	}

	int i;
	for (i = 0; i < argc; i++) {
		std::cout << argv[i] << std::endl;
	}
	
	// parse first arg - action
	action = *argv[1];
	if (verifyAction(&action)) { return 1;}
	printf("action %c\n\n", action);

	// parse second arg - 16 hex chars or 8 char string
	key = argv[2];
	//if (verifyKey(key)) {return 1;}

	// parse third arg - input file
	inputFile = argv[3];
	printf("INPUT FILE: %s\n", inputFile);
	
	// parse fourth arg - output file
	outputFile = argv[4];

	printf("KEY: %llx\n", desKey);
	// TODO - DES
	DESEncrypter des = DESEncrypter::DESEncrypter(desKey);

	//debugDES(des);

	desStuff(des, action, inputFile, outputFile);

	return 0;
}
