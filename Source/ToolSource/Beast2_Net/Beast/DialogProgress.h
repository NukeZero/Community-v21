#pragma once
#include "afxcmn.h"


// CDialogProgress 대화 상자입니다.

class CDialogProgress : public CDialog
{
	DECLARE_DYNAMIC(CDialogProgress)

public:
	CDialogProgress(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogProgress();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_kProgress;
};
