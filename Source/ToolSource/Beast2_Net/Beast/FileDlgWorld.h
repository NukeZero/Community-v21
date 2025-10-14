#if !defined(AFX_FILEDLGWORLD_H__CFD05551_2B3C_40FA_AE04_52D16363BCD8__INCLUDED_)
#define AFX_FILEDLGWORLD_H__CFD05551_2B3C_40FA_AE04_52D16363BCD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDlgWorld.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileDlgWorld dialog

class CFileDlgWorld : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDlgWorld)
	CButton m_check;

public:
	BOOL m_bCheck;
	CFileDlgWorld(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	virtual BOOL OnFileNameOK( );

	//{{AFX_MSG(CFileDlgWorld)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDLGWORLD_H__CFD05551_2B3C_40FA_AE04_52D16363BCD8__INCLUDED_)
