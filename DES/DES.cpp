// DES.cpp : Defines the entry point for the console application.
// Handled verification and normalization of arguments to the application.
//
// Author: Nick Caley
// Date: 10/20/2012
//

#include "stdafx.h"
#include "DESEncrypter.h"

#include <cstdlib>
#include <stdio.h>
#include <string.h>

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
		*action = *action - 32;
	case 'e':
		printf("Encrypting...\n");
		break;
	case 'D':
		*action = *action - 32;
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
	
	// parse first arg - action
	action = *argv[1];
	//if (verifyAction(&action)) { return 1;}

	// parse second arg - 16 hex chars or 8 char string
	key = argv[2];
	//if (verifyKey(key)) {return 1;}

	// parse third arg - input file
	inputFile = argv[3];
	// TODO - check if file exists and we can read
	
	// parse fourth arg - output file
	outputFile = argv[4];
	// TODO - check if file exists and have write permission

	printf("KEY: %llx\n", desKey);
	// TODO - DES
	DESEncrypter des = DESEncrypter::DESEncrypter(desKey);
	
	unsigned __int64 test64 = 0;
	char *input = "The Quick Brown Fox Jumped Over The Fence";
	int i;
	for (i = 0; i < 26; i++) {
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

	return 0;
}

