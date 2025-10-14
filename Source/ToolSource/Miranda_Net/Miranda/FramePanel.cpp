// FramePanel.cpp : implementation file
//

#include "stdafx.h"
#include "Miranda.h"
#include "FramePanel.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFramePanel dialog


CFramePanel::CFramePanel()
{
	//{{AFX_DATA_INIT(CFramePanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc  = NULL;
	m_pView = NULL;
}


void CFramePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFramePanel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFramePanel, CDialogBar)
	//{{AFX_MSG_MAP(CFramePanel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

CMirandaDoc* CFramePanel::GetDoc()
{

	return (CMirandaDoc*)((CMainFrame *)AfxGetMainWnd())->GetActiveDocument();

}


CMirandaView* CFramePanel::GetView()
{

	return (CMirandaView*)((CMainFrame *)AfxGetMainWnd())->GetActiveView();

}

int	CFramePanel::ChangeSelection(int nIdx)
{

	if(m_pDoc == NULL) return -1;

	// 현재 프레임 번호를 바꾼다
	if(m_pDoc->m_Selected.nType == TYPE_OBJECT)
	{
		if(nIdx > m_pDoc->m_ObjMgr.GetMaxFrame(m_pDoc->m_Selected.nID))
			return -1;
		else
		{
			m_pDoc->m_ObjMgr.SetFrame(m_pDoc->m_Selected.nID, nIdx);
			return m_pDoc->m_ObjMgr.GetCurrentFrame(m_pDoc->m_Selected.nID);
		}
	}
	else if(m_pDoc->m_Selected.nType == TYPE_SFX)
	{
		if(nIdx > m_pDoc->m_SfxMgr.GetMaxFrame(m_pDoc->m_Selected.nID))
			return -1;
		else
		{
			m_pDoc->m_SfxMgr.SetFrame(m_pDoc->m_Selected.nID, nIdx);		
			return m_pDoc->m_SfxMgr.GetCurrentFrame(m_pDoc->m_Selected.nID);
		}
	}
	else
		return -1;
}


int	CFramePanel::FindSelection(CPoint point)
{

	int nResult = 0;
	int nX = point.x;
	int nY = point.y;
	CRect rect;

	
	// 일단 범위 안에 있는지 체크 
	rect.SetRect(68, 20, 919, 123);
	if(rect.PtInRect(point))
	{
		int nCount = ((nY - 20) / 17);
		int nRest  = ((nX - 68) / 17);

		nResult= nCount * 50 + nRest;

		return nResult;
	}
	else
		return 0;

}

/////////////////////////////////////////////////////////////////////////////
// CFramePanel message handlers
LONG CFramePanel::OnInitDialog(UINT wParam, LONG lParam) 
{
#ifdef __VS6
	if(!HandleInitDialog(wParam, lParam) || !UpdateData(FALSE))
	{
		return FALSE;
	}
#else
	//gmpbigsun: for VS2003
	HandleInitDialog( wParam, lParam );

	if( !UpdateData( FALSE ) )
	{
		TRACE0("Warning:UpdateData failed during dialog init. \n" );
		return FALSE;
	}
#endif

	// TODO: Add extra initialization here
	// 업데이트 타이머 세팅 
	SetTimer(RENDER_TIMER, 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFramePanel::OnPaint() 
{
	
	CPaintDC dc(this); // device context for painting
	m_pDoc  = GetDoc();
	m_pView = GetView();

	
	// TODO: Add your message handler code here
	RECT	rect;	
	GetClientRect( &rect );
	int nWidth  = rect.right-rect.left;
	int nHeight = rect.bottom-rect.top;
	int nX,nY;
	int nSelectedF = 0;
	int nMaxF	   = 0;
	CString			szNum;

	
	if(m_pDoc->m_Selected.nType == TYPE_OBJECT)
	{
		nSelectedF  = m_pDoc->m_ObjMgr.GetCurrentFrame(m_pDoc->m_Selected.nID);
		nMaxF		= m_pDoc->m_ObjMgr.GetMaxFrame(m_pDoc->m_Selected.nID) - 1;
		if(nSelectedF > nMaxF) nSelectedF = nMaxF;
	}
	else if(m_pDoc->m_Selected.nType == TYPE_SFX)
	{
		nSelectedF  = m_pDoc->m_SfxMgr.GetCurrentFrame(m_pDoc->m_Selected.nID);
		nMaxF	    = m_pDoc->m_SfxMgr.GetMaxFrame(m_pDoc->m_Selected.nID);
		if(nSelectedF > nMaxF) nSelectedF = nMaxF;
	}

	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	CBitmap *pOldBitmap;
	pOldBitmap = memDC.SelectObject( &m_hBitmap );

	// 바탕색 채우기 
	CDC	*pDC = &memDC;
	pDC->FillSolidRect(0, 0, nWidth, nHeight, 0x0efefef);

	// 폰트, 펜, 브러쉬 세팅
	CFont font, *pOldFont;
	font.CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE,
					 FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                     VARIABLE_PITCH, "굴림" );
	pOldFont = pDC->SelectObject( &font );
	
	CPen pen1, pen2, *pOldPen;
	pen1.CreatePen( PS_SOLID, 1, RGB(170, 170, 170) );
	pen2.CreatePen( PS_SOLID, 1, RGB(50, 50, 50) );
	pOldPen = pDC->SelectObject( &pen1 );

	CBrush brush1, brush2, *pOldBrush;
	brush1.CreateSolidBrush(RGB(200, 200, 200));
	brush2.CreateSolidBrush(RGB(50, 50, 50));
	pOldBrush = pDC->SelectObject( &brush1 );
	
	pDC->SetBkMode(TRANSPARENT);
	int nCountX = 0, nCountY = 0;
	
	// 맥스프레임까지 색 채우기
	if(nMaxF > 0)
	{
		RECT rect;
		int nCount = nMaxF / 50;
		int nRest  = nMaxF % 50;
		for(nX = 0; nX < nCount; nX++)
		{
			rect.top = 20 + 17 * nX; rect.bottom = rect.top + 17 ; rect.left = 68; rect.right = 919;
			pDC->FillRect( &rect ,&brush1);
		}
		
		rect.top = 20 + 17 * nX; rect.bottom = rect.top + 17 ; rect.left = 68; rect.right = nRest * 17 + rect.left + 17;
		pDC->FillRect( &rect ,&brush1);
		
	}
	
	// 외곽선 그리기 
	pDC->MoveTo( 20, 20 );
	pDC->LineTo( 20, nHeight - 11 );

	for(nX = 68; nCountX < 51; nX += 17)
	{
		if(nCountX%5) pOldPen = pDC->SelectObject( &pen1 );
		else		  pOldPen = pDC->SelectObject( &pen2 );
		pDC->MoveTo( nX, 20 );
		pDC->LineTo( nX, nHeight - 11 );
		nCountX++;
	}
	pOldPen = pDC->SelectObject( &pen1 );
	for(nY = 20; nCountY < 7; nY += 17)
	{
		pDC->MoveTo( 20, nY );
		pDC->LineTo( 919, nY );
		nCountY++;
	}
	// 프레임 숫자 넣기 
	nCountY = 0, nCountX = 0;
	for(nY = 23; nCountY < 300 ; nY += 17)
	{
		szNum.Format("%d",nCountY);
		pDC->TextOut( 25, nY,szNum);
		nCountY += 50;
	}
	for(nX = 71; nCountX <= 50 ; nX += 85)
	{
		szNum.Format("%d",nCountX);
		pDC->TextOut( nX, 4,szNum);
		nCountX += 5;
	}

	// 선택된 셀 채우기 
	if(nSelectedF > 0)
	{
		RECT rect;
		int nCount = nSelectedF / 50;
		int nRest  = nSelectedF % 50;
		
		rect.top = 20 + 17 * nCount; rect.bottom = rect.top + 17 ; rect.left = 68 + nRest * 17; rect.right = rect.left + 17;
		pDC->FillRect( &rect ,&brush2);
		
	}

	// 마침 
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldFont );

	dc.BitBlt(0, 0, nWidth, nHeight, pDC, 0, 0, SRCCOPY );
	memDC.SelectObject( pOldBitmap );
	// Do not call CDialogBar::OnPaint() for painting messages
}

BOOL CFramePanel::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
	
	return CDialogBar::OnEraseBkgnd(pDC);
}

void CFramePanel::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	RECT	rect;	
	GetClientRect( &rect );
	int nWidth  = rect.right-rect.left;
	int nHeight = rect.bottom-rect.top;

	CPaintDC dc(this);
	m_hBitmap.DeleteObject();
	m_hBitmap.CreateCompatibleBitmap( &dc, nWidth, nHeight );

	Invalidate(FALSE);
	// TODO: Add your message handler code here
	
}

void CFramePanel::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
		case RENDER_TIMER:
		Invalidate(FALSE);		
		break;
	}
	
	CDialogBar::OnTimer(nIDEvent);
}

void CFramePanel::OnLButtonUp(UINT nFlags, CPoint point) 
{

	// TODO: Add your message handler code here and/or call default
	int nResult = 0;


	nResult = FindSelection(point);

	if(nResult > 0) ChangeSelection(nResult);

	CDialogBar::OnLButtonUp(nFlags, point);
}

void CFramePanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	m_pDoc  = GetDoc();
	m_pView = GetView();


	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
			case VK_SPACE :
			// 스위칭 디플로이모드 / 시뮬모드 
			m_pView->m_bPlayMode = !m_pView->m_bPlayMode;
			break;
		case VK_SUBTRACT :
			if(m_pDoc->m_Selected.nType == TYPE_OBJECT) 
				 m_pDoc->m_ObjMgr.AddFrame(m_pDoc->m_Selected.nID, -1);
			else m_pDoc->m_SfxMgr.AddFrame(m_pDoc->m_Selected.nID, -1);
			break;

		case VK_ADD :
			if(m_pDoc->m_Selected.nType == TYPE_OBJECT) 
				 m_pDoc->m_ObjMgr.AddFrame(m_pDoc->m_Selected.nID, 1);
			else m_pDoc->m_SfxMgr.AddFrame(m_pDoc->m_Selected.nID, 1);
			break;
		
	}
	CDialogBar::OnKeyDown(nChar, nRepCnt, nFlags);
}
