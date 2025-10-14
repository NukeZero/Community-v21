// ColaDoc.h : interface of the CColaDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLADOC_H__D0A3EE9F_C6CC_4CD8_BD90_1F63D3B055CB__INCLUDED_)
#define AFX_COLADOC_H__D0A3EE9F_C6CC_4CD8_BD90_1F63D3B055CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CColaDoc : public CDocument
{
protected: // create from serialization only
	CColaDoc();
	DECLARE_DYNCREATE(CColaDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColaDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CColaDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLADOC_H__D0A3EE9F_C6CC_4CD8_BD90_1F63D3B055CB__INCLUDED_)
