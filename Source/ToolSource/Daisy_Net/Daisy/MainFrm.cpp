

#include "stdafx.h"


#include "Daisy.h"
#include "MainFrm.h"
#include "ResourceView.h"

#ifdef __DAISY_UPDATE_1
	#include "DaisyDoc.h"
	#include "DaisyView.h"
	#include "WndSelLanguage.h"
#endif 

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

TCHAR			PROGRAM_NAME[128] = _T( "Daisy" );

CMainFrame*		g_pMainFrame( NULL );
TCHAR			g_szWorkDirectory[MAX_PATH] = { NULL, };

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_WND_BUTTON, OnWndButton)
	ON_UPDATE_COMMAND_UI(ID_WND_BUTTON, OnUpdateWndButton)
	ON_COMMAND(ID_WND_EDIT, OnWndEdit)
	ON_UPDATE_COMMAND_UI(ID_WND_EDIT, OnUpdateWndEdit)
	ON_COMMAND(ID_WND_COMBOBOX, OnWndCombobox)
	ON_UPDATE_COMMAND_UI(ID_WND_COMBOBOX, OnUpdateWndCombobox)
	ON_COMMAND(ID_WND_CHECK, OnWndCheck)
	ON_UPDATE_COMMAND_UI(ID_WND_CHECK, OnUpdateWndCheck)
	ON_COMMAND(ID_WND_RADIO, OnWndRadio)
	ON_UPDATE_COMMAND_UI(ID_WND_RADIO, OnUpdateWndRadio)
	ON_COMMAND(ID_WND_LISTBOX, OnWndListbox)
	ON_UPDATE_COMMAND_UI(ID_WND_LISTBOX, OnUpdateWndListbox)
	ON_COMMAND(ID_WND_LISTCTRL, OnWndListctrl)
	ON_UPDATE_COMMAND_UI(ID_WND_LISTCTRL, OnUpdateWndListctrl)
	ON_COMMAND(ID_WND_TABCTRL, OnWndTabctrl)
	ON_UPDATE_COMMAND_UI(ID_WND_TABCTRL, OnUpdateWndTabctrl)
	ON_COMMAND(ID_WND_STATIC, OnWndStatic)
	ON_UPDATE_COMMAND_UI(ID_WND_STATIC, OnUpdateWndStatic)
	ON_COMMAND(ID_FILE_SAVE_FRAME, OnFileSaveFrame)
	ON_COMMAND(ID_WND_TREECTRL, OnWndTreectrl)
	ON_UPDATE_COMMAND_UI(ID_WND_TREECTRL, OnUpdateWndTreectrl)
	ON_COMMAND(ID_WND_CUSTOM, OnWndCustom)
	ON_UPDATE_COMMAND_UI(ID_WND_CUSTOM, OnUpdateWndCustom)
	ON_COMMAND(ID_WND_TEXT, OnWndText)
	ON_UPDATE_COMMAND_UI(ID_WND_TEXT, OnUpdateWndText)
	ON_COMMAND(ID_WND_GROUPBOX, OnWndGroupbox)
	ON_UPDATE_COMMAND_UI(ID_WND_GROUPBOX, OnUpdateWndGroupbox)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_RECT,   OnUpdateRect)
	ON_COMMAND(ID_WND_PICTURE, OnWndPicture)
	ON_UPDATE_COMMAND_UI(ID_WND_PICTURE, OnUpdateWndPicture)
	ON_COMMAND(ID_LANGUAGE, OnSettingLanguage)
#ifdef __DAISY_UPDATE_1
	ON_COMMAND(ID_ALIGN_LEFT, OnAlignLeft)
	ON_COMMAND(ID_ALIGN_RIGHT, OnAlignRight)
	ON_COMMAND(ID_ALIGN_TOP, OnAlignTop)
	ON_COMMAND(ID_ALIGN_BOTTOM, OnAlignBottom)
	ON_COMMAND(ID_MAKE_SAME_WIDTH, OnMakeSameWidht)
	ON_COMMAND(ID_MAKE_SAME_HEIGHT, OnMakeSameHeight)
	ON_COMMAND(ID_MAKE_SAME_SIZE, OnMakeSameSize)
#endif 

	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_INDICATOR_RECT,
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_SIZE,

};

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CMainFrame::CMainFrame()
:	m_pDaisyView( NULL )
,	m_strSourcePath( "" )
,	m_dwToolSelect( NULL )
,	m_nIdApplet( -1 )
,	m_nIdWndCtrl( -1 )
{
	g_pMainFrame = this;
	::GetCurrentDirectory( MAX_PATH, g_szWorkDirectory );
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{	
	CWndNeuz::FreeTileTexture();
}


//---------------------------------------------------------------------------------------------
// Pre Create Window ( 윈도우 생성하기 전 )
// param	: cs 
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// On Create ( 메인 프레임 생성 될 때 )
// param	: lpCreateStruct 
// return	: int 
//---------------------------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      
	}
	m_DialogToolBar.Create(this);

	if (!m_wndLayoutBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndLayoutBar.LoadToolBar(IDR_LAYOUT_DIALOG))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      
	}
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      
	}

	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_RECT, SBPS_NORMAL, 1000); 	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_SIZE, SBPS_NORMAL, 0);

	// 도킹
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndLayoutBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndLayoutBar);

	m_DialogToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_DialogToolBar,AFX_IDW_DOCKBAR_RIGHT);

	m_wndToolBar.ModifyStyle(0,TBSTYLE_FLAT);
	m_DialogToolBar.ModifyStyle(0,TBSTYLE_FLAT);

	// 컨트롤 바 사이즈
	m_wndSTCBar.Create(this, CSize(300, 100), 1000 );
	m_wndSTCBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndSTCBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndSTCBar, AFX_IDW_DOCKBAR_LEFT);

	m_wndSTCBar.AddView("Resource", RUNTIME_CLASS(CResourceView));

	m_wndSTCBar.EnableToolTips( TRUE );
	m_wndToolBar.EnableToolTips( TRUE );
	m_wndLayoutBar.EnableToolTips( TRUE );
	m_DialogToolBar.EnableToolTips( TRUE );

	SetResourcePath();

	CScript script;
	if( script.Load( "lang.h" ) )
		script.PreScan( FALSE );
	
	CWndBase::m_resMng.Load( RESDATA_INC );
	
	// Applet 아이템 Tree에 넣기
	InsertAppletToTreeView();
	
	return 0;
}


//---------------------------------------------------------------------------------------------
// Insert Applet To TreeView ( 트리에 Applet Window Item 넣기 )
// param	: void 
// return	: bool 
//---------------------------------------------------------------------------------------------
void CMainFrame::InsertAppletToTreeView()
{
	CResourceView* pResourceView = (CResourceView*)m_wndSTCBar.GetView( 0 );
	FLASSERT( pResourceView );

	for( WndAppletIter iter = CWndBase::m_resMng.m_mapWndApplet.begin(); iter != CWndBase::m_resMng.m_mapWndApplet.end(); ++iter )
	{
		DWORD			dwWndId( iter->first );
		LPWNDAPPLET		pWndBase( iter->second );

#if _MSC_VER >= 1400
		FLERROR_ASSERT_LOG_DO( pWndBase, continue, _T("맵에 윈도우 포인터가 비어 있습니다.") );			
#else
		if( NULL == pWndBase )
		{
			FLERROR_LOG( PROGRAM_NAME, _T("맵에 윈도우 포인터가 비어 있습니다.") );
			continue;
		}
#endif
		CString strDef = CScript::GetFindIdStr( _T( "APP_" ), dwWndId );
		pResourceView->AddApplet( strDef, pWndBase );
	}

	pResourceView->SortTreeApplet();
}


//---------------------------------------------------------------------------------------------
// On Update Rect ( 사각 영역 업데이트 될 때 )
// param	: lpCreateStruct 
// return	: int 
//---------------------------------------------------------------------------------------------
void CMainFrame::OnUpdateRect(CCmdUI* pCmdUI) 
{
	if( CWndManager::m_pWndFocusDialog == NULL ||
		CWndManager::m_pWndFocusDialog->m_pWndFocusChild == NULL ) 		
		return;		

	CString string( "" );		
	
	CRect rect = CWndManager::m_pWndFocusDialog ? CWndManager::m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE ) : CRect( 0, 0, 0, 0 );

	if(CWndBase::m_resMng.m_phistorylog != NULL)		
	{
		string.Format(	"Rect[Left:%d][Top:%d][Right:%d][Bottom:%d] Width[%d] Hight[%d] ID[%d] Define[%s] LanguageTest[]",
						CWndBase::m_resMng.m_phistorylog->rect.left,
						CWndBase::m_resMng.m_phistorylog->rect.top,
						CWndBase::m_resMng.m_phistorylog->rect.right,	
						CWndBase::m_resMng.m_phistorylog->rect.bottom,
						CWndBase::m_resMng.m_phistorylog->rect.Width(),
						CWndBase::m_resMng.m_phistorylog->rect.Height(),
						CWndBase::m_resMng.m_phistorylog->dwWndId,
						CWndBase::m_resMng.m_phistorylog->strDefine
					  );
	}

	FLERROR_ASSERT_LOG_RETURN_VOID( pCmdUI, _T("UI 포인터가 존재하지 않습니다.") );

	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(string);
}


//---------------------------------------------------------------------------------------------
// Push Window ID  ( 스택에 윈도우 아이디를 추가 )
// param	: dwWndID 
// return	: void 
//---------------------------------------------------------------------------------------------
void CMainFrame::PushWndId( DWORD dwWndId )
{
	m_wndIdStack.Push( dwWndId );
}


//---------------------------------------------------------------------------------------------
// Pop Window ID  ( 스택에서 아이디를 팝 )
// param	: void 
// return	: void 
//---------------------------------------------------------------------------------------------
DWORD CMainFrame::PopWndId()
{
	if( FALSE == m_wndIdStack.IsEmpty() )
		return m_wndIdStack.Pop();

	FLASSERT( NULL );
	return m_nIdApplet;
}


//---------------------------------------------------------------------------------------------
// CMainFrame message handlers
//---------------------------------------------------------------------------------------------

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	CMDIFrameWnd::RecalcLayout(bNotify);
	CMDIFrameWnd::RecalcLayout(bNotify);
}


void CMainFrame::OnWndButton() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_BUTTON;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndEdit() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_EDIT;	
	else	
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndCheck() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_CHECK;
	else
		m_dwToolSelect = 0;	
} 


void CMainFrame::OnWndRadio() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_RADIO;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndCombobox() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_COMBOBOX;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndListbox() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_LISTBOX;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndListctrl() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_LISTCTRL;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndTabctrl() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_TABCTRL;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndText() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_TEXT;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndPicture() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_PICTURE;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndTreectrl() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_TREECTRL;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndStatic() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_STATIC;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndCustom() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_CUSTOM;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnWndGroupbox() 
{
	if( NULL == m_dwToolSelect )
		m_dwToolSelect = ID_WND_GROUPBOX;
	else
		m_dwToolSelect = 0;	
}


void CMainFrame::OnUpdateWndButton(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_BUTTON );
}


void CMainFrame::OnUpdateWndEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_EDIT );
}


void CMainFrame::OnUpdateWndCheck(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_CHECK );	
}


void CMainFrame::OnUpdateWndRadio(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_RADIO );	
}


void CMainFrame::OnUpdateWndCombobox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_COMBOBOX );	
}


void CMainFrame::OnUpdateWndListbox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_LISTBOX );
}


void CMainFrame::OnUpdateWndListctrl(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_LISTCTRL );		
}


void CMainFrame::OnUpdateWndTabctrl(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_TABCTRL );		
}


void CMainFrame::OnUpdateWndText(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_TEXT );		
}


void CMainFrame::OnUpdateWndPicture(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_PICTURE );		
}


void CMainFrame::OnUpdateWndTreectrl(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_TREECTRL );		
}


void CMainFrame::OnUpdateWndStatic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_STATIC );	
}


void CMainFrame::OnUpdateWndGroupbox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_GROUPBOX );	
}


void CMainFrame::OnUpdateWndCustom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_dwToolSelect == ID_WND_CUSTOM );
}


void CMainFrame::OnSettingLanguage()
{
	CWndSelLanguage SelLang;
	SelLang.DoModal();
}


void CMainFrame::OnFileSaveFrame() 
{
	m_wndStatusBar.SetPaneText( 0, _T("[데이지] 파일 저장을 시도합니다.") );
	
	if( CWndBase::m_resMng.Save( MakePath( "", RESDATA_INC ) ) == FALSE )
		AfxMessageBox( _T( "파일을 저장할 수 없습니다." ), MB_OK, IDABORT );

	m_wndStatusBar.SetPaneText( 0, _T("[데이지] 파일을 성공적으로 저장 하였습니다.") );
}


#ifdef __DAISY_UPDATE_1

	void CMainFrame::OnAlignLeft()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.AlignClipWndLeft();
	}

	void CMainFrame::OnAlignRight()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.AlignClipWndRight();	
	}

	void CMainFrame::OnAlignTop()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.AlignClipWndTop();	
	}

	void CMainFrame::OnAlignBottom()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.AlignClipWndBottom();	
	}

	void CMainFrame::OnMakeSameWidht()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.MakeClipWndSameWidht();	
	}

	void CMainFrame::OnMakeSameHeight()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.MakeClipWndSameHeight();		
	}

	void CMainFrame::OnMakeSameSize()
	{
		if(m_pDaisyView != NULL)
			m_pDaisyView->m_WndMng.MakeClipWndSameSize();	
	}

#endif 


#ifdef _DEBUG

	void CMainFrame::AssertValid() const
	{
		CMDIFrameWnd::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CMDIFrameWnd::Dump(dc);
	}

#endif 

	void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
	{
		if(nID == SC_RESTORE)
		{
			m_DialogToolBar.Invalidate( FALSE );
			m_wndToolBar.Invalidate( FALSE );
			m_wndLayoutBar.Invalidate( FALSE );
			m_wndStatusBar.Invalidate( FALSE );
			m_wndSTCBar.Invalidate( FALSE );

			CTabCtrl* pTabCtrl = m_wndSTCBar.GetTabControl();
			if( pTabCtrl )
			{
				RECT lpRect;
				GetClientRect(&lpRect);
				pTabCtrl->AdjustRect(FALSE, &lpRect);
				pTabCtrl->MoveWindow(&lpRect, TRUE);   
			}

		}

		CMDIFrameWnd::OnSysCommand(nID, lParam);
	}
