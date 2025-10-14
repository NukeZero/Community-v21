#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndManager.h"

#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	if( nID == WIDC_OK )
	{
		SAFE_DELETE( ((CWndShop*)m_pParentWnd)->m_pWndConfirmSell );
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell = new CWndConfirmSell;
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->m_pItemElem = m_pItemElem;
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->m_pMover = m_pMover;
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->Initialize( m_pParentWnd, APP_CONFIRM_SELL );
		Destroy();
	}
	if( nID == WIDC_CANCEL )
	{
		Destroy();
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
			_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		for( int i = 0 ; i < 8 ; i++ )
		{
			char szNumberbuf[8] = {0, };
			strncpy( szNumberbuf, szNumber, 8 );
			
			// 0 : 공백, 48 : 숫자 0, 57 : 숫자 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = m_pItemElem->m_nItemNum;
					_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
					m_pEdit->SetString( szNumberbuf );
					break;												
				}
			}
		}
	}
	DWORD dwCost = m_pItemElem->GetCost();
	DWORD BuyGold = dwCost / 4;
	TCHAR szNumberGold[ 64 ];
#ifdef __NPP_050308
	KarmaProp* pProp = prj.GetKarmaProp( g_pPlayer->m_nSlaughter );
	if( pProp && pProp->fSellPenaltyRate != 0 )
		BuyGold *= pProp->fSellPenaltyRate;

	if( BuyGold < 1 )
		BuyGold = 1;
#endif // __NPP_050308
	BuyGold *= nNumber;
	_itot( BuyGold, szNumberGold, 10 );
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
		_itot( m_pItemElem->m_nItemNum, szNumber, 10 );
		m_pEdit->SetString( szNumber );
	}
	DWORD dwCost = m_pItemElem->GetCost();
	DWORD BuyGold = dwCost / 4;
#ifdef __NPP_050308
	KarmaProp* pProp = prj.GetKarmaProp( g_pPlayer->m_nSlaughter );
	if( pProp && pProp->fSellPenaltyRate != 0 )
		BuyGold *= pProp->fSellPenaltyRate;

	if( BuyGold < 1 )
		BuyGold = 1;
#endif // __NPP_050308
	BuyGold *= m_pItemElem->m_nItemNum;
	TCHAR szNumberGold[ 64 ];
	_itot( BuyGold, szNumberGold, 10 );
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
#ifndef __ONLINE
		int nCost = m_pItemElem->GetCost();
		if( g_pPlayer->GetGold() + nCost > g_pPlayer->GetGold() )
		{
			g_pPlayer->AddGold( nCost );
			PLAYSND( SND_INF_TRADE );
			g_pPlayer->RemoveItemId( 0, m_pItemElem->m_dwObjId );
		}
		else
			TRACE( _T( "헉.. 판매해서 돈을 추가했는데, 돈을 추가 못했다. 아이템 가격 설정 값이 마이너스인 것 같다." ) );
#else	// __ONLINE
		PLAYSND( SND_INF_TRADE );

		int SellNum = 0;
		if( m_pItemElem->m_nItemNum >= 1 )
		{
			SellNum = atoi( m_pEdit->GetString() );
		}
		if( SellNum != 0 )
		{
			g_DPlay.SendSellItem( m_pItemElem->m_dwObjId, SellNum );
		}
#endif	// __ONLINE
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
	TCHAR szString[ 64 ];

	if( dwBuy != atoi(m_pEdit->GetString()) )
	{
		_itot( dwBuy, szString, 10 );	// integer to string
		m_pEdit->SetString( szString );
	}

	DWORD dwTotalBuy = 0;
#ifdef __NPP_050308		
	KarmaProp* pProp = prj.GetKarmaProp( g_pPlayer->m_nSlaughter );
	if( pProp )
	{
		dwTotalBuy = m_pItemElem->GetCost() * pProp->fDiscountRate;
		if( dwTotalBuy <= 0 )
			dwTotalBuy = 1;
		dwTotalBuy = dwBuy * dwTotalBuy * prj.m_fShopCost;
		_itot( dwTotalBuy, szString, 10 );	// integer to string
	}
#else // __NPP_050308
	_itot( dwBuy * m_pItemElem->GetCost() * prj.m_fShopCost, szString, 10 );	// integer to string
#endif // __NPP_050308
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
	
	DWORD dwCost = m_pItemElem->GetCost();
	if( dwCost == 0 )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) ); // "다른 사용자에게 팔렸습니다."
		Destroy();
		return TRUE;
	}

#ifdef __BUYITEM_METHOD_CHANGED0603

	OnChangeBuyCount( 1 );
	return TRUE;

#else
	int iBuyNymber = g_pPlayer->GetGold() / dwCost;
	if( iBuyNymber > m_pItemElem->m_nItemNum )
	{
		iBuyNymber = m_pItemElem->m_nItemNum;
	}
	
	if( m_pItemElem->m_nItemNum == 1 )
	{
		if( iBuyNymber == 0 )
		{
			m_pEdit->SetString( "0" );
		}
		else
		{
			m_pEdit->SetString( "1" );
		}		
	}
	else
	{
		TCHAR szNumber[ 64 ];
		_itot( iBuyNymber, szNumber, 10 );
		m_pEdit->SetString( szNumber );
	}
	
	DWORD BuyGold = iBuyNymber * dwCost;
	TCHAR szNumberGold[ 64 ];
	_itot( BuyGold, szNumberGold, 10 );
	m_pStaticGold->SetTitle( szNumberGold );
	return TRUE;
#endif
}

void CWndConfirmBuy::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwCost = m_pItemElem->GetCost();
	if( dwCost == 0 )
	{
		g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) );  // 다른 사용자에게 팔렸습니다.
		Destroy();
		return;
	}

#ifdef __BUYITEM_METHOD_CHANGED0603
	return;
#else

	LPCTSTR szNumber = m_pEdit->GetString();
	int nNumber = atoi( szNumber );

	int iBuyNymber = g_pPlayer->GetGold() / dwCost;
	if( iBuyNymber > m_pItemElem->m_nItemNum )
	{
		iBuyNymber = m_pItemElem->m_nItemNum;
	}
	
	if( m_pItemElem->m_nItemNum == 1 )
	{
		if( iBuyNymber == 0 )
		{
			m_pEdit->SetString( "0" );
		}
		else
		{
			m_pEdit->SetString( "1" );
		}		
	}
	else
	{
		char szNumberbuf[8] = {0, };
		
		if( iBuyNymber < nNumber )
		{
			nNumber = iBuyNymber;
			_itoa( iBuyNymber, szNumberbuf, 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		
		for( int i = 0 ; i < 8 ; i++ )
		{
			strncpy( szNumberbuf, szNumber, 8 );
			
			// 0 : 공백, 48 : 숫자 0, 57 : 숫자 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = iBuyNymber;
					_itoa( iBuyNymber, szNumberbuf, 10 );
					m_pEdit->SetString( szNumberbuf );
					break;
				}
			}
		}
	}

	DWORD BuyGold = nNumber * dwCost;
	TCHAR szNumberGold[ 64 ];
	_itot( BuyGold, szNumberGold, 10 );
	m_pStaticGold->SetTitle( szNumberGold );
#endif		
} 

const int MAX_BUY_ITEMCOUNT = 99;

BOOL CWndConfirmBuy::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	int nBuyNum = 0;

	switch( nID )
	{
#ifdef __BUYITEM_METHOD_CHANGED0603
	case WIDC_PLUS:
		nBuyNum = atoi(m_pEdit->GetString());
		++nBuyNum;
		if( m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW )
		{
			if ( nBuyNum > 1000 )
				nBuyNum = 1000;
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
			if( m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW )
				OnChangeBuyCount( 1000 );
			else
				OnChangeBuyCount( MAX_BUY_ITEMCOUNT );
		}		
		break;

	case WIDC_CONTROL2:
		if( EN_CHANGE == message )
		{
			if( m_pEdit == NULL )
			{
				ADDERRORMSG( "CWndConfirmBuy::OnChildNotify : m_pEdit == NULL" );
				char szMsg[256];
				sprintf( szMsg, "CWndConfirmBuy::OnChildNotify : more info(%d, %d)", pLResult, GetDlgItem( WIDC_CONTROL2 ) );
				ADDERRORMSG( szMsg );

				nBuyNum = 1;
			}
			else
				nBuyNum = atoi(m_pEdit->GetString());

			nBuyNum = max( nBuyNum, 0 );
			DWORD dwMAXCount = MAX_BUY_ITEMCOUNT;
			if( m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW )
				dwMAXCount = 1000;
			nBuyNum = min( nBuyNum, dwMAXCount );

			OnChangeBuyCount(nBuyNum);
		}
		break;

#endif // __BUYITEM_METHOD_CHANGED0603

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

#ifdef __ONLINE
	// ONLINE인 경우 
	#ifdef __BUYITEM_METHOD_CHANGED0603
		void CWndConfirmBuy::OnOK()
		{
			DWORD dwCost = m_pItemElem->GetCost();
			if( m_pItemElem->m_nItemNum < 1 || dwCost == 0 )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0006) ) );	// 다른 사용자에게 팔렸습니다.
				return;
			}

			int nBuy = atoi( m_pEdit->GetString() );
			if( (nBuy * dwCost) > g_pPlayer->GetGold() )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0009) ) );	// 돈이 부족합니다.	
				return;
			}

			DWORD dwMAXCount = MAX_BUY_ITEMCOUNT;
			if( m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW )
				dwMAXCount = 1000;

			if( nBuy < 1 || nBuy > dwMAXCount )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0086) ) );	// 상점 거래 중 구입 / 판매 개수가 1 ~ 99 사이가 아닌 수를 입력	
				return;
			}

			if( nBuy > m_pItemElem->m_nItemNum )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0087) ) );	// 개인 상점 거래 중 구입 하려는 개수가 상점에 판매하는 개수 보다 많거나, 이미 품절	
				return;
			}

			CWndShop* pWndShop = (CWndShop*)GetWndBase( APP_SHOP_ );

			CWndTabCtrl* pTabCtrl	= (CWndTabCtrl*)pWndShop->GetDlgItem( WIDC_INVENTORY );
			CHAR cTab	= (CHAR)pTabCtrl->GetCurSel();
			g_DPlay.SendBuyItem( cTab, m_pItemElem->m_dwObjId, nBuy, m_dwItemId );	

			PLAYSND( SND_INF_TRADE );
		}
	#else // __BUYITEM_METHOD_CHANGED0603
		void CWndConfirmBuy::OnOK()
		{
			CWndShop* pWndShop = (CWndShop*)GetWndBase( APP_SHOP_ );

			int nBuyNum = 0;
			if( m_pItemElem->m_nItemNum >= 1 )
				nBuyNum = atoi( m_pEdit->GetString() );

			if( nBuyNum != 0 )
			{
				CWndTabCtrl* pTabCtrl	= (CWndTabCtrl*)pWndShop->GetDlgItem( WIDC_INVENTORY );
				CHAR cTab	= (CHAR)pTabCtrl->GetCurSel();
		//				g_DPlay.SendBuyItem( cTab, m_pItemElem->m_dwObjId, nBuyNum, m_pItemElem->m_dwItemId );
				g_DPlay.SendBuyItem( cTab, m_pItemElem->m_dwObjId, nBuyNum, m_dwItemId );	// 2004/03/12
				PLAYSND( SND_INF_TRADE );
			}			
		}
	#endif // __BUYITEM_METHOD_CHANGED0603
#else	
	// OFFLINE이 경우 
	void CWndConfirmBuy::OnOK()
	{
	}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndItemCtrlVendor::CWndItemCtrlVendor()
{
#ifdef __BUYITEM_METHOD_CHANGED0603
	m_bVisibleCount = FALSE;
#endif
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
#ifdef __Y0402
CWndShop::CWndShop() 
{ 
	m_pMover = NULL;
	m_pWndConfirmSell = NULL;
#ifdef __SELLITEM1013
	m_pWndWarning	= NULL;
#endif	// __SELLITEM1013
	m_bSexSort    = FALSE;
	m_bLevelSort  = FALSE;
} 
CWndShop::~CWndShop() 
{ 
	SAFE_DELETE( m_pWndConfirmSell );
#ifdef __SELLITEM1013
	SAFE_DELETE( m_pWndWarning );
#endif	// __SELLITEM1013
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
//		CItemBase* pItemBase	= pItemCtrl->m_pArrayItemBase[nIndex];
		CItemBase* pItemBase	= pItemCtrl->GetItemFromArr( nIndex );
		if( pItemBase )
			dwCost	+= pItemBase->GetCost()/* * ( (CItemElem*)pItemBase )->m_nItemNum*/;
	}

	dwCost = (int)((float)dwCost * prj.m_fShopCost );

#ifdef __NPP_050308
	KarmaProp* pProp = prj.GetKarmaProp( g_pPlayer->m_nSlaughter );
	if( pProp )
	{
		dwCost = (int)( dwCost * pProp->fDiscountRate );
		if( dwCost <= 0 )
			dwCost = 1;
	}
#endif // __NPP_050308
	string.Format( _T( "%d" ), dwCost );

	pCost->SetTitle( string );
} 
void CWndShop::OnInitialUpdate() 
{ 
//	LPWNDCTRL lpWndCtrl = m_resMng.GetAtControl ( GetWndId(), WIDC_CONTROL0 );
//	m_static.Create( _T( "Test" ), 0, lpWndCtrl->rect, this, WIDC_CONTROL0 );
	CWndNeuz::OnInitialUpdate(); 

#ifdef __BUGFIX_CLONE0607
	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndBase(APP_VENDOREX)  )
	{
		Destroy();
		return;
	}
#endif	// __BUGFIX_CLONE0607

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
		for( i = pTabCtrl->GetSize(); i < 3; i++ )
		{
			tabTabItem.pszText = "";
			tabTabItem.pWndBase = NULL;
			pTabCtrl->InsertItem( i, &tabTabItem );
		}
	}
	
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );

#ifdef __Y0402
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
#else // __Y0402
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( rectInventory.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	Move( ptMove );
#endif // __Y0402
} 

BOOL CWndShop::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_SHOP_, 0, 0, pWndParent );
} 

BOOL CWndShop::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndShop::OnSize( UINT nType, int cx, int cy ) \
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
#ifdef __SELLITEM1013
	BOOL	bWarning	= FALSE;
#endif	// __SELLITEM1013

	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		
		BOOL bForbid = TRUE;
		if( lpShortcut->m_dwType == ITYPE_ITEM && lpShortcut->m_dwData != 0 )
		{
			if( nID == 10 || nID == 11 || nID == 12 || nID == 13) // item
			{
				BOOL bResult = TRUE;
#ifdef __S050504_QUESTITEM
				if( ( (CItemElem*)lpShortcut->m_dwData )->IsQuest() )
				{
					bResult = FALSE;
				}
#endif // __S050504_QUESTITEM

				if( bResult && pWndFrame->GetWndId() == APP_INVENTORY )
				{
					CItemElem *pItemElem = (CItemElem*)lpShortcut->m_dwData;
					if( pItemElem )
					{
						ItemProp *pProp = pItemElem->GetProp();
						if( pProp )
						{
							if( pItemElem->IsCharged() )
							{
#ifdef __FINITEITEM0705
								if( !pItemElem->m_dwKeepTime )
#endif	// __FINITEITEM0705
								{
#ifdef __SELLITEM1013
									bWarning	= TRUE;
#else	// __SELLITEM1013
									g_WndMng.PutString( prj.GetText( TID_GAME_NOTTRADE ), NULL, prj.GetTextColor( TID_GAME_NOTTRADE ) );
									return FALSE;
#endif	// __SELLITEM1013
								}
							}
						} else
						{
							LPCTSTR szErr = Message( "CWndShop::OnChildNotify : pProp==NULL %d", pItemElem->m_dwItemId );
							ADDERRORMSG( szErr );
						}
					} else
					{
						LPCTSTR szErr = Message( "CWndShop::OnChildNotify : pItemElem==NULL %d", pItemElem->m_dwItemId );
						ADDERRORMSG( szErr );
					}

					if( FALSE == g_pPlayer->m_Inventory.IsEquip( ( (CItemElem*)lpShortcut->m_dwData)->m_dwObjId ) )
					{
#ifdef __SELLITEM1013
						if( bWarning )
						{
							SAFE_DELETE( m_pWndWarning );
							SAFE_DELETE( m_pWndConfirmSell );
							m_pWndWarning	= new CWndWarning;
							m_pWndWarning->m_pItemElem	= (CItemElem*)lpShortcut->m_dwData;
							m_pWndWarning->m_pMover = m_pMover;
							m_pWndWarning->Initialize( this, APP_WARNING );
						}
						else
						{
							SAFE_DELETE( m_pWndWarning );
							SAFE_DELETE( m_pWndConfirmSell );
							m_pWndConfirmSell = new CWndConfirmSell;
							m_pWndConfirmSell->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmSell->m_pMover = m_pMover;
							m_pWndConfirmSell->Initialize( this, APP_CONFIRM_SELL );
						}
#else	// __SELLITEM1013
						SAFE_DELETE( m_pWndConfirmSell );
						m_pWndConfirmSell = new CWndConfirmSell;
						m_pWndConfirmSell->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
						m_pWndConfirmSell->m_pMover = m_pMover;
						m_pWndConfirmSell->Initialize( this, APP_CONFIRM_SELL );
#endif	// __SELLITEM1013
						bForbid = FALSE;
					}
					else
					{
						g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPTRADE), NULL, prj.GetTextColor(TID_GAME_EQUIPTRADE) );
//						 g_WndMng.PutString( "장착된아이템은 팔수 없습니다.", NULL, 0xffff0000 );
					}
				}
			}
		}
		SetForbid( bForbid );
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
#ifdef __ONLINE
	g_pPlayer->m_vtInfo.SetOther( NULL );
	g_DPlay.SendCloseShopWnd();
#endif	// __ONLINE
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
	if( pWndInventory )
	{
		SAFE_DELETE( pWndInventory->m_pWndConfirmBuy );
	}
	SAFE_DELETE( m_pWndConfirmSell );
}

#else // __Y0402
CWndShop::CWndShop() 
{ 
	m_pMover = NULL;
	m_pWndConfirmSell = NULL;
} 
CWndShop::~CWndShop() 
{ 
	SAFE_DELETE( m_pWndConfirmSell );
} 
void CWndShop::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCHARACTER lpCharacter = m_pMover->GetCharacter();
	if( lpCharacter == NULL)
		return;

	CString string;
	//CWndStatic* pTotalNum = (CWndStatic*) GetDlgItem( WIDC_TOTAL_NUM );
	CWndStatic* pCost = (CWndStatic*) GetDlgItem( WIDC_COST );

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_INVENTORY );
	// 갯수 계산 
	//WTCITEM item;
	//pTabCtrl->GetItem( pTabCtrl->GetCurSel(), &item );
	//CWndItemCtrl* pItemCtrl = (CWndItemCtrl*)item.pWndBase;
	//string.Format( _T( "%d" ), pItemCtrl->GetSelectedCount()  );
	//pTotalNum->SetTitle( string );
	// 가격 합산 계산 
	//CItemBase* pItemBase;
	//DWORD dwCost = 0;
	//for( int i = 0; i < pItemCtrl->GetSelectedCount(); i++ )
	//{
	//	pItemBase = pItemCtrl->GetItem( i );
	//	if( pItemBase )
	//		dwCost += pItemBase->GetProp()->dwCost;
	//}
//	string.Format( _T( "%d" ), 100 );//dwCost  );
//	pCost->SetTitle( string );

	WTCITEM item;
	pTabCtrl->GetItem( pTabCtrl->GetCurSel(), &item );
	CWndItemCtrl* pItemCtrl		= (CWndItemCtrl*)item.pWndBase;
	DWORD dwCost	= 0;
	int nIndex	= -1;
	if( pItemCtrl->GetSelectedCount() > 0 )
		nIndex	= pItemCtrl->GetSelectedItem( 0 );
	if( nIndex >= 0 )
	{
		CItemBase* pItemBase	= pItemCtrl->GetItem( nIndex );
		if( pItemBase )
			dwCost	+= pItemBase->GetCost()/* * ( (CItemElem*)pItemBase )->m_nItemNum*/;
	}
	else {
	}

	string.Format( _T( "%d" ), dwCost );
	pCost->SetTitle( string );
} 
void CWndShop::OnInitialUpdate() 
{ 
//	LPWNDCTRL lpWndCtrl = m_resMng.GetAtControl ( GetWndId(), WIDC_CONTROL0 );
//	m_static.Create( _T( "Test" ), 0, lpWndCtrl->rect, this, WIDC_CONTROL0 );
	CWndNeuz::OnInitialUpdate(); 

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
		for( i = pTabCtrl->GetSize(); i < 3; i++ )
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
	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( rectInventory.Width(), 0 );
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
void CWndShop::OnSize( UINT nType, int cx, int cy ) \
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
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		
		BOOL bForbid = TRUE;
		if( lpShortcut->m_dwType == ITYPE_ITEM && lpShortcut->m_dwData != 0 )
		{
			if( nID == 10 || nID == 11 || nID == 12 || nID == 13) // item
			{
				BOOL bResult = TRUE;

#ifdef __S050504_QUESTITEM
				if( ( (CItemElem*)lpShortcut->m_dwData )->IsQuest() )
				{
					bResult = FALSE;
				}
#endif // __S050504_QUESTITEM

				if( bResult && pWndFrame->GetWndId() == APP_INVENTORY )
				{
					if( FALSE == g_pPlayer->m_Inventory.IsEquip( ( (CItemElem*)lpShortcut->m_dwData)->m_dwObjId ) )
					{
						/*
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
						for( int i = 0; i < pWndItemCtrl->GetSelectedCount(); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						*/
						SAFE_DELETE( m_pWndConfirmSell );
						m_pWndConfirmSell = new CWndConfirmSell;
						m_pWndConfirmSell->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
						m_pWndConfirmSell->m_pMover = m_pMover;
						m_pWndConfirmSell->Initialize( this, APP_CONFIRM_SELL );
						bForbid = FALSE;
					}
					else
					{
						g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPTRADE), NULL, prj.GetTextColor(TID_GAME_EQUIPTRADE) );
//						 g_WndMng.PutString( "장착된아이템은 팔수 없습니다.", NULL, 0xffff0000 );
					}
				}
			}
		}
		SetForbid( bForbid );
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
#ifdef __ONLINE
	g_pPlayer->m_vtInfo.SetOther( NULL );
	g_DPlay.SendCloseShopWnd();
#endif	// __ONLINE
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.GetWndBase( APP_INVENTORY );
	if( pWndInventory )
	{
		SAFE_DELETE( pWndInventory->m_pWndConfirmBuy );
	}
	SAFE_DELETE( m_pWndConfirmSell );
}
#endif // __Y0402

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
	memset( m_ColorRect, 0, sizeof(CRect)*3 );
	memset( m_fColor, 0, sizeof(FLOAT)*3 );
	m_bLButtonClick   = FALSE;
	m_nHairCost       = 0;
	m_nHairColorCostR  = 0;
	m_nHairColorCostG  = 0;
	m_nHairColorCostB  = 0;
} 

CWndBeautyShop::~CWndBeautyShop() 
{ 
//	m_Texture.DeleteDeviceObjects();
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pWndConfirmSell );
} 

HRESULT CWndBeautyShop::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_Texture.Invalidate();
#endif //__YDEBUG
	
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
#ifdef __YDEBUG
	m_Texture.SetInvalidate(m_pApp->m_pd3dDevice);
#endif //__YDEBUG

	CWndBase::RestoreDeviceObjects();

	return S_OK;
}

void CWndBeautyShop::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_pPlayer == NULL || m_pModel == NULL )
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
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
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
	
/*		
#ifdef __YPARTS_EX
		// 외투의상을 입었다면 주방어구 의상은 숨긴다...	
		int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
		int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
		
		O3D_ELEMENT *pElem1 = NULL;
		O3D_ELEMENT *pElem2 = NULL;
		
		for( int nArry=0; nArry<5; nArry++ )
		{
			pElem1 = m_pModel->GetParts( nArryEquip1[nArry] );
			pElem2 = m_pModel->GetParts( nArryEquip2[nArry] );
			
			if( pElem2 )
			{
				if( pElem1 )
					pElem1->m_nEffect |= XE_HIDE;
			}
		}
#endif //__YPARTS_EX	
*/		
	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(m_pModel);
		
	#ifdef __YHAIR_HIDE
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );
		if( pItemElem )
		{
			O3D_ELEMENT* pElement = NULL;
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = m_pModel->GetParts(pItemProp->dwBasePartsIgnore);
				
				//if( pElement )
				//	pElement->m_nEffect |= XE_HIDE;
			}

		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{					
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
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
		#endif //__YPARTS_EX										
		}
		else
		{
		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE);
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		#endif //__YPARTS_EX							
		}

	#endif //__YHAIR_HIDE
		
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
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matWorld);

		D3DXMatrixRotationY(&matRot,timeGetTime()/1000.0f);
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

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = m_fColor[0];
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = m_fColor[1];
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = m_fColor[2];

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );

	#ifdef __YHAIR_HIDE
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );
		if( pItemElem )
		{
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
					m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
				
				m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
			}
		}
		else
		{
			pItemElem	= g_pPlayer->GetEquipItem( PARTS_HAT );
			if( pItemElem )
			{
				if( !pItemElem->IsFlag( CItemElem::expired ) )
				{		
					ItemProp* pItemProp = pItemElem->GetProp();
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );

						m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}				
		}
	#endif //__YHAIR_HIDE
		
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = 1.0f;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = 1.0f;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = 1.0f;
	}

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

	BYTE nColorR = (m_fColor[0] * 255);
	BYTE nColorG = (m_fColor[1] * 255);
	BYTE nColorB = (m_fColor[2] * 255);

	BYTE nOrignalR = g_pPlayer->m_fHairColorR * 255;
	BYTE nOrignalG = g_pPlayer->m_fHairColorG * 255;
	BYTE nOrignalB = g_pPlayer->m_fHairColorB * 255;

	if( nColorR >= nOrignalR  )
		m_nHairColorCostR = (nColorR - nOrignalR)*13;
	else
		m_nHairColorCostR = (nOrignalR - nColorR)*7;

	if( nColorG >= nOrignalG  )
		m_nHairColorCostG = (nColorG - nOrignalG)*13;
	else
		m_nHairColorCostG = (nOrignalG - nColorG)*7;

	if( nColorB >= nOrignalB  )
		m_nHairColorCostB = (nColorB - nOrignalB)*13;
	else
		m_nHairColorCostB = (nOrignalB - nColorB)*7;
	
	CString string;
	CWndStatic* pCost = (CWndStatic*) GetDlgItem( WIDC_COST );
	string.Format( _T( "%d" ), m_nHairCost + m_nHairColorCostR + m_nHairColorCostG + m_nHairColorCostB );
	pCost->SetTitle( string );

	pCost = (CWndStatic*) GetDlgItem( WIDC_STATIC1 );
	string.Format( _T( "%d" ), m_nHairCost );
	pCost->SetTitle( string );

	pCost = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	string.Format( _T( "%d" ), m_nHairColorCostR + m_nHairColorCostG + m_nHairColorCostB );
	pCost->SetTitle( string );
	
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
} 

BOOL CWndBeautyShop::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	if( g_pPlayer == NULL )
		return FALSE;

	m_bLButtonClick   = FALSE;
	m_dwHairMesh      = g_pPlayer->m_dwHairMesh+1;

	SAFE_DELETE( m_pModel );
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND2 );
#ifdef __FINITEITEM0705
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
#else	// __FINITEITEM0705
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_adwEquipment, m_pModel, &g_pPlayer->m_Inventory );
#endif	// __FINITEITEM0705
	
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );

	///
	m_fColor[0] = g_pPlayer->m_fHairColorR;
	m_fColor[1] = g_pPlayer->m_fHairColorG;
	m_fColor[2] = g_pPlayer->m_fHairColorB;
	
	///
	m_ColorRect[0].left   = 44;
	m_ColorRect[0].top    = 251;
	m_ColorRect[0].right  = 162;
	m_ColorRect[0].bottom = 267;
	
	m_ColorRect[1].left   = 44;
	m_ColorRect[1].top    = 272;
	m_ColorRect[1].right  = 162;
	m_ColorRect[1].bottom = 287;

	m_ColorRect[2].left   = 44;
	m_ColorRect[2].top    = 293;
	m_ColorRect[2].right  = 162;
	m_ColorRect[2].bottom = 307;

	m_nHairCost   = 0;
	m_nHairColorCostR  = 0;
	m_nHairColorCostG  = 0;
	m_nHairColorCostB  = 0;

	ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );

	m_OriginalColorScrollBar[0] = m_ColorScrollBar[0];
	m_OriginalColorScrollBar[1] = m_ColorScrollBar[1];
	m_OriginalColorScrollBar[2] = m_ColorScrollBar[2];
	
	m_Texture.LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_THEME, "yellowbuttten.tga" ), 0xffff00ff, TRUE );

	return InitDialog( g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP, 0, 0, pWndParent );
} 

void CWndBeautyShop::ReSetBar( FLOAT r, FLOAT g, FLOAT b )
{
	FLOAT fR = ((r-0.3f)/(1.0f - 0.3f)) * 100.0f;
	FLOAT fG = ((g-0.3f)/(1.0f - 0.3f)) * 100.0f;
	FLOAT fB = ((b-0.3f)/(1.0f - 0.3f)) * 100.0f;

	m_ColorScrollBar[0].x = (((m_ColorRect[0].right-m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left;
	m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	m_ColorScrollBar[1].x = (((m_ColorRect[1].right-m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left;
	m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	m_ColorScrollBar[2].x = (((m_ColorRect[2].right-m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left;
	m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
}

void CWndBeautyShop::OnMouseWndSurface( CPoint point )
{
	if( g_pPlayer == NULL )
		return;
	
	CRect rect = CRect( 22, 198, 186, 298 );

	if( !rect.PtInRect( point ) )
		m_bLButtonClick = FALSE;

	for( int i=0; i<3; i++ )
	{
		CRect DrawRect = m_ColorRect[i];
		
		DrawRect.top    -= 22;
		DrawRect.bottom -= 22;
		
		if( DrawRect.PtInRect( point ) && m_bLButtonClick )
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
			
			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos   = point.x - DrawRect.left;
			
			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));

			D3DXVECTOR2 vec1= D3DXVECTOR2( 0.3f, 1.0f );
			D3DXVECTOR2 vec2= D3DXVECTOR2( 1.0f, 1.0f );
			D3DXVECTOR2 vec3;

			D3DXVec2Lerp( &vec3, &vec1, &vec2, p );
			
			m_fColor[i] = vec3.x;
			
			m_ColorScrollBar[i].x = point.x;
		}
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
	m_bLButtonClick = FALSE;
} 

void CWndBeautyShop::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
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
					//m_pModel->DeleteDeviceObjects();

					m_dwHairMesh      = g_pPlayer->m_dwHairMesh+1;
#ifdef __FINITEITEM0705
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
#else	// __FINITEITEM0705
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_adwEquipment, m_pModel, &g_pPlayer->m_Inventory );
#endif	// __FINITEITEM0705
					
					m_fColor[0] = g_pPlayer->m_fHairColorR;
					m_fColor[1] = g_pPlayer->m_fHairColorG;
					m_fColor[2] = g_pPlayer->m_fHairColorB;

					m_nHairCost = 0;
					
					ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );
				}
				break;
			case WIDC_HAIRSTYLE_LEFT: // hair
				{
					m_dwHairMesh--;

					( m_dwHairMesh < 1 ) ? m_dwHairMesh = MAX_HAIR: m_dwHairMesh;
#ifdef __FINITEITEM0705
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
#else	// __FINITEITEM0705
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_adwEquipment, m_pModel, &g_pPlayer->m_Inventory );
#endif	// __FINITEITEM0705
					
					// 임시...
					if( g_pPlayer->m_dwHairMesh != m_dwHairMesh-1 )
					{
						switch( m_dwHairMesh )
						{
							case 1:
								m_nHairCost = 2500;
								break;
							case 2:
								m_nHairCost = 2500;
								break;
							case 3:
								m_nHairCost = 2500;
								break;
							case 4:
								m_nHairCost = 2500;
								break;
							case 5:
								m_nHairCost = 2500;
								break;
							default:
								m_nHairCost = 4000;
								break;
						}
					}
					else
						m_nHairCost = 0;
				}
				break;
			case WIDC_HAIRSTYLE_RIGHT: // hair
				{
					m_dwHairMesh++;

					( m_dwHairMesh > MAX_HAIR ) ? m_dwHairMesh = 1: m_dwHairMesh;
#ifdef __FINITEITEM0705
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
#else	// __FINITEITEM0705
					CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, m_dwHairMesh-1, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_adwEquipment, m_pModel, &g_pPlayer->m_Inventory );
#endif	// __FINITEITEM0705
					
					// 임시...
					if( g_pPlayer->m_dwHairMesh != m_dwHairMesh-1 )
					{
						switch( m_dwHairMesh )
						{
						case 1:
							m_nHairCost = 2500;
							break;
						case 2:
							m_nHairCost = 2500;
							break;
						case 3:
							m_nHairCost = 2500;
							break;
						case 4:
							m_nHairCost = 2500;
							break;
						case 5:
							m_nHairCost = 2500;
							break;
						default:
							m_nHairCost = 4000;
							break;
						}
					}
					else
						m_nHairCost = 0;
				}
				break;
			case WIDC_OK:
				{
					if( g_pPlayer )
					{
						int nCost = m_nHairCost + m_nHairColorCostR + m_nHairColorCostG + m_nHairColorCostB;
						
						if( nCost < 0 )
							nCost = 0;
#ifdef __ONLINE
						g_DPlay.SendSetHair( m_dwHairMesh-1, m_fColor[0], m_fColor[1], m_fColor[2] );	//, nCost );
#endif
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
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndBeautyShop::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndConfirmSell )
		SAFE_DELETE( m_pWndConfirmSell );

	SAFE_DELETE( m_pModel );
}

void CWndBeautyShop::OnDestroy( void )
{
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pWndConfirmSell );
}

