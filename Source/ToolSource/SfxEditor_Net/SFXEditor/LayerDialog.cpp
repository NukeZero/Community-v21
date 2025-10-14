// LayerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "sfxeditor.h"
#include "LayerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerDialog dialog


CLayerDialog::CLayerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayerDialog)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void CLayerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayerDialog)
	DDX_Text(pDX, IDC_EDIT1, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayerDialog, CDialog)
	//{{AFX_MSG_MAP(CLayerDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerDialog message handlers

void CLayerDialog::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}
