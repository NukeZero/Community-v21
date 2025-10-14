#if !defined(AFX_DLGOPTION_H__3D38DC06_76E7_4F5A_9F94_A49970837AF0__INCLUDED_)
#define AFX_DLGOPTION_H__3D38DC06_76E7_4F5A_9F94_A49970837AF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOption.h : header file
//
#include "BtnST.h"
#include "Picture.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgOption dialog

class CDlgOption : public CDialog
{
// Construction
public:
	CDlgOption(CWnd* pParent = NULL);   // standard constructor
	
	void			ShowDialog( );

	CFont			m_font;
	CPicture		m_pic;
	int				m_nResWidth;	// 선택한 해상도
	int				m_nResHeight;
	BOOL			m_bStartFullScreen;
	
	int				m_nNormalRCount;
	int				m_nWideRCount;

	CBrush m_brush;
	COLORREF m_clrTransparent;

// Dialog Data
	//{{AFX_DATA(CDlgOption)
	enum { IDD = IDD_OPTION };
	CButtonST 	m_BtnCANCEL;
	CButtonST	m_BtnOK;
	CComboBox	m_cbRes;
	int		m_nTexQual;
	int		m_nViewArea;
	int		m_nObjectDetail;
	int		m_nShadow;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadButton();
	void LoadSkin();

	// Generated message map functions
	//{{AFX_MSG(CDlgOption)
	virtual BOOL OnInitDialog();
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPTION_H__3D38DC06_76E7_4F5A_9F94_A49970837AF0__INCLUDED_)
