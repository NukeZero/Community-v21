#include "stdafx.h"
#include "region.h"
#include "WorldMng.h"
#include "lang.h"
#include "respawn.h"
#include "PathObj.h"

#ifdef __CLIENT
#include "resdata.h"
#endif	// __CLIENT

#ifdef __WORLDSERVER
#include "../WorldServer/user.h"
#endif	// __WORLDSERVER

extern	CWorldMng	g_WorldMng;
#ifdef	__YENV	
extern float s_fFogStart;
extern float s_fFogEnd;
#endif //__YENV
#include "InstanceDungeonBase.h"


int				CWorld::s_nLandWidthMax = 0;
int				CWorld::s_nLandHeightMax = 0;


BOOL CWorld::OpenWorld( OBJID idWorld, BOOL bDir )
{
	LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( idWorld );
	FLASSERT( lpWorld->IsValid() );
	return( OpenWorld( lpWorld->m_szFileName, bDir ) );
}

const char* DEFAULT_FNAME = "default";

BOOL CWorld::OpenWorld( LPCTSTR lpszFileName, BOOL bDir )
{
	TCHAR drive[_MAX_DRIVE] = { 0, }, dir[_MAX_DIR] = { 0, }, name[ _MAX_FNAME ] = { 0, }, ext[_MAX_EXT] = { 0, };
	TCHAR szPathName[ _MAX_PATH ] = { 0, };

	if( bDir == FALSE )
	{
		FLStrcpy( szPathName, _countof( szPathName ), lpszFileName );
	}
	else
	{
		FLSplitPath( lpszFileName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
		FLStrcpy( szPathName, _countof( szPathName ), lpszFileName );
		FLStrcat( szPathName, _countof( szPathName ), "\\" );
		FLStrcat( szPathName, _countof( szPathName ), name );
		FLStrcat( szPathName, _countof( szPathName ), ".wld" );
	}

	CScript script;	
	if( script.Load( szPathName ) == FALSE )
		return FALSE;

	bool bLoaded_MPU = false;

	do 
	{
		script.GetToken();
		if( script.Token == _T( "size" ) )
		{
			m_nLandWidth = script.GetNumber();
			script.GetToken();
			m_nLandHeight = script.GetNumber();
			CalcBound();

			if( m_nLandWidth < 0 || m_nLandHeight < 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, "[ INVALID MAP size : m_nLandWidth(%d) m_nLandHeight(%d) ]", m_nLandWidth, m_nLandHeight );
				RELEASE_BREAK;
			}

			s_nLandWidthMax			= max( s_nLandWidthMax, m_nLandWidth );
			s_nLandHeightMax		= max( s_nLandHeightMax, m_nLandHeight );
		}
		else
		if( script.Token == _T( "ambient" ) )
		{
		#ifndef __WORLDSERVER
			m_dwAmbient = script.GetHex();
		#endif	// __WORLDSERVER
		}
		else
		if( script.Token == _T( "revival" ) )
		{
			m_dwIdWorldRevival = script.GetNumber();
			script.GetToken();
			FLStrcpy( m_szKeyRevival, _countof( m_szKeyRevival ), script.token );
		}
		else
		if( script.Token == _T( "indoor" ) )
		{
			m_bIsIndoor = script.GetNumber();
		#if defined( __CLIENT ) 
			m_bViewSkybox = m_bViewWeather = !m_bIsIndoor;
		#endif //__CLIENT
		}
		else
		if( script.Token == _T( "fly" ) )
		{
			m_bFly = script.GetNumber();
		}
		else
		if( script.Token == _T( "bgm" ) )
		{
			m_dwIdMusic = script.GetNumber();
		}
		else
		if( script.Token == _T( "pkmode" ) )
		{
			m_nPKMode = script.GetNumber();
		}
		else
		if( script.Token == _T( "diffuse" ) )
		{
			m_dwDiffuse = script.GetHex();
		}
		else
		if( script.Token == _T( "lightDir" ) )
		{
			m_v3LightDir.x = script.GetFloat();
			m_v3LightDir.y = script.GetFloat();
			m_v3LightDir.z = script.GetFloat();
		}
		else
		if( script.Token == _T( "fogSetting" ) )
		{
		#if defined( __CLIENT )
			m_fFogStartValue = script.GetFloat();
			m_fFogEndValue   = script.GetFloat();
			m_fFogDensity    = script.GetFloat();
			
			#ifdef __YENV			
			s_fFogStart      = m_fFogStartValue;	// 셰이더 넘기기용
			s_fFogEnd        = m_fFogEndValue;		// 셰이더 넘기기용
			#endif //__YENV
		#endif //__CLIENT
		}			
		else 
		if( script.Token == __T("MPU") )
		{
			bLoaded_MPU = true;
			g_MPU = script.GetNumber( );
		}
		else
		if( script.Token == __T( "sky" ) )
		{
			for( int i=0; i<3; ++i)
			{
				script.GetToken();
				if( DEFAULT_FNAME != script.Token )
				{
					m_kWorldEnvir._kSky[i]._bUse = TRUE;
					m_kWorldEnvir._kSky[i]._strFileName = script.token;
				}
			}
			
		}
		else
		if( script.Token == __T( "cloud" ) )
		{
			for( int i=0; i<3; ++i)
			{
				script.GetToken();
				if( DEFAULT_FNAME != script.Token )
				{
					m_kWorldEnvir._kCloud[i]._bUse = TRUE;
					m_kWorldEnvir._kCloud[i]._strFileName = script.token;
				}
			}
		}
		else
		if( script.Token  == __T( "sun" ) )
		{
			script.GetToken();
			if( DEFAULT_FNAME != script.Token )
			{
				m_kWorldEnvir._kSun._bUse = TRUE;
				m_kWorldEnvir._kSun._strFileName = script.token;
			}
		}
		else
		if( script.Token == __T( "moon" ) )
		{
			script.GetToken();
			if( DEFAULT_FNAME != script.Token )
			{
				m_kWorldEnvir._kSun._bUse = TRUE;
				m_kWorldEnvir._kSun._strFileName = script.token;
			}
		}
		else
		if( script.Token == __T( "use24light" ) )
		{
			m_bUsing24Light = script.GetNumber( );
		}
#ifdef __BS_CHANGEABLE_WORLD_SEACLOUD
		else
		if( script.Token == __T( "seacloud" ) )
		{
			script.GetToken( );
			if( DEFAULT_FNAME != script.Token )
			{
				m_kSeaCloud._bUse = TRUE;
				m_kSeaCloud._strFileName = script.token;
			}
		}
#endif //__BS_CHANGEABLE_WORLD_SEACLOUD
	} while( script.tok != FINISHED );

	if( !bLoaded_MPU )
		g_MPU = OLD_MPU;

#ifdef __CLIENT
	m_nVisibilityLand	= (int)( m_fFarPlane / ( MAP_SIZE * MPU ) );
#endif

	// gmpbigsun:여기서 최초 MPU를 변경, 이곳에서 MPU값을 OLD_MPU로 설정하면 4로 고정되어 예전과 같음.
	m_iMPU = g_MPU;						// __DYNAMIC_MPU가 설정된 경우 ".wld"파일에서 읽은 MPU값을 사용한다.!!!!!!!!!!!!!!!!!!!!!!!!

	// 파일명 보관 
	FLSplitPath( szPathName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
	FLStrcpy( m_szFilePath, _countof( m_szFilePath ), dir );
	FLStrcpy( m_szFileName, _countof( m_szFileName ), name );
	FLStrcat( m_szFileName, _countof( m_szFileName ), ext );

#ifdef __WORLDSERVER
	FLStrcpy( m_lpszWorld, _countof( m_lpszWorld ), m_szFileName );

	int nView = 1;								// 시야 1 = 64미터 
	if( m_dwWorldID == WI_WORLD_GUILDWAR )	// 길드대전 맵은 시야 2배 
		nView = 2;

	nView = nView + ( OLD_MPU - m_iMPU );

	m_linkMap.Init( m_nLandWidth, m_nLandHeight, nView, TRUE, m_iMPU );
	m_apHeightMap = new float[ WORLD_WIDTH * WORLD_HEIGHT ];
	memset( m_apHeightMap, 0, sizeof( float ) * WORLD_WIDTH * WORLD_HEIGHT );
	m_apWaterHeight = new WATERHEIGHT[ NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight ];
	memset( m_apWaterHeight, 0, sizeof( WATERHEIGHT) * NUM_PATCHES_PER_SIDE * m_nLandWidth * NUM_PATCHES_PER_SIDE * m_nLandHeight );

	//FLINFO_LOG( PROGRAM_NAME, _T( "[ MAP WIDTH WidthMAX(%d) HeightMAX(%d) ]" ), s_nLandWidthMax, s_nLandHeightMax );
	
#else	// __WORLDSERVER
	m_apLand = new CLandscape * [ m_nLandWidth * m_nLandHeight ];
	memset( m_apLand, 0, sizeof( CLandscape*) * m_nLandWidth * m_nLandHeight );
	LoadRegion();

	InitWorldEnvir( szPathName );
	InitContinent( szPathName );

	

#endif	// __WORLDSERVER


	return TRUE;
}

#ifdef __WORLDSERVER


BOOL CWorld::LoadPatrol()
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.pat", m_szFilePath, strLandName );
	return CPatrolPath::GetInstance()->LoadPatrol( strLandTemp );
}

#ifdef __LAYER_1021
void CWorld::DeleteLayerControls( int nLayer )
{
	CObj* pObj;
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		pObj	= m_apObject[i];
		if( pObj && pObj->GetType() == OT_CTRL && pObj->GetLayer() == nLayer )
			pObj->Delete();
	}
}

#endif	// __LAYER_1021

// 동적 오브젝트 스크립트 로드
#ifdef __LAYER_1021
BOOL CWorld::LoadObject( int nLayer )
#else	// __LAYER_1021
BOOL CWorld::LoadObject()
#endif	// __LAYER_1021
{
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.dyo", m_szFilePath, strLandName );
	CResFile file;

	if( file.Open( strLandTemp,"rb" ) == FALSE )
		return FALSE;

	while( 1 )
	{
		CObj* pObj = ::ReadObj( &file );
		if( pObj == NULL )
			break;

		D3DXVECTOR3 vPos = pObj->GetPos();
		pObj->SetPos( vPos );
		pObj->SetIndex( NULL, pObj->m_dwIndex );
		if( pObj->GetType() == OT_MOVER )
		{
			pObj->InitMotion( ((CMover *)pObj)->m_dwMotion );
//			FLERROR_LOG( "CHARACTER_POS", "%s\t( \"%s\", %d, %4.0f, %4.0f );\t%4.0f", strLandName, static_cast<CMover*>(pObj)->m_szCharacterKey, m_dwWorldID, vPos.x, vPos.z, vPos.y );
		}
		pObj->UpdateLocalMatrix();
		
		if( IsUsableDYO( pObj ) )
		{
			ADDOBJ( pObj, TRUE, nLayer );
//sun: 13, 퀘스트 NPC위치 알림
			if (pObj->GetType() == OT_MOVER)
			{
				if (tagCHARACTER* lpChar = static_cast<CMover*>(pObj)->GetCharacter())
				{
					if (lstrlen(lpChar->m_szKey))
					{
						lpChar->m_dwWorldId = GetID();
						lpChar->m_vPos = pObj->GetPos();
					}
				}
			}

		}
		else
		{
			SAFE_DELETE( pObj );
			continue;
		}
	} 
	
	// __XSHIPBUS
#ifdef __IAOBJ0622	
	if( GetID() == WI_WORLD_MADRIGAL )	// 마드리갈 읽을때 테스트비공정띄우기
	{
		CObj *pObj	= CreateObj( D3DDEVICE, OT_SHIP, 4 );
		D3DXVECTOR3 vPos = D3DXVECTOR3( 6997.2f, 124.2f, 3243.7f );
		pObj->SetPos( vPos );
		pObj->SetAngle( 180.0f );
		if( ADDOBJ( pObj, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pObj );
		}

		CObj *pObj2	= CreateObj( D3DDEVICE, OT_SHIP, 4 );
		D3DXVECTOR3 vPos2 = D3DXVECTOR3( 7508.74f, 90.2f, 3168.28f );
		pObj2->SetPos( vPos2 );
		pObj2->SetAngle( 180.0f );
		if( ADDOBJ( pObj2, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pObj2 );
		}

		CObj *pObj3	= CreateObj( D3DDEVICE, OT_SHIP, 1 );
		D3DXVECTOR3 vPos3 = D3DXVECTOR3( 7528.74f, 90.2f, 3178.28f );
		pObj3->SetPos( vPos3 );
		pObj3->SetAngle( 180.0f );
		if( ADDOBJ( pObj3, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pObj3 );
		}

		CObj *pObj4	= CreateObj( D3DDEVICE, OT_SHIP, 2 );
		D3DXVECTOR3 vPos4 = D3DXVECTOR3( 7558.74f, 90.2f, 3188.28f );
		pObj4->SetPos( vPos4 );
		pObj4->SetAngle( 180.0f );
		if( ADDOBJ( pObj4, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pObj4 );
		}

		CObj *pObj5	= CreateObj( D3DDEVICE, OT_SHIP, 3 );
		D3DXVECTOR3 vPos5 = D3DXVECTOR3( 7598.74f, 90.2f, 3198.28f );
		pObj5->SetPos( vPos5 );
		pObj5->SetAngle( 180.0f );
		if( ADDOBJ( pObj5, TRUE, nLayer ) == FALSE )
		{
			SAFE_DELETE( pObj5 );
		}

	}
#endif // IAOBJ0622

	return TRUE;
}
#endif // __WORLDSERVER

// 저장 포맷을 실제 스트링으로 변환 
void RegionDesc_SaveFormatToString( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\\' && *(pwszSrc + 1 )== 'n')
		{
			*(pwszSrc + 0) = '\r';
			*(pwszSrc + 1) = '\n';
		}
		pwszSrc++;
	}
}
// 실제 스트링을 저장 포맷으로 변경 
void RegionDesc_StringToSaveFormat( wchar_t* pwszDesc )
{
	wchar_t* pwszSrc = pwszDesc;
	while( *pwszSrc )
	{
		if( *pwszSrc == '\r' && *(pwszSrc + 1 )== '\n')
		{
			*(pwszSrc + 0) = '\\';
			*(pwszSrc + 1) = 'n';
		}
		pwszSrc++;
	}	
}

BOOL CWorld::ReadRegion( CScript& s )
{
	BOOL bNewFormat = FALSE;
	BOOL bNewFormat3 = FALSE;
	if( s.Token == _T( "region2" ) )
		bNewFormat = TRUE;
	if( s.Token == _T( "region3" ) )
	{
		bNewFormat3 = TRUE;
		bNewFormat = TRUE;
	}
	DWORD dwType = s.GetNumber();
	UNREFERENCED_PARAMETER( dwType );

	DWORD dwIndex = s.GetNumber();
	D3DXVECTOR3 vPos;
	vPos.x = s.GetFloat();
	vPos.y = s.GetFloat();
	vPos.z = s.GetFloat();

	REGIONELEM re,* pRe = &re;
	ZeroMemory( &re, sizeof( re ) );

	pRe->m_uItemId		= 0xffffffff;
	pRe->m_uiItemCount	= 0xffffffff;
	pRe->m_uiMinLevel	= 0xffffffff;
	pRe->m_uiMaxLevel	= 0xffffffff;
	pRe->m_iQuest		= 0xffffffff;
	pRe->m_iQuestFlag	= 0xffffffff;
	pRe->m_iJob			= 0xffffffff;
	pRe->m_iGender		= 0xffffffff;
	pRe->m_bCheckParty	= FALSE;
	pRe->m_bCheckGuild	= FALSE;
	pRe->m_bChaoKey		= FALSE;
	pRe->m_dwAttribute	= s.GetNumber();
	pRe->m_dwIdMusic = s.GetNumber();
	pRe->m_bDirectMusic = (bool)(s.GetNumber() == 1);

	s.GetToken(); FLStrcpy( pRe->m_szScript, _countof( pRe->m_szScript ), s.token );
	s.GetToken(); FLStrcpy( pRe->m_szSound, _countof( pRe->m_szSound), s.token );
	pRe->m_dwIdTeleWorld = s.GetNumber();

	pRe->m_vPos        = vPos;
	pRe->m_vTeleWorld.x = s.GetFloat();
	pRe->m_vTeleWorld.y = s.GetFloat();
	pRe->m_vTeleWorld.z = s.GetFloat();
	pRe->m_rect.left   = (LONG)s.GetNumber();
	pRe->m_rect.top	   = (LONG)s.GetNumber();
	pRe->m_rect.right  = (LONG)s.GetNumber();
	pRe->m_rect.bottom = (LONG)s.GetNumber();
	s.GetToken();
	FLStrcpy( pRe->m_szKey, _countof( pRe->m_szKey ), s.Token );
	pRe->m_bTargetKey  =  (bool)(s.GetNumber() == 1);
	if( bNewFormat3 )
	{
		pRe->m_uItemId		= (u_long)s.GetNumber();
		pRe->m_uiItemCount	= (UINT)s.GetNumber();
		pRe->m_uiMinLevel	= (UINT)s.GetNumber();
		pRe->m_uiMaxLevel	= (UINT)s.GetNumber();
		pRe->m_iQuest		= s.GetNumber();
		pRe->m_iQuestFlag	= s.GetNumber();
		pRe->m_iJob			= s.GetNumber();
		pRe->m_iGender		= s.GetNumber();
		pRe->m_bCheckParty	= (BOOL)s.GetNumber();
		pRe->m_bCheckGuild	= (BOOL)s.GetNumber();
		pRe->m_bChaoKey		= (BOOL)s.GetNumber();
	}

	if( bNewFormat == FALSE )
	{
		pRe->m_cDescSize = (char)( (LONG)s.GetNumber() );
		if( pRe->m_cDescSize )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			FLStrcpy( pRe->m_szDesc, _countof( pRe->m_szDesc ), s.Token );	

			s.GetToken();	// }

		}
	}
	else
	{
		s.GetToken(); // get "title"
		BOOL bDesc = s.GetNumber();
		if( bDesc )
		{
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			FLStrcpy( pRe->m_szTitle, _countof( pRe->m_szTitle ), s.Token );

			s.GetToken();	// }
		}

		s.GetToken(); // get "desc"
		bDesc = s.GetNumber();
		if( bDesc )
		{
			pRe->m_cDescSize = 1;
			s.GetToken(); // {
			s.GetToken();

			s.Token.Replace( "\\n", "\r\n" );
			FLStrcpy( pRe->m_szDesc, _countof( pRe->m_szDesc ), s.Token );

			s.GetToken(); // }
		}
	}

	
	if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR )	//__JAPAN_PVP
		pRe->m_dwAttribute &= ~RA_FIGHT;

	switch( dwIndex )
	{
	case REGION_INDEX( 12, RI_BEGIN ):
	case REGION_INDEX( 13, RI_REVIVAL ):
		break;
	case REGION_INDEX( 14, RI_STRUCTURE ):
		m_aStructure.AddTail( pRe );
		break;
	default:
		{
			m_aRegion.AddTail( pRe );	
			break;
		}
	}
	return TRUE;
}

BOOL CWorld::ReadRespawn( CScript& s )
{
	int nVersion = 1;
	if( s.Token == _T( "respawn2" ) )
		nVersion = 2;
	if( s.Token == _T( "respawn3" ) )
		nVersion = 3;
	if( s.Token == _T( "respawn4" ) )
		nVersion = 4;	
	if( s.Token == _T( "respawn5" ) )
		nVersion = 5;	
	if( s.Token == _T( "respawn6" ) )
		nVersion = 6;
	if( s.Token == _T( "respawn7" ) )		//sun: 10차 전승시스템	Neuz, World, Trans
		nVersion = 7;
	if( s.Token == _T( "respawn8" ) )
		nVersion = 8;
	
	CRespawnInfo ri,* pInfo = &ri;
	DWORD dwType;
	DWORD dwIndex;
	D3DXVECTOR3 vPos;
	dwType = s.GetNumber();

	dwIndex = s.GetNumber();
	vPos.x = s.GetFloat();
	vPos.y = s.GetFloat();
	vPos.z = s.GetFloat();
	
	pInfo->m_dwIndex     = dwIndex;
	pInfo->m_dwType	= dwType;

#ifdef __S1108_BACK_END_SYSTEM
	pInfo->m_nMaxcb		 = s.GetNumber();
	pInfo->m_cb          = 0;
#else // __S1108_BACK_END_SYSTEM
	pInfo->m_cb          = (u_short)s.GetNumber();
#endif // __S1108_BACK_END_SYSTEM

	pInfo->m_uTime = (short)s.GetNumber();

#ifdef __S1108_BACK_END_SYSTEM
	pInfo->m_nMaxAttackNum = (long)s.GetNumber();
	pInfo->m_nActiveAttackNum = 0;
#else // __S1108_BACK_END_SYSTEM
	pInfo->m_nActiveAttackNum = (long)s.GetNumber();
#endif // __S1108_BACK_END_SYSTEM

	pInfo->m_vPos	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	pInfo->m_fY = vPos.y;
	if( dwType == OT_MOVER )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( dwIndex );
#ifdef __WORLDSERVER
		if( pMoverProp )
		{
			// PK서버가 아니면 가드를 skip한다.
			if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )	// 한국의 경우 skip하지 않는다.
				if( pMoverProp->dwClass == RANK_GUARD && g_eLocal.GetState( EVE_PK ) == FALSE ) 
					return TRUE;
			
			if( pMoverProp->dwClass == RANK_MATERIAL )
				return TRUE;

			if( pMoverProp->dwClass != RANK_BOSS )
			{
				switch( pMoverProp->dwID )
				{
					case OBJECT_INDEX( 20, MI_AIBATT1 ):
					case OBJECT_INDEX( 21, MI_AIBATT2 ):
					case OBJECT_INDEX( 22, MI_AIBATT3 ):
					case OBJECT_INDEX( 96, MI_MUSHPANG1 ):
					case OBJECT_INDEX( 97, MI_MUSHPANG2 ):
					case OBJECT_INDEX( 98, MI_MUSHPANG3 ):
					case OBJECT_INDEX( 24, MI_BURUDENG1 ):
					case OBJECT_INDEX( 25, MI_BURUDENG2 ):
					case OBJECT_INDEX( 26, MI_BURUDENG3 ):
						pInfo->m_uTime	= (u_short)( pInfo->m_uTime	* 0.5 );
						break;
					default:
						pInfo->m_uTime	= (u_short)( pInfo->m_uTime * 0.7 );
						break;
				}						
			}
		if( pMoverProp->dwFlying )
			pInfo->m_vPos	= vPos;
		}
#endif	// __WORLDSERVER
	}
	pInfo->m_rect.left   = (LONG)s.GetNumber();
	pInfo->m_rect.top    = (LONG)s.GetNumber();
	pInfo->m_rect.right  = (LONG)s.GetNumber();
	pInfo->m_rect.bottom = (LONG)s.GetNumber();
	if( nVersion >= 2 )
	{
		pInfo->m_nDayMin = s.GetNumber();
		pInfo->m_nDayMax = s.GetNumber();
		pInfo->m_nHourMin = s.GetNumber();
		pInfo->m_nHourMax = s.GetNumber();
		pInfo->m_nItemMin = s.GetNumber();
		pInfo->m_nItemMax = s.GetNumber();
	}

	if( nVersion >= 4 )
	{		
		pInfo->m_dwAiState = s.GetNumber();
		pInfo->m_fAngle    = s.GetFloat();	
		
		if( pInfo->m_nMaxcb == 1 )
		{
			if( pInfo->m_dwAiState == 8 || pInfo->m_dwAiState == 9 )
			{
				pInfo->m_vPos = vPos;
			}
		}
	}

	if( nVersion >= 5 )
	{		
		pInfo->m_dwPatrolIndex = s.GetNumber();
	}
	
	if( nVersion >= 6 )
	{		
		pInfo->m_bPatrolCycle  = s.GetNumber();
	}

	//_SUN_RESPWANINFO_VER8
	if( nVersion >= 8 )
		pInfo->m_dwAI = s.GetNumber();
	
	if( nVersion >= 3 )
	{	
		int nContrl = s.GetNumber();
		if( nContrl )
		{
			pInfo->m_CtrlElem.m_dwSet = s.GetNumber(); 
			pInfo->m_CtrlElem.m_dwSetItem = s.GetNumber();  
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = s.GetNumber();  
			}
			else
			{
				pInfo->m_CtrlElem.m_dwSetItemCount = 1;
			}
			pInfo->m_CtrlElem.m_dwSetLevel = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetQuestNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwSetFlagNum = s.GetNumber();  
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwSetQuestNum1 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetFlagNum1 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetQuestNum2 = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwSetFlagNum2 = s.GetNumber();  
			}
			pInfo->m_CtrlElem.m_dwSetGender = s.GetNumber();

//sun: 10차 전승시스템	Neuz, World, Trans
			int maxjob = 0;
			if(nVersion <= 6)
				maxjob = 16;
			else
				maxjob = 32;

			for( int i = 0; i < maxjob; i++ )
				pInfo->m_CtrlElem.m_bSetJob[ i ] = s.GetNumber();

			pInfo->m_CtrlElem.m_dwSetEndu = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMinItemNum = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwMaxiItemNum = s.GetNumber();  
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemKind[ i ] = s.GetNumber();  
			for( int i = 0; i < MAX_CTRLDROPITEM; i++ )
				pInfo->m_CtrlElem.m_dwInsideItemPer[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResKind[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonResNum[ i ] = s.GetNumber(); 
			for( int i = 0; i < MAX_CTRLDROPMOB; i++ )
				pInfo->m_CtrlElem.m_dwMonActAttack[ i ] = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapOperType = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapRandomPer = s.GetNumber();  
			pInfo->m_CtrlElem.m_dwTrapDelay = s.GetNumber();  
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapKind[ i ] = s.GetNumber();  
			for( int i = 0; i < MAX_TRAP; i++ )
				pInfo->m_CtrlElem.m_dwTrapLevel[ i ] = s.GetNumber(); 
			
			if( nContrl == 2 )
			{
				pInfo->m_CtrlElem.m_dwTeleWorldId = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleX = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleY = s.GetNumber();  
				pInfo->m_CtrlElem.m_dwTeleZ = s.GetNumber();  
			}
		}
	}

#ifdef __WORLDSERVER
#ifdef __RES0807
	if( FALSE == ri.GenResPoint( this ) )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "type=%d, index=%d, left=%d, top=%d, right=%d, bottom=%d, num=%d" ),
			ri.m_dwType, ri.m_dwIndex, ri.m_rect.left, ri.m_rect.top, ri.m_rect.right, ri.m_rect.bottom, ri.m_nMaxcb );
		return FALSE;
	}
#endif	// __RES0807
	m_respawner.Add( ri );
#endif	// __WORLDSERVER

	return TRUE;
}

BOOL CWorld::LoadRegion()
{
#if defined( __WORLDSERVER ) || defined(__CLIENT)
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	strLandTemp.Format( "%s%s.rgn", m_szFilePath, strLandName );

	CScript s;
	if( s.Load( strLandTemp, FALSE ) )
	{
		D3DXVECTOR3 vPos;
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "region" ) || s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
			{
				ReadRegion( s );
			}
			else
			if( s.Token == _T( "respawn" ) || s.Token == _T( "respawn2" ) || s.Token == _T( "respawn3" ) || s.Token == _T( "respawn4" ) || s.Token == _T( "respawn5" ) || s.Token == _T( "respawn6" ) || s.Token == _T( "respawn7" ))
			{
				ReadRespawn( s );
			}
			s.GetToken();
		}
#ifdef __LAYER_1021
		Expand();
#endif	// __LAYER_1021
		return TRUE;
	}
#endif

	return FALSE;
}


#ifdef __WORLDSERVER
BOOL CWorld::ReadWorld( const CRect & /*rcLandscape*/ )
{
	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );
	FLOAT aHeightMap[ ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) ];
	WATERHEIGHT aWaterHeight[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BYTE aLocation[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	int xLand, yLand;
	D3DXVECTOR3	vPos;

	CObj* pBkgnd;
	DWORD dwObjNum;
	
	for( int y = 0; y < m_nLandHeight; y++ )
	{
		for( int x = 0; x < m_nLandWidth; x++ )
		{
			strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, x , y );

			CFileIO f;
			if( f.Open( strLandTemp, "rb" ) )
			{
				DWORD dwVersion;
				f.Read( &dwVersion, sizeof( DWORD) );
				if( dwVersion >= 1 )
				{
					f.Read( &xLand, sizeof( xLand ) );
					f.Read( &yLand, sizeof( yLand ) );
				}
				f.Read( (void*)aHeightMap, sizeof(FLOAT), ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ) );

				for( int y2 = 0; y2 < MAP_SIZE; y2++ )
				{
					for( int x2 = 0; x2 < MAP_SIZE; x2++ )
					{
						m_apHeightMap[ ( ( ( y * MAP_SIZE ) + y2 ) * WORLD_WIDTH ) + ( ( x * MAP_SIZE ) + x2 ) ]
							= aHeightMap[ ( y2 * ( MAP_SIZE + 1 ) ) + x2 ];
					}
				}
				
				f.Read( (void*)aWaterHeight, sizeof( WATERHEIGHT ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
				for( int y2 = 0; y2 < NUM_PATCHES_PER_SIDE; y2++ )
				{
					for( int x2 = 0; x2 < NUM_PATCHES_PER_SIDE; x2++ )
					{
						m_apWaterHeight[ ( ( ( y * NUM_PATCHES_PER_SIDE ) + y2 ) * ( NUM_PATCHES_PER_SIDE * m_nLandWidth ) ) + ( ( x * NUM_PATCHES_PER_SIDE ) + x2 ) ]
							= aWaterHeight[ ( y2 * ( NUM_PATCHES_PER_SIDE ) ) + x2 ];
					}
				}
				if( dwVersion >= 2 )
				{
					// skip
					f.Read( aLocation, sizeof( BYTE ), NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE );
				}
				BYTE nLayer;
				f.Read( &nLayer, sizeof(BYTE), 1 );
				for( BYTE u = 0; u < nLayer; u++ ) {
					f.Seek( sizeof(WORD), SEEK_CUR );
					f.Seek( ( sizeof(BOOL) * NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ), SEEK_CUR );
					f.Seek( ( sizeof(DWORD) * MAP_SIZE * MAP_SIZE ), SEEK_CUR );
				}

				// read objects
				f.Read( (void*)&dwObjNum, sizeof(DWORD), 1 );
				for( DWORD dw =  0; dw < dwObjNum; dw++ )
				{
					pBkgnd	= ReadObj( &f );
					vPos	= pBkgnd->GetPos();

				//	vPos.x	+= x * LANDREALSCALE;
				//	vPos.z	+= y * LANDREALSCALE;

					vPos.x	+= x * MAP_SIZE * m_iMPU;
					vPos.z	+= y * MAP_SIZE * m_iMPU;

					pBkgnd->SetPos( vPos );

					if( pBkgnd->m_dwIndex <= 10 )
						pBkgnd->SetAngle( (float)( rand() % 360 ) );

					pBkgnd->SetIndex( D3DDEVICE, pBkgnd->m_dwIndex );

					if( pBkgnd->GetType() != OT_OBJ )
					{
						SAFE_DELETE( pBkgnd );
						continue;
					}
					
					if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP && pBkgnd->m_dwIndex == 360 )
					{
						SAFE_DELETE( pBkgnd );
					}
					else
					{
						if( pBkgnd->m_pModel && TRUE == pBkgnd->m_pModel->IsHaveCollObj() )
						{
//							pBkgnd->InitMotion( pBkgnd->m_dwMotion );
							pBkgnd->SetWorld( this );
							InsertObjLink( pBkgnd );
							m_vecBackground.push_back( pBkgnd );
							pBkgnd->UpdateLocalMatrix();
						}
						else
						{
							SAFE_DELETE( pBkgnd );
						}
					}
				}
			}
			f.Close();

			g_WorldMng.AddMapCheckInfo( strLandTemp );
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER

#if defined( __CLIENT ) 

BOOL CWorld::ReadWorld( D3DXVECTOR3 vPos, BOOL bEraseOldLand  )
{
	//gmpbigsun : CWorld::Process중 지워야 할 포인터를 가지고있는 m_apDeleteObjs가 landscape를 새로 읽을 경우 
	//			  초기화 되지 않는 문제로 인해 자주 crash되었는데 개발초기부터 201006까지 수년간 이런문제를 안고 있었던거 같다.
	//            m_nDeleteObjs는 어떠한 이유로든 지워지는 Landsacpe가 있다면 0으로 초기화 되어야 한다. 그 안에 오브젝트는 삭제되므로...
	DeleteObjects();

	CString strFileName;
	CString strLandName = m_szFileName;
	CString strLandTemp;
	strLandName.Delete( strLandName.GetLength() - 4, 4 );


	int x, z;
	WorldPosToLand( vPos, x, z );

	// 랜드 이동이 없다. 리턴. 
	if( x == m_nCharLandPosX && z == m_nCharLandPosZ )
		return FALSE;

	m_nCharLandPosX = x;
	m_nCharLandPosZ = z;
		
	for(int i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for(int j = x - m_nVisibilityLand; j <= ( x + m_nVisibilityLand ); j++)
		{
			if( LandInWorld( j, i ) )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
				if( pLand == NULL )
				{
					//! gmpbigsun : pLand는 할당이 이미 끝난 녀석이다 왜 지우지 않았나;;;
					// delete pLand;

					strLandTemp.Format( "%s%s%02d-%02d.lnd", m_szFilePath, strLandName, j , i );
					pLand = new CLandscape;
					pLand->m_nWorldX = j * MAP_SIZE;
					pLand->m_nWorldY = i * MAP_SIZE;
					pLand->InitDeviceObjects( m_pd3dDevice, this );
					if( pLand->LoadLandscape( strLandTemp, j, i ) == FALSE )
					{
						SAFE_DELETE( pLand );
						continue;
					}
					pLand->RestoreDeviceObjects( m_pd3dDevice );

					m_apLand[ i * m_nLandWidth + j] = pLand;

					g_WorldMng.AddMapCheckInfo( strLandTemp );
				}
			}
		}
	}

	if( bEraseOldLand )
	{
		// 현재 보이지 않는 랜드는 파괴 읽을 때는 3*3을 읽고, 파괴는 현재 위치에서 4*4를 벗어난 것들.
		// 만약 파괴할 때 3*3를 벗어난 것을 파괴하면 캐릭터는 조금만 움직여도 재로딩을 하게 될지 모르기 때문이다.
		// 즉, 3*3을 벗어난 것을 파괴하게 되면 늘 3*3 정도가 로딩을 유지하게 되는데, 
		// 그렇게 되면 로딩 빈도가 잦아져 속도가 느려지게 되는 것이다. 로딩 데이타를 유지하는 것은
		// 4*4로 하여 로딩 빈도를 낮춘다.
		CRect rect( x - m_nVisibilityLand - 1, z - m_nVisibilityLand - 1, x + m_nVisibilityLand + 2, z + m_nVisibilityLand + 2 );
		for( int i = 0; i < m_nLandHeight; i++ )
		{
			for( int j = 0; j < m_nLandWidth; j++ )
			{
				CLandscape* pLand = m_apLand[ i * m_nLandWidth + j];
				if( pLand )
				{
					if( rect.PtInRect( CPoint( j, i ) ) == FALSE )
					{
						m_apLand[ i * m_nLandWidth + j ]->InvalidateDeviceObjects();
						m_apLand[ i * m_nLandWidth + j ]->DeleteDeviceObjects();
						SAFE_DELETE( m_apLand[ i * m_nLandWidth + j] );
						m_nObjCullSize = 0;
						m_nSfxCullSize = 0;
					}
					else
						pLand->SetUsedAllObjects();
				}
				
			}			
		}
		// 사용하지 않는 모델을 파괴한다.
		// 정적인것만 이리로 들어와야 한다. 동적인것(스키닝을 쓰는것)이 여기서 파괴되면 안된다.
		for( MapStrToPtrItor itor = prj.m_modelMng.m_mapFileToMesh.begin(); itor != prj.m_modelMng.m_mapFileToMesh.end(); /*itor++*/ )
		{
			CModel* pModel = ( CModel* )(*itor).second;
			pModel->DeleteDeviceObjects();
			if( pModel->m_pModelElem->m_bUsed == FALSE &&  pModel->m_pModelElem->m_dwType != OT_ITEM )
			{
				SAFE_DELETE( pModel );
				prj.m_modelMng.m_mapFileToMesh.erase( itor++ );
			}
			else
			{
				pModel->m_pModelElem->m_bUsed = FALSE;
				++itor;
			}
		}
	}
	return TRUE;
}

void	CWorld::DeleteObjects()
{
	for( int i = 0; i < m_nDeleteObjs; i++ )
	{
		CObj* pObj = m_apDeleteObjs[ i ];
		if( !pObj )
		{
			FLASSERT( FALSE );
			FLERROR_LOG( PROGRAM_NAME, _T( "m_apDeleteObjs %d is NULL" ), i );
			continue;
		}

		if( m_pObjFocus == pObj )
			SetObjFocus( NULL );

		//sun: 9, 9-10차 펫
		CWndWorld* pWndWorld	= (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		if( pWndWorld )
		{
			if(pWndWorld->m_pSelectRenderObj == pObj)
				pWndWorld->m_pSelectRenderObj = NULL;
			else if(pWndWorld->m_pNextTargetObj == pObj)
				pWndWorld->m_pNextTargetObj = NULL;
		}

		if( CObj::m_pObjHighlight == pObj )
			CObj::m_pObjHighlight = NULL;
		// 화면에 출력되고 있는 오브젝트인가.
		if( pObj->m_ppViewPtr )					//sun : (가끔)pObj->m_ppViewPtr이 이미 지워진 상태다 문제가 많군 제길
		{										//오브젝트 삭제과정에 문제가 있다. 어디선가 꼬이고 있다 추적하기에 시간과 의욕이 없다.
			// 그렇다면 화면 출력 배열에서 자신을 삭제 
			*pObj->m_ppViewPtr = NULL;	
			pObj->m_ppViewPtr = NULL;
		}

		RemoveObjLink( pObj );
		RemoveObjArray( pObj );
		SAFE_DELETE( pObj );
	}

	m_nDeleteObjs = 0;
	memset( m_apDeleteObjs, 0, sizeof( m_apDeleteObjs ) );
}


#endif // __CLIENT


/*
#ifdef __IVERSION_NODARKON
				// x가 11 이하면 완전히 갈 수 없도록 하자.
				if( x <= 11 || ( x == 12 && y == 9 ) || ( x == 12 && y == 8 ) )
				{
					for( int i = 0; i < ( MAP_SIZE + 1 ) * ( MAP_SIZE + 1 ); i++ )
					{
						if( aHeightMap[ i ] >= HGT_DIE )
							aHeightMap[ i ] -= HGT_DIE;
						else
						if( aHeightMap[ i ] >= HGT_NOMOVE )
							aHeightMap[ i ] -= HGT_NOMOVE;
						else
						if( aHeightMap[ i ] >= HGT_NOFLY )
							aHeightMap[ i ] -= HGT_NOFLY;
						else
						if( aHeightMap[ i ] >= HGT_NOWALK )
							aHeightMap[ i ] -= HGT_NOWALK;

						aHeightMap[ i ] += HGT_NOMOVE;
					}
				}
				if( x == 12 && y == 7 ) // 대각선 
				{
					int x2max = 0;
					for( int y2  = 0; y2 < ( MAP_SIZE + 1 ); y2++ )
					{
						for( int x2  = 0; x2 < x2max; x2++ )
						{
							int i = y2 * ( MAP_SIZE + 1 ) + x2;
							if( aHeightMap[ i ] >= HGT_DIE )
								aHeightMap[ i ] -= HGT_DIE;
							else
							if( aHeightMap[ i ] >= HGT_NOMOVE )
								aHeightMap[ i ] -= HGT_NOMOVE;
							else
							if( aHeightMap[ i ] >= HGT_NOFLY )
								aHeightMap[ i ] -= HGT_NOFLY;
							else
							if( aHeightMap[ i ] >= HGT_NOWALK )
								aHeightMap[ i ] -= HGT_NOWALK;

							aHeightMap[ i ] += HGT_NOMOVE;
						}
						x2max++;
						if( x2max >= ( MAP_SIZE + 1 ) ) 
							x2max = ( MAP_SIZE + 1 );
					}
				}
#endif
*/

#ifdef __WORLDSERVER
BOOL CWorld::IsUsableDYO( CObj* pObj )
{
	if( pObj->GetType() == OT_MOVER )
	{
		LPCHARACTER pCharacter	= ( (CMover*)pObj )->GetCharacter();
		if( pCharacter )
		{
			// pk
			if( _stricmp( pCharacter->m_szKey, "Npc_Reward" ) == 0
				|| _stricmp( pCharacter->m_szKey, "MaMa_PKNPC01" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_PK ) == FALSE )
					return FALSE;
			}
			// 길드 대전
			if( _stricmp( pCharacter->m_szKey, "MaFl_GuildWar" ) == 0
					|| _stricmp( pCharacter->m_szKey, "MaFl_Donaris" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_GUILDCOMBAT ) == FALSE )
					return FALSE;
			}
//sun: 11, 일대일 길드 대전
 			if( _stricmp( pCharacter->m_szKey, "MaFl_Annie" ) == 0
 					|| _stricmp( pCharacter->m_szKey, "MaFl_Amos" ) == 0 )
 			{
 				if( g_eLocal.GetState( EVE_GUILDCOMBAT1TO1 ) == FALSE )
 					return FALSE;
 			}

			if( _stricmp( pCharacter->m_szKey, "MaFl_Ray" ) == 0 )
 			{
 				if( g_eLocal.GetState( EVE_ARENA ) == FALSE )
 					return FALSE;
 			}

			if( _stricmp( pCharacter->m_szKey, "MaFl_SecretRoom_EAST" ) == 0
				|| _stricmp( pCharacter->m_szKey, "MaDa_SecretRoom_WEST" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_SECRETROOM ) == FALSE )
					return FALSE;
			}

// 			// don't load designer & artist in taiwan & japan
// 			// 일본, 대만에서 제외
// 			if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN || g_xFlyffConfig->GetMainLanguage() == LANG_HK )// || g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
// 			{
// 				if( _stricmp( pCharacter->m_szKey, "MaFl_HairShop" ) == 0
// 					|| _stricmp( pCharacter->m_szKey, "MaFl_FaceOff" ) == 0 )
// 					return FALSE;
// 			}

			// 13차 레인보우 레이스 - 진행 채널에서만 표시하게...
			if( _stricmp( pCharacter->m_szKey, "MaFl_RainbowStart" ) == 0 )
			{
				if( g_eLocal.GetState( EVE_RAINBOWRACE ) == FALSE )
					return FALSE;
			}

			return IsUsableDYO2( pCharacter );
		}
	}
	return TRUE;
}

// 특정 국가만 출력하도록 스크립트에서 설정[character.inc - SetLang(LANG_...)]
BOOL CWorld::IsUsableDYO2( LPCHARACTER pCharacter )
{
	if( pCharacter->m_vecdwLanguage.empty() )
		return pCharacter->bOutput;
	
	for( DWORD i=0; i<pCharacter->m_vecdwLanguage.size(); i++ )
	{
		if( HIWORD(pCharacter->m_vecdwLanguage[i]) == g_xFlyffConfig->GetMainLanguage() )
			return pCharacter->bOutput;
	}

	return !(pCharacter->bOutput);
}
#endif	// __WORLDSERVER

#ifdef __LAYER_1021
BOOL CWorld::CreateLayer( int nLayer )
{
//	FLASSERT( IsExpandable() );
//	if( !IsExpandable() )
//		return FALSE;
	FLASSERT( nLayer != 0 );
	if( nLayer == 0 )
		return FALSE;
	if( !m_linkMap.CreateLinkMap( nLayer ) )
		return FALSE;
	if( !LoadObject( nLayer ) )
	{
		m_linkMap.DeleteLinkMap( nLayer );
		return FALSE;
	}
	m_respawner.Expand( nLayer );
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
BOOL CWorld::HasNobody( int nLayer )
{
	if( !HasNobody_Process( nLayer ) )
		return FALSE;
	if( !HasNoObj_Add( nLayer ) )
		return FALSE;
	if( !HasNobody_Replace( nLayer ) )
		return FALSE;
	return TRUE;
}

BOOL CWorld::HasNobody_Process( int nLayer )
{
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		if( IsLayerPlayer( m_apObject[i], nLayer ) )
			return FALSE;
	}
	return TRUE;
}

BOOL CWorld::HasNoObj_Add( int nLayer )
{
	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		if( m_apAddObjs[i] && m_apAddObjs[i]->GetLayer() == nLayer )
			return FALSE;
	}
	return TRUE;
}

BOOL CWorld::HasNobody_Replace( int nLayer )
{
	return g_WorldMng.HasNobody_Replace( GetID(), nLayer );
//	return TRUE;
}

BOOL CWorld::IsLayerPlayer( CObj* pObj, int nLayer )
{
	return ( pObj && pObj->GetLayer() == nLayer && pObj->GetType() == OT_MOVER && static_cast<CMover*>( pObj )->IsPlayer() );
}

void CWorld::DriveOut( int nLayer )
{
#ifdef __WORLDSERVER
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		if( IsLayerPlayer( m_apObject[i], nLayer ) )
		{
			FLWSUser* pUser	= static_cast<FLWSUser*>( m_apObject[i] );
			switch( GetID() )
			{
//sun: 13, 하우징 시스템
				case WI_WORLD_MINIROOM : CHousingMng::GetInstance()->GoOut( pUser ); break;
				default :
					if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( GetID() ) )
					{
						CInstanceDungeonHelper::GetInstance()->GoOut( pUser );
						break;
					}
					else if( GuildHouseMng->IsGuildHouse( GetID() ) )
					{
						//pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, pUser->m_vMarkingPos, REPLACE_FORCE, nDefaultLayer );
						Invalidate( nLayer, FALSE );
						break;
					}
					pUser->REPLACE( g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3( 6983.0f, 0.0f, 3330.0f ), REPLACE_FORCE, nDefaultLayer );
					break;
			}
		}
	}
#endif	// __WORLDSERVER
}

BOOL CWorld::ReleaseLayer( int nLayer )
{
	DestroyObj( nLayer );
	m_linkMap.DeleteLinkMap( nLayer );
	m_respawner.Release( nLayer );
	return TRUE;
}

void CWorld::DestroyObj( int nLayer )
{
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		CObj* pObj	= m_apObject[i];
		if( pObj && pObj->GetLayer() == nLayer )
			DestroyObj( pObj );
	}
}
////////////////////////////////////////////////////////////////////////////////
#endif	// __LAYER_1021