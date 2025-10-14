#include "StdAfx.h"
#include "FLKeyGenerator.h"


FLKeyGenerator::FLKeyGenerator()
{
	memset( m_szHeadKey, 0, sizeof(m_szHeadKey) );
	memset( m_szTailKey, 0, sizeof(m_szTailKey) );

	memset( m_szKey, 0, sizeof(m_szKey) );
}

FLKeyGenerator::~FLKeyGenerator()
{
}

FLKeyGenerator* FLKeyGenerator::GetInstance()
{
	static FLKeyGenerator xKeyGenerator;

	return	&	xKeyGenerator;
}

TCHAR	FLKeyGenerator::GetNumeral( WORD wData )
{
	TCHAR	szStartLetter	= _T( '0' );
	TCHAR	szEndLetter		= _T( '9' );

	int		nDiff	= szEndLetter - szStartLetter + 1;

	TCHAR	szLetter;
	szLetter	= static_cast< TCHAR >( szStartLetter + ( wData % nDiff ) );

	return	szLetter;
}

TCHAR	FLKeyGenerator::GetSmallLetter( WORD wData )
{
	TCHAR	szStartLetter	= _T( 'a' );
	TCHAR	szEndLetter		= _T( 'z' );

	int		nDiff	= szEndLetter - szStartLetter + 1;

	TCHAR	szLetter;
	szLetter	= static_cast< TCHAR >( szStartLetter + ( wData % nDiff ) );

	return	szLetter;
}

TCHAR	FLKeyGenerator::GetCapitalLetter( WORD wData )
{
	TCHAR	szStartLetter	= _T( 'A' );
	TCHAR	szEndLetter		= _T( 'Z' );

	int		nDiff	= szEndLetter - szStartLetter + 1;

	TCHAR	szLetter;
	szLetter	= static_cast< TCHAR >( szStartLetter + ( wData % nDiff ) );

	return	szLetter;
}

TCHAR	FLKeyGenerator::GetSpecificFirst( WORD wData )
{
	TCHAR	szStartLetter	= _T( '!' );
	TCHAR	szEndLetter		= _T( '/' );

	int		nDiff	= szEndLetter - szStartLetter + 1;

	TCHAR	szLetter;
	szLetter	= static_cast< TCHAR >( szStartLetter + ( wData % nDiff ) );

	return	szLetter;
}

TCHAR	FLKeyGenerator::GetSpecificSecond( WORD wData )
{
	TCHAR	szStartLetter	= _T( ':' );
	TCHAR	szEndLetter		= _T( '@' );

	int		nDiff	= szEndLetter - szStartLetter + 1;

	TCHAR	szLetter;
	szLetter	= static_cast< TCHAR >( szStartLetter + ( wData % nDiff ) );

	return	szLetter;
}

bool	FLKeyGenerator::SetHeadKey( TCHAR* pHeadKey )
{
	int	nLength	= 0;
	nLength	= _tcslen( pHeadKey );

	if( nLength == 0 )
	{
		return	false;;
	}


	WORD	wNumberData	= 0;

	for( int i=0; i<nLength; ++i )
	{
		wNumberData	= wNumberData + (WORD)( pHeadKey[i] );
	}


	if( sizeof(TCHAR) == sizeof(char) )
	{
		BYTE	byHi	= HIBYTE( wNumberData );
		BYTE	byLo	= LOBYTE( wNumberData );

		wNumberData = (WORD)( byHi + byLo );
	}

	//	BEGINTEST	우선 이렇게 사용.
	m_szHeadKey[0]	= GetSmallLetter( wNumberData );
	m_szHeadKey[1]	= GetNumeral( wNumberData );
	m_szHeadKey[2]	= GetSpecificFirst( wNumberData );
	m_szHeadKey[3]	= GetCapitalLetter( wNumberData );
	m_szHeadKey[4]	= GetSpecificSecond( wNumberData );
	//	ENDTEST	우선 이렇게 사용.

	return	true;
}

bool	FLKeyGenerator::SetTailKey( TCHAR* pTailKey )
{
	int	nLength	= 0;
	nLength	= _tcslen( pTailKey );

	if( nLength == 0 )
	{
		return	false;;
	}


	WORD	wNumberData	= 0;

	for( int i=0; i<nLength; ++i )
	{
		wNumberData	= wNumberData + (WORD)( pTailKey[i] );
	}


	if( sizeof(TCHAR) == sizeof(char) )
	{
		BYTE	byHi	= HIBYTE( wNumberData );
		BYTE	byLo	= LOBYTE( wNumberData );

		wNumberData = (WORD)( byHi + byLo );
	}

	//	BEGINTEST	우선 이렇게 사용.
	m_szTailKey[0]	= GetCapitalLetter( wNumberData );
	m_szTailKey[1]	= GetSpecificSecond( wNumberData );
	m_szTailKey[2]	= GetNumeral( wNumberData );
	m_szTailKey[3]	= GetSmallLetter( wNumberData );
	m_szTailKey[4]	= GetSpecificFirst( wNumberData );
	//	ENDTEST	우선 이렇게 사용.

	return	true;
}

bool	FLKeyGenerator::MakeFileKey( const TCHAR* pFileName )
{
	memset( m_szKey, 0, sizeof(m_szKey) );

	int	nHeadKeyLength	= 0;
	int	nTailKeyLength	= 0;

	nHeadKeyLength	= _tcslen( m_szHeadKey );
	nTailKeyLength	= _tcslen( m_szTailKey );

	if( nHeadKeyLength != (_MAX_HEAD_KEY_SIZE - 1) || nTailKeyLength != (_MAX_TAIL_KEY_SIZE - 1) )
	{
		return	false;
	}

	BYTE	byResult = FLFileKeyGenerator::FL_KEY_GENERATION_NONE;

	FLFileKeyGenerator	xKeyGenerator;
	byResult	= xKeyGenerator.FileKeyGeneration( pFileName );

	unsigned __int64	n64Key = 0;

	if( byResult == FLFileKeyGenerator::FL_KEY_GENERATION_SUCCESS )
	{
		n64Key	= xKeyGenerator.GetFileKey();
	}
	else
	{
		//	byResult is not FLFileKeyGenerator::ffl_key_generation_success
		return	false;
	}


	FLSPrintf( m_szKey, _countof( m_szKey ), "%s%020I64u%s", m_szHeadKey, n64Key, m_szTailKey );

	int	nKeyLength	= 0;
	nKeyLength	= _tcslen( m_szKey );

	if( nKeyLength != 30 )
	{
		//	nKeyLength is Error.
		return	false;
	}

	return	true;
}

TCHAR*	FLKeyGenerator::GetKey()
{
	return	m_szKey;
}
