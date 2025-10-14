// BetaPatchClientDlg.cpp : implementation file
//

#include "stdafx.h"

#include "BetaPatchClient.h"
#include "HttpDownload.h"
#include "Picture.h"
#include "BetaPatchClientDlg.h"
#include "PatchManager.h"
#include "..\_Common\HwOption.h"
#include "DlgOption.h"
#include "DialogOptionFrame.h"

#include <Uxtheme.h>
#pragma comment( lib, "UxTheme.lib" )


extern	CPatchManager g_PatchManager;

CBetaPatchClientDlg *g_pDlg;

/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientDlg dialog

CBetaPatchClientDlg::CBetaPatchClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBetaPatchClientDlg::IDD, pParent)
{
	m_pHttpDownload		= NULL;
	m_nState			= STATE_WAIT_PATCHLISTFILE;
	m_bClose			= FALSE;
	m_pHttpDownload	    = new CHttpDownload( this );

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBetaPatchClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBetaPatchClientDlg)
	DDX_Control(pDX, IDC_BUTTON_START, m_Button_Start);
	DDX_Control(pDX, IDC_BUTTON_REGISTER, m_Button_Register);
	DDX_Control(pDX, IDC_FILEPROGRESS, m_File_Progress);
	DDX_Control(pDX, IDC_TOTALPROGRESS, m_Total_Progress);
	DDX_Control(pDX, IDC_FILE_NUMBER, m_Static_FileNumber);
	DDX_Control(pDX, IDC_DOWN_SPEED, m_Static_DownSpeed);
	DDX_Control(pDX, IDC_CURRENT_STATE, m_Static_CurrentState);
	DDX_Control(pDX, IDC_MASQUERADEVERSION, m_Static_MasqueradeVersion);
	DDX_Control(pDX, IDC_HOME, m_Static_HomeLink);
	DDX_Control(pDX, IDC_OPTION, m_Button_Option);
	DDX_Control(pDX, IDC_EXIT, m_Button_Exit);

	DDX_Control(pDX, IDC_BUTTON_START_DISABLE, m_Button_Start_Disable);
	DDX_Control(pDX, IDC_MINIMIZE, m_Button_Minimize);
	DDX_Control(pDX, IDC_PATCHNOTES, m_Button_PatchNotes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBetaPatchClientDlg, CDialog)
	//{{AFX_MSG_MAP(CBetaPatchClientDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_OPTION, OnOption)
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
	ON_WM_NCHITTEST()
	ON_MESSAGE( WM_HTTPDOWNLOAD_THREAD_FINISHED, OnHttpDownloadThreadFinished )
	ON_MESSAGE( WM_HTTPDOWNLOAD_FAIL_TO, OnHttpDownloadFail )
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, OnButtonRegister)
	ON_BN_CLICKED(IDC_PATCHNOTES, OnButtonPatchNotes)
//	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBetaPatchClientDlg message handlers

//BOOL CBetaPatchClientDlg::OnEraseBkgnd(CDC* pDC) 
//{
//	return CDialog::OnEraseBkgnd( pDC );
//}

HBRUSH CBetaPatchClientDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if( nCtlColor == CTLCOLOR_DLG )
		return m_brush;

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		return m_brush;
		//HBITMAP hPic = m_pic.GetBitmap();
		//if( hPic )
		//{
		//	CRect rtStatic;
		//	CRect rtBitmap;
		//	
		//	pWnd->GetClientRect( rtStatic );		
		//	pWnd->GetWindowRect( rtBitmap );
		//	ScreenToClient( rtBitmap );

		//	HDC hMemdc=CreateCompatibleDC( pDC->m_hDC );
		//	HBITMAP hOldBmp=(HBITMAP)SelectObject(hMemdc, hPic );
		//	BitBlt( pDC->m_hDC, 0, 0, rtStatic.Width(), rtStatic.Height(), hMemdc, rtBitmap.left, rtBitmap.top, SRCCOPY );
		//	SelectObject(hMemdc,hOldBmp);
		//	DeleteDC(hMemdc);

		//	return (HBRUSH)GetStockObject(NULL_BRUSH);
		//}
	}

	return hbr;
}

BOOL CBetaPatchClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_clrTransparent = RGB( 0, 255, 0 );

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	VERIFY((dwStyle & WS_POPUP) != 0);
	VERIFY((dwStyle & WS_BORDER) == 0);
	VERIFY((dwStyle & WS_SIZEBOX) == 0);
	VERIFY((dwStyle & WS_DLGFRAME) == 0);

	m_brush.CreateSolidBrush(m_clrTransparent);
	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	VERIFY((dwExStyle & WS_EX_APPWINDOW) == 0);
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	HMODULE hInst = LoadLibrary(_T("User32.DLL")); 
	typedef BOOL (WINAPI *MYFUNC)(HWND, COLORREF, BYTE, DWORD); 
	MYFUNC SetLayeredWindowAttributes = NULL; 
	SetLayeredWindowAttributes = (MYFUNC)::GetProcAddress(hInst, "SetLayeredWindowAttributes"); 
	SetLayeredWindowAttributes(GetSafeHwnd(), m_clrTransparent, 0, 1); 
	::FreeLibrary(hInst);

#if __CURRENT_LANG == LANG_KOR
#ifdef __LINK_PORTAL
	//무인자 일 경우 홈페이지로 연결
/*	CString strCmdLine;
	strCmdLine.Format("%s", AfxGetApp()->m_lpCmdLine);
	if(strCmdLine.GetLength() == 0 || strCmdLine == "1")
	{
		::ShellExecute(NULL, "open", LOGIN_LINK, NULL, ".", SW_SHOWNORMAL);
		OnExit();
		return FALSE;
	}
*/
//	else
//		AfxMessageBox( strCmdLine );
#endif //__LINK_PORTAL
#endif // LANG_KOR

	SetWindowText("FLYFF");


//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon
	if( InitControls() == FALSE )
		return TRUE;

	m_pHttpDownload->SetServer( PATCHSEVER_URL );
	
#if __CURRENT_LANG == LANG_FRA
	m_Static_CurrentState.SetText( _S(IDS_STR_FRA_SEARCHLIST) );
#else
	m_Static_CurrentState.SetText("Searching For Patch List");
#endif
	SetPatchVersionString( PATCH_VERSION );
	
	g_PatchManager.EnqueueFILE( "list.txt", false, 0, 0 );  
	m_nState = STATE_WAIT_PATCHLISTFILE;
	m_pHttpDownload->BeginDownload( &g_PatchManager );

//  for test
//	m_nState = STATE_WAIT_PATCHLISTFILE;
//	PostMessage( WM_HTTPDOWNLOAD_THREAD_FINISHED );
	return TRUE;	  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//void CBetaPatchClientDlg::OnPaint() 
//{
//	//if (IsIconic())
//	//{
//	//	CPaintDC dc(this); // device context for painting
//
//	//	SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
//
//	//	// Center icon in client rectangle
//	//	int cxIcon = GetSystemMetrics(SM_CXICON);
//	//	int cyIcon = GetSystemMetrics(SM_CYICON);
//	//	CRect rect;
//	//	GetClientRect(&rect);
//	//	int x = (rect.Width() - cxIcon + 1) / 2;
//	//	int y = (rect.Height() - cyIcon + 1) / 2;
//
//	//	// Draw the icon
//	//	dc.DrawIcon(x, y, m_hIcon);
//	//}
//	//else
//	//{
//		CDialog::OnPaint();
//	//}
//}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.


void CBetaPatchClientDlg::OnDestroy() 
{
	if( m_pHttpDownload )
	{
		delete m_pHttpDownload;
		m_pHttpDownload = NULL;
	}

	CDialog::OnDestroy();
}

void CBetaPatchClientDlg::OnOption() 
{
	char szPath[MAX_PATH];
	FLSPrintf( szPath, _countof( szPath ), "%s\\%s", g_PatchManager.GetCurrentLocalDirectory(), "neuz.ini" );

	try
	{
		g_Option.Load( szPath );
	}
	catch( ... )
	{
		g_Option.Init();
		g_Option.Save( szPath );
		return;
	}

	CDialogOptionFrame dlgFrame( this );
	dlgFrame.SetPath( szPath );
	dlgFrame.DoModal();

	//CDlgOption dlg;

	//dlg.m_bStartFullScreen = g_Option.m_bStartFullScreen;
	//dlg.m_nResWidth		= g_Option.m_nResWidth;
	//dlg.m_nResHeight	= g_Option.m_nResHeight;
	//dlg.m_nTexQual		= g_Option.m_nTextureQuality;
	//dlg.m_nViewArea		= g_Option.m_nViewArea;
	//dlg.m_nObjectDetail	= g_Option.m_nObjectDetail;
	//dlg.m_nShadow		= g_Option.m_nShadow;

	//if( dlg.DoModal() == IDOK )
	//{
	//	g_Option.m_bStartFullScreen     = dlg.m_bStartFullScreen;
	//	g_Option.m_nResWidth       = dlg.m_nResWidth;
	//	g_Option.m_nResHeight      = dlg.m_nResHeight;
	//	g_Option.m_nTextureQuality = dlg.m_nTexQual;
	//	g_Option.m_nViewArea	   = dlg.m_nViewArea;
	//	g_Option.m_nObjectDetail   = dlg.m_nObjectDetail;
	//	g_Option.m_nShadow		   = dlg.m_nShadow;

	//	g_Option.Save( szPath );
	//}
}

void CBetaPatchClientDlg::OnMinimize()
{
	::PostMessage( m_pParentWnd->GetSafeHwnd(),  WM_SYSCOMMAND, ( WPARAM )SC_MINIMIZE, 0  );
}

void CBetaPatchClientDlg::OnExit() 
{
	m_bClose = TRUE;
	Sleep( 100 );
	PostQuitMessage( 0 );
}

LRESULT CBetaPatchClientDlg::OnHttpDownloadThreadFinished( WPARAM wParam, LPARAM lParam )
{
	m_pHttpDownload->WaitForSafeToClose();

	switch( m_nState )
	{
	case STATE_WAIT_PATCHLISTFILE:
		{
			CWaitCursor cursor;
			MAKEPATCHLIST_RESULT result = g_PatchManager.MakePatchList( PATCH_VERSION );	
			switch( result )
			{
			case MAKEPATCHLIST_FILE_NOTFOUND:
				break;

			case MAKEPATCHLIST_VERSION_MISMATCH:
				g_PatchManager.EnqueueFILE( "\\NewFlyff.exe", false, 0, NULL );  
				m_nState = STATE_OLD_PATCH;
				m_pHttpDownload->BeginDownload( &g_PatchManager );
				break;
				
			case MAKEPATCHLIST_OK:
				if( g_PatchManager.GetListCount() == 0 )				// 패치 할 필요가 없다면
				{
#if __CURRENT_LANG == LANG_FRA
					m_Static_CurrentState.SetText( _S(IDS_STR_FRA_PATCHCOMP) );	// 패치 완료
#elif __CURRENT_LANG == LANG_POR
					m_Static_CurrentState.SetText( _S(IDS_STR_POR_PATCHCOMP) );	// 패치 완료
#else
					m_Static_CurrentState.SetText( "Patch Completed" );	// 패치 완료
					m_Static_CurrentState.SetFontBold( TRUE );
#endif
					m_Button_Start.ShowWindow( SW_SHOW );
					m_Button_Start.SetFocus();

					m_File_Progress.SetPos( 100 );
					m_Total_Progress.SetPos( 100 );
				}
				else
				{
#if __CURRENT_LANG == LANG_FRA
					m_Static_CurrentState.SetText( _S(IDS_STR_FRA_RUN) );			// 패치할 파일들 다운로드 시작 
#else
					m_Static_CurrentState.SetText( "Run..." );			// 패치할 파일들 다운로드 시작 
#endif
					m_nState = STATE_SELECTION;
					m_pHttpDownload->BeginDownload( &g_PatchManager );
				}
				break;
			}
		}
		break;

	case STATE_SELECTION:
		m_nState = STATE_PATCH_END;
#if __CURRENT_LANG == LANG_FRA
		m_Static_CurrentState.SetText( _S(IDS_STR_FRA_PATCHCOMP) );	// 패치 완료
#elif __CURRENT_LANG == LANG_POR
		m_Static_CurrentState.SetText( _S(IDS_STR_POR_PATCHCOMP) );	// 패치 완료
#else
		m_Static_CurrentState.SetText( "Patch Completed!!" );	// 패치 완료
		m_Static_CurrentState.SetFontBold( TRUE );
#endif
		m_Button_Start_Disable.ShowWindow( SW_HIDE );
		m_Button_Start.ShowWindow( SW_SHOW );
		m_Button_Start.SetFocus();
		m_File_Progress.ShowWindow( SW_HIDE );
		m_Total_Progress.ShowWindow( SW_HIDE );
		break;

	case STATE_OLD_PATCH:
		NewPatchClient();	// 패치 클라이언트가 다운로드 되었으니 updater로 업뎃하자.
		break;
	}
	return 0;
}

LRESULT CBetaPatchClientDlg::OnHttpDownloadFail( WPARAM wParam, LPARAM lParam )
{
	m_pHttpDownload->WaitForSafeToClose();
	PostQuitMessage( 0 );
	return 0;
}

void CBetaPatchClientDlg::OnButtonStart() 
{
#if __CURRENT_LANG == LANG_KOR
#ifdef __LINK_PORTAL
	//무인자 일 경우 홈페이지로 연결
/*	CString strCmdLine;
	strCmdLine.Format("%s", AfxGetApp()->m_lpCmdLine);
	if(strCmdLine.GetLength() == 0 || strCmdLine == "1")
	{
		::ShellExecute(NULL, "open", LOGIN_LINK, NULL, ".", SW_SHOWNORMAL);
		OnExit();
		return;
	}
*/
//	else
//		AfxMessageBox( strCmdLine );
#endif //__LINK_PORTAL
#endif // LANG_KOR
	if( g_PatchManager.GetListCount() == 0 || m_nState == STATE_PATCH_END )
	{
		CBetaPatchClientApp* pApp = (CBetaPatchClientApp*)AfxGetApp();
		pApp->RunClient();
	}
}

void CBetaPatchClientDlg::OnButtonRegister()
{
#if defined(__LANG_ENG_USA) || defined(__LANG_ENG_USATEST)
	::ShellExecute(NULL, "open", "https://member.webzen.com/SignUp/Create", NULL, ".", SW_SHOWNORMAL);
#elif defined(__LANG_GER)
	::ShellExecute(NULL, "open", "https://member.webzen.com/SignUp/Create", NULL, ".", SW_SHOWNORMAL);
#elif defined(__LANG_FRA)
	::ShellExecute(NULL, "open", "https://member.webzen.com/SignUp/Create", NULL, ".", SW_SHOWNORMAL);
#elif defined(__LANG_POR)
	::ShellExecute(NULL, "open", "http://register.gpotato.com.br/index.php?m=Register&a=Registration", NULL, ".", SW_SHOWNORMAL);
#elif defined(__LANG_ENG_ID_MAIN) || defined(__LANG_ENG_ID_TEST) || defined(__LANG_ENG_ID_TEST2)
	::ShellExecute(NULL, "open", "http://www.playmojo.com/newaccount.aspx", NULL, ".", SW_SHOWNORMAL);
#endif
}

void CBetaPatchClientDlg::OnButtonPatchNotes()
{
	::ShellExecute(NULL, "open", "http://en.flyff.gpotato.com/Community/News/PatchNote", NULL, ".", SW_SHOWNORMAL);
}

BOOL CBetaPatchClientDlg::InitControls()
{
#ifdef __LANG_RUS
	enum PublisherVersion { PUBLISHER_VERSION_1 = 0, PUBLISHER_VERSION_2 = 1 };
	PublisherVersion ePublisherVersion = PUBLISHER_VERSION_1;
	CScanner scanner;
	if( scanner.Load( "a.txt" ) == TRUE )
	{
		int nKeyNumber = scanner.GetNumber();
		if( nKeyNumber == 1 )
			ePublisherVersion = PUBLISHER_VERSION_2;
		else
			ePublisherVersion = PUBLISHER_VERSION_1;
	}
	else
		ePublisherVersion = PUBLISHER_VERSION_1;
#endif // __LANG_RUS

//#ifdef __LANG_RUS
//	DWORD dwBitmapID = ( ePublisherVersion == PUBLISHER_VERSION_1 ) ? IDC_BITMAP_MAIN : IDC_BITMAP_MAIN_2;
//	if( m_pic.Load( dwBitmapID ) )
//#else // __LANG_RUS
//	if( m_pic.Load( IDC_BITMAP_MAIN ) )
//#endif // __LANG_RUS
	{

		//사이즈 조정 
		CSize size( 950, 550 );
//		Gdiplus::Image* pImage = Gdiplus::Image::FromFile( L"LauncherBG.png" );
// #if 0
// 	Gdiplus::Image* pImage = Gdiplus::Image::FromFile( L"LauncherBG.png" );
// #else
// 
// 	Gdiplus::Image* pImage;
// 
// 	HINSTANCE hInstance = AfxGetApp()->m_hInstance;
// 	HRSRC hRsc = FindResource( hInstance, MAKEINTRESOURCE( IDR_PNG1 ), TEXT("PNG") );
// 
// 	DWORD dwSize = SizeofResource( hInstance, hRsc );
// 	HGLOBAL hGlobal = LoadResource( hInstance, hRsc );
// 	LPVOID pData = LockResource( hGlobal );
// 
// 	HGLOBAL hBuffer = GlobalAlloc( GMEM_MOVEABLE, dwSize );
// 	LPVOID pBuffer = GlobalLock( hBuffer );
// 
// 	CopyMemory( pBuffer, pData, dwSize );
// 	GlobalUnlock( hBuffer );
// 
// 	IStream* pStream = NULL;
// 	HRESULT hr = CreateStreamOnHGlobal( hBuffer, TRUE, &pStream );
// 
// 	pImage = Gdiplus::Image::FromStream( pStream );
// 
// 	pStream->Release();
// 
// 	FreeResource(hRsc);
// #endif
//		Gdiplus::Image* pImage = Gdiplus::Image::FromStream( L"../Image/LauncherBG.png" );

		//HINSTANCE hInstance = AfxGetApp()->m_hInstance;
		//HRSRC hRsc = FindResource( hInstance, MAKEINTRESOURCE( IDR_PNG1 ), TEXT("PNG") );

		//DWORD dwSize = SizeofResource( hInstance, hRsc );
		//HGLOBAL hGlobal = LoadResource( hInstance, hRsc );
		//LPVOID pData = LockResource( hGlobal );

		//HGLOBAL hBuffer = GlobalAlloc( GMEM_MOVEABLE, dwSize );
		//LPVOID pBuffer = GlobalLock( hBuffer );

		//CopyMemory( pBuffer, pData, dwSize );
		//GlobalUnlock( hBuffer );

		//IStream* pStream = NULL;
		//HRESULT hr = CreateStreamOnHGlobal( hBuffer, TRUE, &pStream );

		//Gdiplus::Image* pImage = Gdiplus::Image::FromStream( pStream );


// 		if( pImage )
// 		{
// 			size.cx = pImage->GetWidth();
// 			size.cy = pImage->GetHeight();
// 
// 			Gdiplus::DllExports::GdipFree( pImage );
// 		}else
// 		{
// 			AfxMessageBox( "failed to load image" );
// 		}
			
		SetWindowPos( NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOMOVE );

		HRGN hRgn = CreateRoundRectRgn( 0, 0, size.cx, size.cy, 20, 20 );
		if( hRgn )
			::SetWindowRgn( m_hWnd, hRgn, FALSE );
	}

	int nInfoGap = 0;
	int nWeblinkGap = 0;

	m_Button_Exit.SetBitmaps(IDB_BITMAP_CLOSE00, RGB(255, 0, 255), IDB_BITMAP_CLOSE01, RGB(255, 0, 255) );
	m_Button_Exit.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_Exit.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_Exit.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_Exit.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_Exit.SizeToContent();
	m_Button_Exit.DrawTransparent(TRUE);
	m_Button_Exit.DrawBorder(FALSE, FALSE);
	m_Button_Exit.SetBtnCursor(IDC_CURSOR1);

	m_Button_Option.SetBitmaps(IDB_BITMAP_OPTION00, RGB(255, 0, 255), IDB_BITMAP_OPTION01, RGB(255, 0, 255) );
	m_Button_Option.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_Option.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_Option.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_Option.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_Option.SizeToContent();
	m_Button_Option.DrawTransparent(TRUE);
	m_Button_Option.DrawBorder(FALSE, FALSE);
	m_Button_Option.SetBtnCursor(IDC_CURSOR1);

	m_Button_Start.SetBitmaps(IDB_BITMAP_START00, RGB(255, 0, 255), IDB_BITMAP_START01, RGB(255, 0, 255) );
	m_Button_Start.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_Start.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_Start.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_Start.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_Start.SizeToContent();
	m_Button_Start.DrawTransparent(TRUE);
	m_Button_Start.DrawBorder(FALSE, FALSE);
	m_Button_Start.SetBtnCursor(IDC_CURSOR1);

	m_Button_Register.SetBitmaps(IDB_BITMAP_REGISTER00, RGB(255, 0, 255), IDB_BITMAP_REGISTER01, RGB(255, 0, 255) );
	m_Button_Register.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_Register.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_Register.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_Register.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_Register.SizeToContent();
	m_Button_Register.DrawTransparent(TRUE);
	m_Button_Register.DrawBorder(FALSE, FALSE);
	m_Button_Register.SetBtnCursor(IDC_CURSOR1);

	m_Button_Minimize.SetBitmaps(IDB_BITMAP_MINIMIZE00, RGB(255, 0, 255), IDB_BITMAP_MINIMIZE01, RGB(255, 0, 255) );
	m_Button_Minimize.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_Minimize.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_Minimize.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_Minimize.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_Minimize.SizeToContent();
	m_Button_Minimize.DrawTransparent(TRUE);
	m_Button_Minimize.DrawBorder(FALSE, FALSE);
	m_Button_Minimize.SetBtnCursor(IDC_CURSOR1);

	m_Button_Start_Disable.SetBitmaps(IDB_BITMAP_START_D00, RGB(255, 0, 255), IDB_BITMAP_START_D01, RGB(255, 0, 255) );
	m_Button_Start_Disable.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_Start_Disable.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_Start_Disable.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_Start_Disable.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_Start_Disable.SizeToContent();
	m_Button_Start_Disable.DrawTransparent(TRUE);
	m_Button_Start_Disable.DrawBorder(FALSE, FALSE);
	m_Button_Start_Disable.SetBtnCursor(IDC_CURSOR1);

	m_Button_PatchNotes.SetBitmaps(IDB_BITMAP_PATCHNOTES00, RGB(255, 0, 255), IDB_BITMAP_PATCHNOTES01, RGB(255, 0, 255) );
	m_Button_PatchNotes.SetAlign(CButtonST::ST_ALIGN_OVERLAP, FALSE);
	m_Button_PatchNotes.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT, FALSE);
	m_Button_PatchNotes.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0));
	m_Button_PatchNotes.SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0));
	m_Button_PatchNotes.SizeToContent();
	m_Button_PatchNotes.DrawTransparent(TRUE);
	m_Button_PatchNotes.DrawBorder(FALSE, FALSE);
	m_Button_PatchNotes.SetBtnCursor(IDC_CURSOR1);
		

//#if defined(__LANG_ENG_USA) || defined(__LANG_ENG_USATEST) || defined(__LANG_GER) || defined(__LANG_FRA) || defined(__LANG_POR) || defined(__LANG_ENG_ID_MAIN) || defined(__LANG_ENG_ID_TEST) || defined(__LANG_ENG_ID_TEST2)
//	m_Button_Register.ShowWindow(SW_SHOW);
//#else
//	m_Button_Register.ShowWindow(SW_HIDE);
//#endif
	// static

//#if defined(__LANG_THAI) //Background Image 교체로 Font Color 변경
//	COLORREF cr = RGB( 95, 159, 159 );
//	m_Static_CurrentState.SetTextColor( cr );
//	m_Static_DownSpeed.SetTextColor( cr );
//	m_Static_FileNumber.SetTextColor( cr );
//	m_Static_MasqueradeVersion.SetTextColor( cr );
//#endif
//
//#if (__CURRENT_LANG == LANG_GER || __CURRENT_LANG == LANG_FRA )//|| __CURRENT_CNTRY == CNTRY_TWN)
//#if __VER == 12
//	COLORREF cr = RGB( 255, 255, 255 );
//	m_Static_CurrentState.SetTextColor( cr );
//	m_Static_DownSpeed.SetTextColor( cr );
//	m_Static_FileNumber.SetTextColor( cr );
//	m_Static_MasqueradeVersion.SetTextColor( cr );
//#endif
//#endif
//
//#if ( __CURRENT_LANG == LANG_SPA )
//	COLORREF cr = RGB( 255, 255, 0 );
//	m_Static_CurrentState.SetTextColor( cr );
//	m_Static_CurrentState.SetFontBold( TRUE );
//	m_Static_DownSpeed.SetTextColor( cr );
//	m_Static_DownSpeed.SetFontBold( TRUE );
//	m_Static_FileNumber.SetTextColor( cr );
//	m_Static_FileNumber.SetFontBold( TRUE );
//	m_Static_MasqueradeVersion.SetTextColor( cr );
//	m_Static_MasqueradeVersion.SetFontBold( TRUE );
//#endif // ( __CURRENT_LANG == __LANG_SPA )

	COLORREF cr = RGB( 255, 255, 255 );
	m_Static_CurrentState.SetTextColor( cr );
	m_Static_DownSpeed.SetTextColor( cr );
	m_Static_FileNumber.SetTextColor( cr );
	m_Static_MasqueradeVersion.SetTextColor( cr );

	TCHAR* szBannerWebURL = WEB_BANNER_PAGE;

#ifdef	__HANGAME0307     // 한게임 대응 버젼 
	TCHAR* szText   = HOME_LINK;
	TCHAR* szURL    = HOME_LINK;
	TCHAR* szWebURL = WEB_FIRST_PAGE;

	if( AfxGetApp()->m_lpCmdLine[0] != '\0' &&
		memcmp( AfxGetApp()->m_lpCmdLine, "hangame.co.jp", 13 ) == 0 )
	{
		szText   = "http://www.hangame.co.jp";
		szURL    = "http://www.hangame.co.jp/game.asp?fo";
		szWebURL = "http://flyff.gpotato.jp/launcher/index_hangame.aspx";//"http://www.flyff.jp/in_client/info/list.asp?domain=hangame.co.jp";
	}
#else					// 일반버젼 
#ifdef __LANG_RUS
	TCHAR* szText  = ( ePublisherVersion == PUBLISHER_VERSION_1 ) ? HOME_LINK : HOME_LINK_2;
	TCHAR* szURL   = ( ePublisherVersion == PUBLISHER_VERSION_1 ) ? HOME_LINK : HOME_LINK_2;
	TCHAR* szWebURL = ( ePublisherVersion == PUBLISHER_VERSION_1 ) ? WEB_FIRST_PAGE : WEB_FIRST_PAGE_2;
#else // __LANG_RUS
	TCHAR* szText   = HOME_LINK;
	TCHAR* szURL    = HOME_LINK;
	TCHAR* szWebURL = WEB_FIRST_PAGE;
#endif // __LANG_RUS
#endif	// __HANGAME0307

#if __CURRENT_LANG == LANG_KOR
#ifdef __LINK_PORTAL
	// 한국 : 엔젤, 버디 포탈 링크 연결
	TCHAR szArg1[64] = {0, }; 
	TCHAR szArg2[64] = {0, }; 

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	_stscanf_s( AfxGetApp()->m_lpCmdLine, "%s %s", szArg1, _countof( szArg1 ), szArg2, _countof( szArg2 ) );
#else
	_stscanf( AfxGetApp()->m_lpCmdLine, "%s %s", szArg1, szArg2 );
#endif

//	AfxMessageBox( AfxGetApp()->m_lpCmdLine );

	CString strCmdLine;
	strCmdLine.Format("%s", szArg2);

	int nLength = strCmdLine.GetLength();
	if(nLength > 4)
	{
		CString strCmpWord = strCmdLine.Mid(nLength-4, nLength);

		if(strCmpWord == "__an")
		{
			szText   = "http://af.gameangel.com";
			szURL    = "http://af.gameangel.com";
		}
		else if(strCmpWord == "__bu")
		{
			szText   = "http://flyff.buddybuddy.co.kr";
			szURL    = "http://flyff.buddybuddy.co.kr";
		}
	}
#endif //__LINK_PORTAL
#endif // LANG_KOR

	// 홈페이지 링크
	m_Static_HomeLink.SetWindowText( szText );
	m_Static_HomeLink.ShowWindow( SW_HIDE );

#if __CURRENT_CNTRY == CNTRY_HK
	m_Static_HomeLink.SetWindowPos( NULL, 300, 22+nWeblinkGap, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW );
#else
	m_Static_HomeLink.SetWindowPos( NULL, 290, 15+nWeblinkGap, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW );
#endif

#if __CURRENT_LANG == LANG_THA
	m_Static_HomeLink.SetColours( RGB( 190, 30, 20 ), RGB( 240, 130, 180), RGB( 112, 147, 219 ) );
#elif __CURRENT_LANG == LANG_RUS
	m_Static_HomeLink.SetColours( RGB( 255, 255, 255 ), RGB( 255, 0, 0 ), RGB( 0, 130, 0 ) );
#elif __CURRENT_LANG == LANG_KOR
	m_Static_HomeLink.SetColours( RGB( 255, 254, 178 ), RGB( 253, 135, 64 ), RGB( 105, 255, 140 ) );
#elif __CURRENT_LANG == LANG_GER
	m_Static_HomeLink.SetColours( RGB( 193, 177, 160 ), RGB( 255, 0, 0 ), RGB( 255, 0, 0 ) );
#elif __CURRENT_LANG == LANG_FRA
	m_Static_HomeLink.SetColours( RGB( 193, 177, 160 ), RGB( 255, 0, 0 ), RGB( 255, 0, 0 ) );
#elif __CURRENT_LANG == LANG_POR
	m_Static_HomeLink.SetColours( RGB( 255, 255, 255 ), RGB( 255, 0, 0 ), RGB( 255, 0, 0 ) );
#elif __CURRENT_LANG == LANG_SPA
	m_Static_HomeLink.SetColours( RGB( 255, 255, 0 ), RGB( 255, 0, 0 ), RGB( 0, 255, 0 ) );
#elif __COUNTRY == CNTRY_IND
	m_Static_HomeLink.SetColours( RGB( 0x2e, 0x31, 0x92 ), RGB( 255, 0, 0 ), RGB( 0, 255, 0 ) );
#else
	m_Static_HomeLink.SetColours( RGB( 0, 0, 0), RGB( 255, 0, 0), RGB( 0, 130, 0 ) );
#endif 

	m_Static_HomeLink.SetURL( szURL );
	m_Static_HomeLink.SetUnderline( CHyperLink::ulAlways );


	//	Progress 초기화
	m_File_Progress.SetPos(0);
	m_Total_Progress.SetPos(0);

	CreateWebControl( szWebURL );
	CreateWebControl2( szBannerWebURL );


	GetDlgItem( IDC_BUTTON_REGISTER )->MoveWindow( 38, 464, 140, 37 );
	GetDlgItem( IDC_OPTION )->MoveWindow( 38, 380, 140, 37 );
	
	GetDlgItem( IDC_CURRENT_STATE)->MoveWindow( 260, 462, 170, 20 );
	GetDlgItem( IDC_DOWN_SPEED)->MoveWindow( 440, 462, 100, 20 );
	GetDlgItem( IDC_FILE_NUMBER)->MoveWindow( 580, 462, 100, 20 );
	GetDlgItem( IDC_FILEPROGRESS )->MoveWindow( 260, 482, 430, 10 );
	GetDlgItem( IDC_TOTALPROGRESS)->MoveWindow( 260, 497, 430, 10 );
	GetDlgItem( IDC_MASQUERADEVERSION)->MoveWindow( 40, 508, 300, 20 );

	GetDlgItem( IDC_BUTTON_START )->MoveWindow( 774, 374, 138, 138 );
	GetDlgItem( IDC_BUTTON_START_DISABLE )->MoveWindow( 774, 374, 138, 138 );
	GetDlgItem( IDC_MINIMIZE )->MoveWindow( 868, 85, 20, 20 );
	GetDlgItem( IDC_EXIT )->MoveWindow( 893, 85, 20, 20 );
	GetDlgItem( IDC_PATCHNOTES )->MoveWindow( 38, 422, 140, 37 );

	::SetWindowTheme( GetDlgItem( IDC_CURRENT_STATE )->GetSafeHwnd(), NULL, NULL );
	::SetWindowTheme( GetDlgItem( IDC_DOWN_SPEED )->GetSafeHwnd(), NULL, NULL );
	::SetWindowTheme( GetDlgItem( IDC_FILE_NUMBER )->GetSafeHwnd(), NULL, NULL );
	::SetWindowTheme( GetDlgItem( IDC_MASQUERADEVERSION )->GetSafeHwnd(), NULL, NULL );

	return TRUE;
}

void CBetaPatchClientDlg::NewPatchClient()
{
	STARTUPINFO si;
	memset( &si, 0, sizeof( STARTUPINFO ) );
	si.cb = sizeof( STARTUPINFO );

	PROCESS_INFORMATION pi;
	
	// 패치 클라이언트 재 실행
#if __CURRENT_LANG == LANG_KOR
#ifdef __LINK_PORTAL
	CString strUpdater;
	strUpdater.Format("Updater.exe myunglang %s", AfxGetApp()->m_lpCmdLine);
//	AfxMessageBox(strUpdater);
	if( FALSE == CreateProcess( NULL, strUpdater.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
#else //__LINK_PORTAL
	if( FALSE == CreateProcess( NULL, "Updater.exe myunglang", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
#endif //__LINK_PORTAL
#else //__LANG_KOR
	if( FALSE == CreateProcess( NULL, "Updater.exe myunglang", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
#endif //__LANG_KOR
	{
		DeleteFile( "NewFlyff.exe.gz");
		DeleteFile( "NewFlyff.exe");
		AfxMessageBox( "Updater.exe file not found" );
		PostQuitMessage( 0 );
		return;
	}
	
	HWND hUpdater = NULL;
	while( hUpdater == NULL )
	{
		Sleep( 100 );
		hUpdater = ::FindWindow( "Aeonsoft", "Updater" );
	}

	pi.dwProcessId = ::GetCurrentProcessId();
	if( IDOK == AfxMessageBox(IDS_UPDATE_PATCH) )
		::SendMessage( hUpdater, SEND_PROCESSID, ( WPARAM )pi.dwProcessId, 0 );
	
	PostQuitMessage( 0 );
}

void CBetaPatchClientDlg::SetPatchVersionString( int nVersion )
{
	char szBuffer[256];
//	FLSPrintf( szBuffer, _countof( szBuffer ), "Flyff Patch Version %.1f %s", nVersion/10.0f, VERSION_NAME );
	FLSPrintf( szBuffer, _countof( szBuffer ), "Ver %.1f %s", nVersion/10.0f, VERSION_NAME );

#if __COUNTRY == CNTRY_IND
	COLORREF cr = RGB( 255, 255, 255 );
	m_Static_MasqueradeVersion.SetTextColor( cr );
#endif
	m_Static_MasqueradeVersion.SetText( szBuffer );

}

void CBetaPatchClientDlg::CreateWebControl( LPCTSTR szURL )
{
	int nStartX = 240;
	int nStartY = 367;

	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, 
		                            "",
				                    WS_VISIBLE | WS_CHILD, 
									CRect(nStartX, nStartY, nStartX + 470, nStartY + 89 ), 
									this, 
									AFX_IDW_PANE_FIRST))
	{
		return;
	}

	m_wndBrowser.ShowScrollBar( SB_BOTH, FALSE );

	
	IWebBrowser2* pBrowser;
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &pBrowser);
	if (SUCCEEDED(hr))
	{
		CString strURL( szURL );
		BSTR bstrURL = strURL.AllocSysString();

		COleSafeArray vPostData;
		LPCTSTR lpszTargetFrameName = NULL;
		LPCTSTR lpszHeaders = NULL;

		pBrowser->Navigate(bstrURL,
			COleVariant((long) 0, VT_I4),
			COleVariant(lpszTargetFrameName, VT_BSTR),
			vPostData,
			COleVariant(lpszHeaders, VT_BSTR));
	}
}

void CBetaPatchClientDlg::CreateWebControl2( LPCTSTR szURL )
{
	int nStartX = 117;
	int nStartY = 100;
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser2.CreateControl(CLSID_WebBrowser, 
		                            "",
				                    WS_VISIBLE | WS_CHILD  | WS_VSCROLL | WS_HSCROLL, 
//									CRect(nStartX, nStartY, nStartX + 720, nStartY + 270 ), 
									CRect(nStartX, nStartY, nStartX + 712, nStartY + 249 ), 
									this, 
									AFX_IDW_PANE_FIRST))
	{
		return;
	}

	IWebBrowser2* pBrowser;
	LPUNKNOWN lpUnk = m_wndBrowser2.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &pBrowser);
	if (SUCCEEDED(hr))
	{
		CString strURL( szURL );
		BSTR bstrURL = strURL.AllocSysString();

		COleSafeArray vPostData;
		LPCTSTR lpszTargetFrameName = NULL;
		LPCTSTR lpszHeaders = NULL;

		pBrowser->Navigate(bstrURL,
			COleVariant((long) 0, VT_I4),
			COleVariant(lpszTargetFrameName, VT_BSTR),
			vPostData,
			COleVariant(lpszHeaders, VT_BSTR));
	}
}


void CBetaPatchClientDlg::ShowDialog()
{
	Create(m_lpszTemplateName, m_pParentWnd);
	ShowWindow(SW_SHOW);
}


