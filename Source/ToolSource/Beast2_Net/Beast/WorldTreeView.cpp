// DialogView.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "WorldDoc.h"
#include "WorldView.h"
#include "WorldTreeView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldTreeView

IMPLEMENT_DYNCREATE(CWorldTreeView, CTreeView)

CWorldTreeView::CWorldTreeView()
{
}

CWorldTreeView::~CWorldTreeView()
{
}


BEGIN_MESSAGE_MAP(CWorldTreeView, CTreeView)
	//{{AFX_MSG_MAP(CWorldTreeView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldTreeView drawing

void CWorldTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CWorldTreeView diagnostics

#ifdef _DEBUG
void CWorldTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CWorldTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldTreeView message handlers

int CWorldTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	//CTreeCtrl
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	pTreeCtrl->ModifyStyle(0, TVS_HASBUTTONS|TVS_LINESATROOT|TVS_HASLINES|TVS_SHOWSELALWAYS);
#ifdef __CSC_WORLDOBJ_LIST
	HTREEITEM hRootItem = pTreeCtrl->InsertItem("WorldList",0,0);
	m_images.Create(IDB_TABIMAGES1, 16, 1, RGB(255,0,255));
	pTreeCtrl->SetImageList(&m_images,TVSIL_NORMAL );

	GetTreeCtrl().SelectItem(hRootItem);
	GetTreeCtrl().Expand(hRootItem, TVE_EXPAND);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_pWorldObjList = this;

#else //__CSC_WORLDOBJ_LIST
	HTREEITEM hRootItem = pTreeCtrl->InsertItem("Resources",0,0);
	pTreeCtrl->InsertItem( _T( "Bitmap"       ), 0, 0, hRootItem );
	m_hApplet = pTreeCtrl->InsertItem( _T( "Applet" ), 0, 1, hRootItem );
	pTreeCtrl->InsertItem( _T( "Icon"         ), 0, 0, hRootItem );
	pTreeCtrl->InsertItem( _T( "Menu"         ), 0, 0, hRootItem );
	pTreeCtrl->InsertItem( _T( "String Table" ), 0, 0, hRootItem );
	pTreeCtrl->InsertItem( _T( "Toolbar"      ), 0, 0, hRootItem );

	m_images.Create(IDB_TABIMAGES, 16, 1, RGB(255,0,255));
	pTreeCtrl->SetImageList(&m_images,TVSIL_NORMAL );
	
	/*
	for (int i=0; i<5; i++)
	{
		CString strItem;
		strItem.Format("File %d", i);
		GetTreeCtrl().InsertItem(strItem,i,i,hRootItem);
	}
	*/
	GetTreeCtrl().SelectItem(hRootItem);
	GetTreeCtrl().Expand(hRootItem, TVE_EXPAND);
#endif //__CSC_WORLDOBJ_LIST
	return 0;
}

#ifdef __CSC_WORLDOBJ_LIST
void CWorldTreeView::ResetObjList()
{
	for(int i=0; i<MAX_OBJTYPE; i++)
	{
		map<DWORD, __WORLD_ELEMENT>::iterator it = m_MapList[i].begin();
		
		while(it != m_MapList[i].end())
		{
			__WORLD_ELEMENT* pElement = &it->second;
			pElement->m_veclist.clear();
			it++;
		}

		m_MapList[i].clear();
	}

	GetTreeCtrl().DeleteAllItems();
	GetTreeCtrl().InsertItem("WorldList",0,0);
}

void CWorldTreeView::MakeObjList(CLandscape* pLand)
{
	for(int i=0; i<MAX_OBJTYPE; i++)
	{
		CObj** apObject = pLand->m_apObject[i];
		int nObjNum = int( pLand->m_adwObjNum[i] );

		for(int j=0; j<nObjNum; j++)
		{
			if( IsValidObj( apObject[j] ) )
			{
				map<DWORD, __WORLD_ELEMENT>::iterator it = m_MapList[i].find(apObject[j]->m_dwIndex);

				if(it == m_MapList[i].end())
				{
					__WORLD_ELEMENT strElement;
					strElement.m_dwIndex = apObject[j]->m_dwIndex;
					strElement.m_dwType = apObject[j]->m_dwType;
					strElement.m_veclist.push_back((void*)apObject[j]);
					m_MapList[i].insert(make_pair( apObject[j]->m_dwIndex, strElement ));
				}
				else
				{
					__WORLD_ELEMENT* pstrElement = &it->second;
					pstrElement->m_veclist.push_back((void*)apObject[j]);
				}
			}
		}
	}
}

void CWorldTreeView::ResetTree()
{
	HTREEITEM hRootItem = GetTreeCtrl().GetRootItem();

	for(int i=0; i<MAX_OBJTYPE; i++)
	{
		HTREEITEM hSubItem = NULL;
		if(m_MapList[i].size() > 0)
		{
			switch(i)
			{
				case OT_OBJ:
					hSubItem = GetTreeCtrl().InsertItem( _T("OBJ"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_ANI:
					hSubItem = GetTreeCtrl().InsertItem( _T("ANI"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_CTRL:
					hSubItem = GetTreeCtrl().InsertItem( _T("CTRL"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_SFX:
					hSubItem = GetTreeCtrl().InsertItem( _T("SFX"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_ITEM:
					hSubItem = GetTreeCtrl().InsertItem( _T("ITEM"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_MOVER:
					hSubItem = GetTreeCtrl().InsertItem( _T("MOVER"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_REGION:
					hSubItem = GetTreeCtrl().InsertItem( _T("REGION"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_SHIP:
					hSubItem = GetTreeCtrl().InsertItem( _T("SHIP"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );
					break;
				case OT_PATH:
					hSubItem = GetTreeCtrl().InsertItem( _T("PATH"), IMAGE_ROOT, IMAGE_ROOT, hRootItem );	
					break;
			}
			
			map<DWORD, __WORLD_ELEMENT>::iterator it = m_MapList[i].begin();
			
			while(it != m_MapList[i].end())
			{
				__WORLD_ELEMENT* pElement = &it->second;
				LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( pElement->m_dwType, pElement->m_dwIndex );
				CString temp, szName;
			
				if(i == OT_MOVER)
				{
					vector<void*>::iterator iit = pElement->m_veclist.begin();
					CMover* pMover = (CMover*)(*iit);
					if(pMover && strlen(pMover->m_szName) > 0)
						szName.Format("%s", pMover->m_szName);
					else
						szName.Format("%s", lpModelElem->m_szName);
				}
				else
					szName.Format("%s", lpModelElem->m_szName);

				if(pElement->m_veclist.size() > 1)
				{
					HTREEITEM hSubItem2;					
					if(lpModelElem)
						temp.Format("%s", szName);

					hSubItem2 = GetTreeCtrl().InsertItem( temp, IMAGE_FOLDER, IMAGE_FOLDER, hSubItem );
					
					for(int k=0; k<(int)( pElement->m_veclist.size() ); k++)
					{
						if(lpModelElem)
							temp.Format("%s__%d", szName, k+1);
						HTREEITEM hItem = GetTreeCtrl().InsertItem( temp, IMAGE_TEXTURE, IMAGE_TEXTURE, hSubItem2 );
						GetTreeCtrl().SetItemData(hItem, pElement->m_dwIndex);
					}
				}
				else
				{
					if(lpModelElem)
						temp.Format("%s", szName);
					HTREEITEM hItem = GetTreeCtrl().InsertItem( temp, IMAGE_TEXTURE, IMAGE_TEXTURE, hSubItem );
					GetTreeCtrl().SetItemData(hItem, pElement->m_dwIndex);
				}

				it++;
			}
		}
	}

	::InvalidateRect(this->m_hWnd,NULL,TRUE);
}
#endif //__CSC_WORLDOBJ_LIST
/*
void CWorldTreeView::AddApplet( LPCTSTR lpszWIDC, LPWNDAPPLET LPWNDAPPLET )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hItem = pTreeCtrl->InsertItem( lpszWIDC, 1, 1, m_hApplet );
	pTreeCtrl->SetItemData( hItem, (DWORD)LPWNDAPPLET );
}
void CWorldTreeView::ChangeApplet( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC )
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
void CWorldTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
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
		CWorldTreeView* pResourceView = (CWorldTreeView*)g_pMainFrame->m_wndSTCBar.GetView( 0 );
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

void CWorldTreeView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	POINT point;
	::GetCursorPos( &point );
	ScreenToClient( &point ); 
	UINT nFlags;
	HTREEITEM hTree = pTreeCtrl->HitTest( point, &nFlags );
	pTreeCtrl->SelectItem( hTree );
	CMenu menu; 
	menu.CreatePopupMenu();
	/*
	DWORD a;

	if( a = pTreeCtrl->GetItemData( hTree ) )
	{
		menu.AppendMenu(MF_STRING, ID_POPUP_RESOURCE_OPEN,"&Open");
		menu.AppendMenu(MF_STRING, ID_POPUP_RESOURCE_REMOVE,"&Remove");
		menu.AppendMenu(MF_STRING, ID_POPUP_RESOURCE_CLOSE,"&Close");
	}
	else
	{
		menu.AppendMenu(MF_STRING, ID_POPUP_RESOURCE_NEW,"&New");
	}
	*/
	::GetCursorPos( &point );
	if( menu.TrackPopupMenu( TPM_LEFTALIGN            , point.x, point.y, this ) )
	{
		;//m_pPopupBkg = pBkg;
	}

	*pResult = 1;
}

void CWorldTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
#ifdef __CSC_WORLDOBJ_LIST
	CTreeCtrl* pTree = &GetTreeCtrl();
	if( pNMTreeView->itemNew.hItem == 0 )
		return;
	DWORD dwIndex = pTree->GetItemData(pNMTreeView->itemNew.hItem);

	if(dwIndex > 0)
	{
		int nIndex = 0;
		CString strType;
		HTREEITEM hTypeItem;
		CString str = pTree->GetItemText(pNMTreeView->itemNew.hItem);
		int nUnderLine = str.Find("__", 0);
		if(nUnderLine > 0)
		{
			str.Delete(0, nUnderLine+2);
			nIndex += atoi(str) - 1;
			hTypeItem = pTree->GetParentItem(pNMTreeView->itemNew.hItem);
		}
		else
			hTypeItem = pNMTreeView->itemNew.hItem;

		HTREEITEM hTree = pTree->GetParentItem(hTypeItem);
		strType = pTree->GetItemText(hTree);

		int nType;
		if(strType == "OBJ")
			nType = 0;
		else if(strType == "ANI")
			nType = 1;
		else if(strType == "CTRL")
			nType = 2;
		else if(strType == "SFX")
			nType = 3;
		else if(strType == "ITEM")
			nType = 4;
		else if(strType == "MOVER")
			nType = 5;
		else if(strType == "REGION")
			nType = 6;
		else if(strType == "SHIP")
			nType = 7;
		else if(strType == "PATH")
			nType = 8;

		map<DWORD, __WORLD_ELEMENT>::iterator it = m_MapList[nType].find(dwIndex);

		if(it != m_MapList[nType].end())
		{
			__WORLD_ELEMENT* pstrElement = &it->second;

			int size = pstrElement->m_veclist.size();

			vector<void*>::iterator iter = pstrElement->m_veclist.begin();

			CObj* pObj = (CObj*)(*(iter+nIndex));
			CWorldView* pBeastView = (CWorldView*)g_pMainFrame->GetActiveView();
			
			if(pBeastView)
			{
				pBeastView->m_world.SetObjFocus( pObj );
				pBeastView->m_aPickCollection.RemoveAll();
				pBeastView->m_aPickCollection.Add( pObj );
				pBeastView->m_nPickCollection = 0;
				pBeastView->m_world.UnselectObj();
				pBeastView->m_world.SetSelectObj( pObj, TRUE );
				
				pBeastView->m_camera.m_vPos.x = pObj->GetPos().x;
				pBeastView->m_camera.m_vPos.z = pObj->GetPos().z;
				pBeastView->m_camera.m_fYaw = 0;
				pBeastView->m_camera.m_fPitch = D3DX_PI / 2;

				pBeastView->RenderScene();
			}

		}
	}
#endif //__CSC_WORLDOBJ_LIST
	*pResult = 0;
}

void CWorldTreeView::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
/*
BOOL CWorldTreeView::OnPopupCommand(UINT nID)
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
