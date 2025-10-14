#include "stdafx.h"
#include "playerdata.h"
#ifdef __CLIENT
#include "../Neuz/dpclient.h"
extern	CDPClient	g_DPlay;
#endif	// __CLIENT

//sun: 11, 캐릭터 정보 통합
CPlayerDataCenter::CPlayerDataCenter()
{
}
CPlayerDataCenter::~CPlayerDataCenter()
{
	Clear();
}

CPlayerDataCenter*	CPlayerDataCenter::GetInstance( void )
{
	static CPlayerDataCenter sPlayerDataMng;
	return &sPlayerDataMng;
}

void CPlayerDataCenter::Clear()
{
	CMclAutoLock	Lock( m_Access );
	for( std::map<u_long, PlayerData*>::iterator i	= m_mapPlayerData.begin(); i != m_mapPlayerData.end(); ++i )
		SAFE_DELETE( i->second );
	m_mapPlayerData.clear();
}

#ifdef __CLIENT
PlayerData*	CPlayerDataCenter::GetPlayerData( u_long idPlayer, BOOL bQuery )
#else	// __CLIENT
PlayerData*	CPlayerDataCenter::GetPlayerData( u_long idPlayer )
#endif	// __CLIENT
{
	PlayerData*	pPlayerData	= NULL;
	std::map<u_long, PlayerData*>::iterator i	= m_mapPlayerData.find( idPlayer );
	if( i == m_mapPlayerData.end() )
	{
#ifdef __CLIENT
		pPlayerData	= new PlayerData;
		pPlayerData->data.nLevel		= 1;
		bool b	= m_mapPlayerData.insert( std::map<u_long, PlayerData*>::value_type( idPlayer, pPlayerData ) ).second;
		if( b == false )
		{
			SAFE_DELETE( pPlayerData );
		}
		if( bQuery )
			g_DPlay.SendQueryPlayerData( idPlayer );
#endif	// __CLIENT
	}
	else
	{
		pPlayerData	= i->second;
#ifdef __CLIENT
		if( pPlayerData->data.nLevel == 0 )
		{
			pPlayerData->data.nLevel		= 1;
			if( bQuery )
				g_DPlay.SendQueryPlayerData( idPlayer );
		}
#endif	// __CLIENT
	}
	return pPlayerData;
}

PlayerData*	CPlayerDataCenter::AddPlayerData( u_long idPlayer, PlayerData & pd )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* ptr	= GetPlayerData( idPlayer );

// __CHIPI_080109 - 캐릭터 삭제 후 동일 이름으로 캐릭터 생성시 문제
	m_mapPlayerStringToId.erase( pd.szPlayer );
// __CHIPI_080109
	bool b	= m_mapPlayerStringToId.insert( std::map<std::string, u_long>::value_type( pd.szPlayer, idPlayer ) ).second;
	FLASSERT( b );

	if( ptr == NULL )
	{
		PlayerData* pPlayerData		= new PlayerData;
		memcpy( pPlayerData, &pd, sizeof(PlayerData) );
		b	= m_mapPlayerData.insert( std::map<u_long, PlayerData*>::value_type( idPlayer, pPlayerData ) ).second;	
		if( b == false )
		{
			SAFE_DELETE( pPlayerData );
		}
		return pPlayerData;
	}
	if( pd.data.nVer == 0 )
	{
		pd.data.nVer	= ptr->data.nVer;
		pd.data.uLogin	= ptr->data.uLogin;
	}
	memcpy( ptr, &pd, sizeof(PlayerData) );
//	FLTRACE_LOG( PROGRAM_NAME, _T( "%s, v = %d, l = %d" ), ptr->szPlayer, ptr->data.nVer, ptr->data.uLogin );
	return ptr;
}

#ifdef __CLIENT
PlayerData*	CPlayerDataCenter::AddPlayerData( u_long idPlayer, const char* pszPlayer )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData	= GetPlayerData( idPlayer );
	if( pPlayerData )
		return pPlayerData;
	bool b	= m_mapPlayerStringToId.insert( std::map<std::string, u_long>::value_type( pszPlayer, idPlayer ) ).second;
	FLASSERT( b );
	pPlayerData		= new PlayerData;
	FLStrcpy( pPlayerData->szPlayer, _countof( pPlayerData->szPlayer ), pszPlayer );
	b	= m_mapPlayerData.insert( std::map<u_long, PlayerData*>::value_type( idPlayer, pPlayerData ) ).second;
	if( b == false )
	{
		SAFE_DELETE( pPlayerData );
	}
	return pPlayerData;
}
#endif	// __CLIENT

BOOL	CPlayerDataCenter::DeletePlayerData( u_long idPlayer )
{
	CMclAutoLock	Lock( m_Access );
	std::map<u_long, PlayerData*>::iterator i	= m_mapPlayerData.find( idPlayer );
	if( i != m_mapPlayerData.end() )
	{
		m_mapPlayerStringToId.erase( i->second->szPlayer );
		SAFE_DELETE( i->second );
		m_mapPlayerData.erase( i );
		return TRUE;
	}
	return FALSE;
}

int CPlayerDataCenter::Serialize( CAr & ar, int nFirst, int nCount )
{
	int nRet	= 0;
	CMclAutoLock	Lock( m_Access );
	if( ar.IsLoading() )
	{
		u_long uSize;
		ar >> uSize;
//		int nRet	= uSize;
		for( u_long i = 0; i < uSize; i++ )
		{
			u_long idPlayer;
			ar >> idPlayer;
			PlayerData pd;
			ar.ReadString( pd.szPlayer, _countof( pd.szPlayer ) );
			ar.Read( &pd.data, sizeof(sPlayerData) );
			AddPlayerData( idPlayer, pd );
		}
	}
	else
	{
		if( !nCount )
		{
			ar << (u_long)m_mapPlayerData.size();
			nRet	= m_mapPlayerData.size();
			for( std::map<u_long, PlayerData*>::iterator i = m_mapPlayerData.begin(); i != m_mapPlayerData.end(); ++i )
			{
				ar << i->first;
				PlayerData* pPlayerData	= i->second; 
				ar.WriteString( pPlayerData->szPlayer );
				ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
			}
		}
		else
		{
			int nRBount	= nFirst + nCount;
			if( nRBount > (int)( m_mapPlayerData.size() ) )
				nRBount	= m_mapPlayerData.size();
			ar << static_cast<u_long>( nRBount - nFirst );
			int nth		= 0;
			for( std::map<u_long, PlayerData*>::iterator i = m_mapPlayerData.begin(); i != m_mapPlayerData.end(); ++i )
			{
				if( nth >= nFirst && nth < nRBount )
				{
					ar << i->first;
					PlayerData* pPlayerData	= i->second; 
					ar.WriteString( pPlayerData->szPlayer );
					ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
					nRet++;
				}	// if
				nth++;
			}	// for
		}	// if
	}
	return nRet;
}

const char*	CPlayerDataCenter::GetPlayerString( u_long idPlayer )
{
//	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData		= GetPlayerData( idPlayer );
	if( pPlayerData )
		return pPlayerData->szPlayer;
	return NULL;
}

u_long	CPlayerDataCenter::GetPlayerId( char* pszPlayer )
{
	u_long idPlayer	 = 0;
	std::map<std::string, u_long>::iterator i		= m_mapPlayerStringToId.find( pszPlayer );
	if( i != m_mapPlayerStringToId.end() )
		idPlayer	= i->second;
	return idPlayer;
}

void CPlayerDataCenter::ReplacePlayerString( u_long idPlayer, char* pszPlayer, char* pszOld, size_t cchOld )
{
	CMclAutoLock	Lock( m_Access );
	PlayerData* pPlayerData	= GetPlayerData( idPlayer );
	if( pPlayerData )
	{
		FLStrcpy( pszOld, cchOld, pPlayerData->szPlayer );
		FLStrcpy( pPlayerData->szPlayer, _countof( pPlayerData->szPlayer ), pszPlayer );
	}
}
