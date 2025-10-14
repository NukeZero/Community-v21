#pragma once
#include "afxwin.h"


// CDlgCalcWorldmap 대화 상자입니다.

class CDlgCalcWorldmap : public CDialog
{
	DECLARE_DYNAMIC(CDlgCalcWorldmap)

public:
	CDlgCalcWorldmap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCalcWorldmap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CALC_WORLDMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	int m_editBitmapX1;
	int m_editBitmapX2;
	int m_editBitmapY1;
	int m_editBitmapY2;
	int m_editWorldmapX1;
	int m_editWorldmapX2;
	int m_editWorldmapY1;
	int m_editWorldmapY2;
	CListBox m_listResult;
	CListBox m_listHistory;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
};
