#include "stdafx.h"
#include "dpaccountclient.h"
#include "dpcertifier.h"
#include "dbmanager.h"
#include <..\Resource\Lang.h>

#include "user.h"

#include "../_Network/tools.h"
#include "../_CommonDefine/Packet/FLPacket_Certify.h"


extern	CDPAccountClient	g_dpAccountClient;
extern	CDbManager			g_DbManager;
//  [10/22/2014 ChangSeop.Kim]
extern	std::string	g_strInGameShopURL;
//  [10/22/2014 ChangSeop.Kim]

CLoginLimit::CLoginLimit()
{
	m_nLimitation		= INT_MAX;
	m_dwTick			= 0;
	m_nRequestPerSecond	= 0;
}

// 초당 login시도가 제한을 넘는지 체크한다.  ( true - 제한을 넘는다 )
bool CLoginLimit::Check()
{
	m_nRequestPerSecond++;
	if( GetTickCount() > m_dwTick )
	{
		m_dwTick = GetTickCount() + 1000;
		m_nRequestPerSecond = 1;
	}

	return ( m_nRequestPerSecond > m_nLimitation );
}

CDPCertifier::CDPCertifier()
{
	ON_MSG( PACKETTYPE_CERTIFY, OnCertify );
	ON_MSG( PACKETTYPE_PING, OnPing );
	ON_MSG( PACKETTYPE_KEEP_ALIVE, OnKeepAlive );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ERROR, OnError );
}

CDPCertifier::~CDPCertifier()
{

}

void CDPCertifier::SetLimitation( int /*nLimit*/ )
{
//	if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
//	{
//		if( nLimit > 0 )
//			m_limit.SetLimitation( nLimit );
//	}
}

void CDPCertifier::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*dpid*/ )
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

void CDPCertifier::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	
	if( dwMsgSize < 4 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "PACKET//0" ) );
		return;
	}

	GETTYPE( ar );
	PACKET_HANDLER_FUNC pfn	=	GetHandler( dw );

//	FLASSERT( pfn );
	if( pfn )
		( this->*( pfn ) )( ar, dpid, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 초단위로 보낸다.
long GetPlayLeftTime( BYTE cbAccountFlag )
{
	if( cbAccountFlag & ACCOUNT_FLAG_18 )	// 성인은 남은 22:00시에 끊기지 않기에, 시간을 알리지 않게함 
		return 0;		

	CTime cur = CTime::GetCurrentTime();
	
	CTimeSpan span; 
	if( cur.GetHour() < 22 )
	{
		CTime target( cur.GetYear(), cur.GetMonth(), cur.GetDay(), 22, 0, 0 );
		span = target - cur;
	}
	else
	{
		CTime target( cur.GetYear(), cur.GetMonth(), cur.GetDay(), 24, 0, 0 );
		span = target - cur;

		CTimeSpan ts( 0, 22, 0, 0 ); 
		span += ts;
	}

	return max( (long)( span.GetTotalSeconds() ), 1 );	// 적어도 1초 이상 ( 0 과 구분하기 위해서 )
}

void CDPCertifier::SendCertifyAck( DPID dpId, DWORD dwAuthKey, BYTE cbAccountFlag, long /*lTimeSpan*/, const char* szGPotatoNo, const char* szCheck, const char* szBak,const int nGPotatoNo )
{
	BEFORESEND( ar, PACKETTYPE_CERTIFY );
	ar << dwAuthKey;
	ar << cbAccountFlag;

	char	temp[MAX_ACCOUNT];
	FLSPrintf( temp, _countof( temp ), "%s %d", szGPotatoNo, nGPotatoNo );
	
	if ( nGPotatoNo )	ar.WriteString( temp );
	else				ar.WriteString( szGPotatoNo );	

	ar.WriteString( szCheck );

	ar.WriteString( szBak );

	if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )
	{
		long lTimeLeft = GetPlayLeftTime( cbAccountFlag );	// 태국의 경우 돌아오는 22:00시까지의 남은 시간을 보낸다.
		ar << lTimeLeft;
	}
	//  [10/22/2014 ChangSeop.Kim]
	ar.WriteString( g_strInGameShopURL.c_str() );
	//  [10/22/2014 ChangSeop.Kim]

	SEND( ar, this, dpId );
}

void CDPCertifier::SendError( LONG lError, DPID dpId )
{
	BEFORESEND( ar, PACKETTYPE_ERROR );
	ar << lError;
	SEND( ar, this, dpId );
//	DestroyPlayer( dpId ); shutdown을 호출하지 못하는 이 아픔 
}

void CDPCertifier::SendErrorString( const char* szError, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_ERROR_STRING );
	ar.WriteString( szError );
	SEND( ar, this, dpid );
}

void	CDPCertifier::SendErrorCodeString( const LONG errorCode, const char* szError, DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_ERROR_CODE_STRING );
	ar << errorCode;
	ar.WriteString( szError );
	SEND( ar, this, dpid );
}

void	CDPCertifier::SendPacket( DWORD dwDPID, const FLPacket* pPacket )
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

// Handlers
/*________________________________________________________________________________*/
void CDPCertifier::OnAddConnection( DPID dpid )
{
	if( !CUserMng::GetInstance()->AddUser( dpid ) )
	{
		DestroyPlayer( dpid );
	}
}

void CDPCertifier::OnRemoveConnection( DPID dpid )
{
	CUserMng::GetInstance()->RemoveUser( dpid );
	g_dpAccountClient.SendRemoveAccount( dpid );
}

void CDPCertifier::OnCertify( CAr & ar, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	/************************************************************************/
	// deserialize
	int nSiteValue		= 0;
	ar >> nSiteValue;

	char pszAccount[ MAX_ACCOUNT ]			= { 0, };
	ar.ReadString( pszAccount, _countof( pszAccount ) );

	char pszPwd[ MAX_ENCRYPT_PWD ]			= { 0, };
	ar.Read( pszPwd, MAX_ENCRYPT_PWD );

	char pszSessionPwd[ MAX_SESSION_PWD ]	= { 0, };
	if( g_xFlyffConfig->GetMainLanguage() == LANG_TWN )	{
		ar.ReadString( pszSessionPwd, _countof( pszSessionPwd ) );
	}
	/************************************************************************/

	/************************************************************************/
	// check
	if( pszAccount[0] == '\0' )
	{
		DestroyPlayer( dpid );
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ACCOUNT IS NULL ]" ) );
		return;
	}
	/************************************************************************/

	char	szDec[ MAX_ENCRYPT_PWD ] = {0, };

	g_xRijndael->ResetChain();
	g_xRijndael->Decrypt( pszPwd, szDec, MAX_ENCRYPT_PWD, CRijndael::CBC );

	::memcpy( pszPwd, szDec, sizeof( char ) * MAX_ENCRYPT_PWD );

	{
		CMclAutoLock Lock( CUserMng::GetInstance()->m_AddRemoveLock );
		CUser* pUser	= CUserMng::GetInstance()->GetUser( dpid );
		if( pUser )
		{
			pUser->SetAccount( pszAccount );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetUser Failed. dpid:[%d]" ), dpid );
			return;
		}
	}

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	memset( &pData->AuthInfo.AccountInfo, 0, sizeof(pData->AuthInfo.AccountInfo) );

	FLStrcpy( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ), pszAccount );
	FLStrLowercase( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ) );
	FLStrcpy( pData->AuthInfo.AccountInfo.szPassword, _countof( pData->AuthInfo.AccountInfo.szPassword ), pszPwd );
	FLStrcpy( pData->AuthInfo.AccountInfo.szSessionPwd, _countof( pData->AuthInfo.AccountInfo.szSessionPwd ), pszSessionPwd );
	
	pData->AuthInfo.AccountInfo.nSiteValue	= nSiteValue;
	pData->AuthInfo.dwIP					= GetPlayerAddr( dpid );
	pData->AuthInfo.dpId					= dpid;
	pData->nQueryMode						= CERTIFY;
	g_DbManager.PostQ( pData );
}

void CDPCertifier::OnPing( CAr & /*ar*/, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	g_dpAccountClient.SendPing( dpid );
}

void CDPCertifier::OnKeepAlive( CAr & /*ar*/, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	CUserMng* pMng	= CUserMng::GetInstance();
	CMclAutoLock	Lock( pMng->m_AddRemoveLock );

	CUser* pUser	= pMng->GetUser( dpid );
	if( pUser )
		pUser->m_bValid		= TRUE;
}

void CDPCertifier::OnCloseExistingConnection( CAr & ar, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nSiteValue	= 0;
	char pszAccount[MAX_ACCOUNT] = { 0, };
	char pszPwd[MAX_ENCRYPT_PWD] = { 0, };
	
	ar >> nSiteValue;
	ar.ReadString( pszAccount, _countof( pszAccount ) );
	ar.ReadString( pszPwd, _countof( pszPwd ) );

	if( pszAccount[0] == '\0' )
		return;

	LPDB_OVERLAPPED_PLUS pData	= g_DbManager.m_pDbIOData->Alloc();
	FLStrcpy( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ), pszAccount );
	FLStrLowercase( pData->AuthInfo.AccountInfo.szAccount, _countof( pData->AuthInfo.AccountInfo.szAccount ) );
	FLStrcpy( pData->AuthInfo.AccountInfo.szPassword, _countof( pData->AuthInfo.AccountInfo.szPassword ), pszPwd );

	pData->AuthInfo.AccountInfo.nSiteValue	= nSiteValue;
	pData->AuthInfo.dwIP					= 0;		// don't use
	pData->AuthInfo.dpId					= dpid;
	pData->nQueryMode						= CLOSE_EXISTING_CONNECTION;
	g_DbManager.PostQ( pData );
}

void CDPCertifier::OnError( CAr & ar, DPID dpid, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	int nError = 0;
	ar >> nError;
	
	//if( nError == ERROR_SHUTDOWN_RULE )
	//{
	//	g_dpCertifier.SendError( ERROR_SHUTDOWN_RULE, dpid );
	////	FLSnapshot_LoginAck ack;
	////	ack.m_eResult		= E_LOGIN_SHUTDOWN_RULE;
	////	SendPacket( dpid, &ack );
	////	return;
	//}

	DestroyPlayer( dpid );
}

/*________________________________________________________________________________*/
CDPCertifier	g_dpCertifier;