#pragma once
#include "afxwin.h"


// CDlgMTE 대화 상자입니다.

class CDlgMTE : public CDialog
{
	DECLARE_DYNAMIC(CDlgMTE)

public:
	CDlgMTE(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMTE();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MTE_UNIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg LONG OnInitDialog(UINT, LONG);
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_kComboCase;
	CEdit m_kEdit2ndTexture;
	int m_iSelectCase;

	CString m_strSelectedComboList;
	CString m_str2ndTexture;


	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnBnClickedButton2ndTexture();
	
};
