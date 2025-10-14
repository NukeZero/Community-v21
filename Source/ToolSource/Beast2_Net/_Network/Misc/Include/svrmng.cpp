#include "stdafx.h"
#include "svrmng.h"

CSvrInfo::CSvrInfo()
{
	*m_lpAddr	= '\0';
	m_fConnect	= 0;	// false
}

CSvrInfo::~CSvrInfo()
{

}

void CSvrInfo::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar.WriteString( m_lpAddr );
		ar << m_fConnect;
	}
	else
	{
		ar.ReadString( m_lpAddr );
		ar >> m_fConnect;
	}
}

CSvrMng::CSvrMng()
{
	memset( m_auSize, 0, sizeof(u_long) * MAX_SVRTYPE );
}

CSvrMng::~CSvrMng()
{

}

void CSvrMng::Add( CHAR cType, const CHAR* lpAddr )
{
	int i	= Find( cType, lpAddr );
	if( i < 0 )
	{
		CSvrInfo* pSvrInfo	= &m_aaSvrInfo[cType][m_auSize[cType]++];
		strcpy( pSvrInfo->m_lpAddr, lpAddr );
		pSvrInfo->m_fConnect	= 1;	// true;
	}
	else
	{
		m_aaSvrInfo[cType][i].m_fConnect	= 1;	// true
	}
}

void CSvrMng::Remove( CHAR cType, const CHAR* lpAddr )
{
	int i	= Find( cType, lpAddr );
	if( i < 0 )
	{
		chASSERT( 0 );
	}

	m_aaSvrInfo[cType][i].m_fConnect	= 0;	// false
}

int CSvrMng::Find( CHAR cType, const CHAR* lpAddr )
{
	CSvrInfo* aSvrInfo	= m_aaSvrInfo[cType];
	int nSize	= (int)m_auSize[cType];

	for( int i = 0; i < nSize; i++ )
	{
		if( strcmp( aSvrInfo[i].m_lpAddr, lpAddr ) == 0 )
			return( i );
	}
	return( -1 );
}

void CSvrMng::Cleanup( void )
{
	memset( m_auSize, 0, sizeof(u_long) * MAX_SVRTYPE );

	for( int i = 0; i < MAX_SVRTYPE; i++ )
	{
		for( int j = 0; j < MAX_SVRINFO; j++ )
		{
			*m_aaSvrInfo[i][j].m_lpAddr		= '\0';
			m_aaSvrInfo[i][j].m_fConnect	= 0;	// false
		}
	}
}

void CSvrMng::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		for( int i = 0; i < MAX_SVRTYPE; i++ )
		{
			ar << m_auSize[i];
			for( u_long j = 0; j < m_auSize[i]; j++ )
				m_aaSvrInfo[i][j].Serialize( ar );
		}
	}
	else
	{
		Cleanup();

		for( int i = 0; i < MAX_SVRTYPE; i++ )
		{
			ar >> m_auSize[i];
			for( u_long j = 0; j < m_auSize[j]; j++ )
			{
				m_aaSvrInfo[i][j].Serialize( ar );
			}
		}
	}
}