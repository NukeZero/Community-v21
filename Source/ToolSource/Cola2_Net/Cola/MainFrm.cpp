// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Cola.h"

#include "MainFrm.h"
#include "scanner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScript g_DefineSound;

CMainFrame	*g_pMainFrm = NULL;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(IDM_SELECT, OnUpdateSelect)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_INFO, OnUpdateViewInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	g_pMainFrm = this;
}

CMainFrame::~CMainFrame()
{
}

int		LoadINI( LPCTSTR szFileName )
{
	CScanner	scan;

	if( scan.Load(MakeExePath( "", szFileName )) == FALSE )
	{
		return FAIL;
	}

	while(1)
	{
		scan.GetToken();					// 첫번째 토큰 읽어봄
		if( scan.tok == FINISHED )		break;
		if( _strcmpi(scan.token, "TEXTURE") == 0 )
		{
			scan.GetToken();
			g_strPathTexture = scan.token;
		}
		if( _strcmpi(scan.token, "SCALE") == 0 )
		{
			g_Option.m_fScale = scan.GetFloat();
		}
		if( _strcmpi(scan.token, "AMBIENT") == 0 )
		{
			g_Option.m_nR = scan.GetNumber();
			g_Option.m_nG = scan.GetNumber();
			g_Option.m_nB = scan.GetNumber();
		}
		if( _strcmpi(scan.token, "RESOURCE") == 0 )
		{
			scan.GetToken();
			g_Option.m_strResource = scan.token;
		}
		if( _strcmpi(scan.token, "BGCOLOR") == 0 )
		{
			int r = scan.GetNumber();
			int g = scan.GetNumber();
			int b = scan.GetNumber();
			g_Option.m_bgColor = D3DCOLOR_ARGB( 255, r, g, b );
		}
	}

	return SUCCESS;
}

int		SaveINI( LPCTSTR szFileName )
{
	FILE	*fp = fopen( MakeExePath( "", szFileName), "wt" );
	if( fp == NULL )
	{
		Error( "%s : 저장 실패", szFileName );
		return FAIL;
	}

	fprintf( fp, "*TEXTURE		\"%s\"\n", (LPCTSTR)g_strPathTexture );
	fprintf( fp, "*RESOURCE		\"%s\"\n", (LPCTSTR)g_Option.m_strResource );
	fprintf( fp, "*SCALE		%0.2f		// %로 넣을것.  소수점 가능\n", g_Option.m_fScale );
	fprintf( fp, "*AMBIENT		%d %d %d	// R G B(0~255).\n", g_Option.m_nR, g_Option.m_nG, g_Option.m_nB );
	fprintf( fp, "*BGCOLOR		%d %d %d	// R G B(0~255).\n", (g_Option.m_bgColor >> 16) & 0xff, (g_Option.m_bgColor >> 8) & 0xff, g_Option.m_bgColor & 0xff );
	fprintf( fp, "\n" );

	fclose(fp);

	return SUCCESS;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_BOTTOM);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Info Dialogview

	if (!m_wndInfo.Create(this, &m_dlgInfo, CString("InfoDialogBar"), IDD_INFO, IDM_VIEW_INFO, WS_CHILD | WS_VISIBLE | CBRS_RIGHT ))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

    m_wndInfo.SetBarStyle( m_wndInfo.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndInfo.EnableDocking( CBRS_ALIGN_RIGHT );
	DockControlBar(&m_wndInfo);
	FloatControlBar( &m_wndInfo, CPoint(1070,43) );

	//gmpbigsun : animation dialogview
	if (!m_wndInfo2.Create(this, &m_dlgAnimation, CString("not loaded"), IDD_DIALOG_ANIMATIONLIST, IDM_VIEW_ANIMATIONLIST, WS_CHILD | WS_VISIBLE | CBRS_LEFT ))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

    m_wndInfo2.SetBarStyle( m_wndInfo2.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndInfo2.EnableDocking( CBRS_ALIGN_LEFT );
	DockControlBar(&m_wndInfo2);
	FloatControlBar( &m_wndInfo2, CPoint(4,50) );

	// Trackview Dialog
	if (!m_wndTrack.Create(this, &m_dlgTrack, CString("InfoDialogBar"), IDD_TRACK, IDM_VIEW_TRACK, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM ))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create
	}

    m_wndTrack.SetBarStyle( m_wndTrack.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndTrack.EnableDocking( CBRS_ALIGN_BOTTOM );
	DockControlBar(&m_wndTrack);

	// ini파일 읽음.
	if( LoadINI( "Cola.ini" ) == FAIL )		// ini가 없으면 디폴트로 하나 만든다.
	{
//		if( g_strPathTexture.IsEmpty() )
		// 텍스쳐 폴더 선택.
		{
			char szDisplayName[_MAX_PATH];
			BROWSEINFO bi;
			bi.hwndOwner = m_hWnd;
			bi.lParam = 0;
			bi.lpfn = NULL;
			bi.lpszTitle = "텍스쳐 파일들이 있는 폴더를 지정해주셍.";
			bi.pidlRoot = NULL;
			bi.pszDisplayName = szDisplayName;
			bi.ulFlags = 0;
			ITEMIDLIST *pidList = SHBrowseForFolder( &bi );
			if( pidList )
			{
				SHGetPathFromIDList( pidList, szDisplayName );
				g_strPathTexture = szDisplayName;
				g_strPathTexture += "\\";

			}
		}
		SaveINI( "Cola.ini" );
	}
//	Message( "%s", g_strPathTexture );
	if( g_strPathTexture.IsEmpty() == FALSE )
	{
		if( g_strPathTexture.Right(1) != "\\" )
			g_strPathTexture += "\\";
	}

	if( g_Option.m_strResource.IsEmpty() == FALSE )
	{
		SetCurrentDirectory( g_Option.m_strResource );
	}
	SetResourcePath();

	//	스크립트 읽기
	if( g_Option.m_strResource.IsEmpty() == FALSE )
	{
		CString	strName;
		if( g_Option.m_strResource.IsEmpty() == FALSE )	// 리소스 폴더가 ini에 지정되어있으면..
			strName = g_Option.m_strResource + "\\" + "defineSound.h";
		else
			strName = "defineSound.h";
		
		if( g_DefineSound.Load( strName ) == TRUE )
			g_DefineSound.PreScan();// TRUE );
		else
			Message( "%s를 읽지 못함", strName );

		if( g_Option.m_strResource.IsEmpty() == FALSE )	// 리소스 폴더가 ini에 지정되어있으면..
			strName = g_Option.m_strResource + "\\" + "client\\sound.inc";
		else
			strName = "client\\sound.inc";
		

		if( g_SoundMng.LoadScript( strName ) == FALSE )
			Message( "%s를 읽지 못함", strName );
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~FWS_ADDTOTITLE;	// '제목없음'을 없앰

	cs.x = 0;
	cs.y = 0;
	cs.cx = 1280;
	cs.cy = 1024;
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnUpdateSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( true );
	if ( g_EditMode == EM_SELECT )
		pCmdUI->SetCheck( true );
	else					
		pCmdUI->SetCheck( false );
	
}

void CMainFrame::OnUpdateViewInfo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( TRUE );
}

void CMainFrame::LoadBone_Auto( TCHAR* szFileName, CModelObject* pModel )
{
	if( !pModel )
	{
		AfxMessageBox( "failed loading the bone " );
		return;
	}

	if( !strstrlwr( szFileName, ".o3d" ) )
	{
		AfxMessageBox( "can not load file, it is not o3d file" );
		return;
	}


	//본 정보 Matrix초기화 
	pModel->m_pBone = NULL;
	SAFE_DELETE_ARRAY( pModel->m_mUpdateBone );

	CString strBone;
	if( strstrlwr( szFileName, "part_" ) )
	{
		if( strstrlwr( szFileName, "part_m" ) ) //남성 
			strBone = "mvr_male";
		else strBone = "mvr_female";
	}else
	{
		strBone = GetFileTitle( szFileName );
	}
	
	strBone += ".chr";

//	CString fullPath = GetResourcePath();
//	fullPath += "Model\\";
//	fullPath += strBone;

	//gmpbigsun(100420) : 모델경로를 최초 o3d를 불러온 디렉토리로 설정
	CString fullPath = g_strModelPath + strBone;

	if( FAIL == pModel->LoadBone( fullPath ) )
	{
		if( FAIL == pModel->LoadBone( fullPath ) )
		{
			Error( "%s : bone not loaded.", szFileName );		
			return;
		}
	}

	//TODO : ANI LIST 출력 
	m_wndInfo2.SetWindowText( (LPCTSTR)strBone );
	CFrameWnd* pWnd = m_wndInfo2.GetDockingFrame();		// ControlBar는 이녀석이 외곽 프레임이다.
	if( pWnd )
	{
		pWnd->SetWindowText( (LPCTSTR)strBone );
		pWnd->UpdateData( FALSE );
	}

	m_dlgAnimation.ListUpAllAnimation( szFileName );
}