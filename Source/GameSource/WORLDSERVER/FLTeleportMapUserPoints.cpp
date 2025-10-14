
#include "stdafx.h"
#include "FLTeleportMapUserPoints.h"


MEMPOOLER_IMPLEMENT( FLTeleportMapUserPoints, 512 );

FLTeleportMapUserPoints::FLTeleportMapUserPoints()
{
	m_dwUserID = 0;
	::memset( m_tUserPoints, 0, sizeof( m_tUserPoints ) );
}

FLTeleportMapUserPoints::~FLTeleportMapUserPoints()
{
	Destroy();
}

void	FLTeleportMapUserPoints::Create( OBJID dwUserID, const std::vector< T_TELEPORTMAP_POINT >* pUserPoints )
{
	Destroy();

	m_dwUserID = dwUserID;

	if( pUserPoints != NULL )
	{
		// 인덱스가 중복되면 먼저 들어간 데이터를 우선한다.
		std::vector< T_TELEPORTMAP_POINT >::const_iterator pos = pUserPoints->begin();
		std::vector< T_TELEPORTMAP_POINT >::const_iterator end = pUserPoints->end();

		for( ; pos != end; ++pos )
		{
			const T_TELEPORTMAP_POINT* pPoint = &(*pos);
			Add( pPoint );
		}
	}
}

void	FLTeleportMapUserPoints::Destroy()
{
	m_dwUserID = 0;
	::memset( m_tUserPoints, 0, sizeof( m_tUserPoints ) );
}

DWORD	FLTeleportMapUserPoints::GetUserID()	const
{
	return m_dwUserID;
}

size_t	FLTeleportMapUserPoints::GetCount()	const
{
	size_t nCount = 0;
	for( int i = 0; i < _countof( m_tUserPoints ); ++i )
	{
		if( m_tUserPoints[i].dwIndex != 0 )
		{
			++nCount;
		}
	}

	return nCount;
}

DWORD	FLTeleportMapUserPoints::GetEmptyID()	const
{
	DWORD dwEmptyID = 0;
	for( int i = 0; i < _countof( m_tUserPoints ); ++i )
	{
		if( m_tUserPoints[i].dwIndex == 0 )
		{
			dwEmptyID = i+1;
			break;
		}
	}

	return dwEmptyID;
}

void	FLTeleportMapUserPoints::GetAllUserPoint( std::vector< T_TELEPORTMAP_POINT >* pUserPoints )	const
{
	if( pUserPoints != NULL )
	{
		for( int i = 0; i < _countof( m_tUserPoints ); ++i )
		{
			if( m_tUserPoints[i].dwIndex != 0 )
			{
				pUserPoints->push_back( m_tUserPoints[i] );
			}
		}
	}
}

bool	FLTeleportMapUserPoints::IsNearPos( const T_WORLD_POSITION* pPos, float fNearDist )	const
{
	if( pPos )
	{
		// 유저 등록 지점 검색
		for( int i = 0; i < _countof( m_tUserPoints ); ++i )
		{
			if( m_tUserPoints[i].dwIndex != 0 )
			{
				if( m_tUserPoints[i].tPosition.dwWorldID == pPos->dwWorldID )
				{
					D3DXVECTOR3 vDist = pPos->tPos - m_tUserPoints[i].tPosition.tPos;
					float fDist = D3DXVec3Length( &vDist );
					if( fDist <= fNearDist )
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	return true;
}

bool	FLTeleportMapUserPoints::Add( const T_TELEPORTMAP_POINT* pUserPoint )
{
	if( pUserPoint == NULL )
	{
		return false;
	}

	if( 0 >= pUserPoint->dwIndex || pUserPoint->dwIndex > _countof( m_tUserPoints ) )
	{
		return false;
	}

	int nIndex = pUserPoint->dwIndex-1;
	if( m_tUserPoints[nIndex].dwIndex != 0 )
	{
		return false;
	}

	m_tUserPoints[nIndex] = *pUserPoint;
	return true;
}

bool	FLTeleportMapUserPoints::ChangeName( DWORD dwPointID, const TCHAR* pszName )
{
	if( dwPointID <= 0 || pszName == NULL )
	{
		return false;
	}

	for( int i = 0; i < _countof( m_tUserPoints ); ++i )
	{
		if( m_tUserPoints[i].dwIndex == dwPointID )
		{
			FLStrcpy( m_tUserPoints[i].szName, _countof( m_tUserPoints[i].szName ), pszName );
			return true;
		}
	}

	return false;
}

bool	FLTeleportMapUserPoints::Del( DWORD dwPointID )
{
	if( dwPointID != 0 )
	{
		for( int i = 0; i < _countof( m_tUserPoints ); ++i )
		{
			if( m_tUserPoints[i].dwIndex == dwPointID )
			{
				::memset( &m_tUserPoints[i], 0, sizeof( m_tUserPoints[i] ) );
				return true;
			}
		}
	}

	return false;
}

bool	FLTeleportMapUserPoints::DelAll()
{
	::memset( m_tUserPoints, 0, sizeof( m_tUserPoints ) );
	return true;
}

const T_TELEPORTMAP_POINT*	FLTeleportMapUserPoints::Find( DWORD dwPointID )	const
{
	const T_TELEPORTMAP_POINT* pUserPoint = NULL;

	if( dwPointID != 0 )
	{
		for( int i = 0; i < _countof( m_tUserPoints ); ++i )
		{
			if( m_tUserPoints[i].dwIndex == dwPointID )
			{
				pUserPoint = &m_tUserPoints[i];
				break;
			}
		}
	}

	return pUserPoint;
}
