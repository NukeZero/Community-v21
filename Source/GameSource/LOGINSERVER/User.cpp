#include "stdafx.h"

#include "user.h"
#include "dpdatabaseclient.h"
#include "dpcoreclient.h"
#include "dploginsrvr.h"

#include "../_Common/FLXTrap_Server.h"

extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_dpCoreClient;

MEMPOOLER_IMPLEMENT( CUser, 1024 );

CUser::CUser( DPID dpid )
{
	m_pKey[0] = '\0';
	m_dwAuthKey	= 0;
	m_idPlayer	= 0;
	m_dpid	= dpid;
	m_nIndexOfCache	= 0;
	m_tPingRecvd	= m_dwTime	= timeGetTime();
	m_bIllegal	= FALSE;

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616
#ifdef __gameguard
	if( g_dpLoginSrvr.IsUseGameGuard() == true )
	{
		m_nWrite	= m_nRead	= 0;
		SetAuth( FALSE );		// 인증이 되어 있지 않음 
		m_csa.Init( true );
	}
	else
#endif
	{
		SetAuth( TRUE );		// 인증이 되어 있음 
	}
	//	END100616
	//////////////////////////////////////////////////////////////////////////

	//	mulcom	BEGIN100409	NPROTECT
	//m_tAuth		= time( NULL );
	m_tAuth		= time( NULL ) + AUTH_PERIOD;
	//	mulcom	END100409	NPROTECT


	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616	WARNINGCODE
	m_byConnectLevel	= CL_NORMAL;
	//	END100616	WARNINGCODE
	//////////////////////////////////////////////////////////////////////////

}

CUser::~CUser()
{
#ifdef __gameguard
	m_csa.Close();
#endif
}

//////////////////////////////////////////////////////////////////////////
//	BEGIN100616	WARNINGCODE
BYTE	CUser::SetConnectLevel( BYTE byConnectLevel )
{
	if( byConnectLevel < CL_MAXVALUE )
	{
		m_byConnectLevel	= byConnectLevel;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "byConnectLevel [%d] is invalid." ), (int)( byConnectLevel ) );
	}

	return	m_byConnectLevel;
}

BYTE	CUser::GetConnectLevel()
{
	return	m_byConnectLevel;
}
//	END100616	WARNINGCODE
//////////////////////////////////////////////////////////////////////////

void CUser::SetExtra( const char* pKey, DWORD dwAuthKey )
{
	FLStrcpy( m_pKey, _countof( m_pKey ), pKey );
	m_dwAuthKey	= dwAuthKey;
}

CUserMng::CUserMng()
{
	m_uCount	= 0;
}

CUserMng::~CUserMng()
{
	Free();
}

void CUserMng::Free( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
		SAFE_DELETE( pUser );
	}
	m_dpid2User.clear();
	m_ac2User.clear();
}

// dpid를 키로 콘테이너에 넣는다.
BOOL CUserMng::AddUser( DPID dpid, CUser* pUser )
{
	FLASSERT( dpid == pUser->m_dpid );

	CMclAutoLock Lock( m_AddRemoveLock );

	if( GetUser( dpid ) == NULL )
	{
		bool bResult	= m_dpid2User.insert( C2User::value_type( dpid, pUser ) ).second;
		FLASSERT( bResult );
		m_uCount++;

#ifdef _xtrap
		if( FLXTrap_Server::GetInstance().IsActive() == TRUE )
		{
			if( FLXTrap_Server::GetInstance().AllocSessionBuffer( /*g_dpLoginSrvr, dpid, */pUser->m_XTrapSession ) == FALSE )
			{
				return FALSE;
			}
		}
#endif
		//////////////////////////////////////////////////////////////////////////
		//	BEGIN100616

// 		if( bResult == true )
// 		{
// 			char	szUserIP[MAX_USERIP];
// 			mem_set( szUserIP, 0, sizeof(szUserIP) );
// 
// 			g_dpLoginSrvr.GetPlayerAddr( dpid, szUserIP );
// 
// 			if( strcmp( szUserIP, "59.10.106.2" ) == 0 || strcmp( szUserIP, "192.168.107.44" ) == 0 )
// 			{
// 				pUser->SetConnectLevel( CL_DEVELOPMENT );
// 				pUser->SetAuth( TRUE );
// 			}
// 			else
// 			{
// 				pUser->SetConnectLevel( CL_NORMAL );
// 			}
// 		}
// 		else
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "AddUser Critial Error insert falied" ) );
// 		}
// 
// 		pUser->SetConnectLevel( CL_NORMAL );
// 		pUser->SetAuth( FALSE );

		//	END100616
		//////////////////////////////////////////////////////////////////////////

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// pKey 키로 콘테이너에 넣는다.
BOOL CUserMng::AddUser( const char* pKey, CUser* pUser )
{
	std::map<std::string, CUser*>::iterator it = m_ac2User.find( pKey );
	if( it != m_ac2User.end() )
	{
		return true;
	}

	// lock이 걸린 상태라고 가정됨 
	bool result = m_ac2User.insert( std::map<std::string, CUser*>::value_type( pKey, pUser ) ).second;
	return ( result == true );

//	return FALSE;
}

CUser* CUserMng::GetUser( const char* pKey )
{
	std::map<std::string, CUser*>::iterator i	= m_ac2User.find( pKey );
	if( i != m_ac2User.end() )
		return i->second;
	return NULL;
}

CUser* CUserMng::GetUser( DPID dpid )
{
	C2User::iterator i = m_dpid2User.find( dpid );
	if( i != m_dpid2User.end() )
		return i->second;
	return NULL;
}

BOOL CUserMng::RemoveUser( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	C2User::iterator i = m_dpid2User.find( dpid );
	if( i != m_dpid2User.end() )
	{
		CUser* pUser	= i->second;
		m_dpid2User.erase( i );
		m_ac2User.erase( pUser->m_pKey );

		if( !pUser->m_bIllegal )
		{
			if( *pUser->m_pKey != '\0' )
			{
				g_dpDBClient.SendLeave( pUser->m_pKey, pUser->m_idPlayer, timeGetTime() - pUser->m_dwTime );
			}
			else
				FLERROR_LOG( PROGRAM_NAME, _T( "*pUser->m_pKey is '\\0'" ) );

			if( pUser->m_idPlayer > 0 )
				g_dpCoreClient.SendLeave( pUser->m_idPlayer );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Illegal" ) );
		}
		g_dpLoginSrvr.CacheOut( pUser->m_nIndexOfCache );

		SAFE_DELETE( pUser );
		m_uCount--;
		return TRUE;
	}
	FLERROR_LOG( PROGRAM_NAME, _T( "dpid not found" ) );
	return FALSE;
}

void CUserMng::DestroyAbnormalPlayer( void )
{
#ifdef __INTERNALSERVER
	return;
#else
	DWORD t		= timeGetTime();
	CTime time	= CTime::GetCurrentTime();
	//CString strTime	= time.Format( "%Y/%m/%d %H:%M:%S" );
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
		if( ( t - pUser->m_tPingRecvd ) > 90000 ) // 90
			g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
	}
#endif //__INTERNALSERVER	
}

// nprotect관련 
// ggsrvlib.lib 링크 
// ggauth.dll

// 서버에서 검사하기 위해서 주기적으로 불려지는 함수 
#ifdef __gameguard
void CUserMng::CheckAll()
{
	time_t	t	= time( NULL );
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
		if( pUser->GetAuth() )		// 인증을 받은 자들은 검사 
		{
			if( pUser->m_tAuth < t )
			{
				//////////////////////////////////////////////////////////////////////////
				//	BEGIN100616	WARNINGCODE
				if( pUser->GetConnectLevel() == CL_DEVELOPMENT )
				{
					FLINFO_LOG( PROGRAM_NAME, _T( "pUser [%s][%d] is CL_DEVELOPMENT." ), pUser->m_pKey, pUser->m_idPlayer );
					pUser->m_tAuth	= t + AUTH_PERIOD;
				}
				else
				{
					UINT32 uReturnedPacketSize = 0;
					const UINT32 uReturn = pUser->m_csa.Get( &uReturnedPacketSize );
					if( uReturn < 3000 ) {
						if( 0 < uReturnedPacketSize ) {
							if( uReturnedPacketSize <= 4096 ) {
								pUser->m_nWrite++;
								pUser->m_tAuth	= t + AUTH_PERIOD;
								BEFORESEND( ar, PACKETTYPE_AUTHQUERY_VER30 );
								ar << uReturnedPacketSize;
								ar.Write( (void*)pUser->m_csa.packet, uReturnedPacketSize );
								SEND( ar, &g_dpLoginSrvr, pUser->m_dpid );
							}
							else {
								FLERROR_LOG( PROGRAM_NAME, _T( "[NPROTECT] invalid data : uReturnedPacketSize (%d)" ), uReturnedPacketSize );
							}
						}
						else {
							FLWARNING_LOG( PROGRAM_NAME, _T( "[NPROTECT] invalid data : uReturnedPacketSize (%d)" ), uReturnedPacketSize );
						}
					}
					else {
						FLERROR_LOG( PROGRAM_NAME, _T( "[NPROTECT] failed create CS certification query. playerID:(%d), uReturn:(%d)" ), pUser->m_idPlayer, uReturn );
						g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
					}

// 					DWORD dwError	= pUser->m_csa.GetAuthQuery();
// 					if( dwError == ERROR_SUCCESS )
// 					{
// 						pUser->m_nWrite++;
// 						pUser->m_tAuth	= t + AUTH_PERIOD;
// 						BEFORESEND( ar, PACKETTYPE_AUTHQUERY );
// 						ar.Write( (void*)&pUser->m_csa.m_AuthQuery, sizeof(GG_AUTH_DATA) );
// 						SEND( ar, &g_dpLoginSrvr, pUser->m_dpid );
// 					}
// 					else
// 					{
// 						FLERROR_LOG( PROGRAM_NAME, _T( "CSAUTH//0//%d, %d, %d//%d, %d, %d, %d" ), dwError, pUser->m_nWrite, pUser->m_nRead, pUser->m_csa.m_AuthQuery.dwIndex, pUser->m_csa.m_AuthQuery.dwValue1, pUser->m_csa.m_AuthQuery.dwValue2, pUser->m_csa.m_AuthQuery.dwValue3 );
// 						g_dpLoginSrvr.DestroyPlayer( pUser->m_dpid );
// 					}
				}
				//	END100616	WARNINGCODE
				//////////////////////////////////////////////////////////////////////////
			}
		}
	}
}
#endif
#ifdef _xtrap
void CUserMng::XTrapUpdate()
{
	if( FLXTrap_Server::GetInstance().IsActive() == FALSE )
		return;

	CMclAutoLock	Lock( m_AddRemoveLock );

	for( C2User::iterator i = m_dpid2User.begin(); i != m_dpid2User.end(); ++i )
	{
		CUser* pUser	= i->second;
	//	if( pUser->GetAuth() == FALSE )		// 인증을 받은 자들 검사 
	//		continue;

		FLXTrap_Server::GetInstance().UpdateCheckSession( g_dpLoginSrvr, pUser->m_dpid, pUser->m_XTrapSession );
	}
}
#endif
CUserMng	g_UserMng;