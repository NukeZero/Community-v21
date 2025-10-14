// TextCmdMain.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#ifdef __CLIENT
#include "AppDefine.h"
#endif
#include "FuncTextCmd.h"
#include "WorldMng.h"

#ifdef __CLIENT
#include "WndAdminCreateItem.h"
#include "WndIndirectTalk.h"
#include "WndChangeFace.h"
#endif

#ifdef __WORLDSERVER
#include "User.h"
#include "UserMacro.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
extern	CUserMng	g_UserMng;
extern CWorldMng g_WorldMng;
#ifdef __SCHOOL0701
#include "eveschool.h"
#endif	// __SCHOOL0701
#endif	// __WORLDSERVER

#ifdef __CLIENT
#ifdef __ONLINE
#include "dpclient.h"
extern CDPClient	g_DPlay;
#endif
#endif

#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "party.h"
#ifdef __WORLDSERVER
extern	CPartyMng	g_PartyMng;
#ifdef __CLIENT
extern	CParty g_Party;
#endif // __CLITEM
#endif // __WORLDSERVER


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif    

#define TCM_CLIENT 0
#define TCM_SERVER 1
#define TCM_BOTH   2


#define	strrtrim	StringTrimRight
/*
struct TextCmdFunc
{
	BOOL (*m_pFunc)( CScanner& scanner );
	TCHAR* m_pCommand;  
	TCHAR* m_pAbbreviation;  
	TCHAR* m_pKrCommand;  
	TCHAR* m_pKrAbbreviation;  
	BOOL  m_bServer;
	DWORD m_dwAuthorization;
};
*/
#define BEGINE_TEXTCMDFUNC_MAP TextCmdFunc m_textCmdFunc[] = {
#define END_TEXTCMDFUNC_MAP 0, 0, 0, 0, 0, 0, AUTH_GENERAL, 0 };
#define ON_TEXTCMDFUNC( a, b, c, d, e, f, g, h ) a, b, c, d, e, f, g, h,

BOOL TextCmd_CommandList( CScanner& scanner )  
{ 
#ifdef __CLIENT
#endif
	return TRUE;
}

BOOL TextCmd_Open( CScanner& scanner )  
{ 
#ifdef __CLIENT
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	g_WndMng.CreateApplet( dwIdApplet );
#endif
	return TRUE;
}
BOOL TextCmd_Close( CScanner& scanner )  
{ 
#ifdef __CLIENT
	scanner.GetToken();
	DWORD dwIdApplet = g_WndMng.GetAppletId( scanner.token );
	CWndBase* pWndBase = g_WndMng.GetWndBase( dwIdApplet );
	if( pWndBase ) pWndBase->Destroy();
#endif
	return TRUE;
}
BOOL TextCmd_To( CScanner& scanner )  
{ 
	return TRUE;
}
BOOL TextCmd_Maximize( CScanner& scanner )  
{ 
	return TRUE;
}
BOOL TextCmd_Minimize( CScanner& scanner )  
{ 
	return TRUE;
}
BOOL TextCmd_Time( CScanner& scanner )  
{ 
#ifdef __CLIENT
	CString string;
	CTime time = CTime::GetCurrentTime();
	//time.Get
	string = time.Format( "Real Time - %H:%M:%S" );
	g_WndMng.PutString( string );
	string.Format( "Madrigal Time - %d:%d:%d\n", g_GameTimer.m_nHour, g_GameTimer.m_nMin, g_GameTimer.m_nSec );
	g_WndMng.PutString( string );
	
#endif
	return TRUE;
}
#ifdef __VCONSTANT
BOOL TextCmd_ItemDropRate( CScanner& scanner )
{
//#ifndef __JAPAN__
//	return TRUE;
//#endif	// __JAPAN__
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();

#ifdef __S1012_SHOPCOST
	int nAllServer = scanner.GetNumber();
	
	if( f > 20.0f )
		f = 1.0f;
//		f = 20.0f;
	else 
	if( f < 0.1f )
		f = 1.0f;
//		f = 0.1f;

	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_ITEMDROP );
		return TRUE;
	}
	
	prj.m_fItemDropRate = f;
	
	g_UserMng.AddGameRate( prj.m_fItemDropRate, GAME_RATE_ITEMDROP );
#else // __S1012_SHOPCOST
	if( scanner.tok != FINISHED )
		prj.m_fItemDropRate = f;
	if( prj.m_fItemDropRate >= 20.0f )
		prj.m_fItemDropRate = 1.0f;
//		prj.m_fItemDropRate = 20.0f;
	if( prj.m_fItemDropRate <= 0.1f )
		prj.m_fItemDropRate = 1.0f;
//		prj.m_fItemDropRate = 0.1f;
	char chMessage[128] = {0,};
	sprintf( chMessage, "Set ItemDropRate (0.1 ~ 20.0) : %f", prj.m_fItemDropRate );
	pUser->AddText( chMessage );
#endif // __S1012_SHOPCOST
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_GoldDropRate( CScanner& scanner )
{
//#ifndef __JAPAN__
//	return TRUE;
//#endif	// __JAPAN__

#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();
	
#ifdef __S1012_SHOPCOST
	int nAllServer = scanner.GetNumber();
	
	if( f > 5.0f )
		f = 1.0f;
//		f = 5.0f;
	else 
	if( f < 0.1f )
		f = 1.0f;
//		f = 0.1f;
	
	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_GOLDDROP );
		return TRUE;
	}
	
	prj.m_fGoldDropRate = f;
	
	g_UserMng.AddGameRate( prj.m_fGoldDropRate, GAME_RATE_GOLDDROP );
#else // __S1012_SHOPCOST
	if( scanner.tok != FINISHED )
		prj.m_fGoldDropRate = f;
	if( prj.m_fGoldDropRate >= 5.0f )
		prj.m_fGoldDropRate = 1.0f;
//		prj.m_fGoldDropRate = 5.0f;
	if( prj.m_fGoldDropRate <=0.1f )
		prj.m_fGoldDropRate = 1.0f;
//		prj.m_fGoldDropRate = 0.1f;
	char chMessage[128] = {0,};
	sprintf( chMessage, "Set GoldDropRate (0.1 ~ 5.0) : %f", prj.m_fGoldDropRate );
	pUser->AddText( chMessage );
#endif // __S1012_SHOPCOST
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_MonsterExpRate( CScanner& scanner )
{
//#ifndef __JAPAN__
//	return TRUE;
//#endif	// __JAPAN__

#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();

#ifdef __S1012_SHOPCOST
	int nAllServer = scanner.GetNumber();
	
	if( f > 50.0f )
		f = 1.0f;
//		f = 50.0f;
	else 
	if( f < 0.1f )
		f = 1.0f;
//		f = 0.1f;
	
	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_MONSTEREXP );
		return TRUE;
	}
	
	prj.m_fMonsterExpRate = f;
	g_UserMng.AddGameRate( prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP );

#else // __S1012_SHOPCOST
	if( scanner.tok != FINISHED )
		prj.m_fMonsterExpRate = f;
	if( prj.m_fMonsterExpRate >= 50.0f )
		prj.m_fMonsterExpRate = 1.0f;
//		prj.m_fMonsterExpRate = 50.0f;
	if( prj.m_fMonsterExpRate <= 0.1f )
		prj.m_fMonsterExpRate = 1.0f;
//		prj.m_fMonsterExpRate = 0.1f;
	char chMessage[128] = {0,};
	sprintf( chMessage, "Set MonsterExpRate (0.1 ~ 20.0) : %f", prj.m_fMonsterExpRate );
	pUser->AddText( chMessage );
#endif // __S1012_SHOPCOST
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_MonsterHitRate( CScanner& scanner )
{
//#ifndef __JAPAN__
//	return TRUE;
//#endif	// __JAPAN__
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();

#ifdef __S1012_SHOPCOST
	int nAllServer = scanner.GetNumber();
	
	if( f > 5.0f )
		f = 1.0f;
//		f = 5.0f;
	else 
	if( f < 0.1f )
		f = 1.0f;
//		f = 0.1f;
	
	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_MONSTERHIT );
		return TRUE;
	}
	
	prj.m_fMonsterHitRate = f;
	g_UserMng.AddGameRate( prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT );
		
#else // __S1012_SHOPCOST
	if( scanner.tok != FINISHED )
		prj.m_fMonsterHitRate = f;
	if( prj.m_fMonsterHitRate >= 5.0f )
		prj.m_fMonsterHitRate = 1.0f;
//		prj.m_fMonsterHitRate = 5.0f;
	if( prj.m_fMonsterHitRate <= 0.1f )
		prj.m_fMonsterHitRate = 1.0f;
//		prj.m_fMonsterHitRate = 0.1f;
	char chMessage[128] = {0,};
	sprintf( chMessage, "Set MonsterHitRate (0.1 ~ 5.0) : %f", prj.m_fMonsterHitRate );
	pUser->AddText( chMessage );
#endif // __S1012_SHOPCOST
#endif	// __WORLDSERVER
	return TRUE;
}
#endif




BOOL TextCmd_X3( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

#ifdef __S1012_SHOPCOST
	FLOAT f = scanner.GetFloat();
	int nAllServer = scanner.GetNumber();
	
	if( f > 5.0f )
		f = 1.0f;
//		f = 5.0f;
	else 
	if( f < 0.1f )
		f = 1.0f;
//		f = 0.1f;
	
	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_X3 );
		return TRUE;
	}
	
	prj.m_fItemDropRate = f;
	prj.m_fGoldDropRate = f;
	prj.m_fMonsterExpRate = f;
	g_UserMng.AddGameRate( f, GAME_RATE_X3 );

#else // __S1012_SHOPCOST
#ifdef __VCONSTANT	
	//int n = scanner.GetNumber();
	FLOAT f = scanner.GetFloat();
	if( f <= 0)
	{
		prj.m_fItemDropRate = 1.0f;
		prj.m_fGoldDropRate = 1.0f;
		prj.m_fMonsterExpRate = 1.0f;
		//prj.m_fMonsterHitRate = 1.0f;
	}
	else
	if( f > 4 )
	{
//		prj.m_fItemDropRate = 4.0f;
//		prj.m_fGoldDropRate = 4.0f;
//		prj.m_fMonsterExpRate = 4.0f;
		prj.m_fItemDropRate = 1.0f;
		prj.m_fGoldDropRate = 1.0f;
		prj.m_fMonsterExpRate = 1.0f;
		//prj.m_fMonsterHitRate = 1.0f;
	}
	else
	{
		prj.m_fItemDropRate = f;
		prj.m_fGoldDropRate = f;
		prj.m_fMonsterExpRate = f;
		//prj.m_fMonsterHitRate = f;
	}
	char chMessage[128] = {0,};
	sprintf( chMessage, prj.GetText(TID_GAME_APPLYGAMEENI), (int)f );
//	sprintf( chMessage, "경험치, 페냐, 아이템드롭 확률이 %d으로 조정되었습니다(1~4)", g_fX3 );
	pUser->AddText( chMessage );

#else
	g_fX3 = scanner.GetNumber();
	if( g_fX3 <= 0)
		g_fX3 = 1;
	if( g_fX3 > 4 )
		g_fX3 = 1;
//		g_fX3 = 4;
	char chMessage[128] = {0,};
	sprintf( chMessage, prj.GetText(TID_GAME_APPLYGAMEENI), g_fX3 );
//	sprintf( chMessage, "경험치, 페냐, 아이템드롭 확률이 %d으로 조정되었습니다(1~4)", g_fX3 );
	pUser->AddText( chMessage );

#endif
#endif // __S1012_SHOPCOST
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __INSTANCE_DUNGEON
BOOL TextCmd_Runeq( CScanner & s )
{
#ifdef __WORLDSERVER
	DWORD dwWorldId		= s.GetNumber();
	LPWORLD lpWorld	= g_WorldMng.GetWorldStruct( dwWorldId );
	if( lpWorld )
		g_DPCoreClient.SendRuneq( dwWorldId );
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __INSTANCE_DUNGEON

#ifdef __S1012_SHOPCOST
BOOL TextCmd_ChangeShopCost( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	FLOAT f = scanner.GetFloat();
	int nAllServer = scanner.GetNumber();

	if( f > 2.0f )
		f = 1.0f;
//		f = 2.0f;
	else 
	if( f < 0.5f )
		f = 1.0f;
//		f = 0.5f;

	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendGameRate( f, GAME_RATE_SHOPCOST );
		return TRUE;
	}

	prj.m_fShopCost = f;
	
	g_UserMng.AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_SetMonsterRespawn( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );

	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 30 ) dwNum = 30;
		if( dwNum < 1 ) dwNum = 1;

		DWORD dwAttackNum	= scanner.GetNumber();
		if( dwAttackNum > dwNum ) dwAttackNum = dwNum;
		if( dwAttackNum < 1 ) dwAttackNum = 0;

		DWORD dwRect = scanner.GetNumber();
		if( dwRect > 255 ) dwRect = 255;
		if( dwRect < 1 ) dwRect = 1;

		DWORD dwTime = scanner.GetNumber();
		if( dwTime > 10800 ) dwTime = 10800;
		if( dwTime < 10 ) dwTime = 10;

		int nAllServer = scanner.GetNumber();
		if( nAllServer != 0 )
		{
			BOOL bFlying = FALSE;
			if( pMoverProp->dwFlying )
				bFlying = TRUE;
			g_DPCoreClient.SendSetMonsterRespawn( pUser->m_idPlayer, pMoverProp->dwID, dwNum, dwAttackNum, dwRect, dwTime, bFlying );
			return TRUE;
		}

		g_WorldMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = pMoverProp->dwID;
		ri.m_cb = dwNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if( pMoverProp->dwFlying != 0 )
			ri.m_vPos = vPos;
		ri.m_rect.left = vPos.x - dwRect;
		ri.m_rect.right = vPos.x + dwRect;
		ri.m_rect.top = vPos.z - dwRect;
		ri.m_rect.bottom = vPos.z + dwRect;
		ri.m_uTime = dwTime;
		ri.m_cbTime = 0;

		char chMessage[512] = {0,};
#ifdef __S1108_BACK_END_SYSTEM
			pWorld->m_respawner.Add( ri, TRUE );
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add( ri );
#endif // __S1108_BACK_END_SYSTEM
		g_WorldMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1

		sprintf( chMessage, "Add Respwan Monster : %s(%d/%d) Rect(%d, %d, %d, %d) Time : %d", 
			pMoverProp->szName, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime );
		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif // __S1012_SHOPCOST
#ifdef __S1108_BACK_END_SYSTEM
BOOL TextCmd_RespawnMonster( CScanner & scanner )
{
	/*
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	CWorld* pWorld	= pUser->GetWorld();
	CRespawnInfo* aRespawnInfo[MAX_RESPAWN];
	g_WorldMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
	int nResSize = pWorld->m_respawner.m_nGMRespawnSize;
	for( int i  = 0 ; i < pWorld->m_respawner.m_nGMRespawnSize ; ++i )
	{
#ifdef __V050322_CACHEQUEST
		aRespawnInfo[i] = pWorld->m_respawner.GetRespawnInfo( pWorld->m_respawner.m_nGMRespawn[i], TRUE );
#else
		aRespawnInfo[i] = pWorld->m_respawner.GetRespawnInfo( pWorld->m_respawner.m_nGMRespawn[i] );
#endif
		if( aRespawnInfo[i] == NULL )
		{
			pUser->AddText( "Error RespawnMonster" );
			g_WorldMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1	
			return FALSE;
		}
	}
	g_WorldMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1	
	MoverProp* pMoverProp = NULL;
	for( i = 0 ; i < nResSize ; ++i )
	{
		 pMoverProp = prj.GetMoverPropEx( aRespawnInfo[i]->m_dwIndex );
		 if( pMoverProp )
		 {
			 char chMessage[512] = {0,};
			 sprintf( chMessage, "Respwan Monster : Id(%d) %s(%d) Rect(%d, %d, %d, %d) Time : %d", 
				 pWorld->m_respawner.m_nGMRespawn[i], pMoverProp->szName, aRespawnInfo[i]->m_cbRespawn, aRespawnInfo[i]->m_rect.left, aRespawnInfo[i]->m_rect.right, aRespawnInfo[i]->m_rect.top, aRespawnInfo[i]->m_rect.bottom, aRespawnInfo[i]->m_uTime );
			 pUser->AddText( chMessage );
		 }
	}
	
	if( nResSize == 0 )
	{
		char chMessage[512] = {0,};
		sprintf( chMessage, "Respwan Monster Not Data" );
		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	*/
	return TRUE;
}
BOOL TextCmd_RemoveRespawnMonster( CScanner & scanner )
{
	/*
#ifdef __WORLDSERVER
	int nRespawnID = scanner.GetNumber();

	CUser* pUser	= (CUser*)scanner.dwValue;
	CWorld* pWorld	= pUser->GetWorld();

	CRespawnInfo* aRespawnInfo = NULL;
	BOOL bResult = FALSE;
	char chMessage[512] = {0,};
	g_WorldMng.m_AddRemoveLock.Enter( theLineFile );	// lock1
	for( int i  = 0 ; i < pWorld->m_respawner.m_nGMRespawnSize ; ++i )
	{
		if( nRespawnID == pWorld->m_respawner.m_nGMRespawn[i] )
		{
			bResult = TRUE;
			break;
		}
	}
	if( bResult )
	{
#ifdef __V050322_CACHEQUEST
		aRespawnInfo = pWorld->m_respawner.GetRespawnInfo( nRespawnID, TRUE );
#else
		aRespawnInfo = pWorld->m_respawner.GetRespawnInfo( nRespawnID );
#endif
		if( aRespawnInfo != NULL )
		{
			CRespawnInfo aRespawnInfoBuf;
			aRespawnInfoBuf.m_cbRespawn = aRespawnInfo->m_cb;
			aRespawnInfoBuf.m_rect.left = aRespawnInfo->m_rect.left;
			aRespawnInfoBuf.m_rect.right = aRespawnInfo->m_rect.right;
			aRespawnInfoBuf.m_rect.top = aRespawnInfo->m_rect.top;
			aRespawnInfoBuf.m_rect.bottom = aRespawnInfo->m_rect.bottom;
			aRespawnInfoBuf.m_uTime = aRespawnInfo->m_uTime;
			MoverProp* pMoverProp = NULL;
			int nGMRespawn = pWorld->m_respawner.m_nGMRespawn[nRespawnID];
			pMoverProp = prj.GetMoverPropEx( aRespawnInfoBuf.m_dwIndex );
			if( pWorld->m_respawner.Remove( nRespawnID, TRUE ) )
			{
				sprintf( chMessage, "Respwan Monster : Id(%d) %s(%d) Rect(%d, %d, %d, %d) Time : %d", 
					nGMRespawn, pMoverProp->szName, aRespawnInfoBuf.m_cbRespawn, aRespawnInfoBuf.m_rect.left, aRespawnInfoBuf.m_rect.right, aRespawnInfoBuf.m_rect.top, aRespawnInfoBuf.m_rect.bottom, aRespawnInfoBuf.m_uTime );
				pUser->AddText( chMessage );
			}
			else
			{
				bResult = FALSE;
			}
		}
		else
		{
			bResult = FALSE;
		}
	}
	g_WorldMng.m_AddRemoveLock.Leave( theLineFile );	// unlock1	

	if( bResult == FALSE )
	{
		sprintf( chMessage, "Not RemoveRespawnMonster ID : %d ", nRespawnID );
		pUser->AddText( chMessage );
	}
#endif	// __WORLDSERVER
	*/
	return TRUE;
}

BOOL TextCmd_PropMonster( CScanner & scanner )
{
#ifdef __CLIENT
	char chMessage[1024] = {0,};
	if( 0 < prj.m_nAddMonsterPropSize )
	{
		for( int i = 0 ; i < prj.m_nAddMonsterPropSize ; ++i )
		{
			sprintf( chMessage, "Monster Prop(%s) AttackPower(%d), Defence(%d), Exp(%d), Hitpioint(%d), ItemDorp(%d), Penya(%d)", 
				prj.m_aAddProp[i].szMonsterName, prj.m_aAddProp[i].nAttackPower, prj.m_aAddProp[i].nDefence, prj.m_aAddProp[i].nExp,
				prj.m_aAddProp[i].nHitPoint, prj.m_aAddProp[i].nItemDrop, prj.m_aAddProp[i].nPenya	);
			g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
		}
	}
	else
	{
		sprintf( chMessage, "Monster Prop Not Data" );
		g_WndMng.PutString( chMessage, NULL, 0xffff0000, CHATSTY_GENERAL );
	}
#endif	// __CLIENT
	return TRUE;
}
#endif // __S1108_BACK_END_SYSTEM

BOOL TextCmd_ItemEvent( CScanner & scanner )
{
#ifdef __WORLDSERVER
#ifdef __S0420
	CUser* pUser	= (CUser*)scanner.dwValue;

	DWORD dwStartTime = scanner.GetNumber();	// 시작 날짜/시간 : 2004102103( 년월일시 )
	DWORD dwEndTime = scanner.GetNumber();		// 끝날 날짜/시간 : 2004102104( 년월일시 )
	int nAllServer = scanner.GetNumber();		// 멀티서버 모두 적용

	BOOL bResult = TRUE;

	int nStartYear, nStartMonth, nStartDay, nStartHour;
	int nEndYear, nEndMonth, nEndDay, nEndHour;
	if( 2000000000 < dwStartTime && 2000000000 < dwEndTime )
	{
		int nBuf = 0;
		nStartYear = dwStartTime / 1000000;
		nBuf = (dwStartTime % 1000000);
		nStartMonth = nBuf / 10000;
		nBuf = nBuf % 10000;
		nStartDay = nBuf / 100;
		nBuf = nBuf % 100;
		nStartHour = nBuf;
		
		nEndYear = dwEndTime / 1000000;
		nBuf = (dwEndTime % 1000000);
		nEndMonth = nBuf / 10000;
		nBuf = nBuf % 10000;
		nEndDay = nBuf / 100;
		nBuf = nBuf % 100;
		nEndHour = nBuf;
		
		
		if( nStartYear < 2004 || nEndYear < 2004 || 2020 < nStartYear || 2020 < nEndYear )
			bResult = FALSE;
		if( nStartMonth < 0 || nEndMonth < 0 || 12 < nStartMonth || 12 < nEndMonth )
			bResult = FALSE;
		if( nStartDay < 0 || nEndDay < 0 || 31 < nStartDay || 31 < nEndDay )
			bResult = FALSE;
		if( nStartHour < 0 || nEndHour < 0 || 24 < nStartHour || 24 < nEndHour )
			bResult = FALSE;
	}
	else
	{
		bResult = FALSE;
	}

	if( bResult )
	{
		if( nAllServer != 0 )
		{
			g_DPCoreClient.SendSetItemEvent( pUser->m_idPlayer, nStartYear, nStartMonth, nStartDay, nStartHour, nEndYear, nEndMonth, nEndDay, nEndHour );
			return TRUE;
		}
		else
		{
			prj.m_tReNewEventStartTime = CTime::CTime( nStartYear, nStartMonth, nStartDay, nStartHour, 0, 0 );
			prj.m_tReNewEventEndTime = CTime::CTime( nEndYear, nEndMonth, nEndDay, nEndHour, 0, 0 );
			char chMessage[512] = {0,};
			sprintf( chMessage, "SetItemEvent %d/%d/%d %d ~ %d/%d/%d %d" , 
				prj.m_tReNewEventStartTime.GetYear(), prj.m_tReNewEventStartTime.GetMonth(), prj.m_tReNewEventStartTime.GetDay(), prj.m_tReNewEventStartTime.GetHour(),
				prj.m_tReNewEventEndTime.GetYear(), prj.m_tReNewEventEndTime.GetMonth(), prj.m_tReNewEventEndTime.GetDay(), prj.m_tReNewEventEndTime.GetHour() );
			pUser->AddText( chMessage );
		}
	}
	else
	{
		char chMessage[512] = {0,};
		sprintf( chMessage, "ItemEvent Error : ReTry" );
		pUser->AddText( chMessage );
	}



/*	prj.m_tReNewEventEndTime = CTime::CTime( dwStartTime / 1000000, (dwStartTime % 1000000) / 10000, ((dwStartTime % 1000000) % 10000 ), 0, 0, 0 );

	if( nAllServer != 0 )
	{
		g_DPCoreClient.SendSetItemEvent( f, GAME_RATE_SHOPCOST );
		return TRUE;
	}
	
	prj.m_fShopCost = f;
	
	g_UserMng.AddGameRate( prj.m_fShopCost, GAME_RATE_SHOPCOST );
*/
#endif // __S0420
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_GameSetting( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->AddGameSetting();	
#endif // __WORLDSERVER
	return TRUE;	
}

#ifdef __S1020_CHANGEFACE
BOOL TextCmd_ChangeFace( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;

	DWORD dwFace = scanner.GetNumber();
	if( dwFace < 0 || 4 < dwFace )
		return TRUE;
	
	if( (pUser->m_dwMode & NOTFRESH_MODE) || (pUser->m_dwMode & NOTFRESH_MODE2) )
	{
		pUser->m_dwHeadMesh = dwFace;
		g_UserMng.AddChangeFace( pUser->m_idPlayer, dwFace );
		if( pUser->m_dwMode & NOTFRESH_MODE )
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE;
			pUser->AddDefinedText( TID_CHANGEFACE_ONE, "" );
//			pUser->AddText( "얼굴변경을 한번 사용하였습니다" );
		}
		else
		{
			pUser->m_dwMode &= ~NOTFRESH_MODE2;
			pUser->AddDefinedText( TID_CHANGEFACE_TWO, "" );
//			pUser->AddText( "얼굴변경을 2번 사용하였습니다" );
		}
	}
	else
	{
		pUser->AddDefinedText( TID_CHANGEFACE_THREE, "" );
//		pUser->AddText( "얼굴변경을 2번 모두 사용하여 사용할수 없습니다" );
	}
#else // __WORLDSERVER
	CWndChangeSex* pWndChangeSex	= (CWndChangeSex*)g_WndMng.GetWndBase( APP_CHANGESEX );
	if( NULL == pWndChangeSex )
	{
		pWndChangeSex	= new CWndChangeSex;
		pWndChangeSex->Initialize();
	}
	pWndChangeSex->SetData( NULL_ID, NULL_ID );
	return FALSE;
#endif
	return TRUE;
}
#endif // __S1020_CHANGEFACE


BOOL TextCmd_AroundKill( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
//#ifdef __DEV
//	pUser->Lock();
	if( pUser->GetWeaponItem() == NULL )
		return TRUE;

	CWorld* pWorld	= pUser->GetWorld();
	if( pWorld )
	{
		prj.m_AddRemoveLock.Enter( theLineFile );
		pWorld->m_AddRemoveLock.Enter( theLineFile );
		pWorld->m_csModifyLink.Enter( theLineFile );
		pUser->SendDamageAround( AF_MAGICSKILL, (CMover*)pUser, OBJTYPE_MONSTER, 1, 3, 0.0, 1.0f );
		pWorld->m_csModifyLink.Leave( theLineFile );
		pWorld->m_AddRemoveLock.Leave( theLineFile );
		prj.m_AddRemoveLock.Leave( theLineFile );
	}
//	pUser->Unlock();
//#endif	// __DEV
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Level( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetToken();
	CString strJob = scanner.Token;

	int nJob = JOB_VAGRANT;

	LONG nLevel = scanner.GetNumber();

	for( int i = 0 ; i < MAX_JOB ; i++ )
	{
		if( strcmp( strJob, prj.m_aJob[i].szName ) == 0 || strcmp( strJob, prj.m_aJob[i].szEName ) == 0 )
		{
			nJob = i;
			break;
		}
	}

#ifdef __11_MONTH_JOB_PRO
	char chMessage[MAX_PATH] = {0,};
	if( MAX_JOB_LEVEL < nLevel && nJob == 0 )
	{
		sprintf( chMessage, prj.GetText(TID_GAME_CHOICEJOB) );
		pUser->AddText( chMessage );		
		return TRUE;
	}
	
	if( nLevel <= MAX_JOB_LEVEL )	
	{
		pUser->InitLevel( JOB_VAGRANT, nLevel );	// lock
	}
	else
	if( MAX_JOB_LEVEL < nLevel &&  nLevel <= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
	{
		if( MAX_JOBBASE <= nJob && nJob < MAX_EXPERT)
		{
			pUser->InitLevel( nJob, nLevel );	// lock
		}
		else
		{
			sprintf( chMessage, "Not Expert Job" );
			pUser->AddText( chMessage );
			sprintf( chMessage, "Expert Job : " );
			for( int i = MAX_JOBBASE ; i < MAX_EXPERT ; ++i )
			{
				if( strlen( prj.m_aJob[i].szName ) < 15 )
				{
					strcat( chMessage, prj.m_aJob[i].szName );
					if( i + 1 != MAX_EXPERT )
					{
						strcat( chMessage, ", ");
					}
				}
			}
			pUser->AddText( chMessage );
			sprintf( chMessage, "Expert Level : %d ~ %d", MAX_JOB_LEVEL + 1, MAX_JOB_LEVEL + MAX_EXP_LEVEL );
			pUser->AddText( chMessage );
			return TRUE;
		}
	}
	else
	if( MAX_JOB_LEVEL + MAX_EXP_LEVEL < nLevel && nLevel < MAX_LEVEL + 1 )
	{
		if( MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL )
		{
			pUser->InitLevel( nJob, nLevel );	// lock
		}
		else
		{
			sprintf( chMessage, "Not Professional Job" );
			pUser->AddText( chMessage );
			sprintf( chMessage, "Professional Job : " );
			for( int i = MAX_EXPERT ; i < MAX_PROFESSIONAL ; ++i )
			{
				if( strlen( prj.m_aJob[i].szName ) < 15 )
				{
					strcat( chMessage, prj.m_aJob[i].szName );
					if( i + 1 != MAX_PROFESSIONAL )
					{
						strcat( chMessage, ", ");
					}
				}
			}
			pUser->AddText( chMessage );
			sprintf( chMessage, "Professional Level : %d ~~~ ", MAX_JOB_LEVEL + MAX_EXP_LEVEL + 1 );
			pUser->AddText( chMessage );
			return TRUE;
		}
	}
#else // __11_MONTH_JOB_PRO
	if( MAX_JOB_LEVEL >= nLevel && nJob != 0 )
	{
		nJob = JOB_VAGRANT;
	}
	
	if( MAX_JOB_LEVEL < nLevel && nJob == 0 )
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, prj.GetText(TID_GAME_CHOICEJOB) );
		pUser->AddText( chMessage );		
		return TRUE;
	}
	if( 2 <= nLevel && nLevel < 100 )
	{
		pUser->InitLevel( nJob, nLevel );	// lock
	}
	else
	{
		char chMessage[128] = {0,};
		sprintf( chMessage, prj.GetText(TID_GAME_LEVELRANGE) );
		pUser->AddText( chMessage );
	}
#endif // __11_MONTH_JOB_PRO
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_stat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetToken();
	//	FILEOUT( "error2.txt", "token str %s tokentype %d \n", scanner.token,  scanner.tokenType );
	CString strstat = scanner.Token;
	
	DWORD dwNum	= scanner.GetNumber();

	if( 2 <= strstat.GetLength() && strstat.GetLength() <= 7)
	{
		strstat.MakeLower();

		if( strcmp( strstat, "str" ) == 0 )
		{
			pUser->m_nStr = dwNum;
		}
		else
		if( strcmp( strstat, "sta" ) == 0 )
		{
			pUser->m_nSta = dwNum;
		}
		else
		if( strcmp( strstat, "dex" ) == 0 )
		{
			pUser->m_nDex = dwNum;
		}
		else
		if( strcmp( strstat, "int" ) == 0 )
		{
			pUser->m_nInt = dwNum;
		}
		else
		if( strcmp( strstat, "lp" ) == 0 )
		{
			pUser->m_nRemainLP = dwNum;
		}
		else
		if( strcmp( strstat, "gp" ) == 0 )
		{
			pUser->m_nRemainGP = dwNum;
		}
#ifdef __SM_CODE_0729
		else
		if( strcmp( strstat, "restate" ) == 0 )
		{
			pUser->ReState();
			return FALSE;
		}
#endif // __SM_CODE_0729
		else
		{
			strstat += "은 설정을 할수 없습니다.";
			pUser->AddText( strstat );
			return FALSE;
		}
	}
	else
	{
		strstat += "은 설정을 할수 없습니다.";
		pUser->AddText( strstat );
		return FALSE;
	}

	pUser->AddSetState( pUser->m_nStr, pUser->m_nSta, pUser->m_nDex, pUser->m_nInt, pUser->m_nRemainLP, pUser->m_nRemainGP );
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __J0601
BOOL TextCmd_SetSnoop( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;

	s.GetToken();
	if( s.tok != FINISHED )
	{
		if( lstrcmp( pUser->GetName(), s.Token ) )
		{
			u_long idPlayer	= prj.GetPlayerID( s.Token );
			if( idPlayer > 0 )
			{
				BOOL bRelease	= FALSE;
				s.GetToken();
				if( s.tok != FINISHED )
					bRelease	= (BOOL)atoi( s.Token );
				g_DPCoreClient.SendSetSnoop( idPlayer, pUser->m_idPlayer,  bRelease );
				OutputDebugString( "WORLDSERVER.EXE\t// TextCmd_SetSnoop" );
			}
			else
				pUser->AddReturnSay( 3, s.Token );
		}
	}
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_SetSnoopGuild( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;

	s.GetToken();
	if( s.tok != FINISHED )
	{
		g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
		CGuild* pGuild	= g_GuildMng.GetGuild( s.Token );
		if( pGuild )
		{
			BOOL bRelease	= FALSE;
			s.GetToken();
			if( s.tok != FINISHED )
				bRelease	= (BOOL)atoi( s.Token );
			g_DPCoreClient.SendSetSnoopGuild( pGuild->m_idGuild, bRelease );
			OutputDebugString( "WORLDSERVER.EXE\t// TextCmd_SetSnoopGuild" );
		}
		g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	}
#endif	// __WORLDSERVER
	return FALSE;
}
#endif	// __J0601

#ifdef __J0519
BOOL TextCmd_QuerySetPlayerName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	scanner.GetToken();
	CString strPlayer	= scanner.Token;
	strPlayer.TrimLeft();
	strPlayer.TrimRight();
	OutputDebugString( "WORLDSERVER.EXE\t// TextCmd_QuerySetPlayerName" );
	g_dpDBClient.SendQuerySetPlayerName( pUser->m_idPlayer, strPlayer, MAKELONG( 0xffff, 0 ) );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}
#endif	// __J0519

#ifdef __J0524
BOOL TextCmd_QuerySetGuildName( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	scanner.GetToken();
	CString strGuild	= scanner.Token;
	strGuild.TrimLeft();
	strGuild.TrimRight();
	OutputDebugString( "WORLDSERVER.EXE\t// TextCmd_QuerySetGuildName" );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		g_DPCoreClient.SendQuerySetGuildName( pUser->m_idPlayer, pUser->m_idGuild, (LPSTR)(LPCSTR)strGuild, 0xff );
	}
	else
	{
		// is not kingpin
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}
#endif	// __J0524

#ifdef __NEWGUILD
BOOL TextCmd_CreateGuild( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	scanner.GetToken();
	GUILD_MEMBER_INFO	info;
	info.idPlayer	= pUser->m_idPlayer;
	info.nLevel	= pUser->GetLevel();
	info.nJob	= pUser->GetJob();
	info.dwSex	= pUser->GetSex();
	g_DPCoreClient.SendCreateGuild( &info, 1, scanner.Token );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_DestroyGuild( CScanner & scanner )
{
#ifdef __CLIENT
#ifdef __ONLINE
	g_DPlay.SendDestroyGuild( g_pPlayer->m_idPlayer );
#endif
	return TRUE; 
#endif	// __CLIENT
	return FALSE;
}

BOOL TextCmd_RemoveGuildMember( CScanner & scanner )
{
#ifdef __CLIENT
#ifdef __ONLINE
	scanner.GetToken();
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	lstrcpy( lpszPlayer, scanner.Token );
	u_long idPlayer	= prj.GetPlayerID( lpszPlayer );
	if( idPlayer != 0 )
		g_DPlay.SendRemoveGuildMember( g_pPlayer->m_idPlayer, idPlayer );
	return TRUE;
#endif
#endif	// __CLIENT
	return FALSE;
}

BOOL TextCmd_GuildChat( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	char sChat[1024]		= { 0, };
	scanner.GetLastFull();
	if( lstrlen( scanner.token ) > 260 )
		return TRUE;
	lstrcpy( sChat, scanner.token );
	strrtrim( sChat );
	g_DPCoreClient.SendGuildChat( pUser, sChat );
	return TRUE;
#endif	// __WORLDSERVER
	return FALSE;
}

BOOL TextCmd_DeclWar( CScanner & scanner )
{
#ifdef __GUILDWAR
#ifdef __CLIENT
	char szGuild[MAX_G_NAME]	= { 0, };
	scanner.GetLastFull();
	if( lstrlen( scanner.token ) > ( MAX_G_NAME - 1 ) )
		return TRUE;
	lstrcpy( szGuild, scanner.token );
	strrtrim( szGuild );
#ifdef __ONLINE	
	g_DPlay.SendDeclWar( g_pPlayer->m_idPlayer, szGuild );
#endif
#endif	// __CLIENT
#endif	// __GUILDWAR
	return FALSE;
}


#ifdef __H0621_GUILDRANK__
// 길드 랭킹정보를 업데이트 시키는 명령어이다.
BOOL TextCmd_GuildRanking( CScanner & scanner )
{
#ifdef __WORLDSERVER
	// TRANS 서버에게 길드 랭킹 정보가 업데이트 되야함을 알린다.
	g_dpDBClient.UpdateGuildRanking();
#endif

	return TRUE;
}

#ifdef __H0809_GUILDRANK__
//*
// 길드 랭킹정보 DB를 업데이트 시키는 명령어이다.
BOOL TextCmd_GuildRankingDBUpdate( CScanner & scanner )
{
#ifdef __WORLDSERVER
	// TRANS 서버에게 길드 랭킹 정보가 업데이트 되야함을 알린다.
	g_dpDBClient.UpdateGuildRankingUpdate();
#endif
	
	return TRUE;
}
#endif//__H0809_GUILDRANK__
#endif//__H0621_GUILDRANK__

//*/
#endif	// __NEWGUILD

BOOL TextCmd_GuildStat( CScanner & scanner )
{
#ifdef __WORLDSERVER
#ifdef __NEWGUILD
	CUser* pUser	= (CUser*)scanner.dwValue;

	scanner.GetToken();
	//	FILEOUT( "error2.txt", "token str %s tokentype %d \n", scanner.token,  scanner.tokenType );
	CString strstat = scanner.Token;
	

	strstat.MakeLower();

	if( strstat == "logo" )
	{
		DWORD dwLogo = scanner.GetNumber();
		TRACE("guild Logo:%d\n", dwLogo);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_LOGO, dwLogo );
	}
	else
	if( strstat == "pxp" )
	{
		DWORD dwPxpCount = scanner.GetNumber();
		TRACE("guild pxpCount:%d\n", dwPxpCount);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PXPCOUNT, dwPxpCount );
	}
	else
	if( strstat == "penya" )
	{
		DWORD dwPenya = scanner.GetNumber();
		TRACE("guild dwPenya:%d\n", dwPenya);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_PENYA, dwPenya);
	}
	else
	if( strstat == "notice" )
	{
		scanner.GetToken();
		TRACE("guild notice:%s\n", scanner.Token);
		g_DPCoreClient.SendGuildStat( pUser, GUILD_STAT_NOTICE, (DWORD)(LPCTSTR)scanner.Token);
	}
	else
	{
		strstat += "syntax error guild stat command.";
		pUser->AddText( strstat );
	}
#endif	//__NEWGUILD
#endif 
	return FALSE;
}

BOOL TextCmd_SetGuildQuest( CScanner & s )
{
#ifdef __GUILDQUEST0826
#ifdef __WORLDSERVER
#ifdef __NEWGUILD
	s.GetToken();
	char sGuild[MAX_G_NAME];
	lstrcpy( sGuild, s.Token );
	int nQuestId	= s.GetNumber();
	int nState		= s.GetNumber();
#ifdef __GUILDQUEST0826
	GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( nQuestId );
	if( !pProp )
		return FALSE;
#endif	// __GUILDQUEST0826
//	if( nState < QS_BEGIN || nState > QS_END )
//		return FALSE;

	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	CGuild* pGuild	= g_GuildMng.GetGuild( sGuild );
	if( pGuild )
	{
		if( nState < QS_BEGIN || nState > QS_END )
		{
#ifdef __REMOVEGUILDQUEST0302
			pGuild->RemoveQuest(nQuestId );
			g_dpDBClient.SendDeleteGuildQuest( pGuild->m_idEnemyGuild, nQuestId );
#endif	// __REMOVEGUILDQUEST0302
		}
		else
		{
			pGuild->SetQuest( nQuestId, nState );
			g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, nQuestId, nState );
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	return TRUE;
#endif	// __NEWGUILD
#endif	// __WORLDSERVER
#endif	// __GUILDQUEST0826
	return FALSE;
}

BOOL TextCmd_PartyLevel( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	DWORD dwLevel = scanner.GetNumber();
	DWORD dwPoint = scanner.GetNumber();
	if( dwPoint == 0 )
		dwPoint = 100;

	TRACE("plv LV:%d POINT:%d\n", dwLevel, dwPoint);
	
	CParty* pParty;
	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
	pParty = g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty )
	{
		pParty->SetPartyLevel( pUser, dwLevel, dwPoint, pParty->GetExp() );
	}
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
//	if (1 <= dwLevel && dwLevel <= 100)
//		g_DPCoreClient.SendPartyLevel( pUser, dwLevel, dwPoint );
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __11_MONTH_JOB_PRO
BOOL  TextCmd_InitSkillExp( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	if( pUser->InitSkillExp() == TRUE )
		pUser->AddInitSkillExp();
#endif // __WORLDSERVER
	return TRUE;
}
#endif // __11_MONTH_JOB_PRO

BOOL TextCmd_SkillLevel( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	DWORD dwSkillKind	= scanner.GetNumber();
	DWORD dwSkillLevel = scanner.GetNumber();

	LPSKILL pSkill = pUser->GetSkill( dwSkillKind );
	if( pSkill )
	{
		pSkill->dwExp = 0;
		pSkill->dwLevel = dwSkillLevel;
		ItemProp* pSkillProp = prj.GetSkillProp( pSkill->dwSkill );
		if( pSkillProp )
		{
			if( dwSkillLevel == pSkillProp->dwExpertMax )
			{
				pSkill->bStop = FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	pUser->AddSetSkill( pSkill->dwSkill, pSkill->dwLevel );
#endif // __WORLDSERVER
#ifdef __ONLINE

	DWORD dwSkillLevel = scanner.GetNumber();

#ifdef __11_MONTH_JOB_PRO
	CWndSkillTreeEx* pSkill = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL );
#else //__11_MONTH_JOB_PRO
	CWndSkill* pSkill = (CWndSkill*)g_WndMng.GetWndBase( APP_SKILL );
#endif //__11_MONTH_JOB_PRO
	if( pSkill )
	{
#ifdef __11_MONTH_JOB_PRO
		int nIndex = pSkill->GetCurSelect();
		if( nIndex == -1 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}
		LPSKILL pSkillbuf = pSkill->GetSkill( nIndex );
		if( pSkillbuf == NULL ) 
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}
#else
		int nIndex = pSkill->m_wndSkillCtrl[0].GetCurSelect();
		if( nIndex == -1 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}
		LPSKILL pSkillbuf = pSkill->m_wndSkillCtrl[0].GetSkill( nIndex );
		if( pSkillbuf == NULL ) 
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
			//g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
			return FALSE;
		}
#endif //#ifdef __11_MONTH_JOB_PRO		
		ItemProp* pSkillProp = prj.GetSkillProp( pSkillbuf->dwSkill );

		if( pSkillProp->dwExpertMax < 1 || pSkillProp->dwExpertMax < dwSkillLevel )
		{
			char szMessage[MAX_PATH];
			sprintf ( szMessage, prj.GetText(TID_GAME_SKILLLEVELLIMIT), pSkillProp->szName, pSkillProp->dwExpertMax );
//			sprintf ( szMessage, "%s' 스킬은 1 ~ %d 로만 레벨을 올릴수 있습니다", pSkillProp->szName, pSkillProp->dwExpertMax );

			g_WndMng.PutString( szMessage, NULL, 0xffff0000 );
			return FALSE;
		}
		char szSkillLevel[MAX_PATH];
		sprintf( szSkillLevel, prj.GetText(TID_GAME_GAMETEXT001), pSkillbuf->dwSkill, dwSkillLevel );
		scanner.SetProg( szSkillLevel );		
		//sprintf( scanner.pBuf, "/스렙 %d %d", pSkillbuf->dwSkill, dwSkillLevel );
	}
	else
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_CHOICESKILL), NULL, 0xffff0000 );
//		g_WndMng.PutString( "스킬창에 있는 스킬을 선택하여 주십시요", NULL, 0xffff0000 );
		return FALSE;
	}
#endif // __ONLINE
	return TRUE;
}

BOOL TextCmd_whisper( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
static	\
CHAR	lpString[1024];

	CUser* pUser	= (CUser*)scanner.dwValue;

#ifdef __SCHOOL0701
#ifdef __SE0722
	if( pUser->IsMode( SAYTALK_MODE ) )
	{
		return TRUE;
	}
#else // __SE0722
	if( pUser->IsMode( DONTALK_MODE ) )
	{
		return TRUE;
	}
#endif // __SE0722
#endif // __SCHOOL0701
//	TCHAR lpszPlayer[MAX_PLAYER];

	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
		if( idPlayer > 0 ) 
		{
			scanner.GetLastFull();
			strcpy( lpString, scanner.token );
			strrtrim( lpString );

			if( strlen( lpString ) > 260 )	//
				return TRUE;

			g_DPCoreClient.SendWhisper( pUser->m_idPlayer, idPlayer, lpString );
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// 자기 자신에게 명령했다.
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_say( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
static	\
CHAR	lpString[1024];

	CUser* pUser	= (CUser*)scanner.dwValue;

#ifdef __SCHOOL0701
#ifdef __SE0722
	if( pUser->IsMode( SAYTALK_MODE ) )
	{
		return TRUE;
	}
#else // __SE0722
	if( pUser->IsMode( DONTALK_MODE ) )
	{
		return TRUE;
	}
#endif // __SE0722
#endif // __SCHOOL0701
//	TCHAR lpszPlayer[MAX_PLAYER];

	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token ) )
	{
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
		if( idPlayer > 0 ) 
		{
			scanner.GetLastFull();
			strcpy( lpString, scanner.token );
			strrtrim( lpString );

			if( strlen( lpString ) > 260 )	//
				return TRUE;

			g_DPCoreClient.SendSay( pUser->m_idPlayer, idPlayer, lpString );
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  	// 자기 자신에게 명령했다.
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ResistItem( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	DWORD dwObjId	= scanner.GetNumber();
	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();

	if( bItemResist < 0 || 5 < bItemResist )
	{
		return FALSE;
	}
	if( nResistAbilityOption < 0 || 10 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
	{
		return FALSE;
	}

	if( bItemResist == 0 )
	{
		nResistAbilityOption = 0;
	}

	pUser->Lock( theLineFile );

	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
	{
		pUser->Unlock( theLineFile );	// unlock1
		return FALSE;
	}
	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		pUser->Unlock( theLineFile );	// unlock1
		return FALSE;
	}
	
	pUser->UpdateItem( ITYPE_ITEM, pItemElem0->m_dwObjId, UI_IR,  bItemResist );
	pUser->UpdateItem( ITYPE_ITEM, pItemElem0->m_dwObjId, UI_RAO,  nResistAbilityOption );
	pUser->UpdateItem( ITYPE_ITEM, pItemElem0->m_dwObjId, UI_AO,  nAbilityOption );

	pUser->Unlock( theLineFile );	// unlock1
#else // __WORLDSEVER
#ifdef __ONLINE
#ifndef __THAI_0525VER
	if( g_WndMng.m_pWndUpgradeBase == NULL )
	{
		SAFE_DELETE( g_WndMng.m_pWndUpgradeBase );
		g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
		g_WndMng.m_pWndUpgradeBase->Initialize( &g_WndMng, APP_TEST );
		return FALSE;
	}
#endif // __THAI_0525VER
	BYTE bItemResist = scanner.GetNumber();
	int nResistAbilityOption = scanner.GetNumber();
	int nAbilityOption	= scanner.GetNumber();
	if( bItemResist < 0 || 5 < bItemResist )
	{
		return FALSE;
	}
	if( nResistAbilityOption < 0 || 10 < nResistAbilityOption || nAbilityOption < 0 || 10 < nAbilityOption )
	{
		return FALSE;
	}

	if( bItemResist == 0 )
	{
		nResistAbilityOption = 0;
	}

#ifndef __THAI_0525VER
	if( g_WndMng.m_pWndUpgradeBase )
	{
		if( g_WndMng.m_pWndUpgradeBase->m_pItemElem[0] )
		{
			DWORD dwObjId = g_WndMng.m_pWndUpgradeBase->m_pItemElem[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			sprintf( szSkillLevel, "/ritem %d %d %d %d", dwObjId, bItemResist, nResistAbilityOption, nAbilityOption );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif//__THAI_0525VER
#endif // __ONLINE
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommercialElem( CScanner& scanner )
{
#ifdef __ONLINE
#ifdef __S1201_NEW_SM_CODE
	SAFE_DELETE( g_WndMng.m_pWndCommerialElem );
	g_WndMng.m_pWndCommerialElem = new CWndCommercialElem;
	g_WndMng.m_pWndCommerialElem->Initialize( &g_WndMng, APP_COMMERCIAL_ELEM );
#endif // __S1201_NEW_SM_CODE
#endif // __ONLINE
	return FALSE;
}

BOOL TextCmd_Piercing( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	DWORD dwObjId	= scanner.GetNumber();
	BYTE bPierNum = scanner.GetNumber();

	if( bPierNum < 1 || 4 < bPierNum )
	{
		return FALSE;
	}
	
	pUser->Lock( theLineFile );
	
	CItemElem* pItemElem0	= pUser->m_Inventory.GetAtId( dwObjId );
	if( NULL == pItemElem0 )
	{
		pUser->Unlock( theLineFile );	// unlock1
		return FALSE;
	}
	if( pUser->m_Inventory.IsEquip( dwObjId ) )
	{
		pUser->AddDefinedText( TID_GAME_EQUIPPUT , "" );
		pUser->Unlock( theLineFile );	// unlock1
		return FALSE;
	}
	
	pUser->UpdateItem( ITYPE_ITEM, (BYTE)pItemElem0->m_dwObjId, UI_PIERCING_SIZE, bPierNum );

	pUser->Unlock( theLineFile );	// unlock1
#else // __WORLDSEVER
#ifdef __ONLINE
	BYTE bPierNum = scanner.GetNumber();

	if( bPierNum < 1 || 4 < bPierNum )
	{
		return FALSE;
	}	

	CWndPiercing* pWndPiercing = (CWndPiercing*)g_WndMng.GetWndBase(APP_PIERCING);
	if( pWndPiercing )
	{
		if( pWndPiercing->m_pItemElem[0] )
		{
			DWORD dwObjId = pWndPiercing->m_pItemElem[0]->m_dwObjId;
			char szSkillLevel[MAX_PATH];
			sprintf( szSkillLevel, "/pier %d %d", dwObjId, bPierNum );
			scanner.SetProg( szSkillLevel );		
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
#endif // __ONLINE
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_shout( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
static	\
	CHAR lpString[1024];

	CUser* pUser	= (CUser*)scanner.dwValue;

#ifdef __SE0722
	if( pUser->IsMode( SHOUTTALK_MODE ) )
		return FALSE;
#else // __SE0722
	if( pUser->m_dwMode & DONTALK_MODE )
		return FALSE;
#endif // __SE0722

	lpString[0] = '\0';

	scanner.GetLastFull();

	/*
	scanner.GetToken();
	while( scanner.tok != FINISHED ) {	// make paragraph
		
		strcat( lpString, scanner.Token );
		strcat( lpString, " " );
		scanner.GetToken();
	}
	*/
	strcpy( lpString, scanner.token );
	strrtrim( lpString );

	if( strlen( lpString ) > 260 )	//
		return TRUE;

#ifdef __SM_SHOUT_0728
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
#ifdef __YDIALOGBOX_COLOR0309
	arBlock << GETID( pUser );
#endif // __YDIALOGBOX_COLOR0309
	arBlock.WriteString(pUser->GetName());
	arBlock.WriteString( lpString );
	GETBLOCK( arBlock, lpBlock, uBlockSize );

	if( pUser->IsSMMode( SM_SHOUT15 ) || pUser->IsSMMode( SM_SHOUT30 ) ) // 여기서 유료 아이템 사용중인지 확인
	{
		g_UserMng.AddShout( pUser->GetPos(), 0, lpBlock, uBlockSize );
	}
	else
	{
		g_UserMng.AddShout( pUser->GetPos(), 0xff, lpBlock, uBlockSize );
	}
#else // __SM_SHOUT_0728
	g_DPCoreClient.SendShout( pUser, lpString );
#endif // __SM_SHOUT_0728
#else	// __WORLDSERVER
#ifdef __ONLINE
	static CTimer timerDobe;
	static CString stringBack;
	CString string = scanner.m_pProg; 
	if( stringBack != string || timerDobe.TimeOut() )
	{
		stringBack = string;
		timerDobe.Set( SEC( 3 ) );
		g_WndMng.WordChange( string );

		CString strCommand;
		strCommand.Format( "/s %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase( APP_COMMUNICATION_CHAT );
		if( pWndChat )
			g_WndMng.PutString( prj.GetText( TID_GAME_CHATSAMETEXT ), NULL, prj.GetTextColor( TID_GAME_CHATSAMETEXT ) );
			//pWndChat->PutString( "연속으로 같은 내용을 입력할 수 없습니다.", 0xffffffa0 );
	}
	return FALSE;
#endif // __ONLINE
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_PartyChat( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	static	\
		CHAR lpString[260];
	
	CUser* pUser	= (CUser*)scanner.dwValue;
	
	lpString[0] = '\0';
	
	scanner.GetToken();
	while( scanner.tok != FINISHED ) {	// make paragraph
		
		strcat( lpString, scanner.Token );
		strcat( lpString, " " );
		scanner.GetToken();
	}
	strrtrim( lpString );

	CParty* pParty;
	
//	CMclAutoLock	Lock1( g_PartyMng.m_AddRemoveLock );
	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
	
	pParty	= g_PartyMng.GetParty( pUser->GetPartyId() );
	if( pParty && pParty->IsMember( pUser->m_idPlayer ))
	{
		// 파티가 있어서 파티원들에게 보냄
		g_DPCoreClient.SendPartyChat( pUser, lpString );
	}
	else
	{
		// 월드서버에서 파티가 없는경우
		pUser->AddSendErrorParty( ERROR_NOPARTY );
	}

	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
	// 클라이언트에서 극단참여중이니즐 먼저 검색함
#else // __WORLDSERVER
#ifdef __ONLINE
	if( g_Party.GetSizeofMember() >= 2 )
	{
		CString string = scanner.m_pProg;
		g_WndMng.WordChange( string );

		if( g_Option.m_nLanguage == LANG_THA )
			string = '"'+string+'"';
		
		CString strCommand;
//		strCommand.Format( "/극단말 %s", string );
		strCommand.Format( "/p %s", string );
		g_DPlay.SendChat( strCommand );
	}
	else
	{
		// 극단원이 아니므로 
		// 극단에 포함되지 않아 극단채팅을 할수 없습니다.
		//g_WndMng.PutString( "극단에 포함되지 않아 극단채팅을 할수 없습니다", NULL, 0xff99cc00 );
		g_WndMng.PutString( prj.GetText( TID_GAME_PARTYNOTCHAT ), NULL, prj.GetTextColor( TID_GAME_PARTYNOTCHAT ) );
		
	}
	return FALSE;
#endif // __ONLINE
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_Gamemasay( CScanner& scanner )
{
#ifdef __WORLDSERVER
static \
CHAR	lpString[1024];
	
	CUser* pUser = (CUser*)scanner.dwValue;
	lpString[0] = '\0';
	
	scanner.GetToken();
	while( scanner.tok != FINISHED ) {
		strcat( lpString, scanner.Token );
		strcat( lpString, " ");
		scanner.GetToken();
	}
	strrtrim(lpString);

	if( strlen( lpString ) > 260 )
		return TRUE;
	
	g_DPCoreClient.SendGMSay( pUser->m_idPlayer, pUser->GetWorld()->GetID(), lpString );
#endif // __WORLDSERVER
		return TRUE;
}

BOOL TextCmd_Music( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;

	u_long idmusic	= scanner.GetNumber();
	g_DPCoreClient.SendPlayMusic( pUser->GetWorld()->GetID(), idmusic );
#else	// __WORLDSERVER
#ifndef __ONLINE
#endif	// __ONLINE
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Sound( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	u_long idsound	= scanner.GetNumber();
	g_DPCoreClient.SendPlaySound( pUser->GetWorld()->GetID(), idsound );
#else	// __WORLDSERVER
#ifndef __ONLINE
#endif	// __ONLINE
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Summon( CScanner& scanner )           
{
#ifdef __WORLDSERVER
	TCHAR lpszPlayer[32];

	scanner.GetToken();
	CUser* pUser	= (CUser*)scanner.dwValue;
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
		if( idPlayer > 0 ){
			strcpy( lpszPlayer, scanner.Token );
			g_DPCoreClient.SendSummonPlayer( pUser->m_idPlayer, pUser->GetWorld()->GetID(), pUser->GetPos(), idPlayer );	
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else 
	{
		pUser->AddReturnSay( 2, " " );  		// 자기 자신에게 명령했다.		
	}
	
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_Teleport( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
static	\
	TCHAR lpString[64], *lpszPlayer = NULL;
	int x, z;
	CUser* pUser	= (CUser*)scanner.dwValue;

	// 플레이어에게 바로 텔레포트 
	int nTok = scanner.GetToken();
	if( nTok != NUMBER )
	{
		// player
		u_long idPlayer	= prj.GetPlayerID( scanner.token );
		if( idPlayer > 0 ) {
			g_DPCoreClient.SendTeleportPlayer( pUser->m_idPlayer, idPlayer );
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, lpString );
		}
	}
	// 첫번째 파라메타는 월드 번호.
	DWORD dwWorldId = atoi( scanner.token );
#ifdef __INSTANCE_DUNGEON
	if( dwWorldId > 1000 )
		dwWorldId	/= 1000;
#endif	// __INSTANCE_DUNGEON

	if( g_WorldMng.GetWorldStruct( dwWorldId ) )
	{
		// 두번째 파라메타가 스트링이면 리젼 키
		if( scanner.GetToken() != NUMBER )
		{
			PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( dwWorldId, scanner.token );
			if( NULL != pRgnElem )
			{
#ifdef __MSVR
				pUser->Replace( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#else	// __MSVR
				pUser->Replace( pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#endif	// __MSVR
			}
		}
		// 스트링이 아니면 좌표 
		else
		{
			x = atoi( scanner.token );
			z = scanner.GetNumber();
			if( x > 0 && z > 0 )
			{
				// pos
#ifdef __MSVR
				pUser->Replace( g_uIdofMulti, dwWorldId, D3DXVECTOR3( (FLOAT)x, 0, (FLOAT)z ) );
#else	// __MSVR
				pUser->Replace( dwWorldId, D3DXVECTOR3( (FLOAT)x, 0, (FLOAT)z ) );
#endif	// __MSVR
			}
		}
	}
//#endif	// __WORLDSERVER
#else
	TCHAR* lpszPlayer = NULL;
	int x, z;
	x	= scanner.GetNumber();
	z	= scanner.GetNumber();
	if( x > 0 && z > 0 )
	{
		// pos
		g_DialogMsg.RemoveAll();
		g_WorldMng()->ReadWorld( D3DXVECTOR3( (FLOAT)x, 130, (FLOAT)z ), FALSE );
		g_pPlayer->SetPos( D3DXVECTOR3( (FLOAT)x, 130, (FLOAT)z ) );
	}		
#endif
	return TRUE;
}
BOOL TextCmd_Out( CScanner& scanner )              
{ 
#ifdef __WORLDSERVER
//	TCHAR lpszPlayer[MAX_PLAYER];
	scanner.GetToken();

	CUser* pUser	= (CUser*)scanner.dwValue;
	if( strcmp( pUser->GetName(), scanner.Token) )
	{	
		u_long idPlayer		= prj.GetPlayerID( scanner.Token );
		if( idPlayer > 0 ) {
			g_DPCoreClient.SendKillPlayer( pUser->m_idPlayer, idPlayer );
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );  		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_RemoveNpc( CScanner& s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	OBJID objid	= (OBJID)s.GetNumber();

//	CMclAutoLock	Lock( prj.m_AddRemoveLock );	// lock
	prj.m_AddRemoveLock.Enter( theLineFile );	// lock1

	CMover* pMover	= prj.GetMover( objid );
	if( IsValidObj( (CObj*)pMover ) )
	{
		if( pMover->IsNPC() )
			pMover->Delete();
	}

	prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1
#endif
#ifdef __ONLINE
	CWorld* pWorld = g_WorldMng();
	CObj* pObj = pWorld->GetObjFocus();
	CString string;
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		string.Format( "/rn %d", ((CMover*)pObj)->GetId() );
		g_DPlay.SendChat( (LPCSTR)string.LockBuffer() );
		string.UnlockBuffer();
	}
	else
	{
		string.Format( "/rn %s", s.m_pProg );
		g_DPlay.SendChat( (LPCSTR)string.LockBuffer() );
		string.UnlockBuffer();
	}
#endif
	return TRUE;
}

BOOL TextCmd_Hit( CScanner& scanner )              
{ 
	return TRUE;
}
BOOL TextCmd_Mana( CScanner& scanner )             
{ 
	return TRUE;
}
BOOL TextCmd_Fatigue( CScanner& scanner )          
{ 
	return TRUE;
}

#ifdef __RANDOMOPTITEM0628
BOOL TextCmd_CreateItem2( CScanner & s )
{
#ifdef __WORLDSERVER
	s.GetToken();
	ItemProp* pItemProp	= prj.GetItemProp( s.Token );
	if( pItemProp )
	{
		int nRandomOptItemId	= s.GetNumber();
		PRANDOMOPTITEM pRandomOptItem	= CRandomOptItemGen::GetInstance()->GetRandomOptItem( nRandomOptItemId );
		if( pRandomOptItem )
		{
			CItemElem itemElem;
			itemElem.m_dwItemId		= pItemProp->dwID;
			itemElem.m_nItemNum	= 1;
			itemElem.m_nHitPoint	= -1;
			itemElem.m_nRandomOptItemId		= pRandomOptItem->nId;
			CUser* pUser	= (CUser*)s.dwValue;
			pUser->CreateItem( &itemElem );
		}
	}
	return TRUE;
#else	// __WORLDSERVER
	return TRUE;
#endif	// __WORLDSERVER
}
#endif	// __RANDOMOPTITEM0628

BOOL TextCmd_CreateItem( CScanner& scanner )       
{
	DWORD dwNum;
	DWORD dwCharged;
	ItemProp* pProp	= NULL;
	scanner.GetToken();

#ifndef __WORLDSERVER
	// 클라이언트에서
#ifndef __ONLINE
	if( scanner.tok == FINISHED )
	{
		if( g_WndMng.GetWndBase( APP_ADMIN_CREATEITEM ) == NULL )
		{
			CWndAdminCreateItem* pWndAdminCreateItem = new CWndAdminCreateItem;
			pWndAdminCreateItem->Initialize();
		}
		return FALSE;
	}
#else
	if( scanner.tok == FINISHED )
	{
		if( g_WndMng.GetWndBase( APP_ADMIN_CREATEITEM ) == NULL )
		{
			CWndAdminCreateItem* pWndAdminCreateItem = new CWndAdminCreateItem;
			pWndAdminCreateItem->Initialize();
		}
		return FALSE;
	}
	return TRUE;
#endif
	// 창이 안뜨는 모드 이므로 아무 처리 없이 서버로 문자열을 전송한다.
//	"/createitem xxx xxxx"	-> lpString
//	g_DPlay.SendChat( lpString );
//	return FALSE;
#endif	// __WORLDSERVER

	if( scanner.tokenType == NUMBER )
		pProp	= prj.GetItemProp( _ttoi( scanner.Token ) );
	else
		pProp	= prj.GetItemProp( scanner.Token );

	if( pProp && pProp->dwItemKind3 != IK3_VIRTUAL )
	{
		dwNum	= scanner.GetNumber();
		dwNum	= ( dwNum == 0? 1: dwNum );
		dwCharged	= scanner.GetNumber();
		dwCharged	= ( dwCharged == 0? 0: 1 );
#ifdef __WORLDSERVER
		CUser* pUser	= (CUser*)scanner.dwValue;
//		pUser->CreateItem( pProp->dwItemKind1, NULL_ID, pProp->dwID, dwNum );
		DWORD dwItemType = 0;
		/*
		if( pProp->dwItemKind1 > 0 && pProp->dwItemKind1 <= 4 )
			dwItemType	= ITYPE_ITEM;
		else if( pProp->dwItemKind1 == IK1_CARD )
			dwItemType	= ITYPE_CARD;
		else if( pProp->dwItemKind1 == IK1_CUBE )
			dwItemType	= ITYPE_CUBE;
			*/
		CItemElem itemElem;
		itemElem.m_dwItemId		= pProp->dwID;
		itemElem.m_nItemNum		= dwNum;
		itemElem.m_nHitPoint	= -1;
#ifdef __PIERCING_SM_TIME
		itemElem.m_bCharged		= dwCharged;
#endif // __PIERCING_SM_TIME
		pUser->CreateItem( &itemElem );
#else	// __WORLDSERVER
		CItemElem itemElem;
		itemElem.m_dwItemId		= pProp->dwID;
		itemElem.m_nItemNum		= dwNum;
		itemElem.m_nHitPoint	= -1;
		g_pPlayer->AddItem( &itemElem );
#endif	// __WORLDSERVER
	}
	else
	{
#ifdef __WORLDSERVER
#else
		//g_WndMng.PutString( _T( "그런 아이템은 없습니다." ) );
		g_WndMng.PutString( prj.GetText( TID_ADMIN_NOITEMNAME ), NULL, prj.GetTextColor( TID_ADMIN_NOITEMNAME ) );
#endif
	}
	return TRUE;
}

#ifdef __J0204
BOOL TextCmd_LocalEvent( CScanner & s )
{
#ifdef __WORLDSERVER
	int id	= s.GetNumber();
	if( id != EVE_18 )	// 이 식별자는 18세 서버를 나타내는 식별자므로 운영자의 명령에 의한 설정 불가
	{
		BYTE nState		= (BYTE)s.GetNumber();
		if( g_eLocal.SetState( id, nState ) )		g_UserMng.AddSetLocalEvent( id, nState );
	}
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __J0204
// 무버이름 캐릭터키 갯수 선공 
BOOL TextCmd_CreateChar( CScanner& scanner )       
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	D3DXVECTOR3 vPos = pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	scanner.GetToken();
	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

		BOOL bActiveAttack = scanner.GetNumber();
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CMover* pMover = (CMover*)CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pMover ) return FALSE; // ASSERT( pObj );
			strcpy( pMover->m_szCharacterKey, strName );
			pMover->InitNPCProperty();
			pMover->InitCharacter( pMover->GetCharacter() );
			pMover->SetPos( vPos );
			pMover->InitMotion( pMover->m_dwMotion );
			pMover->UpdateLocalMatrix();
			if( bActiveAttack )
				pMover->m_bActiveAttack = bActiveAttack;
			pWorld->AddObj( pMover, TRUE );
		}
	}
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __DUNGEON_1118
BOOL TextCmd_CreateCtrl( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)s.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();

	DWORD dwID	= s.GetNumber();
	
	if( dwID == 0 )
		return FALSE;

	CCtrl* pCtrl	= (CCtrl*)CreateObj( D3DDEVICE, OT_CTRL, dwID );
	if( !pCtrl )
		return FALSE;

	pCtrl->SetPos( vPos );
	pWorld->AddObj( pCtrl, TRUE );
#endif	// __WORLDSERVER
	return TRUE;
}
#endif	// __DUNGEON_1118

BOOL TextCmd_CreateNPC( CScanner& scanner )  
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	D3DXVECTOR3 vPos	= pUser->GetPos();
	CWorld* pWorld	= pUser->GetWorld();
#else	//__WORLDSERVER
	D3DXVECTOR3 vPos	= g_pPlayer->GetPos();
	CWorld* pWorld	= g_pPlayer->GetWorld();
#endif	// __WORLDSERVER
	
	MoverProp* pMoverProp	= NULL;

	scanner.GetToken();
//	FILEOUT( "error2.txt", "token str %s tokentype %d \n", scanner.token,  scanner.tokenType );
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	CString strName = scanner.Token;

	if( pMoverProp && pMoverProp->dwID != 0 )
	{
		DWORD dwNum	= scanner.GetNumber();
		if( dwNum > 100 ) dwNum = 100;
		if( dwNum == 0 ) dwNum = 1;

#ifndef __THAI_0525VER
		BOOL bActiveAttack = scanner.GetNumber();
#else//__THAI_0525VER
		BOOL bActiveAttack = scanner.GetNumber();
#endif//__THAI_0525VER
		for( DWORD dw = 0; dw < dwNum; dw++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	return FALSE;	// ASSERT( pObj );
//			FILEOUT( "error2.txt", "a %s b %s c %d \n", strName, pMoverProp->szName, pMoverProp->dwID );
			pObj->SetPos( vPos );
			pObj->InitMotion( pObj->m_dwMotion );
			pObj->UpdateLocalMatrix();

#ifndef __THAI_0525VER
			if( bActiveAttack )
				((CMover*)pObj)->m_bActiveAttack = bActiveAttack;
#else//__THAI_0525VER
			if( bActiveAttack )
				((CMover*)pObj)->m_bActiveAttack = bActiveAttack;
#endif//__THAI_0525VER
			
#ifdef __ACROBAT_0504
			((CMover*)pObj)->SetGold(((CMover*)pObj)->GetLevel()*15);  // 몬스터 생성시 기본 페냐를 설정
#endif //__ACROBAT_0504
			
			pWorld->AddObj( pObj, TRUE );
		}
	}

	return TRUE;
}

/*
BOOL TextCmd_DeleteNPC( CScanner& scanner )
{
	DWORD dwID = scanner.GetNumber(); // NPC ID
	CMover* pMover = prj.GetMover( dwID );
	pMover->Delete();
	return TRUE;
}
*/
BOOL TextCmd_CheckInventory( CScanner& scanner )   
{ 
	return TRUE;
}
BOOL TextCmd_CheckState( CScanner& scanner )       
{ 
	return TRUE;
}
BOOL TextCmd_Invisible( CScanner& scanner )        
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode |= TRANSPARENT_MODE;
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoInvisible( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~TRANSPARENT_MODE);
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_NoUndying( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __ONLINE
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
#endif
#endif
	return TRUE;
}

/*
BOOL TextCmd_NoUndying2( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __ONLINE
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
#endif
#endif
	return TRUE;
}
*/

BOOL TextCmd_Undying( CScanner& scanner )          
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __ONLINE
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS2_MODE);
	pUser->m_dwMode |= MATCHLESS_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Undying2( CScanner& scanner )          
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __ONLINE
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~MATCHLESS_MODE);
	pUser->m_dwMode |= MATCHLESS2_MODE;
#endif
#endif
	return TRUE;
}

BOOL TextCmd_Save( CScanner& scanner )             
{ 
/*
#ifdef __WORLDSERVER
	g_DPCoreClient.SendSaveAllPlayers();
#endif
*/
	return TRUE;
}

BOOL TextCmd_NoDisguise( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
#ifdef __V050322_CACHEQUEST
	pUser->Lock( theLineFile );
	pUser->NoDisguise( NULL );
	pUser->Unlock( theLineFile );
	g_UserMng.AddNoDisguise( pUser );
#else // __V050322_CACHEQUEST
//{{AFX
	// 이미 사람 모양이므로 해제 처리 할 필요 없음. 
	if( pUser->GetIndex() == MI_MALE || pUser->GetIndex() == MI_FEMALE )
		return TRUE;
	// 기존 모델 파괴 
	pUser->Lock( theLineFile );
	if( pUser->m_pModel && pUser->m_pModel->IsAniable() )
		SAFE_DELETE( pUser->m_pModel );
	DWORD dwIndex = (pUser->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	pUser->SetTypeIndex( NULL, OT_MOVER, dwIndex );//
	pUser->ResetScale();
	pUser->SetMotion( MTI_WALK );
	pUser->UpdateLocalMatrix();
	pUser->RedoEquip( FALSE );
	pUser->Unlock( theLineFile );
	g_UserMng.AddNoDisguise( pUser );
//}}AFX
#endif // __V050322_CACHEQUEST
#endif // __WORLDSERVER
	return TRUE;
}
#ifdef __WORLDSERVER
BOOL DoDisguise( CUser* pUser, DWORD dwIndex )
{
#ifdef __V050322_CACHEQUEST
	pUser->Lock( theLineFile );
	pUser->Disguise( NULL, dwIndex );
	pUser->Unlock( theLineFile );
	g_UserMng.AddDisguise( pUser, dwIndex );
#else // __V050322_CACHEQUEST
//{{AFX
	TCHAR szDestName[ 128 ];
	MoverProp* pMoverProp = NULL;
	pMoverProp = prj.GetMoverPropEx( dwIndex );

	if( pMoverProp )
	{
		// 기존 모델 파괴 
		pUser->Lock( theLineFile );
		if( pUser->m_pModel && pUser->m_pModel->IsAniable() )
			SAFE_DELETE( pUser->m_pModel );
		
		pUser->SetTypeIndex( NULL, OT_MOVER, pMoverProp->dwID );//
		pUser->ResetScale();
		pUser->m_dwMotion = pUser->m_dwMotionOld = -1;
		pUser->SetMotion( MTI_STAND );
		pUser->UpdateLocalMatrix();
		pUser->Unlock( theLineFile );
		
		g_UserMng.AddDisguise( pUser, pMoverProp->dwID );
	}
//}}AFX
#endif // __V050322_CACHEQUEST
	return TRUE;
}
#endif // __WORLDSERVER
BOOL TextCmd_Dwarpet( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	DoDisguise( pUser, MI_DWARPETMAS );
#endif
	return TRUE;
}
BOOL TextCmd_Iblis( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	DoDisguise( pUser, MI_CROWNIBLIS );
#endif
	return TRUE;
} 
BOOL TextCmd_Shade( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	DoDisguise( pUser, MI_CROWNSHADE );
#endif
	return TRUE;
}
BOOL TextCmd_Bubble( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	DoDisguise( pUser, MI_CROWNBUBBLE );
	pUser->m_SkillState
#endif
	return TRUE;
}


BOOL TextCmd_Disguise( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	MoverProp* pMoverProp = NULL;
	scanner.GetToken();
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );

	if( pMoverProp )
		DoDisguise( pUser, pMoverProp->dwID );
	/*
	CUser* pUser = (CUser*)scanner.dwValue;
	TCHAR szDestName[ 128 ];
	MoverProp* pMoverProp	= NULL;
	scanner.GetToken();// 변신할 캐릭터 이름 얻기 
	if( scanner.tokenType == NUMBER ) 
	{
		DWORD dwID	= _ttoi( scanner.Token );
		pMoverProp = prj.GetMoverPropEx( dwID );
	}
	else
		pMoverProp	= prj.GetMoverProp( scanner.Token );
	if( pMoverProp )
	{
		//DoDisguise( pUser, pMoverProp->dwID );
	}
	*/
#endif
	return TRUE;
}
BOOL TextCmd_Freeze( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	TCHAR lpszPlayer[32];
	
	scanner.GetToken();

	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
		idFrom	= prj.GetPlayerID( pUser->GetName() );
		idTo	= prj.GetPlayerID( scanner.Token );
		if( idFrom > 0 && idTo > 0 ) {
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)1, idFrom, idTo );	// 1 : 추가 m_dwMode
				
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoFreeze( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	TCHAR lpszPlayer[32];
	
	scanner.GetToken();
	if( strcmp( pUser->GetName(), scanner.Token) )
	{
		u_long idFrom, idTo;
		idFrom	= prj.GetPlayerID( pUser->GetName() );
		idTo	= prj.GetPlayerID( scanner.Token );
		if( idFrom > 0 && idTo > 0 ) {
			g_DPCoreClient.SendModifyMode( DONMOVE_MODE, (BYTE)0, idFrom, idTo );	// 0 : 뺌 m_dwMode
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_Talk( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	TCHAR lpszPlayer[32];
	
	scanner.GetToken();

	u_long idFrom, idTo;
	idFrom	= prj.GetPlayerID( pUser->GetName() );
	idTo	= prj.GetPlayerID( scanner.Token );
	if( idFrom > 0 && idTo > 0 ) 
	{
		g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)0, idFrom, idTo );	// 0 : 뺌 m_dwMode
	}
	else 
	{
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
		pUser->AddReturnSay( 3, scanner.Token );
	}
//	else
//	{
//		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
//	}
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_NoTalk( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER

	CUser* pUser	= (CUser*)scanner.dwValue;
	TCHAR lpszPlayer[32];
	
	scanner.GetToken();

	{
		u_long idFrom, idTo;
		idFrom	= prj.GetPlayerID( pUser->GetName() );
		idTo	= prj.GetPlayerID( scanner.Token );
		if( idFrom > 0 && idTo > 0 ) {
			g_DPCoreClient.SendModifyMode( DONTALK_MODE, (BYTE)1, idFrom, idTo );	// 1 : 추가
		}
		else {
//			scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
			pUser->AddReturnSay( 3, scanner.Token );
		}
	}
/*	else
	{
		pUser->AddReturnSay( 2, " " );		// 자기 자신에게 명령했다.
	}
*/
#endif	// __WORLDSERVER	
	return TRUE;
}

BOOL TextCmd_GetGold( CScanner& scanner )           
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	int nGold = scanner.GetNumber();	
	//pUser->SetPointParam( DST_GOLD, pUser->GetGold() + nGold );
	pUser->AddGold( nGold );
#endif	// __WORLDSERVER	
	return TRUE;
}
// /간접 npcId "대사"
BOOL TextCmd_indirect( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	DWORD dwIdNPC = scanner.GetNumber();
	TCHAR szString[ 1024 ];
	/*
	szString[0]	= '\0';
	scanner.GetToken();
	while( scanner.tok != FINISHED ) 
	{	// make paragraph
		strcat( szString, scanner.Token );
		strcat( szString, " " );
		scanner.GetToken();
	}
*/
	scanner.GetLastFull();
	strcpy( szString, scanner.token );
	strrtrim( szString );
	if( szString[ 0 ] )
	{
		CMover* pMover = prj.GetMover( dwIdNPC );
		if( pMover )
			g_UserMng.AddChat( (CCtrl*)pMover, (LPCSTR)szString );
	}
#else // __WORLDSERVER
	scanner.GetToken();
	if( scanner.tok == FINISHED )
	{
		if( g_WndMng.GetWndBase( APP_ADMIN_INDIRECT_TALK ) == NULL )
		{
			CWndIndirectTalk* pWndIndirectTalk = new CWndIndirectTalk;
			pWndIndirectTalk->Initialize();
		}
		return FALSE;
	}
#endif // __WORLDSERVER
	return TRUE;
}

#ifdef __SCHOOL0701
BOOL TextCmd_sbreport( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Report();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbready( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Ready();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbstart( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Start();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbstart2( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->Start2();
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_sbend( CScanner & scanner )
{
#ifdef __WORLDSERVER
	if( !g_eLocal.GetState( EVE_SCHOOL ) )
		return FALSE;
	return CEveSchool::GetInstance()->End();
#endif	// __WORLDSERVER
	return TRUE;
}

#ifdef __SERVERBATTLE0828
BOOL TextCmd_OpenBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_OPEN_BATTLESERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
#endif	// __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CloseBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
	BEFORESENDDUAL( ar, PACKETTYPE_CLOSE_BATTLESERVER, DPID_UNKNOWN, DPID_UNKNOWN );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
#endif	// __WORLDSERVER
	return TRUE;
}
/*
BOOL TextCmd_JoinBattleServer( CScanner & s )
{
#ifdef __WORLDSERVER
//	time.GetDayOfWeek()
//	CTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1 );
	CUser* pUser = (CUser*)s.dwValue;

	if( CTime::GetCurrentTime() > CTime( 2004, 9, 9, 17, 0, 0 ) )
	{
		pUser->AddDefinedText( TID_SBEVE_SBNJOIN, "" );
		return TRUE;
	}

	BEFORESENDDUAL( ar, PACKETTYPE_JOIN_BATTLESERVER, pUser->m_dpidCache, pUser->m_dpidUser );
	ar << pUser->m_idPlayer;
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );
	pUser->AddDefinedText( TID_SBEVE_SBJOIN, "" );
#endif	// __WORLDSERVER
	return TRUE;
}
*/
#endif	// __SERVERBATTLE0828

BOOL TextCmd_ItemMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( ITEM_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ItemNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( ITEM_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_AttackMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( ATTACK_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_EscapeReset( CScanner& scanner )
{
#ifdef __WORLDSERVER
//	scanner.GetToken();
	CUser* pUser = (CUser*)scanner.dwValue;
	CMover* pMover = prj.GetUserByID( scanner.GetNumber() );
	if( IsValidObj( pMover ) ) {
		pMover->SetSMMode( SM_ESCAPE, 0 );
	}
	else {
		pUser->SetSMMode( SM_ESCAPE, 0 );
	}
#else	// __WORLDSERVER
#ifdef __ONLINE
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/EscapeReset %d", pMover->m_idPlayer  );
		}

		g_DPlay.SendChat( (LPCSTR)strSend );

		return FALSE;
	}
#endif //__ONLINE
#endif	// __WORLDSERVER
	return TRUE;

















	return TRUE;
}



BOOL TextCmd_AttackNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( ATTACK_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( COMMUNITY_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_CommunityNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( COMMUNITY_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetMode( OBSERVE_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

BOOL TextCmd_ObserveNotMode( CScanner& scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->SetNotMode( OBSERVE_MODE );
	g_UserMng.AddModifyMode( pUser );
#endif // __WORLDSERVER
	return TRUE;
}

#endif	// __SCHOOL0701

BOOL TextCmd_Onekill( CScanner& scanner )          
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __ONLINE
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode |= ONEKILL_MODE;
#endif
#endif
	return TRUE;
}
BOOL TextCmd_Position( CScanner& scanner )          
{ 
#ifdef __CLIENT
	CString string;
	D3DXVECTOR3 vPos = g_pPlayer->GetPos();
	//string.Format( "현재좌표 : x = %f, y = %f, z = %f", vPos.x, vPos.y, vPos.z );
	//g_WndMng.PutString( string );
	string.Format( prj.GetText(TID_GAME_NOWPOSITION), vPos.x, vPos.y, vPos.z );
	g_WndMng.PutString( string, NULL, prj.GetTextColor( TID_GAME_NOWPOSITION ) );
	
#endif // __CLIENT
	return TRUE;
}

BOOL TextCmd_NoOnekill( CScanner& scanner )          
{ 
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
	g_UserMng.AddModifyMode( pUser );
#else // __WORLDSERVER
#ifndef __ONLINE
	CMover* pUser = (CMover*)scanner.dwValue;
	pUser->m_dwMode &= (~ONEKILL_MODE);
#endif
#endif
	return TRUE;
}
BOOL TextCmd_ip( CScanner& scanner )               
{ 
#ifdef __WORLDSERVER
	TCHAR lpszPlayer[32];
	scanner.GetToken();

	CUser* pUser = (CUser*)scanner.dwValue;
	u_long idPlayer		= prj.GetPlayerID( scanner.Token );
	if( idPlayer > 0 ) {
		g_DPCoreClient.SendGetPlayerAddr( pUser->m_idPlayer, idPlayer );
	}
	else {
//		scanner.Token라는 이름을 가진 사용자는 이 게임에 존재하지 않는다.
		pUser->AddReturnSay( 3, scanner.Token );
	}
#else	// __WORLDSERVER
	#ifdef __ONLINE
	CWorld* pWorld	= g_WorldMng.Get();
	CObj*	pObj;
	
	pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		CMover* pMover = (CMover*)pObj;
		CString strSend;
		if( pMover->IsPlayer() )
		{
			strSend.Format( "/ip %s", pMover->GetName() );
		}
		else
		{
			strSend.Format( "%s", scanner.m_pBuf );
		}
		g_DPlay.SendChat( (LPCSTR)strSend );
		return FALSE;
	}
	#endif //__ONLINE
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_userlist( CScanner& scanner )         
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	g_DPCoreClient.SendGetCorePlayer( pUser->m_idPlayer );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_count( CScanner& scanner )            
{ 
#ifdef __WORLDSERVER
	CUser* pUser	= (CUser*)scanner.dwValue;
	g_DPCoreClient.SendGetPlayerCount( pUser->m_idPlayer );

	char szCount[128]	= { 0, };
	sprintf( szCount, "%d", g_UserMng.GetCount() );
	pUser->AddText( szCount );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_System( CScanner & scanner )
{
#ifdef __WORLDSERVER
static	\
CHAR	lpString[260];

	CUser* pUser	= (CUser*)scanner.dwValue;
	*lpString	= '\0';

	//scanner.GetToken();
	scanner.GetLastFull();
	while( scanner.tok != FINISHED ) {	// make paragraph
		strcat( lpString, scanner.Token );
		strcat( lpString, " " );
		scanner.GetToken();
	}
	strrtrim( lpString );

	g_DPCoreClient.SendSystem( lpString );
#endif	// __WORLDSERVER
	return TRUE;
}
BOOL TextCmd_LoadScript( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
//  #if defined(__INTERNALSERVER) || defined(_DEBUG) 
	pUser->GetWorld()->LoadAllMoverDialog();
//  #endif

#else // __WORLDSERVER
	//prj.LoadWordToolTip( "wordToolTip.inc" );
#	ifndef __ONLINE
	g_WorldMng()->LoadAllMoverDialog();
#	endif
#endif
	return TRUE;
}

BOOL TextCmd_FallSnow( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendFallSnow();
#endif // __WORLDSERVER
/*
#ifdef __CLIENT
	BOOL bOnOff	= scanner.GetNumber();
	CWorld::m_skyBox.SetWeather( WEATHER_SNOW, bOnOff );
#else

#endif
*/
	return TRUE;
}
BOOL TextCmd_FallRain( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendFallRain();
#endif // __WORLDSERVER
/*
#ifdef __CLIENT
	BOOL bOnOff	= scanner.GetNumber();
	CWorld::m_skyBox.SetWeather( WEATHER_RAIN, bOnOff );
#else

#endif
*/
	return TRUE;
}
BOOL TextCmd_StopSnow( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendStopSnow();
#endif // __WORLDSERVER
/*
#ifdef __CLIENT
	BOOL bOnOff	= scanner.GetNumber();
	CWorld::m_skyBox.SetWeather( WEATHER_SNOW, bOnOff );
#else
	
#endif
*/
	return TRUE;
}
BOOL TextCmd_StopRain( CScanner & scanner )
{
#ifdef __WORLDSERVER
	CUser* pUser	 = (CUser*)scanner.dwValue;
	g_DPCoreClient.SendStopRain();
#endif // __WORLDSERVER
/*
#ifdef __CLIENT
	BOOL bOnOff	= scanner.GetNumber();
	CWorld::m_skyBox.SetWeather( WEATHER_RAIN, bOnOff );
#else
	
#endif
*/
	return TRUE;
}

BOOL TextCmd_JobName( CScanner & scanner )
{
#ifdef __CLIENT
	char chMessage[MAX_PATH] = {0,};
	sprintf( chMessage, "Expert Job : " );
	for( int i = MAX_JOBBASE ; i < MAX_EXPERT ; ++i )
	{
		if( strlen( prj.m_aJob[i].szName ) < 15 )
		{
			strcat( chMessage, prj.m_aJob[i].szName );
			if( i + 1 != MAX_EXPERT )
			{
				strcat( chMessage, ", ");
			}
		}
	}
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
	sprintf( chMessage, "Expert Level : %d ~ %d", MAX_JOB_LEVEL + 1, MAX_JOB_LEVEL + MAX_EXP_LEVEL );
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );

#ifdef __11_MONTH_JOB_PRO
	sprintf( chMessage, "Professional Job : " );
	for( i = MAX_EXPERT ; i < MAX_PROFESSIONAL ; ++i )
	{
		if( strlen( prj.m_aJob[i].szName ) < 15 )
		{
			strcat( chMessage, prj.m_aJob[i].szName );
			if( i + 1 != MAX_PROFESSIONAL )
			{
				strcat( chMessage, ", ");
			}
		}
	}
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
	sprintf( chMessage, "Professional Level : %d ~~~ ", MAX_JOB_LEVEL + MAX_EXP_LEVEL );
	g_WndMng.PutString( chMessage, NULL, 0xffff0000 );
#endif // __11_MONTH_JOB_PRO
#endif // __CLIENT
	return TRUE;
}

#ifdef __Y0601
BOOL TextCmd_tradeagree( CScanner & scanner )
{
	g_Option.m_bTrade = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEAGREE ), NULL, prj.GetTextColor( TID_GAME_TRADEAGREE ) );
	return TRUE;
}
BOOL TextCmd_traderefuse( CScanner & scanner )
{
	g_Option.m_bTrade = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_TRADEREFUSE ), NULL, prj.GetTextColor( TID_GAME_TRADEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_whisperagree( CScanner & scanner )
{
	g_Option.m_bSay = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERAGREE ), NULL, prj.GetTextColor( TID_GAME_WHISPERAGREE ) );
	return TRUE;
}
BOOL TextCmd_whisperrefuse( CScanner & scanner )
{
	g_Option.m_bSay = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_WHISPERREFUSE ), NULL, prj.GetTextColor( TID_GAME_WHISPERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_messengeragree( CScanner & scanner )
{
	g_Option.m_bMessenger = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERAGREE ), NULL, prj.GetTextColor( TID_GAME_MSGERAGREE ) );
	return TRUE;
}
BOOL TextCmd_messengerrefuse( CScanner & scanner )
{
	g_Option.m_bMessenger = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_MSGERREFUSE ), NULL, prj.GetTextColor( TID_GAME_MSGERREFUSE ) );
	return TRUE;
}
BOOL TextCmd_stageagree( CScanner & scanner )
{
	g_Option.m_bParty = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEAGREE ), NULL, prj.GetTextColor( TID_GAME_STAGEAGREE ) );
	return TRUE;
}
BOOL TextCmd_stagerefuse( CScanner & scanner )
{
	g_Option.m_bParty = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_STAGEREFUSE ), NULL, prj.GetTextColor( TID_GAME_STAGEREFUSE ) );
	return TRUE;
}
BOOL TextCmd_connectagree( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNAGREE ), NULL, prj.GetTextColor( TID_GAME_CONNAGREE ) );
	return TRUE;
}
BOOL TextCmd_connectrefuse( CScanner & scanner )
{
	g_Option.m_bMessengerJoin = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_CONNREFUSE ), NULL, prj.GetTextColor( TID_GAME_CONNREFUSE ) );
	return TRUE;
}
BOOL TextCmd_shoutagree( CScanner & scanner )
{
	g_Option.m_bShout = 1;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTAGREE ), NULL, prj.GetTextColor( TID_GAME_SHOUTAGREE ) );
	return TRUE;
}
BOOL TextCmd_shoutrefuse( CScanner & scanner )
{
	g_Option.m_bShout = 0;
	g_WndMng.PutString( prj.GetText( TID_GAME_SHOUTREFUSE ), NULL, prj.GetTextColor( TID_GAME_SHOUTREFUSE ) );
	return TRUE;
}
#endif

#ifdef __NPP_050308 
BOOL TextCmd_RecoverMode( CScanner& scanner )
{
#ifdef __CLIENT
	if( g_pPlayer )
	{
#ifdef __ONLINE		
		g_DPlay.SendRecoverChaoMode();
#endif //__ONLINE
	}
#endif
	return TRUE;
}
#ifdef __V050322_CACHEQUEST
BOOL TextCmd_QuestState( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	int nState = s.GetNumber();
	if( nState >= 0 && nState < 15 )
	{
		QUEST quest;
		if( pUser->SetQuest( nQuest, nState, &quest ) )
			pUser->AddSetQuest( &quest );
	}
#endif
	return TRUE;
}
BOOL TextCmd_BeginQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	QUEST quest;
	if( pUser->SetQuest( nQuest, 0, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
BOOL TextCmd_EndQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	QUEST quest;
	if( pUser->SetQuest( nQuest, QS_END, &quest ) )
		pUser->AddSetQuest( &quest );
#endif
	return TRUE;
}
// 지정한 것, 현재와 완료 다 뒤져서 삭제 
BOOL TextCmd_RemoveQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	pUser->RemoveQuest( nQuest );
	pUser->AddRemoveQuest( nQuest );
#endif
	return TRUE;
}
// 현재, 완료 모두 삭제 
BOOL TextCmd_RemoveAllQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	pUser->RemoveAllQuest();
	pUser->AddRemoveAllQuest();
#endif
	return TRUE;
}
// 완료만 삭제 
BOOL TextCmd_RemoveCompleteQuest( CScanner & s )
{
#ifdef __WORLDSERVER
	CUser* pUser = (CUser*)s.dwValue;
	int nQuest = s.GetNumber();
	pUser->RemoveCompleteQuest();
	pUser->AddRemoveCompleteQuest();
#endif
	return TRUE;
}
#endif // __V050322_CACHEQUEST

BOOL TextCmd_PvpParam( CScanner& scanner )
{
#if defined(__WORLDSERVER) && defined(__NPP_050308)
	CUser* pUser	= (CUser*)scanner.dwValue;
	int	nFame       = scanner.GetNumber();
	int nSlaughter  = scanner.GetNumber();

	pUser->m_nFame  = nFame;
	g_UserMng.AddSetFame( pUser, nFame );

	pUser->ChangeSlaughter( CHANGE_SLAUGHTER_SET, NULL, nSlaughter );
#endif
	return TRUE;
}

#endif // __NPP_050308

BEGINE_TEXTCMDFUNC_MAP

	ON_TEXTCMDFUNC( TextCmd_Open			, _T( "open"           ) , _T( "open"      ) , _T( "열기"         ) , _T( "열기"			), TCM_CLIENT   , AUTH_GAMEMASTER    ,  "" )
	ON_TEXTCMDFUNC( TextCmd_Close			, _T( "close"          ) , _T( "close"     ) , _T( "닫기"         ) , _T( "닫기"			), TCM_CLIENT   , AUTH_GAMEMASTER    ,  "" )
 	ON_TEXTCMDFUNC( TextCmd_whisper	        , _T( "whisper"        ) , _T( "w"         ) , _T( "귓속말"       ) , _T( "귓"				), TCM_SERVER   , AUTH_GENERAL       ,  "귓속말 [/명령 아이디 내용]" )
 	ON_TEXTCMDFUNC( TextCmd_say				, _T( "say"            ) , _T( "say"       ) , _T( "말"           ) , _T( "말"				), TCM_SERVER   , AUTH_GENERAL       ,  "속삭임 [/명령 아이디 내용]" )
 	ON_TEXTCMDFUNC( TextCmd_Position		, _T( "position"       ) , _T( "pos"       ) , _T( "좌표"         ) , _T( "좌표"		    ), TCM_CLIENT   , AUTH_GENERAL       ,  "현재 좌표를 출력해준다." ) 

 	ON_TEXTCMDFUNC( TextCmd_shout			, _T( "shout"          ) , _T( "s"        ) , _T( "외치기"       ) , _T( "외"				), TCM_BOTH   , AUTH_GENERAL       ,  "외치기 [/명령 아이디 내용]" ) // 주의 : 욕 필터링 때문에 클라에서만 별도로 처리하고 따로 전송한다.

	ON_TEXTCMDFUNC( TextCmd_PartyChat		, _T( "partychat"         ) , _T( "p"        ) , _T( "극단말"       ) , _T( "극"				), TCM_BOTH   , AUTH_GENERAL       ,  "파티 채팅 [/명령 내용]" ) // 주의 : 욕 필터링 때문에 클라에서만 별도로 처리하고 따로 전송한다.

#ifdef __Y0601
	ON_TEXTCMDFUNC( TextCmd_tradeagree		, _T( "tradeagree"         ) , _T( "ta"        ) , _T( "거래승인"       ) , _T( "거승"				), TCM_CLIENT   , AUTH_GENERAL       ,  "거래 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_traderefuse		, _T( "traderefuse"        ) , _T( "tr"        ) , _T( "거래거절"       ) , _T( "거절"				), TCM_CLIENT   , AUTH_GENERAL       ,  "거래 거절 [/명령] " )

	ON_TEXTCMDFUNC( TextCmd_whisperagree		, _T( "whisperagree"   ) , _T( "wa"        ) , _T( "귓속말승인"       ) , _T( "귓승"				), TCM_CLIENT   , AUTH_GENERAL       ,  "귓속말 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_whisperrefuse		, _T( "whisperrefuse"  ) , _T( "wr"        ) , _T( "귓속말거절"       ) , _T( "귓절"				), TCM_CLIENT   , AUTH_GENERAL       ,  "귓속말 거절 [/명령] " )

	ON_TEXTCMDFUNC( TextCmd_messengeragree		, _T( "messengeragree" ) , _T( "ma"        ) , _T( "메신저승인"       ) , _T( "메승"				), TCM_CLIENT   , AUTH_GENERAL       ,  "메신저 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_messengerrefuse		, _T( "messengerrefuse") , _T( "mr"        ) , _T( "메신저거절"       ) , _T( "메절"				), TCM_CLIENT   , AUTH_GENERAL       ,  "메신저 거절 [/명령] " )
	
	ON_TEXTCMDFUNC( TextCmd_stageagree		, _T( "stageagree"         ) , _T( "ga"        ) , _T( "극단승인"       ) , _T( "극승"				), TCM_CLIENT   , AUTH_GENERAL       ,  "극단 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_stagerefuse		, _T( "stagerefuse"        ) , _T( "gr"        ) , _T( "극단거절"       ) , _T( "극절"				), TCM_CLIENT   , AUTH_GENERAL       ,  "극단 거절 [/명령] " )

	ON_TEXTCMDFUNC( TextCmd_connectagree		, _T( "connectagree"   ) , _T( "ca"        ) , _T( "접속알림"       ) , _T( "접알"				), TCM_CLIENT   , AUTH_GENERAL       ,  "접속알림 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_connectrefuse		, _T( "connectrefuse"  ) , _T( "cr"        ) , _T( "접속알림해제"   ) , _T( "접해"				), TCM_CLIENT   , AUTH_GENERAL       ,  "접속알림 해제 [/명령] " )
	
	ON_TEXTCMDFUNC( TextCmd_shoutagree		, _T( "shoutagree"         ) , _T( "ha"        ) , _T( "외치기승인"       ) , _T( "외승"				), TCM_CLIENT   , AUTH_GENERAL       ,  "외치기 승인 [/명령] " )
	ON_TEXTCMDFUNC( TextCmd_shoutrefuse		, _T( "shoutrefuse"        ) , _T( "hr"        ) , _T( "외치기해제"       ) , _T( "외해"				), TCM_CLIENT   , AUTH_GENERAL       ,  "외치기 거절 [/명령] " )
#endif // __Y0601


//	ON_TEXTCMDFUNC( TextCmd_Gamemasay		, _T( "gamemashout"	   ) , _T( "gmsay"     ) , _T( "건의"		  ) , _T( "건의"			), TCM_SERVER   , AUTH_GENERAL       ,  "건의 : 영자귓속말" )
 	ON_TEXTCMDFUNC( TextCmd_Music			, _T( "music"          ) , _T( "mu"        ) , _T( "음악"         ) , _T( "음악"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "배경음악" )
 	ON_TEXTCMDFUNC( TextCmd_Sound			, _T( "sound"          ) , _T( "so"        ) , _T( "소리"         ) , _T( "소리"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "사운드 효과" )
 	ON_TEXTCMDFUNC( TextCmd_Summon			, _T( "summon"         ) , _T( "su"        ) , _T( "소환"         ) , _T( "소환"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "유저소환" )
 	ON_TEXTCMDFUNC( TextCmd_Teleport		, _T( "teleport"       ) , _T( "te"        ) , _T( "텔레포트"     ) , _T( "텔레"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "텔레포트" )
 	ON_TEXTCMDFUNC( TextCmd_Out				, _T( "out"            ) , _T( "out"       ) , _T( "퇴출"         ) , _T( "퇴출"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "퇴출" )
	ON_TEXTCMDFUNC( TextCmd_RemoveNpc		, _T( "rmvnpc"         ) , _T( "rn"        ) , _T( "삭제"         )	, _T( "삭"              ), TCM_BOTH     , AUTH_GAMEMASTER    ,	"삭제" )
 	ON_TEXTCMDFUNC( TextCmd_Hit				, _T( "hit"            ) , _T( "hit"       ) , _T( "히트"         ) , _T( "히트"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "히트포인트 업" )
 	ON_TEXTCMDFUNC( TextCmd_Mana			, _T( "mana"           ) , _T( "mana"      ) , _T( "마나"         ) , _T( "마나"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "마나포인트 업" )
 	ON_TEXTCMDFUNC( TextCmd_Fatigue			, _T( "fatigue"        ) , _T( "fat"       ) , _T( "피로"         ) , _T( "피로"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "패티규 포인트 업" )
	ON_TEXTCMDFUNC( TextCmd_CreateItem		, _T( "createitem"     ) , _T( "ci"        ) , _T( "아이템생성"   ) , _T( "아생"			), TCM_BOTH     , AUTH_ADMINISTRATOR ,  "아이템생성" )
#ifdef __RANDOMOPTITEM0628
	ON_TEXTCMDFUNC( TextCmd_CreateItem2		, _T( "createitem2"     ) , _T( "ci2"        ) , _T( "아이템생성2"   ) , _T( "아생2"			), TCM_BOTH     , AUTH_ADMINISTRATOR ,  "아이템생성2" )
#endif	// __RANDOMOPTEITEM0628
 	ON_TEXTCMDFUNC( TextCmd_CreateNPC		, _T( "createnpc"      ) , _T( "cn"        ) , _T( "엔피씨생성"   ) , _T( "엔생"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "npc생성" )
#ifdef __DUNGEON_1118
	ON_TEXTCMDFUNC( TextCmd_CreateCtrl		, _T( "createctrl"	)	,	_T( "ct"        ) , _T( "컨트롤생성"   )	,	_T( "컨생"	)	,	TCM_SERVER	,	AUTH_GAMEMASTER	,	"ctrl생성" )
#endif	// __DUNGEON_1118
#ifdef __J0204
	ON_TEXTCMDFUNC( TextCmd_LocalEvent	, _T( "localevent"      ) , _T( "le"        ) , _T( "지역이벤트"   ) , _T( "지이"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "지역이벤트" )
#endif	// __J0204
 	ON_TEXTCMDFUNC( TextCmd_FallSnow        , _T( "FallSnow"       ) , _T( "fs"        ) , _T( "눈와라"       ) , _T( "눈와"    		), TCM_SERVER   , AUTH_GAMEMASTER    ,  "눈 내리기 토글" )	
	ON_TEXTCMDFUNC( TextCmd_FallRain        , _T( "FallRain"       ) , _T( "fr"        ) , _T( "비와라"       ) , _T( "비와"	   		), TCM_SERVER   , AUTH_GAMEMASTER    ,  "비 내리기 토글"  )	
	ON_TEXTCMDFUNC( TextCmd_StopSnow        , _T( "StopSnow"       ) , _T( "ss"        ) , _T( "눈그만"       ) , _T( "눈끝"    		), TCM_SERVER   , AUTH_GAMEMASTER    ,  "눈 내리기 못하게 토글" )	
	ON_TEXTCMDFUNC( TextCmd_StopRain        , _T( "StopRain"       ) , _T( "sr"        ) , _T( "비그만"       ) , _T( "비끝"	   		), TCM_SERVER   , AUTH_GAMEMASTER    ,  "비 내리기 못하게 토글"  )	
	//ON_TEXTCMDFUNC( TextCmd_DeleteNPC		, _T( "deletenpc"      ) , _T( "dn"        ) , _T( "엔피씨삭제"   ) , _T( "엔삭"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "npc삭제" )
 	ON_TEXTCMDFUNC( TextCmd_Time           	, _T( "Time"           ) , _T( "ti"        ) , _T( "시간"         ) , _T( "시"			    ), TCM_CLIENT   , AUTH_GENERAL	     ,  "시간 보기 [/시간]" )


 	ON_TEXTCMDFUNC( TextCmd_CheckInventory	, _T( "checkinventory" ) , _T( "chi"       ) , _T( "소지품확인"   ) , _T( "소확"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "상대인벤토리창열기" )
 	ON_TEXTCMDFUNC( TextCmd_CheckState		, _T( "checkstate"     ) , _T( "chs"       ) , _T( "상태확인"     ) , _T( "상확"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "상대상태창열기" )
 	ON_TEXTCMDFUNC( TextCmd_Invisible		, _T( "invisible"      ) , _T( "inv"       ) , _T( "투명"         ) , _T( "투명"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "투명화" )
 	ON_TEXTCMDFUNC( TextCmd_NoInvisible		, _T( "noinvisible"    ) , _T( "noinv"     ) , _T( "투명해제"     ) , _T( "투해"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "투명화 해제" )
 	ON_TEXTCMDFUNC( TextCmd_Undying			, _T( "undying"        ) , _T( "ud"        ) , _T( "무적"         ) , _T( "무"			    ), TCM_BOTH     , AUTH_GAMEMASTER    ,  "무적" )
	ON_TEXTCMDFUNC( TextCmd_Undying2		, _T( "undying2"       ) , _T( "ud2"       ) , _T( "반무적"       ) , _T( "반무"			), TCM_BOTH     , AUTH_GAMEMASTER    ,  "반무적" )
	ON_TEXTCMDFUNC( TextCmd_NoUndying		, _T( "noundying"      ) , _T( "noud"      ) , _T( "무적해제"     ) , _T( "무해"			), TCM_BOTH     , AUTH_GAMEMASTER    ,  "무적 해제" )
	//ON_TEXTCMDFUNC( TextCmd_NoUndying2		, _T( "noundying2"     ) , _T( "noud2"     ) , _T( "무적해제2"    ) , _T( "무해2"			), TCM_BOTH     , AUTH_GAMEMASTER    ,  "무적 해제2" )
 	ON_TEXTCMDFUNC( TextCmd_Save			, _T( "save"           ) , _T( "save"      ) , _T( "저장"         ) , _T( "저장"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "세이브" )
#ifdef __VOPTEQUIP
	ON_TEXTCMDFUNC( TextCmd_CreateChar		, _T( "createchar"     ) , _T( "cc"        ) , _T( "캐릭터생성"   ) , _T( "캐생"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "캐릭터생성" )
#endif
 	ON_TEXTCMDFUNC( TextCmd_Disguise		, _T( "disguise"       ) , _T( "dis"       ) , _T( "변신"         ) , _T( "변"	            ), TCM_SERVER   , AUTH_GAMEMASTER   ,  "변신" )
 	ON_TEXTCMDFUNC( TextCmd_Dwarpet 		, _T( "dwarpet"        ) , _T( "dwar"      ) , _T( "드워펫"       ) , _T( "드" 	            ), TCM_SERVER   , AUTH_GAMEMASTER    ,  "마스터 드워펫으로 변신" )
	ON_TEXTCMDFUNC( TextCmd_Iblis   		, _T( "iblis"          ) , _T( "ibl"       ) , _T( "이블리스"     ) , _T( "이불"            ), TCM_SERVER   , AUTH_GAMEMASTER   ,  "이블리스로 변신" )
	ON_TEXTCMDFUNC( TextCmd_Shade   		, _T( "shade"          ) , _T( "sha"       ) , _T( "쉐이드"       ) , _T( "쉐이"            ), TCM_SERVER   , AUTH_GAMEMASTER   ,  "쉐이드로 변신" )
	ON_TEXTCMDFUNC( TextCmd_Bubble   	    , _T( "bubble"         ) , _T( "bub"       ) , _T( "버블"         ) , _T( "버블"            ), TCM_SERVER   , AUTH_GAMEMASTER   ,  "버블로 변신" )

	ON_TEXTCMDFUNC( TextCmd_NoDisguise   	, _T( "noDisguise"     ) , _T( "nodis"     ) , _T( "변신해제"     ) , _T( "변해"            ), TCM_SERVER   , AUTH_GAMEMASTER    ,  "변신 해제" )
 	ON_TEXTCMDFUNC( TextCmd_Freeze			, _T( "freeze"         ) , _T( "fr"        ) , _T( "정지"         ) , _T( "정지"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "움직이지 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_NoFreeze		, _T( "nofreeze"       ) , _T( "nofr"      ) , _T( "정지해제"     ) , _T( "정해"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "움직이지 못하게 하기 해제" )
	ON_TEXTCMDFUNC( TextCmd_NoTalk			, _T( "notalk"		   ) , _T( "ta"		   ) , _T( "말정지"		  ) , _T( "말정"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "말하지 못하게 하기" )
	ON_TEXTCMDFUNC( TextCmd_Talk			, _T( "talk"		   ) , _T( "nota"      ) , _T( "말해제"		  ) , _T( "말해"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "말하지 못하게 하기 해제" )
	ON_TEXTCMDFUNC( TextCmd_GetGold			, _T( "getgold"		   ) , _T( "gg"		   ) , _T( "돈줘"		  ) , _T( "돈"				), TCM_SERVER   , AUTH_ADMINISTRATOR ,  "돈 얻기" )
 	ON_TEXTCMDFUNC( TextCmd_indirect		, _T( "indirect"       ) , _T( "id"        ) , _T( "간접"         ) , _T( "간접"			), TCM_BOTH     , AUTH_GAMEMASTER    ,  "상대에게 간접으로 말하게 하기" )
 	ON_TEXTCMDFUNC( TextCmd_Onekill			, _T( "onekill"        ) , _T( "ok"        ) , _T( "초필"         ) , _T( "초필"			), TCM_BOTH     , AUTH_GAMEMASTER    ,  "적을 한방에 죽이기" )
	ON_TEXTCMDFUNC( TextCmd_NoOnekill		, _T( "noonekill"      ) , _T( "nook"      ) , _T( "초필해제"     ) , _T( "초해"			), TCM_BOTH     , AUTH_GAMEMASTER    ,  "적을 한방에 죽이기 해제" )
 	ON_TEXTCMDFUNC( TextCmd_ip				, _T( "ip"             ) , _T( "ip"        ) , _T( "아이피"       ) , _T( "아이피"			), TCM_BOTH   , AUTH_OPERATOR      ,  "상대 IP알기" )
 	ON_TEXTCMDFUNC( TextCmd_userlist		, _T( "userlist"       ) , _T( "ul"        ) , _T( "사용자리스트" ) , _T( "사용자리"    	), TCM_SERVER   , AUTH_ADMINISTRATOR ,  "사용자 리스트" )
 	ON_TEXTCMDFUNC( TextCmd_count			, _T( "count"          ) , _T( "cnt"       ) , _T( "접속자수"     ) , _T( "접속자수"		), TCM_SERVER   , AUTH_GAMEMASTER    ,  "접속자 카운트" )
	ON_TEXTCMDFUNC( TextCmd_System			, _T( "system"         ) , _T( "sys"       ) , _T( "알림"         ) , _T( "알"				), TCM_SERVER   , AUTH_GAMEMASTER    ,  "시스템 메시지" )
//#ifdef __ONLINE
	ON_TEXTCMDFUNC( TextCmd_LoadScript      , _T( "loadscript"     ) , _T( "loscr"     ) , _T( "로드스크립트" ) , _T( "로스"            ), TCM_BOTH     , AUTH_GAMEMASTER    ,  "스크립트 다시 읽기" )
//#else
//	ON_TEXTCMDFUNC( TextCmd_LoadScript      , _T( "loadscript"     ) , _T( "loscr"     ) , _T( "로드스크립트" ) , _T( "로스"            ), TCM_SERVER   , AUTH_GAMEMASTER    ,  "스크립트 다시 읽기" )
//#endif
	ON_TEXTCMDFUNC( TextCmd_Level			, _T( "level"			) , _T( "lv"		) , _T( "레벨"			) , _T( "렙"			) , TCM_SERVER	, AUTH_GAMEMASTER	 ,	"레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_stat			, _T( "stat"			) , _T( "stat"		) , _T( "스탯"			) , _T( "스탯"			) , TCM_SERVER	, AUTH_GAMEMASTER	 ,	"스탯 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_SkillLevel		, _T( "skilllevel"		) , _T( "slv"		) , _T( "스킬레벨"		) , _T( "스렙"			) , TCM_BOTH	, AUTH_GAMEMASTER	 ,	"스킬레벨 설정 하기" )
	ON_TEXTCMDFUNC( TextCmd_ResistItem		, _T( "ResistItem"      ) , _T( "ritem"     ) , _T( "속성아이템"    ) , _T( "속아"			) , TCM_BOTH    , AUTH_ADMINISTRATOR ,  "속성아이템" ) 
	ON_TEXTCMDFUNC( TextCmd_Piercing		, _T( "Piercing"      ) , _T( "pier"     ) , _T( "피어싱"    ) , _T( "피싱"			) , TCM_BOTH    , AUTH_ADMINISTRATOR ,  "피어싱(소켓)" ) 
#ifdef __11_MONTH_JOB_PRO
	ON_TEXTCMDFUNC( TextCmd_InitSkillExp	, _T( "InitSkillExp"    ) , _T( "InitSE"    ) , _T( "스킬초기화"    ) , _T( "스초"			) , TCM_SERVER  , AUTH_GAMEMASTER    ,  "스킬초기화" ) 
#endif // __11_MONTH_JOB_PRO

	ON_TEXTCMDFUNC( TextCmd_CommercialElem	, _T( "CommercialElem"  ) , _T( "CommercialElem" ) , _T( "속성강화창"    ) , _T( "속강"			) , TCM_CLIENT  , AUTH_GAMEMASTER    ,  "속성강화창 띄우기" ) 

	ON_TEXTCMDFUNC( TextCmd_ItemEvent	    , _T( "ItemEvent"	    ) , _T( "ie"		) , _T( "아이템이벤트"    ) , _T( "아이"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "아이템이벤트"  )
	
#ifdef __S1020_CHANGEFACE
	ON_TEXTCMDFUNC( TextCmd_ChangeFace		, _T( "ChangeFace"		) , _T( "cf"		) , _T( "얼굴변경"		  ) , _T( "얼변"			) , TCM_BOTH  , AUTH_GENERAL				 , "얼굴 변경"  )
#endif // __S1020_CHANGEFACE

	ON_TEXTCMDFUNC( TextCmd_X3				, _T( "ex"				) , _T( "EX"		) , _T( "경험치이벤트"  ) , _T( "경이"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "경험치, 페냐, 아이템 드롭 배수"  )
	ON_TEXTCMDFUNC( TextCmd_AroundKill		, _T( "aroundkill"		) , _T( "ak"		) , _T( "원샷"			) , _T( "원"			) , TCM_SERVER  , AUTH_GAMEMASTER	 , "어라운드에 있는 몬스터 죽이기"  )
#ifdef __LPARTYLEVEL0603
	ON_TEXTCMDFUNC( TextCmd_PartyLevel		, _T( "PartyLevel"		) , _T( "plv"		) , _T( "극단레벨"      ) , _T( "극레"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "극단레벨 설정 하기"  )
#endif

#ifdef __J0519
	ON_TEXTCMDFUNC( TextCmd_QuerySetPlayerName	, _T( "SetPlayerName"		) , _T( "spn"		) , _T( "플레이어이름"      ) , _T( "플이"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "플레이어 이름 변경"  )
#endif	// __J0519
#ifdef __J0524
	ON_TEXTCMDFUNC( TextCmd_QuerySetGuildName	, _T( "SetGuildName"		) , _T( "sgn"		) , _T( "길드이름"      ) , _T( "길이"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "길드 이름 변경"  )
#endif	// __J0524

#ifdef __J0601
	ON_TEXTCMDFUNC( TextCmd_SetSnoop	, _T( "Snoop"		) , _T( "snoop"		) , _T( "감청"      ) , _T( "감청"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "감청"  )
	ON_TEXTCMDFUNC( TextCmd_SetSnoopGuild	, _T( "SnoopGuild"		) , _T( "sg"		) , _T( "길드대화저장"      ) , _T( "길저"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "길드 대화 저장"  )
#endif	// __J0601

#ifdef __INSTANCE_DUNGEON
	ON_TEXTCMDFUNC( TextCmd_Runeq	, _T( "runeq"		)	,	_T( "runeq" )	,	_T( "runeq" )	,	_T( "runeq" )	,	TCM_SERVER	,	AUTH_GAMEMASTER	,	"runeq"  )
#endif	// __INSTANCE_DUNGEON

#ifdef __NEWGUILD
	ON_TEXTCMDFUNC( TextCmd_GuildStat		, _T( "GuildStat"		) , _T( "gstat"		) , _T( "길드스탯"      ) , _T( "길스탯"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "길드 스탯변경"  )
	ON_TEXTCMDFUNC( TextCmd_SetGuildQuest	, _T( "SetGuildQuest"	) , _T( "sgq"		) , _T( "길드퀘스트"      ) , _T( "길퀘"          ) , TCM_SERVER  , AUTH_GAMEMASTER	 , "길드 퀘스트 상태 변경"  )

	ON_TEXTCMDFUNC( TextCmd_GuildChat	, _T( "GuildChat" )	, _T( "g" )	,	_T( "길드말" )	,	_T( "길말" ),		TCM_SERVER	,	AUTH_GENERAL	,	"길드말"	)

	ON_TEXTCMDFUNC( TextCmd_DeclWar	, _T( "DeclWar" )	, _T( "declwar" )	,	_T( "길드전신청" )	,	_T( "길신" ),		TCM_CLIENT	,	AUTH_GAMEMASTER		,	"길드전 신청"	)
// 임시 코드
	ON_TEXTCMDFUNC( TextCmd_CreateGuild		, _T( "createguild" )	, _T( "cg" )	, _T( "길드생성" )	, _T( "길생" ),	TCM_SERVER  ,	AUTH_GAMEMASTER, "길드 생성"  )
	ON_TEXTCMDFUNC( TextCmd_DestroyGuild		, _T( "destroyguild" )	, _T( "dg" )	, _T( "길드해체" )	, _T( "길해" ),	TCM_CLIENT  ,	AUTH_GAMEMASTER, "길드 해체"  )
	ON_TEXTCMDFUNC( TextCmd_RemoveGuildMember	, _T( "rgm" )	, _T( "rgm" )	, _T( "길드추방" )	, _T( "길추" ),	TCM_CLIENT  ,	AUTH_GAMEMASTER, "길드 추방"  )
	
#ifdef __H0621_GUILDRANK__
	ON_TEXTCMDFUNC( TextCmd_GuildRanking		, _T( "GuildRanking") , _T( "ranking"      ) , _T( "길랭"         ) , _T( "길랭"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "" )
#	ifdef __H0809_GUILDRANK__
	ON_TEXTCMDFUNC( TextCmd_GuildRankingDBUpdate, _T( "GuildRankingUpdate") , _T( "rankingupdate"      ) , _T( "길업"         ) , _T( "길업"			), TCM_SERVER   , AUTH_GAMEMASTER    ,  "" )
#	endif
#endif//__H0621_GUILDRANK__
	
	
#endif
#ifdef	__SCHOOL0701
	ON_TEXTCMDFUNC( TextCmd_sbready	, _T( "sbready" )	,	_T( "sbready" )	,	_T( "sbready" )	,	_T( "sbready" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "sbready"  )
	ON_TEXTCMDFUNC( TextCmd_sbstart	, _T( "sbstart" )	,	_T( "sbstart" )	,	_T( "sbstart" )	,	_T( "sbstart" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "sbstart"  )
	ON_TEXTCMDFUNC( TextCmd_sbstart2	, _T( "sbstart2" )	,	_T( "sbstart2" )	,	_T( "sbstart2" )	,	_T( "sbstart2" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "sbstart2"  )
	ON_TEXTCMDFUNC( TextCmd_sbend	, _T( "sbend" )	,	_T( "sbend" )	, _T( "sbend" )	,	_T( "sbend" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "sbend"  )
	ON_TEXTCMDFUNC( TextCmd_sbreport	, _T( "sbreport" )	,	_T( "sbreport" )	, _T( "sbreport" )	,	_T( "sbreport" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "sbreport"  )
	ON_TEXTCMDFUNC( TextCmd_ItemMode	, _T( "gmitem" )	,	_T( "gmitem" )	, _T( "아이템모드" )	,	_T( "아모" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "아이템 못집고 못떨어트리게 하기"  )
	ON_TEXTCMDFUNC( TextCmd_ItemNotMode	, _T( "gmnotitem" )	,	_T( "gmnotitem" )	, _T( "아이템해제" )	,	_T( "아모해" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "아이템 모드 해제"  )
	ON_TEXTCMDFUNC( TextCmd_AttackMode	, _T( "gmattck" )	,	_T( "gmattck" )	, _T( "공격모드" )	,	_T( "공모" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "공격 못하게 하기"  )
	ON_TEXTCMDFUNC( TextCmd_AttackNotMode	, _T( "gmnotattck" )	,	_T( "gmnotattck" )	, _T( "공격해제" )	,	_T( "공모해" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "공격 모드 해제"  )
	ON_TEXTCMDFUNC( TextCmd_CommunityMode	, _T( "gmcommunity" )	,	_T( "gmcommunity" )	, _T( "커뮤니티모드" )	,	_T( "커모" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "길드, 파티, 친구, 거래, 상점 못하게 하기"  )
	ON_TEXTCMDFUNC( TextCmd_CommunityNotMode	, _T( "gmnotcommunity" )	,	_T( "gmnotcommunity" )	, _T( "커뮤니티해제" )	,	_T( "커모해" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "커뮤니티 모드 해체"  )
	ON_TEXTCMDFUNC( TextCmd_ObserveMode	, _T( "gmobserve" )	,	_T( "gmobserve" )	, _T( "관전모드" )	,	_T( "관모" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "아이템, 커뮤니티, 말못하게, 어텍 모드 합한것"  )
	ON_TEXTCMDFUNC( TextCmd_ObserveNotMode	, _T( "gmnotobserve" )	,	_T( "gmnotobserve" )	, _T( "관전해제" )	,	_T( "관모해" ),	TCM_SERVER	,	AUTH_GAMEMASTER, "관전 모드 해제"  )
#endif	// __SCHOOL0701

	ON_TEXTCMDFUNC( TextCmd_EscapeReset	, _T( "EscapeReset" )	,	_T( "EscapeReset" )	, _T( "탈출초기화" )	,	_T( "탈초" ),	TCM_BOTH	,	AUTH_GAMEMASTER, "탈출(귀환석) 시간 초기화"  )
	
#ifdef __SERVERBATTLE0828
	ON_TEXTCMDFUNC( TextCmd_OpenBattleServer,	_T( "bsopen" )	,	_T( "bsopen" )	,	_T( "bsopen" )	,	_T( "bsopen" ),	TCM_SERVER	,	AUTH_GAMEMASTER,	"bsopen"	)
	ON_TEXTCMDFUNC( TextCmd_CloseBattleServer,	_T( "bsclose" )	,	_T( "bsclose" )	,	_T( "bsclose" )	,	_T( "bsclose" ),	TCM_SERVER	,	AUTH_GAMEMASTER,	"bsclose"	)
//	ON_TEXTCMDFUNC( TextCmd_JoinBattleServer,	_T( "bsjoin" )	,	_T( "bsjoin" )	,	_T( "참가" )	,	_T( "참가" ),	TCM_SERVER	,	AUTH_GENERAL,	"참가"	)
#endif	// __SERVERBATTLE0828

#ifdef __S1109_GETJOB_COMMAND
	ON_TEXTCMDFUNC( TextCmd_JobName			,	_T( "jobname"	)	,	_T( "jn"	)	,	_T( "직업이름"	)	,	_T( "직이"	), TCM_CLIENT   , AUTH_GAMEMASTER	     ,  "직업이름 보기" )
#endif // __S1109_GETJOB_COMMAND

#ifdef __S1108_BACK_END_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_RespawnMonster	,	_T( "respawnsetting")	,	_T( "rs"	)	,	_T( "리스폰설정")	,	_T( "리설"	), TCM_SERVER   , AUTH_GAMEMASTER	    ,  "리스폰 설정 보기" )
//	ON_TEXTCMDFUNC( TextCmd_RemoveRespawnMonster	,	_T( "removerespawnmonster")	,	_T( "rrm"	)	,	_T( "리스폰영역지우기")	,	_T( "리영지"	), TCM_SERVER   , AUTH_GAMEMASTER	     ,  "리스폰 몬스터 영역 지우기" )
	ON_TEXTCMDFUNC( TextCmd_PropMonster		,	_T( "monstersetting")	,	_T( "ms"	)	,	_T( "몬스터설정")	,	_T( "몬설"	), TCM_CLIENT   , AUTH_GAMEMASTER	    ,  "몬스터 설정 보기" )
#endif // __S1108_BACK_END_SYSTEM
	ON_TEXTCMDFUNC( TextCmd_GameSetting		,	_T( "gamesetting"  )	,	_T( "gs"	)	,	_T( "게임설정"  )	,	_T( "게설"	), TCM_SERVER	, AUTH_GAMEMASTER		, "게임 설정 보기"  )	
#ifdef __S1012_SHOPCOST
	#ifndef __S1108_BACK_END_SYSTEM
		ON_TEXTCMDFUNC( TextCmd_ChangeShopCost  ,	_T( "changeshopcost"  ) ,	_T( "csc"	)	,	_T( "상점가격조정"    ) , _T( "상가조") , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "상점가격조정 Min(0.5) ~ Max(2.0)"  )
		ON_TEXTCMDFUNC( TextCmd_SetMonsterRespawn,	_T( "setmonsterrespawn") ,	_T( "smr"	)	,	_T( "리스폰영역설정"  ) , _T( "리영설") , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "리스폰 영역 설정"  )
	#endif // __S1108_BACK_END_SYSTEM
#endif // __S1012_SHOPCOST
#ifdef __VCONSTANT
	#ifndef __S1108_BACK_END_SYSTEM
		ON_TEXTCMDFUNC( TextCmd_ItemDropRate	, _T( "ItemDropRate"    ) , _T( "idr"		) , _T( "아이템드롭률"    ) , _T( "아드롭"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "아이템 드롭률 조절 0.0 ~ 1.0 ~ 2.0 "  )
		ON_TEXTCMDFUNC( TextCmd_GoldDropRate 	, _T( "GoldDropRate"    ) , _T( "gdr"		) , _T( "골드드롭률"      ) , _T( "골드롭"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "골드 드롭률 조절 0.0 ~ 1.0 ~ 2.0 "  )
		ON_TEXTCMDFUNC( TextCmd_MonsterExpRate 	, _T( "MonsterExpRate"  ) , _T( "mer"		) , _T( "몬스터경험치률"  ) , _T( "몬경험치"		) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "몬스터 경험치률 조절 0.0 ~ 1.0 ~ 2.0 "  )
		ON_TEXTCMDFUNC( TextCmd_MonsterHitRate  , _T( "MonsterHitRate"  ) , _T( "mdr"		) , _T( "몬스터히트률"    ) , _T( "몬히트"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "몬스터 히트률 조절 0.0 ~ 1.0 ~ 2.0 "  )
	#endif // __S1108_BACK_END_SYSTEM
#endif
#ifdef __NPP_050308 
		ON_TEXTCMDFUNC( TextCmd_PvpParam	, _T( "PvpParam"    ) , _T( "p_Param"	) , _T( "PVP설정"    ) , _T( "피설") , TCM_SERVER  , AUTH_GAMEMASTER		 , "PVP(카오)설정"  )
		ON_TEXTCMDFUNC( TextCmd_RecoverMode	, _T( "RecoverMode"    ) , _T( "r_mode"	) , _T( "시련모드"    ) , _T( "시모") , TCM_CLIENT  , AUTH_GAMEMASTER		 , "시련모드설정"  )
#endif // __NPP_050308 
#ifdef __NPP_050308
#ifdef __V050322_CACHEQUEST
		ON_TEXTCMDFUNC( TextCmd_QuestState	, _T( "QuestState"     ) , _T( "qs"		) , _T( "퀘스트상태"    ) , _T( "퀘상"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "퀘스트 설정 [ID] [State]"  )
		ON_TEXTCMDFUNC( TextCmd_EndQuest	, _T( "EndQuest"       ) , _T( "eq"		) , _T( "퀘스트종료"    ) , _T( "퀘종"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "퀘스트 종료 [ID]"  )
		ON_TEXTCMDFUNC( TextCmd_BeginQuest	, _T( "BeginQuest"     ) , _T( "bq"		) , _T( "퀘스트시작"    ) , _T( "퀘시"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "퀘스트 시작 [ID]"  )
		ON_TEXTCMDFUNC( TextCmd_RemoveQuest , _T( "RemoveQuest"    ) , _T( "rq"		) , _T( "퀘스트제거"    ) , _T( "퀘제"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "퀘스트 제거 [ID]"  )
		ON_TEXTCMDFUNC( TextCmd_RemoveAllQuest , _T( "RemoveAllQuest"    ) , _T( "raq"		) , _T( "퀘스트전체제거"    ) , _T( "퀘전제"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "퀘스트 전체 제거"  )
		ON_TEXTCMDFUNC( TextCmd_RemoveCompleteQuest , _T( "RemoveCompleteQuest"    ) , _T( "rcq"		) , _T( "퀘스트완료제거"    ) , _T( "퀘완제"			) , TCM_SERVER  , AUTH_ADMINISTRATOR		 , "퀘스트 완료 제거"  )
#endif
#endif
END_TEXTCMDFUNC_MAP


int ParsingCommand( LPCTSTR lpszString, CMover* pMover )
{
	CScanner scanner;
	scanner.SetProg( (LPTSTR)lpszString );
//#ifdef __WORLDSERVER
	scanner.dwValue	= (DWORD)pMover;
//#endif
	scanner.GetToken(); // skip /
	scanner.GetToken(); // get command
	int nCount = 0;
	while( m_textCmdFunc[ nCount ].m_pFunc )
	{
		TextCmdFunc* pTextCmdFunc = &m_textCmdFunc[nCount];
		if( scanner.Token == pTextCmdFunc->m_pCommand || scanner.Token == pTextCmdFunc->m_pAbbreviation ||
			scanner.Token == pTextCmdFunc->m_pKrCommand || scanner.Token == pTextCmdFunc->m_pKrAbbreviation )

		{
#ifndef __DEV
			if( pTextCmdFunc->m_dwAuthorization <= pMover->m_dwAuthorization )
#endif	// __DEV
			{
#ifdef __CLIENT
  #ifdef __ONLINE
				if( pTextCmdFunc->m_nServer == TCM_CLIENT || pTextCmdFunc->m_nServer == TCM_BOTH )
				{
					if( ( *pTextCmdFunc->m_pFunc )( scanner ) )
						if( pTextCmdFunc->m_nServer == TCM_BOTH )
						{
							char szSendChat[MAX_PATH];
							sprintf( szSendChat, "%s", scanner.m_pBuf );
							g_DPlay.SendChat( (LPCSTR)szSendChat );
						}
				}
				else
					g_DPlay.SendChat( (LPCSTR)lpszString );
  #else	// __ONLINE
				( *pTextCmdFunc->m_pFunc )( scanner );
  #endif	// __ONLINE
#else	// __CLIENT
				if( pTextCmdFunc->m_nServer == TCM_SERVER ||  pTextCmdFunc->m_nServer == TCM_BOTH )
					( *pTextCmdFunc->m_pFunc )( scanner );
				else
					;// 서버에서, 클라이언 명령이 발견되었다. 이런 일은 있을 수 없지만....
#endif	// __CLIENT
				return TRUE;
			}
			break;
		}
		nCount++;
	}

#ifdef __YEMOTICON_0223
	// 이모티콘 명령
	for( int j=0; j < MAX_EMOTICON_NUM; j++ )
	{
		if( stricmp( scanner.token, g_DialogMsg.m_EmiticonCmd[ j ].m_szCommand ) == 0 )
		{
#ifdef __ONLINE
			g_DPlay.SendChat( (LPCSTR)lpszString );
#endif //__ONLINE
			return TRUE;
		}
	}
#endif //__YEMOTICON_0223

	return FALSE;
}








