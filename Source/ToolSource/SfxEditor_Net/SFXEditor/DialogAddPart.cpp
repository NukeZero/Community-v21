// DialogAddPart.cpp : implementation file
//

#include "stdafx.h"
#include "sfxeditor.h"
#include "DialogAddPart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAddPart dialog


CDialogAddPart::CDialogAddPart(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAddPart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAddPart)
	m_StrName = _T("");
	//}}AFX_DATA_INIT
}

void CDialogAddPart::UpdateRadio(void)
{
	if(m_nType==SFXPARTTYPE_BILL) {
		((CButton*)GetDlgItem(IDC_RADIO_DAP_BILL))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_PARTICLE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_MESH))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_CUSTOMMESH))->SetCheck(BST_UNCHECKED);
	}
	else if(m_nType==SFXPARTTYPE_PARTICLE) {
		((CButton*)GetDlgItem(IDC_RADIO_DAP_BILL))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_PARTICLE))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_MESH))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_CUSTOMMESH))->SetCheck(BST_UNCHECKED);
	}
	else if(m_nType==SFXPARTTYPE_MESH) {
		((CButton*)GetDlgItem(IDC_RADIO_DAP_BILL))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_PARTICLE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_MESH))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_CUSTOMMESH))->SetCheck(BST_UNCHECKED);
	}
	else if(m_nType==SFXPARTTYPE_CUSTOMMESH) {
		((CButton*)GetDlgItem(IDC_RADIO_DAP_BILL))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_PARTICLE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_MESH))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_DAP_CUSTOMMESH))->SetCheck(BST_CHECKED);
	}
}

void CDialogAddPart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAddPart)
	DDX_Text(pDX, IDC_EDIT1, m_StrName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAddPart, CDialog)
	//{{AFX_MSG_MAP(CDialogAddPart)
	ON_BN_CLICKED(IDC_RADIO_DAP_BILL, OnRadioDapBill)
	ON_BN_CLICKED(IDC_RADIO_DAP_CUSTOMMESH, OnRadioDapCustommesh)
	ON_BN_CLICKED(IDC_RADIO_DAP_MESH, OnRadioDapMesh)
	ON_BN_CLICKED(IDC_RADIO_DAP_PARTICLE, OnRadioDapParticle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAddPart message handlers

void CDialogAddPart::OnRadioDapBill() 
{
	// TODO: Add your control notification handler code here
	m_nType=SFXPARTTYPE_BILL;
	UpdateRadio();
}

void CDialogAddPart::OnRadioDapCustommesh() 
{
	// TODO: Add your control notification handler code here
	m_nType=SFXPARTTYPE_CUSTOMMESH;
	UpdateRadio();
}

void CDialogAddPart::OnRadioDapMesh() 
{
	// TODO: Add your control notification handler code here
	m_nType=SFXPARTTYPE_MESH;
	UpdateRadio();
}

void CDialogAddPart::OnRadioDapParticle() 
{
	// TODO: Add your control notification handler code here
	m_nType=SFXPARTTYPE_PARTICLE;
	UpdateRadio();
}

BOOL CDialogAddPart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nType=SFXPARTTYPE_BILL;
	UpdateRadio();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogAddPart::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if( m_StrName.IsEmpty() )
	{
		MessageBox( "레이어 이름을 넣어주세요" );
		return;
	}
	
	CDialog::OnOK();
}
