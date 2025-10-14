// DialogFrame.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "BetaPatchClient.h"
#include "DialogFrame.h"
#include "BetaPatchClientDlg.h"


// CDialogFrame 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogFrame, CDialog)

ULONG_PTR gdiplusToken;
CDialogFrame::CDialogFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogFrame::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDlgPatch = NULL;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CDialogFrame::~CDialogFrame()
{
}

void CDialogFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogFrame, CDialog)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVE()
END_MESSAGE_MAP()

BOOL CDialogFrame::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText("FLYFF");

#if 0
	m_pImage = Gdiplus::Image::FromFile( L"LauncherBG.png" );
#else
	HINSTANCE hInstance = AfxGetApp()->m_hInstance;
	HRSRC hRsc = FindResource( hInstance, MAKEINTRESOURCE( IDR_PNG1 ), TEXT("PNG") );

	DWORD dwSize = SizeofResource( hInstance, hRsc );
	HGLOBAL hGlobal = LoadResource( hInstance, hRsc );
	LPVOID pData = LockResource( hGlobal );

	HGLOBAL hBuffer = GlobalAlloc( GMEM_MOVEABLE, dwSize );
	LPVOID pBuffer = GlobalLock( hBuffer );

	CopyMemory( pBuffer, pData, dwSize );
	GlobalUnlock( hBuffer );

	IStream* pStream = NULL;
	HRESULT hr = CreateStreamOnHGlobal( hBuffer, TRUE, &pStream );

	m_pImage = Gdiplus::Image::FromStream( pStream );
	pStream->Release();
	FreeResource(hRsc);
#endif

	m_blend.BlendOp = 0;
	m_blend.BlendFlags = 0;
	m_blend.AlphaFormat = 1;
	m_blend.SourceConstantAlpha = 255;//AC_SRC_ALPHA

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	UpdateView( );

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
// CDialogFrame 메시지 처리기입니다.

extern CBetaPatchClientDlg* g_pDlg;
int CDialogFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_pDlgPatch = new CBetaPatchClientDlg( this );
	if( NULL != m_pDlgPatch )
		m_pDlgPatch->ShowDialog( );

	g_pDlg = m_pDlgPatch;

	return 0;
}

void CDialogFrame::UpdateView()
{
	SIZE sizeWindow = { m_pImage->GetWidth(), m_pImage->GetHeight()};

	DWORD dwExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	HDC hDC = ::GetDC(m_hWnd);
	HDC hdcMemory = CreateCompatibleDC(hDC);

	HBITMAP hBitMap = CreateCompatibleBitmap(hDC, sizeWindow.cx, sizeWindow.cy);
	::SelectObject(hdcMemory, hBitMap);

	RECT rcWindow;
	GetWindowRect(&rcWindow);

	BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
	int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
	stBmpInfoHeader.biWidth = sizeWindow.cx;   
	stBmpInfoHeader.biHeight = sizeWindow.cy;   
	stBmpInfoHeader.biPlanes = 1;   
	stBmpInfoHeader.biBitCount = 32;   
	stBmpInfoHeader.biCompression = BI_RGB;   
	stBmpInfoHeader.biClrUsed = 0;   
	stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

	PVOID pvBits = NULL;   
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
	ASSERT(hbmpMem != NULL);
	memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);
	if(hbmpMem)   
	{   
		HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

		//POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
		POINT ptWinPos = { rcWindow.left, rcWindow.top };
		Gdiplus::Graphics graph(hdcMemory);
		graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

		graph.DrawImage(m_pImage, 0, 0, sizeWindow.cx, sizeWindow.cy);

		//TranslucentWndList::const_iterator it;
		//for (it = m_translucentWndList.begin(); it != m_translucentWndList.end(); ++it)
		//{
		//	CTranslucentWnd* translucentWnd = *it;
		//	if (translucentWnd->IsVisible())
		//		translucentWnd->Render(graph);
		//}

		HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
		typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
		MYFUNC UpdateLayeredWindow;
		UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
		POINT ptSrc = { 0, 0};

		UpdateLayeredWindow(m_hWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &m_blend, 2);

		graph.ReleaseHDC(hdcMemory);
		::SelectObject( hdcMemory, hbmpOld);   
		::DeleteObject(hbmpMem); 
	}

	::DeleteDC(hdcMemory);
	::DeleteDC(hDC);
}
HBRUSH CDialogFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if( nCtlColor == CTLCOLOR_DLG )
		return (HBRUSH)GetStockObject(NULL_BRUSH);

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CDialogFrame::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

BOOL CDialogFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialog::OnEraseBkgnd(pDC);

	return TRUE;
}

void CDialogFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	::SendMessage( GetSafeHwnd(), WM_SYSCOMMAND, 0xF012, 0);

	CDialog::OnLButtonDown(nFlags, point);
}

void CDialogFrame::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if( m_pDlgPatch )
	{
		CRect rect;
		m_pDlgPatch->GetWindowRect( &rect );
		m_pDlgPatch->MoveWindow( x, y, rect.Width(), rect.Height(), TRUE );
	//	m_pDlgPatch->UpdateWindow();
	}
}
