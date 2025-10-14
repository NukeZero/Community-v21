#include "stdafx.h"
#include "WndManager.h"
#include "Tax.h"		//sun: 12, 세금

#define PARTSMESH_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.o3d" ) : _T( "Part_femaleHair%02d.o3d" ) )
#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )
#define PARTSMESH_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper.o3d"    ) : _T( "Part_femaleUpper.o3d"    ) )
#define PARTSTEX_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper%02d.dds" ) : _T( "Part_femaleUpper%02d.dds" ) )
#define TEX_PART_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper01.dds" ) : _T( "Part_femaleUpper01.dds" ) )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndWarning::CWndWarning() 
{ 
	m_pMover = NULL;
	m_pItemElem = NULL;
} 
CWndWarning::~CWndWarning() 
{ 

} 

BOOL CWndWarning::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_KEY, 0, pWndParent );
	MoveParentCenter();
//	CWndStatic* pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC1 );
//	pLabel->m_dwColor = 0xffff0000;
//	pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC2 );
//	pLabel->m_dwColor = 0xffff0000;

	return TRUE;
}

BOOL CWndWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 

	switch(nID)
	{
	case WIDC_OK:
		{
			switch(_GetContentState(CT_SHOP_CART_17))
			{
			case CS_VER1:
				{
					SAFE_DELETE( ((CWndShop*)m_pParentWnd)->m_pWndConfirmSell );
					((CWndShop*)m_pParentWnd)->m_pWndConfirmSell = new CWndConfirmSell;
					((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->m_pItemElem = m_pItemElem;
					((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->m_pMover = m_pMover;
					((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->Initialize( m_pParentWnd, APP_CONFIRM_SELL );
				}
				break;
			case CS_VER2:
				{
					SAFE_DELETE( ((CFLWndShop_Cart*)m_pParentWnd)->m_pWndConfirmSell );
					((CFLWndShop_Cart*)m_pParentWnd)->m_pWndConfirmSell = new CWndConfirmSell;
					((CFLWndShop_Cart*)m_pParentWnd)->m_pWndConfirmSell->m_pItemElem = m_pItemElem;
					((CFLWndShop_Cart*)m_pParentWnd)->m_pWndConfirmSell->m_pMover = m_pMover;
					((CFLWndShop_Cart*)m_pParentWnd)->m_pWndConfirmSell->Initialize( m_pParentWnd, APP_CONFIRM_SELL );
				}
				break;
			default:
				FLASSERT(0);
				break;
			}
			Destroy();
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmSell::CWndConfirmSell() 
{ 
	m_pMover = NULL;
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_pStatic = NULL;
} 
CWndConfirmSell::~CWndConfirmSell() 
{ 
} 

BOOL CWndConfirmSell::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}

void CWndConfirmSell::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem->IsEmpty() )
	{
		m_pStaticGold->SetTitle( CString( "0" ) );
		return;
	}
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi( szNumber );

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
	}
	else
	{
		if( m_pItemElem->m_nItemNum < nNumber )
		{
			char szNumberbuf[16] = {0, };
			nNumber = m_pItemElem->m_nItemNum;
			FLIntToString( m_pItemElem->m_nItemNum, szNumberbuf, _countof( szNumberbuf ), 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		for( int i = 0 ; i < 8 ; i++ )
		{
			char szNumberbuf[8] = {0, };
			FLStrncpy( szNumberbuf, _countof( szNumberbuf ), szNumber, 8 );
			
			// 0 : 공백, 48 : 숫자 0, 57 : 숫자 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = m_pItemElem->m_nItemNum;
					FLIntToString( m_pItemElem->m_nItemNum, szNumberbuf, _countof( szNumberbuf ), 10 );
					m_pEdit->SetString( szNumberbuf );
					break;												
				}
			}
		}
	}
	DWORD dwCost = m_pItemElem->GetCost();
	DWORD BuyGold = dwCost / 4;
	BuyGold = static_cast< DWORD >( static_cast< float >( BuyGold ) * prj.m_fShopSellRate );
//sun: 12, 세금
	if( CTax::GetInstance()->IsApplySalesTaxRate( g_pPlayer, m_pItemElem ) )
		BuyGold -= ( static_cast<DWORD>(BuyGold * CTax::GetInstance()->GetSalesTaxRate( g_pPlayer )) );
//	TCHAR szNumberGold[ 64 ];
	CString szNumberGold;


	if( BuyGold < 1 )
		BuyGold = 1;

	BuyGold *= nNumber;
//	_itot( BuyGold, szNumberGold, 10 );
	szNumberGold.Format("%u", BuyGold);
	m_pStaticGold->SetTitle( szNumberGold );
} 
void CWndConfirmSell::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pOk   = (CWndButton*)GetDlgItem( WIDC_OK );
	CWndBase* pEdit   = (CWndButton*)GetDlgItem( WIDC_EDITSELL );
	CWndStatic* pStatic = (CWndStatic  *)GetDlgItem( WIDC_STATIC2 );
	pStatic->AddWndStyle(WSS_MONEY);
	pOk->SetDefault( TRUE );
	pEdit->SetFocus();
	
	
} 
BOOL CWndConfirmSell::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_SELL, WBS_KEY, 0, pWndParent );
	MoveParentCenter();

	CWndStatic* pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC );
//	pLabel->m_dwColor = 0xff000000;
	
	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_EDITSELL );
	m_pStatic	= (CWndStatic *)GetDlgItem( WIDC_CONTROL1 );
	m_pStaticGold = (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
//	m_pStatic->m_dwColor = m_pStaticGold->m_dwColor = 0xff000000;
	
	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
//		m_pEdit->SetVisible( FALSE );
	}
	else
	{
		TCHAR szNumber[ 64 ];
		FLIntToString( m_pItemElem->m_nItemNum, szNumber, _countof( szNumber ), 10 );
		m_pEdit->SetString( szNumber );
	}
	DWORD dwCost = m_pItemElem->GetCost();
	DWORD BuyGold = dwCost / 4;
	BuyGold = static_cast< DWORD >( static_cast< float >( BuyGold ) * prj.m_fShopSellRate );
//sun: 12, 세금
	BuyGold -= ( static_cast<DWORD>(BuyGold * CTax::GetInstance()->GetSalesTaxRate( g_pPlayer )) );

	if( BuyGold < 1 )
		BuyGold = 1;

	BuyGold *= m_pItemElem->m_nItemNum;
//	TCHAR szNumberGold[ 64 ];
	CString szNumberGold;
	szNumberGold.Format("%u", BuyGold);
//	_itot( BuyGold, szNumberGold, 10 );
	m_pStaticGold->SetTitle( szNumberGold );	
	return TRUE;
}
BOOL CWndConfirmSell::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndConfirmSell::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndConfirmSell::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndConfirmSell::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndConfirmSell::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		int SellNum = 0;
		if( m_pItemElem->m_nItemNum >= 1 )
		{
			SellNum = atoi( m_pEdit->GetString() );
		}
		if( SellNum != 0 )
		{
			m_pItemElem->m_bControlForbidden = TRUE;
			g_DPlay.SendSellItem( m_pItemElem->m_dwObjId, SellNum );
		}
		PLAYSND( SND_INF_TRADE );
	}
	if( nID == WIDC_CANCEL )
	{
	}
	if( nID != WIDC_EDITSELL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmBuy::CWndConfirmBuy() 
{ 
	m_pMover = NULL;
	m_pItemElem = NULL;
	m_dwItemId	= 0;
	m_pEdit = NULL;
	m_pStatic = NULL;

	m_nBuyType = 0;		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
} 
CWndConfirmBuy::~CWndConfirmBuy() 
{ 
} 

void CWndConfirmBuy::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
} 

BOOL CWndConfirmBuy::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndConfirmBuy::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndConfirmBuy::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndConfirmBuy::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

// 살려는 수량이 바뀌면, 가격표시도 변경시킨다.
void CWndConfirmBuy::OnChangeBuyCount( DWORD dwBuy )
{
//	TCHAR szString[ 64 ];
	CString szString;

	if( dwBuy != atoi(m_pEdit->GetString()) )
	{
//		_itot( dwBuy, szString, 10 );	// integer to string
		szString.Format("%u", dwBuy);
		m_pEdit->SetString( szString );
	}

	DWORD dwTotalBuy = 0;
	DWORD dwCost = 0;
//sun: 8, // __S8_PK
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
	if(m_nBuyType == 0)
	{
		dwCost = m_pItemElem->GetCost();
		dwCost = static_cast< int >( static_cast< float >( dwCost ) * prj.m_fShopBuyRate );
//sun: 12, 세금
		if( CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, m_pItemElem ) )
			dwCost += ( static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );
	}
	else if(m_nBuyType == 1)
		dwCost = m_pItemElem->GetChipCost();

	dwTotalBuy = (DWORD)( dwBuy * dwCost * prj.m_fShopCost );
//sun: 수표 기능 페린 관련
	if( m_pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
	{
		dwCost = PERIN_VALUE;
		dwTotalBuy = dwBuy * dwCost;
	}

	if(dwTotalBuy > INT_MAX)
	{
		dwBuy--;
		szString.Format("%u", dwBuy);
		m_pEdit->SetString( szString );
		dwTotalBuy = (DWORD)( dwBuy * dwCost * prj.m_fShopCost );
	}
	
	szString.Format("%u", dwTotalBuy);
//	_itot( dwTotalBuy, szString, 10 );	// integer to string

	m_pStaticGold->SetTitle( szString );
}

BOOL CWndConfirmBuy::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_BUY_, 0, 0, pWndParent );
	MoveParentCenter();

	if( m_pItemElem )
		m_dwItemId	= m_pItemElem->m_dwItemId;

	CWndStatic* pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC );
//	pLabel->m_dwColor = 0xff000000;
	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_CONTROL2 );
	m_pStatic	= (CWndStatic *)GetDlgItem( WIDC_CONTROL1 );
	m_pStaticGold = (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
//	m_pStatic->m_dwColor = m_pStaticGold->m_dwColor = 0xff000000;
	m_pStaticGold->AddWndStyle(WSS_MONEY);
	
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
	DWORD dwCost;
	if(m_nBuyType == 0)
	{
		dwCost = m_pItemElem->GetCost();
		dwCost = static_cast< int >( static_cast< float >( dwCost ) * prj.m_fShopBuyRate );
//sun: 12, 세금
		if( CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, m_pItemElem ) )
			dwCost += ( static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );
	}
	else if(m_nBuyType == 1)
		dwCost = m_pItemElem->GetChipCost();

	if( dwCost == 0 )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) ); // "다른 사용자에게 팔렸습니다."
		Destroy();
		return TRUE;
	}

	OnChangeBuyCount( 1 );
	return TRUE;
}

void CWndConfirmBuy::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwCost = m_pItemElem->GetCost();
	dwCost = static_cast< int >( static_cast< float >( dwCost ) * prj.m_fShopBuyRate );
//sun: 12, 세금
	if( CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, m_pItemElem ) )
		dwCost += ( static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );
	if( dwCost == 0 )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) );  // 다른 사용자에게 팔렸습니다.
		Destroy();
		return;
	}

	return;
} 

const int MAX_BUY_ITEMCOUNT = 99;

BOOL CWndConfirmBuy::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	int nBuyNum = 0;

	switch( nID )
	{
	case WIDC_PLUS:
		nBuyNum = atoi(m_pEdit->GetString());
		++nBuyNum;

//sun: 13, 화살포스터구입갯수9999개
		if( m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
			m_pItemElem->GetProp()->dwID == ITEM_INDEX( 26460, II_CHP_RED )
			)
		{
			if ( nBuyNum > 9999 )
				nBuyNum = 9999;
		}
		else
		{
			if ( nBuyNum > MAX_BUY_ITEMCOUNT )
				nBuyNum = MAX_BUY_ITEMCOUNT;
		}

		OnChangeBuyCount(nBuyNum);
		break;

	case WIDC_MINUS:
		nBuyNum = atoi(m_pEdit->GetString());
		if ( --nBuyNum < 1 )
			nBuyNum = 1;

		OnChangeBuyCount(nBuyNum);
		break;

	case WIDC_MAX:
		{

//sun: 13, 화살포스터구입갯수9999개
			if( m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
				m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
				m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
				m_pItemElem->GetProp()->dwID == ITEM_INDEX( 26460, II_CHP_RED )
				)
				OnChangeBuyCount( 9999 );

//sun: 수표 기능 페린 관련
			else if( m_pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
				OnChangeBuyCount( 21 );
			else
				OnChangeBuyCount( MAX_BUY_ITEMCOUNT );
		}		
		break;

	case WIDC_CONTROL2:
		if( EN_CHANGE == message )
		{
			if( m_pEdit == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "m_pEdit == NULL" ) );
				FLERROR_LOG( PROGRAM_NAME, _T( "more info(%d, %d)" ), pLResult, GetDlgItem( WIDC_CONTROL2 ) );

				nBuyNum = 1;
			}
			else
				nBuyNum = atoi(m_pEdit->GetString());

			nBuyNum = max( nBuyNum, 0 );
			DWORD dwMAXCount = MAX_BUY_ITEMCOUNT;
//sun: 13, 화살포스터구입갯수9999개
			if( m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
				m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
				m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
				m_pItemElem->GetProp()->dwID == ITEM_INDEX( 26460, II_CHP_RED )
				)
				dwMAXCount = 9999;

//sun: 수표 기능 페린 관련
			else if( m_pItemElem->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
				dwMAXCount = 21;

			nBuyNum = min( nBuyNum, (int)( dwMAXCount ) );

			OnChangeBuyCount(nBuyNum);
		}
		break;

	case WIDC_CANCEL:
		Destroy();
		break;

	case WIDC_OK:
		OnOK();
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndConfirmBuy::OnOK()
{
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
	//아래 메세지 처리 할 것
	DWORD dwCost;
	int nBuy;

	if(m_nBuyType == 1)
	{
		dwCost = m_pItemElem->GetChipCost();
		if( m_pItemElem->m_nItemNum < 1 || dwCost == 0 )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) );	// 다른 사용자에게 팔렸습니다.
			return;
		}

		nBuy = atoi( m_pEdit->GetString() );
		if( (int)( (nBuy * dwCost) ) > g_pPlayer->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) ) )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_CANNTBUY_REDCHIP) ) );	// 칩이 부족합니다.	
			return;
		}
		
	}
	else if(m_nBuyType == 0)
	{
		dwCost = m_pItemElem->GetCost();
//sun: 12, 세금
		if( m_pItemElem->m_dwItemId != ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
		{
			if( CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, m_pItemElem ) )
				dwCost += ( static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );
		}
		if( m_pItemElem->m_nItemNum < 1 || dwCost == 0 )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) );	// 다른 사용자에게 팔렸습니다.
			return;
		}

		nBuy = atoi( m_pEdit->GetString() );
		if( (int)( (nBuy * dwCost) ) > g_pPlayer->GetGold() )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0009) ) );	// 돈이 부족합니다.	
			return;
		}
	}


	DWORD dwMAXCount = MAX_BUY_ITEMCOUNT;
//sun: 13, 화살포스터구입갯수9999개
	if( m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
		m_pItemElem->GetProp()->dwID == ITEM_INDEX( 26460, II_CHP_RED )
		)
		dwMAXCount = 9999;

	if( nBuy < 1 || nBuy > (int)( dwMAXCount ) )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0086) ) );	// 상점 거래 중 구입 / 판매 개수가 1 ~ 99 사이가 아닌 수를 입력	
		return;
	}
	if( nBuy > m_pItemElem->m_nItemNum )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0087) ) );	// 개인 상점 거래 중 구입 하려는 개수가 상점에 판매하는 개수 보다 많거나, 이미 품절	
		return;
	}

	CWndTabCtrl* pTabCtrl;
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			CWndShop* pWndShop = (CWndShop*)GetWndBase( APP_SHOP_ );
			pTabCtrl = (CWndTabCtrl*)pWndShop->GetDlgItem( WIDC_INVENTORY );
		}
		break;
	case CS_VER2:
		{
			CFLWndShop_Cart* pWndShop_Cart = (CFLWndShop_Cart*)GetWndBase( APP_SHOP_CART_ );
			pTabCtrl = (CWndTabCtrl*)pWndShop_Cart->GetDlgItem( WIDC_INVENTORY );
		}
		break;
	}

	CHAR cTab	= (CHAR)pTabCtrl->GetCurSel();
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
	if(m_nBuyType == VENDOR_TYPE_PENYA)
	{
		g_DPlay.SendBuyItem( cTab, m_pItemElem->m_dwObjId, nBuy, m_dwItemId );
	}
	else if(m_nBuyType == VENDOR_TYPE_CHIP)
	{
		g_DPlay.SendBuyChipItem( cTab, m_pItemElem->m_dwObjId, nBuy, m_dwItemId );
	}

	PLAYSND( SND_INF_TRADE );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndItemCtrlVendor::CWndItemCtrlVendor()
{
	m_bVisibleCount = FALSE;
}
CWndItemCtrlVendor::~CWndItemCtrlVendor()
{
}
BOOL CWndItemCtrlVendor::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM  )
		GetParentWnd()->OnChildNotify( WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut ); 
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndShop::CWndShop() 
{ 
	m_pMover			= NULL;
	m_pWndConfirmSell	= NULL;
	m_pWndWarning		= NULL;
	m_bSexSort			= FALSE;
	m_bLevelSort		= FALSE;
} 
CWndShop::~CWndShop() 
{ 
	SAFE_DELETE( m_pWndConfirmSell );
	SAFE_DELETE( m_pWndWarning );
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
} 
void CWndShop::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCHARACTER lpCharacter = m_pMover->GetCharacter();
	if( lpCharacter == NULL)
		return;

	CString string;

	CWndStatic* pCost = (CWndStatic*) GetDlgItem( WIDC_COST );

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_INVENTORY );

	WTCITEM item;
	pTabCtrl->GetItem( pTabCtrl->GetCurSel(), &item );
	CWndItemCtrl* pItemCtrl		= (CWndItemCtrl*)item.pWndBase;
	DWORD dwCost	= 0;
	int nIndex	= -1;
	if( pItemCtrl->GetSelectedCount() > 0 )
		nIndex	= pItemCtrl->GetSelectedItem( 0 );
	if( nIndex >= 0 )
	{
		FLItemBase* pItemBase	= pItemCtrl->GetItemFromArr(nIndex);
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
		if( pItemBase && m_pMover )
		{
			LPCHARACTER lpCharacter = m_pMover->GetCharacter();
			if(lpCharacter)
			{
				if(lpCharacter->m_nVenderType == VENDOR_TYPE_PENYA)
				{
					dwCost += pItemBase->GetCost();
					dwCost = static_cast< int >( static_cast< float >( dwCost ) * prj.m_fShopBuyRate );
				//sun: 12, 세금
					if(CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, (FLItemElem*)pItemBase ))
						dwCost += ( static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );
				}

				else if(lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
					dwCost += pItemBase->GetChipCost();
			}
		}
	}

	dwCost = (int)((float)dwCost * prj.m_fShopCost );

//sun: 수표 기능 페린 관련
	if( nIndex >= 0 )
	{
		FLItemBase* pItemBase	= pItemCtrl->GetItemFromArr( nIndex );
		if( pItemBase && pItemBase->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
			dwCost = PERIN_VALUE;
	}

	string.Format( _T( "%d" ), dwCost );

	pCost->SetTitle( string );
} 
void CWndShop::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
//sun: 11, 주머니
	if(GetWndBase( APP_BAG_EX )) 
		GetWndBase( APP_BAG_EX )->Destroy();

	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_INVENTORY );
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	LPCHARACTER lpCharacter = m_pMover->GetCharacter();

	if( lpCharacter )
	{
		for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		{
			if( lpCharacter->m_venderSlot[ i ].IsEmpty() == FALSE )
			{
				m_wndItemCtrl[ i ].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, i + 10 );
				m_wndItemCtrl[ i ].InitItem( m_pMover->m_ShopInventory[ i ], APP_SHOP_ );
				tabTabItem.pszText = lpCharacter->m_venderSlot[ i ].LockBuffer();
				lpCharacter->m_venderSlot[ i ].UnlockBuffer();
				tabTabItem.pWndBase = &m_wndItemCtrl[ i ];
				pTabCtrl->InsertItem( i, &tabTabItem );
			}
		}
		for( int i = pTabCtrl->GetSize(); i < 3; i++ )
		{
			tabTabItem.pszText = "";
			tabTabItem.pWndBase = NULL;
			pTabCtrl->InsertItem( i, &tabTabItem );
		}
	}
	
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
} 

BOOL CWndShop::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_SHOP_, 0, 0, pWndParent );
} 

BOOL CWndShop::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndShop::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndShop::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndShop::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndShop::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL	bWarning	= FALSE;

	switch(message)
	{
	case WIN_ITEMDROP:
		{
			LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
			CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

			BOOL bForbid = TRUE;
			if( lpShortcut->m_dwType == ITYPE_ITEM && lpShortcut->m_dwData != 0 )
			{
				if( nID == 10 || nID == 11 || nID == 12 || nID == 13) // item
				{
					BOOL bResult = TRUE;
					FLItemElem* pItemElem = ( FLItemElem* )lpShortcut->m_dwData;
					if( pItemElem == NULL )
					{
						bResult = FALSE;
					}

					if( pItemElem->IsQuest() )
					{
						bResult = FALSE;
					}

					if( bResult && pWndFrame->GetWndId() == APP_INVENTORY )
					{
						FLItemElem *pItemElem = (FLItemElem*)lpShortcut->m_dwData;
						if( pItemElem )
						{
							PT_ITEM_SPEC pProp = pItemElem->GetProp();
							if( pProp )
							{
								if( pItemElem->IsCharged() )
								{
									// 090527 김창섭 - 유료지역입장권 사용 후 사용기간이 남아있는 상태에서 NPC를 통해 상점에 팔 경우 경고창이 뜨지않는 현상 수정
									//if( !pItemElem->m_dwKeepTime )
									bWarning	= TRUE;
								}
							}
							else
							{
								FLERROR_LOG( PROGRAM_NAME, _T( "pProp==NULL %d" ), pItemElem->m_dwItemId );
							}
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "pItemElem==NULL %d" ), pItemElem->m_dwItemId );
						}
						if( FALSE == g_pPlayer->m_Inventory.IsEquip( ( (FLItemElem*)lpShortcut->m_dwData)->m_dwObjId ) )
						{
							if( bWarning )
							{
								SAFE_DELETE( m_pWndWarning );
								SAFE_DELETE( m_pWndConfirmSell );
								m_pWndWarning	= new CWndWarning;
								m_pWndWarning->m_pItemElem	= (FLItemElem*)lpShortcut->m_dwData;
								m_pWndWarning->m_pMover = m_pMover;
								m_pWndWarning->Initialize( this, APP_WARNING );
							}
							else
							{
								SAFE_DELETE( m_pWndWarning );
								SAFE_DELETE( m_pWndConfirmSell );
								m_pWndConfirmSell = new CWndConfirmSell;
								m_pWndConfirmSell->m_pItemElem = (FLItemElem*)lpShortcut->m_dwData;
								m_pWndConfirmSell->m_pMover = m_pMover;
								m_pWndConfirmSell->Initialize( this, APP_CONFIRM_SELL );
							}

							bForbid = FALSE;
						}
						else
						{
							g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPTRADE), NULL, prj.GetTextColor(TID_GAME_EQUIPTRADE) );
						}
					}
				}
			}
			SetForbid( bForbid );
		}
		break;
	}

	switch( nID )
	{
	case WIDC_CHECK_LEVEL:
		{
			m_bLevelSort  = !m_bLevelSort;
		}
		break;
	case WIDC_CHECK2:
		{
			m_bSexSort    = !m_bSexSort;
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndShop::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndConfirmSell )
		SAFE_DELETE( m_pWndConfirmSell );
}

void CWndShop::OnDestroy( void )
{
	g_pPlayer->m_vtInfo.SetOther( NULL );
	g_DPlay.SendCloseShopWnd();
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
	if( pWndInventory )
	{
		SAFE_DELETE( pWndInventory->m_pWndConfirmBuy );
	}
	SAFE_DELETE( m_pWndConfirmSell );
}

//------------------------------------------------------------------------------------------------------------------
//
//	_JIN_SHOP_CART
//
//------------------------------------------------------------------------------------------------------------------
CFLWndShop_Cart *CFLWndShop_Cart::m_pShop_Cart = NULL;

//------------------------------------------------------------------------------------------------------------------
CFLWndShop_Cart::CFLWndShop_Cart() 
{ 
	m_pMover			= NULL;
	m_pWndConfirmSell	= NULL;
	m_pWndCart			= NULL;
	m_pWndWarning		= NULL;
	m_bSexSort			= FALSE;
	m_bLevelSort		= FALSE;
	m_pFocusItem		= NULL;
	m_nCartCnt			= 0;
	m_ulTempCost		= 0;
	m_nSlotWidth		= 1;
	m_nCartIcon_PosX	= 0;
	m_nCartIcon_PosY	= 0;
	m_nDropItem			= -1;
	m_bNoCloseButton = TRUE;
	m_pShop_Cart = this;
} 
//------------------------------------------------------------------------------------------------------------------
CFLWndShop_Cart::~CFLWndShop_Cart() 
{ 
	SAFE_DELETE( m_pWndConfirmSell );
	SAFE_DELETE( m_pWndWarning );
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
	g_WndMng.CloseMessageBox();
} 

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCHARACTER lpCharacter = m_pMover->GetCharacter();
	if( lpCharacter == NULL)
		return;

	PFONTCOLOR_WNDSHOP_CART pFontColorWndShop_Cart = g_WndFontColorManager->GetWndShop_Cart();

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_INVENTORY );

	WTCITEM item;
	pTabCtrl->GetItem( pTabCtrl->GetCurSel(), &item );

	m_pItemCtrl		= (CWndItemCtrl*)item.pWndBase;
	DWORD dwCost	= 0;
	int nIndex	= -1;
	if( m_pItemCtrl->GetSelectedCount() > 0 )
		nIndex	= m_pItemCtrl->GetSelectedItem( 0 );
	if( nIndex >= 0 )
	{
		FLItemBase* pItemBase	= m_pItemCtrl->GetItemFromArr( nIndex );
		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
		if( pItemBase && m_pMover )

		{
			LPCHARACTER lpCharacter = m_pMover->GetCharacter();
			if(lpCharacter)
			{
				if(lpCharacter->m_nVenderType == 0)
				{
					dwCost += pItemBase->GetCost();
					dwCost = static_cast< int >( static_cast< float >( dwCost ) * prj.m_fShopBuyRate );
					//sun: 12, 세금
					if(CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, (FLItemElem*)pItemBase ))
						dwCost += ( static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );
				}

				else if(lpCharacter->m_nVenderType == 1)
					dwCost += pItemBase->GetChipCost();
			}
		}
	}

	dwCost = (int)((float)dwCost * prj.m_fShopCost );

	//sun: 수표 기능 페린 관련
	if( nIndex >= 0 )
	{
		FLItemBase* pItemBase	= m_pItemCtrl->GetItemFromArr( nIndex );
		if( pItemBase && pItemBase->m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
			dwCost = PERIN_VALUE;
	}

	//소지한 금액
	CString strPlayerGold;
	CString strBuyGold;
	ULONGLONG ulUserCost = 0;
	if( m_pMover->GetCharacter()->m_nVenderType == VENDOR_TYPE_CHIP )
	{
		DWORD dwItemId = ITEM_INDEX( 26460, II_CHP_RED );
		ULONGLONG ulItemCnt = g_pPlayer->m_Inventory.GetItemNumByItemId(dwItemId);
		ulUserCost = ulItemCnt;
		strPlayerGold.Format("%I64u",ulUserCost);
		m_pPlayerGold->SetTitle(strPlayerGold);
	}
	else
	{
		ulUserCost = g_pPlayer->GetGold();
		strPlayerGold.Format("%I64u",ulUserCost);
		m_pPlayerGold->SetTitle(strPlayerGold);
	}
	strBuyGold = m_pCost->GetTitle();
	ULONGLONG ulBuyCost = _ttoi(strBuyGold);
	if( ulUserCost < ulBuyCost )
	{
		m_pCost->SetTileColor( pFontColorWndShop_Cart->m_stlessUserMoney.GetFontColor() );
	}
	else
	{
		m_pCost->SetTileColor( pFontColorWndShop_Cart->m_stEnoughUserMoney.GetFontColor() );
	}


	//장바구니
	//added by gmpbigsun( 20120315 ) : 장바구니에 물건이 있을때만 사기버튼 활성화.
	CWndButton* pWndBuy = ( CWndButton* )GetDlgItem( WIDC_BT_BUY );
	if( pWndBuy )
	{
		if( m_vecCart.empty() )
			pWndBuy->EnableWindow( FALSE );
		else 
			pWndBuy->EnableWindow( TRUE );
	}
	
 	CPoint Mp = GetMousePoint();
	CString strItemCnt;
	CString	strSlotCnt;
	CWndStatic* pSlotCnt = (CWndStatic*)GetDlgItem(WIDC_STATIC_CARTCNT);
	if(m_vecCart.size() != 0)
	{
		for(unsigned int nItem = 0 ; nItem <= m_vecCart.size()-1; nItem++)
		{
			FLItemElem* pItemElem = m_vecCart[nItem]->m_pItemElem;
			
			if( pItemElem == NULL )	
				continue;

			int nX = nItem % 6;
			int nY = nItem / 6;

			if(pItemElem->m_pTexture)
			{
				int nYValue;
				if(nY == 1)
				{
					nYValue = (m_nCartIcon_PosY*nY)+30;
				}
				else 
				{
					nYValue = (m_nCartIcon_PosY*(nY+1));
				}
				m_pItemCtrl->RanderIcon( p2DRender, pItemElem, (m_nCartIcon_PosX*(nX+1))-20, nYValue,APP_SHOP_CART_);				
				//갯수를 찍어주자!!!
				PT_T_SHOPCART pCart = m_vecCart[nItem];
				int nItemCnt = pCart->m_nItemCnt;
				int nTextX = 0;
				if(nItemCnt != 0)
				{
					strItemCnt.Format("%d", nItemCnt);
					if(strlen(strItemCnt.GetString()) >= 3)
					{
						nTextX = (m_nCartIcon_PosX*(nX+1))-10;
					}
					else
					{
						nTextX = (m_nCartIcon_PosX*(nX+1));
					}
				}
				p2DRender->TextOut(nTextX, nYValue+20, strItemCnt, pFontColorWndShop_Cart->m_stShoppingBagItemCountShadow.GetFontColor());			
				p2DRender->TextOut(nTextX-1, nYValue+19, strItemCnt, pFontColorWndShop_Cart->m_stShoppingBagItemCount.GetFontColor());			
				//strTemp.Format(" x[%d] y[%d], 실제 Y값 [%d] Height[%d] %s",nX,nY,(m_nCartIcon_PosY*(nY+1)),nYValue);
			}
		}
		
		strSlotCnt.Format("( %d / %d )",m_vecCart.size(),MAX_CART_SLOT);

		//TestCode---------------------------------------
//         strTemp2.Format("X[%d] Y[%d]",Mp.x,Mp.y);
// 		p2DRender->TextOut(100,10,strTemp2,0xff000000);
// 		p2DRender->TextOut(75,184,strTest,0xff0000ff);
		//-----------------------------------------------
	}		
	else
	{
		strSlotCnt.Format("( %d / %d )",0,MAX_CART_SLOT);
	}
	pSlotCnt->SetTileColor( pFontColorWndShop_Cart->m_stTotalShoppingBagItemCount.GetFontColor() );
	pSlotCnt->SetTitle( strSlotCnt );
} 


//---------------------------------------------------------------------------------------------
// 윈도우의 생성 될 위치를 계산한다.
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CFLWndShop_Cart::CalculationWindowPosition()
{
	CWndInventory*		pWndInventory( (CWndInventory*)GetWndBase( APP_INVENTORY ) );
	if( NULL == pWndInventory )
		return ;

	CRect	rectInventory( pWndInventory->GetWindowRect( TRUE ) );
	CPoint	ptInventory( rectInventory.TopLeft() );
	CRect	rtCart( GetWindowRect( TRUE ) );
	CPoint	ptMove( 0, 0 );

	// 오른쪽 부터 검사하자
	if( ptInventory.x + rectInventory.Width() + rtCart.Width() < g_Option.m_nResWidth )
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	else
		ptMove = ptInventory - CPoint( rtCart.Width(), 0 );

	Move( ptMove );
}


//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnInitialUpdate() 
 { 
 	CWndNeuz::OnInitialUpdate(); 
	//sun: 11, 주머니
	if(GetWndBase( APP_BAG_EX )) 
		GetWndBase( APP_BAG_EX )->Destroy();


	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase()||g_WndMng.m_pWndBarunaNPC )
	{
		Destroy();
		return;
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_INVENTORY );
	m_pWndShopList = pTabCtrl;
	m_pWndStatic_Cart = ( CWndStatic* )GetDlgItem(WIDC_STATIC3);
	
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	//상점 아이템 세팅
	LPCHARACTER lpCharacter = m_pMover->GetCharacter();
	if( lpCharacter )
	{
		for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		{
			if( lpCharacter->m_venderSlot[ i ].IsEmpty() == FALSE )
			{
				m_wndItemCtrl[ i ].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, i + 10 );
				m_wndItemCtrl[ i ].InitItem( m_pMover->m_ShopInventory[ i ], APP_SHOP_CART_ );
				tabTabItem.pszText = lpCharacter->m_venderSlot[ i ].LockBuffer();
				tabTabItem.pWndBase = &m_wndItemCtrl[ i ];
				lpCharacter->m_venderSlot[ i ].UnlockBuffer();
				pTabCtrl->InsertItem( i, &tabTabItem );		
			}
		}
		for( int i = pTabCtrl->GetSize(); i < 3; i++ )
		{
			tabTabItem.pszText = "";
			tabTabItem.pWndBase = NULL;
			pTabCtrl->InsertItem( i, &tabTabItem );
		}		
	}

	CalculationWindowPosition();

	m_pCost = (CWndStatic*) GetDlgItem( WIDC_ST_ESMOMEY );
	m_pCost->AddWndStyle( WSS_MONEY );

	m_pPlayerGold = (CWndStatic*)GetDlgItem( WIDC_ST_USMOMEY );
	m_pPlayerGold->AddWndStyle( WSS_MONEY );

	m_lpWndCtrl = GetWndCtrl(WIDC_STATIC3);
	m_rectHittest = m_lpWndCtrl->rect;
	CWndStatic* pStUserGold = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	if( m_pMover->GetCharacter()->m_nVenderType == VENDOR_TYPE_CHIP )
	{
		pStUserGold->SetTitle( prj.GetText( TID_MMI_REDCHIP ) );
	}
	else
	{
		pStUserGold->SetTitle( prj.GetText( TID_MMI_POSSESSIONMONEY ) );
	}
} 

//------------------------------------------------------------------------------------------------------------------
BOOL CFLWndShop_Cart::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_SHOP_CART_, 0, 0, pWndParent );
} 

//------------------------------------------------------------------------------------------------------------------
BOOL CFLWndShop_Cart::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	switch(nID)
	{
	case IDOK:
		Destroy();
		break;
	case IDCANCEL:
		break;
	}

	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 

} 

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnMouseMove(UINT nFlags, CPoint point)
{
	
}

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	if( !g_pPlayer )
		return;

	CPoint pt( 3, 3 );
	CRect rect;

	int nItem = HitTest( point );

	if( nItem == -1 ) return;

	CWndBase* pWndBase = this->GetParentWnd();

	FLItemElem* pItemElem = m_vecCart[nItem]->m_pItemElem;
	if( pItemElem )
	{
		if( pItemElem->m_bControlForbidden == TRUE )
		{
			return;
		}
		m_pFocusItem = pItemElem;

		CWndBase* pBase = NULL;

		if( m_pWndShopList->GetWndRect().PtInRect( point ) )
			pBase = m_pWndShopList;
		else if( m_pWndStatic_Cart->GetWndRect().PtInRect( point ) )
			pBase = m_pWndStatic_Cart;

		m_GlobalShortcut.m_pFromWnd   = pBase;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex    = m_nCurSelect;
		m_GlobalShortcut.m_dwType     = 0;
		m_GlobalShortcut.m_dwId       = m_pFocusItem->m_dwObjId;
		m_GlobalShortcut.m_pTexture   = m_pFocusItem->GetTexture();
		m_GlobalShortcut.m_dwData     = (DWORD) m_pFocusItem;
	}
	m_nDropItem = nItem;
} 

//------------------------------------------------------------------------------------------------------------------
int CFLWndShop_Cart::HitTest(CPoint point)
{
	int nDstId = -1;
	CPoint pt( 3, 3 );
	CRect rect;


	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if( pWndBase )
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if( pWndBaseTemp )
			nParent = pWndBaseTemp->GetWndId();
		else
			return nDstId;
	}
	else
	{
		return nDstId;
	}

	for( unsigned int i = 0; i < m_vecCart.size(); i++ ) 
	{
		int nX = i % 6;
		int nY = i / 6;
		int nYValue;
		if(nY == 1)
		{
			nYValue = (m_nCartIcon_PosY*nY)+30;
		}
		else 
		{
			nYValue = (m_nCartIcon_PosY*(nY+1));
		}
		rect.SetRect((m_nCartIcon_PosX*(nX+1))-20, nYValue, (m_nCartIcon_PosX*(nX+1))-20+32, nYValue+30);
		if( rect.PtInRect( point ) )
		{
			nDstId = i;
			break;
		}
	}
	return nDstId;
}

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( !g_pPlayer )
		return;

	int nItem = HitTest( point );

	if( nItem == -1 )	return;

	FLItemElem* pItemElem = m_vecCart[nItem]->m_pItemElem;

	if( pItemElem == NULL )
	{
		return;
	}
	if( pItemElem )
	{
		if( pItemElem->m_bControlForbidden == TRUE )
		{
			return;
		}
		m_pFocusItem = pItemElem;

		CWndBase* pBase = NULL;

		if( m_pWndShopList->GetWndRect().PtInRect( point ) )
			pBase = m_pWndShopList;
		else if( m_pWndStatic_Cart->GetWndRect().PtInRect( point ) )
			pBase = m_pWndStatic_Cart;

		m_GlobalShortcut.m_pFromWnd   = pBase;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex    = m_nCurSelect;
		m_GlobalShortcut.m_dwType     = 0;
		m_GlobalShortcut.m_dwId       = m_pFocusItem->m_dwObjId;
		m_GlobalShortcut.m_pTexture   = m_pFocusItem->GetTexture();
		m_GlobalShortcut.m_dwData     = (DWORD) m_pFocusItem;
	}
	m_nDropItem = nItem;
	SetDropItem(m_nDropItem);
}

BOOL CFLWndShop_Cart::IsSelectedItem( int iItem )
{
	for( int i = 0; i < m_dwSelectAry.GetSize(); i++ )
		if( m_dwSelectAry.GetAt( i ) == iItem )
			return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::Item_DBLCLK(LRESULT* pLResult)
{
	LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;

	FLItemElem* pItemElem = (FLItemElem*)pLResult;
	PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
	if(m_vecCart.size() == MAX_CART_SLOT)
	{
		if(!IsFull(pItemElem, pItemProp, pItemElem->m_nItemNum )) 
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_1) ) );	
			return;
		}
	}
	int nCartCnt = GetCartCnt();
	if(nCartCnt <= MAX_CART_SLOT)
	{
		FLItemElem* pItemElem = (FLItemElem*)pLResult;
		if(pItemElem)
		{
			CString string;
			string = m_pCost->GetTitle();
			ULONGLONG ulTempCost = _ttoi(string);

			//미리 최소수량 금액을 계산해서 구입금액이 넘지 않도록한다!
			ULONGLONG ulTestCost = ulTempCost + GetCostofOneItem( pItemElem );
			if( ulTestCost > INT_MAX || ulTempCost < 0 )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_6) ) );	
				return;
			}
			else
			{
				if(pItemElem->GetProp()->dwPackMax > 1)			//수량확인 단품인지 묶음인지 
				{	
					if(m_pWndCart != NULL) return;
					m_pWndCart	= new FLWndCart;
					m_pWndCart->m_pItemElem	= pItemElem;
					m_pWndCart->m_pMover = m_pMover;
					m_pWndCart->m_pItemCtrl = m_pItemCtrl;
					m_pWndCart->Initialize( this, APP_CONFIRM_ITEM );		
				}
				else//단품이면 
				{
					std::vector<PT_T_SHOPCART>	vecTem;
					ULONGLONG ulCost = GetCostofOneItem( pItemElem );
					if( m_pMover->GetCharacter()->m_nVenderType == VENDOR_TYPE_CHIP )
					{
						ulCost = pItemElem->GetChipCost();
					}
					if(ulTempCost != 0)
					{	
						ulTempCost += ulCost;
						string.Format( _T( "%I64u" ), ulTempCost );
					}
					else
					{
						string.Format( _T( "%I64u" ), ulCost );
					}

					m_pCost->SetTitle( string );
					PT_T_SHOPCART ptCart = new _T_SHOPCART;
					ptCart->m_nCartCnt = (nCartCnt+1);
					ptCart->m_pItemElem = pItemElem;
					ptCart->m_nItemCnt = 1;
					ptCart->m_nShoptab = m_pWndShopList->GetCurSel();
					vecTem = GetVecCart();
					vecTem.push_back(ptCart);
					SetVecCart(vecTem);
					SetCartCnt(m_vecCart.size());
				}
			}
			//strString.Format("Itmename[%s(%d)]\n  cartCount[%d]\n ",pShortcut->m_szString,pItemElem->GetProp()->dwNum,m_nCartCnt);
		}
	}
	else
	{
		//구입수량이 많아 장바구니의 여유 공간이 부족합니다.
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_2) ) );	
	}
	//카트 기본위치 정해줌 
	m_nCart_Width = m_rectHittest.Width()/8;
	m_nCart_nHeight = m_rectHittest.top;

	//IconRender XPosition
	m_nCartIcon_PosX = m_nCart_Width+10;
	//IconRender YPosition
	m_nCartIcon_PosY = m_nCart_nHeight;
}
//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::Item_DROP(UINT nID, LRESULT* pLResult, BOOL bWarning)
{
	LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
	CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

	if(pWndFrame == NULL) return ;

	BOOL bForbid = TRUE;
	if( lpShortcut->m_dwType == ITYPE_ITEM && lpShortcut->m_dwData != 0 )
	{
		if( nID == 10 || nID == 11 || nID == 12 || nID == 13) // item
		{
			BOOL bResult = TRUE;
			FLItemElem* pItemElem = ( FLItemElem* )lpShortcut->m_dwData;
			if( pItemElem == NULL )
			{
				bResult = FALSE;
			}

			if( pItemElem->IsQuest() )
			{
				bResult = FALSE;
			}

			if( bResult && pWndFrame->GetWndId() == APP_INVENTORY )
			{
				FLItemElem *pItemElem = (FLItemElem*)lpShortcut->m_dwData;
				if( pItemElem )
				{
					if( pItemElem->GetProp() )
					{
						if( pItemElem->IsCharged() )
						{
							// 090527 김창섭 - 유료지역입장권 사용 후 사용기간이 남아있는 상태에서 NPC를 통해 상점에 팔 경우 경고창이 뜨지않는 현상 수정
							//if( !pItemElem->m_dwKeepTime )
							bWarning	= TRUE;
						}
					} else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "pProp==NULL %d" ), pItemElem->m_dwItemId );
					}
				} else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "pItemElem==NULL %d" ), pItemElem->m_dwItemId );
				}
	
				if( FALSE == g_pPlayer->m_Inventory.IsEquip( ( (FLItemElem*)lpShortcut->m_dwData)->m_dwObjId ) )
				{
					if( bWarning )
					{
						SAFE_DELETE( m_pWndWarning );
						SAFE_DELETE( m_pWndConfirmSell );
						m_pWndWarning	= new CWndWarning;
						m_pWndWarning->m_pItemElem	= (FLItemElem*)lpShortcut->m_dwData;
						m_pWndWarning->m_pMover = m_pMover;
						m_pWndWarning->Initialize( this, APP_WARNING );
					}
					else
					{
						SAFE_DELETE( m_pWndWarning );
						SAFE_DELETE( m_pWndConfirmSell );
						m_pWndConfirmSell = new CWndConfirmSell;
						m_pWndConfirmSell->m_pItemElem = (FLItemElem*)lpShortcut->m_dwData;
						m_pWndConfirmSell->m_pMover = m_pMover;
						m_pWndConfirmSell->Initialize( this, APP_CONFIRM_SELL );
					}

					bForbid = FALSE;
				}
				else
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPTRADE), NULL, prj.GetTextColor(TID_GAME_EQUIPTRADE) );
				}					
			}
		}
	}
	SetForbid( bForbid );
}
//------------------------------------------------------------------------------------------------------------------
void	CFLWndShop_Cart::Item_CLICKED()
{
}

//------------------------------------------------------------------------------------------------------------------
BOOL CFLWndShop_Cart::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL	bWarning	= FALSE;
	switch (message)
    {
	case WIN_DBLCLK:
		Item_DBLCLK(pLResult);
		break;
	case WIN_ITEMDROP:
		Item_DROP(nID,pLResult,bWarning);
		break;
	case WNM_CLICKED:
		Item_CLICKED();
		break;
	}
	switch( nID )
	{
	case WIDC_CHECK_LEVEL:
		{
			m_bLevelSort  = !m_bLevelSort;
		}
		break;

	case WIDC_CHECK_GENDER:
		{
			m_bSexSort    = !m_bSexSort;
		}
		break;

	case WIDC_BT_BUY:
		if( TRUE == OnOK() )
			EnableWindow( FALSE );
		break;

	case WIDC_BT_CANCEL:
		if(m_vecCart.empty())
		{
			Destroy();
		}
		else
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_NOTICE_CLOSE) ) ,MB_OKCANCEL, this);	
		}
        break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//------------------------------------------------------------------------------------------------------------------
BOOL		CFLWndShop_Cart::OnOK()
{
	if( m_vecCart.empty() == true )
	{
		return FALSE;
	}
	
	CString  strUserGold;
	int 	 nUserGold;

	CString  strCartGold;
	ULONGLONG 	 ulCartGold;

	strUserGold = m_pPlayerGold->GetTitle();
	nUserGold = _ttoi( strUserGold );

	strCartGold = m_pCost->GetTitle();
	ulCartGold = _ttoi( strCartGold );

	//돈없어서 떨거지
	if( ulCartGold > nUserGold )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_3) ) );	
		return FALSE;
	}
	T_PACKET_SHOPPING_BASKET ptShopCart = { 0 };
	//mem_set( &ptShopCart, 0, sizeof( ptShopCart ) );

	ptShopCart.nVendorKind = VENDOR_KIND_NPC;
	ptShopCart.nVendorNPCType = m_pMover->GetCharacter()->m_nVenderType;
	
	for(unsigned int nCnt = 0; nCnt < m_vecCart.size(); nCnt++)
	{
		ptShopCart.tBuyItemInfo[nCnt].nShopTabId = m_vecCart[nCnt]->m_nShoptab;
		ptShopCart.tBuyItemInfo[nCnt].dwObjId = m_vecCart[nCnt]->m_pItemElem->m_dwObjId;
		ptShopCart.tBuyItemInfo[nCnt].dwItemId = m_vecCart[nCnt]->m_pItemElem->m_dwItemId;
		ptShopCart.tBuyItemInfo[nCnt].nItemNum = m_vecCart[nCnt]->m_nItemCnt;
	}
 	g_DPlay.SendReqBuyItem( &ptShopCart );

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndConfirmSell )
		SAFE_DELETE( m_pWndConfirmSell );
}

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::SetDropItem(int nDropItemIdx)
{
	if(nDropItemIdx == -1) 
	{
		SetForbid(TRUE);
		return;
	}
	CString string;
	string = m_pCost->GetTitle();
	int nItemCnt = m_vecCart[nDropItemIdx]->m_nItemCnt;
	ULONGLONG ulCost = 0;
	ULONGLONG ulTempCost = _ttoi(string);
	if(m_pMover->GetCharacter()->m_nVenderType == VENDOR_TYPE_CHIP)
	{
		ulCost = m_vecCart[nDropItemIdx]->m_pItemElem->GetChipCost();
	}
	else
	{
		ulCost = GetCostofOneItem( m_vecCart[nDropItemIdx]->m_pItemElem );
	}

	if(ulTempCost != 0)
	{	
		ulTempCost -= ulCost * nItemCnt;
		string.Format( _T( "%I64u" ), ulTempCost );
	}
	else
	{
		string.Format( _T( "%I64u" ), ulCost );
	}

	m_pCost->SetTitle( string );
	m_vecCart.erase(m_vecCart.begin()+nDropItemIdx);
	SetCartCnt(m_vecCart.size());
//	strTest.Format("index[%d],Count[%d]cost[%d]",nDropItemIdx,nItemCnt,nCost*nItemCnt);
}

//------------------------------------------------------------------------------------------------------------------
void CFLWndShop_Cart::OnDestroy( void )
{
	g_pPlayer->m_vtInfo.SetOther( NULL );
	g_DPlay.SendCloseShopWnd();
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
	if( pWndInventory )
	{
		SAFE_DELETE( pWndInventory->m_pWndConfirmBuy );
	}
	SAFE_DELETE( m_pWndConfirmSell );
	SAFE_DELETE( m_pWndCart );
}

BOOL CFLWndShop_Cart::IsFull(FLItemElem* pElem, PT_ITEM_SPEC pItemProp, int nNum)
{
	std::vector<PT_T_SHOPCART>::iterator itr; 
	for(itr = m_vecCart.begin(); itr != m_vecCart.end(); itr++)
	{
		if((*itr)->m_pItemElem == pElem)
		{			
			if((*itr)->m_nItemCnt < nNum)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}



//------------------------------------------------------------------------------------------------------------------
BOOL CFLWndShop_Cart::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{	
	if( APP_SHOP_CART_ != pShortcut->m_pFromWnd->GetFocusWnd()->GetWndId())			
	{
		return FALSE;
	}
	if( APP_INVENTORY == pShortcut->m_pFromWnd->GetFocusWnd()->GetWndId())			
	{
		return FALSE;
	}
    // 드롭아이템이 장바구니안에서 움직였으면
	if(pShortcut->m_pFromWnd->GetWndId() == m_pWndStatic_Cart->GetWndId())
	{
	}
	else
	{
		FLItemElem* pItemElem = (FLItemElem*)pShortcut->m_dwData;
		PT_ITEM_SPEC pItemProp		= pItemElem->GetProp();
		if(m_vecCart.size() == MAX_CART_SLOT)
		{
			if(!IsFull(pItemElem, pItemProp, pItemElem->m_nItemNum )) 
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_1) ) );	
				return FALSE;
			}
		}
		CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
		
		if( m_lpWndCtrl->rect.PtInRect( point ) )
		{
			int nCartCnt = GetCartCnt();
			if(nCartCnt <= MAX_CART_SLOT)
			{
				if(pItemElem)
				{
					CString string;
					string = m_pCost->GetTitle();
					ULONGLONG ulTempCost = _ttoi(string);

					//미리 최소수량 금액을 계산해서 구입금액이 넘지 않도록한다!
					ULONGLONG ulTestCost = ulTempCost + GetCostofOneItem( pItemElem );
					if( ulTestCost > INT_MAX || ulTempCost < 0 )
					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_6) ) );	
						return FALSE;
					}
					else
					{
						if(pItemElem->GetProp()->dwPackMax > 1)			//수량확인 단품인지 묶음인지 
						{	
							if(m_pWndCart != NULL) return FALSE;
							m_pWndCart	= new FLWndCart;
							m_pWndCart->m_Shortcut.m_dwId = pShortcut->m_dwId;
							m_pWndCart->m_Shortcut.m_dwData = pShortcut->m_dwData;
							m_pWndCart->m_pItemElem	= (FLItemElem*)pShortcut->m_dwData;
							m_pWndCart->m_pMover = m_pMover;
							m_pWndCart->m_pItemCtrl = m_pItemCtrl;
							m_pWndCart->Initialize( this, APP_CONFIRM_ITEM );		
						}
						else//단품이면 
						{
							std::vector<PT_T_SHOPCART> vecTemp;
							CString string;
							string = m_pCost->GetTitle();
							
							ULONGLONG ulCost = GetCostofOneItem( pItemElem );
							ULONGLONG ulTempCost = _ttoi(string);
							
							if(ulTempCost != 0)
							{	
								ulTempCost += ulCost;
								string.Format( _T( "%I64u" ), ulTempCost );
							}
							else
							{
								string.Format( _T( "%I64u" ), ulCost );
							}

							m_pCost->SetTitle( string );
							PT_T_SHOPCART ptCart = new _T_SHOPCART;
							ptCart->m_nCartCnt = (nCartCnt+1);
							ptCart->m_pItemElem = pItemElem;
							ptCart->m_nItemCnt = 1;
							ptCart->m_nShoptab = m_pWndShopList->GetCurSel();
							vecTemp = GetVecCart();
							vecTemp.push_back(ptCart);
							SetVecCart(vecTemp);

							SetCartCnt(nCartCnt+1);
						}
					}
					//strString.Format("Itmename[%s(%d)]\n  cartCount[%d]\n ",pShortcut->m_szString,pItemElem->GetProp()->dwNum,m_nCartCnt);
				}
			}
			else
			{
				//구입수량이 많아 장바구니의 여유 공간이 부족합니다.
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_2) ) );	
			}
		}
		else 
		{
			SetForbid(TRUE);
		}

		//카트 기본위치 정해줌 
		m_nCart_Width = m_rectHittest.Width()/8;
		m_nCart_nHeight = m_rectHittest.top;

		//IconRender XPosition
		m_nCartIcon_PosX = m_nCart_Width+10;
		//IconRender YPosition
		m_nCartIcon_PosY = m_nCart_nHeight;
	}
	return TRUE;
}

void CFLWndShop_Cart::OnMouseWndSurface( CPoint point )
{
	//장바구니 아이템 툴팁을 위해서 

	for(unsigned int n = 0 ; n < m_vecCart.size(); n++)
	{
		FLItemElem* pItemElem = m_vecCart[n]->m_pItemElem;

		int nX = n % 6;
		int nY = n / 6;

		if( pItemElem == NULL ) continue;
		int nYValue;
		if(nY == 1)
		{
			nYValue = (m_nCartIcon_PosY*nY)+30;
		}
		else 
		{
			nYValue = (m_nCartIcon_PosY*(nY+1));
		}
		CRect rectHittest1 = CRect( (m_nCartIcon_PosX*(nX+1))-20, nYValue, (m_nCartIcon_PosX*(nX+1))-20+32, nYValue+30);

		//ToolTip
		if(rectHittest1.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest1 );
			g_WndMng.PutToolTip_Item( pItemElem, point2, &rectHittest1, APP_SHOP_CART_ );		
		}
	}
}
void CFLWndShop_Cart::ReceivedPacket(PT_PACKET_SHOPPING_BASKET  ptShop)
{
	if( true == ptShop->bResult)
	{
		m_vecCart.clear();
		SetCartCnt(m_vecCart.size());
		m_pCost->SetTitle( "0" );
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_NOTICE_BUY) ) );	

		EnableWindow( TRUE );
	}
	else
	{
		m_vecCart.clear();
		SetCartCnt(m_vecCart.size());
		m_pCost->SetTitle( "0" );
		EnableWindow( TRUE );
	}
}


//---------------------------------------------------------------------------------------------
// 아이템 하냐의 가격 구하기
// param	: void
// return	: 가격 
//---------------------------------------------------------------------------------------------
UINT CFLWndShop_Cart::GetCostofOneItem( FLItemElem* pItemElem )
{
	FLERROR_ASSERT_LOG_RETURN( g_pPlayer, NULL, _T("[윈도우 상점 (장바구니)] 전역 플레이어 포인터가 유효하지 않음") );
	FLERROR_ASSERT_LOG_RETURN( pItemElem, NULL, _T("[윈도우 상점 (장바구니)] 아이템 포인터가 유효하지 않음") );

	int nCost( pItemElem->GetCost() );

	if( CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, pItemElem ) )
		nCost += ( static_cast<int>(nCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );

	return nCost;
}


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndBeautyShop::CWndBeautyShop() 
{ 
//	SetPutRegInfo( FALSE );
	m_pWndConfirmSell = NULL;
	m_pModel          = NULL;
	m_dwHairMesh      = 1;
	memset( m_ColorRect, 0, sizeof(m_ColorRect) );
	memset( m_fColor, 0, sizeof(m_fColor) );
	m_bLButtonClick   = FALSE;
	m_nHairCost       = 0;

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	m_pApplyModel = NULL;
	m_nHairColorCost  = 0;

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
	//m_nHairColorCostR  = 0;
	//m_nHairColorCostG  = 0;
	//m_nHairColorCostB  = 0;
	
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	for(int i=0; i<4; i++)
	{
		m_nHairNum[i] = 0;
	}
	m_pHairModel = NULL;
	m_dwSelectHairMesh = 1;
	m_ChoiceBar = -1;
	m_pWndBeautyShopConfirm = NULL;
	
	m_bUseCoupon = FALSE;
	m_pWndUseCouponConfirm = NULL;
} 

CWndBeautyShop::~CWndBeautyShop() 
{ 
//	m_Texture.DeleteDeviceObjects();
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pWndConfirmSell );

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pHairModel);
	SAFE_DELETE(m_pWndBeautyShopConfirm);

	SAFE_DELETE(m_pWndUseCouponConfirm);
} 

void CWndBeautyShop::UseHairCoupon(BOOL isUse)
{
	m_bUseCoupon = isUse;
	if(m_bUseCoupon)
	{
		CString title = GetTitle();
		CString addText;
		addText.Format("  %s", prj.GetText( TID_GAME_NOWUSING_COUPON ));
		title = title + addText;
		SetTitle(title);	
	}
}

HRESULT CWndBeautyShop::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndBeautyShop::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
HRESULT CWndBeautyShop::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	return S_OK;
}

void CWndBeautyShop::OnDraw( C2DRender* p2DRender ) 
{ 
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	if( g_pPlayer == NULL || m_pModel == NULL || m_pApplyModel == NULL )
//	if( g_pPlayer == NULL || m_pModel == NULL )		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가. ( < 8 )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
#endif //__YENV
	
	// 왼쪽 원본 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM5 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;//2;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;//5;
		viewport.Width  = lpFace->rect.Width();//p2DRender->m_clipRect.Width();
		viewport.Height = lpFace->rect.Height();// - 10;//p2DRender->m_clipRect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
/*		
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/		
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixScaling(&matScale, 4.5f, 4.5f, 4.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.0f,-5.6f,0.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-5.2f,0.0f);

		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
		if( g_pPlayer )
			g_pPlayer->OverCoatItemRenderCheck(m_pModel);
		int nVisibleParts = PARTS_HAT;
		int nInVisibleParts = PARTS_CAP;
		if(!g_pPlayer->IsOptionRenderCostume(0) )
		{
			nVisibleParts = PARTS_CAP;
			nInVisibleParts = 0;
		}	
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		FLItemElem* pItemElem	= g_pPlayer->GetEquipItem( nInVisibleParts );
		O3D_ELEMENT* pElement = NULL;
		pElement = m_pModel->GetParts(PARTS_HAIR);
		if( pElement )
			pElement->m_nEffect &= ~XE_HIDE;
		if( pItemElem )
		{
			
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = m_pModel->GetParts(pItemProp->dwBasePartsIgnore);
				
				//if( pElement )
				//	pElement->m_nEffect |= XE_HIDE;
			}

			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			FLItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( nVisibleParts );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( FLItemElem::expired ) )
				{					
					PT_ITEM_SPEC pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE);
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
										
		}
		else
		{
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			FLItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( nVisibleParts );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( FLItemElem::expired ) )
				{
					PT_ITEM_SPEC pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE);
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}

		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}
	// 오른쪽 색입힌 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;//2;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;//5;
		viewport.Width  = lpFace->rect.Width();//p2DRender->m_clipRect.Width();
		viewport.Height = lpFace->rect.Height();// - 10;//p2DRender->m_clipRect.Height();
		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
/*
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/		
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixRotationY(&matRot,g_tmCurrent/1000.0f);
		D3DXMatrixScaling(&matScale, 4.5f, 4.5f, 4.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.0f,-5.6f,0.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-5.2f,0.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		m_pApplyModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = m_fColor[0];
		m_pApplyModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = m_fColor[1];
		m_pApplyModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = m_fColor[2];

		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
		//m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = m_fColor[0];
		//m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = m_fColor[1];
		//m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = m_fColor[2];

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		if( g_pPlayer )
			g_pPlayer->OverCoatItemRenderCheck(m_pApplyModel);
		int nVisibleParts = PARTS_HAT;
		int nInVisibleParts = PARTS_CAP;
		if(g_pPlayer && !g_pPlayer->IsOptionRenderCostume(0) )
		{
			nVisibleParts = PARTS_CAP;
			nInVisibleParts = 0;
		}
		::SetTransformView( matView );
		::SetTransformProj( matProj );

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		FLItemElem* pItemElem	= g_pPlayer->GetEquipItem( nInVisibleParts );
		O3D_ELEMENT* pElement = NULL;
		pElement = m_pModel->GetParts(PARTS_HAIR);
		if( pElement )
			pElement->m_nEffect &= ~XE_HIDE;
		if( pItemElem )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
					m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
				
				m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
			}
		}
		else
		{
			pItemElem	= g_pPlayer->GetEquipItem( nVisibleParts );
			if( pItemElem )
			{
				if( !pItemElem->IsFlag( FLItemElem::expired ) )
				{		
					PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );

						m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}				
		}

		
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		m_pApplyModel->Render( p2DRender->m_pd3dDevice, &matWorld );
		
		m_pApplyModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = 1.0f;
		m_pApplyModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = 1.0f;
		m_pApplyModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = 1.0f;

		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
		//m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
		//m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = 1.0f;
		//m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = 1.0f;
		//m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = 1.0f;

	}

	DrawHairKind(p2DRender, matView);	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.

	viewport.X      = p2DRender->m_ptOrigin.x;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	
	for( int i=0; i<3; i++ )
	{
		CPoint pt = CPoint( m_ColorScrollBar[i].x - ( m_Texture.m_size.cx / 2 ), m_ColorScrollBar[i].y );

		m_Texture.Render( p2DRender, pt );
	}

	for( int j=0; j<3; j++ )
	{
		if( m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x )
			m_Texture.Render( p2DRender, CPoint( m_OriginalColorScrollBar[j].x - ( m_Texture.m_size.cx / 2 ), m_OriginalColorScrollBar[j].y ), 160 );
	}
	
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	// 가격 계산

	BYTE nColorR = (BYTE)( (m_fColor[0] * 255) );
	BYTE nColorG = (BYTE)( (m_fColor[1] * 255) );
	BYTE nColorB = (BYTE)( (m_fColor[2] * 255) );

	BYTE nOrignalR = (BYTE)( g_pPlayer->m_fHairColorR * 255 );
	BYTE nOrignalG = (BYTE)( g_pPlayer->m_fHairColorG * 255 );
	BYTE nOrignalB = (BYTE)( g_pPlayer->m_fHairColorB * 255 );

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	if( (nColorR != nOrignalR || nColorG != nOrignalG || nColorB != nOrignalB) && !m_bUseCoupon )
		m_nHairColorCost = HAIRCOLOR_COST;
	else
		m_nHairColorCost = 0;

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
	//if( nColorR >= nOrignalR  )
	//	m_nHairColorCostR = (nColorR - nOrignalR)*13;
	//else
	//	m_nHairColorCostR = (nOrignalR - nColorR)*7;

	//if( nColorG >= nOrignalG  )
	//	m_nHairColorCostG = (nColorG - nOrignalG)*13;
	//else
	//	m_nHairColorCostG = (nOrignalG - nColorG)*7;

	//if( nColorB >= nOrignalB  )
	//	m_nHairColorCostB = (nColorB - nOrignalB)*13;
	//else
	//	m_nHairColorCostB = (nOrignalB - nColorB)*7;

	CString string;
	CWndStatic* pCost = (CWndStatic*) GetDlgItem( WIDC_COST );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_HK )
	{
		string = "0";
	}
	else
	{
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		string.Format( _T( "%d" ), m_nHairCost + m_nHairColorCost );
//		string.Format( _T( "%d" ), m_nHairCost + m_nHairColorCostR + m_nHairColorCostG + m_nHairColorCostB );	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
	}

	pCost->SetTitle( string );

	pCost = (CWndStatic*) GetDlgItem( WIDC_STATIC1 );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_HK )
	{
		string = "0";
	}
	else
	{
		string.Format( _T( "%d" ), m_nHairCost );
	}
	
	pCost->SetTitle( string );

	pCost = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_HK )
	{
		string = "0";
	}
	else
	{
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
		string.Format( _T( "%d" ), m_nHairColorCost );
//		string.Format( _T( "%d" ), m_nHairColorCostR + m_nHairColorCostG + m_nHairColorCostB );	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가. ( < 8 )
	}
	
	pCost->SetTitle( string );
	
} 

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
void CWndBeautyShop::DrawHairKind(C2DRender* p2DRender, D3DXMATRIX matView)
{
	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;
	
	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matTrans;
	
	//Hair Kind
	DWORD HairNum = m_dwHairMesh;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	
	int custom[4] = {WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4};

	if(m_pHairModel == NULL)
		return;

	LPWNDCTRL lpHair = GetWndCtrl( custom[0] );
	viewport.Width  = lpHair->rect.Width() - 2;
	viewport.Height = lpHair->rect.Height() - 2;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
	FLOAT fov = D3DX_PI/4.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );

	for(int i=0; i<4; i++)
	{
		( HairNum > MAX_HAIR ) ? HairNum = 1: HairNum;
		m_nHairNum[i] = HairNum;

		lpHair = GetWndCtrl( custom[i] );

		//Model Draw
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, HairNum-1, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, m_pHairModel, NULL );
		
		viewport.X      = p2DRender->m_ptOrigin.x + lpHair->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpHair->rect.top;

		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;
		
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.05f,-8.0f,-1.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-7.5f,-1.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );
		
		m_pHairModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pHairModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pHairModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;
		
		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
		
		m_pHairModel->Render( p2DRender->m_pd3dDevice, &matWorld );

		//Select Draw
		if(m_dwSelectHairMesh == m_nHairNum[i])
		{
			CRect rect;
			rect = lpHair->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
		HairNum++;
	}
}

void CWndBeautyShop::UpdateModels()
{
	if(m_pModel != NULL)
	{
		CMover::UpdateParts( g_pPlayer->GetSex(), 
			g_pPlayer->m_dwSkinSet, 
			g_pPlayer->m_dwFace, 
			g_pPlayer->m_dwHairMesh, 
			g_pPlayer->m_dwHeadMesh,
			g_pPlayer->m_aEquipInfo, 
			m_pModel, 
			&g_pPlayer->m_Inventory );
	}
	if(m_pApplyModel != NULL)	
	{
		CMover::UpdateParts( g_pPlayer->GetSex(), 
			g_pPlayer->m_dwSkinSet, 
			g_pPlayer->m_dwFace, 
			m_dwSelectHairMesh-1, 
			g_pPlayer->m_dwHeadMesh,
			g_pPlayer->m_aEquipInfo, 
			m_pApplyModel, 
			&g_pPlayer->m_Inventory );
	}
}


void CWndBeautyShop::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	CRect rect = GetWindowRect( TRUE );

	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );

	Move( ptMove );
	
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM_R );
	rect = lpWndCtrl->rect;
	m_ColorRect[0].left   = rect.left;
	m_ColorRect[0].top    = rect.top + 25;
	m_ColorRect[0].right  = rect.right;
	m_ColorRect[0].bottom = rect.bottom + 25;
	m_ColorRect[0] = rect;

	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM_G );
	rect = lpWndCtrl->rect;
	m_ColorRect[1].left   = rect.left;
	m_ColorRect[1].top    = rect.top + 25;
	m_ColorRect[1].right  = rect.right;
	m_ColorRect[1].bottom = rect.bottom + 25;
	m_ColorRect[1] = rect;
	
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM_B );
	rect = lpWndCtrl->rect;
	m_ColorRect[2].left   = rect.left;
	m_ColorRect[2].top    = rect.top + 25;
	m_ColorRect[2].right  = rect.right;
	m_ColorRect[2].bottom = rect.bottom + 25;
	m_ColorRect[2] = rect;
	
	ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );
	
	m_OriginalColorScrollBar[0] = m_ColorScrollBar[0];
	m_OriginalColorScrollBar[1] = m_ColorScrollBar[1];
	m_OriginalColorScrollBar[2] = m_ColorScrollBar[2];

	m_pRGBEdit[0] = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	m_pRGBEdit[1] = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	m_pRGBEdit[2] = (CWndEdit*)GetDlgItem( WIDC_EDIT3 );

	SetRGBToEdit(m_fColor[0], 0);
	SetRGBToEdit(m_fColor[1], 1);
	SetRGBToEdit(m_fColor[2], 2);

	CWndStatic* kindcost = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	CWndStatic* colorcost = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CWndStatic* totalcost = (CWndStatic*)GetDlgItem( WIDC_COST );
	kindcost->AddWndStyle(WSS_MONEY);
	colorcost->AddWndStyle(WSS_MONEY);
	totalcost->AddWndStyle(WSS_MONEY);
} 

BOOL CWndBeautyShop::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	if( g_pPlayer == NULL )
		return FALSE;

	m_bLButtonClick = FALSE;
	m_dwHairMesh = g_pPlayer->m_dwHairMesh+1;

	SAFE_DELETE( m_pModel );
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? OBJECT_INDEX( 11, MI_MALE ) : OBJECT_INDEX( 12, MI_FEMALE ));
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND2 );

	CMover::UpdateParts( g_pPlayer->GetSex(), 
		g_pPlayer->m_dwSkinSet, 
		g_pPlayer->m_dwFace, 
		g_pPlayer->m_dwHairMesh, 
		g_pPlayer->m_dwHeadMesh,
		g_pPlayer->m_aEquipInfo, 
		m_pModel, 
		&g_pPlayer->m_Inventory );

	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	SAFE_DELETE( m_pApplyModel );
	m_pApplyModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pApplyModel,  OT_MOVER, nMover, MTI_STAND2 );
	CMover::UpdateParts( g_pPlayer->GetSex(), 
		g_pPlayer->m_dwSkinSet, 
		g_pPlayer->m_dwFace, 
		g_pPlayer->m_dwHairMesh, 
		g_pPlayer->m_dwHeadMesh,
		g_pPlayer->m_aEquipInfo, 
		m_pApplyModel, 
		&g_pPlayer->m_Inventory );
	m_pApplyModel->InitDeviceObjects( g_Neuz.GetDevice() );

	m_fColor[0] = g_pPlayer->m_fHairColorR;
	m_fColor[1] = g_pPlayer->m_fHairColorG;
	m_fColor[2] = g_pPlayer->m_fHairColorB;

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	m_dwSelectHairMesh = m_dwHairMesh;

	SAFE_DELETE(m_pHairModel);
	m_pHairModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pHairModel,  OT_MOVER, nMover, MTI_STAND2 );
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pHairModel, &g_pPlayer->m_Inventory );
	m_pHairModel->InitDeviceObjects( g_Neuz.GetDevice() );

	///
	m_nHairCost = 0;
	m_nHairColorCost = 0;	

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
	///
	//m_ColorRect[0].left   = 44;
	//m_ColorRect[0].top    = 251;
	//m_ColorRect[0].right  = 162;
	//m_ColorRect[0].bottom = 267;
	//
	//m_ColorRect[1].left   = 44;
	//m_ColorRect[1].top    = 272;
	//m_ColorRect[1].right  = 162;
	//m_ColorRect[1].bottom = 287;

	//m_ColorRect[2].left   = 44;
	//m_ColorRect[2].top    = 293;
	//m_ColorRect[2].right  = 162;
	//m_ColorRect[2].bottom = 307;

	//m_nHairCost   = 0;
	//m_nHairColorCostR  = 0;
	//m_nHairColorCostG  = 0;
	//m_nHairColorCostB  = 0;

	//ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );

	//m_OriginalColorScrollBar[0] = m_ColorScrollBar[0];
	//m_OriginalColorScrollBar[1] = m_ColorScrollBar[1];
	//m_OriginalColorScrollBar[2] = m_ColorScrollBar[2];

	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"yellowbuttten.tga" ), 0xffff00ff, TRUE );

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_EX, 0, 0, pWndParent );
//	return InitDialog( g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP, 0, 0, pWndParent );		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
}

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
void CWndBeautyShop::SetRGBToEdit(float color, int editnum)
{
	char szNumberbuf[8] = {0, };
#ifdef __Y_HAIR_BUG_FIX
	float colorval = (color / 1.0f) * 255;
#else //__Y_HAIR_BUG_FIX
	float colorval = ((color-0.3f)/(1.0f - 0.3f)) * 255 + 0.5f;
#endif //__Y_HAIR_BUG_FIX
	FLIntToString( (int)( colorval ), szNumberbuf, _countof( szNumberbuf ), 10 );
	m_pRGBEdit[editnum]->SetString(szNumberbuf);
}

void CWndBeautyShop::SetRGBToBar(int editnum)
{
	float RGBNum;
	int ColorNum;
	CWndEdit* pWndEdit;
	
	if(editnum == WIDC_EDIT1)
	{
		ColorNum = 0;
		pWndEdit = m_pRGBEdit[0];
	}
	else if(editnum == WIDC_EDIT2)
	{
		ColorNum = 1;
		pWndEdit = m_pRGBEdit[1];
	}
	else if(editnum == WIDC_EDIT3)
	{
		ColorNum = 2;
		pWndEdit = m_pRGBEdit[2];
	}

	RGBNum = (float)( atoi(pWndEdit->GetString()) );

	if(RGBNum < 0)
	{
		RGBNum = 0;
		pWndEdit->SetString("0");
	}
#ifdef __Y_HAIR_BUG_FIX
	else if(RGBNum > 255)
	{
		RGBNum = 255;
		pWndEdit->SetString("255");
	}	
#else //__Y_HAIR_BUG_FIX
	else if(RGBNum > 254)
	{
		RGBNum = 254;
		pWndEdit->SetString("254");
	}	
#endif //__Y_HAIR_BUG_FIX

#ifdef __Y_HAIR_BUG_FIX
	m_fColor[ColorNum] = (RGBNum / 255) * 1.0f;
#else //__Y_HAIR_BUG_FIX
	m_fColor[ColorNum] = (RGBNum / 255) * (1.0f - 0.3f) + 0.3f;
#endif //__Y_HAIR_BUG_FIX
	ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );
}

void CWndBeautyShop::ReSetBar( FLOAT r, FLOAT g, FLOAT b )
{
#ifdef __Y_HAIR_BUG_FIX
	FLOAT fR = (r/1.0f) * 100.0f;
	FLOAT fG = (g/1.0f) * 100.0f;
	FLOAT fB = (b/1.0f) * 100.0f;
#else //__Y_HAIR_BUG_FIX
	FLOAT fR = ((r-0.3f)/(1.0f - 0.3f)) * 100.0f;
	FLOAT fG = ((g-0.3f)/(1.0f - 0.3f)) * 100.0f;
	FLOAT fB = ((b-0.3f)/(1.0f - 0.3f)) * 100.0f;
#endif //__Y_HAIR_BUG_FIX

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	m_ColorScrollBar[0].x = (LONG)( (((m_ColorRect[0].right-m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left );
	m_ColorScrollBar[0].y = m_ColorRect[0].top;
	m_ColorScrollBar[1].x = (LONG)( (((m_ColorRect[1].right-m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left );
	m_ColorScrollBar[1].y = m_ColorRect[1].top;
	m_ColorScrollBar[2].x = (LONG)( (((m_ColorRect[2].right-m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left );
	m_ColorScrollBar[2].y = m_ColorRect[2].top;

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
	//m_ColorScrollBar[0].x = (LONG)( (((m_ColorRect[0].right-m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left );
	//m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	//m_ColorScrollBar[1].x = (LONG)( (((m_ColorRect[1].right-m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left );
	//m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	//m_ColorScrollBar[2].x = (LONG)( (((m_ColorRect[2].right-m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left );
	//m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
}

void CWndBeautyShop::OnMouseWndSurface( CPoint point )
{
	if( g_pPlayer == NULL )
		return;
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	CRect rect = CRect( 44, 198, 186, 398 );
//	CRect rect = CRect( 22, 198, 186, 298 );	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )

	if( !rect.PtInRect( point ) )
		m_bLButtonClick = FALSE;

// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	if( m_ChoiceBar != -1 && m_bLButtonClick )
	{
		CRect DrawRect = m_ColorRect[m_ChoiceBar];
		point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
		
		LONG Width = DrawRect.right - DrawRect.left;
		LONG Pos   = point.x - DrawRect.left;
		
		FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));
			
#ifdef __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1= D3DXVECTOR2( 0.0f, 1.0f );
		D3DXVECTOR2 vec2= D3DXVECTOR2( 1.0f, 1.0f );
#else //__Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1= D3DXVECTOR2( 0.3f, 0.998f );
		D3DXVECTOR2 vec2= D3DXVECTOR2( 0.998f, 0.998f );
#endif //__Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec3;
			
		D3DXVec2Lerp( &vec3, &vec1, &vec2, p );
		
		m_fColor[m_ChoiceBar] = vec3.x;
			
		m_ColorScrollBar[m_ChoiceBar].x = point.x;
			
		SetRGBToEdit(m_fColor[m_ChoiceBar], m_ChoiceBar);
	}
}
	
BOOL CWndBeautyShop::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndBeautyShop::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndBeautyShop::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_ChoiceBar = -1;		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	m_bLButtonClick = FALSE;
} 

void CWndBeautyShop::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	int i;
	int custom[4] = {WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4};
	LPWNDCTRL lpWndCtrl;

	for( i=0; i<3; i++ )
	{
		CRect DrawRect = m_ColorRect[i];
		if(DrawRect.PtInRect( point ))
			m_ChoiceBar = i;
	}
	for( i=0; i<4; i++ )
	{
		lpWndCtrl = GetWndCtrl( custom[i] );
		CRect DrawRect = lpWndCtrl->rect;
		if(DrawRect.PtInRect( point ))
		{
			//Hair 선택..
			m_dwSelectHairMesh = m_nHairNum[i];
	
			CMover::UpdateParts( g_pPlayer->GetSex(), 
									g_pPlayer->m_dwSkinSet, 
									g_pPlayer->m_dwFace, 
									m_dwSelectHairMesh-1, 
									g_pPlayer->m_dwHeadMesh,
									g_pPlayer->m_aEquipInfo, 
									m_pApplyModel, 
									&g_pPlayer->m_Inventory );
				
			//요금 계산..
			if( g_pPlayer->m_dwHairMesh != m_dwSelectHairMesh-1 && !m_bUseCoupon)
			{
				m_nHairCost = HAIR_COST;
			}
			else
				m_nHairCost = 0;
		}
	}

	m_bLButtonClick = TRUE;
} 

BOOL CWndBeautyShop::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( g_pPlayer == NULL )
		return FALSE;

	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case WIDC_BUTTON1: 
				{
					//ResetButto임
					//m_pModel->DeleteDeviceObjects();

					m_dwHairMesh = g_pPlayer->m_dwHairMesh+1;

					// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
					CMover::UpdateParts( g_pPlayer->GetSex(), 
											g_pPlayer->m_dwSkinSet, 
											g_pPlayer->m_dwFace, 
											m_dwHairMesh-1, 
											g_pPlayer->m_dwHeadMesh,
											g_pPlayer->m_aEquipInfo, 
											m_pApplyModel, 
											&g_pPlayer->m_Inventory );
		

					// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
//					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
					
					m_fColor[0] = g_pPlayer->m_fHairColorR;
					m_fColor[1] = g_pPlayer->m_fHairColorG;
					m_fColor[2] = g_pPlayer->m_fHairColorB;

					m_nHairCost = 0;
					
					ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
					m_nHairColorCost = 0;
					m_dwSelectHairMesh = m_dwHairMesh;
					SetRGBToEdit(m_fColor[0], 0);
					SetRGBToEdit(m_fColor[1], 1);
					SetRGBToEdit(m_fColor[2], 2);
				}
				break;
			case WIDC_HAIRSTYLE_LEFT: // hair
				{
					m_dwHairMesh--;
					( m_dwHairMesh < 1 ) ? m_dwHairMesh = MAX_HAIR: m_dwHairMesh;
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	
					CMover::UpdateParts( g_pPlayer->GetSex(), 
										g_pPlayer->m_dwSkinSet, 
										g_pPlayer->m_dwFace, 
										m_dwHairMesh-1, 
										g_pPlayer->m_dwHeadMesh,
										g_pPlayer->m_aEquipInfo, 
										m_pModel, 
										&g_pPlayer->m_Inventory );
			
					if( g_pPlayer->m_dwHairMesh != m_dwHairMesh-1 )
					{
						//gmpbigsun( 20100913, #0010464 ) : 페이지 버튼 클릭시 이상한 금액으로 보여지는 문제, 주석처리함
						//switch( m_dwHairMesh )
						//{
						//	case 1:
						//		m_nHairCost = 2500;
						//		break;
						//	case 2:
						//		m_nHairCost = 2500;
						//		break;
						//	case 3:
						//		m_nHairCost = 2500;
						//		break;
						//	case 4:
						//		m_nHairCost = 2500;
						//		break;
						//	case 5:
						//		m_nHairCost = 2500;
						//		break;
						//	default:
						//		m_nHairCost = 4000;
						//		break;
						//}
					}
					//flyingjin 201001
					//버튼을 누를때마다 가격이 0되는 문제 주석처리함
					//else
					//	m_nHairCost = 0;
				}
				break;
			case WIDC_HAIRSTYLE_RIGHT: // hair
				{
					m_dwHairMesh++;
					( m_dwHairMesh > MAX_HAIR ) ? m_dwHairMesh = 1: m_dwHairMesh;
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.

					CMover::UpdateParts( g_pPlayer->GetSex(), 
											g_pPlayer->m_dwSkinSet, 
											g_pPlayer->m_dwFace, 
											m_dwHairMesh-1, 
											g_pPlayer->m_dwHeadMesh,
											g_pPlayer->m_aEquipInfo, 
											m_pModel, 
											&g_pPlayer->m_Inventory );
					
					if( g_pPlayer->m_dwHairMesh != m_dwHairMesh-1 )
					{
						//gmpbigsun( 20100913, #0010464 ) : 페이지 버튼 클릭시 이상한 금액으로 보여지는 문제, 주석처리함
						//switch( m_dwHairMesh )
						//{
						//case 1:
						//	m_nHairCost = 2500;
						//	break;
						//case 2:
						//	m_nHairCost = 2500;
						//	break;
						//case 3:
						//	m_nHairCost = 2500;
						//	break;
						//case 4:
						//	m_nHairCost = 2500;
						//	break;
						//case 5:
						//	m_nHairCost = 2500;
						//	break;
						//default:
						//	m_nHairCost = 4000;
						//	break;
						//}
					}
					//flyingjin 201001
					//버튼을 누를때마다 가격이 0되는 문제 주석처리함
					//else
					//	m_nHairCost = 0;
				}
				break;
			case WIDC_OK:
				{
					BOOL noChange = FALSE;
					BYTE nColorR = (BYTE)( (m_fColor[0] * 255) );
					BYTE nColorG = (BYTE)( (m_fColor[1] * 255) );
					BYTE nColorB = (BYTE)( (m_fColor[2] * 255) );
					
					BYTE nOrignalR = (BYTE)( g_pPlayer->m_fHairColorR * 255 );
					BYTE nOrignalG = (BYTE)( g_pPlayer->m_fHairColorG * 255 );
					BYTE nOrignalB = (BYTE)( g_pPlayer->m_fHairColorB * 255 );
					
					if((g_pPlayer->m_dwHairMesh == m_dwSelectHairMesh-1 ) && (nColorR == nOrignalR && nColorG == nOrignalG && nColorB == nOrignalB))
					{
						noChange = TRUE;
					}

					if( g_pPlayer )
					{
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
						int nCost = m_nHairCost + m_nHairColorCost;							
						if( nCost < 0 )
							nCost = 0;
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
						if(m_bUseCoupon && !noChange)
						{
							if(m_pWndUseCouponConfirm == NULL)
							{									
								m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
								m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_EX, 1);
								m_pWndUseCouponConfirm->Initialize(this);							
							}
						}			
						if(nCost >= 0)
						{
							if(m_pWndBeautyShopConfirm == NULL)
							{
								m_pWndBeautyShopConfirm = new CWndBeautyShopConfirm;
								m_pWndBeautyShopConfirm->Initialize(this);
							}
						}
					}
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
					int nCost = m_nHairCost + m_nHairColorCost;
					if(nCost < 0 && (!m_bUseCoupon || noChange))
						Destroy();
				}
				break;
			case WIDC_CANCEL:
				{
					Destroy();
				}
				break;
		}
	}
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	if(nID == WIDC_EDIT1 || nID == WIDC_EDIT2 || nID == WIDC_EDIT3)
		SetRGBToBar(nID);

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndBeautyShop::OnDestroyChildWnd( CWndBase* pWndChild )
{
// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	if( pWndChild == m_pWndConfirmSell )
		SAFE_DELETE( m_pWndConfirmSell );

	//gmpbigsun( 20100913, #0010488 ) : 확인창이 자식으로 등록되어 있어 확인창에서 No를선택해도 모델을 지우지 않게
	//SAFE_DELETE( m_pModel );
}

void CWndBeautyShop::OnDestroy( void )
{
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pWndConfirmSell );

	// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pHairModel);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
}

CWndUseCouponConfirm::CWndUseCouponConfirm() 
{
	m_bUseCoupon = FALSE;
	m_checkClose = TRUE;
	m_TargetWndId = -1;
}
 
CWndUseCouponConfirm::~CWndUseCouponConfirm() 
{ 
} 

void CWndUseCouponConfirm::OnDestroy()
{
	if( m_bUseCoupon == FALSE )
		return;

	if(!m_checkClose && m_MainFlag == 0)
	{
		if(m_TargetWndId != -1)
		{
			if(m_TargetWndId == APP_BEAUTY_SHOP_EX)
			{
				g_WndMng.CreateApplet( APP_INVENTORY );			
				SAFE_DELETE( g_WndMng.m_pWndBeautyShop );
				g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;					
				g_WndMng.m_pWndBeautyShop->Initialize( NULL, APP_BEAUTY_SHOP_EX );
				g_WndMng.m_pWndBeautyShop->UseHairCoupon(m_bUseCoupon);
			}
			else if(m_TargetWndId == APP_BEAUTY_SHOP_SKIN)
			{
				g_WndMng.CreateApplet( APP_INVENTORY );			
				SAFE_DELETE( g_WndMng.m_pWndFaceShop );
				g_WndMng.m_pWndFaceShop = new CWndFaceShop;										
				g_WndMng.m_pWndFaceShop->Initialize( NULL, APP_BEAUTY_SHOP_EX );
				g_WndMng.m_pWndFaceShop->UseFaceCoupon(m_bUseCoupon);
			}	
		}
	}
	if(this->GetParentWnd() == g_WndMng.GetWndBase( APP_WORLD ))
		g_WndMng.m_pWndUseCouponConfirm = NULL;	
	else if(this->GetParentWnd() == g_WndMng.GetWndBase( APP_BEAUTY_SHOP_EX ))
		g_WndMng.m_pWndBeautyShop->m_pWndUseCouponConfirm = NULL;
	else if(this->GetParentWnd() == g_WndMng.GetWndBase( APP_BEAUTY_SHOP_SKIN ))
		g_WndMng.m_pWndFaceShop->m_pWndUseCouponConfirm = NULL;
}

void CWndUseCouponConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndUseCouponConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
/*	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pStatic->SetVisible(FALSE);
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	pStatic->SetVisible(FALSE);
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	pStatic->SetVisible(FALSE);
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	pStatic->SetVisible(FALSE);
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	pStatic->SetVisible(FALSE);
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pStatic->SetVisible(FALSE);
*/	
	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	SetTitle(prj.GetText(TID_GAME_USE_CHANGE_COUPON));

	if(m_TargetWndId != -1)
	{
		if(m_MainFlag == 0)
			pText->AddString(prj.GetText( TID_GAME_ASKUSE_COUPON1 ));
		else if(m_MainFlag == 1)
		{
			pText->AddString(prj.GetText( TID_GAME_ASKUSE_COUPON2 ));
			pText->AddString("\n");
			pText->AddString(prj.GetText( TID_GAME_WARNNING_USE_COUPON ), 0xffff0000);
		}
	}
	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndUseCouponConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_EX_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndUseCouponConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndUseCouponConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndUseCouponConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndUseCouponConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndUseCouponConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_MainFlag == 0)
			m_bUseCoupon = TRUE;
		else if(m_MainFlag == 1)
		{
			if(m_TargetWndId == APP_BEAUTY_SHOP_EX)
			{
				CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)this->GetParentWnd();
				g_DPlay.SendSetHair( (BYTE)( pWndBeautyShop->m_dwSelectHairMesh-1 ), pWndBeautyShop->m_fColor[0], pWndBeautyShop->m_fColor[1], pWndBeautyShop->m_fColor[2] );
				pWndBeautyShop->Destroy();
			}
			else if(m_TargetWndId == APP_BEAUTY_SHOP_SKIN)
			{
				CWndFaceShop* pWndFaceShop = (CWndFaceShop*)this->GetParentWnd();
				g_DPlay.SendChangeFace( g_pPlayer->m_idPlayer, pWndFaceShop->m_nSelectedFace-1, pWndFaceShop->m_nCost );
				pWndFaceShop->Destroy();
			}
		}	
		m_checkClose = FALSE;
	}
	else if( nID == WIDC_NO )
		m_checkClose = FALSE;
	else if( nID == WTBID_CLOSE )
		m_checkClose = TRUE;

	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndUseCouponConfirm::SetInfo(DWORD targetWndId, int flag)
{
	m_TargetWndId = targetWndId;
	m_MainFlag = flag;
}

/*************************
CWndBeautyShopConfirm Class
*************************/
CWndBeautyShopConfirm::CWndBeautyShopConfirm() 
{ 
	m_ParentId = 0;
}
 
CWndBeautyShopConfirm::~CWndBeautyShopConfirm() 
{ 
} 

void CWndBeautyShopConfirm::OnDestroy()
{
	CWndBase* pWndBase = this->GetParentWnd();
	if(m_ParentId == APP_BEAUTY_SHOP_EX)
	{
		CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)pWndBase;
		pWndBeautyShop->m_pWndBeautyShopConfirm = NULL;
	}
	else if(m_ParentId == APP_BEAUTY_SHOP_SKIN)
	{
		CWndFaceShop* pWndFaceShop = (CWndFaceShop*)pWndBase;
		pWndFaceShop->m_pWndBeautyShopConfirm = NULL;
	}
}

void CWndBeautyShopConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndBeautyShopConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
/*	char szNumberbuf[16] = {0, };
	int TotalCost;
	CWndStatic* pCostStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pCostStatic->AddWndStyle(WSS_MONEY);
	CWndStatic* pStatic1 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CWndStatic* pStatic2 = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	CWndBase* pWndBase = this->GetParentWnd();
	m_ParentId = pWndBase->GetWndApplet()->dwWndId;
	if(m_ParentId == APP_BEAUTY_SHOP_EX)
	{
		CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)pWndBase;
		TotalCost = pWndBeautyShop->m_nHairCost + pWndBeautyShop->m_nHairColorCost;
		pStatic1->SetTitle(prj.GetText(TID_GAME_CHANGE_HAIR));
		pStatic2->SetTitle(prj.GetText(TID_GAME_CHANGE_HAIR_QUESTION));
	}
	else if(m_ParentId == APP_BEAUTY_SHOP_SKIN)
	{
		CWndFaceShop* pWndFaceShop = (CWndFaceShop*)pWndBase;
		TotalCost = pWndFaceShop->m_nCost;
		pStatic1->SetTitle(prj.GetText(TID_GAME_CHANGE_FACE));
		pStatic2->SetTitle(prj.GetText(TID_GAME_CHANGE_FACE_QUESTION));
	}
	_itoa( TotalCost, szNumberbuf, 10 );
	pCostStatic->SetTitle(szNumberbuf);
*/
	int TotalCost;
	CString strText, strNum;

	CWndBase* pWndBase = this->GetParentWnd();
	m_ParentId = pWndBase->GetWndApplet()->dwWndId;
	if(m_ParentId == APP_BEAUTY_SHOP_EX)
	{
		CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)pWndBase;
		TotalCost = pWndBeautyShop->m_nHairCost + pWndBeautyShop->m_nHairColorCost;
		strNum.Format("%d", TotalCost);
		strText.Format("%s %s %s %s %s %s", prj.GetText(TID_GAME_SHOP_CHOICE), prj.GetText(TID_GAME_CHANGE_HAIR), prj.GetText(TID_GAME_SHOP_COST),
			GetNumberFormatEx(strNum), prj.GetText(TID_GAME_SHOP_PENYA), prj.GetText(TID_GAME_CHANGE_HAIR_QUESTION) );
	}
	else if(m_ParentId == APP_BEAUTY_SHOP_SKIN)
	{
		CWndFaceShop* pWndFaceShop = (CWndFaceShop*)pWndBase;
		TotalCost = pWndFaceShop->m_nCost;
		strNum.Format("%d", TotalCost);
		strText.Format("%s %s %s %s %s %s", prj.GetText(TID_GAME_SHOP_CHOICE), prj.GetText(TID_GAME_CHANGE_FACE), prj.GetText(TID_GAME_SHOP_COST),
			GetNumberFormatEx(strNum), prj.GetText(TID_GAME_SHOP_PENYA), prj.GetText(TID_GAME_CHANGE_FACE_QUESTION) );
	}

	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pText->SetString(strText);

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBeautyShopConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_EX_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndBeautyShopConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBeautyShopConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndBeautyShopConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndBeautyShopConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndBeautyShopConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_ParentId == APP_BEAUTY_SHOP_EX)
		{
			CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)this->GetParentWnd();
			if( pWndBeautyShop )
			{
				g_DPlay.SendSetHair( (BYTE)( pWndBeautyShop->m_dwSelectHairMesh-1 ), pWndBeautyShop->m_fColor[0], pWndBeautyShop->m_fColor[1], pWndBeautyShop->m_fColor[2] );
			}
			pWndBeautyShop->Destroy();
		}
		else if(m_ParentId == APP_BEAUTY_SHOP_SKIN)
		{
			CWndFaceShop* pWndFaceShop = (CWndFaceShop*)this->GetParentWnd();
			if( pWndFaceShop )
			{
				g_DPlay.SendChangeFace( g_pPlayer->m_idPlayer, pWndFaceShop->m_nSelectedFace-1, pWndFaceShop->m_nCost );
			}
			pWndFaceShop->Destroy();
		}
		Destroy();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
		Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/*************************
	CWndFaceShop Class
*************************/
CWndFaceShop::CWndFaceShop()
{ 
	m_pMainModel = NULL;
	m_pApplyModel = NULL;
	m_pFriendshipFace = NULL;
	m_pNewFace = NULL;
	m_pWndBeautyShopConfirm = NULL;
	m_nSelectedFace = 1;
	m_dwFriendshipFace = 1;
	m_dwNewFace = 6;
	m_nCost = 0;
	m_ChoiceBar = -1;
	m_bUseCoupon = FALSE;
	m_pWndUseCouponConfirm = NULL;
} 

CWndFaceShop::~CWndFaceShop() 
{ 
	SAFE_DELETE(m_pMainModel);
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pFriendshipFace);
	SAFE_DELETE(m_pNewFace);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
	SAFE_DELETE(m_pWndUseCouponConfirm);
} 

void CWndFaceShop::OnDestroy()
{
	SAFE_DELETE(m_pMainModel);
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pFriendshipFace);
	SAFE_DELETE(m_pNewFace);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
}

void CWndFaceShop::OnDestroyChildWnd( CWndBase* pWndChild )
{
}

void CWndFaceShop::UseFaceCoupon(BOOL isUse)
{
	m_bUseCoupon = isUse;
	if(m_bUseCoupon)
	{
		CString title = GetTitle();
		CString addText;
		addText.Format("  %s", prj.GetText( TID_GAME_NOWUSING_COUPON ));
		title = title + addText;
		SetTitle(title);	
	}		
}

void CWndFaceShop::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_pPlayer == NULL || m_pMainModel == NULL || m_pApplyModel == NULL )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
#endif //__YENV
	
	// 왼쪽 원본 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM5 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width  = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixScaling(&matScale, 4.5f, 4.5f, 4.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.0f,-5.6f,0.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-5.2f,0.0f);

		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
		if( g_pPlayer )
			g_pPlayer->OverCoatItemRenderCheck(m_pMainModel);
		
		m_pMainModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}
	// 오른쪽 얼굴변경 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width  = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();
		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;

		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixRotationY(&matRot,g_tmCurrent/1000.0f);
		D3DXMatrixScaling(&matScale, 4.5f, 4.5f, 4.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.0f,-5.6f,0.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-5.2f,0.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		
		::SetLight( FALSE );
		::SetFog( FALSE );

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );

		if( g_pPlayer )
			g_pPlayer->OverCoatItemRenderCheck(m_pApplyModel);

		m_pApplyModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}
	viewport.X      = p2DRender->m_ptOrigin.x;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	
	DrawFaces(0, p2DRender, matView);
	DrawFaces(1, p2DRender, matView);
} 

void CWndFaceShop::DrawFaces(int ChoiceFlag, C2DRender* p2DRender, D3DXMATRIX matView)
{
	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;
	
	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matTrans;
	
	//Face Kind
	DWORD FaceNum;
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	CModelObject* m_pFaceModel;

	if(ChoiceFlag == 0)
	{
		m_pFaceModel = m_pFriendshipFace;
		FaceNum = m_dwFriendshipFace;
	}
	else if(ChoiceFlag == 1)
	{
		m_pFaceModel = m_pNewFace;
		FaceNum = m_dwNewFace;
	}

	int custom_friend[4] = {WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4};
	int custom_new[4] = {WIDC_CUSTOM7, WIDC_CUSTOM8, WIDC_CUSTOM9, WIDC_CUSTOM10};

	if(m_pFaceModel == NULL)
		return;

	LPWNDCTRL lpFace = GetWndCtrl( custom_friend[0] );
	viewport.Width  = lpFace->rect.Width() - 2;
	viewport.Height = lpFace->rect.Height() - 2;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
	FLOAT fov = D3DX_PI/4.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );

	for(int i=0; i<4; i++)
	{
		if(ChoiceFlag == 0)
		{
			( FaceNum > MAX_DEFAULT_HEAD ) ? FaceNum = 1: FaceNum;
			lpFace = GetWndCtrl( custom_friend[i] );
			m_nFriendshipFaceNum[i] = FaceNum;
		}
		else if(ChoiceFlag == 1)
		{
			( FaceNum > MAX_HEAD ) ? FaceNum = 6: FaceNum;
			lpFace = GetWndCtrl( custom_new[i] );
			m_nNewFaceNum[i] = FaceNum;
		}

		//Model Draw
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, FaceNum-1,g_pPlayer->m_aEquipInfo, m_pFaceModel, NULL );
		
		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;

		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;
		
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixScaling(&matScale, 7.5f, 7.5f, 7.5f);
		
		if( g_pPlayer->GetSex() == SEX_MALE )
			D3DXMatrixTranslation(&matTrans,0.05f,-10.0f,-1.0f);
		else
			D3DXMatrixTranslation(&matTrans,0.0f,-9.5f,-1.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );
		
		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
		
		m_pFaceModel->Render( p2DRender->m_pd3dDevice, &matWorld );

		//Select Draw
		if(ChoiceFlag == 0)
		{
			if(m_nSelectedFace == m_nFriendshipFaceNum[i])
			{
				CRect rect;
				rect = lpFace->rect;
				p2DRender->RenderFillRect( rect, 0x60ffff00 );
			}
		}
		else if(ChoiceFlag == 1)
		{
			if(m_nSelectedFace == m_nNewFaceNum[i])
			{
				CRect rect;
				rect = lpFace->rect;
				p2DRender->RenderFillRect( rect, 0x60ffff00 );
			}
		}
		FaceNum++;
	}
}

void CWndFaceShop::UpdateModels()
{
	if(m_pMainModel != NULL)
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pMainModel, &g_pPlayer->m_Inventory );
	if(m_pApplyModel != NULL)	
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, m_nSelectedFace-1, g_pPlayer->m_aEquipInfo, m_pApplyModel, &g_pPlayer->m_Inventory );	
}

void CWndFaceShop::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );

	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );

	Move( ptMove );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndFaceShop::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	if( g_pPlayer == NULL )
		return FALSE;

	if(g_pPlayer->m_dwHeadMesh >= 0 && g_pPlayer->m_dwHeadMesh < 6)
		m_dwFriendshipFace = g_pPlayer->m_dwHeadMesh + 1;
	else if(g_pPlayer->m_dwHeadMesh >= 6 && g_pPlayer->m_dwHeadMesh < 15)
		m_dwNewFace = g_pPlayer->m_dwHeadMesh + 1;

	m_nSelectedFace = g_pPlayer->m_dwHeadMesh + 1;	
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? OBJECT_INDEX( 11, MI_MALE ) : OBJECT_INDEX( 12, MI_FEMALE ));

	SAFE_DELETE( m_pMainModel );
	m_pMainModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pMainModel,  OT_MOVER, nMover, MTI_STAND2 );
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pMainModel, &g_pPlayer->m_Inventory );
	m_pMainModel->InitDeviceObjects( g_Neuz.GetDevice() );

	SAFE_DELETE( m_pApplyModel );
	m_pApplyModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pApplyModel,  OT_MOVER, nMover, MTI_STAND2 );
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pApplyModel, &g_pPlayer->m_Inventory );
	m_pApplyModel->InitDeviceObjects( g_Neuz.GetDevice() );	

	SAFE_DELETE(m_pFriendshipFace);
	m_pFriendshipFace = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pFriendshipFace,  OT_MOVER, nMover, MTI_STAND2 );
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pFriendshipFace, &g_pPlayer->m_Inventory );
	m_pFriendshipFace->InitDeviceObjects( g_Neuz.GetDevice() );

	SAFE_DELETE(m_pNewFace);
	m_pNewFace = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pNewFace,  OT_MOVER, nMover, MTI_STAND2 );
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pNewFace, &g_pPlayer->m_Inventory );
	m_pNewFace->InitDeviceObjects( g_Neuz.GetDevice() );
	
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_SKIN, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndFaceShop::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}
 
void CWndFaceShop::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}
 
void CWndFaceShop::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}
 
void CWndFaceShop::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	int i;
	int custom[8] = {WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4,
					WIDC_CUSTOM7, WIDC_CUSTOM8, WIDC_CUSTOM9, WIDC_CUSTOM10};
	LPWNDCTRL lpWndCtrl;
	
	for( i=0; i<8; i++ )
	{
		lpWndCtrl = GetWndCtrl( custom[i] );
		CRect DrawRect = lpWndCtrl->rect;
		if(DrawRect.PtInRect( point ))
		{
			//Face 선택..
			if(i>=0 && i<4)
				m_nSelectedFace = m_nFriendshipFaceNum[i];
			else if(i>=4 && i<8)
				m_nSelectedFace = m_nNewFaceNum[i-4];
			
			CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, m_nSelectedFace-1, g_pPlayer->m_aEquipInfo, m_pApplyModel, &g_pPlayer->m_Inventory );
			
			//요금 계산..
			if( g_pPlayer->m_dwHeadMesh != m_nSelectedFace-1 && !m_bUseCoupon)
				m_nCost = CHANGE_FACE_COST;
			else
				m_nCost = 0;
		}
	}
} 

BOOL CWndFaceShop::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( g_pPlayer == NULL )
		return FALSE;

	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case WIDC_BTN_RESET: 
				{
					if(g_pPlayer->m_dwHeadMesh >= 0 && g_pPlayer->m_dwHeadMesh < 6)
					{
						m_dwFriendshipFace = g_pPlayer->m_dwHeadMesh + 1;
						m_dwNewFace = 6;
					}
					else if(g_pPlayer->m_dwHeadMesh >= 6 && g_pPlayer->m_dwHeadMesh < 16)
					{
						m_dwNewFace = g_pPlayer->m_dwHeadMesh + 1;
						m_dwFriendshipFace = 1;
					}
					m_nSelectedFace = g_pPlayer->m_dwHeadMesh + 1;
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, m_nSelectedFace-1,g_pPlayer->m_aEquipInfo, m_pApplyModel, &g_pPlayer->m_Inventory );
					
					m_nCost = 0;
				}
				break;
			case WIDC_FRIENDSHIPFACE_LEFT:
				{
					m_dwFriendshipFace--;
					( m_dwFriendshipFace < 1 ) ? m_dwFriendshipFace = MAX_DEFAULT_HEAD: m_dwFriendshipFace;
				}
				break;
			case WIDC_FRIENDSHIPFACE_RIGHT:
				{
					m_dwFriendshipFace++;
					( m_dwFriendshipFace > MAX_DEFAULT_HEAD ) ? m_dwFriendshipFace = 1: m_dwFriendshipFace;
				}
				break;
			case WIDC_NEWFACE_LEFT:
				{
					m_dwNewFace--;
					( m_dwNewFace < 6 ) ? m_dwNewFace = MAX_HEAD: m_dwNewFace;
				}
				break;
			case WIDC_NEWFACE_RIGHT:
				{
					m_dwNewFace++;
					( m_dwNewFace > MAX_HEAD ) ? m_dwNewFace = 6: m_dwNewFace;
				}
				break;
			case WIDC_BTN_OK:
				{
					BOOL noChange = FALSE;
					if(g_pPlayer->m_dwHeadMesh == m_nSelectedFace-1)
						noChange = TRUE;

					if(m_nCost < 0 && (!m_bUseCoupon || noChange))
						Destroy();
					else if( g_pPlayer )
					{
						if( m_nCost < 0 )
							m_nCost = 0;
						if(m_bUseCoupon && !noChange)
						{
							if(m_pWndUseCouponConfirm == NULL)
							{	
								m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
								m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_SKIN, 1);
								m_pWndUseCouponConfirm->Initialize(this);							
							}
						}
						if(m_nCost >= 0)
						{
							if(m_pWndBeautyShopConfirm == NULL)
							{
								m_pWndBeautyShopConfirm = new CWndBeautyShopConfirm;
								m_pWndBeautyShopConfirm->Initialize(this);
							}
						}
					}
				}
				break;
			case WIDC_BTN_CANCEL:
				{
					Destroy();
				}
				break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//------------------------------------------------------------------------------
//				17차 : CartProcess
//				flyingjin
//------------------------------------------------------------------------------
FLWndCart::FLWndCart()
{
	m_pMover = NULL;
	m_pItemElem = NULL;
	m_nCartCnt = 0;
	m_bSameID = TRUE;
	m_lpCharacter = NULL;
	m_bBlockState = TRUE;
}


//------------------------------------------------------------------------------
FLWndCart::~FLWndCart()
{
	m_pMover = NULL;
	m_pItemElem = NULL;
	m_nCartCnt = 0;
	m_lpCharacter = NULL;
	m_bBlockState = FALSE;
}



//---------------------------------------------------------------------------------------------
// 아이템 하냐의 가격 구하기
// param	: void
// return	: 가격 
//---------------------------------------------------------------------------------------------
UINT FLWndCart::GetCostofOneItem()
{
	FLERROR_ASSERT_LOG_RETURN( g_pPlayer, NULL, _T("[윈도우 장바구니] 전역 플레이어 포인터가 유효하지 않음") );
	FLERROR_ASSERT_LOG_RETURN( m_pItemElem, NULL, _T("[윈도우 장바구니] 아이템 포인터가 유효하지 않음") );

	int nCost( m_pItemElem->GetCost() );

	if( CTax::GetInstance()->IsApplyPurchaseTaxRate( m_pMover, m_pItemElem ) )
		nCost += ( static_cast<int>(nCost * CTax::GetInstance()->GetPurchaseTaxRate( m_pMover )) );

	return nCost;
}



//------------------------------------------------------------------------------
BOOL FLWndCart::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, WBS_KEY| WBS_MODAL, 0, pWndParent );
	MoveParentCenter();
	m_pWndEdit = (CWndEdit*)GetDlgItem( WIDC_ET_AMOUNT );
	m_pWndEdit->AddWndStyle( EBS_AUTOHSCROLL | EBS_NUMBER );
	m_pWndItemCnt = (CWndStatic*)GetDlgItem( WIDC_ST_SUM);
	m_pWndEdit->SetString("1");
	m_bBlockState = TRUE;
	m_pWndEdit->SetTextSelectBlock(0,m_pWndEdit->GetOffset());
	
	m_lpCharacter = m_pMover->GetCharacter();
	m_pWndCostText = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	if(m_lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
	{
		m_pWndCostText->SetTitle( prj.GetText( TID_MMI_REDCHIP ) );
		ViewItemCount(m_pWndEdit->GetString(), m_pItemElem->GetChipCost());
	}
	else
	{
		m_pWndCostText->SetTitle( prj.GetText( TID_MMI_PRICE ) );
		ViewItemCount(m_pWndEdit->GetString(), GetCostofOneItem() );
	}
	m_pWndEdit->SetFocus();
	return TRUE;
}

//------------------------------------------------------------------------------
void FLWndCart::InsertNumber( int nNumber )
{
	if( m_pWndEdit == NULL ) 
		return;

	//최대 4자리수 9999개까지 입력가능
	if( strlen( m_pWndEdit->GetString() ) < 4 )
	{
		CString strNumber = _T( "" );
		strNumber.Format( _T( "%d" ), nNumber );
		if(m_bBlockState)
		{
			//한번이라도 숫자입력했음
			m_pWndEdit->SetString( strNumber );
			m_bBlockState = FALSE;
		}
		else
		{
			m_pWndEdit->AddString( strNumber );
		}
		if(m_lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
		{
			ViewItemCount(m_pWndEdit->GetString(), m_pItemElem->GetChipCost());
		}
		else
		{
			ViewItemCount(m_pWndEdit->GetString(), GetCostofOneItem() );
		}
	}
}
//------------------------------------------------------------------------------
void FLWndCart::DeleteNumber_Init( void )
{
	//한꺼번에 다 지워줌
	if( m_pWndEdit )
		m_pWndEdit->Empty();
	ViewItemCount("0", 0);
}

//------------------------------------------------------------------------------
void FLWndCart::DeleteNumber_Back( void )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndEdit, _T("[윈도우 장바구니] 편집 창 포인터가 유효하지 않음") );
	
	//뒤에서 부터 한칸씩 지워줌
	CString strBack = m_pWndEdit->GetString();
	
	int nStrlen = strlen( m_pWndEdit->GetString() );
	
	if( nStrlen == 0 )
		return;
	strBack.Delete(nStrlen-1);
	m_pWndEdit->SetString(strBack);
	if(m_lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
	{
		ViewItemCount(m_pWndEdit->GetString(),m_pItemElem->GetChipCost());
	}
	else
	{
		ViewItemCount(m_pWndEdit->GetString(), GetCostofOneItem() );
	}
}
//------------------------------------------------------------------------------
void FLWndCart::ViewItemCount( const char* strString,int nItemCost)
{
	ULONGLONG   nItemCnt = _ttoi( strString );
	ULONGLONG   lCost = 0;
	ULONGLONG	lCost_Sum = 0;
	CString  strGetCost;

	lCost     = nItemCost;
	lCost_Sum = nItemCnt * lCost;

	//구입예상가격과 수량가격을 미리 계산해주는것 
	CString  strSaveCost = CFLWndShop_Cart::WndShop_Cart()->GetStaticCost()->GetTitle();
	ULONGLONG	ulSaveCost = _ttoi( strSaveCost );
	ULONGLONG	ulSum = ulSaveCost + lCost_Sum;
	if( ulSum > INT_MAX || ulSum < 0 )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_5) ) );	
		strGetCost.Format(_T("%d"),nItemCost);
		m_pWndItemCnt->AddWndStyle( WSS_MONEY );
		m_pWndEdit->SetString("1");
		m_pWndItemCnt->SetTitle(strGetCost);
		return;
	}
	if( lCost_Sum > INT_MAX || lCost_Sum  < 0 )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_5) ) );	
		strGetCost.Format(_T("%d"),nItemCost);
		m_pWndItemCnt->AddWndStyle( WSS_MONEY );
		m_pWndEdit->SetString("1");
		m_pWndItemCnt->SetTitle(strGetCost);
		return;
	}	
	strGetCost.Format(_T("%I64u"),lCost_Sum);
	m_pWndItemCnt->AddWndStyle( WSS_MONEY );
	m_pWndItemCnt->SetTitle(strGetCost);
}
//------------------------------------------------------------------------------
void  FLWndCart::OnBTOK()
{
	CRect rect = GetClientRect();
	CWorld* pWorld	= g_WorldMng.Get();
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_ET_AMOUNT );

	CString string = ""; 
	string = pWndEdit->GetString();
	//입력된 아이템 갯수 
	int nItemCnt = _ttoi( string );
	if(nItemCnt == 0)
	{
		CFLWndShop_Cart::WndShop_Cart()->m_pWndCart = NULL;
		Destroy();
		return;
	}

	std::vector<PT_T_SHOPCART> vecTem = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
	std::vector<PT_T_SHOPCART> vecInsert;
	int nSlot = nItemCnt/ m_pItemElem->m_nItemNum;	//필요한슬롯갯수
	int nTotalItemCount = nItemCnt;	
	int nSpITemCnt = nItemCnt;
	std::vector<PT_T_SHOPCART>::iterator itr;
	int nSlot_Temp = 0;
	int nCartCnt = CFLWndShop_Cart::WndShop_Cart()->GetCartCnt();// 쓰고있는 카트갯수 
	int nSameID = 0;
	CString str;
	if((nCartCnt + nSlot) == MAX_CART_SLOT)
	{
		if(nItemCnt > m_pItemElem->m_nItemNum)
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_4) ) );	
			return;
		}
	}
	//아이템 중복 검사
    if( (nCartCnt + nSlot) <= MAX_CART_SLOT )
	{
		if(vecTem.size() != 0)
		{
			//Slot에 하나라도 아이템이 있다!					
			for(nSameID = 0; nSameID <= nCartCnt; nSameID++)
			{
				if ((int)( vecTem.size() ) > nSameID)
				{
					//아이템이 같은게 있다면!
					if(vecTem[nSameID]->m_pItemElem->GetItemIndex() == m_pItemElem->GetItemIndex())
					{
						//갯수가 최대묶음값과 똑같다면 
						if(vecTem[nSameID]->m_nItemCnt == m_pItemElem->m_nItemNum)
						{
							continue;
						}
						else if(vecTem[nSameID]->m_nItemCnt < m_pItemElem->m_nItemNum )
						{
							//TestCode
							PT_T_SHOPCART pST[MAX_CART_SLOT] = {0,};
							pST[nSameID] =  vecTem[nSameID];

							//갯수가 최대묶음갯수보다 작다면
							int nResult = vecTem[nSameID]->m_nItemCnt + nItemCnt;
							if(nSameID+1 == MAX_CART_SLOT)
							{
								if(nResult > m_pItemElem->m_nItemNum)
								{
									nSlot_Temp = nResult / m_pItemElem->m_nItemNum;
									if(nSlot_Temp > 0)
									{
										//구입수량이 많아 장바구니의 여유 공간이 부족합니다.
										g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_2) ) );	
										return;
									}
								}							
							}
							// 현재 갯수와 입력갯수값이 최대 묶음갯수보다 적으면 그냥 넣어줌 
							if( nResult <= m_pItemElem->m_nItemNum )
							{
								vecTem[nSameID]->m_nItemCnt = nResult;	
							}
							else//99가 넘는다면!!
							{								
								//현재 갯수 - 최대 묶음값 = nResult_2으로
								int nResult_2 = m_pItemElem->m_nItemNum - vecTem[nSameID]->m_nItemCnt;
								vecTem[nSameID]->m_nItemCnt = m_pItemElem->m_nItemNum;// 현재 갯수에 최대 묶음 값 넣어주고
								int nResult_3 = nItemCnt - nResult_2; // 그 나머지의 값으로 
								int nResult_Slot = nResult_3/m_pItemElem->m_nItemNum;//slot을 다시 계산하고 갯수도 계산해서 넣어준다.
								for(int nS = nCartCnt; nS <= (nCartCnt + nResult_Slot); nS++)
								{
									PT_T_SHOPCART ptCart = new _T_SHOPCART;
									ptCart->m_nCartCnt = nS;
									ptCart->m_pItemElem = m_pItemElem;
									if(nResult_3 >= m_pItemElem->m_nItemNum)
									{
										ptCart->m_nItemCnt = m_pItemElem->m_nItemNum;
									}
									else 
									{
										ptCart->m_nItemCnt = nResult_3;
									}
									ptCart->m_nShoptab = CFLWndShop_Cart::WndShop_Cart()->GetWndShopList()->GetCurSel();
									vecInsert = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
									vecInsert.push_back(ptCart);
									CFLWndShop_Cart::WndShop_Cart()->SetVecCart(vecInsert);
									int nsize = CFLWndShop_Cart::WndShop_Cart()->GetVecCart().size();
									CFLWndShop_Cart::WndShop_Cart()->SetCartCnt(vecInsert.size());	
									nResult_3 = nResult_3 - m_pItemElem->m_nItemNum;
								}
							}
							nItemCnt = -1;
							continue;
						}
						else
						{
							PT_T_SHOPCART ptCart = new _T_SHOPCART;
							ptCart->m_nCartCnt = nSameID;
							ptCart->m_pItemElem = m_pItemElem;
							ptCart->m_nItemCnt = nItemCnt;
							ptCart->m_nShoptab = CFLWndShop_Cart::WndShop_Cart()->GetWndShopList()->GetCurSel();
							vecInsert = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
							vecInsert.push_back(ptCart);
							CFLWndShop_Cart::WndShop_Cart()->SetVecCart(vecInsert);
							int nsize = CFLWndShop_Cart::WndShop_Cart()->GetVecCart().size();
							CFLWndShop_Cart::WndShop_Cart()->SetCartCnt(vecInsert.size());	
							nItemCnt = -1;
							continue;
						}
						continue;
					}
				}
				continue;
			}
			if(nCartCnt < nSameID)
			{
				if(nItemCnt != -1)
				{
					//슬롯이 1개만 필요할때 
					if(nSlot == 0)
					{
						PT_T_SHOPCART ptCart = new _T_SHOPCART;
						ptCart->m_nCartCnt = nCartCnt;
						ptCart->m_pItemElem = m_pItemElem;
						ptCart->m_nItemCnt = nItemCnt;
						ptCart->m_nShoptab = CFLWndShop_Cart::WndShop_Cart()->GetWndShopList()->GetCurSel();
						vecInsert = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
						vecInsert.push_back(ptCart);
						CFLWndShop_Cart::WndShop_Cart()->SetVecCart(vecInsert);
						CFLWndShop_Cart::WndShop_Cart()->SetCartCnt(vecInsert.size());
					}
					else //Slot롯이 1개 이상이면 
					{
						//Slot0부터 필요 슬롯까지 
						for(int nSlotCnt = nCartCnt; nSlotCnt <= nSlot+nCartCnt; nSlotCnt++)
						{	
							PT_T_SHOPCART ptCart = new _T_SHOPCART;
							if(nTotalItemCount > 0)
							{
								if( nTotalItemCount >= m_pItemElem->m_nItemNum )
								{
									m_nSlotTemp[nSlotCnt] = m_pItemElem->m_nItemNum; 
								}
								else
								{
									m_nSlotTemp[nSlotCnt] = nTotalItemCount; 
								}
								nTotalItemCount = nTotalItemCount - m_pItemElem->m_nItemNum;
								ptCart->m_nCartCnt = nSlotCnt;
								ptCart->m_pItemElem = m_pItemElem;
								ptCart->m_nItemCnt = m_nSlotTemp[nSlotCnt];
								ptCart->m_nShoptab = CFLWndShop_Cart::WndShop_Cart()->GetWndShopList()->GetCurSel();
								vecInsert = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
								vecInsert.push_back(ptCart);
								CFLWndShop_Cart::WndShop_Cart()->SetVecCart(vecInsert);
								CFLWndShop_Cart::WndShop_Cart()->SetCartCnt(vecInsert.size());

								std::vector<PT_T_SHOPCART> vec = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();

								PT_T_SHOPCART p = vec[nSlotCnt];
							}						
						}
					}				
				}
			}
		}
		else
		{
			//슬롯이 1개만 필요할때 
			if(nSlot == 0)
			{
				PT_T_SHOPCART ptCart = new _T_SHOPCART;
				ptCart->m_nCartCnt = nSlot;
				ptCart->m_pItemElem = m_pItemElem;
				ptCart->m_nItemCnt = nItemCnt;
				ptCart->m_nShoptab = CFLWndShop_Cart::WndShop_Cart()->GetWndShopList()->GetCurSel();
				vecInsert = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
				vecInsert.push_back(ptCart);
				CFLWndShop_Cart::WndShop_Cart()->SetVecCart(vecInsert);
				CFLWndShop_Cart::WndShop_Cart()->SetCartCnt(vecInsert.size());
			}
			else //Slot롯이 1개 이상이면 
			{
				//Slot0부터 필요 슬롯까지 
				for(int nSlotCnt = 0; nSlotCnt <= nSlot; nSlotCnt++)
				{	
					PT_T_SHOPCART ptCart = new _T_SHOPCART;
					if(nTotalItemCount > 0)
					{
						if( nTotalItemCount >= m_pItemElem->m_nItemNum )
						{
							m_nSlotTemp[nSlotCnt] = m_pItemElem->m_nItemNum; 
						}
						else
						{
							m_nSlotTemp[nSlotCnt] = nTotalItemCount; 
						}
						nTotalItemCount = nTotalItemCount - m_pItemElem->m_nItemNum;
						ptCart->m_nCartCnt = nSlotCnt;
						ptCart->m_pItemElem = m_pItemElem;
						ptCart->m_nItemCnt = m_nSlotTemp[nSlotCnt];
						ptCart->m_nShoptab = CFLWndShop_Cart::WndShop_Cart()->GetWndShopList()->GetCurSel();
						vecInsert = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();
						vecInsert.push_back(ptCart);
						CFLWndShop_Cart::WndShop_Cart()->SetVecCart(vecInsert);
						CFLWndShop_Cart::WndShop_Cart()->SetCartCnt(vecInsert.size());

						std::vector<PT_T_SHOPCART> vec = CFLWndShop_Cart::WndShop_Cart()->GetVecCart();

						PT_T_SHOPCART p = vec[nSlotCnt];
					}						
				}
			}				
		}	
		ULONGLONG ulCost = 0;
		if(m_lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
		{
			ulCost = m_pItemElem->GetChipCost();
		}
		else 
		{
			ulCost = GetCostofOneItem();
		}
		if( m_pItemElem )
		{
			m_ulCostSum = ulCost * nSpITemCnt;
		}

		//금액
		CString strCost = CFLWndShop_Cart::WndShop_Cart()->GetStaticCost()->GetTitle();
		ULONGLONG ulOldCost = _ttoi( strCost );
		ulOldCost += m_ulCostSum;
		strCost.Format("%I64u",ulOldCost);
		CFLWndShop_Cart::WndShop_Cart()->GetStaticCost()->SetTitle(strCost);
	}
	else
	{
		//구입수량이 많아 장바구니의 여유 공간이 부족합니다.
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_CART_ERROR_MSG_2) ) );	
	} 		
	CFLWndShop_Cart::WndShop_Cart()->m_pWndCart = NULL;
	Destroy();
}

//------------------------------------------------------------------------------
BOOL FLWndCart::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	//APP_SHOPBASKET에서 진행되는Control
	switch(nID)
	{
	case WIDC_ET_AMOUNT:
		switch(message)
		{
		case EN_CHANGE:
			{
				m_pWndEdit->SetMaxStringNumber(4);
				if(m_lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
				{
					ViewItemCount(m_pWndEdit->GetString(), m_pItemElem->GetChipCost());
				}
				else
				{
					ViewItemCount(m_pWndEdit->GetString(), GetCostofOneItem() );
				}
				//0만입력된경우 0하나만 입력하게함
				if(!strcmp(m_pWndEdit->GetString(),"00"))
				{
					m_pWndEdit->SetString("0");
				}
			}
			break;
		case EN_RETURN:
            OnBTOK();
			break;	
		}
		break;
	case WIDC_BT_1:
		InsertNumber(1);
		break;
	case WIDC_BT_2:
		InsertNumber(2);
		break;
	case WIDC_BT_3:
		InsertNumber(3);
		break;
	case WIDC_BT_4:
		InsertNumber(4);
		break;
	case WIDC_BT_5:
		InsertNumber(5);
		break;
	case WIDC_BT_6:
		InsertNumber(6);
		break;
	case WIDC_BT_7:
		InsertNumber(7);
		break;
	case WIDC_BT_8:
		InsertNumber(8);
		break;
	case WIDC_BT_9:
		InsertNumber(9);
		break;
	case WIDC_BT_0:
		InsertNumber(0);
		break;
	case WIDC_BT_CAL:
		DeleteNumber_Init();
		break;
	case WIDC_BT_BACK:
		DeleteNumber_Back();
		break;
	case WIDC_OK: 
		{
			OnBTOK();
		}
		break;
	case WIDC_CANCEL:
		CFLWndShop_Cart::WndShop_Cart()->m_pWndCart = NULL;
		Destroy();
		break;
	case WTBID_CLOSE:
		CFLWndShop_Cart::WndShop_Cart()->m_pWndCart = NULL;
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 