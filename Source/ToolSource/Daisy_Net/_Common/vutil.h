//-----------------------------------------------------------------------------
// File: D3DUtil.h
//
// Desc: Helper functions and typing shortcuts for Direct3D programming.
//
// Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#ifndef VUTIL_H
#define VUTIL_H
#include <D3D9.h>
#include <D3DX9Math.h>

BOOL IsFontInstalled( LPCTSTR pszFace );

void RepairString( TCHAR* lpStr );
void RepairString( CString& string );
void SetStrNull( TCHAR* lpStr, int nNullLength );
void SetStrNull( CString& string, int nNullLength );

size_t WCharToMBChar( char *mbstr, const wchar_t *wcstr, size_t count )
{
	//CHAR* lpMultiByte = new CHAR[ m_nProgSize + 2 ];
	return WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)wcstr, -1, mbstr, count, NULL, NULL );
}
int GetCharLen( const CHAR* pStr );
//inline BOOL IsWordChar( BYTE c )
//{
//	return IsDBCSLeadByte( c );
//	return c >= 0x80;//( c == EXTRA_HANGUL || ( c >= MIN_HANGUL_HIGH && c < MAX_HANGUL_HIGH ) );
//} 

int CopyChar( const char* pSrc, char* pDest );
BOOL IsMultiByte( const char* pSrc );
BOOL IsHangul( WORD word );
BOOL IsNative( LPCTSTR lpszStr );

// 모든언어의 문자관련
int GetStrCut( const CHAR* pSrc, CHAR* pDest, int nCount );
int GetStrLen( const CHAR* pSrc );

/*
inline BOOL IsHangul( WORD word )
{
	//
	//if( word >= 0x8141 && word <= 0xc6fe)
	//	return TRUE;
	//if( word >= 0xb0a1 && word <= 0xc8fe )
	//	return TRUE;
		
	BYTE l = word & 0xff;
	BYTE h = word >> 8;

	if (0xa1 > l || 0xfe < l ) return FALSE;
	if (0xb0 <= h && 0xc8 >= h ) return TRUE;
	return FALSE;

	

	// 특수 문자면 FALSE
	if( h >= 0xa1 && h <= 0xa9 && l >= 0x80 && l <= 0xff )
		return FALSE;
	// 한글 
	if( h >= 0x81 && h <= 0xc6 && l >= 0x41 && l <= 0xfe )
		return TRUE;
	if( h >= 0xb0 && h <= 0xc8 && l >= 0xa1 && l <= 0xfe )
		return TRUE;
	return FALSE;
} 
*/
#define IMGBIT_8   0  
#define IMGBIT_16  1 
#define IMGBIT_24  2
#define IMGBIT_32  3 

typedef struct tgaIMAGE
{
	LPBYTE lpData;
	SIZE   size;
	int    nImgBit;

	tgaIMAGE()
	:	lpData( NULL )
	,	nImgBit( NULL )
	{
		ZeroMemory( &size, sizeof(SIZE) );
	}
} IMAGE,* LPIMAGE;

HRESULT LoadTextureFromRes( 
	LPDIRECT3DDEVICE9 pd3dDevice, 
	LPCTSTR strTexture,
	LPDIRECT3DTEXTURE9* ppTexture,
	DWORD MipFilter = 1,
	D3DFORMAT d3dFormat = D3DFMT_UNKNOWN 
	);
HRESULT LoadTextureFromRes( 
	LPDIRECT3DDEVICE9 pDevice,
	LPCTSTR pFileName,
	UINT Width,
	UINT Height,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	DWORD Filter,
	DWORD MipFilter,
	D3DCOLOR ColorKey,
	D3DXIMAGE_INFO *pSrcInfo,
	PALETTEENTRY *pPalette,
	LPDIRECT3DTEXTURE9 *ppTexture
	);
#ifdef __V050322_CACHEQUEST
void PaintTexture( LPVOID pDestData, LPIMAGE pImage, CPoint pt, CSize sizeSurface, D3DFORMAT d3dFormat );
#else
void PaintTexture( LPVOID pDestData, LPIMAGE pImage, CPoint pt, CSize sizeSurface );
#endif
void AdjustSize( SIZE* pSize );
//BOOL LoadImage( LPCTSTR lpszFileName, LPIMAGE lppImage );//LPBYTE* lppData, SIZE* pSize, int* pnImgBit );
BOOL LoadImage( IDirect3DDevice9* pDevice, LPCTSTR lpszFileName, LPIMAGE lppImage );//LPBYTE* lppData, SIZE* pSize, int* pnImgBit );
BOOL LoadBMP( LPCTSTR lpszFileName, LPIMAGE lppImage );
BOOL LoadTGA( LPCTSTR lpszFileName, LPIMAGE lppImage );
BOOL LoadPNG( IDirect3DDevice9* pDevice, LPCTSTR lpszFileName, LPIMAGE lppImage );
BOOL SaveBMP( LPCTSTR lpszFileName, LPBYTE lpData, SIZE size );
void GetPickRay( CRect rect,POINT ptCursor,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir );
void GetRayEnd( D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd );

//-----------------------------------------------------------------------------
// Name: IntersectTriangle()
// Desc: orig에서 쏜 d 광선이 v0,v1,v2 삼각형 안에 속하는지를 확인하고,
//       속하고 있다면 삼각형과의 교차점을 pIntersect에 돌려준다.
//       또한 삼각형과의 거리를 fDist에 돌려준다.
//-----------------------------------------------------------------------------

BOOL IntersectTriangle(D3DXVECTOR3& v0,D3DXVECTOR3& v1, D3DXVECTOR3& v2, const D3DXVECTOR3& orig,const D3DXVECTOR3& d, D3DXVECTOR3* pIntersect,FLOAT* pfDist, BOOL bCull = FALSE );

HRESULT D3DUtil_CreateMipTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR strTexture,
                               LPDIRECT3DTEXTURE9* ppTexture,
                               D3DFORMAT d3dFormat = D3DFMT_UNKNOWN, D3DCOLOR d3dKeyColor = 0x00000000 );

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

#ifdef __DIGIT_THAI
#define isdigit(c)   isdigit2((CHAR)c)
inline BOOL isdigit2(CHAR c)
{
	if( c >= '0' && c <= '9' )
		return TRUE;
	else
		return FALSE;
}
#endif

typedef struct _ERROR_STATE
{
	DWORD	m_dwWndId;
	BOOL	m_bIsShowWnd;
	CHAR	m_ch;
} _ERROR_STATE;

extern _ERROR_STATE g_Error_State;


#endif // D3DUTIL_H
