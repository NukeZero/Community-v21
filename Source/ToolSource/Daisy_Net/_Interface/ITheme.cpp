
#include "stdafx.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

char			g_szVersion[128];	
extern TCHAR	PROGRAM_NAME[128];	

#define			DWCOLOR_BLACK	0xFF000000
#define			DWCOLOR_WHITE	0xFFFFFFFF


//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CTheme::CTheme()
:	m_pd3dDevice( NULL )
,	m_pVBTexture( NULL )
,	m_pVBGauge( NULL )
,	m_pActiveDesktop( NULL )
,	m_pFontText( NULL )
,	m_pFontGameTitle( NULL )
,	m_pFontWndTitle( NULL )
,	m_pFontButton( NULL )
,	m_pFontChat( NULL )
,	m_pFontCaption( NULL )
,	m_pFontWorld( NULL )
,	m_pFontStatus( NULL )
,	m_d3dcBackground( DWCOLOR_BLACK )
,	m_d3dcWndBkgr( DWCOLOR_BLACK )
,	m_d3dcWndBolder( DWCOLOR_BLACK )
,	m_d3dcWndFrame( DWCOLOR_BLACK )
,	m_d3dcWndTitle( DWCOLOR_BLACK )
,	m_d3dcWndDeAct( DWCOLOR_BLACK )
,	m_d3dcTextBkgr( DWCOLOR_BLACK )
,	m_d3dcTextBolder( DWCOLOR_BLACK )
,	m_d3dcTextFrame( DWCOLOR_BLACK )
,	m_d3dcButtonBkgr( DWCOLOR_BLACK )
,	m_d3dcButtonBolder( DWCOLOR_BLACK )
,	m_d3dcButtonFrame( DWCOLOR_BLACK ) 
,	m_d3dcFont( DWCOLOR_BLACK )
,	m_d3dcHighLightFont( DWCOLOR_BLACK )
,	m_d3dcPushFont( DWCOLOR_BLACK )
,	m_d3dcHighLightBar( DWCOLOR_BLACK )
,	m_d3dcMenuItemBkgr( DWCOLOR_BLACK )
,	m_d3dcMenuItemBolder( DWCOLOR_BLACK )
,	m_d3dcMenuItemFrame( DWCOLOR_BLACK )
,	m_d3dcChatMyFont( DWCOLOR_BLACK )
,	m_d3dcMsgFont( DWCOLOR_BLACK )
,	m_d3dcMsgFontWarning( DWCOLOR_BLACK ) 
,	m_d3dcMsgFontEffect( DWCOLOR_BLACK )
,	m_d3dcMsgFontNotice( DWCOLOR_BLACK )
,	m_d3dcSBarPad( DWCOLOR_BLACK )
,	m_d3dcSBarStick( DWCOLOR_BLACK )
,	m_dwWallPaperType( DWCOLOR_BLACK )
,	m_bNudeSkin( FALSE )
{
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CTheme::~CTheme()
{
	DeleteTheme();
}


//---------------------------------------------------------------------------------------------
// Create Font ( 폰트 생성하기 )
// param	:	strFontName ( 폰트이름 : ex) gulim )
//				nFontSize	( 폰트크기 )
//				pFont		( 생성 된 폰트와 연결 될 포인터 )
//				strFontID	( 폰트만의 고유한 이름 )
//				nOutLine	( 외각선 라인의 개수 )
//				dwColor		( 폰트 색상 )
//				dwBgColor	( 외각선의 색상 )
//				dwFlags		( 폰트 옵션 : ex) D3DFONT_BOLD )
// return	:	bool
//---------------------------------------------------------------------------------------------
bool CTheme::CreateFont( CString strFontName, UINT nFontSize, CD3DFont*& pFont, CString strFontID, UINT nOutLine, DWORD dwColor, DWORD dwBgColor, DWORD dwFlags )
{
	CD3DFont* pNewFont( NULL );
	pNewFont = new CD3DFont( strFontName, nFontSize, dwFlags );
	FLERROR_ASSERT_LOG_RETURN( pNewFont, false, _T("[테마] 폰트 생성 실패 [ %s ]."), strFontName );	

#ifndef __FREE_TYPE_FONT
	pNewFont->m_nOutLine	= nOutLine;
#else
	//@@수정 : 아웃라인 일단 0으로
	pNewFont->m_nOutLine	= ( nOutLine > 1 ) ? 1 : 0; //pNewFont->m_nOutLine	= nOutLine;

#endif
	pNewFont->m_dwColor		= dwColor;
	pNewFont->m_dwBgColor	= dwBgColor;

	CString strResultFontID( _T("") );

	// 특별한 폰트 아이디가 없다면 기본 폰트 이름 + 크기로 이름을 만든다
	if( 0 == _tcslen( strFontID ) )
	{
		TCHAR szBuffer[256] = { NULL, };
		_sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, _T("%s%d"), strFontName, nFontSize );
		
		m_mapFont.SetAt( szBuffer, pNewFont );
		strResultFontID = szBuffer;
	}
	else
	{
		m_mapFont.SetAt( strFontID, pNewFont );
		strResultFontID = strFontID;
	}

	m_mapFont.Lookup( strFontID, (void*&)pFont );

	return true;
}


//---------------------------------------------------------------------------------------------
// LoadTheme ( 테마 불러오기 )
// param	: pd3dDevice, lpszFileName
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CTheme::LoadTheme( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName)
{
	FLERROR_ASSERT_LOG_RETURN( pd3dDevice, FALSE, _T("[테마] D3D 장치 포인터가 유효하지 않음.") );	
	FLERROR_ASSERT_LOG_RETURN( lpszFileName, FALSE, _T("[테마] 파일 이름 포인터가 유효하지 않음.") );	

	m_pd3dDevice = pd3dDevice;

	CScanner scanner;
	if( FALSE == scanner.Load( MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), lpszFileName ) ) )
		FLERROR_ASSERT_LOG_RETURN( NULL, FALSE, _T("[테마] 파일 읽어드리기 실패.") );	

	scanner.GetToken(); // subject or FINISHED

	while(scanner.tok!=FINISHED)
	{
		if( scanner.Token == "m_d3dcBackground" )
		{
			scanner.GetToken();
			m_d3dcBackground = scanner.GetHex();
		}
		else
		if( scanner.Token == "m_texWallPaper" )
		{
			scanner.GetToken();
			scanner.GetToken();

			m_texWndPaper.LoadTexture( m_pd3dDevice, MakePath( DIR_THEME,CWndBase::m_resMng.GetSelectLang(), "WindField.bmp" ), 0xFF0000 );
		}	
		else
		if( scanner.Token == "m_dwWallPaperType" )
		{
			scanner.GetToken();
			m_dwWallPaperType = scanner.GetNumber();
		}
		scanner.GetToken();
	}

	CreateFont( _T("gulim"),		9,	m_pFontChat,		_T("FontChat")		);
	CreateFont( _T("gulim"),		9,	m_pFontText,		_T("FontText")		);
	CreateFont( _T("gulim"),		8,	m_pFontStatus,		_T("FontStatus")	);
	CreateFont( _T("Arial Black"),	9,	m_pFontWndTitle,	_T("FontWndTitle"),	2, D3DCOLOR_ARGB( 255, 255, 219, 111 ), D3DCOLOR_ARGB( 255, 0, 0, 0 ),		D3DFONT_BOLD );
	CreateFont( _T("Arial Black"),	9,	m_pFontWorld,		_T("FontWorld"),	2, D3DCOLOR_ARGB( 255, 255, 255, 255 ), D3DCOLOR_ARGB( 255, 60, 60, 60 ) );
	CreateFont( _T("가을체"),		15, m_pFontCaption,		_T("FontCaption"),	2, D3DCOLOR_ARGB( 255, 255, 255, 255 ), D3DCOLOR_ARGB( 255, 40, 100, 220 ), D3DFONT_FILTERED );
	CreateFont( _T("gulim"),		9,	m_pFontButton,		_T("FontButton"),	0, D3DCOLOR_ARGB( 255, 255, 255, 255 ), D3DCOLOR_ARGB( 255, 0, 0, 0 ),		D3DFONT_BOLD );

	return TRUE;
}
BOOL CTheme::SaveTheme(LPCTSTR lpszFileName)
{
	return TRUE;
}
void CTheme::DeleteTheme()
{
	DeleteDeviceObjects();
} 
HRESULT CTheme::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->InitDeviceObjects( pd3dDevice );

		__asm nop
	}
//	g_mesh.InitDeviceObjects( m_pd3dDevice );
	//g_mesh.OpenMesh( _T("obj_rideferriswheel.mes") );

//	g_mesh.OpenMesh( _T("obj_starship.mes") );
	
	return TRUE;//m_pFontGameTitle->InitDeviceObjects( pd3dDevice );
}
HRESULT CTheme::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_texWallPaper.Invalidate();
	m_texWndPaper.Invalidate();
#endif //__YDEBUG

	HRESULT h = S_OK;
    SAFE_RELEASE( m_pVBTexture );
	SAFE_RELEASE( m_pVBGauge );
	//g_mesh.InvalidateDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->InvalidateDeviceObjects();
	}
	return h;
}
HRESULT CTheme::DeleteDeviceObjects()
{
	HRESULT h = S_OK;
	m_texWallPaper.DeleteDeviceObjects();
	m_texWndPaper.DeleteDeviceObjects();
	//g_mesh.DeleteDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->DeleteDeviceObjects();
		SAFE_DELETE( pFont );
	}
	m_mapFont.RemoveAll();
	return h;
}
HRESULT CTheme::RestoreDeviceObjects( )
{
#ifdef __YDEBUG
	m_texWallPaper.SetInvalidate(m_pd3dDevice);
	m_texWndPaper.SetInvalidate(m_pd3dDevice);
#endif //__YDEBUG
	
	if( m_pVBTexture ) 
		return S_OK;
	//g_mesh.RestoreDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		pFont->RestoreDeviceObjects();
	}
	HRESULT hr = S_OK;
	m_pd3dDevice->CreateVertexBuffer( sizeof(TEXTUREVERTEX)*24, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBTexture      , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX)*42, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBGauge       , NULL );
	return hr;
}
HRESULT CTheme::FrameMove()
{
	//g_mesh.FrameMove();
	return S_OK;
}
void CTheme::RenderTitle( C2DRender* p2DRender )
{
}
void CTheme::RenderDesktop( C2DRender* p2DRender )
{
	CTexture texture
		= m_texWallPaper;
	texture.SetAutoFree( FALSE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	CRect rectWindow = p2DRender->m_clipRect;
	if( m_dwWallPaperType == WPT_STRETCH ) // 전체 늘리기 
	{
		texture.m_size.cx = rectWindow.Width();
		texture.m_size.cy = rectWindow.Height();
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( 0, 0 ), &texture );
	}
	else
	if( m_dwWallPaperType == WPT_CENTER ) // 중앙 정렬 
	{
		CPoint pt( ( rectWindow.Width() / 2 ) - ( texture.m_size.cx / 2 ), ( rectWindow.Height() / 2 ) - ( texture.m_size.cy / 2 ) );
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( pt, &texture );
	}
	else
	if( m_dwWallPaperType == WPT_CENTERSTRETCH ) // 중앙 늘리기  
	{
		if(( (int) rectWindow.Width() - texture.m_size.cx ) < ( (int)rectWindow.Height() - texture.m_size.cy ) )
		{
			// texture.m_size.cx : m_pd3dsdBackBuffer->Width = texture.m_size.cy : y;
			texture.m_size.cy = rectWindow.Width() * texture.m_size.cy / texture.m_size.cx;
			texture.m_size.cx = rectWindow.Width();
		}
		else
		{
			// texture.m_size.cy : m_pd3dsdBackBuffer->Height = texture.m_size.cx : x;
			texture.m_size.cx = rectWindow.Height() * texture.m_size.cx / texture.m_size.cy;
			texture.m_size.cy = rectWindow.Height();
		}

		CPoint pt( ( rectWindow.Width() / 2 ) - ( texture.m_size.cx / 2 ), ( rectWindow.Height() / 2 ) - ( texture.m_size.cy / 2 ) );
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( pt, &texture );
	}
	else
	if( m_dwWallPaperType == WPT_TILE ) // 타일 정렬 
	{
		FLOAT fu = (FLOAT)rectWindow.Width()  / texture.m_size.cx;
		FLOAT fv = (FLOAT)rectWindow.Height() / texture.m_size.cy;
		texture.m_size.cx = rectWindow.Width();
		texture.m_size.cy = rectWindow.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( 0, 0), &texture );
	}
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	p2DRender->TextOut(  1, 1, "Version", 0xffffffff  );
	p2DRender->TextOut( 50, 1, g_szVersion, 0xffffffff  );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTheme::GradationRect( C2DRender* p2DRender, CRect* pRect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent )
{
	int nFirstHeight = pRect->Height() * nMidPercent / 100;
	
	CRect rect1 = *pRect; rect1.bottom = rect1.top + nFirstHeight;
	CRect rect2 = *pRect; rect2.top    = rect2.top + nFirstHeight;

	p2DRender->RenderFillRect( rect1, dwColor1t, dwColor1t, dwColor1b, dwColor1b );
	p2DRender->RenderFillRect( rect2, dwColor1b, dwColor1b, dwColor2b, dwColor2b );
}

//---------------------------------------------------------------------------------------------
// 윈도우 타이틀 바에 타이틀 텍스트 그리기 ( 그라데이션 그림 )
// param	: p2DRender, pRect, nX, nY, lpszTitle, dwColor
// return	: void 
//---------------------------------------------------------------------------------------------
void CTheme::RenderWndBaseTitleBar( C2DRender* p2DRender, CRect* pRect, int nX, int nY, LPCTSTR lpszTitle, DWORD dwColor  )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender,	_T("[테마] 2D Render 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( pRect,		_T("[테마] Rect 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( lpszTitle,	_T("[테마] Title 포인터가 유효하지 않습니다.") );

	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 255, 255, 255 );	
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 150, 150, 150 );	
	DWORD dwColor3 = D3DCOLOR_ARGB( 255, 230, 230, 230 );	

	GradationRect( p2DRender, pRect, dwColor1 ,dwColor2, dwColor3 );
	p2DRender->TextOut( nX, nY, lpszTitle, dwColor);
}


//---------------------------------------------------------------------------------------------
// 윈도우 타이틀 바에 타이틀 텍스트 그리기 ( 그라데이션 안 그림 )
// param	: p2DRender, nX, nY, lpszTitle, dwColor
// return	: void 
//---------------------------------------------------------------------------------------------
void CTheme::RenderWndBaseTitleBar( C2DRender* p2DRender, int nX, int nY, LPCTSTR lpszTitle, DWORD dwColor  )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender,	_T("[테마] 2D Render 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( lpszTitle,	_T("[테마] Rect 포인터가 유효하지 않습니다.") );

	p2DRender->TextOut( nX, nY, lpszTitle, dwColor );
}


void CTheme::RenderWndBaseFrame( C2DRender* p2DRender, CRect* pRect )
{
	//DWORD dwColor1 = D3DCOLOR_ARGB( 100,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	//DWORD dwColor2 = D3DCOLOR_ARGB( 100,  80,  80,  80 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	//DWORD dwColor3 = D3DCOLOR_ARGB( 150, 160, 160, 160 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	//DWORD dwColor4 = D3DCOLOR_ARGB( 100,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 150 );//
	DWORD dwColor1 = D3DCOLOR_ARGB( 255,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255,  80,  80,  80 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 255, 160, 160, 160 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//
	DWORD dwColor4 = D3DCOLOR_ARGB( 255,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 150 );//

	//CRect rectTemp = *pRect;
	//rectTemp.DeflateRect(2,2);
	//rectTemp.bottom = 20;
	if( m_bNudeSkin == FALSE )
	{
		CTexture texture
			= m_texWndPaper;
		texture.SetAutoFree( FALSE );

		FLOAT fu, fv;


	//	p2DRender->RenderTexture( CPoint( 0, 0), &texture, 0 );

		//////////////////////////////////

		CRect rect, rectOrg = *pRect;
		rectOrg += p2DRender->m_ptOrigin;
		
		
		TEXTUREVERTEX* pVertices;
		m_pVBTexture->Lock( 0, sizeof(TEXTUREVERTEX) * 24, (void**) &pVertices, 0 );

		// horizon

		rect.SetRect( rectOrg.left, rectOrg.top, rectOrg.right, rectOrg.top + 16 + 4 );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;

		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;

		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;
		
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		texture	= m_texWndPaper;
		texture.SetAutoFree( FALSE );
		rect.SetRect( rectOrg.left, rectOrg.bottom - 4, rectOrg.right, rectOrg.bottom );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;

		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;
		
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		// vertical
		texture	= m_texWndPaper;
		texture.SetAutoFree( FALSE );
		rect.SetRect( rectOrg.left, rectOrg.top + 16 + 4, rectOrg.left + 4, rectOrg.bottom - 4 );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;

		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;
		
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		texture	= m_texWndPaper;
		texture.SetAutoFree( FALSE );
		rect.SetRect( rectOrg.right - 4, rectOrg.top + 16 + 4, rectOrg.right, rectOrg.bottom - 4 );
		fu = (FLOAT)rect.Width()  / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;


		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.top,    texture. m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right, (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,  (FLOAT)rect.bottom ,texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.top,    texture. m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.left,   (FLOAT)rect.bottom, texture. m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f ); pVertices++;
		SetTextureVertex( pVertices, (FLOAT)rect.right,  (FLOAT)rect.bottom ,texture. m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f ); pVertices++;

		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		m_pVBTexture->Unlock(); 

		
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, 1 );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, 1 );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_NONE );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_NONE );
		p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		p2DRender->m_pd3dDevice->SetTexture( 0, texture.m_pTexture );
		p2DRender->m_pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX );
		p2DRender->m_pd3dDevice->SetStreamSource( 0, m_pVBTexture, 0,sizeof( TEXTUREVERTEX ) );
		p2DRender->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 8);

	//
	}
	///////////////////////////////////////////////
//return;
	// 테두리 박스 
	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor3 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRect( *pRect, 0xffffffff );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRect( *pRect, dwColor3 );


	/*
	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor2 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor3 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor4 );
	*/
}
void CTheme::RenderEdge( C2DRender* p2DRender, CRect* pRect, BOOL bClient )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	if( bClient )
		p2DRender->RenderFillRect ( *pRect, dwColor1 );
	p2DRender->RenderRoundRect( *pRect, dwColor2 );
	pRect->DeflateRect( 1 , 1 );
	p2DRender->RenderRect( *pRect, dwColor2 );
	pRect->DeflateRect( 1 , 1 );
	p2DRender->RenderRect( *pRect, dwColor3 );

}
void CTheme::RenderWndBaseBkgr( C2DRender* p2DRender, CRect* pRect  )
{
	if( m_bNudeSkin  == FALSE )
	{
		// 테두리 박스 
		DWORD dwColor1t = D3DCOLOR_ARGB( 155, 100, 100, 100 );//D3DCOLOR_ARGB( 255, 250, 250, 255 );//
		DWORD dwColor1b = D3DCOLOR_ARGB( 155,  70,  70,  70 );//D3DCOLOR_ARGB( 255, 200, 200, 210 );//
		DWORD dwColor2b = D3DCOLOR_ARGB( 155,  43,  73, 45 );//D3DCOLOR_ARGB( 255, 143, 173, 245 );//
		//adationRect( p2DRender, pRect, dwColor1t, dwColor1b, dwColor2b ); 

		CTexture texture
			= m_texWndPaper;
		texture.SetAutoFree( FALSE );

		FLOAT fu = (FLOAT)pRect->Width()  / texture.m_size.cx;
		FLOAT fv = (FLOAT)pRect->Height() / texture.m_size.cy;
		texture.m_size.cx = pRect->Width();
		texture.m_size.cy = pRect->Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
		p2DRender->RenderTexture( CPoint( 0, 0), &texture );
	//	p2DRender->RenderFillRect( *pRect, 0xa0ffffff );
	}
	else
	{
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		p2DRender->RenderFillRect( *pRect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );
	}
}
void CTheme::RenderWndEditFrame( C2DRender* p2DRender, CRect* pRect  )
{
	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB(255,200,200,250);//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 200, 200, 200 );//D3DCOLOR_ARGB(255,150,150,200);//

	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderFillRect( *pRect, dwColor2 );
}
void CTheme::RenderWndTextFrame( C2DRender* p2DRender, CRect* pRect  )
{
	//DWORD dwColor1 = D3DCOLOR_ARGB(  88,  50,  50,  50 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 255,255,255 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 226,198,181 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//

	p2DRender->RenderFillRect ( *pRect, dwColor1 );
	p2DRender->RenderRoundRect( *pRect, dwColor2 );

}
//////////////////////////

DWORD CTheme::GetButtonFontColor( CWndButton* pWndButton  )
{
	if( pWndButton->IsWindowEnabled() )
	{
		if( pWndButton->IsHighLight() )
			return pWndButton->GetHighLightColor();
		return pWndButton->IsPush() ? pWndButton->GetPushColor() : pWndButton->GetFontColor();
	}
	return pWndButton->GetDisableColor();
}
POINT CTheme::GetButtonTextPos( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	if( ! ( pWndButton->GetStyle() & WBS_NOCENTER ) )
		return pWndButton->GetStrCenter( p2DRender, pWndButton->GetTitle() );
	return CPoint( 0, 0);
}
void CTheme::RenderWndButton( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int   nCheck      = pWndButton->GetCheck();
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );

//#ifdef __NEWINTERFACE
	if( pWndButton->m_pTexture )
	{
		RenderWndButton_4Texture( p2DRender, pWndButton );

#ifdef __NEW_UI19
#else
		return;
#endif 
	}
	if( nCheck )
	{
		dwColor = D3DCOLOR_ARGB( 0, 0, 0, 0);

	}
	//DWORD dwColor1 = ( bHighLight ?	D3DCOLOR_ARGB( 255, 200, 200, 250 ) : D3DCOLOR_ARGB( 255, 150, 150, 250 ) );
	//DWORD dwColor2 = ( bHighLight ?	D3DCOLOR_ARGB( 255, 130, 130, 200 ) : D3DCOLOR_ARGB( 255,  30,  30, 100 ) );
	//DWORD dwColor1 = ( bHighLight ?	D3DCOLOR_ARGB( 100, 100, 100, 150 ) : D3DCOLOR_ARGB( 200,  200,  200, 200 ) );
	//DWORD dwColor2 = ( bHighLight ?	D3DCOLOR_ARGB(  55,  30,  30, 100 ) : D3DCOLOR_ARGB(  55,   0,   0,  00 ) );


	//DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
	//DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	//DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//

	DWORD dwColor1 = ( bPush ?	D3DCOLOR_ARGB( 255,   100, 255, 255 ) : D3DCOLOR_ARGB( 250, 255, 255, 255 ) );
	DWORD dwColor2 = ( bPush ?	D3DCOLOR_ARGB(  50,     0,   0,   0 ) : D3DCOLOR_ARGB(  50,   0,   0,   0 ) );
	DWORD dwColor3 = ( bPush ?	D3DCOLOR_ARGB( 200,     0, 150, 150 ) : D3DCOLOR_ARGB( 200, 150, 150, 150 ) );
//	DWORD dwColor  = ( bHighLight ? D3DCOLOR_ARGB( 250, 0, 255, 255 ) : D3DCOLOR_ARGB( 250, 255, 255, 255 ) );

	//p2DRender->TextOut( 17, 7, lpszTitle, 0xff000000);
	//p2DRender->TextOut( 16, 6, lpszTitle, 0xffffffff);

#ifdef __NEW_UI19

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( CTheme::m_pFontButton );

	POINT pt = GetButtonTextPos( p2DRender, pWndButton );

	p2DRender->TextOut( pt.x+1, pt.y+2, pWndButton->GetTitle(), 0xFFFFF4D0 );	 
	p2DRender->TextOut( pt.x, pt.y+1, pWndButton->GetTitle(), dwColor );
	p2DRender->SetFont( pOldFont );

	if( pWndButton->m_pTexture )
		return;
#else

	POINT pt = GetButtonTextPos( p2DRender, pWndButton );

	p2DRender->TextOut( pt.x+1, pt.y+1, pWndButton->GetTitle(), 0xff000000 );
	p2DRender->TextOut( pt.x, pt.y, pWndButton->GetTitle(), dwColor );

#endif

//	if( pWndButton->IsWndStyle( WBS_NODRAWFRAME ) )
//		return;

	CRect rect = pWndButton->GetClientRect();
	rect.DeflateRect( 2, 2);
	GradationRect( p2DRender, &rect, dwColor1 ,dwColor2, dwColor3 );
	p2DRender->RenderLine( CPoint( rect.left, rect.top ), CPoint( rect.right, rect.top ), dwColor3 );

	//CRect rect = pWndButton->GetClientRect();
	rect.InflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(  155, 200, 200, 200 ) );
	rect.InflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(  155,  50,  50,  50 ) );
/*
	if( bPush )
		p2DRender->RenderFillRect( rect, dwColor2, dwColor2, dwColor1, dwColor1 );
	else
		p2DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor2, dwColor2 );
*/
}
void CTheme::RenderWndButton_4Texture( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int   nCheck      = pWndButton->GetCheck();
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	if( pWndButton->m_pTexture == FALSE )
		return;
	
	CTexture* pTexture = pWndButton->m_pTexture;
	CSize sizeOld = pTexture->m_size;
	pTexture->m_size.cx /= 4;
	
	if( bEnable == FALSE )
	{
		pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvLT = 0.0f;

		pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
		pTexture->m_fvRT = 0.0f;

		pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

		pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
		pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
	}
	else
	if( bPush == FALSE && nCheck == 0)
	{
		if( bHighLight )
		{
			pTexture->m_fuLT = 0.0f;
			pTexture->m_fvLT = 0.0f;
			
			pTexture->m_fuRT = (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx;
			pTexture->m_fvRT = 0.0f;
			
			pTexture->m_fuLB = 0.0f;
			pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			
			pTexture->m_fuRB = (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx;
			pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
		}
		else
		{
			pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
			pTexture->m_fvLT = 0.0f;
			
			pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
			pTexture->m_fvRT = 0.0f;
			
			pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
			pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );
			
			pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
			pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
		}
	}
	else
	{

		pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
		pTexture->m_fvLT = 0.0f;

		pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvRT = 0.0f;

		pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
		pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );

		pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
		pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

	}
	pTexture->Render( p2DRender, CPoint( 0, 0 ), pWndButton->m_nAlphaCount );
	pTexture->m_size = sizeOld;
}

void CTheme::RenderWndButton_6Texture( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int   nCheck      = pWndButton->GetCheck();
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	if( pWndButton->m_pTexture )
	{
		CTexture* pTexture = pWndButton->m_pTexture;
		CSize sizeOld = pTexture->m_size;
		pTexture->m_size.cx /= 6;
		
		if( bEnable == FALSE )
		{
			if( bPush == FALSE && nCheck == 0 ) 
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 6.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 5.0f;
				pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 6.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		else
		if( bPush == FALSE && nCheck == 0 )
		{
			if( bHighLight )
			{
				pTexture->m_fuLT = 0.0f;
				pTexture->m_fvLT = 0.0f;
				
				pTexture->m_fuRT = (FLOAT) pTexture->m_size.cx / (FLOAT)pTexture->m_sizePitch.cx;
				pTexture->m_fvRT = 0.0f;
				
				pTexture->m_fuLB = 0.0f;
				pTexture->m_fvLB = (FLOAT) pTexture->m_size.cy / (FLOAT)pTexture->m_sizePitch.cy;
				
				pTexture->m_fuRB = (FLOAT) pTexture->m_size.cx / (FLOAT)pTexture->m_sizePitch.cx;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / (FLOAT)pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
				pTexture->m_fvLT = 0.0f;
				
				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvRT = 0.0f;
				
				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 1.0f;
				pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );
				
				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		else
		{
			if( bHighLight )
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 2.0f;
				pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 3.0f;
				pTexture->m_fvLB = ( (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy );

				pTexture->m_fuRB = ( (FLOAT) pTexture->m_size.cx / pTexture->m_sizePitch.cx ) * 4.0f;
				pTexture->m_fvRB = (FLOAT) pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		pTexture->Render( p2DRender, CPoint( 0, 0 ), pWndButton->m_nAlphaCount );
		pTexture->m_size = sizeOld;
	}

}

void CTheme::RenderWndButtonCheck( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	int nFontHeight = pWndButton->GetFontHeight();
	DWORD dwColor = GetButtonFontColor( pWndButton );
	if( pWndButton->m_pTexture ) // pWndButton->IsWndStyle( WBS_PUSHLIKE ) )
	{
		RenderWndButton_6Texture( p2DRender, pWndButton );
		p2DRender->TextOut( pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor );
	}
	else
	{
		BOOL  bHighLight  = pWndButton->IsHighLight();
		BOOL  bEnable     = pWndButton->IsWindowEnabled();
		BOOL  bPush       = pWndButton->IsPush();
//		POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

		int nFontColor = p2DRender->GetTextColor();

		p2DRender->RenderRect( CRect( 0, 0, 10, 10 ), dwColor );
		if( pWndButton->GetCheck() ) 
		{
			p2DRender->RenderLine( CPoint( 2,  2), CPoint( 5, 8), dwColor );
			p2DRender->RenderLine( CPoint( 2,  2), CPoint( 6, 8), dwColor );
			p2DRender->RenderLine( CPoint( 5,  8), CPoint( 8, 3), dwColor );
		}
		p2DRender->TextOut( nFontHeight + 5, 0, pWndButton->GetTitle(), dwColor );
	}
}

void CTheme::RenderWndButtonRadio( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender,	_T("[테마] 렌더러 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndButton, _T("[테마] 윈도우 버튼 포인터가 유효하지 않습니다.") );

	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );

	int nFontHeight( pWndButton->GetFontHeight() );

	if( pWndButton->m_pTexture ) 
	{
		RenderWndButton_6Texture( p2DRender, pWndButton );
		p2DRender->TextOut( pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor );
	}
	else
	{
		CRect rect( 0, 0, 10, 10 );
		
		p2DRender->RenderRoundRect( rect, dwColor );
		if( pWndButton->GetCheck() ) 
		{
			rect.DeflateRect( 2, 2, 2, 2 );
			p2DRender->RenderFillRect( rect, dwColor );
		}
		p2DRender->TextOut( nFontHeight + 5, 0, pWndButton->GetTitle(), dwColor );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTheme::RenderWndButtonText( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	BOOL  bHighLight  = pWndButton->IsHighLight();
	BOOL  bEnable     = pWndButton->IsWindowEnabled();
	BOOL  bPush       = pWndButton->IsPush();
	DWORD dwColor     = GetButtonFontColor( pWndButton );
	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	p2DRender->TextOut( pt.x, pt.y, pWndButton->GetTitle(), dwColor);
}

void CTheme::RenderWndTaskBar( C2DRender* p2DRender, CRect* pRect )
{
	CRect rect = *pRect;

	CTexture texture
		= m_texWndPaper;
	texture.SetAutoFree( FALSE );

	FLOAT fu = (FLOAT)pRect->Width()  / texture.m_size.cx;
	FLOAT fv = (FLOAT)pRect->Height() / texture.m_size.cy;
	texture.m_size.cx = pRect->Width();
	texture.m_size.cy = pRect->Height();
	texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
	texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
	texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
	texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
	p2DRender->RenderTexture( CPoint( 0, 0), &texture );

	// 테두리 박스 
	/*
	DWORD dwColor1t = D3DCOLOR_ARGB( 255,  91, 104, 205 );
	DWORD dwColor2t = D3DCOLOR_ARGB( 255, 116, 128, 220 );
	DWORD dwColor3t = D3DCOLOR_ARGB( 255, 143, 173, 245 );
	DWORD dwColor1b = D3DCOLOR_ARGB( 255,  41, 104, 155 );
	DWORD dwColor2b = D3DCOLOR_ARGB( 255,  66,  78, 170 );
	DWORD dwColor3b = D3DCOLOR_ARGB( 255,   3,  33, 105 );
*/
	DWORD dwColor0 = D3DCOLOR_ARGB( 150, 100, 100, 100 );
	DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );//

	//DWORD dwColor1 = D3DCOLOR_ARGB( 100,   0,   0,   0 );
	//DWORD dwColor2 = D3DCOLOR_ARGB( 150, 255, 255, 255 );
	//DWORD dwColor3 = D3DCOLOR_ARGB( 100,  00,  00,  00 );

	GradationRect( p2DRender, &rect, dwColor1 ,dwColor2, dwColor3, 40 );
	//p2DRender->RenderLine( CPoint( rect.left, rect.top ), CPoint( rect.right, rect.top ), dwColor3);
	
	rect = *pRect;//GetWindowRect();
	rect.DeflateRect( 3, 3);
	rect.left = 65;
	//rect.right = 240;
	rect.right -= 90;
	//p2DRender->RenderRoundRect( rect, dwColor0 );
	rect.DeflateRect( 1, 1);

//	rect = *pRect;//GetWindowRect();
	for( int i = 0; i < rect.Width() / 32; i++ )
		;//p2DRender->RenderTriangle( CPoint( rect.left+ 16 + i * 32, 3 ), CPoint( rect.left + 3 + i * 32, 32 - 3 ), CPoint( rect.left + 32 - 3 + i * 32, 32 - 3 ), D3DCOLOR_ARGB( 100, 200, 170, 170 ) );

	//GradationRect( p2DRender, &rect, dwColor3 ,dwColor2, dwColor1, 80 );
}
void CTheme::RenderWndMenuTask( C2DRender* p2DRender, CRect* pRect )
{
}
void CTheme::RenderWndMenu( C2DRender* p2DRender, CRect* pRect )
{
	RenderWndBaseFrame( p2DRender, pRect );
	/*
	CRect rect = *pRect;
	// 테두리 박스 
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 100, 100, 200 ) );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 180, 180, 220 ) );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 180, 180, 220 ) );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB( 155, 50, 50, 150 ) );
	*/
}
void CTheme::RenderWndMenuItem( C2DRender* p2DRender, CWndButton* pWndButton )
{
}
BOOL CTheme::MakeGaugeVertex( LPDIRECT3DDEVICE9 pd3dDevice, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture )
{
	CPoint pt = pRect->TopLeft();
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = ( pTexture->m_size.cx ) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = (FLOAT)( pt.x );
	FLOAT top    = (FLOAT)( pt.y );
	FLOAT right  = (FLOAT)( pt.x + nTexWidth );//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)( pt.y + nTexHeight );//( pTexture->m_size.cy );

	int nWidth = ( pRect->Width() / nTexWidth );// - 2;

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return FALSE;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	TEXTUREVERTEX2* pVertices,* pVertices_;
	HRESULT hr = pVB->Lock( 0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**) &pVertices_, D3DLOCK_DISCARD );
	if(hr != D3D_OK) return FALSE;
	{
		SIZE size = pTexture->m_size;//
		SIZE sizePitch = pTexture->m_sizePitch;//
		size.cx /= 3;
		
		pVertices = pVertices_;
		for( int i = 0; i < nVertexNum; i++ )
		{
			pVertices->vec.z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left   = (FLOAT)( pt.x );
		right  = (FLOAT)( pt.x + nTexWidth );
			
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		//////////////////////////////////
		
		left   = (FLOAT)( pt.x + nTexWidth );
		right  = (FLOAT)( pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth ) );
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

		//////////////////////////////////
		left   = (FLOAT)( pt.x + ( ( nWidth - 1 ) * nTexWidth ) );
		right  = (FLOAT)( pt.x + ( ( nWidth ) * nTexWidth ) );

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

	}
	pVB->Unlock(); 
	return TRUE;
}				
void CTheme::RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture )
{
	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	
	int nVertexNum = 3 * 6;
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetTexture( 0, pTexture->m_pTexture );
	pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX2 );
	pd3dDevice->SetStreamSource( 0, pVB, 0,sizeof( TEXTUREVERTEX2 ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertexNum / 3 );
}				

void CTheme::RenderGauge( C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture )
{
//	pTexture = &m_texGauEmptyNormal;
    LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice; 
	CPoint pt = pRect->TopLeft();
	pt += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = ( pTexture->m_size.cx ) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = (FLOAT)( pt.x );
	FLOAT top    = (FLOAT)( pt.y );
	FLOAT right  = (FLOAT)( pt.x + nTexWidth );//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)( pt.y + nTexHeight );//( pTexture->m_size.cy );

	int nWidth = ( pRect->Width() / nTexWidth );// - 2;

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	TEXTUREVERTEX2* pVertices,* pVertices_;
	HRESULT hr = pVB->Lock( 0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**) &pVertices_, D3DLOCK_DISCARD );
	if(hr != D3D_OK) return;
	{
		SIZE size = pTexture->m_size;//
		SIZE sizePitch = pTexture->m_sizePitch;//
		size.cx /= 3;
		
		pVertices = pVertices_;
		for( int i = 0; i < nVertexNum; i++ )
		{
			pVertices->vec.z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left   = (FLOAT)( pt.x );
		right  = (FLOAT)( pt.x + nTexWidth );
			
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		//////////////////////////////////
		
		left   = (FLOAT)( pt.x + nTexWidth );
		right  = (FLOAT)( pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth ) );
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

		//////////////////////////////////
		left   = (FLOAT)( pt.x + ( ( nWidth - 1 ) * nTexWidth ) );
		right  = (FLOAT)( pt.x + ( ( nWidth ) * nTexWidth ) );

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

	}
	pVB->Unlock(); 

	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetTexture( 0, pTexture->m_pTexture );
	pd3dDevice->SetFVF( D3DFVF_TEXTUREVERTEX2 );
	pd3dDevice->SetStreamSource( 0, pVB, 0,sizeof( TEXTUREVERTEX2 ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertexNum / 3 );
}				
/*
void CTheme::RenderWndGauge( C2DRender* p2DRender, CRect* pRect, DWORD dwColor )
{
	CRect rect = *pRect;
	rect += p2DRender->m_ptOrigin;
	DRAWVERTEX* pDrawVertices;
	HRESULT hr = m_pVBGauge->Lock( 0,sizeof(DRAWVERTEX)*42,(void**)&pDrawVertices, D3DLOCK_DISCARD );
	DWORD dwRed1   = ( dwColor & 0x00ff0000 ) >> 16;
	DWORD dwGreen1 = ( dwColor & 0x0000ff00 ) >> 8;
	DWORD dwBlue1  = ( dwColor & 0x000000ff );
	DWORD dwRed2   = dwRed1 / 2;
	DWORD dwGreen2 = dwGreen1 / 2;
	DWORD dwBlue2  = dwBlue1 / 2;
	DWORD dwAlpha1 = 255;
	DWORD dwAlpha2 = ( dwColor & 0xff000000 ) >> 24;

	// left top
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 0
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 1
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 2

	// top
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 3
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 4
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 5
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 6
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 7
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 8

	// right top
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 9
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 10
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 11

	// left

	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 );  pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 );  pDrawVertices++; // 16

	// middle

	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB(     99,      0,        0,         0 ); pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB(     99,      0,        0,       0   ); pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB(     99,      0,        0,       0   ); pDrawVertices++; // 16

	// right

	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 16

	// left bottom
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 0
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 1
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 2

	// bottom
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 16

	// right bottom
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 9
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed1, dwGreen1, dwBlue1 ); pDrawVertices++; // 10
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = D3DCOLOR_ARGB( dwAlpha2, dwRed2, dwGreen2, dwBlue2 ); pDrawVertices++; // 11


	m_pVBGauge->Unlock();

	p2DRender->m_pd3dDevice->SetTexture( 0, NULL );
	p2DRender->m_pd3dDevice->SetFVF( D3DFVF_DRAWVERTEX );
	p2DRender->m_pd3dDevice->SetStreamSource( 0, m_pVBGauge, 0,sizeof( DRAWVERTEX ) );
	p2DRender->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 14 );
}
void CTheme::RenderWndGauge2( C2DRender* p2DRender, CRect* pRect, DWORD dwColor )
{
	CRect rect = *pRect;
	rect += p2DRender->m_ptOrigin;
	DRAWVERTEX* pDrawVertices;
	HRESULT hr = m_pVBGauge->Lock( 0,sizeof(DRAWVERTEX)*42,(void**)&pDrawVertices, 0 );

	// left top
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 0
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 1
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 2

	// top
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 3
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 4
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 5
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 6
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 7
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 8

	// right top
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 9
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 10
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 11

	// left

	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 16

	// middle

	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff000000; pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff000000; pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff000000; pDrawVertices++; // 16

	// right

	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.top     + 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 16

	// left bottom
	pDrawVertices->x = (FLOAT)rect.left  + 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 0
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 1
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 2

	// bottom
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 12
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 13
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 14
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 15
	pDrawVertices->x = (FLOAT)rect.left  + 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 16
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 16

	// right bottom
	pDrawVertices->x = (FLOAT)rect.right - 0; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 9
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 0; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xffff0000; pDrawVertices++; // 10
	pDrawVertices->x = (FLOAT)rect.right - 5; pDrawVertices->y = (FLOAT)rect.bottom  - 5; pDrawVertices->z = 0.0f; pDrawVertices->rhw = 1.0f; pDrawVertices->color = 0xff700000; pDrawVertices++; // 11

	m_pVBGauge->Unlock();

	p2DRender->m_pd3dDevice->SetTexture( 0, NULL );
	p2DRender->m_pd3dDevice->SetFVF( D3DFVF_DRAWVERTEX );
	p2DRender->m_pd3dDevice->SetStreamSource( 0, m_pVBGauge,0, sizeof( DRAWVERTEX ) );
	p2DRender->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 14 );
}






















*/

void CTheme::SetVersion(int Lang)
{
	switch( Lang )
	{
		case LANG_KOR:
#ifdef __TESTSERVER
			sprintf_s( g_szVersion, _countof(g_szVersion), "%s %s T", __DATE__, __TIME__ );
#else
			sprintf_s( g_szVersion, _countof(g_szVersion), "%s %s", __DATE__, __TIME__);
#endif
			break;
		case LANG_ENG:
			sprintf_s( g_szVersion, _countof(g_szVersion), "%s %s_ENG", __DATE__, __TIME__);
			break;			
		case LANG_JAP:
			sprintf_s( g_szVersion, _countof(g_szVersion), "%s %s_JPN", __DATE__, __TIME__ );
			break;			
		case LANG_CHI:
		case LANG_TWN:
			sprintf_s( g_szVersion, _countof(g_szVersion), "%s %s_TWN", __DATE__, __TIME__ );
			break;			
			
		case LANG_THA:
			sprintf_s( g_szVersion, _countof(g_szVersion), "%s %s_THA", __DATE__, __TIME__ );
			break;			
	}
}
