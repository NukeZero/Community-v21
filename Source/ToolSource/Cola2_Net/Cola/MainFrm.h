// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__4349B6C4_EC4C_477E_B135_B5F64CD51F0C__INCLUDED_)
#define AFX_MAINFRM_H__4349B6C4_EC4C_477E_B135_B5F64CD51F0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dlg.h"
#include "CoolDialogBar.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
// Operations
public:
	CDlgInfo	m_dlgInfo;
	CDlgTrack	m_dlgTrack;
	CDlgAnimation m_dlgAnimation;		//sun
	CCoolDialogBar	m_wndInfo2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void LoadBone_Auto( TCHAR* szFileName, CModelObject* pModel );

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CCoolDialogBar	m_wndInfo;
	CCoolDialogBar	m_wndTrack;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewInfo(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__4349B6C4_EC4C_477E_B135_B5F64CD51F0C__INCLUDED_)
