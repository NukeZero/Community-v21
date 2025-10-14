#ifndef	__FLSTRINGFUNCTION_H__
#define	__FLSTRINGFUNCTION_H__

#include "FLCommon.h"

#pragma	warning( push )
#pragma warning( disable : 4996 )	
#include <strsafe.h>
#pragma	warning( pop )

#include <ctime>
#include <vector>


//////////////////////////////////////////////////////////////////////////
int				FLSPrintf( char* szDest, size_t cchDest, const char* szFormat, ... );
int				FLVSPrintf( char* szDest, size_t cchDest, const char* szFormat, va_list args );

char*			FLStrcpy( char* szDest, size_t cchDest, const char* szSrc );
char*			FLStrncpy( char* szDest, size_t cchDest, const char* szSrc, size_t cchSrc );

char*			FLStrcat( char* szDest, size_t cchDest, const char* szSrc );
char*			FLStrncat( char* szDest, size_t cchDest, const char* szSrc, size_t cchAppend );

size_t			FLStrlen( const char* szString, size_t cchString );

int				FLStrConvertChars( char* szString, const char* szOld, const char* szNew );	// szOld[i]¸¦ szNew[i]·Î ¹Ù²Û´Ù.

void			FLStrLowercase( char* szString, size_t cchString );
void			FLStrUppercase( char* szString, size_t cchString );

void			FLIntToString( int nValue, char* szString, size_t cchString, int nRadix );
void			FLULongToString( unsigned long nValue, char* szString, size_t cchString, int nRadix );
void			FLInt64ToString( __int64 n64Value, char* szString, size_t cchString, int nRadix );

void			FLTimeToString( char* szString, size_t cchString, const __time32_t* pTime );
void			FLTimeToString( char* szString, size_t cchString, const __time64_t* pTime );
void			FLSystemTimeToDBDateTimeString( const SYSTEMTIME* pSystemTime, char* szOutput, size_t cchOutput );

void			FLEnumStringsIntoCharBuffer( const std::vector< std::string >& vecStrings, char* szOutput, size_t cchOutput, const char* szMarker );
void			FLEnumStringsIntoString( const std::vector< std::string >& vecStrings, std::string& strOutput, const char* szMaker );

void			FLSplitPath( const char* szPath, char* szDrive, size_t cchDrive, char* szDir, size_t cchDir, char* szFileName, size_t cchFileName, char* szExt, size_t cchExt );

int				FLMultiByteToWideChar( UINT dwCodePage, const char* szMultiByteString, int nMultiBytes, wchar_t* szWideCharString, int cchWideChar );
wchar_t*		FLMultiByteToWideCharWithAlloc( UINT dwCodePage, const char* szMultiByteString, int nMultiBytes );

char*			FLTrimLeft( char* szString, size_t cchString );
char*			FLTrimRight( char* szString, size_t cchString );
char*			FLTrim( char* szString, size_t cchString );

std::string		FLTrimLeft( const std::string& strString );
std::string		FLTrimRight( const std::string& strString );
std::string		FLTrim( const std::string& strString );

void			FLTokenize( const std::string& strString, std::vector< std::string >& vecTokens, const std::string& strDelimiters );
//////////////////////////////////////////////////////////////////////////


int				FLSPrintf( wchar_t* szDest, size_t cchDest, const wchar_t* szFormat, ... );
int				FLVSPrintf( wchar_t* szDest, size_t cchDest, const wchar_t* szFormat, va_list args );

wchar_t*		FLStrcpy( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc );
wchar_t*		FLStrncpy( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc, size_t cchSrc );

wchar_t*		FLStrcat( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc );
wchar_t*		FLStrncat( wchar_t* szDest, size_t cchDest, const wchar_t* szSrc, size_t cchAppend );

size_t			FLStrlen( const wchar_t* szString, size_t cchString );

int				FLStrConvertChars( wchar_t* szString, const wchar_t* szOld, const wchar_t* szNew );	// szOld[i]¸¦ szNew[i]·Î ¹Ù²Û´Ù.

void			FLStrLowercase( wchar_t* szString, size_t cchString );
void			FLStrUppercase( wchar_t* szString, size_t cchString );

void			FLIntToString( int nValue, wchar_t* szString, size_t cchString, int nRadix );
void			FLULongToString( unsigned long nValue, wchar_t* szString, size_t cchString, int nRadix );
void			FLInt64ToString( __int64 n64Value, wchar_t* szString, size_t cchString, int nRadix );

void			FLTimeToString( wchar_t* szString, size_t cchString, const __time32_t* pTime );
void			FLTimeToString( wchar_t* szString, size_t cchString, const __time64_t* pTime );
void			FLSystemTimeToDBDateTimeString( const SYSTEMTIME* pSystemTime, wchar_t* szOutput, size_t cchOutput );

void			FLEnumStringsIntoCharBuffer( const std::vector< std::wstring >& vecStrings, wchar_t* szOutput, size_t cchOutput, const wchar_t* szMarker );
void			FLEnumStringsIntoString( const std::vector< std::wstring >& vecStrings, std::wstring& strOutput, const wchar_t* szMaker );

void			FLSplitPath( const wchar_t* szPath, wchar_t* szDrive, size_t cchDrive, wchar_t* szDir, size_t cchDir, wchar_t* szFileName, size_t cchFileName, wchar_t* szExt, size_t cchExt );

int				FLWideCharToMultByte( UINT dwCodePage, const wchar_t* szWideCharString, int cchWideChar, char* szMultiByteString, int nMultBytes );
char*			FLMultiByteToWideCharWithAlloc( UINT dwCodePage, const wchar_t* szWideCharString, int cchWideChar );

wchar_t*		FLTrimLeft( wchar_t* szString, size_t cchString );
wchar_t*		FLTrimRight( wchar_t* szString, size_t cchString );
wchar_t*		FLTrim( wchar_t* szString, size_t cchString );

std::wstring	FLTrimLeft( const std::wstring& strString );
std::wstring	FLTrimRight( const std::wstring& strString );
std::wstring	FLTrim( const std::wstring& strString );

void			FLTokenize( const std::wstring& strString, std::vector< std::wstring >& vecTokens, const std::wstring& strDelimiters );
//////////////////////////////////////////////////////////////////////////




#endif
