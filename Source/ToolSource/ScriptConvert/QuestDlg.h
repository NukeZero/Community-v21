#if !defined(AFX_QUESTDLG_H__01C2CE29_5E51_448E_8C08_6BC4C7892E43__INCLUDED_)
#define AFX_QUESTDLG_H__01C2CE29_5E51_448E_8C08_6BC4C7892E43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CClass.h"

class CQuestDlg : public CDialog
{
// Construction
public:
	VEC_PROPQUEST		m_vecSrc;
	VEC_PROPQUEST		m_vecDest;
		
public:
	BOOL Merge( VEC_PROPQUEST& vecSrc, const VEC_PROPQUEST& vecDest );
	BOOL Save( LPCTSTR lpszFileName );
	BOOL Load(LPCTSTR lpszFileName, VEC_PROPQUEST& vec_quest);
	CQuestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuestDlg)
	enum { IDD = IDD_QUEST_DIALOG };
	CComboBox	m_LangKor;
	CComboBox	m_Lang;
	CString	m_SrcPath;
	CString	m_DestPath;
	BOOL	m_bSkip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuestDlg)
	afx_msg void OnQuestButton1();
	afx_msg void OnQuestMstart();
	afx_msg void OnTclientButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTDLG_H__01C2CE29_5E51_448E_8C08_6BC4C7892E43__INCLUDED_)
