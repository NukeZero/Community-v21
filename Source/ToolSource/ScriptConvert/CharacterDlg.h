#if !defined(AFX_CHARACTERDLG_H__7E011777_030F_454D_9CE7_DFFE33055D56__INCLUDED_)
#define AFX_CHARACTERDLG_H__7E011777_030F_454D_9CE7_DFFE33055D56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CharacterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCharacterDlg dialog

#include "CClass.h"


class CCharacterDlg : public CDialog
{
// Construction
public:
	VEC_CHARACTER_INC      m_vecSrc;
	VEC_CHARACTER_INC	   m_vecDest;

	BOOL Save( LPCTSTR lpszFileName );
	BOOL Load(LPCTSTR lpszFileName, VEC_CHARACTER_INC& vec_char);
	BOOL Merge(VEC_CHARACTER_INC& vecSrc, const VEC_CHARACTER_INC& vecDest);

	
	CCharacterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCharacterDlg)
	enum { IDD = IDD_CHARACTER_DIALOG };
	CComboBox	m_LangKor;
	CComboBox	m_Lang;
	CString	m_SrcPath;
	CString	m_DestPath;
	BOOL	m_bSkip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharacterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCharacterDlg)
	afx_msg void OnCharButton1();
	afx_msg void OnCharButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnCharMstart();
	afx_msg void OnCheck1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARACTERDLG_H__7E011777_030F_454D_9CE7_DFFE33055D56__INCLUDED_)
