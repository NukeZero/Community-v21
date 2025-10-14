
#include "stdafx.h"
#include "lang.h"
#include "Ship.h"
#include "guild.h"
#include "spevent.h"
#include <common/FLStdFile.h>
#include "couple.h"
#include "pet.h"			//sun: 9, 9-10차 펫
#include "collecting.h"		//sun: 11, 채집 시스템
#include "SecretRoom.h"		//sun: 12, 비밀의 방
#include "Tax.h"			//sun: 12, 세금
#include "CreateMonster.h"	//sun: 12, // __NEW_ITEMCREATEMON_SERVER
#include "langman.h"
#include "honor.h"			//sun: 13, 달인

#include "../_Common/SeasonEffect.h"

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#ifdef __AGGRO16
#include "FLAggro.h"
#endif // __AGGRO16

#ifdef __CLIENT
#include "../Neuz/couplehelper.h"
#include "../Neuz/clord.h"
#include "../Neuz/FLExchangeEvent.h"
#endif	// __CLIENT

#ifdef __WORLDSERVER
#include "InstanceDungeonParty.h"
#include "../WorldServer/couplehelper.h"
#include "../WorldServer/ItemUpgrade.h"
#include "../WorldServer/FLMonsterDropItem.h"
#include "../WorldServer/FLLevelDropItem.h"
#include "../WorldServer/FLEventDropItem.h"
#include "../WorldServer/FLDropPenalty_ByLevelDist.h"
#include "../WorldServer/rangda.h"		//sun: 12, // __RANGDA_0521
#include "worldmng.h"
#include "guildquest.h"
#include "MsgHdr.h"
#include "partyquest.h"
#include "../WorldServer/slord.h"
#include "RainbowRace.h"	//sun: 13, 레인보우 레이스
#include "Quiz.h"
#include "../WorldServer/CampusHelper.h"
#include "GuildHouse.h"
#include "../WorldServer/FLCooperativeContributions.h"
#include "../WorldServer/FLConsignmentSale.h"
#include "../worldserver/FLExchangeEvent.h"
#include "../worldserver/FLItemUpgradeManager.h"
#include "../worldserver/FLItemMergeManager.h"
#include "../worldserver/FLItemExtract.h"
#include "../worldserver/FLBarunaUpgradeNpc.h"
#include "../worldserver/FLMadrigalGift.h"
#include "../worldserver/FLFlyffPieceEvent.h"
#include "../worldserver/FLFlyffPieceDailyPayment.h"

#endif // __WORLDSERVER

extern	CWorldMng	g_WorldMng;

FLOAT	CProject::m_fItemDropRate = 1.0f;		// 몬스터 아이템 드롭률
FLOAT	CProject::m_fGoldDropRate = 1.0f;		// 몬스터 페냐 드롭률
FLOAT	CProject::m_fMonsterExpRate = 1.0f;		// 몬스터 경험치룰
FLOAT	CProject::m_fMonsterHitRate = 1.0f;		// 몬스터 공격률
FLOAT	CProject::m_fShopCost = 1.0f;			// 상점가격
FLOAT	CProject::m_fSkillExpRate = 1.0f;		
DWORD	CProject::m_dwVagSP = 1;				// 방랑자 스킬의 레벨업때 필요한 SP포인트
DWORD	CProject::m_dwExpertSP = 2;				// 1차직업 스킬의 레벨업때 필요한 SP포인트
DWORD	CProject::m_dwProSP = 3;				// 2차직업 스킬의 레벨업때 필요한 SP포인트
DWORD	CProject::m_dwLegendSP = 4;				// 3차직업 스킬의 레벨업때 필요한 SP포인트


QuestGoalData::QuestGoalData( void ) : 
m_dwGoalIndex( 0 ),
m_dwGoalWorldID( 0 ),
m_fGoalPositionX( 0.0F ), 
m_fGoalPositionZ( 0.0F ), 
m_nOwnMonster( NULL_ID ),
m_dwGoalTextID( 0 )
{
}

//---------------------------------------------------------------------------------------------
// NPC  
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
// 생성자  
//---------------------------------------------------------------------------------------------
tagCHARACTER::tagCHARACTER()
:	m_strName( _T("") )
,	m_vPos( 0.0f, 0.0f, 0.0f )
,	m_nEquipNum( NULL )
,	m_nStructure( -1 )
,	m_nVenderType( NULL )
,	m_bIgnorePurchaseTax( false )
,	bOutput( TRUE )
,	m_bForcedMenu( FALSE )
,	m_dwWorldId( NULL )
,	m_dwMusicId( NULL )
,	m_dwMoverIdx( NULL )
,	m_dwHairMesh( NULL )
,	m_dwHeadMesh( NULL )
,	m_dwHairColor( NULL )
,	m_dwContributionID( NULL )
{
	ZeroMemory( m_szKey,		sizeof( CHAR ) * 64 );
	ZeroMemory( m_szChar,		sizeof( CHAR ) * 64 );
	ZeroMemory( m_szDialog,		sizeof( CHAR ) * 64 );
	ZeroMemory( m_szDlgQuest,	sizeof( CHAR ) * 64 );

	for( UINT nCount = 0; nCount < 4; nCount++ )
		m_venderSlot[nCount] = _T("");

	ZeroMemory( m_adwEquip,		sizeof( DWORD ) * MAX_HUMAN_PARTS );
	ZeroMemory( m_abMoverMenu,	sizeof( BOOL ) * MAX_MOVER_MENU );
}


//---------------------------------------------------------------------------------------------
// 초기화  
//---------------------------------------------------------------------------------------------
void CHARACTER::Clear()
{
	m_strName = _T("");

	ZeroMemory( m_szKey,		sizeof( CHAR ) * 64 );
	ZeroMemory( m_szChar,		sizeof( CHAR ) * 64 );
	ZeroMemory( m_szDialog,		sizeof( CHAR ) * 64 );
	ZeroMemory( m_szDlgQuest,	sizeof( CHAR ) * 64 );

	for( UINT nCount = 0; nCount < 4; nCount++ )
		m_venderSlot[nCount] = _T("");

	m_vPos				= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_nEquipNum			= NULL;
	m_nStructure		= -1;
	m_nVenderType		= NULL;

	bOutput				= TRUE;
	m_bForcedMenu		= FALSE;
	m_dwWorldId			= NULL;
	m_dwMusicId			= NULL;
	m_dwMoverIdx		= NULL;
	m_dwHairMesh		= NULL;
	m_dwHeadMesh		= NULL;
	m_dwHairColor		= NULL;
	m_dwContributionID	= NULL;

	ZeroMemory( m_adwEquip,		sizeof( DWORD ) * MAX_HUMAN_PARTS );
	ZeroMemory( m_abMoverMenu,	sizeof( BOOL ) * MAX_MOVER_MENU );
	ZeroMemory( &m_randomItem,	sizeof( RANDOM_ITEM ) );

	m_awSrcQuest.RemoveAll();
	m_awDstQuest.RemoveAll();
	m_anSrcQuestItem.RemoveAll();
	m_anDstQuestItem.RemoveAll();

	for( UINT nCount = 0; nCount < 4; nCount++ )
		m_venderItemAry[nCount].RemoveAll();

	for( UINT nCount = 0; nCount < 4; nCount++ )
		m_venderItemAry2[nCount].RemoveAll();

	m_vecdwLanguage.clear();
	m_vecTeleportPos.clear();
	m_vecNPCBuffSkill.clear();
	m_mapTeleportMenu.clear();
}

bool	CHARACTER::IsTeleportMenu( const int nMMI_ID ) const
{
	std::map< DWORD, T_WORLD_POSITION >::const_iterator CItr = m_mapTeleportMenu.find( nMMI_ID );
	if( CItr != m_mapTeleportMenu.end() )
	{
		return true;
	}

	return false;
}

const T_WORLD_POSITION*	CHARACTER::GetTeleportPos( const int nMMI_ID ) const
{
	std::map< DWORD, T_WORLD_POSITION >::const_iterator CItr = m_mapTeleportMenu.find( nMMI_ID );
	if( CItr != m_mapTeleportMenu.end() )
	{
		return &CItr->second;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
// CDropItemGenerator
////////////////////////////////////////////////////////////////////////////////////
DROPITEM* CDropItemGenerator::GetAt( int nIndex, BOOL /*bUniqueMode*/, float fProbability )
{
	//FLASSERT( nIndex < m_dwSize );
	FLASSERT( nIndex < (int)GetSize() );

	if( fProbability > 0.0f ) 
	{
		DROPITEM* lpDropItem	= &m_dropItems[ nIndex ];
		DWORD dwProbability		= lpDropItem->dwProbability;
		DWORD dwRand = xRandom( 3000000000 );
#if defined(__WORLDSERVER) // __WORLDSERVER
		dwRand = static_cast<DWORD>(dwRand / fProbability );
#endif // __WORLDSERVER
		/*
		if( lpDropItem->dwLevel && bUniqueMode && lpDropItem->dwProbability <= 10000000 )
		{
		dwRand /= 2;
		}
		// 레벨(유니크)이 있나 검사
		if( lpDropItem->dwLevel != 0 )
		{
		PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( lpDropItem->dwIndex );
		if( pItemProp && (int)pItemProp->dwItemLV > 0 )
		{
		int nValue	= dwProbability / pItemProp->dwItemLV;
		if( nValue < 21 )	nValue	= 21;
		dwProbability	= nValue - 20;
		}
		}
		*/
		return( dwRand < dwProbability? lpDropItem: NULL );
	}
	else 
	{
		return &m_dropItems[ nIndex ];
	}
}

void CDropItemGenerator::AddTail( CONST DROPITEM & rDropItem, const char* /*s*/ )
{
	m_dropItems.push_back( rDropItem );
}

// CQuestItemGenerator
QUESTITEM* CQuestItemGenerator::GetAt( int nIndex )
{
	FLASSERT( nIndex < (int)( m_uSize ) );
	return &m_pQuestItem[nIndex];
}

void CQuestItemGenerator::AddTail( const QUESTITEM & rQuestItem )
{
	FLASSERT( m_uSize < MAX_QUESTITEM );
	memcpy( &m_pQuestItem[m_uSize++], &rQuestItem, sizeof(QUESTITEM) );
}

void CDropKindGenerator::AddTail( const DROPKIND & rDropKind )
{
	FLASSERT( m_nSize < MAX_DROPKIND );
	memcpy( &m_aDropKind[m_nSize++], &rDropKind, sizeof(DROPKIND) );
}

LPDROPKIND CDropKindGenerator::GetAt( int nIndex )
{
	FLASSERT( nIndex >= 0 );
	FLASSERT( nIndex < m_nSize );
	if( nIndex < 0 || nIndex >= m_nSize )
		return NULL;
	return &m_aDropKind[nIndex];
}


CProject::CProject()
#ifdef __WORLDSERVER
//sun: 12, 튜토리얼 개선
:
m_nMaxSequence( 0 )
#endif	// __WORLDSERVER
{
	memset( m_minMaxIdxAry, 0xff, sizeof(m_minMaxIdxAry) );
	memset( m_aExpUpItem, 0, sizeof(m_aExpUpItem) );
	memset( m_aGuildAppell, 0, sizeof(m_aGuildAppell) );


#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;	
	m_fMonsterHitpointRate = 1.0f;	
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_nMonsterPropSize = 0;
#endif // __S1108_BACK_END_SYSTEM

	memset( m_aSetItemAvail, 0, sizeof(m_aSetItemAvail) );

	m_nMoverPropSize = 0;
	m_pPropMover = new MoverProp[MAX_PROPMOVER];
	//	m_pPropMover = (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );
#ifdef __CLIENT
	m_fShopBuyRate = 1.0f;
	m_fShopSellRate = 1.0f;
#endif // __CLIENT

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	m_bMapTransparent = FALSE;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#ifdef __CLIENT
	m_bLoadedMoverProp  = FALSE;
#endif // __CLIENT
}

CProject::~CProject()
{
#ifdef __CLIENT
	m_mapHelp.RemoveAll();
	m_mapWordToolTip.RemoveAll();
#endif	// __CLIENT

	//sun: 10, __RULE_0615
	m_sInvalidNames.clear();
	m_sAllowedLetter.clear();
#ifdef __VENDOR_1106
	m_sAllowedLetter2.clear();
#endif	// __VENDOR_1106

	int i, j;
	POSITION pos = m_mapCharacter.GetStartPosition();
	LPCHARACTER lpCharacter;
	CString string;

	while(pos)
	{
		m_mapCharacter.GetNextAssoc(pos,string,(void*&)lpCharacter);
		for( j = 0; j < MAX_VENDOR_INVENTORY_TAB; j++ )
		{
			for( i = 0; i < lpCharacter->m_venderItemAry[ j ].GetSize(); i++)
				SAFE_DELETE_CONST( (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[ j ].GetAt(i) );
		}

		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
		for( j = 0; j < MAX_VENDOR_INVENTORY_TAB; j++ )
		{
			for( i = 0; i < lpCharacter->m_venderItemAry2[ j ].GetSize(); i++)
				SAFE_DELETE_CONST( (LPVENDOR_ITEM)lpCharacter->m_venderItemAry2[ j ].GetAt(i) );
		}
		SAFE_DELETE( lpCharacter );
	}

	m_mapCharacter.RemoveAll();

	for( i = 0; i < m_colorText.GetSize(); i++ )
	{
		tagColorText* lpText = m_colorText.GetAt(i);
		if( lpText && lpText->lpszData )
			free( lpText->lpszData ); 
	}
	for( i = 0; i < m_aPropQuest.GetSize(); i++ )
	{
		QuestProp* pQuestProp = (QuestProp*)m_aPropQuest.GetAt( i );
		if( pQuestProp )
		{
#if defined( __WORLDSERVER ) 
			for( int nNth = 0; nNth < 32; ++nNth )
			{
				SAFE_DELETE_ARRAY( pQuestProp->m_apQuestDialog[ nNth ] );
			}
#endif // __WORLDSERVER
			for( j = 0; j < 16; j++ )
			{
				SAFE_DELETE( pQuestProp->m_questState[ j ] );
			}
			SAFE_DELETE_ARRAY( pQuestProp->m_paBeginCondItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndCondItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndRewardItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_lpszEndCondMultiCharacter );
			//sun: 8, // __S8_PK
			SAFE_DELETE_ARRAY( pQuestProp->m_paBeginCondNotItem );
			SAFE_DELETE_ARRAY( pQuestProp->m_paEndCondOneItem );
		}
	}

	m_aPropQuest.RemoveAll();

	SAFE_DELETE_ARRAY( m_pPropMover );
	//	::VirtualFree( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, MEM_DECOMMIT );

#ifdef __CLIENT
	m_setBlockedUserID.clear();
#endif // __CLIENT
}
int SortJobSkill( const void *arg1, const void *arg2 )
{
	SkillProp* pSkillProp1 = *(SkillProp**) arg1;
	SkillProp* pSkillProp2 = *(SkillProp**) arg2;
	if( pSkillProp1->dwReqDisLV < pSkillProp2->dwReqDisLV )
		return -1; 
	if( pSkillProp1->dwReqDisLV >= pSkillProp2->dwReqDisLV )
		return 1;
	return 0;
}

void CProject::LoadSkill()
{
	LoadPropSkill( "propSkill.txt", &m_aPropSkill );
	ZeroMemory( m_aJobSkillNum, sizeof( m_aJobSkillNum ) );
	// 각각의 스킬을 정렬해서 배열에 담자 
	for( int i = 1; i < m_aPropSkill.GetSize(); i++ )
	{
		SkillProp* pSkillProp = m_aPropSkill.GetAt( i );
		if( pSkillProp )
		{
			if( pSkillProp->dwSkillKind1 != JTYPE_COMMON )
			{
				SkillProp** apJobSkill = m_aJobSkill[ pSkillProp->dwSkillKind2 ];
				apJobSkill[ m_aJobSkillNum[ pSkillProp->dwSkillKind2 ] ] = pSkillProp;
				m_aJobSkillNum[ pSkillProp->dwSkillKind2 ]++;
			}
		}
	}

	// 소트하기 
	for( int i = 0; i < MAX_JOB; i++ )
	{
		SkillProp** apJobSkill = m_aJobSkill[ i ];
		qsort( (void *)apJobSkill, (size_t) m_aJobSkillNum[ i ], sizeof( SkillProp* ), SortJobSkill );
	}
}

//sun: 10, __EVE_MINIGAME
BOOL CProject::LoadMiniGame()
{
	if( m_MiniGame.Load_Alphabet() == FALSE )
	{
		// 메세지 처리
	}

	if( m_MiniGame.Load_KawiBawiBo() == FALSE )
	{
		// ...
	}

	if( m_MiniGame.Load_FiveSystem() == FALSE )
	{
		// ...
	}

	if( m_MiniGame.Load_Reassemble() == FALSE )
	{
		// ...
	}

	return TRUE;

}

//sun: 9,10차 제련 __ULTIMATE
BOOL CProject::LoadUltimateWeapon()
{
#ifdef __WORLDSERVER
	if( m_UltimateWeapon.Load_UltimateWeapon() == FALSE )
	{
		return FALSE;
	}
#endif // __WORLDSERVER

	if( m_UltimateWeapon.Load_GemAbility() == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load( lpszFileName )==FALSE)
		return FALSE;

#if !defined(__CLIENT)
	if( LoadPreFiles() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "FALIED LOAD PRE FILES" ) );
		return FALSE;
	}
#endif

#if defined __WORLDSERVER && defined __ENCHANT_BARUNA16
	CItemUpgrade::GetInstance();		//preloading 미리 읽어 data 검증.
#endif //__ENCHANT_BARUNA16

#if defined __WORLDSERVER && defined __AGGRO16
	if( AGGRO_PROP().Load() == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL - LOAD AGGROPROP ]" ) );
		return FALSE;
	}
#endif //__AGGRO16

	//////////////////////////////////////////////////////////////////////////
	// All Spec File Load
	if( g_xSpecManager->LoadSpecAll() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LoadSpecAll Failed." ) );
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	g_pSeasonEffect->load();

#ifdef __WORLDSERVER
	
	g_pItemUpgradeManager->Load();

#endif // __WORLDSERVER


	do 
	{
		scanner.GetToken();
		if( scanner.Token == "propTroupeSkill" )
		{	
			scanner.GetToken();	
			LoadPropSkill( "propTroupeSkill.txt", &m_aPartySkill );
		}
		else if( scanner.Token == "propSkillAdd" )
		{
			scanner.GetToken();
			LoadPropAddSkill( scanner.token );
		}
		else if( scanner.Token == "propQuest" )
		{
			scanner.GetToken();
			LoadPropQuest( scanner.token, FALSE );
		}
		else if( scanner.Token == "expTable" )
		{	
			scanner.GetToken();	
			LoadExpTable( scanner.token );
		}
		else if( scanner.Token == "MotionProp" )
		{	
			scanner.GetToken();	
			LoadMotionProp( scanner.token );
		}
		else if( scanner.Token == "propJob" )
		{
			scanner.GetToken();
			LoadPropJob( "propJob.inc" );
		}
		else if( scanner.Token == "propSkill" )
		{	
			scanner.GetToken();	
			LoadSkill();
		}
		else if( scanner.Token == "model" )
		{	
			scanner.GetToken();	
			m_modelMng.LoadScript( scanner.token );
		}
		else if( scanner.Token == "propMover" )
		{	
			scanner.GetToken();	
			if( !LoadPropMover( "propMover.txt" ) )
				return FALSE;
		}
		else if( scanner.Token == "propCtrl" )
		{
			scanner.GetToken();
			LoadPropCtrl( "propCtrl.txt", &m_aPropCtrl );
		}
		else if( scanner.Token == "propItem" )
		{	
			scanner.GetToken();	
			//LoadPropItem( "propItem.txt", &m_aPropItem );
			OnAfterLoadPropItem();
		}
		else if( scanner.Token == "character" )
		{
			scanner.GetToken();
			LoadCharacter( scanner.token );
		}
		else if( scanner.Token == "world" )
		{	
			scanner.GetToken();	
			g_WorldMng.LoadScript( scanner.token );
		}
		else if( scanner.Token == "terrain" )
		{	
			scanner.GetToken();	
#ifdef __CLIENT
#ifdef __FLYFF_INITPAGE_EXT
			if(!CWndBase::m_Theme.m_bLoadTerrainScript)
				m_terrainMng.LoadScript( scanner.token );
#else //__FLYFF_INITPAGE_EXT
			m_terrainMng.LoadScript( scanner.token );
#endif //__FLYFF_INITPAGE_EXT
#endif
		}
		else if( scanner.Token == "help" )
		{	
			scanner.GetToken();	
#ifdef __CLIENT
			LoadHelp( scanner.token );
#endif
		}
	} while(scanner.tok!=FINISHED);


	LoadMotionProp( "propMotion.txt" );
	LoadPropMoverEx( "PropMoverEx.inc" );
	LoadJobItem( "jobItem.inc" );
	LoadEtc( "etc.inc" );

	//sun: 제련 이팩트 향상 버전
	LoadPropEnchant( "propEnchant.inc" );

#ifdef __WORLDSERVER



	LoadConstant( "Constant.inc" );
	LoadPropGuildQuest( "propGuildQuest.inc" );
	LoadPropPartyQuest( "propPartyQuest.inc" );

	if( _GetContentState( CT_DROP_ITEM_REFORM ) != CS_VER1 )
		LoadDropEvent( "propDropEvent.inc" );

	LoadGiftbox( "propGiftbox.inc" );
	LoadPackItem( "propPackItem.inc" );

	LoadScriptDiePenalty( "DiePenalty.inc" );	//sun: 8,

#endif // __WORLDSERVER

	LoadScriptPK( "PKSetting.inc" );			//sun: 8, // __S8_PK
	LoadPiercingAvail( "propItemEtc.inc" );

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	Load_GemAbility();
#endif

#ifdef __EVENT_0117
#ifdef __WORLDSERVER
	CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" );
#endif	// __WORLDSERVER
#endif	// __EVENT_0117

#ifdef __CLIENT	// 클라이언트만 로드
	CEventGeneric::GetInstance()->LoadScript( "propEvent.inc" );
	LoadFilter( GetLangFileName( g_xFlyffConfig->GetMainLanguage(), FILE_FILTER ) );
#endif	// __CLIENT

	//sun: 10, __RULE_0615
	LoadInvalidName();
	LoadAllowedLetter();
#ifdef __VENDOR_1106
	LoadAllowedLetter( TRUE );
#endif	// __VENDOR_1106

	CPetProperty::GetInstance()->LoadScript( "pet.inc" );		//sun: 9, 9-10차 펫

	//sun: 11, 채집 시스템
	CCollectingProperty::GetInstance()->LoadScript( "collecting.inc" );
	CAccessoryProperty::GetInstance()->LoadScript( "accessory.inc" );

	m_aPropQuest.Optimize();

	//sun: 10, __EVE_MINIGAME
#ifdef __WORLDSERVER
	LoadMiniGame();
#endif // __WORLDSERVER

	LoadUltimateWeapon();		//sun: 9,10차 제련 __ULTIMATE

	if( g_pExchangeEvent->LoadScript() == false )
	{
		return FALSE;
	}

#if defined(__WORLDSERVER) // __WORLDSERVER
	prj.m_EventLua.LoadScript();
#endif // __WORLDSERVER

	//sun: 11, 일대일 길드 대전
#ifdef __WORLDSERVER
	g_GuildCombat1to1Mng.LoadScript();
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
	LoadServerScript( "s.txt" );
#endif	// __WORLDSERVER

#ifdef __EVENT_MONSTER
	CEventMonster::GetInstance()->LoadScript();
#endif // __EVENT_MONSTER

#ifdef __WORLDSERVER
	CSecretRoomMng::GetInstance()->LoadScript();		//sun: 12, 비밀의 방
	CMonsterSkill::GetInstance()->LoadScript();			//sun: 12, // __MONSTER_SKILL
	CCreateMonster::GetInstance()->LoadScript();		//sun: 12, // __NEW_ITEMCREATEMON_SERVER

	CQuiz::GetInstance()->LoadScript();

	CCampusHelper::GetInstance()->LoadScript();

	GuildHouseMng->LoadScript();

//	g_pItemUpgradeManager->Load();
	g_xBarunaUpgradeNpc->LoadScript();
	g_xItemExtract->LoadScript();
	g_xItemMergeManager->LoadScript();

#endif // __WORLDSERVER

	ProtectPropMover();

#ifdef __PERF_0226
	CPartsItem::GetInstance()->Init();
#endif	// __PERF_0226

	//sun: 12, 군주
#ifdef __WORLDSERVER
	CSLord::Instance()->CreateColleagues();
#else	// __WORLDSERVER
	CCLord::Instance()->CreateColleagues();
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
	//sun: 12, // __RANGDA_0521
	CRangdaController::Instance()->LoadScript( "randomeventmonster.inc" );
	CTransformItemProperty::Instance()->LoadScript( "transformitem.inc" );
#endif // __WORLDSERVER

#ifdef __WORLDSERVER
	LoadGuideMaxSeq();	//sun: 12, 튜토리얼 개선

//	CRainbowRaceMng::GetInstance()->LoadScript();	//sun: 13, 레인보우 레이스
	CRainbowRaceMng::GetInstance()->Load();

#endif	// __WORLDSERVER
	//	OutputDropItem();
	//	return FALSE;

	//	CCoupleTestCase ctc;
	//	CTestCaseMgr::Instance()->Test();

	//sun: 13, 커플 시스템
#ifdef __WORLDSERVER
	CCoupleHelper::Instance()->Initialize( &g_dpDBClient, &g_DPSrvr );
#else	// __WORLDSERVER
	CCoupleHelper::Instance()->Initialize();
#endif	// __WORLDSERVER

	//sun: 13, 달인
	CTitleManager::Instance()->LoadTitle("honorList.txt");

#if defined( __CLIENT )
	LoadQuestDestination();
	LoadPatrolDestination();
#endif // defined( __CLIENT )

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
//gmpbigsun( 20120703 ) : LoadMapInformationData가 불리면 캐릭터 모델에 이상현상이 발생한다. 최후 월드진입 직전에 로드
//						  왜 그런지 나중에 찾아보자..
//	m_MapInformationManager.LoadMapInformationData();
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#ifdef __WORLDSERVER
	CONSIGNMENT_SALE_SPEC().Load();

	switch( _GetContentState( CT_DROP_ITEM_REFORM ) )
	{
	case CS_VER1:
		{
			GOLD_ICONSET().Load();
			DROPPENALTY_BYLEVELDIST().Load();

			MONSTERDROPITEM_MNG().Load();
			LEVELDROPITEM_MNG().Load();
//			EVENTDROPITEM_MNG().Load();
		}
		break;
	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID VERSION (%d) ]" ), _GetContentState( CT_DROP_ITEM_REFORM ) );
		break;
	}

	g_pMadrigalGift->LoadScript();

	g_pFlyffPieceEvent->LoadScript();

	using namespace nsFlyffPieceDailyPayment;
	g_pFlyffPieceDailyPayment->Load();

	CInstanceDungeonParty::GetInstance()->LoadScript("PartyDungeon.lua");
#endif //__WORLDSERVER

	return TRUE;
}

#ifdef __WORLDSERVER
void CProject::SetGlobal( UINT type, float fValue )
{
	switch( type )
	{
	case GAME_RATE_ITEMDROP:
		if( fValue > 20.0f )
			fValue = 20.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fItemDropRate = fValue;
		break;
	case GAME_RATE_MONSTEREXP:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fMonsterExpRate = fValue;
		break;
	case GAME_RATE_MONSTERHIT:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fMonsterHitRate = fValue;
		break;
	case GAME_RATE_GOLDDROP:
		if( fValue > 5.0f )
			fValue = 5.0f;
		else if( fValue < 0.1f )
			fValue = 0.1f;
		m_fGoldDropRate = fValue;
		break;
	default:
		FLASSERT( 0 );
	}
}

void CProject::ReadConstant( CScript& script )
{
	do 
	{
		script.GetToken(); 
		if( script.Token == "itemDropRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_ITEMDROP, script.GetFloat() );
		}
		else if( script.Token == "monsterExpRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_MONSTEREXP, script.GetFloat() );
		}
		else if( script.Token == "monsterHitRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_MONSTERHIT, script.GetFloat() );
		}
		else if( script.Token == "goldDropRate" )
		{	
			script.GetToken();// bypass '='
			SetGlobal( GAME_RATE_GOLDDROP, script.GetFloat() );
		}
		else if( script.Token == "dwVagSP" )
		{
			script.GetToken();// bypass '='
			m_dwVagSP = script.GetNumber();
		}
		else if( script.Token == "dwExpertSP" )
		{
			script.GetToken();// bypass '='
			m_dwExpertSP = script.GetNumber();
		}
		else if( script.Token == "dwProSP" )
		{
			script.GetToken();// bypass '='
			m_dwProSP = script.GetNumber();
		}
	} 
	while( script.tok != FINISHED && *script.token != '}' );
}

BOOL CProject::LoadConstant( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	do 
	{
		if( script.Token == "lang" )
		{
			int nLang = script.GetNumber();
			if( g_xFlyffConfig->GetMainLanguage() == nLang ) 
			{
				script.GetToken(); // {
				do 
				{
					if( script.Token == "formula" )
					{
						ReadConstant( script );
					}

					script.GetToken(); // server type
				} while( script.tok != FINISHED && *script.token != '}' );
				script.GetToken(); // }
			}
		}
		script.GetToken();
	} while( script.tok != FINISHED );

	return TRUE;
}
#endif //__WORLDSERVER

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel )
{
	if( dwLevel < 1 )
		dwLevel = 1;

	int nskillIndex = dwSubDefine + dwLevel - 1;

	if( nskillIndex < 5)
		nskillIndex += 5;

	if( nskillIndex < 0 || nskillIndex >= m_aPropAddSkill.GetSize() )	
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "범위침범. %d" ), nskillIndex );
		return 0;
	}

	return m_aPropAddSkill.GetAt( nskillIndex );
}

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine )
{
	return GetAddSkillProp( dwSubDefine, 1 );
}

BOOL CProject::LoadEtc( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	int id;
	do {
		script.GetToken();
		if( script.Token == _T( "job" ) )
		{	
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken();
				FLStrcpy( m_aJob[ id ].szName, _countof( m_aJob[ id ].szName ), script.token );
				m_aJob[ id ].dwJobBase = script.GetNumber();
				m_aJob[ id ].dwJobType = script.GetNumber();
				id = script.GetNumber();
			}
		}
		else if( script.Token == _T( "structure" ) )
			{	
				script.GetToken(); // skip {
				id = script.GetNumber();
				while( *script.token != '}' )
				{
					script.GetToken();
					FLStrcpy( m_aStructure[ id ].szName, _countof( m_aStructure[ id ].szName ), script.token );
					id = script.GetNumber();
				}
			}
			else if( script.Token == _T( "Guild" ) )
				{
					script.GetToken(); // skip {
					id = script.GetNumber();
					while( *script.token != '}' )
					{
						script.GetToken();
						FLStrcpy( m_aGuildAppell[ id ].szName, _countof( m_aGuildAppell[ id ].szName ), script.token );
						id = script.GetNumber();
					}
				}
	} while( script.tok != FINISHED );
	return TRUE;
}

BOOL CProject::LoadMotionProp( LPCTSTR lpszFileName )
{
	// 버전  모션 인덱스  요구Lv  요구경험치  표시단어 루트명 연결단어 
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	MotionProp MotionProp;
	ZeroMemory( &MotionProp, sizeof( MotionProp ) );


	int nVer = script.GetNumber();  // version		061206 ma	8차에 들어갈 모션관리를 위해 버전 추가	propMotion.txt

	while( script.tok != FINISHED )
	{
		DWORD dwID = script.GetNumber();	// IID_
		if( dwID <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s ID:%d last_read(ID:%d name:%s)" ), lpszFileName, dwID, MotionProp.dwID, MotionProp.szName);
			return FALSE;
		}

		MotionProp.dwID = dwID;
		MotionProp.dwMotion = script.GetNumber();
		script.GetToken();  
		script.GetToken();  
		if( strlen(script.m_mszToken) >= sizeof(MotionProp.szIconName) )
			FLERROR_LOG( PROGRAM_NAME, _T( "메모리침범. %s 버퍼크기(%d), 스트링길이(%d)" ), script.m_mszToken, sizeof(MotionProp.szIconName), strlen(script.m_mszToken) );

		FLStrcpy( MotionProp.szIconName, _countof( MotionProp.szIconName ), script.m_mszToken );
		script.GetToken();  
		MotionProp.dwPlay = (DWORD)script.GetNumber();

		script.GetToken();
		FLStrcpy( MotionProp.szName, _countof( MotionProp.szName ), script.m_mszToken );
		script.GetToken();
		FLStrcpy( MotionProp.szDesc, _countof( MotionProp.szDesc ), script.m_mszToken );
		if( nVer <= __VER )
		{
			m_aPropMotion.SetAt( MotionProp.dwID, &MotionProp);
		}

		nVer = script.GetNumber(); 
	}
	m_aPropMotion.Optimize();
	return TRUE;

}

#ifdef __WORLDSERVER
//sun: 12, 튜토리얼 개선
BOOL CProject::LoadGuideMaxSeq()
{

	CScript script;
	if( script.Load(MakePath(DIR_CLIENT, "Guide.inc")) == FALSE )
		return FALSE;

	script.tok = 0;
	m_nMaxSequence = 0;

	while( script.tok != FINISHED )
	{
		script.GetNumber();

		script.GetToken(); // {
		if( *script.token  == '{' )
		{
			script.GetToken(); // BEGINNER
			script.GetNumber();
			script.GetToken(); // SHOWLEVEL
			script.GetNumber();
			script.GetToken(); // FLAG
			script.GetNumber();
			script.GetToken(); // KEY
			script.GetNumber();
			script.GetToken();
			script.Token;
			script.GetToken();
			if( _strcmpi( script.token, "SEQUENCE" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( _strcmpi( script.token, "LEVEL" ) == 0 )
			{
				int	nCurSeq = script.GetNumber();
				if(m_nMaxSequence < nCurSeq) m_nMaxSequence = nCurSeq;
				script.GetToken();
			}
			if( _strcmpi( script.token, "CONDITION" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( _strcmpi( script.token, "INPUTCHAR" ) == 0 )
			{
				script.GetNumber();
				script.GetToken();
			}
			if( _strcmpi( script.token, "INPUTSTR" ) == 0 )
			{
				script.GetToken();
				script.GetToken();	// }
				while(*script.token  != '}')
				{
					script.GetToken();	// }
				}
			}

		}
		else
			script.GetToken();
	}

	return TRUE;
}

#endif	// __WORLDSERVER

#ifdef __CLIENT
BOOL CProject::LoadHelp( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString strKeyword;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		strKeyword	= s.token;
		s.GetToken();	// {
		s.GetToken();
		CString string = "   ";
		while( *s.token != '}' )
		{
			string		+= s.Token;
			string		+= "\n\n   ";
			s.GetToken();
		}
		m_mapHelp.SetAt( strKeyword, string );
		s.GetToken();
	}
	return TRUE;
}
CString CProject::GetHelp( LPCTSTR lpStr )
{
	CString string;
	m_mapHelp.Lookup( lpStr, string );
	return string;
}
CString CProject::GetWordToolTip( LPCTSTR lpStr )
{
	CString string;
	m_mapWordToolTip.Lookup( lpStr, string );
	return string;
}
BOOL CProject::LoadFilter( LPCTSTR lpszFileName )
{	
	CScanner scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	FILTER filter;
	scanner.GetToken();

	while( scanner.tok != FINISHED )
	{
		//sun: 10, __FILTER_0705
		BOOL bAlpha		= TRUE;
		int nLen	= lstrlen( scanner.token );

		int	nIsAlpha	= 0;

		for( int i = 0; i < nLen; i++ )
		{
			nIsAlpha = _ismbcalpha( scanner.token[i] );
			if( nIsAlpha == 0 )
			{
				bAlpha	= FALSE;
				break;
			}
		}
		if( bAlpha )
			scanner.Token.MakeLower();

		FLStrcpy( filter.m_szSrc, _countof( filter.m_szSrc ), scanner.Token );
		scanner.GetToken();
		FLStrcpy( filter.m_szDst, _countof( filter.m_szDst ), scanner.Token );
		//sun: 10, __FILTER_0705
		if( bAlpha )
			m_mapAlphaFilter.insert( std::map<std::string, std::string>::value_type( filter.m_szSrc, filter.m_szDst ) );
		else
			m_mapNonalphaFilter.insert( std::map<std::string, std::string>::value_type( filter.m_szSrc, filter.m_szDst ) );

		scanner.GetToken(); 
	}

	return TRUE;
}
BOOL CProject::LoadWordToolTip( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load(lpszFileName)==FALSE)
		return FALSE;
	CString strWord;
	CString strToolTip;
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		strWord = scanner.Token;
		scanner.GetToken();
		m_mapWordToolTip.SetAt( strWord, scanner.token );
		scanner.GetToken(); 
	}
	return TRUE;
}
#endif

BOOL CProject::LoadPropGuildQuest( LPCTSTR lpszFilename )
{
#ifdef __WORLDSERVER
	CGuildQuestProcessor* pProcessor	= CGuildQuestProcessor::GetInstance();
#endif	// __WORLDSERVER
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
	GUILDQUESTPROP prop;

	int nQuestId	= s.GetNumber();	// id

	while( s.tok != FINISHED )
	{
		ZeroMemory( &prop, sizeof(prop) );

		s.GetToken();
		while( s.Token[0] != '}' )
		{
			if( s.Token == "Title" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( prop.szTitle, _countof( prop.szTitle ), s.Token );
			}
			else if( s.Token == "Level" )
			{
				s.GetToken();	// =
				prop.nLevel		= s.GetNumber();
			}
			else if( s.Token == "Wormon" )
			{
				s.GetToken();	// =
				prop.dwWormon	= s.GetNumber();
			}
			else if( s.Token == "World" )
			{
				s.GetToken();	// =
				prop.dwWorldId	= s.GetNumber();
			}
			else if( s.Token == "Position" )
			{
				s.GetToken();	// =
				prop.vPos.x		= s.GetFloat();
				s.GetToken();	// ,
				prop.vPos.y		= s.GetFloat();
				s.GetToken();	// ,
				prop.vPos.z		= s.GetFloat();
			}
			else if( s.Token == "Region" )
			{
				s.GetToken();	// =
				prop.x1	= s.GetNumber();
				s.GetToken();	// ,
				prop.y1	= s.GetNumber();
				s.GetToken();	// ,
				prop.x2	= s.GetNumber();
				s.GetToken();	// ,
				prop.y2	= s.GetNumber();
#ifdef __WORLDSERVER
				pProcessor->AddQuestRect( nQuestId, prop.x1, prop.y1, prop.x2, prop.y2 );
#endif	// __WORLDSERVER
			}
			else if( s.Token == "State" )
			{
				int nState	= s.GetNumber();
				s.GetToken();	// key or anything
				while( s.Token[0] != '}' )
				{
					if( s.Token == "desc" )
					{
						s.GetToken();	// =
						s.GetToken();
						FLStrcpy( prop.szDesc[nState], _countof( prop.szDesc[nState] ), s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		m_aPropGuildQuest.SetAt( nQuestId, &prop );
		nQuestId	= s.GetNumber();	// id
	}
	m_aPropGuildQuest.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropPartyQuest( LPCTSTR lpszFilename )
{
#ifdef __WORLDSERVER
	CPartyQuestProcessor* pProcessor	= CPartyQuestProcessor::GetInstance();
#endif	// __WORLDSERVER
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;
	//	PARTYQUESTPROP prop;

	int nQuestId	= s.GetNumber();	// id

	while( s.tok != FINISHED )
	{
		PARTYQUESTPROP prop;
		//		ZeroMemory( &prop, sizeof(prop) );

		s.GetToken();
		while( s.Token[0] != '}' )
		{
			if( s.Token == "Title" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( prop.szTitle, _countof( prop.szTitle ), s.Token );
			}
			else if( s.Token == "Level" )
			{
				s.GetToken();	// =
				prop.nLevel		= s.GetNumber();
			}
			else if( s.Token == "Wormon" )
			{
				prop.vecWormon.clear();
				int nErrCnt = 0;
				//
				s.GetToken();
				while( s.Token[0] != '}' )
				{
					WORMON WorMon = { 0 };
					//mem_set( &WorMon, 0, sizeof(WorMon) );
					nErrCnt++;

					WorMon.dwWormon = s.GetNumber();
					s.GetToken();	// ,
					WorMon.vPos.x   = s.GetFloat();
					s.GetToken();	// ,
					WorMon.vPos.y   = s.GetFloat();
					s.GetToken();	// ,
					WorMon.vPos.z   = s.GetFloat();
					s.GetToken();	// ,

					prop.vecWormon.push_back( WorMon );

					if( nErrCnt > 1000 )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "propPartyQuest.inc = %s Error" ), prop.szTitle );
						return FALSE;
					}
				}				
			}
			else if( s.Token == "World" )
			{
				s.GetToken();	// =
				prop.dwWorldId	= s.GetNumber();
			}
			else if( s.Token == "WorldKey" )
			{
				s.GetToken();	// =
				s.GetToken();
				FLStrcpy( prop.szWorldKey, _countof( prop.szWorldKey ), s.Token );
			}	
			else if( s.Token == "Region" )
			{
				s.GetToken();	// =
				prop.x1	= s.GetNumber();
				s.GetToken();	// ,
				prop.y1	= s.GetNumber();
				s.GetToken();	// ,
				prop.x2	= s.GetNumber();
				s.GetToken();	// ,
				prop.y2	= s.GetNumber();
#ifdef __WORLDSERVER
				pProcessor->AddQuestRect( nQuestId, prop.dwWorldId, prop.x1, prop.y1, prop.x2, prop.y2 );
#endif	// __WORLDSERVER
			}
			else if( s.Token == "State" )
			{
				int nState	= s.GetNumber();
				s.GetToken();	// key or anything
				while( s.Token[0] != '}' )
				{
					if( s.Token == "desc" )
					{
						s.GetToken();	// =
						s.GetToken();
						FLStrcpy( prop.szDesc[nState], _countof( prop.szDesc[nState] ), s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		bool bResult = m_propPartyQuest.insert( std::map<int, PARTYQUESTPROP>::value_type( nQuestId, prop) ).second;
		UNREFERENCED_PARAMETER( bResult );
		nQuestId	= s.GetNumber();	// id
	}
	return TRUE;
}


BOOL CProject::LoadPropQuest( LPCTSTR lpszFileName, BOOL bOptimize )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	QuestProp propQuest;
	TCHAR szLinkChar[ 10 ][ 64 ];
	int szLinkCharNum = 0;
	int szEndCondCharNum = 0;
	int nQuest = script.GetNumber();  // id
	if( nQuest > MAX_QUEST_INDEX )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ QUEST INDEX OVERFLOW. INDEX : %d, MAX : %d ]" ), nQuest, MAX_QUEST_INDEX );
		return FALSE;
	}
	static DWORD dwGoalIndex = 0;

	while( script.tok != FINISHED )
	{
		ZeroMemory( &propQuest, sizeof( propQuest ) );

		BOOL bEndCondCharacter = FALSE;
		propQuest.m_nBeginCondSex = -1;
		propQuest.m_wId = static_cast< WORD >( nQuest );

		QuestPropItem aEndRewardItem[ MAX_QUESTCONDITEM ]; 
		int nEndRewardItemNum = 0;
		ZeroMemory( aEndRewardItem, sizeof( aEndRewardItem ) );

		QuestPropItem aBeginCondItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondItemNum = 0;
		ZeroMemory( aBeginCondItem, sizeof( aBeginCondItem ) );

		//sun: 8, // __S8_PK
		QuestPropItem aBeginCondNotItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondNotItemNum = 0;
		ZeroMemory( aBeginCondNotItem, sizeof( aBeginCondNotItem ) );		

		QuestPropItem aEndCondOneItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondOneItemNum = 0;
		ZeroMemory( aEndCondOneItem, sizeof( aEndCondOneItem ) );				

		QuestPropItem aEndCondItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondItemNum = 0;
		ZeroMemory( aEndCondItem, sizeof( aEndCondItem ) );

		ZeroMemory( szLinkChar, sizeof( szLinkChar ) );
		szLinkCharNum = 0;
		szEndCondCharNum = 0;

#if defined( __WORLDSERVER ) 
		ZeroMemory( propQuest.m_apQuestDialog, sizeof( propQuest.m_apQuestDialog ) );
#endif

		//sun: 9, 9-10차 펫
		propQuest.m_nBeginCondPetLevel	= -1;	// PL_EGG가 0이므로 기본값은 -1
		propQuest.m_nEndCondPetLevel	= -1;	// PL_EGG가 0이므로 기본값은 -1

		propQuest.m_nBeginCondTutorialState		= -1;		//sun: 12, 튜토리얼 개선

		script.GetToken();
		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else if( script.Token == "}" )			
				nBlock--;
			else if( script.Token == "SetTitle" )
			{
				script.GetToken(); // (
				FLStrcpy( propQuest.m_szTitle, _countof( propQuest.m_szTitle ), GetLangScript( script ) );
			}
			else if( script.Token == "m_szTitle" )
			{
				script.GetToken(); // (
				script.GetToken();
				FLStrcpy( propQuest.m_szTitle, _countof( propQuest.m_szTitle ), script.Token );
			}
			else if( script.Token == "SetNPCName" )
			{
				script.GetToken(); // (
				FLStrcpy( propQuest.m_szNpcName, _countof( propQuest.m_szNpcName ), GetLangScript( script ) );
			}
			else if( script.Token == "m_szNPC" )
			{
				script.GetToken();
				script.GetToken();
			}
			else if( script.Token == "SetCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // key
				//sun: 13, // chipi_090702 : 국가 코드 추가
				char szKey[512] = {0,};
				FLStrcpy( szKey, _countof( szKey ), script.token );
				FLStrcpy( propQuest.m_szCharacterKey, _countof( propQuest.m_szCharacterKey ), script.token );	

				script.GetToken(); // , or )
				if( script.Token ==  "," )
				{
					int nLang = script.GetNumber();
					script.GetToken(); // ,
					int nSubLang = 0;
					if( script.Token == "," )
						nSubLang = script.GetNumber();
					if( nLang != g_xFlyffConfig->GetMainLanguage() )
						continue;
				}
				szLinkCharNum = 0;
				FLStrcpy( szLinkChar[ szLinkCharNum ], _countof( szLinkChar[ szLinkCharNum ] ), szKey ); 

				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum++ ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awSrcQuest.Add( static_cast< WORD >( nQuest ) );
					lpCharacter->m_anSrcQuestItem.Add( 0 );
				}
			}
			else if( script.Token == "SetMultiCharacter" )
			{
				script.GetToken(); // (
				szLinkCharNum = 0;
				do
				{
					script.GetToken(); // key
					FLStrcpy( szLinkChar[ szLinkCharNum ], _countof( szLinkChar[ szLinkCharNum ] ), script.token ); 
					script.GetToken(); // ,
					int nItem = script.GetNumber();
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awSrcQuest.Add( static_cast< WORD >( nQuest ) );
						lpCharacter->m_anSrcQuestItem.Add( nItem );
					}
					else
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) : %s가 존재하지 않음" ), lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
					}
					szLinkCharNum++;
					script.GetToken(); // , or )
				} while( *script.token != ')' && szLinkCharNum < 10 );
			}
			else
				// 시작 - 조건 
				if( script.Token == "SetBeginCondSex" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondSex  = static_cast< char >( script.GetNumber() );
				}
				else if( script.Token == "SetBeginCondSkillLvl" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondSkillIdx = script.GetNumber(); 
					script.GetToken(); // ,
					propQuest.m_nBeginCondSkillLvl = static_cast< char >( script.GetNumber() );
				}
				//sun: 8, // __S8_PK
				else if( script.Token == "SetBeginCondPKValue" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondPKValue = script.GetNumber();
				}
				else if( script.Token == "SetBeginCondNotItem" )
				{
					script.GetToken(); // (
					int nSex = script.GetNumber();
					script.GetToken(); // ,
					int nType = script.GetNumber();
					script.GetToken(); // ,
					int nJobOrItem = script.GetNumber();
					if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
					{
						nJobOrItem = -1;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetBeginCondNotItem 범위 초과(0~%d)" ), lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
					}
					script.GetToken(); // ,
					aBeginCondNotItem[ nBeginCondNotItemNum ].m_nSex = static_cast< char >( nSex );
					aBeginCondNotItem[ nBeginCondNotItemNum ].m_nType = static_cast< char >( nType );
					aBeginCondNotItem[ nBeginCondNotItemNum ].m_nJobOrItem =  nJobOrItem;
					aBeginCondNotItem[ nBeginCondNotItemNum ].m_nItemIdx = script.GetNumber();
					script.GetToken(); // ,
					aBeginCondNotItem[ nBeginCondNotItemNum ].m_nItemNum = script.GetNumber();
					
					if( ++nBeginCondNotItemNum >= MAX_QUESTCONDITEM )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ BEGIN COND NOT ITEM COUNT OVERFLOW. Max(%d) ]" ), MAX_QUESTCONDITEM );
						return FALSE;
					}
				}
				else if( script.Token == "SetBeginCondLevel" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondLevelMin = static_cast< BYTE >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nBeginCondLevelMax = static_cast< BYTE >( script.GetNumber() );
				}
				else if( script.Token == "SetBeginCondParty" ) 
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondParty = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nBeginCondPartyNumComp = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nBeginCondPartyNum = static_cast< char >( script.GetNumber() );
					script.GetToken(); // , 
					propQuest.m_nBeginCondPartyLeader = static_cast< char >( script.GetNumber() ); 
				}
				else if( script.Token == "SetBeginCondGuild" ) 
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondGuild = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nBeginCondGuildNumComp = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nBeginCondGuildNum = static_cast< WORD >( script.GetNumber() );
					script.GetToken(); // , 
					propQuest.m_nBeginCondGuildLeader = static_cast< char >( script.GetNumber() ); 
				}
				else if( script.Token == "SetBeginCondJob" )
				{
					script.GetToken(); // (
					do
					{
						propQuest.m_nBeginCondJob[ propQuest.m_nBeginCondJobNum++ ] = static_cast< char >( script.GetNumber() );
						script.GetToken(); // , or )
					} while( *script.token != ')' && propQuest.m_nBeginCondJobNum < MAX_JOB );
				}
				else if( script.Token == "SetBeginCondPreviousQuest" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondPreviousQuestType = static_cast< char >( script.GetNumber() ); // 0 시작중, 1 이전 퀘스트 완료
					script.GetToken(); // ,
					int nCnt = 0;
					do
					{
						propQuest.m_anBeginCondPreviousQuest[ nCnt++ ] = static_cast< WORD >( script.GetNumber() );
						script.GetToken(); // , or )
					} while( *script.token != ')' && nCnt < 6 );
				}
				else if( script.Token == "SetBeginCondExclusiveQuest" )
				{
					script.GetToken(); // (
					int nCnt = 0;
					do
					{
						propQuest.m_anBeginCondExclusiveQuest[ nCnt++ ] = static_cast< WORD >(script.GetNumber() );
						script.GetToken(); // , or )
					} while( *script.token != ')' && nCnt < 6 );
				}
				else if( script.Token == "SetBeginCondItem" )
				{
					script.GetToken(); // (
					int nSex = script.GetNumber();
					script.GetToken(); // ,
					int nType = script.GetNumber();
					script.GetToken(); // ,
					int nJobOrItem = script.GetNumber();
					if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
					{
						nJobOrItem = -1;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetBeginCondItem 범위 초과(0~%d)" ), lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
					}
					script.GetToken(); // ,
					aBeginCondItem[ nBeginCondItemNum ].m_nSex = static_cast< char >( nSex );
					aBeginCondItem[ nBeginCondItemNum ].m_nType = static_cast< char >( nType );
					aBeginCondItem[ nBeginCondItemNum ].m_nJobOrItem =  nJobOrItem;
					aBeginCondItem[ nBeginCondItemNum ].m_nItemIdx = script.GetNumber();
					script.GetToken(); // ,
					aBeginCondItem[ nBeginCondItemNum ].m_nItemNum = script.GetNumber();

					if( ++nBeginCondItemNum >= MAX_QUESTCONDITEM )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ BEGIN COND ITEM COUNT OVERFLOW. Max(%d) ]" ), MAX_QUESTCONDITEM );
						return FALSE;
					}
				}
				else if( script.Token == "SetBeginCondDisguise" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginCondDisguiseMoverIndex = script.GetNumber();
				}

				//////////////////////////////////////////////////////////////////
				// 시작 - 추가 및 변경 
				//////////////////////////////////////////////////////////////////
				else if( script.Token == "SetBeginSetDisguise" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginSetDisguiseMoverIndex = script.GetNumber();
				}
				else if( script.Token == "SetBeginSetAddGold" )
				{
					script.GetToken(); // (
					propQuest.m_nBeginSetAddGold = script.GetNumber();
				}
				else if( script.Token == "SetBeginSetAddItem" )
				{
					script.GetToken(); // (
					int nIdx = script.GetNumber();
					if( nIdx < 0 || nIdx > 3 )
					{
						nIdx = 0;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetBeginSetAddItem Idx 값 범위 초과(0~3)" ), lpszFileName, script.GetLineNum() ); 
					}
					script.GetToken(); // ,
					propQuest.m_nBeginSetAddItemIdx[nIdx] = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nBeginSetAddItemNum[nIdx] = script.GetNumber();
				}
				//sun: 9, 9-10차 펫
				else if( script.Token == "SetBeginCondPetExp" )
				{
					script.GetToken();	// (
					propQuest.m_nBeginCondPetExp	= script.GetNumber();
				}
				else if( script.Token == "SetBeginCondPetLevel" )
				{
					script.GetToken();	// (
					propQuest.m_nBeginCondPetLevel	= script.GetNumber();
				}

				//sun: 12, 튜토리얼 개선
				else if( script.Token == "SetBeginCondTutorialState" )
				{
					script.GetToken();	// (
					propQuest.m_nBeginCondTutorialState		= script.GetNumber();
				}
				else if( script.Token == "SetBeginCondTSP" )
				{
					script.GetToken();	// (
					propQuest.m_nBeginCondTSP	= script.GetNumber();
				}

#ifdef __GUILD_HOUSE_MIDDLE

				else if( script.Token == "SetBeginCondGuildWarWin" )
				{
					script.GetToken();	// (
					propQuest.m_nBeginCondGuildWarWin	= script.GetNumber();
				}

#endif // __GUILD_HOUSE_MIDDLE
				////////////////////////// 종료 - 조건 	
				else if( script.Token == "SetEndCondParty" ) 
				{
					script.GetToken(); // (
					propQuest.m_nEndCondParty = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nEndCondPartyNumComp = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nEndCondPartyNum = static_cast< WORD >( script.GetNumber() );
					script.GetToken(); // , 
					propQuest.m_nEndCondPartyLeader = static_cast< char >( script.GetNumber() );
				}
				else if( script.Token == "SetEndCondGuild" ) 
				{
					script.GetToken(); // (
					propQuest.m_nEndCondGuild = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nEndCondGuildNumComp = static_cast< char >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nEndCondGuildNum = static_cast< WORD >( script.GetNumber() );
					script.GetToken(); // , 
					propQuest.m_nEndCondGuildLeader = static_cast< char >( script.GetNumber() );
				}
				else if( script.Token == "SetEndCondState" ) 
				{
					script.GetToken(); // (
					propQuest.m_nEndCondState = static_cast< BYTE >( script.GetNumber() );
				}
				else if( script.Token == "SetEndCondCompleteQuest" ) 
				{
					script.GetToken(); // (
					propQuest.m_nEndCondCompleteQuestOper = static_cast< BYTE >( script.GetNumber() );
					script.GetToken(); // ,
					int nIdx = 0;
					while( *script.token != ')' )
					{
						propQuest.m_nEndCondCompleteQuest[ nIdx++ ] = static_cast< WORD >( script.GetNumber() );
						script.GetToken(); // ,
					}
				}
				else if( script.Token == "SetEndCondSkillLvl" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondSkillIdx = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nEndCondSkillLvl = static_cast< char >( script.GetNumber() );
				}
				//sun: 8, // __S8_PK
				else if( script.Token == "SetEndCondLevel" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondLevelMin = static_cast< BYTE >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nEndCondLevelMax = static_cast< BYTE >( script.GetNumber() );
				}
				//sun: 10차 전승시스템	Neuz, World, Trans
				else if( script.Token == "SetEndCondExpPercent" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondExpPercentMin = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nEndCondExpPercentMax = script.GetNumber();
				}
				else if( script.Token == "SetEndCondGold" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondGold = script.GetNumber();
				}
				else if( script.Token == "SetEndCondOneItem" )
				{
					script.GetToken(); // (
					int nSex = script.GetNumber();
					script.GetToken(); // ,
					int nType = script.GetNumber();
					script.GetToken(); // ,
					int nJobOrItem = script.GetNumber();
					if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
					{
						nJobOrItem = -1;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetEndCondOneItem 범위 초과(0~%d)" ), lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
					}
					script.GetToken(); // ,

					aEndCondOneItem[ nEndCondOneItemNum ].m_nSex = static_cast< char >( nSex );
					aEndCondOneItem[ nEndCondOneItemNum ].m_nType = static_cast< char >( nType );
					aEndCondOneItem[ nEndCondOneItemNum ].m_nJobOrItem =  nJobOrItem;
					aEndCondOneItem[ nEndCondOneItemNum ].m_nItemIdx = script.GetNumber();
					script.GetToken(); // ,
					aEndCondOneItem[ nEndCondOneItemNum ].m_nItemNum = script.GetNumber();
					
					if( ++nEndCondOneItemNum >= MAX_QUESTCONDITEM )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ END COND ONE ITEM COUNT OVERFLOW. Max(%d) ]" ), MAX_QUESTCONDITEM );
						return FALSE;
					}
				}
				else if( script.Token == "SetEndCondLimitTime" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondLimitTime = script.GetNumber();
				}
				else if( script.Token == "SetEndCondItem" )
				{
					script.GetToken(); // (
					int nSex = script.GetNumber();
					script.GetToken(); // ,
					int nType = script.GetNumber();
					script.GetToken(); // ,
					int nJobOrItem = script.GetNumber();
					if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
					{
						nJobOrItem = -1;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetEndCondItem 범위 초과(0~%d)" ), lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
					}
					script.GetToken(); // ,

					aEndCondItem[ nEndCondItemNum ].m_nSex = static_cast< char >( nSex );
					aEndCondItem[ nEndCondItemNum ].m_nType = static_cast< char >( nType );
					aEndCondItem[ nEndCondItemNum ].m_nJobOrItem =  nJobOrItem;
					aEndCondItem[ nEndCondItemNum ].m_nItemIdx = script.GetNumber();
					script.GetToken(); // ,
					aEndCondItem[ nEndCondItemNum ].m_nItemNum = script.GetNumber();
					script.GetToken(); // , or )
					if( script.Token ==  "," )
					{
						aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_dwGoalIndex = ++dwGoalIndex;
						aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_fGoalPositionX = script.GetFloat();
						script.GetToken(); // ,
						aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_fGoalPositionZ = script.GetFloat();
						script.GetToken(); // ,
						script.GetToken_NoDef(); // szGoalTextID
						aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
						script.GetToken(); // , or )
						if( script.Token == "," )
							aEndCondItem[ nEndCondItemNum ].m_ItemGoalData.m_nOwnMonster = script.GetNumber();	// 아이템 소유 몬스터 Index
					}

					if( ++nEndCondItemNum >= MAX_QUESTCONDITEM )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ END COND ITEM COUNT OVERFLOW. Max(%d) ]" ), MAX_QUESTCONDITEM );
						return FALSE;
					}
				}
				else if( script.Token == "SetEndCondKillNPC" )
				{
					script.GetToken(); // (
					int nIdx = script.GetNumber();
					if( nIdx < 0 || nIdx >= MAX_QUEST_KILL_NPC_COUNT )
					{
						nIdx = 0;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetEndCondKillNPC에서 Idx 값 범위 초과:[%d], MAX:[%d]" )
							, lpszFileName, script.GetLineNum(), nIdx, MAX_QUEST_KILL_NPC_COUNT ); 
					}
					script.GetToken(); // ,
					propQuest.m_nEndCondKillNPCIdx[nIdx] = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nEndCondKillNPCNum[nIdx] = script.GetNumber();
					script.GetToken(); // , or )
					if( script.Token ==  "," )
					{
						propQuest.m_KillNPCGoalData[ nIdx ].m_dwGoalIndex = ++dwGoalIndex;
						propQuest.m_KillNPCGoalData[ nIdx ].m_fGoalPositionX = script.GetFloat();
						script.GetToken(); // ,
						propQuest.m_KillNPCGoalData[ nIdx ].m_fGoalPositionZ = script.GetFloat();
						script.GetToken(); // ,
						script.GetToken_NoDef(); // szGoalTextID
						propQuest.m_KillNPCGoalData[ nIdx ].m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
						propQuest.m_KillNPCGoalData[ nIdx ].m_nOwnMonster = propQuest.m_nEndCondKillNPCIdx[nIdx];	
					}
				}
				else if( script.Token == "SetEndCondPatrolZone" )
				{
					script.GetToken(); // (
					propQuest.m_dwEndCondPatrolWorld = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_rectEndCondPatrol.left = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_rectEndCondPatrol.top = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_rectEndCondPatrol.right = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_rectEndCondPatrol.bottom = script.GetNumber();
					propQuest.m_rectEndCondPatrol.NormalizeRect();
					script.GetToken(); // , or )
					if( script.Token ==  "," )
					{
						script.GetToken_NoDef();
						propQuest.m_dwPatrolDestinationID = atoi( script.Token.Right( 4 ) );
						script.GetToken(); // ,
						propQuest.m_PatrolWorldGoalData.m_dwGoalIndex = ++dwGoalIndex;
						propQuest.m_PatrolWorldGoalData.m_fGoalPositionX = script.GetFloat();
						script.GetToken(); // ,
						propQuest.m_PatrolWorldGoalData.m_fGoalPositionZ = script.GetFloat();
						script.GetToken(); // ,
						script.GetToken_NoDef(); // szGoalTextID
						propQuest.m_PatrolWorldGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
					}
				}
				else if( script.Token == "SetBeginCondCharacter" )		//gmpbigsun(20110413) : added 
				{
					script.GetToken(); // (
					script.GetToken(); // "key"	-> 안쓰임, 위에 SetCharacter에서 이미 저장함.
					script.GetToken(); // , or )
					if( script.Token ==  "," )
					{
						propQuest.m_kBeginNpcData.m_dwGoalWorldID	= script.GetNumber();
						if( propQuest.m_kBeginNpcData.m_dwGoalWorldID < WI_WORLD_NONE || propQuest.m_kBeginNpcData.m_dwGoalWorldID > WI_WORLD_GUILDWAR1TO1_L )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "%s QuestID: %d(%s) SetBeginCondCharacter WorldID Error" ), lpszFileName, propQuest.m_wId, propQuest.m_szTitle ); 
						}
						script.GetToken(); // ,
						propQuest.m_kBeginNpcData.m_fGoalPositionX	= script.GetFloat();
						script.GetToken(); // ,
						propQuest.m_kBeginNpcData.m_fGoalPositionZ	= script.GetFloat();
						script.GetToken(); // ')'
					}
				}
				else if( script.Token == "SetEndCondCharacter" )
				{
					script.GetToken(); // (
					script.GetToken(); // "key"
					// 스트링이 없으면 szLinkChar에 세팅된 것으로 대체한다. 즉 자기 자신이다.
					if( strlen( script.token ) == 0 )
						FLStrcpy( propQuest.m_szEndCondCharacter, _countof( propQuest.m_szEndCondCharacter ), szLinkChar[ 0 ] );
					else
						FLStrcpy( propQuest.m_szEndCondCharacter, _countof( propQuest.m_szEndCondCharacter ), script.token );
					LPCHARACTER lpCharacter = GetCharacter( propQuest.m_szEndCondCharacter );
					if( lpCharacter ) 
					{
						lpCharacter->m_awDstQuest.Add( static_cast< WORD >( nQuest ) );
						lpCharacter->m_anDstQuestItem.Add( 0 );
					}
					bEndCondCharacter = TRUE;
					script.GetToken(); // , or )
					if( script.Token ==  "," )
					{
						propQuest.m_MeetCharacterGoalData.m_dwGoalIndex = ++dwGoalIndex;
						propQuest.m_kBeginNpcData.m_dwGoalWorldID	= script.GetNumber();
						script.GetToken(); // ,

						if( propQuest.m_kBeginNpcData.m_dwGoalWorldID < WI_WORLD_NONE || propQuest.m_kBeginNpcData.m_dwGoalWorldID > WI_WORLD_GUILDWAR1TO1_L )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "%s QuestID: %d(%s) SetEndCondCharacter WorldID Error" ), lpszFileName, propQuest.m_wId, propQuest.m_szTitle ); 
						}
						propQuest.m_MeetCharacterGoalData.m_fGoalPositionX = script.GetFloat();
						script.GetToken(); // ,
						propQuest.m_MeetCharacterGoalData.m_fGoalPositionZ = script.GetFloat();
						script.GetToken(); // ,
						script.GetToken_NoDef(); // szGoalTextID
						propQuest.m_MeetCharacterGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
					}
				}
				else if( script.Token == "SetEndCondMultiCharacter" )
				{
					script.GetToken(); // (
					szEndCondCharNum = 0;
					propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
					ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
					do
					{
						script.GetToken(); // key
						FLStrcpy( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ], 64, script.token );

						script.GetToken(); // ,
						int nItem = script.GetNumber();

						LPCHARACTER lpCharacter = GetCharacter( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ] );
						if( lpCharacter ) 
						{
							lpCharacter->m_awDstQuest.Add( static_cast< WORD >( nQuest ) );
							lpCharacter->m_anDstQuestItem.Add( nItem );
						}
						else
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) : %s가 존재하지 않음" ), lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
						}
						szEndCondCharNum++;
						script.GetToken(); // , or )
					} while( *script.token != ')' && szEndCondCharNum < 10 );
					bEndCondCharacter = TRUE;
				}
				else if( script.Token == "SetEndCondDialog" )
				{
					script.GetToken(); // (
					script.GetToken(); 
					FLStrcpy( propQuest.m_szEndCondDlgCharKey, _countof( propQuest.m_szEndCondDlgCharKey ), script.token );
					script.GetToken(); // ,
					script.GetToken(); 
					FLStrcpy( propQuest.m_szEndCondDlgAddKey, _countof( propQuest.m_szEndCondDlgAddKey ), script.token );
					script.GetToken(); // , or )
					if( script.Token ==  "," )
					{
						propQuest.m_DialogCharacterGoalData.m_dwGoalIndex = ++dwGoalIndex;
						propQuest.m_DialogCharacterGoalData.m_fGoalPositionX = script.GetFloat();
						script.GetToken(); // ,
						propQuest.m_DialogCharacterGoalData.m_fGoalPositionZ = script.GetFloat();
						script.GetToken(); // ,
						script.GetToken_NoDef(); // szGoalTextID
						propQuest.m_DialogCharacterGoalData.m_dwGoalTextID = atoi( script.Token.Right( 4 ) );
					}
				}
				//sun: 9, 9-10차 펫
				else if( script.Token == "SetEndCondPetLevel" )
				{	// SetEndCondPetLevel( 0 );
					script.GetToken();	// (
					propQuest.m_nEndCondPetLevel	= script.GetNumber();
				}
				else if( script.Token == "SetEndCondPetExp" )
				{
					// SetEndCondPetExp( 100 );
					script.GetToken();	// (
					propQuest.m_nEndCondPetExp	= script.GetNumber();
				}
				else if( script.Token == "SetEndCondDisguise" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondDisguiseMoverIndex = script.GetNumber();
				}
				else if( script.Token == "SetParam" )
				{
					script.GetToken(); // (
					int nIdx = script.GetNumber();
					if( nIdx < 0 || nIdx > 3 )
					{
						nIdx = 0;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetParam Idx 값 범위 초과(0~3)" ), lpszFileName, script.GetLineNum() ); 
					}
					script.GetToken(); // ,
					propQuest.m_nParam[ nIdx ] = script.GetNumber();
				}
				else if( script.Token == "SetEndCondTSP" )
				{
					script.GetToken(); // (
					propQuest.m_nEndCondTSP  = script.GetNumber();
				}


#ifdef __GUILD_HOUSE_MIDDLE

				else if( script.Token == "SetEndCondGuildWarWin" )
				{
					script.GetToken();	// (
					propQuest.m_nEndCondGuildWarWin	= script.GetNumber();
				}

#endif // __GUILD_HOUSE_MIDDLE
				////////////////////////// 대화 - 보상 
				else if( script.Token == "SetDlgRewardItem" )
				{
					script.GetToken(); // (
					int nIdx = script.GetNumber();
					if( nIdx < 0 || nIdx > 3 )
					{
						nIdx = 0;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetDlgRewardItem Idx 값 범위 초과(0~3)" ), lpszFileName, script.GetLineNum() ); 
					}
					script.GetToken(); // ,
					propQuest.m_nDlgRewardItemIdx[ nIdx ] = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nDlgRewardItemNum[ nIdx ] = script.GetNumber();
				}
				else if( script.Token == "SetEndRewardItem" )
				{
					script.GetToken(); // ( 

					int nSex = script.GetNumber();
					script.GetToken(); // ,

					int nType = script.GetNumber();
					script.GetToken(); // ,
					int nJobOrItem = script.GetNumber();
					if( nType == 0 && nJobOrItem >= MAX_JOB )	// Job 검사인데 왜 MAX_QUESTCONDITEM 으로 검사를 했을까..
					{
						TCHAR szErrorMsg[128] = { 0, };
						FLSPrintf( szErrorMsg, _countof( szErrorMsg ), "[FileName:%s][Line:%d] Job Invalid:[%d]", lpszFileName, script.GetLineNum(), nJobOrItem );
						AfxMessageBox( szErrorMsg, MB_OK );
					}
					else if( nType == 1 )
					{
						PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nJobOrItem );
						if( pItemProp == NULL )
						{
							TCHAR szErrorMsg[128] = { 0, };
							FLSPrintf( szErrorMsg, _countof( szErrorMsg ), "[FileName:%s][Line:%d] ItemId Invalid:[%d]", lpszFileName, script.GetLineNum(), nJobOrItem );
							AfxMessageBox( szErrorMsg, MB_OK );
						}
					}
					script.GetToken(); // ,

					aEndRewardItem[ nEndRewardItemNum ].m_nSex = static_cast< char >( nSex );
					aEndRewardItem[ nEndRewardItemNum ].m_nType = static_cast< char >( nType );
					aEndRewardItem[ nEndRewardItemNum ].m_nJobOrItem =  nJobOrItem;
					aEndRewardItem[ nEndRewardItemNum ].m_nItemIdx = script.GetNumber();
					script.GetToken(); // ,
					aEndRewardItem[ nEndRewardItemNum ].m_nItemNum = script.GetNumber();
					//sun: 13, __CHIPI_QUESTITEM_FLAG
					script.GetToken();
					if( script.Token == "," )	// , (귀속설정)
						aEndRewardItem[ nEndRewardItemNum ].m_byFlag = static_cast<BYTE>( script.GetNumber() );

					if( ++nEndRewardItemNum >= MAX_QUESTCONDITEM )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ END REWARD ITEM COUNT OVERFLOW. Max(%d) ]" ), MAX_QUESTCONDITEM );
						return FALSE;
					}
				}
				else if( script.Token == "SetEndRewardItemWithAbilityOption" )
				{
					script.GetToken();	// ( 
					int nSex	= script.GetNumber();
					script.GetToken();	// ,
					int nType	= script.GetNumber();
					script.GetToken();	// ,
					int nJobOrItem	= script.GetNumber();
					if( nType == 0 && nJobOrItem >= MAX_JOB )	// Job 검사인데 왜 MAX_QUESTCONDITEM 으로 검사를 했을까..
					{
						TCHAR szErrorMsg[128] = { 0, };
						FLSPrintf( szErrorMsg, _countof( szErrorMsg ), "%s(%d) Job Invalid:[%d]", lpszFileName, script.GetLineNum(), nJobOrItem );
						AfxMessageBox( szErrorMsg, MB_OK );
					}
					else if( nType == 1 )
					{
						PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( nJobOrItem );
						if( pItemProp == NULL )
						{
							TCHAR szErrorMsg[128] = { 0, };
							FLSPrintf( szErrorMsg, _countof( szErrorMsg ), "%s(%d) ItemId Invalid:[%d]", lpszFileName, script.GetLineNum(), nJobOrItem );
							AfxMessageBox( szErrorMsg, MB_OK );
						}
					}
					script.GetToken();	// ,
					aEndRewardItem[nEndRewardItemNum].m_nSex	= static_cast< char >( nSex );
					aEndRewardItem[nEndRewardItemNum].m_nType	= static_cast< char >( nType );
					aEndRewardItem[nEndRewardItemNum].m_nJobOrItem	=  nJobOrItem;
					aEndRewardItem[nEndRewardItemNum].m_nItemIdx	= script.GetNumber();
					script.GetToken();	// ,
					aEndRewardItem[nEndRewardItemNum].m_nItemNum	= script.GetNumber();
					script.GetToken();	// ,
					aEndRewardItem[nEndRewardItemNum].m_nAbilityOption	= script.GetNumber();
					//sun: 13, __CHIPI_QUESTITEM_FLAG
					script.GetToken();
					if( script.Token == "," )	// , (귀속설정)
						aEndRewardItem[ nEndRewardItemNum ].m_byFlag = static_cast<BYTE>( script.GetNumber() );

					nEndRewardItemNum++;
				}
				else if( script.Token == "SetEndRewardGold" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRewardGoldMin = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nEndRewardGoldMax = script.GetNumber();
				}
#ifdef DAILY_QUEST
				else if( script.Token == "SetCheckBuff" )
				{
					script.GetToken(); // (
					propQuest.m_nCheckBuffIndex = script.GetNumber();
				}
				else if( script.Token == "SetRewardBuff" )
				{
					script.GetToken(); // (
					propQuest.m_nRewardBuffIndex = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nRewardBuffTime = script.GetNumber();
				}
#endif // DAILY_QUEST
				//sun: 9, 9-10차 펫
				else if( script.Token == "SetEndRewardPetLevelup" )
				{
					propQuest.m_bEndRewardPetLevelup	= TRUE;
				}
				else if( script.Token == "SetEndRewardExp" )
				{
					script.GetToken(); // (
					propQuest.m_n64EndRewardExpMin = script.GetInt64();
					script.GetToken(); // ,
					propQuest.m_n64EndRewardExpMax = script.GetInt64();
				}
				else if( script.Token == "SetEndRewardSkillPoint" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRewardSkillPoint = script.GetNumber();
				}
				//sun: 8, // __S8_PK
				else if( script.Token == "SetEndRewardPKValue" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRewardPKValueMin = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nEndRewardPKValueMax = script.GetNumber();
				}
				//sun: 9차 추가분
				else if( script.Token == "SetEndRewardTeleport" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRewardTeleport = script.GetNumber();
					script.GetToken(); // ,
					propQuest.m_nEndRewardTeleportPos.x = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_nEndRewardTeleportPos.y = script.GetFloat();
					script.GetToken(); // ,
					propQuest.m_nEndRewardTeleportPos.z = script.GetFloat();
				}
				else if( script.Token == "SetEndRewardHide" )
				{
					script.GetToken(); // (
					propQuest.m_bEndRewardHide = ( script.GetNumber() != 0 );
				}
				else if( script.Token == "SetEndRewardSkillBuff" )
				{
					script.GetToken(); // (
					const int Nth = script.GetNumber();
					if( Nth < 0 || Nth >= MAX_QUEST_REWARD_BUFF )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID REWARD SKILL BUFF INDEX. INDEX:(%d), MAX:(%d) ]" )
							, Nth, MAX_QUEST_REWARD_BUFF );
						FLASSERT( 0 );
						return FALSE;
					}
					script.GetToken(); // ,
					propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillID		= script.GetNumber();

					SkillProp* pSkillProp		= GetSkillProp( propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillID );
					if( pSkillProp == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID REWARD SKILL ID. SKILL_ID:(%d) ]" )
							, propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillID );
						FLASSERT( 0 );
						return FALSE;
					}

					script.GetToken(); // ,
					propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillLevel		= script.GetNumber();

					AddSkillProp* pAddSkillProp	= GetAddSkillProp( pSkillProp->dwSubDefine, propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillLevel );
					if( pAddSkillProp == NULL )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID REWARD SKILL LEVEL. SKILL_ID:(%d), LEVEL(%d) ]" )
							, propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillID, propQuest.m_kEndRewardSkillBuff[ Nth ].dwSkillLevel );
						FLASSERT( 0 );
						return FALSE;
					}
				}
				// 종료 퀘스트 제거 
				else if( script.Token == "SetEndRemoveQuest" )
				{
					script.GetToken(); // (
					int nRemoveQuestIdx = 0;
					do
					{
						propQuest.m_anEndRemoveQuest[ nRemoveQuestIdx++ ] = script.GetNumber();;
						script.GetToken(); // , or )
					} while( *script.token != ')' && nRemoveQuestIdx < MAX_QUESTREMOVE );
				}
				// 종료 아이템 제거 
				else if( script.Token == "SetEndRemoveItem" )
				{
					script.GetToken(); // (
					int nIdx = script.GetNumber();
					if( nIdx < 0 || nIdx > 7 )
					{
						nIdx = 0;
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetEndRemoveItem Idx 값 범위 초과(0~3)" ), lpszFileName, script.GetLineNum() ); 
					}
					script.GetToken(); // ,
					propQuest.m_dwEndRemoveItemIdx[ nIdx ] = static_cast< DWORD >( script.GetNumber() );
					script.GetToken(); // ,
					propQuest.m_nEndRemoveItemNum[ nIdx ] = script.GetNumber();
				}
				else if( script.Token == "SetEndRemoveGold" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRemoveGold = script.GetNumber();
				}
				else if( script.Token == "SetRepeat" )
				{
					script.GetToken(); // (
					propQuest.m_bRepeat = ( script.GetNumber() != 0 );
				}
				else if( script.Token == "QuestItem" )
				{
					QUESTITEM qi;
					script.GetToken();	// (
					DWORD dwMoverIdx = script.GetNumber();
					qi.dwQuest	= nQuest;//script.GetNumber();
					qi.dwState	= 0;//script.GetNumber();
					script.GetToken();	// ,
					qi.dwIndex	= script.GetNumber();
					FLASSERT( qi.dwIndex );
					script.GetToken();	// ,
					qi.dwProbability	= script.GetNumber();	//
					script.GetToken();	// ,
					qi.dwNumber	= script.GetNumber();	//
					script.GetToken();	// )


					if( _GetContentState( CT_DROP_ITEM_REFORM ) != CS_VER1 )
					{
						MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
						pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
					}
				}
				else if( script.Token == "SetEndRewardTSP" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRewardTSP  = script.GetNumber();
				}
				else if( script.Token == "SetEndRemoveTSP" )
				{
					script.GetToken(); // (
					propQuest.m_nEndRemoveTSP  = script.GetNumber();
				}
				//////////////////////////
				else if( script.Token == "SetDialog" )
				{
					script.GetToken(); // (
					int nNum = script.GetNumber();
					script.GetToken(); // ,
					CString string = GetLangScript( script );
#if defined( __WORLDSERVER )
					if( nNum >= 0 && nNum < 32 )
					{
						if( propQuest.m_apQuestDialog[ nNum ] != NULL )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE QUEST DIALOG NUMBER (%s)(%s)(%d) ]" ), propQuest.m_szTitle, propQuest.m_apQuestDialog[ nNum ], nNum );
							SAFE_DELETE_ARRAY( propQuest.m_apQuestDialog[ nNum ] );
						}
						
						propQuest.m_apQuestDialog[ nNum ] = new CHAR[ string.GetLength() + 1 ];
						FLStrcpy( propQuest.m_apQuestDialog[ nNum ], string.GetLength() + 1, string );
					}
#endif // __WORLDSERVER
				}
				if( script.Token == "SetPatrolZoneName" )
				{
					script.GetToken(); // (
					//int nNum = script.GetNumber();
					//script.GetToken(); // ,
					CString string = GetLangScript( script );
					if( string.GetLength() < 64 )
						FLStrcpy( propQuest.m_szPatrolZoneName, _countof( propQuest.m_szPatrolZoneName ), string );
					else
						FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) SetPatrolZoneName 스트링 64바이트 초과" ), lpszFileName, script.GetLineNum() ); 
				}
				else if( script.Token == "SetHeadQuest" )
				{
					script.GetToken(); // (
					propQuest.m_nHeadQuest = static_cast< WORD >( script.GetNumber() );
					if( propQuest.m_nHeadQuest >= 1800 && propQuest.m_nHeadQuest <= 1999 )
					{
						switch( propQuest.m_nHeadQuest )
						{
						case 1992:
							{
								propQuest.m_nHeadQuest = QUEST_KIND_EVENT;
								break;
							}
						case 1993:
						case 1994:
						case 1995:
						case 1997:
						case 1998:
							{
								propQuest.m_nHeadQuest = QUEST_KIND_NORMAL;
								break;
							}
						case 1996:
							{
								propQuest.m_nHeadQuest = QUEST_KIND_SCENARIO;
								break;
							}
						case 1999:
							{
								propQuest.m_nHeadQuest = QUEST_KIND_REQUEST;
								break;
							}
						default:
							{
								propQuest.m_nHeadQuest = QUEST_KIND_SCENARIO;
							}
						}
					}
				}
				else if( script.Token == "SetQuestType" )
				{
					script.GetToken(); // (
					propQuest.m_nQuestType = static_cast< CHAR >( script.GetNumber() );
				}
				else if( script.Token == "SetRemove" )
				{
					script.GetToken(); // (
					propQuest.m_bNoRemove = !script.GetNumber();
				}
				else if( script.Token == "state" )
				{
					int nState = script.GetNumber();

					QuestState* pQuestState = new QuestState;
					propQuest.m_questState[ nState ] = pQuestState;
					pQuestState->m_szDesc[ 0 ] = 0;
					script.GetToken(); // key or anything

					int temp=0;


					while( script.Token[0] != '}' )
					{
						temp++;


						if( script.Token == "QuestItem" )
						{
							QUESTITEM qi;
							script.GetToken();	// (
							DWORD dwMoverIdx = script.GetNumber();
							qi.dwQuest	= nQuest;//script.GetNumber();
							qi.dwState	= nState;//script.GetNumber();
							script.GetToken();	// ,
							qi.dwIndex	= script.GetNumber();
							script.GetToken();	// ,
							qi.dwProbability	= script.GetNumber();	//
							script.GetToken();	// ,
							qi.dwNumber	= script.GetNumber();	//
							script.GetToken();	// )

							if( _GetContentState( CT_DROP_ITEM_REFORM ) != CS_VER1 )
							{
								MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
								pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
							}
						}
						else if( script.Token == "SetDesc" )
						{
							script.GetToken(); // (
							CString string = GetLangScript( script );
							if( string.GetLength() < 512 ) // null 때문에 255
								FLStrcpy( pQuestState->m_szDesc, _countof( pQuestState->m_szDesc ), string );
							else
								FLERROR_LOG( PROGRAM_NAME, _T( "%s(%d) LoadPropQuest의 SetDesc 길이 초과" ), lpszFileName, script.GetLineNum() ); 

						}
						else if( script.Token == "SetCond" )
						{
							script.GetToken(); // (
							CString string = GetLangScript( script );
						}
						else if( script.Token == "SetStatus" )
						{
							script.GetToken(); // (
							CString string = GetLangScript( script );
						}
						script.GetToken(); // key or anything
					}
				}
		}
		// EndCondCharacter세팅이 안됐다면 LinkChar로 대체한다.
		if( bEndCondCharacter == FALSE )
		{
			// 스트링이 없으면 szLinkChar에 세팅된 것으로 대체한다. 즉 자기 자신이다.
			if( szLinkCharNum == 1 )
			{
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ 0 ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( static_cast< WORD >( nQuest ) );
					lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( 0 ) );
				}
				FLStrcpy( propQuest.m_szEndCondCharacter, _countof( propQuest.m_szEndCondCharacter ), szLinkChar[ 0 ] );
			}
			else if( szLinkCharNum > 1 )
				{
					propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
					ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
					for( int i = 0; i < szLinkCharNum; i++ )
					{
						FLStrcpy( &propQuest.m_lpszEndCondMultiCharacter[ i * 64 ], 64, szLinkChar[ 0 ] );
						LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ i ] );
						if( lpCharacter )
						{
							lpCharacter->m_awDstQuest.Add( static_cast< WORD >( nQuest ) );
							lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( i ) );
						}
					}
				}
		}
		if( nBeginCondItemNum )
		{
			propQuest.m_paBeginCondItem = new QuestPropItem[ nBeginCondItemNum ];
			memcpy( propQuest.m_paBeginCondItem, aBeginCondItem, sizeof( QuestPropItem ) * nBeginCondItemNum );
			propQuest.m_nBeginCondItemNum = static_cast< char >( nBeginCondItemNum );
		}
		//sun: 8, // __S8_PK
		if( nBeginCondNotItemNum )
		{
			propQuest.m_paBeginCondNotItem = new QuestPropItem[ nBeginCondNotItemNum ];
			memcpy( propQuest.m_paBeginCondNotItem, aBeginCondNotItem, sizeof( QuestPropItem ) * nBeginCondNotItemNum );
			propQuest.m_nBeginCondNotItemNum = static_cast< char >( nBeginCondNotItemNum );
		}

		if( nEndCondOneItemNum )
		{
			propQuest.m_paEndCondOneItem = new QuestPropItem[ nEndCondOneItemNum ];
			memcpy( propQuest.m_paEndCondOneItem, aEndCondOneItem, sizeof( QuestPropItem ) * nEndCondOneItemNum );
			propQuest.m_nEndCondOneItemNum = static_cast< char >( nEndCondOneItemNum );
		}		

		if( nEndCondItemNum )
		{
			propQuest.m_paEndCondItem = new QuestPropItem[ nEndCondItemNum ];
			memcpy( propQuest.m_paEndCondItem, aEndCondItem, sizeof( QuestPropItem ) * nEndCondItemNum );
			propQuest.m_nEndCondItemNum = static_cast< char >( nEndCondItemNum );
		}
		if( nEndRewardItemNum )
		{
			propQuest.m_paEndRewardItem = new QuestPropItem[ nEndRewardItemNum ];
			memcpy( propQuest.m_paEndRewardItem, aEndRewardItem, sizeof( QuestPropItem ) * nEndRewardItemNum );
			propQuest.m_nEndRewardItemNum = nEndRewardItemNum;
		}

		BOOL bAdd	= TRUE;
		// 가위 바위 보 퀘스트는 대만에 한함	// 퀘스트 삭제
		if( //g_xFlyffConfig->GetMainLanguage() != LANG_TWN && 
			nQuest == QUEST_INDEX( 5015, QUEST_KAWIBAWIBO01 ) )
			bAdd	= FALSE;
		if( bAdd )
		{
			if( nQuest >= 0 )
			{
				m_aPropQuest.SetAt( nQuest, &propQuest );
			}
			else
			{
#ifdef __WORLDSERVER
				for( int nNth = 0; nNth < 32; ++nNth )
				{
					SAFE_DELETE_ARRAY( propQuest.m_apQuestDialog[ nNth ] );
				}
#endif // __WORLDSERVER
			}
		}
		else
		{
#ifdef __WORLDSERVER
			for( int nNth = 0; nNth < 32; ++nNth )
			{
				SAFE_DELETE_ARRAY( propQuest.m_apQuestDialog[ nNth ] );
			}
#endif // __WORLDSERVER			}
		}

		nQuest = script.GetNumber();  // id
	}
	if( bOptimize )
		m_aPropQuest.Optimize();

	return TRUE;
}

const DWORD	dwDefault	= (DWORD)0xFFFFFFFF;
// propSkill.txt 로딩 이후에 해야한다.
BOOL CProject::LoadPropAddSkill( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	AddSkillProp pAddSkillProp;
	int i = script.GetNumber( TRUE ); // id
	while ( script.tok != FINISHED )
	{
		if( i == 0 )			// 이거 없으니까 파일뒤에 공백있는것도 줄줄히 읽더라
			break;

		pAddSkillProp.dwID = i;
		pAddSkillProp.dwSkillPropID = script.GetNumber( TRUE );
		pAddSkillProp.dwSkillLvl = script.GetNumber( TRUE );
		pAddSkillProp.dwAbilityMin = script.GetNumber( TRUE );
		pAddSkillProp.dwAbilityMax = script.GetNumber( TRUE );

		pAddSkillProp.dwAbilityMinPVP	= script.GetNumber( TRUE );		// 대인 효과 최소
		pAddSkillProp.dwAbilityMaxPVP	= script.GetNumber( TRUE );		// 대인 효과 최대

		pAddSkillProp.dwAttackSpeed_ = script.GetNumber( TRUE );
		pAddSkillProp.dwDmgShift = script.GetNumber( TRUE );
		pAddSkillProp.dwProbability = script.GetNumber( TRUE );

		pAddSkillProp.dwProbabilityPVP	= script.GetNumber( TRUE );

		pAddSkillProp.dwTaunt_ = script.GetNumber( TRUE );
		pAddSkillProp.dwDestParam[0] = script.GetNumber( TRUE );
		pAddSkillProp.dwDestParam[1] = script.GetNumber( TRUE );
		pAddSkillProp.dwAdjParamVal[0] = script.GetNumber( TRUE );
		pAddSkillProp.dwAdjParamVal[1] = script.GetNumber( TRUE );
		pAddSkillProp.dwChgParamVal[0] = script.GetNumber( TRUE );
		pAddSkillProp.dwChgParamVal[1] = script.GetNumber( TRUE );
		pAddSkillProp.arrAddSkillDestNumData1[0] = script.GetNumber( TRUE );
		pAddSkillProp.arrAddSkillDestNumData1[1] = script.GetNumber( TRUE );
		pAddSkillProp.arrAddSkillDestNumData1[2] = script.GetNumber( TRUE );
		pAddSkillProp.dwActiveSkill = script.GetNumber( TRUE );
		pAddSkillProp.dwActiveSkillRate = script.GetNumber( TRUE );

		pAddSkillProp.dwActiveSkillRatePVP	= script.GetNumber( TRUE );

		pAddSkillProp.nReqMp = script.GetNumber( TRUE );
		pAddSkillProp.nReqFp = script.GetNumber( TRUE );

		pAddSkillProp.dwCooldown		= script.GetNumber( TRUE );
		pAddSkillProp.dwCastingTime	= script.GetNumber( TRUE );

		pAddSkillProp.dwSkillRange = script.GetNumber( TRUE );
		pAddSkillProp.dwCircleTime = script.GetNumber( TRUE );
		pAddSkillProp.dwPainTime  = script.GetNumber( TRUE );
		pAddSkillProp.dwSkillTime = script.GetNumber( TRUE );
		pAddSkillProp.nSkillCount = script.GetNumber( TRUE );
		pAddSkillProp.dwSkillExp_ = script.GetNumber( TRUE );

		pAddSkillProp.dwExp__ = script.GetNumber( TRUE );
		pAddSkillProp.dwComboSkillTime_ = script.GetNumber( TRUE );

		//////////////////////////////////////////////////////////////////////////
		if( pAddSkillProp.dwAbilityMinPVP == dwDefault )
			pAddSkillProp.dwAbilityMinPVP	= pAddSkillProp.dwAbilityMin;
		if( pAddSkillProp.dwAbilityMaxPVP == dwDefault )
			pAddSkillProp.dwAbilityMaxPVP	= pAddSkillProp.dwAbilityMax;

		if( pAddSkillProp.dwProbabilityPVP == dwDefault )
			pAddSkillProp.dwProbabilityPVP	= pAddSkillProp.dwProbability;
		if( pAddSkillProp.dwActiveSkillRatePVP == dwDefault )
			pAddSkillProp.dwActiveSkillRatePVP	= pAddSkillProp.dwActiveSkillRate;

		SkillProp* pSkillProp	= GetSkillProp( pAddSkillProp.dwSkillPropID );
		if( pSkillProp )
		{
			if( pAddSkillProp.dwCooldown == dwDefault )
			{
				pAddSkillProp.dwCooldown	= pSkillProp->dwSkillReady;

				if( pSkillProp->nVer < 9 && pAddSkillProp.dwCastingTime != dwDefault )
				{
					pAddSkillProp.dwCastingTime	/= 4;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		m_aPropAddSkill.SetAt( i, &pAddSkillProp );

		i = script.GetNumber( TRUE ); 
	}

	m_aPropAddSkill.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropMoverEx( LPCTSTR szFileName )
{
	CScript script;
//	int		nBlock = 0;

	if(script.Load(szFileName)==FALSE)
		return FALSE;
	CString string;
	int nVal = script.GetNumber(); // Mover Id

	do 
	{
		MoverProp* pProp = GetMoverProp( nVal );
		if(pProp == NULL)
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "MoverAddProp에서 존재해야될 base Property %s가 없다. %s" ), script.Token );
			continue;
		}

		// 초기화
		pProp->m_nEvasionHP        = 0;
		pProp->m_nEvasionSec       = 0;
		pProp->m_nRunawayHP        = -1;
		pProp->m_nCallHP           = -1;
		pProp->m_nCallHelperMax    = 0;
		ZeroMemory( pProp->m_nCallHelperIdx  , sizeof( pProp->m_nCallHelperIdx   ) );
		ZeroMemory( pProp->m_nCallHelperNum  , sizeof( pProp->m_nCallHelperNum   ) );
		ZeroMemory( pProp->m_bCallHelperParty, sizeof( pProp->m_bCallHelperParty ) );

		pProp->m_nChangeTargetRand = 10;
		pProp->m_dwAttackMoveDelay = 0;
		pProp->m_bPartyAttack      = 0;
		pProp->m_dwRunawayDelay    = SEC( 1 );
		pProp->m_nAttackFirstRange = 10;

		script.GetToken(); // {
		script.GetToken(); 

		//while(*script.token != '}')
		DWORD dwSubMonsterSequence = 0;
		while( *script.token != '}' )
		{
			if(script.Token == ";") 
			{ 
				script.GetToken(); 
				continue; 
			} 
			if( _strcmpi( script.Token, "AI" ) == 0 )		// AI블럭
			{
				BOOL bRet = LoadPropMoverEx_AI( szFileName, script, nVal );
				if ( bRet == FALSE )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "%s : MoverID %s(%d)의 AI{} 블럭 읽기 실패" ), szFileName, pProp->szName, nVal );
					return FALSE;
				}
			}

			if( script.Token == "AddSummonMonster" )
			{
				script.GetToken();			//"("
				//@@@@@@@@@@@@@@@@@		다 로드 한후 
				const DWORD dwMoverIndex		= script.GetNumber();
				const MoverProp * pSubProp		= GetMoverProp( dwMoverIndex );

				//검증.
				if( pProp->dwAI != AII_PARTY_AGGRO_LEADER )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID LEADER AI : NOT AII_PARTY_AGGRO_LEADER ][ MI(%u) Sub_MI(%u) ]" ), nVal, dwMoverIndex );
					RELEASE_BREAK;
				}


				if( pSubProp == NULL )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST SUB MONSTER : AddSubMonster ][ MI(%u) Sub_MI(%u) ]" ), nVal, dwMoverIndex );
					RELEASE_BREAK;
				}

				if( pSubProp->dwAI != AII_PARTY_AGGRO_SUB )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID AI ( NOT AII_PARTY_AGGRO_SUB ) : AddSubMonster ][ MI(%u) Sub_MI(%u) ]" ), nVal, dwMoverIndex );
					RELEASE_BREAK;
				}

				if( pSubProp->m_mapSummonMonsters.size() != 0 )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ SUB MONSTER HAS SUMMON MONSTER LIST : AddSubMonster ][ MI(%u) Sub_MI(%u) ]" ), nVal, dwMoverIndex );	//재귀적
					RELEASE_BREAK;
				}

				MoverProp::FLMonsterKey key;
				key.dwMoverID			= dwMoverIndex;
				key.dwSequence			= dwSubMonsterSequence;

				pProp->m_mapSummonMonsters.insert
					( MoverProp::SummonMonstersMap::value_type( key, dwMoverIndex ) );		//@ 나중에 검증 해야함.
				script.GetToken();			//")"
				script.GetToken();			//";"

				++dwSubMonsterSequence;
			}
			else if(script.Token == "m_nAttackFirstRange")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackFirstRange  = static_cast< short >( script.GetNumber() );

				if( pProp->m_nAttackFirstRange > 100 || pProp->m_nAttackFirstRange <= 0 )
					FLERROR_LOG( PROGRAM_NAME, _T( "LoadPropMoverEx::m_nAttackFirstRange 이상:%d" ), pProp->m_nAttackFirstRange );
			}
			else if(script.Token == "SetEvasion")
			{
				script.GetToken(); // skip (
				pProp->m_nEvasionHP  = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip ,
				pProp->m_nEvasionSec = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip )
			}
			else if(script.Token == "SetRunAway")
			{
				script.GetToken(); // skip (
				pProp->m_nRunawayHP = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip ,
				if( script.Token == "," )
				{
					script.GetNumber();
					script.GetToken(); // skip ,
					script.GetNumber();
					script.GetToken(); // skip )
				}
			}
			else if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				pProp->m_nCallHP = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip ,
				pProp->m_nCallHelperIdx[ pProp->m_nCallHelperMax ] = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip ,
				pProp->m_nCallHelperNum[ pProp->m_nCallHelperMax ] = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip ,
				pProp->m_bCallHelperParty[ pProp->m_nCallHelperMax ] = static_cast< short >( script.GetNumber() );
				script.GetToken(); // skip )
				pProp->m_nCallHelperMax++;
				if( pProp->m_nCallHelperMax > 5 )
				{
					char pszText[100] = { 0, };
					FLSPrintf( pszText, _countof( pszText ), "ID = %d: helper too many", pProp->dwID );
					AfxMessageBox( pszText );
				}
			}
			else if(script.Token == "m_nAttackItemNear")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemNear  = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nAttackItemFar")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemFar  = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nAttackItem1")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem1  = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nAttackItem2")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem2  = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nAttackItem3")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem3  = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nAttackItem4")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem4  = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nAttackItemSec")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemSec = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nMagicReflection")
			{
				script.GetToken(); // skip =
				pProp->m_nMagicReflection = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_nImmortality")
			{
				script.GetToken(); // skip =
				pProp->m_nImmortality = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_bBlow")
			{
				script.GetToken(); // skip =
				pProp->m_bBlow = script.GetNumber();
			}
			else if(script.Token == "m_nChangeTargetRand")
			{
				script.GetToken(); // skip =
				pProp->m_nChangeTargetRand = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_dwAttackMoveDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwAttackMoveDelay = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "m_dwRunawayDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwRunawayDelay = static_cast< short >( script.GetNumber() );
			}
			else if(script.Token == "randomItem")
				InterpretRandomItem( &pProp->m_randomItem, script );
			else if( script.Token == "Maxitem" )
			{
				script.GetToken();	// =
				pProp->m_DropItemGenerator.m_dwMax	= script.GetNumber();
			}
			else if( script.Token == "DropItem" )
			{
				DROPITEM di = { static_cast< DROPTYPE >( 0 ) };
				di.dtType = DROPTYPE_NORMAL;
				script.GetToken();	// (
				di.dwIndex	= script.GetNumber();	// specific item index
				if( di.dwIndex == 0 )
					FLERROR_LOG( PROGRAM_NAME, _T( "%s : %s가 defineItem.h에 정의되지 않았음" ), szFileName, script.token );
				FLASSERT( di.dwIndex != 0 );
				script.GetToken();	// ,
				di.dwProbability	= script.GetNumber();	// probability
				script.GetToken();	// ,
				di.dwLevel	= script.GetNumber();	// level
				script.GetToken();	// ,
				di.dwNumber	= script.GetNumber();	// number

				di.dwNumber2 = 0;
				script.GetToken();
				if( script.Token == ',' )
				{
					di.dwNumber2 = script.GetNumber();	// number min
					script.GetToken();	// )
				}
				
#ifdef __WORLDSERVER
			//	if( _GetContentState( CT_DROP_ITEM_REFORM ) != CS_VER1 )	// 스크립트에서 Drop아이템이 셋팅될 수 있도록 수정
				{
					pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
				}
#endif
//				di.dwNumber2 = 0;
			}
			else if( script.Token == "DropKind" )
			{
				DROPKIND dropKind;
				script.GetToken();	// (
				dropKind.dwIK3	= script.GetNumber();	//	dwIK3
				script.GetToken();	// ,
				int a = script.GetNumber();
				UNREFERENCED_PARAMETER( a );
				script.GetToken();	// ,
				int b = script.GetNumber();
				UNREFERENCED_PARAMETER( b );
				dropKind.nMinUniq = (short)( pProp->dwLevel - 5 );		// 자기레벨보다 2~5레벨 낮은 템이 드랍되도록.
				dropKind.nMaxUniq = (short)( pProp->dwLevel - 2 );
				if( dropKind.nMinUniq < 1 )		
					dropKind.nMinUniq = 1;
				if( dropKind.nMaxUniq < 1 )
					dropKind.nMaxUniq = 1;
				script.GetToken();	// )
#ifdef __WORLDSERVER
				if( _GetContentState( CT_DROP_ITEM_REFORM ) != CS_VER1 )
				{
					pProp->m_DropKindGenerator.AddTail( dropKind );	// copy
				}
#endif

				
			}
			else if( script.Token == "DropGold" )
			{
				DROPITEM di = { static_cast< DROPTYPE >( 0 ) };
				di.dtType = DROPTYPE_SEED;
				di.dwProbability = 0xFFFFFFFF;	// 무조건 나오게.
				script.GetToken();	// (
				di.dwNumber = script.GetNumber();	// gold min
				script.GetToken();	// ,
				di.dwNumber2 = script.GetNumber();	// gold max
				script.GetToken();	// )
#ifdef __WORLDSERVER
				if( _GetContentState( CT_DROP_ITEM_REFORM ) != CS_VER1 )
				{
					pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
				}
#endif //__WORLDSERVER			
			}
			else if( script.Token == "Transform" )
			{
				script.GetToken(); // (
				pProp->m_MonsterTransform.fHPRate = script.GetFloat();
				script.GetToken(); // ,
				pProp->m_MonsterTransform.dwMonsterId = script.GetNumber();
				script.GetToken(); // )
			}
#ifdef __WORLDSERVER			
			else if( _GetContentState( CT_DROP_ITEM_REFORM ) == CS_VER1 )
			{ 
				if( script.Token == "SetDropOwnership_Off" )
				{
					pProp->m_kDropItemAttr.bDropOwnerShipOff	= TRUE;
				}
				else if( script.Token == "SetLevelDropPanalty_Off" )
				{
					pProp->m_kDropItemAttr.bLevelDropPanaltyOff = TRUE;
				}
			}
#endif //__WORLDSERVER			
			//else 
			//{
			//	FLWARNING_LOG( PROGRAM_NAME, _T( "[ INVALID FUNCTION(%s) ]" ), script.Token );
			//}

			script.GetToken();
		} 

		nVal = script.GetNumber(); // Mover Id
	} while(script.tok != FINISHED);

	return TRUE;
}

BOOL CProject::LoadCharacter( LPCTSTR szFileName )
{
	CScript script;

	if(script.Load(szFileName)==FALSE)
		return FALSE;

	CString strName;
	LPCHARACTER lpCharacter;
	script.GetToken(); // Mover name
	strName = script.Token;
	while( script.tok != FINISHED)
	{
		lpCharacter = new CHARACTER;
		//	lpCharacter->Clear();
		// 맵에 추가되는 것은 lowercase다. 그런데, 유일한 key 스트링이 lowercase로 맵에 추가되면
		// 원본 자체가 손상되는 결과구 그 리스트를 뽑아낼 때도 모두 lowercase가 되므로 원본을 m_szKey에
		// 보관해두는 것이다.
		FLStrcpy( lpCharacter->m_szKey, _countof( lpCharacter->m_szKey ), strName );
		strName.MakeLower();

		lpCharacter->m_nVenderType = VENDOR_TYPE_PENYA;		//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)

		lpCharacter->bOutput = TRUE;

#ifdef _DEBUG
		if( m_mapCharacter.Lookup( strName, (void*&)lpCharacter ) )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "%s에서 동일한 %s 발견" ), szFileName, strName );
		}
#endif
		m_mapCharacter.SetAt( strName, lpCharacter);
		script.GetToken(); // {

		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{ 
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else if( script.Token == "}" )			
				nBlock--;
			else if(script.Token == "randomItem")
			{
				InterpretRandomItem(&lpCharacter->m_randomItem,script);
			}
			else if(script.Token == "SetEquip")
			{
				lpCharacter->m_nEquipNum = 0;
				ZeroMemory( lpCharacter->m_adwEquip, sizeof( lpCharacter->m_adwEquip ) );
				script.GetToken(); // (
				while( *script.token != ')' )
				{
					DWORD dwEquip = script.GetNumber();
					if( lpCharacter->m_nEquipNum < MAX_HUMAN_PARTS )
						lpCharacter->m_adwEquip[ lpCharacter->m_nEquipNum++ ] = dwEquip;
					script.GetToken(); // ,
				}
			}
			else if(script.Token == "m_szName")
			{
				script.GetToken(); // =
				script.GetToken(); // name
				lpCharacter->m_strName = script.Token;

			}
			else if(script.Token == "SetName")
			{ 
				script.GetToken(); // (
				lpCharacter->m_strName = GetLangScript( script );
				if( strlen( lpCharacter->m_strName ) == 0 )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "Error SetName m_szName Not Data") );
				}				
			}
			else if( script.Token == "SetFigure" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMoverIdx = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairMesh = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairColor = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHeadMesh = script.GetNumber();
			}

			else if( script.Token == "SetMusic" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMusicId = script.GetNumber();
			}			
			else if(script.Token == "m_nStructure")
			{
				script.GetToken();
				lpCharacter->m_nStructure = script.GetNumber();
			}
			else if(script.Token == "m_szChar") // "SetImage"랑 같다 차후 삭제 예정 
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				//	100805	CSTRING GETBUFFER
				FLStrcpy( lpCharacter->m_szChar, _countof( lpCharacter->m_szChar ), filename.GetString() );
			}
			else if(script.Token == "m_szDialog")
			{
				script.GetToken();
				script.GetToken();
				FLStrcpy( lpCharacter->m_szDialog, _countof( lpCharacter->m_szDialog ), script.Token );
			}
			else if(script.Token == "m_szDlgQuest")
			{
				script.GetToken();
				script.GetToken();
				FLStrcpy( lpCharacter->m_szDlgQuest, _countof( lpCharacter->m_szDlgQuest ), script.Token );
			}
			else if(script.Token == "SetImage" )
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				//	100805	CSTRING GETBUFFER
				FLStrcpy( lpCharacter->m_szChar, _countof( lpCharacter->m_szChar ), filename.GetString() );
			}
			else if( script.Token == "AddMenuLang" )
			{
				script.GetToken(); // (
				int nLang	= script.GetNumber();
				script.GetToken(); // ,
				int nMMI = script.GetNumber(); 
				if( nMMI < 0 || nMMI >= MAX_MOVER_MENU )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ MENU INDEX INVALID. Setting(%d), Max(%d) ]" ), nMMI, MAX_MOVER_MENU );
					return FALSE;
				}
				script.GetToken(); // ) 
				if( g_xFlyffConfig->GetMainLanguage() == nLang )
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else if(script.Token == "AddMenu" )
			{
				script.GetToken(); // (
				int nMMI = script.GetNumber(); 
				if( nMMI < 0 || nMMI >= MAX_MOVER_MENU )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ MENU INDEX INVALID. Setting(%d), Max(%d) ]" ), nMMI, MAX_MOVER_MENU );
					return FALSE;
				}
				script.GetToken(); // ) 

				// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.
				if( ( g_xFlyffConfig->GetMainLanguage() != LANG_TWN && g_xFlyffConfig->GetMainLanguage() != LANG_HK ) || ( nMMI != MMI_BEAUTYSHOP && nMMI != MMI_BEAUTYSHOP_SKIN ) )
					//				if( g_xFlyffConfig->GetMainLanguage() != LANG_TWN || nMMI != MMI_BEAUTYSHOP )		// sun: 8차 헤어샵관련 수정 및 성형수술 기능 추가.( < 8 )
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else if( script.Token == CString( "SetForceMenu" ) )
			{
				//강제로 메뉴 보이기.
				lpCharacter->m_bForcedMenu = TRUE;

				script.GetToken();		//(
				script.GetToken();		//)
			}
			else if( script.Token == "AddMenu_Teleport" )
			{
				script.GetToken(); // (
				
				const int nMMI = script.GetNumber(); 
				if( nMMI < 0 || nMMI >= MAX_MOVER_MENU )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "[ MENU INDEX INVALID. Setting(%d), Max(%d) ]" ), nMMI, MAX_MOVER_MENU );
					return FALSE;
				}

				//////////////////////////////////////////////////////////////////////////
				script.GetToken(); // ,

				T_WORLD_POSITION	kPos;
				kPos.dwWorldID		= script.GetNumber();
				script.GetToken(); // ,
				kPos.tPos.x			= script.GetFloat();
				script.GetToken(); // ,
				kPos.tPos.z			= script.GetFloat();
				//////////////////////////////////////////////////////////////////////////

				script.GetToken(); // ) 

				if( ( g_xFlyffConfig->GetMainLanguage() != LANG_TWN && g_xFlyffConfig->GetMainLanguage() != LANG_HK ) || ( nMMI != MMI_BEAUTYSHOP && nMMI != MMI_BEAUTYSHOP_SKIN ) )
				{
					lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
					const bool bResult = lpCharacter->m_mapTeleportMenu.insert( std::map< DWORD, T_WORLD_POSITION >::value_type( nMMI, kPos ) ).second;
					if( bResult == false )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "[ EXIST MENU INDEX. NPC(%s), MMI_ID(%d) ]" ), strName, nMMI );
						return FALSE;
					}
				}
			}
			else if(script.Token == "AddVenderSlot" ) // "AddVendorSlot"랑 같다 삭제 예정 
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				script.GetToken();
				lpCharacter->m_venderSlot[ nSlot ] = script.token;
				script.GetToken(); // 
			}
			else if(script.Token == "AddVendorSlot" )
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				lpCharacter->m_venderSlot[ nSlot ] = GetLangScript( script );
			}
			//sun: 10, __RULE_0615
			// 국가별 판매 아이템 등록
			else if( script.Token == "AddVendorSlotLang" )
			{
				script.GetToken(); // (
				int nLang	= script.GetNumber();	// lang
				script.GetToken();	//,
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				CString strSlot	= GetLangScript( script );
				if( nLang == g_xFlyffConfig->GetMainLanguage() )
					lpCharacter->m_venderSlot[ nSlot ] = strSlot;
			}
			else if( script.Token == "AddVendorItemLang" )
			{
				script.GetToken();	//(
				int nLang	= script.GetNumber();
				script.GetToken();	//,
				int nSlot	= script.GetNumber(); script.GetToken(); // 
				int nItemKind3	= script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
				int nUniqueMin	= script.GetNumber(); script.GetToken(); // 
				int nUniqueMax	= script.GetNumber(); script.GetToken(); // 
				int nTotalNum	= script.GetNumber(); script.GetToken(); // 
				if( nLang == g_xFlyffConfig->GetMainLanguage() )
				{
					LPVENDOR_ITEM pVendorItem	= new VENDOR_ITEM;
					pVendorItem->m_nItemkind3	= nItemKind3;
					pVendorItem->m_nItemJob		= nItemJob;
					pVendorItem->m_nUniqueMin	= nUniqueMin;
					pVendorItem->m_nUniqueMax	= nUniqueMax;
					pVendorItem->m_nTotalNum	= nTotalNum;
					lpCharacter->m_venderItemAry[ nSlot ].Add( pVendorItem );
				}
			}

			else if( script.Token == "AddVenderItem" || script.Token == "AddVendorItem")
			{
				script.GetToken(); // (
				int nSlot      = script.GetNumber(); script.GetToken(); // 
				int nItemKind3  = script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
				int nUniqueMin = script.GetNumber(); script.GetToken(); // 
				int nUniqueMax = script.GetNumber(); script.GetToken(); // 
				int nTotalNum  = script.GetNumber(); script.GetToken(); // 

				LPVENDOR_ITEM pVendorItem = new VENDOR_ITEM;
				pVendorItem->m_nItemkind3  = nItemKind3;
				pVendorItem->m_nItemJob	= nItemJob;
				pVendorItem->m_nUniqueMin = nUniqueMin;
				pVendorItem->m_nUniqueMax = nUniqueMax;
				pVendorItem->m_nTotalNum  = nTotalNum;
				lpCharacter->m_venderItemAry[ nSlot ].Add( pVendorItem );
			}
			//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
			else if( script.Token == "AddVenderItem2" || script.Token == "AddVendorItem2")
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				DWORD dwId = script.GetNumber(); script.GetToken(); //

				LPVENDOR_ITEM pVendorItem = new VENDOR_ITEM;
				pVendorItem->m_dwItemId = dwId;
				lpCharacter->m_venderItemAry2[ nSlot ].Add( pVendorItem );
			}
			else if( script.Token == "SetVenderType" )
			{
				script.GetToken();
				int nVenderType = script.GetNumber();
				if( nVenderType != VENDOR_TYPE_PENYA && nVenderType != VENDOR_TYPE_CHIP )
				{
					nVenderType = VENDOR_TYPE_PENYA;
				}
				lpCharacter->m_nVenderType = nVenderType;
				script.GetToken();
			}
			else if( script.Token == "SetIgnorePurchaseTax" )
			{
				script.GetToken();	// (
				script.GetToken();	// )
				lpCharacter->m_bIgnorePurchaseTax = true;
			}
			else if( script.Token == "SetBuffSkill" )
			{
				NPC_BUFF_SKILL NPCBuffSkill;
				script.GetToken(); // (
				NPCBuffSkill.dwSkillID = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.dwSkillLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.nMinPlayerLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.nMaxPlayerLV = script.GetNumber(); script.GetToken(); //,
				NPCBuffSkill.dwSkillTime = script.GetNumber();
				script.GetToken(); // )
				lpCharacter->m_vecNPCBuffSkill.push_back( NPCBuffSkill );
			}
			else if( script.Token == "SetLang" )
			{
				script.GetToken(); // (
				DWORD wLang = script.GetNumber();
				DWORD wSubLang = 0;
				script.GetToken(); // , or )
				if( script.Token == "," )
					wSubLang = script.GetNumber();
				else
					script.GetToken(); // )

				lpCharacter->m_vecdwLanguage.push_back( MAKELONG( wSubLang, wLang ) );
			}
			else if( script.Token == "SetOutput" )
			{
				script.GetToken(); // (
				script.GetToken();
				script.Token.MakeUpper();
				if( script.Token == "FALSE" )
					lpCharacter->bOutput = FALSE;
				script.GetToken(); // )
			}
			else if( script.Token == "AddTeleport" )
			{
				script.GetToken(); // (
				D3DXVECTOR3 vPos( 0.0f, 0.0f, 0.0f );
				vPos.x = script.GetFloat();
				script.GetToken(); // ,
				vPos.z = script.GetFloat();
				script.GetToken(); // )
				lpCharacter->m_vecTeleportPos.push_back( vPos );
			}
		}
		script.GetToken(); // Mover name
		strName = script.Token;
	} 
	return TRUE;
}	

void CProject::InterpretRandomItem(LPRANDOM_ITEM /*pRandomItem*/,CScript& script)
{
	script.GetToken(); // {
	script.GetToken(); 
	while(*script.token != '}')
	{
		if(script.Token == ";") 
		{ 
			script.GetToken(); 
			continue; 
		} 
		script.GetToken();
	}
}

BOOL CProject::LoadExpTable( LPCTSTR lpszFileName )
{	
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;

	DWORD dwVal;
	int i = 0;

	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		i = 0;
		if( script.Token == _T( "expCharacter" ) )
		{
			EXPINTEGER nVal;
			ZeroMemory( m_aExpCharacter, sizeof( m_aExpCharacter ) );
			script.GetToken(); // { 
			nVal	= script.GetExpInteger();
			while( *script.token != '}' )
			{
				m_aExpCharacter[i].nExp1	= nVal;
				m_aExpCharacter[i].nExp2	= script.GetExpInteger();
				m_aExpCharacter[i].dwLPPoint	= script.GetNumber();
				m_aExpCharacter[i++].nLimitExp		= script.GetExpInteger();
				nVal	= script.GetExpInteger();
			}
		}
		else if( script.Token == _T( "expFlight" ) )
		{
			ZeroMemory( m_aFxpCharacter, sizeof( m_aFxpCharacter ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aFxpCharacter[ i ].dwFxp = dwVal;
				m_aFxpCharacter[ i++ ].dwFxp2 = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expLPPoint" ) )
		{
			ZeroMemory( m_aExpLPPoint, sizeof( m_aExpLPPoint ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpLPPoint[ i++ ] = dwVal;
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expJobLevel" ) )
		{
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetNumber();		// skip
				script.GetNumber();		// skip
			}
		}
		else if( script.Token == _T( "expSkill" ) )
		{
			ZeroMemory( m_aExpSkill, sizeof( m_aExpSkill ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpSkill[ i++ ] = dwVal;
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "attackStyle" ) )
		{
			//			ZeroMemory( m_aAttackStyle, sizeof( m_aAttackStyle ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetNumber();
				script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expParty" ) )
		{
			ZeroMemory( m_aExpParty, sizeof( m_aExpParty ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpParty[ i ].Exp = dwVal;
				m_aExpParty[ i++ ].Point = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expPartySkill" ) )
		{
			ZeroMemory( m_aAddExpParty, sizeof( m_aAddExpParty ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aAddExpParty[ i ].Exp = dwVal;
				m_aAddExpParty[ i++ ].Level = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else if( script.Token == _T( "expCompanyTest" ) )
		{
			CGuildTable::GetInstance().ReadBlock( script );
		}
		else if( script.Token == _T( "expDropLuck" ) )
		{
			memset( m_adwExpDropLuck, 0, sizeof(m_adwExpDropLuck) );
			script.GetToken();	// {
			DWORD* pdw	= &m_adwExpDropLuck[0][0];
			DWORD dwValue	= (DWORD)script.GetNumber();	//
			while( *script.token != '}' )
			{
				FLASSERT( pdw <= &m_adwExpDropLuck[121][10] );
				*pdw	= dwValue;
				pdw++;
				dwValue	= script.GetNumber();
			}
		}
		else if( script.Token == _T( "Setitem" ) )
		{
			script.GetToken();	// {
			int	 nAbilityOption		= 0;
			int nVal	= script.GetNumber();
			while( *script.token != '}' )
			{
				m_aSetItemAvail[nAbilityOption].nHitRate	= nVal;
				m_aSetItemAvail[nAbilityOption].nBlock	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nMaxHitPointRate	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nAddMagic	= script.GetNumber();
				m_aSetItemAvail[nAbilityOption].nAdded	= script.GetNumber();
				nVal	= script.GetNumber();
				nAbilityOption++;
			}
		}
		else if( script.Token == _T( "expUpitem" ) )
		{
			ZeroMemory( m_aExpUpItem, sizeof( m_aExpUpItem ) );
			script.GetToken();		// {
			int i = 0, j = 0, nVal;
			while( 1 )
			{
				nVal = script.GetNumber();
				if( *script.token == '}' )	break;
				j = 0;
				m_aExpUpItem[ j++ ][ i ] = nVal;
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				m_aExpUpItem[ j++ ][ i ] = script.GetNumber();
#endif

				i++;
			}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
			if( j != 7 )
#else
			if( j != 6 )
#endif
				FLERROR_LOG( PROGRAM_NAME, _T( "expUpitem 블럭의 데이타 열개수가 맞지 않다. %d" ), j );
			if( i != _MAX_ENCHANT_LEVEL + 1 )
				FLERROR_LOG( PROGRAM_NAME, _T( "expUpitem 블럭의 데이타 행개수가 맞지 않다. %d" ), j );


		} // expUpitem
		else if( script.Token == _T( "RestExpFactorTable" ) )
		{
			int nMinLevel, nMaxLevel;
			float fFactor;
			script.GetToken(); // {
			while( 1 )
			{
				nMinLevel	= script.GetNumber();
				if( *script.token == '}' ) break;
				nMaxLevel	= script.GetNumber();
				fFactor		= script.GetFloat();
#ifdef __WORLDSERVER
				GuildHouseMng->AddRestExpFactorTable( nMinLevel, nMaxLevel, fFactor );
#endif // __WORLDSERVER
			}
		}

		script.GetToken(); // type name or }
	}
	return TRUE;
}

CCtrl* CProject::GetCtrl( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return pCtrl;
	return NULL;
}

#ifdef __CLIENT
CSfx* CProject::GetSfx( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SFX ? (CSfx*)pCtrl : NULL );
	return NULL;
}
#endif

CItem* CProject::GetItem( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_ITEM ? (CItem*)pCtrl : NULL );
	return NULL;
}

CMover* CProject::GetMover( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_MOVER ? (CMover*)pCtrl : NULL );
	return NULL;
}

CShip* CProject::GetShip( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SHIP ? (CShip*)pCtrl : NULL );
	return NULL;
}


#ifdef __WORLDSERVER

FLWSUser* CProject::GetUser( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( ( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() ) ? (FLWSUser*)pCtrl : NULL );
	return NULL;
}
#endif	

ObjProp* CProject::GetProp( int nType, int nIndex )
{
	switch( nType )
	{
	case OT_CTRL: return m_aPropCtrl.GetAt( nIndex ); 
		//	case OT_ITEM: return m_aPropItem.GetAt( nIndex ); 
	case OT_MOVER: return GetMoverProp( nIndex ); 
	}
	return NULL;
}

// 프로퍼티 메모리를 읽기전용으로 변경한다.
void CProject::ProtectPropMover()
{
	// 백엔드 시스템을 위해서 막아둔다.
	//	DWORD dwOld;
	//	::VirtualProtect( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, PAGE_READONLY, &dwOld );
}

#ifdef __S1108_BACK_END_SYSTEM
void CProject::AddMonsterProp( MONSTER_PROP MonsterProp )
{
	FLStrcpy( m_aMonsterProp[ m_nMonsterPropSize ].szMonsterName, _countof( m_aMonsterProp[ m_nMonsterPropSize ].szMonsterName ), MonsterProp.szMonsterName );
	m_aMonsterProp[ m_nMonsterPropSize ].nHitPoint = MonsterProp.nHitPoint;
	m_aMonsterProp[ m_nMonsterPropSize ].nAttackPower = MonsterProp.nAttackPower;
	m_aMonsterProp[ m_nMonsterPropSize ].nDefence = MonsterProp.nDefence;
	m_aMonsterProp[ m_nMonsterPropSize ].nExp = MonsterProp.nExp;
	m_aMonsterProp[ m_nMonsterPropSize ].nItemDrop = MonsterProp.nItemDrop;
	m_aMonsterProp[ m_nMonsterPropSize ].nPenya = MonsterProp.nPenya;
	m_aAddProp[ m_nAddMonsterPropSize ] = m_aMonsterProp[ m_nMonsterPropSize ];
	++m_nMonsterPropSize;
	++m_nAddMonsterPropSize;
}

void CProject::RemoveMonsterProp( char* lpszMonsterName )
{
	BOOL bFind = FALSE;
	int nFindIndex = 0;
	for( int i = 0 ; i < m_nMonsterPropSize ; ++i )
	{
		if( strcmp( m_aMonsterProp[ i ].szMonsterName, lpszMonsterName ) == 0 )
		{
			nFindIndex = i;
			bFind = TRUE;
			break;
		}
	}

	if( bFind )
	{
		for( int i = 0 ; i < m_nMonsterPropSize - 1; ++ i )
		{
			m_aMonsterProp[i] = m_aMonsterProp[i + 1];					
		}
		FLStrcpy( m_aRemoveProp[m_nRemoveMonsterPropSize], _countof( m_aRemoveProp[m_nRemoveMonsterPropSize] ), lpszMonsterName );
		++m_nRemoveMonsterPropSize;
		--m_nMonsterPropSize;
	}			
}
#endif // __S1108_BACK_END_SYSTEM

#ifdef __WORLDSERVER
BOOL CProject::LoadDropEvent( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	do {
		s.GetToken();
		if( s.Token == "DropItem" )
		{
			DROPITEM di = { static_cast< DROPTYPE >( 0 ) };
			DWORD dwMinLevel, dwMaxLevel;
			di.dtType = DROPTYPE_NORMAL;
			s.GetToken();	// (
			di.dwIndex	= s.GetNumber();	// specific item index
			s.GetToken();	// ,
			di.dwProbability	= s.GetNumber();	// probability
			s.GetToken();	// ,
			di.dwLevel	= s.GetNumber();	// level
			s.GetToken();	// ,
			di.dwNumber	= s.GetNumber();	// number
			s.GetToken();	// ,
			dwMinLevel	= s.GetNumber();	// min
			s.GetToken();	// ,
			dwMaxLevel	= s.GetNumber();	// max
			s.GetToken();	// )

			set<DWORD>::iterator i2	= m_setExcept.find( di.dwIndex );
			if( i2 != m_setExcept.end() )
				continue;
			if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
			{
				if( di.dwIndex == ITEM_INDEX( 4100, II_GEN_SKILL_BUFFBREAKER ) )
					di.dwProbability	= (DWORD)( di.dwProbability	* 0.5 );
			}

			for( int i = 0; i < m_nMoverPropSize; i++ )
			{
				MoverProp* pProp	= m_pPropMover + i;
				if( pProp->dwID && pProp->dwLevel >= dwMinLevel && pProp->dwLevel <= dwMaxLevel	)
					pProp->m_DropItemGenerator.AddTail( di, pProp->szName );	// copy
			}
		}
	} while( s.tok != FINISHED );

	return TRUE;
}

BOOL CProject::SortDropItem( void )
{
	// DROPITEM의 dwProbability 내림차순으로 Sortting 
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		MoverProp* pProp	= m_pPropMover + i;
		int nDropItemCount = pProp->m_DropItemGenerator.GetSize();

		DROPITEM *pDropItem0, *pDropItem1, *pDropItemTemp;		
		for( int i = 0 ; i < nDropItemCount - 1 ; ++i )
		{
			pDropItem0 = pProp->m_DropItemGenerator.GetAt( i, FALSE, 0.0f );
			for( int j = i + 1 ; j < nDropItemCount ; ++j )
			{
				pDropItem1 = pProp->m_DropItemGenerator.GetAt( j, FALSE, 0.0f );
				if( pDropItem0->dwProbability > pDropItem1->dwProbability )
				{
					pDropItemTemp = pDropItem0;
					pDropItem0 = pDropItem1;
					pDropItem1 = pDropItemTemp;
				}
			}
		}
	}
	return TRUE;
}
#endif	// __WORLDSERVER


#ifdef __WORLDSERVER
CGiftboxMan::CGiftboxMan()
{
	return;
}

CGiftboxMan* CGiftboxMan::GetInstance( void )
{
	static CGiftboxMan sGiftboxMan;
	return &sGiftboxMan;
}

BOOL CGiftboxMan::AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag, int nSpan, int nAbilityOption )
{
	PT_ITEM_SPEC pGiftBoxProp = g_xSpecManager->GetSpecItem( dwGiftbox );
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItem );
	if( pGiftBoxProp == NULL || pItemProp == NULL )
	{
		return FALSE;
	}

	std::map<DWORD, int>::iterator i	= m_mapIdx.find( dwGiftbox );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_vGiftBox.size();
		GIFTBOX giftBox = { 0 };
		//mem_set( &giftBox, 0, sizeof(giftBox) );
		m_vGiftBox.push_back( giftBox );
		m_mapIdx.insert( std::map<DWORD, int>::value_type( dwGiftbox, nIdx1 ) );
	}

	m_vGiftBox[nIdx1].dwGiftbox	= dwGiftbox;
	int nIdx2	= m_vGiftBox[nIdx1].nSize++;
	m_vGiftBox[nIdx1].adwItem[nIdx2]	= dwItem;
	m_vGiftBox[nIdx1].anNum[nIdx2]	= nNum;
	m_vGiftBox[nIdx1].anFlag[nIdx2]	= nFlag;
	m_vGiftBox[nIdx1].anSpan[nIdx2]	= nSpan;
	m_vGiftBox[nIdx1].anAbilityOption[nIdx2]		= nAbilityOption;

	m_vGiftBox[nIdx1].nSum	+= dwProbability;
	m_vGiftBox[nIdx1].adwProbability[nIdx2]	= m_vGiftBox[nIdx1].nSum;
	return TRUE;
}

BOOL CGiftboxMan::IsEnoughEmptySpace( FLWSUser* pUser, DWORD dwGiftBoxId )
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return FALSE;
	}

	std::map<DWORD, int>::iterator it = m_mapIdx.find( dwGiftBoxId );
	if( it == m_mapIdx.end() )
	{
		return FALSE;
	}

	int nIdx		= it->second;
	PGIFTBOX pBox	= &m_vGiftBox[nIdx];
	if( pBox == NULL )
	{
		return FALSE;
	}

	for( int i = 0; i < pBox->nSize; ++i )
	{
		DWORD dwItemId	= pBox->adwItem[ i ];
		int nItemNum	= pBox->anNum[ i ];

		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
		if( pItemProp == NULL )
		{
			return FALSE;
		}

		int nRequireSpace = nItemNum / pItemProp->dwPackMax;
		if( nItemNum % pItemProp->dwPackMax > 0 )
		{
			if( ++nRequireSpace <= 0 )
			{
				return FALSE;
			}
		}

		if( pUser->m_Inventory.GetEmptyCountByItemId( dwItemId ) < nRequireSpace )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CGiftboxMan::Open( DWORD dwGiftbox, PGIFTBOXRESULT pGiftboxResult )
{
	DWORD dwRand	= xRandom( 1000000 );
	std::map<DWORD, int>::iterator i		= m_mapIdx.find( dwGiftbox );
	if( i == m_mapIdx.end() )
	{
		return FALSE;
	}
	int nIdx	= i->second;
	PGIFTBOX pBox	= &m_vGiftBox[nIdx];

	int low	= 0;
	for( int j = 0; j < pBox->nSize; j++ )
	{
		if( dwRand >= (DWORD)( low ) && dwRand < pBox->adwProbability[j] )
		{
			//			*pdwItem	= pBox->adwItem[j];
			//			*pnNum	= pBox->anNum[j];
			//			*pnFlag	= pBox->anFlag[j];
			//			*pnSpan	= pBox->anSpan[j];
			pGiftboxResult->dwItem	= pBox->adwItem[j];
			pGiftboxResult->nNum	= pBox->anNum[j];
			pGiftboxResult->nFlag	= pBox->anFlag[j];
			pGiftboxResult->nSpan	= pBox->anSpan[j];
			pGiftboxResult->nAbilityOption	= pBox->anAbilityOption[j];
			return TRUE;
		}
	}
	return FALSE;
}

void CGiftboxMan::Verify( void )
{
	for( DWORD i = 0; i < m_vGiftBox.size(); i++ )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "GIFTBOX : %d, %d" ), m_vGiftBox[i].dwGiftbox, m_vGiftBox[i].nSum );
		m_vGiftBox[i].adwProbability[m_vGiftBox[i].nSize-1]	+= ( 1000000 - m_vGiftBox[i].nSum );
	}	
}

BOOL CProject::LoadGiftbox( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwGiftbox, dwItem, dwProbability;
	int	nNum;
	BYTE	nFlag;
	int	nSpan;
	int	nAbilityOption;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "GiftBox" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox2" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox3" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= static_cast< BYTE >( s.GetNumber() );
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum, nFlag ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox4" ) || s.Token == _T( "GiftBox5" ) )
		{
			DWORD dwPrecision	= ( s.Token == _T( "GiftBox4" )? 100: 10 );
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= static_cast< BYTE >( s.GetNumber() );
				nSpan	= s.GetNumber();	// 기간

				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * dwPrecision, nNum, nFlag, nSpan ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "GiftBox6" ) )
		{
			dwGiftbox	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwProbability	= s.GetNumber();
				nNum	= s.GetNumber();
				nFlag	= static_cast< BYTE >( s.GetNumber() );
				nSpan	= s.GetNumber();	// 기간
				nAbilityOption	= s.GetNumber();	// +

				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 10, nNum, nFlag, nSpan, nAbilityOption ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
		s.GetToken();
	}
	CGiftboxMan::GetInstance()->Verify();
	return TRUE;
}

#endif	// __WORLDSERVER

//CPackItem::CPackItem()
//{
//	m_nSize	= 0;
//	mem_set( m_packitem, 0, sizeof(m_packitem) );
//}
//
//CPackItem* CPackItem::GetInstance( void )
//{
//	static CPackItem sPackItem;
//	return &sPackItem;
//}
//
//BOOL CPackItem::AddItem( DWORD dwPackItem, DWORD dwItem, int nAbilityOption, int nNum )
//{
//	std::map<DWORD, int>::iterator i	= m_mapIdx.find( dwPackItem );
//	int nIdx1	= 0;
//	if( i != m_mapIdx.end() )
//	{
//		nIdx1	= i->second;
//	}
//	else
//	{
//		nIdx1	= m_nSize++;
//		m_mapIdx.insert( std::map<DWORD, int>::value_type( dwPackItem, nIdx1 ) );
//	}
//
//	//if( m_nSize >= MAX_PACKITEM )
//	//{
//	//	FLTRACE_LOG( PROGRAM_NAME, _T( "TOO MANY PACKITEM" ) );
//	//	return FALSE;
//	//}
//
//	//if( m_packitem[nIdx1].nSize == MAX_ITEM_PER_PACK )
//	//{
//	//	FLTRACE_LOG( PROGRAM_NAME, _T( "TOO MANY ITEM PER PACK" ) );
//	//	return FALSE;
//	//}
//
//	m_packitem[nIdx1].dwPackItem	= dwPackItem;
//	int nIdx2	= m_packitem[nIdx1].nSize++;
//	m_packitem[nIdx1].adwItem[nIdx2]	= dwItem;
//	m_packitem[nIdx1].anAbilityOption[nIdx2]	= nAbilityOption;
//	m_packitem[nIdx1].anNum[nIdx2]	= nNum;
//	return TRUE;
//}
//
//PPACKITEMELEM CPackItem::Open( DWORD dwPackItem )
//{
//	std::map<DWORD, int>::iterator i		= m_mapIdx.find( dwPackItem );
//	if( i != m_mapIdx.end() )
//		return &m_packitem[i->second];	
//	return NULL;
//}
//
////-----------------------------------


FLPackItemProp::FLPackItemProp()
{
}

FLPackItemProp::~FLPackItemProp()
{
	for( PackItemMap::iterator it = m_mapPackItem.begin(); it != m_mapPackItem.end(); ++it )
	{
		delete it->second;
	}
}


FLPackItemProp & FLPackItemProp::GetInstance()
{
	static FLPackItemProp s_PackItem;
	return s_PackItem;
}

BOOL FLPackItemProp::AddItem( const DWORD dwPackItemID, const DWORD dwItemID, const int nAbilityOption, const int nNum )
{
	_T_ELEM * pElem			= NULL;

	PackItemMap::iterator it= m_mapPackItem.find( dwPackItemID );
	if( it == m_mapPackItem.end() )	
	{
		pElem				= new _T_ELEM;
		::ZeroMemory( pElem, sizeof( _T_ELEM ) );
		if( m_mapPackItem.insert( PackItemMap::value_type( dwPackItemID, pElem ) ).second == false )
		{
			//VMessageBox( MB_OK, "[ FLPackItemPropProp::AddItem ][ fail - insert(%u) ]", dwPackItemID );
			FLERROR_LOG( PROGRAM_NAME, "[ FLPackItemPropProp::AddItem ][ fail - insert(%u) ]", dwPackItemID );
			SAFE_DELETE( pElem );
			return FALSE;
		}
	}
	else
	{
		pElem				= it->second;
	}

	pElem->dwPackItemID		= dwPackItemID;
	pElem->vecItemInfo.push_back( _T_ITEM_INFO( dwItemID, nAbilityOption, nNum ) );
	return TRUE;
}

FLPackItemProp::_T_ELEM * FLPackItemProp::Find( const DWORD dwPackItemID )
{
	PackItemMap::iterator it= m_mapPackItem.find( dwPackItemID );
	return ( it == m_mapPackItem.end() ) ? NULL : it->second;
}

//---------------------------------------------------------------------------//



BOOL CProject::LoadPackItem( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwPackItem, dwItem;
	int nAbilityOption, nNum;
	int	nSpan;

	FLTRACE_LOG( PROGRAM_NAME, _T( "packItem" ) );
	FLTRACE_LOG( PROGRAM_NAME, _T( "----------------------------------------" ) );
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "PackItem" ) )
		{
			dwPackItem	= s.GetNumber();
			nSpan	= s.GetNumber();
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				nAbilityOption	= s.GetNumber();
				nNum	= s.GetNumber();

				FLTRACE_LOG( PROGRAM_NAME, _T( "%d" ), dwPackItem );
				//if( !CPackItem::GetInstance()->AddItem( dwPackItem, dwItem, nAbilityOption, nNum ) ) ->
				if( PACK_ITEM_PROP().AddItem( dwPackItem, dwItem, nAbilityOption, nNum ) == FALSE )
					return FALSE;

				dwItem	= s.GetNumber();
			}
			//PPACKITEMELEM pPackItemElem	= CPackItem::GetInstance()->Open( dwPackItem );	->
			FLPackItemProp::_T_ELEM * pPackItemElem	= PACK_ITEM_PROP().Find( dwPackItem );
			if( pPackItemElem )
				pPackItemElem->nSpan	= nSpan;
		}
		s.GetToken();
	}
	FLTRACE_LOG( PROGRAM_NAME, _T( "----------------------------------------" ) );
	return TRUE;
}

PSETITEMAVAIL CProject::GetSetItemAvail( int nAbilityOption )
{
	if( nAbilityOption < 1 )
	{
		return NULL;
	}

	if( nAbilityOption > 10 )
	{
		nAbilityOption = 10;
	}

	return &m_aSetItemAvail[nAbilityOption-1];
}

BOOL CProject::LoadPiercingAvail( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwItemId;
	int nDstParam, nAdjParam;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Piercing" ) )
		{
			dwItemId	= s.GetNumber();
			s.GetToken();	// {
			nDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				nAdjParam	= s.GetNumber();
				if( !CPiercingAvail::GetInstance()->AddPiercingAvail( dwItemId, nDstParam, nAdjParam ) )
					return FALSE;
				nDstParam	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "SetItem" ) )
		{
			int nId;
			char pszString[MAX_SETITEM_STRING];
			nId		= s.GetNumber();

			s.GetToken();
			FLStrcpy( pszString, _countof( pszString ), s.Token );

			CSetItem* pSetItem	= new CSetItem( nId, pszString );

			s.GetToken();	// {
			s.GetToken();	// categori
			while( *s.token != '}' )
			{
				if( s.Token == _T( "Elem" ) )
				{
					s.GetToken();	// {
					DWORD dwItemId	= s.GetNumber();
					int nParts;
					while( *s.token != '}' )
					{
						nParts	= s.GetNumber();
						pSetItem->AddSetItemElem( dwItemId );
						dwItemId	= s.GetNumber();
					}
					s.GetToken();
				}
				else if( s.Token == _T( "Avail" ) )
				{
					s.GetToken();	// {
					int nDstParam	= s.GetNumber();
					while( *s.token != '}' )
					{
						int nAdjParam	= s.GetNumber();
						int nEquiped	= s.GetNumber();
						pSetItem->AddItemAvail( nDstParam, nAdjParam, nEquiped );
						nDstParam	= s.GetNumber();
					}
					s.GetToken();
				}
			}
			pSetItem->SortItemAvail();
			CSetItemFinder::GetInstance()->AddSetItem( pSetItem );
		}

		else if( s.Token == _T( "RandomOptItem" ) )
		{
			RANDOMOPTITEM randomOptItem;
			randomOptItem.nId	= s.GetNumber();
			s.GetToken();
			FLStrcpy( randomOptItem.pszString, _countof( randomOptItem.pszString ), s.Token );
			randomOptItem.nLevel	= s.GetNumber();
			randomOptItem.dwProbability		= s.GetNumber();
			s.GetToken();	// {
			int nDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				int nAdjParam	= s.GetNumber();
				randomOptItem.ia.anDstParam[randomOptItem.ia.nSize]		= nDstParam;
				randomOptItem.ia.anAdjParam[randomOptItem.ia.nSize]		= nAdjParam;
				randomOptItem.ia.nSize++;
				nDstParam	= s.GetNumber();
			}
			CRandomOptItemGen::GetInstance()->AddRandomOption( &randomOptItem );
		}
		s.GetToken();
	}
	CRandomOptItemGen::GetInstance()->Arrange();

	return TRUE;
}

CPiercingAvail::CPiercingAvail()
{
	m_nSize		= 0;
	memset( m_pPiercingAvail, 0, sizeof(m_pPiercingAvail) );
}

CPiercingAvail::~CPiercingAvail()
{
}

CPiercingAvail* CPiercingAvail::GetInstance( void )
{
	static CPiercingAvail sPiercingAvail;
	return &sPiercingAvail;
}

BOOL CPiercingAvail::AddPiercingAvail( DWORD dwItemId, int nDstParam, int nAdjParam )
{
	std::map<DWORD, int>::iterator i	= m_mapIdx.find( dwItemId );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_nSize++;
		m_mapIdx.insert( std::map<DWORD, int>::value_type( dwItemId, nIdx1 ) );
	}

	if( m_nSize >= MAX_PIERCING_MATERIAL )
		return FALSE;

	if( m_pPiercingAvail[nIdx1].nSize == MAX_PIERCING_DSTPARAM )
		return FALSE;

	m_pPiercingAvail[nIdx1].dwItemId	= dwItemId;
	int nIdx2	= m_pPiercingAvail[nIdx1].nSize++;
	m_pPiercingAvail[nIdx1].anDstParam[nIdx2]	= nDstParam;
	m_pPiercingAvail[nIdx1].anAdjParam[nIdx2]	= nAdjParam;

	return TRUE;
}

PPIERCINGAVAIL CPiercingAvail::GetPiercingAvail( DWORD dwItemId  )
{
	std::map<DWORD, int>::iterator i	= m_mapIdx.find( dwItemId );
	if( i != m_mapIdx.end() )
		return &m_pPiercingAvail[i->second];
	return NULL;
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
BOOL CProject::Load_GemAbility()
{
	CScanner s;
	
	if( !s.Load( "Costume_GemAbility.txt" ) )
		return FALSE;
	
//	int nMaxGemNum = 0;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "ABILITY" ) )
		{
			__GEMABILITYKIND GemAbilityKind;
			s.GetToken();
			GemAbilityKind.nAbility = CScript::GetDefineNum( s.Token );
			
			s.GetToken(); // {
			//s.GetToken();
			while( *s.token != '}' )
			{
				for( int i=0; i<4; i++ )
					GemAbilityKind.vecAbility.push_back( s.GetNumber() );
				s.GetToken();
			}
			CSetGem::GetInstance()->AddGemAvail(GemAbilityKind);
		}
		
		s.GetToken();
	}	
	return TRUE;
}

CSetGem::CSetGem()
{
}

CSetGem::~CSetGem()
{
}

CSetGem* CSetGem::GetInstance( void )
{
	static CSetGem sSetGem;
	return &sSetGem;
}

void	CSetGem::AddGemAvail( __GEMABILITYKIND gembilitykind )
{
	m_vecGemAbilityKind.push_back( gembilitykind );

}

int	CSetGem::GetGemAvail( int nAblity, int nNum )
{
	int nValue = 0;
	for( int i = 0; i < m_vecGemAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[i].nAbility == nAblity )
		{
			nValue = m_vecGemAbilityKind[i].vecAbility[nNum];
		}
	}

	return nValue;
}
#endif

CSetItem::CSetItem( int nId, const char* pszString )
{
	m_nId	= nId;
	FLStrcpy( m_pszString, _countof( m_pszString ), pszString );
	m_nElemSize		= 0;
}

BOOL CSetItem::AddSetItemElem( DWORD dwItemId )
{
	if( m_nElemSize == MAX_SETITEM_ELEM )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "too many setitem elements" ) );
		return FALSE;
	}
	for( int i = 0; i < m_nElemSize; ++i )	{
		if( m_adwItemId[i] == dwItemId )	{
			++m_anEquipCount[i];
			return TRUE;
		}
	}

	m_adwItemId[m_nElemSize]	= dwItemId;
	m_anEquipCount[m_nElemSize]	= 1;
	m_nElemSize++;
	return TRUE;
}

BOOL CSetItem::AddItemAvail( int nDstParam, int nAdjParam, int nEquiped )
{
	if( m_avail.nSize == MAX_ITEMAVAIL )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "too many setitem avail" ) );
		return FALSE;
	}
	m_avail.anDstParam[m_avail.nSize]	= nDstParam;
	m_avail.anAdjParam[m_avail.nSize]	= nAdjParam;
	m_avail.anEquiped[m_avail.nSize]	= nEquiped;
	m_avail.nSize++;
	return TRUE;
}

void CSetItem::SortItemAvail( void )
{
	for( int i = 0; i < m_avail.nSize-1; i++ )
	{
		for( int j = i+1; j < m_avail.nSize; j++ )
		{
			if( m_avail.anEquiped[i] > m_avail.anEquiped[j] )
			{
				int nDstParam, nAdjParam, nEquiped;

				nDstParam	= m_avail.anDstParam[i];
				nAdjParam	= m_avail.anAdjParam[i];
				nEquiped	= m_avail.anEquiped[i];
				m_avail.anDstParam[i]	= m_avail.anDstParam[j];
				m_avail.anAdjParam[i]	= m_avail.anAdjParam[j];
				m_avail.anEquiped[i]	= m_avail.anEquiped[j];
				m_avail.anDstParam[j]	= nDstParam;
				m_avail.anAdjParam[j]	= nAdjParam;
				m_avail.anEquiped[j]	= nEquiped;
			}
		}
	}
}

void CSetItem::GetItemAvail( PITEMAVAIL pItemAvail, int nEquiped, BOOL bAll )
{
	for( int i = 0; i < m_avail.nSize; i++ )
	{
		if( m_avail.anEquiped[i] > nEquiped )
			return;

		if( !bAll && m_avail.anEquiped[i] != nEquiped )
			continue;

		int nFind	= -1;
		for( int j = 0; j < pItemAvail->nSize; j++ )
		{
			if( pItemAvail->anDstParam[j] == m_avail.anDstParam[i] )
			{
				nFind	= j;
				break;
			}
		}
		if( nFind < 0 )
			nFind	= pItemAvail->nSize++;
		pItemAvail->anDstParam[nFind]	= m_avail.anDstParam[i];
		pItemAvail->anAdjParam[nFind]	+= m_avail.anAdjParam[i];
	}
}

void CSetItemFinder::Free()
{
	for( std::map<int, CSetItem*>::iterator i	= m_mapSetId.begin(); i != m_mapSetId.end(); ++i )
	{
		CSetItem* pSetItem	= i->second;
		SAFE_DELETE( pSetItem );
	}
	m_mapSetId.clear();
	m_mapItemId.clear();
}

CSetItemFinder*	CSetItemFinder::GetInstance( void )
{
	static	CSetItemFinder	sSetItemFinder;
	return &sSetItemFinder;
}

void CSetItemFinder::AddSetItem( CSetItem* pSetItem )
{
	bool	bResult	= m_mapSetId.insert( std::map<int, CSetItem*>::value_type( pSetItem->m_nId, pSetItem ) ).second;
	if( !bResult )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "[세트 아이템] 세트 아이템 등록 실패. 아이디 중복입니다 ID - [%u]" ), pSetItem->m_nId );
		SAFE_DELETE( pSetItem );
		return;
	}
	for( int i = 0; i < pSetItem->m_nElemSize; i++ )
	{
		bResult	= m_mapItemId.insert( std::map<DWORD, CSetItem*>::value_type( pSetItem->m_adwItemId[i], pSetItem ) ).second;
		if( !bResult )
			FLTRACE_LOG( PROGRAM_NAME, _T( "[세트 아이템] 세트 아이템(파트) 등록 실패. 이미 등록되어 있는 아이템이 있습니다. ID - [%u], 세트이름 - [%s]" ), pSetItem->m_adwItemId[i], pSetItem->m_pszString );
	}
}

CSetItem* CSetItemFinder::GetSetItem( int nSetItemId )
{
	std::map<int, CSetItem*>::iterator i	= m_mapSetId.find( nSetItemId );
	if( i != m_mapSetId.end() )
		return i->second;
	return NULL;
}

CSetItem* CSetItemFinder::GetSetItemByItemId( DWORD dwItemId )
{
	std::map<DWORD, CSetItem*>::iterator i	= m_mapItemId.find( dwItemId );
	if( i != m_mapItemId.end() )
		return i->second;
	return NULL;
}	

CRandomOptItemGen::CRandomOptItemGen()
{
//	int m_nSize		= 0;
	memset( m_aRandomOptItem, 0, sizeof(m_aRandomOptItem) );
	memset( m_anIndex, 0, sizeof(m_anIndex) );
}

void CRandomOptItemGen::Free( void )
{
	m_mapid.clear();
}

BOOL CRandomOptItemGen::AddRandomOption( PRANDOMOPTITEM pRandomOptItem )
{
	if( m_nSize >= MAX_RANDOMOPTITEM )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "adding randomoptitem\t// 1" ) );
		return FALSE;
	}
	memcpy( &m_aRandomOptItem[m_nSize], pRandomOptItem, sizeof(RANDOMOPTITEM) );
	m_nSize++;
	return TRUE;
}

CRandomOptItemGen* CRandomOptItemGen::GetInstance( void )
{
	static	CRandomOptItemGen sRandomOptItemGen;
	return &sRandomOptItemGen;
}

void CRandomOptItemGen::Arrange( void )
{
	// sort
	for( int i = 0; i < m_nSize-1; i++ )
	{
		for( int j = i + 1; j < m_nSize; j++ )
		{
			if( m_aRandomOptItem[i].nLevel > m_aRandomOptItem[j].nLevel )
			{
				RANDOMOPTITEM tmp;
				memcpy( &tmp, &m_aRandomOptItem[i], sizeof(RANDOMOPTITEM) );
				memcpy( &m_aRandomOptItem[i], &m_aRandomOptItem[j], sizeof(RANDOMOPTITEM) );
				memcpy( &m_aRandomOptItem[j], &tmp, sizeof(RANDOMOPTITEM) );
			}
		}
	}
	// make index & make lv index
	int nLevel	= 1, nPrevious	= -1;
	for( int i = 0; i  < m_nSize; i++ )
	{
		bool bResult	= m_mapid.insert( std::map<int, int>::value_type( m_aRandomOptItem[i].nId, i ) ).second;
		if( !bResult )
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "adding randomoptitem failed\t// 0" ) );
		}

		if( m_aRandomOptItem[i].nLevel > nLevel )
		{
			for( int j = nLevel; j < m_aRandomOptItem[i].nLevel; j++ )
				m_anIndex[j-1]	= nPrevious;
			nLevel	= m_aRandomOptItem[i].nLevel;
		}
		nPrevious	= i;
	}

	//sun: 10차 전승시스템	Neuz, World, Trans
	// 15차 히어로 레벨확장
	for( int i = nLevel; i <= MAX_MONSTER_LEVEL; i++ )
		m_anIndex[i-1]	= nPrevious;

	//for( int i = nLevel; i <= MAX_CHARACTER_LEVEL; i++ )
	//	m_anIndex[i-1]	= nPrevious;
}

int CRandomOptItemGen::GenRandomOptItem( int nLevel, FLOAT fPenalty, PT_ITEM_SPEC pItemProp, DWORD dwClass )
{
	if( !pItemProp )
		return 0;

	if( pItemProp->dwItemKind1 != IK1_WEAPON && pItemProp->dwItemKind1 != IK1_ARMOR )
		return 0;

	if( nLevel >= MAX_MONSTER_LEVEL )
		nLevel = MAX_MONSTER_LEVEL - 1 ;

	int i	= m_anIndex[nLevel];

	if( i != -1 )
	{
		int nIndex	= xRandom( i + 1 );
		DWORD dwRandom	= xRandom( 3000000000 );
		if( dwClass == RANK_MIDBOSS )
			dwRandom	/= 5;	// 준 보스 몬스터 랜덤 옵션 확률 500%
		DWORD dwProbability	= (DWORD)(m_aRandomOptItem[nIndex].dwProbability * fPenalty);
		if( dwRandom < dwProbability )
			return m_aRandomOptItem[nIndex].nId;
	}
	return 0;
}

PRANDOMOPTITEM CRandomOptItemGen::GetRandomOptItem( int nId )
{
	std::map<int, int>::iterator i	= m_mapid.find( nId );
	if( i != m_mapid.end() )
		return &m_aRandomOptItem[i->second];
	return NULL;
}

const char* CRandomOptItemGen::GetRandomOptItemString( int nId )
{
	PRANDOMOPTITEM pRandomOptItem	= GetRandomOptItem( nId );
	if( pRandomOptItem )
		return pRandomOptItem->pszString;
	return NULL;
}

BOOL CProject::IsGuildQuestRegion( DWORD dwWorldID, const D3DXVECTOR3 & vPos )
{
	CRect rect;
	POINT point;
	for( int i = 0; i < m_aPropGuildQuest.GetSize(); i++ )
	{
		GUILDQUESTPROP* pProp	= m_aPropGuildQuest.GetAt( i );
		if( pProp == NULL )
		{
			continue;
		}

		if( dwWorldID == pProp->dwWorldId )
		{
			rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
			point.x = (int)vPos.x;
			point.y = (int)vPos.z;
			if( rect.PtInRect( point ) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


//LoadPropItem를 호출하고 후처리를 한다.
void CProject::OnAfterLoadPropItem()
{
	for( SpecItemIdItr it = g_xSpecManager->m_SpecItemIdMap.begin(); it != g_xSpecManager->m_SpecItemIdMap.end(); ++it )
	{
		PT_ITEM_SPEC ptItemSpec = &it->second;
		if( ptItemSpec->dwItemKind3 != NULL_ID )
		{
			m_itemKindAry[ ptItemSpec->dwItemKind3 ].Add( ptItemSpec );

			switch( ptItemSpec->dwItemKind3 )
			{
				// 스크립트에서 삭제하자...
			case IK3_EVENTMAIN:
			case IK3_BINDS:
				//ptItemSpec->dwFlag |= IP_FLAG_BINDS;
				ptItemSpec->nBindCondition = BIND_CONDITION_GET;
				break;
			}
		}
	}


	PT_ITEM_SPEC ptmp;
	for( int i = 0; i < MAX_ITEM_KIND3; i++ )
	{
		// sort
		for( int j = 0; j < m_itemKindAry[i].GetSize() - 1; j++ )
		{
			for( int k = j + 1; k < m_itemKindAry[i].GetSize(); k++ )
			{
				if( ( (PT_ITEM_SPEC)m_itemKindAry[i].GetAt( k ) )->dwItemRare < ( (PT_ITEM_SPEC)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
				{
					ptmp	= (PT_ITEM_SPEC)m_itemKindAry[i].GetAt( j );
					m_itemKindAry[i].SetAt( j, (void*)m_itemKindAry[i].GetAt( k ) );
					m_itemKindAry[i].SetAt( k, (void*)ptmp );
				}
			}
		}
		//
		DWORD dwItemRare	= NULL_ID;
		for( int j = 0; j < m_itemKindAry[i].GetSize(); j++ )
		{
			if( dwItemRare != ( (PT_ITEM_SPEC)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
			{
				dwItemRare	= ( (PT_ITEM_SPEC)m_itemKindAry[i].GetAt( j ) )->dwItemRare;
				if( dwItemRare != NULL_ID )
				{
					m_minMaxIdxAry[i][dwItemRare].cx	= j;
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
				}
			}
			else
			{
				if( dwItemRare != NULL_ID )
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
			}
		}
	}

	LoadExcept( "except.txt" );
}


BOOL CProject::LoadExcept( LPCTSTR lpszFilename )
{
	CScript s;
	if( s.Load( lpszFilename ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		int nLang	= atoi( s.token );
		//	BEGIN110324	확인 해봐야... -_-;;;
		//		int nSubLang	= s.GetNumber();	//

		s.GetToken();	// {
		s.GetToken();	// itemProp & moverProp
		while( *s.token != '}' )
		{
			if( s.Token == "ItemProp" )
			{
				s.GetToken();	// {
				s.GetToken();	// dwID
				while( *s.token != '}' )
				{
					DWORD dwID	= atoi( s.token );
					PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( dwID );
					s.GetToken();	// {
					s.GetToken();	// attribute
					while( *s.token != '}' )
					{
						if( g_xFlyffConfig->GetMainLanguage() != nLang )
						{
							s.GetToken();
							continue;
						}
						if( s.Token == "fFlightSpeed" )
						{
							s.GetToken();	// =
							pItemProp->fFlightSpeed	= s.GetFloat();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwShopAble" )
						{
							s.GetToken();	// =
							pItemProp->dwShopAble	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwCircleTime" )
						{
							s.GetToken();	// =
							pItemProp->dwCircleTime	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwFlag" )
						{
							s.GetToken();	// =
							pItemProp->dwFlag	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwLimitLevel1" )
						{
							s.GetToken();	// =
							pItemProp->nMinLimitLevel	= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "dwSkillReadyType" )
						{	// 금란지교
							s.GetToken();	// =
							pItemProp->dwSkillReadyType		= s.GetNumber();
							s.GetToken();	// ;
						}
						else if( s.Token == "worldDrop" )
						{
							s.GetToken();	// =
							BOOL b	= (BOOL)s.GetNumber();
							if( !b )
								m_setExcept.insert( dwID );	
							s.GetToken();	// ;
						}
						s.GetToken();	// attribute
					}
					s.GetToken();	// dwID
				}
			}
			s.GetToken();	// SkillProp OR moverProp
		}
		s.GetToken();	// lang
	}
	return TRUE;
}

JobProp* CProject::GetJobProp( int nIndex )
{
	if( nIndex < 0 || nIndex >= MAX_JOB )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "범위침범. %d" ), nIndex );
		return NULL;
	}
	return &m_aPropJob[nIndex];
}

//sun:9, 제련 이팩트 향상 버전
BOOL CProject::LoadPropEnchant( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Enchant", "프리프", MB_OK );
		FLERROR_LOG( PROGRAM_NAME, _T( "프로퍼티 읽기실패 : Enchant" ) );
#endif //__CLIENT
		return FALSE;
	}

//	int nKind = 0;
	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		if( scanner.Token == _T( "NORMAL" ) )
		{
			m_nEnchantLimitLevel[0] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "AL" ) )
		{
			m_nEnchantLimitLevel[1] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "NONE" ) )
		{
			m_nEnchantLimitLevel[2] = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "NORMAL_SCAL" ) )
		{
			m_fEnchantLevelScal[0][0] = scanner.GetFloat();
			m_fEnchantLevelScal[0][1] = scanner.GetFloat();
			m_fEnchantLevelScal[0][2] = scanner.GetFloat();
			m_fEnchantLevelScal[0][3] = scanner.GetFloat();
			m_fEnchantLevelScal[0][4] = scanner.GetFloat();
			m_fEnchantLevelScal[0][5] = scanner.GetFloat();
			m_fEnchantLevelScal[0][6] = scanner.GetFloat();
			m_fEnchantLevelScal[0][7] = scanner.GetFloat();
			m_fEnchantLevelScal[0][8] = scanner.GetFloat();
			m_fEnchantLevelScal[0][9] = scanner.GetFloat();
		}
		else if( scanner.Token == _T( "ALL_SCAL" ) )
		{
			m_fEnchantLevelScal[1][0] = scanner.GetFloat();
			m_fEnchantLevelScal[1][1] = scanner.GetFloat();
			m_fEnchantLevelScal[1][2] = scanner.GetFloat();
			m_fEnchantLevelScal[1][3] = scanner.GetFloat();
			m_fEnchantLevelScal[1][4] = scanner.GetFloat();
			m_fEnchantLevelScal[1][5] = scanner.GetFloat();
			m_fEnchantLevelScal[1][6] = scanner.GetFloat();
			m_fEnchantLevelScal[1][7] = scanner.GetFloat();
			m_fEnchantLevelScal[1][8] = scanner.GetFloat();
			m_fEnchantLevelScal[1][9] = scanner.GetFloat();
		}

		scanner.GetToken();
	}

	return TRUE;
}

// propJob.inc를 읽어들인다.
BOOL CProject::LoadPropJob( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Job", "프리프", MB_OK );
		FLERROR_LOG( PROGRAM_NAME, _T( "프로퍼티 읽기실패 : Job" ) );
#endif //__CLIENT
		return FALSE;
	}

	while( 1 )
	{
		int nJob = scanner.GetNumber();	
		if( scanner.tok == FINISHED )
			break;

		JobProp* pProperty = &m_aPropJob[nJob];
		pProperty->fAttackSpeed      = scanner.GetFloat();		
		pProperty->fFactorMaxHP      = scanner.GetFloat();		
		pProperty->fFactorMaxMP      = scanner.GetFloat();		
		pProperty->fFactorMaxFP      = scanner.GetFloat();		
		pProperty->fFactorDef        = scanner.GetFloat();		
		pProperty->fFactorHPRecovery = scanner.GetFloat();	
		pProperty->fFactorMPRecovery = scanner.GetFloat();	
		pProperty->fFactorFPRecovery = scanner.GetFloat();	
		pProperty->fMeleeSWD		 = scanner.GetFloat();	
		pProperty->fMeleeAXE		 = scanner.GetFloat();	
		pProperty->fMeleeSTAFF		 = scanner.GetFloat();	
		pProperty->fMeleeSTICK		 = scanner.GetFloat();	
		pProperty->fMeleeKNUCKLE	 = scanner.GetFloat();	
		pProperty->fMagicWAND		 = scanner.GetFloat();	
		pProperty->fBlocking		 = scanner.GetFloat();	
		pProperty->fMeleeYOYO        = scanner.GetFloat();	
		pProperty->fCritical         = scanner.GetFloat();	
		
		//gmpbigsun( 20111223 ) : 클래스 마크 파일명 추가됨.
		scanner.GetToken();
		FLStrcpy( pProperty->szMarkFileName, _countof( pProperty->szMarkFileName ), scanner.token );
	}
	return TRUE;
}

BOOL CProject::LoadPropCtrl( LPCTSTR lpszFileName, CIndexArray<CtrlProp>*	apObjProp)
{
	CScript scanner;
	// load unicode
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
#ifdef __CLIENT
		MessageBox( g_Neuz.GetSafeHwnd(), "프로퍼티 읽기실패 : Ctrl", "프리프", MB_OK );
		FLERROR_LOG( PROGRAM_NAME, _T( "프로퍼티 읽기실패 : Ctrl" ) );
#endif //__CLIENT
		return FALSE;
	}

	CtrlProp ctrlProp;

	char szTemp[ 128 ];

	int i	= scanner.GetNumber();	// id

	while( scanner.tok != FINISHED )
	{
		ctrlProp.dwID = i;    
		if( i <= 0 )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s 읽다가 ID가 %d이나왔다. 마지막으로 읽은것 %s" ), lpszFileName, i, szTemp );
		scanner.GetToken();
		//FLTRACE_LOG( PROGRAM_NAME, _T( "PropCtrl = %d, %s" ), i, scanner.token );
		FLStrcpy( ctrlProp.szName, _countof( ctrlProp.szName ), scanner.token );
		m_mapCtrl.insert( std::map<std::string, DWORD>::value_type( ctrlProp.szName, ctrlProp.dwID ) );
		ctrlProp.dwCtrlKind1 = scanner.GetNumber();
		ctrlProp.dwCtrlKind2 = scanner.GetNumber();
		ctrlProp.dwCtrlKind3 = scanner.GetNumber();
		ctrlProp.dwSfxCtrl = scanner.GetNumber();
		ctrlProp.dwSndDamage = scanner.GetNumber();

		scanner.GetToken();		// skip szCommand (사용하지 않는다. )
		apObjProp->SetAt( i, &ctrlProp);
		i = scanner.GetNumber(); 
	}
	apObjProp->Optimize();
	return TRUE;
}

//sun: 8,  
BOOL CProject::LoadScriptDiePenalty( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "DiePenalty.ini not found" ) );
		return FALSE;
	}
	m_vecRevivalPenalty.clear();
	m_vecDecExpPenalty.clear();
	m_vecLevelDownPenalty.clear();

	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{	
		if( scanner.Token == _T( "REVIVAL_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {

			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecRevivalPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		if( scanner.Token == _T( "DECEXP_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {

			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecDecExpPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		if( scanner.Token == _T( "LEVEL_DOWN_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {

			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					DIE_PENALTY DiePenalty;
					DiePenalty.nLevel	= scanner.GetNumber();
					DiePenalty.nValue	= scanner.GetNumber();
					m_vecLevelDownPenalty.push_back( DiePenalty );
				}
				scanner.GetToken();
			}
		}
		scanner.GetToken();
	}
	return TRUE;
}


//sun: 8, // __S8_PK
BOOL CProject::LoadScriptPK( LPCTSTR lpszFileName )
{
	CScript scanner;
	if( scanner.Load( lpszFileName, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "PKSetting.inc not found" ) );
		return FALSE;
	}

	scanner.GetToken();
	while( scanner.tok != FINISHED )
	{
		if( scanner.Token == _T( "GENERAL_COLOR" ) )
		{
			m_PKSetting.dwGeneralColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "READY_COLOR" ) )
		{
			m_PKSetting.dwReadyColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "CHAO_COLOR" ) )
		{
			m_PKSetting.dwChaoColor = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "GENERAL_ATTACK_TIME" ) )
		{
			m_PKSetting.nGeneralAttack = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "READY_ATTACK_TIME" ) )
		{
			m_PKSetting.nReadyAttack = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "LIMITE_LEVEL" ) )
		{
			m_PKSetting.nLimitLevel = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "DIE_PROPENSITY_MIN" ) )
		{
			m_PKSetting.nDiePropensityMin = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "DIE_PROPENSITY_MAX" ) )
		{
			m_PKSetting.nDiePropensityMax = scanner.GetNumber();
		}
		else if( scanner.Token == _T( "PROPENSITY_PENALTY" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {

			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDPENALTY" ) )
				{
					int nPropensity;
					CHAO_PROPENSITY bufPropensity;
					nPropensity						= scanner.GetNumber();
					bufPropensity.dwPropensityMin	= scanner.GetNumber();
					bufPropensity.dwPropensityMax	= scanner.GetNumber();
					bufPropensity.nEquipDorpMin		= scanner.GetNumber();
					bufPropensity.nEquipDorpMax		= scanner.GetNumber();
					bufPropensity.nInvenDorpMin		= scanner.GetNumber();
					bufPropensity.nInvenDorpMax		= scanner.GetNumber();
					bufPropensity.nShop				= scanner.GetNumber();
					bufPropensity.nBank				= scanner.GetNumber();
					bufPropensity.nFly				= scanner.GetNumber();
					bufPropensity.nGuardAttack		= scanner.GetNumber();
					bufPropensity.nVendor			= scanner.GetNumber();
					m_PKSetting.mapPropensityPenalty.insert( std::map<int,CHAO_PROPENSITY>::value_type( nPropensity, bufPropensity ) );
				}
				scanner.GetToken();
			}
		}
		else if( scanner.Token == _T( "LEVEL_EXP" ) )
		{
			scanner.GetToken();	// {
			scanner.GetToken();	// {

			while( *scanner.token != '}' )
			{
				if( scanner.Token == _T( "ADDEXP" ) )
				{
					int nLevel = scanner.GetNumber();
					DWORD dwExp = scanner.GetNumber();
					m_PKSetting.mapLevelExp.insert( std::map<int,DWORD>::value_type( nLevel, dwExp ) );
				}
				scanner.GetToken();
			}
		}
		scanner.GetToken();
	}

	return TRUE;
}


int	CProject::GetMinIdx( int nItemKind3, DWORD dwItemRare )
{
	if( dwItemRare >= MAX_UNIQUE_SIZE )
		return -1;
	return m_minMaxIdxAry[nItemKind3][dwItemRare].cx;
}

int CProject::GetMaxIdx( int nItemKind3, DWORD dwItemRare )
{
	if( dwItemRare >= MAX_UNIQUE_SIZE )
		return -1;
	return m_minMaxIdxAry[nItemKind3][dwItemRare].cy;
}

GUILDQUESTPROP*	CProject::GetGuildQuestProp( int nQuestId )
{
	VERIFY_RANGE( nQuestId, 0, m_aPropGuildQuest.GetSize(), "GetGuildQuestProp range_error", NULL );
	return m_aPropGuildQuest.GetAt( nQuestId );
}

PARTYQUESTPROP*	CProject::GetPartyQuestProp( int nQuestId )
{
	std::map<int, PARTYQUESTPROP>::iterator i = m_propPartyQuest.find( nQuestId );
	if( i != m_propPartyQuest.end() )
		return &i->second;
	return NULL;
}	

DWORD CProject::GetSkillPoint( const SkillProp* pSkillProp ) const
{
	DWORD dwPoint = 0;
	if( pSkillProp->dwSkillKind1 == JTYPE_BASE )
	{
		dwPoint = m_dwVagSP;
	}
	else if( pSkillProp->dwSkillKind1 == JTYPE_EXPERT )
	{
		dwPoint = m_dwExpertSP;
	}

	else if( pSkillProp->dwSkillKind1 == JTYPE_PRO )
	{
		dwPoint = m_dwProSP;
	}
	else if( pSkillProp->dwSkillKind1 == JTYPE_LEGEND_HERO )
	{
		dwPoint = m_dwLegendSP;
	}
	else
	{
		dwPoint = 0;
	}

	return dwPoint;
}

LPCTSTR CProject::GetGuildAppell( int nAppell )
{
	VERIFY_RANGE( nAppell, 0, MAX_GUILDAPPELL, "GetGuildAppell range_error", "error" );
	return m_aGuildAppell[ nAppell ].szName;
}


int CProject::GetExpUpItem( FLItemElem* pItemElem, int nOption ) 
{ 
	if( nOption <= 0 )	
	{
		return 0;
	}

	if( pItemElem == NULL )
	{
		return 0;
	}

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if( pItemProp == NULL )
	{
		return 0;
	}

	if( pItemProp->IsBaruna() == TRUE )
	{
		if( nOption > _MAX_ENCHANT_LEVEL_BARUNA )
		{
			return 0;
		}
	}
	else if( pItemElem->IsCollector() == TRUE )
	{
		if( nOption > _MAX_ENCHANT_LEVEL_COLLECTOR )
		{
			return 0;
		}
	}
	else if( pItemElem->IsAccessory() == TRUE || pItemProp->IsAccessory() == TRUE )
	{
		return 0;
	}
	else
	{
		if( nOption > _MAX_ENCHANT_LEVEL_GENERAL )
		{
			return 0;
		}
	}

	// 	if( nOption < 0 || nOption > 11 )	
	// 	{
	// 		FLERROR_LOG( PROGRAM_NAME, _T( "range_error. %d" ), nOption );
	// 		return 0;
	// 	}

	switch( pItemProp->dwItemKind3 )
	{
	case IK3_SWD:
	case IK3_CHEERSTICK:
	case IK3_WAND:	
	case IK3_AXE:
	case IK3_KNUCKLEHAMMER:
	case IK3_STAFF:
	case IK3_YOYO: 
	case IK3_BOW:
	case IK3_COLLECTER:	//sun: 11, 채집 시스템

#ifdef __ADDWEAPON_CROSSBOW16
	case IK3_CROSSBOW:
#endif //__ADDWEAPON_CROSSBOW16
		return m_aExpUpItem[0][ nOption ];
	case IK3_HELMET:
		return m_aExpUpItem[1][ nOption ];
	case IK3_SUIT:
		return m_aExpUpItem[2][ nOption ];
	case IK3_GAUNTLET:
		return m_aExpUpItem[3][ nOption ];
	case IK3_BOOTS:
		return m_aExpUpItem[4][ nOption ];
	case IK3_SHIELD:
	case IK3_SHILDBARUNA:
	case IK3_MAGICBARUNA:
	case IK3_ZEMBARUNA:
		return m_aExpUpItem[5][ nOption ];
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	case IK3_MASK:
	case IK3_HAT:
	case IK3_CLOTH:
	case IK3_GLOVE:
	case IK3_SHOES:
		return m_aExpUpItem[6][ nOption ];
#endif
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "itemkind오류 %d, %d" ), pItemProp->dwItemKind3, nOption );
	return 0;
}


#ifdef __WORLDSERVER
BOOL	CProject::LoadServerScript( const char* sFile )
{
	CScript s;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), sFile );

	if( s.Load( szFullPath ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return FALSE;
	}

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Accessory_Probability" ) )	// 아이템 각성
		{
			CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
			LPDWORD ptr	= pProperty->GetProbabilityPtr();
			int i	= 0;
			s.GetToken();	// {
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				ptr[i++]	= dwProbability;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Collecting_Enchant" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			std::vector<int>* ptr	= pProperty->GetEnchantProbabilityPtr();
			// 1 / 1000
			s.GetToken();	// {
			int nProb	= s.GetNumber();
			while( *s.token != '}' )
			{
				ptr->push_back( nProb );
				nProb	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Collecting_Item" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			std::vector<COLLECTING_ITEM>* ptr	= pProperty->GetItemPtr();
			COLLECTING_ITEM item;
			DWORD dwTotal	= 0;
			s.GetToken();	// {
			item.dwItemId	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwTotal		+= s.GetNumber();
				item.dwProb	= dwTotal;
				ptr->push_back( item );
				item.dwItemId	= s.GetNumber();
			}
		}
#ifdef BATTERY_PREMIUM
		else if( s.Token == _T( "Collecting_PremiumItem" ) )
		{
			CCollectingProperty* pProperty	= CCollectingProperty::GetInstance();
			std::vector<COLLECTING_ITEM>* ptr	= pProperty->GetPremiumItemPtr();
			COLLECTING_ITEM item;
			DWORD dwTotal	= 0;
			s.GetToken();	// {
			item.dwItemId	= s.GetNumber();
			while( *s.token != '}' )
			{
				dwTotal		+= s.GetNumber();
				item.dwProb	= dwTotal;
				ptr->push_back( item );
				item.dwItemId	= s.GetNumber();
			}
		}
#endif
		else if( s.Token == _T( "Pet_LevelupAvail" ) )
		{
			CPetProperty* pProperty		= CPetProperty::GetInstance();
			LPDWORD ptr1	= pProperty->GetLevelupAvailLevelProbabilityPtr();
			LPBYTE ptr2		= pProperty->GetLevelupAvailLevelMaxPtr();
			// 	10000	0	0	0	0	0	0	0	0
			int	nLevel	= (int)PL_D;
			s.GetToken();	// {{
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				//				ptr1[nLevel][0]	= dwProbability;
				ptr1[nLevel*MAX_PET_AVAIL_LEVEL]	= dwProbability;
				for( BYTE i = 1; i < MAX_PET_AVAIL_LEVEL; i++ )
				{
					//					ptr1[nLevel][i]	= s.GetNumber();
					ptr1[nLevel*MAX_PET_AVAIL_LEVEL+i]	= s.GetNumber();
					//					if( ptr1[nLevel][i] > 0 )
					if( ptr1[nLevel*MAX_PET_AVAIL_LEVEL+i] > 0 )
						ptr2[nLevel]	= i;
				}
				nLevel++;
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Pet_AddLifeProbability" ) )
		{
			CPetProperty* pProperty		= CPetProperty::GetInstance();
			std::vector<WORD>* ptr	= pProperty->GetAddLifeProbabilityPtr();
			// 수명 회복액 확률	// 추가 될 경우 합이 100이 되도록 확장
			// 	50	// +1
			s.GetToken();	// {
			WORD wProbability	= static_cast< WORD >( s.GetNumber() );
			while( *s.token != '}' )
			{
				ptr->push_back( wProbability );
				wProbability	= static_cast< WORD >( s.GetNumber() );
			}
		}
		s.GetToken();
	}
	return TRUE;
}
#endif	// __WORLDSERVER

#ifdef __PERF_0226
CPartsItem::CPartsItem()
{
}

CPartsItem::~CPartsItem()
{
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < MAX_HUMAN_PARTS; j++ )
			m_items[i][j].clear();
	}
}

void	CPartsItem::Init()
{
	for( SpecItemIdItr it = g_xSpecManager->m_SpecItemIdMap.begin(); it != g_xSpecManager->m_SpecItemIdMap.end(); ++it )
	{
		PT_ITEM_SPEC pItemProp = &it->second;
		if( pItemProp && pItemProp->dwParts >= 0 && pItemProp->dwParts < MAX_HUMAN_PARTS )
		{
			int nSex	= pItemProp->dwItemSex;
			if( nSex < 0 )
				nSex	= SEX_SEXLESS;
			m_items[nSex][pItemProp->dwParts].push_back( pItemProp );
		}
	}
}

PT_ITEM_SPEC	CPartsItem::GetItemProp( int nSex, int nParts )
{
	int nIndex	= xRandom( m_items[nSex][nParts].size() );
	return	m_items[nSex][nParts][nIndex];
}

CPartsItem*	CPartsItem::GetInstance( void )
{
	static	CPartsItem	sPartsItem;
	return &sPartsItem;
}
#endif	// __PERF_0226

void CProject::OutputDropItem( void )
{
	CString sLog[100];
	int nCount	= 0;
	int nWrite	= 0;
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{

		if( (++nCount % 10) == 0 )
		{
			FILE* fp = FLStdFileOpen("drop.txt","a");
			if(!(fp) )
			{
				fp = FLStdFileOpen("drop.txt" ,"wt");
				if(!(fp))
					return;
			}
			fprintf(fp,sLog[nWrite++]);
			fclose(fp);
		}
		CString s;
		MoverProp* pMoverProp	= prj.GetMoverProp( i );
		if( pMoverProp && pMoverProp->dwID > 0 && pMoverProp->dwID < MAX_PROPMOVER && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
		{
			FLTRACE_LOG( PROGRAM_NAME, _T( "%d" ), i );
			s.Format( "\n%s\t%d", pMoverProp->szName, pMoverProp->m_DropItemGenerator.m_dwMax );
			// dropitem
			{
				int cbDropItem	= pMoverProp->m_DropItemGenerator.GetSize();
//				int nNumber	= 0;
				
				for( int i = 0; i < cbDropItem; i++ )
				{
					DROPITEM* lpDropItem = pMoverProp->m_DropItemGenerator.GetAt( i, FALSE, 0.0f );
					if( lpDropItem )
					{
						if( lpDropItem->dtType == DROPTYPE_NORMAL )
						{
							DWORD dwProbability		= lpDropItem->dwProbability;
							PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( lpDropItem->dwIndex );
							if( lpDropItem->dwLevel != 0 )
							{
								if( pItemProp && (int)pItemProp->dwItemLV > 0 )
								{
									int nValue	= dwProbability / pItemProp->dwItemLV;
									if( nValue < 21 )	nValue	= 21;
									dwProbability	= nValue - 20;
								}
							}
							if( pItemProp )
							{
								CString str;
								str.Format( "\n \t%s\t%f%%\t%d\t%d", pItemProp->szName, (double)dwProbability / (double)30000000, lpDropItem->dwNumber, lpDropItem->dwLevel );
								s	+= str;
							}
						}
						else if( lpDropItem->dtType == DROPTYPE_SEED )
						{
							CString str;
							str.Format(" \tPENYA %d-%d", lpDropItem->dwNumber, lpDropItem->dwNumber2 ); 
							s	+= str;
						}
					}
				}
				sLog[nWrite]	+= s;
			}
			// dropkind
			{
				int nSize	= pMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				for( int i = 0; i < nSize; i++ )
				{
					pDropKind	= pMoverProp->m_DropKindGenerator.GetAt( i );
					pItemKindAry	= prj.GetItemKindAry( pDropKind->dwIK3 );
					int nMinIdx	= -1,	nMaxIdx		= -1;
					for( int j = pDropKind->nMinUniq; j <= pDropKind->nMaxUniq; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pDropKind->dwIK3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( int j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pDropKind->dwIK3, j );
						if( nMaxIdx != -1 )
							break;
					}
					if( nMinIdx < 0 || nMaxIdx < 0 )
						continue;

					for( int a = nMinIdx; a <= nMaxIdx; a++ )
					{
						PT_ITEM_SPEC pItemProp		= (PT_ITEM_SPEC)pItemKindAry->GetAt( a );
						if( !pItemProp )
							continue;

						CString s1;
						s1.Format( "\n \t%s", pItemProp->szName );
						for( int k = 10; k >= 0; k-- )
						{
							DWORD dwPrabability		= (DWORD)( prj.m_adwExpDropLuck[( pItemProp->dwItemLV > 120? 119: pItemProp->dwItemLV-1 )][k] * ( (float)pMoverProp->dwCorrectionValue / 100.0f ) );
							CString s2;
							s2.Format( "\t%d=%f%%", k, (double)dwPrabability / (double)30000000 );
							s1	+= s2;
						}
						sLog[nWrite]	+= s1;
					}
				}
			}
		}
	}

	FILE* fp = FLStdFileOpen("drop.txt","a");
	if(!(fp))
	{
		fp = FLStdFileOpen("drop.txt" ,"wt");
		if(!(fp))
			return;
	}
	fprintf(fp,sLog[nWrite]);
	fclose(fp);
}

#if defined( __CLIENT )
BOOL CProject::LoadQuestDestination( void )
{
	CScanner s;
	if( s.Load( "QuestDestination.txt.txt" ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right( 4 ) );
		s.GetLastFull();
		CString strQuestion = s.Token;
		m_mapQuestDestination.insert(std::make_pair( nID, strQuestion ) );
		s.GetToken();
	}
	return TRUE;
}

const CString& CProject::GetQuestDestination( DWORD dwKey ) const
{
	static CString strEmpty = _T( "" );
	std::map< int, CString >::const_iterator i = m_mapQuestDestination.find( dwKey );
	if( i != m_mapQuestDestination.end() )
		return i->second;
	return strEmpty;
}

BOOL CProject::LoadPatrolDestination( void )
{
	CScanner s;
	if( s.Load( "PatrolDestination.txt.txt" ) == FALSE )
		return FALSE;

	s.GetToken();
	while( s.tok != FINISHED )
	{
		int nID = atoi( s.Token.Right( 4 ) );
		s.GetLastFull();
		CString strQuestion = s.Token;
		m_mapPatrolDestination.insert(std::make_pair( nID, strQuestion ) );
		s.GetToken();
	}
	return TRUE;
}

const CString& CProject::GetPatrolDestination( DWORD dwKey ) const
{
	static CString strEmpty = _T( "" );
	std::map< int, CString >::const_iterator i = m_mapPatrolDestination.find( dwKey );
	if( i != m_mapPatrolDestination.end() )
		return i->second;
	return strEmpty;
}
#endif // defined( __CLIENT )