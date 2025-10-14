// ScriptConvertDlg.h : header file
//

#if !defined(AFX_SCRIPTCONVERTDLG_H__57476516_AA96_46B8_9735_93030386FC0B__INCLUDED_)
#define AFX_SCRIPTCONVERTDLG_H__57476516_AA96_46B8_9735_93030386FC0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct EXCEL_KEY
{
	int m_nOffset;
	CString m_strkey;
} EXCEL_KEY;

typedef struct EXCEL_LINE
{
	int		m_nCodePage;
	CString m_strkey;
	CString m_strstring;

	vector< EXCEL_KEY > m_vecsavekey;
} EXCEL_LINE;


class CScriptConvertDlg : public CDialog
{
public:
	CString IntToStr(int nVal);
	CString LongToStr(long nVal);
	CString ULongToStr(unsigned long nVal);
	CString DblToStr(double dblVal, int ndigits);
	CString DblToStr(float fltVal);
	BOOL GetFieldValue(_RecordsetPtr* pRecordset, int nIndex, CString& strValue, CString strDateFormat);
	void GetExcelDriver();

	CString m_sExcelDriver;

// Construction
public:
	CScriptConvertDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL MakeDirectory(LPCTSTR lpDirName);
	int  GetFileName( const CString& FullPathName, CString* Path, CString* File );
	int  LoadLineString( char* filename, vector<CString>& vecstring );
		
	vector<CString>    m_vecScript;

	vector< vector< EXCEL_LINE > > m_ExcelLine1;
	vector< vector< EXCEL_LINE > > m_ExcelLine2;
	vector< vector< EXCEL_LINE > > m_ExcelLine3;
	
	vector< CString > m_ExcelFile1;
	vector< CString > m_ExcelFile2;
	vector< CString > m_ExcelFile3;
	
	void SaveConvertdataTXT( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList, CString Path = "" );
	void SaveConvertdata( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList );
	CString ResotreKey( EXCEL_LINE excel_line, CString filename );
	void DeleteKey( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList );
	void DeleteKeyThai( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList );
	void ResotreKeyThai( vector< vector< EXCEL_LINE > >& excel_line );
		
	BOOL OnVersionLoad( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList, BOOL bSpaceSkip );
	BOOL OnVersionLoadTxt( vector< vector< EXCEL_LINE > >& excel_line, vector< CString >& vecFileList );
		

// Dialog Data
	//{{AFX_DATA(CScriptConvertDlg)
	enum { IDD = IDD_SCRIPTCONVERT_DIALOG };
	CComboBox	m_ComboControl;
	CListBox	m_ConvertList;
	CListBox	m_TextList1;
	CListBox	m_ScriptList1;
	CString	m_Count1;
	CString	m_Count2;
	CString	m_Directory1;
	CString	m_Directory2;
	CString	m_Directory3;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptConvertDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScriptConvertDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnScriptLoad();
	virtual void OnOK();
	afx_msg void OnSelchangeScriptList1();
	afx_msg void OnTextLoad();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnVersionLoad1();
	afx_msg void OnVersionLoad2();
	afx_msg void OnVersionLoad3();
	afx_msg void OnVersionConvert();
	afx_msg void OnResLoad1();
	afx_msg void OnTextclientButton();
	afx_msg void OnPropquestButton2();
	afx_msg void OnCharacterButton();
	afx_msg void OnHelpButton3();
	afx_msg void OnTreehelpButton2();
	afx_msg void OnFilecompare();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTCONVERTDLG_H__57476516_AA96_46B8_9735_93030386FC0B__INCLUDED_)





















