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
#include "../_CommonDefine/Packet/FLPacket_Certify.h"
extern	CDPAccountClient	g_dpAccountClient;

extern const int MAX_TASKBAR; 
extern const int MAX_APPLETTASKBAR; 
extern const int MAX_ITEMTASKBAR;   
extern const char NullStr[2];

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;


void CDbManager::SendJoin( CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey, 
						  DWORD /*dwBillingClass*/, LPCTSTR szPartyName, LPCTSTR szBankPass, 
						  DWORD dwPlayTime, DWORD /*dwFlyTime*/, int nMaximumLevel, int nTags, TAG_ENTRY* tags,
						  LPCTSTR szTimeGuild, const CTime & tmBirthDate )
{
	PLAY_ACCOUNT playAccount = { 0 };
	FLStrcpy( playAccount.lpszAccount, _countof( playAccount.lpszAccount ), lpDBOP->AccountInfo.szAccount );
	GetLocalTime( &playAccount.m_stLogin );

	BEFORESENDDUAL( ar, PACKETTYPE_JOIN, lpDBOP->dpidCache, lpDBOP->dpidUser );
	ar << E_LOGIN_SUCCESS;
	ar << dwAuthKey;
	ar << tmBirthDate;	//

	ar << playAccount;
	ar << pMover->m_dwWorldID;
#ifdef __LAYER_1015
	ar << pMover->GetLayer();
#endif	// __LAYER_1015
	ar << pMover->m_dwIndex;
	pMover->Serialize( ar, METHOD_NONE );			
	pMover->m_UserTaskBar.Serialize( ar );
	pMover->m_RTMessenger.Serialize( ar );
	ar.WriteString( szPartyName );
	ar.WriteString( szBankPass );
	ar << dwPlayTime;
	ar << nMaximumLevel;
	WriteTag(ar, nTags, tags);	// 쪽지 쓰기 
	ar.WriteString( szTimeGuild );
	ar << pMover->m_dwReturnWorldID;
	ar << pMover->m_vReturnPos;

	ar << pMover->m_nEventFlag;
	ar << pMover->m_dwEventTime;
	ar << pMover->m_dwEventElapsed;

	ar << pMover->m_nExpLog;
	ar << pMover->m_nAngelExpLog;

	ar << pMover->m_nCoupon;

	ar << pMover->m_dwPCBangClass;
//	ar << pMover->m_nRestPoint << static_cast<time_t>( pMover->m_tLogOut ? time_null() - pMover->m_tLogOut : 0 );

	//////////////////////////////////////////////////////////////////////////
	ar << pMover->m_nRestPoint;
	const time_t tCurrTime		= time_null();
	const time_t tLogOutTime	= pMover->m_tLogOut > 0 && tCurrTime > pMover->m_tLogOut ? tCurrTime - pMover->m_tLogOut : 0;
	ar << tLogOutTime;
	//////////////////////////////////////////////////////////////////////////

	SEND( ar, CDPTrans::GetInstance(), lpDBOP->dpid );
	
	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// JOIN_1\t// %d" ), lpDBOP->dpid );
}

/*
캐릭터 복구시 - 뱅크가 사라지는 문제 

캐릭터 생성
해당 캐릭의 뱅크슬롯에 아이템 넣음
캐릭터 삭제
캐릭터 복구
로그인 
S8
result set의 뱅크 정보를 읽지 않음 ( 캐쉬 hit 이기 때문에 )
캐릭터를 새로 읽어도 뱅크는 새로 읽지 않음( 캐쉬에 있기 때문에 )
*/

void CDbManager::Join( CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDBOP )
{
	BYTE	_nSlot;		// 0 ~ 2
	u_long	idPlayer;
	DWORD	dwAuthKey, dwBillingClass		= 0;
	char	szBankPass[5]	= { 0, };
	char	szPartyName[36]	= { 0, };
	char	szTimeGuild[15]	= { 0, };
	TAG_ENTRY tags[MAX_TAGS];

	CAr arRead( lpDBOP->lpBuf, lpDBOP->uBufSize );
	arRead >> dwAuthKey;	// o
	arRead.ReadString( lpDBOP->AccountInfo.szAccount, _countof( lpDBOP->AccountInfo.szAccount ) );
	arRead.ReadString( lpDBOP->AccountInfo.szPassword, _countof( lpDBOP->AccountInfo.szPassword ) );
	arRead >> _nSlot >> idPlayer;

	arRead >> lpDBOP->dpid >> lpDBOP->dpidCache >> lpDBOP->dpidUser;

	DWORD dwPCBangClass;
	arRead >> dwPCBangClass;

	FLTRACE_LOG( PROGRAM_NAME, _T( "DATABASESERVER.EXE\t// JOIN_0\t// %d" ), lpDBOP->dpid );

	//////////////////////////////////////////////////////////////////////////
	// BEGIN110119

	CMover Mover;
	Mover.m_idPlayer	= idPlayer;
	FLStrcpy( Mover.m_szAccount, _countof( Mover.m_szAccount ), lpDBOP->AccountInfo.szAccount );

	T_CHARACTER_STR_INFO tCharacter( Mover, "S8" );
// 	FLStrcpy( tCharacter.szGu, _countof( tCharacter.szGu ), "S8" );
// 	tCharacter.dwPlayerId = idPlayer;
// 	FLStrcpy( tCharacter.szAccount, _countof( tCharacter.szAccount ), lpDBOP->AccountInfo.szAccount );

	if( DBQryCharacter( qry, &tCharacter ) == false )
	{
		return;
	}

	// END110119
	//////////////////////////////////////////////////////////////////////////

	if( !qry->Fetch() )
	{
		return;
	}
	qry->GetStr( "m_szName", lpDBOP->AccountInfo.szPlayer, _countof( lpDBOP->AccountInfo.szPlayer ) );
	m_joinLock.Enter();
	FLStrcpy( m_aszJoin[m_nJoin], _countof( m_aszJoin[m_nJoin] ), lpDBOP->AccountInfo.szPlayer );
	m_nJoin	= ( m_nJoin + 1 ) % MAX_JOIN_SIZE;
	m_joinLock.Leave();

	// BEGIN110520
//	BYTE nSlot = (BYTE)qry->GetInt( "playerslot" );
	const BYTE byDBDataSlot			= static_cast<BYTE>( qry->GetInt( "playerslot" ) );
	const BYTE byProgramDataSlot	= GET_PLAYER_SLOT( byDBDataSlot );
	// END110520

	// 임시 변수에 쿼리 결과를 저장한다
	CMover	mover;

	// BEGIN110520
//	mover.m_nSlot	= nSlot;
	mover.m_nDBDataSlot				= byDBDataSlot;		// 캐릭터 고유의 슬롯넘버를 저장
	// END110520

	mover.m_dwPCBangClass			= dwPCBangClass;

	mover.m_dwIndex					= static_cast<DWORD>( qry->GetInt( "m_dwIndex" ) );
	mover.InitProp();
	mover.m_idPlayer				= static_cast<u_long>( qry->GetInt( "m_idPlayer" ) );

	BOOL bRefresh					= FALSE;
	if( qry->GetInt( "m_idCompany" ) > 0 )
	{
		bRefresh					= TRUE;
	}

	/*GetInventory( &mover, qry, lpDBOP );
	GetEquipment( &mover, qry, lpDBOP );
	GetTaskBar( &mover, qry, lpDBOP );
	GetCardCube( &mover, qry, lpDBOP );
	GetBaseCharacter( &mover, qry, lpDBOP );
	GetJobLv( &mover, qry, lpDBOP );
	GetQuest( &mover, qry, lpDBOP );
	GetSMMode( &mover, qry, lpDBOP );
	GetSKillInfluence( &mover, qry, lpDBOP );*/
	// mirchang_100416 VERIFYSTRING use return value
	GetBaseCharacter( &mover, qry, lpDBOP );
	if( GetInventory( &mover, qry, lpDBOP ) == FALSE
		|| GetEquipment( &mover, qry, lpDBOP ) == FALSE
		|| GetTaskBar( &mover, qry, lpDBOP ) == FALSE
		|| GetQuest( &mover, qry, lpDBOP ) == FALSE
		|| GetSMMode( &mover, qry, lpDBOP ) == FALSE
		|| GetSKillInfluence( &mover, qry, lpDBOP ) == FALSE )
	{
		return;
	}
	// mirchang_100416

	mover.m_RTMessenger.SetState( static_cast<DWORD>( qry->GetInt( "m_nMessengerState" ) ) );

	mover.m_dwReturnWorldID		= qry->GetInt( "m_dwReturnWorldID" );
	mover.m_vReturnPos.x		= qry->GetFloat( "m_vReturnPos_x" );
	mover.m_vReturnPos.y		= qry->GetFloat( "m_vReturnPos_y" );
	mover.m_vReturnPos.z		= qry->GetFloat( "m_vReturnPos_z" );
	
	DWORD dwPlayTime			= qry->GetInt( "TotalPlayTime" );
	mover.m_dwMute				= qry->GetInt( "m_dwFlyTime" );
	DWORD dwFlyTime				= 0;	//
	int nMaximumLevel			= qry->GetInt( "m_nMaximumLevel" );
	qry->GetStr( "m_tGuildMember", szTimeGuild, _countof( szTimeGuild ) );
	
	char cAuth					= qry->GetChar( "m_chLoginAuthority" );
	if( cAuth == 'F' )
	{
		cAuth					= qry->GetChar( "m_chAuthority" );
	}
	mover.m_dwAuthorization		= static_cast<DWORD>( cAuth );
	
	mover.m_dwMode				&= ~( EVENT_OLDBOY_MODE );
	if( qry->GetInt( "last_connect" ) == 0 )
	{
		mover.m_dwMode			|= EVENT_OLDBOY_MODE;
	}
	mover.m_idGuild				= static_cast<u_long>( qry->GetInt( "m_idGuild" ) );
	mover.m_idWar				= static_cast<u_long>( qry->GetInt( "m_idWar" ) );

	mover.m_nEventFlag			= qry->GetInt64( "dwEventFlag" );
	mover.m_dwEventTime			= static_cast<DWORD>( qry->GetInt( "dwEventTime" ) );
	mover.m_dwEventElapsed		= static_cast<DWORD>( qry->GetInt( "dwEventElapsed" ) );

	DWORD dwPetId				= static_cast<DWORD>( qry->GetInt( "m_dwPetId" ) );
	mover.SetPetId( dwPetId );

	mover.m_nExpLog				= qry->GetInt( "m_nExpLog" );
	mover.m_nAngelExpLog		= qry->GetInt( "m_nAngelExpLog" );

	mover.m_nCoupon				= qry->GetInt( "m_nCoupon" );

#ifdef __LAYER_1015
	mover.SetLayer( qry->GetInt( "m_nLayer" ) );
#endif	// __LAYER_1015

	mover.SetCampusPoint( qry->GetInt( "m_nCampusPoint" ) );
	mover.SetCampusId( qry->GetInt( "idCampus" ) );

	mover.m_nRestPoint			= qry->GetInt( "m_nRestPoint" );
	mover.m_tLogOut				= time_null() - qry->GetInt( "m_LogOutTime" );

	if( !qry->MoreResults() )
	{
		return;
	}

	u_long arrPlayerId[ MAX_CHARACTER_SLOT ] = { 0, };

	// BEGIN110520
	int arrDBDataSlot[ MAX_CHARACTER_SLOT ] = { -1, };
	const BYTE bySelectPage = GET_PLAYER_PAGE( byDBDataSlot );
	// END110520

	while( qry->Fetch() )
	{
		const int nDBDataSlot		= qry->GetInt( "playerslot" );

		// BEGIN110520
		if( bySelectPage != GET_PLAYER_PAGE( nDBDataSlot ) )
		{
			continue;
		}

		const int nProgramDataSlot	= GET_PLAYER_SLOT( nDBDataSlot );
		// END110520

		u_long uPlayerid			= static_cast<u_long>( qry->GetInt( "m_idPlayer" ) );

		if( nProgramDataSlot < 0 || nProgramDataSlot >= MAX_CHARACTER_SLOT )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "Player Solt is Invalid. Account:[%s], DBDataSlot:[%d], ProgramDataSlot:[%d]" )
				, lpDBOP->AccountInfo.szAccount, nDBDataSlot, nProgramDataSlot );
			return;
		}
		if( byDBDataSlot == nDBDataSlot )
		{
			qry->GetStr( "m_BankPw", szBankPass, _countof( szBankPass ) );
		}
		//GetBank( &mover, qry, lpDBOP, nPlayerSlot );
		// mirchang_100416 VERIFYSTRING use return value
		if( GetBank( &mover, qry, lpDBOP, nProgramDataSlot ) == FALSE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. GetBank failed. PlayerId:[%07d]" ), mover.m_idPlayer );
			return;
		}
		// mirchang_100416

		arrPlayerId[ nProgramDataSlot ]		= uPlayerid;

		// BEGIN110520
		arrDBDataSlot[ nProgramDataSlot ]	= nDBDataSlot;
		// END110520
	}

	if( qry->MoreResults() )
	{
		while( qry->Fetch() )
		{
			//GetPocket( &mover, qry, lpDBOP );
			// mirchang_100416 VERIFYSTRING use return value
			if( GetPocket( &mover, qry, lpDBOP ) == FALSE )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. GetPocket failed. PlayerId:[%07d]" ), mover.m_idPlayer );
				return;
			}
			// mirchang_100416
		}
	}

	GetMyPartyName( qry, lpDBOP, mover.m_idPlayer, szPartyName, _countof( szPartyName ) );
	int nTags = SelectTag( qry, lpDBOP, mover.m_idPlayer, tags );
	LoadMessenger( &mover, qry );

	GetHonor( &mover, qry, lpDBOP );

	if( !GetSkill( &mover, qry, lpDBOP ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. GetSkill failed. PlayerId:[%07d]" ), mover.m_idPlayer );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// LoadInventory
	if( LoadInventory( &mover, qry ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. LoadInventory failed. PlayerId:[%07d]" ), mover.m_idPlayer );
		return;
	}

	RefreshTaskBar( &mover );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Load Bank
	for( int nPlayerSlot = 0; nPlayerSlot < MAX_CHARACTER_SLOT; ++nPlayerSlot )
	{
		u_long uPlayerId = arrPlayerId[ nPlayerSlot ];
		if( uPlayerId != NULL_PLAYER_ID )
		{
			if( LoadCharacterBank( &mover, qry, uPlayerId, arrDBDataSlot[ nPlayerSlot ] ) == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. LoadCharacterBank failed. PlayerId:[%07d], DBDataSlot:[%d]" )
					, uPlayerId, arrDBDataSlot[ nPlayerSlot ] );
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Load Pocket
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( LoadCharacterPocket( &mover, qry, nPocket ) == false )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CDbManager::Join. LoadCharacterPocket failed. PlayerId:[%07d], nPocket:[%d]" )
				, mover.m_idPlayer, nPocket );
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	m_AddRemoveLock.Enter();
	BOOL bCacheHit	= FALSE;
	BOOL bExists	= FALSE;
	ACCOUNT_CACHE* pCache	= m_AccountCacheMgr.GetAccount( lpDBOP->AccountInfo.szAccount, &bCacheHit ); 
	if( bCacheHit  )
	{
		if( CanJoin( lpDBOP->AccountInfo.szAccount, pCache ) == FALSE )
		{
			m_AddRemoveLock.Leave();
			return;
		}

		if( pCache->pMover[byProgramDataSlot] == NULL )
		{
			pCache->pMover[byProgramDataSlot] = new CMover;
		}
		else
		{
			if( pCache->pMover[byProgramDataSlot]->m_idPlayer == mover.m_idPlayer )
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

	if( memcmp( pCache->m_idPlayerBank, mover.m_idPlayerBank, sizeof(u_long) * MAX_CHARACTER_SLOT ) )
	{
		bRefresh = TRUE;
	}
	if( dwPlayTime == 0 || pMover->m_idPlayer != mover.m_idPlayer )	
	{
		bRefresh = TRUE;
	}

	// 처음 접속하는 계정이거나 새로고침이 설정되어 있는
	// 캐릭터의 경우 어카운트 캐시에 은행을 복사한다
	if( bCacheHit == FALSE || bRefresh == TRUE )
	{
		for( int j = 0 ; j < MAX_CHARACTER_SLOT ; j++ )
		{
			pCache->m_Bank[j].Copy( mover.m_Bank[j] );
		}

		memcpy( pCache->m_idPlayerBank, mover.m_idPlayerBank, sizeof(u_long) * MAX_CHARACTER_SLOT );
		memcpy( pCache->m_dwGoldBank, mover.m_dwGoldBank, sizeof(DWORD) * MAX_CHARACTER_SLOT );
	}

	pMover->Lock();
	// 캐릭터가 캐시에 없거나 새로고침이 설정되어 있으면
	// 모든 정보를 복사한다
	pMover->Copy( &mover, !bExists || bRefresh );
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
	mover.Copy( pMover, TRUE );
	pMover->Unlock();

	m_AccountCacheMgr.AddAccount( pMover->m_idPlayer, lpDBOP->AccountInfo.szAccount, bCacheHit, pCache );
	m_AddRemoveLock.Leave();

	// 지급, 삭제
	BOOL bResult	= FALSE;
	bResult
		= SendItemtoCharacter( &mover, qry, qry1, qrylog, lpDBOP );
	bResult
		= RemoveItemtoCharacter( &mover, qry, qry1, qrylog, lpDBOP )
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
		m_AddRemoveLock.Enter();
		pCache	= m_AccountCacheMgr.Find( lpDBOP->AccountInfo.szAccount );
		if( pCache )
		{
			pMover	= pCache->pMover[byProgramDataSlot];
			if( pMover )
			{
				pMover->Lock();
				pMover->Copy( &mover, TRUE );
				pMover->Unlock();
				FLTRACE_LOG( PROGRAM_NAME, _T( "bResult: 1" ) );
			}
		}
		m_AddRemoveLock.Leave();
	}
	
	PutExpLog( qrylog, &mover, 'I' );
	CTime tmNullTime;
	SendJoin( &mover, lpDBOP, dwAuthKey, dwBillingClass, szPartyName, szBankPass, dwPlayTime, dwFlyTime, nMaximumLevel, nTags, tags, szTimeGuild, tmNullTime );
	g_dpCoreSrvr.SendMemberTime( mover.m_idPlayer, szTimeGuild );

	//////////////////////////////////////////////////////////////////////////
	// TODO 텔레포트 지도 등록지점 데이터 전송
	SelectTeleportMapPointList( qry, idPlayer, lpDBOP );
	//////////////////////////////////////////////////////////////////////////
}

void	CDbManager::SelectTeleportMapPointList( CQuery* pQuery, DWORD dwPlayerID, LPDB_OVERLAPPED_PLUS pDbOverlappedPlus )
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

	FLSPrintf( szQuery, _countof( szQuery ), "{call usp_Teleport_Load(?,?)}" );

	pQuery->BindParameterChar( SQL_PARAM_INPUT,		2,		 szServerIndex );
	pQuery->BindParameterChar( SQL_PARAM_INPUT,		7,		 szPlayerId );
	

	if( pQuery->Exec( szQuery ) == FALSE )
	{
		return;
	}

	FLPacketTeleportMapPointListNoti kPacket;
	kPacket.dwPlayerID = dwPlayerID;

	while( pQuery->Fetch() )
	{
		T_TELEPORTMAP_POINT tUserPoint = { 0 };

		tUserPoint.dwIndex = static_cast< DWORD >( pQuery->GetInt( "PointIdx" ) );
		tUserPoint.byType = TELEPORT_MAP_TYPE_USER;
		tUserPoint.tPosition.dwWorldID = static_cast< DWORD >( pQuery->GetInt( "dwWorldID" ) );
		tUserPoint.tPosition.tPos.x = pQuery->GetInt( "m_vPos_x" ) / 100.f;
		tUserPoint.tPosition.tPos.y = pQuery->GetInt( "m_vPos_y" ) / 100.f;
		tUserPoint.tPosition.tPos.z = pQuery->GetInt( "m_vPos_z" ) / 100.f;
		pQuery->GetStr( "PointName", tUserPoint.szName, _countof( tUserPoint.szName ) );

		kPacket.vecUserPoints.push_back( tUserPoint );
	}

	if( kPacket.vecUserPoints.empty() == false )
	{
		CDPTrans::GetInstance()->SendPacket( pDbOverlappedPlus->dpid, &kPacket );
	}
}
