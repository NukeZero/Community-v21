// TexPanel.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "SFXEditor.h"
#include "TexPanel.h"
#include "childfrm.h"
#include "targa.h"
#include "path.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTexPanel dialog


CTexPanel::CTexPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTexPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTexPanel)
	m_bScalRepeat = FALSE;
	//}}AFX_DATA_INIT
	m_pDoc=NULL;
	m_pView=NULL;
}


void CTexPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTexPanel)
	DDX_Control(pDX, IDC_ZSCALREPEATLOW, m_ScalRepeatZLow);
	DDX_Control(pDX, IDC_ZSCALREPEATHIGH, m_ScalRepeatZHigh);
	DDX_Control(pDX, IDC_YSCALREPEATLOW, m_ScalRepeatYLow);
	DDX_Control(pDX, IDC_YSCALREPEATHIGH, m_ScalRepeatYHigh);
	DDX_Control(pDX, IDC_XSCALREPEATHIGH, m_ScalRepeatXHigh);
	DDX_Control(pDX, IDC_XSCALREPEATLOW, m_ScalRepeatXLow);
	DDX_Control(pDX, IDC_PSCALEENDZ, m_ScalEndZ);
	DDX_Control(pDX, IDC_PSCALEENDY, m_ScalEndY);
	DDX_Control(pDX, IDC_PSCALEENDX, m_ScalEndX);
	DDX_Control(pDX, IDC_PARTICLE_ROT_Z_LOW, m_PRotZLow);
	DDX_Control(pDX, IDC_PARTICLE_ROT_Z_HIGH, m_PRotZHigh);
	DDX_Control(pDX, IDC_PARTICLE_ROT_Y_LOW, m_PRotYLow);
	DDX_Control(pDX, IDC_PARTICLE_ROT_Y_HIGH, m_PRotYHigh);
	DDX_Control(pDX, IDC_PARTICLE_ROT_X_LOW, m_PRotXLow);
	DDX_Control(pDX, IDC_PARTICLE_ROT_X_HIGH, m_PRotXHigh);
	DDX_Control(pDX, IDC_TEXLOOP, m_TexLoop);
	DDX_Control(pDX, IDC_TEXFRAME, m_TexFrame);
	DDX_Control(pDX, IDC_PYLOW, m_PYLow);
	DDX_Control(pDX, IDC_PYHIGH, m_PYHigh);
	DDX_Control(pDX, IDC_PXZLOW, m_PXZLow);
	DDX_Control(pDX, IDC_PXZHIGH, m_PXZHigh);
	DDX_Control(pDX, IDC_PSCALEZ, m_PScaleZ);
	DDX_Control(pDX, IDC_PSCALEY, m_PScaleY);
	DDX_Control(pDX, IDC_PSCALEX, m_PScaleX);
	DDX_Control(pDX, IDC_PSCALESPEEDZ, m_PScaleSpeedZ);
	DDX_Control(pDX, IDC_PSCALESPEEDY, m_PScaleSpeedY);
	DDX_Control(pDX, IDC_PSCALESPEEDX, m_PScaleSpeedX);
	DDX_Control(pDX, IDC_PPOSVARY, m_PPosVarY);
	DDX_Control(pDX, IDC_PPOSVAR, m_PPosVar);
	DDX_Control(pDX, IDC_PKEEP, m_PKeep);
	DDX_Control(pDX, IDC_PDISAPPEAR, m_PDisappear);
	DDX_Control(pDX, IDC_PCREATENUM, m_PCreateNum);
	DDX_Control(pDX, IDC_PCREATE, m_PCreate);
	DDX_Control(pDX, IDC_PAPPEAR, m_PAppear);
	DDX_Control(pDX, IDC_PACCELZ, m_PAccelZ);
	DDX_Control(pDX, IDC_PACCELY, m_PAccelY);
	DDX_Control(pDX, IDC_PACCELX, m_PAccelX);
	DDX_Control(pDX, IDC_PROTZ, m_PosRotZ);
	DDX_Control(pDX, IDC_PROTY, m_PosRotY);
	DDX_Control(pDX, IDC_PROTX, m_PosRotX);
	DDX_Control(pDX, IDC_ANGZ, m_AngleZ);
	DDX_Control(pDX, IDC_ANGY, m_AngleY);
	DDX_Control(pDX, IDC_ANGX, m_AngleX);
	DDX_Control(pDX, IDC_SZ, m_ScaleZ);
	DDX_Control(pDX, IDC_SY, m_ScaleY);
	DDX_Control(pDX, IDC_SX, m_ScaleX);
	DDX_Control(pDX, IDC_A, m_Alpha);
	DDX_Control(pDX, IDC_Z, m_Z);
	DDX_Control(pDX, IDC_Y, m_Y);
	DDX_Control(pDX, IDC_X, m_X);
	DDX_Check(pDX, IDC_SCAL_REPEAT_CHECK, m_bScalRepeat);
	DDX_Check(pDX, IDC_PRT_REPEAT_CHECK2, m_bRepeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTexPanel, CDialog)
	//{{AFX_MSG_MAP(CTexPanel)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_X, OnChangeX)
	ON_EN_CHANGE(IDC_Y, OnChangeY)
	ON_EN_CHANGE(IDC_Z, OnChangeZ)
	ON_EN_CHANGE(IDC_A, OnChangeA)
	ON_EN_CHANGE(IDC_SX, OnChangeSx)
	ON_EN_CHANGE(IDC_SY, OnChangeSy)
	ON_EN_CHANGE(IDC_ANGX, OnChangeAngX)
	ON_BN_CLICKED(IDC_RADIOBILL, OnRadiobill)
	ON_BN_CLICKED(IDC_RADIOBOTTOM, OnRadiobottom)
	ON_BN_CLICKED(IDC_RADIOBLEND, OnRadioblend)
	ON_BN_CLICKED(IDC_RADIOGLOW, OnRadioglow)
	ON_BN_CLICKED(IDC_BUTTONTEX, OnButtontex)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RADIONORMAL, OnRadionormal)
	ON_BN_CLICKED(IDC_RADIOPOLE, OnRadiopole)
	ON_EN_CHANGE(IDC_SZ, OnChangeSz)
	ON_EN_CHANGE(IDC_ANGY, OnChangeAngY)
	ON_EN_CHANGE(IDC_ANGZ, OnChangeAngZ)
	ON_EN_CHANGE(IDC_PROTX, OnChangeProtX)
	ON_EN_CHANGE(IDC_PROTY, OnChangeProtY)
	ON_EN_CHANGE(IDC_PROTZ, OnChangeProtZ)
	ON_BN_CLICKED(IDC_BUTTONTEXNULL, OnButtontexnull)
	ON_BN_CLICKED(IDC_BUTTONLOADSFX, OnButtonloadsfx)
	ON_BN_CLICKED(IDC_BUTTONSAVESFX, OnButtonsavesfx)
	ON_EN_CHANGE(IDC_PACCELX, OnChangePaccelx)
	ON_EN_CHANGE(IDC_PACCELY, OnChangePaccely)
	ON_EN_CHANGE(IDC_PACCELZ, OnChangePaccelz)
	ON_EN_CHANGE(IDC_PAPPEAR, OnChangePappear)
	ON_EN_CHANGE(IDC_PCREATE, OnChangePcreate)
	ON_EN_CHANGE(IDC_PCREATENUM, OnChangePcreatenum)
	ON_EN_CHANGE(IDC_PDISAPPEAR, OnChangePdisappear)
	ON_EN_CHANGE(IDC_PKEEP, OnChangePkeep)
	ON_EN_CHANGE(IDC_PPOSVAR, OnChangePposvar)
	ON_EN_CHANGE(IDC_PPOSVARY, OnChangePposvary)
	ON_EN_CHANGE(IDC_PSCALESPEEDX, OnChangePscalespeedx)
	ON_EN_CHANGE(IDC_PSCALESPEEDY, OnChangePscalespeedy)
	ON_EN_CHANGE(IDC_PSCALESPEEDZ, OnChangePscalespeedz)
	ON_EN_CHANGE(IDC_PSCALEX, OnChangePscalex)
	ON_EN_CHANGE(IDC_PSCALEY, OnChangePscaley)
	ON_EN_CHANGE(IDC_PSCALEZ, OnChangePscalez)
	ON_EN_CHANGE(IDC_PXZHIGH, OnChangePxzhigh)
	ON_EN_CHANGE(IDC_PXZLOW, OnChangePxzlow)
	ON_EN_CHANGE(IDC_PYHIGH, OnChangePyhigh)
	ON_EN_CHANGE(IDC_PYLOW, OnChangePylow)
	ON_EN_CHANGE(IDC_TEXFRAME, OnChangeTexframe)
	ON_EN_CHANGE(IDC_TEXLOOP, OnChangeTexloop)
	ON_BN_CLICKED(IDC_BUTTONQSAVESFX, OnButtonqsavesfx)
	ON_BN_CLICKED(IDC_BUTTONQLOADSFX, OnButtonqloadsfx)
	ON_EN_CHANGE(IDC_PARTICLE_ROT_X_LOW, OnChangeParticleRotXLow)
	ON_EN_CHANGE(IDC_PARTICLE_ROT_X_HIGH, OnChangeParticleRotXHigh)
	ON_EN_CHANGE(IDC_PARTICLE_ROT_Y_HIGH, OnChangeParticleRotYHigh)
	ON_EN_CHANGE(IDC_PARTICLE_ROT_Y_LOW, OnChangeParticleRotYLow)
	ON_EN_CHANGE(IDC_PARTICLE_ROT_Z_HIGH, OnChangeParticleRotZHigh)
	ON_EN_CHANGE(IDC_PARTICLE_ROT_Z_LOW, OnChangeParticleRotZLow)
	ON_BN_CLICKED(IDC_SCAL_REPEAT_CHECK, OnScalRepeatCheck)
	ON_BN_CLICKED(IDC_PRT_REPEAT_CHECK2, OnRepeatCheck)
	ON_BN_CLICKED(IDC_BUTTONLOADSFX2, OnButtonloadsfx2)
	ON_EN_CHANGE(IDC_XSCALREPEATHIGH, OnChangeXscalrepeathigh)
	ON_EN_CHANGE(IDC_XSCALREPEATLOW, OnChangeXscalrepeatlow)
	ON_EN_CHANGE(IDC_YSCALREPEATHIGH, OnChangeYscalrepeathigh)
	ON_EN_CHANGE(IDC_YSCALREPEATLOW, OnChangeYscalrepeatlow)
	ON_EN_CHANGE(IDC_ZSCALREPEATHIGH, OnChangeZscalrepeathigh)
	ON_EN_CHANGE(IDC_ZSCALREPEATLOW, OnChangeZscalrepeatlow)
	ON_EN_CHANGE(IDC_PSCALEENDX, OnChangePscaleendx)
	ON_EN_CHANGE(IDC_PSCALEENDY, OnChangePscaleendy)
	ON_EN_CHANGE(IDC_PSCALEENDZ, OnChangePscaleendz)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTexPanel message handlers

void CTexPanel::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
/*
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_TEXLIST);
	if(pList) {
		RECT rect; 
		GetClientRect(&rect);
		pList->MoveWindow(rect.left+5,rect.top+5,rect.right-10,rect.bottom-10);
	}
*/
}

BOOL CTexPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
/*
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_TEXLIST);
	if(pList) {
		pList->InsertColumn(0,"texture",LVCFMT_LEFT,300);
	}
*/
	CDC dc;
//	m_Bitmap.CreateCompatibleBitmap(&dc,256,256);
//	LoadTexture("default.tga");	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTexPanel::EnableControl(BOOL bEnable)
{
	(CEdit*)GetDlgItem(IDC_X)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_Y)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_Z)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_PROTX)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_PROTY)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_PROTZ)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_SX)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_SY)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_SZ)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_A)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_ANGX)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_ANGY)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_ANGZ)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_TEXFRAME)->EnableWindow(bEnable);
	(CEdit*)GetDlgItem(IDC_TEXLOOP)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOBILL)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOBOTTOM)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOPOLE)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIONORMAL)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOGLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOBLEND)->EnableWindow(bEnable);
}
void CTexPanel::EnableRadio(BYTE nIndex)
{
	if(nIndex==SFXPARTBILLTYPE_BILL) {
		((CButton*)GetDlgItem(IDC_RADIOBILL))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIOBOTTOM))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOPOLE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIONORMAL))->SetCheck(BST_UNCHECKED);
	}
	else if(nIndex==SFXPARTBILLTYPE_BOTTOM) {
		((CButton*)GetDlgItem(IDC_RADIOBILL))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOBOTTOM))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIOPOLE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIONORMAL))->SetCheck(BST_UNCHECKED);
	}
	else if(nIndex==SFXPARTBILLTYPE_POLE) {
		((CButton*)GetDlgItem(IDC_RADIOBILL))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOBOTTOM))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOPOLE))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIONORMAL))->SetCheck(BST_UNCHECKED);
	}
	else if(nIndex==SFXPARTBILLTYPE_NORMAL) {
		((CButton*)GetDlgItem(IDC_RADIOBILL))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOBOTTOM))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOPOLE))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIONORMAL))->SetCheck(BST_CHECKED);
	}
}
void CTexPanel::EnableAlphaRadio(BYTE nIndex)
{
	if(nIndex==SFXPARTALPHATYPE_BLEND) {
		((CButton*)GetDlgItem(IDC_RADIOGLOW))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIOBLEND))->SetCheck(BST_CHECKED);
	}
	else if(nIndex==SFXPARTALPHATYPE_GLOW) {
		((CButton*)GetDlgItem(IDC_RADIOGLOW))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIOBLEND))->SetCheck(BST_UNCHECKED);
	}
}

void CTexPanel::OnChangeX() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString strTemp;
	m_X.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);

	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vPos.x=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
	// TODO: Add your control notification handler code here
	
}

void CTexPanel::OnChangeY() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString strTemp;
	m_Y.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vPos.y=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
	
	// TODO: Add your control notification handler code here
	
}

void CTexPanel::OnChangeZ() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString strTemp;
	m_Z.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vPos.z=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
	
	// TODO: Add your control notification handler code here
	
}

void CTexPanel::OnChangeProtX() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PosRotX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vPosRotate.x=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnChangeProtY() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PosRotY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vPosRotate.y=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnChangeProtZ() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PosRotZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vPosRotate.z=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnChangeA() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString strTemp;
	m_Alpha.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->nAlpha=atoi(strTemp);
		m_pView->SetModified(true);
	}

	// TODO: Add your control notification handler code here
	
}

void CTexPanel::OnChangeSx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString strTemp;
	m_ScaleX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vScale.x=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}

 	// TODO: Add your control notification handler code here
	
}

void CTexPanel::OnChangeSy() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString strTemp;
	m_ScaleY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vScale.y=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
	
	// TODO: Add your control notification handler code here
	
}

void CTexPanel::OnChangeSz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_ScaleZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vScale.z=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
	
}

void CTexPanel::OnChangeAngX() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_AngleX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vRotate.x=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnChangeAngY() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_AngleY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vRotate.y=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnChangeAngZ() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_AngleZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		SfxKeyFrame* pKey=pPart->KeyByFrame(pFrame->m_nLCFrame);
		if(pKey) pKey->vRotate.z=(float)( atof(strTemp) );
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnRadiobill() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nBillType=SFXPARTBILLTYPE_BILL;
		m_pView->SetModified(true);
		EnableRadio(pPart->m_nBillType);
		EnableAlphaRadio(pPart->m_nAlphaType);
	}
}

void CTexPanel::OnRadiobottom() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nBillType=SFXPARTBILLTYPE_BOTTOM;
		m_pView->SetModified(true);
		EnableRadio(pPart->m_nBillType);
		EnableAlphaRadio(pPart->m_nAlphaType);
	}
}

void CTexPanel::OnRadiopole() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nBillType=SFXPARTBILLTYPE_POLE;
		m_pView->SetModified(true);
		EnableRadio(pPart->m_nBillType);
		EnableAlphaRadio(pPart->m_nAlphaType);
	}
}

void CTexPanel::OnRadionormal() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nBillType=SFXPARTBILLTYPE_NORMAL;
		m_pView->SetModified(true);
		EnableRadio(pPart->m_nBillType);
		EnableAlphaRadio(pPart->m_nAlphaType);
	}
}

void CTexPanel::OnRadioblend() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nAlphaType=SFXPARTALPHATYPE_BLEND;
		m_pView->SetModified(true);
		EnableRadio(pPart->m_nBillType);
		EnableAlphaRadio(pPart->m_nAlphaType);
	}
}

void CTexPanel::OnRadioglow() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nAlphaType=SFXPARTALPHATYPE_GLOW;
		m_pView->SetModified(true);
		EnableRadio(pPart->m_nBillType);
		EnableAlphaRadio(pPart->m_nAlphaType);
	}
}

void CTexPanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CTexPanel::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CTexPanel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

//    LPDIRECT3DDEVICE9 pd3ddev = g_;
//    LPDIRECT3DSURFACE9 psurf;

//	dc.BitBlt(10,240,256,256,&dcTemp,0,0,SRCCOPY);
}

void CTexPanel::OnButtontex() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) 
	{
		/*
		TCHAR szCurPath[ 256 ];
		::GetCurrentDirectory( 128, szCurPath );

		TCHAR szTexPath[ 256 ];
		strcpy( szTexPath, GetResourcePath() );
		strcat( szTexPath, "sfx\\texture\\" );
		::SetCurrentDirectory( szTexPath ); //MakePath( "", DIR_SFXTEX ) );
*/
		CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
			"DDS file (*.dds)|*.dds|bmp file (*.bmp)|*.dds|o3d file (*.tga)|*.tga|o3d file (*.o3d)|*.o3d||");
		
		if(pPart->m_nType == SFXPARTTYPE_MESH ) 
		{
			dlgFile.m_ofn.lpstrInitialDir =DIR_MODEL;
			dlgFile.m_ofn.lpstrTitle=_T("메쉬 추가");
		}
		else 
		{
			dlgFile.m_ofn.lpstrInitialDir = DIR_SFXTEX;
			dlgFile.m_ofn.lpstrTitle=_T("새 텍스쳐 추가");
		}
		dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
		if( dlgFile.DoModal() == IDOK) 
		{
			CString filename = dlgFile.GetFileName();
			filename.MakeLower();
			pPart->m_strTex=filename;
			m_pView->SetModified(true);
		}
		//::SetCurrentDirectory( szCurPath );
	}
}

void CTexPanel::LoadTexture(LPCTSTR strTex)
{
	g_SfxTex.DeleteAll();
	LPDIRECT3DTEXTURE9 pTex=g_SfxTex.Tex(strTex);
}

void CTexPanel::OnButtontexnull() 
{
	// TODO: Add your control notification handler code here
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_strTex.Empty();
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnButtonloadsfx() 
{
	// TODO: Add your control notification handler code here
	//CFileDialog dlgFile(TRUE);
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Sfx file (*.sfx)|*.sfx||");
	
	dlgFile.m_ofn.lpstrInitialDir=DIR_SFX;
	dlgFile.m_ofn.lpstrTitle=_T("로드");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		g_SfxObjMng.RemoveAll();
		CString filename=dlgFile.GetPathName();
		filename.MakeLower();
		g_SfxMng.GetSfxBase(0)->m_strName=filename;
		g_SfxMng.GetSfxBase(0)->Load();
		m_pView->m_SfxObj.SetSfx(g_SfxMng.GetSfxBase(0));
		CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
		pFrame->m_SelectedKeys.RemoveAll();
		pFrame->m_nCurFrame=pFrame->m_nLCFrame=pFrame->m_nLeftFrame=0;
		pFrame->m_nCurPart=pFrame->m_nLCPart=pFrame->m_nTopObj=0;
		pFrame->m_pClickedKey=NULL;

#ifdef __AALLSELECT
		pFrame->m_bLayerDragDrop = FALSE;
		pFrame->m_nSfxPartClipBoard = -1;
#endif

		pFrame->UpdatePanel();

		CString strTitle;
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

		strTitle.Format("%s", g_SfxMng.GetSfxBase(0)->m_strName );
		pMain->SetWindowText(strTitle);

		m_pView->SetModified(false);
		m_pView->SetFileType(LOAD_FILE);
	}
}

void CTexPanel::OnButtonsavesfx() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgFile(FALSE);
	
	dlgFile.m_ofn.lpstrInitialDir=DIR_SFX;
	dlgFile.m_ofn.lpstrTitle=_T("세이브");
	dlgFile.m_ofn.Flags|=OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN ;
	dlgFile.m_ofn.lpstrDefExt = "sfx";
	dlgFile.m_ofn.lpstrFilter = "Sfx Files (*.sfx)\0*.sfx\0All Files\0*.*\0";

	if(dlgFile.DoModal()==IDOK) {
		CString str = dlgFile.GetPathName();
		CString filename=dlgFile.GetPathName();
		filename.MakeLower();
		g_SfxMng.GetSfxBase(0)->m_strName=filename;
		g_SfxMng.GetSfxBase(0)->Save();
		m_pView->SetModified(false);
	}
}

void CTexPanel::OnChangePaccelx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PAccelX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vParticleAccel.x=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePaccely() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PAccelY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vParticleAccel.y=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePaccelz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PAccelZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vParticleAccel.z=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePappear() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PAppear.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_nParticleFrameAppear=atoi(strTemp);
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePcreate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PCreate.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_nParticleCreate=atoi(strTemp);
			m_pView->SetModified(true);
		}
		if(pPart->m_nType==SFXPARTTYPE_CUSTOMMESH) {
			CSfxPartCustomMesh* pPartCustomMesh=(CSfxPartCustomMesh*)pPart;
			pPartCustomMesh->m_nPoints=atoi(strTemp);
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePcreatenum() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PCreateNum.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_nParticleCreateNum=atoi(strTemp);
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePdisappear() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PDisappear.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_nParticleFrameDisappear=atoi(strTemp);
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePkeep() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PKeep.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_nParticleFrameKeep=atoi(strTemp);
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePposvar() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PPosVar.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fParticleStartPosVar=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePposvary() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PPosVarY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fParticleStartPosVarY=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscalespeedx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PScaleSpeedX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScaleSpeed.x=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscalespeedy() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PScaleSpeedY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScaleSpeed.y=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscalespeedz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PScaleSpeedZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScaleSpeed.z=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscalex() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PScaleX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScale.x=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscaley() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PScaleY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScale.y=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscalez() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PScaleZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScale.z=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePxzhigh() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PXZHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fParticleXZHigh=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePxzlow() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PXZLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fParticleXZLow=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePyhigh() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PYHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fParticleYHigh=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePylow() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PYLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fParticleYLow=(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeTexframe() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_TexFrame.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nTexFrame=atoi(strTemp);
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnChangeTexloop() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_TexLoop.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	if(pPart) {
		pPart->m_nTexLoop=atoi(strTemp);
		m_pView->SetModified(true);
	}
}

void CTexPanel::OnButtonqsavesfx() 
{
	// TODO: Add your control notification handler code here
//	CFileDialog dlgFile(FALSE);
//	dlgFile.m_ofn.lpstrInitialDir=DIR_SFX;
//	dlgFile.m_ofn.lpstrTitle=_T("세이브");
//	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
//	if(dlgFile.DoModal()==IDOK) {
	if(!g_SfxMng.GetSfxBase(0)->m_strName.IsEmpty()) {
		CString strTemp=g_SfxMng.GetSfxBase(0)->m_strName+"로 세이브하겠소";
		int ret=MessageBox(strTemp,"하시겠소?",MB_OKCANCEL);
		if(ret==IDOK) {
			g_SfxMng.GetSfxBase(0)->Save();
			m_pView->SetModified(false);
			m_pView->SetFileType(LOAD_FILE);
		}
	}
	else {
		MessageBox("이거는 한번도 세이브 안한거라서 퀵세이브 못하셈","경고");
	}
}

void CTexPanel::OnButtonqloadsfx() 
{
	// TODO: Add your control notification handler code here
	if(!g_SfxMng.GetSfxBase(0)->m_strName.IsEmpty()) 
	{
		CString strTemp=g_SfxMng.GetSfxBase(0)->m_strName+"를 다시 로드하겠소";
		int ret=MessageBox(strTemp,"하시겠소?",MB_OKCANCEL);
		if(ret==IDOK) {
			g_SfxMng.GetSfxBase(0)->Load();
//			m_pView->m_SfxObj.SetSfx(g_SfxMng.GetSfxBase(0));
			CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
			pFrame->m_SelectedKeys.RemoveAll();
			pFrame->m_nCurFrame=pFrame->m_nLCFrame=pFrame->m_nLeftFrame=0;
			pFrame->m_nCurPart=pFrame->m_nLCPart=pFrame->m_nTopObj=0;
			pFrame->m_pClickedKey=NULL;
			pFrame->UpdatePanel();
			m_pView->SetModified(false);
			m_pView->SetFileType(LOAD_FILE);
			MessageBox("로드 완료");
		}
	}
	else {
		MessageBox("이거는 한번도 세이브 안한거라서 퀵로드 못하셈","경고");
	}
}

void CTexPanel::EnableRadio_All(BOOL bEnable)
{
	(CButton*)GetDlgItem(IDC_RADIOBILL)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOBOTTOM)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOPOLE)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIONORMAL)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOGLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_RADIOBLEND)->EnableWindow(bEnable);
}


void CTexPanel::EnableParticle(BOOL bEnable)
{
	(CButton*)GetDlgItem(IDC_PCREATE)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PCREATENUM)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PAPPEAR)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PKEEP)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PDISAPPEAR)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PPOSVAR)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PPOSVARY)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PYLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PYHIGH)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PXZLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PXZHIGH)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PACCELX)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PACCELY)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PACCELZ)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALEX)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALEY)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALEZ)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALESPEEDX)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALESPEEDY)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALESPEEDZ)->EnableWindow(bEnable);

	(CButton*)GetDlgItem(IDC_XSCALREPEATLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_XSCALREPEATHIGH)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_YSCALREPEATLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_YSCALREPEATHIGH)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_ZSCALREPEATLOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_ZSCALREPEATHIGH)->EnableWindow(bEnable);

	(CButton*)GetDlgItem(IDC_SCAL_REPEAT_CHECK)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PRT_REPEAT_CHECK2)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALEENDX)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALEENDY)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PSCALEENDZ)->EnableWindow(bEnable);
	
}

void CTexPanel::EnableParticleEx(BOOL bEnable)
{
	(CButton*)GetDlgItem(IDC_PARTICLE_ROT_X_LOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PARTICLE_ROT_X_HIGH)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PARTICLE_ROT_Y_LOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PARTICLE_ROT_Y_HIGH)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PARTICLE_ROT_Z_LOW)->EnableWindow(bEnable);
	(CButton*)GetDlgItem(IDC_PARTICLE_ROT_Z_HIGH)->EnableWindow(bEnable);
}

void CTexPanel::OnChangeParticleRotXLow() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PRotXLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vRotationLow.x =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeParticleRotXHigh() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PRotXHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vRotationHigh.x =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
	
}

void CTexPanel::OnChangeParticleRotYHigh() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PRotYHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vRotationHigh.y =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
	
}

void CTexPanel::OnChangeParticleRotYLow() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PRotYLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vRotationLow.y =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
	
}

void CTexPanel::OnChangeParticleRotZHigh() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PRotZHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vRotationHigh.z =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
	
}

void CTexPanel::OnChangeParticleRotZLow() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString strTemp;
	m_PRotZLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vRotationLow.z =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
	
}

void CTexPanel::OnScalRepeatCheck() 
{
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);

	if(pFrame->m_SelectedKeys.GetSize()!=1) return;

	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);

	CSfxPartParticle* pPartParticle;

	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;

			m_bScalRepeat = !m_bScalRepeat;

			pPartParticle->m_bRepeatScal = m_bScalRepeat;
			m_pView->SetModified(true);

			if( pPartParticle->m_bRepeatScal )
			{
				(CButton*)GetDlgItem(IDC_XSCALREPEATLOW)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_XSCALREPEATHIGH)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_YSCALREPEATLOW)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_YSCALREPEATHIGH)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_ZSCALREPEATLOW)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_ZSCALREPEATHIGH)->EnableWindow(TRUE);
				
				(CButton*)GetDlgItem(IDC_PSCALEENDX)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_PSCALEENDY)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_PSCALEENDZ)->EnableWindow(TRUE);
				
				(CButton*)GetDlgItem(IDC_PSCALESPEEDX)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_PSCALESPEEDY)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_PSCALESPEEDZ)->EnableWindow(FALSE);
			}
			else
			{
				(CButton*)GetDlgItem(IDC_XSCALREPEATLOW)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_XSCALREPEATHIGH)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_YSCALREPEATLOW)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_YSCALREPEATHIGH)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_ZSCALREPEATLOW)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_ZSCALREPEATHIGH)->EnableWindow(FALSE);
				
				(CButton*)GetDlgItem(IDC_PSCALEENDX)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_PSCALEENDY)->EnableWindow(FALSE);
				(CButton*)GetDlgItem(IDC_PSCALEENDZ)->EnableWindow(FALSE);

				(CButton*)GetDlgItem(IDC_PSCALESPEEDX)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_PSCALESPEEDY)->EnableWindow(TRUE);
				(CButton*)GetDlgItem(IDC_PSCALESPEEDZ)->EnableWindow(TRUE);
			}
		}
	}
}


void CTexPanel::OnRepeatCheck()
{
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);

	if(pFrame->m_SelectedKeys.GetSize()!=1) return;

	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);

	CSfxPartParticle* pPartParticle;

	if(pPart) 
	{
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE)
		{
			pPartParticle=(CSfxPartParticle*)pPart;

			m_bRepeat = !m_bRepeat;

			pPartParticle->m_bRepeat = m_bRepeat;
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnButtonloadsfx2() 
{
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"Sfx file (*.sfx)|*.sfx||");
	
	dlgFile.m_ofn.lpstrInitialDir=DIR_SFX;
	dlgFile.m_ofn.lpstrTitle=_T("머지로드");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		g_SfxObjMng.RemoveAll();
		CString filename=dlgFile.GetFileTitle();;
		filename.MakeLower();
		g_SfxMng.GetSfxBase(0)->m_strName=filename;
		g_SfxMng.GetSfxBase(0)->LoadMerge();
		m_pView->m_SfxObj.SetSfx(g_SfxMng.GetSfxBase(0));
		CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
		pFrame->m_SelectedKeys.RemoveAll();
		pFrame->m_nCurFrame=pFrame->m_nLCFrame=pFrame->m_nLeftFrame=0;
		pFrame->m_nCurPart=pFrame->m_nLCPart=pFrame->m_nTopObj=0;
		pFrame->m_pClickedKey=NULL;
		
#ifdef __AALLSELECT
		pFrame->m_bLayerDragDrop = FALSE;
		pFrame->m_nSfxPartClipBoard = -1;
#endif
		
		pFrame->UpdatePanel();
	}
}

void CTexPanel::OnChangeXscalrepeathigh() 
{
	CString strTemp;
	m_ScalRepeatXHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fScalSpeedXHigh =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeXscalrepeatlow() 
{
	CString strTemp;
	m_ScalRepeatXLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fScalSpeedXLow =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeYscalrepeathigh() 
{
	CString strTemp;
	m_ScalRepeatYHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fScalSpeedYHigh =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeYscalrepeatlow() 
{
	CString strTemp;
	m_ScalRepeatYLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fScalSpeedYLow =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeZscalrepeathigh() 
{
	CString strTemp;
	m_ScalRepeatZHigh.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fScalSpeedZHigh =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangeZscalrepeatlow() 
{
	CString strTemp;
	m_ScalRepeatZLow.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_fScalSpeedZLow =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscaleendx() 
{
	CString strTemp;
	m_ScalEndX.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScaleEnd.x =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscaleendy() 
{
	CString strTemp;
	m_ScalEndY.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScaleEnd.y =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}

void CTexPanel::OnChangePscaleendz() 
{
	CString strTemp;
	m_ScalEndZ.GetWindowText(strTemp);
	CFramePanel* pFrame=&((   (CChildFrame*)(m_pView->GetParent()) )->m_FramePanel);
	if(pFrame->m_SelectedKeys.GetSize()!=1) return;
	CSfxPart* pPart=g_SfxMng.GetSfxBase(0)->Part(pFrame->m_nCurPart);
	CSfxPartParticle* pPartParticle;
	if(pPart) {
		if(pPart->m_nType==SFXPARTTYPE_PARTICLE) {
			pPartParticle=(CSfxPartParticle*)pPart;
			pPartParticle->m_vScaleEnd.z =(float)( atof(strTemp) );
			m_pView->SetModified(true);
		}
	}
}
