// DialogView.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "WorldDoc.h"
#include "WorldView.h"
#include "ModelView.h"
//#include "PreviewBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMAGE_ROOT    0
#define IMAGE_GROUP   1
#define IMAGE_FOLDER  2
#define IMAGE_MODEL   3
#define IMAGE_MOTION  4 
#define IMAGE_TEXTURE 5

#define FOLDER 0x80000000

#define IS_FOLDER( x ) ( ( x & FOLDER ) ? TRUE : FALSE )

/////////////////////////////////////////////////////////////////////////////
// CModelView

IMPLEMENT_DYNCREATE(CModelView, CTreeView)

CModelView::CModelView()
{
	m_pDragImage = NULL;
	m_bLDragging = FALSE;
	m_hRootObject = NULL;
	m_hRootTerrain = NULL;
	m_bModifiedMdlObj = FALSE;
	m_bModifiedMdlDyna = FALSE;
	m_bModifiedTerrain = FALSE;
}

CModelView::~CModelView()
{
}


BEGIN_MESSAGE_MAP(CModelView, CTreeView)
	//{{AFX_MSG_MAP(CModelView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	ON_COMMAND_EX(ID_POPUP_MODEL_ADD, OnPopupCommand)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClickTree)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND_EX(ID_POPUP_MODEL_REMOVE, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_MODEL_PROPERTIES, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_MODEL_NEWFOLDER, OnPopupCommand)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelView drawing

void CModelView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CModelView diagnostics

#ifdef _DEBUG
void CModelView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CModelView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelView message handlers

int CModelView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CTreeCtrl* pTree = &GetTreeCtrl();
	HTREEITEM hRootItem = pTree->InsertItem( _T( "Object" ), IMAGE_ROOT, IMAGE_ROOT );
	m_hRootObject = hRootItem;
	LoadModelScript( hRootItem, _T( "MdlObj.inc" ) );
	LoadModelScript( hRootItem, _T( "MdlDyna.inc" ) );

	pTree->ModifyStyle(0, TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES|TVS_SHOWSELALWAYS);

	LoadTerrainScript( _T( "Terrain.inc" ) );
	/*
	// TODO: Add extra initialization here
	CFixedArray< tagMODELELEM >* apModelElem; 	
	CModelMng* pModelMng = &prj.m_modelMng;

	TV_INSERTSTRUCT tv; //TV_ITEM
	tv.hParent = NULL;
	tv.hInsertAfter = TVI_LAST;
	tv.item.mask = TVIF_TEXT;
	tv.item.hItem = NULL;
	tv.item.state = 0;
	tv.item.stateMask = 0;
	tv.item.cchTextMax = 0;
	tv.item.iSelectedImage = 0;
	tv.item.cChildren = 0;
	tv.item.lParam = 0;
	tv.item.iImage = 0;

	CTreeCtrl* pTree = &GetTreeCtrl();
	pTree->ModifyStyle(0, TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES|TVS_SHOWSELALWAYS);

	static TCHAR szRoot[ MAX_OBJARRAY ][ 32 ] = { _T( "obj" ), _T( "ctrl" ), _T( "sfx" ), _T( "item" ), _T( "mvr" ), _T( "Region" ), _T( "" ) }; 

	HTREEITEM hRootItem = pTree->InsertItem( _T( "Object" ), 0, 0 );
	for(int i = 0; i < MAX_OBJARRAY; i++)
	{
		tv.hParent = hRootItem;
		tv.item.pszText = szRoot[i]; 

		//tv.item.lParam = i | FOLDER;
		HTREEITEM hr = pTree->InsertItem( &tv );
		pTree->SetItemData( hr,i | FOLDER );

		apModelElem = &pModelMng->m_aaModelElem[ i ];
		tv.hParent = hr;
		for(int i2 = 0; i2 < apModelElem->GetSize(); i2++)
		{
			LPMODELELEM model = apModelElem->GetAt( i2 ) ;
			if( apModelElem->GetAt( i2 ) && apModelElem->GetAt( i2 )->m_szName[ 0 ] )
			{
				tv.item.pszText = apModelElem->GetAt( i2 )->m_szName; 
				//tv.item.lParam = i2;
				hr = pTree->InsertItem( &tv );
				pTree->SetItemData( hr, i2 );
			}
		}
	}
*/
	m_images.Create(IDB_TABIMAGES1, 16, 1, RGB(255,0,255));
	pTree->SetImageList(&m_images,TVSIL_NORMAL );

	//////////////////////////////////////////////////////////////////////////

/*
	
	CEdit* pEditHeight     = (CEdit*) GetDlgItem( IDC_EDIT_HEIGHT  );
	CEdit* pEditScaleX     = (CEdit*) GetDlgItem( IDC_EDIT_SCALE_X );
	CEdit* pEditScaleY     = (CEdit*) GetDlgItem( IDC_EDIT_SCALE_Y );
	CEdit* pEditScaleZ     = (CEdit*) GetDlgItem( IDC_EDIT_SCALE_Z );
	CEdit* pEditRotationX  = (CEdit*) GetDlgItem( IDC_EDIT_ROTATION_X );
	CEdit* pEditRotationY  = (CEdit*) GetDlgItem( IDC_EDIT_ROTATION_Y );
	CEdit* pEditRotationZ  = (CEdit*) GetDlgItem( IDC_EDIT_ROTATION_Z );
*/
/*

	//CTreeCtrl
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	pTreeCtrl->ModifyStyle(0, 
		TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES|TVS_SHOWSELALWAYS);

	HTREEITEM hRootItem = pTreeCtrl->InsertItem("Resources",0,0);
	pTreeCtrl->InsertItem( _T( "Bitmap"       ), 0, 0, hRootItem );
	m_hApplet = pTreeCtrl->InsertItem( _T( "Applet" ), 0, 1, hRootItem );
	pTreeCtrl->InsertItem( _T( "Icon"         ), 0, 0, hRootItem );
	pTreeCtrl->InsertItem( _T( "Menu"         ), 0, 0, hRootItem );
	pTreeCtrl->InsertItem( _T( "String Table" ), 0, 0, hRootItem );
	pTreeCtrl->InsertItem( _T( "Toolbar"      ), 0, 0, hRootItem );

	m_images.Create(IDB_TABIMAGES, 16, 1, RGB(255,0,255));
	pTreeCtrl->SetImageList(&m_images,TVSIL_NORMAL );
	*/
	/*
	for (int i=0; i<5; i++)
	{
		CString strItem;
		strItem.Format("File %d", i);
		GetTreeCtrl().InsertItem(strItem,i,i,hRootItem);
	}
	*/
	
	//GetTreeCtrl().SelectItem(hRootItem);
	//GetTreeCtrl().Expand(hRootItem, TVE_EXPAND);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_pObjView = this;
	
	return 0;
}
/*
void CModelView::AddApplet( LPCTSTR lpszWIDC, LPWNDAPPLET LPWNDAPPLET )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hItem = pTreeCtrl->InsertItem( lpszWIDC, 1, 1, m_hApplet );
	pTreeCtrl->SetItemData( hItem, (DWORD)LPWNDAPPLET );
}
void CModelView::ChangeApplet( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hChild = pTreeCtrl->GetChildItem( m_hApplet );
	CString string;
	while( hChild )
	{
		string = pTreeCtrl->GetItemText( hChild );
		if( string == lpszOldWIDC )
			pTreeCtrl->SetItemText( hChild, lpszNewWIDC );
		hChild = pTreeCtrl->GetNextSiblingItem( hChild );
	}
}
*/
void CModelView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//OnPopupCommand( ID_POPUP_RESOURCE_OPEN );
	/*
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	LPWNDAPPLET lpWndApplet = (LPWNDAPPLET)pTreeCtrl->GetItemData( hItem );
	if( lpWndApplet && lpWndApplet->pWndBase == NULL )
	{
		CWndDialog* pWndDialog = new CWndDialog;
		lpWndApplet->pWndBase = pWndDialog;
		pWndDialog->InitDialog(	g_pMainFrame->m_pDaisyView->CView::GetSafeHwnd(), lpWndApplet->dwWndId );
		g_pMainFrame->m_pDaisyView->Invalidate( FALSE );
		//pWndDialog->Initialize( NULL, LPWNDAPPLET->dwWndId );
		
		
		TCHAR* szDefData = new TCHAR[ 32 ];
		CString strDefine;
		LPSTR lpDefData; 
		do
		{
			strDefine.Format( _T( "APP_APPLET%d" ), g_pMainFrame->m_nIdApplet ); 
			lpDefData = CScript::GetDefineStr( strDefine );
			if( lpDefData ) g_pMainFrame->m_nIdApplet++;
		} while( lpDefData );
		_itot( g_pMainFrame->m_nIdApplet, szDefData, 10 );
		CScript::AddDefine( strDefine, szDefData );
		g_pMainFrame->m_nIdApplet++;
		CModelView* pResourceView = (CModelView*)g_pMainFrame->m_wndSTCBar.GetView( 0 );
		pResourceView->AddApplet( strDefine, NULL );
		
		Invalidate( FALSE );

	}
	*/
	//LPWNDAPPLET->
	/*
	LPLABEL_PACK pLabelPack; 
	m_mapLabelPack.Lookup(hd,(void*&)pLabelPack);
	if(pLabelPack->pSubLabel)
	{
		CString strFileName = pLabelPack->pRootLabel->string + "_" + pLabelPack->pSubLabel->string + ".spr";
		theApp.OpenDocumentFile(prj.MakeFullPath(prj.m_strDirSprite,strFileName));	
	}
	*/
	//pNMHDR->hwndFrom
	*pResult = 0;
}
HTREEITEM CModelView::GetRootItem( HTREEITEM hTree )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hTreeTemp = hTree;
	do
	{
		hTree = hTreeTemp;
		hTreeTemp = pTreeCtrl->GetParentItem( hTree );
	}  while( hTreeTemp );
	return hTree;
}
void CModelView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	POINT point;
	::GetCursorPos( &point );
	ScreenToClient( &point ); 
	UINT nFlags;
	HTREEITEM hTree = pTreeCtrl->HitTest( point, &nFlags );
	pTreeCtrl->SelectItem( hTree );
	HTREEITEM hRoot = GetRootItem( hTree );//pTreeCtrl->GetRootItem();
	CString strRootName = pTreeCtrl->GetItemText( hRoot );
	if( hRoot == hTree && strRootName == _T( "Object" ) )
		return;
	int nImage = GetItemImage( hTree );
	if( nImage == IMAGE_MOTION )
		return;
	CMenu menu; 
	menu.CreatePopupMenu();
	if( strRootName == _T( "Terrain" ) )
	{
		CString strItemName = pTreeCtrl->GetItemText( hTree );
		if( ( pTreeCtrl->GetItemData( hTree ) & FOLDER ) || strItemName == _T( "Terrain" ) )
		{
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_ADD,"&Add Texture");
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_NEWFOLDER,"&New Folder");
			if( strItemName != _T( "Terrain" ) )
				menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_REMOVE,"&Remove");
		}
		else
		{
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_REMOVE,"&Remove");
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_PROPERTIES,"&Properties");
		}
	}
	else
	if( strRootName == _T( "Object" ) )
	{
		if( pTreeCtrl->GetItemData( hTree ) & FOLDER )
		{
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_ADD,"&Add Model");
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_NEWFOLDER,"&New Folder");
			if( pTreeCtrl->GetParentItem( hTree ) != pTreeCtrl->GetRootItem() )
				menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_REMOVE,"&Remove");
		}
		else
		{
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_REMOVE,"&Remove");
			menu.AppendMenu(MF_STRING, ID_POPUP_MODEL_PROPERTIES,"&Properties");
		}
	}
	::GetCursorPos( &point );
	if( menu.TrackPopupMenu( TPM_LEFTALIGN            , point.x, point.y, this ) )
	{
		;//m_pPopupBkg = pBkg;
	}


	*pResult = 1;
}
/*
void CModelView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
*/
void CModelView::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
/*
BOOL CModelView::OnPopupCommand(UINT nID)
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	LPWNDAPPLET lpWndApplet = (LPWNDAPPLET)pTreeCtrl->GetItemData( hItem );
	if( lpWndApplet )
	{
		switch( nID )
		{
			case ID_POPUP_RESOURCE_OPEN:
				if( lpWndApplet->pWndBase == NULL )
				{
					CWndDialog* pWndDialog = new CWndDialog;
					lpWndApplet->pWndBase = pWndDialog;
					pWndDialog->m_lpWndApplet = lpWndApplet;
					pWndDialog->InitDialog(	g_pMainFrame->m_pDaisyView->CView::GetSafeHwnd(), lpWndApplet->dwWndId );
				}
				else
				{
					lpWndApplet->pWndBase->SetFocus();
				}
				break;
			case ID_POPUP_RESOURCE_REMOVE:
				pTreeCtrl->DeleteItem( hItem );
				if( lpWndApplet->pWndBase )
					lpWndApplet->pWndBase->Destroy( TRUE );
				CWndBase::m_resMng.RemoveApplet( lpWndApplet->dwWndId );
				break;
			case ID_POPUP_RESOURCE_CLOSE:
				if( lpWndApplet->pWndBase )
				{
					lpWndApplet->pWndBase->Destroy( TRUE );
					lpWndApplet->pWndBase = NULL;
				}
				break;
		}
	}
	else
	{
		switch( nID )
		{
			case ID_POPUP_RESOURCE_NEW:
			{
				CWndDialog* pWndDialog = new CWndDialog;
				pWndDialog->Initialize( NULL, CWndBase::m_resMng.GetNewAppletId() );
				WNDAPPLET wndApplet;
				wndApplet.strTitle = _T( "Applet" );
				wndApplet.pWndBase   = pWndDialog;
				wndApplet.dwWndStyle = pWndDialog->GetWndStyle();
				wndApplet.size.cx    = pWndDialog->GetWindowRect().Width();
				wndApplet.size.cy    = pWndDialog->GetWindowRect().Height();
				wndApplet.strDefine  = CWndBase::m_resMng.GetNewAppletDefine();
				wndApplet.dwWndId    = pWndDialog->GetWndId();
				pWndDialog->SetTitle( wndApplet.strTitle );
	
				LPWNDAPPLET lpWndApplet;
				if( CWndBase::m_resMng.AddApplet( &wndApplet ) )
				{
					lpWndApplet = CWndBase::m_resMng.GetAt( wndApplet.dwWndId );
					AddApplet( wndApplet.strDefine, lpWndApplet );
					pWndDialog->m_lpWndApplet = lpWndApplet;
				}
			}
			break;
		}
	}
	g_pMainFrame->m_pDaisyView->Invalidate( FALSE );
	return TRUE;
}
*/
void CModelView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	// 차일드다.
	CTreeCtrl* pTree = &GetTreeCtrl();//(CTreeCtrl*)GetDlgItem(IDC_TREE);
	if( pNMTreeView->itemNew.hItem == 0 )
		return;
	DWORD dwIndex = pTree->GetItemData(pNMTreeView->itemNew.hItem);
	CString str = pTree->GetItemText(pNMTreeView->itemNew.hItem);
	int nImage = GetItemImage( pNMTreeView->itemNew.hItem );

	if( nImage == IMAGE_MODEL )
	{
		//CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE);
		//CTreeCtrl* pTree = &GetTreeCtrl();//(CTreeCtrl*)GetDlgItem(IDC_TREE);
		//HTREEITEM hParent = pTree->GetParentItem(pNMTreeView->itemNew.hItem);
		//if(hParent)
		{
			DWORD dwType = GetModelType( pNMTreeView->itemNew.hItem );//pTree->GetItemData(hParent);
			m_dwObjType = dwType & ~FOLDER;
			m_dwObjIndex = dwIndex;
			LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( m_dwObjType, m_dwObjIndex );
			CWorldView::SetTool( ID_SELECT_PUT );
			SAFE_DELETE( CWorldView::m_pObjBrush );
			CWorldView::m_dwBrushType = BRUSHTYPE_OBJECT;
			CWorldView::m_vBrushPos     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			CWorldView::m_vBrushScale   = D3DXVECTOR3( lpModelElem->m_fScale, lpModelElem->m_fScale, lpModelElem->m_fScale );
			CWorldView::m_vBrushRotaion = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
			//g_pMainFrame->m_wndPreviewBar.m_viwer.SetModel( m_dwObjType, m_dwObjIndex );
			if( pBeastView->IsKindOf( RUNTIME_CLASS( CWorldView ) ) )
			{
				CWorldView::m_pObjBrush = CreateObj( pBeastView->m_pd3dDevice, m_dwObjType, m_dwObjIndex );
				pBeastView->Invalidate( FALSE );
			}
		}
	}
	if( nImage == IMAGE_TEXTURE )
	{
		// TODO: Add your control notification handler code here
			//LPTERRAIN lpTerrain = (LPTERRAIN) phdn->pitem->lParam;
		LPTERRAIN lpTerrain = prj.m_terrainMng.GetTerrain( dwIndex );	
		CWorldView::m_dwBrushType = BRUSHTYPE_TERRAIN;
		CWorldView::m_dwTerrainBrush = lpTerrain->m_dwId;

		CWorldView::m_dwBrushType = BRUSHTYPE_TERRAIN;
		CWorldView::m_dwTerrainBrush = lpTerrain->m_dwId;

		//g_pMainFrame->m_wndPreviewBar.m_viwer.SetTexture( lpTerrain->m_szTextureFileName );
		//m_texture.Free();
		//m_texture.LoadTexture( m_pd3dDevice, _T( MakePath( DIR_WORLDTEX, lpTerrain->m_szTextureFileName ) ), 0);
		//Render3DEnvironment();
		//SetLandscapeTool( 1 );
		CWorldView::SetTool( ID_TERRAIN_TEXTURE );

#ifdef __CSC_TERRAIN_OPACITY
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
		if(pMainFrame)
		{
			pMainFrame->m_nTerrainTexOpacity = 255;
			if(pMainFrame->m_pageTerrainTexture)
				pMainFrame->m_pageTerrainTexture.ResetOpacityValue();
		}		
#endif //__CSC_TERRAIN_OPACITY
	}
	/*
	else
	{
		if( (dwIndex & 0x4fffffff) == OT_REGION )
		{
			m_dwObjType = dwIndex & ~FOLDER;
			m_dwObjIndex = 0;
			CWorldView::SetTool( ID_SELECT_PUT );
			SAFE_DELETE( CWorldView::m_pObjBrush );
			CWorldView::m_dwBrushType = BRUSHTYPE_OBJECT;
			CWorldView::m_vBrushPos     = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			CWorldView::m_vBrushScale   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			CWorldView::m_vBrushRotaion = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
			//aSetObjEditTool( 0 );
			if( pBeastView )
			{
				CWorldView::m_pObjBrush = CreateObj( pBeastView->m_pd3dDevice, m_dwObjType, m_dwObjIndex );
				pBeastView->Invalidate( FALSE );
			}
		}
	}
	*/
	*pResult = 0;
}

void CModelView::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	if( pBeastView == NULL )
		return;
	CWorldView::SetTool( ID_SELECT_PUT );
	//aSetObjEditTool( 0 );
//	pBeastView->m_nView = VIEW_DRAW;
	//g_pMainFrame->m_pObjBrush = CreateObj(pBeastView,m_dwObjType,m_dwObjIndex);
	*pResult = 0;
}
void CModelView::SetFocus()
{
	//CWorldView::SetTool( EDITTYPE_OBJECT );
	//aSetObjEditTool( m_nObjEditTool );
	CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
	if( pBeastView )
		pBeastView->Invalidate( FALSE );
}
void CModelView::ReadModelScript( CScript& script, HTREEITEM hTreeItem, DWORD dwType )
{
	CTreeCtrl* pTree = &GetTreeCtrl();
	TCHAR szObject[64];
//	TCHAR szMotion[64];
	while( *script.token != '}' )
	{
		_tcscpy( szObject, script.token ); // folder 또는 object name


		script.SetMark();
		script.GetToken(); // {
		if( *script.token == '{' )
		{
			HTREEITEM hr = pTree->InsertItem( szObject, IMAGE_FOLDER, IMAGE_FOLDER, hTreeItem );
			pTree->SetItemData( hr, dwType | FOLDER );
		
			script.GetToken(); // object name or }
			//_tcscpy( szObject, script.token ); // folder 또는 object name
			ReadModelScript( script, hr, dwType );
			script.GetToken();
			continue;
		}
		else
			script.GoMark();

		TCHAR	szObjectName[MAX_PATH] = {0,};

		if( script.m_strDefine.GetLength() != 0 )
		{
			_stprintf( szObjectName, "%s - %s (%s)", szObject, script.m_strDefine, script.token );
		}
		else
		{
			_stprintf( szObjectName, "%s - (%s)", szObject, script.token );
		}

		HTREEITEM hModel = pTree->InsertItem( szObjectName, IMAGE_MODEL, IMAGE_MODEL, hTreeItem );
		UINT iObject = script.GetNumber();
		pTree->SetItemData( hModel, iObject );
		script.GetNumber(); // type
		script.GetToken(); // part
		script.GetNumber(); // fly 
		script.GetNumber(); // distant
		script.GetNumber(); // pick
		script.GetFloat(); // scale
		script.GetNumber(); // trans
		script.GetNumber(); // shadow
		script.GetNumber(); // 텍스춰 교체 
#ifndef THAI_0525VER
		script.GetNumber();
#endif//THAI_0525VER
#ifdef __YRENDER_FLAG
		script.GetNumber(); // 옵션SFX보일거냐 말거냐		
#endif //__YRENDER_FLAG
		// object name or { or }
		// 여기부터 애니메이션 
		if( *script.token == '{' )
		{
			script.GetToken(); // motion name or }
			while( *script.token != '}' )
			{
				HTREEITEM hr = pTree->InsertItem( script.token, IMAGE_MOTION, IMAGE_MOTION, hModel );
				UINT iMotion = script.GetNumber();
				pTree->SetItemData( hr, iMotion );
				//_tcscpy( szMotion, script.token );
				
				//_tcscpy( modelElem.m_motionAry[ iMotion ], szMotion );
				script.GetToken(); // motion name or }
			}
			script.GetToken(); // object name or }
		}
	}
}
BOOL CModelView::LoadModelScript( HTREEITEM hRootItem, LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;
	CTreeCtrl* pTree = &GetTreeCtrl();
//	TCHAR szObject[64];
//	TCHAR szMotion[64];
	UINT iType;//, iObject, iMotion;
	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		HTREEITEM hr = pTree->InsertItem( script.token, IMAGE_GROUP, IMAGE_GROUP, hRootItem );
		iType = script.GetNumber();
		pTree->SetItemData( hr, iType | FOLDER );
		script.GetToken(); // {
		script.GetToken(); // object name or }
		ReadModelScript( script, hr, iType );
		script.GetToken(); // type name or }
	}
	pTree->SelectItem( hRootItem );
	pTree->Expand( hRootItem, TVE_EXPAND );
	return TRUE;
}
void CModelView::ReadTerrainScript( CScript& script, HTREEITEM hTreeItem, DWORD dwId )
{
	CTreeCtrl* pTree = &GetTreeCtrl();
	TCHAR szObject[64];
	while( *script.token != '}' && script.tok != FINISHED )
	{
		_tcscpy( szObject, script.token ); // folder 또는 object name
		script.SetMark();
#ifdef __N_WATERFRAME_050202
		script.GetNumber(); 
#endif//__N_WATERFRAME_050202*/
		script.GetToken(); // { or filename
		if( *script.token == '{' )
		{
			HTREEITEM hr = pTree->InsertItem( szObject, IMAGE_FOLDER, IMAGE_FOLDER, hTreeItem );
			pTree->SetItemData( hr, FOLDER );
			dwId = script.GetNumber();
			ReadTerrainScript( script, hr, dwId );
			dwId = script.GetNumber();
			continue;
		}
		else
			script.GoMark();

#ifdef __N_WATERFRAME_050202
		script.GetNumber(); 
#endif//__N_WATERFRAME_050202*/
		script.GetToken(); // fileName
		HTREEITEM hModel = pTree->InsertItem( script.token, IMAGE_TEXTURE, IMAGE_TEXTURE, hTreeItem );
		pTree->SetItemData( hModel, dwId );
		script.GetNumber(); // block
		script.GetToken(); // sound
		dwId = script.GetNumber();
	}
}

BOOL CModelView::LoadTerrainScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;
	CTreeCtrl* pTree = &GetTreeCtrl();
	HTREEITEM hRootItem = pTree->InsertItem( _T( "Terrain" ), IMAGE_ROOT, IMAGE_ROOT );
	pTree->SetItemData( hRootItem, FOLDER );
	m_hRootTerrain = hRootItem;
	DWORD dwId = script.GetNumber(); // id or folder
	ReadTerrainScript( script, hRootItem, dwId );
	pTree->SelectItem( hRootItem );
	pTree->Expand( hRootItem, TVE_EXPAND );
	return TRUE;
}
void CModelView::WriteTerrainScript( CFileIO& file, HTREEITEM hItem, int nInsSpace )
{
	CTreeCtrl* pTree = &GetTreeCtrl();
//	TCHAR szObject[64];
	DWORD dwId;
	CString strSpace;
	CString string;
	CString strName;
	for( int i = 0; i < nInsSpace; i++ )
		strSpace += _T( " " );
#ifdef __N_WATERFRAME_050202
	static int TreeCount;
	int nCount = 0;
	TreeCount++;
#endif//__N_WATERFRAME_050202
	while (	( hItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		HTREEITEM hChildItem = pTree->GetChildItem( hItem );
		dwId = pTree->GetItemData( hItem );
		if( hChildItem != NULL || IS_FOLDER( dwId ) ) 
		{
#ifdef __N_WATERFRAME_050202
			//	폴더안에 폴더가 있을때 
			if ( TreeCount == 1 )
				nCount = NodeCount( pTree, hChildItem, TRUE );		//	폴더의 갯수를 카운트한다.
			else if ( TreeCount == 2 )
				nCount = NodeCount( pTree, hChildItem, FALSE );		//	이미지의 갯수를 카운트한다.
#endif//__N_WATERFRAME_050202
			strName = pTree->GetItemText( hItem );
#ifndef __N_WATERFRAME_050202
			string.Format( _T( "%s\"%s\"\r\n" ), strSpace, strName );
#else
			string.Format( _T( "%s\"%s\" %d\r\n" ), strSpace, strName, nCount );
#endif
			file.PutWideString( string );
			string.Format( _T( "%s{\r\n" ), strSpace );
			file.PutWideString( string );
			WriteTerrainScript( file, hChildItem, nInsSpace + 2 );
			string.Format( _T( "%s}\r\n" ), strSpace, strName );
			file.PutWideString( string );
		}
		else
		{
			LPTERRAIN lpTerrain = prj.m_terrainMng.GetTerrain( dwId );
			strName = pTree->GetItemText( hItem );
#ifndef __N_WATERFRAME_050202
			string.Format( _T( "%s%d \"%s\" %d \"%s\"\r\n" ), strSpace, dwId, strName, lpTerrain->m_bBlock, lpTerrain->m_szSoundFileName );
#else
			string.Format( _T( "%s%d %d \"%s\" %d \"%s\"\r\n" ), strSpace, dwId, 0, strName, lpTerrain->m_bBlock, lpTerrain->m_szSoundFileName );
#endif//__N_WATERFRAME_050202
			file.PutWideString( string );
		}
		hItem = pTree->GetNextSiblingItem( hItem );
	}
#ifdef __N_WATERFRAME_050202
	TreeCount--;
#endif//__N_WATERFRAME_050202
}
BOOL CModelView::SaveTerrainScript( LPCTSTR lpszFileName )
{
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wb" ) ) == FALSE )
		return FALSE;
	// unicode 인식 코드 
	file.PutC( (char)0xff );
	file.PutC( (char)0xfe );

	CTreeCtrl* pTree = &GetTreeCtrl();
	//DWORD dwId = script.GetNumber(); // id or folder
	//DWORD dwId;
	file.PutWideString( _T( "// Terrain Script\r\n\r\n" ) );
	WriteTerrainScript( file, pTree->GetChildItem( m_hRootTerrain ), 0 );
	return TRUE;
}

#ifdef __N_WATERFRAME_050202
int CModelView::NodeCount( CTreeCtrl *pTree, HTREEITEM hTree, BOOL IsFolder )
{
	int rCount = 0;
	while ( hTree != NULL )
	{
		if ( !IsFolder || IS_FOLDER( pTree->GetItemData( hTree ) ) )
		{
			rCount++;
		}
		hTree = pTree->GetNextSiblingItem( hTree );
	}
	return rCount;
}
#endif//__N_WATERFRAME_050202
void CModelView::WriteModelScript( CFileIO& file, HTREEITEM hItem, DWORD dwType, int nInsSpace )
{
	CTreeCtrl* pTree = &GetTreeCtrl();
//	TCHAR szObject[64];
	DWORD dwIndex;
	CString strHeadWord;
	CString strDefine1;
	CString strDefine2;
	CString strSpace;
	CString string;
	CString strName;
	for( int i = 0; i < nInsSpace; i++ )
		strSpace += _T( " " );
	while (	( hItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		HTREEITEM hChildItem = pTree->GetChildItem( hItem );
		dwIndex = pTree->GetItemData( hItem );
		//if( hChildItem != NULL || IS_FOLDER( dwIndex ) ) 
		if( IS_FOLDER( dwIndex ) ) 
		{
			strName = pTree->GetItemText( hItem );
			string.Format( _T( "%s\"%s\"\r\n" ), strSpace, strName );
			file.PutWideString( string );
			string.Format( _T( "%s{\r\n" ), strSpace );
			file.PutWideString( string );
			WriteModelScript( file, hChildItem, dwType, nInsSpace + 2 );
			string.Format( _T( "%s}\r\n" ), strSpace, strName );
			file.PutWideString( string );
		}
		else
		{
			LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( dwType, dwIndex );
			strName = pTree->GetItemText( hItem );
			switch( dwType )
			{
			case OT_OBJ   : strHeadWord = _T( "OI_" ); break;
			case OT_CTRL  : strHeadWord = _T( "CI_" ); break;
			case OT_SFX   : strHeadWord = _T( "XI_" ); break;
			case OT_ITEM  : strHeadWord = _T( "II_" ); break;
			case OT_MOVER : strHeadWord = _T( "MI_" ); break;
			case OT_REGION: strHeadWord = _T( "RI_" ); break;
			}
			strDefine1 = CScript::GetFindIdStr( strHeadWord, dwIndex );
			strDefine2 = CScript::GetFindIdStr( _T( "MODELTYPE_" ), lpModelElem->m_dwModelType );
			string.Format( _T( "%s\"%s\" %s %s \"%s\" %d %d %d %f %d %d %d %d\r\n" ), strSpace, lpModelElem->m_szName, 
				strDefine1, strDefine2, lpModelElem->m_szPart, lpModelElem->m_bFly, lpModelElem->m_dwDistant, lpModelElem->m_bPick, lpModelElem->m_fScale, lpModelElem->m_bTrans, lpModelElem->m_bShadow 
#ifdef	__XTEXTURE_EX
				,lpModelElem->m_bTextureEx 
#endif//__XTEXTURE_EX
#ifdef __YRENDER_FLAG
				,lpModelElem->m_bRenderFlag 
#endif //__YRENDER_FLAG			
				);
				//m_szBillboard );
			file.PutWideString( string );
			hChildItem = pTree->GetChildItem( hItem );
			if( hChildItem )
			{
				string.Format( _T( "%s{\r\n" ), strSpace, strName );
				file.PutWideString( string );
				while( hChildItem ) 
				{
					strName = pTree->GetItemText( hChildItem );
					dwIndex = pTree->GetItemData( hChildItem );
					strDefine1 = CScript::GetFindIdStr( _T( "MTI_" ), dwIndex );
					string.Format( _T( "  %s\"%s\" %s\r\n" ), strSpace, strName, strDefine1 );
					file.PutWideString( string );
					hChildItem = pTree->GetNextSiblingItem( hChildItem );
				}
				string.Format( _T( "%s}\r\n" ), strSpace, strName );
				file.PutWideString( string );
			}
		}
		hItem = pTree->GetNextSiblingItem( hItem );
	}
	/*
	CTreeCtrl* pTree = &GetTreeCtrl();
	TCHAR szObject[64];
	DWORD dwIndex;
	CString strHeadWord;
	CString strDefine1;
	CString strDefine2;
	CString strSpace;
	CString string;
	CString strName;
	for( int i = 0; i < nInsSpace; i++ )
		strSpace += _T( " " );
	while (	( hItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		HTREEITEM hChildItem = pTree->GetChildItem( hItem );
		dwIndex = pTree->GetItemData( hItem );
		//if( hChildItem != NULL || IS_FOLDER( dwIndex ) ) 
		if( IS_FOLDER( dwIndex ) ) 
		{
			strName = pTree->GetItemText( hItem );
			string.Format( _T( "%s\"%s\"\r\n" ), strSpace, strName );
			file.PutString( string );
			string.Format( _T( "%s{\r\n" ), strSpace );
			file.PutString( string );
			WriteModelScript( file, hChildItem, dwType, nInsSpace + 2 );
			string.Format( _T( "%s}\r\n" ), strSpace, strName );
			file.PutString( string );
		}
		else
		{
			LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( dwType, dwIndex );
			strName = pTree->GetItemText( hItem );
			switch( dwType )
			{
			case OT_OBJ   : strHeadWord = _T( "OI_" ); break;
			case OT_CTRL  : strHeadWord = _T( "CI_" ); break;
			case OT_SFX   : strHeadWord = _T( "XI_" ); break;
			case OT_ITEM  : strHeadWord = _T( "II_" ); break;
			case OT_MOVER : strHeadWord = _T( "MI_" ); break;
			case OT_REGION: strHeadWord = _T( "RI_" ); break;
			}
			strDefine1 = CScript::GetFindIdStr( strHeadWord, dwIndex );
			strDefine2 = CScript::GetFindIdStr( _T( "MODELTYPE_" ), lpModelElem->m_dwModelType );
			string.Format( _T( "%s\"%s\" %s %s \"%s\" %d %d %d %f %d %d \r\n" ), strSpace, strName, 
				strDefine1, strDefine2, lpModelElem->m_szPart, lpModelElem->m_bFly, lpModelElem->m_dwDistant, lpModelElem->m_bPick, lpModelElem->m_fScale, lpModelElem->m_bTrans, lpModelElem->m_bShadow );
				//m_szBillboard );
			file.PutString( string );
			hChildItem = pTree->GetChildItem( hItem );
			if( hChildItem )
			{
				string.Format( _T( "%s{\r\n" ), strSpace, strName );
				file.PutString( string );
				while( hChildItem ) 
				{
					strName = pTree->GetItemText( hChildItem );
					dwIndex = pTree->GetItemData( hChildItem );
					strDefine1 = CScript::GetFindIdStr( _T( "MTI_" ), dwIndex );
					string.Format( _T( "  %s\"%s\" %s\r\n" ), strSpace, strName, strDefine1 );
					file.PutString( string );
					hChildItem = pTree->GetNextSiblingItem( hChildItem );
				}
				string.Format( _T( "%s}\r\n" ), strSpace, strName );
				file.PutString( string );
			}
		}
		hItem = pTree->GetNextSiblingItem( hItem );
	}
	*/
}
BOOL CModelView::SaveModelScript( LPCTSTR lpszFileName, DWORD dwObjFilter )
{
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wb" ) ) == FALSE )
		return FALSE;

	// unicode 인식 코드 
	file.PutC( (char)0xff );
	file.PutC( (char)0xfe );

	CTreeCtrl* pTree = &GetTreeCtrl();
//	DWORD dwId;
	file.PutWideString( _T( "// Model Script\r\n" ) );
	// "MaDaPrBar01" 260 MODELTYPE_MESH "" 0 0 0 1.000000 0 0 
	file.PutWideString( _T( "// szName, iObject, dwModelType, szPart, bFly, bDistant, bPick, fScale, bTrans, bShadow bTextureEx \r\n\r\n" ) );

//	TCHAR szObject[64];
	DWORD dwType;//, dwIndex;
	CString strSpace;
	CString string;
	CString strName;
	HTREEITEM hItem = pTree->GetChildItem( m_hRootObject );

	while (	( hItem != NULL) ) 
	{
		HTREEITEM hChildItem = pTree->GetChildItem( hItem );
		dwType = pTree->GetItemData( hItem );
		if( ObjTypeToObjFilter( dwType ) & dwObjFilter )
		{
			if( hChildItem != NULL || IS_FOLDER( dwType ) ) 
			{
				dwType &= ~FOLDER;
				strName = pTree->GetItemText( hItem );
				string.Format( _T( "%s\"%s\" %d\r\n" ), strSpace, strName, dwType );
				file.PutWideString( string );
				string.Format( _T( "%s{\r\n" ), strSpace );
				file.PutWideString( string );
				WriteModelScript( file, hChildItem, dwType & ~FOLDER, 2 );
				string.Format( _T( "%s}\r\n" ), strSpace, strName );
				file.PutWideString( string );
			}
		}
		hItem = pTree->GetNextSiblingItem( hItem );
	}
	//WriteTerrainScript( file, pTree->GetChildItem( m_hRootObject ), 0 );

	/*
	CFileIO file;
	if( file.Open( lpszFileName, _T( "wt" ) ) == FALSE )
		return FALSE;
	CTreeCtrl* pTree = &GetTreeCtrl();
	//DWORD dwId = script.GetNumber(); // id or folder
	DWORD dwId;
	file.PutString( _T( "// Model Script\r\n" ) );
	// "MaDaPrBar01" 260 MODELTYPE_MESH "" 0 0 0 1.000000 0 0 
	file.PutString( _T( "// szName, iObject, dwModelType, szPart, fly, distant, pick, fScale, Trans, Shadow\r\n\r\n" ) );

	TCHAR szObject[64];
	DWORD dwType, dwIndex;
	CString strSpace;
	CString string;
	CString strName;
	HTREEITEM hItem = pTree->GetChildItem( m_hRootObject );
//	return dwData & ~FOLDER ;

	while (	( hItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		HTREEITEM hChildItem = pTree->GetChildItem( hItem );
		dwType = pTree->GetItemData( hItem );
		if( ObjTypeToObjFilter( dwType ) & dwObjFilter )
		{
			if( hChildItem != NULL || IS_FOLDER( dwType ) ) 
			{
				dwType &= ~FOLDER;
				strName = pTree->GetItemText( hItem );
				string.Format( _T( "%s\"%s\" %d\r\n" ), strSpace, strName, dwType );
				file.PutString( string );
				string.Format( _T( "%s{\r\n" ), strSpace );
				file.PutString( string );
				WriteModelScript( file, hChildItem, dwType & ~FOLDER, 2 );
				string.Format( _T( "%s}\r\n" ), strSpace, strName );
				file.PutString( string );
			}
		}
		hItem = pTree->GetNextSiblingItem( hItem );
	}
	//WriteTerrainScript( file, pTree->GetChildItem( m_hRootObject ), 0 );
	*/
	return TRUE;
}

BOOL CModelView::SaveModelTerrain()
{
	CTreeCtrl* pTree = &GetTreeCtrl();

//	if( m_bModifiedMdlObj )
	{
		if( SaveModelScript( MakePath( _T( "MdlObj.inc" ) ), OF_OBJ ) )
			m_bModifiedMdlObj = FALSE;
		else
			AfxMessageBox( _T( "MdlObj.inc를 저장할 수 없습니다. Read Only로 되어 있는게 아닌지 확인하십시오." ) );
	}
	/*
	if( m_bModifiedMdlDyna )
	{
		if( SaveModelScript( MakePath( _T( "MdlDyna.inc" ) ), OF_CTRL | OF_SFX | OF_ITEM | OF_MOVER | OF_REGION ) )
			m_bModifiedMdlDyna = FALSE;
		else
			AfxMessageBox( _T( "MdlDyna.inc를 저장할 수 없습니다. Read Only로 되어 있는게 아닌지 확인하십시오." ) );
	}
	*/
	if( m_bModifiedTerrain )
	{
		if( SaveTerrainScript( MakePath( _T( "Terrain.inc" ) ) ) )
			m_bModifiedTerrain = FALSE;
		else
			AfxMessageBox( _T( "Terrain.inc를 저장할 수 없습니다. Read Only로 되어 있는게 아닌지 확인하십시오." ) );
	}
	return m_bModifiedMdlObj == FALSE && m_bModifiedMdlDyna == FALSE && m_bModifiedTerrain == FALSE; 
}

/*
BOOL CModelView::LoadTerrainScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;
	CTreeCtrl* pTree = &GetTreeCtrl();
	HTREEITEM hRootItem = pTree->InsertItem( _T( "Terrain" ), IMAGE_ROOT, IMAGE_ROOT );

	LPTERRAIN lpTerrain;
	int i = script.GetNumber( TRUE );  // id
	while(script.tok!=FINISHED)
	{
		lpTerrain = &prj.m_terrainMng.m_aTerrain[ i ];
		lpTerrain->m_dwId = i;
		script.GetToken( TRUE );  // texture fileName
		strcpy( lpTerrain->m_szTextureFileName, script.token );
		HTREEITEM hr = pTree->InsertItem( script.token, IMAGE_TEXTURE, IMAGE_TEXTURE, hRootItem );
		pTree->SetItemData( hr, lpTerrain->m_dwId );
		lpTerrain->m_bBlock = script.GetNumber( TRUE ); // block
		script.GetToken( TRUE ); // sound
		strcpy( lpTerrain->m_szSoundFileName, script.token );
		i = script.GetNumber( TRUE );  // texture fileName
		//if( i > m_nSize ) m_nSize = i;
	}
	return TRUE;
}
*/
BOOL CModelView::OnPopupCommand(UINT nID)
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	DWORD dwData = pTreeCtrl->GetItemData( hItem );
	CString strName = pTreeCtrl->GetItemText( hItem );
	//LPMODELLEM pModelElem = prj.m_modelMng;
	DWORD dwType = dwData & ~FOLDER;

	HTREEITEM hRoot = GetRootItem( hItem );
	CString strRootName = pTreeCtrl->GetItemText( hRoot );
	if ( strRootName == _T( "Terrain" ) )
	{
		switch( nID )
		{
			case ID_POPUP_MODEL_ADD:
				{
					CString str = MakePath( DIR_WORLD, "texture" );
					if( ::SetCurrentDirectory( str ) )
					{
						int a;
						a = GetLastError();
					}
					CFileDialog dlg( TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT , 
						"Bitmap file (*.bmp)|*.bmp|Targa file (*.tga)|*.tga|DDS file (*.dds)|*.dds||");
					if( dlg.DoModal() == IDOK ) 
					{
						POSITION pos = dlg.GetStartPosition();
						while( pos ) 
						{
							TCHAR szDrive[8], szDir[128], szName[128], szExt[128];
							_splitpath( dlg.GetNextPathName(pos), szDrive, szDir, szName, szExt );
							strcat( szName, szExt );
							CString strFileName = szName;
							CString strLowerName = szName;
							strLowerName.MakeLower();
							DWORD dwId = prj.m_terrainMng.AddTerrain( dlg.GetFileName() );
							HTREEITEM hr = pTreeCtrl->InsertItem( strFileName, IMAGE_TEXTURE, IMAGE_TEXTURE, hItem, TVI_FIRST );
							pTreeCtrl->SetItemData( hr, dwId );							
						}
						m_bModifiedTerrain = TRUE;			
					}
				}
				break;
			case ID_POPUP_MODEL_NEWFOLDER:
				{
					CDlgNewFolder dlgNewFolder;
					if( dlgNewFolder.DoModal() == IDOK )
					{
						HTREEITEM hr = pTreeCtrl->InsertItem( dlgNewFolder.m_strFolderName, IMAGE_FOLDER, IMAGE_FOLDER, hItem, TVI_FIRST );
						pTreeCtrl->SetItemData( hr, FOLDER | GetModelType( hItem ) );		
						m_bModifiedTerrain = TRUE;
					}
				}
				break;
			case ID_POPUP_MODEL_REMOVE:
				{
					CString string;
					string.Format( _T( "%s를 삭제하시겠습니까?" ), strName );
					if( AfxMessageBox( string, MB_OKCANCEL ) == IDOK )
					{
						DWORD dwData = pTreeCtrl->GetItemData( hItem );
						DWORD dwType = GetModelType( hItem );
						if( IS_FOLDER( dwData ) == FALSE )
							prj.m_terrainMng.RemoveTerrain( dwData );			
						RemoveTerrain( pTreeCtrl->GetChildItem( hItem ) );
						pTreeCtrl->DeleteItem( hItem );
						m_bModifiedTerrain = TRUE;
					}
				}
				break;
		}
	}
	else
	//if( !( dwData & FOLDER ) )
	{
		switch( nID )
		{
			case ID_POPUP_MODEL_ADD:
				{
					CString strO3d, strHead;
					switch( dwType )
					{
					case OT_OBJ   : strO3d = _T( "obj_*.*"  ); strHead = _T( "obj_"  ); break;
					case OT_CTRL  : strO3d = _T( "ctrl_*.*" ); strHead = _T( "ctrl_" ); break;
					case OT_ITEM  : strO3d = _T( "item_*.*" ); strHead = _T( "item_" ); break;
					case OT_SFX   : strO3d = _T( "sfx_*.*"  ); strHead = _T( "sfx_"  ); break;
					case OT_MOVER : strO3d = _T( "mvr_*.*"  ); strHead = _T( "mvr_"  ); break;
					case OT_REGION: strO3d = _T( "rgn_*.*"  ); strHead = _T( "rgn_"  ); break;
					}
					SetCurrentDirectory( MakePath( DIR_MODELTEX, "" ) );
					CFileDialog dlg( TRUE, NULL, strO3d, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT ,  
						"Model file (*.o3d)|*.o3d|SFX Model file (*.sfx)|*.sfx||");
					if( dlg.DoModal() == IDOK ) 
					{
						POSITION pos = dlg.GetStartPosition();
						while( pos ) 
						{
							TCHAR szDrive[8], szDir[128], szName[128], szExt[128];
							_splitpath( dlg.GetNextPathName(pos), szDrive, szDir, szName, szExt );
							strcat( szName, szExt );
							CString strFileName = szName;
							CString strLowerName = szName;
							strLowerName.MakeLower();
							if( strLowerName.Find( strHead, 0 ) != 0 )
							{
								strO3d.Format( _T( "파일이 %s로 시작하지 않습니다." ), strHead );
								AfxMessageBox( strO3d );
							}
							else
							{
								strFileName = strFileName.Right( strFileName.GetLength() - 4 );
								strFileName = strFileName.Left( strFileName.GetLength() - 4 );
								MODELELEM modelElem;
								ZeroMemory( &modelElem, sizeof( modelElem ) );
								modelElem.m_dwIndex = prj.m_modelMng.GetNewModelId( dwType );
								_tcscpy( modelElem.m_szName, strFileName );
								if( strcmp( szExt, ".sfx" ) == 0 )
									modelElem.m_dwModelType = MODELTYPE_SFX;
								else
								{
									if( CObject3D::IsAnimateFile( szName ) )
										modelElem.m_dwModelType = MODELTYPE_ANIMATED_MESH;
									else
										modelElem.m_dwModelType = MODELTYPE_MESH;
								}
								modelElem.m_fScale = 1.0f;
								if( prj.m_modelMng.AddModel( dwType, &modelElem ) == TRUE )
								{
									HTREEITEM hr = pTreeCtrl->InsertItem( strFileName, IMAGE_MODEL, IMAGE_MODEL, hItem, TVI_FIRST );
									pTreeCtrl->SetItemData( hr, modelElem.m_dwIndex );							
									if( dwType == OT_OBJ )
										m_bModifiedMdlObj = TRUE;
									else
										m_bModifiedMdlDyna = TRUE;
								}
							}
						}
					}
				}
				break;
			case ID_POPUP_MODEL_NEWFOLDER:
				{
					CDlgNewFolder dlgNewFolder;
					if( dlgNewFolder.DoModal() == IDOK )
					{
						HTREEITEM hr = pTreeCtrl->InsertItem( dlgNewFolder.m_strFolderName, IMAGE_FOLDER, IMAGE_FOLDER, hItem, TVI_FIRST );
						pTreeCtrl->SetItemData( hr, FOLDER | GetModelType( hItem ) );	
						if( dwType == OT_OBJ )
							m_bModifiedMdlObj = TRUE;
						else
							m_bModifiedMdlDyna = TRUE;
					}
				}
				break;
			case ID_POPUP_MODEL_REMOVE:
				{
					CString string;
					string.Format( _T( "%s를 삭제하시겠습니까?" ), strName );
					if( AfxMessageBox( string, MB_OKCANCEL ) == IDOK )
					{
						DWORD dwData = pTreeCtrl->GetItemData( hItem );
						DWORD dwType = GetModelType( hItem );
						if( IS_FOLDER( dwData ) )
							RemoveModelElem( pTreeCtrl->GetChildItem( hItem ) );
						else
							prj.m_modelMng.RemoveModel( dwType, dwData );			
						pTreeCtrl->DeleteItem( hItem );
						if( dwType == OT_OBJ )
							m_bModifiedMdlObj = TRUE;
						else
							m_bModifiedMdlDyna = TRUE;
					}
				}
				break;
		}
	}
	return TRUE;
}
	
DWORD CModelView::GetModelType( HTREEITEM hCurrent )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hRootItem = pTreeCtrl->GetRootItem();
	HTREEITEM htiParent = hCurrent;
	DWORD dwData = 0;
	do
	{
		dwData = pTreeCtrl->GetItemData( htiParent );
		if( dwData & FOLDER )
			return dwData & ~FOLDER ;
		htiParent = pTreeCtrl->GetParentItem( htiParent );
	}
	while( htiParent != hRootItem && htiParent );
	return dwData;
}
/*
HTREEITEM CModelView::GetRootItem( HTREEITEM hCurrent )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM htiParent = hCurrent;
	do
	{
		htiParent = hCurrent;
		hCurrent = pTreeCtrl->GetParentItem( htiParent );
	}
	while( hCurrent );
	return htiParent;
}
*/
void CModelView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CTreeView::OnLButtonUp(nFlags, point);
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();

	if ( m_bLDragging && m_hitemDrop )
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		delete m_pDragImage;

		// Remove drop target highlighting
		pTreeCtrl->SelectDropTarget(NULL);

		if( m_hitemDrag == m_hitemDrop )
			return;

		// If Drag item is an ancestor of Drop item then return
		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = pTreeCtrl->GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return;
		}
		// 루트가 같지 않으면 리턴 
		HTREEITEM hRootDrag = GetRootItem( m_hitemDrag );
		HTREEITEM hRootDrop = GetRootItem( m_hitemDrop );
		if( hRootDrag != hRootDrop )
			return;
		// 목표가 루트 오브젝트이면 리턴 
		if( m_hitemDrop == m_hRootObject )
			return;
		// 모델일 경우 타입이 같지 않으면 리턴. 
		if( hRootDrop == m_hRootObject )
		{
			DWORD dwModelTypeDrag = GetModelType( m_hitemDrag );
			DWORD dwModelTypeDrop = GetModelType( m_hitemDrop );
			if( dwModelTypeDrag != dwModelTypeDrop )
				return;
			if( dwModelTypeDrop == OT_OBJ )
				m_bModifiedMdlObj = TRUE;
			else
				m_bModifiedMdlDyna = TRUE;
		}
		else
		{
			// 내려놓는 곳이 지형이면 지형 모디피드 플렉 세팅 
			if( hRootDrop == m_hRootTerrain )
				m_bModifiedTerrain = TRUE;
		}

	//	pTreeCtrl->Expand( m_hitemDrop, TVE_EXPAND ) ;
		//HTREEITEM htiNew = pTreeCtrl->CopyBranch( m_hitemDrag, m_hitemDrop, TVI_LAST );

		
		CString string = pTreeCtrl->GetItemText( m_hitemDrag );
		DWORD dwData = pTreeCtrl->GetItemData( m_hitemDrag );
		//if( pTreeCtrl->GetItem( &tvItem ) )
		{
			HTREEITEM htiParent;
			// 내려놓을 곳이 폴더인가?
			if( pTreeCtrl->GetItemData( m_hitemDrop ) & FOLDER )
				htiParent = m_hitemDrop;
			else
				htiParent = pTreeCtrl->GetParentItem( m_hitemDrop );

			//m_hDestItem = pTreeCtrl->InsertItem(string, 0, 0, htiParent, TVI_FIRST );//>   //AddItem(hTDropItem, m_pSourceTreeCtrl->GetItemText(hSelectedItem), TVI_LAST, nImage, nSelImage, 0);
			//AddItem( pTreeCtrl->GetChildItem(hSelectedItem), m_hDestItem );

			int nImage, nSelImage;
			GetItemImages( m_hitemDrag, nImage, nSelImage );

			HTREEITEM htiNew;
			// 놓을 자리가 폴더다.
			if( htiParent == m_hitemDrop )
				htiNew = pTreeCtrl->InsertItem( string, nImage, nSelImage, htiParent, TVI_FIRST );
			else
				htiNew = pTreeCtrl->InsertItem( string, nImage, nSelImage, htiParent, m_hitemDrop );
			pTreeCtrl->SetItemData( htiNew, dwData );
			AddItem( pTreeCtrl->GetChildItem( m_hitemDrag ), htiNew );
	
			pTreeCtrl->DeleteItem( m_hitemDrag );
			pTreeCtrl->SelectItem( htiNew );
		}
	}

}
/*
HTREEITEM CModelView::AddItem( HTREEITEM hParent, CString csItemName, HTREEITEM hInsAfter, int iSelImage, int iNonSelImage, long lParam )
{
	HTREEITEM hItem;
	TV_ITEM tvItem;
	TV_INSERTSTRUCT tvIns;

	tvItem.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	int nLength = csItemName.GetLength() ;
	tvItem.pszText = csItemName.GetBuffer(nLength);
	tvItem.cchTextMax = nLength;
	tvItem.lParam = lParam;

	tvItem.iImage = iNonSelImage;
	tvItem.iSelectedImage = iSelImage;
	
	tvIns.item = tvItem;
	tvIns.hInsertAfter = hInsAfter;
	tvIns.hParent = hParent;

	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	hItem = InsertItem(&tvIns);
	
	return (hItem);
}
*/
int CModelView::GetItemImage( HTREEITEM hSrcTItem )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	CImageList *pImageList = pTreeCtrl->GetImageList(TVSIL_NORMAL);
	//If no image list is associated with the tree control, return
	int nSelItemImage = 0, nNonSelItemImage = 0;
	if (pImageList  )
	{
		// If no image list is associated with Destination tree control
		// Set the image list of source tree control
		if ( pTreeCtrl->GetImageList(TVSIL_NORMAL) == NULL )
		{
			pTreeCtrl->SetImageList(pImageList, TVSIL_NORMAL);
		}
		// Get the image indexes
		pTreeCtrl->GetItemImage(hSrcTItem, nSelItemImage, nNonSelItemImage);
	}
	return nSelItemImage;
}

void CModelView::GetItemImages(HTREEITEM hSrcTItem, int &nSelItemImage, int &nNonSelItemImage)
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	CImageList *pImageList = pTreeCtrl->GetImageList(TVSIL_NORMAL);
	//If no image list is associated with the tree control, return
	if (pImageList == NULL )
	{
		nSelItemImage =0;
		nNonSelItemImage = 0;
	}
	else
	{
		// If no image list is associated with Destination tree control
		// Set the image list of source tree control
		if ( pTreeCtrl->GetImageList(TVSIL_NORMAL) == NULL )
		{
			pTreeCtrl->SetImageList(pImageList, TVSIL_NORMAL);
		}
		// Get the image indexes
		pTreeCtrl->GetItemImage(hSrcTItem, nSelItemImage, nNonSelItemImage);
	}
}
void CModelView::RemoveModelElem( HTREEITEM hItem )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	while (	( hItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		// Get the item text and Image indexes for source tree control
		//CString csItem = pTreeCtrl->GetItemText( hSrcTItem );
		DWORD dwData = pTreeCtrl->GetItemData( hItem );
		DWORD dwType = GetModelType( hItem );

		if( IS_FOLDER( dwData ) == FALSE )
			prj.m_modelMng.RemoveModel( dwType, dwData & ~FOLDER );
		
		//GetItemImages( hSrcTItem, nImage, nSelImage );
		
		// Add item to destination tree control
		//HTREEITEM hDestChildItem = pTreeCtrl->InsertItem( csItem, nImage, nSelImage, hDestTItem, TVI_LAST );//AddItem( hDestTItem, csItem, TVI_LAST, nImage, nSelImage, 0 );
		//pTreeCtrl->SetItemData( hDestChildItem, dwData );
		HTREEITEM hChildItem = pTreeCtrl->GetChildItem( hItem );
		
		if( hChildItem != NULL ) 
			RemoveModelElem( hChildItem );
		
		hItem = pTreeCtrl->GetNextSiblingItem( hItem );
	}
	
}
void CModelView::RemoveTerrain( HTREEITEM hItem )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	while (	( hItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		// Get the item text and Image indexes for source tree control
		//CString csItem = pTreeCtrl->GetItemText( hSrcTItem );
		DWORD dwData = pTreeCtrl->GetItemData( hItem );

		if( IS_FOLDER( dwData ) == FALSE )
			prj.m_terrainMng.RemoveTerrain( dwData );
		
		//GetItemImages( hSrcTItem, nImage, nSelImage );
		
		// Add item to destination tree control
		//HTREEITEM hDestChildItem = pTreeCtrl->InsertItem( csItem, nImage, nSelImage, hDestTItem, TVI_LAST );//AddItem( hDestTItem, csItem, TVI_LAST, nImage, nSelImage, 0 );
		//pTreeCtrl->SetItemData( hDestChildItem, dwData );
		HTREEITEM hChildItem = pTreeCtrl->GetChildItem( hItem );
		
		if( hChildItem != NULL ) 
			RemoveModelElem( hChildItem );
		
		hItem = pTreeCtrl->GetNextSiblingItem( hItem );
	}
	
}
void CModelView::AddItem( HTREEITEM hSrcTItem, HTREEITEM hDestTItem )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	int nImage;
	int nSelImage;
	while (	( hSrcTItem != NULL) ) //&& ( hSrcTItem != m_hDestItem ) ) 
	{
		// Get the item text and Image indexes for source tree control
		CString csItem = pTreeCtrl->GetItemText( hSrcTItem );
		DWORD dwData = pTreeCtrl->GetItemData( hSrcTItem );
		GetItemImages( hSrcTItem, nImage, nSelImage );
		
		// Add item to destination tree control
		HTREEITEM hDestChildItem = pTreeCtrl->InsertItem( csItem, nImage, nSelImage, hDestTItem, TVI_LAST );//AddItem( hDestTItem, csItem, TVI_LAST, nImage, nSelImage, 0 );
		pTreeCtrl->SetItemData( hDestChildItem, dwData );
		HTREEITEM hSrcChildItem = pTreeCtrl->GetChildItem( hSrcTItem );
		
		if( hSrcChildItem != NULL ) 
			AddItem( hSrcChildItem, hDestChildItem );
		
		hSrcTItem = pTreeCtrl->GetNextSiblingItem( hSrcTItem );
		hDestTItem = pTreeCtrl->GetParentItem( hDestChildItem );
	}
	
}
void CModelView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
		HTREEITEM	hitem;
	UINT		flags;
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	if (m_bLDragging)
	{
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);
		if ((hitem = pTreeCtrl->HitTest(point, &flags)) != NULL)
		{
			CImageList::DragShowNolock(FALSE);
			pTreeCtrl->SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
	}
	
//	CTreeCtrl::OnMouseMove(nFlags, point);

	CTreeView::OnMouseMove(nFlags, point);
}

void CModelView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;

	if( GetItemImage( m_hitemDrag ) == IMAGE_MOTION )
		return;

	m_pDragImage = pTreeCtrl->CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() returns NULL if no image list
	// associated with the tree view control
	if( !m_pDragImage )
		return;

	m_bLDragging = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

}
/////////////////////////////////////////////////////////////////////////////
// CDlgNewFolder dialog


CDlgNewFolder::CDlgNewFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewFolder)
	m_strFolderName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewFolder)
	DDX_Text(pDX, IDC_EDIT1, m_strFolderName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewFolder, CDialog)
	//{{AFX_MSG_MAP(CDlgNewFolder)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewFolder message handlers

void CModelView::OnDestroy() 
{
	CTreeView::OnDestroy();
}
