
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


//---------------------------------------------------------------------------------------------
// 폰트 텍스쳐 구조체
//---------------------------------------------------------------------------------------------
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


//---------------------------------------------------------------------------------------------
// 글리프 정보 구조체
//---------------------------------------------------------------------------------------------
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
#if defined( __CLIENT)
    virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
	virtual HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L  );
#endif 
	
    virtual HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
                      const TCHAR* strText, DWORD dwFlags=0L );
	virtual HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                            const TCHAR* strText, DWORD dwFlags=0L  );
    HRESULT DrawTextScaled( FLOAT x, FLOAT y, FLOAT z, 
                            FLOAT fXScale, FLOAT fYScale, DWORD dwColor, 
                            const TCHAR* strText, DWORD dwFlags=0L );
    HRESULT Render3DText( const TCHAR* strText, DWORD dwColor, DWORD dwFlags=0L );
    
    // Function to get extent of text
    virtual HRESULT GetTextExtent( const TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();

    // Constructor / destructor
	CD3DFontAPI();
    CD3DFontAPI( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    virtual ~CD3DFontAPI();

	// add
	DWORD m_dwMaxHeight;

	DWORD GetMaxHeight() { return m_dwMaxHeight; }
};


class CD3DFont;

//---------------------------------------------------------------------------------------------
// D3D 폰트 API VTN
//------------------------------------------------------------------------------------------
class CD3DFontAPIVTN : public CD3DFontAPI
{

public:


					CD3DFontAPIVTN( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
					~CD3DFontAPIVTN();

#if defined( __CLIENT)
   
	HRESULT					DrawText(	FLOAT			x, 
										FLOAT			y,
										DWORD			dwColor,
										CEditString&	editString,
										int				nPos		= 0, 
										int				nLine		= 0, 
										int				nLineSpace	= 0, 
										DWORD			dwFlags		= 0L
									 );
	
	HRESULT					DrawText(	FLOAT			x, 
										FLOAT			y, 
										FLOAT			fXScale, 
										FLOAT			fYScale, 
										DWORD			dwColor,
										CEditString&	editString, 
										int				nPos		= 0, 
										int				nLine		= 0, 
										int				nLineSpace	= 0, 
										DWORD			dwFlags		= 0L
									 );
#endif

	HRESULT			DrawText( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );
	HRESULT			DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, const TCHAR* strText, DWORD dwFlags=0L );

    // Function to get extent of text
    HRESULT			GetTextExtent( const TCHAR* strText, SIZE* pSize );

    // Initializing and destroying device-dependent objects
    HRESULT 		InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT 		InvalidateDeviceObjects( void );
    HRESULT 		RestoreDeviceObjects( void );
    HRESULT 		DeleteDeviceObjects( void );

	DWORD			GetMaxHeight( void );


public:


	CD3DFont*		m_pD3DFont;

};




//---------------------------------------------------------------------------------------------
// D3D 폰트 클레스
//---------------------------------------------------------------------------------------------
class CD3DFont 
{

public:

							CD3DFont( const char* strFontName, DWORD dwHeight, DWORD dwFlags = 0L );
							~CD3DFont();

    HRESULT					InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT					InvalidateDeviceObjects( void );
    HRESULT					RestoreDeviceObjects( void );
    HRESULT					DeleteDeviceObjects( void );

    HRESULT					DrawText( FLOAT x, FLOAT y, DWORD dwColor, const char* strText, DWORD dwFlags = 0L, WORD wCodePage = 0 );
	HRESULT					DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, const char* strText, DWORD dwFlags = 0L, WORD wCodePage = 0 );


#if defined( __CLIENT)

	HRESULT					DrawText(	FLOAT			x, 
										FLOAT			y,
										DWORD			dwColor,
										CEditString&	editString,
										int				nPos		= 0, 
										int				nLine		= 0, 
										int				nLineSpace	= 0, 
										DWORD			dwFlags		= 0L
									 );
	
	HRESULT					DrawText(	FLOAT			x, 
										FLOAT			y, 
										FLOAT			fXScale, 
										FLOAT			fYScale, 
										DWORD			dwColor,
										CEditString&	editString, 
										int				nPos		= 0, 
										int				nLine		= 0, 
										int				nLineSpace	= 0, 
										DWORD			dwFlags		= 0L
									 );



	HRESULT					GetTextExtent_EditString( CEditString& editString, SIZE* pSize, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0 );
	CSize					GetTextExtent_EditString( CEditString& editString, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0 ) { SIZE size; GetTextExtent_EditString( editString, &size, nLineBegin, nLineCount, wCodePage ); return size; }

#endif


    HRESULT					GetTextExtent( const char* strText, SIZE* pSize, WORD wCodePage = 0 );
	FONTTEX*				GetFontTex(const char* begin, const char* end, WORD wCodePage );
	HFONT					GetFont(WORD wCodePage);

	DWORD					GetMaxHeight( void )					{	return m_dwMaxHeight;										}
    CSize					GetTextExtent( LPCTSTR pszText )		{	SIZE size; GetTextExtent( pszText, &size ); return size;	}


private : 

 
    HRESULT   				CreateGDIFont( HDC hDC, HFONT* pFont );
    HRESULT   				PaintAlphabet( HDC hDC,DWORD* pBitmapBits, BOOL bMeasureOnly=FALSE );
	void					CopyToTexture( LPDIRECT3DTEXTURE9 pTex, CSize size, DWORD dwBgColor );
	void					MakeOutLine( int nWidth, WORD* pDst16 );	
	LPDIRECT3DTEXTURE9		CreateTexture( void );


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

	typedef	std::map<GLYPHINFO,FONTTEX>		font_tex_map;
	typedef	font_tex_map::iterator		font_tex_map_iter;

	font_tex_map						m_fontTex;

	typedef std::map<WORD, HFONT>			font_map;
	typedef font_map::iterator			font_map_iter;

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
	DWORD*  							m_pBitmapBits;

	int									m_nCurX;
	int									m_nCurY;

	char    							m_strFontName[80];      
};


#endif 



// 이전 소스코드 주석
//class CD3DFont //: public CD3DFontAPI
//{
//    char    m_strFontName[80];            // Font properties
//    DWORD   m_dwFontHeight;
//    DWORD   m_dwFontFlags;
//
//	DWORD   m_bCaption;
//    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
//	LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
//    DWORD   m_dwTexWidth;                 // Texture dimensions
//    DWORD   m_dwTexHeight;
//    FLOAT   m_fTextScale;
//    DWORD   m_dwSpacing;                  // Character pixel spacing per side
//
//    // Stateblocks for setting and restoring render states
//    LPDIRECT3DSTATEBLOCK9 m_pStateBlockSaved;
//    LPDIRECT3DSTATEBLOCK9 m_pStateBlockDrawText;
//
//    HRESULT   CreateGDIFont( HDC hDC, HFONT* pFont );
//    HRESULT   PaintAlphabet( HDC hDC,DWORD* pBitmapBits, BOOL bMeasureOnly=FALSE );
//	void CopyToTexture( LPDIRECT3DTEXTURE9 pTex, CSize size, DWORD dwBgColor );
//	HFONT   m_hFont;
//	HFONT   m_hFontOld;
//	HDC     m_hDC;
//	HBITMAP m_hbmBitmap;
//	HGDIOBJ m_hbmOld;
//	DWORD*  m_pBitmapBits;
//
//	LPDIRECT3DTEXTURE9 m_pCurTexture;
//	int m_nCurX,m_nCurY;
//
//	void MakeOutLine( int nWidth, WORD* pDst16 );	
//	LPDIRECT3DTEXTURE9 CreateTexture();
//public:
//	CPtrArray m_apTexture;
//    
//	DWORD  m_dwColor;
//	DWORD  m_dwBgColor;
//	DWORD  m_nOutLine;
//	DWORD  m_dwFlags;
//	
//	SIZE m_sizeBitmap;
//	
//	WORD	m_wCodePage;
//
//	FONTTEX* GetFontTex(const char* begin, const char* end, WORD wCodePage );
//	HFONT    GetFont(WORD wCodePage);
//
//	typedef	std::map<GLYPHINFO,FONTTEX>	font_tex_map;
//	typedef	font_tex_map::iterator	font_tex_map_iter;
//
//	font_tex_map	m_fontTex;
//	DWORD			m_dwNumTriangles;
//
//	typedef std::map<WORD, HFONT>		font_map;
//	typedef font_map::iterator		font_map_iter;
//
//	font_map		m_fontMap;
//	
//    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
//                      const char* strText, DWORD dwFlags=0L, WORD wCodePage=0 );
//	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
//                            const char* strText, DWORD dwFlags=0L, WORD wCodePage=0  );
//
//#if defined( __CLIENT)
//    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
//		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L );
//	HRESULT DrawText( FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
//		CEditString& editString, int nPos = 0, int nLine = 0, int nLineSpace = 0, DWORD dwFlags=0L  );
//	HRESULT GetTextExtent_EditString( CEditString& editString, SIZE* pSize, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0 );
//	CSize GetTextExtent_EditString( CEditString& editString, int nLineBegin = 0, int nLineCount = -1, WORD wCodePage=0) { SIZE size; GetTextExtent_EditString( editString, &size, nLineBegin, nLineCount, wCodePage ); return size; }
//#endif
//    // Function to get extent of text
//    HRESULT GetTextExtent( const char* strText, SIZE* pSize, WORD wCodePage=0 );
//    CSize GetTextExtent( LPCTSTR pszText ) { SIZE size; GetTextExtent( pszText, &size ); return size; }
//
//    // Initializing and destroying device-dependent objects
//    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
//    HRESULT RestoreDeviceObjects();
//    HRESULT InvalidateDeviceObjects();
//    HRESULT DeleteDeviceObjects();
//
//    // Constructor / destructor
//    CD3DFont( const char* strFontName, DWORD dwHeight, DWORD dwFlags=0L);
//    ~CD3DFont();
//
//	// add
//	DWORD m_dwMaxHeight;
//
//	DWORD GetMaxHeight() { return m_dwMaxHeight; }
//};







