//-----------------------------------------------------------------------------
// File: D3DUtil.h
//
// Desc: Helper functions and typing shortcuts for Direct3D programming.
//
// Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#ifndef VUTIL_H
#define VUTIL_H
#if !defined(__ACCOUNTSERVER) && !defined(__CACHESERVER) && !defined(__CERTIFIER) && !defined (__CORESERVER) && !defined(__DBSERVER) && !defined(__LOGINSERVER)
#include <D3D9.h>
#include <D3DX9Math.h>
#endif

#include "../_CommonDefine\FlyffCommonDefine.h"

#pragma	warning( push )
#pragma warning( disable : 4995 )
#include <atlstr.h>
#pragma	warning( pop )

BOOL IsFontInstalled( LPCTSTR pszFace );
void RepairString( TCHAR* lpStr );
void RepairString( CString& string );
void SetStrNull( TCHAR* lpStr, int nNullLength );
void SetStrNull( CString& string, int nNullLength );

int GetCharLen( const CHAR* pStr );
int CopyChar( const char* pSrc, char* pDest );
BOOL IsMultiByte( const char* pSrc );
BOOL IsHangul( WORD word );
BOOL IsNative( LPCTSTR lpszStr );

// 모든언어의 문자관련
int GetStrCut( const CHAR* pSrc, CHAR* pDest, int nCount );
int GetStrLen( const CHAR* pSrc );


#define IMGBIT_8   0  
#define IMGBIT_16  1 
#define IMGBIT_24  2
#define IMGBIT_32  3 

struct IMAGE
{
	LPBYTE lpData;
	SIZE   size;
	int    nImgBit;
	DWORD  dwSize;
	DWORD	dwSizeBak;
	IMAGE() 
	{
		Init( );
	}

	~IMAGE() 
	{
		if (lpData)
		{
			delete [] (lpData);
			lpData = nullptr;
		}
	}

	void Init()
	{
		lpData = NULL;
		size.cx = 0;
		size.cy = 0;
		nImgBit = 0;
		dwSize = 0;
		dwSizeBak = 0;
	}

	
}; 

typedef IMAGE* LPIMAGE;

#if !defined(__ACCOUNTSERVER) && !defined(__CACHESERVER) && !defined(__CERTIFIER) && !defined(__CORESERVER) && !defined(__DBSERVER) && !defined(__LOGINSERVER)
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
#endif
void PaintTexture( LPVOID pDestData, LPIMAGE pImage, CPoint pt, CSize sizeSurface, D3DFORMAT d3dFormat );
void AdjustSize( SIZE* pSize );
BOOL LoadImage( LPCTSTR lpszFileName, LPIMAGE lppImage );//LPBYTE* lppData, SIZE* pSize, int* pnImgBit );
BOOL LoadBMP( LPCTSTR lpszFileName, LPIMAGE lppImage );
BOOL LoadTGA( LPCTSTR lpszFileName, LPIMAGE lppImage );
BOOL SaveBMP( LPCTSTR lpszFileName, LPBYTE lpData, SIZE size );
#if !defined(__ACCOUNTSERVER) && !defined(__CACHESERVER) && !defined (__CERTIFIER) && !defined(__CORESERVER) && !defined(__DBSERVER) && !defined(__LOGINSERVER)
void GetPickRay( CRect rect,POINT ptCursor,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir );
void GetRayEnd( D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd );
#endif

//-----------------------------------------------------------------------------
// Name: IntersectTriangle()
// Desc: orig에서 쏜 d 광선이 v0,v1,v2 삼각형 안에 속하는지를 확인하고,
//       속하고 있다면 삼각형과의 교차점을 pIntersect에 돌려준다.
//       또한 삼각형과의 거리를 fDist에 돌려준다.
//-----------------------------------------------------------------------------

#if !defined(__ACCOUNTSERVER) && !defined(__CACHESERVER) && !defined (__CERTIFIER) && !defined(__CORESERVER) && !defined(__DBSERVER) && !defined(__LOGINSERVER)
BOOL IntersectTriangle(D3DXVECTOR3& v0,D3DXVECTOR3& v1, D3DXVECTOR3& v2, const D3DXVECTOR3& orig,const D3DXVECTOR3& d, D3DXVECTOR3* pIntersect,FLOAT* pfDist );
#endif
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
inline int FtoInt( FLOAT f ) { return (int)( f + 0.5f ); }


inline BOOL isdigit2(CHAR c)
{
	if( c >= '0' && c <= '9' )
		return TRUE;
	else
		return FALSE;
}

typedef struct _ERROR_STATE
{
	DWORD	m_dwWndId;
	BOOL	m_bIsShowWnd;
	CHAR	m_ch;
} _ERROR_STATE;

extern _ERROR_STATE g_Error_State;

BOOL IsInteger( const char* pszSource );

#endif
