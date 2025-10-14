#include "StdAfx.h"

#include "FLPlayer_Query.h"

#include "DbManager.h"
#include "FLDataBase.h"

#include "FLExpLog_Query.h"

#include "../DatabaseServer/dpcoresrvr.h"
#include "../_CommonDefine/Packet/FLPacket_Certify.h"


extern	CDPCoreSrvr g_dpCoreSrvr;

FLQuery_SavePlayer::FLQuery_SavePlayer( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize
									   , CMover * pMover ) 
 : FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize )/*, &m_kMover( pMover )*/ 
{ 
	m_kMover.Copy( pMover, TRUE );
}



bool FLQuery_SavePlayer::Execute( CQuery & query )
{
	const bool ret = execQuery( query );
	if( m_kMover.m_bLogout )
	{
		g_DbManager.m_AccountCacheMgr.ChangeMultiServer( m_kMover.m_szAccount, 0 );
	}

	return ret;
}


bool FLQuery_SavePlayer::execQuery( CQuery & query )
{
// 	///////// actMover
// 	char cActMover[50] = {0,};
// 	FLSPrintf( cActMover, _countof( cActMover ), "%d,%d", m_kMover.m_pActMover->m_dwState, m_kMover.m_pActMover->m_dwStateFlag );
// 
// 	///////// Skill
// 	char JobSkill[1024] = {0,};
// 	char LicenseSkill[1024] = {0,};			
// 
// 	///////// JobLv
// 	char JobLv[500] = {0,};
// 	CDbManager::SaveJobLv( &m_kMover, JobLv, _countof( JobLv ) );
// 
// 	///////// Quest
// 	char QuestCnt[3072]	= { 0, };
// 	char m_aCompleteQuest[3072] = { 0, };	// 1024 -> 3072
// 	char CheckedQuest[100] = {0, };
// 	CDbManager::SaveQuest( &m_kMover, QuestCnt, _countof( QuestCnt ), m_aCompleteQuest, _countof( m_aCompleteQuest ), CheckedQuest, _countof( CheckedQuest ) );
// 
// 	///////// Inventory
// 	//  	ItemContainerStruct	icsBank[3];
// 	//  	for( int k = 0 ; k < 3 ; ++k )
// 	//  		SaveBank( &m_kMover, &m_kMover.m_Bank[k], &icsBank[k] );
// 
// 	///////// Equipment
// 	char Equipment[500] = {0,};
// 	CDbManager::SaveEquipment( &m_kMover, Equipment, _countof( Equipment ) );
// 
// 	///////// CardCube	
// 	char Card[2]	= "$";
// 	char sCardIndex[2]	= "$";
// 	char sCardObjIndex[2]	= "$";
// 	char Cube[2]	= "$";
// 	char sCubeIndex[2]	= "$";
// 	char sCubeObjIndex[2]	= "$";
// 
// 	///////// TaskBar
// 	char AppletTaskBar[MAX_APPLETTASKBAR] = { 0, }; 
// 	char ItemTaskBar[MAX_ITEMTASKBAR] = { 0, };  
// 	char SkillTaskBar[1024] = { 0, };
// 	CDbManager::SaveTaskBar( &m_kMover, AppletTaskBar, _countof( AppletTaskBar ), ItemTaskBar, _countof( ItemTaskBar ), SkillTaskBar, _countof( SkillTaskBar ) );
// 
// 	///////// SM 상용화 아이템
// 	char szSMTime[3072] = { 0, };
// 	CDbManager::SaveSMCode( &m_kMover, szSMTime, _countof( szSMTime ) );
// 
// 	char szSkillInfluence[7500] = { 0, };
// 	CDbManager::SaveSkillInfluence( &m_kMover, szSkillInfluence, _countof( szSkillInfluence ) );

//	PutExpLog( pQueryLog, &m_kMover, ( m_kMover.m_dwPeriodicTick == 0 ? 'O': 'P' ) );
//	FLExpLog_Query kLogQuery( m_kMover, ( m_kMover.m_dwPeriodicTick == 0 ? 'O': 'P' ) );
//	kLogQuery.DirectExecute( LOG_DB(), m_kMover.m_szAccount[ 0 ] );
	LOG_DB().PostQueryCommand( m_kMover.m_szAccount[ 0 ], new FLExpLog_Query( m_kMover, m_kMover.m_dwPeriodicTick == 0 ? 'O': 'P' ) );

	// BEGIN110520
	const BYTE byProgramDataSlot = GET_PLAYER_SLOT( m_kMover.m_nDBDataSlot );
	if( byProgramDataSlot < 0 || byProgramDataSlot >= MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "SavePlayer(%s) - Slot Invalid:[%d]" ), m_kMover.m_szName, byProgramDataSlot );
		return true;
	}
	// END110520

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	CDbManager::T_CHARACTER_STR_INFO tCharacter( m_kMover, "C1" );

// 	// 1
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "C1" );
// 	tCharacter.dwPlayerId		=	m_kMover.m_idPlayer;
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), m_kMover.m_szAccount );
// 	FLStrcpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), m_kMover.m_szName );
// 	tCharacter.nPlayerSlot		=	m_kMover.m_nDBDataSlot;
// 	tCharacter.dwWorldId		=	m_kMover.m_dwWorldID;
// 	tCharacter.dwIndex			=	m_kMover.m_dwIndex;
// 	tCharacter.fPos_x			=	m_kMover.m_vPos.x;
// 	tCharacter.fPos_y			=	m_kMover.m_vPos.y;
// 
// 	// 11
// 	tCharacter.fPos_z			=	m_kMover.m_vPos.z;
// 	FLStrcpy( tCharacter.szCharacterKey, _countof( tCharacter.szCharacterKey ), m_kMover.m_szCharacterKey );
// 	tCharacter.dwSkinSet		=	m_kMover.m_dwSkinSet;
// 	tCharacter.dwHairMesh		=	m_kMover.m_dwHairMesh;
// 	tCharacter.dwHairColor		=	m_kMover.m_dwHairColor;
// 	tCharacter.dwHeadMesh		=	m_kMover.m_dwHeadMesh;
// 	tCharacter.dwSex			=	m_kMover.GetSex();
// 	tCharacter.fScale_x			=	m_kMover.m_vScale.x;
// 	tCharacter.dwMotion			=	m_kMover.m_dwMotion;
// 	tCharacter.fAngle			=	m_kMover.m_fAngle;
// 
// 	// 21
// 	tCharacter.nHitPoint		=	m_kMover.m_nHitPoint;
// 	tCharacter.nManaPoint		=	m_kMover.m_nManaPoint;
// 	tCharacter.nFatiguePoint	=	m_kMover.m_nFatiguePoint;
// 	tCharacter.dwRideItemId		=	m_kMover.m_dwRideItemIdx;
// 	tCharacter.dwGold			=	m_kMover.GetGold();
// 	tCharacter.nJob				=	m_kMover.m_nJob;
// 	FLStrcpy( tCharacter.szActMover, _countof( tCharacter.szActMover ), cActMover );
// 	tCharacter.nStr				=	m_kMover.m_Stat.GetOriginStr();
// 	tCharacter.nSta				=	m_kMover.m_Stat.GetOriginSta();
// 	tCharacter.nDex				=	m_kMover.m_Stat.GetOriginDex();
// 
// 	// 31
// 	tCharacter.nInt				=	m_kMover.m_Stat.GetOriginInt();
// 	tCharacter.nLevel			=	m_kMover.m_nLevel;
// 	tCharacter.n64Exp1			=	m_kMover.m_nExp1;
// 	tCharacter.n64Exp2			=	0;
// 	FLStrcpy( tCharacter.szJobSkill, _countof( tCharacter.szJobSkill ), JobSkill );
// 	FLStrcpy( tCharacter.szLicenseSkill, _countof( tCharacter.szLicenseSkill ), LicenseSkill );
// 	FLStrcpy( tCharacter.szJobLv, _countof( tCharacter.szJobLv ), JobLv );
// 	tCharacter.dwExpertLv		=	0;
// 	tCharacter.dwMarkingWorldId	=	m_kMover.m_idMarkingWorld;
// 	tCharacter.fMarkingPos_x	=	m_kMover.m_vMarkingPos.x;
// 
// 	// 41
// 	tCharacter.fMarkingPos_y	=	m_kMover.m_vMarkingPos.y;
// 	tCharacter.fMarkingPos_z	=	m_kMover.m_vMarkingPos.z;
// 	tCharacter.nRemainGP		=	m_kMover.m_Stat.GetRemainStatPoint();
// //	tCharacter.nRemainLP		=	0;
// 	tCharacter.nRemainLP		=	m_kMover.m_kMadrigalGiftPoint.nPoint;
// 	tCharacter.nTutorialState	=	m_kMover.GetTutorialState();		// @im_nFligh
// 	tCharacter.nFxp				=	m_kMover.m_nFxp;
// 	tCharacter.nTxp				=	m_kMover.m_nFxp;
// 	FLStrcpy( tCharacter.szQuestCntArray, _countof( tCharacter.szQuestCntArray ), QuestCnt );
// 	FLStrcpy( tCharacter.szAuthority, _countof( tCharacter.szAuthority ), "F" );
// 	tCharacter.dwMode			=	m_kMover.m_dwMode;
// 
// 	// 51
// 	tCharacter.nPartyId			=	m_kMover.m_idparty;
// 	tCharacter.dwMuerdererId	=	m_kMover.m_idMurderer;
// 	tCharacter.nFame			=	m_kMover.m_nFame;
// 	tCharacter.n64DeathExp		=	m_kMover.m_nDeathExp;
// 	tCharacter.nDeathLevel		=	m_kMover.m_nDeathLevel;
// 	tCharacter.dwFlyTime		=	m_kMover.m_dwMute;
// 	tCharacter.dwMessengerState	=	m_kMover.m_RTMessenger.GetState();
// 	tCharacter.nTotalPlayTime	=	0;
// 	tCharacter.nPKValue			=	m_kMover.m_nPKValue;
// 	tCharacter.dwPKPropensity	=	m_kMover.m_dwPKPropensity;
// 
// 	// 61
// 	tCharacter.dwPKExp			=	m_kMover.m_dwPKExp;
// 	FLStrcpy( tCharacter.szCard, _countof( tCharacter.szCard ), Card );
// 	FLStrcpy( tCharacter.szIndex_Card, _countof( tCharacter.szIndex_Card ), sCardIndex );
// 	FLStrcpy( tCharacter.szObjIndex_Card, _countof( tCharacter.szObjIndex_Card ), sCardObjIndex );
// 	FLStrcpy( tCharacter.szCube, _countof( tCharacter.szCube ), Cube );
// 	FLStrcpy( tCharacter.szIndex_Cube, _countof( tCharacter.szIndex_Cube ), sCubeIndex );
// 	FLStrcpy( tCharacter.szObjIndex_Cube, _countof( tCharacter.szObjIndex_Cube ), sCubeObjIndex );
// 	FLStrcpy( tCharacter.szInventory, _countof( tCharacter.szInventory ), "$" );
// 	FLStrcpy( tCharacter.szIndex_Inventory, _countof( tCharacter.szIndex_Inventory ), "$" );
// 	FLStrcpy( tCharacter.szEquipment, _countof( tCharacter.szEquipment ), Equipment );
// 
// 	// 71
// 	FLStrcpy( tCharacter.szObjIndex_Inventory, _countof( tCharacter.szObjIndex_Inventory ), "$" );
// 	FLStrcpy( tCharacter.szSlotApplet, _countof( tCharacter.szSlotApplet ), AppletTaskBar );
// 	FLStrcpy( tCharacter.szSlotItem, _countof( tCharacter.szSlotItem ), ItemTaskBar );
// 	FLStrcpy( tCharacter.szSlotQueue, _countof( tCharacter.szSlotQueue ), SkillTaskBar );
// 
// 	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
// 	{
// 		tCharacter.nActionPoint		=	m_kMover.m_UserTaskBar.m_nActionPoint;	// @im_SkillBar
// 	}
// 
// 	FLStrcpy( tCharacter.szBank, _countof( tCharacter.szBank ), "$" );
// 	FLStrcpy( tCharacter.szIndex_Bank, _countof( tCharacter.szIndex_Bank ), "$" );
// 	FLStrcpy( tCharacter.szObjIndex_Bank, _countof( tCharacter.szObjIndex_Bank ), "$" );
// 	tCharacter.dwGoldBank		=	m_kMover.m_dwGoldBank[byProgramDataSlot];
// 	tCharacter.nFuel			=	m_kMover.m_nFuel;
// 
// 	// 81
// 	tCharacter.tmAccFuel		=	m_kMover.m_tmAccFuel;
// 	FLStrcpy( tCharacter.szSMTime, _countof( tCharacter.szSMTime ), szSMTime );
// 	FLStrcpy( tCharacter.szSkillInfluence, _countof( tCharacter.szSkillInfluence ), szSkillInfluence );
// 	tCharacter.dwSkillPoint		=	0;
// 	FLStrcpy( tCharacter.szCompleteQuest, _countof( tCharacter.szCompleteQuest ), m_aCompleteQuest );
// 	FLStrcpy( tCharacter.szExtInven, _countof( tCharacter.szExtInven ), "$" );
// 	FLStrcpy( tCharacter.szExtBank, _countof( tCharacter.szExtBank ), "$" );
// 	FLStrcpy( tCharacter.szPirecingInven, _countof( tCharacter.szPirecingInven ), "$" );
// 	FLStrcpy( tCharacter.szPirecingBank, _countof( tCharacter.szPirecingBank ), "$" );
// 	tCharacter.dwReturnWorldId	=	m_kMover.m_dwReturnWorldID;
// 
// 	// 91
// 	tCharacter.fReturnPos_x		=	m_kMover.m_vReturnPos.x;
// 	tCharacter.fReturnPos_y		=	m_kMover.m_vReturnPos.y;
// 	tCharacter.fReturnPos_z		=	m_kMover.m_vReturnPos.z;
// 	tCharacter.nSkillPoint		=	m_kMover.m_nSkillPoint;
// 	tCharacter.nSkillLevel		=	m_kMover.m_nSkillLevel;
// 	tCharacter.n64SkillExp		=	0;
// 	tCharacter.n64EventFlag		=	m_kMover.m_nEventFlag;
// 	tCharacter.dwEventTime		=	m_kMover.m_dwEventTime;
// 	tCharacter.dwEventElapsed	=	m_kMover.m_dwEventElapsed;
// 	tCharacter.n64AngelExp		=	m_kMover.m_nAngelExp;
// 
// 	// 101
// 	tCharacter.nAngelLevel		=	m_kMover.m_nAngelLevel;
// 	FLStrcpy( tCharacter.szInventoryPet, _countof( tCharacter.szInventoryPet ), "$" );
// 	FLStrcpy( tCharacter.szBankPet, _countof( tCharacter.szBankPet ), "$" );
// 	tCharacter.dwPetId			=	m_kMover.GetPetId();
// 	tCharacter.nExpLog			=	m_kMover.m_nExpLog;
// 	tCharacter.nAngelExpLog		=	m_kMover.m_nAngelExpLog;
// 	tCharacter.nCoupon			=	m_kMover.m_nCoupon;
// 	tCharacter.nHonor			=	m_kMover.m_nHonor;
// 	tCharacter.nLayer			=	m_kMover.GetLayer();
// 	FLStrcpy( tCharacter.szCheckedQuest, _countof( tCharacter.szCheckedQuest ), CheckedQuest );
// 
// 	// 111
// 	tCharacter.nCampusPoint		=	m_kMover.GetCampusPoint();
// 	tCharacter.dwCampusId		=	m_kMover.GetCampusId();
// 	tCharacter.dcStr			=	m_kMover.m_Stat.GetDiceStr();
// 	tCharacter.dcSta			=	m_kMover.m_Stat.GetDiceSta();
// 	tCharacter.dcDex			=	m_kMover.m_Stat.GetDiceDex();
// 	tCharacter.dcInt			=	m_kMover.m_Stat.GetDiceInt();


	/************************************************************************/
	if( m_kMover.CheckPlayerOriginStat() == false )
	{
		CDbManager::WriteFileLogCharacter( _T( "MODIFY_STAT" ), _T( "[ SAVE - INVALID DB STAT DATA. ]" ), &tCharacter );
	}
	/************************************************************************/


	if( CDbManager::DBQryCharacter( &query, &tCharacter ) == false )
	{
		return false;
	}

	if( query.Fetch() )
	{
		int nError = query.GetInt( "fError" );
		if( nError != 1 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "SavePlayer(%s) - fError:%d" ), m_kMover.m_szName, nError );
			return false;
		}
	}

//-------------------------------------------------------------------character가 필요 없는---------------------------------------------------//

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0 };
	CDbManager::SaveHonor( &query, m_kMover.m_idPlayer, m_kMover.m_aHonorTitle, szQuery, _countof( szQuery ) );

#ifdef __SKILL_0205
	CDbManager::SaveSkill( &query, m_kMover.m_idPlayer, m_kMover.m_aJobSkill, m_kMover.m_abUpdateSkill, szQuery, _countof( szQuery ) );
#else	// __SKILL_0205
	CDbManager::SaveSkill( &query, m_kMover.m_idPlayer, m_kMover.m_aJobSkill, szQuery );
#endif	// __SKILL_0205

	CDbManager::SaveInventory( &m_kMover, &query );

	for( int nCharacterSlot = 0; nCharacterSlot < MAX_CHARACTER_SLOT; ++nCharacterSlot )
	{
		u_long uPlayerId = m_kMover.m_idPlayerBank[ nCharacterSlot ];
		if( uPlayerId != NULL_PLAYER_ID )
		{
			// BEGIN110520
			const int nDBDataSlot = nCharacterSlot + ( GET_PLAYER_PAGE( m_kMover.m_nDBDataSlot ) * MAX_CHARACTER_SLOT );

			CDbManager::SaveCharacterBank( &m_kMover, &query, uPlayerId , nDBDataSlot );
			// END110520
		}
	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		CDbManager::SaveCharacterPocket( &m_kMover, &query, nPocket );
	}

	// 다른 캐릭터의 bank 저장
	for( int i = 0; i < MAX_CHARACTER_SLOT; ++i )
	{
		if( m_kMover.m_idPlayerBank[i] != 0 && i != byProgramDataSlot )
		{
			ADDBANK_QUERYINFO info( "U1" );
			info.idPlayer		= m_kMover.m_idPlayerBank[i];
			info.dwGoldBank		= m_kMover.m_dwGoldBank[i];

			CDbManager::DBQryAddBankSave( szQuery, _countof( szQuery ), info );
			{
				query.BindParameterVarChar( SQL_PARAM_INPUT,  4290,		"$" );
				query.BindParameterVarChar( SQL_PARAM_INPUT,  215,		"$" );
				query.BindParameterVarChar( SQL_PARAM_INPUT,  215,		"$" );
				query.BindParameterInt( SQL_PARAM_INPUT,  		&info.dwGoldBank );
				query.BindParameterVarChar( SQL_PARAM_INPUT,  2000,		"$" );
				query.BindParameterVarChar( SQL_PARAM_INPUT,  7800,		"$" );
				query.BindParameterVarChar( SQL_PARAM_INPUT,  2689,		"$" );
			}

			if( FALSE == query.Exec( szQuery ) )
			{
				return false;
			}
		}
	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		PocketParam	p;
		p.idPlayer			= m_kMover.m_idPlayer;
		p.bExpired			= m_kMover.m_Pocket.IsExpired( nPocket );
		p.tExpirationDate	= m_kMover.m_Pocket.GetExpirationDate( nPocket );


		CDbManager::MakeQueryPocket( szQuery, _countof( szQuery ), p );
		{
			query.BindParameterInt( SQL_PARAM_INPUT, &nPocket );
			query.BindParameterVarChar( SQL_PARAM_INPUT, 4290,	"$" );
			query.BindParameterVarChar( SQL_PARAM_INPUT, 215,	"$" );
			query.BindParameterVarChar( SQL_PARAM_INPUT, 215,	"$" );
			query.BindParameterVarChar( SQL_PARAM_INPUT, 2000,	"$" );
			query.BindParameterVarChar( SQL_PARAM_INPUT, 7800,	"$" );
			query.BindParameterVarChar( SQL_PARAM_INPUT, 2689,	"$" );
			query.BindParameterInt( SQL_PARAM_INPUT, &p.bExpired );
			query.BindParameterInt( SQL_PARAM_INPUT, &p.tExpirationDate );
		}

		if( !query.Exec( szQuery ) )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------//

bool FLQuery_LoadPlayer::Execute( CQuery & query )
{
	//BYTE	_nSlot;		// 0 ~ 2
	//u_long	idPlayer;
	DWORD	/*dwAuthKey,*/ dwBillingClass		= 0;
	char	szBankPass[5]	= { 0, };
	char	szPartyName[36]	= { 0, };
	char	szTimeGuild[15]	= { 0, };
	TAG_ENTRY tags[MAX_TAGS];

	CAr arRead( m_kDBOverlappedPlus.lpBuf, m_kDBOverlappedPlus.uBufSize );
	FLPacketAtoDB_ChannelJoinAck fromA;
	fromA.Deserialize( arRead );

	FLStrcpy( m_kDBOverlappedPlus.AccountInfo.szAccount, _countof( m_kDBOverlappedPlus.AccountInfo.szAccount ), fromA.m_kAccount.szAccount );
	FLStrcpy( m_kDBOverlappedPlus.AccountInfo.szPassword, _countof( m_kDBOverlappedPlus.AccountInfo.szPassword ), fromA.m_kAccount.szPassword );

	m_kDBOverlappedPlus.dpid		= fromA.m_kDpids.dpid;
	m_kDBOverlappedPlus.dpidCache 	= fromA.m_kDpids.dpidCache;
	m_kDBOverlappedPlus.dpidUser	= fromA.m_kDpids.dpidUser;

	//arRead >> dwAuthKey;	// o
	//arRead.ReadString( m_kDBOverlappedPlus.AccountInfo.szAccount, _countof( m_kDBOverlappedPlus.AccountInfo.szAccount ) );
	//arRead.ReadString( m_kDBOverlappedPlus.AccountInfo.szPassword, _countof( m_kDBOverlappedPlus.AccountInfo.szPassword ) );
	//arRead >> _nSlot >> idPlayer;

	//arRead >> m_kDBOverlappedPlus.dpid >> m_kDBOverlappedPlus.dpidCache >> m_kDBOverlappedPlus.dpidUser;

	//DWORD dwPCBangClass;
	//arRead >> dwPCBangClass;
	//fromA.m_kAtoDB.kDBtoA.kWtoDB.

	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// JOIN_0\t// %d" ), m_kDBOverlappedPlus.dpid );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	CMover Mover;
	Mover.m_idPlayer	= fromA.m_kAccount.uPlayerID;
	FLStrcpy( Mover.m_szAccount, _countof( Mover.m_szAccount ), m_kDBOverlappedPlus.AccountInfo.szAccount );

	CDbManager::T_CHARACTER_STR_INFO tCharacter( Mover, "S8" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S8" );
// 	tCharacter.dwPlayerId = fromA.m_kAccount.uPlayerID;
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), m_kDBOverlappedPlus.AccountInfo.szAccount );

	if( CDbManager::DBQryCharacter( &query, &tCharacter ) == false )
	{
		return false;
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	if( !query.Fetch() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Fetch Failed. Account(%s), PlayerID(%07d) ]" ), fromA.m_kAccount.szAccount, fromA.m_kAccount.uPlayerID );
		return false;
	}

	query.GetStr( "m_szName", m_kDBOverlappedPlus.AccountInfo.szPlayer, _countof( m_kDBOverlappedPlus.AccountInfo.szPlayer ) );
	//m_joinLock.Enter();
	//FLStrcpy( m_aszJoin[m_nJoin], _countof( m_aszJoin[m_nJoin] ), m_kDBOverlappedPlus.AccountInfo.szPlayer );
	//m_nJoin	= ( m_nJoin + 1 ) % MAX_JOIN_SIZE;
	//m_joinLock.Leave();

	// BEGIN110520
	//	BYTE nSlot = (BYTE)query.GetInt( "playerslot" );
	const BYTE byDBDataSlot			= static_cast<BYTE>( query.GetInt( "playerslot" ) );
	const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( byDBDataSlot );
	// END110520

	// 임시 변수에 쿼리 결과를 저장한다
	CMover	mover;

	// BEGIN110520
	//	mover.m_nSlot	= nSlot;
	mover.m_nDBDataSlot				= byDBDataSlot;		// 캐릭터 고유의 슬롯넘버를 저장
	// END110520

	mover.m_dwPCBangClass			= fromA.m_dwPCBangClass;

	mover.m_dwIndex					= static_cast<DWORD>( query.GetInt( "m_dwIndex" ) );
	mover.InitProp();
	mover.m_idPlayer				= static_cast<u_long>( query.GetInt( "m_idPlayer" ) );

	BOOL bRefresh					= FALSE;
	if( query.GetInt( "m_idCompany" ) > 0 )
	{
		bRefresh					= TRUE;
	}

	/*GetInventory( &mover, query, m_kDBOverlappedPlus );
	GetEquipment( &mover, query, m_kDBOverlappedPlus );
	GetTaskBar( &mover, query, m_kDBOverlappedPlus );
	GetCardCube( &mover, query, m_kDBOverlappedPlus );
	GetBaseCharacter( &mover, query, m_kDBOverlappedPlus );
	GetJobLv( &mover, query, m_kDBOverlappedPlus );
	GetQuest( &mover, query, m_kDBOverlappedPlus );
	GetSMMode( &mover, query, m_kDBOverlappedPlus );
	GetSKillInfluence( &mover, query, m_kDBOverlappedPlus );*/
	// mirchang_100416 VERIFYSTRING use return value
	CDbManager::GetBaseCharacter( &mover, &query, &m_kDBOverlappedPlus );
	if( CDbManager::GetInventory( &mover, &query, &m_kDBOverlappedPlus ) == FALSE
		|| CDbManager::GetEquipment( &mover, &query, &m_kDBOverlappedPlus ) == FALSE
		|| CDbManager::GetTaskBar( &mover, &query, &m_kDBOverlappedPlus ) == FALSE
		|| CDbManager::GetQuest( &mover, &query, &m_kDBOverlappedPlus ) == FALSE
		|| CDbManager::GetSMMode( &mover, &query, &m_kDBOverlappedPlus ) == FALSE
		|| CDbManager::GetSKillInfluence( &mover, &query, &m_kDBOverlappedPlus ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Get Data Failed. Account(%s), PlayerID(%07d) ]" ), fromA.m_kAccount.szAccount, fromA.m_kAccount.uPlayerID );
		return false;
	}
	// mirchang_100416

	mover.m_RTMessenger.SetState( static_cast<DWORD>( query.GetInt( "m_nMessengerState" ) ) );

	mover.m_dwReturnWorldID		= query.GetInt( "m_dwReturnWorldID" );
	mover.m_vReturnPos.x		= query.GetFloat( "m_vReturnPos_x" );
	mover.m_vReturnPos.y		= query.GetFloat( "m_vReturnPos_y" );
	mover.m_vReturnPos.z		= query.GetFloat( "m_vReturnPos_z" );

	DWORD dwPlayTime			= query.GetInt( "TotalPlayTime" );
//	mover.m_dwMute				= query.GetInt( "m_dwFlyTime" );
	DWORD dwFlyTime				= 0;	//
	int nMaximumLevel			= query.GetInt( "m_nMaximumLevel" );
	query.GetStr( "m_tGuildMember", szTimeGuild, _countof( szTimeGuild ) );

	char cAuth					= query.GetChar( "m_chLoginAuthority" );
	if( cAuth == 'F' )
	{
		cAuth					= query.GetChar( "m_chAuthority" );
	}
	mover.m_dwAuthorization		= static_cast<DWORD>( cAuth );

	if( mover.m_dwAuthorization < AUTH_OBSERVER 
#ifndef __INTERNALSERVER
		|| !fromA.m_permitedIP
#endif // __INTERNALSERVER
		)
	{
		mover.m_dwAuthorization = AUTH_GENERAL;
	}

#ifdef __INTERNALSERVER
	mover.m_dwAuthorization		= AUTH_ADMINISTRATOR;
#endif // __INTERNALSERVER

	mover.m_dwMode				&= ~( EVENT_OLDBOY_MODE );
	if( query.GetInt( "last_connect" ) == 0 )
	{
		mover.m_dwMode			|= EVENT_OLDBOY_MODE;
	}
	mover.m_idGuild				= static_cast<u_long>( query.GetInt( "m_idGuild" ) );
	mover.m_idWar				= static_cast<u_long>( query.GetInt( "m_idWar" ) );

	mover.m_nEventFlag			= query.GetInt64( "dwEventFlag" );
	mover.m_dwEventTime			= static_cast<DWORD>( query.GetInt( "dwEventTime" ) );
	mover.m_dwEventElapsed		= static_cast<DWORD>( query.GetInt( "dwEventElapsed" ) );

	DWORD dwPetId				= static_cast<DWORD>( query.GetInt( "m_dwPetId" ) );
	mover.SetPetId( dwPetId );

	mover.m_nExpLog				= query.GetInt( "m_nExpLog" );
	mover.m_nAngelExpLog		= query.GetInt( "m_nAngelExpLog" );

	mover.m_nCoupon				= query.GetInt( "m_nCoupon" );

#ifdef __LAYER_1015
	mover.SetLayer( query.GetInt( "m_nLayer" ) );
#endif	// __LAYER_1015

	mover.SetCampusPoint( query.GetInt( "m_nCampusPoint" ) );
	mover.SetCampusId( query.GetInt( "idCampus" ) );
	// stat dice?
	mover.m_nRestPoint			= query.GetInt( "m_nRestPoint" );
//	mover.m_tLogOut				= time_null() - query.GetInt( "m_LogOutTime" );
	mover.m_tLogOut				= query.GetInt( "m_LogOutTime" );


	/************************************************************************/
	if( mover.CheckPlayerOriginStat() == false )
	{
		CDbManager::T_CHARACTER_STR_INFO CharacterStrInfo( mover, "S8" );
		FLStrcpy( CharacterStrInfo.szAccount, _countof( CharacterStrInfo.szAccount ), fromA.m_kAccount.szAccount );
		CDbManager::WriteFileLogCharacter( _T( "MODIFY_STAT" ), _T( "[ LOAD - INVALID DB STAT DATA. ]" ), &CharacterStrInfo );
	}
	/************************************************************************/

	if( !query.MoreResults() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ Get MoreResults Failed. Account(%s), PlayerID(%07d) ]" ), fromA.m_kAccount.szAccount, fromA.m_kAccount.uPlayerID );
		return false;
	}

	u_long arrPlayerId[ MAX_CHARACTER_SLOT ] = { 0, };

	// BEGIN110520
	int arrDBDataSlot[ MAX_CHARACTER_SLOT ] = { -1, };
	const BYTE bySelectPage = GET_PLAYER_PAGE( byDBDataSlot );
	// END110520

	while( query.Fetch() )
	{
		const int nDBDataSlot		= query.GetInt( "playerslot" );

		// BEGIN110520
		if( bySelectPage != GET_PLAYER_PAGE( nDBDataSlot ) )
		{
			continue;
		}

		const int nProgramDataSlot	= GET_PLAYER_SLOT( nDBDataSlot );
		// END110520

		u_long uPlayerid			= static_cast<u_long>( query.GetInt( "m_idPlayer" ) );

		if( nProgramDataSlot < 0 || nProgramDataSlot >= MAX_CHARACTER_SLOT )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Player Solt is Invalid. Account:[%s], DBDataSlot:[%d], ProgramDataSlot:[%d]" )
				, m_kDBOverlappedPlus.AccountInfo.szAccount, nDBDataSlot, nProgramDataSlot );
			return false;
		}
		if( byDBDataSlot == nDBDataSlot )
		{
			query.GetStr( "m_BankPw", szBankPass, _countof( szBankPass ) );
		}
		//GetBank( &mover, query, m_kDBOverlappedPlus, nPlayerSlot );
		// mirchang_100416 VERIFYSTRING use return value
		if( CDbManager::GetBank( &mover, &query, &m_kDBOverlappedPlus, nProgramDataSlot ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. GetBank failed. PlayerId:[%07d]" ), mover.m_idPlayer );
			return false;
		}
		// mirchang_100416

		arrPlayerId[ nProgramDataSlot ]		= uPlayerid;

		// BEGIN110520
		arrDBDataSlot[ nProgramDataSlot ]	= nDBDataSlot;
		// END110520
	}

	if( query.MoreResults() )
	{
		while( query.Fetch() )
		{
			//GetPocket( &mover, query, m_kDBOverlappedPlus );
			// mirchang_100416 VERIFYSTRING use return value
			if( CDbManager::GetPocket( &mover, &query, &m_kDBOverlappedPlus ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. GetPocket failed. PlayerId:[%07d]" ), mover.m_idPlayer );
				return false;
			}
			// mirchang_100416
		}
	}

	CDbManager::GetMyPartyName( &query, &m_kDBOverlappedPlus, mover.m_idPlayer, szPartyName, _countof( szPartyName ) );
	int nTags = CDbManager::SelectTag( &query, &m_kDBOverlappedPlus, mover.m_idPlayer, tags );
	CDbManager::LoadMessenger( &mover, &query );

	CDbManager::GetHonor( &mover, &query, &m_kDBOverlappedPlus );

	if( !CDbManager::GetSkill( &mover, &query, &m_kDBOverlappedPlus ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. GetSkill failed. PlayerId:[%07d]" ), mover.m_idPlayer );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// LoadInventory
	if( CDbManager::LoadInventory( &mover, &query ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. LoadInventory failed. PlayerId:[%07d]" ), mover.m_idPlayer );
		return false;
	}

	CDbManager::RefreshTaskBar( &mover );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Load Bank
	for( int nPlayerSlot = 0; nPlayerSlot < MAX_CHARACTER_SLOT; ++nPlayerSlot )
	{
		u_long uPlayerId = arrPlayerId[ nPlayerSlot ];
		if( uPlayerId != NULL_PLAYER_ID )
		{
			if( CDbManager::LoadCharacterBank( &mover, &query, uPlayerId, arrDBDataSlot[ nPlayerSlot ] ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. LoadCharacterBank failed. PlayerId:[%07d], DBDataSlot:[%d]" )
					, uPlayerId, arrDBDataSlot[ nPlayerSlot ] );
				return false;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Load Pocket
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( CDbManager::LoadCharacterPocket( &mover, &query, nPocket ) == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. LoadCharacterPocket failed. PlayerId:[%07d], nPocket:[%d]" )
				, mover.m_idPlayer, nPocket );
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------------------------------------------//
	//AddToAccountCacheMgr, szAccount, &mover 
	if( g_DbManager.GetMover_FromAccountCacheMgrA( mover, bRefresh, m_kDBOverlappedPlus.AccountInfo.szAccount, byProgramDataSlot, dwPlayTime ) == true )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ GetMover_FromAccountCacheMgrA - Can Join Failed. Account(%s), PlayerID(%07d) ]" ), fromA.m_kAccount.szAccount, fromA.m_kAccount.uPlayerID );
		return true;
	}

//----------------------------------------------------------------------------------------------------------------------------//


	// 지급, 삭제
	BOOL bResult	= FALSE;
	bResult
		= CDbManager::SendItemtoCharacter( &mover, &query, &query, NULL, &m_kDBOverlappedPlus );
	bResult
		= CDbManager::RemoveItemtoCharacter( &mover, &query, &query, NULL, &m_kDBOverlappedPlus )
		|| bResult;

	bResult	= mover.RemoveItemIK3()
		|| bResult;
	/*
	- 문제 -
	임시 변수에서 지급 삭제가 이루어졌기 때문에,
	해당 사용자의 월드 서버로의 진입이 실패할 경우 자료가
	유실 될 수 있다. 따라서 이에 대한 처리가 필요하다.
	- 해결 -
	1. 지급, 삭제로 인하여 변경이 있을 경우 캐시에 재 복사한다.
	2. 지급과 삭제에 대한 쿼리부분에 대한 결과를 임의의 자료에
	저장하고 이를 메모리에서 처리한다.
	- 현재 1, 추후 2로 수정 예정
	*/
	if( bResult )
	{
		g_DbManager.GetMover_FromAccountCacheMgrB( mover, m_kDBOverlappedPlus.AccountInfo.szAccount, byProgramDataSlot );
	}

//	PutExpLog( querylog, &mover, 'I' );
	//FLExpLog_Query kLogQuery( mover, 'I' );
	//kLogQuery.DirectExecute( LOG_DB(), mover.m_szAccount[ 0 ] );
	LOG_DB().PostQueryCommand( mover.m_szAccount[ 0 ], new FLExpLog_Query( mover, 'I' ) );


	CDbManager::SendJoin( &mover, &m_kDBOverlappedPlus, fromA.m_kAccount.dwAuthKey, dwBillingClass, szPartyName, szBankPass, dwPlayTime, dwFlyTime, nMaximumLevel, nTags, tags, szTimeGuild, fromA.m_tmBirthDate );

	g_dpCoreSrvr.SendMemberTime( mover.m_idPlayer, szTimeGuild );

	//////////////////////////////////////////////////////////////////////////
	// TODO 텔레포트 지도 등록지점 데이터 전송
	CDbManager::SelectTeleportMapPointList( &query, fromA.m_kAccount.uPlayerID, &m_kDBOverlappedPlus );
	//////////////////////////////////////////////////////////////////////////

	return true;
}
