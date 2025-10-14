// CDlgWorldLight.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgWorldLight.h"
#include "WorldDoc.h"
#include "WorldView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWorldLight dialog


CDlgWorldLight::CDlgWorldLight(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWorldLight::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWorldLight)
	m_fAmbientB = 1.0f;
	m_fAmbientG = 1.0f;
	m_fAmbientR = 1.0f;
	m_fDiffuseB = 1.0f;
	m_fDiffuseG = 1.0f;
	m_fDiffuseR = 1.0f;
	m_nLightType = 0;
	m_nHour = 12;
	//}}AFX_DATA_INIT
	m_pWorld = NULL;
	m_pWorldView = NULL;
}
void CDlgWorldLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWorldLight)
	DDX_Text(pDX, IDC_AMBIENT_B, m_fAmbientB);
	DDX_Text(pDX, IDC_AMBIENT_G, m_fAmbientG);
	DDX_Text(pDX, IDC_AMBIENT_R, m_fAmbientR);
	DDX_Text(pDX, IDC_DIFFUSE_B, m_fDiffuseB);
	DDX_Text(pDX, IDC_DIFFUSE_G, m_fDiffuseG);
	DDX_Text(pDX, IDC_DIFFUSE_R, m_fDiffuseR);
	DDX_Radio(pDX, IDC_RADIO1, m_nLightType);
	DDX_Text(pDX, IDC_HOUR, m_nHour);
	DDV_MinMaxInt(pDX, m_nHour, 0, 23);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWorldLight, CDialog)
	//{{AFX_MSG_MAP(CDlgWorldLight)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWorldLight message handlers

void CDlgWorldLight::OnApply() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	m_pWorld->m_nLightType = m_nLightType;
	m_pWorld->m_nLightHour = m_nHour;
	m_pWorld->m_lightDirect.r1 = m_fDiffuseR;
	m_pWorld->m_lightDirect.g1 = m_fDiffuseG;
	m_pWorld->m_lightDirect.b1 = m_fDiffuseB;
	m_pWorld->m_lightDirect.r2 = m_fAmbientR;
	m_pWorld->m_lightDirect.g2 = m_fAmbientG;
	m_pWorld->m_lightDirect.b2 = m_fAmbientB;
	m_pWorldView->Invalidate( FALSE );
}

void CDlgWorldLight::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData( TRUE );
	m_pWorld->m_nLightType = m_nLightType;
	m_pWorld->m_nLightHour = m_nHour;
	m_pWorld->m_lightDirect.r1 = m_fDiffuseR;
	m_pWorld->m_lightDirect.g1 = m_fDiffuseG;
	m_pWorld->m_lightDirect.b1 = m_fDiffuseB;
	m_pWorld->m_lightDirect.r2 = m_fAmbientR;
	m_pWorld->m_lightDirect.g2 = m_fAmbientG;
	m_pWorld->m_lightDirect.b2 = m_fAmbientB;
	m_pWorldView->Invalidate( FALSE );
	
	CDialog::OnOK();
}
