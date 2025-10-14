#include "stdafx.h"
#include <stdio.h>

//#include "HwOption.h"

#include <common/FLStdFile.h>

//sun: Wide및 고해상도 추가
#include <D3D9.h>
#pragma comment	(lib, "d3d9.lib")

// chat style ( copy from \\_Interface\\WndManager.h )
	#define CHATSTY_GENERAL   0x0001
	#define CHATSTY_WHISPER   0x0002
	#define CHATSTY_SHOUT     0x0004
	#define CHATSTY_PARTY     0x0008
	#define CHATSTY_GUILD     0x0010

	#define CHATSTY_HELP          0x0060
	#define CHATSTY_HELP_CLIENT   0x0020
	#define CHATSTY_HELP_CHAT     0x0040
	#define CHATSTY_SYSTEM        0x0180
	#define CHATSTY_SYSTEM_CLIENT 0x0080
	#define CHATSTY_SYSTEM_CHAT   0x0100
	#define CHATSTY_GAME          0x0600
	#define CHATSTY_GAME_CLIENT   0x0200
	#define CHATSTY_GAME_CHAT     0x0400

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
		void	GetTokenEx();
		int		GetNumber();
		float	GetFloat();

	public:
		CString	Token;
		bool	tok;

	private:
		char	m_szBuffer[256];
		char*	m_szToken;
		FILE*   m_pFILE;
		char*	m_szNextTok;

	};

	CScannerEx::CScannerEx()
	{
		Token.Empty();
		tok = false;
		m_pFILE = NULL;
		m_szToken = NULL;
		m_szNextTok = NULL;
	}

	CScannerEx::~CScannerEx()
	{
		if( m_pFILE )
			fclose( m_pFILE );
	}

	BOOL CScannerEx::Load_FileIO( LPCTSTR szFilename )
	{
		FILE* fp = FLStdFileOpen( szFilename, "rt" );
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
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
			m_szToken = strtok_s( m_szBuffer, g_seps, &m_szNextTok );
#else
			m_szToken = strtok( m_szBuffer, g_seps );
#endif
			Token = m_szToken; 
		}
		else
		{
			tok = FINISHED;
			Token = "";
		}
	}

	void CScannerEx::GetTokenEx()
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		m_szToken = strtok_s( NULL, g_seps, &m_szNextTok );
#else
		m_szToken = strtok( NULL, g_seps );
#endif
		if( m_szToken[0] == '"' )
		{
			Token = "";
			for( int i=1; m_szToken[i] != '"'; ++i )
				Token += m_szToken[i];
		}
		else
		{
			Token = m_szToken;
		}
	}

	int	 CScannerEx::GetNumber()
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		m_szToken = strtok_s( NULL, g_seps, &m_szNextTok );
#else
		m_szToken = strtok( NULL, g_seps );
#endif
		if( m_szToken )
			return atoi( m_szToken );
		else
			return 0;
	}

	float CScannerEx::GetFloat()
	{
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
		m_szToken = strtok_s( NULL, g_seps, &m_szNextTok );
#else
		m_szToken = strtok( NULL, g_seps );
#endif
		if( m_szToken )
			return static_cast< float >( atof( m_szToken ) );
		else
			return 0.0f;
	}
	int GetLanguage()
	{
		return __CURRENT_LANG;	// patchclient의 versioncommon.h에 정의됨 
	}


///////////////////////////////////////////////////////////////////
//  COption
///////////////////////////////////////////////////////////////////

COption::COption()
{
	//Init();	_SUN_DEFAULT_WINDOW
}

COption::~COption()
{
}

void COption::Init()
{
	memset( m_szFileName, 0, sizeof(m_szFileName) );
	m_nWeatherEffect = TRUE;
	m_bOperator = FALSE;
	m_nResWidth = 1024;
	m_nResHeight = 768;
	
	//_SUN_DEFAULT_WINDOW
	if( ::GetLanguage() == LANG_KOR )
		m_bStartFullScreen = FALSE;		
	else m_bStartFullScreen = TRUE;

	m_nTextureQuality = 1;
	m_nViewArea = 1;
	m_nObjectDistant = 1;
	m_nObjectDetail = 1;
	m_nShadow = 2;
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
	m_bBattleBGM = FALSE;
	m_bGameGradeRendering = FALSE;
	m_nToggleScreen = 0;
	m_nSer = m_nMSer = 0;
	m_nBloom = 0;

	m_nMonName = 1;
	m_nToolTipTexture = 0;
	m_nToolTipText = 0;
	
	m_bVoice        = TRUE;
	m_fEffectVolume = 1.0f;
	m_fBGMVolume    = 0.5f;
	m_nGuide1        = 0;
	m_nGuide2        = 0;
	m_nGuide3        = 0;

	m_nTutorialLv = 0;		//sun: 12, 튜토리얼 개선
	m_bViewMask = TRUE;		//sun: 12, 옵션창 개선

	m_bIsShowReSkillMessage = TRUE;
	m_pGuide = NULL;

	m_dwChatFilter[0] = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD;
	m_dwChatFilter[1] = CHATSTY_GENERAL | CHATSTY_WHISPER ;
	m_dwChatFilter[2] = CHATSTY_GENERAL | CHATSTY_SHOUT ;
	m_dwChatFilter[3] = CHATSTY_GENERAL | CHATSTY_PARTY ;
	m_dwChatFilter[4] = CHATSTY_GENERAL | CHATSTY_GUILD;
	m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT; 

	m_IPAddress[0] = NULL;
#ifdef __SFX_OPT
	m_nSfxLevel = 5;	
#endif

#ifdef __YENV
	m_bExitChange = FALSE;
	m_bSpecBump = TRUE;
#endif //__YENV

	m_bDamageRender = FALSE;
	
	m_bVisibleBuffTimeRender = TRUE;
	
	m_MouseSpeed = 1;
	m_bSFXRenderOff = FALSE;	

	m_bCameraLock = TRUE;			// 디폴트로 켜져있게 내가 고쳤슴데이~ -xuzhu-
	m_bAutoAttack = TRUE;
	m_nPlayerName		= 1;		    // 자신 이름 On / Off
	m_nOtherPlayerName	= 1;		    // 다른 플레이어 이름 On / Off

	//sun: 밝기, 감마, 명암 조절 Neuz
	m_nOverBright = 10;
	m_fGamma = 1.0f;
	m_fContrast = 1.0f;

	m_BuffStatusMode = 0;		// sun 8차 버프창 관련 적용
	
	m_bZoomLimit = TRUE;		//sun: 11, 줌옵션 추가

}

//sun: Wide및 고해상도 추가
void COption::CheckResolution()
{
	IDirect3D9* pD3D9;
	BOOL bResolution = FALSE;
	
	pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	//UINT numAdapters = pD3D9->GetAdapterCount();
	
	D3DFORMAT allowedAdapterFormat = D3DFMT_X8R8G8B8;
	UINT numAdapterModes = pD3D9->GetAdapterModeCount( 0, allowedAdapterFormat );
	for (UINT mode = 0; mode < numAdapterModes; mode++)
	{
		D3DDISPLAYMODE displayMode;
		pD3D9->EnumAdapterModes( 0, allowedAdapterFormat, mode, &displayMode );
		if( displayMode.Width == m_nResWidth && displayMode.Height == m_nResHeight )
			bResolution = TRUE;
	}
	
	if(!bResolution && m_bStartFullScreen) //미지원 해상도의 경우 800x600으로 고정
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
		m_nResWidth = 800;
		m_nResHeight = 600;
	}
	else //그래픽카드가 지원하는 해상도 중 사용가능한 해상도 선별
	{
		switch(m_nResWidth)
		{
		case 800: //800x600
			if(m_nResHeight != 600)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 600;
			}
			break;
		case 1024: //1024x768
			if(m_nResHeight != 768)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 768;
			}
			break;
		case 1280: //1280x1024 or 1280x720 (WIDE) or 1280x768 (WIDE) or 1280x800 (WIDE)
			if(m_nResHeight != 1024 && m_nResHeight != 720 && m_nResHeight != 768 && m_nResHeight != 800)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResWidth = 800;
				m_nResHeight = 600;				
			}
			break;
		case 1600: //1600x1200
			if(m_nResHeight != 1200)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 1200;
			}
			break;
		case 1360: //1360x768 (WIDE)
			if(m_nResHeight != 768)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 768;
			}
			break;
		case 1400: //1400x1050
			if(m_nResHeight != 1050)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 1050;
			}
			break;
		case 1440: //1440x900 (WIDE)
			if(m_nResHeight != 900)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 900;
			}
			break;
		case 1680: //1680x1050 (WIDE)
			if(m_nResHeight != 1050)
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
				m_nResHeight = 1050;
			}
			break;
		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "We Can't Support this Resolution!! - Width : %d, Height : %d" ), m_nResWidth, m_nResHeight );
			m_nResWidth = 800;
			m_nResHeight = 600;
			break;
		}
	}
	//SAFE_RELEASE( pD3D9 );
	pD3D9->Release();
}


int COption::Load( LPCTSTR szFileName )
{
	FLStrcpy( m_szFileName, _countof( m_szFileName ), szFileName );

	CScannerEx scan;
	if(scan.Load_FileIO( szFileName ) == FALSE )
	{
		//neuz.ini가 없으면 다시 옵션값을 세팅한다.
		g_Option.Init();
		return 0;
	}
	do {
		scan.GetToken();
		if( scan.Token == _T( "resolution" ) )
		{
			m_nResWidth = scan.GetNumber();
			m_nResHeight = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "fullscreen" ) )
			m_bStartFullScreen = scan.GetNumber();
		else
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
		if( scan.Token == _T( "bloom" ) )
		{
			m_nBloom = scan.GetNumber();
			m_nBloom	= 0;	// 2008/04/25
		}
		else
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
			scan.GetTokenEx();
			FLStrcpy( m_szAccount, _countof( m_szAccount ), scan.Token );
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
		else
		if( scan.Token == _T( "BattleBGM") )
		{
			m_bBattleBGM = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "GameGrade") )
		{
			m_bGameGradeRendering = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Server" ) )
		{
			m_nSer = scan.GetNumber();
			m_nMSer = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "Sound" ) )
		{
			m_bVoice        = scan.GetNumber();
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume    = scan.GetFloat();
		}
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
		else
		if( scan.Token == _T( "SkillShowMessage" ) )
		{
			m_bIsShowReSkillMessage = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatFilter0" ) )
		{
			m_dwChatFilter[0] = scan.GetNumber();
			m_dwChatFilter[0] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter1" ) )
		{
			m_dwChatFilter[1] = scan.GetNumber();
			m_dwChatFilter[1] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter2" ) )
		{
			m_dwChatFilter[2] = scan.GetNumber();
			m_dwChatFilter[2] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter3" ) )
		{
			m_dwChatFilter[3] = scan.GetNumber();
			m_dwChatFilter[3] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilter4" ) )
		{
			m_dwChatFilter[4] = scan.GetNumber();
			m_dwChatFilter[4] &= ~( CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME );
		}
		else
		if( scan.Token == _T( "ChatFilterEtc" ) )
		{
			//sun: 시스템 메세지 윈도우화... Neuz
			m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT;
		}
		else
		if( scan.Token == _T( "ip" ) )
		{
			scan.GetTokenEx();
			FLStrcpy( m_IPAddress, _countof( m_IPAddress ), scan.Token );
		}
#ifdef __YENV
		else
		if( scan.Token == _T( "SpecBumpEffect" ) )
		{
			m_bSpecBump = scan.GetNumber();
		}
#endif //__YENV
		else
		if( scan.Token == _T( "DamageRender" ) )
		{
			m_bDamageRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "BuffTimeRender" ) )
		{
			m_bVisibleBuffTimeRender = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MouseSpeed" ) )
		{
			m_MouseSpeed = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "SFXRenderOff" ) )
		{
			m_bSFXRenderOff = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "CameraLock" ) )
		{
			m_bCameraLock = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "AutoAttack" ) )
		{
			m_bAutoAttack = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "PRenderName" ) )
		{
			m_nPlayerName = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "OPRenderName" ) )
		{
			m_nOtherPlayerName = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "NRenderName" ) )
		{
			m_nMonName = scan.GetNumber();
		}
		//sun: 밝기, 감마, 명암 조절 Neuz BEGIN
		else
		if( scan.Token == _T( "BRIGHT" ) )
		{
			m_nOverBright = scan.GetNumber();
		}	
		else
		if( scan.Token == _T( "GAMMA" ) )
		{
			m_fGamma = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "CONTRAST" ) )
		{
			m_fContrast = scan.GetFloat();
		}
		//sun: 밝기, 감마, 명암 조절 Neuz END

		// sun 8차 버프창 관련 적용
		else if(scan.Token == _T( "BUFFSTATUSMODE" ))
		{
			m_BuffStatusMode = scan.GetNumber();
		}
	//sun: 11, 줌옵션 추가
		else if(scan.Token == _T( "ZOOMLIMIT" ))
		{
			m_bZoomLimit = scan.GetNumber();
		}
	#ifdef __SFX_OPT
		else if(scan.Token == _T( "SFXLEVEL" ))
		{
			m_nSfxLevel = scan.GetNumber();
		}
	#endif
		else if(scan.Token == _T( "WEATHEREFFECT" ))
		{
			m_nWeatherEffect = scan.GetNumber();
		}
	//sun: 12, 옵션창 개선
		else if(scan.Token == _T( "VIEWMASK" ))
		{
			m_bViewMask = scan.GetNumber();
		}
	} while( scan.tok != FINISHED );

//sun: Wide및 고해상도 추가
	CheckResolution();

//sun: 12, 옵션창 개선
	m_nChatCommand				= 0;
	m_nSlangWord				= 0;
	m_nInterface				= 1;
	m_bRollEffect				= FALSE;
	m_MouseSpeed				= 1;
	m_bVisibleBuffTimeRender	= TRUE;
	m_bShout					= TRUE;
	m_bMessengerJoin   			= TRUE;
	return 1;
}

int	COption::Save( LPCTSTR szFileName )
{
	FILE *fp = FLStdFileOpen( szFileName, _T( "wt" ) );
	if(fp == NULL) 
		return 0;

	_ftprintf(fp, _T( "// neuz browser ini file\n\n" ) );
	_ftprintf(fp, _T( "// option\n" ) ,0);
	_ftprintf(fp, _T( "resolution %d %d\n" ) ,m_nResWidth, m_nResHeight );
	_ftprintf(fp, _T( "fullscreen %d\n" ) ,m_bStartFullScreen );
	_ftprintf(fp, _T( "texture %d\n" ) ,m_nTextureQuality );
	_ftprintf(fp, _T( "view %d\n" ) ,m_nViewArea );
	_ftprintf(fp, _T( "detail %d\n" ) ,m_nObjectDetail );
	_ftprintf(fp, _T( "distant %d\n" ) ,m_nObjectDistant );
	_ftprintf(fp, _T( "shadow %d\n" ) ,m_nShadow );
	_ftprintf(fp, _T( "bloom %d\n" ) ,m_nBloom );
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
	_ftprintf(fp, _T( "BattleBGM %d\n" ), m_bBattleBGM );
	_ftprintf(fp, _T( "GameGrade %d\n" ), m_bGameGradeRendering );
	_ftprintf(fp, _T( "Server %d %d\n" ), m_nSer, m_nMSer );

	if( m_nToggleScreen )
		_ftprintf(fp, _T( "sunkist %d\n" ), m_nToggleScreen );

	_ftprintf(fp, _T( "Sound %d %.1f %.1f\n" ), m_bVoice, m_fEffectVolume, m_fBGMVolume );
	_ftprintf(fp, _T( "Guide %d\n" ), m_nGuide1 );
	_ftprintf(fp, _T( "Guide2 %d\n" ), m_nGuide2 );
	_ftprintf(fp, _T( "Guide3 %d\n" ), m_nGuide3 );
	_ftprintf(fp, _T( "SkillShowMessage %d\n" ), m_bIsShowReSkillMessage );
	_ftprintf(fp, _T( "ChatFilter0 %d\n" ), m_dwChatFilter[0] );
	_ftprintf(fp, _T( "ChatFilter1 %d\n" ), m_dwChatFilter[1] );
	_ftprintf(fp, _T( "ChatFilter2 %d\n" ), m_dwChatFilter[2] );
	_ftprintf(fp, _T( "ChatFilter3 %d\n" ), m_dwChatFilter[3] );
	_ftprintf(fp, _T( "ChatFilter4 %d\n" ), m_dwChatFilter[4] );
	_ftprintf(fp, _T( "ChatFilterEtc %d\n" ), m_dwChatFilterEtc );

#ifdef __YENV
	if( m_bExitChange )
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), !m_bSpecBump );
	else
		_ftprintf(fp, _T( "SpecBumpEffect %d\n" ), m_bSpecBump );	
#endif //__YENV

	_ftprintf(fp, _T( "ip \"%s\"\n" ), m_IPAddress);
	_ftprintf(fp, _T( "DamageRender %d\n" ), m_bDamageRender );
	_ftprintf(fp, _T( "BuffTimeRender %d\n" ), m_bVisibleBuffTimeRender );
	_ftprintf(fp, _T( "MouseSpeed %d\n" ), m_MouseSpeed );
	_ftprintf(fp, _T( "SFXRenderOff %d\n" ), m_bSFXRenderOff );
	_ftprintf(fp, _T( "CameraLock %d\n" ), m_bCameraLock );
	_ftprintf(fp, _T( "AutoAttack %d\n" ), m_bAutoAttack );
	_ftprintf(fp, _T( "PRenderName %d\n" ), m_nPlayerName );
	_ftprintf(fp, _T( "OPRenderName %d\n" ), m_nOtherPlayerName );
	_ftprintf(fp, _T( "NRenderName %d\n" ), m_nMonName );

	//sun: 밝기, 감마, 명암 조절 Neuz B
	_ftprintf(fp, _T( "BRIGHT %d\n" ),   m_nOverBright );
	_ftprintf(fp, _T( "GAMMA %f\n" ),    m_fGamma );
	_ftprintf(fp, _T( "CONTRAST %f\n" ), m_fContrast );
	//sun: 밝기, 감마, 명암 조절 Neuz E
			
	_ftprintf(fp, _T( "BUFFSTATUSMODE %d\n" ), m_BuffStatusMode );	// sun 8차 버프창 관련 적용

	_ftprintf(fp, _T( "ZOOMLIMIT %d\n" ), m_bZoomLimit );			//sun: 11, 줌옵션 추가

#ifdef __SFX_OPT
	_ftprintf(fp, _T( "SFXLEVEL %d\n" ), m_nSfxLevel );
#endif
	_ftprintf(fp, _T( "WEATHEREFFECT %d\n" ), m_nWeatherEffect );

	_ftprintf(fp, _T( "VIEWMASK %d\n" ), m_bViewMask );				//sun: 12, 옵션창 개선

	fclose(fp);
	return 1;
}

