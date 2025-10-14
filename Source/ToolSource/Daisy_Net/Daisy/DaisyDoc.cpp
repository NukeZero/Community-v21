// DaisyDoc.cpp : implementation of the CDaisyDoc class
//

#include "stdafx.h"
#include "Daisy.h"

#include "DaisyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDaisyDoc

IMPLEMENT_DYNCREATE(CDaisyDoc, CDocument)

BEGIN_MESSAGE_MAP(CDaisyDoc, CDocument)
	//{{AFX_MSG_MAP(CDaisyDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDaisyDoc construction/destruction

CDaisyDoc::CDaisyDoc()
{
	// TODO: add one-time construction code here

}

CDaisyDoc::~CDaisyDoc()
{
}

BOOL CDaisyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDaisyDoc serialization

void CDaisyDoc::Serialize(CArchive& ar)
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
// CDaisyDoc diagnostics

#ifdef _DEBUG
void CDaisyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDaisyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDaisyDoc commands

