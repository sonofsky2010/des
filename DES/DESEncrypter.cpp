#include "stdafx.h"
#include "DESEncrypter.h"

void DESEncrypter::createSubkeys(unsigned __int64 key) {
	int lengthIn = 64;
	int lengthOut = 56;

	unsigned __int64 initialPermutedKey = 0;
	int k;
	for (k = 0; k < 56; k++) {
		unsigned char whichPlace = PC1[k];
		unsigned char bit = bitUtils.checkBitFromLeft(key, whichPlace);
		if (bit) {
			initialPermutedKey = bitUtils.setBitFromLeft(initialPermutedKey, 8+k);
		}
	}
	// initialPermutedKey = 00..00initialPermutedKey

	unsigned __int32 right = initialPermutedKey >> 32;
	unsigned __int32 left = initialPermutedKey;

	unsigned __int32 cees[16];
	unsigned __int32 dees[16];
	cees[0] = left;
	dees[0] = right;

	int i;
	for (i = 1; i < 16; i++) {
		int shiftAmount;
		if ( i == 1 || i == 2 || i == 9 || i == 16) {
			shiftAmount = 1;
		}
		else {
			shiftAmount = 2;
		}

		cees[i] = bitUtils.rollLeft28(cees[i-1], shiftAmount);
		dees[i] = bitUtils.rollLeft28(dees[i-1], shiftAmount);
	}

	// create the final keys
	for (i = 0; i < 16; i++) {
		__int64 concatkey = 0;
		concatkey += cees[i];
		concatkey = concatkey << 32;
		concatkey += dees[i];
		
		int j;
		for (j = 0; j < 48; j++) {
			unsigned char whichPlace = PC2[j];
			unsigned char bit = bitUtils.checkBitFromLeft(concatkey, whichPlace);
			if (bit) {
				concatkey = bitUtils.setBitFromLeft(concatkey, 16+j);
				// 00..00concatKey
			}
		}
		keys[i] = concatkey;
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

unsigned __int32 DESEncrypter::efunc(unsigned __int32 msg, unsigned __int64 key) {
	unsigned __int32 result = 0, 
					 total = 0;

	// expand message half to 48 bits
	int i;
	for (i = 0; i < 48; i++) {
		unsigned char whichPlace = SELECT[i];
		unsigned char bit = ( 1 &( msg >> (32-whichPlace)) );	// checkBitFromLeft32
		if (bit) {
			result = (msg | (1<<(32-i)));						// setBitFromLeft32
		}
	}
	// result should now be  expandedBits0000..0000

	result = result >> 16;
	// result should now be 0000..0000expandedBits

	// 0000..0000expandedBits XOR 0000..0000key
	result = (result ^ key);

	// FIND row i and col j 
	unsigned char sbox, sboxResult;
	int k;
	unsigned char sboxes[8];
	for (k = 0; k < 8; k++) {
		//sbox = getSixBits(result, k);
		sbox = result >> (8 * k);
		int col = getColumn(sbox);
		int row = getRow(sbox);
		
		switch (k) {
		case 0:
			sboxResult = S1[row][col];
			break;
		case 1:
			sboxResult = S2[row][col];
			break;
		case 2:
			sboxResult = S3[row][col];
			break;
		case 3:
			sboxResult = S4[row][col];
			break;
		case 4:
			sboxResult = S5[row][col];
			break;
		case 5:
			sboxResult = S6[row][col];
			break;
		case 6:
			sboxResult = S7[row][col];
			break;
		case 7:
			sboxResult = S8[row][col];
			break;
		};

		total += sboxResult;	
		total = total << 4;
	}

	// permute
	unsigned __int32 result2 = 0;
	int m;
	for (m = 0; m < 32; m++) {
		unsigned char whichPlace = PERMUTE[m];
		unsigned char bit = ( 1 &( msg >> (32-whichPlace)) );
		if (bit) {
			result2 = (result | (1<<(32-i)));
		}
	}
	return result2;
}

unsigned __int64 DESEncrypter::encryptBlock(unsigned __int64 plainMsg) {
	plainMsg = bitUtils.toggleEndian(plainMsg);
	unsigned __int64 result = 0;

	// initial msg permute
	unsigned __int64 work = 0;
	int j;
	for (j = 0; j < 64; j++) {
		unsigned char whichPlace = INITIAL_P[j];
		unsigned char bit = bitUtils.checkBitFromLeft(plainMsg, whichPlace-1);
		if (bit) {
			work = bitUtils.setBitFromLeft(work, j);
		}
	}

	// split msg in half
	unsigned __int32 rights[16];
	rights[0]= work;

	unsigned __int32 lefts[16];
	lefts[0] = work >> 32;

	// function
	int i;
	for (i = 1; i < 16; i++) {
		lefts[i] = rights[i-1];
		rights[i] = (lefts[i-1] ^ (efunc(rights[i-1], keys[i])));
	}

	// use the final iteration	
	result = (result | lefts[15]);
	result = result << 32;
	result = result | rights[15];
	
	// final permutation of message
	unsigned __int64 result2 = 0;
	int k;
	for (k = 0; k < 64; k++) {
		unsigned char whichPlace = FINAL_P[k];
		unsigned char bit = bitUtils.checkBitFromLeft(result, whichPlace-1);
		if (bit) {
			result2 = bitUtils.setBitFromLeft(result2, k);
		}
	}

	result2 = bitUtils.toggleEndian(result2);
	return result2;
}

unsigned __int64 DESEncrypter::decryptBlock(unsigned __int64 encryptedMsg) {
	encryptedMsg = bitUtils.toggleEndian(encryptedMsg);
	unsigned __int64 result = 0;

	// initial msg permute
	unsigned __int64 work = 0;
	int j;
	for (j = 0; j < 64; j++) {
		unsigned char whichPlace = INITIAL_P[j];
		unsigned char bit = bitUtils.checkBitFromLeft(encryptedMsg, whichPlace-1);
		if (bit) {
			work = bitUtils.setBitFromLeft(work, j);
		}
	}

	// split msg in half
	unsigned __int32 rights[16];
	rights[0]= work;

	unsigned __int32 lefts[16];
	lefts[0] = work >> 32;

	// function
	int i;
	for (i = 1; i < 16; i++) {
		lefts[i] = rights[i-1];
		rights[i] = (lefts[i-1] ^ (efunc(rights[i-1], keys[16-i])));
	}

	// use the final iteration	
	result = (result | lefts[15]);
	result = result << 32;
	result = result | rights[15];
	
	// final permutation of message
	unsigned __int64 result2 = 0;
	int k;
	for (k = 0; k < 64; k++) {
		unsigned char whichPlace = FINAL_P[k];
		unsigned char bit = bitUtils.checkBitFromLeft(result, whichPlace-1);
		if (bit) {
			result2 = bitUtils.setBitFromLeft(result2, k);
		}
	}

	result2 = bitUtils.toggleEndian(result2);
	return result2;
}

DESEncrypter::DESEncrypter(unsigned __int64 key) {
	printf("CONSTR KEY: %llx\n", key);
	//originalKey = bitUtils.toggleEndian(key);
	createSubkeys(key);
	int i;
	for (i = 0; i < 16; i++) {
		printf("final key %d = %llx\n", i, keys[i]);
	}
}


DESEncrypter::~DESEncrypter(void) {
}