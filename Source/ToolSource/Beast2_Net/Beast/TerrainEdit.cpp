// TerrainEdit.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "TerrainEdit.h"
#include "WorldDoc.h"
#include "WorldView.h"
#include "ModelView.h"
#include "mainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//	이것은 ModelView.cpp에 있는 내용이지만 이 소스코드를 컴파일할때 포함되는
//	내용은 아니므로 일단 추가 시켰다. (지저분하다.)


#define IMAGE_ROOT    0
#define IMAGE_GROUP   1
#define IMAGE_FOLDER  2
#define IMAGE_MODEL   3
#define IMAGE_MOTION  4 
#define IMAGE_TEXTURE 5

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainHeight dialog
IMPLEMENT_DYNCREATE(CPageTerrainHeight, CPropertyPage)

BOOL CPageTerrainHeight::m_bCheckFixedHeight = FALSE;
int CPageTerrainHeight::m_nRadiusHeight = 7;
int CPageTerrainHeight::m_nHardnessHeight = 5;
int CPageTerrainHeight::m_nHeightStyle = 0;
float CPageTerrainHeight::m_fFixedHeight = 0.0f;
int CPageTerrainHeight::m_nHeightAttribute = 0;

CPageTerrainHeight::CPageTerrainHeight() : CPropertyPage(CPageTerrainHeight::IDD)
{
	//{{AFX_DATA_INIT(CPageTerrainHeight)
	m_iMPU = MPU;
	//}}AFX_DATA_INIT
}


void CPageTerrainHeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTerrainHeight)
	DDX_Text(pDX, IDC_FIXED_HEIGHT, m_fFixedHeight);
	DDX_Text(pDX, IDC_EDIT_HEIGHT_MPU, m_iMPU);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTerrainHeight, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTerrainHeight)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_HEIGHT_RADIUS, OnReleasedcaptureHeightRadius)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_HEIGHT_MOUNTAIN, OnHeightMountain)
	ON_BN_CLICKED(IDC_HEIGHT_FLAT, OnHeightFlat)
	ON_BN_CLICKED(IDC_HEIGHT_SOFT, OnHeightSoft)
	ON_EN_CHANGE(IDC_FIXED_HEIGHT, OnChangeFixedHeight)
	ON_BN_CLICKED(IDC_CHECK_FIXED_HEIGHT, OnCheckFixedHeight)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_HEIGHT_HARDNESS, OnReleasedcaptureHeightHardness)
	ON_BN_CLICKED(IDC_HEIGHT_RANDOM, OnHeightRandom)
	ON_BN_CLICKED(IDC_HEIGHT_ATTRIBUTE, OnHeightAttribute)
	ON_BN_CLICKED(IDC_ATTR_NONE, OnAttrNone)
	ON_BN_CLICKED(IDC_ATTR_DIE, OnAttrDie)
	ON_BN_CLICKED(IDC_ATTR_NOFLY, OnAttrNofly)
	ON_BN_CLICKED(IDC_ATTR_NOWALK, OnAttrNowalk)
	ON_BN_CLICKED(IDC_ATTR_NOMOVE, OnAttrNomove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainHeight message handlers

BOOL CPageTerrainHeight::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	CButton* pHeightMountain = (CButton*) GetDlgItem( IDC_HEIGHT_MOUNTAIN );
	CButton* pHeightFlat = (CButton*) GetDlgItem( IDC_HEIGHT_FLAT );
	CButton* pHeightSoft = (CButton*) GetDlgItem( IDC_HEIGHT_SOFT );
	//CButton* pHeightHole = (CButton*) GetDlgItem( IDC_HEIGHT_HOLE );
	CButton* pHeightRandom = (CButton*) GetDlgItem( IDC_HEIGHT_RANDOM );
	CButton* pHeightAttribute = (CButton*) GetDlgItem( IDC_HEIGHT_ATTRIBUTE );
	CButton* pRadioHeightAttribute = (CButton*) GetDlgItem( IDC_ATTR_NONE );

	HICON hIcon1 = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE ( IDI_HEIGHT_MOUNTAIN )  );
	HICON hIcon2 = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE ( IDI_HEIGHT_FLAT )  );
	HICON hIcon3 = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE ( IDI_HEIGHT_SOFT )  );
	//HICON hIcon4 = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE ( IDI_HEIGHT_HOLE )  );
	HICON hIcon5 = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE ( IDI_HEIGHT_RANDOM )  );
	HICON hIcon6 = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE ( IDI_HEIGHT_ATTRIBUTE )  );
	
	//HBITMAP hBitmap = theApp.LoadBitmap( IDC_HEIGHT_MOUNTAIN ) ;

	pHeightMountain->SetIcon( hIcon1 );
	pHeightFlat->SetIcon( hIcon2 );
	pHeightSoft->SetIcon( hIcon3 );
	pHeightRandom->SetIcon( hIcon5 );
	pHeightAttribute->SetIcon( hIcon6 );
	//pHeightHole->SetIcon( hIcon4 );

	pHeightMountain->SetCheck( TRUE );
	pRadioHeightAttribute->SetCheck( TRUE );


	CEdit* pEditRadius  = ( CEdit* ) GetDlgItem( IDC_EDIT_HEIGHT_RADIUS );
	CEdit* pEditHardness  = ( CEdit* ) GetDlgItem( IDC_EDIT_HEIGHT_HARDNESS );
	CSliderCtrl* pSliderRadius = ( CSliderCtrl* ) GetDlgItem( IDC_HEIGHT_RADIUS );
	CSliderCtrl* pSliderHardness = ( CSliderCtrl* ) GetDlgItem( IDC_HEIGHT_HARDNESS );

	pSliderRadius->SetRangeMin( 1 );
	pSliderRadius->SetRangeMax( 40 );
	pSliderRadius->SetPos( m_nRadiusHeight );

	pSliderHardness->SetRangeMin( 1 );
	pSliderHardness->SetRangeMax( 40 );
	pSliderHardness->SetPos( m_nHardnessHeight );

	// 슬라이드 바 세팅 
	CString string;
	string.Format( "%d", pSliderRadius->GetPos() );
	pEditRadius->SetWindowText( string );

	string.Format( "%d", pSliderHardness->GetPos() );
	pEditHardness->SetWindowText( string );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageTerrainHeight::OnReleasedcaptureHeightRadius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEdit* pEditRadius = ( CEdit* ) GetDlgItem( IDC_EDIT_HEIGHT_RADIUS );
	CSliderCtrl* pSliderRadius = ( CSliderCtrl* ) GetDlgItem( IDC_HEIGHT_RADIUS );
	CEdit* pEditHardness = ( CEdit* ) GetDlgItem( IDC_EDIT_HEIGHT_HARDNESS );
	CSliderCtrl* pSliderHardness = ( CSliderCtrl* ) GetDlgItem( IDC_HEIGHT_HARDNESS );

	m_nRadiusHeight = pSliderRadius->GetPos();
	m_nHardnessHeight = pSliderHardness->GetPos();

	if( m_nHardnessHeight > m_nRadiusHeight )
	{
		m_nHardnessHeight = m_nRadiusHeight;
		pSliderHardness->SetPos( m_nHardnessHeight );
	}
	CString string;
	string.Format( "%d", m_nRadiusHeight );
	pEditRadius->SetWindowText( string );
	string.Format( "%d", m_nHardnessHeight );
	pEditHardness->SetWindowText( string );

	*pResult = 0;
}
void CPageTerrainHeight::OnReleasedcaptureHeightHardness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEdit* pEditRadius = ( CEdit* ) GetDlgItem( IDC_EDIT_HEIGHT_RADIUS );
	CSliderCtrl* pSliderRadius = ( CSliderCtrl* ) GetDlgItem( IDC_HEIGHT_RADIUS );
	CEdit* pEditHardness = ( CEdit* ) GetDlgItem( IDC_EDIT_HEIGHT_HARDNESS );
	CSliderCtrl* pSliderHardness = ( CSliderCtrl* ) GetDlgItem( IDC_HEIGHT_HARDNESS );

	m_nRadiusHeight = pSliderRadius->GetPos();
	m_nHardnessHeight = pSliderHardness->GetPos();
	if( m_nHardnessHeight > m_nRadiusHeight )
	{
		m_nHardnessHeight = m_nRadiusHeight;
		pSliderHardness->SetPos( m_nHardnessHeight );
	}
	CString string;
	string.Format( "%d", m_nRadiusHeight );
	pEditRadius->SetWindowText( string );
	string.Format( "%d", m_nHardnessHeight );
	pEditHardness->SetWindowText( string );

	*pResult = 0;
}


void CPageTerrainHeight::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWorldView::m_nTool = ID_TERRAIN_HEIGHT;	
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPageTerrainHeight::OnHeightMountain() 
{
	m_nHeightStyle = 0;
}

void CPageTerrainHeight::OnHeightFlat() 
{
	m_nHeightStyle = 1;
}

void CPageTerrainHeight::OnHeightSoft() 
{
	m_nHeightStyle = 2;
}
void CPageTerrainHeight::OnHeightRandom() 
{
	m_nHeightStyle = 3;	
}

void CPageTerrainHeight::OnHeightAttribute() 
{
	m_nHeightStyle = 4;
}

void CPageTerrainHeight::OnChangeFixedHeight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
}
void CPageTerrainHeight::OnCheckFixedHeight() 
{
	CButton* pButton = (CButton*) GetDlgItem( IDC_CHECK_FIXED_HEIGHT );
	if( pButton->GetCheck() )
		m_bCheckFixedHeight = TRUE;
	else
		m_bCheckFixedHeight = FALSE;
}

void CPageTerrainHeight::OnAttrNone() 
{	
	m_nHeightAttribute = HATTR_NONE;
}
void CPageTerrainHeight::OnAttrNowalk() 
{
	m_nHeightAttribute = HATTR_NOWALK;	
}
void CPageTerrainHeight::OnAttrNofly() 
{
	m_nHeightAttribute = HATTR_NOFLY;	
}
void CPageTerrainHeight::OnAttrNomove() 
{
	m_nHeightAttribute = HATTR_NOMOVE;	
}

void CPageTerrainHeight::OnAttrDie() 
{
	m_nHeightAttribute = HATTR_DIE;	
}

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainColor dialog

IMPLEMENT_DYNCREATE(CPageTerrainColor, CPropertyPage)

int CPageTerrainColor::m_nRadiusVColor = 4;
int CPageTerrainColor::m_nHardnessVColor = 2;
DWORD CPageTerrainColor::m_dwVertexColor = LANDTEX_NORMAL;

CPageTerrainColor::CPageTerrainColor() : CPropertyPage(CPageTerrainColor::IDD)
{
	//{{AFX_DATA_INIT(CPageTerrainColor)
	m_nColorRadius = 0;
	m_nColorSliderBlue = 0;
	m_nColorSliderGreen = 0;
	m_nColorSliderRed = 0;
	m_nColorSliderRadius = 0;
	m_nColorSliderShade = 0;
	//}}AFX_DATA_INIT
	m_nColorBlue = 0;
	m_nColorGreen = 0;
	m_nColorRed = 0;
	m_nColorShade = 0;
}


void CPageTerrainColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTerrainColor)
	DDX_Text(pDX, IDC_COLOR_EDIT_RADIUS, m_nColorRadius);
	DDX_Slider(pDX, IDC_COLOR_SLIDER_BLUE, m_nColorSliderBlue);
	DDX_Slider(pDX, IDC_COLOR_SLIDER_GREEN, m_nColorSliderGreen);
	DDX_Slider(pDX, IDC_COLOR_SLIDER_RED, m_nColorSliderRed);
	DDX_Slider(pDX, IDC_COLOR_SLIDER_RADIUS, m_nColorSliderRadius);
	DDX_Slider(pDX, IDC_COLOR_SLIDER_SHADE, m_nColorSliderShade);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTerrainColor, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTerrainColor)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_RADIUS, OnReleasedcaptureColorSliderRadius)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_SLIDER_RED, OnReleasedcaptureColorSliderRed)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_SLIDER_GREEN, OnReleasedcaptureColorSliderGreen)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_SLIDER_BLUE, OnReleasedcaptureColorSliderBlue)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_SLIDER_SHADE, OnReleasedcaptureColorSliderShade)
	ON_EN_CHANGE(IDC_COLOR_EDIT_BLUE, OnChangeColorEditBlue)
	ON_EN_CHANGE(IDC_COLOR_EDIT_GREEN, OnChangeColorEditGreen)
	ON_EN_CHANGE(IDC_COLOR_EDIT_RED, OnChangeColorEditRed)
	ON_BN_CLICKED(IDC_COLOR_DIALOG, OnVcolor)
	ON_EN_CHANGE(IDC_COLOR_EDIT_SHADE, OnChangeColorEditShade)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_SLIDER_HARDNESS, OnReleasedcaptureColorSliderHardness)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_COLOR_SLIDER_RADIUS, OnReleasedcaptureColorSliderRadius)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainColor message handlers
BOOL CPageTerrainColor::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	CEdit* pEditRadiusVColor    = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_RADIUS );
	CEdit* pEditHardnessVColor  = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_HARDNESS );

	CSliderCtrl* pSliderRadiusVColor   = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_RADIUS );
	CSliderCtrl* pSliderHardnessVColor = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_HARDNESS );
	CSliderCtrl* pSliderRed    = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_RED );
	CSliderCtrl* pSliderGreen  = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_GREEN );
	CSliderCtrl* pSliderBlue   = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_BLUE );
	CSliderCtrl* pSliderShade  = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_SHADE );

	pSliderRadiusVColor->SetRangeMin( 1 );
	pSliderRadiusVColor->SetRangeMax( 40 );
	pSliderRadiusVColor->SetPos( m_nRadiusVColor );
//	pSliderVColor->SetBuddy( pEditVColor );

	pSliderHardnessVColor->SetRangeMin( 1 );
	pSliderHardnessVColor->SetRangeMax( 40 );
	pSliderHardnessVColor->SetPos( m_nHardnessVColor );


	pSliderRed->SetRangeMin( 0 );
	pSliderRed->SetRangeMax( 255 );
	pSliderRed->SetPos( 127 );

	pSliderGreen->SetRangeMin( 0 );
	pSliderGreen->SetRangeMax( 255 );
	pSliderGreen->SetPos( 127 );

	pSliderBlue->SetRangeMin( 0 );
	pSliderBlue->SetRangeMax( 255 );
	pSliderBlue->SetPos( 127 );

	pSliderShade->SetRangeMin( 0 );
	pSliderShade->SetRangeMax( 255 );
	pSliderShade->SetPos( 127 );

	// 슬라이드 바 세팅 
	CString string;
	string.Format( "%d", pSliderRadiusVColor->GetPos() );
	pEditRadiusVColor->SetWindowText( string );
	string.Format( "%d", pSliderHardnessVColor->GetPos() );
	pEditHardnessVColor->SetWindowText( string );

	// 칼라 세팅 

	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_RED  );
	string.Format( "%d", ( m_dwVertexColor & 0x00ff0000 ) >> 16);
	pEdit->SetWindowText( string );

	pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_GREEN  );
	string.Format( "%d", ( m_dwVertexColor & 0x0000ff00 ) >> 8);
	pEdit->SetWindowText( string );

	pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_BLUE );
	string.Format( "%d", m_dwVertexColor & 0x000000ff );
	pEdit->SetWindowText( string );

	pEdit = (CEdit*)GetDlgItem( IDC_COLOR_EDIT_SHADE );
	string.Format( "%d", m_nColorShade );
	pEdit->SetWindowText( string );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPageTerrainColor::OnReleasedcaptureColorSliderRadius(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	CEdit* pEditRadius = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_RADIUS );
	CSliderCtrl* pSliderRadius = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_RADIUS );
	CEdit* pEditHardness = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_HARDNESS );
	CSliderCtrl* pSliderHardness = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_HARDNESS );

	m_nRadiusVColor = pSliderRadius->GetPos();
	m_nHardnessVColor = pSliderHardness->GetPos();
	if( m_nHardnessVColor > m_nRadiusVColor )
	{
		m_nHardnessVColor = m_nRadiusVColor;
		pSliderHardness->SetPos( m_nHardnessVColor );
	}
	CString string;
	string.Format( "%d", m_nRadiusVColor );
	pEditRadius->SetWindowText( string );
	string.Format( "%d", m_nHardnessVColor );
	pEditHardness->SetWindowText( string );

	*pResult = 0;
}
void CPageTerrainColor::OnReleasedcaptureColorSliderHardness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEdit* pEditRadius = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_RADIUS );
	CSliderCtrl* pSliderRadius = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_RADIUS );
	CEdit* pEditHardness = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_HARDNESS );
	CSliderCtrl* pSliderHardness = ( CSliderCtrl* ) GetDlgItem( IDC_COLOR_SLIDER_HARDNESS );

	m_nRadiusVColor = pSliderRadius->GetPos();
	m_nHardnessVColor = pSliderHardness->GetPos();
	if( m_nHardnessVColor > m_nRadiusVColor )
	{
		m_nHardnessVColor = m_nRadiusVColor;
		pSliderHardness->SetPos( m_nHardnessVColor );
	}
	CString string;
	string.Format( "%d", m_nRadiusVColor );
	pEditRadius->SetWindowText( string );
	string.Format( "%d", m_nHardnessVColor );
	pEditHardness->SetWindowText( string );
	
	*pResult = 0;
}

void CPageTerrainColor::OnReleasedcaptureColorSliderRed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );
	CEdit* pEdit = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_RED );
	CString string;
	string.Format( "%d", m_nColorSliderRed - 127 );
	pEdit->SetWindowText( string );
	m_dwVertexColor = ( m_dwVertexColor & 0xff00ffff ) | ( m_nColorSliderRed << 16 );
	UpdateData( FALSE);
//	//SetLandscapeTool( 2 );
	OnPaint();
	
	*pResult = 0;
}

void CPageTerrainColor::OnReleasedcaptureColorSliderGreen(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );
	CEdit* pEdit = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_GREEN );
	CString string;
	string.Format( "%d", m_nColorSliderGreen - 127 );
	pEdit->SetWindowText( string );
	m_dwVertexColor = ( m_dwVertexColor & 0xffff00ff ) | ( m_nColorSliderGreen << 8 );
	UpdateData( FALSE);
//	//SetLandscapeTool( 2 );
	OnPaint();

	*pResult = 0;
}

void CPageTerrainColor::OnReleasedcaptureColorSliderBlue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );
	CEdit* pEdit = ( CEdit* ) GetDlgItem( IDC_COLOR_EDIT_BLUE );
	CString string;
	string.Format( "%d", m_nColorSliderBlue - 127 );
	pEdit->SetWindowText( string );
	m_dwVertexColor = ( m_dwVertexColor & 0xffffff00 ) | ( m_nColorSliderBlue << 0 );
	UpdateData( FALSE);
//	//SetLandscapeTool( 2 );
	OnPaint();
		
	*pResult = 0;
}

void CPageTerrainColor::OnReleasedcaptureColorSliderShade(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );
	CEdit* pEdit = ( CEdit* ) GetDlgItem( IDC_EDIT_COLOR_SHADE );
	CString string;
	string.Format( "%d", m_nColorSliderShade - 127 );
	pEdit->SetWindowText( string );
	m_nColorShade = m_nColorSliderShade;

	UpdateData( FALSE );
	*pResult = 0;
	OnPaint();
}

void CPageTerrainColor::OnChangeColorEditBlue() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_BLUE );
	CString string;
	pEdit->GetWindowText( string );
	int nColor = atoi( string ) + 127;
	m_dwVertexColor = ( m_dwVertexColor & 0xffffff00 ) | nColor;
	m_nColorSliderBlue = nColor;
	UpdateData( FALSE );
	Invalidate( FALSE );
	OnPaint();
}

void CPageTerrainColor::OnChangeColorEditGreen() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_GREEN );
	CString string;
	pEdit->GetWindowText( string );
	int nColor = atoi( string ) + 127;
	m_dwVertexColor = ( m_dwVertexColor & 0xffff00ff ) | ( nColor << 8 );
	m_nColorSliderGreen = nColor;
	//SetLandscapeTool( 2 );
	UpdateData( FALSE );
	Invalidate( FALSE );
	OnPaint();
}


void CPageTerrainColor::OnChangeColorEditRed() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_RED );
	CString string;
	pEdit->GetWindowText( string );
	int nColor = atoi( string ) + 127;
	m_dwVertexColor = ( m_dwVertexColor & 0xff00ffff ) | ( nColor << 16 );
	m_nColorSliderRed = nColor;
	//SetLandscapeTool( 2 );
	UpdateData( FALSE );
	Invalidate( FALSE );
	OnPaint();
}

void CPageTerrainColor::OnChangeColorEditShade() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_COLOR_EDIT_SHADE );
	CString string;
	pEdit->GetWindowText( string );
	m_nColorShade = atoi( string ) + 127;
	m_nColorSliderShade = m_nColorShade;//127;
	m_dwVertexColor = ( m_dwVertexColor & 0xff000000 ) | ( m_nColorShade << 16 ) | ( m_nColorShade << 8 ) | m_nColorShade;
	UpdateData( FALSE );

	pEdit = (CEdit*)GetDlgItem( IDC_COLOR_EDIT_RED );
	string.Format( "%d", m_nColorShade - 127 );
	pEdit->SetWindowText( string );

	pEdit = (CEdit*)GetDlgItem( IDC_COLOR_EDIT_GREEN );
	string.Format( "%d", m_nColorShade - 127 );
	pEdit->SetWindowText( string );

	pEdit = (CEdit*)GetDlgItem( IDC_COLOR_EDIT_BLUE );
	string.Format( "%d", m_nColorShade - 127 );
	pEdit->SetWindowText( string );

	Invalidate( FALSE );	
	OnPaint();
	//SetLandscapeTool( 2 );
}

void CPageTerrainColor::OnVcolor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog colorDialog( m_dwVertexColor );
	if( colorDialog.DoModal() == IDOK )
	{
		m_dwVertexColor = colorDialog.GetColor();

		DWORD dwColor1 = ( m_dwVertexColor & 0x00ff0000 ) >> 16;
		DWORD dwColor2 = ( m_dwVertexColor & 0x0000ff00 ) >> 8;
		DWORD dwColor3 = ( m_dwVertexColor & 0x000000ff );

		m_dwVertexColor = D3DCOLOR_ARGB( 0, dwColor3, dwColor2, dwColor1 ); 

		// 칼라 세팅 
		CString string;
		CEdit* pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_RED  );
		string.Format( "%d", ( ( m_dwVertexColor & 0x00ff0000 ) >> 16 ) - 127 );
		pEdit->SetWindowText( string );

		pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_GREEN  );
		string.Format( "%d", ( ( m_dwVertexColor & 0x0000ff00 ) >> 8 ) - 127 );
		pEdit->SetWindowText( string );

		pEdit = (CEdit*)GetDlgItem( IDC_VCOLOR_BLUE );
		string.Format( "%d", ( m_dwVertexColor & 0x000000ff ) - 127  );
		pEdit->SetWindowText( string );
		Invalidate( FALSE );

		OnPaint();
	}
	//SetLandscapeTool( 2 );
}

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainTexture property page

IMPLEMENT_DYNCREATE(CPageTerrainTexture, CPropertyPage)

int CPageTerrainTexture::m_nHardnessTexture = 2;
int CPageTerrainTexture::m_nRadiusTexture = 4;
#ifdef __CSC_TERRAIN_OPACITY
int CPageTerrainTexture::m_nOpacityTexture = 255;
#endif //__CSC_TERRAIN_OPACITY
BOOL CPageTerrainTexture::m_nLandTexture = LANDTEX_NORMAL;

CPageTerrainTexture::CPageTerrainTexture() : CPropertyPage(CPageTerrainTexture::IDD)
{
	//{{AFX_DATA_INIT(CPageTerrainTexture)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

CPageTerrainTexture::~CPageTerrainTexture()
{
}

void CPageTerrainTexture::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTerrainTexture)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTerrainTexture, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTerrainTexture)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TEXTURE_RADIUS, OnReleasedcaptureTextureRadius)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TEXTURE_HARDNESS, OnReleasedcaptureTextureHardness)
#ifdef __CSC_TERRAIN_OPACITY
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TEXTURE_OPACITY, OnReleasedcaptureTextureOpacity)
	ON_WM_HSCROLL()
#endif //__CSC_TERRAIN_OPACITY
	ON_BN_CLICKED(IDC_LANDTEX_DETAIL, OnLandtexDetail)
	ON_BN_CLICKED(IDC_LANDTEX_NORMAL, OnLandtexNormal)
	ON_BN_CLICKED(IDC_LANDTEX_BLANK, OnLandtexBlank)
	ON_BN_CLICKED(IDC_LANDTEX_LANDMARK, OnLandtexLandmark)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainTexture message handlers

void CPageTerrainTexture::OnReleasedcaptureTextureRadius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEdit* pEditTextureRadius   = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_RADIUS );
	CEdit* pEditTextureHardness = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_HARDNESS );
	CSliderCtrl* pSliderTextureRadius   = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_RADIUS );
	CSliderCtrl* pSliderTextureHardness = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_HARDNESS );

	m_nRadiusTexture = pSliderTextureRadius->GetPos();
	m_nHardnessTexture = pSliderTextureHardness->GetPos();

	//pSliderTextureHardness->SetRangeMin( 1 );
	//pSliderTextureHardness->SetRangeMax( m_nRadiusTexture );
	//pSliderTextureHardness->Invalidate();

	if( m_nHardnessTexture > m_nRadiusTexture )
	{
		m_nHardnessTexture = m_nRadiusTexture;
		pSliderTextureHardness->SetPos( m_nHardnessTexture );
	}
	CString string;
	string.Format( "%d", pSliderTextureRadius->GetPos() );
	pEditTextureRadius->SetWindowText( string );

	string.Format( "%d", pSliderTextureHardness->GetPos() );
	pEditTextureHardness->SetWindowText( string );


//	//SetLandscapeTool( 1 );
	
	*pResult = 0;
}
void CPageTerrainTexture::OnReleasedcaptureTextureHardness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEdit* pEditTextureRadius   = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_RADIUS );
	CEdit* pEditTextureHardness = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_HARDNESS );
	CSliderCtrl* pSliderTextureRadius   = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_RADIUS );
	CSliderCtrl* pSliderTextureHardness = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_HARDNESS );

	m_nRadiusTexture = pSliderTextureRadius->GetPos();
	m_nHardnessTexture = pSliderTextureHardness->GetPos();

	if( m_nHardnessTexture > m_nRadiusTexture )
	{
		m_nHardnessTexture = m_nRadiusTexture;
		pSliderTextureHardness->SetPos( m_nHardnessTexture );
	}
	CString string;
	string.Format( "%d", pSliderTextureRadius->GetPos() );
	pEditTextureRadius->SetWindowText( string );

	string.Format( "%d", pSliderTextureHardness->GetPos() );
	pEditTextureHardness->SetWindowText( string );


/*

	CEdit* pEditTexture = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_HARDNESS );
	CSliderCtrl* pSliderTexture = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_HARDNESS );

	CString string;
	string.Format( "%d", pSliderTexture->GetPos() );
	pEditTexture->SetWindowText( string );

	m_nHardnessTexture = pSliderTexture->GetPos();
	*/
//	//SetLandscapeTool( 1 );
	
	*pResult = 0;
}

#ifdef __CSC_TERRAIN_OPACITY
void CPageTerrainTexture::ResetOpacityValue()
{
	CEdit* pEditTextureOpacity = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_OPACITY );
	CSliderCtrl* pSliderTextureOpacity = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_OPACITY );

	pSliderTextureOpacity->SetRangeMin( 0 );
	pSliderTextureOpacity->SetRangeMax( 255 );

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
		m_nOpacityTexture = pMainFrame->m_nTerrainTexOpacity;

	pSliderTextureOpacity->SetPos( m_nOpacityTexture );

	CString string;
	string.Format( "%d", pSliderTextureOpacity->GetPos() );
	pEditTextureOpacity->SetWindowText( string );
}

void CPageTerrainTexture::OnReleasedcaptureTextureOpacity(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEdit* pEditTextureOpacity = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_OPACITY );
	CSliderCtrl* pSliderTextureOpacity = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_OPACITY );

	m_nOpacityTexture = pSliderTextureOpacity->GetPos();

	CString string;
	string.Format( "%d", pSliderTextureOpacity->GetPos() );
	pEditTextureOpacity->SetWindowText( string );

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
		pMainFrame->m_nTerrainTexOpacity = m_nOpacityTexture;
}

void CPageTerrainTexture::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSliderCtrl = (CSliderCtrl*)pScrollBar;

	if(pSliderCtrl->GetDlgCtrlID() == IDC_TEXTURE_OPACITY)
	{
		CEdit* pEditTextureOpacity = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_OPACITY );
		CSliderCtrl* pSliderTextureOpacity = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_OPACITY );

		m_nOpacityTexture = pSliderTextureOpacity->GetPos();

		CString string;
		string.Format( "%d", pSliderTextureOpacity->GetPos() );
		pEditTextureOpacity->SetWindowText( string );

		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
		if(pMainFrame)
			pMainFrame->m_nTerrainTexOpacity = m_nOpacityTexture;		
	}
	else if(pSliderCtrl->GetDlgCtrlID() == IDC_TEXTURE_RADIUS)
	{
		CEdit* pEditTextureRadius   = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_RADIUS );
		CEdit* pEditTextureHardness = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_HARDNESS );
		CSliderCtrl* pSliderTextureRadius   = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_RADIUS );
		CSliderCtrl* pSliderTextureHardness = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_HARDNESS );

		m_nRadiusTexture = pSliderTextureRadius->GetPos();
		m_nHardnessTexture = pSliderTextureHardness->GetPos();

		if( m_nHardnessTexture > m_nRadiusTexture )
		{
			m_nHardnessTexture = m_nRadiusTexture;
			pSliderTextureHardness->SetPos( m_nHardnessTexture );
		}
		CString string;
		string.Format( "%d", pSliderTextureRadius->GetPos() );
		pEditTextureRadius->SetWindowText( string );

		string.Format( "%d", pSliderTextureHardness->GetPos() );
		pEditTextureHardness->SetWindowText( string );
	}
	else if(pSliderCtrl->GetDlgCtrlID() == IDC_TEXTURE_HARDNESS)
	{
		CEdit* pEditTextureRadius   = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_RADIUS );
		CEdit* pEditTextureHardness = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_HARDNESS );
		CSliderCtrl* pSliderTextureRadius   = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_RADIUS );
		CSliderCtrl* pSliderTextureHardness = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_HARDNESS );

		m_nRadiusTexture = pSliderTextureRadius->GetPos();
		m_nHardnessTexture = pSliderTextureHardness->GetPos();

		if( m_nHardnessTexture > m_nRadiusTexture )
		{
			m_nHardnessTexture = m_nRadiusTexture;
			pSliderTextureHardness->SetPos( m_nHardnessTexture );
		}
		CString string;
		string.Format( "%d", pSliderTextureRadius->GetPos() );
		pEditTextureRadius->SetWindowText( string );

		string.Format( "%d", pSliderTextureHardness->GetPos() );
		pEditTextureHardness->SetWindowText( string );
	}
}
#endif //__CSC_TERRAIN_OPACITY

void CPageTerrainTexture::OnLandtexDetail() 
{
	m_nLandTexture = LANDTEX_DETAIL;
}

void CPageTerrainTexture::OnLandtexNormal() 
{
	m_nLandTexture = LANDTEX_NORMAL;
}
void CPageTerrainTexture::OnLandtexBlank() 
{
	m_nLandTexture = LANDTEX_BLANK;
}

void CPageTerrainTexture::OnLandtexLandmark() 
{
	m_nLandTexture = LANDTEX_LANDMARK;
}

BOOL CPageTerrainTexture::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	CEdit* pEditTextureRadius   = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_RADIUS );
	CEdit* pEditTextureHardness = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_HARDNESS );

	CSliderCtrl* pSliderTextureRadius   = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_RADIUS );
	CSliderCtrl* pSliderTextureHardness = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_HARDNESS );

	pSliderTextureRadius->SetRangeMin( 1 );
	pSliderTextureRadius->SetRangeMax( 40 );
	pSliderTextureRadius->SetPos( m_nRadiusTexture );
//SliderHeight->SetBuddy( pEditHeight );

	pSliderTextureHardness->SetRangeMin( 1 );
	pSliderTextureHardness->SetRangeMax( 40 );
	pSliderTextureHardness->SetPos( m_nHardnessTexture );
//SliderHeight->SetBuddy( pEditHeight );

#ifdef __CSC_TERRAIN_OPACITY
	CEdit* pEditTextureOpacity = ( CEdit* ) GetDlgItem( IDC_TEXTURE_EDIT_OPACITY );
	CSliderCtrl* pSliderTextureOpacity = ( CSliderCtrl* ) GetDlgItem( IDC_TEXTURE_OPACITY );

	pSliderTextureOpacity->SetRangeMin( 0 );
	pSliderTextureOpacity->SetRangeMax( 255 );

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	if(pMainFrame)
		m_nOpacityTexture = pMainFrame->m_nTerrainTexOpacity;

	pSliderTextureOpacity->SetPos( m_nOpacityTexture );
#endif //__CSC_TERRAIN_OPACITY

	// 슬라이드 바 세팅 
	CString string;
	string.Format( "%d", pSliderTextureRadius->GetPos() );
	pEditTextureRadius->SetWindowText( string );
	string.Format( "%d", pSliderTextureHardness->GetPos() );
	pEditTextureHardness->SetWindowText( string );
#ifdef __CSC_TERRAIN_OPACITY
	string.Format( "%d", pSliderTextureOpacity->GetPos() );
	pEditTextureOpacity->SetWindowText( string );
#endif //__CSC_TERRAIN_OPACITY

	CButton* pRadio = (CButton*)GetDlgItem( IDC_LANDTEX_NORMAL );
	pRadio->SetCheck( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPageTerrainHeight::OnSetActive() 
{
	CWorldView::m_nTool = ID_TERRAIN_HEIGHT;	
	return CPropertyPage::OnSetActive();
}

BOOL CPageTerrainTexture::OnSetActive() 
{
	CWorldView::m_nTool = ID_TERRAIN_TEXTURE;
	return CPropertyPage::OnSetActive();
}

BOOL CPageTerrainColor::OnSetActive() 
{
	CWorldView::m_nTool = ID_TERRAIN_COLOR;	
	return CPropertyPage::OnSetActive();
}
BOOL CPageTerrainWater::OnSetActive() 
{
	CWorldView::m_nTool = ID_TERRAIN_WATER;
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CTerrainSheet

IMPLEMENT_DYNAMIC(CTerrainSheet, CPropertySheet)

CTerrainSheet::CTerrainSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTerrainSheet::CTerrainSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CTerrainSheet::~CTerrainSheet()
{
}


BEGIN_MESSAGE_MAP(CTerrainSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTerrainSheet)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainSheet message handlers

void CTerrainSheet::OnSetFocus(CWnd* pOldWnd) 
{
	CPropertySheet::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
}

void CTerrainSheet::OnKillFocus(CWnd* pNewWnd) 
{
	CPropertySheet::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}

void CTerrainSheet::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertySheet::OnLButtonDown(nFlags, point);
}




/////////////////////////////////////////////////////////////////////////////
// CPageTerrainWater property page

IMPLEMENT_DYNCREATE(CPageTerrainWater, CPropertyPage)

CPageTerrainWater::CPageTerrainWater() : CPropertyPage(CPageTerrainWater::IDD)
{
	//{{AFX_DATA_INIT(CPageTerrainWater)
	//}}AFX_DATA_INIT
}

CPageTerrainWater::~CPageTerrainWater()
{
}

void CPageTerrainWater::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTerrainWater)
	DDX_Text(pDX, IDC_EDIT1, m_nWaterHeight);
	DDV_MinMaxInt(pDX, m_nWaterHeight, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTerrainWater, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTerrainWater)
	ON_BN_CLICKED(IDC_WATER_NONE, OnWaterNone)
	ON_BN_CLICKED(IDC_WATER_WATER, OnWaterWater)
	ON_BN_CLICKED(IDC_WATER_CLOUD, OnWaterCloud)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainWater message handlers

int CPageTerrainWater::m_nWaterTexture = WTYPE_NONE;
int CPageTerrainWater::m_nWaterHeight = 0;
int CPageTerrainWater::m_nWaterFrame = 0;

void CPageTerrainWater::OnWaterNone() 
{
	m_nWaterTexture = WTYPE_NONE;
#ifdef __N_WATERFRAME_050202
	ComboBoxDeleteItem( IDC_COMBO1 );
#endif//__N_WATERFRAME_050202
	m_nWaterHeight = 0;
}

void CPageTerrainWater::OnWaterWater() 
{
	m_nWaterTexture = WTYPE_WATER;
	m_nWaterHeight = 96;
#ifdef __N_WATERFRAME_050202
	ComboBoxAddItem( IDC_COMBO1 );
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBO1 );
	pComboBox->SetCurSel(0);
#endif//__N_WATERFRAME_050202
	UpdateData( FALSE );
}

void CPageTerrainWater::OnWaterCloud() 
{
	m_nWaterTexture = WTYPE_CLOUD;
	m_nWaterHeight = 80;
#ifdef __N_WATERFRAME_050202
	ComboBoxDeleteItem( IDC_COMBO1 );
#endif//__N_WATERFRAME_050202
	UpdateData( FALSE );
}

BOOL CPageTerrainWater::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CButton* pRadio = (CButton*)GetDlgItem( IDC_WATER_NONE );
	pRadio->SetCheck( TRUE );
	m_nWaterTexture = WTYPE_NONE;
	m_nWaterHeight = 0;

	m_nWaterFrame = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#ifdef __N_WATERFRAME_050202
//	리스트에 물이 들어간 텍스쳐 폴더 이름을 콤보박스에 추가 시켜준다.
void CPageTerrainWater::ComboBoxAddItem( const int& BoxIndex )
{
	//	트리에서 값을 받아오기 위한 부분
	CString strName;	//	이름만 받아서 체크하여 값을 넣어 준다.
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CModelView* pModelView = (CModelView*) pMainFrame->m_wndWorkspaceBar.GetView( RUNTIME_CLASS( CModelView ) );
	CTreeCtrl* pTree = &pModelView->GetTreeCtrl();
	HTREEITEM hTerrain = pTree->GetChildItem( pModelView->GetHTreeTerrain() );
	HTREEITEM hTerChild;

	BOOL	bChk = FALSE;
	
	ComboBoxDeleteItem( BoxIndex );
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( BoxIndex );
	while (	( hTerrain != NULL) )
	{
		if ( IS_FOLDER( pTree->GetItemData( hTerrain ) ) )
		{
			strName = pTree->GetItemText( hTerrain );
		}
		if ( strName.Find( "물", 0 ) != -1 )
		{
			hTerChild = pTree->GetChildItem( hTerrain );
			while ( hTerChild != NULL )
			{
				if ( IS_FOLDER( pTree->GetItemData( hTerChild ) ) )
				{
					strName = pTree->GetItemText( hTerChild );
					pComboBox->AddString( strName );
					bChk = TRUE;
				}
				hTerChild = pTree->GetNextSiblingItem( hTerChild );
			}
			if ( bChk != TRUE )
			{
				pComboBox->AddString( strName );
			}
		}
		hTerrain = pTree->GetNextSiblingItem( hTerrain );
	}
	
}

void CPageTerrainWater::ComboBoxDeleteItem( const int& BoxIndex )
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( BoxIndex );
	pComboBox->ResetContent( );
}

#endif//__N_WATERFRAME_050202

void CPageTerrainWater::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWorldView::m_nTool = ID_TERRAIN_WATER;	
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CPageTerrainHeight::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}




void CPageTerrainTexture::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWorldView::m_nTool = ID_TERRAIN_TEXTURE;
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPageTerrainColor::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWorldView::m_nTool = ID_TERRAIN_COLOR;
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CPageTerrainWater::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );
}
/////////////////////////////////////////////////////////////////////////////
// CPageTerrainLayer dialog

IMPLEMENT_DYNCREATE(CPageTerrainLayer, CPropertyPage)

CPageTerrainLayer::CPageTerrainLayer() : CPropertyPage(CPageTerrainLayer::IDD)
{
	//{{AFX_DATA_INIT(CPageTerrainLayer)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pLandscape = NULL;
	
}

CPageTerrainLayer::~CPageTerrainLayer()
{
}


void CPageTerrainLayer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageTerrainLayer)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageTerrainLayer, CPropertyPage)
	//{{AFX_MSG_MAP(CPageTerrainLayer)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_ALL_VISIBLE, OnAllVisible)
	ON_BN_CLICKED(IDC_TEXTURE_OPEN_BUTTON, OnOpenTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageTerrainLayer message handlers

BOOL CPageTerrainLayer::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	CButton* pButton = (CButton*)GetDlgItem( IDC_REMOVE );
	CStatic* pStatic = (CStatic*)GetDlgItem( IDC_STATIC_LAND );
	CRect rectButton, rectStatic;
	pButton->GetClientRect( rectButton );
	pStatic->GetClientRect( rectStatic );

	CRect rect;
	GetClientRect( &rect );
	rect.right -= rectButton.Width() + 10;
	rect.top = rectStatic.bottom + 5;
	rect.DeflateRect( 10, 10 );
	m_checkList.Create( rect, this, 999, IDB_CHECK, IDB_UNCHECK, IDB_MIDDLECHECK );
	//m_checkList.AddString( _T( "World Map"          ), NULL, CHECKED, RA_WORLD       );
	//m_checkList.AddString( _T( "Dungeon Map"        ), NULL, CHECKED, RA_DUNGEON     );
	//m_checkList.AddString( _T( "Newbie Map"         ), NULL, CHECKED, RA_NEWBIE      );
//	CButton* pRemove = (CButton*)GetDlgItem( IDC_REMOVE );
	//pRemove->EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPageTerrainLayer::UpdateLayer( D3DXVECTOR3& vPos, CWorld* pWorld )
{
	if( m_hWnd == NULL ) return;
	CLandscape* pLandscape = pWorld->GetLandscape( vPos );

	if( m_pLandscape == pLandscape )
		return;
	m_pLandscape = pLandscape;
	if( m_pLandscape == NULL )
		return;
	int nLayer = pLandscape->m_aLayer.GetSize();
	
	
	CString string = "Land =";
	CString strLandName = pWorld->m_szFileName;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	string.Format( "(%d) %s%02d-%02d.lnd", nLayer, strLandName, (int)vPos.x / ( MAP_SIZE * MPU ) , (int)vPos.z / ( MAP_SIZE * MPU ) );
	CStatic* pStatic = (CStatic*)GetDlgItem( IDC_STATIC_LAND );
	pStatic->SetWindowText( string );
	
	m_checkList.ResetContent();
	CLandLayer* pLayer;
	for( int i = 0; i < nLayer; i++ )
	{
		pLayer = (CLandLayer*)pLandscape->m_aLayer.GetAt( i );
		LPTERRAIN lpTarrain = prj.m_terrainMng.GetTerrain( pLayer->m_nTex );
		string.Format( "%d - %s", i, lpTarrain->m_szTextureFileName );
		m_checkList.AddString( string, NULL, pLayer->m_bVisible, i       );
		if( pLayer->m_nTex )
		{


		}
		if( prj.m_terrainMng.GetTerrain( pLayer->m_nTex )->m_pTexture == NULL )
		{
		}
			
	}
	m_checkList.Invalidate( FALSE );
}

BOOL CPageTerrainLayer::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnChildNotify(message, wParam, lParam, pLResult);
}

BOOL CPageTerrainLayer::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CPageTerrainLayer::OnRemove() 
{
	if( m_hWnd == NULL ) 
		return;
	if( m_pLandscape == NULL )
		return;
	// TODO: Add your control notification handler code here
	int nSelect = m_checkList.GetCurSel();
	if( nSelect < 0 || nSelect >= m_checkList.GetCount() )
		return;
	LISTITEM* lpListItem = m_checkList.GetItem( nSelect );
	CLandLayer* pLayer = (CLandLayer*)m_pLandscape->m_aLayer.GetAt( nSelect );
	m_pLandscape->RemoveLayer_Layer( nSelect );
	m_pLandscape = NULL;
	m_checkList.Invalidate( FALSE );
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CView* pView = pMainFrame->GetActiveView();
	if( pView ) pView->Invalidate( FALSE );
}

void CPageTerrainLayer::OnUpdate() 
{
	if( m_hWnd == NULL ) 
		return;
	if( m_pLandscape == NULL )
		return;


// 업데이트를 하면 선택된 지형 텍스처 파일을 갱신시킨다.
	int nSelect = m_checkList.GetCurSel();
	if( nSelect < 0 || nSelect >= m_checkList.GetCount() )
		return;
	LISTITEM* lpListItem = m_checkList.GetItem( nSelect );
	CLandLayer* pLayer = (CLandLayer*)m_pLandscape->m_aLayer.GetAt( nSelect );
	LPTERRAIN lpTarrain = prj.m_terrainMng.GetTerrain( pLayer->m_nTex );
	SAFE_RELEASE( lpTarrain->m_pTexture );
	prj.m_terrainMng.LoadTexture( pLayer->m_nTex );
//


	// TODO: Add your control notification handler code here
	//int nLayer = pLandscape->m_aLayer.GetSize();
	int nCount = m_checkList.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		LISTITEM* lpListItem = m_checkList.GetItem( i );
		CLandLayer* pLayer = (CLandLayer*)m_pLandscape->m_aLayer.GetAt( i );
		if( lpListItem->nCheckedState )
			pLayer->m_bVisible = TRUE;
		else
			pLayer->m_bVisible = FALSE;
	}
	m_checkList.Invalidate( FALSE );
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CView* pView = pMainFrame->GetActiveView();
	if( pView ) pView->Invalidate( FALSE );
}

void CPageTerrainLayer::OnAllVisible() 
{
	// TODO: Add your control notification handler code here
	if( m_hWnd == NULL ) 
		return;
	if( m_pLandscape == NULL )
		return;
	CString string;
	CLandLayer* pLayer;
	int nLayer = m_pLandscape->m_aLayer.GetSize();
	m_checkList.ResetContent();
	for( int i = 0; i < nLayer; i++ )
	{
		pLayer = (CLandLayer*)m_pLandscape->m_aLayer.GetAt( i );
		pLayer->m_bVisible = TRUE;
		LPTERRAIN lpTarrain = prj.m_terrainMng.GetTerrain( pLayer->m_nTex );
		string.Format( "%d - %s", i, lpTarrain->m_szTextureFileName );
		m_checkList.AddString( string, NULL, pLayer->m_bVisible, i       );
	}
	m_checkList.Invalidate( FALSE );
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CView* pView = pMainFrame->GetActiveView();
	if( pView ) pView->Invalidate( FALSE );
}

void CPageTerrainLayer::OnOpenTexture() 
{
	if( m_hWnd == NULL ) 
		return;
	if( m_pLandscape == NULL )
		return;
	// TODO: Add your control notification handler code here
	int nSelect = m_checkList.GetCurSel();
	if( nSelect < 0 || nSelect >= m_checkList.GetCount() )
		return;
	LISTITEM* lpListItem = m_checkList.GetItem( nSelect );
	CLandLayer* pLayer = (CLandLayer*)m_pLandscape->m_aLayer.GetAt( nSelect );
	LPTERRAIN lpTarrain = prj.m_terrainMng.GetTerrain( pLayer->m_nTex );
	CString str;
	str.Format( "%s%s", DIR_WORLDTEX, lpTarrain->m_szTextureFileName );
	ShellExecute(  NULL, "open", str, "", NULL , SW_SHOW );
}

void CPageTerrainColor::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rcControlWnd, rcThisWnd;
	GetDlgItem(IDC_COLOR)->GetWindowRect(rcControlWnd);
	this->GetWindowRect(rcThisWnd);
	rcControlWnd.left -= rcThisWnd.left;
	rcControlWnd.top -= rcThisWnd.top;
	rcControlWnd.right -= rcThisWnd.left;
	rcControlWnd.bottom -= rcThisWnd.top;

    COLORREF cr;

	cr = RGB((m_dwVertexColor >> 16) & 0xff,(m_dwVertexColor >> 8) & 0xff,m_dwVertexColor & 0xff);

    CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(cr);   // 이 RGB값을 넣고싶은 색으로 하세요.
	pOldBrush = (CBrush*)dc.SelectObject(&brush);   // dc에 선택
	dc.Rectangle(rcControlWnd.left,rcControlWnd.top,rcControlWnd.right,rcControlWnd.bottom);
	dc.SelectObject(pOldBrush);

    brush.DeleteObject();
	
	// Do not call CPropertyPage::OnPaint() for painting messages
}

#ifdef __N_WATERFRAME_050202
//	이 코드는 우선 BYTE단위로 생각이 됬기 때문에 물의 종류가 
//	63가지보다 (63개인 이유는 앞 6바이트를 사용하고 있기 때문이다.)늘어나게 된다면 
//	저장되어 지는 곳에서 BYTE가 아닌 변수를 저장하도록 바꾸어야 할것이다.
void CPageTerrainWater::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBO1 );
	m_nWaterFrame = pComboBox->GetCurSel();

	if ( m_nWaterTexture != WTYPE_WATER && m_nWaterFrame != -1)
	{
		m_nWaterFrame = 0;
		pComboBox->SetCurSel(-1);
	}
	if ( m_nWaterFrame != -1 )
		m_nWaterFrame = m_nWaterFrame << 2;	
	else
		m_nWaterFrame = 0;
}
#endif//__N_WATERFRAME_050202
