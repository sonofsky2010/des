#include "stdafx.h"
#include "DESEncrypter.h"

unsigned __int64 DESEncrypter::permuteKey(unsigned __int64 key, const short table[], int lengthIn, int lengthOut) {
	unsigned __int64 bigEndianKey = bitUtils.toggleEndian(key);
	unsigned __int64 result = 0;
	//printf("KEY: %llx\n", key);
	int i;
	for (i = 0; i < lengthOut; i++) {
		int whichBit = table[i];
		//printf("TABLE INDEX: %d   ", whichBit);
		char bit = bitUtils.checkBitFromLeft(key, whichBit);
		//printf("   bitcheck: %d   \n", bit);
		//printf("\toriginal %d bit:   %d\n", i, bitUtils.checkBitFromLeft(key, i));
		if (bit) { result = bitUtils.setBitFromLeft(result, i); }
	}

	
	printf("permute result: %llx\n", result);
	//result = result >> 8;
	// Should return result in bigEndian
	return result;
}

void DESEncrypter::createSubkeys(unsigned __int64 key) {
	int lengthIn = 64;
	int lengthOut = 56;
	unsigned __int64 initialPermutedKey = permuteKey(key, PC1, lengthIn, lengthOut);

	unsigned __int32 right = initialPermutedKey >> 32;
	unsigned __int32 left = initialPermutedKey;

	unsigned __int32 cees[16];
	unsigned __int32 dees[16];
	cees[0] = left;
	dees[0] = right;

	int i;

	// create split keys
	for (i = 1; i < 16; i++) {
		int shiftAmount;
		if ( i == 1 || i == 2 || i == 9 || i == 16) {
			shiftAmount = 1;
		}
		else {
			shiftAmount = 2;
		}

		cees[i] = bitUtils.rollLeft32(cees[i-1], shiftAmount);
		dees[i] = bitUtils.rollLeft32(dees[i-1], shiftAmount);
	}

	// create the final keys
	for (i = 0; i < 16; i++) {
		__int64 concatkey = (cees[i] << 31 | dees[i]);
		keys[i] = permuteKey(concatkey, PC2, 56, 48);
	}
}

__int64 DESEncrypter::encryptBlock(__int64 plainMsg) {
	__int64 result = 0;

	return result;
}

__int64 DESEncrypter::decryptBlock(__int64 encryptedMsg) {
	__int64 result = 0;

	return result;
}

DESEncrypter::DESEncrypter(unsigned __int64 key) {
	printf("CONSTR KEY: %llx\n", key);
	//originalKey = bitUtils.toggleEndian(key);
	createSubkeys(key);
	int i;
	for (i = 0; i < 16; i++) {
		printf("key %d = %llx\n", i, keys[i]);
	}
}


DESEncrypter::~DESEncrypter(void) {
}