/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998, 1999 by Cristi Posea
// All rights reserved
//
// Use and distribute freely, except: don't remove my name from the
// source or documentation (don't take credit for my work), mark your
// changes (don't get me blamed for your possible bugs), don't alter
// or remove this notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// This class is intended to be used as a base class. Do not simply add
// your code to this file - instead create a new class derived from
// CCoolDialogBar and put there what you need.
// Modify this file only to fix bugs, and don't forget to send me a copy.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc.,
// and I'll try to keep a version up to date.  I can be reached at:
//    cristip@dundas.com
//
// More details at MFC Programmer's SourceBook
// http://www.codeguru.com/docking/docking_window.shtml or search
// www.codeguru.com for my name if the article was moved.
//
/////////////////////////////////////////////////////////////////////////
//
// Acknowledgements:
//  o   Thanks to Harlan R. Seymour (harlans@dundas.com) for his continuous
//      support during development of this code.
//  o   Thanks to Dundas Software for the opportunity to test this code
//      on real-life applications.
//      If you don't know who they are, visit them at www.dundas.com .
//      Their award winning components and development suites are
//      a pile of gold.
//  o   Thanks to Chris Maunder (chrism@dundas.com) who came with the
//      simplest way to query "Show window content while dragging" system
//      setting.
//  o   Thanks to Zafir Anjum (zafir@codeguru.com) for publishing this
//      code on his cool site (www.codeguru.com).
//  o   Some ideas for the gripper came from the CToolBarEx flat toolbar
//      by Joerg Koenig (Joerg.Koenig@rhein-neckar.de). Also he inspired
//      me on writing this notice:) . Thanks, Joerg!
//  o   Thanks to Jakawan Ratiwanich (jack@alpha.fsec.ucf.edu) and to
//      Udo Schaefer (Udo.Schaefer@vcase.de) for the dwStyle bug fix under
//      VC++ 6.0.
//  o   And, of course, many thanks to all of you who used this code,
//      for the invaluable feedback I received.
//      
/////////////////////////////////////////////////////////////////////////


// sizecbar.cpp : implementation file
//

#include "stdafx.h"
#include "sizecbar2.h"
#include "CoolDialogBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSizingTabCtrlBar2

CSizingTabCtrlBar2::CSizingTabCtrlBar2()
{
	m_nActiveTab = 0;
}

CSizingTabCtrlBar2::~CSizingTabCtrlBar2()
{
	while(!m_views.IsEmpty())
	{
		TCB_ITEM2 *pMember=m_views.RemoveHead();
		delete pMember;
	}
}

BEGIN_MESSAGE_MAP(CSizingTabCtrlBar2, CCoolDialogBar ) //CCoolDialogBar)
	//{{AFX_MSG_MAP(CSizingTabCtrlBar2)
    ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDI_TABCTRLBAR, OnTabSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen f? Nachrichten CSizingTabCtrlBar2 

void CSizingTabCtrlBar2::OnPaint()
{
	CCoolDialogBar::OnPaint();
	
	
//	m_tabctrl.Invalidate( FALSE );

	CWnd *pWnd;
    for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    {
         pWnd=m_views.GetAt(pos)->pWnd;
        // pWnd->Invalidate( TRUE );
    }
}
int CSizingTabCtrlBar2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCoolDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//Create the Tab Control
	CRect rect;
	m_tabctrl.Create(WS_VISIBLE | WS_CHILD | TCS_BOTTOM, rect, this, IDI_TABCTRLBAR);
	//CTablCtrl
	//m_tabctrl.GetClientRect( &rect );
    //m_sizeHorz = m_sizeVert = m_sizeFloat = rect.Size();
	//m_sizeHorz.cy += m_cxEdge + m_cxBorder;
	//m_sizeVert.cx += m_cxEdge + m_cxBorder;

	m_images.Create(IDB_TABIMAGES, 16, 1, RGB(255,0,255));
	m_tabctrl.SetImageList(&m_images);
	
	// set "normal" GUI-font
	CFont *font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_tabctrl.SetFont(font);

	return 0;
}
void CSizingTabCtrlBar2::ResizeControls( CRect& rect)
{
	CWnd *pWnd;
    for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    {
         pWnd=m_views.GetAt(pos)->pWnd;
         pWnd->MoveWindow( 9, 9, rect.Width() - 19, rect.Height() - 36 );
    }
	m_tabctrl.MoveWindow( 7, 7, rect.Width() - 14, rect.Height() - 15 );
	m_tabctrl.Invalidate( );

}
void CSizingTabCtrlBar2::OnSize(UINT nType, int cx, int cy) 
{
	CCoolDialogBar::OnSize(nType, cx, cy);
	
//	int bottom = ( IsHorzDocked()/*IsHorz()*/ || IsFloating()) ? cy - 14 : cy - 18;
	//m_tabctrl.MoveWindow(7, 7, cx - 14, bottom );
	//m_tabctrl.Invalidate( );
	/*
	CWnd *pWnd;
    for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    {
         pWnd=m_views.GetAt(pos)->pWnd;
       pWnd->MoveWindow(9, 9, cx-19, bottom-26);
//	   pWnd->SetWindowPos( &wndTopMost ,9, 9, cx-19, bottom-26, SWP_SHOWWINDOW   );
	   	//	((CFrameWnd *)GetParent())->SetActiveView((CView *)m_pActiveView);

    }
	//RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
    */
	//int bottom = (IsHorz() || IsFloating()) ? cy - 14 : cy - 18;

	//m_tabctrl.MoveWindow(7, 7, cx - 14, bottom);
	
	CWnd *pWnd;
    for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
    {
         pWnd=m_views.GetAt(pos)->pWnd;
       //  pWnd->MoveWindow(9, 9, cx-19, bottom-26);
    }


}

BOOL CSizingTabCtrlBar2::AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass, CCreateContext *pContext)
{	

#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
#endif

	CCreateContext context;
	if (pContext == NULL)
	{
		// if no context specified, generate one from the currently selected
		//  client if possible
		CView* pOldView = NULL;
		if (pOldView != NULL && pOldView->IsKindOf(RUNTIME_CLASS(CView)))
		{
			// set info about last pane
			ASSERT(context.m_pCurrentFrame == NULL);
			context.m_pLastView = pOldView;
			context.m_pCurrentDoc = pOldView->GetDocument();
			if (context.m_pCurrentDoc != NULL)
				context.m_pNewDocTemplate =
				context.m_pCurrentDoc->GetDocTemplate();
		}
		pContext = &context;
	}
	
	CWnd* pWnd;
	TRY
	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE0("Out of memory creating a view.\n");
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL
		
    ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd == NULL);       // not yet created
	
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
	CRect rect;
	// Create with the right size and position
	if (!pWnd->Create(NULL, NULL, dwStyle, rect, this, 0, pContext))
	{
		TRACE0("Warning: couldn't create client pane for view.\n");
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}
	m_pActiveView = (CView*) pWnd;

	TCB_ITEM2 *pMember=new TCB_ITEM2;
	pMember->pWnd=pWnd;
	strcpy(pMember->szLabel, lpszLabel);
	m_views.AddTail(pMember);

	int nViews = m_views.GetCount();
	if (nViews!=1)
	{
		pWnd->EnableWindow(FALSE);
		pWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		((CFrameWnd *)GetParent())->SetActiveView((CView *)m_pActiveView);
	}

	TC_ITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)lpszLabel;
	tci.iImage = nViews-1;
	m_tabctrl.InsertItem(nViews, &tci);

	return TRUE;
}

void CSizingTabCtrlBar2::RemoveView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);

	// remove the page from internal list
	m_views.RemoveAt(m_views.FindIndex(nView));
}

void CSizingTabCtrlBar2::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetActiveView(m_tabctrl.GetCurSel());
}

void CSizingTabCtrlBar2::SetActiveView(int nNewTab)
{
	ASSERT_VALID(this);
	ASSERT(nNewTab >= 0);

	if (nNewTab!=-1 && nNewTab!=m_nActiveTab)
	{
        TCB_ITEM2 *newMember=m_views.GetAt(m_views.FindIndex(nNewTab));
        TCB_ITEM2 *oldMember=NULL;
		
        if (m_nActiveTab!=-1)
        {
            oldMember=m_views.GetAt(m_views.FindIndex(m_nActiveTab));
            oldMember->pWnd->EnableWindow(FALSE);
            oldMember->pWnd->ShowWindow(SW_HIDE);
        }
        newMember->pWnd->EnableWindow(TRUE);
        newMember->pWnd->ShowWindow(SW_SHOW);
        newMember->pWnd->SetFocus();

        m_pActiveView = (CView *)newMember->pWnd;
		((CFrameWnd *)GetParent())->SetActiveView(m_pActiveView);

        m_nActiveTab = nNewTab;
		// select the tab (if tab programmatically changed)
		m_tabctrl.SetCurSel(m_nActiveTab);
    }
}

void CSizingTabCtrlBar2::SetActiveView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));

	int nNewTab = 0;
	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM2 *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
			//first hide old first view
            m_pActiveView->EnableWindow(FALSE);
            m_pActiveView->ShowWindow(SW_HIDE);
			
			// set new active view
			m_pActiveView = (CView*)pMember->pWnd;
			// enable, show, set focus to new view
			m_pActiveView->EnableWindow(TRUE);
			m_pActiveView->ShowWindow(SW_SHOW);
			m_pActiveView->SetFocus();
			
			((CFrameWnd *)GetParent())->SetActiveView(m_pActiveView);

	        m_nActiveTab = nNewTab;
			// select the tab
			m_tabctrl.SetCurSel(m_nActiveTab);

			break;
		}
		nNewTab++;
    }
}

CView* CSizingTabCtrlBar2::GetActiveView()
{
	return m_pActiveView;
}

CView* CSizingTabCtrlBar2::GetView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(nView >= 0);

	if (nView!=-1)
	{
        TCB_ITEM2 *pMember=m_views.GetAt(m_views.FindIndex(nView));
		return (CView*)pMember->pWnd;
	}
	else
		return NULL;
}

CView* CSizingTabCtrlBar2::GetView(CRuntimeClass *pViewClass)
{
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));

	for (POSITION pos=m_views.GetHeadPosition(); pos; m_views.GetNext(pos))
	{
		TCB_ITEM2 *pMember=m_views.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
			return (CView*)pMember->pWnd;
		}
    }
	return NULL;
}
void CSizingTabCtrlBar2::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CCoolDialogBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}
