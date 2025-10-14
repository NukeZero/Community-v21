#if !defined(AFX_DLGINPUTANGLE_H__A5A338EE_6799_4DD5_B67B_7B7B23C563C8__INCLUDED_)
#define AFX_DLGINPUTANGLE_H__A5A338EE_6799_4DD5_B67B_7B7B23C563C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputAngle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInputAngle dialog

class CDlgInputAngle : public CDialog
{
// Construction
public:
	CDlgInputAngle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputAngle)
	enum { IDD = IDD_INPUT_ANGLE };
	float	m_fAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputAngle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputAngle)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDlgInputPosition dialog

class CDlgInputPosition : public CDialog
{
// Construction
public:
	CDlgInputPosition(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputPosition)
	enum { IDD = IDD_INPUT_POSITION };
	float	m_fPositionX;
	float	m_fPositionY;
	float	m_fPositionZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputPosition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputPosition)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// CDlgInputScale 대화 상자입니다.

class CDlgInputScale : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputScale)

public:
	CDlgInputScale(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInputScale();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INPUT_SCALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	float m_fEditScale;
	float m_fEditOldScale;
};

#endif // !defined(AFX_DLGINPUTANGLE_H__A5A338EE_6799_4DD5_B67B_7B7B23C563C8__INCLUDED_)
