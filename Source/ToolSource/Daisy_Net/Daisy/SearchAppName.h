#pragma once
#include "afxwin.h"


class CSearchAppName : public CDialog
{
	DECLARE_DYNAMIC(CSearchAppName)

public:
	CSearchAppName(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSearchAppName();
	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtSearch();

private:
	CEdit		 m_Edit_Search;
	CListBox	 m_List_SearchResult;

private:
	void OpenApplet(LPWNDAPPLET lpWndApplet);
	void SearchResultList();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnDblclkListSearchResult();
	afx_msg void OnLbnSelchangeListSearchResult();
	afx_msg void OnEnChangeEditSearch();
protected:
	virtual void OnOK();
};
