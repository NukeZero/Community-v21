//-----------------------------------------------------------------------------
// File: D3DUtil.cpp
//
// Desc: Shortcut macros and functions for using DX objects
//
//
// Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#include "stdafx.h"
#define STRICT
//#ifdef _BEAST
#include "stdafx.h"
#include <wingdi.h>
//#endif
#include <tchar.h>
#include <stdio.h>
//#include "targa.h"
#include "File.h"
//#include "Debug.h"
#if !defined(__SERVER)
//	#include "targa.h"
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
		return IsDBCSLeadByte( (BYTE)( *pwSrc ) );
	}
	else
	if( g_codePage == 936 ) // 한자 : 중국
	{
		return IsDBCSLeadByte( (BYTE)( *pwSrc ) );
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
	int i = 0;
	for( ; i < nLength; )
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
	int i = 0;
	for( ; i < nLength; )
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
	int i = 0;
	for( ; i < nNullLength; )
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
	int i = 0;
	for( ; i < nNullLength; )
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
	for( int i = 0; i < 16; i++ )
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
	_strlwr( szFileName );
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


#define __NEWINTERSECT
