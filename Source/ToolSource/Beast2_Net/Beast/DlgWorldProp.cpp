// DlgWorldProp.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgWorldProp.h"
#include "WorldDoc.h"
#include "WorldView.h"
#include ".\dlgworldprop.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWorldProp dialog

CDlgWorldProp::CDlgWorldProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWorldProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWorldProp)
	m_bIndoor = FALSE;
	m_fFogStart = 0.0f;
	m_fFogEnd = 0.0f;
	m_fFogDensity = 0.0f;
	m_bFly = FALSE;
	m_dwDiffuseBlue = 0;
	m_dwDiffuseGreen = 0;
	m_dwDiffuseRed = 0;
	m_dwAmbientBlue = 0;
	m_dwAmbientRed = 0;
	m_dwAmbientGreen = 0;
	m_fLightDirX = 0.0f;
	m_fLightDirY = 0.0f;
	m_fLightDirZ = 0.0f;
	m_strSky1 = DEFAULT_FNAME;
	m_strSky2 = DEFAULT_FNAME;
	m_strSky3 = DEFAULT_FNAME;
	m_strSun = DEFAULT_FNAME;
	m_strMoon = DEFAULT_FNAME;
	m_strCloud1 = DEFAULT_FNAME;
	m_strCloud2 = DEFAULT_FNAME;
	m_strCloud3 = DEFAULT_FNAME;
	m_bUse24 = FALSE;
	m_strEditSeaCloud = DEFAULT_FNAME;
	//}}AFX_DATA_INIT
	m_pWorld = NULL;
}


void CDlgWorldProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWorldProp)
	DDX_Check(pDX, IDC_INDOOR, m_bIndoor);
	DDX_Text(pDX, IDC_FOG_START, m_fFogStart);
	DDX_Text(pDX, IDC_FOG_END, m_fFogEnd);
	DDX_Text(pDX, IDC_FOG_DENSITY, m_fFogDensity);
	DDX_Check(pDX, IDC_FLY, m_bFly);
	DDX_Text(pDX, IDC_DIFFUSE_BLUE, m_dwDiffuseBlue);
	DDV_MinMaxDWord(pDX, m_dwDiffuseBlue, 0, 255);
	DDX_Text(pDX, IDC_DIFFUSE_GREEN, m_dwDiffuseGreen);
	DDV_MinMaxDWord(pDX, m_dwDiffuseGreen, 0, 255);
	DDX_Text(pDX, IDC_DIFFUSE_RED, m_dwDiffuseRed);
	DDV_MinMaxDWord(pDX, m_dwDiffuseRed, 0, 255);
	DDX_Text(pDX, IDC_AMBIENT_BLUE, m_dwAmbientBlue);
	DDX_Text(pDX, IDC_AMBIENT_RED, m_dwAmbientRed);
	DDV_MinMaxDWord(pDX, m_dwAmbientRed, 0, 255);
	DDX_Text(pDX, IDC_AMBIENT_GREEN, m_dwAmbientGreen);
	DDV_MinMaxDWord(pDX, m_dwAmbientGreen, 0, 255);
	DDX_Text(pDX, IDC_LIGHT_DIR_X, m_fLightDirX);
	DDV_MinMaxFloat(pDX, m_fLightDirX, -1.f, 1.f);
	DDX_Text(pDX, IDC_LIGHT_DIR_Y, m_fLightDirY);
	DDV_MinMaxFloat(pDX, m_fLightDirY, -1.f, 1.f);
	DDX_Text(pDX, IDC_LIGHT_DIR_Z, m_fLightDirZ);
	DDV_MinMaxFloat(pDX, m_fLightDirZ, -1.f, 1.f);
	DDX_Text(pDX, IDC_EDIT_WP_SKY1, m_strSky1);
	DDX_Text(pDX, IDC_EDIT_WP_SKY2, m_strSky2);
	DDX_Text(pDX, IDC_EDIT_WP_SKY3, m_strSky3);
	DDX_Text(pDX, IDC_EDIT_WP_SUN, m_strSun);
	DDX_Text(pDX, IDC_EDIT_WP_MOON, m_strMoon);
	DDX_Text(pDX, IDC_EDIT_WP_CLOUD1, m_strCloud1);
	DDX_Text(pDX, IDC_EDIT_WP_CLOUD2, m_strCloud2);
	DDX_Text(pDX, IDC_EDIT_WP_CLOUD3, m_strCloud3);
	DDX_Check(pDX, IDC_BTN_USE24, m_bUse24);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_WP_SEACLOUD, m_strEditSeaCloud);
}


BEGIN_MESSAGE_MAP(CDlgWorldProp, CDialog)
	//{{AFX_MSG_MAP(CDlgWorldProp)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_PEACE, OnPeace)
	ON_BN_CLICKED(IDC_PENALTYPK, OnPenaltyPK)
	ON_BN_CLICKED(IDC_FREEPK, OnFreePK)
	ON_BN_CLICKED(IDC_BTN_USE24, OnBtnUse24)
	ON_BN_CLICKED(IDC_BTN_EDIT24, OnBtnEdit24)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWorldProp message handlers
#ifdef __N_TRIGGER_AREASET_050203


BOOL CDlgWorldProp::OnInitDialog() 
{
	m_nPKMode = m_pWorld->m_nPKMode;
	CButton* pButton;
	switch( m_nPKMode )
	{
	case RA_SAFETY:
		pButton = (CButton*)GetDlgItem( IDC_PEACE );
		pButton->SetCheck( TRUE );
		break;
	case RA_PENALTY_PK:
		pButton = (CButton*)GetDlgItem( IDC_PENALTYPK );
		pButton->SetCheck( TRUE );
		break;
	case RA_PK:
		pButton = (CButton*)GetDlgItem( IDC_FREEPK );
		pButton->SetCheck( TRUE );
		break;
	default:
		break;
	}

	m_strSky1 = m_pWorld->m_strSkyTex[0];
	m_strSky2 = m_pWorld->m_strSkyTex[1];
	m_strSky3 = m_pWorld->m_strSkyTex[2];

	m_strCloud1 = m_pWorld->m_strCloudTex[0];
	m_strCloud2 = m_pWorld->m_strCloudTex[1];
	m_strCloud3 = m_pWorld->m_strCloudTex[2];

	m_strSun = m_pWorld->m_strSunTex;
	m_strMoon = m_pWorld->m_strMoonTex;

	m_bUse24 = m_pWorld->m_bIsUsing24Light;
	m_strEditSeaCloud = m_pWorld->m_strSeaCloud;
	
	UpdateData(FALSE);

	OnBtnUse24( );
	
	return TRUE;
}


void CDlgWorldProp::OnPeace()
{
	m_nPKMode = RA_SAFETY;
	CButton* pButton = (CButton*)GetDlgItem( IDC_PEACE );
	pButton->SetCheck( TRUE );
	pButton = (CButton*)GetDlgItem( IDC_PENALTYPK );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_FREEPK );
	pButton->SetCheck( FALSE );
}

void CDlgWorldProp::OnPenaltyPK()
{
	m_nPKMode = RA_PENALTY_PK;
	CButton* pButton = (CButton*)GetDlgItem( IDC_PEACE );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_PENALTYPK );
	pButton->SetCheck( TRUE );
	pButton = (CButton*)GetDlgItem( IDC_FREEPK );
	pButton->SetCheck( FALSE );
}

void CDlgWorldProp::OnFreePK()
{
	m_nPKMode = RA_PK;
	CButton* pButton = (CButton*)GetDlgItem( IDC_PEACE );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_PENALTYPK );
	pButton->SetCheck( FALSE );
	pButton = (CButton*)GetDlgItem( IDC_FREEPK );
	pButton->SetCheck( TRUE );
}
#endif __N_TRIGGER_AREASET_050203
void CDlgWorldProp::OnApply() 
{
	// TODO: Add your control notification handler code here
	if( m_pWorld )
	{
		UpdateData( TRUE );
		m_pWorld->m_bFly           = m_bFly        ; 
		m_pWorld->m_bIsIndoor      = m_bIndoor     ; 
		m_pWorld->m_fFogDensity    = m_fFogDensity ; 
		m_pWorld->m_fFogStartValue = m_fFogStart   ; 
		m_pWorld->m_fFogEndValue   = m_fFogEnd     ; 
		m_pWorld->m_v3LightDir.x   = m_fLightDirX  ; 
		m_pWorld->m_v3LightDir.y   = m_fLightDirY  ; 
		m_pWorld->m_v3LightDir.z   = m_fLightDirZ  ; 
		m_pWorld->m_dwDiffuse  = ( m_dwDiffuseRed   << 16);
		m_pWorld->m_dwDiffuse |= ( m_dwDiffuseGreen << 8 );
		m_pWorld->m_dwDiffuse |= ( m_dwDiffuseBlue       );
		m_pWorld->m_dwAmbient  = ( m_dwAmbientRed   << 16);
		m_pWorld->m_dwAmbient |= ( m_dwAmbientGreen << 8 );
		m_pWorld->m_dwAmbient |= ( m_dwAmbientBlue       );
#ifdef __N_TRIGGER_AREASET_050203
		m_pWorld->m_nPKMode = m_nPKMode;
#endif // __N_TRIGGER_AREASET_050203

		
		m_pWorld->m_strSkyTex[0] = m_strSky1;
		m_pWorld->m_strSkyTex[1] = m_strSky2;
		m_pWorld->m_strSkyTex[2] = m_strSky3;

		m_pWorld->m_strCloudTex[0] = m_strCloud1;
		m_pWorld->m_strCloudTex[1] = m_strCloud2;
		m_pWorld->m_strCloudTex[2] = m_strCloud3;

		m_pWorld->m_strSunTex = m_strSun;
		m_pWorld->m_strMoonTex = m_strMoon;
		m_pWorld->m_strSeaCloud = m_strEditSeaCloud;

		if( m_strEditSeaCloud != "default" )
			LoadTextureFromRes( m_pWorld->m_pd3dDevice, MakePath( DIR_WEATHER, m_strEditSeaCloud ), &m_pWorld->m_pTextureSeaCloud );

		m_pWorldView->Invalidate( FALSE );
	}
}

void CDlgWorldProp::OnBtnUse24() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	m_pWorld->m_bIsUsing24Light = m_bUse24;
}

void CDlgWorldProp::OnBtnEdit24() 
{
	// TODO: Add your control notification handler code here

	if( m_bUse24 )
		g_kDlg24Light.ShowWindow( SW_SHOW );
	else
	{
		AfxMessageBox( "you must check button [use24] to work 24Light editing" );
	}
}

