// ModelSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sfxeditor.h"
#include "ModelSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelSettingDlg dialog


CModelSettingDlg::CModelSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelSettingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pView=NULL;
}


void CModelSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelSettingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CModelSettingDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnBoneLoad)
	ON_BN_CLICKED(IDC_BUTTON2, OnModelLoad)
	ON_BN_CLICKED(IDC_BUTTON3, OnMotionLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelSettingDlg message handlers

void CModelSettingDlg::OnBoneLoad() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"chr file (*.chr)|*.chr||");
	
	dlgFile.m_ofn.lpstrInitialDir=DIR_MODEL;
	dlgFile.m_ofn.lpstrTitle=_T("Bone 로드");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		m_strBoneFile = dlgFile.GetFileName();
	}
	else
		m_strBoneFile = "";
	
}

void CModelSettingDlg::OnModelLoad() 
{/*
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"o3d file (*.o3d)|*.o3d||");
	
	dlgFile.m_ofn.lpstrInitialDir=DIR_MODEL;
	dlgFile.m_ofn.lpstrTitle=_T("o3d 로드");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		m_strModelFile = dlgFile.GetFileName();
	}
	else
		m_strModelFile = "";*/
}

void CModelSettingDlg::OnMotionLoad() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"ani file (*.ani)|*.ani||");
	
	dlgFile.m_ofn.lpstrInitialDir=DIR_MODEL;
	dlgFile.m_ofn.lpstrTitle=_T("ani 로드");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		m_strAniFile = dlgFile.GetFileName();
	}
	else
		m_strAniFile = "";
	
}


void CModelSettingDlg::OnOK() 
{
	if( !m_strBoneFile.IsEmpty() )
	{
		SAFE_DELETE(m_pView->m_pModel);

		m_pView->m_pModel = new CModelObject;
		m_pView->m_pModel->InitDeviceObjects( m_pView->m_pd3dDevice );
		m_pView->m_pModel->LoadBone( m_strBoneFile );

		if( _stricmp( m_strBoneFile.GetBuffer(0), "mvr_female.chr" ) == 0 )
		{
			m_pView->m_pModel->LoadElement("Part_femaleHair01.o3d", 0 );
			m_pView->m_pModel->LoadElement("Part_femaleHand.o3d",   1 );
			m_pView->m_pModel->LoadElement("Part_femaleHead01.o3d", 2 );
			m_pView->m_pModel->LoadElement("Part_femaleLower.o3d",  3 );
			m_pView->m_pModel->LoadElement("Part_femaleUpper.o3d",  4 );
			m_pView->m_pModel->LoadElement("Part_femaleFoot.o3d",   5 );
		}
		else
		if( _stricmp( m_strBoneFile.GetBuffer(0), "mvr_male.chr" ) == 0 )
		{
			m_pView->m_pModel->LoadElement("Part_maleHair01.o3d", 0 );
			m_pView->m_pModel->LoadElement("Part_maleHand.o3d",   1 );
			m_pView->m_pModel->LoadElement("Part_maleHead01.o3d", 2 );
			m_pView->m_pModel->LoadElement("Part_maleLower.o3d",  3 );
			m_pView->m_pModel->LoadElement("Part_maleUpper.o3d",  4 );
			m_pView->m_pModel->LoadElement("Part_maleFoot.o3d",   5 );
		}
		else
		{
			m_strBoneFile.Replace( ".chr", ".o3d" );
			
			m_pView->m_pModel->LoadModel(m_strBoneFile);
		}
			
		m_pView->m_pModel->LoadMotion( m_strAniFile );
	}
	else
		m_pView->m_pModel->LoadMotion( m_strAniFile );
		
	CDialog::OnOK();
}
