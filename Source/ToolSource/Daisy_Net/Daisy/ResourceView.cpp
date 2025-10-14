// DialogView.cpp : implementation file
//

#include "stdafx.h"
#include "daisy.h"
#include "DaisyDoc.h"
#include "DaisyView.h"
#include "ResourceView.h"
//#include ".\resourceview.h"
#include "SearchAppName.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceView

IMPLEMENT_DYNCREATE(CResourceView, CTreeView)

CResourceView::CResourceView()
{
}

CResourceView::~CResourceView()
{
}


BEGIN_MESSAGE_MAP(CResourceView, CTreeView)
	//{{AFX_MSG_MAP(CResourceView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	ON_COMMAND_EX(ID_POPUP_RESOURCE_NEW, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_RESOURCE_OPEN, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_RESOURCE_REMOVE, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_RESOURCE_CLOSE, OnPopupCommand)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SEARCH, OnSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceView drawing

void CResourceView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CResourceView diagnostics

#ifdef _DEBUG
void CResourceView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CResourceView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CResourceView message handlers

int CResourceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
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
	pTreeCtrl->GetItemText(m_hApplet);
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
	
	return 0;
}
void CResourceView::AddApplet( LPCTSTR lpszWIDC, LPWNDAPPLET LPWNDAPPLET )
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	LPWNDAPPLET->nDefSizeX = 0;
	LPWNDAPPLET->nDefSizeY = 0;

	HTREEITEM hItem = pTreeCtrl->InsertItem( lpszWIDC, 1, 1, m_hApplet );
	pTreeCtrl->SetItemData( hItem, (DWORD)LPWNDAPPLET );
}
void CResourceView::ChangeApplet( LPCTSTR lpszOldWIDC, LPCTSTR lpszNewWIDC )
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
void CResourceView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnPopupCommand( ID_POPUP_RESOURCE_OPEN );
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
		CResourceView* pResourceView = (CResourceView*)g_pMainFrame->m_wndSTCBar.GetView( 0 );
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

void CResourceView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();

	POINT point;
	GetCursorPos( &point );
	ScreenToClient( &point ); 
	
	UINT nFlags;
	HTREEITEM hTree = pTreeCtrl->HitTest( point, &nFlags );
	if( NULL == hTree )
		return ;
	
	pTreeCtrl->SelectItem( hTree );
	CMenu menu; 
	menu.CreatePopupMenu();
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
	::GetCursorPos( &point );
	if( menu.TrackPopupMenu( TPM_LEFTALIGN            , point.x, point.y, this ) )
		int a = 0;//m_pPopupBkg = pBkg;
	*pResult = 1;
}

void CResourceView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CResourceView::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
BOOL CResourceView::OnPopupCommand(UINT nID)
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	LPWNDAPPLET lpWndApplet = (LPWNDAPPLET)pTreeCtrl->GetItemData( hItem );
	if( lpWndApplet )
	{
		switch( nID )
		{
			case ID_POPUP_RESOURCE_OPEN:
				{
					AppOpen(lpWndApplet);
				}
				break;
			case ID_POPUP_RESOURCE_REMOVE:
				{
					AppRemove(lpWndApplet,pTreeCtrl,hItem);
				}
				break;
			case ID_POPUP_RESOURCE_CLOSE:
				{
					AppClose(lpWndApplet);
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
				NewApplet();
			}
			break;
		}
	}
	g_pMainFrame->m_pDaisyView->Invalidate( FALSE );
	return TRUE;
}
void CResourceView::AppClose(LPWNDAPPLET lpWndApplet)
{
	if( lpWndApplet->pWndBase )
	{
		lpWndApplet->pWndBase->Destroy( TRUE );
		lpWndApplet->pWndBase = NULL;
	}
}
void CResourceView::AppRemove(LPWNDAPPLET lpWndApplet,CTreeCtrl* pTreeCtrl,HTREEITEM hItem)
{
	pTreeCtrl->DeleteItem( hItem );
	if( lpWndApplet->pWndBase )
	{
		lpWndApplet->pWndBase->Destroy( TRUE );
		lpWndApplet->pWndBase = NULL;
	}
	
	CWndBase::m_resMng.RemoveApplet( lpWndApplet->dwWndId );
}
void CResourceView::AppOpen(LPWNDAPPLET lpWndApplet)
{
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
}

void CResourceView::NewApplet()
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
	wndApplet.strTexture = DEF_WNDBASE;	
	wndApplet.m_bTile = TRUE;
	wndApplet.d3dFormat = D3DFMT_A4R4G4B4;

	pWndDialog->SetTitle( wndApplet.strTitle );

	LPWNDAPPLET lpWndApplet;
	if( CWndBase::m_resMng.AddApplet( &wndApplet ) )
	{
		lpWndApplet = CWndBase::m_resMng.GetAt( wndApplet.dwWndId );
		AddApplet( wndApplet.strDefine, lpWndApplet );
		pWndDialog->m_lpWndApplet = lpWndApplet;
		if( lpWndApplet->m_bTile )
		{
			pWndDialog->m_strTexture = lpWndApplet->strTexture;
			pWndDialog->m_bTile = lpWndApplet->m_bTile ? true : false;
		}
		pWndDialog->InitDeviceObjects();
		pWndDialog->RestoreDeviceObjects();
		pWndDialog->AdjustWndBase();
		// 이거 안하면 화면에 안나옴. 이거해야 버텍스 버퍼 만들고 디바이스 재조정함.
		pWndDialog->SetWndRect( CRect( 0, 0, wndApplet.size.cx, wndApplet.size.cy  ) );
		SortTreeApplet();
	}
}

void CResourceView::SortTreeApplet()
{
	CTreeCtrl* pTreeCtrl = &GetTreeCtrl();
	pTreeCtrl->SortChildren( m_hApplet );
}
void CResourceView::OnSearch()
{
	CSearchAppName	SearchAppName;
	SearchAppName.DoModal();
}
