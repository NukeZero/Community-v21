
#include "StdAfx.h"
#include "WndField2.h"
#include "party.h"

#include "WndRegVend.h"
#include "CommonCtrl.h"

#define MAX_PEYNA_FOR_CLIENT ( 2100000000 )		//21억

void MakeMoneyStyle( OUT CString& str )
{
	//천단위마다 ','추가해서 뱉어줌( 100000000 -> 100,000,000 )
	CString rst;
	int len = str.GetLength( );
	if( len < 4 )
	{
		rst.Format( "%10s", str.GetString() );
		str = rst;
		return;
	}

	int nComma = 0;
	for( int i = len-1; i >= 0; --i )
	{
		++nComma;
		char c = str.GetAt( i );
		if( c == ' ' )
			break;
		if( nComma > 3 )
		{
			rst.Insert( 0, ',' );
			nComma = 1;
		}
	
		rst.Insert( 0, c );
	}

	str.Format( "%10s", rst.GetString() );
}

//----------------------------------------------------------------------------------------------------------
//CWndWidget
//----------------------------------------------------------------------------------------------------------
CWndWidget::CWndWidget( )
{
}

CWndWidget::~CWndWidget( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndWidget::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndWidget::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndWidget::Process( void )
{
	return TRUE;
}

void CWndWidget::OnDestroyChildWnd( CWndBase* pWndChild )
{

}

BOOL CWndWidget::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndWidget::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndWidget::OnDraw( C2DRender* p2DRender ) 
{
}


//----------------------------------------------------------------------------------------------------------
//CWndInputNumCommon
//----------------------------------------------------------------------------------------------------------
CWndInputNumCommon::CWndInputNumCommon( )
{
	m_bNoCloseButton = TRUE;
}

CWndInputNumCommon::~CWndInputNumCommon( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInputNumCommon::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_INPUTNUM_NORMAL, WBS_MODAL | WBS_KEY, CPoint( 0, 0 ), pWndParent );
} 

void CWndInputNumCommon::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndEdit* pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	FLASSERT( pEdit );

	pEdit->AddWndStyle(EBS_NUMBER);		//숫자만 입력받음.
	pEdit->SetFocus();
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndInputNumCommon::Process( void )
{
	return TRUE;
}

BOOL CWndInputNumCommon::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	//ENTER입력시 OK와 동일한 기능 수행
	if( WIDC_EDIT1 == nID )
	{
		if( EN_RETURN == message )
			OnOK( );
	}
	else if( WIDC_BUTTON1 == nID )		//확인
	{	
		OnOK( );
	}
	else if( WIDC_BUTTON2 == nID )		//취소
	{
		m_pParentWnd->OnCommand( APP_INPUTNUM_NORMAL, MB_CANCEL, this );
		Destroy();
	}
	
return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndInputNumCommon::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndInputNumCommon::OnOK( )
{
	CWndEdit* pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	int nCount = 0;
	if( pEdit->GetEditNum( nCount ) )
	{
		m_pParentWnd->OnCommand( APP_INPUTNUM_NORMAL, MB_OK, this );
		Destroy();
	}
	else
	{
		//입력문자 오류
		//TODO : 숫자를 입력해야한다는 경고문
		pEdit->ResetString();
	}
}

int CWndInputNumCommon::GetNumeric( )
{
	CWndEdit* pEdit = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	int nCount = 0;
	pEdit->GetEditNum( nCount );

	return nCount;
}

//----------------------------------------------------------------------------------------------------------
//CWndDoanationItem
//----------------------------------------------------------------------------------------------------------
CWndDoanationItem::CWndDoanationItem( ) :
m_pTexture( NULL ),
m_nCount(0),
m_dwItemIndex(NULL_ID)
{
}

CWndDoanationItem::~CWndDoanationItem( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDoanationItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DONATION_ITEM, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 

void CWndDoanationItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndDoanationItem::Process( void )
{
	return TRUE;
}

BOOL CWndDoanationItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	//if( APP_INPUTNUM_NORMAL == nID )		// 수량 입력창 
	//{
	//	if( MB_OK == dwMessage )
	//	{
	//		m_nItem = ( (CWndInputNumCommon*) pWndBase )->GetNumeric();
	//	}
	//	else if( MB_CANCEL == dwMessage )
	//	{
	//		m_nItem = 0;
	//		m_dwItemIndex = 0;
	//		m_dwItemID = NULL_ID;
	//	}
	//}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndDoanationItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	//닫기, 기부, 취소의 경우 알려만 주고 부모에서 삭제을 담당
	switch( nID )
	{
	case WTBID_CLOSE:
		m_pParentWnd->OnCommand( APP_DONATION_ITEM, MB_CANCEL, this );
		break;

	case WIDC_BUTTON1:		//기부
		{
			CObj* pTarget = g_WorldMng.Get()->GetObjFocus();
			if( pTarget )
			{
				if( pTarget->GetType() == OT_MOVER )
					g_DPlay.SendCooperativeContributions_Contribute( ((CMover*)pTarget)->GetId() );
			}
		}
		m_pParentWnd->OnCommand( APP_DONATION_ITEM, MB_OK, this );
		break;

	case WIDC_BUTTON2:
		m_pParentWnd->OnCommand( APP_DONATION_ITEM, MB_CANCEL, this );
		break;

	default: break;

	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDoanationItem::OnDraw( C2DRender* p2DRender ) 
{
	if( m_pTexture )
	{
		WNDCTRL* wndCtrl = GetWndCtrl( WIDC_STATIC1 );		//슬롯
		if( !wndCtrl )
			return;

		if(m_pTexture)
			m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}

	//수량
	CWndStatic* pWndCount = (CWndStatic*)GetDlgItem( WIDC_ET_AMOUNT );
	CString str;
	str.Format( "%d", m_nCount );
	pWndCount->SetTitle( str );
}


//BOOL CWndDoanationItem::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
//{
//	SAFE_DELETE( m_pWndNumTabulet );
//	m_pWndNumTabulet = new CWndInputNumCommon;
//	m_pWndNumTabulet->Initialize( this );
//
//	//정보갱신후 입력기의 행동에따라 적절히 대처한다.
//	m_dwItemID = pShortcut->m_dwIndex;
//	m_dwItemIndex = pShortcut->m_dwId;
//	
//	T_ITEM_SPEC* pItemProp = g_xSpecManager->GetSpecItem( m_dwItemIndex );
//	if( pItemProp )
//		m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );
//
//	return TRUE;
//}

void CWndDoanationItem::SetData( const DWORD dwItemIndex, const int nCount )
{
	m_dwItemIndex = dwItemIndex;
	m_nCount = nCount;

	T_ITEM_SPEC* pItemProp = g_xSpecManager->GetSpecItem( m_dwItemIndex );
	if( pItemProp )
		m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );
}

//----------------------------------------------------------------------------------------------------------
//CWndDoanationPenya
//----------------------------------------------------------------------------------------------------------
CWndDoanationPenya::CWndDoanationPenya( ) :
m_nCount(0)
{
}

CWndDoanationPenya::~CWndDoanationPenya( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDoanationPenya::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DONATION_MONEY, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 

void CWndDoanationPenya::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndDoanationPenya::Process( void )
{
	return TRUE;
}

BOOL CWndDoanationPenya::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndDoanationPenya::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	//닫기, 기부, 취소의 경우 알려만 주고 부모에서 삭제을 담당
	switch( nID )
	{
	case WTBID_CLOSE:
		m_pParentWnd->OnCommand( APP_DONATION_MONEY, MB_CANCEL, this );
		break;

	case WIDC_BUTTON1:		//기부
		{
			CObj* pTarget = g_WorldMng.Get()->GetObjFocus();
			if( pTarget )
			{
				if( pTarget->GetType() == OT_MOVER )
					g_DPlay.SendCooperativeContributions_Contribute( ((CMover*)pTarget)->GetId() );
			}
		}
		
		m_pParentWnd->OnCommand( APP_DONATION_MONEY, MB_OK, this );
		break;

	case WIDC_BUTTON2:
		m_pParentWnd->OnCommand( APP_DONATION_MONEY, MB_CANCEL, this );
		break;

	default: break;

	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDoanationPenya::OnDraw( C2DRender* p2DRender ) 
{
	CWndStatic* pWndCount = (CWndStatic*)GetDlgItem( WIDC_ET_MONEY );
	CString str;
	str.Format( "%d", m_nCount );
	pWndCount->SetTitle( str );
}

void CWndDoanationPenya::SetData( const int nCount )
{
	m_nCount = nCount;
}

//----------------------------------------------------------------------------------------------------------
//CWndDonation
//----------------------------------------------------------------------------------------------------------
CWndDonation::CWndDonation( ) :
m_pTexGuage( NULL ),
m_pVertexBufferGauge( NULL ),
m_pWndItem( NULL ),
m_pWndPenya( NULL ),
m_nMaxGauage( 100 ),
m_nCurrGauage( 0 ),
m_fRate( 0.0f ),
m_n64RemainSec( 0 ),
m_fTimeSaver( 0.0f )
{
}

CWndDonation::~CWndDonation( )
{
	SAFE_DELETE( m_pWndItem );
	SAFE_DELETE( m_pWndPenya );
	DeleteDeviceObjects();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDonation::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DONATION, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndDonation::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_pTexGuage = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("WndBar02.tga") ), WNDCOLOR_DEFAULT_KEY );

	if( !m_pTexGuage )
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pTexGuage(Wndguage.tga) is NULL" ) );

	RestoreDeviceObjects( );

	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	if( pWndGauage )
	{
		CRect rect = pWndGauage->GetWndRect();
		int nMax = rect.Width();
		m_nMaxGauage = nMax;
	}

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC16 );
	pStatic->SetTitle( "" );
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC15 );
	pStatic->SetTitle( "" );
	pStatic = (CWndStatic*)GetDlgItem( WIDC_ST_GOAL );
	pStatic->SetTitle( "" );
	pStatic = (CWndStatic*)GetDlgItem( WIDC_ST_TIME );
	pStatic->SetTitle( "" );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndDonation::SetEnableButtonOK( BOOL bEnable )
{
	CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndBtnOK->EnableWindow( bEnable );
}

BOOL CWndDonation::Process( void )
{
	return TRUE;
}

BOOL CWndDonation::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( APP_DONATION_ITEM == nID )
	{
		if( MB_OK == dwMessage )
		{
			//SAFE_DELETE( m_pWndItem ) 로 하는경우, IsOpenModal같은 함수에서 문제를 일으킬수있다.
			m_pWndItem->Destroy();
			m_pWndItem = NULL;

			//응답받을때까지 버튼 비활성화
			//SetEnableButtonOK( FALSE );
		}
		else if( MB_CANCEL == dwMessage )
		{
			m_pWndItem->Destroy();
			m_pWndItem = NULL;
		}
	}
	else if( APP_DONATION_MONEY == nID )
	{
		if( MB_OK == dwMessage )
		{
			m_pWndPenya->Destroy( );
			m_pWndPenya = NULL;

			//응답받을때까지 버튼 비활성화
			//SetEnableButtonOK( FALSE );
		}
		else if( MB_CANCEL == dwMessage )
		{
			m_pWndPenya->Destroy( );
			m_pWndPenya = NULL;
		}
	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndDonation::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			//무조건 기부항목은 하나임! ( 아이템 이거나 페냐이거나 )
			if( m_kData[0]._unit != 0 )
				CreateSubWindow_Item( m_kData[0]._id, m_kData[0]._unit );
			else if( m_kData[1]._unit != 0 )
				CreateSubWindow_Penya( m_kData[1]._unit );
		}
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDonation::OnMouseWndSurface( CPoint point )
{
	CWndStatic* pWndSlot = NULL;
	CRect rect;

	size_t nSize = m_vRewardItems.size();
	if( nSize == 0 )
		return;

	switch( nSize )
	{
		case 4 : PutTooltip_RewardItems( m_vRewardItems[3], WIDC_STATIC13, point );		// no break;
		case 3 : PutTooltip_RewardItems( m_vRewardItems[2], WIDC_STATIC12, point );
		case 2 : PutTooltip_RewardItems( m_vRewardItems[1], WIDC_STATIC11, point );
		case 1 : PutTooltip_RewardItems( m_vRewardItems[0], WIDC_STATIC10, point );
		break;

		default : FLASSERT( 0 );
		break;
	}
	
}

BOOL CWndDonation::PutTooltip_RewardItems( DWORD index, DWORD ui_index, const CPoint& point )
{
	CWndStatic* pWndSlot = (CWndStatic*)GetDlgItem( ui_index );
	if( !pWndSlot )
		return FALSE;

	CRect rect = pWndSlot->GetWndRect( );

	if( rect.PtInRect( point ) == TRUE )
	{
		FLItemElem kItemElem;
		kItemElem.m_dwItemId = index;
		CPoint point2 = point;
		CRect newRect = rect;
		ClientToScreen( &point2 );
		ClientToScreen( &newRect );

		g_WndMng.PutToolTip_Item( (FLItemBase*)&kItemElem, point2, &newRect );
		
		return TRUE;
	}

	return FALSE;
}

void CWndDonation::CreateSubWindow_Item( const DWORD dwIndex, const int nCount )
{
	SAFE_DELETE( m_pWndItem );
	m_pWndItem = new CWndDoanationItem;
	m_pWndItem->Initialize( this );
	m_pWndItem->SetData( dwIndex, nCount );
}

void CWndDonation::CreateSubWindow_Penya( const int nCount )
{
	SAFE_DELETE( m_pWndPenya);
	m_pWndPenya = new CWndDoanationPenya;
	m_pWndPenya->Initialize( this );
	m_pWndPenya->SetData( nCount );
}

void CWndDonation::SetData_MainInfo( DWORD subject, __time64_t tRemainSec, DWORD condition )
{
	//목표
	CWndStatic* pStatic1 = (CWndStatic*)GetDlgItem( WIDC_STATIC16 );
	if( pStatic1 ) 
	{
		pStatic1->SetTitle( prj.GetText( subject ) );
	}

	//조건
	CWndStatic* pStatic2 = (CWndStatic*)GetDlgItem( WIDC_STATIC15 );
	if( pStatic2 ) 
	{
		pStatic2->SetTitle( prj.GetText( condition ) );
	}

	m_n64RemainSec = tRemainSec;
	
}

void CWndDonation::SetData_Item( DWORD itemID, size_t unit, DWORD max,  DWORD curr )
{
	m_kData[0]._id = itemID;
	m_kData[0]._max = max;
	m_kData[0]._unit = unit;
	m_kData[0]._curr = curr;

	m_fRate = (float)curr / (float)max;
	if( max <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "max:%d" ), max );
	}

	//CWndStatic* pStatic2 = (CWndStatic*)GetDlgItem( WIDC_STATIC15 );
	//if( pStatic2 )
	//{
	//	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( itemID );
	//	if( pItemProp )
	//	{
	//		CString str;
	//		str.Format( "%s %d stirng id없음", pItemProp->szName, unit );
	//		pStatic2->SetTitle( str );
	//	}
	//}
}

void CWndDonation::SetData_Money( size_t unit, int nGoldMax, int curr )
{
	m_kData[1]._id = 0;
	m_kData[1]._max = nGoldMax;
	m_kData[1]._unit = unit;
	m_kData[1]._curr = curr;

	m_fRate = (float)curr / (float)nGoldMax;
	if( nGoldMax <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "max:%d" ), nGoldMax );
	}

	CWndStatic* pStatic2 = (CWndStatic*)GetDlgItem( WIDC_STATIC15 );
	if( pStatic2 )
	{
		CString str;
		str.Format( "%d", unit );
		pStatic2->SetTitle( str );
	}
}

void CWndDonation::SetData_Reward( std::vector< DWORD >& items, std::vector< DWORD >& strings )
{
	m_vRewardItems = items;

	size_t nSize = m_vRewardItems.size();
	CWndBase* pWndSlot = NULL;
	pWndSlot = (CWndBase*)GetDlgItem(WIDC_STATIC10);
	pWndSlot->SetVisible( FALSE );
	//pWndSlot = (CWndBase*)GetDlgItem(WIDC_STATIC11);
	//pWndSlot->SetVisible( FALSE );
	//pWndSlot = (CWndBase*)GetDlgItem(WIDC_STATIC12);
	//pWndSlot->SetVisible( FALSE );
	//pWndSlot = (CWndBase*)GetDlgItem(WIDC_STATIC13);
	//pWndSlot->SetVisible( FALSE );

	//m_fRate = 1.0f;

	//switch( nSize )
	//{
	//case 4 : 
	//	pWndSlot = (CWndStatic*)GetDlgItem(WIDC_STATIC13);
	//	pWndSlot->SetVisible( TRUE );
	//case 3 : 
	//	pWndSlot = (CWndStatic*)GetDlgItem(WIDC_STATIC12);
	//	pWndSlot->SetVisible( TRUE );
	//case 2 : 
	//	pWndSlot = (CWndStatic*)GetDlgItem(WIDC_STATIC11);
	//	pWndSlot->SetVisible( TRUE );
	//case 1 : 
	//	pWndSlot = (CWndStatic*)GetDlgItem(WIDC_STATIC10);
	//	pWndSlot->SetVisible( TRUE );
	//	break;

	//default :
	//	break;
	//}
		
	CWndStatic* pStatic = NULL;
	size_t size_strings = strings.size();
	if( size_strings > 0 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC17 );
		if( pStatic )
			pStatic->SetTitle( prj.GetText( strings[0] ) );
	}
	
	if( size_strings > 1 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC18 );
		if( pStatic )
			pStatic->SetTitle( prj.GetText( strings[1] ) );
	}

	if( size_strings > 2 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC19 );
		if( pStatic )
			pStatic->SetTitle( prj.GetText( strings[2] ) );
	}

	if( size_strings > 3 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC20 );
		if( pStatic )
			pStatic->SetTitle( prj.GetText( strings[3] ) );
	}
}

void CWndDonation::RenderRewardItems( C2DRender* p2DRender, DWORD index, DWORD wndid )
{
	LPWNDCTRL pSlot = GetWndCtrl( wndid );
	if( !pSlot )
		return;

	CTexture* pTex = NULL;
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( index );
	if( pItemProp )
	{
		pTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if( pTex )
			pTex->Render( p2DRender, CPoint( pSlot->rect.left, pSlot->rect.top ));
	}
}

void CWndDonation::OnDraw( C2DRender* p2DRender ) 
{
	CWndStatic* pWndGauage = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	if( pWndGauage )
	{
		FLASSERT( m_nMaxGauage > 0 );

		m_nCurrGauage = (int)(m_fRate * (float)m_nMaxGauage);
		
		CRect rect = pWndGauage->GetWndRect();
		rect.right = rect.left + m_nCurrGauage;

		m_pTheme->RenderGauge(p2DRender, &rect, WNDCOLOR_WHITE, m_pVertexBufferGauge, m_pTexGuage);
		CString strTemp;
		int nPercent = (int)( m_fRate * 100 );
		strTemp.Format( "%d%%", nPercent );

		p2DRender->TextOut( rect.left + 10, rect.top + 1,  strTemp, m_dwColor );
	}

	size_t size_item = m_vRewardItems.size();
	if( size_item > 0 )
		RenderRewardItems( p2DRender, m_vRewardItems[0], WIDC_STATIC10 );
	if( size_item > 1 )
		RenderRewardItems( p2DRender, m_vRewardItems[1], WIDC_STATIC11 );
	if( size_item > 2 )
		RenderRewardItems( p2DRender, m_vRewardItems[2], WIDC_STATIC12 );
	if( size_item > 3 )
		RenderRewardItems( p2DRender, m_vRewardItems[3], WIDC_STATIC13 );

	//자체 타이머 돌림
	m_fTimeSaver += _GetDeltaTimeSec();
	if( m_fTimeSaver >= 1.0f )
	{
		m_n64RemainSec -= 1;
		m_fTimeSaver = 0.0f;

		if( m_n64RemainSec < 0 )
			m_n64RemainSec = 0;
	}

	//남은시간
	CWndStatic* pStaticTime = (CWndStatic*)GetDlgItem( WIDC_ST_TIME );
	if( pStaticTime ) 
	{
		__time64_t tRemainSec = m_n64RemainSec;
		int nDay = (int)( tRemainSec / (60 * 60 * 24) );
		tRemainSec = tRemainSec - (60 * 60 * 24) * nDay;
		int nHour = (int)( tRemainSec / ( 60  * 60 ) );
		tRemainSec = tRemainSec - ( 60 * 60 ) * nHour;
		int nMin = (int)( tRemainSec / 60 );
		tRemainSec = tRemainSec - 60 * nMin;
		int nSec = (int)tRemainSec;

		FLASSERT( nHour < 24 );
		FLASSERT( nMin < 60 );
		FLASSERT( nSec < 60 );

		CString str;
		str.Format( prj.GetText( TID_TOOLTIP_DATE ), nDay, nHour, nMin, nSec );	// 지속시간 : 
		pStaticTime->SetTitle( str );
	}
} 

HRESULT CWndDonation::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	FLASSERT(m_pVertexBufferGauge != NULL);
	
	return S_OK;
}
HRESULT CWndDonation::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
HRESULT CWndDonation::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}

//----------------------------------------------------------------------------------------------------------
//CWndDonationRanking
//----------------------------------------------------------------------------------------------------------
CWndDonationRanking::CWndDonationRanking( )
{
}

CWndDonationRanking::~CWndDonationRanking( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDonationRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DONATIONRANK, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 

void CWndDonationRanking::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndDonationRanking::Process( void )
{
	return TRUE;
}

BOOL CWndDonationRanking::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndDonationRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1:		//OK
		Destroy();
		break;

	default: break;

	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndDonationRanking::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndDonationRanking::SetData( int arrCount, TCHAR szData[][MAX_NAME] )
{
	if( arrCount != 3 )
	{
		FLASSERT( 0 );
		FLERROR_LOG( PROGRAM_NAME, _T( "arrCount %d" ), arrCount );
		return;
	}
	
	CWndStatic* pStatic = NULL;
	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
	if( pStatic )
		pStatic->SetTitle( szData[ 0 ] );

	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	if( pStatic )
		pStatic->SetTitle( szData[ 1 ] );

	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	if( pStatic )
		pStatic->SetTitle( szData[ 2 ] );
}


//----------------------------------------------------------------------------------------------------------
//CWndColosseumJoin
//----------------------------------------------------------------------------------------------------------
CWndColosseumJoin::CWndColosseumJoin( ) :
m_nSelectedID( 0 )
{
}

CWndColosseumJoin::~CWndColosseumJoin( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumJoin::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOENTER, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumJoin::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	SetEnableButton( WIDC_BUTTON3, FALSE );
	SetEnableButton( WIDC_BUTTON4, FALSE );
} 

BOOL CWndColosseumJoin::Process( void )
{
	return TRUE;
}

BOOL CWndColosseumJoin::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumJoin::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1:	//저렙일반
		SetSelectedButton( WIDC_BUTTON1 );
		SetEnableButton( WIDC_BUTTON3, TRUE );
		SetEnableButton( WIDC_BUTTON4, TRUE );
		break;

	case WIDC_BUTTON2:	//고렙일반
		SetSelectedButton( WIDC_BUTTON2 );
		SetEnableButton( WIDC_BUTTON3, TRUE );
		SetEnableButton( WIDC_BUTTON4, TRUE );
		break;
		
	case WIDC_BUTTON3:	//일반 신청
		SendPacket( 0 );
		break;

	case WIDC_BUTTON4:  //길드 신청
		SendPacket( 1 );
		break;

//	case WIDC_BUTTON6: //취소
//		Destroy( );
//		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumJoin::SendPacket( const int nKind )
{
	T_PACKET_COLOSSEUM_ENTER_REQ kData;

	//리더만 허용
	if( FALSE == g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
	{
		Destroy();
		return;
	}

	//레벨 
	if( WIDC_BUTTON1 == m_nSelectedID )
		kData.dwDungeonLevel = 0;
	else if( WIDC_BUTTON2 == m_nSelectedID )
		kData.dwDungeonLevel = 1;
	else
		return;
	
	//일반,길드
	if( 0 == nKind ) 
		kData.bGuild = FALSE;
	else if( 1 == nKind )
		kData.bGuild = TRUE;
	else 
		return;
	
	g_DPlay.SendColosseumEnter( &kData );
	SetEnableButtonAll( FALSE );
}

void CWndColosseumJoin::SetEnableButton( const DWORD id, const BOOL bEnable )
{
	CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem( id );		
	if( pWndBtnOK )
		pWndBtnOK->EnableWindow( bEnable );
}

void CWndColosseumJoin::SetEnableButtonAll( const BOOL bEnable )
{
	CWndButton* pWndBtn = NULL;
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON1);		
	pWndBtn->EnableWindow( bEnable );
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON2);		
	pWndBtn->EnableWindow( bEnable );
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON3);		
	pWndBtn->EnableWindow( bEnable );
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON4);		
	pWndBtn->EnableWindow( bEnable );
}

void CWndColosseumJoin::ResetButtonAll( )
{
	m_nSelectedID = 0;

	CWndButton* pWndBtn = NULL;
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON1);		
	pWndBtn->EnableWindow( TRUE );
	pWndBtn->SetCheck( 0 );
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON2);		
	pWndBtn->EnableWindow( TRUE );
	pWndBtn->SetCheck( 0 );
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON3);		
	pWndBtn->EnableWindow( FALSE );
	pWndBtn->SetCheck( 0 );
	pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON4);		
	pWndBtn->EnableWindow( FALSE );
	pWndBtn->SetCheck( 0 );
}

void CWndColosseumJoin::OnDraw( C2DRender* p2DRender ) 
{

} 

void CWndColosseumJoin::SetSelectedButton( int nID )
{
	CWndButton* pWndBtn1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);		//저레벨
	CWndButton* pWndBtn2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);		//고레벨 

	pWndBtn1->SetCheck( 0 );
	pWndBtn2->SetCheck( 0 );

	CWndButton* pWndBtnSelected = (CWndButton*)GetDlgItem(nID);		
	if( pWndBtnSelected )
	{
		pWndBtnSelected->SetCheck( 2 );
		m_nSelectedID = nID;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Button ID is %d" ), nID );
	}
}

//----------------------------------------------------------------------------------------------------------
//CWndColosseumReadyToStart
//----------------------------------------------------------------------------------------------------------
CWndColosseumReadyToStart::CWndColosseumReadyToStart( ) 
{
	m_itemID = NULL_ID;
	m_itemIndex = NULL_ID;
	m_fSec = 0.0f;
	m_pWndMsgBox = NULL;

	m_bNoCloseButton = TRUE;
}

CWndColosseumReadyToStart::~CWndColosseumReadyToStart( )
{
	SAFE_DELETE( m_pWndMsgBox );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumReadyToStart::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOSTART, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumReadyToStart::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	CRect rect = GetWndRect( );
	Move( rect.left, 30 );
}
 

BOOL CWndColosseumReadyToStart::Process( void )
{
	CWndButton* pBtnStart = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( !pBtnStart )
		return FALSE;

	if( !g_pPlayer )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	//극단장만 시작버튼 활성
	BOOL bLeader = g_Party.IsLeader( g_pPlayer->m_idPlayer );
	pBtnStart->EnableWindow( bLeader );

    m_fSec -= _GetDeltaTimeSec( );

	//시간초과되더라도 클라는 0으로 표시만 하고, 서버사이드에서 처리한다.
	if( m_fSec < 0.0f )
		m_fSec = 0.0f;

	int nTotalSec = (int)m_fSec;
	FLASSERT( nTotalSec < ( 24 * 60 * 60 ) );		//하루를 넘길수없다고 가정함.

	DWORD dwColor = m_dwColor;
	if( nTotalSec < 11 )
		dwColor = 0xffff1111;

	int nHour, nMin, nSec;
	_GetHMS_bySec( nTotalSec, nHour, nMin, nSec );

	CString str;
	str.Format( "%02d : %02d : %02d", nHour, nMin, nSec );

	CWndStatic* pWndTime = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	pWndTime->SetTileColor( dwColor );
	pWndTime->SetTitle( str );

	return TRUE;
}
BOOL CWndColosseumReadyToStart::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId( ) )
	{
		FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
		if( pItem )
		{
			T_ITEM_SPEC* pProp = pItem->GetProp( );
			if( pProp->dwID != ITEM_INDEX( 20941, II_SYS_SYS_SCR_BOSSPLUSE ) )
				return FALSE;

			//극단장이 아니면 패스
			if( FALSE == g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
				return FALSE;
		}
	
		//gmpbigsun( 20110531 )
		//메르데르의 인장 사용시 간헐적으로 일어나는 크래쉬는 this포인터 깨짐으로 확인댐
		//OpenMessageBox로 부모포인터를 넣는행위를 안하는것이 좋겟다..문제점을 완젼히 없앨때까지
		//g_WndMng.OpenMessageBox( prj.GetText(TID_COLOSSEUM_JINBOSS01), MB_OKCANCEL, this );	//진보스 확률 업이다 진짜 사용할래?

		//아래 자체 메세지 박스 생성을 대체함.
		SAFE_DELETE( m_pWndMsgBox );
		m_pWndMsgBox = new CWndMessageBox;
		BOOL bInit = m_pWndMsgBox->Initialize( prj.GetText( TID_COLOSSEUM_JINBOSS01 ), this, MB_OKCANCEL );
		if( FALSE == bInit )
		{
			SAFE_DELETE( m_pWndMsgBox );
			FLERROR_LOG( PROGRAM_NAME, "init failed" );
		}
		else
		{
			CWndButton* pButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
			if( pButton )
				pButton->EnableWindow( FALSE );
		}

		SetItemID( pShortcut->m_dwId );
	}

	return TRUE;
	
}

void CWndColosseumReadyToStart::DropedIcon( const DWORD itemid )
{
	//드래그로 아이템 빼냄
	g_DPlay.SendColosseumUnregistAdditionalRealMonsterRateItem();
}

void CWndColosseumReadyToStart::OnLButtonDown( UINT nFlags, CPoint point )
{
	//극단장이 아니면 패스
	if( FALSE == g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
		return;

	//아이템이 등록 안되어 있으면 패스
	if( m_itemID == NULL_ID )
		return;

	//해당 슬롯을 판별해서 정보를 shortcut에 넣어준다.
	LPWNDCTRL wndCtrl = NULL;
	CRect rect;

	CWndStatic* pWndSlot = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	rect = pWndSlot->GetWndRect( );

	if( FALSE == rect.PtInRect( point ) )
		return;

	FLItemBase* pItem = g_pPlayer->GetItemId( m_itemID );
	if( !pItem )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "cannot find item objid:%d" ), m_itemID );
		return;
	}
	
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
	m_GlobalShortcut.m_dwId       = m_itemID;
	m_GlobalShortcut.m_dwIndex    = pItem->GetProp()->dwID;
	m_GlobalShortcut.m_pTexture   = pItem->GetTexture( );
	m_GlobalShortcut.m_dwData     = 0;
}


BOOL CWndColosseumReadyToStart::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	//유료아이템 사용 확인에 대한 응답 메세지
	if( nID == IDOK )
	{
		//ok
		g_DPlay.SendColosseumRegistAdditionalRealMonsterRateItem( m_itemID );
	}
	else if( nID == IDCANCEL )
	{
		m_itemID = NULL_ID;
	}

	//hm.. 
	CWndButton* pButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pButton )
		pButton->EnableWindow( TRUE );

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndColosseumReadyToStart::SetItemID( const OBJID objid )
{
	m_itemID = objid;
}


void CWndColosseumReadyToStart::ReceivedPacketNoti( const DWORD itemindex )
{
	if( itemindex == NULL_ID )	//해제됨
	{
		m_itemID = NULL_ID;
		m_itemIndex = NULL_ID;
	}
	else
	{
		//m_itemID ok
		m_itemIndex = itemindex;
	}
}

BOOL CWndColosseumReadyToStart::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1 : 
		SendPacket( );
		Destroy();
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumReadyToStart::SetTimer( const float seconds )
{
	m_fSec = seconds;	
}

void CWndColosseumReadyToStart::SendPacket( )
{
	g_DPlay.SendColosseumStart( );
}

void CWndColosseumReadyToStart::OnDraw( C2DRender* p2DRender ) 
{
	if( m_itemIndex == NULL_ID )
		return;

	CWndStatic* pWndSlot = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	CRect rect = pWndSlot->GetWndRect( );

	T_ITEM_SPEC* pItemProp = g_xSpecManager->GetSpecItem( m_itemIndex );
	if( !pItemProp )
		return;

	CTexture* pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

	if( pTexture )
		p2DRender->RenderTexture( rect.TopLeft(), pTexture );
} 

//----------------------------------------------------------------------------------------------------------
//CWndColosseumRetry
//----------------------------------------------------------------------------------------------------------
CWndColosseumRetry::CWndColosseumRetry( ) 
{
	m_fSec = 0.0f;
	m_bNoCloseButton = TRUE;
}

CWndColosseumRetry::~CWndColosseumRetry( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumRetry::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLORETRY, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumRetry::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndColosseumRetry::Process( void )
{
	m_fSec -= _GetDeltaTimeSec( );

	//시간 초과되면 클라는 걍 0으로 표시되고 서버사이드에서 처리한다.
	if( m_fSec < 0.0f )
		m_fSec = 0.0f;

	int nTotalSec = (int)m_fSec;
	FLASSERT( nTotalSec < ( 24 * 60 * 60 ) );		//하루를 넘길수없다고 가정함.

	DWORD dwColor = m_dwColor;
	if( nTotalSec < 11 )
		dwColor = 0xffff1111;

	int nHour, nMin, nSec;
	_GetHMS_bySec( nTotalSec, nHour, nMin, nSec );

	CString str;
	str.Format( "%02d : %02d : %02d", nHour, nMin, nSec );

	CWndStatic* pWndTime = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	pWndTime->SetTileColor( dwColor );
	pWndTime->SetTitle( str );

	CWndButton* pWndRetry = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	if( !pWndRetry )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "cannot find WIDC_BUTTON3" ) );
		return TRUE;
	}

	//파티원들은 재도전 버튼 비활성
	if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
	{
		//메르데르의 가보가 있다면 활성, 없으면 비활성
		FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtByItemKind3( IK3_COLOSSEUM_RETRY );
		pWndRetry->EnableWindow( pItem != NULL );	
	}
	else
	{
		pWndRetry->EnableWindow( FALSE );
	}

	return TRUE;
}

BOOL CWndColosseumRetry::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumRetry::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1 : //go out
		g_DPlay.SendColosseumOut( );
		Destroy();
		break;

	case WIDC_BUTTON2:	//shop
		g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_WEBBOX );
		break;

	case WIDC_BUTTON3:	//retry
		{
			FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtByItemKind3( IK3_COLOSSEUM_RETRY );
			if( pItem )
				g_DPlay.SendColosseumRetry( pItem->m_dwObjId );
		}
	
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumRetry::SetTimer( const float seconds )
{
	m_fSec = seconds;	
}


void CWndColosseumRetry::OnDraw( C2DRender* p2DRender ) 
{

} 

//----------------------------------------------------------------------------------------------------------
//CWndColosseumModelessBox
//----------------------------------------------------------------------------------------------------------
CWndColosseumModelessBox::CWndColosseumModelessBox( ) : 
m_bTimeOver( FALSE ),
m_nStage( 0 ),
m_nCountDown( 0 ),
m_fEffectSound( 0.0f )
{
	m_bNoCloseButton = TRUE;
}

CWndColosseumModelessBox::~CWndColosseumModelessBox( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumModelessBox::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumModelessBox::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();

	m_kRect = GetWndRect();
	Move( m_kRect.left, 72 );
} 

BOOL CWndColosseumModelessBox::Process( void )
{
	m_fSec -= _GetDeltaTimeSec( );

	if( m_fSec < 0.0f )
	{
		if( !m_bTimeOver )
			DoTimeOver( );

		m_bTimeOver = TRUE;
		m_fSec = 0.0f;
	}

	int nTotalSec = (int)m_fSec;
	FLASSERT( nTotalSec < ( 24 * 60 * 60 ) );		//하루를 넘길수없다고 가정함.

	DWORD dwColor = 0xffffffff;
	if( nTotalSec < 11 )
	{
		if( m_nCountDown != nTotalSec )
		{
			PLAYSND( SND_COLO_COUNT );		//1초마다 플레이
			m_nCountDown = nTotalSec;
		}

		dwColor = 0xffff5555;
	}

	int nHour, nMin, nSec;
	_GetHMS_bySec( nTotalSec, nHour, nMin, nSec );

	CString str;
	str.Format( "%02d : %02d : %02d", nHour, nMin, nSec );

	//타임 표시
	CWndStatic* pWndTime = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndTime->SetTileColor( dwColor );
	pWndTime->SetTitle( str );

	if( BT_STAGE_COUNTDOWN == m_eType )
	{
		m_fEffectSound += _GetDeltaTimeSec( );
		if( m_fEffectSound > 15.0f )
		{
			PLAYSND( SND_COLO_PEOPLECHEER );	//환호성
			m_fEffectSound = 0.0f;
		}
	}
	
	return TRUE;
}

BOOL CWndColosseumModelessBox::IsPickupSpace(CPoint point)
{
	CRect rect = GetWindowRect();
	if(rect.PtInRect(point))
	{
		return TRUE;
	}
	return FALSE;
}

void CWndColosseumModelessBox::DoTimeOver( )
{
	switch( m_eType )
	{
	case BT_STAGE_READY:
		break;

	case BT_STAGE_COUNTDOWN:
		break;

	case BT_RETRY_READY:
		break;
	}
}

BOOL CWndColosseumModelessBox::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumModelessBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumModelessBox::OnDraw( C2DRender* p2DRender ) 
{
} 

void CWndColosseumModelessBox::SetType( BOX_TYPE eType, int nStage )
{

	m_eType = eType;
	m_nStage = nStage;

	switch( eType )
	{
	case BT_STAGE_READY:
		{
			CWndStatic* pWndSubject = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
			CString str;
			str.Format( prj.GetText(TID_COLOSSEUM_WATINGSTAGESTART01), m_nStage );		//잠시 후 %d번째 스테이지가 시작됩니다.
			pWndSubject->SetTitle( str );
		}
		break;

	case BT_STAGE_COUNTDOWN:
		{
			//m_nAlphaCount = 125;
			
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			if( pWndWorld )
			{
				PLAYSND( SND_COLO_BOSSAPPEAR );		//보스등장음
				pWndWorld->m_fEffectTime_StateStart = 1.0f;
			}
		}
		break;

	case BT_RETRY_READY:
		{
			CWndStatic* pWndSubject = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
			CString str;
			str.Format( prj.GetText( TID_COLOSSEUM_WATINGSTAGERESTART01), m_nStage );		//잠시 후 %d번째 스테이지에 재 도전 합니다.
			pWndSubject->SetTitle( str );
		}
		break;

	default : 
		break;
	}
}

void CWndColosseumModelessBox::SetTimer( float fSec )
{
	FLASSERT( fSec >= 0.0f );
	m_fSec = fSec;
}

//----------------------------------------------------------------------------------------------------------
//CWndColosseumModalBox
//----------------------------------------------------------------------------------------------------------
CWndColosseumModalBox::CWndColosseumModalBox( )
{
	m_bNoCloseButton = TRUE;
}

CWndColosseumModalBox::~CWndColosseumModalBox( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumModalBox::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOMASSAGE, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumModalBox::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndText * pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetTitle( CString("") );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndColosseumModalBox::Process( void )
{
	return TRUE;
} 

BOOL CWndColosseumModalBox::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumModalBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1:
		DoAction_Type1( );
		break;
	case WIDC_BUTTON2:
		DoAction_Type2( );
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumModalBox::DoAction_Type1()
{
	//2011_01_12 : 비행중처리
	if( g_pPlayer->IsFly() )
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_COLOSSEUM_NOTENTER_FLYING ) );
		return;
	}

	switch( m_eType )
	{
	case BT_INVITE:
		{
			//콜로세움 초대 승낙
			T_PACKET_COLOSSEUM_AUTO_INVITE_ACK kData = { 0 };
			//mem_set( &kData, 0, sizeof( T_PACKET_COLOSSEUM_AUTO_INVITE_ACK ) );
			kData.bYes = TRUE;
			kData.dwWorldID = m_dwWorldID;
			g_DPlay.SendColosseumInviteRst( &kData );
			Destroy();
		}
		break;

	default:
		break;
	}
}

void CWndColosseumModalBox::DoAction_Type2()
{
	switch( m_eType )
	{
	case BT_INVITE:
		{
			//콜로세움 초대거절
			T_PACKET_COLOSSEUM_AUTO_INVITE_ACK kData = { 0 };
			//mem_set( &kData, 0, sizeof( T_PACKET_COLOSSEUM_AUTO_INVITE_ACK ) );
			kData.bYes = FALSE;
			kData.dwWorldID = m_dwWorldID;
			g_DPlay.SendColosseumInviteRst( &kData );
			Destroy( );
		}
		break;

	default:
		break;
	}
}


void CWndColosseumModalBox::OnDraw( C2DRender* p2DRender ) 
{
} 

void CWndColosseumModalBox::SetType( BOX_TYPE eType )
{
	m_eType = eType;

	CWndText * pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	switch( eType )
	{
	case BT_INVITE:
		{
			CString str;
			//콜로세움 무한대전에 극단장 %s님이 입장하여 %s님을 초대합니다. 초대에 응하시겠습니까?
			str.Format( prj.GetText( TID_COLOSSEUM_COLOISGUILDJOIN01 ), m_strInvitePlayerName.GetString(), g_pPlayer->GetName() ); 
			pWndText->SetString( str );
		}
		break;

	default: break;
	}
}

void CWndColosseumModalBox::SetDataWorldID( DWORD dwWorldID )
{
	m_dwWorldID = dwWorldID;
}

void CWndColosseumModalBox::SetInvitePlayerName( const TCHAR* szName )
{
	m_strInvitePlayerName = CString( szName );
}


//----------------------------------------------------------------------------------------------------------
//CWndColosseumCleared
//----------------------------------------------------------------------------------------------------------
CWndColosseumCleared::CWndColosseumCleared( ) : 
m_fClearTime( 0.0f ),
m_pGauge( NULL ),
m_fSec( 300.0f ),
m_fMaxSec( 300.0f )
{
	m_bNoCloseButton = TRUE;
}

CWndColosseumCleared::~CWndColosseumCleared( )
{
	m_texGauFillNormal.DeleteDeviceObjects( );
}	

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumCleared::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLOCOMPLETE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumCleared::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndColoGageBar01.tga" ), 0xffff00ff, TRUE );
	
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pWndStatic->SetTitle( CString("Not supported yet") );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndColosseumCleared::Process( void )
{
	if( m_fSec > 0.0f )
	{
		m_fSec -= _GetDeltaTimeSec( );

		if( m_fSec < 0.0f )
			m_fSec = 0.0f;
	}
	return TRUE;
} 

BOOL CWndColosseumCleared::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumCleared::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1:	//나가기
		g_DPlay.SendColosseumOut( );
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumCleared::OnDraw( C2DRender* p2DRender ) 
{
	//gauge
	MakeGaugeRect( );
	m_pTheme->RenderGauge( p2DRender, &m_rectGauge, 0xffffffff, m_pGauge, &m_texGauFillNormal );
} 

void CWndColosseumCleared::MakeGaugeRect( )
{
	if( m_fMaxSec <= 0.0f )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_fMaxSec %f" ), m_fSec );
		return;
	}

	LPWNDCTRL lpEXP   = GetWndCtrl( WIDC_STATIC4 );
	m_rectGauge = lpEXP->rect;

	int nWidthClient = lpEXP->rect.Width();
	float fRatio = m_fSec / m_fMaxSec;		//백분율( 0 ~ 1 )
	int nOffsetRight = (int)( nWidthClient * fRatio ) ;

	m_rectGauge.right = m_rectGauge.left + nOffsetRight;
}
void CWndColosseumCleared::SetData( std::vector< FLRewardItem >& vData )
{
	m_kDatas = vData;
}

void CWndColosseumCleared::SetClearTime( const float fSeconds )
{
	m_fClearTime = fSeconds;

	int nTotalSec = (int)m_fClearTime;
	FLASSERT( nTotalSec < ( 24 * 60 * 60 ) );		//하루를 넘길수없다고 가정함.

	int nHour, nMin, nSec;
	_GetHMS_bySec( nTotalSec, nHour, nMin, nSec );

	CString str;
	str.Format( "%02d : %02d : %02d", nHour, nMin, nSec );

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	pWndStatic->SetTitle( str );
}

void CWndColosseumCleared::SetTimer( const float fSec )
{
	float fReSec = fSec;
	if( fReSec <= 0 )
	{
		fReSec = 10.0f;
		FLERROR_LOG( PROGRAM_NAME, _T( "fReSec %f" ), fReSec );
	}

	m_fMaxSec = fReSec;
	m_fSec = fReSec;
}

void CWndColosseumCleared::SetRank( const bool bVisible, const int nRank )
{
	CString str( _T("") );
	str.Format( _T("%d"), nRank );

	if( bVisible )
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );

		if( pWndStatic )
			pWndStatic->SetTitle( str );
	}
	else
	{
		CWndStatic* pWndRankNumber = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );

		if( pWndRankNumber )
			pWndRankNumber->SetVisible( FALSE );

		CWndStatic* pWndRankText = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );

		if( pWndRankText )
			pWndRankText->SetVisible( FALSE );
	}
}


HRESULT CWndColosseumCleared::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pGauge, NULL );
	}
	
	return S_OK;
}
HRESULT CWndColosseumCleared::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGauge );

	return S_OK;//return S_OK;
}
HRESULT CWndColosseumCleared::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

//----------------------------------------------------------------------------------------------------------
//CWndColosseumRankingTab
//----------------------------------------------------------------------------------------------------------
CWndColosseumRankingTab::CWndColosseumRankingTab( ) 
{
	m_bNoCloseButton = TRUE;
}

CWndColosseumRankingTab::~CWndColosseumRankingTab( )
{
}	

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumRankingTab::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COLO_RANKTAB, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumRankingTab::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CWndStatic* pWndStatic2 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	if( pWndStatic && pWndStatic2 )
	{
		pWndStatic->SetTitle( prj.GetText( TID_COLOSSEUM_HIGHCOLORANKING01 ) );		//콜로세움
		pWndStatic2->SetTitle( prj.GetText( TID_COLOSSEUM_LOWCOLORANKING01 )  );	//저레벨 콜로세움
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndColosseumRankingTab::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumRankingTab::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1:	//닫기
		Destroy( );
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumRankingTab::OnDraw( C2DRender* p2DRender ) 
{
} 

void CWndColosseumRankingTab::AddListData( CWndListBox* pList, int index, DWORD dwTick, const char* szGuild, DWORD style )
{
	char buffer[20] = { 0 };
	int nH, nM, nS;
	CString strRst, strTemp;

	if( index > 0 )
		FLIntToString( index, buffer, _countof( buffer ), 10 );

	if( dwTick > 0 )
	{
		_GetHMS_bySec( dwTick / 1000, nH, nM, nS );
		strTemp.Format( "%02d : %02d : %02d", nH, nM, nS );
		
		strRst.Format( "%-20s %-20s %-30s", buffer, strTemp.GetString(), szGuild );
	}
	else
	{
		strRst.Format( "%-20s %-20s", buffer, "--" );
	}

	pList->AddString( strRst.GetString(), m_dwColor, style );
}

void CWndColosseumRankingTab::SetData( const T_COLOSSEUM_RANKING& normal, const T_COLOSSEUM_RANKING& hard )
{
	CWndListBox* pWndList_hard = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	CWndListBox* pWndList_normal = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	if( !pWndList_hard || !pWndList_normal )
		return;

	pWndList_hard->ResetContent( );
	pWndList_normal->ResetContent( );

	//고레벨 콜로세움 1~5위
	for( int i = 0; i < T_COLOSSEUM_RANKING::RANKING_MAX; ++i )
	{
		AddListData( pWndList_hard, (i+1), hard.dwArrClearTick[i], hard.szArrGuild[i] );
		AddListData( pWndList_normal, (i+1), normal.dwArrClearTick[i], normal.szArrGuild[i] );
	}

	// 본인기록
	AddListData( pWndList_hard, -1, hard.dwClearTick, hard.szMyGuild, ESSTY_BOLD );
	AddListData( pWndList_normal, -1, normal.dwClearTick, normal.szMyGuild, ESSTY_BOLD );
}

//----------------------------------------------------------------------------------------------------------
//CWndColosseumRanking
//----------------------------------------------------------------------------------------------------------
//CWndColosseumRanking::CWndColosseumRanking( ) 
//{
//	m_bNoCloseButton = TRUE;
//	mem_set( m_kData, 0, sizeof( T_COLOSSEUM_RANKING_PACK ) * 2 );
//}
//
//CWndColosseumRanking::~CWndColosseumRanking( )
//{
//}	
//
//// 처음 이 함수를 부르면 윈도가 열린다.
//BOOL CWndColosseumRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
//{ 
//	// Daisy에서 설정한 리소스로 윈도를 연다.
//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COLORANKLIST, 0, CPoint( 0, 0 ), pWndParent );
//} 
//
//void CWndColosseumRanking::OnInitialUpdate() 
//{ 
//	CWndNeuz::OnInitialUpdate(); 
//	// 여기에 코딩하세요
//
//	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
//	CRect rect = GetClientRect();
//	rect.left = 5;
//	rect.top = 0;
//	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
//	
//	m_WndTab.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COLO_RANKTAB );
//
//	WTCITEM tabTabItem;
//	
//	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
//	tabTabItem.pszText = prj.GetText( TID_COLOSSEUM_DAYTIMERANKING01 ); //"주간ID필요";
//	tabTabItem.pWndBase = &m_WndTab;
//	pWndTabCtrl->InsertItem( 0, &tabTabItem );
//
//	tabTabItem.pszText = prj.GetText( TID_COLOSSEUM_MONTHLYRANKING01 ); //"월간ID필요";
//	tabTabItem.pWndBase = &m_WndTab;
//	pWndTabCtrl->InsertItem( 1, &tabTabItem );
//	
//	tabTabItem.pszText = prj.GetText( TID_COLOSSEUM_HALLOFFAME01 ); //"명예의전당ID필요";
//	tabTabItem.pWndBase = &m_WndTab;
//	pWndTabCtrl->InsertItem( 2, &tabTabItem );
//	
//	////Test
////	T_COLOSSEUM_RANKING data;
////	for( int i = 0; i < T_COLOSSEUM_RANKING::RANKING_MAX; ++i )
////	{
////		FLStrcpy( data.szArrGuild[i], _countof( data.szArrGuild[i] ), "guildname" );
////		data.dwArrClearTick[i] = 0;
////	}
//
////	FLStrcpy( data.szMyGuild, _countof( data.szMyGuild ), "myguildname" );
////	data.dwClearTick = 1;
//
////	m_WndTab.SetData( data, data );
//	
//	pWndTabCtrl->SetCurSel( 0 );
//	// 윈도를 중앙으로 옮기는 부분.
//	MoveParentCenter();
//} 
//
//BOOL CWndColosseumRanking::Process( void )
//{
//	return TRUE;
//} 
//
//BOOL CWndColosseumRanking::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
//{
//	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
//}
//
//BOOL CWndColosseumRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
//{
//	switch( nID )
//	{
//	case WIDC_BUTTON1:	//닫기
//		Destroy( );
//		break;
//
//	case WIDC_TABCTRL1:
//		if( WNM_SELCHANGE == message )
//		{
//			CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
//			int sel = pWndTabCtrl->GetCurSel();
//			if( 0 == sel )
//				m_WndTab.SetData( m_kData[0].WeekRanking, m_kData[1].WeekRanking );
//			else if( 1 == sel )
//				m_WndTab.SetData( m_kData[0].MonthRanking, m_kData[1].MonthRanking );
//			else if( 2 == sel )
//				m_WndTab.SetData( m_kData[0].TotalRanking, m_kData[1].TotalRanking );
//			else
//			{
//				FLASSERT( 0 );
//			}
//	
//		}
//		break;
//
//	default: break;
//	}
//
//	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
//} 
//
//void CWndColosseumRanking::OnDraw( C2DRender* p2DRender ) 
//{
//} 
//
//void CWndColosseumRanking::SetData( const T_COLOSSEUM_RANKING_PACK* pDatas )
//{
//	m_kData[0] = pDatas[0];	
//	m_kData[1] = pDatas[1];
//
//	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
//	pWndTabCtrl->SetCurSel( 0 );
//
//	m_WndTab.SetData( m_kData[0].WeekRanking, m_kData[1].WeekRanking );
//	
//}


//----------------------------------------------------------------------------------------------------------
//CWndColosseumRanking
//----------------------------------------------------------------------------------------------------------
CWndColosseumRanking::CWndColosseumRanking( ) :
m_dwSelectedID_level1( WIDC_BUTTON5 ),
m_dwSelectedID_level2( WIDC_BUTTON2 )
{
	m_bNoCloseButton = TRUE;
	memset( m_kData, 0, sizeof( T_COLOSSEUM_RANKING_PACK ) * 2 );
}

CWndColosseumRanking::~CWndColosseumRanking( )
{
}	

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndColosseumRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_NEWCOLORANKNOMAL, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndColosseumRanking::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndColosseumRanking::Process( void )
{
	return TRUE;
} 

BOOL CWndColosseumRanking::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndColosseumRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BUTTON1:	//닫기
		Destroy( );
		break;

	case WIDC_BUTTON2:	//주간
	case WIDC_BUTTON3:	//월간
	case WIDC_BUTTON4:	//명예의 전당
		SetCheckRadio_level2( nID );
		UpdateData( );
		break;

	case WIDC_BUTTON5:
	case WIDC_BUTTON6:
		SetCheckRadio_level1( nID );
		UpdateData( );
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndColosseumRanking::OnDraw( C2DRender* p2DRender ) 
{
} 

void CWndColosseumRanking::SetData( const T_COLOSSEUM_RANKING_PACK* pDatas )
{
	m_kData[0] = pDatas[0];	
	m_kData[1] = pDatas[1];

	SetCheckRadio_level1( WIDC_BUTTON5 ); //저레벨
	SetCheckRadio_level2( WIDC_BUTTON2 ); //주간
	UpdateData( );
}

void CWndColosseumRanking::ResetItem( const DWORD wndID )
{
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( wndID );
	if( pStatic )
	{
		pStatic->SetTitle( "" );
	}
}

void CWndColosseumRanking::ResetItemAll( )
{
	ResetItem( WIDC_STATIC2 );
	ResetItem( WIDC_STATIC3 );
	ResetItem( WIDC_STATIC4 );
	ResetItem( WIDC_STATIC5 );
	ResetItem( WIDC_STATIC6 );
	
	ResetItem( WIDC_STATIC7 );
	ResetItem( WIDC_STATIC8 );
	ResetItem( WIDC_STATIC9 );
	ResetItem( WIDC_STATIC10 );
	ResetItem( WIDC_STATIC11 );

	ResetItem( WIDC_STATIC12 );
	ResetItem( WIDC_STATIC13 );
	ResetItem( WIDC_STATIC14 );
	ResetItem( WIDC_STATIC15 );
	ResetItem( WIDC_STATIC16 );
}

void CWndColosseumRanking::UpdateData( )
{
	//컨트럴 ID가 불규칙하므로 for문을 지양하므로 RANKING_MAX가 변하면 아니된다.
	if( 5 != T_COLOSSEUM_RANKING::RANKING_MAX )
	{
		FLASSERT( 0 );
		FLERROR_LOG( PROGRAM_NAME, _T( "angrySUN: T_COLOSSEUM_RANKING::RANKING_MAX is not 5" ) );
		return;
	}

	ResetItemAll( );

	T_COLOSSEUM_RANKING_PACK* pUpdateData = NULL;
	T_COLOSSEUM_RANKING kData;

	if( WIDC_BUTTON5 == m_dwSelectedID_level1 )
		pUpdateData = &m_kData[0];
	else if( WIDC_BUTTON6 == m_dwSelectedID_level1 )
		pUpdateData = &m_kData[1];

	if( !pUpdateData )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndColosseumRanking::UpdateData m_dwSelectedID_level1 %d" ), m_dwSelectedID_level1 );
		return;
	}
	
	if( WIDC_BUTTON2 == m_dwSelectedID_level2 )
		kData = pUpdateData->WeekRanking;
	if( WIDC_BUTTON3 == m_dwSelectedID_level2 )
		kData = pUpdateData->MonthRanking;
	if( WIDC_BUTTON4 == m_dwSelectedID_level2 )
		kData = pUpdateData->TotalRanking;

	CWndStatic* pStatic = NULL;

	if( kData.dwArrClearTick[0] > 0 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );		//seq
		pStatic->SetTitle( "1st" );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );		//guild names
		pStatic->SetTitle( kData.szArrGuild[0] );
		UpdateData_Records( WIDC_STATIC12, kData.dwArrClearTick[0] );	//records
	}

	if( kData.dwArrClearTick[1] > 0 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );		//seq
		pStatic->SetTitle( "2nd" );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC8 );		//guild names
		pStatic->SetTitle( kData.szArrGuild[1] );
		UpdateData_Records( WIDC_STATIC13, kData.dwArrClearTick[1] );	//records
	}

	if( kData.dwArrClearTick[2] > 0 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );		//seq
		pStatic->SetTitle( "3rd" );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );		//guild names
		pStatic->SetTitle( kData.szArrGuild[2] );
		UpdateData_Records( WIDC_STATIC14, kData.dwArrClearTick[2] );	//records
	}

	if( kData.dwArrClearTick[3] > 0 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );		//seq
		pStatic->SetTitle( "4th" );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC10 );		//guild names
		pStatic->SetTitle( kData.szArrGuild[3] );
		UpdateData_Records( WIDC_STATIC15, kData.dwArrClearTick[3] );	//records
	}

	if( kData.dwArrClearTick[4] > 0 )
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );		//seq
		pStatic->SetTitle( "5th" );
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );		//guild names
		pStatic->SetTitle( kData.szArrGuild[4] );
		UpdateData_Records( WIDC_STATIC16, kData.dwArrClearTick[4] );	//records
	}
}

void CWndColosseumRanking::UpdateData_Records( const DWORD wndID, const DWORD tick )
{
	CString strTemp;
	int nH, nM, nS;
	_GetHMS_bySec( tick / 1000, nH, nM, nS );
	strTemp.Format( "%02d:%02d:%02d", nH, nM, nS );
	
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( wndID );	
	if( pStatic )
		pStatic->SetTitle( strTemp );
}

void CWndColosseumRanking::SetCheckRadio_level1( const DWORD child_id )
{
	CWndButton* pButtonTab = NULL;
	pButtonTab = (CWndButton*)GetDlgItem( WIDC_BUTTON5 );		//저렙
	pButtonTab->SetCheck( 0 );
	pButtonTab = (CWndButton*)GetDlgItem( WIDC_BUTTON6 );		//고렙
	pButtonTab->SetCheck( 0 );

	CWndButton* pButton = (CWndButton*)GetDlgItem( child_id );
	if( pButton )
	{
		pButton->SetCheck( 2 );
		m_dwSelectedID_level1 = child_id;
	}
}

void CWndColosseumRanking::SetCheckRadio_level2( const DWORD child_id )
{
	CWndButton* pButtonTab = NULL;
	pButtonTab = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );		//주간
	pButtonTab->SetCheck( 0 );
	pButtonTab = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );		//월간
	pButtonTab->SetCheck( 0 );
	pButtonTab = (CWndButton*)GetDlgItem( WIDC_BUTTON4 );		//명예의전당
	pButtonTab->SetCheck( 0 );

	CWndButton* pButton = (CWndButton*)GetDlgItem( child_id );
	if( pButton )
	{
		pButton->SetCheck( 2 );
		m_dwSelectedID_level2 = child_id;
	}
}


//----------------------------------------------------------------------------------------------------------------------------------
// EVENT ARENA
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaCharInfo
CWndEArenaCharInfo::CWndEArenaCharInfo( ):
m_pGauge( NULL ),
m_bSelected( FALSE )
{
	m_bNoCloseButton = TRUE;
	m_kData.dwObjID = NULL_ID;
}

CWndEArenaCharInfo::~CWndEArenaCharInfo( )
{
//	DeleteDeviceObjects( );
	m_texGauge.DeleteDeviceObjects();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaCharInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENAMEMBER, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaCharInfo::OnInitialUpdate() 
{ 

	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_texGauge.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndHPBar.tga" ), 0xffff00ff, TRUE );
//	m_texGauge.LoadTexture( m_pApp->m_pd3dDevice, 
//		MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndColoGageBar01.tga" ), 0xffff00ff, TRUE );

	// 윈도를 중앙으로 옮기는 부분.
	//MoveParentCenter();

	DelWndStyle( WBS_MOVE );

	//init
	m_kData.nCurrentHP = 60;
	m_kData.nMaxHP = 100;
} 

BOOL CWndEArenaCharInfo::Process( void )
{
	return TRUE;
}

BOOL CWndEArenaCharInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndEArenaCharInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaCharInfo::OnLButtonDown( UINT nFlags, CPoint point )
{
}

void CWndEArenaCharInfo::OnLButtonUp( UINT nFlags, CPoint point )
{
	//m_pParentWnd->OnCommand( APP_EVENTARENAMEMBER, WM_LBUTTONUP, this );

	if( g_WndMng.m_pWndEArenaTeam )
		g_WndMng.m_pWndEArenaTeam->SelectedUser( m_kData.dwObjID );
}

void CWndEArenaCharInfo::OnDraw( C2DRender* p2DRender ) 
{
	DWORD dwNameColor = 0xfffffe8b;	//파티원

	if( m_kData.nState == EVENT_ARENA_TEAM_MEMBER_STATE_LOGOUT )
	{
		//로긴중이 아니다.
		//일단 회색처리
	//	CRect rect = GetWindowRect();
	//	p2DRender->RenderFillRect( rect, 0xff808080 );

		dwNameColor = 0xff999999;	//로그아웃
	}
	else
	{
		//본인일경우
		if( g_pPlayer && g_pPlayer->GetId() == m_kData.dwObjID )
			dwNameColor = 0xffffffff;	//본인

		//gauge
		MakeGaugeRect();

		if( m_pGauge )
			m_pTheme->RenderGauge( p2DRender, &m_rectGauge, 0xffffffff, m_pGauge, &m_texGauge );

		//선택된 대상
		if( m_bSelected )
		{
			CRect rect	= GetWindowRect();
			p2DRender->RenderRect( rect, 0xffff0000 );
		}
	}

	CWndStatic* pWndName = ( CWndStatic* )GetDlgItem( WIDC_ST_MEMBER1 );
	if( pWndName )
		pWndName->SetTileColor( dwNameColor );
}

void CWndEArenaCharInfo::MakeGaugeRect( )
{
	LPWNDCTRL lpEXP = GetWndCtrl( WIDC_STATIC2 );
	m_rectGauge = lpEXP->rect;

	int nWidthClient = lpEXP->rect.Width();

	if( m_kData.nMaxHP < 1 )
	{
		FLERROR_LOG(  PROGRAM_NAME, "MakeGaugeRect maxHP %d", m_kData.nMaxHP );
		m_kData.nMaxHP = 100;
		//return;		//오류 넘김
	}

	float fRatio = (float)m_kData.nCurrentHP / (float)m_kData.nMaxHP;		//백분율( 0 ~ 1 )
	int nOffsetRight = (int)( nWidthClient * fRatio ) ;
	
	m_rectGauge.right = m_rectGauge.left + nOffsetRight;
}

void CWndEArenaCharInfo::SetData( const T_EVENT_ARENA_TEAM_MEMBER_INFO& kData )
{
	m_kData = kData;

	CWndStatic* pWndName = ( CWndStatic* )GetDlgItem( WIDC_ST_MEMBER1 );
	if( pWndName )
		pWndName->SetTitle( kData.szCharName );
}


HRESULT CWndEArenaCharInfo::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pGauge, NULL );
	}

	FLASSERT( m_pGauge );
	
	return S_OK;
}
HRESULT CWndEArenaCharInfo::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGauge );

	return S_OK;//return S_OK;
}
HRESULT CWndEArenaCharInfo::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaGroup
CWndEArenaGroup::CWndEArenaGroup( ):
m_byPartyID( 0 ),
m_bToggleVisible( TRUE )
{
	m_bNoCloseButton = TRUE;
}

CWndEArenaGroup::~CWndEArenaGroup( )
{
	SeqDeleter< WndEACharInfo_Container > ( ) ( m_vecMembers );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaGroup::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENAPARTY, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaGroup::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	//MoveParentCenter();

	SetTitle( "GROUP NAME" );

	CWndStatic* pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_STATIC1 );
	if( pWndStatic )
		pWndStatic->AddWndStyle( WSS_NOPICKUP );
} 

BOOL CWndEArenaGroup::Process( void )
{
	return TRUE;
}

BOOL CWndEArenaGroup::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( APP_EVENTARENAMEMBER == nID && dwMessage == WM_LBUTTONUP )
	{
		for( WndEACharInfo_Iter iter = m_vecMembers.begin(); iter != m_vecMembers.end(); ++iter )
		{
			CWndEArenaCharInfo* pMember = *iter;
			if( pMember == pWndBase )
				pMember->m_bSelected = TRUE;
			else pMember->m_bSelected = FALSE;
		}
	}

	// 자식 콘트롤이 알려줘야 드래그효과를 줄수있는데.. 이방법 말고 더 좋은방법은?
	//if( WIDC_STATIC1 == nID && WM_LBUTTONDOWN == dwMessage )
	//{
	//	if( ( m_dwStyle & WBS_MOVE ) )
	//	{
	//		m_bPickup = TRUE;
	//		SetCapture();
	//		m_pointOld = GetMousePoint();

	//		//m_pointOld
	//	}
	//}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndEArenaGroup::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch( nID )
	{
	case WIDC_BT_MINIMUM:
		{
			CWndButton* pButton = ( CWndButton* )GetDlgItem( WIDC_BT_MINIMUM );
			if( pButton )
			{
				m_bToggleVisible = !m_bToggleVisible;
				m_bToggleVisible ? pButton->SetCheck( 0 ) : pButton->SetCheck( 2 );

				SetVisibleMember( m_bToggleVisible );
			}
		}
		break;

	default: 
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaGroup::OnDraw( C2DRender* p2DRender ) 
{

}

void CWndEArenaGroup::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

	AutoPositon( );
}

void CWndEArenaGroup::SetTitle( const char* pszTitle )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	if( pWndStatic )
		pWndStatic->SetTitle( CString( pszTitle ) );
}

void CWndEArenaGroup::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndEArenaGroup::IsPickupSpace(CPoint point)
{
	CRect rect = GetWindowRect();
	if(rect.PtInRect(point))
	{
		//static만 pickup에서 제거
		CWndBase* pWnd = NULL;
		for(int i = 0; i < m_wndArray.GetSize(); i++)
		{
			pWnd = (CWndBase*)m_wndArray[i];
			if( pWnd )
			{
				//ok static 이면 캡춰!
				if( WTYPE_STATIC == pWnd->GetWndType( ) && pWnd->m_rectWindow.PtInRect( point ) )
					return TRUE;
			}
		}
		
		return FALSE;
	}
	return FALSE;
}

void CWndEArenaGroup::Reset( )
{
	SeqDeleter< WndEACharInfo_Container > ( ) ( m_vecMembers );
}

void CWndEArenaGroup::InsertMember( const T_EVENT_ARENA_TEAM_MEMBER_INFO& kMember )
{
	//멤버가 이미 있다면 업데이트만 
	CWndEArenaCharInfo* pWndChar = FindWndChar( kMember.szCharName );
	if( pWndChar )
		pWndChar->SetData( kMember );
	else
	{
		pWndChar = new CWndEArenaCharInfo;
		pWndChar->Initialize( this );
		pWndChar->SetData( kMember );
		m_vecMembers.push_back( pWndChar );
	}

	AutoPositon( );
}

void CWndEArenaGroup::AutoPositon( )
{
	if( m_vecMembers.empty() )
		return;

	int nHeight = m_vecMembers[0]->m_rectWindow.Height();

	int nIndex = 0;
	for( WndEACharInfo_Iter iter = m_vecMembers.begin(); iter != m_vecMembers.end(); ++iter )
	{
		CWndEArenaCharInfo* pMember = *iter;
		pMember->Move( m_rectWindow.left, m_rectWindow.bottom + nIndex * nHeight );

		++nIndex;
	}
}

void CWndEArenaGroup::SetData( const T_EVENT_ARENA_TEAM_INFO& kData )
{
//	m_kData = kData;

	//set group name
	//SetTitle( kData.szName );
}

CWndEArenaCharInfo* CWndEArenaGroup::FindWndChar( const char* szName )
{
	for( WndEACharInfo_Iter iter = m_vecMembers.begin(); iter != m_vecMembers.end(); ++iter )
	{
		CWndEArenaCharInfo* pMember = *iter;
		if( strcmp( pMember->m_kData.szCharName, szName ) == 0 )
			return pMember;
	}

	return NULL;
}

void CWndEArenaGroup::SetSelctedUser( const DWORD dwSelectedID )
{
	for( WndEACharInfo_Iter iter = m_vecMembers.begin(); iter != m_vecMembers.end(); ++iter )
	{
		CWndEArenaCharInfo* pMember = *iter;
		if( pMember->m_kData.dwObjID != 0 && pMember->m_kData.dwObjID == dwSelectedID )
			pMember->m_bSelected = TRUE;
		else 
			pMember->m_bSelected = FALSE;
			
	}
}

void CWndEArenaGroup::SetVisibleMember( BOOL bHide )
{
	for( WndEACharInfo_Iter iter = m_vecMembers.begin(); iter != m_vecMembers.end(); ++iter )
	{
		CWndEArenaCharInfo* pMember = *iter;
		pMember->SetVisible( bHide );
	}
}

void CWndEArenaGroup::SetPartyID( const BYTE byPartyID )
{
	m_byPartyID = byPartyID;
	LPCTSTR pszName = prj.GetText( TID_MMI_EVENTARENA_PARTYNUMBER );
	const char* pszFind	= ::strstr( pszName, "%d" );
	if( NULL == pszFind )
	{
		FLERROR_LOG( PROGRAM_NAME, "이벤트아레나 그룹이름 문자열 오류 _%d", TID_MMI_EVENTARENA_PARTYNUMBER );
	}

	CString strName;
	strName.Format( pszName, byPartyID );

	SetTitle( strName.GetString() );
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaTeam
CWndEArenaTeam::CWndEArenaTeam( ) :
m_bLock( FALSE ),
m_eMode( EAM_USER )
{
	m_bNoCloseButton = TRUE;
}

CWndEArenaTeam::~CWndEArenaTeam( )
{
	Reset( );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaTeam::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaTeam::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	m_rectForLock = m_rectWindow;
} 

//BOOL CWndEArenaTeam::Process( void )
//{
//	return CWndBase::Process( );
//}

BOOL CWndEArenaTeam::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndEArenaTeam::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaTeam::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndEArenaTeam::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

	AutoPosition( );
}

void CWndEArenaTeam::SetMode( E_ARENA_MODE eMode )
{
	m_eMode = eMode;
	switch( m_eMode )
	{
	case EAM_USER:
		SetLock( FALSE ); 
		InitPosition( );
		break;

	case EAM_MANAGER:
		SetLock( TRUE );
		break;

	default:
		FLASSERT( 0 );
		break;
	}
}

void CWndEArenaTeam::SetTitle( const char* pszTitle )
{
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	if( pStatic )
		pStatic->SetTitle( pszTitle );
}

void CWndEArenaTeam::SetLock( BOOL bLock )
{
	m_bLock = bLock;

	if( m_bLock )
	{
		//Manager UI복구, 내부 윈도우 이동잠금
		SetWndRect( m_rectForLock );
	//	SetFocus();

		for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
		{
			CWndEArenaGroup* pWndGroup = *iter;
			pWndGroup->DelWndStyle( WBS_MOVE );
		}
	}
	else
	{
		//Manager UI숨김, 내부 윈도우 이동품
		SetWndRect( CRect( 0, 0, 0, 0 ) );
	
		for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
		{
			CWndEArenaGroup* pWndGroup = *iter;
			pWndGroup->AddWndStyle( WBS_MOVE );
		}
	}
}

void CWndEArenaTeam::Reset( )
{
	SeqDeleter< WndEAGroup_Container > ( ) ( m_vecGroups );
}

int CWndEArenaTeam::FindGroupIndex( DWORD dwGroupID )
{
	int nIndex = 0;
	for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
	{
		CWndEArenaGroup* pWndGroup = *iter;
		if( pWndGroup->GetPartyID() == dwGroupID )
			return nIndex;

		++nIndex;
	}

	return -1;
}

void CWndEArenaTeam::SetData( const FLEventArenaTeamStream& kData )
{
//	Reset( );

	m_kData = kData;
	
	typedef std::vector< T_EVENT_ARENA_TEAM_MEMBER_INFO >	EA_MemberInfo_Container;
	typedef EA_MemberInfo_Container::iterator				EA_MemberInfo_Iter;

	for( EA_MemberInfo_Iter iter = m_kData.vecMembers.begin(); iter != m_kData.vecMembers.end(); ++iter )
	{
		T_EVENT_ARENA_TEAM_MEMBER_INFO& kMember = *iter;
		int nIndex = FindGroupIndex( kMember.byPartyNumber );
		if( -1 == nIndex )		//찾을수없음. 
		{
			//신규그룹
			CWndEArenaGroup* pNewGroup = new CWndEArenaGroup;
			pNewGroup->Initialize( this );
			pNewGroup->SetData( m_kData.tTeamInfo );
			pNewGroup->InsertMember( kMember );
			pNewGroup->SetPartyID( kMember.byPartyNumber );
	
			m_vecGroups.push_back( pNewGroup );
		}
		else
		{
			m_vecGroups[ nIndex ]->InsertMember( kMember );
		}
	}

	AutoPosition( );
}

void CWndEArenaTeam::AutoPosition( )
{
	if( m_vecGroups.empty() )
		return;
	
	int nIndex = 0;
	int nWidth = m_vecGroups[0]->m_rectWindow.Width();
	
	for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
	{
		CWndEArenaGroup* pWndGroup = *iter;
		pWndGroup->Move( m_rectWindow.left + nIndex * nWidth, m_rectWindow.bottom );

		++nIndex;
	}
}

void CWndEArenaTeam::InitPosition( )
{
	//초기위치를 잡아준다. ( 관리자모드에선 써봤자 안될껄? )
	int x = 50;
	int y = 100;
	CWndStatus* pWndStatus = (CWndStatus*)GetWndBase( APP_STATUS1 );
	if( pWndStatus)
	{
		CRect kRootRect = pWndStatus->GetWndRect( );
		x = kRootRect.left;
		y = kRootRect.bottom + 10;
	}

	Move( x, y );
}

void CWndEArenaTeam::SelectedUser( const DWORD dwSelectedID )
{
	CMover* pSelectMover = prj.GetMover( dwSelectedID );
	if( IsValidObj(pSelectMover) )
	{
		CWorld* pWorld = pSelectMover->GetWorld();
		if( pWorld )
			pWorld->SetObjFocus( pSelectMover );			// 이놈을 타겟으로 설정함.
	}
	else 
	{
		// 유효하지 않다면 선택중지
		return; 
	}

	for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
	{
		CWndEArenaGroup* pWndGroup = *iter;
		if( pWndGroup )
			pWndGroup->SetSelctedUser( dwSelectedID );
	}
}

void CWndEArenaTeam::DoSelectedUser( const DWORD dwSelectedID )
{
	for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
	{
		CWndEArenaGroup* pWndGroup = *iter;
		if( pWndGroup )
			pWndGroup->SetSelctedUser( dwSelectedID );
	}
}

void CWndEArenaTeam::UpdateData( const T_EVENT_ARENA_TEAM_MEMBER_INFO& kData )
{
	for( WndEAGroup_iter iter = m_vecGroups.begin(); iter != m_vecGroups.end(); ++iter )
	{
		CWndEArenaGroup* pWndGroup = *iter;
		CWndEArenaCharInfo* pWndChar = pWndGroup->FindWndChar( kData.szCharName );
		if( pWndChar )
		{
			pWndChar->SetData( kData );
			return;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaTeamMng
CWndEArenaTeamMng::CWndEArenaTeamMng( )
{
	m_bNoCloseButton = TRUE;
}

CWndEArenaTeamMng::~CWndEArenaTeamMng( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaTeamMng::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_MEMBERADMIN, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaTeamMng::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.

	MoveParentCenter();

	//관리창 옆에 붙여준다.
	if( g_WndMng.m_pWndEArenaReferee )
	{
		int nTop = g_WndMng.m_pWndEArenaReferee->m_rectWindow.top;
		int nLeft = g_WndMng.m_pWndEArenaReferee->m_rectWindow.right;
		nLeft += 5;

		Move( nLeft, nTop );
	}

	m_kTeam[EAT_RED].Initialize( this, APP_EVENTARENA_REDMEMADMIN );	
	m_kTeam[EAT_RED].SetMode( CWndEArenaTeam::EAM_MANAGER );
	m_kTeam[EAT_RED].SetTitle( prj.GetText( TID_MMI_EVENTARENA_REDTEAM ) );		//레드
	m_kTeam[EAT_RED].Move( m_rectWindow.left,  m_rectWindow.bottom );
	m_kTeam[EAT_RED].DelWndStyle( WBS_MOVE );
	
	m_kTeam[EAT_BLUE].Initialize( this, APP_EVENTARENA_BLUEMEMADMIN);	
	m_kTeam[EAT_BLUE].SetMode( CWndEArenaTeam::EAM_MANAGER );
	m_kTeam[EAT_BLUE].SetTitle( prj.GetText( TID_MMI_EVENTARENA_BLUETEAM ) );	//블루
	m_kTeam[EAT_BLUE].Move( m_rectWindow.left + m_kTeam[EAT_RED].m_rectWindow.Width(), m_rectWindow.bottom );
	m_kTeam[EAT_BLUE].DelWndStyle( WBS_MOVE );

	CWndStatic* pTitle = ( CWndStatic* )GetDlgItem( WIDC_STATIC1 );
	if( pTitle )
		pTitle->SetTitle( "ADMINISTRATOR" );
} 

//BOOL CWndEArenaTeam::Process( void )
//{
//	return CWndBase::Process( );
//}

BOOL CWndEArenaTeamMng::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	// 자식 콘트롤이 알려줘야 드래그효과를 줄수있는데.. 이방법 말고 더 좋은방법은?
	//if( typeid( CWndStatic ) == typeid( *pWndBase ) && WM_LBUTTONDOWN == dwMessage )
	if( WIDC_STATIC1 == nID && WM_LBUTTONDOWN == dwMessage )
	{
		if( m_dwStyle & WBS_MOVE )
		{
			m_bPickup = TRUE;
			SetCapture();
			m_pointOld = GetMousePoint();

			//m_pointOld
		}
	}
    
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndEArenaTeamMng::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaTeamMng::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndEArenaTeamMng::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
	m_kTeam[EAT_RED].Move( m_rectWindow.left, m_rectWindow.bottom );
	m_kTeam[EAT_BLUE].Move( m_rectWindow.left + m_kTeam[EAT_RED].m_rectWindow.Width(), m_rectWindow.bottom );
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaReferee
CWndEArenaReferee::CWndEArenaReferee( ):
m_bStarted( FALSE ),
m_pWndStopBattle( NULL ),
m_pWndMessageBox( NULL )
{
	m_fBattleTime = 60.0f * 5.0f;	//test , server로부터 받은값이 될것이다.
	m_bNoCloseButton = TRUE;
}

CWndEArenaReferee::~CWndEArenaReferee( )
{
	SAFE_DELETE( m_pWndStopBattle );
	SAFE_DELETE( m_pWndMessageBox );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaReferee::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_ADMIN, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaReferee::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.

	MoveParentCenter();

	Move( 10, m_rectWindow.top );

	CWndStatic* pTimeText = ( CWndStatic* )GetDlgItem( WIDC_ST_FIGHTTIME );
	if( pTimeText )
		pTimeText->SetFont( CWndBase::m_Theme.m_pFontGameTitle );

	CWndButton* pWndButton = NULL;
	CWndComboBox* pBox = NULL;

	//경기 타입
	pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
	if( pBox )
	{
		pBox->AddWndStyle( EBS_READONLY );
	}

	//팀 콤보박스 비활성
	pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_REDLIST );
	if( pBox )
	{
		pBox->EnableWindow( FALSE );
		pBox->AddWndStyle( EBS_READONLY );
	}

	pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_BLUELIST );
	if( pBox )
	{
		pBox->EnableWindow( FALSE );
		pBox->AddWndStyle( EBS_READONLY );
	}


	//팀 체크박스 비활성
	pWndButton = ( CWndButton* )GetDlgItem( WIDC_CH_REDCHECK );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );

	pWndButton = ( CWndButton* )GetDlgItem( WIDC_CH_BLUECHECK );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );
	

	//선수소환 버튼 비활성
	pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_MEMBERCALL );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );

	//선수입장 비활성
	pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_MEMBERENTER );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );

	//전투시작 비활성
	pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_FIGHT );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );

	//경기중단 버튼 비활성
	pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_FIGHTSTOP );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );

	//경기종료 버튼 비활성
	pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_FIGHTEND );
	if( pWndButton )
		pWndButton->EnableWindow( FALSE );
} 

BOOL CWndEArenaReferee::Process( void )
{
	if( m_bStarted && m_fBattleTime > 0.0f )
		m_fBattleTime -= _GetDeltaTimeSec( );

	if( m_fBattleTime < 0.0f )
		m_fBattleTime = 0.0f;

	int nMin = (int)( m_fBattleTime / 60.0f );
	int nSec = (int)m_fBattleTime - ( nMin * 60 );

	CString strTime;
	strTime.Format( "%02d : %02d", nMin, nSec );
	CWndStatic* pTimeText = ( CWndStatic* )GetDlgItem( WIDC_ST_FIGHTTIME );
	if( pTimeText )
	{
		DWORD dwColor = ( m_bStarted ? 0xffff0000 : 0xffffffff );
		pTimeText->SetTileColor( dwColor );
		pTimeText->SetTitle( strTime );
	}

	if( m_pWndMessageBox )
		m_pWndMessageBox->Process( );
	
	return TRUE;
}

BOOL CWndEArenaReferee::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( APP_EVENTARENA_ENTER == nID && pWndBase )
	{
		if( WM_CLOSE == dwMessage )
		{
			//입장진행중인 윈도만 클라에서 삭제후 전투시작활성
			if( m_pWndMessageBox->GetType() == CWndEArenaAdminMessage::MSG_DOING_ENTRANCE )	
			{
				SAFE_DELETE( m_pWndMessageBox );
			}
			else if( m_pWndMessageBox->GetType() == CWndEArenaAdminMessage::MSG_DOING_SUMMONS )	
			{
				CompletedSummonsPlayer();
			}
		}
	}
	else if( APP_EVENTARENA_STOP == nID )
	{
		if( WIDC_BT_CANCEL == dwMessage )
		{
			//경기 중단 윈도에서 취소를 보내옴 -> 경기중단 버튼 활성화
			EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );
		}
	}
   
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndEArenaReferee::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 

	switch( nID )
	{
		case WTBID_MIN:
			SetVisible( FALSE );
			g_WndMng.m_pWndWorld->GetAdvMgr()->AddAdvButton(APP_EVENTARENA_ADMIN, TRUE);
			break;

		case WIDC_CO_MATCHNAME:	//경기 타입( 8강전.... )
			{
				if( CBN_SELCHANGE == message )
				{
					CWndComboBox* pBox = NULL;

					//콤보박스 옆 " *판 *승 " 정보 출력
					pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
					if( pBox )
					{
						DWORD dwTourID = pBox->GetItemData( pBox->GetCurSel() );
						T_EVENT_ARENA_TONEMENT* pTourData = FindTournament( dwTourID );
						if( pTourData )
						{
							CWndStatic* pTourType = ( CWndStatic* )GetDlgItem( WIDC_ST_MATCH );
							if( pTourType )
							{
								//CString strBuffer;
								//strBuffer.Format( " %d 판 %d 선승제 ID필요", pTourData->nNeedWinCount * 2 + 1, pTourData->nNeedWinCount );
								pTourType->SetTitle( CString(pTourData->szDesc) );
							}

							//sendpacket
							FLPacketEventArenaManageSetTonementNoti kPacket;
							kPacket.dwTonementID = dwTourID;		// 8강, 4강?인듯
							g_DPlay.SendPacket( &kPacket );
						}
					}

					//팀콤보박스 초기화
					pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_REDLIST );
					if( pBox )
					{
						pBox->EnableWindow( TRUE );
						pBox->SetCurSel( -1 );
					}

					pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_BLUELIST );
					if( pBox )
					{
						pBox->EnableWindow( TRUE );
						pBox->SetCurSel( -1 );
					}

				}
				
			}
			break;

		case WIDC_CO_REDLIST:	//레드팀 팀선택 콤보
			{
				if( CBN_SELCHANGE == message )
				{
					CWndButton* pWndCheck = ( CWndButton* )GetDlgItem( WIDC_CH_REDCHECK );
					if( pWndCheck )
						pWndCheck->EnableWindow( TRUE );
				}
			}
			break;

		case WIDC_CO_BLUELIST:	//블루팀 팀선택 콤보
			{
				if( CBN_SELCHANGE == message )
				{
					CWndButton* pWndCheck = ( CWndButton* )GetDlgItem( WIDC_CH_BLUECHECK );
					if( pWndCheck )
						pWndCheck->EnableWindow( TRUE );
				}
			}
			break;


		case WIDC_CH_REDCHECK:	//레드팀 고정체크
			{
				CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_CH_REDCHECK ); 
				CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_CO_REDLIST);
				if( !pWndButton || !pCombo )
					break;

				//팀을 고르지 않았다면 다시
				if( pCombo->GetCurSel() < 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_EVENTARENA_SELECTTEAM ) );		//팀을 선택하세요
					pWndButton->SetCheck( 0 );
					break;
				}
				
				//체크되면 왼쪽 콤보박스 비활성, 체크해제되면 활성
				BOOL bCheck = pWndButton->GetCheck();
				pCombo->EnableWindow( !bCheck );

				AutoButton_SummonsPlayer( );
	
				//레드팀 선택완료 
				DWORD dwTeamID = 0;
				if( bCheck )
				{
					int nSelected = pCombo->GetCurSel( );
					if( nSelected > -1 )
						dwTeamID = pCombo->GetItemData( nSelected );
				}
				else 
				{
					pCombo->SetCurSel( -1 );
					pWndButton->EnableWindow( FALSE );

				}

				//선택된 아이템의 팀 아이디를 얻어서 sendpacket!
				FLPacketEventArenaManageSetRedTeamNoti kPacket;
				kPacket.dwRedTeamID = dwTeamID;
				g_DPlay.SendPacket( &kPacket );
				
			}
			break;

		case WIDC_CH_BLUECHECK:	//블루팀 고정체크
			{
				CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_CH_BLUECHECK ); 
				CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_CO_BLUELIST);
				if( !pWndButton || !pCombo )
					break;

				//팀을 고르지 않았다면 다시
				if( pCombo->GetCurSel() < 0 )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_EVENTARENA_SELECTTEAM ) );		//팀을 선택하세요
					pWndButton->SetCheck( 0 );
					break;
				}

				//체크되면 왼쪽 콤보박스 비활성, 체크해제되면 활성
				BOOL bCheck = pWndButton->GetCheck();
				pCombo->EnableWindow( !bCheck );

				AutoButton_SummonsPlayer( );

				//블루팀 선택완료 
				DWORD dwTeamID = 0;
				if( bCheck )
				{
					int nSelected = pCombo->GetCurSel( );
					if( nSelected > -1 )
						dwTeamID = pCombo->GetItemData( nSelected );
				}
				else 
				{
					pCombo->SetCurSel( -1 );
					pWndButton->EnableWindow( FALSE );
				}

				//선택된 아이템의 팀 아이디를 얻어서 sendpacket!
				FLPacketEventArenaManageSetBlueTeamNoti kPacket;
				kPacket.dwBlueTeamID = dwTeamID;
				g_DPlay.SendPacket( &kPacket );
			}
			break;
			

		case WIDC_BT_MEMBERCALL:		//선수소환
			{
				CWndButton* pWndBtnMemberCall = (CWndButton*)GetDlgItem( WIDC_BT_MEMBERCALL ); 
				if( pWndBtnMemberCall )
				{
					pWndBtnMemberCall->EnableWindow( FALSE );

					CWndButton* pWndBtnStop = ( CWndButton*)GetDlgItem( WIDC_BT_FIGHTSTOP );
					if( pWndBtnStop )
						pWndBtnStop->EnableWindow( TRUE );

					//sendpacket
					FLPacketEventArenaManageCallPlayersNoti kPacket;
					g_DPlay.SendPacket( &kPacket );

					//체크버튼 비활성
					CWndButton* pWndCheckRed = (CWndButton*)GetDlgItem( WIDC_CH_REDCHECK );
					if( pWndCheckRed )
						pWndCheckRed->EnableWindow( FALSE );
					
					CWndButton* pWndCheckBlue = (CWndButton*)GetDlgItem( WIDC_CH_BLUECHECK );
					if( pWndCheckBlue )
						pWndCheckBlue->EnableWindow( FALSE );

				}
			}
			break;

		case WIDC_BT_MEMBERENTER:  //선수입장
			{
				CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_MEMBERENTER );
				if( pWndButton )
					pWndButton->EnableWindow( FALSE );

				//sendpacket
				FLPacketEventArenaManageJoinPlayersNoti kPacket;
				g_DPlay.SendPacket( &kPacket );
			}
			break;

		case WIDC_BT_FIGHT:	//전투시작
			{
				CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_FIGHT );
				if( pWndButton )
					pWndButton->EnableWindow( FALSE );

				//sendpacket
				FLPacketEventArenaManageBattleStartNoti kPacket;
				g_DPlay.SendPacket( &kPacket );
			}
			break;


		case WIDC_BT_FIGHTSTOP:	//경기중단
			{
				CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_FIGHTSTOP );
				if( pWndButton )
					pWndButton->EnableWindow( FALSE );

				SAFE_DELETE( m_pWndStopBattle );
				m_pWndStopBattle = new CWndEArenaStopBattle;
				m_pWndStopBattle->Initialize( this );
			}
			break;

		case WIDC_BT_FIGHTEND:	//경기 종료
			{
				CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BT_FIGHTEND );
				if( pWndButton )
					pWndButton->EnableWindow( FALSE );

				//sendpacket
				FLPacketEventArenaManageGameEndNoti kPacket;
				g_DPlay.SendPacket( &kPacket );
			}
			break;

		
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaReferee::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndStopBattle )
	{
		SAFE_DELETE( m_pWndStopBattle );
	}
}


void CWndEArenaReferee::UpdateData( const T_EVENT_ARENA_GAME_INFO& kData )
{
	// 설정값만 바꾼다.
	m_kData.tGameInfo = kData;

	//Reset
	ResetAllChildWnd( );

	//토너먼트 방식
	CWndComboBox* pWndCombo = NULL;
	pWndCombo = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
	if( pWndCombo )
		pWndCombo->SelectItem( kData.dwTonementID, FALSE );

	//팀 - 레드, 블루
	pWndCombo = ( CWndComboBox* )GetDlgItem( WIDC_CO_REDLIST );
	if( pWndCombo )
		pWndCombo->SelectItem( kData.dwRedTeamID, FALSE );

	pWndCombo = ( CWndComboBox* )GetDlgItem( WIDC_CO_BLUELIST );
	if( pWndCombo )
		pWndCombo->SelectItem( kData.dwBlueTeamID, FALSE );

	//승리수
	UpdateWinCount( kData.byRedWinFlag, kData.byBlueWinFlag );

	m_fBattleTime = (float)m_kData.tGameInfo.dwRemainBattleTime/1000.0f;

	//상태값에 따라 각 콘트롤들의 대한 Enable상태를 제어
	UpdateState( kData );

}

void CWndEArenaReferee::UpdateState( const T_EVENT_ARENA_GAME_INFO& kData )
{
	//모두 비활성으로 초기화
	EnableChildWnd( WIDC_CO_MATCHNAME, FALSE );
	EnableChildWnd( WIDC_CO_REDLIST, FALSE );
	EnableChildWnd( WIDC_CO_BLUELIST, FALSE );
	EnableChildWnd( WIDC_CH_REDCHECK, FALSE );
	EnableChildWnd( WIDC_CH_BLUECHECK, FALSE );
	EnableChildWnd( WIDC_BT_MEMBERCALL, FALSE );		//선수소환
	EnableChildWnd( WIDC_BT_MEMBERENTER, FALSE );		//선수입장
	EnableChildWnd( WIDC_BT_FIGHT, FALSE );
	EnableChildWnd( WIDC_BT_FIGHTSTOP, FALSE );
	EnableChildWnd( WIDC_BT_FIGHTEND, FALSE );

	SwitchingTimer( FALSE );

	CWndComboBox* pBox = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
	if( NULL == pBox )
		return;

	if( kData.nState > EVENT_ARENA_STATE_CONFIG )
	{
		//설정단계를 지났다면 초기화 해야할일을 한다.
		CWndButton* pWndCheckRed = (CWndButton*)GetDlgItem( WIDC_CH_REDCHECK );
		if( pWndCheckRed )
			pWndCheckRed->SetCheck( TRUE );

		CWndButton* pWndCheckBlue = (CWndButton*)GetDlgItem( WIDC_CH_BLUECHECK );
		if( pWndCheckBlue )
			pWndCheckBlue->SetCheck( TRUE );
	}

	//set 토너먼트 룰
	if( 0 != kData.dwTonementID )
	{
		T_EVENT_ARENA_TONEMENT* pTourData = FindTournament( kData.dwTonementID );
		if( pTourData )
		{
			CWndStatic* pTourType = ( CWndStatic* )GetDlgItem( WIDC_ST_MATCH );
			if( pTourType )
				pTourType->SetTitle( CString(pTourData->szDesc) );
		}
	}
	
	switch( kData.nState )
	{
	case EVENT_ARENA_STATE_CONFIG:		// 설정 단계
		if( 0 == kData.dwTonementID )
		{
			//토너먼트 조차 고르지 않았다 처음부터 다시시작.
			EnableChildWnd( WIDC_CO_MATCHNAME, TRUE );
		}
		else
		{
			//토너먼트는 이미 정해져있는경우
			//팀이 정해지지 않았다면 고르게 함.
			if( 0 == kData.dwRedTeamID )
				EnableChildWnd( WIDC_CO_REDLIST, TRUE );
			else 
			{
				//팀이 결정댔다.. 확인버튼 체크
				CWndButton* pWndCheckRed = (CWndButton*)GetDlgItem( WIDC_CH_REDCHECK );
				if( pWndCheckRed )
				{
					pWndCheckRed->SetCheck( TRUE );
					EnableChildWnd( WIDC_CH_REDCHECK, TRUE );
				}
			}


			if( 0 == kData.dwBlueTeamID )
				EnableChildWnd( WIDC_CO_BLUELIST, TRUE );
			else
			{
				//팀이 결정댔다.. 확인버튼 체크
				CWndButton* pWndCheckBlue = (CWndButton*)GetDlgItem( WIDC_CH_BLUECHECK );
				if( pWndCheckBlue )
				{
					pWndCheckBlue->SetCheck( TRUE );
					EnableChildWnd( WIDC_CH_BLUECHECK, TRUE );
				}
			}

			//다음단계로 진행가능하면 진행.
			AutoButton_SummonsPlayer( );

		}
		break;

	case EVENT_ARENA_STATE_CALL_WAIT:					// 선수 소환 대기
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		break;

	case EVENT_ARENA_STATE_CALL:						// 선수 소환
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		//not to do
		break;
	
	case EVENT_ARENA_STATE_CALLED:  					// 선수 소환 끝
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		EnableChildWnd( WIDC_BT_MEMBERENTER, TRUE );	// 선수입장 활성화
		CompletedSummonsPlayer( );
		break;

	case EVENT_ARENA_STATE_JOIN:						// 선수 입장
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		//not to do
		break;

	case EVENT_ARENA_STATE_JOINED:						// 선수 입장 끝
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		CompletedJoinPlayer( );
		break;

	//전투중
	case EVENT_ARENA_STATE_COUNTDOWN:	// 카운트 다운
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		//not to do
		break;

	case EVENT_ARENA_STATE_PLAYING:		// 전투 중
		EnableChildWnd( WIDC_BT_FIGHTSTOP, TRUE );		// 경기중단 활성화
		SwitchingTimer( TRUE );
		break;

	//한경기 종료
	case EVENT_ARENA_STATE_RESULT:		// 전투 결과
		CompletedOneGame( );
		break;

	//풀세트 경기종료
	case EVENT_ARENA_STATE_END:			// 종료
		EnableChildWnd( WIDC_BT_FIGHTEND, TRUE );	// 경기종료 활성
		CompletedFullGame( );
		break;
	}

}

void CWndEArenaReferee::SetData( const FLSnapshotEventArenaManageWindowNoti& kData )
{
	//기본 데이터를 모두 채워넣는다.
	m_kData = kData;

 	SetTimer( (float)m_kData.tGameInfo.dwRemainBattleTime/1000.0f );

	//토너먼트 
	CWndComboBox* pWndComboTournament = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
	if( !pWndComboTournament )
		return;

	pWndComboTournament->ResetContent( );
	for( std::vector< T_EVENT_ARENA_TONEMENT >::iterator iter = m_kData.vecTonements.begin(); iter != m_kData.vecTonements.end(); ++iter )
	{
		T_EVENT_ARENA_TONEMENT& kTourData = *iter;
		//pWndComboTournament->InsertString( kTourData.dwID, kTourData.szName );
		int nIndex = pWndComboTournament->AddString( kTourData.szName );
		pWndComboTournament->SetItemData( nIndex, kTourData.dwID );
	}
	
	//각팀
	CWndComboBox* pWndComboTeam_RED = ( CWndComboBox* )GetDlgItem( WIDC_CO_REDLIST );
	CWndComboBox* pWndComboTeam_BLUE = ( CWndComboBox* )GetDlgItem( WIDC_CO_BLUELIST );
	if( !pWndComboTeam_RED || !pWndComboTeam_BLUE )
		return;

	pWndComboTeam_RED->ResetContent( );
	pWndComboTeam_BLUE->ResetContent( );
	for( std::vector< T_EVENT_ARENA_TEAM_INFO >::iterator iter = m_kData.vecTeams.begin(); iter != m_kData.vecTeams.end(); ++iter )
	{
		T_EVENT_ARENA_TEAM_INFO& kTeamData = *iter;
	//	pWndComboTeam_RED->InsertString( kTeamData.dwID, kTeamData.szName );
		int nIndex1 = pWndComboTeam_RED->AddString( kTeamData.szName );
		pWndComboTeam_RED->SetItemData( nIndex1, kTeamData.dwID );

	//	pWndComboTeam_BLUE->InsertString( kTeamData.dwID, kTeamData.szName );
		int nIndex2 = pWndComboTeam_BLUE->AddString( kTeamData.szName );
		pWndComboTeam_BLUE->SetItemData( nIndex2, kTeamData.dwID );
	}

	//각 데이터가 채워진 이후 업데이트 
	if( m_kData.tGameInfo.dwTonementID == 0 )
	{
		//초기상태이므로 업데이트할껏이 없다.
	}
	else
	{
		//경기방식등 유효한 설정값이므로 업데이트 해야함
		UpdateData( m_kData.tGameInfo );		
	}
}

void CWndEArenaReferee::ResetAllChildWnd( )
{
	CWndStatic* pTourType = ( CWndStatic* )GetDlgItem( WIDC_ST_MATCH );
	if( pTourType )
		pTourType->SetTitle( "" );

	//콤보박스, 체크박스를 초기화 ( Enable이 아님 )
	CWndComboBox* pWndCombo = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
	if( pWndCombo )
		pWndCombo->SetCurSel( -1 );

	CWndComboBox* pWndRedTeam = ( CWndComboBox* )GetDlgItem( WIDC_CO_REDLIST );
	if( pWndRedTeam )
		pWndRedTeam->SetCurSel( -1 );

	CWndComboBox* pWndBlueTeam = ( CWndComboBox* )GetDlgItem( WIDC_CO_BLUELIST );
	if( pWndBlueTeam )
		pWndBlueTeam->SetCurSel( -1 );

	CWndButton* pWndCheckRed = (CWndButton*)GetDlgItem( WIDC_CH_REDCHECK ); 
	if( pWndCheckRed )
		pWndCheckRed->SetCheck( FALSE );

	CWndButton* pWndCheckBlue = (CWndButton*)GetDlgItem( WIDC_CH_BLUECHECK ); 
	if( pWndCheckBlue )
		pWndCheckBlue->SetCheck( FALSE );
}

void CWndEArenaReferee::UpdateWinCount( const BYTE* pRedWin, const BYTE* pBlueWin )
{	
	if( NULL == pRedWin || NULL == pBlueWin )
		return;

	CWndButton* pWndCheckRED = NULL;
	CWndButton* pWndCheckBLUE = NULL;

	UINT nRedCheck[MAX_EVENT_ARENA_WIN_COUNT] = { WIDC_CH_REDRESULT01, WIDC_CH_REDRESULT02, WIDC_CH_REDRESULT03, WIDC_CH_REDRESULT04, WIDC_CH_REDRESULT05, WIDC_CH_REDRESULT06, WIDC_CH_REDRESULT07 };
	UINT nBlueCheck[MAX_EVENT_ARENA_WIN_COUNT] = { WIDC_CH_BLUERESULT01, WIDC_CH_BLUERESULT02, WIDC_CH_BLUERESULT03, WIDC_CH_BLUERESULT04, WIDC_CH_BLUERESULT05, WIDC_CH_BLUERESULT06, WIDC_CH_BLUERESULT07 };

	for( int i = 0; i < MAX_EVENT_ARENA_WIN_COUNT; ++i )
	{
		pWndCheckRED = ( CWndButton* )GetDlgItem( nRedCheck[i] );
		pWndCheckBLUE = ( CWndButton* )GetDlgItem( nBlueCheck[i] );

		if( !pWndCheckRED || !pWndCheckBLUE )
			break;

		pRedWin[i] == 0 ? pWndCheckRED->SetCheck( 0 ) : pWndCheckRED->SetCheck( 1 );
		pBlueWin[i] == 0 ? pWndCheckBLUE->SetCheck( 0 ) : pWndCheckBLUE->SetCheck( 1 );

		//귀찮으니 여기서 disable
		pWndCheckRED->EnableWindow( FALSE );
		pWndCheckBLUE->EnableWindow( FALSE );

	}
}

void CWndEArenaReferee::EnableChildWnd( DWORD dwWIDC, BOOL bEnable )
{
	CWndBase* pChild = (CWndBase*)GetDlgItem( dwWIDC );
	if( pChild )
		pChild->EnableWindow( bEnable );
}

T_EVENT_ARENA_TONEMENT* CWndEArenaReferee::FindTournament( DWORD dwTourID )
{
	for( std::vector< T_EVENT_ARENA_TONEMENT >::iterator iter = m_kData.vecTonements.begin(); iter != m_kData.vecTonements.end(); ++iter )
	{
		T_EVENT_ARENA_TONEMENT* pTourData = &(*iter);
		if( pTourData->dwID == dwTourID )
			return pTourData;
	}

	return NULL;
}

void CWndEArenaReferee::CompletedSummonsPlayer( )
{
	SAFE_DELETE( m_pWndMessageBox );
}

void CWndEArenaReferee::CompletedJoinPlayer( )
{
	EnableChildWnd( WIDC_BT_FIGHT, TRUE );			// 전투시작 활성화
}

void CWndEArenaReferee::CompletedOneGame( )
{
	//선수입장 활성화
//	EnableChildWnd( WIDC_BT_MEMBERENTER, TRUE );

	//stop timer
	SwitchingTimer( FALSE );
}

void CWndEArenaReferee::CompletedFullGame( )
{
	//stop timer
	SwitchingTimer( FALSE );
}

void CWndEArenaReferee::MakeMessageBox( int nType, DWORD dwTime )
{
	FLASSERT( dwTime > 0 );

	SAFE_DELETE( m_pWndMessageBox );

	m_pWndMessageBox = new CWndEArenaAdminMessage;
	m_pWndMessageBox->Initialize( this );
	m_pWndMessageBox->SetType( nType );
	m_pWndMessageBox->SetTimer( (float)dwTime, (float)dwTime );
}

void CWndEArenaReferee::SetTimer( float fRemainTime )
{
	FLASSERT( fRemainTime > 0 );

	m_fBattleTime = fRemainTime;
}

void CWndEArenaReferee::AutoButton_SummonsPlayer( )
{
	//각팀의 체크버튼 상태에 따라 "선수소환" 버튼 활성, 비활성
	//진행경기 타입 콤보박스 활성, 비활성

	CWndButton* pWndCheckRed = (CWndButton*)GetDlgItem( WIDC_CH_REDCHECK ); 
	CWndButton* pWndCheckBlue = (CWndButton*)GetDlgItem( WIDC_CH_BLUECHECK ); 

	if( !pWndCheckBlue || !pWndCheckBlue )
		return;

	CWndButton* pWndSummonsPlayer = ( CWndButton* )GetDlgItem( WIDC_BT_MEMBERCALL );
	if( !pWndSummonsPlayer )
		return;

	CWndComboBox* pWndCombo = ( CWndComboBox* )GetDlgItem( WIDC_CO_MATCHNAME );
	if( !pWndCombo )
		return;

	if( pWndCheckRed->GetCheck() && pWndCheckBlue->GetCheck( ) )
	{
		//양팀이 같다면 오류다.
		DWORD dwRedTeamID = 0;
		DWORD dwBlueTeamID = 0;
		CWndComboBox* pWndRedTeam = ( CWndComboBox* )GetDlgItem( WIDC_CO_REDLIST );
		if( pWndRedTeam )
			dwRedTeamID = pWndRedTeam->GetSelectedItemData( );

		CWndComboBox* pWndBlueTeam = ( CWndComboBox* )GetDlgItem( WIDC_CO_BLUELIST );
		if( pWndBlueTeam )
			dwBlueTeamID = pWndBlueTeam->GetSelectedItemData( );

		if( dwRedTeamID == dwBlueTeamID )
		{
			pWndCheckBlue->SetCheck( 0 );
			pWndBlueTeam->EnableWindow( TRUE );
			g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_EVENTARENA_SELECTSAMETEAM ) );	//똑같은 팀이 선택되었으니 다시 선택
			return;
		}

		pWndSummonsPlayer->EnableWindow( TRUE );
		pWndCombo->EnableWindow( FALSE );
	}
	else if( pWndCheckRed->GetCheck() || pWndCheckBlue->GetCheck( ) )
	{
		//양팀중 하나만 체크댐
		pWndSummonsPlayer->EnableWindow( FALSE );
		pWndCombo->EnableWindow( FALSE );
	}
	else 
	{
		//양팀 모두 체크안댐
		pWndSummonsPlayer->EnableWindow( FALSE );
		pWndCombo->EnableWindow( TRUE );
	}
}

void CWndEArenaReferee::OnDraw( C2DRender* p2DRender ) 
{
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaWinner
CWndEArenaWinner::CWndEArenaWinner( ) : 
m_nMode( MODE_WINNER ),
m_fTime( 5.0f ),
m_pTexture( NULL )
{
	m_bNoCloseButton = TRUE;
	m_pWndMatchInfo = NULL;
}

CWndEArenaWinner::~CWndEArenaWinner( )
{
	SAFE_DELETE( m_pWndMatchInfo );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaWinner::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_WINNER, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaWinner::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	DelWndStyle( WBS_MOVE );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndEArenaWinner::Process( void )
{
	//카운트 다운, 시간종료시 제거 
	if( m_fTime > 0.0f )
		m_fTime -= _GetDeltaTimeSec( );
	else 
		Destroy( );

	return TRUE;
}

BOOL CWndEArenaWinner::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
   
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndEArenaWinner::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaWinner::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndMatchInfo )
	{
		SAFE_DELETE( m_pWndMatchInfo );
	}
}

void CWndEArenaWinner::OnDraw( C2DRender* p2DRender ) 
{
	WNDCTRL* wndCtrl = GetWndCtrl( WIDC_STATIC7 );		

	if( m_pTexture && wndCtrl )
		m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), CPoint( wndCtrl->rect.Width(), wndCtrl->rect.Height() ) );
}

void CWndEArenaWinner::SetMode( const int nMode )
{
	m_nMode = nMode;

	if( MODE_WINNER == m_nMode )
	{
		m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, 
			MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndWinner.bmp" ), 0xffff00ff );
	}
	else if( MODE_FINAL_WINNER == m_nMode )
	{
		m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, 
			MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndFinalWinner.bmp" ), 0xffff00ff );
	}
}

void CWndEArenaWinner::SetData_BattleResult( const FLSnapshotEventArenaBattleResultNoti& kData )
{
	PLAYSND( SND_EVENTARENA_WINNER );

	SetTimer( (float)kData.dwWinnerDisplayTime );
	SetMode( MODE_WINNER );

	if( EVENT_ARENA_RED	== kData.nBattleWinTeam )
	{
		//승자 : 레드팀
		CWndStatic* pWinnerTeamName = ( CWndStatic* )GetDlgItem( WIDC_ST_WINNERTEAM );
		if( pWinnerTeamName )
			pWinnerTeamName->SetTitle( kData.szRedName );
	}
	else if( EVENT_ARENA_BLUE == kData.nBattleWinTeam )
	{
		//승자 : 블루팀
		CWndStatic* pWinnerTeamName = ( CWndStatic* )GetDlgItem( WIDC_ST_WINNERTEAM );
		if( pWinnerTeamName )
			pWinnerTeamName->SetTitle( kData.szBlueName );
	}

	CWndStatic* pWndStatic = NULL;
	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDTEAM01 );
	if( pWndStatic )
		pWndStatic->SetTitle( kData.szRedName );

	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUETEAM );
	if( pWndStatic )
		pWndStatic->SetTitle( kData.szBlueName );

	CString strBuffer;
	strBuffer.Format( "%d", kData.nRedWinCount );
	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDTEAMSCORE );
	if( pWndStatic )
		pWndStatic->SetTitle( strBuffer );

	strBuffer.Format( "%d", kData.nBlueWinCount );
	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUETEAMSCORE );
	if( pWndStatic )
		pWndStatic->SetTitle( strBuffer );
}

void CWndEArenaWinner::SetData_GameResult( const FLSnapshotEventArenaGameResultNoti& kData )
{
	PLAYSND( SND_EVENTARENA_WINNER );

	SetTimer( (float)kData.dwWinnerDisplayTime );
	SetMode( MODE_FINAL_WINNER );

	if( EVENT_ARENA_RED	== kData.nGameWinTeam )
	{
		//승자 : 레드팀
		CWndStatic* pWinnerTeamName = ( CWndStatic* )GetDlgItem( WIDC_ST_WINNERTEAM );
		if( pWinnerTeamName )
			pWinnerTeamName->SetTitle( kData.szRedName );
	}
	else if( EVENT_ARENA_BLUE == kData.nGameWinTeam )
	{
		//승자 : 블루팀
		CWndStatic* pWinnerTeamName = ( CWndStatic* )GetDlgItem( WIDC_ST_WINNERTEAM );
		if( pWinnerTeamName )
			pWinnerTeamName->SetTitle( kData.szBlueName );
	}

	CWndStatic* pWndStatic = NULL;
	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDTEAM01 );
	if( pWndStatic )
		pWndStatic->SetTitle( kData.szRedName );

	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUETEAM );
	if( pWndStatic )
		pWndStatic->SetTitle( kData.szBlueName );

	CString strBuffer;
	strBuffer.Format( "%d", kData.nRedWinCount );
	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDTEAMSCORE );
	if( pWndStatic )
		pWndStatic->SetTitle( strBuffer );

	strBuffer.Format( "%d", kData.nBlueWinCount );
	pWndStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUETEAMSCORE );
	if( pWndStatic )
		pWndStatic->SetTitle( strBuffer );

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if( pWndWorld )
		pWndWorld->BeginEArenaShow( );

	//match info 윈도우 생성
	if( g_WndMng.m_pWndScoreTitle )
	{
		SetData_BattleInfo( g_WndMng.m_pWndScoreTitle->GetMatchInfoData() );
	}
}

void CWndEArenaWinner::SetData_BattleInfo( const std::vector< T_BATTLE_RESULT_LOG >& kData )
{
	SAFE_DELETE( m_pWndMatchInfo );

	m_pWndMatchInfo = new CWndMatchInfo;
	m_pWndMatchInfo->Initialize( this );
	m_pWndMatchInfo->SetData( kData );
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaAdminMessage
CWndEArenaAdminMessage::CWndEArenaAdminMessage( ):
m_fMaxSec( 1.0f ),
m_fRemainSec( 1.0f ),
m_pGauge( NULL ),
m_nType( MSG_DOING_SUMMONS )
{
	m_bNoCloseButton = TRUE;
}

CWndEArenaAdminMessage::~CWndEArenaAdminMessage( )
{
	m_texGauge.DeleteDeviceObjects();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaAdminMessage::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_ENTER, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaAdminMessage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_texGauge.LoadTexture( m_pApp->m_pd3dDevice, 
		MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndColoGageBar01.tga" ), 0xffff00ff, TRUE );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	Move( m_rectWindow.left, 100 );

	//TEST
	SetTimer( 90.0f, 180.0f );
} 

BOOL CWndEArenaAdminMessage::Process( void )
{
	if( m_fRemainSec > 0.0f )
	{
		m_fRemainSec -= _GetDeltaTimeSec( );

		if( m_fRemainSec < 0.0f )
			m_fRemainSec = 0.0f;
	}
	else
	{
		m_pParentWnd->OnCommand( this->GetWndId(), WM_CLOSE, this );
	}

	return TRUE;
}

BOOL CWndEArenaAdminMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaAdminMessage::OnDraw( C2DRender* p2DRender ) 
{
	MakeGaugeRect( );
	m_pTheme->RenderGauge( p2DRender, &m_rectGauge, 0xffffffff, m_pGauge, &m_texGauge );
}

void CWndEArenaAdminMessage::SetType( const int nType )
{
	m_nType = nType;

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_ST_STATIC1 );
	if( NULL == pStatic )
		return;

	switch( nType )
	{
	case MSG_DOING_SUMMONS:
		SetTitle( prj.GetText( TID_MMI_EVENTARENA_CALLADMIN_TITLE ) );
		
		if( pStatic )
			pStatic->SetTitle( prj.GetText( TID_MMI_EVENTARENA_CALLADMIN ) ); //선수소환이 진행중입니다.

		break;

	case MSG_DOING_ENTRANCE:
		SetTitle( prj.GetText( TID_MMI_EVENTARENA_MEMBERENTER_TITLE ) );
		
		if( pStatic )
			pStatic->SetTitle( prj.GetText( TID_MMI_EVENTARENA_MEMBERENTER ) ); //선수입장이 진행중입니다.

		break;

	default : 
		FLASSERT( 0 );
		break;
	}
}

void CWndEArenaAdminMessage::MakeGaugeRect( )
{
	if( m_fMaxSec <= 0.0f )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_fMaxSec %f" ), m_fMaxSec );
		return;
	}

	LPWNDCTRL lpEXP   = GetWndCtrl( WIDC_ST_DELAY );
	m_rectGauge = lpEXP->rect;

	int nWidthClient = lpEXP->rect.Width();

	//exception
	if( m_fMaxSec <= 0.0f )
		m_fMaxSec = 10.0f;

	float fRatio = m_fRemainSec / m_fMaxSec;		//백분율( 0 ~ 1 )
	int nOffsetRight = (int)( nWidthClient * fRatio ) ;

	m_rectGauge.right = m_rectGauge.left + nOffsetRight;
}

void CWndEArenaAdminMessage::SetTimer( float fRemainSec, float fMaxSec )
{
	if( fMaxSec <= 0.0f )
	{
		FLASSERT( 0 );
		return;
	}

	m_fRemainSec = fRemainSec;
	m_fMaxSec = fMaxSec;
}

HRESULT CWndEArenaAdminMessage::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pGauge, NULL );
	}
	
	return S_OK;
}
HRESULT CWndEArenaAdminMessage::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGauge );

	return S_OK;//return S_OK;
}
HRESULT CWndEArenaAdminMessage::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}


//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaUserMessage_Summons
CWndEArenaUserMessage_Summons::CWndEArenaUserMessage_Summons( ):
m_fMaxSec( 100.0f ),
m_fRemainSec( 0.0f ),
m_pGauge( NULL )
{
	m_bNoCloseButton = TRUE;
}

CWndEArenaUserMessage_Summons::~CWndEArenaUserMessage_Summons( )
{
	m_texGauge.DeleteDeviceObjects();
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaUserMessage_Summons::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_MEMCALL, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaUserMessage_Summons::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_texGauge.LoadTexture( m_pApp->m_pd3dDevice, 
		MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndColoGageBar01.tga" ), 0xffff00ff, TRUE );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	//TEST
	SetTimer( 90.0f, 180.0f );

	CWndStatic* pStatic = ( CWndStatic* )GetDlgItem( WIDC_STATIC1 );
	if( pStatic )
		pStatic->SetTitle( prj.GetText(	TID_MMI_EVENTARENA_CALLMEMBER ) );	//아레나의 대기 지역으로 소환됩니다.
} 

BOOL CWndEArenaUserMessage_Summons::Process( void )
{
	if( m_fRemainSec > 0.0f )
	{
		m_fRemainSec -= _GetDeltaTimeSec();
	}
	else
	{
		m_fRemainSec = 0.0f;
		SendPacket( );			
		Destroy();
	}
	
	return TRUE;
}

BOOL CWndEArenaUserMessage_Summons::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BT_OK:
		SendPacket( );
		break;

	default:
		FLASSERT( 0 );
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaUserMessage_Summons::OnDraw( C2DRender* p2DRender ) 
{
	MakeGaugeRect();
	m_pTheme->RenderGauge( p2DRender, &m_rectGauge, 0xffffffff, m_pGauge, &m_texGauge );
}

void CWndEArenaUserMessage_Summons::MakeGaugeRect( )
{
	if( m_fMaxSec <= 0.0f )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_fMaxSec %f" ), m_fMaxSec );
		return;
	}

	LPWNDCTRL lpEXP   = GetWndCtrl( WIDC_ST_DELAY );
	m_rectGauge = lpEXP->rect;

	int nWidthClient = lpEXP->rect.Width();
	float fRatio = m_fRemainSec / m_fMaxSec;		//백분율( 0 ~ 1 )
	int nOffsetRight = (int)( nWidthClient * fRatio ) ;

	m_rectGauge.right = m_rectGauge.left + nOffsetRight;
}

void CWndEArenaUserMessage_Summons::SetTimer( float fRemainSec, float fMaxSec )
{
	if( fMaxSec <= 0.0f )
	{
		FLASSERT( 0 );
		return;
	}

	m_fRemainSec = fRemainSec;
	m_fMaxSec = fMaxSec;
}

void CWndEArenaUserMessage_Summons::SendPacket( )
{
	Destroy( );
}

HRESULT CWndEArenaUserMessage_Summons::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pGauge, NULL );
	}
	
	return S_OK;
}
HRESULT CWndEArenaUserMessage_Summons::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGauge );

	return S_OK;//return S_OK;
}
HRESULT CWndEArenaUserMessage_Summons::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndEArenaStopBattle
CWndEArenaStopBattle::CWndEArenaStopBattle( )
{
	m_bNoCloseButton = TRUE;
}

CWndEArenaStopBattle::~CWndEArenaStopBattle( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndEArenaStopBattle::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_STOP, WBS_MODAL | WBS_KEY, CPoint( 0, 0 ), pWndParent );
} 

void CWndEArenaStopBattle::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.

	MoveParentCenter();

	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	if( pText )
	{
		CString strBuffer;
		strBuffer.Format( prj.GetText(TID_MMI_EVENTARENA_MATCHSTOP ), prj.GetText(TID_MMI_EVENTARENA_MATCHSTOPWORD ) );	//경기중단? [경기중단]입력요망
		pText->SetString( strBuffer );		
	}

	AddWndStyle( EBS_WANTRETURN );
} 

//BOOL CWndEArenaTeam::Process( void )
//{
//	return CWndBase::Process( );
//}


BOOL CWndEArenaStopBattle::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message || nID == WIDC_BT_OK )
	{
		if( TRUE == SendPacket( ) )
			Destroy( );

		//ok.
	}

	if( WIDC_BT_CANCEL == nID )
	{
		//취소인경우 부모윈도우( 운영자윈도 ) 의 경기중단버튼 활성화
		if( m_pParentWnd )
			m_pParentWnd->OnCommand( this->GetWndId(), nID, this );

		Destroy( );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndEArenaStopBattle::OnDraw( C2DRender* p2DRender ) 
{
}

BOOL CWndEArenaStopBattle::SendPacket( )
{
	CString strPASSWORD = prj.GetText( TID_MMI_EVENTARENA_MATCHSTOPWORD ); // "경기 중단" );

	CString strBuffer;
	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_ET_STOP );
	if( pEdit )
		strBuffer = pEdit->GetString( );

	if( FALSE == strBuffer.IsEmpty( ) )
	{
		//아레나를 중단시키는 명령어
		if( strPASSWORD == strBuffer )
		{
			//sendpacket
			FLPacketEventArenaManageGameStopNoti kPacket;
			g_DPlay.SendPacket( &kPacket );
			return TRUE;
		}
		else 
		{
			g_WndMng.OpenMessageBox( "try again!" );
			pEdit->SetString("");
			return FALSE;
		}
	}

	//경기중단을 입력해야한다는 메시지 뿌릴꺼면 여기서.

	return FALSE;
}

static const float SCORE_TIME_MAX = 10.0f;

//----------------------------------------------------------------------------------------------------------------------------------
//CWndScoreTitle
CWndScoreTitle::CWndScoreTitle( ) : 
m_bToggleContent( TRUE ),
m_bToggleMatchInfo( FALSE ),
m_fDt( 0.0f ),
m_fAddedDt( 0.0f ),
m_bBlinked( FALSE ),
m_pWndMatchInfo( NULL )
{
}

CWndScoreTitle::~CWndScoreTitle( )
{
	SAFE_DELETE( m_pWndMatchInfo );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndScoreTitle::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_DISPLAYTITLE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndScoreTitle::OnInitialUpdate() 
{ 

	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	//MoveParentCenter();

	if( m_pWndRoot )
	{
		CRect rcRoot = m_pWndRoot->m_rectWindow;
		CRect rcTarget = rcRoot;

		rcTarget.left = rcRoot.right - m_rectWindow.Width() * 2;
		rcTarget.right = rcTarget.left + m_rectWindow.Width();
		rcTarget.bottom = rcTarget.top + m_rectWindow.Height();

		SetWndRect( rcTarget, FALSE );
	}
		
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_ST_SITUATION );
	if( pStatic )
	{
		pStatic->AddWndStyle( WSS_NOPICKUP );
		pStatic->AddWndStyle( WSS_TOLEFT_AXISX );
	}

	m_kContent.Initialize( this );
	m_kContent.Move( m_rectWindow.left, m_rectWindow.bottom );

	m_rectMy = m_rectWindow;
	m_rectContent = m_kContent.m_rectWindow;
} 

BOOL CWndScoreTitle::Process( void )
{
	if( m_bBlinked )
	{
		m_fDt += _GetDeltaTimeSec( );

		if( m_fDt > 0.5f )
		{
			m_fAddedDt += m_fDt;
			m_fDt = 0.0f;
			CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			if( pStatic )
			{
				pStatic->SetVisible( !pStatic->IsVisible() );
			}
		}

		if( m_fAddedDt > SCORE_TIME_MAX )
		{
			m_fDt = 0.0f;
			m_fAddedDt = 0.0f;
			m_bBlinked = FALSE;
		}
	}

	return TRUE;
}

void CWndScoreTitle::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndMatchInfo )
	{
		SAFE_DELETE( m_pWndMatchInfo );
		//자식윈도우가 파괴되었으므로 Toggle button update!
		SetUpDownButton2( FALSE );
	}
}


BOOL CWndScoreTitle::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON2:		//전광판
		{
			CWndButton* pWndToggleBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
			if( pWndToggleBtn )
				SetUpDownButton( !m_bToggleContent );
		}
		break;

	case WIDC_BT_MINIMUM:		//정보
		{
			CWndButton* pWndToggleBtn = ( CWndButton* )GetDlgItem( WIDC_BT_MINIMUM );
			if( pWndToggleBtn )
				SetUpDownButton2( !m_bToggleMatchInfo );
		}
		break;

	default: 
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndScoreTitle::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndScoreTitle::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

	if( m_kContent.m_pApp != NULL )
		m_kContent.Move( m_rectWindow.left, m_rectWindow.bottom );

	if( m_pWndMatchInfo )
		AutoPosition_MatchInfo( );
}

void CWndScoreTitle::SetData_Board( const FLSnapshotEventArenaGameScoreboardNoti& kData )
{
	//상태가 바뀌면 알림효과 
#if 0
	if( m_kData.byGameState != kData.byGameState )
		BeginEff_ChangedState( );
#endif 

	m_kData = kData;

	//경기상태
	CWndStatic* pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_SITUATION );
	if( pStatic )
	{
		int nTID = 0;
		switch( kData.byGameState )
		{
		case EVENT_ARENA_GAME_STATE_NONE:	nTID = TID_MMI_EVENTARENA_STANDBY;			break;
		case EVENT_ARENA_GAME_STATE_CALL:	nTID = TID_MMI_EVENTARENA_PLAYERCALL;		break;
		case EVENT_ARENA_GAME_STATE_JOIN:	nTID = TID_MMI_EVENTARENA_PLAYERENTER;		break;
		case EVENT_ARENA_GAME_STATE_PLAYING:	nTID = TID_MMI_EVENTARENA_PLAYERFIGHT;	break;
		default:
			FLASSERT( 0 ); break;
		}
		
		pStatic->SetTitle( prj.GetText( nTID ) );
	}

	//경기준비중이라면 내용부 hiding
	if( m_kData.byGameState == EVENT_ARENA_GAME_STATE_NONE )
		SetUpDownButton( FALSE );
	else SetUpDownButton( TRUE );

	m_kContent.UpdateData( kData );
}

void CWndScoreTitle::SetData_Board2( const std::vector< T_BATTLE_RESULT_LOG >& vecData )
{
	//match 윈도우는 없을수도있으므로 데이터을 보관해야한다.
	m_vecMatchInfoData = vecData;

	if( m_pWndMatchInfo )
		m_pWndMatchInfo->SetData( m_vecMatchInfoData );
}

void CWndScoreTitle::BeginEff_ChangedState( )
{
	//전광판이 축소모드일경우 상태가 바뀌면 알려줌효과를 준다.
	if( FALSE ==  m_kContent.IsVisible( ) )
		m_bBlinked = TRUE;

	m_kContent.m_bEffectChangedState = TRUE;
	m_kContent.m_fEffTime = 0.0f;
}

void CWndScoreTitle::SetUpDownButton( BOOL bOn )
{
	m_bToggleContent = bOn;

	m_kContent.SetVisible( m_bToggleContent );

	CWndButton* pWndToggleBtn = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
	if( pWndToggleBtn ) 
		m_bToggleContent ? pWndToggleBtn->SetCheck( 0 ) : pWndToggleBtn->SetCheck( 2 );	

	AutoPosition_MatchInfo( );
}

void CWndScoreTitle::SetUpDownButton2( BOOL bOn )
{
	//match info toggle
	m_bToggleMatchInfo = bOn;

	if( NULL == m_pWndMatchInfo )
	{
		m_pWndMatchInfo = new CWndMatchInfo;
		m_pWndMatchInfo->Initialize( this );
		m_pWndMatchInfo->SetData( m_vecMatchInfoData );
		//m_pWndMatchInfo->Move( m_rectWindow.left, m_rectWindow.bottom + m_rectContent.Height() );
	}

	if( m_pWndMatchInfo )
	{
		m_pWndMatchInfo->SetVisible( m_bToggleMatchInfo );
		m_pWndMatchInfo->m_fDelta = 0.0f;
	}

	CWndButton* pWndToggleBtn = ( CWndButton* )GetDlgItem( WIDC_BT_MINIMUM );
	if( pWndToggleBtn ) 
		m_bToggleMatchInfo ? pWndToggleBtn->SetCheck( 0 ) : pWndToggleBtn->SetCheck( 2 );	

	AutoPosition_MatchInfo( );
}

void CWndScoreTitle::AutoPosition_MatchInfo( )
{
	if( NULL == m_pWndMatchInfo )
		return;

	if( m_kContent.IsVisible( ) )
	{
		m_pWndMatchInfo->Move( m_rectWindow.left, m_rectWindow.bottom + m_rectContent.Height() );
	}
	else 
	{
		m_pWndMatchInfo->Move( m_rectWindow.left, m_rectWindow.bottom );
	}
}
//----------------------------------------------------------------------------------------------------------------------------------
//CWndScoreContent
CWndScoreContent::CWndScoreContent( )
{
	m_fDt = 0.0f;
	m_fMaxDt = 0.5f;
	m_nAni = 0;

	m_bEffectChangedState = FALSE;
	m_fEffTime = 0.0f;
}

CWndScoreContent::~CWndScoreContent( )
{
	m_kEffTex[0].DeleteDeviceObjects( );
	m_kEffTex[1].DeleteDeviceObjects( );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndScoreContent::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_SCOREDISPLAY, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndScoreContent::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.

	MoveParentCenter();

	DelWndStyle( WBS_MOVE );

	m_kEffTex[0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ArenaDisplayEffect01.bmp" ), 0xffff00ff, TRUE );
	m_kEffTex[1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"ArenaDisplayEffect02.bmp" ), 0xffff00ff, TRUE );
} 

//void CWndScoreContent::SetWndRect( CRect rectWnd, BOOL bOnSize )
//{
//	CWndNeuz::SetWndRect( rectWnd, bOnSize );
//	if( m_pWndMatchInfo )
//		m_pWndMatchInfo->Move( m_rectWindow.left, m_rectWindow.bottom + 2);
//}

//BOOL CWndScoreContent::Process( void )
//{
//	return CWndBase::Process( );
//}

BOOL CWndScoreContent::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndScoreContent::OnDraw( C2DRender* p2DRender ) 
{
	if( m_nAni > 1 )
		m_nAni = 0;

	if( m_bEffectChangedState )
	{
		static BOOL sbSwitch = TRUE;
		static float sfEffTime_MAX = 0.0f;
		m_fEffTime += _GetDeltaTimeSec( );

		if( m_fEffTime > 0.5f )
		{
			sbSwitch = !sbSwitch;
			sfEffTime_MAX += m_fEffTime;
			m_fEffTime = 0.0f;
		}

		if( sfEffTime_MAX > SCORE_TIME_MAX )
		{
			sfEffTime_MAX = 0.0f;
			sbSwitch = TRUE;
			m_bEffectChangedState = FALSE;
		}

		if( sbSwitch )
		{
			p2DRender->RenderTexture( CPoint(0,0), &m_kEffTex[m_nAni] );
			
			CWndStatic* pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_SITUATION );
			if( pStatic )
				p2DRender->RenderRect( pStatic->m_rectWindow, 0xffff0000 );
		}

	}
	else
		p2DRender->RenderTexture( CPoint(0,0), &m_kEffTex[m_nAni] );
	

	m_fDt += _GetDeltaTimeSec( );
	if( m_fDt > m_fMaxDt )
	{
	//	if( ( rand() % 5 ) == 0 )
	//		m_fMaxDt = ( rand() % 5 + 1 ) * 1.0f;
	//	else 
	//		m_fMaxDt = 0.5f;

		m_fDt = 0.0f;
		++m_nAni;
	}
}

void CWndScoreContent::UpdateData( const FLSnapshotEventArenaGameScoreboardNoti& kData )
{
	CWndStatic* pStatic = NULL;
	CString strBuffer;

	//경기상태
	//pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_SITUATION );
	//if( pStatic )
	//{
	//	int nTID = 0;
	//	switch( kData.byGameState )
	//	{
	//	case EVENT_ARENA_GAME_STATE_NONE:	nTID = TID_MMI_EVENTARENA_STANDBY;			break;
	//	case EVENT_ARENA_GAME_STATE_CALL:	nTID = TID_MMI_EVENTARENA_PLAYERCALL;		break;
	//	case EVENT_ARENA_GAME_STATE_JOIN:	nTID = TID_MMI_EVENTARENA_PLAYERENTER;		break;
	//	case EVENT_ARENA_GAME_STATE_PLAYING:	nTID = TID_MMI_EVENTARENA_PLAYERFIGHT;	break;
	//	default:
	//		FLASSERT( 0 ); break;
	//	}
	//	
	//	pStatic->SetTitle( prj.GetText( nTID ) );
	//}

	//경기 타입설정
	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_TONEMENT );
	if( pStatic )
	{
		strBuffer.Format( "%s( %s )", kData.szTonementName, kData.szTonementDesc );
		pStatic->SetTitle( strBuffer );
		pStatic->SetTileColor( 0xffffffff );
	}

	//팀이름 설정
	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDTEAMNAME );
	if( pStatic )
	{
		pStatic->SetTitle( kData.szRedTeamName );
		pStatic->SetTileColor( 0xffffffff );
	}

	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUETEAMNAME );
	if( pStatic )
	{
		pStatic->SetTitle( kData.szBlueTeamName );
		pStatic->SetTileColor( 0xffffffff );
	}
	
	//스코어 설정
	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDSCORE );
	if( pStatic )
	{
		strBuffer.Format( "%d", kData.nRedWinCount );
		pStatic->SetTitle( strBuffer );
		pStatic->SetFont( m_Theme.m_pFontWndTitle );
	}

	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUESCORE );
	if( pStatic )
	{
		strBuffer.Format( "%d", kData.nBlueWinCount );
		pStatic->SetTitle( strBuffer );
		pStatic->SetFont( m_Theme.m_pFontWndTitle );
	}

	//살아남은 인원
	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_REDSURVIVOR );
	if( pStatic )
	{
		strBuffer.Format( "%d", kData.nRedSurvivalMembers );
		pStatic->SetTitle( strBuffer );
	}

	pStatic = ( CWndStatic* )GetDlgItem( WIDC_ST_BLUESURVIVOR );
	if( pStatic )
	{
		strBuffer.Format( "%d", kData.nBlueSurvivalMembers );
		pStatic->SetTitle( strBuffer );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndMatchInfo
CWndMatchInfo::CWndMatchInfo( )
{
	m_fDelta = 0.0f;
}

CWndMatchInfo::~CWndMatchInfo( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMatchInfo::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EVENTARENA_MATCHINFO, WS_CHILD, CPoint( 0, 0 ), pWndParent );
} 

void CWndMatchInfo::OnInitialUpdate() 
{ 

	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndListBox* pListbox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( pListbox )
	{
		pListbox->m_nLineSpace = (int)( ( 20 - pListbox->GetFontHeight( ) ) * 0.5f );
	}

	// 윈도를 중앙으로 옮기는 부분.
	//MoveParentCenter();

	if( m_pParentWnd )
	{
		CRect rcParent = m_pParentWnd->m_rectWindow;
		CRect rcTarget = rcParent;

		rcTarget.right = rcTarget.left + m_rectWindow.Width();
		rcTarget.top = rcParent.bottom + 2 ;
		rcTarget.bottom = rcTarget.top + m_rectWindow.Height();

		SetWndRect( rcTarget, FALSE );
	}
}

BOOL CWndMatchInfo::Process( void )
{
	return TRUE;
}


BOOL CWndMatchInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1:
		Destroy( );
		break;

	default: 
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndMatchInfo::OnDraw( C2DRender* p2DRender ) 
{
	//타임아웃 기능 막음
	//if( IsVisible( ) )
	//	m_fDelta += _GetDeltaTimeSec( );

	//if( m_fDelta > 10.0f )
	//{
	//	Destroy( );
	//	return;
	//}

	//이미지, 승리팀, 전투시간, 레드생존자, 블루생존자
	CWndListBox* pList = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pList )
		return;
   
	CRect kRect = pList->m_rectWindow;

	const int nHeightGap = 4;
	int nIndex = 0;
	CString strBuffer;
	
	CTexture* pTexture = NULL;
	
	const char redfilename[] = "WndRedTeam01.tga";
	const char bluefilename[] = "WndBlueTeam01.tga";
	const char* pszFileName = NULL;

	for( BRL_Iter iter = m_vecLog.begin(); iter != m_vecLog.end(); ++iter )
	{
		T_BATTLE_RESULT_LOG& kData = (*iter);

		CPoint pt( kRect.left , kRect.top + (nHeightGap -5) + (nIndex)* 20 );

		//승자팀에 따라서 texture loading
		kData.nWinTeam == EVENT_ARENA_RED ? pszFileName = redfilename : 0;
		kData.nWinTeam == EVENT_ARENA_BLUE ? pszFileName = bluefilename : 0 ;

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), pszFileName ), 0xffff00ff );
	
		//Icon
		if( pTexture )
			p2DRender->RenderTexture( pt, pTexture );
		
		//Name
		p2DRender->TextOut( kRect.left + 30, kRect.top + nHeightGap + (nIndex)*20, kData.szWinTeamName, 0xffffffff );

		//전투시간
		int nSec = (int)kData.u64BattleTime;	//21억초를 넘으면? 막나가자는거지머
		int hh = 0;
		int mm = 0;
		int ss = 0;
		_GetHMS_bySec( nSec, hh, mm, ss );
		strBuffer.Format( "%02d:%02d", mm, ss );
		p2DRender->TextOut( kRect.left + 202, kRect.top + nHeightGap + (nIndex)*20, strBuffer.GetString(), 0xffffffff );

		//Red생존자
		strBuffer.Format( "%3d", kData.nRedSurvivalMembers );
		p2DRender->TextOut( kRect.left + 280, kRect.top + nHeightGap + (nIndex)*20, strBuffer.GetString(), 0xffffffff );

		//Blue생존자
		strBuffer.Format( "%3d", kData.nBlueSurvivalMembers );
		p2DRender->TextOut( kRect.left + 350, kRect.top + nHeightGap + (nIndex)*20, strBuffer.GetString(), 0xffffffff );

		++nIndex;
	}
}

void CWndMatchInfo::SetData( const std::vector< T_BATTLE_RESULT_LOG >& vecData )
{
	m_vecLog = vecData;

	//리스트 갱신
	CWndListBox* pListbox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListbox )
		return;

	pListbox->ResetContent( );

	if( m_vecLog.empty() )
		return;

	int nIndex = 0;
	for( BRL_Iter iter = m_vecLog.begin(); iter != m_vecLog.end(); ++iter )
	{
		T_BATTLE_RESULT_LOG& kLog = *iter;
		nIndex = pListbox->AddString( "" );
		pListbox->SetItemDataPtr( nIndex, &kLog );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
//EArenaSfxShow
EArenaSfxShow::EArenaSfxShow( )
{
	m_fDt = 0.0f;
}

EArenaSfxShow::~EArenaSfxShow( )
{
}

void EArenaSfxShow::Init( )
{
	//light
//	SetAmbient( 10, 10, 10 );
//	SetDiffuse( 10, 10, 10 );
	
	for( int i = 0; i < 3; ++i )
	{
		D3DXVECTOR3 vPos;
		vPos = D3DXVECTOR3( 1253, 103, 1261 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1258, 103, 1271 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1252, 103, 1282 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1241, 103, 1286 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1231, 103, 1281 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1226, 103, 1271 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1231, 103, 1260 );
		m_vTargetPoint.push_back( vPos );

		vPos = D3DXVECTOR3( 1242, 103, 1256 );
		m_vTargetPoint.push_back( vPos );
	}
}

BOOL EArenaSfxShow::Update( float fDt )
{
	if( m_vTargetPoint.empty() )
		return TRUE;

	m_fDt += fDt;


	if( m_fDt > 1.0f )
	{
		m_fDt = 0.0f;
		std::vector<D3DXVECTOR3>::iterator iter = m_vTargetPoint.end();
		--iter;

		D3DXVECTOR3 vPos = *iter;
		CreateSfx( g_Neuz.m_pd3dDevice, XI_INDEX( 1082, XI_NAT_FIRESHOWER01 ), vPos, NULL_ID );
	
		m_vTargetPoint.pop_back();
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//거래 중개 시스템
//----------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------------
//CWndConsignmentMarket
CWndConsignmentMarket::CWndConsignmentMarket( )
{
	m_dwCurButton = 0;
	m_dwCurPage = 0;
}

CWndConsignmentMarket::~CWndConsignmentMarket( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndConsignmentMarket::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADEBG, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndConsignmentMarket::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndEdit* pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	if( pWndEdit )
	{
		pWndEdit->AddWndStyle( EBS_READONLY );
	}
	
	MoveParentCenter();
} 

BOOL CWndConsignmentMarket::Process( void )
{
	m_kWndPage1.Process( );
//	m_kWndPage2.Process( );
//	m_kWndPage3.Process( );

	return CWndBase::Process( );
}

BOOL CWndConsignmentMarket::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
 	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndConsignmentMarket::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1: 
		m_dwCurButton = nID;
		m_dwCurPage = APP_TRADE_MAIN;
		UpdateVisablePage( );
		break;

	case WIDC_BUTTON2:
		{
			m_dwCurButton = nID;
			m_dwCurPage = APP_TRADE_REGISTER;
			UpdateVisablePage( );
		}
		break;

	case WIDC_BUTTON3:
		m_dwCurButton = nID;
		m_dwCurPage = APP_TRADE_MASSAGE;
		UpdateVisablePage( );
		break;

	case WIDC_BUTTON4:		//개수 확장
		g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_ITEMREGISTADDMESSAGE ) );
		break;

	default: break;

	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndConsignmentMarket::OnDraw( C2DRender* p2DRender ) 
{
}

BOOL CWndConsignmentMarket::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int a = 0;
	return TRUE;
}

void CWndConsignmentMarket::OnMouseWndSurface( CPoint point )
{
	int b = 0;
}

void CWndConsignmentMarket::UpdateVisablePage( )
{
	m_kWndPage1.OnPage( FALSE );
	m_kWndPage2.OnPage( FALSE );
	m_kWndPage3.OnPage( FALSE );

	DWORD dwBtnIDs[3] = { WIDC_BUTTON1, WIDC_BUTTON2, WIDC_BUTTON3 };

	CWndButton* pWndBtn = NULL;
	for( int i = 0; i < 3; ++i )
	{
		pWndBtn = ( CWndButton* )GetDlgItem( dwBtnIDs[ i ] );
		if( pWndBtn )
			pWndBtn->SetCheck( 0 );
	}

	if( m_dwCurButton > 0 )
	{
		pWndBtn = ( CWndButton* )GetDlgItem( m_dwCurButton );
		if( pWndBtn )
			pWndBtn->SetCheck( 2 );
	}

	switch( m_dwCurButton )
	{
	case WIDC_BUTTON1:		m_kWndPage1.OnPage( TRUE );		break;
	case WIDC_BUTTON2:		m_kWndPage2.OnPage( TRUE );		break;
	case WIDC_BUTTON3:		m_kWndPage3.OnPage( TRUE );		break;
	default : break;
	}
}

BOOL CWndConsignmentMarket::IsShowWndId( DWORD dwWndId )
{
	return ( m_dwCurPage == dwWndId );
}

void CWndConsignmentMarket::DoModalRegister( DWORD dwItemId )
{
	if( m_dwCurPage != m_kWndPage2.GetWndId() )
	{
		FLASSERT( 0 );
		return;
	}

	m_kWndPage2.CreateInputWnd( dwItemId );
}

void CWndConsignmentMarket::SetRegistCount( DWORD dwCurr, DWORD dwMax )
{
	DWORD dwCount = dwMax - dwCurr;

	if( dwCount > INT_MAX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("dwCount %X"), dwCount );
		return;
	}


	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( pWndEdit )
	{
		CString strBuffer;
		CWndCM_Sub1List::CStringFormat( strBuffer, (int)dwCount );
		pWndEdit->SetString( strBuffer.GetString( ) );
	}

	m_kWndPage2.SetRegCount( dwCurr, dwMax );
}

void CWndConsignmentMarket::InitCustom( )
{
	CRect rect = GetClientRect();
	rect.left += 0;
	rect.top += 40;
	
	m_kWndPage1.Create( WBS_CHILD | WBS_CHILDFRAME, rect, this, APP_TRADE_MAIN );
	m_kWndPage2.Create( WBS_CHILD | WBS_CHILDFRAME, rect, this, APP_TRADE_REGISTER );
	m_kWndPage3.Create( WBS_CHILD | WBS_CHILDFRAME, rect, this, APP_TRADE_MASSAGE );

	m_kWndPage1.Move( rect.left, rect.top );
	m_kWndPage2.Move( rect.left, rect.top );
	m_kWndPage3.Move( rect.left, rect.top );

	//no1 tab
	m_dwCurButton = WIDC_BUTTON1;
	m_dwCurPage = APP_TRADE_MAIN;
	UpdateVisablePage( );
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndCM_Sub1List

static const int CM_LIST_HEIGHT = 40;
CWndCM_Sub1List::CWndCM_Sub1List( )
{
	m_nCurPageButtonID = 0;
	m_nPageSet = 0;
	m_nCurrPage = 0;
	m_nPageMax = 0;
	m_dwSearchGrade = GT_NOMAL | GT_ULTRA | GT_UNIQUE | GT_BARUNA;
	m_nSearchMinLevel = 1;
	m_nSearchMaxLevel = 200;
	m_nSearchMinEnchantLevel = 0;
	m_nSearchMaxEnchantLevel = 20;

	m_bItem = FALSE;
	m_bLevel = FALSE;
	m_bPrice = FALSE;
	m_bEachPrice = FALSE;
	m_bSeller = FALSE;

	m_nSelected = -1;
	m_bReceivedData = FALSE;
	m_pMsgBox_Buy = NULL;
	m_fAlpha = 0.0f;

	m_bPushedSearch = FALSE;
}

CWndCM_Sub1List::~CWndCM_Sub1List( )
{
	AssDeleter< mapTreeDataContainer > ( ) ( m_mapTreeData );

	SAFE_DELETE( m_pMsgBox_Buy );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCM_Sub1List::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_MAIN, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndCM_Sub1List::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndComboBox* pCombo = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX1 );
	if( pCombo )
	{
		pCombo->AddWndStyle( EBS_READONLY );

		int nIndex = 0;
		nIndex = pCombo->AddString( prj.GetText( TID_MMI_TRADESYSTEM_ITEMSEARCHINCLUSION01 ) ); //"검색어포함ID필요" );
//		nIndex = pCombo->AddString( "검색어포함ID필요" ); 
		pCombo->SetItemData( nIndex, 0 );

		nIndex = pCombo->AddString( prj.GetText( TID_MMI_TRADESYSTEM_ITEMSEARCHACCORD01 ) ); //"두번째항목ID필요" );
//		nIndex = pCombo->AddString( "두번째항목ID필요" );
		pCombo->SetItemData( nIndex, 1 );


		pCombo->SetCurSel( 0 );
	}

	CWndStatic* pWndStatic = ( CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	if( pWndStatic )
		pWndStatic->SetTitle( "-" );

	CWndEdit* pEdit = NULL;
	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );	//레벨최소
	if( pEdit )
	{
		pEdit->AddWndStyle( EBS_NUMBER );
		pEdit->SetMaxStringNumber( 3 );
	//	pEdit->m_bForceDrawFrame = TRUE;
	}

	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT2 );	//레벨최대
	if( pEdit )
	{
		pEdit->AddWndStyle( EBS_NUMBER );
		pEdit->SetMaxStringNumber( 3 );
	//	pEdit->m_bForceDrawFrame = TRUE;
	}

	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT3 );	//제련최소
	if( pEdit )
	{
		pEdit->AddWndStyle( EBS_NUMBER );
		pEdit->SetMaxStringNumber( 3 );
	//	pEdit->m_bForceDrawFrame = TRUE;
	}

	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT5 ); //제련최대
	if( pEdit )
	{
		pEdit->AddWndStyle( EBS_NUMBER );
		pEdit->SetMaxStringNumber( 3 );
	//	pEdit->m_bForceDrawFrame = TRUE;
	}

	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT4 );	//검색어
	if( pEdit )
	{
		pEdit->SetMaxStringNumber( 20 );
	//	pEdit->m_bForceDrawFrame = TRUE;
	}

//	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_BUTTON23 );
//	pWndButton->EnableWindow( FALSE );

	//리스트박스에 들어갈 아이템의 높이 
	CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( pWndListBox )
	{
		pWndListBox->m_bOnItem = TRUE;
		pWndListBox->m_nLineSpace = (int)( ( CM_LIST_HEIGHT - pWndListBox->GetFontHeight( ) ) * 0.5f );
	}

	CWndTreeCtrl* pTree = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	if( pTree )
		pTree->m_bForceDrawFrame = FALSE;

	InitSearch( );
	SendPacket_Search( );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	MakeTreeItem( );
//	MakeListItem( );
	UpdatePageButton( );
	
	DelWndStyle( WBS_CAPTION );
} 

BOOL CWndCM_Sub1List::Process( void )
{
	return CWndBase::Process( );
}

BOOL CWndCM_Sub1List::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( m_pMsgBox_Buy && pWndBase == m_pMsgBox_Buy )
	{
		if( IDOK == nID )
		{
			//ok, sendpaket
			SendPacket_Buy( );
			m_pMsgBox_Buy->Destroy();
		}
		else if( IDCANCEL == nID )
		{
			m_pMsgBox_Buy->Destroy();
		}

	}
   
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndCM_Sub1List::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_LISTBOX1:
		{
			CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			m_nSelected	= pWndListBox->GetCurSel();

		//	if( m_nSelected >= 0 )
		//		PutTooltip( m_nSelected );

			if( WNM_ONITEM == message )
			{
				PutTooltip( *pLResult );
			}
			else if( WNM_DBLCLK == message )
			{
				CreateMessageBox_Buy( );
			}
		}
		break;

	case WIDC_CHECK1:	//등급 : 일반
	case WIDC_CHECK2:	//유니크
	case WIDC_CHECK3:	//얼터멋
	case WIDC_CHECK4:	//바루나
		UpdateCheckButton_Grade( );
		break;

	case WIDC_BUTTON3: //검색
		SendPacket_Search( );
		break;

	case WIDC_BUTTON4:	//초기화
		InitSearch( );
		break;

	case WIDC_BUTTON5:	//구입
		//구입 확정 메세지 박스 띄움
		CreateMessageBox_Buy( );
		break;

	case WIDC_BUTTON7: //페이지 앞으로
	//	if( m_nPageSet > 0 )
		{
			SendPacket_MovePageSet( PREVIOUS_PAGE_SET );
			UpdatePageButton( );
		}
		break;

	case WIDC_BUTTON18: //페이지 뒤로
		if( m_nPageSet < MAX_PAGE_SET )
		{
			SendPacket_MovePageSet( NEXT_PAGE_SET  );
			UpdatePageButton( );
		}
		break;

	case WIDC_BUTTON6:	//맨처음
		if( m_nCurrPage > 0 )
		{
			SendPacket_MovePageSet( FIRST_PAGE_SET  );
			UpdatePageButton( );
		}
		break;

	case WIDC_BUTTON19:		//맨뒤로
		if( m_nPageSet < MAX_PAGE_SET )
		{
			SendPacket_MovePageSet( LAST_PAGE_SET );
			UpdatePageButton( );
		}
		break;

	case WIDC_BUTTON8:		//page -0
	case WIDC_BUTTON9:
	case WIDC_BUTTON10:
	case WIDC_BUTTON11:
	case WIDC_BUTTON12:
	case WIDC_BUTTON13:
	case WIDC_BUTTON14:
	case WIDC_BUTTON15:
	case WIDC_BUTTON16:
	case WIDC_BUTTON17:		//page -9
		{
			m_nCurPageButtonID = nID;
	
			int nIndex = PageIdToIndex( );
			nIndex = m_nPageSet * MAX_PAGE + nIndex;	//페이지셑 = 10 으로계산한 최종인덱스

			SendPacket_Page( nIndex );
			UpdatePageButton( );
		}
		break;

	case WIDC_BUTTON2:	//아이템 버튼( 소팅 )
		m_bItem = !m_bItem;
		SendPacket_Search( nID );
		break;

	case WIDC_BUTTON20:	//레벨( 소팅 )
		m_bLevel = !m_bLevel;
		SendPacket_Search( nID );
		break;	

	case WIDC_BUTTON21:	//판매가격( 소팅 )
		m_bPrice = !m_bPrice;
		SendPacket_Search( nID );
		break;

	case WIDC_BUTTON22:	//개당판매가격( 소팅 )
		m_bEachPrice = !m_bEachPrice;
		SendPacket_Search( nID );
		break;

	case WIDC_BUTTON23:	//판매자( 소팅 ) - 지원안함
		m_bSeller = !m_bSeller;
		break;

	case WIDC_BUTTON24:	//새로고침
		SendPacket_Refresh( );
		break;

	case WIDC_COMBOBOX1:
		if( message == WNM_SELCHANGE )
		{
			CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
			int curSel = pCombo->GetCurSel();
		}
		break;

	default:
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCM_Sub1List::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pMsgBox_Buy )
	{
		SAFE_DELETE( m_pMsgBox_Buy );
	}
}

void CWndCM_Sub1List::OnMouseWndSurface( CPoint point )
{
}

void CWndCM_Sub1List::OnMouseMove(UINT nFlags, CPoint point)
{
	//마우스가 유효한 데이터를 가리키고 있지 않은경우 툴팁제거 
	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( pListBox )
	{
		if( FALSE == pListBox->IsInValidArea( point ) )
			g_toolTip.CancelToolTip( );
	}
}

void CWndCM_Sub1List::OnDraw( C2DRender* p2DRender ) 
{
	//void CWndGuildHousing::OnDraw( C2DRender* p2DRender ) 참고

	CWndListBox* pList = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pList )
		return;

	int nSelected = pList->GetCurSel( );
	CRect kRect = pList->m_rectWindow;

	//선택된 데이터
	FLOneSaleInfo* pSelectedData = NULL;
	if( nSelected > -1 )
		pSelectedData = ( FLOneSaleInfo*) pList->GetItemDataPtr( nSelected );
   
	const int nHeightGap = 20;
	int nIndex = 0;
	CString strBuffer;
	DWORD dwRoundColor = 0x001111ff;
	for( SaleDataIter iter = m_vecListData.begin(); iter != m_vecListData.end(); ++iter )
	{
		FLOneSaleInfo* pData = &(*iter);

		CPoint pt( kRect.left + 16, kRect.top + (nHeightGap-12) + (nIndex)*CM_LIST_HEIGHT );
		FLItemElem& kElem = pData->kItemElem;
		PT_ITEM_SPEC pProp = kElem.GetProp();
		
		if( NULL == pProp )
			pProp = g_xSpecManager->GetSpecItem( kElem.GetItemIndex( ) );

		if( NULL == pProp )
			continue;

		//Icon
		CTexture* pTexture = kElem.GetTexture( );
		if( !pTexture )
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), 0xffff00ff );
		}

		if( pTexture )
			p2DRender->RenderTexture( pt, pTexture );

	
		//count
		if( pProp->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 

			int nItemNum	= kElem.m_nItemNum;
			if( kElem.GetExtra() > 0 )
				nItemNum -= kElem.GetExtra();			
							
			TCHAR szTemp[ 32 ];
			FLSPrintf( szTemp, _countof( szTemp ), "%d", nItemNum );
			CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );

			m_p2DRender->TextOut( pt.x + 32 - size.cx, pt.y + 32 - size.cy, szTemp, CWndCM::COUNT_COLOR );
			p2DRender->SetFont( pOldFont ); 
		}
		
		//Name
		strBuffer = CString( pData->szItemName );
		int nReduceCount = 0;
		if( strBuffer.GetLength() > 20 )
		{
			for( nReduceCount = 0; nReduceCount < 18; )
			{
				if( IsDBCSLeadByte( strBuffer[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}

			strBuffer = strBuffer.Left( nReduceCount );
			strBuffer += CString( "..." );
		}
		p2DRender->TextOut( kRect.left + 60, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//level?
		strBuffer.Format( "%3d", pProp->dwLimitLevel1 );
		p2DRender->TextOut( kRect.left + 270, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//판매가격
		__int64 nTotalPrice = pData->nItemCostTot;// * kElem.m_nItemNum;
		strBuffer.Format( "%I64d", nTotalPrice );
		MakeMoneyStyle( strBuffer );
		p2DRender->TextOut( kRect.left + 340, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//개당 판매가격
		strBuffer.Format( "%d", static_cast<int>( pData->nItemCostTot / kElem.m_nItemNum ) );
		MakeMoneyStyle( strBuffer );
		p2DRender->TextOut( kRect.left + 460, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//판매자
		strBuffer = CString( pData->szPlayerName );
		if( strBuffer.GetLength() > 12 )
		{
			for( nReduceCount = 0; nReduceCount < 10; )
			{
				if( IsDBCSLeadByte( strBuffer[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}

			strBuffer = strBuffer.Left( nReduceCount );
			strBuffer += CString( ".." );
		}

		p2DRender->TextOut( kRect.left + 550, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//라인
		CPoint kLS( kRect.left + 1, kRect.top + nHeightGap + ( nIndex )*CM_LIST_HEIGHT +24 );
		CPoint kLE( kRect.right - 1, kRect.top + nHeightGap + ( nIndex )*CM_LIST_HEIGHT +24 );
		p2DRender->RenderLine( kLS, kLE, CWndCM::LINE_COLOR );

		//선택된 아이템 박스표시
		if( pSelectedData == pData )
		{
		//	m_fAlpha += _GetDeltaTimeSec( );
		//	if( m_fAlpha > 1.0f )
		//		m_fAlpha = 0.0f;

		//	DWORD dwAlpha = (DWORD) ( 255 * m_fAlpha );

		//	dwRoundColor |= ( dwAlpha << 24 );
			CRect renderBoxRect = kRect;

			renderBoxRect.left += 1;
			renderBoxRect.right -= 1;

			renderBoxRect.top = renderBoxRect.top + 5 + (nIndex)*CM_LIST_HEIGHT,
			renderBoxRect.bottom = renderBoxRect.top + 39;
	
			p2DRender->RenderFillRect( renderBoxRect, CWndCM::SELECT1_COLOR );
			p2DRender->RenderRect( renderBoxRect, CWndCM::SELECT2_COLOR );

		}

		++nIndex;
	}
	
}

void CWndCM_Sub1List::OnPage( BOOL bOn  )
{
	if( bOn )
	{
		CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
		if( pListBox )
		{
			pListBox->SetCurSel( -1 );
		}

		SetVisible( TRUE );
		SetFocus( );
	}
	else
	{
		SetVisible( FALSE );
	}
}

BOOL CWndCM_Sub1List::ReadTreeFile( const char* szFileName )
{
	CScript script;
	if( FALSE == script.Load( _T( szFileName ) ) )
		return FALSE;

	if( !m_mapTreeData.empty( ) )
		AssDeleter< mapTreeDataContainer > ( ) ( m_mapTreeData );
	
	// 루트는 프로그램에서 만들어줌
	TREEDATA* pRootData = new TREEDATA;
	pRootData->_dwKey = NULL_ID;
	pRootData->_dwParentKey = 0;
	pRootData->_dwType[0] = TYPE1_NONE;
	pRootData->_dwType[1] = TYPE2_NONE;
	pRootData->_strTitle = prj.GetText( TID_DIAG_0063 ); //전체
	m_mapTreeData.insert( mapTreeDataContainer::value_type( pRootData->_dwKey, pRootData ) );

	DWORD dwKey = script.GetNumber(); // ID;

	while( script.tok != FINISHED )
	{
		//읽어서 채울 Data
		TREEDATA* pData = new TREEDATA;
		pData->_dwKey = dwKey; // ID;
		FLASSERT( pData->_dwKey != 0 );
	
		script.GetToken();	// {

		int nBlock = 1;
	
		while( nBlock > 0 && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == _T( "{" ) )
			{
				++nBlock;
			}
			else if( script.Token == _T( "}" ) )
			{
				--nBlock;
			}
			else if( script.Token == _T( "parent" ) )
			{	
				script.GetToken(); // (
				pData->_dwParentKey = script.GetNumber( );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "title" ) )
			{
				script.GetToken(); // (
				CString strTemp = prj.GetLangScript( script );
				BOOL bOK = script.GetStringByStringKey( std::string( strTemp.GetString() ), strTemp );
				if( bOK )
					pData->_strTitle = strTemp;
			}
			else if( script.Token == _T( "type1" ) )
			{
				script.GetToken(); // (
				pData->_dwType[0] = script.GetNumber( );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
			else if( script.Token == _T( "type2" ) )
			{
				script.GetToken(); // (
				pData->_dwType[1] = script.GetNumber( );
				script.GetToken(); // )
				script.GetToken(); // ;
			}
		}

		//ok One data is done
		std::pair< mapTreeDataIter, bool > rst = m_mapTreeData.insert( mapTreeDataContainer::value_type( pData->_dwKey, pData ) );
		if( false == rst.second )
		{
			FLERROR_LOG( PROGRAM_NAME, "Duplicated id %d", pData->_dwKey );
			FLASSERT( 0 );
			SAFE_DELETE( pData );
		}

		dwKey = script.GetNumber(); // ID;
	}

	return TRUE;
}

void CWndCM_Sub1List::MakeTreeItem( )
{
	//최상위 key값은 NULL_ID
	//다음 상위 데이터들은 데이터파일에서 읽어들인 값이다.
	//데이터의 부모값이 0이라면 상위 루트란 뜻이고 최상위의 자식이 된다.

	CWndTreeCtrl* pWndTree = ( CWndTreeCtrl* )GetDlgItem( WIDC_TREECTRL1 );
	if( !pWndTree )
		return;

	pWndTree->DeleteAllItems( );

	//Read script fist!!
	BOOL bReadOK = ReadTreeFile( "propMediation.inc" );
	if( FALSE == bReadOK )
	{
		FLERROR_LOG( PROGRAM_NAME, "Read error" );
		return;
	}

	LPTREEELEM pRootTree = NULL;
	mapTreeDataIter RootIter = m_mapTreeData.find( NULL_ID );
	if( RootIter != m_mapTreeData.end() )
	{
		TREEDATA* pRootData = RootIter->second;
		pRootTree = pWndTree->InsertItem( NULL, pRootData->_strTitle, pRootData->_dwKey );
	}
	else 
	{
		FLERROR_LOG( PROGRAM_NAME, "cannot find root" );
		FLASSERT( 0 );

		return;
	}

	//1. 루트 등록
	mapTreeDataContainer mapCopyTreeData = m_mapTreeData;		//copy data

	for( mapTreeDataIter iter = mapCopyTreeData.begin(); iter != mapCopyTreeData.end();  /*none*/ )
	{
		TREEDATA* pData = iter->second;

		//최상위 부모는 pass
		if( NULL_ID == pData->_dwKey )
		{
			mapCopyTreeData.erase( iter++ );
			continue;
		}

		if( 0 == pData->_dwParentKey ) //상위루트
		{
			pWndTree->InsertItem( pRootTree, pData->_strTitle, pData->_dwKey );
			mapCopyTreeData.erase( iter++ );
			continue;
		}

		++iter;
	}

	//3. 자식등록. 루트의 제 1자식부터 등록하고 모두 없어질때까지 계속한다.
	BOOL bRecursive = TRUE;
	while( bRecursive )
	{
		int nSafeWhile = 0;
		for( mapTreeDataIter iter = mapCopyTreeData.begin(); iter != mapCopyTreeData.end();  /*none*/ )
		{
			TREEDATA* pData = iter->second;

			if( 0 == pData->_dwParentKey ) //루트는 이미 제거되어있어야 한다.
			{
				FLASSERT( 0 );
				continue;
			}

			LPTREEELEM pElem = pWndTree->FindTreeElem( pData->_dwParentKey );	//부모를 찾아서 삽입
			if( pElem )
			{
				pWndTree->InsertItem( pElem, pData->_strTitle, pData->_dwKey );

				mapCopyTreeData.erase( iter++ );
				continue;
			}

			++iter;
		}

		++nSafeWhile;

		if( nSafeWhile > 10 || mapCopyTreeData.empty() )		// 10회이상 순환하거나 다 했으면 종료
			bRecursive = FALSE;
	}

	FLASSERT( mapCopyTreeData.empty() );

	//CWndTreeCtrl* pWndTree = ( CWndTreeCtrl* )GetDlgItem( WIDC_TREECTRL1 );
	//if( pWndTree )
	//{
	//	LPTREEELEM pTreeA =  pWndTree->InsertItem( NULL, "TREE_a", 0 );
	//	pWndTree->InsertItem( pTreeA, "TREE_a_1", 0 );
	//	pWndTree->InsertItem( pTreeA, "TREE_a_2", 0 );

	//	LPTREEELEM pTreeB =  pWndTree->InsertItem( NULL, "TREE_b", 0 );
	//	pWndTree->InsertItem( pTreeB, "TREE_b_1", 0 );
	//	pWndTree->InsertItem( pTreeB, "TREE_b_2", 0 );
	//}
}

void CWndCM_Sub1List::MakeListItem( )
{
	//서버로부터 받은 데이터로 List data를 재구성한다.

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return;

	pListBox->ResetContent( );

	CEditString strBuffer;
	int nIndex = 0;
	for( SaleDataIter iter = m_vecListData.begin(); iter != m_vecListData.end(); ++iter )
	{
		FLOneSaleInfo* pData = &(*iter);
		if( NULL == pData )
			continue;

		strBuffer.Empty();
		g_WndMng.PutItemName( &pData->kItemElem, &strBuffer );
		strBuffer.Trim();
		g_WndMng.PutItemAbilityPiercing( &pData->kItemElem, &strBuffer, 0xffff0000 );
		FLStrcpy( pData->szItemName, _countof( pData->szItemName ), strBuffer.GetString() );
		nIndex = pListBox->AddString( "" );
		pListBox->SetItemDataPtr( nIndex, pData );
	}

	if( m_bPushedSearch )
	{
		if(	m_vecListData.empty() )
			g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_ITEMREGISTERNOTFOUND01 ) );

		m_bPushedSearch = FALSE;
	}

}

const int CWndCM_Sub1List::m_snButtonID[CWndCM_Sub1List::MAX_PAGE] = 
	{ WIDC_BUTTON8, WIDC_BUTTON9, WIDC_BUTTON10, WIDC_BUTTON11, WIDC_BUTTON12, 
		WIDC_BUTTON13, WIDC_BUTTON14, WIDC_BUTTON15, WIDC_BUTTON16, WIDC_BUTTON17 };

void CWndCM_Sub1List::UpdatePageButton( )
{
	CWndButton* pWndPageButton = NULL;
	CString str;
	int nRealPage = 0;
	for( int i = 0; i < MAX_PAGE; ++i )
	{
		pWndPageButton = ( CWndButton* )GetDlgItem( m_snButtonID[ i ] );
		if( pWndPageButton )
		{
			str.Format( "%d", ( m_nPageSet * MAX_PAGE + (i+1) ) );
			pWndPageButton->m_bForcedDrawTitle = TRUE;		//Tile방식이 아닌 이미지를 쓰는경우 타이틀 강제로 그려라!

			// 없는페이지는 페이지번호 생략
			nRealPage = m_nPageSet * 10 + i;
			if( nRealPage > m_nPageMax )
			{
				pWndPageButton->SetTitle( "" );
				pWndPageButton->EnableWindow( FALSE );
			}
			else
			{
				pWndPageButton->SetTitle( str );
				pWndPageButton->EnableWindow( TRUE );
			}

			pWndPageButton->SetCheck( 0 );
		}
	}

	pWndPageButton = ( CWndButton* )GetDlgItem( m_nCurPageButtonID );
	if( pWndPageButton )
	{
		pWndPageButton->SetCheck( 2 );
		pWndPageButton->SetTileColor( 0xffff0000 );
	}

}

void CWndCM_Sub1List::UpdateCheckButton_Grade( )
{
	m_dwSearchGrade = 0;

	CWndButton* pCheck = NULL;
	pCheck = ( CWndButton* )GetDlgItem( WIDC_CHECK1 );
	if( pCheck )
		pCheck->GetCheck( ) ? m_dwSearchGrade |= GT_NOMAL : 0;

	pCheck = ( CWndButton* )GetDlgItem( WIDC_CHECK2 );
	if( pCheck )
		pCheck->GetCheck( ) ? m_dwSearchGrade |= GT_UNIQUE : 0;

	pCheck = ( CWndButton* )GetDlgItem( WIDC_CHECK3 );
	if( pCheck )
		pCheck->GetCheck( ) ? m_dwSearchGrade |= GT_ULTRA : 0;

	pCheck = ( CWndButton* )GetDlgItem( WIDC_CHECK4 );
	if( pCheck )
		pCheck->GetCheck( ) ? m_dwSearchGrade |= GT_BARUNA : 0;
}

int CWndCM_Sub1List::GetLastPageSet( )
{
	int nSet = m_nPageMax / MAX_PAGE;
	return nSet; 
}

int CWndCM_Sub1List::PageIdToIndex( )
{
	//페이지버튼의 ID를 0부터 시작하는 index로 
	for( int i = 0; i < MAX_PAGE; ++i )
	{
		if( m_nCurPageButtonID == m_snButtonID[ i ] )
			return i;
	}

	return -1;
}

BOOL CWndCM_Sub1List::InitSearch( )
{
	CWndButton* pWndBtn = NULL;

	DWORD dwCheckID[ 4 ] = { WIDC_CHECK1, WIDC_CHECK2, WIDC_CHECK3, WIDC_CHECK4 };

	//등급
	for( int i = 0; i < 4; ++i )
	{
		pWndBtn = (CWndButton*)GetDlgItem( dwCheckID[i] );
		if( pWndBtn )
			pWndBtn->SetCheck( 1 );
	}

	UpdateCheckButton_Grade( );

	CString strBuffer;
	
	//레벨구간
	CWndEdit* pWndEdit = NULL;
	pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( pWndEdit )
	{
		CStringFormat( strBuffer, m_nSearchMinLevel );
		pWndEdit->SetString( strBuffer );
	}

	pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT5 );
	if( pWndEdit )
	{
		CStringFormat( strBuffer, m_nSearchMaxLevel );
		pWndEdit->SetString( strBuffer );
	}

	//제련구간
	pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT2 );
	if( pWndEdit )
	{
		CStringFormat( strBuffer, m_nSearchMinEnchantLevel );
		pWndEdit->SetString( strBuffer );
	}

	pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT3 );
	if( pWndEdit )
	{
		CStringFormat( strBuffer, m_nSearchMaxEnchantLevel );
		pWndEdit->SetString( strBuffer );
	}

	//검색어
	pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT4 );
	if( pWndEdit )
	{
		pWndEdit->SetString( "" );
	}
	
	//combo
	CWndComboBox* pCombo = ( CWndComboBox* )GetDlgItem( WIDC_COMBOBOX1 );
	if( pCombo )
		pCombo->SetCurSel( 0 );

	return TRUE;
}

void CWndCM_Sub1List::CStringFormat( CString& strBuffer, int nNum )
{
	strBuffer.Format( "%d", nNum );
}

void CWndCM_Sub1List::SetPage( const int nPage )
{
	int nPageSet = nPage / MAX_PAGE;
	int nPageIndex = nPage % MAX_PAGE;

	m_nPageSet = nPageSet;
	m_nCurPageButtonID = m_snButtonID[ nPageIndex ];

	FLASSERT( nPageIndex < MAX_PAGE );

	UpdatePageButton();
}

void CWndCM_Sub1List::CreateMessageBox_Buy( )
{
	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return;

	if( m_nSelected < 0 )
		return;

	FLOneSaleInfo* pData = (FLOneSaleInfo*)pListBox->GetItemDataPtr( m_nSelected );
	if( NULL == pData )
		return;

	SAFE_DELETE( m_pMsgBox_Buy );

	m_pMsgBox_Buy = new CWndMessageBox;
	CString strBuffer;
	strBuffer.Format( prj.GetText( TID_MMI_TRADESYSTEM_BUYTEXT04 ), pData->szItemName, pData->nItemCostTot );
	if( FALSE == m_pMsgBox_Buy->Initialize( strBuffer.GetString(), this, MB_OKCANCEL ) )
	{
		SAFE_DELETE( m_pMsgBox_Buy );
		return;
	}

	m_pMsgBox_Buy->AddWndStyle( WBS_KEY );
	m_pMsgBox_Buy->AddWndStyle( WBS_MODAL );
	m_pMsgBox_Buy->AddWndStyle( WBS_TOPMOST );

}

BOOL CWndCM_Sub1List::PutTooltip( const int nListIndex )
{
	CPoint point2 = GetMousePoint( );

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return FALSE;

	FLOneSaleInfo* pData = ( FLOneSaleInfo* )pListBox->GetItemDataPtr( nListIndex );
	if( NULL == pData )
		return FALSE;

	CRect kRectBox = pListBox->m_rectWindow;
	CRect kRectItem = pListBox->GetItemRect( nListIndex );

	kRectBox.top += kRectItem.top;

	ClientToScreen( &point2 );
	ClientToScreen( &kRectBox );

	FLItemElem* pItemElem = &pData->kItemElem;
	g_WndMng.PutToolTip_Item( pItemElem, point2, &kRectBox, -1 );
		
	return TRUE;
}

void CWndCM_Sub1List::SetTotalItemNum( const int nNum )
{
	CWndStatic* pStatic = ( CWndStatic* )GetDlgItem( WIDC_STATIC4 );
	if( pStatic )
	{
		CString strTotal;
		strTotal.Format( "%d", nNum );
		pStatic->SetTitle( strTotal );
	}
}

void CWndCM_Sub1List::SetListData( const FLPacket_SearchAck::SaleInfoVec& vecData )
{
	m_vecListData = vecData;
	
	//리스트 갱신
	MakeListItem( );
	m_bReceivedData = TRUE;
}

void CWndCM_Sub1List::SetPageInfo( int nCurrPage, int nMaxPage )
{
	SetPage( nCurrPage );

	m_nCurrPage = nCurrPage;
	m_nPageMax = nMaxPage;

	UpdatePageButton();
}

void CWndCM_Sub1List::SendPacket_Refresh( )
{
	g_DPlay.SendPacket( &m_kOldPacket );
}

void CWndCM_Sub1List::MakePacketData( FLPacket_SearchReq& kPacket )
{
	CWndEdit* pEdit = NULL;

	//레벨 
	int nLevelMin = -1;
	int nLevelMax = -1;
	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	if( pEdit )
		pEdit->GetEditNum( nLevelMin );
	
	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT5 );
	if( pEdit )
		pEdit->GetEditNum( nLevelMax );

	if( nLevelMin < 0 || nLevelMax < 0 )
	{
		g_WndMng.OpenMessageBox( "min level or max level is wrong" );
		return;
	}

	//제련레벨
	int nEnchantLevelMin = -1;
	int nEnchantLevelMax = -1;

	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT2 );
	if( pEdit )
		pEdit->GetEditNum( nEnchantLevelMin );

	pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT3 );
	if( pEdit )
		pEdit->GetEditNum( nEnchantLevelMax );
	
	//검색어
	CString strSearchString;
	int nComboSelected = -1;
	CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	if( pCombo )
		nComboSelected = pCombo->GetCurSel( );

	if( nComboSelected != -1 )
	{
		//무엇인가 선택됐음.
		pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT4 );
		if( pEdit )
			strSearchString = pEdit->GetString( );
	}

	DWORD dwType1 = TYPE1_NONE;
	DWORD dwType2 = TYPE2_NONE;

	//type ( from treectrl )
	CWndTreeCtrl* pTree = (CWndTreeCtrl*)GetDlgItem( WIDC_TREECTRL1 );
	if( pTree )
	{
		LPTREEELEM pElem = pTree->GetCurSel( );
		if( pElem )
		{
			mapTreeDataIter iter = m_mapTreeData.find( pElem->m_dwData );
			if( iter != m_mapTreeData.end() )
			{
				TREEDATA* pData = iter->second;
				dwType1 = pData->_dwType[0];
				dwType2 = pData->_dwType[1];
			}
			
		}
	}

	kPacket.m_eOrderingOption = FLPacket_SearchReq::E_NONE;
	kPacket.m_bASC = TRUE;
	FLStrcpy( kPacket.m_szSearchName, _countof( kPacket.m_szSearchName ), strSearchString.GetString( ) );
	kPacket.m_bSearchBySameName = ( nComboSelected == 1 ? TRUE : FALSE );
	kPacket.m_arrGrade[ 0 ] = ( m_dwSearchGrade & GT_NOMAL ? ITEM_GRADE_NORMAL : ITEM_GRADE_NONE );
	kPacket.m_arrGrade[ 1 ] = ( m_dwSearchGrade & GT_UNIQUE ? ITEM_GRADE_UNIQUE : ITEM_GRADE_NONE );
	kPacket.m_arrGrade[ 2 ] = ( m_dwSearchGrade & GT_ULTRA ? ITEM_GRADE_ULTIMATE : ITEM_GRADE_NONE );
	kPacket.m_arrGrade[ 3 ] = ( m_dwSearchGrade & GT_BARUNA ? ITEM_GRADE_BARUNA : ITEM_GRADE_NONE );
	kPacket.m_dwPage = 0;
	kPacket.m_nLimitLevelMin = nLevelMin;
	kPacket.m_nLimitLevelMax = nLevelMax;
	kPacket.m_nAbilityOptionMin = nEnchantLevelMin;
	kPacket.m_nAbilityOptionMax = nEnchantLevelMax;
	kPacket.m_dwItemType1 = dwType1;
	kPacket.m_dwItemType2 = dwType2;
}

BOOL CWndCM_Sub1List::SendPacket_Search( DWORD dwBtnId, int nPage )
{
	FLPacket_SearchReq kPacket;

	if( 0 != dwBtnId || -1 != nPage )
	{
		//검색이 최소한번 진행된경우만 지원되는 기능임
		if( FALSE == m_bReceivedData )
		{
			g_WndMng.OpenMessageBox( "검색을 진행한후 이용할수있습니다 ID필요" );
			return FALSE;
		}
	}

	//소팅요청 
	if( 0 != dwBtnId )
	{
		kPacket = m_kOldPacket;
		switch( dwBtnId )
		{
		case WIDC_BUTTON2: //아이템 버튼
			kPacket.m_eOrderingOption = FLPacket_SearchReq::E_ITEM_NAME;
			kPacket.m_bASC = m_bItem;
			break;

		case WIDC_BUTTON20: //레벨
			kPacket.m_eOrderingOption = FLPacket_SearchReq::E_ITEM_LEVEL;
			kPacket.m_bASC = m_bLevel;
			break;

		case WIDC_BUTTON21: //판매가격
			kPacket.m_eOrderingOption = FLPacket_SearchReq::E_PRICE;
			kPacket.m_bASC = m_bPrice;
			break;

		case WIDC_BUTTON22: //개당 판매가격
			kPacket.m_eOrderingOption = FLPacket_SearchReq::E_ONE_PRICE;
			kPacket.m_bASC = m_bEachPrice;
			break;

		case WIDC_BUTTON23: //판매자
			//no func
			break;
		}

		m_kOldPacket = kPacket;
	}
	else if( -1 != nPage )
	{
		//페이지 요청
		kPacket = m_kOldPacket;
		kPacket.m_dwPage = nPage;

		m_kOldPacket = kPacket;
	}
	else
	{
		//검색버튼을 이용한경우 ( no sorting, no page )
		MakePacketData( kPacket );
	
		//검색버튼을 눌러서 검색을 실시한 경우, 해당검색옵션을 저장해놔야함.
		m_kOldPacket = kPacket;

		//검색버튼을 눌른경우 아이템이 없다면 메세지 출력용
		m_bPushedSearch = TRUE;
	}
	
	g_DPlay.SendPacket( &kPacket );
	
	return TRUE;
}

BOOL CWndCM_Sub1List::SendPacket_Buy( )
{
	CWndListBox* pList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( pList )
	{
		int nItem = pList->GetCurSel();
		if( nItem < 0 )
		{
			//ASSERT( 0 );
			return FALSE;
		}
        
		FLOneSaleInfo* pData = ( FLOneSaleInfo* )pList->GetItemDataPtr( nItem );
		if( pData )
		{
			FLPacket_BuyItemReq kPacket;
			kPacket.m_u64SaleSRL = pData->u64SaleSRL;

			g_DPlay.SendPacket( &kPacket );
		}

	}
	return TRUE;
}

BOOL CWndCM_Sub1List::SendPacket_Page( const DWORD dwPage )
{
	if( (int)dwPage > m_nPageMax )
		return FALSE;

	m_kOldPacket.m_dwPage = dwPage;
	g_DPlay.SendPacket( &m_kOldPacket );

	return TRUE;
}

void CWndCM_Sub1List::SendPacket_MovePageSet( const int nType )
{
	int nOldPageSet = m_nPageSet;
	int nOldCurrPageButtonID = m_nCurPageButtonID;

	switch( nType )
	{
	case FIRST_PAGE_SET :	//맨앞
		m_nPageSet = 0;	
		m_nCurPageButtonID = m_snButtonID[0];
		break;	

	case PREVIOUS_PAGE_SET : //한셑앞
	//	m_nPageSet -= 1;
		{
			int nIndex = PageIdToIndex( );
			if( 0 == nIndex )
			{
				//젤 앞이면 앞페이지로
				if( m_nPageSet > 0 )
				{
					m_nPageSet -= 1;
					m_nCurPageButtonID = m_snButtonID[ MAX_PAGE -1 ];
				}

			}else if( 0 < nIndex )
			{
				m_nCurPageButtonID = m_snButtonID[ nIndex - 1 ];
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "error index %d" ), nIndex  );
				break;
			}
		}
		break;	

	case NEXT_PAGE_SET :	//한셑뒤
	//	m_nPageSet += 1;			
		{
			int nIndex = PageIdToIndex( );
			if( 9 == nIndex )
			{
				if( m_nPageSet < m_nPageMax )
				{
					m_nPageSet += 1;
					m_nCurPageButtonID = m_snButtonID[0];
				}
			}
			else if( 0 <= nIndex && 9 > nIndex )
			{
				m_nCurPageButtonID = m_snButtonID[ nIndex + 1 ];
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "error index %d" ), nIndex );
				break;
			}
		}
		break;  

	case LAST_PAGE_SET :	//맨뒤
		{
			m_nPageSet = MAX_PAGE_SET;
			m_nCurPageButtonID = m_snButtonID[0];

			m_nPageSet = (int)(m_nPageMax / MAX_PAGE);
			int nMax = m_nPageMax - ( (int)(m_nPageMax / MAX_PAGE) * MAX_PAGE );
			m_nCurPageButtonID = m_snButtonID[nMax];
		}
		break;  

	default:
		FLASSERT( 0 );
		return;
	}

	if( m_nPageSet < 0 || m_nPageSet > MAX_PAGE_SET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("m_nPageSet is %d"), m_nPageSet );

		m_nPageSet = nOldPageSet;
		m_nCurPageButtonID = nOldCurrPageButtonID;
		return;
	}
	
	//페이지 정보요청
	int nIndex = PageIdToIndex( );
	nIndex = m_nPageSet * MAX_PAGE + nIndex;	//페이지셑 = 10 으로계산한 최종인덱스
	BOOL bOK  = SendPacket_Page( nIndex );

	if( FALSE == bOK )
	{
		m_nPageSet = nOldPageSet;
		m_nCurPageButtonID = nOldCurrPageButtonID;

//		FLASSERT( 0 );
//		FLERROR_LOG( PROGRAM_NAME, _T( "nIndex" ), nIndex );
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndMediation_Sub2Register
CWndCM_Sub2Registration::CWndCM_Sub2Registration( )
{
	m_pWndMsgBox_CancelReg = NULL;
	m_pWndMsgBox_ConfirmReg = NULL;
	m_pWndInput = NULL;

	m_dwRegItemId = NULL_ID;
	m_dwMaxRegCount = 0;
	m_dwCurrRegCount = 0;
}

CWndCM_Sub2Registration::~CWndCM_Sub2Registration( )
{
	SAFE_DELETE( m_pWndMsgBox_CancelReg );
	SAFE_DELETE( m_pWndMsgBox_ConfirmReg );
	SAFE_DELETE( m_pWndInput );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCM_Sub2Registration::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_REGISTER, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndCM_Sub2Registration::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	//리스트박스에 들어갈 아이템의 높이 
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( pWndListBox )
	{
		pWndListBox->m_bOnItem = TRUE;
		pWndListBox->m_nLineSpace = (int)( ( CM_LIST_HEIGHT - pWndListBox->GetFontHeight( ) ) * 0.5f );
	}

	CWndStatic* pWndTotal = ( CWndStatic* )GetDlgItem( WIDC_STATIC4 );
	if( pWndTotal )
		pWndTotal->AddWndStyle( WSS_MONEY );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	CWndStatic* pWnd = ( CWndStatic* )GetDlgItem( WIDC_STATIC3 );
	if( pWnd )
		pWnd->SetTitle( "" );

} 

//BOOL CWndConsignmentMarket::Process( void )
//{
//	return CWndBase::Process( );
//}

BOOL CWndCM_Sub2Registration::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	//등록해제 확인
	if( m_pWndMsgBox_CancelReg && pWndBase == m_pWndMsgBox_CancelReg )
	{
		if( IDOK == nID )
		{
			//ok, sendpaket
			SendPacket_CancelReg( );
			m_pWndMsgBox_CancelReg->Destroy();
		}
		else if( IDCANCEL == nID )
		{
			m_pWndMsgBox_CancelReg->Destroy();
		}

	}
	//등록확인
	else if( m_pWndMsgBox_ConfirmReg && pWndBase == m_pWndMsgBox_ConfirmReg )
	{
		//입력창이 열린상태고, 그상태에서 확인물어봤으므로 확인인경우 입력창까지 닫아줘야한다.
		if( IDOK == nID )
		{
			if( m_pWndInput )
				m_pWndInput->Destroy( );

			m_pWndMsgBox_ConfirmReg->Destroy( );

			//ok, sendpacket
			SendPakcet_Registration( );
		}
		else if( IDCANCEL == nID )
		{
			m_pWndMsgBox_ConfirmReg->Destroy( );
		}
	}
   
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndCM_Sub2Registration::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	//2 : DBL CLICK
	switch( nID )
	{
	case WIDC_BUTTON1:	//아이템 ( 소팅 )
		break;

	case WIDC_BUTTON2:	//레벨
		break;

	case WIDC_BUTTON3:	//판매가격
		break;

	case WIDC_BUTTON4:	//개당 판매 가격
		break;

	case WIDC_BUTTON5:	//판매 마감 기간
		break;

	case WIDC_BUTTON6:	//취소
		break;

	case WIDC_BUTTON7:	//닫기
		break;

	case WIDC_EDIT1:
		break;

	case WIDC_LISTBOX1:
		if( message == WNM_DBLCLK || message == WNM_SELCANCEL ) // 지멋대로구만.
		{
			CreateMsgBox_CancelReg( );
		}
		else if( WNM_ONITEM == message )
		{
			PutTooltip( *pLResult );
		}
		break;

	case APP_TRADE_TRADE:
		if( WIDC_BUTTON13 == message ) //확인
		{
			if( m_pWndInput )
			{
				__int64 n64TotalPrice = m_pWndInput->GetTotalPrice( );

				if( n64TotalPrice <= MAX_PEYNA_FOR_CLIENT )
					SendPacket_Tax( (int)n64TotalPrice );
			}
		}
		else if( WIDC_BUTTON14 == message ) //취소
		{
			m_pWndInput->Destroy( );
		}
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCM_Sub2Registration::OnDraw( C2DRender* p2DRender ) 
{
	CWndListBox* pList = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pList )
		return;

	int nSelected = pList->GetCurSel( );
	CRect kRect = pList->m_rectWindow;

	FLOneSaleInfo* pSelectedData = NULL;
	if( nSelected > -1 )
		pSelectedData = ( FLOneSaleInfo*) pList->GetItemDataPtr( nSelected );

	const int nHeightGap = 20;
	int nIndex = 0;
	CString strBuffer;

	SaleDataIter iter = m_vecListData.begin();

	//스크롤위치에 맞게 최초 iterator set position
	int nScrollPos = pList->GetScrollPos( );
	if( nScrollPos > 0 )
	{
		if( (int)m_vecListData.size() <= nScrollPos )
		{
			FLERROR_LOG( PROGRAM_NAME, "error nScrollPos %d", nScrollPos );
			pList->SetScrollPos( 0 );
			nScrollPos = 0;
		}
		else
		{
			for( int i = 0; i < nScrollPos; ++i )
				++iter;
		}
	}

	for( /*none*/; iter != m_vecListData.end(); ++iter )
	{
		FLOneSaleInfo* pData = &(*iter);
		//FLOneSaleInfo* pData = *iter;

		CPoint pt( kRect.left + 16, kRect.top + 10 + (nIndex)*CM_LIST_HEIGHT );
		FLItemElem& kElem = pData->kItemElem;

		//Icon
		CTexture* pTexture = kElem.GetTexture();
		PT_ITEM_SPEC pProp = kElem.GetProp();

		if( NULL == pProp )
			pProp = g_xSpecManager->GetSpecItem( kElem.GetItemIndex( ) );

		if( NULL == pProp )
			continue;

		if( NULL == pTexture )
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), 0xffff00ff );

		if( pTexture )
			p2DRender->RenderTexture( pt, pTexture );

		//count
		if( pProp->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 

			int nItemNum	= kElem.m_nItemNum;
			if( kElem.GetExtra() > 0 )
				nItemNum -= kElem.GetExtra();			
							
			TCHAR szTemp[ 32 ];
			FLSPrintf( szTemp, _countof( szTemp ), "%d", nItemNum );
			CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );

			m_p2DRender->TextOut( pt.x + 32 - size.cx, pt.y + 32 - size.cy, szTemp, CWndCM::COUNT_COLOR );
			p2DRender->SetFont( pOldFont ); 
		}
		
		//Name
		strBuffer = CString( pData->szItemName );
		if( strBuffer.GetLength() > 20 )
		{
			int nReduceCount = 0;
			for( nReduceCount = 0; nReduceCount < 18; )
			{
				if( IsDBCSLeadByte( strBuffer[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}

			strBuffer = strBuffer.Left( nReduceCount );
			strBuffer += CString( "..." );
		}
		p2DRender->TextOut( kRect.left + 60, kRect.top + 20 + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//level?
		strBuffer.Format( "%3d", pProp->dwLimitLevel1 );
		p2DRender->TextOut( kRect.left + 270, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//판매가격
		__int64 nTotalPrice = pData->nItemCostTot;// * kElem.m_nItemNum;
		strBuffer.Format( "%I64d", nTotalPrice );
		MakeMoneyStyle( strBuffer );
		p2DRender->TextOut( kRect.left + 360, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//개당 판매가격
		strBuffer.Format( "%d", static_cast<int>( pData->nItemCostTot / kElem.m_nItemNum ) );
		MakeMoneyStyle( strBuffer );
		p2DRender->TextOut( kRect.left + 500, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//판매마감 시간
		//남은시간이 필요함.
		int nD = 0, nH = 0, nM = 0;
		_GetDHM_bySec( pData->dwRemainSec, nD, nH, nM );

		strBuffer.Format( "%02d day(s) %02d:%02d", nD, nH, nM );

		if( pData->dwRemainSec > 0 )
			p2DRender->TextOut( kRect.left + 620, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );
		else 
			p2DRender->TextOut( kRect.left + 620, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR_WARNING );

		//라인
		CPoint kLS( kRect.left + 1, kRect.top + nHeightGap + ( nIndex )*CM_LIST_HEIGHT +24 );
		CPoint kLE( kRect.right - 4, kRect.top + nHeightGap + ( nIndex )*CM_LIST_HEIGHT +24 );
		p2DRender->RenderLine( kLS, kLE, CWndCM::LINE_COLOR );

		//선택된 아이템 박스표시
		if( pSelectedData == pData )
		{

			CRect renderBoxRect = kRect;

			renderBoxRect.left += 1;
			renderBoxRect.right -= 12;

			renderBoxRect.top = renderBoxRect.top + 5 + (nIndex)*CM_LIST_HEIGHT,
			renderBoxRect.bottom = renderBoxRect.top + 39;
	
			p2DRender->RenderFillRect( renderBoxRect, CWndCM::SELECT1_COLOR );
			p2DRender->RenderRect( renderBoxRect, CWndCM::SELECT2_COLOR );
		}

		++nIndex;

		if( MAX_ITEMCOUNT_PAGE <= nIndex )
			break;

	}
}

BOOL CWndCM_Sub2Registration::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	//무효하면 pass
	if( pShortcut->m_dwData == 0 && pShortcut->m_dwId == 0 )
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_ITEMNOTREGISTER01 ) );
		return FALSE;
	}

	// 아이템이 인벤토리에서 왔는가?
	CWndBase* pWndFrame = NULL;
	if( pShortcut->m_pFromWnd )
		pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();

	if( NULL == pWndFrame )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_ITEMREGIST02 ) ); //"인벤토리아이템만 등록가능 ID필요" );		
		return FALSE;
	}
	
	FLItemElem* pTempElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( !pTempElem )
		return FALSE;

	CreateInputWnd( pShortcut->m_dwId );

	return TRUE;
}

void CWndCM_Sub2Registration::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndInput )
	{
		SAFE_DELETE( m_pWndInput );
	}else if( m_pWndMsgBox_CancelReg == pWndChild )
	{
		SAFE_DELETE( m_pWndMsgBox_CancelReg );
	}else if( m_pWndMsgBox_ConfirmReg == pWndChild )
	{
		SAFE_DELETE( m_pWndMsgBox_ConfirmReg );
	}
}

void CWndCM_Sub2Registration::OnMouseMove(UINT nFlags, CPoint point)
{
	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( pListBox )
	{
		if( FALSE == pListBox->IsInValidArea( point ) )
			g_toolTip.CancelToolTip( );
	}
}

void CWndCM_Sub2Registration::OnPage( BOOL bOn  )
{
	if( bOn )
	{
		FLPacket_RegistedInfoListReq kPacket;
		g_DPlay.SendPacket( &kPacket );

		SetVisible( TRUE );
		CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
		if( pListBox )
		{
			pListBox->SetCurSel( -1 );
		}

		SetFocus( );
	}
	else
	{
		SetVisible( FALSE );
	}
}

void CWndCM_Sub2Registration::CreateInputWnd( DWORD dwItemId )
{
	//입력창을 띄워야 한다.
	SAFE_DELETE( m_pWndInput );

	m_pWndInput = new CWndInputNumCommon2;
	m_pWndInput->Initialize( this );
	m_pWndInput->SetItem( dwItemId );

	m_dwRegItemId = dwItemId;
}

void CWndCM_Sub2Registration::CreateMsgBox_CancelReg( )
{
	//등록해제 확인 메세지박스 생성
	CWndListBox* pListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return;

	int nSel = pListBox->GetCurSel( );
	FLOneSaleInfo* pData = (FLOneSaleInfo*) pListBox->GetItemDataPtr( nSel );
	if( NULL == pData )
		return;

	CString strBuffer;
//	strBuffer.Format( "%s 를 해제할려고요?", pData->szItemName );
	strBuffer.Format( prj.GetText( TID_MMI_TRADESYSTEM_CANCELTEXT01 ) , pData->szItemName );

	//해제하시겠습니까?
	SAFE_DELETE( m_pWndMsgBox_CancelReg );
	m_pWndMsgBox_CancelReg = new CWndMessageBox;
	if( FALSE == m_pWndMsgBox_CancelReg->Initialize( strBuffer.GetString(), this, MB_OKCANCEL ) )
	{
		SAFE_DELETE( m_pWndMsgBox_CancelReg );
	}

}

void CWndCM_Sub2Registration::CreateMsgBox_ConfirmReg( const int nTax )
{
	SAFE_DELETE( m_pWndMsgBox_ConfirmReg );

	CString strCommission;
	strCommission.Format( "%d", nTax );
	MakeMoneyStyle( strCommission );

	FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( m_dwRegItemId );
	if( NULL == pItem )
		return;

	CString strBuffer;
	strBuffer.Format( prj.GetText( TID_MMI_TRADESYSTEM_ITEMREGIST01 ), pItem->GetName().GetString(), nTax );		//등록수수료%d

	m_pWndMsgBox_ConfirmReg = new CWndMessageBox;
	m_pWndMsgBox_ConfirmReg->Initialize( strBuffer.GetString(), this, MB_OKCANCEL );
}

BOOL CWndCM_Sub2Registration::PutTooltip( const int nListIndex )
{
	CPoint point2 = GetMousePoint( );

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return FALSE;

	FLOneSaleInfo* pData = ( FLOneSaleInfo* )pListBox->GetItemDataPtr( nListIndex );
	if( NULL == pData )
		return FALSE;

	CRect kRectBox = pListBox->m_rectWindow;
	CRect kRectItem = pListBox->GetItemRect( nListIndex );

	kRectBox.top += kRectItem.top;

	ClientToScreen( &point2 );
	ClientToScreen( &kRectBox );

	FLItemElem* pItemElem = &pData->kItemElem;
	g_WndMng.PutToolTip_Item( pItemElem, point2, &kRectBox, -1 );
		
	return TRUE;
}

void CWndCM_Sub2Registration::SetListData( const FLPacket_RegistedInfoListAck::SaleInfoVec& vecData )
{
	m_vecListData = vecData;

	__int64 n64Total = 0;

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( pListBox )
	{
		pListBox->ResetContent( );

		int nIndex = 0;
		CEditString strBuffer;
		for( SaleDataIter iter = m_vecListData.begin(); iter != m_vecListData.end(); ++iter )
		{
			FLOneSaleInfo* pData = &(*iter);
			if( NULL == pData )
				continue;

			strBuffer.Empty();
			g_WndMng.PutItemName( &pData->kItemElem, &strBuffer );
			strBuffer.Trim();
			g_WndMng.PutItemAbilityPiercing( &pData->kItemElem, &strBuffer, 0xffff0000 );
			FLStrcpy( pData->szItemName, _countof( pData->szItemName ), strBuffer.GetString() );
			nIndex = pListBox->AddString( "" );
			pListBox->SetItemDataPtr( nIndex, pData );

			n64Total += (__int64)( pData->nItemCostTot );
		}
	}

	//update 총판매 예상금액 
	CString strBuffer;
	strBuffer.Format( "%I64d", n64Total );
	CWndStatic* pWndTotal = ( CWndStatic* )GetDlgItem( WIDC_STATIC4 );
	if( pWndTotal )
		pWndTotal->SetTitle( strBuffer );
}

void CWndCM_Sub2Registration::SetRegCount( const DWORD dwCurr, const DWORD dwMax )
{
	//update static box
	m_dwCurrRegCount = dwCurr;
	m_dwMaxRegCount = dwMax;

	CWndStatic* pWndCount = ( CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	if( pWndCount )
	{
		CString strBuffer;
		strBuffer.Format( prj.GetText( TID_MMI_TRADESYSTEM_ADMITITEMNUMBER ), m_dwCurrRegCount, m_dwMaxRegCount );	//등록한물품(%d/%d)
		pWndCount->SetTitle( strBuffer );
	}
}

void CWndCM_Sub2Registration::SendPacket_CancelReg( )
{
	CWndListBox* pListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return;

	int nSel = pListBox->GetCurSel( );
	if( nSel < 0 )
		return;

	FLOneSaleInfo* pData = (FLOneSaleInfo*) pListBox->GetItemDataPtr( nSel );
	if( NULL == pData )
		return;

	FLPacket_CancelRegistedReq kPacket;
	kPacket.m_u64SaleSRL = pData->u64SaleSRL;

	g_DPlay.SendPacket( &kPacket );
}

void CWndCM_Sub2Registration::SendPacket_Tax( const int nPrice )
{
	FLPacket_CalcTaxReq kPacket;
	kPacket.m_nPrice = nPrice; 
	g_DPlay.SendPacket( &kPacket );
}

void CWndCM_Sub2Registration::OnPacket_Tax( const int nTax )
{
	//세금패킷 받음
	//확인 메세지 띄움
	if( nTax < 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "error nTax is %d" ), nTax );
		FLASSERT( 0 );
	}
	CreateMsgBox_ConfirmReg( nTax );
}

void CWndCM_Sub2Registration::SendPakcet_Registration( )
{
	if( NULL == m_pWndInput )
		return;

	__int64 n64Price = m_pWndInput->GetEachPrice( );
	int nCount = m_pWndInput->GetCount( );

	n64Price = n64Price * nCount;

	if( n64Price < 1 || n64Price > MAX_PEYNA_FOR_CLIENT )
	{
		FLASSERT( 0 );
		g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_SELLTEXT01 ) );	//최소 1원이상되어야함.
		return;

	}

	if( nCount < 1 )
	{
		FLASSERT( 0 );
		g_WndMng.OpenMessageBox( "Check count!!" );
		return;
	}
	
	//sendpacket
	FLPacket_RegistReq kPacket;
	kPacket.m_dwItemObjID = m_dwRegItemId;
	kPacket.m_nItemCount = nCount;
	kPacket.m_nItemCost = (int)n64Price;

	g_DPlay.SendPacket( &kPacket );
}


//----------------------------------------------------------------------------------------------------------------------------------
//CWndCM_Sub3Calculation
CWndCM_Sub3Calculation::CWndCM_Sub3Calculation( )
{
	m_pWndMsgBox_OneItem = NULL;
}

CWndCM_Sub3Calculation::~CWndCM_Sub3Calculation( )
{
	SAFE_DELETE( m_pWndMsgBox_OneItem );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCM_Sub3Calculation::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_MASSAGE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndCM_Sub3Calculation::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	//리스트박스에 들어갈 아이템의 높이 
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( pWndListBox )
	{
		pWndListBox->m_bOnItem = TRUE;
		pWndListBox->m_nLineSpace = (int)( ( CM_LIST_HEIGHT - pWndListBox->GetFontHeight( ) ) * 0.5f );
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	CWndStatic* pWnd = ( CWndStatic* )GetDlgItem( WIDC_STATIC3 );
	if( pWnd )
		pWnd->SetTitle( "" );
} 

//BOOL CWndCM_Sub3Calculation::Process( void )
//{
//	return CWndBase::Process( );
//}

void CWndCM_Sub3Calculation::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndMsgBox_OneItem )
	{
		SAFE_DELETE( m_pWndMsgBox_OneItem );
	}
}

BOOL CWndCM_Sub3Calculation::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( m_pWndMsgBox_OneItem && pWndBase == m_pWndMsgBox_OneItem )
	{
		if( IDOK == nID )
		{
			//ok, sendpaket
			SendPacket_OneItem( );
			m_pWndMsgBox_OneItem->Destroy();
		}
		else if( IDCANCEL == nID )
		{
			m_pWndMsgBox_OneItem->Destroy();
		}

	}
   
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndCM_Sub3Calculation::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1:	//아이템( 소팅 )
		break;

	case WIDC_BUTTON2: //판매가격( 소팅 )
		break;

	case WIDC_BUTTON3: //판매마감기간( 소팅 )
		break;

	case WIDC_BUTTON4: //일괄정산
		SendPacket_AllItem( );
		break;

	case WIDC_LISTBOX1:
		if( message == WNM_DBLCLK || message == WNM_SELCANCEL ) // 지멋대로구만.
		{
			CreateMsgBox( );
		}
		else if( WNM_ONITEM == message )
		{
			PutTooltip( *pLResult );
		}
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCM_Sub3Calculation::OnMouseMove(UINT nFlags, CPoint point)
{
	//마우스가 유효한 데이터를 가리키고 있지 않은경우 툴팁제거 
	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( pListBox )
	{
		if( FALSE == pListBox->IsInValidArea( point ) )
			g_toolTip.CancelToolTip( );
	}
}

typedef FLPacket_SaleGoldInfoListAck::SaleGoldInfoVec::iterator SaleGoldIter;
void CWndCM_Sub3Calculation::OnDraw( C2DRender* p2DRender ) 
{
	CWndListBox* pList = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pList )
		return;

	int nSelected = pList->GetCurSel( );
	CRect kRect = pList->m_rectWindow;

	FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo* pSelectedData = NULL;
	if( nSelected > -1 )
		pSelectedData = ( FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo*) pList->GetItemDataPtr( nSelected );

	const int nHeightGap = 20;
	int nIndex = 0;
	CString strBuffer;

	SaleGoldIter iter = m_vecListData.begin();

	const int nScrollPos = pList->GetScrollPos();
	if( nScrollPos > 0 && nScrollPos < static_cast<int>( m_vecListData.size() ) ) {
		for( int i = 0; i < nScrollPos; ++i ) {
			++iter;
		}
	}
	else {
		pList->SetScrollPos( 0 );
	}

	for( ; iter != m_vecListData.end(); ++iter )
	{
		FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo* pData = &(*iter);

		CPoint pt( kRect.left + 16, kRect.top + 10 + (nIndex)*CM_LIST_HEIGHT );
		FLItemElem& kElem = pData->kItemElem;

		//Icon
		CTexture* pTexture = kElem.GetTexture( );
		PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( kElem.GetItemIndex() );
		if( NULL == pProp )
			continue;

		if( NULL == pTexture )
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), 0xffff00ff );

		if( pTexture )
			p2DRender->RenderTexture( pt, pTexture );

		//count
		if( pProp->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 

			int nItemNum	= kElem.m_nItemNum;
			if( kElem.GetExtra() > 0 )
				nItemNum -= kElem.GetExtra();			
							
			TCHAR szTemp[ 32 ];
			FLSPrintf( szTemp, _countof( szTemp ), "%d", nItemNum );
			CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );

			m_p2DRender->TextOut( pt.x + 32 - size.cx, pt.y + 32 - size.cy, szTemp, CWndCM::COUNT_COLOR );
			p2DRender->SetFont( pOldFont ); 
		}
		
		//Name
		strBuffer = CString( pData->kItemElem.m_szItemText );
		if( strBuffer.GetLength() > 20 )
		{
			int nReduceCount = 0;
			for( nReduceCount = 0; nReduceCount < 18; )
			{
				if( IsDBCSLeadByte( strBuffer[ nReduceCount ] ) )
					nReduceCount += 2;
				else
					++nReduceCount;
			}

			strBuffer = strBuffer.Left( nReduceCount );
			strBuffer += CString( "..." );
		}
		p2DRender->TextOut( kRect.left + 60, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//판매가격
		__int64 nTotalPrice = pData->nGold;
		strBuffer.Format( "%I64d", nTotalPrice );
		MakeMoneyStyle( strBuffer );
		p2DRender->TextOut( kRect.left + 360, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strBuffer.GetString(), CWndCM::TEXT_COLOR );

		//판매마감 시간
		tstring strTime = FLTmToString( pData->kSaleDate, FLTm::E_FORMAT::YYYYMMDD );
		p2DRender->TextOut( kRect.left + 620, kRect.top + nHeightGap + (nIndex)*CM_LIST_HEIGHT, strTime.c_str(), CWndCM::TEXT_COLOR );

		//라인
		CPoint kLS( kRect.left + 1, kRect.top + nHeightGap + ( nIndex )*CM_LIST_HEIGHT +24 );
		CPoint kLE( kRect.right - 4, kRect.top + nHeightGap + ( nIndex )*CM_LIST_HEIGHT +24 );
		p2DRender->RenderLine( kLS, kLE, CWndCM::LINE_COLOR );

		//선택된 아이템 박스표시
		if( pSelectedData == pData )
		{

			CRect renderBoxRect = kRect;

			renderBoxRect.left += 2;
			renderBoxRect.right -= 12;

			renderBoxRect.top = renderBoxRect.top + 5 + (nIndex)*CM_LIST_HEIGHT,
			renderBoxRect.bottom = renderBoxRect.top + 39;
	
			p2DRender->RenderFillRect( renderBoxRect, CWndCM::SELECT1_COLOR );
			p2DRender->RenderRect( renderBoxRect, CWndCM::SELECT2_COLOR );
		}

		++nIndex;

		if( MAX_ITEMCOUNT_PAGE <= nIndex ) {
			break;
		}
	}
}

void CWndCM_Sub3Calculation::SetTotalNum( const DWORD nNum )
{

}

void CWndCM_Sub3Calculation::SetListData( const FLPacket_SaleGoldInfoListAck::SaleGoldInfoVec& vecData )
{
	m_vecListData = vecData;

	__int64 n64Total = 0;

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( pListBox )
	{
		pListBox->ResetContent( );

		int nIndex = 0;
		CEditString strBuffer;
		for( SaleGoldIter iter = m_vecListData.begin(); iter != m_vecListData.end(); ++iter )
		{
			FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo* pData = &(*iter);

			strBuffer.Empty();
			g_WndMng.PutItemName( &pData->kItemElem, &strBuffer );
			strBuffer.Trim();
			g_WndMng.PutItemAbilityPiercing( &pData->kItemElem, &strBuffer, 0xffff0000 );
			FLStrcpy( pData->kItemElem.m_szItemText, _countof( pData->kItemElem.m_szItemText), strBuffer.GetString() );

			nIndex = pListBox->AddString( "" );
			pListBox->SetItemDataPtr( nIndex, pData );

			n64Total += ( pData->nGold );
		}
	}

	//update 총판매 예상금액 
	CString strBuffer;
	strBuffer.Format( "%I64d", n64Total );
	CWndStatic* pWndTotal = ( CWndStatic* )GetDlgItem( WIDC_STATIC4 );
	if( pWndTotal )
		pWndTotal->SetTitle( strBuffer );
}

typedef FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo TypeFLSaleGold;
void CWndCM_Sub3Calculation::SendPacket_OneItem( )
{
	FLPacket_CollectSaleGoldReq kPacket;

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return;

	int nSel = pListBox->GetCurSel( );
	if( nSel < 0 )
		return;

	TypeFLSaleGold* pData = (TypeFLSaleGold*)pListBox->GetItemDataPtr( nSel );
	if( NULL == pData )
		return;

	kPacket.m_u64SaleGoldSRL = pData->u64SaleGoldSRL;

	g_DPlay.SendPacket( &kPacket );
}

void CWndCM_Sub3Calculation::SendPacket_AllItem( )
{
	FLPacket_CollectSaleGoldReq kPacket;

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return;

	const int nMaxCount = pListBox->GetCount( );
	for( int i = 0; i < nMaxCount; ++i )
	{
		TypeFLSaleGold* pData = (TypeFLSaleGold*)pListBox->GetItemDataPtr( i );
		if( NULL == pData )
			continue;

		kPacket.m_u64SaleGoldSRL = pData->u64SaleGoldSRL;
		g_DPlay.SendPacket( &kPacket );
		Sleep( 500 );
	}
}

void CWndCM_Sub3Calculation::OnPage( BOOL bOn  )
{
	if( bOn )
	{
		//정산 리스트 요청
		FLPacket_SaleGoldInfoListReq kPacket;
		g_DPlay.SendPacket( &kPacket );

		SetVisible( TRUE );

		SetFocus( );
	}
	else
	{
		SetVisible( FALSE );
	}
}

void CWndCM_Sub3Calculation::CreateMsgBox( )
{
	m_pWndMsgBox_OneItem = new CWndMessageBox;
	m_pWndMsgBox_OneItem->Initialize( prj.GetText( TID_MMI_TRADESYSTEM_ITEMCALCULATE01 ), this, MB_OKCANCEL );
//	m_pWndMsgBox_OneItem->Initialize( "진짜 정산?  ID필요", this, MB_OKCANCEL );
}

BOOL CWndCM_Sub3Calculation::PutTooltip( const int nListIndex )
{
	CPoint point2 = GetMousePoint( );

	CWndListBox* pListBox = ( CWndListBox* )GetDlgItem( WIDC_LISTBOX1 );
	if( NULL == pListBox )
		return FALSE;

	FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo* pData = ( FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo* )pListBox->GetItemDataPtr( nListIndex );
	if( NULL == pData )
		return FALSE;

	CRect kRectBox = pListBox->m_rectWindow;
	CRect kRectItem = pListBox->GetItemRect( nListIndex );

	kRectBox.top += kRectItem.top;

	ClientToScreen( &point2 );
	ClientToScreen( &kRectBox );

	FLItemElem* pItemElem = &pData->kItemElem;
	g_WndMng.PutToolTip_Item( pItemElem, point2, &kRectBox, -1 );
		
	return TRUE;
}



//----------------------------------------------------------------------------------------------------------------------------------
//CWndInputNumCommon2
CWndInputNumCommon2::CWndInputNumCommon2( )
{
	m_dwTargetWnd = 0;
	m_dwItemID = NULL_ID;
	m_n64MaxPrice = MAX_PEYNA_FOR_CLIENT;
}

CWndInputNumCommon2::~CWndInputNumCommon2( )
{
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndInputNumCommon2::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_TRADE, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 

void CWndInputNumCommon2::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	SetTargetWnd( WIDC_EDIT1 );

	CWndEdit* pWndEdit = NULL;
	pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( pWndEdit )
	{
		pWndEdit->AddWndStyle( EBS_NUMBER );
		pWndEdit->SetMaxStringNumber( 4 );
		pWndEdit->SetString( "1" );
	}

	pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	if( pWndEdit )
	{
		pWndEdit->AddWndStyle( EBS_NUMBER );
		pWndEdit->SetMaxStringNumber( 10 );
	}

	CWndStatic* pWndTotalMoney = ( CWndStatic* )GetDlgItem( WIDC_STATIC5 );
	if( pWndTotalMoney )
	{
		pWndTotalMoney->AddWndStyle( WSS_MONEY );
		pWndTotalMoney->SetTitle( "" );
	}

	//확인버튼 비활성
	EnableChildWnd( WIDC_BUTTON13, FALSE );
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndInputNumCommon2::Process( void )
{
	return CWndBase::Process( );
}

BOOL CWndInputNumCommon2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
   
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndInputNumCommon2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON13:	//확인
		//SendPacket( );
		if( m_pParentWnd )
		{
			__int64 n64Total = GetTotalPrice( );
			if( n64Total > MAX_PEYNA_FOR_CLIENT )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_SELLTEXT04  ) );
				break;
			}

			int nPrice = ( int )n64Total;
			m_pParentWnd->OnChildNotify( WIDC_BUTTON13, GetWndId(), (LRESULT*)&nPrice );
		}

		break;

	case WIDC_BUTTON14: //취소
		if( m_pParentWnd )
			m_pParentWnd->OnChildNotify( WIDC_BUTTON14, GetWndId(), NULL );


		break;

	case WIDC_BUTTON11: // <<
		AddCount( -1 );
		break;

	case WIDC_BUTTON12: // >>
		AddCount( 1 );
		break;

	case WIDC_EDIT1:				
	case WIDC_EDIT2:				
		SetTargetWnd( nID );
		break;
		
	case WIDC_BUTTON1: AddString( "1" );		break;		//1
	case WIDC_BUTTON2: AddString( "2" );		break;		//2
	case WIDC_BUTTON3: AddString( "3" );		break;		//3
	case WIDC_BUTTON4: AddString( "4" );		break;		//4
	case WIDC_BUTTON5: AddString( "5" );		break;		//5
	case WIDC_BUTTON6: AddString( "6" );		break;		//6
	case WIDC_BUTTON7: AddString( "7" );		break;		//7
	case WIDC_BUTTON8: AddString( "8" );		break;		//8
	case WIDC_BUTTON9: AddString( "9" );		break;		//9
	case WIDC_BUTTON10: AddString( "0" );		break;		//0
	case WIDC_BUTTON17: AddString( "00" );		break;		//00
	case WIDC_BUTTON18: AddString( "000" );		break;		//000
	case WIDC_BUTTON19: AddString( "0000" );	break;		//0000
		break;

	case WIDC_BUTTON20: //all : 최대수량
		Process_All( );
		break;

	case WIDC_BUTTON15: //c
		ResetString( m_dwTargetWnd );
		break;

	case WIDC_BUTTON16: //backspace
		SubString( );
		break;
	}


	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

int CWndInputNumCommon2::GetCount( )
{
	CWndEdit* pWndCount = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( NULL == pWndCount )
		return 0;

	int nCount = 0;
	pWndCount->GetEditNum( nCount );

	return nCount;
}

__int64 CWndInputNumCommon2::GetEachPrice( )
{
	CWndEdit* pWndPrice = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	if( NULL == pWndPrice )
		return 0;

	__int64 n64Price = 0;
	pWndPrice->GetEditNum64( n64Price );

	return n64Price;
}

__int64 CWndInputNumCommon2::GetTotalPrice( )
{
	return GetCount() * GetEachPrice( );
}


void CWndInputNumCommon2::OnDraw( C2DRender* p2DRender ) 
{
	UpdateTotalMoney( );

	if( m_dwTargetWnd != 0 )
	{
		CWndEdit* pWndTarget = (CWndEdit*)GetDlgItem( m_dwTargetWnd );
		if( pWndTarget )
		{
			CRect kRect = pWndTarget->m_rectWindow;
			kRect += CRect( 2, 2, 2, 2 );

			p2DRender->RenderRect( kRect, 0xffff0000 );
		}
	}
}

void CWndInputNumCommon2::SetItem( DWORD dwItemID )
{
	m_dwItemID = dwItemID;

	FLItemElem* pTempElem = (FLItemElem*)g_pPlayer->GetItemId( m_dwItemID );
	if( !pTempElem )
		return;

	//등록시 현재 가지고있는 갯수만큼 설정
	SetCount( pTempElem->m_nItemNum );

	//만일 중복불가능한 아이템이면 "<< >>"버튼 비활성
	PT_ITEM_SPEC pSpec = pTempElem->GetProp( );
	if( pSpec )
	{
		CWndButton* pWndAdd = (CWndButton*)GetDlgItem( WIDC_BUTTON11 );
		CWndButton* pWndSub = (CWndButton*)GetDlgItem( WIDC_BUTTON12 );
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

		if( NULL == pWndAdd || NULL == pWndSub || NULL == pWndEdit || NULL == pWndEdit2 )
			return;

		if( 1 < pSpec->dwPackMax )	//중복가능
		{
			pWndAdd->EnableWindow( TRUE );
			pWndSub->EnableWindow( TRUE );
			pWndEdit->EnableWindow( TRUE );

			pWndEdit->SetFocus();
			SetTargetWnd( WIDC_EDIT1 );
		}
		else
		{
			pWndAdd->EnableWindow( FALSE );
			pWndSub ->EnableWindow( FALSE );
			pWndEdit->EnableWindow( FALSE );

			pWndEdit2->SetFocus( );
			SetTargetWnd( WIDC_EDIT2 );
		}
	}
}

void CWndInputNumCommon2::SetCount( const int nCount )
{
	CWndEdit* pWndCount = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( NULL == pWndCount )
		return;

	if( nCount < 0 )
		return;

	CString strBuffer;
	strBuffer.Format( "%d", nCount );

	pWndCount->SetString( strBuffer.GetString() );
}

BOOL CWndInputNumCommon2::CheckCount( int nCount )
{
	//수량이 올바른지 체크
	FLItemElem* pTempElem = (FLItemElem*)g_pPlayer->GetItemId( m_dwItemID );
	if( !pTempElem )
		return FALSE;

	PT_ITEM_SPEC pSpec = pTempElem->GetProp( );
	if( NULL == pSpec )
		return FALSE;

	//현재 가지고있는 수량보다 크면 안댐
	if( pTempElem->m_nItemNum < nCount )
		return FALSE;

	return TRUE;
}

BOOL CWndInputNumCommon2::GetMaxCount( )
{
	//현재 소지량 최대값( 한슬롯 )
	FLItemElem* pTempElem = (FLItemElem*)g_pPlayer->GetItemId( m_dwItemID );
	if( !pTempElem )
		return FALSE;

	return pTempElem->m_nItemNum;
}

void CWndInputNumCommon2::AddCount( const int nCount )
{
	CWndEdit* pWndCount = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( pWndCount )
	{
		int nVal = 0;
		pWndCount->GetEditNum( nVal );

		nVal = nVal + nCount;

		//수량은 1보다 작을순 없음.
		if( nVal < 1 )
			nVal = 1;
		//1보다 크다면 현재소지량과 검사후 적절히 대처
		else if( nVal > 1  )
		{
			BOOL bCheckOK = CheckCount( nVal );
			if( FALSE == bCheckOK )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_ITEMREGISTEROVER01 ) ); //"최대값입니다 ID필요" );
				nVal = GetMaxCount( );
			}
		}

		CString strBuffer;
		strBuffer.Format( "%d", nVal );

		pWndCount->SetString( strBuffer.GetString() );

		SetTargetWnd( WIDC_EDIT1 );
	}
}

void CWndInputNumCommon2::AddString( const char* szString )
{
	CWndEdit* pWndTargetEdit = (CWndEdit*)GetDlgItem( m_dwTargetWnd );
	if( NULL == pWndTargetEdit )
		return;

	//CHECK1: 현재 타겟 에디트박스에 1~9까지의 숫자가 없는상태에서 0~0000까지 들어올경우 무시
	__int64 nCheckNum = 0;
	pWndTargetEdit->GetEditNum64( nCheckNum );
	if( nCheckNum < 1 )
	{
		int nAddNum = atoi( szString );
		if( nAddNum == 0 )
			return;
	}

	CString strOld = pWndTargetEdit->GetString( );

	pWndTargetEdit->AddString( szString );

	//CHECK2: 수량및 금액의 한계값 체크 
	//수량
	if( WIDC_EDIT1 == m_dwTargetWnd )
	{
		int nCount = 0;
		pWndTargetEdit->GetEditNum( nCount );
		if( nCount > 9999 )
		{
			pWndTargetEdit->SetString( strOld );
			g_WndMng.OpenMessageBox( prj.GetText(  TID_MMI_TRADESYSTEM_ITEMENTRYOVERNUMBER01 ) ); //"더이상 큰수량을 입력할수 없습니다 ID필요" );
			return;
		}
	}
	//금액
	else if( WIDC_EDIT2 == m_dwTargetWnd )	
	{
		__int64 n64Price = 0;
		pWndTargetEdit->GetEditNum64( n64Price );
		if( n64Price > m_n64MaxPrice )
		{
			pWndTargetEdit->SetString( strOld );
			g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TRADESYSTEM_SELLTEXT04 ) ); //"더이상 큰금액을 입력할수없습니다 ID필요" );
			return;
		}
	}
}

void CWndInputNumCommon2::SubString( )
{
	CWndEdit* pWndTarget = (CWndEdit*)GetDlgItem( m_dwTargetWnd );
	if( NULL == pWndTarget )
		return;

	CString strPrice = pWndTarget->GetString( );
	int nLen = strPrice.GetLength();
	if( nLen < 1 )
		return;

	strPrice.Delete( nLen - 1 );

	pWndTarget->SetString( strPrice.GetString() );
}

void CWndInputNumCommon2::ResetString( const DWORD dwWndId )
{
	CWndEdit* pWndPrice = (CWndEdit*)GetDlgItem( dwWndId );
	if( pWndPrice )
	{
		pWndPrice->SetString( "" );
	}
}

void CWndInputNumCommon2::UpdateTotalMoney( )
{
	CWndEdit* pWndCount = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndPrice = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

	if( NULL == pWndCount || NULL == pWndPrice )
		return;

	int nCount = 0;
	__int64 n64Price = 0;
	pWndCount->GetEditNum( nCount );
	pWndPrice->GetEditNum64( n64Price );

	if( nCount < 0 || n64Price < 0 )
	{
		EnableChildWnd( WIDC_BUTTON13, FALSE );
		return;
	}
	
	const __int64 n64BasePrice = 1000000;	//백만

	CWndStatic* pWndTotalMoney = ( CWndStatic* )GetDlgItem( WIDC_STATIC5 );
	if( pWndTotalMoney )
	{
		__int64 n64Total = n64Price * nCount;

		DWORD dwColor = 0xffffffff;
		if( n64Total < n64BasePrice )
		{
		}
		else if( n64Total < n64BasePrice * 10 )	// 천만이하
		{
			dwColor = 0xff0000ff; //파랑
		}
		else if( n64Total < n64BasePrice * 100 ) // 1억이하
		{
			dwColor = 0xffcc9000; //노랑
		}
		else if( n64Total < n64BasePrice * 1000 ) // 10억이하
		{
			dwColor = 0xff996600;	//주황
		}
		else
		{
			dwColor = 0xff663300;  //갈색
		}
		

		CString strBuffer;
		strBuffer.Format( "%I64d", n64Total );
		pWndTotalMoney->SetTitle( strBuffer );
		pWndTotalMoney->SetTileColor( dwColor );

		n64Total > 0  ? EnableChildWnd( WIDC_BUTTON13, TRUE ) : EnableChildWnd( WIDC_BUTTON13, FALSE );
	}
}

void CWndInputNumCommon2::SetTargetWnd( const DWORD dwWndId )
{
	if( m_dwTargetWnd != dwWndId )
	{
	//	ResetString( dwWndId );
		m_dwTargetWnd = dwWndId;
	}
}

void CWndInputNumCommon2::Process_All( )
{
	if( WIDC_EDIT1 == m_dwTargetWnd )
	{
		int nMax = GetMaxCount( );
		SetCount( nMax );
	}
	else if( WIDC_EDIT2 == m_dwTargetWnd )
	{
		ResetString( m_dwTargetWnd );
		CString strBuffer;
		strBuffer.Format( "%d", MAX_PEYNA_FOR_CLIENT );
		AddString( strBuffer.GetString() );
	}
}

void CWndInputNumCommon2::EnableChildWnd( DWORD dwWndId, BOOL bEnable )
{
	CWndBase* pWndBase = ( CWndBase* )GetDlgItem( dwWndId );
	if( pWndBase )
		pWndBase->EnableWindow( bEnable );
}

//----------------------------------------------------------------------------------------------------------------------------------
//CWndComposeTwohandWeapon
const float CWndComposeTwohandWeapon::m_sfDirectTimeMAX = 6.0f;
CWndComposeTwohandWeapon::CWndComposeTwohandWeapon( )
{
	m_bNoCloseButton = TRUE;		//x버튼 제거
	m_pItem = NULL;
	m_fDTime = 0.0f;
	m_fDirectTime = 0.0f;
	m_bOnDirection = FALSE;

	m_pMsgBoxStart = NULL;
	m_pMsgBoxClose = NULL;
}

CWndComposeTwohandWeapon::~CWndComposeTwohandWeapon( )
{
	//unlock item

	ResetSlot( SLOT_MAX );

	SAFE_DELETE( m_pItem );
	SAFE_DELETE( m_pMsgBoxStart );
	SAFE_DELETE( m_pMsgBoxClose );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndComposeTwohandWeapon::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WEAPONMERGE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndComposeTwohandWeapon::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	ResetSlot( SLOT_MAX );	//모든슬롯 초기화
	MoveParentCenter();
} 

BOOL CWndComposeTwohandWeapon::Process( void )
{
	UpdateButton( );

	if( m_bOnDirection )
	{
		m_fDirectTime += _GetDeltaTimeSec( );
		if( m_fDirectTime > m_sfDirectTimeMAX )
		{
			SendPacket();
			m_bOnDirection = FALSE;
		}
	}

	WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_PICTURE1 );
	WNDCTRL* pWndSlot2 = GetWndCtrl( WIDC_PICTURE2 );
	WNDCTRL* pWndSlot3 = GetWndCtrl( WIDC_PICTURE3 );

	if( NULL == pWndSlot1 || NULL == pWndSlot2 || NULL == pWndSlot3 )
		return FALSE;

	CPoint point = GetMousePoint();

	//discern slot
	CRect rectSlot;
	FLItemElem* pItem = NULL; 
	if( NULL_ID != m_dwSlotItem[SLOT_1] && PtInRect( pWndSlot1->rect, point ) )
	{
		//제 1 무기슬롯
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_1 ] );
		rectSlot = pWndSlot1->rect;
	}
	else if( PtInRect( pWndSlot2->rect, point ) )
	{
		//제 2 무기슬롯
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_2 ] );
		rectSlot = pWndSlot2->rect;
	}
	else if( PtInRect( pWndSlot3->rect, point ) )
	{
		// 캐시 아이템
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_3 ] );
		rectSlot = pWndSlot3->rect;
	}

	if( pItem )
	{
		CPoint Point2 = point;
		ClientToScreen( &Point2 );
		ClientToScreen( &rectSlot );
		g_WndMng.PutToolTip_Item( pItem, Point2, &rectSlot );
	}


	return CWndBase::Process( );
}

BOOL CWndComposeTwohandWeapon::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( pWndBase )
	{
		if( pWndBase == m_pMsgBoxStart )
		{
			//캐시 미등록시 확인 messagebox
			if( IDOK == nID )
			{
				DoDirection( );
			}
		}
		else if( pWndBase == m_pMsgBoxClose )
		{
			//닫기전 등록되어있는 아이템이 있다고 알려주는 messagebox
			if( IDOK == nID )
				Destroy( );
		}
	}
		

 	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

BOOL CWndComposeTwohandWeapon::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1: 
		if( NULL_ID == m_dwSlotItem[ SLOT_3 ] )
		{
			//캐시 아이템 미등록의 경우 messagebox 띄움
			SAFE_DELETE( m_pMsgBoxStart );
			m_pMsgBoxStart = new CWndMessageBox;
			BOOL bInit = m_pMsgBoxStart->Initialize( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_CONFIRM02 ), this, MB_OKCANCEL );	//연마석 없이 깨질수있음

			//메세지 박스 생성 실패땐 그냥 보내는걸로.
			if( FALSE == bInit )
				DoDirection( );
		}
		else
		{
			//캐시 아이템 등록된 경우 바로 패킷전송
			DoDirection( );
		}
		break;

	case WIDC_BUTTON2: 
		if( NULL_ID != m_dwSlotItem[SLOT_1] || NULL_ID != m_dwSlotItem[SLOT_2] || NULL_ID != m_dwSlotItem[SLOT_3] )
		{
			//하나라도 등록이 되어있는경우 messagebox 띄움
			SAFE_DELETE( m_pMsgBoxClose );
			m_pMsgBoxClose = new CWndMessageBox;
			BOOL bInit = m_pMsgBoxClose->Initialize( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_CONFIRM ), this, MB_OKCANCEL );		//confirm
			if( FALSE == bInit )
				Destroy( );
		}
		else
		{
			//슬롯이 모두 비어있다면 걍 종료
			Destroy( );
		}
		break;

	default: break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndComposeTwohandWeapon::OnDraw( C2DRender* p2DRender ) 
{
	if( false == m_strEditString.IsEmpty() )
		p2DRender->TextOut_EditString( 230, 16, m_strEditString );

	FLItemElem* pItem = NULL;
	for( int i = 0; i < SLOT_MAX; ++i )
	{
		if( NULL_ID == m_dwSlotItem[ i ] )
			continue;
		
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ i ] );


		if( pItem )
		{
			CRect rect = GetSlotRect( i );
			p2DRender->RenderTexture( rect.TopLeft(), pItem->GetTexture() );
		}
	}

	//check model
	if( NULL == m_pItem )
		return;

	CModel* pItemModel = m_pItem->GetModel( );
	if( NULL == pItemModel )
		return;

	CRect rect = GetClientRect();

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_STATIC4 );
	if( NULL == lpFace )
		return;

	CPoint point( lpFace->rect.left-12, lpFace->rect.top-22 );
	CRect rectTemp;

	//render background
	//if(m_pTexPetStatusBg != NULL)
	//	m_pTexPetStatusBg->Render( p2DRender,  point );

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

//	pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD  );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE);
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

	float fRadius = pItemModel->GetRadius( );
	float fW = pItemModel->GetMaxWidth();
	float fH = pItemModel->GetMaxHeight();

	FLOAT fov = D3DX_PI/4.0f; //4.0f;//796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXVECTOR3 vMin = pItemModel->m_vMin;
	D3DXVECTOR3 vMax = pItemModel->m_vMax;

	//view
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );

	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

	//모델은 x축으로 누워있기때문에 모델을 같은 위치에 위치시키기 위함.
	matTrans._42 -= vMin.x;
	matTrans._42 -= 0.7f;


	D3DXMatrixRotationZ( &matRot1, DEGREETORADIAN(90) );
	
	static const float fCon = m_sfDirectTimeMAX / D3DX_PI * 2.0f;
	m_fDTime += _GetDeltaTimeSec( );
	
	//연출중이라면 서서히 더 빨리돌림
	if( m_bOnDirection )
	{
		m_fDTime += ( m_fDirectTime / fCon );
		float fScaleXYZ = ( m_sfDirectTimeMAX - m_fDirectTime );
		
		float fScale = m_sfDirectTimeMAX - m_fDirectTime;
		if( fScale < 2.0f )
		{
			fScale = 3.0f - fScale;
			D3DXMatrixScaling( &matScale, fScale, fScale, fScale );
		}
	}

	if( m_fDTime > D3DX_PI )
		m_fDTime = 0.0f;

	D3DXMatrixRotationY( &matRot2, m_fDTime );
	D3DXMatrixMultiply( &matRot1, &matRot1,&matRot2);
	
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
//	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot2);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// 랜더링 
//	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
//	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;

	::SetTransformView( matView );
	::SetTransformProj( matProj );

//	pItemModel->SetTextureEx( m_pPetModel->m_pModelElem->m_nTextureEx );
	
	float fBlend = ( m_sfDirectTimeMAX - m_fDirectTime ) * ( 1.0f/m_sfDirectTimeMAX ) * 255.0f;

	pItemModel->SetBlendFactor( (DWORD)fBlend );
	pItemModel->Render(pd3dDevice, &matWorld);
//	pItemModel->RenderEffect(pd3dDevice, &matWorld, IK3_SWD, 10, 10);
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 

//	pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
}

void CWndComposeTwohandWeapon::ReceivedPacket_Result( const DWORD dwResult )
{
	if( FSC_ITEMUPGRADE_UPGRADE_SUCCESS == dwResult )
	{
		//합성결과 : 성공
		ResetSlot( SLOT_1 );
		ResetSlot( SLOT_2 );
		ResetSlot( SLOT_3 );
	}
	else if( FSC_ITEMUPGRADE_UPGRADE_FAIL == dwResult )
	{
		//합성결과 : 실패
		if( NULL_ID != m_dwSlotItem[ SLOT_3 ] )
		{
			//ok
		}
		else 
		{
			ResetSlot( SLOT_2 ); 
		}
	}
// 	else if( E_UPGRADE_ERROR == byRst )
// 	{
// 		// 아예 합성하지 못함. 
// 		//Reset( );
// 	}

	UpdateButton( );
	UpdateSlot( );
}

CRect CWndComposeTwohandWeapon::GetSlotRect( int nSlot )
{
	DWORD dwWndId = 0;

	switch( nSlot )
	{
	case SLOT_1: dwWndId = WIDC_PICTURE1; break;
	case SLOT_2: dwWndId = WIDC_PICTURE2; break;
	case SLOT_3: dwWndId = WIDC_PICTURE3; break;
	default: 
		FLASSERT( 0 );
		break;
	}

	WNDCTRL* pWnd = GetWndCtrl( dwWndId );
	if( pWnd )
		return pWnd->rect;

	return CRect( 0, 0, 0, 0 );
}

void CWndComposeTwohandWeapon::DropedIcon( DWORD dwItemId )
{
	//본 윈도우부터 아이콘을 빼냄
	for( int i = 0; i < SLOT_MAX; ++i )
	{
		if( m_dwSlotItem[ i ] == dwItemId )
		{
			//ok
			if( SLOT_1 == i )
				ResetSlot( SLOT_2 );

			ResetSlot( i );
			break;
		}
	}
}

void CWndComposeTwohandWeapon::ResetSlot( int nSlot )
{
	FLItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ nSlot ] );

	switch( nSlot )
	{
	case SLOT_1:	//무기 1번
		SAFE_DELETE( m_pItem );
		if( pItemElem )
			pItemElem->SetExtra( 0 );

		m_dwSlotItem[ nSlot ] = NULL_ID;
		m_fDirectTime = 0.0f;
		break;

	case SLOT_2:	//무기2번
		if( pItemElem )
			pItemElem->SetExtra( 0 );

		m_dwSlotItem[ nSlot ] = NULL_ID;
		UpdateOptionTip( NULL );
		break;

	case SLOT_3:	//캐시
		if( pItemElem )
			pItemElem->SetExtra( 0 );
		
		m_dwSlotItem[ nSlot ] = NULL_ID;
		break;

	case SLOT_MAX:
		{
			for( int i = 0; i < SLOT_MAX; ++i )
			{
				ResetSlot( i );
			}
		}
		break;

	default:
		FLASSERT( 0 );
		break;
	}
}

void CWndComposeTwohandWeapon::UpdateButton( )
{
	//1,2번 슬롯이 유효한경우만 Start버튼 활성
	CWndButton* pWndBtnStart = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	if( NULL == pWndBtnStart )
		return;

	if( m_dwSlotItem[ SLOT_1 ] != NULL_ID && m_dwSlotItem[ SLOT_2 ] != NULL_ID )
		pWndBtnStart->EnableWindow( TRUE );
	else pWndBtnStart->EnableWindow( FALSE );

}

void CWndComposeTwohandWeapon::UpdateSlot( )
{
	//현재 등록어있는 슬롯 아이템 <->인벤토리 검사후 리셋
	FLItemElem* pItemElem = NULL;
	for( int i = 0; i < SLOT_MAX; ++i )
	{
		pItemElem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ i ] );
		if( NULL == pItemElem )
		{
			ResetSlot( i );	
		}
	}
}

void CWndComposeTwohandWeapon::UpdateOptionTip( FLItemElem* pItem )
{
	//1. 아이템에 대하여 피어싱옵션, 각성옵션, 접두사, 일반제련 에 대한 옵션을 출력해준다.

	m_strEditString.Empty();

	LPWNDCTRL pCtrl = GetWndCtrl( WIDC_STATIC5 );
	if( pCtrl )
	{
		CRect rect = pCtrl->rect;
		rect.DeflateRect( 2, 2, 14, 2 );
		m_strEditString.Adjust( GetFont(), &rect );
	}
	
	if( NULL == pItem )
	{
		m_strEditString.AddString( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_INFOTEXT01 ) );		//어떤것들만 추출가능하다.
		return;
	}

	MakeTipTex( pItem, &m_strEditString );
		
}

BOOL IsDst_Rate( int nDstParam );
char *FindDstString( int nDstParam );
BOOL CWndComposeTwohandWeapon::MakeTipTex( FLItemElem* pItem, CEditString* pEdit )
{
	if( NULL == pItem )
		return FALSE;

	CString	str;

	LPCTSTR szTitle = prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_RESULTTEXT01 );
	str.Format( "%s", szTitle );
	pEdit->AddString( str.GetString() , 0xffff0000 );		//"[양손 무기 합성 옵션]"

	//1. 일반제련 데미지 ?
	BOOL bAddedOption = FALSE;
	const DWORD dwCombinedAddDamage = pItem->GetMinAddDamage();
	
	if( dwCombinedAddDamage > 0 )
	{
		pEdit->AddString( "\n" );
		str.Format( prj.GetText(TID_GAME_TOOLTIP_ATTACKRANGE2) );
		pEdit->AddString( str, g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwGeneral );
		str.Format( " %d", (int)dwCombinedAddDamage );
		pEdit->AddString( str.GetString(), 0xffff0000 );

		bAddedOption = TRUE;
	}

	//2. 피어싱
	CEditString strTemp;
	if( FALSE == bAddedOption )
		strTemp = *pEdit;

	g_WndMng.PutPiercingOpt( pItem, pEdit, 0 );
	
	if( FALSE == bAddedOption && strTemp != *pEdit )
		bAddedOption = TRUE;

	//3. 랜덤옵션
	if( FALSE == bAddedOption )
		strTemp = *pEdit;

	g_WndMng.PutRandomOpt( pItem, pEdit, 0 );
	if( FALSE == bAddedOption && strTemp != *pEdit )
		bAddedOption = TRUE;
	
	//4. 각성옵션
	PT_ITEM_SPEC pItemProp = pItem->GetProp( );
	if( NULL == pItemProp )
		return FALSE;
	
	if( FALSE == bAddedOption )
		strTemp = *pEdit;

	for( size_t Nth = 0; Nth < pItem->GetRandomOptionExtensionSize(); ++Nth )
	{
		WORD wDstID = pItem->GetRandomOptionExtensionDstID( Nth );
		short shAdjValue = pItem->GetRandomOptionExtensionAdjValue( Nth );

		//각성 보호된 옵션이면 pass
		if( pItem->IsSetSafeGuardRandomOptionExtensionFlag() == true )
			continue;
		
		if( IsDst_Rate( wDstID ) == TRUE )
		{
			if( wDstID == DST_ATTACKSPEED )
			{
				str.Format( "\n%s %c%d%% ", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue / 2 / 10 ) );
			}
			else
			{
				str.Format( "\n%s %c%d%%", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
			}
		}
		else
		{
			str.Format( "\n%s %c%d", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
		}

		if( pItemProp->IsWeaponParts() == TRUE || pItemProp->IsArmorParts() == TRUE )
		{
			pEdit->AddString( str, g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAwakening );
		}
		else
		{
			pEdit->AddString( str, g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwBlessing );
		}
	}

	if( FALSE == bAddedOption && strTemp != *pEdit )
		bAddedOption = TRUE;

	if( FALSE == bAddedOption )
	{
		pEdit->AddString( "\n" );
		str.Format( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_INFOTEXT02 ), pItem->GetName().GetString() );
		pEdit->AddString( str.GetString() );		//얻을수있는게 없다.
	}

	return TRUE;
}

void CWndComposeTwohandWeapon::SendPacket( )
{
	FLPacketItemUpgradeCombineReq kPacket;
	kPacket.dwMainItemObjID = m_dwSlotItem[ SLOT_1 ];
	kPacket.dwMaterialItemObjID = m_dwSlotItem[ SLOT_2 ];
	kPacket.dwProtectionItemObjID = m_dwSlotItem[ SLOT_3 ];

	g_DPlay.SendPacket( &kPacket );
}

void CWndComposeTwohandWeapon::DoDirection( )
{
	m_bOnDirection = TRUE;
	m_fDirectTime = 0.0f;
}

void CWndComposeTwohandWeapon::Reset( )
{
	ResetSlot( SLOT_MAX );

	m_strEditString.Empty( );
}

BOOL CWndComposeTwohandWeapon::RegItem( DWORD dwItemId, int nSlot )
{
	BOOL bReg = FALSE;
	if( -1 == nSlot )
	{
		//자동등록
		for( int i = 0; i < SLOT_MAX; ++i )
		{
			if( NULL_ID == m_dwSlotItem[ i ] )
			{
				bReg = PutItem( i, dwItemId );
				break;
			}
		}
	}
	else
	{
		//수동등록
		bReg = PutItem( nSlot, dwItemId );
	}

	return bReg;

}

BOOL CWndComposeTwohandWeapon::PutItem( int nSlot, DWORD dwItemId )
{
	//최종적으로 slot에 item을 지정하는 함수
	//각종메세지는 여기서 처리해버리자
	FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( dwItemId );
	if( NULL == pItem )
		return FALSE;

	//같은 아이템의 경우 pass
	if( m_dwSlotItem[ nSlot ] == dwItemId )
		return FALSE;

	//check valid item
	if( !IsUsableItem( pItem ) )
		return FALSE;

	FLItemElem* pOldItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ nSlot ] );

	BOOL bOK = TRUE;
	switch( nSlot )
	{
	case SLOT_1:		
		{
			PT_ITEM_SPEC pSpec = pItem->GetProp( );
			if( NULL == pSpec )
				return FALSE;

			if( pSpec->IsBaruna( ) )
			{
				//바루나 등급임.
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT04 ) );
				return TRUE;
			}

			if( pSpec->dwHanded != HD_TWO )
			{
				//양손무기 아님
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03 ) );
				return TRUE;
			}

			if( pItem->IsPeriod() )
			{
				//기간제 아이템임
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT02 ) );
				return TRUE;
			}

			if( pItem->IsSetCombinedOption( ) )
			{
				//이미 합성된 아이템임
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT01 ) );
				return TRUE;
			}

			//ok succeed
			if( pOldItem )
				pOldItem->SetExtra( 0 );

			m_dwSlotItem[nSlot] = dwItemId;
			pItem->SetExtra( pItem->GetExtra() + 1 );

			SAFE_DELETE( m_pItem );
			m_pItem = new CItem;
			m_pItem->m_pItemBase = new FLItemBase;
			m_pItem->SetIndex( D3DDEVICE, pItem->m_dwItemId, TRUE );
		}
		break;

	case SLOT_2:
		{
			PT_ITEM_SPEC pSpec = pItem->GetProp( );
			if( NULL == pSpec )
				return FALSE;

			if( pSpec->IsBaruna( ) )
			{
				//바루나 등급임.
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT04 ) );
				return TRUE;
			}

			if( pSpec->dwHanded != HD_TWO )
			{
				//양손무기 아님
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT03 ) );
				return TRUE;
			}

			if( pItem->IsPeriod() )
			{
				//기간제 아이템임
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT02 ) );
				return TRUE;
			}

			if( pItem->IsSetCombinedOption( ) )
			{
				//이미 합성된 아이템임
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT01 ) );
				return TRUE;
			}

			if( TRUE == pItem->IsOwnState( ) )
			{
				//거래불가
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT06 ) );
				return TRUE;
			}

			FLItemElem* pItemSlot1 = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[SLOT_1] );
			if( NULL == pItemSlot1 )
			{
				//1번 슬롯 비어있음.
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT09 ) );
				return TRUE;
			}

			PT_ITEM_SPEC pSpecSlot1 = pItemSlot1->GetProp();
			if( NULL == pSpecSlot1 )
				return FALSE;

			if( pSpecSlot1->dwSubCategory != pSpec->dwSubCategory )
			{
				//종류틀림
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT05 ) );
				return TRUE;
			}

			if( pSpecSlot1->dwLimitLevel1 < pSpec->dwLimitLevel1 )
			{
				//레벨높음
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT07 ) );
				return TRUE;
			}

			if( pSpecSlot1->dwItemJob < pSpec->dwItemJob )
			{
				//사용 직업 높음
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT08 ) );
				return TRUE;
			}

			//ok succeed
			if( pOldItem )
				pOldItem->SetExtra( 0 );

			m_dwSlotItem[nSlot] = dwItemId;
			pItem->SetExtra( pItem->GetExtra() + 1 );

			UpdateOptionTip( pItem );
		}
		break;

	case SLOT_3:
		{
			PT_ITEM_SPEC pSpec = pItem->GetProp( );
			if( NULL == pSpec )
				return FALSE;

			if( IK1_PASSIVE == pSpec->dwItemKind1 && IK2_TWOWEAPONMERGE == pSpec->dwItemKind2 && IK3_TWOWEAPONMERGE_PROTECTION == pSpec->dwItemKind3 )
			{
				//ok
			}
			else
			{
				//해당 아이템이아님
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT10 ) );
				return TRUE;
			}

			//ok succeed
			if( pOldItem )
				pOldItem->SetExtra( 0 );

			m_dwSlotItem[nSlot] = dwItemId;
			pItem->SetExtra( pItem->GetExtra() + 1 );

		}
		break;

	default: 
		break;
	}

	return bOK;
}

void CWndComposeTwohandWeapon::OnMouseWndSurface( CPoint point )
{
	int b = 0;
}

void CWndComposeTwohandWeapon::OnLButtonDown( UINT nFlags, CPoint point )
{
	CRect rect;
	int nSlot = -1;
	for( int i = 0; i < SLOT_MAX; ++i )
	{
		rect = GetSlotRect( i );
		if( PtInRect( &rect, point ) )
		{
			//ok
			nSlot = i;
			break;
		}
	}

	// 어떠한 슬롯도 누르지 않았다면 패스
	if( -1 == nSlot )
		return;

	//아이템이 등록 안되어 있으면 패스
	if( NULL_ID == m_dwSlotItem[nSlot] )
		return;

	//해당 슬롯을 판별해서 정보를 shortcut에 넣어준다.
	FLItemBase* pItem = g_pPlayer->GetItemId( m_dwSlotItem[nSlot] );
	if( !pItem )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "cannot find item objid:%d" ), m_dwSlotItem[nSlot] );
		return;
	}
	
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
	m_GlobalShortcut.m_dwId       = m_dwSlotItem[nSlot];
	m_GlobalShortcut.m_dwIndex    = pItem->m_dwItemId; // ->GetProp()->dwID;
	m_GlobalShortcut.m_pTexture   = pItem->GetTexture( );
	m_GlobalShortcut.m_dwData     = 0;
}

void CWndComposeTwohandWeapon::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	int nSlot = -1;
	for( int i = 0; i < SLOT_MAX; ++i )
	{
		rect = GetSlotRect( i );
		if( PtInRect( &rect, point ) )
		{
			//ok
			ResetSlot( i );
			break;
		}
	}
}

void CWndComposeTwohandWeapon::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pMsgBoxStart )
	{
		SAFE_DELETE( m_pMsgBoxStart );
	}
	else if( pWndChild == m_pMsgBoxClose )
	{
		SAFE_DELETE( m_pMsgBoxClose );
	}
}

BOOL CWndComposeTwohandWeapon::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId( ) )
	{
		WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_PICTURE1 );
		WNDCTRL* pWndSlot2 = GetWndCtrl( WIDC_PICTURE2 );
		WNDCTRL* pWndSlot3 = GetWndCtrl( WIDC_PICTURE3 );

		if( NULL == pWndSlot1 || NULL == pWndSlot2 || NULL == pWndSlot3 )
			return FALSE;

		//discern slot
		if( PtInRect( pWndSlot1->rect, point ) )
		{
			//제 1 무기슬롯
			RegItem( pShortcut->m_dwId, SLOT_1 );
		}
		else if( PtInRect( pWndSlot2->rect, point ) )
		{
			//제 2 무기슬롯
			RegItem( pShortcut->m_dwId, SLOT_2 );
		}
		else if( PtInRect( pWndSlot3->rect, point ) )
		{
			// 캐시 아이템
			RegItem( pShortcut->m_dwId, SLOT_3 );
		}

	}

	return TRUE;
	
}

//--------------------------------------------------------------------------------------------------------------------------------------
//CWndCancel_ComposeWeapon
CWndCancel_ComposeWeapon::CWndCancel_ComposeWeapon( )
{
	m_pGauge = NULL;
	m_bNoCloseButton = TRUE;
	m_dwSlotItem = NULL_ID;
	m_fRemainSec = 0.0f;
	m_fMaxSec = 6.0f;

	m_pMsgBox = NULL;
	m_bDirection = FALSE;
}

CWndCancel_ComposeWeapon::~CWndCancel_ComposeWeapon( )
{
	ResetSlot( );
	SAFE_DELETE( m_pMsgBox );
	m_texGauFillNormal.DeleteDeviceObjects( );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCancel_ComposeWeapon::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WEAPONMERGE_CANCEL, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndCancel_ComposeWeapon::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndColoGageBar01.tga" ), 0xffff00ff, TRUE );

	//test
	//SetTimer( 6.0f );
	Reset( );
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndCancel_ComposeWeapon::Process( void )
{
	UpdateButton( );

	if( m_fRemainSec > 0.0f )
	{
		m_fRemainSec -= _GetDeltaTimeSec( );
		if( m_fRemainSec < 0.0f )
		{
			m_fRemainSec = 0.0f;
			SendPakcet( );
		}
	}

	WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_PICTURE1 );

	if( NULL == pWndSlot1 )
		return FALSE;

	CPoint point = GetMousePoint();

	//discern slot
	CRect rectSlot;
	FLItemElem* pItem = NULL; 
	if( NULL_ID != m_dwSlotItem && PtInRect( pWndSlot1->rect, point ) )
	{
		//제 1 무기슬롯
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem );
		rectSlot = pWndSlot1->rect;
	}

	if( pItem )
	{
		CPoint Point2 = point;
		ClientToScreen( &Point2 );
		ClientToScreen( &rectSlot );
		g_WndMng.PutToolTip_Item( pItem, Point2, &rectSlot );
	}

	return TRUE;
}

BOOL CWndCancel_ComposeWeapon::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( m_pMsgBox && pWndBase == m_pMsgBox )
	{
		if( IDOK == nID )
		{
			//ok, sendpaket
			DoDirection( );
			m_pMsgBox->Destroy();
		}
		else if( IDCANCEL == nID )
		{
			m_pMsgBox->Destroy();
		}

	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndCancel_ComposeWeapon::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_START:
		m_pMsgBox = new CWndMessageBox;
		if( FALSE == m_pMsgBox->Initialize( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_CANCEL_CONFIRM ), this, MB_OKCANCEL ) )		//진행의사 물음
		{
			SAFE_DELETE( m_pMsgBox );
		}

	//	DoDirection( );
		break;

	case WIDC_CANCEL:
		Destroy( );
		break;
	
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCancel_ComposeWeapon::OnDraw( C2DRender* p2DRender ) 
{
	if( NULL_ID != m_dwSlotItem )
	{
		FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem );
		if( pItem )
		{
			WNDCTRL* pCtrl = GetWndCtrl( WIDC_PICTURE1 );
			if( pCtrl )
				p2DRender->RenderTexture( pCtrl->rect.TopLeft(), pItem->GetTexture() );
		}
	}

	//gauge
	if( m_bDirection )
	{
		MakeGaugeRect( );
		m_pTheme->RenderGauge( p2DRender, &m_rectGauge, 0xffffffff, m_pGauge, &m_texGauFillNormal );
	}
}

BOOL CWndCancel_ComposeWeapon::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId( ) )
	{
		WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_PICTURE1 );
	
		if( NULL == pWndSlot1 )
			return FALSE;

		//discern slot
		if( PtInRect( pWndSlot1->rect, point ) )
		{
			//제 1 무기슬롯
			SetSlotItem( pShortcut->m_dwId );
		}
		
	}

	return TRUE;
}


void CWndCancel_ComposeWeapon::OnLButtonDown( UINT nFlags, CPoint point )
{
	//아이템이 등록 안되어 있으면 패스
	if( NULL_ID == m_dwSlotItem )
		return;

	WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_PICTURE1 );
	if( NULL == pWndSlot1 )
		return;

	int nSlot = -1;
	
    if( PtInRect( &pWndSlot1->rect, point ) )
	{
		//ok
		//해당 슬롯을 판별해서 정보를 shortcut에 넣어준다.
		FLItemBase* pItem = g_pPlayer->GetItemId( m_dwSlotItem );
		if( !pItem )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "cannot find item objid:%d" ), m_dwSlotItem );
			return;
		}
		
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
		m_GlobalShortcut.m_dwId       = m_dwSlotItem;
		m_GlobalShortcut.m_dwIndex    = pItem->m_dwItemId; // ->GetProp()->dwID;
		m_GlobalShortcut.m_pTexture   = pItem->GetTexture( );
		m_GlobalShortcut.m_dwData     = 0;
	}

}

void CWndCancel_ComposeWeapon::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pMsgBox )
	{
		SAFE_DELETE( m_pMsgBox );
	}
}

void CWndCancel_ComposeWeapon::SendPakcet( )
{
	FLPacketItemUpgradeCombineInitializeReq kPacket;
	kPacket.dwMainItemObjID = m_dwSlotItem;
	g_DPlay.SendPacket( &kPacket );
}

void CWndCancel_ComposeWeapon::SetSlotItem( DWORD dwItemId )
{
	//TODO: 가능한 아이템인지 검사
	FLItemElem* pItem = (FLItemElem*)g_pPlayer->GetItemId( dwItemId );
	if( NULL == pItem )
		return;

	if( false == pItem->IsSetCombinedOption( ) )
	{
		//g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_TWOHANDEDWEAPONMERGE_ERRORTEXT011 ) );	//합성한 아이템만 가능
		return;
	}

	pItem->SetExtra( pItem->GetExtra() + 1 );

	m_dwSlotItem = dwItemId;

	//필요페냐 요청
	FLPacketItemUpgradeCombineInitializeDataReq kPacket;
	kPacket.dwMainItemObjID = dwItemId;
	g_DPlay.SendPacket( &kPacket );

	////필요 페냐
	//PT_ITEM_SPEC pProp = pItem->GetProp( );
	//if( NULL == pProp )
	//	return;

	//DWORD dwNeedPenya = 0;
	//switch( pProp->dwItemGrade )
	//{
	//case ITEM_GRADE_NORMAL: 
	//	dwNeedPenya = pProp->dwLimitLevel1 * 5000;
	//	break;

	//case ITEM_GRADE_UNIQUE:
	//	dwNeedPenya = pProp->dwLimitLevel1 * 10000;
	//	break;

	//case ITEM_GRADE_ULTIMATE:
	//	dwNeedPenya = pProp->dwLimitLevel1 * 15000;
	//	break;

	//default:
	//	FLASSERT( 0 );
	//	break;
	//}

	//CString strBuffer;
	//strBuffer.Format( "%d", (int)dwNeedPenya );

	//CWndStatic* pWndNeedPenya = ( CWndStatic* )GetDlgItem( WIDC_ST_PENYA02 );
	//if( pWndNeedPenya )
	//	pWndNeedPenya->SetTitle( strBuffer );
}

void CWndCancel_ComposeWeapon::ResetSlot( )
{
	FLItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem );
	if( pItemElem )
		pItemElem->SetExtra( 0 );

	m_dwSlotItem = NULL_ID;

	UpdateButton( );
}

void CWndCancel_ComposeWeapon::DropedIcon( DWORD dwItemId )
{
	//본 윈도우부터 아이콘을 빼냄
	ResetSlot( );
}

void CWndCancel_ComposeWeapon::SetTimer( float fMaxSec )
{
	m_fMaxSec = fMaxSec;
	m_fRemainSec = fMaxSec;
}

void CWndCancel_ComposeWeapon::Reset( )
{
	CWndStatic* pWndNeedPenya = ( CWndStatic* )GetDlgItem( WIDC_ST_PENYA02 );
	if( pWndNeedPenya )
	{
		pWndNeedPenya->SetTitle( "" );
		pWndNeedPenya->AddWndStyle( WSS_MONEY );
	}

	ResetSlot( );

	m_bDirection = FALSE;
}

void CWndCancel_ComposeWeapon::ReceivedPacket( const FLSnapshotItemUpgradeCombineInitializeDataAck& kData )
{
	if( false == kData.bCanInitialize )
	{
		g_WndMng.OpenMessageBox( "등록할수 없는아이템 확인요망 ID필요" );
		Reset( );
		return;
	}

	int nNeedPenya = kData.nInitializePenya;

	CWndStatic* pWndNeedPenya = ( CWndStatic* )GetDlgItem( WIDC_ST_PENYA02 );
	if( pWndNeedPenya )
	{
		CString strBuffer;
		strBuffer.Format( "%d", nNeedPenya );
		pWndNeedPenya->SetTitle( strBuffer );
	}
}

void CWndCancel_ComposeWeapon::ReceivedPacket_Result( const DWORD dwResult )
{
	if( FSC_ITEMUPGRADE_UPGRADE_SUCCESS == dwResult )
	{
		// 성공
		Reset( );
	}
	else if( FSC_ITEMUPGRADE_UPGRADE_FAIL == dwResult )
	{
		//실패
	}
	else	// if( E_UPGRADE_ERROR == byResult )
	{
		//조건 불충분. 
		Reset( );
	}

	m_bDirection = FALSE;
}

void CWndCancel_ComposeWeapon::MakeGaugeRect( )
{
	if( m_fMaxSec <= 0.0f )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_fMaxSec %f" ), m_fMaxSec );
		return;
	}

	LPWNDCTRL lpEXP   = GetWndCtrl( WIDC_PC_DELAY );
	if( NULL == lpEXP )
		return;

	m_rectGauge = lpEXP->rect;

	int nWidthClient = lpEXP->rect.Width();
	float fRatio = (m_fMaxSec - m_fRemainSec) / m_fMaxSec;		//백분율( 0 ~ 1 )
	int nOffsetRight = (int)( nWidthClient * fRatio ) ;

	m_rectGauge.right = m_rectGauge.left + nOffsetRight;
}

void CWndCancel_ComposeWeapon::UpdateButton( )
{
	CWndButton* pWndBtnStart = ( CWndButton* )GetDlgItem( WIDC_START );
	if( NULL == pWndBtnStart )
		return;

	pWndBtnStart->EnableWindow( (NULL_ID != m_dwSlotItem) );
}

void CWndCancel_ComposeWeapon::DoDirection( )			//연출시작
{
	SetTimer( 2.0f );
	m_bDirection = TRUE;
}

HRESULT CWndCancel_ComposeWeapon::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pGauge, NULL );
	}

	FLASSERT( m_pGauge );
	
	return S_OK;
}
HRESULT CWndCancel_ComposeWeapon::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGauge );

	return S_OK;//return S_OK;
}
HRESULT CWndCancel_ComposeWeapon::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

//----------------------------------------------------------------------------------------------------------
//CWndChangeLook
//----------------------------------------------------------------------------------------------------------
CWndChangeLook::CWndChangeLook( )
{
	m_bNoCloseButton = TRUE;
	m_pMsgBoxConfirm = NULL;
	m_pVirtualRstItem = NULL;
}

CWndChangeLook::~CWndChangeLook( )
{
	ResetSlot( SLOT_MAX );

	SAFE_DELETE( m_pMsgBoxConfirm );
	SAFE_DELETE( m_pVirtualRstItem );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeLook::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_LOOKCHANGE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndChangeLook::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	ResetSlot( SLOT_MAX );
	UpdateButton( );
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndChangeLook::Process( void )
{
	WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_STATIC1 );
	WNDCTRL* pWndSlot2 = GetWndCtrl( WIDC_STATIC2 );
	WNDCTRL* pWndSlot3 = GetWndCtrl( WIDC_STATIC3 );

	if( NULL == pWndSlot1 || NULL == pWndSlot2 || NULL == pWndSlot3 )
		return FALSE;

	CPoint point = GetMousePoint();

	//discern slot
	CRect rectSlot;
	FLItemElem* pItem = NULL; 
	if( NULL_ID != m_dwSlotItem[SLOT_1] && PtInRect( pWndSlot1->rect, point ) )
	{
		//제 1 무기슬롯
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_1 ] );
		rectSlot = pWndSlot1->rect;
	}
	else if( PtInRect( pWndSlot2->rect, point ) )
	{
		//제 2 무기슬롯
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_2 ] );
		rectSlot = pWndSlot2->rect;
	}
	else if( PtInRect( pWndSlot3->rect, point ) )
	{
		// 결과물
		pItem = m_pVirtualRstItem;
		rectSlot = pWndSlot3->rect;
	}

	if( pItem )
	{
		CPoint Point2 = point;
		ClientToScreen( &Point2 );
		ClientToScreen( &rectSlot );
		g_WndMng.PutToolTip_Item( pItem, Point2, &rectSlot );
	}

	return TRUE;
}

void CWndChangeLook::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pMsgBoxConfirm )
	{
		SAFE_DELETE( m_pMsgBoxConfirm );
	}
}

BOOL CWndChangeLook::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( pWndBase == m_pMsgBoxConfirm )
	{
		if( IDOK == nID )
		{
			//ok, sendpaket
			SendPacket( );
			m_pMsgBoxConfirm->Destroy( );
		}
		else if( IDCANCEL == nID )
		{
			m_pMsgBoxConfirm->Destroy();
		}
	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndChangeLook::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_BUTTON1:		//확인
		{
			SAFE_DELETE( m_pMsgBoxConfirm );
			m_pMsgBoxConfirm = new CWndMessageBox;
			BOOL bInit = m_pMsgBoxConfirm->Initialize( prj.GetText( TID_MMI_LOOKCHAGE11 ), this, MB_OKCANCEL );		//깨질수있음 진짜할래여?
			if( FALSE == bInit )
			{
				SAFE_DELETE( m_pMsgBoxConfirm );
				FLERROR_LOG( PROGRAM_NAME, "init failed" );
			}
		}

		break;

	case WIDC_BUTTON2:		//취소
		Destroy( );
		break;

	default: break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndChangeLook::OnDraw( C2DRender* p2DRender ) 
{
	CRect rect;
	FLItemElem* pItem = NULL;
	for( int i = 0; i <= SLOT_2; ++i )
	{
		if( NULL_ID == m_dwSlotItem[ i ] )
			continue;
		
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ i ] );

		if( pItem )
		{
			rect = GetSlotRect( i );
			rect.left += 2;
			rect.top += 2;
			p2DRender->RenderTexture( rect.TopLeft(), pItem->GetTexture(), 255, 1.2f, 1.2f );
		}
	}

	//SLOT_3은 가상의 결과물이기때문에 인벤에서 찾을수없다.
	if( m_pVirtualRstItem )
	{
		pItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_2 ] );

		if( pItem )
		{
			//2번슬롯이 아이콘으로 보여줘야 한다.
			rect = GetSlotRect( SLOT_3 );
			rect.left += 2;
			rect.top += 2;
			p2DRender->RenderTexture( rect.TopLeft(), pItem->GetTexture( ), 255, 1.2f, 1.2f );
		}
	}

}

CRect CWndChangeLook::GetSlotRect( int nSlot )
{
	DWORD dwWndId = 0;

	switch( nSlot )
	{
	case SLOT_1: dwWndId = WIDC_STATIC1; break;
	case SLOT_2: dwWndId = WIDC_STATIC2; break;
	case SLOT_3: dwWndId = WIDC_STATIC3; break;
	default: 
		FLASSERT( 0 );
		break;
	}

	WNDCTRL* pWnd = GetWndCtrl( dwWndId );
	if( pWnd )
		return pWnd->rect;

	return CRect( 0, 0, 0, 0 );
}

void CWndChangeLook::UpdateSlot3( )
{
	if( NULL_ID == m_dwSlotItem[ SLOT_1 ] || NULL_ID == m_dwSlotItem[ SLOT_2 ] )
	{
		SAFE_DELETE( m_pVirtualRstItem );
		return;
	}

	FLItemElem* pItemSlot1 = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_1 ] );
	FLItemElem* pItemSlot2 = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ SLOT_2 ] );

	if( NULL == pItemSlot1 || NULL == pItemSlot2 )
	{
		FLASSERT( 0 );
		FLERROR_LOG( PROGRAM_NAME, _T( "fatal error" ) );
		Destroy( );
		return;
	}
	
	BOOL bNeedCreating = TRUE;
	if( m_pVirtualRstItem )
	{
		//이미 유효하면  pass
		if( m_pVirtualRstItem->m_dwObjId == pItemSlot1->m_dwObjId )
			bNeedCreating = FALSE;
	}

	if( bNeedCreating )
	{
		SAFE_DELETE( m_pVirtualRstItem );
		m_pVirtualRstItem = new FLItemElem;
		*m_pVirtualRstItem = *pItemSlot1;
		m_pVirtualRstItem->SetLooksChangeItemID( pItemSlot2->m_dwObjId );
	}
}

void CWndChangeLook::UpdateButton( )
{
	CWndButton* pBtnOK = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( NULL == pBtnOK )
	{
		FLERROR_LOG( PROGRAM_NAME, "no id %d", WIDC_BUTTON1 );
		Destroy( );
		return;
	}

	if( NULL_ID != m_dwSlotItem[ SLOT_1 ] && NULL_ID != m_dwSlotItem[ SLOT_2 ] )
	{
		//ok
		pBtnOK->EnableWindow( TRUE );
	}
	else
	{
		pBtnOK->EnableWindow( FALSE );
	}
}

void CWndChangeLook::SendPacket( )
{
	if( NULL_ID == m_dwSlotItem[ SLOT_1 ] || NULL_ID == m_dwSlotItem[ SLOT_2 ] )
	{
		FLASSERT( 0 );
		return;
	}

	FLPacketItemUpgradeLooksChangeReq kReq;
	kReq.dwMainItemObjID = m_dwSlotItem[ SLOT_1 ];
	kReq.dwMaterialItemObjID = m_dwSlotItem[ SLOT_2 ];
	g_DPlay.SendPacket( &kReq );
}

void CWndChangeLook::ReceivedPacket( )
{
	ResetSlot( SLOT_MAX );
}

BOOL CWndChangeLook::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	if( APP_INVENTORY == pWndFrame->GetWndId( ) || APP_BAG_EX == pWndFrame->GetWndId( ) )
	{
		WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_STATIC1 );
		WNDCTRL* pWndSlot2 = GetWndCtrl( WIDC_STATIC2 );
	
		if( NULL == pWndSlot1 || NULL == pWndSlot2 )
			return FALSE;

		//discern slot
		if( PtInRect( pWndSlot1->rect, point ) )
		{
			//제 1 슬롯
			RegItem( pShortcut->m_dwId, SLOT_1 );
		}
		else if( PtInRect( pWndSlot2->rect, point ) )
		{
			//제 2 슬롯
			RegItem( pShortcut->m_dwId, SLOT_2 );
		}
		
	}

	return TRUE;
}

void CWndChangeLook::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	WNDCTRL* pWndSlot1 = GetWndCtrl( WIDC_STATIC1 );
	WNDCTRL* pWndSlot2 = GetWndCtrl( WIDC_STATIC2 );

	if( NULL == pWndSlot1 || NULL == pWndSlot2 )
		return;	

	if( PtInRect( pWndSlot1->rect, point ) )
	{
		ResetSlot( SLOT_1 );
	}
	else if( PtInRect( pWndSlot2->rect, point ) )
	{
		ResetSlot( SLOT_2 );
	}
}


void CWndChangeLook::ResetSlot( int nSlot )
{
	FLItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ nSlot ] );

	switch( nSlot )
	{
	case SLOT_1:	
		if( pItemElem )
			pItemElem->SetExtra( 0 );

		m_dwSlotItem[ nSlot ] = NULL_ID;
		UpdateSlot3( );
		UpdateButton( );

		ResetSlot( SLOT_2 );
		break;

	case SLOT_2:	
		if( pItemElem )
			pItemElem->SetExtra( 0 );

		m_dwSlotItem[ nSlot ] = NULL_ID;
		UpdateSlot3( );
		UpdateButton( );
		break;

	case SLOT_3:
		SAFE_DELETE( m_pVirtualRstItem );
		m_dwSlotItem[ nSlot ] = NULL_ID;
		UpdateButton( );
		break;

	case SLOT_MAX:
		{
			for( int i = 0; i < SLOT_MAX; ++i )
			{
				ResetSlot( i );
			}
		}
		break;

	default:
		FLASSERT( 0 );
		break;
	}
}

BOOL CWndChangeLook::RegItem( DWORD dwItemId, int nSlot )
{
	BOOL bReg = FALSE;
	if( -1 == nSlot )
	{
		//자동등록
		for( int i = 0; i < SLOT_MAX; ++i )
		{
			if( NULL_ID == m_dwSlotItem[ i ] )
			{
				bReg = PutItem( i, dwItemId );
				break;
			}
		}
	}
	else
	{
		//수동등록
		bReg = PutItem( nSlot, dwItemId );
	}

	return bReg;
}

void CWndChangeLook::DropedIcon( DWORD dwItemId )
{
	//본 윈도우부터 아이콘을 빼냄
	for( int i = 0; i < SLOT_MAX; ++i )
	{
		if( m_dwSlotItem[ i ] == dwItemId )
		{
			//ok
			ResetSlot( i );
			break;
		}
	}
}

BOOL CWndChangeLook::PutItem( int nSlot, DWORD dwItemId )
{
	//최종적으로 slot에 item을 지정하는 함수
	//각종메세지는 여기서 처리해버리자
	FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtId( dwItemId );
	if( NULL == pItem )
		return TRUE;

	PT_ITEM_SPEC pSpec = pItem->GetProp( );
	if( NULL == pSpec )
		return TRUE;

	//같은 아이템의 경우 pass
	if( m_dwSlotItem[ nSlot ] == dwItemId )
		return TRUE;

	//check valid item
	if( !IsUsableItem( pItem ) )
		return TRUE;

	FLItemElem* pOldItem = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[ nSlot ] );

	BOOL bOK = TRUE;
	switch( nSlot )
	{
	case SLOT_1:		
		{
			PT_ITEM_SPEC pSpec = pItem->GetProp( );
			if( NULL == pSpec )
				return TRUE;

			//1. 외형변경이 이미된경우
			if( 0 != pItem->GetLooksChangeItemID( ) )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHANGE01 ) ); //  "이미 외형변경되어있음 ID필요" );
				return TRUE;
			}

			//2. 
			if( FALSE == pSpec->bCanLooksChange )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHANGE08 ) );	// 외형 적용이 불가능한 아이템입니다. 아이템을 다시 확인 해 주십시요.
				return TRUE;
			}

			//ok succeed
			if( pOldItem )
				pOldItem->SetExtra( 0 );

			m_dwSlotItem[nSlot] = dwItemId;
			pItem->SetExtra( pItem->GetExtra() + 1 );

			ResetSlot( SLOT_2 );
			UpdateButton();
		}
		break;

	case SLOT_2:
		{
			PT_ITEM_SPEC pSpec = pItem->GetProp( );
			if( NULL == pSpec )
				return TRUE;

			FLItemElem* pItemSlot1 = g_pPlayer->m_Inventory.GetAtId( m_dwSlotItem[SLOT_1] );
			if( NULL == pItemSlot1 )
			{
				//1번 슬롯 비어있음.
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHANGE03 ) );
				return TRUE;
			}

			PT_ITEM_SPEC pSpecSlot1 = pItemSlot1->GetProp();
			if( NULL == pSpecSlot1 )
				return TRUE;

			if( 0 != pItem->GetLooksChangeItemID( ) )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHANGE01 ) );
				return TRUE;
			}

			//gmpbigsun( 20120504, #24247 ) : src 아이템이 공용인경우 통과 
			if( pSpec->dwItemSex != NULL_ID && pSpec->dwItemSex != pSpecSlot1->dwItemSex )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHAGE12 ) ); //"성별틀림" );
				return TRUE;
			}

			if( FALSE == pSpecSlot1->bIsLooksChangeMaterial )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHANGE09 ) ); // 외형 추출이 불가능한 아이템입니다. 아이템을 다시 확인 해 주십시요.
				return TRUE;
			}

			if( pSpecSlot1->dwItemKind3 != pSpec->dwItemKind3 || pSpecSlot1->dwHanded != pSpec->dwHanded )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_LOOKCHANGE07 ) ); //"서로다른 파츠의 아이템 ID필요" );
				return TRUE;
			}

			//ok succeed
			if( pOldItem )
				pOldItem->SetExtra( 0 );

			m_dwSlotItem[nSlot] = dwItemId;
			pItem->SetExtra( pItem->GetExtra() + 1 );

			//3번 슬롯에 결과아이템을 표시해주어야함.
			UpdateSlot3( );
			UpdateButton( );
		}
		break;

	default: 
		break;
	}

	return bOK;
}


//----------------------------------------------------------------------------------------------------------
//CWndTargetInfo_HUD
//----------------------------------------------------------------------------------------------------------
CWndTargetInfo_HUD::CWndTargetInfo_HUD( )
{
	m_pGaugeHP = NULL;
	m_pGaugeMP = NULL;
	m_pTexMark = NULL;
}

CWndTargetInfo_HUD::~CWndTargetInfo_HUD( )
{
	m_texGau_HP.DeleteDeviceObjects( );
	m_texGau_MP.DeleteDeviceObjects( );
	m_kTexEff.DeleteDeviceObjects( );

	for( int i = 0; i < ELEMENT_KIND_NUM; ++i )
		m_texAttrCircle[i].DeleteDeviceObjects( );

	for( int i = 0; i < RANK_KIND_NUM; ++i )
	{
		for( int j = 0; j < RANK_EACH_NUM; ++j )
		{
			m_texRank[i][j].DeleteDeviceObjects( );
		}
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTargetInfo_HUD::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TARGETINFORNATION, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndTargetInfo_HUD::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndStatic* pWndStaticName = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 
	if( NULL == pWndStaticName )
		return;

	pWndStaticName->SetFont( CWndBase::m_Theme.m_pFontWndTitle );

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	if( pWndStatic )
		pWndStatic->SetTitle( "" );

	m_texGau_HP.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Targetgauge01.tga" ), 0xffff00ff, TRUE );
	m_texGau_MP.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Targetgauge02.tga" ), 0xffff00ff, TRUE );

	m_texAttrCircle[0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypeRed01.tga" ), 0xffff00ff, TRUE );
	m_texAttrCircle[1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypeBlue01.tga" ), 0xffff00ff, TRUE );
	m_texAttrCircle[2].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypeYellow01.tga" ), 0xffff00ff, TRUE );
	m_texAttrCircle[3].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypeGreen01.tga" ), 0xffff00ff, TRUE );
	m_texAttrCircle[4].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypePurple01.tga" ), 0xffff00ff, TRUE );
	m_texAttrCircle[5].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypeWhite01.tga" ), 0xffff00ff, TRUE );

	m_texRank[0][0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanBlue01.tga" ), 0xffff00ff, TRUE );
	m_texRank[0][1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanBlue02.tga" ), 0xffff00ff, TRUE );
	m_texRank[0][2].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanBlue03.tga" ), 0xffff00ff, TRUE );
	m_texRank[0][3].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanBlue03.tga" ), 0xffff00ff, TRUE );
	m_texRank[0][4].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanBlue02.tga" ), 0xffff00ff, TRUE );
	m_texRank[0][5].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanBlue01.tga" ), 0xffff00ff, TRUE );

	m_texRank[1][0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanGreen01.tga" ), 0xffff00ff, TRUE );
	m_texRank[1][1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanGreen02.tga" ), 0xffff00ff, TRUE );
	m_texRank[1][2].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanGreen03.tga" ), 0xffff00ff, TRUE );
	m_texRank[1][3].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanGreen03.tga" ), 0xffff00ff, TRUE );
	m_texRank[1][4].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanGreen02.tga" ), 0xffff00ff, TRUE );
	m_texRank[1][5].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanGreen01.tga" ), 0xffff00ff, TRUE );

	m_texRank[2][0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanPurple01.tga" ), 0xffff00ff, TRUE );
	m_texRank[2][1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanPurple02.tga" ), 0xffff00ff, TRUE );
	m_texRank[2][2].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanPurple03.tga" ), 0xffff00ff, TRUE );
	m_texRank[2][3].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanPurple03.tga" ), 0xffff00ff, TRUE );
	m_texRank[2][4].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanPurple02.tga" ), 0xffff00ff, TRUE );
	m_texRank[2][5].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanPurple01.tga" ), 0xffff00ff, TRUE );

	m_texRank[3][0].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanRed01.tga" ), 0xffff00ff, TRUE );
	m_texRank[3][1].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanRed02.tga" ), 0xffff00ff, TRUE );
	m_texRank[3][2].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanRed03.tga" ), 0xffff00ff, TRUE );
	m_texRank[3][3].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanRed03.tga" ), 0xffff00ff, TRUE );
	m_texRank[3][4].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanRed02.tga" ), 0xffff00ff, TRUE );
	m_texRank[3][5].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"RanRed01.tga" ), 0xffff00ff, TRUE );

	m_texBaseForNonMonster.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndTargetInfornation02.tga" ), 0xffff00ff, TRUE );

	m_kTexEff.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"eElementTypeEff.tga" ), 0xffff00ff, TRUE );

	CString strBuffer;
	for( int i = 0; i < 10; ++i )
	{
		strBuffer.Format( "TargetNum%d.tga", i );
		m_texZeroToNine[i].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), strBuffer.GetString() ), 0xffff00ff, TRUE );
	}

	D3DXMatrixIdentity( &m_kMat );
		
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	Move( m_rectWindow.left, 0 );
} 

BOOL CWndTargetInfo_HUD::Process( void )
{
	if( NULL == g_WorldMng() )
	{
		SetVisible( FALSE );
		return FALSE;
	}

	CObj* pFocusObj = g_WorldMng()->GetObjFocus();
	if( pFocusObj )
		SetVisible( OT_MOVER == pFocusObj->GetType() );
	else
		SetVisible( FALSE );
	
	return TRUE;
}

void CWndTargetInfo_HUD::OnDestroyChildWnd( CWndBase* pWndChild )
{

}

BOOL CWndTargetInfo_HUD::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndTargetInfo_HUD::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndTargetInfo_HUD::PaintFrame( C2DRender* p2DRender )
{
	CObj* pFocusObj = g_WorldMng()->GetObjFocus();
	if( NULL == pFocusObj )
		return;

	if( pFocusObj->GetType( ) == OT_MOVER )
	{
		CMover* pTarget = static_cast< CMover* > ( pFocusObj );
		if( pTarget->IsMonster( ) )
			SetTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "WndTargetInfornation.tga" ), 0xffff00ff );
		else
			SetTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "WndTargetInfornation02.tga" ), 0xffff00ff );
	}

	CWndNeuz::PaintFrame( p2DRender );
}

void CWndTargetInfo_HUD::OnDraw( C2DRender* p2DRender ) 
{
	if( NULL == g_WorldMng() || NULL == g_pPlayer )
		return;

	CObj* pFocusObj = g_WorldMng()->GetObjFocus();
	if( NULL == pFocusObj )
		return;

	if( OT_MOVER != pFocusObj->GetType( ) )
	{
//		DrawNoMover( p2DRender, pFocusObj );		//정책필요 일단 제거
		return;
	}

	//!  여기서부턴 OT_MOVER!!
	CMover* pTarget = static_cast< CMover* >( pFocusObj );

	//왼쪽 마크 부분만 따로 분기함.
	if( pTarget->IsPlayer() )
	{
		DrawPlayerMark( p2DRender, pTarget );
	}
	else if( pTarget->IsMonster( ) )
	{
		DrawMonsterMark( p2DRender, pTarget );		
	}
	else if( pTarget->IsCitizen() )
	{
		DrawCitizenMark( p2DRender, pTarget );
	}

	//HP & MP
	int nRate = 0;
	int nWidget = 0;
	LPWNDCTRL lpCtrl = NULL;
	CRect rectGauge;
	
	//HP
	nRate = pTarget->GetHitPointPercent( ); 
	if( nRate > 100 )
		nRate = 100;

	lpCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	if( NULL == lpCtrl )
		return;

	rectGauge = lpCtrl->rect;
	nWidget = lpCtrl->rect.Width();
	nWidget = (int)( (float)nWidget * ( (float)nRate * 0.01f ) ) ;
	rectGauge.right = rectGauge.left + nWidget;
	m_pTheme->RenderGauge( p2DRender, &rectGauge, 0xffffffff, m_pGaugeHP, &m_texGau_HP );

	//MP
	nRate = pTarget->GetManaPointPercent(); 
	if( nRate > 100 )
		nRate = 100;

	lpCtrl   = GetWndCtrl( WIDC_CUSTOM2 );
	if( NULL == lpCtrl )
		return;

	//20111226 기획요청
	if( pTarget->IsCitizen() )
		nRate = 100;

	rectGauge = lpCtrl->rect;
	nWidget = lpCtrl->rect.Width();
	nWidget = (int)( (float)nWidget * ( (float)nRate * 0.01f ) ) ;
	rectGauge.right = rectGauge.left + nWidget;
	m_pTheme->RenderGauge( p2DRender, &rectGauge, 0xffffffff, m_pGaugeMP, &m_texGau_MP );
}

void CWndTargetInfo_HUD::DrawMonsterMark( C2DRender* p2DRender, CMover* pMover )
{
	CString strBuffer( _T("") );
	int nX = 0;
	int nY = 0;
	
	CWndStatic* pWndStaticName = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 
	if( NULL == pWndStaticName )
		return;

	//운영자라면 HP표기
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
	{
		p2DRender->SetViewport( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight );
		
		CString string;
		string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_INFORMATION_MONSTER ), pMover->GetHitPoint(), pMover->GetMaxHitPoint() );
		p2DRender->TextOut( GetClientRect().left+ 20, GetClientRect().bottom + 30, string.GetString() );

		p2DRender->SetViewport( m_rectWindow );
	}


	DWORD dwColor = C_NORMAL;
	if( pMover->IsDie( ) )
		dwColor = C_DIE;
	else if( pMover->m_bActiveAttack )
		dwColor = C_ATTACK;

	strBuffer.Format( "%s", pMover->GetName() );

	DrawObjectName( p2DRender, strBuffer, dwColor );

	//레벨
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	if( NULL == lpWndCtrl4 )
		return;

	MoverProp* pProp = pMover->GetProp();
	if( NULL == pProp )
		return;

	//속성
	int nEleIndex = 0;
	switch( pProp->eElementType )
	{
	case SAI79::NO_PROP:		nEleIndex = 5;		break;
	case SAI79::FIRE:			nEleIndex = 0;		break;
	case SAI79::WATER:			nEleIndex = 1;		break;
	case SAI79::ELECTRICITY:	nEleIndex = 2;		break;
	case SAI79::WIND:			nEleIndex = 3;		break;
	case SAI79::EARTH:			nEleIndex = 4;		break;
	default:
		FLASSERT( 0 );
		break;
	}

	m_texAttrCircle[nEleIndex].Render( p2DRender, lpWndCtrl4->rect.TopLeft( ) );

	//m_kMat._41 += 0.001f;
	//m_kMat._42 += 0.001f;

	//if( m_kMat._41 > 1.0f )
	//	m_kMat._41 = 0.0f;

	//if( m_kMat._42 > 1.0f )
	//	m_kMat._42 = 0.0f;

	//p2DRender->RenderTextureWithEffect2( lpWndCtrl4->rect.TopLeft(), &m_texAttrCircle[nEleIndex], &m_kTexEff, &m_kMat );
	
	int nLevel = pMover->GetLevel();

	//너무 막생각해서...좀그렇다..최적화 필요
	if( m_texZeroToNine[0].m_pTexture && nLevel <= 999 )
	{
		int nNofN[3] = { -1, -1, -1 };
		SIZE kSize = m_texZeroToNine[0].m_size;
		int nTotalSize = 0;
		int nMax = 0;
		if( nLevel / 100 )
			nMax = 2;
		else if( nLevel / 10 )
			nMax = 1;
		else nMax = 0;

		for( int i = nMax; i >= 0; --i )
		{
			nNofN[i] = nLevel / (int)pow( 10.0f, i );
			nLevel = nLevel - nNofN[i] * (int)(pow( 10.0f, i ));
			if( nNofN[i] >= 0 )
			{
				if( nNofN[i] > 9 )
					nNofN[i] = 9;

				if( nNofN[i] >= 0 )
					nTotalSize += ( kSize.cx );
			}
		}

		int nX = 0, nY = 0;
		int nStartX = (nTotalSize / kSize.cx) - 1;
		for( int i = 0; i < 3; ++i)
		{
			if( nNofN[i] >= 0 ) 
			{
				nX = lpWndCtrl4->rect.left + ( lpWndCtrl4->rect.Width() / 2 - nTotalSize / 2 + ( nStartX * kSize.cx ) );
				nY = lpWndCtrl4->rect.top + ( lpWndCtrl4->rect.Height() / 2 - ( kSize.cy / 2 ) );

				p2DRender->RenderTexture( CPoint( nX, nY ), &m_texZeroToNine[nNofN[i]] );
			}

			--nStartX;
		}
	}

	//rank
	DWORD dwRankUI_id[ 6 ] = { WIDC_CUSTOM10, WIDC_CUSTOM9, WIDC_CUSTOM8, WIDC_CUSTOM6, WIDC_CUSTOM5, WIDC_CUSTOM7 };

	int nMax = 0;
	switch( pProp->dwClass )
	{
	case RANK_MATERIAL:		nMax = 0;		break;
	case RANK_LOW:			nMax = 1;		break;
	case RANK_NORMAL:		nMax = 2;		break;
	case RANK_CAPTAIN:		nMax = 3;		break;
	case RANK_MIDBOSS:		nMax = 4;		break;
	case RANK_BOSS:			nMax = 5;		break;
	case RANK_SUPER:		nMax = 6;		break;
	default:
		break;
	}

	//pProp->dwAreaColor별로 색상을 결정해야함.
	LPWNDCTRL lpRankSlot = NULL;
	for( int i = 0; i < nMax; ++i )
	{
		lpRankSlot = GetWndCtrl( dwRankUI_id[ i ] );
		if( NULL == lpRankSlot )
			continue;

		//test
		int nKind = 0;
		switch( pProp->dwAreaColor )
		{
		case AREA_NORMAL:			nKind = 0;		break;
		case AREA_DUNGEON:			nKind = 1;		break;
		case AREA_CASH:				nKind = 2;		break;
		case AREA_INSTANCE:			nKind = 3;		break;
		default:	
			break;
		}
		m_texRank[nKind][i].Render( p2DRender, lpRankSlot->rect.TopLeft() );
	}
}


//---------------------------------------------------------------------------------------------
// 선택 된 오브젝트의 이름을 출력한다.
// param	: 렌더러
// return	: void
// 설명		: 번역의 경우 이름이 많이 긴 경우가 있어서 강제적으로 뷰포트를 늘려준다.
//---------------------------------------------------------------------------------------------
void CWndTargetInfo_HUD::DrawObjectName( C2DRender* p2DRender, const CString strName , const DWORD dwFontColor )
{
	LPDIRECT3DDEVICE9 pd3dDevice( p2DRender->m_pd3dDevice );
	if( NULL == pd3dDevice )
		return;

	CWndStatic* pWndStaticName = (CWndStatic*)GetDlgItem( WIDC_STATIC1 ); 
	if( NULL == pWndStaticName )
		return;

	pWndStaticName->SetTitle( _T("") );

	D3DVIEWPORT9 OldViewPort;
	pd3dDevice->GetViewport( &OldViewPort );

	D3DVIEWPORT9 NewViewPort( OldViewPort );
	NewViewPort.Width = OldViewPort.Width * 3;

	pd3dDevice->SetViewport( &NewViewPort );

	CD3DFont* pOldFont = p2DRender->GetFont(); 
	CD3DFont* pNewFont = m_Theme.GetFont( _T("FontWndTitle") );

	if( pNewFont )
		p2DRender->SetFont( pNewFont );

	p2DRender->TextOut( pWndStaticName->GetClientRect( TRUE ).left, pWndStaticName->GetClientRect( TRUE ).top, strName, dwFontColor );

	if( pOldFont )
		p2DRender->SetFont( pOldFont );

	pd3dDevice->SetViewport( &OldViewPort );
}



void CWndTargetInfo_HUD::DrawCitizenMark( C2DRender* p2DRender, CMover* pMover )
{
	MoverProp* pProp = pMover->GetProp();
	if( NULL == pProp )
		return;

	int nX = 0;
	int nY = 0;

	//이름 & 레벨
//	strBuffer.Format( "%s <Lv. %d>", pMover->GetName(), pMover->GetLevel() );		//20120102: 이름만 표기되도록 변경됨.

	CString strBuffer;
	strBuffer.Format( "%s", pMover->GetName() );

	DrawObjectName( p2DRender, strBuffer );

	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_STATIC2 );
	if( NULL == lpWndCtrl4 )
		return;

	//마크 
	if( lstrcmp (pProp->szMarkFileName, "=" ) != 0 )
	{
		CString strFilename;
		strFilename.Format( "%s.tga", pProp->szMarkFileName );
		m_pTexMark = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), strFilename.GetString() ), 0xffff00ff, TRUE );
	
		if( m_pTexMark )
			m_pTexMark->Render( p2DRender, lpWndCtrl4->rect.TopLeft() );
	}

	//레벨
//	int nLevel = pMover->GetLevel();
//	strBuffer.Format( "%d", nLevel );
//	CSize kSize = p2DRender->GetFont()->GetTextExtent( strBuffer.GetString() );
//	nX = lpWndCtrl4->rect.left + ( lpWndCtrl4->rect.Width() / 2 - kSize.cx / 2 );
//	nY = lpWndCtrl4->rect.top + (lpWndCtrl4->rect.Height() / 2);
//	p2DRender->TextOut( nX, nY, 1.0f, 1.0f, strBuffer.GetString(), 0xffaaaaaa  );
}

void CWndTargetInfo_HUD::DrawPlayerMark( C2DRender* p2DRender, CMover* pMover )
{
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_STATIC2 );
	if( NULL == lpWndCtrl4 )
		return;

	CString strBuffer;
	int nX = 0;
	int nY = 0;

	//운영자라면 레벨표기
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
	{
		CString string;

		if(pMover->IsMaster())
			string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_LEVEL_PLAYER_MASTER ), pMover->GetName(), pMover->GetLevel() );
		else if(pMover->IsHero())
			string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_LEVEL_PLAYER_HERO ), pMover->GetName(), pMover->GetLevel() );
		else
			string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_LEVEL_PLAYER ), pMover->GetName(), pMover->GetLevel() );

		p2DRender->SetViewport( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight );
		p2DRender->TextOut( GetClientRect().left, GetClientRect().bottom + 48 + 4, string.GetString(), COLOR_PLAYER );

		if( pMover->IsPlayer() )
			string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_INFORMATION_PLAYER ), pMover->GetHitPoint(), pMover->GetMaxHitPoint(), pMover->GetGold(), (float)pMover->GetExpPercent()/100.0f, pMover->GetExp1(), pMover->GetMaxExp1() );
		else
			string.Format( prj.GetText( TID_GAME_SELECT_OBJECT_INFORMATION_MONSTER ), pMover->GetHitPoint(), pMover->GetMaxHitPoint() );
		
		p2DRender->TextOut( GetClientRect().left+ 4, GetClientRect().bottom + 48 + 18, string.GetString() );

		p2DRender->SetViewport( m_rectWindow );
	}
	
	strBuffer = pMover->GetName();
	
	DWORD dwColor = C_NORMAL;
	if( pMover->IsDie( ) )
		dwColor = C_DIE;
	else if( pMover->IsChaotic( ) )
		dwColor = C_CAO;

	DrawObjectName( p2DRender, strBuffer, dwColor );

	//마크 
	JobProp* pProperty = prj.GetJobProp( pMover->GetJob() ); 
	if( pProperty )
	{
		CString strFileName;
		strFileName.Format( "%s.tga", pProperty->szMarkFileName );
		m_pTexMark = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), strFileName.GetString() ), 0xffff00ff, TRUE );
	}

	if( m_pTexMark )
		m_pTexMark->Render2( p2DRender, lpWndCtrl4->rect.TopLeft(), 0xffffffff );
}

void CWndTargetInfo_HUD::DrawMoverBuff( C2DRender* p2DRender, CMover* pMover )
{
	if( NULL == g_WndMng.m_pWndWorld )
		return;

	g_WndMng.m_pWndWorld->RenderMoverBuff( pMover, p2DRender );
}

void CWndTargetInfo_HUD::DrawNoMover( C2DRender* p2DRender, CObj* pObj )
{
	if( NULL == pObj )
		return;
	
	if( pObj->GetType() != OT_CTRL )
		return;

	if( NULL == g_WndMng.m_pWndWorld )
		return;

	// OT_CTRL타입에 대하여 예전코드를 그대로 가져왔지만 뭐하는지 알수없음. 기획없음.
	BOOL& rCtrlInfo = g_WndMng.m_pWndWorld->m_bCtrlInfo;	

	CCommonCtrl* pCtrl = (CCommonCtrl*)pObj;
	if( pCtrl && rCtrlInfo == FALSE )
	{
		if( pCtrl->m_dwDelete == 0xffffffff )
		{
			g_DPlay.SendExpBoxInfo( pCtrl->GetId() );
		}
		
		rCtrlInfo = TRUE;
	}
	
	if( pCtrl && rCtrlInfo )
	{
		if( pCtrl->m_dwDelete != 0xffffffff )
		{
			CString str,strTemp,strTime;
			CTimeSpan ct( (pCtrl->m_dwDelete - GetTickCount()) / 1000 );
			
			if( pCtrl->m_CtrlElem.m_dwSet & UA_PLAYER_ID )
			{
				CString strName;					

				strName		= CPlayerDataCenter::GetInstance()->GetPlayerString( pCtrl->m_idExpPlayer );	//sun: 11, 캐릭터 정보 통합

				str.Format( prj.GetText(TID_GAME_EXPBOX_INFO), strName );
	
				DrawObjectName( p2DRender, strName, COLOR_MONSTER );
			}
		}
	}
	else
	{
		rCtrlInfo = FALSE;
	}
}

HRESULT CWndTargetInfo_HUD::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pGaugeHP == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pGaugeHP, 
												 NULL);
	FLASSERT(m_pGaugeHP != NULL);

	if( m_pGaugeMP == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pGaugeMP, 
												 NULL);
	FLASSERT(m_pGaugeMP != NULL);
	
	return S_OK;
}
HRESULT CWndTargetInfo_HUD::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pGaugeHP );
	SAFE_RELEASE( m_pGaugeMP );
	return S_OK;
}
HRESULT CWndTargetInfo_HUD::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pGaugeHP );
	SAFE_RELEASE( m_pGaugeMP );
	return S_OK;
}





