#include "stdafx.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineItem.h"
#include "AppDefine.h"

#include "DialogMsg.h"  
#include "ClientMsg.h"  
#include "DPClient.h"
#include "MsgHdr.h"
#include "WndMessenger.h"
#include "WndIndirectTalk.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

extern	CDPClient	g_DPlay;

#include "party.h"
extern	CParty		g_Party;
extern	CPartyMng	g_PartyMng;

#ifdef __GUILDCOMBAT
#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;
#endif // __GUILDCOMBAT


#include "yUtil.h"
extern CFLASH g_Flash;

extern CMapStrToObjId	g_MapStrToObjId;

#ifdef __ACROBAT_0504
#include "defineskill.h"
#endif //__ACROBAT_0504


int g_nSkillCurSelect;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_POWER_GAUGE = 1800;
#ifdef __ACROBAT_0504
	const DWORD POWER_ATK_DELAY = 1800;
#else
	const DWORD POWER_ATK_DELAY = 0;
#endif

//헬프 이중생성 방지

vector<CString> g_vecHelpInsKey;

CCaption::CCaption()
{
	m_nCount = 0;
}
CCaption::~CCaption()
{
	RemoveAll();
}
void CCaption::RemoveAll()
{
	for( int i = 0; i < m_aCaption.GetSize(); i++ )
	{
		LPCAPTION lpCaption = ( LPCAPTION ) m_aCaption.GetAt( i );
		delete lpCaption;
	}
	m_aCaption.RemoveAll();
	m_nCount = 0;
	m_bEnd = FALSE;
}
void CCaption::Process()
{
	if(	m_nCount == 0 && m_aCaption.GetSize() )
		m_nCount = 1;
	if( m_bEnd )
	{
		if( m_timer.IsTimeOut() )
		{
			for( int i = 0; i < m_aCaption.GetSize(); i++ )
			{
				LPCAPTION lpCaption = ( LPCAPTION ) m_aCaption.GetAt( i );
				lpCaption->m_nAlpha -= 3;
				if( lpCaption->m_nAlpha < 0 )
					lpCaption->m_nAlpha = 0;
			}		
		}
	}
	else
	for( int i = 0; i < m_nCount; i++ )
	{
		LPCAPTION lpCaption = ( LPCAPTION ) m_aCaption.GetAt( i );
		lpCaption->m_fAddScale += 0.002f;
		lpCaption->m_fXScale += lpCaption->m_fAddScale;
		lpCaption->m_fYScale += lpCaption->m_fAddScale;
		if( lpCaption->m_fXScale > 1.0f ) 
			lpCaption->m_fXScale = 1.0f; 
		if( lpCaption->m_fYScale > 1.0f ) 
			lpCaption->m_fYScale = 1.0f;
		if( lpCaption->m_fXScale > 0.1f ) 
		{ 
			if( i == m_nCount - 1 ) 
			{ 
				m_nCount++; 
				if( m_nCount > m_aCaption.GetSize() ) 
				{
					m_nCount = m_aCaption.GetSize();
					if( lpCaption->m_fYScale >= 1.0f ) 
					{
						m_bEnd = TRUE;
						m_timer.Set( SEC( 5 ) );
					}
				}
				break; 
			}
		}
	}
}
HRESULT CCaption::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CCaption::DeleteDeviceObjects()
{
	RemoveAll();
	return S_OK;
}
HRESULT CCaption::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CCaption::InvalidateDeviceObjects()
{
	RemoveAll();
	return S_OK;
}
void CCaption::Render( CPoint ptBegin, C2DRender* p2DRender )
{
	//CD3DFont* pFontOld = p2DRender->GetFont();
	int nCount = m_nCount > m_aCaption.GetSize() ? m_aCaption.GetSize() : m_nCount;

	for( int i = 0; i < nCount; i++ )
	{
		LPCAPTION lpCaption = ( LPCAPTION ) m_aCaption.GetAt( i );
		//p2DRender->SetFont( lpCaption->m_pFont );
		CRect rect = p2DRender->m_clipRect;//GetWndRect();
		CPoint point = CPoint( rect.Width() / 2, 0 );
#ifndef __VBUGFIX_20050105
		CSize size;
	#ifndef __VTITLECAPTION_JAPAN
			if( g_osVersion <= WINDOWS_ME )
				CWndBase::m_Theme.m_pFontCaption->GetTextExtent( lpCaption->m_szCaption, &size );
			else
				lpCaption->m_pFont->GetTextExtent( lpCaption->m_szCaption, &size );
	#else //__VTITLECAPTION_JAPAN
			CWndBase::m_Theme.m_pFontCaption->GetTextExtent( lpCaption->m_szCaption, &size );
	#endif //__VTITLECAPTION_JAPAN
#else //__VBUGFIX_20050105
		CSize size = lpCaption->m_size;
#endif //__VBUGFIX_20050105
		FLOAT fXScale = ( 7.0f - lpCaption->m_fXScale * 6.0f ); // 최대 7배 사이즈 
		FLOAT fYScale = ( 7.0f - lpCaption->m_fYScale * 6.0f ); // 최대 7배 사이즈 
		point.x -= ( size.cx / 2 ) * fXScale;
		//point.y -= ( size.cy / 2 ) * fYScale;
		point += ptBegin;

//		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

		if( lpCaption->m_nAlpha )
		{
	#ifndef __VTITLECAPTION_JAPAN
			if( g_osVersion <= WINDOWS_ME )
				CWndBase::m_Theme.m_pFontCaption->DrawText( point.x, point.y, fXScale, fYScale, D3DCOLOR_ARGB(  (int)(lpCaption->m_fXScale * 255) - ( 255 - lpCaption->m_nAlpha), 250, 250, 255 ), lpCaption->m_szCaption );
			else
			{
				if( lpCaption->m_texture.m_pTexture )
					p2DRender->RenderTexture( CPoint( point.x, point.y ), &lpCaption->m_texture, (int)(lpCaption->m_fXScale * 255) - ( 255 - lpCaption->m_nAlpha), fXScale, fYScale  );
				else
				{
					lpCaption->m_pFont->DrawText( point.x, point.y, fXScale, fYScale, D3DCOLOR_ARGB(  (int)(lpCaption->m_fXScale * 255) - ( 255 - lpCaption->m_nAlpha), 250, 250, 255 ), lpCaption->m_szCaption );
				}
			}
	#else //__VTITLECAPTION_JAPAN
			CWndBase::m_Theme.m_pFontCaption->DrawText( point.x, point.y, fXScale, fYScale, D3DCOLOR_ARGB(  (int)(lpCaption->m_fXScale * 255) - ( 255 - lpCaption->m_nAlpha), 250, 250, 255 ), lpCaption->m_szCaption );
	#endif //__VTITLECAPTION_JAPAN
		}
		ptBegin.y += size.cy;
	}
//	p2DRender->SetFont( pFontOld );
}
void CCaption::AddCaption( LPCTSTR lpszCaption, CD3DFontAPI* pFont )
{
	if( m_bEnd && m_timer.IsTimeOut() )
		RemoveAll();
#ifdef __VBUGFIX_20050105
	CSize size;

#ifndef __VTITLECAPTION_JAPAN
	if( g_osVersion <= WINDOWS_ME )
		CWndBase::m_Theme.m_pFontCaption->GetTextExtent( lpszCaption, &size );
	else
		pFont->GetTextExtent( lpszCaption, &size );
#else //__VTITLECAPTION_JAPAN
	CWndBase::m_Theme.m_pFontCaption->GetTextExtent( lpszCaption, &size );
#endif //__VTITLECAPTION_JAPAN

#endif //__VBUGFIX_20050105

	LPCAPTION lpCaption = new CAPTION;
	strcpy( lpCaption->m_szCaption, lpszCaption );
	lpCaption->m_pFont = pFont;
	lpCaption->m_fXScale = 0.0f;
	lpCaption->m_fYScale = 0.0f;
	lpCaption->m_fAddScale = 0.0f;
	lpCaption->m_nAlpha = 255;
#ifdef __VBUGFIX_20050105
	lpCaption->m_size = size;
#endif
	m_aCaption.Add( lpCaption );
	m_bEnd = FALSE;

#ifndef __VTITLECAPTION_JAPAN
	if( g_osVersion <= WINDOWS_ME )
		return;
#else //__VTITLECAPTION_JAPAN
	return;
#endif //__VTITLECAPTION_JAPAN

	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;

#ifndef __VBUGFIX_20050105
	CSize size;
	pFont->GetTextExtent( lpszCaption, &size );
#endif

	// 여분을 만들자 
#ifdef __VBUGFIX_20050105
	size.cx += 16 + 64; 
	size.cy += 16;
#else
	size.cx += 16; 
	size.cy += 16;
#endif

	D3DVIEWPORT9 viewportOld;
	pd3dDevice->GetViewport(&viewportOld);

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = size.cx;
	viewport.Height = size.cy;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	D3DDeviceCombo* pDeviceCombo = g_Neuz.m_d3dSettings.PDeviceCombo();
	HRESULT hr = g_Neuz.m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      pDeviceCombo->AdapterFormat,
                                      D3DUSAGE_RENDERTARGET,
                                      D3DRTYPE_TEXTURE,
                                      D3DFMT_A8R8G8B8 );
	if( hr == D3D_OK )
	{
		AdjustSize( &size );
		if( lpCaption->m_texture.CreateTexture( pd3dDevice, size.cx, size.cy, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT ) )
		{
			IDirect3DSurface9* pOldSurface;
			IDirect3DSurface9* pOldSurfaceZ;
			IDirect3DSurface9* pDstSurface; 
			pd3dDevice->GetRenderTarget( 0, &pOldSurface );
			pd3dDevice->GetDepthStencilSurface( &pOldSurfaceZ );
			lpCaption->m_texture.m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
			pd3dDevice->SetRenderTarget( 0, pDstSurface );//, pOldSurfaceZ );

			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000/*CWorld::m_dwBgColor*/, 1.0f, 0);
			if( pd3dDevice->BeginScene() == S_OK )
			{
				lpCaption->m_pFont->DrawText( 8, 8, 1.0f, 1.0f, D3DCOLOR_ARGB(  255, 255, 255, 255 ), lpCaption->m_szCaption );
				pd3dDevice->EndScene();
			}
			pd3dDevice->SetRenderTarget( 0, pOldSurface );//, pOldSurfaceZ );
			pOldSurface->Release();
			pOldSurfaceZ->Release();
			pDstSurface->Release();
			pd3dDevice->SetViewport(&viewportOld);
		}
	}
}
#ifdef __V050322_CACHEQUEST
HRESULT CCapTime::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return S_OK;
}
HRESULT CCapTime::DeleteDeviceObjects()
{
	m_texture.DeleteDeviceObjects();
	return S_OK;
}
HRESULT CCapTime::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CCapTime::InvalidateDeviceObjects()
{
	m_texture.DeleteDeviceObjects();
	return S_OK;
}

CCapTime::CCapTime()
{
	m_fXScale = 1.0f;
	m_fYScale= 1.0f;
	m_fAddScale = 0;
	m_nAlpha = 255;
	m_nTime = 0;
	m_bRender = FALSE;
	m_nStep = 0;
//	m_size
}
CCapTime::~CCapTime()
{
}
void CCapTime::Render( CPoint ptBegin, C2DRender* p2DRender )
{
	if( m_bRender == FALSE )
		return;

#ifndef __VTITLECAPTION_JAPAN
	if( m_texture.m_pTexture == NULL )
		return;
#endif //__VTITLECAPTION_JAPAN

	// CEditString에서 폰트 class가 다르기 때문에 Init를 할 수 없다. 
	CEditString strTemp;
	CString strTime;
	strTime.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), m_nTime );
	strTemp.AddParsingString( strTime );
	strTime = strTemp;

	//p2DRender->SetFont( lpCaption->m_pFont );
	CRect rect = p2DRender->m_clipRect;//GetWndRect();
	CPoint point = CPoint( rect.Width() / 2, 0 );
	FLOAT fXScale = m_fXScale; // 최대 7배 사이즈 
	FLOAT fYScale = m_fYScale; // 최대 7배 사이즈 
	point.x -= ( m_size.cx / 2 ) * fXScale;
	point.y -= ( m_size.cy / 2 ) * fYScale;
	point += ptBegin;

	//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	if(  m_nAlpha )
	{
	#ifndef __VTITLECAPTION_JAPAN
		if( g_osVersion <= WINDOWS_ME )
			CWndBase::m_Theme.m_pFontCaption->DrawText( point.x, point.y, fXScale, fYScale, D3DCOLOR_ARGB(  (int)(m_fXScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), strTime );
		else
		{
			if( m_texture.m_pTexture )
				p2DRender->RenderTexture( CPoint( point.x, point.y ), &m_texture, (int) m_nAlpha, fXScale, fYScale  );
			else
			{
				m_pFont->DrawText( point.x, point.y, fXScale, fYScale, D3DCOLOR_ARGB(  (int)(m_fXScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), strTime );
			}
		}
	#else //__VTITLECAPTION_JAPAN
		CWndBase::m_Theme.m_pFontCaption->DrawText( point.x, point.y, fXScale, fYScale, D3DCOLOR_ARGB(  (int)(m_fXScale * 255) - ( 255 - m_nAlpha), 250, 250, 255 ), strTime );
	#endif //__VTITLECAPTION_JAPAN
	}
}
void CCapTime::SetTime( int nTime, CD3DFontAPI* pFont )
{
 	m_nTime = nTime;
	CEditString strTemp;
	CString strTime;
	strTime.Format( GETTEXT( TID_QUEST_LIMIT_TIME ), m_nTime );
	strTemp.AddParsingString( strTime );
	strTime = strTemp;
	m_bRender = TRUE;

//	CSize size;
	m_fAddScale = 0.001f;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_nStep = 0;
	m_nAlpha = 0;

#ifndef __VTITLECAPTION_JAPAN
	if( g_osVersion <= WINDOWS_ME )
		CWndBase::m_Theme.m_pFontCaption->GetTextExtent( strTime, &m_size );
	else
		pFont->GetTextExtent( strTime, &m_size );

	if( g_osVersion <= WINDOWS_ME )
		return;
#else //__VTITLECAPTION_JAPAN
	CWndBase::m_Theme.m_pFontCaption->GetTextExtent( strTime, &m_size );
	return;
#endif //__VTITLECAPTION_JAPAN

	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;

	m_pFont = pFont;
	m_size.cx += 16;// + 64; 
	m_size.cy += 16;

	D3DVIEWPORT9 viewportOld;
	pd3dDevice->GetViewport(&viewportOld);

	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = m_size.cx;
	viewport.Height = m_size.cy;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	D3DDeviceCombo* pDeviceCombo = g_Neuz.m_d3dSettings.PDeviceCombo();
	HRESULT hr = g_Neuz.m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                      D3DDEVTYPE_HAL,
                                      pDeviceCombo->AdapterFormat,
                                      D3DUSAGE_RENDERTARGET,
                                      D3DRTYPE_TEXTURE,
                                      D3DFMT_A8R8G8B8 );
	if( hr == D3D_OK )
	{
		CSize size = m_size;
		AdjustSize( &size );
		if( m_texture.CreateTexture( pd3dDevice, size.cx, size.cy, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT ) )
		{
			IDirect3DSurface9* pOldSurface;
			IDirect3DSurface9* pOldSurfaceZ;
			IDirect3DSurface9* pDstSurface; 
			pd3dDevice->GetRenderTarget( 0, &pOldSurface );
			pd3dDevice->GetDepthStencilSurface( &pOldSurfaceZ );
			m_texture.m_pTexture->GetSurfaceLevel( 0, &pDstSurface );
			pd3dDevice->SetRenderTarget( 0, pDstSurface );//, pOldSurfaceZ );

			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000/*CWorld::m_dwBgColor*/, 1.0f, 0);
			if( pd3dDevice->BeginScene() == S_OK )
			{
				pFont->DrawText( 8, 8, 1.0f, 1.0f, D3DCOLOR_ARGB(  255, 255, 255, 255 ), strTime );
				pd3dDevice->EndScene();
			}
			pd3dDevice->SetRenderTarget( 0, pOldSurface );//, pOldSurfaceZ );
			pOldSurface->Release();
			pOldSurfaceZ->Release();
			pDstSurface->Release();
			pd3dDevice->SetViewport(&viewportOld);
		}
	}
}
void CCapTime::Process()
{

	if( m_nStep == 0 )
	{
		m_nAlpha += 3;
		if( m_nAlpha >= 255 )
		{
			m_nAlpha = 255;
			m_nStep++;
		}
	}
	else
	{	m_fAddScale += 0.001f;
		m_fXScale += m_fAddScale;
		m_fYScale -= m_fAddScale;
		m_nAlpha -= 5;
		if( m_nAlpha < 0 )
			m_nAlpha = 0;
		if( m_fXScale > 3.0f ) 
			m_fXScale = 3.0f; 
		if( m_fYScale < 0.0f ) 
			m_fYScale = 0.0f;
		if( m_fXScale > 0.1f ) 
		{ 
		/*f( i == m_nCount - 1 ) 
			{ 
				m_nCount++; 
				if( m_nCount > m_aCaption.GetSize() ) 
				{
					m_nCount = m_aCaption.GetSize();
					if( m_fYScale >= 1.0f ) 
					{
						m_bEnd = TRUE;
						m_timer.Set( SEC( 3 ) );
					}
				}
				break; 
			}*/
		}	
	}
}
CCapTime g_CapTime;
#endif
CCaption g_Caption1;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
D3DXVECTOR3 CWndWorld::m_vTerrainPoint;

#ifdef __YCLOTH
CCloth g_Cloth;
#endif

CWndWorld::CWndWorld()
{
#ifdef __V050322_CACHEQUEST
	m_bFreeMove = FALSE;
#endif
	m_bBGM = FALSE;
	SetFullMax( TRUE );
//	m_pWndWorld = this;
	g_WndMng.m_pWndWorld = this;
	m_fHigh = -1.0f;
	m_fLow  = 999999.0f;
	//m_bCameraLock=FALSE;
	//m_pSelectObj = NULL;
	m_bNewSelect = TRUE;
	m_bFlyMove = FALSE;
	//m_pWndDialog = NULL;
	m_nMouseMode = 0;

	s_bUped = s_bDowned = s_bLefted = s_bRighted = FALSE;
	s_bCombatKeyed = s_bFlyKeyed = s_bAccKeyed = FALSE;
	s_bTempKeyed = FALSE;
	m_bTemp2ed = FALSE;
	m_bTemp3ed	= FALSE;
	s_bBoarded = FALSE;
	
	m_bSelectTarget = FALSE;
	m_timerFocusAttack.Set( 500 );

	m_dwIdBgmMusic = 0;
#ifdef __VRCAMERA
	m_bCameraMode = FALSE;
#endif

	m_pVBGauge = NULL;
	m_vTerrainPoint = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bAutoAttack = FALSE;
	m_dwNextSkill = NEXTSKILL_NONE;

	//m_pCaptureObj = NULL;

	//m_pWndTradeGold = NULL;
	m_objidTracking		= NULL_ID;
	m_dwDropTime	= GetTickCount();
	
	m_vTelePos = D3DXVECTOR3( 0.0f, 0.0f ,0.0f );

	m_nSelect = 0;
	ClearFlyTarget();
	m_fRollAng = 0;
	m_pFontAPICaption = NULL;
	m_pFontAPITitle = NULL;
#ifdef __V050322_CACHEQUEST
	m_pFontAPITime = NULL;
#endif

	m_bFirstFlying = FALSE;
	m_SkillState.Init();
	m_pBuffTexture.clear();

#ifdef __XITEMBUFF
	m_pBuffTexture.resize(3);
#else //__XITEMBUFF
	m_pBuffTexture.resize(2);
#endif //__XITEMBUFF

	m_dwOneSecCount = GetTickCount();

#ifdef __ONLINE
	for( int j = 0 ; j < SM_MAX ; ++j )
	{
		m_dwSMItemTexture[j] = NULL;
		m_bSMFlsh[j] = FALSE;
		m_nSMAlpha[j] = 192;
	}
#endif // __ONLINE

	// 헬프 이중 생성 방지

	g_vecHelpInsKey.clear();
	
#ifdef __YGUIDESYSTEM0923
	m_pWndGuideSystem = NULL;	
#endif //CWndCGuideSystem 


	m_pWndReSkillPoint_Text = NULL;
	m_pWndReSkillPoint = NULL;
	
#ifdef __S0126_MOTION_AUTORUN
	m_nDubleUp = 0;
	m_timerAutoRunPush.Set( 200 );
	m_timerAutoRun.Set( 200 );
	m_timerAutoRunBlock.Set( 200 );
#endif // __S0126_MOTION_AUTORUN

	m_dwPowerTick = 0;	
#ifdef __NPP_050308
	m_idLastTarget	= NULL_ID;
#endif	// __NPP_050308
	
#ifdef __GUILDCOMBAT	
	n_nMoverSelectCount = 0;
	m_dwGuildCombatTime = 0xffffffff;
	memset( m_szGuildCombatStr, 0, sizeof(char) * 64 );
#endif //__GUILDCOMBAT

#ifdef __Y_POST_0912
	m_IsMailTexRender = FALSE;
#endif //__Y_POST_0912	

	m_rcCheck.reserve( 4 );
}

CWndWorld::~CWndWorld()
{
	//SAFE_DELETE( m_pWndDialog );
	DeleteDeviceObjects();
}
int nColor;
BOOL g_bAlpha;
FLOAT g_fXScale, g_fYScale;
FLOAT g_fAddScale;
int		g_nFlySpeed = 0;
int		g_nDrift = 0;
#ifdef __XUZHU
float _g_fReg[16];
#endif
void CWndWorld::OnDraw( C2DRender* p2DRender )
{ 
#if defined(__XUZHU) || defined(__PROF) || defined(__J) || defined(_DEBUG)
	TCHAR strDebug[128];
	strDebug[0] = 0;
#endif

#ifdef __XUZHU
	p2DRender->TextOut( 2, 110 ,g_Neuz.m_strFrameStats , D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	if( g_pPlayer ) {
		if( g_pPlayer->GetIAObjLink() )
			p2DRender->TextOut( 2, 100 ,1 , D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		else
			p2DRender->TextOut( 2, 100 ,0 , D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

		_stprintf( strDebug, _T("Obj:%d  Face:%d   LFace:%d" ), g_pPlayer->GetWorld()->m_nObjCullSize, g_nMaxTri,g_nRenderedLandTries );
		p2DRender->TextOut( 2, 130, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("%f %f %f %f %f" ), _g_fReg[0], _g_fReg[1], _g_fReg[2], _g_fReg[3], _g_fReg[4]  );
		p2DRender->TextOut( 2, 150, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("%d %d %d %d" ), m_ptMouseOld.x, m_ptMouseOld.y, g_WndMng.m_pWndTaskBar->m_nExecute, 0  );
		p2DRender->TextOut( 2, 170, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	}

#ifdef __XGLARE
	_stprintf( strDebug, _T("bloom %d" ), g_Option.m_nBloom  );
	p2DRender->TextOut( 2, 255, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
#endif
	
#endif

#ifdef _DEBUG
	if( g_pPlayer )
	{
		_stprintf( strDebug, _T("전기저항 %d" ), g_pPlayer->GetParam( DST_RESIST_ELECTRICITY, 0 )  );
		p2DRender->TextOut( 2, 180, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("불저항 %d" ), g_pPlayer->GetParam( DST_RESIST_FIRE, 0 )  );
		p2DRender->TextOut( 2, 195, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("바람저항 %d" ), g_pPlayer->GetParam( DST_RESIST_WIND, 0 )  );
		p2DRender->TextOut( 2, 210, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("물저항 %d" ), g_pPlayer->GetParam( DST_RESIST_WATER, 0 )  );
		p2DRender->TextOut( 2, 225, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("땅저항 %d" ), g_pPlayer->GetParam( DST_RESIST_EARTH, 0 )  );
		p2DRender->TextOut( 2, 240, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		_stprintf( strDebug, _T("시련모드 %d" ), g_pPlayer->IsMode(RECOVERCHAO_MODE)  );
		p2DRender->TextOut( 2, 255, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	}
#endif // DEBUG	

#ifdef __XUZHU
	if( g_pPlayer )
	{
		_stprintf( strDebug, _T("%d"), g_pPlayer->m_tmAccFuel );
		p2DRender->TextOut( 400, 170, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	}
#endif
#ifdef __PROF
	//p2DRender->TextOut( 600, 120, g_Prof.m_szResult, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	extern DWORD	_dwA, _dwB, _dwC;
	_stprintf( strDebug, _T("%d %d %d" ), _dwA, _dwB, _dwC );
	p2DRender->TextOut( 2, 140, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );


	
#endif 
#ifdef __J
	_stprintf( strDebug, _T("%d %d" ), g_lBytesRecvd, g_lBytesProcessed );
	p2DRender->TextOut( 2, 140, strDebug, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );

#endif

	g_DialogMsg.Render( p2DRender );
	// 768 : 484 = Height : y
	p2DRender->SetFont( 	CWndBase::m_Theme.m_pFontWorld );
	g_ClientMsg.Render( CPoint( p2DRender->m_clipRect.Width() / 2 , 520 * p2DRender->m_clipRect.Height() / 768 ), p2DRender );
	p2DRender->SetFont( 	CWndBase::m_Theme.m_pFontText );
	// 768 : 184 = Height : y
	g_Caption1.Render( CPoint( 0, 184 * p2DRender->m_clipRect.Height() / 768 ), p2DRender );
#ifdef __V050322_CACHEQUEST
	g_CapTime.Render( CPoint( 0, 50 * p2DRender->m_clipRect.Height() / 768 ), p2DRender );
#endif // __V050322_CACHEQUEST

#ifdef _DEBUG
	CString strTime;
	if( g_GameTimer.m_nHour > 12 )
		strTime.Format( "pm %d시, %d분, %d초\n", g_GameTimer.m_nHour - 12, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	else
		strTime.Format( "am %d시, %d분, %d초\n", g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	//p2DRender->TextOut( 0, 500, strTime );
#endif	

#ifdef __YCLOTH
	if( g_pPlayer )
	{
		D3DXMATRIX matWorld;

		D3DXMatrixScaling( &matWorld, 0.1f, 0.1f, 0.1f );
		//matWorld *= g_pPlayer->GetMatrixTrans();

		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );

		g_Cloth.Render(p2DRender->m_pd3dDevice, &matWorld );
	}
#endif
	

/*
	// 비행중 십자선.
	if( g_pPlayer->m_pActMover->IsFly() && g_Neuz.m_camera.m_fZoom <= 2.0f )
	{
		CPoint pt1, pt2;
		int		nCx = g_Neuz.GetCreationWidth() / 2;
		int		nCy = g_Neuz.GetCreationHeight() / 2;
		pt1.x = nCx - 8;		pt1.y = nCy;		// 가로선
		pt2.x = nCx + 8;		pt2.y = pt1.y;
		p2DRender->RenderLine( pt1, pt2, D3DCOLOR_ARGB( 192, 255, 0, 0) );
		pt1.x = nCx - 0;		pt1.y = nCy - 8;	// 세로선
		pt2.x = pt1.x;			pt2.y = nCy + 8;
		p2DRender->RenderLine( pt1, pt2, D3DCOLOR_ARGB( 192, 255, 0, 0) );
	}
*/
}
/*
void CWndWorld::ProjectionAndView( LPDIRECT3DDEVICE9 pd3dDevice )
{
	Projection( D3DDEVICE );
	g_Neuz.m_camera.Process( D3DDEVICE );
	g_Neuz.m_camera.Transform( D3DDEVICE, g_WorldMng() );
}
*/
void CWndWorld::Projection( LPDIRECT3DDEVICE9 pd3dDevice )
{
	// Frame Window와 관련된 Viewport 세팅 
	CRect rectRoot = m_pWndRoot->GetWindowRect();
	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = rectRoot.Width();
	viewport.Height = rectRoot.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	if( 0 ) //bFull ) 
	{
		/*
		CWndBase* pWndFull 
			= g_WndMng.FindFullWnd();
		if(pWndFull)
			viewport.Width = CLIENT_WIDTH - pWndFull->GetWindowRect().Width();
		else
			viewport.Width = CLIENT_WIDTH;
		pd3dDevice->SetViewport( &viewport );
		*/
	}
	else
	{
		//CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
		CRect rect = GetClientRect();
		viewport.Width = rect.Width();
		viewport.Height = rect.Height();
		//m_pd3dDevice->SetViewport(&viewport);
	}
	// 프로젝션 
	g_WorldMng.Get()->Projection( pd3dDevice, viewport.Width, viewport.Height );
}

BOOL CWndWorld::OnEraseBkgnd(C2DRender* p2DRender)
{
	ClearFlyTarget();		// 비행때 공격가능한 타겟들 리스트 지워줌.

	CHECK1();

	Projection( D3DDEVICE );
	// CNeuzApp:Render()에도 Clear가 있어 중복되므로 지웠음. -XuZhu-
	//p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, CWorld::m_dwBgColor, 1.0f, 0 ) ;
	//if( m_nWinSize != WSIZE_MAX )
		//p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 90, 146, 222 ), 1.0f, 0 ) ;
	DWORD dwColor = CWorld::GetDiffuseColor();
	p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

	if( g_pPlayer == NULL )		return FALSE;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	CWorld* pWorld = g_WorldMng.Get();
	// 필드 출력 
	pWorld->SetCamera( &g_Neuz.m_camera );
	CHECK1();
/*
	if( pFocus  )
	{
		CMover* pMover = (CMover*)pFocus;
		CModelObject* pModel = (CModelObject*)pMover->m_pModel;
		for( int i=0; i<MAX_ELEMENT; i++ )
			pModel->SetEffect( i, XE_HIGHLIGHT_OBJ );
	}	
*/		
	pWorld->Render( pd3dDevice, m_Theme.m_pFontWorld );
	CHECK2("Render World" );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
	
	CObj *pFocus = pWorld->GetObjFocus();

	int nGaugeValue;
	int nPower = GetGaugePower( &nGaugeValue );	
	BOOL bRenderGauge = FALSE;
	DWORD dwShootItemKind = 0;

	// 완드나 스테프를 들고 있을 때는 마법 게이지 출력 
	CItemElem* pItemElem = g_pPlayer->GetWeaponItem();

#ifdef __ACROBAT_0504
	// 보우 공격
	if( pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_BOW ) 
	{
		// 그리고 왼쪽 버튼을 누르고 포커스가 있어야한다. 그래야 마법 타이밍 작동하니까.
		if( pFocus && pFocus->GetType() == OT_MOVER && g_pPlayer->IsLive() )
		{
			bRenderGauge = TRUE;
			if( m_bLButtonDown && m_bSelectTarget )
			{
				if( nPower >= MAX_CHARGE_LEVEL )			// 맥스치가 되면 자동으로 발사됨.
					dwShootItemKind = IK3_BOW;
			}
		}
	}
#endif //__ACROBAT_0504

	if( pItemElem &&  pItemElem->GetProp()->dwItemKind3 == IK3_WAND )
	{
		// 그리고 왼쪽 버튼을 누르고 포커스가 있어야한다. 그래야 마법 타이밍 작동하니까.
		if( pFocus && pFocus->GetType() == OT_MOVER && g_pPlayer->IsLive() )
		{
			bRenderGauge = TRUE;
			if( m_bLButtonDown && m_bSelectTarget )
			{
				if( g_pPlayer->GetManaPoint() >= 5 )
				{
					if( nPower >= MAX_CHARGE_LEVEL )	// 최고 단계

						dwShootItemKind = IK3_WAND;
				} 
				else
				{	// 마나가 모자른상황이면 최고 단계까지 올라가지 않고 최고 -1단계 짜리를 쏜다. 
					if( nPower >= (MAX_CHARGE_LEVEL-1) )	
						dwShootItemKind = IK3_WAND;
				}
			}
		}
	} // IK3_WAND
	
	if( dwShootItemKind )
	{
		ASSERT( nPower > 0 );
		if( g_pPlayer->IsAttackAble(pFocus) )
		{
			OBJID idTarget = ((CCtrl*)pFocus)->GetId();
			switch( dwShootItemKind )
			{
			case IK3_WAND:
				m_dwPowerTick = ::GetTickCount() + POWER_ATK_DELAY;
				g_pPlayer->CMD_SetMagicAttack( idTarget, nPower );		
				break;
			case IK3_BOW:
				m_dwPowerTick = ::GetTickCount() + POWER_ATK_DELAY;
				g_pPlayer->CMD_SetRangeAttack( idTarget, nPower );		
				break;
			}
		}
	}

	if( bRenderGauge )
	{
		if( nPower == -1 )	// 초고단계 공격 후 딜레이 상태?
			nPower = MAX_CHARGE_LEVEL;		// 풀 게이지로 그린다
		else if( m_bLButtonDown == FALSE || m_bSelectTarget == FALSE)	
			nPower = 0;					// 그리지 않는다.

		g_pPlayer->RenderGauge( pd3dDevice, nPower );	// 0-4
	}

#ifdef __XTURBO
	// 비행시 터보게이지

	if( g_pPlayer->IsFly() )
	{
		// 최대 12초 가속을 기준으로한 가속 게이지.
		g_pPlayer->RenderTurboGauge( pd3dDevice, 0xff0000ff, g_pPlayer->m_tmAccFuel, 12 * 1000 );
	}
#endif
	
#ifdef __Y0325
	D3DXVECTOR3 v3CameraDir, v3PartyMemberDir;
	v3CameraDir = g_Neuz.m_camera.m_vLookAt - g_Neuz.m_camera.m_vPos;
	D3DXVec3Normalize( &v3CameraDir, &v3CameraDir );

	for( int i = 0 ; i < g_Party.GetSizeofMember() ; i++ )
	{
		u_long idPlayer = g_Party.GetPlayerId( i );
		CMover* pMover  = prj.GetUserByID( idPlayer );
		
		if( pMover == g_pPlayer )
			continue;
		
		if( !IsValidObj(pMover) )
			continue;
		
		v3PartyMemberDir = pMover->GetPos() - g_Neuz.m_camera.m_vPos;
		D3DXVec3Normalize( &v3PartyMemberDir, &v3PartyMemberDir );

		if( D3DXVec3Dot( &v3CameraDir, &v3PartyMemberDir ) < 0.0f )
			continue;
		
		pMover->RenderHP( g_Neuz.m_pd3dDevice );
	}
	
	if( IsValidObj(g_pPlayer) )
	{
		if( CWorld::m_nZoomLevel == 0 )
		{
				g_pPlayer->RenderHP( g_Neuz.m_pd3dDevice );
		#ifdef __S0504_FLY
				g_pPlayer->RenderCasting( g_Neuz.m_pd3dDevice );
		#endif // __S0504_FLY
		#ifdef __NPP_050308
				g_pPlayer->RenderPVPCount( g_Neuz.m_pd3dDevice );
		#endif // __NPP_050308
		}
	}

#ifdef __S0504_FLY
//	RenderCasting( p2DRender );
#endif // __S0504_FLY
	
#endif // __Y0305
	
	if( pFocus && pFocus->GetType() == OT_MOVER )
	{
		if( ((CMover *)pFocus)->IsMode( TRANSPARENT_MODE ) )		// 상대가 투명화 상태면

			pWorld->SetObjFocus( NULL );							// 타겟잡은거 풀림.
	}

	RenderSelectObj( p2DRender );
#ifndef __XGAU0520
	if( g_pPlayer->m_pActMover->IsFly() )
		RenderGauFlight( p2DRender );
#endif

	// 날기 모드가 아니면 고도계 출력 안하고 종료 
	if( g_pPlayer && g_pPlayer->m_pActMover->IsFly() == TRUE )
		RenderAltimeter();


	//p2DRender->SetFont( 	CWndBase::m_Theme.m_pFontWorld );
	//g_ClientMsg.Render( CPoint( 0, 0 ), p2DRender );
	//p2DRender->SetFont( 	CWndBase::m_Theme.m_pFontText );

	CRect rectClient = GetClientRect();
	g_Neuz.m_camera.Transform( g_Neuz.m_pd3dDevice, g_WorldMng.Get() );

#ifdef __CLIENT
	// 머리위에 뜨는 데미지 숫자를 위해 게임화면 뷰표트를 받아둠.
	g_DamageNumMng.m_matView = g_Neuz.m_camera.m_matView;
	g_DamageNumMng.m_matProj = g_WorldMng.Get()->m_matProj;
	pd3dDevice->GetViewport( &g_DamageNumMng.m_viewport );
#endif
	
	if( g_pPlayer->IsDie() )	// 죽었을땐 타겟팅을 못하도록 하자. 이것땜에 팅기더라.
		CObj::m_pObjHighlight = NULL;
	else
		HighlightObj( GetMousePoint() );

	CRect rect = GetClientRect();
	D3DXVECTOR3 vRayEnd;
	
	//pWorld->ClientPointToVector( rectClient, GetMousePoint(), D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, D3DXVECTOR3* pVector, BOOL bObject )
//	pWorld->ClientPointToVector( rectClient, GetMousePoint(), &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, &m_vTerrainPoint, FALSE );
	g_DamageNumMng.Render();
	if( g_WndMng.m_pWndTaskBar->m_nCurQueue >= 0 )
	{
		int nCount = 5 - g_WndMng.m_pWndTaskBar->m_nCurQueue % 5;
		g_DamageNumMng.m_textPackNum.Render( p2DRender, CPoint( rectClient.Width() - 20, rectClient.Height() - 20 ), nCount );
	}
	CHECK2( "Render WndWorld" );
	RenderArrow();

	RenderWantedArrow();

	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pApp->m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// LIGHT / FOG가 꺼진상태에서 드로우 되어야 한다,.
	// 비행중 사경반경이내에 들어오는 플레이어들은 사각테두리가 쳐진다.
	{
		int	i, nSize = m_aFlyTarget.GetSize();
		CRect rect;
		CMover *pMover;
		OBJID idMover;
		
		for( i = 0; i < nSize; i ++ )
		{
			idMover = m_aFlyTarget.GetAt(i);
			pMover = prj.GetMover( idMover );
			if( IsValidObj(pMover) )
			{
				GetBoundRect( pMover, &rect );		// 화면상에서의 바운드 렉트를 구함.
				g_Neuz.m_2DRender.RenderRect( rect, D3DCOLOR_ARGB(0xff, 255, 32, 32) );
			}
		}
	}

	g_Flash.RenderFlash( p2DRender );
	
#ifndef __WORLDSERVER
	RenderBuff( p2DRender );
#endif // __WORLDSERVER


#ifndef __WORLDSERVER
#ifdef __GUILDCOMBAT
	if( m_dwGuildCombatTime != 0xffffffff )
	{
		CTimeSpan ct( ((m_dwGuildCombatTime - GetTickCount()) / 1000) );

		char szMsg[256] = { 0 };			
		
		sprintf( szMsg, "%s %.2d:%.2d", m_szGuildCombatStr, ct.GetMinutes(), ct.GetSeconds() );

		if( ct.GetHours() <=0 && ct.GetMinutes() <=0 && ct.GetSeconds() <=0 )
			m_dwGuildCombatTime = 0xffffffff;

		int nX = m_rectWindow.Width() / 2;
		int nY = 110;

		CSize cSize = p2DRender->m_pFont->GetTextExtent( m_szGuildCombatStr );
		nX -= (cSize.cx / 2);
		
		p2DRender->TextOut( nX, nY, szMsg, 0xffff00ff, 0xff990099 );
	}
	if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
	{
		char szMsgGuild[128] = { 0 };			
		char szMsg[128] = {0,};
		int nX = 20;
		int nY = (m_rectWindow.Height()/2) - 80;
		CSize cSize = p2DRender->m_pFont->GetTextExtent( m_szGuildCombatStr );
				
		CRect crect;

		int nCount = 0;
		for( vector<__GuildCombatJoin>::iterator  itv = m_vGuildCombatSort.begin() ; itv != m_vGuildCombatSort.end() ; ++itv )
		{
			if( 9 < nCount )
				break;

			map< u_long, __GuildCombatJoin >::iterator itw = m_GuildCombatJoin.find( ((__GuildCombatJoin)*itv).uidGuild );
			if( itw != m_GuildCombatJoin.end() )
			{
				__GuildCombatJoin GuildCombatJoin = itw->second;
				

				if( g_GuildCombatMng.m_nState == CGuildCombat::WAR_STATE )
				{
					if( GuildCombatJoin.nJoinSize == 0 )
						continue;
				}

				nY += (cSize.cy+10);

				char szBuf[MAX_NAME] = {0,};

				GetStrCut( GuildCombatJoin.szJoinGuildName, szBuf, 8 );
				
				if( 8 <= GetStrLen(GuildCombatJoin.szJoinGuildName) )
				{
					strcat( szBuf, "..." );
				}
				else
				{
					strcpy( szBuf, GuildCombatJoin.szJoinGuildName );
				}
				
				sprintf( szMsgGuild, "%s", szBuf );
				sprintf( szMsg, "%.2d/%.2d", GuildCombatJoin.nJoinSize, GuildCombatJoin.nJoinSize + GuildCombatJoin.nOutSize );

				crect = CRect(nX-10, nY-5, nX+160, nY+18);
				p2DRender->RenderFillRect(crect, D3DCOLOR_ARGB( 30, 0, 0, 200 ) );

				if( nCount < 1 )
				{
					p2DRender->TextOut( nX, nY, szMsgGuild, 0xFFFF0000, 0xFF000000 );
					p2DRender->TextOut( nX + 120, nY, szMsg, 0xFFFF0000, 0xFF000000 );
				}
				else if( nCount < 3 )
				{
					p2DRender->TextOut( nX, nY, szMsgGuild, 0xFF26F4F6, 0xFF000000 );
					p2DRender->TextOut( nX + 120, nY, szMsg, 0xFF26F4F6, 0xFF000000 );
				}
				else
				{
					p2DRender->TextOut( nX, nY, szMsgGuild, 0xFFFFFFFF, 0xFF000000 );
					p2DRender->TextOut( nX + 120, nY, szMsg, 0xFFFFFFFF, 0xFF000000 );
				}

				++nCount;
			}
		}
	}
#endif //__GUILDCOMBAT
#endif //__WORLDSERVER

#ifdef __CLIENT
#ifdef __Y_POST_0912
#ifdef _DEBUG
	if( m_IsMailTexRender )
	{
		p2DRender->TextOut( 10, 300, "Mail Receive!!!", 0xFF26F4F6, 0xFF000000 );		
	}	
#endif //_DEBUG
#endif //__Y_POST_0912
#endif //__CLIENT
	
	
	return TRUE;
}
void CWndWorld::RenderArrow()
{
	// 타겟의 방향을 가리키는 화살표를 그린다.
	CWorld* pWorld = g_WorldMng();
	if( !g_pPlayer || !pWorld ) 
		return; // 플레이어가 없으면 렌더 안한다
	D3DXVECTOR3 vSrc = g_pPlayer->GetPos();
	D3DXVECTOR3 vDest;
	LPDIRECT3DDEVICE9 pd3dDevice = m_pApp->m_pd3dDevice;

	if( m_stnv.dwWorldId == pWorld->GetID() )
		vDest	= m_stnv.vPos;
	else
	if( pWorld->GetObjFocus() ) 
		vDest = ((CMover*)( pWorld->GetObjFocus() ) )->GetPos();
	else 
		return; // 타겟 안잡혀 있어도 당근 렌더 안한다.

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
/*
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
*/	
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	// 화살표의 위치, 회전값을 결정한다.
	D3DXMatrixLookAtLH( &matWorld, &vDest, &vSrc, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	D3DXMatrixInverse (&matWorld,NULL,&matWorld);
	matWorld._41 = g_pPlayer->GetPos().x; matWorld._42 = g_pPlayer->GetPos().y + 2.0f; matWorld._43 = g_pPlayer->GetPos().z;

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	m_meshArrow.SetBlendFactor( 80 );
	m_meshArrow.Render( pd3dDevice, &matWorld );

}
BOOL CWndWorld::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	DWORD dwCursor = 0xffffffff;
#ifdef __VRCAMERA
	if( m_bRButtonDown &&/* m_bCameraMode &&*/ g_WorldMng()->GetObjFocus() != CObj::m_pObjHighlight )
#else
	if( m_bMButtonDown )
#endif
	{
		dwCursor = CUR_CAMERA;
	}
	else
	if( CObj::m_pObjHighlight )
	{
		if( CObj::m_pObjHighlight->IsDelete() == FALSE )
		{
			if( CObj::m_pObjHighlight->GetType() == OT_MOVER )
			{
				CMover* pMover = (CMover*) CObj::m_pObjHighlight;
				if( pMover->IsPeaceful() )
				{
					if( pMover->IsPlayer() == FALSE )
					{
						dwCursor = CUR_NPC;
					}
				}
				else
					dwCursor = CUR_ATTACK;
			}
			else
			if( CObj::m_pObjHighlight->GetType() == OT_ITEM )
				dwCursor = CUR_GETITEM;
			else if( CObj::m_pObjHighlight->GetType() == OT_CTRL )
			{
				ObjProp* pObjProp = CObj::m_pObjHighlight->GetProp();

				if( pObjProp )
				{
					CtrlProp* pCtrlProp;
					pCtrlProp = prj.GetCtrlProp(pObjProp->dwID);

					if( pCtrlProp )
					{
						switch(pCtrlProp->dwCtrlKind1) 
						{
						default:
							case CK1_CHEST:
								dwCursor = CUR_CONTROL;
								break;
								break;
						}
					}
				}
			}
			else
				dwCursor = CUR_NPC;
		}
	}
	else
	{
		if( ( CMover::GetActiveMover() && CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() ) || g_WndMng.GetWndBase(APP_WEBBOX)
#ifdef __TAIWAN_WEBBOX
			|| g_WndMng.GetWndBase(APP_WEBBOX)
#endif	// __TAIWAN_WEBBOX
		)
			dwCursor = CUR_NO;		
	}
	if( dwCursor == 0xffffffff )
		CWndNeuz::OnSetCursor( pWndBase, nHitTest, message );
	else
		SetMouseCursor( dwCursor );
	return TRUE;
}
void CWndWorld::GetBoundRect( CObj* pObj, CRect* pRect )
{
	CWorld* pWorld	= g_WorldMng.Get();
	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vMin, vMax, vPos = pObj->GetPos();
	D3DVIEWPORT9 vp;
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y , vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixScale() );
	D3DXMatrixMultiply( &matWorld, &matWorld, &pObj->GetMatrixRotation() );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );

	const BOUND_BOX* pBB = pModel->GetBBVector();
	D3DXVECTOR3 vOut[ 8 ];
	for( int i = 0; i < 8; i++ )
		D3DXVec3Project( &vOut[ i ], &pBB->m_vPos[ i ], &vp, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &matWorld );	

	CRect rectClient = GetClientRect();
	//m_rectBound.SetRect( rectClient.right, rectClient.bottom, rectClient.left, rectClient.top );
	pRect->SetRect( 65535.0f, 65535.0f, -65535.0f, -65535.0f );
	for( i = 0; i < 8; i++ )
	{
		vPos = vOut[ i ];
		if( vPos.x < pRect->left )
			pRect->left = vPos.x;
		if( vPos.x > pRect->right )
			pRect->right = vPos.x;
		if( vPos.y < pRect->top )
			pRect->top = vPos.y;
		if( vPos.y > pRect->bottom )
			pRect->bottom = vPos.y;
	}
}

void CWndWorld::RenderSelectObj( C2DRender* p2DRender )
{
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pObj;
		//		if( pMover->m_pActMover->IsDie() )		
		//			pObj = NULL;	// 무버가 죽은상태면 타겟팅 표시 안되게 함
		if( IsValidObj( pMover ) && ( pMover->IsPeaceful() == FALSE || pMover->IsPlayer() ) )
		{
#ifdef _DEBUG
#ifdef __XUZHU
			{
				D3DXVECTOR3 vDest = g_pPlayer->GetPos();
				D3DXVECTOR3 vSrc  = pMover->GetPos();		// 몬스터쪽에서 플레이어쪽의벡터.
				_g_fReg[4] = GetDegreeX( vDest, vSrc );
			}
#endif
#endif			
			BOOL bSkip = FALSE;
			// 운영자이고 변신중이면 HP바 안그린다.
			if( pMover->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
			{
				if( pMover->m_SkillState.HasLikeItemBuf(IK3_TEXT_DISGUISE) )
					bSkip = TRUE;
			}
			if( bSkip == FALSE )
			{
				// 게이지 출력
				CRect rect;// = GetClientRect();
				//	pMover->m_nHitPoint = 50;
				int nWidthClient = 200;
				//100 : rect = hp : x
				int nWidth;

				CString strTemp;
				
				// HP
				nWidth = nWidthClient * ((float)pMover->GetHitPoint() / pMover->GetMaxHitPoint());
				//if( m_nHPWidth != nWidth ) 
				{
					//m_nHPWidth = nWidth;
					int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
					//if( )
					rect = CRect( nPos, 30, nPos + nWidthClient, 0 );
					CPoint point = rect.TopLeft();
					CRect rectTemp = rect; 
					rectTemp.right = rectTemp.left + nWidth;
					ClientToScreen( rect );
					ClientToScreen( rectTemp );
					m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB( 200, 255, 255, 255 ), m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 128, 255, 15, 15 ), m_pVBGauge, &m_texGauEmptyNormal );
					m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
					
					//p2DRender->RenderTexture( CPoint( nPos-60, 7 ), &m_texTargetGauge );
					
					//int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
					
					CD3DFont* pOldFont = p2DRender->GetFont();
					p2DRender->SetFont( m_Theme.m_pFontWorld );
					TCHAR	szText[128];
					if( pMover->IsPlayer() )
					{
						sprintf( szText, "%s", pMover->GetName() );
					}
					else
					{
						BOOL bViewLevel = TRUE;
	#ifdef __XHALLOW
						if( prj.m_nLanguage == LANG_THA || prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN )	// 태국/대만의 경우

							if( pMover->GetIndex() == MI_MRPUMPKIN5 )						// 괴수 미스터펌킨은
								bViewLevel = FALSE;
	#else
	#endif
						if( pMover->GetProp()->dwHideLevel == 1 )
							bViewLevel = FALSE;
								
						if( bViewLevel )
							sprintf( szText, "%s LV%d", pMover->GetName(), pMover->GetLevel() );
						else
							sprintf( szText, "%s", pMover->GetName() );
					}
					if( pMover->IsPlayer() )
					{
						p2DRender->TextOut( point.x + 70, point.y - 15, szText, COLOR_PLAYER );
					}
					else
					{
						p2DRender->TextOut( point.x + 50, point.y - 18, szText, COLOR_MONSTER );
					}

					int nLevelIntv;
					if( pMover->IsPlayer() )
						nLevelIntv = pMover->GetLevel() - g_pPlayer->GetLevel();
					else
						nLevelIntv = pMover->GetProp()->dwLevel - g_pPlayer->GetLevel();
					// 타겟이 플레이어가 아닐 경우 레벨 화실표 표시 
					if( pMover->IsPlayer() == FALSE )
					{
						BOOL bAble = TRUE;
	#ifdef __XHALLOW
						if( prj.m_nLanguage == LANG_THA || prj.m_nLanguage == LANG_CHI || prj.m_nLanguage == LANG_TWN )	// 태국/대만의 경우

						if( pMover->GetIndex() == MI_MRPUMPKIN5 )						// 괴수 미스터펌킨은
							bAble = FALSE;		// 화살표 표시 안함.
	#endif // xHALLOW
	//					if( pMover->GetProp()->dwHideLevel == 1 )
	//						bAble = FALSE;		// 화살표 표시 안함.

						if( bAble )
						{
							if( nLevelIntv > 0 )	// 높은쪽은 1이라도 높으면 표시
							{
								if( nLevelIntv > 5 )
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvUp2 );
								else
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvUp );
							}
							else
							if( nLevelIntv < -1 )	// 낮은쪽은 2단계 이상 차이나야 표시
							{
								if( nLevelIntv < -5 )
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvDn2 );
								else
									p2DRender->RenderTexture( CPoint( point.x+30, point.y - 20 ), &m_texLvDn );
							}
						}

	#ifdef __YATTRICON
						if( pMover->GetProp() )
						{
							if( pMover->GetProp()->eElementType )
							{
								point.y -= 25;
								
								m_texAttrIcon.Render( p2DRender, point, pMover->GetProp()->eElementType-1, 255, 1.5f, 1.5f );
							}
						}
	#endif //__YATTRICON				
					}
					else
					// 타겟이 플레이어인데, 내가 운영자라면 상대의 정보를 볼수 있다.
					if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
					{
						CString string;
						string.Format( "Lv(%d), Gold(%d), Karma(%d), ", pMover->GetLevel(), pMover->GetGold(), pMover->GetKarma() );
						strTemp += string;
					}
					// 운영자라면 선택된 캐릭터의 최대HP와 현재 남은 HP를 표시해준다.
					if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
					{
						char buffer[100] = { 0 };
						sprintf( buffer, "HP(%d/%d), ", pMover->GetHitPoint(), pMover->GetMaxHitPoint() );
						strTemp += buffer;
						
					}
	#ifdef __YMONSTER_BUFFRENDER
					RenderMoverBuff( pMover, p2DRender );
	#endif //__YMONSTER_BUFFRENDER	
					p2DRender->TextOut( rect.left, rect.top + 12, strTemp );
					p2DRender->SetFont( pOldFont );	
				}
			}
		}
	}
#ifdef __S1011_PING
#ifdef __ONLINE	
	CRect rect;
	rect = GetClientRect();
	TCHAR strPing[128];
	_stprintf( strPing, _T("%d ms"), g_Neuz.m_dwPingTime );
	if( g_Neuz.m_dwPingTime > 450 )
		p2DRender->TextOut( rect.right / 2 - 47, 1, strPing, 0xffff0000 );
	else if( g_Neuz.m_dwPingTime > 100 )
		p2DRender->TextOut( rect.right / 2 - 47, 1, strPing, 0xffffffa0 );
	else
		p2DRender->TextOut( rect.right / 2 - 47, 1, strPing, 0xffffff00 );

#endif // not online
#endif // __S1011_PING
	if( pObj && pObj->GetType() != OT_CTRL )
	{
		if( pObj->IsCull() == TRUE )	// 재조정할것도 없고 컬링되서 안보이면 안찍음.
		{
		} else
		{
			CRect rectBound;
			GetBoundRect( pObj, &rectBound );
			RenderFocusObj( rectBound, D3DCOLOR_ARGB( 100, 255,  0,  0 ), 0xffffff00);
		}
	}
}
void CWndWorld::RenderFocusObj( CRect rect, DWORD dwColor1, DWORD dwColor2 )
{
	BOOL	bFly = g_pPlayer->m_pActMover->IsFly();

	CPoint pt1, pt2, pt3, pt4;
	if( bFly )
	{
		pt1 = CPoint( rect.left - 16, rect.top - 32   );
		pt2 = CPoint( rect.right - 16, rect.top - 32   );
		pt3 = CPoint( rect.left -16 , rect.bottom  );
		pt4 = CPoint( rect.right - 16, rect.bottom );
	} else
	{
		pt1 = CPoint( rect.left - 32, rect.top - 32   );
		pt2 = CPoint( rect.right, rect.top - 32   );
		pt3 = CPoint( rect.left -32 , rect.bottom );
		pt4 = CPoint( rect.right, rect.bottom );
	}
	
	CWorld* pWorld	= g_WorldMng.Get();
	CObj* pObj = pWorld->GetObjFocus();

//	if( i < 0 ) i = 0;
//	i *= 2;

	// 타겟표시의 4귀퉁이 그림.
	if( (m_bSelectTarget || m_bAutoAttack) && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPeaceful() == FALSE ) // pObj == g_pPlayer->GetDestObj() )
	{
		if( bFly )
		{
			m_texTargetFly.GetAt( 8 )->Render( &g_Neuz.m_2DRender, pt1 );		// 붉은색 표시.
			m_texTargetFly.GetAt( 9 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTargetFly.GetAt( 10)->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTargetFly.GetAt( 11)->Render( &g_Neuz.m_2DRender, pt4 );
		} else
		{
			m_texTarget.GetAt( 8 )->Render( &g_Neuz.m_2DRender, pt1 );		// 붉은색 표시.
			m_texTarget.GetAt( 9 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTarget.GetAt( 10)->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTarget.GetAt( 11)->Render( &g_Neuz.m_2DRender, pt4 );
		}
	}
	else
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			m_texTargetFly.GetAt( 0 )->Render( &g_Neuz.m_2DRender, pt1 );		// 흰색표시
			m_texTargetFly.GetAt( 1 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTargetFly.GetAt( 2 )->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTargetFly.GetAt( 3 )->Render( &g_Neuz.m_2DRender, pt4 );
		} else
		{
			m_texTarget.GetAt( 0 )->Render( &g_Neuz.m_2DRender, pt1 );		// 흰색표시
			m_texTarget.GetAt( 1 )->Render( &g_Neuz.m_2DRender, pt2 );
			m_texTarget.GetAt( 2 )->Render( &g_Neuz.m_2DRender, pt3 );
			m_texTarget.GetAt( 3 )->Render( &g_Neuz.m_2DRender, pt4 );
		}
	}

}
// 화면중심에서 pt방향으로 화살표 표시.
void CWndWorld::RenderFocusArrow( CPoint pt )
{
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj == NULL )	return;

	BOOL bAdjust = FALSE;		// 타겟방향 화살표 표시검사 대상이다.
	BOOL bAdjust2 = FALSE;		// 방향 화살표를 표시해야하는 상황이냐.
	CPoint	ptOut;				// 타겟이 화면을 벗어났을경우 화면테두리 좌표.
	CRect rectBound;
	if( g_pPlayer->m_pActMover->IsFly() )	// 주인공이 비행중이고

	{
		if( pObj->GetType() == OT_MOVER || pObj->GetType() == OT_SHIP ) // 잡은 타겟이 무버라면.
			bAdjust = TRUE;		// 타겟이 화면을 벗어나지 않게 함.
	}
	if( bAdjust )
	{
		GetBoundRect( pObj, &rectBound );
		int nWidth = (rectBound.right - rectBound.left);	// 타겟사각형의 가로세로 폭.
		int nHeight = (rectBound.bottom - rectBound.top);
		// 타겟의 중심좌표.
		ptOut = CPoint( rectBound.left + nWidth / 2, rectBound.top + nHeight / 2 );	
		if( (int)ptOut.x > (int)g_Neuz.GetCreationWidth() )	// 오른쪽으로 벗어남.
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.x = (LONG)g_Neuz.GetCreationWidth() - 32;		// 경계좌표를 설정.
		}
		if( (int)ptOut.x < 0 )	// 왼쪽으로 벗어났을때.
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.x = 32;
		}
		if( (int)ptOut.y > (int)g_Neuz.GetCreationHeight() )
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.y = (int)g_Neuz.GetCreationHeight() - 32;
		}
		if( (int)rectBound.top < 0 )		// 위로 벗어났을때.
		{
			bAdjust2 = TRUE;		// 화면을 벗어났으므로 화살표로 표시함.
			ptOut.y = 32;
		}
	}
	
	if( bAdjust2 == FALSE)	// 타겟이 화면내에 있으면 안찍음.
		return;

	pt = ptOut;

	D3DXVECTOR2	vC;
	vC.x = (FLOAT)g_Neuz.GetCreationWidth() / 2;
	vC.y = (FLOAT)g_Neuz.GetCreationHeight() / 2;
	FLOAT xDist = pt.x - vC.x;
	FLOAT yDist = pt.y - vC.y;
	FLOAT fTheta = atan2( yDist, xDist );
#ifdef _DEBUG
	FLOAT fAng = D3DXToDegree( fTheta );
#endif
	
	// 가장큰 화살표
	m_texTargetArrow.GetAt( 0 )->RenderRotate( &g_Neuz.m_2DRender, pt, fTheta );

	D3DXVECTOR2 vOut, vPt;	// 실제 화살표가 찍힐 좌표
//	CPoint	ptOut;			// 실제 화살표가 찍힐 좌표 CPoint버전.
	vPt.x = pt.x;	vPt.y = pt.y;

}

void CWndWorld::RenderGauFlight( C2DRender* p2DRender )
{
	if( g_pPlayer && !g_pPlayer->IsVisible() )
		return;

#ifndef __XGAU0520
	int	cx = (int)g_Neuz.GetCreationWidth() / 2;
	int cy = (int)g_Neuz.GetCreationHeight() / 2;

	CPoint	pt;

	pt.x = cx - 64;	// 게이지 크기가 가로 128이니까 /2해서 -64
	pt.y = cy + 80;

	m_texGauFlight.GetAt(1)->Render( p2DRender, pt );		// 수평 게이지.
	ItemProp *pItemProp = prj.GetItemProp( g_pPlayer->GetRideItemIdx() );
	if( pItemProp == NULL )
		return;

	TCHAR strText[128];
	_stprintf( strText, _T("%d Km/h"), g_nFlySpeed );
	p2DRender->TextOut( pt.x + 130, pt.y, strText, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	
//	FLOAT fPercent = (FLOAT)g_pPlayer->m_nFuel / pItemProp->dwFFuelReMax;	// 백분율.
	FLOAT fPercent = (FLOAT)g_nFlySpeed / 400;	// 그래프 최고속도가 400이라고 침.
	int nSpeed = (int)(fPercent * 128.0f);		// 백분율을 128단위로 변환,.
	
	pt.x += 6;
	pt.x += nSpeed;
	pt.y += 6;
	m_texGauFlight.GetAt(0)->Render( p2DRender, pt );	// 현재 위치 화살표.
	
#ifdef _DEBUG
	pt.x = cx;
	_stprintf( strText, _T("%d"), g_pPlayer->m_nFuel );
	p2DRender->TextOut( pt.x - 90, pt.y, strText, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
#endif
	
#endif // not xGau0520
}


void CWndWorld::RenderAltimeter()
{
	return;
	// 고도계

	CRect rect = GetClientRect();

	if( m_nWinSize == WSIZE_MAX )
	{
		int nHeight =  rect.Height() * 60 / 100;
		rect.left = rect.right - 30;
		rect.top += 130;
		rect.bottom -= 135;
	}
	else
	{
		//int nHeight =  rect.Height() * 60 / 100;
		rect.left = rect.right - 30;
		//rect.bottom = rect.top + nHeight;
	}
	g_Neuz.m_2DRender.RenderFillRect( rect, D3DCOLOR_ARGB( 0x30, 0, 0, 0 ) );
	rect.top += 10;
	rect.bottom -= 10;

	FLOAT fHigh1 = -1.0f, fLow1 = 999999.0f;
	FLOAT fHigh2 = -1.0f, fLow2 = 999999.0f;
	CObj* pObj;
	CLandscape* pLand;
//	FOR_LAND( &g_World, pLand, g_World.m_nVisibilityLand, FALSE )
	FOR_LAND( g_WorldMng.Get(), pLand, g_WorldMng.Get()->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			D3DXVECTOR3 vPos = pObj->GetPos();
			if( fHigh1 < vPos.y ) fHigh1 = vPos.y;
			if( fLow1 > vPos.y ) fLow1 = vPos.y;
		}
		END_OBJ
	}
	END_LAND

	if( fHigh1 == -1.0f && fLow1 == 999999.0f )
		return;
	fHigh2 = fHigh1;
	fLow2  = fLow1;
	if( m_fHigh == -1.0f )
	{
		m_fHigh = fHigh2 = fHigh1;
		m_fLow = fLow2  = fLow1;
	}
	else
	{
		if( fHigh1 < m_fHigh )
		{
			fHigh2 = m_fHigh - 2;
			if( fHigh2 < fHigh1 )
				fHigh2 = fHigh1;
		}
		else
		if( fHigh1 > m_fHigh )
		{
			fHigh2 = m_fHigh + 2;
			if( fHigh2 > fHigh1 )
				fHigh2 = fHigh1;
		}
		if( fLow1 > m_fLow )
		{
			fLow2 = m_fLow + 2;
			if( fLow2 > fLow1 )
				fLow2 = fLow1;
		}
		else
		if( fLow1 < m_fLow )
		{
			fLow2 = m_fLow - 2;
			if( fLow2 < fLow1 )
				fLow2 = fLow1;
		}
	}
//	CWorld* pWorld = &g_World;
	CWorld* pWorld	= g_WorldMng.Get();

//	FOR_LAND( &g_World, pLand, g_World.m_nVisibilityLand, FALSE )
	FOR_LAND( g_WorldMng.Get(), pLand, g_WorldMng.Get()->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			CMover* pMover = (CMover*) pObj;
			/*
			if( g_WndMng.m_nObjectFilter == OBJFILTER_PLAYER && pMover->IsPlayer() == FALSE )
				continue;
			if( g_WndMng.m_nObjectFilter == OBJFILTER_MONSTER && ( pMover->IsPlayer() == TRUE || pMover->IsPeaceful() ) )
				continue;
			if( g_WndMng.m_nObjectFilter == OBJFILTER_NPC && ( pMover->IsPlayer() == TRUE || pMover->IsPeaceful() ) )
				continue;
				*/
			if( pMover->IsMode( TRANSPARENT_MODE ) )	// 대상이 투명모드일땐 타겟 안됨.			
				continue;	
			DWORD dwColor = 0xffffffff;
			if( pObj == g_pPlayer )
				dwColor = 0xffffff00;

			D3DXVECTOR3 vPos = pObj->GetPos();
			vPos.y -= fLow2;
			// ( fHigh - fLow ) : vPos.z = rect.Height() : z
			int y = vPos.y * (FLOAT) rect.Height() / ( fHigh2 - fLow2 );
			g_Neuz.m_2DRender.RenderLine( CPoint( rect.left + 7, rect.bottom - y ), CPoint( rect.right, rect.bottom - y ), dwColor );
			int nFontMid = g_Neuz.m_2DRender.m_pFont->GetMaxHeight() / 2;
			CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( pMover->GetName() );
			size.cx += 5;
			g_Neuz.m_2DRender.TextOut( rect.left - size.cx + 1, rect.bottom - y + 1 - nFontMid, pMover->GetName(), 0xff000000 );
			g_Neuz.m_2DRender.TextOut( rect.left - size.cx , rect.bottom - y - nFontMid, pMover->GetName(), dwColor );
			g_Neuz.m_2DRender.RenderFillTriangle( 
				CPoint( rect.left, rect.bottom - y - 5 ), 
				CPoint( rect.left, rect.bottom - y + 5 ), 
				CPoint( rect.left + 5, rect.bottom - y ), 
				dwColor	);
		}
		END_OBJ
	}
	END_LAND

	pObj = pWorld->GetObjFocus();
	if( pObj )
	{
		DWORD dwColor = dwColor = 0xffff0000;
		CMover* pMover = (CMover*)pObj;
		D3DXVECTOR3 vPos = pObj->GetPos();
		vPos.y -= fLow2;
		// ( fHigh - fLow ) : vPos.z = rect.Height() : z
		int y = vPos.y * (FLOAT) rect.Height() / ( fHigh2 - fLow2 );
		g_Neuz.m_2DRender.RenderLine( CPoint( rect.left + 7, rect.bottom - y ), CPoint( rect.right, rect.bottom - y ), dwColor );
		int nFontMid = g_Neuz.m_2DRender.m_pFont->GetMaxHeight() / 2;
		CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( pMover->GetName() );
		size.cx += 5;
		g_Neuz.m_2DRender.TextOut( rect.left - size.cx + 1, rect.bottom - y + 1 - nFontMid, pMover->GetName(), 0xff000000 );
		g_Neuz.m_2DRender.TextOut( rect.left - size.cx , rect.bottom - y - nFontMid, pMover->GetName(), dwColor );
		g_Neuz.m_2DRender.RenderFillTriangle( 
			CPoint( rect.left, rect.bottom - y - 5 ), 
			CPoint( rect.left, rect.bottom - y + 5 ), 
			CPoint( rect.left + 5, rect.bottom - y ), 
			dwColor	);
	}
	FLOAT y = (FLOAT) rect.Height() / ( fHigh2 - fLow2 );
	for( FLOAT j = 0; j < rect.Height(); j += y )
	{
		g_Neuz.m_2DRender.RenderLine( CPoint( rect.left + 15, rect.bottom - j ), CPoint( rect.right, rect.bottom - j ), 0x80ffffff );
	}
	m_fHigh = fHigh2;
	m_fLow  = fLow2;
}
/*
#define MMI_DIALOG        0
#define MMI_TRADE         1
#define MMI_FIGHT         2
#define MMI_MESSAGE       3
#define MMI_ADD_MESSANGER 4
#define MMI_INVITE_PARTY    5

#define MAX_MOVER_MENU    6 
*/
/*
TCHAR g_aszMoverMenu[ MAX_MOVER_MENU ][ 32 ] =
{
	_T( "대화" ),
	_T( "퀘스트" ),
	_T( "거래" ),
	_T( "대전" ),
	_T( "메시지" ),
	_T( "메신저 추가" ),
	_T( "극단 초청" ),
	_T( "컴퍼니 초청" ),
	_T( "장소 지정" ),
	_T( "보관" ),
	_T( "듀얼신청" )

};
*/
struct LANGFONT
{
	CHAR szFontFirst[ 32 ], szFontSecond[ 32 ],nDivTitle,nDivCaption;
} g_langFont[ LANG_MAX ] =
{ 
	"휴먼매직체"   , "굴림"            , 20,  50,
	"Arial"        , "Arial"           , 20,  50, //eng
	"MS Gothic"    , "굃굍 굊긕긘긞긏" , 40, 100, // jap
	"MS Sans Serif", "MS Sans Serif"   , 20,  50, //tha
	"MingLiU"      , "MingLiU"         , 20,  50, // twn
	"SimSun"       , "SimSun"          , 20,  50  // chi
};
void CWndWorld::OnInitialUpdate()
{
	ConnectLog( "WndWorld OnInit" );
	CWndNeuz::OnInitialUpdate();
	AddWndStyle( WBS_THICKFRAME );  

#ifdef __V050322_CACHEQUEST
	m_strTexture = "WndTile200.tga";
#endif

	SetSizeMax();
	CRect rectClient = GetClientRect();

#ifndef __VTITLECAPTION_JAPAN
	//g_osVersion = WINDOWS_ME;
	CString strFont;
	strFont = g_langFont[ g_Option.m_nLanguage ].szFontFirst;
	if( IsFontInstalled( strFont ) == FALSE )
		strFont = g_langFont[ g_Option.m_nLanguage ].szFontSecond;
	if( g_osVersion > WINDOWS_ME )
	{
		m_pFontAPICaption = new CD3DFontAPI( strFont, rectClient.Width() / g_langFont[ g_Option.m_nLanguage ].nDivCaption );//, D3DFONT_BOLD );
		m_pFontAPICaption->m_nOutLine = 2;
		m_pFontAPICaption->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPICaption->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPICaption->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPICaption->InitDeviceObjects( m_pApp->m_pd3dDevice );

		m_pFontAPITitle = new CD3DFontAPI( strFont, rectClient.Width() / g_langFont[ g_Option.m_nLanguage ].nDivTitle );//, D3DFONT_BOLD );
		m_pFontAPITitle->m_nOutLine = 2;
		m_pFontAPITitle->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPITitle->m_dwBgColor = D3DCOLOR_ARGB( 255, 40, 100, 220 );
		m_pFontAPITitle->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPITitle->InitDeviceObjects( m_pApp->m_pd3dDevice );
#ifdef __V050322_CACHEQUEST
		m_pFontAPITime = new CD3DFontAPI( g_langFont[ g_Option.m_nLanguage ].szFontSecond, rectClient.Width() / 40 );//, D3DFONT_BOLD );
		m_pFontAPITime->m_nOutLine = 2;
		m_pFontAPITime->m_dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255);
		m_pFontAPITime->m_dwBgColor = D3DCOLOR_ARGB( 255, 220, 100, 40 );
		m_pFontAPITime->m_dwFlags = D3DFONT_FILTERED;
		m_pFontAPITime->InitDeviceObjects( m_pApp->m_pd3dDevice );
#endif //__V050322_CACHEQUEST
	}
#endif //__VTITLECAPTION_JAPAN

	m_wndMenuMover.CreateMenu( this );	

#ifndef __V050322_CACHEQUEST
	m_strTile = "WndTile200.tga";
#endif

	m_texTarget.LoadScript( D3DDEVICE, MakePath( DIR_ICON, "icon_target.inc" ) );
	m_texTargetFly.LoadScript( D3DDEVICE, MakePath( DIR_ICON, "icon_FlightTargetB.inc" ) );			// 비행모드시 타겟 4귀퉁이.
	m_texTargetArrow.LoadScript( D3DDEVICE, MakePath( DIR_ICON, "icon_FlightTargetArrow.inc" ) );	// 비행모드시 타겟방향을 가르키는 화살표
	m_texTargetArrow.GetAt(0)->m_ptCenter.x += 32;
	m_texTargetArrow.GetAt(0)->m_ptCenter.y += 32;
	m_texGauFlight.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "Theme_GauFlight.inc" ) );		// 비행모드시 게이지 인터페이스
 #ifdef __XTURBO
	m_texFontDigital.LoadScript( D3DDEVICE, MakePath( DIR_THEME, "Theme_FontDigital1.inc" ) );		// 디지탈모양의 폰트.
 #endif
	
	//m_texFlaris.LoadTexture( D3DDEVICE, MakePath( DIR_EFFECT, "WelcomeToFlaris.tga" ), 0xff000000 );
	//m_texFlaris.m_ptCenter = CPoint( m_texFlaris.m_size.cx / 2, m_texFlaris.m_size.cy / 2 );

	m_meshArrow.InitDeviceObjects( m_pApp->m_pd3dDevice );
	m_meshArrow.LoadModel( "etc_arrow.o3d" );

  #ifdef __NPP_050308
	m_meshArrowWanted.InitDeviceObjects( m_pApp->m_pd3dDevice );
	m_meshArrowWanted.LoadModel( "arrow.o3d" );
	m_bRenderArrowWanted = FALSE;
	m_dwRenderArrowTime  = 0;
  #endif //__NPP_050308	
	//m_texTargetGauge.LoadTexture( D3DDEVICE, MakePath( DIR_THEME, "target.tga" ), 0 );

	
	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texLvUp.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvUp.bmp" ), 0xffff00ff, TRUE );
	m_texLvDn.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvDn.bmp" ), 0xffff00ff, TRUE );
	m_texLvUp2.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvUp2.bmp" ), 0xffff00ff, TRUE );
	m_texLvDn2.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "LvDn2.bmp" ), 0xffff00ff, TRUE );


	int i = 0;
	BUFFSKILL   buffskill;
	for( i=0; i<prj.m_aPropAddSkill.GetSize(); i++ )
	{
		AddSkillProp* pAddSkill = prj.m_aPropAddSkill.GetAt(i);

		if( pAddSkill )
		{
			if( pAddSkill->dwSkillTime != -1 )
			{
				ItemProp* pItem = prj.GetSkillProp(pAddSkill->dwName);

				if( pItem )
				{
					buffskill.m_bFlsh    = 0;
					buffskill.m_nAlpha  = 192;
					
					buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ICON, pItem->szIcon ), 0xffff00ff );

					m_pBuffTexture[0].insert( make_pair(pItem->dwID, buffskill) );
				}
			}
		}
	}

	for( i=0; i<prj.m_aPartySkill.GetSize(); i++ )
	{
		ItemProp* pItemProp = prj.m_aPartySkill.GetAt(i);
		
		if( pItemProp )
		{
			if( pItemProp->dwSkillTime != -1 )
			{
				buffskill.m_bFlsh    = 0;
				buffskill.m_nAlpha  = 192;
				
				buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ICON, pItemProp->szIcon ), 0xffff00ff );
				m_pBuffTexture[1].insert( make_pair(pItemProp->dwID, buffskill) );
			}
		}
	}

#ifdef __XITEMBUFF
	for( i=0; i<prj.m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp = (ItemProp*)prj.m_aPropItem.GetAt(i);
		
		if( pItemProp )
		{
			if( pItemProp->dwSkillTime != -1 )
			{
				buffskill.m_bFlsh    = 0;
				buffskill.m_nAlpha  = 192;
				
				buffskill.m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );
				m_pBuffTexture[2].insert( make_pair(pItemProp->dwID, buffskill) );
			}
		}
	}
#endif //__XITEMBUFF
	


	switch( m_rectWindow.Width() )
	{
	default:
	case 800:
		m_nLimitBuffCount = 7;
		break;
	case 1024:
		m_nLimitBuffCount = 10;
		break;
	case 1280:
		m_nLimitBuffCount = 13;
		break;
	}
	
#ifdef __ONLINE
	// 아이콘 텍스쳐 로딩

	CString str;
	for( i=0; i<CUSTOM_LOGO_MAX; i++ )
	{
		str.Format( "Icon_CloakSLogo%02d.jpg", i+1 );
		
		if( !m_pTextureLogo[i].LoadTexture( g_Neuz.GetDevice(), MakePath( DIR_ICON, str ), D3DCOLOR_XRGB(0,0,0), FALSE ) )
		{
			Error( "길드 로고 텍스쳐 로딩 실패 : %s", str );
		}
		
	}
#endif

#ifdef __YCLOTH
	if( g_pPlayer )
	{
		D3DXMATRIX  mat1;
		
		mat1 = ((CModelObject*)g_pPlayer->m_pModel)->m_mUpdateBone[10];

		mat1 = mat1 * g_pPlayer->GetMatrixTrans();

		g_Cloth.DeleteDeviceObjects();
		g_Cloth.Init( 13, D3DXVECTOR3( 0.0f, -0.98f, 0.0f), 2.0f, 0.1f, 0.03f, 0.9f, 5, m_pTextureLogo[0].m_pTexture, &mat1 );
		
	}
#endif
	
	m_texMsgIcon.LoadScript( m_pApp->m_pd3dDevice, "icon\\icon_IconMessenger.inc" );
	
#ifdef __YATTRICON
	m_texAttrIcon.LoadScript( m_pApp->m_pd3dDevice, "icon\\Icon_MonElemantkind.inc" );
#endif //_YATTRICON	
	
	for( int j = 0 ; j < SM_MAX ; ++j )
	{
		if( j != SM_RESIST_ATTACK_LEFT && j != SM_RESIST_ATTACK_RIGHT && j != SM_RESIST_DEFENSE )
		{
			ItemProp* pItem = prj.GetItemProp( g_AddSMMode.dwSMItemID[j] );
			if( pItem )
				m_dwSMItemTexture[j] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItem->szIcon ), 0xffff00ff );
		}
	}

	DWORD dwArry[10] = { II_CHR_SYS_SCR_FIREASTONE,
						II_CHR_SYS_SCR_WATEILSTONE,
						II_CHR_SYS_SCR_LIGHTINESTONE,
						II_CHR_SYS_SCR_WINDYOSTONE,
						II_CHR_SYS_SCR_EARTHYSTONE,
						II_CHR_SYS_SCR_DEFIREASTONE,
						II_CHR_SYS_SCR_DEWATEILSTONE,
						II_CHR_SYS_SCR_DELIGHTINESTONE,
						II_CHR_SYS_SCR_DEWINDYOSTONE,
						II_CHR_SYS_SCR_DEEARTHYSTONE
						};
	
	ItemProp* pItem = NULL;
	for( int kk = 0; kk < 10; kk++ )
	{
		pItem = prj.GetItemProp( dwArry[kk] );

		if( pItem )
			m_dwSMResistItemTexture[kk] = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_ITEM, pItem->szIcon ), 0xffff00ff );	
	}
	
	m_wndTitleBar.SetVisible( FALSE );

	g_DialogMsg.ClearVendorObjMsg();
	
	Projection( D3DDEVICE );
	ConnectLog( "WndWorld OnInit end" );
	
	g_WndMng.m_ChatString = "";

	
#ifdef __YGUIDESYSTEM0923
	SAFE_DELETE(m_pWndGuideSystem);
	m_pWndGuideSystem = new CWndGuideSystem;
	m_pWndGuideSystem->Initialize();

	if( g_pPlayer->m_dwMode & FRESH_MODE )
	{
		m_pWndGuideSystem->CreateGuideSelection();
		m_pWndGuideSystem->m_dwGuideLevel = 0;
		*g_Option.m_pGuide = 0;
	}
	else
	{
#ifdef __ONLINE		
		m_pWndGuideSystem->m_dwGuideLevel = *g_Option.m_pGuide;
#else //__ONLINE
		g_Option.m_pGuide = &g_Option.m_nGuide1;
		m_pWndGuideSystem->m_dwGuideLevel = *g_Option.m_pGuide;		
#endif //__ONLINE

		if( m_pWndGuideSystem->m_dwGuideLevel == 2 )
			m_pWndGuideSystem->m_bVisible = FALSE;
								

		m_pWndGuideSystem->GuideStart(FALSE);
	}
#endif //CWndCGuideSystem 

	g_nSkillCurSelect = -1;
	
#ifdef __YADV_RESKILLPOINT
	if( g_pPlayer && g_pPlayer->m_dwSkillPoint > 0 )
#else //__YADV_RESKILLPOINT
	if( g_pPlayer && g_pPlayer->m_dwSkillPoint > 0 && g_pPlayer->IsExpert() )
#endif //__YADV_RESKILLPOINT
	{
#ifdef __YADV_RESKILLPOINT
		g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SKILL );
/*
		if( g_Option.m_bIsShowReSkillMessage )
		{
			SAFE_DELETE(m_pWndReSkillPoint_Text);
			m_pWndReSkillPoint_Text = new CWndReSkillPoint_Text;
			m_pWndReSkillPoint_Text->Initialize(this);
		}		
*/
#else //__YADV_RESKILLPOINT
		SAFE_DELETE(m_pWndReSkillPoint);

		switch( g_pPlayer->GetJob() )
		{
			case JOB_MERCENARY:
				m_pWndReSkillPoint = new CWndReSkillPoint_Me;
				break;
			case JOB_ASSIST:
				m_pWndReSkillPoint = new CWndReSkillPoint_As;
				break;
			case JOB_MAGICIAN:
				m_pWndReSkillPoint = new CWndReSkillPoint_Ma;
				break;
		}

		if( m_pWndReSkillPoint )
		{
			m_pWndReSkillPoint->Initialize(this);
			m_pWndReSkillPoint->InitItem( g_pPlayer->m_dwSkillPoint, g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );

			if( g_Option.m_bIsShowReSkillMessage )
			{
				SAFE_DELETE(m_pWndReSkillPoint_Text);
				m_pWndReSkillPoint_Text = new CWndReSkillPoint_Text;
				m_pWndReSkillPoint_Text->Initialize(this);
			}
		}
		else
			Error( "스킬재분배창 초기화 에러 NULL" );
#endif //__YADV_RESKILLPOINT
	}		


	g_MapStrToObjId.clear();

#ifdef __NPP_050308
	g_Neuz.m_NeuzEnemy.Clear();
#endif // __NPP_050308
/*
#ifdef __POST0906
	g_DPlay.SendQueryMailBox();
#endif //__POST0906
*/
}
BOOL CWndWorld::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 100,100,500,400 );
	SetTitle( _T( "World" ) );
	return CWndNeuz::Create(WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME|WBS_MANAGER|WBS_MAXIMIZEBOX,rect,pWndParent,dwWndId);
}
BOOL CWndWorld::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	return FALSE;
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case WTBID_CLOSE:
				return FALSE;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndWorld::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	CCtrl* pFocusObj = (CCtrl*)g_WorldMng()->GetObjFocus();
	CMover* pFocusMover = (CMover*)pFocusObj;
	if( pWndBase == &m_wndMenuMover && pFocusObj )
	{
		switch( nID )
		{
	#ifdef __GUILDCOMBAT
		case MMI_GUILDWAR_APP:
			{
				CWndGuildWarAppConfirm* pWndGuildCombat = new CWndGuildWarAppConfirm;

				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		case  MMI_GUILDWAR_STATE:
			{
				g_DPlay.SendGuildCombatList( g_pPlayer->GetId() );
			}
			break;
		case MMI_GUILDWAR_CANCEL:
			{
				CWndGuildWarCancelConfirm* pWndGuildCombat = new CWndGuildWarCancelConfirm;

				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		case MMI_GUILDWAR_JOIN:
			{
				CWndGuildWarJoinConfirm* pWndGuildCombat = new CWndGuildWarJoinConfirm;
				
				if(pWndGuildCombat)
					pWndGuildCombat->Initialize( NULL );
			}
			break;
		case MMI_GUILDCOMBAT_RANKING:
			{
				g_DPlay.SendGuildCombatRanking( g_pPlayer->GetId() );
			}
			break;
	#endif //__GUILDCOMBAT
	#ifdef __Y_POST_0912
		case MMI_POST:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "개인간 거래중이므로 뷰티샵을 이용할수 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}
				
	#ifdef __Y_POST_0912
				pWndBaseBuf = g_WndMng.GetWndBase( APP_POST );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_MAIL_OTHERWORK ), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK ) );
					break;
				}		
	#endif //__Y_POST_0912				
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKTRADESELF ), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 은행을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}				
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "아이템 수리중이므로 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION  ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "상거래중이므로 뷰티샵을 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 뷰티샵을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING) );
					break;
				}
				
				g_WndMng.CreateApplet( APP_INVENTORY );
				
				SAFE_DELETE( g_WndMng.m_pWndPost );
				g_WndMng.m_pWndPost = new CWndPost;
				g_WndMng.m_pWndPost->Initialize( NULL, APP_POST );	
			}
			break;
	#endif //__Y_POST_0912
		case MMI_BEAUTYSHOP:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "개인간 거래중이므로 뷰티샵을 이용할수 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "아이템 수리중이므로 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION  ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "상거래중이므로 뷰티샵을 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP ), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 뷰티샵을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING) );
					break;
				}

				g_WndMng.CreateApplet( APP_INVENTORY );
				
				SAFE_DELETE( g_WndMng.m_pWndBeautyShop );
				g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;
				g_WndMng.m_pWndBeautyShop->Initialize( NULL, APP_BEAUTY_SHOP );
				
			}
			break;
		case MMI_MESSAGE:
			{
				g_WndMng.OpenMessage( pFocusMover->GetName() );
			}
			break;
		case MMI_ADD_MESSENGER:
			{
				if( g_WndMng.m_Messenger.IsFriend( pFocusMover->m_idPlayer ) )
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0051) ) );
				}
				else
				{
					g_DPlay.SendAddFriendReqest( pFocusMover->m_idPlayer );
					CString str;
					str.Format( prj.GetText(TID_GAME_MSGINVATE), pFocusMover->GetName() );
					g_WndMng.PutString( str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE) );
					
				}
			}
			break;
		case MMI_MARKING: // 장소 마킹 
#ifdef __ONLINE
			g_DPlay.SendHdr( PACKETTYPE_SETLODELIGHT );
#else	// __ONLINE
			g_pPlayer->SetMarkingPos();
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0010) ) );
//			g_WndMng.OpenMessageBox( _T( "로드라이트에 장소를 마킹했습니다." ) );
#endif	// __ONLINE
			break;
		case MMI_DIALOG: // dialog
#ifdef __ONLINE

	#ifdef __YADV_RESKILLPOINT
			if( g_pPlayer && g_pPlayer->m_dwSkillPoint )
			{
				g_WndMng.PutString( prj.GetText(TID_RESKILLPOINT_NOTDIALOG), NULL, 0xffff0000 );
				break;
			}
	#endif //__YADV_RESKILLPOINT
			

			if( GetTickCount() - m_dwDropTime < 3000 )
				break;

			{
				CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_VENDOREX );

				if( pWndBase )
					break;
			}
			g_DPlay.SendScriptDialogReq( ( (CMover*)pFocusObj )->GetId(), NULL, 0, 0, 0, 0 );
			SAFE_DELETE( g_WndMng.m_pWndDialog );
			g_WndMng.m_pWndDialog = new CWndDialog;
#ifdef __V050322_CACHEQUEST
			g_WndMng.m_pWndDialog->m_idMover = ((CMover*)pFocusObj)->GetId();
#else// __V050322_CACHEQUEST
			g_WndMng.m_pWndDialog->m_pMover = (CMover*)pFocusObj;
#endif// __V050322_CACHEQUEST
			g_WndMng.m_pWndDialog->Initialize( &g_WndMng, APP_DIALOG );
#else	// __ONLINE
			if( g_WndMng.m_pWndDialog == NULL )
			{
				g_WndMng.m_pWndDialog = new CWndDialog;
#ifdef __V050322_CACHEQUEST
				g_WndMng.m_pWndDialog->m_idMover = ((CMover*)pFocusObj)->GetId();
#else// __V050322_CACHEQUEST
				g_WndMng.m_pWndDialog->m_pMover = (CMover*)pFocusObj;
#endif// __V050322_CACHEQUEST
				g_WndMng.m_pWndDialog->Initialize( &g_WndMng, APP_DIALOG );
			}
#endif	// __ONLINE

			break;
	#ifdef __QUERYEQUIP0630
		case MMI_QUERYEQUIP: // 살펴보기

			{
				if( pFocusMover && pFocusMover->IsPlayer() )
				{					
					g_DPlay.SendQueryEquip( pFocusMover->GetId() );
				}
			}
			break;
	#endif //__QUERYEQUIP0630

		case MMI_TRADE: //  거래 
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
								
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKTRADESELF ), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF ) );
					break;
				}
	#ifdef __Y_POST_0912
				pWndBaseBuf = g_WndMng.GetWndBase( APP_POST );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_MAIL_OTHERWORK ), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK ) );
					break;
				}		
	#endif //__Y_POST_0912				

				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "이미 은행을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "뷰티샵을 이용중입니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "아이템 수리중이므로 이용할수가 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOACTION ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_TRADE );
				if( pWndBaseBuf )
				{
			#ifndef __THAI_0525VER
					g_WndMng.PutString( prj.GetText( TID_GAME_YETTRADE ), NULL, prj.GetTextColor( TID_GAME_YETTRADE ) );
			#endif//__THAI_0525VER
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );	
				if( pWndBaseBuf )
				{
			#ifndef __THAI_0525VER
					g_WndMng.PutString( prj.GetText( TID_GAME_YETTRADE ), NULL, prj.GetTextColor( TID_GAME_YETTRADE ) );
			#endif//__THAI_0525VER
					break;
				}
#ifdef __NPP_050308
				KarmaProp* pProp = prj.GetKarmaProp( g_pPlayer->m_nSlaughter );
				if( pProp )
				{
					if( pProp->nGrade == -6 )					
					{
						g_WndMng.PutString( prj.GetText(TID_PK_MARKETNOUSE), NULL, prj.GetTextColor(TID_PK_MARKETNOUSE) );
						break;
					}
				}
#endif // __NPP_050308
				
#ifndef __ONLINE

				g_WndMng.CreateApplet( APP_INVENTORY );

				if( pFocusMover->IsPlayer() )
				{
					g_WndMng.CreateApplet( APP_TRADE );
				}
				else
				{
					SAFE_DELETE( g_WndMng.m_pWndShop );
					g_WndMng.m_pWndShop = new CWndShop;
					//g_WndMng.m_pWndShop->m_pMover = (CMover*)pFocusObj;
					g_WndMng.m_pWndShop->m_pMover = pFocusMover;
					g_WndMng.m_pWndShop->Initialize( NULL, APP_SHOP_ );
				}
#else
				if( pFocusMover->IsPlayer() )
				{
					g_DPlay.SendConfirmTrade( pFocusMover );
					CString sMessageBuf;
					//sMessageBuf = pFocusMover->GetName();
					//sMessageBuf += "님에게 거래를 요청했습니다.";
					//g_WndMng.PutString( sMessageBuf, NULL, 0xffffff00 );
					sMessageBuf.Format( prj.GetText(TID_GAME_TRADEREQUEST ), pFocusMover->GetName() );
					g_WndMng.PutString( sMessageBuf, NULL, prj.GetTextColor(TID_GAME_TRADEREQUEST) );
					
//					g_DPlay.SendTrade( pFocusMover );
				}
				else
				{
					if( g_pPlayer->m_vtInfo.GetOther() == NULL )
					{
#ifdef __NPP_050308
						KarmaProp* pProp = prj.GetKarmaProp( g_pPlayer->m_nSlaughter );
						if( pProp )
						{
							if( pProp->fDiscountRate != -1.0f )
								g_DPlay.SendOpenShopWnd( pFocusMover->GetId() );
							else
								g_WndMng.PutString( prj.GetText( TID_PK_MARKETNOUSE ), NULL, prj.GetTextColor( TID_PK_MARKETNOUSE ) );
						}
#else // __NPP_050308						
						g_DPlay.SendOpenShopWnd( pFocusMover->GetId() );
#endif // __NPP_050308
					}
				}
#endif
				break;
			}
		case MMI_REPAIR:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKEBANK ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKEBANK ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKETRADE ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKETRADE ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "상거래중이므로 아이템 수리를 이용하실수 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKESHOP  ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKESHOP  ) );
					break;
				}
				
					
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
				{
					//g_WndMng.PutString( "뷰티샵 이용중이므로 아이템 수리를 이용하실수 없습니다", NULL, 0xffffff00 );
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TAKEBEAUTY ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKEBEAUTY ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_VENDOREX );
				
				if( pWndBaseBuf )
				{
					// 개인상점 중에는 아이템 수리를 이용하실수 없습니다
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_TRADEING ), NULL, prj.GetTextColor(TID_GAME_REPAIR_TRADEING ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					// 이미 수리중입니다.
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_STILL ), NULL, prj.GetTextColor(TID_GAME_REPAIR_STILL ) );
					break;
				}

				g_WndMng.m_pWndRepairItem = new CWndRepairItem;
				g_WndMng.m_pWndRepairItem->Initialize( &g_WndMng, APP_REPAIR );
				
				break;
			}
		case MMI_BANKING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOBANK ), NULL, prj.GetTextColor(TID_GAME_TRADENOBANK ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOACTION ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKNOTRADE  ), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE  ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
				{
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING   ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING  ) );
					break;
				}
#ifdef __INSTANT_BANK0715
				g_DPlay.SendOpenBankWnd( NULL_ID, 0 );
#else	// __INSTANT_BANK0715
				g_DPlay.SendOpenBankWnd();
#endif	// __INSTANT_BANK0715
				break;
			}

		case	MMI_GUILDBANKING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADENOBANK ), NULL, prj.GetTextColor(TID_GAME_TRADENOBANK ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_REPAIR_NOACTION ), NULL, prj.GetTextColor( TID_GAME_REPAIR_NOACTION ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKNOTRADE  ), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE  ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BANKSTILLUSING   ), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING   ) );
					break;
				}
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_BEAUTYSHOPUSING   ), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING  ) );
					break;
				}

				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
				{
					break;
				}
				
				g_DPlay.SendOpenGuildBankWnd();

				break;
			}
		case MMI_RANK_GUILD:
			{
				SAFE_DELETE( g_WndMng.m_pWndRankGuild );
				g_WndMng.m_pWndRankGuild = new CWndRankGuild;
				g_WndMng.m_pWndRankGuild->Initialize( &g_WndMng, APP_RANK_GUILD );
				
				g_DPlay.SendGuildRank( CGuildRank::Instance()->m_Version );
				break;
			}
		case MMI_RANK_WAR:
			{
				SAFE_DELETE( g_WndMng.m_pWndRankWar );
				g_WndMng.m_pWndRankWar = new CWndRankWar;
				g_WndMng.m_pWndRankWar->Initialize( &g_WndMng, APP_RANK_WAR );
				
				g_DPlay.SendGuildRank( CGuildRank::Instance()->m_Version );
				break;
			}
		case MMI_RANK_INFO:
			{
				SAFE_DELETE( g_WndMng.m_pWndRankInfo );
				g_WndMng.m_pWndRankInfo = new CWndRankInfo;
				g_WndMng.m_pWndRankInfo->Initialize( &g_WndMng, APP_RANK_INFO );

				g_DPlay.SendGuildRank( CGuildRank::Instance()->m_Version );
				break;
			}

		case MMI_INVITE_PARTY: // 파티참여
			{ 
				if( pFocusMover->IsPlayer() )
				{
					if( g_pPlayer->m_nDuel == 2 )
					{
						DWORD dwText = TID_GAME_PPVP_ADDPARTY;	// 극단 듀얼중 초청불갑니다~
						g_WndMng.PutString( prj.GetText(dwText), NULL, prj.GetTextColor(dwText) );
						break;
					}
					if( g_Party.m_nKindTroup == 0 )	// 단막극단일때는 포인트가 필요없이 초대할수 있다
					{
						if( g_Party.m_aMember[0].m_uPlayerId == 0 || g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
						{
							int nMaxPartyMember = MAX_PTMEMBER_SIZE;
							CString szMessageBuf;
							
							if( g_Party.GetLevel() <= 5 )
							{
								nMaxPartyMember	= 3;
								szMessageBuf = prj.GetText(TID_GAME_FULLPARTY1);
							}
							else
							{
								nMaxPartyMember	= 4;
								szMessageBuf = prj.GetText(TID_GAME_FULLPARTY2);
							}
							
							if( nMaxPartyMember >= g_Party.m_nSizeofMember + 1 )
							{
								g_DPlay.SendPartyMemberRequest( g_pPlayer, pFocusMover->m_idPlayer, FALSE );
							}
							else
							{
								g_WndMng.PutString( szMessageBuf, NULL, prj.GetTextColor(TID_GAME_FULLPARTY1) );
							}
						}
						else
						{
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0007) ) );
						}
					}
					else // 순회극단일대 스킬을 이용하여 초대

					{
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0024) ) );
					}
				}
				break;
			}
		case MMI_INVITE_COMPANY:
			{
				if( pFocusMover->IsPlayer() )
				{
					CGuild* pGuild	= g_pPlayer->GetGuild();
					if( pGuild )
					{
#ifdef __GUILDCOMBAT
						if( g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE )
							g_WndMng.PutString( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE), NULL, prj.GetTextColor(TID_GAME_GUILDCOMBAT_NOT_USE  ) );	
						else
#endif // __GUILDCOMBAT
						g_DPlay.SendGuildInvite( pFocusMover->GetId() );
					}
				}
				break;
			}
		case MMI_DUEL:
			if( pFocusMover->IsPlayer() )
			{
#ifndef __NPP_050308
				if( !pFocusMover->IsRegionAttr(RA_FIGHT) )	// 상대가 듀얼존에 없으면 에라

				{
					g_WndMng.PutString( prj.GetText(TID_GAME_DUALZONENOEXIST ), NULL, prj.GetTextColor(TID_GAME_DUALZONENOEXIST  ) );
					break;
				}
				if( !g_pPlayer->IsRegionAttr(RA_FIGHT) )	// 내가 듀얼존에 없으면 에라

				{
					g_WndMng.PutString( prj.GetText(TID_GAME_DUALNODUALZONE ), NULL, prj.GetTextColor(TID_GAME_DUALNODUALZONE  ) );
					break;
				}
				if( pFocusMover->GetLevel() < 15 )	// 상대가 레벨이 낮으면 에라.
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_DUALLOWLEVEL ), NULL, prj.GetTextColor(TID_GAME_DUALLOWLEVEL  ) );
					break;
				}
#ifdef __BUGFIX_DUEL050407
				if( g_pPlayer->IsFly() || pFocusMover->IsFly() )
				{
					g_WndMng.PutString( prj.GetTextColor( TID_PK_NOFLIGHT_DURING ), NULL, prj.GetTextColor( TID_PK_NOFLIGHT_DURING ) );
					break;
				}
#endif	// __BUGFIX_DUEL050407
#endif // __NPP_050308
				if( pFocusMover->IsDie() )
				{
					break;
				}
				g_DPlay.SendDuelRequest( g_pPlayer, pFocusMover );		// 상대에게 맞짱뜨자고 메시지 날림.
			}
			break;
		// 극단듀얼.
		case MMI_DUEL_PARTY:
			if( pFocusMover->IsPlayer() )
			{
#ifndef __NPP_050308
				if( pFocusMover->GetLevel() < 15 )
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_DUALLOWLEVEL ), NULL, prj.GetTextColor(TID_GAME_DUALLOWLEVEL  ) ); // 그 사람은 레벨이 낮아서 듀얼신청을 할 수 없습니다.
					break;
				}
#endif // __NPP_050308
				if( pFocusMover->IsDie() )
				{
					break;
				}
				g_DPlay.SendDuelPartyRequest( g_pPlayer, pFocusMover );		// 상대에게 한판뜨자고 메시지 날림.
			}
			break;

		case MMI_TRACE:
			if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			{
				m_objidTracking	= pFocusMover->GetId();
				g_pPlayer->SetDestObj( pFocusMover );
			}
			break;
		case MMI_CHEER:
			if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			{
				if( pFocusMover->IsPlayer() )
					g_DPlay.SendCheering( pFocusMover->GetId() );				
			}
			break;

#ifdef __PIERCING_SM_TIME
		case MMI_PIERCING:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_TEST );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REMOVE_ELEM );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_PIERCING );
				if( pWndBaseBuf )
					break;

				SAFE_DELETE( g_WndMng.m_pWndPiercing );
				g_WndMng.m_pWndPiercing = new CWndPiercing;
				g_WndMng.m_pWndPiercing->Initialize( &g_WndMng, APP_PIERCING );
			}
			break;
#endif //__PIERCING_SM_TIME

		case MMI_UPGRADE:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_TEST );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REMOVE_ELEM );
				if( pWndBaseBuf )
					break;

				SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
				g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
				g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );

			}
			break;

			
		case MMI_CHANGEELEM:
			{
				CWndBase* pWndBaseBuf = g_WndMng.GetWndBase( APP_TRADE );
				
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_REPAIR );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_SHOP_ );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMON_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_CONFIRM_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BANK_PASSWORD );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_DROP_ITEM );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_GUILD_BANK );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_TEST );
				if( pWndBaseBuf )
					break;
				
				pWndBaseBuf = g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
				if( pWndBaseBuf )
					break;

				pWndBaseBuf = g_WndMng.GetWndBase( APP_REMOVE_ELEM );
				if( pWndBaseBuf )
					break;

				SAFE_DELETE( g_WndMng.m_pWndCommerialElem );
				g_WndMng.m_pWndCommerialElem = new CWndCommercialElem;
				g_WndMng.m_pWndCommerialElem->Initialize( &g_WndMng, APP_COMMERCIAL_ELEM );
				
			}
			break;

#ifdef __NPP_050308
		case MMI_INPUT_REWARD: // 현상금 걸기

			{
			#ifdef __ONLINE // 현상범 리스트 요청 패킷
				g_DPlay.SendWantedName();
			#endif// __ONLINE
			}
			break;
		case MMI_SHOW_REWARD:  // 현상범 목록보기

			{
				SAFE_DELETE( g_WndMng.m_pWanted );
			#ifdef __ONLINE // 현상범 리스트 요청 패킷
				g_DPlay.SendWantedList();
			#endif// __ONLINE
			}
			break;
#endif //__NPP_050308			

		} // switch
		// 포커스를 윈도로 돌려주어야 매뉴가 닫힌다.
		SetFocus();
	}
	/*
	CRect rect = GetWindowRect( TRUE );
	
	switch(nID)
	{
		case 100: // wnd1 
			m_wndMenu.Move( CPoint( rect.right, rect.top ) );
			m_wndMenu.SetVisible( !m_wndMenu.IsVisible() ); 
			m_wndMenu.SetFocus();
			break;
	}
	*/
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndWorld::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	CWndNeuz::SetWndRect( rectWnd, bOnSize );

}
void CWndWorld::OnSize( UINT nType, int cx, int cy )
{
//		cx /= 16; cx *= 16;
//	cy /= 16; cy *= 16;
	if( m_nWinSize != WSIZE_MAX )
	{
		CRect rectWnd = GetWndRect();
		CSize size2( rectWnd.Width(), rectWnd.Height() );
		CSize sizeDiv = size2;
		sizeDiv.cx %= 16;
		sizeDiv.cy %= 16;
		size2.cx /= 16; size2.cx *= 16;
		size2.cy /= 16; size2.cy *= 16;
		if( sizeDiv.cx ) size2.cx += 16;
		if( sizeDiv.cy ) size2.cy += 16;
		rectWnd.bottom = rectWnd.top + size2.cy;
		rectWnd.right = rectWnd.left + size2.cx;
		SetWndRect( rectWnd, FALSE );
	}
	AdjustWndBase();
	//}//
//#endif
	m_wndTitleBar.Replace(); 

	CWndBase::OnSize( nType, cx, cy );
	//CWndNeuz::OnSize(nType,cx,cy);
	//Projection( D3DDEVICE );
}
LRESULT CWndWorld::WndMsgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

BOOL CWndWorld::UseFocusObj( CCtrl* pFocusObj )
{
	if( g_pPlayer->m_pActMover->IsFly() )	return FALSE;	// 날고있는중이었으면 취소
	if( g_pPlayer->m_pActMover->IsActJump() )	return FALSE;	// 점프중이면 취소
	if( g_pPlayer->m_pActMover->IsActAttack() )	return FALSE;	// 어택 동작중이면 취소.
	if( g_pPlayer->m_pActMover->IsDie() )		return FALSE;	// 플레이어가 죽은상태면 취소.
	if( IsInvalidObj(pFocusObj) )			return FALSE;	// 타겟이 거시기한 상태면 취소
	if( pFocusObj->GetType() == OT_OBJ )	return FALSE;	// 타겟이 OT_OBJ면 취소
	if( g_pPlayer->m_dwMode & DONMOVE_MODE )	return FALSE;	// 돈무브 모드면 암것도 못함.

//	DEBUG_CHATMSG( "타겟사용", NULL, 0xffffffff );
	
	CWorld* pWorld = g_WorldMng();
	CRect rect = GetClientRect();

	if( pFocusObj->GetType() == OT_ITEM || pFocusObj->GetType() == OT_CTRL )
	{
		if( m_bLButtonDown && !m_bLButtonDowned )
			g_pPlayer->CMD_SetUseItem( (CCtrl*)pFocusObj );	// 목표가 주어지면 자동 이동 
		m_bLButtonDowned = m_bLButtonDown;
	}
	else if( pFocusObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pFocusObj;
		MoverProp* pMoverProp = pMover->GetProp();

		// 여기는 무조건 공격인데.. 단거리 공격은 목표만 세팅해주고 장거리 공격은 실제 공격 코드 삽입 

		CItemElem* pItemElem = g_pPlayer->GetWeaponItem();
		if( pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_WAND ) 
		{
			// 만약 플레이어가 완드류를 들고 있었다면 여기서 처리하지 않고 LBUTTON_UP에서 처리해준다. 
		}
		else
		{
			if( g_pPlayer->m_pActMover->IsSit() )	// 앉아있는 상태였으면 해제
			{
				if( FALSE == g_pPlayer->m_pActMover->IsActJump() &&
					FALSE == g_pPlayer->m_pActMover->IsActAttack() && 
					FALSE == g_pPlayer->m_pActMover->IsActDamage() &&
					FALSE == g_pPlayer->m_pActMover->IsAction() &&
					!( g_pPlayer->m_dwReqFlag & REQ_USESKILL ) )
				{
					g_DPlay.SendMotion( OBJMSG_STANDUP );
				}
			} 
			else
			{
				// 연속공격을 여기서 시작 
#ifdef	__ACROBAT_0504
				if( pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_BOW )
				{
					if( !m_bLButtonDown )
						g_pPlayer->CMD_SetRangeAttack( pFocusObj->GetId(), 0 );		// 원거리공격 패턴에 들어감 
				}
				else
					g_pPlayer->CMD_SetMeleeAttack( pFocusObj->GetId() );		// 근접패턴공격 들어감.
#else	//!__ACROBAT_0504 
				g_pPlayer->CMD_SetMeleeAttack( pFocusObj->GetId() );			// 근접패턴공격 들어감.
#endif	//!__ACROBAT_0504
			}
		}
	}
	else
	{
		return FALSE;	// 그외의 오브젝트타입은 일단 실패로 하자.
	}

	return TRUE;
}

BOOL CWndWorld::UseSkillToFocusObj( CCtrl* pFocusObj )
{
//	if( pFocusObj && IsValidObj( pFocusObj ) && pFocusObj->GetType() != OT_OBJ )
//	if( pFocusObj && pFocusObj->GetType() != OT_OBJ )
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		BOOL bRet = pTaskBar->UseSkillQueue( (CCtrl*)pFocusObj );
//		if( bRet == FALSE )
//			m_dwNextSkill = NULL_ID;		// <<< 이거 왜넣었으!
		return bRet;
	}
	return FALSE;
}
CObj* CWndWorld::PickObj( POINT point, BOOL bOnlyNPC )
{
#ifdef __NEWPICK
	CWorld* pWorld = g_WorldMng();
	CRect rectClient = GetClientRect();
	CObj* pObj;

	// 박스 피킹먼저.
	pObj = pWorld->PickObject_Fast( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, 
									OF_MOVER|OF_ITEM|OF_CTRL,  CMover::GetActiveMover(), TRUE, bOnlyNPC );
	if( pObj )
	{
 		if( pObj->GetType() == OT_MOVER )
		{
			DWORD dwClass = ((CMover *)pObj)->GetProp()->dwClass;
			switch( dwClass )
			{
			case RANK_SUPER:
			case RANK_BOSS:
			case RANK_MATERIAL:
				// 대형몹의 경우엔 정밀피킹을 한번더 한다
				pObj = pWorld->PickObject( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER|OF_ITEM, CMover::GetActiveMover(), NULL, FALSE, bOnlyNPC );
				break;
			}
		}
 	}

#else 
	// old Pick
	CWorld* pWorld = g_WorldMng();
	CRect rectClient = GetClientRect();
	CObj* pObj = pWorld->PickObject( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER|OF_ITEM, CMover::GetActiveMover(), NULL, FALSE, bOnlyNPC );
	if( pObj == NULL ) // 정확하게 오브젝트를 골라내지 못했다면 바운드박스만 다시 채크한다.(이 부분이 없으면 너무 정교하게 오브젝트를 Pick하느라 셀렉팅이 힘들어진다.)
		pObj = pWorld->PickObject_Fast( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER|OF_ITEM,  CMover::GetActiveMover(), TRUE, bOnlyNPC );
	
#endif // not NEWPICK

#ifdef _DEBUG
	if( pObj )
	{
		int a = 0 ;
	}
#endif

	return pObj;
}

CObj* CWndWorld::SelectObj( POINT point )
{
	CWorld* pWorld = g_WorldMng();

	BOOL bOnlyNPC = FALSE;
	if( GetAsyncKeyState( VK_TAB ) & 0x8000 )	// 탭키를 누르고 있으면 NPC만 셀렉트 된다.
		bOnlyNPC = TRUE;
	
	CObj* pObj = PickObj( point, bOnlyNPC );
	if( pObj )
	{
		if( pObj->GetType() == OT_MOVER )
		{
	#ifdef __GUILDCOMBAT
			// 길드컴뱃중에는 죽은케릭을 피킹 금지

			if( pWorld->GetID() == WI_WORLD_GUILDWAR )
			{
				if( ((CMover*)pObj)->IsDie() )
					return NULL;
			}
	#endif //__GUILDCOMBAT
			if( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE )	) // 대상이 투명모드일땐 타겟 안됨.
			{
				return NULL;
			}			
		}
		pWorld->SetObjFocus( pObj );
	}
	return pObj;
}
CObj* CWndWorld::HighlightObj( POINT point )
{
	/*
	CWorld* pWorld = g_WorldMng();
	CRect rectClient = GetClientRect();
	CObj* pObj = pWorld->PickObject_Fast( rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER|OF_ITEM,  CMover::GetActiveMover(), TRUE );
	if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsDie() )
		pObj = NULL;
		*/
	BOOL bOnlyNPC = FALSE;
	if( GetAsyncKeyState( VK_TAB ) & 0x8000 )	// 탭키를 누르고 있으면 NPC만 셀렉트 된다.
		bOnlyNPC = TRUE;
	CObj* pObj = PickObj( point, bOnlyNPC );	
	CObj::m_pObjHighlight = pObj;
	if( pObj && pObj->GetType() == OT_ITEM )
	{
		CRect rect;
		GetBoundRect( pObj, &rect );
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_WndMng.PutToolTip_Item( ((CItem*)pObj)->m_pItemBase, point, &rect );
	}
	return pObj;
}

// 누적된 공격게이지를 구한다. ( 게이지 단계:0, 1, 2, 3, 4)
// 예외:  0  - 모으지 않은 상태, -1 - 3단계 발사후 딜레이 상태
int CWndWorld::GetGaugePower( int* pnValue )
{
	int nTime = GetTickCount() - m_dwPowerTick;
	if( nTime < 0 )
	{
		if( pnValue )
			*pnValue = 0;
		return -1;
	}

	nTime = min( nTime, MAX_POWER_GAUGE );
	if( pnValue )
		*pnValue = nTime;

#ifdef __ACROBAT_0504
	return MulDiv( nTime, 4, MAX_POWER_GAUGE );		
#else
	return MulDiv( nTime, 3, MAX_POWER_GAUGE );
#endif
}

void CWndWorld::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL || g_pPlayer->IsDie() ) 
		return;

	if( m_nMouseMode == 0 && m_bRButtonDown == FALSE )
 		ReleaseCapture();

	CWorld* pWorld = g_WorldMng.Get();
	m_bFlyMove = FALSE;

	// 완드,활 어택 
	CObj* pFocusObj = pWorld->GetObjFocus();
	if( pFocusObj && m_bSelectTarget && m_timerFocusAttack.IsTimeOut() == TRUE )
	{
		if( g_pPlayer->IsAttackAble(pFocusObj) )	// pFocusObj를 공격할수있는지 검사.
		{
			if( pFocusObj->GetType() == OT_MOVER )
			{
				OBJID idTarget = ((CCtrl*)pFocusObj)->GetId();		
	
				int nPower = GetGaugePower( NULL );	
				if( nPower >= 0 )
				{
					m_dwPowerTick = GetTickCount();
				#ifdef __ACROBAT_0504

					ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
					if( pItemProp->dwItemKind3 == IK3_WAND )
						g_pPlayer->CMD_SetMagicAttack( idTarget, nPower );
					else if( pItemProp->dwItemKind3 == IK3_BOW )
						g_pPlayer->CMD_SetRangeAttack( idTarget, nPower );	

				#else 
					g_pPlayer->CMD_SetMagicAttack( idTarget, nPower );
				#endif //__ACROBAT_0504
				}	
			}
		}
	}

	ControlPlayer( WM_LBUTTONUP, point );
	m_bSelectTarget = FALSE;
}


// pTarget에 맞는 메뉴를 띄운다.
void CWndWorld::ShowMoverMenu( CMover* pTarget )
{
	// 메뉴를 띄우기에 적당한 거리인지 확인.
	D3DXVECTOR3 vDist = pTarget->GetPos() - g_pPlayer->GetPos();
	FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
	if( (fDistSq < 20.0f * 20.0f) && m_bRButtonDown == FALSE )
	{
		g_pPlayer->ClearDest();
		m_wndMenuMover.DeleteAllMenu();

	#ifdef __V050322_CACHEQUEST
		// 이걸 안해주면 이미 SetCapture가 된 상태라서 매시지가 매뉴에 안간다.
		// 처음 눌렀을 때 그냥 닫혀버리게 된다.
		ReleaseCapture();
	#endif
		BOOL bView = FALSE;

		if( pTarget->IsPlayer() )
		{
			if( pTarget->m_vtInfo.IsVendorOpen() )
				return;

			m_wndMenuMover.AppendMenu( 0,  MMI_TRADE        , prj.GetText( TID_MMI_TRADE ) );
			m_wndMenuMover.AppendMenu( 0,  MMI_ADD_MESSENGER, prj.GetText( TID_MMI_ADD_MESSENGER ) );
			m_wndMenuMover.AppendMenu( 0,  MMI_INVITE_PARTY , prj.GetText( TID_MMI_INVITE_PARTY ) );

			CGuild* pGuild	= g_pPlayer->GetGuild();
			if( pGuild )
			{
				CGuildMember* pMember	= pGuild->GetMember( g_pPlayer->m_idPlayer );
				if( pMember )
				{
					if( pGuild->m_adwPower[pMember->m_nMemberLv] & PF_INVITATION )
						m_wndMenuMover.AppendMenu( 0, MMI_INVITE_COMPANY, prj.GetText( TID_MMI_INVITE_COMPANY ) );
				}
			}

#ifndef __JAPAN_PVP
			if( g_eLocal.GetState( EVE_18 ) == 1 )
#endif // __JAPAN_PVP
			{
				if( !g_pPlayer->IsBaseJob() )	// 레벨이 15이상일때만 메뉴가 나타남.
				{
				#ifdef __NPP_050308
#ifdef __JAPAN_PVP
					if( g_pPlayer->IsRegionAttr( RA_FIGHT ) && pTarget->IsRegionAttr( RA_FIGHT ) && pTarget->IsDie() == FALSE )
#else // __JAPAN_PVP
					if( g_pPlayer->IsRegionAttr( RA_SAFETY ) == FALSE && pTarget->IsDie() == FALSE )
#endif // __JAPAN_PVP
					{
						m_wndMenuMover.AppendMenu( 0,  MMI_DUEL, prj.GetText( TID_MMI_DUEL ) );
						if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) == TRUE )
							m_wndMenuMover.AppendMenu( 0,  MMI_DUEL_PARTY, prj.GetText( TID_MMI_DUEL_PARTY ) );
					}
				#else // __NPP_050308
					m_wndMenuMover.AppendMenu( 0, MMI_DUEL, prj.GetText( TID_MMI_DUEL ) );
					m_wndMenuMover.AppendMenu( 0, MMI_DUEL_PARTY, prj.GetText( TID_MMI_DUEL_PARTY ) );
				#endif // __NPP_050308
				}
			}

			m_wndMenuMover.AppendMenu( 0, MMI_TRACE, prj.GetText( TID_MMI_TRACE ) );	
			
#ifdef __QUERYEQUIP0630
			m_wndMenuMover.AppendMenu( 0,  MMI_QUERYEQUIP , prj.GetText(TID_MMI_QUERYEQUIP) );
#endif //__QUERYEQUIP0630			
			m_wndMenuMover.AppendMenu( 0,  MMI_CHEER , prj.GetText( TID_MMI_CHEER ) );			
			bView = TRUE;
		}
		else
		{
			LPCHARACTER lpCharacter = pTarget->GetCharacter();
			if( lpCharacter )
			{
				for( int i = 0; i < MAX_MOVER_MENU; i++ )
				{
					if( i == MMI_QUEST )
					{
						// 이 캐릭터에게 퀘스트가 있나?
						// 퀘스트prop를 뒤져서 동일 이름의 prop이 있는지 확인.
						// 확인 되었으면 일단 퀘스트는 존재하는 것.
						// prop에서 해당 캐릭터에 해당하는 퀘스트를 수집한다.
						// 수집이 완료되면, 내기수행한 퀘스트 갯수와 비교한다.
						// 내가 해당 캐릭터에 해당하는 퀘스트보다 적은 갯수이면 더 할 수 있으므로
						// 퀘스트 메뉴 활성화. 
					}
					if( lpCharacter->m_abMoverMenu[ i ] )
					{
						if( i == MMI_GUILDBANKING )
						{
							CGuild* pGuild	= g_pPlayer->GetGuild();
							if( pGuild && g_eLocal.GetState( ENABLE_GUILD_INVENTORY ) )
								m_wndMenuMover.AppendMenu( 0, i, prj.GetText( TID_MMI_DIALOG + i ) );
							bView = TRUE;
						}
						else
						{
							m_wndMenuMover.AppendMenu( 0, i, prj.GetText( TID_MMI_DIALOG + i ) );
							bView = TRUE;
						}
					}
				}
			}
		}

		if( bView )
		{
			CRect rectBound;
			GetBoundRect( pTarget, &rectBound );
			m_wndMenuMover.Move( CPoint( rectBound.right, rectBound.top ) );
			m_wndMenuMover.SetVisible( TRUE );
			m_wndMenuMover.SetFocus();
		} else
		{
			m_wndMenuMover.SetVisible( FALSE );
		}
	}
	else
	{
		if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			g_pPlayer->SetDestObj( pTarget );	// 목표가 주어지면 자동 이동 
	}
}


void CWndWorld::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_objidTracking	= NULL_ID;

	if( g_pPlayer == NULL || g_pPlayer->IsDie() ) 
		return;
	if( g_pPlayer->m_vtInfo.VendorIsVendor() )
		return;
	if( g_pPlayer->m_dwMode & DONMOVE_MODE )	// 돈무브 모드면 암것도 못함.
		return;

#ifdef __SM_ITEM_2ND
	if( GetBuffIconRect( II_SYS_SYS_SCR_RETURN, point ) )	// 귀환의 두루마리 아이콘을 클릭하면 더블클릭까지 검사 
		return;
#endif //__SM_ITEM_2ND
	
	if( g_WndMng.GetWndBase(APP_WEBBOX) 
#ifdef __TAIWAN_WEBBOX
		|| g_WndMng.GetWndBase(APP_WEBBOX2)
#endif	// __TAIWAN_WEBBOX
	)
		return;

	m_timerLButtonDown.Set( 0 );
	// R버튼클릭 200ms이내라면 동시에 눌러진 것으로 판단 -> 스킬 사용 또는 포커싱을 스킵함.
	if( m_timerRButtonDown.GetLeftTime() < 200 )
		return;

	SetCapture();
	m_ptMouseOld = point;
	CWorld* pWorld = g_WorldMng.Get();

	CObj* pSelectOld = pWorld->GetObjFocus();
	CObj* pSelectObj = CObj::m_pObjHighlight;	// 현재 커서 대고 있는 오브젝트.

	if( pSelectObj && pSelectObj->GetType() == OT_MOVER )
	{
		CMover* pSelectMover = (CMover *)pSelectObj;
		if( m_bRButtonDown || m_nMouseMode == 1 )	// 우버튼 누르고 있거나 / 마우스 커서 사라진 모드일땐 무조건 안잡힌다.
			pSelectObj = NULL;
		else
		{
			if( g_pPlayer->m_pActMover->IsFly() )
			{	
				if( pSelectMover->IsNPC() )
				{
					if( pSelectMover->IsPeaceful() == FALSE && pSelectMover->IsFlyingNPC() == FALSE )	
						pSelectObj = NULL;			// 비행못하는 몬스터는 셀렉트 안됨.
				}
				else if( pSelectMover->IsPlayer() )		
					pSelectObj = NULL;			// 플레이어도 셀렉트 안됨.
				else
					pSelectObj = SelectObj( point ); 
			} 
			else
				pSelectObj = SelectObj( point );  
		}
	} 
	else
		pSelectObj = SelectObj( point );


	m_bAutoAttack = FALSE;
//	if( GetAsyncKeyState(VK_CONTROL) & 0x8000 )	// 컨트롤 누르면서 클릭했다.
	if( (nFlags & MK_CONTROL) == MK_CONTROL )	// 컨트롤 누르면서 클릭했다.
		if( pSelectObj && pSelectObj->GetType() == OT_MOVER )
			m_bAutoAttack = TRUE;

	m_dwNextSkill = NEXTSKILL_NONE;	// 왼쪽버튼 누르면 스킬예약 취소.

#ifdef __CLIENT
	// 스킬바 실행중에 왼쪽버튼 클릭했다면 스킬바 실행 취소
#ifdef __XSYNC0711
	CWndTaskBar* pTaskBar = (CWndTaskBar*)g_WndMng.m_pWndTaskBar;
	if( pTaskBar->m_nExecute )
		g_DPlay.SendEndSkillQueue();		// 스킬큐 실행 중단하라고 서버로 보냄.
#else
	CWndTaskBar* pTaskBar = (CWndTaskBar*)g_WndMng.m_pWndTaskBar;
	pTaskBar->OnCancelSkill();					
#endif
#endif // Client

	// 스킬바 사용중에 이동하면 취소된다.
	if( g_pPlayer->m_dwFlag & MVRF_SKILL )		
		g_pPlayer->m_dwFlag &= (~MVRF_SKILL);

	if( pSelectObj == NULL ) 
	{
		m_bFreeMove = TRUE;
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			s_bAccKeyed	= FALSE;
			m_bFlyMove	= TRUE;
		}
	}
	else 
	{
		if( pSelectObj->GetType() == OT_MOVER )
		{

		#ifdef __SCHOOL0701
			if( g_eLocal.GetState( EVE_SCHOOL ) )
			{
				if( ( (CMover*)pSelectObj )->IsPlayer() )
					return;
			}
		#endif // __SCHOOL0701

			CMover* pMover = (CMover*)pSelectObj;

			if( pMover->IsPlayer() &&
				!pMover->IsActiveMover() &&
				pMover->m_vtInfo.IsVendorOpen() &&
				CMover::GetActiveMover()->m_vtInfo.GetOther() == NULL &&
				pMover->m_fDistCamera < 20.0f
				)
			{
#ifdef __NPP_050308
				if( pMover->IsRegionAttr( RA_SAFETY ) )
				{
					g_DPlay.SendQueryPVendorItem( pMover->GetId() );
					
				}
				else if( ( GetAsyncKeyState(VK_CONTROL) & 0x8000 ) == FALSE )
				{
					g_DPlay.SendQueryPVendorItem( pMover->GetId() );
				}
#else // __NPP_050308
				g_DPlay.SendQueryPVendorItem( pMover->GetId() );
#endif // __NPP_050308
			}
			else if( pMover->IsPlayer() && FALSE == pMover->IsActiveMover() && g_bKeyTable['Z'] )
			{
				if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// 비행중이 아닐때만 따라가기 된다.
				{			
				#if defined(__YGUIDESYSTEM0923) && defined(__ONLINE)
					if( m_pWndGuideSystem )
						m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_TRACKING_MOVE);
				#endif //__YGUIDESYSTEM0923
					
					m_objidTracking	= pMover->GetId();
					g_pPlayer->SetDestObj( pMover );
				} 
			}	
			else
			{
#ifdef __XPET
//				if( pMover->GetId() == g_pPlayer->m_idPet )		// 자기펫을 클릭했을때.
					PLAYSND( pMover->GetProp()->dwSndIdle1 );
#endif
				// 메뉴를 띄우기 
				if( pMover->IsPeaceful() && !g_pPlayer->IsFly() &&
				   ( pMover->IsPlayer() == FALSE || ( pMover->IsPlayer() && (GetAsyncKeyState(VK_MENU) & 0x8000) ) ) ) 
				{
					ShowMoverMenu( pMover );
				} 
			}

			m_bSelectTarget = TRUE;
			if( pSelectObj != pSelectOld )
			{
				m_timerFocusAttack.Reset();
				//static int a; a++;
				//TRACE( "fuck %d \n", a );
			}
			m_dwPowerTick = ::GetTickCount();
		}
		else if( pSelectObj->GetType() == OT_ITEM )
		{
			m_bSelectTarget = TRUE;
		}
		else if( pSelectObj->GetType() == OT_CTRL )
		{
			m_bSelectTarget = TRUE;
		}
	}

	ControlPlayer( WM_LBUTTONDOWN, point );
}

void CWndWorld::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( m_nMouseMode == 0 )	// 모드1일땐 릴리즈 해선 안된다.
		ReleaseCapture();
	ControlPlayer( WM_RBUTTONUP, point );
	CMover* pMover = CMover::GetActiveMover();
//	pMover->SendActMsg( OBJMSG_ACC_STOP );	// 가속중지

#ifndef __VRCAMERA
	if( g_pPlayer && g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock() )
		g_Neuz.m_camera.Unlock();
#endif
#ifdef __VRCAMERA
	if( m_bCameraMode == TRUE )
	{
		if( g_pPlayer && g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock() )
			g_Neuz.m_camera.Unlock();
		m_bCameraMode = FALSE;
	}
#endif
}
void CWndWorld::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( g_Option.m_nInterface == 0 )	// 구버전일때만
		m_objidTracking		= NULL_ID;	// 오른버튼 누르면 따라가기 해제된다.
	if( IsFullMax() && m_wndTitleBar.GetWindowRect( TRUE ).PtInRect( point ) )
		return;
	if( g_pPlayer == NULL ) 
		return;
	if( CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
		return;
	if( g_WndMng.GetWndBase(APP_WEBBOX) 
#ifdef __TAIWAN_WEBBOX
		|| g_WndMng.GetWndBase(APP_WEBBOX2)
#endif	// __TAIWAN_WEBBOX
	)
		return;
	

#ifdef __VRCAMERA
	if( CObj::m_pObjHighlight == NULL )
	{
		SetCapture();
		m_ptMouseOld = point;
		m_bCameraMode = TRUE;
	}
#endif
//	m_bAutoAttack = FALSE;
	m_ptMouseSpot = point;
	SetCapture();
	m_timerRButtonDown.Set( 0 );;
	// 좌측 버튼이 눌러진지 1000분의 200초 내라면 동시에 눌러진 것으로 판단, 동시 눌러진 것은 점프, 
	// 따라서 스킬 사용 또는 포커싱을 스킵함.
	if( m_timerLButtonDown.GetLeftTime() < 200 )
		return;
//	CRect rect = GetClientRect();
	if( g_Option.m_nInterface == 0 )	// 구버전인터페이스 방식에서만 우클릭이 스킬.
	{
		CWorld* pWorld = g_WorldMng();
		CObj* pTargetObj = CObj::m_pObjHighlight;		// 커서를 대고 있던 오브젝트가 하이라이트 오브젝이다.
		if( g_pPlayer->m_pActMover->IsFly() == FALSE )
		{
			if( pTargetObj )
			{
				pWorld->SetObjFocus( pTargetObj );

				CMover* pMover = (CMover*)pTargetObj;
				if( pMover->GetType() == OT_MOVER )
					m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
			} else
				m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
		}
	}

}	

//
//
void CWndWorld::OnMButtonUp(UINT nFlags, CPoint point)
{
	if( m_nMouseMode == 0 )
		ReleaseCapture();
#ifndef __VRCAMERA
	if( g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock() )
		g_Neuz.m_camera.Unlock();
#endif
}
void CWndWorld::OnMButtonDown(UINT nFlags, CPoint point)
{
	if( IsFullMax() && m_wndTitleBar.GetWindowRect( TRUE ).PtInRect( point ) )
		return;
#ifndef __VRCAMERA
	SetCapture();
	m_ptMouseOld = point;
#endif
}

#ifdef __SM_ITEM_2ND
BOOL CWndWorld::GetBuffIconRect( DWORD dwID, const CPoint& point )
{
	vector <BUFFICONRECT_INFO>::iterator it = m_rcCheck.begin();
	for( ; it != m_rcCheck.end(); ++it )
	{
		BUFFICONRECT_INFO &info = *it;
		if( PtInRect( &info.rc, point ) == TRUE && info.dwID == dwID )
		{
			return TRUE;
		}
	}

	return FALSE;
}
#endif //__SM_ITEM_2ND

void CWndWorld::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	m_objidTracking		= NULL_ID;
	ControlPlayer( WM_LBUTTONDBLCLK, point );
	CRect rect = GetClientRect();
	CWorld* pWorld = g_WorldMng.Get();
	CObj* pFocusObj	= g_WorldMng.Get()->GetObjFocus();
	if( pFocusObj && CObj::m_pObjHighlight == pFocusObj )
		m_bAutoAttack = TRUE;

#ifdef __SM_ITEM_2ND
	if( GetBuffIconRect( II_SYS_SYS_SCR_RETURN, point ) )
		g_DPlay.SendReturnScroll( -1 );
#endif //__SM_ITEM_2ND
}

void CWndWorld::OnMButtonDblClk( UINT nFlags, CPoint point)
{
	if( g_Option.m_nInterface == 0 )
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			if( g_Neuz.m_camera.IsLock() ) 
				g_Neuz.m_camera.Unlock();
			else 
				g_Neuz.m_camera.Lock();
		}
		else
		{
			g_Neuz.m_camera.m_fRotx=-g_pPlayer->GetAngle();
		}
	}
}
void CWndWorld::OnRButtonDblClk( UINT nFlags, CPoint point)
{
	CMover* pMover = CMover::GetActiveMover();
//	CRect rect = GetClientRect();
	CWorld* pWorld = g_WorldMng.Get();
	// 포커스 타겟 해제
	CObj* pTargetObj = CObj::m_pObjHighlight;

	if( g_pPlayer->m_pActMover->IsFly() )
	{
		m_nMouseMode ^= 1;		// 비행중 마우스 이동모드 토글.
		if( m_nMouseMode == 0 )
		{
			ReleaseCapture();
			ClipCursor( NULL ); // 윈도우를 rcWindow를 벗어나지 않게 한다.
		}
		else
		{
			SetCapture();
			RECT rcWindow;
			::GetWindowRect( g_Neuz.GetSafeHwnd(), &rcWindow );
			ClipCursor( &rcWindow ); // 윈도우를 rcWindow를 벗어나지 않게 한다.
		}
	}
	else
	{
		
		if( g_Option.m_nInterface == 1 )	
		{
			g_Neuz.m_camera.m_fRotx = -g_pPlayer->GetAngle();
		}
	}
	
}
BOOL CWndWorld::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( CWorld::m_nZoomLevel != 0 )
		return FALSE;

	// zoom
	if(zDelta < 0)
	{
		g_Neuz.m_camera.m_fZoom --;

#ifdef __YGUIDESYSTEM0923
		if( g_Neuz.m_camera.m_fZoom > 2 )
			m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_KEY_ZOOM);
#endif //__YGUIDESYSTEM0923
		
		if(g_Neuz.m_camera.m_fZoom < 0)
			g_Neuz.m_camera.m_fZoom = 0;
	}
	else
	{
		g_Neuz.m_camera.m_fZoom ++;
#if defined(__XUZHU) && defined(_DEBUG)
		if(g_Neuz.m_camera.m_fZoom > 15)
			g_Neuz.m_camera.m_fZoom = 15;
#else

#ifdef __YGUIDESYSTEM0923
		if( g_Neuz.m_camera.m_fZoom > 5 )
			m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_KEY_ZOOM);
#endif //__YGUIDESYSTEM0923

		if(g_Neuz.m_camera.m_fZoom > 7)
			g_Neuz.m_camera.m_fZoom = 7;
#endif
	}
	
#ifdef __YCAMERA_EX	
	g_Neuz.m_camera.m_fBackupZoom = g_Neuz.m_camera.m_fZoom;
#endif //__YCAMERA_EX		

	return TRUE;
}
void CWndWorld::OnMouseMove(UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL )
		return;

#ifdef __VRCAMERA
	if( m_bRButtonDown ) //&& m_bCameraMode )
	{
		g_Neuz.m_camera.m_fRotx += point.x - m_ptMouseOld.x;
		if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
		if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;

		//if(g_Neuz.m_camera.m_fRotx>180) g_Neuz.m_camera.m_fRotx=-180;
		//if(g_Neuz.m_camera.m_fRotx<-180) g_Neuz.m_camera.m_fRotx=180;

		g_Neuz.m_camera.m_fRoty += point.y - m_ptMouseOld.y;
		if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
		if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
	}
	else
	if( m_bLButtonDown )
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
			CRect rect= GetClientRect();
			// rect.Height() : 100 = point.y : x
			int a = 100 * point.y / rect.Height();
			g_Neuz.m_camera.m_fRoty = a - 60;
			TRACE( "%f \n",g_Neuz.m_camera.m_fRoty );

		}
		else
		{

		}
	}	

#else // VRCAMERA
	CPoint pt2 = point;
	// m_nMouseMove==0 이면 우버튼 누르고 있어야 하고

	// m_nMouseMove==1 이면 안누르고 있으도 움직인다.
	// 신/구 버전에 관계 없다.
	if( (m_nMouseMode == 0 && m_bRButtonDown) || (m_nMouseMode == 1) )
	{
		SetCapture();
		CMover *pPlayer = CMover::GetActiveMover();
		if( pPlayer->m_pActMover->IsFly() /*&& m_bMButtonDown == FALSE*/ )
		{
			int nWidth = (int)g_Neuz.GetCreationWidth();
			int nHeight = (int)g_Neuz.GetCreationHeight();
			int nLeft = 0, nTop = 0;
			if( g_Neuz.IsWindow() )	// 윈도우 모드로 실행중이면.
			{
				RECT rrr;
				::GetWindowRect( g_Neuz.GetSafeHwnd(), &rrr ); 
				nLeft = rrr.left;		nTop = rrr.top;

			}
#ifndef __XUZHU			
			g_Neuz.m_pd3dDevice->ShowCursor( FALSE );
#endif
			ControlFlying( WM_MOUSEMOVE, point );
			if( (int)point.x > nWidth - 10 || point.x < 10 )
			{
				g_Neuz.m_pd3dDevice->SetCursorPosition( nLeft + nWidth / 2, nTop + point.y, 0 );
				point.x = nWidth / 2;
			}
			if( point.y < 10 || point.y > nHeight - 10 )
			{
				g_Neuz.m_pd3dDevice->SetCursorPosition( nLeft + point.x, nTop + nHeight / 2, 0 );
				point.y = nHeight / 2;
			}
		}
		else
			g_Neuz.m_pd3dDevice->ShowCursor( TRUE );
	
	}
	else
		g_Neuz.m_pd3dDevice->ShowCursor( TRUE );

//	if( pt2.x > 800 )
//		g_Neuz.m_pd3dDevice->ShowCursor( TRUE );
	
	
	
	
	BOOL bCamera = FALSE;
	// 휠버튼드래그는 카메라만 회전.
	if( g_pPlayer->m_pActMover->IsFly() )
	{
		if( m_bMButtonDown )	// 비행중엔 무조건 휠이 카메라회전이다.
			bCamera = TRUE;
	} else
	{	// 지상에서만.
		if( g_Option.m_nInterface == 0 )	// 구 버전.
		{
			if( m_bMButtonDown )		// 휠로 카메라를 돌림.
				bCamera = TRUE;
		} else
		{								// 신버전.
			if( m_bRButtonDown )		// 우클릭으로 카메라를 돌림.
				bCamera = TRUE;
		}
	}
	if( bCamera )
	{
#ifdef __YGUIDESYSTEM0923
		if( m_pWndGuideSystem )
			m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_CAMERAMOVE);
#endif //__YGUIDESYSTEM0923
		FLOAT fRotSpeed = 1.0f;

	#ifdef __ACROBAT_0504
		switch( g_Option.m_MouseSpeed )
		{
			case 0:
				fRotSpeed = 1.0f; break;
			case 1:
				fRotSpeed = 0.2f; break;
			case 2:
				fRotSpeed = 0.1f; break;
			default:
				fRotSpeed = 0.2f; break;
		}
	#endif //__ACROBAT_0504
		
		g_Neuz.m_camera.m_fRotx += ((point.x - m_ptMouseOld.x) * fRotSpeed );
		if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
		if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;
		
		//if(g_Neuz.m_camera.m_fRotx>180) g_Neuz.m_camera.m_fRotx=-180;
		//if(g_Neuz.m_camera.m_fRotx<-180) g_Neuz.m_camera.m_fRotx=180;
		
		g_Neuz.m_camera.m_fRoty += ((point.y - m_ptMouseOld.y) * fRotSpeed );
		if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
		if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
	}
	else
	if( m_bMButtonDown || m_bLButtonDown )
	{
		if( g_pPlayer->m_pActMover->IsFly() )
		{
		}
		else
		{
		}
	}	
#endif // not VRCAMERA
	m_ptMouseOld = point;
}

void CWndWorld::OnMouseWndSurface( CPoint point )
{
	
}


void CWndWorld::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef __CLIENT
	CMover* pMover = CMover::GetActiveMover();
	
	if( nChar == VK_SCROLL )
	{
		CWorld::m_nZoomLevel++;

		if( CWorld::m_nZoomLevel > 3 )
		{
			CWorld::m_nZoomLevel = 0;
		}

		if( CWorld::m_nZoomLevel != 0 )
		{
			g_Neuz.m_camera.m_fZoom = 0.0f;
			char buff[16];
			sprintf( buff, "%dX Zoom", CWorld::m_nZoomLevel );
			g_WndMng.PutString( buff, NULL, 0xff00ff00 );
			
		}
	}
#ifdef __GUILDCOMBAT
	else
	if( nChar == VK_TAB )
	{
		CWorld* pWorld = g_WorldMng.Get();
		CMover* pMover = NULL;

		if( n_nMoverSelectCount > MAX_MOVERSELECT )
			n_nMoverSelectCount = 0;

		for( int i=0; i<MAX_MOVERSELECT; i++ )
		{
			if( n_nMoverSelectCount == i )
				continue;

			pMover = CWorld::m_amvrSelect[i];

			if( pMover )
				break;
		}

		if( pMover )
		{
			if( !pMover->IsMode( TRANSPARENT_MODE )	&& !pMover->IsDie() ) // 대상이 투명모드일땐 타겟 안됨.
				pWorld->SetObjFocus( pMover );
		}
		
		n_nMoverSelectCount = i;
	}
/*
	else
	if( nChar == 'F' || nChar == 'f' )
	{
		g_DPlay.SendFocusObj();
	}
*/
#endif //__GUILDCOMBAT

#endif //__CLIENT	
	
	
	//g_bKeyTable[ nChar ] = FALSE;
}
void CWndWorld::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWorld* pWorld = g_WorldMng.Get();
	
	if( g_Option.m_nInterface == 1 )		// 신버전 인터페이스 방식은 X 가 스킬사용이다.
	{
//		if( nChar == VK_SHIFT || nChar == 'C' )	
		if( nChar == 'C' )	
		{
//			CObj* pTargetObj = CObj::m_pObjHighlight;		// 커서를 대고 있던 오브젝트가 하이라이트 오브젝이다.
			CObj* pTargetObj = pWorld->GetObjFocus();		// 선택된 오브젝트.
			if( g_pPlayer->m_pActMover->IsFly() == FALSE )
			{
				if( pTargetObj )	// 커서를 대고 있던 오브젝트가 있으면

				{
#ifndef __XSKILL0425
					pWorld->SetObjFocus( pTargetObj );	// 그놈을 셀렉트 하는 동시에.
#endif // not xSkill0425
					
					CMover* pMover = (CMover*)pTargetObj;
					if( pMover->GetType() == OT_MOVER )
						m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
				} else
					m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
			}
		}

#ifdef __S0810_NOTMONNAME
//		if( nChar == VK_OEM_3 )
		if( nChar == 192 )
		{
			if( g_Option.m_nMonName )
			{
				g_Option.m_nMonName = 0;
			}
			else
			{
				g_Option.m_nMonName = 1;
			}
		}
#endif // __S0810_NOTMONNAME
	}
}
BOOL CWndWorld::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if( pWndFrame == NULL )
	{
		LPCTSTR szErr = Error( "CWndWorld::OnDropIcon : pWndFrame==NULL" );
		ADDERRORMSG( szErr );
	}

	if( g_WndMng.GetWndBase( APP_TEST ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITITEM), NULL, prj.GetTextColor(TID_GAME_TRADELIMITITEM) );
		SetForbid( TRUE );
		return FALSE;
	}		
	
	if( g_WndMng.GetWndBase( APP_SHOP_ ) ||
		g_WndMng.GetWndBase( APP_GUILD_MERIT ) ||
		g_WndMng.GetWndBase( APP_GUILD_BANK ) ||
		g_WndMng.GetWndBase( APP_COMMON_BANK ) ||
		g_WndMng.GetWndBase( APP_BEAUTY_SHOP ) ||
		g_WndMng.GetWndBase( APP_TRADE ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITITEM), NULL, prj.GetTextColor(TID_GAME_TRADELIMITITEM) );
			//g_WndMng.PutString( "거래중에 아이템을 버릴수 없어요.", NULL, 0xffff0000 );
			SetForbid( TRUE );
			return FALSE;
		}
	}
	else
	if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTDROP), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOTDROP) );
			//g_WndMng.PutString( "수리중에 아이템을 버릴수 없어요.", NULL, 0xffff0000 );
			SetForbid( TRUE );
			return FALSE;
		}
	}

	
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM ) //&& pWndFrame->GetWndId() == APP_INVENTORY )
	{
		CRect rect = GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		D3DXVECTOR3 vPos, vOut;
		pWorld->ClientPointToVector( NULL, rect, point, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &vPos, FALSE );
		vOut = vPos - g_pPlayer->GetPos();	// 플레이어에서 찍은곳을 향하는 벡터
		D3DXVec3Normalize( &vPos, &vOut );	// 그 것을 노말라이즈.
		vPos += g_pPlayer->GetPos();			// 절대좌표로 변환.
		vPos.y += 1000.0f;		// 최상위 높이를 검사하기 위해.
		FLOAT fHeight	= pWorld->GetFullHeight( vPos );	// 절대좌표의 y높이 구함.
		if( fabs(fHeight - g_pPlayer->GetPos().y) > 1.0f )	// 아이템놓을곳과 플레이어의 좌표가 1미터 이상차이가 나면

			vPos = g_pPlayer->GetPos();		// 걍 제자리에 놓음.
		else
			vPos.y	= fHeight + 0.2f;	// 경사면에선 땅에 파묻히는 현상이 약간 있어서 좀 올려 놓이게 했다.
		
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			PLAYSND( SND_INF_GROUNDDROP );
			if( pShortcut->m_dwData == 0 )
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				g_WndMng.m_pWndTradeGold = new CWndTradeGold;
				g_WndMng.m_pWndTradeGold->m_dwGold = 0;
				g_WndMng.m_pWndTradeGold->m_vPos = vPos;
				memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT) );
				g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
				g_WndMng.m_pWndTradeGold->MoveParentCenter();

				if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
				{
					CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC ));
					pWndStatic->SetTitle( prj.GetText(TID_GAME_DROPPENYAREMOVE) );
				}

				return TRUE;
			}
			else
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
				if( pItemBase == NULL )
					return FALSE;

				if( pItemBase->GetProp()->dwItemKind3 == IK3_CLOAK && ( (CItemElem*)pItemBase )->m_idGuild != 0 )
					return FALSE;
				
				if( ((CItemElem*)pItemBase)->IsCharged() )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTDROP ), NULL, prj.GetTextColor( TID_GAME_NOTDROP ) );
					return FALSE;
				}

				if( IsUsingItem( pItemBase ) )
				{
					SetForbid( TRUE );
					return FALSE;
				}

#ifdef __ONLINE	
				CItemElem* pItemElem = (CItemElem*)pShortcut->m_dwData;
				
#ifdef __S050504_QUESTITEM
				if( pItemElem->IsQuest() )
				{
					g_WndMng.m_pWndQuestItemWarning = new CWndQuestItemWarning;
					g_WndMng.m_pWndQuestItemWarning->m_pItemElem = pItemElem;
					g_WndMng.m_pWndQuestItemWarning->m_vPos = vPos;
					g_WndMng.m_pWndQuestItemWarning->Initialize( NULL, APP_QUEITMWARNING );
				}
				else
				{
#endif // __S050504_QUESTITEM
					if( pItemElem->m_nItemNum == 1 )	// 갯수가 하나라면 그냥 떨군다.
					{
						g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
						g_WndMng.m_pWndDropConfirm->m_pItemElem = pItemElem;
						g_WndMng.m_pWndDropConfirm->m_vPos = vPos;
						g_WndMng.m_pWndDropConfirm->Initialize( NULL, APP_DROP_CONFIRM );

						if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
						{
							CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndDropConfirm->GetDlgItem( WIDC_STATIC1 ));
							pWndStatic->SetTitle( prj.GetText(TID_GAME_DROPITEMREMOVE) );
						}
					}
					else
					{
						//SAFE_DELETE( g_WndMng.m_pWndDropItem );
						g_WndMng.m_pWndDropItem = new CWndDropItem;
						g_WndMng.m_pWndDropItem->m_pItemElem = pItemElem;
						g_WndMng.m_pWndDropItem->m_vPos = vPos;
						g_WndMng.m_pWndDropItem->Initialize( NULL, APP_DROP_ITEM );

						if( g_eLocal.GetState( EVE_DROPITEMREMOVE ) )
						{
							CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndDropItem->GetDlgItem( WIDC_STATIC1 ));
							pWndStatic->SetTitle( prj.GetText(TID_GAME_DROPITEMREMOVE2) );
						}
					}
#ifdef __S050504_QUESTITEM
				}
#endif // __S050504_QUESTITEM
#endif	// __ONLINE	
				
		#ifdef __ONLINE
//				g_DPlay.SendDropItem( pShortcut->m_dwType, pShortcut->m_dwId, 0, vPos );
		#else // __ONLINE
				CItem* pItem = new CItem;
				pItem->m_pItemBase = new CItemElem;
				*( (CItemElem*)pItem->m_pItemBase )		= *( (CItemElem*)pItemBase );
				pItem->SetIndex( g_Neuz.m_pd3dDevice, pItemBase->m_dwItemId );
				vPos.y += ( pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y ) / 2.0f;
				pItem->SetPos( vPos );
				pItem->SetAngle( xRandom( 360 ) );
				pWorld->AddObj( pItem, TRUE );
				g_pPlayer->DoFakeEquip( pItemBase->m_dwItemId, 0, FALSE, 0, g_pBipedMesh );
				g_pPlayer->RemoveItemId( pShortcut->m_dwType, pShortcut->m_dwId );

		#endif // __ONLINE
				return TRUE;
			}
		}
	}
	SetForbid( TRUE );
	return FALSE;
}

BOOL CWndWorld::Process()
{
	if( g_pPlayer )
	{
		// 퀘스트 제한 시간 체크 및 시간 출력 세팅 
#ifdef __V050322_CACHEQUEST
		CMover::ProcessQuestEmoticon();
#endif

		if( m_dwIdBgmMusic )
		{
			if( IsStopMusic() == TRUE )
			{
				PlayBGM( m_dwIdBgmMusic );
				if( m_bBGM == FALSE )
					m_dwIdBgmMusic = 0;
			}
		}
		else
		{
			m_dwIdBgmMusic = g_WorldMng()->m_dwIdMusic;
		}

		CWorld* pWorld = g_WorldMng();
		// 캐릭터가 움직이면 대화, 상인, 거래 창을 무조건 닫는다.
		if( g_pPlayer->m_pActMover->IsState( OBJSTA_STAND ) == FALSE )
		{
#ifdef __YGUIDESYSTEM0923
			if( m_pWndGuideSystem && m_pWndGuideSystem->m_bIsGuideChart[0] == FALSE )
				m_pWndGuideSystem->m_wndGuideText->m_bVisible = FALSE;
#endif //__YGUIDESYSTEM0923
			
			CWndBase* pWndBase = g_WndMng.GetWndBase( APP_DIALOG );			
			if( pWndBase ) 
				pWndBase->Destroy();

#ifdef __ONLINE
			// raiders_test
			if( g_pPlayer->m_vtInfo.GetOther() )
			{
				if( g_pPlayer->m_vtInfo.GetOther()->IsPlayer() )	
					g_DPlay.SendTradeCancel();

				g_DPlay.SendCloseShopWnd();
				g_pPlayer->m_vtInfo.SetOther( NULL );
			}
#else
			pWndBase	= g_WndMng.GetWndBase( APP_TRADE );
			if( pWndBase )
				( (CWndTrade*)pWndBase )->DoCancel();
#endif
			
#ifdef __YENCHANT_0608
			pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
			if( pWndBase )
			{
				((CWndInventory*)pWndBase)->BaseMouseCursor();
			}	
#endif //__YENCHANT_0608

			pWndBase	= g_WndMng.GetWndBase( APP_VENDOREX );				
			if( pWndBase )
			{
				pWndBase->Destroy();
				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase )
					pWndBase->Destroy();
			}

			pWndBase	= g_WndMng.GetWndBase( APP_SHOP_ );
			if( pWndBase )
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_TRADE_GOLD );
				if( pWndBase )	
					pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase ) 
					pWndBase->Destroy();
			}

			pWndBase	= g_WndMng.GetWndBase( APP_BEAUTY_SHOP );
			if( pWndBase )
			{
				pWndBase->Destroy();

				pWndBase	= g_WndMng.GetWndBase( APP_INVENTORY );

				if( pWndBase )
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_COMMON_BANK );
			if( pWndBase )
			{
				g_DPlay.SendCloseBankWnd();
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_TRADE_GOLD );
				if( pWndBase )	
					pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase )	
					pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_REPAIR );
			if( pWndBase )
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
				if( pWndBase )	pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_BANK_PASSWORD );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CONFIRM_BANK );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_DROP_CONFIRM );
			if( pWndBase )
				pWndBase->Destroy();

			pWndBase	= g_WndMng.GetWndBase( APP_GUILD_BANK );
			if( pWndBase )
			{
				g_DPlay.SendCloseGuildBankWnd();
				pWndBase->Destroy();
			}
			pWndBase	= g_WndMng.GetWndBase( APP_RANK_WAR );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RANK_INFO );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_RANK_GUILD );
			if( pWndBase )
				pWndBase->Destroy();

#ifdef __PIERCING_SM_TIME
			pWndBase	= g_WndMng.GetWndBase( APP_PIERCING );
			if( pWndBase )
				pWndBase->Destroy();
#endif //__PIERCING_SM_TIME			
		
			pWndBase	= g_WndMng.GetWndBase( APP_TEST );
			if( pWndBase )
				pWndBase->Destroy();
			
			pWndBase	= g_WndMng.GetWndBase( APP_COMMERCIAL_ELEM );
			if( pWndBase )
				pWndBase->Destroy();
			
			pWndBase	= g_WndMng.GetWndBase( APP_REMOVE_ELEM );
			if( pWndBase )
				pWndBase->Destroy();

#ifdef __S050524_SM_ITEM
			pWndBase	= g_WndMng.GetWndBase( APP_CHANGECLASS_1 );
			if( pWndBase )
				pWndBase->Destroy();
			pWndBase	= g_WndMng.GetWndBase( APP_CHANGECLASS_2 );
			if( pWndBase )
				pWndBase->Destroy();
#endif // __S050524_SM_ITEM
			
#ifdef __QUERYEQUIP0630
			pWndBase	= g_WndMng.GetWndBase( APP_QUERYEQUIP );
			if( pWndBase )
				pWndBase->Destroy();
#endif //__QUERYEQUIP0630

#ifdef __Y_POST_0912
			pWndBase	= g_WndMng.GetWndBase( APP_POST );
			if( pWndBase )
				pWndBase->Destroy();
#endif //__Y_POST_0912

#ifdef __GUILDCOMBAT
			pWndBase	= g_WndMng.GetWndBase( APP_GUILDCOMBAT_RANKING );
			if( pWndBase )
				pWndBase->Destroy();

			pWndBase	= g_WndMng.GetWndBase( APP_GUILD_WAR_STATE );
			if( pWndBase )
				pWndBase->Destroy();

			pWndBase	= g_WndMng.GetWndBase( APP_CLOSE_EXISTING_CONNECTION );
			if( pWndBase )
				pWndBase->Destroy();		
#endif //__GUILDCOMBAT
			

		}
		g_SoundMng.SetListener( g_pPlayer->GetPos(), g_Neuz.m_camera.m_fCurRotx );
		g_WorldMng()->SetCamera( &g_Neuz.m_camera );
		g_WorldMng()->Process( 0 ); // 월드를 처리함 (오브젝트, 기타 등등 )

		// 지상에서 상대를 공격하거나 해당 오브젝트 사용하기.
		if( g_pPlayer->m_pActMover && g_pPlayer->m_pActMover->IsFly() == FALSE )
		{
			CObj* pObj = pWorld->GetObjFocus();
			if( m_dwNextSkill == NEXTSKILL_ACTIONSLOT )	// 걍 1이면 액션슬롯 사용 
			{
				UseSkillToFocusObj( (CCtrl*)pObj );		// 스킬을 사용함.
			} else
			if( m_dwNextSkill != NEXTSKILL_NONE )		// 그외는 예약된 스킬을 실행.
			{
				g_WndMng.UseSkillShortCut( 0, m_dwNextSkill );
			} else
			{	// 일반 공격.
				if( m_bLButtonDown || m_bAutoAttack )	// 누른 순간에만 처리하기 위해서...
				{
					if( m_bAutoAttack || (m_bSelectTarget && m_timerFocusAttack.IsTimeOut() == TRUE ) )
					{
						if( pObj )
						{
							// 컨트롤/무버/아이템 일 경우만...
							if( pObj->GetType() == OT_CTRL || pObj->GetType() == OT_MOVER || pObj->GetType() == OT_ITEM )
							{	// 걍 일반 공격/사용 들어감.  스킬응답중일땐 실행하지 말자.
								if( g_pPlayer->IsAttackAble(pObj) && (g_pPlayer->m_dwReqFlag & REQ_USESKILL) == 0 )	
								{					
									UseFocusObj( (CCtrl*) pObj );	// 대상을 Use한다.
								}
							}
						} else
							ControlPlayer( 0, GetMousePoint() );
					}
				} 
				else
				{
					// 플레이어 마우스 및 키보드 조작 처리 
					ControlPlayer( 0, GetMousePoint() );
					m_bSelectTarget = FALSE;
				}
			}
		} else
		{
			CObj* pObj = pWorld->GetObjFocus();
			if( m_bLButtonDown && m_bSelectTarget )
			{
				if( pObj )
				{
					if( pObj->GetType() == OT_ITEM || pObj->GetType() == OT_CTRL )
					{
						if( m_bLButtonDown && !m_bLButtonDowned )
							g_pPlayer->CMD_SetUseItem( (CCtrl*)pObj );	// 목표가 주어지면 자동 이동 
						m_bLButtonDowned = m_bLButtonDown;
					}
				}
			} else
			{
				// 플레이어 마우스 및 키보드 조작 처리 
				ControlPlayer( 0, GetMousePoint() );
				m_bSelectTarget = FALSE;
			}
		}
		m_bLButtonDowned = m_bLButtonDown;
		int nSize = pWorld->m_aRegion.GetSize();
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		for( int i = 0; i < nSize; i++ )
		{
			LPREGIONELEM lpRegionElem = pWorld->m_aRegion.GetAt( i );
			if( lpRegionElem->m_rect.PtInRect( CPoint( vPos.x, vPos.z ) ) )
			{
				if( lpRegionElem->m_bInside == FALSE )
				{
					//if( lpRegionElem->m_bDirectMusic )
					//	lpRegionElem->m_bPlayDirectMusic = TRUE;
					m_dwIdBgmMusic = lpRegionElem->m_dwIdMusic;
					if( lpRegionElem->m_bDirectMusic == TRUE )
						m_bBGM = TRUE;
					else
						m_bBGM = FALSE;
					if( IsStopMusic()  ) //|| lpRegionElem->m_bDirectMusic ) ) //&& !lpRegionElem->m_bPlayDirectMusic )
					{
						PlayMusic( lpRegionElem->m_dwIdMusic );
						if( m_bBGM == FALSE )
							m_dwIdBgmMusic = 0;
					}
					lpRegionElem->m_bInside = TRUE;

					//if( lpRegionElem->m_szDesc[ 0 ] )
					{
						BOOL bResult = TRUE;
						if( lpRegionElem->m_dwAttribute & RA_INN )
						{
							if( (int)g_pPlayer->GetPos().y != (int)g_WorldMng()->GetLandHeight( g_pPlayer->GetPos() ) )
								bResult = FALSE;
						}
						if( bResult )
						{
							CHAR szDesc[ MAX_REGIONDESC ];
							CHAR* pDescSrc = lpRegionElem->m_szDesc;
							CHAR* pDescDsc = szDesc;
							int nNumCaption = 0;
							while( 1 )
							{
								if( ( *pDescSrc == '\r' && *(pDescSrc + 1 ) == '\n' ) || *pDescSrc == 0 )
								{
									*pDescDsc = 0;
									if( szDesc[ 0 ] )
										g_ClientMsg.AddMessage( szDesc, 0xffffffff );//.AddCaption( szDesc, m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
									nNumCaption++;
									pDescDsc = szDesc;
									pDescSrc+=2;
									if( *pDescSrc == 0 )
										break;
								}
								else
								{
									*pDescDsc++ = *pDescSrc++;
									if( IsDBCSLeadByte( *pDescSrc ) )
										*pDescDsc++ = *pDescSrc++;
								}
							}
							pDescSrc = lpRegionElem->m_szTitle;
							pDescDsc = szDesc;
							int nCnt = 0;
							while( 1 )
							{
								if( ( *pDescSrc == '\r' && *(pDescSrc + 1 ) == '\n' ) || *pDescSrc == 0 )
								{
									*pDescDsc = 0;
									if( szDesc[ 0 ] )
									{
										if( nCnt == 0 )
										{
#if 0 //def __VBUGFIX_20050105
											g_Caption1.AddCaption( "～デミアンの居住地～"/*szDesc*/, m_pFontAPITitle );// CWndBase::m_Theme.m_pFontCaption );

#else
	#ifndef __VTITLECAPTION_JAPAN
											g_Caption1.AddCaption( szDesc, m_pFontAPITitle );// CWndBase::m_Theme.m_pFontCaption );
	#else //__VTITLECAPTION_JAPAN
											g_Caption1.AddCaption( szDesc, NULL );// CWndBase::m_Theme.m_pFontCaption );
	#endif //__VTITLECAPTION_JAPAN
#endif
										}
										else
	#ifndef __VTITLECAPTION_JAPAN
											g_Caption1.AddCaption( szDesc, m_pFontAPICaption );// CWndBase::m_Theme.m_pFontCaption );
	#else //__VTITLECAPTION_JAPAN
											g_Caption1.AddCaption( szDesc, NULL );// CWndBase::m_Theme.m_pFontCaption );
	#endif //__VTITLECAPTION_JAPAN
									}
									pDescDsc = szDesc;
									pDescSrc+=2;
									nCnt++;
									if( *pDescSrc == 0 )
										break;
								}
								else
								{
									*pDescDsc++ = *pDescSrc++;
									if( IsDBCSLeadByte( *pDescSrc ) )
										*pDescDsc++ = *pDescSrc++;
								}
							}
						}
						lpRegionElem->m_bInside = (bResult == TRUE);
					}
				}
			}
			else
				lpRegionElem->m_bInside = FALSE;
		}
		if( g_bKeyTable[ VK_LEFT ] )
		{
			g_Neuz.m_camera.m_fRotx -= 4;
			if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
			if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;
#ifdef __YGUIDESYSTEM0923
			if( m_pWndGuideSystem )
				m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_KEY_CAMERAMOVE);
#endif //__YGUIDESYSTEM0923
			
		}	
		if( g_bKeyTable[ VK_RIGHT ] )
		{
			g_Neuz.m_camera.m_fRotx += 4;
			if(g_Neuz.m_camera.m_fRotx > 360 ) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
			if(g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;
#ifdef __YGUIDESYSTEM0923
			if(m_pWndGuideSystem)
				m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_KEY_CAMERAMOVE);
#endif //__YGUIDESYSTEM0923
		}	
		if( g_bKeyTable[ VK_UP ] )
		{
			g_Neuz.m_camera.m_fRoty += 2;
			if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
			if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
#ifdef __YGUIDESYSTEM0923
			if(m_pWndGuideSystem)
				m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_KEY_CAMERAMOVE);
#endif //__YGUIDESYSTEM0923
		}
		if( g_bKeyTable[ VK_DOWN ] )
		{
			g_Neuz.m_camera.m_fRoty -= 2;
			if(g_Neuz.m_camera.m_fRoty>80-g_Neuz.m_camera.m_fZoom*4) g_Neuz.m_camera.m_fRoty=80-g_Neuz.m_camera.m_fZoom*4;
			if(g_Neuz.m_camera.m_fRoty<-80) g_Neuz.m_camera.m_fRoty=-80;
#ifdef __YGUIDESYSTEM0923
			if(m_pWndGuideSystem)
				m_pWndGuideSystem->SendGuideMessage(GUIDE_EVENT_KEY_CAMERAMOVE);
#endif //__YGUIDESYSTEM0923
		}	

		if( g_bKeyTable[ VK_PRIOR ])
		{
			g_Neuz.m_camera.m_fZoom-=0.6f;
			if(g_Neuz.m_camera.m_fZoom < 0)
				g_Neuz.m_camera.m_fZoom = 0;
		}
		if( g_bKeyTable[ VK_NEXT ])
		{
			g_Neuz.m_camera.m_fZoom+=0.6f;
			if(g_Neuz.m_camera.m_fZoom > 7)
				g_Neuz.m_camera.m_fZoom = 7;
		}

		if( g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->m_bIsIndoor == 1 )
			SetLODDetail( 0 );
	}
	if( nColor > 0 )
	{
		g_fAddScale += 0.002f;
		g_fXScale += g_fAddScale;
		g_fYScale += g_fAddScale;
		if( g_fXScale > 1.0f ) g_fXScale = 1.0f;
		if( g_fYScale > 1.0f ) g_fYScale = 1.0f;

		/*
		if( g_bAlpha == FALSE ) nColor++;
		if( g_bAlpha == TRUE ) nColor--;
		if( nColor > 255 ) { g_bAlpha = TRUE; }
		*/
	}

#ifdef __EVE_NEWYEAR
	//	c.12/31일 11시59분 부터 프리프 인터페이스에 공지처럼 시간이 나오는것.
	//	예)23시59분때 게임창에 59초 58초 57초 56초 55초 54초........01초 happy new year
	if( g_DPlay.m_nCountdown > 0 && ( g_nProcessCnt & 63 ) == 0 )
	{
		g_DPlay.m_nCountdown--;
		if( g_DPlay.m_nCountdown == 0 )
		{
			//			게임창에 happy new year
			g_Caption1.RemoveAll();
			g_Caption1.AddCaption( "Happy New Year", m_pFontAPITitle );
		}
		else
		{
			//			게임창에 59초 58초 57초 56초 55초 54초........01초
			
			char buffer[64];
			_itoa( g_DPlay.m_nCountdown, buffer, 10 );
			
			g_Caption1.RemoveAll();
			g_Caption1.AddCaption( buffer, m_pFontAPITitle );
		}
	}
#endif	// __EVE_NEWYEAR
	
	g_Caption1.Process();
#ifdef __V050322_CACHEQUEST
	g_CapTime.Process();
#endif // __V050322_CACHEQUEST


	if( g_pPlayer )
		m_SkillState.Process();

	DWORD dwBufCount = GetTickCount();
	if( g_pPlayer && dwBufCount >= m_dwOneSecCount + 1000 )
	{
		int Mod = ( dwBufCount - m_dwOneSecCount ) % 1000;
		m_dwOneSecCount = GetTickCount() - Mod;
		for( int j = 0 ; j < SM_MAX ; ++j )
		{
			if( 0 < g_pPlayer->m_dwSMTime[j] && g_pPlayer->m_dwSMTime[j] != 1 )
			{
				--g_pPlayer->m_dwSMTime[j];
			}
		}
	}

#ifdef __YCLOTH
	if( g_pPlayer )
	{
		D3DXMATRIX  mat1, mat2;
		D3DXVECTOR3 Pos1, Pos2;

		mat1 = ((CModelObject*)g_pPlayer->m_pModel)->m_mUpdateBone[10];
		mat2 = ((CModelObject*)g_pPlayer->m_pModel)->m_mUpdateBone[14];

		mat1 = mat1 * g_pPlayer->GetMatrixWorld();
		mat2 = mat2 * g_pPlayer->GetMatrixWorld();
		
		Pos1 = D3DXVECTOR3( mat1._41, mat1._42, mat1._43 );
		Pos2 = D3DXVECTOR3( mat2._41, mat2._42, mat2._43 );

		g_Cloth.Process(0,Pos2, Pos1);
	}
#endif

	if( g_Neuz.m_dwTimeReSkillPoint < GetTickCount() )
	{
		if( g_pPlayer && g_pPlayer->m_dwSkillPoint > 0 && g_pPlayer->IsExpert() )
		{
#ifdef __YADV_RESKILLPOINT
			g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_SKILL );		
#else //__YADV_RESKILLPOINT
			SAFE_DELETE(m_pWndReSkillPoint);
			
			switch( g_pPlayer->GetJob() )
			{
			case JOB_MERCENARY:
				m_pWndReSkillPoint = new CWndReSkillPoint_Me;
				break;
			case JOB_ASSIST:
				m_pWndReSkillPoint = new CWndReSkillPoint_As;
				break;
			case JOB_MAGICIAN:
				m_pWndReSkillPoint = new CWndReSkillPoint_Ma;
				break;
			}
			
			if( m_pWndReSkillPoint )
			{
				g_Neuz.m_dwTimeReSkillPoint = 0xffffffff;
				m_pWndReSkillPoint->Initialize(this);
				m_pWndReSkillPoint->InitItem( g_pPlayer->m_dwSkillPoint, g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );
			}
			else
				Error( "스킬재분배창 초기화 에러 NULL" );
#endif //__YADV_RESKILLPOINT
		}		
	}		

	return CWndNeuz::Process();
}
HRESULT CWndWorld::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	m_meshArrow.RestoreDeviceObjects();

  #ifdef __NPP_050308
	m_meshArrowWanted.RestoreDeviceObjects();
  #endif //__NPP_050308
//	m_texTarget.RestoreDeviceObjects();
	if( m_pFontAPICaption )
		m_pFontAPICaption->RestoreDeviceObjects();
	if( m_pFontAPITitle )
		m_pFontAPITitle->RestoreDeviceObjects();
#ifdef __V050322_CACHEQUEST
	if( m_pFontAPITime )
		m_pFontAPITime->RestoreDeviceObjects();
#endif
	
#ifdef __YDEBUG
	m_texTarget.RestoreDeviceObjects(m_pApp->m_pd3dDevice);		// 지상에서의 4귀퉁이 타겟그림
	m_texTargetFly.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행중에서의 4귀퉁이 타겟그림.
	m_texTargetArrow.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 타겟이 화면을 벗어났을때 화살표방향표시.
	m_texGauFlight.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.

	m_texMsgIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.
	m_texAttrIcon.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.
	m_texFontDigital.RestoreDeviceObjects(m_pApp->m_pd3dDevice);	// 비행 게이지 인터페이스.
#endif //__YDEBUG	


	return S_OK;
}
HRESULT CWndWorld::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	m_meshArrow.InvalidateDeviceObjects();
  #ifdef __NPP_050308
	m_meshArrowWanted.InvalidateDeviceObjects();
  #endif //__NPP_050308
	
//	m_texTarget.InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );

	if( m_pFontAPICaption )
		m_pFontAPICaption->InvalidateDeviceObjects();
	if( m_pFontAPITitle )
		m_pFontAPITitle->InvalidateDeviceObjects();
#ifdef __V050322_CACHEQUEST
	if( m_pFontAPITime )
		m_pFontAPITime->InvalidateDeviceObjects();
	g_CapTime.InvalidateDeviceObjects();
#endif
	g_Caption1.InvalidateDeviceObjects();

#ifdef __YDEBUG
	m_texTarget.InvalidateDeviceObjects();		// 지상에서의 4귀퉁이 타겟그림
	m_texTargetFly.InvalidateDeviceObjects();	// 비행중에서의 4귀퉁이 타겟그림.
	m_texTargetArrow.InvalidateDeviceObjects();	// 타겟이 화면을 벗어났을때 화살표방향표시.
	m_texGauFlight.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.


	m_texMsgIcon.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.
	m_texAttrIcon.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.
	m_texFontDigital.InvalidateDeviceObjects();	// 비행 게이지 인터페이스.
#endif //__YDEBUG	
	
	return S_OK;
}
HRESULT CWndWorld::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	m_meshArrow.DeleteDeviceObjects();

  #ifdef __NPP_050308
	m_meshArrowWanted.DeleteDeviceObjects();
  #endif //__NPP_050308
	
	m_texTarget.DeleteDeviceObjects();
	m_texTargetFly.DeleteDeviceObjects();
	m_texLvUp.DeleteDeviceObjects();
	m_texLvDn.DeleteDeviceObjects();
	m_texLvUp2.DeleteDeviceObjects();
	m_texLvDn2.DeleteDeviceObjects();
	m_texGauEmptyNormal.DeleteDeviceObjects();
	m_texGauFillNormal.DeleteDeviceObjects();
	if( m_pFontAPICaption )
		m_pFontAPICaption->DeleteDeviceObjects();
	SAFE_DELETE( m_pFontAPICaption );
	if( m_pFontAPITitle )
		m_pFontAPITitle->DeleteDeviceObjects();
	SAFE_DELETE( m_pFontAPITitle );
#ifdef __V050322_CACHEQUEST
	if( m_pFontAPITime )
		m_pFontAPITime->DeleteDeviceObjects();
	SAFE_DELETE( m_pFontAPITime );
	g_CapTime.DeleteDeviceObjects();
#endif
	g_Caption1.DeleteDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	
	for(int i=0; i<CUSTOM_LOGO_MAX; i++)
	{
		m_pTextureLogo[i].DeleteDeviceObjects();
	}

#ifdef __YGUIDESYSTEM0923
	SAFE_DELETE(m_pWndGuideSystem);
#endif //CWndCGuideSystem 
	
	SAFE_DELETE(m_pWndReSkillPoint_Text );
	SAFE_DELETE(m_pWndReSkillPoint);

#ifdef __YDEBUG
	m_texTargetArrow.DeleteDeviceObjects();	// 타겟이 화면을 벗어났을때 화살표방향표시.
	m_texGauFlight.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
	
	
	m_texMsgIcon.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
	m_texAttrIcon.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
	m_texFontDigital.DeleteDeviceObjects();	// 비행 게이지 인터페이스.
#endif //__YDEBUG	
	
	return S_OK;
}

void CWndWorld::UseSkill()
{
	CWorld* pWorld = g_WorldMng.Get();
	
//	CObj* pTargetObj = CObj::m_pObjHighlight;		// 커서를 대고 있던 오브젝트가 하이라이트 오브젝이다.
	CObj* pTargetObj = pWorld->GetObjFocus();		// 선택되어 있는타겟에게 사용

	if( g_pPlayer->m_pActMover->IsFly() == FALSE )
	{
		if( pTargetObj )	// 커서를 대고 있던 오브젝트가 있으면

		{
			pWorld->SetObjFocus( pTargetObj );	// 그놈을 셀렉트 하는 동시에.
			
			CMover* pMover = (CMover*)pTargetObj;
			if( pMover->GetType() == OT_MOVER )
				m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
		} else
			m_dwNextSkill = NEXTSKILL_ACTIONSLOT;	// 스킬 사용 예약.
	}
}

#ifdef __SM_ITEM_2ND
void CWndWorld::RenderBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	int nTexture;
	RECT rectHittest;	
	multimap< DWORD, BUFFSKILL >::value_type* pp;
	
	if( bPlayer )
		nTexture = 0;
	else
		nTexture = 1;

	pInfo->pt.x += pInfo->nDelta;
	
	ItemProp* pItem = NULL;
	if( pSkill->wType == BUFF_SKILL )
	{
		pp = &(*(m_pBuffTexture[nTexture].find(pSkill->wID)));			
		if( bPlayer )
			pItem = prj.GetSkillProp( pSkill->wID );
		else
			pItem = prj.GetPartySkill( pSkill->wID );
	}
	else
	{
		pp = &(*(m_pBuffTexture[2].find(pSkill->wID)));
		pItem = prj.GetItemProp( pSkill->wID );
	}
	
	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;

	DWORD dwOddTime = pSkill->tmCount - (timeGetTime() - pSkill->tmTime);
	BOOL bFlash = ( dwOddTime < 20 * 1000 );		// 20초 이하 남았으면 깜빡거림
#ifdef __SM_ITEM_2ND
	if( pItem->dwID == II_SYS_SYS_SCR_RETURN )		// 귀환의 두루마리는 깜빡거림
		bFlash = TRUE;
#endif

	if( bFlash )		
	{
		p2DRender->RenderTexture( pInfo->pt, pp->second.m_pTexture, pp->second.m_nAlpha );
		
		if( pp->second.m_bFlsh == TRUE )
		{
			pp->second.m_nAlpha+=6;
			
			if( pp->second.m_nAlpha > 192 )
			{
				pp->second.m_nAlpha = 192;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha-=6;
			
			if( pp->second.m_nAlpha < 32 )
			{
				pp->second.m_nAlpha = 32;
				pp->second.m_bFlsh = TRUE;
			}
		}
	}
	else
	{
		p2DRender->RenderTexture( pInfo->pt, pp->second.m_pTexture, 192 );
	}
	
	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+32, pInfo->pt.y+32 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	if( pItem->dwItemRare == 102 )
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ) );
	else if( pItem->dwItemRare == 103 )
		strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ) );
	else
		strEdit.AddString( pItem->szName, 0xff0000ff );

	CString str;
	switch( pItem->dwID )
	{
	case II_SYS_SYS_SCR_SMELPROT:
#ifdef __SM_ITEM_2ND_EX
	case II_SYS_SYS_SCR_SMELPROT2:
#endif	// __SM_ITEM_2ND_EX
	case II_SYS_SYS_SCR_SMELTING:
		str.Format( "\n%s", prj.GetText( TID_GAME_DEMOL_USE ) );
		break;
#ifdef __SM_ITEM_2ND
	case II_SYS_SYS_SCR_RETURN:
		{
			//str.Format( "\n%s", prj.GetText( TID_TOOLTIP_RETURN_USE ) );

			BUFFICONRECT_INFO info;
			CopyRect( &info.rc, &rectHittest );
			info.dwID = pItem->dwID;
			m_rcCheck.push_back( info );			
		}
#endif
	default:
		{
			CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
			if( ct.GetHours() >= 1 )
				str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
			else
				str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );						// 분초
			RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
		}
		break;
	}

	CString strTemp;
	strTemp.Format( "\n%s", pItem->szCommand );

	strEdit += strTemp;
	strEdit += str;
	
	g_toolTip.PutToolTip( pSkill->wID, strEdit, rectHittest, ptMouse, 0 );	

	++pInfo->nCount;
	if( (pInfo->nCount % m_nLimitBuffCount) == 0 )
	{
		pInfo->pt.x  = (m_rectWindow.Width() / 2) - 100;
		pInfo->pt.y += GetBuffTimeGap();
	}		
}

#ifdef __OVERLAPPING_EXPITEM_S0601
void CWndWorld::RenderExpBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BUFFICON_INFO* pInfo, CPoint ptMouse, DWORD dwItemID )
{
	// 경험치 중복 아이템 : 루프를 돌아서 몇개인지 확인. 버프아이콘은 한개, 설명은 3개면 3개.
	int nExpCount = 0;
	DWORD dwExpTime[3];
	ZeroMemory( dwExpTime, sizeof(dwExpTime) );

	if( dwItemID == II_SYS_SYS_SCR_AMPESA || dwItemID == II_SYS_SYS_SCR_AMPESA1 || dwItemID == II_SYS_SYS_SCR_AMPESA2 )
		dwItemID = II_SYS_SYS_SCR_AMPESA;
	else if( dwItemID == II_SYS_SYS_SCR_AMPESB || dwItemID == II_SYS_SYS_SCR_AMPESB1 || dwItemID == II_SYS_SYS_SCR_AMPESB2 )
		dwItemID = II_SYS_SYS_SCR_AMPESB;
	else if( dwItemID == II_SYS_SYS_SCR_AMPESC || dwItemID == II_SYS_SYS_SCR_AMPESC1 || dwItemID == II_SYS_SYS_SCR_AMPESC2 )
		dwItemID = II_SYS_SYS_SCR_AMPESC;
	else
		dwItemID = II_SYS_SYS_SCR_AMPESD;

	for( int i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		SKILLINFLUENCE* pSkillBuf = g_pPlayer->m_SkillState.Get(i);
		if( pSkillBuf->wID == dwItemID || pSkillBuf->wID == dwItemID + 1 || pSkillBuf->wID == dwItemID + 2 )	
		{
			dwExpTime[nExpCount] = pSkillBuf->tmCount - (timeGetTime() - pSkillBuf->tmTime);
			++nExpCount;			
		}
	}
	int nTexture;
	RECT rectHittest;	
	multimap< DWORD, BUFFSKILL >::value_type* pp;
	
	nTexture = 0;
	pInfo->pt.x += pInfo->nDelta;
	
	ItemProp* pItem = NULL;
	if( pSkill->wType == BUFF_SKILL )
	{
		pp = &(*(m_pBuffTexture[nTexture].find(pSkill->wID)));			
		pItem = prj.GetSkillProp( pSkill->wID );
	}
	else
	{
		pp = &(*(m_pBuffTexture[2].find( dwItemID )));
		pItem = prj.GetItemProp( dwItemID );
	}	
	ASSERT( pItem );
	if( pp->second.m_pTexture == NULL )
		return;

	D3DXCOLOR color;
	if( pItem->nEvildoing < 0 )
		color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );
	else
		color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );

	for( int j=0 ; j < nExpCount ; ++j )
	{
		if( j == 0 )
			p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1, 1, color );
		else if( j == 1 )
			p2DRender->RenderTexture2( CPoint( pInfo->pt.x + 5, pInfo->pt.y+30 ), pp->second.m_pTexture, 0.5f, 0.5f, color );
		else 
			p2DRender->RenderTexture2( CPoint( pInfo->pt.x + 22, pInfo->pt.y+30 ), pp->second.m_pTexture, 0.5f, 0.5f, color );
	}

	SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+32, pInfo->pt.y+32 );
	ClientToScreen( &rectHittest );
	
	CEditString strEdit;
	strEdit.AddString( pItem->szName, 0xff0000ff );
	
	float fFactor = 1;
	int nLevel = g_pPlayer->GetLevel();
	switch( nExpCount )
	{
	case 1:
		{
			fFactor *= 1.5f;
		}
		break;
	case 2:
		{
			if( nLevel < 61 )
				fFactor *= 2.0f;
			else
				fFactor *= 1.5f;
		}
		break;
	case 3:
		{
			if( nLevel < 41 )
				fFactor *= 2.5f;
			else if( nLevel < 61 )
				fFactor *= 2.0f;
			else
				fFactor *= 1.5f;
		}
		break;
	}
	
	int nMaxExpCount = 1;
	if( nLevel < 41 )
		nMaxExpCount = 3;
	else if( nLevel < 61 )
		nMaxExpCount = 2;
	
	DWORD dwColor;
	if( nExpCount == 1 )
		dwColor = D3DCOLOR_XRGB( 240, 240, 0 );
	else if( nExpCount == 2 )
		dwColor = D3DCOLOR_XRGB( 0, 240, 0 );
	
	if( nMaxExpCount <= nExpCount )
		dwColor = D3DCOLOR_XRGB( 240, 0, 0 );
	
	CString strTemp;
	//	strTemp.Format( "X %d", nExpCount );
	//	p2DRender->TextOut(  pInfo->pt.x+5, pInfo->pt.y+32, strTemp, dwColor );
	strEdit.AddString( '\n' );
	strEdit.AddString( prj.GetText( TID_GAME_EXPTOOLTIP ) );
	strTemp.Format( "%.1f", fFactor );
	strEdit.AddString( strTemp, D3DCOLOR_ARGB( 255, 255, 90, 80 ) );
	strEdit.AddString( prj.GetText( TID_GAME_EXPTOOLTIP1 ) );
	
	strEdit.AddString( '\n' );
	strEdit.AddString( prj.GetText( TID_GAME_EXP_COUTMSG0 ));//, 0xff99cc00 );
	for( i = 0 ; i < nExpCount ; ++i )
	{
		DWORD dwMsg = TID_GAME_EXP_COUTMSG3;
		if( i == 0 )
			dwMsg = TID_GAME_EXP_COUTMSG1;
		else if( i == 1 )
			dwMsg = TID_GAME_EXP_COUTMSG2;
		
		strEdit.AddString( "\n  " );
		strEdit.AddString( prj.GetText( dwMsg ) );
		
		CString strTime;
		DWORD dwOddTime = dwExpTime[i];		
		CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
		if( ct.GetHours() >= 1 )
			strTime.Format( " %.2d:%.2d:%.2d ", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
		else
			strTime.Format( " %.2d:%.2d ", ct.GetMinutes(), ct.GetSeconds() );						// 분초
		strEdit.AddString( strTime );
		//		if( i+1 < nExpCount )
		//			strEdit.AddString( '/', D3DCOLOR_ARGB( 255, 220, 80, 200 ) );
		//		RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
	}
	
	
	g_toolTip.PutToolTip( dwItemID, strEdit, rectHittest, ptMouse, 0 );	
	
	++pInfo->nCount;
	if( (pInfo->nCount % m_nLimitBuffCount) == 0 )
	{
		pInfo->pt.x  = (m_rectWindow.Width() / 2) - 100;
		pInfo->pt.y += GetBuffTimeGap();
	}
}
#endif // __OVERLAPPING_EXPITEM_S0601

//TODO m_rcCheck를 넣을 것. 
void CWndWorld::RenderSMBuff( C2DRender *p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	RECT rectHittest;	
	
	for( int i = 0; i < SM_MAX ; ++i )
	{
		if( g_pPlayer->m_dwSMTime[i] <= 0 )
			continue;
		
		int nResistTexture = 1000;
		if( i == SM_RESIST_ATTACK_LEFT )
			nResistTexture = g_pPlayer->m_nAttackResistLeft - 1;
		else if( i == SM_RESIST_ATTACK_RIGHT )
			nResistTexture = g_pPlayer->m_nAttackResistRight - 1;
		else if( i == SM_RESIST_DEFENSE )
			nResistTexture = g_pPlayer->m_nDefenseResist + 5 - 1;
		
		ItemProp* pItem = prj.GetItemProp( g_AddSMMode.dwSMItemID[i] );
		
		if( pItem != NULL &&
			( m_dwSMItemTexture[i] != NULL || 
			( ( i == SM_RESIST_ATTACK_LEFT || i== SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE ) && m_dwSMResistItemTexture[nResistTexture] != NULL && SAI79::END_PROP > nResistTexture ) 
			) )
		{
			CEditString strEdit;
			CTimeSpan ct( g_pPlayer->m_dwSMTime[i] );
			if( 0 == ct.GetDays() && 0 == ct.GetHours() && ct.GetMinutes() <= 30 || 
				i == SM_STR_DOWN || i == SM_STA_DOWN || i == SM_INT_DOWN || i == SM_DEX_DOWN )
			{
				pInfo->pt.x += (32+5);
				
				if( g_pPlayer->m_dwSMTime[i] < 60 && i != SM_REVIVAL )		// 20초 이하 남았으면 깜빡거림.
				{
					if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE )
						p2DRender->RenderTexture( pInfo->pt, m_dwSMResistItemTexture[nResistTexture], m_nSMAlpha[i] );
					else
						p2DRender->RenderTexture( pInfo->pt, m_dwSMItemTexture[i], m_nSMAlpha[i] );
					
					if( m_bSMFlsh[i] == TRUE )
					{
						m_nSMAlpha[i]+=6;
						
						if( m_nSMAlpha[i] > 192 )
						{
							m_nSMAlpha[i] = 192;
							m_bSMFlsh[i] = FALSE;
						}
					}
					else
					{
						m_nSMAlpha[i]-=6;
						
						if( m_nSMAlpha[i] < 32 )
						{
							m_nSMAlpha[i] = 32;
							m_bSMFlsh[i] = TRUE;
						}
					}
				}
				else
				{
					if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE )
						p2DRender->RenderTexture( pInfo->pt, m_dwSMResistItemTexture[nResistTexture], 192 );
					else
						p2DRender->RenderTexture( pInfo->pt, m_dwSMItemTexture[i], 192 );
				}
				
				SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+32, pInfo->pt.y+32 );
				ClientToScreen( &rectHittest );
				if( pItem->dwItemRare == 102 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ) );
				else if( pItem->dwItemRare == 103 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ) );
				else
					strEdit.AddString( pItem->szName, 0xff0000ff );
				
				CString str;
				if( pItem->dwCircleTime == 1 )
					str.Format( "\n%s", prj.GetText( TID_GAME_COND_USE ) );
				else
				{
					if( ct.GetHours() >= 1 )
						str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
					else
						str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );
				}
				strEdit += str;
				g_toolTip.PutToolTip( g_AddSMMode.dwSMItemID[i], strEdit, rectHittest, ptMouse, 0 );
				
				pInfo->nCount++;
				
				if( pItem->dwCircleTime != 1 )
					RenderOptBuffTime( p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
				
				if( (pInfo->nCount % m_nLimitBuffCount) == 0 )
				{
					pInfo->pt.x  = (m_rectWindow.Width() / 2) + 75;
					pInfo->pt.y += GetBuffTimeGap();
				}
			}
		}
	}
}

// 스킬버프는 모두 왼쪽에, 아이템 버프는 오른쪽에 표시한다.
void CWndWorld::RenderBuff(C2DRender *p2DRender)
{
	m_rcCheck.clear();

	if( g_pPlayer == NULL )
		return;
	int i=0, nCount = 0;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	SKILLINFLUENCE* pSkill;
	BUFFICON_INFO left, right;
	
	left.pt = CPoint( (m_rectWindow.Width() / 2) - 100, 10 );
	left.nCount = 0;
	left.nDelta = -(32+5);
	right.pt = CPoint( (m_rectWindow.Width() / 2) + 75, 10 );
	right.nCount = 0;	
	right.nDelta = (32+5);
	
	BUFFICON_INFO* pInfo;

#ifdef __OVERLAPPING_EXPITEM_S0601
	BOOL bExpRander[4];
	ZeroMemory( bExpRander, sizeof( bExpRander ) );
#endif // __OVERLAPPING_EXPITEM_S0601

	for( i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		pSkill = g_pPlayer->m_SkillState.Get(i);
		if( pSkill->wID  )	
		{
			if( pSkill->wType == BUFF_SKILL )
				pInfo = &left;
			else
				pInfo = &right;
			
			#ifdef __OVERLAPPING_EXPITEM_S0601
			int nExpkind = 100;
			if( pSkill->wID == II_SYS_SYS_SCR_AMPESA || pSkill->wID == II_SYS_SYS_SCR_AMPESA1 || pSkill->wID == II_SYS_SYS_SCR_AMPESA2 )
			{
				nExpkind = 0;
			}
			else if( pSkill->wID == II_SYS_SYS_SCR_AMPESB || pSkill->wID == II_SYS_SYS_SCR_AMPESB1 || pSkill->wID == II_SYS_SYS_SCR_AMPESB2 )
			{
				nExpkind = 1;
			}
			else if( pSkill->wID == II_SYS_SYS_SCR_AMPESC || pSkill->wID == II_SYS_SYS_SCR_AMPESC1 || pSkill->wID == II_SYS_SYS_SCR_AMPESC2 )
			{
				nExpkind = 2;
			}
			else if( pSkill->wID == II_SYS_SYS_SCR_AMPESD || pSkill->wID == II_SYS_SYS_SCR_AMPESD1 || pSkill->wID == II_SYS_SYS_SCR_AMPESD2 )
			{
				nExpkind = 3;
			}
			
			if( nExpkind != 100 )
			{
				if( bExpRander[nExpkind] == FALSE )
					RenderExpBuffIcon( p2DRender, pSkill, pInfo, ptMouse, pSkill->wID );		// 
				bExpRander[nExpkind] = TRUE;
			}
			else
			{
				RenderBuffIcon( p2DRender, pSkill, TRUE, pInfo, ptMouse );
			}
#else // __OVERLAPPING_EXPITEM_S0601
			RenderBuffIcon( p2DRender, pSkill, TRUE, pInfo, ptMouse );		// 
#endif // __OVERLAPPING_EXPITEM_S0601
		}
	}

	for( i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		pSkill = m_SkillState.Get(i);
		if( pSkill->wID )	
		{
			pInfo = &left;
			RenderBuffIcon( p2DRender, pSkill, FALSE, pInfo, ptMouse );		// 
		}
	}

	// 상용화 아이템 버프
	RenderSMBuff( p2DRender, &right, ptMouse );
}
#else // __SM_ITEM_2ND
//{{AFX
void CWndWorld::RenderBuff(C2DRender *p2DRender)
{
	if( g_pPlayer )
	{
		int i=0;
		int Count = 0;
		CPoint Lpoint = CPoint( (m_rectWindow.Width() / 2) - 100, 10 );

		SKILLINFLUENCE* pSkill = NULL;

		RECT rectHittest;
		CPoint ptMouse = GetMousePoint();
		ClientToScreen( &ptMouse );
		// 일반 스킬 버프 표시
		for( i=0; i<MAX_SKILLINFLUENCE; i++)
		{
			pSkill = g_pPlayer->m_SkillState.Get(i);

			ItemProp* pItem = NULL;
#ifdef __XITEMBUFF
			if( pSkill->wID == 0 )	continue;
			if( pSkill->wType == BUFF_SKILL )
				pItem = prj.GetSkillProp( pSkill->wID );
			else
				pItem = prj.GetItemProp( pSkill->wID );
#else
			if( pSkill->dwSkill == 0 )		continue;
			pItem = prj.GetSkillProp( pSkill->dwSkill );
#endif


			multimap< DWORD, BUFFSKILL >::value_type* pp;
#ifdef __XITEMBUFF
			if( pSkill->wType == BUFF_ITEM )
				pp = &(*(m_pBuffTexture[2].find(pSkill->wID)));
			else
				pp = &(*(m_pBuffTexture[0].find(pSkill->wID)));			
#else
			pp = &(*(m_pBuffTexture[0].find(pSkill->dwSkill)));
#endif
			
			if( pp->second.m_pTexture != NULL )
			{
				Lpoint.x -= (32+5);

//				DWORD dwTimeCurrent	= timeGetTime();		// 현재 시간 읽음.
				
//				if( (dwTimeCurrent - pSkill->dwStartTime) > (pSkill->dwTime)-20000 )	
				DWORD dwOddTime = pSkill->tmCount - (timeGetTime() - pSkill->tmTime);
				if( dwOddTime < 20 * 1000 )		// 20초 이하 남았으면 깜빡거림.
				{
					p2DRender->RenderTexture( Lpoint, pp->second.m_pTexture, pp->second.m_nAlpha );

					if( pp->second.m_bFlsh == TRUE )
					{
						pp->second.m_nAlpha+=6;

						if( pp->second.m_nAlpha > 192 )
						{
							pp->second.m_nAlpha = 192;
							pp->second.m_bFlsh = FALSE;
						}
					}
					else
					{
						pp->second.m_nAlpha-=6;

						if( pp->second.m_nAlpha < 32 )
						{
							pp->second.m_nAlpha = 32;
							pp->second.m_bFlsh = TRUE;
						}
					}
				}
				else
				{
					p2DRender->RenderTexture( Lpoint, pp->second.m_pTexture, 192 );
				}

				SetRect( &rectHittest, Lpoint.x, Lpoint.y, Lpoint.x+32, Lpoint.y+32 );
				ClientToScreen( &rectHittest );
				
				CString str;
//				CTimeSpan ct( (pSkill->dwTime - (dwTimeCurrent - pSkill->dwStartTime)) / 1000 );
				CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
				str.Format( "%s\n%.2d:%.2d", pItem->szName, ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
#ifdef __XITEMBUFF
				g_toolTip.PutToolTip( pSkill->wID, str, rectHittest, ptMouse, 0 );	
#else
				g_toolTip.PutToolTip( pSkill->dwSkill, str, rectHittest, ptMouse, 0 );	
#endif
				Count++;
				
				RenderOptBuffTime( p2DRender, Lpoint, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );

				if( (Count % m_nLimitBuffCount) == 0 )
				{
					Lpoint.x  = (m_rectWindow.Width() / 2) - 100;
					Lpoint.y += GetBuffTimeGap();
				}
			}
		}

		// 극단 스킬 버프 표시
		Count = 0;
		CPoint Rpoint = CPoint( (m_rectWindow.Width() / 2) + 75, 10 );

		for( i = 0; i < MAX_SKILLINFLUENCE; i++ )
		{
			pSkill = m_SkillState.Get(i);
			
			ItemProp* pItem = NULL;
#ifdef __XITEMBUFF
			if( pSkill->wID == 0 )	continue;
			if( pSkill->wType == BUFF_SKILL )
				pItem = prj.GetPartySkill( pSkill->wID );
			else
				pItem = prj.GetItemProp( pSkill->wID );
#else
			if( pSkill->dwSkill == 0 )		continue;
			pItem = prj.GetPartySkill( pSkill->dwSkill );
#endif
			
			
			multimap< DWORD, BUFFSKILL >::value_type* pp;
#ifdef __XITEMBUFF
			if( pSkill->wType == BUFF_ITEM )
				pp = &(*(m_pBuffTexture[2].find(pSkill->wID)));
			else
				pp = &(*(m_pBuffTexture[1].find(pSkill->wID)));						
#else
			pp = &(*(m_pBuffTexture[1].find(pSkill->dwSkill)));
#endif

			if( pp->second.m_pTexture != NULL )
			{
				Rpoint.x += (32+5);
				
				DWORD dwTimeCurrent	= timeGetTime();		// 현재 시간 읽음.
				
				DWORD dwOddTime = pSkill->tmCount - (timeGetTime() - pSkill->tmTime);
				if( dwOddTime < 20 * 1000 )		// 20초 이하 남았으면 깜빡거림.
				{
					p2DRender->RenderTexture( Rpoint, pp->second.m_pTexture, pp->second.m_nAlpha );
					
					if( pp->second.m_bFlsh == TRUE )
					{
						pp->second.m_nAlpha+=6;
						
						if( pp->second.m_nAlpha > 192 )
						{
							pp->second.m_nAlpha = 192;
							pp->second.m_bFlsh = FALSE;
						}
					}
					else
					{
						pp->second.m_nAlpha-=6;
						
						if( pp->second.m_nAlpha < 32 )
						{
							pp->second.m_nAlpha = 32;
							pp->second.m_bFlsh = TRUE;
						}
					}
				}
				else
				{
					p2DRender->RenderTexture( Rpoint, pp->second.m_pTexture, 192 );
				}

				SetRect( &rectHittest, Rpoint.x, Rpoint.y, Rpoint.x+32, Rpoint.y+32 );
				ClientToScreen( &rectHittest );
				
				CString str;
				CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
				str.Format( "%s\n%.2d:%.2d", pItem->szName, ct.GetMinutes(), ct.GetSeconds() );
#ifdef __XITEMBUFF
				g_toolTip.PutToolTip( pSkill->wID, str, rectHittest, ptMouse, 0 );
#else
				g_toolTip.PutToolTip( pSkill->dwSkill, str, rectHittest, ptMouse, 0 );
#endif
				Count++;

				RenderOptBuffTime( p2DRender, Rpoint, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
				
				if( (Count % m_nLimitBuffCount) == 0 )
				{
					Rpoint.x  = (m_rectWindow.Width() / 2) + 75;
					Rpoint.y += GetBuffTimeGap();
				}
			}
		}

		// 유료 아이템 표시
		for( i = 0; i < SM_MAX ; ++i )
		{
			if( g_pPlayer->m_dwSMTime[i] <= 0 )
				continue;

			int nResistTexture = 1000;
			if( i == SM_RESIST_ATTACK_LEFT )
				nResistTexture = g_pPlayer->m_nAttackResistLeft - 1;
			else if( i == SM_RESIST_ATTACK_RIGHT )
				nResistTexture = g_pPlayer->m_nAttackResistRight - 1;
			else if( i == SM_RESIST_DEFENSE )
				nResistTexture = g_pPlayer->m_nDefenseResist + 5 - 1;

			ItemProp* pItem = prj.GetItemProp( g_AddSMMode.dwSMItemID[i] );
			
			if( pItem != NULL &&
				( m_dwSMItemTexture[i] != NULL || 
					( ( i == SM_RESIST_ATTACK_LEFT || i== SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE ) && m_dwSMResistItemTexture[nResistTexture] != NULL && SAI79::END_PROP > nResistTexture ) 
				) )
			{
				CEditString strEdit;
				CTimeSpan ct( g_pPlayer->m_dwSMTime[i] );
#ifdef __NPP_050308
				if( 0 == ct.GetDays() && 0 == ct.GetHours() && ct.GetMinutes() <= 30 || 
					i == SM_STR_DOWN || i == SM_STA_DOWN || i == SM_INT_DOWN || i == SM_DEX_DOWN )
#else //__NPP_050308
				if( 0 == ct.GetDays() && 0 == ct.GetHours() && ct.GetMinutes() <= 30 )
#endif //__NPP_050308
				{
					Rpoint.x += (32+5);
					
#ifdef __S050524_SM_ITEM
					if( g_pPlayer->m_dwSMTime[i] < 60 && i != SM_REVIVAL )		// 20초 이하 남았으면 깜빡거림.
#else // __S050524_SM_ITEM
					if( g_pPlayer->m_dwSMTime[i] < 60 )		// 20초 이하 남았으면 깜빡거림.
#endif // __S050524_SM_ITEM
					{
						if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE )
							p2DRender->RenderTexture( Rpoint, m_dwSMResistItemTexture[nResistTexture], m_nSMAlpha[i] );
						else
							p2DRender->RenderTexture( Rpoint, m_dwSMItemTexture[i], m_nSMAlpha[i] );
						
						if( m_bSMFlsh[i] == TRUE )
						{
							m_nSMAlpha[i]+=6;
							
							if( m_nSMAlpha[i] > 192 )
							{
								m_nSMAlpha[i] = 192;
								m_bSMFlsh[i] = FALSE;
							}
						}
						else
						{
							m_nSMAlpha[i]-=6;
							
							if( m_nSMAlpha[i] < 32 )
							{
								m_nSMAlpha[i] = 32;
								m_bSMFlsh[i] = TRUE;
							}
						}
					}
					else
					{
						if( i == SM_RESIST_ATTACK_LEFT || i == SM_RESIST_ATTACK_RIGHT || i == SM_RESIST_DEFENSE )
							p2DRender->RenderTexture( Rpoint, m_dwSMResistItemTexture[nResistTexture], 192 );
						else
							p2DRender->RenderTexture( Rpoint, m_dwSMItemTexture[i], 192 );
					}
					
					SetRect( &rectHittest, Rpoint.x, Rpoint.y, Rpoint.x+32, Rpoint.y+32 );
					ClientToScreen( &rectHittest );
#ifdef __YITEMTOOLTIP_COLOR
					if( pItem->dwItemRare == 102 )
						strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ) );
					else
					if( pItem->dwItemRare == 103 )
						strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ) );
					else
						strEdit.AddString( pItem->szName, 0xff0000ff );
#else //__YITEMTOOLTIP_COLOR
					strEdit.AddString( pItem->szName, 0xff0000ff );
#endif //__YITEMTOOLTIP_COLOR

					CString str;
#ifdef __NPP_050308
					if( pItem->dwCircleTime == 1 )
					{
#ifdef __S050524_SM_ITEM
						str.Format( "\n%s", prj.GetText( TID_GAME_COND_USE ) );
#endif // __S050524_SM_ITEM
					}
					else
					{
						str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );
					}
#else //__NPP_050308
					if( pItem->dwCircleTime == 1 )
					{
#ifdef __S050524_SM_ITEM
						str.Format( "\n%s", prj.GetText( TID_GAME_COND_USE ) );
#endif // __S050524_SM_ITEM
					}
					else
					{
						str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );			
					}
					
#endif //__NPP_050308
					strEdit += str;
					g_toolTip.PutToolTip( g_AddSMMode.dwSMItemID[i], strEdit, rectHittest, ptMouse, 0 );

					Count++;

					if( pItem->dwCircleTime != 1 )
						RenderOptBuffTime( p2DRender, Rpoint, ct, D3DCOLOR_XRGB( 240, 240, 0 ) );
					
					if( (Count % m_nLimitBuffCount) == 0 )
					{
						Rpoint.x  = (m_rectWindow.Width() / 2) + 75;
						Rpoint.y += GetBuffTimeGap();
					}
				}
			}
		}
	}

}
//}}AFX
#endif // __SM_ITEM_2ND

#ifdef __S0504_FLY
void CWndWorld::RenderCasting(C2DRender *p2DRender)
{
	if( g_pPlayer && g_pPlayer->IsStateMode( STATE_BASEMOTION_MODE ) )
	{
		if( g_pPlayer->m_nReadyTime )
		{
			CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( g_pPlayer->m_dwUseItemId );
			if( pItemElem )
			{
				DWORD dwTime = timeGetTime() - g_pPlayer->m_nReadyTime;
				// 게이지 출력
				CRect rect;// = GetClientRect();
				int nWidthClient = 200;
				int nWidth;

				nWidth = nWidthClient * ( (float)dwTime / (float)pItemElem->GetProp()->dwSkillReadyType );
				TRACE( "%d / %d : %f\n", pItemElem->GetProp()->dwSkillReadyType, dwTime, ( (float)dwTime / (float)pItemElem->GetProp()->dwSkillReadyType ) );
				int nPos = ( GetClientRect().Width() - nWidthClient ) / 2;
				rect = CRect( nPos, GetClientRect().Height() / 2, nPos + nWidthClient, 0 );
				CRect rectTemp = rect; 
				rectTemp.right = rectTemp.left + nWidth;
				ClientToScreen( rect );
				ClientToScreen( rectTemp );
				m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB( 200, 255, 255, 255 ), m_pVBGauge, &m_texGauEmptyNormal );
				m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
				m_Theme.MakeGaugeVertex( p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB( 128, 255, 15, 15 ), m_pVBGauge, &m_texGauEmptyNormal );
				m_Theme.RenderGauge( p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal );
			}
		}
	}
}
#endif // __S0504_FLY


#ifdef __YMONSTER_BUFFRENDER
void CWndWorld::RenderMoverBuff( CMover* pMover, C2DRender *p2DRender)
{
	if( pMover == NULL || p2DRender == NULL )
		return;

	int nIconSize = 24;
	
	CPoint Lpoint = CPoint( ( GetClientRect().Width() - 200 ) / 2 - 20, 44 );
	SKILLINFLUENCE* pSkill = NULL;
	
	int Count = 0;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );

#ifdef __OVERLAPPING_EXPITEM_S0601
	BOOL bExpRander = FALSE;
#endif // __OVERLAPPING_EXPITEM_S0601

	// 일반 스킬 버프 표시
	for( int i=0; i<MAX_SKILLINFLUENCE; i++)
	{
		pSkill = pMover->m_SkillState.Get(i);

		ItemProp* pItem = NULL;
#ifdef __XITEMBUFF
		if( pSkill->wID == 0 )	continue;
		DWORD dwSkillID = pSkill->wID;
#ifdef __OVERLAPPING_EXPITEM_S0601
		if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESA;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESB;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESC;
		else if( dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2 )
			dwSkillID = II_SYS_SYS_SCR_AMPESD;
#endif // __OVERLAPPING_EXPITEM_S0601

		if( pSkill->wType == BUFF_SKILL )
			pItem = prj.GetSkillProp( dwSkillID );
		else
			pItem = prj.GetItemProp( dwSkillID );
#else
		if( dwSkillID == 0 )		continue;
		pItem = prj.GetSkillProp( dwSkillID );
#endif
		
		
		multimap< DWORD, BUFFSKILL >::value_type* pp;
#ifdef __XITEMBUFF
		if( pSkill->wType == BUFF_ITEM )
			pp = &(*(m_pBuffTexture[2].find(dwSkillID)));
		else
		if( pSkill->wType == BUFF_SKILL )
			pp = &(*(m_pBuffTexture[0].find(dwSkillID)));			
		else
			pp = &(*(m_pBuffTexture[1].find(dwSkillID)));
#else
		pp = &(*(m_pBuffTexture[0].find(dwSkillID)));
#endif
		
		
		if( pp->second.m_pTexture != NULL )
		{
			DWORD dwOddTime = pSkill->tmCount - (timeGetTime() - pSkill->tmTime);

#ifdef __OVERLAPPING_EXPITEM_S0601
			if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2 
				|| dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2 
				|| dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2 
				|| dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2 
				)
				
			{
				if( bExpRander == FALSE )
				{
					Lpoint.x += nIconSize;
					pp->second.m_pTexture->Render( p2DRender, Lpoint, CPoint(nIconSize,nIconSize), 192 );
				}
			}
			else
			{
#endif // __OVERLAPPING_EXPITEM_S0601
				Lpoint.x += nIconSize;
				if( dwOddTime < 20 * 1000 )		// 20초 이하 남았으면 깜빡거림.
				{
					pp->second.m_pTexture->Render( p2DRender, Lpoint, CPoint(nIconSize,nIconSize), pp->second.m_nAlpha );
					
					if( pp->second.m_bFlsh == TRUE )
					{
						pp->second.m_nAlpha+=6;
						
						if( pp->second.m_nAlpha > 192 )
						{
							pp->second.m_nAlpha = 192;
							pp->second.m_bFlsh = FALSE;
						}
					}
					else
					{
						pp->second.m_nAlpha-=6;
						
						if( pp->second.m_nAlpha < 32 )
						{
							pp->second.m_nAlpha = 32;
							pp->second.m_bFlsh = TRUE;
						}
					}
				}
				else
				{
					pp->second.m_pTexture->Render( p2DRender, Lpoint, CPoint(nIconSize,nIconSize), 192 );
				}
#ifdef __OVERLAPPING_EXPITEM_S0601
			}
#endif // __OVERLAPPING_EXPITEM_S0601

#ifdef __OVERLAPPING_EXPITEM_S0601
			BOOL bExpMsg = TRUE;
			if( dwSkillID == II_SYS_SYS_SCR_AMPESA || dwSkillID == II_SYS_SYS_SCR_AMPESA1 || dwSkillID == II_SYS_SYS_SCR_AMPESA2
				|| dwSkillID == II_SYS_SYS_SCR_AMPESB || dwSkillID == II_SYS_SYS_SCR_AMPESB1 || dwSkillID == II_SYS_SYS_SCR_AMPESB2
				|| dwSkillID == II_SYS_SYS_SCR_AMPESC || dwSkillID == II_SYS_SYS_SCR_AMPESC1 || dwSkillID == II_SYS_SYS_SCR_AMPESC2
				|| dwSkillID == II_SYS_SYS_SCR_AMPESD || dwSkillID == II_SYS_SYS_SCR_AMPESD1 || dwSkillID == II_SYS_SYS_SCR_AMPESD2
				)
			{
				if( bExpRander )
					bExpMsg = FALSE;
				bExpRander = TRUE;

			}
			if( bExpMsg )
			{
#endif // __OVERLAPPING_EXPITEM_S0601
				SetRect( &rectHittest, Lpoint.x, Lpoint.y, Lpoint.x+nIconSize, Lpoint.y+nIconSize );
				ClientToScreen( &rectHittest );
				
				CString str;
				str.Format( "\n%s", pItem->szCommand );	
				
				CEditString strEdit;

				if( pItem->dwItemRare == 102 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 0, 93, 0 ) );
				else if( pItem->dwItemRare == 103 )
					strEdit.AddString( pItem->szName, D3DCOLOR_XRGB( 182, 0, 255 ) );
				else
					strEdit.AddString( pItem->szName, 0xff0000ff );

				strEdit += str;
				
				CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌
				str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );		// 남은시간을 분/초 형태로 출력.
				strEdit += str;
				
	#ifdef __XITEMBUFF
				g_toolTip.PutToolTip( dwSkillID, strEdit, rectHittest, ptMouse, 0 );
	#else
				g_toolTip.PutToolTip( dwSkillID, strEdit, rectHittest, ptMouse, 0 );
	#endif
				
				Count++;
				if( (Count % 8) == 0 )
				{
					Lpoint.x = ( GetClientRect().Width() - 200 ) / 2 - 20;
					Lpoint.y+= nIconSize;
				}
#ifdef __OVERLAPPING_EXPITEM_S0601
			}
#endif // __OVERLAPPING_EXPITEM_S0601
		}
	}	
}
#endif //__YMONSTER_BUFFRENDER

void CWndWorld::RenderWantedArrow()
{
#ifdef __NPP_050308
	CWorld* pWorld = g_WorldMng();
	if( !g_pPlayer || !pWorld ) 
		return; // 플레이어가 없으면 렌더 안한다

	if( !m_bRenderArrowWanted )
		return;

	if( timeGetTime() < m_dwRenderArrowTime + MIN(60) )
	{
		D3DXVECTOR3 vSrc = g_pPlayer->GetPos();
		D3DXVECTOR3 vDest = m_v3Dest;

		LPDIRECT3DDEVICE9 pd3dDevice = m_pApp->m_pd3dDevice;

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );

		// 화살표의 위치, 회전값을 결정한다.
		D3DXMatrixLookAtLH( &matWorld, &vDest, &vSrc, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		D3DXMatrixInverse (&matWorld,NULL,&matWorld);
		matWorld._41 = g_pPlayer->GetPos().x; matWorld._42 = g_pPlayer->GetPos().y + 2.0f; matWorld._43 = g_pPlayer->GetPos().z;

		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		m_meshArrowWanted.SetBlendFactor( 128 );
		m_meshArrowWanted.Render( pd3dDevice, &matWorld );
	}
	else
	{
		m_bRenderArrowWanted = FALSE;
	}
#endif //__NPP_050308
}

void CWndWorld::RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor )
{
#ifdef __YVISIBLE_BUFF_TIME
	if(g_Option.m_bVisibleBuffTimeRender)
	{
		CString str;

		if( ct.GetDays() >= 1 )
		{
			str.Format( prj.GetText(TID_PK_LIMIT_DAY), ct.GetDays() );		
			p2DRender->TextOut(  point.x+5, point.y+32, str, dwColor );
		}
		else
		if( ct.GetHours() >= 1 )
		{

			str.Format( prj.GetText(TID_PK_LIMIT_HOUR), ct.GetHours() );				
			p2DRender->TextOut(  point.x+5, point.y+32, str, dwColor );
		}
		else
		if( ct.GetMinutes() >= 1 )
		{
			str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes() );	
			p2DRender->TextOut(  point.x+5, point.y+32, str, dwColor );
		}
		else
		{
			str.Format( prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds() );	
			p2DRender->TextOut(  point.x+3, point.y+32, str, dwColor );
		}
	}
#endif //__YVISIBLE_BUFF_TIME
				
}

int CWndWorld::GetBuffTimeGap()
{
#ifdef __YVISIBLE_BUFF_TIME
	if(g_Option.m_bVisibleBuffTimeRender)
		return 40+5;
	else
#endif //__YVISIBLE_BUFF_TIME
		return 32+5;
	
}

BOOL CWndWorld::UpdateMailRender()
{
#ifdef __POST0906
	// 편지가 갱신이 되면 화면에 표시를 해줄것인가 말것인가를 체크한다.
	CMailBox* pMailBox	= CMailBox::GetInstance();

	BOOL bReadMail = FALSE;
	for( vector<CMail*>::iterator i = pMailBox->begin(); i != pMailBox->end(); ++i )
	{
		CMail* pMail	= *i;
		if( pMail->m_pItemElem != NULL || pMail->m_nGold != 0 )
		{
			bReadMail = TRUE;
			break;
		}
	}
	
	if( bReadMail )
	{	
		m_IsMailTexRender = TRUE;			
	}
	else
	{
		m_IsMailTexRender = FALSE;			
	}
#endif // __POST0906	

	return TRUE;
}
