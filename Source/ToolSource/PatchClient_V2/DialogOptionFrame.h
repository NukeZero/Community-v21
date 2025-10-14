#pragma once


class CDlgOption;
// CDialogOptionFrame 대화 상자입니다.

class CDialogOptionFrame : public CDialog
{
	DECLARE_DYNAMIC(CDialogOptionFrame)

public:
	CDialogOptionFrame(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogOptionFrame();

	void UpdateView( );
	void SetPath( const CString& strPath )	{ m_strPath = strPath; }
	const CString& GetPath( )				{ return m_strPath; }
	void OK_byChild( );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG2 };

protected:
	BLENDFUNCTION m_blend;
	Gdiplus::Image* m_pImage;
	CDlgOption* m_pDlgOption;
	CString m_strPath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
};
