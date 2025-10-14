#pragma once

class CBetaPatchClientDlg;

// CDialogFrame 대화 상자입니다.

class CDialogFrame : public CDialog
{
	DECLARE_DYNAMIC(CDialogFrame)

public:
	CDialogFrame(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogFrame();

	void UpdateView( );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

public:
	CBetaPatchClientDlg* m_pDlgPatch;

protected:
	HICON m_hIcon;
	BLENDFUNCTION m_blend;
	Gdiplus::Image* m_pImage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
};
