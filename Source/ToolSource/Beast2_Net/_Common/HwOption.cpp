#include "stdafx.h"
#include <stdio.h>

#ifndef __MINIMIZE_USE_OPTION
	#include "scanner.h"
	#include "lang.h"
#endif

#include "HwOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  

COption		g_Option;


#ifdef __MINIMIZE_USE_OPTION
	#define FINISHED	true
	#define Error		printf
	static const char g_seps[] = " ,\t";

	class CScannerEx
	{
	public:
		CScannerEx();
		~CScannerEx();

		BOOL	Load_FileIO( LPCTSTR szFilename );
		void	GetToken();
		int		GetNumber();
		float	GetFloat();

	public:
		CString	Token;
		bool	tok;

	private:
		char	m_szBuffer[256];
		char*	m_szToken;
		FILE*   m_pFILE;

	};

	CScannerEx::CScannerEx()
	{
		Token.Empty();
		tok = false;
		m_pFILE = NULL;
		m_szToken = NULL;
	}

	CScannerEx::~CScannerEx()
	{
		if( m_pFILE )
			fclose( m_pFILE );
	}

	BOOL CScannerEx::Load_FileIO( LPCTSTR szFilename )
	{
		FILE* fp = fopen( szFilename, "rt" );
		if( fp == NULL )
			return FALSE;
		else
		{
			m_pFILE = fp;
			return TRUE;
		}
	}
	void CScannerEx::GetToken()
	{
		if( fgets( m_szBuffer, sizeof( m_szBuffer ), m_pFILE ) )
		{
			m_szToken = strtok( m_szBuffer, g_seps );
			Token = m_szToken; 
		}
		else
		{
			tok = FINISHED;
			Token = "";
		}
	}

	int	 CScannerEx::GetNumber()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken )
			return atoi( m_szToken );
		else
			return 0;
	}

	float CScannerEx::GetFloat()
	{
		m_szToken = strtok( NULL, g_seps );
		if( m_szToken )
			return atof( m_szToken );
		else
			return 0.0f;
	}

#else

	#define CScannerEx	CScanner

#endif

///////////////////////////////////////////////////////////////////
//  COption
///////////////////////////////////////////////////////////////////

COption::COption()
{
	Init();
}

COption::~COption()
{
}

void COption::Init()
{
	memset( m_szFileName, 0, sizeof(m_szFileName) );

//*
#if __CURRENT_LANG == LANG_JAP
	m_nLanguage = LANG_JAP;
#elif __CURRENT_LANG == LANG_THA
	m_nLanguage = LANG_THA;
#elif __CURRENT_LANG == LANG_CHI
	m_nLanguage = LANG_CHI;
#elif __CURRENT_LANG == LANG_TWN
	m_nLanguage = LANG_TWN;
#elif __CURRENT_LANG == LANG_ENG
	m_nLanguage = LANG_ENG;
#else
	m_nLanguage = LANG_KOR; // default
#endif	
/**/
//	m_nLanguage = __CURRENT_LANG; // default


//	m_nSoundEffect = 1;
	m_bOperator = FALSE;
	m_nResWidth = 1024;//800;
	m_nResHeight = 768;//600;
	m_bStartFullScreen = TRUE;		// 나중에 정식 서비스 들어가면 1로 바꾸자.

#ifdef __BEAST
//	m_fMusicVolume = 1.0f;
	m_nTextureQuality = 0;//HIGH;
	m_nViewArea = 0;
	m_nObjectDistant = 0;
	m_nObjectDetail = 0;
	m_nShadow = 0;
#else
//	m_fMusicVolume = 1.0f;
	m_nTextureQuality = 0;//HIGHest;
	m_nViewArea = 1;
	m_nObjectDistant = 1;
	m_nObjectDetail = 1;
	m_nShadow = 2;
#endif
	m_nWindowAlpha = 255; 
	m_nWindowEffect = 0;
	m_nSlangWord    = 0;
	m_nChatCommand  = 0;
	m_nInstantHelp  = 1;
	m_nInterface	= 1;	// 디폴트를 신버전으로 해놓자.
	m_bRollEffect = 0;
	m_bTip = 1;
	
	m_bSaveAccount = 0;
	m_szAccount[ 0 ] = 0;
	m_bNotice = TRUE;
	m_bTip = TRUE;
	m_tNoticeTime = 0;
	m_bTrade = TRUE;
	m_bSay = TRUE;
	m_bParty = TRUE;
	m_bShout = TRUE;
	m_bMessengerJoin = TRUE;
	m_bMessenger = TRUE;
	m_nToggleScreen = 0;

#ifdef __S0602
	m_nSer = m_nMSer = 0;
#endif // __S0602
	
#ifdef __XGLARE
	m_nBloom = 0;
#endif

#ifdef __S0810_NOTMONNAME
	m_nMonName = 1;
#endif // __S0810_NOTMONNAME
	
#ifdef __TOOLTIPTEXTURE
	m_nToolTipTexture = 0;
#endif // __TOOLTIPTEXTURE
	m_nToolTipText = 0;
	
//#ifdef __YADDSOUND
	m_bVoice        = TRUE;
	m_fEffectVolume = 1.0f;
	m_fBGMVolume    = 0.5f;
//#endif //__YADDSOUND
#ifdef __YGUIDESYSTEM0923
	m_nGuide1        = 0;
	m_nGuide2        = 0;
	m_nGuide3        = 0;
#endif
#ifdef __DEV1020
	m_bDev	= FALSE;
#endif	// __DEV1020
	
	m_bIsShowReSkillMessage = TRUE;
	m_pGuide = NULL;

#ifdef __VCHATFILTER_20050316
#ifdef __CLIENT
#ifdef __V050523_BUGFIX_CHATFILTER
	m_dwChatFilter[0] = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD;
	m_dwChatFilter[1] = CHATSTY_GENERAL | CHATSTY_WHISPER ;
	m_dwChatFilter[2] = CHATSTY_GENERAL | CHATSTY_SHOUT ;
	m_dwChatFilter[3] = CHATSTY_GENERAL | CHATSTY_PARTY ;
	m_dwChatFilter[4] = CHATSTY_GENERAL | CHATSTY_GUILD;
	m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; 
#else
	m_dwChatFilter[0] = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD
		| CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; // 일반 
	m_dwChatFilter[1] = CHATSTY_GENERAL | CHATSTY_WHISPER 
		| CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; // 귓속말 
	m_dwChatFilter[2] = CHATSTY_GENERAL | CHATSTY_SHOUT 
		| CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; // 외치기
	m_dwChatFilter[3] = CHATSTY_GENERAL | CHATSTY_PARTY 
		| CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; // 파티 
	m_dwChatFilter[4] = CHATSTY_GENERAL | CHATSTY_GUILD
		| CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; // 길드 
#endif
#else
	m_dwChatFilter = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD
		| CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT;// | CHATSTY_CLIENTMSG;
#ifdef __JAPAN__
	m_dwChatFilter	|= CHATSTY_GAME_CHAT;
	m_dwChatFilter	&= ~CHATSTY_GAME_CLIENT;
	m_dwChatFilter	|= CHATSTY_SYSTEM_CHAT;
	m_dwChatFilter	&= ~CHATSTY_SYSTEM_CLIENT;
	m_dwChatFilter	&= ~CHATSTY_HELP_CLIENT;
	m_dwChatFilter	|= CHATSTY_HELP_CHAT;
	m_dwChatFilter	|= CHATSTY_GENERAL;
	m_dwChatFilter	|= CHATSTY_WHISPER;
	m_dwChatFilter	|= CHATSTY_SHOUT;
	m_dwChatFilter	|= CHATSTY_PARTY;
	m_dwChatFilter	|= CHATSTY_GUILD;
#endif	// __JAPAN__
#endif
#endif // __CLIENT


#ifdef __H1207_IPSET__
	m_IPAddress[0] = NULL;
	m_szRegistry[0] = NULL;
#endif

#ifdef __YENV
	m_bExitChange = FALSE;
	m_bSpecBump = TRUE;
#endif //__YENV

#ifdef __YDAMAGE_RENDER
	m_bDamageRender = FALSE;
#endif //__YDAMAGE_RENDER	
	
#ifdef __YVISIBLE_BUFF_TIME
	m_bVisibleBuffTimeRender = FALSE;
#endif 
	
#ifdef __ACROBAT_0504
	m_MouseSpeed = 1;
#endif //__ACROBAT_0504
	
#ifdef __YRENDER_FLAG	
	m_bSFXRenderOff = FALSE;	
#endif //__YRENDER_FLAG	
	
}

#ifdef __SDEBUG_TOOLTIP_TEST
int COption::LoadToolTip( LPCTSTR szFileName )
{
	_tcscpy( m_szFileName, szFileName);
	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
		return 0;
	
	m_nToolTipText = 2;
	int nR = 0;
	int nG = 0;
	int nB = 0;

	do
	{
		nR = 0;
		nG = 0;
		nB = 0;
		scan.GetToken();
		if( scan.Token == _T( "아이템이름0" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName0 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName1 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName2 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName3 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름4" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName4 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "아이템이름5" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwName5 = D3DCOLOR_ARGB( 255, nR, nG, nB );
		}
		else if( scan.Token == _T( "일반" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwGeneral = D3DCOLOR_ARGB( 255, nR, nG, nB );		// 일반	
		}
		else if( scan.Token == _T( "플러스옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwPlusOption = D3DCOLOR_ARGB( 255, nR, nG, nB );	// 플러스 옵션
		}
		else if( scan.Token == _T( "피어싱" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwPiercing = D3DCOLOR_ARGB( 255, nR, nG, nB );	// 피어싱
		}
		else if( scan.Token == _T( "속성일반" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResist = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 일반
		}
		else if( scan.Token == _T( "속성불" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistFire = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 불
		}
		else if( scan.Token == _T( "속성물" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistWater = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 물
		}
		else if( scan.Token == _T( "속성땅" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistEarth = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 땅
		}
		else if( scan.Token == _T( "속성전기" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistElectricity = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 전기
		}
		else if( scan.Token == _T( "속성바람" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistWind = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 바람
		}
		else if( scan.Token == _T( "속성상용화무기" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistSM = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 상용화 무기
		}
		else if( scan.Token == _T( "속성상용화방어구" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwResistSM1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 속성 상용화 방어구
		}
		else if( scan.Token == _T( "시간" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwTime = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 시간 및 효과
		}
		else if( scan.Token == _T( "효능0" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective0 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능
		}
		else if( scan.Token == _T( "효능1" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능1
		}
		else if( scan.Token == _T( "효능2" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective2 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능2
		}
		else if( scan.Token == _T( "효능3" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEffective3 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 아이템 효능3
		}
		else if( scan.Token == _T( "랜덤옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwRandomOption = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 랜덤 옵션
		}
		else if( scan.Token == _T( "인첸트옵션" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwEnchantOption = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 인첸트 옵션
		}
		else if( scan.Token == _T( "세트이름" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetName = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 이름
		}
		else if( scan.Token == _T( "세트목록비착용" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetItem0 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 목록(비착용시)
		}
		else if( scan.Token == _T( "세트목록착용" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetItem1 = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 목록(착용시)
		}
		else if( scan.Token == _T( "세트효과" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwSetEffect = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 세트 효과
		}
		else if( scan.Token == _T( "가격" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwGold = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 가격
		}
		else if( scan.Token == _T( "설명" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwCommand = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 설명
		}
		else if( scan.Token == _T( "사용못함" ) )
		{
			nR = scan.GetNumber();			
			nG = scan.GetNumber();
			nB = scan.GetNumber();
			g_WndMng.dwItemColor[m_nToolTipText].dwNotUse = D3DCOLOR_ARGB( 255, nR, nG, nB );			// 사용못함	
		}
	} while( scan.tok != FINISHED );

	return 1;
}
#endif // __SDEBUG_TOOLTIP_TEST


int COption::Load( LPCTSTR szFileName )
{
	_tcscpy( m_szFileName, szFileName);
	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
	{
//#ifdef	_DEBUG
//		Error( "%s : 찾을 수 없다. ", szFileName );
//#endif
		return 0;
	}
	do {
		scan.GetToken();
		if( scan.Token == _T( "lang" ) )
		{
			m_nLanguage = scan.GetNumber();
			if( m_nLanguage == 18 )
				m_nLanguage = LANG_KOR;

#if __CURRENT_LANG == LANG_TWN
			m_nLanguage = LANG_TWN;
#endif			
#ifdef __LANG_TAIWAN
			m_nLanguage = LANG_TWN;
#endif // __LANG_TAIWAN
		}
		else
//		if( scan.Token == _T( "sound" ) ) 
//			m_nSoundEffect = scan.GetNumber();
//		else
		if( scan.Token == _T( "resolution" ) )
		{
			m_nResWidth = scan.GetNumber();
			m_nResHeight = scan.GetNumber();

			if( m_nResWidth != 800 && m_nResWidth != 1024 && m_nResWidth != 1280 )
			{
				Error( "지원하지않는 해상도 발견!! - Width : %d", m_nResWidth );
				m_nResWidth = 800;
			}

			if( m_nResHeight != 600 && m_nResHeight != 768 && m_nResHeight != 1024 )
			{
				Error( "지원하지않는 해상도 발견!! - Height : %d", m_nResHeight );
				m_nResHeight = 600;
			}
		}
		else
		if( scan.Token == _T( "fullscreen" ) )
			m_bStartFullScreen = scan.GetNumber();
		else
//		if( scan.Token == _T( "music" ) ) 
//			m_fMusicVolume = scan.GetFloat();
//		else
		if( scan.Token == _T( "texture" ) )
			m_nTextureQuality = scan.GetNumber();
		else
		if( scan.Token == _T( "view" ) )
		{
			m_nViewArea = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "distant" ) )
		{
			m_nObjectDistant = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "shadow" ) )
		{
			m_nShadow = scan.GetNumber();
		}
		else
#ifdef __XGLARE
		if( scan.Token == _T( "bloom" ) )
		{
			m_nBloom = scan.GetNumber();
		}
		else
#endif
		if( scan.Token == _T( "detail" ) )
			m_nObjectDetail = scan.GetNumber();
		else
		if( scan.Token == _T( "hotkey" ) ) 
		{
		}
		else
		if( scan.Token == _T( "WindowAlpha" ) ) 
		{
			m_nWindowAlpha = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SlangWord" ) ) 
		{
			m_nSlangWord = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatCommand" ) ) 
		{
			m_nChatCommand = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "InstantHelp" ) ) 
		{
			m_nInstantHelp = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Interface" ) ) 
		{
			m_nInterface = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "RollEffect" ) ) 
		{
			m_bRollEffect = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "account" ) ) 
		{
			scan.GetToken();
			strcpy( m_szAccount, scan.Token );
		}
		else
		if( scan.Token == _T( "saveAccount" ) ) 
		{
			m_bSaveAccount = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "notice" ) ) 
		{
			m_bNotice = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "noticeTime" ) ) 
		{
			m_tNoticeTime = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "tip" ) ) 
		{
			m_bTip = scan.GetNumber();
		}				
		else
		if( scan.Token == _T( "Trade") )
		{
			m_bTrade = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Say") )
		{
			m_bSay = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Party") )
		{
			m_bParty = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "sunkist" ) )
		{
			m_bOperator = TRUE;
			m_nToggleScreen = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Shout" ) )
		{
			m_bShout = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MessengerJoin" ) )
		{
			m_bMessengerJoin = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Messenger") )
		{
			m_bMessenger = scan.GetNumber();
		}
#ifdef __S0602
		else
		if( scan.Token == _T( "Server" ) )
		{
			m_nSer = scan.GetNumber();
			m_nMSer = scan.GetNumber();
		}
#endif // __S0602
//#ifdef __YADDSOUND
		else
		if( scan.Token == _T( "Sound" ) )
		{
			m_bVoice        = scan.GetNumber();
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume    = scan.GetFloat();
		}
//#endif //__YADDSOUND			
#ifdef __YGUIDESYSTEM0923
		else
		if( scan.Token == _T( "Guide" ) )
		{
			m_nGuide1        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide2" ) )
		{
			m_nGuide2        = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Guide3" ) )
		{
			m_nGuide3        = scan.GetNumber();
		}
#endif //__YGUIDESYSTEM0923	

#ifdef __DEV1020
		else if( scan.Token == _T( "Dev" ) )
		{
			m_bDev	= scan.GetNumber();
		}
#endif	// __DEV1020
		else
		if( scan.Token == _T( "SkillShowMessage" ) )
		{
			m_bIsShowReSkillMessage = scan.GetNumber();
		}
#ifdef __VCHATFILTER_20050316
		else
		if( scan.Token == _T( "ChatFilter0" ) )
		{
			m_dwChatFilter[0] = scan.GetNumber();
#ifdef __V050523_BUGFIX_CHATFILTER
			m_dwChatFilter[0] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
#endif
		}
		else
		if( scan.Token == _T( "ChatFilter1" ) )
		{
			m_dwChatFilter[1] = scan.GetNumber();
#ifdef __V050523_BUGFIX_CHATFILTER
			m_dwChatFilter[1] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
#endif
		}
		else
		if( scan.Token == _T( "ChatFilter2" ) )
		{
			m_dwChatFilter[2] = scan.GetNumber();
#ifdef __V050523_BUGFIX_CHATFILTER
			m_dwChatFilter[2] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
#endif
		}
		else
		if( scan.Token == _T( "ChatFilter3" ) )
		{
			m_dwChatFilter[3] = scan.GetNumber();
#ifdef __V050523_BUGFIX_CHATFILTER
			m_dwChatFilter[3] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
#endif
		}
		else
		if( scan.Token == _T( "ChatFilter4" ) )
		{
			m_dwChatFilter[4] = scan.GetNumber();
#ifdef __V050523_BUGFIX_CHATFILTER
			m_dwChatFilter[4] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
#endif
		}
#ifdef __V050523_BUGFIX_CHATFILTER
		else
		if( scan.Token == _T( "ChatFilterEtc" ) )
		{
			m_dwChatFilterEtc = scan.GetNumber();
		}
#endif
#else
		else
		if( scan.Token == _T( "ChatFilter" ) )
		{
			m_dwChatFilter = scan.GetNumber();
		}
#endif

#ifdef __H1207_IPSET__
		else
		if( scan.Token == _T( "ip" ) )
		{
			scan.GetToken();
			strcpy( m_IPAddress, scan.Token );
		}
		else
		if( scan.Token == _T( "registry" ) )
		{
			scan.GetToken();
			strcpy( m_szRegistry, scan.Token );
		}
#endif // __H1207_IPSET__
#ifdef __YENV
		else
		if( scan.Token == _T( "SpecBumpEffect" ) )
		{
			m_bSpecBump = scan.GetNumber();
		}
#endif //__YENV
#ifdef __YDAMAGE_RENDER
		else
		if( scan.Token == _T( "DamageRender" ) )
		{
			m_bDamageRender = scan.GetNumber();
		}
#endif //__YDAMAGE_RENDER	
#ifdef __YVISIBLE_BUFF_TIME
		else
		if( scan.Token == _T( "BuffTimeRender" ) )
		{
			m_bVisibleBuffTimeRender = scan.GetNumber();
		}
#endif //__YVISIBLE_BUFF_TIME
#ifdef __ACROBAT_0504
		else
		if( scan.Token == _T( "MouseSpeed" ) )
		{
			m_MouseSpeed = scan.GetNumber();
		}
#endif //__ACROBAT_0504
#ifdef __YRENDER_FLAG
		else
		if( scan.Token == _T( "SFXRenderOff" ) )
		{
			m_bSFXRenderOff = scan.GetNumber();
		}
#endif //__YRENDER_FLAG
	} while( scan.tok != FINISHED );
	//SetDefaultHotkey(100);

	//CDC dc;//( GetSafeHwnd() );
	//g_bmp.CreateDIBSection( &dc, NULL, 1024, 768, 32 );

	return 1;
}

int	COption::Save( LPCTSTR szFileName )
{
	FILE *fp = fopen( szFileName, _T( "wt" ) );
	if(fp == NULL) return 0;
	_ftprintf(fp, _T( "// neuz browser ini file\n\n" ) );
	_ftprintf(fp, _T( "// option\n" ) ,0);
	_ftprintf(fp, _T( "resolution %d %d\n" ) ,m_nResWidth, m_nResHeight );
//	_ftprintf(fp, _T( "sound %d\n" ) ,m_nSoundEffect );
//	_ftprintf(fp, _T( "music %f\n" ) ,m_fMusicVolume );
	_ftprintf(fp, _T( "lang %d\n" ) ,m_nLanguage );
	_ftprintf(fp, _T( "fullscreen %d\n" ) ,m_bStartFullScreen );
	_ftprintf(fp, _T( "texture %d\n" ) ,m_nTextureQuality );
	_ftprintf(fp, _T( "view %d\n" ) ,m_nViewArea );
	_ftprintf(fp, _T( "detail %d\n" ) ,m_nObjectDetail );
	_ftprintf(fp, _T( "distant %d\n" ) ,m_nObjectDistant );
	_ftprintf(fp, _T( "shadow %d\n" ) ,m_nShadow );
#ifdef __XGLARE
	_ftprintf(fp, _T( "bloom %d\n" ) ,m_nBloom );
#endif
	
	_ftprintf(fp, _T( "WindowAlpha %d\n" ) ,m_nWindowAlpha );
	_ftprintf(fp, _T( "SlangWord %d\n" )   ,m_nSlangWord );
	_ftprintf(fp, _T( "ChatCommand %d\n" ) ,m_nChatCommand );
	_ftprintf(fp, _T( "InstantHelp %d\n" ) ,m_nInstantHelp );
	_ftprintf(fp, _T( "Interface %d\n" ) ,m_nInterface );
	_ftprintf(fp, _T( "RollEffect %d\n" ) ,m_bRollEffect );
	
	_ftprintf(fp, _T( "saveAccount %d\n" ), m_bSaveAccount );
	if( m_bSaveAccount )
		_ftprintf(fp, _T( "account \"%s\"\n" ), m_szAccount );
	else
		_ftprintf(fp, _T( "account \"%s\"\n" ), "" );
	_ftprintf(fp, _T( "notice %d\n" ), m_bNotice );
	_ftprintf(fp, _T( "noticeTime %d\n" ), m_tNoticeTime );
	_ftprintf(fp, _T( "tip %d\n" ), m_bTip );
	_ftprintf(fp, _T( "Trade %d\n" ), m_bTrade );
	_ftprintf(fp, _T( "Say %d\n" ), m_bSay );
	_ftprintf(fp, _T( "Party %d\n" ), m_bParty );
	_ftprintf(fp, _T( "Shout %d\n" ), m_bShout );
	_ftprintf(fp, _T( "MessengerJoin %d\n" ), m_bMessengerJoin );
	_ftprintf(fp, _T( "Messenger %d\n" ), m_bMessenger );
#ifdef __S0602
	_ftprintf(fp, _T( "Server %d %d\n" ), m_nSer, m_nMSer );
#endif // __S0602
#ifdef __DEV1020
	if( m_bDev )
		_ftprintf( fp, _T( "Dev %d\n" ), m_bDev );
#endif	// __DEV1020
	if( m_nToggleScreen )
		_ftprintf(fp, _T( "sunkist %d\n" ), m_nToggleScreen );

//#ifdef __YADDSOUND
	_ftprintf(fp, _T( "Sound %d %.1f %.1f\n" ), m_bVoice, m_fEffectVolume, m_fBGMVolume );
//#endif //__YADDSOUND
#ifdef __YGUIDESYSTEM0923
	_ftprintf(fp, _T( "Guide %d\n" ), m_nGuide1 );
	_ftprintf(fp, _T( "Guide2 %d\n" ), m_nGuide2 );
	_ftprintf(fp, _T( "Guide3 %d\n" ), m_nGuide3 );
#endif //__YGUIDESYSTEM0923
	
	_ftprintf(fp, _T( "SkillShowMessage %d\n" ), m_bIsShowReSkillMessage );

#ifdef __VCHATFILTER_20050316
	_ftprintf(fp, _T( "ChatFilter0 %d\n" ), m_dwChatFilter[0] );
	_ftprintf(fp, _T( "ChatFilter1 %d\n" ), m_dwChatFilter[1] );
	_ftprintf(fp, _T( "ChatFilter2 %d\n" ), m_dwChatFilter[2] );
	_ftprintf(fp, _T( "ChatFilter3 %d\n" ), m_dwChatFilter[3] );
	_ftprintf(fp, _T( "ChatFilter4 %d\n" ), m_dwChatFilter[4] );
#ifdef __V050523_BUGFIX_CHATFILTER
	_ftprintf(fp, _T( "ChatFilterEtc %d\n" ), m_dwChatFilterEtc );
#endif
#else
	_ftprintf(fp, _T( "ChatFilter %d\n" ), m_dwChatFilter );
#endif

#ifdef __YENV
	if( m_bExitChange )
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), !m_bSpecBump );
	else
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), m_bSpecBump );
	
#endif //__YENV

#ifdef __H1207_IPSET__
	_ftprintf(fp, _T( "ip \"%s\"\n" ), m_IPAddress);
	if( m_szRegistry[0] != '\0' )
		_ftprintf(fp, _T( "registry \"%s\"\n" ), m_szRegistry );
#endif
	
#ifdef __YDAMAGE_RENDER
	_ftprintf(fp, _T( "DamageRender %d\n" ), m_bDamageRender );
#endif //__YDAMAGE_RENDER	

#ifdef __YVISIBLE_BUFF_TIME
	_ftprintf(fp, _T( "BuffTimeRender %d\n" ), m_bVisibleBuffTimeRender );
#endif //__YVISIBLE_BUFF_TIME
		
#ifdef __ACROBAT_0504
	_ftprintf(fp, _T( "MouseSpeed %d\n" ), m_MouseSpeed );
#endif //__ACROBAT_0504

#ifdef __YRENDER_FLAG	
	_ftprintf(fp, _T( "SFXRenderOff %d\n" ), m_bSFXRenderOff );
#endif //__YRENDER_FLAG	
	
	/*
	_ftprintf(fp, "map %d\n"         ,g_bOverlapShowMap);
	_ftprintf(fp, "wisper %d\n"      ,g_bWhisperMode);
	_ftprintf(fp, "filter %d\n"      ,g_nFilterType);
	_ftprintf(fp, "sfx %d\n"         ,g_bSfxEnable);
	_ftprintf(fp, "tooltip %d\n"     ,g_bToolTipEnable);
	_ftprintf(fp, "transparence %d\n",g_bTransparence);
	_ftprintf(fp, "status %d\n"      ,g_nStatusTimer);
	_ftprintf(fp, "dialog %d\n"      ,g_nDialogTimer);
	_ftprintf(fp, "memorySize %d\n"  ,g_nMemorySize);
	_ftprintf(fp, "notice %d\n"      ,g_nNoticeDate);
	_ftprintf(fp, "memory %d\n"      ,g_nMemoryDate);
	_ftprintf(fp, "remember %d\n"    ,g_nRememberDate);
	_ftprintf(fp, "tooltip %d\n"     ,g_ToolTip.GetEnable());
	_ftprintf(fp, "account %s\n"     ,g_strAccount);
	_ftprintf(fp, "\n// Custom hotkey\n");
	for(int i = 0; i < HOTKEY_MAX; i++)
	{
		LPHOTKEY lpHotkey = &m_HotkeyTable[i];
		if(lpHotkey->m_nHotkey)
			_ftprintf(fp, "hotkey %d %d %d %d\n",i,lpHotkey->m_bShift,lpHotkey->m_bControl,lpHotkey->m_nHotkey);
	}
	_ftprintf(fp, "\n// Custom font face\n");
	_ftprintf(fp, "FontFaceStatus	%s\n",g_FontFace.GetAt(1));
	_ftprintf(fp, "FontFaceNormal	%s\n",g_FontFace.GetAt(2));
	_ftprintf(fp, "FontFaceItalic	%s\n",g_FontFace.GetAt(3));
	_ftprintf(fp, "FontFaceBold	%s\n",g_FontFace.GetAt(4));
	_ftprintf(fp, "FontFaceSmall	%s\n",g_FontFace.GetAt(5));
	_ftprintf(fp, "FontFaceMenu9	%s\n",g_FontFace.GetAt(8));
*/
	fclose(fp);
	return 1;
}





















