#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined(_MULTI_LANGUAGE_)
//{{AFX
CEditString::CEditString() : CString( )
{
	m_nWidth = 100;
	m_sizeFont.cx = 12;
	m_sizeFont.cy = 12;
	m_pFont = NULL;
}
CEditString::CEditString( const CString& stringSrc ) : CString( stringSrc )
{
}
CEditString::CEditString( const CEditString& stringSrc ) : CString( stringSrc )
{
	m_adwColor.Append( stringSrc.m_adwColor );
	m_abyStyle.Append( stringSrc.m_abyStyle );
}
CEditString::CEditString( TCHAR ch, int nRepeat ) : CString( ch, nRepeat )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
}
CEditString::CEditString( LPCTSTR lpch, int nLength ) : CString( lpch, nLength )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
}
CEditString::CEditString( const unsigned char* psz ) : CString( psz )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
}
CEditString::CEditString( LPCWSTR lpsz ) : CString( lpsz )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
}
CEditString::CEditString( LPCSTR lpsz ) : CString( lpsz )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
}

CEditString::~CEditString()
{
}
void CEditString::SetAlpha( DWORD dwAlpha )
{
	for( int i = 0; i < GetLength(); i++)
	{
		m_adwColor.SetAt( 
			i ,
			( m_adwColor.GetAt( i ) & 0x00ffffff ) | ( dwAlpha << 24 )
		);
	}
}
void CEditString::SetColor( DWORD dwColor )
{
	m_adwColor.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_adwColor.SetAt( i , dwColor );
}
void CEditString::SetStyle( DWORD dwStyle )
{
	m_abyStyle.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_abyStyle.SetAt( i , dwStyle );
}
void CEditString::SetColor( int nPos, int nLength, DWORD dwColor )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_adwColor.SetAt( i, m_adwColor.GetAt( i ) | dwColor );
}
void CEditString::SetStyle( int nPos, int nLength, DWORD dwStyle )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_abyStyle.SetAt( i, m_abyStyle.GetAt( i ) | dwStyle );
}
void CEditString::Empty( )
{
	CString::Empty();
	m_adwLineOffset.RemoveAll();
	m_adwColor.RemoveAll();
	m_abyStyle.RemoveAll();
	m_adwLineOffset.Add( 0 );
	//m_adwColor.Add( 0 );
	//m_abyStyle.Add( 0 );
}
void CEditString::ParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, CString& string, CDWordArray& adwColor, CByteArray& abyStyle )
{	// #cffffffff // 칼라 
	// #u // underline
	// #b // bold
	DWORD dwCurColor = dwColor;
	DWORD dwCurStyle = dwStyle;
	CString strTemp;
	TCHAR szColor[9];
	int nLen = strlen( lpsz );
	for( int i = 0; i < nLen; i++ )
	{
		if( lpsz[ i ] == '#' ) // 인식 코드
		{
			if( ++i >= nLen )
				break;
			switch( lpsz[ i ] )
			{
			case 'c':
				{
					if( ++i >= nLen )
						break;
					memcpy( szColor, &lpsz[ i ], 8 );
					szColor[ 8 ] = 0;
					DWORDLONG dwlNumber = 0;
					DWORD dwMulCnt = 0;
					CHAR cVal;
					for( int j = 7; j >= 0; j--)
					{
						cVal = szColor[ j ];
						if( cVal >= 'a' )
							cVal = ( cVal - 'a' ) + 10;
						else cVal -= '0';
						dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
						dwMulCnt += 4;
					}
					dwCurColor = (DWORD)dwlNumber;
					i += 7;
				}
				break;
			case 'u':
				dwCurStyle |= ESSTY_UNDERLINE;
				break;
			case 'b':
				dwCurStyle |= ESSTY_BOLD;
				break;
#ifdef __V050322_CACHEQUEST 
			case 's':
				dwCurStyle |= ESSTY_STRIKETHROUGH;
				break;
#endif
			case 'n':
				if( ++i >= nLen )
					break;
				switch( lpsz[ i ] )
				{
				case 'c':
					dwCurColor = dwColor;
					break;
				case 'b':
					dwCurStyle &= ~ESSTY_BOLD; 
					break;
				case 'u':
					dwCurStyle &= ~ESSTY_UNDERLINE; 
					break;
#ifdef __V050322_CACHEQUEST 
				case 's':
					dwCurStyle &= ~ESSTY_STRIKETHROUGH; 
					break;
#endif
				}
				break;
			default: // 명령코드를 발견 못했을 경우 
				// #코드를 넣어준다
				strTemp += lpsz[ i - 1 ];
				adwColor.Add( dwCurColor );
				abyStyle.Add( dwCurStyle );			
				// 다음 문자를 넣어준다.
				strTemp += lpsz[ i ];
				adwColor.Add( dwCurColor );
				abyStyle.Add( dwCurStyle );			
				break;
			}
		}
		else
		{
			if( lpsz[ i ] == '\\' && lpsz[ i+1 ] == 'n' )
			{
				strTemp += '\n';
				adwColor.Add( dwCurColor );
				abyStyle.Add( dwCurStyle );
				
				i+=1;
			}
			else
			{
				strTemp += lpsz[ i ];
				adwColor.Add( dwCurColor );
				abyStyle.Add( dwCurStyle );
			}
		}
	}
	string += strTemp;
	//*((CString*)this) = string; 
	//m_adwColor.RemoveAll();
	//m_adwColor.Append( adwColor );
}
void CEditString::AddParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle )
{ 
	int nLine = GetLineCount() - 1;
	ParsingString( lpsz, dwColor, dwStyle, (CString&)*this, m_adwColor, m_abyStyle );
	/*
	int nBegin = GetLength();
	int nEnd = nBegin + _tcslen( lpsz );
	*((CString*)this) += lpsz; 
	for( int i = nBegin; i < nEnd; i++ )
		m_adwColor.SetAtGrow( i, dwColor ); 
	for( i = nBegin; i < nEnd; i++ )
		m_abyStyle.SetAtGrow( i, dwStyle ); 
		*/
	Align( m_pFont, nLine );
}
void CEditString::SetParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle )
{ 
	Empty();
	ParsingString( lpsz, dwColor, dwStyle, (CString&)*this, m_adwColor, m_abyStyle );
	//*((CString*)this) = lpsz; 
	//SetColor( dwColor );
	//SetStyle( dwStyle );
	//ParsingString( lpsz, dwColor );
	Align( m_pFont, 0 );
}
void CEditString::AddString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle )
{ 
	int nLine = GetLineCount() - 1;
	int nBegin = GetLength();
	int nEnd = nBegin + _tcslen( lpsz );
	*((CString*)this) += lpsz; 
	for( int i = nBegin; i < nEnd; i++ )
		m_adwColor.SetAtGrow( i, dwColor ); 
	for( i = nBegin; i < nEnd; i++ )
		m_abyStyle.SetAtGrow( i, dwStyle ); 
	Align( m_pFont, nLine );
}
void CEditString::AddString( char ch, DWORD dwColor, DWORD dwStyle )
{ 
	int nLine = GetLineCount() - 1;
	*((CString*)this) += ch; 
	m_adwColor.Add( dwColor ); 
	m_abyStyle.Add( dwStyle ); 
	Align( m_pFont, nLine );
}
void CEditString::SetString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle )
{ 
	Empty();
	*((CString*)this) = lpsz; 
	SetColor( dwColor );
	SetStyle( dwStyle );
	Align( m_pFont, 0 );
	//ParsingString( lpsz, dwColor );
}
void CEditString::SetString( char* lpsz, DWORD dwColor , DWORD dwStyle)
{
	Empty();
	*((CString*)this) = lpsz; 
	SetColor( dwColor );
	SetStyle( dwStyle );
	Align( m_pFont, 0 );
}
void CEditString::AddEditString( const CEditString& string )
{
	int nLine = GetLineCount() - 1;
	int nBegin = GetLength();
	int nEnd = nBegin + string.GetLength();
	*((CString*)this) += string; 
	m_adwColor.Append( string.m_adwColor );
	m_abyStyle.Append( string.m_abyStyle );
	Align( m_pFont, nLine );
}
void CEditString::SetEditString( const CEditString& string )
{
	Empty();
	*((CString*)this) = string; 
	m_adwColor.RemoveAll();
	m_adwColor.Append( string.m_adwColor );
	m_abyStyle.RemoveAll();
	m_abyStyle.Append( string.m_abyStyle );
	Align( m_pFont, 0 );
}
int CEditString::Insert( int nIndex, TCHAR ch, DWORD dwColor, DWORD dwStyle  )
{
	int nLine = GetLineCount() - 1;
	CString::Insert( nIndex, ch );
	m_adwColor.InsertAt( nIndex, dwColor );
	m_abyStyle.InsertAt( nIndex, dwStyle );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::Insert( int nIndex, LPCTSTR pstr, DWORD dwColor, DWORD dwStyle )
{
	int nLine = GetLineCount() - 1;
	CString::Insert( nIndex, pstr );
	m_adwColor.InsertAt( nIndex, dwColor, _tcslen( pstr) );
	m_abyStyle.InsertAt( nIndex, dwStyle, _tcslen( pstr) );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::Delete( int nIndex, int nCount )
{
	int nLine = GetLineCount() - 1;
	CString::Delete( nIndex, nCount );
	m_adwColor.RemoveAt( nIndex, nCount );
	m_abyStyle.RemoveAt( nIndex, nCount );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::DeleteLine( int nBeginLine, int nCount )
{
	int nMax = nBeginLine + nCount;
	int nBeginOffset, nEndOffset;
	int nLengthCount = 0;
	if( nMax > m_adwLineOffset.GetSize() )
	{
		nCount -= nMax - m_adwLineOffset.GetSize();
		nMax = m_adwLineOffset.GetSize();
	}
	for( int i = nBeginLine; i < nMax; i++ )
	{
		nBeginOffset = GetLineOffset( i );
		nEndOffset = GetLineOffset( i + 1 );
		nLengthCount += nEndOffset - nBeginOffset;
	}
	nBeginOffset = GetLineOffset( nBeginLine );
	CString::Delete( nBeginOffset, nLengthCount );
	m_adwColor.RemoveAt( nBeginOffset, nLengthCount );
	m_abyStyle.RemoveAt( nBeginOffset, nLengthCount );
	m_adwLineOffset.RemoveAt( nBeginLine, nCount ); 

	for( int j = nBeginLine; j < m_adwLineOffset.GetSize(); j++ )
	{
		int nNewOffset = m_adwLineOffset.GetAt( j ) - nLengthCount;
		m_adwLineOffset.SetAt( j, nNewOffset );
	}

	//ign( m_pFont, 0 );//nLine );
	return TRUE;
}
TCHAR CEditString::GetAt( int nIndex ) const
{
	return CString::GetAt( nIndex );
}
void CEditString::SetAt( int nIndex, TCHAR ch, DWORD dwColor, DWORD dwStyle )
{
	int nLine = GetLineCount() - 1;
	CString::SetAt( nIndex, ch );
	m_adwColor.SetAt( nIndex, dwColor );
	m_abyStyle.SetAt( nIndex, dwStyle );
	Align( m_pFont, 0 );//nLine );
}
void CEditString::Init( int nWidth, SIZE sizeFont )
{
	m_nWidth = nWidth;
	m_sizeFont = sizeFont;
}
void CEditString::Init( CD3DFont* pFont, CRect* pRect )
{
	TCHAR str[ 3 ] = _T( "가" );

	m_sizeFont = pFont->GetTextExtent(str);

	BOOL bAlign = FALSE;
	// 사이즈와 폰트 둘중에 하나라도 기존것과 다르다면 정렬을 해주어야한다.
	if( m_nWidth != pRect->Width() || m_pFont != pFont )
		bAlign = TRUE;
	m_nWidth = pRect->Width();
	m_pFont = pFont;
	if( bAlign )
		Align( m_pFont, 0 );//nLine );
}
void CEditString::Adjust( int nWidth, SIZE sizeFont )
{
	m_nWidth = nWidth;
	m_sizeFont = sizeFont;
}
void CEditString::Adjust( CD3DFont* pFont, CRect* pRect )
{
	TCHAR str[ 3 ] = _T( "가" );
	m_sizeFont = pFont->GetTextExtent(str);
	BOOL bAlign = FALSE;
	// 사이즈와 폰트 둘중에 하나라도 기존것과 다르다면 정렬을 해주어야한다.
	if( m_nWidth != pRect->Width() || m_pFont != pFont )
		bAlign = TRUE;
	m_nWidth = pRect->Width();
	m_pFont = pFont;
	if( bAlign )
		Align( m_pFont, 0 );//nLine );
}

/*
void CEditString::Reset( CD3DFont* pFont, CRect* pRect, BOOL bHScroll, BOOL bVScroll )
{
	Init( pFont, pRect );
	m_adwLineOffset.SetSize( 0 );
	Align( pFont, 0 );
}
*/
void CEditString::Align( CD3DFont* pFont, int nBeginLine )
{
	if( pFont == NULL )
		return;
//	m_adwLineOffset.SetSize( 0 );
	int nWidth  = m_nWidth;//m_sizeFont.cx;//lpRect->Width();// - 20;
	//int nHeight = pRect->Height();
	TCHAR str[ 3 ] = _T( "가" );
	if( nBeginLine < 0 ) nBeginLine = 0;
	CSize sizeString = m_sizeFont;//pFont->GetTextExtent(str);
	int nLength = GetLength();
	if( nLength == 0 || nBeginLine == 0 )
	{
		m_adwLineOffset.RemoveAll();
		if( nLength == 0 )
			return;
	}
	CString strTemp;
	int nOffset = 0;
	if( GetLineCount() )
	{
		nOffset = GetLineOffset( nBeginLine );
		//  라인옵셋 값이 이상 값이라면 
		if( nOffset < 0 || nOffset >= nLength ) 
			Error( "CEditString::Align : pos = %c, LineCount = %d, Length = %d", nOffset, GetLineCount(), nLength );
	}
	char chr = 0; 
	BOOL bNextLine = FALSE;
	BOOL bHangul   = FALSE;
	int nLineHeight = sizeString.cy;
	LPCTSTR lpszString = *this;
	do
	{
		m_adwLineOffset.SetAtGrow( nBeginLine++,  nOffset );
		strTemp = _T("");
		do 
		{
			chr = GetAt( nOffset++ );
			// 스페이스를 발견하면? (영어식 출력을 위해)
			//if(chr == ' ');
			// 엔터코드면 스킵 
/*
#ifdef __THAI// __CURRENT_LANG == LANG_THA

			int nLen = GetCharLen( &lpszString[ nOffset ]  );

			if(chr != '\r')
			{
				for( int i = 0; i < nLen; i++ )
					strTemp += lpszString[ i + nOffset ];
			}
			nOffset += nLen;

			if(chr == '\n')
				break;

			CSize sizeTemp;
			pFont->GetTextExtent( strTemp, &sizeTemp );
			if( sizeTemp.cx > nWidth ) 
			{
				if( bHangul )
					strTemp.Delete( strTemp.GetLength() - 2, 2 ), nOffset -= 2;
				else
					strTemp.Delete( strTemp.GetLength() - 1, 1 ), nOffset -= 1;
				bHangul = FALSE;
				break;
			}		
			bHangul = FALSE;
#else
*/
			if(chr != '\r')
				strTemp += chr;
			if(chr == '\n')
				break;

			// 한글 코드면 계속
			if( IsDBCSLeadByte(chr) && bHangul == FALSE )//chr & 0x80 && bHangul == FALSE) 
				bHangul = TRUE;
			else
			{
				CSize sizeTemp;
				pFont->GetTextExtent( strTemp, &sizeTemp );
				if( sizeTemp.cx > nWidth ) 
				{
					if( bHangul )
						strTemp.Delete( strTemp.GetLength() - 2, 2 ), nOffset -= 2;
					else
						strTemp.Delete( strTemp.GetLength() - 1, 1 ), nOffset -= 1;
					bHangul = FALSE;
					break;
				}		
				bHangul = FALSE;
			}
//#endif
		} while( nOffset < nLength && chr != '\0' && chr != '\n' );
		bNextLine = TRUE;
		sizeString.cy += nLineHeight;
	} 
	while( chr != '\0' && nOffset < nLength );
}

DWORD CEditString::GetLineCount() 
{
//	if( m_adwLineOffset.GetSize() == 0 )
//		return 0;
	return (DWORD) m_adwLineOffset.GetSize();//- 1;
}
DWORD CEditString::GetLineOffset( DWORD dwLine ) 
{
	if ( dwLine >= (DWORD)m_adwLineOffset.GetSize() ) 
		return GetLength();
	if ( dwLine < 0 ) return 0;
	return m_adwLineOffset[ dwLine ];
}
CString CEditString::GetLine(DWORD dwLine)
{
	CString retval = _T("");

	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin ;//- 1;
		else 
			size = GetLength() - begin;
		retval = Mid( begin, size ]); 
		//retval.Remove('\r');
		//retval.Remove('\n');
	}
	return retval;
}
CEditString CEditString::GetEditString( DWORD dwLine )
{
	CEditString retval = _T("");
	
	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin;
		else 
			size = GetLength() - begin;
		retval = Mid( begin, size ); 
		for( int i = 0; i < size; i++ )
		{
			retval.m_adwColor[ i ] = m_adwColor[ begin + i ];
			retval.m_abyStyle[ i ] = m_abyStyle[ begin + i ];
		}
	}
	return retval;
}
DWORD CEditString::OffsetToLine(DWORD dwOffset)
{
	CString retval = _T("");

	DWORD dwLineCount = GetLineCount();

	for(DWORD i = 0; i < dwLineCount; i++) 
	{
		DWORD dwBegin = GetLineOffset( i );
		DWORD dwNext = GetLineOffset( i + 1 );

		if(dwOffset >= dwBegin && dwOffset < dwNext)
			return i;
	}
	if( dwOffset == (DWORD)GetLength() )
	{
		if( dwOffset > 0 && GetAt( dwOffset - 1 ) == '\n')
			return dwLineCount;
		return dwLineCount - 1;
	}
	return 0;
}
//}}AFX
#else //_MULTI_LANGUAGE_

#define CODE_PAGE(wCodePage) ( wCodePage ? wCodePage : g_codePage )

CEditString::CEditString() : CString( )
{
	m_nWidth = 100;
	m_sizeFont.cx = 12;
	m_sizeFont.cy = 12;
	m_pFont = NULL;
}

CEditString::CEditString( const CString& stringSrc ) : CString( stringSrc )
{
}

CEditString::CEditString( const CEditString& stringSrc ) : CString( stringSrc )
{
	m_adwColor.Append( stringSrc.m_adwColor );
	m_abyStyle.Append( stringSrc.m_abyStyle );
	m_awCodePage.Append( stringSrc.m_awCodePage );
}
CEditString::CEditString( TCHAR ch, int nRepeat ) : CString( ch, nRepeat )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
	SetCodePage( 0 );
}
CEditString::CEditString( LPCTSTR lpch, int nLength ) : CString( lpch, nLength )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
	SetCodePage( 0 );
}
CEditString::CEditString( const unsigned char* psz ) : CString( psz )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
	SetCodePage( 0 );
}
CEditString::CEditString( LPCWSTR lpsz ) : CString( lpsz )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
	SetCodePage( 0 );
}
CEditString::CEditString( LPCSTR lpsz ) : CString( lpsz )
{
	SetColor( 0xffffffff );
	SetStyle( 0 );
	SetCodePage( 0 );
}
CEditString::~CEditString()
{
}
void CEditString::SetAlpha( DWORD dwAlpha )
{
	for( int i = 0; i < GetLength(); i++)
	{
		m_adwColor.SetAt( 
			i ,
			( m_adwColor.GetAt( i ) & 0x00ffffff ) | ( dwAlpha << 24 )
		);
	}
}
void CEditString::SetColor( DWORD dwColor )
{
	m_adwColor.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_adwColor.SetAt( i , dwColor );
}
void CEditString::SetStyle( DWORD dwStyle )
{
	m_abyStyle.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_abyStyle.SetAt( i , (BYTE)( dwStyle ) );
}
void CEditString::SetCodePage( WORD wCodePage )
{
	m_awCodePage.SetSize( GetLength() );
	for( int i = 0; i < GetLength(); i++)
		m_awCodePage.SetAt( i , CODE_PAGE(wCodePage) );
}
void CEditString::SetColor( int nPos, int nLength, DWORD dwColor )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_adwColor.SetAt( i, m_adwColor.GetAt( i ) | dwColor );
}
void CEditString::SetStyle( int nPos, int nLength, DWORD dwStyle )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_abyStyle.SetAt( i, (BYTE)( m_abyStyle.GetAt( i ) | dwStyle ) );
}
void CEditString::SetCodePage( int nPos, int nLength, WORD wCodePage )
{
	for( int i = nPos; i < nPos + nLength; i++)
		m_awCodePage.SetAt( i, CODE_PAGE(wCodePage) );
}
void CEditString::Empty( )
{
	CString::Empty();
	m_adwLineOffset.RemoveAll();
	m_adwColor.RemoveAll();
	m_abyStyle.RemoveAll();
	m_awCodePage.RemoveAll();
	m_adwLineOffset.Add( 0 );
	//m_adwColor.Add( 0 );
	//m_abyStyle.Add( 0 );
	//m_adwLineOffset.Add( 0 );
}
void CEditString::ParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, CString& string, CDWordArray& adwColor, CByteArray& abyStyle, CWordArray& awCodePage )
{
	ASSERT( wCodePage );
	// #cffffffff // 칼라 
	// #u // underline
	// #b // bold
	DWORD dwCurColor = dwColor;
	DWORD dwCurStyle = dwStyle;
	DWORD wCurCodePage = wCodePage;
	CString strTemp;
	TCHAR szColor[9];
	int nLen = strlen( lpsz );
	for( int i = 0; i < nLen; i++ )
	{
		if( lpsz[ i ] == '#' ) // 인식 코드
		{
			if( ++i >= nLen )
				break;
			switch( lpsz[ i ] )
			{
			case 'c':
				{
					if( ++i >= nLen )
						break;
					memcpy( szColor, &lpsz[ i ], 8 );
					szColor[ 8 ] = 0;
					DWORDLONG dwlNumber = 0;
					DWORD dwMulCnt = 0;
					CHAR cVal;
					for( int j = 7; j >= 0; j--)
					{
						cVal = szColor[ j ];
						if( cVal >= 'a' )
							cVal = ( cVal - 'a' ) + 10;
						else cVal -= '0';
						dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
						dwMulCnt += 4;
					}
					dwCurColor = (DWORD)dwlNumber;
					i += 7;
				}
				break;
			case 'u':
				dwCurStyle |= ESSTY_UNDERLINE;
				break;
			case 'b':
				dwCurStyle |= ESSTY_BOLD;
				break;
#ifdef __V050322_CACHEQUEST 
			case 's':
				dwCurStyle |= ESSTY_STRIKETHROUGH;
				break;
#endif

			case 'l':
				{
					if(++i >= nLen)
						break;
					memcpy( szColor, &lpsz[ i ], 4 );
					szColor[ 4 ] = 0;
					wCurCodePage = CODE_PAGE( atoi( szColor ) );
					i += 3;
				}				
				break;
			case 'n':
				if( ++i >= nLen )
					break;
				switch( lpsz[ i ] )
				{
				case 'c':
					dwCurColor = dwColor;
					break;
				case 'b':
					dwCurStyle &= ~ESSTY_BOLD; 
					break;
				case 'u':
					dwCurStyle &= ~ESSTY_UNDERLINE; 
					break;
#ifdef __V050322_CACHEQUEST 
				case 's':
					dwCurStyle &= ~ESSTY_STRIKETHROUGH; 
					break;
#endif
				case 'l':
					wCurCodePage = wCodePage;
					break;
				}
				break;
			default: // 명령코드를 발견 못했을 경우 
				// #코드를 넣어준다
				strTemp += lpsz[ i - 1 ];
				adwColor.Add( dwCurColor );
				abyStyle.Add( (BYTE)( dwCurStyle ) );
				awCodePage.Add( (BYTE)( wCurCodePage ) );
				// 다음 문자를 넣어준다.
				strTemp += lpsz[ i ];
				adwColor.Add( dwCurColor );
				abyStyle.Add( (BYTE)( dwCurStyle ) );			
				awCodePage.Add( (BYTE)( wCurCodePage ) );
				break;
			}
		}
		else
		{
			if( lpsz[ i ] == '\\' && lpsz[ i+1 ] == 'n' )
			{
				strTemp += '\n';
				adwColor.Add( dwCurColor );
				abyStyle.Add( (BYTE)( dwCurStyle ) );
				awCodePage.Add( (BYTE)( wCurCodePage ) );

				i+=1;
			}
			else
			{
				strTemp += lpsz[ i ];
				int nlength = strTemp.GetLength();
				adwColor.Add( dwCurColor );
				abyStyle.Add( (BYTE)( dwCurStyle ) );
				awCodePage.Add( (BYTE)( wCurCodePage ) );
			}
		}
	}
	string += strTemp;
	//*((CString*)this) = string; 
	//m_adwColor.RemoveAll();
	//m_adwColor.Append( adwColor );
}
void CEditString::AddParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	int nLine = GetLineCount() - 1;
	ParsingString( lpsz, dwColor, dwStyle, CODE_PAGE(wCodePage),(CString&)*this, m_adwColor, m_abyStyle, m_awCodePage );
	/*
	int nBegin = GetLength();
	int nEnd = nBegin + _tcslen( lpsz );
	*((CString*)this) += lpsz; 
	for( int i = nBegin; i < nEnd; i++ )
		m_adwColor.SetAtGrow( i, dwColor ); 
	for( i = nBegin; i < nEnd; i++ )
		m_abyStyle.SetAtGrow( i, dwStyle ); 
		*/
	Align( m_pFont, nLine );
}
void CEditString::SetParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	Empty();
	ParsingString( lpsz, dwColor, dwStyle, CODE_PAGE(wCodePage), (CString&)*this, m_adwColor, m_abyStyle, m_awCodePage );
	//*((CString*)this) = lpsz; 
	//SetColor( dwColor );
	//SetStyle( dwStyle );
	//ParsingString( lpsz, dwColor );
	Align( m_pFont, 0 );
}
void CEditString::AddString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	int nLine = GetLineCount() - 1;
	int nBegin = GetLength();
	int nEnd = nBegin + _tcslen( lpsz );
	*((CString*)this) += lpsz; 
	for( int i = nBegin; i < nEnd; i++ )
		m_adwColor.SetAtGrow( i, dwColor ); 
	for( int i = nBegin; i < nEnd; i++ )
		m_abyStyle.SetAtGrow( i, (BYTE)( dwStyle ) ); 
	for( int i = nBegin; i < nEnd; i++ )
		m_awCodePage.SetAtGrow( i, CODE_PAGE(wCodePage) ); 
	Align( m_pFont, nLine );
}
void CEditString::AddString( char ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	int nLine = GetLineCount() - 1;
	*((CString*)this) += ch; 
	m_adwColor.Add( dwColor ); 
	m_abyStyle.Add( (BYTE)( dwStyle ) ); 
	m_awCodePage.Add( CODE_PAGE(wCodePage) );
	Align( m_pFont, nLine );
}
void CEditString::SetString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{ 
	Empty();
	*((CString*)this) = lpsz; 
	SetColor( dwColor );
	SetStyle( dwStyle );
	SetCodePage( CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );
	//ParsingString( lpsz, dwColor );
}
void CEditString::SetString( char* lpsz, DWORD dwColor , DWORD dwStyle, WORD wCodePage )
{
	Empty();
	*((CString*)this) = lpsz; 
	SetColor( dwColor );
	SetStyle( dwStyle );
	SetCodePage( CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );
}
void CEditString::AddEditString( const CEditString& string )
{
	int nLine = GetLineCount() - 1;
	int nBegin = GetLength();
	int nEnd = nBegin + string.GetLength();
	*((CString*)this) += string; 
	m_adwColor.Append( string.m_adwColor );
	m_abyStyle.Append( string.m_abyStyle );
	m_awCodePage.Append( string.m_awCodePage );
	Align( m_pFont, nLine );
}
void CEditString::SetEditString( const CEditString& string )
{
	Empty();
	*((CString*)this) = string; 
	m_adwColor.RemoveAll();
	m_adwColor.Append( string.m_adwColor );
	m_abyStyle.RemoveAll();
	m_abyStyle.Append( string.m_abyStyle );
	m_awCodePage.RemoveAll();
	m_awCodePage.Append( string.m_awCodePage );
	Align( m_pFont, 0 );
}
int CEditString::Insert( int nIndex, TCHAR ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{
	int nLine = GetLineCount() - 1;
	CString::Insert( nIndex, ch );
	m_adwColor.InsertAt( nIndex, dwColor );
	m_abyStyle.InsertAt( nIndex, (BYTE)( dwStyle ) );
	m_awCodePage.InsertAt( nIndex, CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::Insert( int nIndex, LPCTSTR pstr, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{
	int nLine = GetLineCount() - 1;
	CString::Insert( nIndex, pstr );
	m_adwColor.InsertAt( nIndex, dwColor, _tcslen( pstr) );
	m_abyStyle.InsertAt( nIndex, (BYTE)( dwStyle ), _tcslen( pstr) );
	m_awCodePage.InsertAt( nIndex, CODE_PAGE(wCodePage), _tcslen( pstr) );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::Delete( int nIndex, int nCount )
{
	int nLine = GetLineCount() - 1;
	CString::Delete( nIndex, nCount );
	m_adwColor.RemoveAt( nIndex, nCount );
	m_abyStyle.RemoveAt( nIndex, nCount );
	m_awCodePage.RemoveAt( nIndex, nCount );
	Align( m_pFont, 0 );//nLine );
	return TRUE;
}
int CEditString::DeleteLine( int nBeginLine, int nCount )
{
	int nMax = nBeginLine + nCount;
	int nBeginOffset, nEndOffset;
	int nLengthCount = 0;
	if( nMax > m_adwLineOffset.GetSize() )
	{
		nCount -= nMax - m_adwLineOffset.GetSize();
		nMax = m_adwLineOffset.GetSize();
	}
	for( int i = nBeginLine; i < nMax; i++ )
	{
		nBeginOffset = GetLineOffset( i );
		nEndOffset = GetLineOffset( i + 1 );
		nLengthCount += nEndOffset - nBeginOffset;
	}
	nBeginOffset = GetLineOffset( nBeginLine );
	CString::Delete( nBeginOffset, nLengthCount );
	m_adwColor.RemoveAt( nBeginOffset, nLengthCount );
	m_abyStyle.RemoveAt( nBeginOffset, nLengthCount );
	m_awCodePage.RemoveAt( nBeginOffset, nLengthCount );
	m_adwLineOffset.RemoveAt( nBeginLine, nCount ); 

	for( int j = nBeginLine; j < m_adwLineOffset.GetSize(); j++ )
	{
		int nNewOffset = m_adwLineOffset.GetAt( j ) - nLengthCount;
		m_adwLineOffset.SetAt( j, nNewOffset );
	}

	//ign( m_pFont, 0 );//nLine );
	return TRUE;
}
TCHAR CEditString::GetAt( int nIndex ) const
{
	return CString::GetAt( nIndex );
}
void CEditString::SetAt( int nIndex, TCHAR ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage )
{
	int nLine = GetLineCount() - 1;
	CString::SetAt( nIndex, ch );
	m_adwColor.SetAt( nIndex, dwColor );
	m_abyStyle.SetAt( nIndex, (BYTE)( dwStyle ) );
	m_awCodePage.SetAt( nIndex, CODE_PAGE(wCodePage) );
	Align( m_pFont, 0 );//nLine );
}
void CEditString::Init( int nWidth, SIZE sizeFont )
{
	m_nWidth = nWidth;
	m_sizeFont = sizeFont;
}
void CEditString::Init( CD3DFont* pFont, CRect* pRect )
{
	TCHAR str[ 3 ] = _T( "가" );

	m_sizeFont = pFont->GetTextExtent(str);

	BOOL bAlign = FALSE;
	// 사이즈와 폰트 둘중에 하나라도 기존것과 다르다면 정렬을 해주어야한다.
	if( m_nWidth != pRect->Width() || m_pFont != pFont )
		bAlign = TRUE;
	m_nWidth = pRect->Width();
	m_pFont = pFont;
	if( bAlign )
		Align( m_pFont, 0 );//nLine );
}
void CEditString::Adjust( int nWidth, SIZE sizeFont )
{
	m_nWidth = nWidth;
	m_sizeFont = sizeFont;
}
void CEditString::Adjust( CD3DFont* pFont, CRect* pRect )
{
	TCHAR str[ 3 ] = _T( "가" );
	m_sizeFont = pFont->GetTextExtent(str);
	BOOL bAlign = FALSE;
	// 사이즈와 폰트 둘중에 하나라도 기존것과 다르다면 정렬을 해주어야한다.
	if( m_nWidth != pRect->Width() || m_pFont != pFont )
		bAlign = TRUE;
	m_nWidth = pRect->Width();
	m_pFont = pFont;
	if( bAlign )
		Align( m_pFont, 0 );//nLine );
}

/*
void CEditString::Reset( CD3DFont* pFont, CRect* pRect, BOOL bHScroll, BOOL bVScroll )
{
	Init( pFont, pRect );
	m_adwLineOffset.SetSize( 0 );
	Align( pFont, 0 );
}
*/

//{{AFX
void CEditString::Align( CD3DFont* pFont, int nBeginLine )
{
	if( pFont == NULL )
		return;
//	m_adwLineOffset.SetSize( 0 );
	int nWidth  = m_nWidth;//m_sizeFont.cx;//lpRect->Width();// - 20;
	//int nHeight = pRect->Height();
	TCHAR str[ 3 ] = _T( "가" );
	if( nBeginLine < 0 ) nBeginLine = 0;
	CSize sizeString = m_sizeFont;//pFont->GetTextExtent(str);
	int nLength = GetLength();
	if( nLength == 0 || nBeginLine == 0 )
	{
		m_adwLineOffset.RemoveAll();
		if( nLength == 0 )
			return;
	}
	CString strTemp;
	int nOffset = 0;
	if( GetLineCount() )
	{
		nOffset = GetLineOffset( nBeginLine );
		//  라인옵셋 값이 이상 값이라면 
		if( nOffset < 0 || nOffset >= nLength ) 
			Error( "CEditString::Align : pos = %c, LineCount = %d, Length = %d", nOffset, GetLineCount(), nLength );
	}
	char chr = 0; 
	BOOL bNextLine = FALSE;
	BOOL bHangul   = FALSE;
	int nLineHeight = sizeString.cy;
	LPCTSTR lpszString = *this;
	CSize sizeTemp;
	CString strTemp2;

	do
	{
		m_adwLineOffset.SetAtGrow( nBeginLine++,  nOffset );
		strTemp = _T("");
		sizeTemp = CSize( 0, 0 );
		do 
		{
			chr = GetAt( nOffset++ );
			// 스페이스를 발견하면? (영어식 출력을 위해)
			//if(chr == ' ');
			// 엔터코드면 스킵 

			if(chr != '\r')
			{
				strTemp += chr;
				strTemp2 += chr;
			}
			if(chr == '\n')
				break;

			// 한글 코드면 계속
			if( IsDBCSLeadByte( chr ) && bHangul == FALSE )
				bHangul = TRUE;
			else
			{
				CSize sizeTemp2;
				pFont->GetTextExtent( strTemp2, &sizeTemp2 );
				sizeTemp.cx += sizeTemp2.cx;
				strTemp2.Empty();
				if( sizeTemp.cx > nWidth ) 
				{
					if( bHangul )
						strTemp.Delete( strTemp.GetLength() - 2, 2 ), nOffset -= 2;
					else
						strTemp.Delete( strTemp.GetLength() - 1, 1 ), nOffset -= 1;
					bHangul = FALSE;
					break;
				}		
				bHangul = FALSE;
			}
		} while( nOffset < nLength && chr != '\0' && chr != '\n' );
		bNextLine = TRUE;
		sizeString.cy += nLineHeight;
	} 
	while( chr != '\0' && nOffset < nLength );
}
//}}AFX


DWORD CEditString::GetLineCount() 
{
//	if( m_adwLineOffset.GetSize() == 0 )
//		return 0;
	return (DWORD) m_adwLineOffset.GetSize();//- 1;
}
DWORD CEditString::GetLineOffset( DWORD dwLine ) 
{
	if ( dwLine >= (DWORD)m_adwLineOffset.GetSize() ) 
		return GetLength();
	if ( dwLine < 0 ) return 0;
	return m_adwLineOffset[ dwLine ];
}
void CEditString::GetLine( DWORD dwLine, CHAR* szStr )
{
	CString retval = _T("");

	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin ;//- 1;
		else 
			size = GetLength() - begin;

#ifdef __VS6
		memcpy( (VOID*)szStr, (VOID*)&m_pchData[ begin ], size );
#else
		memcpy( (VOID*)szStr, (VOID*)&CString::GetString()[ begin ], size );
#endif
		szStr[ size ] = 0;
	}
}

CString CEditString::GetLine(DWORD dwLine)
{
	CString retval = _T("");

	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin ;//- 1;
		else 
			size = GetLength() - begin;
		retval = Mid( begin, size ); 
		//retval.Remove('\r');
		//retval.Remove('\n');
	}
	return retval;
}
CEditString CEditString::GetEditString( DWORD dwLine )
{
	CEditString retval = _T("");
	
	if( dwLine >= 0 && dwLine < GetLineCount() ) 
	{
		LONG begin = GetLineOffset( dwLine );
		LONG size;
		if( dwLine < GetLineCount() ) 
			size = GetLineOffset( dwLine + 1 ) - begin;
		else 
			size = GetLength() - begin;
		retval = Mid( begin, size ); 
		for( int i = 0; i < size; i++ )
		{
			retval.m_adwColor[ i ] = m_adwColor[ begin + i ];
			retval.m_abyStyle[ i ] = m_abyStyle[ begin + i ];
			retval.m_awCodePage[ i ] = m_awCodePage[ begin + i ];
		}
	}
	return retval;
}
DWORD CEditString::OffsetToLine(DWORD dwOffset)
{
	CString retval = _T("");

	DWORD dwLineCount = GetLineCount();

	for(DWORD i = 0; i < dwLineCount; i++) 
	{
		DWORD dwBegin = GetLineOffset( i );
		DWORD dwNext = GetLineOffset( i + 1 );

		if(dwOffset >= dwBegin && dwOffset < dwNext)
			return i;
	}
	if( dwOffset == (DWORD)GetLength() )
	{
		if( dwOffset > 0 && GetAt( dwOffset - 1 ) == '\n')
			return dwLineCount;
		return dwLineCount - 1;
	}
	return 0;
}

#endif //_MULTI_LANGUAGE_

