// FileEncryption.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <ShellAPI.h>
#include "FileEncryption.h"
#include "..\..\program\_CommonDefine\FLFileEncryption.h"
#define MAX_LOADSTRING 100

// 전역 변수입니다.
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.


//////////////////////////////////////////////////////////////////////////
HINSTANCE	g_Instance;

static HFONT	g_TextFont;

HWND	g_hWndLog;
HWND	g_hWndMain;
HWND	g_hWndStatus;
int		g_aWidths[7] = {0,};

#define ID_LOG_EDIT			0x0a
#define ID_STATUSBAR		0xf0
#define ID_TIMER			0xf3

#define MAX_LOG_LINE	50
//////////////////////////////////////////////////////////////////////////



// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FILEENCRYPTION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FILEENCRYPTION);



	DWORD	dwEncryptionResult	= g_xFileEncryption->EncryptionFile( lpCmdLine );

	if( dwEncryptionResult == FLFileEncryption::ENCTYPTION_SUCCESS )
	{
//		int	aaa = 0;
	}


	DWORD	dwDecryptionResult	= g_xFileEncryption->DecryptionFile( lpCmdLine );

	if( dwDecryptionResult == FLFileEncryption::ENCTYPTION_SUCCESS )
	{
//		int	aaa = 0;
	}

// 	if( g_xFileEncryption->EncryptionFile( lpCmdLine ) == true )
// 	{
// 		if( g_xFileEncryption->DecryptionFile( lpCmdLine ) == true )
// 		{
// 
// 		}
// 	}

	

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
// 함수: MyRegisterClass()
//
// 목적: 창 클래스를 등록합니다.
//
// 설명:
//
// Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
// 해당 코드가 Win32 시스템과 호환되도록
// 하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
// 해당 응용 프로그램에 연결된
// '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;


	g_Instance	= hInstance;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_FILEENCRYPTION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_FILEENCRYPTION;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
// 함수: InitInstance(HANDLE, int)
//
// 목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
// 설명:
//
// 이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
// 주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowEx( WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   SetWindowPos( hWnd, NULL, 200, 200, 600, 400, SWP_SHOWWINDOW );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void	PrintLog( const TCHAR *lpszFmt, ... )
{
	TCHAR szBuf[2048];
	va_list vargs;

	memset(szBuf,0,2048);
	va_start(vargs,lpszFmt);
	_vsnprintf(szBuf,2048, lpszFmt, vargs);
	va_end(vargs);

//	_stprintf( szBuf, "%s\r\n", szBuf );

// 	DWORD Len;
// 	while( Edit_GetLineCount(g_hWndLog) > MAX_LOG_LINE )
// 	{
// 		Len = Edit_LineLength(g_hWndLog, 0);
// //		SendMessage(g_hWndLog, EM_SETSEL, 0, Len+2);
// //		SendMessage(g_hWndLog, EM_REPLACESEL, 0, (LPARAM)"");
// 	}
// 
// 	Len = Edit_GetTextLength(g_hWndLog);
// //	SendMessage(g_hWndLog, EM_SETSEL, Len, Len);

	for( int i=0; i<5; ++i )
	{
		SendMessage(g_hWndLog, LB_INSERTSTRING, 0, (LPARAM)szBuf);
	}
}

//
// 함수: WndProc(HWND, unsigned, WORD, LONG)
//
// 목적: 주 창의 메시지를 처리합니다.
//
// WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
// WM_PAINT	- 주 창을 그립니다.
// WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static	BOOL	bTimer;
	static	int		nTime;
	static	int		nCharHeight;
	static	DWORD	dwStartTick;

	switch (message) 
	{
		case WM_CREATE:
			{
				int nChar;
				HDC hDC;
				HFONT OldFont;
				TEXTMETRIC tm;

				hDC = GetDC(hWnd);

				nChar = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
				g_TextFont = CreateFont(nChar, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
					OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_DONTCARE, "Fixedsys");
				OldFont = (HFONT)SelectObject(hDC, g_TextFont);
				GetTextMetrics(hDC, &tm);
				nCharHeight = tm.tmHeight + tm.tmExternalLeading;

				ReleaseDC(hWnd, hDC);


				//	Create Server Log Edit
				g_hWndLog = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES, "LISTBOX", "LOG",
					WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LBS_DISABLENOSCROLL,// | ES_WANTRETURN,
					0, 0, 0, 0, hWnd, (HMENU)ID_LOG_EDIT, g_Instance, NULL);

				SetWindowText(g_hWndLog, "");

				if(g_hWndLog == NULL)
				{
					DestroyWindow(g_hWndMain);
				}
				SendMessage(g_hWndLog, WM_SETFONT, (WPARAM)g_TextFont, 0);


				//	Create Server Status Bar
				g_hWndStatus = CreateWindowEx(0L, STATUSCLASSNAME, "",
					WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,
					0, 0, 0, 0, hWnd, (HMENU)ID_STATUSBAR, g_Instance, NULL);

				if( g_hWndStatus == NULL )
				{
					DestroyWindow( g_hWndMain );
				}

				SendMessage(g_hWndStatus, SB_SETPARTS, 5, (LPARAM)g_aWidths);
				SendMessage(g_hWndStatus, SB_SETTEXT, 0 | SBT_NOBORDERS, (LPARAM)(szTitle) );
				SendMessage(g_hWndStatus, SB_SETTEXT, 1, (LPARAM)"");
				SendMessage(g_hWndStatus, SB_SETTEXT, 2, (LPARAM)"");
				SendMessage(g_hWndStatus, SB_SETTEXT, 3, (LPARAM)"");
				SendMessage(g_hWndStatus, SB_SETTEXT, 4, (LPARAM)"");
				SendMessage(g_hWndStatus, SB_SETTEXT, 5, (LPARAM)"");


				if( SetTimer( hWnd, ID_TIMER, 1000, NULL ) == 0 )
				{
					bTimer = FALSE;
				}
				else
				{
					bTimer = TRUE;
				}

				nTime = 0;
			}
			break;
		case WM_COMMAND:
			{
				wmId    = LOWORD(wParam); 
				wmEvent = HIWORD(wParam); 
				// 메뉴의 선택 영역을 구문 분석합니다.
				switch (wmId)
				{
					case IDM_ABOUT:
						{
							DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
						}
						break;
					case IDM_EXIT:
						{
							DestroyWindow(hWnd);
						}
						break;
					case ID_LOG_EDIT:
						{
							switch(wmEvent)
							{
								case WM_DROPFILES:
									{
										MessageBox( NULL, "22222", "sdasdfas", MB_OK );
									}
								default:
									break;
							}
						}
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		case WM_DROPFILES :
			{
				HDROP	aaaa = (HDROP)( wParam );
				//				UINT	iFile;
				TCHAR	szFileName[MAX_PATH] = {0, };

				UINT	nCount	= 0;
				UINT	nLen	= 0;
				int		i=0;

				for( int i=0; i<4; ++i )
				{
					::memset( szFileName, 0, sizeof(szFileName) );
					nLen = DragQueryFile( aaaa, i, (LPTSTR)szFileName, sizeof(szFileName) );

					int c= 0;
				}


				int aaaaaa = 0;

			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;
		case WM_SIZE:
			{
				RECT	rc;
				int		nStatusGap;
				RECT	rcClient;
				int		cxSize, cySize;

				GetClientRect(g_hWndMain, &rc);

				if(g_hWndStatus != NULL) {
					RECT	rcStatus;

					MoveWindow(g_hWndStatus, 0, HIWORD(lParam)-10, LOWORD(lParam), HIWORD(lParam), TRUE);
					g_aWidths[0] = (LOWORD(lParam) *2) / 7;
					g_aWidths[1] = (LOWORD(lParam) *3) / 7;
					g_aWidths[2] = (LOWORD(lParam) *4) / 7;
					g_aWidths[3] = (LOWORD(lParam) *5) / 7;
					g_aWidths[4] = (LOWORD(lParam) *6) / 7;
					g_aWidths[5] = -1;
					SendMessage(g_hWndStatus, SB_SETPARTS, 6, (LPARAM)g_aWidths);

					GetClientRect(g_hWndStatus, &rcStatus);
					nStatusGap = rcStatus.bottom - rcStatus.top +1;
				}

				GetClientRect(hWnd, &rcClient);
				cxSize = rcClient.right - rcClient.left +1;
				cySize = rcClient.bottom - rcClient.top +1;

				if(g_hWndLog != NULL)
				{
					MoveWindow(g_hWndLog, 0, 0, cxSize, cySize-14, TRUE);
				}
			}
			break;
		case WM_TIMER:
			{
				SYSTEMTIME st;

				if( wParam == ID_TIMER )
				{
					char szTimeStr[24];

					GetLocalTime(&st);

					memset(szTimeStr, 0, sizeof(szTimeStr));
					wsprintf(szTimeStr, "%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);
					SendMessage(g_hWndStatus, SB_SETTEXT, 2, (LPARAM)szTimeStr);
					memset(szTimeStr, 0, sizeof(szTimeStr));
					wsprintf(szTimeStr, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
					SendMessage(g_hWndStatus, SB_SETTEXT, 3, (LPARAM)szTimeStr);

					int nSec, nMin, nHour, nDay;
					DWORD dwCurrent;

					SendMessage(g_hWndStatus, SB_SETTEXT, 1, (LPARAM)"On-Line");

					dwCurrent = GetTickCount() - dwStartTick;
					nSec = dwCurrent / 1000;
					nMin = nSec / 60;
					nSec = nSec % 60;
					nHour = nMin / 60;
					nMin = nMin % 60;
					nDay = nHour / 24;
					nHour = nHour % 24;

					wsprintf(szTimeStr, "%02d:%02d:%02d:%02d", nDay, nHour, nMin, nSec);
					SendMessage(g_hWndStatus, SB_SETTEXT, 4, (LPARAM)szTimeStr);
				}
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
