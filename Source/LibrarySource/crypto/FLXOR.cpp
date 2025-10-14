
#include "FLXOR.h"



void	FLXOR( BYTE& byKeyIndex, LPBYTE pOutput, LPBYTE pInput, int nSize, const unsigned char* pXORTable )
{
	for( int i = 0; i < nSize; ++i )
	{
		pOutput[i] = pInput[i] ^ pXORTable[byKeyIndex];
		++byKeyIndex;
	}
}
