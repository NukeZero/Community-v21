
#define _WIN32_WINNT 0x0501

#if	defined( _DLL ) && defined( _MT )
	#define	_AFXDLL
#endif
#include <cstdio>
#include <afx.h>


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Aeonsoft");
LPCTSTR lpszWindowName = TEXT("Updater");
CString g_strCmdLine;

void Update();
void ErrorMsg();

int __stdcall WinMain(HINSTANCE__* hInstance, HINSTANCE__*, char* const lpszCmdParam, int)
{
	TCHAR szArg1[64] = {0, }; 
	TCHAR szArg2[64] = {0, }; 
	TCHAR szArg3[64] = {0, }; 
	TCHAR szArg4[64] = {0, };

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( lpszCmdParam, "%s %s %s %s",
		szArg1, sizeof( szArg1 )/sizeof( szArg1[0] ),
		szArg2, sizeof( szArg2 )/sizeof( szArg2[0] ),
		szArg3, sizeof( szArg3 )/sizeof( szArg3[0] ),
		szArg4, sizeof( szArg4 )/sizeof( szArg4[0] ) );
#else
	sscanf( lpszCmdParam, "%s %s %s %s", szArg1, szArg2, szArg3, szArg4 );
#endif

	if( _stricmp( szArg1, "myunglang" ) != 0)
		return 0;

	g_strCmdLine.Format("%s %s %s", szArg2, szArg3, szArg4);

	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	
	WndClass.hCursor = LoadCursor(nullptr,IDC_ARROW);
	WndClass.hIcon = LoadIcon(nullptr,IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = nullptr;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	const HWND hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);
	hWndMain = hWnd;
	
	while(GetMessage(&Message,nullptr,0,0)) 
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(const HWND hWnd, const UINT iMessage, const WPARAM wParam, const LPARAM lParam)
{
	PAINTSTRUCT ps;


	DWORD ExitCode;

	switch(iMessage) 
	{
		case WM_USER + 2:
		{
			static HANDLE hProc = 0;
			static DWORD ProcID = 0;
			ProcID = wParam;
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcID);
			GetExitCodeProcess(hProc, &ExitCode);
			if (ExitCode != STILL_ACTIVE)
				MessageBox(hWndMain, "종료시킬 핸들 없음", "에러", MB_OK);
			else
			{
				TerminateProcess(hProc, 0);
				WaitForSingleObject(hProc, INFINITE);
				Sleep(1500);
				CloseHandle(hProc);
			}
			Update();
			return 0;
		}
		case WM_CREATE:
			return 0;
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default: return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	}

}

void Update()
{
	unsigned long dwFileAttributes = GetFileAttributes( "NewFlyff.exe" );
	if( 0xFFFFFFFF != dwFileAttributes )
	{
		dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributes("NewFlyff.exe", dwFileAttributes );
	}
	dwFileAttributes = GetFileAttributes( "Flyff.exe" );
	if( 0xFFFFFFFF != dwFileAttributes )
	{
		dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributes( "Flyff.exe", dwFileAttributes );
	}

	void* hNewFile = CreateFile("NewFlyff.exe", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, nullptr);

	if( INVALID_HANDLE_VALUE == hNewFile )
	{
		ErrorMsg();
		PostQuitMessage( 0 );
		return;
	}
	CloseHandle( hNewFile );

	if( FALSE == DeleteFile( "Flyff.exe" ) )
	{
		ErrorMsg();
		MessageBox( hWndMain, "Flyff.exe", "에러", MB_OK );
		PostQuitMessage( 0 );
		return;
	}

	if( 0 != rename( "NewFlyff.exe", "Flyff.exe" ) )
	{
		ErrorMsg();
		PostQuitMessage( 0 );
		return;
	}

	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;

	CString strCmd;
	strCmd.Format("Flyff.exe %s", g_strCmdLine.GetString());

	if (!CreateProcess(nullptr, strCmd.GetBuffer(0), nullptr, nullptr, FALSE, 0, nullptr,
		nullptr, &si, &pi ) )
	{
		ErrorMsg();
		PostQuitMessage( 0 );
		return;
	}
	PostQuitMessage( 0 );
}

void ErrorMsg()
{
	char szMsg[1024];
	char szErrorMsg[1024];

	const DWORD dwErr = GetLastError();
	
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, nullptr, dwErr, 0, szErrorMsg, 1024, nullptr );
	wsprintf( szMsg, "에러코드 : %d, 메시지 : %s", dwErr, szErrorMsg );
	MessageBox( hWndMain, szMsg, "에러", MB_OK );
}