// Beast.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Beast.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "WorldDoc.h"
#include "WorldView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBeastApp

BEGIN_MESSAGE_MAP(CBeastApp, CWinApp)
	//{{AFX_MSG_MAP(CBeastApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN_PROJECT, OnFileOpenProject)
	ON_COMMAND(ID_FILE_NEW_PROJECT, OnFileNewProject)
	//ON_COMMAND(ID_FILE_SAVE_PROJECT, OnFileSaveProject)
	ON_COMMAND(ID_FILE_CLOSE_PROJECT, OnFileCloseProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_PROJECT, OnUpdateFileCloseProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_PROJECT, OnUpdateFileOpenProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_PROJECT, OnUpdateFileNewProject)
	//ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROJECT, OnUpdateFileSaveProject)
	ON_COMMAND(ID_FILE_OPEN_WORLD, OnFileOpenWorld)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_WORLD, OnUpdateFileOpenWorld)
	ON_COMMAND(ID_FILE_NEW_WORLD, OnFileNewWorld)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_WORLD, OnUpdateFileNewWorld)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeastApp construction

CBeastApp::CBeastApp()
{
	//_crtBreakAlloc = 70;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CBeastApp::~CBeastApp( )
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBeastApp object

CBeastApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBeastApp initialization

BOOL CBeastApp::InitInstance()
{
	AfxEnableControlContainer();

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_BEASTTYPE,
		RUNTIME_CLASS(CWorldDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CWorldView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

BOOL CBeastApp::ProcessShellCommand(CCommandLineInfo& rCmdInfo)
{
	BOOL bResult = TRUE;
	switch (rCmdInfo.m_nShellCommand)
	{
	case CCommandLineInfo::FileNew:
		if ( !prj.OpenProject( "masquerade.prj" ) )
			bResult = FALSE;
		g_pMainFrame->SetTitle( "masquerade.prj" );
		g_pMainFrame->SetWindowText( "masquerade.prj" );
		g_pMainFrame->OpenWorkSpace();
		break;

		// If we've been asked to open a file, call OpenDocumentFile()

	case CCommandLineInfo::FileOpen:
		if (!prj.OpenProject(rCmdInfo.m_strFileName.LockBuffer()))
			bResult = FALSE;
		g_pMainFrame->SetTitle( rCmdInfo.m_strFileName );
		g_pMainFrame->SetWindowText( rCmdInfo.m_strFileName );
		g_pMainFrame->OpenWorkSpace();
		break;

		// If the user wanted to print, hide our main window and
		// fire a message to ourselves to start the printing

	case CCommandLineInfo::FilePrintTo:
	case CCommandLineInfo::FilePrint:
		m_nCmdShow = SW_HIDE;
		ASSERT(m_pCmdInfo == NULL);
		OpenDocumentFile(rCmdInfo.m_strFileName);
		m_pCmdInfo = &rCmdInfo;
		m_pMainWnd->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT);
		m_pCmdInfo = NULL;
		bResult = FALSE;
		break;

		// If we're doing DDE, hide ourselves

	case CCommandLineInfo::FileDDE:
		m_pCmdInfo = (CCommandLineInfo*)m_nCmdShow;
		m_nCmdShow = SW_HIDE;
		break;

	// If we've been asked to unregister, unregister and then terminate
	case CCommandLineInfo::AppUnregister:
		{
			UnregisterShellFileTypes();
			BOOL bUnregistered = Unregister();

			// if you specify /EMBEDDED, we won't make an success/failure box
			// this use of /EMBEDDED is not related to OLE

			if (!rCmdInfo.m_bRunEmbedded)
			{
				if (bUnregistered)
					AfxMessageBox(AFX_IDP_UNREG_DONE);
				else
					AfxMessageBox(AFX_IDP_UNREG_FAILURE);
			}
			bResult = FALSE;    // that's all we do

			// If nobody is using it already, we can use it.
			// We'll flag that we're unregistering and not save our state
			// on the way out. This new object gets deleted by the
			// app object destructor.

			if (m_pCmdInfo == NULL)
			{
				m_pCmdInfo = new CCommandLineInfo;
				m_pCmdInfo->m_nShellCommand = CCommandLineInfo::AppUnregister;
			}
		}
		break;
	}
	return bResult;
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
void CBeastApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CBeastApp message handlers

void CBeastApp::OnFileOpenProject() 
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Beastprj file (*.brj)|*.brj||");
	ShowWindow(dlg.m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(dlg.m_hWnd);
	if(dlg.DoModal()!=IDOK) 
		return;
	g_pMainFrame->SetTitle( dlg.GetFileName() );
	g_pMainFrame->SetWindowText( dlg.GetFileName() );
	/*
	if(prj.IsOpen())
	{
		int nResult = IDCANCEL;
		if(prj.IsModified())
		{
			nResult = g_pMainFrame->MessageBox("프로젝트를 세이브 하시겠습니까?","경고!",MB_ICONEXCLAMATION|MB_YESNOCANCEL);
			if(nResult == IDYES)
			{
				g_pMainFrame->m_workSpaceDlg.WriteFieldTreeToPrj();
				prj.SaveProject();
			}
			if(nResult == IDCANCEL)
				return;
		}
		m_strOpenPrjFileName = dlg.GetFileName();
		m_nOpenPrjWnds     = 0            ;
		m_bNowClosePrjWnds = TRUE         ;
		m_nClosePrjCmd     = CPC_OPEN_PRJ ;
		g_pMainFrame->SendMessageToDescendants(WM_CLOSE_PROJECT);
		return;
	}
	*/
	//prj.SetDynamicLoadTile(FALSE);
	prj.OpenProject( dlg.GetFileName().LockBuffer() );//E);
	//g_pMainFrame->OpenWorkSpace(); 
	//CAnim::m_pPaletteSpr 
	//	= CSprPack::Load("Sprite\\palette.spr");

}
void CBeastApp::OnUpdateFileOpenProject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !prj.IsOpenProject() );
	
}
void CBeastApp::OnFileNewProject() 
{
	
}
void CBeastApp::OnUpdateFileNewProject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !prj.IsOpenProject() );
	
}
/*
void CBeastApp::OnFileSaveProject() 
{
	
}
void CBeastApp::OnUpdateFileSaveProject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( prj.IsOpenProject() );
}
*/
void CBeastApp::OnFileCloseProject() 
{ 
	
}
void CBeastApp::OnUpdateFileCloseProject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( prj.IsOpenProject() );
}


void CBeastApp::OnFileOpenWorld() 
{
	CFileDlgWorld dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"World files (*.wld)|*.wld||");
		//"World files (*.wld;*.bmp)|*.wld;*.bmp||");
	if(dlg.DoModal()!=IDOK) 
		return;
	CWorldDoc* pWorldDoc = (CWorldDoc*)OpenDocumentFile( dlg.GetPathName()/*dlg.GetFileName()*/ );	
	if( dlg.m_bCheck ) pWorldDoc->ReadWorld();
}

void CBeastApp::OnUpdateFileOpenWorld(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( prj.IsOpenProject() );
}


void CBeastApp::OnFileNewWorld() 
{
	OnFileNew();
}

void CBeastApp::OnUpdateFileNewWorld(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( prj.IsOpenProject() );
}
