#include "stdafx.h"
#include "Material.h" 
#include "DialogMsg.h"
#include "party.h"
#include "../_Common/ModelObject.h" 
#include "../_Common/HwOption.h"
#include "../_Common/ParticleMng.h"
#include "TailEffectMng.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "guild.h"
#include "guildwar.h"
#include "webbox.h"
#include "NPGameLib.h"
#include "ClientMsg.h"
#include "WndBase.h"
#include "WndGuideSystem.h"
#include "WorldMap.h"
#include <crypto/FLMD5.h>
#include "GuildHouse.h"
#include "HelperWebBox.h"

#include <GdiPlus.h>
#pragma comment( lib, "gdiplus" )

unsigned int __stdcall LoadProperty( void*  );
//XTrap
#ifdef _USE_XTRAP
#include "xtrap/20110503_522_Xtrap_C_Interface.h"
#endif

extern CGuildWarMng	g_GuildWarMng;
extern CGuildMng		g_GuildMng;
extern CDPClient		g_DPlay;	 
extern CDPLoginClient	g_dpLoginClient;
extern CDPCertified	g_dpCertified;
extern CSfxTexture      g_SfxTex;
extern CSfxMng          g_SfxMng;
extern CSfxObjMng       g_SfxObjMng;
extern CSfxMeshMng      g_SfxMeshMng;
extern HANDLE			g_hMutex;

CDamageNumMng			g_DamageNumMng;
CModelObject			g_Shadow;

//////////////////////////////////////////////////////////////////////////////
// global enum
//////////////////////////////////////////////////////////////////////////////

enum 
{
	NOTIFY_STATUS_NONE,
	NOTIFY_STATUS_45MIN,
	NOTIFY_STATUS_50MIN,
	NOTIFY_STATUS_55MIN,
	NOTIFY_STATUS_56MIN,
	NOTIFY_STATUS_57MIN,
	NOTIFY_STATUS_58MIN,
	NOTIFY_STATUS_59MIN,
};

const FLOAT CNeuzApp::GAME_GRADE_MARK_FADING_SPEED( 800.0F );

//////////////////////////////////////////////////////////////////////////////
// global function
//////////////////////////////////////////////////////////////////////////////
void SetMouseCursor( DWORD dwCursor )
{
	g_Neuz.SetDeviceCursor( g_Neuz.m_ahCursor[ dwCursor ] );
}

//////////////////////////////////////////////////////////////////////////////
// CNeuzApp 
//////////////////////////////////////////////////////////////////////////////
CNeuzApp::CNeuzApp()
{
	m_dwTimeOutDis = 0xffffffff;
	m_dwTimeReSkillPoint = 0xffffffff;
	m_bStartFullscreen = false;
	
    m_d3dEnumeration.AppUsesDepthBuffer = true;
    m_d3dEnumeration.AppMinDepthBits = 16;
    m_d3dEnumeration.AppUsesMixedVP = true;

	m_szSessionPwd[0]	= '\0';

	m_dwCreationWidth  = FULLSCREEN_WIDTH;
	m_dwCreationHeight = FULLSCREEN_HEIGHT - GetSystemMetrics( SM_CYMENU );

	m_strWindowTitle  =  R"(FLYFF)";
	g_pD3dApp = this;

	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bFrameSkip = TRUE;
	m_dwTempMessage = 0;

	memset( m_lpCacheAddr, 0, sizeof(m_lpCacheAddr) );
	memset( &m_loginSI, 0x00, sizeof(m_loginSI) );

	m_szAccount   [ 0 ] = 0; // 어카운트
	m_szPassword  [ 0 ] = 0; // password
	m_bGPotatoAuth	= false;
	*m_szGPotatoNo	= '\0';
	*m_szCheck	= '\0';
	m_szCaptureFileName[ 0 ] = 0;
	m_bCapture = FALSE;
	m_nCaptureCnt = 0;
	g_pBipedMesh = NULL;
	m_dwSys	= 0;
	m_bQuit = FALSE;

	m_uIdofMulti	= NULL_ID;
	m_b18Server     = FALSE;
	m_dwAuthKey		= 0;
	m_cbAccountFlag	= 0X00;

	m_dwTimeLeft = 0;
	m_dwCurTick	 = 0;		
	m_nLeftTimeNotifyStatus = NOTIFY_STATUS_NONE;
	
	m_bActiveNeuz = TRUE;
	
	m_dwSummonPartyData = 0;
	m_dwSummonPartyObjid = 0;
	ZeroMemory( m_szSummonPartyWorldName, sizeof( m_szSummonPartyWorldName ) );
	
//sun: 12, 몬스터 생성 Neuz, World
	m_vCursorPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_pCreateMonItem = NULL;

	m_szWhisperName[0] = '\0';

	m_bThroughPortal	= FALSE;
	m_idPlayer	= 0;
	m_hThread = INVALID_HANDLE_VALUE;
	memset( m_apPlayer, 0, sizeof(m_apPlayer) );

	memset( m_aSavedInven, 0, sizeof(m_aSavedInven) );	//sun: 11, 개인상점 개선

	for(int i = 0; i < 6; i++) m_pMasterIcon[i] = NULL;
	m_pHeroIcon = NULL;
	m_n2ndPasswordNumber = -1;

	m_dwTimeGameGradeMarkRendering = 0xffffffff;
	m_bGameGradeMarkRenderingSwitch = FALSE;
	m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MINIMUM_ALPHA;
	m_eGameGradeMarkState = FADE_IN;
	m_dwTimeOld = g_tmCurrent;
	m_dwTimeGameGradeMarkShowing = 0xffffffff;

	pOldModalWindow = NULL;

	m_byTotalCharNum = 0;
	m_byTotalCharSelectPage = 0;
	m_byCurrentCharSelectPage = 0;

	m_certifySiteValue			= 0;
}

BOOL CNeuzApp::LoadOption()
{
	g_Option.Load( "neuz.ini" );
	
#ifdef _DEBUG
	g_Option.LoadToolTip( "ToolTip.ini" );
#endif // _DEBUG

	// 화면 사이즈 세팅 
	FULLSCREEN_WIDTH	= g_Option.m_nResWidth;
	FULLSCREEN_HEIGHT	= g_Option.m_nResHeight;

	SetLODDetail( g_Option.m_nViewArea );
	
	m_dwCreationWidth  = FULLSCREEN_WIDTH;
	m_dwCreationHeight = FULLSCREEN_HEIGHT - GetSystemMetrics( SM_CYMENU );

	return TRUE;
}


HRESULT CNeuzApp::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	// 스키닝 코드 
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE

	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )	// 버텍스쉐이더 1.1을 지원 못하는가.
	{
		g_bUsableVS = FALSE;		// 버텍스쉐이더 사용안됨.
		if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )	// hw vp는 사용못함
			return E_FAIL;
	}
    // Check that the device can blend vertices with at least two matrices
    // (Software can always do up to 4 blend matrices)
	if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
	{
        if( pCaps->MaxPointSize <= 1.0f )
            return E_FAIL;
		if( pCaps->MaxVertexBlendMatrices < 2 )
			return E_FAIL;
	}

    // If this is a TnL device, make sure it supports directional lights
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( !(pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS ) )
            return E_FAIL;
    }

	if( pCaps->RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
		return S_OK;

	return S_OK;
}


//
// 디바이스 생성후 단 한번 실행한다.
//

HRESULT CNeuzApp::OneTimeSceneInit()
{
	// 사운드 초기화
	HRESULT hr = g_SoundMng.Initialize( GetSafeHwnd(), DSSCL_PRIORITY, 2, 22050, 16 );
	InitCustomSound( FAILED(hr) ? FALSE : TRUE );

	g_SoundMng.m_nSoundVolume = (int)( (-1) * (1.0f - g_Option.m_fEffectVolume) * 5000 );

	::SetVolume( g_Option.m_fBGMVolume );

	m_ahCursor[ CUR_ATTACK  ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_ATTACK  ) );
	m_ahCursor[ CUR_BASE    ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_BASE    ) );
	m_ahCursor[ CUR_CAMERA  ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_CAMERA  ) );
	m_ahCursor[ CUR_CONTROL ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_CONTROL ) );
	m_ahCursor[ CUR_DELAY   ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_DELAY   ) );
	m_ahCursor[ CUR_EDIT    ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_EDIT    ) );
	m_ahCursor[ CUR_GETITEM ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_GETITEM ) );
	m_ahCursor[ CUR_HELP    ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_CURSOR2    ) );
	m_ahCursor[ CUR_NO      ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_CURSOR3      ) );
	m_ahCursor[ CUR_NPC     ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_NPC     ) );
	m_ahCursor[ CUR_SELECT  ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_SELECT  ) );
	m_ahCursor[ CUR_RESIZE_HORI ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_RESIZE_HORI ) );
	m_ahCursor[ CUR_RESIZE_VERT ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_RESIZE_VERT ) );
	m_ahCursor[ CUR_RESIZE_HV1   ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_RESIZE_HV1   ) );
	m_ahCursor[ CUR_RESIZE_HV2   ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_RESIZE_HV2   ) );
	m_ahCursor[ CUR_DOOR ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_DOOR ) );
	m_ahCursor[ CUR_GATE ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_GATE ) );
	m_ahCursor[ CUR_LEVER   ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_LEVER   ) );
	m_ahCursor[ CUR_HAMMER   ] = LoadCursor( m_hInstance, MAKEINTRESOURCE( IDC_HAMMER   ) );

	CWndBase::m_hDefaultCursor = m_ahCursor[ CUR_BASE ];
	CWndEdit::m_hEditCursor = m_ahCursor[ CUR_EDIT ];
	
	return S_OK;
}

void	CNeuzApp::InitSecurity()
{
#ifdef _USE_XTRAP
	InitXTrap();
#else // _USE_XTRAP
#ifdef __Gameguard
	InitNProtect();
#endif
#endif // _USE_XTRAP
}

#ifdef __Gameguard
void CNeuzApp::InitNProtect()
{
	CScript s1;
	if( s1.Load(MakePath( DIR_CLIENT, "GameGuard.txt.txt" ) ) )
		s1.LoadString();
	
	CScript s2;
	if( s2.Load(MakePath( DIR_CLIENT, "GameGuard.inc" ) ) )
	{
		s2.GetToken();
		while( s2.tok != FINISHED )
		{
			m_strArray.Add( s2.Token );
			s2.GetToken();
		}
	}

	DWORD dwResult = GetNProtect()->Init();
	if( dwResult != NPGAMEMON_SUCCESS )
    {
		TCHAR msg[256];
		LPCSTR lpszMsg;
		
		switch (dwResult)
		{
		case NPGAMEMON_ERROR_EXIST:
			lpszMsg	= m_strArray.GetAt(0);//"게임가드가 이미 실행 중 입니다. 재부팅 후 다시 실행해보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_GAME_EXIST:
			lpszMsg	= m_strArray.GetAt(1);//"게임이 중복 실행되었거나 게임가드가 이미 실행 중 입니다. 게임 종료 후 다시 실행해보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_INIT:
			lpszMsg	= m_strArray.GetAt(2);//"게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수 있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
		case NPGAMEMON_ERROR_NFOUND_INI:
			lpszMsg	= m_strArray.GetAt(3);//"게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_CRYPTOAPI:
			lpszMsg	= m_strArray.GetAt(4);//"윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를 다시 설치해보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_EXECUTE:
			lpszMsg	= m_strArray.GetAt(5);//"게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치해보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			lpszMsg	= m_strArray.GetAt(6);//"불법 프로그램이 발견되었습니다. 불필요한 프로그램을 종료한 후 다시 실행해보시기 바랍니다.";
			break;
		case NPGMUP_ERROR_ABORT:
			lpszMsg	= m_strArray.GetAt(7);//"게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 방화벽 상태를 점검해보시기 바랍니다.";
			break;
		case NPGMUP_ERROR_CONNECT:
		case NPGMUP_ERROR_DOWNCFG:
			lpszMsg	= m_strArray.GetAt(8);//"게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 재시도 해보거나, 인터넷 및 방화벽 상태를 점검해 보시기 바랍니다.";
			break;
		case NPGMUP_ERROR_AUTH:
			lpszMsg	= m_strArray.GetAt(9);//"게임가드 업데이트를 완료하지 못 했습니다. 바이러스 검사를 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정한 후 게임을 실행해 보시기 바랍니다.";
			break;
		case NPGAMEMON_ERROR_NPSCAN:
		default:
			// 적절한 종료 메시지 출력, UI 초기화 후라면 MessageBox 대신 게임 UI를 사용해도 됨
			//wsprintf( msg, "게임가드 실행 에러 : %lu", dwResult );
			FLSPrintf( msg, _countof( msg ), m_strArray.GetAt(10), dwResult );
			lpszMsg = msg;
			break;
		}
		if( lpszMsg )
			FLERROR_LOG( PROGRAM_NAME, _T( "nProtect GameGuard : %s" ), lpszMsg );
//			MessageBox( GetSafeHwnd(), lpszMsg, "nProtect GameGuard", MB_OK );

		//ExitProcess( -1 );
		::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );

	}

	GetNProtect()->SetHwnd( GetSafeHwnd() );
}
#endif


///////////////////////////////////////////////////////
HRESULT CNeuzApp::RestoreDeviceObjects()
{
#ifdef __YENV
	if( m_pEffect != NULL )
		m_pEffect->OnResetDevice();

	
	if( FAILED( LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, "Env.dds" ), &m_pEnvTex ) ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Env.dds 텍스쳐 못읽음" ) );
	}
#endif //__YENV

	FLERROR_LOG( PROGRAM_NAME, _T( "RestoreDeviceObjects BEGIN" ) );
	
	
    if( ( m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) || ( m_dwCreateFlags & D3DCREATE_MIXED_VERTEXPROCESSING ) ||
        m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1) )
    {
		CreateSkinningVS( m_pd3dDevice, _T("skining2.vsh") );
		FLERROR_LOG( PROGRAM_NAME, _T( "CreateSkinningVS OK" ) );
    }
	
	if( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS )	// d3d9.0에 새로 추가된 기능인데 이걸 지원못하는 카드가 있다. 그래서 이렇게 검사한다.
		g_ModelGlobal.m_bDepthBias = TRUE;
	if( g_Option.m_nShadow < 2 && m_d3dCaps.MaxSimultaneousTextures <= 2 )		// 실시간그림자를 세팅하고 하드웨어가 딸리면
	{
		g_Option.m_nShadow = 2;		// 실시간 그림자 기능을 끈다.
		FLERROR_LOG( PROGRAM_NAME, _T( "g_Option.m_nShadow = 2" ) );
		MessageBox( GetSafeHwnd(), m_strArray.GetAt(13), m_strArray.GetAt(11), MB_OK );
		//MessageBox( GetSafeHwnd(), "이 그래픽카드에선 그림자 기능을 사용할 수 없습니다. 그림자 기능을 낮음으로 바꿉니다.", "안내", MB_OK );
		
	}
	
	UINT nMem = m_pd3dDevice->GetAvailableTextureMem();
	if( g_Option.m_nTextureQuality == 0 && nMem <= (1024 * 1024 * 64) )
	{
		g_Option.m_nTextureQuality = 1;		// 64메가 이하의 텍스쳐메모리에선 텍스쳐품질을 낮춘다.
		FLERROR_LOG( PROGRAM_NAME, _T( "g_Option.m_nTextureQuality = 1" ) );
		MessageBox( GetSafeHwnd(), m_strArray.GetAt(15), m_strArray.GetAt(14), MB_OK );
		//MessageBox( GetSafeHwnd(), "사용가능한 텍스쳐 메모리가 부족해서 텍스쳐 품질을 낮춥니다.", "경고", MB_OK );
	}
	if( g_Option.m_nShadow < 2 )
	{
		if( nMem <= (1024 * 1024 * 64) )
		{
			g_Option.m_nShadow = 2;		// 64메가 이하의 텍스쳐메모리에선 그림자기능을 사용하지 못한다.
			FLERROR_LOG( PROGRAM_NAME, _T( "g_Option.m_nShadow = 2" ) );
			MessageBox( GetSafeHwnd(), m_strArray.GetAt(16), m_strArray.GetAt(14), MB_OK );
			//MessageBox( GetSafeHwnd(), "사용가능한 텍스쳐 메모리가 부족해서 그림자 기능을 해제합니다.", "경고", MB_OK  );
		} else
		{
//			CreateShadowMap( m_pd3dDevice, m_d3dpp.BackBufferFormat );
			BOOL bSuccess = CreateShadowMap( m_pd3dDevice, D3DFMT_R5G6B5 );
			if( bSuccess == FALSE )
			{	
				FLERROR_LOG( PROGRAM_NAME, _T( "CreateShadowMap FAILED" ) );

				// 실패하면 비됴메모리 부족이라 판단하고 옵션 강제로 낮춤.
				if( g_Option.m_nShadow < 2 )
					g_Option.m_nShadow ++;
				if( g_Option.m_nTextureQuality < 2 )
					g_Option.m_nTextureQuality ++;
			}
		}
	}

	if( g_Option.m_nBloom == 1 )
	{
		if( m_d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1,1) )
		{
			g_Glare.Create( m_pd3dDevice, D3DFMT_R5G6B5, g_Option.m_nResWidth, g_Option.m_nResHeight - 48 );
			FLERROR_LOG( PROGRAM_NAME, _T( "g_Glare.Create OK" ) );
		}
		else
			g_Option.m_nBloom = 0;
	}

	g_ParticleMng.RestoreDeviceObjects( m_pd3dDevice );
	g_TailEffectMng.RestoreDeviceObjects( m_pd3dDevice );

    // Initialize the particle system
//	HRESULT hr;

	extern LPDIRECT3DTEXTURE9 g_pReflectMap;
	if( FAILED( LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, "etc_reflect.tga" ), &g_pReflectMap ) ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "etc_Reflect.tga 텍스쳐 못읽음" ) );
	}

	g_ModelGlobal.RestoreDeviceObjects( m_pd3dDevice );

	m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE,   D3DSHADE_GOURAUD ); 
	CWndBase::m_Theme.RestoreDeviceObjects();

	g_WndMng.RestoreDeviceObjects();
	if( m_pd3dDevice )
	{
		m_2DRender.RestoreDeviceObjects(&m_d3dsdBackBuffer);
		if( g_WorldMng.Get() )
			g_WorldMng.Get()->RestoreDeviceObjects( m_pd3dDevice );
		CWorld::StaticRestoreDeviceObjects( m_pd3dDevice );
		prj.m_modelMng.RestoreDeviceObjects(m_pd3dDevice);
		prj.m_terrainMng.RestoreDeviceObjects();
	}
	if( g_pBipedMesh )
		g_pBipedMesh->RestoreDeviceObjects();	

	g_SfxMng.RestoreDeviceObjects();
	g_SfxMeshMng.RestoreDeviceObjects();
	
	g_DamageNumMng.RestoreDeviceObjects();
	g_DialogMsg.RestoreDeviceObjects();
	
	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->RestoreDeviceObjects(); 

	FLERROR_LOG( PROGRAM_NAME, _T( "RestoreDeviceObjects END" ) );

	return S_OK;
}

///////////////////////////////////////////////////
HRESULT CNeuzApp::InvalidateDeviceObjects()
{
#ifdef __YENV
	if( m_pEffect != NULL )
		m_pEffect->OnLostDevice();

	SAFE_RELEASE(m_pEnvTex);	

#endif //__YENV	

	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->InvalidateDeviceObjects(); 

	g_Glare.DeleteDeviceObjects();
	DeleteShadowMap( m_pd3dDevice );
	DeleteVertexShader( m_pd3dDevice );
	CWndBase::m_Theme.InvalidateDeviceObjects();
	if( g_WorldMng.Get() )
		g_WorldMng.Get()->InvalidateDeviceObjects();
	CWorld::StaticInvalidateDeviceObjects();
	g_WndMng.InvalidateDeviceObjects();
	prj.m_modelMng.InvalidateDeviceObjects();
	prj.m_terrainMng.InvalidateDeviceObjects();
	m_2DRender.InvalidateDeviceObjects();
	if( g_pBipedMesh )
		g_pBipedMesh->InvalidateDeviceObjects();

	g_SfxMng.InvalidateDeviceObjects();
	g_SfxMeshMng.InvalidateDeviceObjects();
	
	extern LPDIRECT3DTEXTURE9 g_pReflectMap;
	SAFE_RELEASE( g_pReflectMap	);

	g_ParticleMng.InvalidateDeviceObjects();
	g_TailEffectMng.InvalidateDeviceObjects();

	g_ModelGlobal.InvalidateDeviceObjects( m_pd3dDevice );

	g_DamageNumMng.InvalidateDeviceObjects();
	g_DialogMsg.InvalidateDeviceObjects();
	
	return S_OK;
}


///////////////////////////////////////////////////////
HRESULT CNeuzApp::DeleteDeviceObjects()
{
#ifdef __YENV
	SAFE_RELEASE(m_pEffect);
#endif //__YENV

	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->DeleteDeviceObjects(); 
	g_toolTip.Delete();
	g_toolTipSub1.Delete();
	g_toolTipSub2.Delete();
	g_Glare.DeleteDeviceObjects();
	CWndBase::m_Theme.DeleteDeviceObjects();
	g_WndMng.DestroyApplet();
	g_WndMng.Free();

	if( g_WorldMng.Get() )
		g_WorldMng.Get()->DeleteDeviceObjects();

	g_WorldMng.Free();
	CWorld::StaticDeleteDeviceObjects();
	g_WndMng.DeleteDeviceObjects();
	prj.m_modelMng.DeleteDeviceObjects();
	prj.m_terrainMng.DeleteDeviceObjects();
	m_2DRender.DeleteDeviceObjects();
	g_DamageNumMng.DeleteDeviceObjects();
	g_DialogMsg.DeleteDeviceObjects();
	if( g_pBipedMesh )
		g_pBipedMesh->DeleteDeviceObjects();

	g_TextureMng.DeleteDeviceObjects();
 
	SAFE_DELETE( g_pBipedMesh );		// g_Object3DMng가 파괴되기전에 부를것
	g_Shadow.DeleteDeviceObjects();
 
	// 여기서 이것을 따로 불러준 이유는 게임종료시에 Release되지 않은 수를 판정할때 
	// g_Object3DMng의 파괴자가 늦게 발동되어 누락되기 때문이다.  그래서 여기서 먼저 파괴시켜준다.  결과는 같다.
	// Destroy를 다른데서 호출하지 말것!
	g_Object3DMng.DeleteDeviceObjects();

	g_SfxMng.DeleteDeviceObjects();
	g_SfxTex.DeleteAll();
	g_SfxObjMng.RemoveAll();
	g_SfxMeshMng.DeleteAll();

	g_Object3DMng.Destroy();	
	CWndNeuz::FreeTileTexture();

	//sun: 11, 채집 시스템
	m_TexCltGauge[0].DeleteDeviceObjects();
	m_TexCltGauge[1].DeleteDeviceObjects();
	
	m_TextureGauge[0].DeleteDeviceObjects();
	m_TextureGauge[1].DeleteDeviceObjects();
	m_TextureGauge[2].DeleteDeviceObjects();
	m_TextureGauge[3].DeleteDeviceObjects();
	m_TextureGauge[4].DeleteDeviceObjects();

	m_TextureHPGauge[0].DeleteDeviceObjects();
	m_TextureHPGauge[1].DeleteDeviceObjects();
	m_TextureHPGauge[2].DeleteDeviceObjects();

	m_TextureTurboGauge[0].DeleteDeviceObjects();
	m_TextureTurboGauge[1].DeleteDeviceObjects();
	m_TextureCastingGauge[0].DeleteDeviceObjects();
	m_TextureCastingGauge[1].DeleteDeviceObjects();	

	//sun: 8차 엔젤 소환 Neuz, World, Trans
	m_TextureAngelGauge[0].DeleteDeviceObjects();
	m_TextureAngelGauge[1].DeleteDeviceObjects();

	m_TexturePackPVP.DeleteDeviceObjects();
	m_TexLoading.DeleteDeviceObjects();
	m_texQuestEmoticon.DeleteDeviceObjects();
	return S_OK;
}
// 
// 종료하기 전 객체를 파괴하는 곳
//
HRESULT CNeuzApp::FinalCleanup()
{
#ifdef _DEBUG
	WaitLoading();
#endif

	if( g_hMutex )
		::CloseHandle( g_hMutex );
	
	// 다시 알트탭 되게
	::SystemParametersInfo (SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);		// 95, 98, Me면 API를 쓴다.

	//sun: 밝기, 감마, 명암 조절 Neuz
	Drv_SetGamma(GetSafeHwnd(), 1.0f, 10, 1.0f);
	
	for( int i = 0; i < MAX_CHARACTER_SLOT; i++ )
	{
		if( m_apPlayer[i] )
			m_apPlayer[i]->m_pModel = NULL; 
		SAFE_DELETE( m_apPlayer[i] );
	}
	g_dpCertified.DeleteDPObject();
	g_dpLoginClient.DeleteDPObject();
	g_DPlay.DeleteDPObject();
	g_GuildWarMng.Clear();
	g_GuildMng.Clear();

	MEMPOOLER_DESTROY( CParty );
	MEMPOOLER_DESTROY( CChatting );
	MEMPOOLER_DESTROY( CGuildMember );
	MEMPOOLER_DESTROY( CGuild );
	MEMPOOLER_DESTROY( CMover );
	MEMPOOLER_DESTROY( CActionMover );
	MEMPOOLER_DESTROY( CItem );
	MEMPOOLER_DESTROY( CDamageNum );
	MEMPOOLER_DESTROY( CSfxGenMoveMark );
	MEMPOOLER_DESTROY( CSfxGenWaterCircle );
	MEMPOOLER_DESTROY( CSfxGenNormalDmg01 );
	MEMPOOLER_DESTROY( CSfxModel );
	MEMPOOLER_DESTROY( CSfxGenRainCircle );

	// 군주 협력 객체의 제거
	CCLord::Instance()->DestroyColleagues();		//sun: 12, 군주

	UninitializeNetLib();

	CWndNeuz::FreeTileTexture();
	CResFile::FreeResource();
	UnInitCustomSound();

	g_Option.Save( "neuz.ini" );
 	return S_OK;
}

#ifdef __WRITEAVI

#include "..\_multimedia\writeAvi.h"
#pragma comment( lib, "vfw32.lib" ) 
CAVIFile avi( _T( "Capture\\test.avi" ) );

HRESULT CNeuzApp::CaptureBitmap()
{
	IDirect3DSurface8* pDestSurface;
	m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	RECT  rcSource[] = {  0,  0,  1024,  768 };
	POINT ptDest[]   = {  0,  0 };

	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );
	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport( &viewport );
	//LPBYTE lpData = new BYTE[ viewport.Width * viewport.Height * 3 ];D3DLOCK_DISCARD 

	if( pDestSurface->LockRect( &lockedRect, rectIn, D3DLOCK_READONLY  ) == D3D_OK )// Lock
	{
	//	LPDWORD pWord = (LPDWORD)lockedRect.pBits;
	//	memcpy( bmp.GetData(), pWord, viewport.Width * viewport.Height * 4 );

		//
		//LPWORD pWord = (LPWORD)lockedRect.pBits;

		//for(int y = 0; y < viewport.Height; y++ )
		//{
		//	for(int x = 0; x < viewport.Width; x++)
		//	{
		///		WORD wData = pWord[ y * viewport.Width + x ]; 
		//		// 16 bit color
		//		BYTE byData1 = ( wData & 0xf800 ) >> 8;
		//		BYTE byData2 = ( wData & 0x07e0 ) >> 3;
		//		BYTE byData3 = ( wData & 0x001f ) << 3 ;
		//		DWORD dwOffset = ( y * viewport.Width + x ) * 3;
		//		lpData[ dwOffset + 0 ] = byData3;
		//		lpData[ dwOffset + 1 ] = byData2;
		//		lpData[ dwOffset + 2 ] = byData1;
		//	}
		//}
		//
		


			LPDWORD pWord = (LPDWORD)lockedRect.pBits;
			memcpy( g_bmp.GetData(), pWord, viewport.Width * viewport.Height * 4 );

		pDestSurface->UnlockRect();

	}
	pDestSurface->Release();
	avi.AddFrame( g_bmp );
	//SAFE_DELETE( lpData );
	return S_OK;
}
#endif
/*
	IDirect3DSurface8* pDestSurface;
	m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface );
	D3DLOCKED_RECT lockedRect;
	CRect rectIn ( 0, 0, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height );//cb.bih.biWidth, abs(cb.bih.biHeight) );

	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport( &viewport );
	LPBYTE lpData = new BYTE[ viewport.Width * viewport.Height * 3 ];
	ZeroMemory( lpData, viewport.Width * viewport.Height * 3 );
	if( pDestSurface->LockRect( &lockedRect, rectIn, 0 ) == D3D_OK )// Lock
	{
		
		LPWORD pWord = (LPWORD)lockedRect.pBits;
		for(int y = 0; y < viewport.Height; y++ )
		{
			for(int x = 0; x < viewport.Width; x++)
			{
				WORD wData = pWord[ y * viewport.Width + x ]; 
				// 16 bit color
				BYTE byData1 = ( wData & 0xf800 ) >> 8;
				BYTE byData2 = ( wData & 0x07e0 ) >> 3;
				BYTE byData3 = ( wData & 0x001f ) << 3 ;
				DWORD dwOffset = ( y * viewport.Width + x ) * 3;
				lpData[ dwOffset + 0 ] = byData3;
				lpData[ dwOffset + 1 ] = byData2;
				lpData[ dwOffset + 2 ] = byData1;
			}
		}
		pDestSurface->UnlockRect();

		TCHAR szFileName[ 128 ];
		_stprintf( szFileName, _T( "Capture\\%s%06d.bmp" ), m_szCaptureFileName, m_nCaptureCnt );
//	SaveBMP( szFileName, (LPBYTE)lockedRect.pBits, CSize( viewport.Width, viewport.Height ) );
		delete lpData;
		m_nCaptureCnt++;

	}
	pDestSurface->Release();

	return S_OK;
}
*/

// 
// 화면을 랜더링 한다. (출력)
//
HRESULT CNeuzApp::Render()
{
	if(	m_bActive == FALSE )
		return S_FALSE;
	// Clear the viewport
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, CWndBase::m_Theme.m_d3dcBackground, 1.0f, 0 ) ;
	
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	
	if( pWndWorld )
		pWndWorld ->ClearFlyTarget();		// 비행때 공격가능한 타겟들 리스트 지워줌.	

	//pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 90, 146, 222 ), 1.0f, 0 ) ;
	//m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 255, 255, 255 ), 1.0f, 0 ) ;
	//m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 0 ), 1.0f, 0 ) ;
	// Begin the scene
	
	if( g_Option.m_nShadow < 2 )		// 높음/중간 만 쉐도우 맵을 만든다.
	{
		void RenderShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, CObj **pList, int nMax );
		if( g_pPlayer )
		{
			CWorld *pWorld = g_pPlayer->GetWorld();
			if( pWorld )
			{
//sun: 13, 하우징 시스템
				if( pWorld->GetID() != WI_WORLD_MINIROOM ) // 7.28기획요청 : 하우징 그림자 제거
					RenderShadowMap( m_pd3dDevice, pWorld->m_aobjCull, pWorld->m_nObjCullSize );
			}

		}
	}

	// 뽀샤시 옵션이 켜져있으면 렌더타겟에 게임화면을 렌더함.
	if( g_Option.m_nBloom )
	{
		CWorld* pWorld = g_WorldMng.Get();
		if( pWorld && g_pPlayer && g_Glare.m_bActive )
		{
			pWorld->Projection( m_pd3dDevice, g_Glare.m_Src.m_nWidth, g_Glare.m_Src.m_nHeight );
			pWorld->SetCamera( &m_camera );
			g_Glare.m_Src.BeginScene();
			DWORD dwColor = CWorld::GetDiffuseColor();
			m_2DRender.m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0 ) ;
			pWorld->RenderBase( m_pd3dDevice, g_WndMng.m_Theme.m_pFontWorld );		// 지형과 오브젝트를 미리 렌더링 해둠.
			g_Glare.m_Src.EndScene();

			g_Glare.Blur( m_pd3dDevice );		// m_Src를 m_Surface[0]에 블러함.		
		}
	}

	if(SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		g_nRenderCnt ++;

		CWorldMap* pWorldMap = CWorldMap::GetInstance();
		g_WndMng.PaintRoot(&m_2DRender);		
		
		if( g_WndMng.m_pWndWorld )
			g_WndMng.m_pWndWorld->RenderFocusArrow( CPoint(0,0) );

		// 드래그 아이템 
		if(	CWndBase::m_GlobalShortcut.IsEmpty() == FALSE )
		{
			GET_CLIENT_POINT( GetSafeHwnd(), point );
			if( CWndBase::m_GlobalShortcut.m_pTexture )
				CWndBase::m_GlobalShortcut.m_pTexture->Render( &m_2DRender, CPoint( point.x - 16, point.y - 16 ) );
			else
			{
				int nHeight = m_2DRender.m_pFont->GetMaxHeight() / 2;
				m_2DRender.TextOut( point.x - nHeight , point.y - nHeight, CWndBase::m_GlobalShortcut.m_szString );
			}
		}

		// 툴립 관련
		g_toolTip.Paint(&m_2DRender);
		g_toolTipSub1.Paint( &m_2DRender );
		g_toolTipSub2.Paint( &m_2DRender );
		m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_2DRender.SetFont( 	CWndBase::m_Theme.m_pFontWorld );
		g_ClientMsg.Render( CPoint( m_2DRender.m_clipRect.Width() / 2 , 520 * m_2DRender.m_clipRect.Height() / 768 ), &m_2DRender );
		m_2DRender.SetFont( 	CWndBase::m_Theme.m_pFontText );

		// 맵이동간에 이미지 로딩출력
		if( m_bTexLoad && m_bTexLoadAlpha )
		{
			m_nTexAlpha	= (int)( m_nTexAlpha - 6.0f );

			if( m_nTexAlpha < 0 )
			{
				m_nTexAlpha	   = 0;
				m_bTexLoad     = FALSE;
				m_bTexLoadAlpha  = FALSE;
				
				m_TexLoading.DeleteDeviceObjects();	
				
			}
			else
			{
//sun: Wide및 고해상도 추가
				int wideOffsetX = 0;
				
				if(g_Option.m_nResWidth == 1280 && (g_Option.m_nResHeight == 720 || g_Option.m_nResHeight == 768 || g_Option.m_nResHeight == 800)) //Wide Offset
				{
					m_TexLoading.m_size = CSize(960, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 960) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else if(g_Option.m_nResWidth == 1360 && g_Option.m_nResHeight == 768)
				{
					m_TexLoading.m_size = CSize(1024, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 1024) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else if(g_Option.m_nResWidth == 1440 && g_Option.m_nResHeight == 900)
				{
					m_TexLoading.m_size = CSize(1200, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 1200) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else if(g_Option.m_nResWidth == 1680 && g_Option.m_nResHeight == 1050)
				{
					m_TexLoading.m_size = CSize(1400, FULLSCREEN_HEIGHT);
					wideOffsetX = (g_Option.m_nResWidth - 1400) / 2;
					m_2DRender.RenderFillRect( CRect( 0, 0, g_Option.m_nResWidth, g_Option.m_nResHeight ), 0xff000000 );
				}
				else
					m_TexLoading.m_size = CSize(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
				
				m_2DRender.RenderTexture( CPoint(0 + wideOffsetX, 0), &m_TexLoading, m_nTexAlpha );		

			}			
		}

		pWorldMap->RenderWorldMap( &m_2DRender );

		if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
		{
			g_WndMng.m_pTheme->RenderGameGradeMark( &m_2DRender, static_cast< DWORD >( m_nGameGradeMarkAlpha ) );
		}
		
		m_pd3dDevice->EndScene();
	}
	
	return S_OK;
}

// 플레이 가능 시간 세팅 
void CNeuzApp::SetLeftTime( long lTimeLeft )
{
	m_dwTimeLeft = lTimeLeft * 1000;
	m_dwCurTick = timeGetTime();
}

void CNeuzApp::NotifyLeftMinute( UINT type, int nMin )
{
	if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
	{
		if( type == m_nLeftTimeNotifyStatus )
			return;

		m_nLeftTimeNotifyStatus = type;

		char szBuffer[256];
		FLSPrintf( szBuffer, _countof( szBuffer ), prj.GetText(TID_GAME_REMAINPLAYTIMEMIN), nMin );
		g_WndMng.OpenMessageBox( szBuffer );
	}
}

// 태국의 경우 플레이 가능 시간을 표시해 준다.
void CNeuzApp::NotifyLeftTime()
{
	if( g_xFlyffConfig->GetMainLanguage() != LANG_THA )
		return; 

	if( m_dwTimeLeft == 0 )
		return;

	DWORD dw = timeGetTime() - m_dwCurTick;
	if( dw > 1000 ) 
	{
		m_dwCurTick = timeGetTime();

		if( m_dwTimeLeft >= dw )
			m_dwTimeLeft -= dw;
		else
		{
			m_dwTimeLeft = 0;								// 0초는 표시하지 않는다.
			return;
		}

		long lLeftTime = ( m_dwTimeLeft + 500 ) / 1000;		// + 500반올림 
		if( lLeftTime <= 10 )			// 10초 이하 
		{
			char szBuffer[256];
			FLSPrintf( szBuffer, _countof( szBuffer ), prj.GetText(TID_GAME_REMAINPLAYTIMESEC), lLeftTime );			
			g_WndMng.PutString( szBuffer );
		}
		else if( lLeftTime <= 60 * 15 )	// 15분 이하 
		{
			switch( lLeftTime )
			{
			case 60 * 1:
				NotifyLeftMinute( NOTIFY_STATUS_59MIN, 1 );
				break;
			case 60 * 2:
				NotifyLeftMinute( NOTIFY_STATUS_58MIN, 2 );
				break;
			case 60 * 3:
				NotifyLeftMinute( NOTIFY_STATUS_57MIN, 3 );
				break;
			case 60 * 4:
				NotifyLeftMinute( NOTIFY_STATUS_56MIN, 4 );
				break;
			case 60 * 5:
				NotifyLeftMinute( NOTIFY_STATUS_55MIN, 5 );
				break;
			case 60 * 10:
				NotifyLeftMinute( NOTIFY_STATUS_50MIN, 10 );
				break;
			case 60 * 15:
				NotifyLeftMinute( NOTIFY_STATUS_45MIN, 15 );
				break;
			}
		}
	}
}

extern int g_nOldVirtKey;

//
// Frame을 움직인다. 기존의 Process와 같은 역할 
//

//for window mode dragging
BOOL g_bForWindowDragging = FALSE;
POINT g_ptForWindowDragging;

HRESULT CNeuzApp::FrameMove()
{
	if( g_bForWindowDragging )
	{
		//gmpbigsun( 20110419 ) : Reposition client on window mode ( 멈춤없이... )

		//순간적으로 마우스를 이동하고 버튼을 때게 되면 UP메세지가 안온다. 해서 추가됨.
		const int logicalLeftButton = ::GetSystemMetrics( SM_SWAPBUTTON ) ? VK_RBUTTON : VK_LBUTTON;
		if( false == ::GetAsyncKeyState( logicalLeftButton ) )
		{
			g_bForWindowDragging = FALSE;
		}
		else
		{
			POINT pt;
			::GetCursorPos( &pt );

			int nOffset_X = pt.x - g_ptForWindowDragging.x;
			int nOffset_Y = pt.y - g_ptForWindowDragging.y;

			if( nOffset_X != 0 || nOffset_Y != 0 )
			{
				g_ptForWindowDragging = pt;

				RECT rect;
				::GetWindowRect( m_hWnd, &rect );
				::SetWindowPos( m_hWnd, NULL, rect.left + nOffset_X, rect.top + nOffset_Y, rect.right - rect.left, rect.bottom - rect.top, 0 );
			}
		}
	}

	g_tmCurrent = timeGetTime();		// 전역으로 사용할 현재 시간.
	g_tCurrent	= time( NULL );
	g_nProcessCnt ++;
	if( g_GameTimer.m_nHour >= 21 || g_GameTimer.m_nHour <= 6 )
		::SetNight( TRUE );
	else
		::SetNight( FALSE );

	NotifyLeftTime();

	_UpdateGTM( );	//gmpbigsun: 전역 틱관리자 

#ifdef _USE_XTRAP
	static DWORD sdwTimeForXTrap = 0;
	sdwTimeForXTrap += _GetDeltaTime( );
	if( sdwTimeForXTrap > 5000 )
	{
		sdwTimeForXTrap = 0;
		XTrap_C_CallbackAlive( 5000 ); 
	}
#endif //_USE_XTRAP

	g_DPlay.ReceiveMessage();
	g_DPlay.SendSnapshot();

	g_DPlay.FlushPlayerAngle();

	g_dpLoginClient.ReceiveMessage();
	g_dpCertified.ReceiveMessage();

	if( m_dwTempMessage )
	{
		CWndBase* pWndBase;
		if( m_timerConnect.TimeOut() )
		{
			m_dwTempMessage = 0;
			pWndBase = g_WndMng.GetWndBase( APP_SELECT_CHAR );
			if( pWndBase )
			{
				((CWndSelectChar*)pWndBase)->Connected();
				goto NEXT;
			}
		}
	}

	if( g_pBipedMesh )
		g_pBipedMesh->FrameMove();

	g_WndMng.Process(); // 실제적인 월드와 캐릭터 컨트롤 인터페이스는 CWndWorld::Process가 처리함 
	
	// 위치 바꾸지 마시오!! ata3k
	// 키보드 업했을때 반응하기 위해 클리어 시켜준다....
	if( g_nOldVirtKey != 0 )
		g_bKeyTable[ g_nOldVirtKey ] = FALSE;	

NEXT:
	if( g_WndMng.IsTitle() == TRUE )
		CWndBase::m_Theme.FrameMove();
	GET_CLIENT_POINT( GetSafeHwnd(), point );
	g_toolTip.Process( point, &m_2DRender );
	g_toolTipSub1.Process( point, &m_2DRender );
	g_toolTipSub2.Process( point, &m_2DRender );
	g_DamageNumMng.Process();
#ifdef __FLYFF_INITPAGE_EXT
	if(CWndBase::m_Theme.m_bRenderTitleWorld)
	{
		g_GameTimer.m_bFixed = TRUE;
		g_GameTimer.m_nHour = 10;
	}
	else
		g_GameTimer.Compute();
#else //__FLYFF_INITPAGE_EXT
	g_GameTimer.Compute();
#endif //__FLYFF_INITPAGE_EXT
	ProcessFadeMusic();
	CalcUPS();
	g_Object3DMng.Process();

	g_ParticleMng.Process();
	g_TailEffectMng.Process();

	if( g_WndMng.GetWndBase( APP_WEBBOX ) )
	{
		if( CWebBox::GetInstance()->Get_Start() == false )
			CWebBox::GetInstance()->Set_Start();

		char szPotato[42];
		if ( g_xFlyffConfig->GetMainLanguage() == LANG_USA )
		{
			if ( strcmp ( m_szGPotatoNo2, "" ) )	strcpy ( szPotato, m_szGPotatoNo2 );
			else									strcpy ( szPotato, m_szGPotatoNo );
		}
		else													strcpy ( szPotato, m_szGPotatoNo );

		CWebBox::GetInstance()->Process( GetSafeHwnd(), m_hInstance
			, m_bGPotatoAuth? szPotato: m_szAccount
			, CMover::GetActiveMover()? CMover::GetActiveMover()->m_idPlayer: 0
			, m_dwSys
			, CMover::GetActiveMover()? (int)CMover::GetActiveMover()->GetLevel(): 0
			, CMover::GetActiveMover()? (int)CMover::GetActiveMover()->GetJob(): 0
			, CMover::GetActiveMover()? (int)CMover::GetActiveMover()->GetSex(): 0
			, CMover::GetActiveMover()? CMover::GetActiveMover()->GetName(): ""
			);
	}
	
	CWorldMap* pWorldMap = CWorldMap::GetInstance();
	pWorldMap->Process();

	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )
	{
		CalculateGameGradeMarkAlpha();
	}
	
	if( m_bQuit )
	{
		if( g_pPlayer == NULL || m_timerQuit.IsTimeOut() )
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0 );
	}
	return S_OK;
}

BOOL CALLBACK NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	if( g_Neuz.m_strArray.GetSize() == 0 )
		return false;

	bool bAppExit	= false;

	TCHAR msg[128];
	LPCTSTR lpszMsg	= NULL;

	switch (dwMsg) 
	{
		case NPGAMEMON_CHECK_CSAUTH3:	// 서버로 인증 패킷을 전송
			{
				PCSAuth3Data pCSAuth3 = (PCSAuth3Data)dwArg;
				if( pCSAuth3->dwPacketSize <= 4096 ) {
					BEFORESENDSOLE( ar, PACKETTYPE_AUTHQUERY_VER30, DPID_UNKNOWN );
					ar << pCSAuth3->dwPacketSize;
					ar.Write( (void*)pCSAuth3->bPacket, pCSAuth3->dwPacketSize );
					SEND( ar, &g_dpLoginClient, DPID_SERVERPLAYER );

// 					GG_AUTH_DATA AuthData;
// 					memcpy( &AuthData, (PVOID)dwArg, sizeof(GG_AUTH_DATA) );
// 					
// 					BEFORESENDSOLE( ar, PACKETTYPE_AUTHQUERY, DPID_UNKNOWN );
// 					ar.Write( (void*)&AuthData, sizeof(GG_AUTH_DATA) );
// 					SEND( ar, &g_dpLoginClient, DPID_SERVERPLAYER );

					if( g_Neuz.m_loginSI.nCount == 1 )
					{
						++g_Neuz.m_loginSI.nCount;
						g_dpLoginClient.SendGetPlayerList( g_Neuz.m_loginSI.dwID, g_Neuz.m_loginSI.pszAccount, g_Neuz.m_loginSI.pszPassword );
					}
				}
				else {
					FLERROR_LOG( PROGRAM_NAME, _T( "[NPROTECT] invalid data : pCSAuth3->dwPacketSize (%d)" ), pCSAuth3->dwPacketSize );
				}
				return true;
			}
		case NPGAMEMON_COMM_ERROR:
		case NPGAMEMON_COMM_CLOSE:
			bAppExit = true;
			return false;
		case NPGAMEMON_INIT_ERROR:		// "게임가드 초기화 에러
			bAppExit = true;
			FLSPrintf( msg, _countof( msg ), g_Neuz.m_strArray.GetAt(17), dwArg );
			lpszMsg	= msg;
			break;
		case NPGAMEMON_SPEEDHACK:		// "스피드핵이 감지되었습니다."
			bAppExit = true;
			lpszMsg	= g_Neuz.m_strArray.GetAt(18);
			break;
		case NPGAMEMON_GAMEHACK_KILLED:	// "게임핵이 발견되었습니다."
		case NPGAMEMON_GAMEHACK_DETECT:	// "게임핵이 발견되었습니다."
			bAppExit = true;
			lpszMsg	= g_Neuz.m_strArray.GetAt(19);
			break;
		case NPGAMEMON_GAMEHACK_DOUBT:	//"게임이나 게임가드가 변조되었습니다.";	
			bAppExit = true;
			lpszMsg = g_Neuz.m_strArray.GetAt(20);
			break;
	}

	if( lpszMsg )
	{
#ifdef __BUILD_FOR_MSGBOX
		FLERROR_LOG( PROGRAM_NAME, _T( lpszMsg ) );
#else __BUILD_FOR_MSGBOX
		MessageBox( g_Neuz.GetSafeHwnd(), lpszMsg, "nProtect GameGuard", MB_OK );
#endif	// __BUILD_FOR_MSGBOX
	}

	if( bAppExit )
	{
		//ExitProcess( 0 );
		::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
		return false;
	}

    return true;
}


void CNeuzApp::HashMD5( char* szResult, size_t cchResult, LPCTSTR szPWD )
{
	char szBuffer[256];
	//char* szKey = Settings::NEUZ_PHASH;

	FLSPrintf( szBuffer, _countof( szBuffer ), "%s%s", Settings::NEUZ_PHASH, szPWD );
	md5( szResult, cchResult, szBuffer );
}

// 캐릭터 삭제 키를 얻는다. ( szNo는 유저 입력 값 )
//     일본 - 패스워드 -> 2차 비번
//     미국 - 패스워드 
//     그외 - 입력값 
void CNeuzApp::GetDeleteKey( LPCTSTR szNo, TCHAR* deleteKey, const size_t lenDeleteKey )
{
	if( g_xFlyffConfig->IsEncryptPWDMode() == true )
	{
		switch( g_xFlyffConfig->GetMainLanguage() )
		{
// 		case LANG_JAP:
// 			if( m_bThroughPortal )
// 				strcpy( szKey, szNo );	// 한게임은 이미 암호화 되어 있음 
// 			else
// 				HashMD5( szKey, szNo );
// 			break;
		case LANG_USA:
		case LANG_ID:
		case LANG_PHP:
		case LANG_GER:
		case LANG_SPA:
		case LANG_POR:
		case LANG_FRE:
		//case LANG_VTN: //베트남어는 이미 암호화 되어 있다.
		case LANG_RUS:
			HashMD5( deleteKey, lenDeleteKey, szNo );
			break;
		case LANG_TWN:
		case LANG_HK:
		default:
			FLStrcpy( deleteKey, lenDeleteKey, szNo );
			break;
		}
	}
	else
	{
		FLStrcpy( deleteKey, lenDeleteKey, szNo );
	}
}

void CNeuzApp::SetAccountInfo( LPCTSTR lpszAccount, LPCTSTR lpszpw, LPCTSTR lpszSessionPwd, bool bEncrypt )
{
	CString strAccount	= lpszAccount;
	strAccount.TrimLeft();	
	strAccount.TrimRight();		// TRIM
	FLStrcpy( g_Neuz.m_szAccount, _countof( g_Neuz.m_szAccount ), (LPCTSTR)strAccount );
	FLStrLowercase( g_Neuz.m_szAccount, _countof( g_Neuz.m_szAccount ) );

	CString strSessionPwd	= lpszSessionPwd;
	strSessionPwd.TrimLeft();	
	strSessionPwd.TrimRight();
	FLStrcpy( g_Neuz.m_szSessionPwd, _countof( g_Neuz.m_szSessionPwd ), (LPCTSTR)strSessionPwd );

	if( g_xFlyffConfig->IsEncryptPWDMode() == true && bEncrypt)
	{
		if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
		{
			CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetApplet( APP_LOGIN );
			if (!pWndLogin)
			{
				const int nSiteValue = pWndLogin->GetCurrentDomainChannel();
				if( nSiteValue == 10 )	// GPotato
					FLStrcpy( g_Neuz.m_szPassword, _countof( g_Neuz.m_szPassword ), lpszpw );
				else
					HashMD5( g_Neuz.m_szPassword, _countof( g_Neuz.m_szPassword ), lpszpw );
			}
			else
				HashMD5( g_Neuz.m_szPassword, _countof( g_Neuz.m_szPassword ), lpszpw );
		}
		else
			HashMD5( g_Neuz.m_szPassword, _countof( g_Neuz.m_szPassword ), lpszpw );
	}
	else
		FLStrcpy( g_Neuz.m_szPassword, _countof( g_Neuz.m_szPassword ), lpszpw );
}


int g_nOldVirtKey = 0;
//
//// 특정키들은 눌렀을때 반응하도록 한다...
//// 나머지 키들은 업했을때 반응
//static BOOL IsPushedKey( int nVirtKey )
//{
//	const PFLSHORTCUTKEY_CONTROL pShortcutKey_Control = g_xKeyManager->GetShortcutKey_Control();
//
//	if( nVirtKey == VK_LEFT					||
//		nVirtKey == VK_RIGHT				||
//		nVirtKey == VK_UP					||
//		nVirtKey == VK_DOWN					||
//		nVirtKey == VK_PRIOR				||
//		nVirtKey == VK_NEXT					||
//		nVirtKey == pShortcutKey->KeyUp 	||
//		nVirtKey == pShortcutKey->KeyDown	||
//		nVirtKey == pShortcutKey->KeyLeft	||
//		nVirtKey == pShortcutKey->KeyRight	||
//		nVirtKey == pShortcutKey->KeyJump )
//	{
//		return TRUE;
//	}
//
//	return FALSE;
//}

LRESULT CNeuzApp::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	BOOL bHook = GuildDeploy()->MsgProc( hWnd, msg, wParam, lParam );
	if( bHook )
		return 0;

//	g_Console.Log( "MSG %d", msg );

	CWndEdit* pWndEdit = CWndEdit::GetWndEditCtrl();
	if( pWndEdit )
	{
// 9차 채팅상황에서 Function Key 사용 가능
		if( !( ( msg == WM_KEYDOWN || msg == WM_KEYUP ) && (wParam == VK_ESCAPE || wParam == VK_F1 || wParam == VK_F2 || wParam == VK_F3 ||
			wParam == VK_F4 || wParam == VK_F5 || wParam == VK_F6 || wParam == VK_F7 || wParam == VK_F8 || wParam == VK_F9) ) )
//		if( !( ( msg == WM_KEYDOWN || msg == WM_KEYUP ) && wParam == VK_ESCAPE ) )	// 9차 채팅상황에서 Function Key 사용 가능( < 9 )
		{
			if( pWndEdit->IsYouMessage( msg, wParam, lParam ) )	
				return 0;
			if( ( msg == WM_KEYDOWN || msg == WM_KEYUP ) && wParam != VK_RETURN )
				return 0;
		}
	}
	else
	{
		if( msg >= WM_IME_SETCONTEXT && msg <= WM_IME_KEYUP ) // 모든 IME 매시지는 스킵 
			return 0;

		if( CWndEdit::m_hIMCBackup == NULL )
		{
			CWndEdit::m_hIMCBackup = ::ImmAssociateContext( CWndEdit::m_hWnd, NULL );
		}
		CWndEdit::SetAlphaNumeric();
	}

	g_WndMng.WindowRootProc( msg, wParam, lParam );

	//gmpbigsun(100308): here is stop if activated modal window
	if( g_WndMng.IsOpenModalWnd() == TRUE && msg != WM_LOGOUT )
	{
		switch( msg )
		{
		case WM_KEYDOWN:
			{
				if( g_WndMng.m_pLogOutWaitting )
					break;

				if( KillWindow( wParam ) )
					break;

				int nVirtKey = ( int )wParam;
				switch( nVirtKey )
				{
					case	VK_RETURN:
						{
							if( g_WndMng.m_pWndCommItemDlg ) {
								g_WndMng.m_pWndCommItemDlg->OnChildNotify( EN_RETURN, g_WndMng.m_pWndCommItemDlg->m_nIdWnd, (LRESULT*)this ); 
								break;
							}

							if( g_WndMng.m_pWndStatDice ) {
								g_WndMng.m_pWndStatDice->SetFinish();
								break;
							}

							CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
							if( pWndChat && pWndEdit == NULL ) {
								pWndChat->AddWndStyle( WBS_MODAL );
								pOldModalWindow = g_WndMng.m_pWndFocus;

								pWndChat->SetFocus();
								CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
								pWndEdit->SetFocus();

								break;
							}
						}
						break;
					default:
						break;
				}

				break;
			}
#ifdef __IMPROVE_MAP_SYSTEM
		case WM_CLOSE:
			{
				prj.m_MapInformationManager.DeleteAllMapInformation();
				g_Neuz.m_bQuit = TRUE;
				if( wParam == FALSE )
				{
					if( g_pPlayer )
					{
						g_WndMng.m_bConnect = FALSE;
						StopMusic();
						return 1;
					}
				}
				break;
			}
#endif // __IMPROVE_MAP_SYSTEM

		case WM_MOVE:

			//gmpbigsun : client window가 움직였을때 보정 
			//상점 웹박스
			CWebBox* pWebBox = CWebBox::GetInstance();
			if( pWebBox )
				pWebBox->AutoPosition();

			//헬퍼 웹박스
			CHelperWebBox* pHelperWebBox = CHelperWebBox::GetInstance();
			if( pHelperWebBox )
				pHelperWebBox->AutoPostion( );
		break;

		}
		return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
	}

	switch( msg )
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:		//gmpbigsun (20120105 ) : system key 추가 
		{
		//sun: 12, 튜토리얼 개선
			CWndGuideSystem* pWndGuide = NULL;
			pWndGuide = (CWndGuideSystem*)g_WndMng.GetWndBase( APP_GUIDE );
			if(pWndGuide && pWndGuide->IsVisible()) pWndGuide->m_Condition.nInputKey = (UINT)wParam;
		
			if( g_WndMng.m_pLogOutWaitting ) // 종료중이면 키보드 입력 불가
				break;
			
			if( KillWindow( wParam ) )
				break;

			int nVirtKey = ( int )wParam;
			switch( nVirtKey )
			{
			case	VK_RETURN:
				{
					if( g_WndMng.m_pWndCommItemDlg ) {
						g_WndMng.m_pWndCommItemDlg->OnChildNotify( EN_RETURN, g_WndMng.m_pWndCommItemDlg->m_nIdWnd, (LRESULT*)this ); 
						break;
					}

					if( g_WndMng.m_pWndStatDice ) {
						if( !g_WndMng.m_pWndStatDice->IsFinish() )
							g_WndMng.m_pWndStatDice->SetFinish();
						else
							g_WndMng.m_pWndStatDice->OnChildNotify( 0, WIDC_OK, (LRESULT*)this ); 
						break;
					}

					CWndChat* pWndChat = ( CWndChat* )g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
					if( pWndChat && pWndEdit == NULL ) {
						pWndChat->SetFocus();
						CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
						pWndEdit->SetFocus();
						break;
					}
				}
				break;
			default:
				break;
			}

//sun: 13, 하우징 시스템
			if(nVirtKey == VK_ESCAPE)
			{
				if(CDeployManager::GetInstance()->IsReady())
				{
					CDeployManager::GetInstance()->EndDeploy();
					return FALSE;
				}

				GuildHouse->m_dwSelectedObjID = NULL_ID;		//최우선순위이므로 걍 NULL_ID로 ..
			}

			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
			if( pWndWorld && pWndWorld->IsFocusWnd() == FALSE )
				pWndWorld->OnKeyDown( nVirtKey, 0, 0 );
			
			g_bKeyTable[ nVirtKey ] = TRUE;
			g_WndMng.SetMessengerAutoState();

#ifdef __BS_CONSOLE
			if( nVirtKey == VK_INSERT )
			{
				gConsole()->Show( );			
			}
#endif 

		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP: //gmpbigsun (20120105 ) : system key 추가 
		{
			int nVirtKey = (int) wParam;    // virtual-key code
			int lKeyData = lParam;
			for( int i = 0; i < MAX_SLOT_ITEM; ++i )
			{
				if( g_bSlotSwitchAboutEquipItem[ i ] == TRUE )
					g_bSlotSwitchAboutEquipItem[ i ] = FALSE;
			}
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet( APP_WORLD );
			if( pWndWorld && pWndWorld->IsFocusWnd() == FALSE )
				pWndWorld->OnKeyUp( nVirtKey, 0, 0 );

			const PFLSHORTCUTKEY_CONTROL	pShortcutKey_Control( g_xKeyManager->GetShortcutKey_Control() );
			const PFLSHORTCUTKEY_UI			pShortcutKey_UI( g_xKeyManager->GetShortcutKey_UI() );

			if(g_bKeyTable[ pShortcutKey_Control->KeyCameraZoomOut ])
			{
				if(CDeployManager::GetInstance()->IsReady())
				{
					CDeployManager::GetInstance()->m_fAngle += 45.0f;
					if(CDeployManager::GetInstance()->m_fAngle > 359.0f) CDeployManager::GetInstance()->m_fAngle = 0.0f;
					CDeployManager::GetInstance()->m_pTargetObj->SetAngle(CDeployManager::GetInstance()->m_fAngle);
					CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
				}
			}
			else if(g_bKeyTable[ pShortcutKey_Control->KeyCameraZoomIn ])
			{
				if(CDeployManager::GetInstance()->IsReady())
				{
					CDeployManager::GetInstance()->m_fAngle -= 45.0f;
					if(CDeployManager::GetInstance()->m_fAngle < -359.0f) CDeployManager::GetInstance()->m_fAngle = 0.0f;
					CDeployManager::GetInstance()->m_pTargetObj->SetAngle(CDeployManager::GetInstance()->m_fAngle);
					CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
				}
			}

			if( g_bKeyTable[ pShortcutKey_UI->KeyAllUIOnOff ] ) 
			{
				if(!g_WndMng.m_clearFlag)
					g_WndMng.ClearAllWnd();
				else
					g_WndMng.RestoreWnd();
			}
			
			g_bKeyTable[ nVirtKey ] = FALSE;
			g_WndMng.SetMessengerAutoState();
		}
		break;

	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
//sun: 13, 하우징 시스템
		if(CDeployManager::GetInstance()->IsReady() && msg == WM_MOUSEWHEEL)
		{
			int nDelta = (short)HIWORD(wParam);
			if(nDelta < 0)
			{
				CDeployManager::GetInstance()->m_pTargetObj->AddAngle(2.5f);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
			else
			{
				CDeployManager::GetInstance()->m_pTargetObj->AddAngle(-2.5f);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
		}
			
		g_WndMng.SetMessengerAutoState();		
		
		break;

	case WM_CLOSE:
		{
#ifdef __IMPROVE_MAP_SYSTEM
			prj.m_MapInformationManager.DeleteAllMapInformation();
#endif // __IMPROVE_MAP_SYSTEM
			g_Neuz.m_bQuit = TRUE;
			if( wParam == FALSE )
			{
				if( g_pPlayer )
				{
					g_WndMng.m_bConnect = FALSE;
					StopMusic();
					return 1;
				}
			}
		}
		break;
	case WM_LOGOUT:
		{
			g_WndMng.OpenTitle();
			PlayMusic( BGM_TITLE, 0 );
			g_WndMng.m_bConnect = FALSE;
			return 1;
		}
		break;
		
	case WM_MOVE:
		{
			CWebBox* pWebBox	= CWebBox::GetInstance();
			if( pWebBox && pWebBox->m_hDlg )
				::SendMessage( pWebBox->m_hDlg, WM_INITDIALOG, 0, 0 );
		}
		break;

	case WM_COMMAND:
		break;
	case WM_ACTIVATE:
		//gmpbigsun( 20120328 ) : 활성화/비활성시 키버퍼를 리셋시킨다.
		memset( &g_bKeyTable, 0, sizeof( g_bKeyTable ) );
		
		m_bActiveNeuz = TRUE;
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			m_bActiveNeuz = FALSE;
		}
		break;

	case WM_SYSCOMMAND:
		{
			if( (wParam & 0xFFF0) == SC_MOVE )
			{
				if( g_bForWindowDragging )
				{
					::GetCursorPos( &g_ptForWindowDragging );
					return 0;
				}
			}
		}
		break;

	case WM_NCLBUTTONUP:
		g_bForWindowDragging = FALSE;
		break;

	case WM_NCLBUTTONDOWN:
		{
			if( m_bWindowed == true && wParam == HTCAPTION )
			{
				g_bForWindowDragging = TRUE;
			}
		}
		break;

	case WM_KILLFOCUS:
		g_bForWindowDragging = FALSE;
		break;
	}

	return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}

BOOL CNeuzApp::KillWindow( int wParam )
{
	CWndStateConfirm* pWndStateConfirm = (CWndStateConfirm*)g_WndMng.GetWndBase( APP_STATE_CONFIRM );
	if( pWndStateConfirm )
	{
		if( wParam == VK_RETURN )
		{
			pWndStateConfirm->SendYes();
		}
		else if( wParam == VK_ESCAPE )
		{
//sun: 13, 캐릭터정보창 리뉴얼
	//CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER3 );		//gmpbigsun(20100625) : not used

			pWndStateConfirm->Destroy();
		}
		return TRUE;
	}

	return FALSE;
}

unsigned int __stdcall LoadProperty( void* p )
{ 
	BOOL bOK = prj.OpenProject( "Masquerade.prj" );
	if( FALSE == bOK )
	{
		//spec manager의 문제발생 시스템에 의거 클라도 죽인다.
		::AfxMessageBox( "Error Open Project" );
		::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0 );
	}
	
	return 0;
}

void CNeuzApp::WaitLoading()
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "WaitLoading start" ) );
	if( m_hThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hThread, INFINITE );

		CloseHandle( m_hThread );
		m_hThread = INVALID_HANDLE_VALUE;
	}
	FLTRACE_LOG( PROGRAM_NAME, _T( "WaitLoading end" ) );
}

void CNeuzApp::FinalLoaing( )
{
#ifdef __CLIENT 
	//스레드 생성 없이 마지막 로딩을 진행한다.	
	prj.m_MapInformationManager.LoadMapInformationData( );
#endif 
}

//---------------------------------------------------------------------------------------------
// 로딩 쓰레드 시작
//---------------------------------------------------------------------------------------------
void CNeuzApp::BeginLoadThread()
{
	CResFile::ScanResource( _T("") );
	prj.LoadPreFiles();

	unsigned int dwThreadId( 0 );
	m_hThread = (HANDLE) _beginthreadex( NULL, 0, LoadProperty, (LPVOID)this, 0, &dwThreadId );

//  gmpbigsun( 20120629 ) : Select server로 지연 ::Connected
//	WaitLoading();
}
//---------------------------------------------------------------------------------------------


//
// 디바이스와 비독립적인 매쉬, 폴리곤 등을 처음 초기화 하는 곳 
//

//gmpbigsun( 20120717 ) : Logo Window
Gdiplus::Image* g_pImage = NULL;
ULONG_PTR g_pGdiplusToken = NULL;
BLENDFUNCTION g_blend;

void UpdateLogoWnd( HWND hWnd )
{
	SIZE sizeWindow = { g_pImage->GetWidth(), g_pImage->GetHeight() };

	DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		::SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle | 0x80000);

	HDC hDC = ::GetDC(hWnd);
	HDC hdcMemory = CreateCompatibleDC(hDC);

	HBITMAP hBitMap = CreateCompatibleBitmap(hDC, sizeWindow.cx, sizeWindow.cy);
	::SelectObject(hdcMemory, hBitMap);

	RECT rcWindow;
	::GetWindowRect( hWnd, &rcWindow );

	BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
	int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
	stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
	stBmpInfoHeader.biWidth = sizeWindow.cx;   
	stBmpInfoHeader.biHeight = sizeWindow.cy;   
	stBmpInfoHeader.biPlanes = 1;   
	stBmpInfoHeader.biBitCount = 32;   
	stBmpInfoHeader.biCompression = BI_RGB;   
	stBmpInfoHeader.biClrUsed = 0;   
	stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

	PVOID pvBits = NULL;   
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
	ASSERT(hbmpMem != NULL);
	memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);
	if(hbmpMem)   
	{   
		HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

		//POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
		POINT ptWinPos = { rcWindow.left-107, rcWindow.top-139 };
		Gdiplus::Graphics graph(hdcMemory);
		graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

		graph.DrawImage(g_pImage, 0, 0, sizeWindow.cx, sizeWindow.cy);

		HMODULE hFuncInst = LoadLibrary(_T("User32.DLL"));
		typedef BOOL (WINAPI *MYFUNC)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);          
		MYFUNC UpdateLayeredWindow;
		UpdateLayeredWindow = (MYFUNC)::GetProcAddress(hFuncInst, "UpdateLayeredWindow");
		POINT ptSrc = { 0, 0};

		UpdateLayeredWindow(hWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &g_blend, 2);

		graph.ReleaseHDC(hdcMemory);
		::SelectObject( hdcMemory, hbmpOld);   
		::DeleteObject(hbmpMem); 
	}

	::DeleteDC(hdcMemory);
	::DeleteDC(hDC);
}

BOOL CALLBACK LogoDlgProc( HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	switch( iMsg )
	{
	case WM_INITDIALOG:
		{
			//setup GDI+
			HINSTANCE hInstance = g_Neuz.m_hInstance;
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			if( Gdiplus::GdiplusStartup( &g_pGdiplusToken, &gdiplusStartupInput, NULL ) != Gdiplus::Ok )
			{
				AfxMessageBox( _T( "Error: Failed to initalize GDI+" ) );
				return FALSE;
			}

			HRSRC hRsc = FindResource( hInstance, MAKEINTRESOURCE( IDR_PNG1 ), TEXT("PNG") );
			if( hRsc )
			{
				g_blend.BlendOp = 0;
				g_blend.BlendFlags = 0;
				g_blend.AlphaFormat = 1;
				g_blend.SourceConstantAlpha = 255;//AC_SRC_ALPHA

				DWORD imageSize = SizeofResource( hInstance, hRsc );
				HGLOBAL hGlobal = LoadResource( hInstance, hRsc );
				LPVOID pData = LockResource( hGlobal );
				HGLOBAL hBuffer = GlobalAlloc( GMEM_MOVEABLE, imageSize );
				LPVOID pBuffer = GlobalLock( hBuffer );
				CopyMemory( pBuffer, pData, imageSize );
				GlobalUnlock( hBuffer );

				IStream* pStream = NULL;
				HRESULT hr = CreateStreamOnHGlobal( hBuffer, TRUE, &pStream );
				g_pImage = Gdiplus::Image::FromStream( pStream );

				pStream->Release();
				FreeResource( hRsc );

				UpdateLogoWnd( hDlg );
			}
		}
		break;

	case WM_PAINT:
//	case WM_NCPAINT:
		{
		}
		break;

	case WM_DESTROY:
	case WM_QUIT:
		{
			//Remove GDI
			SAFE_DELETE( g_pImage );
			Gdiplus::GdiplusShutdown( g_pGdiplusToken );
			
		}
		break;
	}

	return FALSE;
}

HRESULT CNeuzApp::InitDeviceObjects()
{
	InitSecurity();

	//LOGO window
	m_hWndLogo = CreateDialog( m_hInstance, MAKEINTRESOURCE( IDD_DIALOG4 ), ::GetForegroundWindow(), LogoDlgProc );

	BeginLoadThread();

	SetMouseCursor( CUR_DELAY ); 

	g_MusicMng.LoadScript( MakePath( DIR_MUSIC, _T( "default.bgm" ) ) );
	g_SoundMng.LoadScript( MakePath( DIR_CLIENT, _T( "sound.inc" ) ) );

#ifndef _DEBUG
	{
		UINT nMem = m_pd3dDevice->GetAvailableTextureMem();
		int nVersion = LOWORD(m_d3dSettings.pWindowed_AdapterInfo->AdapterIdentifier.DriverVersion.u.LowPart);
		FLERROR_LOG( PROGRAM_NAME, _T( "%s, %d(%d)" ), m_d3dSettings.pWindowed_AdapterInfo->AdapterIdentifier.Description, nMem, nVersion );
	}
#endif 

#ifdef __YENV
	if( m_d3dCaps.PixelShaderVersion < D3DPS_VERSION(1,1) )
		g_dwUsablePS_Level = 0;
	else if( m_d3dCaps.PixelShaderVersion < D3DPS_VERSION(2,0) )
		g_dwUsablePS_Level = 1;
	else 
		g_dwUsablePS_Level = 2;
#endif //__YENV

	if( m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY )	// 정사각형의 텍스쳐만 지원하는 카드나 드라이버라면 실해못한다.
	{
		MessageBox( GetSafeHwnd(), m_strArray.GetAt(12), m_strArray.GetAt(11), MB_OK );
		FLERROR_LOG( PROGRAM_NAME, m_strArray.GetAt(12) );
		int *p = NULL;
		*p = 1;
	}

	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	prj.m_modelMng.InitDeviceObjects( m_pd3dDevice );
	prj.m_terrainMng.InitDeviceObjects( m_pd3dDevice );

	CWorld::StaticInitDeviceObjects( m_pd3dDevice );
	
	g_DamageNumMng.LoadTexture( m_pd3dDevice );		// 데미지 이펙트 

	g_SfxMng.InitDeviceObjects(m_pd3dDevice);
	g_SfxMeshMng.InitDeviceObjects(m_pd3dDevice);

	g_Shadow.InitDeviceObjects( m_pd3dDevice );
	g_Shadow.LoadElement( "Shadow.o3d" );

	g_ParticleMng.InitDeviceObjects( m_pd3dDevice );
	if( g_Option.m_nTextureQuality == 2 )		// 텍스쳐 퀄리티 낮음이면 파티클 실행안됨.
		g_ParticleMng.m_bActive = FALSE;

	m_TextureGauge[0].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauMagChargeBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureGauge[1].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauMagChargeHead.TGA" ), 0xffff00ff, TRUE );
	m_TextureGauge[2].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauMagChargeBody.TGA" ), 0xffff00ff, TRUE );
	m_TextureGauge[3].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauMagChargeBody2.tga" ), 0xffff00ff, TRUE );
	m_TextureGauge[4].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauMagChargeTail.TGA" ), 0xffff00ff, TRUE );
	
	m_TextureHPGauge[0].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauPartyHpBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureHPGauge[1].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauPartyHp.TGA" ), 0xffff00ff, TRUE );
	m_TextureHPGauge[2].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauPartyHp_Dbf.TGA" ), 0xffff00ff, TRUE );
	m_TextureTurboGauge[0].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauAccelBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureTurboGauge[1].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauAccelInner.TGA" ), 0xffff00ff, TRUE );
	m_TextureCastingGauge[0].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauAccelBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureCastingGauge[1].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauAccelInner.TGA" ), 0xffff00ff, TRUE );

	m_pMasterIcon[0] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Expert1.dds"), 0xffff00ff );
	m_pMasterIcon[1] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Expert2.dds"), 0xffff00ff );
	m_pMasterIcon[2] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Expert3.dds"), 0xffff00ff );
	m_pMasterIcon[3] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Expert4.dds"), 0xffff00ff );
	m_pMasterIcon[4] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Expert5.dds"), 0xffff00ff );
	m_pMasterIcon[5] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Expert6.dds"), 0xffff00ff );
	m_pHeroIcon		 = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_Hero.dds"), 0xffff00ff );

//sun: 11, 채집 시스템
	m_TexCltGauge[0].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauAccelBG.TGA" ), 0xffff00ff, TRUE );
	m_TexCltGauge[1].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauAccelInner.TGA" ), 0xffff00ff, TRUE );
	//m_TexCltGauge[2].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, "GauPartyHp_Dbf.TGA" ), 0xffff00ff, TRUE );
	
	//sun: 8차 엔젤 소환 Neuz, World, Trans
	m_TextureAngelGauge[0].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauAngelExpBG.TGA" ), 0xffff00ff, TRUE );
	m_TextureAngelGauge[1].LoadTexture( m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauAngelExp.TGA" ), 0xffff00ff, TRUE );

	m_dwPVPTime = GetTickCount();
	m_nTexAlpha		= 255;
	m_bTexLoad      = FALSE;
	m_bTexLoadAlpha = FALSE;

	g_toolTip.InitTexture();
	g_toolTipSub1.InitTexture();
	g_toolTipSub2.InitTexture();
	
#ifdef __YENV
	HRESULT	hr;
	LPD3DXBUFFER	pErr = NULL;

#ifdef _DEBUG
	if( FAILED(hr = D3DXCreateEffectFromFile( m_pd3dDevice, "hlsl.fx", NULL, NULL, 0, NULL, &m_pEffect, &pErr )))
#else //_DEBUG
	if( FAILED(hr = D3DXCreateEffectFromResource( D3DDEVICE, NULL, MAKEINTRESOURCE(IDR_DATA3), NULL, NULL, 0, NULL, &m_pEffect, &pErr )))
#endif //_DEBUG
	{
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK );
	}
	else
	{
		m_hTechnique	= m_pEffect->GetTechniqueByName( "TShader" );
		m_hmWVP			= m_pEffect->GetParameterByName( NULL, "mWVP" );
		m_hTexDiffuse   = m_pEffect->GetParameterByName( NULL, "Tex_Diffuse" );
		m_hTexBump      = m_pEffect->GetParameterByName( NULL, "Tex_Bump" );
		m_hTexSpecular  = m_pEffect->GetParameterByName( NULL, "Tex_Specular" );
		m_hvLightDir	= m_pEffect->GetParameterByName( NULL, "vLightDir" );
		m_hvEyePos		= m_pEffect->GetParameterByName( NULL, "vEyePos" );	
		m_hvFog			= m_pEffect->GetParameterByName( NULL, "vFog" );	
		m_hvDiffuse		= m_pEffect->GetParameterByName( NULL, "I_d" );	
		m_hvAmbient		= m_pEffect->GetParameterByName( NULL, "I_a" );			
		m_hvSpecual		= m_pEffect->GetParameterByName( NULL, "I_s" );			
	}
	SAFE_RELEASE( pErr );

	if( g_dwUsablePS_Level < 2 )
		g_Option.m_bSpecBump = FALSE;

	m_pEnvTex = NULL;
#endif //__YENV	
/*

	{
		CScript s;
		if( s.Load(MakePath( DIR_CLIENT, "GameGuard.inc" )) )
		{
			s.GetToken();
			while( s.tok != FINISHED )
			{
				m_strArray.Add( s.Token );
				s.GetToken();
			}
		}
	}

	InitNProtect();

*/

	// 테마 세팅 
	CWndBase::m_Theme.LoadTheme( m_pd3dDevice, "Default.thm" );
	CWndBase::m_Theme.InitDeviceObjects( m_pd3dDevice );

	FLERROR_LOG( PROGRAM_NAME, "m_Theme.InitDeviceObjects() OK" );

	// CWndBase 세팅 
	g_WndMng.m_pApp = this;
	g_WndMng.m_pTheme = &CWndBase::m_Theme;
	g_WndMng.m_pFont = CWndBase::m_Theme.m_pFontText;
	g_WndMng.Initialize();
	g_WndMng.InitDeviceObjects();
	FLERROR_LOG( PROGRAM_NAME, "g_WndMng.InitDeviceObjects OK" );
	CWndBase::m_hWnd = GetSafeHwnd();
	CWndBase::SetForbidTexture( m_pd3dDevice, MakePath( DIR_ICON, "icon_forbid.dds" ) );

	// 다이얼로그 메시지 세팅 
	g_DialogMsg.InitDeviceObjects( m_pd3dDevice );
	FLERROR_LOG( PROGRAM_NAME, "g_DialogMsg.InitDeviceObjects OK" );
	

	// 2DRender 세팅 
	m_2DRender.InitDeviceObjects( m_pd3dDevice );
	m_2DRender.m_pFont = CWndBase::m_Theme.m_pFontText;

	g_WndMng.OpenTitle( TRUE );
	FLERROR_LOG( PROGRAM_NAME, "g_WndMng.OpenTitle OK" );

	InitFST();		// init frame skipping time

	m_texQuestEmoticon.LoadScript( m_pd3dDevice, MakePath( DIR_ICON, "icon_QuestEmoticon.inc" ) );
	m_TexturePackPVP.LoadScript( m_pd3dDevice, MakePath( DIR_SFX, "CountFight.inc" ) );

	TexturePool::Get()->Init( m_pd3dDevice );

	PlayMusic( BGM_TITLE, 0 );

	FLERROR_LOG( PROGRAM_NAME, "InitDeviceObjects OK" );

	return S_OK;
}

void CNeuzApp::ResetStaticValues( )
{
	GuildHouse->ResetValues( );
}

//sun: 밝기, 감마, 명암 조절 Neuz
void CNeuzApp::Drv_SetGamma(HWND hWnd, float gamma, int overbright, float fContrast)
{
    unsigned short ramp[3*256];
    float f;
	   
    GetDeviceGammaRamp( GetDC(hWnd), ramp );
	   
    for ( int i = 0; i < 256; i++ )
	   {
        f = (float)(255 * pow((float)i/256, 1/gamma));
        f = f * fContrast + overbright;
        if (f < 0)
            f = 0;
        if (f > 255)
            f = 255;
        ramp[i+0] = ramp[i+256] = ramp[i+512] = ((unsigned short)f<<8) ;
    }
	   
    SetDeviceGammaRamp( GetDC(hWnd), ramp );
}
/*
void CNeuzApp::Loop()
{
	   if (GetAsyncKeyState(VK_F5)) 
		   g_fGamma -= 0.1f;
	   if (GetAsyncKeyState(VK_F6)) 
		   g_fGamma += 0.1f;
	   if (GetAsyncKeyState(VK_F7)) 
		   g_nOverBright -= 1;
	   if (GetAsyncKeyState(VK_F8)) 
		   g_nOverBright += 1;
	   if (GetAsyncKeyState(VK_F9)) 
		   g_fContrast -= 0.1f;
	   if (GetAsyncKeyState(VK_F10)) 
		   g_fContrast += 0.1f;
	   
	   Drv_SetGamma(GetSafeHwnd(), g_fGamma, g_nOverBright, g_fContrast);
}
*/

void CNeuzApp::CalculateGameGradeMarkAlpha( void )
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
	if( pWndWorld == NULL )
	{
		return;
	}

	//gmpbigsun( 20110909 ) : 
	//등급물 표시가 ON->항상보임, OFF->1시간마다
	if( g_Option.m_bGameGradeRendering == TRUE )
	{
		m_bGameGradeMarkRenderingSwitch = TRUE;
		m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MAXIMUM_ALPHA;
		m_eGameGradeMarkState = FADE_IN;
		m_dwTimeOld = g_tmCurrent;
		m_dwTimeGameGradeMarkRendering = g_tmCurrent + SEC( 0 );
	}
	else if( g_Option.m_bGameGradeRendering == FALSE )
	{
		if( g_tmCurrent > m_dwTimeGameGradeMarkRendering )
		{
			m_bGameGradeMarkRenderingSwitch = TRUE;
			m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MINIMUM_ALPHA;
			m_eGameGradeMarkState = FADE_IN;
			m_dwTimeOld = g_tmCurrent;
			m_dwTimeGameGradeMarkRendering = g_tmCurrent + SEC( GAME_GRADE_MARK_RENDERING_INTERVAL_SECOND );
		}
	}

	if( m_bGameGradeMarkRenderingSwitch == TRUE )
	{
		float fDelta = ( static_cast< float >( g_tmCurrent - m_dwTimeOld ) / 1000.0f );
		m_dwTimeOld = g_tmCurrent;

		switch( m_eGameGradeMarkState )
		{
		case FADE_IN:
			{
				m_nGameGradeMarkAlpha += static_cast< int >( GAME_GRADE_MARK_FADING_SPEED * fDelta );
				if( m_nGameGradeMarkAlpha > GAME_GRADE_MARK_MAXIMUM_ALPHA )
				{
					m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MAXIMUM_ALPHA;
					m_dwTimeGameGradeMarkShowing = g_tmCurrent + SEC( GAME_GRADE_MARK_SHOWING_SECOND );
					m_eGameGradeMarkState = SHOW;
				}
				break;
			}
		case SHOW:
			{
				if( g_tmCurrent > m_dwTimeGameGradeMarkShowing )
				{
					m_eGameGradeMarkState = FADE_OUT;
				}
				break;
			}
		case FADE_OUT:
			{
				m_nGameGradeMarkAlpha -= static_cast< int >( GAME_GRADE_MARK_FADING_SPEED * fDelta );
				if( m_nGameGradeMarkAlpha < GAME_GRADE_MARK_MINIMUM_ALPHA )
				{
					m_nGameGradeMarkAlpha = GAME_GRADE_MARK_MINIMUM_ALPHA;
					m_bGameGradeMarkRenderingSwitch = FALSE;
					m_eGameGradeMarkState = FADE_IN;
				}
				break;
			}
		}
	}
}


#ifdef _DEBUG
BOOL CNeuzApp::LoadAllModel( )
{
	CModelMng* pModelMng = &prj.m_modelMng;
	if( NULL == pModelMng )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	CModel* pModel = NULL;

	std::set< CModel* > setTest;

	int nTotal = 0;

	for( int nType = 0; nType < MAX_OBJTYPE ; nType++ )
	{
		CIndexArray< tagMODELELEM >* apModelElem = &pModelMng->m_aaModelElem[ nType ];
		int nMax = apModelElem->GetSize();
		for( int i = 0; i < nMax; ++i )
		{
			++nTotal;
			pModel = NULL;
			LPMODELELEM pModelElem = (LPMODELELEM) apModelElem->GetAt( i );
			if( pModelElem )
			{
				pModel = pModelMng->LoadModel( m_pd3dDevice, nType, pModelElem->m_dwIndex );
				if( NULL == pModel )
				{
					//FLASSERT( 0 && "pModel is NULL" );
				}
			}
			else 
			{
				//FLASSERT( 0 && "pModelElem is NULL" );
			}

			//MODELTYPE_MESH 타입은 내부에서 포인터를 저장하고있다. 
			if( pModel && pModelElem->m_dwModelType != MODELTYPE_MESH )
				setTest.insert( pModel );

		}

		//SeqDeleter< std::vector< CModel* > > ( ) ( vecTest );
 		for( std::set< CModel* >::iterator iter = setTest.begin(); iter != setTest.end(); ++iter )
 		{
 			CModel* pDelModel = *iter;
 			SAFE_DELETE( pDelModel );
 		}
 
 		setTest.clear();

	}

	int a = 0;

	AfxMessageBox( "Loaded all model OK" );

	return TRUE;
}
#endif
