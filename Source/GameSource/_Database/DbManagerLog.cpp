#include "StdAfx.h"
#include "dbmanager.h"
#include "../DatabaseServer/dploginsrvr.h"
#include "../DatabaseServer/dpcoresrvr.h"
#include "../DatabaseServer/dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include <logging/FLMyTrace.h>

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

#include "eveschool.h"

#include "GuildHouse.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;


void	CDbManager::LogTradeItem( CQuery *pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	const int nTradeID	= GetTradeNo();

	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	T_LOG_TRADE kLogTradeOwner;
	kLogTradeOwner.Deserialize( arRead );

	T_LOG_TRADE kLogTradeTrader;
	kLogTradeTrader.Deserialize( arRead );

	call_uspLoggingTrade( *pQuery, nTradeID, kLogTradeOwner.dwWorldID );

	call_uspLoggingTradePlayer( *pQuery, nTradeID, kLogTradeOwner );

	call_uspLoggingTradePlayer( *pQuery, nTradeID, kLogTradeTrader );

	//////////////////////////////////////////////////////////////////////////
	// Owner item log
	LogItemInfo kLogItemInfoOwner;

	FLStrcpy( kLogItemInfoOwner.Action, _countof( kLogItemInfoOwner.Action ), _T( "T" ) );
	FLStrcpy( kLogItemInfoOwner.SendName, _countof( kLogItemInfoOwner.SendName ), kLogTradeOwner.szPlayerName );
	FLStrcpy( kLogItemInfoOwner.RecvName, _countof( kLogItemInfoOwner.RecvName ), kLogTradeTrader.szPlayerName );

	kLogItemInfoOwner.idSendPlayer	= kLogTradeOwner.idPlayerID;
	kLogItemInfoOwner.idRecvPlayer	= kLogTradeTrader.idPlayerID;
	kLogItemInfoOwner.WorldId		= kLogTradeOwner.dwWorldID;
	kLogItemInfoOwner.Gold			= kLogTradeOwner.nTradeGold + kLogTradeOwner.nHaveGold;
	kLogItemInfoOwner.Gold2			= kLogTradeOwner.nTradeGold + kLogTradeOwner.nHaveGold;
	kLogItemInfoOwner.Gold_1		= kLogTradeTrader.nHaveGold;

	for( DWORD dwNth = 0; dwNth < kLogTradeOwner.dwTradeItemCount; ++dwNth )
	{
		T_LOG_ITEM kLogItemOwner;
		kLogItemOwner.Deserialize( arRead );

		call_uspLoggingTradeItem( *pQuery, nTradeID, kLogTradeOwner.idPlayerID, kLogItemOwner );

		kLogItemInfoOwner.kLogItem = kLogItemOwner;

// 		TCHAR szQuery[MAX_QUERY_SIZE] = { 0, };
// 
// 		DBQryNewItemLog( szQuery, _countof( szQuery ), kLogItemInfoOwner );
// 
// 		if( pQuery->Exec( szQuery ) == FALSE )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szQuery );
// 		}

		DBQryNewItemLog( *pQuery, kLogItemInfoOwner );
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Trader item log
	LogItemInfo kLogItemInfoTrader;

	FLStrcpy( kLogItemInfoTrader.Action, _countof( kLogItemInfoTrader.Action ), _T( "T" ) );
	FLStrcpy( kLogItemInfoTrader.SendName, _countof( kLogItemInfoTrader.SendName ), kLogTradeTrader.szPlayerName );
	FLStrcpy( kLogItemInfoTrader.RecvName, _countof( kLogItemInfoTrader.RecvName ), kLogTradeOwner.szPlayerName );

	kLogItemInfoTrader.idSendPlayer	= kLogTradeTrader.idPlayerID;
	kLogItemInfoTrader.idRecvPlayer	= kLogTradeOwner.idPlayerID;
	kLogItemInfoTrader.WorldId		= kLogTradeTrader.dwWorldID;
	kLogItemInfoTrader.Gold			= kLogTradeTrader.nTradeGold + kLogTradeTrader.nHaveGold;
	kLogItemInfoTrader.Gold2		= kLogTradeTrader.nTradeGold + kLogTradeTrader.nHaveGold;
	kLogItemInfoTrader.Gold_1		= kLogTradeOwner.nHaveGold;

	for( DWORD dwNth = 0; dwNth < kLogTradeTrader.dwTradeItemCount; ++dwNth )
	{
		T_LOG_ITEM kLogItemTrader;
		kLogItemTrader.Deserialize( arRead );

		call_uspLoggingTradeItem( *pQuery, nTradeID, kLogTradeTrader.idPlayerID, kLogItemTrader );

		kLogItemInfoTrader.kLogItem = kLogItemTrader;

// 		TCHAR szQuery[MAX_QUERY_SIZE] = { 0, };
// 
// 		DBQryNewItemLog( szQuery, _countof( szQuery ), kLogItemInfoTrader );
// 
// 		if( pQuery->Exec( szQuery ) == FALSE )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szQuery );
// 		}

		DBQryNewItemLog( *pQuery, kLogItemInfoTrader );
	}
	//////////////////////////////////////////////////////////////////////////
}

void CDbManager::LogItem(CQuery *pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	/* 기존 트레이드 로그와 일반 아이템 로그를 같이 처리하는 구조에서 트레이드 로그를 따로 분리 */
	//////////////////////////////////////////////////////////////////////////
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	LogItemInfo kLogItemInfo;
	kLogItemInfo.Deserialize( arRead );

// 	TCHAR szQuery[MAX_QUERY_SIZE]	= { 0, };
// 
// 	DBQryNewItemLog( szQuery, _countof( szQuery ), kLogItemInfo );
// 
// 	if( pQuery->Exec( szQuery ) == FALSE )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ EXECUTE FAILED. Query(%s)]" ), szQuery );
// 	}

	DBQryNewItemLog( *pQuery, kLogItemInfo );
	//////////////////////////////////////////////////////////////////////////


// 	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
// 
// 	// 이곳이 Database server의 핸들러 부분이라면, 
// 	//	ACTION + 캐릭터1 + 캐릭터2 + 장소 + 시드1 + 시드2 + 잔액1 + 잔액2 +
// 	//	아이템개수1 + 아이템개수2 + ( 아이템고유번호 + 아이템이름 + 개수 )1 + .... + ( 아이템고유번호 + 아이템이름 + 개수 )2 + .....
// 	char chAction[2];
// 	char lpszPlayer1[36], lpszPlayer2[36], lpszItemText[36];
// 	arRead.ReadString( chAction, _countof( chAction ) );
// 	char chswitch = chAction[0];
// 	arRead.ReadString( lpszPlayer1, _countof( lpszPlayer1 ) );
// 	arRead.ReadString( lpszPlayer2, _countof( lpszPlayer2 ) );
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 	u_long idSendPlayer, idRecvPlayer;
// 	idSendPlayer = idRecvPlayer = 0;
// 
// 	arRead >> idSendPlayer;
// 	arRead >> idRecvPlayer;
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	CString strPlayer1	= lpszPlayer1;
// 	CString strPlayer2	= lpszPlayer2;
// 	strPlayer1.Replace( '\'', ' ' );
// 	FLStrcpy( lpszPlayer1, _countof( lpszPlayer1 ), (LPCSTR)strPlayer1 );
// 	strPlayer2.Replace( '\'', ' ' );
// 	FLStrcpy( lpszPlayer2, _countof( lpszPlayer2 ), (LPCSTR)strPlayer2 );
// 
// 	DWORD dwWorldID;
// 	arRead >> dwWorldID;
// 	
// 	int iSeed1, iSeed2;
// 	int iReMainder1, iReMainder2;
// 	arRead >> iSeed1 >> iSeed2;
// 
// 	char szQuery[MAX_QUERY_SIZE]	= { 0,};
// 
// 	switch( chswitch )
// 	{
// 	case 'T':
// 		{
// 			int nTradeID	= GetTradeNo();
// 			// 0. tblTradeLog. 트레이드 단위에 대한 저장입니다. (TradeID, 시간, 장소가 저장됩니다)
// 			T_TRADE_ITEM_LOG kTradeLog;
// 			memset( &kTradeLog, 0, sizeof( kTradeLog ) );
// 			
// 			kTradeLog.nFlag = 0;
// 			kTradeLog.nTradeID = nTradeID;
// 			kTradeLog.dwWorldID = dwWorldID;
// 			
// 			call_uspLoggingTrade( qry, kTradeLog );
// 			
// 			arRead >> iReMainder1 >> iReMainder2;
// 			u_long idPlayer1, idPlayer2;
// 			int nLevel1, nLevel2, nJob1, nJob2;
// 			arRead >> idPlayer1 >> nLevel1 >> nJob1;
// 			arRead >> idPlayer2 >> nLevel2 >> nJob2;
// 			char lpAddr1[100]	= { 0, };
// 			char lpAddr2[100]	= { 0, };
// 			arRead.ReadString( lpAddr1, _countof( lpAddr1 ) );
// 			arRead.ReadString( lpAddr2, _countof( lpAddr2 ) );
// 			
// 			// 1. tblTradeDetailLog. 트레이드의 사람별 정보입니다. (idPlayer, IP 등)
// 			memset( &kTradeLog, 0, sizeof( kTradeLog ) );
// 			
// 			kTradeLog.nFlag = 1;
// 			kTradeLog.nTradeID = nTradeID;
// 			kTradeLog.idPlayer = idPlayer1;
// 			kTradeLog.dwTradeGold = iSeed1;
// 			FLStrcpy( kTradeLog.szAddress, _countof( kTradeLog.szAddress ), lpAddr1 );
// 			kTradeLog.nLevel = nLevel1;
// 			kTradeLog.nJob = nJob1;
// 
// 			call_uspLoggingTrade( qry, kTradeLog );
// 
// 			memset( &kTradeLog, 0, sizeof( kTradeLog ) );
// 			
// 			kTradeLog.nFlag = 1;
// 			kTradeLog.nTradeID = nTradeID;
// 			kTradeLog.idPlayer = idPlayer2;
// 			kTradeLog.dwTradeGold = iSeed2;
// 			FLStrcpy( kTradeLog.szAddress, _countof( kTradeLog.szAddress ), lpAddr2 );
// 			kTradeLog.nLevel = nLevel2;
// 			kTradeLog.nJob = nJob2;
// 
// 			call_uspLoggingTrade( qry, kTradeLog );
// 
// 			u_long uSize1, uSize2;
// 			arRead >> uSize1 >> uSize2;
// 			for( DWORD i = 0; i < uSize1; i++ )
// 			{
// 				memset( lpszItemText, 0, sizeof( lpszItemText ) );
// 				SERIALNUMBER iSerialNumber;
// 				int nItemNum = 0;
// 				int	  nAbilityOption = 0;
// 				int	  nItemResist = 0;
// 				int	  nResistAbilityOption = 0;
// 				int   nNegudo = 0;
// 				int	  nMaxNegudo = 0;
// 				DWORD dwItemId	= 0;
// 				arRead >> dwItemId;
// 				arRead >> iSerialNumber;
// 				arRead.ReadString( lpszItemText, _countof( lpszItemText ) );
// 				CString strItem	= lpszItemText;
// 				strItem.Replace( '\'', ' ' );
// 				FLStrcpy( lpszItemText, _countof( lpszItemText ), (LPCSTR)strItem );
// 				arRead >> nItemNum;
// 				arRead >> nAbilityOption;
// 				arRead >> nItemResist;
// 				arRead >> nResistAbilityOption;
// 				arRead >> nNegudo;
// 				arRead >> nMaxNegudo;
// 
// 				LogItemInfo aLogItem;
// 				memset( &(aLogItem), 0, sizeof( aLogItem ) );
// 				//aLogItem.SendName = lpszPlayer1;
// 				//aLogItem.RecvName = lpszPlayer2;
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), lpszPlayer1 );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), lpszPlayer2 );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 				aLogItem.idSendPlayer = idSendPlayer;
// 				aLogItem.idRecvPlayer = idRecvPlayer;
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				aLogItem.WorldId = dwWorldID;
// 				aLogItem.Gold = iReMainder1 + iSeed1;
// 				aLogItem.Gold2 = iReMainder1 + iSeed1;
// 				aLogItem.ItemNo = iSerialNumber;
// 				//aLogItem.ItemName = lpszItemText;
// 				FLStrncpy( aLogItem.szItemName, _countof( aLogItem.szItemName ), lpszItemText, 30 );
// 
// 				aLogItem.itemNumber = nItemNum;
// 				aLogItem.nAbilityOption = nAbilityOption;
// 				aLogItem.nItemResist = nItemResist;
// 				aLogItem.nResistAbilityOption = nResistAbilityOption;
// 				aLogItem.Negudo = nNegudo;
// 				aLogItem.MaxNegudo = nMaxNegudo;
// 				aLogItem.Gold_1 = iReMainder2;
// 				//aLogItem.Action = chAction;
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), chAction );
// 
// 				arRead >> aLogItem.m_bCharged;
// 				arRead >> aLogItem.m_dwKeepTime;
// 				arRead >> aLogItem.nPiercedSize;
// 				
// 				size_t	k = 0;
// 				
// 				for( k=0; k<aLogItem.nPiercedSize; k++ )
// 					arRead >> aLogItem.adwItemId[k];
// 				arRead >> aLogItem.nUMPiercedSize;
// 				for( k=0; k<aLogItem.nUMPiercedSize; k++ )
// 					arRead >> aLogItem.adwUMItemId[k];
// 
// 				arRead >> aLogItem.nPetKind;
// 				arRead >> aLogItem.nPetLevel;
// 				arRead >> aLogItem.dwPetExp;
// 				arRead >> aLogItem.wPetEnergy;
// 				arRead >> aLogItem.wPetLife;
// 				arRead >> aLogItem.nPetAL_D;
// 				arRead >> aLogItem.nPetAL_C;
// 				arRead >> aLogItem.nPetAL_B;
// 				arRead >> aLogItem.nPetAL_A;
// 				arRead >> aLogItem.nPetAL_S;
// 
// 				arRead >> aLogItem.dwCouplePlayerId;
// 				arRead >> aLogItem.nLevelDown;
// 				arRead >> aLogItem.wRandomOptionOriginId;
// 				arRead >> aLogItem.nRandomOptionExtensionSize;
// 				arRead >> aLogItem.wRandomOptionExtensionFlag;
// 				
// 				for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_SIZE; ++nIndex )
// 				{
// 					arRead >> aLogItem.awDstID[ nIndex ];
// 					arRead >> aLogItem.ashAdjValue[ nIndex ];
// 				}
// 
// 				DBQryNewItemLog( szQuery, _countof( szQuery ), aLogItem );
// 
// 				if ( FALSE == qry->Exec( szQuery ) )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s" ), lpszPlayer1, lpszPlayer2 );
// 					return;
// 				}
// 				// 2. tblTradeItemLog. 트레이드 대상 아이템 정보입니다. (ItemIndex, SerialNo 외)
// 				T_TRADE_ITEM_LOG kTradeLogSecong;
// 				memset( &kTradeLogSecong, 0, sizeof( kTradeLogSecong ) );
// 
// 				kTradeLogSecong.nFlag							= 2;
// 				kTradeLogSecong.nTradeID						= nTradeID;
// 				kTradeLogSecong.idPlayer						= idPlayer2;
// 				kTradeLogSecong.nItemIndex					= dwItemId;
// 				kTradeLogSecong.iItemSerialNum				= iSerialNumber;
// 				kTradeLogSecong.nItemCnt						= nItemNum;
// 				kTradeLogSecong.nAbilityOpt					= nAbilityOption;
// 				kTradeLogSecong.nItemResist					= nItemResist;
// 				kTradeLogSecong.nResistAbilityOpt				= nResistAbilityOption;
// 				kTradeLogSecong.dwCouplePlayerId				= aLogItem.dwCouplePlayerId;
// 				kTradeLogSecong.nLevelDown					= aLogItem.nLevelDown;
// 				kTradeLogSecong.wRandomOptionOriginId			= aLogItem.wRandomOptionOriginId;
// 				kTradeLogSecong.nRandomOptionExtensionSize	= aLogItem.nRandomOptionExtensionSize;
// 				kTradeLogSecong.wRandomOptionExtensionFlag	= aLogItem.wRandomOptionExtensionFlag;
// 				for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_SIZE; ++nIndex )
// 				{
// 					kTradeLogSecong.awDstID[ nIndex ]		= aLogItem.awDstID[ nIndex ];
// 					kTradeLogSecong.ashAdjValue[ nIndex ]		= aLogItem.ashAdjValue[ nIndex ];
// 				}
// 				
// 				call_uspLoggingTrade( qry, kTradeLogSecong );
// 			}
// 			for( DWORD i = 0; i < uSize2; i++ )
// 			{
// 				memset( lpszItemText, 0, sizeof( lpszItemText ) );
// 				SERIALNUMBER iSerialNumber;
// 				int nItemNum = 0;
// 				int	  nAbilityOption = 0;
// 				int	  nItemResist = 0;
// 				int	  nResistAbilityOption = 0;
// 				int   nNegudo = 0;
// 				int		nMaxNegudo	= 0;
// 				DWORD dwItemId	= 0;
// 				arRead >> dwItemId;
// 				arRead >> iSerialNumber;
// 				arRead.ReadString( lpszItemText, _countof( lpszItemText ) );
// 				CString strItem	= lpszItemText;
// 				strItem.Replace( '\'', ' ' );
// 				FLStrcpy( lpszItemText, _countof( lpszItemText ), (LPCSTR)strItem );
// 				arRead >> nItemNum;
// 				arRead >> nAbilityOption;
// 				arRead >> nItemResist;
// 				arRead >> nResistAbilityOption;
// 				arRead >> nNegudo;
// 				arRead >> nMaxNegudo;
// 
// 				LogItemInfo aLogItem;
// 				memset( &(aLogItem), 0, sizeof( aLogItem ) );
// 				//aLogItem.SendName = lpszPlayer2;
// 				//aLogItem.RecvName = lpszPlayer1;
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), lpszPlayer2 );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), lpszPlayer1 );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 				aLogItem.idSendPlayer = idSendPlayer;
// 				aLogItem.idRecvPlayer = idRecvPlayer;
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				aLogItem.WorldId = dwWorldID;
// 				aLogItem.Gold = iReMainder2 + iSeed2;
// 				aLogItem.Gold2 = iReMainder2 + iSeed2;
// 				aLogItem.ItemNo = iSerialNumber;
// 				//aLogItem.ItemName = lpszItemText;
// 				FLStrncpy( aLogItem.szItemName, _countof( aLogItem.szItemName ), lpszItemText, 30 );
// 
// 				aLogItem.itemNumber = nItemNum;
// 				aLogItem.nAbilityOption = nAbilityOption;
// 				aLogItem.nItemResist = nItemResist;
// 				aLogItem.nResistAbilityOption = nResistAbilityOption;
// 				aLogItem.Negudo = nNegudo;
// 				aLogItem.MaxNegudo	= nMaxNegudo;
// 				aLogItem.Gold_1 = iReMainder1;
// 				//aLogItem.Action = chAction;
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), chAction );
// 				
// 				arRead >> aLogItem.m_bCharged;
// 				arRead >> aLogItem.m_dwKeepTime;
// 
// 				arRead >> aLogItem.nPiercedSize;
// 				for( size_t j=0; j<aLogItem.nPiercedSize; j++ )
// 					arRead >> aLogItem.adwItemId[j];
// 				arRead >> aLogItem.nUMPiercedSize;
// 				for( size_t j=0; j<aLogItem.nUMPiercedSize; j++ )
// 					arRead >> aLogItem.adwUMItemId[j];
// 
// 				arRead >> aLogItem.nPetKind;
// 				arRead >> aLogItem.nPetLevel;
// 				arRead >> aLogItem.dwPetExp;
// 				arRead >> aLogItem.wPetEnergy;
// 				arRead >> aLogItem.wPetLife;
// 				arRead >> aLogItem.nPetAL_D;
// 				arRead >> aLogItem.nPetAL_C;
// 				arRead >> aLogItem.nPetAL_B;
// 				arRead >> aLogItem.nPetAL_A;
// 				arRead >> aLogItem.nPetAL_S;
// 
// 				arRead >> aLogItem.dwCouplePlayerId;
// 				arRead >> aLogItem.nLevelDown;
// 				arRead >> aLogItem.wRandomOptionOriginId;
// 				arRead >> aLogItem.nRandomOptionExtensionSize;
// 				arRead >> aLogItem.wRandomOptionExtensionFlag;
// 
// 				for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_SIZE; ++nIndex )
// 				{
// 					arRead >> aLogItem.awDstID[ nIndex ];
// 					arRead >> aLogItem.ashAdjValue[ nIndex ];
// 				}
// 
// 				DBQryNewItemLog( szQuery, _countof( szQuery ), aLogItem );
// 
// 				if ( FALSE == qry->Exec( szQuery ) )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s" ), lpszPlayer1, lpszPlayer2 );
// 					return;
// 				}
// 				// 2. tblTradeItemLog. 트레이드 대상 아이템 정보입니다. (ItemIndex, SerialNo 외)
// 				T_TRADE_ITEM_LOG kTradeLogThird;
// 				memset( &kTradeLogThird, 0, sizeof( kTradeLogThird ) );
// 				
// 				kTradeLogThird.nFlag							= 2;
// 				kTradeLogThird.nTradeID						= nTradeID;
// 				kTradeLogThird.idPlayer						= idPlayer1;
// 				kTradeLogThird.nItemIndex					= dwItemId;
// 				kTradeLogThird.iItemSerialNum				= iSerialNumber;
// 				kTradeLogThird.nItemCnt						= nItemNum;
// 				kTradeLogThird.nAbilityOpt					= nAbilityOption;
// 				kTradeLogThird.nItemResist					= nItemResist;
// 				kTradeLogThird.nResistAbilityOpt				= nResistAbilityOption;
// 				kTradeLogThird.dwCouplePlayerId				= aLogItem.dwCouplePlayerId;
// 				kTradeLogThird.nLevelDown					= aLogItem.nLevelDown;
// 				kTradeLogThird.wRandomOptionOriginId			= aLogItem.wRandomOptionOriginId;
// 				kTradeLogThird.nRandomOptionExtensionSize	= aLogItem.nRandomOptionExtensionSize;
// 				kTradeLogThird.wRandomOptionExtensionFlag	= aLogItem.wRandomOptionExtensionFlag;
// 				for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_SIZE; ++nIndex )
// 				{
// 					kTradeLogThird.awDstID[ nIndex ]		= aLogItem.awDstID[ nIndex ];
// 					kTradeLogThird.ashAdjValue[ nIndex ]		= aLogItem.ashAdjValue[ nIndex ];
// 				}
// 				
// 				call_uspLoggingTrade( qry, kTradeLogThird );
// 			}
// 			if( iSeed1 > 0 )
// 			{
// 				LogItemInfo aLogItem;
// 				memset( &(aLogItem), 0, sizeof( aLogItem ) );
// 				//aLogItem.SendName = lpszPlayer1;
// 				//aLogItem.RecvName = lpszPlayer2;
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), lpszPlayer1 );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), lpszPlayer2 );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 				aLogItem.idSendPlayer = idSendPlayer;
// 				aLogItem.idRecvPlayer = idRecvPlayer;
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				aLogItem.WorldId = dwWorldID;
// 				aLogItem.Gold = iReMainder1 + iSeed1;
// 				aLogItem.Gold2 = iReMainder1;
// 				//aLogItem.ItemName = "SEED";
// 				FLSPrintf( aLogItem.szItemName, _countof( aLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
// 				aLogItem.itemNumber = iSeed1;
// 				aLogItem.Gold_1 = iReMainder2;
// 				//aLogItem.Action = chAction;
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), chAction );
// 				DBQryNewItemLog( szQuery, _countof( szQuery ), aLogItem );
// 
// 				if ( FALSE == qry->Exec( szQuery ) )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s" ), lpszPlayer1, lpszPlayer2 );
// 					return;
// 				}
// 			}
// 			if( iSeed2 > 0 )
// 			{
// 				LogItemInfo aLogItem;
// 				memset( &(aLogItem), 0, sizeof( aLogItem ) );
// 				//aLogItem.SendName = lpszPlayer2;
// 				//aLogItem.RecvName = lpszPlayer1;
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), lpszPlayer2 );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), lpszPlayer1 );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 				aLogItem.idSendPlayer = idSendPlayer;
// 				aLogItem.idRecvPlayer = idRecvPlayer;
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				aLogItem.WorldId = dwWorldID;
// 				aLogItem.Gold = iReMainder2 + iSeed2;
// 				aLogItem.Gold2 = iReMainder2;
// 				//aLogItem.ItemName = "SEED";
// 				FLSPrintf( aLogItem.szItemName, _countof( aLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
// 				aLogItem.itemNumber = iSeed2;
// 				aLogItem.Gold_1 = iReMainder1;
// 				//aLogItem.Action = chAction;
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), chAction );
// 				DBQryNewItemLog( szQuery, _countof( szQuery ), aLogItem );
// 
// 				if ( FALSE == qry->Exec( szQuery ) )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s" ), lpszPlayer1, lpszPlayer2 );
// 					return;
// 				}	
// 			}
// 			break;
// 		}
// 	default:
// 		{
// 			memset( lpszItemText, 0, sizeof( lpszItemText ) );
// 			SERIALNUMBER	ItemNo;
// 			DWORD nItemNum = 0;
// 			int nNegudo = 0; // 내구도
// 			int nMaxNegudo	= 0; 
// 			int	  nAbilityOption = 0;
// 			DWORD dwGold_1 = 0;
// 			int nSlot = 0;
// 			int nSlot1 = 0;
// 			int nItemResist = 0;
// 			int nResistAbilityOption = 0;
// 			
// 			arRead >> ItemNo;
// 			arRead >> nNegudo;
// 			arRead >> nMaxNegudo;
// 			arRead.ReadString( lpszItemText, _countof( lpszItemText ) );
// 			CString strItem	= lpszItemText;
// 			strItem.Replace( '\'', ' ' );
// 			FLStrcpy( lpszItemText, _countof( lpszItemText ), (LPCSTR)strItem );
// 			arRead >> nItemNum;
// 			arRead >> nAbilityOption;
// 			arRead >> dwGold_1;
// 			arRead >> nSlot;
// 			arRead >> nSlot1;
// 			arRead >> nItemResist;
// 			arRead >> nResistAbilityOption;
// 
// 			LogItemInfo aLogItem;
// 			memset( &(aLogItem), 0, sizeof( aLogItem ) );
// 			//aLogItem.SendName = lpszPlayer1;
// 			//aLogItem.RecvName = lpszPlayer2;
// 			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), lpszPlayer1 );
// 			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), lpszPlayer2 );
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 			aLogItem.idSendPlayer = idSendPlayer;
// 			aLogItem.idRecvPlayer = idRecvPlayer;
// 			//////////////////////////////////////////////////////////////////////////
// 
// 			aLogItem.WorldId = dwWorldID;
// 			aLogItem.Gold = iSeed1;
// 			aLogItem.Gold2 = iSeed2;
// 			aLogItem.ItemNo		= ItemNo;
// 			//aLogItem.ItemName = lpszItemText;
// 			FLStrncpy( aLogItem.szItemName, _countof( aLogItem.szItemName ), lpszItemText, 30 );
// 
// 			aLogItem.itemNumber = nItemNum;
// 			aLogItem.nAbilityOption = nAbilityOption;
// 			aLogItem.nItemResist = nItemResist;
// 			aLogItem.nResistAbilityOption = nResistAbilityOption;
// 			aLogItem.Negudo = nNegudo;
// 			aLogItem.MaxNegudo	= nMaxNegudo;
// 			aLogItem.Gold_1 = dwGold_1;
// 			aLogItem.nSlot = nSlot;
// 			aLogItem.nSlot1 = nSlot1;
// 			//aLogItem.Action = chAction;
// 			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), chAction );
// 
// 			arRead >> aLogItem.m_bCharged;
// 			arRead >> aLogItem.m_dwKeepTime;
// 
// 			arRead >> aLogItem.nPiercedSize;
// 			for( size_t i=0; i<aLogItem.nPiercedSize; i++ )
// 				arRead >> aLogItem.adwItemId[i];
// 			arRead >> aLogItem.nUMPiercedSize;
// 			for( size_t i=0; i<aLogItem.nUMPiercedSize; i++ )
// 				arRead >> aLogItem.adwUMItemId[i];
// 
// 			arRead >> aLogItem.nPetKind;
// 			arRead >> aLogItem.nPetLevel;
// 			arRead >> aLogItem.dwPetExp;
// 			arRead >> aLogItem.wPetEnergy;
// 			arRead >> aLogItem.wPetLife;
// 			arRead >> aLogItem.nPetAL_D;
// 			arRead >> aLogItem.nPetAL_C;
// 			arRead >> aLogItem.nPetAL_B;
// 			arRead >> aLogItem.nPetAL_A;
// 			arRead >> aLogItem.nPetAL_S;
// 
// 			arRead >> aLogItem.dwCouplePlayerId;
// 			arRead >> aLogItem.nLevelDown;
// 			arRead >> aLogItem.wRandomOptionOriginId;
// 			arRead >> aLogItem.nRandomOptionExtensionSize;
// 			arRead >> aLogItem.wRandomOptionExtensionFlag;
// 
// 			for( size_t nIndex = 0; nIndex < MAX_RANDOMOPTION_SIZE; ++nIndex )
// 			{
// 				arRead >> aLogItem.awDstID[ nIndex ];
// 				arRead >> aLogItem.ashAdjValue[ nIndex ];
// 			}
// 
// 			DBQryNewItemLog( szQuery, _countof( szQuery ), aLogItem );
// 
// 			if ( FALSE == qry->Exec( szQuery ) )
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "LogItem %s, %s" ), lpszPlayer1, lpszPlayer2 );
// 				if( strlen( szQuery ) < 4096 )
// 					FLERROR_LOG( PROGRAM_NAME, _T( "LogItem Qyery => %s" ), szQuery );
// 				return;
// 			}
// 			break;
// 		}
// 	}
}

void CDbManager::LogPlayDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char		cPlay[MAX_PLAYER] = { 0, };					// 캐릭터 아이디
	int			iServer_No;							// 서버 번호
	DWORD		dWorld_No;							// 월드 번호
	D3DXVECTOR3	Pos;								// 좌표
	int			iLevel;								// 레벨
	int			iAttackDmg;							// 최대공격력
	int			iMaxHP;								// 최대 HP
	char		cMurder[64] = { 0, };				// 살인자 (몬스터나 캐릭, 트릭)
	u_long		uidPlayer;


	arRead.ReadString( cPlay, _countof( cPlay ) );
	iServer_No	= g_appInfo.dwSys;
	arRead >> dWorld_No;
	arRead >> Pos;
	arRead >> iLevel;
	arRead >> iAttackDmg;
	arRead >> iMaxHP;
	arRead.ReadString( cMurder, _countof( cMurder ) );
	arRead >> uidPlayer;

	
	char szQuery[MAX_QUERY_SIZE]	= { 0,};

	//	mulcom	BEGIN100407	LOG_STR L2 cMurder quotation mark 제거
	//DBQryLog( szQuery, "L2", uidPlayer, iServer_No, 0, iLevel, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'A', dWorld_No, cMurder, Pos.x, Pos.y, Pos.z, iAttackDmg, iMaxHP );
	CString	strMurderName	= cMurder;
	strMurderName.Replace( "'", " " );

	//	100805	CSTRING GETBUFFER	CAUTION ??????
// 	DBQryLog( szQuery, "L2", uidPlayer, iServer_No, 0, iLevel, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 'A', dWorld_No, strMurderName.GetBuffer(), Pos.x, Pos.y, Pos.z, iAttackDmg, iMaxHP );
	DBQryLog( szQuery, _countof( szQuery ), "L2", uidPlayer, iServer_No, 0, iLevel, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A', dWorld_No, (LPTSTR)( strMurderName.GetString() ), Pos.x, Pos.y, Pos.z, iAttackDmg, iMaxHP );
	//	mulcom	BEGIN100407	LOG_STR L2 cMurder quotation mark 제거

	/// 레벨을 추가해야함
	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%d" ), uidPlayer );
		return;
	}

//	qry->Clear();
}

void CDbManager::LogLevelUp(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char		cPlay[17] = {0,};					// 캐릭터 아이디
	UNREFERENCED_PARAMETER( cPlay );

	int			iServer_No;							// 서버 번호
	int			iLevel;								// 캐릭터 레벨
	int			iPlayJob;							// 직업_No
	
	EXPINTEGER	iExp1;								// Exp(경험치)
	int			iFlightLevel;						// 비행레벨
	char		cAction;							// 행동( 몬스터를 죽여서 렙업을 하여였는지, 퀘스트를 이용하여 렙업을 하였는지를 나타냄
	u_long		uidPlayer;							
	DWORD		dwGold;
	DWORD		dwPlayerTime;	

	iServer_No	= g_appInfo.dwSys;

	Stat _stat;

	arRead >> iExp1;
	arRead >> iFlightLevel;
	arRead >> iLevel;
	arRead >> iPlayJob;

	_stat.Serialize( arRead );
	
	arRead >> dwGold;
	arRead >> dwPlayerTime;
	
	int iAction;
	arRead >> iAction;
	arRead >> uidPlayer;
	switch( iAction )
	{
	case 1:					// 레벨 업
		cAction = 'L';
		break;
	case 2:					// 스탯 업
		cAction = 'S';
		break;
	case 3:					// 잡(직업)레벨 업
		cAction = 'J';
		break;
	case 4:					// 전직
		cAction = 'C';
		break;
	case 5:					// 경험치 렙 5이상 20%단위
		cAction = 'E';
		break;
	case 6:
		cAction = 'D';		// 경험치 다운시 렙따
		break;
	case 7:
		cAction = 'M';		// 경험치 다운
		break;
	case 8:
		cAction = '1';		// 로드라이트
		break;
	case 9:
		cAction = '2';		// 로드스타
		break;
	case 10:
		cAction = '3';		// 누군가가 부활을 해주었따
		break;		
	default:
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%c" ), iAction );
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 월드서버에서 고정값으로 보내던 데이터.
	const int	iJobLevel	= 1;							// 직업 레벨
	const int	iLP			= 0;							// LP(교육포인트)
	//////////////////////////////////////////////////////////////////////////

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryLog( szQuery, _countof( szQuery ), "L1", uidPlayer, iServer_No, iExp1, iLevel, iPlayJob, iJobLevel, iFlightLevel, _stat.GetOriginStr(), _stat.GetOriginDex(), _stat.GetOriginInt(), _stat.GetOriginSta(), _stat.GetRemainStatPoint(), iLP, cAction, 0, '\0', 0, 0, 0, dwGold, dwPlayerTime );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%d, %s" ), uidPlayer, szQuery );
		return;
	}
}

void CDbManager::LogServerDeath(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int			iServer_No;							// 서버 번호
	SYSTEMTIME	siStart;
		
	iServer_No	= g_appInfo.dwSys;
	arRead.Read( &siStart, sizeof(SYSTEMTIME) );

	if( g_pCommonCheck->CheckTime( siStart ) == false )
	{
		GetLocalTime( &siStart );
	}

	CTime tStart( siStart );
	CString strStart = tStart.Format( "%Y%m%d%H%M%S" );

	char szQuery[MAX_QUERY_SIZE]	= { 0,};

	//	100805	CSTRING GETBUFFER	CAUTION
// 	DBQryLog( szQuery, "L8", 0, iServer_No, 0, 0, 0, 0, 0,
// 		0, 0, 0, 0, 0, 0, 'A',
// 		0, '\0', 0.0f, 0.0f, 0.0f, 0, 0,
// 		0, 0, 0, 0, '\0', 0, 0,
// 		0, strStart.GetBuffer( strStart.GetLength() ) );
// 	strStart.ReleaseBuffer( strStart.GetLength() );
	DBQryLog( szQuery, _countof( szQuery ), "L8", 0, iServer_No, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A',
		0, '\0', 0.0f, 0.0f, 0.0f, 0, 0,
		0, 0, 0, 0, '\0', 0, 0,
		0, (LPTSTR)(strStart.GetString()) );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}
}

void	CDbManager::LogGetHonorTime(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int			iServer_No;							// 서버 번호
	u_long		uidPlayer;
	int			nGetHonor;
	
	iServer_No	= g_appInfo.dwSys;
	arRead >> nGetHonor;
	arRead >> uidPlayer;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"uspHonorLog @serverindex='%02d',@idPlayer='%07d',@nHonor='%d'",  g_appInfo.dwSys,uidPlayer, nGetHonor );
//	qry->Clear();
	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
//		qry->Clear();
		return;
	}

//	qry->Clear();
}

void CDbManager::LogUniqueItem(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int			iServer_No;							// 서버 번호
	char		cCrateMan[17] = {0,};				// 생성자
	int			iWorld;								// 월드번호
	D3DXVECTOR3	Pos;								// 좌표
	char		cUniqueName[51] = {0,};				// 아이템 명
	int			cItemAddLevel;						// 아이템 추가레벨
	u_long		uidPlayer;
	
	iServer_No	= g_appInfo.dwSys;
	arRead.ReadString( cCrateMan, _countof( cCrateMan ) );
	arRead >> iWorld;
	arRead >> Pos;
	arRead.ReadString( cUniqueName, _countof( cUniqueName ) );
	arRead >> cItemAddLevel;
	arRead >> uidPlayer;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryLog( szQuery, _countof( szQuery ), "L4", uidPlayer, iServer_No, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A',
		iWorld, '\0', Pos.x, Pos.y, Pos.z, 0, 0, '\0', 0, 0, 0, cUniqueName, 0, 0, cItemAddLevel );

	qry->Exec( szQuery );
}

void CDbManager::LogQuest(CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int Action;
	char cName[17] = {0,};
	int iServer_No;
	int nQuest;
	CTime		tQuest	= CTime::GetCurrentTime();	// 날짜
	u_long	uidPlayer;
	
	iServer_No	= g_appInfo.dwSys;
	CString strQuestTime	= tQuest.Format( "%Y%m%d%H%M%S" );

	arRead >> Action;
	arRead.ReadString( cName, _countof( cName ) );
	arRead >> nQuest;
	arRead >> uidPlayer;
	
	char szQuery[MAX_QUERY_SIZE]	= { 0,};

//	qry->Clear();
	if( Action == 1 ) // 퀘스트 시작
	{
		//	100805	CSTRING GETBUFFER	CAUTION
// 		DBQryLog( szQuery, "L6", uidPlayer, iServer_No, 0, 0, 0, 0, 0, 
// 			0, 0, 0, 0, 0, 0, 'C', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
// 			0, 0, 0, '\0', 0, 0, 0, strQuestTime.GetBuffer( strQuestTime.GetLength() ), 0, '\0', nQuest );
// 		strQuestTime.ReleaseBuffer( strQuestTime.GetLength() );
		DBQryLog( szQuery, _countof( szQuery ), "L6", uidPlayer, iServer_No, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 'C', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
			0, 0, 0, '\0', 0, 0, 0, (LPTSTR)( strQuestTime.GetString() ), 0, '\0', nQuest );

		if ( FALSE == qry->Exec( szQuery ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
			return;
		}

//		qry->Clear();
	}
	else	// 퀘스트 완료
	{
		//	100805	CSTRING GETBUFFER	CAUTION
// 		DBQryLog( szQuery, "L7", uidPlayer, iServer_No, 0, 0, 0, 0, 0, 
// 			0, 0, 0, 0, 0, 0, 'A', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
// 			0, 0, 0, '\0', 0, 0, 0, strQuestTime.GetBuffer( strQuestTime.GetLength() ), 0, '\0', nQuest );
// 		strQuestTime.ReleaseBuffer( strQuestTime.GetLength() );
		DBQryLog( szQuery, _countof( szQuery ), "L7", uidPlayer, iServer_No, 0, 0, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 'A', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
			0, 0, 0, '\0', 0, 0, 0, (LPTSTR)( strQuestTime.GetString() ), 0, '\0', nQuest );

		if ( FALSE == qry->Exec( szQuery ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
			return;
		}

//		qry->Clear();
	}
}

void CDbManager::DBQryLog( char* qryLog, size_t cchLog, char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1, int nLevel,
						  int nJob, int JobLv, int FlightLv, int nStr, int nDex,
						  int nInt, int nSta, int nRemainGP, int nRemainLP, char szState,
						  int WorldID, char* szkilled_szName, float vPos_x, float vPos_y, float vPos_z,
						  int nAttackPower, int nMax_HP, char *uGetidPlayer, DWORD dwGold, int nRemainGold,
						  int nItem_UniqueNo, char *szItem_Name, int Item_durability, int Item_count, int ItemAddLv,
						  char* szStart_Time, int TotalPlayerTime, char *szRemoteIP, int nQuest_Index, u_long uKill_idPlayer )
{
	CString strItem	= szItem_Name;
	strItem.Replace( '\'', ' ' );
	FLSPrintf( qryLog, cchLog, "LOG_STR '%s',@im_idPlayer='%07d',@iserverindex='%02d',@im_nExp1=%I64d,@im_nLevel=%d,"
		"@im_nJob=%d,@im_nJobLv=%d,@im_nFlightLv=%d,@im_nStr=%d,@im_nDex=%d,"
		"@im_nInt=%d,@im_nSta=%d,@im_nRemainGP=%d,@im_nRemainLP=%d,@iState='%c',"
		"@idwWorldID=%d,@ikilled_m_szName='%s',@im_vPos_x=%f,@im_vPos_y=%f,@im_vPos_z=%f,"
		"@iattackPower=%d,@imax_HP=%d,@im_GetszName='%s',@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@iItem_count=%d,@iItem_AddLv=%d,"
		"@iStart_Time='%s',@iTotalPlayTime=%d,@iremoteIP='%s',@iQuest_Index=%d,@ikilled_m_idPlayer='%07d'",
		Gu, idPlayer, nserverindex, nExp1, nLevel,
		nJob, JobLv, FlightLv, nStr, nDex,
		nInt, nSta, nRemainGP, nRemainLP, szState,
		WorldID, szkilled_szName, vPos_x, vPos_y, vPos_z,
		nAttackPower, nMax_HP, uGetidPlayer, dwGold, nRemainGold,
//		nItem_UniqueNo, szItem_Name, Item_durability, Item_count, ItemAddLv,
		nItem_UniqueNo, (LPCSTR)strItem, Item_durability, Item_count, ItemAddLv,
		szStart_Time, TotalPlayerTime, szRemoteIP, nQuest_Index, uKill_idPlayer );	
}

void CDbManager::Gamema_Chat( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char szGamema[1024];
	u_long uidPlayer;

	arRead >> uidPlayer;
	arRead.ReadString( szGamema, _countof( szGamema ) );
	
	CString strBuf = szGamema;
	strBuf.Replace( "--", "" );
	strBuf.Replace( "'", "" );
	strBuf.Replace( "/*", "" );
	strBuf.Replace( "*/", "" );

	// LOG_GAMEMASTER_STR '%s','%s','%s'
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"LOG_GAMEMASTER_STR @im_idPlayer='%07d',@iserverindex='%02d',@iszWords='%s'", uidPlayer, g_appInfo.dwSys, strBuf );

//	qry->Clear();
	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
//		qry->Clear();
		return;
	}

//	qry->Clear();
}

void CDbManager::LogConcurrentUserNumber( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	//	qry->Clear();
	
	int cbSize;
	int anCount[64] = { 0 };
	int nChannel;

	ar >> nChannel;
	ar >> cbSize;
	ar.Read( anCount, sizeof(int) * cbSize );
	//	LOG_USER_CNT_STR '00', 0, 0, 0, 0
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "LOG_USER_CNT_STR '%02d', %d", g_appInfo.dwSys, nChannel );
	
	char szCount[10];
	for( int i = 0; i < cbSize; i++ )
	{
		FLSPrintf( szCount,  _countof( szCount ), ", %d", anCount[i] );
		FLStrcat( szQuery, _countof( szQuery ), szCount );
	}
	
	qry->Exec( szQuery );
	
	//	qry->Clear();	
}

void CDbManager::LogPkPvp( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int nSendBuf = 0;
	u_long uidPlayer = NULL_PLAYER_ID;
	u_long uLoseidPlayer = NULL_PLAYER_ID;
	int iServer_No = -1;
	int iLoseServer_No = -1;
	int nLevel = 0;
	int nLoseLevel = 0;
	int nPoint = 0;
	int nPKValue = 0;
	int nLosePKValue = 0;
	DWORD dwPKPropensity = 0;
	DWORD dwLosePKPropensity = 0;
	int nFame = 0;
	int nLoseFame = 0;
	char chState = 0;
	D3DXVECTOR3 vPos, vLosePos;

	arRead >> nSendBuf;
	arRead >> nPoint;
	arRead >> chState;
	
	if( nSendBuf == 1 || nSendBuf == 3 )
	{
		arRead >> uLoseidPlayer;
		arRead >> iLoseServer_No;
		arRead >> nLoseLevel;
		arRead >> vLosePos;
		arRead >> nLoseFame;
		arRead >> nLosePKValue;
		arRead >> dwLosePKPropensity;
	}
	
	if( nSendBuf == 2 || nSendBuf == 3 )
	{
		arRead >> uidPlayer;
		arRead >> iServer_No;
		arRead >> nLevel;
		arRead >> vPos;
		arRead >> nFame;
		arRead >> nPKValue;
		arRead >> dwPKPropensity;
	}

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	if( nSendBuf == 3 )
	{
		DBQryLog( szQuery, _countof( szQuery ), "LA", uidPlayer, g_appInfo.dwSys, nLoseLevel, nLevel, nPoint, dwPKPropensity, nPKValue,
			nFame, dwLosePKPropensity, nLoseFame, 0, 0, 0, chState, 0, '\0', vPos.x, 0.0f, vPos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, uLoseidPlayer );
	}
	else if( nSendBuf == 1 )
	{
		DBQryLog( szQuery, _countof( szQuery ), "LA", 0, g_appInfo.dwSys, nLoseLevel, 0, nPoint, 0, nLosePKValue,
			0, dwLosePKPropensity, nLoseFame, 0, 0, 0, chState, 0, '\0', vLosePos.x, 0.0f, vLosePos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, uLoseidPlayer );
	}
	else
	{
		DBQryLog( szQuery, _countof( szQuery ), "LA", uidPlayer, g_appInfo.dwSys, 0, nLevel, nPoint, dwPKPropensity, nPKValue,
			nFame, 0, 0, 0, 0, 0, chState, 0, '\0', vPos.x, 0.0f, vPos.z, 0, 0,
			'\0', 0, 0, 0, '\0', 0, 0, 0, 
			'\0', 0, '\0', 0, 0 );
	}

	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), qry );
		return;
	}
}

void CDbManager::LogSchool( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long uidPlayer;
	char szName[128];
		
	arRead >> uidPlayer;
	arRead.ReadString( szName, _countof( szName ) );
	
	//LOG_EVENT_STR @im_idPlayer = '000001',@iserverindex = '00'
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "LOG_EVENT_STR @im_idPlayer = '%07d',@iserverindex = '%02d', @im_szName = '%s'", uidPlayer, g_appInfo.dwSys, szName );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), qry );
		return;
	}
			
//	qry->Clear();		
}

void CDbManager::LogSkillPoint( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long			uidPlayer;						// 플레이어 아이디
	int				nAction;						// 행동
	EXPINTEGER		nSkillExp = 0;						// 스킬경험치
	int				nSkillPoint;					// SP
	int				nSkillLevel = 0;					// 지금까지 올린 스킬레벨
	DWORD			dwSkillID;						// 스킬 아이디
	DWORD			dwLevel;						// 위스킬에 대한 레벨
	int				nPoint;							// 사용한 스킬 포인트
	
	arRead >> nAction;
	arRead >> uidPlayer;
	arRead >> nSkillLevel;
	arRead >> nSkillPoint;
	arRead >> dwSkillID;
	arRead >> dwLevel;
	arRead >> nPoint;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};

	if( nAction == LOG_SKILLPOINT_GET_QUEST )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingAcquireSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,10", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}
	else if( nAction == LOG_SKILLPOINT_GET_HUNT )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingAcquireSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,11", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}
	else if( nAction == LOG_SKILLPOINT_USE )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingUsingSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,20", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}
	else if( nAction == LOG_SKILLPOINT_GET_REWARD )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingAcquireSkillPoint '%02d','%07d',%d,%d,%d,%d,%d,%I64d,12", 
			g_appInfo.dwSys, uidPlayer, dwSkillID, dwLevel, nPoint, nSkillPoint, nSkillLevel, nSkillExp );
	}

	if ( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), pQuery );
		return;
	}
}

//void	CDbManager::DBQryNewItemLog( char* qryLog, size_t cchLog, const LogItemInfo & info )
BOOL	CDbManager::DBQryNewItemLog( CQuery & kQuery, const LogItemInfo & kLogItemInfo )
{
	const DWORD BASE_PARAMETER	= 63;

	const DWORD MAX_PARAMETER	= _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ? BASE_PARAMETER + MAX_ADD_ITEM_PARAMETER : BASE_PARAMETER;

	TCHAR szQuery[MAX_QUERY_SIZE] = { 0, };

	MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "LOG_ITEM_STR" ), MAX_PARAMETER );

	LogItemInfo kLog		= kLogItemInfo;

	kQuery.BindParameterVarChar( SQL_PARAM_INPUT, _countof( kLog.SendName ) - 1, kLog.SendName );

	TCHAR szServerIndex[3]	= { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	kQuery.BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );

	kQuery.BindParameterVarChar( SQL_PARAM_INPUT, _countof( kLog.RecvName ) - 1, kLog.RecvName );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.WorldId );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.Gold );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.Gold2 );
//	kQuery.BindParameterInt64( SQL_PARAM_INPUT, &kLog.kLogItem.dwSerialNumber );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwSerialNumber );
	kQuery.BindParameterVarChar( SQL_PARAM_INPUT, _countof( kLog.kLogItem.szItemName ) - 1, kLog.kLogItem.szItemName );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nHitPoint );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nAbilityOption );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.Gold_1 );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nQuantity );
	kQuery.BindParameterChar( SQL_PARAM_INPUT, _countof( kLog.Action ) - 1, kLog.Action );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.nSlot );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.nSlot1 );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nItemResist );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nResistAbilityOption );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.bCharged );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwKeepTime );
	kQuery.BindParameterInt64( SQL_PARAM_INPUT, &kLog.kLogItem.n64RandomOptID );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nGeneralPiercedSize );
	for( size_t Nth = 0; Nth < 5; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrGeneralPiercedItemID[ Nth ] );
	}
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nMaxHitPoint );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetKind );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetLevel );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwPetExp );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwPetEnergy );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwPetLife );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetAL_D );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetAL_C );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetAL_B );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetAL_A );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nPetAL_S );
	for( size_t Nth = 5; Nth < MAX_PIERCING; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrGeneralPiercedItemID[ Nth ] );
	}
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nUltimatePiercedSize );
	for( size_t Nth = 0; Nth < MAX_PIERCING_ULTIMATE; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrUltimatePiercedItemID[ Nth ] );
	}

	TCHAR szSendPlayerID[8]	= { 0, };
	FLSPrintf( szSendPlayerID, _countof( szSendPlayerID ), "%07d", kLog.idSendPlayer );
	kQuery.BindParameterChar( SQL_PARAM_INPUT, _countof( szSendPlayerID ) - 1, szSendPlayerID );

	TCHAR szRecvPlayerID[8]	= { 0, };
	FLSPrintf( szRecvPlayerID, _countof( szRecvPlayerID ), "%07d", kLog.idRecvPlayer );
	kQuery.BindParameterChar( SQL_PARAM_INPUT, _countof( szRecvPlayerID ) - 1, szRecvPlayerID );

	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwCouplePlayerID );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nLevelDown );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwRandomOptionOriginID );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nRandomOptionExtensionSize );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwRandomOptionExtensionFlag );
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrDstID[ Nth ] );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrAdjValue[ Nth ] );
	}

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nGuildID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nResistSMItemID );

		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwLooksChangeItemID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.bIsCombined );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwCombinedAddDamage );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.dwCombinedRandomOptionOriginID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nCombinedGeneralPiercingSize );
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrCombinedGeneralPiercingID[ Nth ] );
		}
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.nCombinedRandomOptionExtensionSize );
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrCombinedDstID[ Nth ] );
			kQuery.BindParameterInt( SQL_PARAM_INPUT, &kLog.kLogItem.arrCombinedAdjValue[ Nth ] );
		}
	}

	if( kQuery.Exec( szQuery ) == FALSE )
	{
		WriteLogItem( kLog );
		return FALSE;
	}

	return TRUE;


// 	FLSPrintf( qryLog, cchLog, "LOG_ITEM_STR "
// 		"@im_szName='%s',@iserverindex='%02d',@im_GetszName='%s',@idwWorldID=%d,@im_dwGold=%d,@im_nRemainGold=%d,"
// 		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@im_nAbilityOption=%d,@Im_GetdwGold=%d,"
// 		"@iItem_count=%d,@iState='%s',@im_nSlot0=%d,@im_nSlot1=%d,@im_bItemResist=%d,@im_nResistAbilityOption=%d"
// 		",@im_bCharged=%d,@im_dwKeepTime=%d,"
// 		"@im_nRandomOptItemId=%I64d"
// 		",@inPiercedSize=%d,@iadwItemId0=%d,@iadwItemId1=%d,@iadwItemId2=%d,@iadwItemId3=%d"
// 		",@iadwItemId4=%d"
// 		",@iMaxDurability=%d"
// 		",@inPetKind=%d,@inPetLevel=%d,@idwPetExp=%d,@iwPetEnergy=%d,@iwPetLife=%d"
// 		",@inPetAL_D=%d,@inPetAL_C=%d,@inPetAL_B=%d,@inPetAL_A=%d,@inPetAL_S=%d"
// 		",@iadwItemId5=%d,@iadwItemId6=%d,@iadwItemId7=%d,@iadwItemId8=%d,@iadwItemId9=%d"
// 		",@inUMPiercedSize=%d,@iadwUMItemId0=%d,@iadwUMItemId1=%d,@iadwUMItemId2=%d,@iadwUMItemId3=%d,@iadwUMItemId4=%d"
// 		",@im_idPlayer='%07d',@m_GetidPlayer=%d"
// 		",@idCouplePlayer=%d, @nLevelDown=%d, @nRandomOptionOriginId=%d"
// 		",@nRandomOptionExtSize=%d, @nRandomOptionExtFlag=%d"
// 		",@nRandomOptionExtDst1=%d, @nRandomOptionExtAdj1=%d"
// 		",@nRandomOptionExtDst2=%d, @nRandomOptionExtAdj2=%d"
// 		",@nRandomOptionExtDst3=%d, @nRandomOptionExtAdj3=%d"
// 		",@nRandomOptionExtDst4=%d, @nRandomOptionExtAdj4=%d"
// 		
// 		, info.SendName, g_appInfo.dwSys, info.RecvName, info.WorldId, info.Gold, info.Gold2
// 		, info.kLogItem.dwSerialNumber, info.kLogItem.szItemName, info.kLogItem.nHitPoint, info.kLogItem.nAbilityOption, info.Gold_1
// 		, info.kLogItem.nQuantity, info.Action, info.nSlot, info.nSlot1, info.kLogItem.byItemResist, info.kLogItem.nResistAbilityOption
// 		, info.kLogItem.bCharged, info.kLogItem.dwKeepTime, static_cast<__int64>( 0 )
// 		, info.kLogItem.nGeneralPiercedSize
// 		, info.kLogItem.arrGeneralPiercedItemID[0]
// 		, info.kLogItem.arrGeneralPiercedItemID[1]
// 		, info.kLogItem.arrGeneralPiercedItemID[2]
// 		, info.kLogItem.arrGeneralPiercedItemID[3]
// 		, info.kLogItem.arrGeneralPiercedItemID[4]
// 		, info.kLogItem.nMaxHitPoint
// 		, info.kLogItem.byPetKind, info.kLogItem.byPetLevel, info.kLogItem.dwPetExp, info.kLogItem.wPetEnergy, info.kLogItem.wPetLife
// 		, info.kLogItem.nPetAL_D, info.kLogItem.nPetAL_C, info.kLogItem.nPetAL_B, info.kLogItem.nPetAL_A, info.kLogItem.nPetAL_S
// 		, info.kLogItem.arrGeneralPiercedItemID[5]
// 		, info.kLogItem.arrGeneralPiercedItemID[6]
// 		, info.kLogItem.arrGeneralPiercedItemID[7]
// 		, info.kLogItem.arrGeneralPiercedItemID[8]
// 		, info.kLogItem.arrGeneralPiercedItemID[9]
// 		, info.kLogItem.nUltimatePiercedSize
// 		, info.kLogItem.arrUltimatePiercedItemID[0]
// 		, info.kLogItem.arrUltimatePiercedItemID[1]
// 		, info.kLogItem.arrUltimatePiercedItemID[2]
// 		, info.kLogItem.arrUltimatePiercedItemID[3]
// 		, info.kLogItem.arrUltimatePiercedItemID[4]
// 		, info.idSendPlayer, info.idRecvPlayer
// 		, info.kLogItem.dwCouplePlayerID, info.kLogItem.nLevelDown, info.kLogItem.wRandomOptionOriginID
// 		, info.kLogItem.nRandomOptionExtensionSize, info.kLogItem.wRandomOptionExtensionFlag
// 		, info.kLogItem.arrDstID[0], info.kLogItem.arrAdjValue[0]
// 		, info.kLogItem.arrDstID[1], info.kLogItem.arrAdjValue[1]
// 		, info.kLogItem.arrDstID[2], info.kLogItem.arrAdjValue[2]
// 		, info.kLogItem.arrDstID[3], info.kLogItem.arrAdjValue[3]
// 		);
}

// BOOL CDbManager::call_uspLoggingTrade( CQuery* pQuery, const T_TRADE_ITEM_LOG & kTradeItemLog )
// {
// 	char pszQuery[1024]	= { 0, };
// 	switch( kTradeItemLog.nFlag )
// 	{
// 		case 0:
// 			{
// 				FLSPrintf( pszQuery, _countof( pszQuery ), "uspLoggingTrade @pFlag=0, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d",
// 					kTradeItemLog.nTradeID, g_appInfo.dwSys, kTradeItemLog.dwWorldID );
// 				break;
// 			}
// 		case 1:
// 			{
// 				FLSPrintf( pszQuery, _countof( pszQuery ), "uspLoggingTrade @pFlag=1, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d, @pidPlayer='%07d', @pTradeGold=%d, @pPlayerIP='%s', @pLevel=%d, @pJob=%d",
// 					kTradeItemLog.nTradeID, g_appInfo.dwSys, kTradeItemLog.dwWorldID, kTradeItemLog.idPlayer, kTradeItemLog.dwTradeGold
// 					, kTradeItemLog.szAddress, kTradeItemLog.nLevel, kTradeItemLog.nJob );
// 				break;
// 			}
// 		case 2:
// 			{
// 				FLSPrintf( pszQuery, _countof( pszQuery ), "uspLoggingTrade @pFlag=2, @pTradeID=%d, @pserverindex='%02d', @pidPlayer='%07d'"
// 					", @pItemIndex='%d', @pItemSerialNum=%d, @pItemCnt=%d, @pAbilityOpt=%d, @pItemResist=%d, @pResistAbilityOpt=%d, @pRandomOpt=%I64d"
// 					", @idCouplePlayer=%d, @nLevelDown=%d, @nRandomOptionOriginId=%d"
// 					", @nRandomOptionExtSize=%d, @nRandomOptionExtFlag=%d"
// 					", @nRandomOptionExtDst1=%d, @nRandomOptionExtAdj1=%d"
// 					", @nRandomOptionExtDst2=%d, @nRandomOptionExtAdj2=%d"
// 					", @nRandomOptionExtDst3=%d, @nRandomOptionExtAdj3=%d"
// 					", @nRandomOptionExtDst4=%d, @nRandomOptionExtAdj4=%d"
// 					, kTradeItemLog.nTradeID, g_appInfo.dwSys, kTradeItemLog.idPlayer
// 					, kTradeItemLog.kLogItem.dwItemID, kTradeItemLog.kLogItem.dwSerialNumber
// 					, kTradeItemLog.kLogItem.nQuantity, kTradeItemLog.kLogItem.nAbilityOption
// 					, kTradeItemLog.kLogItem.byItemResist, kTradeItemLog.kLogItem.nResistAbilityOption
// 					, static_cast<__int64>( 0 )
// 					, kTradeItemLog.kLogItem.dwCouplePlayerID, kTradeItemLog.kLogItem.nLevelDown, kTradeItemLog.kLogItem.wRandomOptionOriginID
// 					, kTradeItemLog.kLogItem.nRandomOptionExtensionSize, kTradeItemLog.kLogItem.wRandomOptionExtensionFlag
// 					, kTradeItemLog.kLogItem.arrDstID[0], kTradeItemLog.kLogItem.arrAdjValue[0]
// 					, kTradeItemLog.kLogItem.arrDstID[1], kTradeItemLog.kLogItem.arrAdjValue[1]
// 					, kTradeItemLog.kLogItem.arrDstID[2], kTradeItemLog.kLogItem.arrAdjValue[2]
// 					, kTradeItemLog.kLogItem.arrDstID[3], kTradeItemLog.kLogItem.arrAdjValue[3] );
// 				break;
// 			}
// 	}
// 
// 	if ( FALSE == pQuery->Exec( pszQuery ) )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), pszQuery );
// 		return FALSE;
// 	}
// 
// 	return TRUE;
// }

void	CDbManager::WriteLogItem( const LogItemInfo & kLogItemInfo )
{
	TCHAR szLog[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szLog, _countof( szLog ), "LOG_ITEM_STR @im_szName = '%s'", kLogItemInfo.SendName );
	FLSPrintf( szLog, _countof( szLog ), "%s, serverindex = '%02d'", szLog, g_appInfo.dwSys );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_GetszName = '%s'", szLog, kLogItemInfo.RecvName );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idwWorldID = %d", szLog, kLogItemInfo.WorldId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_dwGold = %d", szLog, kLogItemInfo.Gold );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_nRemainGold = %d", szLog, kLogItemInfo.Gold2 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iItem_UniqueNo = %d", szLog, kLogItemInfo.kLogItem.dwSerialNumber );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iItem_Name = '%s'", szLog, kLogItemInfo.kLogItem.szItemName );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iItem_durability = %d", szLog, kLogItemInfo.kLogItem.nHitPoint );

	FLSPrintf( szLog, _countof( szLog ), "%s, @im_nAbilityOption = %d", szLog, kLogItemInfo.kLogItem.nAbilityOption );
	FLSPrintf( szLog, _countof( szLog ), "%s, @Im_GetdwGold = %d", szLog, kLogItemInfo.Gold_1 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iItem_count = %d", szLog, kLogItemInfo.kLogItem.nQuantity );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iState = '%s'", szLog, kLogItemInfo.Action );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_nSlot0 = %d", szLog, kLogItemInfo.nSlot );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_nSlot1 = %d", szLog, kLogItemInfo.nSlot1 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_bItemResist = %d", szLog, kLogItemInfo.kLogItem.nItemResist );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_nResistAbilityOption = %d", szLog, kLogItemInfo.kLogItem.nResistAbilityOption );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_bCharged = %d", szLog, kLogItemInfo.kLogItem.bCharged );
	FLSPrintf( szLog, _countof( szLog ), "%s, @im_dwKeepTime = %d", szLog, kLogItemInfo.kLogItem.dwKeepTime );

	FLSPrintf( szLog, _countof( szLog ), "%s, @im_nRandomOptItemId = %I64d", szLog, kLogItemInfo.kLogItem.n64RandomOptID );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPiercedSize = %d", szLog, kLogItemInfo.kLogItem.nGeneralPiercedSize );
	for( size_t Nth = 0; Nth < 5; ++Nth )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @iadwItemId%d = %d", szLog, Nth, kLogItemInfo.kLogItem.arrGeneralPiercedItemID[ Nth ] );
	}

	FLSPrintf( szLog, _countof( szLog ), "%s, @iMaxDurability = %d", szLog, kLogItemInfo.kLogItem.nMaxHitPoint );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetKind = %d", szLog, kLogItemInfo.kLogItem.nPetKind );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetLevel = %d", szLog, kLogItemInfo.kLogItem.nPetLevel );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idwPetExp = %d", szLog, kLogItemInfo.kLogItem.dwPetExp );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iwPetEnergy = %d", szLog, kLogItemInfo.kLogItem.dwPetEnergy );
	FLSPrintf( szLog, _countof( szLog ), "%s, @iwPetLife = %d", szLog, kLogItemInfo.kLogItem.dwPetLife );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetAL_D = %d", szLog, kLogItemInfo.kLogItem.nPetAL_D );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetAL_C = %d", szLog, kLogItemInfo.kLogItem.nPetAL_C );

	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetAL_B = %d", szLog, kLogItemInfo.kLogItem.nPetAL_B );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetAL_A = %d", szLog, kLogItemInfo.kLogItem.nPetAL_A );
	FLSPrintf( szLog, _countof( szLog ), "%s, @inPetAL_S = %d", szLog, kLogItemInfo.kLogItem.nPetAL_S );
	for( size_t Nth = 5; Nth < MAX_PIERCING; ++Nth )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @iadwItemId%d = %d", szLog, Nth, kLogItemInfo.kLogItem.arrGeneralPiercedItemID[ Nth ] );
	}
	FLSPrintf( szLog, _countof( szLog ), "%s, @inUMPiercedSize = %d", szLog, kLogItemInfo.kLogItem.nUltimatePiercedSize );
	for( size_t Nth = 0; Nth < MAX_PIERCING_ULTIMATE; ++Nth )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @iadwUMItemId%d = %d", szLog, Nth, kLogItemInfo.kLogItem.arrUltimatePiercedItemID[ Nth ] );
	}

	FLSPrintf( szLog, _countof( szLog ), "%s, @im_idPlayer = '%07d'", szLog, kLogItemInfo.idSendPlayer );
	FLSPrintf( szLog, _countof( szLog ), "%s, @m_GetidPlayer = '%07d'", szLog, kLogItemInfo.idRecvPlayer );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idCouplePlayer = %d", szLog, kLogItemInfo.kLogItem.dwCouplePlayerID );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nLevelDown = %d", szLog, kLogItemInfo.kLogItem.nLevelDown );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionOriginId = %d", szLog, kLogItemInfo.kLogItem.dwRandomOptionOriginID );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtSize = %d", szLog, kLogItemInfo.kLogItem.nRandomOptionExtensionSize );

	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtFlag = %d", szLog, kLogItemInfo.kLogItem.dwRandomOptionExtensionFlag );
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtDst%d = %d", szLog, Nth + 1, kLogItemInfo.kLogItem.arrDstID[ Nth ] );
		FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtAdj%d = %d", szLog, Nth + 1, kLogItemInfo.kLogItem.arrAdjValue[ Nth ] );
	}

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @idGuild = %d", szLog, kLogItemInfo.kLogItem.nGuildID );
		FLSPrintf( szLog, _countof( szLog ), "%s, @nResistSMItemId = %d", szLog, kLogItemInfo.kLogItem.nResistSMItemID );

		FLSPrintf( szLog, _countof( szLog ), "%s, @dwLooksChangeItemID = %d", szLog, kLogItemInfo.kLogItem.dwLooksChangeItemID );
		FLSPrintf( szLog, _countof( szLog ), "%s, @bIsCombined = %d", szLog, kLogItemInfo.kLogItem.bIsCombined );
		FLSPrintf( szLog, _countof( szLog ), "%s, @dwCombinedAddDamage = %d", szLog, kLogItemInfo.kLogItem.dwCombinedAddDamage );
		FLSPrintf( szLog, _countof( szLog ), "%s, @wCombinedRandomOptionOriginID = %d", szLog, kLogItemInfo.kLogItem.dwCombinedRandomOptionOriginID );
		FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedGeneralPiercingSize = %d", szLog, kLogItemInfo.kLogItem.nCombinedGeneralPiercingSize );
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			FLSPrintf( szLog, _countof( szLog ), "%s, @dwCombinedGeneralPiercingID%d = %d", szLog, Nth + 1, kLogItemInfo.kLogItem.arrCombinedGeneralPiercingID[ Nth ] );
		}
		FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedRandomOptionExtensionSize = %d", szLog, kLogItemInfo.kLogItem.nCombinedRandomOptionExtensionSize );
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedRandomOptionExtDst%d = %d", szLog, Nth + 1, kLogItemInfo.kLogItem.arrCombinedDstID[ Nth ] );
			FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedRandomOptionExtAdj%d = %d", szLog, Nth + 1, kLogItemInfo.kLogItem.arrCombinedAdjValue[ Nth ] );
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szLog );
}

void	CDbManager::call_uspLoggingTrade( CQuery & kQuery, const int nTradeID, const DWORD dwWorldID )
{
	TCHAR szQuery[MAX_QUERY_SIZE] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingTrade @pFlag=0, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d",
		nTradeID, g_appInfo.dwSys, dwWorldID );

	if( kQuery.Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szQuery );
	}
}

void	CDbManager::call_uspLoggingTradePlayer( CQuery & kQuery, const int nTradeID, const T_LOG_TRADE & kLogTrade )
{
	TCHAR szQuery[MAX_QUERY_SIZE] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingTrade @pFlag=1, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d, @pidPlayer='%07d', @pTradeGold=%d, @pPlayerIP='%s', @pLevel=%d, @pJob=%d",
		nTradeID, g_appInfo.dwSys, kLogTrade.dwWorldID, kLogTrade.idPlayerID, kLogTrade.nTradeGold
		, kLogTrade.szIP, kLogTrade.nPlayerLevel, kLogTrade.nPlayerJob );

	if( kQuery.Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szQuery );
	}
}

void	CDbManager::call_uspLoggingTradeItem( CQuery & kQuery, const int nTradeID, const u_long idPlayerID, const T_LOG_ITEM & kLogItem )
{
	TCHAR szQuery[MAX_QUERY_SIZE] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingTrade @pFlag=2, @pTradeID=%d, @pserverindex='%02d', @pidPlayer='%07d'"
		", @pItemIndex='%d', @pItemSerialNum=%d, @pItemCnt=%d, @pAbilityOpt=%d, @pItemResist=%d, @pResistAbilityOpt=%d, @pRandomOpt=%I64d"
		", @idCouplePlayer=%d, @nLevelDown=%d, @nRandomOptionOriginId=%d"
		", @nRandomOptionExtSize=%d, @nRandomOptionExtFlag=%d"
		", @nRandomOptionExtDst1=%d, @nRandomOptionExtAdj1=%d"
		", @nRandomOptionExtDst2=%d, @nRandomOptionExtAdj2=%d"
		", @nRandomOptionExtDst3=%d, @nRandomOptionExtAdj3=%d"
		", @nRandomOptionExtDst4=%d, @nRandomOptionExtAdj4=%d"

		, nTradeID, g_appInfo.dwSys, idPlayerID, kLogItem.dwItemID, kLogItem.dwSerialNumber
		, kLogItem.nQuantity, kLogItem.nAbilityOption, kLogItem.nItemResist, kLogItem.nResistAbilityOption, static_cast<__int64>( 0 )
		, kLogItem.dwCouplePlayerID, kLogItem.nLevelDown, kLogItem.dwRandomOptionOriginID
		, kLogItem.nRandomOptionExtensionSize, kLogItem.dwRandomOptionExtensionFlag
		, kLogItem.arrDstID[0], kLogItem.arrAdjValue[0]
		, kLogItem.arrDstID[1], kLogItem.arrAdjValue[1]
		, kLogItem.arrDstID[2], kLogItem.arrAdjValue[2]
		, kLogItem.arrDstID[3], kLogItem.arrAdjValue[3] );

	if( kQuery.Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szQuery );
	}
}

void CDbManager::LogExpBox( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer;
	OBJID objid;
	EXPINTEGER iExp;
	BOOL bGet;

	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> objid >> iExp >> bGet;
	char pszQuery[1024]	= { 0, };

	FLSPrintf( pszQuery, _countof( pszQuery ), "uspLogExpBox @pserverindex='%02d', @pidPlayer='%07d', @pobjid=%d, @piExp=%I64d, @pState='%c'",
		g_appInfo.dwSys, idPlayer, objid, iExp, ( bGet? 'G': 'D' ) );

	if ( FALSE == pQuery->Exec( pszQuery ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), pszQuery );
//	FLTRACE_LOG( PROGRAM_NAME, pszQuery );
}

void CDbManager::LogInstanceDungeon( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	DWORD dwDungeonId, dwWorldId;
	u_long uChannel;
	int	nDungeonType;
	char chState;

	arRead >> dwDungeonId >> dwWorldId >> uChannel >> nDungeonType >> chState;
	
	pQuery->Execute( "usp_Log_Ins_Dungeon_Insert @serverindex = '%02d', @m_DungeonID = '%07d', @m_WorldID = %d, @m_channel = %d, @m_Type = %d, @m_State = '%c'", 
	g_appInfo.dwSys, dwDungeonId, dwWorldId, uChannel, nDungeonType, chState );
}

void CDbManager::LogError( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idPlayer, idChannel;
	char szAccount[MAX_ACCOUNT] = {0, };
	char chType;
	char szError[1024] = {0, };

	arRead >> idPlayer;
	arRead.ReadString( szAccount, _countof( szAccount ) );
	arRead >> idChannel >> chType;
	arRead.ReadString( szError, _countof( szError ) );
	
	pQuery->Execute( "usp_SystemError_Insert @m_idPlayer = '%07d', @serverindex = '%02d', @account = '%s', @nChannel = %d, @chType = '%c', @szError = '%s'", 
	idPlayer, g_appInfo.dwSys, szAccount, idChannel, chType, szError );
}

void CDbManager::LogGuildFurniture( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	DWORD dwGuildId;
	GH_Fntr_Info gfi;
	char chState;

	arRead >> dwGuildId;
	gfi.Serialize( arRead );
	arRead >> chState;

	pQuery->Execute( "usp_GuildFurniture_Log @serverindex = '%02d', @m_idGuild = '%06d', @SEQ = %d, @ItemIndex = %d, @tKeepTime = %d, @bSetup = %d, @nState = '%c'", 
		g_appInfo.dwSys, dwGuildId, gfi.nSeqNum, gfi.dwItemId, gfi.tKeepTime, static_cast<int>( gfi.bSetup ), chState );
}
