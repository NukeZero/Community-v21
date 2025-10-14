// DlgBoneList.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "DlgBoneList.h"
#include ".\dlgbonelist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBoneList dialog


CDlgBoneList::CDlgBoneList( )//CWnd* pParent /*=NULL*/)
//	: CDialogBar( CDlgBoneList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBoneList)
	//}}AFX_DATA_INIT
}


void CDlgBoneList::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBoneList)
	DDX_Control(pDX, IDC_BUTTON_BONESAVE, m_btnBoneSave);
	DDX_Control(pDX, IDC_BUTTON_LINKEFFECT, m_btnLinkEffect);
	DDX_Control(pDX, IDC_BUTTON_MTEFF, m_btnMTE);
	DDX_Control(pDX, IDC_LIST_BONE, m_ctrlBoneList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBoneList, CDialogBar)
	//{{AFX_MSG_MAP(CDlgBoneList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BONE, OnItemchangedListBone)
	ON_BN_CLICKED(IDC_BUTTON_LINKEFFECT, OnButtonLinkeffect)
	ON_BN_CLICKED(IDC_BUTTON_BONESAVE, OnButtonBonesave)
	ON_BN_CLICKED(IDC_BUTTON_MTEFF, OnBnClickedButtonMteff)
	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_LINKEFFECT, OnUpdateButton1 )
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_BONESAVE, OnUpdateButton2 )
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_MTEFF, OnUpdateButton3 )

	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBoneList message handlers

void CDlgBoneList::ResetAllWnd( )
{
	m_ctrlBoneList.DeleteAllItems( );
}

void CDlgBoneList::OnUpdateButton1( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
}

void CDlgBoneList::OnUpdateButton2( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
}

void CDlgBoneList::OnUpdateButton3( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
}

void CDlgBoneList::Make_BoneList( CModelObject* pModel )
{
	if( !pModel )
		return;

	m_ctrlBoneList.DeleteAllItems( );

	CBones* pBones = pModel->m_pBone;
	if( !pBones )
		return;

	for( int i = 0; i < pBones->m_nMaxBone ; ++i )
	{
		BONE* pBone = pBones->GetBone( i );
		if( pBone )
			m_ctrlBoneList.InsertItem( i, pBone->m_szName );
	}
}

LONG CDlgBoneList::OnInitDialog( UINT wParam, LONG lParam ) 
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

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "bone_name";
	lvc.cx = 250;
	lvc.fmt = LVCFMT_LEFT;
	m_ctrlBoneList.InsertColumn( 0, &lvc );

	m_ctrlBoneList.SetExtendedStyle( LVS_EX_GRIDLINES );
	
	return TRUE;
}

void CDlgBoneList::SelectBone( const char* szBoneName )
{
	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	if( !pView )
		return;

	CModelObject* pModel = (CModelObject*)pView->m_pAutoModel;
	if( !pModel )
	{
		AfxMessageBox( "모델이 없습니다!" );
		return;
	}
		
	CBones* pBones = pModel->m_pBone;
	if( !pBones )
	{
		AfxMessageBox( "본없음 치명적" );
		return;
	}

	for( int i = 0; i < pBones->m_nMaxBone; ++i )
	{
		BONE* pBone = pBones->GetBone( i );
		if( !pBones )
			continue;

		if( 0 == strcmp( pBone->m_szName, szBoneName ) )
		{
			pBone->m_iOverload = 1;
			if( pView->m_pSfxUnit )
				pView->m_pSfxUnit->SetFollowBoneName( pBone->m_szName );

			pView->m_strFollowedBone = pBone->m_szName;
		}
		else
			pBone->m_iOverload = 0;
	}
}

void CDlgBoneList::SelectBone_Multi( const char* szBoneName )
{
	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	if( !pView )
		return;

	CModelObject* pModel = (CModelObject*)pView->m_pAutoModel;
	if( !pModel )
	{
		AfxMessageBox( "모델이 없습니다!" );
		return;
	}
		
	CBones* pBones = pModel->m_pBone;
	if( !pBones )
	{
		AfxMessageBox( "본없음 치명적" );
		return;
	}

	for( int i = 0; i < pBones->m_nMaxBone; ++i )
	{
		BONE* pBone = pBones->GetBone( i );
		if( !pBones )
			continue;

		if( 0 == strcmp( pBone->m_szName, szBoneName ) )
		{
			pBone->m_iOverload = 1;
//			if( pView->m_pSfxUnit )
//				pView->m_pSfxUnit->SetFollowBoneName( pBone->m_szName );

			pView->m_strFollowedBone = pBone->m_szName;

//			size_t size_max = pView->m_cSfxUnits.size();
//			if( size_max > 0 )
//				pView->m_cSfxUnits[ size_max - 1 ]->SetFollowBoneName( pBone->m_szName );
		}
		else
			pBone->m_iOverload = 0;
	}
}

void CDlgBoneList::OnItemchangedListBone(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	TCHAR szData[ 256 ];
	int iSelectedItem = pNMListView->iItem;
	m_ctrlBoneList.GetItemText( iSelectedItem, 0, szData, 255 );

	g_bMultiMode ? SelectBone_Multi( szData ) : SelectBone( szData );
	
	*pResult = 0;
}

void CDlgBoneList::OnButtonLinkeffect() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ctrlBoneList.GetFirstSelectedItemPosition( );
	if( pos == NULL )
	{
		AfxMessageBox( "SFX를 링크하기위해서 먼져 본을 선택해야 합니다." );
		return;
	}

	CFileDialog dlgFile( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		"sfx file (*.sfx)|*.sfx||");
	
	dlgFile.m_ofn.lpstrInitialDir="sfx\\";
	dlgFile.m_ofn.lpstrTitle=_T("load sfx file");
	dlgFile.m_ofn.Flags|=OFN_NOCHANGEDIR;
	if(dlgFile.DoModal()==IDOK) 
	{
		CString filename=dlgFile.GetPathName();
	//	filename.MakeLower();

		CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
		if( !pView )
			return;

		g_bMultiMode ? pView->Load_SFX_Multi( filename, "temp" ) : pView->Load_SFX( filename );

		//링크된 본이름을 보존한다. 이펙트만 바꿀경우를 위해 
		if( !g_bMultiMode )
		{
			pView->m_pSfxUnit->SetFollowBoneName( pView->m_strFollowedBone );

			//로드된 sfx이름을 보존한다. 링크될 본의 위치만 바꿀경우를 위해 
			m_strLoadedSfx = filename;
		}
	}
	
}

void CDlgBoneList::OnButtonBonesave() 
{
	if( g_bMultiMode )
	{
		AfxMessageBox( "멀티 뷰 모드에서는 저장할수 없습니다, 싱글 뷰 모드에서 저장하세요" );
		return;
	}

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	if( !pFrame )
		return;

	CMirandaView* pView = (CMirandaView*)pFrame->GetActiveView();
	if( !pView )
		return;

	size_t size = pView->m_cSfxUnits.size();
	if( size > 4 )
	{
		AfxMessageBox( "더이상 저장 할 수 없습니다. 동시에 출력가능한 Sfx는 5개입니다" );
		return;
	}

	// TODO: Add your control notification handler code here


	//pFrame->m_kEditUnit.ShowWindow( SW_SHOW );

	POSITION pos = m_ctrlBoneList.GetFirstSelectedItemPosition( );
	if (pos == NULL)
	{
		AfxMessageBox( " 등록하기 위해서는 본을 선택하고 Sfx를 링크시켜야 합니다" );
		return;
	}
	else
	{
	  int nItem = m_ctrlBoneList.GetNextSelectedItem(pos);
	  // you could do your own processing on nItem here

	  CString strBone = m_ctrlBoneList.GetItemText( nItem, 0 );
	  pFrame->m_kEditUnit.m_stcBoneName = strBone;
	  CString strSfx = GetFileName( m_strLoadedSfx );
	  pFrame->m_kEditUnit.m_stcSfxName = strSfx;
	}

	//loop check button init
	pFrame->m_kEditUnit.m_bLoop = FALSE;

	BOOL bModalOK = FALSE;
	int iCase = -1;
	CString selectedString;
	while( !bModalOK )
	{
		if( IDOK == pFrame->m_kEditUnit.DoModal( ) )
		{
			// 데이터 등록 
			selectedString = pFrame->m_kEditUnit.m_strSelectedComboList;

			if( selectedString.IsEmpty() )
			{
				AfxMessageBox( "Effect 출력시점을 반드시 선택해야 합니다!" );
				bModalOK = FALSE;
			}
			else bModalOK = TRUE;
		}
		else return;
	}
	
	pFrame->m_kFinal.ShowWindow( SW_SHOWNORMAL );

	BOOL bLoop = pFrame->m_kEditUnit.m_bLoop;
	CString strLoop = ( bLoop ? "1" : "0" );

	pFrame->m_kFinal.AddData( CDlgFinal::EK_LINKSFX, selectedString, pFrame->m_kEditUnit.m_stcSfxName, pFrame->m_kEditUnit.m_stcBoneName, strLoop );
}

void CDlgBoneList::OnBnClickedButtonMteff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//멀티 텍스쳐 효과를 추가하려고 한다.
	if( m_kMTE.DoModal() == IDOK )
	{
		//여기서 데이터를 등록 

		if( -1 == m_kMTE.m_iSelectCase )			//어떤 상태에서를 선택하지 않으면 추가무시
			return;

		CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
		if( !pFrame )
			return;

		pFrame->m_kFinal.AddData( CDlgFinal::EK_MULTITEXTURING, m_kMTE.m_strSelectedComboList, m_kMTE.m_str2ndTexture, CString("--"), CString("--") );

	}
}
