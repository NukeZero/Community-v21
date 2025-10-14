// sExt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "sExt.h"
#include "sExtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSExtApp

BEGIN_MESSAGE_MAP(CSExtApp, CWinApp)
	//{{AFX_MSG_MAP(CSExtApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSExtApp construction

CSExtApp::CSExtApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSExtApp object

CSExtApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSExtApp initialization

BOOL CSExtApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

	CSExtDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
