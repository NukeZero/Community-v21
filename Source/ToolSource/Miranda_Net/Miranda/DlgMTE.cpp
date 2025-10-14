// DlgMTE.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Miranda.h"
#include "DlgMTE.h"
#include ".\dlgmte.h"


// CDlgMTE 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMTE, CDialog)
CDlgMTE::CDlgMTE(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMTE::IDD, pParent)
	, m_iSelectCase(0)
{
}

CDlgMTE::~CDlgMTE()
{
}

void CDlgMTE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_kComboCase);
	DDX_CBIndex(pDX, IDC_COMBO1, m_iSelectCase);
	DDX_Control(pDX, IDC_EDIT_2ND_TEXTURE, m_kEdit2ndTexture);
}


BEGIN_MESSAGE_MAP(CDlgMTE, CDialog)
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON_2ND_TEXTURE, OnBnClickedButton2ndTexture)
END_MESSAGE_MAP()


// CDlgMTE 메시지 처리기입니다.

LONG CDlgMTE::OnInitDialog( UINT wParam, LONG lParam ) 
{
 	CDialog::OnInitDialog();

#ifdef __VS6
	if(!HandleInitDialog(wParam, lParam) || !UpdateData(FALSE))
	{
		return FALSE;
	}
#else
	//gmpbigsun: for VS2003
	HandleInitDialog( wParam, lParam );

	if( !UpdateData( FALSE ) )
	{
		TRACE0("Warning:UpdateData failed during dialog init. \n" );
		return FALSE;
	}
#endif

	
	// TODO: Add extra initialization here
	
	// 이펙트,사운드가 플레이되는 시점 리스트 
	m_kComboCase.InsertString( 0, "create" );
	m_kComboCase.InsertString( 1, "battle" );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMTE::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSel = m_kComboCase.GetCurSel( );
	m_kComboCase.GetLBText( iSel, m_strSelectedComboList );
}

void CDlgMTE::OnBnClickedButton2ndTexture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"dds file (*.dds)|*.dds||");
	
	dlgFile.m_ofn.lpstrInitialDir="Sfx\\Texture\\";
	dlgFile.m_ofn.lpstrTitle=_T("load dds file");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		CString filename=dlgFile.GetFileName();
		filename.MakeLower();
		m_kEdit2ndTexture.SetWindowText( (LPCTSTR)filename );

		m_str2ndTexture = filename;
	}
}
