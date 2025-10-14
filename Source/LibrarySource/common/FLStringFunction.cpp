
#include "FLStringFunction.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <functional>


//////////////////////////////////////////////////////////////////////////
int		FLSPrintf( char* szDest, size_t cchDest, const char* szFormat, ... )
{
	va_list args;
	va_start( args, szFormat );

	int nRet = FLVSPrintf( szDest, cchDest, szFormat, args );

	va_end( args );

	return nRet;
}

int		FLVSPrintf( char* szDest, size_t cchDest, const char* szFormat, va_list args )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szFormat != NULL );

	int nRet = -1;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szFormat == NULL )
		{
			szDest[0] = '\0';
			nRet = 0;
		}
		else
		{
			char* szEnd = NULL;
			size_t nRemaining = 0;

			HRESULT hResult = ::StringCchVPrintfExA( szDest, cchDest, &szEnd, &nRemaining, 0, szFormat, args );
//			HRESULT hResult = ::StringCchVPrintfA( szDest, cchDest, szFormat, args );

			FLASSERT( SUCCEEDED( hResult ) );
			FLASSERT( cchDest >= nRemaining );

			if( SUCCEEDED( hResult ) && cchDest >= nRemaining )
			{
				nRet = (int)( cchDest - nRemaining );
			}
		}
	}

	return nRet;
}

char*	FLStrcpy( char* szDest, size_t cchDest, const char* szSrc )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	char* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL )
		{
			szDest[0] = '\0';
			szEnd = &szDest[0];
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCopyExA( szDest, cchDest, szSrc, &szEnd, &nRemaining, 0 );
//			::StringCchCopyA( szDest, cchDest, szSrc );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

char*	FLStrncpy( char* szDest, size_t cchDest, const char* szSrc, size_t cchSrc )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	char* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL || cchSrc <= 0 )
		{
			szDest[0] = '\0';
			szEnd = &szDest[0];
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCopyNExA( szDest, cchDest, szSrc, cchSrc, &szEnd, &nRemaining, 0 );
//			::StringCchCopyNA( szDest, cchDest, szSrc, cchSrc );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

char*	FLStrcat( char* szDest, size_t cchDest, const char* szSrc )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	char* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL )
		{
			szEnd = szDest + FLStrlen( szDest, cchDest );
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCatExA( szDest, cchDest, szSrc, &szEnd, &nRemaining, 0 );
//			::StringCchCatA( szDest, cchDest, szSrc );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

char*	FLStrncat( char* szDest, size_t cchDest, const char* szSrc, size_t cchAppend )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	char* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL || cchAppend <= 0 )
		{
			szEnd = szDest + FLStrlen( szDest, cchDest );
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCatNExA( szDest, cchDest, szSrc, cchAppend, &szEnd, &nRemaining, 0 );
//			::StringCchCatNA( szDest, cchDest, szSrc, cchAppend );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

size_t	FLStrlen( const char* szString, size_t cchString )
{
	FLASSERT( szString != NULL );
	FLASSERT( cchString > 0 );

	size_t nCount = 0;
	if( szString != NULL && cchString > 0 )
	{
#ifdef	_DEBUG
		HRESULT hResult =
#endif

		::StringCchLengthA( szString, cchString, &nCount );

#ifdef	_DEBUG
		FLASSERT( SUCCEEDED( hResult ) );
#endif
	}

	return nCount;
}

int		FLStrConvertChars( char* szString, const char* szOld, const char* szNew )
{
	FLASSERT( szString != NULL );
	FLASSERT( szOld != NULL );
	FLASSERT( szNew != NULL );

	FLASSERT( ::strlen( szOld ) == ::strlen( szNew ) );

	int nReplaceCount = 0;
	if( szString != NULL && szOld != NULL && szNew != NULL )
	{
		size_t nNewLen = ::strlen( szNew );
		for( ; *szString != '\0'; ++szString )
		{
			const char* pFind = ::strchr( szOld, *szString );
			if( pFind != NULL )
			{
				ptrdiff_t idx = pFind - szOld;
				if( 0 <= idx && static_cast< size_t >( idx ) < nNewLen )
				{
					*szString = szNew[idx];
					++nReplaceCount;
				}
			}
		}
	}

	return nReplaceCount;
}

void	FLStrLowercase( char* szString, size_t cchString )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_strlwr_s( szString, cchString ) == 0 );
#else
		::_strlwr( szString );
#endif
	}
}

void	FLStrUppercase( char* szString, size_t cchString )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_strupr_s( szString, cchString ) == 0 );
#else
		::_strupr( szString );
#endif
	}
}

void	FLIntToString( int nValue, char* szString, size_t cchString, int nRadix )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_itoa_s( nValue, szString, cchString, nRadix ) == 0 );
#else
		::_itoa( nValue, szString, nRadix );
#endif
	}
}

void	FLULongToString( unsigned long nValue, char* szString, size_t cchString, int nRadix )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_ultoa_s( nValue, szString, cchString, nRadix ) == 0 );
#else
		::_ultoa( nValue, szString, nRadix );
#endif
	}
}

void	FLInt64ToString( __int64 n64Value, char* szString, size_t cchString, int nRadix )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_i64toa_s( n64Value, szString, cchString, nRadix ) == 0 );
#else
		::_i64toa( n64Value, szString, nRadix );
#endif
	}
}

void	FLTimeToString( char* szString, size_t cchString, const __time32_t* pTime )
{
	FLASSERT( szString != NULL );
	FLASSERT( pTime != NULL );
	if( szString != NULL && cchString > 0 && pTime != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_ctime32_s( szString, cchString, pTime ) == 0 );
#else
		FLStrcpy( szString, cchString, ::ctime( pTime ) );
#endif
	}
}

void	FLTimeToString( char* szString, size_t cchString, const __time64_t* pTime )
{
	FLASSERT( szString != NULL );
	FLASSERT( pTime != NULL );
	if( szString != NULL && cchString > 0 && pTime != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_ctime64_s( szString, cchString, pTime ) == 0 );
#else
		FLStrcpy( szString, cchString, ::_ctime64( pTime ) );
#endif
	}
}


void	FLSystemTimeToDBDateTimeString( const SYSTEMTIME* pSystemTime, char* szOutput, size_t cchOutput )
{
	FLASSERT( pSystemTime != NULL );
	FLASSERT( szOutput != NULL );
	FLASSERT( cchOutput > 0 );
	if( pSystemTime != NULL && szOutput != NULL && cchOutput > 0 )
	{
		static const char szDBTimeFormat[] = "%04d-%02d-%02d %02d:%02d:%02d";

		FLSPrintf( szOutput, cchOutput, szDBTimeFormat,
			pSystemTime->wYear, pSystemTime->wMonth, pSystemTime->wDay,
			pSystemTime->wHour, pSystemTime->wMinute, pSystemTime->wSecond );
	}
}

void	FLEnumStringsIntoCharBuffer( const std::vector< std::string >& vecStrings, char* szOutput, size_t cchOutput, const char* szMarker )
{
	FLASSERT( szOutput != NULL );
	FLASSERT( cchOutput > 0 );
	if( szOutput != NULL && cchOutput > 0 )
	{
		if( !vecStrings.empty() )
		{
			std::vector< std::string >::const_iterator pos = vecStrings.begin();
			std::vector< std::string >::const_iterator end = vecStrings.end();
			const std::string& strEnd = vecStrings.back();

			while( pos != end )
			{
				const std::string& strName = (*pos);
				FLStrcat( szOutput, cchOutput, strName.c_str() );

				if( *pos != strEnd && szMarker != NULL )
				{
					FLStrcat( szOutput, cchOutput, szMarker );
				}

				++pos;
			}
		}
	}
}

void	FLEnumStringsIntoString( const std::vector< std::string >& vecStrings, std::string& strOutput, const char* szMaker )
{
	if( !vecStrings.empty() )
	{
		std::vector< std::string >::const_iterator pos = vecStrings.begin();
		std::vector< std::string >::const_iterator end = vecStrings.end();
		const std::string& strEnd = vecStrings.back();

		while( pos != end )
		{
			const std::string& strName = (*pos);
			strOutput += strName;

			if( *pos != strEnd && szMaker != NULL )
			{
				strOutput += szMaker;
			}

			++pos;
		}
	}
}

void	FLSplitPath( const char* szPath,
					 char* szDrive, size_t cchDrive,
					 char* szDir, size_t cchDir,
					 char* szFileName, size_t cchFileName,
					 char* szExt, size_t cchExt )
{
	FLASSERT( szPath != NULL );
	if( szPath != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_splitpath_s( szPath, szDrive, cchDrive, szDir, cchDir, szFileName, cchFileName, szExt, cchExt ) == 0 );
#else
		(cchDrive);
		(cchDir);
		(cchFileName);
		(cchExt);
		::_splitpath( szPath, szDrive, szDir, szFileName, szExt );
#endif
	}
}

int		FLMultiByteToWideChar( UINT dwCodePage, const char* szMultiByteString, int nMultiBytes, wchar_t* szWideCharString, int cchWideChar )
{
	FLASSERT( szMultiByteString != NULL );

	int nResult = 0;
	if( szMultiByteString != NULL )
	{
		nResult = ::MultiByteToWideChar( dwCodePage, 0, szMultiByteString, nMultiBytes, szWideCharString, cchWideChar );
	}

	FLASSERT( nResult > 0 );
	return nResult;
}

wchar_t*	FLMultiByteToWideCharWithAlloc( UINT dwCodePage, const char* szMultiByteString, int nMultiBytes )
{
	FLASSERT( szMultiByteString != NULL );

	wchar_t* pWideChar = NULL;
	if( szMultiByteString != NULL )
	{
		int nLen = FLMultiByteToWideChar( dwCodePage, szMultiByteString, nMultiBytes, NULL, 0 );
		if( nLen > 0 )
		{
			pWideChar = new wchar_t[nLen];
			if( pWideChar != NULL )
			{
				if( FLMultiByteToWideChar( dwCodePage, szMultiByteString, nMultiBytes, pWideChar, nLen ) <= 0 )
				{
					delete [] pWideChar;
					pWideChar = NULL;
				}
			}
		}
	}

	FLASSERT( pWideChar != NULL );
	return pWideChar;
}

char*	FLTrimLeft( char* szString, size_t cchString )
{
	if( szString && cchString )
	{
		const char* pBuf = szString;
		const char* pEnd = szString + cchString;
		while( pBuf != pEnd && *pBuf && ::isspace( *pBuf ) )
		{
			++pBuf;
		}

		if( szString != pBuf && pBuf != pEnd )
		{
			size_t nLen = FLStrlen( szString, cchString );
			nLen -= (pBuf - szString);
			::memmove( szString, pBuf, nLen*sizeof( char ) );
			szString[nLen] = '\0';
		}
	}

	return szString;
}

char*	FLTrimRight( char* szString, size_t cchString )
{
	if( szString && cchString )
	{
		size_t nLen = FLStrlen( szString, cchString );
		if( nLen < cchString )
		{
			while( nLen && ::isspace( szString[nLen-1] ) )
			{
				--nLen;
			}

			szString[nLen] = '\0';
		}
	}

	return szString;
}

char*	FLTrim( char* szString, size_t cchString )
{
	return FLTrimLeft( FLTrimRight( szString, cchString ), cchString );
}

std::string	FLTrimLeft(const std::string& strString)
{
	std::string cpy = strString;
	cpy.erase(cpy.begin(), std::find_if(cpy.begin(), cpy.end(), [](int c) {return !std::isspace(c); }));
	return cpy;
}


std::string	FLTrimRight(const std::string& strString)
{
	std::string s = strString;
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
	return s;

}

std::string	FLTrim( const std::string& strString )
{
	return FLTrimLeft( FLTrimRight( strString ) );
}

void	FLTokenize( const std::string& strString, std::vector< std::string >& vecTokens, const std::string& strDelimiters )
{
	std::string::size_type nPos = 0;
	std::string::size_type nStart = 0;

	do
	{
		nPos = strString.find_first_of( strDelimiters, nStart );

		std::string::size_type nLen = 0;
		if( nPos == std::string::npos )
		{
			nLen = nPos;
		}
		else
		{
			nLen = nPos - nStart;
		}

		vecTokens.push_back( strString.substr( nStart, nLen ) );
		nStart += (nLen + 1);

	} while( nPos != std::string::npos );
}

//////////////////////////////////////////////////////////////////////////

int		FLSPrintf( wchar_t* szDest, size_t cchDest, const wchar_t* szFormat, ... )
{
	va_list args;
	va_start( args, szFormat );

	int nRet = FLVSPrintf( szDest, cchDest, szFormat, args );

	va_end( args );

	return nRet;
}

int		FLVSPrintf( wchar_t* szDest, size_t cchDest, const wchar_t* szFormat, va_list args )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szFormat != NULL );

	int nRet = -1;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szFormat == NULL )
		{
			szDest[0] = L'\0';
			nRet = 0;
		}
		else
		{
			wchar_t* szEnd = NULL;
			size_t nRemaining = 0;

			HRESULT hResult = ::StringCchVPrintfExW( szDest, cchDest, &szEnd, &nRemaining, 0, szFormat, args );
//			HRESULT hResult = ::StringCchVPrintfW( szDest, cchDest, szFormat, args );

			FLASSERT( SUCCEEDED( hResult ) );
			FLASSERT( cchDest >= nRemaining );

			if( SUCCEEDED( hResult ) && cchDest >= nRemaining )
			{
				nRet = int( cchDest - nRemaining );
			}
		}
	}

	return nRet;
}

wchar_t*	FLStrcpy( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	wchar_t* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL )
		{
			szDest[0] = L'\0';
			szEnd = &szDest[0];
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCopyExW( szDest, cchDest, szSrc, &szEnd, &nRemaining, 0 );
//			::StringCchCopyW( szDest, cchDest, szSrc );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

wchar_t*	FLStrncpy( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc, size_t cchSrc )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	wchar_t* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL || cchSrc <= 0 )
		{
			szDest[0] = L'\0';
			szEnd = &szDest[0];
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCopyNExW( szDest, cchDest, szSrc, cchSrc, &szEnd, &nRemaining, 0 );
//			::StringCchCopyNW( szDest, cchDest, szSrc, cchSrc );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

wchar_t*	FLStrcat( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	wchar_t* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL )
		{
			szEnd = szDest + FLStrlen( szDest, cchDest );
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCatExW( szDest, cchDest, szSrc, &szEnd, &nRemaining, 0 );
//			::StringCchCatW( szDest, cchDest, szSrc );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

wchar_t*	FLStrncat( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc, size_t cchAppend )
{
	FLASSERT( szDest != NULL );
	FLASSERT( cchDest > 0 );
	FLASSERT( szSrc != NULL );

	wchar_t* szEnd = NULL;
	if( szDest != NULL && cchDest > 0 )
	{
		if( szSrc == NULL || cchAppend <= 0 )
		{
			szEnd = szDest + FLStrlen( szDest, cchDest );
		}
		else
		{
			size_t nRemaining = 0;

#ifdef	_DEBUG
			HRESULT hResult =
#endif

			::StringCchCatNExW( szDest, cchDest, szSrc, cchAppend, &szEnd, &nRemaining, 0 );
//			::StringCchCatNW( szDest, cchDest, szSrc, cchAppend );

#ifdef	_DEBUG
			FLASSERT( SUCCEEDED( hResult ) );
#endif
		}
	}

	return szEnd;
}

size_t	FLStrlen( const wchar_t* szString, size_t cchString )
{
	FLASSERT( szString != NULL );
	FLASSERT( cchString > 0 );

	size_t nCount = 0;
	if( szString != NULL && cchString > 0 )
	{
#ifdef	_DEBUG
		HRESULT hResult =
#endif

		::StringCchLengthW( szString, cchString, &nCount );

#ifdef	_DEBUG
		FLASSERT( SUCCEEDED( hResult ) );
#endif
	}

	return nCount;
}

int		FLStrConvertChars( wchar_t* szString, const wchar_t* szOld, const wchar_t* szNew )
{
	FLASSERT( szString != NULL );
	FLASSERT( szOld != NULL );
	FLASSERT( szNew != NULL );

	FLASSERT( ::wcslen( szOld ) == ::wcslen( szNew ) );

	int nReplaceCount = 0;
	if( szString != NULL && szOld != NULL && szNew != NULL )
	{
		size_t nNewLen = ::wcslen( szNew );
		for( ; *szString != L'\0'; ++szString )
		{
			const wchar_t* pFind = ::wcschr( szOld, *szString );
			if( pFind != NULL )
			{
				ptrdiff_t idx = pFind - szOld;
				if( 0 <= idx && static_cast< size_t >( idx ) < nNewLen )
				{
					*szString = szNew[idx];
					++nReplaceCount;
				}
			}
		}
	}

	return nReplaceCount;
}

void	FLStrLowercase( wchar_t* szString, size_t cchString )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_wcslwr_s( szString, cchString ) == 0 );
#else
		::_wcslwr( szString );
#endif
	}
}

void	FLStrUppercase( wchar_t* szString, size_t cchString )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_wcsupr_s( szString, cchString ) == 0 );
#else
		::_wcsupr( szString );
#endif
	}
}

void	FLIntToString( int nValue, wchar_t* szString, size_t cchString, int nRadix )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_itow_s( nValue, szString, cchString, nRadix ) == 0 );
#else
		::_itow( nValue, szString, nRadix );
#endif
	}
}

void	FLULongToString( unsigned long nValue, wchar_t* szString, size_t cchString, int nRadix )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_ultow_s( nValue, szString, cchString, nRadix ) == 0 );
#else
		::_ultow( nValue, szString, nRadix );
#endif
	}
}

void	FLInt64ToString( __int64 n64Value, wchar_t* szString, size_t cchString, int nRadix )
{
	FLASSERT( szString != NULL );
	if( szString != NULL && cchString > 0 )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_i64tow_s( n64Value, szString, cchString, nRadix ) == 0 );
#else
		::_i64tow( n64Value, szString, nRadix );
#endif
	}
}

void	FLTimeToString( wchar_t* szString, size_t cchString, const __time32_t* pTime )
{
	FLASSERT( szString != NULL );
	FLASSERT( pTime != NULL );
	if( szString != NULL && cchString > 0 && pTime != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_wctime32_s( szString, cchString, pTime ) == 0 );
#else
		FLStrcpy( szString, cchString, ::_wctime( pTime ) );
#endif
	}
}

void	FLTimeToString( wchar_t* szString, size_t cchString, const __time64_t* pTime )
{
	FLASSERT( szString != NULL );
	FLASSERT( pTime != NULL );
	if( szString != NULL && cchString > 0 && pTime != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_wctime64_s( szString, cchString, pTime ) == 0 );
#else
		FLStrcpy( szString, cchString, ::_wctime64( pTime ) );
#endif
	}
}

void	FLSystemTimeToDBDateTimeString( const SYSTEMTIME* pSystemTime, wchar_t* szOutput, size_t cchOutput )
{
	FLASSERT( pSystemTime != NULL );
	FLASSERT( szOutput != NULL );
	FLASSERT( cchOutput > 0 );
	if( pSystemTime != NULL && szOutput != NULL && cchOutput > 0 )
	{
		static const wchar_t szDBTimeFormat[] = L"%04d-%02d-%02d %02d:%02d:%02d";

		FLSPrintf( szOutput, cchOutput, szDBTimeFormat,
			pSystemTime->wYear, pSystemTime->wMonth, pSystemTime->wDay,
			pSystemTime->wHour, pSystemTime->wMinute, pSystemTime->wSecond );
	}
}

void	FLEnumStringsIntoCharBuffer( const std::vector< std::wstring >& vecStrings, wchar_t* szOutput, size_t cchOutput, const wchar_t* szMarker )
{
	FLASSERT( szOutput != NULL );
	FLASSERT( cchOutput > 0 );
	if( szOutput != NULL && cchOutput > 0 )
	{
		if( !vecStrings.empty() )
		{
			std::vector< std::wstring >::const_iterator pos = vecStrings.begin();
			std::vector< std::wstring >::const_iterator end = vecStrings.end();
			const std::wstring& strEnd = vecStrings.back();

			while( pos != end )
			{
				const std::wstring& strName = (*pos);
				FLStrcat( szOutput, cchOutput, strName.c_str() );

				if( *pos != strEnd && szMarker != NULL )
				{
					FLStrcat( szOutput, cchOutput, szMarker );
				}

				++pos;
			}
		}
	}
}

void	FLEnumStringsIntoString( const std::vector< std::wstring >& vecStrings, std::wstring& strOutput, const wchar_t* szMaker )
{
	if( !vecStrings.empty() )
	{
		std::vector< std::wstring >::const_iterator pos = vecStrings.begin();
		std::vector< std::wstring >::const_iterator end = vecStrings.end();
		const std::wstring& strEnd = vecStrings.back();

		while( pos != end )
		{
			const std::wstring& strName = (*pos);
			strOutput += strName;

			if( *pos != strEnd && szMaker != NULL )
			{
				strOutput += szMaker;
			}

			++pos;
		}
	}
}

void	FLSplitPath( const wchar_t* szPath,
					 wchar_t* szDrive, size_t cchDrive,
					 wchar_t* szDir, size_t cchDir,
					 wchar_t* szFileName, size_t cchFileName,
					 wchar_t* szExt, size_t cchExt )
{
	FLASSERT( szPath != NULL );
	if( szPath != NULL )
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		FLVERIFY( ::_wsplitpath_s( szPath, szDrive, cchDrive, szDir, cchDir, szFileName, cchFileName, szExt, cchExt ) == 0 );
#else
		(cchDrive);
		(cchDir);
		(cchFileName);
		(cchExt);
		::_wsplitpath( szPath, szDrive, szDir, szFileName, szExt );
#endif
	}
}

int		FLWideCharToMultByte( UINT dwCodePage, const wchar_t* szWideCharString, int cchWideChar, char* szMultiByteString, int nMultBytes )
{
	FLASSERT( szWideCharString != NULL );

	int nResult = 0;
	if( szWideCharString != NULL )
	{
		nResult = ::WideCharToMultiByte( dwCodePage, 0, szWideCharString, cchWideChar, szMultiByteString, nMultBytes, NULL, NULL );
	}

	FLASSERT( nResult > 0 );
	return nResult;
}

char*	FLMultiByteToWideCharWithAlloc( UINT dwCodePage, const wchar_t* szWideCharString, int cchWideChar )
{
	FLASSERT( szWideCharString != NULL );

	char* pMultiChar = NULL;
	if( szWideCharString != NULL )
	{
		int nLen = FLWideCharToMultByte( dwCodePage, szWideCharString, cchWideChar, NULL, 0 );
		if( nLen > 0 )
		{
			pMultiChar = new char[nLen];
			if( pMultiChar != NULL )
			{
				if( FLWideCharToMultByte( dwCodePage, szWideCharString, cchWideChar, pMultiChar, nLen ) <= 0 )
				{
					delete [] pMultiChar;
					pMultiChar = NULL;
				}
			}
		}
	}

	FLASSERT( pMultiChar != NULL );
	return pMultiChar;
}

wchar_t*	FLTrimLeft( wchar_t* szString, size_t cchString )
{
	if( szString && cchString )
	{
		const wchar_t* pBuf = szString;
		const wchar_t* pEnd = szString + cchString;
		while( pBuf != pEnd && *pBuf && ::isspace( *pBuf ) )
		{
			++pBuf;
		}

		if( szString != pBuf && pBuf != pEnd )
		{
			size_t nLen = FLStrlen( szString, cchString );
			nLen -= (pBuf - szString);
			::memmove( szString, pBuf, nLen*sizeof( wchar_t ) );
			szString[nLen] = L'\0';
		}
	}

	return szString;
}

wchar_t*	FLTrimRight( wchar_t* szString, size_t cchString )
{
	if( szString && cchString )
	{
		size_t nLen = FLStrlen( szString, cchString );
		if( nLen < cchString )
		{
			while( nLen && ::iswspace( szString[nLen-1] ) )
			{
				--nLen;
			}

			szString[nLen] = L'\0';
		}
	}

	return szString;
}

wchar_t*	FLTrim( wchar_t* szString, size_t cchString )
{
	return FLTrimLeft( FLTrimRight( szString, cchString ), cchString );
}

std::wstring	FLTrimLeft( const std::wstring& strString )
{
	std::wstring cpy = strString;
	cpy.erase(cpy.begin(), std::find_if(cpy.begin(), cpy.end(), [](int c) {return !::iswspace(c); }));
	return cpy;
}

std::wstring	FLTrimRight( const std::wstring& strString )
{
	std::wstring s = strString;
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !::iswspace(ch); }).base(), s.end());
	return s;
}

std::wstring	FLTrim( const std::wstring& strString )
{
	return FLTrimLeft( FLTrimRight( strString ) );
}

void	FLTokenize( const std::wstring& strString, std::vector< std::wstring >& vecTokens, const std::wstring& strDelimiters )
{
	std::wstring::size_type nPos = 0;
	std::wstring::size_type nStart = 0;

	do
	{
		nPos = strString.find_first_of( strDelimiters, nStart );

		std::wstring::size_type nLen = 0;
		if( nPos == std::wstring::npos )
		{
			nLen = nPos;
		}
		else
		{
			nLen = nPos - nStart;
		}

		vecTokens.push_back( strString.substr( nStart, nLen ) );
		nStart += (nLen + 1);

	} while( nPos != std::wstring::npos );
}
