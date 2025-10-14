
#include "StdAfx.h"
#include "langman.h"


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CLangMan::CLangMan()
{
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CLangMan::~CLangMan()
{
	for( ItorLanguageData Itor = m_mapLanguageData.begin(); Itor != m_mapLanguageData.end(); ++Itor )
	{
		PLANG_DATA pLangData = Itor->second;
		FLERROR_ASSERT_LOG_DO( pLangData, continue, _T("[언어 매니져] 언어 정보 포인터가 유효하지 않습니다. 삭제 무시") );	
		
		SAFE_DELETE( pLangData );
	}

	m_mapLanguageData.clear();
}


//---------------------------------------------------------------------------------------------
// Load ( 불러오기 )
// param	: lpszFileName ( 현재 : proplang.txt )
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CLangMan::Load( LPCTSTR lpszFileName )
{
	FLERROR_ASSERT_LOG_RETURN( lpszFileName, FALSE, _T("[언어 매니져] 파일을 불러 올 수 없습니다.") );	

	CScanner s;
	if( FALSE == s.Load( lpszFileName, TRUE ) ) 
		FLERROR_ASSERT_LOG_RETURN( lpszFileName, FALSE, _T("[언어 매니져] 스캐너로 읽어오기 실패.") );	

	DWORD dwLang( s.GetNumber() );	// dwLang

	while( s.tok != FINISHED )
	{
		PLANG_DATA	pData = new LANG_DATA(); 
		FLERROR_ASSERT_LOG_DO( pData, continue, _T("[언어 매니져] 언어 데이터 생성 실패 - 언어번호 : [%u]."), dwLang );	
		
		s.GetToken();
		s.GetToken();	// szPath
		FLStrcpy( pData->szPath, _countof( pData->szPath ), s.token );
		s.GetToken();

		s.GetToken();	//	szFileName
		FLStrcpy( pData->szFileName, _countof( pData->szFileName ), s.token );

		s.GetToken();	//	szTitle
		FLStrcpy( pData->szTitle, _countof( pData->szTitle ), s.token );

		s.GetToken();
		s.GetToken();	//	szFontFirst
		FLStrcpy( pData->stFont.lfCaption.szFontFirst, _countof( pData->stFont.lfCaption.szFontFirst ), s.token );
		s.GetToken();

		s.GetToken();
		s.GetToken();	//	szFontSecond
		FLStrcpy( pData->stFont.lfCaption.szFontSecond, _countof( pData->stFont.lfCaption.szFontSecond ), s.token );
		s.GetToken();

		pData->stFont.lfCaption.nDivTitle	= static_cast< CHAR >( s.GetNumber() );				// nDivTitle
		pData->stFont.lfCaption.nDivCaption	= static_cast< CHAR >( s.GetNumber() );				// nDivCaption
		pData->stFont.fdLang.charset		= s.GetNumber();									// charset
		pData->stFont.fdLang.wCodePage		= static_cast< WORD >( s.GetNumber() );				// wCodePage

		s.GetToken();
		s.GetToken();
		FLStrcpy( pData->stFont.fdLang.faceNT, _countof( pData->stFont.fdLang.faceNT ), s.token );	// faceNT
		s.GetToken();

		s.GetToken();
		s.GetToken();
		FLStrcpy( pData->stFont.fdLang.face9x, _countof( pData->stFont.fdLang.face9x ), s.token );	// face9x
		s.GetToken();


		UINT uiFontCount( s.GetNumber() );

		for( UINT nCount = 0; nCount < uiFontCount; nCount++ )
		{
			FONT_INFO	stFontInfo;

			DWORD		dwColorA( NULL ),
						dwColorR( NULL ),
						dwColorG( NULL ),
						dwColorB( NULL );

			s.GetToken();
			s.GetToken();									// szFont
			FLStrcpy( stFontInfo.szFont, _countof( stFontInfo.szFont ), s.token );
			s.GetToken();

			stFontInfo.dwFontSize	= s.GetNumber();		// dwFontSize

			dwColorA = s.GetNumber();						// dwColor_A
			dwColorR = s.GetNumber();						// dwColor_R
			dwColorG = s.GetNumber();						// dwColor_G
			dwColorB = s.GetNumber();						// dwColor_B
			stFontInfo.dwColor = D3DCOLOR_ARGB( dwColorA, dwColorR, dwColorG, dwColorB );

			dwColorA = s.GetNumber();						// dwBgColor6_A
			dwColorR = s.GetNumber();						// dwBgColor6_R
			dwColorG = s.GetNumber();						// dwBgColor6_G
			dwColorB = s.GetNumber();						// dwBgColor6_B
			stFontInfo.dwBgColor = D3DCOLOR_ARGB( dwColorA, dwColorR, dwColorG, dwColorB );

			stFontInfo.nOutLine		= s.GetNumber();		// nOutLine
			stFontInfo.dwFlags		= s.GetNumber();		// dwFlags

			pData->stFont.vecFontInfo.push_back( stFontInfo );
		}

		m_mapLanguageData.insert(std::pair<DWORD, PLANG_DATA>( dwLang, pData ) );

		dwLang	= s.GetNumber();
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// Get Lang Data ( 언어 데이터 얻기 )
// param	: dwLang
// return	: BOOL 
//---------------------------------------------------------------------------------------------
LANG_DATA const * CLangMan::GetLangData( DWORD dwLang )
{
	ItorLanguageData Itor = m_mapLanguageData.find( dwLang );

	if( Itor == m_mapLanguageData.end() )
		FLERROR_ASSERT_LOG_RETURN( NULL, NULL, _T("[언어 매니져] 언어 정보를 찾을 수 없습니다 - [ %u ]."),  dwLang );	
		

	PLANG_DATA pLangData = Itor->second;
	FLERROR_ASSERT_LOG_RETURN( pLangData, NULL, _T("[언어 매니져] 언어 정보 포인터가 유효하지 않습니다 - [ %u ]."),  dwLang );	

	return pLangData;
}


//---------------------------------------------------------------------------------------------
// Get Instance ( 인스턴스 얻기 )
// param	: void
// return	: CLangMan* 
//---------------------------------------------------------------------------------------------
CLangMan* CLangMan::GetInstance( void )
{
	static CLangMan	sLocal;
	return &sLocal;
}
