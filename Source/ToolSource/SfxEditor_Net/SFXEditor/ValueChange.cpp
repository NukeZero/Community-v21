// ValueChange.cpp : implementation file
//

#include "stdafx.h"
#include "FramePanel.h"
#include "sfxeditor.h"
#include "ValueChange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CValueChange dialog
extern CFramePanel*   g_pFramePanel;


CValueChange::CValueChange(CWnd* pParent /*=NULL*/)
	: CDialog(CValueChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CValueChange)
	//}}AFX_DATA_INIT
}


void CValueChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CValueChange)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CValueChange, CDialog)
	//{{AFX_MSG_MAP(CValueChange)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	ON_BN_CLICKED(IDC_CHECK8, OnCheck8)
	ON_BN_CLICKED(IDC_CHECK9, OnCheck9)
	ON_BN_CLICKED(IDC_CHECK10, OnCheck10)
	ON_BN_CLICKED(IDC_CHECK11, OnCheck11)
	ON_BN_CLICKED(IDC_CHECK12, OnCheck12)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CValueChange message handlers

BOOL CValueChange::OnInitDialog() 
{
	CDialog::OnInitDialog();

	EnableControl( FALSE );

	((CButton*)GetDlgItem(IDC_X))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_Y))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_Z))->SetWindowText("0");
	
	((CButton*)GetDlgItem(IDC_PROTX))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_PROTY))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_PROTZ))->SetWindowText("0");
	
	((CButton*)GetDlgItem(IDC_SX))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_SY))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_SZ))->SetWindowText("0");
	
	((CButton*)GetDlgItem(IDC_ANGX))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_ANGY))->SetWindowText("0");
	((CButton*)GetDlgItem(IDC_ANGZ))->SetWindowText("0");

	Invalidate();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CValueChange::EnableControl(BOOL bFlag)
{
	(CButton*)GetDlgItem(IDC_X)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_Y)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_Z)->EnableWindow( bFlag );
	
	(CButton*)GetDlgItem(IDC_PROTX)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_PROTY)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_PROTZ)->EnableWindow( bFlag );
	
	(CButton*)GetDlgItem(IDC_SX)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_SY)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_SZ)->EnableWindow( bFlag );
	
	(CButton*)GetDlgItem(IDC_ANGX)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_ANGY)->EnableWindow( bFlag );
	(CButton*)GetDlgItem(IDC_ANGZ)->EnableWindow( bFlag );
}

void CValueChange::OnCheck1() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_X)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_X)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_X)->EnableWindow( FALSE );	
}

void CValueChange::OnCheck2() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_Y)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_Y)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_Y)->EnableWindow( FALSE );		
}

void CValueChange::OnCheck3() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_Z)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_Z)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_Z)->EnableWindow( FALSE );		
}

void CValueChange::OnCheck4() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_PROTX)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_PROTX)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_PROTX)->EnableWindow( FALSE );			
}

void CValueChange::OnCheck5() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_PROTY)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_PROTY)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_PROTY)->EnableWindow( FALSE );				
}

void CValueChange::OnCheck6() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_PROTZ)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_PROTZ)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_PROTZ)->EnableWindow( FALSE );					
}

void CValueChange::OnCheck7() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_SX)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_SX)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_SX)->EnableWindow( FALSE );					
	
}

void CValueChange::OnCheck8() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_SY)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_SY)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_SY)->EnableWindow( FALSE );					
	
}

void CValueChange::OnCheck9() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_SZ)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_SZ)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_SZ)->EnableWindow( FALSE );					
	
}

void CValueChange::OnCheck10() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_ANGX)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_ANGX)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_ANGX)->EnableWindow( FALSE );					
	
}

void CValueChange::OnCheck11() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_ANGY)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_ANGY)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_ANGY)->EnableWindow( FALSE );					
	
}

void CValueChange::OnCheck12() 
{
	DWORD dwStyle = (DWORD)(CButton*)GetDlgItem(IDC_ANGZ)->GetStyle();
	if( dwStyle & WS_DISABLED ) 
		(CButton*)GetDlgItem(IDC_ANGZ)->EnableWindow( TRUE );
	else
		(CButton*)GetDlgItem(IDC_ANGZ)->EnableWindow( FALSE );					
	
}

void CValueChange::OnOK() 
{
	// TODO: Add extra validation here
	for( int i =0; i<g_pFramePanel->m_SelectedKeys.GetSize(); i++ )
	{
		SfxKeyFrame* pKey=(SfxKeyFrame*)(g_pFramePanel->m_SelectedKeys.GetAt(i));

		DWORD dwStyle;
		CString pStr;

		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_X))->GetStyle();

		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_X))->GetWindowText( pStr );
			pKey->vPos.x = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_Y))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_Y))->GetWindowText( pStr );
			pKey->vPos.y = (float)( atoi(pStr.GetBuffer(0)) );
		}

		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_Z))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_Z))->GetWindowText( pStr );
			pKey->vPos.z = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_PROTX))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_PROTX))->GetWindowText( pStr );
			pKey->vPosRotate.x = (float)( atoi(pStr.GetBuffer(0)) );
		}

		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_PROTY))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_PROTY))->GetWindowText( pStr );
			pKey->vPosRotate.y = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_PROTZ))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_PROTZ))->GetWindowText( pStr );
			pKey->vPosRotate.z = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_SX))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_SX))->GetWindowText( pStr );
			pKey->vScale.x = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_SY))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_SY))->GetWindowText( pStr );
			pKey->vScale.y = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_SZ))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_SZ))->GetWindowText( pStr );
			pKey->vScale.z = (float)( atoi(pStr.GetBuffer(0)) );
		}

		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_ANGX))->GetStyle();

		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_ANGX))->GetWindowText( pStr );
			pKey->vRotate.x = (float)( atoi(pStr.GetBuffer(0)) );
		}

		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_ANGY))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_ANGY))->GetWindowText( pStr );
			pKey->vRotate.y = (float)( atoi(pStr.GetBuffer(0)) );
		}
		
		dwStyle = (DWORD)((CButton*)GetDlgItem(IDC_ANGZ))->GetStyle();
		
		if( !(dwStyle & WS_DISABLED) ) 
		{
			((CButton*)GetDlgItem(IDC_ANGZ))->GetWindowText( pStr );
			pKey->vRotate.z = (float)( atoi(pStr.GetBuffer(0)) );
		}
	}

	CDialog::OnOK();
}

BOOL CValueChange::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_RETURN )
			return FALSE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
