#if !defined(AFX_RESDATACONVERT_H__2AF74F05_C68B_4B07_A117_DDC6702729C2__INCLUDED_)
#define AFX_RESDATACONVERT_H__2AF74F05_C68B_4B07_A117_DDC6702729C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResdataConvert.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CResdataConvert dialog
#include "resmanager.h"

class CResdataConvert : public CDialog
{
// Construction
public:
	int			m_nLang;
	int			m_nLangKor;
	
	LPWNDCTRL IsSameDefine( LPWNDAPPLET lpWndApplet, LPCTSTR lpszDefine );
	LPWNDAPPLET IsSameDefine( CResManager* pWndApplet, LPWNDAPPLET lpWndApplet );
	CResManager m_ressrc;
	CResManager m_resdest;
	
	BOOL ConvertStart( CResManager* pSrcWndApplet, CResManager* pDestWndApplet );

	CResdataConvert::CResdataConvert(CWnd* pParent /*=NULL*/);
		// Dialog Data
	//{{AFX_DATA(CResdataConvert)
	enum { IDD = IDD_RESDATA_DIALOG };
	CComboBox	m_ChangeLangKor;
	CComboBox	m_ChangeLang;
	CString	m_DestPath;
	CString	m_SrcPath;
	BOOL	m_bSkip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResdataConvert)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResdataConvert)
	afx_msg void OnResButton1();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnResButton3();
	afx_msg void OnResButton2();
	afx_msg void OnCheck1();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESDATACONVERT_H__2AF74F05_C68B_4B07_A117_DDC6702729C2__INCLUDED_)
