#include "stdafx.h"
#include "tools.h"
#include <crypto/FLMD5.h>

void MakeTutorialStateString( char * szOut, size_t cchQut, int nState, const char* szKey )
{
	char szIn[128]	= { 0,};
	FLSPrintf( szIn, _countof( szIn ), "%s%d", szKey, nState );
	md5( szOut, cchQut, szIn );
}