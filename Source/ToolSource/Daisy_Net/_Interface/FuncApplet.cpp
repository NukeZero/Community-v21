// AppMain.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndNotice.h"
#include "WndMessenger.h"
#include "WndAdminBalance.h"
#include "WndBank.h"		 // 은행 / 창고
#include "WndRepairItem.h"	 // 수리창
#include "WndDebugInfo.h"    // 디버그 창 
#include "WndAdmin.h"        // 운영자 매뉴 
#include "WndHelp.h"

#include "WndWebBox.h"

#include "WndVendor.h"
#include "WndCommItem.h"

/*
     여기서 정의된 애플랫 클래스는 테스크 매뉴에 추가될 수 있는 세팅을 포함한다.
	 태스크 매뉴에 애플렛을 추가하기 위해서는 

	 Step. 1 - Applet ID 정의
	   ResData.h를 열고 Applet ID를 정의한다. 매인프로세서는 Applet ID로 Applet을 인식한다.
	   만약 Daisy를 사용할 경우라면 Daisy에서 Id를 지정해주면 된다. 이경우 직접 ResData.h를 수정할 필요는 없다.

	 Step. 2 - DECLAREAPPLET 생성자 함수 선언하기
	   DECLAREAPPLET는 실행 명령이 내려졌을 때 해당 class를 할당하여 매인 프로세서에게 포인터를
	   돌려주는 역할을 한다. 이 함수가 정의되어있지 않으면 매인 프로세서는 class를 실행 시킬 수 없게 된다.
	   DECLAREAPPLET은 AppMain 함수를 만드는 것을 간단하게 정의한 define이다. 용법은 다음과 같다.
	   DECLAREAPPLET( 함수명, new className ); 

	 Step. 3 - map에 Add하기. 마지막 필드의 툴팁 세팅은 Resource\textClient.inc를 참고. 
*/
   
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "WndExtra.h"
#include "WndQuest.h"
#include "WndMotion.h"
#include "WndPvp.h"

#include "WndGuildVote.h"

DECLAREAPPLET( AppMain_WndNavigator   , new CWndNavigator   );

DECLAREAPPLET( AppMain_WndCharacter       , new CWndCharacter       );
DECLAREAPPLET( AppMain_WndJob         , new CWndJob         );
DECLAREAPPLET( AppMain_WndSkill       , new CWndSkill       );
DECLAREAPPLET( AppMain_WndSkill2     , new CWndSkillTreeEx       );
DECLAREAPPLET( AppMain_WndMacro       , new CWndMacro       );
DECLAREAPPLET( AppMain_WndStyle       , new CWndStyle       );
DECLAREAPPLET( AppMain_WndEmotion     , new CWndEmotion     );
DECLAREAPPLET( AppMain_WndMotion      , new CWndMotion      );
DECLAREAPPLET( AppMain_WndTrade       , new CWndTrade       );
DECLAREAPPLET( AppMain_WndVendor      ,	new CWndVendor );
DECLAREAPPLET( AppMain_WndQuest       , new CWndQuest       );
DECLAREAPPLET( AppMain_WndPvp         , new CWndPvp       );
DECLAREAPPLET( AppMain_WndInventory   , new CWndInventory   );
DECLAREAPPLET( AppMain_WndWebBox   , new CWndWebBox  );
#ifdef __TAIWAN_WEBBOX
DECLAREAPPLET( AppMain_WndWebBox2	, new	CWndWebBox2	);
#endif	// __TAIWAN_WEBBOX
DECLAREAPPLET( AppMain_WndFriend      , new CWndFriend );
DECLAREAPPLET( AppMain_WndParty       , new CWndParty  );
DECLAREAPPLET( AppMain_WndGuild       , new CWndGuild  );
DECLAREAPPLET( AppMain_WndGuildVote   , new CWndGuildVote  );
DECLAREAPPLET( AppMain_WndCommItem    , new CWndCommItem  );
DECLAREAPPLET( AppMain_WndUpgradeBase , new CWndUpgradeBase  );

#ifdef __PIERCING_SM_TIME	
DECLAREAPPLET( AppMain_WndPiercing  , new CWndPiercing  );
#endif //__PIERCING_SM_TIME


DECLAREAPPLET( AppMain_WndClub        , new CWndClub   );

DECLAREAPPLET( AppMain_WndChat        , new CWndChat    );
DECLAREAPPLET( AppMain_WndMessenger   , new CWndMessenger );
DECLAREAPPLET( AppMain_WndEMail       , new CWndEMail   );
DECLAREAPPLET( AppMain_WndBBS         , new CWndBBS     );

DECLAREAPPLET( AppMain_WndSearchPeople , new CWndSearchPeople);
DECLAREAPPLET( AppMain_WndSearchGuild  , new CWndSearchGuild );
DECLAREAPPLET( AppMain_WndSearchClub   , new CWndSearchClub  );
DECLAREAPPLET( AppMain_WndSearchShop   , new CWndSearchShop  );

#ifndef __N_OPTIONUNITY_041209
DECLAREAPPLET( AppMain_WndOptVideo     , new CWndOptVideo    );
#endif//__N_OPTIONUNITY_041209
DECLAREAPPLET( AppMain_WndOptTheme     , new CWndOptTheme    );
DECLAREAPPLET( AppMain_WndOptSound     , new CWndOptSound    );
DECLAREAPPLET( AppMain_WndOptMusic     , new CWndOptMusic    );
DECLAREAPPLET( AppMain_WndOptWindow    , new CWndOptWindow   );
DECLAREAPPLET( AppMain_WndOptSlang     , new CWndOptSlang    );
DECLAREAPPLET( AppMain_WndOptPassword  , new CWndOptPassword );
DECLAREAPPLET( AppMain_WndOptMouse     , new CWndOptMouse    );
DECLAREAPPLET( AppMain_WndOptMyInfo    , new CWndOptMyInfo   );
DECLAREAPPLET( AppMain_WndOptHotkey    , new CWndOptHotkey   );
#ifndef __N_OPTIONUNITY_041209
DECLAREAPPLET( AppMain_WndOption       , new CWndOption      );
DECLAREAPPLET( AppMain_WndOptionGame   , new CWndOptionGame  );
#else//__N_OPTIONUNITY_041209
DECLAREAPPLET( AppMain_WndOption       , new CWndTotalOption );
#endif//__N_OPTIONUNITY_041209

DECLAREAPPLET( AppMain_WndInfoNotice   , new CWndInfoNotice  );
DECLAREAPPLET( AppMain_WndInfoEvent    , new CWndInfoEvent   );
DECLAREAPPLET( AppMain_WndInfoVersion  , new CWndInfoVersion );
DECLAREAPPLET( AppMain_WndInfoSchedule , new CWndInfoSchedule);

DECLAREAPPLET( AppMain_WndHelpHelp     , new CWndHelp        );
DECLAREAPPLET( AppMain_WndHelpTip      , new CWndHelpTip     );
DECLAREAPPLET( AppMain_WndHelpFAQ      , new CWndHelpFAQ     );

DECLAREAPPLET( AppMain_WndJoinVote     , new CWndJoinVote    );
DECLAREAPPLET( AppMain_WndJoinEvent    , new CWndJoinEvent   );

DECLAREAPPLET( AppMain_WndExtraCapture , new CWndExtraCapture);

DECLAREAPPLET( AppMain_WndLogOut       , new CWndLogOut      );
DECLAREAPPLET( AppMain_WndQuit         , new CWndQuit        );

DECLAREAPPLET( AppMain_WndSequence     , new CWndSequence    );

DECLAREAPPLET( AppMain_WndApplet       , new CWndApplet      );
DECLAREAPPLET( AppMain_WndAdmin        , new CWndAdmin       );
DECLAREAPPLET( AppMain_WndWorld        , new CWndWorld       );
//DECLAREAPPLET( AppMain_WndDialog      , new CWndDialog      );
DECLAREAPPLET( AppMain_WndDebugInfo    , new CWndDebugInfo   );
DECLAREAPPLET( AppMain_WndAdminBalance , new CWndAdminBalance );
DECLAREAPPLET( AppMain_WndStatus       , new CWndStatus       );
DECLAREAPPLET( AppMain_WndControlTest  , new CWndControlTest );

// Title
DECLAREAPPLET( AppMain_WndLogin        , new CWndLogin        );
DECLAREAPPLET( AppMain_WndSelectServer , new CWndSelectServer );
DECLAREAPPLET( AppMain_WndCreateChar   , new CWndCreateChar   );
DECLAREAPPLET( AppMain_WndSelectChar   , new CWndSelectChar   );
//DECLAREAPPLET( AppMain_WndRegistration , new CWndRegistration );
//DECLAREAPPLET( AppMain_WndAbout        , new CWndAbout        );
//DECLAREAPPLET( AppMain_WndOption       , new CWndOption       );

void CWndMgr::AddAllApplet()
{
	//             생성자                    ID                            타이틀                    아이콘                          툴팁 텍스트 
	AddAppletFunc( AppMain_WndNavigator    , APP_NAVIGATOR               , _T( "WndNavigator" )    , _T( "Icon_Navigator.dds" )    , GETTEXT( TID_TIP_NAVIGATOR             ),  'N'  );

	AddAppletFunc( AppMain_WndStatus       , APP_STATUS                  , _T( "WndStatus" )       , _T( "Icon_Status.dds"    )    , GETTEXT( TID_TIP_STATUS                ), 'T' ); 
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER             ), 'H' );
	AddAppletFunc( AppMain_WndInventory    , APP_INVENTORY               , _T( "WndInventory" )    , _T( "Icon_Inventory.dds" )    , GETTEXT( TID_TIP_INVENTORY             ), 'I' );
	
	AddAppletFunc( AppMain_WndWebBox    , APP_WEBBOX               , _T( "WebBox" )    , _T( "Icon_CitemMall.dds" )    , GETTEXT( TID_TIP_ITEMMALL             ), 0 );

#ifdef __WEB_GM_HELP1201
//	Icon_CGMHelp.DDS
	AddAppletFunc( AppMain_WndWebBox    , APP_WEBBOX               , _T( "WebBox" )    , _T( "Icon_CGMHelp.DDS" )    , GETTEXT( TID_TIP_ITEMMALL             ), 0 );
#endif	// __WEB_GM_HELP1201

#ifdef __TAIWAN_WEBBOX
	AddAppletFunc( AppMain_WndWebBox2	, APP_WEBBOX2				, _T( "WebBox2" )	, _T( "Icon_CGMHelp.DDS" )		, GETTEXT( TID_TIP_GM_HELP), 0 );
#endif	// __TAIWAN_WEBBOX

//	AddAppletFunc( AppMain_WndJob          , APP_JOB                     , _T( "WndJob"       )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_JOB                   ),  0  );
	AddAppletFunc( AppMain_WndSkill2         , APP_SKILL                   , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL                 ), 'K' );
	AddAppletFunc( AppMain_WndMacro        , APP_MACRO                   , _T( "WndMacro"     )    , _T( "Icon_Macro.dds"     )    , GETTEXT( TID_TIP_MACRO                 ),  0  );
	AddAppletFunc( AppMain_WndStyle        , APP_STYLE                   , _T( "WndStyle"     )    , _T( "Icon_Style.dds"     )    , GETTEXT( TID_TIP_STYLE                 ),  0  );
	AddAppletFunc( AppMain_WndMotion       , APP_MOTION                  , _T( "WndMotion"    )    , _T( "Icon_Motion.dds"   )     , GETTEXT( TID_TIP_MOTION               ),  'M'  );
	AddAppletFunc( AppMain_WndTrade        , APP_TRADE                   , _T( "WndTrade"     )    , _T( "Icon_Trade.dds"     )    , GETTEXT( TID_TIP_TRADE                 ),  0  );

	AddAppletFunc( AppMain_WndVendor	, APP_VENDOREX	, _T( "WndVendor" )	, _T( "Icon_Applet.dds" ),	GETTEXT( TID_TIP_VENDOR ), 'R' );

   	AddAppletFunc( AppMain_WndQuest        , APP_QUEST                   , _T( "WndQuest"     )    , _T( "Icon_Quest.dds"     )    , GETTEXT( TID_TIP_QUEST                 ), 'Q' );
//	AddAppletFunc( AppMain_WndPvp		   , APP_PVP					 , _T( "WndPvp" )		   , _T( "Icon_Pvp.dds" )		   , GETTEXT( TID_TIP_PVP					), 'V' );
	
	//AddAppletFunc( AppMain_WndFriend       , APP_COMMUNITY_FRIEND        , _T( "WndFriend"    )    , _T( "Icon_Friend.dds"    )    , GETTEXT( TID_TIP_COMMUNITY_FRIEND      ),  0  );
	AddAppletFunc( AppMain_WndParty        , APP_PARTY                   , _T( "WndParty"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY                 ),  'P' );

	AddAppletFunc( AppMain_WndGuild        , APP_GUILD                   , _T( "WndGuild"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY                 ),  'G' );

#ifdef __GUILDVOTE
	AddAppletFunc( AppMain_WndGuildVote    , APP_GUILD_VOTE                   , _T( "WndGuildVote"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY                 ),  'V' );
#endif	
	
	AddAppletFunc( AppMain_WndCommItem    , APP_COMM_ITEM                   , _T( "WndCommItem"     )    , _T( "Icon_CItemTime.dds"    )    , GETTEXT( TID_TIP_ITEMTIME          ),  'J' );
	
	AddAppletFunc( AppMain_WndUpgradeBase    , APP_TEST               , _T( "WndUpgradeBase"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY                 ),  0 );

#ifdef __PIERCING_SM_TIME	
	AddAppletFunc( AppMain_WndPiercing    , APP_PIERCING               , _T( "WndPiercing"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY                 ),  0 );
#endif //__PIERCING_SM_TIME
	

	//	AddAppletFunc( AppMain_WndGuild        , APP_COMMUNITY_GUILD         , _T( "WndGuild"     )    , _T( "Icon_Guild.dds"     )    , GETTEXT( TID_TIP_COMMUNITY_GUILD       ),  0  );
	AddAppletFunc( AppMain_WndClub         , APP_COMMUNITY_CLUB          , _T( "WndClub"      )    , _T( "Icon_Club.dds"      )    , GETTEXT( TID_TIP_COMMUNITY_CLUB        ),  0  );

	AddAppletFunc( AppMain_WndChat         , APP_COMMUNICATION_CHAT      , _T( "WndChat"      )    , _T( "Icon_Chat.dds"      )    , GETTEXT( TID_TIP_COMMUNICATION_CHAT    ),  0 );
	AddAppletFunc( AppMain_WndMessenger    , APP_MESSENGER_              , _T( "WndMessenger" )    , _T( "Icon_Messenger.dds"   )    , GETTEXT( TID_TIP_MESSENGER ),  'E'  );
	AddAppletFunc( AppMain_WndEMail        , APP_COMMUNICATION_EMAIL     , _T( "WndEMail"     )    , _T( "Icon_EMail.dds"     )    , GETTEXT( TID_TIP_COMMUNICATION_EMAIL   ),  0  );
	AddAppletFunc( AppMain_WndBBS          , APP_COMMUNICATION_BBS       , _T( "WndBBS"       )    , _T( "Icon_BBS.dds"       )    , GETTEXT( TID_TIP_COMMUNICATION_BBS     ),  0  );

	AddAppletFunc( AppMain_WndSearchPeople , APP_FIND_PEOPLE             , _T( "WndSearchPeople" ) , _T( "Icon_FindchPeople.dds" ) , GETTEXT( TID_TIP_FIND_PEOPLE           ),  0  );
	AddAppletFunc( AppMain_WndSearchGuild  , APP_FILD_GUILD              , _T( "WndSearchGuild"  ) , _T( "Icon_SearchGuild.dds"  ) , GETTEXT( TID_TIP_FILD_GUILD            ),  0  );
	AddAppletFunc( AppMain_WndSearchClub   , APP_FIND_CLUB               , _T( "WndSearchClub"   ) , _T( "Icon_SearchClub.dds"   ) , GETTEXT( TID_TIP_FIND_CLUB             ),  0  );
	AddAppletFunc( AppMain_WndSearchShop   , APP_FIND_SHOP               , _T( "WndSearchShop"   ) , _T( "Icon_SearchShop.dds"   ) , GETTEXT( TID_TIP_FIND_SHOP             ),  0  );

#ifndef __N_OPTIONUNITY_041209
	AddAppletFunc( AppMain_WndOptVideo     , APP_OPTION_VIDEO            , _T( "WndOptVideo" )     , _T( "Icon_OptVideo.dds" )     , GETTEXT( TID_TIP_OPTION_VIDEO          ),  0  );
#endif// __N_OPTIONUNITY_041209
	AddAppletFunc( AppMain_WndOptSound     , APP_OPTION_SOUND            , _T( "WndOptSound" )     , _T( "Icon_OptSound.dds" )     , GETTEXT( TID_TIP_OPTION_SOUND          ),  0  );
	AddAppletFunc( AppMain_WndOptMusic     , APP_OPTION_MUSIC            , _T( "WndOptMusic" )     , _T( "Icon_OptMusic.dds" )     , GETTEXT( TID_TIP_OPTION_MUSIC          ),  0  );
	AddAppletFunc( AppMain_WndOptPassword  , APP_OPTION_PASSWORD         , _T( "WndOptPassword" )  , _T( "Icon_Applet.dds"      )  , GETTEXT( TID_TIP_OPTION_PASSWORD       ),  0  );
	AddAppletFunc( AppMain_WndOptWindow    , APP_OPTION_WINDOW           , _T( "WndOptWindow" )    , _T( "Icon_Applet.dds"      )  , GETTEXT( TID_TIP_OPTION_WINDOW         ),  0  );
	AddAppletFunc( AppMain_WndOptSlang     , APP_OPTION_SLANG            , _T( "WndOptSlang" )     , _T( "Icon_Applet.dds"      )  , GETTEXT( TID_TIP_OPTION_SLANG          ),  0  );
	AddAppletFunc( AppMain_WndOptMouse     , APP_OPTION_MOUSE            , _T( "WndOptMouse" )     , _T( "Icon_OptMouse.dds" )     , GETTEXT( TID_TIP_OPTION_MOUSE          ),  0  );
	AddAppletFunc( AppMain_WndOptMyInfo    , APP_OPTION_MYINFO           , _T( "WndOptMyInfo" )    , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_MYINFO         ),  0  );
	AddAppletFunc( AppMain_WndOptHotkey    , APP_OPTION_HOTKEY           , _T( "WndOptHotkey" )    , _T( "Icon_OptHotkey.dds" )    , GETTEXT( TID_TIP_OPTION_HOTKEY         ),  0  );
	AddAppletFunc( AppMain_WndOptTheme     , APP_OPTION_THEME            , _T( "WndOptTheme" )     , _T( "Icon_OptTheme.dds" )     , GETTEXT( TID_TIP_OPTION_THEME          ),  0  );
#ifndef __N_OPTIONUNITY_041209
	AddAppletFunc( AppMain_WndOption       , APP_OPTION_ETC              , _T( "WndOption" )       , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_ETC            ),  0  );
	AddAppletFunc( AppMain_WndOptionGame   , APP_OPTION_GAME             , _T( "WndOptionGame" )   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_GAME           ),  0  );	
#else//__N_OPTIONUNITY_041209
	AddAppletFunc( AppMain_WndOption	   , APP_OPTIONEX				 , _T( "WndOption" )	   , _T( "Icon_Applet.dds" )	   , GETTEXT( TID_APP_OPTION				),	0  );			
#endif//__N_OPTIONUNITY_041209

	AddAppletFunc( AppMain_WndInfoNotice   , APP_INFO_NOTICE             , _T( "WndInfoNotice" )   , _T( "Icon_Applet.dds" )   , GETTEXT( TID_TIP_INFO_NOTICE           ),  0  );
	AddAppletFunc( AppMain_WndInfoEvent    , APP_INFO_EVENT              , _T( "WndInfoEvent" )    , _T( "Icon_InfoEvent.dds" )    , GETTEXT( TID_TIP_INFO_EVENT            ),  0  );
	AddAppletFunc( AppMain_WndInfoVersion  , APP_INFO_VERSION            , _T( "WndInfoVersion" )  , _T( "Icon_InfoVersion.dds" )  , GETTEXT( TID_TIP_INFO_VERSION          ),  0  );
	AddAppletFunc( AppMain_WndInfoSchedule , APP_INFO_SCHEDULE           , _T( "WndInfoSchedule" ) , _T( "Icon_InfoSchedule.dds" ) , GETTEXT( TID_TIP_INFO_SCHEDULE         ),  0  );

	AddAppletFunc( AppMain_WndHelpHelp     , APP_HELPER_HELP             , _T( "WndHelpHelp" )     , _T( "Icon_HelperHelp.dds" )   , GETTEXT( TID_TIP_HELPER_HELP           ),  0  );
	AddAppletFunc( AppMain_WndHelpTip      , APP_HELPER_TIP              , _T( "WndHelpTip" )      , _T( "Icon_HelperTip.dds" )    , GETTEXT( TID_TIP_HELPER_TIP            ),  0  );
	AddAppletFunc( AppMain_WndHelpFAQ      , APP_HELPER_FAQ              , _T( "WndHelpFAQ" )      , _T( "Icon_HelperFAQ.dds" )    , GETTEXT( TID_TIP_HELPER_FAQ            ),  0  );

	AddAppletFunc( AppMain_WndJoinVote     , APP_JOIN_VOTE               , _T( "WndJoinVote" )     , _T( "Icon_JoinVote.dds" )     , GETTEXT( TID_TIP_JOIN_VOTE             ),  0  );
	AddAppletFunc( AppMain_WndJoinEvent    , APP_JOIN_EVENT              , _T( "WndJoinEvent" )    , _T( "Icon_JoinEvent.dds" )    , GETTEXT( TID_TIP_JOIN_EVENT            ),  0  );

	AddAppletFunc( AppMain_WndExtraCapture , APP_EXTRA_CAPTURE           , _T( "WndExtraCapture" )    , _T( "Icon_JoinEvent.dds" ) , GETTEXT( TID_TIP_EXTRA_CAPTURE         ),  0  );

	AddAppletFunc( AppMain_WndLogOut       , APP_LOGOUT                  , _T( "WndLogout"   )     , _T( "Icon_Logout.dds"   )     , GETTEXT( TID_TIP_LOGOUT              ),  0  );
	AddAppletFunc( AppMain_WndQuit         , APP_QUIT                    , _T( "WndQuit"      )    , _T( "Icon_Quit.dds"      )    , GETTEXT( TID_TIP_QUIT                  ),  0  );

	AddAppletFunc( AppMain_WndApplet       , APP_APPLET                  , _T( "WndApplet" )       , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_APPLET                ),  0  );
	AddAppletFunc( AppMain_WndAdmin        , APP_ADMIN                   , _T( "WndAdmin" )        , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_ADMIN                 ),  0  );
	AddAppletFunc( AppMain_WndWorld        , APP_WORLD                   , _T( "WndWorld" )        , _T( "Icon_Applet.dds" )        , GETTEXT( TID_TIP_WORLD                 ),  0  );
	//AddAppletFunc( AppMain_WndDialog       , APP_DIALOG                  , _T( "WndDialog" )       , _T( "Icon_Dialog.dds" )       , GETTEXT( TID_TIP_DIALOG                ),  0  );
	AddAppletFunc( AppMain_WndDebugInfo    , APP_DEBUGINFO               , _T( "WndDebugInfo" )    , _T( "Icon_Applet.dds" )        , GETTEXT( TID_TIP_DEBUGINFO             ),  0  );
	AddAppletFunc( AppMain_WndAdminBalance  , APP_ADMIN_BALANCE, _T( "WndAdminBalance" )       , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_DIALOG                ),  0  );
	AddAppletFunc( AppMain_WndControlTest  , APP_CONTROLTEST             , _T( "WndControlTest" )  , _T( "Icon_ControlTest.dds" )  , GETTEXT( TID_TIP_CONTROLTEST           ),  0  );
	AddAppletFunc( AppMain_WndSequence     , APP_SEQUENCE                , _T( "WndSequence"    )  , _T( "Icon_ControlTest.dds" )  , GETTEXT( TID_TIP_CONTROLTEST           ),  0  );

	// title
	AddAppletFunc( AppMain_WndLogin        , APP_LOGIN                   , _T( "WndLogin"      )   , _T( "Icon_Login.dds"      )   , GETTEXT( TID_TIP_APPLET                ),  0  );
	AddAppletFunc( AppMain_WndSelectServer , APP_SELECT_SERVER           , _T( "WndSelectServer")  , _T( "Icon_SelectServer.dds")  , GETTEXT( TID_TIP_WORLD                 ),  0  );
	AddAppletFunc( AppMain_WndCreateChar   , APP_CREATE_CHAR             , _T( "WndCreateChar" )   , _T( "Icon_CreateChar.dds" )   , GETTEXT( TID_TIP_WORLD                 ),  0  );
	AddAppletFunc( AppMain_WndSelectChar   , APP_SELECT_CHAR             , _T( "WndSelectChar" )   , _T( "Icon_SelectChar.dds" )   , GETTEXT( TID_TIP_DIALOG                ),  0  );

}
/*
_T( "Icon_Navigator.dds" )   

_T( "Icon_Character.dds" )   
_T( "Icon_State.dds"    )    
_T( "Icon_Inventory.dds" )   
_T( "Icon_Skill.dds"     )   
_T( "Icon_Skill.dds"     )   
_T( "Icon_Macro.dds"     )   
_T( "Icon_Style.dds"     )   
_T( "Icon_Emotion.dds"   )   
_T( "Icon_Trade.dds"     )   

_T( "Icon_Friend.dds"    )   
_T( "Icon_Party.dds"     )   
_T( "Icon_Guild.dds"     )   
_T( "Icon_Club.dds"      )   

_T( "Icon_Chat.dds"      )   
_T( "Icon_Message.dds"   )   
_T( "Icon_EMail.dds"     )   
_T( "Icon_BBS.dds"       )   

_T( "Icon_FindchPeople.dds" )
_T( "Icon_SearchGuild.dds"  )
_T( "Icon_SearchClub.dds"   )
_T( "Icon_SearchShop.dds"   )

_T( "Icon_OptVideo.dds" )    
_T( "Icon_OptSound.dds" )    
_T( "Icon_OptMusic.dds" )    
_T( "Icon_OptPassword.dds" ) 
_T( "Icon_OptMouse.dds" )    
_T( "Icon_OptMyInfo.dds" )   
_T( "Icon_OptHotkey.dds" )   
_T( "Icon_OptTheme.dds" )    

_T( "Icon_InfoNotice.dds" )  
_T( "Icon_InfoEvent.dds" )   
_T( "Icon_InfoVersion.dds" ) 
_T( "Icon_InfoSchedule.dds" )

_T( "Icon_HelpHelp.dds" )    
_T( "Icon_HelpTip.dds" )     
_T( "Icon_HelpFAQ.dds" )     

_T( "Icon_JoinVote.dds" )    
_T( "Icon_JoinEvent.dds" )   

_T( "Icon_Logout.dds"   )    
_T( "Icon_Quit.dds"      )   

_T( "Icon_Applet.dds" )      
_T( "Icon_Applet.dds" )      
_T( "Icon_World.dds" )       
_T( "Icon_Dialog.dds" )      
_T( "Icon_Debug.dds" )       
_T( "Icon_ControlTest.dds" ) 
_T( "Icon_ControlTest.dds" ) 


_T( "Icon_Login.dds"      )  
_T( "Icon_SelectServer.dds") 
_T( "Icon_CreateChar.dds" )  
_T( "Icon_SelectChar.dds" )  

  */