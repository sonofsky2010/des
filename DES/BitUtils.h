#pragma once
class BitUtils
{
public:
	unsigned __int64 toggleEndian(unsigned __int64);
	unsigned __int64 setBitFromRight(unsigned __int64, int);
	unsigned __int64 setBitFromLeft(unsigned __int64, int);
	unsigned __int64 unsetBit(unsigned __int64, int);
	unsigned __int64 unsetBitFromLeft(unsigned __int64, int);
	unsigned __int64 toggleBitFromLeft(unsigned __int64, int);
	unsigned __int64 rollLeft(unsigned __int64, int);
	unsigned __int32 rollLeft28(unsigned __int32, int);
	char checkBitFromRight(unsigned __int64, int);
	char checkBitFromLeft(unsigned __int64, int);
	BitUtils(void);
	~BitUtils(void);
};
