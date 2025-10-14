
#include "FLMyTrace.h"
#include "../common/FLStringFunction.h"
#include "../thread/FLAutoLock.h"


//////////////////////////////////////////////////////////////////////////
DWORD	FLMyTrace::Key( const char* pszKey )
{
	char szLwr[256] = { 0, };
	FLStrcpy( szLwr, _countof( szLwr ), pszKey );
	FLStrLowercase( szLwr, _countof( szLwr ) );

	DWORD dwKey	= 0, dwNum = 0;
	for( int i	= static_cast< int >( ::strlen( szLwr ) ), j = 0; i > 0 && j < 4; j++, i-- )
	{
		dwKey	= dwKey << 8;
		dwNum	= szLwr[i-1];
		dwKey	|= dwNum;
	}

	return dwKey;
}
//////////////////////////////////////////////////////////////////////////

FLMyTrace::FLMyTrace()
{
	m_hFont	= NULL;
	m_nHead	= m_nTail	= 0;
	::memset( m_szStatusBar, 0, sizeof( m_szStatusBar ) );
}

FLMyTrace::~FLMyTrace()
{
	if( m_hFont )
	{
		::DeleteObject( m_hFont );
		m_hFont = NULL;
	}
}

void	FLMyTrace::Initialize( HWND hWnd, const TCHAR* pszFace, COLORREF cr, COLORREF crBk )
{
	m_hWnd	= hWnd;
	m_cr	= cr;
	m_crBk	= crBk;
	m_hFont	= ::CreateFont( 24, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY, VARIABLE_PITCH | FF_DONTCARE, pszFace );
}

int		FLMyTrace::FindKey( DWORD dwKey )
{
	if( m_nHead <= m_nTail )
	{
		for( int i = m_nHead; i < m_nTail; i++ )
		{
			if( m_tMyTrace[i].dwKey == dwKey )
				return i;
		}
	}
	else
	{
		for( int i = m_nHead; i < MAX_LOG; i++ )
		{
			if( m_tMyTrace[i].dwKey == dwKey )
				return i;
		}
		for( int i = 0; i < m_nTail; i++ )
		{
			if( m_tMyTrace[i].dwKey == dwKey )
				return i;
		}
	}

	return -1;
}

void	FLMyTrace::Add( DWORD dwKey, BOOL bInverse, const TCHAR* pszFormat, ... )
{
	static const DWORD dwKeyStatusBar = FLMyTrace::Key( "status" );

	va_list args;
	va_start( args, pszFormat );
	TCHAR szBuffer[512]	= { 0, };
	FLVSPrintf( szBuffer, _countof( szBuffer ), pszFormat, args );
	va_end( args );

	if( bInverse )
	{
		int nString = static_cast< int >( ::_tcslen( szBuffer ) );
		for( int i = 0; i < 160 - nString; ++i )
		{
			szBuffer[nString+i] = _T( ' ' );
		}

		szBuffer[159]	= _T( '\0' );
	}

	{
		int nIndex = 0;
		CMclAutoLock kLock( m_kAddRemoveLock );

		if( dwKey == dwKeyStatusBar )
		{
			FLStrcpy( m_szStatusBar, _countof( m_szStatusBar ), szBuffer );
		}
		else
		{
			if( dwKey == 0 || ( nIndex = FindKey( dwKey ) ) < 0 )
			{
				m_tMyTrace[m_nTail].dwKey		= dwKey;
				m_tMyTrace[m_nTail].bInverse	= bInverse;

				FLStrcpy( m_tMyTrace[m_nTail].szText, _countof( m_tMyTrace[m_nTail].szText ), szBuffer );

				m_nTail		= ( m_nTail + 1 ) % MAX_LOG;
				if( m_nTail == m_nHead )
				{
					m_nHead	= ( m_nHead + 1 ) % MAX_LOG;
				}
			}
			else
			{
				m_tMyTrace[nIndex].bInverse	= bInverse;
				FLStrcpy( m_tMyTrace[nIndex].szText, _countof( m_tMyTrace[nIndex].szText ), szBuffer );
			}
		}
	}

	::InvalidateRect( m_hWnd, NULL, TRUE );
}

void	FLMyTrace::AddLine( TCHAR ch )
{
	TCHAR szLine[160] = { 0, };
	for( int i = 0; i < _countof( szLine )-1; ++i )
	{
		szLine[i] = ch;
	}

	szLine[159]	= _T( '\0' );
	Add( 0, FALSE, szLine );
}

void	FLMyTrace::Paint( HDC hDC )
{
	static const int nYStatusBar = 14 * ( MAX_LOG - 1 ) + 5;

	HFONT hFontOld	= reinterpret_cast< HFONT >( ::SelectObject( hDC, m_hFont ) );
	::SetBkColor( hDC, m_cr );

	{
		int cy	= 0;

		CMclAutoLock kLock( m_kAddRemoveLock );
		if( m_nHead <= m_nTail )
		{
			for( int i = m_nHead; i < m_nTail; i++, cy += 14 )
			{
				TextOut( hDC, 4, cy, m_tMyTrace[i].szText, m_tMyTrace[i].bInverse );
			}
		}
		else
		{
			for( int i = m_nHead; i < MAX_LOG; i++, cy += 14 )
			{
				TextOut( hDC, 4, cy, m_tMyTrace[i].szText, m_tMyTrace[i].bInverse );
			}
			for( int i = 0; i < m_nTail; i++, cy += 14 )
			{
				TextOut( hDC, 4, cy, m_tMyTrace[i].szText, m_tMyTrace[i].bInverse );
			}
		}

		if( m_szStatusBar[0] != '\0' )
		{
			FLMyTrace::TextOut( hDC, 0, nYStatusBar-14, _T( "________________________________________________________________________________________________________________________________________________________________" ), FALSE );
			FLMyTrace::TextOut( hDC, 4, nYStatusBar, m_szStatusBar, FALSE );
		}
	}

	::SelectObject( hDC, hFontOld );
}

void	FLMyTrace::TextOut( HDC hDC, int x, int y, const TCHAR* pszString, BOOL bInverse )
{
	::SetBkMode( hDC, bInverse ? OPAQUE : TRANSPARENT );
	::SetTextColor( hDC, bInverse ? m_crBk : m_cr );

	int nString	= static_cast< int >( ::_tcslen( pszString ) );
	::TextOut( hDC, x, y, pszString, nString );
}

void	FLMyTrace::GetText( DWORD dwKey, LPTSTR pszText, size_t cchText )
{
	int nIndex	= FindKey( dwKey );
	if( nIndex >= 0 )
	{
		FLStrcpy( pszText, cchText, m_tMyTrace[nIndex].szText );
	}
}
