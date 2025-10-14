

#pragma once

#include "FLColorInfo.h"


//---------------------------------------------------------------------------------------------
// 되도록 구조체 이름은 폰트 칼라를 쓰는 클레스와 동일하게 해주세요 :)
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 윈도우 버튼 컨트롤
//---------------------------------------------------------------------------------------------
typedef struct stFLWndButton
{

	CFLColorInfo	m_stHighLightFill;							// 하이라이트 채움 색상
	CFLColorInfo	m_stHighLightRound;							// 하이라이트 테두리 색상
	CFLColorInfo	m_stMenuTriangleFill;						// 메뉴 삼각형 채움 색상
	CFLColorInfo	m_stTopDownMenuTriangleFill;				// 탑 다운 메뉴 삼각형 채움 색상

	CFLColorInfo	m_stMainMenuCheckIcon;						// 메인 메뉴 체크 아이콘 색상

	stFLWndButton()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stHighLightFill.SetColorInfo( 200, 200, 200, 155 );
			m_stHighLightRound.SetColorInfo( 240, 240, 240, 200 );
			m_stMenuTriangleFill.SetColorInfo( 0, 160, 255 );
			m_stTopDownMenuTriangleFill.SetColorInfo( 240, 240, 240, 200 );

			m_stMainMenuCheckIcon.SetColorInfo( 0, 0, 0 );

		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stHighLightFill.SetColorInfo( 200, 200, 200, 155 );
			m_stHighLightRound.SetColorInfo( 240, 240, 240, 200 );
			m_stMenuTriangleFill.SetColorInfo( 246, 204, 77 );
			m_stTopDownMenuTriangleFill.SetColorInfo( 68, 140, 203 );

			m_stMainMenuCheckIcon.SetColorInfo( 246, 204, 77 );
		}
	}

}FLCOLOR_WNDBUTTON, *PFLCOLOR_WNDBUTTON;


//---------------------------------------------------------------------------------------------
// 윈도우 파티 정보 ( 극단 정보 )
//---------------------------------------------------------------------------------------------
typedef struct stFLWndPartyInfo
{

	CFLColorInfo	m_stStatusBoxOutLine;					// 상태 박스 테두리 색상
	CFLColorInfo	m_stStatusBoxFill_Normal;				// 상태 박스 채움 색상_보통
	CFLColorInfo	m_stStatusBoxFill_Die;					// 상태 박스 채움 색상_죽음
	CFLColorInfo	m_stStatusBoxFill_LowHP;				// 상태 박스 채움 색상_생명력 낮음
	CFLColorInfo	m_stStatusBoxFill_NotFoundFromAround;	// 상태 박스 채움 색상_주위에서 찾을 수 없음
	CFLColorInfo	m_stStatusBoxFill_Disconnect;			// 상태 박스 채움 색상_서버에 없음

	CFLColorInfo	m_stSelectMember;						// 극단 원 선택 색상


	stFLWndPartyInfo()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stStatusBoxOutLine.SetColorInfo( 0, 0, 0 );

			m_stStatusBoxFill_Normal.SetColorInfo( 0, 0, 255 );
			m_stStatusBoxFill_Die.SetColorInfo( 255, 0, 0 );					
			m_stStatusBoxFill_LowHP.SetColorInfo( 255, 255, 0 );				
			m_stStatusBoxFill_NotFoundFromAround.SetColorInfo( 135, 135, 135 );	
			m_stStatusBoxFill_Disconnect.SetColorInfo( 0, 0, 0 );	

			m_stSelectMember.SetColorInfo( 255, 255, 0, 96 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stStatusBoxOutLine.SetColorInfo( 255, 255, 255 );

			m_stStatusBoxFill_Normal.SetColorInfo( 0, 138, 255 );				
			m_stStatusBoxFill_Die.SetColorInfo( 115, 0, 0 );					
			m_stStatusBoxFill_LowHP.SetColorInfo( 192, 104, 1 );				
			m_stStatusBoxFill_NotFoundFromAround.SetColorInfo( 135, 135, 135 );	
			m_stStatusBoxFill_Disconnect.SetColorInfo( 15, 39, 39 );			

			m_stSelectMember.SetColorInfo( 255, 245, 140, 96 );
		}
	}

}FLCOLOR_WNDPARTYINFO, *PFLCOLOR_WNDPARTYINFO;


//---------------------------------------------------------------------------------------------
// 윈도우 길드 하우징
//---------------------------------------------------------------------------------------------
typedef struct stFLWndGuildHousing
{
	CFLColorInfo	m_stSelectBoxFill;						// 선택박스 채움색상

	stFLWndGuildHousing()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelectBoxFill.SetColorInfo( 255, 0, 0, 85 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stSelectBoxFill.SetColorInfo( 255, 0, 0, 85 );
		}
	}

}FLCOLOR_WNDGUILDHOUSING, *PFLCOLOR_WNDGUILDHOUSING;



//---------------------------------------------------------------------------------------------
// 윈도우 길드 하우징에서 가구 설치 중 
//---------------------------------------------------------------------------------------------
typedef struct stFLWndGHousingShowOneUnit
{
	CFLColorInfo	m_stBoxFill;							// 박스 채움색상

	stFLWndGHousingShowOneUnit()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stBoxFill.SetColorInfo( 255, 0, 0, 85 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stBoxFill.SetColorInfo( 255, 0, 0, 85 );
		}
	}

}FLCOLOR_WNDGHOUSINGSHOWONEUNIT, *PFLCOLOR_WNDGHOUSINGSHOWONEUNIT;


//---------------------------------------------------------------------------------------------
// 윈도우 길드 대전 개인 순위
//---------------------------------------------------------------------------------------------
typedef struct stFLWndGuildCombatRank_Class
{
	CFLColorInfo	m_stLineUp;							// 선 위에 줄 채움색상
	CFLColorInfo	m_stLineDown;						// 선 아래 줄 채움색상

	CFLColorInfo	m_stSelectBoxFill;					// 선택박스 채움색상

	stFLWndGuildCombatRank_Class()
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stLineUp.SetColorInfo( 0, 0, 0, 128 );
			m_stLineDown.SetColorInfo( 0, 0, 0, 128  );

			m_stSelectBoxFill.SetColorInfo( 0, 0, 0, 64 );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_stLineUp.SetColorInfo( 246, 204, 77, 255 );
			m_stLineDown.SetColorInfo( 246, 204, 77, 255 );

			m_stSelectBoxFill.SetColorInfo( 255, 245, 140, 96 );
		}
	}

}FLCOLOR_WNDGUILDCOMBATRANK_CLASS, *PFLCOLOR_WNDGUILDCOMBATRANK_CLASS;


//---------------------------------------------------------------------------------------------
// 마드리갈의 선물
//---------------------------------------------------------------------------------------------
typedef struct stFLWndMadrigalGift
{
	CFLColorInfo	m_stRoundGuage_StepNone;			// 원 게이지 단계 없을 때 채움 색상
	CFLColorInfo	m_stRoundGuage_Step1;				// 원 게이지 1단계 채움색상
	CFLColorInfo	m_stRoundGuage_Step2;				// 원 게이지 2단계 채움색상
	CFLColorInfo	m_stRoundGuage_Step3;				// 원 게이지 3단계 채움색상

	stFLWndMadrigalGift()
	{
		m_stRoundGuage_StepNone.SetColorInfo( 0, 255, 0 );
		m_stRoundGuage_Step1.SetColorInfo( 255, 255, 0 );
		m_stRoundGuage_Step2.SetColorInfo( 255, 0, 0 );
		m_stRoundGuage_Step3.SetColorInfo( 255, 0, 0 );
	}

}FLCOLOR_WNDMADRIGALGIFT, *PFLCOLOR_WNDMADRIGALGIFT;


//---------------------------------------------------------------------------------------------
// 프리프 조각 시스템
//---------------------------------------------------------------------------------------------
typedef struct stFLWndFlyffPieceSystem
{
	CFLColorInfo	m_stListBoxLineColor;				// 리스트 박스 선 색상
	CFLColorInfo	m_stListBoxSelectRect;				// 리스트 선택 테두리 색상
	CFLColorInfo	m_stListBoxSelectFillRect;			// 리스트 선택 채움 색상


	stFLWndFlyffPieceSystem()
	{
		m_stListBoxLineColor.SetColorInfo( 119, 102, 85 );
		m_stListBoxSelectRect.SetColorInfo( 0x7711ff11 );
		m_stListBoxSelectFillRect.SetColorInfo( 0x2211ff11 );
	}

}FLCOLOR_WNDFLYFFPIECESYSTEM, *PFLCOLOR_WNDFLYFFPIECESYSTEM;


//---------------------------------------------------------------------------------------------
// 범용 아이템 교환 윈도우
//---------------------------------------------------------------------------------------------
typedef struct stFLWndCommonItemExchange
{
	CFLColorInfo	m_stListBoxLineColor;				// 리스트 박스 선 색상
	CFLColorInfo	m_stListBoxSelectRect;				// 리스트 선택 테두리 색상
	CFLColorInfo	m_stListBoxSelectFillRect;			// 리스트 선택 채움 색상

	stFLWndCommonItemExchange()
	{
		m_stListBoxLineColor.SetColorInfo( 119, 102, 85 );
		m_stListBoxSelectRect.SetColorInfo( 0x7711ff11 );
		m_stListBoxSelectFillRect.SetColorInfo( 0x2211ff11 );
	}

}FLCOLOR_WNDCOMMONITEMEXCHANGE, *PFLCOLOR_WNDCOMMONITEMEXCHANGE;
