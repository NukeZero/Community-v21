

#include "StdAfx.h"
#include "WndCommonInputCount.h"

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndCommonInputCount::CWndCommonInputCount()
:	m_pWndEditCount( NULL )
,	m_bySlot( -1 )
,	m_nCount( NULL )
{
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndCommonInputCount::~CWndCommonInputCount()
{
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
// 설명		: 돈 교환창을 베이스로 쓴다. 새로 UI를 만들기에 리소스 낭비 및 기존에 이렇게 쓰고 있더라... (ㅠ_ ㅠ)a
//---------------------------------------------------------------------------------------------
BOOL CWndCommonInputCount::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[범용 개수 입력 창] 부모 윈도우 포인터가 유효하지 않습니다.") );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, WBS_MODAL, 0, pWndParent );
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndCommonInputCount::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndButtonOk( static_cast<CWndButton*>( GetDlgItem( WIDC_OK ) ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndButtonOk, _T("[범용 개수 입력 창] 에디트 컨트롤 포인터가 유효하지 않습니다.") );

	pWndButtonOk->SetDefault( TRUE );

	m_pWndEditCount = ( static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT ) ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndEditCount, _T("[범용 개수 입력 창] 에디트 컨트롤 포인터가 유효하지 않습니다.") );

	m_pWndEditCount->SetString( _T("0") );	
	m_pWndEditCount->SetFocus();
	
	MoveParentCenter();
}  


//---------------------------------------------------------------------------------------------
// 유효한 단축 아이콘 정보인가 ?
// param	: 선택 아이템 정보
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndCommonInputCount::IsValidShortCutInfo()
{
	if( SHORTCUT_NONE == m_stShortcut.m_dwShortcut )
		return false;

	if( NULL == m_stShortcut.m_pFromWnd )
		return false;

	CWndBase* pWndFromWnd( m_stShortcut.m_pFromWnd->GetFrameWnd() );
	if( NULL == pWndFromWnd )
		return false;

	return true;
}


//---------------------------------------------------------------------------------------------
// 처리 ( 은행으로 부터 왔음 )
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonInputCount::ProcessPrivateBank()
{
	if( false == IsValidShortCutInfo() )
		return FALSE;

	if( APP_COMMON_BANK != m_stShortcut.m_pFromWnd->GetFrameWnd()->GetWndId() )
		return FALSE;

	if( APP_INVENTORY_ITEM != m_pParentWnd->GetWndId() )
		return FALSE;

	CString strCount( m_pWndEditCount->GetString() );
	m_nCount = _ttoi( strCount );

	if( m_stShortcut.m_dwData == 0 ) // 돈
	{
		if( m_nCount > static_cast<int>( g_pPlayer->m_dwGoldBank[m_bySlot] ) ) 
			m_nCount = g_pPlayer->m_dwGoldBank[m_bySlot];
	
		strCount.Format( _T("%d"), m_nCount );
		m_pWndEditCount->SetString( strCount );
	}
	else
	{
		FLItemBase* pItemBase( g_pPlayer->GetItemBankId( m_bySlot, m_stShortcut.m_dwId ) );
		if( NULL == pItemBase )
			return FALSE;
		
		if( m_nCount > ( (FLItemElem*)pItemBase )->m_nItemNum )
			m_nCount = ( (FLItemElem*)pItemBase )->m_nItemNum;

		strCount.Format( _T("%d"), m_nCount );
		m_pWndEditCount->SetString( strCount );
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 처리 ( 길드 은행으로 부터 왔음 )
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonInputCount::ProcessGuildBank()
{
	if( false == IsValidShortCutInfo() )
		return FALSE;

	if( APP_GUILD_BANK != m_stShortcut.m_pFromWnd->GetFrameWnd()->GetWndId() )
		return FALSE;

	if( APP_INVENTORY_ITEM != m_pParentWnd->GetWndId() )
		return FALSE;

	CString strCount( m_pWndEditCount->GetString() );
	m_nCount = _ttoi( strCount );

	if( m_stShortcut.m_dwData == 0 ) // 돈
	{
		if( m_nCount > static_cast<int>( g_pPlayer->GetGuild()->m_nGoldGuild ) )
			m_nCount = g_pPlayer->GetGuild()->m_nGoldGuild;
	
		strCount.Format( _T("%d"), m_nCount );
		m_pWndEditCount->SetString( strCount );
	}
	else
	{
		FLItemBase* pItemBase( g_pPlayer->GetGuild()->m_GuildBank.GetAtId( m_stShortcut.m_dwId ) );
		if( NULL == pItemBase )
			return FALSE;
		
		if( m_nCount > ( (FLItemElem*)pItemBase )->m_nItemNum )
			m_nCount = ( (FLItemElem*)pItemBase )->m_nItemNum;

		strCount.Format( _T("%d"), m_nCount );
		m_pWndEditCount->SetString( strCount );
	}

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 처리 ( 휴대 가방으로 부터 왔음 )
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonInputCount::ProcessMobileBag()
{
	if( false == IsValidShortCutInfo() )
		return FALSE;

	if( APP_GUILD_BANK != m_stShortcut.m_pFromWnd->GetFrameWnd()->GetWndId() )
		return FALSE;

	if( APP_INVENTORY_ITEM != m_pParentWnd->GetWndId() )
		return FALSE;

	CString strCount( m_pWndEditCount->GetString() );
	m_nCount = _ttoi( strCount );

	if( m_stShortcut.m_dwData == 0 ) // 돈
		return FALSE;

	FLItemBase* pItemBase( g_pPlayer->GetGuild()->m_GuildBank.GetAtId( m_stShortcut.m_dwId ) );
	if( NULL == pItemBase )
		return FALSE;
	
	if( m_nCount > ( (FLItemElem*)pItemBase )->m_nItemNum )
		m_nCount = ( (FLItemElem*)pItemBase )->m_nItemNum;

	strCount.Format( _T("%d"), m_nCount );
	m_pWndEditCount->SetString( strCount );

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 메시지 처리 ( 은행으로 부터 왔음 )
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
bool CWndCommonInputCount::ProcessChildNotifyFromPrivateBank()
{
	if( false == IsValidShortCutInfo() )
		return false;

	if( -1 == m_bySlot )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 개수 입력 창] 슬롯 인덱스 정보가 유효하지 않습니다.") );

	if( APP_COMMON_BANK != m_stShortcut.m_pFromWnd->GetFrameWnd()->GetWndId() )
		return false;

	if( APP_INVENTORY_ITEM != m_pParentWnd->GetWndId() )
		return false;

	if( m_stShortcut.m_dwData == 0 )	// 돈
	{
		if( m_nCount > 0 )
			g_DPlay.SendGetGoldBank( m_bySlot, m_nCount );
	}
	else								// 아이템
	{
		if( ITYPE_ITEM != m_stShortcut.m_dwType )
			FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 개수 입력 창] 단축 아이템 정보에 아이템 타입 정보가 유효하지 않습니다.") );

		g_DPlay.SendGetItemBank( m_bySlot, (BYTE)( m_stShortcut.m_dwId ), m_nCount );
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 메시지 처리 ( 길드 은행으로 부터 왔음 )
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
bool CWndCommonInputCount::ProcessChildNotifyFromGuildBank()
{
	if( false == IsValidShortCutInfo() )
		return false;

	if( APP_GUILD_BANK != m_stShortcut.m_pFromWnd->GetFrameWnd()->GetWndId() )
		return false;

	if( APP_INVENTORY_ITEM != m_pParentWnd->GetWndId() )
		return false;

	if( m_stShortcut.m_dwData == 0 )	// 돈
	{
		if( m_nCount > 0 )
			g_DPlay.SendGetItemGuildBank( static_cast<BYTE>( m_stShortcut.m_dwId ), m_nCount, NULL );
	}
	else								// 아이템
	{
		if( ITYPE_ITEM != m_stShortcut.m_dwType )
			FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 개수 입력 창] 단축 아이템 정보에 아이템 타입 정보가 유효하지 않습니다.") );

		g_DPlay.SendGetItemGuildBank( static_cast<BYTE>( m_stShortcut.m_dwId ), m_nCount, 1 ); 
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 메시지 처리 ( 휴대 가방으로 부터 왔음 )
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
bool CWndCommonInputCount::ProcessChildNotifyFromMobileBag()
{
	if( false == IsValidShortCutInfo() )
		return false;

	if( APP_BAG_EX != m_stShortcut.m_pFromWnd->GetFrameWnd()->GetWndId() )
		return false;

	if( APP_INVENTORY_ITEM != m_pParentWnd->GetWndId() )
		return false;

	if( m_stShortcut.m_dwData == 0 )
		return false;

	if( ITYPE_ITEM != m_stShortcut.m_dwType )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 개수 입력 창] 단축 아이템 정보에 아이템 타입 정보가 유효하지 않습니다.") );

		g_DPlay.SendMoveItem_Pocket( m_bySlot, m_stShortcut.m_dwId, m_nCount , -1 );

	return true;
}


//---------------------------------------------------------------------------------------------
// 자식으로 부터 메시지 전달
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonInputCount::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( NULL == m_pWndEditCount )
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	
	if( false == IsValidShortCutInfo() )
		return CWndNeuz::OnChildNotify( message, nID, pLResult );

	if( nID == WIDC_OK || message == EN_RETURN )
	{
		FLERROR_ASSERT_LOG_RETURN( m_stShortcut.m_pFromWnd, FALSE, _T("[범용 개수 입력 창] 단축 아이템 정보에서 From 윈도우의 포인터가 유효하지 않습니다.") );
		FLERROR_ASSERT_LOG_RETURN( m_stShortcut.m_pFromWnd->GetFrameWnd(), FALSE, _T("[범용 개수 입력 창] 단축 아이템 정보에서 From 윈도우의 포인터가 유효하지 않습니다.") );
	
		ProcessChildNotifyFromPrivateBank();
		ProcessChildNotifyFromGuildBank();
		ProcessChildNotifyFromMobileBag();

		m_stShortcut.Empty();
		Destroy();
	}

	if( WIDC_EDIT == nID )
	{
		ProcessPrivateBank();
		ProcessGuildBank();
		ProcessMobileBag();
	}

	if( WIDC_CANCEL == nID )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


//---------------------------------------------------------------------------------------------
// 출력 할 정보 설정하기
// param	: void
// return	: 성공 / 실패
//---------------------------------------------------------------------------------------------
void CWndCommonInputCount::SetInputCountInfo( const SHORTCUT* const pShortcut, const BYTE bySlot /*= 0*/ )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( pShortcut, _T("[범용 개수 입력 창] 단축 아이템 정보 포인터가 유효하지 않습니다.") );

	m_stShortcut	= (*pShortcut);
	m_bySlot		= bySlot;

	if( m_stShortcut.m_dwData == 0 ) // 돈
	{
		SetTitle( prj.GetText( TID_MMI_BANK_SAVEMONEY ) );

		CWndStatic* pStaticDesc( (CWndStatic*)GetDlgItem( WIDC_STATIC ) );
		if( pStaticDesc )
			pStaticDesc->m_strTitle = prj.GetText( TID_GAME_MOVEPENYA );

		CWndStatic* pStaticCount( (CWndStatic*)GetDlgItem( WIDC_CONTROL1 ) );
		if( pStaticCount )
			pStaticCount->m_strTitle = prj.GetText( TID_GAME_PENYACOUNT );
	}
	else
	{
		SetTitle( prj.GetText( TID_MMI_BANK_SAVEITEM ) );

		CWndStatic* pStaticDesc( (CWndStatic*)GetDlgItem( WIDC_STATIC ) );
		if( pStaticDesc )
			pStaticDesc->m_strTitle = prj.GetText( TID_GAME_MOVECOUNT );

		CWndStatic* pStaticCount( (CWndStatic*)GetDlgItem( WIDC_CONTROL1 ) );
		if( pStaticCount )
			pStaticCount->m_strTitle = prj.GetText( TID_GAME_NUMCOUNT );
	}
}
