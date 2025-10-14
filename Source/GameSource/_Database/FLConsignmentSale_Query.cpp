#include "StdAfx.h"

#include "FLConsignmentSale_Query.h"

#include "../DatabaseServer/dptrans.h"
#include "FLDBConsignmentSale.h"
#include "FLItemGoldLog_Query.h"

#include "../_Common/FLDBCommonFuntion.h"
#include "../_CommonDefine/FLCommonCount.h"
#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"

#include "FLDataBase.h"

extern	APP_INFO			g_appInfo;


namespace nsConsignmentSale {


bool FLQuery_Regist::Execute( CQuery & query )
{
	CAr ar( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );

	FLPacketWDB_RegistReq req;
	req.Deserialize( ar );

	//////////////////////////////////////////////////////////////////////////
	TCHAR szPlayerId[8]		= { 0, };
	TCHAR szServerIndex[3]	= { 0, };
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	
	const DWORD MAX_PARAMETER		= _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ? 
									MAX_ITEM_PARAMETER + 12 + MAX_ADD_ITEM_PARAMETER : MAX_ITEM_PARAMETER + 12;

	TCHAR szQuery[MAX_QUERY_SIZE]	= { 0, };
	CDbManager::MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "uspConsignmentSale_Insert" ), MAX_PARAMETER );

	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_nPrice );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_nOnePrice );
	query.BindParameterVarChar( SQL_PARAM_INPUT,	64, req.m_szItemName );
	query.BindParameterVarChar( SQL_PARAM_INPUT, 32,	req.m_szPlayerName );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_dwItemType1 );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_dwItemType2 );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_dwGrade );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_nLimitLevel );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_uRemainHour );
	query.BindParameterInt( SQL_PARAM_INPUT, &req.m_eRegistMode );
	query.BindParameterChar( SQL_PARAM_INPUT, _countof( szPlayerId ) - 1, szPlayerId );
	query.BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );

	CDbManager::T_ITEM_QUERY_INFO kItem;
	kItem.Copy( req.m_kItemElem );

	CDbManager::SaveOneItemCommon( query, kItem );

	query.BindParameterChar( SQL_PARAM_INPUT, ( _countof( kItem.szState ) - 1 ), kItem.szState );
	//////////////////////////////////////////////////////////////////////////


// 	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
// 	FLSPrintf( szQuery, _countof( szQuery ), " uspConsignmentSale_Insert "
// 		"@nPrice=%d"
// 		",@nOnePrice=%d"
// 		",@szRealItemName='%s'"
// 		",@szPlayerName='%s'"
// 		",@dwItemType1=%u"
// 		",@dwItemType2=%u"
// 		",@dwGrade=%u"
// 		",@dwLimitLevel=%u"
// 		",@nRmainHour=%d"
// 		",@eRegistMode=%d"
// 
// 		",@m_idPlayer='%07d'"
// 		",@serverindex='%02d'"
// 		",@dwItemId=%d"
// 		",@dwInvenIndex=%d"
// 		",@nItemNum=%d"
// 		",@nUniqueNum=%d"
// 		",@nAbilityOption=%d"
// 		",@nItemResist=%d"
// 		",@nResistAbilityOption=%d"
// 		",@dwFlag=%d"
// 		",@bCharged=%d"
// 		",@dwKeepTime=%d"
// 		",@nRandomOptItemId=%I64d"
// 		",@nHitPoint=%d"
// 		",@nRepairNumber=%d"
// 		",@nRepair=%d"
// 		",@nIsTransformVisPet=%d"
// 		",@nUltimatePiercingSize=%d"
// 		",@dwUltimatePiercingId1=%d"
// 		",@dwUltimatePiercingId2=%d"
// 		",@dwUltimatePiercingId3=%d"
// 		",@dwUltimatePiercingId4=%d"
// 		",@dwUltimatePiercingId5=%d"
// 		",@nGeneralPiercingSize=%d"
// 		",@dwGeneralPiercingId1=%d"
// 		",@dwGeneralPiercingId2=%d"
// 		",@dwGeneralPiercingId3=%d"
// 		",@dwGeneralPiercingId4=%d"
// 		",@dwGeneralPiercingId5=%d"
// 		",@dwGeneralPiercingId6=%d"
// 		",@dwGeneralPiercingId7=%d"
// 		",@dwGeneralPiercingId8=%d"
// 		",@dwGeneralPiercingId9=%d"
// 		",@dwGeneralPiercingId10=%d"
// 		",@dwGeneralPiercingKeepTime1=%d"
// 		",@dwGeneralPiercingKeepTime2=%d"
// 		",@dwGeneralPiercingKeepTime3=%d"
// 		",@dwGeneralPiercingKeepTime4=%d"
// 		",@dwGeneralPiercingKeepTime5=%d"
// 		",@dwGeneralPiercingKeepTime6=%d"
// 		",@dwGeneralPiercingKeepTime7=%d"
// 		",@dwGeneralPiercingKeepTime8=%d"
// 		",@dwGeneralPiercingKeepTime9=%d"
// 		",@dwGeneralPiercingKeepTime10=%d"
// 		",@nIsRarePet=%d"
// 		",@nRarePetKind=%d"
// 		",@nRarePetLevel=%d"
// 		",@nRarePetExp=%d"
// 		",@nRarePetEnergy=%d"
// 		",@nRarePetLife=%d"
// 		",@nRarePetAvailLevel1=%d"
// 		",@nRarePetAvailLevel2=%d"
// 		",@nRarePetAvailLevel3=%d"
// 		",@nRarePetAvailLevel4=%d"
// 		",@nRarePetAvailLevel5=%d"
// 		",@szRarePetName='%s'"
// 		",@nSerialNumberStatus=%d"
// 		",@nSerialNumberId=%d"
// 		",@idCouplePlayer=%d"
// 		",@nLevelDown=%d"
// 		",@nRandomOptionOriginId=%d"
// 		",@nRandomOptionExtSize=%d"
// 		",@nRandomOptionExtFlag=%d"
// 		",@nRandomOptionExtDst1=%d"
// 		",@nRandomOptionExtAdj1=%d"
// 		",@nRandomOptionExtDst2=%d"
// 		",@nRandomOptionExtAdj2=%d"
// 		",@nRandomOptionExtDst3=%d"
// 		",@nRandomOptionExtAdj3=%d"
// 		",@nRandomOptionExtDst4=%d"
// 		",@nRandomOptionExtAdj4=%d "
// 
// 
// 		, req.m_nPrice
// 		, req.m_nOnePrice
// 		, req.m_szItemName
// 		, req.m_szPlayerName
// 		, req.m_dwItemType1
// 		, req.m_dwItemType2
// 		, req.m_dwGrade
// 		, req.m_nLimitLevel
// 		, req.m_uRemainHour
// 		, req.m_eRegistMode
// 
// 		, req.m_uPlayerID
// 		, g_appInfo.dwSys
// 		, req.m_kItemElem.m_dwItemId
// 		, req.m_kItemElem.m_dwObjIndex
// 		, req.m_kItemElem.m_nItemNum
// 		, req.m_kItemElem.GetSerialNumber()
// 		, req.m_kItemElem.GetAbilityOption()
// 		, req.m_kItemElem.m_bItemResist
// 		, req.m_kItemElem.m_nResistAbilityOption
// 		, req.m_kItemElem.m_byFlag
// 		, req.m_kItemElem.m_bCharged
// 		, req.m_kItemElem.m_dwKeepTime
// 		, static_cast<__int64>( 0 ) // pItemElem->GetRandomOptItemId()
// 		, req.m_kItemElem.m_nHitPoint
// 		, req.m_kItemElem.m_nRepairNumber
// 		, req.m_kItemElem.m_nRepair
// 		, req.m_kItemElem.IsTransformVisPet()
// 		, req.m_kItemElem.GetUltimatePiercingSize()
// 		, req.m_kItemElem.GetUltimatePiercingItemID( 0 )
// 		, req.m_kItemElem.GetUltimatePiercingItemID( 1 )
// 		, req.m_kItemElem.GetUltimatePiercingItemID( 2 )
// 		, req.m_kItemElem.GetUltimatePiercingItemID( 3 )
// 		, req.m_kItemElem.GetUltimatePiercingItemID( 4 )
// 		, req.m_kItemElem.GetGeneralPiercingSize()
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 0 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 1 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 2 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 3 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 4 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 5 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 6 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 7 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 8 )
// 		, req.m_kItemElem.GetGeneralPiercingItemID( 9 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 0 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 1 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 2 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 3 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 4 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 5 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 6 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 7 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 8 )
// 		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 9 )
// 		, ( req.m_kItemElem.m_pPet ? 1 : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetKind() : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetLevel() : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetExp() : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetEnergy() : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetLife() : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_D ) : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_C ) : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_B ) : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_A ) : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_S ) : 0 )
// 		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetName() : "" )
// 		, 0 //SaveSerialNumberStatus( pMover->m_Inventory.GetAllocedSerialNumber() )
// 		, 0 // Managed Serial Number Id
// 		, req.m_kItemElem.GetCoupleId()
// 		, req.m_kItemElem.GetLevelDown()
// 		, req.m_kItemElem.GetRandomOptionOriginID()
// 		, req.m_kItemElem.GetRandomOptionExtensionSize()
// 		, req.m_kItemElem.GetRandomOptionExtensionFlag()
// 		, req.m_kItemElem.GetRandomOptionExtensionDstID( 0 )
// 		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 0 )
// 		, req.m_kItemElem.GetRandomOptionExtensionDstID( 1 )
// 		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 1 )
// 		, req.m_kItemElem.GetRandomOptionExtensionDstID( 2 )
// 		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 2 )
// 		, req.m_kItemElem.GetRandomOptionExtensionDstID( 3 )
// 		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 3 )
// 		);

	FLPacketWDB_RegistAck ack;
	ack.m_uPlayerID				= req.m_uPlayerID;

	if( query.PrepareQuery( szQuery ) == FALSE )
	{
		ack.m_eResult				= FLPacketWDB_RegistAck::E_FAILURE;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );	
		return false;
	}


	if( query.Exec( szQuery ) == FALSE )
	{ 
		ack.m_eResult				= FLPacketWDB_RegistAck::E_FAILURE;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	if( query.Fetch() == FALSE )		// NO_DATA
	{ 
		ack.m_eResult				= FLPacketWDB_RegistAck::E_FAILURE;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	const int nRet				= query.GetInt( "nRet" );
	if( nRet != 0 )
	{
		ack.m_eResult				= FLPacketWDB_RegistAck::E_FAILURE;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ret(%d) (%s) ]" ), nRet, szQuery ); 
		return true;
	}

	//ItemLog
	{
		LogItemInfo info;
		MakeItemLogInfo( info, "s", req.m_szPlayerName, "ConsignmentSale RegistEnd"
			, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, req.m_kItemElem, req.m_kItemElem.m_nItemNum, 0, 0 );
		LOG_DB().PostQueryCommand( info.idSendPlayer, new FLItemGold_LogQuery( info ) );
	}

	// send to client

	ack.m_eResult				= FLPacketWDB_RegistAck::E_SUCCESS;
	CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );

	// add info
	FLDBConsignmentSale::FLSaleInfo kSaleInfo;
	kSaleInfo.u64ItemInstanceSRL	= query.GetInt64( "u64ItemInstanceSRL" );
	kSaleInfo.uPlayerID				= req.m_uPlayerID;
			  
	kSaleInfo.nPrice				= req.m_nPrice;
	kSaleInfo.nOnePrice				= req.m_nOnePrice;
	FLStrcpy( kSaleInfo.szItemName, _countof( kSaleInfo.szItemName ), req.m_szItemName );
	FLStrcpy( kSaleInfo.szPlayerName, _countof( kSaleInfo.szPlayerName ), req.m_szPlayerName );
	kSaleInfo.dwItemType1			= req.m_dwItemType1;
	kSaleInfo.dwItemType2			= req.m_dwItemType2;
	kSaleInfo.dwGrade				= req.m_dwGrade;
	kSaleInfo.nLimitLevel			= req.m_nLimitLevel;
	kSaleInfo.nAbilityOption		= req.m_kItemElem.GetAbilityOption();
	kSaleInfo.eRegistMode			= req.m_eRegistMode;		


	//만료 시간
	//const FLTm tmCurr				= DATE_TIMER().GetCurrentTime();
	//const long nCurrSec				= FLTimeToSecond( tmCurr.year, tmCurr.hour, tmCurr.minute, tmCurr.second );
	const long nPeriodSec			= req.m_uRemainHour * FLTm::ONE_HOUR_SECOND;
	kSaleInfo.tmExpiredDate			= DATE_TIMER().GetCurrentTime();
	if( FLAddDateTime( kSaleInfo.tmExpiredDate, nPeriodSec ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATE currDate(%s), add second(%d) ]" ), FLTmToString( DATE_TIMER().GetCurrentTime(), FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), nPeriodSec );
	}
			  
	kSaleInfo.kItemElem				= req.m_kItemElem;

	DB_CONSIGNMENT_SALE()._AddItem_AndSendRegistedCount( m_kDBOverlappedPlus.dpid, kSaleInfo );
	return true;
}

bool FLQuery_CancelRegisted::Execute( CQuery & query )
{
	CAr ar( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );

	FLPacketWDB_CancelRegistedReq req;
	req.Deserialize( ar );

	if( DB_CONSIGNMENT_SALE()._AddReserve( req.m_u64SaleSRL ) == false )
	{
		FLPacketWDB_CancelRegistedAck ack;
		ack.m_uPlayerID			= req.m_uPlayerID;
		ack.m_u64SaleSRL		= req.m_u64SaleSRL;
		ack.m_eResult			= FLPacket_CancelRegistedAck::E_BUSY;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		return true;
	}

	//find cache
	FLDBConsignmentSale::FLSaleInfo kSaleInfo;
	if( DB_CONSIGNMENT_SALE()._PopItem_AndSendRegistedCount( kSaleInfo, m_kDBOverlappedPlus.dpid, req.m_u64SaleSRL ) == false )
	{
		FLPacketWDB_CancelRegistedAck ack;
		ack.m_uPlayerID			= req.m_uPlayerID;
		ack.m_u64SaleSRL		= req.m_u64SaleSRL;
		ack.m_eResult			= FLPacket_CancelRegistedAck::E_AREADY_CANCELED;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		//FLERROR_LOG( PROGRAM_NAME, "[ NO DATA - CANCEL PROCESS - SRL(%I64u) ]", req.m_u64SaleSRL );
		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
		return true;
	}
	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );

	// Query
	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ call uspConsignmentSale_Delete ( ?, ?, ? ) }" );
	if( query.PrepareQuery( szQuery ) == FALSE )
	{
		//DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );		
		return true;
	}

	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );

	query.BindParameterChar( SQL_PARAM_INPUT,		_countof(szPlayerId)-1,		 szPlayerId );
	query.BindParameterChar( SQL_PARAM_INPUT,		_countof(szServerIndex)-1,	 szServerIndex );
	query.BindParameterUInt64( SQL_PARAM_INPUT,		&req.m_u64SaleSRL );


	if( query.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	if( query.Fetch() == FALSE )		// NO_DATA
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	FLPacketWDB_CancelRegistedAck ack;
	ack.m_uPlayerID			= req.m_uPlayerID;
	ack.m_u64SaleSRL		= req.m_u64SaleSRL;

	const int nRet				= query.GetInt( "nRet" );
	if( nRet != 0 )
	{
		ack.m_eResult			= FLPacket_CancelRegistedAck::E_AREADY_CANCELED;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		return true;
	}

	// item log
	{
		LogItemInfo info;
		MakeItemLogInfo( info, "s", req.m_szPlayerName, "ConsignmentSale CancelComplete"
			, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kSaleInfo.kItemElem, kSaleInfo.kItemElem.m_nItemNum, 0, 0 );
		LOG_DB().PostQueryCommand( info.idSendPlayer, new FLItemGold_LogQuery( info ) );
	}

	// mail
	CDbManager::PostMail( query, req.m_uPlayerID, 0, kSaleInfo.kItemElem, 0, "", "" );

	// send to client
	ack.m_eResult				= FLPacket_CancelRegistedAck::E_SUCCESS;
	CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
	
	return true;
}

bool FLQuery_BuyItem::Execute( CQuery & query )
{
	CAr ar( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );

	FLPacketWDB_BuyItemReq req;
	req.Deserialize( ar );

	//find cache
	FLDBConsignmentSale::FLSaleInfo kSaleInfo;
	if( DB_CONSIGNMENT_SALE()._PopItem_AndSendRegistedCount( kSaleInfo, m_kDBOverlappedPlus.dpid, req.m_u64SaleSRL ) == false )
	{
		FLPacketWDB_CancelRegistedAck ack;
		ack.m_uPlayerID			= req.m_uPlayerID;
		ack.m_u64SaleSRL		= req.m_u64SaleSRL;
		ack.m_eResult			= FLPacket_CancelRegistedAck::E_AREADY_CANCELED;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
		//FLERROR_LOG( PROGRAM_NAME, "[ NO DATA - CANCEL PROCESS - SRL(%I64u) ]", req.m_u64SaleSRL );
		return true;
	}
	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );

	// Query
	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ call uspConsignmentSale_Buy ( ?, ?, ?, ? ) }" );

	//IN
	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	TCHAR szSalerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
	FLSPrintf( szSalerId, _countof( szSalerId ), "%07d", req.m_uSalerID );

	query.BindParameterChar( SQL_PARAM_INPUT,		_countof(szServerIndex)-1,	 szServerIndex );
	query.BindParameterUInt64( SQL_PARAM_INPUT,								 &req.m_u64SaleSRL );
	query.BindParameterChar( SQL_PARAM_INPUT,			_countof(szSalerId)-1,	 szSalerId );
	query.BindParameterInt( SQL_PARAM_INPUT,								 &req.m_nPrice );

	//EXEC
	if( query.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}


	FLPacketWDB_BuyItemAck ack;
	ack.m_uPlayerID			= req.m_uPlayerID;
	ack.m_u64SaleSRL		= req.m_u64SaleSRL;


	if( query.Fetch() == FALSE )		// NO_DATA
	{ 
		ack.m_eResult			= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );

		// Logging & 골드 복구
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	const int nRet				= query.GetInt( "nRet" );
	if( nRet != 0 )
	{
		ack.m_eResult			= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );

		// Logging & 골드 복구
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return true;
	}

	// log
	LogItemInfo info;
	MakeItemLogInfo( info, "s", req.m_szPlayerName, "ConsignmentSale BuyEnd"
		, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kSaleInfo.kItemElem, kSaleInfo.kItemElem.m_nItemNum, 0, 0 );
	LOG_DB().PostQueryCommand( info.idSendPlayer, new FLItemGold_LogQuery( info ) );

	// mail
	CDbManager::PostMail( query, req.m_uPlayerID, 0, kSaleInfo.kItemElem, 0, "", "" );

	// send to client
	ack.m_eResult				= FLPacketWDB_BuyItemAck::E_SUCCESS;
	ack.m_dwItemID				= kSaleInfo.kItemElem.m_dwItemId;
	CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );

	FLPacketWDB_SellItemNoti noti;
	noti.m_uPlayerID			= kSaleInfo.uPlayerID;
	FLStrcpy( noti.m_szPlayerName, _countof( noti.m_szPlayerName ), req.m_szPlayerName );
	noti.m_dwItemID				= kSaleInfo.kItemElem.m_dwItemId;
	noti.m_nGold				= kSaleInfo.nPrice;
	noti.m_eRegistMode			= kSaleInfo.eRegistMode;	

	CDPTrans::GetInstance()->SendPacket( DPID_ALLPLAYERS, &noti );

	return true;
}

bool FLQuery_CollectSaleGold::Execute( CQuery & query )
{
	CAr ar( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );

	FLPacketWDB_CollectSaleGoldReq req;
	req.Deserialize( ar );

	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call uspConsignmentSale_CollectSaleGold ( ?, ?, ? ) }" );

	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );

	int nRet				= 0;
	query.BindParameterInt( SQL_PARAM_OUTPUT, &nRet );
	query.BindParameterChar( SQL_PARAM_INPUT,	_countof(szPlayerId)-1,	 szPlayerId );
	query.BindParameterChar( SQL_PARAM_INPUT,	_countof(szServerIndex)-1, szServerIndex );
	query.BindParameterUInt64( SQL_PARAM_INPUT,	 &req.m_u64SaleGoldSRL );


	if( query.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s " ), szQuery ); 
		return false;
	}

	FLPacketWDB_CollectSaleGoldAck ack;
	ack.m_uPlayerID				= req.m_uPlayerID;
	ack.m_u64SaleGoldSRL		= req.m_u64SaleGoldSRL;


	if( query.Fetch() == FALSE )		// NO_DATA
	{ 
		ack.m_eResult				= FLPacketWDB_CollectSaleGoldAck::E_AREADY_COLLECT;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		FLERROR_LOG( PROGRAM_NAME, _T( "%s " ), szQuery ); 
		return false;
	}

	if( nRet != 0 )
	{
		ack.m_eResult			= FLPacketWDB_CollectSaleGoldAck::E_AREADY_COLLECT;
		CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
		return true;
	}

	ack.m_u64SaleGoldSRL		= req.m_u64SaleGoldSRL;//->GetInt64( "u64SaleGoldSRL" );
	ack.m_nGold					= query.GetInt( "nGold" );

	FLItemElem kItemElem;
	CDbManager::PostMail( query, req.m_uPlayerID, 0, kItemElem, ack.m_nGold, "", "" );

	ack.m_eResult				= FLPacketWDB_CollectSaleGoldAck::E_SUCCESS;
	CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );

	return true;
}

bool FLQuery_SaleGoldInfoList::Execute( CQuery & query )
{
	CAr ar( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );

	FLPacketWDB_SaleGoldInfoListReq req;
	req.Deserialize( ar );

	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ call uspConsignmentSale_GetSaleGoldInfoList ( ?, ? ) }" );

	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );

	query.BindParameterChar( SQL_PARAM_INPUT,		_countof(szPlayerId)-1,		szPlayerId );
	query.BindParameterChar( SQL_PARAM_INPUT,		_countof(szServerIndex)-1,	szServerIndex );

	if( query.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	FLPacketWDB_SaleGoldInfoListAck ack;
	ack.m_uPlayerID				= req.m_uPlayerID;

	while( query.Fetch() == TRUE )		// NO_DATA
	{ 
		FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo info;

		//query.SetTable( "T1" );
		info.u64SaleGoldSRL		= query.GetInt64( "u64ItemInstanceSRL" );
		info.nGold				= query.GetInt( "nGold" );

		char szSaleDate[ DB_DATETIME_BUFFER_LEN ] = { 0 };
		query.GetStr( "tRegDate", szSaleDate, _countof( szSaleDate ) );
		szSaleDate[ _countof( szSaleDate ) - DB_DATETIME_MILLISECOND_LEN - 1 ] = '\0';
		info.kSaleDate		= FLStringToFLTm( szSaleDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS );

		FLItemElem kItemElem;
		int nSerialNumberStatus = 0;
		CDbManager::LoadOneItemCommon( query, /*"T2",*/ 0, 0, kItemElem, nSerialNumberStatus );
		info.kItemElem		= kItemElem;

		PT_ITEM_SPEC pSpec	= g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
		if( pSpec != NULL )
		{
			FLStrcpy( info.szItemName, _countof( info.szItemName ), pSpec->szName );
		}
		
		ack.m_vecSaleGoldInfo.push_back( info );
	}

	CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );
	return true;
}


bool FLQuery_Load::Execute( CQuery & query )
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return true;


	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ call uspConsignmentSale_Load ( ? ) }" );

	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	query.BindParameterChar( SQL_PARAM_INPUT, _countof(szServerIndex)-1, szServerIndex );
	if( query.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
		return false;
	}

	while( query.Fetch() == TRUE )		// NO_DATA
	{ 
		FLDBConsignmentSale::FLSaleInfo kSaleInfo;
		CDbManager::LoadOneSaleInfo_ForServer( query, kSaleInfo );

		if( DATE_TIMER().GetCurrentTime() > kSaleInfo.tmExpiredDate )
		{
			DB_CONSIGNMENT_SALE()._AddExpiredData( kSaleInfo );
			continue;
		}

		DB_CONSIGNMENT_SALE()._AddItem_AndSendRegistedCount( DPID_UNKNOWN, kSaleInfo );
	}
	return true;
}


bool FLQuery_Search::Execute( CQuery & /*query*/ )
{
	using namespace nsConsignmentSale;

	CAr ar( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );
	FLPacketWDB_SearchReq req;
	req.Deserialize( ar );

	FLPacketWDB_SearchAck ack;
	ack.m_uPlayerID				= req.m_uPlayerID;
	DB_CONSIGNMENT_SALE()._SearchItemList( ack, req );

	CDPTrans::GetInstance()->SendPacket( m_kDBOverlappedPlus.dpid, &ack );	
	return true;
}

} /* nsConsignmentSale */