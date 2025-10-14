#include "stdafx.h"
#include "defineObj.h"
#include "WorldMng.h"
#include "World.h"
#include "Sfx.h"


CWorldMng::CWorldMng()
{
	SetSize( 2, 4, 4 );
	m_pWorld	= NULL;

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

	SAFE_DELETE( CActionMover::m_pPool );	
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

		int nWSize = m_aWorld.GetSize();

		if(nWSize > i)
		{
			LPWORLD pWorld = (LPWORLD)m_aWorld.GetAt(i);
			if(pWorld == NULL)
				m_aWorld.SetAtGrow( i, lpWorld );
			else
				SAFE_DELETE(lpWorld);
		}
		else
			m_aWorld.SetAtGrow( i, lpWorld );
#else
		WORLD world;			//	lenajang : 워닝을 없애기위해서 아래로 뺀것이기도 하지만 
								//			   필요 없는 메모리는 낭비가 아닐까하는 생각에서...(주석은 왜쓰는거냐??)
		world.m_dwId = i;
		scanner.GetToken();		// fileName
		strcpy( world.m_szFileName, scanner.token );
		m_aWorld.SetAtGrow( i, &world );
#endif

		scanner.SetMark();
		i	= scanner.GetNumber();	// texture fileName
		if( i > m_nSize )	m_nSize		= i;
	}
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

	return;
}
void CWorldMng::LoadAllMoverDialog()
{
	m_pWorld->LoadAllMoverDialog();
}
