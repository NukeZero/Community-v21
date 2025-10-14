#include "StdAfx.h"
#include "DPDatabaseClient.h"
#include "dpcoreclient.h"
extern	CDPCoreClient	g_dpCoreClient;
#include "DPLoginSrvr.h"
#include "User.h"
#include "LoginProtect.h"

extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CUserMng			g_UserMng;



CDPLoginSrvr::CDPLoginSrvr()
{
	ON_MSG( PACKETTYPE_PRE_JOIN, OnPreJoin );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_QUERYTICKCOUNT, OnQueryTickCount );
	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnAddConnection );
	ON_MSG( PACKETTYPE_AUTHQUERY, OnAuthQuery );
#ifdef __gameguard
	ON_MSG( PACKETTYPE_AUTHQUERY_VER30, OnAuthQueryVer30 );
#endif
#ifdef __xtrap
	ON_MSG( PACKETTYPE_XTRAP_COMMAND_RESPONSE, OnXTrapCommandResponse );
#endif

	memset( m_aCache, 0, sizeof(m_aCache) );
	m_nSizeOfCache	= 0;

	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616
	m_bUseGameGuard	= false;
	//	END100616
	//////////////////////////////////////////////////////////////////////////
}

CDPLoginSrvr::~CDPLoginSrvr()
{

}


//////////////////////////////////////////////////////////////////////////
//	BEGIN100616
void	CDPLoginSrvr::SetUseGameGuard()
{
	m_bUseGameGuard	= true;
}

void	CDPLoginSrvr::ResetUseGameGuard()
{
	m_bUseGameGuard	= false;

	return;
}

bool	CDPLoginSrvr::IsUseGameGuard()
{
	return	m_bUseGameGuard;
}

// bool	CDPLoginSrvr::IsGameGuardAuth( CUser* pUser )
// {
// 	bool	bIsGameGuardAuth	= false;
// 
// 	if( IsUseGameGuard() == false )
// 	{
// 		bIsGameGuardAuth	= false;
// 	}
// 	else
// 	{
// 	}
// 
// 	return	bIsGameGuardAuth;
// }
//	END100616
//////////////////////////////////////////////////////////////////////////

void CDPLoginSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
	case DPSYS_CREATEPLAYERORGROUP:
		{
			LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
			DPID dpid = lpCreatePlayer->dpId;

			CUser* pUser = new CUser( dpid ); 
		
			if( g_UserMng.AddUser( dpid, pUser ) )
			{
#ifdef _gameguard
				//////////////////////////////////////////////////////////////////////////
				//	BEGIN100616
				if( IsUseGameGuard() == true )
				{
					if( pUser->GetConnectLevel() == CL_DEVELOPMENT )
					{
						FLINFO_LOG( PROGRAM_NAME, _T( "CL_DEVELOPMENT is Connected." ) );
					}
					else
					{
						UINT32 uReturnedPacketSize = 0;
						const UINT32 uReturn = pUser->m_csa.Get( &uReturnedPacketSize );
						if( uReturn < 3000 ) {
							if( 0 < uReturnedPacketSize ) {
								if( uReturnedPacketSize <= 4096 ) {
									pUser->m_nWrite++;
									BEFORESEND( ar, PACKETTYPE_AUTHQUERY_VER30 );
									ar << uReturnedPacketSize;
									ar.Write( (void*)pUser->m_csa.packet, uReturnedPacketSize );
									SEND( ar, this, dpid );
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
							DestroyPlayer( dpid );
						}
// 						DWORD dwError = pUser->m_csa.GetAuthQuery();
// 
// 						if( dwError == ERROR_SUCCESS )
// 						{
// 							pUser->m_nWrite++;
// 
// 							BEFORESEND( ar, PACKETTYPE_AUTHQUERY );
// 							ar.Write( (void*)&pUser->m_csa.m_AuthQuery, sizeof(GG_AUTH_DATA) );
// 							SEND( ar, this, dpid );
// 						}
// 						else
// 						{
// 							FLERROR_LOG( PROGRAM_NAME, _T( "CSAUTH//#//%d, %d, %d//%d, %d, %d, %d" ), dwError, pUser->m_nWrite, pUser->m_nRead, pUser->m_csa.m_AuthQuery.dwIndex, pUser->m_csa.m_AuthQuery.dwValue1, pUser->m_csa.m_AuthQuery.dwValue2, pUser->m_csa.m_AuthQuery.dwValue3 );
// 							DestroyPlayer( dpid );
// 						}
					}
				}
#endif
				//	END100616
				//////////////////////////////////////////////////////////////////////////
			}
			else
			{
				DestroyPlayer( dpid );
				SAFE_DELETE( pUser );
			}
			break;
		}
	case DPSYS_DESTROYPLAYERORGROUP:
		{
			LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
			OnRemoveConnection( lpDestroyPlayer->dpId );
			break;
		}
	}
}


void CDPLoginSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);
	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );		// Skip dpid

	if( dwMsgSize < 8 )		// INVALID PACKET
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "PACKET//0" ) );
		return;
	}

	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn )
	{
		( this->*( pfn ) )( ar, idFrom );
	}
	else
	{
		g_dpDBClient.SendToServer( idFrom, lpMsg, dwMsgSize );
	}
}

void CDPLoginSrvr::AddCahce( const char* lpCacheAddr )
{
	FLStrcpy( m_aCache[m_nSizeOfCache].lpAddr, _countof( m_aCache[m_nSizeOfCache].lpAddr ), lpCacheAddr );
	++m_nSizeOfCache;
}

int CDPLoginSrvr::CacheIn( void )
{
	int nIndexOfCache	= 0;
	int nCount	= m_aCache[0].uCount;
	for( int i = 1; i < m_nSizeOfCache; i++ )
	{
		if( (int)( m_aCache[i].uCount ) < nCount ) {
			nIndexOfCache	= i;
			nCount	= m_aCache[i].uCount;
		}
	}
	m_aCache[nIndexOfCache].uCount++;
	return nIndexOfCache;
}

void CDPLoginSrvr::CacheOut( int nIndexOfCache )
{
	m_aCache[nIndexOfCache].uCount--;
}

void CDPLoginSrvr::SendCacheAddr( int nIndexOfCache, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_CACHE_ADDR );
	ar.WriteString( m_aCache[nIndexOfCache].lpAddr );
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::SendError( LONG lError, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_ERROR );
	ar << lError;
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::OnAddConnection( CAr & ar, DPID dpid )
{
	TCHAR lpszAccount[MAX_ACCOUNT] = { 0 };
	TCHAR lpszPassword[MAX_ENCRYPT_PWD] = { 0 };
	DWORD dwAuthKey, dwID;
	BYTE bySelectPage;

	ar >> dwAuthKey;
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar.ReadString( lpszPassword, _countof( lpszPassword ) );
	ar >> dwID;

	// BEGIN110520
	ar >> bySelectPage;
	// END110520

	if( dwAuthKey == 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "dwAuthKey == 0" ) );
		DestroyPlayer( dpid );
		return;
	}

	CMclAutoLock Lock( g_UserMng.m_AddRemoveLock );

	CUser* pUser = g_UserMng.GetUser( lpszAccount );

	if( pUser && pUser->m_dpid != dpid)
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "dpid Error" ) );
		DestroyPlayer( dpid );
		return;
	}

	pUser = g_UserMng.GetUser( dpid );
	if( pUser == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pUser == NULL" ) );
		return;
	}

	if( pUser->GetAuth() == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pUser->GetAuth() == FALSE" ) );
		return;
	}

	pUser->SetExtra( lpszAccount, dwAuthKey );
	if( g_UserMng.AddUser( lpszAccount, pUser ) )
	{
		pUser->m_nIndexOfCache	= g_dpLoginSrvr.CacheIn();
		g_dpLoginSrvr.SendCacheAddr( pUser->m_nIndexOfCache, dpid );

		// BEGIN110520
//		g_dpDBClient.SendGetPlayerList( dpid, lpszAccount, lpszPassword, dwAuthKey, (u_long)dwID );
		g_dpDBClient.SendGetPlayerList( dpid, lpszAccount, lpszPassword, dwAuthKey, (u_long)dwID, bySelectPage );
		// END110520

		SendNumPadId( dpid );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "AddUser() return 0, %s" ), lpszAccount );
		CUser* pDuplicatedUser = g_UserMng.GetUser( lpszAccount );
		if( pDuplicatedUser )
		{
			DestroyPlayer( pDuplicatedUser->m_dpid );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "pUser is NULL, %s" ), lpszAccount );
		}
		DestroyPlayer( dpid );
	}
}

void CDPLoginSrvr::OnRemoveConnection( DPID dpid )
{
	g_UserMng.RemoveUser( dpid );
}

void CDPLoginSrvr::OnPreJoin( CAr & ar, DPID dpid )
{
	char lpszAccount[MAX_ACCOUNT], lpszPlayer[MAX_PLAYER];
	u_long idPlayer;
	int nSecretNum = 0;

	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );
	ar >> nSecretNum;
	CUser* pUser	= g_UserMng.GetUser( dpid );
	if( pUser )
	{
		if( lstrcmp( lpszAccount, pUser->m_pKey ) == 0 )
		{
			if( g_xFlyffConfig->Is2ndPWDMode() == true )
			{
				LOGIN_CHECK result = LOGIN_OK;
				result = CLoginProtect::GetInstance()->Check( idPlayer );
				if( result == LOGIN_3TIMES_ERROR )
				{
					SendError( ERROR_15MIN_PREVENT, dpid );
					return;
				}
				u_long idNumPad = CLoginProtect::GetInstance()->GetNumPadId( dpid );
				if( idNumPad <= MAX_NUMPADID && nSecretNum >= 0 && nSecretNum <= MAX_SECRETNUM )
				{
					int nBankPW = CLoginProtect::GetInstance()->GetNumPad2PW( idNumPad, nSecretNum );
					if( nBankPW <= MAX_SECRETNUM )
						g_dpDBClient.SendLoginProtect( lpszAccount, lpszPlayer, idPlayer, nBankPW, dpid );
					else
						SendLoginProtect( FALSE, dpid );
				}
				else
					SendLoginProtect( FALSE, dpid );
			}
			else
			{
				pUser->m_idPlayer	= idPlayer;
				g_dpCoreClient.SendPreJoin( pUser->m_dwAuthKey, lpszAccount, idPlayer, lpszPlayer );	// o
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Different lpszAccount, %s" ), lpszAccount );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "dpid not found, %s" ), lpszAccount );
	}
}

void CDPLoginSrvr::OnPing( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( dpid );
	if( pUser )
	{
		pUser->m_tPingRecvd		= timeGetTime();

		DWORD dwPingTime;
		ar >> dwPingTime;
		
		BEFORESEND( ar1, PACKETTYPE_PING );
		ar1 << dwPingTime;
		SEND( ar1, this, dpid );
	}
}

void CDPLoginSrvr::OnAuthQuery( CAr & /*ar*/, DPID /*dpid*/ )
{
	return;

// 	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
// 	CUser* pUser	= g_UserMng.GetUser( dpid );
// 	if( pUser )
// 	{
// 		ar.Read( (void*)&pUser->m_csa.m_AuthAnswer, sizeof(GG_AUTH_DATA) );
// 		DWORD dwError	= pUser->m_csa.CheckAuthAnswer();
// 		if( dwError == ERROR_SUCCESS )
// 		{
// 			pUser->SetAuth( TRUE );		// ÀÎÁõµÊ 
// 		}
// 		else
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "CSAUTH//1//%d" ), dwError );
// 			DestroyPlayer( pUser->m_dpid );
// 		}
// 	}
}

#ifdef _gaugeguard
void CDPLoginSrvr::OnAuthQueryVer30( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser* pUser	= g_UserMng.GetUser( dpid );
	if( pUser )
	{
		UINT32 uReceivedSize = 0;
		ar >> uReceivedSize;
		if( uReceivedSize <= 4096 ) {
			ar.Read( (void*)pUser->m_csa.packet, uReceivedSize );
			const UINT32 uReturn = pUser->m_csa.Check( uReceivedSize );
			if( uReturn < 3000 ) {
				pUser->SetAuth( TRUE );
			}
			else {
				FLERROR_LOG( PROGRAM_NAME, _T( "[NPROTECT] error check client CS certification. playerID:(%d), uReturn:(%d)" ), pUser->m_idPlayer, uReturn );
				DestroyPlayer( pUser->m_dpid );
			}
		}
		else {
			FLERROR_LOG( PROGRAM_NAME, _T( "[NPROTECT] invalid data. destroy player : uReceivedSize (%d), playerID:(%d)" ), uReceivedSize, pUser->m_idPlayer );
			DestroyPlayer( pUser->m_dpid );
		}
	}
}
#endif

void CDPLoginSrvr::OnQueryTickCount( CAr & ar, DPID dpid )
{
	DWORD dwTime;
	ar >> dwTime;

	BEFORESEND( ar1, PACKETTYPE_QUERYTICKCOUNT );
	ar1 << dwTime << g_TickCount.GetTickCount();
	SEND( ar1, this, dpid );
}


#ifdef _xtrap
void CDPLoginSrvr::OnXTrapCommandResponse( CAr & ar, DPID dpid )
{
	if( FLXTrap_Server::GetInstance().IsActive() == FALSE )
		return;

	CMclAutoLock	Lock( g_UserMng.m_AddRemoveLock );
	CUser * pUser	= g_UserMng.GetUser( dpid );
	if( pUser == NULL  )
		return;

	char recvBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ] = { 0 };
	ar.Read( recvBuffer, sizeof( recvBuffer ) );

	FLXTrap_Server::GetInstance().Response( pUser->m_XTrapSession, recvBuffer );
}
#endif


void CDPLoginSrvr::SendNumPadId( DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_LOGIN_PROTECT_NUMPAD );
	u_long idNumPad = CLoginProtect::GetInstance()->SetNumPadId( dpid, FALSE );
	ar << idNumPad;
	SEND( ar, this, dpid );
}

void CDPLoginSrvr::SendLoginProtect( BOOL bLogin, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_LOGIN_PROTECT_CERT );
	u_long idNumPad = CLoginProtect::GetInstance()->SetNumPadId( dpid, FALSE );
	ar << bLogin << idNumPad;
	SEND( ar, this, dpid );
}

CDPLoginSrvr	g_dpLoginSrvr;