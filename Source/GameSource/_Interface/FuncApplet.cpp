
#include "StdAfx.h"

#include "WndPvp.h"
#include "WndBank.h"		 		// 은행 / 창고
#include "WndHelp.h"
#include "WndQuest.h"
#include "WndBagEx.h"
#include "WndParty.h"
#include "WndMotion.h"
#include "WndWebBox.h"
#include "WndVendor.h"
#include "WndNotice.h"
#include "WndHousing.h"				//sun: 13, 하우징 시스템
#include "WndCommItem.h"
#include "WndDebugInfo.h"    		// 디버그 창 
#include "WndGuildVote.h"
#include "WndMessenger.h"
#include "WndNavigator.h"
#include "WndRepairItem.h"	 		// 수리창
#include "WndPartyQuick.h"			//sun: 8차 파티창 관련 적용 Neuz
#include "WndGuildHouse.h"
#include "WndElldinsJar.h"			//lenahyang: 19, 엘딘의 항아리
#include "Wnd2ndPassword.h"
#include "WndHelperWebBox.h"
#include "WndMadrigalGift.h"		//lenahyang: 20, 마드리갈의 선물
#include "WndFlyffPieceSystem.h"	//lenahyang: 20, 프리프 조각 시스템
#include "WndInventoryItem.h"		//lenahyang: 20. 인벤토리 리뉴얼 (아이템)
#include "WndInventoryWear.h"		//lenahyang: 20, 인벤토리 리뉴얼 (의상)

#ifdef __IMPROVE_MAP_SYSTEM
	#include "WndMapEx.h"
#else 
	#include "WndMap.h"
#endif 

//---------------------------------------------------------------------------------------------
//	여기서 정의된 애플랫 클래스는 테스크 매뉴에 추가될 수 있는 세팅을 포함한다.
//	태스크 매뉴에 애플렛을 추가하기 위해서는 
//	
//	Step. 1 - Applet ID 정의
//	  ResData.h를 열고 Applet ID를 정의한다. 매인프로세서는 Applet ID로 Applet을 인식한다.
//	  만약 Daisy를 사용할 경우라면 Daisy에서 Id를 지정해주면 된다. 이경우 직접 ResData.h를 수정할 필요는 없다.
//	
//	Step. 2 - DECLAREAPPLET 생성자 함수 선언하기
//	  DECLAREAPPLET는 실행 명령이 내려졌을 때 해당 class를 할당하여 매인 프로세서에게 포인터를
//	  돌려주는 역할을 한다. 이 함수가 정의되어있지 않으면 매인 프로세서는 class를 실행 시킬 수 없게 된다.
//	  DECLAREAPPLET은 AppMain 함수를 만드는 것을 간단하게 정의한 define이다. 용법은 다음과 같다.
//	  DECLAREAPPLET( 함수명, new className ); 
//	
//	Step. 3 - map에 Add하기. 마지막 필드의 툴팁 세팅은 Resource\textClient.inc를 참고. 
//---------------------------------------------------------------------------------------------
  
#pragma region DeclareApplet


	DECLAREAPPLET( AppMain_WndNavigator    			, new CWndNavigator  			);
	DECLAREAPPLET( AppMain_WndCharacter    			, new CWndCharacter  			);
	DECLAREAPPLET( AppMain_WndEmotion      			, new CWndEmotion    			);
	DECLAREAPPLET( AppMain_WndMotion       			, new CWndMotion     			);
	DECLAREAPPLET( AppMain_WndTrade        			, new CWndTrade      			);
	DECLAREAPPLET( AppMain_WndVendor       			, new CWndVendor				);
	DECLAREAPPLET( AppMain_WndQuest        			, new CWndQuest					);
	DECLAREAPPLET( AppMain_WndPvp          			, new CWndPvp					);
	DECLAREAPPLET( AppMain_WndInventory    			, new CWndInventory				);
	DECLAREAPPLET( AppMain_WndWebBox       			, new CWndWebBox				);
	DECLAREAPPLET( AppMain_WndParty        			, new CWndParty					);
	DECLAREAPPLET( AppMain_WndGuild        			, new CWndGuild					);
	DECLAREAPPLET( AppMain_WndCommItem     			, new CWndCommItem				);
	DECLAREAPPLET( AppMain_WndUpgradeBase  			, new CWndUpgradeBase			);
	DECLAREAPPLET( AppMain_WndPiercing     			, new CWndPiercing				);
	DECLAREAPPLET( AppMain_WndChat         			, new CWndChat					);
	DECLAREAPPLET( AppMain_WndOptSound     			, new CWndOptSound    			);
	DECLAREAPPLET( AppMain_WndOptWindow    			, new CWndOptWindow   			);
	DECLAREAPPLET( AppMain_WndOptMyInfo    			, new CWndOptMyInfo   			);
	DECLAREAPPLET( AppMain_WndOption       			, new CWndTotalOption 			);
	DECLAREAPPLET( AppMain_WndOption2      			, new CWndTotalOption 			);
	DECLAREAPPLET( AppMain_WndInfoNotice   			, new CWndInfoNotice  			);
	DECLAREAPPLET( AppMain_WndHelpHelp     			, new CWndHelp        			);
	DECLAREAPPLET( AppMain_WndHelpTip      			, new CWndHelpTip     			);
	DECLAREAPPLET( AppMain_WndHelpFAQ      			, new CWndHelpFAQ     			);
	DECLAREAPPLET( AppMain_WndLogOut       			, new CWndLogOut      			);
	DECLAREAPPLET( AppMain_WndQuit         			, new CWndQuit        			);
	DECLAREAPPLET( AppMain_WndWorld        			, new CWndWorld       			);
	DECLAREAPPLET( AppMain_WndDebugInfo    			, new CWndDebugInfo   			);
	DECLAREAPPLET( AppMain_WndStatus       			, new CWndStatus       			);
	DECLAREAPPLET( AppMain_WndLogin        			, new CWndLogin        			);
	DECLAREAPPLET( AppMain_WndSelectServer 			, new CWndSelectServer 			);
	DECLAREAPPLET( AppMain_WndCreateChar   			, new CWndCreateChar   			);
	DECLAREAPPLET( AppMain_WndSelectChar   			, new CWndSelectChar   			);
	DECLAREAPPLET( AppMain_WndInvenRemoveItem		, new CWndInvenRemoveItem		);
	DECLAREAPPLET( AppMain_WndPartyQuick			, new CWndPartyQuick			);	//sun		: 8, 파티창 관련 적용 Neuz
	DECLAREAPPLET( AppMain_WndBuffStatus			, new CWndBuffStatus			);	//sun		: 8, 버프창 관련 적용
	DECLAREAPPLET( AppMain_BagEx					, new CWndBagEx					);	//sun		: 11, 주머니
	DECLAREAPPLET( AppMain_WndMessenger    			, new CWndMessengerEx 			);	//sun		: 11, 메신저창 개선
	DECLAREAPPLET( AppMain_LordSkill				, new CWndLordSkill				);	//sun		: 12, 군주
	DECLAREAPPLET( AppMain_InfoPang					, new CWndInfoPang				);	//sun		: 12, 튜토리얼 개선
	DECLAREAPPLET( AppMain_Housing					, new CWndHousing				);	//sun		: 13, 하우징 시스템
	DECLAREAPPLET( AppMain_Couple					, new CWndCoupleManager 		);	//sun		: 13, 커플 시스템
	DECLAREAPPLET( AppMain_GuildHousing				, new CWndGuildHousing  		);
	DECLAREAPPLET( AppMain_Wnd2ndPassword			, new CWnd2ndPassword			);
	DECLAREAPPLET( AppMain_WndHelperWebBox			, new CWndHelperWebBox  		);
	DECLAREAPPLET( AppMain_WndTaskBar_HUD   		, new CWndTaskBar_HUD			);	//gmbigsun	: 19, 새로운 작업바
	DECLAREAPPLET( AppMain_WndElldinsJar			, new CWndElldinsJar			);	//lenahyang	: 19, 엘딘의 항아리
	DECLAREAPPLET( AppMain_WndMadirigalGift   		, new CWndMadrigalGift			);	//lenahyang	: 20, 마드리갈의 선물
	DECLAREAPPLET( AppMain_WndFlyffPieceSystem 		, new CWndFlyffPieceSystem		);	//lenahyang	: 20, 프리프 조각 시스템
	DECLAREAPPLET( AppMain_WndInventoryWear 		, new CWndInventoryWear			);	//lenahyang	: 20, 인벤토리 리뉴얼 (의상)
	DECLAREAPPLET( AppMain_WndInventoryItem 		, new CWndInventoryItem			);	//lenahyang	: 20, 인벤토리 리뉴얼 (아이템)


	#ifdef __IMPROVE_MAP_SYSTEM															//sun		: 9, 스타트메뉴에 전체지도 메뉴 추가
		DECLAREAPPLET( AppMain_WndMap				, new CWndMapEx					);
	#else 
		DECLAREAPPLET( AppMain_WndMap				, new CWndMap					);
	#endif 

	#ifdef __SKILL_UI16
		DECLAREAPPLET( AppMain_WndSkill2			, new CWndSkill_16				);
	#else  
		DECLAREAPPLET( AppMain_WndSkill2			, new CWndSkillTreeEx			);
	#endif 

	#ifdef __GUILDVOTE
		DECLAREAPPLET( AppMain_WndGuildVote			, new CWndGuildVote				);
	#endif


#pragma endregion DeclareApplet


#pragma region AddAllApplet

//---------------------------------------------------------------------------------------------
// 모든 애플릿을 추가한다.
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMgr::AddAllApplet()
{
	PFLSHORTCUTKEY_UI pShortcutKey_UI = g_xKeyManager->GetShortcutKey_UI();

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//				| 생성자						| ID							| 타이틀						| 아이콘						| 툴팁 텍스트										| 단축키
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	AddAppletFunc( AppMain_WndNavigator				, APP_NAVIGATOR               	, _T( "WndNavigator" )    		, _T( "Icon_Navigator.dds" )    , GETTEXT( TID_TIP_NAVIGATOR )						, pShortcutKey_UI->KeyNavigation		);	
	AddAppletFunc( AppMain_WndQuest					, APP_QUEST_EX_LIST           	, _T( "WndQuest" )    			, _T( "Icon_Quest.dds" )		, GETTEXT( TID_TIP_QUEST )							, pShortcutKey_UI->KeyQuest				);
	AddAppletFunc( AppMain_WndParty					, APP_PARTY                   	, _T( "WndParty" )    			, _T( "Icon_Troupe.dds" )		, GETTEXT( TID_TIP_PARTY )							, pShortcutKey_UI->KeyParty 			);
	AddAppletFunc( AppMain_WndGuild					, APP_GUILD                   	, _T( "WndGuild" )    			, _T( "Icon_Troupe.dds" )		, GETTEXT( TID_TIP_COMPANY )						, pShortcutKey_UI->KeyGuild 			);
	AddAppletFunc( AppMain_WndCommItem				, APP_COMM_ITEM               	, _T( "WndCommItem" )    		, _T( "Icon_CItemTime.dds" )    , GETTEXT( TID_TIP_ITEMTIME )						, pShortcutKey_UI->KeyItemContinuedDate );
	AddAppletFunc( AppMain_WndUpgradeBase   		, APP_TEST						, _T( "WndUpgradeBase" )  		, _T( "Icon_Troupe.dds" )		, GETTEXT( TID_TIP_PARTY )							, 0 									);
	AddAppletFunc( AppMain_WndPiercing				, APP_PIERCING					, _T( "WndPiercing" ) 			, _T( "Icon_Troupe.dds" )		, GETTEXT( TID_TIP_PARTY )							, 0 									);
	AddAppletFunc( AppMain_WndChat         			, APP_COMMUNICATION_CHAT     	, _T( "WndChat" )    			, _T( "Icon_Chat.dds" )			, GETTEXT( TID_TIP_COMMUNICATION_CHAT )				, 0 									);
	AddAppletFunc( AppMain_WndMessenger    			, APP_MESSENGER_             	, _T( "WndMessenger" )    		, _T( "Icon_Messenger.dds" )	, GETTEXT( TID_TIP_MESSENGER )						, pShortcutKey_UI->KeyMessanger			);
	AddAppletFunc( AppMain_WndOptSound     			, APP_OPTION_SOUND           	, _T( "WndOptSound" )     		, _T( "Icon_OptSound.dds" )     , GETTEXT( TID_TIP_OPTION_SOUND )					, 0  									);
	AddAppletFunc( AppMain_WndOptWindow    			, APP_OPTION_WINDOW          	, _T( "WndOptWindow" )    		, _T( "Icon_Applet.dds" )		, GETTEXT( TID_TIP_OPTION_WINDOW )					, 0  									);
	AddAppletFunc( AppMain_WndOptMyInfo    			, APP_OPTION_MYINFO          	, _T( "WndOptMyInfo" )    		, _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_MYINFO )					, 0  									);
	AddAppletFunc( AppMain_WndOption	   			, APP_OPTIONEX				 	, _T( "WndOption" )				, _T( "Icon_Applet.dds" )		, GETTEXT( TID_APP_OPTION )							, 0  									);			
	AddAppletFunc( AppMain_WndOption2	   			, APP_OPTIONEX2				 	, _T( "WndOption" )				, _T( "Icon_Applet.dds" )		, GETTEXT( TID_APP_OPTION )							, 0  									);			
	AddAppletFunc( AppMain_WndHelpHelp     			, APP_HELPER_HELP            	, _T( "WndHelpHelp" )     		, _T( "Icon_HelperHelp.dds" )   , GETTEXT( TID_TIP_HELPER_HELP )					, 0  									);
	AddAppletFunc( AppMain_WndHelpTip      			, APP_HELPER_TIP             	, _T( "WndHelpTip" )      		, _T( "Icon_HelperTip.dds" )    , GETTEXT( TID_TIP_HELPER_TIP )						, 0  									);
	AddAppletFunc( AppMain_WndHelpFAQ      			, APP_HELPER_FAQ             	, _T( "WndHelpFAQ" )      		, _T( "Icon_HelperFAQ.dds" )    , GETTEXT( TID_TIP_HELPER_FAQ )						, 0  									);
	AddAppletFunc( AppMain_WndLogOut       			, APP_LOGOUT                 	, _T( "WndLogout" )     		, _T( "Icon_Logout.dds" )		, GETTEXT( TID_TIP_LOGOUT )							, 0  									);
	AddAppletFunc( AppMain_WndQuit         			, APP_QUIT                   	, _T( "WndQuit" )    			, _T( "Icon_Quit.dds" )			, GETTEXT( TID_TIP_QUIT )							, 0  									);
	AddAppletFunc( AppMain_WndWorld        			, APP_WORLD                  	, _T( "WndWorld" )        		, _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_WORLD )							, 0  									);
	AddAppletFunc( AppMain_WndDebugInfo    			, APP_DEBUGINFO              	, _T( "WndDebugInfo" )    		, _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_DEBUGINFO )						, 0  									);
	AddAppletFunc( AppMain_WndLogin        			, APP_LOGIN                  	, _T( "WndLogin" )   			, _T( "Icon_Login.dds" )		, GETTEXT( TID_TIP_APPLET )							, 0  									);
	AddAppletFunc( AppMain_WndSelectServer 			, APP_SELECT_SERVER          	, _T( "WndSelectServer")  		, _T( "Icon_SelectServer.dds")  , GETTEXT( TID_TIP_WORLD )							, 0  									);
	AddAppletFunc( AppMain_WndCreateChar   			, APP_CREATE_CHAR            	, _T( "WndCreateChar" )   		, _T( "Icon_CreateChar.dds" )   , GETTEXT( TID_TIP_WORLD )							, 0  									);
	AddAppletFunc( AppMain_WndSelectChar   			, APP_SELECT_CHAR            	, _T( "WndSelectChar" )   		, _T( "Icon_SelectChar.dds" )   , GETTEXT( TID_TIP_DIALOG )							, 0  									);
	AddAppletFunc( AppMain_Wnd2ndPassword  			, APP_2ND_PASSWORD_NUMBERPAD 	, _T( "Wnd2ndPassword" )		, _T( "Icon_Applet.dds" )		, GETTEXT( TID_2ND_PASSWORD_WINDOW_OPEN )			, 0  									);
	AddAppletFunc( AppMain_WndHelperWebBox 			, APP_WEBBOX2					, _T( "HelperWebBox" )      	, _T( "Icon_HelperHelp.dds" )	, GETTEXT( TID_GAME_HELPER_WEB_BOX_ICON_TOOLTIP )	, 0										);
	AddAppletFunc( AppMain_WndPartyQuick			, APP_PARTY_QUICK				, _T( "WndPartyQuick" )			, _T( "" )						, GETTEXT( TID_TIP_DIALOG )							, 0  									);
	AddAppletFunc( AppMain_WndBuffStatus			, APP_BUFF_STATUS				, _T( "WndBuffStatus" )			, _T( "" )						, GETTEXT( TID_TIP_DIALOG )							, 0  									);
	AddAppletFunc( AppMain_WndVendor				, APP_VENDOR_REVISION			, _T( "WndVendor" )				, _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_VENDOR )							, 0										);
	AddAppletFunc( AppMain_BagEx					, APP_BAG_EX					, _T( "WndBagEx" )				, _T( "Icon_BagBag.tga" )		, GETTEXT( TID_APP_BAG_EX )							, pShortcutKey_UI->KeyPrivateBag		);
	AddAppletFunc( AppMain_InfoPang					, APP_INFOPANG					, _T( "WndInfoPang" )       	, _T( "Icon_Infopang.dds" )		, GETTEXT( TID_TIP_INFOPANG )						, 0										);
	AddAppletFunc( AppMain_Housing					, APP_HOUSING					, _T( "WndHousing" )			, _T( "Icon_Housing.dds" )		, GETTEXT( TID_GAME_HOUSING_BOX )					, pShortcutKey_UI->KeyHousing			);
	AddAppletFunc( AppMain_GuildHousing     		, APP_GH_FURNITURE_STORAGE		, _T( "WndGuildHousing" )   	, _T( "Icon_Housing.dds" )		, GETTEXT( TID_GAME_HOUSING_BOX )					, pShortcutKey_UI->KeyFurnitureStorage	);
	AddAppletFunc( AppMain_WndCharacter				, APP_CHARACTER3				, _T( "WndCharacter" )			, _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER )						, pShortcutKey_UI->KeyCharInfo			);
	AddAppletFunc( AppMain_WndInventory				, APP_INVENTORY					, _T( "WndInventory" )			, _T( "Icon_Inventory.dds" )    , GETTEXT( TID_TIP_INVENTORY )						, pShortcutKey_UI->KeyInventory			);
	AddAppletFunc( AppMain_WndWebBox				, APP_WEBBOX					, _T( "WebBox" )				, _T( "Icon_CitemMall.dds" )    , GETTEXT( TID_TIP_ITEMMALL )						, 0										);
	AddAppletFunc( AppMain_WndMotion				, APP_MOTION					, _T( "WndMotion" )				, _T( "Icon_Motion.dds" )		, GETTEXT( TID_TIP_MOTION )							, pShortcutKey_UI->KeyMotion			);
	AddAppletFunc( AppMain_WndTrade					, APP_TRADE						, _T( "WndTrade" )				, _T( "Icon_Trade.dds" )		, GETTEXT( TID_TIP_TRADE )							, 0										);
	AddAppletFunc( AppMain_LordSkill				, APP_LORD_SKILL				, _T( "WndLordSkill" )      	, _T( "Icon_Infopang.dds" )		, GETTEXT( TID_TIP_INFOPANG )						, pShortcutKey_UI->KeyLordSkill			);
	AddAppletFunc( AppMain_Couple					, APP_COUPLE_MAIN				, _T( "WndCoupleManager" )  	, _T( "Icon_Couple.dds" )		, GETTEXT( TID_GAME_COUPLE )						, pShortcutKey_UI->KeyCouple			);
	AddAppletFunc( AppMain_WndInventoryWear			, APP_INVENTORY_WEAR			, _T( "WndInventoryWear" )  	, _T( "" )						, _T( "" )											, 0										);
	AddAppletFunc( AppMain_WndInventoryItem			, APP_INVENTORY_ITEM			, _T( "WndInventoryItem" )  	, _T( "" )						, _T( "" )											, 0										);

	//if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		AddAppletFunc( AppMain_WndStatus			, APP_STATUS1                 	, _T( "WndStatus" )       		, _T( "Icon_Status.dds" )		, GETTEXT( TID_TIP_STATUS )							, 0								);	

	if( CS_VER1 == _GetContentState( CT_ELLDINS_JAR ) )
		AddAppletFunc( AppMain_WndElldinsJar		, APP_ELLDINPOTION				, _T( "WndElldinsJar" )			, _T( "Icon_Applet.dds" )       , _T( "" )											, pShortcutKey_UI->KeyElldinsJar		);	

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		AddAppletFunc( AppMain_WndTaskBar_HUD		, APP_NEWTASKBAR				, _T( "WndTaskBar_HUD" )		, _T( "" )						, _T( "" )											,0										);

	if( CS_VER1 == _GetContentState( CT_MADRIGAL_GIFT ) )
		AddAppletFunc( AppMain_WndMadirigalGift		, APP_MADRIGALGIFT				, _T( "WndMadrigalGift" )		, _T( "" )						, _T( "" )											,0										);

	if( CS_NOT_SUPPORTED != _GetContentState( CT_FLYFF_PIECE_SYSTEM ) )
		AddAppletFunc( AppMain_WndFlyffPieceSystem	, APP_FLYFFCOUPONTRADE			, _T( "WndFlyffPieceSystem" )	, _T( "" )						, _T( "" )											,0										);

//sun: 9, 스타트메뉴에 전체지도 메뉴 추가
#ifdef __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap					, APP_MAP_EX					, _T( "WndMap" )       			, _T( "Icon_Applet.dds" )		, GETTEXT( TID_TIP_MAP )							, pShortcutKey_UI->KeyWorldMap 			);
#else 
	AddAppletFunc( AppMain_WndMap					, APP_MAP						, _T( "WndMap" )       			, _T( "Icon_Applet.dds" )		, GETTEXT( TID_TIP_MAP )							, pShortcutKey_UI->KeyWorldMap 			);
#endif 


//sun: 9차 전승관련 Clienet
#ifdef __SKILL_UI16
	AddAppletFunc( AppMain_WndSkill2				, APP_SKILL4					, _T( "WndSkill" )    			, _T( "Icon_Skill.dds" )		, GETTEXT( TID_TIP_SKILL )							, pShortcutKey_UI->KeySkill 			);
#else	
	AddAppletFunc( AppMain_WndSkill2				, APP_SKILL3					, _T( "WndSkill" )    			, _T( "Icon_Skill.dds" )		, GETTEXT( TID_TIP_SKILL )							, pShortcutKey_UI->KeySkill 			);
#endif 

#ifdef __GUILDVOTE
	AddAppletFunc( AppMain_WndGuildVote				, APP_GUILD_VOTE				, _T( "WndGuildVote")			, _T( "Icon_Troupe.dds" )		, GETTEXT( TID_TIP_COMPANY )						, 0										);
#endif



	switch(_GetContentState(CT_INFO_NOTICE))
	{
	case CS_VER1:
		AddAppletFunc( AppMain_WndInfoNotice		, APP_INFO_NOTICE				, _T( "WndInfoNotice" )			, _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_INFO_NOTICE )					, 0  									);
		break;
	case CS_VER2:
		AddAppletFunc( AppMain_WndInfoNotice		, APP_NOTICE					, _T( "WndInfoNotice" )			, _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_INFO_NOTICE )					, 0  									);
		break;
	}

}

#pragma endregion AddAllApplet
