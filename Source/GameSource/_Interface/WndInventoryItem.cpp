

#include "StdAfx.h"
#include "WndInventoryItem.h"
#include "WndVendor.h"

/// 맵이동전에 열려있는 인벤토리 윈도우 다 닫고 시작하자 응 알갔지? 아마 코딩되어 있을거 같긴 하다만...

namespace WndInventoryItem
{
	static const BYTE	MAXITEMCOUNT_OF_ONE_PAGE( 42 );
	static const BYTE	HORIZON_ITEMCOUNT( 6 );
	static const BYTE	VERTICAL_ITEMCOUNT( MAXITEMCOUNT_OF_ONE_PAGE / HORIZON_ITEMCOUNT );	
};

using namespace WndInventoryItem;


//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndInventoryItem::CWndInventoryItem()
:	m_eCurrentItemType( eItemType_Normal )
,	m_eCurrentItemSlot( eItemSlot_1 )
,	m_pButtonItemArrange( NULL )
,	m_pWndConfirmBuy( NULL )
{
	ZeroMemory( m_pButtonItemType,	sizeof( CWndButton* ) * eItemType_Max );
	ZeroMemory( m_pButtonItemSlot,	sizeof( CWndButton* ) * eItemSlot_Max );

	m_vecItemInfo.reserve( MAXITEMCOUNT_OF_ONE_PAGE );
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndInventoryItem::~CWndInventoryItem()
{
	SAFE_DELETE( m_pWndConfirmBuy );
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryItem::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[윈도우 인벤토리(아이템)] 부모 윈도우 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN( dwWndId,		FALSE, _T("[윈도우 인벤토리(아이템)] 생성 윈도우 아이디가 유효하지 않습니다.") );

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}


//---------------------------------------------------------------------------------------------
// 윈도우 컨트롤 영역 얻어오기
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
CRect CWndInventoryItem::GetWndCtrlRect( DWORD dwWndId )
{
	CRect rtWndCtrl( 0, 0, 0 ,0 );

	LPWNDCTRL pWndCtrl( GetWndCtrl( dwWndId ) );
	FLERROR_ASSERT_LOG_RETURN( pWndCtrl, rtWndCtrl, _T("[윈도우 인벤토리(아이템)] 윈도우 컨트롤 포인터가 유효하지 않습니다. ID - [ %u ]"), dwWndId ); 

	return pWndCtrl->rect;
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pButtonItemType[eItemType_Normal]		= static_cast<CWndButton*>( GetDlgItem(WIDC_INV_ITEM_ICON) );
	m_pButtonItemType[eItemType_Pet]		= static_cast<CWndButton*>( GetDlgItem(WIDC_INV_PET_ICON) );
	m_pButtonItemType[eItemType_Costume]	= static_cast<CWndButton*>( GetDlgItem(WIDC_INV_COSTUME_ICON) );
	m_pButtonItemType[eItemType_Quest]		= static_cast<CWndButton*>( GetDlgItem(WIDC_INV_QUEST_ICON) );

	m_pButtonItemSlot[eItemSlot_1]			= static_cast<CWndButton*>( GetDlgItem(WIDC_BUTT_INV_SLOT1) );
	m_pButtonItemSlot[eItemSlot_2]			= static_cast<CWndButton*>( GetDlgItem(WIDC_BUTT_INV_SLOT2) );
	m_pButtonItemSlot[eItemSlot_3]			= static_cast<CWndButton*>( GetDlgItem(WIDC_BUTT_INV_SLOT3) );
	m_pButtonItemSlot[eItemSlot_4]			= static_cast<CWndButton*>( GetDlgItem(WIDC_BUTT_INV_SLOT4) );

	m_pButtonItemArrange					= static_cast<CWndButton*>( GetDlgItem(WIDC_INV_SORT_ICON) );

	SetSelectedButtonFromItemType();
	SetSelectedButtonFromItemSlot();

	CalculationItemRect();
}  


//---------------------------------------------------------------------------------------------
// 아이템 사각형 영역을 계산한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::CalculationItemRect()
{
	CRect	rtItemTotalSpace( GetWndCtrlRect( WIDC_ITEM_TOTAL_SPACE ) );  
	CRect	rtItemDrawSpace( GetWndCtrlRect( WIDC_ITEM_DRAW_SPACE ) );  
	CRect	rtItemOrgDrawSpace( rtItemDrawSpace );  

	WORD	wOffsetVertical( rtItemTotalSpace.Width() - rtItemDrawSpace.Width() );
	WORD	wOffsetHorizon( rtItemTotalSpace.Height() - rtItemDrawSpace.Height() );

	WORD	wCount( NULL );

	for( BYTE byVerticalCount = 0; byVerticalCount < VERTICAL_ITEMCOUNT; byVerticalCount++ )
	{
		rtItemDrawSpace = rtItemOrgDrawSpace;
		rtItemDrawSpace.OffsetRect( 0, (rtItemDrawSpace.Height() + wOffsetVertical) * byVerticalCount );

		for( BYTE byHorizonCount = 0; byHorizonCount < HORIZON_ITEMCOUNT; byHorizonCount++ )
		{
			ITEMINFO stItemInfo;
			stItemInfo.rtDrawItemSpace = rtItemDrawSpace;

			m_vecItemInfo.push_back( stItemInfo );
			
			rtItemDrawSpace.OffsetRect( rtItemDrawSpace.Width() + wOffsetHorizon, 0 );
			wCount++;
		}
	}

	if( m_vecItemInfo.size() > MAXITEMCOUNT_OF_ONE_PAGE )
		FLERROR_ASSERT_LOG_RETURN_VOID( NULL, _T("[윈도우 인벤토리(아이템)] 한 페이지에서 보여줄 수 있는 아이템 개수 보다 생성 된 정보가 더 많습니다.") );
}


//---------------------------------------------------------------------------------------------
// 현재 아이템 타입에 따라 버튼을 설정한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::SetSelectedButtonFromItemType()
{
	for( BYTE byCount = eItemType_Normal; byCount < eItemType_Max; byCount++ )
	{
		FLERROR_ASSERT_LOG_DO( m_pButtonItemType[byCount], continue, _T("[윈도우 인벤토리(아이템)] 아이템 타입 버튼 포인터가 유효하지 않습니다 : [ %d ]"), byCount );
		m_pButtonItemType[byCount]->SetCheck( NULL );

		if( byCount == m_eCurrentItemType )
			m_pButtonItemType[byCount]->SetCheck( 2 );
	}
}



//---------------------------------------------------------------------------------------------
// 현재 아이템 슬롯 번호에 따라 버튼을 설정한다.
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::SetSelectedButtonFromItemSlot()
{
	for( BYTE byCount = eItemSlot_1; byCount < eItemSlot_Max; byCount++ )
	{
		FLERROR_ASSERT_LOG_DO( m_pButtonItemSlot[byCount], continue, _T("[윈도우 인벤토리(아이템)] 슬롯 버튼 포인터가 유효하지 않습니다 : [ %d ]"), byCount );
		m_pButtonItemSlot[byCount]->SetCheck( NULL );

		if( byCount == m_eCurrentItemSlot )
			m_pButtonItemSlot[byCount]->SetCheck( 2 );
	}
}


//---------------------------------------------------------------------------------------------
// 아이템 리스트를 갱신한다 ( 일반 )
// param	: void
// return	: 성공 / 실패
// 설명		: 아이템은 현재 일차원 배열에 생성되어있다... 젠장.. 그래서 맞는 배열의 인덱스 접근하여
//			  아이템을 가져와서 넣어야 한다.
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::UpdateItemList_Normal()
{
	if( eItemType_Normal != m_eCurrentItemType )
		return false;

	WORD wStartCount( MAX_HUMAN_PARTS );
	WORD wEndCount( MAX_INVENINDEX_GENERAL );

	WORD nCurrentCount( NULL );

	for( WORD wCount = wStartCount; wCount < wEndCount; wCount++ )
	{
		FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( wCount ) );
		if( pItemElem )
			m_vecItemInfo[nCurrentCount].pItemElem = pItemElem;

		m_vecItemInfo[nCurrentCount].dwOrgArrayIndex = wCount;
		nCurrentCount++;
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 리스트를 갱신한다 ( 펫 )
// param	: void
// return	: 성공 / 실패
// 설명		: 아이템은 현재 일차원 배열에 생성되어있다... 젠장.. 그래서 맞는 배열의 인덱스 접근하여
//			  아이템을 가져와서 넣어야 한다.
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::UpdateItemList_Pet()
{
	if( eItemType_Pet != m_eCurrentItemType )
		return false;

	WORD wStartCount( MAX_INVENINDEX_GENERAL );
	WORD wEndCount( MAX_INVENINDEX_PET );

	WORD nCurrentCount( NULL );

	for( WORD wCount = wStartCount; wCount < wEndCount; wCount++ )
	{
		FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( wCount ) );
		if( pItemElem )
			m_vecItemInfo[nCurrentCount].pItemElem = pItemElem;

		m_vecItemInfo[nCurrentCount].dwOrgArrayIndex = wCount;
		nCurrentCount++;
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 리스트를 갱신한다 ( 코스튬 )
// param	: void
// return	: 성공 / 실패
// 설명		: 아이템은 현재 일차원 배열에 생성되어있다... 젠장.. 그래서 맞는 배열의 인덱스 접근하여
//			  아이템을 가져와서 넣어야 한다.
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::UpdateItemList_Costume()
{
	if( eItemType_Costume != m_eCurrentItemType )
		return false;

	WORD wStartCount( MAX_INVENINDEX_PET );
	WORD wEndCount( MAX_INVENINDEX_COSTUME );

	WORD nCurrentCount( NULL );

	for( WORD wCount = wStartCount; wCount < wEndCount; wCount++ )
	{
		FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( wCount ) );
		if( pItemElem )
			m_vecItemInfo[nCurrentCount].pItemElem = pItemElem;

		m_vecItemInfo[nCurrentCount].dwOrgArrayIndex = wCount;
		nCurrentCount++;
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 리스트를 갱신한다 ( 퀘스트 )
// param	: void
// return	: 성공 / 실패
// 설명		: 아이템은 현재 일차원 배열에 생성되어있다... 젠장.. 그래서 맞는 배열의 인덱스 접근하여
//			  아이템을 가져와서 넣어야 한다.
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::UpdateItemList_Quest()
{
	if( eItemType_Quest != m_eCurrentItemType )
		return false;

	WORD wStartCount( MAX_INVENINDEX_COSTUME );
	WORD wEndCount( MAX_INVENINDEX_QUEST );

	WORD nCurrentCount( NULL );

	for( WORD wCount = wStartCount; wCount < wEndCount; wCount++ )
	{
		FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( wCount ) );
		if( pItemElem )
			m_vecItemInfo[nCurrentCount].pItemElem = pItemElem;

		m_vecItemInfo[nCurrentCount].dwOrgArrayIndex = wCount;
		nCurrentCount++;
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 리스트를 초기화 한다.
// param	: void
// return	: 성공 / 실패
// 설명		: 아이템 영역은 바뀌지 않으므로 아이템 포인터만 초기화 시킨다.
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::ClearItemList()
{
	for( UINT nCount = 0; nCount < m_vecItemInfo.size(); nCount++ )
		m_vecItemInfo[nCount].pItemElem = NULL;
}


//---------------------------------------------------------------------------------------------
// 처리
// param	: void
// return	: 성공 / 실패
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryItem::Process()
{
	if( NULL == g_pPlayer )
		return FALSE;

	ClearItemList();
	UpdateItemList_Normal();
	UpdateItemList_Pet();
	UpdateItemList_Costume();
	UpdateItemList_Quest();

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 명령이 왔을 때의 이벤트 처리
// param	: 윈도우 아이디, 메시지, 기본 윈도우
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand( nID, dwMessage, pWndBase );
}
 

//---------------------------------------------------------------------------------------------
// 자식으로 부터 메시지 전달
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_INV_ITEM_ICON:	
			m_eCurrentItemType = eItemType_Normal;
			break;

		case WIDC_INV_PET_ICON:	
			m_eCurrentItemType = eItemType_Pet;
			break;

		case WIDC_INV_COSTUME_ICON:	
			m_eCurrentItemType = eItemType_Costume;
			break;

		case WIDC_INV_QUEST_ICON:	
			m_eCurrentItemType = eItemType_Quest;
			break;

		case WIDC_BUTT_INV_SLOT1:	
			m_eCurrentItemSlot = eItemSlot_1;
			break;

		case WIDC_BUTT_INV_SLOT2:	
			m_eCurrentItemSlot = eItemSlot_2;
			break;

		case WIDC_BUTT_INV_SLOT3:	
			m_eCurrentItemSlot = eItemSlot_3;
			break;

		case WIDC_BUTT_INV_SLOT4:	
			m_eCurrentItemSlot = eItemSlot_4;
			break;

		default:
			break;
	}

	SetSelectedButtonFromItemType();
	SetSelectedButtonFromItemSlot();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


//---------------------------------------------------------------------------------------------
// 아이템 아이콘 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::DrawItemIcon( C2DRender* const p2DRender )
{
	CPoint	ptMouse( GetMousePoint() );

	for( UINT nCount = 0; nCount < m_vecItemInfo.size(); nCount++ )
	{
		ITEMINFO	stItemInfo( m_vecItemInfo[nCount] );
		bool		bChangeTextureSize( false );
		
		FLItemElem* pItemElem( stItemInfo.pItemElem );
		if( NULL == pItemElem )
			continue;

		PT_ITEM_SPEC pItemSpec( pItemElem->GetProp() );
		if( NULL == pItemSpec )
			continue;

		CTexture* pTexture = m_textureMng.AddTexture( p2DRender->m_pd3dDevice, MakePath( DIR_ITEM, pItemSpec->szIcon ), WNDCOLOR_DEFAULT_KEY );
		if( NULL == pTexture )
			continue;

		if( stItemInfo.rtDrawItemSpace.PtInRect( ptMouse ) )
		{
			pTexture->m_size.cx += 4;
			pTexture->m_size.cy += 4;
			bChangeTextureSize = true;
		}
		
		CPoint	ptTexture( 0, 0 );
		CRect	rtOrgItemDrawSpace( GetWndCtrlRect( WIDC_ITEM_DRAW_SPACE ) );  

		long nOffsetX = static_cast<long>( ( rtOrgItemDrawSpace.Width() - pTexture->m_size.cx ) * 0.5f );
		long nOffsetY = static_cast<long>( ( rtOrgItemDrawSpace.Width() - pTexture->m_size.cx ) * 0.5f );

		ptTexture.x = stItemInfo.rtDrawItemSpace.left + nOffsetX;
		ptTexture.y = stItemInfo.rtDrawItemSpace.top + nOffsetY;

		if( pItemElem->IsFlag( FLItemElem::expired ) )
			pTexture->Render2( p2DRender, ptTexture, D3DCOLOR_ARGB( 255, 255, 100, 100 ) );
		else
			pTexture->Render( p2DRender, ptTexture );

		if( bChangeTextureSize )
		{
			pTexture->m_size.cx -= 4;
			pTexture->m_size.cy -= 4;
		}
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::OnDraw( C2DRender* p2DRender )
{
	if( NULL == p2DRender )
		return ;

	DrawItemIcon( p2DRender );
}


//---------------------------------------------------------------------------------------------
// 마우스가 윈도우 위에 올라갔을 때 
// param	: 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::OnMouseWndSurface(  CPoint point  )
{
	if( m_vecItemInfo.empty() )
		return;

	for( UINT nCount = 0; nCount < m_vecItemInfo.size(); nCount++ )
	{
		ITEMINFO stItemInfo( m_vecItemInfo[nCount] );
		
		if( stItemInfo.rtDrawItemSpace.PtInRect( point ) )
		{
			FLItemBase* pItemBase( stItemInfo.pItemElem );
			if( NULL == pItemBase )
				return ;

			CPoint	ptConvert( point );
			CRect	rtConvert( stItemInfo.rtDrawItemSpace );

			ClientToScreen( &ptConvert );
			ClientToScreen( &rtConvert );
		
			g_toolTip.SetSubToolTipNumber( 0 );
			g_WndMng.PutToolTip_Item( pItemBase, ptConvert, &rtConvert, APP_INVENTORY );

			return;
		}
	}
}

//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 눌렀을 때
// param	: 플레그, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::OnLButtonDown( UINT nFlags, CPoint point )
{
}


//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 더블 클릭 했을 때 ( 아이템 장착 )
// param	: 플레그, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::OnLButtonDblClk( UINT nFlags, CPoint point )
{
}


//---------------------------------------------------------------------------------------------
// 아이템을 집는다.
// param	: 플레그, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::PickItem( const CPoint point )
{
	if( false == m_bLButtonDown )
		return ;

	if( FALSE == m_GlobalShortcut.IsEmpty() )
		return ;

	for( UINT nCount = 0; nCount < m_vecItemInfo.size(); nCount++ )
	{
		ITEMINFO stItemInfo( m_vecItemInfo[nCount] );
		
		if( stItemInfo.rtDrawItemSpace.PtInRect( point ) )
		{
			FLItemElem* pItemElem( stItemInfo.pItemElem ); 
			
			if( NULL == pItemElem )
				return ;
			
			if( FALSE == IsUsableItem( pItemElem ) )
				return ;
			
			///& m_useDieFlag 이거 뭐꼬? 아이템 락과 관련되어 있다는데 아이템을 사용할 수 있는 상황을 체크 하는거 같다.
			/// !pWndInventory->m_bRemoveJewel 이건 또 뭐꼬....
			/// if( ((pItemElem != NULL && m_useDieFlag) || IsUsableItem( pItemElem )) && !pWndInventory->m_bRemoveJewel) ( 원래코드 )

			m_GlobalShortcut.Empty();
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId       = pItemElem->m_dwObjId;
			m_GlobalShortcut.m_dwIndex    = stItemInfo.dwOrgArrayIndex;
			m_GlobalShortcut.m_pTexture   = pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD)pItemElem;
				
			FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pItemElem->GetName() );
		}
	}
}



//---------------------------------------------------------------------------------------------
// 마우스 움직였을 때
// param	: 플레그, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventoryItem::OnMouseMove( UINT nFlags, CPoint point )
{
	PickItem( point );
}


//---------------------------------------------------------------------------------------------
// 유효한 단축 아이콘 정보인가 ?
// param	: 선택 아이템 정보
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::IsValidShortCutInfo( LPSHORTCUT pShortcut )
{
	FLERROR_ASSERT_LOG_RETURN( pShortcut, false, _T("[윈도우 인벤토리(아이템)] 선택 한 아이템 정보가 유효하지 않습니다.") );

	if( SHORTCUT_ITEM != pShortcut->m_dwShortcut )
		return false;

	if( ITYPE_ITEM != pShortcut->m_dwType )	/// 카드 , 펫 , 큐브 일 수도 있지 않냐?
		return false;

	if( NULL == pShortcut->m_pFromWnd )
		return false;

	CWndBase* pWndFromWnd( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFromWnd )
		return false;

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이콘 올려놨을 때
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
BOOL CWndInventoryItem::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	/// 거래중 또는 은행 거래 중 이럴때 막아야 하나 생각해 보자

	if( false == IsValidShortCutInfo( pShortcut ) )
		FLERROR_ASSERT_LOG_RETURN( NULL, FALSE, _T("[윈도우 인벤토리(아이템)] 선택 한 아이템 정보가 유효하지 않습니다.") );

	ProcessItemMove_FromInventoryItem( pShortcut, point );	
	ProcessItemMove_FromPrivateBank( pShortcut, point );
	ProcessItemMove_FromGuildBank( pShortcut, point );
	ProcessItemMove_FromMobileBag( pShortcut, point );
	ProcessItemMove_FromVendor( pShortcut, point );
	ProcessItemMove_FromPost( pShortcut, point );

	return CWndBase::OnDropIcon( pShortcut, point );
}



//---------------------------------------------------------------------------------------------
// 아이템 이동 처리 ( 아이템 인벤토리로 부터 왔을 때 [자기 자신])
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::ProcessItemMove_FromInventoryItem( const SHORTCUT* const pShortcut, const CPoint point )
{
	CWndBase* pWndFrame( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFrame )
		return false;

	if( APP_INVENTORY_ITEM != pWndFrame->GetWndId() )
		return false;

	/// 돈인지 아닌지 판단해서 처리해야 하지 않을까?

	for( UINT nCount = 0; nCount < m_vecItemInfo.size(); nCount++ )
	{
		ITEMINFO stItemInfo( m_vecItemInfo[nCount] );
		
		if( stItemInfo.rtDrawItemSpace.PtInRect( point ) )
		{
			FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( pShortcut->m_dwIndex ) );
			
			if( NULL == pItemElem )
				return false;
			
			if( FALSE == IsUsableItem( pItemElem ) )
				return false;

			if( IsUsingItem( pItemElem ) )
				return false;
			
			g_DPlay.SendMoveItem( pShortcut->m_dwIndex, stItemInfo.dwOrgArrayIndex );
			return true;
		}
	}

	return false;
}


//---------------------------------------------------------------------------------------------
// 아이템 이동 처리 ( 개인 은행으로 부터 왔을 때 )
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::ProcessItemMove_FromPrivateBank( const SHORTCUT* const pShortcut, const CPoint point )
{
	CWndBase* pWndFrame( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFrame )
		return false;

	if( APP_COMMON_BANK != pWndFrame->GetWndId() )
		return false;

	FLERROR_ASSERT_LOG_RETURN( g_WndMng.m_pWndBank, false, _T("[윈도우 인벤토리(아이템)] 은행 윈도우가 없습니다. 아이템 이동을 무시합니다.") )

	BYTE byBankItemSlotIndex( g_WndMng.m_pWndBank->GetCurrentItemSlotIndex() );
	BYTE byBankMoneySlotIndex( g_WndMng.m_pWndBank->GetCurrentMoneySlotIndex() );

	DWORD dwItemElem( pShortcut->m_dwData );
	FLItemElem* pItemElem = (FLItemElem*)(dwItemElem );
	if( pItemElem )	// 아이템 
	{
		if( pItemElem->m_nItemNum > 1 )
		{
			CWndCommonInputCount* pWndCommonInputCount( g_WndMng.GetWndCommonInputCount( this ) );
			FLERROR_ASSERT_LOG_RETURN( pWndCommonInputCount, false, _T("[윈도우 인벤토리(아이템)] 범용 개수 입력 윈도우 포인터가 유효하지 않습니다.") );
				
			pWndCommonInputCount->SetInputCountInfo( pShortcut, byBankItemSlotIndex );
		}
		else
			g_DPlay.SendGetItemBank( byBankItemSlotIndex, static_cast<BYTE>( pShortcut->m_dwId ), pItemElem->m_nItemNum );

		return true;
	
	}
	else // 돈
	{
		CWndCommonInputCount* pWndCommonInputCount( g_WndMng.GetWndCommonInputCount( this ) );
		FLERROR_ASSERT_LOG_RETURN( pWndCommonInputCount, false, _T("[윈도우 인벤토리(아이템)] 범용 개수 입력 윈도우 포인터가 유효하지 않습니다.") );
			
		pWndCommonInputCount->SetInputCountInfo( pShortcut, byBankMoneySlotIndex );
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------------------------
// 아이템 이동 처리 ( 길드 은행으로 부터 왔을 때 )
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::ProcessItemMove_FromGuildBank( const SHORTCUT* const pShortcut, const CPoint point )
{
	CWndBase* pWndFrame( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFrame )
		return false;

	if( APP_GUILD_BANK != pWndFrame->GetWndId() )
		return false;

	FLERROR_ASSERT_LOG_RETURN( g_WndMng.m_pWndGuildBank, false, _T("[윈도우 인벤토리(아이템)] 길드은행 윈도우가 없습니다. 아이템 이동을 무시합니다.") )

	CGuild* pGuild( g_pPlayer->GetGuild() );
	FLERROR_ASSERT_LOG_RETURN( pGuild, false, _T("[윈도우 인벤토리(아이템)] 캐릭터가 길드에 속해있지 않습니다.") )

	if( FALSE == pGuild->IsGetItem( g_pPlayer->m_idPlayer ) )
		return false;

	DWORD dwItemElem( pShortcut->m_dwData );
	FLItemElem* pItemElem = (FLItemElem*)(dwItemElem );
	if( pItemElem )	// 아이템 
	{
		if( pItemElem->m_nItemNum > 1 )
		{
			CWndCommonInputCount* pWndCommonInputCount( g_WndMng.GetWndCommonInputCount( this ) );
			FLERROR_ASSERT_LOG_RETURN( pWndCommonInputCount, false, _T("[윈도우 인벤토리(아이템)] 범용 개수 입력 윈도우 포인터가 유효하지 않습니다.") );
				
			pWndCommonInputCount->SetInputCountInfo( pShortcut );
		}
		else
			g_DPlay.SendGetItemGuildBank( static_cast<BYTE>( pShortcut->m_dwId ), 1, 1 );

		return true;
	
	}
	else // 돈
	{
		CWndCommonInputCount* pWndCommonInputCount( g_WndMng.GetWndCommonInputCount( this ) );
		FLERROR_ASSERT_LOG_RETURN( pWndCommonInputCount, false, _T("[윈도우 인벤토리(아이템)] 범용 개수 입력 윈도우 포인터가 유효하지 않습니다.") );
			
		pWndCommonInputCount->SetInputCountInfo( pShortcut );
		return true;
	}

	return false;
}


//---------------------------------------------------------------------------------------------
// 아이템 이동 처리 ( 우편으로 부터 왔을 때 )
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::ProcessItemMove_FromPost( const SHORTCUT* const pShortcut, const CPoint point )
{
	CWndBase* pWndFrame( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFrame )
		return false;

	if( APP_POST_READ != pWndFrame->GetWndId() )
		return false;

	CWndPostRead* pWndPostRead = static_cast<CWndPostRead*>( pWndFrame );
	FLERROR_ASSERT_LOG_RETURN( pWndPostRead, false, _T("[윈도우 인벤토리(아이템)] 받은 편지 윈도우가 없습니다. 아이템 이동을 무시합니다.") )

	if( pShortcut->m_dwData != 0 )
		pWndPostRead->MailReceiveItem();
	else
		pWndPostRead->MailReceiveGold();
					
	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 이동 처리 ( 휴대가방으로 부터 왔을 때 )
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::ProcessItemMove_FromMobileBag( const SHORTCUT* const pShortcut, const CPoint point )
{
	CWndBase* pWndFrame( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFrame )
		return false;

	if( APP_BAG_EX != pWndFrame->GetWndId() )
		return false;

	if( pShortcut->m_dwData == 0 )
		return false;

	DWORD dwItemElem( pShortcut->m_dwData );
	FLItemElem* pItemElem = (FLItemElem*)(dwItemElem );
	FLERROR_ASSERT_LOG_RETURN( pItemElem, false, _T("[윈도우 인벤토리(아이템)] 단축 아이템 정보 아이템 포인터가 유효하지 않습니다.") );

	BYTE		bySlot( NULL );
	CWndBase*	pWndSlot( pWndFrame->m_pCurFocus );
	
	if( pWndSlot->m_pParentWnd->GetWndId() == WIDC_BASIC )
	{
		bySlot = 0;
	}
	else if( pWndSlot->m_pParentWnd->GetWndId() == WIDC_EXBAG1 )
	{
		if( FALSE == g_pPlayer->m_Pocket.IsAvailable(1) )
			return false;
		else		
			bySlot = 1;
	}
	else
	{
		if( FALSE == g_pPlayer->m_Pocket.IsAvailable(2) )
			return false;
		else		
			bySlot = 2;
	}

	if( pItemElem->m_nItemNum > 1 )
	{
		CWndCommonInputCount* pWndCommonInputCount( g_WndMng.GetWndCommonInputCount( this ) );
		FLERROR_ASSERT_LOG_RETURN( pWndCommonInputCount, false, _T("[윈도우 인벤토리(아이템)] 범용 개수 입력 윈도우 포인터가 유효하지 않습니다.") );
			
		pWndCommonInputCount->SetInputCountInfo( pShortcut, bySlot );
	}
	else
		g_DPlay.SendMoveItem_Pocket( bySlot, pShortcut->m_dwId, 1 , -1 );

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 이동 처리 ( 개인상점으로 부터 왔을 때 )
// param	: 선택 아이템 정보, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndInventoryItem::ProcessItemMove_FromVendor( const SHORTCUT* const pShortcut, const CPoint point )
{
	CWndBase* pWndFrame( pShortcut->m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFrame )
		return false;

	if( APP_VENDOR_REVISION != pWndFrame->GetWndId() )
		return false;

	if( pShortcut->m_dwData == 0 )
		return false;

	CWndVendor* pWndVendor( static_cast<CWndVendor*>( pWndFrame ) ); 
	FLERROR_ASSERT_LOG_RETURN( pWndVendor, false, _T("[윈도우 인벤토리(아이템)] 개인상점 윈도우 포인터가 유효하지 않습니다.") );

	if( pWndVendor->m_pVendor == g_pPlayer )
		g_DPlay.SendUnregisterPVendorItem( static_cast<BYTE>( pShortcut->m_dwIndex ) );
	else
	{
		SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
		pWndVendor->m_pWndVendorBuy	= new CWndVendorBuy( (FLItemBase*)pShortcut->m_dwData, pShortcut->m_dwIndex );
		FLERROR_ASSERT_LOG_RETURN( pWndVendor, false, _T("[윈도우 인벤토리(아이템)] 개인상점 구입 창 생성 실패.") );
		pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );
	}

	return true;
}
