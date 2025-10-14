#include "stdafx.h"
#include "linkmap.h"
#include "layeredlinkmap.h"

#ifdef __LAYER_1015

CLayeredLinkMap::CLayeredLinkMap()
:
m_nLandWidth( 0 )
, m_nLandHeight( 0 )
, m_nView( 0 )
, m_iMPU( 0 )
{
}

CLayeredLinkMap::~CLayeredLinkMap()
{
	Release();
}

void CLayeredLinkMap::Init( int nLandWidth, int nLandHeight, int nView, BOOL bBaseLayer, int nMPU )
{
	m_nLandWidth	= nLandWidth;
	m_nLandHeight	= nLandHeight;
	m_nView		= nView;
	m_iMPU = nMPU;

	if( bBaseLayer )
		CreateLinkMap( nDefaultLayer );
	
}

void CLayeredLinkMap::Release()
{
	for( MLM::iterator i = m_mapLinkMap.begin(); i != m_mapLinkMap.end(); ++i )
	{
		CLinkMap* pLinkMap	= i->second;
		//pLinkMap->Release();
		SAFE_DELETE( pLinkMap );
	}
	m_mapLinkMap.clear();
}
//
//int CLayeredLinkMap::GetLinkWidth( DWORD dwLinkType, int nLinkLevel, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	return pLinkMap->GetLinkWidth( dwLinkType, nLinkLevel );
//}
//
//CObj** CLayeredLinkMap::GetObj( DWORD dwLinkType, DWORD dwLinkLevel, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	return pLinkMap->GetObj( dwLinkType, dwLinkLevel );
//}
//
//DWORD CLayeredLinkMap::CalcLinkLevel( CObj* pObj, float fObjWidth, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	return pLinkMap->CalcLinkLevel( pObj, fObjWidth );
//}
//
//int	CLayeredLinkMap::GetMaxLinkLevel( DWORD dwLinkType, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	return pLinkMap->GetMaxLinkLevel( dwLinkType );
//}
//
//void CLayeredLinkMap::SetMaxLinkLevel( DWORD dwLinkType, int nLevel, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	pLinkMap->SetMaxLinkLevel( dwLinkType, nLevel );
//}
//
BOOL CLayeredLinkMap::InsertObjLink( CObj* pObj )
{
	//CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	//return pLinkMap->InsertObjLink( pObj );

	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	if( pLinkMap == NULL )
	{
		DEBUG_BREAK;
		return FALSE;
	}

	const bool ret			= pLinkMap->m_kMapGrid.Enter( pObj );	
	DEBUG_ASSERT( ret == true );
	return ( ret == true ) ? TRUE : FALSE;


	//if( pObj->IsCCtrl() == true )
	//{
	//	CCtrl * pCtrl			= static_cast< CCtrl * >( pObj );
	//	pLinkMap->m_kMapGrid.Enter( pCtrl, pCtrl->GetPos() );			//@@@@@@@@@@@@@@@@@@@
	//	return TRUE;
	//}

	//if( pObj->IsCollitionObj() == true )
	//{
	//	pLinkMap->m_kMapGrid.EnterCollision( pObj, pObj->GetPos() );
	//	return TRUE;
	//}

	//DEBUG_BREAK;
}

BOOL CLayeredLinkMap::RemoveObjLink( CObj* pObj )
{
	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	if( pLinkMap == NULL )
	{
		DEBUG_BREAK;
		return FALSE;
	}

	const bool ret			= pLinkMap->m_kMapGrid.Leave( pObj );
	DEBUG_ASSERT( ret == true );
	return ( ret == true ) ? TRUE : FALSE;
}
//
BOOL CLayeredLinkMap::RemoveObjLink2( CObj* pObj )
{
	//CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	//return pLinkMap->RemoveObjLink2( pObj );
	return RemoveObjLink( pObj );
}
//
//BOOL CLayeredLinkMap::RemoveObjLink( CObj* pObj )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
//	return pLinkMap->RemoveObjLink( pObj );
//}
//
//CObj* CLayeredLinkMap::GetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	return pLinkMap->GetObjInLinkMap( vPos, dwLinkType, nLinkLevel );
//}
//
//BOOL CLayeredLinkMap::SetObjInLinkMap( const D3DXVECTOR3 & vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj, int nLayer )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
//	return pLinkMap->SetObjInLinkMap( vPos, dwLinkType, nLinkLevel, pObj );
//}
//
//void CLayeredLinkMap::AddItToView( CCtrl* pCtrl )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( pCtrl->GetLayer() );
//	pLinkMap->AddItToView( pCtrl );
//}
//
//void CLayeredLinkMap::ModifyView( CCtrl* pCtrl )
//{
//	CLinkMap* pLinkMap	= GetLinkMap( pCtrl->GetLayer() );
//	pLinkMap->ModifyView( pCtrl );
//}

BOOL CLayeredLinkMap::CreateLinkMap( int nLayer )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	if( pLinkMap )
		return FALSE;

	if( m_nLandWidth > CWorld::s_nLandWidthMax || m_nLandHeight > CWorld::s_nLandHeightMax )	//@@@@@@@@@@@@@@
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ LandWidthMax(%d) m_nLandWidth(%d), LandHeightMax(%d) m_nLandHegith(%d) ]" ), CWorld::s_nLandWidthMax, m_nLandWidth, CWorld::s_nLandHeightMax, m_nLandHeight );
		return FALSE;
	}

	pLinkMap	= new CLinkMap;
	pLinkMap->Init( m_nLandWidth, m_nLandHeight, m_nView, m_iMPU );
	bool bResult = m_mapLinkMap.insert( MLM::value_type( nLayer, pLinkMap ) ).second;
	if( bResult == false )
	{
		SAFE_DELETE( pLinkMap );
		return FALSE;
	}
	return TRUE;
}

CLinkMap* CLayeredLinkMap::GetLinkMap( int nLayer )
{
	MLM::iterator i	= m_mapLinkMap.find( nLayer );
	if( i != m_mapLinkMap.end() )
		return i->second;
	return NULL;
}

void CLayeredLinkMap::DeleteLinkMap( int nLayer )
{
	MLM::iterator i	= m_mapLinkMap.find( nLayer );
	if( i != m_mapLinkMap.end() )
	{
		CLinkMap* pLinkMap	= i->second;
		//pLinkMap->Release();
		SAFE_DELETE( pLinkMap );
		m_mapLinkMap.erase( i );
	}
}

void CLayeredLinkMap::Process( CWorld* pWorld )
{
	for( MLM::iterator i = m_mapLinkMap.begin(); i != m_mapLinkMap.end(); )
	{
		int nLayer	= i->first;
		CLinkMap* pLinkMap	= i->second;
		++i;
		if( nLayer <= 0 )
			continue;
		if( pLinkMap->IsInvalid() )
		{
			if( pWorld->HasNobody( nLayer ) )
			{
				pWorld->ReleaseLayer( nLayer );
				FLTRACE_LOG( PROGRAM_NAME, _T( "CWorld.ReleaseLayer( %d, %d )" ), pWorld->GetID(), nLayer );
			}
			else
				pWorld->DriveOut( nLayer );
		}
	}
}

void CLayeredLinkMap::Invalidate( int nLayer, BOOL bInvalid )
{
	CLinkMap* pLinkMap	= GetLinkMap( nLayer );
	if( pLinkMap )
	{
		pLinkMap->Invalidate( bInvalid );
	}
}

bool CLayeredLinkMap::OnMove( CObj * pObj, const D3DXVECTOR3 & vOldPos, const D3DXVECTOR3 & vNewPos )
{
	if( pObj->GetType() != OT_MOVER )
		return false;

	CLinkMap* pLinkMap	= GetLinkMap( pObj->GetLayer() );
	if( pLinkMap == NULL )
		return false;

	const bool ret			= pLinkMap->m_kMapGrid.TryMove( pObj, vOldPos, vNewPos );	
	DEBUG_ASSERT( ret == true );

	//if( pObj->IsCCtrl() == true )
	//	if( pLinkMap->m_kMapGrid.TryMove( static_cast< CMover * >( pObj ), vOldPos, vNewPos ) == false )
	//		return false;

	//if( pObj->IsCollitionObj() == true )
	//	if( pLinkMap->m_kMapGrid.TryMoveCollision( pObj, vOldPos, vNewPos ) == false )
	//		return false;

	//return true;
	return ret;
}

#endif	// __LAYER_1015