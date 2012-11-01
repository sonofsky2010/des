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

unsigned __int64 efunc(unsigned __int32 msg, unsigned __int64 key) {
	unsigned __int64 result = 0;
	result = msg << 31;

	result = permuteKey(result, SELECT, 32, 48);
	result = result ^ key;

	// FIND row i and col j 
	char sbox;
	int k;
	for (k = 0; k < 8; k++) {
		//sbox = getSixBits(result, k);
		// i row     = first and last bit represent 2bit number
		// j column  = middle 4 bits represent 4bit number
		//value = getValue(SBOX, i, j);
	}

	return result;
}

unsigned __int64 DESEncrypter::encryptBlock(unsigned __int64 plainMsg) {
	unsigned __int64 result = 0;

	// initial msg permute
	unsigned __int64 work = permuteKey(plainMsg, INITIAL_P, 64, 64);

	// split msg in half
	unsigned __int32 lefts[16];
	lefts[0] = work >> 31;
	unsigned __int32 rights[16];
	rights[0]= work;

	int i;
	for (i = 1; i < 16; i++) {
		lefts[i] = rights[i-1];
		rights[i] = lefts[i-1] + efunc(rights[i-1], keys[i]);
	}

	// run function f on 32bit msg and 48 bit key

	// for (int i = 0; i < 16; i++)
	// Li = Ri-1
	// Ri = Li-1 + f(Ri-1, Ki)

	// final 

	// function f
	// 32bits = expand(Ri-1)
	// B1B2B3B4B5B6B7B8 = 32bits XOR Ki
	// 32bit = S1(B1) S2(B2) S3(B3) S4(B4) S5(B5) S6(B6) S7(B7) S8(B8)
	// 32bit = permute(32bit, P)

	

	return result;
}

unsigned __int64 DESEncrypter::decryptBlock(unsigned __int64 encryptedMsg) {
	unsigned __int64 result = 0;

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