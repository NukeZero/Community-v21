#include "StdAfx.h"

#include "WndHelperWebBox.h"
#include "../Neuz/HelperWebBox.h"
#include "../Neuz/WebCtrl.h"

//-----------------------------------------------------------------------------
CWndHelperWebBox::CWndHelperWebBox( void )
{
	SetPutRegInfo( FALSE );
}
//-----------------------------------------------------------------------------
CWndHelperWebBox::~CWndHelperWebBox( void )
{
	Release();
}
//-----------------------------------------------------------------------------
BOOL CWndHelperWebBox::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WEBBOX2, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
}
//-----------------------------------------------------------------------------
void CWndHelperWebBox::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	SetTitle( prj.GetText( TID_GAME_HELPER_WEB_BOX_ICON_TITLE ) );

	if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
	{
		CRect rectHelperWebBox = GetWndRect();
		rectHelperWebBox.right = CHelperWebBox::HELPER_WEB_SIZE_LARGE_X;
		rectHelperWebBox.bottom = CHelperWebBox::HELPER_WEB_SIZE_LARGE_Y;
		SetWndRect( rectHelperWebBox );
	}

//	DelWndStyle( WBS_MOVE );
	AddWndStyle( WBS_TOPMOST );
//	AddWndStyle( WBS_THICKFRAME );		//사이즈 조정 가능하게

//	CHelperWebBox::GetInstance()->Initialize();

//	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndHelperWebBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WTBID_CLOSE:
		{
			Release();
			Destroy();
			return TRUE;
		}
		
	//case WIDC_BUTTON_GO_BACK:
	//	{
	//		CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	//		if( pHelperWebBox && pHelperWebBox->GetDialogBoxHandle() )
	//		{
	//			::SendMessage( pHelperWebBox->GetDialogBoxHandle(), WEB_CTRL_GO_BACK, 0, 0 );
	//		}
	//		break;
	//	}
	//case WIDC_BUTTON_GO_FORWARD:
	//	{
	//		CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	//		if( pHelperWebBox && pHelperWebBox->GetDialogBoxHandle() )
	//		{
	//			::SendMessage( pHelperWebBox->GetDialogBoxHandle(), WEB_CTRL_GO_FORWARD, 0, 0 );
	//		}
	//		break;
	//	}
	//case WIDC_BUTTON_GO_FIRST_PAGE:
	//	{
	//		CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	//		if( pHelperWebBox && pHelperWebBox->GetDialogBoxHandle() )
	//		{
	//			pHelperWebBox->GoFirstPage();
	//		}
	//		break;
	//	}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndHelperWebBox::OnLButtonDown( UINT nFlags, CPoint point )
{
	int b = 0;
}

//-----------------------------------------------------------------------------
BOOL CWndHelperWebBox::Process( void )
{
	CHelperWebBox::GetInstance()->Process();

	//gmpbigsun: set position web box
	CHelperWebBox::GetInstance()->AutoPostion();

	return TRUE;
}

void CWndHelperWebBox::OnSetFocus(CWndBase* pOldWnd)
{
	int b = 0;
}
//-----------------------------------------------------------------------------
HRESULT CWndHelperWebBox::RestoreDeviceObjects( void )
{
	CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();

	if( pHelperWebBox && pHelperWebBox->GetDialogHandle() )
		::SendMessage( pHelperWebBox->GetDialogHandle(), WM_INITDIALOG, 0, 0 );

	return CWndNeuz::RestoreDeviceObjects();
}
//-----------------------------------------------------------------------------
void CWndHelperWebBox::Release( void )
{
	ShowWindow( CHelperWebBox::GetInstance()->GetDialogHandle(), SW_HIDE );
	CHelperWebBox::GetInstance()->Close();
}
//-----------------------------------------------------------------------------
void CWndHelperWebBox::OnSize( UINT nType, int cx, int cy )
{
//	CHelperWebBox::GetInstance()->OnSize( cx, cy );
	CWndNeuz::OnSize( nType, cx, cy );
}
//-----------------------------------------------------------------------------
void CWndHelperWebBox::SetMode( WEB_MODE mode )
{
	if( WMODE_CERTIFICATION == mode )
	{
		AddWndStyle( WBS_MOVE );
		CString strTemp;
		if( lstrcmp( g_xFlyffConfig->GetCertifierAddr(), "203.216.213.6" ) == 0 )
		{
			strTemp.Format( "http://test.secure.gpotato.jp/flyff/member/userconfirm.aspx?UID=%s", g_Option.m_szAccount );
		}
		else
		{
			strTemp.Format( "http://secure.gpotato.jp/flyff/member/userconfirm.aspx?UID=%s", g_Option.m_szAccount );
		}

		this->SetWndSize( CHelperWebBox::TYPE_A_X + 10, CHelperWebBox::TYPE_A_Y + 30 );
		CHelperWebBox::GetInstance()->SetSize( CHelperWebBox::TYPE_A_X, CHelperWebBox::TYPE_A_Y );

		//여기서 페이지를 다시 잡기때문에 Initialize() 후에 페이지를 다시 잡는다.
		CHelperWebBox::GetInstance()->Initialize();
		CHelperWebBox::GetInstance()->m_strHelperWebAddress = strTemp; 
		CHelperWebBox::GetInstance()->GoFirstPage();
	}
	else if( WMODE_HELPER == mode )
	{
	//	CHelperWebBox::GetInstance()->InitializeWebInformation();
		this->SetWndSize( 800, 600 );
		CHelperWebBox::GetInstance()->SetSize( CHelperWebBox::HELPER_WEB_SIZE_NORMAL_X, CHelperWebBox::HELPER_WEB_SIZE_NORMAL_Y );

		if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
		{
			//gmpbigsun(20101116): ??? * 800 모드에서 UI엔진적인 문제인것으로 판단되는 위치자동이동이 발생한다.
			// 나중에 더 살펴봐야 겠다. 일단 사이즈 조정으로 그런현상이 안나타나게 해놨음.
			int nRevisionHeight = 0;
			if( g_Option.m_nResHeight <= 800 )
				nRevisionHeight = 20;

			this->SetWndSize( 1024, 768 - nRevisionHeight );
			CHelperWebBox::GetInstance()->SetSize( CHelperWebBox::HELPER_WEB_SIZE_LARGE_X, CHelperWebBox::HELPER_WEB_SIZE_LARGE_Y - nRevisionHeight );
		}

		if( g_Option.m_nResHeight <= 768 )
		{
			//Mentis9449
			//gmpbigsun( 20100723 ) : 웹크기가 클라윈도와 일치되는경우( 1024 768이하 ) 약간의 움직임으로 (타이틀바를 클릭하거나) 인한 문제 수정
			//gmpbigsun( 20100816) : 와이드 문제 수정
			DelWndStyle( WBS_MOVE );
		}

		CHelperWebBox::GetInstance()->Initialize();
		CHelperWebBox::GetInstance()->GoFirstPage();

		SetFocus();
	}
	else if( WMODE_FACEBOOK == mode )
	{
		AddWndStyle( WBS_MOVE );

		this->SetWndSize( CHelperWebBox::TYPE_A_X + 10, CHelperWebBox::TYPE_A_Y + 30 );
		CHelperWebBox::GetInstance()->SetSize( CHelperWebBox::TYPE_A_X, CHelperWebBox::TYPE_A_Y );

		//여기서 페이지를 다시 잡기때문에 Initialize() 후에 페이지를 다시 잡는다.
		CHelperWebBox::GetInstance()->Initialize();
		//
		if ( g_xFlyffConfig->IsConnectedMainServer() ) 
			CHelperWebBox::GetInstance()->m_strHelperWebAddress = _T( "https://secure6.playpark.com/play/games/login.aspx?id=facebook&gsid=262" );
		else
			CHelperWebBox::GetInstance()->m_strHelperWebAddress = _T( "http://119.46.99.241/play/games/?id=facebook&gsid=262" );
		CHelperWebBox::GetInstance()->GoFirstPage();
	}
	else if( WMODE_GOOGLE == mode )
	{
		AddWndStyle( WBS_MOVE );

		this->SetWndSize( CHelperWebBox::TYPE_A_X + 10, CHelperWebBox::TYPE_A_Y + 30 );
		CHelperWebBox::GetInstance()->SetSize( CHelperWebBox::TYPE_A_X, CHelperWebBox::TYPE_A_Y );

		//여기서 페이지를 다시 잡기때문에 Initialize() 후에 페이지를 다시 잡는다.
		CHelperWebBox::GetInstance()->Initialize();
		
		if ( g_xFlyffConfig->IsConnectedMainServer() ) 
			CHelperWebBox::GetInstance()->m_strHelperWebAddress = _T( "https://secure6.playpark.com/play/games/login.aspx?id=google&gsid=262" );
		else
			CHelperWebBox::GetInstance()->m_strHelperWebAddress = _T( "http://119.46.99.241/play/games/?id=google&gsid=262" );
		CHelperWebBox::GetInstance()->GoFirstPage();
	}
}
//-----------------------------------------------------------------------------
