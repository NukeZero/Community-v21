#include "StdAfx.h"
#include "dbmanager.h"
#include "../DatabaseServer/dploginsrvr.h"
#include "../DatabaseServer/dpcoresrvr.h"
#include "../DatabaseServer/dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include <logging/FLMyTrace.h>
#include "lang.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;


void CDbManager::ItemUpdateThread( void )
{
	if( prj.m_bItemUpdate == FALSE )
		return;
	
	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::ITEM))
#else
	if( FALSE == pQuery->Connect( 3, DSN_NAME_ITEMUPDATE, DB_ADMIN_ID_ITEMUPDATE, DB_ADMIN_PASS_ITEMUPDATE ) )
#endif
	{
		char s[128];
		FLSPrintf( s, _countof( s ), "%s, %d ItemUpdate Connect Failed", __FILE__, __LINE__ );
		AfxMessageBox( s );
		SAFE_DELETE( pQuery );
		return;
	}

	HANDLE hHandle	= m_hItemUpdateCloseWorker;
	int nItemYes = 0;

	while( WaitForSingleObject( hHandle, 5000 ) == WAIT_TIMEOUT )	// 10초	= 1000 * 10 //1000 * 60 * 5
	{
		if( nItemYes < m_nItemUpdate )
		{
			nItemYes = m_nItemUpdate;
			ChangeItemUpdate( pQuery );	
			ChangeSkillUpdate( pQuery );
			ChangeMonsterUpdate( pQuery );
			ChangeQuestUpdate( pQuery );	
//			ChangeJobNameUpdate( pQuery );
//			ChangeSexUpdate( pQuery );			
			FLTRACE_LOG( PROGRAM_NAME, _T( "Item Update Complete GOOD !!!" ) );
		}
		FLTRACE_LOG( PROGRAM_NAME, _T( "Worker ITEM_UPDATE" ) );
	}
	SAFE_DELETE( pQuery );
}

void CDbManager::ChangeItemUpdate( CQuery* pQuery )
{
	char szSQL[4096] = {0,};
	
	FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'D1'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		FLERROR_LOG( PROGRAM_NAME, szSQL );
		return;		
	}

	for( SpecItemIdItr it = g_xSpecManager->m_SpecItemIdMap.begin(); it != g_xSpecManager->m_SpecItemIdMap.end(); ++it )
	{
		PT_ITEM_SPEC pItemProp =  &it->second;
		if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL && pItemProp->dwID != 0 )
		{
			TCHAR szName[64];
			FLStrcpy( szName, _countof( szName ), pItemProp->szName );
			FLStrConvertChars( szName, "'", " " );

			FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'I1', %d,'%s','%s'," // 0
				"%d,%d,%d,%d,%d,"	// 1
				"%d,%d,%d,%d,%d,"	// 2
				"%d,%d,%d,%d,%d,"	// 3
				"%d,%d,%d,%d,%d,"	// 4
				"%d,%d,%d,%d,%d,"	// 5
				"%d,%d,%d,%d,%d,"	// 6
				"%d,%d,%d,%d,%d,"	// 7
				"%d,%d,%f,%d,%d,"	// 8
				"%d,%d,%d,%d,%d,"	// 9
				"%d,%d,%d,%d,%d,"	// 10
				"%d,%d,%d,%d,%d,"	// 11
				"%d,%d,%d,%d,%d,"	// 12
				"%d,%d,%d,%d,%d,"	// 13
				"%d,%d,%d,%d,%d,"	// 14
				"%d,%d,%d,%d,%d,"	// 15
				"%d,%d,%d,%d,%d,"	// 16
				"%d,%d,%d,%d,%d,"	// 17
				"%d,  %f,  %d,%d,%d,%d,%d,  %d,  '%s'",

				pItemProp->dwID, szName, szName, // 0
				pItemProp->dwNum, pItemProp->dwPackMax, pItemProp->dwItemKind1, pItemProp->dwItemKind2, pItemProp->dwItemKind3, // 1
				pItemProp->dwItemJob, pItemProp->bPermanence, pItemProp->dwUseable, pItemProp->dwItemSex, 0,	// 2
				pItemProp->dwCost, pItemProp->bCharged, pItemProp->dwEndurance, 0, 0,		// 3
				pItemProp->nAbrasion, pItemProp->nMaxRepair,
				pItemProp->dwParts, pItemProp->dwItemLV, 0,		// 4
				pItemProp->dwItemRare, pItemProp->nLog, 0, 0, pItemProp->dwAbilityMin, // 5
				pItemProp->dwAbilityMax, pItemProp->nAddSkillMin, pItemProp->nAddSkillMax, pItemProp->dwWeaponType, pItemProp->tmContinuousPain, // 6
				//pItemProp->dwShellQuantity, pItemProp->dwRecoil, pItemProp->dwCaliber, pItemProp->dwBarrageRate, pItemProp->dwLoadingTime, 
				pItemProp->dwRecoil, pItemProp->dwRecoil, 0, 0, pItemProp->dwLoadingTime, // 7
				pItemProp->nAdjHitRate, 0, pItemProp->fAttackSpeed, pItemProp->dwDmgShift, pItemProp->dwAttackRange, // 8
				0, pItemProp->nProbability, pItemProp->dwDestParam[0], pItemProp->dwDestParam[1], pItemProp->nAdjParamVal[0], // 9
				pItemProp->nAdjParamVal[1], pItemProp->dwChgParamVal[0], pItemProp->dwChgParamVal[1], 0, 0, // 10
				0, 0, 0, pItemProp->dwReqMp, pItemProp->dwReqFp, // 11
				0, pItemProp->dwReqDisLV, pItemProp->dwSkillReadyType, pItemProp->dwSkillReady, pItemProp->_dwSkillRange, // 12
				pItemProp->dwCircleTime, pItemProp->dwSkillTime, 0, pItemProp->dwExeTarget, pItemProp->dwUseChance, // 13
				pItemProp->dwSpellRegion, pItemProp->dwSpellType, pItemProp->dwSkillType, 0, pItemProp->nItemResistElecricity, // 14
				0, pItemProp->nItemResistFire, pItemProp->nItemResistWind, pItemProp->nItemResistWater, pItemProp->nItemResistEarth, // 15
				0, 0, pItemProp->nEvildoing, 0, 0, // 16
				pItemProp->dwExpertLV, pItemProp->dwExpertMax, pItemProp->dwSubDefine, pItemProp->dwExp, 0, // 17
				0, 
				pItemProp->fFlightSpeed, 
				pItemProp->dwFlightLimit, 0, pItemProp->nMinLimitLevel, 0, 0, 
				pItemProp->nReflect, 
				""
				);
			if( FALSE == pQuery->Exec( szSQL ) )
			{
				FLERROR_LOG( PROGRAM_NAME, szSQL );
				return;
			}
		}	// if
	}	// for
	FLTRACE_LOG( PROGRAM_NAME, _T( "ChangeItemUpdate Complete" ) );
}

void CDbManager::ChangeSkillUpdate( CQuery* pQuery )
{
	char szSQL[1024] = {0,};
	
	FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'D2'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		FLERROR_LOG( PROGRAM_NAME, szSQL );
		return;		
	}
	
	for( int i = 0; i < prj.m_aPropSkill.GetSize(); i++ )
	{
		SkillProp* pSkillProp =  prj.GetSkillProp( i );
		if( pSkillProp && pSkillProp->dwID != 0 )
		{
			TCHAR szSkillName[64];
			FLStrcpy( szSkillName, _countof( szSkillName ), pSkillProp->szName );
			FLStrConvertChars( szSkillName, "'", " " );

			FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'I2', %d,'%s','',%d",
				pSkillProp->dwID, szSkillName, pSkillProp->dwSkillKind2 );
			if( FALSE == pQuery->Exec( szSQL ) )
			{
				FLERROR_LOG( PROGRAM_NAME, szSQL );
				return;
			}
		}
	}

	FLTRACE_LOG( PROGRAM_NAME, _T( "ChangeSkillUpdate Complete" ) );
}

void CDbManager::ChangeMonsterUpdate( CQuery* pQuery )
{
	char szSQL[1024]	= {0,};
	
	FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'D3'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		FLERROR_LOG( PROGRAM_NAME, szSQL );
		return;		
	}

	for( int i = 0; i < prj.m_nMoverPropSize; i++ )
	{
		MoverProp* pMoverProp	= prj.m_pPropMover + i;
		if( pMoverProp->dwID != 0 && pMoverProp->dwAI != AII_NONE && pMoverProp->dwAI != AII_MOVER	)
		{
			//	mulcom	BEGIN100406	ITEM_STR I3 MoverName quotation mark 제거
			TCHAR szMoverName[64];
			FLStrcpy( szMoverName, _countof( szMoverName ), pMoverProp->szName );
			FLStrConvertChars( szMoverName, "'", " " );

			FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'I3', %d,'%s','',%d",
				pMoverProp->dwID, szMoverName, pMoverProp->dwLevel );
			//	mulcom	END100406	ITEM_STR I3 MoverName quotation mark 제거

			
			if( FALSE == pQuery->Exec( szSQL ) )
			{
				FLERROR_LOG( PROGRAM_NAME, szSQL );
				return ;
			}
		}
	}
	
	FLTRACE_LOG( PROGRAM_NAME, _T( "ChangeMonsterUpdate Complete" ) );
}


void CDbManager::ChangeQuestUpdate( CQuery* pQuery )
{
	char szSQL[1024] = {0,};
	
	FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'D4'" );
	
	if( FALSE == pQuery->Exec( szSQL ) )
	{
		FLERROR_LOG( PROGRAM_NAME, szSQL );
		return ;		
	}

	for( INT2STRING::iterator i = m_int2StrItemUpdate.begin(); i != m_int2StrItemUpdate.end(); ++i )
	{
		CString	strQuest	= (*i).second.c_str();
		strQuest.Replace( "'", " " );
		FLSPrintf( szSQL, _countof( szSQL ), "ITEM_STR 'I4', %d,'%s'",
			(*i).first, strQuest );
		
		if( FALSE == pQuery->Exec( szSQL ) )
		{
			FLERROR_LOG( PROGRAM_NAME, szSQL );
			return ;
		}
	}

	FLTRACE_LOG( PROGRAM_NAME, _T( "ChangeQuestUpdate Complete" ) );
}

void CDbManager::ChangeJobNameUpdate( CQuery* /*pQuery*/ )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "ChangeJobNameUpdate Complete" ) );
}

void CDbManager::ChangeSexUpdate( CQuery* /*pQuery*/ )
{
//	SEX_MALE, SEX_FEMALE, SEX_SEXLESS
	FLTRACE_LOG( PROGRAM_NAME, _T( "ChangeSexUpdate Complete" ) );
}

