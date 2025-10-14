// ObjProperty.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "ObjProperty.h"
#include "WorldDoc.h"
#include "WorldView.h"
#include "ModelView.h"
#include "DlgEditString.h"
#include ".\objproperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjProperty

IMPLEMENT_DYNAMIC(CObjProperty, CPropertySheet)

CObjProperty::CObjProperty(CWorldView* pWorldView,UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pWorldView = pWorldView;
}

CObjProperty::CObjProperty(CWorldView* pWorldView,LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pWorldView = pWorldView;
}

CObjProperty::~CObjProperty()
{
}


BEGIN_MESSAGE_MAP(CObjProperty, CPropertySheet)
	//{{AFX_MSG_MAP(CObjProperty)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_BN_CLICKED(IDOK,OnOk)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjProperty message handlers

void CObjProperty::OnOk() 
{ 
	SendMessageToDescendants(WM_COMMAND,IDOK); 
	m_pWorldView->SendMessage(WM_CLOSE_OBJ_PROP_SHEET);
//	DestroyWindow();
	//ropertySheet::OnOk();
}
void CObjProperty::OnCancel() 
{
	SendMessageToDescendants(WM_COMMAND,IDCANCEL); 
	m_pWorldView->SendMessage(WM_CLOSE_OBJ_PROP_SHEET);
//	DestroyWindow();
	//CPropertySheet::OnCancel();
}

BOOL CObjProperty::DestroyWindow() 
{
	//	SendMessageToDescendants(WM_COMMAND,IDCANCEL); 
	//m_pFieldView->SendMessage(WM_CLOSE_OBJ_PROP_SHEET);
	//ndMessageToDescendants(WM_COMMAND,IDCANCEL); 
	//pFieldView->SendMessage(WM_CLOSE_OBJ_PROP_SHEET);

	return 0;//CPropertySheet::DestroyWindow();
}
BOOL CObjProperty::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	// TODO: Add your specialized code here
	CRect rect1;
	CRect rect2;
	GetClientRect(&rect2);
	GetWindowRect(&rect1);
	rect1.bottom += 35;
	MoveWindow(rect1);
	CRect rect3(10,rect2.bottom+5,90,rect2.bottom+27);
	CRect rect4(100,rect2.bottom+5,200,rect2.bottom+27);
	m_buttonOk.Create("확인",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect3, this, IDOK);
	m_buttonCancel.Create("취소",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect4, this, IDCANCEL);
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropObj property page

IMPLEMENT_DYNCREATE(CPagePropObj, CPropertyPage)

CPagePropObj::CPagePropObj( ) : CPropertyPage(CPagePropObj::IDD)
{
	//{{AFX_DATA_INIT(CPagePropObj)
	m_bLandIndependence = FALSE;
	//}}AFX_DATA_INIT
	m_pObj = NULL;

	m_strOBJID			= _T("");
	m_strModifyOBJID	= _T("");
	m_strModifyOBJName	= _T("");

}
void CPagePropObj::Init( CObj* pObj )
{
	m_pObj = pObj;
}

CPagePropObj::~CPagePropObj()
{
}

void CPagePropObj::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropObj)
	DDX_Check(pDX, IDC_LAND_INDEPENDENCE, m_bLandIndependence);
	DDX_Text(pDX, IDC_OBJID, m_strOBJID);
	DDX_Text(pDX, IDC_MODIFY_OBJID, m_strModifyOBJID);
	DDX_Text(pDX, IDC_MODIFY_OBJNAME, m_strModifyOBJName);
	//}}AFX_DATA_MAP
}

BOOL CPagePropObj::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	MODELELEM* pModelElem = NULL;
	pModelElem = prj.m_modelMng.GetModelElem( OT_OBJ, m_pObj->m_dwIndex );

	if( pModelElem != NULL )
	{
		m_strOBJID.Format( "%s - ( %d )", pModelElem->m_szName, m_pObj->m_dwIndex );
	}
	else
	{
		AfxMessageBox( "[ERROR] Unregisterd Obj -> onInitDialog" );
	}

	m_strModifyOBJID = _T("");
	m_strModifyOBJName	= _T("");
//	m_pObj->GetModel()
//	m_strOBJID.Format( "%s (%d)", pPropMoverID, m_pObj->m_dwIndex );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPagePropObj::OnBnClickedFindObjid()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );

	if( IsValidObj( m_pObj ) == FALSE )
	{
		AfxMessageBox("[ERROR] OnBnClickedFindObjid invalid Object .");

		return;
	}

	DWORD	dwOrgOBJID	= m_pObj->GetIndex();
	DWORD	dwModifyOBJID	= atoi(m_strModifyOBJID);

	if( dwModifyOBJID == 0 )
	{
		AfxMessageBox("incorrect oid.");

		return;
	}
	else if( dwModifyOBJID == dwOrgOBJID )
	{
		AfxMessageBox("oid and original is same.");

		return;
	}
	else
	{
		MODELELEM* pModelElem = NULL;
		pModelElem = prj.m_modelMng.GetModelElem( OT_OBJ, dwModifyOBJID );

		if( pModelElem != NULL )
		{
			m_strModifyOBJName = pModelElem->m_szName;

			UpdateData( FALSE );
		}
	}

// 	int	nModelElemCount	= prj.m_modelMng.GetModelElemCount( OT_OBJ );
// 	
// 	MODELELEM* lpModelElem = lpModelElem = prj.m_modelMng.GetModelElem( OT_OBJ, i );
// 
// 	prj.m_modelMng.G
// 	
// 	for( int i=0; i< nModelElemCount; ++i )
// 	{
// 		lpModelElem = prj.m_modelMng.GetModelElem( OT_OBJ, i );
// 	}
// 	OT_OBJ


// 	OnInitDialog();
// 	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
// 	pBeastView->Invalidate( FALSE );
// 
// 	CModelView* pModelView = (CModelView*)g_pMainFrame->m_wndWorkspaceBar.GetView( RUNTIME_CLASS( CModelView ) );
// 	CTreeCtrl* pTree = &pModelView->GetTreeCtrl();
// 	HTREEITEM hObjectTreeItem = pTree->GetChildItem( pModelView->GetHTreeObject() );
// 
// 	CString strName;	//	이름만 받아서 체크하여 값을 넣어 준다.
// 	DWORD	dwOBJType	= 0;
// 
// 	int	aaa = 1;
// 
// 
// 	LPMODELELEM CModelMng::GetModelElem( DWORD dwType, DWORD dwIndex )
// 
// 	while( hObjectTreeItem != NULL )
// 	{
// 		aaa = 1;
// 
// 		dwOBJType = pTree->GetItemData( hObjectTreeItem );
// 		strName	= pTree->GetItemText( hObjectTreeItem );
// 
// 		if( IS_FOLDER( dwOBJType ) == TRUE )
// 		{
// 			hObjectTreeItem	= pTree->GetNextVisibleItem( hObjectTreeItem );
// 
// 			continue;
// 		}
// 		else
// 		{
// 			dwOBJType = pTree->GetItemData( hObjectTreeItem );
// 			strName	= pTree->GetItemText( hObjectTreeItem );
// 
// 			aaa = 0;
// 		}
// // 		hObjectTreeItem->Get
// // 		if( IS_FOLDER( dwOBJType ) == TRUE )
// // 		{
// // 			hChildItem = pTree->GetChildItem( hObjectTreeItem );
// // 
// // 			while( hChildItem )
// // 			{
// // 				if( IS_FOLDER( hChildItem ) == TRUE )
// // 				{
// // 				}
// // 			}
// // 		}
// 
// 		HTREEITEM	hNextTreeItem = pTree->GetNextSiblingItem( hObjectTreeItem );
// 
// 		if( hNextTreeItem == NULL )
// 		{
// 			hObjectTreeItem	= pTree->GetParentItem( hObjectTreeItem );
// 
// 			dwOBJType = pTree->GetItemData( hObjectTreeItem );
// 			strName	= pTree->GetItemText( hObjectTreeItem );
// 
// 			if( hObjectTreeItem != NULL )
// 			{
// 				if( hObjectTreeItem	== pModelView->GetHTreeObject() )
// 				{
// 					break;
// 				}
// 				else
// 				{
// 					hObjectTreeItem = pTree->GetNextSiblingItem( hObjectTreeItem );
// 				}
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}
// 		else
// 		{
// 			hObjectTreeItem = hNextTreeItem;
// 		}
// 	}

// 	while (	hObjectTreeItem != NULL) 
// 	{
// 		DWORD	dwOBJType	= pTree->GetItemData( hObjectTreeItem );
// 		strName	= pTree->GetItemText( hObjectTreeItem );
// 
// 		if( IS_FOLDER( dwOBJType ) ) 
// 		{
// 			hChildItem = pTree->GetChildItem( hObjectTreeItem );
// 			dwOBJType	= pTree->GetItemData( hObjectTreeItem );
// ;.
// 			while ( hChildItem != NULL )
// 			{
// 				strName		= pTree->GetItemText( hChildItem );
// 				dwOBJType	= pTree->GetItemData( hObjectTreeItem );
// 
// 				if ( IS_FOLDER( pTree->GetItemData( hChildItem ) ) )
// 				{
// 					strName = pTree->GetItemText( hChildItem );
// 				}
// 				else
// 				{
// //					LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( dwOBJType, dwOBJType );
// //					strName = pTree->GetItemText( hChildItem );
// // 					switch( dwOBJType )
// // 					{
// // 					case OT_OBJ   : strHeadWord = _T( "OI_" ); break;
// // 					case OT_CTRL  : strHeadWord = _T( "CI_" ); break;
// // 					case OT_SFX   : strHeadWord = _T( "XI_" ); break;
// // 					case OT_ITEM  : strHeadWord = _T( "II_" ); break;
// // 					case OT_MOVER : strHeadWord = _T( "MI_" ); break;
// // 					case OT_REGION: strHeadWord = _T( "RI_" ); break;
// // 					}
// 
// 				}
// 				hChildItem = pTree->GetNextSiblingItem( hChildItem );
// 			}
// 
// 			strName	= pTree->GetItemText( hChildItem );
// // 			hTerChild = pTree->GetChildItem( hTerrain );
// // 			while ( hTerChild != NULL )
// // 			{
// // 				if ( IS_FOLDER( pTree->GetItemData( hTerChild ) ) )
// // 				{
// // 					strName = pTree->GetItemText( hTerChild );
// // 					pComboBox->AddString( strName );
// // 					bChk = TRUE;
// // 				}
// // 				hTerChild = pTree->GetNextSiblingItem( hTerChild );
// // 			}
// // 			if ( bChk != TRUE )
// // 			{
// // 				pComboBox->AddString( strName );
// // 			}
// 
// //			DWORD	dwObjectID	= pTree->GetItemData(
// // 			dwType &= ~FOLDER;
// // 			strName = pTree->GetItemText( hItem );
// // 			string.Format( _T( "%s\"%s\" %d\r\n" ), strSpace, strName, dwType );
// // 			file.PutWideString( string );
// // 			string.Format( _T( "%s{\r\n" ), strSpace );
// // 			file.PutWideString( string );
// // 			WriteModelScript( file, hChildItem, dwType & ~FOLDER, 2 );
// // 			string.Format( _T( "%s}\r\n" ), strSpace, strName );
// // 			file.PutWideString( string );
// 		}
// 
// //		strName	= pTree->GetItemText( hChildItem );
// 		hObjectTreeItem = pTree->GetNextSiblingItem( hObjectTreeItem );
// 	}

// 	while (	( hTerrain != NULL) )
// 	{
// 		if ( IS_FOLDER( pTree->GetItemData( hTerrain ) ) )
// 		{
// 			strName = pTree->GetItemText( hTerrain );
// 		}
// 		if ( strName.Find( "물", 0 ) != -1 )
// 		{
// 			hTerChild = pTree->GetChildItem( hTerrain );
// 			while ( hTerChild != NULL )
// 			{
// 				if ( IS_FOLDER( pTree->GetItemData( hTerChild ) ) )
// 				{
// 					strName = pTree->GetItemText( hTerChild );
// 					pComboBox->AddString( strName );
// 					bChk = TRUE;
// 				}
// 				hTerChild = pTree->GetNextSiblingItem( hTerChild );
// 			}
// 			if ( bChk != TRUE )
// 			{
// 				pComboBox->AddString( strName );
// 			}
// 		}
// 		hTerrain = pTree->GetNextSiblingItem( hTerrain );
// 	}
}

void CPagePropObj::OnBnClickedChangeObjid()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );

	if( IsValidObj( m_pObj ) == FALSE )
	{
		AfxMessageBox("Invalid Object -> OnBnClickedChangObjid.");

		return;
	}

	CWorld* pWorld	= NULL;
	pWorld	= m_pObj->GetWorld();

	if( pWorld == NULL )
	{
		AfxMessageBox("Wrong World -> OnBClickedChangeObjid.");

		return;
	}

	DWORD	dwOrgOBJID	= m_pObj->GetIndex();
	DWORD	dwModifyOBJID	= atoi(m_strModifyOBJID);

	if( dwModifyOBJID == 0 )
	{
		AfxMessageBox("onCLikcedChangeobjid cannot modify to 0.");

		return;
	}
	
	if( dwModifyOBJID == dwOrgOBJID )
	{
		AfxMessageBox("onCLikcedChangeobjid cannot modify to same.");

		return;
	}
	else
	{
		MODELELEM* pModelElem = NULL;
		pModelElem = prj.m_modelMng.GetModelElem( OT_OBJ, dwModifyOBJID );

		if( pModelElem != NULL )
		{
			BOOL	bModified = FALSE;
			bModified	= m_pObj->SetIndex( pWorld->m_pd3dDevice, dwModifyOBJID );

			if( bModified == TRUE )
			{
				CLandscape* pLand;
				pLand	= pWorld->GetLandscape( m_pObj );

				if( pLand != NULL )
				{
					pLand->SetModified( TRUE );
				}
			}
		}
		else
		{
			AfxMessageBox("objid incorrect or not found.");

			return;
		}

	}

// 	int			nCount	= pWorld->GetObjFocus()->GetIndex();
// 	int			nCount2	= 0;
// 
// 	CLandscape* pLand;// m_nVisibilityLand
// 
// 	int	nVisibilityLand	= pWorld->m_nVisibilityLand;
// 
// 	int			nCount3	= 0;
// 	int			nCount4	= 0;
// 
// 	int	nType	= 0;

// 	FOR_LAND( pWorld, pLand, nVisibilityLand, FALSE )
// 	{
// 		FOR_OBJARRAY( pLand, pObj )
// 		{
// 			nCount3	= nCount3 + 1;
// 			// 대상이 컨트롤이면.
// 			if( pObj->GetIndex()  == nCount )
// 			{
// 				int		ddd = 0;
// 			}
// 		}
// 		END_OBJARRAY
// 	}
// 	END_LAND

	//////////////////////////////////////////////////////////////////////////
/*	for( int i = 0; i < pWorld->m_nLandHeight; i++)
	{
		for( int j = 0; j < pWorld->m_nLandWidth; j++)
		{
			if( pWorld->m_apLand[ i * pWorld->m_nLandWidth + j] )
			{
//				BOOL bModified = bFullSave ? TRUE : m_apLand[ i * m_nLandWidth + j]->IsModified();
//				if( bModified )
				{
					CString strLand = pWorld->m_szFileName;
					strLand.Delete( strLand.GetLength() - 4, 4 );

					strLandTemp.Format( "%s%s%02d-%02d.lnd",  pWorld->m_szFilePath,  strLand, j , i );
					if( bSaveLND )
					{
						if( pWorld->m_apLand[ i * pWorld->m_nLandWidth + j]->SaveLandscape( strLandTemp, j, i ) == FALSE )
						{
							CString string;
							string.Format( "%s를 저장할 수 없습니다. Read only로 되어 있는지 확인하십시오." );
							AfxMessageBox( string );
							return;
						}
						else
							if( bFullSave == FALSE )
								 pWorld->m_apLand[ i *  pWorld->m_nLandWidth + j]->SetModified( FALSE );
					}
					if( bSaveDDS )
					{
						strLandTemp.Format( "%s%s%02d-%02d.dds", pWorld->m_szFilePath, strLand, j , i );
						 pWorld->SaveLandBitmap( strLandTemp, j, i );
					}
				}
			}
// 			else
// 			{
// 				//gmpbigsun: 전체저장의 경우 컬링 오브젝트 m_aobjCull에 쓰레기값이 들어가있어 죽는다.. 나중에 해결하고 일단 이렇게 
// #ifdef __BS_SAVE_2DMAP
// 				for( int icull = 0; icull < pWorld->m_nObjCullSize; icull++ )
// 					pWorld->m_aobjCull[ icull ] = NULL;
// 				for( int icull = 0; pWorld->m_nSfxCullSize < pWorld->m_nSfxCullSize; icull++ )
// 					pWorld->m_asfxCull[ icull ] = NULL;
// 
// 				pWorld->m_nObjCullSize = 0;
// 				pWorld->m_nSfxCullSize = 0;
// #endif
// 
// 				// 이곳으로 올 경우는 원본을 오픈 후 다른 폴더에 저장할 때 로딩하지 않은 랜드가 있다면
// 
// 				// 바이너리로 복사한다. 바이너리로 원본을 오픈하지 못하면 파일이 임의로 삭제되거나 문제가 생긴 것이다.
// 				if( pWorld->LandInWorld( j, i ) )
// 				{
// 					CString strLand = pWorld->m_szFileName;
// 					strLand.Delete( strLand.GetLength() - 4, 4 );
// 
// 					strLandTemp.Format( "%s%s%02d-%02d.lnd",  pWorld->m_szFilePath, strLand, j , i );
// 
// 					CLandscape* pLand = pWorld->m_apLand[ i * pWorld->m_nLandWidth + j];
// 					if( pLand == NULL )
// 					{
// 						pLand = new CLandscape;
// 						pLand->m_nWorldX = j * MAP_SIZE;
// 						pLand->m_nWorldY = i * MAP_SIZE;
// 						pLand->InitDeviceObjects( pWorld->m_pd3dDevice, this );
// 						if( pLand->LoadLandscape( strLandTemp, j, i ) == FALSE )
// 						{
// 							delete pLand;
// 							continue;
// 						}
// 						pLand->RestoreDeviceObjects(pWorld->m_pd3dDevice);
// 						pWorld->m_apLand[ i * pWorld->m_nLandWidth + j] = pLand;
// 
// 						pLand->SetModified( FALSE );
// 
// 						strLandTemp.Format( "%s%s%02d-%02d.lnd", pWorld->m_szFilePath, strLand, j , i );
// 						if( bSaveLND )
// 						{
// 							if( pWorld->m_apLand[ i * pWorld->m_nLandWidth + j]->SaveLandscape( strLandTemp, j, i ) == FALSE )
// 							{
// 								CString string;
// 								string.Format( "%s를 저장할 수 없습니다. Read only로 되어 있는지 확인하십시오." );
// 								AfxMessageBox( string );
// 								return FALSE;
// 							}
// 							else
// 								if( bFullSave == FALSE )
// 									pWorld->m_apLand[ i * pWorld->m_nLandWidth + j]->SetModified( FALSE );
// 						}
// 						if( bSaveDDS )
// 						{
// 							strLandTemp.Format( "%s%s%02d-%02d.dds", pWorld->m_szFilePath, strLand, j , i );
// 							pWorld->SaveLandBitmap( strLandTemp, j, i );
// 						}
// 
// #ifdef __BS_SAVE_2DMAP
// 						if( pLand )						//gmpbigsun: 메모리 부족으로 죽는데 쓰고 지워버리자 
// 							SAFE_DELETE( pLand );
// 
// 						pWorld->m_apLand[ i * pWorld->m_nLandWidth + j] = NULL;
// #endif
// 					}
// 
// 				}
// 			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
// 	for( int i=0; i<MAX_DISPLAYOBJ; i++ )
// 	{
// 		int	dwIndex = (*pWorld).m_aobjCull[1]->m_dwIndex;
// 
// 		if( dwIndex == 215 )
// 		{
// 			int	Test = 0;
// 		}
// 	}
//	pWorld->m_apObject;
*/

	OnInitDialog();
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	pBeastView->Invalidate( FALSE );

	return;
}

void CPagePropObj::OnBnClickedChangeallObjid()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	CWorld* pWorld	= m_pObj->GetWorld();

	DWORD	dwOrgOBJID	= m_pObj->GetIndex();
	DWORD	dwModifyOBJID	= atoi(m_strModifyOBJID);

	if( dwModifyOBJID == 0 )
	{
		AfxMessageBox("변경 할 OID를 입력하지 않았습니다.");

		return;
	}
	
	if( dwModifyOBJID == m_pObj->m_dwIndex )
	{
		AfxMessageBox("변경 할 OID가 원본과 같습니다.");

		return;
	}

	MODELELEM* pModelElem = NULL;
	pModelElem = prj.m_modelMng.GetModelElem( OT_OBJ, dwModifyOBJID );

	if( pModelElem == NULL )
	{
		AfxMessageBox("변경 할 OID가 잘못 입력 되었습니다.");

		return;
	}


	CLandscape*	pLand = NULL;
	CObj*		pObj = NULL;

	for( int i = 0; i < pWorld->m_nLandHeight; i++)
	{
		for( int j = 0; j < pWorld->m_nLandWidth; j++)
		{
			pLand = pWorld->m_apLand[ i * pWorld->m_nLandWidth + j];

			if( pLand != NULL )
			{
				FOR_OBJARRAY( pLand, pObj )
				{
					if( pObj->GetIndex() == dwOrgOBJID )
					{
						BOOL	bModified = FALSE;
						bModified	= pObj->SetIndex( pWorld->m_pd3dDevice, dwModifyOBJID );

						if( bModified == TRUE )
						{
							pLand->SetModified( TRUE );
						}
					}
				}
				END_OBJARRAY
			}
			else
			{
//				AfxMessageBox("pLand == NULL.");
			}
		}
	}

	OnInitDialog();
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	pBeastView->Invalidate( FALSE );

	return;
}

void CPagePropObj::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CPropertyPage::OnOK();
}


BEGIN_MESSAGE_MAP(CPagePropObj, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropObj)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHANGE_OBJID, OnBnClickedChangeObjid)
	ON_BN_CLICKED(IDC_CHANGEALL_OBJID, OnBnClickedChangeallObjid)
	ON_BN_CLICKED(IDC_FIND_OBJID, OnBnClickedFindObjid)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropObj message handlers
/////////////////////////////////////////////////////////////////////////////
// CPagePropLight dialog


CPagePropLight::CPagePropLight() : CPropertyPage(CPagePropLight::IDD)
{
	//{{AFX_DATA_INIT(CPagePropLight)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPagePropLight::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropLight)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropLight, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropLight)
	//}}AFX_MSG_MAP
//	ON_WM_INITMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropLight message handlers

void CPagePropLight::Init( CObj* pObj )
{
	m_pObj = pObj;
}
void CPagePropLight::OnOK()
{
	D3DXVECTOR3 vPos = m_pObj->GetPos();
	m_pObj->m_pLight = new CLight;
	m_pObj->m_pLight->SetLight( "", D3DLIGHT_POINT, vPos.x, vPos.y, vPos.z );
	m_pObj->m_pLight->Attenuation0 = 0.0f;
	m_pObj->m_pLight->Attenuation1 = 0.4f;
	m_pObj->m_pLight->Attenuation2 = 0.0f;
	m_pObj->m_pLight->Range        = 90.0f;
	m_pObj->m_pLight->Diffuse.r    = 1.0f;
	m_pObj->m_pLight->Diffuse.g    = 1.0f;
	m_pObj->m_pLight->Diffuse.b    = 1.0f;
	m_pObj->m_pLight->Specular.r   = 1.0f;
	m_pObj->m_pLight->Specular.g   = 1.0f;
	m_pObj->m_pLight->Specular.b   = 1.0f;
	m_pObj->m_pLight->Ambient.r    = 1.2f;
	m_pObj->m_pLight->Ambient.g    = 1.2f;
	m_pObj->m_pLight->Ambient.b    = 1.2f;

	CObjProperty* pObjProperty = (CObjProperty*)GetParent();
	m_pObj->m_pLight->Appear( pObjProperty->m_pWorldView->m_pd3dDevice );
}
void CPagePropLight::OnCancel()
{
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropRegion dialog


CPagePropRegion::CPagePropRegion() : CPropertyPage(CPagePropRegion::IDD)
{
	//{{AFX_DATA_INIT(CPagePropRegion)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPagePropRegion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropRegion)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropRegion, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropRegion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropRegion message handlers
void CPagePropRegion::OnOK()
{

}
void CPagePropRegion::OnCancel()
{
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropTrigger property page

IMPLEMENT_DYNCREATE(CPagePropTrigger, CPropertyPage)

CPagePropTrigger::CPagePropTrigger() : CPropertyPage(CPagePropTrigger::IDD)
{
	//{{AFX_DATA_INIT(CPagePropTrigger)
	m_strFunction = _T("");
	m_strMusic = _T("");
	m_strSound = _T("");
	m_fTeleX = 0.0f;
	m_fTeleY = 0.0f;
	m_fTeleZ = 0.0f;
	m_bBgm = FALSE;
	m_strTeleport = _T("");
	//}}AFX_DATA_INIT
	m_pRegion = NULL;
}

CPagePropTrigger::~CPagePropTrigger()
{
}
void CPagePropTrigger::Init( CRegion* pRegion )
{
	m_pRegion = pRegion;
}
void CPagePropTrigger::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropTrigger)
	DDX_CBString(pDX, IDC_FUNCTION, m_strFunction);
	DDX_CBString(pDX, IDC_MUSIC, m_strMusic);
	DDX_Text(pDX, IDC_SOUND, m_strSound);
	DDX_Text(pDX, IDC_TELE_X, m_fTeleX);
	DDX_Text(pDX, IDC_TELE_Y, m_fTeleY);
	DDX_Text(pDX, IDC_TELE_Z, m_fTeleZ);
	DDX_Check(pDX, IDC_BGM, m_bBgm);
	DDX_CBString(pDX, IDC_TELEPORT, m_strTeleport);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPagePropTrigger, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropTrigger)
	ON_BN_CLICKED(IDC_PLAY_SOUND, OnPlaySound)
	ON_BN_CLICKED(IDC_PLAY_MUSIC, OnPlayMusic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropTrigger message handlers

BOOL CPagePropTrigger::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_bBgm = m_pRegion->m_region.m_bDirectMusic;

	CComboBox* pComboMusic = (CComboBox*) GetDlgItem( IDC_MUSIC );
	CComboBox* pComboFunction = (CComboBox*) GetDlgItem( IDC_FUNCTION );
	CComboBox* pComboTeleport = (CComboBox*) GetDlgItem( IDC_TELEPORT );

	CStringArray strArray;
	CScript::GetFindIdToArray( "BGM_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
		pComboMusic->AddString( strArray.GetAt( i ) );

	m_strMusic = CScript::GetFindIdStr( "BGM_", m_pRegion->m_region.m_dwIdMusic );
	//pComboMusic->SelectString( 0, strMusic );

	strArray.RemoveAll();
	CScript::GetFindIdToArray( "WI_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
		pComboTeleport->AddString( strArray.GetAt( i ) );

	m_strTeleport = CScript::GetFindIdStr( "WI_", m_pRegion->m_region.m_dwIdTeleWorld );

	m_fTeleX = m_pRegion->m_region.m_vTeleWorld.x;
	m_fTeleY = m_pRegion->m_region.m_vTeleWorld.y;
	m_fTeleZ = m_pRegion->m_region.m_vTeleWorld.z;

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPagePropTrigger::OnOK( )
{
	UpdateData( TRUE );

	int nWorld = CScript::GetDefineNum( m_strTeleport );
	m_pRegion->m_region.m_dwIdMusic = CScript::GetDefineNum( m_strMusic );
	m_pRegion->m_region.m_bDirectMusic = ( m_bBgm != 0 );
	m_pRegion->m_region.m_dwIdTeleWorld =  nWorld;//
	m_pRegion->m_region.m_vTeleWorld = D3DXVECTOR3( m_fTeleX, m_fTeleY, m_fTeleZ );

}
/////////////////////////////////////////////////////////////////////////////
// CPagePropAttribute property page

IMPLEMENT_DYNCREATE(CPagePropAttribute, CPropertyPage)

CPagePropAttribute::CPagePropAttribute() : CPropertyPage(CPagePropAttribute::IDD)
{
	//{{AFX_DATA_INIT(CPagePropAttribute)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pRegion = NULL;
}

CPagePropAttribute::~CPagePropAttribute()
{
}
void CPagePropAttribute::Init( CRegion* pRegion )
{
	m_pRegion = pRegion;
}

void CPagePropAttribute::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropAttribute)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropAttribute, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropAttribute)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_SET_ALL, OnSetAll)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	ON_LBN_SELCANCEL(IDC_LIST1, OnSelcancelList1)
	ON_LBN_SETFOCUS(IDC_LIST1, OnSetfocusList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropAttribute message handlers

BOOL CPagePropAttribute::OnInitDialog() 
{
	#define MAKECHECK( x ) ( m_pRegion->m_region.m_dwAttribute & x ? CHECKED : UNCHECKED )

	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	CButton* pButton = (CButton*)GetDlgItem( IDC_CLEAR );
	CRect rectButton;
	pButton->GetClientRect( rectButton );
	CRect rect;
	GetClientRect( &rect );
	rect.right -= rectButton.Width() + 10;
	rect.DeflateRect( 10, 10 );
	m_checkList.Create( rect, this, 999, IDB_CHECK, IDB_UNCHECK, IDB_MIDDLECHECK );
	m_checkList.AddString( _T( "World Map"          ), NULL, MAKECHECK( RA_WORLD        ), RA_WORLD       );
	m_checkList.AddString( _T( "Dungeon Map"        ), NULL, MAKECHECK( RA_DUNGEON      ), RA_DUNGEON     );
	m_checkList.AddString( _T( "Newbie Map"         ), NULL, MAKECHECK( RA_NEWBIE       ), RA_NEWBIE      );
	m_checkList.AddString( _T( "Begin Map"          ), NULL, MAKECHECK( RA_BEGIN        ), RA_BEGIN       );
	m_checkList.AddString( _T( "Safety Region"      ), NULL, MAKECHECK( RA_SAFETY       ), RA_SAFETY      );
	m_checkList.AddString( _T( "Shrine Region"      ), NULL, MAKECHECK( RA_SHRINE       ), RA_SHRINE      );
	m_checkList.AddString( _T( "Town Region"        ), NULL, MAKECHECK( RA_TOWN         ), RA_TOWN         );
	m_checkList.AddString( _T( "Sight Region"       ), NULL, MAKECHECK( RA_SIGHT        ), RA_SIGHT       );
	m_checkList.AddString( _T( "Daylight Region"    ), NULL, MAKECHECK( RA_DAYLIGHT     ), RA_DAYLIGHT    );
	m_checkList.AddString( _T( "Fight Region"       ), NULL, MAKECHECK( RA_FIGHT        ), RA_FIGHT       );
	m_checkList.AddString( _T( "Penalty PK Region"  ), NULL, MAKECHECK( RA_PENALTY_PK   ), RA_PENALTY_PK  );
	m_checkList.AddString( _T( "Free PK Region"     ), NULL, MAKECHECK( RA_PK           ), RA_PK          );
	m_checkList.AddString( _T( "OX Region"          ), NULL, MAKECHECK( RA_OX           ), RA_OX          );
	m_checkList.AddString( _T( "Danger Region"      ), NULL, MAKECHECK( RA_DANGER       ), RA_DANGER      );
	m_checkList.AddString( _T( "No Chat Region"     ), NULL, MAKECHECK( RA_NO_CHAT      ), RA_NO_CHAT     );
	m_checkList.AddString( _T( "No Attack Region"   ), NULL, MAKECHECK( RA_NO_ATTACK    ), RA_NO_ATTACK   );
	m_checkList.AddString( _T( "No Damage Region"   ), NULL, MAKECHECK( RA_NO_DAMAGE    ), RA_NO_DAMAGE   );
	m_checkList.AddString( _T( "No Skill Region"    ), NULL, MAKECHECK( RA_NO_SKILL     ), RA_NO_SKILL    );
	m_checkList.AddString( _T( "No Item Region"     ), NULL, MAKECHECK( RA_NO_ITEM      ), RA_NO_ITEM     );
	m_checkList.AddString( _T( "No Teleport Region" ), NULL, MAKECHECK( RA_NO_TELEPORT  ), RA_NO_TELEPORT );
	m_checkList.AddString( _T( "Collecting Region" ), NULL, MAKECHECK( RA_COLLECTING  ), RA_COLLECTING );
	//CListBox* pWndListBox = (CListBox*)GetDlgItem( IDC_LIST1 );
	//pWndListBox->AddString("fuck");
	//pWndListBox->AddString("sex");
	//pWndListBox->AddString("date");
	//pWndListBox->AddString("file");
	//int q = pWndListBox->FindString( 0, "fua" );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPagePropAttribute::OnOK( )
{
//	UpdateData( TRUE );
	m_pRegion->m_region.m_dwAttribute = 0;
	int nCount = m_checkList.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		LISTITEM* lpListItem = m_checkList.GetItem( i );
		if( lpListItem->nCheckedState )
			m_pRegion->m_region.m_dwAttribute |= lpListItem->dwID;
	}
}
void CPagePropAttribute::OnClear() 
{
	int nCount = m_checkList.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		m_checkList.SetCheck( i, FALSE );
	}
}

void CPagePropAttribute::OnSetAll() 
{
	int nCount = m_checkList.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		m_checkList.SetCheck( i, TRUE );
	}
}

void CPagePropAttribute::OnInvert() 
{
	int nCount = m_checkList.GetCount();
	for( int i = 0; i < nCount; i++ )
	{
		m_checkList.SetCheck( i, !m_checkList.GetCheck( i ) );
	}
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropRespawn property page

IMPLEMENT_DYNCREATE(CPagePropRespawn, CPropertyPage)

CPagePropRespawn::CPagePropRespawn() : CPropertyPage(CPagePropRespawn::IDD)
{
	//{{AFX_DATA_INIT(CPagePropRespawn)
	m_nDelay = 0;
	m_bRespawnEnable = FALSE;
	m_nSum = 0;
	m_nActiveAttackNum = 0;
	m_nDayMax = 24;
	m_nDayMin = 1;
	m_nHourMax = 31;
	m_nHourMin = 1;
	m_nItemMax = 1;
	m_nItemMin = 1;
	//}}AFX_DATA_INIT
	m_pMover = NULL;
	m_pItem = NULL;
	m_pCtrl = NULL;
}
void CPagePropRespawn::Init( CObj* pObj )
{
	m_pMover = NULL;
	m_pItem = NULL;
	m_pCtrl = NULL;
	CRespawnInfo* pRespawnInfo = NULL;
	if( pObj->GetType() == OT_MOVER )
	{
		m_pMover = (CMover*) pObj;
		pRespawnInfo = &m_pMover->m_respawnInfo;
	}
	if( pObj->GetType() == OT_ITEM )
	{
		m_pItem = (CItem*) pObj;
		pRespawnInfo = &m_pItem->m_respawnInfo;
	}
	if( pObj->GetType() == OT_CTRL )
	{
		m_pCtrl = (CCommonCtrl*) pObj;
		pRespawnInfo = &m_pCtrl->m_respawnInfo;
	}
	if( pRespawnInfo )
	{
		m_nSum = pRespawnInfo->m_cb;
		m_nDelay = pRespawnInfo->m_uTime;
		m_nActiveAttackNum = pRespawnInfo->m_nActiveAttackNum;
		m_bRespawnEnable = pRespawnInfo->m_dwIndex ? TRUE : FALSE;
		m_nDayMin  = pRespawnInfo->m_nDayMin ;
		m_nDayMax  = pRespawnInfo->m_nDayMax ;
		m_nHourMin = pRespawnInfo->m_nHourMin;
		m_nHourMax = pRespawnInfo->m_nHourMax;
		m_nItemMin = pRespawnInfo->m_nItemMin;
		m_nItemMax = pRespawnInfo->m_nItemMax;

#ifdef __YAIMONSTER_EX
		if( pRespawnInfo->m_dwIndex )
			pObj->m_dwAIInterfaceState = pRespawnInfo->m_dwAiState;
#endif //__YAIMONSTER_EX
	}
}

CPagePropRespawn::~CPagePropRespawn()
{
}
void CPagePropRespawn::OnOK( )
{
	CRespawnInfo* pRespawnInfo = NULL;
	if( m_pMover  )
	{
		pRespawnInfo = &m_pMover->m_respawnInfo;
	}
	if( m_pItem )
	{
		pRespawnInfo = &m_pItem->m_respawnInfo;
	}
	if( m_pCtrl )
	{
		pRespawnInfo = &m_pCtrl->m_respawnInfo;
	}
	if( m_bRespawnEnable )
	{
		D3DXVECTOR3 vPos;// = m_pMover->GetPos();
		CRect rect( 0, 0, MPU * 4, MPU * 4);
		if( pRespawnInfo->m_dwIndex == 0 )
		{
			rect.left   = - MPU * 2;
			rect.top    = - MPU * 2;
			rect.right  = + MPU * 2;
			rect.bottom = + MPU * 2;
			pRespawnInfo->m_rect = rect;
		}
		if( m_pMover )
		{
			if( m_nActiveAttackNum > m_nSum )
				m_nActiveAttackNum = m_nSum;
			pRespawnInfo->m_dwIndex = m_pMover->GetIndex();
			pRespawnInfo->m_nActiveAttackNum = m_nActiveAttackNum;
		}
		else
		if( m_pItem )
		{
			pRespawnInfo->m_dwIndex = m_pItem->GetIndex();
			pRespawnInfo->m_nActiveAttackNum = 0;
		}
		else
		if( m_pCtrl )
		{
			pRespawnInfo->m_dwIndex = m_pCtrl->GetIndex();
			pRespawnInfo->m_nActiveAttackNum = 0;
		}
		pRespawnInfo->m_cb = m_nSum;
		pRespawnInfo->m_uTime = m_nDelay;
		pRespawnInfo->m_nDayMin  = m_nDayMin ;
		pRespawnInfo->m_nDayMax  = m_nDayMax ;
		pRespawnInfo->m_nHourMin = m_nHourMin;
		pRespawnInfo->m_nHourMax = m_nHourMax;
		pRespawnInfo->m_nItemMin = m_nItemMin;
		pRespawnInfo->m_nItemMax = m_nItemMax;
	}	
	else
	{
		pRespawnInfo->m_dwIndex = 0;
		pRespawnInfo->m_cb = 0;
		pRespawnInfo->m_uTime = 0;
		pRespawnInfo->m_nActiveAttackNum = 0;
	}
}
void CPagePropRespawn::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropRespawn)
	DDX_Text(pDX, IDC_DELAY, m_nDelay);
	DDV_MinMaxInt(pDX, m_nDelay, 0, 1000000);
	DDX_Check(pDX, IDC_RESPAWN_ENABLE, m_bRespawnEnable);
	DDX_Text(pDX, IDC_SUM, m_nSum);
	DDV_MinMaxInt(pDX, m_nSum, 0, 64);
	DDX_Text(pDX, IDC_ACTIVEATTACK, m_nActiveAttackNum);
	DDV_MinMaxInt(pDX, m_nActiveAttackNum, 0, 100);
	DDX_Text(pDX, IDC_EDIT_DAY_MAX, m_nDayMax);
	DDV_MinMaxInt(pDX, m_nDayMax, 1, 31);
	DDX_Text(pDX, IDC_EDIT_DAY_MIN, m_nDayMin);
	DDV_MinMaxInt(pDX, m_nDayMin, 1, 31);
	DDX_Text(pDX, IDC_EDIT_HOUR_MAX, m_nHourMax);
	DDV_MinMaxInt(pDX, m_nHourMax, 1, 24);
	DDX_Text(pDX, IDC_EDIT_HOUR_MIN, m_nHourMin);
	DDV_MinMaxInt(pDX, m_nHourMin, 1, 24);
	DDX_Text(pDX, IDC_EDIT_ITEM_MAX, m_nItemMax);
	DDV_MinMaxInt(pDX, m_nItemMax, 1, 255);
	DDX_Text(pDX, IDC_EDIT_ITEM_MIN, m_nItemMin);
	DDV_MinMaxInt(pDX, m_nItemMin, 1, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropRespawn, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropRespawn)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropRespawn message handlers
/////////////////////////////////////////////////////////////////////////////
// CPagePropMover property page

IMPLEMENT_DYNCREATE(CPagePropMover, CPropertyPage)

CPagePropMover::CPagePropMover() : CPropertyPage(CPagePropMover::IDD)
{
	//{{AFX_DATA_INIT(CPagePropMover)
	m_strCharacter = _T("");
	m_strName = _T("");
	m_bImmortality = FALSE;
	m_bSuperPower = FALSE;
	m_strDialog = _T("");
	m_strBelli = _T("");
	m_strAI = _T("");
	m_PathIndex = _T("");
	//}}AFX_DATA_INIT
	m_pMover = NULL;
	m_pWorldView = NULL;
	m_strState = _T("");
	m_strPropMoverID = _T("");
}
void CPagePropMover::Init( CObj* pObj )
{
	m_pMover = (CMover*) pObj;
}

CPagePropMover::~CPagePropMover()
{
}

void CPagePropMover::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropMover)
	DDX_Control(pDX, IDC_CYCLE, m_Cycle);
	DDX_CBString(pDX, IDC_CHARACTER, m_strCharacter);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Check(pDX, IDC_IMMORTALITY, m_bImmortality);
	DDX_Check(pDX, IDC_SUPER_POWER, m_bSuperPower);
	DDX_Text(pDX, IDC_DIALOG, m_strDialog);
	DDX_CBString(pDX, IDC_BELLI, m_strBelli);
	DDX_CBString(pDX, IDC_AI   , m_strAI);
	DDX_CBString(pDX, IDC_AI_STATE   , m_strState);
	DDX_Text(pDX, IDC_PATH_INDEX, m_PathIndex);
	DDX_Text(pDX, IDC_PROPMOVERID, m_strPropMoverID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropMover, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropMover)
	ON_BN_CLICKED(IDC_DIALOG_BROWSER, OnDialogBrowser)
	ON_CBN_SELCHANGE(IDC_CHARACTER, OnSelchangeCharacter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropMover message handlers

BOOL CPagePropMover::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_strCharacter		= m_pMover->m_szCharacterKey;
	m_strName			= m_pMover->m_szName;

	TCHAR*	pPropMoverID	= NULL;
	pPropMoverID	= prj.GetPropMoverID( m_pMover->m_dwIndex );

	if( pPropMoverID != NULL )
	{
		m_strPropMoverID.Format( "%s (%d)", pPropMoverID, m_pMover->m_dwIndex );
	}
	else
	{
		m_strPropMoverID	= pPropMoverID;
	}

	//m_strDialog = m_pMover->m_pNpcProperty->GetDlgName();
	m_bSuperPower = m_pMover->IsSuperPower();
	m_bImmortality = m_pMover->IsImmortality();

	CComboBox* pComboBox = (CComboBox*) GetDlgItem( IDC_CHARACTER );
	CComboBox* pBelli = (CComboBox*) GetDlgItem( IDC_BELLI );
	CComboBox* pAI = (CComboBox*) GetDlgItem( IDC_AI );
#ifdef __YAIMONSTER_EX
	CComboBox* pAIState = (CComboBox*) GetDlgItem( IDC_AI_STATE );
#endif //__YAIMONSTER_EX	

	pComboBox->AddString( _T( "" ) );
	POSITION pos = prj.m_mapCharacter.GetStartPosition();
	LPCHARACTER lpCharacter;
	CString string;
	while(pos)
	{
		prj.m_mapCharacter.GetNextAssoc( pos, string, (void*&)lpCharacter );
		pComboBox->AddString( lpCharacter->m_szKey );
	}
	CStringArray strArray;
	CScript::GetFindIdToArray( "BELLI_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
		pBelli->AddString( strArray.GetAt( i ) );
	m_strBelli = CScript::GetFindIdStr( "BELLI_", m_pMover->m_dwBelligerence );

	strArray.RemoveAll();
	CScript::GetFindIdToArray( "AII_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
		pAI->AddString( strArray.GetAt( i ) );
	
	CRespawnInfo* pRespawnInfo = &(m_pMover->m_respawnInfo);
	
	//added by gmpbigsun( 20101012 ) : 구역정보를 이용할때만 구역AI를 무버에 설정해줌
	if( pRespawnInfo && pRespawnInfo->m_dwIndex != 0 )
		m_pMover->m_dwAIInterface =  pRespawnInfo->m_dwAI;

	m_strAI = CScript::GetFindIdStr( "AII_", m_pMover->m_dwAIInterface );

#ifdef __YAIMONSTER_EX	
	int nSel = (m_pMover->m_dwAIInterfaceState-1);
	pAIState->SetCurSel( nSel );
	pAIState->GetLBText( nSel, m_strState );
#endif //__YAIMONSTER_EX

#ifdef __Y_PATROL
	if( pRespawnInfo )
	{
		m_PathIndex = g_pMainFrame->m_dlgPatrol.GetPathStringName( pRespawnInfo->m_dwPatrolIndex );
		m_Cycle.SetCheck( pRespawnInfo->m_bPatrolCycle );
	}
#endif //__Y_PATROL

	UpdateData( FALSE );

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPagePropMover::OnOK( )
{
	UpdateData( TRUE );
	BOOL bSetMover = FALSE;
	if( m_strCharacter != m_pMover->m_szCharacterKey, m_strCharacter )
		bSetMover = TRUE;
	_tcscpy( m_pMover->m_szCharacterKey, m_strCharacter );
	_tcscpy( m_pMover->m_szName, m_strName );
	//m_pMover->m_pNpcProperty->SetDlgName( m_strDialog );
	if( m_bSuperPower ) m_pMover->AddExtraFlag( EXFLAG_SUPERPOWER );
	else m_pMover->DelExtraFlag( EXFLAG_SUPERPOWER );
	if( m_bImmortality ) m_pMover->AddExtraFlag( EXFLAG_IMMORTALITY );
	else m_pMover->DelExtraFlag( EXFLAG_IMMORTALITY );
	m_pMover->m_dwBelligerence = CScript::GetDefineNum( m_strBelli );

#ifdef __YAIMONSTER_EX
/*
	if( m_pMover->IsNPC() && m_pMover->IsPeaceful() == FALSE )
	{
		if( CScript::GetDefineNum( m_strAI ) != AII_MONSTER )
		{
			AfxMessageBox( "Test" );
		}
	}
*/
	CComboBox* pAIState = (CComboBox*) GetDlgItem( IDC_AI_STATE );
	m_pMover->SetAIInterface( CScript::GetDefineNum( m_strAI ), (pAIState->GetCurSel()+1) );
	(m_pMover->m_respawnInfo).m_dwAiState = (pAIState->GetCurSel()+1);
	(m_pMover->m_respawnInfo).m_bPatrolCycle = m_Cycle.GetCheck();

#ifdef __BS_AI_RESPAWN_ATTR
	(m_pMover->m_respawnInfo).m_dwAI = CScript::GetDefineNum( m_strAI );
#endif 

#else //__YAIMONSTER_EX
	m_pMover->SetAIInterface( CScript::GetDefineNum( m_strAI ) );
#endif //__YAIMONSTER_EX

	LPCHARACTER lpCharacter = m_pMover->GetCharacter(); 
	if( lpCharacter )
	{
		MoverProp* pMoverProp = prj.GetMoverProp( lpCharacter->m_dwMoverIdx );

		if( bSetMover && pMoverProp && lpCharacter->m_dwMoverIdx )
		{
			if( m_pMover->m_pModel && m_pMover->m_pModel->IsAniable() )
				SAFE_DELETE( m_pMover->m_pModel );
			m_pMover->m_Inventory.Clear();
			m_pMover->InitCharacter( lpCharacter );
			m_pMover->SetTypeIndex( m_pWorldView->m_pd3dDevice, OT_MOVER, lpCharacter->m_dwMoverIdx );//
			m_pMover->ResetScale();
			m_pMover->UpdateLocalMatrix();
			m_pMover->m_dwMotion = m_pMover->m_dwMotionOld = -1;
			m_pMover->SetMotion( MTI_STAND );
			m_pMover->SetHairColor( m_pMover->m_dwHairColor );
			m_pMover->RedoEquip( FALSE );
		}
	}
}

void CPagePropMover::OnDialogBrowser() 
{
	UpdateData( TRUE );
	CFileDialog dlg( TRUE, NULL, _T( "Ch*.txt" ), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , 
		"Dialog file (*.txt)|*.txt||");
	if( dlg.DoModal() == IDOK ) 
		m_strDialog = dlg.GetFileName();
	UpdateData( FALSE );
}
void CPagePropMover::OnSelchangeCharacter() 
{
	UpdateData( TRUE );
	LPCHARACTER lpChar = prj.GetCharacter( m_strCharacter );
	if( lpChar )
	{
		m_strName = lpChar->m_strName;
		m_strDialog = lpChar->m_szDialog;
		UpdateData( FALSE );
	}
/*
		CHAR    m_szKey[ 64 ]      ;
	CString m_strName          ;
	CHAR    m_szChar[ 64 ]     ;
	CHAR    m_szDialog[ 64 ]   ;
	CHAR    m_szDlgQuest[ 64 ] ;

	_tcscpy( m_pMover->m_szCharacterKey, m_strCharacter );
	_tcscpy( m_pMover->m_szName, m_strName );
	_tcscpy( m_pMover->m_szDialogFile, m_strDialog );
	if( m_bSuperPower ) m_pMover->AddExtraFlag( EXFLAG_SUPERPOWER );
	else m_pMover->DelExtraFlag( EXFLAG_SUPERPOWER );
	if( m_bImmortality ) m_pMover->AddExtraFlag( EXFLAG_IMMORTALITY );
	else m_pMover->DelExtraFlag( EXFLAG_IMMORTALITY );
	*/
}

void CPagePropTrigger::OnPlaySound() 
{
	// TODO: Add your control notification handler code here
	
}

void CPagePropTrigger::OnPlayMusic() 
{
	// TODO: Add your control notification handler code here
	
}

void CPagePropAttribute::OnSelcancelList1() 
{
	// TODO: Add your control notification handler code here
	
}

void CPagePropAttribute::OnSetfocusList1() 
{
	// TODO: Add your control notification handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropCaption property page

IMPLEMENT_DYNCREATE(CPagePropCaption, CPropertyPage)

CPagePropCaption::CPagePropCaption() : CPropertyPage(CPagePropCaption::IDD)
{
	//{{AFX_DATA_INIT(CPagePropCaption)
	m_strDescription = _T("");
	m_strSubTitle = _T("");
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
	m_pRegion = NULL;
}

CPagePropCaption::~CPagePropCaption()
{
}

void CPagePropCaption::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropCaption)
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_strDescription);
	//DDX_Text(pDX, IDC_EDIT_SUBTITLE, m_strSubTitle);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	//}}AFX_DATA_MAP
}
void CPagePropCaption::Init( CRegion* pRegion )
{
	m_pRegion = pRegion;
}

BEGIN_MESSAGE_MAP(CPagePropCaption, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropCaption)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropCaption message handlers


BOOL CPagePropCaption::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_strDescription = m_pRegion->m_region.m_szDesc;
	m_strTitle = m_pRegion->m_region.m_szTitle;
	// TODO: Add extra initialization here
	//CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST1 );
	//for( int i = 0; i < m_pRegion->m_region.m_cDescSize; i++ )
	//{
		//pListBox->AddString( m_pRegion->m_region.m_apszDesc[ i ] );
	//}
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPagePropCaption::OnAdd() 
{
	// TODO: Add your control notification handler code here
	//CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST1 );
	//CDlgEditString dlgEditString;
	//if( dlgEditString.DoModal() == IDOK )
	//{
	//	pListBox->AddString( dlgEditString.m_string );
	//}
}

void CPagePropCaption::OnRemove() 
{
	// TODO: Add your control notification handler code here
	//CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST1 );
	//int nCurSel = pListBox->GetCurSel();
	//pListBox->DeleteString( nCurSel );
}

void CPagePropCaption::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	//CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST1 );
	//int nCount = pListBox->GetCount();
	m_strDescription.TrimLeft();
	m_strDescription.TrimRight();
	if( m_strDescription != m_pRegion->m_region.m_szDesc )
	{
		strcpy( m_pRegion->m_region.m_szDesc, m_strDescription );
		for( int i = 0; i < LANG_MAX; i++ )
			m_pRegion->m_region.m_apwszDesc[ i ][ 0 ] = 0;
	}
	m_strTitle.TrimLeft();
	m_strTitle.TrimRight();
	if( m_strTitle != m_pRegion->m_region.m_szTitle )
	{
		strcpy( m_pRegion->m_region.m_szTitle, m_strTitle );
		for( int i = 0; i < LANG_MAX; i++ )
			m_pRegion->m_region.m_apwszTitle[ i ][ 0 ] = 0;
	}

	/*
	CString string;
	for( int i = 0; i < nCount; i++ )
	{
		pListBox->GetText( i, string );
//		SAFE_DELETE( m_pRegion->m_region.m_apszDesc[ i ] );
//		m_pRegion->m_region.m_apszDesc[ i ] = new TCHAR[ string.GetLength() + 1 ];
//		strcpy( m_pRegion->m_region.m_apszDesc[ i ], string );
	}
	*/
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CPagePropPos property page

IMPLEMENT_DYNCREATE(CPagePropPos, CPropertyPage)

CPagePropPos::CPagePropPos() : CPropertyPage(CPagePropPos::IDD)
{
	//{{AFX_DATA_INIT(CPagePropPos)
	m_strKey = _T("");
	m_bTargetKey = FALSE;
	m_bChaoKey = FALSE;
	//}}AFX_DATA_INIT
}

CPagePropPos::~CPagePropPos()
{
}

void CPagePropPos::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropPos)
	DDX_Text(pDX, IDC_KEY, m_strKey);
	DDX_Check(pDX, IDC_TARGET_KEY, m_bTargetKey);
	DDX_Check(pDX, IDC_CHAO_CHECK, m_bChaoKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropPos, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropPos message handlers

BOOL CPagePropPos::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_strKey = m_pRegion->m_region.m_szKey;
	m_bTargetKey = m_pRegion->m_region.m_bTargetKey;
	m_bChaoKey = m_pRegion->m_region.m_bChaoKey;
	//m_strKey.MakeLower();

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPagePropPos::Init( CRegion* pRegion )
{
	m_pRegion = pRegion;
}

void CPagePropPos::OnOK()  
{
	UpdateData( TRUE );
	//m_strKey.MakeLower();
	strcpy( m_pRegion->m_region.m_szKey, m_strKey );
	m_pRegion->m_region.m_bTargetKey = ( m_bTargetKey != 0 );
	m_pRegion->m_region.m_bChaoKey = m_bChaoKey;
	CPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropStructure property page

IMPLEMENT_DYNCREATE(CPagePropStructure, CPropertyPage)

CPagePropStructure::CPagePropStructure() : CPropertyPage(CPagePropStructure::IDD)
{
	//{{AFX_DATA_INIT(CPagePropStructure)
	m_bUnit = FALSE;
	m_strStructure = _T("");
	//}}AFX_DATA_INIT
}

CPagePropStructure::~CPagePropStructure()
{
}
void CPagePropStructure::Init( CRegion* pRegion )
{
	m_pRegion = pRegion;
}


void CPagePropStructure::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropStructure)
	DDX_Check(pDX, IDC_UNIT, m_bUnit);
	DDX_CBString(pDX, IDC_STRUCTURE, m_strStructure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropStructure, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropStructure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropStructure message handlers

BOOL CPagePropStructure::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

	CComboBox* pStructure = (CComboBox*)GetDlgItem( IDC_STRUCTURE );
	CButton* pUnit = (CButton*)GetDlgItem( IDC_UNIT );

	CStringArray strArray;
	CScript::GetFindIdToArray( "SRT_", &strArray );
	for( int i = 0; i < strArray.GetSize(); i++ )
		pStructure->AddString( strArray.GetAt( i ) );
	m_strStructure = CScript::GetFindIdStr( "SRT_", m_pRegion->m_region.m_dwStructure );
	m_bUnit = m_pRegion->m_region.m_bUnit;
	
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPagePropStructure::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData( TRUE );

	//m_strKey.MakeLower();
	//strcpy( m_pRegion->m_region.m_szKey, m_strKey );
	m_pRegion->m_region.m_dwStructure = CScript::GetDefineNum( m_strStructure );
	m_pRegion->m_region.m_bUnit = ( m_bUnit != 0 );
	
	CPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropControl property page

IMPLEMENT_DYNCREATE(CPagePropControl, CPropertyPage)

CPagePropControl::CPagePropControl() : CPropertyPage(CPagePropControl::IDD)
{
	//{{AFX_DATA_INIT(CPagePropControl)
	m_strCondGender = _T("");
	m_strCondItem = _T("");
	m_strCondItemCount = _T("");
	m_strCondJob = _T("");
	m_strItemKind1 = _T("");
	m_strItemKind2 = _T("");
	m_strItemKind3 = _T("");
	m_strMonsterKind1 = _T("");
	m_strMonsterKind2 = _T("");
	m_strMonsterKind3 = _T("");
	m_strTrapKind1 = _T("");
	m_strTrapKind2 = _T("");
	m_strTrapKind3 = _T("");
	m_strContQuest1 = _T("");
	m_strContQuest2 = _T("");
	m_strContQuest3 = _T("");
	m_bGender = FALSE;
	m_bItem = FALSE;
	m_bLevel = FALSE;
	m_bQuest = FALSE;
	m_nItemMax = 0;
	m_nItemRare1 = 0;
	m_nItemRare2 = 0;
	m_nItemRare3 = 0;
	m_nMonActAttack1 = 0;
	m_nMonActAttack2 = 0;
	m_nMonActAttack3 = 0;
	m_nMonNum1 = 0;
	m_nMonNum2 = 0;
	m_nMonNum3 = 0;
	m_nCondLevel = 0;
	m_nCondQuestState1 = 0;
	m_nCondQuestState2 = 0;
	m_nCondQuestState3 = 0;
	m_nItemMin = 0;
	m_nTrapType = 0;
	m_nTrapRandomPercent = 0;
	m_nEndurance = 0;
	m_nTrapLevel1 = 0;
	m_nTrapLevel2 = 0;
	m_nTrapLevel3 = 0;
	m_nTrapDelay = 0;
	m_nCondTeleWorldId = 0;
	m_nCondTeleX = 0;
	m_nCondTeleY = 0;
	m_nCondTeleZ = 0;
	m_strCtrlKey = _T("");
	m_strLinkKey = _T("");
	//}}AFX_DATA_INIT
	m_pCommonCtrl = NULL;
}

CPagePropControl::~CPagePropControl()
{
}

void CPagePropControl::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropControl)
	DDX_CBString(pDX, IDC_COMBO_COND_GENDER, m_strCondGender);
	DDX_CBString(pDX, IDC_COMBO_COND_ITEM, m_strCondItem);
	DDX_CBString(pDX, IDC_COMBO_COND_ITEMCOUNT, m_strCondItemCount);
	DDX_CBString(pDX, IDC_COMBO_COND_JOB, m_strCondJob);
	DDX_CBString(pDX, IDC_COMBO_COND_QUEST1, m_strContQuest1);
	DDX_CBString(pDX, IDC_COMBO_COND_QUEST2, m_strContQuest2);
	DDX_CBString(pDX, IDC_COMBO_COND_QUEST3, m_strContQuest3);
	DDX_CBString(pDX, IDC_COMBO_ITEM_KIND1, m_strItemKind1);
	DDX_CBString(pDX, IDC_COMBO_ITEM_KIND2, m_strItemKind2);
	DDX_CBString(pDX, IDC_COMBO_ITEM_KIND3, m_strItemKind3);
	DDX_CBString(pDX, IDC_COMBO_MONSTER_KIND1, m_strMonsterKind1);
	DDX_CBString(pDX, IDC_COMBO_MONSTER_KIND2, m_strMonsterKind2);
	DDX_CBString(pDX, IDC_COMBO_MONSTER_KIND3, m_strMonsterKind3);
	DDX_CBString(pDX, IDC_COMBO_TRAP_KIND1, m_strTrapKind1);
	DDX_CBString(pDX, IDC_COMBO_TRAP_KIND2, m_strTrapKind2);
	DDX_CBString(pDX, IDC_COMBO_TRAP_KIND3, m_strTrapKind3);
	
	DDX_Check(pDX, IDC_COND_GENDER, m_bGender);
	DDX_Check(pDX, IDC_COND_ITEM, m_bItem);
	DDX_Check(pDX, IDC_COND_LEVEL, m_bLevel);
	DDX_Check(pDX, IDC_COND_QUEST, m_bQuest);
	DDX_Check(pDX, IDC_COND_QUEST_END, m_bQuestEnd);
	DDX_Check(pDX, IDC_COND_TELEPORT, m_bTeleport);
	DDX_Text(pDX, IDC_EDIT_ITEM_MAX, m_nItemMax);
	DDV_MinMaxInt(pDX, m_nItemMax, 1, 100);
	DDX_Text(pDX, IDC_EDIT_ITEM_RATE1, m_nItemRare1);
	DDX_Text(pDX, IDC_EDIT_ITEM_RATE2, m_nItemRare2);
	DDX_Text(pDX, IDC_EDIT_ITEM_RATE3, m_nItemRare3);
	DDX_Text(pDX, IDC_EDIT_MONSTER_ACT_ATTACK1, m_nMonActAttack1);
	DDV_MinMaxInt(pDX, m_nMonActAttack1, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MONSTER_ACT_ATTACK2, m_nMonActAttack2);
	DDV_MinMaxInt(pDX, m_nMonActAttack2, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MONSTER_ACT_ATTACK3, m_nMonActAttack3);
	DDV_MinMaxInt(pDX, m_nMonActAttack3, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MONSTER_NUM1, m_nMonNum1);
	DDV_MinMaxInt(pDX, m_nMonNum1, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MONSTER_NUM2, m_nMonNum2);
	DDV_MinMaxInt(pDX, m_nMonNum2, 0, 100);
	DDX_Text(pDX, IDC_EDIT_MONSTER_NUM3, m_nMonNum3);
	DDV_MinMaxInt(pDX, m_nMonNum3, 0, 100);
	DDX_Text(pDX, IDC_EDIT_COND_LEVEL, m_nCondLevel);
	DDV_MinMaxInt(pDX, m_nCondLevel, 0, 200);
	DDX_Text(pDX, IDC_EDIT_COND_QUEST_STATE1, m_nCondQuestState1);
	DDV_MinMaxInt(pDX, m_nCondQuestState1, 0, 14);
	DDX_Text(pDX, IDC_EDIT_COND_QUEST_STATE2, m_nCondQuestState2);
	DDV_MinMaxInt(pDX, m_nCondQuestState2, 0, 14);
	DDX_Text(pDX, IDC_EDIT_COND_QUEST_STATE3, m_nCondQuestState3);
	DDV_MinMaxInt(pDX, m_nCondQuestState3, 0, 14);
	DDX_Text(pDX, IDC_EDIT_ITEM_MIN, m_nItemMin);
	DDV_MinMaxInt(pDX, m_nItemMin, 0, 99);
	DDX_Radio(pDX, IDC_TRAP_TYPE, m_nTrapType);
	DDX_Text(pDX, IDC_EDIT_TRAP_RANDOM_PERCENT, m_nTrapRandomPercent);
	DDX_Text(pDX, IDC_EDIT_ENDURANCE, m_nEndurance);
	DDX_Text(pDX, IDC_EDIT_TRAP_LEVEL1, m_nTrapLevel1);
	DDV_MinMaxInt(pDX, m_nTrapLevel1, 0, 20);
	DDX_Text(pDX, IDC_EDIT_TRAP_LEVEL2, m_nTrapLevel2);
	DDV_MinMaxInt(pDX, m_nTrapLevel2, 0, 20);
	DDX_Text(pDX, IDC_EDIT_TRAP_LEVEL3, m_nTrapLevel3);
	DDV_MinMaxInt(pDX, m_nTrapLevel3, 0, 20);
	DDX_Text(pDX, IDC_EDIT_TRAP_DELAY, m_nTrapDelay);
	DDV_MinMaxInt(pDX, m_nTrapDelay, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_CTRL_KEY, m_strCtrlKey);
	DDX_Text(pDX, IDC_EDIT_LINK_KEY, m_strLinkKey);
	DDX_Text(pDX, IDC_EDIT_COND_TELE_WORLDID, m_nCondTeleWorldId);
	DDV_MinMaxInt(pDX, m_nCondTeleWorldId, 0, 100000);
	DDX_Text(pDX, IDC_EDIT_COND_TELE_X, m_nCondTeleX);
	DDV_MinMaxInt(pDX, m_nCondTeleX, 0, 100000);
	DDX_Text(pDX, IDC_EDIT_COND_TELE_Y, m_nCondTeleY);
	DDV_MinMaxInt(pDX, m_nCondTeleY, 0, 100000);
	DDX_Text(pDX, IDC_EDIT_COND_TELE_Z, m_nCondTeleZ);
	DDV_MinMaxInt(pDX, m_nCondTeleZ, 0, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropControl, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropControl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_COND_ITEM, OnCheckItem)
	ON_BN_CLICKED(IDC_COND_QUEST, OnCheckQuest)
	ON_BN_CLICKED(IDC_COND_QUEST_END, OnCheckQuestEnd)
	ON_BN_CLICKED(IDC_COND_LEVEL, OnCheckLevel)
	ON_BN_CLICKED(IDC_COND_GENDER, OnCheckGender)
	ON_BN_CLICKED(IDC_COND_TELEPORT, OnCheckTeleport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropControl message handlers
 
void CPagePropControl::Init( CCommonCtrl* pCommonCtrl )
{

	m_pCommonCtrl = pCommonCtrl;
}

BOOL CPagePropControl::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CCtrlElem* pCtrlElem = &m_pCommonCtrl->m_respawnInfo.m_CtrlElem;

	m_bItem   = ( pCtrlElem->m_dwSet & UA_ITEM   ) ? TRUE : FALSE;
	m_bLevel  = ( pCtrlElem->m_dwSet & UA_LEVEL  ) ? TRUE : FALSE;
	m_bQuest  = ( pCtrlElem->m_dwSet & UA_QUEST  ) ? TRUE : FALSE;
	m_bGender = ( pCtrlElem->m_dwSet & UA_GENDER ) ? TRUE : FALSE;
#ifdef __S_DONGEON_0504
	m_bQuestEnd = ( pCtrlElem->m_dwSet & UA_QUEST_END ) ? TRUE : FALSE;
	m_bTeleport = ( pCtrlElem->m_dwSet & UA_TELEPORT ) ? TRUE : FALSE;
#endif // __S_DONGEON_0504

	m_nCondLevel = pCtrlElem->m_dwSetLevel;
	m_nCondQuestState1 = pCtrlElem->m_dwSetFlagNum; 
#ifdef __S_DONGEON_0504
	m_nCondQuestState2 = pCtrlElem->m_dwSetFlagNum1;
	m_nCondQuestState3 = pCtrlElem->m_dwSetFlagNum2;

	m_nCondTeleWorldId = pCtrlElem->m_dwTeleWorldId;
	m_nCondTeleX = pCtrlElem->m_dwTeleX;
	m_nCondTeleY = pCtrlElem->m_dwTeleY;
	m_nCondTeleZ = pCtrlElem->m_dwTeleZ;
#endif // __S_DONGEON_0504

	int nNum = 0;
	CString string;
	CStringArray strArray;

#ifdef __S_DONGEON_0504
	// TelePort
	CEdit* pEditTeleWorldId = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_WORLDID );
	CEdit* pEditTeleX = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_X );
	CEdit* pEditTeleY = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_Y );
	CEdit* pEditTeleZ = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_Z );
	pEditTeleWorldId->EnableWindow( m_bTeleport );
	pEditTeleX->EnableWindow( m_bTeleport );
	pEditTeleY->EnableWindow( m_bTeleport );
	pEditTeleZ->EnableWindow( m_bTeleport );
#endif // __S_DONGEON_0504
	
	// 레벨
	CEdit* pEditLevel = (CEdit*)GetDlgItem( IDC_EDIT_COND_LEVEL );
	pEditLevel->EnableWindow( m_bLevel );

	// 성별
	CComboBox* pCondGenderCombo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_GENDER );
	CScript::GetFindIdToArray( "SEX_", &strArray );
	m_strCondGender = CScript::GetFindIdStr( "SEX_", pCtrlElem->m_dwSetGender );
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		pCondGenderCombo->SetItemData( pCondGenderCombo->AddString( string ), CScript::GetDefineNum( string )  );
	}
	pCondGenderCombo->EnableWindow( m_bGender );

	// 퀘스트
	strArray.RemoveAll();
	CScript::GetFindIdToArray( "QUEST_", &strArray );
	CComboBox* pCondQuest1Combo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_QUEST1 );
	CComboBox* pCondQuest2Combo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_QUEST2 );
	CComboBox* pCondQuest3Combo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_QUEST3 );
	CEdit* pEditQuestState1 = (CEdit*)GetDlgItem( IDC_EDIT_COND_QUEST_STATE1 );
	CEdit* pEditQuestState2 = (CEdit*)GetDlgItem( IDC_EDIT_COND_QUEST_STATE2 );
	CEdit* pEditQuestState3 = (CEdit*)GetDlgItem( IDC_EDIT_COND_QUEST_STATE3 );
	
	pCondQuest1Combo->SetItemData( pCondQuest1Combo->AddString( "NONE" ), 0 );
	pCondQuest2Combo->SetItemData( pCondQuest2Combo->AddString( "NONE" ), 0 );
	pCondQuest3Combo->SetItemData( pCondQuest3Combo->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwSetQuestNum )
		m_strContQuest1 = CScript::GetFindIdStr( "QUEST_", pCtrlElem->m_dwSetQuestNum );
	else
		m_strContQuest1 = "NONE";

#ifdef __S_DONGEON_0504
	if( pCtrlElem->m_dwSetQuestNum1 )
		m_strContQuest2 = CScript::GetFindIdStr( "QUEST_", pCtrlElem->m_dwSetQuestNum1 );
	else
		m_strContQuest2 = "NONE";

	if( pCtrlElem->m_dwSetQuestNum2 )
		m_strContQuest3 = CScript::GetFindIdStr( "QUEST_", pCtrlElem->m_dwSetQuestNum2 );
	else
		m_strContQuest3 = "NONE";
#endif // __S_DONGEON_0504
	
	
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
		{
			pCondQuest1Combo->SetItemData( pCondQuest1Combo->AddString( string ), (DWORD)pProp  );
			pCondQuest2Combo->SetItemData( pCondQuest2Combo->AddString( string ), (DWORD)pProp  );
			pCondQuest3Combo->SetItemData( pCondQuest3Combo->AddString( string ), (DWORD)pProp  );
		}
	}
	pCondQuest1Combo->EnableWindow( m_bQuest );
	pEditQuestState1->EnableWindow( m_bQuest );

	pCondQuest2Combo->EnableWindow( m_bQuestEnd );
	pCondQuest3Combo->EnableWindow( m_bQuestEnd );
	pEditQuestState2->EnableWindow( m_bQuestEnd );
	pEditQuestState3->EnableWindow( m_bQuestEnd );

	// 아이템
	strArray.RemoveAll();
	CScript::GetFindIdToArray( "II_", &strArray );

	CComboBox* pCondItemCombo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_ITEM );

	pCondItemCombo->SetItemData( pCondItemCombo->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwSetItem )
		m_strCondItem = CScript::GetFindIdStr( "II_", pCtrlElem->m_dwSetItem );
	else
		m_strCondItem = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pCondItemCombo->SetItemData( pCondItemCombo->AddString( string ), (DWORD)pProp  );
	}
	pCondItemCombo->EnableWindow( m_bItem );

	// 아이템 개수
#ifdef __S_DONGEON_0504
	CComboBox* pCondItemCountCombo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_ITEMCOUNT );
	CString strItemCount;
	if( 0<= pCtrlElem->m_dwSetItemCount && pCtrlElem->m_dwSetItemCount < 0x99999999 )
	{
		strItemCount.Format( "%d", pCtrlElem->m_dwSetItemCount );
		m_strCondItemCount = strItemCount;
	}
	for( int i = 0 ; i < 10 ; ++i )
	{
		strItemCount.Format( "%d", i );
		pCondItemCountCombo->SetItemData( pCondItemCountCombo->AddString( strItemCount ), i );
	}
	pCondItemCountCombo->EnableWindow( m_bItem );
#endif // __S_DONGEON_0504

	// Generate Item
	m_nItemMin = pCtrlElem->m_dwMinItemNum;
	m_nItemMax = pCtrlElem->m_dwMaxiItemNum;

	m_nItemRare1 = pCtrlElem->m_dwInsideItemPer[ 0 ];
	m_nItemRare2 = pCtrlElem->m_dwInsideItemPer[ 1 ];
	m_nItemRare3 = pCtrlElem->m_dwInsideItemPer[ 2 ];

	CComboBox* pItemKind1 = (CComboBox*)GetDlgItem( IDC_COMBO_ITEM_KIND1 );
	CComboBox* pItemKind2 = (CComboBox*)GetDlgItem( IDC_COMBO_ITEM_KIND2 );
	CComboBox* pItemKind3 = (CComboBox*)GetDlgItem( IDC_COMBO_ITEM_KIND3 );

	pItemKind1->SetItemData( pItemKind1->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwInsideItemKind[ 0 ] )
		m_strItemKind1 = CScript::GetFindIdStr( "II_", pCtrlElem->m_dwInsideItemKind[ 0 ] );
	else
		m_strItemKind1 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pItemKind1->SetItemData( pItemKind1->AddString( string ), (DWORD)pProp  );
	}
	pItemKind2->SetItemData( pItemKind2->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwInsideItemKind[ 1 ] )
		m_strItemKind2 = CScript::GetFindIdStr( "II_", pCtrlElem->m_dwInsideItemKind[ 1 ] );
	else
		m_strItemKind2 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pItemKind2->SetItemData( pItemKind2->AddString( string ), (DWORD)pProp  );
	}
	pItemKind3->SetItemData( pItemKind3->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwInsideItemKind[ 2 ] )
		m_strItemKind3 = CScript::GetFindIdStr( "II_", pCtrlElem->m_dwInsideItemKind[ 2 ] );
	else
		m_strItemKind3 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pItemKind3->SetItemData( pItemKind3->AddString( string ), (DWORD)pProp  );
	}
	/*
	for( int i = 0; i < prj.m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pProp = prj.GetItemProp( i );
		if( pProp )
			pItemKind1->SetItemData( pItemKind1->AddString( pProp->szName ), (DWORD)pProp  );
	}
*/
	// Generate Monster 
	m_nMonActAttack1 = pCtrlElem->m_dwMonActAttack[ 0 ];
	m_nMonActAttack2 = pCtrlElem->m_dwMonActAttack[ 1 ];
	m_nMonActAttack3 = pCtrlElem->m_dwMonActAttack[ 2 ];
	m_nMonNum1       = pCtrlElem->m_dwMonResNum   [ 0 ];
	m_nMonNum2       = pCtrlElem->m_dwMonResNum   [ 1 ];
	m_nMonNum3       = pCtrlElem->m_dwMonResNum   [ 2 ];

	CComboBox* pMonsterKind1 = (CComboBox*)GetDlgItem( IDC_COMBO_MONSTER_KIND1 );
	CComboBox* pMonsterKind2 = (CComboBox*)GetDlgItem( IDC_COMBO_MONSTER_KIND2 );
	CComboBox* pMonsterKind3 = (CComboBox*)GetDlgItem( IDC_COMBO_MONSTER_KIND3 );

	strArray.RemoveAll();
	CScript::GetFindIdToArray( "MI_", &strArray );

	pMonsterKind1->SetItemData( pMonsterKind1->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwMonResKind[ 0 ] )
		m_strMonsterKind1 = CScript::GetFindIdStr( "MI_", pCtrlElem->m_dwMonResKind[ 0 ] );
	else
		m_strMonsterKind1 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		MoverProp* pProp = prj.GetMoverProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pMonsterKind1->SetItemData( pMonsterKind1->AddString( string ), (DWORD)pProp  );
	}

	pMonsterKind2->SetItemData( pMonsterKind2->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwMonResKind[ 1 ] )
		m_strMonsterKind2 = CScript::GetFindIdStr( "MI_", pCtrlElem->m_dwMonResKind[ 1 ] );
	else
		m_strMonsterKind2 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		MoverProp* pProp = prj.GetMoverProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pMonsterKind2->SetItemData( pMonsterKind2->AddString( string ), (DWORD)pProp  );
	}

	pMonsterKind3->SetItemData( pMonsterKind3->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwMonResKind[ 2 ] )
		m_strMonsterKind3 = CScript::GetFindIdStr( "MI_", pCtrlElem->m_dwMonResKind[ 2 ] );
	else
		m_strMonsterKind3 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		MoverProp* pProp = prj.GetMoverProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pMonsterKind3->SetItemData( pMonsterKind3->AddString( string ), (DWORD)pProp  );
	}
    // Trap 

	m_nTrapRandomPercent = pCtrlElem->m_dwTrapRandomPer;
	m_nTrapType          = pCtrlElem->m_dwTrapOperType;
	m_nTrapDelay         = pCtrlElem->m_dwTrapDelay;

	m_nTrapLevel1 = pCtrlElem->m_dwTrapLevel[ 0 ];
	m_nTrapLevel2 = pCtrlElem->m_dwTrapLevel[ 1 ];
	m_nTrapLevel3 = pCtrlElem->m_dwTrapLevel[ 2 ];

	CComboBox* pTrapKind1 = (CComboBox*)GetDlgItem( IDC_COMBO_TRAP_KIND1 );
	CComboBox* pTrapKind2 = (CComboBox*)GetDlgItem( IDC_COMBO_TRAP_KIND2 );
	CComboBox* pTrapKind3 = (CComboBox*)GetDlgItem( IDC_COMBO_TRAP_KIND3 );

	strArray.RemoveAll();
	CScript::GetFindIdToArray( "SI_", &strArray );

	pTrapKind1->SetItemData( pTrapKind1->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwTrapKind[ 0 ] )
		m_strTrapKind1 = CScript::GetFindIdStr( "SI_", pCtrlElem->m_dwTrapKind[ 0 ] );
	else 
		m_strTrapKind1 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pTrapKind1->SetItemData( pTrapKind1->AddString( string ), (DWORD)pProp  );
	}
	pTrapKind2->SetItemData( pTrapKind2->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwTrapKind[ 1 ] )
		m_strTrapKind2 = CScript::GetFindIdStr( "SI_", pCtrlElem->m_dwTrapKind[ 1 ] );
	else 
		m_strTrapKind2 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pTrapKind2->SetItemData( pTrapKind2->AddString( string ), (DWORD)pProp  );
	}
	pTrapKind3->SetItemData( pTrapKind3->AddString( "NONE" ), 0 );
	if( pCtrlElem->m_dwTrapKind[ 2 ] )
		m_strTrapKind3 = CScript::GetFindIdStr( "SI_", pCtrlElem->m_dwTrapKind[ 2 ] );
	else 
		m_strTrapKind3 = "NONE";
	for( int i = 0; i < strArray.GetSize(); i++ )
	{
		string = strArray.GetAt( i );
		ItemProp* pProp = prj.GetItemProp( CScript::GetDefineNum( string ) );
		if( pProp )
			pTrapKind3->SetItemData( pTrapKind3->AddString( string ), (DWORD)pProp  );
	}
	// 기타 
	m_nEndurance = pCtrlElem->m_dwSetEndu;

#ifdef __YDUNGEON_TRIGGER
	m_strCtrlKey = pCtrlElem->m_strCtrlKey;
	m_strLinkKey = pCtrlElem->m_strLinkCtrlKey;
#endif//__YDUNGEON_TRIGGER			헤더파일에 디파인이 있어서 이곳에도 디파인을 넣어준다.
	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPagePropControl::OnCheckItem() 
{
	UpdateData( TRUE );
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBO_COND_ITEM );
	pComboBox->EnableWindow( m_bItem );
	pComboBox = (CComboBox*)GetDlgItem( IDC_COMBO_COND_ITEMCOUNT );
	pComboBox->EnableWindow( m_bItem );
}

void CPagePropControl::OnCheckQuest() 
{
	UpdateData( TRUE );
	CComboBox* pCondQuest1Combo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_QUEST1 );
	CEdit* pEditQuestState1 = (CEdit*)GetDlgItem( IDC_EDIT_COND_QUEST_STATE1 );
	pCondQuest1Combo->EnableWindow( m_bQuest );
	pEditQuestState1->EnableWindow( m_bQuest );
}

void CPagePropControl::OnCheckLevel() 
{
	UpdateData( TRUE );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_COND_LEVEL );
	pEdit->EnableWindow( m_bLevel );
}

void CPagePropControl::OnCheckGender() 
{
	UpdateData( TRUE );
	CComboBox* pCondGenderCombo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_GENDER );
	pCondGenderCombo->EnableWindow( m_bGender );	
}

void CPagePropControl::OnCheckTeleport()
{
	UpdateData( TRUE );
#ifdef __S_DONGEON_0504
	// TelePort
	CEdit* pEditTeleWorldId = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_WORLDID );
	CEdit* pEditTeleX = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_X );
	CEdit* pEditTeleY = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_Y );
	CEdit* pEditTeleZ = (CEdit*)GetDlgItem( IDC_EDIT_COND_TELE_Z );
	pEditTeleWorldId->EnableWindow( m_bTeleport );
	pEditTeleX->EnableWindow( m_bTeleport );
	pEditTeleY->EnableWindow( m_bTeleport );
	pEditTeleZ->EnableWindow( m_bTeleport );
#endif // __S_DONGEON_0504
}

void CPagePropControl::OnCheckQuestEnd()
{
	UpdateData( TRUE );
#ifdef __S_DONGEON_0504
	CComboBox* pCondQuest2Combo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_QUEST2 );
	CComboBox* pCondQuest3Combo = (CComboBox*)GetDlgItem( IDC_COMBO_COND_QUEST3 );
	CEdit* pEditQuestState2 = (CEdit*)GetDlgItem( IDC_EDIT_COND_QUEST_STATE2 );
	CEdit* pEditQuestState3 = (CEdit*)GetDlgItem( IDC_EDIT_COND_QUEST_STATE3 );
	pCondQuest2Combo->EnableWindow( m_bQuestEnd );
	pCondQuest3Combo->EnableWindow( m_bQuestEnd );
	pEditQuestState2->EnableWindow( m_bQuestEnd );
	pEditQuestState3->EnableWindow( m_bQuestEnd );
#endif // __S_DONGEON_0504
}

DWORD GetDefineNum( LPCTSTR lpszStr )
{
	int nValue = CScript::GetDefineNum( lpszStr );
	if( nValue == -1 ) return 0;
	return nValue;
}
void CPagePropControl::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CCtrlElem* pCtrlElem = &m_pCommonCtrl->m_respawnInfo.m_CtrlElem;

	pCtrlElem->m_dwSet = 0;
	if( m_bItem   ) pCtrlElem->m_dwSet |= UA_ITEM  ;
	if( m_bLevel  ) pCtrlElem->m_dwSet |= UA_LEVEL ;
	if( m_bQuest  ) pCtrlElem->m_dwSet |= UA_QUEST ;
	if( m_bGender ) pCtrlElem->m_dwSet |= UA_GENDER;
	if( m_bTeleport ) pCtrlElem->m_dwSet |= UA_TELEPORT;
	if( m_bQuestEnd ) pCtrlElem->m_dwSet |= UA_QUEST_END;

	pCtrlElem->m_dwSetItem     = GetDefineNum( m_strCondItem );
#ifdef __S_DONGEON_0504
	pCtrlElem->m_dwSetItemCount = atoi( m_strCondItemCount );
#endif // __S_DONGEON_0504
	pCtrlElem->m_dwSetGender   = GetDefineNum( m_strCondGender );
	pCtrlElem->m_dwSetLevel    = m_nCondLevel      ;

	pCtrlElem->m_dwSetQuestNum	= GetDefineNum( m_strContQuest1 );
	pCtrlElem->m_dwSetFlagNum	= m_nCondQuestState1;
#ifdef __S_DONGEON_0504
	pCtrlElem->m_dwSetQuestNum1	= GetDefineNum( m_strContQuest2 );
	pCtrlElem->m_dwSetFlagNum1	= m_nCondQuestState2;
	pCtrlElem->m_dwSetQuestNum2	= GetDefineNum( m_strContQuest3 );
	pCtrlElem->m_dwSetFlagNum2	= m_nCondQuestState3;
#endif // __S_DONGEON_0504

#ifdef __S_DONGEON_0504
	pCtrlElem->m_dwTeleWorldId = m_nCondTeleWorldId;
	pCtrlElem->m_dwTeleX = m_nCondTeleX;
	pCtrlElem->m_dwTeleY = m_nCondTeleY;
	pCtrlElem->m_dwTeleZ = m_nCondTeleZ;
#endif // __S_DONGEON_0504

	// Generate Item
	pCtrlElem->m_dwMinItemNum  = m_nItemMin;
	pCtrlElem->m_dwMaxiItemNum = m_nItemMax;

	pCtrlElem->m_dwInsideItemPer[ 0 ] = m_nItemRare1;
	pCtrlElem->m_dwInsideItemPer[ 1 ] = m_nItemRare2;
	pCtrlElem->m_dwInsideItemPer[ 2 ] = m_nItemRare3;

	pCtrlElem->m_dwInsideItemKind[ 0 ] = GetDefineNum( m_strItemKind1 );
	pCtrlElem->m_dwInsideItemKind[ 1 ] = GetDefineNum( m_strItemKind2 );
	pCtrlElem->m_dwInsideItemKind[ 2 ] = GetDefineNum( m_strItemKind3 );

	// Generate Monster 
	pCtrlElem->m_dwMonActAttack[ 0 ] = m_nMonActAttack1;
	pCtrlElem->m_dwMonActAttack[ 1 ] = m_nMonActAttack2;
	pCtrlElem->m_dwMonActAttack[ 2 ] = m_nMonActAttack3;
	pCtrlElem->m_dwMonResNum   [ 0 ] = m_nMonNum1      ;
	pCtrlElem->m_dwMonResNum   [ 1 ] = m_nMonNum2      ;
	pCtrlElem->m_dwMonResNum   [ 2 ] = m_nMonNum3      ;

	pCtrlElem->m_dwMonResKind[ 0 ] = GetDefineNum( m_strMonsterKind1 );
	pCtrlElem->m_dwMonResKind[ 1 ] = GetDefineNum( m_strMonsterKind2 );
	pCtrlElem->m_dwMonResKind[ 2 ] = GetDefineNum( m_strMonsterKind3 );

    // Trap 
	pCtrlElem->m_dwTrapRandomPer = m_nTrapRandomPercent;
	pCtrlElem->m_dwTrapOperType  = m_nTrapType         ;
	pCtrlElem->m_dwTrapDelay     = m_nTrapDelay        ;

	pCtrlElem->m_dwTrapKind[ 0 ] = GetDefineNum( m_strTrapKind1 );
	pCtrlElem->m_dwTrapKind[ 1 ] = GetDefineNum( m_strTrapKind2 );
	pCtrlElem->m_dwTrapKind[ 2 ] = GetDefineNum( m_strTrapKind3 );

	pCtrlElem->m_dwTrapLevel[ 0 ] = m_nTrapLevel1;
	pCtrlElem->m_dwTrapLevel[ 1 ] = m_nTrapLevel2;
	pCtrlElem->m_dwTrapLevel[ 2 ] = m_nTrapLevel3;

	// 기타

	pCtrlElem->m_dwSetEndu = m_nEndurance;

#ifdef __YDUNGEON_TRIGGER
	strcpy( pCtrlElem->m_strCtrlKey, m_strCtrlKey );
	strcpy( pCtrlElem->m_strLinkCtrlKey, m_strLinkKey );
#endif//__YDUNGEON_TRIGGER		
	
	CPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CPagePropAreaSet property page

#ifdef __N_TRIGGER_AREASET_050203

IMPLEMENT_DYNCREATE(CPagePropAreaSet, CPropertyPage)

CPagePropAreaSet::CPagePropAreaSet() : CPropertyPage(CPagePropAreaSet::IDD)
{
	//{{AFX_DATA_INIT(CPagePropAreaSet)
	m_bCheckLevel = FALSE;
	m_bCheckParty = FALSE;
	m_bCheckQuest = FALSE;
	m_bCheckJob = FALSE;
	m_bCheckGuild = FALSE;
	m_bCheckGender = FALSE;
	m_uiMinLevel = 0;
	m_uiMaxLevel = 0;
	m_iQuestFlag = 0;
	//}}AFX_DATA_INIT
}

CPagePropAreaSet::~CPagePropAreaSet()
{
}

void CPagePropAreaSet::Init(CRegion* pRegion)
{
	m_pRegion = pRegion;
}

void CPagePropAreaSet::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPagePropAreaSet)
	DDX_Check(pDX, IDC_CHECKITEM, m_bCheckItem);
	DDX_Check(pDX, IDC_CHECKLEVEL, m_bCheckLevel);
	DDX_Check(pDX, IDC_CHECKPARTY, m_bCheckParty);
	DDX_Check(pDX, IDC_CHECKQUEST, m_bCheckQuest);
	DDX_Check(pDX, IDC_CHECKJOB, m_bCheckJob);
	DDX_Check(pDX, IDC_CHECKGUILD, m_bCheckGuild);
	DDX_Check(pDX, IDC_CHECKGENDER, m_bCheckGender);
	DDX_Text(pDX, IDC_EDITMIN, m_uiMinLevel);
	DDV_MinMaxUInt(pDX, m_uiMinLevel, 0, 300);
	DDX_Text(pDX, IDC_EDITMAX, m_uiMaxLevel);
	DDV_MinMaxUInt(pDX, m_uiMaxLevel, 0, 300);
	DDX_Text(pDX, IDC_QUESTFLAG, m_iQuestFlag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePropAreaSet, CPropertyPage)
	//{{AFX_MSG_MAP(CPagePropAreaSet)
	ON_BN_CLICKED(IDC_CHECKLEVEL, OnChecklevel)
	ON_BN_CLICKED(IDC_CHECKJOB, OnCheckjob)
	ON_BN_CLICKED(IDC_CHECKQUEST, OnCheckquest)
	ON_BN_CLICKED(IDC_CHECKGENDER, OnCheckgender)
	ON_EN_CHANGE(IDC_EDITMIN, OnChangeEditmin)
	ON_EN_CHANGE(IDC_EDITMAX, OnChangeEditmax)
	ON_BN_CLICKED(IDC_CHECKITEM, OnCheckitem)
	ON_EN_CHANGE(IDC_ITEM, OnChangeItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePropAreaSet message handlers

void CPagePropAreaSet::OnChecklevel() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CEdit* pEdit;
	pEdit = ( (CEdit*)GetDlgItem(IDC_EDITMIN) );
	pEdit->EnableWindow( m_bCheckLevel );
	pEdit = (CEdit*)GetDlgItem(IDC_EDITMAX) ;
	pEdit->EnableWindow( m_bCheckLevel );
}

void CPagePropAreaSet::OnCheckjob() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOJOB );
	pComboBox->EnableWindow( m_bCheckJob );
}

void CPagePropAreaSet::OnCheckquest() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	
	CComboBox* pComboBox = ( (CComboBox*)GetDlgItem( IDC_COMBOQUEST ) );
	pComboBox->EnableWindow( m_bCheckQuest );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_QUESTFLAG ) ;
	pEdit->EnableWindow( m_bCheckQuest );
}

void CPagePropAreaSet::OnCheckgender() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOGENDER );
	pComboBox->EnableWindow( m_bCheckGender );
}

void CPagePropAreaSet::OnCheckitem() 
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOITEM );
	pComboBox->EnableWindow( m_bCheckItem );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_ITEM ) ;
	pEdit->EnableWindow( m_bCheckItem );
}


BOOL CPagePropAreaSet::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here

	int nSelect;
	CComboBox* pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOJOB );

	for ( int i = 0 ; i < MAX_JOB ; i++ )
	{
		pComboBox->AddString( prj.m_aJob[i].szName );
	}
	if( m_pRegion->m_region.m_iJob != 0xffffffff )
	{
		pComboBox->SetCurSel( m_pRegion->m_region.m_iJob );
		m_bCheckJob = TRUE;
	}
	else
	{
		pComboBox->SetCurSel( 0 );
		m_bCheckJob = FALSE;
	}
	UpdateData( !m_bCheckJob );
	pComboBox->EnableWindow( m_bCheckJob );

//	m_pRegion->m_region.m_uItemId		= 0xffffffff;
//	m_pRegion->m_region.m_uiItemCount	= 0xffffffff;
//	m_pRegion->m_region.m_uiMinLevel	= 0xffffffff;
//	m_pRegion->m_region.m_uiMaxLevel	= 0xffffffff;
//	m_pRegion->m_region.m_iQuest		= 0xffffffff;
//	m_pRegion->m_region.m_iQuestFlag	= 0xffffffff;
//	m_pRegion->m_region.m_bCheckParty	= FALSE;
//	m_pRegion->m_region.m_bCheckGuild	= FALSE;
	
	pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOITEM );
	if( m_pRegion->m_region.m_uItemId != 0xffffffff )
		m_bCheckItem = TRUE;
	else
		m_bCheckItem = FALSE;
	nSelect = 0;
	int j = 0;
	for( int i = 0; i < prj.m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp =  prj.GetItemProp( i );
		if( pItemProp )
		{
			pComboBox->AddString( pItemProp->szName );
			if( m_bCheckItem == TRUE && pItemProp->dwID == m_pRegion->m_region.m_uItemId )
			{
				nSelect = j;
			}
			++j;
		}
	}
	UpdateData( !m_bCheckItem );
	pComboBox->SetCurSel( nSelect );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_ITEM ) ;
	if( m_bCheckItem )
		SetDlgItemInt( IDC_ITEM, m_pRegion->m_region.m_uiItemCount );
	pEdit->EnableWindow( m_bCheckItem );
	pComboBox->EnableWindow( m_bCheckItem );

	nSelect = 0;
	if( m_pRegion->m_region.m_iGender != 0xffffffff )
		m_bCheckGender = TRUE;
	else
		m_bCheckGender = FALSE;
	pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOGENDER );
	pComboBox->AddString( "MAN" );
	pComboBox->AddString( "WOMAN" );
	if( m_bCheckGender )
	{
		if( m_pRegion->m_region.m_iGender != SEX_MALE )
		{
			nSelect = 1;
		}
	}
	UpdateData( !m_bCheckGender );
	pComboBox->EnableWindow( m_bCheckGender );
	pComboBox->SetCurSel( nSelect );

	nSelect = 0;
	j = 0;
	if( m_pRegion->m_region.m_iQuest != 0xffffffff )
		m_bCheckQuest = TRUE;
	else
		m_bCheckQuest = FALSE;	
	pComboBox = (CComboBox*)GetDlgItem( IDC_COMBOQUEST );
	for ( int i = 0 ; i < prj.m_aPropQuest.GetSize() ; i++ )
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
			pComboBox->AddString ( pQuestProp->m_szTitle );
			if( m_pRegion->m_region.m_iQuest == i )
			{
				nSelect = j;
			}
			++j;
		}
	}
	UpdateData( !m_bCheckQuest );
	pEdit = (CEdit*)GetDlgItem( IDC_QUESTFLAG ) ;
	if( m_bCheckQuest )
		SetDlgItemInt( IDC_QUESTFLAG, m_pRegion->m_region.m_iQuestFlag );
	pEdit->EnableWindow( m_bCheckQuest );
	pComboBox->EnableWindow( m_bCheckQuest );
	pComboBox->SetCurSel( nSelect );

	if( m_pRegion->m_region.m_uiMinLevel != 0xffffffff )
		m_bCheckLevel = TRUE;
	UpdateData( !m_bCheckLevel );
	pEdit = (CEdit*)GetDlgItem( IDC_EDITMIN );
	if( m_bCheckLevel )
		SetDlgItemInt( IDC_EDITMIN, m_pRegion->m_region.m_uiMinLevel );
	else
		SetDlgItemInt( IDC_EDITMIN, 0 );
	pEdit->EnableWindow( m_bCheckLevel );
	pEdit = (CEdit*)GetDlgItem( IDC_EDITMAX );
	if( m_bCheckLevel )
		SetDlgItemInt( IDC_EDITMAX, m_pRegion->m_region.m_uiMaxLevel );
	else
		SetDlgItemInt( IDC_EDITMAX, MAX_CHARACTER_LEVEL );
	pEdit->EnableWindow( m_bCheckLevel );

	m_bCheckParty = m_pRegion->m_region.m_bCheckParty;
	UpdateData( !m_bCheckParty );
	
	m_bCheckGuild = m_pRegion->m_region.m_bCheckGuild;
	UpdateData( !m_bCheckGuild );
	return TRUE;
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPagePropAreaSet::OnChangeEditmin() 
{
	// TODO: Add your control notification handler code here
	if( m_bCheckLevel )
	{
		m_uiMinLevel = GetDlgItemInt( IDC_EDITMIN );
		m_uiMaxLevel = GetDlgItemInt( IDC_EDITMAX );
		if( m_uiMinLevel <= 0 )
		{
			AfxMessageBox( "Error MIN Level Minus Value Default Value 1" );
			SetDlgItemInt( IDC_EDITMIN, 1 );
		}	
		else if( MAX_CHARACTER_LEVEL < m_uiMinLevel )
		{
			AfxMessageBox( "Error MIN Level Overflow MIN Value 1" );
			SetDlgItemInt( IDC_EDITMIN, 1 );
		}
	}
}

void CPagePropAreaSet::OnChangeEditmax() 
{
	if( m_bCheckLevel )
	{
		m_uiMinLevel = GetDlgItemInt( IDC_EDITMIN );
		m_uiMaxLevel = GetDlgItemInt( IDC_EDITMAX );
		CString strMessage;
		if( m_uiMaxLevel <= 0 )
		{
			strMessage.Format( "Error MAX Level Minus Value Default Value %d", MAX_CHARACTER_LEVEL ); 
			AfxMessageBox( strMessage );
			SetDlgItemInt( IDC_EDITMAX, MAX_CHARACTER_LEVEL );
		}	
		else if( MAX_CHARACTER_LEVEL < m_uiMaxLevel )
		{
			strMessage.Format( "Error MAXMIN Level Overflow MAX Value %d", MAX_CHARACTER_LEVEL ); 
			AfxMessageBox( strMessage );
			SetDlgItemInt( IDC_EDITMAX, MAX_CHARACTER_LEVEL );
		}
	}
}

void CPagePropAreaSet::OnChangeItem() 
{
	// TODO: Add your control notification handler code here
	int nSelectInt = GetDlgItemInt( IDC_ITEM );
	if( nSelectInt <= 0 )
	{
		AfxMessageBox( "Error ItemCount Minus Value Default 1" );
		SetDlgItemInt( IDC_ITEM, 1 );
	}	
	else if( 99 < nSelectInt )
	{
		AfxMessageBox( "Error ItemCount Overflow Max Value 99" );
		SetDlgItemInt( IDC_ITEM, 99 );
	}
}

void CPagePropAreaSet::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData( TRUE );
	//	여기에 값을 저장하는것을 넣는다.
	m_pRegion->m_region.m_uItemId		= 0xffffffff;
	m_pRegion->m_region.m_uiItemCount	= 0xffffffff;
	m_pRegion->m_region.m_uiMinLevel	= 0xffffffff;
	m_pRegion->m_region.m_uiMaxLevel	= 0xffffffff;
	m_pRegion->m_region.m_iQuest		= 0xffffffff;
	m_pRegion->m_region.m_iQuestFlag	= 0xffffffff;
	m_pRegion->m_region.m_iJob			= 0xffffffff;
	m_pRegion->m_region.m_iGender		= 0xffffffff;
	m_pRegion->m_region.m_bCheckParty	= FALSE;
	m_pRegion->m_region.m_bCheckGuild	= FALSE;

	CString strSelectText;
	if( m_bCheckItem )
	{
		GetDlgItemText( IDC_COMBOITEM, strSelectText );
		ItemProp *pItemProp = prj.GetItemProp( strSelectText );
		if( pItemProp )
		{
			m_pRegion->m_region.m_uItemId = pItemProp->dwID;
			m_pRegion->m_region.m_uiItemCount = GetDlgItemInt( IDC_ITEM );
		}
	}
	if( m_bCheckLevel )
	{
		m_pRegion->m_region.m_uiMinLevel = GetDlgItemInt( IDC_EDITMIN );
		m_pRegion->m_region.m_uiMaxLevel = GetDlgItemInt( IDC_EDITMAX );
	}
	if( m_bCheckQuest )
	{
		GetDlgItemText( IDC_COMBOQUEST, strSelectText );
		for ( int i = 0 ; i < prj.m_aPropQuest.GetSize() ; i++ )
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt( i );
			if( pQuestProp && 0 == strcmp( pQuestProp->m_szTitle, strSelectText ) )
			{
				m_pRegion->m_region.m_iQuest = i;
				m_pRegion->m_region.m_iQuestFlag = GetDlgItemInt( IDC_QUESTFLAG );
			}
		}
	}
	if( m_bCheckJob )
	{
		GetDlgItemText( IDC_COMBOJOB, strSelectText );
		for ( int i = 0 ; i < MAX_JOB ; i++ )
		{
			if( 0 == strcmp( prj.m_aJob[i].szName, strSelectText ) )
			{
				m_pRegion->m_region.m_iJob = i;
			}
		}
	}
	if( m_bCheckGender )
	{
		GetDlgItemText( IDC_COMBOQUEST, strSelectText );
		if( 0 == strcmp( "MAN", strSelectText ) )
		{
			m_pRegion->m_region.m_iGender = SEX_MALE;
		}
		else
		{
			m_pRegion->m_region.m_iGender = SEX_FEMALE;
		}
	}
	if( m_bCheckParty )
	{
		m_pRegion->m_region.m_bCheckParty = TRUE;
	}
	if( m_bCheckGuild )
	{
		m_pRegion->m_region.m_bCheckGuild = TRUE;
	}	
	CPropertyPage::OnOK();
}



#endif//__N_TRIGGER_AREASET_050203



//void CPagePropLight::OnInitMenu(CMenu* pMenu)
//{
//	CPropertyPage::OnInitMenu(pMenu);
//
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//}

//BOOL CPagePropLight::OnInitDialog()
//{
//	CPropertyPage::OnInitDialog();
//
//	// TODO:  여기에 추가 초기화 작업을 추가합니다.
//
//	return TRUE;  // return TRUE unless you set the focus to a control
//	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
//}

