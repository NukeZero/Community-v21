
#pragma once

#include "WndFontColorInfo.h"

//---------------------------------------------------------------------------------------------
// 되도록 구조체 이름은 폰트 칼라를 쓰는 클레스와 동일하게 해주세요 :)
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 윈도우 베이스
//---------------------------------------------------------------------------------------------
typedef struct stWndBase
{
	CWndFontColorInfo	m_stDefault;					// 기본 폰트색상

	stWndBase()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 255, 255, 255 );
		}
	}

}FONTCOLOR_WNDBASE, *PFONTCOLOR_WNDBASE;


//---------------------------------------------------------------------------------------------
// 윈도우 트리 컨트롤
//---------------------------------------------------------------------------------------------
typedef struct stWndTreeCtrl
{

	CWndFontColorInfo	m_stDefault;					// 기본 폰트색상
	CWndFontColorInfo	m_stSelect;						// 선택 된 트리 아이템 폰트색상

	stWndTreeCtrl()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 64, 64, 64 );
			m_stSelect.SetFontColorInfo( 0, 0 ,255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 255, 255, 255 );
			m_stSelect.SetFontColorInfo( 150, 221, 59 );
		}
	}

}FONTCOLOR_WNDTREECTRL, *PFONTCOLOR_WNDTREECTRL;


//---------------------------------------------------------------------------------------------
// 윈도우 탭 컨트롤
//---------------------------------------------------------------------------------------------
typedef struct stWndTabCtrl
{

	CWndFontColorInfo	m_stDefault;					// 기본 폰트색상
	CWndFontColorInfo	m_stSelectTab;					// 선택 된 탭 폰트색상

	stWndTabCtrl()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 255, 255, 255 );
			m_stSelectTab.SetFontColorInfo( 0, 0 ,0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 150, 137, 112 );
			m_stSelectTab.SetFontColorInfo( 255, 255 ,255 );
		}
	}

}FONTCOLOR_WNDTABCTRL, *PFONTCOLOR_WNDTABCTRL;


//---------------------------------------------------------------------------------------------
// 윈도우 버튼 컨트롤
//---------------------------------------------------------------------------------------------
typedef struct stWndButton
{

	CWndFontColorInfo	m_stMainMenuDesc;					// 메인 메뉴 설명 폰트색상
	CWndFontColorInfo	m_stMainMenuShortcut;				// 메인 메뉴 단축키 폰트색상

	CWndFontColorInfo	m_stPush;							// 버튼 눌렀을 때 폰트색상
	CWndFontColorInfo	m_stHighLight;						// 버튼 하이라이트 일 때 폰트색상
	CWndFontColorInfo	m_stDisable;						// 버튼 비 활성화 일 때 폰트색상
	CWndFontColorInfo	m_stShadow;							// 버튼 글씨 그림자 폰트 색상

	stWndButton()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stMainMenuDesc.SetFontColorInfo( 0, 0, 0 );
			m_stMainMenuShortcut.SetFontColorInfo( 0, 0 ,0 );

			m_stPush.SetFontColorInfo( 255, 255, 100 );
			m_stHighLight.SetFontColorInfo( 64, 64, 255 );
			m_stDisable.SetFontColorInfo( 140, 140, 140 );
			m_stShadow.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stMainMenuDesc.SetFontColorInfo( 255, 255, 255 );
			m_stMainMenuShortcut.SetFontColorInfo( 255, 255 ,255 );

			m_stPush.SetFontColorInfo( 0, 0, 0 );
			m_stHighLight.SetFontColorInfo( 116, 107, 85 );
			m_stDisable.SetFontColorInfo( 23, 23, 23 );
			m_stShadow.SetFontColorInfo( 255, 244, 204 );
		}
	}

}FONTCOLOR_WNDBUTTON, *PFONTCOLOR_WNDBUTTON;


//---------------------------------------------------------------------------------------------
// 윈도우 텍스트
//---------------------------------------------------------------------------------------------
typedef struct stWndText
{

	CWndFontColorInfo	m_stDefault;					// 기본 폰트색상

	stWndText()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 255, 255, 255 );
		}
	}

}FONTCOLOR_WNDTEXT, *PFONTCOLOR_WNDTEXT;


//---------------------------------------------------------------------------------------------
// 윈도우 리스트 박스
//---------------------------------------------------------------------------------------------
typedef struct stWndListBox
{

	CWndFontColorInfo	m_stSelectColor;			// 선택 폰트색상
	CWndFontColorInfo	m_stMouseOverColor;			// 마우스 오버 폰트색상
	CWndFontColorInfo	m_stInvalidColor;			// 유효하지 않은 폰트색상

	stWndListBox()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelectColor.SetFontColorInfo( 64, 64, 255 );
			m_stMouseOverColor.SetFontColorInfo( 255, 128, 0 );
			m_stInvalidColor.SetFontColorInfo( 170, 170, 170 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelectColor.SetFontColorInfo( 64, 64, 255 );
			m_stMouseOverColor.SetFontColorInfo( 255, 128, 0 );
			m_stInvalidColor.SetFontColorInfo( 170, 170, 170 );
		}
	}

}FONTCOLOR_WNDLISTBOX, *PFONTCOLOR_WNDLISTBOX;


//---------------------------------------------------------------------------------------------
// 윈도우 다이얼로그
//---------------------------------------------------------------------------------------------
typedef struct stWndDialog
{
	CWndFontColorInfo	m_stGroupBoxTitle;				// 그룹박스 타이틀 폰트 색상

	CWndFontColorInfo	m_stKeyButtonDetault;			// 키 버튼 기본 폰트 색상
	CWndFontColorInfo	m_stKeyButtonSelect;			// 키 버튼 선택 폰트 색상
	CWndFontColorInfo	m_stKeyButtonRollOver;			// 키 버튼 마우스 롤 오버 폰트 색상

	stWndDialog()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stGroupBoxTitle.SetFontColorInfo( 128, 0, 64 );
			
			m_stKeyButtonDetault.SetFontColorInfo( 80, 80, 80 );
			m_stKeyButtonSelect.SetFontColorInfo( 128, 0, 255 );
			m_stKeyButtonRollOver.SetFontColorInfo( 16, 16, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stGroupBoxTitle.SetFontColorInfo( 246, 204, 77 );

			m_stKeyButtonDetault.SetFontColorInfo( 255, 255, 255 );
			m_stKeyButtonSelect.SetFontColorInfo( 29, 252, 255 );
			m_stKeyButtonRollOver.SetFontColorInfo( 246, 204, 77 );
		}
	}

}FONTCOLOR_WNDDIALOG, *PFONTCOLOR_WNDDIALOG;


//---------------------------------------------------------------------------------------------
// 편집 문자열
//---------------------------------------------------------------------------------------------
typedef struct stEditSring
{

	CWndFontColorInfo	m_stDefault;					// 기본 폰트색상

	stEditSring()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDefault.SetFontColorInfo( 255, 255, 255 );
		}
	}

}FONTCOLOR_EDITSTRING, *PFONTCOLOR_EDITSTRING;


//---------------------------------------------------------------------------------------------
// 서버 선택
//---------------------------------------------------------------------------------------------
typedef struct stWndSelectServer
{

	CWndFontColorInfo	m_stServerListSelect;			// 서버 리스트 선택
	CWndFontColorInfo	m_stServerListMouseOver;		// 서버 리스트 마우스 오버
 	CWndFontColorInfo	m_stServerListInvalid;			// 서버 리스트 선택 불가
	CWndFontColorInfo	m_stMultiListSelect;			// 채널 리스트 선택
	CWndFontColorInfo	m_stMultiListMouseOver;			// 채널 리스트 마우스 오버
	CWndFontColorInfo	m_stMultiListInvalid;			// 채널 리스트 선택 불가

	stWndSelectServer()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stServerListSelect.SetFontColorInfo( 0, 0, 255 );
			m_stServerListMouseOver.SetFontColorInfo( 255, 128, 0 );
			m_stServerListInvalid.SetFontColorInfo( 170, 170, 170 );
			m_stMultiListSelect.SetFontColorInfo( 0, 0, 255 );	
			m_stMultiListMouseOver.SetFontColorInfo( 255, 128, 0 );	
			m_stMultiListInvalid.SetFontColorInfo( 170, 170, 170 );

		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stServerListSelect.SetFontColorInfo( 39, 196, 203 );
			m_stServerListMouseOver.SetFontColorInfo( 253, 95, 255 );
			m_stServerListInvalid.SetFontColorInfo( 170, 170, 170 );
			m_stMultiListSelect.SetFontColorInfo( 39, 196, 203 );	
			m_stMultiListMouseOver.SetFontColorInfo( 253, 95, 255 );	
			m_stMultiListInvalid.SetFontColorInfo( 170, 170, 170 );
		}
	}

}FONTCOLOR_WNDSELECTSERVER, *PFONTCOLOR_WNDSELECTSERVER;


//---------------------------------------------------------------------------------------------
// 퀘스트
//---------------------------------------------------------------------------------------------
typedef struct stWndQuestDetail
{

	CWndFontColorInfo	m_stQuestCondition;				// 퀘스트 완료조건	
	CWndFontColorInfo	m_stQuestConditionSelect;		// 퀘스트 완료조건 선택
	CWndFontColorInfo	m_stQuestConditionComplete;		// 퀘스트 완료조건 완료

	stWndQuestDetail()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stQuestCondition.SetFontColorInfo( 0, 0, 0 );
			m_stQuestConditionSelect.SetFontColorInfo( 0, 0, 255 );
			m_stQuestConditionComplete.SetFontColorInfo( 64, 64, 64 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stQuestCondition.SetFontColorInfo( 255, 255, 255 );
			m_stQuestConditionSelect.SetFontColorInfo( 39, 196, 203 );
			m_stQuestConditionComplete.SetFontColorInfo( 255, 255, 0 );
		}
	}

}FONTCOLOR_WNDQUESTDETAIL, *PFONTCOLOR_WNDQUESTDETAIL;


//---------------------------------------------------------------------------------------------
// 작업 바
//---------------------------------------------------------------------------------------------
typedef struct stWndTaskBar
{
	CWndFontColorInfo	m_stToolTip_ShortCutApplet;		// 작업 바에 윈도우
	CWndFontColorInfo	m_stToolTip_ShortCutHotKey;		// 작업 바에 단축키
	CWndFontColorInfo	m_stToolTip_ShortCutMotion;		// 작업 바에 행동
	
	stWndTaskBar()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stToolTip_ShortCutApplet.SetFontColorInfo( 0, 0, 0 );
			m_stToolTip_ShortCutHotKey.SetFontColorInfo( 0, 0, 255 );
			m_stToolTip_ShortCutMotion.SetFontColorInfo( 0, 0, 0 );

		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stToolTip_ShortCutApplet.SetFontColorInfo( 255, 255, 255 );
			m_stToolTip_ShortCutHotKey.SetFontColorInfo( 255, 255, 0 );
			m_stToolTip_ShortCutMotion.SetFontColorInfo( 255, 255, 255 );
		}
	}

}FONTCOLOR_WNDTASKBAR, *PFONTCOLOR_WNDTASKBAR;


//---------------------------------------------------------------------------------------------
// 툴팁
//---------------------------------------------------------------------------------------------
typedef struct stToolTip
{
	CWndFontColorInfo	m_stToolTip;					// 일반적인 툴팁
	CWndFontColorInfo	m_stPutEquipItemText;			// [ 착용 중인 아이템 ] 이란 툴팁 폰트 색상

	stToolTip()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stToolTip.SetFontColorInfo( 0, 0, 0 );
			m_stPutEquipItemText.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stToolTip.SetFontColorInfo( 255, 255, 255 );
			m_stPutEquipItemText.SetFontColorInfo( 255, 255, 255 );
		}
	}

}FONTCOLOR_WNDTOOLTIP, *PFONTCOLOR_WNDTOOLTIP;


//---------------------------------------------------------------------------------------------
// 윈도우 캐릭터 정보 
//---------------------------------------------------------------------------------------------
typedef struct stWndCharInfo
{
	CWndFontColorInfo	m_stPropertyNameColor;					// 캐릭터 정보 속성 이름 폰트색상
	CWndFontColorInfo	m_stPropertyNumberColor;				// 캐릭터 정보 속성 숫자 폰트색상
	CWndFontColorInfo	m_stIncreasePropertyNumberColor;		// 캐릭터 정보 속성 숫자 증가 폰트색상

	CWndFontColorInfo	m_stIncreaseAbilityNumberColor;			// 캐릭터 능력치 숫자 증가 폰트색상
	CWndFontColorInfo	m_stDecreaseAbilityNumberColor;			// 캐릭터 능력치 숫자 감소 폰트색상

	stWndCharInfo()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stPropertyNameColor.SetFontColorInfo( 0, 0, 180 );
			m_stPropertyNumberColor.SetFontColorInfo( 0, 0, 0 );
			m_stIncreasePropertyNumberColor.SetFontColorInfo( 255, 0, 0 );
			m_stIncreaseAbilityNumberColor.SetFontColorInfo( 0, 0, 255 );
			m_stDecreaseAbilityNumberColor.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stPropertyNameColor.SetFontColorInfo( 246, 204, 77 );
			m_stPropertyNumberColor.SetFontColorInfo( 255, 255, 255 );
			m_stIncreasePropertyNumberColor.SetFontColorInfo( 121, 80, 255 );
			m_stIncreaseAbilityNumberColor.SetFontColorInfo( 74, 123, 197 );
			m_stDecreaseAbilityNumberColor.SetFontColorInfo( 255, 0, 0 );
		}
	}

}FONTCOLOR_WNDCHARINFO, *PFONTCOLOR_WNDCHARINFO;



//---------------------------------------------------------------------------------------------
// 펫 상태 창
//---------------------------------------------------------------------------------------------
typedef struct stWndPetStatus
{
	CWndFontColorInfo	m_stPropertyNameColor;				// 펫 정보 속성 이름 폰트색상
	CWndFontColorInfo	m_stPropertyNameShadowColor;		// 펫 정보 속성 이름 그림자 폰트색상
	CWndFontColorInfo	m_stPropertyValueColor;				// 펫 정보 속성 숫자 폰트색상
	CWndFontColorInfo	m_stGaugeValueColor;				// 펫 정보 게이지 숫자 폰트색상

	stWndPetStatus()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stPropertyNameColor.SetFontColorInfo( 74, 74, 173 );
			m_stPropertyNameShadowColor.SetFontColorInfo( 160, 160, 240 );
			m_stPropertyValueColor.SetFontColorInfo( 255, 28, 174 );
			m_stGaugeValueColor.SetFontColorInfo( 255, 255, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stPropertyNameColor.SetFontColorInfo( 246, 204, 77 );
			m_stPropertyNameShadowColor.SetFontColorInfo( 30, 30, 30 );
			m_stPropertyValueColor.SetFontColorInfo( 145, 203, 255 );
			m_stGaugeValueColor.SetFontColorInfo( 255, 255, 255 );
		}
	}

}FONTCOLOR_WNDPETSTATUS, *PFONTCOLOR_WNDPETSTATUS;


//---------------------------------------------------------------------------------------------
// 파티 정보 창
//---------------------------------------------------------------------------------------------
typedef struct stWndPartyInfo
{
	CWndFontColorInfo	m_stLeader;						// 파티 리더 정보 폰트색상

	stWndPartyInfo()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stLeader.SetFontColorInfo( 31, 183, 45 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stLeader.SetFontColorInfo( 255, 95, 95 );
		}
	}

}FONTCOLOR_WNDPARTYINFO, *PFONTCOLOR_WNDPARTYINFO;


//---------------------------------------------------------------------------------------------
// 파티 스킬 창
//---------------------------------------------------------------------------------------------
typedef struct stWndPartySkill
{
	CWndFontColorInfo	m_stlessPoint;						// 스킬을 배웠으나 포인트 부족 폰트색상
	CWndFontColorInfo	m_stNoLearnSkill;					// 스킬을 안 배웠음 폰트색상

	stWndPartySkill()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stlessPoint.SetFontColorInfo( 0, 0, 255 );
			m_stNoLearnSkill.SetFontColorInfo( 255, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stlessPoint.SetFontColorInfo( 0, 0, 255 );
			m_stNoLearnSkill.SetFontColorInfo( 255, 0, 0 );
		}
	}

}FONTCOLOR_WNDPARTYSKILL, *PFONTCOLOR_WNDPARTYSKILL;


//---------------------------------------------------------------------------------------------
// 메신져
//---------------------------------------------------------------------------------------------
typedef struct stWndMessengerEx
{
	CWndFontColorInfo	m_stName;					// 이름 폰트색상
	CWndFontColorInfo	m_stNameShadow;				// 이름 그림자 폰트색상
	CWndFontColorInfo	m_stState;					// 상태 폰트색상
	CWndFontColorInfo	m_stCampusPoint;			// 캠퍼스 포인트 폰트색상
	CWndFontColorInfo	m_stServer;					// 서버 폰트색상
	CWndFontColorInfo	m_stChannel;				// 채널 폰트색상

	stWndMessengerEx()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stName.SetFontColorInfo( 96, 96, 96 );
			m_stNameShadow.SetFontColorInfo( 96, 96, 96 );
			m_stState.SetFontColorInfo( 96, 96, 96 );
			m_stCampusPoint.SetFontColorInfo( 96, 96, 96 );
			m_stServer.SetFontColorInfo( 96, 96, 96 );
			m_stChannel.SetFontColorInfo( 96, 96, 96 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stName.SetFontColorInfo( 255, 255, 255 );
			m_stNameShadow.SetFontColorInfo( 96, 96, 96 );
			m_stState.SetFontColorInfo( 54, 224, 82 );
			m_stCampusPoint.SetFontColorInfo( 246, 204, 77 );
			m_stServer.SetFontColorInfo( 246, 204, 77 );
			m_stChannel.SetFontColorInfo( 246, 204, 77 );
		}
	}

}FONTCOLOR_WNDMESSENGEREX, *PFONTCOLOR_WNDMESSENGEREX;


//---------------------------------------------------------------------------------------------
// 친구 창 ( In 메신져 )
//---------------------------------------------------------------------------------------------
typedef struct stWndFriendCtrlEx
{
	CWndFontColorInfo	m_stVisitAllow;					// 미니룸 입장 허가
	CWndFontColorInfo	m_stSelect;						// 선택 폰트색상
	CWndFontColorInfo	m_stBlock;						// 차단 폰트색상

	stWndFriendCtrlEx()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stVisitAllow.SetFontColorInfo( 0, 255, 0 );
			m_stSelect.SetFontColorInfo( 96, 96, 255 );
			m_stBlock.SetFontColorInfo( 255, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stVisitAllow.SetFontColorInfo( 0, 255, 0 );
			m_stSelect.SetFontColorInfo( 39, 196, 203 );
			m_stBlock.SetFontColorInfo( 255, 0, 0 );
		}
	}

}FONTCOLOR_WNDFRIENDCTRLEX, *PFONTCOLOR_WNDFRIENDCTRLEX;



//---------------------------------------------------------------------------------------------
// 길드 창 ( In 메신져 )
//---------------------------------------------------------------------------------------------
typedef struct stWndGuildCtrlEx
{
	CWndFontColorInfo	m_stSelect;						// 선택 폰트색상

	stWndGuildCtrlEx()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 96, 96, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 39, 196, 203 );
		}
	}

}FONTCOLOR_WNDGUILDCTRLEX, *PFONTCOLOR_WNDGUILDCTRLEX;


//---------------------------------------------------------------------------------------------
// 사제 창 ( In 메신져 )
//---------------------------------------------------------------------------------------------
typedef struct stWndCampus
{
	CWndFontColorInfo	m_stSelect;						// 선택 폰트색상

	stWndCampus()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 96, 96, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 39, 196, 203 );
		}
	}

}FONTCOLOR_WNDCAMPUS, *PFONTCOLOR_WNDCAMPUS;


//---------------------------------------------------------------------------------------------
// 윈도우 하우징
//---------------------------------------------------------------------------------------------
typedef struct stWndHousing
{
	CWndFontColorInfo	m_stSelect;						// 선택 폰트색상

	stWndHousing()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 39, 196, 203 );
		}
	}

}FONTCOLOR_WNDHOUSING, *PFONTCOLOR_WNDHOUSING;


//---------------------------------------------------------------------------------------------
// 윈도우 길드 하우징
//---------------------------------------------------------------------------------------------
typedef struct stWndGuildHousing
{
	CWndFontColorInfo	m_stDeploy;						// 입장허가 폰트색상

	stWndGuildHousing()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDeploy.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stDeploy.SetFontColorInfo( 237, 20, 91 );
		}
	}

}FONTCOLOR_WNDGUILDHOUSING, *PFONTCOLOR_WNDGUILDHOUSING;


//---------------------------------------------------------------------------------------------
// 윈도우 길드 공헌창
//---------------------------------------------------------------------------------------------
typedef struct stWndGuildMetricCtrl
{
	CWndFontColorInfo	m_stItemCount;				// 각각의 개수 폰트 색상
	CWndFontColorInfo	m_stItemCountShadow;		// 각각의 개수의 그림자 폰트 색상

	stWndGuildMetricCtrl()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 16, 16, 255 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDGUILDMETRICCTRL, *PFONTCOLOR_WNDGUILDMETRICCTRL;


//---------------------------------------------------------------------------------------------
// 윈도우 길드 대전 개인 순위
//---------------------------------------------------------------------------------------------
typedef struct stWndGuildCombatRank_Class
{
	CWndFontColorInfo	m_st1stPlayer;			// 1등 유져 폰트 색상
	CWndFontColorInfo	m_stSelfPlayer;			// 플레어어 자신 폰트 색상

	stWndGuildCombatRank_Class()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_st1stPlayer.SetFontColorInfo( 200, 0, 0 );
			m_stSelfPlayer.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_st1stPlayer.SetFontColorInfo( 200, 0, 0 );
			m_stSelfPlayer.SetFontColorInfo( 97, 97, 255 );
		}
	}

}FONTCOLOR_WNDGUILDCOMBATRANK_CLASS, *PFONTCOLOR_WNDGUILDCOMBATRANK_CLASS;


//---------------------------------------------------------------------------------------------
// 윈도우 상점 
//---------------------------------------------------------------------------------------------
typedef struct stWndShop_Cart
{
	CWndFontColorInfo	m_stlessUserMoney;						// 돈이 적을 때의 구입 예상 가격 폰트색상
	CWndFontColorInfo	m_stEnoughUserMoney;					// 돈이 충분 할 때의 구입 예상 가격 폰트색상

	CWndFontColorInfo	m_stShoppingBagItemCount;				// 쇼핑 백에 담겨져 있는 아이템 각각의 개수 폰트 색상
	CWndFontColorInfo	m_stShoppingBagItemCountShadow;			// 쇼핑 백에 담겨져 있는 아이템 각각의 개수의 그림자 폰트 색상

	CWndFontColorInfo	m_stTotalShoppingBagItemCount;			// 쇼핑 백에 담겨져 있는 아이템의 총 개수 폰트 색상

	stWndShop_Cart()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stlessUserMoney.SetFontColorInfo( 255, 0, 0 );
			m_stEnoughUserMoney.SetFontColorInfo( 46, 112, 169 );

			m_stShoppingBagItemCount.SetFontColorInfo( 0, 204, 0 );
			m_stShoppingBagItemCountShadow.SetFontColorInfo( 0, 0, 0 );

			m_stTotalShoppingBagItemCount.SetFontColorInfo( 255, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stlessUserMoney.SetFontColorInfo( 242, 7, 29 );
			m_stEnoughUserMoney.SetFontColorInfo( 150, 221, 59 );

			m_stShoppingBagItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stShoppingBagItemCountShadow.SetFontColorInfo( 0, 0, 0 );

			m_stTotalShoppingBagItemCount.SetFontColorInfo( 246, 204, 77 );
		}
	}

}FONTCOLOR_WNDSHOP_CART, *PFONTCOLOR_WNDSHOP_CART;



//---------------------------------------------------------------------------------------------
// 윈도우 엘딘의 항아리
//---------------------------------------------------------------------------------------------
typedef struct stWndElldinsJar
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상

	stWndElldinsJar()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 176, 176, 240 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDELLDINSJAR, *PFONTCOLOR_WNDELLDINSJAR;


//---------------------------------------------------------------------------------------------
// 윈도우 인벤토리 ( 인벤토리 위에 있는 장비들... 네이밍 내가 안했음 )
//---------------------------------------------------------------------------------------------
typedef struct stWndInventory
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상

	stWndInventory()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 176, 176, 240 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDINVENTORY, *PFONTCOLOR_WNDINVENTORY;


//---------------------------------------------------------------------------------------------
// 윈도우 아이템 컨트롤 ( 인벤토리 밑에 아이템들... 네이밍 내가 안했음 )
//---------------------------------------------------------------------------------------------
typedef struct stWndItemCtrl
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상

	stWndItemCtrl()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 176, 176, 240 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDITEMCTRL, *PFONTCOLOR_WNDITEMCTRL;


//---------------------------------------------------------------------------------------------
// 윈도우 편지 보내기
//---------------------------------------------------------------------------------------------
typedef struct stWndPostSend
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상

	stWndPostSend()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 176, 176, 240 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDPOSTSEND, *PFONTCOLOR_WNDPOSTSEND;


//---------------------------------------------------------------------------------------------
// 윈도우 편지 받기
//---------------------------------------------------------------------------------------------
typedef struct stWndPostReceive
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상

	stWndPostReceive()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 176, 176, 240 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDPOSTRECEIVE, *PFONTCOLOR_WNDPOSTRECEIVE;


//---------------------------------------------------------------------------------------------
// 윈도우 편지 읽기
//---------------------------------------------------------------------------------------------
typedef struct stWndPostRead
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상

	stWndPostRead()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 176, 176, 240 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemCount.SetFontColorInfo( 136, 255, 133 );
			m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDPOSTREAD, *PFONTCOLOR_WNDPOSTREAD;


//---------------------------------------------------------------------------------------------
// 윈도우 길드 멤버 탭
//---------------------------------------------------------------------------------------------
typedef struct stWndGuildTabMember
{
	CWndFontColorInfo	m_stSelect;								// 선택 폰트색상
	CWndFontColorInfo	m_stOffLine;							// 오프라인 폰트색상

	stWndGuildTabMember()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 0, 0, 255 );
			m_stOffLine.SetFontColorInfo( 144, 144, 144 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelect.SetFontColorInfo( 39, 196, 203 );
			m_stOffLine.SetFontColorInfo( 82, 82, 82 );
		}
	}

}FONTCOLOR_WNDGUILDTABMEMBER, *PFONTCOLOR_WNDGUILDTABMEMBER;



//---------------------------------------------------------------------------------------------
// 버프
//---------------------------------------------------------------------------------------------
typedef struct stWndBuffStatus
{
	CWndFontColorInfo	m_stBuffSkillTime;						// 스킬 버프 지속 시간 폰트색상
	CWndFontColorInfo	m_stBuffSkillTimeShadow;				// 스킬 버프 지속 시간 그림자 폰트색상

	stWndBuffStatus()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stBuffSkillTime.SetFontColorInfo( 255, 255, 255 );
			m_stBuffSkillTimeShadow.SetFontColorInfo( 0, 0, 0 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stBuffSkillTime.SetFontColorInfo( 118, 255, 177 );
			m_stBuffSkillTimeShadow.SetFontColorInfo( 0, 0, 0 );
		}
	}

}FONTCOLOR_WNDBUFFSTATUS, *PFONTCOLOR_WNDBUFFSTATUS;


//---------------------------------------------------------------------------------------------
// 버프
//---------------------------------------------------------------------------------------------
typedef struct stWndMgr
{
	CWndFontColorInfo	m_stBuffSkillToolTipAbility;						// 스킬 버프 툴팁에 능력치 옵션 폰트색상

	stWndMgr()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stBuffSkillToolTipAbility.SetFontColorInfo( 0, 0, 255 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stBuffSkillToolTipAbility.SetFontColorInfo( 97, 97, 255 );
		}
	}

}FONTCOLOR_WNDMGR, *PFONTCOLOR_WNDMGR;


//---------------------------------------------------------------------------------------------
// 마드리갈의 선물
//---------------------------------------------------------------------------------------------
typedef struct stWndMadrigalGift
{
	CWndFontColorInfo	m_stToolTip;							// 툴팁 폰트색상

	stWndMadrigalGift()
	{
		m_stToolTip.SetFontColorInfo( 75, 145, 220 );					
	}

}FONTCOLOR_WNDMADRIGALGIFT, *PFONTCOLOR_WNDMADRIGALGIFT;


//---------------------------------------------------------------------------------------------
// 프리프 조각 시스템
//---------------------------------------------------------------------------------------------
typedef struct stWndFlyffPieceSystem
{
	CWndFontColorInfo	m_stGuildText;							// 안내 문구 폰트색상
	CWndFontColorInfo	m_stLoadingText;						// 로딩 문구 폰트색상
	CWndFontColorInfo	m_stItemListEmptyText;					// 아이템 리스트가 비어있다는 문구 폰트색상


	stWndFlyffPieceSystem()
	{
		m_stGuildText.SetFontColorInfo( 255, 255, 255 );
		m_stLoadingText.SetFontColorInfo( 255, 255, 255 );
		m_stItemListEmptyText.SetFontColorInfo( 255, 255, 255 );
	}

}FONTCOLOR_WNDFLYFFPIECESYSTEM, *PFONTCOLOR_WNDFLYFFPIECESYSTEM;


//---------------------------------------------------------------------------------------------
// 윈도우 월드
//---------------------------------------------------------------------------------------------
typedef struct stWndWorld
{
	CWndFontColorInfo	m_stExpBuffIconToolTipItemName;			// 경험치 버프 아이콘 툴팁 아이템 이름 색상

	stWndWorld()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stExpBuffIconToolTipItemName.SetFontColorInfo( 0, 0, 255 );
		}
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stExpBuffIconToolTipItemName.SetFontColorInfo( 47, 190, 109 );
		}
	}

}FONTCOLOR_WNDWORLD, *PFONTCOLOR_WNDWORLD;


//---------------------------------------------------------------------------------------------
// 윈도우 아이템 지속일자 컨트롤
//---------------------------------------------------------------------------------------------
typedef struct stWndCommItemCtrl
{
	CWndFontColorInfo	m_stItemName;						// 아이템 이름 색상
	CWndFontColorInfo	m_stToolTipItemName;				// 툴팁 아이템 이름 색상
	
	stWndCommItemCtrl()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemName.SetFontColorInfo( 0, 0, 255 );
			m_stToolTipItemName.SetFontColorInfo( 0, 0, 255 );
		}

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stItemName.SetFontColorInfo( 47, 190, 109 );
			m_stToolTipItemName.SetFontColorInfo( 47, 190, 109 );
		}
	}

}FONTCOLOR_WNDCOMMITEMCTRL, *PFONTCOLOR_WNDCOMMITEMCTRL;


//---------------------------------------------------------------------------------------------
// 윈도우 인스턴스 메시지
//---------------------------------------------------------------------------------------------
typedef struct stWndInstantMsg
{
	CWndFontColorInfo	m_stMessage;						// 메시지 색상
	
	stWndInstantMsg()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stMessage.SetFontColorInfo( 0, 0, 255 );
		}

		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stMessage.SetFontColorInfo( 246, 204, 77 );
		}
	}

}FONTCOLOR_WNDINSTANTMSG, *PFONTCOLOR_WNDINSTANTMSG;


//---------------------------------------------------------------------------------------------
// 범용 아이템 교환 창
//---------------------------------------------------------------------------------------------
typedef struct stWndCommonItemExchange
{
	CWndFontColorInfo	m_stItemCount;							// 아이템 개수 폰트색상
	CWndFontColorInfo	m_stItemCountShadow;					// 아이템 개수 그림자 폰트색상
	CWndFontColorInfo	m_stNotEnoughItemCount;					// 아이템 개수 부족할 때의 폰트색상

	stWndCommonItemExchange()
	{
		m_stItemCount.SetFontColorInfo( 136, 255, 133 );
		m_stItemCountShadow.SetFontColorInfo( 0, 0, 0 );
		m_stNotEnoughItemCount.SetFontColorInfo( 255, 0, 0 );
	}

}FONTCOLOR_WNDCOMMONITEMEXCHANGE, *PFONTCOLOR_WNDCOMMONITEMEXCHANGE;