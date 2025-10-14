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

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];

#include "honor.h"

#include "eveschool.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;


// void CDbManager::SavePlayer( CQuery *qry, CQuery* pQueryLog, CMover* pMover, char* szQuery, size_t cchQuery )
// {
// 	szQuery[0] = '\0';
// 
// 	///////// actMover
// 	char cActMover[50] = {0,};
// 	FLSPrintf( cActMover, _countof( cActMover ), "%d,%d", pMover->m_pActMover->m_dwState, pMover->m_pActMover->m_dwStateFlag );
// 
// 	///////// Skill
// 	char JobSkill[1024] = {0,};
// 	char LicenseSkill[1024] = {0,};			
// 	
// 	///////// JobLv
// 	char JobLv[500] = {0,};
// 	SaveJobLv( pMover, JobLv, _countof( JobLv ) );
// 	
// 	///////// Quest
// 	char QuestCnt[3072]	= { 0, };
// 	char m_aCompleteQuest[3072] = { 0, };	// 1024 -> 3072
// 	char CheckedQuest[100] = {0, };
// 	SaveQuest( pMover, QuestCnt, _countof( QuestCnt ), m_aCompleteQuest, _countof( m_aCompleteQuest ), CheckedQuest, _countof( CheckedQuest ) );
// 
// 	///////// Inventory
// //  	ItemContainerStruct	icsBank[3];
// //  	for( int k = 0 ; k < 3 ; ++k )
// //  		SaveBank( pMover, &pMover->m_Bank[k], &icsBank[k] );
// 
// 	///////// Equipment
// 	char Equipment[500] = {0,};
// 	SaveEquipment( pMover, Equipment, _countof( Equipment ) );
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
// 	SaveTaskBar( pMover, AppletTaskBar, _countof( AppletTaskBar ), ItemTaskBar, _countof( ItemTaskBar ), SkillTaskBar, _countof( SkillTaskBar ) );
// 
// 	///////// SM 상용화 아이템
// 	char szSMTime[3072] = { 0, };
// 	SaveSMCode( pMover, szSMTime, _countof( szSMTime ) );
// 
// 	char szSkillInfluence[7500] = { 0, };
// 	SaveSkillInfluence( pMover, szSkillInfluence, _countof( szSkillInfluence ) );
// 
// 	PutExpLog( pQueryLog, pMover, ( pMover->m_dwPeriodicTick == 0 ? 'O': 'P' ) );
// 
// 	// BEGIN110520
// 	const BYTE byProgramDataSlot = GET_PLAYER_SLOT( pMover->m_nDBDataSlot );
// 	if( byProgramDataSlot < 0 || byProgramDataSlot >= MAX_CHARACTER_SLOT )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "SavePlayer(%s) - Slot Invalid:[%d]" ), pMover->m_szName, byProgramDataSlot );
// 		return;
// 	}
// 	// END110520
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// BEGIN110119
// 
// 	T_CHARACTER_STR_INFO tCharacter( *pMover, "C1" );
// 
// // 	// 1
// // 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "C1" );
// // 	tCharacter.dwPlayerId		=	pMover->m_idPlayer;
// // 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), pMover->m_szAccount );
// // 	FLStrcpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), pMover->m_szName );
// // 	tCharacter.nPlayerSlot		=	pMover->m_nDBDataSlot;
// // 	tCharacter.dwWorldId		=	pMover->m_dwWorldID;
// // 	tCharacter.dwIndex			=	pMover->m_dwIndex;
// // 	tCharacter.fPos_x			=	pMover->m_vPos.x;
// // 	tCharacter.fPos_y			=	pMover->m_vPos.y;
// // 
// // 	// 11
// // 	tCharacter.fPos_z			=	pMover->m_vPos.z;
// // 	FLStrcpy( tCharacter.szCharacterKey, _countof( tCharacter.szCharacterKey ), pMover->m_szCharacterKey );
// // 	tCharacter.dwSkinSet		=	pMover->m_dwSkinSet;
// // 	tCharacter.dwHairMesh		=	pMover->m_dwHairMesh;
// // 	tCharacter.dwHairColor		=	pMover->m_dwHairColor;
// // 	tCharacter.dwHeadMesh		=	pMover->m_dwHeadMesh;
// // 	tCharacter.dwSex			=	pMover->GetSex();
// // 	tCharacter.fScale_x			=	pMover->m_vScale.x;
// // 	tCharacter.dwMotion			=	pMover->m_dwMotion;
// // 	tCharacter.fAngle			=	pMover->m_fAngle;
// // 												
// // 	// 21
// // 	tCharacter.nHitPoint		=	pMover->m_nHitPoint;
// // 	tCharacter.nManaPoint		=	pMover->m_nManaPoint;
// // 	tCharacter.nFatiguePoint	=	pMover->m_nFatiguePoint;
// // 	tCharacter.dwRideItemId		=	pMover->m_dwRideItemIdx;
// // 	tCharacter.dwGold			=	pMover->GetGold();
// // 	tCharacter.nJob				=	pMover->m_nJob;
// // 	FLStrcpy( tCharacter.szActMover, _countof( tCharacter.szActMover ), cActMover );
// // 	tCharacter.nStr				=	pMover->m_Stat.GetOriginStr();
// // 	tCharacter.nSta				=	pMover->m_Stat.GetOriginSta();
// // 	tCharacter.nDex				=	pMover->m_Stat.GetOriginDex();
// // 												
// // 	// 31
// // 	tCharacter.nInt				=	pMover->m_Stat.GetOriginInt();
// // 	tCharacter.nLevel			=	pMover->m_nLevel;
// // 	tCharacter.n64Exp1			=	pMover->m_nExp1;
// // 	tCharacter.n64Exp2			=	0;
// // 	FLStrcpy( tCharacter.szJobSkill, _countof( tCharacter.szJobSkill ), JobSkill );
// // 	FLStrcpy( tCharacter.szLicenseSkill, _countof( tCharacter.szLicenseSkill ), LicenseSkill );
// // 	FLStrcpy( tCharacter.szJobLv, _countof( tCharacter.szJobLv ), JobLv );
// // 	tCharacter.dwExpertLv		=	0;
// // 	tCharacter.dwMarkingWorldId	=	pMover->m_idMarkingWorld;
// // 	tCharacter.fMarkingPos_x	=	pMover->m_vMarkingPos.x;
// // 												
// // 	// 41
// // 	tCharacter.fMarkingPos_y	=	pMover->m_vMarkingPos.y;
// // 	tCharacter.fMarkingPos_z	=	pMover->m_vMarkingPos.z;
// // 	tCharacter.nRemainGP		=	pMover->m_Stat.GetRemainStatPoint();
// // 	tCharacter.nRemainLP		=	0;
// // 	tCharacter.nTutorialState	=	pMover->GetTutorialState();		// @im_nFligh
// // 	tCharacter.nFxp				=	pMover->m_nFxp;
// // 	tCharacter.nTxp				=	pMover->m_nFxp;
// // 	FLStrcpy( tCharacter.szQuestCntArray, _countof( tCharacter.szQuestCntArray ), QuestCnt );
// // 	FLStrcpy( tCharacter.szAuthority, _countof( tCharacter.szAuthority ), "F" );
// // 	tCharacter.dwMode			=	pMover->m_dwMode;
// // 												
// // 	// 51
// // 	tCharacter.nPartyId			=	pMover->m_idparty;
// // 	tCharacter.dwMuerdererId	=	pMover->m_idMurderer;
// // 	tCharacter.nFame			=	pMover->m_nFame;
// // 	tCharacter.n64DeathExp		=	pMover->m_nDeathExp;
// // 	tCharacter.nDeathLevel		=	pMover->m_nDeathLevel;
// // 	tCharacter.dwFlyTime		=	pMover->m_dwMute;
// // 	tCharacter.dwMessengerState	=	pMover->m_RTMessenger.GetState();
// // 	tCharacter.nTotalPlayTime	=	0;
// // 	tCharacter.nPKValue			=	pMover->m_nPKValue;
// // 	tCharacter.dwPKPropensity	=	pMover->m_dwPKPropensity;
// // 												
// // 	// 61
// // 	tCharacter.dwPKExp			=	pMover->m_dwPKExp;
// // 	FLStrcpy( tCharacter.szCard, _countof( tCharacter.szCard ), Card );
// // 	FLStrcpy( tCharacter.szIndex_Card, _countof( tCharacter.szIndex_Card ), sCardIndex );
// // 	FLStrcpy( tCharacter.szObjIndex_Card, _countof( tCharacter.szObjIndex_Card ), sCardObjIndex );
// // 	FLStrcpy( tCharacter.szCube, _countof( tCharacter.szCube ), Cube );
// // 	FLStrcpy( tCharacter.szIndex_Cube, _countof( tCharacter.szIndex_Cube ), sCubeIndex );
// // 	FLStrcpy( tCharacter.szObjIndex_Cube, _countof( tCharacter.szObjIndex_Cube ), sCubeObjIndex );
// // 	FLStrcpy( tCharacter.szInventory, _countof( tCharacter.szInventory ), "$" );
// // 	FLStrcpy( tCharacter.szIndex_Inventory, _countof( tCharacter.szIndex_Inventory ), "$" );
// // 	FLStrcpy( tCharacter.szEquipment, _countof( tCharacter.szEquipment ), Equipment );
// // 												
// // 	// 71
// // 	FLStrcpy( tCharacter.szObjIndex_Inventory, _countof( tCharacter.szObjIndex_Inventory ), "$" );
// // 	FLStrcpy( tCharacter.szSlotApplet, _countof( tCharacter.szSlotApplet ), AppletTaskBar );
// // 	FLStrcpy( tCharacter.szSlotItem, _countof( tCharacter.szSlotItem ), ItemTaskBar );
// // 	FLStrcpy( tCharacter.szSlotQueue, _countof( tCharacter.szSlotQueue ), SkillTaskBar );
// // 
// // 	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
// // 	{
// // 		tCharacter.nActionPoint		=	pMover->m_UserTaskBar.m_nActionPoint;	// @im_SkillBar
// // 	}
// // 
// // 	FLStrcpy( tCharacter.szBank, _countof( tCharacter.szBank ), "$" );
// // 	FLStrcpy( tCharacter.szIndex_Bank, _countof( tCharacter.szIndex_Bank ), "$" );
// // 	FLStrcpy( tCharacter.szObjIndex_Bank, _countof( tCharacter.szObjIndex_Bank ), "$" );
// // 	tCharacter.dwGoldBank		=	pMover->m_dwGoldBank[byProgramDataSlot];
// // 	tCharacter.nFuel			=	pMover->m_nFuel;
// // 												
// // 	// 81
// // 	tCharacter.tmAccFuel		=	pMover->m_tmAccFuel;
// // 	FLStrcpy( tCharacter.szSMTime, _countof( tCharacter.szSMTime ), szSMTime );
// // 	FLStrcpy( tCharacter.szSkillInfluence, _countof( tCharacter.szSkillInfluence ), szSkillInfluence );
// // 	tCharacter.dwSkillPoint		=	0;
// // 	FLStrcpy( tCharacter.szCompleteQuest, _countof( tCharacter.szCompleteQuest ), m_aCompleteQuest );
// // 	FLStrcpy( tCharacter.szExtInven, _countof( tCharacter.szExtInven ), "$" );
// // 	FLStrcpy( tCharacter.szExtBank, _countof( tCharacter.szExtBank ), "$" );
// // 	FLStrcpy( tCharacter.szPirecingInven, _countof( tCharacter.szPirecingInven ), "$" );
// // 	FLStrcpy( tCharacter.szPirecingBank, _countof( tCharacter.szPirecingBank ), "$" );
// // 	tCharacter.dwReturnWorldId	=	pMover->m_dwReturnWorldID;
// // 												
// // 	// 91
// // 	tCharacter.fReturnPos_x		=	pMover->m_vReturnPos.x;
// // 	tCharacter.fReturnPos_y		=	pMover->m_vReturnPos.y;
// // 	tCharacter.fReturnPos_z		=	pMover->m_vReturnPos.z;
// // 	tCharacter.nSkillPoint		=	pMover->m_nSkillPoint;
// // 	tCharacter.nSkillLevel		=	pMover->m_nSkillLevel;
// // 	tCharacter.n64SkillExp		=	0;
// // 	tCharacter.n64EventFlag		=	pMover->m_nEventFlag;
// // 	tCharacter.dwEventTime		=	pMover->m_dwEventTime;
// // 	tCharacter.dwEventElapsed	=	pMover->m_dwEventElapsed;
// // 	tCharacter.n64AngelExp		=	pMover->m_nAngelExp;
// // 												
// // 	// 101
// // 	tCharacter.nAngelLevel		=	pMover->m_nAngelLevel;
// // 	FLStrcpy( tCharacter.szInventoryPet, _countof( tCharacter.szInventoryPet ), "$" );
// // 	FLStrcpy( tCharacter.szBankPet, _countof( tCharacter.szBankPet ), "$" );
// // 	tCharacter.dwPetId			=	pMover->GetPetId();
// // 	tCharacter.nExpLog			=	pMover->m_nExpLog;
// // 	tCharacter.nAngelExpLog		=	pMover->m_nAngelExpLog;
// // 	tCharacter.nCoupon			=	pMover->m_nCoupon;
// // 	tCharacter.nHonor			=	pMover->m_nHonor;
// // 	tCharacter.nLayer			=	pMover->GetLayer();
// // 	FLStrcpy( tCharacter.szCheckedQuest, _countof( tCharacter.szCheckedQuest ), CheckedQuest );
// // 												
// // 	// 111
// // 	tCharacter.nCampusPoint		=	pMover->GetCampusPoint();
// // 	tCharacter.dwCampusId		=	pMover->GetCampusId();
// 
// 
// 	if( DBQryCharacter( qry, &tCharacter ) == false )
// 	{
// 		return;
// 	}
// 
// 	if( qry->Fetch() )
// 	{
// 		int nError = qry->GetInt( "fError" );
// 		if( nError != 1 )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "SavePlayer(%s) - fError:%d" ), pMover->m_szName, nError );
// 			return;
// 		}
// 	}
// 
// 	SaveHonor( qry, pMover->m_idPlayer, pMover->m_aHonorTitle, szQuery, cchQuery );
// 
// #ifdef __SKILL_0205
// 	SaveSkill( qry, pMover->m_idPlayer, pMover->m_aJobSkill, pMover->m_abUpdateSkill, szQuery, cchQuery );
// #else	// __SKILL_0205
// 	SaveSkill( qry, pMover->m_idPlayer, pMover->m_aJobSkill, szQuery );
// #endif	// __SKILL_0205
// 
// 	SaveInventory( pMover, qry );
// 
// 	for( int nCharacterSlot = 0; nCharacterSlot < MAX_CHARACTER_SLOT; ++nCharacterSlot )
// 	{
// 		u_long uPlayerId = pMover->m_idPlayerBank[ nCharacterSlot ];
// 		if( uPlayerId != NULL_PLAYER_ID )
// 		{
// 			// BEGIN110520
// 			const int nDBDataSlot = nCharacterSlot + ( GET_PLAYER_PAGE( pMover->m_nDBDataSlot ) * MAX_CHARACTER_SLOT );
// 			
// 			SaveCharacterBank( pMover, qry, uPlayerId , nDBDataSlot );
// 			// END110520
// 		}
// 	}
// 
// 	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
// 	{
// 		SaveCharacterPocket( pMover, qry, nPocket );
// 	}
// 
// 	// 다른 캐릭터의 bank 저장
// 	for( int i = 0; i < MAX_CHARACTER_SLOT; ++i )
// 	{
// 		if( pMover->m_idPlayerBank[i] != 0 && i != byProgramDataSlot )
// 		{
// 			ADDBANK_QUERYINFO info( "U1" );
// 			info.idPlayer		= pMover->m_idPlayerBank[i];
// 			info.dwGoldBank		= pMover->m_dwGoldBank[i];
// 
// 			DBQryAddBankSave( szQuery, cchQuery, info );
// 			{
// 				qry->BindParameterVarChar( SQL_PARAM_INPUT,  4290,		"$" );
// 				qry->BindParameterVarChar( SQL_PARAM_INPUT,  215,		"$" );
// 				qry->BindParameterVarChar( SQL_PARAM_INPUT,  215,		"$" );
// 				qry->BindParameterInt( SQL_PARAM_INPUT,  	&info.dwGoldBank );
// 				qry->BindParameterVarChar( SQL_PARAM_INPUT,  2000,		"$" );
// 				qry->BindParameterVarChar( SQL_PARAM_INPUT,  7800,		"$" );
// 				qry->BindParameterVarChar( SQL_PARAM_INPUT,  2689,		"$" );
// 			}
// 
// 			if( FALSE == qry->Exec( szQuery ) )
// 			{
// 				return;
// 			}
// 		}
// 	}
// 
// 	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
// 	{
// 		PocketParam	p;
// 		p.idPlayer			= pMover->m_idPlayer;
// 		p.bExpired			= pMover->m_Pocket.IsExpired( nPocket );
// 		p.tExpirationDate	= pMover->m_Pocket.GetExpirationDate( nPocket );
// 
// 
// 		MakeQueryPocket( szQuery, cchQuery, p );
// 		{
// 			qry->BindParameterInt( SQL_PARAM_INPUT, &nPocket );
// 			qry->BindParameterVarChar( SQL_PARAM_INPUT, 4290,	 	"$" );
// 			qry->BindParameterVarChar( SQL_PARAM_INPUT, 215,		 	"$" );
// 			qry->BindParameterVarChar( SQL_PARAM_INPUT, 215,		 	"$" );
// 			qry->BindParameterVarChar( SQL_PARAM_INPUT, 2000,	 	"$" );
// 			qry->BindParameterVarChar( SQL_PARAM_INPUT, 7800,	 	"$" );
// 			qry->BindParameterVarChar( SQL_PARAM_INPUT, 2689,	 	"$" );
// 			qry->BindParameterInt( SQL_PARAM_INPUT, &p.bExpired );
// 			qry->BindParameterInt( SQL_PARAM_INPUT, &p.tExpirationDate );
// 		}
// 
// 		if( !qry->Exec( szQuery ) )
// 		{
// 			return;
// 		}
// 	}
// }

void	CDbManager::SaveHonor( CQuery *qry, u_long uidPlayer, int * aHonor, char* szQuery, size_t cchQuery )
{
	int	aHonorEtc[50] = {0,};
	int	aHonorItem[50] = {0,};
	int	aHonorMonster[50] = {0,};

    int nEtc = 0,nItem = 0,nMonster = 0;
	int nType = 0;
	int nCurrentTitleCount =  CTitleManager::Instance()->m_nCurrentTitleCount;
	for(int i=0;i<nCurrentTitleCount;i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if( nType == HI_HUNT_MONSTER)
		{
			aHonorMonster[nMonster] = aHonor[i];
			nMonster++;
		}
		else if( nType == HI_USE_ITEM)
		{
			aHonorItem[nItem] = aHonor[i];
			nItem++;
		}
		else
		{
			aHonorEtc[nEtc] = aHonor[i];
			nEtc++;
		}
	}

	FLSPrintf( szQuery, cchQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 1,
		aHonorEtc[0],aHonorEtc[1],aHonorEtc[2],aHonorEtc[3],aHonorEtc[4],aHonorEtc[5],aHonorEtc[6],aHonorEtc[7],aHonorEtc[8],aHonorEtc[9],
		aHonorEtc[10],aHonorEtc[11],aHonorEtc[12],aHonorEtc[13],aHonorEtc[14],aHonorEtc[15],aHonorEtc[16],aHonorEtc[17],aHonorEtc[18],aHonorEtc[19],
		aHonorEtc[20],aHonorEtc[21],aHonorEtc[22],aHonorEtc[23],aHonorEtc[24],aHonorEtc[25],aHonorEtc[26],aHonorEtc[27],aHonorEtc[28],aHonorEtc[29],
		aHonorEtc[30],aHonorEtc[31],aHonorEtc[32],aHonorEtc[33],aHonorEtc[34],aHonorEtc[35],aHonorEtc[36],aHonorEtc[37],aHonorEtc[38],aHonorEtc[39],
		aHonorEtc[40],aHonorEtc[41],aHonorEtc[42],aHonorEtc[43],aHonorEtc[44],aHonorEtc[45],aHonorEtc[46],aHonorEtc[47],aHonorEtc[48],aHonorEtc[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}

	FLSPrintf( szQuery, cchQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 2,
		aHonorItem[0],aHonorItem[1],aHonorItem[2],aHonorItem[3],aHonorItem[4],aHonorItem[5],aHonorItem[6],aHonorItem[7],aHonorItem[8],aHonorItem[9],
		aHonorItem[10],aHonorItem[11],aHonorItem[12],aHonorItem[13],aHonorItem[14],aHonorItem[15],aHonorItem[16],aHonorItem[17],aHonorItem[18],aHonorItem[19],
		aHonorItem[20],aHonorItem[21],aHonorItem[22],aHonorItem[23],aHonorItem[24],aHonorItem[25],aHonorItem[26],aHonorItem[27],aHonorItem[28],aHonorItem[29],
		aHonorItem[30],aHonorItem[31],aHonorItem[32],aHonorItem[33],aHonorItem[34],aHonorItem[35],aHonorItem[36],aHonorItem[37],aHonorItem[38],aHonorItem[39],
		aHonorItem[40],aHonorItem[41],aHonorItem[42],aHonorItem[43],aHonorItem[44],aHonorItem[45],aHonorItem[46],aHonorItem[47],aHonorItem[48],aHonorItem[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}

	FLSPrintf( szQuery, cchQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 3,
		aHonorMonster[0],aHonorMonster[1],aHonorMonster[2],aHonorMonster[3],aHonorMonster[4],aHonorMonster[5],aHonorMonster[6],aHonorMonster[7],aHonorMonster[8],aHonorMonster[9],
		aHonorMonster[10],aHonorMonster[11],aHonorMonster[12],aHonorMonster[13],aHonorMonster[14],aHonorMonster[15],aHonorMonster[16],aHonorMonster[17],aHonorMonster[18],aHonorMonster[19],
		aHonorMonster[20],aHonorMonster[21],aHonorMonster[22],aHonorMonster[23],aHonorMonster[24],aHonorMonster[25],aHonorMonster[26],aHonorMonster[27],aHonorMonster[28],aHonorMonster[29],
		aHonorMonster[30],aHonorMonster[31],aHonorMonster[32],aHonorMonster[33],aHonorMonster[34],aHonorMonster[35],aHonorMonster[36],aHonorMonster[37],aHonorMonster[38],aHonorMonster[39],
		aHonorMonster[40],aHonorMonster[41],aHonorMonster[42],aHonorMonster[43],aHonorMonster[44],aHonorMonster[45],aHonorMonster[46],aHonorMonster[47],aHonorMonster[48],aHonorMonster[49]	); 

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}


}

#ifdef __SKILL_0205
void CDbManager::SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery, size_t cchQuery )
#else	// #define	__SKILL_0205
void CDbManager::SaveSkill( CQuery *qry, u_long uidPlayer, LPSKILL aJobSkill, char* szQuery, size_t cchQuery )
#endif	// #define	__SKILL_0205
{
	UNREFERENCED_PARAMETER( abUpdateSkill );

	for( int i = 0 ; i < MAX_SKILL_JOB ; i++)
	{
#ifdef __SKILL_0205
// 		if( abUpdateSkill[i] == 0 )
// 			continue;
#endif	// __SKILL_0205
		FLSPrintf( szQuery, cchQuery,
			"uspLearnSkill @serverindex='%02d',@pPlayerID='%07d',@pSkillID=%d,@pSkillLv=%d,@pSkillPosition=%d",
			g_appInfo.dwSys, uidPlayer, aJobSkill[i].dwSkill, aJobSkill[i].dwLevel, i ); 
	
		if( FALSE == qry->Exec( szQuery ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
			return;
//			continue;
		}
	}
}

// raiders_todo 의미 없음 
void CDbManager::SaveJobLv( CMover* /*pMover*/, char* szJobLv, size_t cchJobLv )
{
	char OneJobLv[16] = {0,};
	for( int i = 0 ; i < MAX_JOB ; i++)
	{
		FLSPrintf(OneJobLv, _countof( OneJobLv ), "%d/", 0);
		FLStrncat(szJobLv, cchJobLv, OneJobLv, _countof(OneJobLv));
	}
	FLStrncat(szJobLv, cchJobLv, NullStr, _countof(NullStr));
}

void CDbManager::SaveQuest( CMover* pMover, char* szQuestCnt, size_t cchQuestCnt, char* szm_aCompleteQuest, size_t cchCompleteQuest, char* szCheckedQuest, size_t cchCheckedQuest )
{
	/* 퀘스트 하나당 저장되는 사이즈가 최대 50. 저장되는 파라미터 추가 시 사이즈 고려하여 MAX_DOING_QUEST 사이즈를 수정 바람.*/
	char sPerQuest[128];
	char OneCompleteQuest[20] = {0,};
	char OneCheckedQuest[20] = {0, };

	for( size_t i = 0; i < pMover->m_nQuestSize; i++ )
	{
		FLSPrintf( sPerQuest, _countof( sPerQuest ), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
			pMover->m_aQuest[i].m_wId, pMover->m_aQuest[i].m_nState, pMover->m_aQuest[i].m_wTime,
			pMover->m_aQuest[i].m_nKillNPCNum[0], pMover->m_aQuest[i].m_nKillNPCNum[1],
			pMover->m_aQuest[i].m_bPatrol, pMover->m_aQuest[i].m_bReserve2,	pMover->m_aQuest[i].m_bReserve3, pMover->m_aQuest[i].m_bReserve4,
			pMover->m_aQuest[i].m_bReserve5, pMover->m_aQuest[i].m_bReserve6, pMover->m_aQuest[i].m_bReserve7, pMover->m_aQuest[i].m_bReserve8
			, pMover->m_aQuest[i].m_nKillNPCNum[2], pMover->m_aQuest[i].m_nKillNPCNum[3]
			);
		FLStrncat( szQuestCnt, cchQuestCnt, sPerQuest, _countof(sPerQuest) );
	}
	
	for( size_t i = 0; i < pMover->m_nCompleteQuestSize; i++ )
	{
		FLSPrintf(OneCompleteQuest, _countof( OneCompleteQuest ), "%d/", pMover->m_aCompleteQuest[i]);
		FLStrncat( szm_aCompleteQuest, cchCompleteQuest, OneCompleteQuest, _countof(OneCompleteQuest) );
	}

	FLStrcat( szQuestCnt, cchQuestCnt, NullStr );
	FLStrcat( szm_aCompleteQuest, cchCompleteQuest, NullStr );

	for( size_t i = 0; i < pMover->m_nCheckedQuestSize; ++i )
	{
		FLSPrintf( OneCheckedQuest, _countof( OneCheckedQuest ), "%d/", pMover->m_aCheckedQuest[i]);
		FLStrncat( szCheckedQuest, cchCheckedQuest, OneCheckedQuest, _countof( OneCheckedQuest ) );
	}
	FLStrcat( szCheckedQuest, cchCheckedQuest, NullStr );
}

// void	CDbManager::SavePocket( CMover* pMover, PPocketStruct pPocketStruct )
// {
// 	for( int nPocket = 0; nPocket < MAX_POCKET; nPocket++ )
// 	{
// 		ItemStruct is;
// 		BOOL	bExt	= FALSE;
// 		BOOL	bPiercing	= FALSE;
// 		BOOL	bPet	= FALSE;
// 		char sIndex[10]		= { 0,};
// 		char sObjIndex[16]		= { 0,};
// 		CPocket* pPocket	= pMover->m_Pocket.GetPocket( nPocket );
// 		if( pPocket )
// 		{
// 			for( DWORD i = 0; i < pPocket->GetMax(); i++ )
// 			{
// 				SaveOneItem( pPocket->GetAtId( i ), &is );
// 				FLStrncat( pPocketStruct[nPocket].szItem, _countof( pPocketStruct[nPocket].szItem ), is.szItem, _countof(is.szItem) );
// 				FLStrncat( pPocketStruct[nPocket].szExt, _countof( pPocketStruct[nPocket].szExt ), is.szExt, _countof(is.szExt) );
// 				FLStrncat( pPocketStruct[nPocket].szPiercing, _countof( pPocketStruct[nPocket].szPiercing ), is.szPiercing, _countof(is.szPiercing) );
// 				FLSPrintf( sIndex, _countof( sIndex ), "%d/", pPocket->m_apIndex[i] );
// 				FLStrncat( pPocketStruct[nPocket].szIndex, _countof( pPocketStruct[nPocket].szIndex ), sIndex, _countof(sIndex) );
// 				FLSPrintf( sObjIndex, _countof( sObjIndex ), "%d/", pPocket->m_apItem[i].m_dwObjIndex );
// 				FLStrcat( pPocketStruct[nPocket].szObjIndex, _countof( pPocketStruct[nPocket].szObjIndex ), sObjIndex );
// 				FLStrncat( pPocketStruct[nPocket].szPet, _countof( pPocketStruct[nPocket].szPet ), is.szPet, _countof(is.szPet) );
// 				if( pPocket->m_apItem[i].m_bCharged || 0 < pPocket->m_apItem[i].m_dwKeepTime || 0 != pPocket->m_apItem[i].GetRandomOptItemId() )
// 					bExt	= TRUE;
// 				if( pPocket->m_apItem[i].IsPiercedItem() )
// 					bPiercing	= TRUE;
// 				if( pPocket->m_apItem[i].m_pPet )
// 					bPet	= TRUE;
// 			}
// 			FLStrncat( pPocketStruct[nPocket].szItem, _countof( pPocketStruct[nPocket].szItem ), NullStr, _countof(NullStr) );
// 			FLStrncat( pPocketStruct[nPocket].szIndex, _countof( pPocketStruct[nPocket].szIndex ), NullStr, _countof(NullStr) );
// 			FLStrcat( pPocketStruct[nPocket].szObjIndex, _countof( pPocketStruct[nPocket].szObjIndex ), NullStr );
// 			if( bExt == FALSE )
// 				*pPocketStruct[nPocket].szExt	= '\0';
// 			if( bPiercing == FALSE )
// 				*pPocketStruct[nPocket].szPiercing	= '\0';
// 			if( bPet == FALSE )
// 				*pPocketStruct[nPocket].szPet	= '\0';
// 			FLStrcat( pPocketStruct[nPocket].szExt, _countof( pPocketStruct[nPocket].szExt ), NullStr );
// 			FLStrcat( pPocketStruct[nPocket].szPiercing, _countof( pPocketStruct[nPocket].szPiercing ), NullStr );
// 			FLStrcat( pPocketStruct[nPocket].szPet, _countof( pPocketStruct[nPocket].szPet ), NullStr );
// 			pPocketStruct[nPocket].bExpired		= pPocket->IsExpired();
// 			pPocketStruct[nPocket].tExpirationDate	= pPocket->GetExpirationDate();
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			SaveSerialNumberStatus( pPocketStruct[nPocket].szItem, _countof( pPocketStruct[nPocket].szItem ), pPocket->GetAllocedSerialNumber() );
// 			//////////////////////////////////////////////////////////////////////////
// 		}
// 		else
// 		{
// 			FLStrcpy( pPocketStruct[nPocket].szItem, _countof( pPocketStruct[nPocket].szItem ), NullStr ); 
// 			FLStrcpy( pPocketStruct[nPocket].szIndex, _countof( pPocketStruct[nPocket].szIndex ), NullStr ); 
// 			FLStrcpy( pPocketStruct[nPocket].szObjIndex, _countof( pPocketStruct[nPocket].szObjIndex ), NullStr ); 
// 			FLStrcpy( pPocketStruct[nPocket].szExt, _countof( pPocketStruct[nPocket].szExt ), NullStr ); 
// 			FLStrcpy( pPocketStruct[nPocket].szPiercing, _countof( pPocketStruct[nPocket].szPiercing ), NullStr ); 
// 			FLStrcpy( pPocketStruct[nPocket].szPet, _countof( pPocketStruct[nPocket].szPet ), NullStr ); 
// 			pPocketStruct[nPocket].bExpired		= TRUE;
// 			pPocketStruct[nPocket].tExpirationDate	= 0;
// 		}
// 	}
// }

void CDbManager::SaveEquipment( CMover* pMover, char* szEquipment, size_t cchEquipmen )
{
	char OneEquipment[16] = {0,};
	for( int i = 0 ; i < MAX_HUMAN_PARTS ; i++)
	{
		FLSPrintf( OneEquipment, _countof( OneEquipment ), "%d/", pMover->m_aEquipInfo[i].byFlag );
		FLStrncat(szEquipment, cchEquipmen, OneEquipment, _countof(OneEquipment));
	}
	FLStrncat(szEquipment, cchEquipmen, NullStr, _countof(NullStr));
}

// void CDbManager::SaveBank( CMover* pMover, CItemContainer<FLItemElem  >* pPlayerBank, ItemContainerStruct * pItemContainerStruct )
// {
// 	ItemStruct	is;
// 	char Onem_apIndex[10] = {0,};
// 	char sPerObjIndex[16]	= { 0, };
// 	BOOL bExtBank = FALSE;
// 	BOOL bPirecingBank = FALSE;
// 
// 	BOOL	bPet	= FALSE;
// 
// 	for( DWORD ch = 0; ch < pPlayerBank->GetMax(); ch++ )	// 0-504
// 	{	
// 		SaveOneItem( &pPlayerBank->m_apItem[ch], &is );
// 		FLStrncat( pItemContainerStruct->szItem, _countof( pItemContainerStruct->szItem ), is.szItem, _countof(is.szItem) );
// 		FLStrncat( pItemContainerStruct->szExt, _countof( pItemContainerStruct->szExt ), is.szExt, _countof(is.szExt) );
// 		FLStrncat( pItemContainerStruct->szPiercing, _countof( pItemContainerStruct->szPiercing ), is.szPiercing, _countof(is.szPiercing) );
// 		FLSPrintf( Onem_apIndex, _countof( Onem_apIndex ), "%d/", pPlayerBank->m_apIndex[ch]);
// 		FLStrncat( pItemContainerStruct->szIndex, _countof( pItemContainerStruct->szIndex ), Onem_apIndex, _countof(Onem_apIndex) );
// 		FLSPrintf( sPerObjIndex, _countof( sPerObjIndex ), "%d/", pPlayerBank->m_apItem[ch].m_dwObjIndex );
// 		FLStrcat( pItemContainerStruct->szObjIndex, _countof( pItemContainerStruct->szObjIndex ), sPerObjIndex );
// 		FLStrncat( pItemContainerStruct->szPet, _countof( pItemContainerStruct->szPet ), is.szPet, _countof(is.szPet) );
// 		if( pPlayerBank->m_apItem[ch].m_bCharged || 0 < pPlayerBank->m_apItem[ch].m_dwKeepTime || 0 != pPlayerBank->m_apItem[ch].GetRandomOptItemId() )
// 			bExtBank = TRUE;
// 		if( pPlayerBank->m_apItem[ch].IsPiercedItem() )
// 			bPirecingBank = TRUE;
// 		if( pPlayerBank->m_apItem[ch].m_pPet )
// 			bPet	= TRUE;
// 	}
// 	FLStrncat( pItemContainerStruct->szItem, _countof( pItemContainerStruct->szItem ), NullStr, _countof(NullStr) );
// 	FLStrncat( pItemContainerStruct->szIndex, _countof( pItemContainerStruct->szIndex ), NullStr, _countof(NullStr) );
// 	FLStrcat( pItemContainerStruct->szObjIndex, _countof( pItemContainerStruct->szObjIndex ), NullStr );
// 	if( bExtBank == FALSE )
// 		*pItemContainerStruct->szExt	= '\0';
// 	if( bPirecingBank == FALSE )
// 		*pItemContainerStruct->szPiercing	= '\0';
// 	if( bPet == FALSE )
// 		*pItemContainerStruct->szPet	= '\0';
// 	FLStrcat( pItemContainerStruct->szExt, _countof( pItemContainerStruct->szExt ), NullStr );
// 	FLStrcat( pItemContainerStruct->szPiercing, _countof( pItemContainerStruct->szPiercing ), NullStr );
// 	FLStrcat( pItemContainerStruct->szPet, _countof( pItemContainerStruct->szPet ), NullStr );
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	SaveSerialNumberStatus( pItemContainerStruct->szItem, _countof( pItemContainerStruct->szItem ), pPlayerBank->GetAllocedSerialNumber() );
// 	//////////////////////////////////////////////////////////////////////////
// }

// void CDbManager::SaveGuildBank( CItemContainer<FLItemElem>*  pGuildBank, PItemContainerStruct pItemContainerStruct )
// {
// 	ItemStruct is;
// 	char Onem_apIndex[10] = {0,};
// 	char sPerObjIndex[16]	= { 0, };
// 	BOOL bExtBank = FALSE;
// 	BOOL bPirecingBank = FALSE;
// 	BOOL bPet	= FALSE;
// 
// 	for( DWORD ch = 0; ch < pGuildBank->GetMax(); ch++ )	// 0-504
// 	{	
// 		SaveOneItem( &pGuildBank->m_apItem[ch], &is );
// 		FLStrncat( pItemContainerStruct->szItem, _countof( pItemContainerStruct->szItem ), is.szItem, _countof(is.szItem) );
// 		FLStrncat( pItemContainerStruct->szExt, _countof( pItemContainerStruct->szExt ), is.szExt, _countof(is.szExt) );
// 		FLStrncat( pItemContainerStruct->szPiercing, _countof( pItemContainerStruct->szPiercing ), is.szPiercing, _countof(is.szPiercing) );
// 		FLSPrintf( Onem_apIndex, _countof( Onem_apIndex ), "%d/", pGuildBank->m_apIndex[ch]);
// 		FLStrncat( pItemContainerStruct->szIndex, _countof( pItemContainerStruct->szIndex ), Onem_apIndex, _countof(Onem_apIndex) );
// 		FLSPrintf( sPerObjIndex, _countof( sPerObjIndex ), "%d/", pGuildBank->m_apItem[ch].m_dwObjIndex );
// 		FLStrcat( pItemContainerStruct->szObjIndex, _countof( pItemContainerStruct->szObjIndex ), sPerObjIndex );
// 		FLStrncat( pItemContainerStruct->szPet, _countof( pItemContainerStruct->szPet ), is.szPet, _countof(is.szPet) );
// 
// 		if( pGuildBank->m_apItem[ch].m_bCharged || 0 < pGuildBank->m_apItem[ch].m_dwKeepTime || 0 != pGuildBank->m_apItem[ch].GetRandomOptItemId() )
// 			bExtBank = TRUE;
// 		if( pGuildBank->m_apItem[ch].IsPiercedItem() )
// 			bPirecingBank = TRUE;
// 		if( pGuildBank->m_apItem[ch].m_pPet )
// 			bPet	= TRUE;
// 	}
// 	FLStrncat( pItemContainerStruct->szItem, _countof( pItemContainerStruct->szItem ), NullStr, _countof(NullStr) );
// 	FLStrncat( pItemContainerStruct->szIndex, _countof( pItemContainerStruct->szIndex ), NullStr, _countof(NullStr));
// 	FLStrcat( pItemContainerStruct->szObjIndex, _countof( pItemContainerStruct->szObjIndex ), NullStr );
// 	if( bExtBank == FALSE )
// 		*pItemContainerStruct->szExt	= '\0';
// 	if( bPirecingBank == FALSE )
// 		*pItemContainerStruct->szPiercing	= '\0';
// 	FLStrcat( pItemContainerStruct->szExt, _countof( pItemContainerStruct->szExt ), NullStr );
// 	FLStrcat( pItemContainerStruct->szPiercing, _countof( pItemContainerStruct->szPiercing ), NullStr );
// 	FLStrcat( pItemContainerStruct->szPet, _countof( pItemContainerStruct->szPet ), NullStr );
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	SaveSerialNumberStatus( pItemContainerStruct->szItem, _countof( pItemContainerStruct->szItem ), pGuildBank->GetAllocedSerialNumber() );
// 	//////////////////////////////////////////////////////////////////////////
// }

void CDbManager::SaveTaskBar( CMover* pMover, char* szAppletTaskBar, size_t cchAppletTaskBar, char* szItemTaskBar, size_t cchItemTaskBar, char* szSkillTaskBar, size_t cchSkillTaskBar )
{
	char OneAppletTaskBar[MAX_TASKBAR] = { 0, };
	char OneItemTaskBar[MAX_TASKBAR] = { 0, };
	char OneSkillTaskBar[256] = { 0, };

	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
	{
		for( int ch = 0 ; ch < MAX_SLOT_APPLET ; ch++ )
		{
			if( pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut != SHORTCUT_NONE )
			{
				if( pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut == SHORTCUT_ITEM )
				{
					DWORD dwObjIndex = pMover->m_Inventory.GetObjIndexByObjId( pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwId );
					if( dwObjIndex != NULL_ID )
					{
						pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwUserId = dwObjIndex;
					}
				}

				FLSPrintf( OneAppletTaskBar, _countof( OneAppletTaskBar ), "%d,%d,%d,%d,%d,%d,%d"
					, ch
					, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut
					, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwId
					, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwType
					, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwIndex
					, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwUserId
					, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwData );

				if( pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut == SHORTCUT_CHAT)
					SetStrDBFormat( OneAppletTaskBar, _countof( OneAppletTaskBar ), pMover->m_UserTaskBar.m_aSlotApplet[ch].m_szString );

				FLStrncat( szAppletTaskBar, cchAppletTaskBar, OneAppletTaskBar, _countof(OneAppletTaskBar));
				FLStrcat( szAppletTaskBar, cchAppletTaskBar, "/" );
			}
		}
	}
	FLStrcat( szAppletTaskBar, cchAppletTaskBar, NullStr );
	
	for( int ch = 0; ch < MAX_SLOT_ITEM_COUNT; ch++ )
	{
		for( int j = 0; j < MAX_SLOT_ITEM; j++ )
		{
			if( pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut != SHORTCUT_NONE )
			{
				if( pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut == SHORTCUT_ITEM )
				{
					DWORD dwObjIndex = pMover->m_Inventory.GetObjIndexByObjId( pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwId );
					if( dwObjIndex != NULL_ID )
					{
						pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwUserId = dwObjIndex;
					}
				}

				FLSPrintf( OneItemTaskBar, _countof( OneItemTaskBar ), "%d,%d,%d,%d,%d,%d,%d,%d"
					, ch
					, j
					, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut
					, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwId
					, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwType
					, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwIndex
					, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwUserId
					, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwData );
				
				if( pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut == SHORTCUT_CHAT )
					SetStrDBFormat( OneItemTaskBar, _countof( OneItemTaskBar ), pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_szString );
				
				FLStrncat( szItemTaskBar, cchItemTaskBar, OneItemTaskBar, _countof(OneItemTaskBar) );
				FLStrcat( szItemTaskBar, cchItemTaskBar, "/" );
			}
		}
	}
	FLStrcat( szItemTaskBar, cchItemTaskBar, NullStr );
	
	for( int ch = 0; ch < MAX_SLOT_QUEUE; ch++ )
	{
		if( pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut != SHORTCUT_NONE )		// 값이 들어있는지 검사
		{
			FLSPrintf( OneSkillTaskBar, _countof( OneSkillTaskBar ), "%d,%d,%d,%d,%d,%d,%d/"
				, ch
				, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut
				, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwId
				, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwType
				, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwIndex
				, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwUserId
				, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwData );
			FLStrncat( szSkillTaskBar, cchSkillTaskBar, OneSkillTaskBar, _countof( OneSkillTaskBar ) );
		}			
	}
	FLStrcat( szSkillTaskBar, cchSkillTaskBar, NullStr );
}

void CDbManager::SaveSMCode( CMover* pMover, char* szSMTime, size_t cchSMTime )
{
	BOOL bSMSave = FALSE;
	for( int ch = 0 ; ch < SM_MAX ; ++ch )
	{
		if( 0 < pMover->m_dwSMTime[ch] )
		{
			bSMSave = TRUE;
		}
		char OneSMTime[256] = { 0, };
		if( ch == SM_MAX_HP50 )
		{
			FLSPrintf( OneSMTime, _countof( OneSMTime ), "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nPlusMaxHitPoint );
		}
		else
		if( ch == SM_RESIST_ATTACK_LEFT )
		{
			FLSPrintf( OneSMTime, _countof( OneSMTime ), "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nAttackResistLeft );
		}
		else
		if( ch == SM_RESIST_ATTACK_RIGHT )
		{
			FLSPrintf( OneSMTime, _countof( OneSMTime ), "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nAttackResistRight );
		}
		else
		if( ch == SM_RESIST_DEFENSE )
		{
			FLSPrintf( OneSMTime, _countof( OneSMTime ), "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nDefenseResist );
		}
		else
		{
			FLSPrintf( OneSMTime, _countof( OneSMTime ), "%d,", pMover->m_dwSMTime[ch] );
		}
		FLStrcat( szSMTime, cchSMTime, OneSMTime );
	}
	FLStrcat( szSMTime, cchSMTime, NullStr );
	
	if( bSMSave == FALSE )
	{
		memset( szSMTime, 0, sizeof( szSMTime ) );
	}
}

void CDbManager::SaveSkillInfluence( CMover* pMover, char* szSkillInfluence, size_t cchSkillInfluence )
{
	pMover->m_buffs.ToString( szSkillInfluence, cchSkillInfluence );
}

void CDbManager::SavePlayTime( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	DWORD dwTime;
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, _countof( lpDbOverlappedPlus->AccountInfo.szAccount ) );
	u_long idPlayer;
	arRead >> idPlayer;
	arRead >> dwTime; // 시각
	dwTime /= 1000;

	//////////////////////////////////////////////////////////////////////////
	// BEGIN11019

	CMover Mover;
	Mover.m_idPlayer	= idPlayer;

	T_CHARACTER_STR_INFO tCharacter( Mover, "U3" );
	tCharacter.nPlayerSlot	= static_cast<int>( dwTime );

// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "U3" );
// 	tCharacter.dwPlayerId = idPlayer;
// 	tCharacter.nPlayerSlot = dwTime;

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return;
	}
	
// 	if( FALSE == qry->Exec( szQuery ) )
// 	{
// 		WriteLog( "%s, %d\t%s\r\n\t%s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szPlayer );
// 	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}

void CDbManager::DBQryAddBankSave( char* szSql, size_t cchSql, const ADDBANK_QUERYINFO & info )
{
	FLSPrintf( szSql, cchSql, "{call ADD_BANK_STR('U1','%07d','%02d', ?, ?, ?, ?, ?, ?, ?)}", info.idPlayer, g_appInfo.dwSys ); 
}

void CDbManager::MakeQueryPocket( char* szQuery, size_t cchQuery, const PocketParam & p )
{
	FLSPrintf( szQuery, cchQuery, "{call uspSavePocket( '%02d', '%07d', ?, ?, ?, ?, ?, ?, ?, ?, ?) }", g_appInfo.dwSys, p.idPlayer );
}

// void CDbManager::SaveOneItem( FLItemElem* pItemElem, PItemStruct pItemStruct )
// {
// 	char szPiercing[32]		= {0,};
// 
// 	if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE )
// 	{
// 		if( pItemElem->m_dwItemId == ITEM_INDEX( 26476, II_SYS_SYS_SCR_SEALCHARACTER ) )
// 			FLSPrintf( pItemStruct->szItem, _countof( pItemStruct->szItem ), "%d,%d,%d,%d,,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",	//17
// 					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
// 					0, 0, 
// 					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
// 					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
// 					0, pItemElem->m_byFlag,
// 					pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
//   					pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
//   					pItemElem->m_idGuild,
// 					pItemElem->m_nResistSMItemId
// 				);
// 		else
// 			FLSPrintf( pItemStruct->szItem, _countof( pItemStruct->szItem ), "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",	//17
// 					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
// 					0, 0, pItemElem->m_szItemText,
// 					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
// 					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
// 					0, pItemElem->m_byFlag,
// 					pItemElem->GetSerialNumber(), pItemElem->GetOption(), 
//   					pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption,
//   					pItemElem->m_idGuild,
// 					pItemElem->m_nResistSMItemId
// 				);
// 
// 		// ext
// 		FLSPrintf( pItemStruct->szExt, _countof( pItemStruct->szExt ), "%d,%d,%I64d"
// 			",%d"
// 			"/", pItemElem->m_bCharged, pItemElem->m_dwKeepTime, pItemElem->GetRandomOptItemId()
// 			, static_cast<int>( pItemElem->m_bTranformVisPet )
// 			);
// 
// 		// piercing
// 		PT_ITEM_SPEC itemProp = g_xSpecManager->GetSpecItem( pItemElem->m_dwItemId );
// 		if( itemProp && itemProp->IsUltimate() )
// 		{
// 			FLSPrintf( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), "%d", pItemElem->GetUltimatePiercingSize() );
// 			for( int nPirecing = 0 ; nPirecing < pItemElem->GetUltimatePiercingSize(); ++nPirecing )
// 			{
// 				FLSPrintf( szPiercing, _countof( szPiercing ), ",%d", pItemElem->GetUltimatePiercingItemID( nPirecing ) );
// 				FLStrncat( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), szPiercing, _countof(szPiercing) );
// 			}
// 			if( pItemElem->GetGeneralPiercingSize() > 0 )
// 			{
// 				FLSPrintf( szPiercing, _countof( szPiercing ), ",%d", pItemElem->GetGeneralPiercingSize() );
// 				FLStrncat( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), szPiercing, _countof(szPiercing) );
// 			}
// 		}
// 		else
// 			FLSPrintf( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), "%d", pItemElem->GetGeneralPiercingSize() );
// 
// 		for( int nPirecing = 0 ; nPirecing < pItemElem->GetGeneralPiercingSize(); ++nPirecing )
// 		{
// 			FLSPrintf( szPiercing, _countof( szPiercing ), ",%d", pItemElem->GetGeneralPiercingItemID( nPirecing ) );
// 			FLStrncat( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), szPiercing, _countof(szPiercing) );
// 		}
// 
// 		if( pItemElem->IsVisPet() )
// 		{
// 			for( int nPirecing = 0; nPirecing < pItemElem->GetGeneralPiercingSize(); ++nPirecing )
// 			{
// 				FLSPrintf( szPiercing, _countof( szPiercing ), ",%d", pItemElem->GetVisPetPiercingItemKeepTime( nPirecing ) );
// 				FLStrncat( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), szPiercing, _countof(szPiercing) );
// 			}
// 		}
// 
// 		FLStrcat( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), "/" );
// 
// 		// pet
// 		if( pItemElem->m_pPet )
// 		{
// 			FLSPrintf( pItemStruct->szPet, _countof( pItemStruct->szPet ), "1,%d,%d,%d,%d,%d", 
// 				pItemElem->m_pPet->GetKind(), pItemElem->m_pPet->GetLevel(),
// 				pItemElem->m_pPet->GetExp(), pItemElem->m_pPet->GetEnergy(),
// 				pItemElem->m_pPet->GetLife() );
// 			for( int i = PL_D; i <= pItemElem->m_pPet->GetLevel(); i++ )
// 			{
// 				char szAvailLevel[16]	= { 0,};
// 				FLSPrintf( szAvailLevel, _countof( szAvailLevel ), ",%d", pItemElem->m_pPet->GetAvailLevel( i ) );
// 				FLStrcat( pItemStruct->szPet, _countof( pItemStruct->szPet ), szAvailLevel );
// 			}
// 
// 			char szTemp[MAX_PET_NAME_FMT+1]		= { 0,};
// 			char szFmt[MAX_PET_NAME_FMT]	= { 0,};
// 			SetDBFormatStr( szFmt, MAX_PET_NAME_FMT, pItemElem->m_pPet->GetName() );
// 			FLSPrintf( szTemp, _countof( szTemp ), ",%s", szFmt );
// 			FLStrcat( pItemStruct->szPet, _countof( pItemStruct->szPet ), szTemp );
// 			FLStrcat( pItemStruct->szPet, _countof( pItemStruct->szPet ), "/" );
// 		}
// 		else
// 		{
// 			FLSPrintf( pItemStruct->szPet, _countof( pItemStruct->szPet ), "0/" );
// 		}
// 	}
// 	else
// 	{
// 		// item
// 		ZeroMemory( pItemStruct->szItem, sizeof(pItemStruct->szItem) );
// 		// ext
// 		FLSPrintf( pItemStruct->szExt, _countof( pItemStruct->szExt ), "%d,%d,%d/", 0, 0, 0 );
// 		// piercing
// 		FLSPrintf( pItemStruct->szPiercing, _countof( pItemStruct->szPiercing ), "%d/", 0 );
// 		// pet
// 		FLSPrintf( pItemStruct->szPet, _countof( pItemStruct->szPet ), "0/" );
// 	}
// }