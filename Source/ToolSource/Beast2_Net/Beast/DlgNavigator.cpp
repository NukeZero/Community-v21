// DlgNavigator.cpp : implementation file
//

#include "stdafx.h"
#include "beast.h"
#include "DlgNavigator.h"
#include "WorldDoc.h"
#include "WorldView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNavigator dialog


CDlgNavigator::CDlgNavigator(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNavigator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNavigator)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pWorld = NULL;
}


void CDlgNavigator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNavigator)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CDlgNavigator::Update( CWorld* pWorld, CWorldView* pWorldView )
{
	m_pWorld = pWorld;
	m_pWorldView = pWorldView;
	Invalidate();

}
void CDlgNavigator::UpdatePos()
{
	if( !m_pWorldView )
		return;

	if( m_vPos != m_pWorldView->m_camera.m_vPos )
		Invalidate();
	m_vPos = m_pWorldView->m_camera.m_vPos;
}
BEGIN_MESSAGE_MAP(CDlgNavigator, CDialog)
	//{{AFX_MSG_MAP(CDlgNavigator)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNavigator message handlers

void CDlgNavigator::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if( m_pWorld == NULL )
		return;

	CRect rect, rectClient;
	GetClientRect( &rectClient );
	rect = rectClient;

	CPen pen    ( PS_SOLID, 1, (COLORREF )0x00ffffff );
	CPen penPos ( PS_SOLID, 2, (COLORREF )0x00ff0000 );
	CBrush brushLand1( 0x00808080 );
	CBrush brushLand2( 0x00d0d0d0 );
	
	CPen* pPen = dc.SelectObject( &pen  );

/*
	int nWidth = m_pWorld->m_nLandWidth * 16;
	int nHeight = m_pWorld->m_nLandHeight * 16;
	int nNewWidth, nNewHeight;

	nNewHeight = nHeight * rect.Height() / nHeight;
	nNewWidth = nWidth * nNewHeight / nHeight ;
*/

	float fWidth = (float)rect.Width() / (float)m_pWorld->m_nLandWidth;
	float fHeight =(float)rect.Height() / (float)m_pWorld->m_nLandHeight;

	for( int y = 0; y < m_pWorld->m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_pWorld->m_nLandWidth; x++ )
		{
			rect.SetRect( (int)( x * fWidth ), (int)( y * fHeight ), (int)( ( x * fWidth ) + fWidth + 1 ), (int)( ( y * fHeight ) + fHeight + 1 ) );
			if( m_pWorld->GetLandscape( x, m_pWorld->m_nLandHeight - y - 1 ) )
				dc.SelectObject( &brushLand1  );
			else
				dc.SelectObject( &brushLand2 );
			dc.Rectangle(  rect );
		}
	}
	// 1024 : pos.x = clientWidth : x 
	rect = rectClient;
	rect.left = (LONG)( m_pWorldView->m_camera.m_vPos.x * rect.Width() / ( m_pWorld->m_nLandWidth * LANDREALSCALE ) );
	rect.right = rect.left + 2;
	rect.top = (LONG)( m_pWorldView->m_camera.m_vPos.z * rect.Height() / ( m_pWorld->m_nLandHeight * LANDREALSCALE ) );
	rect.top = rectClient.Height() - rect.top ;
	rect.bottom = rect.top + 2;
	rect.OffsetRect( -1, -1 );

	dc.SelectObject( &penPos  );
	dc.Rectangle(  rect );

	dc.SelectObject( pPen  );

	

	// Do not call CDialog::OnPaint() for painting messages
}



void CDlgNavigator::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	Invalidate();
}

void CDlgNavigator::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if( m_pWorld == NULL )
		return;

	CRect rect;
	GetClientRect( &rect );

	float fWidth = (float)rect.Width() / (float)m_pWorld->m_nLandWidth;
	float fHeight =(float)rect.Height() / (float)m_pWorld->m_nLandHeight;

	for( int y = 0; y < m_pWorld->m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_pWorld->m_nLandWidth; x++ )
		{
			rect.SetRect( (int)( x * fWidth ), (int)( y * fHeight ), (int)( ( x * fWidth ) + fWidth + 1 ), (int)( ( y * fHeight ) + fHeight + 1 ) );
			if( rect.PtInRect( point ) )
			{ 
				// 탑뷰로 만들기 
				m_pWorldView->m_camera.m_fYaw = 0;
				m_pWorldView->m_camera.m_fPitch = D3DX_PI/2;
				// fWidth : point.x = LANDREALSCALE : ?
				m_pWorldView->m_camera.m_vPos.x = point.x * LANDREALSCALE / fWidth;
				m_pWorldView->m_camera.m_vPos.z = point.y * LANDREALSCALE / fHeight;// ( ( m_pWorld->m_nLandHeight - y - 1 ) * LANDREALSCALE ) - ( point.y - rect.top );
				m_pWorldView->m_camera.m_vPos.z = ( LANDREALSCALE * m_pWorld->m_nLandHeight ) - m_pWorldView->m_camera.m_vPos.z;
				m_pWorldView->m_camera.m_vPos.y = 500.0f;
				//m_pWorldView->m_camera.m_vPos.y = m_pWorld->GetLandHeight( m_pWorldView->m_camera.m_vPos ) + 100.0f;
						//RenderScene();
				m_pWorldView->Invalidate();
				Invalidate();
			}
		}
	}
	
	CDialog::OnLButtonDblClk(nFlags, point);
}
