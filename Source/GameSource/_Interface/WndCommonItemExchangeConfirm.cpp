

#include "StdAfx.h"
#include "WndCommonItemExchangeConfirm.h"

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndCommonItemExchangeConfirm::CWndCommonItemExchangeConfirm()
:	m_pWndEditCount( NULL )
,	m_dwMMIDefineIndex( -1 )
,	m_nSelectedIndex( -1 )
,	m_nPossibleMaxExchangeCount( NULL )
{
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndCommonItemExchangeConfirm::~CWndCommonItemExchangeConfirm()
{
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonItemExchangeConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[범용 아이템 교환 창(아이템 교환 개수 입력)] 부모 윈도우 포인터가 유효하지 않습니다.") );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, WBS_MODAL, 0, pWndParent );
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchangeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndButtonOk( static_cast<CWndButton*>( GetDlgItem( WIDC_OK ) ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndButtonOk, _T("[범용 아이템 교환 창(아이템 교환 개수 입력)] 에디트 컨트롤 포인터가 유효하지 않습니다.") );

	pWndButtonOk->SetDefault( TRUE );

	m_pWndEditCount = ( static_cast<CWndEdit*>( GetDlgItem( WIDC_EDIT ) ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndEditCount, _T("[범용 아이템 교환 창(아이템 교환 개수 입력)] 에디트 컨트롤 포인터가 유효하지 않습니다.") );

	m_pWndEditCount->SetString( _T("0") );	
	m_pWndEditCount->SetFocus();
	
	MoveParentCenter();
}  


//---------------------------------------------------------------------------------------------
// 아이템 교환 정보 설정
// param	: MMI 정의 인덱스
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchangeConfirm::SetItemExchangeInfo(const DWORD dwMMIDefineIndex, const int nSelectedIndex, const int PossibleMaxExchangeCount)
{
	if( -1 == dwMMIDefineIndex )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 아이템 교환 창(아이템 교환 개수 입력)] MMI 인덱스 정보가 올바르지 않습니다.") );

	if( -1 == nSelectedIndex )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 아이템 교환 창(아이템 교환 개수 입력)] 선택 인덱스 정보가 올바르지 않습니다.") );

	if( 1 > PossibleMaxExchangeCount )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 아이템 교환 창(아이템 교환 개수 입력)] 교환 개수 정보가 올바르지 않습니다.") );

	m_dwMMIDefineIndex			= dwMMIDefineIndex;
	m_nSelectedIndex			= nSelectedIndex;
	m_nPossibleMaxExchangeCount = PossibleMaxExchangeCount;
	
	SetTitle( prj.GetText( TID_MMI_EXCHANGEAMOUNT_TITLE ) );

	CWndStatic* pStaticDesc( (CWndStatic*)GetDlgItem( WIDC_STATIC ) );
	if( pStaticDesc )
		pStaticDesc->m_strTitle = prj.GetText( TID_MMI_EXCHANGEAMOUNT_TEXT );

	CWndStatic* pStaticCount( (CWndStatic*)GetDlgItem( WIDC_CONTROL1 ) );
	if( pStaticCount )
		pStaticCount->m_strTitle = prj.GetText( TID_GAME_NUMCOUNT );

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 개수 입력 처리
// param	: void
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonItemExchangeConfirm::ProcessInputCount()
{
	CString strCount( m_pWndEditCount->GetString() );
	int nCount = _ttoi( strCount );

	if( nCount > m_nPossibleMaxExchangeCount )
		nCount = m_nPossibleMaxExchangeCount;

	strCount.Format( _T("%d"), nCount );
	m_pWndEditCount->SetString( strCount );

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 자식으로 부터 메시지 전달
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonItemExchangeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( NULL == m_pWndEditCount )
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		if( m_pParentWnd &&
			APP_DIALOG_EVENT == m_pParentWnd->GetWndId() )
		{
			CString strCount( m_pWndEditCount->GetString() );
			int nCount = _ttoi( strCount );

			if( NULL < nCount )
			{
				FLPacketItemExchangeReq	kPacket;

				kPacket.dwMenuID		= m_dwMMIDefineIndex;
				kPacket.dwListNth		= m_nSelectedIndex;
				kPacket.nExchangeCount	= nCount;

				g_DPlay.SendPacket( &kPacket );

				CWndCommonItemExchange* pWndCommonItemExchange( static_cast<CWndCommonItemExchange*>( m_pParentWnd ) );
				if( pWndCommonItemExchange )
					pWndCommonItemExchange->SetSendPacket();		
			}
		}
		
		Destroy();
	}

	if( WIDC_EDIT == nID )
	{
		ProcessInputCount();
	}

	if( WIDC_CANCEL == nID )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
