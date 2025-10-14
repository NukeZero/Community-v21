// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__56FDF542_24A9_4220_931D_B72F1F4F7964__INCLUDED_)
#define AFX_MAINFRM_H__56FDF542_24A9_4220_931D_B72F1F4F7964__INCLUDED_

#include "CtrlPanel.h"
#include "FramePanel.h"
#include "AniDialogBar.h"
#include "DlgBoneList.h"
#include "DlgEditUnit.h"
#include "DlgFinal.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	DWORD		   m_dwColor;
	D3DXVECTOR3    m_vRot;

// Operations
public:

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

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	

public:
	CCtrlPanel	m_CtrlPanel;
	CFramePanel m_FramePanel;

	CAniDialogBar m_kAniPanel;		//sun!!
	CDlgBoneList m_kBoneList;
	CDlgEditUnit m_kEditUnit;
	CDlgFinal m_kFinal;

	void SwitchVisibility(int nID);
	void ShowFrame(int nIsShow);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonIconWireframe();
	afx_msg void OnUpdateWireframe( CCmdUI* pCmdUI );
	afx_msg void OnButtonIconRenderbone();
	afx_msg void OnUpdateRenderbone( CCmdUI* pCmdUI );
	afx_msg void OnButtonIconMultiMode();
	afx_msg void OnUpdateMultiMode( CCmdUI* pCmdUI );

	afx_msg void OnButtonIconFinal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__56FDF542_24A9_4220_931D_B72F1F4F7964__INCLUDED_)
