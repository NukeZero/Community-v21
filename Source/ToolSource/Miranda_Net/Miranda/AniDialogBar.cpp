// AniDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "AniDialogBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAniDialogBar dialog


CAniDialogBar::CAniDialogBar()
//	: CDialogBar(CAniDialogBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAniDialogBar)
	//}}AFX_DATA_INIT
}


void CAniDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAniDialogBar)
	DDX_Control(pDX, IDC_LIST1, m_ctrlAniList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAniDialogBar, CDialogBar)
	//{{AFX_MSG_MAP(CAniDialogBar)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAniDialogBar message handlers

LONG CAniDialogBar::OnInitDialog( UINT wParam, LONG lParam ) 
{

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
	
	//CDialogBar::OnInitDialog();
	
	// TODO: Add extra initialization here
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "ani_name";
	lvc.cx = 250;
	lvc.fmt = LVCFMT_LEFT;
	m_ctrlAniList.InsertColumn( 0, &lvc );

	m_ctrlAniList.SetExtendedStyle( LVS_EX_GRIDLINES );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAniDialogBar::ResetAllWnd( )
{
	m_ctrlAniList.DeleteAllItems( );
}

int CAniDialogBar::ListUpAllAnimation( const TCHAR* szFilename )
{
	int nAni = 0;

	// TODO : 관련이 있는 Ani파일을 모두 읽어드린후 list up!

	CString szOrgName;
	
	if( strstrlwr( szFilename, "part_" ) )
	{
		if( strstrlwr( szFilename, "part_m" ) ) //남성 
			szOrgName = "mvr_male";
		else szOrgName = "mvr_female";
	}else
	{
		szOrgName = GetFileTitle( szFilename );
	}
	
	// 1. 초기화 
	m_ctrlAniList.DeleteAllItems( );
	
	// 2. "*.ani"를 대상으로 szFilename의 확장자를 제거한 같은 문자열이 있다면 등록한다.
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;

	CString strPath = GetResourcePath();
	strPath += "Model/*.ani";
	
	// 지정 디렉토리에 모든 확장자 ani에 대하여 
	hSrch = FindFirstFile( strPath, &wfd );
	if( hSrch == INVALID_HANDLE_VALUE )
		return 0;

	
	int index = 0;
	while( bResult )
	{
		BOOL bFind = strstrlwr( wfd.cFileName, szOrgName.GetBuffer( 0 ) );		// abcd.o3d 라면 ani이름 규칙은 abcd_actionname 임 
		if( bFind )
		{
			//OK 이녀석 ani임
			m_ctrlAniList.InsertItem( index++, wfd.cFileName );
		}

		bResult = FindNextFile( hSrch, &wfd );
	}

	FindClose( hSrch );

	return nAni;
}

void CAniDialogBar::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	TCHAR szData[ 256 ];
	int iSelectedItem = pNMListView->iItem;
	m_ctrlAniList.GetItemText( iSelectedItem, 0, szData, 255 );

//	CString strPath = GetResourcePath();

	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	if( !pView )
		return;

	CModelObject* pModel = (CModelObject*)pView->m_pAutoModel;
	if( !pModel )
	{
		AfxMessageBox( "모델이 없습니다!" );
	}
	else
	{
//		strPath += "Model\\";
//		strPath += szData;
		pModel->LoadMotion( szData );
	}
	
	*pResult = 0;
}

void CAniDialogBar::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	if( !pView )
		return;

	CModelObject* pModel = (CModelObject*)pView->m_pAutoModel;

	if( pModel )
		pModel->InitAnimate( );
	
	*pResult = 0;
}
