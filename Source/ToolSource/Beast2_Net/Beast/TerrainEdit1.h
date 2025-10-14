#if !defined(AFX_TERRAINEDIT1_H__06E43138_447C_4E98_9B51_EA7F58BEBE5D__INCLUDED_)
#define AFX_TERRAINEDIT1_H__06E43138_447C_4E98_9B51_EA7F58BEBE5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainEdit1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TerrainEdit dialog

class TerrainEdit : public CPropertyPage
{
	DECLARE_DYNCREATE(TerrainEdit)

// Construction
public:
	TerrainEdit();
	~TerrainEdit();

// Dialog Data
	//{{AFX_DATA(TerrainEdit)
	enum { IDD = IDD_TERRAIN_COLOR_EDIT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TerrainEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(TerrainEdit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINEDIT1_H__06E43138_447C_4E98_9B51_EA7F58BEBE5D__INCLUDED_)
