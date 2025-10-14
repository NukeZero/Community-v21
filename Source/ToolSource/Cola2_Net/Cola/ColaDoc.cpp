// ColaDoc.cpp : implementation of the CColaDoc class
//

#include "stdafx.h"
#include "Cola.h"

#include "ColaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColaDoc

IMPLEMENT_DYNCREATE(CColaDoc, CDocument)

BEGIN_MESSAGE_MAP(CColaDoc, CDocument)
	//{{AFX_MSG_MAP(CColaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColaDoc construction/destruction

CColaDoc::CColaDoc()
{
	// TODO: add one-time construction code here

}

CColaDoc::~CColaDoc()
{
}

BOOL CColaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CColaDoc serialization

void CColaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CColaDoc diagnostics

#ifdef _DEBUG
void CColaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CColaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColaDoc commands
