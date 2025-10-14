// SFXEditorDoc.cpp : implementation of the CSFXEditorDoc class
//

#include "stdafx.h"
#include "SFXEditor.h"

#include "SFXEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorDoc

IMPLEMENT_DYNCREATE(CSFXEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSFXEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CSFXEditorDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorDoc construction/destruction

CSFXEditorDoc::CSFXEditorDoc()
{
	m_strTest[0]=0;
}

CSFXEditorDoc::~CSFXEditorDoc()
{
}

BOOL CSFXEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSFXEditorDoc serialization

void CSFXEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		g_SfxMng.GetSfxBase(0)->m_strName=ar.GetFile()->GetFileName();
		g_SfxMng.GetSfxBase(0)->m_strName.MakeLower();
		g_SfxMng.GetSfxBase(0)->m_strName.Replace(".sfx","");
		g_SfxMng.GetSfxBase(0)->Save();
	}
	else
	{
		g_SfxMng.GetSfxBase(0)->m_strName=ar.GetFile()->GetFileName();
		g_SfxMng.GetSfxBase(0)->m_strName.MakeLower();
		g_SfxMng.GetSfxBase(0)->m_strName.Replace(".sfx","");
		g_SfxMng.GetSfxBase(0)->Load();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorDoc diagnostics

#ifdef _DEBUG
void CSFXEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSFXEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSFXEditorDoc commands
