#pragma once
class BitUtils
{
public:
	__int64 toggleEndian(unsigned __int64);
	__int64 setBit(unsigned __int64, int);
	__int64 setBitFromLeft(unsigned __int64, int);
	__int64 unsetBit(unsigned __int64, int);
	__int64 unsetBitFromLeft(unsigned __int64, int);
	__int64 toggleBitFromLeft(unsigned __int64, int);
	char checkBitFromRight(unsigned __int64, int);
	char checkBitFromLeft(unsigned __int64, int);
	BitUtils(void);
	~BitUtils(void);
};
