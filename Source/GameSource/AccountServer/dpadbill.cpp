#include "stdafx.h"
#include "dpdbsrvr.h"
#include "dpadbill.h"

extern	CDPDBSrvr	g_dpDbSrvr;
extern	char g_szBillingIP[16];
//static const TCHAR	LOG_BUYING_INFO[] = _T( "BuyingInfo" );

CDPAdbill::CDPAdbill()
{
	m_dwKey	= 0;
}

CDPAdbill::~CDPAdbill()
{

}

void CDPAdbill::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID /*idFrom*/ )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				break;
			}
	}
}

void CDPAdbill::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD /*dwMsgSize*/, DPID idFrom )
{
	PBUYING_INFO3 pbi3	= new BUYING_INFO3;
	memcpy( (void*)pbi3, lpMsg, sizeof(BUYING_INFO) );

	//////////////////////////////////////////////////////////////////////////
	// BILLING IP Check
	TCHAR szAddress[16] = { 0, };
	GetPlayerAddr( idFrom, szAddress, _countof( szAddress ) );

	if( _tcscmp( g_szBillingIP, szAddress ) != 0 )
	{
		FLERROR_LOG( "BillingLog", _T( "[ INVALID BILLING IP CONNECTED. IP(%s), ServerIndex(%02d), BuyPlayer:(%07d), RecvPlayer:(%07d), ItemId:(%d), ItemNum:(%d), szBxaid:(%s) ]" )
			, szAddress, pbi3->dwServerIndex, pbi3->dwSenderId, pbi3->dwPlayerId, pbi3->dwItemId, pbi3->dwItemNum, pbi3->szBxaid );
		SAFE_DELETE( pbi3 );
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	pbi3->dpid	= idFrom;
	pbi3->dwKey	= m_dwKey++;
	pbi3->dwTickCount	= GetTickCount();

	CBuyingInfoManager::GetInstance()->Add( pbi3 );
	g_dpDbSrvr.SendBuyingInfo( (PBUYING_INFO2)pbi3 );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110315 game shop buying info
	TCHAR szBuyingLog[512] = { 0, };
	FLSPrintf( szBuyingLog, _countof( szBuyingLog )
		, "[Billing->AccountServer] - szAddress:[%s], Key:[%d], ServerIndex[%02d], BuyPlayer:[%07d], RecvPlayer:[%07d], ItemId:[%d], ItemNum:[%d], szBxaid:[%s]"
		, szAddress, pbi3->dwKey, pbi3->dwServerIndex, pbi3->dwSenderId, pbi3->dwPlayerId, pbi3->dwItemId, pbi3->dwItemNum, pbi3->szBxaid );

	FLINFO_LOG( "BillingLog", _T( "%s" ), szBuyingLog );

	// END110315 game shop buying info
	//////////////////////////////////////////////////////////////////////////

//	FLTRACE_LOG( PROGRAM_NAME, _T( "RECV PACKETTYPE_BUYING_INFO FROM ADBILLSOFT" ) );
	FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// Recv from billing\t// dwServerIndex = %d\tdwPlayerId = %d\tdwSenderId=%d\tdwItemId = %d\tdwItemNum = %d" ),
		pbi3->dwServerIndex, pbi3->dwPlayerId, pbi3->dwSenderId, pbi3->dwItemId, pbi3->dwItemNum );

//	FLTRACE_LOG( PROGRAM_NAME, _T( "dwServerIndex = %d\tdwPlayerId = %d\tdwItemId = %d\tdwItemNum = %d" ), 
//		pbi3->dwServerIndex, pbi3->dwPlayerId, pbi3->dwItemId, pbi3->dwItemNum );
//	FLINFO_LOG( LOG_BUYING_INFO, _T( "dwServerIndex = %d\tdwPlayerId = %d\tdwItemId = %d\tdwItemNum = %d" ),
//		pbi3->dwServerIndex, pbi3->dwPlayerId, pbi3->dwItemId, pbi3->dwItemNum );
}

CDPAdbill* CDPAdbill::GetInstance()
{
	static CDPAdbill	sdpAdbill;
	return &sdpAdbill;
}

CBuyingInfoManager *	CBuyingInfoManager::GetInstance()
{
	static CBuyingInfoManager	xBuyingInfoMng;

	return	( &xBuyingInfoMng );
}