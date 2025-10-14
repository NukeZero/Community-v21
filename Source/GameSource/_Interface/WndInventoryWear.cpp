

#include "StdAfx.h"
#include "WndInventoryWear.h"
#include "WndInventoryItem.h"

namespace WndInventoryWear
{
	static const BYTE LWEAPON_OF_TWO_HAND_WEAPON_RENDER_ALPHA(100);	// 양손 무기 일 경우 왼손 무기 그리는 투명도 값
};

using namespace WndInventoryWear;


//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndInventoryWear::CWndInventoryWear()
:	m_eCurrentWearType( eWearType_Normal )
{
	ZeroMemory( m_pButtonWearType,			sizeof( CWndButton* ) * eWearType_Max );
	ZeroMemory( m_pTexWearTypeBackground,	sizeof( CTexture* ) * eWearType_Max );
	ZeroMemory( m_rtWearRect,				sizeof( CRect ) * MAX_HUMAN_PARTS );
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndInventoryWear::~CWndInventoryWear()
{
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryWear::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[윈도우 인벤토리(의상)] 부모 윈도우 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN( dwWndId,		FALSE, _T("[윈도우 인벤토리(의상)] 생성 윈도우 아이디가 유효하지 않습니다.") );

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}


//---------------------------------------------------------------------------------------------
// 윈도우 컨트롤 영역 얻어오기
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
CRect CWndInventoryWear::GetWndCtrlRect( DWORD dwWndId )
{
	CRect rtWndCtrl( 0, 0, 0 ,0 );

	LPWNDCTRL pWndCtrl( GetWndCtrl( dwWndId ) );
	FLERROR_ASSERT_LOG_RETURN( pWndCtrl, rtWndCtrl, _T("[윈도우 인벤토리(의상)] 윈도우 컨트롤 포인터가 유효하지 않습니다. ID - [ %u ]"), dwWndId ); 

	return pWndCtrl->rect;
}


//---------------------------------------------------------------------------------------------
// 배경 텍스쳐를 불러온다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::LoadBackgroundTexture()
{
	if( SEX_MALE == g_pPlayer->GetSex() )
	{
		m_pTexWearTypeBackground[eWearType_Normal]	= m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("APP_INVENTORY_WEAR_F_NORMAL.tga") ), 0, TRUE );
		m_pTexWearTypeBackground[eWearType_Costume] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("APP_INVENTORY_WEAR_F_COSTUME.tga") ), 0, TRUE );
	}
	else if( SEX_FEMALE == g_pPlayer->GetSex() )
	{
		m_pTexWearTypeBackground[eWearType_Normal]	= m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("APP_INVENTORY_WEAR_F_NORMAL.tga") ), 0, TRUE );
		m_pTexWearTypeBackground[eWearType_Costume] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("APP_INVENTORY_WEAR_F_COSTUME.tga") ), 0, TRUE );
	}
	else
	{
		FLERROR_ASSERT_LOG_RETURN_VOID( NULL, _T("[윈도우 인벤토리(의상)] 캐릭터의 성별이 존재하지 않습니다.") );
	}
}


//---------------------------------------------------------------------------------------------
// 현재 의상 타입에 따라 배경 텍스쳐를 설정한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::SetBackgroundTextureFromWearType()
{
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pTexWearTypeBackground[eWearType_Normal],		_T("[윈도우 인벤토리(의상)] 배경 텍스쳐가 없습니다(일반).") );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pTexWearTypeBackground[eWearType_Costume],	_T("[윈도우 인벤토리(의상)] 배경 텍스쳐가 없습니다(코스튬).") );

	if( eWearType_Normal == m_eCurrentWearType )
		m_pTexture = m_pTexWearTypeBackground[eWearType_Normal];
	else if( eWearType_Costume == m_eCurrentWearType )
		m_pTexture = m_pTexWearTypeBackground[eWearType_Costume];
	else
		FLERROR_ASSERT_LOG_RETURN_VOID( NULL, _T("[윈도우 인벤토리(의상)] 존재하지 않는 의상 타입입니다.") );
}


//---------------------------------------------------------------------------------------------
// 현재 의상 타입에 따라 버튼을 설정한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::SetSelectedButtonFromWearType()
{
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pButtonWearType[eWearType_Normal],	_T("[윈도우 인벤토리(의상)] 버튼 포인터가 유효하지 않습니다(일반).") );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pButtonWearType[eWearType_Costume],	_T("[윈도우 인벤토리(의상)] 버튼 포인터가 유효하지 않습니다(코스튬).") );

	m_pButtonWearType[eWearType_Normal]->SetCheck( NULL );	
	m_pButtonWearType[eWearType_Costume]->SetCheck( NULL );	

	if( eWearType_Normal == m_eCurrentWearType )
		m_pButtonWearType[eWearType_Normal]->SetCheck( 2 );
	else if( eWearType_Costume == m_eCurrentWearType )
		m_pButtonWearType[eWearType_Costume]->SetCheck( 2 );
	else
		FLERROR_ASSERT_LOG_RETURN_VOID( NULL, _T("[윈도우 인벤토리(의상)] 존재하지 않는 의상 타입입니다.") );
}


//---------------------------------------------------------------------------------------------
// 인벤토리의 슬롯 사각 영역 정보를 설정한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::SetInventorySlotRectInfo()
{
	CRect rtWeaponSlot						= GetWndCtrlRect( WIDC_WEAPON );
	CRect rtShieldSlot						= GetWndCtrlRect( WIDC_SHIELD );
	CRect rtProjectileSlot					= GetWndCtrlRect( WIDC_PROJECTILE );
	CRect rtCloakSlot						= GetWndCtrlRect( WIDC_CLOAK );
	CRect rtMaskSlot						= GetWndCtrlRect( WIDC_MASK );
	CRect rtHeadSlot						= GetWndCtrlRect( WIDC_HEAD );
	CRect rtChestSlot						= GetWndCtrlRect( WIDC_CHEST );
	CRect rtHandSlot						= GetWndCtrlRect( WIDC_HAND );
	CRect rtFootSlot						= GetWndCtrlRect( WIDC_FOOT );
	CRect rtFlightVehicleSlot				= GetWndCtrlRect( WIDC_FLIGHT_VEHICLE );
	CRect rtRingLeftSlot					= GetWndCtrlRect( WIDC_RING_LEFT );
	CRect rtRingRightSlot					= GetWndCtrlRect( WIDC_RING_RIGHT );
	CRect rtNecklaceSlot					= GetWndCtrlRect( WIDC_NECKLACE );
	CRect rtEarringLeftSlot					= GetWndCtrlRect( WIDC_EARRING_LEFT );
	CRect rtEarringRightSlot				= GetWndCtrlRect( WIDC_EARRING_RIGHT );
	CRect rtCostumeHeadSlot					= GetWndCtrlRect( WIDC_COSTUME_HEAD );
	CRect rtCostumeChestSlot				= GetWndCtrlRect( WIDC_COSTUME_CHEST );
	CRect rtCostumeHandSlot					= GetWndCtrlRect( WIDC_COSTUME_HAND );
	CRect rtCostumeFootSlot					= GetWndCtrlRect( WIDC_COSTUME_FOOT );
	CRect rtCostumeHeadInvSlot				= GetWndCtrlRect( WIDC_COSTUME_HEAD_INV );
	CRect rtCostumeChestInvSlot				= GetWndCtrlRect( WIDC_COSTUME_CHEST_INV );
	CRect rtCostumeHandInvSlot				= GetWndCtrlRect( WIDC_COSTUME_HAND_INV );
	CRect rtCostumeFootInvSlot				= GetWndCtrlRect( WIDC_COSTUME_FOOT_INV );
	
	///--------------------------------------------------------------------------------------
	/// 한 윈도우 컨트롤 영역에 연결되어 있는 인벤토리 파츠가 여러개다...
	/// 정말 그지같다... 이건 기획자도 뭐가 연결되어 있는지 모른다...
	/// 오직 프로그래머만 알 수 있음.. 절 욕하지 마세요... 이미 돌아가기에는 
	/// 너무 많이 와버렸습니다.
	///--------------------------------------------------------------------------------------

	// 좌측 슬롯
	m_rtWearRect[PARTS_RWEAPON]		= rtWeaponSlot;
	m_rtWearRect[PARTS_LWEAPON]		= m_rtWearRect[PARTS_SHIELD] = rtShieldSlot;
	m_rtWearRect[PARTS_BULLET]		= rtProjectileSlot;
	m_rtWearRect[PARTS_CLOAK]		= rtCloakSlot;
	m_rtWearRect[PARTS_MASK]		= rtMaskSlot;

	// 우측 슬롯
	m_rtWearRect[PARTS_CAP]			= m_rtWearRect[PARTS_CAP2]		= rtHeadSlot;
	m_rtWearRect[PARTS_UPPER_BODY]	= m_rtWearRect[PARTS_UPPER2]	= rtChestSlot;
	m_rtWearRect[PARTS_HAND]		= m_rtWearRect[PARTS_HAND2]		= rtHandSlot;
	m_rtWearRect[PARTS_FOOT]		= m_rtWearRect[PARTS_FOOT2]		= rtFootSlot;
	m_rtWearRect[PARTS_RIDE]		= rtFlightVehicleSlot;

	// 상단 슬롯
	m_rtWearRect[PARTS_RING1]		= rtRingLeftSlot;
	m_rtWearRect[PARTS_EARRING1]	= rtEarringLeftSlot;
	m_rtWearRect[PARTS_NECKLACE1]	= rtNecklaceSlot;
	m_rtWearRect[PARTS_EARRING2]	= rtEarringRightSlot;
	m_rtWearRect[PARTS_RING2]		= rtRingRightSlot;

	// 하단 슬롯 ( 코스튬 )
	m_rtWearRect[PARTS_HAT]			= rtCostumeHeadSlot;
	m_rtWearRect[PARTS_CLOTH]		= rtCostumeChestSlot;
	m_rtWearRect[PARTS_GLOVE]		= rtCostumeHandSlot;
	m_rtWearRect[PARTS_BOOTS]		= rtCostumeFootSlot;
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	SetInventorySlotRectInfo();

	LoadBackgroundTexture();
	SetBackgroundTextureFromWearType();

	m_pButtonWearType[eWearType_Normal]		= static_cast<CWndButton*>( GetDlgItem(WIDC_BUTT_NORMAL_WEAR) );
	m_pButtonWearType[eWearType_Costume]	= static_cast<CWndButton*>( GetDlgItem(WIDC_BUTT_COSTUME_WEAR) );

	SetSelectedButtonFromWearType();
}  


//---------------------------------------------------------------------------------------------
// 처리
// param	: void
// return	: 성공 / 실패
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryWear::Process()
{
	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 명령이 왔을 때의 이벤트 처리
// param	: 윈도우 아이디, 메시지, 기본 윈도우
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryWear::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand( nID, dwMessage, pWndBase );
}
 

//---------------------------------------------------------------------------------------------
// 자식으로 부터 메시지 전달
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryWear::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_BUTT_NORMAL_WEAR:	
			m_eCurrentWearType = eWearType_Normal;
			break;

		case WIDC_BUTT_COSTUME_WEAR:	
			m_eCurrentWearType = eWearType_Costume;
			break;

		default:
			break;
	}

	SetBackgroundTextureFromWearType();
	SetSelectedButtonFromWearType();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


//---------------------------------------------------------------------------------------------
// 양손 무기인지 체크하고 양손 무기 일 시 왼손 무기에 오른쪽 무기를 똑같이 설정해준다.
// param	: 파츠 인덱스, 현재 아이템, 렌더링 투명도 값 
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::CheckTwoHandsWeapon( const UINT& nIndex, FLItemBase*& pItemWear, DWORD& dwRenderAlpha )
{
	if( PARTS_LWEAPON != nIndex )
		return ;

	FLItemBase* pRWeapon = g_pPlayer->GetEquipItem( PARTS_RWEAPON );	 
	if( NULL == pRWeapon )
		return ;
		
	if( HD_TWO == pRWeapon->GetProp()->dwHanded )				
	{
		pItemWear		= pRWeapon;									
		dwRenderAlpha   = LWEAPON_OF_TWO_HAND_WEAPON_RENDER_ALPHA;
	}
}


//---------------------------------------------------------------------------------------------
// 아이템 개수 그리기
// param	: 렌더러, 아이템 인덱스, 아이템 포인터
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::DrawItemCount( const C2DRender* p2DRender, const UINT& nIndex, FLItemElem* pItemWearElem )
{
	PT_ITEM_SPEC pItemSpec = pItemWearElem->GetProp();
	if( NULL == pItemWearElem )
		return;

	if( pItemSpec->dwPackMax <= 1 )
		return;

	CString szCount( _T("") );
	szCount.Format( _T("%d"), pItemWearElem->m_nItemNum );

	CSize Size( m_p2DRender->m_pFont->GetTextExtent( szCount ) );

	int nX( m_rtWearRect[nIndex].left ); 	
	int nY( m_rtWearRect[nIndex].top );

	PFONTCOLOR_WNDINVENTORY pFontColorWndInventory = g_WndFontColorManager->GetWndInventory();
	m_p2DRender->TextOut( nX + 30 - Size.cx, nY + 33 - Size.cy, szCount, pFontColorWndInventory->m_stItemCount.GetFontColor(), pFontColorWndInventory->m_stItemCountShadow.GetFontColor() );
}


//---------------------------------------------------------------------------------------------
// 그려야 할 파트 인가?
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryWear::IsDrawParts( UINT nPartsIndex )
{
	if( eWearType_Normal == m_eCurrentWearType )
	{
		if( PARTS_RWEAPON == nPartsIndex	||
			PARTS_LWEAPON == nPartsIndex	||
			PARTS_SHIELD == nPartsIndex		||
			PARTS_BULLET == nPartsIndex		||
			PARTS_CLOAK == nPartsIndex		||
			PARTS_MASK == nPartsIndex		||
			PARTS_CAP == nPartsIndex		||
			PARTS_CAP2 == nPartsIndex		||
			PARTS_UPPER_BODY == nPartsIndex	||
			PARTS_HAND == nPartsIndex		||
			PARTS_HAND2 == nPartsIndex		||
			PARTS_FOOT == nPartsIndex		||
			PARTS_FOOT2 == nPartsIndex		||
			PARTS_RIDE == nPartsIndex		||
			PARTS_RING1 == nPartsIndex		||
			PARTS_EARRING1 == nPartsIndex	||
			PARTS_NECKLACE1 == nPartsIndex	||
			PARTS_EARRING2 == nPartsIndex	||
			PARTS_RING2 == nPartsIndex )
			return true;
	}
	else if( eWearType_Costume == m_eCurrentWearType )
	{
		if( PARTS_HAT == nPartsIndex		||
			PARTS_CLOTH == nPartsIndex		||
			PARTS_GLOVE == nPartsIndex		||
			PARTS_BOOTS == nPartsIndex		)
			return true;	
	}
	else
		return false;

	return false;
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::OnDraw( C2DRender* p2DRender )
{
	if( NULL == p2DRender )
		return ;

	for( UINT nIndex = PARTS_UPPER_BODY; nIndex < MAX_HUMAN_PARTS; nIndex++ )
	{
		if( false == IsDrawParts( nIndex ) )
			continue;

		DWORD dwRenderAlpha( 255 );

		FLItemBase* pItemWear( g_pPlayer->GetEquipItem( nIndex ) );
		CheckTwoHandsWeapon( nIndex, pItemWear, dwRenderAlpha );

		if( NULL == pItemWear )
			continue;
	
		CTexture* pTexItem( pItemWear->GetTexture() );
		if( NULL == pTexItem )
			continue;

		FLItemElem* pItemWearElem( static_cast<FLItemElem*>( pItemWear ) );
		if( NULL == pItemWearElem )
			continue;

		if( pItemWearElem->IsFlag( FLItemElem::expired ) )
			pTexItem->Render2( p2DRender, m_rtWearRect[nIndex].TopLeft(), D3DCOLOR_XRGB( 255, 100, 100 ) );
		else
			pTexItem->Render2( p2DRender, m_rtWearRect[nIndex].TopLeft(), D3DCOLOR_ARGB( dwRenderAlpha, 255, 255, 255 ) );

		DrawItemCount( p2DRender, nIndex, pItemWearElem );
	}

	DrawItemToolTip();
}


//---------------------------------------------------------------------------------------------
// 빈 슬롯에 안내 툴팁 출력하기
// param	: 파츠 인덱스, 위치, 영역
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::DrawEmptySlotToolTip( UINT nPartsIndex, CPoint ptConvert, CRect rtConvert )
{
	CString strText( _T("") );

	switch( nPartsIndex )
	{
		case PARTS_UPPER_BODY:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_SUIT );
			break;
		case PARTS_HAND:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_GAUNTLET );
			break;
		case PARTS_FOOT:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_BOOTS );
			break;
		case PARTS_CAP:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_HELMET );
			break;
		case PARTS_CLOAK:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_CLO );
			break;
		case PARTS_LWEAPON:
		case PARTS_RWEAPON:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_WEAPON );
			break;
		case PARTS_SHIELD:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_GUARD );
			break;
		case PARTS_MASK:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_MASK );
			break;
		case PARTS_RIDE:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_RID );
			break;
		case PARTS_NECKLACE1:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_NECKLACE );
			break;
		case PARTS_RING1:
		case PARTS_RING2:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_RING );
			break;
		case PARTS_EARRING1:
		case PARTS_EARRING2:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_EARRING );
			break;
		case PARTS_BULLET:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_BULLET );
			break;							
		case PARTS_HAT:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_CLOTHES_01 );
			break;	
		case PARTS_CLOTH:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_CLOTHES_02 );
			break;	
		case PARTS_GLOVE:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_CLOTHES_03 );
			break;	
		case PARTS_BOOTS:
			strText = prj.GetText( TID_TOOLTIP_INVENTORY_CLOTHES_04 );
			break;							
	}

	g_toolTip.PutToolTip( 100000, strText, rtConvert, ptConvert );
}


//---------------------------------------------------------------------------------------------
// 아이템 툴팁 그리기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::DrawItemToolTip()
{
	if( m_pWndOnMouseMove != this )
		return ;

	CPoint point( GetMousePoint() );

	for( UINT nIndex = PARTS_UPPER_BODY; nIndex < MAX_HUMAN_PARTS; nIndex++ )
	{
		if( false == IsDrawParts( nIndex ) )
			continue;
		
		if( m_rtWearRect[nIndex].PtInRect( point ) )
		{
			FLItemBase* pItemBase( g_pPlayer->GetEquipItem( nIndex ) );

			if( PARTS_LWEAPON == nIndex )
				pItemBase = g_pPlayer->GetEquipItem( PARTS_SHIELD );

			CPoint	ptConvert( point );
			CRect	rtConvert( m_rtWearRect[nIndex] );

			ClientToScreen( &ptConvert );
			ClientToScreen( &rtConvert );
		
			if( pItemBase )
			{
				g_toolTip.SetSubToolTipNumber( 0 );
				g_WndMng.PutToolTip_Item( pItemBase, ptConvert, &rtConvert, APP_INVENTORY );
				return ;
			}
			else
			{
				DrawEmptySlotToolTip( nIndex, ptConvert, rtConvert );
				return ;
			}
		}
	}
}


//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 더블 클릭 했을 때 ( 아이템 탈착 )
// param	: 플레그, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryWear::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	for( UINT nIndex = PARTS_UPPER_BODY; nIndex < MAX_HUMAN_PARTS; nIndex++ )
	{
		if( false == IsDrawParts( nIndex ) )
			continue;

		if( m_rtWearRect[nIndex].PtInRect( point ) )
		{
			FLItemElem* pItemElem( g_pPlayer->GetEquipItem( nIndex ) );
			if( NULL == pItemElem )
				continue;

			g_DPlay.SendDoEquip( pItemElem, nIndex );	
		}
	}
}


//---------------------------------------------------------------------------------------------
// 교환 창 들이 열려있는가?
// param	: void
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndInventoryWear::IsOpenTradeWindows()
{
	if( g_WndMng.GetWndBase( APP_SHOP_ )	||
		g_WndMng.GetWndBase( APP_BANK )		||
		g_WndMng.GetWndBase( APP_TRADE )	||
		g_WndMng.GetWndBase( APP_SHOP_CART_ ) )	/// 길드 은행도 걸러지는 대상이 아닐까?
	{
		SetForbid( TRUE );
		g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------------------------
// 수리 창 들이 열려있는가?
// param	: void
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndInventoryWear::IsOpenRepairWindows()
{
	if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		SetForbid( TRUE );
		g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------------------------
// 유효한 단축 아이콘 정보인가 ?
// param	: 선택 아이템 정보
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndInventoryWear::IsValidShortCutInfo( LPSHORTCUT pShortcut )
{
	FLERROR_ASSERT_LOG_RETURN( pShortcut, false, _T("[윈도우 인벤토리(의상)] 선택 한 아이템 정보가 유효하지 않습니다.") );

	if( SHORTCUT_ITEM != pShortcut->m_dwShortcut )
		return false;
	
	if( ITYPE_ITEM != pShortcut->m_dwType )
		return false;

	if( NULL == pShortcut->m_dwData )	// dwData가 0이면 돈 
		return false;

	if( NULL == pShortcut->m_pFromWnd )
		return false;

	CWndBase* pWndFromWnd( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFromWnd )
		return false;

	if( APP_INVENTORY_ITEM != pWndFromWnd->GetWndId() )
		return false;

	return true;
}



//---------------------------------------------------------------------------------------------
// 아이콘 올려놨을 때
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
// 설명		: 기획 정책 상 마우스 드레그로 아이템 장착은 지원하지 않도록 변경하였다.
//			  추후 필요 시 아래 주석을 해지하면 사용 가능
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryWear::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	//if( false == IsValidShortCutInfo( pShortcut ) )
	//	return FALSE;

	//if( IsOpenTradeWindows() )
	//	return FALSE;

	//if( IsOpenRepairWindows() )
	//	return FALSE;

	///// 이걸로 인해 발생 할 수 있는 오류가 있는지 생각해보자
	///// 아이템 든 상태에서 인벤토리 닫을 수 있나 확인 해 보자  
	//CWndBase* pWndInventoryItem( static_cast<CWndInventoryItem*>( g_WndMng.GetWndBase( APP_INVENTORY_ITEM ) ) ); 
	//FLERROR_ASSERT_LOG_RETURN( pWndInventoryItem, FALSE, _T("[윈도우 인벤토리(의상)] 아이템 인벤토리(아이템) 창이 열려 있지 않습니다. 장착을 할 수 없습니다") );

	//FLItemElem* pItemElem( (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId ) );
	//FLERROR_ASSERT_LOG_RETURN( pItemElem, FALSE, _T("[윈도우 인벤토리(의상)] 아이템 포인터가 유효하지 않습니다. 장착을 할 수 없습니다") );

	//PT_ITEM_SPEC pItemSpec( pItemElem->GetProp() );
	//FLERROR_ASSERT_LOG_RETURN( pItemElem, FALSE, _T("[윈도우 인벤토리(의상)] 아이템 속성 포인터가 유효하지 않습니다. 장착을 할 수 없습니다") );

	//if( NULL_ID == pItemSpec->dwParts )
	//	return false;
	//
	//if( pItemElem->IsUseBindState() )
	//{
	//	SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
	//	g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DRAG_AND_DROP);
	//	if( g_WndMng.m_pWndEquipBindConfirm )
	//	{
	//		g_WndMng.m_pWndEquipBindConfirm->SetInformation(pItemElem);
	//		g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
	//	}
	//}
	//else
	//	g_DPlay.SendDoEquip( pItemElem );

	return CWndBase::OnDropIcon( pShortcut, point );
}