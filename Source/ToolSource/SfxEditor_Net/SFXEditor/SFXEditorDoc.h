// SFXEditorDoc.h : interface of the CSFXEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SFXEDITORDOC_H__CCD5CB9C_52AE_449A_9BDD_487458318625__INCLUDED_)
#define AFX_SFXEDITORDOC_H__CCD5CB9C_52AE_449A_9BDD_487458318625__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSFXEditorDoc : public CDocument
{
protected: // create from serialization only
	CSFXEditorDoc();
	DECLARE_DYNCREATE(CSFXEditorDoc)

// Attributes
public:
	char m_strTest[80];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSFXEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSFXEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSFXEditorDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SFXEDITORDOC_H__CCD5CB9C_52AE_449A_9BDD_487458318625__INCLUDED_)
