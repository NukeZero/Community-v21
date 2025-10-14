
#include "StdAfx.h"
#include "langman.h"

#ifdef __FLYFF_INITPAGE_EXT
	#include "ResData.h"
#endif 

char		g_szVersion[128] = { NULL, };	

#define		ITHEME_TEXTURE_KEYCOLOR_	0xFFFF00FF

#define		DWCOLOR_BLACK				0xFF000000
#define		DWCOLOR_WHITE				0xFFFFFFFF

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CTheme::CTheme()
:	m_pd3dDevice( NULL )	
,	m_pVBTexture( NULL )
,	m_pVBGauge( NULL )
,	m_pActiveDesktop( NULL )
,	m_pFontGuildCombatText( NULL )
,	m_pFontGameTitle( NULL )
,	m_pFontSRMyGiuld( NULL )		
,	m_pFontWndTitle( NULL )
,	m_pFontSRGiuld( NULL )
,	m_pFontCaption( NULL )
,	m_pFontButton( NULL )
,	m_pFontStatus( NULL )
,	m_pFontWorld( NULL )
,	m_pFontText( NULL )
,	m_pFontChat( NULL )
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
,	m_pGameGradeTexture( NULL )
,	m_dwWallPaperType( DWCOLOR_BLACK )
,	m_bNudeSkin( FALSE )
{

#ifdef __FLYFF_INITPAGE_EXT
	m_pTitleWorld			= NULL;
	m_bLoadTerrainScript	= FALSE;
	m_bRenderTitleWorld		= FALSE;
	m_dwTexturAlpha1		= 0;
	m_dwTexturAlpha2		= 0;
	m_dwStartTime			= 0;
	m_dwEndTime				= 0;
	m_bStartCameraWork		= FALSE;
#endif 

}

//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CTheme::~CTheme()
{
	DeleteTheme();
}


//---------------------------------------------------------------------------------------------
// Set Detail Font Info ( 폰트 세부 정보 설정하기 )
// param	:	strFontName ( 폰트이름 : ex) gulim )
//				nFontSize	( 폰트크기 )
//				pConnectFont( 생성 된 폰트와 연결 될 포인터 )
//				strFontID	( 폰트만의 고유한 이름 )
//				nOutLine	( 외각선 라인의 개수 )
//				dwColor		( 폰트 색상 )
//				dwBgColor	( 외각선의 색상 )
//				dwFlags		( 폰트 옵션 : ex) D3DFONT_BOLD )
// return	:	bool
//---------------------------------------------------------------------------------------------
bool CTheme::CreateFont( CString strFontName, UINT nFontSize, CD3DFont*& pConnectFont, CString strFontID, UINT nOutLine, DWORD dwColor, DWORD dwBgColor, DWORD dwFlags )
{
	CD3DFont* pNewFont( NULL );
	pNewFont = new CD3DFont( strFontName, nFontSize, dwFlags );
	FLERROR_ASSERT_LOG_RETURN( pNewFont, false, _T("[테마] 폰트 생성 실패 [ %s ]."), strFontName );	

	pNewFont->m_nOutLine	= nOutLine;
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

	m_mapFont.Lookup( strFontID, (void*&)pConnectFont );

	FLINFO_LOG( PROGRAM_NAME, _T("[테마] 폰트생성 : 폰트이름 - [%s], 폰트크기 - [%u], 폰트 아이디 - [%s]"), strFontName, nFontSize, strFontID );
	return true;
}


//---------------------------------------------------------------------------------------------
// Init Font ( 폰트 생성 및 초기화 )
// param	: nMainLanguage
// return	: bool
//---------------------------------------------------------------------------------------------
bool CTheme::InitFont( int nMainLanguage )
{
	LANG_DATA const * pLangData = CLangMan::GetInstance()->GetLangData( nMainLanguage );
	FLERROR_ASSERT_LOG_RETURN( pLangData, false, _T("[테마] 폰트 정보 파일 읽어드리기 실패.") );	

	NEUZ_FONT NeuzFont = pLangData->stFont;

	CreateFont( NeuzFont.vecFontInfo[0].szFont,	NeuzFont.vecFontInfo[0].dwFontSize,	m_pFontText,			_T("FontText"),				NeuzFont.vecFontInfo[0].nOutLine, NeuzFont.vecFontInfo[0].dwColor, NeuzFont.vecFontInfo[0].dwBgColor, NeuzFont.vecFontInfo[0].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[1].szFont,	NeuzFont.vecFontInfo[1].dwFontSize,	m_pFontStatus,			_T("FontStatus"),			NeuzFont.vecFontInfo[1].nOutLine, NeuzFont.vecFontInfo[1].dwColor, NeuzFont.vecFontInfo[2].dwBgColor, NeuzFont.vecFontInfo[2].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[4].szFont,	NeuzFont.vecFontInfo[4].dwFontSize,	m_pFontWorld,			_T("FontWorld"),			NeuzFont.vecFontInfo[4].nOutLine, NeuzFont.vecFontInfo[4].dwColor, NeuzFont.vecFontInfo[4].dwBgColor, NeuzFont.vecFontInfo[4].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[5].szFont,	NeuzFont.vecFontInfo[5].dwFontSize,	m_pFontCaption,			_T("FontCaption"),			NeuzFont.vecFontInfo[5].nOutLine, NeuzFont.vecFontInfo[5].dwColor, NeuzFont.vecFontInfo[5].dwBgColor, NeuzFont.vecFontInfo[5].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[6].szFont,	NeuzFont.vecFontInfo[6].dwFontSize,	m_pFontButton,			_T("FontButtom"),			NeuzFont.vecFontInfo[6].nOutLine, NeuzFont.vecFontInfo[6].dwColor, NeuzFont.vecFontInfo[6].dwBgColor, NeuzFont.vecFontInfo[6].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[7].szFont,	NeuzFont.vecFontInfo[7].dwFontSize,	m_pFontSRMyGiuld,		_T("FontSRMyGuild"),		NeuzFont.vecFontInfo[7].nOutLine, NeuzFont.vecFontInfo[7].dwColor, NeuzFont.vecFontInfo[7].dwBgColor, NeuzFont.vecFontInfo[7].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[8].szFont,	NeuzFont.vecFontInfo[8].dwFontSize,	m_pFontSRGiuld,			_T("FontSRGuild"),			NeuzFont.vecFontInfo[8].nOutLine, NeuzFont.vecFontInfo[8].dwColor, NeuzFont.vecFontInfo[8].dwBgColor, NeuzFont.vecFontInfo[8].dwFlags );	
	CreateFont( NeuzFont.vecFontInfo[9].szFont,	NeuzFont.vecFontInfo[9].dwFontSize,	m_pFontGuildCombatText,	_T("FontGuildCombatText"),	NeuzFont.vecFontInfo[9].nOutLine, NeuzFont.vecFontInfo[9].dwColor, NeuzFont.vecFontInfo[9].dwBgColor, NeuzFont.vecFontInfo[9].dwFlags );	

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		CreateFont( NeuzFont.vecFontInfo[3].szFont,	NeuzFont.vecFontInfo[3].dwFontSize,	m_pFontWndTitle,	_T("FontWndTitle"),			2, DWCOLOR_WHITE, D3DCOLOR_ARGB( 255, 217, 91, 51 ), NeuzFont.vecFontInfo[3].dwFlags );	
		CreateFont( NeuzFont.vecFontInfo[0].szFont,	NeuzFont.vecFontInfo[0].dwFontSize,	m_pFontChat,		_T("FontChat"),				NeuzFont.vecFontInfo[0].nOutLine, DWCOLOR_BLACK, NeuzFont.vecFontInfo[0].dwBgColor, NeuzFont.vecFontInfo[0].dwFlags );	
	}
	
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CreateFont( NeuzFont.vecFontInfo[3].szFont,	NeuzFont.vecFontInfo[3].dwFontSize,	m_pFontWndTitle,	_T("FontWndTitle"),			NeuzFont.vecFontInfo[3].nOutLine, NeuzFont.vecFontInfo[3].dwColor, NeuzFont.vecFontInfo[3].dwBgColor, NeuzFont.vecFontInfo[3].dwFlags );	
		CreateFont( NeuzFont.vecFontInfo[0].szFont,	NeuzFont.vecFontInfo[0].dwFontSize,	m_pFontChat,		_T("FontChat"),				NeuzFont.vecFontInfo[0].nOutLine, NeuzFont.vecFontInfo[0].dwColor, NeuzFont.vecFontInfo[0].dwBgColor, NeuzFont.vecFontInfo[0].dwFlags );	
	}

	FLINFO_LOG( PROGRAM_NAME, "[테마] 폰트생성 완료" );
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

	int nMainLanguage( g_xFlyffConfig->GetMainLanguage() );
	
	CScanner scanner;
	if( FALSE == scanner.Load( MakePath( DIR_THEME, nMainLanguage,lpszFileName ) ) )
		FLERROR_ASSERT_LOG_RETURN( NULL, FALSE, _T("[테마] 테마 파일 읽어드리기 실패.") );	

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

			m_texWallPaper.LoadTexture( m_pd3dDevice, MakePath( DIR_THEME , nMainLanguage, scanner.token ),		ITHEME_TEXTURE_KEYCOLOR_ );
			m_texWndPaper.LoadTexture(	m_pd3dDevice, MakePath( DIR_THEME , nMainLanguage, "WindField.bmp" ),	ITHEME_TEXTURE_KEYCOLOR_ );
		}	
		else
		if( scanner.Token == "m_dwWallPaperType" )
		{
			scanner.GetToken();
			m_dwWallPaperType = scanner.GetNumber();
		}
		scanner.GetToken();
	}

	m_GameGradeScreenTexture.LoadTexture( m_pd3dDevice, MakePath( _T( DIR_THEME ), nMainLanguage, _T( "GameGradeWarningScreen.tga" ) ), ITHEME_TEXTURE_KEYCOLOR_ );
	m_pGameGradeTexture = CWndBase::m_textureMng.AddTexture( m_pd3dDevice, MakePath( _T( DIR_THEME  ), nMainLanguage, _T( "GameGradeMark.bmp" ) ), ITHEME_TEXTURE_KEYCOLOR_ );

	InitFont( nMainLanguage );
	SetVersion( nMainLanguage ); 

#ifdef __FLYFF_INITPAGE_EXT
	ReadTitleWorld();
#endif 

	FLINFO_LOG( PROGRAM_NAME, "[테마] 테마 불러오기 완료" );
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
	}
//	g_mesh.InitDeviceObjects( m_pd3dDevice );
	//g_mesh.OpenMesh( _T("obj_rideferriswheel.mes") );

//	g_mesh.OpenMesh( _T("obj_starship.mes") );
	
	return TRUE;//m_pFontGameTitle->InitDeviceObjects( pd3dDevice );
}
HRESULT CTheme::InvalidateDeviceObjects()
{
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
	m_GameGradeScreenTexture.DeleteDeviceObjects();
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

#ifdef __FLYFF_INITPAGE_EXT
	if(m_pTitleWorld != NULL)
	{
		m_pTitleWorld->InvalidateDeviceObjects();
		m_pTitleWorld->DeleteDeviceObjects();
		SAFE_DELETE(m_pTitleWorld);
	}
#endif //__FLYFF_INITPAGE_EXT

	return h;
}
HRESULT CTheme::RestoreDeviceObjects( )
{
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

#ifdef __FLYFF_INITPAGE_EXT
void CTheme::ReadTitleWorld()
{
	if(m_pTitleWorld == NULL)
	{
		m_pTitleWorld = new CWorld;
		if(m_pTitleWorld != NULL)
		{
			if(!m_bLoadTerrainScript)
				m_bLoadTerrainScript = prj.m_terrainMng.LoadScript("terrain.inc");
			
			if(m_bLoadTerrainScript)
			{
				if(m_pTitleWorld->InitDeviceObjects( m_pd3dDevice ) == S_OK)
				{
					if(m_pTitleWorld->OpenWorld( MakePath( DIR_WORLD, "WdArena" ), TRUE ))
					{
						D3DXVECTOR3 vecWorld(128.0f, 128.0f, 128.0f);
						m_pTitleWorld->ReadWorld(vecWorld);

						D3DXVECTOR3 vecPos(149.0f, 105.0f, 170.0f);
						D3DXVECTOR3 vecLookat(213.0f, 116.0f, 184.0f);
						CCamera camera;
						camera.SetPos(vecPos);
						camera.m_vLookAt = vecLookat;
						m_pTitleWorld->SetCamera(&camera);

						m_pFlyffLogo = CWndBase::m_textureMng.AddTexture( m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T( "flyfftitletest.bmp" ) ), 0xffff00ff );
						m_pGameGrade = CWndBase::m_textureMng.AddTexture( m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T( "gamegradetest.bmp" ) ), 0xffff00ff );
						m_pAeonLogo = CWndBase::m_textureMng.AddTexture( m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T( "aeonsoftlogotest.bmp" ) ), 0xffff00ff );
						m_pGalaLogo = CWndBase::m_textureMng.AddTexture( m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T( "galalogotest.bmp" ) ), 0xffff00ff );
						
						m_bRenderTitleWorld = TRUE;
					}
					else
						SAFE_DELETE(m_pTitleWorld);
				}
			}
		}
	}
}

void CTheme::DestoryTitleWorld()
{
	if(m_pTitleWorld != NULL)
	{
		m_pTitleWorld->InvalidateDeviceObjects();
		m_pTitleWorld->DeleteDeviceObjects();
		SAFE_DELETE(m_pTitleWorld);
	}

	m_dwTexturAlpha1 = 0;
	m_dwTexturAlpha2 = 0;
}
#endif //__FLYFF_INITPAGE_EXT

HRESULT CTheme::FrameMove()
{
#ifdef __FLYFF_INITPAGE_EXT
	if(m_pTitleWorld != NULL)
	{
		m_pTitleWorld->Process();

		if(!m_bStartCameraWork)
		{
			static const DWORD MAX_ALPHA = 255;
			static const DWORD EVENT_ALPHA = 155;
			static const DWORD EFFECT_ALPHA = 100;
			static const float ALPHA_FRAME = 2.0f;

			static bool bEffect = false;

			if( bEffect == false )
			{
				if(m_dwTexturAlpha1 < MAX_ALPHA)
				{
					m_dwTexturAlpha1 += static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha1 > MAX_ALPHA)
						m_dwTexturAlpha1 = MAX_ALPHA;
				}

				if(m_dwTexturAlpha1 > EVENT_ALPHA && m_dwTexturAlpha2 < MAX_ALPHA)
				{
					m_dwTexturAlpha2 += static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha2 > MAX_ALPHA)
						m_dwTexturAlpha2 = MAX_ALPHA;
				}
			}
			else
			{
				if(m_dwTexturAlpha1 > EFFECT_ALPHA)
				{
					m_dwTexturAlpha1 -= static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha1 < EFFECT_ALPHA)
						m_dwTexturAlpha1 = EFFECT_ALPHA;
				}

				if(m_dwTexturAlpha2 > EFFECT_ALPHA)
				{
					m_dwTexturAlpha2 -= static_cast<DWORD>(ALPHA_FRAME);
					if(m_dwTexturAlpha2 < EFFECT_ALPHA)
						m_dwTexturAlpha2 = EFFECT_ALPHA;
				}
			}

			if( m_dwTexturAlpha1 == MAX_ALPHA && m_dwTexturAlpha2 == MAX_ALPHA )
			{
				bEffect = true;
			}

			if( m_dwTexturAlpha1 == EFFECT_ALPHA && m_dwTexturAlpha2 == EFFECT_ALPHA )
			{
				bEffect = false;
			}
		}
	}
#endif //__FLYFF_INITPAGE_EXT
	return S_OK;
}
void CTheme::RenderTitle( C2DRender* p2DRender )
{
}
void CTheme::RenderDesktop( C2DRender* p2DRender )
{
#ifdef __FLYFF_INITPAGE_EXT
	D3DVIEWPORT9 viewport;
	viewport.X      = 0;
	viewport.Y      = 0;
	viewport.Width  = 1360;
	viewport.Height = 768;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	// 프로젝션 
	FLOAT fAspect = (FLOAT)viewport.Width / (FLOAT)viewport.Height;
	float fFov = D3DX_PI / 4.0f;
	float fNear = CWorld::m_fNearPlane;

	D3DXMatrixPerspectiveFovLH( &m_pTitleWorld->m_matProj, fFov, fAspect, fNear - 0.01f, CWorld::m_fFarPlane );
	p2DRender->m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_pTitleWorld->m_matProj );

	DWORD dwColor = CWorld::GetDiffuseColor();
	p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;

	// 필드 출력 
	if(m_pTitleWorld != NULL)
	{
		//static float fCameraPositionX = 445.0f;
		//static float fCameraPositionY = 160.0f;
		//static float fCameraPositionY = 572.0f;
		static const float START_X = 345.0f;
		static const float END_X = 445.0f;
		static const float START_TO_END_X = END_X - START_X;
		static const float START_Y = 110.0f;
		static const float END_Y = 160.0f;
		static const float START_TO_END_Y = END_Y - START_Y;
		static const float START_Z = 572.0f;
		static const float END_Z = 572.0f;
		static const float START_TO_END_Z = END_Z - START_Z;
		static const float SPEED_RATE = 500.0f;

		static float fCameraPositionX = START_X;
		static float fCameraSpeedX = START_TO_END_X / SPEED_RATE;
		if( fCameraPositionX <= END_X )
		{
			fCameraPositionX += fCameraSpeedX;
		}
		static float fCameraPositionY = START_Y;
		static float fCameraSpeedY = START_TO_END_Y / SPEED_RATE;
		if( fCameraPositionY <= END_Y )
		{
			fCameraPositionY += fCameraSpeedY;
		}
		static float fCameraPositionZ = START_Z;
		static float fCameraSpeedZ = START_TO_END_Z / SPEED_RATE;
		if( fCameraPositionZ <= END_Z )
		{
			fCameraPositionZ += fCameraSpeedZ;
		}
		D3DXVECTOR3 vecPos( fCameraPositionX, fCameraPositionY, fCameraPositionZ );
		D3DXVECTOR3 vecLookat(663.0f, 123.0f, 632.0f);
		CCamera camera;
		camera.SetPos(vecPos);
		camera.m_vLookAt = vecLookat;
		m_pTitleWorld->SetCamera( &camera );

		m_pTitleWorld->Render( p2DRender->m_pd3dDevice, m_pFontWorld );

		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );

		if(m_pFlyffLogo != NULL)
		{
			CPoint ptPos;
			ptPos.x = (g_Option.m_nResWidth / 2) - (m_pFlyffLogo->m_size.cx / 2);
			ptPos.y = g_Option.m_nResHeight / 9;
			m_pFlyffLogo->Render( p2DRender, ptPos, m_dwTexturAlpha1 );
		}
		if(m_pGameGrade != NULL)
		{
			CPoint ptPos;
			ptPos.x = (g_Option.m_nResWidth - 10) - m_pGameGrade->m_size.cx;
			ptPos.y = 20;
			m_pGameGrade->Render( p2DRender, ptPos, m_dwTexturAlpha2 );
		}
		if(m_pAeonLogo != NULL)	
		{
			CPoint ptPos;
			ptPos.x = (g_Option.m_nResWidth - 10) - m_pAeonLogo->m_size.cx;
			ptPos.y = g_Option.m_nResHeight - m_pAeonLogo->m_size.cy - 10;
			m_pAeonLogo->Render( p2DRender, ptPos, m_dwTexturAlpha2 );
		}
		if(m_pGalaLogo != NULL)
		{
			CPoint ptPos;
			ptPos.x = 10;
			ptPos.y = g_Option.m_nResHeight - m_pGalaLogo->m_size.cy - 10;
			m_pGalaLogo->Render( p2DRender, ptPos, m_dwTexturAlpha2 );
		}
	}
#else //__FLYFF_INITPAGE_EXT
//sun: Wide및 고해상도 추가
	int xOffset = 0;
	int rectWidth = 0;
	BOOL isWide = FALSE;

	CTexture texture
		= m_texWallPaper;
	texture.SetAutoFree( FALSE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	CRect rectWindow = p2DRender->m_clipRect;
	if( m_dwWallPaperType == WPT_STRETCH ) // 전체 늘리기 
	{
//sun: Wide및 고해상도 추가
		if(rectWindow.Width() == 1280 && (rectWindow.Height() == 720 || rectWindow.Height() == 768 || rectWindow.Height() == 800)) //Wide
		{
			rectWidth = 960;
			isWide = TRUE;
		}
		else
		{
			switch(rectWindow.Width())
			{
			case 1360:
				rectWidth = 1024;
				isWide = TRUE;
				break;
			case 1440:
				rectWidth = 1200;
				isWide = TRUE;
				break;
			case 1680:
				rectWidth = 1400;
				isWide = TRUE;
			    break;
			}
		}

		if(isWide)
		{
			texture.m_size.cx = rectWidth;
			texture.m_size.cy = rectWindow.Height();
			xOffset = (rectWindow.Width() - rectWidth) / 2;
		}
		else
		{
			texture.m_size.cx = rectWindow.Width();
			texture.m_size.cy = rectWindow.Height();
		}

		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( xOffset, 0 ), &texture );

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
//sun: Wide및 고해상도 추가
	p2DRender->TextOut( 1 + xOffset, 1, "Version", 0xffffffff  );
	p2DRender->TextOut( 50 + xOffset, 1, g_szVersion, 0xffffffff  );

#endif //__FLYFF_INITPAGE_EXT
}

void CTheme::RenderGameGradeScreen( C2DRender* p2DRender )
{
	int xOffset = 0;
	int rectWidth = 0;
	BOOL isWide = FALSE;
	CTexture texture;
	texture = m_GameGradeScreenTexture;
	texture.SetAutoFree( FALSE );
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	CRect rectWindow = p2DRender->m_clipRect;
	if( g_WndMng.m_Theme.m_dwWallPaperType == WPT_STRETCH ) // 전체 늘리기 
	{
		if(rectWindow.Width() == 1280 && (rectWindow.Height() == 720 || rectWindow.Height() == 768 || rectWindow.Height() == 800)) //Wide
		{
			rectWidth = 960;
			isWide = TRUE;
		}
		else
		{
			switch(rectWindow.Width())
			{
			case 1360:
				rectWidth = 1024;
				isWide = TRUE;
				break;
			case 1440:
				rectWidth = 1200;
				isWide = TRUE;
				break;
			case 1680:
				rectWidth = 1400;
				isWide = TRUE;
				break;
			}
		}

		if(isWide)
		{
			texture.m_size.cx = rectWidth;
			texture.m_size.cy = rectWindow.Height();
			xOffset = (rectWindow.Width() - rectWidth) / 2;
		}
		else
		{
			texture.m_size.cx = rectWindow.Width();
			texture.m_size.cy = rectWindow.Height();
		}

		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, g_WndMng.m_Theme.m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( xOffset, 0 ), &texture );
	}
	else if( g_WndMng.m_Theme.m_dwWallPaperType == WPT_CENTER ) // 중앙 정렬 
	{
		CPoint pt( ( rectWindow.Width() / 2 ) - ( texture.m_size.cx / 2 ), ( rectWindow.Height() / 2 ) - ( texture.m_size.cy / 2 ) );
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, g_WndMng.m_Theme.m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( pt, &texture );
	}
	else if( g_WndMng.m_Theme.m_dwWallPaperType == WPT_CENTERSTRETCH ) // 중앙 늘리기  
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
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, g_WndMng.m_Theme.m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( pt, &texture );
	}
	else if( g_WndMng.m_Theme.m_dwWallPaperType == WPT_TILE ) // 타일 정렬 
	{
		FLOAT fu = (FLOAT)rectWindow.Width()  / texture.m_size.cx;
		FLOAT fv = (FLOAT)rectWindow.Height() / texture.m_size.cy;
		texture.m_size.cx = rectWindow.Width();
		texture.m_size.cy = rectWindow.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu  ; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv  ;
		texture.m_fuRB = fu  ; texture.m_fvRB = fv  ;
		p2DRender->m_pd3dDevice->Clear(0, NULL,  D3DCLEAR_TARGET, g_WndMng.m_Theme.m_d3dcBackground, 1.0f, 0 ) ;
		p2DRender->RenderTexture( CPoint( 0, 0), &texture );
	}
}

void CTheme::RenderGameGradeMark( C2DRender* p2DRender, DWORD dwAlpha )
{
	if( m_pGameGradeTexture )
	{
		CPoint ptPos( ( g_Option.m_nResWidth - 10 ) - m_pGameGradeTexture->m_size.cx, 20 );
		m_pGameGradeTexture->Render( p2DRender, ptPos, dwAlpha );
	}
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

	p2DRender->TextOut( nX, nY, lpszTitle, dwColor);
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
#ifdef __IMPROVE_MAP_SYSTEM
	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 200, 200, 200 );//D3DCOLOR_ARGB(255,200,200,250);//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB(255,150,150,200);//
#else __IMPROVE_MAP_SYSTEM
	DWORD dwColor1 = D3DCOLOR_ARGB( 255, 255, 255, 255 );//D3DCOLOR_ARGB(255,200,200,250);//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 200, 200, 200 );//D3DCOLOR_ARGB(255,150,150,200);//
#endif // __IMPROVE_MAP_SYSTEM
	
	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderFillRect( *pRect, dwColor2 );
}
void CTheme::RenderWndTextFrame( C2DRender* p2DRender, CRect* pRect  )
{
	//DWORD dwColor1 = D3DCOLOR_ARGB(  88,  50,  50,  50 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//

	DWORD dwColor1 = 0, dwColor2 = 0;

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 255,255,255 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );//
		dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 226,198,181 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//
	}
	else if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		dwColor1 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 40,40,40 );
		dwColor2 = D3DCOLOR_ARGB( CWndBase::m_nAlpha - 32, 50,55,60 );//D3DCOLOR_ARGB( 255,  80,  80, 120 );//
	}

	p2DRender->RenderFillRect ( *pRect, dwColor1 );
	p2DRender->RenderRoundRect( *pRect, dwColor2 );

}
//////////////////////////

DWORD CTheme::GetButtonFontColor( CWndButton* pWndButton  )
{
	if( pWndButton->IsWindowEnabled() )
	{
		DWORD dwColor = pWndButton->GetFontColor();
		
		if( pWndButton->IsHighLight() )
			dwColor = pWndButton->GetHighLightFontColor();

		if( pWndButton->IsPush() )
			dwColor = pWndButton->GetPushFontColor();

		return dwColor;
	}

	return pWndButton->GetDisableFontColor();
}
POINT CTheme::GetButtonTextPos( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	if( ! ( pWndButton->GetStyle() & WBS_NOCENTER ) )
		return pWndButton->GetStrCenter( p2DRender, pWndButton->GetTitle() );
	return CPoint( 0, 0);
}



//---------------------------------------------------------------------------------------------
// 윈도우 버튼 그리기
// param	: p2DRender, pRect, nX, nY, lpszTitle, dwColor
// return	: void 
//---------------------------------------------------------------------------------------------
void CTheme::RenderWndButton( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender,	_T("[테마] 2D Render 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndButton,	_T("[테마] 윈도우 버튼 포인터가 유효하지 않습니다.") );

	int   		nCheck( pWndButton->GetCheck() );
	BOOL		bPush( pWndButton->IsPush() );
	BOOL		bEnable( pWndButton->IsWindowEnabled() );

	DWORD 		dwStringColor( GetButtonFontColor( pWndButton ) );
	DWORD 		dwStringShadowColor( pWndButton->GetShadowFontColor() ); 

	if( pWndButton->m_pTexture )
	{
		RenderWndButton_4Texture( p2DRender, pWndButton );

		switch( _GetContentState( CT_NEWUI_19 ) )
		{
		case CS_VER1 : 
			//이미지를 쓰지만 강제로 title을 출력해야 하는경우, by gmpbigsun( 20110620 )
			if( FALSE == pWndButton->m_bForcedDrawTitle )
				return;

		case CS_VER2:
			//타이틀이 없다면 패스 ( 이미지 보호 )
			if( true == (pWndButton->GetTitle( )).IsEmpty() )
				return;
		}
	}

	if( nCheck )
		dwStringColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		POINT pt = GetButtonTextPos( p2DRender, pWndButton );

		p2DRender->TextOut( pt.x+1, pt.y+1, pWndButton->GetTitle(), dwStringShadowColor );
		p2DRender->TextOut( pt.x, pt.y, pWndButton->GetTitle(), dwStringColor );
	}
	else
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CTheme::m_pFontButton );
		
		POINT pt = GetButtonTextPos( p2DRender, pWndButton );

		if( FALSE == bPush )
		{
			if( bEnable )
				p2DRender->TextOut( pt.x+1, pt.y+2, pWndButton->GetTitle(), dwStringShadowColor );
			
			p2DRender->TextOut( pt.x, pt.y+1, pWndButton->GetTitle(), dwStringColor );
		}
		else
		{
			if( bEnable )
				p2DRender->TextOut( pt.x+3, pt.y+4, pWndButton->GetTitle(), dwStringShadowColor );
			
			p2DRender->TextOut( pt.x+2, pt.y+3, pWndButton->GetTitle(), dwStringColor );
		}

		p2DRender->SetFont( pOldFont );

		// 이미 유효한 texture로 그렸기때문에 Text 만 출력하고 return
		if( pWndButton->m_pTexture )
			return;
	}

	DWORD dwColor1 = ( bPush ?	D3DCOLOR_ARGB( 255,   100, 255, 255 ) : D3DCOLOR_ARGB( 250, 255, 255, 255 ) );
	DWORD dwColor2 = ( bPush ?	D3DCOLOR_ARGB(  50,     0,   0,   0 ) : D3DCOLOR_ARGB(  50,   0,   0,   0 ) );
	DWORD dwColor3 = ( bPush ?	D3DCOLOR_ARGB( 200,     0, 150, 150 ) : D3DCOLOR_ARGB( 200, 150, 150, 150 ) );

	CRect rect = pWndButton->GetClientRect();
	rect.DeflateRect( 2, 2);
	GradationRect( p2DRender, &rect, dwColor1 ,dwColor2, dwColor3 );
	p2DRender->RenderLine( CPoint( rect.left, rect.top ), CPoint( rect.right, rect.top ), dwColor3 );

	rect.InflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(  155, 200, 200, 200 ) );
	rect.InflateRect(1,1);
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(  155,  50,  50,  50 ) );
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


//---------------------------------------------------------------------------------------------
// 윈도우 체크 박스 버튼 그리기
// param	: p2DRender, pWndButton
// return	: void 
//---------------------------------------------------------------------------------------------
void CTheme::RenderWndButtonCheck( C2DRender* p2DRender, CWndButton* pWndButton  )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender,	_T("[테마] 렌더러 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndButton, _T("[테마] 윈도우 버튼 포인터가 유효하지 않습니다.") );

	int		nFontHeight = pWndButton->GetFontHeight();
	DWORD	dwColor		= GetButtonFontColor( pWndButton );

	if( pWndButton->m_pTexture ) 
	{
		RenderWndButton_6Texture( p2DRender, pWndButton );
		p2DRender->TextOut( pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor );
	}
	else
	{
		BOOL  bPush       = pWndButton->IsPush();
		BOOL  bHighLight  = pWndButton->IsHighLight();
		BOOL  bEnable     = pWndButton->IsWindowEnabled();

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


//---------------------------------------------------------------------------------------------
// 윈도우 라디오 버튼 그리기
// param	: p2DRender, pWndButton
// return	: void 
//---------------------------------------------------------------------------------------------
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
		p2DRender->TextOut( pWndButton->m_pTexture->m_size.cx / 6 + 2, 
							p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, 
							pWndButton->GetTitle(), dwColor );
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
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

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
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice; 
	CPoint pt = pRect->TopLeft();
	pt += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = (int)( (FLOAT)pTexture->m_size.cx / 3.0f);
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = (FLOAT)( pt.x );
	FLOAT top    = (FLOAT)( pt.y );
	FLOAT right  = (FLOAT)( pt.x + nTexWidth );//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)( pt.y + nTexHeight );//( pTexture->m_size.cy );

	int nWidth = (int)( (FLOAT)pRect->Width() / (FLOAT)nTexWidth + 0.5f );// - 2;		//반올림? 반내림?

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	TEXTUREVERTEX2* pVertices = NULL;
	TEXTUREVERTEX2* pVertices_ = NULL;
	HRESULT hr = pVB->Lock( 0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**) &pVertices_, D3DLOCK_DISCARD );
	if(hr != D3D_OK) 
		return;

	SIZE size = pTexture->m_size;//
	SIZE sizePitch = pTexture->m_sizePitch;//
	size.cx /= 3;
	
	pVertices = pVertices_;
	for( int i = 0; i < nVertexNum; i++ )
	{
		pVertices->vec.z = 0.0f;
		pVertices->rhw = 1.0f;
		pVertices->color = dwColor;
		pVertices++;
	}
	pVertices = pVertices_;

	left   = (FLOAT)( pt.x );
	right  = (FLOAT)( pt.x + nTexWidth );
	
	pVertices->vec.x = left;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = 0.0f;
	pVertices->v = 0.0f;
	pVertices++;

	pVertices->vec.x = right;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = (FLOAT)size.cx / (FLOAT)sizePitch.cx;
	pVertices->v = 0.0f;
	pVertices++;

	pVertices->vec.x = left;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = 0.0f;
	pVertices->v = (FLOAT)size.cy / (FLOAT)sizePitch.cy;
	pVertices++;

	pVertices->vec.x = right;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = (FLOAT)size.cx / (FLOAT)sizePitch.cx;
	pVertices->v = 0.0f;
	pVertices++;
	
	pVertices->vec.x = left;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = 0.0f;
	pVertices->v = (FLOAT)size.cy / (FLOAT)sizePitch.cy;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = (FLOAT)size.cx / (FLOAT)sizePitch.cx;
	pVertices->v = (FLOAT)size.cy / (FLOAT)sizePitch.cy;
	pVertices++;

	//////////////////////////////////
	left   = (FLOAT)( pt.x + nTexWidth );
	right  = (FLOAT)( pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth ) );
	
	pVertices->vec.x = left ;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 1.0f;
	pVertices->v = 0.0f;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 2.0f;
	pVertices->v = 0.0f;
	pVertices++;
	
	pVertices->vec.x = left;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 1.0f;
	pVertices->v = ( (FLOAT)size.cy / (FLOAT)sizePitch.cy ) * 1.0f;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 2.0f;
	pVertices->v = 0.0f;
	pVertices++;
	
	pVertices->vec.x = left;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 1.0f;
	pVertices->v = ( (FLOAT)size.cy / (FLOAT)sizePitch.cy ) * 1.0f;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 2.0f;
	pVertices->v = ( (FLOAT)size.cy / (FLOAT)sizePitch.cy ) * 1.0f;
	pVertices++;

	//////////////////////////////////
	left   = (FLOAT)( pt.x + ( ( nWidth - 1 ) * nTexWidth ) );
	right  = (FLOAT)( pt.x + ( ( nWidth ) * nTexWidth ) );

	pVertices->vec.x = left;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 2.0f;
	pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 3.0f;
	pVertices->v = 0.0f;
	pVertices++;
	
	pVertices->vec.x = left;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 2.0f;
	pVertices->v = ( (FLOAT)size.cy / (FLOAT)sizePitch.cy ) * 1.0f;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = top - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 3.0f;
	pVertices->v = 0.0f;
	pVertices++;
	
	pVertices->vec.x = left;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 2.0f;
	pVertices->v = ( (FLOAT)size.cy / (FLOAT)sizePitch.cy ) * 1.0f;
	pVertices++;
	
	pVertices->vec.x = right;
	pVertices->vec.y = bottom - 0.5f;
	pVertices->u = ( (FLOAT)size.cx / (FLOAT)sizePitch.cx ) * 3.0f;
	pVertices->v = ( (FLOAT)size.cy / (FLOAT)sizePitch.cy ) * 1.0f;
	pVertices++;

	pVB->Unlock(); 

	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

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

void CTheme::SetVersion(int Lang)
{
	FLSPrintf( g_szVersion, _countof( g_szVersion ), "%s %s %d", __DATE__, __TIME__, Lang );
}

CD3DFont* CTheme::GetFont( const CString& strFontName )
{
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; 
	CD3DFont* pFont = NULL;
	while(pos)
	{
		m_mapFont.GetNextAssoc( pos, strFont, (void*&)pFont );
		if( strFont == strFontName )
			return pFont;
	}	

	return NULL;
}
