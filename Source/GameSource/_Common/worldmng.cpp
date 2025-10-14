#include "stdafx.h"
#include "WorldMng.h"
#include "World.h"
#ifdef __CLIENT
#include "Sfx.h"
#include "DialogMsg.h"
#include "ResData.h"
#include "../Neuz/DPClient.h"
extern CDPClient g_DPlay;
#endif // CLIENT

#include "tools.h"
#include <crypto/FLMD5.h>

#ifdef __WORLDSERVER
#include "ServerDesc.h"
#include "../WorldServer/User.h"

CWorldMng	g_WorldMng;

#endif	// __WORLDSERVER

CWorldMng::CWorldMng()
{
#ifdef __WORLDSERVER
	m_pFirstActive	= NULL;
	m_lCount	= 0;
#else	// __WORLDSERVER

	m_pWorld	= NULL;
#endif	// __WORLDSERVER
	m_nSize = 0;
}


CWorldMng::~CWorldMng()
{
	Free();
}
#ifndef __WORLDSERVER
void CWorldMng::Free()
{
	for( std::map<std::string, CWorld*>::iterator i = begin(); i != end(); ++i )
	{
		CWorld* pWorld	= i->second;
		SAFE_DELETE( pWorld );
	}
	clear();
	m_aWorld.RemoveAll();
}
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
void CWorldMng::Free()
{
	CWorld* pWorld, *pRemove;

	pWorld	= GetFirstActive();
	//while( ( pRemove = pWorld ) )
	for( pRemove = pWorld; pRemove != NULL; pRemove = pWorld )
	{
		pWorld	= pWorld->nextptr;
		SAFE_DELETE( pRemove );
	}
	m_pFirstActive	= NULL;
	m_aWorld.RemoveAll();

}

u_long	CWorldMng::Respawn()
{
	CWorld* pWorld;
	u_long	uRespawned	= 0;
	
	pWorld	= GetFirstActive();
	while( pWorld )
	{
		uRespawned	+= pWorld->Respawn();
		pWorld	= pWorld->nextptr;
	}

	return uRespawned;
}

void CWorldMng::ReadObject()
{
	CWorld* pWorld;
	pWorld	= GetFirstActive();
	while( pWorld )
	{
	#ifdef __LAYER_1021
		pWorld->LoadObject( nDefaultLayer );
	#else	// __LAYER_1021
		pWorld->LoadObject();
	#endif	// __LAYER_1021
	#ifndef __S1108_BACK_END_SYSTEM
		pWorld->LoadRegion();	// x
	#endif // __S1108_BACK_END_SYSTEM
		pWorld->LoadPatrol();
		pWorld	= pWorld->nextptr;
	}
}

void CWorldMng::Add( CJurisdiction* pJurisdiction )
{
	LPWORLD lpWorld	= GetWorldStruct( pJurisdiction->m_dwWorldID );
	FLASSERT( lpWorld );

	CWorld* pWorld = GetWorld( pJurisdiction->m_dwWorldID );
	if( pWorld )
	{
		pWorld->ReadWorld( pJurisdiction->m_rect );
	}
	else
	{
		pWorld	= new CWorld;

		pWorld->m_dwWorldID	= pJurisdiction->m_dwWorldID;
		pWorld->OpenWorld( MakePath( DIR_WORLD, lpWorld->m_szFileName ), TRUE );
		pWorld->ReadWorld( pJurisdiction->m_rect );

		pWorld->nextptr	= m_pFirstActive;
		m_pFirstActive	= pWorld;
		m_lCount++;
	}
	FLStrcpy( pWorld->m_szWorldName, _countof( pWorld->m_szWorldName ), lpWorld->m_szWorldName );
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

#ifdef __LAYER_1015
BOOL CWorldMng::AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId, int nLayer )
#else	// __LAYER_1015
BOOL CWorldMng::AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId )
#endif	// __LAYER_1015
{
	BOOL bResult = FALSE;
	CWorld* pWorld = GetWorld( dwWorldID );
	if( pWorld )
		bResult = pWorld->ADDOBJ( pObj, bAddItToGlobalId, nLayer );
	return bResult;
}

BOOL CWorldMng::PreremoveObj( OBJID objid )
{
	CWorld* pWorld	= GetFirstActive();
	while( pWorld )
	{
		if( TRUE == pWorld->PreremoveObj( objid ) )
			return TRUE;
		pWorld	= pWorld->nextptr;
	}
	return FALSE;
}

CObj* CWorldMng::PregetObj( OBJID objid )
{
	CObj* pObj;
	CWorld* pWorld	= GetFirstActive();
	while( pWorld )
	{
		pObj	= pWorld->PregetObj( objid );
		if( NULL != pObj )
			return pObj;
		pWorld	= pWorld->nextptr;
	}
	return NULL;
}

#else	// __WORLDSERVER

CWorld* CWorldMng::Open( LPDIRECT3DDEVICE9 pd3dDevice, OBJID idWorld )
{
	CWorld* pWorld	= NULL;
	LPWORLD	lpWorld	= GetWorldStruct( idWorld );
	if( lpWorld ) 
	{
		pWorld	= Open( pd3dDevice, lpWorld->m_szFileName );
		pWorld->m_dwWorldID	= idWorld;
		pWorld->InProcessing( );			//added by gmpbigsun
		FLStrcpy( pWorld->m_szWorldName, _countof( pWorld->m_szWorldName ), lpWorld->m_szWorldName );
	}
	return pWorld;
}
void CWorldMng::DestroyCurrentWorld()
{
	if( m_pWorld != NULL )
	{
		m_pWorld->OutProcessing( );			//added by gmpbigsun

		CWndBase* pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
		if(pWndBase != NULL)
			((CWndInventory*)pWndBase)->BaseMouseCursor();

		m_pWorld->InvalidateDeviceObjects();
		m_pWorld->DeleteDeviceObjects();
		g_pMoveMark = NULL;
		g_DialogMsg.RemoveAll();
		LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( m_pWorld->m_dwWorldID );
		if( lpWorld )
		{
			FLASSERT( lpWorld->IsValid() );
			erase( lpWorld->m_szFileName );
		}
		else
		{
			TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
			FLSplitPath( m_pWorld->m_szFileName, drive, _countof( drive ), dir, _countof( dir ), name, _countof( name ), ext, _countof( ext ) );
			erase( name );
		}
		SAFE_DELETE( m_pWorld );
	}
}


BOOL CWorldMng::CheckWorldData( CWorld* pWorld )
{
	//gmpbigsun(100610) : 월드 데이터 검증, 치명적인 오류에 대해서 log를 남기고 클라는 종료된다.

	if( !pWorld )
		return FALSE;
	
	// 월드 사이즈를 검사한다. 무조건 1, 1 이상이여야 한다. 문제가 있다면 클라는 crash되므로 log찍고 종료시킨다.
	if( 1 > pWorld->m_nLandHeight || 1 > pWorld->m_nLandWidth )
	{
		AfxMessageBox( "World data is wrong, terminate the client" );
		FLERROR_LOG( PROGRAM_NAME, _T( "!!World size is wrong w%d, h%d, plz check the file(****.wld)" ), pWorld->m_nLandWidth, pWorld->m_nLandHeight );
		::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
		return FALSE;
	}
	
	return TRUE;
}

CWorld* CWorldMng::Open( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR lpszWorld )
{
	FLINFO_LOG( PROGRAM_NAME, _T( "BEGIN" ) );
	if( m_pWorld != NULL )
	{
		DestroyCurrentWorld();
	}
	CWorld* pWorld;
	std::map<std::string, CWorld*>::iterator i	= find( lpszWorld );
	if( i == end() )
	{
		pWorld	= new CWorld;
		pWorld->InitDeviceObjects( pd3dDevice );
		pWorld->OpenWorld( MakePath( DIR_WORLD, lpszWorld ), TRUE );
		bool bResult	= insert( std::map<std::string, CWorld*>::value_type( lpszWorld, pWorld ) ).second;
		if( bResult == false )
		{
			SAFE_DELETE( pWorld );
		}
		FLASSERT( bResult );
	}
	else
	{
		pWorld	= i->second;
		pWorld->InitDeviceObjects( pd3dDevice );
	}

#ifdef __CLIENT
	CheckWorldData( pWorld );	//gmpbigsun(20100610) :  데이터검증
#endif 

	FLINFO_LOG( PROGRAM_NAME, _T( "END" ) );

	return( m_pWorld = pWorld );
}
#endif	// __WORLDSERVER

// common
BOOL CWorldMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "loading error : %s" ), lpszFileName );
		return FALSE;
	}

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

		if( scanner.Token == "SetTitle" )
		{
			WORLD* pWorld = m_aWorld.GetAt( i );
			if( pWorld != NULL )
			{
				scanner.GetToken(); // (
				FLStrcpy( pWorld->m_szWorldName, _countof( pWorld->m_szWorldName ), prj.GetLangScript( scanner ) );				
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ WorldName Not found in <world.inc> (ex : WI_WORLD_TEST   \"WdTest\")]" ) );
			}
		}
		else
		{
			scanner.GoMark();
			i	= scanner.GetNumber(); // id
			if( i > m_nSize )	m_nSize		= i;


			WORLD world = { 0 };
			// mem_set( &world, 0, sizeof(world) );

			world.m_dwId = i;
			scanner.GetToken();		// fileName
			FLStrcpy( world.m_szFileName, _countof( world.m_szFileName ), scanner.token );
			FLTRACE_LOG( PROGRAM_NAME, _T( "Load World %s %d" ), world.m_szFileName, i );
			m_aWorld.SetAt( i, &world );
		}

		scanner.SetMark();
		i	= scanner.GetNumber();	// texture fileName
		if( i > m_nSize )	m_nSize		= i;
	}
	m_aWorld.Optimize();
	m_nSize++;

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
	TCHAR lpFileName[MAX_PATH] = { 0, };
	FLSPrintf( lpFileName, _countof( lpFileName ), "%s%s\\%s.rgn", DIR_WORLD, lpszWorld, lpszWorld );
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

				pRgnElem->m_dwWorldId	= dwWorldId;
				pRgnElem->m_dwAttribute	= s.GetNumber();
				pRgnElem->m_dwIdMusic	= s.GetNumber();
				pRgnElem->m_bDirectMusic	= (bool)(s.GetNumber() == 1);
				s.GetToken();	FLStrcpy( pRgnElem->m_szScript, _countof( pRgnElem->m_szScript ), s.Token );
				s.GetToken();	FLStrcpy( pRgnElem->m_szSound, _countof( pRgnElem->m_szSound ), s.Token );
				pRgnElem->m_dwIdTeleWorld	= s.GetNumber();
				pRgnElem->m_vPos		= vPos;
				pRgnElem->m_vTeleWorld.x		= s.GetFloat();
				pRgnElem->m_vTeleWorld.y		= s.GetFloat();
				pRgnElem->m_vTeleWorld.z		= s.GetFloat();
				pRgnElem->m_rect.left	= (LONG)s.GetNumber();
				pRgnElem->m_rect.top		= (LONG)s.GetNumber();
				pRgnElem->m_rect.right	= (LONG)s.GetNumber();
				pRgnElem->m_rect.bottom	= (LONG)s.GetNumber();
				s.GetToken();	FLStrcpy( pRgnElem->m_szKey, _countof( pRgnElem->m_szKey ), s.Token );
				pRgnElem->m_bTargetKey	= (bool)(s.GetNumber() == 1);

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

				if( bNewFormat == FALSE )
				{
					pRgnElem->m_cDescSize	= (char)( (LONG)s.GetNumber() );
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
							if( nLang == g_xFlyffConfig->GetMainLanguage() )
								FLStrcpy( pRgnElem->m_szDesc, _countof( pRgnElem->m_szDesc ), s.m_mszToken );
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
							if( nLang == g_xFlyffConfig->GetMainLanguage() )
								FLStrcpy( pRgnElem->m_szDesc, _countof( pRgnElem->m_szDesc ), s.m_mszToken );
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
							if( nLang == g_xFlyffConfig->GetMainLanguage() )
								FLStrcpy( pRgnElem->m_szDesc, _countof( pRgnElem->m_szDesc ), s.m_mszToken );
							nLang = s.GetNumber();
						} while( *s.token != '}' );
					}
				}

				if( dwIndex == REGION_INDEX( 13, RI_REVIVAL ) || dwIndex == REGION_INDEX( 15, RI_PLACE ) )
				{
					pRgnElem->m_dwIndex		= dwIndex;

					if( FALSE == pRgnElem->m_bTargetKey )
					{
						m_aRevivalPos.AddTail( pRgnElem );	// _memcpy
					}
					else
					{
						FLASSERT( strlen( pRgnElem->m_szKey ) );
						m_aRevivalRgn.AddTail( pRgnElem );	// _memcpy
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
	pWorld = GetFirstActive();
	while( pWorld )
	{
		pWorld->LoadAllMoverDialog();
		pWorld = pWorld->nextptr;
	}
#endif //__WORLDSERVER
}


#ifdef __WORLDSERVER

PRegionElem	CWorldMng::GetRevivalPosChao( DWORD dwWorldId, LPCTSTR sKey )
{
	int nSize	= m_aRevivalPos.GetSize();
	PRegionElem pRgnElem;
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && strcmp( sKey, pRgnElem->m_szKey ) == 0 && pRgnElem->m_bChaoKey )
		{
			return pRgnElem;
		}
	}
	return NULL;	// not found
}
PRegionElem CWorldMng::GetNearRevivalPosChao( DWORD dwWorldId, const D3DXVECTOR3 & vPos )
{
	RegionElem *pRgnElem, *ptr	= NULL;
	CPoint point;
	
	int nSize;
	
	nSize	= m_aRevivalPos.GetSize();
	long	d	= 2147483647L, tmp;	// 46340	// 21722
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && pRgnElem->m_bChaoKey )
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

	if( ptr == NULL )	// 같은 서버에 찾지를 못했을 경우 다른서버검색
	{
		for( int i = 0; i < nSize; i++ )
		{
			pRgnElem	= m_aRevivalPos.GetAt( i );
			if( dwWorldId != pRgnElem->m_dwWorldId && pRgnElem->m_bChaoKey )
			{
				ptr		= pRgnElem;
				break;			// 거리계산은 필요없음
			}
		}
	}
	return ptr;
}

PRegionElem	CWorldMng::GetRevivalPos( DWORD dwWorldId, LPCTSTR sKey )
{
	int nSize	= m_aRevivalPos.GetSize();
	PRegionElem pRgnElem;
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && strcmp( sKey, pRgnElem->m_szKey ) == 0 && pRgnElem->m_bChaoKey == FALSE )
			return pRgnElem;
	}
	return NULL;	// not found
}

PRegionElem CWorldMng::GetNearRevivalPos( DWORD dwWorldId, const D3DXVECTOR3 & vPos )
{
	RegionElem *pRgnElem, *ptr	= NULL;
	CPoint point;

	int nSize	= m_aRevivalRgn.GetSize();
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalRgn.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && pRgnElem->m_dwIndex == REGION_INDEX( 13, RI_REVIVAL ) && pRgnElem->m_bChaoKey == FALSE )
		{
			point.x	= (LONG)( vPos.x );
			point.y	= (LONG)( vPos.z );
			if( pRgnElem->m_rect.PtInRect( point ) )
				return GetRevivalPos( dwWorldId, pRgnElem->m_szKey );
		}
	}

	nSize	= m_aRevivalPos.GetSize();
	long	d	= 2147483647L, tmp;	// 46340	// 21722
	for( int i = 0; i < nSize; i++ )
	{
		pRgnElem	= m_aRevivalPos.GetAt( i );
		if( dwWorldId == pRgnElem->m_dwWorldId && pRgnElem->m_bChaoKey == FALSE )
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
void CWorldMng::Process()
{
	CWorld* pWorld = GetFirstActive();
	while( pWorld )
	{
		pWorld->Process();
		pWorld = pWorld->nextptr;
	}
}

BOOL CWorldMng::HasNobody_Replace( DWORD dwWorldId, int nLayer )
{
	CWorld* pWorld = GetFirstActive();
	while( pWorld )
	{
		for( int i=0; i<pWorld->m_cbReplaceObj; ++i )
		{
			if( pWorld->m_aReplaceObj[i].dwWorldID == dwWorldId && pWorld->m_aReplaceObj[i].nLayer == nLayer )
				return FALSE;
		}
		pWorld = pWorld->nextptr;
	}

	return TRUE;
}

// 모든 월드의 오브젝트 카운트를 얻는다.
DWORD CWorldMng::GetObjCount()
{
	DWORD dwCount = 0;

	CWorld* pWorld = GetFirstActive();

//	FLDEBUG_LOG( PROGRAM_NAME, _T("[ ------------- start Obj Count ------------- ]") );
	while( pWorld )
	{
//		dwCount += pWorld->GetObjCount();
		dwCount	+=	pWorld->GetRunnableObjectCount();

//		FLDEBUG_LOG( PROGRAM_NAME, _T("[ WordID:(%u) - Count(%u) ]"), pWorld->GetID(), pWorld->GetRunnableObjectCount() );

		pWorld = pWorld->nextptr;
	}
//	FLDEBUG_LOG( PROGRAM_NAME, _T("[ -------------- end Obj Count(%u) -------------- ]"), dwCount );

	return dwCount;
}

#endif	// __WORLDSERVER

BOOL CWorldMng::SetMapCheck( const char* szFileName )
{
	CResFile file;
	if( file.Open( szFileName, "rb" ) )
	{
		size_t nSize = file.GetLength() / sizeof( DWORD );
		DWORD dwTotal = 0;
		DWORD* dwData = new DWORD[nSize];
		file.Read( dwData, sizeof( DWORD ), nSize );
		file.Close();
		for( size_t i=0; i<nSize; i++ )
			dwTotal += dwData[i];
		delete [] dwData;

		char szFileNameOut[33] = {0,};
		md5( szFileNameOut, _countof( szFileNameOut ), szFileName );

		char szIn[16] = {0,}, szOut[33] = {0,};
		FLSPrintf( szIn, _countof( szIn ), "%u", dwTotal );
		md5( szOut, _countof( szOut ), szIn );

#ifdef __WORLDSERVER
		m_mapMapKey.insert( std::map<string, string>::value_type( szFileNameOut, szOut ) );
#endif // __WORLDSERVER
#ifdef __CLIENT
		g_DPlay.SendMapKey( szFileNameOut, szOut );
#endif // __CLIENT

		return TRUE;
	}

	return FALSE;
}

void CWorldMng::AddMapCheckInfo( const char* szFileName )
{
#ifndef __INTERNALSERVER
#ifdef __WORLDSERVER
	UNREFERENCED_PARAMETER( szFileName );
	SetMapCheck( szFileName );
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_vecstrFileName.push_back( szFileName );
#endif // __CLIENT

#else
	UNREFERENCED_PARAMETER( szFileName );
#endif // __INTERNALSERVER
}

#ifdef __CLIENT
void CWorldMng::CheckMapKey()
{
	if( m_vecstrFileName.size() > 0 )
	{
		SetMapCheck( m_vecstrFileName.back().c_str() );
		m_vecstrFileName.pop_back();
	}
}
#endif // __CLIENT
#ifdef __WORLDSERVER
void CWorldMng::CheckMapKey( FLWSUser* pUser, const char* szFileName, const char* szMapKey )
{	
	std::map<string, string>::iterator it = m_mapMapKey.find( szFileName );
	if( it != m_mapMapKey.end() )
	{
		if( !_strcmpi( szMapKey, it->second.c_str() ) )	// 맵 인증 성공!
			return;
	}

	
	// 맵 인증 실패 접속을 끊어버려야 한다.
	int x = int( pUser->GetPos().x ) / ( MAP_SIZE * pUser->GetWorld()->m_iMPU );
	int z = int( pUser->GetPos().z ) / ( MAP_SIZE * pUser->GetWorld()->m_iMPU );
	char szLandFile[MAX_PATH] = {0,};
	if( pUser->GetWorld() )
	{
		FLStrncpy( szLandFile, _countof( szLandFile ), pUser->GetWorld()->m_szFileName, strlen( pUser->GetWorld()->m_szFileName ) - 4 );
	}

	FLWARNING_LOG( PROGRAM_NAME, _T( "Invalid Map Key - [User : %s, %07d], [Land : %s%02d-%02d]" ), pUser->GetName(), pUser->m_idPlayer, szLandFile, x, z );

	g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
}
#endif // __WORLDSERVER
