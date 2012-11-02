#include "stdafx.h"
#include "BitUtils.h"

/* converts from little-endian to big-endian */
unsigned __int64 BitUtils::toggleEndian(unsigned __int64 data) {
	return 
		  ((data >> 56)     |
          ((data<<40) & 0x00FF000000000000) |
          ((data<<24) & 0x0000FF0000000000) |
          ((data<<8)  & 0x000000FF00000000) |
          ((data>>8)  & 0x00000000FF000000) |
          ((data>>24) & 0x0000000000FF0000) |
          ((data>>40) & 0x000000000000FF00) |
		  (data << 56) ); 
}

unsigned __int64 BitUtils::setBitFromRight(unsigned __int64 data, int place) {
	return (data | (1i64 << place));
}

/* sets the bit (1) in 'place' in the given byte */
unsigned __int64 BitUtils::setBitFromLeft(unsigned __int64 data, int place) {
	return (data | (1i64<<(63-place)));
}

/* clears the bit (0) in 'place' in the given byte */
unsigned __int64 BitUtils::unsetBitFromLeft(unsigned __int64 data, int place) {
	return (data & ~(1i64<<(63-place)));
}

/* toggles the bit in 'place' in the given byte */
unsigned __int64 BitUtils::toggleBitFromLeft(unsigned __int64 data, int place) {
	return (data ^ (1i64<<(63-place)));
}

/* returns 0 if the bit in 'place' in the given byte is 0, otherwise it returns
 * >0 and the bit is a 1 
 */
char BitUtils::checkBitFromLeft(unsigned __int64 data, int place) {
	return ( 1 & (data >> (63-place)) );
}

char BitUtils::checkBitFromRight(unsigned __int64 data, int place) {
	return (1 & (data >> place));
}

unsigned __int32 BitUtils::rollLeft28(unsigned __int32 data, int amount) {	
	return (data >> amount | data << (28-amount));
}

BitUtils::BitUtils(void)
{
}


BitUtils::~BitUtils(void)
{
}