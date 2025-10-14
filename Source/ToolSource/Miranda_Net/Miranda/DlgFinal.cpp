// DlgFinal.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "DlgFinal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int MAX_BUFFER = 64;

/////////////////////////////////////////////////////////////////////////////
// CDlgFinal dialog


CDlgFinal::CDlgFinal(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFinal::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFinal)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgFinal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFinal)
	DDX_Control(pDX, IDC_LIST_FINAL, m_ctrlData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFinal, CDialog)
	//{{AFX_MSG_MAP(CDlgFinal)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FINAL, OnItemchangedListFinal)
	ON_BN_CLICKED(IDSAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFinal message handlers

BOOL CDlgFinal::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	const int MAX_CULOM = 6;

	static _TCHAR* szColumnLabel[ MAX_CULOM ] =
	{
		_T("CASE"), _T("KIND"), _T("Link SFX"), _T("Bone"), _T("Loop"), _T("Sound")
	};
	static int nColumnWidth[ MAX_CULOM ] =
	{
		100, 150, 250, 250, 60, 200
	};

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	for( int i = 0; i < MAX_CULOM; ++i )
	{
		lvc.iSubItem = i;
		lvc.pszText = szColumnLabel[i];
		lvc.cx = nColumnWidth[i];
		lvc.fmt = LVCFMT_LEFT;
		m_ctrlData.InsertColumn( i, &lvc );
	}

	m_ctrlData.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

// 	CRect rect;
// 	GetWindowRect( &rect );
// 
// 	CRect parentRect;
// 	((CMainFrame*)AfxGetMainWnd())->GetWindowRect( &parentRect );
// 
// 	CRect rstRect = rect;
// 	rstRect.top = parentRect.bottom - rect.Height();
// 	rstRect.bottom = rstRect.top + rect.Height();
// 	
// 	MoveWindow( rstRect );
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFinal::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ctrlData.GetFirstSelectedItemPosition( );
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{

	  int nItem = m_ctrlData.GetNextSelectedItem(pos);
	  // you could do your own processing on nItem here

	  SubData( nItem );
	}
}


void CDlgFinal::AddData( EFF_KIND eKind, CString& strCase, CString& strLinkSfx, CString& strBone, CString& strLoop )
{
	int iCount = m_ctrlData.GetItemCount( );

	LVITEM kItem;
	kItem.mask = LVIF_TEXT;
	kItem.iItem = iCount;		//아이디가 아님 걍 사이즈 인덱스임 ( 임의로 지정하면 안될껄 )

	kItem.iSubItem = 0; 
	kItem.pszText = (LPSTR)strCase.GetBuffer(0);
	m_ctrlData.InsertItem( &kItem );

	CString strKind;
	switch( eKind )
	{
	case EK_LINKSFX : strKind = "LINKED_SFX"; break;
	case EK_MULTITEXTURING : 
		strKind = "MULTI_TEXTURING"; 
		//strLinkSfx = "--";
		strBone = "--";
		strLoop = "--";
		break;
	}

	ListView_SetItemText( m_ctrlData.GetSafeHwnd(), kItem.iItem, 1, strKind.GetBuffer(0) );
	ListView_SetItemText( m_ctrlData.GetSafeHwnd(), kItem.iItem, 2, strLinkSfx.GetBuffer(0) );
	ListView_SetItemText( m_ctrlData.GetSafeHwnd(), kItem.iItem, 3, strBone.GetBuffer(0) );
	ListView_SetItemText( m_ctrlData.GetSafeHwnd(), kItem.iItem, 4, strLoop.GetBuffer(0) );

}

void CDlgFinal::SubData( const int nItem )
{
	m_ctrlData.DeleteItem( nItem );
}

BOOL CDlgFinal::SaveData( CString& filename )
{
	//1. save to file by lua format        

	int nMax = m_ctrlData.GetItemCount( );
	if( nMax < 1 )
	{
		if( IDOK == AfxMessageBox( "저장할 목록이 없습니다! 파일내용을 지우시겠습니까?", MB_OKCANCEL ) )
		{
			//lua파일 제거 
		}
		else return FALSE;
	}

	CString strFileTitle = GetFileTitle( (LPCTSTR)filename );

	// filedialog 열어서 저장하게 하자.
	CFileDialog		dlgFileOpen( false, "lua", (LPCTSTR)filename, OFN_FILEMUSTEXIST,
								"LUA Files(*.lua)|*.lua|AllFiles(*.*)|*.*||", NULL );

//	CString strPath = GetResourcePath( );
//	strPath += "SFX/Scripts/";
//	strPath += filename;

	CString strPath;
	if ( dlgFileOpen.DoModal() == IDOK )
	{
		strPath = dlgFileOpen.GetFileName();
	}else
	{
		return FALSE;
	}

	DWORD dwWritten;
	HANDLE hFile = CreateFile( strPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == hFile )
		return FALSE;

	char buffer[5][MAX_BUFFER];
	CString strTemp;

	typedef set< CString >			CStrContainer;
	typedef CStrContainer::iterator CStrIter;
	CStrContainer cKeyStrings;
	CStrContainer cKeyStrings_copy;

	//중복된 상황 문자열을 검색하고 보존한다.
	for( int ix = 0; ix < nMax; ++ix )
	{
		m_ctrlData.GetItemText( ix, 0, buffer[ 0 ], MAX_BUFFER );
		cKeyStrings.insert( CStrContainer::value_type( CString( buffer[ 0 ] ) ) );
	}

	//카피본을 만든후 
	cKeyStrings_copy = cKeyStrings;

	strFileTitle.MakeLower();

	//복사본으로 중복된 상황함수만 전부 만든다.
	for( int ix = 0; ix < nMax; ++ix )
	{
		m_ctrlData.GetItemText( ix, 0, buffer[0], MAX_BUFFER );
		CStrIter finder = cKeyStrings_copy.find( CStrContainer::value_type( CString( buffer[0] ) ) );	

		if( finder != cKeyStrings_copy.end() )
		{
			//함수 시작 
			strTemp.Format( "\r\nfunction %s_%s( %s )", strFileTitle, buffer[0], "id_who" );		
			WriteFile( hFile, strTemp, strTemp.GetLength(), &dwWritten, NULL );		

			//함수 내용 
			WriteOverlapedFunc( hFile, buffer[ 0 ], nMax );

			//함수 끝
			WriteFile( hFile, "\r\nend\r\n", 7, &dwWritten, NULL );		

			// 함수작성이 끝난 상황은 검색목록에서 제거한다.
			cKeyStrings_copy.erase( finder++ );
		}
		
	}

	BOOL bLoop = FALSE;
	int nState = 0;
	EFF_KIND eKind = EK_NONE;

	//나머지 중복 안댄 상황함수 처리 
	for( int i = 0; i < nMax; ++i )
	{
		m_ctrlData.GetItemText( i, 0, buffer[0], MAX_BUFFER );		//case

		if( cKeyStrings.find( CString( buffer[0] ) ) != cKeyStrings.end() )			//이미 처리된 상황함수
			continue;

		m_ctrlData.GetItemText( i, 1, buffer[1], MAX_BUFFER );		//kind
		m_ctrlData.GetItemText( i, 2, buffer[2], MAX_BUFFER );		//sfx
		m_ctrlData.GetItemText( i, 3, buffer[3], MAX_BUFFER );		//bone
		m_ctrlData.GetItemText( i, 4, buffer[4], MAX_BUFFER );		//loop

		bLoop = atoi( buffer[4]	);
		nState = strcmp( buffer[0], "battle" ) == 0 ? OBJSTAF_COMBAT : 0;
	
		strTemp.Format( "\r\nfunction %s_%s( %s )", strFileTitle, buffer[0], "id_who" );
		WriteFile( hFile, strTemp, strTemp.GetLength(), &dwWritten, NULL );	
		
		const TCHAR* szSfxName = GetFileTitle( buffer[2] );

		eKind = StringToEK( buffer[ 1 ] );

		if( EK_LINKSFX == eKind )
			strTemp.Format( "\r\ncall_sfx(id_who, %d, \"%s\", \"%s\", %d, %d )", eKind, szSfxName, buffer[3], bLoop, nState );
		else
		if( EK_MULTITEXTURING == eKind )
			strTemp.Format( "\r\ncall_mte(id_who, %d, \"%s\")", eKind, szSfxName );

		WriteFile( hFile, strTemp, strTemp.GetLength(), &dwWritten, NULL );		
		WriteFile( hFile, "\r\nend\r\n", 7, &dwWritten, NULL );		
	}

	CloseHandle( hFile );
	
	filename = strPath;
	return TRUE;
}

void CDlgFinal::WriteOverlapedFunc( HANDLE hFile, const char* szCase, const int nMax )
{
	// 모든 등록된 리스트 아이템에 대해 case 문자열이 같은경우 함수만 등록한다 
	char buffer[5][MAX_BUFFER];
	CString strTemp;
	DWORD dwWritten;
	BOOL bLoop = FALSE;
	int nState = 0;
	BOOL bMultiEff = FALSE;

	for( int i = 0; i < nMax; ++i )
	{
		m_ctrlData.GetItemText( i, 0, buffer[0], MAX_BUFFER );
		if( 0 == strcmp( szCase, buffer[0] ) )	// 상황이 같다 ( 중복이다 )
		{
			m_ctrlData.GetItemText( i, 1, buffer[1], MAX_BUFFER );		//kind
			m_ctrlData.GetItemText( i, 2, buffer[2], MAX_BUFFER );		//sfx
			m_ctrlData.GetItemText( i, 3, buffer[3], MAX_BUFFER );		//bone
			m_ctrlData.GetItemText( i, 4, buffer[4], MAX_BUFFER );		//loop

			const TCHAR* szSfxName = GetFileTitle( buffer[2] );
			bLoop = atoi( buffer[4] );

			nState = strcmp( szCase, "battle" ) == 0 ? OBJSTAF_COMBAT : 0;
	
			EFF_KIND eKind = StringToEK( buffer[1] );

			// c++호출부만 만듬 
			if( EK_LINKSFX == eKind )
				strTemp.Format( "\r\ncall_sfx(id_who, %d, \"%s\", \"%s\", %d, %d )", eKind, szSfxName, buffer[3], bLoop, nState );
			else
			if( EK_MULTITEXTURING == eKind && !bMultiEff )
			{
				strTemp.Format( "\r\ncall_mte(id_who, %d, \"%s\")", eKind, szSfxName );
				bMultiEff = TRUE;
			}
			else 
			{
				if( bMultiEff )	// multi texturing effect를 중복해서 넣었다.. 
					AfxMessageBox( "Multi texturing effect가 같은상황에서 중복되었으므로 무시됩니다", MB_OK );
				else // 그외 Kind가 EK_LINKSFX or EK_MULTITEXTURING 가 아님
					AfxMessageBox( "effect의 Kind가 지원하는 값의 범위를 넘어섭니다" );

				continue;
			}

			WriteFile( hFile, strTemp, strTemp.GetLength(), &dwWritten, NULL );		
		}
	}

}

extern LPDIRECT3DTEXTURE9 g_pReflectMap;

void CDlgFinal::OnItemchangedListFinal(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	static TCHAR szOldCase[ 255 ];

	int iSelectedItem = pNMListView->iItem;
	if( -1 == iSelectedItem )
		return;

	TCHAR szKind[ MAX_BUFFER ];
	TCHAR szBone[ MAX_BUFFER ];
	TCHAR szSFX[ MAX_BUFFER ];
	TCHAR szSelCase[ MAX_BUFFER ];		//선택된 case
	TCHAR szCmpCase[ MAX_BUFFER ];		//비교할 case

	m_ctrlData.GetItemText( iSelectedItem, 3, szBone, MAX_BUFFER );
	m_ctrlData.GetItemText( iSelectedItem, 2, szSFX, MAX_BUFFER );
	m_ctrlData.GetItemText( iSelectedItem, 1, szKind, MAX_BUFFER );
	m_ctrlData.GetItemText( iSelectedItem, 0, szSelCase, MAX_BUFFER );

	EFF_KIND eKind = StringToEK( szKind );
			
	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	if( !pView )
		return;

	CString strSFX_fullpath = GetResourcePath();
	strSFX_fullpath += "SFX/";
	strSFX_fullpath += szSFX;

	CMainFrame* pFrame = ( CMainFrame* )AfxGetMainWnd( );
	if( !pFrame )
		return;

	CObject3D* pObj = pView->m_pAutoModel->GetObject3D( );
	if( !pObj )
	{
		assert( 0 );
		return;
	}
			
	pObj->m_dwEffectEX = 0;
	
	if( g_bMultiMode )
	{
		//멀티 모드에서는 선택한 항목의 case와 같은녀석을 모두 찾아 보여준다.
		pView->ClearSfxUnits( );
	
		int nMax = m_ctrlData.GetItemCount( );
		int nCase = 0;

		//일단 모든 컬럼상태를 초기화 한다.
		m_ctrlData.SetItemState( -1, 0, LVIS_SELECTED  );
		
		for( int ix = 0; ix < nMax; ++ix )
		{
			m_ctrlData.GetItemText( ix, 0, szCmpCase, MAX_BUFFER );

			if( 0 == strcmp( szSelCase, szCmpCase ) )
			{
				m_ctrlData.GetItemText( ix, 1, szKind, MAX_BUFFER );		//kind
				m_ctrlData.GetItemText( ix, 2, szSFX, MAX_BUFFER );			//sfx
				m_ctrlData.GetItemText( ix, 3, szBone, MAX_BUFFER );		//bone

				eKind = StringToEK( szKind );
				if( eKind == EK_LINKSFX )		
				{
					pFrame->m_kBoneList.SelectBone_Multi( szBone );
					strSFX_fullpath = GetResourcePath();
					strSFX_fullpath += "SFX/";
					strSFX_fullpath += szSFX;
					pView->Load_SFX_Multi( strSFX_fullpath, szBone );

					m_ctrlData.SetItemState( ix, LVIS_SELECTED, LVIS_SELECTED );

					strcpy( szOldCase, szSelCase );
				}
				else if( EK_MULTITEXTURING == eKind )
				{
					pObj->m_dwEffectEX = XE_MTE;
					pView->Load_MTE_Texture1( pObj->m_szFileName );
					pView->Load_MTE_Texture2( szSFX );

					m_ctrlData.SetItemState( ix, LVIS_SELECTED, LVIS_SELECTED );
				}
			}
		}

	}
	else // solo mode
	{
		
		if( EK_LINKSFX == eKind )
		{
			pView->Load_SFX( strSFX_fullpath );
			pFrame->m_kBoneList.SelectBone( szBone );
		}
		else if( EK_MULTITEXTURING == eKind )
		{
			pObj->m_dwEffectEX = XE_MTE;
			pView->Load_MTE_Texture1( pObj->m_szFileName );
			pView->Load_MTE_Texture2( szSFX );
		}
	}
	
	*pResult = 0;

	
}

void CDlgFinal::OnSave() 
{
	// TODO: Add your control notification handler code here

	// 파일이름 규칙 
	// o3d 파일이름을 그대로 쓴다 firedog.o3d->firedog.lua

	//1. 현재 로드된 o3d이름을 가져오자 
	
	CMirandaView* pView = (CMirandaView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveView();
	assert( pView );

	CString strName = GetFileTitle(pView->m_strLoadedO3DName);
	if( strName.Find( "Part_f" ) != -1 )
		strName = "mvr_female";
	else if( strName.Find( "Part_m" ) != -1 )
		strName = "mvr_male";

	//2. 파일이름은 "titlename.lua"
	CString strFileName = strName + ".lua";
	
	BOOL bOK = SaveData( strFileName );

	strFileName += "로 저장 되었습니다";

	if( bOK )
		AfxMessageBox( strFileName );
	else 
		AfxMessageBox( "저장이 취소되었거나, 저장할수 없습니다" );
}

BOOL CDlgFinal::ReadData( const char* filename )
{
	m_ctrlData.DeleteAllItems();

	CMainFrame* pFrame = ( CMainFrame* )AfxGetMainWnd( );

	CString strKey;
	CString strSfx;
	CString strBone;
	CString strLoop;

	EFF_KIND eKind = EK_NONE;

	CScanner scanner;
	if( scanner.Load( filename ) == FALSE )
	{
		//Message( "%s not found", filename );
		return FALSE;
	}

	do {
		scanner.GetToken();

		if(scanner.Token == "function")
		{	
			scanner.GetToken( );
			if( scanner.Token.Find( "_create" ) != -1 )
			{
				strKey = "create";
			}
			else if( scanner.Token.Find( "_peace" ) != -1 )
			{
				strKey = "peace";
			}
			else if( scanner.Token.Find( "_battle" ) != -1 )
			{
				strKey = "battle";
			}
			else
			{

				assert( 0 );
				return FALSE;
			}
		}
		else if( scanner.Token == "end" )
		{
			strKey = "";
		}
		else if ( scanner.Token == "call_sfx" )
		{
			assert( !strKey.IsEmpty() );

			scanner.GetToken();
			scanner.GetToken( TRUE );		//id
			eKind = (EFF_KIND)scanner.GetNumber( TRUE );		//kind
			scanner.GetToken( TRUE );		//sfx
			strSfx = scanner.Token;
			scanner.GetToken( TRUE);		//bone
			strBone = scanner.Token;

			scanner.GetToken( TRUE);		//loop
			strLoop = scanner.Token;

			scanner.GetToken( TRUE );	// state는 이미 함수명에서 체크했으므로 그냥 읽기만 한다.
		
		//	strLoop.Remove( ')' );

			//lua에는 sfx확장자가 없다.
			strSfx += ".sfx";

			pFrame->m_kFinal.AddData( eKind, strKey, strSfx, strBone, strLoop );
		}
		else if( scanner.Token == "call_mte" )
		{
			scanner.GetToken( TRUE ); //"(id_who"
			
			eKind = (EFF_KIND)scanner.GetNumber( TRUE );

			scanner.GetToken( );
			strSfx = scanner.Token;
			strSfx += ".dds";
			
			pFrame->m_kFinal.AddData( eKind, strKey, strSfx, CString("--"), CString("--") );
			
		}
	
		
	} while(scanner.tok!=FINISHED);

	return TRUE;
}

CDlgFinal::EFF_KIND CDlgFinal::StringToEK( const CString& str )
{
	if( "LINKED_SFX" == str )
		return EK_LINKSFX;
	else if( "MULTI_TEXTURING" )
		return EK_MULTITEXTURING;

	return EK_NONE;
}

CString CDlgFinal::EKToString( EFF_KIND eKind )
{
	switch( eKind )
	{
	case EK_LINKSFX : return CString("LINKED_SFX"); 
	case EK_MULTITEXTURING : return CString("MULTI_TEXTURING");
	}

	assert( 0 );

	return CString("");
}



