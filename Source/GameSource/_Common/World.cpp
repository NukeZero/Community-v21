#include "stdafx.h"
#include "region.h"
#include "defineWorld.h"
#include "commonctrl.h"

//sun: 9, 9-10차 펫
#ifdef __CLIENT
#include "resdata.h"
#endif	// __CLIENT

#include "..\_AIInterface\AIInterface.h"
#include "..\_AIInterface\FLFSM.h"

#ifdef __WORLDSERVER
#include "../worldserver/LinkMap.h"
#include "../WorldServer/user.h"
#include "worldmng.h"
#include "guild.h"
#include "eveschool.h"

extern	CWorldMng		g_WorldMng;
extern	CGuildMng		g_GuildMng;
extern	CGuildCombat	g_GuildCombatMng;
#endif	// __WORLDSERVER

#ifdef __CLIENT
#include "dialogmsg.h"
#include "../Neuz/DPClient.h"
extern	CDPClient	g_DPlay;

#include "GuildHouse.h"
#include "WndHousing.h"
#include "WndField2.h"

#endif	// __CLIENT

#ifdef __WORLDSERVER
#include "../WorldServer/npchecker.h"
#include "../worldserver/LinkMap.h"
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
/*
1	= 0  1 ~ 1.9
2	= 1  2 ~ 3 
4	= 2  4 ~ 7
8	= 3  8 ~ 15  
16	= 4
32	= 5
64	= 6
128	= 7
*/
int	CLandscape::m_nWidthLinkMap[MAX_LINKLEVEL];
#endif	// __WORLDSERVER



#ifndef __WORLDSERVER
CObj* CWorld::m_aobjCull[ MAX_DISPLAYOBJ ];
CObj* CWorld::m_asfxCull[ MAX_DISPLAYSFX ];
int CWorld::m_nObjCullSize = 0;
int CWorld::m_nSfxCullSize = 0;
#endif

CWorld::CWorld()
#ifdef __WORLDSERVER
:
m_cbRunnableObject( 0 )
#endif	// __WORLDSERVER
{
	m_nLandWidth	= 0;
	m_nLandHeight	= 0;
	WORLD_WIDTH = 0;
	WORLD_HEIGHT = 0;
	m_dwWorldID	= NULL_ID;
	m_dwIdWorldRevival = WI_WORLD_NONE; 
	ZeroMemory( m_szKeyRevival, sizeof( m_szKeyRevival ) );	
	m_nDeleteObjs	= 0;
	m_szFileName[0]	= '\0';
	m_fMaxHeight = 200.0f;		//091209 기획요청으로 200
	m_fMinHeight = 85.0f;
	m_bFly = TRUE;
	m_bIsIndoor  = FALSE;
	m_dwDiffuse  = 0;
	m_v3LightDir = D3DXVECTOR3( 0.0f,0.0f,0.0f );
	m_dwIdMusic  = 0;
	m_nPKMode = 0;
	m_szWorldName[0] = '\0';

#ifdef __WORLDSERVER
	m_apHeightMap	= NULL;
	m_apWaterHeight = NULL;
	m_dwObjNum	= 0;
	m_cbAddObjs		= 0;
	memset( m_lpszWorld, 0, sizeof(m_lpszWorld) );
	m_cbUser	= 0;
	nextptr	= NULL;
	m_cbModifyLink	= 0;
	m_cbReplaceObj	= 0;
	m_bLoadScriptFlag = FALSE;

	m_cbOnDie	= 0;
	memset( m_aOnDie, 0, sizeof(m_aOnDie) );
#else	// __WORLDSERVER
	m_fElapsedTime	=0;
	m_nVisibilityLand	= (int)( m_fFarPlane / ( MAP_SIZE * MPU ) );
	m_fFogStartValue	= 70;
	m_fFogEndValue	= 400;
	m_fFogDensity	= 0.0f;
	m_pd3dDevice	= NULL;
	m_pBoundBoxVertexBuffer = NULL;
	m_nCharLandPosX = -1;
	m_nCharLandPosZ = -1;

	m_bViewGrid	= FALSE;
	m_bViewGridPatch = FALSE;
	m_bViewGridLandscape = FALSE;
	m_bViewWireframe = FALSE;
	m_bViewSkybox = TRUE;
	m_bViewTerrain = TRUE;
	m_bViewAllObjects = TRUE;
	m_bViewFog = TRUE;
	m_bViewWater = TRUE;
	m_bViewName	= TRUE;
	m_bViewHP	= TRUE;	
	m_bViewLight = TRUE;
	m_bViewWeather = TRUE;
	m_bViewBoundBox	= FALSE;
	m_bCullObj = TRUE;
	m_bProcess = TRUE;
	m_bViewObj = TRUE;
	m_bViewMover = TRUE;
	m_bViewItem	= TRUE;
	m_bViewRegion = TRUE;
	m_bViewSpawn = TRUE;
	m_bViewHeightAttribute = FALSE;
	m_bViewLODObj = TRUE;

	m_nObjCullSize = 0;
	m_nSfxCullSize = 0;
	m_bViewIdState = FALSE;
	m_dwAmbient	= D3DCOLOR_ARGB( 255,128,128,128);
	m_pObjFocus		= NULL;
	m_idObjFocusOld	= NULL_ID;
	m_dwLightIndex	= 0;
	m_pCamera	= NULL;
	m_apLand	= NULL;

	m_bProcessingEnvir = FALSE;
	m_dwAddedTime	= 0;
	m_bUsing24Light = FALSE;

	m_bViewDebugDialog	= FALSE;
#endif // __WORLDSERVER
}

CWorld::~CWorld()
{
	Free();
}

#ifdef __WORLDSERVER
void	CWorld::removeObjectAtProcessArray( CObj* pObj )
{
	if( pObj == NULL ) {
		return;
	}

	// _add
	for( int nth = 0; nth < m_cbAddObjs; ++nth ) {
		if( m_apAddObjs[nth] == pObj ) {
			m_apAddObjs[nth] = NULL;
			break;
		}
	}
	/*
	// _process
	for( u_long nth = 0; nth < m_dwObjNum; ++nth ) {
		if( m_apObject[nth] == pObj ) {
			m_apObject[nth] = NULL;
			break;
		}
	}
	*/
	// _replace
	for( int nth = 0; nth < m_cbReplaceObj; ++nth ) {
		if( m_aReplaceObj[nth].pObj == pObj ) {
			m_aReplaceObj[nth].pObj = NULL;
			break;
		}
	}

	pObj->SetWorld( NULL );
}
#endif // __WORLDSERVER

void CWorld::Free()
{
#ifdef __WORLDSERVER
	CObj* pObjtmp;
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		pObjtmp		= m_apObject[i];
		SAFE_DELETE( pObjtmp );
	}
	
	int nBk = m_vecBackground.size();
	for( int i = 0; i < nBk; ++i )
		SAFE_DELETE( m_vecBackground[i] );
	m_vecBackground.clear();
	
	m_linkMap.Release();
	SAFE_DELETE_ARRAY( m_apHeightMap );
	SAFE_DELETE_ARRAY( m_apWaterHeight );

//	CCtrlRegion* pCtrlRegion;
//	while( !m_lspCtrlRegion.empty() )
//	{
//		pCtrlRegion	= m_lspCtrlRegion.front();
//		m_lspCtrlRegion.pop_front();
//		SAFE_DELETE( pCtrlRegion );
//	}
#else	// __WORLDSERVER
	// Light 파괴 
	POSITION pos 
		= m_mapLight.GetStartPosition();
	CLight* pLight;
	CString string;
	while( pos )
	{
		m_mapLight.GetNextAssoc( pos, string, (void*&)pLight );
		SAFE_DELETE( pLight );
	}
	m_mapLight.RemoveAll();
	if( m_apLand != NULL )
	{
		for( int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
			SAFE_DELETE( m_apLand[ i ] );
		SAFE_DELETE_ARRAY( m_apLand );
	}

	m_pObjFocus	= NULL;
	m_nDeleteObjs	= 0;

	m_strCurContName = "";

#ifdef __BS_EFFECT_LUA
	CSfxModelMng::Free( );
#endif	//__BS_EFFECT_LUA

#endif	// __WORLDSERVER
}

bool	CWorld::CanCreateParty() const
{
	const bool bCan	= ( m_dwWorldID != WI_WORLD_GUILDWAR /*&& m_dwWorldID != WI_WORLD_FWC*/ )
					? true : false;

	return bCan;
}

// CPU를 사용율 줄이기 위해서 이렇게...
void CWorld::CalcBound()
{
	WORLD_WIDTH = MAP_SIZE * m_nLandWidth;
	WORLD_HEIGHT = MAP_SIZE * m_nLandHeight;
}

void CWorld::LoadAllMoverDialog()
{
#ifdef __WORLDSERVER
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		if( m_apObject[ i ] && m_apObject[ i ]->GetType() == OT_MOVER )
			((CMover*)m_apObject[ i ])->LoadDialog();
	}
#endif
}

#ifdef __CLIENT
// pObj는 CItem도 들어온다.
void CWorld::SetObjFocus( CObj* pObj, BOOL bSend ) 
{
//sun: 13, 하우징 시스템
	CtrlProp* pProp = NULL;
	CCtrl* pCtrl = NULL;
	if( pObj && OT_CTRL == pObj->GetType( ) )
	{
		pCtrl = ( CCtrl* )pObj;
		pProp = prj.GetCtrlProp( pCtrl->GetIndex( ) );
		if( !pProp )
			return;

		if( CK1_HOUSING == pProp->dwCtrlKind1 )
			return;
	}

	//gmpbigsun : 현재 콘트롤들은 클릭->서버처리(작동)->타겟제거 방식으로 작동되고, 수정되지 않는한 콘트롤에 대해 다른기능를 부여하는것이 일반적이지 않다.
	//			  그러나 길드하우징 오브젝트는 팝업메뉴로 다른기능을 하게끔 해야 하는 상황이고, 아래와 같이 우회한다.
	//			  팝업메뉴를 설정한뒤, 현재 선택할려는 대상이 길드하우징 오브젝트이면 현재 타겟을 바꾸지 않고 PASS한다.
	//            팝업메뉴는 서버와 비동기로 작동하며 메뉴를 선택할경우 현재 선택대상과는 별개로 작동한다. ( CWndWorld::OnCommand를 발생시키지 않는다 )
	//            팝업메뉴는 바로 CGuildHouseBase의 해당 함수를 불러주고 끝나는 형태이다.
	//            CGuildHouseBases는 선택대상이 길드하우징 오브젝트일경우 해당 아이디를 보존한다.
	if( pObj && OT_CTRL == pObj->GetType( ) )
	{
		if( pProp->IsGuildHousingObj( ) )
		{
			GuildHouse->m_dwSelectedObjID = pCtrl->GetId( );			// 길드하우징 이라면 id저장하고 pass!

			CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet( APP_GH_FURNITURE_STORAGE );
			if( pWnd )
				pWnd->SetSelectedByObjID( GuildHouse->m_dwSelectedObjID );
			return;
		}
	}

	//EVENT_ARENA
	if( g_WndMng.m_pWndEArenaTeam )
	{
		if( NULL == pObj )
		{
			g_WndMng.m_pWndEArenaTeam->DoSelectedUser( 0 );
		}
		else if( OT_MOVER == pObj->GetType( ) )
		{
			CMover* pTarget = static_cast< CMover* > ( pObj );
			g_WndMng.m_pWndEArenaTeam->DoSelectedUser( pTarget->GetId() );
		}
	}


//sun: 13, 퀘스트 NPC위치 알림
	CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
	if(pWndWorld)
	{
		if(pWndWorld->m_bSetQuestNPCDest)
			pWndWorld->m_bSetQuestNPCDest = FALSE;
	}

	CObj *pOldFocus = m_pObjFocus;
	
	if( m_pObjFocus && m_pObjFocus->GetType() == OT_MOVER )
		m_idObjFocusOld = ((CMover *)m_pObjFocus)->GetId();		// 한번 잡았던 오브젝트의 ID를 기억해둔다. 이오브젝트가 다시 Add되었을때 자동으로 다시 잡아준다.
	if( pOldFocus )	// 기존에 뭔가 잡혀있었는가.
	{
		if( pOldFocus->GetType() == OT_MOVER )		// 타겟이 Mover일때만 처리하자
		{
			CMover *pMoverTarget = (CMover *)pOldFocus;
			OBJID idTarget = pMoverTarget->GetId();
			if( pOldFocus != pObj && pMoverTarget->IsLive() )		// 기존타겟과 다른타겟(널포함)을 잡으려 하면 서버에 알림
			{
				if( bSend )
					g_DPlay.SendSetTarget( idTarget, 1 );	// idTarget에게 g_pPlayer가 타겟을 해제했다는것을 알림.
				pMoverTarget->m_idTargeter = NULL_ID;	// 클라에도 클리어 시켜줘야 한다.
			}
		}
	}

	if( pObj != m_pObjFocus )
	{
		if( pObj )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				CMover *pMoverTarget = (CMover *)pObj;
				OBJID idTarget = pMoverTarget->GetId();
				if( bSend )
					g_DPlay.SendSetTarget( idTarget, 2 );	// g_pPlayer가 idTarget을 포커스했다는걸(2) 알림.
			}
		}
		else
		{
			if( bSend )
				g_DPlay.SendSetTarget( NULL_ID, 2 );	// 타겟 클리어.
//sun: 11, 타겟 표시 방법 변경
			CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
			if(pWndWorld)
			{
				pWndWorld->m_pNextTargetObj = NULL;
				pWndWorld->m_pRenderTargetObj = NULL;
			}
		}
	}

	m_pObjFocus = pObj; 
	
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		if( ((CMover*)pObj)->IsPlayer() )
		{
			if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) == TRUE )
			{
				if( bSend )
					g_DPlay.SendMoverFocus( ((CMover*)pObj)->m_idPlayer );	// 겜마가 유저를 클릭한경우 그 유저의 정보를 달라는 신호를 보냄.
			}
		}
	}
}
#endif


#ifndef __WORLDSERVER
void CWorld::AddLight( CLight* pLight )
{
	pLight->m_dwIndex = m_dwLightIndex++;
	m_mapLight.SetAt( pLight->m_strKey, pLight );
}

CLight* CWorld::GetLight( LPCTSTR lpszKey )
{
	CLight* pLight = NULL;
	m_mapLight.Lookup( lpszKey, (void*&)pLight );
	return pLight;
}
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
BOOL CWorld::DoNotAdd( CObj* pObj )
{

	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		if( m_apAddObjs[i] == pObj )
		{
			pObj->SetWorld( NULL );
			m_apAddObjs[i]	= NULL;
			return TRUE;
		}
	}
	return FALSE;
}
#endif	// __WORLDSERVER

#ifdef __LAYER_1015
BOOL CWorld::AddObj( CObj* pObj, BOOL bAddItToGlobalId, int nLayer )
#else	// __LAYER_1015
BOOL CWorld::AddObj( CObj* pObj, BOOL bAddItToGlobalId )
#endif	// __LAYER_1015
{
	if( pObj->GetWorld() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "obj's world is NOT NULL." ) );
		return FALSE;
	}

	D3DXVECTOR3 v = pObj->GetPos();
	if( VecInWorld( v ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "world id:%d x:%f y:%f z:%f world size w:%d,h:%d" ), GetID(), v.x, v.y, v.z, m_nLandWidth, m_nLandHeight );
	
		return FALSE;
	}

	if( pObj->IsDynamicObj() )
	{
#ifndef __WORLDSERVER
		// 이 코드는 데드락을 유발하므로(내부적 prj lock), _add에서 실행한다.
		if( bAddItToGlobalId ) 
			( (CCtrl*)pObj )->AddItToGlobalId();
#endif	// __WORLDSERVER
		FLOAT fHeight	= GetLandHeight( v );
		v.y		= ( v.y < fHeight ? fHeight : v.y );
		
		// 예외 처리...Door컨트롤은 땅에 묻혀두 됨
		if( pObj->m_dwType == OT_CTRL )
		{
			CCommonCtrl* pCCtrl = (CCommonCtrl*)pObj;
			CtrlProp* pProp = pCCtrl->GetProp();
			if( pProp )
			{
				if( pProp->dwCtrlKind1 == CK1_DOOR )
					v.y = pObj->GetPos().y;
			}
		}

		pObj->SetPos( v );
	}
	pObj->UpdateLocalMatrix();
#ifdef __LAYER_1015
	pObj->SetLayer( nLayer );
#endif	// __LAYER_1015
	pObj->SetWorld( this );

	if( pObj->m_pAIInterface ) 
		pObj->m_pAIInterface->InitAI();
#ifdef __WORLDSERVER
	if( pObj->m_pFSM )
	{
		pObj->m_pFSM->InitProp();
	}

	FLASSERT( m_cbAddObjs < MAX_ADDOBJS );
	if( m_cbAddObjs < 0 || m_cbAddObjs >= MAX_ADDOBJS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "World AddObj Overflow World: %d, AddObjs: %d" ), GetID(), m_cbAddObjs );
		return FALSE;
	}

	pObj->m_nAddObjsArrayIndex		= m_cbAddObjs;

	m_bAddItToGlobalId[m_cbAddObjs]	= bAddItToGlobalId;
	m_apAddObjs[m_cbAddObjs++]		= pObj;
#else	// __WORLDSERVER
	InsertObjLink( pObj );
	AddObjArray( pObj );
#endif	// __WORLDSERVER

#ifdef __CLIENT
	if( CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover &&
		CMover::GetActiveMover()->m_pActMover->IsFly() )
	{
		if( pObj->GetType() == OT_MOVER )
		{
			if( ((CMover *)pObj)->GetId() == m_idObjFocusOld )
			{
				SetObjFocus( pObj );
			}
		}
	}

#ifdef __BS_EFFECT_LUA
	
	// 생성시점에서 생성관련 Effect 처리( NPC only!! )
	if( OT_MOVER == pObj->GetType() )
	{
		CMover* pMover = static_cast< CMover* > ( pObj );
		
		if( pMover->IsNPC() )
			run_lua_sfx( OBJSTA_NONE, pMover->GetId(), pMover->GetNameO3D() );
	}

#endif //__MOVER_STATE_EFFECT

#endif //__CLIENT

	return TRUE;
}

void CWorld::RemoveObj( CObj* pObj )
{
	if( pObj->GetWorld() == NULL ) 
		return;
	pObj->SetWorld( NULL );
	RemoveObjLink( pObj );
	RemoveObjArray( pObj );
}

// 모든 루프가 끝나고 메모리에서 delete 된다.
void CWorld::DeleteObj( CObj* pObj )
{
#ifdef __WORLDSERVER
	// AddObj를 한후 _proceess에서 아직 add 되지 않은 경우
	if( pObj->m_nAddObjsArrayIndex >= 0 )
	{
		removeObjectAtProcessArray( pObj );
		SAFE_DELETE( pObj );
		return;
	}
#endif //__WORLDSERVER
	if( !pObj->IsDelete() )
	{
		pObj->SetDelete( TRUE );
#ifdef __WORLDSERVER
		if( m_nDeleteObjs >= MAX_DELETEOBJS )
		{
			if( pObj->GetType() == OT_MOVER )
				FLERROR_LOG( PROGRAM_NAME, _T( "\"%s\" %d" ), ((CMover *)pObj)->GetName(), m_nDeleteObjs );
			else
				FLERROR_LOG( PROGRAM_NAME, _T( "type=%d idx=%d %d" ), pObj->GetType(), pObj->GetIndex(), m_nDeleteObjs );
		}

		if( m_nDeleteObjs >= MAX_DELETEOBJS )
			FLERROR_LOG( PROGRAM_NAME, _T( "MAX_DELETEOBJS" ) );
		m_apDeleteObjs[ m_nDeleteObjs++ ] = pObj;
#else

		if(pObj->m_ppViewPtr)
		{
			*pObj->m_ppViewPtr = NULL;
			pObj->m_ppViewPtr = NULL;
		}

		m_apDeleteObjs[ m_nDeleteObjs++ ] = pObj;
#endif
	}

}

BOOL CWorld::AddObjArray( CObj* pObj )
{
#ifdef __WORLDSERVER
	int nIndex;
	if( m_ObjStack.GetCount() > 0 )
	{
		nIndex	= m_ObjStack.Pop();
		FLASSERT( !m_apObject[nIndex] );
	}
	else
	{
		nIndex	= m_dwObjNum++;
	}

	if( nIndex == 0xffffffff )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "AddObjArray nIndex = 0xffffffff" ) );
		return FALSE;
	}

	m_apObject[nIndex]	= (CCtrl*)pObj;
	pObj->m_dwObjAryIdx	= nIndex;
	m_cbRunnableObject++;
#else	// __WORLDSERVER
	CLandscape* pLandscape	= GetLandscape( pObj );
	if( NULL != pLandscape ) 
	{
		pLandscape->AddObjArray( pObj );
	}
	/*
	else 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "AOA error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f" ),
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
	}
	*/
#endif	// __WORLDSERVER
	return TRUE;
}

void CWorld::RemoveObjArray( CObj* pObj )
{
#ifdef __WORLDSERVER
	//if( pObj->m_dwObjAryIdx == 0xffffffff )
	if( pObj->m_dwObjAryIdx < 0 || pObj->m_dwObjAryIdx >= MAX_DYNAMICOBJ )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "invalid pObj->m_dwObjAryIdx : %u" ), pObj->m_dwObjAryIdx );
		return;
	}

	if( m_apObject[pObj->m_dwObjAryIdx] == pObj )
	{
		m_ObjStack.Push( pObj->m_dwObjAryIdx );
		m_apObject[pObj->m_dwObjAryIdx]		= NULL;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_apObject[pObj->m_dwObjAryIdx] != pObj" ) );
	}
	m_cbRunnableObject--;
#else	// __WORLDSERVER
	CLandscape* pLandscape	= GetLandscape( pObj );

	if( NULL != pLandscape ) 
	{
		pLandscape->RemoveObjArray( pObj );
	}
	/*
	else 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "ROA error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f" ),
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
	}
	*/
#endif	// __WORLDSERVER
}

BOOL CWorld::InsertObjLink( CObj* pObj )
{
	D3DXVECTOR3 vPos	= pObj->GetLinkPos();
	if( VecInWorld( vPos ) == FALSE )
		return FALSE;

#ifdef __WORLDSERVER
	return m_linkMap.InsertObjLink( pObj );
#else	// __WORLDSERVER
	FLOAT rX	= vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ	= vPos.z / MPU;	// - m_nWorldY;
	int mX	= int( rX / MAP_SIZE );
	int mZ	= int( rZ / MAP_SIZE );

	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) 
	{
		return pLandscape->InsertObjLink( pObj );
	}
	/*
	else 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "IOL error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f" ),
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
	}
	*/
	return FALSE;
#endif	// __WORLDSERVER
}

BOOL CWorld::RemoveObjLink2( CObj* pObj )
{
#ifdef __WORLDSERVER
	return m_linkMap.RemoveObjLink2( pObj );
#else
	return TRUE;
#endif
}

BOOL CWorld::RemoveObjLink( CObj* pObj )
{
	D3DXVECTOR3	vPos	= pObj->GetLinkPos();
	if( VecInWorld( vPos ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "ROL//INVALID POS//%d, %d, %f, %f, %f, %f, %f, %f" ),
			pObj->GetType(), pObj->GetIndex(), pObj->GetPos().x, pObj->GetPos().y, pObj->GetPos().z, vPos.x, vPos.y, vPos.z );
		return FALSE;
	}
#ifdef __WORLDSERVER
	return m_linkMap.RemoveObjLink( pObj );
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;	
	FLOAT rZ = vPos.z / MPU;	
	int   mX = int( rX / MAP_SIZE );
	int   mZ = int( rZ / MAP_SIZE );

	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		return pLandscape->RemoveObjLink( pObj );
	}
	/*
	else {
		FLERROR_LOG( PROGRAM_NAME, _T( "ROL error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f" ),
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
	}
	*/
	return FALSE;
#endif	// __WORLDSERVER
}

#ifndef  __WORLDSERVER

#ifdef __LAYER_1015
CObj* CWorld::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, int nLayer )
#else	// __LAYER_1015
CObj* CWorld::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel )
#endif	// __LAYER_1015
{
	if( VecInWorld( vPos ) == FALSE )
		return NULL;
#ifdef __WORLDSERVER
//#ifdef __LAYER_1015
//	return m_linkMap.GetObjInLinkMap( vPos, dwLinkType, nLinkLevel, nLayer );
//#else	// __LAYER_1015
//	return m_linkMap.GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );
//#endif	// __LAYER_1015
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;
	FLOAT rZ = vPos.z / MPU;
	int	mX	= int( rX / MAP_SIZE );
	int	mZ	= int( rZ / MAP_SIZE );
	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		return pLandscape->GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );
	}
	/*
	else {
		FLERROR_LOG( PROGRAM_NAME, _T( "GOILM error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, vPos.x, vPos.z );
	}
	*/
	return NULL;
#endif	// __WORLDSERVER
}

#ifdef __LAYER_1015
BOOL CWorld::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj, int nLayer )
#else	// __LAYER_1015
BOOL CWorld::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj )
#endif	// __LAYER_1015
{
	if( VecInWorld( vPos ) == FALSE )
		return FALSE;
#ifdef __WORLDSERVER
	#ifdef __LAYER_1015
	return m_linkMap.SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj, nLayer );
	#else	// __LAYER_1015
	return m_linkMap.SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );
	#endif	// __LAYER_1015
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;
	FLOAT rZ = vPos.z / MPU;
	int mX	= int( rX / MAP_SIZE );
	int mZ	= int( rZ / MAP_SIZE );
	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		pLandscape->SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );	//m_aObjLinkMap[mX_ + mZ_ * MAP_SIZE]	= pObj;
	}
	/*
	else {
		FLERROR_LOG( PROGRAM_NAME, _T( "SOILM error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f" ),
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, vPos.x, vPos.z );
	}
	*/
	return TRUE;
#endif	// __WORLDSERVER

} 

#endif //_CLIENT

#ifdef __WORLDSERVER
void CWorld::Process()
{
	_add();
	_process();		// iocp
	_modifylink();	// _replace();
	_OnDie();		// FLWSUserManager	// CGuildMng
	_replace();		//_modifylink();
	_delete();

	if( m_bLoadScriptFlag )
	{
		LoadAllMoverDialog();
		m_bLoadScriptFlag = FALSE;
	}
#ifdef __LAYER_1021
	m_linkMap.Process( this );
#endif	// __LAYER_1021
}

#else	// __WORLDSERVER
void CWorld::Process()
{	
	// 처리 프로세스 
	int i, j, k, l, x, y;
	WorldPosToLand( m_pCamera->m_vPos, x, y );
	CLandscape* pLand = NULL;
	DWORD dwObjProcessNum = 0;
	D3DXVECTOR3 vPos, *pvCameraPos = &m_pCamera->m_vPos;
	float	fLengthSq;

	//suntodo!! 3dvision
	float	fFarSq = CWorld::m_fFarPlane / 2;
	fFarSq *= fFarSq;

	int		nNumObj;
	CObj* pObj = NULL;
	
#ifdef _DEBUG
	int _nCnt = 0;
#endif

	// 정적 오브젝트의 반투명 처리를 위한 프로세스 처리 
	int nNonCullNum = 0;
	for( i = 0; i < m_nObjCullSize; i++ )
	{
		pObj = m_aobjCull[ i ];
		if( pObj && pObj->GetType() == OT_OBJ && pObj->GetModel()->m_pModelElem->m_bTrans && pObj->IsCull() == FALSE )
		{
			pObj->Process();
		}
	}

	// OT_SHIP이 OT_ITEM이나 OT_MOVER보다 먼저 프로세스 되어야 해서 어쩔수 없이 이 방법을 씀.
	static int idx[ MAX_OBJARRAY ] = { 0, 1, 2, 7, 3, 4, 5, 6 };	
	for( i = y - m_nVisibilityLand; i <= y + m_nVisibilityLand; i++ )
	{
		for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++ )
		{
			if( LandInWorld( j, i ) && m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
				for( k = OT_ANI; k < MAX_OBJARRAY; k++ )
				{
					CObj** apObject = pLand->m_apObject[ idx[k] ];
					nNumObj = pLand->m_adwObjNum[ idx[k] ];
					for( l = 0; l < nNumObj; l++ )
					{
						if( *apObject && (*apObject)->IsDelete() == FALSE && (*apObject)->GetWorld() != NULL )
						{
							vPos = (*apObject)->GetPos() - *pvCameraPos;
							fLengthSq = D3DXVec3LengthSq( &vPos );
							if( fLengthSq < fFarSq ) 
							{
							#ifdef _DEBUG
								_nCnt ++;
							#endif								
								(*apObject)->Process();
							}
						}
						apObject ++;
					}
				}
			}
		}
	}
	
	// 자, 여기서 랜드에 속한 배열에서 바로 프로세싱하지 않고  apObjTemp 배열에 추가한 후에
	// Process한 것에 주목해야한다. Process 과정에서 오브제트가 삭제되거나 추가될 수 있는데
	// 랜드에 속한 배열을 루핑하면서 동일 배열에 추가되거나 삭제되게 되면  버그의  가능성이
	// 높아진다. 따라서 약간의 오버해드를 감수하면서 apObjTemp를 활용한다.

	g_DialogMsg.RemoveDeleteObjMsg();
	CWndTaskBar* pWndTaskBar = g_WndMng.m_pWndTaskBar;
	if( pWndTaskBar )
		pWndTaskBar->RemoveDeleteObj();

	// 오브젝트 Delete ( DeleteObj가 호출된 오브젝트들)
	DeleteObjects();

	if( m_pCamera )
	{
		if( CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover )
		{
			if( CMover::GetActiveMover()->m_pActMover->IsFly() )
				m_pCamera->Process( m_pd3dDevice, 10.0f );
			else
				m_pCamera->Process( m_pd3dDevice, 4.0f );
		}
	}
	if( g_pPlayer )
	{
		ReadWorld( g_pPlayer->GetPos() );

		if( m_bViewWeather )
		{
			m_skyBox.m_pWorld = this;
			m_skyBox.Process();
		}

		CheckInOutContinent( );
	}

	// 물 애니메이션
	for(i=0; i<prj.m_terrainMng.m_nWaterFrame; i++)
    {
		if(i<MAX_WATER)
		{
			prj.m_terrainMng.m_fWaterFrame[i] += prj.m_terrainMng.m_pWaterIndexList[i].fWaterFrame;

			if(prj.m_terrainMng.m_fWaterFrame[i] >= static_cast<FLOAT>(prj.m_terrainMng.m_pWaterIndexList[i].ListCnt))
				prj.m_terrainMng.m_fWaterFrame[i] = 0.0f;
		}
	}

#ifdef __BS_EFFECT_LUA
	CSfxModelMng::GetThis()->Update();
#endif	//__BS_EFFECT_LUA

}
#endif	// not __WORLDSERVER

// 
// GetHeight(D3DXVECTOR vecPos)
// vecPos의 해당하는 필드의 높이를 정교하게 계산해서 돌려 준다. 
// return 값은 y 좌표에 해당된다.
//
FLOAT CWorld::GetLandHeight( float x, float z )
{
	float _x = x, _z = z;

	if( VecInWorld( x, z ) == FALSE )
		return 0;
	
	x /= m_iMPU;
	z /= m_iMPU;
#ifdef __WORLDSERVER
	int px, pz;
	FLOAT dx, dz;
	FLOAT dy1, dy2, dy3, dy4;
	px = (int)x;
	pz = (int)z;
	dx = x - px;
	dz=  z - pz;

	FLOAT y1 = m_apHeightMap[ px + pz * WORLD_WIDTH ];
	FLOAT y2 = m_apHeightMap[ px + 1 + pz * WORLD_WIDTH ];

	if( px + ( pz + 1 ) * WORLD_WIDTH >= WORLD_WIDTH * WORLD_HEIGHT )
		FLERROR_LOG( PROGRAM_NAME, _T( "%d %d,  %f %f,  %d %d" ), px, pz, _x, _z, m_nLandWidth, m_nLandHeight );

	FLOAT y3 = m_apHeightMap[ px + ( pz + 1 ) * WORLD_WIDTH ];
	FLOAT y4 = m_apHeightMap[ px + 1 + ( pz + 1 ) * WORLD_WIDTH ];
	if( y1 >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y1 / HGT_NOWALK);
		y1 -= (float)(HGT_NOWALK * n);
	}
	if( y2 >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y2 / HGT_NOWALK);
		y2 -= (float)(HGT_NOWALK * n);
	}
	if( y3 >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y3 / HGT_NOWALK);
		y3 -= (float)(HGT_NOWALK * n);
	}
	if( y4 >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y4 / HGT_NOWALK);
		y4 -= (float)(HGT_NOWALK * n);
	}
	dy1 = y1 * ( 1 - dx ) * ( 1 - dz );
	dy2 = y2 * dx * ( 1 - dz );
	dy3 = y3 * ( 1 - dx ) * dz;
	dy4 = y4 * dx * dz;
	return dy1 + dy2 + dy3 + dy4;	
#else
	float rX = x;
	float rZ = z;
	int mX = int( rX / MAP_SIZE );
	int mZ = int( rZ / MAP_SIZE );
	CLandscape* pLand = m_apLand[ mX + mZ * m_nLandWidth ];
	if( pLand == NULL ) 
		return 0;

	return pLand->GetHeight( x - ( mX * MAP_SIZE ), z - ( mZ * MAP_SIZE ) );
#endif
}

FLOAT CWorld::GetLandHeight( const D3DXVECTOR3& vPos )
{
	return GetLandHeight( vPos.x, vPos.z );
}

#ifdef __CLIENT
// 
// GetHeight(D3DXVECTOR vecPos)
// vecPos의 해당하는 필드의 높이를 정교하게 계산해서 돌려 준다. 
// return 값은 y 좌표에 해당된다.
//
FLOAT CWorld::GetLandHeight_Fast( float x, float z )
{
	if( VecInWorld( x, z ) == FALSE )
		return 0;
	x /= MPU;
	z /= MPU;
	float rX = x;
	float rZ = z;
	int mX = int( rX / MAP_SIZE );
	int mZ = int( rZ / MAP_SIZE );
	if( m_apLand == NULL )
	{
		FLASSERT( 0 );
		return 0;
	}
	CLandscape* pLand = m_apLand[ mX + mZ * m_nLandWidth ];
	if( pLand == NULL ) 
		return 0;
	return pLand->GetHeight_Fast( x - ( mX * MAP_SIZE ), z - ( mZ * MAP_SIZE ) );
}
#endif

int CWorld::GetLandTris( float x, float z, D3DXVECTOR3* pTris )
{
	int offset=0;
	if(GetLandTri2(x,			z,			pTris+offset)) offset+=6;
	if(GetLandTri2(x-m_iMPU,	z-m_iMPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x,			z-m_iMPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x+m_iMPU,	z-m_iMPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x-m_iMPU,	z,			pTris+offset)) offset+=6;
	if(GetLandTri2(x+m_iMPU,	z,			pTris+offset)) offset+=6;
	if(GetLandTri2(x-m_iMPU,	z+m_iMPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x,			z+m_iMPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x+m_iMPU,	z+m_iMPU,	pTris+offset)) offset+=6;
	return offset;
}
BOOL CWorld::GetLandTri2( float x, float z, D3DXVECTOR3* pTri )
{
	if( VecInWorld( x, z ) == FALSE ) return FALSE;

	x /= m_iMPU; 
	z /= m_iMPU;

#ifdef __WORLDSERVER
	int gx = (int)x;
	int gz = (int)z;
	float glx = x - gx;
	float glz = z - gz;

	if( (gx + gz) % 2 == 0 ) 
	{
		if( glx > glz ) 
		{
			//0,3,1
			pTri[0] = D3DXVECTOR3( (float)( (gx  ) * m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1] = D3DXVECTOR3( (float)( (gx+1) * m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2] = D3DXVECTOR3( (float)( (gx+1) * m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );

			pTri[3] = D3DXVECTOR3( (float)( (gx  ) * m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[4] = D3DXVECTOR3( (float)( (gx  ) * m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[5] = D3DXVECTOR3( (float)( (gx+1) * m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
		}
		else {
			//0,2,3
			pTri[0] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );

			pTri[3] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[4] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[5] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
		}
	}
	else 
	{
		if( glx + glz < 1.0f )
		{
			//0,2,1
			pTri[0] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );

			pTri[3] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[4] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[5] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
		}
		else 
		{
			//1,2,3
			pTri[0] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );

			pTri[3] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[4] = D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[5] = D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
		}
	}
	float y = pTri[0].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[0].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[1].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[1].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[2].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[2].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[3].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[3].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[4].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[4].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[5].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[5].y -= (float)(HGT_NOWALK * n);
	}
	
#else
	int lx=(int)(x/MAP_SIZE);
	int lz=(int)(z/MAP_SIZE);
	DWORD gx=(int)x-lx*MAP_SIZE;
	DWORD gz=(int)z-lz*MAP_SIZE;
	float glx=x-(int)x;
	float glz=z-(int)z;

	CLandscape* pLand = m_apLand[ lx + lz * m_nLandWidth ];
	if( pLand == NULL ) return FALSE;

	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );

			pTri[3]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[4]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[5]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );

			pTri[3]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[4]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[5]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );

			pTri[3]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[4]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[5]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );

			pTri[3]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
			pTri[4]=D3DXVECTOR3( (float)( (gx  +lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx  ,gz+1) ), (float)( (gz+1+lz*MAP_SIZE)*m_iMPU ) );
			pTri[5]=D3DXVECTOR3( (float)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (float)( pLand->GetHeight(gx+1,gz  ) ), (float)( (gz  +lz*MAP_SIZE)*m_iMPU ) );
		}
	}
#endif
	return TRUE;
}

void CWorld::GetLandTri( float x, float z, D3DXVECTOR3* pTri )
{
	if( VecInWorld( x, z ) == FALSE ) return;

	x /= m_iMPU;			//SMPU
	z /= m_iMPU;

#ifdef __WORLDSERVER
	int gx=(int)x;
	int gz=(int)z;
	float glx=x-gx;
	float glz=z-gz;

	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH] ), (float)( (gz  )*m_iMPU ) );
			pTri[1]=D3DXVECTOR3( (float)( (gx  )*m_iMPU ), (float)( m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
			pTri[2]=D3DXVECTOR3( (float)( (gx+1)*m_iMPU ), (float)( m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH] ), (float)( (gz+1)*m_iMPU ) );
		}
	}

	float y = pTri[0].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[0].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[1].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[1].y -= (float)(HGT_NOWALK * n);
	}
	y = pTri[2].y;
	if( y >= HGT_NOWALK )		// 1000이상 값이면 그이상값을 잘라버린다.
	{
		int n = (int)(y / HGT_NOWALK);
		pTri[2].y -= (float)(HGT_NOWALK * n);
	}
	
#else
	int lx=(int)(x/MAP_SIZE);
	int lz=(int)(z/MAP_SIZE);
	DWORD gx=(int)x-lx*MAP_SIZE;
	DWORD gz=(int)z-lz*MAP_SIZE;
	float glx=x-(int)x;
	float glz=z-(int)z;

	CLandscape* pLand = m_apLand[ lx + lz * m_nLandWidth ];
	if( pLand == NULL ) return;

	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3( (FLOAT)( (gx  +lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx  ,gz  ) ), (FLOAT)( (gz  +lz*MAP_SIZE)*m_iMPU) );
			pTri[1]=D3DXVECTOR3( (FLOAT)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx+1,gz+1) ), (FLOAT)( (gz+1+lz*MAP_SIZE)*m_iMPU) );
			pTri[2]=D3DXVECTOR3( (FLOAT)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx+1,gz  ) ), (FLOAT)( (gz  +lz*MAP_SIZE)*m_iMPU) );
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3( (FLOAT)( (gx  +lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx  ,gz  ) ), (FLOAT)( (gz  +lz*MAP_SIZE)*m_iMPU) );
			pTri[1]=D3DXVECTOR3( (FLOAT)( (gx  +lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx  ,gz+1) ), (FLOAT)( (gz+1+lz*MAP_SIZE)*m_iMPU) );
			pTri[2]=D3DXVECTOR3( (FLOAT)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx+1,gz+1) ), (FLOAT)( (gz+1+lz*MAP_SIZE)*m_iMPU) );
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3( (FLOAT)( (gx  +lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx  ,gz  ) ), (FLOAT)( (gz  +lz*MAP_SIZE)*m_iMPU) );
			pTri[1]=D3DXVECTOR3( (FLOAT)( (gx  +lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx  ,gz+1) ), (FLOAT)( (gz+1+lz*MAP_SIZE)*m_iMPU) );
			pTri[2]=D3DXVECTOR3( (FLOAT)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx+1,gz  ) ), (FLOAT)( (gz  +lz*MAP_SIZE)*m_iMPU) );
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3( (FLOAT)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx+1,gz  ) ), (FLOAT)( (gz  +lz*MAP_SIZE)*m_iMPU) );
			pTri[1]=D3DXVECTOR3( (FLOAT)( (gx  +lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx  ,gz+1) ), (FLOAT)( (gz+1+lz*MAP_SIZE)*m_iMPU) );
			pTri[2]=D3DXVECTOR3( (FLOAT)( (gx+1+lx*MAP_SIZE)*m_iMPU ), (FLOAT)( pLand->GetHeight(gx+1,gz+1) ), (FLOAT)( (gz+1+lz*MAP_SIZE)*m_iMPU) );
		}
	}
#endif

}


#ifdef __WORLDSERVER
//void CWorld::AddItToView( CCtrl* pCtrl )
//{
//	//m_linkMap.AddItToView( pCtrl );
//}

#ifdef _DEBUG
int nTick	= 0;
int nCall	= 0;
int nTime	= 0;
#endif

//void CWorld::ModifyView( CCtrl* pCtrl )
//{
//	//m_linkMap.ModifyView( pCtrl );
//}


BOOL CWorld::PreremoveObj( OBJID objid )
{
	CObj* pObj;
	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		pObj	= m_apAddObjs[i];
		if( pObj && pObj->IsDynamicObj() && ( (CCtrl*)pObj )->GetId() == objid )
		{
			SAFE_DELETE( m_apAddObjs[i] );
			return TRUE;
		}
	}
	return FALSE;
}

CObj* CWorld::PregetObj( OBJID objid )
{
	CObj* pObj;
	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		pObj	= m_apAddObjs[i];
		if( pObj && pObj->IsDynamicObj() && ( (CCtrl*)pObj )->GetId() == objid )
			return pObj;
	}

	return NULL;
}

void CWorld::_add( void )
{
	CObj* pObj;

	if( g_DPCoreClient.CheckIdStack() == FALSE )
	{
		return;
	}

	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		pObj	= m_apAddObjs[i];
		//if( NULL == pObj )	
		if( !::IsValidObj( pObj ) )
			continue;

#ifdef __WORLDSERVER

		pObj->m_nAddObjsArrayIndex		= -1;

#ifdef __GUILD_HOUSE_MIDDLE
		if( GuildHouseMng->IsGuildHouse( m_dwWorldID ) == TRUE && pObj->GetType() == OT_CTRL && pObj->GetLayer() > 0 )
		{
			GuildHouseMng->RemoveGuildHouseDoor( pObj, m_dwWorldID, pObj->GetLayer() );
		}
#endif // __GUILD_HOUSE_MIDDLE


//#ifdef __WORLDSERVER
		if( !pObj->IsVirtual() )
//#endif	// __WORLDSERVER
		{
			if( pObj->IsDynamicObj() ) 
			{
				CCtrl* pCtrl = static_cast< CCtrl* >( pObj );
				if( pCtrl != NULL )
				{
					if( m_bAddItToGlobalId[i] )
						pCtrl->AddItToGlobalId();	// prj.m_objmap 와 prj.m_idPlayerToUserPtr에 넣는다.
					//AddItToView( pCtrl );
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Ctrl == NULL" ) );
				}
			}
		}


		if( !pObj->IsVirtual() )
#endif	// __WORLDSERVER
		{
			if( !InsertObjLink( pObj ))			// 링크맵에 넣는다 ( 에러가 날 수 있다 )
				continue;
		}

		if( !AddObjArray( pObj ))			// m_apObject 에 넣는다.
		{
			RemoveObjLink( pObj );
			continue;
		}


#ifdef __WORLDSERVER
		CNpcChecker::GetInstance()->AddNpc( pObj );
#endif	// __WORLDSERVER
	}
	m_cbAddObjs		= 0;
}

void CWorld::_modifylink( void )
{
	CObj* pObj;//, *pObjtmp;
	D3DXVECTOR3 vOld, vCur, vOldtmp, vCurtmp;
	int nLinkLevel;
	DWORD dwLinkType;

	for( int i = 0; i < m_cbModifyLink; i++ )
	{
		pObj	= m_apModifyLink[i];
		if( IsInvalidObj( pObj ) )	
			continue;

		if( pObj->GetWorld() != this )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "LINKMAP world different" ) );	// temp
			continue;
		}

		vOld	= pObj->GetLinkPos();
		vCur	= pObj->GetRemovalPos();
		vOldtmp		= vOld	/ (FLOAT)( m_iMPU );
		vCurtmp		= vCur / (FLOAT)( m_iMPU );

		if( (int)vOldtmp.x != (int)vCurtmp.x || (int)vOldtmp.z != (int)vCurtmp.z )
		{
			dwLinkType	= pObj->GetLinkType();
			nLinkLevel	= (int)pObj->GetLinkLevel();
#ifdef __LAYER_1015
//			int nLayer	= pObj->GetLayer();
#endif	// __LAYER_1015

		//	if( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() )
			//	FLERROR_LOG( PROGRAM_NAME, _T( "ML//%s//%d" ), ( (CMover*)pObj )->GetName(), ( (CMover*)pObj )->m_idPlayer );

//			if( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() && nLinkLevel != 0 )
//				FLERROR_LOG( PROGRAM_NAME, _T( "ML//%s//%d//%d" ), ( (CMover*)pObj )->GetName(), ( (CMover*)pObj )->m_idPlayer, nLinkLevel );
//#ifdef __LAYER_1015
//			if( !pObj->m_pPrev && GetObjInLinkMap( vOld, dwLinkType, nLinkLevel, nLayer ) != pObj )
//#else	// __LAYER_1015
//			if( !pObj->m_pPrev && GetObjInLinkMap( vOld, dwLinkType, nLinkLevel ) != pObj )
//#endif	// __LAYER_1015
//			{
//				FLERROR_LOG( PROGRAM_NAME, _T( "ML//BINGO//%d//%d//%d" ), pObj->GetType(), pObj->GetIndex(), pObj->GetLinkLevel() );
//				RemoveObjLink2( pObj );
//			}

//#ifdef __LAYER_1015
//			if( GetObjInLinkMap( vOld, dwLinkType, nLinkLevel, nLayer ) == pObj )
//				SetObjInLinkMap( vOld, dwLinkType, nLinkLevel, pObj->m_pNext, nLayer );
//#else	// __LAYER_1015
//			if( GetObjInLinkMap( vOld, dwLinkType, nLinkLevel ) == pObj )
//				SetObjInLinkMap( vOld, dwLinkType, nLinkLevel, pObj->m_pNext );
//#endif	// __LAYER_1015
//			pObj->DelNode();
//
//#ifdef __LAYER_1015
//			pObjtmp = GetObjInLinkMap( vCur, dwLinkType, nLinkLevel, nLayer );
//			if( pObjtmp != NULL )
//				pObjtmp->InsNextNode( pObj );
//			else
//				SetObjInLinkMap( vCur, dwLinkType, nLinkLevel, pObj, nLayer );
//#else	// __LAYER_1015
//			if( ( pObjtmp = GetObjInLinkMap( vCur, dwLinkType, nLinkLevel ) ) )
//				pObjtmp->InsNextNode( pObj );
//			else
//				SetObjInLinkMap( vCur, dwLinkType, nLinkLevel, pObj );
//#endif	// __LAYER_1015

			pObj->SetLinkPos( pObj->GetRemovalPos() );
			pObj->SetRemovalPos( D3DXVECTOR3( 0, 0, 0 ) );

//			if( pObj->IsDynamicObj() )
//				ModifyView( ( CCtrl* )pObj );
		}
	}
	m_cbModifyLink	= 0;
}

void CWorld::_delete( void )
{
	CObj* pObj;

	for( int i = 0; i < m_nDeleteObjs; i++ )
	{
		pObj	= m_apDeleteObjs[i];

		for( int j = 0; j < m_cbReplaceObj; j++ )
		{
			if( m_aReplaceObj[j].pObj == pObj )
				m_aReplaceObj[j].pObj	= NULL;
		}
		
		for( int j = 0; j < m_cbModifyLink; j++ )
		{
			if( m_apModifyLink[j] == pObj )
			{
				m_apModifyLink[j]	= NULL;
				pObj->m_vRemoval	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			}
		}

#ifdef __WORLDSERVER
		if( !pObj->IsVirtual() )
#endif	// __WORLDSERVER
			RemoveObjLink( pObj );
		DestroyObj( pObj );
	}
	m_nDeleteObjs	= 0;
}

void CWorld::DestroyObj( CObj* pObj )
{
	RemoveObjArray( pObj );
#ifdef __WORLDSERVER
	CNpcChecker::GetInstance()->RemoveNpc( pObj );
#endif	// __WORLDSERVER
	SAFE_DELETE( pObj );
}

void CWorld::_replace( void )
{
	CMover*		pMover;
	DWORD		dwWorldID;
	D3DXVECTOR3 vPos;
	u_long		uIdofMulti;
	
	for( int i = 0; i < m_cbReplaceObj; ++i )
	{
		pMover	= m_aReplaceObj[i].pObj;
		if( ::IsInvalidObj( pMover ) )
			continue;

		//const DWORD dwOldWorldID = m_aReplaceObj[i].dwOldWorldID;
		dwWorldID	= m_aReplaceObj[i].dwWorldID;
		vPos  	    = m_aReplaceObj[i].vPos;
		uIdofMulti	= m_aReplaceObj[i].uIdofMulti;
#ifdef __LAYER_1015
		int nLayer	= m_aReplaceObj[i].nLayer;
#endif	// __LAYER_1015

		if( uIdofMulti != g_uIdofMulti )		// 현재구현에서는 보류 
			continue;

#ifdef __LAYER_1015
		if( GetID() == dwWorldID && pMover->GetLayer() == nLayer )
#else	// __LAYER_1015
		if( GetID() == dwWorldID )
#endif	// __LAYER_1015
		{
			if( vPos.y == 0.0f )
			{
				vPos.y = 100.0f;
				vPos.y = GetFullHeight( vPos );  // 검사 
			}
			g_xWSUserManager->AddSetPos( pMover, vPos );
			//vPos.y = GetFullHeight( vPos );  // 검사 
			pMover->SetPos( vPos );
			if( pMover->IsPlayer() )
				( (FLWSUser*)pMover )->Notify();	
		}
		else
		{	
			if( pMover->IsPlayer() == FALSE )
				continue;

			FLWSUser* pUser = (FLWSUser*)pMover;

			if( GuildHouseMng->IsGuildHouse( GetID() ) )
				GuildHouseMng->CheckDestroyGuildHouse( pUser );

			CWorld* pWorld = g_WorldMng.GetWorld( dwWorldID );
			if( pWorld && pWorld->VecInWorld( vPos ) )	
			{
				CLinkMap* pLinkMap = pWorld->m_linkMap.GetLinkMap( nLayer );
				if( pLinkMap == NULL || pLinkMap->IsInvalid() == TRUE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ LinkMap Invalid. WorldID(%d), LayerID(%d) ]" ), dwWorldID, nLayer );
					continue;
				}

				//pUser->RemoveItFromView( TRUE );		// pc
				//pUser->RemoveItFromView2( TRUE );		// npc
//sun: 9, 9-10차 펫
				if( pUser->HasPet() )
				{
					pUser->RemovePet();
				}

				//////////////////////////////////////////////////////////////////////////
				// BEGIN110503 다른월드로 이동할때만 픽업펫 해제
				if( pUser->HasActivatedEatPet() == TRUE )
				{
					pUser->InactivateEatPet();
				}
				// END110503
				//////////////////////////////////////////////////////////////////////////

//버그 수정
#ifdef __WORLDSERVER
				if( pUser->GetWorld() != NULL && CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( pUser->GetWorld()->GetID() ) == TRUE )
				{
					//////////////////////////////////////////////////////////////////////////
					// BEGIN110502 콜로세움에서 나갔다면 진보스확률 버프 삭제
					if( CInstanceDungeonHelper::GetInstance()->GetDungeonKind( pUser ) == IDKIND_COLOSSEUM )
					{
						pUser->RemoveIk3Buffs( IK3_COLOSSEUM_BOSS_RATE );
					}
					// END110502
					//////////////////////////////////////////////////////////////////////////

					CInstanceDungeonHelper::GetInstance()->LeaveDungeon( pUser, pUser->GetWorld()->GetID( ) );	
				}
				else if( CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon( dwWorldID ) == TRUE )
				{
					CInstanceDungeonHelper::GetInstance()->IncreasePlayerCount( dwWorldID, nLayer );
				}
#endif //__WORLDSERVER


//sun: 11, 일대일 길드 대전
				if( dwWorldID == WI_WORLD_MADRIGAL && pUser->GetWorld() &&
					(pUser->GetWorld()->GetID() >= WI_WORLD_GUILDWAR1TO1_0 &&
					pUser->GetWorld()->GetID() <= WI_WORLD_GUILDWAR1TO1_L) )
				{
					pUser->m_nGuildCombatState = 0;
					g_xWSUserManager->AddGuildCombatUserState( pUser );
				}

				GuildHouseMng->ResetApplyDST( pUser );
				pUser->ClearDest();

				pUser->AddReplace( dwWorldID, vPos );
				pUser->Notify();	
				RemoveObj( pUser );						// pWorld = NULL, RemoveObjLink, RemoveObjArray
				pUser->SetPos( vPos );
			//	pUser->PCSetAt( pUser->GetId(), pUser );
				pUser->AddAddObj( pUser );
				pWorld->ADDOBJ( pUser, FALSE, nLayer );			// set pWorld 


				pUser->AddSMModeAll();
				pUser->AddSeasonEffect(); // 현재 날씨 세팅
				GuildHouseMng->SetApplyDST( pUser );

//sun: 13, 하우징 시스템
				if( dwWorldID == WI_WORLD_MINIROOM )
				{
					CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( static_cast<DWORD>( nLayer ) );
					if( pHousing )
					{
						pUser->AddHousingPaperingInfo( NULL_ID, FALSE );	// 벽지 및 장판 초기화
						std::vector<DWORD> vecTemp = pHousing->GetAllPaperingInfo();
						for( DWORD i=0; i<vecTemp.size(); i++ )
						{
							pUser->AddHousingPaperingInfo( vecTemp.at( i ), TRUE );
						}
					}
				}

			}
			else
			{
				FLASSERT( 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "_replace world id:%d x:%f y:%f z:%f" ), dwWorldID, vPos.x, vPos.y, vPos.z );
			}
		}
	}
	m_cbReplaceObj = 0;

}

void CWorld::_process( void )
{
	CObj* pObj;
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		pObj	= m_apObject[i];
		if( pObj && !pObj->IsDelete() && pObj->m_dwObjAryIdx != 0xffffffff )
			pObj->Process();
	}
}
#endif	// __WORLDSERVER

#ifndef __WORLDSERVER
LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z )
{
	if( VecInWorld( (FLOAT)( x ), (FLOAT)( z ) ) == FALSE )
		return NULL;
	x /= m_iMPU;
	z /= m_iMPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return 0;
	float rX = (float)( x );// - m_nWorldX;
	float rZ = (float)( z );// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);
	
	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return 0;
	
	int tx = ( x % MAP_SIZE ) / PATCH_SIZE;
	int tz = ( z % MAP_SIZE ) / PATCH_SIZE;
	
	return m_apLand[ mX + mZ * m_nLandWidth ]->GetWaterHeight( tx, tz );
}
int CWorld::GetHeightAttribute( float x, float z )
{
	if( VecInWorld( x, z ) == FALSE )
		return -1;

	float mpuInv = 1.0f / (float)m_iMPU;

	x *= mpuInv; //x /= MPU;
	z *= mpuInv; //z /= MPU;

	int rX = (int)( x );
	int rZ = (int)( z );
	int mX=int( rX / MAP_SIZE);
	int mZ=int( rZ / MAP_SIZE);

	if( m_apLand[mX + mZ * m_nLandWidth] == NULL ) 
		return -1;
	return	( m_apLand[ mX + mZ * m_nLandWidth ]->GetHeightAttribute( (int)( x-(mX*MAP_SIZE) ), (int)( z-(mZ*MAP_SIZE) ) ) );
}

#endif // not WORLDSERVER

#ifdef __WORLDSERVER
int CWorld::GetHeightAttribute( float x, float z )
{
	if( VecInWorld( x, z ) == FALSE )
		return -1;

	float inv_mpu = 1.0f / (float)m_iMPU;

	x *= inv_mpu; //x /= MPU;
	z *= inv_mpu; //z /= MPU;

	FLOAT fHeight = m_apHeightMap[ (int)x + (int)z * WORLD_WIDTH ]; //WORLD_WIDTH = MAP_SIZE * m_nLandWidth
	if( fHeight >= HGT_NOWALK )
	{
		if( fHeight >= HGT_DIE )
			return HATTR_DIE;
		if( fHeight >= HGT_NOMOVE )
			return HATTR_NOMOVE;
		if( fHeight >= HGT_NOFLY )
			return HATTR_NOFLY;
		return HATTR_NOWALK;
	}
	return HATTR_NONE;
}
LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z )
{
	if( VecInWorld( (float)( x ), (float)( z ) ) == FALSE )
		return NULL;

	x /= m_iMPU;
	z /= m_iMPU;

	int tx = x / PATCH_SIZE;
	int tz = z / PATCH_SIZE;

	LPWATERHEIGHT lpWaterHeight = &m_apWaterHeight[ tx + tz * ( NUM_PATCHES_PER_SIDE * m_nLandWidth ) ];
	return lpWaterHeight;
}

#endif // WORLDSERVER


// pvPos의 좌표를 중심으로 범위 데미지를 준다.
// ------------------------------------------
// nDmgType : 데미지 타입 ex) AF_MAGICSKILL
// pAttacker : 공격자(시전자) - 앞으로 CCtrl*로 바꿔야 한다
// nApplyType : 적용대상. ex) OBJTYPE_PLAYER | OBJTYPE_MONSTER
// nSkill : 사용한 스킬종류
// fRange : 타겟을 중심으로한 데미지 범위(미터)
// (&vPos, AF_MAGICSKILL, this, nSkill, 5.0f, 0.0, 1.0f )
void	CWorld::SendDamageAround( const D3DXVECTOR3 *pvPos, int nDmgType, CMover *pAttacker, int nApplyType, int nAttackID, float fRange )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4, 8, 16 단위로 넣자.
	float fDistSq;
	CObj* pObj;
	CMover *pTarget;
	D3DXVECTOR3 vPos = *pvPos;
	D3DXVECTOR3 vDist;

	if( fRange <= 4.0f )
		nRange = 4;
	else if( fRange <= 8.0f )
		nRange = 8;
	else if( fRange <= 16.0f )
		nRange = 16;
	else
		nRange = 32;

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		FLERROR_LOG( PROGRAM_NAME, _T( "D:%d,%d,%d A:%s %d %f" ), pvPos->x, pvPos->y, pvPos->z, pAttacker->GetName(), nAttackID, fRange );

	SkillProp* pProp;
	if( nDmgType == AF_MAGICSKILL )
	{
		pProp = prj.GetSkillProp( nAttackID );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
		if( pProp == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s. 스킬(%d)의 프로퍼티가 없다." ), pAttacker->GetName(), nAttackID );
			return;	// property not found
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s. 아이템(%d)의 프로퍼티가 없다." ), pAttacker->GetName(), nAttackID );
		return;	// property not found
	}
	
	BOOL	bDamage = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkPlayer, pAttacker->GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				if( pAttacker->IsPlayer() )
				{
					bDamage = TRUE;
				} else
				// 공격자가 몬스터
				{
					bDamage = TRUE;
				}

				if( bDamage )
				{
					vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
					fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
					{
						if( pObj != pAttacker )		// 어태커는 검색대상에서 제외.
						{
							pTarget = (CMover *)pObj;
							if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
							{
								if( pProp->dwComboStyle == CT_FINISH )
									pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
								else
									pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							}
						}
					}
					
					bDamage = FALSE;	// 다음 루프를 위해서 초기화.
				} // bDamage
			}
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & OBJTYPE_MONSTER )
	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic, pAttacker->GetLayer() )
		{
			if( pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPeaceful() == FALSE )
			{
				vDist = pObj->GetPos() - vPos;		// this -> 타겟까지의 벡터
				fDistSq = D3DXVec3LengthSq( &vDist );
				if( fDistSq < fRange * fRange )		// 타겟과의 거리가 fRange미터 이내인것을 대상으로.
				{
					if( pObj != pAttacker )		// 공격자는 검사대상에서 제외.
					{
						pTarget = (CMover *)pObj;
						if( IsValidObj( (CObj*)pTarget ) && pTarget->IsLive() )
						{
							if( pProp->dwComboStyle == CT_FINISH )
								pTarget->m_pActMover->SendDamageForce( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
							else
								pTarget->m_pActMover->SendDamage( nDmgType, pAttacker->GetId(), (nAttackID << 16) );
						}
					}
				}
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // SendDamageAround()

#ifdef __CLIENT
void CWorld::ProcessAllSfx( void )
{
	for( int i = 0; i < m_nLandWidth; i++ )
	{
		for ( int j = 0; j < m_nLandWidth; j++ )
		{
			if( LandInWorld( j, i ) && m_apLand[i * m_nLandWidth + j] )
			{
				CLandscape* pLand = m_apLand[i * m_nLandWidth + j];
				CObj** apObject		= pLand->m_apObject[OT_SFX];
				int nNumObj	= pLand->m_adwObjNum[OT_SFX];
				for( int l = 0; l < nNumObj; l++ )
				{
					if( *apObject && (*apObject)->IsDelete() == FALSE && (*apObject)->GetWorld() != NULL )
						(*apObject)->Process();
					apObject++;
				}
			}
		}
	}
}
#endif	// __CLIENT

#ifdef __WORLDSERVER
void CWorld::OnDie( CMover* pDie, CMover* pAttacker )
{
	if( m_cbOnDie >= MAX_ON_DIE )
		return;
	for( int i = 0; i < m_cbOnDie; i++ )
	{
		if( m_aOnDie[i].pDie == pDie )
			return;
	}
	m_aOnDie[m_cbOnDie].pDie	= (FLWSUser*)pDie;
	m_aOnDie[m_cbOnDie].pAttacker	= (FLWSUser*)pAttacker;
	m_cbOnDie++;
}

void CWorld::_OnDie( void )
{
	for( int i = 0; i < m_cbOnDie; i++ )
	{
		g_GuildCombatMng.OutWar( m_aOnDie[i].pDie, NULL );
		g_GuildCombatMng.GetPoint( m_aOnDie[i].pAttacker, m_aOnDie[i].pDie );
//sun: 11, 일대일 길드 대전
		int nIndex = g_GuildCombat1to1Mng.GetTenderGuildIndexByUser( m_aOnDie[i].pDie );
		if( nIndex != NULL_ID )
		{
			int nStageId = g_GuildCombat1to1Mng.m_vecTenderGuild.at( nIndex ).nStageId;
			if( nStageId != NULL_ID )
				g_GuildCombat1to1Mng.m_vecGuilCombat1to1.at( nStageId ).SetLost( m_aOnDie[i].pDie );
		}
	}
	m_cbOnDie	= 0;
}

CMover* CWorld::FindMover( LPCTSTR szName )
{
	CMover* pMover;
	for( DWORD i = 0; i < m_dwObjNum; i++ )
	{
		if( m_apObject[ i ] && m_apObject[ i ]->GetType() == OT_MOVER )
		{
			pMover = (CMover*)m_apObject[ i ];
			if( _tcscmp( pMover->GetName(), szName ) == NULL )
				return pMover;
		}
	}

	return NULL;
}
#endif	// __WORLDSERVER

#ifdef __LAYER_1021
void CWorld::Expand()
{
	m_respawner.Expand( nDefaultLayer );
// 	int nExpand	= CTicketProperty::GetInstance()->GetExpanedLayer( GetID() );
// 	for( int nLayer = -1, i = 0; i < nExpand; i++, nLayer-- )
// 		CreateLayer( nLayer );

	PT_WORLD_EXPAND_LAYER pSpec	= g_xSpecManager->GetSpecWorldExpandLayer( GetID() );
	if( pSpec != NULL )
	{
		for( int nLayer = -1, nCount = 0; nCount < pSpec->byExpandLayerCount; --nLayer, ++nCount )
		{
			CreateLayer( nLayer );
		}
	}
}
#endif	// __LAYER_1021

