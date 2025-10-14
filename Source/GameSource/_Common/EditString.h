
#pragma once 

#include "vutil.h"

#define		ESSTY_UNDERLINE     		1
#define		ESSTY_BOLD          		2 
#define		ESSTY_STRIKETHROUGH 		4
#define		ESSTY_SHADOW				8
#define		ESSTY_BLOCK					16

#ifdef __HYPERLINK_ITEM16
	#define ESSTY_HYPERLINK_ITEM		32
#endif 


class CD3DFont* pFont;


//---------------------------------------------------------------------------------------------
// 편집 문자열 클레스
//---------------------------------------------------------------------------------------------
class CEditString		: public CString
{

public:

						CEditString( void );
						CEditString( LPCSTR		lpsz );
						CEditString( LPCWSTR	lpsz );
						CEditString( LPCTSTR	lpch, int nLength );

						CEditString( const UCHAR*			psz );
						CEditString( const CString&			stringSrc );
						CEditString( const CEditString&		stringSrc );

						CEditString( TCHAR ch, int nRepeat = 1 );

						~CEditString();


	void 				ParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, CString& string, CDWordArray& adwColor, CByteArray& abyStyle, CWordArray& awCodePage, DWORD dwPStyle = 0x00000001 );

	void 				Init( int nWidth, SIZE sizeFont );
	void 				Init( CD3DFont* pFont, CRect* pRect );
	int					Insert( int nIndex, TCHAR ch, DWORD dwColor = 0xff000000, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	int					Insert( int nIndex, LPCTSTR pstr, DWORD dwColor = 0xff000000, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );
	void 				Adjust( int nWidth, SIZE sizeFont );
	void 				Adjust( CD3DFont* pFont, CRect* pRect );
	int					Delete( int nIndex, int nCount = 1 );
	int					DeleteLine( int nBeginLine, int nCount  = 1 );

	int					Find( LPCTSTR pstr, int nStart ) const;		
	void				Align( CD3DFont* pFont, int nBeginLine = 0 );
	void				Empty( void );

	void 				AddString( LPCTSTR lpsz );
	void 				AddString( LPCTSTR lpsz, DWORD dwColor );
	void 				AddString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle );
	void 				AddString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage );
	void 				AddString( char ch );
	void 				AddString( char ch,	DWORD dwColor, DWORD dwStyle );
	void 				AddString( char ch, DWORD dwColor, DWORD dwStyle, WORD wCodePage );
	void				AddParsingString( LPCTSTR lpsz );
	void				AddParsingString( LPCTSTR lpsz, DWORD dwColor );
	void				AddParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle );
	void				AddParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, DWORD dwPStyle );
	void 				AddEditString( const CEditString& string );

	void 				SetString( LPCTSTR lpsz );
	void 				SetString( LPCTSTR lpsz, DWORD dwColor );
	void 				SetString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle );
	void 				SetString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage );
	void 				SetString( char* lpsz );
	void 				SetString( char* lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage );
	void 				SetParsingString( LPCTSTR lpsz );
	void 				SetParsingString( LPCTSTR lpsz, DWORD dwColor );
	void 				SetParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, DWORD dwPStyle );
	void 				SetParsingString( LPCTSTR lpsz, DWORD dwColor, DWORD dwStyle, WORD wCodePage, DWORD dwPStyle, BOOL bNoInitAlign );
	void 				SetEditString( const CEditString& string );

	void 				SetCodePage( WORD codePage );
	void 				SetColor( int nPos, int nLength, DWORD dwColor );
	void 				SetColor( DWORD dwColor );
	void 				SetAlpha( DWORD dwColor );
	void 				SetStyle( int nPos, int nLength, DWORD dwStyle );
	void 				SetStyle( DWORD dwStyle );
	void				SetCodePage( int nPos, int nLength, WORD wCodePage );
	void				SetAt( int nIndex, TCHAR ch, DWORD dwColor = 0xff000000, DWORD dwStyle = 0x00000000, WORD wCodePage = 0 );

	void				GetTextFormat( CString& str );
	CString				GetLine( DWORD dwLine );
	void				GetLine( DWORD dwLine, CHAR* pszStr );
	DWORD				GetLineCount( void );
	DWORD				GetLineOffset( DWORD dwLine ) ;
	CEditString			GetEditString( DWORD dwLine );
	TCHAR				GetAt( int nIndex ) const;

	DWORD				OffsetToLine( DWORD dwOffset );

	void				ClearStyle( int nPos, int nLength, DWORD dwStyle );
	void				ClearStyle( DWORD dwStyle );

#ifdef __IMPROVE_MAP_SYSTEM

	void				ClearColor( DWORD dwColor );
	void				ClearColor( int nPos, int nLength, DWORD dwColor );

	CString				GetLine( DWORD dwLine, int& rnLineStartPosition );

#endif 

	// 연산자
	const CEditString&	operator=( const CString& stringSrc ) { SetString( stringSrc ); return *this; } 
	const CEditString&	operator=( LPCSTR lpsz )  { SetString( lpsz ); return *this; } 
	const CEditString&	operator=( const CEditString& stringSrc );

#ifdef _UNICODE
	const CEditString&	operator=( char ch ) { SetString( ch ); return *this; } 
#endif

	const CEditString&	operator+=( const CEditString& string ) { AddEditString( string ); return *this; } 
	const CEditString&	operator+=( const CString& string ) { AddString( string ); return *this; } 
	const CString&		operator+= (LPCTSTR lpsz ) { SetString( lpsz ); return *this; } 
	const CString&		operator+= (char* lpsz ) { SetString( lpsz ); return *this; } 

#ifdef _UNICODE
	const CString&		operator+=( char ch ) { SetString( ch ); return *this; } 
#endif


protected:
	
	
	void				InitWordAlignment( void );


public:


	CD3DFont*			m_pFont;
	SIZE				m_sizeFont;
	BOOL				m_bWordAlign;
	int					m_nWidth;

	CDWordArray 		m_adwColor;
	CByteArray  		m_abyStyle;
	CDWordArray 		m_adwLineOffset;
	CWordArray  		m_awCodePage;

};


//---------------------------------------------------------------------------------------------
// 인라인 함수
//---------------------------------------------------------------------------------------------
inline BOOL IsWhite(TCHAR c)
{
	return (iswspace(c) || c == '\r' || c == '\n' || !iswprint(c)) && !IsDBCSLeadByte(c);
}

inline BOOL IsAlphaNum(TCHAR c)
{
	return iswalnum(c);
}

