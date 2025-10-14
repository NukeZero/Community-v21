#include "stdafx.h"
#include "definetext.h"
#include "AppDefine.h"
#include "WndVendor.h"
#include "WndRegVend.h"
#include "dpclient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
	WndId : APP_REGVEND
	CtrlId : WIDC_EDIT1 - 개수
	CtrlId : WIDC_EDIT2 - 단가
	CtrlId : WIDC_OK
	CtrlId : WIDC_BUTTON2
*/
CWndRegVend::CWndRegVend()
{
	m_pItemBase	= NULL;
	m_iIndex	= -1;

#ifdef __YNEWPERSONSHOP_EX1026
	m_dwFocus   = -1;
	m_pt[0] = m_pt[1] = CPoint(0,0);
#endif //__YNEWPERSONSHOP_EX1026
}

CWndRegVend::~CWndRegVend()
{

}

void CWndRegVend::SetItem( int iIndex, CItemBase* pItemBase )
{
#ifdef __YNEWPERSONSHOP_EX1026
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_SELLNUM);

	char str[32]	= { 0, };
	sprintf( str, "%d", ( (CItemElem*)pItemBase )->m_nItemNum );
	pStatic->SetTitle( str );
	m_dwFocus = 0;
	
	ItemProp* pItemProp	= pItemBase->GetProp();
	if( pItemProp )
	{
		pStatic	= (CWndStatic*)GetDlgItem(WIDC_SELLPRI);
		sprintf( str, "%d", pItemProp->dwCost );
		pStatic->SetTitle( str );

		if(( (CItemElem*)pItemBase )->m_nItemNum <= 1 )
			m_dwFocus = 1;
	}
#else //__YNEWPERSONSHOP_EX1026
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	char str[32]	= { 0, };
	sprintf( str, "%d", ( (CItemElem*)pItemBase )->m_nItemNum );
	pWndEdit->SetString( str );
	pWndEdit->SetFocus();
	ItemProp* pItemProp	= pItemBase->GetProp();
	if( pItemProp )
	{
		pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		sprintf( str, "%d", pItemProp->dwCost );
		pWndEdit->SetString( str );

		if(( (CItemElem*)pItemBase )->m_nItemNum <= 1 )
			pWndEdit->SetFocus();
	}
#endif //__YNEWPERSONSHOP_EX1026		
	m_iIndex	= iIndex;
	m_pItemBase		= pItemBase;
}

void CWndRegVend::OnDraw( C2DRender* p2DRender ) 
{
#ifdef __YNEWPERSONSHOP_EX1026
	//SetFocus();
	
	if( m_dwFocus != -1 )
	{
		m_pTex->Render( p2DRender, m_pt[m_dwFocus] );
	}
#endif //__YNEWPERSONSHOP_EX1026	
}

void CWndRegVend::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
#ifdef __YNEWPERSONSHOP_EX1026
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_SELLNUM);
	CRect rect = pStatic->GetWindowRect(TRUE);
	m_pt[0] = pStatic->GetWindowRect(TRUE).TopLeft();
	m_pt[0].y -= 3.0f;
	m_pt[0].x -= 7.0f;
	pStatic->AddWndStyle(WSS_MONEY);

	pStatic = (CWndStatic*)GetDlgItem(WIDC_SELLPRI);
	rect = pStatic->GetWindowRect(TRUE);
	m_pt[1] = pStatic->GetWindowRect(TRUE).TopLeft();
	m_pt[1].y -= 3.0f;
	m_pt[1].x -= 7.0f;
	pStatic->AddWndStyle(WSS_MONEY);
	
	m_bIsFirst      = TRUE;
	m_Calc.m_nValue = 0;
	m_Calc.m_ch     = 0;
	SetFocus();

	m_pTex = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "WndVenderArrow.tga" ), 0xffff00ff );
#else //__YNEWPERSONSHOP_EX1026

	m_dwOldFocus = WIDC_EDIT1;

	CWndEdit* pWndEdit = NULL;
	pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->EnableModeChange( FALSE );
	pWndEdit->SetTabStop(TRUE);

//	if( pWndEdit )
//		pWndEdit->SetFocus();
				
	pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	pWndEdit->EnableModeChange( FALSE );
	pWndEdit->SetTabStop(TRUE);
	
#endif //__YNEWPERSONSHOP_EX1026

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndRegVend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_VENDOREX_SELL, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndRegVend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndRegVend::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndRegVend::OnLButtonUp( UINT nFlags, CPoint point )
{
#ifdef __YNEWPERSONSHOP_EX1026
	{
		CPoint point2 = GetMousePoint();
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_SELLNUM );
		CRect rectCtrl = lpWndCtrl->rect;
		
		if( rectCtrl.PtInRect( point2 ) )
		{
			m_dwFocus = 0;
			return;
		}

		lpWndCtrl = GetWndCtrl(WIDC_SELLPRI);
		rectCtrl = lpWndCtrl->rect;
		if( rectCtrl.PtInRect( point2 ) )
		{
			m_dwFocus = 1;
			return;
		}
	}
#endif // __YNEWPERSONSHOP_EX1026
}

void CWndRegVend::OnLButtonDown( UINT nFlags, CPoint point )
{
}

BOOL CWndRegVend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CString str;

#ifdef __YNEWPERSONSHOP_EX1026
	SetFocus();
	
	CWndStatic* pWndStatic;
	pWndStatic = (m_dwFocus == 0) ? (CWndStatic*)GetDlgItem( WIDC_SELLNUM ): (CWndStatic*)GetDlgItem( WIDC_SELLPRI );

	str = pWndStatic->GetTitle();

	switch( nID )
	{
	case WIDC_VENEQUA:
		{
			if( m_bIsFirst )
			{
				char buffer[20] = { 0 };
				int i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_bIsFirst = FALSE;
				m_Calc.m_nValue = 0;
			}
		}
		break;
	case WIDC_VENPLUS:
		OnChar('+');
		break;
	case WIDC_VENMINUS:
		OnChar('-');
		break;
	case WIDC_VENMULTI:
		OnChar('*');
		break;
	case WIDC_VENDIVID:
		OnChar('/');
		break;
	case WIDC_NUM0:
		OnChar('0');
		break;
	case WIDC_NUM1:
		OnChar('1');
		break;
	case WIDC_NUM2:
		OnChar('2');
		break;
	case WIDC_NUM3:
		OnChar('3');
		break;
	case WIDC_NUM4:
		OnChar('4');
		break;
	case WIDC_NUM5:
		OnChar('5');
		break;
	case WIDC_NUM6:
		OnChar('6');
		break;
	case WIDC_NUM7:
		OnChar('7');
		break;
	case WIDC_NUM8:
		OnChar('8');
		break;
	case WIDC_NUM9:
		OnChar('9');
		break;
	case WIDC_NUM00:
		{
			if(str == "0")
				break;

			str+="00";
			pWndStatic->SetTitle(str);
			break;
		}
	case WIDC_NUM000:
		{
			if(str == "0")
				break;

			str+="000";
			pWndStatic->SetTitle(str);
			break;
		}
	case WIDC_VENMIN:
		{
			pWndStatic->SetTitle( "1" );
			break;
		}
	case WIDC_VENMAX:
		{
			if(m_pItemBase)
			{
				ItemProp *pItemProp = m_pItemBase->GetProp();

				int nValue = 0;
				nValue = ( m_dwFocus == 0 ) ? ( (CItemElem*)m_pItemBase )->m_nItemNum: pItemProp->dwCost;
				
				CString str;
				str.Format( "%d", nValue );
				
				pWndStatic->SetTitle(str);
				
			}
			break;
		}
	}
	
	if( nID == WIDC_OK || message ==  EN_RETURN )
	{
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_SELLNUM );
		LPCTSTR str	= pWndStatic->GetTitle();
		int nNum	= 1;
		nNum	= atoi( str );
		CItemElem* pItemElem	= (CItemElem*)m_pItemBase;
		if( nNum < 1 )	return TRUE;	// 개수가 너무 작습니다.
		if( nNum > pItemElem->m_nItemNum )	nNum	= pItemElem->m_nItemNum;

		int nCost	= 0;
		pWndStatic	= (CWndStatic*)GetDlgItem( WIDC_SELLPRI );
		str		= pWndStatic->GetTitle();
		if( strlen( str ) > 9 )	return TRUE;	// 숫자가 너무 큽니다.
		nCost	= atoi( str );
		if( nCost < 1 )	return TRUE;	// 비정상적인 수치가 입력되었습니다.
		
		ItemProp* pItemProp	= m_pItemBase->GetProp();
		if( nCost > pItemProp->dwCost * 100 )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_LIMITSELL)), MB_OK, this );
			
			return TRUE;
		}
#ifdef __ONLINE
		g_DPlay.SendRegisterPVendorItem( m_iIndex, 0, m_pItemBase->m_dwObjId, nNum, nCost );
#endif	// __ONLINE
			
		Destroy( FALSE );
	}
	else if( nID == WIDC_CANCEL )	// WIDC_CANCEL
	{
		Destroy( FALSE );
	}
	
#else //__YNEWPERSONSHOP_EX1026
	CWndEdit* pWndEdit = NULL;

	switch( nID )
	{
		case WIDC_EDIT1:
			m_dwOldFocus = WIDC_EDIT1;
			break;
		case WIDC_EDIT2:
			m_dwOldFocus = WIDC_EDIT2;
			break;
	}

	// 포커스 주기
	switch( m_dwOldFocus )
	{
		case WIDC_EDIT1:
			{
				pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
				pWndEdit->SetFocus();
				break;
			}
		case WIDC_EDIT2:
			{
				pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
				pWndEdit->SetFocus();
				break;
			}
	}
	

	switch( nID )
	{
	case WIDC_NUM0:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '0', 0 );
		break;
	case WIDC_NUM1:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '1', 0 );
		break;
	case WIDC_NUM2:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '2', 0 );
		break;
	case WIDC_NUM3:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '3', 0 );
		break;
	case WIDC_NUM4:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '4', 0 );
		break;
	case WIDC_NUM5:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '5', 0 );
		break;
	case WIDC_NUM6:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '6', 0 );
		break;
	case WIDC_NUM7:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '7', 0 );
		break;
	case WIDC_NUM8:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '8', 0 );
		break;
	case WIDC_NUM9:
		::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '9', 0 );
		break;
	case WIDC_NUM00:
		{
			::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '0', 0 );
			::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '0', 0 );
			break;
		}
	case WIDC_NUM000:
		{
			::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '0', 0 );
			::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '0', 0 );
			::SendMessage( pWndEdit->m_hWnd, WM_CHAR, '0', 0 );
			break;
		}
	case WIDC_VENMIN:
		{
			if(pWndEdit)
				pWndEdit->SetString("1");
			break;
		}
	case WIDC_VENMAX:
		{
			if(pWndEdit && m_pItemBase)
			{
				ItemProp *pItemProp = m_pItemBase->GetProp();
				
				int nValue = 0;
				nValue = ( m_dwOldFocus == WIDC_EDIT1 ) ? ( (CItemElem*)m_pItemBase )->m_nItemNum: pItemProp->dwCost;

				CString str;
				str.Format( "%d", nValue );

				pWndEdit->SetString(str);
			}
			break;
		}
	}
	
	if( nID == WIDC_OK || message ==  EN_RETURN )
	{
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		LPCTSTR str	= pWndEdit->GetString();
		int nNum	= 1;
		nNum	= atoi( str );
		CItemElem* pItemElem	= (CItemElem*)m_pItemBase;
		if( nNum < 1 )	return TRUE;	// 개수가 너무 작습니다.
		if( nNum > pItemElem->m_nItemNum )	nNum	= pItemElem->m_nItemNum;

		int nCost	= 0;
		pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		str		= pWndEdit->GetString();
		if( strlen( str ) > 9 )	return TRUE;	// 숫자가 너무 큽니다.
		nCost	= atoi( str );
		if( nCost < 1 )	return TRUE;	// 비정상적인 수치가 입력되었습니다.
		
		ItemProp* pItemProp	= m_pItemBase->GetProp();
		if( nCost > pItemProp->dwCost * 100 )
		{
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_LIMITSELL)), MB_OK, this );
			
			return TRUE;
		}

#ifdef __ONLINE
		g_DPlay.SendRegisterPVendorItem( m_iIndex, 0, m_pItemBase->m_dwObjId, nNum, nCost );
#endif	// __ONLINE
			
		Destroy( FALSE );
	}
	else if( nID == WIDC_CANCEL )	// WIDC_CANCEL
	{
		Destroy( FALSE );
	}
#endif //__YNEWPERSONSHOP_EX1026




	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


#ifdef __YNEWPERSONSHOP_EX1026
int CWndRegVend::ProcessCalc(CALC_DATA calc, int num)
{
	int i;

	switch( calc.m_ch )
	{
	case '*':
		i = m_Calc.m_nValue * num;
		return i;
	case '/':
		i = m_Calc.m_nValue / num;
		return i;
	case '+':
		i = m_Calc.m_nValue + num;
		return i;
	case '-':
		i = m_Calc.m_nValue - num;
		return i;
	}
	
	return 0;
}

void CWndRegVend::OnChar(UINT nChar)
{
	int i;
	int nLen;
	CString str;
	BOOL    bKeychar = FALSE;
	CWndStatic* pWndStatic;
	pWndStatic = (m_dwFocus == 0) ? (CWndStatic*)GetDlgItem( WIDC_SELLNUM ): (CWndStatic*)GetDlgItem( WIDC_SELLPRI );
	
	str = pWndStatic->GetTitle();
	nLen = str.GetLength();
	char buffer[20] = { 0 };

	SetFocus();
	
	switch( nChar )
	{
		case 27:
			{
				m_bIsFirst = FALSE;
				m_Calc.m_nValue = 0;
				pWndStatic->SetTitle("0");
				break;
			}
		case '*':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}				
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case '/':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}				
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case '+':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}				
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case '-':
			if( m_bIsFirst && m_Calc.m_nValue != 0 )
			{
				i = ProcessCalc( m_Calc, atoi(str) );
				_itoa( i, buffer, 10 );
				str = buffer;
				pWndStatic->SetTitle(str);
				m_Calc.m_nValue = 0;
			}
			m_bIsFirst = FALSE;
			m_Calc.m_nValue = atoi(str);
			m_Calc.m_ch     = nChar;
			break;
		case 8:
			if( nLen > 1 )
				str.Delete( nLen-1 );
			else
				str = "0";

			pWndStatic->SetTitle(str);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			bKeychar = TRUE;
			break;
	}	

	if( bKeychar )
	{
		if( m_bIsFirst == FALSE )
		{
			str = "0";
			m_bIsFirst = TRUE;
		}
		
		(str != "0") ? str+=nChar: str=nChar;
		pWndStatic->SetTitle(str);
	}
	
}
#endif //__YNEWPERSONSHOP_EX1026

