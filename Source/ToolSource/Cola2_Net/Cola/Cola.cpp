// Cola.cpp : Defines the class behaviors for the application.

//



#include "stdafx.h"

#include "Cola.h"



#include "MainFrm.h"

#include "ColaDoc.h"

#include "ColaView.h"



#include "scanner.h"



#include "..\_UnhandledException\ExceptionHandler.h"



CColaApp	*g_pApp = NULL;





/////////////////////////////////////////////////////////////////////////////

// CColaApp



BEGIN_MESSAGE_MAP(CColaApp, CWinApp)

	//{{AFX_MSG_MAP(CColaApp)

	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)

		// NOTE - the ClassWizard will add and remove mapping macros here.

		//    DO NOT EDIT what you see in these blocks of generated code!

	//}}AFX_MSG_MAP

	// Standard file based document commands

	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)

	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)

END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////

// CColaApp construction



CColaApp::CColaApp()

{

	// TODO: add construction code here,

	// Place all significant initialization in InitInstance

	g_pApp = this;



#ifndef _DEBUG

	InitEH();		// 익셉션 핸들러 초기화.

#endif

	

}





/////////////////////////////////////////////////////////////////////////////

// The one an only CColaApp object



CColaApp theApp;

	

/////////////////////////////////////////////////////////////////////////////

// CColaApp initialization



BOOL CColaApp::InitInstance()

{

	if( Convert() == TRUE )		// 커맨드라인 처리.  컨버트를 했으면 프로그램 종료

		return FALSE;



	InitLIB();



//	float fTest = 1.12345678f;

//	Message( "%4.7f", fTest );

	

	

	AfxEnableControlContainer();


	// Change the registry key under which our settings are stored.

	// TODO: You should modify this string to be something appropriate

	// such as the name of your company or organization.

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));



	LoadStdProfileSettings();  // Load standard INI file options (including MRU)



	// Register the application's document templates.  Document templates

	//  serve as the connection between documents, frame windows and views.



	CSingleDocTemplate* pDocTemplate;

	pDocTemplate = new CSingleDocTemplate(

		IDR_MAINFRAME,

		RUNTIME_CLASS(CColaDoc),

		RUNTIME_CLASS(CMainFrame),       // main SDI frame window

		RUNTIME_CLASS(CColaView));

	AddDocTemplate(pDocTemplate);



	// Parse command line for standard shell commands, DDE, file open

	CCommandLineInfo cmdInfo;

	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line

	if (!ProcessShellCommand(cmdInfo))

		return FALSE;



    if( !g_pView->IsReady() )

        return FALSE;



	// The one and only window has been initialized, so show and update it.

	m_pMainWnd->ShowWindow(SW_SHOW);

	m_pMainWnd->UpdateWindow();



//	SetFrameSkip( FALSE );



	return TRUE;

}





/////////////////////////////////////////////////////////////////////////////

// CAboutDlg dialog used for App About



class CAboutDlg : public CDialog

{

public:

	CAboutDlg();



// Dialog Data

	//{{AFX_DATA(CAboutDlg)

	enum { IDD = IDD_ABOUTBOX };

	//}}AFX_DATA



	// ClassWizard generated virtual function overrides

	//{{AFX_VIRTUAL(CAboutDlg)

	protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//}}AFX_VIRTUAL



// Implementation

protected:

	//{{AFX_MSG(CAboutDlg)

		// No message handlers

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};



CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)

{

	//{{AFX_DATA_INIT(CAboutDlg)

	//}}AFX_DATA_INIT

}



void CAboutDlg::DoDataExchange(CDataExchange* pDX)

{

	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CAboutDlg)

	//}}AFX_DATA_MAP

}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)

	//{{AFX_MSG_MAP(CAboutDlg)

		// No message handlers

	//}}AFX_MSG_MAP

END_MESSAGE_MAP()



// App command to run the dialog

void CColaApp::OnAppAbout()

{

	CAboutDlg aboutDlg;

	aboutDlg.DoModal();

}



/////////////////////////////////////////////////////////////////////////////

// CColaApp message handlers





BOOL CColaApp::OnIdle(LONG lCount) 

{

	// TODO: Add your specialized code here and/or call the base class

    // Do not render if the app is minimized

    if( m_pMainWnd->IsIconic() )

        return FALSE;



    // Update and render a frame

    if( g_pView->IsReady() )

    {

        g_pView->CheckForLostFullscreen();

        g_pView->RenderScene();

    }



    // Keep requesting more idle time

	CWinApp::OnIdle(lCount);

    return TRUE;

	

}



//

// 커맨드라인 방식의 컨버트기능 삭제함.

//

BOOL	CColaApp :: Convert( void )

{

	char	szASE[1024];

//	CScanner	scan;

//	MessageBox( NULL, m_lpCmdLine, NULL, MB_OK );

	memset( szASE, 0, 1024 );

	if( IsEmpty(m_lpCmdLine) == FALSE )

	{

		memcpy( szASE, m_lpCmdLine+1, strlen(m_lpCmdLine)-2 );		// "test.ase" 에서 따옴표 떼냄



		// 짧은파일명으로 읽일때가 있으므로 롱파일명으로 변환

		DWORD ret = GetLongPathName( szASE, szASE, MAX_PATH );

	}

//	Message( "%s", szASE );



//	Error( "%s\r\n%d %s %d", m_lpCmdLine, ret, szASE, GetLastError() );

//	MessageBox( NULL, szASE, NULL, MB_OK );



/*

	CAse	ase;

	CScanner	scan;

	char	szName[256];



	scan.SetProg( m_lpCmdLine );



	scan.GetToken();					// 첫번째 토큰 읽어봄

	strcpy( szASE, scan.token );		// "로 둘러쌓인 ase파일명을 일단 받고



	scan.GetToken();

	if( strcmp(scan.token, "BON") == 0 )	// BON으로 시작하냐

	{

		ase.LoadModel( szASE );

		CGeoMObject	*pBiped = ase.GetBiped();

		scan.GetToken();

		strcpy( szName, scan.token );

		if( IsEmpty(szName) )

		{

			strcpy( szName, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

			strcat( szName, ".bon" );				// title + ".mes"

		}



		ase.SaveBON( szName );

//		pBiped->SaveBON( szName );

		return TRUE;

	} else

	if( strcmp(scan.token, "ANI") == 0 )	// ANI로 시작하냐

	{

		ase.LoadModel( szASE );

		CGeoMObject	*pBiped = ase.GetBiped();

		scan.GetToken();	// ani filename

		strcpy( szName, scan.token );

		if( IsEmpty(szName) )

		{

			strcpy( szName, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

			strcat( szName, ".ani" );				// title + ".mes"

		}

		pBiped->SaveANI( szName );

		return TRUE;

	} else

	if( strcmp(scan.token, "o3d") == 0 )	// MES로 시작하냐

	{

		ase.LoadModel( szASE );



//		char strTemp[256];

//		strcpy( strTemp, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

//		strcat( strTemp, ".mes" );				// title + ".mes"

				

		if( ase.m_bBiped )

			ase.SaveBipedMesh( NULL );		// 오브젝트 이름으로 저장, 말머리 없음.

		else

		{

			scan.GetToken();	// mes name

			strcpy( szName, scan.token );

			if( IsEmpty(szName) )

			{

				strcpy( szName, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

				strcat( szName, ".mes" );				// title + ".mes"

			}

			ase.SaveNormMesh( szName );		// 일반오브젝트는 지정된 이름으로 저장

		}

		return  TRUE;

	}

*/	

	m_strCmdFile = szASE;		// cmd line으로 입력된 ase명을 받아둠.  View::OnInitial()에서 실제로 Load





	return FALSE;

}





/*

BOOL	CColaApp :: Convert( void )

{

	CAse	ase;

	CScanner	scan;

	char	szName[256];

	char	szASE[256];



	scan.SetProg( m_lpCmdLine );



	scan.GetToken();					// 첫번째 토큰 읽어봄

	strcpy( szASE, scan.token );		// "로 둘러쌓인 ase파일명을 일단 받고



	scan.GetToken();

	if( strcmp(scan.token, "BON") == 0 )	// BON으로 시작하냐

	{

		ase.LoadASE( szASE );

		CGeoMObject	*pBiped = ase.GetBiped();

		scan.GetToken();

		strcpy( szName, scan.token );

		if( IsEmpty(szName) )

		{

			strcpy( szName, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

			strcat( szName, ".bon" );				// title + ".mes"

		}



		ase.SaveBON( szName );

//		pBiped->SaveBON( szName );

		return TRUE;

	} else

	if( strcmp(scan.token, "ANI") == 0 )	// ANI로 시작하냐

	{

		ase.LoadASE( szASE );

		CGeoMObject	*pBiped = ase.GetBiped();

		scan.GetToken();	// ani filename

		strcpy( szName, scan.token );

		if( IsEmpty(szName) )

		{

			strcpy( szName, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

			strcat( szName, ".ani" );				// title + ".mes"

		}

		pBiped->SaveANI( szName );

		return TRUE;

	} else

	if( strcmp(scan.token, "o3d") == 0 )	// MES로 시작하냐

	{

		ase.LoadASE( szASE );



//		char strTemp[256];

//		strcpy( strTemp, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

//		strcat( strTemp, ".mes" );				// title + ".mes"

				

		if( ase.m_bBiped )

			ase.SaveBipedMesh( NULL );		// 오브젝트 이름으로 저장, 말머리 없음.

		else

		{

			scan.GetToken();	// mes name

			strcpy( szName, scan.token );

			if( IsEmpty(szName) )

			{

				strcpy( szName, GetFileTitle( szASE ) );		// ase name의 타이틀만 빼옴

				strcat( szName, ".mes" );				// title + ".mes"

			}

			ase.SaveNormMesh( szName );		// 일반오브젝트는 지정된 이름으로 저장

		}

		return  TRUE;

	}



	m_strCmdFile = szASE;		// cmd line으로 입력된 ase명을 받아둠.  View::OnInitial()에서 실제로 Load



	return FALSE;

}

*/









































