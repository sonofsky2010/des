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
		char bit = bitUtils.checkBitFromLeft(bigEndianKey, whichBit);
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

int DESEncrypter::getColumn(unsigned char sixBits) {
	int result = 0;

	// TODO
	// (0 index)

	// get 1st - set as 0th from right
	if (bitUtils.checkBitFromRight(sixBits, 1)) {
		bitUtils.setBitFromRight(result, 0);
	}
	// get 2nd - set as 1st from right
	if (bitUtils.checkBitFromRight(sixBits, 2)) {
		bitUtils.setBitFromRight(result, 1);
	}
	// get 3rd - set as 2nd from right
	if (bitUtils.checkBitFromRight(sixBits, 3)) {
		bitUtils.setBitFromRight(result, 2);
	}
	// get 4th - set as 3rd from right
	if (bitUtils.checkBitFromRight(sixBits, 4)) {
		bitUtils.setBitFromRight(result, 3);
	}

	return result;
}

int DESEncrypter::getRow(unsigned char sixBits) {
	int result = 0;

	// TODO 
	// get first bit(0th) - set as 2nd from right in result
	if (bitUtils.checkBitFromRight(sixBits, 0)) {
		bitUtils.setBitFromRight(result, 0);
	}
	// get last bit(5th) - set as 1st from right in result
	if (bitUtils.checkBitFromRight(sixBits, 5)) {
		bitUtils.setBitFromRight(result, 2);
	}

	return result;
}

// Actually returns 8 bits but we just don't use the first 2
unsigned char DESEncrypter::getSixBits(unsigned __int64 data, int group) {
	// data is 48 bits - 00..00data
	unsigned char result = 0;

	// TODO
	unsigned char sixBits[8];

	int i;
	for (i = 0; i < 8; i++) {
		unsigned char bits = data >> (8 * i);
		sixBits[i] = bits;
	}

	return result;
}

unsigned __int64 DESEncrypter::efunc(unsigned __int32 msg, unsigned __int64 key) {
	unsigned __int64 result = 0;

	// result should be 0000..0000msg
	result = msg << 31;
	// result should be msg0000..0000

	result = permuteKey(result, SELECT, 32, 48);
	// result should now be  expandedBits0000..0000

	result = result ^ key;

	// FIND row i and col j 
	unsigned char sbox;
	int k;
	for (k = 0; k < 8; k++) {
		sbox = getSixBits(result, k);

		int col = getColumn(sbox);
		int row = getRow(sbox);
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