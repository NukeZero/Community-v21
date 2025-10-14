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

#include "../DatabaseServer/dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;

#include "eveschool.h"

#include "serialnumber.h"
#include "../DatabaseServer/DBSerialNumberRequest.h"

#include "post.h"

#include "playerdata.h"

#include "../DatabaseServer/GuildHouseDBCtrl.h"

#include "../DatabaseServer/CampusDBCtrl.h"

#include "lang.h"

#include "../_CommonDefine/FLTm.h"
#include "../_CommonDefine/FLPacketStruct.h"
#include "../_CommonDefine/FLCommonCount.h"

#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"

#include "../_Common/FLDBCommonFuntion.h"

#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "../DatabaseServer/Resource.h"

#include "FLDBConsignmentSale.h"

#include "FLDataBase.h"
#include "FLItemGoldLog_Query.h"

#include "../_CommonDefine/Packet/FLPacketGuildBankUpdate.h"


//#include "../_Common/Item.h"

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;

static const TCHAR	LOG_SEND_ITEM[]	= _T( "SendItem" );
static const TCHAR	LOG_BANK_TO_ITEM_SEND_TBL[]	= _T( "BankToItemSendTbl" );
static const TCHAR	LOG_INVENTORY_TO_ITEM_SEND_TBL[]	= _T( "InventoryToItemSendTbl" );
static const TCHAR	LOG_RESULT[]	= _T( "Result" );
static const TCHAR	LOG_INVENTORY_TBL[]	= _T( "INVENTORY_TBL" );
static const TCHAR	LOG_INVENTORY_EXT_TBL[]	= _T( "INVENTORY_EXT_TBL" );
static const TCHAR	LOG_BANK_TBL[]	= _T( "BANK_TBL" );
static const TCHAR	LOG_BANK_EXT_TBL[]	= _T( "BANK_EXT_TBL" );
static const TCHAR	LOG_GUILD_BANK_TBL[]	= _T( "GUILD_BANK_TBL" );
static const TCHAR	LOG_GUILD_BANK_EXT_TBL[]	= _T( "GUILD_BANK_EXT_TBL" );
static const TCHAR	LOG_TBL_POCKET[]	= _T( "tblPocket" );
static const TCHAR	LOG_TBL_POCKET_EXT[]	= _T( "tblPocketExt" );
//static const TCHAR	LOG_QUERY_ITEM_ID[]	= _T( "queryItemid" );
static const TCHAR	LOG_IR[]	= _T( "ir" );
static const TCHAR	LOG_QUERY[]	= _T( "query" );
static const TCHAR	LOG_QUERY_RESTORE_PET[]	= _T( "queryRestorePet" );


CDbManager & g_DbManager = CDbManager::GetInstance();							// CDbManager 클래스 생성

// MAX_TASKBAR 300일경우 MAX_APPLETTASKBAR = 5400, MAX_ITEMTASKBAR = 10800
// MAX_TASKBAR 172일경우 MAX_APPLETTASKBAR = 3096, MAX_ITEMTASKBAR = 6192
extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];


#define	SAFE_DELETE_FREEREQUEST( pDBOverlappedPlus )	\
	if( pDBOverlappedPlus )	\
	{	\
		FreeRequest( pDBOverlappedPlus );	\
		pDBOverlappedPlus = NULL;	\
	}	\


CDbManager & CDbManager::GetInstance()
{
	static CDbManager DbManager;
	return DbManager;
}

CDbManager::CDbManager()
{
	memset( m_aszJoin, 0, sizeof(m_aszJoin) );
	m_nJoin	= 0;

	m_dwGuildBankUseWorldKey = NULL_ID;

	m_idPlayer	= 0;
	//	mulcom	BEGIN100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.
	m_pDbIOData		= new FLMemPooler<DB_OVERLAPPED_PLUS>( 1024 );
	//	mulcom	END100415	DB_OVERLAPPED_PLUS 메모리 풀 사용.


#ifdef __S1108_BACK_END_SYSTEM
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __S1108_BACK_END_SYSTEM

	m_hItemUpdateWorker = m_hItemUpdateCloseWorker = NULL;
	m_nItemUpdate = 0;
	m_RemoveItem_List.clear();
	memset( m_apQuery, 0, sizeof(m_apQuery) );

	DB_ADMIN_PASS_LOG[0] = '\0';
	DB_ADMIN_PASS_CHARACTER01[0] = '\0';
	DB_ADMIN_PASS_BACKSYSTEM[0] = '\0';
	DB_ADMIN_PASS_ITEMUPDATE[0] = '\0';
	m_cbTrade	= 0;
}

CDbManager::~CDbManager()
{
	Clear();	
}



void CDbManager::Clear( void )
{
	CloseDbWorkers();

	CMclAutoLock Lock( m_AddRemoveLock );

	m_AccountCacheMgr.Clear();

	SAFE_DELETE( m_pDbIOData );
}

void CDbManager::CreatePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, _countof( lpDbOverlappedPlus->AccountInfo.szAccount ) );
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, _countof( lpDbOverlappedPlus->AccountInfo.szPassword ) );

	BYTE nSlot, nFace, nCostume, nSkinSet, nHairMesh;
	DWORD dwHairColor;
	BYTE nSex, nJob, nHeadMesh;
	int nBankPW = 0;

	//	 CHARACTER_STR 'I1',@im_idPlayer,@iserverindex,@iaccount,@im_szName,@iplayerslot,@idwWorldID,@im_dwIndex,@im_vPos_x,@im_vPos_y,@im_vPos_z,
	//	 @im_szCharacterKey,@im_dwSkinSet,@im_dwHairMesh,@im_dwHairColor,@im_dwHeadMesh,@im_dwSex
	// 	 CHARACTER_STR 'I1','','01','beat','샛별공주',0,0,0,0,0,0,'',0,0,0,0,0
	arRead >> nSlot;
	if( nSlot < 0 || nSlot >= MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Character Slot Invalid:[%d]" ), nSlot );
		return;
	}
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szPlayer, _countof( lpDbOverlappedPlus->AccountInfo.szPlayer ) );

	// 해킹이므로 무시
	if( prj.IsInvalidName( lpDbOverlappedPlus->AccountInfo.szPlayer ) || prj.IsAllowedLetter( lpDbOverlappedPlus->AccountInfo.szPlayer ) == FALSE )
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// mirchang_20100708 Character Name check
	if( g_xFlyffConfig->GetMainLanguage() == LANG_USA )
	{
		TCHAR szPlayerName[256] = {0, };
		//mems_et( szPlayerName, 0, sizeof( szPlayerName ) );
		FLStrncpy( szPlayerName, _countof( szPlayerName ), lpDbOverlappedPlus->AccountInfo.szPlayer, 255 );

		int nLen = _tcslen( szPlayerName );

		if( nLen == 0 )
		{
			return;
		}

		if( szPlayerName[0] >= '0' && szPlayerName[0] <= '9' )
		{
			return;
		}
		else
		{
			for( int i = 0; i < nLen; ++i )
			{
				if( ( szPlayerName[i] >= 'a' && szPlayerName[i] <= 'z' )
					|| ( szPlayerName[i] >= 'A' && szPlayerName[i] <= 'Z' )
					|| ( szPlayerName[i] >= '0' && szPlayerName[i] <= '9' ) )
				{
					// ok;
				}
				else
				{
					return;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	prj.Formalize( lpDbOverlappedPlus->AccountInfo.szPlayer, _countof( lpDbOverlappedPlus->AccountInfo.szPlayer ) );

	arRead >> nFace >> nCostume >> nSkinSet >> nHairMesh;
	arRead >> dwHairColor;
	arRead >> nSex >> nJob >> nHeadMesh;
	arRead >> nBankPW;

	DWORD dwAuthKey;
	arRead >> dwAuthKey;

	// BEGIN110520
	arRead >> lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage;
	// 현재로선 캐릭터 수 3개가 기본정책이므로 3개를 넘어가면 생성못함..
	if( lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Character Select Page is not default:[%d]" ), lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage );
		return;
	}
	// END110520

	DWORD dwIndex	= ( nSex == SEX_FEMALE ? OBJECT_INDEX( 12, MI_FEMALE ) : OBJECT_INDEX( 11, MI_MALE ) );
	D3DXVECTOR3	vPos;
	DWORD dwWorldID	= WI_WORLD_MADRIGAL;
	if( !g_appInfo.dwSys )
		dwWorldID	= WI_WORLD_EVENT01;

	if( prj.GetRandomBeginPos( dwWorldID, &vPos ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GetRandomBeginPos return false. szPlayer : %s" ), lpDbOverlappedPlus->AccountInfo.szPlayer );
		return;
	}

	if( nSex != SEX_FEMALE && nSex != SEX_MALE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nSex Invalid. szPlayer : %s, nSex : %d" ), lpDbOverlappedPlus->AccountInfo.szPlayer, nSex );
		return;
	}

	if( nBankPW < 0 || nBankPW > 9999 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nBankPW is Invalid! szPlayer : %s, nBankPW : %d" ), lpDbOverlappedPlus->AccountInfo.szPlayer, nBankPW );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	CMover Mover;
	FLStrcpy( Mover.m_szAccount, _countof( Mover.m_szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
	FLStrcpy( Mover.m_szName, _countof( Mover.m_szName ), lpDbOverlappedPlus->AccountInfo.szPlayer );
	Mover.m_nDBDataSlot		= nSlot;
	Mover.m_dwWorldID = dwWorldID;
	Mover.m_dwIndex = dwIndex;
	Mover.m_vPos.x = vPos.x;
	Mover.m_vPos.y = vPos.y;
	Mover.m_vPos.z = vPos.z;
	Mover.m_dwSkinSet = nSkinSet;
	Mover.m_dwHairMesh = nHairMesh;
	Mover.m_dwHairColor = dwHairColor;
	Mover.m_dwHeadMesh = nHeadMesh;
	Mover.SetSex( nSex );

	T_CHARACTER_STR_INFO tCharacter( Mover, "I1" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "I1" );
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
// 	FLStrcpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), lpDbOverlappedPlus->AccountInfo.szPlayer );
// 
// 	// BEGIN110520
// 	tCharacter.nPlayerSlot = nSlot;
// //	tCharacter.nPlayerSlot = nSlot + ( lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage * MAX_CHARACTER_SLOT );
// 	// END110520
// 
// 	tCharacter.dwWorldId = dwWorldID;
// 	tCharacter.dwIndex = dwIndex;
// 	tCharacter.fPos_x = vPos.x;
// 	tCharacter.fPos_y = vPos.y;
// 	tCharacter.fPos_z = vPos.z;
// 	tCharacter.dwSkinSet = nSkinSet;
// 	tCharacter.dwHairMesh = nHairMesh;
// 	tCharacter.dwHairColor = dwHairColor;
// 	tCharacter.dwHeadMesh = nHeadMesh;
// 	tCharacter.dwSex = nSex;

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return;
	}

// 	if( FALSE == qry->Exec( szQuery ) )
// 	{
// 		WriteLog( "%s, %d\r\n\t%s", __FILE__, __LINE__, szQuery );
// 		return;
// 	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	if( qry->Fetch() )
	{
		int nError;
		u_long nidPlayer;
		nError = qry->GetInt( "fError" );
		if( nError == 1 )	// 캐릭터 생성 ok
		{
			nidPlayer = qry->GetInt( "m_idPlayer" );	

			PlayerData pd;
//			pd.nJob	= 0;
//			pd.dwState	= 0;
			pd.data.nLevel	= 1;
			pd.data.nSex	= nSex;
			pd.data.nVer	= 1;
			FLStrcpy( pd.szPlayer, _countof( pd.szPlayer ), lpDbOverlappedPlus->AccountInfo.szPlayer );
			PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->AddPlayerData( nidPlayer, pd );
			if( pPlayerData == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "AddPlayerData Failed. PlayerID:[%07d]" ), nidPlayer );
				return;
			}

			m_AddRemoveLock.Enter();
			m_AccountCacheMgr.AddMover( nidPlayer, lpDbOverlappedPlus->AccountInfo.szAccount, nSlot );
			m_AddRemoveLock.Leave();

			CDPTrans::GetInstance()->SendAddPlayerData( nidPlayer, pPlayerData );

			g_dpLoginSrvr.SendPlayerList( lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, lpDbOverlappedPlus->dpid, dwAuthKey );

			char szQuery[MAX_QUERY_SIZE]	= { 0,};
			FLSPrintf( szQuery, _countof( szQuery ), "BANK_STR 'U', '%07d', '%02d', '%04d'", nidPlayer, g_appInfo.dwSys, nBankPW );
			
			if( FALSE == qry->Exec( szQuery ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
				return;
			}
		}
		else if( nError == 0 )	// 캐릭터 이름 중복
		{
			g_dpLoginSrvr.SendError( ERROR_USER_EXISTS, lpDbOverlappedPlus->dpid );
		}
		else if( nError == 2 )	// 캐릭터 생성 수 초과
		{
			g_dpLoginSrvr.SendError( ERROR_USER_OVERFLOW, lpDbOverlappedPlus->dpid );
		}
	}
}

// 캐릭터 삭제 
void CDbManager::RemovePlayer( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long idPlayer;
	char szNo[255] = {0,};
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, _countof( lpDbOverlappedPlus->AccountInfo.szAccount ) );
	arRead.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, _countof( lpDbOverlappedPlus->AccountInfo.szPassword ) );
	arRead.ReadString( szNo, _countof( szNo ) );

	arRead >> idPlayer;
	DWORD dwAuthKey;
	arRead >> dwAuthKey;

	size_t nSize;	u_long uMessengerId;
	std::vector<u_long> vecMessenger;
	arRead >> nSize;
	vecMessenger.reserve( nSize );
	for( size_t i=0; i<nSize; i++ )
	{
		arRead >> uMessengerId;
		vecMessenger.push_back( uMessengerId );
	}

	// BEGIN110520
	arRead >> lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage;
	// END110520

	arRead >> lpDbOverlappedPlus->dpid;

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	CMover Mover;
	Mover.m_idPlayer	= idPlayer;
	FLStrcpy( Mover.m_szAccount, _countof( Mover.m_szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
	FLStrncpy( Mover.m_szName, _countof( Mover.m_szName ), szNo, _countof( Mover.m_szName ) - 1 );
	
	T_CHARACTER_STR_INFO tCharacter( Mover, "D1" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "D1" );
// 	tCharacter.dwPlayerId = idPlayer;
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
// 	FLStrcpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), szNo );

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return;
	}

// 	if( FALSE == qry->Exec( szQuery ) )
// 	{
// 		WriteLog( "%s, %d\t%s\r\n\t%s, %s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword );
// 		return;
// 	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	if( qry->Fetch() )
	{
		int nError = 0;
		nError = qry->GetInt( "fError" );
		switch( nError )
		{
			case 1:	// 주민번호틀림
				g_dpLoginSrvr.SendError( ERROR_NO_SUCH_GROUP, lpDbOverlappedPlus->dpid );
				return;
			case 2:	// 계정없음
				return;
			case 3:	// 길드전 참가
				{
					g_dpLoginSrvr.SendError( ERROR_WARTIME, lpDbOverlappedPlus->dpid );
					return;
				}
			case 4:	// 길드 소속
				{					
					u_long idGuild	= qry->GetInt( "fText" );
					g_dpCoreSrvr.SendDelPlayer( idPlayer, idGuild );
					break;
				}
		}
	}
	else
	{
		return;
	}

	CCampus* pCampus = CCampusHelper::GetInstance()->GetCampus( CCampusHelper::GetInstance()->GetCampusIdByPlayerId( idPlayer ) );
	if( pCampus && pCampus->IsMember( idPlayer ) )
	{
		CAr ar;
		ar << pCampus->GetCampusId() << idPlayer;
		u_long nBufSize;
		LPBYTE lpBuf = ar.GetBuffer( &nBufSize );
		CCampusHelper::GetInstance()->PostRequest( CAMPUS_REMOVE_MEMBER, lpBuf, nBufSize );
	}

	CPlayerDataCenter::GetInstance()->DeletePlayerData( idPlayer );

	m_AddRemoveLock.Enter();
	m_AccountCacheMgr.RemoveMover( idPlayer, lpDbOverlappedPlus->AccountInfo.szAccount );
	m_AddRemoveLock.Leave();

	CDPTrans::GetInstance()->SendDeletePlayerData( idPlayer );

	for( std::vector<u_long>::iterator it=vecMessenger.begin(); it!=vecMessenger.end(); it++ )
	{
		qry->Execute( "uspDeleteMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, (*it) );
		g_dpCoreSrvr.SendRemovePlayerFriend( idPlayer, (*it) );
	}

	// BEGIN110520
	// 캐릭터 삭제 후 디폴트 페이지로 전송.. 
	g_dpLoginSrvr.SendPlayerList( lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, lpDbOverlappedPlus->dpid, dwAuthKey );
	// END110520
}


void CDbManager::ChangeBankPass( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	char szName[33]		= { 0, };
	char szNewPass[5]	= { 0, };
	u_long uidPlayer	= NULL_PLAYER_ID;
	arRead.ReadString( szName, _countof( szName ) );
	arRead.ReadString( szNewPass, _countof( szNewPass ) );
	arRead >> uidPlayer;

	//BANK_STR 'U','000001','01','1234'

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 4;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "BANK_STR", MAX_PARAMETER );

	TCHAR szGu[ 2 ] = { 0, };
	TCHAR szPlayerId[ 8 ] = { 0, };
	TCHAR szServerIndex[ 3 ] = { 0, };
	FLStrcpy( szGu, _countof( szGu ), "U" );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", uidPlayer );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	qry->BindParameterChar( SQL_PARAM_INPUT, _countof( szGu ) - 1, szGu );
	qry->BindParameterChar( SQL_PARAM_INPUT, _countof( szPlayerId ) - 1, szPlayerId );
	qry->BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );
	qry->BindParameterChar( SQL_PARAM_INPUT, _countof( szNewPass ) - 1, szNewPass );

// 	FLSPrintf( szQuery, _countof( szQuery ),
// 		"BANK_STR 'U', '%07d', '%02d', '%s'", uidPlayer, g_appInfo.dwSys, szNewPass );

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s, %s, %s" )
			, szQuery, szGu, szServerIndex, szPlayerId, szNewPass );
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}

// 플레이어 리스트를 보냄(한계정에 있는 모든 플레이어)
void CDbManager::SendPlayerList( CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	BEFORESENDSOLE( ar, PACKETTYPE_PLAYER_LIST, lpDbOverlappedPlus->dpid );
	ar << lpDbOverlappedPlus->dwAuthKey;	

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	CMover Mover;
	FLStrcpy( Mover.m_szAccount, _countof( Mover.m_szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
	FLStrncpy( Mover.m_szName, _countof( Mover.m_szName ), lpDbOverlappedPlus->AccountInfo.szPassword, _countof( Mover.m_szName ) - 1 );

	T_CHARACTER_STR_INFO tCharacter( Mover, "S2" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S2" );
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
// 	FLStrncpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), lpDbOverlappedPlus->AccountInfo.szPassword, 32 );

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return;
	}

// 	if( FALSE == qry->Exec( szQuery ) )
// 	{
// 		WriteLog( "%s, %d\t%s\r\n\t%s, %s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword );
// 		return;
// 	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
	
	int countPlayer		= 0;
	int countMessenger	= 0;

	u_long	uidPlayer[MAX_CHARACTER_SLOT]			= { 0, };
	BYTE	arrDBDataSlot[MAX_CHARACTER_SLOT]		= { 0, };
	BYTE	MessengerSlot[MAX_CHARACTER_SLOT]		= { 0, };
	DWORD	dwMessengerState[MAX_CHARACTER_SLOT]	= { 0, };
	

	ptrdiff_t nPlayerCountTag = ar.ReserveSpace( sizeof( countPlayer ) );

//	DWORD	adwWorldId[MAX_CHARACTER_SLOT]	= { 0, };
	D3DXVECTOR3	avPos[MAX_CHARACTER_SLOT];

	// BEGIN110520
// 	ACCOUNT_CACHE* pAccount		= NULL;
// 	g_DbManager.m_AddRemoveLock.Enter();
// 	{
// 		pAccount = m_AccountCacheMgr.Find( lpDbOverlappedPlus->AccountInfo.szAccount );
// 		if( pAccount )
// 		{
// 			for( int i = 0; i < MAX_CHARACTER_SLOT; i++ )
// 			{
// 				if( pAccount->pMover[i] )
// 				{
// 					adwWorldId[i] = pAccount->pMover[i]->m_dwWorldID;
// 					avPos[i]      = pAccount->pMover[i]->m_vPos;
// 				}
// 			}
// 		}
// 	}
// 	g_DbManager.m_AddRemoveLock.Leave();
	// END110520
	
	//	mulcom	BEGIN100218	S2 Patch
	int	nWhileCount	= 0;
	//	mulcom	END100218	S2 Patch

	// BEGIN110520
	BYTE byTotalCharNum		= 0;		// 총 캐릭터 수
	BYTE byLastPlayerSlot	= 0;		// 마지막 캐릭터의 슬롯 넘버..
	// END110520

	while( qry->Fetch() )
	{
		// BEGIN110520
		++byTotalCharNum;

		const BYTE byDBDataSlot			= static_cast<BYTE>( qry->GetInt("playerslot") );
		const BYTE bySelectPage			= GET_PLAYER_PAGE( byDBDataSlot );

		byLastPlayerSlot				= byDBDataSlot;

		if( bySelectPage != lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage )
		{
			continue;
		}
		
		const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( byDBDataSlot );
		// END110520

		//	mulcom	BEGIN100218	S2 Patch
		nWhileCount++;

		if( nWhileCount > MAX_CHARACTER_SLOT )
		{
			continue;
		}

		if( countPlayer >= MAX_CHARACTER_SLOT )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s : countPlayer : [%d]" ), Mover.m_szAccount, countPlayer );

			break;
		}
		//	mulcom	END100218	S2 Patch


		int nError = 0;
		nError = qry->GetInt( "fError" );
		if( nError == 1 || nError == 2 )	// 1 : 암호틀림 2 : 계정없음
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s, %s, %d" ), Mover.m_szAccount, Mover.m_szName, nError );
//			qry->Clear();
			return;
		}

//		int islot = qry->GetInt("playerslot");

		//	mulcom	BEGIN100218	S2 Patch

		if( byProgramDataSlot < 0 || byProgramDataSlot >= MAX_CHARACTER_SLOT )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s SlotError nProgramDataSlot [%d]" ), Mover.m_szAccount, byProgramDataSlot );
			
			continue;
		}

		for( int nSlotCount = 0; nSlotCount < countPlayer; nSlotCount++ )
		{
			if( arrDBDataSlot[nSlotCount] == byDBDataSlot )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "DBDataSlot Duplication. Account:[%s], Slot:[%d]" )
					, Mover.m_szAccount, byDBDataSlot );
				
				continue;
			}
		}
		//	mulcom	END100218	S2 Patch

		arrDBDataSlot[countPlayer] = byDBDataSlot;


		CMover mover;
		mover.InitProp();

		int nBlock = 0;

		// BEGIN110520
		mover.m_dwWorldID	= qry->GetInt( "dwWorldID" );
		mover.m_vPos.x		= qry->GetFloat( "m_vPos_x");
		mover.m_vPos.y		= qry->GetFloat( "m_vPos_y");
		mover.m_vPos.z		= qry->GetFloat( "m_vPos_z");

// 		BOOL bRefresh	= qry->GetInt( "m_idCompany" ) > 0;
// 
// 		if( adwWorldId[nProgramDataSlot] > 0 && bRefresh == FALSE )
// 		{
// 			dwWorldID		= adwWorldId[nProgramDataSlot];
// 			mover.m_vPos	= avPos[nProgramDataSlot];
// 		}
// 		else
// 		{
// 			dwWorldID		= qry->GetInt( "dwWorldID" );
// 			mover.m_vPos.x	= qry->GetFloat( "m_vPos_x");
// 			mover.m_vPos.y	= qry->GetFloat( "m_vPos_y");
// 			mover.m_vPos.z	= qry->GetFloat( "m_vPos_z");
// 		}
		// END110520

		mover.m_dwMode	= 0;
		if( qry->GetInt( "last_connect" ) == 3 )		// ???
		{
			mover.m_dwMode	= EVENT_OLDBOY_MODE;
		}

		countPlayer++;

		qry->GetStr( "m_szName", mover.m_szName, _countof( mover.m_szName ) );

		char strEnd_Time[13] = {0,};
		qry->GetStr("BlockTime", strEnd_Time, _countof( strEnd_Time ) );
		CTime tEnd_Time;

		GetStrTime(&tEnd_Time, strEnd_Time);

		CTime NowTime = CTime::GetCurrentTime();
		if( NowTime > tEnd_Time )
		{
			// 접속이 가능한 상태
			nBlock = 1;
		}

		{
			mover.m_dwIndex		= (DWORD)qry->GetInt("m_dwIndex");
			mover.m_idPlayer	= (DWORD)qry->GetInt( "m_idPlayer" );
			mover.m_idparty		= (DWORD)qry->GetInt("m_idparty");
			mover.m_idGuild		= (DWORD)qry->GetInt( "m_idGuild" );
			mover.m_idWar		= (DWORD)qry->GetInt( "m_idWar" );

			mover.m_dwSkinSet	= (DWORD)qry->GetInt("m_dwSkinSet");
			mover.m_dwHairMesh	= (DWORD)qry->GetInt("m_dwHairMesh");
			mover.m_dwHeadMesh	= (DWORD)qry->GetInt("m_dwHeadMesh");
			mover.m_dwHairColor = (DWORD)qry->GetInt("m_dwHairColor");
			mover.SetSex( (BYTE) qry->GetInt("m_dwSex") );
			mover.m_nJob		= (LONG)qry->GetInt("m_nJob");
			mover.m_nLevel		= (LONG)qry->GetInt("m_nLevel");
			mover.m_Stat.SetOriginStr( qry->GetInt( "m_nStr" ) );
			mover.m_Stat.SetOriginSta( qry->GetInt( "m_nSta" ) );
			mover.m_Stat.SetOriginDex( qry->GetInt( "m_nDex" ) );
			mover.m_Stat.SetOriginInt( qry->GetInt( "m_nInt" ) );
			mover.m_Stat.SetDiceStr( qry->GetInt( "dcStr" ) );
			mover.m_Stat.SetDiceSta( qry->GetInt( "dcSta" ) );
			mover.m_Stat.SetDiceDex( qry->GetInt( "dcDex" ) );
			mover.m_Stat.SetDiceInt( qry->GetInt( "dcInt" ) );

			int CountStr	= 0;
			int IndexItem		= 0;
			char Inven[8024]	= { 0, };
			qry->GetStr( "m_Inventory", Inven, _countof( Inven ) );
			VERIFYSTRING( Inven, mover.m_szName );
			while( '$' != Inven[CountStr] )
			{
				FLItemElem BufItemElem;
				IndexItem = GetOneItem( &BufItemElem, Inven, &CountStr );
				if( IndexItem == -1 )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Inventory : << 프로퍼티 없음. %s, %d" ), mover.m_szName, BufItemElem.m_dwItemId );
				}
				else
				{
					if( IndexItem >= MAX_INVENTORY + MAX_HUMAN_PARTS )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "Inventory : << IndexItem %s, %d" ), mover.m_szName, IndexItem );
						FLERROR_LOG( PROGRAM_NAME, _T( "Inventory = %s" ), Inven );
						return;
					}
					mover.m_Inventory.m_apItem[IndexItem] = BufItemElem;
				}
			}

			mover.m_Inventory.SetAllocedSerialNumber( GetSerialNumberStatus( Inven, &CountStr ) );


			CountStr	= 0;
			int iIndex	= 0;
			char sIndex[512]	= { 0, };
			qry->GetStr( "m_apIndex", sIndex, _countof( sIndex ) );
			VERIFYSTRING( sIndex, mover.m_szName );
			while( '$' != sIndex[CountStr] )
			{
				mover.m_Inventory.m_apIndex[iIndex]	= (DWORD)GetIntFromStr( sIndex, &CountStr );
				if( mover.m_Inventory.m_apIndex[iIndex] != NULL_ID )
				{
					mover.m_Inventory.m_apItem[ mover.m_Inventory.m_apIndex[iIndex] ].m_dwObjIndex = iIndex;
				}
				iIndex++;
			}

			CountStr	= 0;
			int IndexEquipment	= 0;
			char Equipment[500]	= {0,};
			qry->GetStr( "m_adwEquipment", Equipment, _countof( Equipment ) );
			VERIFYSTRING( Equipment, mover.m_szName );
			while( '$' != Equipment[CountStr] )
			{
				if(IndexEquipment < MAX_HUMAN_PARTS)
					mover.m_aEquipInfo[IndexEquipment].dwItemID		= (DWORD)GetIntFromStr(Equipment, &CountStr);
				IndexEquipment++;
			}

			mover.m_Inventory.ConvertInventoryIndex( MAX_INVENTORY_OLD, MAX_HUMAN_PARTS_OLD );
			mover.m_Inventory.RefreshIndex();

			CAr arbuf;

			arbuf << byProgramDataSlot;
			arbuf << nBlock;
			arbuf << mover.m_dwWorldID;
			arbuf << mover.m_dwIndex;
			arbuf.WriteString( mover.m_szName );
			arbuf << mover.m_vPos;
			arbuf << mover.m_idPlayer;
			arbuf << mover.m_idparty;
			arbuf << mover.m_idGuild;
			arbuf << mover.m_idWar;
			arbuf << mover.m_dwSkinSet;
			arbuf << mover.m_dwHairMesh;
			arbuf << mover.m_dwHairColor;
			arbuf << mover.m_dwHeadMesh;
			arbuf << mover.GetSex();
			arbuf << mover.m_nJob;
			arbuf << mover.m_nLevel;
			arbuf << static_cast<DWORD>( 0 );
			mover.m_Stat.Serialize( arbuf );
			arbuf << mover.m_dwMode;

			MessengerSlot[countMessenger] = byProgramDataSlot;
			uidPlayer[countMessenger] = mover.m_idPlayer;
			dwMessengerState[countMessenger] = qry->GetInt( "m_nMessengerState" );
			countMessenger++;

			int countbuf = 0;
			ptrdiff_t nCountTag = arbuf.ReserveSpace( sizeof( countbuf ) );
			FLItemElem* pItemElem;
			for( DWORD i = 0; i < MAX_HUMAN_PARTS; i++ )
			{
				pItemElem	= mover.m_Inventory.GetEquip( i );
				if( pItemElem && !pItemElem->IsFlag( FLItemElem::expired ) )
				{
					PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( pItemElem->m_dwItemId );
					if( pItemProp )
					{
						if( pItemProp->dwParts == i || pItemProp->dwPartsub == i ||
							( pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON ) )
						{
							arbuf << pItemElem->m_dwItemId;
							countbuf++;
						}
					}
				}
			}
			arbuf.WriteReservedSpace( nCountTag, &countbuf, sizeof( countbuf ) );
			ar.Write( arbuf );
		} 
	} // while	
	ar.WriteReservedSpace( nPlayerCountTag, &countPlayer, sizeof( countPlayer ) );


	ar << countMessenger;
	for( int i = 0 ; i < countMessenger ; i++ )
	{
		CMover cMoverBuf;
		cMoverBuf.InitProp();
		cMoverBuf.m_idPlayer = uidPlayer[i];
		LoadMessenger( &cMoverBuf, qry );
		cMoverBuf.m_RTMessenger.SetState( dwMessengerState[i] );
		ar << MessengerSlot[i];
		cMoverBuf.m_RTMessenger.Serialize( ar );

		std::vector< EQUIP_INFO >	vecEquipInfo;
		LoadEquipInfo( qry, uidPlayer[i], MAX_HUMAN_PARTS, vecEquipInfo );
		ar << vecEquipInfo.size();
		for( size_t j = 0; j < vecEquipInfo.size(); ++j )
		{
			ar << vecEquipInfo.at( j );
		}
	}

	// BEGIN110520
	ar << byTotalCharNum;			// 총 보유 캐릭터 수

	const BYTE byTotalCharSelectPage = GET_PLAYER_PAGE( byLastPlayerSlot );
	ar << byTotalCharSelectPage;	// 총 캐릭터 선택 페이지

	ar << lpDbOverlappedPlus->AccountInfo.byPlayerSelectPage;	// 현재 선택한 페이지
	// END110520
#ifdef PASSWORD_RESET_2ND

	int nResetPassword = 0;

	for ( int i = 0; i < byTotalCharNum; i++ )
	{
		if ( i >= MAX_CHARACTER_SLOT ) break;
		nResetPassword = CheckResetPassword ( qry, uidPlayer[i] );
		ar << nResetPassword;
	}

#endif // PASSWORD_RESET_2ND
	SEND( ar, &g_dpLoginSrvr, DPID_ALLPLAYERS );
}

BOOL CDbManager::OpenGuildCombat( void )
{
	m_nGuildCombatIndex	= 0;
	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	m_uBestPlayer = 0;
	m_vecGuildCombat.clear();
	
	CQuery qry;
#ifdef __DbStuff
	if (!qry.Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
		AfxMessageBox( "Error : Not Connect Character DB" );
		return FALSE;
	}
	
	char szQuery[40960] = { 0, };
	// 인덱스 얻어오기
	FLSPrintf( szQuery, _countof( szQuery ), "uspLoadMaxCombatID '%02d'", g_appInfo.dwSys );
	if( FALSE == qry.Exec( szQuery ) )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
	
	if( qry.Fetch() )
	{
		m_nGuildCombatIndex = qry.GetInt( "MaxNum" );
		
		// 가지고온 컴벳 인덱스가 종료한 인덱스 인지 확인
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoadCombatInfo %d, '%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
		
		if( qry.Fetch() )
		{
			int nClose = qry.GetInt( "Status" );
			
			if( nClose == 30 || nClose == -100 ) // 길드대전 종료
			{
				++m_nGuildCombatIndex;
				FLSPrintf( szQuery, _countof( szQuery ), "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
				if( FALSE == qry.Exec( szQuery ) )
				{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
			}
		}
		else
		{
			FLSPrintf( szQuery, _countof( szQuery ), "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
			if( FALSE == qry.Exec( szQuery ) )
			{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
		}
		
		// 최종 승리길드 및 연승 얻어오기
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoadWinnerGuildInfo %d,'%02d'", m_nGuildCombatIndex-1, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );	return FALSE; }
		if( qry.Fetch() )
		{
			m_uWinGuildId = qry.GetInt( "GuildID" );
			m_nWinGuildCount = qry.GetInt( "StraightWin" );
		}

		// 최종 베스트 플레이어 얻어오기
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoadCombatBestPlayer %d,'%02d'", m_nGuildCombatIndex-1, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );	return FALSE; }
		if( qry.Fetch() )
		{
			m_uBestPlayer = qry.GetInt( "PlayerID" );
		}
		
		// 참가길드 얻어오기
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoadCombatGuildList %d, '%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
		
		while( qry.Fetch() )
		{
			u_long GuildId	= (u_long)( qry.GetInt( "GuildID" ) );
			DWORD dwPenya	= (DWORD)( qry.GetInt64( "CombatFee" ) );
			int nStatus		= qry.GetInt( "Status" );

			if( nStatus == 10 || nStatus == 12 || nStatus == 20 )
			{
				__GUILDCOMBATJOIN GCJoin;
				GCJoin.uGuildId = GuildId;
				GCJoin.dwPenya = dwPenya;
				GCJoin.bRequest = TRUE;
				m_vecGuildCombat.push_back( GCJoin);
			}
		}
		
		// 지금 까지 보상 및 참여금을 안받은 길드 및 베스트 플레이어 얻어오기
		m_GCResultValueGuild.clear();
		m_GCResultValuePlayer.clear();
		FLSPrintf( szQuery, _countof( szQuery ), "uspLoadCombatUnpaidList '%02d'", g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
		while( qry.Fetch() )
		{
			char cFlag;
			cFlag = qry.GetChar( "Flag" );
			if( cFlag == 'G' )	// 길드
			{
				__GCRESULTVALUEGUILD ResultValueGuild;
				ResultValueGuild.nCombatID = qry.GetInt( "CombatID" );
				ResultValueGuild.uidGuild = qry.GetInt( "GuildID" );
				ResultValueGuild.nReturnCombatFee = qry.GetInt64( "ReturnCombatFee" );
				ResultValueGuild.nReward = qry.GetInt64( "Reward" );
				m_GCResultValueGuild.push_back( ResultValueGuild );
			}
			else				// 플레이어
			{
				__GCRESULTVALUEPLAYER ResultValuePlayer;
				ResultValuePlayer.nCombatID = qry.GetInt( "CombatID" );
				ResultValuePlayer.uidGuild = qry.GetInt( "GuildID" );
				ResultValuePlayer.uidPlayer = qry.GetInt( "PlayerID" );
				ResultValuePlayer.nReward = qry.GetInt64( "Reward" );
				m_GCResultValuePlayer.push_back( ResultValuePlayer );
			}
		}

		m_vecGCPlayerPoint.clear();
		FLSPrintf( szQuery, _countof( szQuery ), "uspRankGuildCombatPlayer '%02d'", g_appInfo.dwSys );
		if( FALSE == qry.Exec( szQuery ) )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
		while( qry.Fetch() )
		{
			__GCPLAYERPOINT GCPlayerPoint;
			GCPlayerPoint.uidPlayer = qry.GetInt( "PlayerID" );
			GCPlayerPoint.nJob = qry.GetInt( "Job" );
			GCPlayerPoint.nPoint = qry.GetInt( "PointSummary" );
			m_vecGCPlayerPoint.push_back( GCPlayerPoint );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
	}
	return TRUE;
}

BOOL	CDbManager::LoadPlayerData( void )
{
	CQuery	query;
#ifdef __DbStuff
	if (!query.Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( query.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LoadPlayerData: can't connect character db" ) );
		AfxMessageBox( "can't connect character db" );
	}
	char szQuery[100]	= { 0,};

	FLSPrintf( szQuery, _countof( szQuery ), "uspLoadPlayerData '%02d'", g_appInfo.dwSys );
	if( !query.Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "query: %s" ), szQuery );
		return FALSE;
	}

	PlayerData	pd;
	u_long idPlayer		= 0;
	while( query.Fetch() )
	{
		idPlayer	= query.GetInt( "m_idPlayer" );
		query.GetStr( "m_szName", pd.szPlayer, _countof( pd.szPlayer ) );
		pd.data.nSex	= static_cast< BYTE >( query.GetInt( "m_dwSex" ) );
		pd.data.nJob	= static_cast< BYTE >( query.GetInt( "m_nJob" ) );
		pd.data.nLevel	= static_cast< BYTE >( query.GetInt( "m_nLevel" ) );
//		pd.data.dwState	= (DWORD)query.GetInt( "m_nMessengerState" );
		pd.data.nVer		= 1;	//
		pd.data.uLogin	= 0;
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->AddPlayerData( idPlayer, pd );
		FLASSERT( pPlayerData );
	}
	m_idPlayer	= idPlayer;
	return TRUE;
}


BOOL CDbManager::GetPartyName( void )
{
	CQuery qry;
	
#ifdef __DbStuff
	if (!qry.Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
		return FALSE;
	}
	
	char szQuery[256], szPartyName[36];
	u_long id	=0;

	
	FLSPrintf( szQuery, _countof( szQuery ),
		"PARTY_STR 'S2','','%02d'",
		g_appInfo.dwSys );

	if( FALSE == qry.Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return FALSE;
	}
	
	while( qry.Fetch() )
	{
		qry.GetStr( "partyname", szPartyName, _countof( szPartyName ) );
		id	= qry.GetInt( "m_idPlayer" );
		m_2PartyNamePtr.insert( std::map<u_long, std::string>::value_type( id, szPartyName ) );
	}
	m_idPlayer	= id;
	return TRUE;	
}

BOOL CDbManager::GetMyPartyName( CQuery* qry, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/, u_long uidPlayer, char * szPartyName, size_t cchPartyName )
{
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"PARTY_STR 'S1','%07d','%02d'",
		uidPlayer, g_appInfo.dwSys );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return FALSE;
	}
	
	if( qry->Fetch() )
	{
		qry->GetStr( "partyname", szPartyName, cchPartyName );
	}
	return TRUE;
}

//
//SP : uspLoggingLogin
//
//Parameter :
//          @pPlayerID      char(6),
//          @pserverindex   char(2),
//          @pAccount       varchar(32),
//          @pWorldID       int,
//          @pGold          bigint,
//          @pLevel         int,  -- 캐릭터 레벨
//          @pJob           int, -- 캐릭터 직업
//          @pStartDt       char(14),
//          @pTotalPlayTime int,
//          @pRemoteIP      varchar(16)
//Return : N/N


void CDbManager::LogPlayConnect( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int		iServer_No;				// 서버 번호
	DWORD	iMap_No;				// 맵 번호
	char	cIP[16] = { 0, };		// 접속한 넘의 아이피
	DWORD	dConnectTime;
	DWORD	dSeed;					// 돈
	SYSTEMTIME	siIn;
	u_long	uidPlayer;
	char szAccount[MAX_ACCOUNT];
	int nLevel;
	int nJob;

	iServer_No	= g_appInfo.dwSys;
	arRead >> iMap_No;
	arRead.Read( &siIn, sizeof(SYSTEMTIME) );
	arRead.ReadString( cIP, _countof( cIP ) );
	arRead >> dSeed;
	arRead >> uidPlayer;
	arRead.ReadString( szAccount, _countof( szAccount ) );
	arRead >> nLevel;
	arRead >> nJob;
	DWORD dwPCBangClass;
	arRead >> dwPCBangClass;

	if( g_pCommonCheck->CheckTime( siIn ) == false )
	{
		GetLocalTime( &siIn );
	}

	CTime tIn( siIn ), tOut	= CTime::GetCurrentTime();
	CTimeSpan tConnect	= tOut - tIn;
	
	CString strOut = tOut.Format( "%Y%m%d%H%M%S" );

	CString strIn	= tIn.Format( "%Y%m%d%H%M%S" );
	dConnectTime	= (DWORD)tConnect.GetTotalSeconds();
	char timebuf[15] = {0,};
	FLStrncpy( timebuf, _countof( timebuf ), (LPCTSTR)strIn, 14 );
	
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
			"uspLoggingLogin '%07d', '%02d', '%s', %d, %d, %d, %d, '%s', %d, '%s'"
			", %d"
			, uidPlayer, g_appInfo.dwSys, szAccount, iMap_No, dSeed, nLevel, nJob, timebuf, dConnectTime, cIP
			,dwPCBangClass
			 );

	if ( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%d, %s" ), uidPlayer, szQuery );
		return;
	}
}


void CDbManager::AddPartyName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long uidPlayer;
	TCHAR	sParty[33];		
	arRead >> uidPlayer;
	arRead.ReadString( sParty, _countof( sParty ) );

	CString strPartyName	= sParty;
	strPartyName.Replace( '\'', ' ' );
	TCHAR szPartyName[33] = { 0, };
	FLStrcpy( szPartyName, _countof( szPartyName ), strPartyName );

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"PARTY_STR 'A1','%07d','%02d','%s'",
		uidPlayer, g_appInfo.dwSys, szPartyName );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}
	
	ULONG2STRING::iterator iter = m_2PartyNamePtr.find( uidPlayer );
	if( iter != m_2PartyNamePtr.end() )
		m_2PartyNamePtr.erase( iter );

	m_2PartyNamePtr.insert( std::map<u_long, std::string>::value_type( uidPlayer, sParty ) );
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

BOOL CDbManager::SendItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CMover Mover;
	Mover.m_idPlayer		= pMover->m_idPlayer;
	FLStrcpy( Mover.m_szAccount, _countof( Mover.m_szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
	FLStrncpy( Mover.m_szName, _countof( Mover.m_szName ), lpDbOverlappedPlus->AccountInfo.szPassword, _countof( Mover.m_szName ) - 1 );

	T_CHARACTER_STR_INFO tCharacter( Mover, "S4" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S4" );
// 	tCharacter.dwPlayerId = pMover->m_idPlayer;
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), lpDbOverlappedPlus->AccountInfo.szAccount );
// 	FLStrncpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), lpDbOverlappedPlus->AccountInfo.szPassword, 32 );

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	std::vector<__SendItemContents> vecSendItem;

	while( qry->Fetch() )
	{
		__SendItemContents kSendItem;

		if( GetSendItem( qry, &kSendItem ) == TRUE )
		{
			vecSendItem.push_back( kSendItem );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::SendItemtoCharacter. PlayerName = %s, Item_Name = %s" )
				, pMover->m_szName, kSendItem.OneItem );
		}
	}

	while( vecSendItem.empty() == false )
	{
		__SendItemContents kSendItem = vecSendItem.back();

		FLItemElem* pItemElem = &kSendItem.itemElem;

		if( 0 == strcmp( kSendItem.OneItem, "penya" ) )				// 페냐인지 확인
		{
			// Penya 지급
			if( SendPenya( qry1, pMover, kSendItem.Item_Count, kSendItem.nNo ) == TRUE )
			{
				bResult		= TRUE;

				LogItemInfo aLogItem;
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->m_szName );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "DB" );

				//////////////////////////////////////////////////////////////////////////
				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
				aLogItem.idSendPlayer = pMover->m_idPlayer;
				aLogItem.idRecvPlayer = 0;
				//////////////////////////////////////////////////////////////////////////

				aLogItem.Gold = pMover->GetGold() - kSendItem.Item_Count;
				aLogItem.Gold2 = pMover->GetGold();

				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
				aLogItem.kLogItem.nQuantity = kSendItem.Item_Count;
				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "U" );

#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
				UNREFERENCED_PARAMETER( qrylog );
				LOG_DB().PostQueryCommand( pMover->m_szAccount[ 0 ], new FLItem_LogQuery( "LC", aLogItem, pMover->m_idPlayer, kSendItem.nNo, kSendItem.OneItem ) );
#else
				ItemLogQuery( qrylog, "LC", aLogItem, pMover->m_idPlayer, kSendItem.nNo, kSendItem.OneItem );
#endif 
			}
		}
		else	// 아이템 지급
		{
			SERIALNUMBER iSerialNumber = SendItem( qry1, pMover, kSendItem );
			if( iSerialNumber != 0 )
			{
				bResult		= TRUE;
				LogItemInfo aLogItem;
				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->m_szName );
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "DB" );

				//////////////////////////////////////////////////////////////////////////
				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
				aLogItem.idSendPlayer = pMover->m_idPlayer;
				aLogItem.idRecvPlayer = 0;
				//////////////////////////////////////////////////////////////////////////

				aLogItem.Gold = pMover->GetGold();
				aLogItem.Gold2 = pMover->GetGold();

				aLogItem.kLogItem.CopyItemInfo( *pItemElem );
				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", kSendItem.itemElem.m_dwItemId );
				aLogItem.kLogItem.nQuantity = kSendItem.Item_Count;
				aLogItem.kLogItem.dwSerialNumber = iSerialNumber;


// 				FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.szItemName ), "%d", kSendItem.itemElem.m_dwItemId );
// 				aLogItem.kLogItem.nQuantity = kSendItem.Item_Count;
// 				aLogItem.kLogItem.dwSerialNumber = iSerialNumber;
// 
// 				aLogItem.kLogItem.nAbilityOption = pItemElem->GetAbilityOption();
// 				aLogItem.kLogItem.byItemResist = pItemElem->m_byItemResist;
// 				aLogItem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// BEGIN110614
// 				aLogItem.dwCouplePlayerId			= pItemElem->GetCoupleId();
// 				aLogItem.nLevelDown					= pItemElem->GetLevelDown();
// 				aLogItem.wRandomOptionOriginId		= pItemElem->GetRandomOptionOriginID();
// 				aLogItem.nRandomOptionExtensionSize	= pItemElem->GetRandomOptionExtensionSize();
// 				aLogItem.wRandomOptionExtensionFlag	= pItemElem->GetRandomOptionExtensionFlag();
// 
// 				for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_SIZE; ++Nth )
// 				{
// 					aLogItem.awDstID[Nth]	= pItemElem->GetRandomOptionExtensionDstID( Nth );
// 					aLogItem.ashAdjValue[Nth]	= pItemElem->GetRandomOptionExtensionAdjValue( Nth );
// 				}
				// END110614
				//////////////////////////////////////////////////////////////////////////

				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "U" );

#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
				UNREFERENCED_PARAMETER( qrylog );
				LOG_DB().PostQueryCommand( pMover->m_szAccount[ 0 ], new FLItem_LogQuery( "LC", aLogItem, pMover->m_idPlayer, kSendItem.nNo, kSendItem.OneItem ) );
#else
				ItemLogQuery( qrylog, "LC", aLogItem, pMover->m_idPlayer, kSendItem.nNo, kSendItem.OneItem );
#endif 
			}
		}

		vecSendItem.pop_back();
	}


	//////////////////////////////////////////////////////////////////////////
// 	BOOL bResult	= FALSE;
// 	while( qry->Fetch() )
// 	{
// 		__SendItemContents SendItemContents;
// 
// 		FLItemElem* pItemElem = &SendItemContents.itemElem;
// 		pItemElem->m_bCharged = TRUE;
// 
// 		if( GetSendItem( qry, &SendItemContents ) == FALSE )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::SendItemtoCharacter. PlayerName = %s, Item_Name = %s" )
// 				, pMover->m_szName, SendItemContents.OneItem );
// 			continue;
// 		}
// 
// 		if( 0 == strcmp( SendItemContents.OneItem, "penya" ) )				// 페냐인지 확인
// 		{
// 			// Penya 지급
// 			if( SendPenya( qry1, pMover, SendItemContents.Item_Count, SendItemContents.nNo ) == FALSE )
// 			{
// 				FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::SendItemtoCharacter. PlayerName = %s, Item_Count = %d" )
// 					, pMover->m_szName, SendItemContents.Item_Count );
// 				continue;
// 			}
// 
// 			bResult		= TRUE;
// 			
// 			LogItemInfo aLogItem;
// 			//aLogItem.SendName = pMover->m_szName;
// 			//aLogItem.RecvName = "DB";
// 			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->m_szName );
// 			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "DB" );
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 			aLogItem.idSendPlayer = pMover->m_idPlayer;
// 			aLogItem.idRecvPlayer = 0;
// 			//////////////////////////////////////////////////////////////////////////
// 
// 			aLogItem.Gold = pMover->GetGold() - SendItemContents.Item_Count;
// 			aLogItem.Gold2 = pMover->GetGold();
// 			//aLogItem.ItemName = "SEED";
// 			FLSPrintf( aLogItem.szItemName, _countof( aLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
// 			aLogItem.itemNumber = SendItemContents.Item_Count;
// 			//aLogItem.Action = "U";
// 			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "U" );
// 
// #ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
// 			UNREFERENCED_PARAMETER( qrylog );
// 			LOG_DB().PostQueryCommand( pMover->m_szAccount[ 0 ], new FLItem_LogQuery( "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem ) );
// #else
// 			if( ItemLogQuery( qrylog, "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem ) == FALSE )
// 			{
// 				continue;
// 			}
// #endif 
// 		}
// 		else	// 아이템 지급
// 		{
// 			SERIALNUMBER iSerialNumber = SendItem( qry1, pMover, SendItemContents );
// 			if( iSerialNumber != 0 )
// 			{
// 				bResult		= TRUE;
// 				LogItemInfo aLogItem;
// 				//aLogItem.SendName = pMover->m_szName;
// 				//aLogItem.RecvName = "DB";
// 				FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->m_szName );
// 				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "DB" );
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
// 				aLogItem.idSendPlayer = pMover->m_idPlayer;
// 				aLogItem.idRecvPlayer = 0;
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				aLogItem.Gold = pMover->GetGold();
// 				aLogItem.Gold2 = pMover->GetGold();
// 				//aLogItem.ItemName = SendItemContents.OneItem;
// 				FLSPrintf( aLogItem.szItemName, _countof( aLogItem.szItemName ), "%d", SendItemContents.itemElem.m_dwItemId );
// 				aLogItem.itemNumber = SendItemContents.Item_Count;
// 				aLogItem.ItemNo = iSerialNumber;
// 
// 				aLogItem.nAbilityOption = pItemElem->GetAbilityOption();
// 				aLogItem.nItemResist = pItemElem->m_bItemResist;
// 				aLogItem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
// 
// 				//////////////////////////////////////////////////////////////////////////
// 				// BEGIN110614
// 				aLogItem.dwCouplePlayerId			= pItemElem->GetCoupleId();
// 				aLogItem.nLevelDown					= pItemElem->GetLevelDown();
// 				aLogItem.wRandomOptionOriginId		= pItemElem->GetRandomOptionOriginID();
//                 aLogItem.nRandomOptionExtensionSize	= pItemElem->GetRandomOptionExtensionSize();
// 				aLogItem.wRandomOptionExtensionFlag	= pItemElem->GetRandomOptionExtensionFlag();
// 
// 				for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_SIZE; ++Nth )
// 				{
// 					aLogItem.awDstID[Nth]	= pItemElem->GetRandomOptionExtensionDstID( Nth );
// 					aLogItem.ashAdjValue[Nth]	= pItemElem->GetRandomOptionExtensionAdjValue( Nth );
// 				}
// 				// END110614
// 				//////////////////////////////////////////////////////////////////////////
// 
// 				//aLogItem.Action = "U";
// 				FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "U" );
// 
// #ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
// 				UNREFERENCED_PARAMETER( qrylog );
// 				LOG_DB().PostQueryCommand( pMover->m_szAccount[ 0 ], new FLItem_LogQuery( "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem ) );
// #else
// 				if( ItemLogQuery( qrylog, "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem ) == FALSE )
// 				{
// 					continue;
// 				}
// #endif 
// 			}
// 			else
// 			{
// 				continue;
// 			}
// 		}
// 	}
	//////////////////////////////////////////////////////////////////////////

	return bResult;
}

// END110119
//////////////////////////////////////////////////////////////////////////

BOOL CDbManager::GetSendItem( CQuery *pQry, __SendItemContents * pSendItemContents )
{
	pSendItemContents->nNo = pQry->GetInt( "m_nNo" );										// 유니크 넘버
	pQry->GetStr( "Item_Name", pSendItemContents->OneItem, _countof( pSendItemContents->OneItem ) );								// 이름
	
	pSendItemContents->Item_Count				= pQry->GetInt( "Item_count" );				// 갯수

	int nIndex = atoi( pSendItemContents->OneItem );
	PT_ITEM_SPEC pItemProp = NULL;
	if( nIndex > 0 )
	{
		pItemProp = g_xSpecManager->GetSpecItem( nIndex );	
	}
	else
	{
		pItemProp = g_xSpecManager->GetSpecItem( pSendItemContents->OneItem );
	}

	if( pItemProp == NULL )
	{
		if( strcmp( pSendItemContents->OneItem, "penya" ) == 0 )
		{
			return TRUE;
		}

		FLERROR_LOG( PROGRAM_NAME, _T( "Item Property is NULL. [%s]" ), pSendItemContents->OneItem );
		return FALSE;
	}

	FLItemElem* pItemElem = &pSendItemContents->itemElem;
	pItemElem->m_dwItemId				= pItemProp->dwID;
	pItemElem->m_nItemNum				= pSendItemContents->Item_Count > 0 ? pSendItemContents->Item_Count : 1;
	pItemElem->SetAbilityOption( pQry->GetInt( "m_nAbilityOption" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_nAbilityOption" ) );
	
	// mirchang_100514 TransformVisPet_Log
	pItemElem->m_bTranformVisPet		= static_cast<BOOL>( pQry->GetInt( "nIsTransformVisPet" ) );
	DWORD dwItemResist					= pQry->GetInt( "m_bItemResist" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_bItemResist" );

	if( pItemProp->dwItemKind3 == IK3_PET )
	{
		if( pItemElem->m_dwItemId == dwItemResist && pItemElem->m_bTranformVisPet == FALSE )
		{
			pItemElem->m_bTranformVisPet	= TRUE;
			pItemElem->m_byItemResist		= SAI79::NO_PROP;
		}
		else
		{
			pItemElem->m_byItemResist		= SAI79::NO_PROP;
		}
	}
	else
	{
		pItemElem->m_byItemResist		= static_cast<BYTE>( dwItemResist );
	}
	// mirchang_100514 TransformVisPet_Log

	pItemElem->m_nResistAbilityOption	= pQry->GetInt( "m_nResistAbilityOption" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "m_nResistAbilityOption" );
	pItemElem->m_bCharged				= pQry->GetInt( "m_bCharged" ) <= 0 ? TRUE : FALSE;
	pSendItemContents->m_dwKeepTime		= pQry->GetInt64( "m_dwKeepTime" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt64( "m_dwKeepTime" );
	
	//////////////////////////////////////////////////////////////////////////
	// mirchang_101215 기간제 아이템인데 기간셋팅이 안되어 있는 경우..
	if( pItemElem->IsPeriod() == TRUE && pSendItemContents->m_dwKeepTime == 0 )
	{
		PT_ITEM_SPEC pItemPropTmp = pItemElem->GetProp();
		if( pItemPropTmp != NULL )
		{
			CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pItemPropTmp->dwSkillTime, 0 );
			pSendItemContents->m_dwKeepTime	= (DWORD)( tm.GetTime() );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	pItemElem->SetGeneralPiercingSize( pQry->GetInt( "nPiercedSize" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "nPiercedSize" ) );
	for( size_t i=0; i<pItemElem->GetGeneralPiercingSize(); i++ )
	{
		char szItem[32] = {0,};
		FLSPrintf( szItem, _countof( szItem ), "adwItemId%d", i );
		pItemElem->SetGeneralPiercingItemID( i, pQry->GetInt( szItem ) <= CQuery::CQUERYNULL ? 0 : static_cast<DWORD>( pQry->GetInt( szItem ) ) );
	}
	pItemElem->SetUltimatePiercingSize( pQry->GetInt( "nUMPiercedSize" ) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt( "nUMPiercedSize" ) );
	for( size_t i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
	{
		char szItem[32] = {0,};
		FLSPrintf( szItem, _countof( szItem ), "adwUMItemId%d", i );
		pItemElem->SetUltimatePiercingItemID( i, pQry->GetInt( szItem ) <= CQuery::CQUERYNULL ? 0 : static_cast<DWORD>( pQry->GetInt( szItem ) ) );
	}

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110614
	if( pQry->IsValidColumn( "idCouplePlayer" ) == true )
	{
		const int nCouplePlayerId = pQry->GetInt( "idCouplePlayer" );
		if( nCouplePlayerId > 0 )
		{
			pItemElem->SetCoupleId( static_cast<DWORD>( nCouplePlayerId ) );
		}
	}

	if( pQry->IsValidColumn( "nLevelDown" ) == true )
	{
		const int nLevelDown = pQry->GetInt( "nLevelDown" );
		if( nLevelDown > 0 )
		{
			pItemElem->SetLevelDown( nLevelDown );
		}
	}

	if( pQry->IsValidColumn( "nRandomOptionOriginId" ) == true )
	{
		const WORD wRandomOptionOriginID = static_cast<WORD>( pQry->GetInt( "nRandomOptionOriginId" ) );
		pItemElem->SetRandomOptionOriginID( wRandomOptionOriginID );
	}

	if( pQry->IsValidColumn( "nRandomOptionExtFlag" ) == true )
	{
		const WORD wRandomOptionExtFlag = static_cast<WORD>( pQry->GetInt( "nRandomOptionExtFlag" ) );
		pItemElem->SetRandomOptionExtensionFlag( wRandomOptionExtFlag );
	}

	if( pQry->IsValidColumn( "nRandomOptionExtSize" ) == true )
	{
		const int nRandomOptionExtSize = pQry->GetInt( "nRandomOptionExtSize" );
		UNREFERENCED_PARAMETER( nRandomOptionExtSize );
	}
	
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		TCHAR column_DST[64] = { 0 };
		FLSPrintf( column_DST, _countof( column_DST ), "%s%d", "nRandomOptionExtDst", Nth + 1 );
		TCHAR column_ADJ[64] = { 0 };
		FLSPrintf( column_ADJ, _countof( column_ADJ ), "%s%d", "nRandomOptionExtAdj", Nth + 1 );
		if( pQry->IsValidColumn( column_DST ) && pQry->IsValidColumn( column_ADJ ) )
		{
			pItemElem->SetRandomOptionExtension( DB_RANDOMOPTION_DST( pQry->GetInt( column_DST ) ), DB_RANDOMOPTION_ADJ( pQry->GetInt( column_ADJ ) ) );
		}
	}
	// END110614
	//////////////////////////////////////////////////////////////////////////

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		if( pQry->IsValidColumn( "idGuild" ) == true )
		{
			const int nGuildID = pQry->GetInt( "idGuild" );
			if( nGuildID > 0 )
			{
				pItemElem->m_idGuild	= static_cast<DWORD>( nGuildID );
			}
		}

		if( pQry->IsValidColumn( "nResistSMItemId" ) == true )
		{
			const int nResistSMItemID	= pQry->GetInt( "nResistSMItemId" );
			if( nResistSMItemID > 0 )
			{
				pItemElem->m_nResistSMItemId	= nResistSMItemID;
			}
		}

		if( pQry->IsValidColumn( "dwLooksChangeItemID" ) == true )
		{
			const DWORD dwLooksChangeItemID	= static_cast<DWORD>( pQry->GetInt( "dwLooksChangeItemID" ) );
			pItemElem->SetLooksChangeItemID( dwLooksChangeItemID );
		}

		if( pQry->IsValidColumn( "bIsCombined" ) == true )
		{
			const bool bIsCombined = static_cast<bool>( pQry->GetInt( "bIsCombined" ) == 1 );
			pItemElem->SetCombinedOption( bIsCombined );
		}

		if( pQry->IsValidColumn( "dwCombinedAddDamage" ) == true )
		{
			const DWORD dwCombinedAddDamage	= static_cast<DWORD>( pQry->GetInt( "dwCombinedAddDamage" ) );
			pItemElem->SetCombinedAddDamage( dwCombinedAddDamage );
		}

		if( pQry->IsValidColumn( "wCombinedRandomOptionOriginID" ) == true )
		{
			const WORD wCombinedRandomOptionOriginID	= static_cast<WORD>( pQry->GetInt( "wCombinedRandomOptionOriginID" ) );
			pItemElem->SetCombinedRandomOptionOriginID( wCombinedRandomOptionOriginID );
		}

		if( pQry->IsValidColumn( "nCombinedGeneralPiercingSize" ) == true )
		{
			const size_t nCombinedGeneralPiercingSize	= static_cast<size_t>( pQry->GetInt( "nCombinedGeneralPiercingSize" ) );
			pItemElem->SetCombinedGeneralPiercingSize( nCombinedGeneralPiercingSize );
		}

		for( size_t Nth = 0; Nth < pItemElem->GetCombinedGeneralPiercingSize(); ++Nth )
		{
			TCHAR szColumn[64] = { 0 };
			FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "dwCombinedGeneralPiercingID", ( Nth + 1 ) );

			if( pQry->IsValidColumn( szColumn ) == true )
			{
				const DWORD dwItemID = static_cast<DWORD>( pQry->GetInt( szColumn ) );
				pItemElem->SetCombinedGeneralPiercingItemID( Nth, dwItemID );
			}
		}

		if( pQry->IsValidColumn( "nCombinedRandomOptionExtensionSize" ) == true )
		{
			const int nCombinedRandomOptionExtensionSize = pQry->GetInt( "nCombinedRandomOptionExtensionSize" );
			UNREFERENCED_PARAMETER( nCombinedRandomOptionExtensionSize );
		}

		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			TCHAR column_DST[64] = { 0 };
			FLSPrintf( column_DST, _countof( column_DST ), "%s%d", "nCombinedRandomOptionExtDst", Nth + 1 );
			TCHAR column_ADJ[64] = { 0 };
			FLSPrintf( column_ADJ, _countof( column_ADJ ), "%s%d", "nCombinedRandomOptionExtAdj", Nth + 1 );
			if( pQry->IsValidColumn( column_DST ) && pQry->IsValidColumn( column_ADJ ) )
			{
				pItemElem->SetCombinedRandomOptionExtension( DB_RANDOMOPTION_DST( pQry->GetInt( column_DST ) ), DB_RANDOMOPTION_ADJ( pQry->GetInt( column_ADJ ) ) );
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

BOOL CDbManager::SendPenya( CQuery *pQry, CMover* pMover, int nPenya, int nNo )
{
	if( SendItemDeleteQuery( pQry, nNo ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::SendItemtoCharacter. PlayerName = %s, Penya = %d" )
			, pMover->m_szName, nPenya );
		return FALSE;
	}
 
//	ULONGLONG ullSumGold = pMover->m_dwGold + nPenya;
//	DWORD dwError = 0xffffffff;
//	if( dwError < ullSumGold )
//	{
//		pMover->m_dwGold = dwError;
//	}
//	else
//	{
//		pMover->m_dwGold = ullSumGold;
//	}
	pMover->AddGold( nPenya, FALSE );
	return TRUE;
}

SERIALNUMBER CDbManager::SendItem( CQuery *pQry, CMover* pMover, __SendItemContents& SendItemContents )
{
	FLItemElem* pItemElem = &SendItemContents.itemElem;
	pItemElem->SetSerialNumber();

	// 유지시간 검사
	DWORD dwKeepTime = (DWORD)SendItemContents.m_dwKeepTime;
	if( pItemElem->IsPeriod() == FALSE && dwKeepTime > 0 )
	{
		DWORD dwTemp;
		int nYear, nMonth, nDay, nHour, nMin;
		nYear	= (int)( SendItemContents.m_dwKeepTime / 100000000 );	dwTemp	= (DWORD)( SendItemContents.m_dwKeepTime % 100000000 );
		nMonth = dwTemp / 1000000;		dwTemp = dwTemp % 1000000;
		nDay = dwTemp / 10000;			dwTemp = dwTemp % 10000;
		nHour = dwTemp / 100;			dwTemp = dwTemp % 100;
		nMin = dwTemp;

		if( g_pCommonCheck->CheckTime( nYear, nMonth, nDay, nHour, nMin, 0 ) == true )
		{
			CTime tKeepTime( nYear, nMonth, nDay, nHour, nMin, 0 );
			dwKeepTime = (DWORD)( tKeepTime.GetTime() );
		}
		else
		{
			return 0;
		}
		
		if( (time_t)dwKeepTime <= time( NULL ) )
		{
			FLERROR_LOG( LOG_SEND_ITEM, _T( "[ERROR] dwKeepTime Over IdPlayer = %d, PlayerName = %s, ItemName : %s, KeepTime = %d" )
				, pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem, pItemElem->m_dwKeepTime );
			SendItemDeleteQuery( pQry, SendItemContents.nNo );
			return 0;
		}
	}

	if( dwKeepTime > 0 )
	{
		pItemElem->m_dwKeepTime = dwKeepTime;
	}

	// 피어싱 검사
	if( pItemElem->IsSetGeneralPiercingOption() == true )
	{
		for( size_t i=0; i<pItemElem->GetGeneralPiercingSize(); i++ )		// 일반 아이템
		{
			if( pItemElem->GetGeneralPiercingItemID( i ) != 0 )
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemElem->GetGeneralPiercingItemID( i ) );
				if( pItemProp == NULL || pItemElem->IsPierceAble( pItemProp->dwItemKind3 ) == FALSE )
				{
					FLERROR_LOG( LOG_SEND_ITEM, _T( "[ERROR] Pierced Item is Not SOCKETCARD. Player:[%07d], Name:[%s], ItemName:[%s], MainItemId;[%d]" )
						, pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwItemId );
					SendItemDeleteQuery( pQry, SendItemContents.nNo );
					return 0;
				}
			}
		}
	}

	if( pItemElem->IsSetUltimatePiercingOption() == true )
	{
		for( size_t i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )		// 얼터멋 웨폰
		{
			if( pItemElem->GetUltimatePiercingItemID( i ) != 0 )
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pItemElem->GetUltimatePiercingItemID( i ) );
				if( pItemProp == NULL || pItemProp->dwItemKind3 != IK3_ULTIMATE )
				{
					FLERROR_LOG( LOG_SEND_ITEM, _T( "[ERROR] Pierced Item is Not ULTIMATE. Player:[%07d], Name:[%s], ItemName:[%s], MainItemId;[%d]" )
						, pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwItemId );
					SendItemDeleteQuery( pQry, SendItemContents.nNo );
					return 0;
				}
			}
		}
	}
	// 피어싱 검사

	//////////////////////////////////////////////////////////////////////////
	// 현재 타고 있는 비행체 아이템의 자리 보장.
	if( pMover->m_Inventory.GetEquip( PARTS_RIDE ) != NULL
		&& pMover->m_Inventory.GetEquip( PARTS_RIDE )->GetProp()->GetInventoryType() == pItemElem->GetProp()->GetInventoryType()
		&& pMover->m_Inventory.GetEmptyCountByInventoryType( pItemElem->GetProp()->GetInventoryType() ) <= 1 ) {
			FLINFO_LOG( LOG_SEND_ITEM, _T( "Inventory Full IdPlayer = %d, PlayerName = %s, ItemName : %s" )
				, pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem );
			return 0;
	}
	//////////////////////////////////////////////////////////////////////////

	if( pMover->m_Inventory.Add( pItemElem ) == FALSE )
	{
		FLINFO_LOG( LOG_SEND_ITEM, _T( "Inventory Full IdPlayer = %d, PlayerName = %s, ItemName : %s" )
			, pMover->m_idPlayer, pMover->m_szName,  SendItemContents.OneItem );
		return 0;
	}

	if( SendItemDeleteQuery( pQry, SendItemContents.nNo ) == FALSE )
	{
		return 0;
	}

	return pItemElem->GetSerialNumber();
}

BOOL CDbManager::SendItemDeleteQuery( CQuery *pQry, int nNo )
{
	CMover Mover;

	T_CHARACTER_STR_INFO tCharacter( Mover, "S5" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S5" );
 	tCharacter.nPlayerSlot = nNo;

	if( DBQryCharacter( pQry, &tCharacter ) == false )
	{
		return FALSE;
	}
	
	if( pQry->Fetch() )
	{
		int nError	= pQry->GetInt( "fError" );
		if( nError == 0 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

// END110119
//////////////////////////////////////////////////////////////////////////

BOOL CDbManager::ItemLogQuery( CQuery *pQryLog, char *szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, char *szItemName )
{
	char szQuerylog[4096] = { 0, };

	DBQryLog( szQuerylog, _countof( szQuerylog ), szQueryState, uIdPlayer , g_appInfo.dwSys, nNo, 0, 0, 0, 0, info.kLogItem.nItemResist,
		info.kLogItem.nResistAbilityOption, 0, 0, 0, 0, 'A', 0, '\0',
		0.0f, 0.0f, 0.0f, 0, 0, "\0", 0, 0, 0, szItemName, 0, info.kLogItem.nQuantity );
			
	if( FALSE == pQryLog->Exec( szQuerylog ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LC = %s" ), szQuerylog );
		return FALSE;
	}
	
// 	DBQryNewItemLog( szQuerylog, _countof( szQuerylog ), info );
// 	
// 	if( FALSE == pQryLog->Exec( szQuerylog ) )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuerylog );
// 		return FALSE;
// 	}
// 	return TRUE;

	return DBQryNewItemLog( *pQryLog, info );
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

BOOL CDbManager::RemoveItemtoCharacter( CMover* pMover, CQuery *qry, CQuery *qry1, CQuery *qrylog, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/ )
{
	BOOL	bResult		= FALSE;

	CMover Mover;
	Mover.m_idPlayer	= pMover->m_idPlayer;

	T_CHARACTER_STR_INFO tCharacter( Mover, "S6" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S6" );
// 	tCharacter.dwPlayerId = pMover->m_idPlayer;

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return FALSE;
	}

	struct FLRemoveItem
	{
		FLRemoveItem() : nNo( 0 ), Item_Count( 0 ), nAbilityOption( 0 ), nItemResist( 0 ), nResistAbilityOption( 0 ), chState( 0 )
		{
			OneItem[ 0 ] = '\0';
		}

		int					nNo;
		char				OneItem[MAX_PATH];
		int					Item_Count;
		int					nAbilityOption;
		int					nItemResist;
		int					nResistAbilityOption;
		char				chState;
	};

	std::vector< FLRemoveItem > vecRemoveItem;
	
	while( qry->Fetch() )
	{
		//int nNo = 0;
		//char OneItem[MAX_PATH] = { 0, };
		//int Item_Count = 0, m_nAbilityOption = 0, nItemResist = 0, nResistAbilityOption = 0;
		//char chState = 0;
		FLRemoveItem item;

		if( GetRemoveItem( qry, item.nNo, item.OneItem, _countof( item.OneItem ), item.Item_Count, item.nAbilityOption, item.nItemResist, item.nResistAbilityOption, item.chState ) == FALSE )
			continue;

		vecRemoveItem.push_back( item );
	}


	for( std::vector< FLRemoveItem >::iterator it = vecRemoveItem.begin(); it != vecRemoveItem.end(); ++it )
	{
		FLRemoveItem & item = *it;


		SERIALNUMBER iSerialNumber = 0;
		BOOL bIsPenya = FALSE;

		if( 0 == strcmp( item.OneItem, "penya" ) )				// 페냐인지 확인
		{
			if( RemovePenya( qry1, pMover, item.Item_Count, item.nNo, item.chState ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "RemovePenya PlayerName = %s, Item_Count = %d, State = %c" )
					, pMover->m_szName, item.Item_Count, item.chState );
				continue;
			}
			bIsPenya = TRUE;
		}
		else												// 아이템 삭제
		{
			iSerialNumber	= RemoveItem( qry1, item.nNo, pMover, item.OneItem, item.Item_Count, item.nAbilityOption, item.nItemResist, item.nResistAbilityOption, item.chState );
		}

		if( bIsPenya )				// 페냐인지 확인
		{
			//sprintf( OneItem, "SEED" );
			FLSPrintf( item.OneItem, _countof( item.OneItem ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
			iSerialNumber = 0;
		}
		else
		{
			if( iSerialNumber == 0 )
				continue;
		}

		bResult		= TRUE;

		LogItemInfo aLogItem;

		if( item.chState == 'I' )
		{
			//aLogItem.Action = "I";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "I" );
		}
		else
		{
			//aLogItem.Action = "K";
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "K" );
		}

		//aLogItem.SendName = pMover->m_szName;
		//aLogItem.RecvName = "DB";
		FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pMover->m_szName );
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "DB" );

		//////////////////////////////////////////////////////////////////////////
		// mirchang_101011 LOG_ITEM_STR Send, Recv PlayerID 추가
		aLogItem.idSendPlayer = pMover->m_idPlayer;
		aLogItem.idRecvPlayer = 0;
		//////////////////////////////////////////////////////////////////////////

		//aLogItem.ItemName = OneItem;
		// mirchang_100409 item name -> item Id
		char szItemId[32] = {0, };
		if( lstrlen( item.OneItem ) > 0 )
		{
			PT_ITEM_SPEC pItemProp = NULL;
			int nIndex = atoi( item.OneItem );
			if( nIndex > 0 )
			{
				pItemProp = g_xSpecManager->GetSpecItem( nIndex );
			}
			else
			{
				pItemProp = g_xSpecManager->GetSpecItem( item.OneItem );
			}
			if( pItemProp != NULL )
			{
				FLULongToString( pItemProp->dwID, szItemId, _countof( szItemId ), 10 );
			}
			else
			{
				FLSPrintf( szItemId, _countof( szItemId ), "%d", -1 );
			}
		}
		FLStrcpy( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), szItemId );
		// mirchang_100317 item name -> item Id
		aLogItem.Gold2 = pMover->GetGold();
		aLogItem.kLogItem.nQuantity = item.Item_Count;
		aLogItem.kLogItem.nItemResist = item.nItemResist;
		aLogItem.kLogItem.nResistAbilityOption = item.nResistAbilityOption;
		
		if( bIsPenya )
		{
			aLogItem.Gold = pMover->GetGold() + item.Item_Count;
		}
		else
		{
			aLogItem.Gold = pMover->GetGold();
			aLogItem.kLogItem.dwSerialNumber = iSerialNumber;
			aLogItem.kLogItem.nAbilityOption =item.nAbilityOption;
		}

#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
		UNREFERENCED_PARAMETER( qrylog );
		LOG_DB().PostQueryCommand( pMover->m_szAccount[ 0 ], new FLItem_LogQuery( "LD", aLogItem, pMover->m_idPlayer, item.nNo, item.OneItem ) );
#else
		if( ItemLogQuery( qrylog, "LD", aLogItem, pMover->m_idPlayer, item.nNo, item.OneItem ) == FALSE )
			continue;
#endif

	}
	return bResult;
}

BOOL CDbManager::RemoveItemDeleteQuery( CQuery *pQry, int nNo )
{
	CMover Mover;

	T_CHARACTER_STR_INFO tCharacter( Mover, "S7" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S7" );
 	tCharacter.nPlayerSlot = nNo;

	if( DBQryCharacter( pQry, &tCharacter ) == false )
	{
		return FALSE;
	}

	if( pQry->Fetch() )
	{
		int nError	= pQry->GetInt( "fError" );
		if( nError == 0 )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

// END110119
//////////////////////////////////////////////////////////////////////////

HANDLE s_hHandle	= (HANDLE)NULL;

BOOL CDbManager::CreateDbWorkers( void )
{
	s_hHandle	= CreateEvent( NULL, FALSE, FALSE, NULL );

	DWORD dwThreadId;

	m_hIOCPGet	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// select
	FLASSERT( m_hIOCPGet );
	for( int i = 0; i < MAX_GETTHREAD_SIZE; i++ )
	{
		m_hThreadGet[i]		= chBEGINTHREADEX( NULL, 0, _GetThread, (LPVOID)this, 0, &dwThreadId ); 
		FLASSERT( m_hThreadGet[i] );
		SetThreadPriority( m_hThreadGet[i], THREAD_PRIORITY_BELOW_NORMAL );
//		Sleep( 1000 );
		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC" ) );
	}
	m_hIOCPPut	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// insert log
	FLASSERT( m_hIOCPPut );
	for( int i = 0; i < MAX_PUTTHREAD_SIZE; i++ )
	{
		m_hThreadPut[i]		= chBEGINTHREADEX( NULL, 0, _PutThread, (LPVOID)this, 0, &dwThreadId ); 
		FLASSERT( m_hThreadPut[i] );
		SetThreadPriority( m_hThreadPut[i], THREAD_PRIORITY_BELOW_NORMAL );
//		Sleep( 1000 );
		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC" ) );
	}

	m_hIOCPGuild = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// 길드용
	m_hThreadGuild = chBEGINTHREADEX( NULL, 0, _GuildThread, (LPVOID)this, 0, &dwThreadId ); 

	if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
		FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC" ) );

	m_hCloseSPThread	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hSPThread	= chBEGINTHREADEX( NULL, 0, _SPThread, (LPVOID)this, 0, &dwThreadId );	// update
	FLASSERT( m_hSPThread );

#ifdef __S1108_BACK_END_SYSTEM
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _BackSystem, this, 0, &dwThreadId );
	FLASSERT( m_hWorker );
#endif // __S1108_BACK_END_SYSTEM
	
	m_hItemUpdateCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hItemUpdateWorker	= chBEGINTHREADEX( NULL, 0, _ItemUpdateThread, this, 0, &dwThreadId );
	FLASSERT( m_hWorker );

	for( int i = 0; i < MAX_QUERY_RESERVED; i++ )
	{
		m_apQuery[i]	= new CQuery;
#ifdef __DbStuff
		if (!m_apQuery[i]->Connect(CQuery::SqlDatabase::CHARACTER))
#else
		if( FALSE == m_apQuery[i]->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
#endif
		{
			for( i = 0; i < MAX_QUERY_RESERVED; i++ )
				SAFE_DELETE( m_apQuery[i] );
			FLTRACE_LOG( PROGRAM_NAME, _T( "MAX_QUERY_RESERVED" ) );
			return FALSE;
		}
	}
	return TRUE;
}

void CDbManager::CloseDbWorkers( void )
{
	if( NULL != m_hIOCPGet )
	{
		for( int i = 0; i < MAX_GETTHREAD_SIZE; i++ )
			PostQueuedCompletionStatus( m_hIOCPGet, 0, NULL, NULL );
		WaitForMultipleObjects( MAX_GETTHREAD_SIZE, m_hThreadGet, TRUE, INFINITE );
		CLOSE_HANDLE( m_hIOCPGet );
		for( int i = 0; i < MAX_GETTHREAD_SIZE; i++ )
			CLOSE_HANDLE( m_hThreadGet[i] );
	}
	if( NULL != m_hIOCPPut )
	{
		for( int i = 0; i < MAX_PUTTHREAD_SIZE; i++ )
			PostQueuedCompletionStatus( m_hIOCPPut, 0, NULL, NULL );
		WaitForMultipleObjects( MAX_PUTTHREAD_SIZE, m_hThreadPut, TRUE, INFINITE );
		CLOSE_HANDLE( m_hIOCPPut );
		for( int i = 0; i < MAX_PUTTHREAD_SIZE; i++ )
			CLOSE_HANDLE( m_hThreadPut[i] );
	}

	if( m_hIOCPGuild )
	{
		PostQueuedCompletionStatus( m_hIOCPGuild, 0, NULL, NULL );
		WaitForSingleObject( m_hThreadGuild, INFINITE );
		CLOSE_HANDLE( m_hIOCPGuild );
		CLOSE_HANDLE( m_hThreadGuild );
	}

#ifdef __S1108_BACK_END_SYSTEM
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
#endif // __S1108_BACK_END_SYSTEM

	CLOSE_THREAD( m_hItemUpdateWorker, m_hItemUpdateCloseWorker );

	CLOSE_THREAD( m_hSPThread, m_hCloseSPThread );

	for( int i = 0; i < MAX_QUERY_RESERVED; i++ )
		SAFE_DELETE( m_apQuery[i] );

	CLOSE_HANDLE( s_hHandle );
}

UINT CDbManager::_GetThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->GetThread();
	return 0;
}

UINT CDbManager::_PutThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->PutThread();
	return 0;
}

UINT CDbManager::_SPThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->SPThread();
	return 0;
}

UINT CDbManager::_UpdateThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->UpdateThread();
	return 0;
}

UINT CDbManager::_GuildThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->GuildThread();
	return 0;
}

#ifdef __S1108_BACK_END_SYSTEM
UINT CDbManager::_BackSystem( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->BackSystem();
	return 0;
}
#endif // __S1108_BACK_END_SYSTEM

UINT CDbManager::_ItemUpdateThread( LPVOID pParam )
{
	CDbManager* pDbManager	= (CDbManager*)pParam;
	pDbManager->ItemUpdateThread();
	return 0;
}

void CDbManager::GetThread( void )
{
	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		return;
	}
	
	CQuery* pQuery1	= new CQuery;
#ifdef __DbStuff
	if (!pQuery1->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( FALSE == pQuery1->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQuery1 );
		return;
	}

	CQuery* pQueryLog = new CQuery;
#ifdef __DbStuff
	if (!pQueryLog->Connect(CQuery::SqlDatabase::LOGGING))
#else
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LogDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQuery1 );
		SAFE_DELETE( pQueryLog );
		return;
	}

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	ULONG_PTR dwCompletionKey	= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPGet, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );

		if( FALSE == bReturnValue )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetQueuedCompletionStatus return FALSE. Error:[%d]" ), ::GetLastError() );
			SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
			continue;
		}

		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQuery );
			SAFE_DELETE( pQuery1 );
			SAFE_DELETE( pQueryLog );
			SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
			return;
		}

		switch( lpDbOverlappedPlus->nQueryMode )
		{
		case SENDPLAYERLIST:
			SendPlayerList( pQuery, lpDbOverlappedPlus );
			break;
		case JOIN:
			Join( pQuery, pQuery1, pQueryLog, lpDbOverlappedPlus );
			break;
		case CREATEPLAYER:
			CreatePlayer( pQuery, lpDbOverlappedPlus );
			break;
		case REMOVEPLAYER:
			RemovePlayer( pQuery, lpDbOverlappedPlus );
			break;
		case QM_LOGIN_PROTECT:
			LoginProtectCert( pQuery, lpDbOverlappedPlus );
			break;
#ifdef PASSWORD_RESET_2ND
		case CHANGE_BANKPASS:
			ChangeBankPass( pQuery, lpDbOverlappedPlus );
			break;
#endif // PASSWORD_RESET_2ND
		default:
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "lpDbOverlappedPlus->nQueryMode Invalid : [%d]" ), lpDbOverlappedPlus->nQueryMode );
				break;
			}
		}

		SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
	}
}

void CDbManager::PutThread( void )	// log
{
	CQuery* pQueryChar	= new CQuery;
#ifdef __DbStuff
	if (!pQueryChar->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( FALSE == pQueryChar->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterDB Connect Failed" ) );
		SAFE_DELETE( pQueryChar );
		return;
	}

	CQuery* pQueryLog	= new CQuery;
#ifdef __DbStuff
	if (!pQueryLog->Connect(CQuery::SqlDatabase::LOGGING))
#else
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LogDB Connect Failed" ) );
		SAFE_DELETE( pQueryChar );
		SAFE_DELETE( pQueryLog );
		return;
	}


	if( CSerialNumber::GetInstance()->IsInit() == false )
	{
		//////////////////////////////////////////////////////////////////////////
		// TODO 시리얼 넘버를 먼저 할당 받는다.(나중에 구조적으로 이쁘게 수정하자)
		static bool bInitSerialNumber = false;
		static CMclCritSec csInitSerialNumber;

		CMclAutoLock	Lock( csInitSerialNumber );
		if( bInitSerialNumber == false )
		{
			for( int i = 0; i < 2; ++i )
			{
				LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = CDBSerialNumberRequest::GetInstance()->MakeRequest();
				SelectSerialNumber( pQueryChar, lpDbOverlappedPlus );
			}

			bInitSerialNumber = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	ULONG_PTR dwCompletionKey		= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPPut, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		
		if( FALSE == bReturnValue )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetQueuedCompletionStatus return FALSE. Error:[%d]" ), ::GetLastError() );
			SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
			continue;
		}

		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQueryChar );
			SAFE_DELETE( pQueryLog );
			SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
			return;
		}

		switch( lpDbOverlappedPlus->nQueryMode )
		{
			case SAVECONCURRENTUSERNUMBER:
				LogConcurrentUserNumber( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_PLAY_CONNECT:
				LogPlayConnect( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_TRADE_ITEM:
				LogTradeItem( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_ITEM:
				LogItem( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_PLAY_DEATH:
				LogPlayDeath( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_LEVELUP:
				LogLevelUp( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_SERVER_DEATH:
				LogServerDeath( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_UNIQUEITEM:
				LogUniqueItem( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_QUEST:
				LogQuest( pQueryLog, lpDbOverlappedPlus );
				break;
			case GAMEMA_CHAT:
				Gamema_Chat( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_PK_PVP:
				LogPkPvp( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_SCHOOL:
				LogSchool( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_SKILLPOINT:
				LogSkillPoint( pQueryLog, lpDbOverlappedPlus );
				break;

			case QM_SET_PLAYER_NAME:
				SetPlayerName( pQueryChar, pQueryLog, lpDbOverlappedPlus );
				break;

			case QM_SNOOPGUILD:
				SnoopGuild( pQueryLog, lpDbOverlappedPlus );
				break;

			case SAVE_PLAY_TIME:
				SavePlayTime( pQueryChar, lpDbOverlappedPlus );
				break;

			case LOG_GETHONORTIME: 
				LogGetHonorTime( pQueryLog, lpDbOverlappedPlus );
				break;

			case QM_ADD_MESSENGER:
				AddMessenger( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_DELETE_MESSENGER:
				DeleteMessenger( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_UPDATE_MESSENGER:
				UpdateMessenger( pQueryChar, lpDbOverlappedPlus );
				break;

			case ADD_PARTYNAME:
				AddPartyName( pQueryChar, lpDbOverlappedPlus );
				break;
			case CHANGE_BANKPASS:
				ChangeBankPass( pQueryChar, lpDbOverlappedPlus );
				break;

			case INSERT_TAG:
				InsertTag( pQueryChar, lpDbOverlappedPlus );
				break;

			case SCHOOL_REPORT:
				SchoolReport( pQueryLog, lpDbOverlappedPlus );
				break;

			case QM_DELETE_REMOVE_GUILD_BANK_TBL:
				DeleteRemoveGuildBankTbl( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_CALL_USPLOGGINGQUEST:
				call_uspLoggingQuest( pQueryLog, lpDbOverlappedPlus );
				break;
			case QM_CALL_XXX_MULTI_SERVER:
				call_uspXXXMultiServer( pQueryChar, lpDbOverlappedPlus );
				break;
			case QM_LOG_EXPBOX:
				LogExpBox( pQueryLog, lpDbOverlappedPlus );
				break;
			case QM_CALL_USP_PET_LOG:
				CalluspPetLog( pQueryLog, lpDbOverlappedPlus );
				break;
			case LOG_INSTANCEDUNGEON:
				LogInstanceDungeon( pQueryLog, lpDbOverlappedPlus );
				break;

			case LOG_ERROR:
				LogError( pQueryLog, lpDbOverlappedPlus );
				break;

			case LOG_GUILDFURNITURE:
				LogGuildFurniture( pQueryLog, lpDbOverlappedPlus );
				break;

				//////////////////////////////////////////////////////////////////////////
				// mirchang_100723 give coupon item event

			case QM_QUERY_COUPON_NUMBER:
				GetCouponNumberItem( pQueryChar, lpDbOverlappedPlus );
				break;

				// mirchang_100723 give coupon item event
				//////////////////////////////////////////////////////////////////////////

			case QM_QUERY_SERIALNUMBER:
				SelectSerialNumber( pQueryChar, lpDbOverlappedPlus );
				break;

			case QM_TELEPORT_MAP_ADD_POINT:			// 텔레포트맵 등록 지점 추가
				TeleportMapAddPoint( pQueryChar, pQueryLog, lpDbOverlappedPlus );
				break;
			case QM_TELEPORT_MAP_CHG_POINT_NAME:	// 텔레포트맵 등록 지점 이름 변경
				TeleportMapChgPointName( pQueryChar, pQueryLog, lpDbOverlappedPlus );
				break;
			case QM_TELEPORT_MAP_DEL_POINT:			// 텔레포트맵 등록 지점 삭제
				TeleportMapDelPoint( pQueryChar, pQueryLog, lpDbOverlappedPlus );
				break;

			//case QM_CONSIGNMENT_SALE_GET_REGISTED_COUNT:
			//	ConsignmentSale_GetRegistedCount( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_REGIST:
			//	ConsignmentSale_Regist( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_CANCEL_REGISTED:
			//	ConsignmentSale_CancelRegisted( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_REGISTED_INFO_LIST:
			//	ConsignmentSale_RegistedInfoList( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_GET_PRICE:
			//	ConsignmentSale_GetPrice( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_CLEAR_RESERVED_INFO:
			//	ConsignmentSale_ClearReserveInfo( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_BUY_ITEM:
			//	ConsignmentSale_BuyItem( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_COLLECT_SALE_GOLD:
			//	ConsignmentSale_CollectSaleGold( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_SEARCH:
			//	ConsignmentSale_Search( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;
			//case QM_CONSIGNMENT_SALE_SALE_GOLD_INFO_LIST:
			//	ConsignmentSale_SaleGoldInfoList( pQueryChar, pQueryLog, lpDbOverlappedPlus );
			//	break;

			case QM_CHARACTER_SERVER_TRANSFORM:
				CharacterServerTransform( pQueryChar, lpDbOverlappedPlus );
				break;
			default:
				FLERROR_LOG( PROGRAM_NAME, _T( "lpDbOverlappedPlus->nQueryMode Invalid : [%d]" ), lpDbOverlappedPlus->nQueryMode );
				break;
		} // end of switch 

		SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
	} // end of while
}

void CDbManager::UpdateThread( void )
{
#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
#else
	CQuery* pQuery	= new CQuery;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		return;
	}

	CQuery* pQueryLog	= new CQuery;
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LogDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQueryLog );
		return;
	}

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	ULONG_PTR dwCompletionKey		= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	char* szQuery	= new char[MAX_QUERY_SIZE];
	CMover* pMover	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPUpdate, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		
		if( FALSE == bReturnValue )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetQueuedCompletionStatus return FALSE. Error:[%d]" ), ::GetLastError() );
			continue;
		}

		if( dwCompletionKey == 0 )
		{
			SAFE_DELETE( pQuery );
			SAFE_DELETE( pQueryLog );
			SAFE_DELETE_ARRAY( szQuery );
			return;
		}

//		SetEvent( m_hWait );

		pMover	= (CMover*)dwCompletionKey;
		SavePlayer( pQuery, pQueryLog, pMover, szQuery, MAX_QUERY_SIZE );
		if( pMover->m_bLogout )
		{
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer( pMover->m_szAccount, 0 );
		}

		SAFE_DELETE( pMover );
	}
#endif
}

void CDbManager::SPThread( void )
{
#ifdef MODIFY_INVEN_ITEM_DISSOLUTION_BUG
#else
	DWORD dwThreadId;
	m_hIOCPUpdate	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );	// insert log
	FLASSERT( m_hIOCPUpdate );
	for( int i = 0; i < MAX_UPDATETHREAD_SIZE; i++ )
	{
		m_hThreadUpdate[i]		= chBEGINTHREADEX( NULL, 0, _UpdateThread, (LPVOID)this, 0, &dwThreadId ); 
		FLASSERT( m_hThreadUpdate[i] );
		SetThreadPriority( m_hThreadUpdate[i], THREAD_PRIORITY_BELOW_NORMAL );
		if( WaitForSingleObject( s_hHandle, SEC( 3 ) ) == WAIT_TIMEOUT )
			FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// TIMEOUT\t// ODBC" ) );
	}
	
	WaitForSingleObject( m_hCloseSPThread, INFINITE );

	for( int i = 0; i < MAX_UPDATETHREAD_SIZE; i++ )
		PostQueuedCompletionStatus( m_hIOCPUpdate, 0, NULL, NULL );
	WaitForMultipleObjects( MAX_UPDATETHREAD_SIZE, m_hThreadUpdate, TRUE, INFINITE );
	CLOSE_HANDLE( m_hIOCPUpdate );
	for( int i = 0; i < MAX_UPDATETHREAD_SIZE; i++ )
		CLOSE_HANDLE( m_hThreadUpdate[i] );
#endif
}

void CDbManager::CreateGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//	u_long idPlayer, idGuild;
	int nSize;
	GUILD_MEMBER_INFO info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];
//	ar >> idPlayer >> idGuild;
	ar >> nSize;
	ar.Read( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar >> idGuild;
	ar.ReadString( szGuild, _countof( szGuild ) );

	GUILD_QUERYINFO qi1( "A1" );
	qi1.idGuild	= idGuild;
	qi1.pszGuild	= szGuild;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi1 );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
	if( pQuery->Fetch() )
	{
		int nError	= pQuery->GetInt( "nError" );
		if( nError != 1 )
		{
			return;
		}
	}
	for( int i = 0; i < nSize; i++ )
	{
		if( info[i].idPlayer != 0 )
		{
			GUILD_QUERYINFO qi2( "A2" );
//			qi2.idPlayer	= idPlayer;
			qi2.idPlayer	= info[i].idPlayer;
			qi2.idGuild	= idGuild;
			qi2.nLevel		= ( i == 0? GUD_MASTER: GUD_ROOKIE );
			DBQryGuild( szQuery, _countof( szQuery ), qi2 );
			if( FALSE == pQuery->Exec( szQuery ) )
			{
//				FLERROR_LOG( PROGRAM_NAME, _T( "CreateGuild(): A2" ) );
			}
		}
	}

	GUILDLOG_QUERYINFO qi( "L3" );
	qi.idGuild = idGuild;
	qi.idPlayer = info[0].idPlayer;
	qi.pszState = "S";
	qi.pszGuildBank = szGuild;
	
	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		return;
	}
	GUILDLOG_QUERYINFO LogQi( "L1" );
	LogQi.idGuild = idGuild;
	LogQi.idDoPlayer = info[0].idPlayer;
	LogQi.pszState = "G";

	for( int i = 0; i < nSize; i++ )
	{
		if( info[i].idPlayer != 0 )
		{
			LogQi.idPlayer = info[i].idPlayer;
			DBQryGuildLog( szQuery, _countof( szQuery ), LogQi );
			if( FALSE == pQueryLog->Exec( szQuery ) )
			{
				return;
			}
		}
	}
}

void CDbManager::DestroyGuild( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild, idMaster;
	ar >> idGuild;
	ar >> idMaster;

	GUILD_QUERYINFO info( "D1" );
	info.idGuild	= idGuild;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}

	GuildHouseDBMng->PostRequest( GUILDHOUSE_REMOVE, NULL, 0, idGuild );

	GUILDLOG_QUERYINFO qi( "L3" );
	qi.idGuild = idGuild;
	qi.idPlayer = idMaster;
	qi.pszState = "E";
	
	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		return;
	}
}
	
void CDbManager::AddGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer, idGuild, DoidPlayer;
	ar >> idPlayer >> idGuild >> DoidPlayer;
	GUILD_QUERYINFO qi( "A2" );
	qi.idPlayer	= idPlayer;
	qi.idGuild	= idGuild;
	qi.nLevel		= GUD_ROOKIE;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}

	GUILDLOG_QUERYINFO LogQi( "L1" );
	LogQi.idGuild = idGuild;
	LogQi.idPlayer = idPlayer;
	LogQi.idDoPlayer = DoidPlayer;
	LogQi.pszState = "J";
	
	DBQryGuildLog( szQuery, _countof( szQuery ), LogQi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::RemoveGuildMember( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer, idGuild, DoidPlayer;
	ar >> idPlayer >> idGuild >> DoidPlayer;

	GUILD_QUERYINFO qi( "D2" );
	qi.idPlayer	= idPlayer;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}

	GUILDLOG_QUERYINFO LogQi( "L1" );
	LogQi.idGuild = idGuild;
	LogQi.idPlayer = idPlayer;
	LogQi.idDoPlayer = DoidPlayer;
	if( idPlayer == DoidPlayer )
		LogQi.pszState = "L";
	else
		LogQi.pszState = "R";
	
	DBQryGuildLog( szQuery, _countof( szQuery ), LogQi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildMemberLv( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv;

	GUILD_QUERYINFO qi( "U2" );
	qi.idPlayer	= idPlayer;
	qi.nLevel	= static_cast< WORD >( nMemberLv );

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildClass( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer;
	int nClass;
	ar >> idPlayer >> nClass;
	
	GUILD_QUERYINFO qi( "U9" );
	qi.idPlayer	= idPlayer;
	qi.nClass	= nClass;
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildNickName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer;
	char strNickName[MAX_GM_ALIAS] = { 0, };
	ar >> idPlayer;
	ar.ReadString( strNickName, _countof( strNickName ) );
	
	GUILD_QUERYINFO qi( "UA" );
	qi.idPlayer	= idPlayer;
	qi.pszGuild = strNickName;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildMaster( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idPlayer, idPlayer2;
	ar >> idPlayer >> idPlayer2;

	GUILD_QUERYINFO qi( "U2" );
	qi.idPlayer	= idPlayer;
	qi.nLevel	= GUD_ROOKIE;
	qi.nClass	= 0;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), qi );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}

	qi.idPlayer	= idPlayer2;
	qi.nLevel	= GUD_MASTER;
	DBQryGuild( szQuery, _countof( szQuery ), qi );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::OpenGuild( void )
{
//	0
	g_GuildMng.Clear();
	g_GuildWarMng.Clear();

//	1
	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
#endif
	{
		char s[128] = { 0, };
		FLSPrintf( s, _countof( s ), "DB open failed: %s", DSN_NAME_CHARACTER01 );
		AfxMessageBox( s );
		SAFE_DELETE( pQuery );
		return;
	}


	CQuery* rankingQuery = new CQuery;
#ifdef __DbStuff // Properly intialize a connection with ranking
	if (!rankingQuery->Connect(CQuery::SqlDatabase::RANKING))
#else
	if (!rankingQuery->Connect(3, "RANKING_DBF", DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
#endif
	{
		char s[128] = { 0, };
		FLSPrintf(s, _countof(s), "DB open failed: RANKING_DBF");
		AfxMessageBox(s);
		SAFE_DELETE(pQuery);
		SAFE_DELETE(rankingQuery);
		return;
	}

	char szSql[1024];		
	//  길드 가져오기 모두 다~~~
	DBQryGuild( szSql, _countof( szSql ), "S1" );	// ORDER BY
	if( FALSE == pQuery->Exec( szSql ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "sql error: %s" ), szSql );
		SAFE_DELETE( pQuery );
		return;
	}

	while( pQuery->Fetch() )
	{
		CGuild* pGuild	= new CGuild;
		
		pGuild->m_idGuild						= (u_long)pQuery->GetInt( "m_idGuild" );
		pGuild->m_nLevel						= pQuery->GetInt( "m_nLevel" );
		pGuild->m_dwContributionPxp				= (DWORD)pQuery->GetInt( "m_nGuildPxp" );
		pGuild->m_dwLogo						= (DWORD)pQuery->GetInt( "m_dwLogo" );
		//  권한
		pGuild->m_adwPower[GUD_MASTER]			= 0x000000FF;
		pGuild->m_adwPower[GUD_KINGPIN]			= (DWORD)pQuery->GetInt( "Lv_1" );
		pGuild->m_adwPower[GUD_CAPTAIN]			= (DWORD)pQuery->GetInt( "Lv_2" );
		pGuild->m_adwPower[GUD_SUPPORTER]		= (DWORD)pQuery->GetInt( "Lv_3" );
		pGuild->m_adwPower[GUD_ROOKIE]			= (DWORD)pQuery->GetInt( "Lv_4" );
		
		//  월급량
		pGuild->m_adwPenya[GUD_MASTER]			= (DWORD)pQuery->GetInt( "Pay_0" );
		pGuild->m_adwPenya[GUD_KINGPIN]			= (DWORD)pQuery->GetInt( "Pay_1" );
		pGuild->m_adwPenya[GUD_CAPTAIN]			= (DWORD)pQuery->GetInt( "Pay_2" );
		pGuild->m_adwPenya[GUD_SUPPORTER]		= (DWORD)pQuery->GetInt( "Pay_3" );
		pGuild->m_adwPenya[GUD_ROOKIE]			= (DWORD)pQuery->GetInt( "Pay_4" );
				
		pGuild->m_bActive						= ( pQuery->GetChar( "isuse" ) == 'F' );

		pQuery->GetStr( "m_szNotice", pGuild->m_szNotice, _countof( pGuild->m_szNotice ) );
		pQuery->GetStr( "m_szGuild", pGuild->m_szGuild, _countof( pGuild->m_szGuild ) );
		StringTrimRight( pGuild->m_szGuild );
		
		pGuild->m_nGoldGuild					= (DWORD)pQuery->GetInt( "m_nGuildGold" );
		pGuild->m_nWin	= pQuery->GetInt( "m_nWin" );
		pGuild->m_nSurrender	= pQuery->GetInt( "m_nSurrender" );
		pGuild->m_nLose	= pQuery->GetInt( "m_nLose" );
		g_GuildMng.AddGuild( pGuild );
	}
	// 길드 멤버 가지고 오기 ~~ 길드에 따른 멤버들의 정보
	DBQryGuild( szSql, _countof( szSql ), "S2" );
	if( FALSE == pQuery->Exec( szSql ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "sql error: %s" ), szSql );
		SAFE_DELETE( pQuery );
		return;
	}

	u_long idGuild;
	while( pQuery->Fetch() )
	{
		CGuildMember* pMember	= new CGuildMember;
		pMember->m_idPlayer			= (u_long)pQuery->GetInt( "m_idPlayer" );
		pMember->m_nMemberLv		= static_cast< BYTE >( pQuery->GetInt( "m_nMemberLv" ) );
		pMember->m_nGiveGold		= pQuery->GetInt( "m_nGiveGold" );
		pMember->m_dwGivePxpCount	= (DWORD)pQuery->GetInt( "m_nGivePxp" );
		pMember->m_nPay				= pQuery->GetInt( "m_nPay" );
		pMember->m_nWin				= static_cast< short >( pQuery->GetInt( "m_nWin" ) );
		pMember->m_nLose			= static_cast< short >( pQuery->GetInt( "m_nLose" ) );

#ifdef __GUILDVOTE
		pMember->m_idSelectedVote	= pQuery->GetInt( "m_idVote" );
#endif
		pMember->m_nClass			= pQuery->GetInt( "m_nClass" );
		
		pQuery->GetStr( "m_szAlias", pMember->m_szAlias, _countof( pMember->m_szAlias ) );		
		idGuild						= (u_long)pQuery->GetInt( "m_idGuild" );
		CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );
		if( pGuild )
		{
			pGuild->AddMember( pMember );
			if( pMember->m_nMemberLv == GUD_MASTER )
				pGuild->m_idMaster	= pMember->m_idPlayer;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Player's guild not found - %d, %d" ), pMember->m_idPlayer, idGuild );
			SAFE_DELETE( pMember );
		}
	}
#ifdef __GUILDVOTE
	//	 GUILD 투표 전부 가져오기  GUILD_VOTE_STR 'S1',	@iserverindex
	VOTE_QUERYINFO info("S1");
	DBQryVote( szSql, _countof( szSql ), info );
	if( pQuery->Exec( szSql ) )
	{
		VOTE_INSERTED_INFO vote_info;

		BYTE cbStatus;
		BYTE cbCounts[4];

		while( pQuery->Fetch() )
		{
			vote_info.idGuild	= (u_long)pQuery->GetInt( "m_idGuild" );			
			vote_info.idVote		= (u_long)pQuery->GetInt( "m_idVote" );			

			CGuild* pGuild	= g_GuildMng.GetGuild( vote_info.idGuild );
			if( pGuild == NULL )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "vote[%d] guild[%d] not found" ), info.idVote, info.idGuild );
				continue;
			}

			cbStatus =  static_cast< BYTE >( pQuery->GetInt( "m_cbStatus" ) ); // cbStatus - 투표중:1, 투표완료:2  
			pQuery->GetStr( "m_szTitle", vote_info.szTitle, _countof( vote_info.szTitle ) );	
			pQuery->GetStr( "m_szQuestion", vote_info.szQuestion, _countof( vote_info.szQuestion ) );	
			pQuery->GetStr( "m_szString1", vote_info.szSelections[0], _countof( vote_info.szSelections[0] ) );
			pQuery->GetStr( "m_szString2", vote_info.szSelections[1], _countof( vote_info.szSelections[1] ) );
			pQuery->GetStr( "m_szString3", vote_info.szSelections[2], _countof( vote_info.szSelections[2] ) );
			pQuery->GetStr( "m_szString4", vote_info.szSelections[3], _countof( vote_info.szSelections[3] ) );

			cbCounts[0]	= (BYTE)pQuery->GetInt( "m_cbCount1" );			
			cbCounts[1]	= (BYTE)pQuery->GetInt( "m_cbCount2" );			
			cbCounts[2]	= (BYTE)pQuery->GetInt( "m_cbCount3" );			
			cbCounts[3]	= (BYTE)pQuery->GetInt( "m_cbCount4" );			
			
			pGuild->AddVote( vote_info, cbStatus == 2, cbCounts );
		}
	} 
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "VOTE QUERY ERROR" ) );
	}
#endif // __GUILDVOTE

	// 길드 랭킹정보 받아오기
	CGuildRank* pGuildRank = CGuildRank::Instance();
	pGuildRank->GetRanking(rankingQuery, szSql, _countof( szSql ) );

	WAR_QUERYINFO wqi( "S1" );
	DBQryWar( szSql, _countof( szSql ), wqi );
	
	if( FALSE == pQuery->Exec( szSql ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "sql error: %s" ), szSql );
		SAFE_DELETE( pQuery );
		return;
	}

	char sTime[13]	= { 0, };
	while( pQuery->Fetch() )
	{
		CGuildWar* pWar	= new CGuildWar;
		pWar->m_idWar	= (u_long)pQuery->GetInt( "m_idWar" );
		pWar->m_Decl.idGuild	= (u_long)pQuery->GetInt( "m_idGuild" );
		pWar->m_Decl.nSize	= (u_long)pQuery->GetInt( "m_nCount" );
		pWar->m_Decl.nSurrender	= (u_long)pQuery->GetInt( "m_nSurrender" );
		pWar->m_Decl.nDead	= (u_long)pQuery->GetInt( "m_nDeath" );
		pWar->m_Decl.nAbsent	= (u_long)pQuery->GetInt( "m_nAbsent" );
		pWar->m_Acpt.idGuild	= (u_long)pQuery->GetInt( "f_idGuild" );
		pWar->m_Acpt.nSize	= (u_long)pQuery->GetInt( "f_nCount" );
		pWar->m_Acpt.nSurrender	= (u_long)pQuery->GetInt( "f_nSurrender" );
		pWar->m_Acpt.nDead	= (u_long)pQuery->GetInt( "f_nDeath" );
		pWar->m_Acpt.nAbsent	= (u_long)pQuery->GetInt( "f_nAbsent" );
		pWar->m_nFlag	= pQuery->GetChar( "State" );
		pQuery->GetStr( "StartTime", sTime, _countof( sTime ) );
		GetStrTime( &pWar->m_time, sTime );
		g_GuildWarMng.AddWar( pWar );
		CGuild* pDecl	= g_GuildMng.GetGuild( pWar->m_Decl.idGuild );
		if( pDecl )
			pDecl->m_idEnemyGuild	= pWar->m_Acpt.idGuild;
		CGuild* pAcpt	= g_GuildMng.GetGuild( pWar->m_Acpt.idGuild );
		if( pAcpt )
			pAcpt->m_idEnemyGuild	= pWar->m_Decl.idGuild;
	}

	WAR_QUERYINFO wqi2( "S2" );
	DBQryWar( szSql, _countof( szSql ), wqi2 );
	if( FALSE == pQuery->Exec( szSql ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "sql error: %s" ), szSql );
		return;
	}
	if( pQuery->Fetch() )
		g_GuildWarMng.m_id	= pQuery->GetInt( "Max_m_idWar" );

	SAFE_DELETE( pQuery );
}


void CDbManager::UpdateGuildLogo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD dwLogo;
	ar >> idGuild >> dwLogo;

	GUILD_QUERYINFO info("U3");
	info.idGuild = idGuild;
	info.dwLogo = dwLogo;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildContribution( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	BYTE nLevelUp;  // 레벨업 1 : 0( 했으면 1 )
	LONG nMemberLv;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	CONTRIBUTION_CHANGED_INFO cci;
	ar >> cci;
	ar >> nLevelUp;
	ar >> nMemberLv;

	GUILD_QUERYINFO info("U4");
	info.idGuild	= cci.idGuild;          // 길드 업뎃 
	info.nGuildPxp	= cci.dwGuildPxpCount;	// 길드 Pxp
	info.nGuildGold = cci.dwGuildPenya;		// 길드 Penya
	info.nLevel		= cci.nGuildLevel;		// 길드 레벨
	info.idPlayer	= cci.idPlayer;			// 길드멤버 업뎃 
	info.dwLv1		= cci.dwPenya;			// 공헌 Penya
	info.dwLv2		= cci.dwPxpCount;		// 공헌 Pxp

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}

	if( info.dwLv1 != 0 )	
	{
		// 돈을 넣었을경우
		GUILDLOG_QUERYINFO qi( "L2" );	// 길드 창고 로그
		qi.idGuild = info.idGuild;
		qi.idPlayer = info.idPlayer;
		qi.nGuildGold = info.nGuildGold;
		qi.nItem	= info.dwLv1;
		if( nLevelUp )
		{
			qi.pszState = "L";
		}
		else
		{
			qi.pszState = "I";
		}
		DBQryGuildLog( szQuery, _countof( szQuery ), qi );
		if( FALSE == pQueryLog->Exec( szQuery ) )
		{
			return;
		}
		GUILDLOG_QUERYINFO qi1( "L4" );	// 길드 공헌 로그
		qi1.idGuild = info.idGuild;
		qi1.idPlayer = info.idPlayer;
		qi1.nGuildGold = info.nGuildGold;
		qi1.nLevel = nMemberLv;
		qi1.nGuildLv = info.nLevel;
		qi1.nGuildPxp = info.nGuildPxp;
		if( nLevelUp )
		{
			qi1.pszState = "L";
		}
		else
		{
			qi1.pszState = "G";
		}
		DBQryGuildLog( szQuery, _countof( szQuery ), qi1 );
		if( FALSE == pQueryLog->Exec( szQuery ) )
		{
			return;
		}
	}
	else
	{
		// Pxp를 넣었을 경우
		GUILDLOG_QUERYINFO qi2( "L4" );
		qi2.idGuild = info.idGuild;
		qi2.idPlayer = info.idPlayer;
		qi2.nGuildGold = info.nGuildGold;
		qi2.nLevel = nMemberLv;
		qi2.nGuildLv = info.nLevel;
		qi2.nGuildPxp = info.nGuildPxp;
		if( nLevelUp )
		{
			qi2.pszState = "L";
		}
		else
		{
			qi2.pszState = "P";
		}
		DBQryGuildLog( szQuery, _countof( szQuery ), qi2 );
		if( FALSE == pQueryLog->Exec( szQuery ) )
		{
			return;
		}
	}
}

void CDbManager::UpdateGuildNotice( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];
	ar >> idGuild;
	ar.ReadString( szNotice, _countof( szNotice ) );

	GUILD_QUERYINFO info("U5");
	info.idGuild = idGuild;
	info.pszNotice = szNotice;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info);

	if( pQuery->BindParameterVarChar( SQL_PARAM_INPUT, _countof( szNotice ) - 1, szNotice ) == FALSE )
	{
		return;
	}
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildAuthority( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD adwAuthority[MAX_GM_LEVEL];

	ar >> idGuild;
	ar.Read( adwAuthority, sizeof(DWORD) * MAX_GM_LEVEL );
	
	GUILD_QUERYINFO info("U1");
	info.idGuild = idGuild;
	info.dwLv1 = adwAuthority[GUD_KINGPIN];
	info.dwLv2 = adwAuthority[GUD_CAPTAIN];
	info.dwLv3 = adwAuthority[GUD_SUPPORTER];
	info.dwLv4 = adwAuthority[GUD_ROOKIE];

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildPenya( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD adwPenya[MAX_GM_LEVEL];
	ar >> idGuild;
	ar.Read( adwPenya, sizeof(DWORD) * MAX_GM_LEVEL );
	
	GUILD_QUERYINFO info("U6");
	info.idGuild = idGuild;
	info.dwLogo = adwPenya[GUD_MASTER];
	info.dwLv1 = adwPenya[GUD_KINGPIN];
	info.dwLv2 = adwPenya[GUD_CAPTAIN];
	info.dwLv3 = adwPenya[GUD_SUPPORTER];
	info.dwLv4 = adwPenya[GUD_ROOKIE];

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info);
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildRealPay( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	DWORD dwGold;
	DWORD dwPay;
	ar >> idGuild >> dwGold >> dwPay;
		
	GUILD_QUERYINFO info("U7");
	info.idGuild = idGuild;
	info.dwLv1 = dwGold;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}

	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild = idGuild;
	qi.nGuildGold = dwGold;
	qi.nItem	= dwPay;
	qi.pszState = "P";
	
	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildSetName( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild;
	char szName[ MAX_G_NAME ];
	ar >> idGuild;
	ar.ReadString( szName, _countof( szName ) );
	
	GUILD_QUERYINFO info("U8");
	info.idGuild = idGuild;
	info.pszGuild = szName;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryGuild( szQuery, _countof( szQuery ), info );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		return;
	}
}

void CDbManager::OpenQueryGuildBank( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr	ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	//////////////////////////////////////////////////////////////////////////
	DWORD dwWorldServerKey;
	ar >> dwWorldServerKey;

	if( m_dwGuildBankUseWorldKey == NULL_ID )
	{
		m_dwGuildBankUseWorldKey = dwWorldServerKey;
		FLINFO_LOG( PROGRAM_NAME, _T( "[ GUILD BANK USE WORLD ID:(%d) ]" ), m_dwGuildBankUseWorldKey );
	}
	else
	{
		if( m_dwGuildBankUseWorldKey != dwWorldServerKey )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Guild Bank Use Ack World Invalid. Already Use World Key:[%d], Now Ack World Key:[%d]" )
				, m_dwGuildBankUseWorldKey, dwWorldServerKey );

			T_GUILD_BANK_QUERY_FAIL_ACK toWorld;
			toWorld.dwErrorCode				= E_EXIST_ALREADY_USE_WORLD;
			toWorld.dwGuildBankUseWorldID	= m_dwGuildBankUseWorldKey;
			
			BEFORESENDDUAL( ar_toWorld, PACKETTYPE_GUILD_BANK_QUERY_FAIL_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
			ar_toWorld.Write( &toWorld, sizeof( toWorld ) );
			SEND( ar_toWorld, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );
			
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	// 2. 쿼리문을 생성후 실행한다.
	char szSql[128] = { 0, };
	FLSPrintf(szSql, _countof( szSql ), "GUILD_BANK_STR 'S1','0','%02d'", g_appInfo.dwSys);
	if( FALSE == pQuery->Exec( szSql ) )
	{
		return;
	}

	BOOL bFetch	= FALSE;

	//////////////////////////////////////////////////////////////////////////
	CQuery* pBankQuery = new CQuery;

	const char* pass	= CDbManager::GetInstance().DB_ADMIN_PASS_CHARACTER01;
#ifdef __DbStuff
	if (!pBankQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( pBankQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, pass ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Can't connect db: CreateBankQuery" ) );
		SAFE_DELETE( pBankQuery );

		T_GUILD_BANK_QUERY_FAIL_ACK toWorld;
		toWorld.dwErrorCode				= E_QUERY_FAIL;

		BEFORESENDDUAL( ar_toWorld, PACKETTYPE_GUILD_BANK_QUERY_FAIL_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
		ar_toWorld.Write( &toWorld, sizeof( toWorld ) );
		SEND( ar_toWorld, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );

		return;
	}
	//////////////////////////////////////////////////////////////////////////

	do
	{
		// 3. 월드 서버에 전송할 패킷을 생성한다.
		BEFORESENDDUAL( ar2, PACKETTYPE_GUILD_BANK, DPID_UNKNOWN, DPID_UNKNOWN );

		// 4. 로딩할 길드창고 데이터를 스택에 생성한다.
	//	CItemContainer<FLItemElem>	GuildBank;	// 길드 창고
		int							nGoldGuild; // 길드 Credit
		int							nGuildId	= 0;
//		int							nBufsize	= 0;
		int							nCount		= 0;
		ptrdiff_t					nCountTag	= ar2.ReserveSpace( sizeof( nCount ) );

		// 5. 쿼리한 결과를 저장한다.
		//while( bFetch = pQuery->Fetch() )
		for( bFetch = pQuery->Fetch(); bFetch == TRUE ; bFetch = pQuery->Fetch() )
		{
			nCount++;
			CItemContainer<FLItemElem>	GuildBank;	// 길드 창고
			GuildBank.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_GUILD_BANK, MAX_GUILDBANK );
	//		GuildBank.Clear();
			
			nGuildId		= pQuery->GetInt( "m_idGuild" );
			nGoldGuild		= pQuery->GetInt( "m_nGuildGold" );

			if( CDbManager::GetInstance().LoadGuildBank( nGuildId, &GuildBank, pBankQuery ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "LoadGuildBank Failed. GuildId:[%d]" ), nGuildId );
				SAFE_DELETE( pBankQuery );

				T_GUILD_BANK_QUERY_FAIL_ACK toWorld;
				toWorld.dwErrorCode				= E_QUERY_FAIL;

				BEFORESENDDUAL( ar_toWorld, PACKETTYPE_GUILD_BANK_QUERY_FAIL_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
				ar_toWorld.Write( &toWorld, sizeof( toWorld ) );
				SEND( ar_toWorld, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );

				return;
			}
			
// 			int		CountStr		= 0;
// 			int		IndexItem		= 0;
// 			int		itemCount		= 0;
// 			bool	bExistOldData	= false;
// 			
// 
// 			char	Bank[7500]		= { 0, };
// 			pQuery->GetStr( "m_GuildBank", Bank, _countof( Bank ) );
// 			VERIFY_GUILD_STRING( Bank, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
// 			while( '$' != Bank[CountStr] )
// 			{
// 				bExistOldData	= true;
// 
// 				FLItemElem BufItemElem;
// 				IndexItem = GetOneItem( &BufItemElem, Bank, &CountStr );
// 				if( IndexItem == -1 )
// 				{
// 					FLERROR_LOG( PROGRAM_NAME, _T( "프로퍼티 없음. %d, %d" ), nGuildId, BufItemElem.m_dwItemId );
// 				}
// 				else
// 				{
// 					if( IndexItem >= MAX_GUILDBANK )
// 					{
// 						FLERROR_LOG( PROGRAM_NAME, _T( "IndexItem %d, %d" ), nGuildId, IndexItem );
// 						FLERROR_LOG( PROGRAM_NAME, _T( "GuildBank = %s" ), Bank );
// 						return;
// 					}
// 					GuildBank.m_apItem[IndexItem] = BufItemElem;
// 				}
// 			}
// 
// 			GuildBank.SetAllocedSerialNumber( GetSerialNumberStatus( Bank, &CountStr ) );
// 
// 
// 			CountStr		= 0;
// 			char m_apIndex[512]		= { 0, };
// 			pQuery->GetStr( "m_apIndex", m_apIndex, _countof( m_apIndex ) );
// 			VERIFY_GUILD_STRING( m_apIndex, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
// 			while( '$' != m_apIndex[CountStr] )
// 			{
// 				GuildBank.m_apIndex[itemCount]	= (DWORD)GetIntFromStr( m_apIndex, &CountStr );
// 				itemCount++;
// 			}
// 			
// 			CountStr	= 0;
// 			int IndexObjIndex	= 0;
// 			char ObjIndex[512]	= {0,};
// 			pQuery->GetStr( "m_dwObjIndex", ObjIndex, _countof( ObjIndex ) );
// 			VERIFY_GUILD_STRING( ObjIndex, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
// 			while( '$' != ObjIndex[CountStr] )
// 			{
// 				GuildBank.m_apItem[IndexObjIndex].m_dwObjIndex	= (DWORD)GetIntFromStr( ObjIndex, &CountStr );
// 				IndexObjIndex++;
// 			}
// 		
// 
// 			CountStr	= 0;
// 			int nExtBank = 0;
// 			char ExtBank[2000] = {0,};
// 			pQuery->GetStr( "m_extGuildBank", ExtBank, _countof( ExtBank ) );
// 			VERIFYSTRING( ExtBank, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
// 			while( '$' != ExtBank[CountStr] )
// 			{
// 				GuildBank.m_apItem[nExtBank].m_bCharged					= (BOOL)GetIntPaFromStr( ExtBank, &CountStr );
// 				if( GuildBank.m_apItem[nExtBank].m_bCharged != 1 )
// 					GuildBank.m_apItem[nExtBank].m_bCharged	= 0;
// 				GuildBank.m_apItem[nExtBank].m_dwKeepTime				= (DWORD)GetIntPaFromStr( ExtBank, &CountStr );
// 
// //				GuildBank.m_apItem[nExtBank].SetRandomOptItemId( GetInt64PaFromStr( ExtBank, &CountStr ) );
// 
// 				__int64 n64RandomOpionId = GetInt64PaFromStr( ExtBank, &CountStr );
// 				GuildBank.m_apItem[nExtBank].RandomOptionDataConverting( n64RandomOpionId );
// 
// 				GuildBank.m_apItem[nExtBank].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( ExtBank, &CountStr ) );
// 
// 				++CountStr;
// 				++nExtBank;
// 			}
// 		
// 			CountStr	= 0;
// 			int nPirecingBank = 0;
// 			char PirecingBank[4000] = {0,};
// 			pQuery->GetStr( "m_GuildBankPiercing", PirecingBank, _countof( PirecingBank ) );
// 			VERIFYSTRING( PirecingBank, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
// 			while( '$' != PirecingBank[CountStr] )
// 			{
// 				LoadPiercingInfo( GuildBank.m_apItem[nPirecingBank], PirecingBank, &CountStr );
// 				++nPirecingBank;
// 			}
// 
// 			CountStr	= 0;
// 			int nGuildBankPet = 0;
// 			char szGuildBankPet[4200] = {0,};
// 			pQuery->GetStr( "szGuildBankPet", szGuildBankPet, _countof( szGuildBankPet ) );
// 			VERIFYSTRING( szGuildBankPet, g_GuildMng.GetGuild(nGuildId)->m_szGuild );
// 			while( '$' != szGuildBankPet[CountStr] )
// 			{
// 	//			SAFE_DELETE( GuildBank.m_apItem[nGuildBankPet].m_pPet );
// 				BOOL bPet	= (BOOL)GetIntFromStr( szGuildBankPet, &CountStr );
// 				if( bPet )
// 				{
// 					CPet* pPet	= GuildBank.m_apItem[nGuildBankPet].m_pPet	= new CPet;
// 					BYTE nKind	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
// 					pPet->SetKind( nKind );
// 					BYTE nLevel	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
// 					pPet->SetLevel( nLevel );
// 					DWORD dwExp	= (DWORD)GetIntFromStr( szGuildBankPet, &CountStr );
// 					pPet->SetExp( dwExp );
// 					WORD wEnergy	= (WORD)GetIntFromStr( szGuildBankPet, &CountStr );
// 					pPet->SetEnergy( wEnergy );
// 					WORD wLife	= (WORD)GetIntPaFromStr( szGuildBankPet, &CountStr );
// 					pPet->SetLife( wLife );
// 					for( int i = PL_D; i <= pPet->GetLevel(); i++ )
// 					{
// 						BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szGuildBankPet, &CountStr );
// 						pPet->SetAvailLevel( i, nAvailLevel );
// 					}
// 					char szFmt[MAX_PET_NAME_FMT]	= { 0,};
// 					GetStrFromStr( szGuildBankPet, szFmt, &CountStr );
// 					char szName[MAX_PET_NAME]	= { 0,};
// 					GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
// 					pPet->SetName( szName );
// 				}
// 	//			++CountStr;
// 				++nGuildBankPet;
// 			}
// 
// 			GuildBank.RefreshIndex();

			ar2 << nGuildId;
			ar2 << nGoldGuild;

			GuildBank.Serialize(ar2);

			if( nCount >= 1000 )
				break;
		}
		// 패킷 헤더를 설정한다.
		ar2.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );

		// 패킷을 월드서버에 전송한다.
		SEND( ar2, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );

// 		if( bFetch == FALSE )
// 			break;
	} while( bFetch == TRUE );

	SAFE_DELETE( pBankQuery );
}

void CDbManager::UpdateGuildBankUpdate( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr							ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	CItemContainer<FLItemElem>	GuildBank;	// 길드 창고
	int							nGoldGuild; // 길드 Credit
	int							nGuildId	= 0;
	BYTE						cbUpdateContribution;
	u_long						idPlayer, idLogPlayer;
	int							nMode;		// GUILD_PUT_ITEM, GUILD_GET_ITEM, GUILD_PUT_PENYA, GUILD_GET_PENYA, GUILD_CLOAK
	DWORD						dwItemId;	// 아이템 아이디
	int							nItemCount; // 아이템 갯수
	int							nAbilityOption;	// 옵션
	SERIALNUMBER				iSerialNumber;	// 유니크 넘버
	DWORD						dwPenya;	// 거래 페냐량
	GuildBank.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_GUILD_BANK, MAX_GUILDBANK );

	ar >> nGuildId;
	ar >> nGoldGuild;
	GuildBank.Deserialize(ar);
	ar >> cbUpdateContribution;	// 길드멤버 공헌페냐를 변경해야 하는가?
	ar >> idPlayer;				// 뱅크를 업뎃한 플레이어
	ar >> nMode;
	ar >> dwItemId;
	ar >> nAbilityOption;
	ar >> iSerialNumber;
	ar >> nItemCount;
	ar >> dwPenya;

	idLogPlayer = idPlayer;		// 로그에 남길 주체자
	if( cbUpdateContribution != 1 )	// 망토를 산경우에는 공헌도를 감소하지 않는다.
		idPlayer = 0;

//	ItemContainerStruct icsGuildBank;
//	SaveGuildBank( &GuildBank, &icsGuildBank );

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"GUILD_BANK_STR 'U1','%06d','%02d','%d','%s','%s','%s','%07d','%s','%s','%s'",
		nGuildId, g_appInfo.dwSys, nGoldGuild, "$", "$", "$", idPlayer, "$", "$", "$" );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "update guild bank:%s " ), szQuery );
		return;
	}

	SaveGuildBank( nGuildId, &GuildBank, pQuery );

	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild = nGuildId;
	qi.idPlayer = idLogPlayer;
	qi.nGuildGold = nGoldGuild;

	switch( nMode )
	{
	case GUILD_PUT_ITEM:
		qi.pszState = "A";		// 아이템 넣기
		qi.nItem	= dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_GET_ITEM:
		qi.pszState = "D";		// 아이템 빼기
		qi.nItem	= dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_CLOAK:
		qi.pszState = "C";		// 망토 생성
		qi.nItem	= dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_GET_PENYA:
		qi.pszState = "O";		// 돈 빼기
		qi.nItem	= dwPenya;
		break;
	case GUILD_QUERY_REMOVE_GUILD_BANK:
		qi.pszState	= "R";
		break;
	}

	TCHAR szGuildItemLog[6144] = { 0, };
	MakeLogGuildItem( &GuildBank, szGuildItemLog, _countof( szGuildItemLog ) );

	qi.pszGuildBank		= szGuildItemLog;

	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	if( FALSE == pQueryLog->Exec( szQuery ) )
	{
		return;
	}
}

// TODO:try-catch
void CDbManager::AddGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	int i;
	u_long idGuild;
	char szTitle[MAX_BYTE_VOTETITLE];
	char szQuestion[MAX_BYTE_VOTEQUESTION];
	char szSelections[4][MAX_BYTE_VOTESELECT];

	ar >> idGuild;
	ar.ReadString( szTitle, _countof( szTitle ) );
	ar.ReadString( szQuestion, _countof( szQuestion ) );
	for( i=0; i<4; ++i )
		ar.ReadString( szSelections[i], _countof( szSelections[i] ) );


	char szQuery[MAX_QUERY_SIZE]	= { 0,};
    FLSPrintf( szQuery, _countof( szQuery ),
		     "{call GUILD_VOTE_STR('A1', '%02d', '%06d', %d, ?, ?, ?, ?, ?, ?, %d)}",
			 g_appInfo.dwSys, idGuild, 0, 0);

	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_BYTE_VOTETITLE, szTitle );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_BYTE_VOTEQUESTION, szQuestion );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_BYTE_VOTESELECT, szSelections[0] );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_BYTE_VOTESELECT, szSelections[1] );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_BYTE_VOTESELECT, szSelections[2] );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_BYTE_VOTESELECT, szSelections[3] );
	
	u_long idVote = 0;
	if( pQuery->Exec( szQuery ) )
	{
		if( pQuery->Fetch() )
		{
			idVote = (u_long)pQuery->GetInt( "m_idVote" );
		}
	}
	// 성공이던지, 실패이던지 패킷을 보낸다.
	VOTE_INSERTED_INFO	info;
	info.idGuild = idGuild;
	info.idVote = idVote;
	memcpy(info.szTitle, szTitle, MAX_BYTE_VOTETITLE);
	memcpy(info.szQuestion, szQuestion, MAX_BYTE_VOTEQUESTION);
	for( i=0; i<4; ++i )
		memcpy( info.szSelections[i], szSelections[i], MAX_BYTE_VOTESELECT );
	g_dpCoreSrvr.SendAddVoteResult( info );
}

// 투표제거 
void CDbManager::RemoveGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idVote;
	ar >> idVote;

	VOTE_QUERYINFO info("D1");
	info.idVote = idVote;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryVote( szQuery, _countof( szQuery ), info );  
	pQuery->Exec( szQuery );
}

// 투표 종료 
void CDbManager::CloseGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idVote;
	ar >> idVote;

	VOTE_QUERYINFO info("U2");
	info.idVote = idVote;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryVote( szQuery, _countof( szQuery ), info );  
	pQuery->Exec( szQuery );
}

//투표하기 
void CDbManager::CastGuildVote( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idVote;
	BYTE cbSelection;
	ar >> idVote;
	ar >> cbSelection;	
	
	VOTE_QUERYINFO info("U1");
	info.idVote = idVote;
	info.cbSelection = cbSelection;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryVote( szQuery, _countof( szQuery ), info );  
	pQuery->Exec( szQuery );
}



void CDbManager::AcptWar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "ACPT_GUILD_WAR" ) );
	u_long idWar, idDecl, idAcpt;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idDecl >> idAcpt;
	
//	GUILD_WAR_STR 'A1',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild
	WAR_QUERYINFO wqi( "A1" );
	wqi.idGuild	= idDecl;
	wqi.idWar	= idWar;
	wqi.iState	= WF_WARTIME;
	wqi.f_idGuild	= idAcpt;
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, _countof( szQuery ), wqi );
	pQuery->Exec( szQuery );
}

void CDbManager::WarEnd( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "WAR_END" ) );
	u_long idWar, idDecl, idAcpt;
	int nWptDecl, nWptAcpt, nType;
	char GuildWarStartTime[15] = {0,};
	int nWinPointDecl, nWinPointAcpt, nGetPointDecl, nGetPointAcpt;
	
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idDecl >> idAcpt >> nWptDecl >> nWptAcpt >> nType;
	ar.ReadString( GuildWarStartTime, _countof( GuildWarStartTime ) );
	ar >> nWinPointDecl >> nWinPointAcpt >> nGetPointDecl >> nGetPointAcpt;
	
	//	GUILD_WAR_STR 'U3',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild 
	WAR_QUERYINFO wqi( "U3" );
	wqi.idWar	= idWar;
	wqi.idGuild	= idDecl;
	wqi.nWinPoint	= nWptDecl;
	wqi.f_idGuild	= idAcpt;
	wqi.f_nWinPoint	= nWptAcpt;
	switch( nType )
	{
	case WR_DECL_GN:
		wqi.iState	= '1';
		break;
	case WR_DECL_SR:
		wqi.iState	= '2';
		break;
	case WR_ACPT_GN:
		wqi.iState	= '3';
		break;
	case WR_ACPT_SR:
		wqi.iState	= '4';
		break;
	default:
		wqi.iState	= '9';
		break;
	}
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, _countof( szQuery ), wqi );
	pQuery->Exec( szQuery );

//	@im_idGuild,@im_idPlayer(f_idPlayer),@iserverindex,@im_nGuildGold(m_nCurExp),@im_nLevel(m_nGetExp), 
//	@im_GuildLv(m_nCurPenya),@iGuildPoint(m_nGetPenya),'',@iState,@im_Item(m_idWar),'',0,0,0,@is_date
//	LOG_GUILD_STR 'L5','000001', '000001','01',100,10,100,10,'','1',9,'',0,0,0,'20040717123012' @iState : 1 승 2
	LPCTSTR szState[2];

	switch( nType )
	{
	case WR_DECL_GN:
		szState[0]	= "1";
		szState[1]	= "5";
		break;
	case WR_DECL_SR:
		szState[0]	= "2";
		szState[1]	= "6";
		break;
	case WR_ACPT_GN:
		szState[0]	= "5";
		szState[1]	= "1";
		break;
	case WR_ACPT_SR:
		szState[0]	= "6";
		szState[1]	= "2";
		break;
	default:
		szState[0]	= "9";
		szState[1]	= "9";
		break;
	}

	GUILDLOG_QUERYINFO qi( "L5" );
	qi.idGuild = idDecl;
	qi.idPlayer = idAcpt;
	qi.nGuildGold = nWinPointDecl;
	qi.nLevel = nGetPointDecl;
	qi.nGuildLv = 0;
	qi.nGuildPxp = 0;
	qi.nItem = idWar;
	qi.pszTime = GuildWarStartTime;
	qi.pszState = szState[0];

	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	pQueryLog->Exec( szQuery );
	qi.idGuild = idAcpt;
	qi.idPlayer = idDecl;
	qi.nGuildGold = nWinPointAcpt;
	qi.nLevel = nGetPointAcpt;
	qi.nGuildLv = 0;
	qi.nGuildPxp = 0;
	qi.pszState = (char*)wqi.iState;
	qi.nItem = idWar;
	qi.pszTime = GuildWarStartTime;
	qi.pszState = szState[1];
	
	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	pQueryLog->Exec( szQuery );
}

void CDbManager::Surrender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "SURRENDER" ) );
	u_long idWar, idPlayer, idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idPlayer >> idGuild;

// GUILD_WAR_STR 'U1',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild,@im_idPlayer
	WAR_QUERYINFO wqi( "U1" );
	wqi.idGuild	= idGuild;
	wqi.idWar	= idWar;
	wqi.idPlayer	= idPlayer;
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, _countof( szQuery ), wqi );
	pQuery->Exec( szQuery );
}

void CDbManager::WarDead( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idWar, idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idGuild;

//	GUILD_WAR_STR 'U2',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild (m_idPlayer)
	WAR_QUERYINFO wqi( "U2" );
	wqi.idWar	= idWar;
	wqi.idGuild	= idGuild;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, _countof( szQuery ), wqi );
	pQuery->Exec( szQuery );
}

void CDbManager::WarMasterAbsent( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idWar, idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idWar >> idGuild;

//	GUILD_WAR_STR 'U4',@im_idGuild,@iserverindex,@im_idWar,@iState,@if_idGuild (m_idPlayer)
	WAR_QUERYINFO wqi( "U4" );
	wqi.idWar	= idWar;
	wqi.idGuild	= idGuild;
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	DBQryWar( szQuery, _countof( szQuery ), wqi );
	pQuery->Exec( szQuery );
}


void CDbManager::UpdateGuildRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr	ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	// 길드 랭크 정보를 받아온다.
	CGuildRank* pGuildRank = CGuildRank::Instance();

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	pGuildRank->GetRanking( pQuery, szQuery, _countof( szQuery ) );
	
	// 길드 랭크 정보가 Refresh 되었을때..
	// Core서버에게 정보를 보내준다.
	CDPTrans::GetInstance()->SendUpdateGuildRankFinish();
}


void CDbManager::UpdateGuildRankingDB( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr	ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	// 길드 랭크 정보를 받아온다.
	CGuildRank* pGuildRank = CGuildRank::Instance();

	char szQuery[MAX_QUERY_SIZE]	= { 0,};

	pGuildRank->RankingDBUpdate( pQuery, szQuery, _countof( szQuery ) );
	pGuildRank->GetRanking( pQuery, szQuery, _countof( szQuery ) );
	
	// 길드 랭크 정보가 Refresh 되었을때..
	// Core서버에게 정보를 보내준다.
	CDPTrans::GetInstance()->SendUpdateGuildRankFinish();	
}


// 쪽지를 ar에 기록한다.
void CDbManager::WriteTag(CAr &ar, int count, const TAG_ENTRY* tags)
{
	short nTagCount = static_cast< short >( count );
	ar << nTagCount;

	const TAG_ENTRY * pEntry;
	for( short i=0; i<nTagCount; ++i )
	{
		pEntry = &tags[i];

		ar << pEntry->idFrom;					// 보낸사람 
		ar << pEntry->dwDate;					// 보낸시각 
		ar.WriteString( pEntry->szString );		// 쪽지 내용 
	}
}

int CDbManager::SelectTag( CQuery* qry, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/, u_long uidPlayer, TAG_ENTRY* tags)
{
	// TAG_STR 'S1','000001','01' 
	char szQuery[MAX_QUERY_SIZE]	= { 0,};

	FLSPrintf( szQuery, _countof( szQuery ),
		"TAG_STR 'S1','%07d','%02d'",
		uidPlayer, g_appInfo.dwSys );
	
	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return 0;
	}

	int count = 0;			
	while( qry->Fetch() )
	{
		TAG_ENTRY *pEntry = &tags[count];

		pEntry->idFrom = qry->GetInt( "f_idPlayer" );	// 보낸사람 
		pEntry->dwDate = qry->GetInt( "CreateTime" );	// 보낸시각 
		qry->GetStr( "m_Message", pEntry->szString, _countof( pEntry->szString ) );	// 메세지 

		if( ++count >= MAX_TAGS )
			break;
	}
	return count;
}

void CDbManager::InsertTag( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idFrom;
	u_long idTo;
	char szString[256];
	arRead >> idFrom >> idTo;
	arRead.ReadString( szString, _countof( szString ) );

	FLTRACE_LOG( PROGRAM_NAME, _T( "%d %d %s" ), idFrom, idTo, szString );
	//TAG_STR 'A1','000001','01','000002','안녕하세요?'
	
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ),
		"{call TAG_STR('A1', '%07d', '%02d', '%07d', ?)}", idTo, g_appInfo.dwSys, idFrom);

	if( qry->BindParameterVarChar( SQL_PARAM_INPUT, 256, szString ) == FALSE )
	{
		return;
	}

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}

	if( qry->Fetch() )
	{
		int nError = qry->GetInt( "fError" );
		if (nError == 1)	
			g_dpCoreSrvr.SendTagResult(idFrom, 0);	// 쪽지가 20개를 초과했어요
		else
			g_dpCoreSrvr.SendTagResult(idFrom, 1);	// 성공 
	}
}

void CDbManager::RecommendEve( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	char szAccount[MAX_ACCOUNT];
	u_long idPlayer;
	LONG nLevel;
	BYTE nSex;
	int nValue;
	
	arRead.ReadString( szAccount, _countof( szAccount ) );
	arRead >> idPlayer >> nLevel >> nSex >> nValue;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "MAIL_STR_REALTIME 'I1', '%02d', %d, %d, '%07d', %d, '%s', %d", g_appInfo.dwSys, nSex, nValue, idPlayer, nLevel, szAccount, time(NULL) );

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}

	if( qry->Fetch() )
	{
	}
}

void CDbManager::SchoolReport( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "SCHOOL_REPORT" ) );
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	SCHOOL_ENTRY school[MAX_SCHOOL];
	ar.Read( &school, sizeof(SCHOOL_ENTRY) * MAX_SCHOOL );
/*
	[200407_1_SCHOOL_TBL]  저장하기
	 ex ) 
	 [200407_1_SCHOOL_STR] @iranking,@im_szGuild,@isurvivor
	 [200407_1_SCHOOL_STR] 1,'대원외국어고등학교',12
*/
	for( int i = 0; i < MAX_SCHOOL; i++ )
	{
		if( lstrlen( school[i].lpName ) > 0 )
		{
			char szQuery[MAX_QUERY_SIZE]	= { 0,};
			FLSPrintf( szQuery, _countof( szQuery ),
				"EVENT_FINAL_STR %d, '%s', %d",
				( i + 1 ), school[i].lpName, school[i].nSurvivor );
			if( FALSE == pQuery->Exec( szQuery ) )
				FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		}
	}
}

// szDst에 comma를 붙이고 16진수로 변환된 문자열을 붙인다.
void CDbManager::SetStrDBFormat( char* szDst, size_t cchDst, const char* szSrc )
{
	size_t nDstLen = strlen( szDst );
	if( nDstLen < cchDst-1 )
	{
		char* pCur = szDst + nDstLen;
		cchDst = cchDst - nDstLen;
		*pCur++ = ',';
		cchDst--;

		while( *szSrc && cchDst >= 3 )
		{
			FLSPrintf( pCur, cchDst, "%2X",  (BYTE)*szSrc );
			pCur += 2;
			cchDst -= 2;
			szSrc++;
		}
		*pCur = '\0';
	}
}

inline void CDbManager::DBQryGuild( char* szSql, size_t cchSql, const char* szType, u_long idPlayer, u_long idGuild, const char* szGuild )
{
	FLSPrintf( szSql, cchSql, "GUILD_STR '%s', '%07d', '%02d', '%06d', '%s'", szType, idPlayer, g_appInfo.dwSys, idGuild, szGuild );
}

void CDbManager::DBQryGuild( char* szSql, size_t cchSql, const GUILD_QUERYINFO& info )
{
	if( info.pszType[0] == 'U' && info.pszType[1] == '5' )
	{
		FLSPrintf( szSql, cchSql, "{ call GUILD_STR('%s',"
			"'%07d','%02d','%06d','%s',%d,%d,%d,%d,"
			"%d,%d,%d,%d,%d,%d,"
			"%d,?,%d) }", 
			info.pszType,
			info.idPlayer, g_appInfo.dwSys, info.idGuild, info.pszGuild,info.dwLv1, info.dwLv2, info.dwLv3, info.dwLv4,
			info.nLevel, info.nGuildGold, info.nGuildPxp, info.nWin, info.nLose, info.nSurrender,
			info.dwLogo, info.nClass );
		
	}
	else
	{
		FLSPrintf( szSql, cchSql, "GUILD_STR '%s',"
			"@im_idPlayer='%07d',@iserverindex='%02d',@im_idGuild='%06d',@im_szGuild='%s',@iLv_1=%d,@iLv_2=%d,@iLv_3=%d,@iLv_4=%d,"
			"@im_nLevel=%d,@im_nGuildGold=%d,@im_nGuildPxp=%d,@im_nWin=%d,@im_nLose=%d,@im_nSurrender=%d,"
			"@im_dwLogo=%d,@im_szNotice='%s',@im_nClass=%d", 
			info.pszType,
			info.idPlayer, g_appInfo.dwSys, info.idGuild, info.pszGuild,info.dwLv1, info.dwLv2, info.dwLv3, info.dwLv4,
			info.nLevel, info.nGuildGold, info.nGuildPxp, info.nWin, info.nLose, info.nSurrender,
			info.dwLogo, info.pszNotice, info.nClass
			);
		
	}


	FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), szSql);
}

void CDbManager::DBQryVote( char* szSql, size_t cchSql, const VOTE_QUERYINFO& info )
{
	FLSPrintf( szSql, cchSql, "GUILD_VOTE_STR '%s',@iserverindex='%02d',@im_idGuild='%06d',@im_idVote=%d,"
		            "@im_szTitle='%s',@im_szQuestion='%s',@im_szString1='%s',@im_szString2='%s',@im_szString3='%s',@im_szString4='%s',@im_cbCount=%d",
			info.pszType, g_appInfo.dwSys, info.idGuild, info.idVote, 
			info.pszTitle, info.pszQuestion, 
			info.pszSelections[0], info.pszSelections[1], info.pszSelections[2], info.pszSelections[3],
			info.cbSelection );
	FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), szSql );
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

bool	CDbManager::DbQryMail( CQuery* pQuery, const PT_MAIL_STR_INFO ptMail )
{
	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD BASE_PARAMETER	= 69;

	const DWORD ADD_PARAMETER	= 24;

	const DWORD MAX_PARAMETER = _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ? BASE_PARAMETER + ADD_PARAMETER : BASE_PARAMETER;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "MAIL_STR" ), MAX_PARAMETER );


	TCHAR szServerIndex[3] = { 0, };
	TCHAR szReceivePlayerId[8] = { 0, };
	TCHAR szSendPlayerId[8] = { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szReceivePlayerId, _countof( szReceivePlayerId ), "%07d", ptMail->dwReceivePlayerId );
	FLSPrintf( szSendPlayerId, _countof( szSendPlayerId ), "%07d", ptMail->dwSendPlayerId );


	// 1
	pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( ptMail->szGu ) - 1, ptMail->szGu );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwMailId );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szReceivePlayerId ) - 1, szReceivePlayerId );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szSendPlayerId ) - 1, szSendPlayerId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nGold );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->tmCreate );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nReadDay );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, _countof( ptMail->szMailTitle ) - 1,	ptMail->szMailTitle );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, _countof( ptMail->szMailText ) - 1, ptMail->szMailText );


	// 11
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwItemId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nItemNum );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nRepairNumber );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nHitPoint );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nRepair );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nMaterial );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nFlag );
//	pQuery->BindParameterInt64( SQL_PARAM_INPUT, &ptMail->dwSerialNumber );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwSerialNumber );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nOPtion );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nItemResist );


	// 21
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nResistAbilityOption );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwGuildId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwResistSMItemId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->bCharged );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwKeepTime );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT, &ptMail->n64RandomOptItemId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPiercingSize );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId1 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId2 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId3 );


	// 31
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId4 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->bIsPet );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetKind );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetLevel );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPetExp );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPetEnergy );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPetLife );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetAvailLevel_D );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetAvailLevel_C );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetAvailLevel_B );


	// 41
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetAvailLevel_A );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nPetAvailLevel_S );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId5 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId6 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId7 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId8 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId9 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId10 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId11 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId12 );


	// 51
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId13 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId14 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwPiercedItemId15 );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nAddPiercingSize );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, _countof( ptMail->szPetName ) - 1, ptMail->szPetName );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT, &ptMail->dwManagedSerialNumber );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwCouplePlayerId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nLevelDown );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwRandomOptionOriginId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwRandomOptionExtSize );

	// 61
	pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwRandomOptionExtFlag );
	for( DWORD dwNth = 0; dwNth < MAX_RANDOMOPTION_DB_COLUMN; ++dwNth )
	{
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->arrRandomOptionExtensionDstID[dwNth] );
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->arrRandomOptionExtensionAdjValue[dwNth] );
	}

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwLooksChangeItemID );
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->bIsCombined );
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwCombinedAddDamage );
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->dwCombinedRandomOptionOriginID );
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nCombinedGeneralPiercingSize );
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->arrCombinedGeneralPiercingID[ Nth ] );
		}
		pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->nCombinedRandomOptionExtensionSize );
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->arrCombinedRandomOptionExtensionDstID[ Nth ] );
			pQuery->BindParameterInt( SQL_PARAM_INPUT, &ptMail->arrCombinedRandomOptionExtensionAdjValue[ Nth ] );
		}
	}

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		WriteLogMail( ptMail );
		return false;
	}

	return true;
}

void CDbManager::WriteLogMail( const PT_MAIL_STR_INFO ptMail )
{
	TCHAR szLog[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szLog, _countof( szLog ), "MAIL_STR @iGu = '%s'", ptMail->szGu );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nMail = %d", szLog, ptMail->dwMailId );
	FLSPrintf( szLog, _countof( szLog ), "%s, serverindex = '%02d'", szLog, g_appInfo.dwSys );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idReceiver = '%07d'", szLog, ptMail->dwReceivePlayerId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idSender = '%07d'", szLog, ptMail->dwSendPlayerId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nGold = %d", szLog, ptMail->nGold );
	FLSPrintf( szLog, _countof( szLog ), "%s, @tmCreate = %d", szLog, ptMail->tmCreate );
	FLSPrintf( szLog, _countof( szLog ), "%s, @byRead = %d", szLog, ptMail->nReadDay );
	FLSPrintf( szLog, _countof( szLog ), "%s, @szTitle = '%s'", szLog, ptMail->szMailTitle );
	FLSPrintf( szLog, _countof( szLog ), "%s, @szText = '%s'", szLog, ptMail->szMailText );

	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId = %d", szLog, ptMail->dwItemId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nItemNum = %d", szLog, ptMail->nItemNum );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nRepairNumber = %d", szLog, ptMail->nRepairNumber );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nHitPoint = %d", szLog, ptMail->nHitPoint );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nMaxHitPoint = %d", szLog, ptMail->nRepair );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nMaterial = %d", szLog, ptMail->nMaterial );
	FLSPrintf( szLog, _countof( szLog ), "%s, @byFlag = %d", szLog, ptMail->nFlag );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwSerialNumber = %d", szLog, ptMail->dwSerialNumber );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nOption = %d", szLog, ptMail->nOPtion );
	FLSPrintf( szLog, _countof( szLog ), "%s, @bItemResist = %d", szLog, ptMail->nItemResist );

	FLSPrintf( szLog, _countof( szLog ), "%s, @nResistAbilityOption = %d", szLog, ptMail->nResistAbilityOption );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idGuild = %d", szLog, ptMail->dwGuildId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nResistSMItemId = %d", szLog, ptMail->dwResistSMItemId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @bCharged = %d", szLog, ptMail->bCharged );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwKeepTime = %d", szLog, ptMail->dwKeepTime );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptItemId = %I64d", szLog, ptMail->n64RandomOptItemId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nPiercedSize = %d", szLog, ptMail->nPiercingSize );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId1 = %d", szLog, ptMail->dwPiercedItemId1 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId2 = %d", szLog, ptMail->dwPiercedItemId2 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId3 = %d", szLog, ptMail->dwPiercedItemId3 );

	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId4 = %d", szLog, ptMail->dwPiercedItemId4 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @bPet = %d", szLog, ptMail->bIsPet );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nKind = %d", szLog, ptMail->nPetKind );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nLevel = %d", szLog, ptMail->nPetLevel );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwExp = %d", szLog, ptMail->dwPetExp );
	FLSPrintf( szLog, _countof( szLog ), "%s, @wEnergy = %d", szLog, ptMail->dwPetEnergy );
	FLSPrintf( szLog, _countof( szLog ), "%s, @wLife = %d", szLog, ptMail->dwPetLife );
	FLSPrintf( szLog, _countof( szLog ), "%s, @anAvailLevel_D = %d", szLog, ptMail->nPetAvailLevel_D );
	FLSPrintf( szLog, _countof( szLog ), "%s, @anAvailLevel_C = %d", szLog, ptMail->nPetAvailLevel_C );
	FLSPrintf( szLog, _countof( szLog ), "%s, @anAvailLevel_B = %d", szLog, ptMail->nPetAvailLevel_B );

	FLSPrintf( szLog, _countof( szLog ), "%s, @anAvailLevel_A = %d", szLog, ptMail->nPetAvailLevel_A );
	FLSPrintf( szLog, _countof( szLog ), "%s, @anAvailLevel_S = %d", szLog, ptMail->nPetAvailLevel_S );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId5 = %d", szLog, ptMail->dwPiercedItemId5 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId6 = %d", szLog, ptMail->dwPiercedItemId6 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId7 = %d", szLog, ptMail->dwPiercedItemId7 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId8 = %d", szLog, ptMail->dwPiercedItemId8 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId9 = %d", szLog, ptMail->dwPiercedItemId9 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId10 = %d", szLog, ptMail->dwPiercedItemId10 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId11 = %d", szLog, ptMail->dwPiercedItemId11 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId12 = %d", szLog, ptMail->dwPiercedItemId12 );

	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId13 = %d", szLog, ptMail->dwPiercedItemId13 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId14 = %d", szLog, ptMail->dwPiercedItemId14 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @dwItemId15 = %d", szLog, ptMail->dwPiercedItemId15 );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nPiercedSize2 = %d", szLog, ptMail->nAddPiercingSize );
	FLSPrintf( szLog, _countof( szLog ), "%s, @szPetName = %s", szLog, ptMail->szPetName );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nSerialNumberId = %d", szLog, ptMail->dwManagedSerialNumber );
	FLSPrintf( szLog, _countof( szLog ), "%s, @idCouplePlayer = %d", szLog, ptMail->dwCouplePlayerId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nLevelDown = %d", szLog, ptMail->nLevelDown );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionOriginId = %d", szLog, ptMail->dwRandomOptionOriginId );
	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtSize = %d", szLog, ptMail->dwRandomOptionExtSize );

	FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtFlag = %d", szLog, ptMail->dwRandomOptionExtFlag );
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtDst%d = %d", szLog, Nth + 1, ptMail->arrRandomOptionExtensionDstID[ Nth ] );
		FLSPrintf( szLog, _countof( szLog ), "%s, @nRandomOptionExtAdj%d = %d", szLog, Nth + 1, ptMail->arrRandomOptionExtensionAdjValue[ Nth ] );
	}

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		FLSPrintf( szLog, _countof( szLog ), "%s, @dwLooksChangeItemID = %d", szLog, ptMail->dwLooksChangeItemID );
		FLSPrintf( szLog, _countof( szLog ), "%s, @bIsCombined = %d", szLog, ptMail->bIsCombined );
		FLSPrintf( szLog, _countof( szLog ), "%s, @dwCombinedAddDamage = %d", szLog, ptMail->dwCombinedAddDamage );
		FLSPrintf( szLog, _countof( szLog ), "%s, @wCombinedRandomOptionOriginID = %d", szLog, ptMail->dwCombinedRandomOptionOriginID );
		FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedGeneralPiercingSize = %d", szLog, ptMail->nCombinedGeneralPiercingSize );
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			FLSPrintf( szLog, _countof( szLog ), "%s, @dwCombinedGeneralPiercingID%d = %d", szLog, Nth + 1, ptMail->arrCombinedGeneralPiercingID[ Nth ] );
		}
		FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedRandomOptionExtensionSize = %d", szLog, ptMail->nCombinedRandomOptionExtensionSize );
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedRandomOptionExtDst%d = %d", szLog, Nth + 1, ptMail->arrCombinedRandomOptionExtensionDstID[ Nth ] );
			FLSPrintf( szLog, _countof( szLog ), "%s, @nCombinedRandomOptionExtAdj%d = %d", szLog, Nth + 1, ptMail->arrCombinedRandomOptionExtensionAdjValue[ Nth ] );
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FAILED(%s) ]" ), szLog );
}

void	CDbManager::WriteFileLogCharacter( const TCHAR* _pFileName, const TCHAR* _pContext, const PT_CHARACTER_STR_INFO pCharacterStrInfo )
{
	if( _pFileName == NULL || _pContext == NULL || pCharacterStrInfo == NULL )
	{
		return;
	}

	static bool PrintTitle	= false;

	const TCHAR Title[] = "m_idPlayer"
		",serverindex"
		",account"
		",m_szName"
		",playerslot"
		",dwWorldID"
		",m_dwIndex"
		",m_vPos_x"
		",m_vPos_y"
		",m_vPos_z"
		",m_szCharacterKey"
		",m_dwSkinSet"
		",m_dwHairMesh"
		",m_dwHairColor"
		",m_dwHeadMesh"
		",m_dwSex"
		",m_vScale_x"
		",m_dwMotion"
		",m_fAngle"
		",m_nHitPoint"
		",m_nManaPoint"
		",m_nFatiguePoint"
		",m_dwRideItemIdx"
		",m_dwGold"
		",m_nJob"
		",m_pActMover"
		",m_nStr"
		",m_nSta"
		",m_nDex"
		",m_nInt"
		",m_nLevel"
		",m_nExp1"
		",m_nExp2"
		",m_aJobSkill"
		",m_aLicenseSkill"
		",m_aJobLv"
		",m_dwExpertLv"
		",m_idMarkingWorld"
		",m_vMarkingPos_x"
		",m_vMarkingPos_y"
		",m_vMarkingPos_z"
		",m_nRemainGP"
		",m_nRemainLP"
		",m_nFlightLv"
		",m_nFxp"
		",m_nTxp"
		",m_lpQuestCntArray"
		",m_chAuthority"
		",m_dwMode"
		",m_idparty"
		",m_idMuerderer"
		",m_nFame"
		",m_nDeathExp"
		",m_nDeathLevel"
		",m_dwFlyTime"
		",m_nMessengerState"
		",TotalPlayTime"
		",m_nPKValue"
		",m_dwPKPropensity"
		",m_dwPKExp"
		",m_Card"
		",m_Index_Card"
		",m_ObjIndex_Card"
		",m_Cube"
		",m_Index_Cube"
		",m_ObjIndex_Cube"
		",m_Inventory"
		",m_apIndex"
		",m_adwEquipment"
		",m_dwObjIndex"
		",m_aSlotApplet"
		",m_aSlotItem"
		",m_aSlotQueue"
		",m_SkillBar"
		",m_Bank"
		",m_apIndex_Bank"
		",m_dwObjIndex_Bank"
		",m_dwGoldBank"
		",m_nFuel"
		",m_tmAccFuel"
		",m_dwSMTime"
		",SkillInfluence"
		",m_dwSkillPoint"
		",m_aCompleteQuest"
		",m_extInventory"
		",m_extBank"
		",m_InventoryPiercing"
		",m_BankPiercing"
		",m_dwReturnWorldID"
		",m_vReturnPos_x"
		",m_vReturnPos_y"
		",m_vReturnPos_z"
		",m_SkillPoint"
		",m_SkillLv"
		",m_SkillExp"
		",dwEventFlag"
		",dwEventTime"
		",dwEventElapsed"
		",m_nAngelExp"
		",m_nAngelLevel"
		",szInventoryPet"
		",szBankPet"
		",m_dwPetId"
		",m_nExpLog"
		",m_nAngelExpLog"
		",m_nCoupon"
		",m_nHonor"
		",m_nLayer"
		",m_aCheckedQuest"
		",m_nCampusPoint"
		",m_idCampus"
		",m_DiceStr"
		",m_DiceSta"
		",m_DiceDex"
		",m_DiceInt";

	if( PrintTitle == false )
	{
		FLERROR_LOG( _pFileName, _T( "%s,%s" ), _pContext, Title );
		
		PrintTitle	= true;
	}

	TCHAR Log[ MAX_QUERY_SIZE ]	= { 0, };

	FLSPrintf( Log, _countof( Log ), "%07d",		pCharacterStrInfo->dwPlayerId );
	FLSPrintf( Log, _countof( Log ), "%s,%02d",		Log, g_appInfo.dwSys );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szAccount );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szPlayerName );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nPlayerSlot );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwWorldId );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwIndex );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fPos_x );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fPos_y );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fPos_z );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szCharacterKey );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwSkinSet );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwHairMesh );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwHairColor );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwHeadMesh );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwSex );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fScale_x );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwMotion );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fAngle );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nHitPoint );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nManaPoint );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nFatiguePoint );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwRideItemId );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwGold );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nJob );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szActMover );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nStr );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nSta );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nDex );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nInt );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nLevel );
	FLSPrintf( Log, _countof( Log ), "%s,%I64d",	Log, pCharacterStrInfo->n64Exp1 );
	FLSPrintf( Log, _countof( Log ), "%s,%I64d",	Log, pCharacterStrInfo->n64Exp2 );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szJobSkill );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szLicenseSkill );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szJobLv );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwExpertLv );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwMarkingWorldId );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fMarkingPos_x );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fMarkingPos_y );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fMarkingPos_z );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nRemainGP );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nRemainLP );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nTutorialState );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nFxp );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nTxp );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szQuestCntArray );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szAuthority );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwMode );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nPartyId );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwMuerdererId );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nFame );
	FLSPrintf( Log, _countof( Log ), "%s,%I64d",	Log, pCharacterStrInfo->n64DeathExp );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nDeathLevel );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwFlyTime );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwMessengerState );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nTotalPlayTime );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nPKValue );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwPKPropensity );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwPKExp );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szCard );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szIndex_Card );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szObjIndex_Card );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szCube );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szIndex_Cube );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szObjIndex_Cube );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szInventory );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szIndex_Inventory );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szEquipment );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szObjIndex_Inventory );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szSlotApplet );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szSlotItem );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szSlotQueue );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nActionPoint );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szBank );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szIndex_Bank );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szObjIndex_Bank );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwGoldBank );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nFuel );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->tmAccFuel );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szSMTime );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szSkillInfluence );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwSkillPoint );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szCompleteQuest );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szExtInven );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szExtBank );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szPirecingInven );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szPirecingBank );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwReturnWorldId );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fReturnPos_x );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fReturnPos_y );
	FLSPrintf( Log, _countof( Log ), "%s,%f",		Log, pCharacterStrInfo->fReturnPos_z );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nSkillPoint );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nSkillLevel );
	FLSPrintf( Log, _countof( Log ), "%s,%I64d",	Log, pCharacterStrInfo->n64SkillExp );
	FLSPrintf( Log, _countof( Log ), "%s,%I64d",	Log, pCharacterStrInfo->n64EventFlag );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwEventTime );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwEventElapsed );
	FLSPrintf( Log, _countof( Log ), "%s,%I64d",	Log, pCharacterStrInfo->n64AngelExp );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nAngelLevel );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szInventoryPet );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szBankPet );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwPetId );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nExpLog );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nAngelExpLog );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nCoupon );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nHonor );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nLayer );
	FLSPrintf( Log, _countof( Log ), "%s,'%s'",		Log, pCharacterStrInfo->szCheckedQuest );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->nCampusPoint );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dwCampusId );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dcStr );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dcSta );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dcDex );
	FLSPrintf( Log, _countof( Log ), "%s,%d",		Log, pCharacterStrInfo->dcInt );
	
	FLERROR_LOG( _pFileName, _T( "%s,%s" ), _pContext, Log );
}

// END110119
//////////////////////////////////////////////////////////////////////////

//void CDbManager::MakeQueryAddMail( char * szSql, const size_t cchSql, const CMail * pMail, const u_long idReceiver )
//{
//	FLItemElem item;
//	CPet pet;
//	BOOL bPet	= FALSE;
//	if( pMail->m_pItemElem )
//	{
//		item	= *pMail->m_pItemElem;
//		if( pMail->m_pItemElem->m_pPet )
//		{
//			pet		= *pMail->m_pItemElem->m_pPet;
//			bPet	= TRUE;
//		}
//	}
//	else
//		item.m_nItemNum = 0;
//
//	if( item.GetProp() && item.GetProp()->IsUltimate() )
//	{
//		FLSPrintf( szSql, cchSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
//			"%d, %d, %d, %d, %d, %d, %d, %d,"
//			"%d, %d, %d, %d, %d, %d, %d,"
//			"%I64d,"
//			"%d, %d, %d, %d, %d"
//			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
//			",%d"
//			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
//			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
//			",%d"	// 추가 피어싱 사이즈
//			")}",
//
//			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
//			item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
//			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
//			item.GetRandomOptItemId(),
//			item.GetUltimatePiercingSize(), item.GetUltimatePiercingItemID( 0 ), item.GetUltimatePiercingItemID( 1 ), item.GetUltimatePiercingItemID( 2 ), item.GetUltimatePiercingItemID( 3 ),
//			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
//			pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
//			, item.GetUltimatePiercingItemID( 4 )
//			, item.GetGeneralPiercingItemID( 0 ), item.GetGeneralPiercingItemID( 1 ), item.GetGeneralPiercingItemID( 2 ), item.GetGeneralPiercingItemID( 3 ),  item.GetGeneralPiercingItemID( 4 )
//			, item.GetGeneralPiercingItemID( 5 ), item.GetGeneralPiercingItemID( 6 ), item.GetGeneralPiercingItemID( 7 ), item.GetGeneralPiercingItemID( 8 ),  item.GetGeneralPiercingItemID( 9 )
//			, item.GetGeneralPiercingSize()
//			);
//	}
//	else
//	{
//		FLSPrintf( szSql, cchSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
//			"%d, %d, %d, %d, %d, %d, %d, %d,"
//			"%d, %d, %d, %d, %d, %d, %d,"
//			"%I64d,"
//			"%d, %d, %d, %d, %d"
//			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
//			",%d"
//			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
//			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
//			",%d"	// 추가 피어싱 사이즈
//			")}",
//
//			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
//			item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
//			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
//			item.GetRandomOptItemId(),
//			item.GetGeneralPiercingSize(), item.GetGeneralPiercingItemID( 0 ), item.GetGeneralPiercingItemID( 1 ), item.GetGeneralPiercingItemID( 2 ), item.GetGeneralPiercingItemID( 3 ),
//			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
//			pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
//			, item.GetGeneralPiercingItemID( 4 )
//			, item.GetGeneralPiercingItemID( 5 ), item.GetGeneralPiercingItemID( 6 ), item.GetGeneralPiercingItemID( 7 ), item.GetGeneralPiercingItemID( 8 ), item.GetGeneralPiercingItemID( 9 )
//			, 0, 0, 0, 0, 0
//			, 0
//			);
//	}
//}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

// void CDbManager::MakeQueryAddMail( char * szSql, const size_t cchSql, const CMail * pMail, const u_long idReceiver )
// {
// 	FLSPrintf( szSql, cchSql, "{call MAIL_STR(" );
// 	const size_t size		= FLStrlen( szSql, cchSql );
// 
// 	MakeQueryMailParam( szSql + size, cchSql - size, pMail, idReceiver );
// }

// END110119
//////////////////////////////////////////////////////////////////////////

void CDbManager::MakeQuery_CooperativeContributionsRewardItemMail( const u_long idPlayer, const DWORD dwContributionID, const FLTm tExpire
														 ,  char * o_szSql, const size_t cchSql, const CMail * pMail, const u_long /*idReceiver*/ )
{
	if( pMail == NULL || pMail->m_pItemElem == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID ARGUMENT" ) );
		return;
	}

	FLSPrintf( o_szSql, cchSql, "{call test_uspCooperativeContributions_TryRewardItemByMail( '%07d', %u, '%s', '%s', %u, " 
					, idPlayer, dwContributionID, FLTmToString( tExpire, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), "ITEM", pMail->m_pItemElem->m_dwItemId );
	const size_t size		= FLStrlen( o_szSql, cchSql );
	UNREFERENCED_PARAMETER( size );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

// 	MakeQueryMailParam( o_szSql + size, cchSql - size, pMail, idReceiver );

	// END110119
	//////////////////////////////////////////////////////////////////////////
}


//////////////////////////////////////////////////////////////////////////
// BEGIN110119

// void CDbManager::MakeQueryMailParam( char * szSql, const size_t cchSql, const CMail * pMail, const u_long idReceiver )
// {
// 	FLItemElem item;
// 	CPet pet;
// 	BOOL bPet	= FALSE;
// 	if( pMail->m_pItemElem )
// 	{
// 		item	= *pMail->m_pItemElem;
// 		if( pMail->m_pItemElem->m_pPet )
// 		{
// 			pet		= *pMail->m_pItemElem->m_pPet;
// 			bPet	= TRUE;
// 		}
// 	}
// 	else
// 		item.m_nItemNum = 0;
// 
// 	if( item.GetProp() && item.GetProp()->IsUltimate() )
// 	{
// 		FLSPrintf( szSql, cchSql, "'A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
// 			"%d, %d, %d, %d, %d, %d, %d, %d,"
// 			"%d, %d, %d, %d, %d, %d, %d,"
// 			"%I64d,"
// 			"%d, %d, %d, %d, %d"
// 			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
// 			",%d"
// 			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
// 			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
// 			",%d"	// 추가 피어싱 사이즈
// 			")}",
// 
// 			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
// 			item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
// 			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
// 			item.GetRandomOptItemId(),
// 			item.GetUltimatePiercingSize(), item.GetUltimatePiercingItemID( 0 ), item.GetUltimatePiercingItemID( 1 ), item.GetUltimatePiercingItemID( 2 ), item.GetUltimatePiercingItemID( 3 ),
// 			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
// 			pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
// 			, item.GetUltimatePiercingItemID( 4 )
// 			, item.GetGeneralPiercingItemID( 0 ), item.GetGeneralPiercingItemID( 1 ), item.GetGeneralPiercingItemID( 2 ), item.GetGeneralPiercingItemID( 3 ),  item.GetGeneralPiercingItemID( 4 )
// 			, item.GetGeneralPiercingItemID( 5 ), item.GetGeneralPiercingItemID( 6 ), item.GetGeneralPiercingItemID( 7 ), item.GetGeneralPiercingItemID( 8 ),  item.GetGeneralPiercingItemID( 9 )
// 			, item.GetGeneralPiercingSize()
// 			);
// 	}
// 	else
// 	{
// 		FLSPrintf( szSql, cchSql, "'A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
// 			"%d, %d, %d, %d, %d, %d, %d, %d,"
// 			"%d, %d, %d, %d, %d, %d, %d,"
// 			"%I64d,"
// 			"%d, %d, %d, %d, %d"
// 			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
// 			",%d"
// 			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
// 			",%d, %d, %d, %d, %d"	// 추가 피어싱 아이템
// 			",%d"	// 추가 피어싱 사이즈
// 			")}",
// 
// 			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
// 			item.m_dwItemId, item.m_nItemNum, item.m_nRepairNumber, item.m_nHitPoint, item.m_nRepair, 0, item.m_byFlag, item.GetSerialNumber(),
// 			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, item.m_idGuild, item.m_nResistSMItemId, item.m_bCharged, item.m_dwKeepTime,
// 			item.GetRandomOptItemId(),
// 			item.GetGeneralPiercingSize(), item.GetGeneralPiercingItemID( 0 ), item.GetGeneralPiercingItemID( 1 ), item.GetGeneralPiercingItemID( 2 ), item.GetGeneralPiercingItemID( 3 ),
// 			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
// 			pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
// 			, item.GetGeneralPiercingItemID( 4 )
// 			, item.GetGeneralPiercingItemID( 5 ), item.GetGeneralPiercingItemID( 6 ), item.GetGeneralPiercingItemID( 7 ), item.GetGeneralPiercingItemID( 8 ), item.GetGeneralPiercingItemID( 9 )
// 			, 0, 0, 0, 0, 0
// 			, 0
// 			);
// 	}
// }

// END110119
//////////////////////////////////////////////////////////////////////////

void CDbManager::DBQryGuildLog( char* szSql, size_t cchSql, const GUILDLOG_QUERYINFO& info )
{
	FLSPrintf( szSql, cchSql, "LOG_GUILD_STR '%s', @im_idGuild='%06d', @im_idPlayer='%07d', @iserverindex='%02d',"
		"@im_nGuildGold=%d, @im_nLevel=%d, @im_GuildLv=%d, @iGuildPoint=%d,"
		"@iDo_m_idPlayer='%07d', @iState='%s', @im_Item=%d, @im_GuildBank='%s',"
		"@im_nAbilityOption=%d, @iItem_count=%d, @iItem_UniqueNo=%d, @is_date='%s'",
		info.pszType, info.idGuild, info.idPlayer, g_appInfo.dwSys,
		info.nGuildGold, info.nLevel, info.nGuildLv, info.nGuildPxp,
		info.idDoPlayer, info.pszState, info.nItem, info.pszGuildBank,
		info.nAbilityOption, info.nItem_count, info.iUniqueNo, info.pszTime );

	if( strlen(szSql) > 8012 )
		FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), szSql);
}

//////////////////////////////////////////////////////////////////////////
void	CDbManager::MakeLogGuildItem( CItemContainer<FLItemElem>* pGuildBank, TCHAR* pszItemLog, size_t cchItemLog )
{
	

	for( DWORD dwObjId = 0; dwObjId < pGuildBank->GetMax(); ++dwObjId )
	{
		TCHAR szOneItem[512] = { 0 };
		FLItemElem* pItemElem = pGuildBank->GetAtId( dwObjId );

		if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE )
		{
			if( pItemElem->m_dwItemId == ITEM_INDEX( 26476, II_SYS_SYS_SCR_SEALCHARACTER ) )
			{
				FLSPrintf( szOneItem, _countof( szOneItem ), "%d,%d,%d,%d,,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",	//17
					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
					0, 0, 
					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
					0, pItemElem->m_byFlag,
					pItemElem->GetSerialNumber(), pItemElem->GetAbilityOption(), 
					pItemElem->m_byItemResist, pItemElem->m_nResistAbilityOption,
					pItemElem->m_idGuild,
					pItemElem->m_nResistSMItemId
					);
			}
			else
			{
				FLSPrintf( szOneItem, _countof( szOneItem ), "%d,%d,%d,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",	//17
					pItemElem->m_dwObjId, pItemElem->m_dwItemId,
					0, 0, pItemElem->m_szItemText,
					pItemElem->m_nItemNum, pItemElem->m_nRepairNumber,
					pItemElem->m_nHitPoint, pItemElem->m_nRepair,
					0, pItemElem->m_byFlag,
					pItemElem->GetSerialNumber(), pItemElem->GetAbilityOption(), 
					pItemElem->m_byItemResist, pItemElem->m_nResistAbilityOption,
					pItemElem->m_idGuild,
					pItemElem->m_nResistSMItemId
					);
			}
		}

		FLStrncat( pszItemLog, cchItemLog, szOneItem, _countof( szOneItem ) );
	}

	FLStrncat( pszItemLog, cchItemLog, NullStr, _countof( NullStr ) );
}
//////////////////////////////////////////////////////////////////////////

void CDbManager::DBQryWar( char* szSql, size_t cchSql, const WAR_QUERYINFO & info )
{
	FLSPrintf( szSql, cchSql, "GUILD_WAR_STR '%s', @im_idGuild='%06d', @iserverindex='%02d', @im_idWar='%06d', @iState='%c', @if_idGuild='%06d', @im_idPlayer='%07d', @im_nWinPoint = %d, @if_nWinPoint = %d",
		info.pszType, info.idGuild, g_appInfo.dwSys, info.idWar, info.iState, info.f_idGuild, info.idPlayer, info.nWinPoint, info.f_nWinPoint );
}


void CDbManager::QueryGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_QUERYGUILDQUEST" ) );
//	GUILD_QUEST_STR 'S1', @im_idGuild,@iserverindex
	char	pszSql[128] = { 0, };
	FLSPrintf( pszSql, _countof( pszSql ), "GUILD_QUEST_STR 'S1', @im_idGuild = '', @iserverindex = '%02d'", g_appInfo.dwSys );
	if( !pQuery->Exec( pszSql ) )
	{
		return;
	}

	int nCount	= 0;
	u_long idGuild	= 0, idCurr = 0;
	GUILDQUEST pQuest[MAX_GUILD_QUEST];
	BYTE nQuestSize = 0;

	BEFORESENDDUAL( ar, PACKETTYPE_QUERYGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN );
	ptrdiff_t nCountTag = ar.ReserveSpace( sizeof( nCount ) );

	while( pQuery->Fetch() )
	{
		idCurr	= (u_long)pQuery->GetInt( "m_idGuild" );
		if( idCurr != idGuild )
		{
			if( idGuild != 0 )
			{
				ar << idGuild << nQuestSize;
				ar.Write( (void*)pQuest, sizeof(GUILDQUEST) * nQuestSize );
				nCount++;
			}
			nQuestSize	= 0;
			idGuild		= idCurr;
		}
		pQuest[nQuestSize].nId	= pQuery->GetInt( "n_Id" );
		pQuest[nQuestSize].nState	= pQuery->GetInt( "nState" );
		nQuestSize++;
	}

	if( idGuild != 0 )
	{
		ar << idGuild << nQuestSize;
		ar.Write( (void*)pQuest, sizeof(GUILDQUEST) * nQuestSize );
		nCount++;
	}

	ar.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );
	SEND( ar, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid );
}

void CDbManager::InsertGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_SETGUILDQUEST" ) );
//	GUILD_QUEST_STR 'A1', @im_idGuild,@iserverindex,@in_Id
//	GUILD_QUEST_STR 'A1', '000001','01',0
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idGuild;
	int nId;
	ar >> idGuild >> nId;

	char	pszSql[128] = { 0, };
	FLSPrintf( pszSql, _countof( pszSql ), "GUILD_QUEST_STR 'A1', @im_idGuild = '%06d', @iserverindex = '%02d', @in_Id = %d",
		idGuild, g_appInfo.dwSys, nId );

	if( !pQuery->Exec( pszSql ) )
	{
		return;
	}
}

void CDbManager::UpdateGuildQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "PACKETTYPE_UPDATEGUILDQUEST" ) );
//	GUILD_QUEST_STR 'U1', @im_idGuild,@iserverindex,@in_Id,@in_State
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idGuild;
	int nId, nState;
	ar >> idGuild >> nId >> nState;
	
	char	pszSql[128] = { 0, };
	FLSPrintf( pszSql, _countof( pszSql ), "GUILD_QUEST_STR 'U1', @im_idGuild = '%06d', @iserverindex = '%02d', @in_Id = %d, @in_State = %d",
		idGuild, g_appInfo.dwSys, nId, nState );

	if( !pQuery->Exec( pszSql ) )
	{
		return;
	}
}


void CDbManager::GuildThread( void )
{
	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( FALSE == pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		return;
	}

	CQuery* pQueryLog	= new CQuery;
#ifdef __DbStuff
	if (!pQueryLog->Connect(CQuery::SqlDatabase::LOGGING))
#else
	if( FALSE == pQueryLog->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG ) )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LogDB Connect Failed" ) );
		SAFE_DELETE( pQuery );
		SAFE_DELETE( pQueryLog );
		return;
	}

	/*CQuery* pQueryRanking = new CQuery;
#if defined(__DbStuff)
	if (!pQueryRanking->Connect(CQuery::SqlDatabase::RANKING))
#else
	if (!pQueryRanking->Connect(3, "RANKING_DBF", DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG))
#endif
	{
		FLERROR_LOG(PROGRAM_NAME, _T("RANKINGDBF Connect Failed"));
		SAFE_DELETE(pQuery);
		SAFE_DELETE(pQueryLog);
		SAFE_DELETE(pQueryRanking);
		return;
	}*/

	SetEvent( s_hHandle );

	BOOL bReturnValue	= FALSE;
	DWORD dwBytesTransferred	= 0;
	ULONG_PTR dwCompletionKey		= 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus	= NULL;
	
	while( true )
	{
		bReturnValue	= GetQueuedCompletionStatus( m_hIOCPGuild, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE );
		
		if( FALSE == bReturnValue )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "GetQueuedCompletionStatus return FALSE. Error:[%d]" ), ::GetLastError() );
			SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
			continue;
		}

		if( dwBytesTransferred == 0 )
		{
			SAFE_DELETE( pQuery );
			SAFE_DELETE( pQueryLog );
			SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
			return;
		}

		if( dwCompletionKey != 0 && lpDbOverlappedPlus == NULL )
		{
			CDPTrans::GetInstance()->SendAllGuildData( (DPID)dwCompletionKey );
			CDPTrans::GetInstance()->SendAllGuildCombat( (DPID)dwCompletionKey );
			continue;
		}

		switch( lpDbOverlappedPlus->nQueryMode )
		{
			case CREATE_GUILD:
				CreateGuild( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case DESTROY_GUILD:
				DestroyGuild( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case ADD_GUILD_MEMBER:
				AddGuildMember( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case REMOVE_GUILD_MEMBER:
				RemoveGuildMember( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_MEMBER_LEVEL:
				UpdateGuildMemberLv( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_CLASS:
				UpdateGuildClass( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_NICKNAME:
				UpdateGuildNickName( pQuery, lpDbOverlappedPlus );
				break;
			case QM_UPDATEGUILDMASTER:
				UpdateGuildMaster( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_LOGO:
				UpdateGuildLogo( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_CONTRIBUTION:
				UpdateGuildContribution( pQuery, pQueryLog, lpDbOverlappedPlus ); 
				break;
			case UPDATE_GUILD_NOTICE:
				UpdateGuildNotice( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_AUTHORITY:
				UpdateGuildAuthority( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_PENYA:
				UpdateGuildPenya( pQuery, lpDbOverlappedPlus );
				break;
			case UPDATE_GUILD_REALPAY:
				UpdateGuildRealPay( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case GUILD_SETNAME:
				UpdateGuildSetName( pQuery, lpDbOverlappedPlus );
				break;
			case GUILD_BANK_UPDAATE:
				UpdateGuildBankUpdate( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case GUILD_BANK_QUERY:
				OpenQueryGuildBank( pQuery, lpDbOverlappedPlus );
				break;

			case ADD_GUILD_VOTE:
				AddGuildVote( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_GUILD_VOTE:
				RemoveGuildVote( pQuery, lpDbOverlappedPlus );
				break;
			case CLOSE_GUILD_VOTE:
				CloseGuildVote( pQuery, lpDbOverlappedPlus );
				break;
			case CAST_GUILD_VOTE:
				CastGuildVote( pQuery, lpDbOverlappedPlus );
				break;

			case ACPT_GUILD_WAR:
				AcptWar( pQuery, lpDbOverlappedPlus );
				break;
			case WAR_END:
				WarEnd( pQuery, pQueryLog, lpDbOverlappedPlus );
				break;
			case SURRENDER:
				Surrender( pQuery, lpDbOverlappedPlus );
				break;
			case WAR_MASTER_ABSENT:
				WarMasterAbsent( pQuery, lpDbOverlappedPlus );
				break;
			case WAR_DEAD:
				WarDead( pQuery, lpDbOverlappedPlus );
				break;

			case GUILD_RANKING_UPDATE:
				UpdateGuildRanking( pQuery, lpDbOverlappedPlus );
				break;
			case GUILD_RANKING_UPDATE_DB:
				UpdateGuildRankingDB(pQuery, lpDbOverlappedPlus );
				break;

			case QM_QUERYGUILDQUEST:
				QueryGuildQuest( pQuery, lpDbOverlappedPlus );
				break;
			case QM_INSERTGUILDQUEST:
				InsertGuildQuest( pQuery, lpDbOverlappedPlus );
				break;
			case QM_UPDATEGUILDQUEST:
				UpdateGuildQuest( pQuery, lpDbOverlappedPlus );
				break;

			case WANTED_OP:
				WantedOperation( pQuery, lpDbOverlappedPlus );
				break;
			case START_GUILDCOMBAT:
				StartGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case IN_GUILDCOMBAT:
				InGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case OUT_GUILDCOMBAT:
				OutGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case RESULT_GUILDCOMBAT:
				ResultGuildCombat( pQuery, lpDbOverlappedPlus );
				break;
			case GETPENYAGUILD_GUILDCOMBAT:
				GetPenyaGuildGC( pQuery, lpDbOverlappedPlus );
				break;
			case GETPENYAPLAYER_GUILDCOMBAT:
				GetPenyaPlayerGC( pQuery, lpDbOverlappedPlus );
				break;
			case CONTINUE_GUILDCOMBAT:
				ContinueGC( pQuery, lpDbOverlappedPlus );
				break;
			case ADD_MAIL:
				AddMail( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_MAIL:
				RemoveMail( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_MAIL_ITEM:
				RemoveMailItem( pQuery, lpDbOverlappedPlus );
				break;
			case REMOVE_MAIL_GOLD:
				RemoveMailGold( pQuery, lpDbOverlappedPlus );
				break;
			case READ_MAIL:
				ReadMail( pQuery, lpDbOverlappedPlus );
				break;
			case RECOMMEND_EVE:
				RecommendEve( pQuery, lpDbOverlappedPlus );
				break;
			case QM_GETMAIL_REALTIME:
				QueryGetMailRealTime( pQuery );
				break;

			case GC1TO1_LOAD:
				LoadGC1to1TenderGuild( pQuery, lpDbOverlappedPlus, (DPID)dwCompletionKey );
				break;
			case GC1TO1_STATE:
				GC1to1State( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_TENDER:
				GC1to1Tender( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_LINEUP:
				GC1to1LineUp( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_WARPERSON:
				GC1to1WarPerson( pQuery, lpDbOverlappedPlus );
				break;
			case GC1TO1_WARGUILD:
				GC1to1WarGuild( pQuery, lpDbOverlappedPlus );
				break;

			case QM_GUILDBANK_LOG_VIEW:
				GuildBankLogView( pQueryLog, lpDbOverlappedPlus );
				break;

			case QM_SEALCHAR:
				SealChar( pQuery, lpDbOverlappedPlus );
				break;
			case QM_SEALCHARCONM:
				SealCharConm( pQuery, lpDbOverlappedPlus );
				break;
			case QM_SEALCHARGET:
				SealCharGet( pQuery, lpDbOverlappedPlus );
				break;
			case QM_SEALCHARSET:
				SealCharSet( pQuery, lpDbOverlappedPlus );
				break;

			case QM_ALL_PLAYER_DATA:
				{
					CDPTrans::GetInstance()->SendAllPlayerData( lpDbOverlappedPlus->dpid );
					CDPTrans::GetInstance()->SendAllMail( lpDbOverlappedPlus->dpid );
					break;
				}
			case QM_QUERY_MAIL_BOX:
				{
// 					//	BEGINTEST
// 					FLINFO_LOG( PROGRAM_NAME, _T( "QM_QUERY_MAIL_BOX" ) );

					CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
					u_long idReceiver;
					ar >> idReceiver;
					CPost *pPost    = CPost::GetInstance();
					CMclAutoLock	Lock( pPost->m_csPost );

					CMailBox* pMailBox	= NULL;
					pMailBox = pPost->GetMailBox( idReceiver );
					
					if( pMailBox != NULL )
					{
						CDPTrans::GetInstance()->SendMailBox( pMailBox, lpDbOverlappedPlus->dpid );
					}
					else
					{
						CDPTrans::GetInstance()->SendMailBoxReq( idReceiver, lpDbOverlappedPlus->dpid, FALSE, pMailBox );
					}

					break;
				}
				break;
			//case QM_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTION_INFO_AND_CLEAR_EXPIRED:
			//	{
			//		ExecCooperativeContributions_GetContributionInfo_And_ClearExpired( pQuery, lpDbOverlappedPlus );
			//	}
			//	break;
			case QM_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE:
				{
					ExecCooperativeContributions_Contribute( pQuery, lpDbOverlappedPlus );
				}
				break;

			case QM_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED:
				{
					ExecCooperativeContributions_GetContributed( pQuery, lpDbOverlappedPlus );
				}
				break;

			case QM_COOPERATIVE_CONTRIBUTIONS_GET_RANKING:
				{
					ExecCooperativeContributions_GetRanking( pQuery, lpDbOverlappedPlus );
				}
				break;
			//case QM_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM:
			//	{
			//		ExecCooperativeContributions_TryRewardItem( pQuery, lpDbOverlappedPlus );
			//	}
			//	break;
			case QM_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO:
				{
					ExecCooperativeContributions_TryWriteItemRewardInfo( pQuery, lpDbOverlappedPlus );
				}
				break;

			case QM_RECORD_DUNGEON_BEST_COMPLETE_TIME:
				{
					ExecRecord_DungeonBestCompleteTime( pQuery, pQueryLog, lpDbOverlappedPlus );
				}
				break;
			case QM_RECORD_GET_DUNGEON_RANKING:
				{
					ExecRecord_GetDungeonRanking( pQuery, lpDbOverlappedPlus );
				}
				break;

			case GUILD_BANK_INPUT_ITEM:
				{
					GuildBankInputItem( pQuery, pQueryLog, lpDbOverlappedPlus );
				}
				break;

			case GUILD_BANK_OUTPUT_ITEM:
				{
					GuildBankOutputItem( pQuery, pQueryLog, lpDbOverlappedPlus );
				}
				break;

// 			case GUILD_BANK_INPUT_GOLD:
// 				{
// 					GuildBankInputGold( pQuery, pQueryLog, lpDbOverlappedPlus );
// 				}
// 				break;

			case GUILD_BANK_OUTPUT_GOLD:
				{
					GuildBankOutputGold( pQuery, pQueryLog, lpDbOverlappedPlus );
				}
				break;

			default:
				FLERROR_LOG( PROGRAM_NAME, _T( "lpDbOverlappedPlus->nQueryMode Invalid : [%d]" ), lpDbOverlappedPlus->nQueryMode );
				break;
		}

		SAFE_DELETE_FREEREQUEST( lpDbOverlappedPlus );
	}
}

void CDbManager::SerializeGuildCombat( CAr & ar )
{
	ar << m_nGuildCombatIndex;
	ar << m_uWinGuildId;
	ar << m_nWinGuildCount;
	ar << m_uBestPlayer;
	ar << (u_long)m_vecGuildCombat.size();
	for( DWORD gci = 0 ; gci < m_vecGuildCombat.size() ; ++gci )
	{
		__GUILDCOMBATJOIN GCJoin = m_vecGuildCombat.at( gci );
		ar << GCJoin.uGuildId;
		ar << GCJoin.dwPenya;
		ar << GCJoin.bRequest;
	}

	SerializeResultValue( ar );
	SerializePlayerPoint( ar );
}
void CDbManager::SerializeResultValue( CAr & ar )
{
	// 길드 보상 관련 정보 주기
	ar << (u_long)m_GCResultValueGuild.size();
	for( DWORD veci = 0 ; veci < m_GCResultValueGuild.size() ; ++veci )
	{
		__GCRESULTVALUEGUILD ResultValueGuild = m_GCResultValueGuild.at( veci );
		ar << ResultValueGuild.nCombatID;
		ar << ResultValueGuild.uidGuild;
		ar << ResultValueGuild.nReturnCombatFee;
		ar << ResultValueGuild.nReward;
	}
	// 베스트 플레이어 보상 관련 정보 주기
	ar << (u_long)m_GCResultValuePlayer.size();
	for( DWORD veci = 0 ; veci < m_GCResultValuePlayer.size() ; ++veci )
	{
		__GCRESULTVALUEPLAYER ResultValuePlayer = m_GCResultValuePlayer.at( veci );
		ar << ResultValuePlayer.nCombatID;
		ar << ResultValuePlayer.uidGuild;
		ar << ResultValuePlayer.uidPlayer;
		ar << ResultValuePlayer.nReward;
	}
}
void CDbManager::SerializePlayerPoint( CAr & ar )
{
	// 길드대전 개인 포인트
	ar << (u_long)m_vecGCPlayerPoint.size();
	for( DWORD veci = 0 ; veci < m_vecGCPlayerPoint.size() ; ++veci )
	{
		__GCPLAYERPOINT GCPlayerPoint = m_vecGCPlayerPoint.at( veci );
		ar << GCPlayerPoint.uidPlayer;
		ar << GCPlayerPoint.nJob;
		ar << GCPlayerPoint.nPoint;
	}
}

/*
u_long CDbManager::GetPlayerID( LPCSTR lpszPlayer )
{
	CMclAutoLock	Lock( s_cs );
	m_idPlayer++;
	m_ulong2str.insert( std::map<u_long, string>::value_type( m_idPlayer, lpszPlayer ) );
	return( m_idPlayer );
}
*/

void CDbManager::SerializeWanted( CQuery* pQuery, CAr& out )
{
	int		nCount = 0;
	char	szPlayer[64];
	__int64 penya;
	CTime	date;

	u_long	idPlayer;
	char	szMsg[WANTED_MSG_MAX + 1] = "";

	ptrdiff_t nCountTag = out.ReserveSpace( sizeof( nCount ) );

	char szBuffer[32];
	char szDate[13] = {0,};
	while( pQuery->Fetch() )
	{
		idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
		pQuery->GetStr( "m_szName", szPlayer, _countof( szPlayer ) );			// 현상범 이름 
		penya = pQuery->GetInt64( "penya" );			// 현상금액 
		pQuery->GetStr( "s_date", szDate, _countof( szDate ) );				// 만기일 
		pQuery->GetStr( "szMsg", szBuffer, _countof( szBuffer ) );			// 십자평 

		GetStrTime( &date, szDate );
		memcpy( szMsg, szBuffer, WANTED_MSG_MAX );		// 잘못된 데이타를 막기위한 코드 
		szMsg[WANTED_MSG_MAX] = '\0';

		out << idPlayer;
		out.WriteString( szPlayer );
		out << penya;
		out << (long)date.GetTime(); 
		out.WriteString( szMsg );

		nCount++;
	}

	out.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );
}

BOOL CDbManager::OnWantedQuery( CQuery* pQuery, WANTED_QUERYINFO& info )
{
	char szQeury[1024] = { 0, };
	FLSPrintf( szQeury, _countof( szQeury ),
			"{call WANTED_STR('%s', '%07d', '%02d', '%d', ?)}", 
             info.pszType, info.idPlayer, g_appInfo.dwSys, info.nGold );

	if( !pQuery->BindParameterVarChar( SQL_PARAM_INPUT, WANTED_MSG_MAX,  info.szMsg ) )
		return FALSE;

	if( !pQuery->Exec( szQeury ) )
		return FALSE ;

	return TRUE;
}

BOOL CDbManager::OpenWanted( CAr& ar )
{
	CQuery qry;
#ifdef __DbStuff
	if (!qry.Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( qry.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
#endif
	{
		AfxMessageBox( "Error : Not Connect Character DB" );
		return FALSE;
	}

	WANTED_QUERYINFO info( NULL );
	info.pszType = "S1";
	if( OnWantedQuery( &qry, info ) )
	{
		SerializeWanted( &qry, ar );
		return TRUE;
	}
	else
		return FALSE;
}

//	WANTED_STR @iGu,@m_idPlayer,@serverindex,@penya,@szMsg
//	@iGu = 'S1' 현상범 리스트 얻기 (@m_idPlayer,@serverindex ) 
//	@iGu = 'A1' 현상금 걸기 (@m_idPlayer,@serverindex,@penya,@szMsg ) 
//	@iGu = 'D1' 현상범 제거 (@m_idPlayer,@serverindex) 
//	현상금 시스템 디비 처리를 한다.
void CDbManager::WantedOperation( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	BYTE	byReqType;
	u_long	idPlayer;
	int		nGold;
	long	nEnd;
	char	szMsg[WANTED_MSG_MAX + 1] = "";

	ar >> byReqType;
	ar >> idPlayer;
	ar >> nGold;
	ar >> nEnd;					// 사용하지 않는다.
	ar.ReadString( szMsg, _countof( szMsg ) );

	WANTED_QUERYINFO info( NULL );
	switch( byReqType )
	{
	case CT_WANTED_REQ_LIST:
		info.pszType = "S1";
		break;
	case CT_WANTED_SET:
		if( nGold == 0 )
		{
			info.pszType	= "D1";
			info.idPlayer	= idPlayer;
		}
		else
		{
			info.pszType	= "A1";
			info.idPlayer	= idPlayer;
			info.nGold		= nGold;
			FLStrcpy( info.szMsg, _countof( info.szMsg ), szMsg );
		}
		break;
	default:
		FLASSERT( FALSE );
		break;
	}

#ifdef __INTERNALSERVER
	OnWantedQuery( pQuery, info );
#else
	if( OnWantedQuery( pQuery, info ) )
	{
		if( byReqType == CT_WANTED_REQ_LIST )
		{
			BEFORESEND( out, PACKETTYPE_TC_LIST );
			SerializeWanted( pQuery, out );
			SEND( out, &g_dpCoreSrvr, DPID_ALLPLAYERS );
		}
	}
#endif
}

void CDbManager::InGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idGuild;
	DWORD dwPenya;
	DWORD dwExistingPenya;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idGuild;
	ar >> dwPenya;	
	ar >> dwExistingPenya;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspJoinGuildToCombat %d,'%02d','%06d',%d,%d", 
		m_nGuildCombatIndex, g_appInfo.dwSys, idGuild, dwPenya,0 );

	if( pQuery->Exec( szQuery ) )
	{
		__GUILDCOMBATJOIN GCJoin;
		GCJoin.dwPenya = dwPenya;
		GCJoin.bRequest = TRUE;
		GCJoin.uGuildId = idGuild;
		m_vecGuildCombat.push_back( GCJoin );
		CDPTrans::GetInstance()->SendInGuildCombat( idGuild, dwPenya, dwExistingPenya );
	}
	else
	{	FLERROR_LOG( PROGRAM_NAME, _T( "InGuildCombat()" ) ); }	
}

void CDbManager::OutGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	u_long idGuild;
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> idGuild;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspCancelGuildToCombat %d,'%02d','%06d'", m_nGuildCombatIndex, g_appInfo.dwSys, idGuild );

	if( pQuery->Exec( szQuery ) )
	{
// 		BOOL bFind = FALSE;
// 		__GUILDCOMBATJOIN GCJoin;
// 		for( DWORD gci = 0 ; gci < m_vecGuildCombat.size() ; ++gci )
// 		{
// 			if( idGuild == m_vecGuildCombat[gci].uGuildId )
// 			{
// 				bFind = TRUE;
// 				GCJoin = m_vecGuildCombat[gci];
// 				break;
// 			}
// 		}
// 		if( bFind )
// 			GCJoin.bRequest = FALSE;

		for( std::vector< __GUILDCOMBATJOIN >::iterator pos = m_vecGuildCombat.begin(); pos != m_vecGuildCombat.end(); ++pos )
		{
			__GUILDCOMBATJOIN kGCJoin = *pos;
			if( kGCJoin.uGuildId == idGuild )
			{
				m_vecGuildCombat.erase( pos );
				break;
			}
		}

		CDPTrans::GetInstance()->SendOutGuildCombat( idGuild );
	}
	else
	{	FLERROR_LOG( PROGRAM_NAME, _T( "OutGuildCombat()" ) );	}
}
void CDbManager::StartGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	// 길드 보상 및 출전 페냐 소멸
	for( DWORD veci = 0 ; veci < m_GCResultValueGuild.size() ; ++veci )
	{
		__GCRESULTVALUEGUILD ResultValueGuild = m_GCResultValueGuild.at( veci );

		char szQuery[MAX_QUERY_SIZE]	= { 0,};
		FLSPrintf( szQuery, _countof( szQuery ), "uspExpireCombatReward %d,'%02d','%06d','%07d'", 
			ResultValueGuild.nCombatID, g_appInfo.dwSys, ResultValueGuild.uidGuild, 0 );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "StartGuildCombat()" ) );	}
	}
	m_GCResultValueGuild.clear();

	// 베스트 플레이어 보상
	for( DWORD veci = 0 ; veci < m_GCResultValuePlayer.size() ; ++veci )
	{
		__GCRESULTVALUEPLAYER ResultValuePlayer = m_GCResultValuePlayer.at( veci );
		char szQuery[MAX_QUERY_SIZE]	= { 0,};
		FLSPrintf( szQuery, _countof( szQuery ), "uspExpireCombatReward %d,'%02d','%06d','%07d'", 
			ResultValuePlayer.nCombatID, g_appInfo.dwSys, ResultValuePlayer.uidGuild, ResultValuePlayer.uidPlayer );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "StartGuildCombat()" ) );	}
	}
	m_GCResultValueGuild.clear();
	
	// 대전 전쟁 시작
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspStartCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "StartGuildCombat()" ) );	}
}
void CDbManager::ResultGuildCombat( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	// 대전 종료
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspEndCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}
	
/*	// 베스트 플레이어 저장
	u_long uBestPlayer;
	u_long uBestPlayerGuild;
	DWORD dwBestPlayerPrize;
	int nGetPoint;
	ar >> uBestPlayer;
	ar >> uBestPlayerGuild;
	ar >> nGetPoint;
	ar >> dwBestPlayerPrize;

	sprintf( lpDbOverlappedPlus->szQuery, "uspGradeCombatPlayer %d,'%02d','%06d','%06d',%d, %d", 
		m_nGuildCombatIndex, g_appInfo.dwSys, uBestPlayerGuild, uBestPlayer, nGetPoint, dwBestPlayerPrize );
	if( FALSE == pQuery->Exec( lpDbOverlappedPlus->szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}
	
*/	
	ar >> m_uWinGuildId;
	ar >> m_nWinGuildCount;
	ar >> m_uBestPlayer;	

	int nGuildPoint, nWinCount;
	u_long uidGuild, uSizeGuild;
	__int64 nPrize;
	DWORD dwReturnPenya;
	// 길드 저장
	ar >> uSizeGuild;
	for( DWORD i = 0 ; i < uSizeGuild ; ++i )
	{
		ar >> uidGuild;
		ar >> nGuildPoint;
		ar >> nPrize;
		ar >> dwReturnPenya;
		ar >> nWinCount;
		memset( szQuery, 0, sizeof( szQuery ) );
		FLSPrintf( szQuery, _countof( szQuery ), "uspGradeCombatGuild %d,'%02d','%06d',%d,%d,%I64d,%d", 
			m_nGuildCombatIndex, g_appInfo.dwSys, uidGuild, nGuildPoint, dwReturnPenya, nPrize, nWinCount );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}

		// 플레이어 저장
		u_long uSizePlayer;
		u_long uidPlayer;
		int nPlayerPoint;
		__int64 nPrizePlyaer;
		ar >> uSizePlayer;
		for( DWORD j = 0 ; j < uSizePlayer ; ++j )
		{
			ar >> uidPlayer;
			ar >> nPlayerPoint;
			ar >> nPrizePlyaer;
			if( m_uBestPlayer == uidPlayer )
				nPrizePlyaer = 1;
//			if( 0 < nPrizePlyaer )
//				m_uBestPlayer = uidPlayer;

			memset( szQuery, 0, sizeof( szQuery ) );
			FLSPrintf( szQuery, _countof( szQuery ), "uspGradeCombatPlayer %d,'%02d','%06d','%07d',%d,%I64d", 
				m_nGuildCombatIndex, g_appInfo.dwSys, uidGuild, uidPlayer, nPlayerPoint, nPrizePlyaer );
			if( FALSE == pQuery->Exec( szQuery ) )
			{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}
		}
	}

	m_vecGuildCombat.clear();
	++m_nGuildCombatIndex;

	// 다음대전 시작
	memset( szQuery, 0, sizeof( szQuery ) );
	FLSPrintf( szQuery, _countof( szQuery ), "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}

	// 보상 및 참여금을 안받은 길드 및 베스트 플레이어 얻어오기
	m_GCResultValueGuild.clear();
	m_GCResultValuePlayer.clear();
	memset( szQuery, 0, sizeof( szQuery ) );
	FLSPrintf( szQuery, _countof( szQuery ), "uspLoadCombatUnpaidList '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}
	while( pQuery->Fetch() )
	{
		char cFlag;
		cFlag = pQuery->GetChar( "Flag" );
		if( cFlag == 'G' )	// 길드
		{
			__GCRESULTVALUEGUILD ResultValueGuild;
			ResultValueGuild.nCombatID = pQuery->GetInt( "CombatID" );
			ResultValueGuild.uidGuild = pQuery->GetInt( "GuildID" );
			ResultValueGuild.nReturnCombatFee = pQuery->GetInt64( "ReturnCombatFee" );
			ResultValueGuild.nReward = pQuery->GetInt64( "Reward" );
			m_GCResultValueGuild.push_back( ResultValueGuild );
		}
		else				// 플레이어
		{
			__GCRESULTVALUEPLAYER ResultValuePlayer;
			ResultValuePlayer.nCombatID = pQuery->GetInt( "CombatID" );
			ResultValuePlayer.uidGuild = pQuery->GetInt( "GuildID" );
			ResultValuePlayer.uidPlayer = pQuery->GetInt( "PlayerID" );
			ResultValuePlayer.nReward = pQuery->GetInt64( "Reward" );
			m_GCResultValuePlayer.push_back( ResultValuePlayer );
		}
	}	
	
	// 개인 포인트 가지고 오기
	m_vecGCPlayerPoint.clear();
	memset( szQuery, 0, sizeof( szQuery ) );
	FLSPrintf( szQuery, _countof( szQuery ), "uspRankGuildCombatPlayer '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "ResultGuildCombat()" ) );	}
	while( pQuery->Fetch() )
	{
		__GCPLAYERPOINT GCPlayerPoint;
		GCPlayerPoint.uidPlayer = pQuery->GetInt( "PlayerID" );
		GCPlayerPoint.nJob = pQuery->GetInt( "Job" );
		GCPlayerPoint.nPoint = pQuery->GetInt( "PointSummary" );
		m_vecGCPlayerPoint.push_back( GCPlayerPoint );
	}
	
	CDPTrans::GetInstance()->SendResultGuildCombat();
	CDPTrans::GetInstance()->SendPlayerPointGuildCombat();
}
void CDbManager::GetPenyaGuildGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long uidPlayer = NULL_PLAYER_ID;
	u_long uidGuild = 0;
	int nGuildCombatID;
	ar >> uidPlayer;
	ar >> nGuildCombatID;
	ar >> uidGuild;

	std::vector<__GCRESULTVALUEGUILD>::iterator erase_pos = m_GCResultValueGuild.end();
	std::vector<__GCRESULTVALUEGUILD>::iterator pos = m_GCResultValueGuild.begin();
	std::vector<__GCRESULTVALUEGUILD>::iterator end = m_GCResultValueGuild.end();
	__GCRESULTVALUEGUILD ResultValuetmp = { 0 };
	for( ; pos != end; ++pos )
	{
		ResultValuetmp = (*pos);
		if( nGuildCombatID == ResultValuetmp.nCombatID && uidGuild == ResultValuetmp.uidGuild )
		{
			erase_pos = pos;
			break;
		}
	}
	if( erase_pos != end )
	{
		char szQuery[MAX_QUERY_SIZE]	= { 0,};
		FLSPrintf( szQuery, _countof( szQuery ), "uspRewardCombatGuild %d,'%02d','%06d'", ResultValuetmp.nCombatID, g_appInfo.dwSys, ResultValuetmp.uidGuild );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "GetPenyaGuildGuildCombat()" ) );	}

		CDPTrans::GetInstance()->SendGetPenyaGuildGC( uidPlayer, nGuildCombatID, uidGuild );
		m_GCResultValueGuild.erase( erase_pos );		
	}
}
void CDbManager::GetPenyaPlayerGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long uidPlayer = NULL_PLAYER_ID;
	u_long uidGuild = 0;
	int nGuildCombatID;
	ar >> uidPlayer;
	ar >> nGuildCombatID;
	ar >> uidGuild;

	std::vector<__GCRESULTVALUEPLAYER>::iterator erase_pos = m_GCResultValuePlayer.end();
	std::vector<__GCRESULTVALUEPLAYER>::iterator pos = m_GCResultValuePlayer.begin();
	std::vector<__GCRESULTVALUEPLAYER>::iterator end = m_GCResultValuePlayer.end();
	__GCRESULTVALUEPLAYER ResultValuetmp = { 0 };
	for( ; pos != end; ++pos )
	{
		ResultValuetmp = (*pos);
		if( nGuildCombatID == ResultValuetmp.nCombatID && uidPlayer == ResultValuetmp.uidPlayer && uidGuild == ResultValuetmp.uidGuild )
		{
			erase_pos = pos;
			break;
		}
	}
	if( erase_pos != end )
	{
		char szQuery[MAX_QUERY_SIZE]	= { 0,};
		FLSPrintf( szQuery, _countof( szQuery ), "uspRewardCombatPlayer %d,'%02d','%06d','%07d'"
			, ResultValuetmp.nCombatID, g_appInfo.dwSys, uidGuild, ResultValuetmp.uidPlayer );
		if( FALSE == pQuery->Exec( szQuery ) )
		{	FLERROR_LOG( PROGRAM_NAME, _T( "GetPenyaGuildGuildCombat()" ) );	}

		CDPTrans::GetInstance()->SendGetPenyaPlayerGC( uidPlayer, nGuildCombatID, uidGuild );
		m_GCResultValuePlayer.erase( erase_pos );		
	}
}

void CDbManager::ContinueGC( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long uidGuild = 0;
	int nGuildCombatID;
	int nWinCount = 0;
	ar >> nGuildCombatID;
	ar >> uidGuild;
	ar >> nWinCount;

	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspCombatContinue %d,'%02d','%06d',%d"
		, nGuildCombatID, g_appInfo.dwSys, uidGuild, nWinCount );
	if( FALSE == pQuery->Exec( szQuery ) )
	{	FLERROR_LOG( PROGRAM_NAME, _T( "GetPenyaGuildGuildCombat()" ) );	}

	CDPTrans::GetInstance()->SendContinueGC();
}

BOOL CDbManager::CanJoin( LPCTSTR szAccount, ACCOUNT_CACHE* pAccountCache )
{ 
	time_t t	= ::time( NULL );
	if( pAccountCache->IsPlaying() )
	{
		// 멀티 서버 번호가 초기화되지 않는 경우가 있다.
		// 종료 후 시간이 3분 이상 경과 되었다면 허용한다.
		if( t > pAccountCache->m_tmPrevent + 180 )
		{
			pAccountCache->ChangeMultiServer( 0 );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ Not Initialize MultiServer. Account(%s) ]" ), szAccount );
			return FALSE;
		}
	}

	if( t > pAccountCache->m_tmPrevent )	// 일반 
	{
		return TRUE;
	}
	else
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "CanJoin - cur:%d prevent:%d" ), t, pAccountCache->m_tmPrevent );
		return FALSE;
	}
}

void CDbManager::PutExpLog( CQuery* pQueryLog, CMover* pMover, char ch )
{
	BOOL bPut	= ( ch != 'P' );
	DWORD dwTick	= GetTickCount();
	if( !bPut )
	{
		if( pMover->m_dwPeriodicTick < dwTick )
			bPut	= TRUE;
	}
	if( bPut )
	{
		pMover->m_dwPeriodicTick	= dwTick + MIN( 120 );

		char szSql[1024]	= { 0, };
		DBQryLog( szSql, _countof( szSql ), "L1", pMover->m_idPlayer, g_appInfo.dwSys,
			pMover->m_nExp1, pMover->m_nLevel,
			pMover->m_nJob, 0, pMover->GetFlightLv(),
			pMover->m_Stat.GetOriginStr(), pMover->m_Stat.GetOriginDex(), pMover->m_Stat.GetOriginInt(), pMover->m_Stat.GetOriginSta(),
			pMover->m_Stat.GetRemainStatPoint(), 0,
			ch, 0, '\0', 0, 0, 0, pMover->GetGold(), 0 );

		if( FALSE == pQueryLog->Exec( szSql ) )
			return;
	}
}

void CDbManager::SetPlayerName( CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long idPlayer = NULL_PLAYER_ID;
	DWORD dwData = 0;
	char lpszPlayer[MAX_PLAYER];
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, _countof( lpszPlayer ) );
	ar >> dwData;

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	BOOL bRet	= FALSE;

	CMover Mover;
	Mover.m_idPlayer	= idPlayer;
	FLStrcpy( Mover.m_szName, _countof( Mover.m_szName ), lpszPlayer );

	T_CHARACTER_STR_INFO tCharacter( Mover, "U4" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "U4" );
// 	tCharacter.dwPlayerId = idPlayer;
// 	FLStrcpy( tCharacter.szPlayerName, _countof( tCharacter.szPlayerName ), lpszPlayer );

	if( DBQryCharacter( pQueryChar, &tCharacter ) == true )
	{
		if( pQueryChar->Fetch() )
		{
			bRet = (BOOL)pQueryChar->GetInt( "fError" );
		}

		FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// QM_SET_PLAYER_NAME\t//1(%d)\n" ), bRet );
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	if( bRet )
	{
		char szOldPlayer[MAX_PLAYER]		= { 0, };
		CPlayerDataCenter::GetInstance()->ReplacePlayerString( idPlayer, lpszPlayer, szOldPlayer, _countof( szOldPlayer ) );

/*
		uspChangeNameLog
                  @pserverindex char(2) :
                  @pidPlayer char(6) :
                  @pOldName varchar(32) :
                  @pNewName varchar(32) :
*/

		CString strOldName	= szOldPlayer;
		strOldName.Replace( '\'', ' ' );
		TCHAR szOldName[ MAX_PLAYER ]	= { 0, };
		FLStrcpy( szOldName, _countof( szOldName ), strOldName );

		CString strCurrName	= lpszPlayer;
		strCurrName.Replace( '\'', ' ' );
		TCHAR szCurrName[ MAX_PLAYER ]	= { 0, };
		FLStrcpy( szCurrName, _countof( szCurrName ), strCurrName );

		char szQuery[MAX_QUERY_SIZE]	= { 0,};
		FLSPrintf( szQuery, _countof( szQuery ), "uspChangeNameLog @pserverindex = '%02d', @pidPlayer = '%07d', @pOldName='%s', @pNewName='%s'",
			g_appInfo.dwSys, idPlayer, szOldName, szCurrName );
#ifdef _DEBUG
		FLTRACE_LOG( PROGRAM_NAME, szQuery );
#endif	// _DEBUG
		if( pQueryLog->Exec( szQuery ) == FALSE )
			FLERROR_LOG( PROGRAM_NAME, _T( "query failed: %s" ), szQuery );
	}
	g_dpCoreSrvr.SendSetPlayerName( idPlayer, lpszPlayer, dwData, bRet );
}

void CDbManager::SnoopGuild( CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	u_long idGuild = 0;
	u_long idPlayer = NULL_PLAYER_ID;
	char lpszChat[1024]		= { 0, };
	ar >> idGuild >> idPlayer;
	ar.ReadString( lpszChat, _countof( lpszChat ) );

	CString strChat	= lpszChat;
	strChat.Replace( "--", "" );
	strChat.Replace( "'", "" );
	strChat.Replace( "/*", "" );
	strChat.Replace( "*/", "" );

	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// QM_SNOOPGUILD" ) );
	// LOG_GAMEMASTER_STR '%s','%s','%s'
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "LOG_GAMEMASTER_STR  '@im_idPlayer=%07d',@iserverindex='%02d',@iszWords='%s',@im_idGuild='%06d'", idPlayer, g_appInfo.dwSys, strChat, idGuild );
	if ( FALSE == pQueryLog->Exec( szQuery ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
}

//////////////////////////////////////////////////////////////////////////
// BEGIN110119

bool CDbManager::DBQryCharacter( CQuery* pQuery, const PT_CHARACTER_STR_INFO ptCharacter )
{
	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

// 	FLSPrintf( szQuery, _countof( szQuery ), "{call CHARACTER_STR("
// 		"?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?,?,?,?,?"
// 		",?,?,?,?,?,?"
// 		")}" );


	const DWORD MAX_PARAMETER = 116;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "CHARACTER_STR" ), MAX_PARAMETER );
	
	TCHAR szPlayerId[8] = { 0, };
	TCHAR szServerIndex[3] = { 0, };
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", ptCharacter->dwPlayerId );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	// 1
	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		2,		ptCharacter->szGu );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		7,		szPlayerId );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		2,		szServerIndex );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   32, 	ptCharacter->szAccount );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   32, 	ptCharacter->szPlayerName );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   &ptCharacter->nPlayerSlot );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   &ptCharacter->dwWorldId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwIndex );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT, &ptCharacter->fPos_x );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT, &ptCharacter->fPos_y );


	// 11
	pQuery->BindParameterFloat( SQL_PARAM_INPUT, &ptCharacter->fPos_z );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   32, 	ptCharacter->szCharacterKey );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwSkinSet );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwHairMesh );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwHairColor );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   &ptCharacter->dwHeadMesh );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwSex );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT, &ptCharacter->fScale_x );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   &ptCharacter->dwMotion );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT, &ptCharacter->fAngle );


	// 21
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nHitPoint );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nManaPoint );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nFatiguePoint );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwRideItemId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwGold );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nJob );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   50, 	ptCharacter->szActMover );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nStr );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nSta );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nDex );


	// 31
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nInt );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nLevel );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT, &ptCharacter->n64Exp1 );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT, &ptCharacter->n64Exp2 );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 500, 	 ptCharacter->szJobSkill );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 500, 	 ptCharacter->szLicenseSkill );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 500, 	 ptCharacter->szJobLv );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   		 &ptCharacter->dwExpertLv );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   		 &ptCharacter->dwMarkingWorldId );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT,  &ptCharacter->fMarkingPos_x );


	// 41
	pQuery->BindParameterFloat( SQL_PARAM_INPUT,  &ptCharacter->fMarkingPos_y );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT,  &ptCharacter->fMarkingPos_z );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nRemainGP );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nRemainLP );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,   &ptCharacter->nTutorialState );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nFxp );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nTxp );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   3072,	 ptCharacter->szQuestCntArray );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,      1,		 ptCharacter->szAuthority );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwMode );


	// 51
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nPartyId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwMuerdererId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nFame );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT,  &ptCharacter->n64DeathExp );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nDeathLevel );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->dwFlyTime );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwMessengerState );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nTotalPlayTime );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nPKValue );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwPKPropensity );


	// 61
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->dwPKExp );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   1980, 	 ptCharacter->szCard );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   215,  	 ptCharacter->szIndex_Card );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   215,  	 ptCharacter->szObjIndex_Card );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   1980, 	 ptCharacter->szCube );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   215,   ptCharacter->szIndex_Cube );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   215,  	 ptCharacter->szObjIndex_Cube );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   6940, 	 ptCharacter->szInventory );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   345 , 	 ptCharacter->szIndex_Inventory );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   135 , 	 ptCharacter->szEquipment );


	// 71
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   345 , 	 ptCharacter->szObjIndex_Inventory );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   3100, 	 ptCharacter->szSlotApplet );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   6885, 	 ptCharacter->szSlotItem );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   225,	 ptCharacter->szSlotQueue );
	pQuery->BindParameterShort( SQL_PARAM_INPUT,  &ptCharacter->nActionPoint );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   4290,	 ptCharacter->szBank );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   215,	 ptCharacter->szIndex_Bank );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   215,	 ptCharacter->szObjIndex_Bank );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwGoldBank );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nFuel );


	// 81
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->tmAccFuel );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   2560, 	 ptCharacter->szSMTime );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   2048, 	 ptCharacter->szSkillInfluence );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwSkillPoint );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   3072, 	 ptCharacter->szCompleteQuest );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   2000, 	 ptCharacter->szExtInven );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   2000, 	 ptCharacter->szExtBank );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   8000, 	 ptCharacter->szPirecingInven );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   8000, 	 ptCharacter->szPirecingBank );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->dwReturnWorldId );


	// 91
	pQuery->BindParameterFloat( SQL_PARAM_INPUT,  &ptCharacter->fReturnPos_x );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT,  &ptCharacter->fReturnPos_y );
	pQuery->BindParameterFloat( SQL_PARAM_INPUT,  &ptCharacter->fReturnPos_z );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nSkillPoint );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nSkillLevel );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT,  &ptCharacter->n64SkillExp );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT,  &ptCharacter->n64EventFlag );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->dwEventTime );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->dwEventElapsed );
	pQuery->BindParameterInt64( SQL_PARAM_INPUT,  &ptCharacter->n64AngelExp );


	// 101
	pQuery->BindParameterInt( SQL_PARAM_INPUT, 	&ptCharacter->nAngelLevel );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   4200,	 ptCharacter->szInventoryPet );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   4200,	 ptCharacter->szBankPet );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->dwPetId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nExpLog );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nAngelExpLog );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,    &ptCharacter->nCoupon );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nHonor );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,     &ptCharacter->nLayer );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,   100,	 ptCharacter->szCheckedQuest );


	// 111
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	&ptCharacter->nCampusPoint );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	&ptCharacter->dwCampusId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	&ptCharacter->dcStr );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	&ptCharacter->dcSta );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	&ptCharacter->dcDex );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	&ptCharacter->dcInt );


	if( pQuery->Exec( szQuery ) == TRUE )
	{
		return true;
	}

	WriteLogCharacter( ptCharacter );
	//SendMessage ( hMainWnd, WM_CLOSE, 0, 0 );
	
	return false;
}

void CDbManager::WriteLogCharacter( const PT_CHARACTER_STR_INFO ptCharacter )
{
	TCHAR szLog[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szLog, _countof( szLog )
		, "CHARACTER_STR @iGu = '%s'"
		", @im_idPlayer = '%07d'"
		", @iserverindex = '%02d'"
		", @iaccount = '%s'"
		", @im_szName = '%s'"
		", @iplayerslot = %d"
		", @idwWorldID = %d"
		", @im_dwIndex = %d"
		", @im_vPos_x = %f"
		", @im_vPos_y = %f"
		", @im_vPos_z = %f"
		", @im_szCharacterKey = '%s'"
		", @im_dwSkinSet = %d"
		", @im_dwHairMesh = %d"
		", @im_dwHairColor = %d"
		", @im_dwHeadMesh = %d"
		", @im_dwSex = %d"
		", @im_vScale_x = %f"
		", @im_dwMotion = %d"
		", @im_fAngle = %f"
		", @im_nHitPoint = %d"
		", @im_nManaPoint = %d"
		", @im_nFatiguePoint = %d"
		", @im_dwRideItemIdx = %d"
		", @im_dwGold = %d"
		", @im_nJob = %d"
		", @im_pActMover = '%s'"
		", @im_nStr = %d"
		", @im_nSta = %d"
		", @im_nDex = %d"
		", @im_nInt = %d"
		", @im_nLevel = %d"
		", @im_nExp1 = %I64d"
		", @im_nExp2 = %I64d"
		", @im_aJobSkill = '%s'"
		", @im_aLicenseSkill = '%s'"
		", @im_aJobLv = '%s'"
		", @im_dwExpertLv = %d"
		", @im_idMarkingWorld = %d"
		", @im_vMarkingPos_x = %f"
		", @im_vMarkingPos_y = %f"
		", @im_vMarkingPos_z = %f"
		", @im_nRemainGP = %d"
		", @im_nRemainLP = %d"
		", @im_nFlightLv = %d"
		", @im_nFxp = %d"
		", @im_nTxp = %d"
		", @im_lpQuestCntArray = '%s'"
		", @im_chAuthority = '%s'"
		", @im_dwMode = %d"
		", @im_idparty = %d"
		", @im_idMuerderer = %d"
		", @im_nFame = %d"
		", @im_nDeathExp = %I64d"
		", @im_nDeathLevel = %d"
		", @im_dwFlyTime = %d"
		", @im_nMessengerState = %d"
		", @iTotalPlayTime = %d"
		", @im_nPKValue = %d"
		", @im_dwPKPropensity = %d"
		", @im_dwPKExp = %d"
		", @im_Card = '%s'"
		", @im_Index_Card = '%s'"
		", @im_ObjIndex_Card = '%s'"
		", @im_Cube = '%s'"
		", @im_Index_Cube = '%s'"
		", @im_ObjIndex_Cube = '%s'"
		", @im_Inventory = '%s'"
		", @im_apIndex = '%s'"
		", @im_adwEquipment = '%s'"
		", @im_dwObjIndex = '%s'"
		", @im_aSlotApplet = '%s'"
		", @im_aSlotItem = '%s'"
		", @im_aSlotQueue = '%s'"
		", @im_SkillBar = %d"
		", @im_Bank = '%s'"
		", @im_apIndex_Bank = '%s'"
		", @im_dwObjIndex_Bank = '%s'"
		", @im_dwGoldBank = %d"
		", @im_nFuel = %d"
		", @im_tmAccFuel = %d"
		", @im_dwSMTime = '%s'"
		", @iSkillInfluence = '%s'"
		", @im_dwSkillPoint = %d"
		", @im_aCompleteQuest = '%s'"
		", @im_extInventory = '%s'"
		", @im_extBank = '%s'"
		", @im_InventoryPiercing = '%s'"
		", @im_BankPiercing = '%s'"
		", @im_dwReturnWorldID = %d"
		", @im_vReturnPos_x = %f"
		", @im_vReturnPos_y = %f"
		", @im_vReturnPos_z = %f"
		", @im_SkillPoint = %d"
		", @im_SkillLv = %d"
		", @im_SkillExp = %I64d"
		", @idwEventFlag = %I64d"
		", @idwEventTime = %d"
		", @idwEventElapsed = %d"
		", @im_nAngelExp = %I64d"
		", @im_nAngelLevel = %d"
		", @iszInventoryPet = '%s'"
		", @iszBankPet = '%s'"
		", @im_dwPetId = %d"
		", @im_nExpLog = %d"
		", @im_nAngelExpLog = %d"
		", @im_nCoupon = %d"
		", @im_nHonor = %d"
		", @im_nLayer = %d"
		", @im_aCheckedQuest = '%s'"
		", @im_nCampusPoint = %d"
		", @im_idCampus = %d"

		, ptCharacter->szGu
		, ptCharacter->dwPlayerId
		, g_appInfo.dwSys
		, ptCharacter->szAccount
		, ptCharacter->szPlayerName
		, ptCharacter->nPlayerSlot
		, ptCharacter->dwWorldId
		, ptCharacter->dwIndex
		, ptCharacter->fPos_x
		, ptCharacter->fPos_y
		, ptCharacter->fPos_z
		, ptCharacter->szCharacterKey
		, ptCharacter->dwSkinSet
		, ptCharacter->dwHairMesh
		, ptCharacter->dwHairColor
		, ptCharacter->dwHeadMesh
		, ptCharacter->dwSex
		, ptCharacter->fScale_x
		, ptCharacter->dwMotion
		, ptCharacter->fAngle
		, ptCharacter->nHitPoint
		, ptCharacter->nManaPoint
		, ptCharacter->nFatiguePoint
		, ptCharacter->dwRideItemId
		, ptCharacter->dwGold
		, ptCharacter->nJob
		, ptCharacter->szActMover
		, ptCharacter->nStr
		, ptCharacter->nSta
		, ptCharacter->nDex
		, ptCharacter->nInt
		, ptCharacter->nLevel
		, ptCharacter->n64Exp1
		, ptCharacter->n64Exp2
		, ptCharacter->szJobSkill
		, ptCharacter->szLicenseSkill
		, ptCharacter->szJobLv
		, ptCharacter->dwExpertLv
		, ptCharacter->dwMarkingWorldId
		, ptCharacter->fMarkingPos_x
		, ptCharacter->fMarkingPos_y
		, ptCharacter->fMarkingPos_z
		, ptCharacter->nRemainGP
		, ptCharacter->nRemainLP
		, ptCharacter->nTutorialState
		, ptCharacter->nFxp
		, ptCharacter->nTxp
		, ptCharacter->szQuestCntArray
		, ptCharacter->szAuthority
		, ptCharacter->dwMode
		, ptCharacter->nPartyId
		, ptCharacter->dwMuerdererId
		, ptCharacter->nFame
		, ptCharacter->n64DeathExp
		, ptCharacter->nDeathLevel
		, ptCharacter->dwFlyTime
		, ptCharacter->dwMessengerState
		, ptCharacter->nTotalPlayTime
		, ptCharacter->nPKValue
		, ptCharacter->dwPKPropensity
		, ptCharacter->dwPKExp
		, ptCharacter->szCard
		, ptCharacter->szIndex_Card
		, ptCharacter->szObjIndex_Card
		, ptCharacter->szCube
		, ptCharacter->szIndex_Cube
		, ptCharacter->szObjIndex_Cube
		, ptCharacter->szInventory
		, ptCharacter->szIndex_Inventory
		, ptCharacter->szEquipment
		, ptCharacter->szObjIndex_Inventory
		, ptCharacter->szSlotApplet
		, ptCharacter->szSlotItem
		, ptCharacter->szSlotQueue
		, ptCharacter->nActionPoint
		, ptCharacter->szBank
		, ptCharacter->szIndex_Bank
		, ptCharacter->szObjIndex_Bank
		, ptCharacter->dwGoldBank
		, ptCharacter->nFuel
		, ptCharacter->tmAccFuel
		, ptCharacter->szSMTime
		, ptCharacter->szSkillInfluence
		, ptCharacter->dwSkillPoint
		, ptCharacter->szCompleteQuest
		, ptCharacter->szExtInven
		, ptCharacter->szExtBank
		, ptCharacter->szPirecingInven
		, ptCharacter->szPirecingBank
		, ptCharacter->dwReturnWorldId
		, ptCharacter->fReturnPos_x
		, ptCharacter->fReturnPos_y
		, ptCharacter->fReturnPos_z
		, ptCharacter->nSkillPoint
		, ptCharacter->nSkillLevel
		, ptCharacter->n64SkillExp
		, ptCharacter->n64EventFlag
		, ptCharacter->dwEventTime
		, ptCharacter->dwEventElapsed
		, ptCharacter->n64AngelExp
		, ptCharacter->nAngelLevel
		, ptCharacter->szInventoryPet
		, ptCharacter->szBankPet
		, ptCharacter->dwPetId
		, ptCharacter->nExpLog
		, ptCharacter->nAngelExpLog
		, ptCharacter->nCoupon
		, ptCharacter->nHonor
		, ptCharacter->nLayer
		, ptCharacter->szCheckedQuest
		, ptCharacter->nCampusPoint
		, ptCharacter->dwCampusId
		);

	FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] %s" ), szLog );
}

void	CDbManager::MakeQueryBindParameter( TCHAR* pSql, size_t cchSql, TCHAR* pProcedure, DWORD dwParameterNum )
{
	FLSPrintf( pSql, cchSql, "{call %s(?", pProcedure );

	for( DWORD i = 1; i < dwParameterNum; ++i )
	{
		FLStrcat( pSql, cchSql, _T( ",?" ) );
	}

	FLStrcat( pSql, cchSql, _T( ")}" ) );
}

// END110119
//////////////////////////////////////////////////////////////////////////

BOOL CheckValidItem( DWORD dwItemId, int nNum )
{
		PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( dwItemId );
		if( NULL == pItemProp )
		{
			return FALSE;
		}
		else
		{
			if( nNum <= 0 || nNum > (int)pItemProp->dwPackMax )
				return FALSE;
		}
		return TRUE;
}

BOOL CDbManager::LoadPost( void )
{
	CPost *pPost = CPost::GetInstance();

	CMclAutoLock	Lock( pPost->m_csPost );
#ifdef __DbStuff
	if (!m_qryPostProc.Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( m_qryPostProc.Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Can't connect CHAR_DB" ) );
		AfxMessageBox( "Error : Can't connect CHAR_DB" );
		return FALSE;
	}

	// 우편 무결성 검사
	std::set<int>	setnMail;
	int	nTotal	= 0;

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	T_MAIL_STR_INFO tMail;

	FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "S1" ) );

	if( DbQryMail( &m_qryPostProc, &tMail ) == false )
	{
		AfxMessageBox( "QUERY: MAIL_STR 'S1'" );
		return FALSE;
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	while( m_qryPostProc.Fetch() )
	{
		u_long idReceiver	= m_qryPostProc.GetInt( "idReceiver" );
		CMail* pMail	= new CMail;
		pMail->m_nMail	= m_qryPostProc.GetInt( "nMail" );
		pMail->m_idSender	= m_qryPostProc.GetInt( "idSender" );

		if( !CPlayerDataCenter::GetInstance()->GetPlayerData( idReceiver ) )
		{
			idReceiver	= pMail->m_idSender;	// pMail->m_idSender가 없는 경우는?
			pMail->m_idSender	= 0;
		}

		int nGoldFlag	= m_qryPostProc.GetInt( "GoldFlag" );
		pMail->m_nGold	= 0;
		if( nGoldFlag == 0 )
			pMail->m_nGold	= m_qryPostProc.GetInt( "nGold" );
		pMail->m_tmCreate	= (time_t)m_qryPostProc.GetInt( "tmCreate" );
		pMail->m_byRead	= (BYTE)m_qryPostProc.GetInt( "byRead" );

		//	mulcom	BEGIN100420	
		//m_qryPostProc.GetStr( "szTitle", pMail->m_szTitle );
		char	szTempTitle[128] = { 0 };
		//mem_set( szTempTitle, 0, sizeof(szTempTitle) );
		m_qryPostProc.GetStr( "szTitle", szTempTitle, _countof( szTempTitle ) );
		
		::memcpy( pMail->m_szTitle, szTempTitle, sizeof(pMail->m_szTitle) );
		pMail->m_szTitle[MAX_MAILTITLE-1] = '\0';
		//	mulcom	END100420	


		m_qryPostProc.GetStr( "szText", pMail->m_szText, _countof( pMail->m_szText ) );

		int nItemFlag	= m_qryPostProc.GetInt( "ItemFlag" );
		DWORD dwItemId	= m_qryPostProc.GetInt( "dwItemId" );

		if( dwItemId && nItemFlag == 0 )
		{
			pMail->m_pItemElem	= new FLItemElem;
			pMail->m_pItemElem->m_dwItemId	= dwItemId;
			GetItemFromMail( &m_qryPostProc, pMail->m_pItemElem );

			if( CheckValidItem( dwItemId, pMail->m_pItemElem->m_nItemNum ) == FALSE )
			{
				nTotal++;
				SAFE_DELETE( pMail );
				continue;
			}
		}
		bool bResult	= setnMail.insert( pMail->m_nMail ).second;
		if( !bResult )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager.LoadPost: Mail Index is Duplication. nMail:[%d]" ), pMail->m_nMail );
			return FALSE;
		}

		if( FALSE == CPost::GetInstance()->AddMail( idReceiver, pMail ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager.LoadPost: AddMail Failed. idReceiver:[%07d], nMail:[%d]" ), idReceiver, pMail->m_nMail );
			return FALSE;
		}
		nTotal++;
	}

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 1;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspLoadMaxMailID", MAX_PARAMETER );

	TCHAR szServerIndex[ 3 ] = { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );


	m_qryPostProc.BindParameterChar( SQL_PARAM_INPUT, 2,	szServerIndex );

	if( m_qryPostProc.Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s" ), szQuery, szServerIndex );
		return FALSE;
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	if( m_qryPostProc.Fetch() )
	{
		int nMaxMailID	= m_qryPostProc.GetInt( "MaxMailID" );
		int nCount	= m_qryPostProc.GetInt( "nCount" );
		if( nTotal != nCount )
		{
			char szTemp[255] = {0,};
			FLSPrintf( szTemp, _countof( szTemp ), "CDbManager.LoadPost: count -> count : %d, nTotal : %d", nCount, nTotal );
			AfxMessageBox( szTemp );
			return FALSE;
		}
		if( (int)( CMail::s_nMail ) > nMaxMailID )
		{
			AfxMessageBox( "MaxMailID is not valid" );
			return FALSE;
		}
		CMail::s_nMail	= nMaxMailID;
	}
	return TRUE;
}

void CDbManager::GetItemFromMail( CQuery* pQuery, FLItemElem* pItemElem )
{
	if( pQuery->IsValidColumn( "bCharged" ) == true )
	{
		pItemElem->m_bCharged	= static_cast<BOOL>( pQuery->GetInt( "bCharged" ) == 1 );
	}

	if( pQuery->IsValidColumn( "bItemResist" ) == true )
	{
		pItemElem->m_byItemResist	= static_cast< BYTE >( pQuery->GetInt( "bItemResist" ) );
	}

	if( pQuery->IsValidColumn( "byFlag" ) == true )
	{
		pItemElem->m_byFlag			= static_cast< BYTE >( pQuery->GetInt( "byFlag" ) );
	}

	if( pQuery->IsValidColumn( "dwKeepTime" ) == true )
	{
		pItemElem->m_dwKeepTime		= pQuery->GetInt( "dwKeepTime" );
	}

	if( pQuery->IsValidColumn( "dwSerialNumber" ) == true )
	{
		pItemElem->SetSerialNumber( static_cast<DWORD>( pQuery->GetSerialNumber( "dwSerialNumber" ) ) );
	}

	if( pQuery->IsValidColumn( "idGuild" ) == true )
	{
		pItemElem->m_idGuild		= static_cast<DWORD>( pQuery->GetInt( "idGuild" ) );
	}

	if( pQuery->IsValidColumn( "nOption" ) == true )
	{
		pItemElem->SetAbilityOption( pQuery->GetInt( "nOption" ) );
	}

	if( pQuery->IsValidColumn( "nHitPoint" ) == true )
	{
		pItemElem->m_nHitPoint		= pQuery->GetInt( "nHitPoint" );
	}

	if( pQuery->IsValidColumn( "nMaxHitPoint" ) == true )
	{
		pItemElem->m_nRepair		= pQuery->GetInt( "nMaxHitPoint" );
	}

	if( pQuery->IsValidColumn( "nItemNum" ) == true )
	{
		pItemElem->m_nItemNum		= pQuery->GetInt( "nItemNum" );
	}

	__int64 n64RandomOptionId		= 0;
	if( pQuery->IsValidColumn( "nRandomOptItemId" ) == true )
	{
		n64RandomOptionId			= pQuery->GetInt64( "nRandomOptItemId" );
	}

	if( pQuery->IsValidColumn( "nRepairNumber" ) == true )
	{
		pItemElem->m_nRepairNumber			= static_cast< BYTE >( pQuery->GetInt( "nRepairNumber" ) );
	}

	if( pQuery->IsValidColumn( "nResistAbilityOption" ) == true )
	{
		pItemElem->m_nResistAbilityOption	= pQuery->GetInt( "nResistAbilityOption" );
	}

	if( pQuery->IsValidColumn( "nResistSMItemId" ) == true )
	{
		pItemElem->m_nResistSMItemId		= pQuery->GetInt( "nResistSMItemId" );
	}

	GetPiercingInfoFromMail( pQuery, pItemElem );

	if( pItemElem->m_dwItemId == ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER ) )
	{
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( pItemElem->m_nHitPoint );

		if(lpszPlayer != NULL)
		{
			FLStrcpy( pItemElem->m_szItemText, _countof( pItemElem->m_szItemText ), lpszPlayer );
		}
		CPlayerDataCenter::GetInstance()->m_Access.Leave();
	}

	if( pQuery->IsValidColumn( "bPet" ) == true )
	{
		const BOOL bPet	= static_cast<BOOL>( pQuery->GetInt( "bPet" ) == 1 );
		if( bPet )
		{
			SAFE_DELETE( pItemElem->m_pPet );
			CPet* pPet	= pItemElem->m_pPet	= new CPet;
			if( pQuery->IsValidColumn( "nKind" ) == true )
			{
				const int nKind		= pQuery->GetInt( "nKind" );
				if( nKind > 0 )
				{
					pPet->SetKind( static_cast<BYTE>( nKind ) );
				}
			}

			if( pQuery->IsValidColumn( "nLevel" ) == true )
			{
				const int nLevel	= pQuery->GetInt( "nLevel" );
				if( nLevel > 0 )
				{
					pPet->SetLevel( static_cast<BYTE>( nLevel ) );
				}
			}

			if( pQuery->IsValidColumn( "dwExp" ) == true )
			{
				const int nExp		= pQuery->GetInt( "dwExp" );
				if( nExp > 0 )
				{
					pPet->SetExp( static_cast<DWORD>( nExp ) );
				}
			}

			if( pQuery->IsValidColumn( "wEnergy" ) == true )
			{
				const int nEnergy	= pQuery->GetInt( "wEnergy" );
				if( nEnergy > 0 )
				{
					pPet->SetEnergy( static_cast<WORD>( nEnergy ) );
				}
			}

			if( pQuery->IsValidColumn( "wLife" ) == true )
			{
				const int nLife		= pQuery->GetInt( "wLife" );
				if( nLife > 0 )
				{
					pPet->SetLife( static_cast<WORD>( nLife ) );
				}
			}

			if( pQuery->IsValidColumn( "anAvailLevel_D" ) == true )
			{
				const int nAvailLevel	= pQuery->GetInt( "anAvailLevel_D" );
				if( nAvailLevel > 0 )
				{
					pPet->SetAvailLevel( PL_D, static_cast<BYTE>( nAvailLevel ) );
				}
			}

			if( pQuery->IsValidColumn( "anAvailLevel_C" ) == true )
			{
				const int nAvailLevel	= pQuery->GetInt( "anAvailLevel_C" );
				if( nAvailLevel > 0 )
				{
					pPet->SetAvailLevel( PL_C, static_cast<BYTE>( nAvailLevel ) );
				}
			}

			if( pQuery->IsValidColumn( "anAvailLevel_B" ) == true )
			{
				const int nAvailLevel	= pQuery->GetInt( "anAvailLevel_B" );
				if( nAvailLevel > 0 )
				{
					pPet->SetAvailLevel( PL_B, static_cast<BYTE>( nAvailLevel ) );
				}
			}

			if( pQuery->IsValidColumn( "anAvailLevel_A" ) == true )
			{
				const int nAvailLevel	= pQuery->GetInt( "anAvailLevel_A" );
				if( nAvailLevel > 0 )
				{
					pPet->SetAvailLevel( PL_A, static_cast<BYTE>( nAvailLevel ) );
				}
			}

			if( pQuery->IsValidColumn( "anAvailLevel_S" ) == true )
			{
				const int nAvailLevel	= pQuery->GetInt( "anAvailLevel_S" );
				if( nAvailLevel > 0 )
				{
					pPet->SetAvailLevel( PL_S, static_cast<BYTE>( nAvailLevel ) );
				}
			}

			char szPetName[MAX_PET_NAME]	= { 0,};
			if( pQuery->IsValidColumn( "szPetName" ) == true )
			{
				pQuery->GetStr( "szPetName", szPetName, _countof( szPetName ) );
				if( strcmp( szPetName, "NULL" ) == 0 )
					szPetName[0]	= '\0';
				
				pPet->SetName( szPetName );
			}
		}
	}

	if( pQuery->IsValidColumn( "nSerialNumberId" ) == true )
	{
		DWORD dwSerialNumberId = static_cast<DWORD>( pQuery->GetInt64( "nSerialNumberId" ) );
		UNREFERENCED_PARAMETER( dwSerialNumberId );
	}

	if( pQuery->IsValidColumn( "idCouplePlayer" ) == true )
	{
		const int nCouplePlayerId = pQuery->GetInt( "idCouplePlayer" );
		if( nCouplePlayerId > 0 )
		{
			pItemElem->SetCoupleId( static_cast<DWORD>( nCouplePlayerId ) );
		}
	}

	if( pQuery->IsValidColumn( "nLevelDown" ) == true )
	{
		int nLevelDown = pQuery->GetInt( "nLevelDown" );
		if( nLevelDown > 0 )
		{
			pItemElem->SetLevelDown( nLevelDown );
		}
	}

	if( pQuery->IsValidColumn( "nRandomOptionOriginId" ) == true )
	{
		const WORD wRandomOptionOriginID = static_cast<WORD>( pQuery->GetInt( "nRandomOptionOriginId" ) );
		pItemElem->SetRandomOptionOriginID( wRandomOptionOriginID );
	}

	if( pQuery->IsValidColumn( "nRandomOptionExtFlag" ) == true )
	{
		const WORD wRandomOptionExtFlag = static_cast<WORD>( pQuery->GetInt( "nRandomOptionExtFlag" ) );
		pItemElem->SetRandomOptionExtensionFlag( wRandomOptionExtFlag );
	}

	if( pQuery->IsValidColumn( "nRandomOptionExtSize" ) == true )
	{
		const int nRandomOptionExtSize = pQuery->GetInt( "nRandomOptionExtSize" );
		UNREFERENCED_PARAMETER( nRandomOptionExtSize );
	}
	
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		TCHAR column_DST[64] = { 0 };
		FLSPrintf( column_DST, _countof( column_DST ), "%s%d", "nRandomOptionExtDst", Nth + 1 );
		TCHAR column_ADJ[64] = { 0 };
		FLSPrintf( column_ADJ, _countof( column_ADJ ), "%s%d", "nRandomOptionExtAdj", Nth + 1 );
		if( pQuery->IsValidColumn( column_DST ) && pQuery->IsValidColumn( column_ADJ ) )
		{
			pItemElem->SetRandomOptionExtension( DB_RANDOMOPTION_DST( pQuery->GetInt( column_DST ) ), DB_RANDOMOPTION_ADJ( pQuery->GetInt( column_ADJ ) ) );
		}
	}

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		if( pQuery->IsValidColumn( "dwLooksChangeItemID" ) == true )
		{
			const DWORD dwLooksChangeItemID	= static_cast<DWORD>( pQuery->GetInt( "dwLooksChangeItemID" ) );
			pItemElem->SetLooksChangeItemID( dwLooksChangeItemID );
		}

		if( pQuery->IsValidColumn( "bIsCombined" ) == true )
		{
			const bool bIsCombined = static_cast<bool>( pQuery->GetInt( "bIsCombined" ) == 1 );
			pItemElem->SetCombinedOption( bIsCombined );
		}

		if( pQuery->IsValidColumn( "dwCombinedAddDamage" ) == true )
		{
			const DWORD dwCombinedAddDamage	= static_cast<DWORD>( pQuery->GetInt( "dwCombinedAddDamage" ) );
			pItemElem->SetCombinedAddDamage( dwCombinedAddDamage );
		}

		if( pQuery->IsValidColumn( "wCombinedRandomOptionOriginID" ) == true )
		{
			const WORD wCombinedRandomOptionOriginID	= static_cast<WORD>( pQuery->GetInt( "wCombinedRandomOptionOriginID" ) );
			pItemElem->SetCombinedRandomOptionOriginID( wCombinedRandomOptionOriginID );
		}

		if( pQuery->IsValidColumn( "nCombinedGeneralPiercingSize" ) == true )
		{
			const size_t nCombinedGeneralPiercingSize	= static_cast<size_t>( pQuery->GetInt( "nCombinedGeneralPiercingSize" ) );
			pItemElem->SetCombinedGeneralPiercingSize( nCombinedGeneralPiercingSize );
		}

		for( size_t Nth = 0; Nth < pItemElem->GetCombinedGeneralPiercingSize(); ++Nth )
		{
			TCHAR szColumn[64] = { 0 };
			FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "dwCombinedGeneralPiercingID", ( Nth + 1 ) );
			if( pQuery->IsValidColumn( szColumn ) == true )
			{
				const DWORD dwItemID = static_cast<DWORD>( pQuery->GetInt( szColumn ) );
				pItemElem->SetCombinedGeneralPiercingItemID( Nth, dwItemID );
			}
		}

		if( pQuery->IsValidColumn( "nCombinedRandomOptionExtensionSize" ) == true )
		{
			const int nCombinedRandomOptionExtensionSize = pQuery->GetInt( "nCombinedRandomOptionExtensionSize" );
			UNREFERENCED_PARAMETER( nCombinedRandomOptionExtensionSize );
		}

		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			TCHAR column_DST[64] = { 0 };
			FLSPrintf( column_DST, _countof( column_DST ), "%s%d", "nCombinedRandomOptionExtDst", Nth + 1 );
			TCHAR column_ADJ[64] = { 0 };
			FLSPrintf( column_ADJ, _countof( column_ADJ ), "%s%d", "nCombinedRandomOptionExtAdj", Nth + 1 );
			if( pQuery->IsValidColumn( column_DST ) && pQuery->IsValidColumn( column_ADJ ) )
			{
				pItemElem->SetCombinedRandomOptionExtension( DB_RANDOMOPTION_DST( pQuery->GetInt( column_DST ) ), DB_RANDOMOPTION_ADJ( pQuery->GetInt( column_ADJ ) ) );
			}
		}
	}

	if( n64RandomOptionId != 0 && pItemElem->IsSetRandomOptionExtension() == false )
	{
		pItemElem->RandomOptionDataConverting( n64RandomOptionId );
	}
}

void CDbManager::AddMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver	= NULL_PLAYER_ID;
	u_long idSender		= NULL_PLAYER_ID;
	BYTE byItem			= 0;
	FLItemElem kItemElem;
	int nGold			= 0;
	TCHAR szMailTitle[MAX_MAILTITLE]	= { 0, };
	TCHAR szMailText[MAX_MAILTEXT]		= { 0, };

	ar >> idReceiver >> idSender >> byItem;
	
	if( byItem )
	{
		kItemElem.Deserialize( ar );
	}

	ar >> nGold;
	ar.ReadString( szMailTitle, _countof( szMailTitle ) );
	ar.ReadString( szMailText, _countof( szMailText ) );

	PostMail( *pQuery, idReceiver, idSender, kItemElem, nGold, szMailTitle, szMailText );
}


void	CDbManager::PostMail( CQuery & kQuery, const u_long uReceiver, const u_long uSender, FLItemElem & kItemElem, const int nGold, const char * szTitle, const char * szText )
{
	CMail* pMail	= NULL;

	try
	{
		pMail	= new CMail;
		pMail->m_tmCreate		= time( NULL );
		pMail->m_idSender		= uSender;
		if( kItemElem.IsEmpty() == FALSE )
		{
			pMail->m_pItemElem	= new FLItemElem;
			*pMail->m_pItemElem	= kItemElem;
		}
		pMail->m_nGold			= nGold;
		FLStrcpy( pMail->m_szTitle, _countof( pMail->m_szTitle ), szTitle );
		FLStrcpy( pMail->m_szText, _countof( pMail->m_szText ), szText );


		CPost *pPost    = CPost::GetInstance();

		//	CMclAutoLock	Lock( pPost->m_csPost );
		pPost->m_csPost.Enter();
		BOOL bResult	= CPost::GetInstance()->AddMail( uReceiver, pMail ) > 0;
		pPost->m_csPost.Leave();

		//////////////////////////////////////////////////////////////////////////
		// BEGIN110119

		if( bResult )
		{
			T_MAIL_STR_INFO tMail;

			FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "A1" ) );
			FLStrcpy( tMail.szMailTitle, _countof( tMail.szMailTitle ), pMail->m_szTitle );
			FLStrcpy( tMail.szMailText, _countof( tMail.szMailText ), pMail->m_szText );
			tMail.dwReceivePlayerId = uReceiver;
			tMail.CopyMailInfo( pMail );

			if( DbQryMail( &kQuery, &tMail ) == false )
			{
				CDPTrans::GetInstance()->SendPostMail( FALSE, uReceiver, pMail );
				pPost->m_csPost.Enter();
				CMailBox* pMailBox	= pPost->GetMailBox( uReceiver );
				pMailBox->RemoveMail( pMail->m_nMail );
				pPost->m_csPost.Leave();
				return;
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "AddMail false. idSender : %d, pMail->m_nMail : %d" ), pMail->m_idSender, pMail->m_nMail );
			CDPTrans::GetInstance()->SendPostMail( FALSE, uReceiver, pMail );
			pPost->m_csPost.Enter();
			CMailBox* pMailBox	= pPost->GetMailBox( uReceiver );
			pMailBox->RemoveMail( pMail->m_nMail );
			pPost->m_csPost.Leave();
			return;
		}

		CDPTrans::GetInstance()->SendPostMail( TRUE, uReceiver, pMail );
	}
	catch( ... )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ CMail Alloc Failed. Sender(%07d), Receiver(%07d), ItemID(%d), ItemNum(%d), Gold(%d), Title(%s), Text(%s) ]" )
			, uSender, uReceiver, kItemElem.m_dwItemId, kItemElem.m_nItemNum, nGold, szTitle, szText );
		SAFE_DELETE( pMail );
	}
}

void CDbManager::RemoveMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;

	CPost *pPost    = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->RemoveMail( nMail );
		pPost->m_csPost.Leave();	// u

		//////////////////////////////////////////////////////////////////////////
		// BEGIN110119

		T_MAIL_STR_INFO tMail;

		FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "D1" ) );
		tMail.dwMailId = nMail;

		if( DbQryMail( pQuery, &tMail ) == true )
		{
			CDPTrans::GetInstance()->SendRemoveMail( idReceiver, nMail );
		}

		// END110119
		//////////////////////////////////////////////////////////////////////////
	}
	else
		pPost->m_csPost.Leave();	// u
}

void CDbManager::RemoveMail(std::list<CMail*> & lspMail )
{
	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	for(std::list<CMail*>::iterator i = lspMail.begin(); i != lspMail.end(); ++i )
	{
		CMail* pMail	= *i;

		T_MAIL_STR_INFO tMail;

		FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "D1" ) );
		tMail.dwMailId = pMail->m_nMail;
		
		if( DbQryMail( &m_qryPostProc, &tMail ) == true )
		{
			CMailBox* pMailBox	= pMail->GetMailBox();
			pMailBox->RemoveMail( tMail.dwMailId );
			CDPTrans::GetInstance()->SendRemoveMail( pMailBox->m_idReceiver, tMail.dwMailId );
		}
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}

void CDbManager::RemoveMailItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;

	CPost* pPost = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->RemoveMailItem( nMail );
		pPost->m_csPost.Leave();	// u

		//////////////////////////////////////////////////////////////////////////
		// BEGIN110119

		T_MAIL_STR_INFO tMail;

		FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "U1" ) );
		tMail.dwMailId = nMail;

		if( DbQryMail( pQuery, &tMail ) == true )
		{
			CDPTrans::GetInstance()->SendGetMailItem( idReceiver, nMail, uQuery );
		}

		// END110119
		//////////////////////////////////////////////////////////////////////////
	}
	else
		pPost->m_csPost.Leave();	// u
}

void CDbManager::RemoveMailGold( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;

	CPost* pPost = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->RemoveMailGold( nMail );
		pPost->m_csPost.Leave();	// u

		//////////////////////////////////////////////////////////////////////////
		// BEGIN110119

		T_MAIL_STR_INFO tMail;

		FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "U2" ) );
		tMail.dwMailId = nMail;

		if( DbQryMail( pQuery, &tMail ) == true )
		{
			CDPTrans::GetInstance()->SendGetMailGold( idReceiver, nMail, uQuery );
		}

		// END110119
		//////////////////////////////////////////////////////////////////////////
	}
	else
		pPost->m_csPost.Leave();	// u
}

void CDbManager::ReadMail( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;

	CPost* pPost = CPost::GetInstance();
//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox	= pPost->GetMailBox( idReceiver );
	if( pMailBox )
	{
		pMailBox->ReadMail( nMail );
		pPost->m_csPost.Leave();	// u

		//////////////////////////////////////////////////////////////////////////
		// BEGIN110119

		T_MAIL_STR_INFO tMail;

		FLStrcpy( tMail.szGu, _countof( tMail.szGu ), _T( "U3" ) );
		tMail.dwMailId = nMail;

		if( DbQryMail( pQuery, &tMail ) == true )
		{
			CDPTrans::GetInstance()->SendReadMail( idReceiver, nMail );
		}

		// END110119
		//////////////////////////////////////////////////////////////////////////
	}
	else
	{
		pPost->m_csPost.Leave();	// u
		FLERROR_LOG( PROGRAM_NAME, _T( "pMailBox is NULL. idReceiver : %07d" ), idReceiver );
	}
}

int	CDbManager::GetTradeNo( void )
{
	time_t	t	= time( NULL );
	t	&= 0xffffffe0;
	LONG lCount	= InterlockedIncrement( &m_cbTrade );
	lCount	&= 0x0000001f;
	return (int)( t + lCount );
}

CQuery* CDbManager::CreateCharQuery( void )
{
	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( pQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01 ) )
#endif
		return pQuery;
	SAFE_DELETE( pQuery );
	return NULL;
}

BOOL CDbManager::QueryGetMailRealTime( CQuery* pQuery )
{
	if( NULL == pQuery )
		return FALSE;

	char szQuery[MAX_QUERY_SIZE]	= { 0, };
	FLSPrintf( szQuery, _countof( szQuery ), "MAIL_STR_REALTIME 'S1', '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return FALSE;
	}

	__MAIL_REALTIME OneMail;
	std::vector< __MAIL_REALTIME > vecMailRT;

	while( pQuery->Fetch() )
	{
		u_long idReceiver	= pQuery->GetInt( "idReceiver" );
		CMail* pMail	= new CMail;
		OneMail.nMail_Before = pQuery->GetInt( "nMail" );
		pMail->m_idSender	= pQuery->GetInt( "idSender" );
		int nGoldFlag	= pQuery->GetInt( "GoldFlag" );
		pMail->m_nGold	= 0;
		if( nGoldFlag == 0 )
			pMail->m_nGold	= pQuery->GetInt( "nGold" );
		pMail->m_tmCreate	= (time_t)pQuery->GetInt( "tmCreate" );
		pMail->m_byRead	= (BYTE)pQuery->GetInt( "byRead" );

		//	mulcom	BEGIN100420	
		//pQuery->GetStr( "szTitle", pMail->m_szTitle );
		char	szTempTitle[128] = { 0 };
		pQuery->GetStr( "szTitle", szTempTitle, _countof( szTempTitle ) );

		::memcpy( pMail->m_szTitle, szTempTitle, sizeof(pMail->m_szTitle) );
		pMail->m_szTitle[MAX_MAILTITLE-1] = '\0';
		//	mulcom	END100420	

		pQuery->GetStr( "szText", pMail->m_szText, _countof( pMail->m_szText ) );

		int nItemFlag	= pQuery->GetInt( "ItemFlag" );
		DWORD dwItemId	= pQuery->GetInt( "dwItemId" );

		if( dwItemId && nItemFlag == 0 )
		{
			PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( dwItemId );
			if( !pItemProp )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Not SkillProp = %d" ), dwItemId );
				continue;
			}
			pMail->m_pItemElem	= new FLItemElem;
			pMail->m_pItemElem->m_dwItemId	= dwItemId;
			GetItemFromMail( pQuery, pMail->m_pItemElem );
			
			if( CheckValidItem( dwItemId, pMail->m_pItemElem->m_nItemNum ) == FALSE )
			{
				SAFE_DELETE( pMail );
				continue;
			}

			CPost *pPost    = CPost::GetInstance();

			pPost->m_csPost.Enter();
			OneMail.nMail_After	= CPost::GetInstance()->AddMail( idReceiver, pMail );

			pPost->m_csPost.Leave();
			CDPTrans::GetInstance()->SendPostMail( TRUE, idReceiver, pMail );

			vecMailRT.push_back( OneMail );
		}
	}

	for( DWORD i = 0 ; i < vecMailRT.size() ; ++i )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "MAIL_STR_REALTIME 'U1', '%02d', %d, %d, '%07d', %d, '%s', %d, %d, %d", 
				g_appInfo.dwSys, vecMailRT[i].nMail_Before, vecMailRT[i].nMail_After, 0, 0, "", 0, vecMailRT[i].m_iSerialNumber, vecMailRT[i].m_nHitPoint );

		if( FALSE == pQuery->Exec( szQuery ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CDbManager::QueryRemoveGuildBankTbl( void )
{
	if( NULL == m_apQuery[0] )
		return FALSE;
/*
	uspLoadRemoveItemFromGuildBank
	Parameter :
		@pserverindex char(2)
	Return : 
		idGuild char(6),
		nNum int,
		ItemIndex varchar(32),
		ItemSerialNum int,
		ItemCnt int,
		DeleteCnt int
*/
	char pszQuery[100]	= { 0, };
	FLSPrintf( pszQuery, _countof( pszQuery ), "uspLoadRemoveItemFromGuildBank @pserverindex = '%02d'", g_appInfo.dwSys );
	if( FALSE == m_apQuery[0]->Exec( pszQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "query: %s" ), pszQuery );
		return FALSE;
	}
	while( m_apQuery[0]->Fetch() )
	{
		int nNo	= (int)m_apQuery[0]->GetInt( "nNum" );
		u_long idGuild	= (u_long)m_apQuery[0]->GetInt( "idGuild" );

		char pszItemIndex[100]	= { 0, };
		m_apQuery[0]->GetStr( "ItemIndex", pszItemIndex, _countof( pszItemIndex ) );
		DWORD dwItemId	= 0;
		if( lstrcmp( pszItemIndex, "penya" ) )	// item
			dwItemId	= atoi( pszItemIndex );
		SERIALNUMBER iSerialNumber	= (SERIALNUMBER)m_apQuery[0]->GetSerialNumber( "ItemSerialNum" );
		DWORD dwItemNum	= (DWORD)m_apQuery[0]->GetInt( "DeleteRequestCnt" );
		CDPTrans::GetInstance()->SendQueryRemoveGuildBankTbl( nNo, idGuild, dwItemId, iSerialNumber, dwItemNum );
#ifdef _DEBUG
		FLTRACE_LOG( PROGRAM_NAME, _T( "TRANS: QueryRemoveGuildBankTbl: nNo = %d, idGuild=%d, dwItemId=%d, liSerialNumber=%d, nItemNum=%d" ), nNo, idGuild, dwItemId, iSerialNumber, dwItemNum );
#endif	// _DEBUG
	}
	return TRUE;
}

void CDbManager::DeleteRemoveGuildBankTbl( CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpov )
{
	int nNo;
	DWORD dwRemoved;
	CAr ar( lpov->lpBuf, lpov->uBufSize );
	ar >> nNo >> dwRemoved;
/*
	uspRemoveItemFromGuildBank:
		Parameter :
			@pserverindex char(2)
			@pNum int
		Return :
			0 : not exist
			1 : success
*/
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspRemoveItemFromGuildBank @pserverindex = '%02d', @pNum = %d, @pDeleteCnt=%d", g_appInfo.dwSys, nNo, dwRemoved );

	FLTRACE_LOG( PROGRAM_NAME, szQuery );

	if( FALSE == pQueryChar->Exec( szQuery ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
}

void CDbManager::call_uspLoggingQuest( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer;
	int nQuest, nState;
	char pszComment[100]	= { 0, };
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> nQuest >> nState;
	ar.ReadString( pszComment, _countof( pszComment ) );
/*
	uspLoggingQuest:
			Parameter :
          @pserverindex
           @pidPlayer
           @pQuestIndex
           @pState
           @Comment
		Return :
*/
	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspLoggingQuest @pserverindex='%02d', @pidPlayer='%07d', @pQuestIndex=%d, @pState=%d, @pComment='%s'", g_appInfo.dwSys, idPlayer, nQuest, nState, pszComment );

	FLTRACE_LOG( PROGRAM_NAME, szQuery );

	if( FALSE == pQuery->Exec( szQuery ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
}

void CDbManager::call_uspXXXMultiServer( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long uKey, idPlayer;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> uKey >> idPlayer;

	char szQuery[MAX_QUERY_SIZE]	= { 0,};

	if( uKey > 0 )
	{
		int i	= (int)uKey;// / 100;
		if( idPlayer > 0 )	//			uspChangeMultiServer @pserverindex(char), @pidPlayer (char), @pMultiServer(int)
			FLSPrintf( szQuery, _countof( szQuery ), "uspChangeMultiServer @pserverindex='%02d', @pidPlayer='%07d', @pMultiServer=%d", g_appInfo.dwSys, idPlayer, i );
		else	// uspInitMultiServer @pserverindex(char), @pMultiServer(int)
			FLSPrintf( szQuery, _countof( szQuery ), "uspInitMultiServer @pserverindex='%02d', @pMultiServer=%d", g_appInfo.dwSys, i );
	}
	else
	{
		if( idPlayer > 0 )	// o
			FLSPrintf( szQuery, _countof( szQuery ), "uspChangeMultiServer @pserverindex='%02d', @pidPlayer='%07d', @pMultiServer=%d", g_appInfo.dwSys, idPlayer, 0 );
	}

	FLTRACE_LOG( PROGRAM_NAME, szQuery );

	if( FALSE == pQuery->Exec( szQuery ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
}

DB_OVERLAPPED_PLUS*		CDbManager::AllocRequest( void )
{
	//	return	m_pDbIOData->Alloc();
	DB_OVERLAPPED_PLUS* ptr	= m_pDbIOData->Alloc();
	memset( ptr, 0, sizeof(*ptr) );
	return ptr;
}
void	CDbManager::FreeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus )
{
	SAFE_DELETE_ARRAY( pOverlappedPlus->lpBuf );
	m_pDbIOData->Free( pOverlappedPlus );
}

void	CDbManager::MakeRequest( DB_OVERLAPPED_PLUS* pOverlappedPlus,  LPBYTE lpBuf, u_long uBufSize )
{
	//	mulcom	BEGIN100415
	if( uBufSize < 0 || uBufSize >= (1024*1024) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "uBufSize Error. [%d]" ), uBufSize );
		
		uBufSize = 0;
	}
	//	mulcom	END100415

	pOverlappedPlus->lpBuf	= new BYTE[uBufSize];
	memset( pOverlappedPlus->lpBuf, 0, sizeof( BYTE ) * uBufSize );

	memcpy( pOverlappedPlus->lpBuf, lpBuf, uBufSize );
	pOverlappedPlus->uBufSize	= uBufSize;

	return;
}

void CDbManager::CalluspPetLog( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	CAr ar( pov->lpBuf, pov->uBufSize );
	
	u_long idPlayer;
	SERIALNUMBER iSerial;
	DWORD dwData;
	int nType;
	CPet pet;

	ar >> idPlayer >> iSerial >> dwData >> nType;
	pet.Serialize( ar );

	char szQuery[MAX_QUERY_SIZE]	= { 0,};
	FLSPrintf( szQuery, _countof( szQuery ), "uspPetLog "
		"@serverindex='%02d', @idPlayer='%07d', @iSerial=%d, @nType=%d, @dwData=%d, @nKind=%d,"
		"@nLevel=%d, @wLife=%d, @wEnergy=%d, @dwExp=%d,"
		"@nAvailParam1=%d, @nAvailParam2=%d, @nAvailParam3=%d, @nAvailParam4=%d, @nAvailParam5=%d",
		g_appInfo.dwSys, idPlayer, iSerial, nType, dwData, pet.GetKind(), pet.GetLevel(), pet.GetLife(), pet.GetEnergy(), pet.GetExp(),
		pet.GetAvailLevel( PL_D ), pet.GetAvailLevel( PL_C ), pet.GetAvailLevel( PL_B ), pet.GetAvailLevel( PL_A ), pet.GetAvailLevel( PL_S )
	 );

	FLTRACE_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );

	pQuery->Exec( szQuery );
}

void	CDbManager::LoadMessenger( CMover* pMover, CQuery* pQuery )
{
	pMover->m_RTMessenger.clear();
	char szSQL[100]	= { 0,};
	FLSPrintf( szSQL, _countof( szSQL ), "uspLoadMessenger '%02d', '%07d'" , g_appInfo.dwSys, pMover->m_idPlayer );
	if( !pQuery->Exec( szSQL ) )
		return;
	u_long idFriend;
	Friend f;
	int nSize	= 0;
	while( pQuery->Fetch() )
	{
		idFriend	= (u_long)pQuery->GetInt( "idFriend" );
		f.bBlock	= (DWORD)pQuery->GetInt( "bBlock" );
		if( ++nSize > MAX_FRIEND )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "TOO MANY FRIEND: %07d" ), pMover->m_idPlayer );
			break;
		}
		pMover->m_RTMessenger.SetFriend( idFriend, &f );
	}
}

void CDbManager::AddMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer, idFriend;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> idFriend;
	// uspAddMessenger '01', '0000001', '0000002'

	char szSQL[100]	= { 0,};
	FLSPrintf( szSQL, _countof( szSQL ), "uspAddMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, idFriend );
	if( !pQuery->Exec( szSQL ) )
	{
		// error
	}
}

void CDbManager::DeleteMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer, idFriend;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> idFriend;
	// uspDeleteMessenger '01', '0000001', '0000002'

	char szSQL[100]	= { 0,};
	FLSPrintf( szSQL, _countof( szSQL ), "uspDeleteMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, idFriend );
	if( !pQuery->Exec( szSQL ) )
	{
		// error
	}
}

void CDbManager::UpdateMessenger( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov )
{
	u_long idPlayer, idFriend;
	BOOL bBlock;
	CAr ar( pov->lpBuf, pov->uBufSize );
	ar >> idPlayer >> idFriend >> bBlock;
	// uspUpdateMessenger '01', '0000001', '0000002, %d'

	char szSQL[100]	= { 0,};
	FLSPrintf( szSQL, _countof( szSQL ), "uspUpdateMessenger '%02d', '%07d', '%07d', %d", g_appInfo.dwSys, idPlayer, idFriend, bBlock );
	if( !pQuery->Exec( szSQL ) )
	{
		// error
	}
}

void CDbManager::LoadGC1to1TenderGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS /*lpDbOverlappedPlus*/, DPID dpId )
{
	std::vector< CGuildCombat1to1Mng::__GC1TO1TENDER > vecTenderGuild;
	std::vector< CGuildCombat1to1Mng::__GC1TO1TENDER > vecTenderFailGuild;
	
	char szQuery[MAX_QUERY_SIZE] = {0,};
	// 인덱스 얻어오기
	FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_CombatID '%02d'", g_appInfo.dwSys );
	if( FALSE == pQuery->Exec( szQuery ) )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
	
	if( pQuery->Fetch() )
	{
		prj.m_GuildCombat1to1.m_nCombatId = pQuery->GetInt( "CombatID" );	// 가장 최근 combatId
		
		// 입찰 길드
		FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', '%s'",
				g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, 0, 0, ' ', "S1" );
		if( FALSE == pQuery->Exec( szQuery ) )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
		while( pQuery->Fetch() )	// 입찰 길드 목록
		{
			CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1Tender;
			gc1to1Tender.ulGuildId = pQuery->GetInt( "m_idGuild" );
			gc1to1Tender.nPenya = pQuery->GetInt( "m_nPenya" );

			vecTenderGuild.push_back( gc1to1Tender );
		}
		for( DWORD i=0; i<vecTenderGuild.size(); i++ )	// 입찰 길드 선수 목록
		{
			FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
					g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, vecTenderGuild.at( i ).ulGuildId, 0, 0, ' ', "S1" );
			if( FALSE == pQuery->Exec( szQuery ) )
			{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
			while( pQuery->Fetch() )
				vecTenderGuild.at( i ).vecMemberId.push_back( pQuery->GetInt( "m_idPlayer" ) );
		}

		// 입찰 실패 길드
		FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', '%s'",
				g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, 0, 0, ' ', "S2" );
		if( FALSE == pQuery->Exec( szQuery ) )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
		while( pQuery->Fetch() )
		{
			CGuildCombat1to1Mng::__GC1TO1TENDER gc1to1TenderFail;
			gc1to1TenderFail.ulGuildId = pQuery->GetInt( "m_idGuild" );
			gc1to1TenderFail.nPenya = pQuery->GetInt( "m_nPenya" );

			vecTenderFailGuild.push_back( gc1to1TenderFail );
		}
		
		if( vecTenderGuild.size() == 0 )
			prj.m_GuildCombat1to1.m_nCombatId++;
		
		CDPTrans::GetInstance()->SendGC1to1TenderGuild( vecTenderGuild, vecTenderFailGuild, dpId );
	}
}

void CDbManager::GC1to1State( CQuery* /*pQuery*/, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar >> prj.m_GuildCombat1to1.m_nState;
	if( prj.m_GuildCombat1to1.m_nState == CGuildCombat1to1Mng::GC1TO1_CLOSE )
		prj.m_GuildCombat1to1.m_nCombatId++;
}

void CDbManager::GC1to1Tender( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	char	cGu;
	u_long	uIdGuild;
	int		nPenya;
	char	cState;
	ar >> cGu >> uIdGuild >> nPenya >> cState;

	char szQuery[MAX_QUERY_SIZE] = {0,};
	
	switch( cGu )
	{
		case 'I' :
			FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', 'I1'",
					g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, nPenya, cState );
			break;

		case 'U' :
			FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Tender '%02d', %d, '%06d', %d, '%c', 'U1'",
					g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, nPenya, cState );
			break;
	}

	if( pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }	
}

void CDbManager::GC1to1LineUp( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long	uIdGuild, uPlayerId;
	int nVecSize;
	std::vector<u_long> vecMemberId;
	
	ar >> uIdGuild >> nVecSize;
	for( int i=0; i<nVecSize; i++ )
	{
		ar >> uPlayerId;
		vecMemberId.push_back( uPlayerId );
	}

	char szQuery[MAX_QUERY_SIZE] = {0,};
	FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, 0, 0, ' ', "D1" );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }

	for( DWORD i=0; i<vecMemberId.size(); i++ )
	{
		FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, vecMemberId.at( i ), i+1, 'N', "I1" );
		if( pQuery->Exec( szQuery ) == FALSE )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
	}
}

void CDbManager::GC1to1WarPerson( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	u_long	uIdGuild, uPlayerId;
	char cState;
		
	ar >> uIdGuild >> uPlayerId >> cState;
	
	char szQuery[MAX_QUERY_SIZE] = {0,};
	FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Battle_Person '%02d', %d, '%06d', '%07d', %d, '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, uIdGuild, uPlayerId, 0, cState, "U1" );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
}

void CDbManager::GC1to1WarGuild( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	DWORD dwWorldId;
	u_long uGuildId_0, uGuildId_1;
	char cState;
		
	ar >> dwWorldId >> uGuildId_0 >> uGuildId_1 >> cState;
	
	char szQuery[MAX_QUERY_SIZE] = {0,};
	switch( cState )
	{
		case 'W':
			FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Battle '%02d', %d, %d, '%06d', '%06d', '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, dwWorldId, uGuildId_0, uGuildId_1, cState, "I1" );
			break;

		case 'E':
			FLSPrintf( szQuery, _countof( szQuery ), "usp_Guild_Combat_1to1_Battle '%02d', %d, %d, '%06d', '%06d', '%c', '%s'",
						g_appInfo.dwSys, prj.m_GuildCombat1to1.m_nCombatId, dwWorldId, uGuildId_0, uGuildId_1, cState, "U1" );
			break;
	}
	
	if( pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
}

void CDbManager::GuildBankLogView( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	BYTE	byListType;
	u_long	idReceiver,g_uIdofMulti;
	DWORD	idGuild;

	ar >> idReceiver;
	ar >> idGuild;
	ar >> byListType;
	ar >> g_uIdofMulti;			

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	char szQuery[MAX_QUERY_SIZE] = {0,};
	switch( byListType )
	{
		case GI_LOG_VIEW_ITEM_ADD:
			FLSPrintf( szQuery, _countof( szQuery ), "dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S1", idGuild ,g_appInfo.dwSys );
			break;
		case GI_LOG_VIEW_ITEM_REMOVE:
			FLSPrintf( szQuery, _countof( szQuery ), "dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S2", idGuild ,g_appInfo.dwSys );
			break;
		case GI_LOG_VIEW_MONEY_ADD:
			FLSPrintf( szQuery, _countof( szQuery ), "dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S3", idGuild ,g_appInfo.dwSys );
			break;
		case GI_LOG_VIEW_MONEY_REMOVE:
			FLSPrintf( szQuery, _countof( szQuery ), "dbo.usp_guildbank_log_view '%s','%06d','%02d'",
						 "S4", idGuild ,g_appInfo.dwSys );
			break;
	}
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
	}
	else
	{
//		BEFORESENDSOLE( out, PACKETTYPE_GUILDLOG_VIEW, dpidMulti);
		BEFORESENDDUAL( out, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN, DPID_UNKNOWN);
		int		nCount = 0,m_Item = 0,Item_count = 0,nAbilityOption = 0;
		CTime	date;

		u_long	idPlayer;

		ptrdiff_t nCountTag = out.ReserveSpace( sizeof( nCount ) );
		out << byListType;	
		out << idReceiver;	

		char szDate[32] = {0,};
		while( pQuery->Fetch() )
		{
			idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
			char szSender[MAX_PLAYER]	= "unknown";

			CPlayerDataCenter::GetInstance()->m_Access.Enter();
			const char* pszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer );
			if( pszPlayer )
				FLStrcpy( szSender, _countof( szSender ), pszPlayer );
			CPlayerDataCenter::GetInstance()->m_Access.Leave();

			pQuery->GetStr( "s_date", szDate, _countof( szDate ) );				//  
			m_Item = pQuery->GetInt( "m_Item" );		// 
			nAbilityOption = pQuery->GetInt( "m_nAbilityOption" );		// 
			Item_count = pQuery->GetInt( "Item_count" );		// 

			GetStrTime( &date, szDate );

			out.WriteString( szSender );
			out << (long)date.GetTime(); 
			out << m_Item;
			out << nAbilityOption;
			out << Item_count;

			nCount++;
		}

		out.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
//		SEND( out, CDPTrans::GetInstance(), g_uIdofMulti );
	}
}

// 봉인을 하겠다고 요청. 계정 내 캐릭이 2개 이상 있는지 체크.
void CDbManager::SealChar( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;
	TCHAR	lpszAccount[MAX_ACCOUNT];

	ar >> idReceiver;
	ar >> g_uIdofMulti;			
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 2;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspLoadSealChar", MAX_PARAMETER );

	TCHAR szServerIndex[ 3 ] = { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 32,		 lpszAccount );


// 	FLSPrintf( szQuery, _countof( szQuery ), "uspLoadSealChar '%02d','%s'",
// 						 g_appInfo.dwSys, lpszAccount);
	
	if( pQuery->Exec( szQuery ) == TRUE )
	{ 
		BEFORESENDDUAL( out, PACKETTYPE_SEALCHAR_REQ, DPID_UNKNOWN, DPID_UNKNOWN);
		int		nCount = 0;
		u_long	idPlayer,playerslot;

		ptrdiff_t nCountTag = out.ReserveSpace( sizeof( nCount ) );
		out << idReceiver;

//		char szDate[13] = {0,};
		while( pQuery->Fetch() )
		{
			playerslot = pQuery->GetInt( "playerslot" );		// 

			if( playerslot >= MAX_CHARACTER_SLOT )
			{
				break;
			}

			idPlayer = pQuery->GetInt( "m_idPlayer" );		// 
			char szName[MAX_PLAYER]	= "unknown";
			pQuery->GetStr( "m_szName", szName, _countof( szName ) );				//  
			out << playerslot;
			out << idPlayer;
			out.WriteString( szName );

			nCount++;
		}

		out.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s" ), szQuery, szServerIndex, lpszAccount );
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}
void CDbManager::SealCharConm( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idPlayer, g_uIdofMulti;

	ar >> idPlayer;
	ar >> g_uIdofMulti;			

//	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 2;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspSetSealChar", MAX_PARAMETER );

	TCHAR szServerIndex[ 3 ] = { 0, };
	TCHAR szPlayerId[ 8 ] = { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", idPlayer );

	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		2,		 szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		7,		 szPlayerId );

// 	FLSPrintf( szQuery, _countof( szQuery ), "uspSetSealChar '%02d','%07d'",
// 						 g_appInfo.dwSys, idReceiver);
	
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s" ), szQuery, szServerIndex, szPlayerId );
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}
void CDbManager::SealCharGet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;
	TCHAR	lpszAccount[MAX_ACCOUNT];
	DWORD	dwGetID;

	ar >> idReceiver;
	ar >> g_uIdofMulti;			
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> dwGetID;

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 2;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspLoadSealChar", MAX_PARAMETER );

	TCHAR szServerIndex[ 3 ] = { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		2,		 szServerIndex );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 		32,		 lpszAccount );

// 	FLSPrintf( szQuery, _countof( szQuery ), "uspLoadSealChar '%02d','%s'",
// 						 g_appInfo.dwSys, lpszAccount);

	if( pQuery->Exec( szQuery ) == TRUE )
	{
		BEFORESENDDUAL( out, PACKETTYPE_SEALCHARGET_REQ, DPID_UNKNOWN, DPID_UNKNOWN);
		(nBufSize);
		int		nCount = 0;
		u_long	idPlayer,playerslot;

		ptrdiff_t nCountTag = out.ReserveSpace( sizeof( nCount ) );
		out << idReceiver;	
		out << dwGetID;	

//		char szDate[13] = {0,};
		while( pQuery->Fetch() )
		{
			playerslot = pQuery->GetInt( "playerslot" );

			if( playerslot >= MAX_CHARACTER_SLOT )
			{
				break;
			}
			idPlayer = pQuery->GetInt( "m_idPlayer" );
			char szName[MAX_PLAYER]	= "unknown";
			pQuery->GetStr( "m_szName", szName, _countof( szName ) );
			out << playerslot;
			out << idPlayer;
			out.WriteString( szName );

			nCount++;
		}

		out.WriteReservedSpace( nCountTag, &nCount, sizeof( nCount ) );
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s" ), szQuery, szServerIndex, lpszAccount );
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}
void CDbManager::SealCharSet( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	u_long	idReceiver,g_uIdofMulti;
	LONG	lSetPlayerSlot;
	TCHAR	lpszAccount[MAX_ACCOUNT];
	DWORD	dwSealPlayerID;
	ar >> idReceiver;
	ar >> g_uIdofMulti;			
	ar.ReadString( lpszAccount, _countof( lpszAccount ) );
	ar >> lSetPlayerSlot;			
	ar >> dwSealPlayerID;			

//	DPID	dpidMulti = lpDbOverlappedPlus->dpid;

	
	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 5;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspSetSealCharUpdate", MAX_PARAMETER );

	TCHAR szServerIndex[ 3 ] = { 0, };
	TCHAR szPlayerId[ 8 ] = { 0, };
	TCHAR szSealPlayerId[ 8 ] = { 0, };
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", idReceiver );
	FLSPrintf( szSealPlayerId, _countof( szSealPlayerId ), "%07d", dwSealPlayerID );

	pQuery->BindParameterChar(  SQL_PARAM_INPUT, 		2,		 szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		7,		 szPlayerId );
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,		32,		 lpszAccount );
	pQuery->BindParameterInt(  SQL_PARAM_INPUT,  &lSetPlayerSlot );
	pQuery->BindParameterChar(  SQL_PARAM_INPUT, 		7,		 szSealPlayerId );

	
// 	FLSPrintf( szQuery, _countof( szQuery ), "uspSetSealCharUpdate '%02d','%07d','%s','%d','%07d'",
// 						 g_appInfo.dwSys, idReceiver,lpszAccount,lSetPlayerSlot,dwSealPlayerID);

	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s, %s, %d, %s" )
			, szQuery, szServerIndex, szPlayerId, lpszAccount, lSetPlayerSlot, szSealPlayerId );
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////
}

void CDbManager::LoginProtectCert( CQuery *qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr arRead( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	
	char lpszAccount[MAX_ACCOUNT] = {0, };
	char lpszPlayer[MAX_PLAYER]	= {0, };
	u_long idPlayer = NULL_PLAYER_ID;
	int nBankPW = 0;

	arRead.ReadString( lpszAccount, _countof( lpszAccount ) );
	arRead.ReadString( lpszPlayer, _countof( lpszPlayer ) );
	arRead >> idPlayer >> nBankPW;

	char szQuery[MAX_QUERY_SIZE]	= {0, };
	FLSPrintf( szQuery, _countof( szQuery ), "usp_BankPW_Check '%02d', '%07d', '%04d'", g_appInfo.dwSys, idPlayer, nBankPW );

	if( FALSE == qry->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}
	int nError = 1;
	if( qry->Fetch() )
		nError = qry->GetInt( "f_check" );

	if( nError == 0 )
		g_dpLoginSrvr.SendLoginProtect( lpszAccount, lpszPlayer, idPlayer, TRUE, lpDbOverlappedPlus->dpid );
	else
		g_dpLoginSrvr.SendLoginProtect( lpszAccount, lpszPlayer, idPlayer, FALSE, lpDbOverlappedPlus->dpid );
}

//////////////////////////////////////////////////////////////////////////
// mirchang_100723 give coupon item event

void	CDbManager::GetCouponNumberItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	T_PACKET_COUPON_NUMBER tCouponNumber = { 0 };
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( (void*)(&tCouponNumber), sizeof( T_PACKET_COUPON_NUMBER ) );

	m_AddRemoveLock.Enter();

	ACCOUNT_CACHE* pCache = m_AccountCacheMgr.Find( tCouponNumber.szAccount );

	if( pCache != NULL )
	{
		TCHAR szQuery[128] = { 0, };

		FLSPrintf( szQuery, _countof( szQuery ), "USP_GETITEM_COUPON '%02d', '%07d', '%s', '%s'",
			g_appInfo.dwSys, tCouponNumber.dwPlayerId, tCouponNumber.szAccount, tCouponNumber.szCouponNumber );

		if( pQuery->Exec( szQuery ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
			m_AddRemoveLock.Leave();
			return;
		}
		else
		{
			if( pQuery->Fetch() == TRUE )
			{
				tCouponNumber.nError = pQuery->GetInt( "ret_code" );

				if( tCouponNumber.nError == 0 )
				{
					TCHAR szItemId[6] = { 0 };
					pQuery->GetStr( "ret_item", szItemId, _countof( szItemId ) );
					tCouponNumber.dwItemId = _tstol( szItemId );
				}
				else
				{
					pQuery->GetStr( "ret_msg", tCouponNumber.szError, _countof( tCouponNumber.szError ) );
				}

				CDPTrans::GetInstance()->SendCouponNumber( &tCouponNumber, lpDbOverlappedPlus->dpid );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "query Fetch fail. User[%07d], Account:[%s], Coupon:[%s]" ),
					tCouponNumber.dwPlayerId, tCouponNumber.szAccount, tCouponNumber.szCouponNumber );
			}
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Invalid Account:[%s]" ), tCouponNumber.szAccount );
	}

	m_AddRemoveLock.Leave();
	return;
}

// mirchang_100723 give coupon item event
//////////////////////////////////////////////////////////////////////////



void	CDbManager::ExecCooperativeContributions_Contribute( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	enum E_DB_REWARD_ITEM_RESULT
	{
		DBRST_FAIL		= 0,
		DBRST_SUCCESS,
		DBRST_ALREADY_REWARD,

		DBRST_IGNORE = 9,
	};


	T_W2DB_PACKET_CONTRIBUTE_REQ req;

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &req, sizeof( req ) );

	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	//FLSPrintf( szQuery, _countof( szQuery ), "{ call testCooperativeContributions_Contribute( '%02d', '%07d', ?, %u, '%s', '%s', %u, %u ) }"
	FLSPrintf( szQuery, _countof( szQuery ), "{ call usp_CooperativeContributions_ContributeAndTryReward( '%02d', '%07d', ?, %u, '%s', '%s', %u, %u ) }"
		, g_appInfo.dwSys, req.idPlayer, /*req.szName,*/ req.dwContributionID
		, FLTmToString( req.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( req.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
		, req.dwContiributionCount, req.dwContiributionMax );
	

	const size_t MAX_DB_CHARACTER_NAME = 32;
	
	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, MAX_DB_CHARACTER_NAME/*_countof( req.szName ) - 1*/,	 req.szName );

	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
	}
	else
	{
		if( pQuery->Fetch() == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[Fetch] %s" ), szQuery ); 
		}
		else
		{
			T_W2DB_PACKET_CONTRIBUTE_ACK ack;
			ack.idPlayer				= req.idPlayer;
			ack.dwContributionID		= req.dwContributionID;
			//ack.tmStart					= req.tmStart;			//ack.eContributionKind		= req.eContributionKind;	//ack.dwIndexOfKind			= req.dwIndexOfKind;

			BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
			ack.dwContiributionCount	= pQuery->GetInt( "dwContributedCount" );
			const /*T_W2DB_PACKET_CONTRIBUTE_ACK::*/E_DB_REWARD_ITEM_RESULT eRewardItem_ret= static_cast< E_DB_REWARD_ITEM_RESULT >( pQuery->GetInt( "iRewardSP_Return" ) );				// iReturn 은 usp_CooperativeContributions_TryRewardItem의 리턴값을 가공한 것이다.
			out.Write( &ack, sizeof( ack ) );

			switch( eRewardItem_ret )
			{
			case DBRST_FAIL:				//보상 시도 실패
			case DBRST_SUCCESS:				//보상 시도 성공
			case DBRST_ALREADY_REWARD:		//이미 보상
			case DBRST_IGNORE:				//보상 시도 실패
				__noop;
				break;
			default:
				FLERROR_LOG( PROGRAM_NAME, _T( "[ UNREGIST RETRUN VALUE ][ nRewardItemRet(%d) Query(%s) ] " ), eRewardItem_ret, szQuery );
				break;
			}

			SEND( out, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
		}
	}
}

void	CDbManager::ExecCooperativeContributions_GetContributed( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	T_W2DB_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_INFO_REQ req;

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &req, sizeof( req ) );

	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "usp_CooperativeContributions_GetInfo '%02d', %u, '%s', '%s'"
		, g_appInfo.dwSys, req.dwContributionID
		, FLTmToString( req.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( req.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str() );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
	}
	else
	{
		if( pQuery->Fetch() == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[Fetch] %s" ), szQuery ); 
		}
		else
		{
			T_W2DB_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_INFO_ACK ack;
			ack.dwContributionID		= req.dwContributionID;
			ack.tmStart					= req.tmStart;
			ack.tmEnd					= req.tmEnd;
			//ack.eContributionKind		= req.eContributionKind;			//ack.dwIndexOfKind			= req.dwIndexOfKind;

			BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
			ack.dwContiributionCount	= pQuery->GetInt( "dwContributedCount" );
			out.Write( &ack, sizeof( ack ) );

			const DPID dpidMulti	= lpDbOverlappedPlus->dpid;
			SEND( out, CDPTrans::GetInstance(), dpidMulti );
		}
	}
}

void	CDbManager::ExecCooperativeContributions_GetRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_REQ req;

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &req, sizeof( req ) );

	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ call usp_CooperativeContributions_GetRanking ( '%02d', %u, '%s', '%s' ) }"
		, g_appInfo.dwSys, req.dwContributionID
		, FLTmToString( req.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( req.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str() );
	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[Exec] %s" ), szQuery ); 
	}
	else
	{
		T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_ACK ack = { 0 };
		ack.dwContributionID		= req.dwContributionID;
		ack.idPlayer				= req.idPlayer;

		for( size_t i = 0; i < T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_ACK::RANKING_MAX && pQuery->Fetch(); ++i )
		{
			pQuery->GetStr( "m_szName", ack.arrNameString[ i ], _countof( ack.arrNameString[ i ] ) );
		}

		BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
		out.Write( &ack, sizeof( ack ) );

		const DPID dpidMulti	= lpDbOverlappedPlus->dpid;
		SEND( out, CDPTrans::GetInstance(), dpidMulti );
	}
}
//
//void	CDbManager::ExecCooperativeContributions_TryRewardItem( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	T_W2DB_PACKET_TRY_REWARD_ITEM_REQ req;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//	ar.Read( &req, sizeof( req ) );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call usp_CooperativeContributions_TryRewardItem ( '%02d', %u, '%s', '%s' ) }"
//		, g_appInfo.dwSys, req.dwContributionID
//		, FLTmToString( req.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( req.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str() );
//
//	if( pQuery->PrepareQuery( szQuery ) == FALSE )
//	{
//		return;
//	}
//
//	int nOutBind		= 0;
//	SQLINTEGER cbLen	= SQL_NTS;
//	int nRet			= 0;
//	BOOL bRet			= TRUE;
//
//	bRet				&= pQuery->BindParameter( ++nOutBind, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "[Exec] %s" ), szQuery ); 
//	}
//	else
//	{
//		T_W2DB_PACKET_TRY_REWARD_ITEM_ACK ack = { 0 };
//		//pQuery->GetStr( "m_szName", ack.arrNameString[ i ], _countof( ack.arrNameString[ i ] ) );
//		ack.dwContributionID= req.dwContributionID;
//		//ack.eResult			= static_cast< T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::E_RESULT >( pQuery->GetInt( "nRet" ) );
//		ack.eResult			= static_cast< T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::E_RESULT >( nRet ); //static_cast< T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::E_RESULT >( pQuery->GetInt( "nRet" ) );
//		//ack.eResult			= ( T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::E_RESULT )pQuery->GetInt( "nResult" ); //static_cast< T_W2DB_PACKET_TRY_REWARD_ITEM_ACK::E_RESULT >( pQuery->GetInt( "nRet" ) );
//
//		BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
//		out.Write( &ack, sizeof( ack ) );
//
//		SEND( out, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
//	}
//}

void	CDbManager::ExecCooperativeContributions_TryWriteItemRewardInfo( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_REQ req;

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &req, sizeof( req ) );

	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call usp_CooperativeContributions_TryWriteItemRewardInfo ( '%02d', %u, '%s', '%s', %u, %d ) }"
		, g_appInfo.dwSys, req.dwContributionID
		, FLTmToString( req.tmStart, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str(), FLTmToString( req.tmEnd, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
		, req.dwItemID, req.shItemCount );

	if( pQuery->PrepareQuery( szQuery ) == FALSE )
	{
		return;
	}

	int nRet			= 0;

	//bRet				&= pQuery->BindParameter( ++nOutBind, SQL_RETURN_VALUE, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
	pQuery->BindParameterInt( SQL_PARAM_OUTPUT, &nRet );

	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[Exec] %s" ), szQuery ); 
	}
	else
	{
		T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK ack = { 0 };
		//pQuery->GetStr( "m_szName", ack.arrNameString[ i ], _countof( ack.arrNameString[ i ] ) );
		ack.dwContributionID= req.dwContributionID;
		ack.tmStart			= req.tmStart;
		ack.tmEnd			= req.tmEnd;
		ack.dwItemID		= req.dwItemID;
		ack.shItemCount		= req.shItemCount;

		ack.eResult			= static_cast< T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK::E_RESULT >( nRet );//( T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK::E_RESULT )pQuery->GetInt( "nResult" );//static_cast< T_W2DB_PACKET_TRY_WRITE_ITEM_REWARD_INFO_ACK::E_RESULT >( pQuery->GetInt( "nRet" ) );

		BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_WRITE_ITEM_REWARD_INFO_ACK, DPID_UNKNOWN, DPID_UNKNOWN );
		out.Write( &ack, sizeof( ack ) );

		SEND( out, CDPTrans::GetInstance(), DPID_ALLPLAYERS );
	}
}


void	CDbManager::ExecRecord_DungeonBestCompleteTime( CQuery* pQuery, CQuery * pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	if( pQuery == NULL || pQueryLog == NULL )
	{
		return;
	}

	//T_W2DB_PACKET_RECORD_DUNGEON_BEST_COMPLETE_TIME req;
	T_W2DB_PACKET_RECORD_DUNGEON_PLAY_INFO req = { 0, };

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &req, sizeof( req ) );
	req.nWeeklyRank		= 0;


	//완료 시간 기록( 랭킹DB )
	if( req.bSuccess == TRUE )
	{
		char szQuery[ MAX_QUERY_SIZE ] = { 0 };
		FLSPrintf( szQuery, _countof( szQuery ), "uspRecord_DungeonBestPlayTime '%06d', '%02d', %u, %d, %u"
			, req.dwGuildID, g_appInfo.dwSys, req.dwWorldID, req.eDungeonLevel, req.dwCompleteTimeTick );

		if( pQuery->Exec( szQuery ) == FALSE )
		{ 
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED EXECUTE SP(%s) ]" ), szQuery );
			CDPTrans::GetInstance()->SendDungeonBestCompleteTimeAck( lpDbOverlappedPlus->dpid, req );
			return;
		}
		
		if( pQuery->MoreResults() == TRUE )
		{
			while( pQuery->Fetch() ==TRUE )
			{
				if( pQuery->IsValidColumn( "m_rank" ) == true )
				{
					req.nWeeklyRank = pQuery->GetInt( "m_rank" );
					break;
				}
			}
		}

		CDPTrans::GetInstance()->SendDungeonBestCompleteTimeAck( lpDbOverlappedPlus->dpid, req );
	}

	//LogDB 기록
	if( req.tmStartedDate == INVALID_DATETIME || req.tmFinishedDate == INVALID_DATETIME )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATETIME s(%I64u), f(%I64u) ]" ), req.tmStartedDate.value, req.tmFinishedDate.value );
		return;
	}

	//로그 기록
	char szLogQuery[ MAX_QUERY_SIZE ] = { 0 };
	FLSPrintf( szLogQuery, _countof( szLogQuery ), "uspRecord_DungeonBestPlayTime_Log '%06d', '%02d', %u, %d, %u, '%s', '%s', %d"
		, req.dwGuildID, g_appInfo.dwSys, req.dwWorldID, req.eDungeonLevel, req.dwCompleteTimeTick
		, FLTmToString( req.tmStartedDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()  
		, FLTmToString( req.tmFinishedDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
		, ( req.bSuccess == TRUE ) ? 1 : 0
		);
	if( pQueryLog->Exec( szLogQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED QUERY EXECUTE. SP:(%s) ]" ), szLogQuery );
	}
}

void	CDbManager::ExecRecord_GetDungeonRanking( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	if( pQuery == NULL )
	{
		return;
	}

	T_W2DB_PACKET_COLOSSEUM_GET_RANKING_INFO req;

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &req, sizeof( req ) );

	T_PACKET_COLOSSEUM_GET_RANKING_INFO ack = { 0 };

	const int RANKING_KIND_MAX = 3;
	//const int DUNGEON_LEVEL_MAX = 2;

	for( int eDungeonLevel = 0; eDungeonLevel < T_PACKET_COLOSSEUM_GET_RANKING_INFO::DUNGEON_LEVEL_MAX; ++eDungeonLevel )
	{
		//랭킹 가져오기		tblDungeonTimeTopRecord_Weekly
		for( int kind = 0; kind < RANKING_KIND_MAX; ++kind )
		{
			char szQuery[ MAX_QUERY_SIZE ] = { 0 };
			FLSPrintf( szQuery, _countof( szQuery ), "uspRecord_SearchDungeonRanking '%07d', '%02d', %u, %d, %d"
				, req.idPlayer, g_appInfo.dwSys, req.dwWorldID, eDungeonLevel, kind );
			if( pQuery->Exec( szQuery ) == FALSE )
			{ 
				FLERROR_LOG( PROGRAM_NAME, _T( "%s  (%d)" ), szQuery, kind ); 
				continue;
			}

			if( pQuery->Fetch() == FALSE )		// NO_DATA
			{ 
				continue;
			}

			//자신 정보 가져오기
			//pQuery->GetInt( "m_rank" );
			char szGuild[ MAX_G_NAME ] = { 0 };
			pQuery->GetStr( "m_szGuild", szGuild, _countof( szGuild ) );
			const DWORD dwClearTick		= pQuery->GetInt( "tBestCompleteTime" );

			switch( kind )
			{
			case 0:
				FLStrcpy( ack.arrRanking[ eDungeonLevel ].WeekRanking.szMyGuild, _countof( ack.arrRanking[ eDungeonLevel ].WeekRanking.szMyGuild ), szGuild );
				ack.arrRanking[ eDungeonLevel ].WeekRanking.dwClearTick		= dwClearTick;
				break;
			case 1:
				FLStrcpy( ack.arrRanking[ eDungeonLevel ].MonthRanking.szMyGuild, _countof( ack.arrRanking[ eDungeonLevel ].MonthRanking.szMyGuild ), szGuild );
				ack.arrRanking[ eDungeonLevel ].MonthRanking.dwClearTick	= dwClearTick;
				break;
			case 2:
				FLStrcpy( ack.arrRanking[ eDungeonLevel ].TotalRanking.szMyGuild, _countof( ack.arrRanking[ eDungeonLevel ].TotalRanking.szMyGuild ), szGuild );
				ack.arrRanking[ eDungeonLevel ].TotalRanking.dwClearTick	= dwClearTick;
				break;
			default:
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID time index dungeonLevel(%d) (%d)] %s" ), eDungeonLevel, kind, szQuery ); 
				break;
			}

			if( pQuery->MoreResults() == FALSE )
			{
				continue;
			}

			//top 5 가져오기
			for( int i = 0; ( pQuery->Fetch() != FALSE ) && i < T_COLOSSEUM_RANKING::RANKING_MAX; ++i )
			{
				pQuery->GetInt( "m_rank" );
				memset( szGuild, 0, sizeof( szGuild ) );
				pQuery->GetStr( "m_szGuild", szGuild, _countof( szGuild ) );
				const DWORD dwBestClearTick		= pQuery->GetInt( "tBestCompleteTime" );

				//m_rank, m_szGuild, tBestCompleteTime
				switch( kind )
				{
				case 0:
					FLStrcpy( ack.arrRanking[ eDungeonLevel ].WeekRanking.szArrGuild[ i ], _countof( ack.arrRanking[ eDungeonLevel ].WeekRanking.szArrGuild[ i ] ), szGuild );
					ack.arrRanking[ eDungeonLevel ].WeekRanking.dwArrClearTick[ i ]		= dwBestClearTick;
					break;
				case 1:
					FLStrcpy( ack.arrRanking[ eDungeonLevel ].MonthRanking.szArrGuild[ i ], _countof( ack.arrRanking[ eDungeonLevel ].MonthRanking.szArrGuild[ i ] ), szGuild );
					ack.arrRanking[ eDungeonLevel ].MonthRanking.dwArrClearTick[ i ]	= dwBestClearTick;
					break;
				case 2:
					FLStrcpy( ack.arrRanking[ eDungeonLevel ].TotalRanking.szArrGuild[ i ], _countof( ack.arrRanking[ eDungeonLevel ].TotalRanking.szArrGuild[ i ] ), szGuild );
					ack.arrRanking[ eDungeonLevel ].TotalRanking.dwArrClearTick[ i ]	= dwBestClearTick;
					break;
				default:
					FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID time index dungeonLevel(%d) (%d)] %s" ), eDungeonLevel, kind, szQuery ); 
					break;
				}
			}
		}
	}

	BEFORESENDDUAL( out, PACKETTYPE_COLOSSEUM_GET_RANKING_INFO, DPID_UNKNOWN, DPID_UNKNOWN );
	out << req.idPlayer;
	out.Write( &ack, sizeof( ack ) );

	const DPID dpidMulti	= lpDbOverlappedPlus->dpid;
	SEND( out, CDPTrans::GetInstance(), dpidMulti );
}
//
//void	CDbManager::ExecRecord_SearchDungeonBestPlay( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	if( pQuery == NULL )
//		return;
//
////	T_W2DB_PACKET_RECORD_DUNGEON_BEST_COMPLETE_TIME req;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//	ar.Read( &req, sizeof( req ) );
//}


void	CDbManager::SelectSerialNumber( CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	T_PACKET_SERIALNUMBER_REQ tReqMsg = { 0 };

	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
	ar.Read( &tReqMsg, sizeof( tReqMsg ) );

	TCHAR szQuery[512] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "usp_GetSerialNumber '%02d', %u, %u, %u",
				g_appInfo.dwSys,
				CSerialNumber::SN_TYPE_ITEM,
				tReqMsg.dwCount,
				CSerialNumber::SN_END_NUMBER );

	CMclAutoLock Lock( m_csSerialNumber );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
	}
	else
	{
		if( pQuery->Fetch() == TRUE )
		{
			T_PACKET_SERIALNUMBER_ACK tMsg = { 0 };

			//////////////////////////////////////////////////////////////////////////
			tMsg.iStartNumber = pQuery->GetInt( "StartNumber" );
			tMsg.iEndNumber = pQuery->GetInt( "CurrentNumber" );
			//////////////////////////////////////////////////////////////////////////


			if( lpDbOverlappedPlus->dpid == DPID_UNKNOWN )
			{
				if( CSerialNumber::GetInstance()->Add( tMsg.iStartNumber, tMsg.iEndNumber ) == false )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "SerialNumber 추가 실패 Start: %d, End: %d" ), tMsg.iStartNumber, tMsg.iEndNumber );
				}
			}
			else
			{
				CDPTrans::GetInstance()->SendSerialNumberAck( &tMsg, lpDbOverlappedPlus->dpid );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "query Fetch fail. Count[%d]" ), tReqMsg.dwCount );
		}
	}
}

void	CDbManager::TeleportMapAddPoint( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	CAr ar( pDbOverlappedPlus->lpBuf, pDbOverlappedPlus->uBufSize );
	FLPacketTeleportMapAddPointNoti kRecvPacket;
	kRecvPacket.Deserialize( ar );

	TCHAR szServerIndex[3] = { 0, };
	TCHAR szPlayerId[8] = { 0, };

	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", kRecvPacket.dwPlayerID );

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "{call usp_Teleport_Insert(?,?,?,?,?,?,?,?)}" );


	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		2,		 szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT, 		7,		 szPlayerId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,  &kRecvPacket.tUserPoint.dwIndex );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,  &kRecvPacket.tUserPoint.tPosition.dwWorldID );

	int x = static_cast< int >( kRecvPacket.tUserPoint.tPosition.tPos.x * 100 );
	int y = static_cast< int >( kRecvPacket.tUserPoint.tPosition.tPos.y * 100 );
	int z = static_cast< int >( kRecvPacket.tUserPoint.tPosition.tPos.z * 100 );

	pQuery->BindParameterInt( SQL_PARAM_INPUT,  &x );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,  &y );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,  &z );

	pQuery->BindParameterVarChar( SQL_PARAM_INPUT, 	32,		 kRecvPacket.tUserPoint.szName );

	if( pQuery->Exec( szQuery ) == TRUE )
	{
		if( pQuery->Fetch() )
		{
			int nRet = pQuery->GetInt( "iReturn" );
			if( nRet == 1 )
			{
				LogTeleportMap( pLogQuery, kRecvPacket.dwPlayerID, &kRecvPacket.tUserPoint, 'I' );
				return;
			}
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "usp_Teleport_Insert() 실패 Sidx: %s, playerid: %s, pointidx: %d, worldid: %d, x: %d, y: %d, z: %d" ),
		szServerIndex, szPlayerId, kRecvPacket.tUserPoint.dwIndex, kRecvPacket.tUserPoint.tPosition.dwWorldID, x, y, z );
}

void	CDbManager::TeleportMapChgPointName( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	CAr ar( pDbOverlappedPlus->lpBuf, pDbOverlappedPlus->uBufSize );
	FLPacketTeleportMapChgPointNameNoti kRecvPacket;
	kRecvPacket.Deserialize( ar );

	TCHAR szServerIndex[3] = { 0, };
	TCHAR szPlayerId[8] = { 0, };

	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", kRecvPacket.dwPlayerID );

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "{call usp_Teleport_Update(?,?,?,?,?,?,?,?)}" );

	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		7,		 szPlayerId );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	 &kRecvPacket.tUserPoint.dwIndex );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	 &kRecvPacket.tUserPoint.tPosition.dwWorldID );

	int x = static_cast< int >( kRecvPacket.tUserPoint.tPosition.tPos.x * 100 );
	int y = static_cast< int >( kRecvPacket.tUserPoint.tPosition.tPos.y * 100 );
	int z = static_cast< int >( kRecvPacket.tUserPoint.tPosition.tPos.z * 100 );

	pQuery->BindParameterInt( SQL_PARAM_INPUT,	 &x );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	 &y );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,	 &z );

	pQuery->BindParameterVarChar( SQL_PARAM_INPUT,	32,		 kRecvPacket.tUserPoint.szName );

	if( pQuery->Exec( szQuery ) == TRUE )
	{
		if( pQuery->Fetch() )
		{
			int nRet = pQuery->GetInt( "iReturn" );
			if( nRet == 1 )
			{
				LogTeleportMap( pLogQuery, kRecvPacket.dwPlayerID, &kRecvPacket.tUserPoint, 'U' );
				return;
			}
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "usp_Teleport_Update() 실패 Sidx: %s, playerid: %s, pointidx: %d, worldid: %d, x: %d, y: %d, z: %d" ),
		szServerIndex, szPlayerId, kRecvPacket.tUserPoint.dwIndex, kRecvPacket.tUserPoint.tPosition.dwWorldID, x, y, z );
}

void	CDbManager::TeleportMapDelPoint( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	CAr ar( pDbOverlappedPlus->lpBuf, pDbOverlappedPlus->uBufSize );
	FLPacketTeleportMapDelPointNoti kRecvPacket;
	kRecvPacket.Deserialize( ar );

	TCHAR szServerIndex[3] = { 0, };
	TCHAR szPlayerId[8] = { 0, };

	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", kRecvPacket.dwPlayerID );

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "{call usp_Teleport_Delete(?,?,?)}" );


	pQuery->BindParameterChar(  SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterChar(  SQL_PARAM_INPUT,		7,		 szPlayerId );
	pQuery->BindParameterInt(  SQL_PARAM_INPUT,	 &kRecvPacket.tUserPoint.dwIndex );
	if( pQuery->Exec( szQuery ) == TRUE )
	{
		if( pQuery->Fetch() )
		{
			int nRet = pQuery->GetInt( "iReturn" );
			if( nRet == 1 )
			{
				LogTeleportMap( pLogQuery, kRecvPacket.dwPlayerID, &kRecvPacket.tUserPoint, 'D' );
				return;
			}
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "usp_Teleport_Delete() 실패 Sidx: %s, playerid: %s, pointidx: %d" ),
		szServerIndex, szPlayerId, kRecvPacket.tUserPoint.dwIndex );
}

//////////////////////////////////////////////////////////////////////////
// BEGIN Character Inventory
void	CDbManager::LogTeleportMap( CQuery* pLogQuery, DWORD dwPlayerID, PT_TELEPORTMAP_POINT pUserPoint, char chOP )
{
	if( _GetContentState( CT_TELEPORTER ) == CS_NOT_SUPPORTED )
	{
		return;
	}

	TCHAR szServerIndex[3] = { 0, };
	TCHAR szPlayerId[8] = { 0, };

	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", dwPlayerID );

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "{call usp_Teleport_Log(?,?,?,?,?,?,?,?,?)}" );


	pLogQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pLogQuery->BindParameterChar( SQL_PARAM_INPUT,		7,		 szPlayerId );
	pLogQuery->BindParameterInt( SQL_PARAM_INPUT,	 &pUserPoint->dwIndex );
	pLogQuery->BindParameterInt( SQL_PARAM_INPUT,	 &pUserPoint->tPosition.dwWorldID );

	int x = static_cast< int >( pUserPoint->tPosition.tPos.x * 100 );
	int y = static_cast< int >( pUserPoint->tPosition.tPos.y * 100 );
	int z = static_cast< int >( pUserPoint->tPosition.tPos.z * 100 );

	pLogQuery->BindParameterInt( SQL_PARAM_INPUT,	 &x );
	pLogQuery->BindParameterInt( SQL_PARAM_INPUT,	 &y );
	pLogQuery->BindParameterInt( SQL_PARAM_INPUT,	 &z );

	pLogQuery->BindParameterVarChar( SQL_PARAM_INPUT,	32,		 pUserPoint->szName );
	pLogQuery->BindParameterChar( SQL_PARAM_INPUT,		1,		 &chOP );

	if( pLogQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "usp_Teleport_Log() 실패 Sidx: %s, playerid: %s, pointidx: %d, worldid: %d, x: %d, y: %d, z: %d" ),
			szServerIndex, szPlayerId, pUserPoint->dwIndex, pUserPoint->tPosition.dwWorldID, x, y, z );

		return;
	}
}

bool	CDbManager::LoadEquipInfo( CQuery* pQuery, DWORD dwPlayerId, DWORD dwMaxPartsSize, std::vector< EQUIP_INFO > & vecEquipInfo )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowGetEquipInfo '%07d', '%02d', %d", 
		dwPlayerId, g_appInfo.dwSys, dwMaxPartsSize );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}
	else
	{
		vecEquipInfo.clear();

		while( pQuery->Fetch() == TRUE )
		{
			EQUIP_INFO kEquipInfo;

			if( pQuery->IsValidColumn( "dwItemId" ) == true )
			{
				kEquipInfo.dwItemID			= static_cast<DWORD>( pQuery->GetInt( "dwItemId" ) );
			}

			if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
			{
				if( pQuery->IsValidColumn( "dwLooksChangeItemID" ) == true )
				{
					const DWORD dwLooksChangeItemID		= static_cast<DWORD>( pQuery->GetInt( "dwLooksChangeItemID" ) );
					PT_ITEM_SPEC pLooksChangeItemProp	= g_xSpecManager->GetSpecItem( dwLooksChangeItemID );
					if( pLooksChangeItemProp != NULL && pLooksChangeItemProp->bIsLooksChangeMaterial == TRUE )
					{
						kEquipInfo.dwLooksChangeItemID	= dwLooksChangeItemID;
					}
				}

				if( pQuery->IsValidColumn( "bIsCombined" ) == true )
				{
					kEquipInfo.bIsCombined				= static_cast<bool>( pQuery->GetInt( "bIsCombined" ) == 1 );
				}
			}

			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( kEquipInfo.dwItemID );
			if( pItemProp != NULL && pItemProp->dwParts != NULL_ID )
			{
				vecEquipInfo.push_back( kEquipInfo );
			}
		}
	}

	return true;
}

bool	CDbManager::LoadInventory( CMover* pMover, CQuery* pQuery )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowInventoryLoad '%07d', '%02d'", 
		pMover->m_idPlayer, g_appInfo.dwSys );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ QUERY EXECUTE FALIED(%s) ]" ), szQuery );
		return false;
	}
	else
	{
		DWORD	dwObjId = 0;
		int		nSerialNumberStatus = 0;

		DWORD	dwPrevObjIndex = NULL_ID;
		DWORD	dwCurrObjIndex = NULL_ID;

		while( pQuery->Fetch() == TRUE )
		{
			dwCurrObjIndex = static_cast<DWORD>( pQuery->GetInt( "dwInvenIndex" ) );
			if( dwCurrObjIndex >= pMover->m_Inventory.GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Invalid. Player:[%07d], InvenIndex:[%d]" ), pMover->m_idPlayer, dwCurrObjIndex );
				continue;
			}

			if( dwPrevObjIndex != NULL_ID && dwCurrObjIndex != NULL_ID && dwPrevObjIndex == dwCurrObjIndex )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Duplicate. Player:[%07d], InvenIndex:[%d]" ), pMover->m_idPlayer, dwCurrObjIndex );
				continue;
			}

			dwPrevObjIndex = dwCurrObjIndex;

			if( dwObjId >= pMover->m_Inventory.GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Inventory Overflow. Player:[%07d]" ), pMover->m_idPlayer );
				break;
			}

			FLItemElem kItemElem;
	
			LoadOneItemCommon( *pQuery, dwObjId, dwCurrObjIndex, kItemElem, nSerialNumberStatus );
			const bool bModify	= ModifyItemElem( *pMover, kItemElem );
			if( bModify == true )
			{
				FLINFO_LOG( PROGRAM_NAME, _T( "[ ITEM DATA MODIFIED. PLAYER_ID(%07d) ]" ), pMover->m_idPlayer );
			}
			pMover->m_Inventory.m_apItem[dwObjId]	= kItemElem;

			++dwObjId;
		}

		pMover->m_Inventory.SetAllocedSerialNumber( GetSerialNumberStatus( nSerialNumberStatus ) );
	}
	
	pMover->m_Inventory.RefreshIndex();
	return true;
}

void	CDbManager::SaveInventory( CMover* pMover, CQuery* pQuery )
{
	if( UpdateInventory( pMover, pQuery ) == true )
	{
		InsertInventory( pMover, pQuery );
		DeleteInventory( pMover, pQuery );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Inventory Save Failed. PlayerId:[%07d]" ), pMover->m_idPlayer );
	}
}

bool	CDbManager::UpdateInventory( CMover* pMover, CQuery* pQuery )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowInventoryUpdate '%07d', '%02d'", 
		pMover->m_idPlayer, g_appInfo.dwSys );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}

	if( pQuery->Fetch() == TRUE )
	{
		int nError = 0;
		nError = pQuery->GetInt( "fError" );

		if( nError == 0 )
		{
			return true;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "UpdateInventory Failed. Player:[%07d]" ), pMover->m_idPlayer );
			return false;
		}
	}
		
	FLERROR_LOG( PROGRAM_NAME, _T( "UpdateInventory Fetch Failed. Player:[%07d]" ), pMover->m_idPlayer );
	return false;

	//return false;
}

void	CDbManager::InsertInventory( CMover* pMover, CQuery* pQuery )
{
	const DWORD MAX_PARAMETER = _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ?
		MAX_ITEM_PARAMETER + MAX_ADD_ITEM_PARAMETER + 2 : MAX_ITEM_PARAMETER + 2;

	for( DWORD dwNth = 0; dwNth < pMover->m_Inventory.GetMax(); ++dwNth )
	{
		FLItemElem* pItemElem = pMover->m_Inventory.GetAtId( dwNth );
	
		if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwObjIndex < pMover->m_Inventory.GetMax() )
		{
			TCHAR szQuery[MAX_QUERY_SIZE]	= { 0, };
			
			MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "uspRowInventoryInsert" ), MAX_PARAMETER );

			TCHAR szPlayerId[8]		= { 0, };
			TCHAR szServerIndex[3]	= { 0, };
			FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", pMover->m_idPlayer );
			FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, ( _countof( szPlayerId ) - 1 ), szPlayerId );
			pQuery->BindParameterChar( SQL_PARAM_INPUT, ( _countof( szServerIndex ) - 1 ), szServerIndex );

			T_ITEM_QUERY_INFO kItem;
			kItem.Copy( *pItemElem );

			SaveOneItemCommon( *pQuery, kItem );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, ( _countof( kItem.szState ) - 1 ), kItem.szState );

			if( pQuery->Exec( szQuery ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED. PLAYER_ID(%07d), ITEM_ID(%d), QUANTITY(%d), SERIAL_NUMBER(%d) ]" )
					, pMover->m_idPlayer, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetSerialNumber() );
			}
		}
	}
}

void	CDbManager::DeleteInventory( CMover* pMover, CQuery* pQuery )
{
	TCHAR szQuery[64] = {0, };
	FLSPrintf( szQuery, _countof( szQuery ), "uspRowInventoryDelete '%07d', '%02d'", 
		pMover->m_idPlayer, g_appInfo.dwSys );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
	}
}
// END Character Inventory
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BEGIN Character Bank
bool	CDbManager::LoadCharacterBank( CMover* pMover, CQuery* pQuery, u_long uPlayerId, int nDBDataSlot )
{
	if( nDBDataSlot < 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LoadCharacterBank. DBDataSlot Invalid. DBDataSlot:[%d]" ), nDBDataSlot );
		return false;
	}

	const int nSlot = GET_PLAYER_SLOT( nDBDataSlot );
	if( nSlot < 0 || nSlot >= MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LoadCharacterBank. ProgramDataSlot Invalid. ProgramDataSlot:[%d]" ), nSlot );
		return false;
	}

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 3;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspRowBankLoad", MAX_PARAMETER );

	TCHAR szPlayerId[ 8 ] = { 0, };
	TCHAR szServerIndex[ 3 ] = { 0, };
	
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", uPlayerId );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );


	pQuery->BindParameterChar( SQL_PARAM_INPUT,		7,		 szPlayerId );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,				 &nDBDataSlot );


	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s, %d" ), szQuery, szServerIndex, szPlayerId, nDBDataSlot );
		return false;
	}
	else
	{
		DWORD	dwObjId = 0;
		int		nSerialNumberStatus = 0;

		DWORD	dwPrevObjIndex = NULL_ID;
		DWORD	dwCurrObjIndex = NULL_ID;

		while( pQuery->Fetch() == TRUE )
		{
			dwCurrObjIndex = static_cast<DWORD>( pQuery->GetInt( "dwInvenIndex" ) );
			if( dwCurrObjIndex >= pMover->m_Bank[nSlot].GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Invalid. Player:[%07d], InvenIndex:[%d], ProgramDataSlot:[%d]" )
					, uPlayerId, dwCurrObjIndex, nSlot );
				
				continue;
			}

			if( dwPrevObjIndex != NULL_ID && dwCurrObjIndex != NULL_ID && dwPrevObjIndex == dwCurrObjIndex )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Duplicate. Player:[%07d], InvenIndex:[%d], ProgramDataSlot:[%d]" )
					, uPlayerId, dwCurrObjIndex, nSlot );
				
				continue;
			}

			dwPrevObjIndex = dwCurrObjIndex;

			if( dwObjId >= pMover->m_Bank[nSlot].GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Bank Overflow. Player:[%07d], ProgramDataSlot:[%d]" ), uPlayerId, nSlot );
				break;
			}

			FLItemElem kItemElem;

			LoadOneItemCommon( *pQuery, dwObjId, dwCurrObjIndex, kItemElem, nSerialNumberStatus );
			const bool bModify	= ModifyItemElem( *pMover, kItemElem );
			if( bModify == true )
			{
				FLINFO_LOG( PROGRAM_NAME, _T( "[ ITEM DATA MODIFIED. PLAYER_ID(%07d) ]" ), pMover->m_idPlayer );
			}
			pMover->m_Bank[nSlot].m_apItem[dwObjId]	= kItemElem;
		
			++dwObjId;
		}

		pMover->m_Bank[nSlot].SetAllocedSerialNumber( GetSerialNumberStatus( nSerialNumberStatus ) );
	}

	pMover->m_Bank[nSlot].RefreshIndex();

	return true;
}

void	CDbManager::SaveCharacterBank( CMover* pMover, CQuery* pQuery, u_long uPlayerId, int nDBDataSlot )
{
	if( UpdateCharacterBank( uPlayerId, pQuery, nDBDataSlot ) == true )
	{
		if( InsertCharacterBank( pMover, pQuery, uPlayerId, nDBDataSlot ) == true )
		{
			DeleteCharacterBank( uPlayerId, pQuery, nDBDataSlot );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterBank Save Failed. PlayerId:[%07d], DBDataSlot:[%d]" ), uPlayerId, nDBDataSlot );
	}
}

bool	CDbManager::UpdateCharacterBank( u_long uPlayerId, CQuery* pQuery, int nDBDataSlot )
{
	if( nDBDataSlot < 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "UpdateCharacterBank. DBDataSlot Invalid. DBDataSlot:[%d]" ), nDBDataSlot );
		return false;
	}

	const int nSlot = GET_PLAYER_SLOT( nDBDataSlot );
	if( nSlot < 0 || nSlot >= MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "UpdateCharacterBank. ProgramDataSlot Invalid. ProgramDataSlot:[%d]" ), nSlot );
		return false;
	}

	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowBankUpdate '%07d', '%02d', %d", 
		uPlayerId, g_appInfo.dwSys, nDBDataSlot );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}

	if( pQuery->Fetch() == TRUE )
	{
		int nError = 0;
		nError = pQuery->GetInt( "fError" );

		if( nError == 0 )
		{
			return true;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Update CharacterBank Failed. Player:[%07d], DBDataSlot:[%d]" ), uPlayerId, nDBDataSlot );
			return false;
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "Update CharacterBank Fetch Failed. Player:[%07d], DBDataSlot:[%d]" ), uPlayerId, nDBDataSlot );
	return false;
}

bool	CDbManager::InsertCharacterBank( CMover* pMover, CQuery* pQuery, u_long uPlayerId, int nDBDataSlot )
{
	if( nDBDataSlot < 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "InsertCharacterBank. DBDataSlot Invalid. DBDataSlot:[%d]" ), nDBDataSlot );
		return false;
	}

	const int nSlot = GET_PLAYER_SLOT( nDBDataSlot );
	if( nSlot < 0 || nSlot >= MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "InsertCharacterBank. ProgramDataSlot Invalid. ProgramDataSlot:[%d]" ), nSlot );
		return false;
	}

	const DWORD MAX_PARAMETER = _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ?
		MAX_ITEM_PARAMETER + MAX_ADD_ITEM_PARAMETER + 3 : MAX_ITEM_PARAMETER + 3;

	TCHAR szPlayerId[8]		= { 0, };
	TCHAR szServerIndex[3]	= { 0, };
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", uPlayerId );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );


	for( DWORD dwNth = 0; dwNth < pMover->m_Bank[ nSlot ].GetMax(); ++dwNth )
	{
		FLItemElem* pItemElem = pMover->m_Bank[ nSlot ].GetAtId( dwNth );

		if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwObjIndex < pMover->m_Bank[ nSlot ].GetMax() )
		{
			TCHAR szQuery[MAX_QUERY_SIZE]	= { 0, };
			
			MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "uspRowBankInsert" ), MAX_PARAMETER );

			pQuery->BindParameterChar( SQL_PARAM_INPUT,	_countof( szPlayerId ) - 1, szPlayerId );
			pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );
			pQuery->BindParameterInt( SQL_PARAM_INPUT, &nDBDataSlot );

			T_ITEM_QUERY_INFO kItem;
			kItem.Copy( *pItemElem );

			SaveOneItemCommon( *pQuery, kItem );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, ( _countof( kItem.szState ) - 1 ), kItem.szState );

			if( pQuery->Exec( szQuery ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED. PLAYER_ID(%07d), SLOT(%d), ITEM_ID(%d), QUANTITY(%d), SERIAL_NUMBER(%d) ]" )
					, uPlayerId, nDBDataSlot, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetSerialNumber() );
			}
		}
	}

	return true;
}

void	CDbManager::DeleteCharacterBank( u_long uPlayerId, CQuery* pQuery, int nDBDataSlot )
{
	if( nDBDataSlot < 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "DeleteCharacterBank. DBDataSlot Invalid. DBDataSlot:[%d]" ), nDBDataSlot );
		return;
	}

	const int nSlot = GET_PLAYER_SLOT( nDBDataSlot );
	if( nSlot < 0 || nSlot >= MAX_CHARACTER_SLOT )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "DeleteCharacterBank. ProgramDataSlot Invalid. ProgramDataSlot:[%d]" ), nSlot );
		return;
	}

	TCHAR szQuery[64] = {0, };
	FLSPrintf( szQuery, _countof( szQuery ), "uspRowBankDelete '%07d', '%02d', %d", 
		uPlayerId, g_appInfo.dwSys, nDBDataSlot );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
	}
}
// END Character Bank
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BEGIN Character Pocket
bool	CDbManager::LoadCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket )
{
	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 3;

//	int nSlot = 0;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "uspRowPocketLoad", MAX_PARAMETER );

	TCHAR szPlayerId[ 8 ]		= { 0, };
	TCHAR szServerIndex[ 3 ]	= { 0, };
	
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", pMover->m_idPlayer );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );


	pQuery->BindParameterChar( SQL_PARAM_INPUT,		7,		 szPlayerId );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterInt( SQL_PARAM_INPUT,				 &nPocket );

	if( pQuery->Exec( szQuery ) == FALSE )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "[ERROR] Exec. %s - %s, %s, %d" ), szQuery, szServerIndex, szPlayerId, nPocket );
		return false;
	}
	else
	{
		DWORD	dwObjId = 0;
		int		nSerialNumberStatus = 0;

		DWORD	dwPrevObjIndex = NULL_ID;
		DWORD	dwCurrObjIndex = NULL_ID;

		while( pQuery->Fetch() == TRUE )
		{
			if( pMover->m_Pocket.m_kPocket[ nPocket ].IsAlloced() == FALSE )
			{
				pMover->m_Pocket.Avail( nPocket );
			}

			dwCurrObjIndex = static_cast<DWORD>( pQuery->GetInt( "dwInvenIndex" ) );
			if( dwCurrObjIndex >= pMover->m_Pocket.m_kPocket[ nPocket ].GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Invalid. Player:[%07d], InvenIndex:[%d], nPocket:[%d]" )
					, pMover->m_idPlayer, dwCurrObjIndex, nPocket );
				
				continue;
			}

			if( dwPrevObjIndex != NULL_ID && dwCurrObjIndex != NULL_ID && dwPrevObjIndex == dwCurrObjIndex )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Duplicate. Player:[%07d], InvenIndex:[%d], nPocket:[%d]" )
					, pMover->m_idPlayer, dwCurrObjIndex, nPocket );
				
				continue;
			}

			dwPrevObjIndex = dwCurrObjIndex;

			if( dwObjId >= pMover->m_Pocket.m_kPocket[ nPocket ].GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Pocket Overflow. Player:[%07d], nPocket:[%d]" ), pMover->m_idPlayer, nPocket );
				break;
			}

			FLItemElem kItemElem;

			LoadOneItemCommon( *pQuery, dwObjId, dwCurrObjIndex, kItemElem, nSerialNumberStatus );
			const bool bModify	= ModifyItemElem( *pMover, kItemElem );
			if( bModify == true )
			{
				FLINFO_LOG( PROGRAM_NAME, _T( "[ ITEM DATA MODIFIED. PLAYER_ID(%07d) ]" ), pMover->m_idPlayer );
			}
			pMover->m_Pocket.m_kPocket[ nPocket ].m_apItem[dwObjId]	= kItemElem;

			++dwObjId;
		}

		pMover->m_Pocket.m_kPocket[ nPocket ].SetAllocedSerialNumber( GetSerialNumberStatus( nSerialNumberStatus ) );
	}

	pMover->m_Pocket.m_kPocket[ nPocket ].RefreshIndex();

	return true;
}

void	CDbManager::SaveCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket )
{
	if( UpdateCharacterPocket( pMover, pQuery, nPocket ) == true )
	{
		InsertCharacterPocket( pMover, pQuery, nPocket );
		DeleteCharacterPocket( pMover, pQuery, nPocket );
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CharacterPocket Save Failed. PlayerId:[%07d]" ), pMover->m_idPlayer );
	}
}

bool	CDbManager::UpdateCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowPocketUpdate '%07d', '%02d', %d", 
		pMover->m_idPlayer, g_appInfo.dwSys, nPocket );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}

	if( pQuery->Fetch() == TRUE )
	{
		int nError = 0;
		nError = pQuery->GetInt( "fError" );

		if( nError == 0 )
		{
			return true;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Update CharacterPocket Failed. Player:[%07d]" ), pMover->m_idPlayer );
			return false;
		}
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "Update CharacterPocket Fetch Failed. Player:[%07d]" ), pMover->m_idPlayer );
	return false;
}

void	CDbManager::InsertCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket )
{
	const DWORD MAX_PARAMETER = _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ?
		MAX_ITEM_PARAMETER + MAX_ADD_ITEM_PARAMETER + 3 : MAX_ITEM_PARAMETER + 3;
	
	TCHAR szPlayerId[8]		= { 0, };
	TCHAR szServerIndex[3]	= { 0, };
	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", pMover->m_idPlayer );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	for( DWORD dwNth = 0; dwNth < pMover->m_Pocket.m_kPocket[ nPocket ].GetMax(); ++dwNth )
	{
		FLItemElem* pItemElem = pMover->m_Pocket.m_kPocket[ nPocket ].GetAtId( dwNth );

		if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwObjIndex < pMover->m_Pocket.m_kPocket[ nPocket ].GetMax() )
		{
			TCHAR szQuery[MAX_QUERY_SIZE] = { 0 };

			MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "uspRowPocketInsert" ), MAX_PARAMETER );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szPlayerId ) - 1,	szPlayerId );
			pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );
			pQuery->BindParameterInt( SQL_PARAM_INPUT, &nPocket );

			T_ITEM_QUERY_INFO kItem;
			kItem.Copy( *pItemElem );

			SaveOneItemCommon( *pQuery, kItem );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, ( _countof( kItem.szState ) - 1 ), kItem.szState );

			if( pQuery->Exec( szQuery ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED. PLAYER_ID(%07d), POCKET(%d), ITEM_ID(%d), QUANTITY(%d), SERIAL_NUMBER(%d) ]" )
					, pMover->m_idPlayer, nPocket, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetSerialNumber() );
			}
		}
	}
}

void	CDbManager::DeleteCharacterPocket( CMover* pMover, CQuery* pQuery, int nPocket )
{
	TCHAR szQuery[64] = {0, };
	FLSPrintf( szQuery, _countof( szQuery ), "uspRowPocketDelete '%07d', '%02d', %d", 
		pMover->m_idPlayer, g_appInfo.dwSys, nPocket );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
	}
}
// END Character Pocket
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BEGIN Guild Bank
bool	CDbManager::LoadGuildBank( DWORD dwGuildId, CItemContainer<FLItemElem>* pGuildBank, CQuery* pQuery )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowGuildBankLoad '%06d', '%02d'", 
		dwGuildId, g_appInfo.dwSys );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}
	else
	{
		DWORD	dwObjId = 0;
		int		nSerialNumberStatus = 0;

		DWORD	dwPrevObjIndex = NULL_ID;
		DWORD	dwCurrObjIndex = NULL_ID;

		while( pQuery->Fetch() == TRUE )
		{
			dwCurrObjIndex = static_cast<DWORD>( pQuery->GetInt( "dwInvenIndex" ) );
			if( dwCurrObjIndex >= pGuildBank->GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Invalid. GuildId:[%06d], InvenIndex:[%d]" ), dwGuildId, dwCurrObjIndex );

				continue;
			}

			if( dwPrevObjIndex != NULL_ID && dwCurrObjIndex != NULL_ID && dwPrevObjIndex == dwCurrObjIndex )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "InvenIndex is Duplicate. GuildId:[%06d], InvenIndex:[%d]" ), dwGuildId, dwCurrObjIndex );

				continue;
			}

			dwPrevObjIndex = dwCurrObjIndex;

			if( dwObjId >= pGuildBank->GetMax() )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Inventory Overflow. GuildId:[%06d]" ), dwGuildId );
				break;
			}

			FLItemElem kItemElem;

			LoadOneItemCommon( *pQuery, dwObjId, dwCurrObjIndex, kItemElem, nSerialNumberStatus );
			pGuildBank->m_apItem[dwObjId]	= kItemElem;

			++dwObjId;
		}

		pGuildBank->SetAllocedSerialNumber( GetSerialNumberStatus( nSerialNumberStatus ) );
	}

	pGuildBank->RefreshIndex();

	return true;
}

bool	CDbManager::SaveGuildBank( DWORD dwGuildId, CItemContainer<FLItemElem>* pGuildBank, CQuery* pQuery )
{
	if( UpdateGuildBank( dwGuildId, pQuery ) == true )
	{
		InsertGuildBank( dwGuildId, pGuildBank, pQuery );
		DeleteGuildBank( dwGuildId, pQuery );

		return true;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GuildBank Save Failed. GuildId:[%06d]" ), dwGuildId );
	}

	return false;
}

bool	CDbManager::UpdateGuildBank( DWORD dwGuildId, CQuery* pQuery )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowGuildBankUpdate '%06d', '%02d'", 
		dwGuildId, g_appInfo.dwSys );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}
	if( pQuery->Fetch() == TRUE )
	{
		int nError = 0;
		nError = pQuery->GetInt( "fError" );

		if( nError == 0 )
		{
			return true;
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "UpdateGuildBank Failed. GuildId:[%06d]" ), dwGuildId );
			return false;
		}
	}
	FLERROR_LOG( PROGRAM_NAME, _T( "UpdateGuildBank Fetch Failed. GuildId:[%06d]" ), dwGuildId );
	return false;
}

void	CDbManager::InsertGuildBank( DWORD dwGuildId, CItemContainer<FLItemElem>* pGuildBank, CQuery* pQuery )
{
	const DWORD MAX_PARAMETER = _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 ?
		MAX_ITEM_PARAMETER + MAX_ADD_ITEM_PARAMETER + 2 : MAX_ITEM_PARAMETER + 2;

	TCHAR szGuildID[7]		= { 0, };
	TCHAR szServerIndex[3]	= { 0, };
	FLSPrintf( szGuildID, _countof( szGuildID ), "%06d", dwGuildId );
	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );

	for( DWORD dwNth = 0; dwNth < pGuildBank->GetMax(); ++dwNth )
	{
		FLItemElem* pItemElem = pGuildBank->GetAtId( dwNth );

		if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwObjIndex < pGuildBank->GetMax() )
		{
			TCHAR szQuery[MAX_QUERY_SIZE] = { 0 };

			MakeQueryBindParameter( szQuery, _countof( szQuery ), _T( "uspRowGuildBankInsert" ), MAX_PARAMETER );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szGuildID ) - 1, szGuildID );
			pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( szServerIndex ) - 1, szServerIndex );

			T_ITEM_QUERY_INFO kItem;
			kItem.Copy( *pItemElem );

			SaveOneItemCommon( *pQuery, kItem );

			pQuery->BindParameterChar( SQL_PARAM_INPUT, _countof( kItem.szState ) - 1, kItem.szState );

			if( pQuery->Exec( szQuery ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED. GUILD_ID(%06d), ITEM_ID(%d), QUANTITY(%d), SERIAL_NUMBER(%d) ]" )
					, dwGuildId, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetSerialNumber() );
			}
		}
	}
}

void	CDbManager::DeleteGuildBank( DWORD dwGuildId, CQuery* pQuery )
{
	TCHAR szQuery[64] = {0, };
	FLSPrintf( szQuery, _countof( szQuery ), "uspRowGuildBankDelete '%06d', '%02d'", 
		dwGuildId, g_appInfo.dwSys );

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
	}
}

//void CDbManager::ConsignmentSale_GetRegistedCount( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_GetRegistedCountReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_GetRegistedCount ( ?, ? ) }" );
//
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );
//
//	int i					= 0;
//	SQLINTEGER cbLen = SQL_NTS;
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szPlayerId)-1,		0, szPlayerId, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	FLPacketWDB_GetRegistedCountAck ack;
//	ack.m_uPlayerID				= req.m_uPlayerID;
//	ack.m_dwDefaultRegistModeCount = pQuery->GetInt( "nDefaultRegistModeCount" );
//	ack.m_dwItemRegistModeCount = pQuery->GetInt( "nItemRegistModeCount" );
//
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}
//
//
//void CDbManager::ConsignmentSale_Regist( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_RegistReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), " test2_uspConsignmentSale_Insert "
//		"@nPrice=%d"
//		",@nOnePrice=%d"
//		",@szRealItemName='%s'"
//		",@szPlayerName='%s'"
//		",@dwItemType1=%u"
//		",@dwItemType2=%u"
//		",@dwGrade=%u"
//		",@dwLimitLevel=%u"
//		",@nRmainHour=%d"
//		",@eRegistMode=%d"
//
//		",@m_idPlayer='%07d'"
//		",@serverindex='%02d'"
//		",@dwItemId=%d"
//		",@dwInvenIndex=%d"
//		",@nItemNum=%d"
//		",@nUniqueNum=%d"
//		",@nAbilityOption=%d"
//		",@nItemResist=%d"
//		",@nResistAbilityOption=%d"
//		",@dwFlag=%d"
//		",@bCharged=%d"
//		",@dwKeepTime=%d"
//		",@nRandomOptItemId=%I64d"
//		",@nHitPoint=%d"
//		",@nRepairNumber=%d"
//		",@nRepair=%d"
//		",@nIsTransformVisPet=%d"
//		",@nUltimatePiercingSize=%d"
//		",@dwUltimatePiercingId1=%d"
//		",@dwUltimatePiercingId2=%d"
//		",@dwUltimatePiercingId3=%d"
//		",@dwUltimatePiercingId4=%d"
//		",@dwUltimatePiercingId5=%d"
//		",@nGeneralPiercingSize=%d"
//		",@dwGeneralPiercingId1=%d"
//		",@dwGeneralPiercingId2=%d"
//		",@dwGeneralPiercingId3=%d"
//		",@dwGeneralPiercingId4=%d"
//		",@dwGeneralPiercingId5=%d"
//		",@dwGeneralPiercingId6=%d"
//		",@dwGeneralPiercingId7=%d"
//		",@dwGeneralPiercingId8=%d"
//		",@dwGeneralPiercingId9=%d"
//		",@dwGeneralPiercingId10=%d"
//		",@dwGeneralPiercingKeepTime1=%d"
//		",@dwGeneralPiercingKeepTime2=%d"
//		",@dwGeneralPiercingKeepTime3=%d"
//		",@dwGeneralPiercingKeepTime4=%d"
//		",@dwGeneralPiercingKeepTime5=%d"
//		",@dwGeneralPiercingKeepTime6=%d"
//		",@dwGeneralPiercingKeepTime7=%d"
//		",@dwGeneralPiercingKeepTime8=%d"
//		",@dwGeneralPiercingKeepTime9=%d"
//		",@dwGeneralPiercingKeepTime10=%d"
//		",@nIsRarePet=%d"
//		",@nRarePetKind=%d"
//		",@nRarePetLevel=%d"
//		",@nRarePetExp=%d"
//		",@nRarePetEnergy=%d"
//		",@nRarePetLife=%d"
//		",@nRarePetAvailLevel1=%d"
//		",@nRarePetAvailLevel2=%d"
//		",@nRarePetAvailLevel3=%d"
//		",@nRarePetAvailLevel4=%d"
//		",@nRarePetAvailLevel5=%d"
//		",@szRarePetName='%s'"
//		",@nSerialNumberStatus=%d"
//		",@nSerialNumberId=%d"
//		",@idCouplePlayer=%d"
//		",@nLevelDown=%d"
//		",@nRandomOptionOriginId=%d"
//		",@nRandomOptionExtSize=%d"
//		",@nRandomOptionExtFlag=%d"
//		",@nRandomOptionExtDst1=%d"
//		",@nRandomOptionExtAdj1=%d"
//		",@nRandomOptionExtDst2=%d"
//		",@nRandomOptionExtAdj2=%d"
//		",@nRandomOptionExtDst3=%d"
//		",@nRandomOptionExtAdj3=%d"
//		",@nRandomOptionExtDst4=%d"
//		",@nRandomOptionExtAdj4=%d "
//	
//
//		, req.m_nPrice
//		, req.m_nOnePrice
//        , req.m_szItemName
//		, req.m_szPlayerName
//		, req.m_dwItemType1
//		, req.m_dwItemType2
//		, req.m_dwGrade
//		, req.m_nLimitLevel
//		, req.m_uRemainHour
//		, req.m_eRegistMode
//
//		, req.m_uPlayerID
//		, g_appInfo.dwSys
//		, req.m_kItemElem.m_dwItemId
//		, req.m_kItemElem.m_dwObjIndex
//		, req.m_kItemElem.m_nItemNum
//		, req.m_kItemElem.GetSerialNumber()
//		, req.m_kItemElem.GetOption()
//		, req.m_kItemElem.m_bItemResist
//		, req.m_kItemElem.m_nResistAbilityOption
//		, req.m_kItemElem.m_byFlag
//		, req.m_kItemElem.m_bCharged
//		, req.m_kItemElem.m_dwKeepTime
//		, static_cast<__int64>( 0 ) // pItemElem->GetRandomOptItemId()
//		, req.m_kItemElem.m_nHitPoint
//		, req.m_kItemElem.m_nRepairNumber
//		, req.m_kItemElem.m_nRepair
//		, req.m_kItemElem.IsTransformVisPet()
//		, req.m_kItemElem.GetUltimatePiercingSize()
//		, req.m_kItemElem.GetUltimatePiercingItemID( 0 )
//		, req.m_kItemElem.GetUltimatePiercingItemID( 1 )
//		, req.m_kItemElem.GetUltimatePiercingItemID( 2 )
//		, req.m_kItemElem.GetUltimatePiercingItemID( 3 )
//		, req.m_kItemElem.GetUltimatePiercingItemID( 4 )
//		, req.m_kItemElem.GetGeneralPiercingSize()
//		, req.m_kItemElem.GetGeneralPiercingItemID( 0 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 1 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 2 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 3 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 4 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 5 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 6 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 7 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 8 )
//		, req.m_kItemElem.GetGeneralPiercingItemID( 9 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 0 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 1 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 2 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 3 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 4 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 5 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 6 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 7 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 8 )
//		, req.m_kItemElem.GetVisPetPiercingItemKeepTime( 9 )
//		, ( req.m_kItemElem.m_pPet ? 1 : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetKind() : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetLevel() : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetExp() : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetEnergy() : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetLife() : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_D ) : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_C ) : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_B ) : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_A ) : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetAvailLevel( PL_S ) : 0 )
//		, ( req.m_kItemElem.m_pPet ? req.m_kItemElem.m_pPet->GetName() : "" )
//		, 0 //SaveSerialNumberStatus( pMover->m_Inventory.GetAllocedSerialNumber() )
//		, 0 // Managed Serial Number Id
//		, req.m_kItemElem.GetCoupleId()
//		, req.m_kItemElem.GetLevelDown()
//		, req.m_kItemElem.GetRandomOptionOriginID()
//		, req.m_kItemElem.GetRandomOptionExtensionSize()
//		, req.m_kItemElem.GetRandomOptionExtensionFlag()
//		, req.m_kItemElem.GetRandomOptionExtensionDstID( 0 )
//		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 0 )
//		, req.m_kItemElem.GetRandomOptionExtensionDstID( 1 )
//		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 1 )
//		, req.m_kItemElem.GetRandomOptionExtensionDstID( 2 )
//		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 2 )
//		, req.m_kItemElem.GetRandomOptionExtensionDstID( 3 )
//		, req.m_kItemElem.GetRandomOptionExtensionAdjValue( 3 )
//	);
//
//	if( pQuery->PrepareQuery( szQuery ) == FALSE )
//	{
//			FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );	return;
//	}
//
////	int i					= 0;
////	SQLINTEGER cbLen		= SQL_NTS;
////	int nRet				= 0;
//
////	pQuery->BindParameter( ++i, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
//
//	FLPacketWDB_RegistAck ack;
//	ack.m_uPlayerID				= req.m_uPlayerID;
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
////		ack.m_eResult				= FLPacketWDB_RegistAck::E_FAILURE;
////		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	const int nRet				= pQuery->GetInt( "nRet" );
//	if( nRet != 0 )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "[ ret(%d) (%s) ]" ), nRet, szQuery ); 
//		return;
//	}
//
//	//ItemLog
//	{
//		LogItemInfo info;
//		MakeItemLogInfo( info, "s", "ConsignmentSale RegistEnd"
//			, req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, req.m_kItemElem, req.m_kItemElem.m_nItemNum, 0, 0 );
//		if( Exec_ItemLogQuery( *pLogQuery, info ) == FALSE )
//			FLERROR_LOG( PROGRAM_NAME, _T( "[ (%s) ]" ), szQuery ); 
//	}
//
//	ack.m_eResult				= FLPacketWDB_RegistAck::E_SUCCESS;
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}
//
//
//void CDbManager::ConsignmentSale_CancelRegisted( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_CancelRegistedReq req;
//	req.Deserialize( ar );
//
//
//	if( DB_CONSIGNMENT_SALE()._AddReserve( req.m_u64SaleSRL ) == false )
//	{
//		FLPacketWDB_CancelRegistedAck ack;
//		ack.m_uPlayerID			= req.m_uPlayerID;
//		ack.m_u64SaleSRL		= req.m_u64SaleSRL;
//		ack.m_eResult			= FLPacket_CancelRegistedAck::E_AREADY_CANCELED;
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		return;
//	}
//
//
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	//FLSPrintf( szQuery, _countof( szQuery ), "{ call test2_uspConsignmentSale_Delete ( ?, ?, ? ) }" );
//	FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call test2_uspConsignmentSale_Delete ( ?, ?, ? ) }" );
//	//FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call test2_uspConsignmentSale_Delete ( '%s', '%s', %I64u ) }",
//	//	szPlayerId,	szServerIndex, req.m_u64SaleSRL	);
//
//
//	if( pQuery->PrepareQuery( szQuery ) == FALSE )
//	{
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );		return;
//	}
//
//	int i					= 0;
//	SQLINTEGER cbLen		= SQL_NTS;
//	int nRet				= 0;
//
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
//	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szPlayerId)-1,		0, szPlayerId,			0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex,		0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_INTEGER,						0,	0, &req.m_u64SaleSRL,	0, 0 );
//
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	FLPacketWDB_CancelRegistedAck ack;
//	ack.m_uPlayerID			= req.m_uPlayerID;
//	ack.m_u64SaleSRL		= req.m_u64SaleSRL;
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	if( nRet != 0 )
//	{
//		ack.m_eResult			= FLPacket_CancelRegistedAck::E_AREADY_CANCELED;
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		return;
//	}
//
//	//FLPacketWDB_CancelRegistedAck ack;
//	//ack.m_uPlayerID			= req.m_uPlayerID;
//	//ack.m_u64SaleSRL		= req.m_u64SaleSRL;
//
//	//const int nError		= pQuery->GetInt( "nError" );
//	//if( nError != CQuery::CQUERYNOCOL )		//에러가 있다면
//	//{
//	//	switch ( nError )
//	//	{
//	//	case 1:		case 2:
//	//		{
//	//			ack.m_eResult			= FLPacket_CancelRegistedAck::E_AREADY_CANCELED;
//	//			
//	//		}
//	//		break;
//	//	default:
//	//		ack.m_eResult			= FLPacket_CancelRegistedAck::E_FAILURE;
//	//		FLERROR_LOG( PROGRAM_NAME, _T( "[ INDALID Error ID(%d) ]" ), nError );
//	//		break;
//	//	}
//	//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//	//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//	//	return;
//	//}
//
//
////	ack.m_eRegistMode		= static_cast<E_REGIST_MODE>( pQuery->GetInt( "eRegistMode" ) );
//
//	//if( pQuery->MoreResults() == FALSE )
//	//{
//	//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//	//	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//	//	return;
//	//}
//
//	//if( pQuery->Fetch() == FALSE )		// NO_DATA
//	//{ 
//	//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//	//	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//	//	return;
//	//}
//
////	ack.m_u64SaleSRL		= pQuery->GetInt64( "T2.u64ItemInstanceSRL" );
//	
//	FLItemElem kItemElem;
//	int nSerialNumberStatus	= 0;
//	LoadOneItemCommon( *pQuery, /*"T2",*/ 0, 0, kItemElem, nSerialNumberStatus );
//
//
//	//ItemLog
//	{
//		LogItemInfo info;
//		MakeItemLogInfo( info, "s", "ConsignmentSale CancelComplete"
//			, req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kItemElem, kItemElem.m_nItemNum, 0, 0 );
//		if( Exec_ItemLogQuery( *pLogQuery, info ) == FALSE )
//			FLERROR_LOG( PROGRAM_NAME, _T( "[ (%s) ]" ), szQuery ); 
//	}
//	
//
//	PostMail( *pQuery, req.m_uPlayerID, 0, kItemElem, 0, "", "" );
//
//	ack.m_eResult				= FLPacket_CancelRegistedAck::E_SUCCESS;
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//
//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//}
//
//void CDbManager::ConsignmentSale_RegistedInfoList( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_RegistedInfoListReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_GetRegistedList ( ?, ? ) }" );
//
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );
//
//	int i					= 0;
//	SQLINTEGER cbLen = SQL_NTS;
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szPlayerId)-1,	0, szPlayerId, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,		0, szServerIndex, 0, &cbLen );
//
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	FLPacketWDB_RegistedInfoListAck ack;
//	ack.m_uPlayerID				= req.m_uPlayerID;
//
//	// 첫번째 쿼리결과를 채운다.
//	while( pQuery->Fetch() == TRUE )		// NO_DATA
//	{ 
//		int nSerialNumberStatus			= 0;
//
//		FLOneSaleInfo kOneSaleInfo;
//		LoadOneSaleInfoHeader( *pQuery, /*"T1",*/ kOneSaleInfo );
//		LoadOneItemCommon( *pQuery, /*"T2",*/ 0, 0, kOneSaleInfo.kItemElem, nSerialNumberStatus );
//
//		const FLTm tmCurr			= DATE_TIMER().GetCurrentTime();
//		const long nExpiredSec		= FLTimeToSecond( kOneSaleInfo.tmExpiredDate.year, kOneSaleInfo.tmExpiredDate.hour, kOneSaleInfo.tmExpiredDate.minute, kOneSaleInfo.tmExpiredDate.second );
//		const long nCurrSec			= FLTimeToSecond( tmCurr.year, tmCurr.hour, tmCurr.minute, tmCurr.second );
//		kOneSaleInfo.dwRemainSec	= ( nExpiredSec > nCurrSec ) ? ( nExpiredSec - nCurrSec ) : 0;
//		
//		ack.m_vecSaleInfoList.push_back( kOneSaleInfo );
//	}
//
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//
//}
//
//void CDbManager::ConsignmentSale_GetPrice( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_GetPriceReq req;
//	req.Deserialize( ar );
//
//	if( DB_CONSIGNMENT_SALE()._AddReserve( req.m_u64SaleSRL ) == false )
//	{	
//		FLPacketWDB_GetPriceAck ack;
//		ack.m_uPlayerID			= req.m_uPlayerID;
//		ack.m_u64SaleSRL		= req.m_u64SaleSRL;
//		ack.m_eResult			= FLPacketWDB_GetPriceAck::E_ALREADY_DELETE;
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		return;
//	}
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_GetPrice ( ?, ? ) }" );
//
//	//OUT
//	int i		= 0;
//	SQLINTEGER cbLen	= SQL_NTS;
//
//	//IN
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_INTEGER,0,							0, &req.m_u64SaleSRL,	0, 0 );
//
//	//EXEC
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		////이미 삭제된 데이터 입니다.
//		//ack.m_eResult		= FLPacketWDB_GetPriceAck::E_ALREADY_DELETE;
//		//CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//
//	FLPacketWDB_GetPriceAck ack;
//	ack.m_uPlayerID			= req.m_uPlayerID;
//	ack.m_u64SaleSRL		= req.m_u64SaleSRL;
//
//	//const int nError		= pQuery->GetInt( "nError" );
//	//if( nError != CQuery::CQUERYNOCOL )		//에러가 있다면
//	//{
//	//	switch ( nError )
//	//	{
//	//	case 1:
//	//		{
//	//			ack.m_eResult		= FLPacketWDB_GetPriceAck::E_ALREADY_DELETE;
//	//		}
//	//		break;
//	//	default:
//	//		ack.m_eResult			= FLPacketWDB_GetPriceAck::E_FAILURE;
//	//		FLERROR_LOG( PROGRAM_NAME, _T( "[ INDALID Error ID(%d) ]" ), nError );
//	//		break;
//	//	}
//	//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//	//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//	//	return;
//	//}
//
//	const u_long uSellerPlayerID	= static_cast< u_long >( pQuery->GetInt( "m_idPlayer" ) );
//
//	if( req.m_uPlayerID == uSellerPlayerID )
//	{
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		ack.m_eResult			= FLPacketWDB_GetPriceAck::E_CANNOT_BUY_SAME_OWNER_ITEM;	
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		return;
//	}
//
//
//	ack.m_eResult			= FLPacketWDB_GetPriceAck::E_SUCCESS;
//	ack.m_nPrice			= pQuery->GetInt( "nPrice" );
//	
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}
//
//void CDbManager::ConsignmentSale_ClearReserveInfo( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_ClearReservedInfoNoti noti;
//	noti.Deserialize( ar );
//
//	DB_CONSIGNMENT_SALE()._RemoveReserve( noti.m_u64SaleSRL );
//
//	//char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	//FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_TryClearReserveInfo ( ?, ? ) }" );
//
//	////OUT
//	//int i		= 0;
//	//SQLINTEGER cbLen	= SQL_NTS;
//
//	////IN
//	//TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	//FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//
//	//pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//	//pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_INTEGER,0,							0, &noti.m_u64SaleSRL,	0, 0 );
//
//	////EXEC
//	//if( pQuery->Exec( szQuery ) == FALSE )
//	//{ 
//	//	FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//	//	return;
//	//}
//}
//
//
//void CDbManager::ConsignmentSale_BuyItem( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_BuyItemReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	//FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_BuyItem ( ?, ? ) }" );
//	FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call test_uspConsignmentSale_BuyItem ( ?, ? ) }" );
//
//	//OUT
//	int i					= 0;
//	SQLINTEGER cbLen		= SQL_NTS;
//	int nRet				= 0;
//
//	//IN
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_INTEGER,0,							0, &req.m_u64SaleSRL,	0, 0 );
//
//	//EXEC
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//
//	FLPacketWDB_BuyItemAck ack;
//	ack.m_uPlayerID			= req.m_uPlayerID;
//	ack.m_u64SaleSRL		= req.m_u64SaleSRL;
//
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//
//		ack.m_eResult			= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//
//		//FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//	//	ack.m_eResult				= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
//	//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//
//		// Logging & 골드 복구
//		LogItemInfo info;
//		FLItemElem kItemElem;
//		MakeItemLogInfo( info, "s", "ConsignmentSale Buy-RecoverPrice", req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kItemElem, kItemElem.m_nItemNum, 0, req.m_nPrice );
//		if( Exec_ItemLogQuery( *pLogQuery, info ) == FALSE )
//			FLERROR_LOG( PROGRAM_NAME, _T( "[ (%s) ]" ), szQuery ); 
//
//		PostMail( *pQuery, req.m_uPlayerID, 0, kItemElem, req.m_nPrice, "", "" );
//		return;
//	}
//
//	if( nRet != 0 )
//	{
//		ack.m_eResult			= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
//		DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//
//		// Logging & 골드 복구
//		LogItemInfo info;
//		FLItemElem kItemElem;
//		MakeItemLogInfo( info, "s", "ConsignmentSale Buy-RecoverPrice", req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kItemElem, kItemElem.m_nItemNum, 0, req.m_nPrice );
//		if( Exec_ItemLogQuery( *pLogQuery, info ) == FALSE )
//			FLERROR_LOG( PROGRAM_NAME, _T( "[ (%s) ]" ), szQuery ); 
//
//		PostMail( *pQuery, req.m_uPlayerID, 0, kItemElem, req.m_nPrice, "", "" );
//		return;
//	}
//
//
//	//const int nError		= pQuery->GetInt( "nError" );
//	//if( nError != CQuery::CQUERYNOCOL )		//에러가 있다면
//	//{
//	//	switch ( nError )
//	//	{
//	//	case 1:
//	//	case 2:
//	//		{
//	//			ack.m_eResult				= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
//	//			CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//	//		}
//	//		break;
//	//	default:
//	//		FLERROR_LOG( PROGRAM_NAME, _T( "[ INDALID Error ID(%d) ]" ), nError );
//	//		break;
//	//	}
//
//	//	FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL - PROCESSING TIME 40SEC OVER(%s) ]" ), szQuery );
//
//	//	// Logging & 골드 복구
//	//	LogItemInfo info;
//	//	FLItemElem kItemElem;
//	//	MakeItemLogInfo( info, "s", "ConsignmentSale Buy-RecoverPrice", req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kItemElem, kItemElem.m_nItemNum, 0, req.m_nPrice );
//	//	if( Exec_ItemLogQuery( *pLogQuery, info ) == FALSE )
//	//		FLERROR_LOG( PROGRAM_NAME, _T( "[ (%s) ]" ), szQuery ); 
//
//	//	PostMail( *pQuery, req.m_uPlayerID, 0, kItemElem, req.m_nPrice, "", "" );
//
//	//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//	//	return;
//	//}
//
//	//메일을 보내기위해 아이템 정보를 가져옴
//	FLItemElem kItemElem;
//	int nSerialNumberStatus = 0;
//
//	//pQuery->SetTable( "T1" );
//	const int eRegistMode	= pQuery->GetInt( "eRegistMode" );
//	const int nGold			= pQuery->GetInt( "nPrice" );
//	const u_long uPlayerID	= static_cast<u_long>( pQuery->GetInt( "m_idPlayer" ) );
//	LoadOneItemCommon( *pQuery, /*"T2",*/ 0, 0, kItemElem, nSerialNumberStatus );
//
//	LogItemInfo info;
//	MakeItemLogInfo( info, "s", "ConsignmentSale BuyEnd"
//		, req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kItemElem, kItemElem.m_nItemNum, 0, 0 );
//	Exec_ItemLogQuery( *pLogQuery, info );
//
//	PostMail( *pQuery, req.m_uPlayerID, 0, kItemElem, 0, "", "" );
//
//	ack.m_eResult				= FLPacketWDB_BuyItemAck::E_SUCCESS;
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//
//	FLPacketWDB_SellItemNoti noti;
//
//	noti.m_uPlayerID			= uPlayerID;
//	FLStrcpy( noti.m_szPlayerName, _countof( noti.m_szPlayerName ), req.m_szPlayerName );
//	noti.m_dwItemID				= kItemElem.m_dwItemId;
//	noti.m_nGold				= nGold;
//	noti.m_eRegistMode			= static_cast< E_REGIST_MODE >( eRegistMode );	
//
//	DB_CONSIGNMENT_SALE()._RemoveReserve( req.m_u64SaleSRL );
//	CDPTrans::GetInstance()->SendPacket( DPID_ALLPLAYERS, &noti );
//}

//void CDbManager::ConsignmentSale_BuyItem( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	//FLPacket_CancelRegistedReq 재활용
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_BuyItemReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ ?=call test_uspConsignmentSale_BuyItem2 ( ?, ? ) }" );
//	if( pQuery->PrepareQuery( szQuery ) == FALSE )
//	{
//		return;
//	}
//
//	//OUT
//	int i		= 0;
//	SQLINTEGER cbLen	= SQL_NTS;
//	int nRet			= 0;
//
//	pQuery->BindParameter( ++i, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
//
//	//IN
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_INTEGER,0,							0, &req.m_u64SaleSRL,	0, 0 );
//
//	//EXEC
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	FLPacketWDB_BuyItemAck ack;
//	ack.m_uPlayerID			= req.m_uPlayerID;
//
//	//const int nRet			= pQuery->GetInt( "nRet" );
//	if( nRet == 0 )
//	{
//
//		ack.m_eResult				= FLPacketWDB_BuyItemAck::E_ALREADY_DELETE;
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		return;
//	}
//
//	//메일을 보내기위해 아이템 정보를 가져옴
//	FLItemElem kItemElem;
//	int nSerialNumberStatus = 0;
//	LoadOneItemCommon( *pQuery, "", 0, 0, kItemElem, nSerialNumberStatus );
//
//	ack.m_eResult				= FLPacketWDB_BuyItemAck::E_SUCCESS;
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}


//void CDbManager::ConsignmentSale_CollectSaleGold( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_CollectSaleGoldReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ ? = call test_uspConsignmentSale_CollectSaleGold ( ?, ?, ? ) }" );
//
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );
//
//	int i					= 0;
//	SQLINTEGER cbLen = SQL_NTS;
//	int nRet				= 0;
//	pQuery->BindParameter( ++i, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &nRet, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szPlayerId)-1,		0, szPlayerId, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_INTEGER,0,							0, &req.m_u64SaleGoldSRL,	0, 0 );
//
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s " ), szQuery ); 
//		return;
//	}
//
//	FLPacketWDB_CollectSaleGoldAck ack;
//	ack.m_uPlayerID				= req.m_uPlayerID;
//	ack.m_u64SaleGoldSRL		= req.m_u64SaleGoldSRL;
//
//
//	if( pQuery->Fetch() == FALSE )		// NO_DATA
//	{ 
//		ack.m_eResult				= FLPacketWDB_CollectSaleGoldAck::E_AREADY_COLLECT;
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s " ), szQuery ); 
//		return;
//	}
//
//	if( nRet != 0 )
//	{
//		ack.m_eResult			= FLPacketWDB_CollectSaleGoldAck::E_AREADY_COLLECT;
//		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//		return;
//	}
//
//
//	//const int nError			= pQuery->GetInt( "nError" );
//	//if( nError != CQuery::CQUERYNOCOL )		//에러가 있다면
//	//{		
//	//	switch ( nError )
//	//	{
//	//	case 1:
//	//	case 2:
//	//		{
//	//			ack.m_eResult				= FLPacketWDB_CollectSaleGoldAck::E_AREADY_COLLECT;
//	//			CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//	//		}
//	//		break;
//	//	default:
//	//		FLERROR_LOG( PROGRAM_NAME, _T( "[ INDALID Error ID(%d) ]" ), nError );
//	//		break;
//	//	}
//	//	return;
//	//}
//
//	ack.m_u64SaleGoldSRL		= req.m_u64SaleGoldSRL;//->GetInt64( "u64SaleGoldSRL" );
//	ack.m_nGold					= pQuery->GetInt( "nGold" );
//
//	//골드 로그
//	//(x)MakeItemLogInfo( info, "s", "ConsignmentSale RegistEnd"
//	//	, req.m_szPlayerName, req.m_uPlayerID, NULL_PLAYER_ID, req.m_dwWorldID, kItemElem, kItemElem.m_nItemNum, req.m_nGold, 0 );
//	//@@@@@@@@@@@@@@@@@@@@@
//
//	PostMail( *pQuery, req.m_uPlayerID, 0, FLItemElem(), ack.m_nGold, "", "" );
//
//	ack.m_eResult				= FLPacketWDB_CollectSaleGoldAck::E_SUCCESS;
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}
//
//void CDbManager::ConsignmentSale_Search( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_SearchReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_Search ( ?,?, ?,?, ?, ?,?,?,?, ?,?, ?,?, ?,?, ?,? ) }" );
//
//	TCHAR szServerIndex[ SERVER_INDEX_BUFFER_LEN ]	= { 0 };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//	unsigned int LIST_COUNT_MAX		= CONSIGNMENT_SALE_ITEM_LIST_MAX;
//
//	TCHAR szOrderingColumn[ FLPacketWDB_SearchReq::ODERING_COLUMN_STRING_BUFFER_MAX ] = { 0 };
//	FLSPrintf( szOrderingColumn, _countof( szOrderingColumn ), FLPacketWDB_SearchReq::EnumToString( req.m_eOrderingOption ) );
//
//    
//	int i					= 0;
//	SQLINTEGER cbLen = SQL_NTS;
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof( szServerIndex )-1,0, szServerIndex,			0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,							0, &LIST_COUNT_MAX,			0, 0 );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,		_countof( szOrderingColumn )-1,0, szOrderingColumn,		0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,	0,							0, &req.m_bASC,				0, 0 );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,							0, &req.m_dwPage,			0, 0 );
//
//	for( size_t index = 0; index < FLPacketWDB_SearchReq::GRADE_MULTI_CHECK_MAX; ++index )
//	{
//		pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,						0, &req.m_arrGrade[ index ],0, 0 );
//	}
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,							0, &req.m_nLimitLevelMin,		0, 0 );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,							0, &req.m_nLimitLevelMax,		0, 0 );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,	0,							0, &req.m_nAbilityOptionMin,0, 0 );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,	0,							0, &req.m_nAbilityOptionMax,0, 0 );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,							0, &req.m_dwItemType1,		0, 0 );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER,	0,							0, &req.m_dwItemType2,		0, 0 );
//
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,		_countof( req.m_szSearchName )-1,	0, req.m_szSearchName,		0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,	0,							0, &req.m_bSearchBySameName,0, 0 );
//
//
//	FLPacketWDB_SearchAck ack;
//	ack.m_uPlayerID				= req.m_uPlayerID;
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s " ), szQuery ); 
//		return;
//	}
//
//	while( pQuery->Fetch() == FALSE )
//	{
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s " ), szQuery ); 
//		return;
//	}
//
//	ack.m_dwCountMax		= pQuery->GetInt( "nTotCount" );
//	ack.m_dwPage			= min( req.m_dwPage, ack.m_dwCountMax / LIST_COUNT_MAX );
//
//	// 첫번째 쿼리결과를 채운다.
//
//	if( pQuery->MoreResults() == TRUE )
//	{
//		while( pQuery->Fetch() == TRUE )		// NO_DATA
//		{ 
//			int nSerialNumberStatus			= 0;
//
//			FLOneSaleInfo kOneSaleInfo;// = new FLOneSaleInfo;
//			LoadOneSaleInfoHeader( *pQuery, /*"T1",*/ kOneSaleInfo );
//			LoadOneItemCommon( *pQuery, /*"T2",*/ 0, 0, kOneSaleInfo.kItemElem, nSerialNumberStatus );
//
//			const FLTm tmCurr			= DATE_TIMER().GetCurrentTime();
//			const long nExpiredSec		= FLTimeToSecond( kOneSaleInfo.tmExpiredDate.year, kOneSaleInfo.tmExpiredDate.hour, kOneSaleInfo.tmExpiredDate.minute, kOneSaleInfo.tmExpiredDate.second );
//			const long nCurrSec			= FLTimeToSecond( tmCurr.year, tmCurr.hour, tmCurr.minute, tmCurr.second );
//			kOneSaleInfo.dwRemainSec	= ( nExpiredSec > nCurrSec ) ? ( nExpiredSec - nCurrSec ) : 0;
//			ack.m_vecSaleInfoList.push_back( kOneSaleInfo );
//		}
//	}
//
//
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}
//
//void CDbManager::ConsignmentSale_SaleGoldInfoList( CQuery* pQuery, CQuery* pLogQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
//{
//	using namespace nsConsignmentSale;
//
//	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
//
//	FLPacketWDB_SaleGoldInfoListReq req;
//	req.Deserialize( ar );
//
//	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
//	FLSPrintf( szQuery, _countof( szQuery ), "{ call test_uspConsignmentSale_GetSaleGoldInfoList ( ?, ? ) }" );
//
//	TCHAR szServerIndex[SERVER_INDEX_BUFFER_LEN]	= { 0, };
//	TCHAR szPlayerId[PLAYER_ID_BUFFER_LEN]		= { 0, };
//	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
//	FLSPrintf( szPlayerId, _countof( szPlayerId ), "%07d", req.m_uPlayerID );
//
//	int i					= 0;
//	SQLINTEGER cbLen = SQL_NTS;
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szPlayerId)-1,		0, szPlayerId, 0, &cbLen );
//	pQuery->BindParameter( ++i, SQL_PARAM_INPUT, SQL_C_CHAR,  SQL_CHAR,		_countof(szServerIndex)-1,	0, szServerIndex, 0, &cbLen );
//
//	if( pQuery->Exec( szQuery ) == FALSE )
//	{ 
//		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); 
//		return;
//	}
//
//	FLPacketWDB_SaleGoldInfoListAck ack;
//	ack.m_uPlayerID				= req.m_uPlayerID;
//
//	while( pQuery->Fetch() == TRUE )		// NO_DATA
//	{ 
//		FLPacket_SaleGoldInfoListAck::FLSaleGoldInfo info;
//		//pQuery->SetTable( "T1" );
//		info.u64SaleGoldSRL		= pQuery->GetInt64( "u64ItemInstanceSRL" );
//		//info.dwItemID			= static_cast<DWORD>( pQuery->GetInt( "dwItemID" ) );
//		info.nGold				= pQuery->GetInt( "nGold" );
//		
//		char szSaleDate[ DB_DATETIME_BUFFER_LEN ] = { 0 };
//		pQuery->GetStr( "tRegDate", szSaleDate, _countof( szSaleDate ) );
//		szSaleDate[ _countof( szSaleDate ) - DB_DATETIME_MILLISECOND_LEN - 1 ] = '\0';
//		info.kSaleDate		= FLStringToFLTm( szSaleDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS );
//
//		FLItemElem kItemElem;
//		int nSerialNumberStatus = 0;
//		LoadOneItemCommon( *pQuery, /*"T2",*/ 0, 0, kItemElem, nSerialNumberStatus );
//		info.kItemElem		= kItemElem;
//
//		ack.m_vecSaleGoldInfo.push_back( info );
//	}
//
//	//MoreResult // kItemElem
//
//
//
//	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &ack );
//}

//////////////////////////////////////////////////////////////////////////
void	CDbManager::CharacterServerTransform( CQuery* pQuery, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus )
{
	CAr ar( pDbOverlappedPlus->lpBuf, pDbOverlappedPlus->uBufSize );
	FLPacketCharacterServerTransformNoti kRecvPacket;
	kRecvPacket.Deserialize( ar );

	TCHAR szServerIndex[3] = { 0, };
	TCHAR szTransformServerIndex[3] = { 0, };
	TCHAR szPlayerID[8] = { 0, };

	FLSPrintf( szServerIndex, _countof( szServerIndex ), "%02d", g_appInfo.dwSys );
	FLSPrintf( szTransformServerIndex, _countof( szTransformServerIndex ), "%02d", kRecvPacket.dwTransformServerIndex );
	FLSPrintf( szPlayerID, _countof( szPlayerID ), "%07d", kRecvPacket.dwPlayerID );

	TCHAR szQuery[ MAX_QUERY_SIZE ] = { 0, };

	const DWORD MAX_PARAMETER = 5;

	MakeQueryBindParameter( szQuery, _countof( szQuery ), "usp_CharMove_UseItem", MAX_PARAMETER );

	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szTransformServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		7,		 szPlayerID );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		32,		 kRecvPacket.szAccount );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		32,		 kRecvPacket.szBankPassword );

	if( pQuery->Exec( szQuery ) == TRUE )
	{
		if( pQuery->Fetch() == TRUE )
		{
			int nRet = pQuery->GetInt( "fError" );
			if( nRet != 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "usp_CharMove_UseItem return fail. ret:[%d]" ), nRet );
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "fetch usp_CharMove_UseItem fail." ) );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "exec usp_CharMove_UseItem fail. [%s], [%s], [%s], [%s], [%s]" )
			, szServerIndex, szTransformServerIndex, szPlayerID, kRecvPacket.szAccount, kRecvPacket.szBankPassword );
	}
}
//////////////////////////////////////////////////////////////////////////


// END Character Inventory
//////////////////////////////////////////////////////////////////////////

BOOL	CDbManager::GetSerialNumberStatus( int nSerialNumberStatus )
{
	//////////////////////////////////////////////////////////////////////////
	// 시리얼넘버 변환 테그 확인
	BOOL bNewSerialNumber = FALSE;

	if( CSerialNumber::GetInstance()->IsManaged() == false )
	{
		bNewSerialNumber = TRUE;
	}
	else
	{
		if( nSerialNumberStatus == g_appInfo.nSerialNumberStatus )
		{
			bNewSerialNumber = TRUE;
		}
	}

	return bNewSerialNumber;
	//////////////////////////////////////////////////////////////////////////
}

int		CDbManager::SaveSerialNumberStatus( BOOL bNewSerialNumber )
{
	int nSerialNumberStatus = 0;

	if( CSerialNumber::GetInstance()->IsManaged() == true )
	{
		if( bNewSerialNumber == TRUE )
		{
			nSerialNumberStatus = g_appInfo.nSerialNumberStatus;
		}
	}

	return nSerialNumberStatus;
}

void	CDbManager::RefreshTaskBar( CMover* pMover )
{
	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )
	{
		for( int i = 0; i < MAX_SLOT_APPLET; ++i )
		{
			if( pMover->m_UserTaskBar.m_aSlotApplet[ i ].m_dwShortcut == SHORTCUT_ITEM )
			{
				if( pMover->m_UserTaskBar.m_aSlotApplet[ i ].m_dwUserId > 0 )
				{
					DWORD dwObjId = pMover->m_Inventory.GetObjIdByObjIndex( pMover->m_UserTaskBar.m_aSlotApplet[ i ].m_dwUserId );
					if( dwObjId != NULL_ID )
					{
						pMover->m_UserTaskBar.m_aSlotApplet[ i ].m_dwId = dwObjId;
					}
				}
			}
		}
	}

	for( int i = 0; i < MAX_SLOT_ITEM_COUNT; ++i )
	{
		for( int j = 0; j < MAX_SLOT_ITEM; ++j )
		{
			if( pMover->m_UserTaskBar.m_aSlotItem[ i ][ j ].m_dwShortcut == SHORTCUT_ITEM )
			{
				if( pMover->m_UserTaskBar.m_aSlotItem[ i ][ j ].m_dwUserId > 0 )
				{
					DWORD dwObjId = pMover->m_Inventory.GetObjIdByObjIndex( pMover->m_UserTaskBar.m_aSlotItem[ i ][ j ].m_dwUserId );
					if( dwObjId != NULL_ID )
					{
						pMover->m_UserTaskBar.m_aSlotItem[ i ][ j ].m_dwId = dwObjId;
					}
				}
			}
		}
	}
}



void CDbManager::LoadOneSaleInfoHeader( CQuery & kQuery/*, const char * pszTableName*/, nsConsignmentSale::FLOneSaleInfo & o_kOneSaleInfo )
{
	//kQuery.SetTable( pszTableName );

	o_kOneSaleInfo.u64SaleSRL		= kQuery.GetInt64( "u64ItemInstanceSRL" );
	o_kOneSaleInfo.nItemCostTot		= kQuery.GetInt( "nPrice" );

	kQuery.GetStr( "szItemName", o_kOneSaleInfo.szItemName, _countof( o_kOneSaleInfo.szItemName ) );
	kQuery.GetStr( "szPlayerName", o_kOneSaleInfo.szPlayerName, _countof( o_kOneSaleInfo.szPlayerName ) );

	char szExpiredDate[ DB_DATETIME_BUFFER_LEN ] = { 0 };
	kQuery.GetStr( "tExpiredDate", szExpiredDate, _countof( szExpiredDate ) );
	szExpiredDate[ _countof( szExpiredDate ) - DB_DATETIME_MILLISECOND_LEN - 1 ] = '\0';
	o_kOneSaleInfo.tmExpiredDate	= FLStringToFLTm( szExpiredDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS );
}

void CDbManager::LoadOneSaleInfo_ForServer( CQuery & kQuery, FLDBConsignmentSale::FLSaleInfo & o_kSaleInfo )
{
	//kQuery.SetTable( pszTableName );
	o_kSaleInfo.u64ItemInstanceSRL	= kQuery.GetInt64( "u64ItemInstanceSRL" );
	o_kSaleInfo.uPlayerID			= kQuery.GetInt( "m_idPlayer" );
	o_kSaleInfo.nPrice				= kQuery.GetInt( "nPrice" );
	o_kSaleInfo.nOnePrice			= kQuery.GetInt( "nOnePrice" );
	kQuery.GetStr( "szItemName", o_kSaleInfo.szItemName, _countof( o_kSaleInfo.szItemName ) );
	kQuery.GetStr( "szPlayerName", o_kSaleInfo.szPlayerName, _countof( o_kSaleInfo.szPlayerName ) );

	o_kSaleInfo.dwItemType1			= static_cast< DWORD >( kQuery.GetInt( "dwItemType1" ) );
	o_kSaleInfo.dwItemType2			= static_cast< DWORD >( kQuery.GetInt( "dwItemType2" ) );
	o_kSaleInfo.dwGrade				= static_cast< DWORD >( kQuery.GetInt( "dwGrade" ) );
	o_kSaleInfo.nLimitLevel			= kQuery.GetInt( "dwLimitLevel" );
	o_kSaleInfo.nAbilityOption		= kQuery.GetInt( "nAbilityOption" );
	o_kSaleInfo.eRegistMode			= static_cast< E_REGIST_MODE >( kQuery.GetInt( "eRegistMode" ) );
	
	char szExpiredDate[ DB_DATETIME_BUFFER_LEN ] = { 0 };
	kQuery.GetStr( "tExpiredDate", szExpiredDate, _countof( szExpiredDate ) );
	szExpiredDate[ _countof( szExpiredDate ) - DB_DATETIME_MILLISECOND_LEN - 1 ] = '\0';
	o_kSaleInfo.tmExpiredDate	= FLStringToFLTm( szExpiredDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS );

	int nSerialNumberStatus			= 0;
	LoadOneItemCommon( kQuery, 0, 0, o_kSaleInfo.kItemElem, nSerialNumberStatus );
}


void CDbManager::LoadOneItemCommon( CQuery & kQuery/*, const char * pszTableName*/, const DWORD dwObjID, const DWORD dwCurrObjIndex, FLItemElem & o_kItemElem, int & o_nSerialNumberStatus )
{
	//kQuery.SetTable( pszTableName );

	o_kItemElem.m_dwObjId				= dwObjID;
	o_kItemElem.m_dwObjIndex			= dwCurrObjIndex;

	if( kQuery.IsValidColumn( "dwItemId" ) == true )
	{
		o_kItemElem.m_dwItemId			= static_cast<DWORD>( kQuery.GetInt( "dwItemId" ) );
	}

	if( kQuery.IsValidColumn( "nItemNum" ) == true )
	{
		o_kItemElem.m_nItemNum			= kQuery.GetInt( "nItemNum" );
	}

	if( kQuery.IsValidColumn( "nUniqueNum" ) == true )
	{
		o_kItemElem.SetSerialNumber( static_cast<DWORD>( kQuery.GetInt64( "nUniqueNum" ) ) );
	}

	if( kQuery.IsValidColumn( "nAbilityOption" ) == true )
	{
		o_kItemElem.SetAbilityOption( kQuery.GetInt( "nAbilityOption" ) );
	}

	if( kQuery.IsValidColumn( "nItemResist" ) == true )
	{
		o_kItemElem.SetItemResist( static_cast<BYTE>( kQuery.GetInt( "nItemResist" ) ) );
	}

	if( kQuery.IsValidColumn( "nResistAbilityOption" ) == true )
	{
		o_kItemElem.SetResistAbilityOption( kQuery.GetInt( "nResistAbilityOption" ) );
	}

	if( kQuery.IsValidColumn( "dwFlag" ) == true )
	{
		o_kItemElem.m_byFlag			= static_cast<BYTE>( kQuery.GetInt( "dwFlag" ) );
	}

	if( kQuery.IsValidColumn( "bCharged" ) == true )
	{
		o_kItemElem.m_bCharged			= static_cast<BOOL>( kQuery.GetInt( "bCharged" ) == 1 );
	}

	if( kQuery.IsValidColumn( "dwKeepTime" ) == true )
	{
		o_kItemElem.m_dwKeepTime		= static_cast<DWORD>( kQuery.GetInt( "dwKeepTime" ) );
	}

// 	//////////////////////////////////////////////////////////////////////////
// 	// mirchang_101215 기간제 아이템인데 기간셋팅이 안되어 있는 경우..
// 	if( o_kItemElem.m_dwKeepTime == 0 )
// 	{
// 		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( o_kItemElem.m_dwItemId );
// 		if( pItemProp != NULL )
// 		{
// 			if( ( pItemProp->dwFlag & IP_FLAG_PERIOD ) || ( o_kItemElem.m_byFlag & FLItemElem::period ) )
// 			{
// 				CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pItemProp->dwSkillTime, 0 );
// 				o_kItemElem.m_dwKeepTime	= static_cast<DWORD>( ( tm.GetTime() ) );
// 			}
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////

	__int64 n64RandomOptionId	= 0;
	if( kQuery.IsValidColumn( "nRandomOptItemId" ) == true )
	{
		n64RandomOptionId				= kQuery.GetInt64( "nRandomOptItemId" );
	}

	if( kQuery.IsValidColumn( "nHitPoint" ) == true )
	{
		o_kItemElem.m_nHitPoint			= kQuery.GetInt( "nHitPoint" );
	}

	if( kQuery.IsValidColumn( "nRepairNumber" ) == true )
	{
		o_kItemElem.m_nRepairNumber		= static_cast< BYTE >( kQuery.GetInt( "nRepairNumber" ) );
	}

	if( kQuery.IsValidColumn( "nRepair" ) == true )
	{
		o_kItemElem.m_nRepair			= kQuery.GetInt( "nRepair" );
	}

	if( kQuery.IsValidColumn( "nIsTransformVisPet" ) == true )
	{
		o_kItemElem.m_bTranformVisPet	= static_cast<BOOL>( kQuery.GetInt( "nIsTransformVisPet" ) == 1 );
	}

	if( kQuery.IsValidColumn( "nUltimatePiercingSize" ) == true )
	{
		o_kItemElem.SetUltimatePiercingSize( kQuery.GetInt( "nUltimatePiercingSize" ) );
	}

	for( size_t nCount = 0; nCount < o_kItemElem.GetUltimatePiercingSize(); ++nCount )
	{
		TCHAR szColumn[64] = { 0 };
		FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "dwUltimatePiercingId", ( nCount + 1 ) );
		if( kQuery.IsValidColumn( szColumn ) == true )
		{
			const DWORD dwItemId = static_cast<DWORD>( kQuery.GetInt( szColumn ) );
			o_kItemElem.SetUltimatePiercingItemID( nCount, dwItemId );
		}
	}

	if( kQuery.IsValidColumn( "nGeneralPiercingSize" ) == true )
	{
		o_kItemElem.SetGeneralPiercingSize( kQuery.GetInt( "nGeneralPiercingSize" ) );
	}

	for( size_t nCount = 0; nCount < o_kItemElem.GetGeneralPiercingSize(); ++nCount )
	{
		TCHAR szColumn[64] = { 0 };
		FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "dwGeneralPiercingId", ( nCount + 1 ) );
		if( kQuery.IsValidColumn( szColumn ) == true )
		{
			const DWORD dwItemId = static_cast<DWORD>( kQuery.GetInt( szColumn ) );
			o_kItemElem.SetGeneralPiercingItemID( nCount, dwItemId );
		}
	}

	if( o_kItemElem.m_bTranformVisPet == FALSE )
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( o_kItemElem.m_dwItemId );
		if( pItemProp != NULL && pItemProp->dwItemKind3 == IK3_PET && pItemProp->dwReferStat1 != PET_VIS )
		{
			if( o_kItemElem.GetGeneralPiercingSize() > 0 )
			{
				o_kItemElem.m_bTranformVisPet = TRUE;
			}
		}
	}

	if( o_kItemElem.IsVisPet() == TRUE )
	{
		o_kItemElem.SetVisPetPiercingSize( o_kItemElem.GetGeneralPiercingSize() );
		for( size_t nCount = 0; nCount < o_kItemElem.GetGeneralPiercingSize(); ++nCount )
		{
			TCHAR szColumn[64] = { 0 };
			FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "dwGeneralPiercingKeepTime", ( nCount + 1 ) );
			if( kQuery.IsValidColumn( szColumn ) == true )
			{
				const DWORD dwKeepTime = static_cast<DWORD>( kQuery.GetInt( szColumn ) );
				o_kItemElem.SetVisPetPiercingItemKeepTime( nCount, dwKeepTime );
			}
		}
	}

	if( kQuery.IsValidColumn( "nIsRarePet" ) == true )
	{
		const BOOL bIsRarePet = static_cast<BOOL>( kQuery.GetInt( "nIsRarePet" ) == 1 );
		if( bIsRarePet == TRUE )
		{
			SAFE_DELETE( o_kItemElem.m_pPet );
			CPet* pPet	= o_kItemElem.m_pPet = new CPet;

			if( kQuery.IsValidColumn( "nRarePetKind" ) == true )
			{
				const int nKind		= kQuery.GetInt( "nRarePetKind" );
				if( nKind > 0 )
				{
					pPet->SetKind( static_cast<BYTE>( nKind ) );
				}
			}

			if( kQuery.IsValidColumn( "nRarePetLevel" ) == true )
			{
				const int nLevel	= kQuery.GetInt( "nRarePetLevel" );
				if( nLevel > 0 )
				{
					pPet->SetLevel( static_cast<BYTE>( nLevel ) );
				}
			}

			if( kQuery.IsValidColumn( "nRarePetExp" ) == true )
			{
				const int nExp		= kQuery.GetInt( "nRarePetExp" );
				if( nExp > 0 )
				{
					pPet->SetExp( static_cast<DWORD>( nExp ) );
				}
			}

			if( kQuery.IsValidColumn( "nRarePetEnergy" ) == true )
			{
				const int nEnergy	= kQuery.GetInt( "nRarePetEnergy" );
				if( nEnergy > 0 )
				{
					pPet->SetEnergy( static_cast<WORD>( nEnergy ) );
				}
			}

			if( kQuery.IsValidColumn( "nRarePetLife" ) == true )
			{
				const int nLife		= kQuery.GetInt( "nRarePetLife" );
				if( nLife > 0 )
				{
					pPet->SetLife( static_cast<WORD>( nLife ) );
				}
			}

			for( BYTE nGrade = PL_D; nGrade <= pPet->GetLevel(); ++nGrade )
			{
				TCHAR szColumn[64] = { 0 };
				FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "nRarePetAvailLevel", nGrade );
				if( kQuery.IsValidColumn( szColumn ) == true )
				{
					const int nAvailLevel	= kQuery.GetInt( szColumn );
					if( nAvailLevel > 0 )
					{
						pPet->SetAvailLevel( nGrade, static_cast<BYTE>( nAvailLevel ) );
					}
				}
			}

			TCHAR szPetName[MAX_PET_NAME] = {0, };
			if( kQuery.IsValidColumn( "szRarePetName" ) == true )
			{
				kQuery.GetStr( "szRarePetName", szPetName, _countof( szPetName ) );
				if( strcmp( szPetName, "NULL" ) == 0 )
				{
					szPetName[0] = '\0';
				}
				pPet->SetName( szPetName );
			}
		}
	}

	if( kQuery.IsValidColumn( "nSerialNumberStatus" ) == true )
	{
		o_nSerialNumberStatus	= kQuery.GetInt( "nSerialNumberStatus" );
	}

	if( kQuery.IsValidColumn( "nSerialNumberId" ) == true )
	{
		const DWORD dwSerialNumberId = static_cast<DWORD>( kQuery.GetInt64( "nSerialNumberId" ) );
		UNREFERENCED_PARAMETER( dwSerialNumberId );
	}

	if( kQuery.IsValidColumn( "idCouplePlayer" ) == true )
	{
		const int nCouplePlayerId =  kQuery.GetInt( "idCouplePlayer" );
		if( nCouplePlayerId > 0 )
		{
			o_kItemElem.SetCoupleId( static_cast<DWORD>( nCouplePlayerId ) );
		}
	}

	if( kQuery.IsValidColumn( "nLevelDown" ) == true )
	{
		const int nLevelDown = kQuery.GetInt( "nLevelDown" );
		if( nLevelDown > 0 )
		{
			o_kItemElem.SetLevelDown( nLevelDown );
		}
	}

	if( kQuery.IsValidColumn( "nRandomOptionOriginId" ) == true )
	{
		const int nRandomOptionOriginId = kQuery.GetInt( "nRandomOptionOriginId" );
		if( nRandomOptionOriginId > 0 )
		{
			o_kItemElem.SetRandomOptionOriginID( static_cast<WORD>( nRandomOptionOriginId ) );
		}
	}

	if( kQuery.IsValidColumn( "nRandomOptionExtFlag" ) == true )
	{
		const WORD wRandomOptionExtFlag = static_cast<WORD>( kQuery.GetInt( "nRandomOptionExtFlag" ) );
		o_kItemElem.SetRandomOptionExtensionFlag( wRandomOptionExtFlag );
	}

	if( kQuery.IsValidColumn( "nRandomOptionExtSize" ) == true )
	{
		const int nRandomOptionExtSize = kQuery.GetInt( "nRandomOptionExtSize" );
		UNREFERENCED_PARAMETER( nRandomOptionExtSize );
	}
	
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		TCHAR column_DST[64] = { 0 };
		FLSPrintf( column_DST, _countof( column_DST ), "%s%d", "nRandomOptionExtDst", Nth + 1 );
		TCHAR column_ADJ[64] = { 0 };
		FLSPrintf( column_ADJ, _countof( column_ADJ ), "%s%d", "nRandomOptionExtAdj", Nth + 1 );
		if( kQuery.IsValidColumn( column_DST ) && kQuery.IsValidColumn( column_ADJ ) )
		{
			o_kItemElem.SetRandomOptionExtension( DB_RANDOMOPTION_DST( kQuery.GetInt( column_DST ) ), DB_RANDOMOPTION_ADJ( kQuery.GetInt( column_ADJ ) ) );
		}
	}

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		if( kQuery.IsValidColumn( "idGuild" ) == true )
		{
			const int nGuildID = kQuery.GetInt( "idGuild" );
			if( nGuildID > 0 )
			{
				o_kItemElem.m_idGuild	= static_cast<DWORD>( nGuildID );
			}
		}

		if( kQuery.IsValidColumn( "nResistSMItemId" ) == true )
		{
			const int nResistSMItemID	= kQuery.GetInt( "nResistSMItemId" );
			if( nResistSMItemID > 0 )
			{
				o_kItemElem.m_nResistSMItemId	= nResistSMItemID;
			}
		}

		if( kQuery.IsValidColumn( "dwLooksChangeItemID" ) == true )
		{
			const DWORD dwLooksChangeItemID	= static_cast<DWORD>( kQuery.GetInt( "dwLooksChangeItemID" ) );
			o_kItemElem.SetLooksChangeItemID( dwLooksChangeItemID );
		}

		if( kQuery.IsValidColumn( "bIsCombined" ) == true )
		{
			const bool bIsCombined = static_cast<bool>( kQuery.GetInt( "bIsCombined" ) == 1 );
			o_kItemElem.SetCombinedOption( bIsCombined );
		}

		if( kQuery.IsValidColumn( "dwCombinedAddDamage" ) == true )
		{
			const DWORD dwCombinedAddDamage	= static_cast<DWORD>( kQuery.GetInt( "dwCombinedAddDamage" ) );
			o_kItemElem.SetCombinedAddDamage( dwCombinedAddDamage );
		}

		if( kQuery.IsValidColumn( "wCombinedRandomOptionOriginID" ) == true )
		{
			const WORD wCombinedRandomOptionOriginID	= static_cast<WORD>( kQuery.GetInt( "wCombinedRandomOptionOriginID" ) );
			o_kItemElem.SetCombinedRandomOptionOriginID( wCombinedRandomOptionOriginID );
		}

		if( kQuery.IsValidColumn( "nCombinedGeneralPiercingSize" ) == true )
		{
			const size_t nCombinedGeneralPiercingSize	= static_cast<size_t>( kQuery.GetInt( "nCombinedGeneralPiercingSize" ) );
			o_kItemElem.SetCombinedGeneralPiercingSize( nCombinedGeneralPiercingSize );
		}

		for( size_t Nth = 0; Nth < o_kItemElem.GetCombinedGeneralPiercingSize(); ++Nth )
		{
			TCHAR szColumn[64] = { 0 };
			FLSPrintf( szColumn, _countof( szColumn ), "%s%d", "dwCombinedGeneralPiercingID", ( Nth + 1 ) );
			if( kQuery.IsValidColumn( szColumn ) == true )
			{
				const DWORD dwItemID = static_cast<DWORD>( kQuery.GetInt( szColumn ) );
				o_kItemElem.SetCombinedGeneralPiercingItemID( Nth, dwItemID );
			}
		}

		if( kQuery.IsValidColumn( "nCombinedRandomOptionExtensionSize" ) == true )
		{
			const int nCombinedRandomOptionExtensionSize = kQuery.GetInt( "nCombinedRandomOptionExtensionSize" );
			UNREFERENCED_PARAMETER( nCombinedRandomOptionExtensionSize );
		}

		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			TCHAR column_DST[64] = { 0 };
			FLSPrintf( column_DST, _countof( column_DST ), "%s%d", "nCombinedRandomOptionExtDst", Nth + 1 );
			TCHAR column_ADJ[64] = { 0 };
			FLSPrintf( column_ADJ, _countof( column_ADJ ), "%s%d", "nCombinedRandomOptionExtAdj", Nth + 1 );
			if( kQuery.IsValidColumn( column_DST ) && kQuery.IsValidColumn( column_ADJ ) )
			{
				o_kItemElem.SetCombinedRandomOptionExtension( DB_RANDOMOPTION_DST( kQuery.GetInt( column_DST ) ), DB_RANDOMOPTION_ADJ( kQuery.GetInt( column_ADJ ) ) );
			}
		}
	}

	if( o_kItemElem.m_dwItemId == ITEM_INDEX( 26476, II_SYS_SYS_SCR_SEALCHARACTER ) )
	{
		CPlayerDataCenter::GetInstance()->m_Access.Enter();

		const char*	lpszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( o_kItemElem.m_nHitPoint );
		if( lpszPlayer != NULL)
		{
			FLStrcpy( o_kItemElem.m_szItemText, _countof( o_kItemElem.m_szItemText ), lpszPlayer );
		}

		CPlayerDataCenter::GetInstance()->m_Access.Leave();
	}

	if( n64RandomOptionId != 0 && o_kItemElem.IsSetRandomOptionExtension() == false )
	{
		o_kItemElem.RandomOptionDataConverting( n64RandomOptionId );
	}
}

bool	CDbManager::ModifyItemElem( const CMover & kMover, FLItemElem & kItemElem )
{
	bool bModify	= false;

	//////////////////////////////////////////////////////////////////////////
	// 기간제 아이템 체크
	if( kItemElem.m_dwKeepTime == 0 )
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( kItemElem.m_dwItemId );
		if( pItemProp != NULL )
		{
			if( ( pItemProp->dwFlag & IP_FLAG_PERIOD ) || ( kItemElem.m_byFlag & FLItemElem::period ) )
			{
				CTime tm	= CTime::GetCurrentTime() + CTimeSpan( 0, 0, pItemProp->dwSkillTime, 0 );
				kItemElem.m_dwKeepTime	= static_cast<DWORD>( ( tm.GetTime() ) );

				bModify		= true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 외형변경 체크
	const DWORD dwLooksChangeMaterialItemID	= kItemElem.GetLooksChangeItemID();
	if( dwLooksChangeMaterialItemID > 0 )
	{
		const bool bMatching = kItemElem.IsMathingLooksChangeMaterialItem( dwLooksChangeMaterialItemID );
		if( bMatching == false )
		{
			// TODO : FLItemElem log
			TCHAR szLog[MAX_LOG_SIZE]	= { 0, };

			kItemElem.LogItemElem( szLog, _countof( szLog ) );

			FLINFO_LOG( _T( "MODIFY_ITEM" ), _T( "[ INITIALIZE(LooksChangeItemID). PLAYER(%07d), %s ]" )
				, kMover.m_idPlayer, szLog );
			
			kItemElem.InitializeLooksChangeItemID();

			bModify		= true;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	return bModify;
}

void	CDbManager::SaveOneItemCommon( CQuery & kQuery, T_ITEM_QUERY_INFO & kItem )
{
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwItemID );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwObjIndex );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nItemNum );
//	kQuery.BindParameterInt64( SQL_PARAM_INPUT, &kItem.dwSerialNumber );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwSerialNumber );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nAbilityOption );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nItemResist );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nResistAbilityOption );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nFlag );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.bCharged );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwKeepTime );
	kQuery.BindParameterInt64( SQL_PARAM_INPUT, &kItem.n64RandomOptID );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nHitPoint );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nRepairNumber );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nRepair );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.bIsTransformVisPet );


	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nUltimatePiercingSize );
	// 16
	for( size_t Nth = 0; Nth < MAX_PIERCING_ULTIMATE; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.arrUltimatePiercingItemID[ Nth ] );
	}
	// 21

	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nGeneralPiercingSize );
	for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.arrGeneralPiercingItemID[ Nth ] );
	}
	//32

	for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.arrVisPetPiercingItemKeepTime[ Nth ] );
	}
	// 42

	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.bIsPet );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nPetKind );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nPetLevel );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwPetExp );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwPetEnergy );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwPetLife );
	// 48

	for( BYTE byLevel = PL_D; byLevel < PL_MAX; ++byLevel )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.arrPetAvailLevel[ byLevel ] );
	}
	// 53

	kQuery.BindParameterVarChar( SQL_PARAM_INPUT, ( _countof( kItem.szPetName ) - 1 ), kItem.szPetName );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nSerialNumberStatus );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwManagedSerialNumber );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwCoupleID);
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nLevelDown );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwRandomOptionOriginID );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.nRandomOptionExtensionSize );
	kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.dwRandomOptionExtensionFlag );
	// 61

	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.arrRandomOptionExtensionDstID[ Nth ] );
		kQuery.BindParameterInt( SQL_PARAM_INPUT, &kItem.arrRandomOptionExtensionAdjValue[ Nth ] );
	}
	// 69

	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.nGuildID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.nResistSMItemID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.dwLooksChangeItemID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.bIsCombined );
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.dwCombinedAddDamage );
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.dwCombinedRandomOptionOriginID );
		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.nCombinedGeneralPiercingSize );
		for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
		{
			kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.arrCombinedGeneralPiercingItemID[ Nth ] );
		}

		kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.nCombinedRandomOptionExtensionSize );
		for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
		{
			kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.arrCombinedRandomOptionExtensionDstID[ Nth ] );
			kQuery.BindParameterInt( SQL_PARAM_INPUT,	&kItem.arrCombinedRandomOptionExtensionAdjValue[ Nth ] );
		}
	}
}

BOOL CDbManager::Exec_ItemLogQuery( CQuery & kLogQuery, const LogItemInfo & info )
{
// 	char szQuery[ MAX_QUERY_SIZE ] = { 0 };
// 	DBQryNewItemLog( szQuery, _countof( szQuery ), info );
// 	if( kLogQuery.Exec( szQuery ) == FALSE )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - ITEM LOG(%s) ]" ), szQuery );
// 		return FALSE;
// 	}
// 	return TRUE;

	return DBQryNewItemLog( kLogQuery, info );
}

bool CDbManager::GetMover_FromAccountCacheMgrA( CMover & io_mover, const BOOL brefresh, const char * szAccount, const BYTE byProgramDataSlot, const DWORD dwPlayTime )
{
	m_AddRemoveLock.Enter();
	BOOL bCacheHit	= FALSE;
	BOOL bExists	= FALSE;
	ACCOUNT_CACHE* pCache	= m_AccountCacheMgr.GetAccount( szAccount, &bCacheHit ); 
	if( bCacheHit  )
	{
		if( CDbManager::CanJoin( szAccount, pCache ) == FALSE )
		{
			m_AddRemoveLock.Leave();
			return true;
		}

		if( pCache->pMover[byProgramDataSlot] == NULL )
		{
			pCache->pMover[byProgramDataSlot] = new CMover;
		}
		else
		{
			if( pCache->pMover[byProgramDataSlot]->m_idPlayer == io_mover.m_idPlayer )
			{
				bExists		= TRUE;		// 캐시에 남아있는 캐릭터면 캐시 정보를 사용한다
			}
			else
			{
				SAFE_DELETE( pCache->pMover[byProgramDataSlot] );

				pCache->pMover[byProgramDataSlot] = new CMover;
			}
		}
	}
	else
	{
		pCache->pMover[byProgramDataSlot] = new CMover;
	}

	CMover* pMover	= pCache->pMover[byProgramDataSlot];

	BOOL bRefresh		= brefresh;
	if( memcmp( pCache->m_idPlayerBank, io_mover.m_idPlayerBank, sizeof(u_long) * MAX_CHARACTER_SLOT ) )
	{
		bRefresh = TRUE;
	}
	if( dwPlayTime == 0 || pMover->m_idPlayer != io_mover.m_idPlayer )	
	{
		bRefresh = TRUE;
	}

	// 처음 접속하는 계정이거나 새로고침이 설정되어 있는
	// 캐릭터의 경우 어카운트 캐시에 은행을 복사한다
	if( bCacheHit == FALSE || bRefresh == TRUE )
	{
		for( int j = 0 ; j < MAX_CHARACTER_SLOT ; j++ )
		{
			pCache->m_Bank[j].Copy( io_mover.m_Bank[j] );
		}

		memcpy( pCache->m_idPlayerBank, io_mover.m_idPlayerBank, sizeof(u_long) * MAX_CHARACTER_SLOT );
		memcpy( pCache->m_dwGoldBank, io_mover.m_dwGoldBank, sizeof(DWORD) * MAX_CHARACTER_SLOT );
	}

	pMover->Lock();
	// 캐릭터가 캐시에 없거나 새로고침이 설정되어 있으면
	// 모든 정보를 복사한다
	pMover->Copy( &io_mover, !bExists || bRefresh );
	// 캐릭터에 은행을 복사한다
	for( int j = 0; j < MAX_CHARACTER_SLOT; j++ )
	{
		pMover->m_Bank[j].Copy( pCache->m_Bank[j] );
	}

	memcpy( pMover->m_idPlayerBank, pCache->m_idPlayerBank, sizeof(u_long) * MAX_CHARACTER_SLOT );
	memcpy( pMover->m_dwGoldBank, pCache->m_dwGoldBank, sizeof(DWORD) * MAX_CHARACTER_SLOT );

	pMover->PeriodTick();
	// 임시 변수에 모든 정보를 복사한다.
	// 이것은 락이 없는 상태에서 쿼리를 실행하기 위해서이다
	io_mover.Copy( pMover, TRUE );
	pMover->Unlock();

	m_AccountCacheMgr.AddAccount( pMover->m_idPlayer, szAccount, bCacheHit, pCache );
	m_AddRemoveLock.Leave();

	return false;
}

void CDbManager::GetMover_FromAccountCacheMgrB( CMover & o_mover, const char * szAccount, const BYTE byProgramDataSlot )
{
	m_AddRemoveLock.Enter();
	ACCOUNT_CACHE* pCache	= m_AccountCacheMgr.Find( szAccount );
	if( pCache )
	{
		CMover* pMover	= pCache->pMover[byProgramDataSlot];
		if( pMover )
		{
			pMover->Lock();
			pMover->Copy( &o_mover, TRUE );
			pMover->Unlock();
			FLTRACE_LOG( PROGRAM_NAME, _T( "bResult: 1" ) );
		}
	}
	m_AddRemoveLock.Leave();
}

//////////////////////////////////////////////////////////////////////////
void	CDbManager::GuildBankInputItem( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	FLPacketGuildBankInputItemReq	fromWorld;

	fromWorld.Deserialize( ar );

	const bool bRet = SaveGuildBank( fromWorld.dwGuildID, &fromWorld.GuildBank, pQuery );

	FLPacketGuildBankInputItemAck	toWorld;
	toWorld.bSuccess			= bRet;
	toWorld.dwGuildID			= fromWorld.dwGuildID;
	toWorld.dwPlayerID			= fromWorld.dwPlayerID;
	toWorld.kInputItemElem		= fromWorld.kInputItemElem;

	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &toWorld );

	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild					= fromWorld.dwGuildID;
	qi.idPlayer					= fromWorld.dwPlayerID;
	qi.pszState					= "A";		// 아이템 넣기
	qi.nItem					= fromWorld.kInputItemElem.m_dwItemId;
	qi.nAbilityOption			= fromWorld.kInputItemElem.GetAbilityOption();
	qi.iUniqueNo				= fromWorld.kInputItemElem.GetSerialNumber();
	qi.nItem_count				= fromWorld.kInputItemElem.m_nItemNum;

	TCHAR szGuildItemLog[ 6144 ] = { 0, };
	MakeLogGuildItem( &fromWorld.GuildBank, szGuildItemLog, _countof( szGuildItemLog ) );

	qi.pszGuildBank				= szGuildItemLog;

	TCHAR szQuery[ MAX_QUERY_SIZE ]	= { 0, };
	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	if( pQueryLog->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED EXECUTE QUERY. szQuery(%s) ]" ), szQuery );
	}
}

void	CDbManager::GuildBankOutputItem( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	FLPacketGuildBankOutputItemReq	fromWorld;

	fromWorld.Deserialize( ar );

	const bool bRet = SaveGuildBank( fromWorld.dwGuildID, &fromWorld.GuildBank, pQuery );

	FLPacketGuildBankOutputItemAck	toWorld;
	toWorld.bSuccess			= bRet;
	toWorld.dwGuildID			= fromWorld.dwGuildID;
	toWorld.dwPlayerID			= fromWorld.dwPlayerID;
	toWorld.kOutputItemElem		= fromWorld.kOutputItemElem;

	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &toWorld );

	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild					= fromWorld.dwGuildID;
	qi.idPlayer					= fromWorld.dwPlayerID;
	qi.pszState					= "D";		// 아이템 빼기
	qi.nItem					= fromWorld.kOutputItemElem.m_dwItemId;
	qi.nAbilityOption			= fromWorld.kOutputItemElem.GetAbilityOption();
	qi.iUniqueNo				= fromWorld.kOutputItemElem.GetSerialNumber();
	qi.nItem_count				= fromWorld.kOutputItemElem.m_nItemNum;

	TCHAR szGuildItemLog[ 6144 ] = { 0, };
	MakeLogGuildItem( &fromWorld.GuildBank, szGuildItemLog, _countof( szGuildItemLog ) );

	qi.pszGuildBank				= szGuildItemLog;

	TCHAR szQuery[ MAX_QUERY_SIZE ]	= { 0, };
	DBQryGuildLog( szQuery, _countof( szQuery ), qi );
	if( pQueryLog->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED EXECUTE QUERY. szQuery(%s) ]" ), szQuery );
	}
}

// void	CDbManager::GuildBankInputGold( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
// {
// 	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );
// 
// 	FLPacketGuildBankInputGoldReq	fromWorld;
// 
// 	fromWorld.Deserialize( ar );
// 
// 	/************************************************************************/
// 	/* 
// 	길드 골드가 0 보다 크고 플레이어 아이디가 0 이 아닐 경우
// 	GUILD_MEMBER_TBL 에서 공헌 페냐를 갱신 시킨다.
// 	그러므로 GUILD_BANK_STR 호출 시 플레이어 아이디는 0 일 수도 있다.
// 	*/
// 	/************************************************************************/
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 길드 골드 업데이트. 
// 	const DWORD dwPlayerID_Contribution	= fromWorld.bUpdateContribution == true ? fromWorld.dwPlayerID : NULL_PLAYER_ID;
// 	TCHAR szQuery[ MAX_QUERY_SIZE ]		= { 0, };
// 	FLSPrintf( szQuery, _countof( szQuery ), "GUILD_BANK_STR 'U1','%06d','%02d','%d','%s','%s','%s','%07d','%s','%s','%s'",
// 		fromWorld.dwGuildID, g_appInfo.dwSys, fromWorld.dwGuildGold, "$", "$", "$", dwPlayerID_Contribution, "$", "$", "$" );
// 
// 	FLPacketGuildBankInputGoldAck toWorld;
// 	toWorld.dwPlayerID		= fromWorld.dwPlayerID;
// 	toWorld.dwInputGold		= fromWorld.dwInputGold;
// 
// 	if( FALSE == pQuery->Exec( szQuery ) )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "update guild bank:%s " ), szQuery );
// 		toWorld.bSuccess	= false;
// 		CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &toWorld );
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// }

void	CDbManager::GuildBankOutputGold( CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus )
{
	CAr ar( lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize );

	FLPacketGuildBankOutputGoldReq	fromWorld;

	fromWorld.Deserialize( ar );

	/************************************************************************/
	/* 
	길드 골드가 0 보다 크고 플레이어 아이디가 0 이 아닐 경우
	GUILD_MEMBER_TBL 에서 공헌 페냐를 갱신 시킨다.
	그러므로 GUILD_BANK_STR 호출 시 플레이어 아이디는 0 일 수도 있다.
	*/
	/************************************************************************/

	const DWORD dwPlayerID_Contribution	= fromWorld.bUpdateContribution == true ? fromWorld.dwPlayerID : NULL_PLAYER_ID;
	TCHAR szQuery[ MAX_QUERY_SIZE ]		= { 0, };
	FLSPrintf( szQuery, _countof( szQuery ), "GUILD_BANK_STR 'U1','%06d','%02d','%d','%s','%s','%s','%07d','%s','%s','%s'",
		fromWorld.dwGuildID, g_appInfo.dwSys, fromWorld.dwGuildGold, "$", "$", "$", dwPlayerID_Contribution, "$", "$", "$" );

	const bool bRet = ( pQuery->Exec( szQuery ) == TRUE ) ? true : false;
	if( bRet == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED EXECUTE QUERY. szQuery(%s) ]" ), szQuery );
	}

	FLPacketGuildBankOutputGoldAck	toWorld;
	toWorld.bSuccess		= bRet;
	toWorld.dwGuildID		= fromWorld.dwGuildID;
	toWorld.dwPlayerID		= fromWorld.dwPlayerID;
	toWorld.dwOutputGold	= fromWorld.dwOutputGold;

	CDPTrans::GetInstance()->SendPacket( lpDbOverlappedPlus->dpid, &toWorld );

	GUILDLOG_QUERYINFO qi( "L2" );
	qi.idGuild				= fromWorld.dwGuildID;
	qi.idPlayer				= fromWorld.dwPlayerID;
	qi.nItem				= fromWorld.dwOutputGold;
	qi.pszState				= "O";		// 돈 빼기

	TCHAR szLogQuery[ MAX_QUERY_SIZE ]	= { 0, };
	DBQryGuildLog( szLogQuery, _countof( szLogQuery ), qi );
	if( pQueryLog->Exec( szLogQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED EXECUTE QUERY. szQuery(%s) ]" ), szLogQuery );
	}
}
//////////////////////////////////////////////////////////////////////////


#ifdef PASSWORD_RESET_2ND
int	CDbManager::CheckResetPassword( CQuery* pQuery, DWORD dwPlayerId )
{
	TCHAR szQuery[64] = {0, };

	FLSPrintf( szQuery, _countof( szQuery ),"usp_BankPW_Check '%02d', '%07d', '%04d'", g_appInfo.dwSys, dwPlayerId, 0000 );

	if ( pQuery->Exec( szQuery ) == FALSE )
	{
		return false;
	}

	int nError = 1;
	if( pQuery->Fetch() )
		nError = pQuery->GetInt( "f_check" );

	return nError;
}
#endif // PASSWORD_RESET_2ND


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void		PostOverlappedPlus( const int nQM, const HANDLE hIOCP, const DWORD dwNumberOfBytesTransferred, const ULONG_PTR dwCompletionKey
							   , const DPID dpid, const DPID dpidCache, const DPID dpidUser, const LPBYTE lpBuf, const u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->dpid		= dpid;
	lpDbOverlappedPlus->dpidCache	= dpidCache;
	lpDbOverlappedPlus->dpidUser	= dpidUser;
	lpDbOverlappedPlus->nQueryMode	= nQM;
	PostQueuedCompletionStatus( hIOCP, dwNumberOfBytesTransferred, dwCompletionKey, &lpDbOverlappedPlus->Overlapped );
}
