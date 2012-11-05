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
	int blocks;
	int remainder;
	struct dataNode *next;
};

struct dataNode {
	unsigned __int64 data;
	struct dataNode *next;
};


void debugDES(DESEncrypter des) {
	/************  TEST  ************/
	unsigned __int64 test64 = 0;
	char *input = "type things";// and he got away with it panda banana horse fidjfgdsoijgosgsjo";
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
	
	//test64 = 0x50a7b03c8b0d65;

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
	// determine number of blocks we need to write
	__int64 MAX_BLOCK = (__int64)(fileSize / 8);
	if (fileSize % 8 != 0) {
		MAX_BLOCK++;
		result.blocks = MAX_BLOCK;
		result.remainder = fileSize % 8;
	}
	/*printf("\n---- File Info ----\n");
	printf("Filesize: %d\n", fileSize);
	printf("Filesize/8 (Max Blocks): %d\n", fileSize/8);
	printf("Filesize Remainder (Partial Block): %d\n", fileSize % 8);
	printf("---- ---- ---- ----\n\n");*/

	unsigned __int64 currentData = 0, currentBlock = 0;
	while (currentBlock <= MAX_BLOCK) {
		fread(&currentData, 8, 1, file);	// read into buffer
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

	//printf("Done reading file\n");
	return result;
}

/* 
 * Writes out blocks of 64bit data to a file
 */
void writeFile(char *filepath, struct dataNodeHead start) {
	printf("Writing file %s\n", filepath);
	FILE *file;
	fopen_s(&file, filepath, "wb");

	if (file == NULL) {
		printf("Could not open file for writing at %s\n", filepath);
		perror("The following error occured");
		exit (1);
	}
	size_t wrote = fwrite(&(start.fileInfo), 8, 1, file);

	int MAX_BLOCK = start.blocks,
		currentBlock = 0;
	struct dataNode *current = start.next;
	//printf("Want to write %d blocks\n", MAX_BLOCK);
	while (currentBlock <= MAX_BLOCK) {
		if (&(current->data) == NULL) {
			break;
		}
		fwrite(&(current->data), sizeof(__int64), 1, file);
		current = current->next;
		currentBlock++;
	}
	//printf("Done writing file\n");

	fclose(file);
}

void writeDecryptedFile(char *filepath, struct dataNodeHead start) {
	printf("Writing decrypted file %s\n", filepath);
	FILE *file;
	fopen_s(&file, filepath, "w+b");
	if (file == NULL) {
		printf("Could not open file for writing at %s\n", filepath);
		perror("The following error occured");
		exit (1);
	}

	int currentBlock = 0, MAX_BLOCK = start.blocks;
	//printf("NUMBER OF DECRYPTED BLOCKS: %d\n", start.blocks);
	//printf("REMAINDER: %d\n", start.remainder);
	struct dataNode *current = start.next;
	if (start.blocks % 8 == 0) {
		MAX_BLOCK++;
	}
	while (currentBlock <= MAX_BLOCK) {
		
		//printf("___data: %llx\n", current->data);
		if (&(current->data) == NULL) {
			break;
		}
		if (currentBlock == start.blocks && start.remainder != 0) {
			fwrite(&(current->data), start.remainder, 1, file);
			//perror("The following error occured");
		}
		else {
			fwrite(&(current->data), sizeof(__int64), 1, file);
			//perror("The following error occured");
		}
		
		current = current->next;
		currentBlock++;
	}
	//printf("Done writing file\n");

	fclose(file);
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
	int BLOCKS = 0;
	/*while (BLOCKS < plainText.blocks) {
		printf("data: %llx\n", current->data);
		BLOCKS++;
		current = current->next; 
	}*/
	current = plainText.next;

	if (action == 'e') {
		// TODO - adjust current->data to contain garbage on one side
		newHead->fileInfo = des.encryptBlock(plainText.fileInfo);
		newHead->blocks = plainText.blocks;
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

		writeFile(outputFile, *newHead);
	}
	else if (action == 'd') {
		// TODO - adjust current->data to contain garbage on one side
		newHead->fileInfo = des.decryptBlock(plainText.next->data);
		newHead->blocks = (int) ((newHead->fileInfo)/8);
		newHead->remainder = (newHead->blocks) % 8;
		newHead->next = NULL;

		current = current->next;

		while (current != NULL) {
			unsigned __int64 encryptedMessage = des.decryptBlock(current->data);
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

		writeDecryptedFile(outputFile, *newHead);
	}
	
	printf("\n---- DONE! ----\n");
}

/* Verifies that the key is legal. If it is surrounded in single quotes then it 
 * may contain any 8 ASCII characters. If it is not surrounded in quotes, then 
 * the key must contain 16 hex characters.
 */
int verifyKey(char *key) {    // should this be const?
	printf("Using Key: %s\n", key);
	char *c;
	int size, result = 0, i;

	size = strlen(key);
	//printf("Key size: %d\n", size);
	c = key;
	// check to see if we have an 8 byte stringencryptedMsg
	if (size == 10) {
		// if we do, it should be surrounded in single quotes
		if (key[0] == 39 && (key[size-1]) == 39) {
			// check if the values are valid ASCII
		}
	}
	// or a 16 hex string
	else if (size == 16) {
		// check if all values are hex - and convert string to direct hex vals
		for (i=0; i < size; i++) {
			if (key[i] >= 38 && key[i] <= 57) {
				key[i] = key[i] - 48;
			}
			else if (key[i] >= 65 && key[i] <= 70) {
				key[i] = key[i] - 55;
			}
			//printf("KEY[%d] = %d\n", i, key[i]);
			// is the hex value valid?
			if (key[i] <= 16 && key[i] >= 0) {
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
int main(int argc, char *argv[])
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

	// TODO _ FIX THIS _
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
		//std::cout << argv[i] << std::endl;
	}
	// TODO _ FIX THIS _

	
	// parse first arg - action
	action = *argv[1];
	if (verifyAction(&action)) { return 1;}
	printf("Action %c\n\n", action);

	// parse second arg - 16 hex chars or 8 char string
	key = argv[2];
	if (verifyKey(key)) {printf("Bad key\n"); return 1;}

	// parse third arg - input file
	inputFile = argv[3];
	printf("Input File: %s\n", inputFile);
	
	// parse fourth arg - output file
	outputFile = argv[4];
	printf("Output File: %s\n", outputFile);

	printf("Key: %llx\n", desKey);
	DESEncrypter des = DESEncrypter::DESEncrypter(desKey);

	//debugDES(des);

	desStuff(des, action, inputFile, outputFile);

	return 0;
}
