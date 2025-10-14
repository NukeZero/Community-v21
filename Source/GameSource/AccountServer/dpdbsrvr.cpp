#include "stdafx.h"
#include "dpsrvr.h"
#include "dpadbill.h"
#include "dpdbsrvr.h"
#include "account.h"
#include <misc.h>

#include "FLASDBManager.h"
#include "../_Common/FLServerDescription.h"
#include "../_Common/FLShutdownRule.h"

#include "../_CommonDefine/Packet/FLPacket_Certify.h"

#include <..\Resource\Lang.h>

#include "ConnectionIPCheck.h"

extern	CDPSrvr		g_dpSrvr;
extern	CAccountMng		g_AccountMng;


CDPDBSrvr::CDPDBSrvr()
{
	ON_MSG( PACKETTYPE_MYREG, OnAddConnection );
	ON_MSG( PACKETTYPE_REMOVE_ACCOUNT, OnRemoveAccount );
	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnGetPlayerList );
	ON_MSG( PACKETTYPE_DEL_PLAYER, OnRemovePlayer );
	ON_MSG( PACKETTYPE_JOIN, OnJoin );
	ON_MSG( PACKETTYPE_REMOVE_ALLACCOUNTS, OnRemoveAllAccounts );
	ON_MSG( PACKETTYPE_BUYING_INFO, OnBuyingInfo );

}

CDPDBSrvr::~CDPDBSrvr()
{

}

void CDPDBSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*dpId*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
//			{
//				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				break;
//			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPDBSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	FLASSERT( pfn );
	( this->*( pfn ) )( ar, dpId, (LPBYTE)lpMsg, (u_long)dwMsgSize );
}

void CDPDBSrvr::OnAddConnection( CAr & ar, DPID dpid, LPBYTE, u_long )
{
	DWORD dwIdofServer;
	ar >> dwIdofServer;
	g_AccountMng.AddIdofServer( dpid, dwIdofServer );

	FLPacketServerDescriptionNoti kPacket;
	g_pServerDescription->GetAllServerDescription( &kPacket.vecServerDesc );
	SendPacket( dpid, &kPacket );
}

void CDPDBSrvr::OnRemoveConnection( DPID dpid )
{
	g_AccountMng.RemoveIdofServer( dpid );
}

void CDPDBSrvr::OnRemoveAccount( CAr & ar, DPID /*dpid*/, LPBYTE, u_long )
{
	TCHAR lpszAccount[MAX_ACCOUNT];
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	g_AccountMng.RemoveAccount( lpszAccount );
}

void CDPDBSrvr::OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char szAccount[MAX_ACCOUNT]		= { 0, };
	char szPassword[MAX_ENCRYPT_PWD]	= { 0, };
	char szDeleteKey[MAX_PASSWORD]			= { 0, };
	ar.ReadString( szAccount, _countof( szAccount ) );
	ar.ReadString( szPassword, _countof( szPassword ) );	// szPassword
	ar.ReadString( szDeleteKey, _countof( szDeleteKey ) );	// szNo
	u_long idPlayer = NULL_PLAYER_ID;
	ar >> idPlayer;		// x
	DWORD dwAuthKey;
	ar>> dwAuthKey;		// o
	size_t nSize; u_long uTemp;
	ar >> nSize;
	for( size_t i=0; i<nSize; i++ )
	{
		ar >> uTemp;	// 그냥 받기만 한다.(쓸모없음)
	}

	// BEGIN110520
	BYTE bySelectPage;
	ar >> bySelectPage;		// x
	// END110520

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( szAccount );
	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1/*step : select character*/ )
	{
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
		FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// PACKETTYPE_DEL_PLAYER" ) );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "RemovePlayer Error. Account:[%s], PlayerId:[%07d]" ), szAccount, idPlayer );
	}
}

void CDPDBSrvr::OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	DWORD dwAuthKey		= 0;
	u_long uIdofMulti	= 0;
	TCHAR szAccount[MAX_ACCOUNT]	= { 0, };
	TCHAR szPassword[MAX_ENCRYPT_PWD]	= { 0, };

	ar >> dwAuthKey;	// o
	ar >> uIdofMulti;
	ar.ReadString( szAccount, _countof( szAccount ) );	// o
 	ar.ReadString( szPassword, _countof( szPassword ) );
 
// 	// BEGIN110520
 	BYTE bySelectPage	= 0;
 	ar >> bySelectPage;
// 	// END110520

	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );

	// BEGIN110520
	CAccount* pAccount	= g_AccountMng.GetAccount( szAccount );
	if( pAccount == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Account Data Not Found. szAccount(%s) ]" ), szAccount );
		SendFail( ERROR_FLYFF_ACCOUNT, szAccount, dpid );
		return;
	}

	if( pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_bCertify == TRUE )
	{
		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );

		if( pAccount->m_cbRef == 0 )
		{
			++pAccount->m_cbRef;
			pAccount->m_fRoute			= TRUE;
			pAccount->m_dwIdofServer	= g_AccountMng.GetIdofServer( dpid );
			pAccount->m_uIdofMulti		= uIdofMulti;

			const T_SERVER_DESC* pServerDesc  = g_pServerDescription->FindByID( pAccount->m_dwIdofServer, uIdofMulti );
			if( pServerDesc != NULL )
			{
				g_dpSrvr.DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
			}
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Account Data Invalid. szAccount(%s), ExsitAuthKey(%d), ReceiveAuthKey(%d), m_bCertify(%d) ]" )
			, szAccount, pAccount->m_dwAuthKey, dwAuthKey, pAccount->m_bCertify );
		SendFail( ERROR_FLYFF_ACCOUNT, szAccount, dpid );
	}

// 	CAccount* pAccount	= g_AccountMng.GetAccount( szAccount );
// 	if( pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 0 )
// 	{
// 		pAccount->m_cbRef++;
// 		Send( (LPVOID)lpBuf, (DWORD)uBufSize, dpid );
// 		pAccount->m_fRoute	= TRUE;
// 		pAccount->m_dwIdofServer	= g_AccountMng.GetIdofServer( dpid );
// 		pAccount->m_uIdofMulti	= uIdofMulti;
// 		u_long uId	= pAccount->m_dwIdofServer * 100 + uIdofMulti;
// 		std::map<u_long, LPSERVER_DESC>::iterator i	= g_dpSrvr.m_2ServersetPtr.find( uId );
// 		if( i != g_dpSrvr.m_2ServersetPtr.end() )
// 		{
// 			g_dpSrvr.DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
// 		}
// 	}
// 	else
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "OnGetPlayerList - Account Invalid. Account:[%s]" ), szAccount );
// 		SendFail( ERROR_FLYFF_ACCOUNT, szAccount, dpid );		// ^
// 	}
	// END110520
}

void CDPDBSrvr::OnJoin( CAr & ar, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	FLPacketDBtoA_ChannelJoinReq fromDB;
	fromDB.Deserialize( ar );


	CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
	CAccount* pAccount	= g_AccountMng.GetAccount( fromDB.m_kAccount.szAccount );
	if( pAccount == NULL )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ Get Account Failed. Account(%s) ]" ), fromDB.m_kAccount.szAccount );
		return;
	}

	if( pAccount->m_dwAuthKey == fromDB.m_kAccount.dwAuthKey 
		&& pAccount->m_bCertify == TRUE && pAccount->m_cbRef == 1 )
	{
		FLPacketAtoDB_ChannelJoinAck toDB;
		toDB.m_kAccount			= fromDB.m_kAccount;
		toDB.m_kDpids			= fromDB.m_kDpids;
		toDB.m_dwPCBangClass	= g_AccountMng.PopPCBangPlayer( fromDB.m_kAccount.dwAuthKey );
		toDB.m_tmBirthDate		= pAccount->m_tmBirthDate;
		toDB.m_permitedIP		= CConnectionIPCheck::GetInstance()->IsPermitedInternalConnectionIP( pAccount->m_address );

		if( g_xFlyffConfig->GetMainLanguage() == LANG_KOR && FLShutdownRule::InShutDownRule( pAccount->m_tmBirthDate ) == true )
		{
			toDB.m_eResult		= E_LOGIN_SHUTDOWN_RULE;
			SendPacket( dpid, &toDB );
			FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// PACKETTYPE_JOIN SUTDOWN RULE" ) );
			return;
		}

		pAccount->m_cbRef++;
		toDB.m_eResult		= E_LOGIN_SUCCESS;
		SendPacket( dpid, &toDB );
		FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// PACKETTYPE_JOIN" ) );
		
		// 동접을 보낸다.
		long lConnectionCount = g_pServerDescription->IncreaseConnectionCount( pAccount->m_dwIdofServer, pAccount->m_uIdofMulti );
		g_dpSrvr.SendPlayerCount( pAccount->m_dwIdofServer, pAccount->m_uIdofMulti, lConnectionCount );
	}
	else
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "[ Account Data Invalid. Account(%s), Account_AuthKey(%d), Receive_AuthKey(%d), Account_bCertify(%d), Account_cbRef(%d) ]" )
		, fromDB.m_kAccount.szAccount, pAccount->m_dwAuthKey, fromDB.m_kAccount.dwAuthKey, pAccount->m_bCertify, pAccount->m_cbRef );
	}
}

void CDPDBSrvr::SendCloseExistingConnection( const char* lpszAccount, LONG /*lError*/ )
{
	BEFORESEND( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPDBSrvr::SendPlayerCount( void )
{
	for( int nChannel = 0; nChannel < (int)( m_vecstrChannelAccount.size()+1 ); nChannel++ )
	{

		DWORD dwParent	= NULL_ID;
		int cbSize = 0;
		int anCount[64];
		DPID adpid[64] = { 0xffff, };
			//mem_set( adpid, 0xff, sizeof(adpid) );

		DWORD dwIdofServer;
		g_AccountMng.m_AddRemoveLock.Enter();
		for( std::map<DWORD, DWORD>::iterator i2 = g_AccountMng.m_2IdofServer.begin(); i2 != g_AccountMng.m_2IdofServer.end(); ++i2 )
		{
			dwIdofServer	= i2->second;
			if( dwIdofServer >= 0 && dwIdofServer < 64 )
				adpid[dwIdofServer]		= i2->first;
		}
		g_AccountMng.m_AddRemoveLock.Leave();

		ServerDescVec vecServerDesc;
		g_pServerDescription->GetAllServerDescription( &vecServerDesc );

		for( ServerDescCItr kCItr = vecServerDesc.begin(); kCItr != vecServerDesc.end(); ++kCItr )
		{
			if( kCItr->dwParentID == NULL_ID )
			{
				if( dwParent >= 0 && dwParent < 64 && adpid[dwParent] != DPID_UNKNOWN )
				{
					BEFORESEND( ar, PACKETTYPE_PLAYER_COUNT );
					ar << nChannel;
					ar << cbSize;
					ar.Write( (void*)anCount, sizeof(int) * cbSize );
					SEND( ar, this, adpid[dwParent] );
				}
				dwParent	= kCItr->dwMyID;
				cbSize	= 0;
			}
			else
			{
				if( nChannel == 0 )
				{
					anCount[cbSize++]		= kCItr->lConnectionCount;
				}
				else
				{
					CMclAutoLock	Lock( g_AccountMng.m_AddRemoveLock );
					long lCount = 0;
					std::map<std::string, CAccount*> mapTemp = g_AccountMng.GetMapAccount();
					for( std::map<std::string, CAccount*>::iterator it=mapTemp.begin(); it!=mapTemp.end(); ++it )
					{
						if( it->first.find( m_vecstrChannelAccount.at(nChannel-1) ) != -1
							&& it->second->m_dwIdofServer == kCItr->dwParentID
							&& it->second->m_uIdofMulti == kCItr->dwMyID )
						{
							lCount++;
						}
					}
					anCount[cbSize++] = lCount;
				}
			}
		}
		if( dwParent >= 0 && dwParent < 64 && adpid[dwParent] != DPID_UNKNOWN )
		{
			BEFORESEND( ar, PACKETTYPE_PLAYER_COUNT );
			ar << nChannel;
			ar << cbSize;
			ar.Write( (void*)anCount, sizeof(int) * cbSize );
			SEND( ar, this, adpid[dwParent] );
		}
	}
}

void CDPDBSrvr::OnRemoveAllAccounts( CAr & /*ar*/, DPID dpid, LPBYTE, u_long )
{
	g_AccountMng.RemoveIdofServer( dpid, FALSE );
}


void CDPDBSrvr::SendFail( long lError, const char* lpszAccount, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_FAIL );
	ar << lError;
	ar.WriteString( lpszAccount );
	SEND( ar, this, dpid );
}




void CDPDBSrvr::SendOneHourNotify( CAccount* pAccount )
{
	BEFORESEND( ar, PACKETTYPE_ONE_HOUR_NOTIFY );
	ar.WriteString( pAccount->m_lpszAccount );
	
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPDBSrvr::SendBuyingInfo( PBUYING_INFO2 pbi2 )
{
	BEFORESEND( ar, PACKETTYPE_BUYING_INFO );
	ar.Write( pbi2, sizeof(BUYING_INFO2) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void	CDPDBSrvr::SendPacket( DPID dwDPID, const FLPacket* pPacket )
{
	if( pPacket == NULL )
	{
		return;
	}

	CAr ar;
	u_long nBufSize = 0;

	if( pPacket->Serialize( ar ) == false )
	{
		return;
	}

	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
	Send( lpBuf, nBufSize, dwDPID );
}


void CDPDBSrvr::OnBuyingInfo( CAr & ar, DPID /*dpid*/, LPBYTE lpBuf, u_long uBufSize )
{
	BUYING_INFO2	bi2;
	SERIALNUMBER	iSerialNumber = 0;

	ar.Read( (void*)&bi2, sizeof(BUYING_INFO2) );
	ar >> iSerialNumber;

	{
		CMclAutoLock Lock( CBuyingInfoManager::GetInstance()->m_AddRemoveLock );

		PBUYING_INFO3 pbi3	= CBuyingInfoManager::GetInstance()->Get( bi2.dwKey );
		if( pbi3 )
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// Recv from trans\t// dwRetVal = %d" ), bi2.dwRetVal );

			//////////////////////////////////////////////////////////////////////////
			// BEGIN110315 game shop buying info

			TCHAR szBuyingLog[512] = { 0, };

			if( CDPAdbill::GetInstance()->Send( &bi2, sizeof(BUYING_INFO), bi2.dpid ) == TRUE )
			{
				FLSPrintf( szBuyingLog, _countof( szBuyingLog )
					, "[AccountServer->Billing] [ SEND OK ]- Key:[%d], ServerIndex[%02d], InvenCreate:[%d], BuyPlayer:[%07d], RecvPlayer:[%07d], ItemId:[%d], ItemNum:[%d], szBxaid:[%s], iSerialNumber:[%d]"
					, bi2.dwKey, bi2.dwServerIndex, bi2.dwRetVal, bi2.dwSenderId, bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum, bi2.szBxaid, iSerialNumber );
			}
			else
			{
				
				FLSPrintf( szBuyingLog, _countof( szBuyingLog )
					, "[AccountServer->Billing] [ SEND FAIL ] - Key:[%d], ServerIndex[%02d], InvenCreate:[%d], BuyPlayer:[%07d], RecvPlayer:[%07d], ItemId:[%d], ItemNum:[%d], szBxaid:[%s], iSerialNumber:[%d]"
					, bi2.dwKey, bi2.dwServerIndex, bi2.dwRetVal, bi2.dwSenderId, bi2.dwPlayerId, bi2.dwItemId, bi2.dwItemNum, bi2.szBxaid, iSerialNumber );
			}

			FLINFO_LOG( "BillingLog", _T( "%s" ), szBuyingLog );

			// END110315 game shop buying info
			//////////////////////////////////////////////////////////////////////////

			CBuyingInfoManager::GetInstance()->Remove( bi2.dwKey );
			SAFE_DELETE( pbi3 );
			/*
			bi2.szBxaid		// 구매 번호
			bi2.dwItemId	// 아이템 인덱스
			bi2.dwItemNum	// 아이템 개수
			bi2.dwServerIndex	// 서버 인덱스
			bi2.dwPlayerId		// 플레이어 식별자
			bi2.dwRetVal	// 지급 성공 여부, 성공 : 1, 실패 : 0
			iSerialNumber		// 시리얼 번호
			*/

			PT_AS_DBOVERLAPPED lpDbOverlappedPlus = g_xASDBManager->m_pDbIOData->Alloc();
			::memcpy( lpDbOverlappedPlus->lpBuf, lpBuf + sizeof(DWORD), uBufSize - sizeof(DWORD) );
			lpDbOverlappedPlus->dwBufSize	= uBufSize - sizeof(DWORD);
			lpDbOverlappedPlus->nQueryMode	= LOG_SM_ITEM;
			PostQueuedCompletionStatus( g_xASDBManager->m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );
		}
		else
		{
			// 치명적 오류 : 어카운트 서버에는 지급 명령에 대한 정보가 없다.
			FLERROR_LOG( "BillingLog", _T( "BUYING_INFO3 Not Exist. Key:[%d]" ), bi2.dwKey );
		}
	}

	FLTRACE_LOG( PROGRAM_NAME, _T( "RECV PACKETTYPE_BUYING_INFO FROM TRANS" ) );
}
//
//bool CDPDBSrvr::inShutDownRule( const CTime & tmBirthDate )
//{
//	//16살 이상이면
//	const CTime tmExpired		= tmBirthDate + CTimeSpan( FLShutdownRule::SHUTDOWN_AGE * 365, 0, 0, 0 );
//	if( tmExpired >= CTime::GetCurrentTime() )
//		return false;
//
//	//셧다운 시간이면
//	const CTime & tmCurr		= CTime::GetCurrentTime();
//	if( FLShutdownRule::SHUTDOWN_START_HOUR > FLShutdownRule::SHUTDOWN_END_HOUR )
//	{
//		if( FLShutdownRule::SHUTDOWN_START_HOUR > tmCurr.GetHour() && tmCurr.GetHour() > FLShutdownRule::SHUTDOWN_END_HOUR )
//			return false;
//	}
//	else
//	{
//		if( FLShutdownRule::SHUTDOWN_START_HOUR > tmCurr.GetHour() || tmCurr.GetHour() > FLShutdownRule::SHUTDOWN_END_HOUR )
//			return false;
//	}
//
//	return true;
//}


CDPDBSrvr	g_dpDbSrvr;