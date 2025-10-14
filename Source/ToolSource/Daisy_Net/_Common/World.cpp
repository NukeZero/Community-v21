#include "stdafx.h"
#include "region.h"
#include "defineWorld.h"

#ifdef __WORLDSERVER
#include "user.h"
#include "dpsrvr.h"
#include "dpcoreclient.h"
#include "worldmng.h"

extern	CDPSrvr	g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
  
#ifdef __INFOCRITSECT_DEBUG__
#include "threadmng.h"
#endif	// __INFOCRITSECT_DEBUG__

#ifdef __WORLDSERVER
u_int	__stdcall	WorkerThread_0123( LPVOID lpv );
#endif	// __WORLDSERVER
#endif

#ifdef __CLIENT
#include "dialogmsg.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

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

#ifdef _DEBUG
#define	new DEBUG_NEW
#undef THIS_FILE
static	char	THIS_FILE[] = __FILE__;
#endif

//#ifdef __NEWPROCESS
#ifndef __WORLDSERVER
CObj* CWorld::m_aobjCull[ MAX_DISPLAYOBJ ];
CObj* CWorld::m_asfxCull[ MAX_DISPLAYSFX ];
int CWorld::m_nObjCullSize = 0;
int CWorld::m_nSfxCullSize = 0;
#endif
//#endif

CWorld::CWorld()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CWorld::m_AddRemoveLock" ),
m_csModifyLink( "CWorld::m_csModifyLink" ),
m_csReplaceObj( "CWorld::m_csReplaceObj" )
#endif	// __INFOCRITSECT_DEBUG__
{
	m_nLandWidth	= 0;
	m_nLandHeight	= 0;
	m_nDeleteObjs	= 0;
	m_szFileName[0]	= '\0';
	m_fMaxHeight = 270.0f;
	m_fMinHeight = 85.0f;
	m_bFly = TRUE;
	m_bIsIndoor  = FALSE;
	m_dwDiffuse  = 0;
	m_v3LightDir = D3DXVECTOR3( 0.0f,0.0f,0.0f );
	m_dwIdMusic  = 0;
	
#ifdef __N_TRIGGER_AREASET_050203
	m_nPKMode = 0;
	m_szWorldName[0] = '\0';
#endif // __N_TRIGGER_AREASET_050203	

#ifdef __WORLDSERVER
	m_apHeightMap	= NULL;
	m_apWaterHeight = NULL;
	m_apLocation = NULL;
	m_dwObjNum	= 0;
	m_dwBkgndNum	= 0;
	m_cbAddObjs		= 0;
	memset( m_lpszWorld, 0, sizeof(TCHAR) * 64 );
	m_cbUser	= 0;
	m_nThread	= 0;
	nextptr	= NULL;
	m_hIOCP	= (HANDLE)NULL;
	m_uWorker	= 0;
	m_lWork		= 0L;
	m_cbModifyLink	= 0;
	m_cbReplaceObj	= 0;
	m_hWait		= CreateEvent( NULL, FALSE, FALSE, NULL );
	ASSERT( m_hWait );
#ifdef __INSTANCE_DUNGEON
	m_bSrc	= FALSE;
#endif	// __INSTANCE_DUNGEON
#else	// __WORLDSERVER
	//g_pIB	= NULL;
	//g_pExIB	= NULL;
	m_fElapsedTime	=0;
	m_nVisibilityLand	= m_fFarPlane / ( MAP_SIZE * MPU );
	m_fFogStartValue	= 70;//m_fFarPlane - 384;	// kentauji edited 5/28
	m_fFogEndValue	= 400;//m_fFarPlane + 300;/// 2;	//1000.0f;
	m_fFogDensity	= 0.0f;
	m_pd3dDevice	= NULL;
	//m_pCloudVertexBuffer	= NULL;
	//m_pWaterVertexBuffer	= NULL;
	m_pBoundBoxVertexBuffer = NULL;
	m_nCharLandPosX = -1;
	m_nCharLandPosZ = -1;
	
	//m_nWaterVertexNum    	= 0;
	//m_nCloudVertexNum	    = 0;

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
	m_bViewLight = TRUE;
	m_bViewWeather = TRUE;//FALSE;
	m_bViewBoundBox	= FALSE;
	m_bCullObj = TRUE;
	m_bProcess = TRUE;
	m_bViewObj = TRUE;
	m_bViewMover = TRUE;
	m_bViewItem	= TRUE;
	m_bViewRegion = TRUE;
	m_bViewSpawn = TRUE;
//#ifdef __BEAST
//	m_bFixedHour = TRUE;
//#else
//	m_bFixedHour = FALSE;
//#endif
//	m_nFixedHour = 12;
	
#ifdef __CLIENT	
	m_nObjCullSize = 0;
	m_nSfxCullSize = 0;
	m_bViewHeightAttribute = FALSE;
#else
	m_bViewHeightAttribute = TRUE;
#endif
	m_bViewLODObj = TRUE;
	m_bViewIdState = FALSE;
	// 디버그 관련 플랙 끝 

	m_dwAmbient	= D3DCOLOR_ARGB( 255,128,128,128);
	m_pObjFocus		= NULL;
	m_idObjFocusOld	= NULL_ID;
	m_dwLightIndex	= 0;
	m_pCamera	= NULL;
	m_apLand	= NULL;

	// 디버그 관련 플랙  
#ifdef __BEAST
	//m_pGridVertexBuffer	    = NULL;
	//m_nGridVertexNum	    = 0   ;
	m_pBoundBoxVertexBuffer	= NULL;
	m_nBoundBoxVertexNum	= 0   ;
	//m_pCircleVertexBuffer	= NULL;
	//m_nCircleVertexNum	    = 0   ;
	m_pAxisVertexBuffer     = NULL;
	m_nAxisVertexNum        = 0   ;
	m_nLightType            = 0   ;
	m_nLightHour            = 12  ;
	m_lightDirect.r1        = 0.5f;
	m_lightDirect.g1        = 0.5f;
	m_lightDirect.b1        = 0.5f;
	m_lightDirect.r2        = 0.5f;
	m_lightDirect.g2        = 0.5f;
	m_lightDirect.b2        = 0.5f;
#endif	// __BEAST
#endif	// __WORLDSERVER
	m_dwWorldID	= NULL_ID;
	m_dwIdWorldRevival = WI_WORLD_NONE; 
	ZeroMemory( m_szKeyRevival, sizeof( m_szKeyRevival ) );	
	m_apLandAttr = NULL;

#ifndef __ONLINE
	m_bLoadScriptFlag = FALSE;
#endif
#ifdef  __V050322_CACHEQUEST
	m_timerScript.Set( MIN( 1 ) );
#endif // __VGLOVALSCRIPT_20050319
}

CWorld::~CWorld()
{
	Free();
}

void CWorld::Free()
{
#ifdef __WORLDSERVER
	CObj* pObjtmp;
	m_AddRemoveLock.Enter( theLineFile );
	for( i = 0; i < m_dwObjNum; i++ )
	{
		pObjtmp		= m_apObject[i];
		SAFE_DELETE( pObjtmp );
	}
	m_AddRemoveLock.Leave( theLineFile );
	
	// bkgr
	for( i = 0; i < m_dwBkgndNum; i++ )
	{
		pObjtmp	= m_apBkgnd[i];
		SAFE_DELETE( pObjtmp );
	}

	m_linkMap.Release();

	SAFE_DELETE_ARRAY( m_apHeightMap );
	SAFE_DELETE_ARRAY( m_apWaterHeight );
	SAFE_DELETE_ARRAY( m_apLocation );

	CCtrlRegion* pCtrlRegion;
	while( !m_lspCtrlRegion.empty() )
	{
		pCtrlRegion	= m_lspCtrlRegion.front();
		m_lspCtrlRegion.pop_front();
		SAFE_DELETE( pCtrlRegion );
	}
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
	/*
	// 나머지 파괴 
	#ifdef __BEAST
	//SAFE_RELEASE( m_pGridVertexBuffer );
	SAFE_RELEASE( m_pBoundBoxVertexBuffer );
	SAFE_RELEASE( m_pCircleVertexBuffer );
	#endif
	*/
	m_pObjFocus	= NULL;
	m_nDeleteObjs	= 0;

	for( ObjItor k = m_aryObjBuffer.begin(); k != m_aryObjBuffer.end(); ++k )
	{
		CObj* pObj = *k;
		delete pObj;
 	}
	m_aryObjBuffer.clear();

#endif	// __WORLDSERVER
#ifndef __BEAST
	int nSize = m_aRegion.GetSize();
	for( i = 0; i < nSize; i++ )
	{
		LPREGIONELEM lpRegionElem = m_aRegion.GetAt( i );
		for( int j = 0; j < lpRegionElem->m_cDescSize; j++ )
		{
			//SAFE_DELETE( lpRegionElem->m_apszDesc[ j ] );
		}
	}
	nSize = m_aBeginPos.GetSize();
	for( i = 0; i < nSize; i++ )
	{
		LPREGIONELEM lpRegionElem = m_aBeginPos.GetAt( i );
		for( int j = 0; j < lpRegionElem->m_cDescSize; j++ )
		{
			//SAFE_DELETE( lpRegionElem->m_apszDesc[ j ] );
		}
	}
#endif
	SAFE_DELETE_ARRAY( m_apLandAttr );
}
void CWorld::LoadAllMoverDialog()
{
#ifdef __CLIENT
	int nSize = m_nLandWidth * m_nLandHeight;
	for( int i = 0; i < nSize; i++ )
	{
		CLandscape* pLand = m_apLand[ i ];
		if( pLand )
		{
			CObj** pObjs = pLand->m_apObject[ OT_MOVER ];
			int nNum = pLand->m_adwObjNum[ OT_MOVER ];
			for( int k = 0; k < nNum; k++ )
			{
				if( pObjs[ k ] )
					((CMover*)pObjs[ k ])->LoadDialog();
			}
		}
	}
#endif
#ifdef __WORLDSERVER
	for( int i = 0; i < m_dwObjNum; i++ )
	{
		if( m_apObject[ i ] && m_apObject[ i ]->GetType() == OT_MOVER )
			((CMover*)m_apObject[ i ])->LoadDialog();
	}
#endif
#ifdef  __V050322_CACHEQUEST
#ifndef __BEAST
	m_WorldScript.Free();
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( m_szFileName, drive, dir, name, ext );
	CHAR szScriptName[ 128 ];
	MakePath( szScriptName, DIR_WORLD, name );
	strcat( szScriptName, "\\" );
	strcat( szScriptName, name );
	strcat( szScriptName, ".txt" );
	m_WorldScript.LoadScript( szScriptName );
	m_WorldScript.Run( "#init", NULL, 0, m_dwWorldID, m_dwWorldID, 0, WES_WORLD );
#endif // __BEAST
#endif // __V050322_CACHEQUEST

}
#ifdef __CLIENT
// pObj는 CItem도 들어온다.
void CWorld::SetObjFocus( CObj* pObj, BOOL bSend ) 
{ 
	CObj *pOldFocus = m_pObjFocus;
	
//	if( pObj && pObj->GetType() != OT_MOVER )		// 여기는 무버만 처리한다.
//	{
//		LPCTSTR szErr = Message( "CWorld::SetObjFocus : pObj != OT_MOVER %d %d", pObj->GetType(), pObj->GetIndex() );
//		ADDERRORMSG( szErr );
//	}

	if( m_pObjFocus && m_pObjFocus->GetType() == OT_MOVER )
		m_idObjFocusOld = ((CMover *)m_pObjFocus)->GetId();		// 한번 잡았던 오브젝트의 ID를 기억해둔다. 이오브젝트가 다시 Add되었을때 자동으로 다시 잡아준다.
	if( pOldFocus )	// 기존에 뭔가 잡혀있었는가.
	{
		if( pOldFocus->GetType() == OT_MOVER )		// 타겟이 Mover일때만 처리하자
		{
			CMover *pMoverTarget = (CMover *)pOldFocus;
			OBJID idTarget = pMoverTarget->GetId();
			// PK때에는 스틸처리가 적용되지 않음.
//			if( pMoverTarget->IsNPC() )
			{
				if( pOldFocus != pObj && pMoverTarget->IsLive() )		// 기존타겟과 다른타겟(널포함)을 잡으려 하면 서버에 알림
				{
					if( bSend )
						g_DPlay.SendSetTarget( idTarget, 1 );	// idTarget에게 g_pPlayer가 타겟을 해제했다는것을 알림.
					pMoverTarget->m_idTargeter = NULL_ID;	// 클라에도 클리어 시켜줘야 한다.
				}
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
		} else
		{
			if( bSend )
				g_DPlay.SendSetTarget( NULL_ID, 2 );	// 타겟 클리어.
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
#ifdef __BEAST
void CWorld::SetObjFocus( CObj* pObj ) 
{ 
	m_pObjFocus = pObj; 
}
#endif

// removed /2003/10/09

#ifndef __WORLDSERVER
void CWorld::AddLight( CLight* pLight )
{
	pLight->m_dwIndex = m_dwLightIndex++;
	m_mapLight.SetAt( pLight->m_strKey, pLight );
	//m_pd3dDevice->SetLight( pLight->m_dwIndex, pLight );
	//m_pd3dDevice->LightEnable( pLight->m_dwIndex, TRUE );
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
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		if( m_apAddObjs[i] == pObj )
		{
			pObj->SetWorld( NULL );
			m_apAddObjs[i]	= NULL;
			m_AddRemoveLock.Leave( theLineFile );	// unlock1
			return TRUE;
		}
	}
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return FALSE;
}
#endif	// __WORLDSERVER

BOOL CWorld::AddObj( CObj* pObj, BOOL bAddItToGlobalId )
{
	if( pObj->GetWorld() )
	{
		WriteError("CWorld::AddObj obj's world is NOT NULL." );
		return FALSE;
	}

	D3DXVECTOR3 v = pObj->GetPos();
	if( VecInWorld( v ) == FALSE )
	{
		WriteError("CWorld::AddObj world id:%d x:%f y:%f z:%f", GetID(), v.x, v.y, v.z );
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
	pObj->SetWorld( this );
	if( pObj->m_pAIInterface ) 
		pObj->m_pAIInterface->InitAI();

#ifdef __WORLDSERVER
	m_AddRemoveLock.Enter( theLineFile );
	ASSERT( m_cbAddObjs < MAX_ADDOBJS );
	m_bAddItToGlobalId[m_cbAddObjs]	= bAddItToGlobalId;
	m_apAddObjs[m_cbAddObjs++]	= pObj;
	m_AddRemoveLock.Leave( theLineFile );
#else	// __WORLDSERVER
	InsertObjLink( pObj );
	AddObjArray( pObj );
#endif	// __WORLDSERVER

#ifdef __CLIENT
	if( CMover::GetActiveMover()->m_pActMover->IsFly() )
	{
		if( pObj->GetType() == OT_MOVER )
		{
			if( ((CMover *)pObj)->GetId() == m_idObjFocusOld )
			{
				SetObjFocus( pObj );
			}
		}
	}
#endif

	return TRUE;
}

void CWorld::RemoveObj( CObj* pObj )
{
	if( pObj->GetWorld() == NULL ) 
		return;
	pObj->SetWorld( NULL );
#ifdef __WORLDSERVER
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1
#endif
	RemoveObjLink( pObj );
	RemoveObjArray( pObj );
#ifdef __WORLDSERVER
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
}

// 모든 루프가 끝나고 메모리에서 delete 된다.
void CWorld::DeleteObj( CObj* pObj )
{
	if( !pObj->IsDelete() )
	{
		pObj->SetDelete( TRUE );
#ifdef __WORLDSERVER
		if( m_nDeleteObjs >= MAX_DELETEOBJS )
		{
			if( pObj->GetType() == OT_MOVER )
				Error( "CWorld::DeleteObj : %s %d", ((CMover *)pObj)->GetName(), m_nDeleteObjs );
			else
				Error( "CWorld::DeleteObj : type=%d idx=%d %d", pObj->GetType(), pObj->GetIndex(), m_nDeleteObjs );
		}

		m_AddRemoveLock.Enter( theLineFile );
//		ASSERT( m_nDeleteObjs < MAX_DELETEOBJS );
		if( m_nDeleteObjs >= MAX_DELETEOBJS )
			Error( "MAX_DELETEOBJS" );
		m_apDeleteObjs[ m_nDeleteObjs++ ] = pObj;
		m_AddRemoveLock.Leave( theLineFile );
#else
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
		ASSERT( !m_apObject[nIndex] );
	}
	else
	{
		nIndex	= m_dwObjNum++;
	}

	if( nIndex == 0xffffffff )
	{
	#ifdef __IDC
		FILEOUT( "..\\error.log", "1)" );
	#else	// __IDC
		FILEOUT( "error.log", "1)" );
	#endif	// __IDC
		return FALSE;
	}
	m_apObject[nIndex]	= (CCtrl*)pObj;
	pObj->m_dwObjAryIdx	= nIndex;
#else	// __WORLDSERVER
	CLandscape* pLandscape	= GetLandscape( pObj );
	if( NULL != pLandscape ) 
	{
		pLandscape->AddObjArray( pObj );
	}
	else 
	{
#ifndef __BEAST
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "AOA error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
		DEBUGOUT2( szMessage );
#endif
	}
#endif	// __WORLDSERVER
	return TRUE;
}

void CWorld::RemoveObjArray( CObj* pObj )
{
#ifdef __WORLDSERVER
	if( pObj->m_dwObjAryIdx == 0xffffffff )
	{
		WriteError( "RemoveObjArray  pObj->m_dwObjAryIdx == 0xffffffff " );
		return;
	}

	if( m_apObject[pObj->m_dwObjAryIdx] == pObj )
	{
		m_ObjStack.Push( pObj->m_dwObjAryIdx );
		m_apObject[pObj->m_dwObjAryIdx]		= NULL;
	}
	else
	{
		WriteError( "RemoveObjArray m_apObject[pObj->m_dwObjAryIdx] != pObj " );
	}
#else	// __WORLDSERVER
	CLandscape* pLandscape	= GetLandscape( pObj );

	if( NULL != pLandscape ) 
	{
		pLandscape->RemoveObjArray( pObj );
	}
	else 
	{
#ifndef __BEAST
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "ROA error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
		DEBUGOUT2( szMessage );
#endif
	}
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
	else 
	{
#ifndef __BEAST
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "IOL error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
		DEBUGOUT2( szMessage );
#endif
	}
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
		WriteError( "ROL//INVALID POS//%d, %d, %f, %f, %f, %f, %f, %f", 
			pObj->GetType(), pObj->GetIndex(), pObj->GetPos().x, pObj->GetPos().y, pObj->GetPos().z, vPos.x, vPos.y, vPos.z );
		return FALSE;
	}
#ifdef __WORLDSERVER
	return m_linkMap.RemoveObjLink( pObj );
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;	// - m_nWorldY;
	int   mX = int( rX / MAP_SIZE );
	int   mZ = int( rZ / MAP_SIZE );

	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		return pLandscape->RemoveObjLink( pObj );
	}
	else {
#ifndef __BEAST
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "ROL error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, pObj->GetPos().x, pObj->GetPos().z );
		DEBUGOUT2( szMessage );
#endif
	}
	return FALSE;
#endif	// __WORLDSERVER
}

CObj* CWorld::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel )
{
	if( VecInWorld( vPos ) == FALSE )
		return NULL;
#ifdef __WORLDSERVER
	return m_linkMap.GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;// ;- m_nWorldY;
	int	mX	= int( rX / MAP_SIZE );
	int	mZ	= int( rZ / MAP_SIZE );
//	int	mX_	= int( int(rX) % MAP_SIZE );
//	int	mZ_	= int( int(rZ) % MAP_SIZE );
	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		return pLandscape->GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );	//m_aObjLinkMap[mX_ + mZ_ * MAP_SIZE];
	}
	else {
#ifndef __BEAST
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "GOILM error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, vPos.x, vPos.z );
		DEBUGOUT2( szMessage );
#endif
		return NULL;
	}
#endif	// __WORLDSERVER
}

BOOL CWorld::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj )
{
	if( VecInWorld( vPos ) == FALSE )
		return FALSE;
#ifdef __WORLDSERVER
	return m_linkMap.SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );
#else	// __WORLDSERVER
	FLOAT rX = vPos.x / MPU;	// - m_nWorldX;
	FLOAT rZ = vPos.z / MPU;	// - m_nWorldY;
	int mX	= int( rX / MAP_SIZE );
	int mZ	= int( rZ / MAP_SIZE );
//	int mX_	= int( int(rX) % MAP_SIZE );
//	int mZ_	= int( int(rZ) % MAP_SIZE );
	CLandscape* pLandscape	= m_apLand[mX + mZ * m_nLandWidth];
	if( NULL != pLandscape ) {
		pLandscape->SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );	//m_aObjLinkMap[mX_ + mZ_ * MAP_SIZE]	= pObj;
	}
	else {
#ifndef __BEAST
		char szMessage[260]	= { 0, };
		sprintf( szMessage, "SOILM error with landscape unloaded - g_pPlayer = %f, %f\t//pObj = %f, %f",
			g_pPlayer->GetPos().x, g_pPlayer->GetPos().z, vPos.x, vPos.z );
		DEBUGOUT2( szMessage );
#endif
	}
#endif	// __WORLDSERVER
	return TRUE;
} 

#ifdef __WORLDSERVER
void CWorld::Process( void )
{
	// INDEX 9
	ADDERRORSEQ( 9, 0 );
	ADDERRORTICK( 0, false );
#ifdef __ALL_PROCESS_SERIALIZE041005
	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1 
#endif

#ifdef __EH
	int i	= 0;
	try
	{
#endif	// __EH
	ADDERRORSEQ( 9, -1 );
	_add();
#ifdef __EH
	i	= 1;
#endif	// __EH
	ADDERRORSEQ( 9, 0 );
	_process();	// iocp
#ifdef __EH
	i	= 2;
#endif	// __EH
	ADDERRORSEQ( 9, 1 );
	_modifylink(); // _replace();
#ifdef __EH
	i	= 3;
#endif	// __EH
	ADDERRORSEQ( 9, 2 );
	_replace(); //_modifylink();
#ifdef __EH
	i	= 4;
#endif	// __EH
	ADDERRORSEQ( 9, 3 );
	_delete();
#ifdef __EH
	i	= 5;
#endif	// __EH
	ADDERRORSEQ( 9, 4 );
	if( m_bLoadScriptFlag )
	{
		LoadAllMoverDialog();
		m_bLoadScriptFlag = FALSE;
	}
	ADDERRORSEQ( 9, 5 );
#ifdef __EH
	}
	catch( ... )
	{
		WriteError( "%s %d, i = %d", __FILE__, __LINE__, i );
	}
#endif	// __EH

#ifdef __ALL_PROCESS_SERIALIZE041005
	prj.m_AddRemoveLock.Leave( theLineFile );	// lock1 
#endif

#ifdef  __V050322_CACHEQUEST
	//if( m_timerScript.IsTimeOut() ) 
	{
		m_timerScript.Reset();
		prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
		m_WorldScript.Run( "#auto", NULL, 0, m_dwWorldID, m_dwWorldID, 0, WES_WORLD );//GetId(),GetId()); WES_WORLD
		prj.m_AddRemoveLock.Leave( theLineFile );	// lock1 
	}
#endif // __V050322_CACHEQUEST

	ADDERRORTICK( 0, true );
}
#else	// __WORLDSERVER
void CWorld::Process( FLOAT fElapsedTime )
{
#ifdef __XUZHU2
	CObj::m_pObjHighlight = GetObjFocus();
#endif
	
	// 처리 프로세스 
	CHECK1();
	int i, j, k, l, x, y;
	WorldPosToLand( m_pCamera->m_vPos, x, y );
	CLandscape* pLand;
//	CObj* m_apObjProcess[ 5000 ];
	DWORD dwObjProcessNum = 0;
	D3DXVECTOR3 vPos, *pvCameraPos = &m_pCamera->m_vPos;
	float	fLengthSq;
	float	fFarSq = CWorld::m_fFarPlane / 2;
	fFarSq *= fFarSq;
	int		nNumObj;
	CObj* pObj;
	
#ifdef _DEBUG
	int _nCnt = 0;
#endif

#ifdef __CLIENT
	// 정적 오브젝트의 반투명 처리를 위한 프로세스 처리 
//	CObj* pNonCullObjs[ 10000 ];
	int nNonCullNum = 0;
	for( i = 0; i < m_nObjCullSize; i++ )
	{
		pObj = m_aobjCull[ i ];
		if( pObj && pObj->GetType() == OT_OBJ && pObj->GetModel()->m_pModelElem->m_bTrans && pObj->IsCull() == FALSE )
		{
			pObj->Process( 0 );
		}
	}
#endif	

	// OT_SHIP이 OT_ITEM이나 OT_MOVER보다 먼저 프로세스 되어야 해서 어쩔수 없이 이 방법을 씀.
	static int idx[ MAX_OBJARRAY ] = { 0, 1, 2, 7, 3, 4, 5, 6 };	
	for( i = y - m_nVisibilityLand; i <= y + m_nVisibilityLand; i++ )
	{
		for( j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++ )
		{
			if( LandInWorld( j, i ) && m_apLand[ i * m_nLandWidth + j ] )
			{
				pLand = m_apLand[ i * m_nLandWidth + j ];
#ifdef __BEAST
				for( k = 0; k < MAX_OBJARRAY; k++ )
#else				
				for( k = OT_ANI; k < MAX_OBJARRAY; k++ )
#endif
				{
					CObj** apObject = pLand->m_apObject[ idx[k] ];
					nNumObj = pLand->m_adwObjNum[ idx[k] ];
					for( l = 0; l < nNumObj; l++ )
					{
						if( *apObject && /*(*apObject)->m_pModel->IsAniable() &&*/ (*apObject)->IsDelete() == FALSE && (*apObject)->GetWorld() != NULL )
						{
							vPos = (*apObject)->GetPos() - *pvCameraPos;
							fLengthSq = D3DXVec3LengthSq( &vPos );
							if( fLengthSq < fFarSq ) 
							{
#ifdef _DEBUG
								_nCnt ++;
#endif								
								(*apObject)->Process( 0 );
							}
								//m_apObjProcess[ dwObjProcessNum++ ] = *apObject;
						}
						apObject ++;
					}
				}
			}
		}
	}
	CHECK2("    Visibi");
	
	// 자, 여기서 랜드에 속한 배열에서 바로 프로세싱하지 않고  apObjTemp 배열에 추가한 후에
	// Process한 것에 주목해야한다. Process 과정에서 오브제트가 삭제되거나 추가될 수 있는데
	// 랜드에 속한 배열을 루핑하면서 동일 배열에 추가되거나 삭제되게 되면  버그의  가능성이
	// 높아진다. 따라서 약간의 오버해드를 감수하면서 apObjTemp를 활용한다.
	CHECK1();
	//for( i = 0; i < dwObjProcessNum; i++ )
	//{
	//	m_apObjProcess[ i ]->Process( 0 );
	//}
	
	CHECK2("    prc");
	//	m_weather.Process( this );
	#ifdef __CLIENT
	g_DialogMsg.RemoveDeleteObjMsg();
	CWndTaskBar* pWndTaskBar = g_WndMng.m_pWndTaskBar;
	if( pWndTaskBar )
		pWndTaskBar->RemoveDeleteObj();
	#endif
	// 오브젝트 Delete ( DeleteObj가 호출된 오브젝트들)

	for( i = 0; i < m_nDeleteObjs; i++ )
	{
		pObj = m_apDeleteObjs[ i ];
		//#ifdef __CLIENT
		if( m_pObjFocus == pObj )
			SetObjFocus( NULL );
		if( CObj::m_pObjHighlight == pObj )
			CObj::m_pObjHighlight = NULL;
//#ifdef __NEWPROCESS
		// 화면에 출력되고 있는 오브젝트인가.
		if( pObj->m_ppViewPtr )
		{
			// 그렇다면 화면 출력 배열에서 자신을 삭제 
			*pObj->m_ppViewPtr = NULL;
			pObj->m_ppViewPtr = NULL;
		}
//#endif
		//#endif
		RemoveObjLink( pObj );
		RemoveObjArray( pObj );
		SAFE_DELETE( pObj );
	}
	m_nDeleteObjs = 0;
	#ifdef __CLIENT
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
#ifdef __CLIENT
		if( m_bViewWeather )
		{
			m_skyBox.m_pWorld = this;
			m_skyBox.Process( fElapsedTime );
		}
#endif
	}
#if !defined(__BEAST) && !defined(__ONLINE)
	//Respawn();
#endif	// __ONLINE
	#endif	// __CLIENT
#ifdef __CLIENT
	// 물 애니메이션 
	CLandscape::m_fWaterFrame += 0.15f;
	
#ifdef __YWATER_ANI_EX
	if( CLandscape::m_fWaterFrame >= 54.0f )
#else //__YWATER_ANI_EX
	if( CLandscape::m_fWaterFrame >= 34.0f )
#endif //__YWATER_ANI_EX
		CLandscape::m_fWaterFrame = 0.0f;	
	//int nTemp = ( CLandscape::m_fWaterFrame - (int)CLandscape::m_fWaterFrame ) * 10;
	// 10 : nTemp = 255 : x
//	nTemp * 255 / 10
#endif	
	/*
#ifdef __CLIENT
	if( m_pCamera )
	{
		m_pCamera->Process( m_pd3dDevice );
		ReadWorld( m_pCamera->m_vPos );
	}
#endif
	*/
#if defined( __V050322_CACHEQUEST ) && !defined(__ONLINE )
#ifndef __BEAST
	if( m_timerScript.IsTimeOut() ) 
	{
		m_timerScript.Reset();
		m_WorldScript.Run("#auto",NULL,0,m_dwWorldID,m_dwWorldID);//GetId(),GetId());
	}
#endif // __BEAST
#endif // __V050322_CACHEQUEST
}
#endif	// __WORLDSERVER

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
	x /= MPU; 
	z /= MPU;
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
		Message( "CWorld::GetLandHeight : %d %d,  %f %f,  %d %d", px, pz, _x, _z, m_nLandWidth, m_nLandHeight );

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
FLOAT CWorld::GetLandHeight( D3DXVECTOR3 vPos )
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
		ASSERT( 0 );
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
	if(GetLandTri2(x,		z,		pTris+offset)) offset+=6;
	if(GetLandTri2(x-MPU,	z-MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x,		z-MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x+MPU,	z-MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x-MPU,	z,		pTris+offset)) offset+=6;
	if(GetLandTri2(x+MPU,	z,		pTris+offset)) offset+=6;
	if(GetLandTri2(x-MPU,	z+MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x,		z+MPU,	pTris+offset)) offset+=6;
	if(GetLandTri2(x+MPU,	z+MPU,	pTris+offset)) offset+=6;
	return offset;
}
BOOL CWorld::GetLandTri2( float x, float z, D3DXVECTOR3* pTri )
{
	if( VecInWorld( x, z ) == FALSE ) return FALSE;

	x /= MPU; 
	z /= MPU;

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
			pTri[0] = D3DXVECTOR3((gx  ) * MPU, m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[1] = D3DXVECTOR3((gx+1) * MPU, m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[2] = D3DXVECTOR3((gx+1) * MPU, m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH], (gz  )*MPU );

			pTri[3] = D3DXVECTOR3((gx  ) * MPU, m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[4] = D3DXVECTOR3((gx  ) * MPU, m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[5] = D3DXVECTOR3((gx+1) * MPU, m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
		}
		else {
			//0,2,3
			pTri[0] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[1] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[2] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );

			pTri[3] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[4] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[5] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH], (gz  )*MPU );
		}
	}
	else 
	{
		if( glx + glz < 1.0f )
		{
			//0,2,1
			pTri[0] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[1] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[2] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH], (gz  )*MPU );

			pTri[3] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[4] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[5] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
		}
		else 
		{
			//1,2,3
			pTri[0] = D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[1] = D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[2] = D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );

			pTri[3] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH], (gz  )*MPU );
			pTri[4] = D3DXVECTOR3((gx  )*MPU, m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH], (gz+1)*MPU );
			pTri[5] = D3DXVECTOR3((gx+1)*MPU, m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH], (gz  )*MPU );
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

	//	pLand->GetHeight();;
	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);

			pTri[3]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[4]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[5]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);

			pTri[3]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[4]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[5]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);

			pTri[3]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[4]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[5]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);

			pTri[3]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[4]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[5]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
		}
	}
#endif
	return TRUE;
}

void CWorld::GetLandTri( float x, float z, D3DXVECTOR3* pTri )
{
	if( VecInWorld( x, z ) == FALSE ) return;

	x /= MPU; 
	z /= MPU;

#ifdef __WORLDSERVER
	int gx=(int)x;
	int gz=(int)z;
	float glx=x-gx;
	float glz=z-gz;

	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH],(gz  )*MPU);
			pTri[1]=D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH],(gz+1)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH],(gz  )*MPU);
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH],(gz  )*MPU);
			pTri[1]=D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH],(gz+1)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH],(gz+1)*MPU);
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz  )*WORLD_WIDTH],(gz  )*MPU);
			pTri[1]=D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH],(gz+1)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH],(gz  )*MPU);
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz  )*WORLD_WIDTH],(gz  )*MPU);
			pTri[1]=D3DXVECTOR3((gx  )*MPU,m_apHeightMap[(gx  )+(gz+1)*WORLD_WIDTH],(gz+1)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1)*MPU,m_apHeightMap[(gx+1)+(gz+1)*WORLD_WIDTH],(gz+1)*MPU);
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

	//	pLand->GetHeight();;
	if((gx+gz)%2==0) {
		if(glx>glz) {
			//0,3,1
			pTri[0]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
		}
		else {
			//0,2,3
			pTri[0]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
		}
	}
	else {
		if(glx+glz<1.0f) {
			//0,2,1
			pTri[0]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
		}
		else {
			//1,2,3
			pTri[0]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz  ),(gz  +lz*MAP_SIZE)*MPU);
			pTri[1]=D3DXVECTOR3((gx  +lx*MAP_SIZE)*MPU,pLand->GetHeight(gx  ,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
			pTri[2]=D3DXVECTOR3((gx+1+lx*MAP_SIZE)*MPU,pLand->GetHeight(gx+1,gz+1),(gz+1+lz*MAP_SIZE)*MPU);
		}
	}
#endif

}
#if 0
/*
FLOAT CWorld::GetFullHeight( D3DXVECTOR3 vPos, CObj* pExceptionObj )
{
	D3DXVECTOR3 vDir( 0.0f, -1.0f, 0.0f ); 
	D3DXVECTOR3 vIntersect;
	FLOAT fDist;              

	CObj* pObj;
	int nRange = 1;
	//return GetLandHeight( vPos.x, vPos.z );
#ifdef __WORLDSERVER
	FOR_LINKMAP( this, vPos, pObj, nRange )
	{
		if( pObj->GetType() == OT_OBJ && pObj != pExceptionObj )
		{
			// 오브젝트의 높이를 얻는다. 
			// 구하는 방법은 캐릭터의 키 높이에서 수직으로 지형을 향해 광선을 쏘아 충돌하는 오브젝트의 좌표를 얻는 것이다.
			// 내 키를 넘는 오브젝트는 높이를 얻어내지 못한다. 이러한 오브젝트는 기본적으로 진행 자체를 할 수 없으므로
			// 다른 충돌 알고리즘으로 충돌 여부를 확인해야한다.
			D3DXMATRIX mWorld = pObj->GetMatrixWorld();
			CModel* pModel = pObj->m_pModel;
			if( pModel->IntersectBB( vPos, vDir, mWorld, &vIntersect, &fDist ) == TRUE )
				if( pModel->Intersect( vPos, vDir, mWorld, &vIntersect, &fDist ) == TRUE )
					return vIntersect.y;
		}
	}
	END_LINKMAP
#else
	FOR_LINKMAP( this, vPos, pObj, nRange )
	{
		if( pObj->GetType() == OT_OBJ && pObj->GetIndex() >= 15 && pObj->GetIndex() != 27 && pObj->GetIndex() != 86 && pObj->GetIndex() != 87 )
		{
			D3DXMATRIX mWorld = pObj->GetMatrixWorld();
			CModel* pModel = pObj->m_pModel;
			if( pModel->IntersectBB( vPos, vDir, mWorld, &vIntersect, &fDist ) == TRUE )
				//return vIntersect.y;
				if( pModel->Intersect( vPos, vDir, mWorld, &vIntersect, &fDist ) == TRUE )
					return vIntersect.y;
		}
	}
	END_LINKMAP
#endif
	return GetLandHeight( vPos.x, vPos.z );
}
*/
#endif //0
/*
int GetMeshheight( CObj* pObj )
{
	if( pObj->GetType() == OT_OBJ )
	{
		// 오브젝트의 높이를 얻는다. 
		// 구하는 방법은 캐릭터의 키 높이에서 수직으로 지형을 향해 광선을 쏘아 충돌하는 오브젝트의 좌표를 얻는 것이다.
		// 내 키를 넘는 오브젝트는 높이를 얻어내지 못한다. 이러한 오브젝트는 기본적으로 진행 자체를 할 수 없으므로
		// 다른 충돌 알고리즘으로 충돌 여부를 확인해야한다.

		D3DXVECTOR3 vPickRayOrig = vPos;// - D3DXVECTOR3( 0.0f, 2.0f, 0.0f );
		D3DXVECTOR3 vPickRayDir( 0.0f, -1.0f, 0.0f ); ;
		D3DXVECTOR3 vIntersect;
		FLOAT fDist;              
		FLOAT fNearDist = m_fFarPlane;                    

		D3DXMATRIX mWorld = pObj->GetMatrixWorld();
		CModel* pModel = pObj->m_pModel;

		if( pModel->IntersectBB( vPickRayOrig, vPickRayDir, mWorld, &vIntersect, &fDist ) == TRUE )
		{
			if( pModel->Intersect( vPickRayOrig, vPickRayDir, mWorld, &vIntersect, &fDist ) == TRUE )
				return vIntersect.y;
		}
	}
	return -1;
}
GetAround( GetPos(), 1, GetModelHeight );

void CWorld::GetAround( D3DXVECTOR3 vPos, int nRange, BOOL (*pFunc)() )
{
	float rX = vPos.x / MPU;
	float rZ = vPos.z / MPU;
	for( int i = -nRange; i <= nRange; i++ )
	{
		for( int j = -nRange; j <= nRange; j++ )
		{
			int mX = int( ( rX + ( j * MPU ) ) / MAP_SIZE );
			int mZ = int( ( rZ + ( i * MPU ) ) / MAP_SIZE );
			CLandscape* pLand = m_apLand[ mX + mZ * m_nLandWidth ];
			if( pLand ) 
			{
				int nMaxWidth = MAP_SIZE;
				int nDiv = 1;
				for( int k = 0; k < 8; k++ )
				{
					D3DXVECTOR3 vPosTemp = vPos;
					vPosTemp.x /= MPU; vPosTemp.x -= pLand->m_nWorldX;
					vPosTemp.z /= MPU; vPosTemp.z -= pLand->m_nWorldY;
					vPosTemp.x /= nDiv;
					vPosTemp.z /= nDiv;
					int nPos = ( int( vPosTemp.z ) + i ) * nMaxWidth + ( int( vPosTemp.x ) + j );
					if( nPos >= 0 && nPos < nMaxWidth * nMaxWidth )
					{
						CObj** pObjs = pLand->m_aObjLinkMap2[ k ];
						CObj* pObj = pObjs[ nPos ];
						while( pObj )
						{
							if( (* pFunc->m_pFunc )( pObj ) == TRUE )
								return TRUE;
							pObj = pObj->GetNextNode();
						}
					}
					nDiv *= 2;
					nMaxWidth /= 2;
				}
			}
		}
	}
	return FALSE;
}
*/
#ifndef __BEAST
// 임시임...
extern int		IsTouchAABB( const D3DXVECTOR3 &vMin1, const D3DXVECTOR3 &vMax1,  const D3DXVECTOR3 &vMin2, const D3DXVECTOR3 &vMax2 );
// vMin, vMax와 겹친 오브젝트를 찾는다.
// pActor(행위자)는 검사대상에서 제외
CMover*		CWorld::FindTouchObj( CObj *pActor, D3DXVECTOR3 vMin, D3DXVECTOR3 vMax )
{
	D3DXVECTOR3	vMin2, vMax2;
	D3DXVECTOR3	vPos = pActor->GetPos();
//	CObj* pObj;
//	CMover *pMover;
	int nRange = 0;

#ifdef __WORLDSERVER
#else	// __WORLDSERVER
#endif	// __WORLDSERVER
	return NULL;
}
#endif	// __BEAST

/*
#ifdef __WORLDSERVER
void CWorld::AddMoveLandObj( CObj* pObj )
{
	m_apMoveLandObjs[ m_nMoveLandObjs++ ] = pObj;
}
#endif
*/

#ifdef __WORLDSERVER
void CWorld::AddItToView( CCtrl* pCtrl )
{
	m_linkMap.AddItToView( pCtrl );
}

#ifdef _DEBUG
int nTick	= 0;
int nCall	= 0;
int nTime	= 0;
#endif

void CWorld::ModifyView( CCtrl* pCtrl )
{
	m_linkMap.ModifyView( pCtrl );
}


BOOL CWorld::PreremoveObj( OBJID objid )
{
	CObj* pObj;
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		pObj	= m_apAddObjs[i];
		if( pObj && pObj->IsDynamicObj() && ( (CCtrl*)pObj )->GetId() == objid )
		{
			SAFE_DELETE( m_apAddObjs[i] );
			m_AddRemoveLock.Leave( theLineFile );	// unlock1
			return TRUE;
		}
	}
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return FALSE;
}

CObj* CWorld::PregetObj( OBJID objid )
{
	CObj* pObj;
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		pObj	= m_apAddObjs[i];
		if( pObj && pObj->IsDynamicObj() && ( (CCtrl*)pObj )->GetId() == objid )
		{
			m_AddRemoveLock.Leave( theLineFile );	// unlock1
			return pObj;
		}
	}

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return NULL;
}

void CWorld::_add( void )
{
	CObj* pObj;
	BOOL bAddItToGlobalId;

//	CMclAutoLock	Lock2( prj.m_AddRemoveLock );
//	CMclAutoLock	Lock( m_AddRemoveLock );
	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
	m_AddRemoveLock.Enter( theLineFile );	// lock2

	g_DPCoreClient.CheckIdStack();

	for( int i = 0; i < m_cbAddObjs; i++ )
	{
		pObj	= m_apAddObjs[i];
		if( NULL == pObj )	
			continue;

#ifdef __WORLDSERVER
		if( !pObj->IsVirtual() )
#endif	// __WORLDSERVER
		{
			if( !InsertObjLink( pObj ))			// 링크맵에 넣는다 ( 에러가 날 수 있다 )
				continue;
		}

		if( !AddObjArray( pObj ))			// m_apObject 에 넣는다.
		{
			WriteError( "if( !AddObjArray( pObj ))" );
			RemoveObjLink( pObj );
			continue;
		}

#ifdef __WORLDSERVER
		if( !pObj->IsVirtual() )
#endif	// __WORLDSERVER
		{
			if( pObj->IsDynamicObj() ) 
			{
				if( m_bAddItToGlobalId[i] )
					( (CCtrl*)pObj )->AddItToGlobalId();	// prj.m_objmap 와 prj.m_idPlayerToUserPtr에 넣는다.
				AddItToView( (CCtrl*)pObj );
			}
		}
	}
	m_cbAddObjs		= 0;

	m_AddRemoveLock.Leave( theLineFile );	// unlock2
	prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

void CWorld::CreateWorker( u_long uWorker )
{
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0/*(DWORD)uWorker*/ );
//	m_hIOCP	= CreateNewCompletionPort( (DWORD)uWorker );
	ASSERT( m_hIOCP );
	m_uWorker	= uWorker;
	HANDLE hThread;
	DWORD dwThreadId;
	for( int i = 0; i < m_uWorker; i++ )
	{
		hThread		= chBEGINTHREADEX( NULL, 0, WorkerThread_0123, this, 0, &dwThreadId );
		ASSERT( hThread );
#ifdef __THREADPRIORITY1010
		SetThreadPriority( hThread, THREAD_PRIORITY_ABOVE_NORMAL );
#else	// __DEADLOCK__
		SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
#endif	// __DEADLOCK__
		
		m_listthread.push_back( hThread );
	}
}

void CWorld::CloseWorker( void )
{
	u_long cbThread;
	LPHANDLE lphThread;
	
	cbThread	= m_listthread.size();
	if( cbThread > 0 )
	{
		lphThread	= new HANDLE[cbThread];
		for( int i = 0; i < cbThread; i++ )
		{
			lphThread[i]	= m_listthread.front();
			m_listthread.pop_front();
			PostQueuedCompletionStatus( m_hIOCP, CLOSEWORKER, TRUE, NULL );
		}
		WaitForMultipleObjects( cbThread, lphThread, TRUE, INFINITE );
		for( i = 0; i < cbThread; i++ ) {
			CloseHandle( (HANDLE)lphThread[i] );
		}
		SAFE_DELETE_ARRAY( lphThread );
	}
//	m_uWorker	= 0;
	CLOSE_HANDLE( m_hIOCP );
}

void CWorld::_modifylink( void )
{
	CObj* pObj, *pObjtmp;
	D3DXVECTOR3 vOld, vCur, vOldtmp, vCurtmp;
	int nLinkLevel;
	DWORD dwLinkType;
#ifdef __DEADLOCK__
	prj.m_AddRemoveLock.Enter( theLineFile );
#endif	// __DEADLOCK__
//	CMclAutoLock	Lock( m_csModifyLink );
	m_csModifyLink.Enter( theLineFile );		// lock1

	for( int i = 0; i < m_cbModifyLink; i++ )
	{
		pObj	= m_apModifyLink[i];
		if( IsInvalidObj( pObj ) )	
			continue;

		if( pObj->GetWorld() != this )
		{
			WriteError( "LINKMAP world different" );	// temp
			continue;
		}

		vOld	= pObj->GetLinkPos();
		vCur	= pObj->GetRemovalPos();
		vOldtmp		= vOld	/ MPU;
		vCurtmp		= vCur / MPU;

		if( (int)vOldtmp.x != (int)vCurtmp.x || (int)vOldtmp.z != (int)vCurtmp.z )
		{
			dwLinkType	= pObj->GetLinkType();
			nLinkLevel	= (int)pObj->GetLinkLevel();

			if( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() && nLinkLevel != 0 )
				WriteError( "ML//%s//%d//%d", ( (CMover*)pObj )->GetName(), ( (CMover*)pObj )->m_idPlayer, nLinkLevel );
			if( !pObj->m_pPrev && GetObjInLinkMap( vOld, dwLinkType, nLinkLevel ) != pObj )
			{
				WriteError( "ML//BINGO//%d//%d//%d", pObj->GetType(), pObj->GetIndex(), pObj->GetLinkLevel() );
				RemoveObjLink2( pObj );
			}

			if( GetObjInLinkMap( vOld, dwLinkType, nLinkLevel ) == pObj )
				SetObjInLinkMap( vOld, dwLinkType, nLinkLevel, pObj->m_pNext );
			pObj->DelNode();

			if( ( pObjtmp = GetObjInLinkMap( vCur, dwLinkType, nLinkLevel ) ) )
				pObjtmp->InsNextNode( pObj );
			else
				SetObjInLinkMap( vCur, dwLinkType, nLinkLevel, pObj );

			pObj->SetLinkPos( pObj->GetRemovalPos() );
			pObj->SetRemovalPos( D3DXVECTOR3( 0, 0, 0 ) );

			if( pObj->IsDynamicObj() )
				ModifyView( ( CCtrl* )pObj );
		}
	}
	m_cbModifyLink	= 0;

	m_csModifyLink.Leave( theLineFile );		// unlock1
#ifdef __DEADLOCK__
	prj.m_AddRemoveLock.Leave( theLineFile );
#endif	// __DEADLOCK__
}

void CWorld::_delete( void )
{
	CObj* pObj;

//	CMclAutoLock	Lock( prj.m_AddRemoveLock );
//	CMclAutoLock	Lock1( m_AddRemoveLock );
//	CMclAutoLock	Lock2( m_csReplaceObj );
//	CMclAutoLock	Lock3( m_csModifyLink );
	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
	m_AddRemoveLock.Enter( theLineFile );	// lock2
	m_csReplaceObj.Enter( theLineFile );	// lock3
	m_csModifyLink.Enter( theLineFile );	// lock4

	for( int i = 0; i < m_nDeleteObjs; i++ )
	{
		pObj	= m_apDeleteObjs[i];

		for( int j = 0; j < m_cbReplaceObj; j++ )
		{
			if( m_aReplaceObj[j].pObj == pObj )
				m_aReplaceObj[j].pObj	= NULL;
		}
		
		for( j = 0; j < m_cbModifyLink; j++ )
		{
			if( m_apModifyLink[j] == pObj )
			{
				m_apModifyLink[j]	= NULL;
				pObj->m_vRemoval	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			}
		}

#if 0
		if( pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() )
			m_cbUser--;
#endif
		
#ifdef __WORLDSERVER
		if( !pObj->IsVirtual() )
#endif	// __WORLDSERVER
			RemoveObjLink( pObj );
		RemoveObjArray( pObj );
		SAFE_DELETE( pObj );
	}
	m_nDeleteObjs	= 0;

	m_csModifyLink.Leave( theLineFile );	// unlock4
	m_csReplaceObj.Leave( theLineFile );	// unlock3
	m_AddRemoveLock.Leave( theLineFile );	// unlock2
	prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

void CWorld::_replace( void )
{
	CMover*		pMover;
	DWORD		dwWorldID;
	D3DXVECTOR3 vPos;
	DPID		dpid;
	u_long		uIdofMulti;
	
	prj.m_AddRemoveLock.Enter( theLineFile );	
	g_UserMng.m_AddRemoveLock.Enter( theLineFile );
	m_csReplaceObj.Enter( theLineFile );		
	m_csModifyLink.Enter( theLineFile );

	for( int i = 0; i < m_cbReplaceObj; ++i )
	{
		pMover	= m_aReplaceObj[i].pObj;
		if( ::IsInvalidObj( pMover ) )
			continue;

		dwWorldID	= m_aReplaceObj[i].dwWorldID;
		vPos  	    = m_aReplaceObj[i].vPos;
		dpid	    = m_aReplaceObj[i].dpid;
		uIdofMulti	= m_aReplaceObj[i].uIdofMulti;

		if( uIdofMulti != g_uIdofMulti )		// 현재구현에서는 보류 
			continue;

		if( GetID() == dwWorldID )
		{
			if( vPos.y == 0.0f )
			{
				vPos.y = 1000.0f;
				vPos.y = GetFullHeight( vPos );  // 검사 
			}
			pMover->SetPos( vPos );
			g_UserMng.AddSetPos( (CCtrl*)pMover, vPos );
			if( pMover->IsPlayer() )
				( (CUser*)pMover )->Notify();	
		}
		else
		{	
			if( pMover->IsPlayer() == FALSE )
				continue;

			CUser* pUser = (CUser*)pMover;
			CWorld* pWorld = g_WorldMng.GetWorld( dwWorldID );
			if( pWorld && pWorld->VecInWorld( vPos ) )	
			{
				pUser->RemoveItFromView( TRUE );		// pc
				pUser->RemoveItFromView2( TRUE );		// npc
				pUser->AddReplace( dwWorldID, vPos );
				pUser->Notify();	
				RemoveObj( pUser );						// pWorld = NULL, RemoveObjLink, RemoveObjArray
				pUser->SetPos( vPos );
				pUser->PCSetAt( pUser->GetId(), pUser );
				pUser->AddAddObj( pUser );
				pWorld->AddObj( pUser, FALSE );			// set pWorld 
				g_UserMng.AddHdr( pUser, SNAPSHOTTYPE_RESETBUFFSKILL );	// 버프스킬 다시 세팅
			}
			else
			{
				ASSERT( 0 );
				WriteError("_replace world id:%d x:%f y:%f z:%f", dwWorldID, vPos.x, vPos.y, vPos.z );
			}
		}
	}
	m_cbReplaceObj = 0;

	m_csModifyLink.Leave( theLineFile );
	m_csReplaceObj.Leave( theLineFile );
	g_UserMng.m_AddRemoveLock.Leave( theLineFile );
	prj.m_AddRemoveLock.Leave( theLineFile );
}

void CWorld::_process( void )
{
	prj.m_AddRemoveLock.Enter( theLineFile );

	int cb, val, i, c;
	cb = m_dwObjNum;

	if( cb )	
	{
		val		= ( cb % m_uWorker ) == 0? (int)( cb / m_uWorker ): (int)( cb / m_uWorker + 1 );
		m_lWork		= ( cb % val ) == 0? (int)( cb / val ): (int)( cb / val + 1 );
		long lWork	= m_lWork;

		i	= c	= 0;
		while( cb > 0 )
		{
			c	= ( cb > val? val: cb );
			PostQueuedCompletionStatus( m_hIOCP, c, i, NULL );
			i	+= c;
			cb	-= c;
			lWork--;
		}
#ifdef __DEADLOCK__
		if( lWork )
			Error( "lWork NOT ZERO" );
#endif	// __DEADLOCK__
		WaitForSingleObject( m_hWait, INFINITE );
	}

	prj.m_AddRemoveLock.Leave( theLineFile );
}

u_int	__stdcall WorkerThread_0123( LPVOID lpv )
{
#ifdef __INFOCRITSECT_DEBUG__
	CThreadDesc	td( "WorkerThread_0123" );
#endif	// __INFOCRITSECT_DEBUG__

#ifdef __DEADLOCK__
	static long s_lIdx	= 0;
	long lIdx	= InterlockedIncrement( &s_lIdx );
#endif	// __DEADLOCK__

	CWorld* pWorld	= (CWorld*)lpv;

	LPOVERLAPPED lpov;
	DWORD cb;
	int l, r;
	
	BOOL fOk;
	while( 1 )
	{
		fOk		= GetQueuedCompletionStatus( pWorld->GetIOCP(), &cb, (LPDWORD)&l, &lpov, INFINITE );

		if( cb == CLOSEWORKER )
			return( 0 );
		
		if( !fOk || cb == 0 )
		{
			ASSERT( 0 );
		}
		
		CObj* pObj;
		r	= l + cb;
		for( int i = l; i < r; i++ )
		{
			pObj	= pWorld->m_apObject[i];
			if( pObj && !pObj->IsDelete() )
			{
				pObj->Lock( theLineFile );
#ifdef __DEADLOCK__
				pObj->Process( (FLOAT)lIdx );
#else	// __DEADLOCK__
				pObj->Process( 0 );
#endif	// __DEADLOCK__
				pObj->Unlock( theLineFile );
			}
		}

		LONG lThreadsBusy	= InterlockedDecrement( &pWorld->m_lWork );
		if( lThreadsBusy == 0 )
			SetEvent( pWorld->m_hWait );
//		Sleep( 0 );
	}
	return( 0 );
}
#endif	// __WORLDSERVER

#if !defined(__BEAST) && !defined(__ONLINE)
static REGIONELEM g_regionElem;

LPREGIONELEM CWorld::GetBeginPos( int i ) 
{ 
	LPREGIONELEM lpRegionElem;
	lpRegionElem = m_aBeginPos.GetAt( i ); 
	if( lpRegionElem ) return lpRegionElem;
	return &g_regionElem;
}
LPREGIONELEM CWorld::GetRandomBeginPos()
{
	LPREGIONELEM apRegionElem[ 100 ];
	int nSize = 0;
	for( int i = 0; i < m_aBeginPos.GetSize(); i++ )
	{
		if( m_aBeginPos.GetAt( i ) )
			apRegionElem[ nSize++ ] = m_aBeginPos.GetAt( i );
	}
	if( nSize )
		return apRegionElem[ xRandom( nSize ) ];
	g_regionElem.m_vPos.x = 50.0f;
	g_regionElem.m_vPos.y = 50.0f;
	g_regionElem.m_vPos.z = 50.0f;
	return &g_regionElem;
}
#endif
#ifndef __WORLDSERVER
LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z )
{
	if( VecInWorld( x, z ) == FALSE )
		return NULL;
	x /= MPU;
	z /= MPU;
	if( x < 0 || z < 0 || (int)x >= m_nLandWidth * MAP_SIZE || (int)z >= m_nLandHeight * MAP_SIZE )
		return 0;
	float rX = x;// - m_nWorldX;
	float rZ = z;// - m_nWorldY;
	int mX=int(rX/MAP_SIZE);
	int mZ=int(rZ/MAP_SIZE);
	
	if( m_apLand[ mX + mZ * m_nLandWidth ] == NULL )
		return 0;
	
	int tx = ( x % MAP_SIZE ) / PATCH_SIZE;
	int tz = ( z % MAP_SIZE ) / PATCH_SIZE;
	
	return m_apLand[ mX + mZ * m_nLandWidth ]->GetWaterHeight( tx, tz );
}
int CWorld::GetHeightAttribute(int x, int z )
{
	if( VecInWorld( x, z ) == FALSE )
		return -1;
	x /= MPU;
	z /= MPU;
	int rX = x;
	int rZ = z;
	int mX=int( rX / MAP_SIZE);
	int mZ=int( rZ / MAP_SIZE);
	if( m_apLand[mX + mZ * m_nLandWidth] == NULL ) 
		return -1;
	return m_apLand[ mX + mZ * m_nLandWidth ]->GetHeightAttribute( x-(mX*MAP_SIZE), z-(mZ*MAP_SIZE) );
}

#endif // not WORLDSERVER

#ifdef __WORLDSERVER
int CWorld::GetHeightAttribute( int x, int z )
{
//	if( x < 0 || x > MAP_SIZE || z < 0 || z > MAP_SIZE ) 
//		return -1;

	if( VecInWorld( x, z ) == FALSE )
		return -1;

	x /= MPU;
	z /= MPU;

//	FLOAT fHeight = m_apHeightMap[ x + z * ( MAP_SIZE + 1 ) ];
	FLOAT fHeight	= m_apHeightMap[ x + z * WORLD_WIDTH ];
	if( fHeight >= HGT_NOWALK )
	{
		if( fHeight >= HGT_DIE )
			return HATTR_DIE;
		if( fHeight >= HGT_NOMOVE )
			return HATTR_NOMOVE;
		if( fHeight >= HGT_NOFLY )
			return HATTR_NOFLY;
		if( fHeight >= HGT_NOWALK )
			return HATTR_NOWALK;
		return HATTR_NONE;
	}
	return HATTR_NONE;
}
LPWATERHEIGHT CWorld::GetWaterHeight(int x, int z )
{
//	if( x < 0 || x > MAP_SIZE || z < 0 || z > MAP_SIZE ) 
//		return -1;

	if( VecInWorld( x, z ) == FALSE )
		return NULL;

	x /= MPU;
	z /= MPU;

	int tx = x / PATCH_SIZE;
	int tz = z / PATCH_SIZE;

//	FLOAT fHeight = m_apHeightMap[ x + z * ( MAP_SIZE + 1 ) ];
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
		Message( "CWorld::SendDamageAround : D:%d,%d,%d A:%s %d %f", pvPos->x, pvPos->y, pvPos->z, pAttacker->GetName(), nAttackID, fRange );

	ItemProp* pProp;
	if( nDmgType == AF_MAGICSKILL )
	{
		pProp = prj.GetSkillProp( nAttackID );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
		if( pProp == NULL )
		{
			Error( "CWorld::SendDamageAround : %s. 스킬(%d)의 프로퍼티가 업ㅂ다.", pAttacker->GetName(), nAttackID );
			return;	// property not found
		}
	} else
	{
		pProp = prj.GetItemProp( nAttackID );		// 아이템 프로퍼티꺼냄
		if( pProp == NULL )
		{
			Error( "CWorld::SendDamageAround : %s. 아이템(%d)의 프로퍼티가 업ㅂ다.", pAttacker->GetName(), nAttackID );
			return;	// property not found
		}
	}
	
	BOOL	bDamage = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkPlayer )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				if( pAttacker->IsPlayer() )
				{
#ifdef __NPP_050308
					bDamage = TRUE;
#else
					if( pAttacker->m_nDuel == 2 )	// 공격자가 파티듀얼중일땐 
					{
						if( ((CMover*)pObj)->m_idparty == pAttacker->m_idDuelParty )	// 상대 파티원에게만 데미지
							bDamage = TRUE;
					} else
					if( pAttacker->m_nDuel == 1 ) // 공격자가 그냥 듀얼중일땐.
					{
						if( ((CMover*)pObj)->GetId() == pAttacker->m_idDuelOther )	// 듀얼상대에게만
							bDamage = TRUE;
					}
#endif // __NPP_050308
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
		FOR_LINKMAP( this, vPos, pObj, nRange, CObj::linkDynamic )
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
						(*apObject)->Process( 0 );
					apObject++;
				}
			}
		}
	}
}
#endif	// __CLIENT