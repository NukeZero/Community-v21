// MirandaDoc.cpp : implementation of the CMirandaDoc class
//

#include "stdafx.h"
#include "Miranda.h"
#include "MainFrm.h"
#include "MirandaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMirandaDoc

IMPLEMENT_DYNCREATE(CMirandaDoc, CDocument)

BEGIN_MESSAGE_MAP(CMirandaDoc, CDocument)
	//{{AFX_MSG_MAP(CMirandaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMirandaDoc construction/destruction

CMirandaDoc::CMirandaDoc()
{
	// TODO: add one-time construction code here
	m_Terrain = NULL;
 
}

CMirandaDoc::~CMirandaDoc()
{

	if(m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = NULL;
	}


}

BOOL CMirandaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMirandaDoc serialization

void CMirandaDoc::Serialize(CArchive& ar)
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
// CMirandaDoc diagnostics

#ifdef _DEBUG
void CMirandaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMirandaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMirandaDoc commands
int CMirandaDoc::Setup(LPDIRECT3DDEVICE9 Device)
{
	
	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);

	// 지형 관련 
	if(!m_Terrain)
	{
		m_Terrain = new Terrain(Device, 20, 20, 4, 0.0f);
	}

	if(!m_Terrain->loadTexture("World\\Texture\\Default.dds")) 
	{
		if(!m_Terrain->loadTexture("Default.dds")) 
		return 1; // 텍스쳐 로딩 실패 
	}
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 타임 관련 
	g_GameTimer.SetTime(1, 1, 0, 0);

	return 0;
}


BOOL CMirandaDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	// 유효한 확장자인지 체크한다 
	CString FileName((LPCTSTR)lpszPathName);
	FileName = FileName.Right(FileName.GetLength() - FileName.ReverseFind('\\') - 1);
	// 확장자 얻음
	CString	ExtName(FileName);
	ExtName = ExtName.Right(ExtName.GetLength() - ExtName.Find('.') - 1);

	if(ExtName == "scn")
	{
		CString strFilename=_T( LPCTSTR(lpszPathName) );
		CFileIO file;
	

		if(file.Open(strFilename,"rb")) 
		{
			((CMainFrame *)AfxGetMainWnd())->m_CtrlPanel.DelAll();
			int nSfxCount = m_SfxMgr.Load(file);
			((CMainFrame *)AfxGetMainWnd())->m_CtrlPanel.AddSfx(nSfxCount);
			nSfxCount = m_SfxMgr.GetObjCount();
			int nObjCount = m_ObjMgr.Load(file);
			((CMainFrame *)AfxGetMainWnd())->m_CtrlPanel.AddObj(nObjCount);
			// 지형 관련
			m_Terrain->Load(file);
			// 글로벌 타임
			file.Read(&g_GameTimer.m_nHour, sizeof(int));
		}
		file.Close();
	}
	else
	{
		AfxMessageBox("유효하지 않은 파일입니다.");
		return FALSE;
	}
	int nSfxCount = m_SfxMgr.GetObjCount();
	// 순차적으로 파일 오픈
	return FALSE;
}

BOOL CMirandaDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	// 유효한 확장자인지 체크한다 
	CString FileName((LPCTSTR)lpszPathName);
	FileName = FileName.Right(FileName.GetLength() - FileName.ReverseFind('\\') - 1);
	// 확장자 얻음
	CString	ExtName(FileName);
	ExtName = ExtName.Right(ExtName.GetLength() - ExtName.Find('.') - 1);

	if(ExtName != "scn")
	{
		AfxMessageBox("유효하지 않은 파일입니다.");
		return FALSE;
	}
	
	// 순차적으로 파일 저장
	CString strFilename=_T( LPCTSTR(lpszPathName) );
	CFileIO file;
	

	if(file.Open(strFilename,"wb")) 
	{
		// file.Write(&nPart,sizeof(int));
		// 오브젝트, 이펙트 저장
		m_SfxMgr.Save(file);
		m_ObjMgr.Save(file);
		
		// 지형 관련 저장
		m_Terrain->Save(file);
		// 글로벌 타임 저장
		file.Write(&g_GameTimer.m_nHour, sizeof(int));
	}
	else
	{
		AfxMessageBox("파일을 여는데 실패했습니다.");
		return FALSE;
	}
	// 저장끝 
	file.Close();
	return TRUE;
	//return CDocument::OnSaveDocument(lpszPathName);
}
