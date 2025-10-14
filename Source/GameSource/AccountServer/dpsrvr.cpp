#include "stdafx.h"
#include "dpdbsrvr.h"
extern	CDPDBSrvr	g_dpDbSrvr;

#include <lang.h>

#include "dpsrvr.h"
#include "account.h"

#include <logging/FLMyTrace.h>

#include "FLASDBManager.h"
#include "../_Common/FLServerDescription.h"

const int MAX_CONN = 50000;

extern	CAccountMng		g_AccountMng;
extern	FLMyTrace		g_MyTrace;

#include "ConnectionIPCheck.h"

CDPSrvr::CDPSrvr()
{
	m_bCheckAddr	= true;
	m_nMaxConn		=	MAX_CONN;
	m_bReloadPro	= TRUE;
	
// 	memset( m_sAddrPmttd, 0, sizeof(m_sAddrPmttd) );
// 	m_nSizeofAddrPmttd	= 0;

	memset( m_nIPCut, 0, sizeof( m_nIPCut ) );
	m_nSizeofIPCut	= 0;
	

	ON_MSG( PACKETTYPE_ADD_ACCOUNT, OnAddAccount );
	ON_MSG( PACKETTYPE_REMOVE_ACCOUNT, OnRemoveAccount );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );

}

CDPSrvr::~CDPSrvr()
{

}

void CDPSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);
	
	LPBYTE lpBuffer		= (LPBYTE)lpMsg + nSize;
	u_long uBufSize		= dwMsgSize - nSize;

	CAr ar( lpBuffer, uBufSize );
	GETTYPE( ar )
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	FLASSERT( pfn );
	( this->*( pfn ) )( ar, idFrom, *(UNALIGNED LPDPID)lpMsg );
}

void CDPSrvr::OnAddConnection( DPID dpid )
{
	g_AccountMng.AddConnection( dpid );

	FLPacketServerDescriptionNoti kPacket;
	g_pServerDescription->GetAllServerDescription( &kPacket.vecServerDesc );

	SendPacket( dpid, &kPacket );
}

void CDPSrvr::OnRemoveConnection( DPID dpid )
{
	g_AccountMng.RemoveConnection( dpid );
}

void CDPSrvr::DestroyPlayer( DPID dpid1, DPID dpid2 )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DESTROY_PLAYER, dpid2 );
	SEND( ar, this, dpid1 );
}

void CDPSrvr::OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 )
{
	TCHAR	lpszAccount[MAX_ACCOUNT];
	char	lpAddr[16];
	DWORD	dwAuthKey = 0;
	BYTE	cbAccountFlag = 0;
	int		fCheck = 0;

	ar.ReadString( lpAddr, _countof( lpAddr ) );	
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> cbAccountFlag;

	char szCheck[255]	= { 0,};
	char szBak[MAX_ACCOUNT]	= { 0,};

	ar.ReadString( szCheck, _countof( szCheck ) );
	ar.ReadString( szBak, _countof( szBak ) );

	DWORD dwPCBangClass;
	ar >> dwPCBangClass;


	CTime tmBirthDate;
//	if( _GetContentState( CT_SHUTDOWN_RULE  ) == CS_VER1 )
	{
		ar >> tmBirthDate;
	}


	BYTE cbResult = ACCOUNT_CHECK_OK;

	int nPermitip = 0;

	if( CConnectionIPCheck::GetInstance()->IsPermitedInternalConnectionIP( lpAddr ) )	// ip로 테스트서버 EnableCheck
	{
		nPermitip = 1;
	}

	// 1. 외부 아이피 검사
	if( m_bCheckAddr )
	{
		if( !CConnectionIPCheck::GetInstance()->IsPermitedInternalConnectionIP( lpAddr ) ) {
			cbResult = ACCOUNT_EXTERNAL_ADDR;
		}

// 		CMclAutoLock	Lock( m_csAddrPmttd );
// 
// 		int i = 0;
// 		for( ; i < m_nSizeofAddrPmttd; i++ )
// 		{
// 			if( strstr( lpAddr, m_sAddrPmttd[i] ) )
// 				break;		// 검사 통과 
// 		}
// 		if( i == m_nSizeofAddrPmttd )
// 			cbResult = ACCOUNT_EXTERNAL_ADDR;	 
	}

	{
		CMclAutoLock	Lock( m_csIPCut );
		if( lpAddr[0] == '\0' || IsABClass(lpAddr) == TRUE )
			cbResult = ACCOUNT_IPCUT_ADDR;	

		if( lpAddr[0] == '\0' )
			FLERROR_LOG( PROGRAM_NAME, _T( "Not Addr : %s" ), lpAddr );
	}
	
	// 2. MAX, 중복검사  
	if( cbResult == ACCOUNT_CHECK_OK )
	{
		if( g_AccountMng.m_nCount < m_nMaxConn )	
		{
			cbResult = g_AccountMng.AddAccount( lpszAccount, dpid1, dpid2, &dwAuthKey, cbAccountFlag, fCheck, tmBirthDate, lpAddr );

			if( dwPCBangClass != 0 )
				g_AccountMng.PushPCBangPlayer( dwAuthKey, dwPCBangClass );

		}
		else
			cbResult = ACCOUNT_OVERFLOW;
	}


	OnAfterChecking( cbResult, lpszAccount, dpid1, dpid2, dwAuthKey, cbAccountFlag, 0, szCheck, szBak, nPermitip );
}

void CDPSrvr::OnAfterChecking( BYTE cbResult, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, 
							   DWORD dwAuthKey, BYTE cbAccountFlag, time_t /*lTimeSpan*/, const char* szCheck, const char* szBak, int nPermitip )
{
	if( cbResult == ACCOUNT_CHECK_OK )
	{
		g_xASDBManager->UpdateTracking( TRUE, lpszAccount );  // 유저가 login 했음을 디비에 쓴다.
	}

	BEFORESENDSOLE( ar, PACKETTYPE_ADD_ACCOUNT, dpid2 );
	ar << cbResult;
	ar << dwAuthKey;
	ar << cbAccountFlag;
	ar << nPermitip;

	ar.WriteString( lpszAccount );
	ar.WriteString( szCheck );

	ar.WriteString( szBak );

	SEND( ar, this, dpid1 );
}

void CDPSrvr::OnRemoveAccount( CAr & /*ar*/, DPID dpid1, DPID dpid2 )
{
	g_AccountMng.RemoveAccount( dpid1, dpid2 );
}


void CDPSrvr::OnPing( CAr & /*ar*/, DPID dpid1, DPID dpid2 )
{
	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );

	CAccount* pAccount	= g_AccountMng.GetAccount( dpid1, dpid2 );
	if( NULL != pAccount )
		pAccount->m_dwPing	= timeGetTime();
}

// bool CDPSrvr::LoadAddrPmttd( LPCTSTR lpszFileName )
// {
// 	CMclAutoLock	Lock( m_csAddrPmttd );
// 
// 	m_nSizeofAddrPmttd	= 0;
// 
// 	CScanner s;
// 
// 	if( s.Load( lpszFileName ) )
// 	{
// 		s.GetToken();
// 		while( s.tok != FINISHED )
// 		{
// 			FLStrcpy( m_sAddrPmttd[m_nSizeofAddrPmttd], _countof( m_sAddrPmttd[m_nSizeofAddrPmttd] ), (LPCSTR)s.Token );
// 			++m_nSizeofAddrPmttd;
// 			s.GetToken();
// 		}
// 		return true;
// 	}
// 	return false;
// }


BOOL CDPSrvr::LoadIPCut( LPCSTR lpszFileName )
{
	CMclAutoLock	Lock( m_csIPCut );

	InitIPCut();

	char strABClass[MAX_PATH];

	CScanner s;
	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			ZeroMemory( strABClass, sizeof( strABClass ) );
			
			GetABCClasstoString( s.Token, strABClass, m_nIPCut[m_nSizeofIPCut][0] );
			s.GetToken();
			s.GetToken();
			GetABCClasstoString( s.Token, strABClass, m_nIPCut[m_nSizeofIPCut][1] );

			int nFindLast = -1;
			STRING2INT::iterator iFind = m_sIPCut.find( strABClass );
			if( iFind != m_sIPCut.end() )
			{	
				const int nFind = iFind->second;
				if( nFind >= MAX_IP )
				{
					return FALSE;
				}

				int nBufFind = m_nIPCut[nFind][2];
				nFindLast = nFind;
				while( nBufFind != -1 )
				{
					nFindLast = nBufFind;
					nBufFind = m_nIPCut[nBufFind][2];
				}
			}
			m_sIPCut.insert( std::map<std::string, int>::value_type( strABClass, m_nSizeofIPCut ) );
			++m_nSizeofIPCut;

			if( nFindLast != -1 )
			{
				m_nIPCut[nFindLast][2] = m_nSizeofIPCut - 1;
			}
			s.GetToken();
		}
		return TRUE;
	}
	
	return FALSE;
}

BOOL CDPSrvr::IsABClass( LPCSTR lpszIP )
{
	char strABClass[MAX_PATH] = {0,};
	int nCClass = 0;
	GetABCClasstoString( lpszIP, strABClass, nCClass );
	STRING2INT::iterator iFind = m_sIPCut.find( strABClass );
	if( iFind != m_sIPCut.end() )
	{
		int nFind = iFind->second;
		if( m_nIPCut[nFind][0] <= nCClass && nCClass <= m_nIPCut[nFind][1] )
		{
			return TRUE;
		}

		while( m_nIPCut[nFind][2] != -1 )
		{
			nFind = m_nIPCut[nFind][2];
			if( m_nIPCut[nFind][0] <= nCClass && nCClass <= m_nIPCut[nFind][1] )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CDPSrvr::GetABCClasstoString( LPCSTR lpszIP, char * lpABClass, int &nCClass )
{
	int nFindAB, nFindC;
	CString strBuf1;
	char strCClass[MAX_PATH];
	ZeroMemory( strCClass, sizeof( strCClass ) );
	strBuf1 = lpszIP;
	nFindAB = strBuf1.Find( "." );
	if( nFindAB == -1 )
		AfxMessageBox( strBuf1 );
	nFindAB = strBuf1.Find( ".", nFindAB + 1 );
	if( nFindAB == -1 )
		AfxMessageBox( strBuf1 );
	
	memcpy( lpABClass, lpszIP, nFindAB );
	nFindC = strBuf1.Find( ".", nFindAB + 1 );
	if( nFindC == -1 )
		AfxMessageBox( strBuf1 );
	for( int i = nFindAB + 1, j = 0 ; i < nFindC ; ++i, ++j )
	{
		strCClass[j] = lpszIP[i];
	}
	nCClass = atoi( strCClass );
}


void CDPSrvr::InitIPCut( void )
{
	m_nSizeofIPCut = 0;
	for( int i = 0 ; i < MAX_IP ; ++i )
	{
		m_nIPCut[i][0] = 0;
		m_nIPCut[i][1] = 0;
		m_nIPCut[i][2] = -1;
	}
	m_sIPCut.clear();
}

void CDPSrvr::SendPlayerCount( DWORD dwParentID, DWORD dwMyID, long lConnectionCount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PLAYER_COUNT, DPID_UNKNOWN );
	ar << dwParentID << dwMyID << lConnectionCount;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void	CDPSrvr::SendAccountLogout( const char* szAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_LOGOUT_ACCOUNT, DPID_UNKNOWN );
	ar.WriteString( szAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPSrvr::SendServerConnectionMode( DWORD dwParentID, DWORD dwMyID, long lCanConnection )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ENABLE_SERVER, DPID_UNKNOWN );
	ar << dwParentID << dwMyID << lCanConnection;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPSrvr::RemoteServerConnectionMode( DWORD dwParentID, DWORD dwMyID, long lCanConnection )
{
	g_pServerDescription->SetConnectionMode( dwParentID, dwMyID, lCanConnection );
	SendServerConnectionMode( dwParentID, dwMyID, lCanConnection );
	g_MyTrace.Add( ( dwParentID * 100 + dwMyID ), FALSE, "%d/%d - %s", dwParentID, dwMyID, lCanConnection ? "o": "x" );
}

void CDPSrvr::OnCloseExistingConnection( CAr & ar, DPID /*dpid1*/, DPID /*dpid2*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	CloseExistingConnection( lpszAccount, 0 );	// 0 - 에러가 아니다.
}

// 현재 접속한 어카운트를 끊는다.
// 접속은 2가지 경우가 있다. - 접속 후 플레이를 하는 경우 
//                           - 접속 과정 
void CDPSrvr::CloseExistingConnection( LPCTSTR lpszAccount, LONG lError )
{
	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( lpszAccount );
	if( pAccount )
	{
		if( pAccount->m_fRoute )	// 접속 후 플레이를 하는 경우 
		{
			g_dpDbSrvr.SendCloseExistingConnection( lpszAccount, lError );
		}
		else						// 접속 과정 
		{
			DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
			g_AccountMng.RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
		}
	}
}

void	CDPSrvr::SendPacket( DPID dwDPID, const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;
	ar << DPID_UNKNOWN;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, dwDPID );
}

CDPSrvr		g_dpSrvr;
