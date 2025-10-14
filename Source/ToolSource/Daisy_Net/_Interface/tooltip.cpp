// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "DialogMsg.h"
extern CDialogMsg g_DialogMsg;


CToolTip::CToolTip()
{
	m_bReadyToopTip = FALSE;
	m_dwToolTipId   = 0;
	m_ptToolTip     = 0;
	m_ptOldToolTip  = 0;
	m_bPutToolTip = FALSE;
	m_bEnable = TRUE;
	m_nPosition = 0;
	m_bToolTip = FALSE;
	m_nAlpha = 255;
}
CToolTip::~CToolTip()
{
#ifdef __TOOLTIPTEXTURE
	Delete();
#endif // __TOOLTIPTEXTURE
}
#ifdef __TOOLTIPTEXTURE
void CToolTip::Delete()
{
	int nloadTexture = 0;
	for( int i = 0 ; i < MAX_TT ; ++i )
	{
		for( int j = 0 ; j < 9 ; ++ j )
		{
			m_apTextureToolTip[j].DeleteDeviceObjects();
			++nloadTexture;
		}
	}
}
#endif // __TOOLTIPTEXTURE

#ifdef __TOOLTIPTEXTURE
void CToolTip::InitTexture()
{
	CString szTextName;
	CString szTextNamebuf;
	szTextNamebuf = "WndTooltipTile";
	
	char szBuf[32];
	int nloadTexture = 0;
	for( int i = 0 ; i < MAX_TT ; ++i )
	{
		for( int j = 0 ; j < 9 ; ++ j )
		{
			szTextName = szTextNamebuf;
			sprintf( szBuf, "%02d", ( i * 10 ) + j );
			szTextName += szBuf;
//			szTextName += _itoa( ( i * 10 ) + j, szBuf, 10 );
			szTextName += ".tga";
			m_apTextureToolTip[nloadTexture].LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, szTextName ), 0xffff00ff, TRUE );
			++nloadTexture;
		}
	}
}
#endif // __TOOLTIPTEXTURE
/////////////////////////////////////////////////////////////////////////////
// Parameters :
// Return     :
// Remarks    :
//   현재 열려진 툴팁을 켄슬한다.(사라지게 한다.)
//
void CToolTip::CancelToolTip()
{
	if(m_bEnable == FALSE)
		return;
	m_bReadyToopTip = FALSE;
	//m_dwToolTipId = 0;
	m_bToolTip = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Func. Name : 
// Parameters :
// Return     :
// Remarks    :
//   툴팁을 입력한다. 0.5초가 지나면 알아서 출력한다.
//
void CToolTip::PutToolTip( DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos )
{
	PutToolTip( dwToolTipId, (LPCTSTR)string, rect, pt, nToolTipPos );
}	
void CToolTip::PutToolTip( DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos )
{
	if(m_bEnable == FALSE)
		return;
	if(!rect.PtInRect(pt))
		return;
//if(m_bReadyToopTip == FALSE && dwToolTipId != m_dwToolTipId)
	if( dwToolTipId != m_dwToolTipId || m_rect != rect )
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		//m_timerToopTip.Set(100);
		m_timerToopTip.Set(0);
		if(m_bToolTip)
			m_bToolTip = FALSE;
	}
#ifdef __V050714_BUGFIX_TOOLTIP
	m_strToolTip = lpszString;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender 
		= CRect( 
			0, 
			0, 
			size.cx + 6, 
			size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) 
		);
	m_rect = rect;
#else // __V050714_BUGFIX_TOOLTIP
//{{AFX
	int nMaxHeight = CWndBase::m_Theme.m_pFontText->GetMaxHeight();
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent( lpszString );
	int cx, cy;
	// 기본 사이즈 계산 
	if( size.cx >= 200 )
	{
		cy = size.cy + ( ( size.cx / 200 ) * nMaxHeight );
		cx = 200;
	}
	else
	{
		cx = size.cx;
		cy = size.cy;
	}
	m_rectRender 
		= CRect( 
			0, 
			0, 
			cx + 6, 
			cy + 3 + ( 2 * ( cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) 
		);
	m_rect = rect;
	//m_strToolTip.SetString( lpszString, 0xffffffff );

	m_strToolTip = "";
	m_strToolTip.Init( g_Neuz.m_2DRender.m_pFont, &CRect(0,0,cx,cy) );
	m_strToolTip.SetString( lpszString, 0xff000000 );

//}}AFX
#endif
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
}
void CToolTip::PutToolTip( DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos )
{
	if(m_bEnable == FALSE)
		return;
	if(!rect.PtInRect(pt))
		return;
//if(m_bReadyToopTip == FALSE && dwToolTipId != m_dwToolTipId)
	if( dwToolTipId != m_dwToolTipId)
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		//m_timerToopTip.Set(100);
		m_timerToopTip.Set(0);
		if(m_bToolTip)
			m_bToolTip = FALSE;
	}
#ifdef __V050714_BUGFIX_TOOLTIP
	m_strToolTip = string;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	m_rectRender 
		= CRect( 
			0, 
			0, 
			size.cx + 6, 
			size.cy + 3 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) 
		);
	m_rect = rect;
#else // __V050714_BUGFIX_TOOLTIP
//{{AFX
	int nMaxHeight = CWndBase::m_Theme.m_pFontText->GetMaxHeight();
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent( string );
	int cx, cy;
	// 기본 사이즈 계산 
	if( size.cx >= 200 )
	{
		cy = size.cy + ( ( size.cx / 200 ) * nMaxHeight );
		cx = 200;
	}
	else
	{
		cx = size.cx;
		cy = size.cy;
	}
	m_rectRender 
		= CRect( 
			0, 
			0, 
			cx + 6, 
			cy + 3 + ( 2 * ( cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) 
		);
	m_rect = rect;
	//m_strToolTip.SetString( string );
	m_strToolTip = "";
	m_strToolTip.Init( g_Neuz.m_2DRender.m_pFont, &CRect(0,0,cx,cy) );
	m_strToolTip.SetEditString( string );

//}}AFX
#endif // __V050714_BUGFIX_TOOLTIP
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Func. Name : 
// Parameters :
// Return     :
// Remarks    :
//   툴팁처리를 한다. 시간이 지나면 입력된 것을 출력한다.
//
void CToolTip::Process(CPoint pt,C2DRender* p2DRender)
{
	CD3DFont* pFont = p2DRender->m_pFont;
	if(m_bEnable == FALSE)
		return;
	// Put을 호출하지 못했다. 그건 툴립 출력할일이 없다는 뜻.
	if(m_bPutToolTip == FALSE)
	{
		m_dwToolTipId = -1;
		CancelToolTip();
		return;
	}
	if(m_bPutToolTip == TRUE && m_bReadyToopTip == TRUE && m_timerToopTip.Over())
	{
		if(m_bToolTip == NULL)
			m_ptToolTip = pt;
		else
			m_bToolTip = FALSE;

		if(0) //m_ptToolTip != pt)
		{
			m_bReadyToopTip = FALSE;
		}
		else
		{
			m_bToolTip = TRUE;
			/*
			CD3DFont* pFont = p2DRender->m_pFont;
			LPCTSTR string = m_strToolTip;
			DWORD dwHeight = pFont->GetMaxHeight();//string);//lpszString,nCount GetStringPixel((CHanString)str);memDC.GetStringPixel((CHanString)string);
			int nStrMax, nLength = strlen(string);
			CSize sizeString;
			if(nLength > 20)
				nStrMax = 20;
			else
				nStrMax = nLength;
			CStringArray strArray;
			CString str;
			char chr = ' ';
			int c = 0;
			CSize sizeLine;
			int nMaxLen = 0;
			do
			{
				chr = string[c];
				str = "";
				do 
				{
					chr = string[c++];
					if(chr != '\r' && chr != '\0')
						str += chr;
				} 
				while((c < nStrMax || chr != ' ') && chr != '\0' && chr != '\r');

				nStrMax = c + 20;
				strArray.Add(str);
				sizeLine = pFont->GetTextExtent( str );
				if(nMaxLen < sizeLine.cx)
					nMaxLen = sizeLine.cx;
			} 
			while(chr != '\0');
			int nWidth = nMaxLen + 6;
			int nHeight = sizeString.cy * strArray.GetSize() + 6;

			//m_bToolTip = new CDibBitmap;
			//m_bToolTip->CreateDIBSection(&memDC,NULL,nWidth,nHeight,16);
			//nWidth = m_bToolTip->GetWidth();
			//memDC.SelectBitmap_(m_bToolTip);
			//memDC.Clear(0xffff);
			//memDC.SetTextColor(MKHIGHRGB(0x1a,0x1a,0x1a));
			for(c = 0; c < strArray.GetSize(); c++)
			{
				//if(strArray[c][0] != '\r')
					p2DRender->TextOut( 3, 3 + c * sizeString.cy, strArray[c] );
			}
			//memDC.SelectObject(pOldFont);
			*/
		}
	}
	if(m_rect.PtInRect(pt) == FALSE)
		m_bPutToolTip = FALSE;
	/*
	else
	{
	//	m_rectRender.InflateRect( 1, 1 );

		if( m_rectRender.left < m_rectRender.left )
			m_rectRender.left = m_rectRender.left;
		if( m_rectRender.right > m_rectRender.right )
			m_rectRender.right = m_rectRender.right;
		if( m_rectRender.top < m_rectRender.top )
			m_rectRender.top = m_rectRender.top;
		if( m_rectRender.bottom > m_rectRender.bottom )
			m_rectRender.bottom = m_rectRender.bottom;
	}
	*/
	m_nAlpha += 15;
	if( m_nAlpha > 255 ) m_nAlpha = 255;
}
void CToolTip::Paint(C2DRender* p2DRender)
{
	if(m_bEnable == FALSE)
		return;
	if( m_bToolTip )
	{
		CPoint pt = m_ptToolTip;
		int nPostion = m_nPosition;
		if(nPostion == 0) // top
		{
			pt = m_rect.TopLeft();
			pt.y -= m_rectRender.Height() + 2;
		}
		else
		if(nPostion == 1) // bottom
		{
			pt = CPoint(m_rect.left,m_rect.bottom);
			//rect.SetRect(pt.x,pt.y,pt.x+m_bToolTip->GetWidth(),pt.y+m_bToolTip->GetHeight());
		}
		else
		if(nPostion == 2) // left
		{
			pt = m_rect.TopLeft();
			pt.x -= m_rectRender.Width() + 2;
		}
		else
		if(nPostion == 3) // right
		{
			pt = CPoint(m_rect.right,m_rect.top);
			//pt.x += m_bToolTip->GetHeight() + 2;
		}
		//CRect rect(pt.x,pt.y,pt.x+m_bToolTip->GetWidth(),pt.y+m_bToolTip->GetHeight());
		CRect rect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );
		// 사각형이 만들어졌지만, 화면 가장자리를 벗어날 수 있다. 벗어나면 벗어날 수 없도록 수정 
		if( rect.right > p2DRender->m_clipRect.right )
		{
			pt.x -= m_rectRender.Width() - m_rect.Width();
		}
		if( rect.top < p2DRender->m_clipRect.top )
		{
			pt.y += m_rectRender.Height() + m_rect.Height();
		}

		rect.SetRect( pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() );

#ifdef __TOOLTIPTEXTURE
		int nPlusLow = 8;
		int nPlusColumn = 8;
		int nSetting = g_Option.m_nToolTipTexture * 9;

		float fNa = float( rect.bottom - rect.top + nPlusLow * 2 ) / 16.0f;
		int nlowDraw = (int)fNa;
		if( nlowDraw == 0 && 0 < fNa )
			nlowDraw = 1;

		fNa = float( rect.right - rect.left + nPlusColumn * 2 ) / 16.0f;
		int nColumnDraw = (int)fNa;
		
		if( nColumnDraw == 0 && 0 < fNa )
			nColumnDraw = 1;

		CRect PlusRect = rect;
		PlusRect.left = rect.left - nPlusLow;
		PlusRect.top = rect.top - nPlusColumn;
		PlusRect.right = rect.right- 16 + nPlusLow;
		PlusRect.bottom = rect.bottom - 16 + nPlusColumn;

		for( int i = 0 ; i < nlowDraw ; ++i )
		{
			for( int j = 0 ; j < nColumnDraw ; ++j )
			{
				// 테두리 처리
				if( i == 0 || j == 0 )
				{
					if( j == 0 && i != 0 )
					{
						if( i + 1 == nlowDraw )
						{
							m_apTextureToolTip[ nSetting + 3 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top + ( i * 16 ) ), CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
							m_apTextureToolTip[ nSetting + 5 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top + ( i * 16 ) ), CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
						}
						else 
						{
							m_apTextureToolTip[ nSetting + 3 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top + ( i * 16 ) ) );
							m_apTextureToolTip[ nSetting + 5 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top + ( i * 16 ) ) );
						}
						
					}
					else if( i == 0 && j != 0 )
					{
						if( j + 1 == nColumnDraw )
						{
							m_apTextureToolTip[ nSetting + 1 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top ), CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
							m_apTextureToolTip[ nSetting + 7 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.bottom ), CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
						}
						else
						{
							m_apTextureToolTip[ nSetting + 1 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top ) );
							m_apTextureToolTip[ nSetting + 7 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.bottom ) );
						}
					}		
					continue;
				}
					
				// 가운데 처리
				if( i == nlowDraw - 1 || j == nColumnDraw - 1 )
				{
					// 끝에 맞지 않은 구조 처리 
					if( i == nlowDraw - 1 && j == nColumnDraw - 1 )
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );
						
					}
					else if( i == nlowDraw - 1 )
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( 16, PlusRect.bottom - ( PlusRect.top + ( i * 16 ) ) ) );

					}
					else
					{
						m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ),
							CPoint( PlusRect.right - ( PlusRect.left + ( j * 16 ) ), 16 ) );
					}
				}
				else
				{
					m_apTextureToolTip[ nSetting + 4 ].Render( p2DRender, CPoint( PlusRect.left + ( j * 16 ), PlusRect.top + ( i * 16 ) ) );
				}
			}
		}

		m_apTextureToolTip[ nSetting + 0 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.top ) );
		m_apTextureToolTip[ nSetting + 2 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.top ) );
		m_apTextureToolTip[ nSetting + 6 ].Render( p2DRender, CPoint( PlusRect.left, PlusRect.bottom ) );
		m_apTextureToolTip[ nSetting + 8 ].Render( p2DRender, CPoint( PlusRect.right, PlusRect.bottom ) );
		
#else // __TOOLTIPTEXTURE
		// 테두리 박스		
		p2DRender->RenderFillRect( rect + CPoint( 3, 3 ), D3DCOLOR_ARGB( m_nAlpha * 100 / 255, 0, 0, 0 ) );
		p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( m_nAlpha * 200 / 255, 255, 255, 255 ) );
#endif // __TOOLTIPTEXTURE
		//p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 255, 91, 104, 205 ) );
		//rect.DeflateRect(1,1);
		//p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 255, 116, 128, 220 ) );
		//rect.DeflateRect(1,1);
		//p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

		p2DRender->TextOut_EditString( rect.TopLeft().x + 3, rect.TopLeft().y + 3, m_strToolTip, 0, 0, 2 );//, D3DCOLOR_ARGB( m_nAlpha * 255 / 255, 0, 0, 0 ) );

//		DWORD dwLineCount = m_strToolTip.GetLineCount();
//		DWORD dwMaxHeight = p2DRender->m_pFont->GetMaxHeight() + 2;
//		CEditString string;
		//for( int i = 0; i < dwLineCount; i++)
		//{
			//string.SetEditString( m_strToolTip.GetEditString( i ) );
			//p2DRender->TextOut_EditString( rect.TopLeft().x + 3, rect.TopLeft().y + 3 + i * dwMaxHeight, m_strToolTip, 0, 0, 2 );//, D3DCOLOR_ARGB( m_nAlpha * 255 / 255, 0, 0, 0 ) );
		//}
		//p2DRender->PaintDibBitmap(m_bToolTip,rect.TopLeft(),0xffff);
	}
}