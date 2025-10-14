// WorldDoc.h : interface of the CWorldDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDDOC_H__E1C9F9C7_E42C_41B5_BCED_E236E64F2C45__INCLUDED_)
#define AFX_WORLDDOC_H__E1C9F9C7_E42C_41B5_BCED_E236E64F2C45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWorldDoc : public CDocument
{
protected: // create from serialization only
	CWorldDoc();
	DECLARE_DYNCREATE(CWorldDoc)

// Attributes
public:
	void ReadWorld();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL
	void OnActivate(UINT state, CWnd* other, BOOL bMinimized);
// Implementation
public:
	virtual ~CWorldDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWorldDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDDOC_H__E1C9F9C7_E42C_41B5_BCED_E236E64F2C45__INCLUDED_)
