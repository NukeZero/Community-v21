// DaisyDoc.h : interface of the CDaisyDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DAISYDOC_H__D2B1B810_C72E_467D_9BB2_0679728013F0__INCLUDED_)
#define AFX_DAISYDOC_H__D2B1B810_C72E_467D_9BB2_0679728013F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDaisyDoc : public CDocument
{
protected: // create from serialization only
	CDaisyDoc();
	DECLARE_DYNCREATE(CDaisyDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDaisyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDaisyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDaisyDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAISYDOC_H__D2B1B810_C72E_467D_9BB2_0679728013F0__INCLUDED_)
