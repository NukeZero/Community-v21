// SetSfxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "SetSfxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetSfxDlg dialog


CSetSfxDlg::CSetSfxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetSfxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetSfxDlg)
	m_cbvKey = _T("");
	m_cbvObj = _T("");
	//}}AFX_DATA_INIT

	m_bIsLoop = false;
	m_nHotKey = 0;
	m_nObjID  = 0;

}


// ÇÔ¼ö 
void CSetSfxDlg::SetValue(bool bIsLoop, int nHotKey, int nObjID)
{

	m_bIsLoop = bIsLoop;
	m_nHotKey = nHotKey;
	m_nObjID  = nObjID;

}


bool  CSetSfxDlg::GetIsLoop()
{
	return m_bIsLoop;
}


int	 CSetSfxDlg::GetHotKey()
{
	return m_nHotKey;
}


int  CSetSfxDlg::GetObjNum()
{
	return m_nObjID;
}


void CSetSfxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetSfxDlg)
	DDX_Control(pDX, IDC_FOBJ, m_CtrlObj);
	DDX_Control(pDX, IDC_HOTKEY, m_CtrlKey);
	DDX_CBString(pDX, IDC_HOTKEY, m_cbvKey);
	DDX_CBString(pDX, IDC_FOBJ, m_cbvObj);
	//}}AFX_DATA_MAP
}

BOOL CSetSfxDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char		temp[20];


	// TODO: Add extra initialization here
	for(int i = 0; i < 10; i++)
	{
		sprintf(temp, "%d", i);
		m_CtrlObj.AddString(temp);
		m_CtrlKey.AddString(temp);
	}
	
	m_CtrlObj.SetCurSel(0);
	m_CtrlKey.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



BEGIN_MESSAGE_MAP(CSetSfxDlg, CDialog)
	//{{AFX_MSG_MAP(CSetSfxDlg)
	ON_BN_CLICKED(IDC_LOOPF, OnLoopf)
	ON_BN_CLICKED(IDC_LOOPT, OnLoopt)
	ON_CBN_SELCHANGE(IDC_FOBJ, OnSelchangeFobj)
	ON_CBN_SELCHANGE(IDC_HOTKEY, OnSelchangeHotkey)
	ON_CBN_SELENDOK(IDC_HOTKEY, OnSelendokHotkey)
	ON_CBN_EDITUPDATE(IDC_HOTKEY, OnEditupdateHotkey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetSfxDlg message handlers

void CSetSfxDlg::OnLoopf() 
{
	// TODO: Add your control notification handler code here
	m_bIsLoop = false;
}

void CSetSfxDlg::OnLoopt() 
{
	// TODO: Add your control notification handler code here
	m_bIsLoop = true;
}



void CSetSfxDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}


void CSetSfxDlg::OnSelchangeFobj() 
{

	// TODO: Add your control notification handler code here
	m_CtrlObj.SetCurSel(m_CtrlObj.GetCurSel());
	UpdateData((TRUE));
	m_nObjID = atoi(m_cbvObj);

}

void CSetSfxDlg::OnSelchangeHotkey() 
{

	// TODO: Add your control notification handler code here
	int		nTemp = 0;
	m_CtrlKey.SetCurSel(m_CtrlKey.GetCurSel());
	UpdateData((TRUE));
	switch(atoi(m_cbvKey))
	{
		case 1:
			m_nHotKey = '1';
			break;
		case 2:
			m_nHotKey = '2';
			break;
		case 3:
			m_nHotKey = '3';
			break;
		case 4:
			m_nHotKey = '4';
			break;
		case 5:
			m_nHotKey = '5';
			break;
		case 6:
			m_nHotKey = '6';
			break;
		case 7:
			m_nHotKey = '7';
			break;
		case 8:
			m_nHotKey = '8';
			break;
		case 9:
			m_nHotKey = '9';
			break;
	}
	

}

void CSetSfxDlg::OnSelendokHotkey() 
{
	// TODO: Add your control notification handler code here
}

void CSetSfxDlg::OnEditupdateHotkey() 
{
	// TODO: Add your control notification handler code here
}
