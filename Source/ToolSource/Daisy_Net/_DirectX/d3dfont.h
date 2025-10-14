
#ifndef D3DFONT_H
#define D3DFONT_H

#include <D3D9.h>
#include <tchar.h>
#include <d3dx9core.h>

#if defined( __CLIENT) || defined( __DAISY )
	#include "EditString.h"
#endif


// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_TWOSIDED    0x0004
#define D3DFONT_FILTERED    0x0008


typedef struct tagFontTexture
{
	LPDIRECT3DTEXTURE9 pTexture;
	FLOAT tx1;
	FLOAT ty1;
	FLOAT tx2;
	FLOAT ty2;

	tagFontTexture()
	:	pTexture( NULL )
	,	tx1( 0.0f )
	,	ty1( 0.0f )
	,	tx2( 0.0f )
	,	ty2( 0.0f )
	{
	}

} FONTTEX, * LPFONTTEX;


typedef struct tagGlyphInfo
{
	WORD wCodePage;
	char text[6];
	bool operator < (const tagGlyphInfo& glyphInfo) const { return memcmp(this, &glyphInfo, sizeof(tagGlyphInfo)) < 0; }
	
	tagGlyphInfo()
	:	wCodePage( NULL )
	{
		ZeroMemory( text, sizeof(char) * _countof(text) );
	}
} GLYPHINFO;

void    SetCodePage( int nLang );

//-----------------------------------------------------------------------------
// Name: class CD3DFontAPI
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFontAPI
{
    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

	DWORD   m_bCaption;
    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
    FLOAT   m_fTextScale;
    DWORD   m_dwSpacing;                  // Character pixel spacing per side

	/////////////////////////////////////////
	LPD3DXFONT    m_pFont;
	//LPD3DXFONT    m_pFont2;
	//LPD3DXFONT    m_pStatsFont;
	LPD3DXMESH    m_pMesh3DText;
	//WCHAR*        m_strTextBuffer;
	
	ID3DXSprite*  m_pD3DXSprite;
	
//	int           m_nFontSize;
	/////////////////////////////////////////

public:
	int   m_nOutLine;
	DWORD m_dwColor;
	DWORD m_dwBgColor;
	DWORD m_dwFlags;
	
public:
	SIZE m_sizeBitmap;

	//LPFONTTEX GetFontOffset( WORD wText ) { if( wText >= m_aFontOffset.GetSize() ) return NULL; return (LPFONTTEX)m_aFontOffset.GetAt( wText ); }
    // 2D and 3D text drawing functions
#if defined( __CLIENT) || defined( __DAISY )
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L  );
#endif 
	
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
                      const TCHAR* strText, DWORD dwFlags=0L );
	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            const TCHAR* strText, DWORD dwFlags=0L  );
    HRESULT DrawTextScaled( FLOAT x, FLOAT y, FLOAT z, 
                            FLOAT fXScale, FLOAT fYScale, DWORD dwColor, 
                            const TCHAR* strText, DWORD dwFlags=0L );
    HRESULT Render3DText( const TCHAR* strText, DWORD dwColor, DWORD dwFlags=0L );
    
    // Function to get extent of text
    HRESULT GetTextExtent( const TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

    // Constructor / destructor
    CD3DFontAPI( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    ~CD3DFontAPI();

	// add
	DWORD m_dwMaxHeight;

	DWORD GetMaxHeight() { return m_dwMaxHeight; }
};



class CD3DFont 
{

public:

										CD3DFont( const char* strFontName, DWORD dwHeight, DWORD dwFlags = 0L );
										~CD3DFont();

    HRESULT								InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT								InvalidateDeviceObjects( void );
    HRESULT								RestoreDeviceObjects( void );
    HRESULT								DeleteDeviceObjects( void );

    HRESULT								DrawText( FLOAT x, FLOAT y, DWORD dwColor, const char* strText, DWORD dwFlags = 0L, WORD wCodePage = 0 );
	HRESULT								DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, const char* strText, DWORD dwFlags = 0L, WORD wCodePage = 0 );

#ifdef __FREE_TYPE_FONT
private:
	HRESULT					PrevOutlineDrawText( FLOAT x, FLOAT y, DWORD dwColor, const char* strText, DWORD dwFlags = 0L, WORD wCodePage = 0 );
public:
#endif

#if defined( __CLIENT) || defined( __DAISY )

	HRESULT 							DrawText( FLOAT x, FLOAT y, DWORD dwColor, CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );


#ifdef __FREE_TYPE_FONT
private:
	HRESULT					PrevOutlineDrawText( 	FLOAT			x, 
													FLOAT			y,
													DWORD			dwColor,
													CEditString&	editString,
													int				nPos		= 0, 
													int				nLine		= 0, 
													int				nLineSpace	= 0, 
													DWORD			dwFlags		= 0L
													);
public:

#endif

	HRESULT 							DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L  );

#ifdef __V050714_BUGFIX_TOOLTIP
	HRESULT								GetTextExtent_EditString( CEditString& editString, SIZE* pSize, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0 );
	CSize								GetTextExtent_EditString( CEditString& editString, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0) { SIZE size; GetTextExtent_EditString( editString, &size, nLineBegin, nLineCount, wCodePage ); return size; }
#endif

#endif


    HRESULT								GetTextExtent( const char* strText, SIZE* pSize, WORD wCodePage = 0 );
	FONTTEX*							GetFontTex(const char* begin, const char* end, WORD wCodePage );
	HFONT								GetFont(WORD wCodePage);

	DWORD								GetMaxHeight( void )					{	return m_dwMaxHeight;										}
    CSize								GetTextExtent( LPCTSTR pszText )		{	SIZE size; GetTextExtent( pszText, &size ); return size;	}


private : 

 
    HRESULT   							CreateGDIFont( HDC hDC, HFONT* pFont );
    HRESULT   							PaintAlphabet( HDC hDC,DWORD* pBitmapBits, BOOL bMeasureOnly=FALSE );
	void								CopyToTexture( LPDIRECT3DTEXTURE9 pTex, CSize size, DWORD dwBgColor, int PenX , int PenY );
#ifndef __FREE_TYPE_FONT
	void								MakeOutLine( int nWidth, WORD* pDst16 );	
#else
	void								MakeOutLine( int nWidth, DWORD* pDst32 );
#endif


	LPDIRECT3DTEXTURE9					CreateTexture( void );

#ifdef __FREE_TYPE_FONT
	//-->@@추가 KYT [ 2012 / 11 / 13 ] : 프리타입에 필요 함수목록
	HRESULT   							CreateFreeTypeFont(  HDC hDC,  const char* szFontName );
	void								SetFreeFontSize( int nFontSize );
	bool								SelecUnicodeCharmap( FT_Face face );
	bool								Charencoing( int& nCodePage );
	int									ConvertCodePoint( const int nCodePoint, const int nCodePage,  wchar_t wChar );
	int									FindGlyphIndex( const wchar_t* wszChar );
	bool								FreeTypeFont_GlyphLoad( const char* szchar );
	void								FreeTypeFont_TexOut( const char* szChar );

	int									get_pen_start_x( int nFontSize );
	int									get_pen_start_y( int nFontSize );

	int									get_x_margin( int nFontSize);
	int									get_y_margin( int nFontSize);
	//<-- 
#endif

public:

   
	DWORD								m_dwColor;
	DWORD								m_dwBgColor;
	DWORD								m_nOutLine;
	DWORD								m_dwFlags;
	
	SIZE								m_sizeBitmap;
	
	

	WORD								m_wCodePage;

	DWORD								m_dwNumTriangles;
	DWORD								m_dwMaxHeight;

	CPtrArray							m_apTexture;

	typedef	map<GLYPHINFO,FONTTEX>		font_tex_map;
	typedef	font_tex_map::iterator		font_tex_map_iter;

	font_tex_map						m_fontTex;

	typedef map<WORD, HFONT>			font_map;
	typedef font_map::iterator			font_map_iter;

#ifdef __FREE_TYPE_FONT
	typedef std::vector< DWORD >		vector_BitColor;
#endif 

	font_map							m_fontMap;
	

private:


    LPDIRECT3DDEVICE9					m_pd3dDevice;			
	LPDIRECT3DVERTEXBUFFER9				m_pVB;					
	LPDIRECT3DSTATEBLOCK9				m_pStateBlockSaved;
    LPDIRECT3DSTATEBLOCK9				m_pStateBlockDrawText;
	LPDIRECT3DTEXTURE9					m_pCurTexture;
		
    DWORD   							m_dwFontHeight;
    DWORD   							m_dwFontFlags;

	DWORD								m_bCaption;
    DWORD   							m_dwTexWidth;           
    DWORD   							m_dwTexHeight;
    FLOAT   							m_fTextScale;
    DWORD   							m_dwSpacing;            

	HFONT   							m_hFont;
	HFONT   							m_hFontOld;
	HDC     							m_hDC;
	HBITMAP 							m_hbmBitmap;
	HGDIOBJ 							m_hbmOld;
#ifndef __FREE_TYPE_FONT
	DWORD*  							m_pBitmapBits;
#else
	vector_BitColor						m_pBitmapBits;
#endif 

	int									m_nCurX;
	int									m_nCurY;

	char    							m_strFontName[80];

#ifdef __FREE_TYPE_FONT
	//-->@@추가 KYT [ 2012 / 11 / 13 ] : 프리타입에 필요 멤버
	FT_Library							m_ftLibrary;
	FT_Face								m_pFtCurrentFace;
	FT_UInt								m_nGlpyhID;
	SIZE								m_FreeTypeFont_Size;
	bool								m_IsSelectUnicodeMap;

	int									m_Pen_x;
	int									m_Pen_y;

	int									m_StartPen_x;
	int									m_StartPen_y;

	int									m_BitCount;
#endif 

};
#endif 










