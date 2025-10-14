// DlgNavigatorMapView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Beast.h"
#include "DlgNavigatorMapView.h"
#include "WorldView.h"
#include "squish.h"


// CDlgNavigatorMapView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgNavigatorMapView, CDialog)

CDlgNavigatorMapView::CDlgNavigatorMapView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNavigatorMapView::IDD, pParent)
	, m_vPos(0, 0, 0)
	, m_pWorld(NULL)
	, m_pWorldView(NULL)
	, m_TexInfo(NULL)
{
}

CDlgNavigatorMapView::~CDlgNavigatorMapView()
{	
	DeleteBitmap( m_bmpNavi );
	DeleteDC( m_memDC );

}

void CDlgNavigatorMapView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNavigatorMapView, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CDlgNavigatorMapView 메시지 처리기입니다.


BOOL CDlgNavigatorMapView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	int CaptionHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
	// 
 	CClientDC cdc(this);	
 
 	m_memDC.CreateCompatibleDC( &cdc);
 	m_bmpNavi.CreateCompatibleBitmap( &cdc, TexSIZEX, TexSIZEY - CaptionHeight );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgNavigatorMapView::OnPaint()
{
	if( m_pWorld == NULL )
		return;

	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	CRect rect, ClientRect;
	GetClientRect( &ClientRect );
	rect = ClientRect;

	int CaptionHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);

	CPen pen   ( PS_SOLID, 1, (COLORREF )0x00c0c0c0 );
	CPen penPos ( PS_SOLID, 2, (COLORREF )0x00ff00ff );

	CPen* pPen = dc.SelectObject( &pen  );

	float fWidth = (float)rect.Width() / (float)m_pWorld->m_nLandWidth;
	float fHeight =(float)rect.Height() / (float)m_pWorld->m_nLandHeight;

	m_memDC.SetStretchBltMode(HALFTONE);
	dc.StretchBlt( 0, 0, ClientRect.Width(), ClientRect.Height(), &m_memDC, 0,  0, TexSIZEX, TexSIZEY - CaptionHeight, SRCCOPY );

	for( int y = 0; y < m_pWorld->m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_pWorld->m_nLandWidth; x++ )
		{
			rect.SetRect( (int)( x * fWidth ), (int)( y * fHeight ), (int)( ( x * fWidth ) + fWidth + 1 ), (int)( ( y * fHeight ) + fHeight + 1 ) );
			dc.SelectObject( (HBRUSH)GetStockObject( NULL_BRUSH )  );
			dc.Rectangle(  rect );
		}
	}
	rect = ClientRect;
	rect.left = (LONG)( m_pWorldView->m_camera.m_vPos.x * rect.Width() / ( m_pWorld->m_nLandWidth * LANDREALSCALE ) );
	rect.right = rect.left + 2;
	rect.top = (LONG)( m_pWorldView->m_camera.m_vPos.z * rect.Height() / ( m_pWorld->m_nLandHeight * LANDREALSCALE ) );
	rect.top = ClientRect.Height() - rect.top ;
	rect.bottom = rect.top + 2;
	rect.OffsetRect( -1, -1 );

	dc.SelectObject( &penPos  );
	dc.Rectangle(  rect );

	dc.SelectObject( pPen  );
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}

void CDlgNavigatorMapView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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

//네비게이션용 파일 로딩
void CDlgNavigatorMapView::LoadNaviMapTexture( )
{	
	CRect rect, ClientRect;
	GetClientRect( &ClientRect );
	rect = ClientRect;

	float fWidth = (float)rect.Width() / (float)m_pWorld->m_nLandWidth;
	float fHeight =(float)rect.Height() / (float)m_pWorld->m_nLandHeight;

	m_TexInfo = new NAVIMAPINFO[ (m_pWorld->m_nLandWidth) * (m_pWorld->m_nLandHeight) ];

	for( int y = 0; y < m_pWorld->m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_pWorld->m_nLandWidth; x++ )
		{

			int iIndex =  y * m_pWorld->m_nLandWidth + x;
			rect.SetRect( (int)( x * fWidth ), (int)( y * fHeight ), (int)( ( x * fWidth ) + fWidth + 1 ), (int)( ( y * fHeight ) + fHeight + 1 ) );
		
 			CString strLandName = m_pWorld->m_szFileName;
			strLandName.Delete( strLandName.GetLength() - 4, 4 );
			CString FileName;
			FileName.Format( "%s%s%02d-%02d.dds", m_pWorld->m_szFilePath, strLandName, x, ( m_pWorld->m_nLandHeight - y - 1 ) );	

			LPDIRECT3DTEXTURE9	pTex = NULL;
			HRESULT	hr	= E_FAIL;
			hr = D3DXCreateTextureFromFileEx( m_pWorldView->m_pd3dDevice
				, FileName 
				, D3DX_DEFAULT
				, D3DX_DEFAULT
				, 1
				, 0
				, D3DFMT_UNKNOWN
				, D3DPOOL_MANAGED
				, D3DX_FILTER_NONE
				, D3DX_FILTER_NONE
				, 0xFF000000
				, &m_TexInfo[ iIndex ].ImageInfo
				, NULL
				, &pTex );

			if( FAILED(hr) )
			{
				pTex = NULL;
				m_TexInfo[ iIndex ].lpvBits = NULL;
				m_TexInfo[ iIndex ].bRender = FALSE;
			}
			else
 			{				
				//	LPDIRECT3DTEXTURE9 pTestTex;	 
				D3DLOCKED_RECT	LockRt;
				D3DSURFACE_DESC	ScrDesc;

				pTex->GetLevelDesc(0, &ScrDesc);
				//텍스쳐에 LockRect를 해주면 텍스쳐의 픽셀정보를 얻어올 수 있다
				if( pTex->LockRect(0, &LockRt, 0, 0) == D3D_OK )
				{
					int nByte = LockRt.Pitch / ScrDesc.Width;

					int iWidth = ScrDesc.Width;
					int iHeight = ScrDesc.Height;

					int iWidthBytes;

					m_TexInfo[ iIndex ].lpvBits = new BYTE[ iWidth * iHeight * 4 ];				

					//비트맵 구조체 정보를 채워준다
					//ZeroMemory( &bmi, sizeof(bmi) );
					m_TexInfo[iIndex].bmi.bmiHeader.biSize		=	sizeof(BITMAPINFOHEADER);
					m_TexInfo[iIndex].bmi.bmiHeader.biPlanes		=	1;
					m_TexInfo[iIndex].bmi.bmiHeader.biWidth		=	iWidth;
					m_TexInfo[iIndex].bmi.bmiHeader.biHeight		=	-iHeight;
					m_TexInfo[iIndex].bmi.bmiHeader.biBitCount	=	32;
					m_TexInfo[iIndex].bmi.bmiHeader.biCompression	= BI_RGB;

					if( m_TexInfo[ iIndex ].ImageInfo.Format == D3DFMT_DXT1 )
					{
						BYTE* tempBits = new BYTE[ iWidth * iHeight * 4 ];
						squish::DecompressImage( tempBits, iWidth, iHeight, LockRt.pBits, squish::kDxt1 ); 
						iWidthBytes = iWidth * 4;
						// tempBits 에 저장된 픽셀을 m_lpvBits 에 대입(RGBA -> BGRA로 바꿔줌)
						for( int i = 0; i < iHeight; ++i )
						{
							for( int j = 0; j < iWidthBytes; j += 4 )
							{
								m_TexInfo[ iIndex ].lpvBits[ ( i * iWidthBytes + j ) + 0 ] = tempBits[ ( i  * iWidthBytes + j) + 2 ];
								m_TexInfo[ iIndex ].lpvBits[ ( i * iWidthBytes + j ) + 1 ] = tempBits[ ( i  * iWidthBytes + j) + 1 ];
								m_TexInfo[ iIndex ].lpvBits[ ( i * iWidthBytes + j ) + 2 ] = tempBits[ ( i  * iWidthBytes + j) + 0 ];
								m_TexInfo[ iIndex ].lpvBits[ ( i * iWidthBytes + j ) + 3 ] = tempBits[ ( i  * iWidthBytes + j) + 3 ];
							}
						}
						SAFE_DELETE_ARRAY(tempBits);	
					}
					else
					{
						if( m_TexInfo[iIndex].ImageInfo.Format == D3DFMT_A8R8G8B8 )
							iWidthBytes = iWidth * 4;

						else if( m_TexInfo[iIndex].ImageInfo.Format == D3DFMT_A1R5G5B5 )
						{
							m_TexInfo[iIndex].bmi.bmiHeader.biBitCount = 16;
							iWidthBytes = iWidth * 2;
						}

						for( int i = 0; i < iHeight; ++i )
						{
							for( int j = 0; j < iWidthBytes; ++j )
							{
								m_TexInfo[ iIndex ].lpvBits[ i * iWidthBytes + j ] = ((BYTE*)LockRt.pBits)[ i  * iWidthBytes + j ];
							}
						}	
					}				
					pTex->UnlockRect(0);
					m_TexInfo[ iIndex ].bRender = TRUE;
				}
			}
			SAFE_RELEASE( pTex );
		}
	}	
	CreateNaviTex();
}

//네비게이터용 텍스쳐생성
void CDlgNavigatorMapView::CreateNaviTex()
{
	int CaptionHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
	CRect rect(0, 0, TexSIZEX, TexSIZEY - CaptionHeight );

	m_memDC.SelectObject( &m_bmpNavi );

	CPen pen   ( PS_SOLID, 1, (COLORREF )0x00808080 );
	CPen* pPen = m_memDC.SelectObject( &pen  );
	CBrush brushLand1( 0x00808080 );

	float fWidth = (float)rect.Width() / (float)m_pWorld->m_nLandWidth;
	float fHeight =(float)rect.Height() / (float)m_pWorld->m_nLandHeight;

	for( int y = 0; y < m_pWorld->m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_pWorld->m_nLandWidth; x++ )
		{
			int iIndex =  y * m_pWorld->m_nLandWidth + x;
			rect.SetRect( (int)( x * fWidth ), (int)( y * fHeight ), (int)( ( x * fWidth ) + fWidth + 1 ), (int)( ( y * fHeight ) + fHeight + 1 ) );
			
			m_memDC.SetStretchBltMode(HALFTONE);
			if( m_TexInfo[ iIndex ].bRender )
			{
				StretchDIBits(m_memDC.m_hDC
					, rect.left
					, rect.top
					, rect.Width() 
					, rect.Height()
					, 0
					, 0
					, m_TexInfo[ iIndex ].ImageInfo.Width
					, m_TexInfo[ iIndex ].ImageInfo.Height
					, m_TexInfo[ iIndex ].lpvBits
					, &(m_TexInfo[ iIndex ].bmi)
					, DIB_RGB_COLORS
					, SRCCOPY ); 			
			}
			else
			{
				m_memDC.SelectObject( &brushLand1 );
				m_memDC.Rectangle(  rect );	
			}					
		}
	}

	for( int y = 0; y < m_pWorld->m_nLandHeight * m_pWorld->m_nLandWidth; y++ )
		SAFE_DELETE_ARRAY(m_TexInfo[y].lpvBits);		

	SAFE_DELETE_ARRAY(m_TexInfo);
}



void CDlgNavigatorMapView::Update(  CWorld* pWorld, CWorldView* pWorldView  )
{
	m_pWorld		= pWorld;
	m_pWorldView	= pWorldView;
	Invalidate();
}

void CDlgNavigatorMapView::UpdatePos()
{
	if( !m_pWorldView )
		return;

	if( m_vPos != m_pWorldView->m_camera.m_vPos )
		Invalidate();
	m_vPos = m_pWorldView->m_camera.m_vPos;
}

void CDlgNavigatorMapView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	Invalidate();
}

