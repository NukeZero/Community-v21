// FileDlgWorld.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "FileDlgWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDlgWorld

IMPLEMENT_DYNAMIC(CFileDlgWorld, CFileDialog)

CFileDlgWorld::CFileDlgWorld(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_bCheck = FALSE;
}


BEGIN_MESSAGE_MAP(CFileDlgWorld, CFileDialog)
	//{{AFX_MSG_MAP(CFileDlgWorld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CFileDlgWorld::OnInitDialog() 
{
	//CFileDialog::OnInitDialog();	
	
	// TODO: Add extra initialization here

	// This variable should be changed acording to your wishes
	// about the size of the finished dialog
	const UINT iExtraSize = 30;
	// Number of controls in the File Dialog
	const UINT nControls = 7;	

	// Get a pointer to the original dialog box.
	CWnd *wndDlg = GetParent();

	CRect rect;
	wndDlg->GetWindowRect(&rect);
	// Change the size
	wndDlg->SetWindowPos(NULL, 0, 0, 
						 rect.right - rect.left, 
						 rect.bottom - rect.top + iExtraSize, 
						 SWP_NOMOVE);
	/*
	CWnd* pWnd = GetParent();
	CRect rect;
	pWnd->GetClientRect( &rect );
	CRect rect2;
	pWnd->GetWindowRect( &rect2);

	MoveWindow( 0, 0, rect.Width(), rect.Height()  );
*/
	CFileDialog::OnInitDialog();	

	rect = CRect( 10, rect.Height() - 50, rect.Width() - 60, rect.Height() - 35 );
	if( m_bOpenFileDialog )
		m_check.Create( _T( "Load Full World" ), WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKBOX   , rect, wndDlg, 9999 );
	//else
	//	m_check.Create( _T( "Save Full World" ), WS_VISIBLE | BS_AUTOCHECKBOX | BS_CHECKBOX   , rect, wndDlg, 9999 );
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CFileDlgWorld::OnFileNameOK( )
{
	if( m_bOpenFileDialog )
		m_bCheck = m_check.GetCheck();
	return CFileDialog::OnFileNameOK();
}