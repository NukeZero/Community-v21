//-----------------------------------------------------------------------------
// File: D3DUtil.cpp
//
// Desc: Shortcut macros and functions for using DX objects
//
//
// Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#define STRICT
//#ifdef _BEAST
#include "stdafx.h"
#include <wingdi.h>
//#endif
#include <tchar.h>
#include <stdio.h>
#include "D3DX9.h"
//#include "targa.h"
#include "File.h"
#include "Debug.h"
#if !defined(__SERVER)
	#include "targa.h"
#endif

static int CALLBACK FontEnumProc(const LOGFONT*, const TEXTMETRIC*, DWORD, LPARAM lParam)
{
	if (lParam != NULL)
	{
		*(BOOL*)lParam = TRUE;
	}
	return 0;
}
BOOL IsFontInstalled( LPCTSTR pszFace )
{
	BOOL bInstalled;
	HDC hDC;
	LOGFONT lf;

	memset(&lf, 0, sizeof(lf));

	lstrcpy(lf.lfFaceName, pszFace);
	lf.lfCharSet = DEFAULT_CHARSET;

	bInstalled = FALSE;
	hDC = ::GetDC(NULL);
	if (hDC != NULL)
	{
		::EnumFontFamiliesEx(hDC, &lf, FontEnumProc, (LPARAM)&bInstalled, 0);
		::ReleaseDC(NULL, hDC);
	}

	return bInstalled;
}

int GetCharLen( const CHAR* pStr )
{
	char* pNext = CharNext( pStr );//CP_ACP, pStr, 0 );
	return (int) (pNext - pStr );
}

// 외국어포함 문장의 길이를 잘라준다.
int GetStrCut( const CHAR* pSrc, CHAR* pDest, int nCount )
{
	int nOffset = 0;
	int nLen;
	
	while( nCount )
	{
		nLen = GetCharLen( &pSrc[ nOffset ]  );
		nOffset += nLen;
		nCount--;
	}
	
	memcpy( pDest, pSrc, sizeof(char)*nOffset);
	
	return 1;
}	

// 외국어포함 문자 갯수를 알아냄
int GetStrLen( const CHAR* pSrc )
{
	int nCount = 0;
	int nOffset = 0;
	int nLen;
	
	while( pSrc[ nOffset ] != NULL )
	{
		nLen = GetCharLen( &pSrc[ nOffset ]  );
		nOffset += nLen;
		nCount++;
	}
	
	return nCount;
}	


#ifdef _MULTI_LANGUAGE_

int CopyChar( const char* pSrc, char* pDest )
{
	const char* pNext = CharNextEx(pSrc);

	memcpy(pDest, pSrc, pNext - pSrc);

	return pNext - pSrc;
}

BOOL IsMultiByte( const char* pSrc )
{
	return ( CharNextEx(pSrc) - pSrc ) > 1;
}

BOOL IsNative( LPCTSTR lpszStr )
{
	ASSERT( g_codePage != 0 );

	LPCWSTR pwSrc = (LPCWSTR) lpszStr;

	if( g_codePage == 874 ) // 타이 
	{
		return (BYTE)*lpszStr >= 0xa1 && (BYTE)*lpszStr <= 0xfb;
	}
	else
	if( g_codePage == 949 ) // 한글 
	{
		return IsHangul( *pwSrc );
	}
	else
	if( g_codePage == 932 ) // 일본 
	{
		return IsDBCSLeadByte( *pwSrc );
	}
	else
	if( g_codePage == 936 ) // 한자 : 중국
	{
		return IsDBCSLeadByte( *pwSrc );
	}
	else
	if( g_codePage == 950 ) // 한자 : 대만 
	{
//		return IsDBCSLeadByte( *pwSrc );

		if( ((BYTE)*lpszStr >= 0xCA && (BYTE)*lpszStr <= 0xFD) && ( (BYTE)*lpszStr+1 >= 0xA1 ) && ( (BYTE)*lpszStr+1 <= 0xFE) )
		{
			return TRUE;
		}
		else
		if ( ( ( (BYTE)*lpszStr >= 0x41 ) && 
		 	 ( (BYTE)*lpszStr <= 0x5A ) ) || 
		   	( ( (BYTE)*lpszStr >= 0x61 ) && ( (BYTE)*lpszStr <= 0x7A) ) ) 	
		{ 
			return TRUE;			
		} 
		else
		if( isdigit( (BYTE)*lpszStr ) )
			return TRUE;
		
	}
	return FALSE;
}
#endif

BOOL IsHangul( WORD word )
{
	//
	//if( word >= 0x8141 && word <= 0xc6fe)
	//	return TRUE;
	//if( word >= 0xb0a1 && word <= 0xc8fe )
	//	return TRUE;
		
	BYTE l = word & 0xff;
	BYTE h = word >> 8;

	//if (0xa1 > l || 0xfe < l ) return FALSE;
	//if (0xb0 <= h && 0xc8 >= h ) return TRUE;
	//return FALSE;

	

	// 특수 문자, 일어 기타 언어 FALSE
	// 특수 문자, 일어 기타 언어 FALSE
	//if( h >= 0xa1 && h <= 0xac && l >= 0xa0 && l <= 0xaf )
	if( h >= 0xa1 && h <= 0xac && l >= 0xa0 && l <= 0xff )
		return FALSE;
	// 확장 완성형  
	if( h >= 0x81 && h <= 0xc6 && l >= 0x41 && l <= 0xfe )
	//( h >= 0x81 && h <= 0xc6 && l >= 0x41 && l <= 0x5a )
		return TRUE;
	// 완성형 코드 
	if( h >= 0xb0 && h <= 0xc8 && l >= 0xa1 && l <= 0xfe )
		return TRUE;
	return FALSE;
} 

void RepairString( TCHAR* lpStr )
{
	int nLength = strlen( lpStr );// - 1;
	for( int i = 0; i < nLength; )
	{
#ifdef __CLIENT
#ifdef __THAILAND__
		if( g_codePage == 874 ) // 타이 
			i++;
		else
#endif//__THAILAND__
#endif//__CLIENT
		if( IsDBCSLeadByte( lpStr[ i ] ) )
			i += 2;
		else
			i++;
	}
	// i가 nLength 보다 크다면 Word캐릭터일 것이고, 끝부분이 깨져서 오차가 생긴 것이다.
	if( i > nLength )
		lpStr[ i - 2 ] = 0;
	else
		lpStr[ i ] = 0;
}
void RepairString( CString& string )
{
	int nLength = string.GetLength();// - 1;
	for( int i = 0; i < nLength; )
	{
#ifdef __CLIENT
	#ifdef __THAILAND__
		if( g_codePage == 874 ) // 타이 
			i++;
		else
	#endif //__THAILAND__
#endif//__CLIENT
		if( IsDBCSLeadByte( string[ i ] ) )
			i += 2;
		else
			i++;
	}
	// i가 nLength 보다 크다면 Word캐릭터일 것이고, 끝부분이 깨져서 오차가 생긴 것이다.
	if( i > nLength )
		string = string.Left( i - 2 );
		//lpStr[ i - 2 ] = 0;
	else
		string = string.Left( i );
		//lpStr[ i ] = 0;
	
//	string = string.Left( i );
}
void SetStrNull( TCHAR* lpStr, int nNullLength )
{
	int nLength = strlen( lpStr );
	// 0을 넣을 포지션이 실제 스트링 길이보다 길면 실제 스트링 길이로 맞출 필요가 있음
	if( nNullLength > nLength )
		nNullLength = nLength;
	for( int i = 0; i < nNullLength; )
	{
#ifdef __CLIENT
	#ifdef __THAILAND__
		if( g_codePage == 874 ) // 타이 
			i++;
		else
	#endif
#endif//__CLIENT
		if( IsDBCSLeadByte( lpStr[ i ] ) )
			i+=2;
		else
			i++;
	}
	// i가 nLength 보다 크다면 Word캐릭터일 것이고, 끝부분이 깨져서 오차가 생긴 것이다.
	if( i > nNullLength )
		lpStr[ i - 2 ] = 0;
	else
		lpStr[ i ] = 0;
}
void SetStrNull( CString& string, int nNullLength )
{
	int nLength = string.GetLength();
	// 0을 넣을 포지션이 실제 스트링 길이보다 길면 실제 스트링 길이로 맞출 필요가 있음
	if( nNullLength > nLength )
		nNullLength = nLength;
	for( int i = 0; i < nNullLength; )
	{
#ifdef __CLIENT
	#ifdef __THAILAND__
		if( g_codePage == 874 ) // 타이 
			i++;
		else
	#endif
#endif//__CLIENT
		if( IsDBCSLeadByte( string[ i ] ) )
			i+=2;
		else
			i++;
	}
	// i가 nLength 보다 크다면 Word캐릭터일 것이고, 끝부분이 깨져서 오차가 생긴 것이다.
	if( i > nNullLength )
		string = string.Left( i - 2 );
	else
		string = string.Left( i );
}
HRESULT LoadTextureFromRes( 
	LPDIRECT3DDEVICE9 pd3dDevice, 
	LPCTSTR strTexture,
	LPDIRECT3DTEXTURE9* ppTexture,
	DWORD MipFilter,
	D3DFORMAT d3dFormat 
	)
{

    return LoadTextureFromRes( pd3dDevice, strTexture, 
		D3DX_DEFAULT, D3DX_DEFAULT, MipFilter, 0, d3dFormat, 
		D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, ppTexture );
}

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
	)
{
	HRESULT hr = E_FAIL;
#if	defined (__WORLDSERVER) || defined (__CLIENT)  || defined (__BEAST) || defined (__DAISY ) || defined (__SFXEDITOR)
	CResFile file;
	if( file.Open( pFileName, "rb" ) == FALSE )
	{
#if !defined(__SINGLE)
		LPCTSTR szErr = Message( "::LoadTextureFromRes : %s not found", pFileName );
		ADDERRORMSG( szErr );
#endif
		return E_FAIL;
	}
	
	int nSrcDataSize = file.GetLength();
	LPBYTE pSrcData = new BYTE[ nSrcDataSize ];
	file.Read( pSrcData, nSrcDataSize );
	
	hr = D3DXCreateTextureFromFileInMemoryEx( pDevice,
		pSrcData, nSrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture );

	SAFE_DELETE( pSrcData );
#endif	//	defined (__WORLDSERVER) || (__CLIENT)	
	return hr;
}
#ifdef __V050322_CACHEQUEST
void PaintTexture( LPVOID pDestData, LPIMAGE pImage, CPoint pt, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	SIZE sizeTexture = pImage->size;
	DWORD dwOffsetSrc = 0;
	DWORD dwOffsetDest = ( pt.y * sizeSurface.cx + pt.x );
	DWORD dwLine = sizeSurface.cx - sizeTexture.cx;
	int nSizeSurface = sizeSurface.cx * sizeSurface.cy;
	if( d3dFormat == D3DFMT_A4R4G4B4 )
	{
		LPWORD pDest = (LPWORD) pDestData;
		LPBYTE pSrc = pImage->lpData;
		if( pImage->nImgBit == IMGBIT_24 )
		{
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 3, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						BYTE byData1 = pSrc[ dwOffsetSrc + 0 ] >> 4; 
						BYTE byData2 = pSrc[ dwOffsetSrc + 1 ] >> 4; 
						BYTE byData3 = pSrc[ dwOffsetSrc + 2 ] >> 4; 
						// 원래 있던 배경을 보존하기 위해서 ff00ff면 찍지 않는다.
						if( byData1 != 0xf || byData2 != 0 || byData3 != 0xf )
							pDest[ dwOffsetDest ] = 0xf000 | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
					}
				}
			}
		}
		else
		if( pImage->nImgBit == IMGBIT_32 )
		{
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 4, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						BYTE byData1 = pSrc[ dwOffsetSrc + 0 ] >> 4; 
						BYTE byData2 = pSrc[ dwOffsetSrc + 1 ] >> 4; 
						BYTE byData3 = pSrc[ dwOffsetSrc + 2 ] >> 4; 
						BYTE byData4 = pSrc[ dwOffsetSrc + 3 ] >> 4; 
						// 원래 있던 배경을 보존하기 위해서 알파값이 전혀 없으면 찍지 않는다.
						if( byData4 )
							pDest[ dwOffsetDest ] = ( byData4 << 12 ) | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
					}
				}
			}
		}
	}
	else
	if( d3dFormat == D3DFMT_A8R8G8B8 )
	{
		LPDWORD pDest = (LPDWORD) pDestData;
		if( pImage->nImgBit == IMGBIT_24 )
		{
			LPBYTE pSrc = pImage->lpData;
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 3, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						BYTE byData1 = pSrc[ dwOffsetSrc + 0 ]; 
						BYTE byData2 = pSrc[ dwOffsetSrc + 1 ]; 
						BYTE byData3 = pSrc[ dwOffsetSrc + 2 ]; 
						// 원래 있던 배경을 보존하기 위해서 ff00ff면 찍지 않는다.
						if( byData1 != 0xff || byData2 != 0 || byData3 != 0xff )
							pDest[ dwOffsetDest ] = 0xf000 | ( byData3 << 16 ) | ( byData2 << 8 ) | byData1;
					}
				}
			}
		}
		else
		if( pImage->nImgBit == IMGBIT_32 )
		{
			LPDWORD pSrc = (LPDWORD)pImage->lpData;
			for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
			{
				for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc++, dwOffsetDest++ )
				{
					if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
					{
						// 원래 있던 배경을 보존하기 위해서 알파값이 전혀 없으면 찍지 않는다.
						if( pSrc[ dwOffsetSrc ] & 0xff000000 )
							pDest[ dwOffsetDest ] = pSrc[ dwOffsetSrc ];//yData4 << 24 ) | ( byData3 << 16 ) | ( byData2 << 8 ) | byData1;
					}
				}
			}
		}
	}
}
#else // __V050322_CACHEQUEST
//{{AFX
void PaintTexture( LPWORD pDest, LPIMAGE pImage, CPoint pt, CSize sizeSurface )
{
	SIZE sizeTexture = pImage->size;
	LPBYTE pSrc = pImage->lpData;
	DWORD dwOffsetSrc = 0;
	DWORD dwOffsetDest = ( pt.y * sizeSurface.cx + pt.x );
	DWORD dwLine = sizeSurface.cx - sizeTexture.cx;
	int nSizeSurface = sizeSurface.cx * sizeSurface.cy;
	if( pImage->nImgBit == IMGBIT_24 )
	{
		for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
		{
			for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 3, dwOffsetDest++ )
			{
				if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
				{
					BYTE byData1 = pSrc[ dwOffsetSrc + 0 ] >> 4; 
					BYTE byData2 = pSrc[ dwOffsetSrc + 1 ] >> 4; 
					BYTE byData3 = pSrc[ dwOffsetSrc + 2 ] >> 4; 
					if( byData1 == 15 && byData2 == 0 && byData3 == 15 )
					{
						//pDest[ dwOffsetDest  ] = 0;
					}
					else
					{
						WORD wColor = 0xf000 | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
						pDest[ dwOffsetDest ] = wColor;
					}
				}
			}
		}
	}
	else
	for( int y = pt.y; y < pt.y + sizeTexture.cy; y++, dwOffsetDest += dwLine )
	{
		for( int x = pt.x; x < pt.x + sizeTexture.cx; x++, dwOffsetSrc += 4, dwOffsetDest++ )
		{
			if( dwOffsetDest >= 0 && (DWORD)dwOffsetDest < (DWORD)nSizeSurface )
			{
				BYTE byData1 = pSrc[ dwOffsetSrc + 0 ] >> 4; 
				BYTE byData2 = pSrc[ dwOffsetSrc + 1 ] >> 4; 
				BYTE byData3 = pSrc[ dwOffsetSrc + 2 ] >> 4; 
				BYTE byData4 = pSrc[ dwOffsetSrc + 3 ] >> 4; 
				if( byData4 )
				{
					WORD wColor = ( byData4 << 12 ) | ( byData3 << 8 ) | ( byData2 << 4 ) | byData1;
					pDest[ dwOffsetDest ] = wColor;
				}
			}
		}
	}
}
//}}AFX
#endif // __V050322_CACHEQUEST
void AdjustSize( SIZE* pSize )
{
	// 사이즈를 2의 자승으로 바꾸기 
	WORD dwTemp = 0x8000;
	for( int i = 0; i < 16; i++ )
	{
		if( pSize->cx & dwTemp )
		{
			if( pSize->cx % dwTemp )
			{
				dwTemp <<= 1;
				pSize->cx = dwTemp;
			}
			break;
		}
		dwTemp >>= 1;
	}
	dwTemp = 0x8000;
	for( i = 0; i < 16; i++ )
	{
		if( pSize->cy & dwTemp )
		{
			if( pSize->cy % dwTemp )
			{
				dwTemp <<= 1;
				pSize->cy = dwTemp;
			}
			break;
		}
		dwTemp >>= 1;
	}
	//return size;
}
BOOL LoadImage( LPCTSTR lpszFileName, LPIMAGE lpImage ) //LPBYTE* lppData, SIZE* pSize, int* pnImgBit )
{
	TCHAR szFileName[ 128 ];
	strcpy( szFileName, lpszFileName );
	strlwr( szFileName );
	if( strstr( szFileName, ".tga" ) )
	{
		return LoadTGA( lpszFileName, lpImage );
	}
	else
	if( strstr( szFileName, ".bmp" ) )
	{
		return LoadBMP( lpszFileName, lpImage );
	}				
	return FALSE;
}
BOOL LoadTGA( LPCTSTR lpszFileName, LPIMAGE lpImage ) //LPBYTE* lppData, SIZE* pSize )
{
#if !defined(__SERVER)
	lpImage->nImgBit = IMGBIT_32;
	CTarga targa;
	return targa.Load( lpszFileName, &lpImage->lpData, &lpImage->size );
#endif
	return FALSE;
}

BOOL LoadBMP( LPCTSTR lpszFileName, LPIMAGE lpImage ) //LPBYTE* lppData, SIZE* pSize )
{
	//DEBUGOUT( "LoadBMP %s\n",lpszFileName );
	CResFile file;
	if( file.Open( lpszFileName, "rb" ) == FALSE )
		return FALSE;

	BITMAPINFOHEADER infoHeader;
	/*
	DEBUGOUT( "Handle %d\n",file.fp );
	DEBUGOUT( "Handle %d\n",file.Handle() );
	DEBUGOUT( "Length %d\n",_filelength64(file.Handle()) );
	DEBUGOUT( "Length %d\n",file.GetLength() );
	*/
	LPBYTE lpData = (LPBYTE)file.Read();
	LPBYTE lpDataDelete = lpData;
	lpData += sizeof( BITMAPFILEHEADER );
	memcpy( &infoHeader, lpData, sizeof( BITMAPINFOHEADER ) );
	lpData += sizeof( BITMAPINFOHEADER );

	int bitCount = 1;
	if( infoHeader.biBitCount < 8)
	{
		lpImage->size.cx = 0;
		lpImage->size.cy = 0;
		lpImage->lpData = NULL;
		lpImage->nImgBit = IMGBIT_24;
		delete lpDataDelete;
		return FALSE;
	}
	if( infoHeader.biBitCount > 8)
	{
		bitCount = infoHeader.biBitCount / 8;
	}
	if( infoHeader.biCompression)
	{
		lpImage->size.cx = 0;
		lpImage->size.cy = 0;
		lpImage->lpData = NULL;
		lpImage->nImgBit = IMGBIT_24;
		delete lpDataDelete;
		return FALSE;
	}
	int nLgHeight = infoHeader.biHeight;
	int nLgWidth  = infoHeader.biWidth;

	lpImage->size.cx = nLgWidth;
	lpImage->size.cy = nLgHeight;

	if( bitCount == 1 ) 
	{
		if( infoHeader.biClrUsed == 0 )
			infoHeader.biClrUsed = 256;
		//if(pPal != NULL) 
		//{
		//	RGBQUAD rgbquad[256];
		//	memcpy( rgbquad, lpData, infoHeader.biClrUsed * 4 );
			//pPal->CreateIdPalette( rgbquad, infoHeader.biClrUsed );
		//}
		lpData += infoHeader.biClrUsed * 4;
	}
	//else
	//	pPal = NULL;

	LPBYTE lpNewData = new BYTE[ nLgWidth * nLgHeight * bitCount * 2 ]; // 버그 때문에 2 곱했음. 사이즈가 4의 배수가 아니어서 문제 생겨서 * 2 했음 

	//CreateDIBSection( pDc, pPal, nLgWidth, -nLgHeight, m_infoHeader.biBitCount);	

	// CreateDibSection을 거치면서 m_infoHeader.biWidth는 물리적 좌표로 변환한다.
	int nPgHeight = abs( infoHeader.biHeight );
	int nPgWidth  = infoHeader.biWidth;

	nPgWidth *= bitCount;
	nLgWidth *= bitCount;
	nLgWidth += ( ( nLgWidth % 4 ) ? ( 4 - nLgWidth % 4 ) : 0 );

	LPBYTE pTempDst = lpNewData + ( nPgWidth * ( nPgHeight - 1 ) );

	for( int i = 0; i < nPgHeight; i++ )
	{
		memcpy( pTempDst, lpData, nLgWidth );
		lpData += nLgWidth;
		pTempDst -= nPgWidth;
	}
	delete lpDataDelete;
	lpImage->lpData = lpNewData;
	lpImage->nImgBit = IMGBIT_24;
	return TRUE;
}

BOOL SaveBMP( LPCTSTR lpszFileName, LPBYTE lpData, SIZE size )
{
	int i, j;
	BITMAPFILEHEADER BMPheader;
	BITMAPINFOHEADER header = { 40,640,-480,1,24,0,0,5904,5904,0,0 };
	header.biBitCount = 24;
	header.biWidth  = size.cx;
	header.biHeight = -abs(size.cy);

	LPBYTE pBMP = NULL;
	LPBYTE pp;

	LPBYTE pSurface = (LPBYTE) lpData;

	int nBit = 24 >> 3;//m_infoHeader.biBitCount >> 3;

	BMPheader.bfType = 'M';
	BMPheader.bfType <<= 8;
	BMPheader.bfType += 'B';
	BMPheader.bfReserved1 = 0; 
	BMPheader.bfReserved2 = 0;
	// 16비트(nBit==2)라도 팔레트가 있는 것 처럼 안하면 그림이 밀려서 출력한다. 아케인 캡춰 관련 버그 
	if(nBit == 1 || nBit == 2)
	{
		BMPheader.bfSize = ( sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + header.biWidth * abs(header.biHeight) * nBit );
		BMPheader.bfOffBits = sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
	}
	else
	{
		BMPheader.bfSize = ( sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER) + header.biWidth * abs(header.biHeight) * nBit );
		BMPheader.bfOffBits = sizeof(BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER);
	}
 	pBMP = new BYTE[BMPheader.bfSize];
	if( pBMP == NULL )	return	FALSE;
	pp = pBMP;

	header.biHeight = abs( header.biHeight);

	memcpy( pp, &BMPheader, sizeof(BITMAPFILEHEADER) ); pp += sizeof(BITMAPFILEHEADER);
	memcpy( pp, &header, sizeof(BITMAPINFOHEADER) ); pp += sizeof(BITMAPINFOHEADER);

	if(nBit == 1 || nBit == 2)
	{
		//if(rgbQuad)
		//	memcpy(pp,rgbQuad,sizeof(RGBQUAD)*256); 
		//pp += (sizeof(RGBQUAD) * 256);
	}

	j = header.biWidth * abs(header.biHeight) * nBit;


	for( i = 0; i < abs(header.biHeight); i ++ )
	{
		j  -= header.biWidth * nBit;
		memcpy( pp, pSurface + j, header.biWidth * nBit );		
		pp += header.biWidth * nBit;
	}
	//memset(pp2,0xff,800*2);
	//memset(pp2+800*2,0xff,800*2);

	CFile file;
	if(file.Open(lpszFileName,CFile::modeWrite|CFile::modeCreate))
	{
		file.Write(pBMP,(int)(pp - pBMP));
		delete pBMP;
		return TRUE;
	}

	delete pBMP;
	return FALSE; 
}
#if	defined (__WORLDSERVER) || defined (__CLIENT) || defined (__BEAST)
void GetPickRay(CRect rect,POINT ptCursor,D3DXMATRIX* pmatProj,D3DXMATRIX* pmatView,D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir)
{
//	HRESULT hr;

	//CRect rect;
	//GetClientRect(&rect);

	// Compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / rect.Width()  ) - 1 ) / pmatProj->_11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / rect.Height() ) - 1 ) / pmatProj->_22;
	v.z =  1.0f;

	// Get the inverse view matrix
	D3DXMATRIX m;
	//m_pd3dDevice->GetTransform( D3DTS_VIEW, &g_matView );
	D3DXMatrixInverse( &m, NULL, pmatView );

	// Transform the screen space pick ray into 3D space
	pvPickRayDir->x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	pvPickRayDir->y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	pvPickRayDir->z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	pvPickRayOrig->x = m._41;
	pvPickRayOrig->y = m._42;
	pvPickRayOrig->z = m._43;
	//TRACE("RayOrg (%f,%f,%f) RayDir(%f,%f,%f)\n",vPickRayOrig.x,vPickRayOrig.y,vPickRayOrig.z,vPickRayDir.x,vPickRayDir.y,vPickRayDir.z);
}
#endif	//defined (__WORLDSERVER) || defined (__CLIENT) || defined (__BEAST)
void GetRayEnd(D3DXVECTOR3* pvPickRayOrig,D3DXVECTOR3* pvPickRayDir,D3DXVECTOR3* pvPickRayEnd)
{
	FLOAT fyOrig = pvPickRayOrig->y;
	FLOAT fyDir  = -pvPickRayDir->y;
	pvPickRayDir->x = fyOrig * pvPickRayDir->x / fyDir; // vecY : posY = vecX : posX
	pvPickRayDir->z = fyOrig * pvPickRayDir->z / fyDir; // vecY : posY = vecZ : posZ
	pvPickRayDir->y = -fyOrig; // 아래 *pvPickRayDir + *pvPickRayOrig에서 y가 0이 나오게 하려면 -fyOrig를 넣어야 한다.
	//pvPickRayDir->y = fyOrig * m_vPickRayDir.y / fyDir;
	*pvPickRayEnd = *pvPickRayDir + *pvPickRayOrig;
	//PickRayEnd->y = 0;
}

#define __NEWINTERSECT
//#define __DIRECTXINTERSECT
#if	defined (__WORLDSERVER) || defined (__CLIENT) || defined (__BEAST) || defined(__XTOOL)
	#ifdef __NEWINTERSECT
BOOL IntersectTriangle( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
			const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3* pIntersect, FLOAT* pfDist, BOOL bCull )
                                       //FLOAT* t, FLOAT* u, FLOAT* v )
{
	if( bCull == FALSE )
	{
		FLOAT fU,fV;
		if(D3DXIntersectTri( &v0, &v1, &v2, &orig, &dir, &fU, &fV, pfDist ) == TRUE )
		{
			*pIntersect = orig + *pfDist * dir;
			return TRUE;
		}
		return FALSE;
	}
    // Find vectors for two edges sharing vert0
	FLOAT t,u,v;
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
		// 반대 방향. cull ok
		return FALSE;
		// 뒤집는다. 
        //tvec = v0 - orig;
        //det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( &dir, &qvec );
    if( v < 0.0f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

	*pIntersect = orig + t * dir;
    return TRUE;
}
#else
// 이것저것 방법을 동원해본 버전.  
	
BOOL IntersectTriangle(D3DXVECTOR3& v0,D3DXVECTOR3& v1, D3DXVECTOR3& v2, const D3DXVECTOR3& orig,const D3DXVECTOR3& d, D3DXVECTOR3* pIntersect,FLOAT* pfDist)
{
	// 밑에 구하는 코드가 있으나 엄청 느리기 때문에 최적화 된 DX 함수를 먼저 실행해서 교차 여부를 확인한다. 
	// 교차가 성공적이면 아래 코드를 실행하여 좌표를 구한다.
	FLOAT fU,fV;
	if(D3DXIntersectTri(&v0,&v1,&v2,&orig,&d,&fU,&fV,pfDist) == FALSE)
		return FALSE;

	// Find vectors for two edges sharing vert0
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	D3DXVECTOR3 end, dir = d;

	// pfDist는 목표와의 거리. 이 변수에 2를 곱하면 두배 end값을 만들 수 있다.
	// 목표 거리가 필요. 또는 그 방향으로 지나는 목표값이어도 된다.
	end = (d * (*pfDist * 2)) + orig;
	dir = end - orig;

	// 법선 벡터를 구하자 
	D3DXVECTOR3 normal, L1;
	D3DXVec3Cross( &normal, &edge1, &edge2 );

	// dot는 백터와 거리 값을 입력. 백터와 백터, 거리와 거리는 안됨 
	FLOAT fLinelength = D3DXVec3Dot(&dir,&normal); // This gives us the line length (the blue dot L3 + L4 in figure d)

	if (fabsf(fLinelength) < 0.00001f) // check it does not = 0 with tolerance for floating point rounding errors
		return FALSE; //=0 means the line is parallel to the plane so can not intersect it

	L1 = v0 - orig;// calculate vector L1 (the PINK line in figure d)
	FLOAT fDist = D3DXVec3Dot(&L1,&normal); // gives the distance from the plane (ORANGE Line L3 in figure d)
	FLOAT fPercentage = fDist / fLinelength; // How far from Linestart , intersection is as a percentage of 0 to 1 

	if (fPercentage < 0) // The plane is behind the start of the line
		return FALSE;
	else
	if (fPercentage > 1) // The line does not reach the plane
		return FALSE;

	*pIntersect = orig + (dir * fPercentage);

	return TRUE;
	/*
	D3DXVECTOR3 vecDist = orig - *pIntersect;
	*pfDist = sqrt( (vecDist.x*vecDist.x)+(vecDist.y*vecDist.y)+(vecDist.z*vecDist.z) );
	
	// 구한 교차 포인트가 삼각형 안에 속하는지 확인 
	// 교차 점이 삼각형 안에 있는지 여부를 확인하게 위해 교차점과 세개의 꼭지점의 각도를 더해 
	// 360도 또는 라디안으로 6.28이 되는지를 확인한다. 360도일 경우 내부에 포함되어 있다.

	D3DXVECTOR3 Line0,Line1,Line2;
	float total_angles,result,vlength;

	Line0 = *pIntersect - v0;
	vlength = sqrt( (Line0.x*Line0.x)+(Line0.y*Line0.y)+(Line0.z*Line0.z) ); // Nomralize Vectors
	Line0 = Line0 / vlength;

	Line1 = *pIntersect - v1;
	vlength = sqrt( (Line1.x*Line1.x)+(Line1.y*Line1.y)+(Line1.z*Line1.z) ); // Nomralize Vectors
	Line1 = Line1 / vlength;

	Line2 = *pIntersect - v2;
	vlength = sqrt( (Line2.x*Line2.x)+(Line2.y*Line2.y)+(Line2.z*Line2.z) ); // Nomralize Vectors
	Line2 = Line2 / vlength;

	total_angles = 0.0f; // lets start couting those angles

	total_angles += acos(D3DXVec3Dot(&Line0,&Line1)); 
	total_angles += acos(D3DXVec3Dot(&Line1,&Line2)); 
	total_angles += acos(D3DXVec3Dot(&Line2,&Line0)); 

	if (total_angles > 0 && fabsf (total_angles-6.28)<0.01) // lets take 6.28 radians(360 degrees) away from total_angles and if its Zero (with tolerence) weve hit it
		return TRUE;

	return FALSE;
	*/
}
	#endif
#endif	//defined (__WORLDSERVER) || defined (__CLIENT) || defined (__BEAST)

#ifndef __WORLDSERVER
	#if	defined (__CLIENT)
HRESULT D3DUtil_CreateMipTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR strTexture,
                                  LPDIRECT3DTEXTURE9* ppTexture, D3DFORMAT d3dFormat, D3DCOLOR d3dKeyColor )
{
    HRESULT hr;
    TCHAR strPath[MAX_PATH];

    // Get the path to the texture
    if( FAILED( hr = DXUtil_FindMediaFileCb( strPath, sizeof(strPath), (TCHAR*)strTexture ) ) )
        return hr;

    // Get the path to the texture
//    TCHAR strPath[MAX_PATH];
  //  DXUtil_FindMediaFile( strPath, strTexture );


	  D3DXIMAGE_INFO pSrcInfo;

    // Create the texture using D3DX
    return D3DXCreateTextureFromFileEx( pd3dDevice, strPath, 
                D3DX_DEFAULT, D3DX_DEFAULT, 6, 0, d3dFormat, 
                D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, d3dKeyColor, &pSrcInfo, NULL, ppTexture );
}
	#endif	//defined (__CLIENT)
#endif // not worldserver

