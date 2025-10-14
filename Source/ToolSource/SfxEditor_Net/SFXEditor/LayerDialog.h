#if !defined(AFX_LAYERDIALOG_H__8EDA0246_5D8F_4FB4_9631_D1EB46D224BD__INCLUDED_)
#define AFX_LAYERDIALOG_H__8EDA0246_5D8F_4FB4_9631_D1EB46D224BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayerDialog dialog

class CLayerDialog : public CDialog
{
// Construction
public:
	CLayerDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLayerDialog)
	enum { IDD = IDD_NAME_DIALOG };
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLayerDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERDIALOG_H__8EDA0246_5D8F_4FB4_9631_D1EB46D224BD__INCLUDED_)
