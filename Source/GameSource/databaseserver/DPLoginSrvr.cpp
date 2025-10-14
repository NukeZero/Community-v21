#include "StdAfx.h"
#include "dpaccountclient.h"
#include "DPLoginSrvr.h"
#include <logging/FLMyTrace.h>
#include "..\_Network\Objects\Obj.h"
#include <misc.h>
#include <mmsystem.h>

extern	FLMyTrace	g_MyTrace;
extern	CProject	prj;
extern	APP_INFO	g_appInfo;
extern	T_SLOT_ACTION_FLAG	g_tSlotActionFlag;
extern	CDPAccountClient	g_dpAccountClient;

CDPLoginSrvr::CDPLoginSrvr()
{
	m_dpid	= 0xFFFFFFFF;

	ON_MSG( PACKETTYPE_GETPLAYERLIST, OnGetPlayerList );
	ON_MSG( PACKETTYPE_CREATE_PLAYER, OnCreatePlayer );
	ON_MSG( PACKETTYPE_DEL_PLAYER, OnRemovePlayer );
	ON_MSG( PACKETTYPE_LEAVE, OnLeave );
	ON_MSG( PACKETTYPE_CLOSE_ERROR, OnCloseError );
	ON_MSG( PACKETTYPE_LOGIN_PROTECT_CERT, OnLoginProtect );
#ifdef PASSWORD_RESET_2ND
	ON_MSG( PACKETTYPE_CHANGEBANKPASS, OnChangeBankPass );
#endif // PASSWORD_RESET_2ND
	
}

CDPLoginSrvr::~CDPLoginSrvr()
{

}

void CDPLoginSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
//				g_MyTrace.Add( FLMyTrace::Key("Login" ), FALSE, "" );
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
//				g_MyTrace.Add( FLMyTrace::Key("Login" ), TRUE, "" );
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPLoginSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID /*idFrom*/ )
{
	static size_t	nSize	= sizeof(DPID);
	
	CAr ar( (LPBYTE)lpMsg + nSize, dwMsgSize - nSize );
	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );
	
	if( pfn ) {
		( this->*( pfn ) )( ar, *(UNALIGNED LPDPID)lpMsg, (LPBYTE)lpMsg + nSize + nSize, dwMsgSize - nSize - nSize );
	}
	else {
		FLTRACE_LOG( PROGRAM_NAME, _T( "Handler not found(%08x)" ), dw );
	}
}

void CDPLoginSrvr::OnAddConnection( DPID dpid )
{
	TCHAR szIP[16] = { 0, };
	GetPlayerAddr( dpid, szIP, _countof( szIP ) );
	FLINFO_LOG( PROGRAM_NAME, _T( "Connected LoginServer IP:[%s]" ), szIP );

	if( m_dpid == 0xFFFFFFFF )
		m_dpid	= dpid;
}

void CDPLoginSrvr::OnRemoveConnection( DPID dpid )
{
	if( m_dpid == dpid )
	{
		g_dpAccountClient.SendRemoveAllAccounts();
		m_dpid	= 0xFFFFFFFF;
	}
}

void CDPLoginSrvr::SendError( LONG lError, DPID dpid )
{
	BEFORESENDSOLE( ar, PACKETTYPE_ERROR, dpid );
	ar << lError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendFail( long lError, const char* lpszAccount )
{
	BEFORESENDSOLE( ar, PACKETTYPE_FAIL, DPID_UNKNOWN );
	ar << lError;
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendCloseExistingConnection( const char* lpszAccount, LONG lError )
{
	BEFORESENDSOLE( ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	ar << lError;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendOneHourNotify( const char* lpszAccount)
{
	BEFORESENDSOLE( ar, PACKETTYPE_ONE_HOUR_NOTIFY, DPID_UNKNOWN );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPLoginSrvr::SendPlayerList( const char* lpszAccount, const char* lpszPassword, DPID dpid, DWORD dwAuthKey, BYTE bySelectPage )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= g_DbManager.AllocRequest();
	lpDbOverlappedPlus->dpid							= dpid;
	lpDbOverlappedPlus->nQueryMode						= SENDPLAYERLIST;
	lpDbOverlappedPlus->dwAuthKey						= dwAuthKey;
	FLStrcpy( lpDbOverlappedPlus->AccountInfo.szAccount, _countof( lpDbOverlappedPlus->AccountInfo.szAccount ), lpszAccount );
	FLStrcpy( lpDbOverlappedPlus->AccountInfo.szPassword, _countof( lpDbOverlappedPlus->AccountInfo.szPassword ), lpszPassword );

	// BEGIN110520
	lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage	= bySelectPage;
	// END110520

	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPLoginSrvr::OnCreatePlayer( CAr & /*ar*/, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_tSlotActionFlag.bNotCreate == true )
	{
		SendError( ERROR_SLOT_DONOT_CREATE, dpid );
		FLINFO_LOG( PROGRAM_NAME, _T( "해당 서버에서는 캐릭터를 생성할 수 없습니다." ) );
		return;
	}

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = CREATEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPLoginSrvr::OnRemovePlayer( CAr & /*ar*/, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	if( g_tSlotActionFlag.bNotDelete == true )
	{
		SendError( ERROR_SLOT_DONOT_DELETE, dpid );
		FLINFO_LOG( PROGRAM_NAME, _T( "해당 서버에서는 캐릭터를 삭제할 수 없습니다." ) );
		return;
	}

	CAr ar1;
	ar1 << PACKETTYPE_DEL_PLAYER;
	ar1.Write( lpBuf, uBufSize );
	ar1 << dpid;
	u_long nBlockSize;
	LPBYTE pBlock	= ar1.GetBuffer( &nBlockSize );
	g_dpAccountClient.Send( pBlock, nBlockSize, DPID_SERVERPLAYER );

	return;
}

void CDPLoginSrvr::OnGetPlayerList( CAr & /*ar*/, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	CAr ar1;
	ar1 << PACKETTYPE_GETPLAYERLIST;
	ar1.Write( lpBuf, uBufSize );
	ar1 << dpid;
	u_long nBlockSize;
	LPBYTE pBlock	= ar1.GetBuffer( &nBlockSize );
	g_dpAccountClient.Send( pBlock, nBlockSize, DPID_SERVERPLAYER );
}

void CDPLoginSrvr::OnLeave( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT] = { 0, };
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	g_dpAccountClient.SendRemoveAccount( lpszAccount );

	u_long idPlayer;
	ar >> idPlayer;
	if( idPlayer == 0 )
		return;

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid	= dpid;
	lpDbOverlappedPlus->nQueryMode = SAVE_PLAY_TIME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );

#ifdef __INTERNALSERVER
	FLINFO_LOG( PROGRAM_NAME, _T( "CDPLoginSrvr::OnLeave(%s)" ), lpszAccount );
#endif
}

void CDPLoginSrvr::OnCloseError( CAr & ar, DPID /*dpid*/, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	g_dpAccountClient.SendRemoveAccount( lpszAccount );

#ifdef __INTERNALSERVER
	FLERROR_LOG( PROGRAM_NAME, _T( "CDPLoginSrvr::OnCloseError(%s)" ), lpszAccount );
#endif
}

void CDPLoginSrvr::OnLoginProtect( CAr & /*ar*/, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = QM_LOGIN_PROTECT;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#ifdef PASSWORD_RESET_2ND
void CDPLoginSrvr::OnChangeBankPass( CAr & /*ar*/, DPID dpid, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = CHANGE_BANKPASS;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif // PASSWORD_RESET_2ND

void CDPLoginSrvr::SendLoginProtect( const char* lpszAccount, const char* lpszPlayer, u_long idPlayer, BOOL bLogin, DPID dpId )
{
	BEFORESENDSOLE( ar, PACKETTYPE_LOGIN_PROTECT_CERT, dpId );
	ar.WriteString( lpszAccount );
	ar.WriteString( lpszPlayer );
	ar << idPlayer << bLogin;
	SEND( ar, this, DPID_ALLPLAYERS );
}

CDPLoginSrvr	g_dpLoginSrvr;