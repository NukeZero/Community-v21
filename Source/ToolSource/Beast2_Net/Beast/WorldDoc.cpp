// WorldDoc.cpp : implementation of the CWorldDoc class
//

#include "stdafx.h"
#include "Beast.h"

#include "WorldDoc.h"
#include "WorldView.h"
#include "DlgNewWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldDoc

IMPLEMENT_DYNCREATE(CWorldDoc, CDocument)

BEGIN_MESSAGE_MAP(CWorldDoc, CDocument)
	//{{AFX_MSG_MAP(CWorldDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	//ON_MESSAGE(&CWorldDoc::OnActivate)
	//ON_WM_ACTIVATE(CWorldDoc, OnActivate)
	//
	//ON_MESSAGE(WM_ACTIVATE, &CWorldDoc::OnActivate)
	//ON_WM_ACTIVATE()

END_MESSAGE_MAP()

//void CWorldDoc::OnActivate(UINT state, CWnd* other, BOOL bMinimized)
//{
//	ShowWindow(SW_SHOWNORMAL);
//	UpdateWindow();
//}

/////////////////////////////////////////////////////////////////////////////
// CWorldDoc construction/destruction

CWorldDoc::CWorldDoc()
{
	// TODO: add one-time construction code here

}

CWorldDoc::~CWorldDoc()
{
}

BOOL CWorldDoc::OnNewDocument()
{
	if( CWorldView::m_nOpenViewCnt > 1 )
	{
		AfxMessageBox( _T( "WorldView is already Open." ) );
		return FALSE;
	}
	if (!CDocument::OnNewDocument())
		return FALSE;

	CDlgNewWorld dlgNewWorld;
	if( dlgNewWorld.DoModal() == IDOK )
	{
		// TODO: add reinitialization code here
		// (SDI documents will reuse this document)

		//gmpbigsun: set MPU if indoor
		if( dlgNewWorld.m_bIndoor )
			g_MPU = dlgNewWorld.m_iMPU;

		POSITION pos = GetFirstViewPosition();
		while (pos != NULL)
		{
			CWorldView* pView = (CWorldView*)GetNextView(pos);
			pView->CreateDevice();
			if( dlgNewWorld.m_strHeightBitmap.IsEmpty() )
				pView->m_world.NewWorld( dlgNewWorld.m_nUnitWidth, dlgNewWorld.m_nUnitHeight, dlgNewWorld.m_nSelectTexture, dlgNewWorld.m_fLandHeight );
			else
			if( pView->m_world.NewWorld( dlgNewWorld.m_strHeightBitmap, dlgNewWorld.m_nSelectTexture, dlgNewWorld.m_fMutipleHeight ) == FALSE )
			{
				AfxMessageBox( _T( "Height Bitmap 파일을 오픈하지 못했거나 Bitmap의 사이즈가 128의 배수가 아닙니다." ) );
				return FALSE;
			}
			pView->InvalidateDeviceObjects();
			pView->RestoreDeviceObjects( );// pView->m_pd3dDevice );

			//Init continent window 
			g_kDlgContinent.Init( &pView->m_world );

		}   

		//MPU window 갱신 
		g_pMainFrame->m_pageTerrainHeight.m_iMPU = g_MPU;
		g_pMainFrame->m_pageTerrainHeight.UpdateData( FALSE );


		return TRUE;
	}
	return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CWorldDoc serialization

void CWorldDoc::Serialize(CArchive& ar)
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
void CWorldDoc::ReadWorld()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CWorldView* pView = (CWorldView*)GetNextView(pos);
		pView->m_world.ReadWorld();
	}   
}
BOOL CWorldDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if( CWorldView::m_nOpenViewCnt > 1 )
	{
		AfxMessageBox( _T( "WorldView already Open : CWorldDoc::OnOpenDocument." ) );
		return FALSE;
	}
	if (IsModified())
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CWorldView* pView = (CWorldView*)GetNextView(pos);
		pView->CreateDevice();
		pView->OpenWorld( lpszPathName );
	}   
	SetTitle( lpszPathName );
/*
	CFileException fe;
	CFile* pFile = GetFile(lpszPathName,
		CFile::modeRead|CFile::shareDenyWrite, &fe);
	if (pFile == NULL)
	{
		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	DeleteContents();
	SetModifiedFlag();  // dirty during de-serialize

	CArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
	loadArchive.m_pDocument = this;
	loadArchive.m_bForceFlat = FALSE;
	TRY
	{
		CWaitCursor wait;
		if (pFile->GetLength() != 0)
			Serialize(loadArchive);     // load me
		loadArchive.Close();
		ReleaseFile(pFile, FALSE);
	}
	CATCH_ALL(e)
	{
		ReleaseFile(pFile, TRUE);
		DeleteContents();   // remove failed contents

		TRY
		{
			ReportSaveLoadException(lpszPathName, e,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		}
		END_TRY
		//DELETE_EXCEPTION(e);
		return FALSE;
	}
	END_CATCH_ALL
*/
	SetModifiedFlag(FALSE);     // start off with unmodified

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CWorldDoc diagnostics

#ifdef _DEBUG
void CWorldDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWorldDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldDoc commands
