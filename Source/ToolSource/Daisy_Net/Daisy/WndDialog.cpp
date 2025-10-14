// DaisyView.cpp : implementation of the CDaisyView class
//

#include "stdafx.h"
#include "resource.h"
#include "DaisyDoc.h"
#include "DaisyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
#ifdef __DAISY
			if( m_bEdit && pWnd == m_pWndFocus )
			{
				p2DRender->m_clipRect = rectOld;
				p2DRender->SetViewportOrg(ptViewPortOld);
				p2DRender->SetViewport( CRect( 0, 0, m_pApp->m_d3dsdBackBuffer.Width, m_pApp->m_d3dsdBackBuffer.Height) );

				CRect rect = pWnd->GetWindowRect( TRUE );
				rect.InflateRect( 1, 1 );
				p2DRender->RenderRect( rect, 0x80ffffff );
				rect.InflateRect( 1, 1 );
				p2DRender->RenderRect( rect, 0x80ffffff );
				rect.InflateRect( 1, 1 );
				p2DRender->RenderRect( rect, 0x80ffffff );
			}
#endif
*/

CWndBase* CWndManager::m_pWndFocusDialog = NULL;

BOOL CWndManager::Process()
{
	if( m_pWndFocus != this && 
		m_pWndFocus->IsDestroy() == FALSE && 
		m_pWndFocus->IsVisible() == TRUE )
		m_pWndFocusDialog = m_pWndFocus;
	
	return TRUE;
}
void CWndManager::PaintRoot( C2DRender* p2DRender )
{
	CWndBase::PaintRoot( p2DRender );
	if( m_pWndFocusDialog  )
	{
		CRect rect = m_pWndFocusDialog->GetWindowRect( TRUE );
		p2DRender->RenderResizeRect( rect, 4 );
	}

}
CWndManager::CWndManager()
{
	m_nResizeDir = 0;
	m_bShowGrid = TRUE;
	
#ifdef __DAISY_UPDATE_1
	InitializeClipedWnd();
	m_nCountClip = 0;
#endif //__DAISY_UPDATE_1
}

CWndManager::~CWndManager()
{
}

#ifdef __DAISY_UPDATE_1
void CWndManager::InitializeClipedWnd()
{
	for(int i=0; i<MAX_CLIPED_WINDOWS; i++)
		m_pClipedWnd[i] = NULL;
}

void CWndManager::ClipWindow(CWndBase* pWndBase)
{
	for(int i=0; i<MAX_CLIPED_WINDOWS; i++)
	{
		if(m_pClipedWnd[i] == NULL)
		{
			m_pClipedWnd[i] = pWndBase;
			m_nCountClip++;
			i = MAX_CLIPED_WINDOWS;
		}
	}
}

void CWndManager::RemoveClipedWnd(CWndBase* pWndBase)
{
	for(int i=0; i<MAX_CLIPED_WINDOWS; i++)
	{
		if(m_pClipedWnd[i] == pWndBase)
		{
			m_pClipedWnd[i] = NULL;
			m_nCountClip--;
		}
	}

	CWndBase* pTempWnd[MAX_CLIPED_WINDOWS];
	memcpy(pTempWnd, m_pClipedWnd, sizeof(CWndBase*)*MAX_CLIPED_WINDOWS);

	InitializeClipedWnd();
	int k=0;
	for(int i=0; i<MAX_CLIPED_WINDOWS; i++)
	{
		if(pTempWnd[i] != NULL)
		{
			m_pClipedWnd[k] = pTempWnd[i];
			k++;
		}
	}
}

BOOL CWndManager::IsClipedWindow(CWndBase* pWndBase)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<MAX_CLIPED_WINDOWS; i++)
	{
		if(m_pClipedWnd[i] == pWndBase)
			rtnval = TRUE;
	}

	return rtnval;
}

#ifdef __DAISY_UPDATE_3
void CWndManager::UpdateClipedWindow(LPWNDCTRL lpWndCtrl, int nCount)
{
	if(m_nCountClip == nCount)
	{
		for(int i=0; i<nCount; i++)
		{
			if(m_pClipedWnd[i])
				m_pClipedWnd[i]->SetWndRect(lpWndCtrl[i].rect);
		}
	}
}
#endif //__DAISY_UPDATE_3

LRESULT CWndManager::WindowRootProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( m_pApp == NULL )
		return 0;
	m_rectCurrentWindow = m_rectWindow;
	m_rectCurrentClient = m_rectClient;

	GET_CLIENT_POINT( m_pApp->GetSafeHwnd(),  point );
	CRect rectWnd = m_rectWindow;
	CPoint ptClient = point - m_rectCurrentClient.TopLeft();
	CPoint ptWindow = point - m_rectCurrentWindow.TopLeft();
	m_ptMouse = ptClient;
	// WM_MOUSEMOVE 매시지지만 마우스 포인트가 움직이지 않았다면 무효화 
	if( message == WM_MOUSEMOVE )
	{
		static CPoint ptOld;
		if( point == ptOld )
			return 0;
		ptOld = point;
	}
	// 루트에서 포커스 메시지가 날아오면 윈도가 날린 메시지다. 포커스는 CWndBase에서만 따로 사용한다. 따라서 그냥 리턴 
	if(message == WM_SETFOCUS || message == WM_KILLFOCUS || message == WM_GETFONT || message == WM_CAPTURECHANGED )
		return 0;
	// IME 메시지 걸러내기 
	if(message == WM_IME_SETCONTEXT || message == WM_IME_NOTIFY || message == WM_IME_CONTROL || message == WM_IME_COMPOSITIONFULL || message == WM_IME_SELECT)
		return 0;
	if(message == WM_GETFONT || message == WM_GETTEXT || message == WM_IME_NOTIFY ||  message == WM_IME_CHAR || message == WM_IME_KEYDOWN || message == WM_IME_KEYUP || message == 0x287)
		return 0;

	// OnSetCursor와 OnMouseMove 메시지 보내기 
	CWndBase* pWndOnMouseMain  = NULL;
	CWndBase* pWndOnMouseChild = NULL;
#ifndef __V050913_TABWND
//{{AFX
	CWndBase* pWndOnMouseChildFrame = NULL;
//}}AFX
#endif
	m_pWndOnSetCursor = NULL;
	m_pWndOnMouseMove = NULL;
	if( m_pWndCapture )
	{
		m_pWndOnMouseMove = m_pWndCapture;
		m_pWndOnSetCursor = m_pWndCapture;
		pWndOnMouseMain = m_pWndCapture;
	}
	else
	// 클릭할 때만 윈도를 검색하는 최적화가 필요함.
	for( int i = m_wndOrder.GetSize() - 1; i >= 0 ; i-- )
	{
		CWndBase* pWndBase = (CWndBase*)m_wndOrder[ i ];
		if( pWndBase->IsVisible() )
		{
			CRect rect = pWndBase->GetScreenRect();
			if( rect.PtInRect( ptClient ) ) 
			{
				// 마우스가 있는 윈도를 골랐다. 
				pWndOnMouseMain = pWndBase;
				if( m_pWndCapture == NULL )
				{
					// 차일드, 차일드프레임 모두 포함 
					CWndBase* pWndChild = GetChildFocus( pWndBase, ptClient );
					if( pWndBase != pWndChild )
					{
#ifndef __V050913_TABWND // 도대체 WBS_CHILDFRAME는 어따 쓰는거지??
//{{AFX
						if( pWndChild->IsWndStyle( WBS_CHILDFRAME ) == FALSE )
							pWndOnMouseChildFrame = GetChildFrameFocus( pWndBase, ptClient );
//}}AFX
#endif
						pWndBase = pWndChild;
						pWndOnMouseChild = pWndChild;
					}
					m_pWndOnSetCursor = pWndBase;
					m_pWndOnMouseMove = pWndBase;
				}
				break;
			}
		}
	}
	// 시스템 처리부 
	switch(message)	
	{
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_LBUTTONDBLCLK:
#ifdef __CLIENT
		g_toolTip.CancelToolTip();
#endif
		// 지금 포커스가 BWS_MODAL 스타일이라면 포커스와 순서를 변경하지 않는다.
		if( m_pWndFocus && m_pWndFocus->m_dwStyle & WBS_MODAL )
		{
			GetChildFocus( m_pWndFocus, ptClient )->SetFocus();
			break;
		}
		if( pWndOnMouseMain )
		{
			pWndOnMouseMain->SetFocus();
			if(m_pWndFocusDialog != pWndOnMouseMain)
			{
				m_nCountClip = 0;
				InitializeClipedWnd();
			}
		}
#ifndef __V050913_TABWND
//{{AFX
		if( pWndOnMouseChildFrame )
			pWndOnMouseChildFrame->SetFocus();
//}}AFX
#endif
		// pWndOnMouseChildFramer과 pWndOnMouseChild가 같을 수 있다. 
		// 같다면 구지 pWndOnMouseChild에 셋포커스를 할 필요는 없다
		
		if( pWndOnMouseChild )
		{
			if(message == WM_LBUTTONDOWN && GetKeyState(VK_CONTROL) < 0)
			{
				//1. Clip이 안된 Window를 선택했을 때
				if(!IsClipedWindow(pWndOnMouseChild))
					ClipWindow(pWndOnMouseChild);
				else //2. 이미 Clip된 Window를 선택했을 때
				{
					ClipWindow(pWndOnMouseMain->m_pWndFocusChild);
					RemoveClipedWnd(pWndOnMouseChild);
					pWndOnMouseChild->SetFocus();
				}
			}
			else if(GetKeyState(VK_CONTROL) >= 0)
			{
				//Clip Window Initiailize
				InitializeClipedWnd();
				m_nCountClip = 0;
				pWndOnMouseChild->SetFocus();
			}
		}
		break;
	case WM_LBUTTONUP: //  case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_LBUTTONDBLCLK:
		if( m_GlobalShortcut.m_dwShortcut != SHORTCUT_NONE )
		{
			if( pWndOnMouseChild == NULL )
				pWndOnMouseChild = pWndOnMouseMain;
			if( pWndOnMouseChild )
			{
				//pWndOnMouseChild->SetFocus();
				pWndOnMouseChild->OnDropIcon( &m_GlobalShortcut, pWndOnMouseChild->GetMousePoint() );
				// 지금 포커스가 매뉴라면 매뉴의 페어런트에 포커스를 옮긴다.(결과 매뉴를 닫게 된다.)
				if( pWndOnMouseChild->IsWndStyle( WBS_POPUP ) ) 
				{
					CWndBase* pWndTemp = pWndOnMouseChild;
					while( pWndTemp && pWndTemp->IsWndStyle( WBS_POPUP ) )
						pWndTemp = (CWndMenu*) pWndTemp->GetParentWnd();
					if( pWndTemp ) pWndTemp->m_pParentWnd->SetFocus();
				}
			}
			ZeroMemory( &m_GlobalShortcut, sizeof( m_GlobalShortcut ) );
		}
		break;
	}
	return WindowProc( message, wParam, lParam );
}

void CWndManager::AlignClipWndLeft()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{				
				int width;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				width = rectWnd.right - rectWnd.left;
				rectWnd.left = RootWndRect.left;
				rectWnd.right = rectWnd.left + width;
				m_pClipedWnd[i]->Move( rectWnd.TopLeft() );
				m_pClipedWnd[i]->AdjustWndBase();

				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}

void CWndManager::AlignClipWndRight()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{			
				int width;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				width = rectWnd.right - rectWnd.left;
				rectWnd.right = RootWndRect.right;
				rectWnd.left = rectWnd.right - width;
				m_pClipedWnd[i]->Move( rectWnd.TopLeft() );
				m_pClipedWnd[i]->AdjustWndBase();

				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}

void CWndManager::AlignClipWndTop()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{				
				int height;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				height = rectWnd.bottom - rectWnd.top;
				rectWnd.top = RootWndRect.top;
				rectWnd.bottom = rectWnd.top + height;
				m_pClipedWnd[i]->Move( rectWnd.TopLeft() );
				m_pClipedWnd[i]->AdjustWndBase();

				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}

void CWndManager::AlignClipWndBottom()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{				
				int height;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				height = rectWnd.bottom - rectWnd.top;
				rectWnd.bottom = RootWndRect.bottom;
				rectWnd.top = rectWnd.bottom - height;
				m_pClipedWnd[i]->Move( rectWnd.TopLeft() );
				m_pClipedWnd[i]->AdjustWndBase();
			
				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}

void CWndManager::MakeClipWndSameWidht()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{	
				int width;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				width = RootWndRect.right - RootWndRect.left;
				rectWnd.right = rectWnd.left + width;
				m_pClipedWnd[i]->SetWndRect( rectWnd );
				m_pClipedWnd[i]->AdjustWndBase();

				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}

void CWndManager::MakeClipWndSameHeight()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{				
				int height;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				height = RootWndRect.bottom - RootWndRect.top;
				rectWnd.bottom = rectWnd.top + height;
				m_pClipedWnd[i]->SetWndRect( rectWnd );
				m_pClipedWnd[i]->AdjustWndBase();

				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}

void CWndManager::MakeClipWndSameSize()
{
	if(m_nCountClip > 0)
	{
#ifdef __DAISY_UPDATE_3
		WNDCTRL OldWndCtrl[MAX_CLIPED_WINDOWS];
		WNDCTRL ChangeWndCtrl[MAX_CLIPED_WINDOWS];
#endif //__DAISY_UPDATE_3
		CRect RootWndRect = m_pWndFocusDialog->m_pWndFocusChild->GetWindowRect( TRUE );
		for(int i=0; i<m_nCountClip; i++)
		{
			if(m_pClipedWnd[i] != NULL)
			{				
				int width, height;
				CRect rectWnd = m_pClipedWnd[i]->GetWindowRect( TRUE );
				width = RootWndRect.right - RootWndRect.left;
				height = RootWndRect.bottom - RootWndRect.top;
				rectWnd.right = rectWnd.left + width;
				rectWnd.bottom = rectWnd.top + height;
				m_pClipedWnd[i]->SetWndRect( rectWnd );
				m_pClipedWnd[i]->AdjustWndBase();

				LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl(m_pWndFocusDialog->GetWndId(), m_pClipedWnd[i]->GetWndId());
#ifdef __DAISY_UPDATE_3
				OldWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
				if( lpWndCtrl )
					lpWndCtrl->rect = m_pClipedWnd[i]->GetWindowRect(TRUE);
#ifdef __DAISY_UPDATE_3
				ChangeWndCtrl[i] = *lpWndCtrl;
#endif //__DAISY_UPDATE_3
			}
		}
#ifdef __DAISY_UPDATE_3
//		CClipedCtrlsChange* pClipedCtrlsChange = new CClipedCtrlsChange(OldWndCtrl, ChangeWndCtrl, m_nCountClip);
//		g_pCommandManager->SetCommand(pClipedCtrlsChange);
#endif //__DAISY_UPDATE_3
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
		CWndBase::m_resMng.SetModifiedScript( TRUE );
	}
}
#endif //__DAISY_UPDATE_1

void CWndManager::OnDraw(C2DRender* p2DRender)
{
//	GetMousePoint()
}

BOOL CWndManager::OnEraseBkgnd( C2DRender* p2DRender )
{
	return TRUE;

}
void CWndManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nResizeDir = 0;
	ReleaseCapture();
}
void CWndManager::OnLButtonDown(UINT nFlags, CPoint point)
{
		// 1 = top
		// 2 = bottom
		// 3 = left
		// 4 = right
		// 5 = topLeft
		// 6 = topRight
		// 7 = bottomLeft
		// 8 = bottomRigh;
	if( m_pWndFocusDialog && m_pWndFocusDialog->IsWndStyle( WBS_DOCKING ) == FALSE )
	{
		CRect rect = m_pWndFocusDialog->GetWindowRect( TRUE );
		m_nResizeDir = GetResizeDir( rect, point );
		m_pointOld = point;// - rect.TopLeft();
		m_rectOld = rect;
	}
	SetCapture();
}
int CWndManager::GetResizeDir( CRect rect,CPoint ptClient)
{
	// 1 = top
	// 2 = bottom
	// 3 = left
	// 4 = right
	// 5 = topLeft
	// 6 = topRight
	// 7 = bottomLeft
	// 8 = bottomRigh;

	if( rect.PtInRect( ptClient ) == TRUE )
		return 0;
	rect.InflateRect(5,5);
	if( rect.PtInRect( ptClient ) == FALSE )
		return 0;
	rect.DeflateRect(5,5);
	
	int nDir = 0;
	if(ptClient.x <= rect.left && ptClient.y <= rect.top)
		return 5;
	if(ptClient.x >= rect.right && ptClient.y <= rect.top)
		return 6;
	if(ptClient.x <= rect.left && ptClient.y >= rect.bottom)
		return 7;
	if(ptClient.x >= rect.right && ptClient.y >= rect.bottom)
		return 8;

	if(ptClient.y <= rect.top)
		return 1;
	if(ptClient.y >= rect.bottom)
		return 2;
	if(ptClient.x <= rect.left)
		return 3;
	if(ptClient.x >= rect.right)
		return 4;
	return nDir;
}
void CWndManager::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_nResizeDir )
	{
		CRect rectWnd = m_pWndFocusDialog->GetWindowRect( TRUE );
		CPoint pt = point - m_pointOld;
		pt.x = ( pt.x / 16 ) * 16;
		pt.y = ( pt.y / 16 ) * 16;

		switch( m_nResizeDir )
		{
		case 1: rectWnd.top    = m_rectOld.top    + pt.y; break; 
		case 2: rectWnd.bottom = m_rectOld.bottom + pt.y; break;
		case 3: rectWnd.left   = m_rectOld.left   + pt.x; break;
		case 4: rectWnd.right  = m_rectOld.right  + pt.x; break;
		case 5: rectWnd.left   = m_rectOld.left  + pt.x; rectWnd.top    = m_rectOld.top + pt.y; break; 
		case 6: rectWnd.right  = m_rectOld.right + pt.x; rectWnd.top    = m_rectOld.top + pt.y; break;
		case 7: rectWnd.left   = m_rectOld.left  + pt.x; rectWnd.bottom = m_rectOld.bottom + pt.y; break;
		case 8: rectWnd.right  = m_rectOld.right + pt.x; rectWnd.bottom = m_rectOld.bottom + pt.y; break;
/*
		case 1: rectWnd.top    += pt.y; break; 
		case 2: rectWnd.bottom += pt.y; break;
		case 3: rectWnd.left   += pt.x; break;
		case 4: rectWnd.right  += pt.x; break;
		case 5: rectWnd.left  += pt.x; rectWnd.top += pt.y; break; 
		case 6: rectWnd.right += pt.x; rectWnd.top += pt.y; break;
		case 7: rectWnd.left  += pt.x; rectWnd.bottom += pt.y; break;
		case 8: rectWnd.right += pt.x; rectWnd.bottom += pt.y; break;
		*/
		}
		/*
		if( m_bCling )// && !IsWndStyle( WBS_NOCLING ) )
		{
			CRect rect;
			if( m_pParentWnd == m_pWndRoot )
				rect = m_pParentWnd->GetClientRect();
			else
				rect = m_pParentWnd->GetLayoutRect();

			if( rectWnd.top < rect.top + 10 ) rectWnd.top = rect.top;
			if( rectWnd.bottom > rect.bottom - 10 ) rectWnd.bottom = rect.bottom;
			if( rectWnd.left < rect.left + 10 ) rectWnd.left = rect.left;
			if( rectWnd.right > rect.right - 10 ) rectWnd.right = rect.right;
		}
		*/

		//m_pointOld = point;// - rectWnd.TopLeft();
		m_pWndFocusDialog->SetWndRect( rectWnd, TRUE );
	}
}
void CWndManager::OnDestroyChildWnd( CWndBase* pWndChild )
{
	SAFE_DELETE( pWndChild );
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

CWndDialog::CWndDialog()
{
	m_nResizeDir = 0;
	m_lpWndApplet = NULL;
	
}

CWndDialog::~CWndDialog()
{
	if( CWndManager::m_pWndFocusDialog == this )
		CWndManager::m_pWndFocusDialog = NULL;
	m_lpWndApplet->pWndBase = NULL;
}
void CWndDialog::OnDraw(C2DRender* p2DRender)
{
	if( CWndBase::m_nLayout == 2 && ((CWndManager*)m_pWndRoot)->m_bShowGrid )
	{
		CRect rect = GetClientRect();
		for( int y = 0; y < rect.Height(); y += m_nGridHeight * 10)
		{
			for( int x = 0; x < rect.Width(); x += m_nGridWidth * 10 )
			{
				p2DRender->RenderFillRect( CRect( x, y, x+1,y+1), 0xff808080 );
			}
		}
		p2DRender->RenderRect( CRect( 8, 8, rect.Width() - 8, rect.Height() - 8 ), 0xff808080 );
	}
	if( m_pWndFocusChild && m_pWndFocusChild->IsWndStyle( WBS_DOCKING ) == FALSE )
	{
		CRect rect = m_pWndFocusChild->GetWindowRect( TRUE );
		p2DRender->RenderResizeRect( rect, 4 );

#ifdef __DAISY_UPDATE_1
		if(CWndManager::m_pWndFocusDialog == this)
		{
			for(int i=0; i<MAX_CLIPED_WINDOWS; i++)
			{
				if(g_pMainFrame->m_pDaisyView->m_WndMng.m_pClipedWnd[i] != NULL)
				{
					int nThick = 4;
					
					rect = g_pMainFrame->m_pDaisyView->m_WndMng.m_pClipedWnd[i]->GetWindowRect( TRUE );

					CRect rectT( rect.left, rect.top - nThick, rect.right, rect.top );
					CRect rectB( rect.left, rect.bottom, rect.right, rect.bottom + nThick );
					CRect rectL( rect.left - nThick, rect.top, rect.left, rect.bottom );
					CRect rectR( rect.right, rect.top, rect.right + nThick, rect.bottom );
					CRect rectLT( rect.left  - nThick, rect.top - nThick, rect.left, rect.top );
					CRect rectRT( rect.right, rect.top - nThick, rect.right + nThick, rect.top );
					CRect rectLB( rect.left - nThick, rect.bottom, rect.left, rect.bottom + nThick );
					CRect rectRB( rect.right, rect.bottom, rect.right + nThick, rect.bottom + nThick );
					
					p2DRender->RenderFillRect( rectT, 0xaf38b0de );
					p2DRender->RenderFillRect( rectB, 0xaf38b0de );
					p2DRender->RenderFillRect( rectL, 0xaf38b0de );
					p2DRender->RenderFillRect( rectR, 0xaf38b0de );
					p2DRender->RenderFillRect( rectLT, 0xff8080ff );
					p2DRender->RenderFillRect( rectRT, 0xff8080ff );
					p2DRender->RenderFillRect( rectLB, 0xff8080ff );
					p2DRender->RenderFillRect( rectRB, 0xff8080ff );
				}
			}
		}
#endif //__DAISY_UPDATE_1
		/*
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0x80808080 );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0xaf808080 );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0x80808080 );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0xaf808080 );
*/
		LPWNDCTRL lpWndCtrl = CWndBase::m_resMng.GetAtControl( GetWndId(), m_pWndFocusChild->GetWndId() );
		if( lpWndCtrl )
		{
			if( lpWndCtrl->rect != m_pWndFocusChild->GetWindowRect( TRUE ) )
			{
#ifdef __DAISY_UPDATE_3
				if(CWndBase::m_resMng.m_nChangeItem == -1)
				{
					//lpWndCtrl->dwWndHistoryType = CWndBase::m_resMng.CTRL_CHANGE;
					memcpy(&CWndBase::m_resMng.m_OldWndCtrl, lpWndCtrl, sizeof(WNDCTRL));
					CWndBase::m_resMng.m_nChangeItem = CWndBase::m_resMng.CTRL_CHANGE;
				}
#endif //__DAISY_UPDATE_3

				CWndBase::m_resMng.SetModifiedScript( TRUE );
				lpWndCtrl->rect = m_pWndFocusChild->GetWindowRect( TRUE );
			}
		}
	}
// 	CRect rect = GetWindowRect();
// 	if( m_lpWndApplet->size.cx != rect.Width() || m_lpWndApplet->size.cy != rect.Height() )
// 	{
// 		m_lpWndApplet->nDefSizeX = m_lpWndApplet->size.cx - rect.Width();
// 		m_lpWndApplet->nDefSizeY = m_lpWndApplet->size.cy - rect.Height();
// 		CWndBase::m_resMng.SetModifiedScript( TRUE );
// 		m_lpWndApplet->size.cx = rect.Width();
// 		m_lpWndApplet->size.cy = rect.Height();
// 	}
}
void CWndDialog::OnInitialUpdate()
{
	/*
	m_wndMusic.Create( "Music", BBS_CHECK, CRect( 0,  0, 100, 20 ), this, 100 ); 
	m_wndSound.Create( "Sound", BBS_CHECK, CRect( 0, 25, 100, 50 ), this, 101 ); 
	//m_wndMusic.SetPushColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	//m_wndMusic.SetFontColor( D3DCOLOR_ARGB( 255, 000, 000, 255 ) );
	//m_wndSound.SetPushColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	//m_wndSound.SetFontColor( D3DCOLOR_ARGB( 255, 000, 000, 255 ) );
	//m_wndMusic.SetCheck( g_Music.m_nMusicVolume );
	//m_wndSound.SetCheck( g_Sound.m_nSoundVolume );
	m_wndComboBox.Create( 0, CRect( 1, 40, 200, 60 ), this, 10 );
	m_wndlistCtrl.Create( WLVS_ICON, CRect( 1, 65, 200, 200 ), this, 11 );
	m_wndButtonIcon.Create  ( "Icon"  , BBS_RADIO, CRect(   1, 210,  50, 230 ), this, 200 );
	m_wndButtonReport.Create( "Report", BBS_RADIO, CRect(  51, 210, 100, 230 ), this, 201 );
	m_wndButtonList.Create  ( "List"  , BBS_RADIO, CRect( 101, 210, 150, 230 ), this, 202 );
	m_wndButtonIcon.SetGroup( TRUE );

	TCHAR	szTitle[3][10] = {_T("File Name"), _T("Id"), _T("최대")};
	
	LV_COLUMN  pColumn;
	LV_ITEM	   lvitem;
	TCHAR      strNum[10];
	//
	// Report Column을 입력 
	//
	for(int i = 0;  i < 3; i++)
	{
		pColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		pColumn.fmt = LVCFMT_LEFT;
		pColumn.iSubItem = i;
		pColumn.cx = 60;  // SubItem is twice as large
		pColumn.pszText = szTitle[i];	
		m_wndlistCtrl.InsertColumn(i, &pColumn);  // assumes return value is OK.
	}
	//
	// 소스 아이템을 입력
	//
	for(i = 0; i <  256; i++) 
	{
		
		LPTERRAIN lpTerrain = prj.m_terrainMng.GetTerrain( i );
		if( lpTerrain->IsValid() )
		{
			lvitem.mask = LVIF_TEXT | LVIF_PARAM; // | (iSubItem == 0? LVIF_IMAGE : 0);
			lvitem.iItem = m_wndlistCtrl.GetItemCount(); //(iSubItem == 0)? iItem : iActualItem;
			lvitem.iSubItem = 0;
			lvitem.lParam = (LPARAM) lpTerrain;
			lvitem.pszText = lpTerrain->m_szTextureFileName; 
			m_wndlistCtrl.InsertItem(&lvitem);

			lvitem.mask = LVIF_TEXT;
			lvitem.iSubItem = 1; 
			_itoa(lpTerrain->m_dwId,strNum,10);
			lvitem.pszText = strNum;
			m_wndlistCtrl.SetItem(&lvitem);

			lvitem.iSubItem = 2; 
			//_itoa(pItemProp->m_nMax,strNum,10);
			lvitem.pszText = "b";
			m_wndlistCtrl.SetItem(&lvitem);
		}
		
	}

	CRect rect( 210,0,300,100);
	m_wndText.Create(WBS_CHILD,rect,this,10);
	rect = m_wndText.GetClientRect();
	m_wndText.m_string.SetString( _T("나는\n 자랑스러운\n 태극기 앞에 조국과 민족의 무궁한 영광을 위하여 몸과 마음을 바쳐 충성을 다할 것을 굳게 다짐합니다.\n 할랠루야 ") );
	m_wndText.m_string.Reset( m_pFont, &rect );
*/
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndDialog::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect( 0, 0, 300, 300 );
	SetTitle( _T( "Applet" ) );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, NULL, dwWndId);
}
BOOL CWndDialog::OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult )
{

	return CWndNeuz::OnChildNotify( nCode, nID, pLResult );
}
BOOL CWndDialog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 100: // wnd2
			break;
		case 101: // wnd1 
//			g_Sound.m_nSoundVolume = !g_Sound.m_nSoundVolume;
			break;
		case 200: // icon
			m_wndlistCtrl.m_dwListCtrlStyle = WLVS_ICON;
			m_wndlistCtrl.SetWndRect( m_wndlistCtrl.GetWindowRect( TRUE ) );
			break;
		case 201: // report
			m_wndlistCtrl.m_dwListCtrlStyle = WLVS_REPORT;
			m_wndlistCtrl.SetWndRect( m_wndlistCtrl.GetWindowRect( TRUE ) );
			break;
	}
	*/
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndDialog::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::m_resMng.SetModifiedScript( TRUE );
	CWndNeuz::OnSize(nType,cx,cy);
}

void CWndDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_nResizeDir = 0;
}
void CWndDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
		// 1 = top
		// 2 = bottom
		// 3 = left
		// 4 = right
		// 5 = topLeft
		// 6 = topRight
		// 7 = bottomLeft
		// 8 = bottomRigh;
	if( m_pWndFocusChild && m_pWndFocusChild->IsWndStyle( WBS_DOCKING ) == FALSE )
	{
		CRect rect = m_pWndFocusChild->GetWindowRect( TRUE );
		m_nResizeDir = GetResizeDir( rect, point );
		m_pointOld = point;// - rect.TopLeft();
		m_rectOld = rect;
	}
	CWndManager::m_pWndFocusDialog = this;
	SetCapture();
}
int CWndDialog::GetResizeDir( CRect rect,CPoint ptClient)
{
	// 1 = top
	// 2 = bottom
	// 3 = left
	// 4 = right
	// 5 = topLeft
	// 6 = topRight
	// 7 = bottomLeft
	// 8 = bottomRigh;

	if( rect.PtInRect( ptClient ) == TRUE )
		return 0;
	rect.InflateRect(5,5);
	if( rect.PtInRect( ptClient ) == FALSE )
		return 0;
	rect.DeflateRect(5,5);
	
	int nDir = 0;
	if(ptClient.x <= rect.left && ptClient.y <= rect.top)
		return 5;
	if(ptClient.x >= rect.right && ptClient.y <= rect.top)
		return 6;
	if(ptClient.x <= rect.left && ptClient.y >= rect.bottom)
		return 7;
	if(ptClient.x >= rect.right && ptClient.y >= rect.bottom)
		return 8;

	if(ptClient.y <= rect.top)
		return 1;
	if(ptClient.y >= rect.bottom)
		return 2;
	if(ptClient.x <= rect.left)
		return 3;
	if(ptClient.x >= rect.right)
		return 4;
	return nDir;
}
void CWndDialog::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_pWndFocusChild )
	{
		if( m_nResizeDir )
		{
			CRect rectWnd = m_rectOld;//m_pWndFocusChild->GetWindowRect( TRUE );
			//CPoint pt = point - m_pointOld;
			CPoint pt = point - m_pointOld;

			switch( m_nResizeDir )
			{
			case 1: rectWnd.top    += pt.y; break; 
			case 2: rectWnd.bottom += pt.y; break;
			case 3: rectWnd.left   += pt.x; break;
			case 4: rectWnd.right  += pt.x; break;
			case 5: rectWnd.left  += pt.x; rectWnd.top += pt.y; break; 
			case 6: rectWnd.right += pt.x; rectWnd.top += pt.y; break;
			case 7: rectWnd.left  += pt.x; rectWnd.bottom += pt.y; break;
			case 8: rectWnd.right += pt.x; rectWnd.bottom += pt.y; break;
			}
			if( CWndBase::m_nLayout == LAYOUT_GRID )
			{
				rectWnd.top    = ( rectWnd.top    / CWndBase::m_nGridWidth  ) * CWndBase::m_nGridWidth;
				rectWnd.bottom = ( rectWnd.bottom / CWndBase::m_nGridHeight ) * CWndBase::m_nGridHeight;
				rectWnd.left   = ( rectWnd.left   / CWndBase::m_nGridWidth  ) * CWndBase::m_nGridWidth;
				rectWnd.right  = ( rectWnd.right  / CWndBase::m_nGridHeight ) * CWndBase::m_nGridHeight;
			}
			/*
			if( m_bCling ) //&& !IsWndStyle( WBS_NOCLING ) )
			{
				CRect rect = GetLayoutRect();
				if( rectWnd.top < rect.top + 10 && rectWnd.top > rect.top ) rectWnd.top = rect.top;
				if( rectWnd.bottom > rect.bottom - 10 && rectWnd.bottom < rect.bottom ) rectWnd.bottom = rect.bottom;
				if( rectWnd.left < rect.left + 10 && rectWnd.left > rect.left ) rectWnd.left = rect.left;
				if( rectWnd.right > rect.right - 10 && rectWnd.right < rect.right) rectWnd.right = rect.right;
			}
			*/
			//m_pointOld = point;// - rectWnd.TopLeft();
			m_pWndFocusChild->SetWndRect( rectWnd );
			CWndBase::m_resMng.SetModifiedScript( TRUE );
			AdjustWndBase();
		}
	}
}
void CWndDialog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CWndDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( m_pWndFocusChild )
	{
		CRect rectWnd = m_pWndFocusChild->GetWindowRect( TRUE );
		switch( nChar )
		{
		case VK_DELETE:
	//		m_pWndFocusChild->Destroy( TRUE );

			break;
		case VK_UP:
			rectWnd.OffsetRect( 0, -CWndBase::m_nGridHeight );
			m_pWndFocusChild->Move( rectWnd.TopLeft() );
			break;
		case VK_DOWN:
			rectWnd.OffsetRect( 0, CWndBase::m_nGridHeight );
			m_pWndFocusChild->Move( rectWnd.TopLeft() );
			break;
		case VK_LEFT:
			rectWnd.OffsetRect( -CWndBase::m_nGridWidth, 0 );
			m_pWndFocusChild->Move( rectWnd.TopLeft() );
			break;
		case VK_RIGHT:
			rectWnd.OffsetRect( CWndBase::m_nGridWidth, 0 );
			m_pWndFocusChild->Move( rectWnd.TopLeft() );
			break;
		}
		CWndManager::m_pWndFocusDialog->AdjustWndBase();
		((CDaisyView*)m_pApp)->Invalidate( TRUE );
	}
}
BOOL CWndDialog::IsPickupSpace( CPoint ptWindow )
{
	if( m_nResizeDir == 0 )
		return CWndBase::IsPickupSpace( ptWindow );
	return FALSE;
}
