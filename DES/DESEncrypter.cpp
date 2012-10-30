#include "stdafx.h"
#include "DESEncrypter.h"

__int64 DESEncrypter::permuteKey(__int64 key, const short table[], int lengthIn, int lengthOut) {
	__int64 bigEndianKey = bitUtils.toggleEndian(key);
	__int64 result = 0;
	printf("KEY: %llx\n", bigEndianKey);
	int i;
	for (i = 0; i < lengthOut; i++) {
		int whichBit = table[i];
		printf("TABLE INDEX: %d   ", whichBit);
		char bit = bitUtils.checkBitFromLeft(bigEndianKey, whichBit);
		printf("   bitcheck: %d   \n", bit);
		if (bit) { bitUtils.setBitFromLeft(result, i); }
	}

	printf("result: %llx\n", result);

	// Should return result in bigEndian
	return result;
}

void DESEncrypter::createSubkeys(__int64 key) {
	int lengthIn = 64;
	int lengthOut = 56;
	__int64 initialPermutedKey = permuteKey(key, PC1, lengthIn, lengthOut);
}

__int64 DESEncrypter::encryptBlock(__int64 plainMsg) {
	__int64 result = 0;

	return result;
}

__int64 DESEncrypter::decryptBlock(__int64 encryptedMsg) {
	__int64 result = 0;

	return result;
}

DESEncrypter::DESEncrypter(__int64 key) {
	printf("CONSTR KEY: %llx\n", key);
	//originalKey = bitUtils.toggleEndian(key);
	createSubkeys(key);
}


DESEncrypter::~DESEncrypter(void) {
}