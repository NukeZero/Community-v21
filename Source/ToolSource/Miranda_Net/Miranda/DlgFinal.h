#if !defined(AFX_DLGFINAL_H__F41E043A_A184_4F15_848A_871C6F51B501__INCLUDED_)
#define AFX_DLGFINAL_H__F41E043A_A184_4F15_848A_871C6F51B501__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFinal.h : header file
//

#define		OBJSTAF_COMBAT				0x00000001	// 전투 모드

struct FinalDataSet
{
	int _iCase;
	char _szSfx[ 64 ];
	char _szBone[ 64 ];
	BOOL _bLoop;
};

typedef std::map< int, FinalDataSet* >	FDSContainer;
typedef FDSContainer::iterator			FDSIter;

/////////////////////////////////////////////////////////////////////////////
// CDlgFinal dialog

class CDlgFinal : public CDialog
{
// Construction
public:
	enum EFF_KIND 
	{
		EK_NONE,
		EK_LINKSFX,
		EK_MULTITEXTURING,
	};

	CDlgFinal(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFinal)
	enum { IDD = IDD_DIALOG_FINAL };
	CListCtrl	m_ctrlData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFinal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddData( EFF_KIND eKind, CString& strCase, CString& strLinkSfx, CString& strBone, CString& strLoop );
	void SubData( const int nItem );
	BOOL SaveData( CString& filename );
	BOOL ReadData( const char* filename );

	FDSContainer _cDatas;

	EFF_KIND StringToEK( const CString& str );
	CString EKToString( EFF_KIND eKind );

protected:

	void WriteOverlapedFunc( HANDLE hFile, const char* szCase, const int nMax  );

	// Generated message map functions
	//{{AFX_MSG(CDlgFinal)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDelete();
	afx_msg void OnItemchangedListFinal(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFINAL_H__F41E043A_A184_4F15_848A_871C6F51B501__INCLUDED_)
