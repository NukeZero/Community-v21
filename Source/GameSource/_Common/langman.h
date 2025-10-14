
#pragma once

#include "lang.h"


//---------------------------------------------------------------------------------------------
// 언어 폰트 구조체 
//---------------------------------------------------------------------------------------------
typedef struct stLANG_FONT
{
	CHAR	nDivTitle;
	CHAR	nDivCaption;
	CHAR	szFontFirst[32];
	CHAR	szFontSecond[32];

	stLANG_FONT()
	:	nDivTitle( NULL )
	,	nDivCaption( NULL )
	{
		ZeroMemory( szFontFirst, sizeof(CHAR) * _countof(szFontFirst) );
		ZeroMemory( szFontSecond, sizeof(CHAR) * _countof(szFontSecond) );
	}
}LANG_FONT, *PLANG_FONT;


//---------------------------------------------------------------------------------------------
// 폰트 데이터 구조체 
//---------------------------------------------------------------------------------------------
typedef struct stFONT_DATA
{
	int		charset;
	WORD	wCodePage;
	char	faceNT[32];
	char	face9x[32];

	stFONT_DATA()
	:	charset( NULL )
	,	wCodePage( NULL )
	{
		ZeroMemory( faceNT, sizeof(char) * _countof(faceNT) );
		ZeroMemory( face9x, sizeof(char) * _countof(face9x) );
	}
}FONT_DATA, *PFONT_DATA;


//---------------------------------------------------------------------------------------------
// 폰트 정보 구조체 
//---------------------------------------------------------------------------------------------
typedef	struct	stFONT_INFO
{
	DWORD	dwColor;
	DWORD	dwBgColor;
	DWORD	nOutLine;
	DWORD	dwFlags;
	TCHAR	szFont[32];
	DWORD	dwFontSize;

	stFONT_INFO()
	:	dwColor( 0xFFFFFFFF )
	,	dwBgColor( 0xFF000000 )	
	,	nOutLine( NULL )	
	,	dwFlags( 0L )
	,	dwFontSize( 9 )
	{
		ZeroMemory( szFont, sizeof(TCHAR) * _countof(szFont) );
	}
}FONT_INFO, *PFONT_INFO;


//---------------------------------------------------------------------------------------------
// NEUZ 폰트 구조체 
//---------------------------------------------------------------------------------------------
typedef	struct	stNEUZ_FONT
{
	LANG_FONT			lfCaption;		// Caption
	FONT_DATA			fdLang;			// Language Data
	std::vector<FONT_INFO>	vecFontInfo;	
	
	stNEUZ_FONT()
	{
	}

	~stNEUZ_FONT()
	{
		vecFontInfo.clear();
	}
}NEUZ_FONT, *PNEUZ_FONT;


//---------------------------------------------------------------------------------------------
// NEUZ 폰트 구조체 
//---------------------------------------------------------------------------------------------
typedef	struct	_LANG_DATA
{
	NEUZ_FONT	stFont;
	char		szPath[MAX_PATH];		// "Korean\\"
	char		szFileName[MAX_PATH];	// "_TWN.inc"
	char		szTitle[32];			// worldserver

	_LANG_DATA()
	{
		ZeroMemory( szPath,		sizeof(char) * MAX_PATH );
		ZeroMemory( szFileName, sizeof(char) * MAX_PATH );
		ZeroMemory( szTitle,	sizeof(char) * 32		);

		szPath[0]		= '\0';
		szFileName[0]	= '\0';
		szTitle[0]		= '\0';
	}
}LANG_DATA,	*PLANG_DATA;


typedef std::map<DWORD, PLANG_DATA>					MAP_LANGUAGE_DATA;
typedef std::map<DWORD, PLANG_DATA>::iterator		ItorLanguageData;


//---------------------------------------------------------------------------------------------
// 폰트 매니져 클레스
//---------------------------------------------------------------------------------------------
class CLangMan
{

private:


	MAP_LANGUAGE_DATA	m_mapLanguageData;


public:

						CLangMan();
	virtual				~CLangMan();

	BOOL				Load( LPCTSTR lpszFileName );
	LANG_DATA const *	GetLangData( DWORD dwLang );

	static	CLangMan*	GetInstance( void );
};


