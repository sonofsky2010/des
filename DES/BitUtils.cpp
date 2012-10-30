#include "stdafx.h"
#include "BitUtils.h"

/* converts from little-endian to big-endian */
__int64 BitUtils::toggleEndian(unsigned __int64 data) {
	return  
		(
		((data & 0x00000000000000FF) << 56) |
        ((data & 0x000000000000FF00) << 40) |
        ((data & 0x0000000000FF0000) << 24) |
        ((data & 0x00000000FF000000) << 8)  |
        ((data & 0x000000FF00000000) >> 8)  |
        ((data & 0x0000FF0000000000) >> 24) | 
        ((data & 0x00FF000000000000) >> 40) |
        ((data & 0xFF00000000000000) >> 56)
		);
}

/* sets the bit (1) in 'place' in the given byte */
__int64 BitUtils::setBitFromLeft(unsigned __int64 data, int place) {
	return (data | 1i64<<(63-place));
}

/* clears the bit (0) in 'place' in the given byte */
__int64 BitUtils::unsetBitFromLeft(unsigned __int64 data, int place) {
	return (data & ~(1i64<<(63-place)));
}

/* toggles the bit in 'place' in the given byte */
__int64 BitUtils::toggleBitFromLeft(unsigned __int64 data, int place) {
	return (data ^ 1i64<<(63-place));
}

/* returns 0 if the bit in 'place' in the given byte is 0, otherwise it returns
 * >0 and the bit is a 1 
 */
char BitUtils::checkBitFromLeft(unsigned __int64 data, int place) {
	return (data & 1i64<<(place));
}

BitUtils::BitUtils(void)
{
}


BitUtils::~BitUtils(void)
{
}