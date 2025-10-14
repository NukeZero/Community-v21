
#include "StdAfx.h"

#ifdef __BS_CONSOLE

#include "Console.h"
#include "../Neuz/dpclient.h"
extern	CDPClient	g_DPlay;

//TEST!!!!!
#include "WndGuildHouse.h"
#include "WndHousing.h"
#include "ResData.h"
#include "WndGuildTabPower.h"
#include "AppDefine.h"
#include "defineText.h"
#include "../Neuz/dpCertified.h"
#include "WndExtra.h"
#include "WndEnchantBaruna.h"
#include "WndTooltip.h"

#include "WndHelperWebBox.h"
#include "WndField2.h"
#include "defineObj.h"
#include "../Neuz/WebBox.h"
#include "WndField3.h"

void _help_commend(  std::vector< string >& arg )
{ 
	gConsole()->AddString( "***************** Command list ************************" );
	gConsole()->ShowAllCommand( );	
	gConsole()->AddString( "***************** Command list end ***");
}

void _show_mypos( std::vector< string >& arg )
{
	if( !g_pPlayer )
	{
		gConsole()->AddString( "g_pPlayer is NULL" );
		return;
	}
			
	D3DXVECTOR3 kMyPos = g_pPlayer->GetPos( );
	char temp[ 64 ];
	FLSPrintf( temp, _countof(temp), "MYPOS:%f %f %f", kMyPos.x, kMyPos.y, kMyPos.z );
	gConsole()->AddString( temp );
}

void _show_fps( std::vector< string >& arg )
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( 2002 );
	if( !pWndWorld )
		return;

	pWndWorld->m_bRenderFPS = !pWndWorld->m_bRenderFPS;

	gConsole()->Log( "=>now showing fps is %d", pWndWorld->m_bRenderFPS );
	
}

void _set_speed( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	float fSpeedMul = 1.0f;
	const char* str = arg[ 1 ].c_str( );
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( str, "%f", &fSpeedMul );
#else
	sscanf( str, "%f", &fSpeedMul );
#endif

	gConsole()->_fSpeedMul = fSpeedMul;

	gConsole()->Log( "=>now speed is ( org * %3.0f )", fSpeedMul );
}

void _show_mpu( std::vector< string >& arg )
{
	gConsole()->Log( "=>MPU : %d", g_MPU );

}

void _volum( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int iVol = 0;
	const char* str = arg[ 1 ].c_str( );
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( str, "%d", &iVol );
#else
	sscanf( str, "%d", &iVol );
#endif
	float fVol = (float)iVol / 255.0f;
	SetVolume( fVol );
}

void _show_music_info( std::vector< string >& arg )
{
#if ( __USE_FMOD_MUSIC == ML_MILES )
	gConsole()->Log( "FMOD Music, Current Volum is : %f", GetVolume( ) );
#else
	gConsole()->Log( "Miles Music or ..., Current Volum is :%f", GetVolum( ) );
#endif
}

void _play_music( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int musicIndex = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[ 1 ].c_str(), "%d", &musicIndex );
#else
	sscanf( arg[ 1 ].c_str(), "%d", &musicIndex );
#endif
	PlayBGM( musicIndex );
}

void _play_sound( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int soundIndex = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[ 1 ].c_str(), "%d", &soundIndex );
#else
	sscanf( arg[ 1 ].c_str(), "%d", &soundIndex );
#endif
	//PlayBGM( musicIndex );
	PLAYSND( soundIndex );

}

void _set_weather( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int weather = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[ 1 ].c_str( ), "%d", &weather );
#else
	sscanf( arg[ 1 ].c_str( ), "%d", &weather );
#endif
	CWorld::m_skyBox.SetWeather( weather, TRUE );
}

void _change_model( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;
	
	int modelIndex = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[ 1 ].c_str(), "%d", &modelIndex );
#else
	sscanf( arg[ 1 ].c_str(), "%d", &modelIndex );
#endif

	if( g_pPlayer ) 
	{
		g_pPlayer->SetIndex( D3DDEVICE , modelIndex );
	}
}

void _sfx_pos( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int index = 0;
	int nRepeat = 5;

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[1].c_str(), "%d", &index );
#else	
	sscanf( arg[1].c_str(), "%d", &index );
#endif
	
	if( g_pPlayer )
	{
		D3DXVECTOR3 kPos = g_pPlayer->GetPos( );
		D3DXMATRIX kMat = g_pPlayer->GetMatrixRotation( );

		D3DXVECTOR3 kDir;
		kDir.x = kMat._31;
		kDir.y = kMat._32;
		kDir.z = kMat._33;

		kPos = kPos - kDir * 2.0f;

		CSfx *pSfx = NULL;
		for( int i = 0; i < nRepeat; ++i )
		{
			
// 			CMover* pPet =  prj.GetMover( g_pPlayer->GetEatPetId( ) );
// 			if( pPet )
// 			{
// 				index = pPet->m_dwMoverSfxId;
// 				pSfx = CreateSfx( D3DDEVICE, index, kPos );
// 				pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f  ) );
// 			}
			
		}
	}
}

void _sfx_link( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	int index = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[1].c_str(), "%d", &index );
#else
	sscanf( arg[1].c_str(), "%d", &index );
#endif

	if( g_pPlayer )
	{
		CSfx *pSfx = CreateSfx( D3DDEVICE, index, g_pPlayer->GetPos(), g_pPlayer->GetId() );  // 시전동작이기때문에 무조건 자기에게 나타난다.
		if( pSfx )
		{
			pSfx->SetPartLink( 0 );		// 오른손에 링크.
			pSfx->SetPartLink( 1 );
		}

	}
}

void _world_height( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	float fnewMax = 0.0f;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[1].c_str(), "%f", &fnewMax );
#else
	sscanf( arg[1].c_str(), "%f", &fnewMax );
#endif

	g_WorldMng.Get()->m_fMaxHeight = fnewMax;

	gConsole()->Log( "==>OK.. Now the world height is %f(m)", fnewMax );
}

void _test_func( std::vector< string >& arg )
{
	BOOL bAuthority = FALSE;
	CGuild* pGuild = g_pPlayer->GetGuild( );
	if( pGuild )
		bAuthority = pGuild->IsAuthority( g_pPlayer->m_idPlayer, PF_GUILDHOUSE_UPKEEP );

	if( !bAuthority )		//유지비 권한이 없으면 
	{
		g_WndMng.PutString( GETTEXT( TID_GAME_GUILDHOUSE_TAX_LEVEL ) );
		return;
	}

	if( !g_WndMng.m_pWndUpkeep )
	{
		g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
		g_WndMng.m_pWndUpkeep->Initialize(&g_WndMng, APP_CONFIRM_BUY_);
		
	}
}

void _random_moving( std::vector< string >& arg )
{
	g_Console._bRandomMoving = !g_Console._bRandomMoving;

	g_Console.Log( "now Random Moving is %d", g_Console._bRandomMoving );
}

void _ambient( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	DWORD all = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[1].c_str(), "%d", &all );
#else
	sscanf( arg[1].c_str(), "%d", &all );
#endif 
	
	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			pWorld->m_dwAmbient = 0;
			pWorld->m_dwAmbient |= (all << 16 ) ;
			pWorld->m_dwAmbient |= (all << 8 ) ;
			pWorld->m_dwAmbient |= all;
			
		}
	}
}

void _diffuse( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	DWORD all = 0;
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[1].c_str(), "%d", &all );
#else
	sscanf( arg[1].c_str(), "%d", &all );
#endif
	
	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			pWorld->m_dwDiffuse = 0;
			pWorld->m_dwDiffuse |= (all << 16 ) ;
			pWorld->m_dwDiffuse |= (all << 8 ) ;
			pWorld->m_dwDiffuse |= all;
			
		}
	}
}

//void _change_reflect_texture1( std::vector< string >& arg )
//{
//	if( arg.size() < 2 )
//		return;
//
//	if( g_pReflectMap )
//		SAFE_RELEASE( g_pReflectMap );
//	
//	if( FAILED( LoadTextureFromRes( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, arg[1].c_str() ), &g_pReflectMap ) ) )
//		AfxMessageBox( "error! cannot find reflect1 texture file" );
//
//	g_Console.Log( "changed reflection texture1" );
//}
//
//void _change_reflect_texture2( std::vector< string >& arg )
//{
//	if( arg.size() < 2 )
//		return;
//
//	if( g_pReflectMap2 )
//		SAFE_RELEASE( g_pReflectMap2 );
//	
//	if( FAILED( LoadTextureFromRes( g_Neuz.m_pd3dDevice, MakePath( DIR_MODELTEX, arg[1].c_str() ), &g_pReflectMap2 ) ) )
//		AfxMessageBox( "error! cannot find reflect1 texture file" );
//
//	g_Console.Log( "changed reflection texture2" );
//}

void _fog_start( std::vector< string >& arg )
{
	if( arg.size() < 2 )
		return;

	float fVal = 0.0f;

#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	sscanf_s( arg[1].c_str(), "%f", &fVal );
#else
	sscanf( arg[1].c_str(), "%f", &fVal );
#endif

	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			pWorld->m_fFogStartValue = fVal;
		}
	}
}


// nomal func here
void RandomMoving( DWORD dt )
{
	static DWORD dwDelta = 0;
	dwDelta += dt;
	if( dwDelta > 5000 && g_pPlayer )
	{
		CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );		
		if( pWndChat )
		{
			static BOOL bFlaris = FALSE;

			CString str;

			if( bFlaris )
 				str = "/텔레 1 flaris";
			else str = "/텔레 1 darkon";

			bFlaris = !bFlaris;

			pWndChat->m_wndEdit.AddString( (LPCTSTR)str );
			pWndChat->OnChildNotify( EN_RETURN, WIDC_EDIT, NULL );

			static DWORD index = 0;
			g_Console.Log( "moving the other world %d", index++ );
		}

		//D3DXVECTOR3 kDir;
		//float fAngle = float( rand() % 361 );
		//float fSpeed = float( rand() % 30 );
		//AngleToVectorXZ( &kDir, fAngle, fSpeed );

		//D3DXVECTOR3 kPos = g_pPlayer->GetPos( );
		//kPos += kDir;
		//g_pPlayer->SetDestPos( kPos );

		//static unsigned int index = 0;
		//g_Console.Log( "(%d) : DestPos( %f, %f )", index, kPos.x, kPos.z );
		//++index;

		//if( index > 0xfffffff0 )
		//	index = 0;

		dwDelta = 0;
	}
}

//extern	CDPCertified	g_dpCertified;


void ConsoleRegister( )
{	
	// 여기서 명령 문자열 과 함수를 등록합니다.
	CONSOLE_REGISTER( string("help"), CON_DATA::VT_FUNCTION, _help_commend, string("도움말") );	
	CONSOLE_REGISTER( string("?"), CON_DATA::VT_FUNCTION, _help_commend, string("도움말") );
	CONSOLE_REGISTER( string("pos"), CON_DATA::VT_FUNCTION, _show_mypos, string("내좌표 출력") );
	CONSOLE_REGISTER( string("fps"), CON_DATA::VT_FUNCTION, _show_fps, string("toggle show fps") );	
	CONSOLE_REGISTER( string("speed"), CON_DATA::VT_FUNCTION, _set_speed, string("스피드 배율") );	
	CONSOLE_REGISTER( string("ㄴ"), CON_DATA::VT_FUNCTION, _set_speed, string("스피드 배율") );	
	CONSOLE_REGISTER( string("mpu"), CON_DATA::VT_FUNCTION, _show_mpu, string("print MPU") );	
	CONSOLE_REGISTER( string("vol"), CON_DATA::VT_FUNCTION, _volum, string("music volum ( 0 - 255 )") );	
	CONSOLE_REGISTER( string("music_info"), CON_DATA::VT_FUNCTION, _show_music_info, string("music info") );	
	CONSOLE_REGISTER( string("music"), CON_DATA::VT_FUNCTION, _play_music, string("play music (number)") );
	CONSOLE_REGISTER( string("sound"), CON_DATA::VT_FUNCTION, _play_sound, string("play sound (number)") );
	
	CONSOLE_REGISTER( string("weather"), CON_DATA::VT_FUNCTION, _set_weather, string("change weather( 2rain, 3snow)") );
	CONSOLE_REGISTER( string("change_model"), CON_DATA::VT_FUNCTION, _change_model, string( "모델바꿈" ) );
	CONSOLE_REGISTER( string("sfx_pos"), CON_DATA::VT_FUNCTION, _sfx_pos, string( "주인공 앞에 play sfx" ) );
	CONSOLE_REGISTER( string("sfx_link"), CON_DATA::VT_FUNCTION, _sfx_link, string( "쥬인공 link sfx" ) );
	CONSOLE_REGISTER( string("world_height"), CON_DATA::VT_FUNCTION, _world_height, string( "월드 높이 설정" ) );
	CONSOLE_REGISTER( string("wh"), CON_DATA::VT_FUNCTION, _world_height, string( "월드 높이 설정" ) );
	CONSOLE_REGISTER( string("test" ), CON_DATA::VT_FUNCTION, _test_func, string( "test" ) );
	CONSOLE_REGISTER( string("random_moving"), CON_DATA::VT_FUNCTION, _random_moving, string("무작위 이동") );

	CONSOLE_REGISTER( string("ambient"), CON_DATA::VT_FUNCTION, _ambient, string("ambient value") );
	CONSOLE_REGISTER( string("diffuse"), CON_DATA::VT_FUNCTION, _diffuse, string("diffuse value") );

	//CONSOLE_REGISTER( string("crm1"), CON_DATA::VT_FUNCTION, _change_reflect_texture1, string("change reflect1") );
	//CONSOLE_REGISTER( string("crm2"), CON_DATA::VT_FUNCTION, _change_reflect_texture2, string("change reflect2") );
	
	CONSOLE_REGISTER( string("fog_start"), CON_DATA::VT_FUNCTION, _fog_start, string("fog start") );
}

void _auto_start( )
{
	
#ifdef _DEBUG
	g_Console.AddString( "★ Excute auto-command ★" );

	std::vector< string > arg;

//	arg.clear();
	arg.push_back( "speed" );
	arg.push_back( "1" );
	_set_speed( arg );

// 	arg.clear( );
// 	arg.push_back( "test" );
// 	_test_func( arg );

//	g_Console.Show( );
#endif

}

int gnFail = 0;
int gnReq = 0;

int debugIndex = -1;

void _update_console2( );		//only hj
void _update_console( )
{
	_update_console2( );

	if( (GetAsyncKeyState( VK_CONTROL ) & 0x80000) && (GetAsyncKeyState( VK_LSHIFT ) & 0x80000) )
	{
		if( g_bKeyTable[ 'T' ] )
		{
			g_bKeyTable[ 'T' ] = 0;
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			if( pWndWorld )
			{
				g_Caption1.AddCaption( "dlaks", pWndWorld->m_pFontAPITitle, TRUE, D3DCOLOR_ARGB( 255, 255, 255, 153 ) );
			}
		}
		else if( g_bKeyTable[ 'I' ] )
		{
			g_bKeyTable[ 'I' ] = FALSE;
			static CWndCM_Sub1List* pWnd = NULL;

			if( !pWnd )
			{
				pWnd = new CWndCM_Sub1List;
				pWnd->Initialize( &g_WndMng  );
			}
			else
			{
				SAFE_DELETE( pWnd );
			}

		}
		else if( g_bKeyTable[ 'L' ] )
		{
			g_bKeyTable[ 'L' ] = FALSE;
			if( !g_WndMng.m_pWndProxyMarket )
			{
				//유저정보 요청
				FLPacket_UserInfoReq kPacket;
				g_DPlay.SendPacket( &kPacket );
				
			//	g_WndMng.m_pWndProxyMarket = new CWndConsignmentMarket;
			//	g_WndMng.m_pWndProxyMarket->Initialize( &g_WndMng );
			}
			else
			{
				SAFE_DELETE( g_WndMng.m_pWndProxyMarket );
			}
			
		}
		else if( g_bKeyTable[ '1' ] )
		{
			g_bKeyTable[ '1' ] = FALSE;

			if( !g_WndMng.m_pWndReawakening )
			{
				g_WndMng.m_pWndReawakening = new CWndReawakening;
				g_WndMng.m_pWndReawakening->Initialize( &g_WndMng );
			}
			else
			{
				SAFE_DELETE( g_WndMng.m_pWndReawakening );
			}

		}
		else if( g_bKeyTable[ '2' ] )
		{
			g_bKeyTable[ '2' ] = FALSE;
			if( g_WndMng.m_pWndReawakening )
			{
				g_WndMng.m_pWndReawakening->StartAnimation( );
			}
		}
		else if( g_bKeyTable[ '3' ] )
		{
			g_bKeyTable[ '3' ] = FALSE;
			if( !g_WndMng.m_pWndChangeLook )
			{
				g_WndMng.m_pWndChangeLook = new CWndChangeLook;
				g_WndMng.m_pWndChangeLook->Initialize( &g_WndMng );
			}
			else
			{
				SAFE_DELETE( g_WndMng.m_pWndCancel_CTW );
			}
		}
		else if( g_bKeyTable[ '4' ] )
		{
			g_bKeyTable[ '4' ] = FALSE;
		}
		else if( g_bKeyTable[ '5' ] )
		{
			g_bKeyTable[ '5' ] = FALSE;

			//HUD on
			static BOOL bOn = FALSE;

			bOn = !bOn;
			g_WndMng.EnableHUD( bOn );
		}
		else if( g_bKeyTable[ '6' ] )
		{
			g_bKeyTable[ '6' ] = FALSE;
			if( g_WndMng.m_pWndTaskBar )
				((CWndTaskBar_HUD*) g_WndMng.m_pWndTaskBar)->Rotate( );
//			if( g_WndMng.m_pWndActionSlotHUD )
//				g_WndMng.m_pWndActionSlotHUD->Rotate( );
		}
		else if( g_bKeyTable[ '7' ] )
		{
			g_bKeyTable[ '7' ] = FALSE;
			if( g_WndMng.m_pWndTaskBar )
				((CWndTaskBar_HUD*) g_WndMng.m_pWndTaskBar)->Rewind( );
//			if( g_WndMng.m_pWndActionSlotHUD )
//				g_WndMng.m_pWndActionSlotHUD->Reset( );
		}
		else if( g_bKeyTable[ '8' ] )
		{
			g_bKeyTable[ '8' ] = FALSE;
			BOOL CALLBACK NPGameMonCallback( DWORD dwMsg, DWORD dwArg );
			NPGameMonCallback( 0, 0 );

		}
		else if ( g_bKeyTable[ 'P' ] )
		{
			g_bKeyTable[ 'P' ] = FALSE;

#ifdef _DEBUG
			if( g_Console._bTestMTE == FALSE )
			{
				g_pPlayer->DeleteEnchantEffect_NEW( 1 );	
				g_Console._bTestMTE = TRUE;
			}
			else
			{
				//원래는 mdldyna.inc에 등록이 된걸 만들어야 하지만 테스트이기때문에 파일이름으로 불러들임
				g_pPlayer->CreateEnchantEffect_NEW( 1, "fly_sfx_fire_level5" );
				g_Console._bTestMTE = FALSE;
			}
#endif
		}
		else if( g_bKeyTable[ '7' ] )
		{
			if( !g_WndMng.m_pWndColosseumReadyToStart )
			{
				g_WndMng.m_pWndColosseumReadyToStart = new CWndColosseumReadyToStart;
				g_WndMng.m_pWndColosseumReadyToStart->Initialize( &g_WndMng );
			}
			else
				SAFE_DELETE( g_WndMng.m_pWndColosseumReadyToStart );

			g_WndMng.OpenMessageBox( prj.GetText(TID_COLOSSEUM_JINBOSS01), MB_OKCANCEL, g_WndMng.m_pWndColosseumReadyToStart );	//진보스 확률 업이다 진짜 사용할래?
		}
		else if( GetAsyncKeyState( VK_ADD ) & 0x80000 )
		{
			g_Console._fTestMTE += 0.02f;
			if( g_Console._fTestMTE > 3.0f )
				g_Console._fTestMTE = 3.0f;
		}
		else if( GetAsyncKeyState( VK_SUBTRACT ) & 0x80000 )
		{
			g_Console._fTestMTE -= 0.02f;
			if( g_Console._fTestMTE < 1.0f )
				g_Console._fTestMTE = 1.0f;
		}
		
	}

 	static DWORD sDelta = 0;
 	static DWORD sOld = timeGetTime( );

	if( g_Console._bRandomMoving )
		RandomMoving( _GetDeltaTime() );
}

void _update_console2( )
{
#ifdef __INTERNALSERVER
	//hj test code here

	if( GetAsyncKeyState( VK_CONTROL ) & 0x80000 )
	{
		if( g_bKeyTable[ 'Z' ] )
		{	
			switch( _GetContentState(CT_SHOP_CART_17) )
			{
			case CS_VER1:
				{
					GetCS()->SetContentState(CT_SHOP_CART_17,CS_VER2);
					g_Console.Log( "장바구니 : New" );
				}
				break;
			case CS_VER2:
				{
					GetCS()->SetContentState(CT_SHOP_CART_17,CS_VER1);
					g_Console.Log( "장바구니 : Old" );
				}
				break;
			}
		}
		else if( g_bKeyTable[ 'B' ] )
		{
			switch(_GetContentState(CT_CANCELLATION))
			{
			case CS_VER1:
				GetCS()->SetContentState(CT_CANCELLATION,CS_NOT_SUPPORTED );
				g_Console.Log( "귀속해제 : NOT_SUPPORTED" );
				break;
			default:
				GetCS()->SetContentState(CT_CANCELLATION,CS_VER1);
				g_Console.Log( "귀속해제 : VER1" );
				break;
			}
		}
		else if( g_bKeyTable[ 'I' ] )
        {
			switch(_GetContentState(CT_NEW_INVENTORY))
			{
			case CS_VER1:
				GetCS()->SetContentState(CT_NEW_INVENTORY,CS_VER2);
				g_Console.Log( "인벤확장 : VER_2" );
				break;
			case CS_VER2:
				GetCS()->SetContentState(CT_NEW_INVENTORY,CS_VER1);
				g_Console.Log( "인벤확장 : VER1" );
				break;
			default:
				FLASSERT(0);
				break;

			}
		}
		else if( g_bKeyTable[ 'C' ] )
		{
			switch(_GetContentState(CT_ITEM_CHARGE))
			{
			case CS_VER1:
				GetCS()->SetContentState(CT_ITEM_CHARGE,CS_NOT_SUPPORTED);
				g_Console.Log( "아이템 유료화 : NOT_SUPPORTED" );
				break;
			default:
				GetCS()->SetContentState(CT_ITEM_CHARGE,CS_VER1);
				g_Console.Log( "아이템 유료화 : VER1" );
				break;
			}
		}
		else if( g_bKeyTable[ 'R' ] )
		{
			switch(_GetContentState(CT_BARUNA_PEARCING))
			{
			case CS_VER1:
				GetCS()->SetContentState(CT_BARUNA_PEARCING,CS_NOT_SUPPORTED);
				g_Console.Log( "바루나 피어싱 : NOT_SUPPORTED" );
				break;
			default:
				GetCS()->SetContentState(CT_BARUNA_PEARCING,CS_VER1);
				g_Console.Log( "바루나 피어싱 : VER1" );
				break;
			}
		}
	}

#endif //INTERNALSERVER
}


#endif //__BS_CONSOLE
