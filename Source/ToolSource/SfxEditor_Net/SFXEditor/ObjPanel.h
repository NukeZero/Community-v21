#if !defined(AFX_OBJPANEL_H__8237FC38_C1E2_42E8_AC11_72A3254D7823__INCLUDED_)
#define AFX_OBJPANEL_H__8237FC38_C1E2_42E8_AC11_72A3254D7823__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjPanel dialog

class CObjPanel : public CDialog
{
// Construction
public:
	CObjPanel(CWnd* pParent = NULL);   // standard constructor

	CSFXEditorDoc* m_pDoc;
	CSFXEditorView* m_pView;
// Dialog Data
	//{{AFX_DATA(CObjPanel)
	enum { IDD = IDD_1OBJ };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJPANEL_H__8237FC38_C1E2_42E8_AC11_72A3254D7823__INCLUDED_)
