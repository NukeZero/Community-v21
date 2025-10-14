// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FE74F456_7C48_4FE7_A7AC_FE99952940F8__INCLUDED_)
#define AFX_MAINFRM_H__FE74F456_7C48_4FE7_A7AC_FE99952940F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "WorkSpace.h"
//#include "SizingTabCtrlBar.h"
#include "SizecBar2.h"
//#include "PreviewBar.h"
#include "TerrainEdit.h"
#include "DlgNavigator.h"
#include "DlgNavigatorMapView.h"

#ifdef __Y_PATROL
#include "DlgPatrol.h"
#endif //__Y_PATROL

#include "DlgObejctTextureList.h"

class CObj;
class CModelView;
#ifdef __CSC_WORLDOBJ_LIST
class CWorldTreeView;
#endif //__CSC_WORLDOBJ_LIST
class CMainFrame : public CMDIFrameWnd
	{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	CTerrainSheet        m_terrainSheet;
	CPageTerrainHeight   m_pageTerrainHeight;
	CPageTerrainColor    m_pageTerrainColor; 
	CPageTerrainTexture  m_pageTerrainTexture;
	CPageTerrainWater    m_pageTerrainWater; 
	CPageTerrainLayer    m_pageTerrainLayer; 

	CDlgNavigator m_dlgNavigator;
	CDlgNavigatorMapView m_dlgNavigatorMapView;
#ifdef __Y_PATROL
	CDlgPatrol m_dlgPatrol;
#endif //__Y_PATROL

	DlgObejctTextureList m_dlg_Object_Texture_List;
	
// Attributes
public:
	int m_nTerrainTool;
	int m_nSelect;
	CSizingTabCtrlBar2 m_wndWorkspaceBar;
	//CPreviewBar m_wndPreviewBar;
	//CWorkSpace   m_workSpace;
	CView* GetActiveView();
	CModelView* m_pObjView;
#ifdef __CSC_WORLDOBJ_LIST
	CWorldTreeView* m_pWorldObjList;
#endif //__CSC_WORLDOBJ_LIST

#ifdef __CSC_TERRAIN_OPACITY
	int m_nTerrainTexOpacity;
#endif //__CSC_TERRAIN_OPACITY

	void OpenWorkSpace();
	void CloseWorkSpace();
	void ShowWorkSpace( BOOL bShow );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_wndTerrainBar;
	CToolBar    m_wndViewBar;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnViewWorkspace();
	afx_msg void OnUpdateViewWorkspace(CCmdUI* pCmdUI);
	afx_msg void OnViewPreviewer();
	afx_msg void OnUpdateViewPreviewer(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveProject();
	afx_msg void OnUpdateFileSaveProject(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnViewNavigator();
		afx_msg void OnUpdateViewNavigator(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__FE74F456_7C48_4FE7_A7AC_FE99952940F8__INCLUDED_)
