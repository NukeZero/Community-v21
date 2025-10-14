#include "stdafx.h"
#include "defineObj.h"
#include "WorldMng.h"
#include "World.h"
#include "Sfx.h"
#ifdef __CLIENT
#include "DialogMsg.h"
#endif // CLIENT

#ifdef __WORLDSERVER
#include "ServerDesc.h"
#include "DPCoreClient.h"
#include "DPSrvr.h"
#include "MyTrace.h"
#include "User.h"

extern	CDPCoreClient	g_DPCoreClient;
extern	CDPSrvr		g_DPSrvr;
extern	CMyTrace	g_MyTrace;
extern	CUserMng	g_UserMng;
#endif	// __WORLDSERVER
  
CWorldMng::CWorldMng()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CWorldMng::m_AddRemoveLock" )
#endif	// __INFOCRITSECT_DEBUG__
{
#ifdef __WORLDSERVER
	m_pFirstActive	= NULL;
	m_lCount	= 0;
#else	// __WORLDSERVER
	SetSize( 2, 4, 4 );
	m_pWorld	= NULL;
#endif	// __WORLDSERVER
	m_nSize = 0;
}


CWorldMng::~CWorldMng()
{
	Free();
}
#ifndef __WORLDSERVER
void CWorldMng::Free( void )
{
#ifdef __BEAST
	for( int i = 0; i < m_aWorld.GetSize(); i++ )
	{
		if( m_aWorld.GetAt( i ) )
			delete (LPWORLD)m_aWorld.GetAt( i );
	}
#else
	CMyBucket2<CWorld*>* pBucket	= GetFirstActive();
	while( pBucket )
	{
		SAFE_DELETE( pBucket->m_value );
		pBucket	= pBucket->pNext;
	}
#endif
	m_aWorld.RemoveAll();
}
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
void CWorldMng::Free( void )
{
	CWorld* pWorld, *pRemove;
//	CMclAutoLock Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1
	
	pWorld	= GetFirstActive();
	while( ( pRemove = pWorld ) )
	{
		pWorld	= pWorld->nextptr;
		delete pRemove;
	}
	m_pFirstActive	= NULL;
	m_aWorld.RemoveAll();

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

/*
void CWorldMng::Process( void )
{
	CWorld* pWorld;
	CMclAutoLock Lock( m_AddRemoveLock );
	
	pWorld	= GetFirstActive();
	while( pWorld )
	{
		pWorld->Process( 0 );
		pWorld	= pWorld->pNext;
	}
}
*/

u_long	CWorldMng::Respawn( void )
{
	CWorld* pWorld;
	u_long	uRespawned	= 0;
	
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	pWorld	= GetFirstActive();
	while( pWorld )
	{
#ifdef __INSTANCE_DUNGEON
		if( !pWorld->m_bSrc )
#endif	// __INSTANCE_DUNGEON
			uRespawned	+= pWorld->Respawn();
		pWorld	= pWorld->nextptr;
	}

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return uRespawned;
}

void CWorldMng::ReadObject( void )
{
	CWorld* pWorld;
//	CMclAutoLock Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1
	
	pWorld	= GetFirstActive();
	while( pWorld )
	{
		pWorld->LoadObject();
#ifndef __S1108_BACK_END_SYSTEM
		pWorld->LoadRegion();
#endif // __S1108_BACK_END_SYSTEM
		pWorld	= pWorld->nextptr;
	}

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

void CWorldMng::Add( CJurisdiction* pJurisdiction )
{
	LPWORLD lpWorld	= GetWorldStruct( pJurisdiction->m_dwWorldID );
	ASSERT( lpWorld );

//	CMclAutoLock Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	CWorld* pWorld	= GetWorld( pJurisdiction->m_dwWorldID );
	if( pWorld )
	{
		char sz[160];
		g_MyTrace.GetText( CMyTrace::Key( lpWorld->m_szFileName ), sz );
		g_MyTrace.Add( CMyTrace::Key( lpWorld->m_szFileName ),
											FALSE,
											"%s %-3d, %-3d, %-3d*%-3d",
											sz,
											pJurisdiction->m_rect.left,
											pJurisdiction->m_rect.top,
											pJurisdiction->m_rect.Width(),
											pJurisdiction->m_rect.Height()
										);

		pWorld->ReadWorld( pJurisdiction->m_rect );
	}
	else
	{
		g_MyTrace.Add( CMyTrace::Key( lpWorld->m_szFileName ),
											FALSE,
											"%-16s %-3d, %-3d, %-3d*%-3d",
											lpWorld->m_szFileName,
											pJurisdiction->m_rect.left,
											pJurisdiction->m_rect.top,
											pJurisdiction->m_rect.Width(),
											pJurisdiction->m_rect.Height()
										);
		
		pWorld	= new CWorld;

		pWorld->m_dwWorldID		= pJurisdiction->m_dwWorldID;
		pWorld->OpenWorld( MakePath( DIR_WORLD, lpWorld->m_szFileName ), TRUE );
		pWorld->ReadWorld( pJurisdiction->m_rect );

		pWorld->nextptr	= m_pFirstActive;
		m_pFirstActive	= pWorld;
		m_lCount++;
	}
#ifdef __INSTANCE_DUNGEON
	pWorld->m_bSrc	= pJurisdiction->m_bSrc;
#endif	// __INSTANCE_DUNGEON
#ifdef __NPP_050308
	strcpy( pWorld->m_szWorldName, lpWorld->m_szWorldName );
#endif // __NPP_050308

	CCtrlRegion* pCtrlRegion	= new CCtrlRegion( pJurisdiction );
	pWorld->m_lspCtrlRegion.push_back( pCtrlRegion );

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

CWorld* CWorldMng::GetWorld( DWORD dwWorldID )
{
	CWorld* pWorld	= GetFirstActive();
	while( pWorld )
	{
		if( pWorld->GetID() == dwWorldID )
			return( pWorld );
		pWorld	= pWorld->nextptr;
	}
	return( FALSE );
}

BOOL CWorldMng::AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId )
{
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	BOOL bResult = FALSE;
	CWorld* pWorld = GetWorld( dwWorldID );
	if( pWorld )
		bResult = pWorld->AddObj( pObj, bAddItToGlobalId );

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return bResult;
}

BOOL CWorldMng::PreremoveObj( OBJID objid )
{
//	CMclAutoLock Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	CWorld* pWorld	= GetFirstActive();
	while( pWorld )
	{
		if( TRUE == pWorld->PreremoveObj( objid ) )
		{
			m_AddRemoveLock.Leave( theLineFile );	// unlock1
			return TRUE;
		}
		pWorld	= pWorld->nextptr;
	}

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return FALSE;
}

CObj* CWorldMng::PregetObj( OBJID objid )
{
//	CMclAutoLock Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	CObj* pObj;
	CWorld* pWorld	= GetFirstActive();
	while( pWorld )
	{
		pObj	= pWorld->PregetObj( objid );
		if( NULL != pObj )
		{
			m_AddRemoveLock.Leave( theLineFile );	// unlock1
			return pObj;
		}
		pWorld	= pWorld->nextptr;
	}

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
	return NULL;
}

void CWorldMng::Unify( WORD wWorldSrvr )	// remove specific world server's
{
}

void CWorldMng::Duplicate( WORD wWorldSrvr )	// copy object to specific world server
{
}
#else	// __WORLDSERVER
#	ifdef __BEAST
DWORD CWorldMng::AddWorldStruct( LPCTSTR lpszFileName )
{
	LPWORLD lpWorld = new WORLD;
	ZeroMemory( lpWorld, sizeof( WORLD ) );
	_tcscpy( lpWorld->m_szFileName, lpszFileName );
	lpWorld->m_dwId = m_aWorld.Add( lpWorld );
	return lpWorld->m_dwId;
	/*

	for( int i = 0; i < MAX_WORLD; i++ )
	{
		lpWorld = &m_aWorld[ i ];
		if( lpWorld->m_szFileName[ 0 ] == 0 )
		{
			_tcscpy( lpWorld->m_szFileName, lpszFileName );
			lpWorld->m_dwId = i;
			return i;
		}
	}
	*/
	return NULL_ID;
}
BOOL CWorldMng::RemoveWorldStruct( OBJID idWorld )
{
	delete m_aWorld.GetAt( idWorld );
	m_aWorld.RemoveAt( idWorld );
	return TRUE;
	/*
	if( idWorld >= MAX_WORLD ) return FALSE;
	LPWORLD lpWorld = &m_aWorld[ idWorld ];
	ZeroMemory( lpWorld, sizeof( WORLD ) );
	return TRUE;
	*/
}
#	else	// __BEAST
CWorld* CWorldMng::Open( LPDIRECT3DDEVICE9 pd3dDevice, OBJID idWorld )
{
	CWorld* pWorld	= NULL;
	LPWORLD	lpWorld	= GetWorldStruct( idWorld );
	if( lpWorld ) {
		pWorld	= Open( pd3dDevice, lpWorld->m_szFileName );
		pWorld->m_dwWorldID	= idWorld;
#ifdef __NPP_050308
		strcpy( pWorld->m_szWorldName, lpWorld->m_szWorldName );
#endif // __NPP_050308
	}
	return pWorld;
}
void CWorldMng::DestroyCurrentWorld()
{
	if( m_pWorld != NULL )
	{
		m_pWorld->InvalidateDeviceObjects();
		m_pWorld->DeleteDeviceObjects();
		g_pMoveMark = NULL;
		g_DialogMsg.RemoveAll();
		LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( m_pWorld->m_dwWorldID );
		if( lpWorld )
		{
			ASSERT( lpWorld->IsValid() );
			RemoveKey( lpWorld->m_szFileName );
		}
		else
		{
			TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
			_splitpath( m_pWorld->m_szFileName, drive, dir, name, ext );
			RemoveKey( name );
		}
		SAFE_DELETE( m_pWorld );
	}
}
CWorld* CWorldMng::Open( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR lpszWorld )
{
//#		ifndef __WORLDSERVER
	if( m_pWorld != NULL )
	{
		DestroyCurrentWorld();
	}
//#		endif	// __WORLDSERVER

	CWorld* pWorld;

	if( FALSE == Lookup( lpszWorld, pWorld ) )
	{
		pWorld	= new CWorld;
		pWorld->InitDeviceObjects( pd3dDevice );
		pWorld->OpenWorld( MakePath( DIR_WORLD, lpszWorld ), TRUE );
		SetAt( lpszWorld, pWorld );
	}
	else
	{
		pWorld->InitDeviceObjects( pd3dDevice );
	}

	return( m_pWorld = pWorld );
}
#	endif	// __BEAST
#endif	// __WORLDSERVER

// common
BOOL CWorldMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
		return FALSE;

	LPWORLD lpWorld;
	
	m_nSize		= 0;
	int nBrace	= 1;
	scanner.SetMark();
	int i	= scanner.GetNumber();	// folder or id
	while( nBrace )
	{
		if( *scanner.token == '}' || scanner.tok == FINISHED )
		{
			nBrace--;
			if( nBrace > 0 )
			{
				scanner.SetMark();
				i	= scanner.GetNumber();	// folder or id
				if( i > m_nSize )	m_nSize	= i;
				continue;
			}
			if( nBrace == 0 )
				continue;
		}
//		i	= _ttoi( scanner.token );
		scanner.GetToken();		// { or filename
#ifdef __NPP_050308
		if( scanner.Token == "SetTitle" )
		{
			WORLD* pWorld = m_aWorld.GetAt( i );
			if( pWorld )
			{
				scanner.GetToken(); // (
				strcpy(	pWorld->m_szWorldName, prj.GetLangScript( scanner ) );				
			}
			else
			{
				Error( "CWorldMng::LoadScript -> Load WorldName" );
			}
		}
		else
		{
			scanner.GoMark();
			i	= scanner.GetNumber(); // id
			if( i > m_nSize )	m_nSize		= i;

#ifdef __BEAST
			lpWorld = new WORLD;
			lpWorld->m_dwId	= i;
			scanner.GetToken();		// fileName
			strcpy( lpWorld->m_szFileName, scanner.token );
			m_aWorld.SetAtGrow( i, lpWorld );
#else
			WORLD world;			//	lenajang : 워닝을 없애기위해서 아래로 뺀것이기도 하지만 
			//			   필요 없는 메모리는 낭비가 아닐까하는 생각에서...(주석은 왜쓰는거냐??)
			world.m_dwId = i;
			scanner.GetToken();		// fileName
			strcpy( world.m_szFileName, scanner.token );
			TRACE( "Load World %s %d\n", world.m_szFileName, i );
			m_aWorld.SetAtGrow( i, &world );
#endif
		}
#else // __NPP_050308
		if( *scanner.token == '{' )
		{
			nBrace++;
			scanner.SetMark();
			i	= scanner.GetNumber(); // id
			if( i > m_nSize )	m_nSize		= i;
			continue;
		}
		else
		{
			scanner.GoMark();
			i	= scanner.GetNumber(); // id
			if( i > m_nSize )	m_nSize		= i;
		}
		//lpWorld		= &m_aWorld[i];
#ifdef __BEAST
		lpWorld = new WORLD;
		lpWorld->m_dwId	= i;
		scanner.GetToken();		// fileName
		strcpy( lpWorld->m_szFileName, scanner.token );
		m_aWorld.SetAtGrow( i, lpWorld );
#else
		WORLD world;			//	lenajang : 워닝을 없애기위해서 아래로 뺀것이기도 하지만 
								//			   필요 없는 메모리는 낭비가 아닐까하는 생각에서...(주석은 왜쓰는거냐??)
		world.m_dwId = i;
		scanner.GetToken();		// fileName
		strcpy( world.m_szFileName, scanner.token );
		m_aWorld.SetAtGrow( i, &world );
#endif
#endif // __NPP_050308
		scanner.SetMark();
		i	= scanner.GetNumber();	// texture fileName
		if( i > m_nSize )	m_nSize		= i;
	}
#ifndef __BEAST
		m_aWorld.Optimize();
#endif
	m_nSize++;

//	monorith
	for( i = 0; i < m_nSize; i++ )
	{
		lpWorld	= GetWorldStruct( i );
		if( lpWorld && lpWorld->IsValid() )
		{
			LoadRevivalPos( lpWorld->m_dwId, lpWorld->m_szFileName );
		}
	}

	return TRUE;
}

void CWorldMng::LoadRevivalPos( DWORD dwWorldId, LPCTSTR lpszWorld )
{
#ifdef __WORLDSERVER
	TCHAR lpFileName[MAX_PATH];
	sprintf( lpFileName, "%s%s\\%s.rgn", DIR_WORLD, lpszWorld, lpszWorld );
	CScanner s;
	if( s.Load( lpFileName ) == TRUE )
	{
		DWORD dwType, dwIndex;
		D3DXVECTOR3 vPos;

		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == _T( "region" ) || s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
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
				dwType	= s.GetNumber();
				dwIndex	= s.GetNumber();
				vPos.x	= s.GetFloat();
				vPos.y	= s.GetFloat();
				vPos.z	= s.GetFloat();
				
				REGIONELEM rgnElem, *pRgnElem	= &rgnElem;
				memset( &rgnElem, 0, sizeof(rgnElem) );
				
#ifdef __N_TRIGGER_AREASET_050203
				rgnElem.m_uItemId		= 0xffffffff;
				rgnElem.m_uiItemCount	= 0xffffffff;
				rgnElem.m_uiMinLevel	= 0xffffffff;
				rgnElem.m_uiMaxLevel	= 0xffffffff;
				rgnElem.m_iQuest		= 0xffffffff;
				rgnElem.m_iQuestFlag	= 0xffffffff;
				rgnElem.m_iJob			= 0xffffffff;
				rgnElem.m_iGender		= 0xffffffff;
				rgnElem.m_bCheckParty	= FALSE;
				rgnElem.m_bCheckGuild	= FALSE;
				rgnElem.m_bChaoKey		= FALSE;
#endif // __N_TRIGGER_AREASET_050203

				pRgnElem->m_dwWorldId	= dwWorldId;
				pRgnElem->m_dwAttribute	= s.GetNumber();
				pRgnElem->m_dwIdMusic	= s.GetNumber();
				pRgnElem->m_bDirectMusic	= (bool)(s.GetNumber() == 1);
				s.GetToken();	_tcscpy( pRgnElem->m_szScript, s.Token );
				s.GetToken();	_tcscpy( pRgnElem->m_szSound, s.Token );
				pRgnElem->m_dwIdTeleWorld	= s.GetNumber();
				pRgnElem->m_vPos		= vPos;
				pRgnElem->m_vTeleWorld.x		= s.GetFloat();
				pRgnElem->m_vTeleWorld.y		= s.GetFloat();
				pRgnElem->m_vTeleWorld.z		= s.GetFloat();
				pRgnElem->m_rect.left	= (LONG)s.GetNumber();
				pRgnElem->m_rect.top		= (LONG)s.GetNumber();
				pRgnElem->m_rect.right	= (LONG)s.GetNumber();
				pRgnElem->m_rect.bottom	= (LONG)s.GetNumber();
				s.GetToken();	_tcscpy( pRgnElem->m_szKey, s.Token );
				pRgnElem->m_bTargetKey	= (bool)(s.GetNumber() == 1);

#ifdef __N_TRIGGER_AREASET_050203
				if( bNewFormat3 )
				{
					pRgnElem->m_uItemId		= (u_long)s.GetNumber();
					pRgnElem->m_uiItemCount	= (UINT)s.GetNumber();
					pRgnElem->m_uiMinLevel	= (UINT)s.GetNumber();
					pRgnElem->m_uiMaxLevel	= (UINT)s.GetNumber();
					pRgnElem->m_iQuest		= s.GetNumber();
					pRgnElem->m_iQuestFlag	= s.GetNumber();
					pRgnElem->m_iJob		= s.GetNumber();
					pRgnElem->m_iGender		= s.GetNumber();
					pRgnElem->m_bCheckParty	= (BOOL)s.GetNumber();
					pRgnElem->m_bCheckGuild	= (BOOL)s.GetNumber();
					pRgnElem->m_bChaoKey	= (BOOL)s.GetNumber();
				}
#endif // __N_TRIGGER_AREASET_050203

				if( bNewFormat == FALSE )
				{
					pRgnElem->m_cDescSize	= (LONG)s.GetNumber();
					if( pRgnElem->m_cDescSize )
					{
						pRgnElem->m_cDescSize = 1;
						CString string;
						s.GetToken(); // {
						int nLang;
						nLang = s.GetNumber();
						do
						{
							s.GetLastFull();
							if( nLang == prj.m_nLanguage )
								strcpy( pRgnElem->m_szDesc, s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
				}
				else
				{
					s.GetToken(); // title
					if( s.GetNumber() )
					{
						CString string;
						s.GetToken(); // {
						int nLang;
						nLang = s.GetNumber();
						do
						{
							s.GetLastFull();
							if( nLang == prj.m_nLanguage )
								strcpy( pRgnElem->m_szDesc, s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
					s.GetToken(); // desc
					if( s.GetNumber() )
					{
						CString string;
						s.GetToken(); // {
						int nLang;
						nLang = s.GetNumber();
						do
						{
							s.GetLastFull();
							if( nLang == prj.m_nLanguage )
								strcpy( pRgnElem->m_szDesc, s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
				}

				if( dwIndex == RI_REVIVAL || dwIndex == RI_PLACE )
				{
					pRgnElem->m_dwIndex		= dwIndex;

					if( FALSE == pRgnElem->m_bTargetKey )
					{
						m_aRevivalPos.AddTail( pRgnElem );	// memcpy
					}
					else
					{
						ASSERT( strlen( pRgnElem->m_szKey ) );
						m_aRevivalRgn.AddTail( pRgnElem );	// memcpy
					}
				}
			}

			else if( s.Token == _T( "respawn" ) )
			{
				// type, index, x, y, z, cb, time, left, top, right, bottom
				s.GetNumber();	s.GetNumber();	s.GetFloat();	s.GetFloat();	s.GetFloat();	s.GetNumber();	s.GetNumber();
				s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();
			}
			else if( s.Token == _T( "region3" ) )
			{
				
			}

			s.GetToken();
		}
	}
#endif	// __WORLDSERVER
}
void CWorldMng::LoadAllMoverDialog()
{
#ifdef __WORLDSERVER
	CWorld* pWorld;
	m_AddRemoveLock.Enter( theLineFile );	// lock1
	pWorld = GetFirstActive();
	while( pWorld )
	{
		pWorld->LoadAllMoverDialog();
		pWorld = pWorld->nextptr;
	}
	m_AddRemoveLock.Leave( theLineFile );	// unlock1
#else
	m_pWorld->LoadAllMoverDialog();
#endif
}


#ifdef __WORLDSERVER

#ifdef __NPP_050308
PRegionElem CWorldMng::GetNearRevivalPosChao( DWORD dwWorldId, D3DXVECTOR3 & vPos )
{
	RegionElem *pRgnElem, *ptr	= NULL;
	CPoint point;
	
	int nSize;
	
	nSize	= m_aRevivalPos.GetSize();
	long	d	= 2147483647L, tmp;	// 46340	// 21722
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId )
		{
			D3DXVECTOR3 vd	= vPos - pRgnElem->m_vPos;
			tmp		= (long)D3DXVec3LengthSq( &vd );
			if( pRgnElem->m_bChaoKey )
			{
				if( tmp < d )
				{
					ptr		= pRgnElem;
					d	= tmp;
				}
			}
		}
	}

	if( ptr == NULL )	// 같은 서버에 찾지를 못했을 경우 다른서버검색
	{
		for( i = 0; i < nSize; i++ )
		{
			pRgnElem	= m_aRevivalPos.GetAt( i );
			if( dwWorldId != pRgnElem->m_dwWorldId )
			{
				if( pRgnElem->m_bChaoKey )
				{
					ptr		= pRgnElem;
					break;			// 거리계산은 필요없음
				}
			}
		}
	}
	return ptr;
}
#endif // __NPP_050308

PRegionElem	CWorldMng::GetRevivalPos( DWORD dwWorldId, LPCTSTR sKey )
{
	int nSize	= m_aRevivalPos.GetSize();
	PRegionElem pRgnElem;
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && strcmp( sKey, pRgnElem->m_szKey ) == 0 )
		{
			return pRgnElem;
		}
	}
	return NULL;	// not found
}

PRegionElem CWorldMng::GetNearRevivalPos( DWORD dwWorldId, D3DXVECTOR3 & vPos )
{
	RegionElem *pRgnElem, *ptr	= NULL;
	CPoint point;

	int nSize	= m_aRevivalRgn.GetSize();
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalRgn.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && pRgnElem->m_dwIndex == RI_REVIVAL )
		{
			point.x	= vPos.x;	point.y	= vPos.z;
			if( pRgnElem->m_rect.PtInRect( point ) )
				return GetRevivalPos( dwWorldId, pRgnElem->m_szKey );
		}
	}

	nSize	= m_aRevivalPos.GetSize();
	long	d	= 2147483647L, tmp;	// 46340	// 21722
	for( i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId )
		{
			D3DXVECTOR3 vd	= vPos - pRgnElem->m_vPos;
			tmp		= (long)D3DXVec3LengthSq( &vd );
			if( tmp < d )
			{
				ptr		= pRgnElem;
				d	= tmp;
			}
		}
	}
	return ptr;
}
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
#ifdef __INSTANCE_DUNGEON
CWorld* CWorldMng::Add( DWORD dwWorldId, DWORD dwAddId )
{
	LPWORLD lpWorld	= GetWorldStruct( dwWorldId );
//	ASSERT( lpWorld );

	m_AddRemoveLock.Enter( theLineFile );

	CWorld* pWorld	= new CWorld;
	pWorld->m_dwWorldID		= dwAddId;
	pWorld->OpenWorld( MakePath( DIR_WORLD, lpWorld->m_szFileName ), TRUE );
	CRect rect( 0, 0, pWorld->m_nLandWidth, pWorld->m_nLandHeight );
	pWorld->ReadWorld( rect );
	pWorld->nextptr		= m_pFirstActive;
	m_pFirstActive	= pWorld;
	m_lCount++;

	m_AddRemoveLock.Leave( theLineFile );	// unlock1

	char s[0xf]	= { 0, };
	sprintf( s, "%d", m_lCount );
	g_MyTrace.Add( CMyTrace::Key( s ),
											FALSE,
											"%-16s/%d/ %-3d, %-3d, %-3d*%-3d",
											lpWorld->m_szFileName,
											m_lCount,
											0,
											0,
											pWorld->m_nLandWidth,
											pWorld->m_nLandHeight
										);
	return pWorld;
}
#endif	// __INSTANCE_DUNGEON
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
CWorldMng	g_WorldMng;
#endif	// __WORLDSERVER