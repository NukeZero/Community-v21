#include "stdafx.h"
#include <stdio.h>

#include "HwOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  

COption		g_Option;

#define CScannerEx	CScanner

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
	m_nLanguage = LANG_KOR; // default

//	m_nSoundEffect = 1;
	m_bOperator = FALSE;
	m_nResWidth = 1024;//800;
	m_nResHeight = 768;//600;
	m_bStartFullScreen = TRUE;		// 나중에 정식 서비스 들어가면 1로 바꾸자.
#ifdef __BEAST
//	m_fMusicVolume = 1.0f;
	m_nTextureQuality = 0;//HIGHEST;	//gmpbigsun(20100607) : 음 이전에 바꾸었는데 이사와서 기존코드로 돌아가있네 -_-;
	m_nViewArea = 0;
	m_nObjectDistant = 0;
	m_nObjectDetail = 0;
	m_nShadow = 0;
#else
//	m_fMusicVolume = 1.0f;
	m_nTextureQuality = 0;//HIGHEST;
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


	m_nToolTipText = 0;
	
	m_bVoice        = TRUE;
	m_fEffectVolume = 1.0f;
	m_fBGMVolume    = 0.5f;
	
	m_bIsShowReSkillMessage = TRUE;
	m_pGuide = NULL;


	m_MouseSpeed = 1;
	
#ifdef __YRENDER_FLAG	
	m_bSFXRenderOff = FALSE;	
#endif //__YRENDER_FLAG	
	
}


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
		else
		if( scan.Token == _T( "Sound" ) )
		{
			m_bVoice        = scan.GetNumber();
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume    = scan.GetFloat();
		}
		else
		if( scan.Token == _T( "SkillShowMessage" ) )
		{
			m_bIsShowReSkillMessage = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "ChatFilter" ) )
		{
			m_dwChatFilter = scan.GetNumber();
		}
		else
		if( scan.Token == _T( "MouseSpeed" ) )
		{
			m_MouseSpeed = scan.GetNumber();
		}

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
	if( m_nToggleScreen )
		_ftprintf(fp, _T( "sunkist %d\n" ), m_nToggleScreen );

	_ftprintf(fp, _T( "Sound %d %.1f %.1f\n" ), m_bVoice, m_fEffectVolume, m_fBGMVolume );
	
	_ftprintf(fp, _T( "SkillShowMessage %d\n" ), m_bIsShowReSkillMessage );

	_ftprintf(fp, _T( "ChatFilter %d\n" ), m_dwChatFilter );


	_ftprintf(fp, _T( "MouseSpeed %d\n" ), m_MouseSpeed );

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





















