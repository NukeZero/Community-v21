#if !defined(AFX_TEXTCLIENTDLG_H__868FF275_CF4D_4F1F_8D12_EF2F849366B5__INCLUDED_)
#define AFX_TEXTCLIENTDLG_H__868FF275_CF4D_4F1F_8D12_EF2F849366B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct TEXTCLIENT
{
	CString m_strDefine;
	CString m_strColor;
	
	wchar_t awszMessage [ LANG_MAX ][ 1024 ];
} TEXTCLIENT, *LPTEXTCLIENT;

typedef vector< TEXTCLIENT > 		VEC_TEXTCLIENT;

class CTextClientDlg : public CDialog
{
public:
	int					m_nLangKor;
	int					m_nLang;

	VEC_TEXTCLIENT		m_vecTextSrc;
	VEC_TEXTCLIENT		m_vecTextDest;

	BOOL LoadTextClient( LPCTSTR lpszFileName, VEC_TEXTCLIENT&	vec_textclient );
	BOOL Save( LPCTSTR lpszFileName );



	CTextClientDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextClientDlg)
	enum { IDD = IDD_TEXTCLIENT_DIALOG };
	CComboBox	m_SelLangKor;
	CComboBox	m_SelLang;
	CString	m_SrcPath;
	CString	m_DestPath;
	BOOL	m_bSkip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextClientDlg)
	afx_msg void OnTclientButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnTclientMstart();
	afx_msg void OnTclientButton2();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnCheck1();
	afx_msg void OnSelchangeCombo2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
