#include "StdAfx.h"
#include "FLServerDescription.h"


FLServerDescription::FLServerDescription()
: m_dwServerIndex( NULL_ID )
{
	Clear();
}

FLServerDescription::~FLServerDescription()
{
	Clear();
}

FLServerDescription*	FLServerDescription::GetInstance()
{
	static FLServerDescription	kServerDescription;

	return & kServerDescription;
}

void	FLServerDescription::Clear()
{
	m_vecServerDesc.clear();
}

const T_SERVER_DESC*	FLServerDescription::FindByID( DWORD dwParentID, DWORD dwMyID ) const
{
	for( ServerDescCItr kCItr = m_vecServerDesc.begin(); kCItr != m_vecServerDesc.end(); ++kCItr )
	{
		if( kCItr->dwParentID == dwParentID && kCItr->dwMyID == dwMyID )
		{
			return &( *kCItr );
		}
	}

	return NULL;
}

const T_SERVER_DESC*	FLServerDescription::FindByNth( DWORD dwParentID, DWORD dwNth ) const
{
	DWORD dwCount = 0;
	for( ServerDescCItr kCItr = m_vecServerDesc.begin(); kCItr != m_vecServerDesc.end(); ++kCItr )
	{
		if( kCItr->dwParentID == dwParentID )
		{
			if( dwCount == dwNth )
			{
				return &( *kCItr );
			}
			else
			{
				++dwCount;
			}
		}
	}

	return NULL;
}

void	FLServerDescription::Add( const T_SERVER_DESC* pServerDesc )
{
	if( pServerDesc != NULL )
	{
		m_vecServerDesc.push_back( *pServerDesc );
	}
}

long	FLServerDescription::IncreaseConnectionCount( DWORD dwParentID, DWORD dwMyID )
{
	long lConnectionCount		= 0;

	T_SERVER_DESC* pServerDesc	= Find( dwParentID, dwMyID );
	if( pServerDesc != NULL )
	{
		lConnectionCount = InterlockedIncrement( &( pServerDesc->lConnectionCount ) );
	}

	return lConnectionCount;
}

long	FLServerDescription::DecreaseConnectionCount( DWORD dwParentID, DWORD dwMyID )
{
	long lConnectionCount		= 0;
	
	T_SERVER_DESC* pServerDesc	= Find( dwParentID, dwMyID );
	if( pServerDesc != NULL )
	{
		lConnectionCount = InterlockedDecrement( &( pServerDesc->lConnectionCount ) );
	}

	return lConnectionCount;
}

void	FLServerDescription::SetConnectionCount( DWORD dwParentID, DWORD dwMyID, long lConnectionCount )
{
	T_SERVER_DESC* pServerDesc	= Find( dwParentID, dwMyID );
	if( pServerDesc != NULL )
	{
		InterlockedExchange( &( pServerDesc->lConnectionCount ), lConnectionCount );
	}
}

void	FLServerDescription::SetConnectionMode( DWORD dwParentID, DWORD dwMyID, long lCanConnection )
{
	T_SERVER_DESC* pServerDesc	= Find( dwParentID, dwMyID );
	if( pServerDesc != NULL )
	{
		InterlockedExchange( &( pServerDesc->lCanConnection ), lCanConnection );
	}
}

void	FLServerDescription::GetAllServerDescription( ServerDescVec* pServerDesc ) const
{
	if( pServerDesc != NULL )
	{
		pServerDesc->assign( m_vecServerDesc.begin(), m_vecServerDesc.end() );
	}
}

void	FLServerDescription::GetCanConnectionServerDescription( ServerDescVec* pServerDesc ) const
{
	if( pServerDesc != NULL )
	{
		for( ServerDescCItr kCItr = m_vecServerDesc.begin(); kCItr != m_vecServerDesc.end(); ++kCItr )
		{
			if( kCItr->dwParentID == NULL_ID && kCItr->lCanConnection != 0 )
			{
				pServerDesc->push_back( *kCItr );
			}
		}
	}
}

void	FLServerDescription::SetServerIndex( DWORD dwServerIndex )
{
	m_dwServerIndex = dwServerIndex;
}

DWORD	FLServerDescription::GetServerIndex()
{
	return m_dwServerIndex;
}

T_SERVER_DESC*	FLServerDescription::Find( DWORD dwParentID, DWORD dwMyID )
{
	for( ServerDescItr kItr = m_vecServerDesc.begin(); kItr != m_vecServerDesc.end(); ++kItr )
	{
		if( kItr->dwParentID == dwParentID && kItr->dwMyID == dwMyID )
		{
			return &( *kItr );
		}
	}

	return NULL;
}
