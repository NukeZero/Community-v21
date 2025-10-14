
#include "FLMD5.h"
#include "../common/FLStringFunction.h"

#include <wincrypt.h>

void md5( char* out, size_t cchQut, const BYTE* pbContent, DWORD cbContent )
{
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTHASH hHash = NULL;
	BYTE bHash[0x7f];
	DWORD dwHashLen	= 16;	// The MD5 algorithm always returns 16 bytes.
	out[0]	= '\0';

	if( CryptAcquireContext( &hCryptProv, 
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET ) )
	{
		if( CryptCreateHash( hCryptProv,
			CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
			0, 0, &hHash ) )
		{
			if( CryptHashData( hHash, pbContent, cbContent, 0 ) )
			{
				if( CryptGetHashParam( hHash, HP_HASHVAL, bHash, &dwHashLen, 0 ) )
				{
					// Make a string version of the numeric digest value
					char tmp[4] = { 0, };
					for( int i = 0; i < 16; i++ )
					{
						FLSPrintf( tmp, _countof( tmp ), "%02x", bHash[i] );
						FLStrcat( out, cchQut, tmp );
					}
				}
			}
		}
	}

	if( hHash != NULL )
	{
		CryptDestroyHash( hHash );
	}
	if( hCryptProv != NULL )
	{
		CryptReleaseContext( hCryptProv, 0 );
	}
}

void md5( char* out, size_t cchQut, const char* in )
{
	DWORD cbContent		= static_cast< DWORD >( ::strlen( in ) );
	BYTE* pbContent	= (BYTE*)in;
	md5( out, cchQut, pbContent, cbContent );
}
