#include "StdAfx.h"
#include "HelperWebBox.h"
#include "WebCtrl.h"

//fix webbox position
#include "WndHelperWebBox.h"


//-----------------------------------------------------------------------------
BOOL CALLBACK HelperWebDialogProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
//-----------------------------------------------------------------------------
CHelperWebBox::CHelperWebBox( void ) : 
m_strHelperWebAddress( _T( "" ) ), 
m_hInstance( NULL ), 
m_hParent( NULL ), 
m_hDialog( NULL ), 
m_hDialogBox( NULL ), 
m_rectHelperWeb( 0, 0, 0, 0 ),
m_iSizeX( HELPER_WEB_SIZE_NORMAL_X ),
m_iSizeY( HELPER_WEB_SIZE_NORMAL_Y )
{
}
//-----------------------------------------------------------------------------
CHelperWebBox::~CHelperWebBox( void )
{
	Close();
}
//-----------------------------------------------------------------------------
CHelperWebBox* CHelperWebBox::GetInstance( void )
{
	static CHelperWebBox sHelperWebBox;
	return &sHelperWebBox;
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Initialize( void )
{
	//InitWebCtrl( );
	InitializeWebInformation();

	D3DDEVICE->SetDialogBoxMode( TRUE );

	m_hInstance = g_Neuz.m_hInstance;
	m_hParent = g_Neuz.GetSafeHwnd();

	//if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
	//{
	//	m_rectHelperWeb.SetRect( -1, 0, HELPER_WEB_SIZE_LARGE_X + 5, HELPER_WEB_SIZE_LARGE_Y );
	//}
	//else
	//{
	//	m_rectHelperWeb.SetRect( -1, 0, HELPER_WEB_SIZE_NORMAL_X + 5, HELPER_WEB_SIZE_NORMAL_Y );
	//}

	Close();

	m_rectHelperWeb = CRect( 0, 0, CHelperWebBox::m_iSizeX, CHelperWebBox::m_iSizeY );
	HINSTANCE hInstance = GetModuleHandle( NULL );
	m_hDialog = CreateDialog( hInstance, MAKEINTRESOURCE( IDD_WEBDLG ), m_hParent, HelperWebDialogProc );
	
	//gmpbigsun(20100630): Alt+F4막음, SetClassLong은 32Bit전용이라 확장성를 고려해 Ptr버젼을 사용했다.
	SetClassLongPtr( m_hDialog, GCL_STYLE, (LONG_PTR)CS_NOCLOSE );

	m_hDialogBox = CreateWindow( 
		WC_WEBCTRL, 
		m_strHelperWebAddress, 
		WS_EX_TOPMOST | WS_CHILD | CS_NOCLOSE | WS_BORDER ,
		m_rectHelperWeb.left, 
		m_rectHelperWeb.top, 
		m_rectHelperWeb.right, 
		m_rectHelperWeb.bottom, 
		m_hDialog, 
		NULL, 
		m_hInstance, 
		NULL );
	
	GoFirstPage();

	ShowWindow( m_hDialogBox, SW_SHOW );

	//gmpbigsun:웹 페이지 스크롤 등장
	CWebCtrl *pthis = (CWebCtrl *) GetProp( m_hDialogBox, "this" );
	if( pthis )
		pthis->ShowScroll( TRUE );
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Process( void )
{
	if( m_hDialogBox )
	{
		static BOOL bBackKeySwitch = FALSE;
		if( GetAsyncKeyState( VK_CONTROL ) & 0x80000 )	//gmpbigsun: 웹페이지에서 ( CTRL+VK_BACK ) == Back to the page
		{
			if( GetAsyncKeyState( VK_BACK ) & 0x8000 )
			{
				if( bBackKeySwitch == FALSE )
				{
					SendMessage( m_hDialogBox, WEB_CTRL_GO_BACK, 0, 0 );
					bBackKeySwitch = TRUE;
				}
			}
			else
			{
				if( bBackKeySwitch == TRUE )
				{
					bBackKeySwitch = FALSE;
				}
			}
		}

		static BOOL bF5KeySwitch = FALSE;
		if( GetAsyncKeyState( VK_F5 ) & 0x8000 )
		{
			if( bF5KeySwitch == FALSE )
			{
				SendMessage( m_hDialogBox, WEB_CTRL_REFRESH, 0, 0 );
				bF5KeySwitch = TRUE;
			}
		}
		else
		{
			if( bF5KeySwitch == TRUE )
			{
				bF5KeySwitch = FALSE;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Release( void )
{
	ShowWindow( m_hDialogBox, SW_HIDE );
	SetFocus( m_hParent );
	Close();
	UninitWebCtrl();
}
//-----------------------------------------------------------------------------
void CHelperWebBox::InitializeWebInformation( void )
{
	switch( g_xFlyffConfig->GetMainLanguage() )
	{
#ifdef __INTERNALSERVER
	case LANG_KOR:
		{
			m_strHelperWebAddress = _T( "http://flyff-wiki.gpotato.com/index.php/Main_Page" );
		}
		break;
#endif // __INTERNALSERVER
	case LANG_USA:
		{
			m_strHelperWebAddress = _T( "http://flyff-wiki.gpotato.com/index.php/Main_Page" );
		}
		break;
	}

	return;
}
//-----------------------------------------------------------------------------
void CHelperWebBox::Close( void )
{
	if( m_hDialogBox )
	{	
		DestroyWindow( m_hDialogBox );
		m_hDialogBox = NULL;
	}

	if( m_hDialog )
	{
		DestroyWindow( m_hDialog );
		m_hDialog = NULL;
	}

	return;
}
//-----------------------------------------------------------------------------
void CHelperWebBox::GoFirstPage( void )
{
	if( m_strHelperWebAddress == _T( "" ) )
	{
		return;
	}

	TCHAR szHelperWebAddress[ 512 ] = { 0, };
	TCHAR header[WEB_STR_LEN] = { 0, };
	FLStrcpy( szHelperWebAddress, _countof( szHelperWebAddress ), m_strHelperWebAddress );
	SendMessage( m_hDialogBox, WEB_CTRL_ADDRESS, 0, ( LPARAM )szHelperWebAddress );
	SendMessage( m_hDialogBox, WEB_CTRL_NAVIGATE, 0, 0 );
}
//-----------------------------------------------------------------------------
const CString& CHelperWebBox::GetHelperWebAddress( void ) const
{
	return m_strHelperWebAddress;
}
//-----------------------------------------------------------------------------
const HWND& CHelperWebBox::GetParentHandle( void ) const
{
	return m_hParent;
}
//-----------------------------------------------------------------------------
const HWND& CHelperWebBox::GetDialogHandle( void ) const
{
	return m_hDialog;
}
//-----------------------------------------------------------------------------
const HWND& CHelperWebBox::GetDialogBoxHandle( void ) const
{
	return m_hDialogBox;
}
//-----------------------------------------------------------------------------
void CHelperWebBox::AutoPostion( )
{
	//gmpbigsun : 웹박스의 위치를 잡아준다.
	CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
	int nHelperWebSizeX = m_iSizeX;
	int nHelperWebSizeY = m_iSizeY;
	
	CRect rcCenter( 0, 0, 0, 0 );
	GetWindowRect( pHelperWebBox->GetParentHandle(), &rcCenter );

	int xLeft = ( rcCenter.left + rcCenter.right ) / 2 - nHelperWebSizeX / 2;
	int yTop = ( rcCenter.top + rcCenter.bottom ) / 2 - nHelperWebSizeY / 2;

	//부모윈도
	CWndHelperWebBox* pWndHelperWebBox = ( CWndHelperWebBox* )g_WndMng.GetApplet( APP_WEBBOX2 );
	if( pWndHelperWebBox )
	{
		//CRect rect = pWndHelperWebBox->GetWndRect();	
		CRect rect = pWndHelperWebBox->GetWndRect();	
		xLeft = rcCenter.left + rect.left + 8;
		yTop = rcCenter.top + rect.top + 32;
	}

	DWORD GapH = 0;


	if( g_Neuz.IsWindow() == true )
		GapH = GetSystemMetrics( SM_CYCAPTION );
	else
		GapH = 9;

	SetWindowPos( m_hDialog, NULL, xLeft, yTop+GapH, nHelperWebSizeX, nHelperWebSizeY,  SWP_NOZORDER | SWP_NOACTIVATE );
	//	MoveWindow( m_hDialog, xLeft, yTop + GapH, nHelperWebSizeX, nHelperWebSizeY, TRUE );
}
//-----------------------------------------------------------------------------
void CHelperWebBox::SetSize( int cx, int cy )
{
	m_iSizeX = cx;
	m_iSizeY = cy;

	if( cx < 0 || cy < 0 )
		FLERROR_LOG( PROGRAM_NAME, _T( "x %d, y %d" ), cx, cy  );
}
//-----------------------------------------------------------------------------
void CHelperWebBox::OnSize( int cx, int cy )
{
//	CRect rect;
//	::GetWindowRect( m_hDialog, &rect );
//	::MoveWindow( m_hDialog, rect.left, rect.top, cx, cy, TRUE );
//	::SetWindowPos( m_hDialog, NULL, rect.left, rect.top, cx, cy, SWP_FRAMECHANGED );
}
//-----------------------------------------------------------------------------

BOOL CALLBACK HelperWebDialogProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();

	int nHelperWebSizeX = CHelperWebBox::HELPER_WEB_SIZE_NORMAL_X;
	int nHelperWebSizeY = CHelperWebBox::HELPER_WEB_SIZE_NORMAL_Y;
//	if( g_Option.m_nResWidth >= 1024 && g_Option.m_nResHeight >= 768 )
//	{
//		nHelperWebSizeX = CHelperWebBox::HELPER_WEB_SIZE_NORMAL_X;
//		nHelperWebSizeY = CHelperWebBox::HELPER_WEB_SIZE_LARGE_Y;
//	}

	switch( Msg )
	{
	case WM_INITDIALOG:
		{
			CRect rcCenter( 0, 0, 0, 0 );
			GetWindowRect( pHelperWebBox->GetParentHandle(), &rcCenter );

			int xLeft = ( rcCenter.left + rcCenter.right ) / 2 - nHelperWebSizeX / 2;
			int yTop = ( rcCenter.top + rcCenter.bottom ) / 2 - nHelperWebSizeY / 2;

			DWORD CapH = 0;

			if( g_Neuz.IsWindow() == true )
				CapH = GetSystemMetrics( SM_CYCAPTION );
			else
				CapH = 9;

			SetWindowPos( hWnd, NULL, xLeft, yTop+CapH, nHelperWebSizeX, nHelperWebSizeY, SWP_NOZORDER | SWP_NOACTIVATE );
			break;
		}
	case WM_COMMAND:
		{
			switch( wParam )
			{
			case IDCANCEL:
				{
					ShowWindow( pHelperWebBox->GetDialogHandle(), SW_HIDE );
					break;
				}
			}
			break;
		}
	}
	return FALSE;
}
//-----------------------------------------------------------------------------

