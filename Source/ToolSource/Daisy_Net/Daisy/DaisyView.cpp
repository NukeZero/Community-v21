
#include "stdafx.h"

#include "Daisy.h"
#include "MainFrm.h"
#include "DaisyDoc.h"
#include "DaisyView.h"
#include "ResourceView.h"
#include "DlgNewClass.h"
#include "DlgGuidesSetting.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDaisyView

IMPLEMENT_DYNCREATE(CDaisyView, CView)

BEGIN_MESSAGE_MAP(CDaisyView, CView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_COMMAND(ID_FILE_NEWAPP, OnFileNewapp)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND_EX(ID_POPUP_VIEW_OPEN, OnPopupCommand)
	ON_COMMAND(ID_INSERT_NEW_CLASS, OnInsertNewClass)
	ON_UPDATE_COMMAND_UI(ID_INSERT_NEW_CLASS, OnUpdateInsertNewClass)
	ON_COMMAND(ID_LAYOUT_GUIDE_SETTING, OnLayoutGuideSetting)
	ON_COMMAND(ID_LAYOUT_CLING, OnLayoutCling)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_CLING, OnUpdateLayoutCling)
	ON_COMMAND_EX(ID_POPUP_VIEW_REMOVE, OnPopupCommand)
	ON_COMMAND_EX(ID_POPUP_VIEW_PROPERTIES, OnPopupCommand)
	ON_COMMAND(ID_LAYOUT_SHOWGRID, OnLayoutShowgrid)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_SHOWGRID, OnUpdateLayoutShowgrid)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CDaisyView::CDaisyView()
:	m_dwWndId( 100 )
,	m_hwndRenderWindow( NULL )
,	m_hwndRenderFullScreen( NULL )
,	m_pProperties( NULL )

{
	g_pMainFrame->m_pDaisyView			= this;
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;

#ifdef __DAISY_UPDATE_1
	m_pTempWnd.SetWndId(-1);
#endif 
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CDaisyView::~CDaisyView()
{
}


//---------------------------------------------------------------------------------------------
// Create ( 생성 )
// param	: ...
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CDaisyView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CWnd::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext );
}


//---------------------------------------------------------------------------------------------
// On Intialize Update ( 초기화 )
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	CreateDevice();	

	//RenderHelper_DragIcon::Instance()->CreateRenderHelper( m_pd3dDevice );
}


//---------------------------------------------------------------------------------------------
// Pre Create Window ( 윈도우 생성하기 전 )
// param	: cs 
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CDaisyView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.x	= 0;
	cs.y	= 0;
	cs.cx	= 100;
	cs.cy	= 100;

	return CView::PreCreateWindow(cs);
}


//---------------------------------------------------------------------------------------------
// Window Procedure ( 윈도우 프로시져 ) 
// param	: ... 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
LRESULT CDaisyView::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	if( CWndEdit::GetWndEditCtrl( ) )
	{
		if( CWndEdit::GetWndEditCtrl()->IsYouMessage( message, wParam, lParam ) )	
			return 0;

		if( message == WM_KEYDOWN || message == WM_KEYUP )
			return 0;
	}

	m_WndMng.WindowRootProc( message, wParam, lParam );

	LRESULT result = CView::WindowProc( message, wParam, lParam );
	return result;
}


//---------------------------------------------------------------------------------------------
// Check For Lost Full Screen 
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::CheckForLostFullscreen()
{
    HRESULT hRes( S_OK );

    if( m_bWindowed )
        return hRes;

    if( FAILED( hRes = m_pd3dDevice->TestCooperativeLevel() ) )
	{
		ForceWindowed();
		return S_FALSE;
	}

    return hRes;
}


//---------------------------------------------------------------------------------------------
// On Draw ( 그리기 )
// param	: pDC ( Device Context Pointer ) 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnDraw(CDC* pDC)
{
	CDaisyDoc* pDoc = GetDocument();
	FLERROR_ASSERT_LOG_RETURN_VOID( pDoc, _T("도큐먼트 포인터를 얻어올 수 없습니다.") );			

	if( S_OK == CheckForLostFullscreen() )
		RenderScene();
}


//---------------------------------------------------------------------------------------------
// Create Device ( 장치 오브젝트 생성 )
// param	: void 
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CDaisyView::CreateDevice() 
{
	m_hwndRenderWindow = CView::GetSafeHwnd();

	CD3DApplication::m_hWnd			= m_hwndRenderWindow;
	CD3DApplication::m_hWndFocus	= NULL;
	CD3DApplication::Create( AfxGetInstanceHandle() );

	DXUtil_Timer( TIMER_RESET );
	SetTimer( 1, 1, NULL );

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// Init Device Objects ( 장치 오브젝트들 초기화 )
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::InitDeviceObjects()
{
	CWndBase::m_Theme.LoadTheme( m_pd3dDevice, _T("Default.thm") );
	CWndBase::m_Theme.InitDeviceObjects( m_pd3dDevice );

	m_WndMng.m_pApp		= this;
	m_WndMng.m_pTheme	= &CWndBase::m_Theme;
	m_WndMng.m_pFont	= CWndBase::m_Theme.m_pFontText;
	
	m_WndMng.Initialize();
	m_WndMng.Create( WBS_NOFRAME | WBS_MANAGER | WBS_NODRAWFRAME, m_rcWindowClient, &m_WndMng, 100 );
	m_WndMng.InitDeviceObjects();

	m_2DRender.InitDeviceObjects( m_pd3dDevice );
	m_2DRender.m_pFont = CWndBase::m_Theme.m_pFontText;

	CWndBase::m_bEdit = TRUE;

    return S_OK;
}


//---------------------------------------------------------------------------------------------
// Invaildate Device Objects ( 장치 오브젝트들 무효화 )
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::InvalidateDeviceObjects()
{
	CWndBase::m_Theme.InvalidateDeviceObjects();
	m_WndMng.InvalidateDeviceObjects(  );
	m_2DRender.InvalidateDeviceObjects();

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// Restore Device Objects ( 장치 오브젝트들 복구 )
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::RestoreDeviceObjects()
{
	CWndBase::m_Theme.RestoreDeviceObjects();
	m_WndMng.RestoreDeviceObjects();
	m_2DRender.RestoreDeviceObjects(&m_d3dsdBackBuffer);

    return S_OK;
}


//---------------------------------------------------------------------------------------------
// Delete Device Objects ( 장치 오브젝트들 삭제 )
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::DeleteDeviceObjects()
{
	CWndBase::m_Theme.DeleteDeviceObjects();
	m_WndMng.RestoreDeviceObjects();
	m_2DRender.DeleteDeviceObjects();
	CWndBase::FreeTileTexture();
    return S_OK;
}


//---------------------------------------------------------------------------------------------
// One Time Scene Init ( Scene 렌더링 전 한번만 초기화 )
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::OneTimeSceneInit()
{
    return S_OK;
}


//---------------------------------------------------------------------------------------------
// Frame Move 
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::FrameMove()
{
	m_WndMng.Process();
    return S_OK;
}


//---------------------------------------------------------------------------------------------
// Render ( 그리기 )
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::Render()
{
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00808080, 1.0f, 0);

	if( SUCCEEDED( m_pd3dDevice->BeginScene( ) ) )
	{
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		m_WndMng.PaintRoot( &m_2DRender );
		m_WndControlFactory.RenderPreviewControl( g_pMainFrame->m_dwToolSelect, &m_2DRender, &(m_WndMng.GetMousePoint()) );
/*
		for( int i = 0; i < m_WndMng.m_wndOrder.GetSize(); ++i )
		{
			CWndBase* pBase = static_cast< CWndBase*>( m_WndMng.m_wndOrder[ i] );

			for(int i = 0; i < pBase->m_wndArray.GetSize(); i++)
			{
				CWndBase* pWnd = (CWndBase*)pBase->m_wndArray[ i ];

				if( !pWnd->m_pTexture || !pWnd->IsWndStyle( WSS_PICTURE ) || ( pBase->GetFocusChild() != pWnd ) )
					continue;

				CRect rect = pWnd->GetScreenRect();
				CPoint ptClient = rect.TopLeft() - pWnd->m_pTexture->m_ptCenter;

				if( pWnd->m_bLButtonDown && !RenderHelper_DragIcon::Instance()->m_IsIntUpdate )
				{
					RenderHelper_DragIcon::Instance()->PrevMousePoint( pWnd->m_ptMouse );
					RenderHelper_DragIcon::Instance()->PrevDragIconPoint( ptClient );

					RenderHelper_DragIcon::Instance()->Begin( pWnd->m_strTexture );
					RenderHelper_DragIcon::Instance()->m_IsIntUpdate = true;

					TRACE("처음 처음 \n" );
				}

				if( pWnd->m_bDragControl && pWnd->m_bLButtonDown )
				{
					CPoint ptPrvMouse = RenderHelper_DragIcon::Instance()->GetPrevMousePoint();
					CPoint ptCurrentPt = ptClient + ( pWnd->m_ptMouse - ptPrvMouse );

					//TRACE("%d\n",ptCurrentPt.x, ptCurrentPt.y );


					//RenderHelper_DragIcon::Instance()->SetDragTexture( pWnd->m_strTexture );
					RenderHelper_DragIcon::Instance()->UpdateVertexFromPoint( ptCurrentPt, pWnd->m_pTexture );
					RenderHelper_DragIcon::Instance()->Draw( CPoint( 300, 300 ) );
					
					//RenderHelper_DragIcon::Instance()->PrevDragIconPoint( ptCurrentPt );
				}

				if( !pWnd->m_bLButtonDown )
				{
					RenderHelper_DragIcon::Instance()->m_IsIntUpdate = false;
					RenderHelper_DragIcon::Instance()->End();

					if( pWnd->m_bDragControl )
						__asm nop
				}
			}
		}
*/
		m_pd3dDevice->EndScene();
	}

  return S_OK;
}


//---------------------------------------------------------------------------------------------
// Adjust Window For Change
// param	: void 
// return	: HRESULT 
//---------------------------------------------------------------------------------------------
HRESULT CDaisyView::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        ::ShowWindow( m_hwndRenderFullScreen, SW_HIDE );
        CD3DApplication::m_hWnd = m_hwndRenderWindow;
    }
    else
    {
        if( ::IsIconic( m_hwndRenderFullScreen ) )
            ::ShowWindow( m_hwndRenderFullScreen, SW_RESTORE );
  
		::ShowWindow( m_hwndRenderFullScreen, SW_SHOW );
        CD3DApplication::m_hWnd = m_hwndRenderFullScreen;
    }
    return S_OK;
}


//---------------------------------------------------------------------------------------------
// On Size ( 뷰의 사이즈가 변경 될 때 )
// param	: nType, cx, cy 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize( nType, cx, cy );
	
	if( m_bActive && m_bWindowed )
	{
		RECT rcClientOld( m_rcWindowClient );

		CView::GetWindowRect(&m_rcWindowBounds );
		CView::GetClientRect(&m_rcWindowClient );

		if( rcClientOld.right - rcClientOld.left !=
			m_rcWindowClient.right - m_rcWindowClient.left ||
			rcClientOld.bottom - rcClientOld.top !=
			m_rcWindowClient.bottom - m_rcWindowClient.top )
		{
			m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
			m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

			HRESULT hr( S_OK );
			if( FAILED( hr = Reset3DEnvironment() ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T("[데이지 뷰] 3D 환경 리셋 실패") );
				FLASSERT( NULL );
				return;
			}
		}
	}	

	m_WndMng.SetWndRect( CRect( 0, 0, cx, cy ) );
}


//---------------------------------------------------------------------------------------------
// On Mouse Move ( 마우스 이동 할 때 ) 
// param	: nFlags, point 
// return	: void
//---------------------------------------------------------------------------------------------
void CDaisyView::OnMouseMove(UINT nFlags, CPoint point) 
{
	Invalidate( FALSE );
	CView::OnMouseMove(nFlags, point);
}


//---------------------------------------------------------------------------------------------
// On Mouse Right Button Down ( 마우스 오른쪽 버튼 눌렀을 때 ) 
// param	: nFlags, point 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWndDialog* pWndDialog = (CWndDialog*) m_WndMng.GetFocusWnd();
	if( NULL == pWndDialog )
		return;

	LPWNDAPPLET	lpWndApplet = CWndBase::m_resMng.GetAt( pWndDialog->GetWndId() );
	if( NULL == lpWndApplet )
		return;

	Invalidate( FALSE );

	CMenu menu; 
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_POPUP_VIEW_OPEN,"&Open");
	menu.AppendMenu(MF_STRING, ID_POPUP_VIEW_REMOVE,"&Remove");
	menu.AppendMenu(MF_STRING, ID_POPUP_VIEW_PROPERTIES,"&Properties");
	::GetCursorPos( &point );
	menu.TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );

	Invalidate( FALSE );
	
	CView::OnRButtonDown(nFlags, point);
}


//---------------------------------------------------------------------------------------------
// On LButton Up ( 마우스 왼쪽 버튼 눌렀다가 땔 때 )
// param	: nFlag, point 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWndDialog* pWndDialog = (CWndDialog*) m_WndMng.GetFocusWnd();
	if( NULL == pWndDialog )
		return;

#ifdef __DAISY_UPDATE_1
	if( pWndDialog == m_WndMng.m_pCurFocus && g_pMainFrame->m_dwToolSelect && CWndBase::m_resMng.GetAt( pWndDialog->GetWndId() ) )
#else 
	if( g_pMainFrame->m_dwToolSelect && CWndBase::m_resMng.GetAt( pWndDialog->GetWndId() ) )
#endif 
	{
		WNDCTRL	wndCtrl = m_WndControlFactory.CreateControl( g_pMainFrame->m_dwToolSelect, pWndDialog );
		FLERROR_ASSERT_LOG_RETURN_VOID( wndCtrl.dwWndId, _T("[데이지 뷰] 컨트롤을 생성 실패!!") );			

		point -= pWndDialog->GetClientRect( TRUE ).TopLeft();
		wndCtrl.rect += point;
		point = CPoint( wndCtrl.rect.Width() / 2, wndCtrl.rect.Height() / 2 );
		wndCtrl.rect -= point;

		char* pData( NULL );
		if( CScript::LookupDefine( wndCtrl.strDefine, (void*&)pData ) == TRUE )
			wndCtrl.dwWndId = atoi( pData );
	
		CWndBase* pWndBase = pWndDialog->CreateControl( CView::GetSafeHwnd(), &wndCtrl );
		FLERROR_ASSERT_LOG_RETURN_VOID( pWndBase, _T("윈도우 컨트롤을 생성 할 수 없습니다.") );			

		wndCtrl.dwWndStyle = pWndBase->GetStyle();
		CWndBase::m_resMng.AddControl( pWndDialog->GetWndId(), &wndCtrl ,TRUE);
		m_dwWndId++;

#ifndef __DAISY_UPDATE_1		
		g_pMainFrame->m_dwToolSelect= 0;
#else
		g_pMainFrame->m_dwToolSelect= 0;
		pWndBase->SetFocus();
#endif 

		pWndDialog->AdjustWndBase();
	}

#ifdef __DAISY_UPDATE_3

	if( pWndDialog->m_pWndFocusChild )
	{
		if( CWndBase::m_resMng.m_nChangeItem > 0 )
		{
			if(CWndBase::m_resMng.m_nChangeItem == CWndBase::m_resMng.CTRL_CHANGE)
			{
				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl( pWndDialog->GetWndId(), pWndDialog->m_pWndFocusChild->GetWndId() );
				if(lpWndCtrl)
				{
					memcpy(&CWndBase::m_resMng.m_OldWndCtrl,lpWndCtrl,sizeof(WNDCTRL));

					LPWNDCTRL lpWndCtrl_new = new WNDCTRL;

					LPWNDCTRL lpWndCtrl_Old = lpWndCtrl;

					*lpWndCtrl_new = *lpWndCtrl_Old;
					lpWndCtrl_new->dwWndHistoryType = CWndBase::m_resMng.CTRL_CHANGE;
					CWndBase::m_resMng.SaveHistory(lpWndCtrl_new);
				}				
			}
			CWndBase::m_resMng.m_nChangeItem = -1;
		}
	}

#endif 

	Invalidate( FALSE );
	CView::OnLButtonUp(nFlags, point);
}


//---------------------------------------------------------------------------------------------
// Open Property ( 속성 창 열기 ) 
// param	: uID 
// return	: BOOL 
//---------------------------------------------------------------------------------------------
bool CDaisyView::OpenProperty( BOOL bIsControl, LPWNDAPPLET pWndApplet, CWndDialog* pWndDialog, CWndBase* pWndBase )
{
	CString strProperty( _T("") );

	if( bIsControl )
	{
		LPWNDCTRL pWndControl = pWndDialog->GetWndCtrl( pWndBase->GetWndId() );
		FLERROR_ASSERT_LOG_RETURN( pWndControl, false, _T("[데이지 뷰] 컨트롤 포인터를 얻어올 수 없습니다.") );			

		strProperty = m_WndControlFactory.GetDescription( pWndBase->GetWndType() , pWndBase->GetWndStyle() ) + CString(_T( " 속성"));
	}
	else
		strProperty = _T("애플릿 속성");


	CPropertySheet				PropertySheet( strProperty );

	if( bIsControl )	// 컨트롤 일 때
	{
		CPropControlGeneral		PropControlGeneral;
		CPropControlBaseStyle	PropControlBaseStyle;
		
		PropertySheet.AddPage( &PropControlGeneral );
		PropertySheet.AddPage( &PropControlBaseStyle );

		PropControlGeneral.m_pDaisyView		= this; 
		PropControlGeneral.m_pWndApplet_	= pWndApplet;
		PropControlGeneral.m_pWndCtrl_		= pWndApplet->GetAt( pWndBase->GetWndId() );
		PropControlGeneral.m_pWndBase		= pWndBase;
		PropControlGeneral.m_pWndDialog		= (CWndDialog*)m_WndMng.m_pWndFocusDialog;

		PropControlBaseStyle.m_pDaisyView	= this;
		PropControlBaseStyle.m_pWndApplet_	= pWndApplet;
		PropControlBaseStyle.m_pWndCtrl_	= pWndApplet->GetAt( pWndBase->GetWndId() );
		PropControlBaseStyle.m_pWndBase		= pWndBase;
		PropControlBaseStyle.m_pWndDialog	= (CWndDialog*)m_WndMng.m_pWndFocusDialog;

		PropertySheet.DoModal();
	}
	else				// 애플릿 일 때
	{
		/// 이건 뭥미... 왜 이게 여기에...
		//마우스로 창크기 조절시 사이즈 변하지 않는것 수정
		pWndApplet->size.cx = pWndBase->GetWndRect().Width();
		pWndApplet->size.cy = pWndBase->GetWndRect().Height();
		/// 확인 예정

		CPropAppletGeneral		PropAppletGeneral;
		CPropAppletBaseStyle	PropAppletBaseStyle;

		PropertySheet.AddPage( &PropAppletGeneral );
		PropertySheet.AddPage( &PropAppletBaseStyle );

		PropAppletGeneral.m_pDaisyView		= this; 
		PropAppletGeneral.m_pWndApplet_		= pWndApplet;
		PropAppletGeneral.m_pWndDialog		= (CWndDialog*)pWndBase;
		
		PropAppletBaseStyle.m_pWndApplet_	= pWndApplet;
		PropAppletBaseStyle.m_pWndDialog	= (CWndDialog*)pWndBase;	

		PropertySheet.DoModal();
		
		CMainFrame*		pMainFrame( (CMainFrame*)AfxGetMainWnd() );
		CResourceView*	pResourceView( (CResourceView*)pMainFrame->m_wndSTCBar.GetView( 0 ) );

		pResourceView->SortTreeApplet();
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// On PopUp Command ( 마우스 우측버튼 팝업 메뉴 이벤트 처리 ) 
// param	: uID 
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CDaisyView::OnPopupCommand(UINT nID)
{
	Invalidate( FALSE );

	CWndBase*		pWndBase( m_WndMng.m_pCurFocus );
	CWndDialog*		pWndDialog( (CWndDialog*)m_WndMng.GetFocusWnd() );
	LPWNDAPPLET		lpWndApplet( CWndBase::m_resMng.GetAt(pWndDialog->GetWndId()) );

	FLERROR_ASSERT_LOG_RETURN( lpWndApplet, false,	_T("[데이지 뷰] 애플릿 포인터를 얻어올 수 없습니다.") );			
	FLERROR_ASSERT_LOG_RETURN( pWndDialog,	false,	_T("[데이지 뷰] 다이얼로그 포인터를 얻어올 수 없습니다.") );			
	FLERROR_ASSERT_LOG_RETURN( pWndBase,	false,	_T("[데이지 뷰] 컨트롤 포인터를 얻어올 수 없습니다.") );			

	switch( nID )
	{
	case ID_POPUP_VIEW_REMOVE:
		{
			if( FALSE == pWndBase->IsWndStyle( WBS_CHILD ) )
				break;

			pWndBase->Destroy();

			LPWNDCTRL lpWndCtrl( NULL );
			lpWndCtrl = lpWndApplet->GetAt( pWndBase->GetWndId() );

			if( NULL == lpWndCtrl )
				break;
		
			CWndBase::m_resMng.RemoveControl( lpWndApplet->dwWndId, lpWndCtrl->strDefine,TRUE);
			CWndBase::m_resMng.ResetNewControlId();
			pWndDialog->AdjustWndBase();
		}
		break;
	case ID_POPUP_VIEW_PROPERTIES:
		OpenProperty( pWndBase->IsWndStyle( WBS_CHILD ), lpWndApplet, pWndDialog, pWndBase );
		break;
	}

	Invalidate( FALSE );
	return TRUE;
}


//---------------------------------------------------------------------------------------------
// On File New App 
// param	: void 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnFileNewapp() 
{
	CWndDialog* pWndDialog = new CWndDialog;
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndDialog,  _T("[데이지 뷰]윈도우 다이얼로그 생성 실패.") );			

	pWndDialog->Initialize( NULL, g_pMainFrame->m_nIdApplet );

	WNDAPPLET wndApplet;

	wndApplet.dwWndStyle	= pWndDialog->GetWndStyle();
	wndApplet.strTitle		= pWndDialog->GetTitle();
	wndApplet.size.cx		= pWndDialog->GetWindowRect().Width();
	wndApplet.size.cy		= pWndDialog->GetWindowRect().Height();
	wndApplet.d3dFormat		= D3DFMT_A4R4G4B4;
	wndApplet.nDefSizeX		= 0;
	wndApplet.nDefSizeY		= 0;

	CString strDefine = CWndBase::m_resMng.GetNewAppletDefine();
	if( CWndBase::m_resMng.AddApplet( &wndApplet ) )
	{
		CResourceView* pResourceView = (CResourceView*)g_pMainFrame->m_wndSTCBar.GetView( 0 );
		pResourceView->AddApplet( strDefine, NULL );
	}

	Invalidate( FALSE );
}


//---------------------------------------------------------------------------------------------
// On Key Up ( 키를 눌렀다가 땟을 때 ) 
// param	: nChar, nRepCnt, nFlags 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch( nChar )
	{
		case VK_DELETE:
			break;
#ifdef __DAISY_UPDATE_1
		case VK_ESCAPE:
			g_pMainFrame->m_dwToolSelect = 0;
			break;
#endif //__DAISY_UPDATE_1
	}

#ifdef __DAISY_UPDATE_1
	if(GetKeyState(VK_CONTROL) < 0)
	{
		switch(nChar) 
		{
			case 67: //Copy - Ctrl+c
				{
					CWndBase* pWndBase = m_WndMng.m_pCurFocus;
					if(pWndBase)
						memcpy(&m_pTempWnd, pWndBase, sizeof(CWndBase));
				}
				break;
			case 86: //Paste - Ctrl+v
				if(m_pTempWnd.GetWndId() != -1)
				{
					CWndDialog* pWndDialog = (CWndDialog*) m_WndMng.GetFocusWnd();
					
					LPWNDAPPLET lpWndApplet = CWndBase::m_resMng.GetAt( pWndDialog->GetWndId() );
					
					WNDCTRL wndCtrl = *(lpWndApplet->GetAt( m_pTempWnd.GetWndId() ));
					wndCtrl.dwWndId = CWndBase::m_resMng.GetNewControlId( pWndDialog->GetWndId() );
					wndCtrl.rect.bottom += 10;
					wndCtrl.rect.top += 10;
					wndCtrl.rect.right += 10;
					wndCtrl.rect.left += 10;
					CString strTemp = wndCtrl.strDefine;
					
					wndCtrl.strDefine = CWndBase::m_resMng.GetNewControlDefine( pWndDialog->GetWndId(), wndCtrl.strDefine );
					CHAR* pData;
					if( CScript::LookupDefine( wndCtrl.strDefine, (void*&)pData ) == TRUE )
						wndCtrl.dwWndId = atoi( pData );
					// 콘트롤 생성 
					CWndBase* pWndBase = pWndDialog->CreateControl( CView::GetSafeHwnd(), &wndCtrl );
					
					// 최종 추가 
					wndCtrl.dwWndStyle = pWndBase->GetStyle();
					CWndBase::m_resMng.AddControl( pWndDialog->GetWndId(), &wndCtrl, TRUE );
					m_dwWndId++;
					
					pWndDialog->AdjustWndBase();
				}
				break;
#ifdef __DAISY_UPDATE_3
			case 90: //Undo - Ctrl+z
				{
					CWndDialog* pWndDialog = (CWndDialog*)m_WndMng.GetFocusWnd();
					CWndBase* pWndBase = m_WndMng.m_pCurFocus;
					LPWNDAPPLET lpWndApplet = CWndBase::m_resMng.GetAt( pWndDialog->GetWndId() );
					if(pWndBase)
					{
						CWndBase::m_resMng.UndoHistory(lpWndApplet,pWndBase,pWndDialog,CView::GetSafeHwnd());
					}
					pWndDialog->AdjustWndBase();
				}
				break;
			case 89: //Redo - Ctrl+y
				{
					CWndDialog* pWndDialog = (CWndDialog*)m_WndMng.GetFocusWnd();
					CWndBase* pWndBase = m_WndMng.m_pCurFocus;
					LPWNDAPPLET lpWndApplet = CWndBase::m_resMng.GetAt( pWndDialog->GetWndId() );
					if(pWndBase)
					{
						CWndBase::m_resMng.RedoHistory(lpWndApplet,pWndBase,pWndDialog,CView::GetSafeHwnd());
					}
					pWndDialog->AdjustWndBase();
				}
				break;
#endif //__DAISY_UPDATE_3
		}
		g_pMainFrame->m_pDaisyView->CView::OnPaint();
	}
#endif //__DAISY_UPDATE_1

	Invalidate( FALSE );
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}



//---------------------------------------------------------------------------------------------
// On Insert New Class ( 새로운 클레스 추가 ) 아... 누가 이렇게 리팩토링 예정
// param	: void 
// return	: void 
//---------------------------------------------------------------------------------------------
void CDaisyView::OnInsertNewClass() 
{
	CWndBase* pWndBase = m_WndMng.GetFocusWnd();

	if( pWndBase && pWndBase != &m_WndMng )
	{
		CDlgNewClass dlgNewClass;
		dlgNewClass.Init( (CWndDialog*)pWndBase );
		if( dlgNewClass.DoModal() == IDOK )
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

			CString strBlankFile;
			CHAR* pszString;
			int nResult = IDYES;
			//CHAR* pszStringDest;
			CFile file;
			int nLen = 0;
			//
			// Source
			//
			strBlankFile.Format( "%sDaisy\\WndBlank.cpp", GetResourcePath() ); 
			if( file.Open( strBlankFile, CFile::modeRead ) == FALSE )
			{
				AfxMessageBox( "Resource\\Daisy\\WndBlank.cpp 파일을 찾을 수 없습니다." );
				return; 
			}
			CString strString;
			nLen = (int)( file.GetLength() );
			pszString = new CHAR[ nLen + 1 ];
			file.Read( pszString, nLen );
			pszString[ nLen ] = 0;
			strString = pszString;
			delete pszString;
			file.Close();

			strString.Replace( "CWndBlank", dlgNewClass.m_strClassName );
			strString.Replace( "__WNDID"  , dlgNewClass.m_strWndId     );
			strString.Replace( "CWndBlank", dlgNewClass.m_strClassName );
			CString strInclude;
			strInclude.Format( "#include \"%s\"", dlgNewClass.m_strHeaderName );
			strString.Replace( "__INCLUDE", strInclude );
			CString strWIDLabel;
			LPWNDAPPLET lpWndApplet = CWndBase::m_resMng.GetAt( pWndBase->GetWndId() );
			strWIDLabel += "/****************************************************\n";
			strWIDLabel += "  WndId : ";
			strWIDLabel += lpWndApplet->strDefine; 
			strWIDLabel += " - ";
			strWIDLabel += lpWndApplet->strTitle;
			strWIDLabel += "\n"; 
			for( int i = 0; i < lpWndApplet->ptrCtrlArray.GetSize(); i++ )
			{
				LPWNDCTRL lpWndCtrl = (LPWNDCTRL) lpWndApplet->ptrCtrlArray.GetAt( i );
				strWIDLabel += "  CtrlId : "; 
				//strWIDLabel += lpWndCtrl->szDefine; 
				strWIDLabel += lpWndCtrl->strDefine; 
				strWIDLabel += " - ";
				//strWIDLabel += lpWndCtrl->szTitle;
				strWIDLabel += lpWndCtrl->strTitle;
				strWIDLabel += "\n"; 
			}
			strWIDLabel += "****************************************************/";
			strString.Replace( "__WID_LABEL", strWIDLabel );
			
			CString strFileName;
			strFileName.Format( "%s\\%s", pMainFrame->m_strSourcePath, dlgNewClass.m_strFileName );

			if( file.Open( strFileName, CFile::modeRead ) )
			{
				file.Close();
				CString strMessage;
				strMessage.Format( "%s 파일이 있습니다. YES는 덮어쓰기, NO는 추가하기. CANCEL은 취소입니다.", strFileName );
				nResult = AfxMessageBox( strMessage, MB_YESNOCANCEL );
				if( nResult == IDCANCEL )
					return;
			} 
			// overWrite or create
			if( nResult == IDYES )
			{
				if( file.Open( strFileName, CFile::modeCreate | CFile::modeWrite ) == FALSE )
				{
					AfxMessageBox( "소스 파일 저장에 실패했습니다." );
					return;
				}
				file.Write( strString.LockBuffer(), strString.GetLength() );
			}
			else
			// append
			if( nResult == IDNO )
			{
				if( file.Open( strFileName, CFile::modeRead ) == FALSE )
				{
					AfxMessageBox( "소스 파일을 읽는데 실패했습니다." );
					return;
				}
				int nLen2 = (int)( file.GetLength() );
				pszString = new CHAR[ nLen2 + 1 ];
				file.Read( pszString, nLen2 );
				pszString[ nLen2 ] = 0;
				file.Close();
				if( file.Open( strFileName, CFile::modeCreate | CFile::modeWrite ) == FALSE )
				{
					AfxMessageBox( "소스 파일 저장에 실패했습니다." );
					return;
				}
				file.Write( pszString, nLen2 );
				file.Write( strString.LockBuffer(), strString.GetLength() );
				delete pszString;
			}
			strString.UnlockBuffer();
			file.Close();
			//
			// Header
			//
			strBlankFile.Format( "%sDaisy\\WndBlank.h", GetResourcePath() ); 
			if( file.Open( strBlankFile, CFile::modeRead ) == FALSE )
			{
				AfxMessageBox( "Resource\\Daisy\\WndBlank.h 파일을 찾을 수 없습니다." );
				return; 
			}
			nLen =  (int)( file.GetLength() );
			pszString = new CHAR[ nLen + 1 ];
			file.Read( pszString, nLen );
			pszString[ nLen ] = 0;
			strString = pszString;
			delete pszString;
			file.Close();
			
			strString.Replace( "CWndBlank", dlgNewClass.m_strClassName );
			CString strBLANK, strBLANK2;
			strBLANK = dlgNewClass.m_strClassName;
			if( strBLANK.IsEmpty() == FALSE && strBLANK[ 0 ] == 'C' )
				strBLANK.Delete( 0 );
			strBLANK.MakeUpper();
			strBLANK2.Format( "__%s__H", strBLANK );
			strString.Replace( "__BLANK_H", strBLANK2 );

			strFileName.Format( "%s\\%s", pMainFrame->m_strSourcePath, dlgNewClass.m_strHeaderName );
			nResult = IDYES ;
			if( file.Open( strFileName, CFile::modeRead ) )
			{
				file.Close();
				CString strMessage;
				//strMessage.Format( "%s 파일이 있습니다. 덮어쓰시겠습니까?", strFileName );
				strMessage.Format( "%s 파일이 있습니다. YES는 덮어쓰기, NO는 추가하기. CANCEL은 취소입니다.", strFileName );
				nResult = AfxMessageBox( strMessage, MB_YESNOCANCEL );
				if( nResult == IDCANCEL )
					return;
			}
			// overWrite or create
			if( nResult == IDYES )
			{
				if( file.Open( strFileName, CFile::modeCreate | CFile::modeWrite ) == FALSE )
				{
					AfxMessageBox( "해더 파일 저장에 실패했습니다." );
					return;
				}
				file.Write( strString.LockBuffer(), strString.GetLength() );
			}
			else
			// append
			if( nResult == IDNO )
			{
				if( file.Open( strFileName, CFile::modeRead ) == FALSE )
				{
					AfxMessageBox( "소스 파일을 읽는데 실패했습니다." );
					return;
				}
				int nLen2 = (int)( file.GetLength() );
				pszString = new CHAR[ nLen2 + 1 ];
				file.Read( pszString, nLen2 );
				pszString[ nLen2 ] = 0;
				file.Close();
				if( file.Open( strFileName, CFile::modeCreate | CFile::modeWrite ) == FALSE )
				{
					AfxMessageBox( "소스 파일 저장에 실패했습니다." );
					return;
				}
				file.Write( pszString, nLen2 );
				file.Write( strString.LockBuffer(), strString.GetLength() );
				delete pszString;
			}
			strString.UnlockBuffer();
			
			file.Close();
		}
	}
}


void CDaisyView::OnUpdateInsertNewClass(CCmdUI* pCmdUI) 
{
	CWndBase* pWndBase = m_WndMng.GetFocusWnd();
	if( pWndBase == &m_WndMng )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( pWndBase ? TRUE : FALSE );
}


void CDaisyView::OnLayoutGuideSetting() 
{
	CDlgGuideSetting dlgGuideSetting;
	
	dlgGuideSetting.m_nWidth  = CWndBase::m_nGridWidth;
	dlgGuideSetting.m_nHeight = CWndBase::m_nGridHeight;
	dlgGuideSetting.m_nLayout = CWndBase::m_nLayout;
	
	if( dlgGuideSetting.DoModal() == IDOK )
	{
		CWndBase::m_nGridWidth  = dlgGuideSetting.m_nWidth ;
		CWndBase::m_nGridHeight = dlgGuideSetting.m_nHeight;
		CWndBase::m_nLayout		= dlgGuideSetting.m_nLayout;
	}
}


void CDaisyView::OnLayoutCling() 
{
	m_WndMng.m_bCling = !m_WndMng.m_bCling;	
}


void CDaisyView::OnUpdateLayoutCling(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_WndMng.m_bCling );
}


void CDaisyView::OnLayoutShowgrid() 
{
	m_WndMng.m_bShowGrid = !m_WndMng.m_bShowGrid;
	Invalidate( FALSE );	
}


void CDaisyView::OnUpdateLayoutShowgrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_WndMng.m_bShowGrid );	
}


#ifdef _DEBUG

	void CDaisyView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CDaisyView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CDaisyDoc* CDaisyView::GetDocument() 
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDaisyDoc)));
		return (CDaisyDoc*)m_pDocument;
	}

#endif 