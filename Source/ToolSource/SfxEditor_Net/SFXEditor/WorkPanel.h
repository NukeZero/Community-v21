#if !defined(AFX_WORKPANEL_H__4C85988F_14B7_4B53_9631_75C6AE1D7B40__INCLUDED_)
#define AFX_WORKPANEL_H__4C85988F_14B7_4B53_9631_75C6AE1D7B40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkPanel.h : header file
//

#include"objpanel.h"
#include"texpanel.h"

/////////////////////////////////////////////////////////////////////////////
// CWorkPanel dialog

class CWorkPanel : public CDialogBar
{
// Construction
public:
	CWorkPanel(CWnd* pParent = NULL);   // standard constructor

	CSFXEditorDoc* m_pDoc;
	CSFXEditorView* m_pView;

	void Init(void);

	CObjPanel m_ObjPanel;
	CTexPanel m_TexPanel;

// Dialog Data
	//{{AFX_DATA(CWorkPanel)
	enum { IDD = IDD_WORKPANEL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKPANEL_H__4C85988F_14B7_4B53_9631_75C6AE1D7B40__INCLUDED_)
