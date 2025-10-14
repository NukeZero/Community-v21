#include "stdafx.h"
#include "svrmng.h"

CSvrInfo::CSvrInfo()
{
	*m_lpAddr	= '\0';
	m_fConnect	= 0;	// false
	m_dwID	= NULL_ID;
	m_dpid	= DPID_UNKNOWN;
	m_nProcessorTime	= 0;
	m_nCommittedBytesInUse	= 0;
	m_uPlayerCount	= 0;
}

CSvrInfo::~CSvrInfo()
{

}

void CSvrInfo::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar.WriteString( m_lpAddr );
		ar << m_fConnect << m_dwID;
		ar << m_nProcessorTime << m_nCommittedBytesInUse;
		ar << m_uPlayerCount;
	}
	else
	{
		ar.ReadString( m_lpAddr );
		ar >> m_fConnect >> m_dwID;
		ar >> m_nProcessorTime >> m_nCommittedBytesInUse;
		ar >> m_uPlayerCount;
	}
}

CSvrMng::CSvrMng()
{
	m_uTotalPlayer	= 0;
	memset( m_auSize, 0, sizeof(u_long) * MAX_SVRTYPE );
}

CSvrMng::~CSvrMng()
{

}

void CSvrMng::Add( CHAR cType, const CHAR* lpAddr, DWORD dwID, DPID dpid )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	int i	= Find( cType, lpAddr, dwID );
	if( i < 0 )
	{
		CSvrInfo* pSvrInfo	= &m_aaSvrInfo[cType][m_auSize[cType]++];
		strcpy( pSvrInfo->m_lpAddr, lpAddr );
		pSvrInfo->m_dwID	= dwID;
		pSvrInfo->m_fConnect	= 1;	// true;
		pSvrInfo->m_dpid	= dpid;
	}
	else
	{
		m_aaSvrInfo[cType][i].m_fConnect	= 1;	// true
		m_aaSvrInfo[cType][i].m_dpid	= dpid;
	}
}

void CSvrMng::Remove( CHAR cType, DPID dpid )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	int i	= Find( cType, dpid );
	if( i < 0 )
	{
		ASSERT( 0 );
	}

	m_aaSvrInfo[cType][i].m_fConnect	= 0;	// false
	m_aaSvrInfo[cType][i].m_nProcessorTime	= 0;
	m_aaSvrInfo[cType][i].m_nCommittedBytesInUse	= 0;
	m_aaSvrInfo[cType][i].m_uPlayerCount	= 0;
}

int CSvrMng::Find( CHAR cType, const CHAR* lpAddr, DWORD dwID )
{
	CSvrInfo* aSvrInfo	= m_aaSvrInfo[cType];
	int nSize	= (int)m_auSize[cType];

	for( int i = 0; i < nSize; i++ )
	{
		if( strcmp( aSvrInfo[i].m_lpAddr, lpAddr ) == 0 && aSvrInfo[i].m_dwID == dwID )
			return( i );
	}
	return( -1 );
}

int	CSvrMng::Find( CHAR cType, DPID dpid )
{
	CSvrInfo* aSvrInfo	= m_aaSvrInfo[cType];
	int nSize	= (int)m_auSize[cType];

	for( int i = 0; i < nSize; i++ )
	{
		if( aSvrInfo[i].m_dpid == dpid )
			return( i );
	}
	return( -1 );
}

void CSvrMng::Cleanup( CHAR cType )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	m_auSize[cType]		= 0;

	for( int j = 0; j < MAX_SVRINFO; j++ )
	{
		*m_aaSvrInfo[cType][j].m_lpAddr		= '\0';
		m_aaSvrInfo[cType][j].m_fConnect	= 0;	// false
		m_aaSvrInfo[cType][j].m_dwID	= NULL_ID;
		m_aaSvrInfo[cType][j].m_uPlayerCount	= 0;
	}
}

void CSvrMng::Serialize( CAr & ar )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	if( ar.IsStoring() )
	{
		ar << m_uTotalPlayer;
		for( int i = 0; i < MAX_SVRTYPE; i++ )
		{
			ar << m_auSize[i];
			for( u_long j = 0; j < m_auSize[i]; j++ )
				m_aaSvrInfo[i][j].Serialize( ar );
		}
	}
	else
	{
		ar >> m_uTotalPlayer;
		u_long uSize;
		for( int i = 0; i < MAX_SVRTYPE; i++ )
		{
			ar >> uSize;

			if( uSize > 0 )
			{
				Cleanup( i );
				m_auSize[i]		= uSize;
				for( u_long j = 0; j < uSize; j++ )
					m_aaSvrInfo[i][j].Serialize( ar );
			}
		}
	}
}