
#ifndef	__FLMD5_H__
#define	__FLMD5_H__


#include "FLCryptoCommon.h"

void	md5( char* out, size_t cchQut, const BYTE* pbContent, DWORD cbContent );
void	md5( char* out, size_t cchQut, const char* in );



#endif