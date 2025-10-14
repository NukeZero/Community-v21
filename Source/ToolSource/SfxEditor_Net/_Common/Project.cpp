// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "defineItem.h"
#include "lang.h"

#include "Ship.h"
#ifdef __WORLDSERVER
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#endif	// __WORLDSERVER

#include "guild.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __WORLDSERVER
#include "guildquest.h"
#endif	// __WORLDSERVER 

#ifndef		__XTOOL

// state를 사용하는 퀘스트를 저장한다
// 최신 버젼의 퀘스트 시스템은 state 사용을 지양한다.
// 따라서 기존에 state를 사용하던 테이블을 저장하여 향후 임의로 state가 사용될 때
// 이것과 비교하여 에러를 발생시켜, 개발자들로 하여금 state 사용을 원천 봉쇄한다.
// 만약 불가피하게 사용할 필요가 있다면 별도로 프로그래머들에게 요청한다.
#ifdef __V050322_CACHEQUEST
#ifdef __CLIENT
//#define __WRITE_STATE_QUEST 
//#define __CHECK_STATE_QUEST
#endif
#endif

CString GetLangFileName( int nLang, int nType )
{
	char* file[] = { "Filter", "InvalidName", "Notice" };
	CString fileName = file[nType];

	switch( nLang )
	{
	case LANG_KOR:
		fileName = fileName+"_KOR.inc";
		break;
	case LANG_ENG:
		fileName = fileName+"_ENG.inc";
		break;
	case LANG_JAP:
		fileName = fileName+"_JAP.inc";
		break;
	case LANG_CHI:
		fileName = fileName+"_CHI.inc";
		break;
	case LANG_THA:
		fileName = fileName+"_THA.inc";
		break;
	case LANG_TWN:
		fileName = fileName+"_TWN.inc";
		break;
	default:
		Error( "%s File Error = %d", file[nType], nLang );
		break;
	}

	return fileName;
}


void CHARACTER::Clear()
{
	m_nStructure        = -1;
	m_nEquipNum         = 0;
	m_dwHairMesh = 0;
	m_dwHairColor = 0;
	m_dwHeadMesh = 0;

#ifdef __V050322_CACHEQUEST
	m_dwMoverIdx = 0;
	m_dwMusicId = 0;
#endif
	ZeroMemory( m_szKey, sizeof( m_szKey ) );
	ZeroMemory( m_szChar, sizeof( m_szChar ) );
	ZeroMemory( m_szDialog, sizeof( m_szDialog ) );
	ZeroMemory( m_szDlgQuest, sizeof( m_szDlgQuest ) );
	ZeroMemory( m_abMoverMenu, sizeof( m_abMoverMenu ) );
	ZeroMemory( m_adwEquip, sizeof( m_adwEquip ) );
}



/*--------------------------------------------------------------------------------*/
// CDropItemGenerator
DROPITEM* CDropItemGenerator::GetAt( int nIndex, BOOL bUniqueMode, BOOL fProbability )
{
	ASSERT( nIndex < m_dwSize );
	if( fProbability ) {
		DROPITEM* lpDropItem	= &m_lpDropItem[nIndex];
		DWORD dwProbability		= lpDropItem->dwProbability;
		DWORD dwRand = xRandom( 3000000000 );

		if( lpDropItem->dwLevel && bUniqueMode && lpDropItem->dwProbability <= 10000000 )
		{
			dwRand /= 2;
		}
		// 레벨(유니크)이 있나 검사


		// 확률(dwProba....)이 1억이하 인놈만...
		// dwRand를 /2한다.

		//		INT(원래확률/아이템레벨-20)
//		lpDropItem->dwLevel	= nAbilityOption
		if( lpDropItem->dwLevel != 0 )
		{
			ItemProp* pItemProp	= prj.GetItemProp( lpDropItem->dwIndex );
			if( pItemProp && (int)pItemProp->dwItemLV > 0 )
			{
				int nValue	= dwProbability / pItemProp->dwItemLV;
				if( nValue < 21 )	nValue	= 21;
				dwProbability	= nValue - 20;
			}
		}

		return( dwRand < dwProbability? lpDropItem: NULL );
	}
	else {
		return &m_lpDropItem[nIndex];
	}
}

void CDropItemGenerator::AddTail( CONST DROPITEM & rDropItem )
{
	chASSERT( m_dwSize < MAX_DROPITEM );
	memcpy( &m_lpDropItem[m_dwSize++], &rDropItem, sizeof(DROPITEM) );
}

// CQuestItemGenerator
QUESTITEM* CQuestItemGenerator::GetAt( int nIndex )
{
	ASSERT( nIndex < m_uSize );
	return &m_pQuestItem[nIndex];
}

void CQuestItemGenerator::AddTail( const QUESTITEM & rQuestItem )
{
	chASSERT( m_uSize < MAX_QUESTITEM );
	memcpy( &m_pQuestItem[m_uSize++], &rQuestItem, sizeof(QUESTITEM) );
}

void CDropKindGenerator::AddTail( const DROPKIND & rDropKind )
{
	chASSERT( m_nSize < MAX_DROPKIND );
	memcpy( &m_aDropKind[m_nSize++], &rDropKind, sizeof(DROPKIND) );
}

LPDROPKIND CDropKindGenerator::GetAt( int nIndex )
{
	ASSERT( nIndex >= 0 );
	ASSERT( nIndex < m_nSize );
	if( nIndex < 0 || nIndex >= m_nSize )
		return NULL;
	return &m_aDropKind[nIndex];
}
/*--------------------------------------------------------------------------------*/

//CProject prj;

CProject::CProject()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CProject::m_AddRemoveLock" ), m_csPlayerID( "CProject::m_csPlayerID" )
#endif	// __INFOCRITSECT_DEBUG__
{
	m_bOpenProject = FALSE;

#if __CURRENT_LANG == LANG_JAP
	m_nLanguage = LANG_JAP;
#else
	m_nLanguage = LANG_KOR; // default
#endif	

	memset( m_minMaxIdxAry, 0xff, sizeof(SIZE) * MAX_ITEM_KIND3 * MAX_UNIQUE_SIZE );
	memset( m_aExpUpItem, 0, sizeof(m_aExpUpItem) );
	memset( m_aGuildAppell, 0, sizeof(m_aGuildAppell) );

	m_fItemDropRate = 1.0f;
	m_fGoldDropRate = 1.0f;
	m_fMonsterExpRate = 1.0f;
	m_fMonsterHitRate = 1.0f;
	m_fSkillExpRate		= 1.0f;
	m_fShopCost	= 1.0f;
#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;	
	m_fMonsterHitpointRate = 1.0f;	
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_nMonsterPropSize = 0;
#endif // __S1108_BACK_END_SYSTEM

#ifdef __J0608
	memset( m_aSetItemAvail, 0, sizeof(m_aSetItemAvail) );
#endif	// __J0608

	m_nMoverPropSize = 0;
//	m_pPropMover = new MoverProp[MAX_PROPMOVER];
	m_pPropMover = (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );
}

CProject::~CProject()
{
#ifdef __CLIENT
	m_mapHelp.RemoveAll();
	m_mapWordToolTip.RemoveAll();
#endif

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
				delete (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[ j ].GetAt(i);
		}
		delete lpCharacter;
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
#ifdef __V050322_CACHEQUEST 
#if defined( __SINGLE ) || defined( __WORLDSERVER ) 
			for( int j = 0; j < 32; j++ )
				SAFE_DELETE( pQuestProp->m_apQuestDialog[ j ] );
#endif
			for( j = 0; j < 16; j++ )
				SAFE_DELETE( pQuestProp->m_questState[ j ] );
			SAFE_DELETE( pQuestProp->m_paBeginCondItem );
			SAFE_DELETE( pQuestProp->m_paEndCondItem );
			SAFE_DELETE( pQuestProp->m_paEndRewardItem );
			SAFE_DELETE( pQuestProp->m_lpszEndCondMultiCharacter );
#else // __V050322_CACHEQUEST
//{{AFX
			for( int j = 0; j < 32; j++ )
				SAFE_DELETE( pQuestProp->m_apQuestDialog[ j ] );
//}}AFX
#endif // __V050322_CACHEQUEST
		}
	}

//	delete [] m_pPropMover;
	::VirtualFree( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, MEM_DECOMMIT );

}
int SortJobSkill( const void *arg1, const void *arg2 )
{
	ItemProp* pItemProp1 = *(ItemProp**) arg1;
	ItemProp* pItemProp2 = *(ItemProp**) arg2;
	if( pItemProp1->dwReqDisLV < pItemProp2->dwReqDisLV )
		return -1;
	if( pItemProp1->dwReqDisLV > pItemProp2->dwReqDisLV )
		return 1;
	return 0;
}


BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load( lpszFileName )==FALSE)
		return FALSE;

	m_bOpenProject = TRUE;
	_tcscpy( m_szPrjFile, lpszFileName );
	::GetCurrentDirectory( 128, m_szPrjPath );
	_tcscat( m_szPrjPath, "\\" );
	SetResourcePath();

#if defined( __CHECK_STATE_QUEST ) && !defined( __WRITE_STATE_QUEST )
	CFileIO fileState;
	if( fileState.Open( "questState.dat", "rb" ) )
	{
		int nSize;
		fileState.Read( &nSize, sizeof( nSize ) );
		for( int i = 0;i < nSize; i++ )
		{
			int nQuest;
			fileState.Read( &nQuest, sizeof( nQuest ) );
			m_aStateQuest.Add( nQuest );
		}
	}
#endif

	do {
		scanner.GetToken();
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef __BEAST
		if( scanner.Token == _T( "textClient" ) )
		{	
			scanner.GetToken();
			LoadText( scanner.token );
		}
		else
		if( scanner.Token == _T( "propTroupeSkill" ) )
		{	
			scanner.GetToken();	
			LoadPropItem( "propTroupeSkill.txt", &m_aPartySkill );
		}
		else
		if( scanner.Token == _T( "propCard" ) )
		{	
			scanner.GetToken();	
			LoadPropItem( scanner.token, &m_aPropCard );
		}
		else
		if( scanner.Token == _T( "propSfx" ) )
		{	
			scanner.GetToken();
			LoadPropObj( scanner.token, &m_aPropSfx );
		}
		else
		if( scanner.Token == _T( "propObj" ) )
		{	
			scanner.GetToken();	
			LoadPropObj( scanner.token, &m_aPropObj );
		}
		else
		if( scanner.Token == _T( "propSkillAdd") )
		{
			scanner.GetToken();
			LoadPropAddSkill( scanner.token );
		}
		else
		if( scanner.Token == _T( "propQuest") )
		{
			scanner.GetToken();
			LoadPropQuest( scanner.token );
		}
		else
		if( scanner.Token == _T( "expTable" ) )
		{	
			scanner.GetToken();	
			LoadExpTable( scanner.token );
		}
		else
		if( scanner.Token == _T( "MotionProp" ) )
		{	
			scanner.GetToken();	
			LoadMotionProp( scanner.token );
		}
#ifdef __NPP_050308
		else if( scanner.Token == _T( "propKarma" ) )
		{
			scanner.GetToken();
			LoadPropKarma( "propKarma.txt" );
		}
#endif	// __NPP_050308
		else if( scanner.Token == _T( "propJob" ) )
		{
			scanner.GetToken();
			LoadPropJob( "propJob.inc" );
		}
		else
		if( scanner.Token == _T( "propSkill" ) )
		{	
			scanner.GetToken();	
			LoadPropItem( "propSkill.txt", &m_aPropSkill );
			ZeroMemory( m_aJobSkillNum, sizeof( m_aJobSkillNum ) );
			// 각각의 스킬을 정렬해서 배열에 담자 
			for( int i = 1; i < m_aPropSkill.GetSize(); i++ )
			{
				ItemProp* pItemProp = (ItemProp*)m_aPropSkill.GetAt( i );
				if( pItemProp )
				{
					{
						ItemProp** apJobSkill = m_aJobSkill[ pItemProp->dwItemKind2 ];
						apJobSkill[ m_aJobSkillNum[ pItemProp->dwItemKind2 ] ] = pItemProp;
						m_aJobSkillNum[ pItemProp->dwItemKind2 ]++;
					}
				}
				else
				{
					TRACE( "PropSkill Prop NULL SkillIndex = %d\n", i );
				}
			}
			// 소트하기 
			for( i = 0; i < MAX_JOB; i++ )
			{
				ItemProp** apJobSkill = m_aJobSkill[ i ];
				qsort( (void *)apJobSkill, (size_t) m_aJobSkillNum[ i ], sizeof( ItemProp* ), SortJobSkill );
			}
		}
#ifdef __CLIENT
		else
		if( scanner.Token == _T( "help" ) )
		{	
			scanner.GetToken();	
			LoadHelp( scanner.token );
		}
#endif
		else
#endif // __BEAST
//////////////////////////////////////////////////////////////////////////////////////////
		if( scanner.Token == _T( "project" ) )
		{	
		}
		else
#ifdef __BEAST
		if( scanner.Token == _T( "define" ) || scanner.Token == _T( "localDef" ) )
#else
		if( scanner.Token == _T( "define" ) )
#endif
		{	
			scanner.GetToken();
			CScript script;
			if( script.Load( scanner.Token ) == TRUE )
				script.PreScan( FALSE );//TRUE );
		}
		else
		if( scanner.Token == _T( "localDef" ) )
		{	
			scanner.GetToken();
			CScript script;
			if( script.Load( scanner.Token ) == TRUE )
				script.PreScan( TRUE );//TRUE );
		}
		else
		if( scanner.Token == _T( "model" ) )
		{	
			scanner.GetToken();	
			m_modelMng.LoadScript( scanner.token );
		}
		else
		if( scanner.Token == _T( "billboard" ) )
		{	
			scanner.GetToken();	
			m_modelMng.LoadBillboard( MakePath( DIR_MODEL, scanner.token ) );
		}
		else
		if( scanner.Token == _T( "propMover" ) )
		{	
			scanner.GetToken();	
			if( !LoadPropMover( "propMover.txt" ) )
				return FALSE;
		}
		else if( scanner.Token == _T( "propCtrl" ) )
		{
			scanner.GetToken();
#if defined(__WORLDSERVER) || defined(__CLIENT)
			LoadPropCtrl( "propCtrl.txt", &m_aPropCtrl );
#endif
		}
		else
		if( scanner.Token == _T( "propItem" ) )
		{	
			scanner.GetToken();	
			LoadPropItem( "propItem.txt", &m_aPropItem );
			OnAfterLoadPropItem();
		}
		else
		if( scanner.Token == _T( "character") )
		{
			scanner.GetToken();
			LoadCharacter( scanner.token );
		}
		else
		if( scanner.Token == _T( "terrain" ) )
		{	
			scanner.GetToken();	
			m_terrainMng.LoadScript( scanner.token );
		}
		else
		if( scanner.Token == _T( "world" ) )
		{	
			scanner.GetToken();	
			g_WorldMng.LoadScript( scanner.token );
		}

#ifdef __WORLDSERVER
		else
		if( scanner.Token == _T( "Event_LevelUp_ItemList") )
		{
			scanner.GetToken();	
			LoadReNewEvent( scanner.token );
		}
#endif // __WORLDSERVER
	} while(scanner.tok!=FINISHED);

// 비스트에선 읽지 않는다.
#ifndef __BEAST

	LoadMotionProp( _T( "propMotion.txt" ) );
	LoadPropMoverEx( _T( "PropMoverEx.inc" ) );
	LoadJobItem( _T( "jobItem.inc" ) );
	LoadEtc( _T( "etc.inc" ) );

#ifdef __WORLDSERVER
	#ifndef __S1108_BACK_END_SYSTEM
		LoadConstant( "Constant.inc" );
	#endif // __S1108_BACK_END_SYSTEM
	LoadPropGuildQuest( _T( "propGuildQuest.inc" ) );
	LoadDropEvent( _T( "propDropEvent.inc" ) );
	LoadGiftbox( _T( "propGiftbox.inc" ) );
#endif	// __WORLDSERVER

#ifdef __J0524
	LoadPackItem( _T( "propPackItem.inc" ) );
#endif	// __J0524

#ifdef __PIERCING_SM_TIME
	LoadPiercingAvail( _T( "propItemEtc.inc" ) );
#endif	// __PIERCING_SM_TIME

#ifdef __CLIENT
	
	CString strFilter;
	strFilter = GetLangFileName( g_Option.m_nLanguage, FILE_FILTER );

	LoadFilter( _T( strFilter ) );
	//LoadWordToolTip( _T( "wordToolTip.inc") );
#endif

	// 배열 최적화 시작 (한꺼번에 한다.)
	m_aPropQuest.Optimize();
	// 배열 최적화 끝 

#ifdef __WRITE_STATE_QUEST
	CFileIO fileState;
	if( fileState.Open( "questState.dat", "wb" ) )
	{
		int nSize = m_aStateQuest.GetSize();
		fileState.Write( &nSize, sizeof( nSize ) );
		for( int i = 0;i < m_aStateQuest.GetSize(); i++ )
		{
			int nQuest = m_aStateQuest.GetAt( i );
			fileState.Write( &nQuest, sizeof( nQuest ) );
		}
	}
#endif
#ifdef __CHECK_STATE_QUEST
	m_aStateQuest.RemoveAll();
#endif
	//CScript::m_localDef.RemoveAll();
#endif // __BEAST

#ifdef __PRINT_ITEM0718
/*
3. 상점
4. 드롭
5. 제련
6. 피어싱
*/
// 1	// set
	int cbMonster	= 0;
	MoverProp*	apMonster[4096];
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		MoverProp* pMoverProp	= GetMoverProp( i );
		if( pMoverProp && pMoverProp->dwID && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
			apMonster[cbMonster++]	= pMoverProp;
	}

	int cbWeapon	= 0;
	ItemProp*	apWeapon[1024];
	int cbArmor	= 0;
	ItemProp*	apArmor[4096];

	for( i = 0; i < m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp	= m_aPropItem.GetAt( i );
		if( pItemProp && pItemProp->dwItemKind3 != IK3_VIRTUAL )
		{
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
				apWeapon[cbWeapon++]	= pItemProp;
			else if( pItemProp->dwItemKind1 == IK1_ARMOR )
				apArmor[cbArmor++]	= pItemProp;
		}
	}
//	1	// sort
	for( i = 0; i < cbMonster - 1; i++ )
	{
		for( int j = i + 1; j < cbMonster; j++ )
		{
			if( (int)apMonster[i]->dwLevel > (int)apMonster[j]->dwLevel )
			{
				MoverProp* tmp	= apMonster[i];
				apMonster[i]	= apMonster[j];
				apMonster[j]	= tmp;
			}
		}
	}

	for( i = 0; i < cbWeapon - 1; i++ )
	{
		for( int j = i + 1; j < cbWeapon; j++ )
		{
			if( (int)apWeapon[i]->dwItemLV > (int)apWeapon[j]->dwItemLV )
			{
				ItemProp* tmp	= apWeapon[i];
				apWeapon[i]	= apWeapon[j];
				apWeapon[j]	= tmp;
			}
		}
	}

	for( i = 0; i < cbArmor - 1; i++ )
	{
		for( int j = i + 1; j < cbArmor; j++ )
		{
			if( (int)apArmor[i]->dwItemLV > (int)apArmor[j]->dwItemLV )
			{
				ItemProp* tmp	= apArmor[i];
				apArmor[i]	= apArmor[j];
				apArmor[j]	= tmp;
			}
		}
	}
// print
	for( int k = 0; k < cbMonster; k++ )
	{
		FILEOUT( "monster.txt", "%d\t%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
			apMonster[k]->dwLevel, apMonster[k]->dwID, apMonster[k]->szName, apMonster[k]->dwHR, apMonster[k]->dwStr, apMonster[k]->dwSta, apMonster[k]->dwDex, apMonster[k]->dwInt,
			apMonster[k]->dwAddHp,
			apMonster[k]->dwNaturalArmor, apMonster[k]->dwResisMgic, apMonster[k]->dwER, apMonster[k]->dwAtk1, apMonster[k]->dwAtkMin, apMonster[k]->dwAtkMax,
			apMonster[k]->eElementType, apMonster[k]->wElementAtk );
	}

	for( k = 0; k < MAX_JOB; k++ )
	{
//		char pszFile[MAX_PATH]	= { 0, };
//		sprintf( pszFile, "weapon-%d.txt", k );
//		FILEOUT( pszFile, "LEVEL\tNAME\t\tMIN-MAX", "" );
		for( i = 0; i < cbWeapon; i++ )
		{
			if( apWeapon[i]->dwItemJob == k )
			{
//				FILEOUT( pszFile, "%d\t%s\t\t%d-%d\r\n", 
	//				apWeapon[i]->dwItemLV, apWeapon[i]->szName, apWeapon[i]->dwAbilityMin, apWeapon[i]->dwAbilityMax );
				FILEOUT( "item.txt", "%d\t%d\t%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%f\t%d\t%d", 
					k, apWeapon[i]->dwLimitLevel1, apWeapon[i]->dwID, apWeapon[i]->szName,
					apWeapon[i]->nAddSkillMin, apWeapon[i]->nAddSkillMax,
					apWeapon[i]->dwWeaponType,
					apWeapon[i]->dwAbilityMin, apWeapon[i]->dwAbilityMax, apWeapon[i]->fAttackSpeed, apWeapon[i]->dwItemKind3, apWeapon[i]->dwItemJob );
			}
		}
/*
		sprintf( pszFile, "armor-%d.txt", k );
		FILEOUT( pszFile, "LEVEL\tNAME\t\tMIN-MAX", "" );
		for( i = 0; i < cbArmor; i++ )
		{
			if( apArmor[i]->dwItemJob == k )
			{
				FILEOUT( pszFile, "%d\t%s\t\t%d-%d\r\n", 
					apArmor[i]->dwItemLV, apArmor[i]->szName, apArmor[i]->dwAbilityMin, apArmor[i]->dwAbilityMax );
			}
		}
		*/
	}
	for( i = 0; i < m_aPropAddSkill.GetSize(); i++ )
	{
		AddSkillProp* pAddSkillProp		= m_aPropAddSkill.GetAt( i );
		if( pAddSkillProp )
		{
			FILEOUT( "pas.txt", "%d\t%d\t%d\t%d\t%d\t%d", 
				i,
				pAddSkillProp->dwID,
				pAddSkillProp->dwSkillLvl,
				pAddSkillProp->dwAbilityMin,
				pAddSkillProp->dwAbilityMax,
				pAddSkillProp->dwSkillReady );
		}
	}
/*
	for( i = 0; i < m_aPropSkill.GetSize(); i++ )
	{
		ItemProp* pSkillProp	= m_aPropSkill.GetAt( i );
		if( pSkillProp )
		{
			FILEOUT( "skill.txt", "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
				pSkillProp->dwID, pSkillProp->szName, pSkillProp->dwSpellType, pSkillProp->dwSubDefine,
				pSkillProp->dwReferStat1, pSkillProp->dwReferStat2, pSkillProp->dwReferTarget1, pSkillProp->dwReferTarget2,
				pSkillProp->dwReferValue1, pSkillProp->dwReferValue2 );
		}
	}
*/
	for( i = 0; i < MAX_JOB; i++ )
	{
		for( int j = 0; j < MAX_JOB; j++ )
		{
			ItemProp** apSkillProp	= prj.m_aJobSkill[j];
			int nJobNum	= prj.m_aJobSkillNum[j];

			BOOL b	= FALSE;
			if( j == JOB_VAGRANT || i == j )
				b	= TRUE;
			if( m_aJob[i].dwJobType == JTYPE_PRO && JOB_VAGRANT < j && j < MAX_EXPERT )
			{
				if( j * 2 + 4 == i || j * 2 + 5 == i )
					b	= TRUE;
			}
			if( b )
			{
				for( int k = 0; k < nJobNum; k++ )
				{
					ItemProp* pSkillProp = apSkillProp[k];
					if( ( pSkillProp->dwExeTarget == EXT_MAGICATK || pSkillProp->dwExeTarget == EXT_MAGICATKSHOT ) && pSkillProp->dwSubDefine < 4000 )
					{
						FILEOUT( "skill.txt", "%d\t%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
							i, pSkillProp->dwID, pSkillProp->szName, pSkillProp->dwSpellType, pSkillProp->dwSubDefine,
							pSkillProp->dwReferStat1, pSkillProp->dwReferStat2, pSkillProp->dwReferTarget1, pSkillProp->dwReferTarget2,
							pSkillProp->dwReferValue1, pSkillProp->dwReferValue2 );
					}
				}
			}
		}
	}

	return FALSE;
#endif	// __PRINT_ITEM0718

/*
#ifdef __SHOP_AND_DROP_0802
//1. 상점 아이템
//	- 입력 : 캐릭터 식별자
//	- 출력 : 판매 물품 리스트
//2. 드롭 아이템
//	- 입력 : 몬스터 식별자, 반복자
//	- 출력 : 드롭 물품 리스트

	POSITION pos	= m_mapCharacter.GetStartPosition();
	LPCHARACTER lpCharacter;
	CString string;
	while( pos )
	{
		m_mapCharacter.GetNextAssoc( pos, string, (void*&)lpCharacter );
		FILEOUT( "shop.txt", "[%s]", lpCharacter->m_szKey );
		for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
		{
			int cbSize	= 0;
			ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
			if( lpCharacter->m_venderItemAry[i].GetSize() )
			{

				for( int j = 0; j < lpCharacter->m_venderItemAry[i].GetSize(); j++ )
				{
					LPVENDOR_ITEM pVendor	= (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[i].GetAt( j );
					CPtrArray* pItemKindAry		= GetItemKindAry( pVendor->m_nItemkind3 );
					ItemProp* pItemProp	= NULL;
					
					if( pVendor->m_nUniqueMin < 0 )
					{
						TRACE( "%s, pVendor->m_nUniqueMin < 0\n", lpCharacter->m_szKey );
						continue;
					}
					if( pVendor->m_nUniqueMax < 0 )
					{
						TRACE( "%s, pVendor->m_nUniqueMax < 0\n", lpCharacter->m_szKey );
						continue;
					}

					if( cbSize >= MAX_VENDOR_INVENTORY )
					{
						TRACE( "%s, cbSize >= MAX_VENDOR_INVENTORY\n", lpCharacter->m_szKey );
						continue;
					}
					
					int nMinIdx	= -1, nMaxIdx	= -1;
					
					{
						for( int j = pVendor->m_nUniqueMin; j <= pVendor->m_nUniqueMax; j++ )
						{
							nMinIdx		= prj.GetMinIdx( pVendor->m_nItemkind3, j );
							if( nMinIdx != -1 )
								break;
						}
						for( j = pVendor->m_nUniqueMax; j >= pVendor->m_nUniqueMin; j-- )
						{
							nMaxIdx		= prj.GetMaxIdx( pVendor->m_nItemkind3, j );
							if( nMaxIdx != -1 )
								break;
						}
					}
					
					if( nMinIdx < 0 )
					{
						TRACE( "%s, nMinIdx < 0\n", lpCharacter->m_szKey );
						continue;
					}
					if( nMaxIdx < 0 )
					{
						TRACE( "%s, nMaxIdx < 0\n", lpCharacter->m_szKey );
						continue;
					}

					{
						for( int k = nMinIdx; k <= nMaxIdx; k++ )
						{
							pItemProp	= (ItemProp*)pItemKindAry->GetAt( k );
							if( ( NULL == pItemProp ) ||
								( pItemProp->dwShopAble == (DWORD)-1 ) ||
								( pVendor->m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor->m_nItemJob ) )
								continue;
							
							if( cbSize >= MAX_VENDOR_INVENTORY )
								break;

							apItemProp[cbSize]	= pItemProp;
							cbSize++;
						}
					}

					for( int l = 0; l < cbSize - 1; l++ )
					{
						for( int k = l + 1; k < cbSize; k++ )
						{
							if( ( apItemProp[k]->dwItemKind1 < apItemProp[l]->dwItemKind1 ) ||
								( apItemProp[k]->dwItemKind1 == apItemProp[l]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[l]->dwItemRare ) )
							{
								ItemProp* ptmp	= apItemProp[l];
								apItemProp[l]	= apItemProp[k];
								apItemProp[k]	= ptmp;
							}
						}
					}
				}
			}
			//
			{
				for( int i = 0; i < cbSize; i++ )
				{
					FILEOUT( "shop.txt", " \t%s\t%d\t%d\t%d\t%d", apItemProp[i]->szName, apItemProp[i]->dwLimitLevel1, apItemProp[i]->dwItemLV, apItemProp[i]->dwItemRare, apItemProp[i]->dwLimitJob1 );
				}
			}
		}	// for
	}	// while
	return FALSE;
#endif	// __SHOP_AND_DROP_0802
*/
/*
#ifdef __OUTPUT_INFO_0803
	OutputDropItem();
	return FALSE;
#endif	// __OUTPUT_INFO_0803
*/
#ifdef __ADDSKILLPROP0811
	OutputSkill();
	return FALSE;
#endif	// __ADDSKILLPROP0811
	return TRUE;
}

#ifdef __ADDSKILLPROP0811
void CProject::OutputSkill( void )
{
	for( int i = 0; i < m_aPropSkill.GetSize(); i++ )
	{
		ItemProp* pSkillProp	= m_aPropSkill.GetAt( i );
		if( pSkillProp && pSkillProp->dwID )
		{
			AddSkillProp* pAddSkillProp	= GetAddSkillProp( pSkillProp->dwSubDefine );
			if( pAddSkillProp )
				FILEOUT( "skilltable.txt", "%d->%d", pSkillProp->dwID, pAddSkillProp->dwID );
		}
	}
}
#endif	// __ADDSKILLPROP0811

#ifdef __OUTPUT_INFO_0803
void CProject::OutputStore( const char* lpStrKey, CMover* pMover  )
{
	LPCHARACTER lpCharacter	= GetCharacter( lpStrKey );
	if( !lpCharacter )
		return;

	FILEOUT( "shop.txt", "%s\t%s\t%.2f\t%.2f", pMover->GetName(), lpCharacter->m_szKey, pMover->GetPos().x, pMover->GetPos().z );
	for( int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++ )
	{
		int cbSize	= 0;
		ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
		if( lpCharacter->m_venderItemAry[i].GetSize() )
		{

			for( int j = 0; j < lpCharacter->m_venderItemAry[i].GetSize(); j++ )
			{
				LPVENDOR_ITEM pVendor	= (LPVENDOR_ITEM)lpCharacter->m_venderItemAry[i].GetAt( j );
				CPtrArray* pItemKindAry		= GetItemKindAry( pVendor->m_nItemkind3 );
				ItemProp* pItemProp	= NULL;
				
				if( pVendor->m_nUniqueMin < 0 )
				{
					TRACE( "%s, pVendor->m_nUniqueMin < 0\n", lpCharacter->m_szKey );
					continue;
				}
				if( pVendor->m_nUniqueMax < 0 )
				{
					TRACE( "%s, pVendor->m_nUniqueMax < 0\n", lpCharacter->m_szKey );
					continue;
				}

				if( cbSize >= MAX_VENDOR_INVENTORY )
				{
					TRACE( "%s, cbSize >= MAX_VENDOR_INVENTORY\n", lpCharacter->m_szKey );
					continue;
				}
				
				int nMinIdx	= -1, nMaxIdx	= -1;
				
				{
					for( int j = pVendor->m_nUniqueMin; j <= pVendor->m_nUniqueMax; j++ )
					{
						nMinIdx		= prj.GetMinIdx( pVendor->m_nItemkind3, j );
						if( nMinIdx != -1 )
							break;
					}
					for( j = pVendor->m_nUniqueMax; j >= pVendor->m_nUniqueMin; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pVendor->m_nItemkind3, j );
						if( nMaxIdx != -1 )
							break;
					}
				}
				
				if( nMinIdx < 0 )
				{
					TRACE( "%s, nMinIdx < 0\n", lpCharacter->m_szKey );
					continue;
				}
				if( nMaxIdx < 0 )
				{
					TRACE( "%s, nMaxIdx < 0\n", lpCharacter->m_szKey );
					continue;
				}

				{
					for( int k = nMinIdx; k <= nMaxIdx; k++ )
					{
						pItemProp	= (ItemProp*)pItemKindAry->GetAt( k );
						if( ( NULL == pItemProp ) ||
							( pItemProp->dwShopAble == (DWORD)-1 ) ||
							( pVendor->m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor->m_nItemJob ) )
							continue;
						
						if( cbSize >= MAX_VENDOR_INVENTORY )
							break;

						apItemProp[cbSize]	= pItemProp;
						cbSize++;
					}
				}

				for( int l = 0; l < cbSize - 1; l++ )
				{
					for( int k = l + 1; k < cbSize; k++ )
					{
						if( ( apItemProp[k]->dwItemKind1 < apItemProp[l]->dwItemKind1 ) ||
							( apItemProp[k]->dwItemKind1 == apItemProp[l]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[l]->dwItemRare ) )
						{
							ItemProp* ptmp	= apItemProp[l];
							apItemProp[l]	= apItemProp[k];
							apItemProp[k]	= ptmp;
						}
					}
				}
			}
		}
		//
		{
			for( int i = 0; i < cbSize; i++ )
			{
				FILEOUT( "shop.txt", " \t%s\t%d\t%d\t%d\t%d", apItemProp[i]->szName, apItemProp[i]->dwLimitLevel1, apItemProp[i]->dwItemLV, apItemProp[i]->dwItemRare, apItemProp[i]->dwLimitJob1 );
			}
		}
	}	// for
}

void CProject::OutputDropItem( void )
{
	const	DROP_COUNT	= 1;
	CString sLog[100];
	int nCount	= 0;
	int nWrite	= 0;
	for( int i = 0; i < m_nMoverPropSize; i++ )
	{
		
		if( (++nCount % 10) == 0 )
		{
			FILE* fp;
			if(!(fp = fopen("drop.txt","a")))
				if(!(fp = fopen("drop.txt" ,"wt")))
					return;
			fprintf(fp,sLog[nWrite++]);
		//	fprintf(fp,"\n");
			fclose(fp);
		}
		CString s;
		MoverProp* pMoverProp	= prj.GetMoverProp( i );
		if( pMoverProp && pMoverProp->dwID && pMoverProp->dwBelligerence != BELLI_PEACEFUL && pMoverProp->dwBelligerence != BELLI_ALLIANCE )
		{
			TRACE( "%d\n", i );
//			FILEOUT( "drop.txt", "%s\t%d", pMoverProp->szName, pMoverProp->m_DropItemGenerator.m_dwMax );
			s.Format( "\n%s\t%d", pMoverProp->szName, pMoverProp->m_DropItemGenerator.m_dwMax );
			
//			QUESTITEM
			/*
			int cbQuest	= (int)pMoverProp->m_QuestItemGenerator.GetSize();
			for( int j = 0; j < cbQuest; j++ )
			{
				QUESTITEM* pQuestItem	= pMoverProp->m_QuestItemGenerator.GetAt( j );
				ItemProp* pItemProp	= prj.GetItemProp( pQuestItem->dwIndex );
				FILEOUT( "drop.txt", "QUESTITEM\t%d\t%d\t%s\t%d\t%f", pQuestItem->dwQuest, pQuestItem->dwState, pItemProp->szName, pQuestItem->dwNumber, pQuestItem->dwProbability / 3000000000 );
			}
			*/
//			DROPITEM
//			for( j = 0; j < DROP_COUNT; j++ )
			{
				int cbDropItem	= pMoverProp->m_DropItemGenerator.GetSize();
				int nNumber	= 0;
				DROPITEM* lpDropItem;
				for( int i = 0; i < cbDropItem; i++ )
				{
					if( lpDropItem = pMoverProp->m_DropItemGenerator.GetAt( i, FALSE, false ) )
					{
						if( lpDropItem->dtType == DROPTYPE_NORMAL )
						{
							DWORD dwProbability		= lpDropItem->dwProbability;
							ItemProp* pItemProp	= prj.GetItemProp( lpDropItem->dwIndex );
							if( lpDropItem->dwLevel != 0 )
							{
								if( pItemProp && (int)pItemProp->dwItemLV > 0 )
								{
									int nValue	= dwProbability / pItemProp->dwItemLV;
									if( nValue < 21 )	nValue	= 21;
									dwProbability	= nValue - 20;
								}
							}
//							lpDropItem->dwIndex
//							lpDropItem->dwLevel
//							xRandom( lpDropItem->dwNumber ) + 1;
//							if( lpDropItem->dwNumber != (DWORD)-1 )
//								nNumber++;
//							if( nNumber == lpMoverProp->m_DropItemGenerator.m_dwMax )
//								break;
							if( pItemProp )
							{
//								FILEOUT( "drop.txt", " \t%s\t%f%%\t%d\t%d", 
//									pItemProp->szName, ((double)dwProbability / (double)3000000000)* 100.f, lpDropItem->dwNumber, lpDropItem->dwLevel );
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
//							FILEOUT( "drop.txt", " \tPENYA\t%d-%d", 
//									lpDropItem->dwNumber, lpDropItem->dwNumber2 );
						}
					}
				}
				sLog[nWrite]	+= s;
			}
//			DROPKIND
			{
				int nSize	= pMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				int nAbilityOption, nDropLuck;
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
					for( j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j-- )
					{
						nMaxIdx		= prj.GetMaxIdx( pDropKind->dwIK3, j );
						if( nMaxIdx != -1 )
							break;
					}
					if( nMinIdx < 0 || nMaxIdx < 0 )
						continue;

					for( int a = nMinIdx; a <= nMaxIdx; a++ )
					{
						ItemProp* pItemProp		= (ItemProp*)pItemKindAry->GetAt( a );
						if( !pItemProp )
							continue;

						DWORD dwAdjRand;
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

	FILE* fp;
	if(!(fp = fopen("drop.txt","a")))
		if(!(fp = fopen("drop.txt" ,"wt")))
			return;
	fprintf(fp,sLog[nWrite]);
//	fprintf(fp,"\n");
	fclose(fp);
}
#endif	// __OUTPUT_INFO_0803


void CProject::CloseProject()
{
	if( IsOpenProject() == FALSE ) 
		return;
}
void CProject::ReadConstant( CScript& script )
{
	do {
		script.GetToken(); 
		if( script.Token == _T( "itemDropRate" ) )
		{	
			script.GetToken();// bypass '='
			m_fItemDropRate = script.GetFloat();
			//g_eLocal.SetState( DROP_RATE, 50 );
			//price * g_eLocal.GetState( DROP_RATE) / 100 ;

		}
		else
		if( script.Token == _T( "monsterExpRate" ) )
		{	
			script.GetToken();// bypass '='
			m_fMonsterExpRate = script.GetFloat();

		}
		else
		if( script.Token == _T( "monsterHitRate" ) )
		{	
			script.GetToken();// bypass '='
			m_fMonsterHitRate = script.GetFloat();
		}
		else
		if( script.Token == _T( "goldDropRate" ) )
		{	
			script.GetToken();// bypass '='
			m_fGoldDropRate = script.GetFloat();
		}
		else if( script.Token == _T( "skillExpRate" ) )
		{	
			script.GetToken();// bypass '='
			m_fSkillExpRate	= script.GetFloat();
		}
	} while( script.tok != FINISHED && *script.token != '}' );
}
BOOL CProject::LoadConstant( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;

	do {
		if( script.Token == "lang" )
		{
			int nLang = script.GetNumber();
			if( m_nLanguage == nLang ) 
			{
				script.GetToken(); // {
				do {
	#ifdef __TESTSERVER
					if( script.Token == "test" )
	#else
					if( script.Token == "formula" )
	#endif
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
AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel )
{
	int nskillIndex = dwSubDefine + dwLevel - 1;

	if( nskillIndex < 5)
		nskillIndex += 5;

	if( nskillIndex < 0 || nskillIndex >= m_aPropAddSkill.GetSize() )	
	{
		Error( "CProject::GetAddSkillProp 범위침범. %d", nskillIndex );
		return 0;
	}
	
	return m_aPropAddSkill.GetAt( nskillIndex );
}

AddSkillProp* CProject::GetAddSkillProp( DWORD dwSubDefine )
{
	int nskillIndex = dwSubDefine;
	if( nskillIndex < 5)
		nskillIndex += 5;
	
	if( nskillIndex < 0 || nskillIndex >= m_aPropAddSkill.GetSize() )	
	{
		Error( "CProject::GetAddSkillProp2 범위침범. %d", nskillIndex );
		return 0;
	}
	
	return m_aPropAddSkill.GetAt( nskillIndex );
}


BOOL CProject::LoadText( LPCTSTR lpszFileName )
{
	tagColorText colorText, *pColorText;

	CScript scanner;
	if( scanner.Load( lpszFileName ) == FALSE )
		return FALSE;

	CStringArray strArray;
	CDWordArray  colorArray;
	DWORD dwId = scanner.GetNumber();

	do {
		DWORD dwColor = scanner.GetNumber(); // color
		scanner.GetToken();
		if( *scanner.token  == '{' )
		{
			DWORD dwNation = scanner.GetNumber();
			while( *scanner.token  != '}' )
			{
				scanner.GetLastFull();//Token(); // 스트링 
				CString token = scanner.token;
				token.Replace( "\"", "" );
				if( dwNation == m_nLanguage )
				{
					CString str = token;//scanner.token;
					if( str.IsEmpty() )
						str = "Empty";
#ifdef _DEBUG
					if( strArray.GetSize() > dwId )
						if( strArray.GetAt( dwId ).IsEmpty() == FALSE )
							Message( "CProject::LoadText : 같은 아이디 존재 %d - %s", dwId, str );
#endif					
					strArray.SetAtGrow( dwId, str );
					colorArray.SetAtGrow( dwId, dwColor );
				}
				dwNation = scanner.GetNumber(); // 국가 코드 
			}
		}
		dwId = scanner.GetNumber();
#ifdef _DEBUG
		if( dwId == 925 )
		{
			int a = 0;
		}
#endif		
	} while( scanner.tok != FINISHED );

	for( DWORD i = 0; i < strArray.GetSize(); i++ )
	{
		if( strArray.GetAt( i ).IsEmpty() == FALSE )
		{
/*
			m_colorText.SetAtGrow( i, &colorText, sizeof( colorText ) + strArray.GetAt( i ).GetLength() + 1 );
			pColorText = m_colorText.GetAt( i );
			pColorText->dwColor = colorArray.GetAt( i );
			_tcscpy( pColorText->szData, strArray.GetAt( i ) );
*/
			m_colorText.SetAtGrow( i, &colorText );

			pColorText = m_colorText.GetAt( i );
			pColorText->dwColor = colorArray.GetAt( i );
			pColorText->lpszData = strdup( strArray.GetAt( i ) ) ;
		}
	}
	m_colorText.Optimize();
	return TRUE;
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
				script.GetToken(  );	// korean Name 
				if( m_nLanguage == LANG_KOR ) _tcscpy( m_aJob[ id ].szName, script.token );
				script.GetToken(  );	// english Name 
				if( m_nLanguage == LANG_ENG ) _tcscpy( m_aJob[ id ].szName, script.token );
				script.GetToken(  ); // Japan Name
				if( m_nLanguage == LANG_JAP ) _tcscpy( m_aJob[ id ].szName, script.token );
				script.GetToken(  ); // Chinese Name
				if( m_nLanguage == LANG_CHI ) _tcscpy( m_aJob[ id ].szName, script.token );
				script.GetToken(  ); // Thailand  Name
				if( m_nLanguage == LANG_THA ) _tcscpy( m_aJob[ id ].szName, script.token );
				script.GetToken(); // Taiwan  Name
				if( m_nLanguage == LANG_TWN ) _tcscpy( m_aJob[ id ].szName, script.token );

				//script.GetToken(); // name
				//strcpy( m_aJob[ id ].szName, script.token );
				//script.GetToken(); // ename
				//strcpy( m_aJob[ id ].szEName, script.token );

				m_aJob[ id ].dwJobBase = script.GetNumber();
				m_aJob[ id ].dwJobType = script.GetNumber();
				id = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "structure" ) )
		{	
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{

				script.GetToken(  );	// korean Name 
				if( m_nLanguage == LANG_KOR ) _tcscpy( m_aStructure[ id ].szName, script.token );
				script.GetToken(  );	// english Name 
				if( m_nLanguage == LANG_ENG ) _tcscpy( m_aStructure[ id ].szName, script.token );
				_tcscpy( m_aStructure[ id ].szEName, script.token );
				script.GetToken(  ); // Japan Name
				if( m_nLanguage == LANG_JAP ) _tcscpy( m_aStructure[ id ].szName, script.token );
				script.GetToken(  ); // Chinese Name
				if( m_nLanguage == LANG_CHI ) _tcscpy( m_aStructure[ id ].szName, script.token );
				script.GetToken(  ); // Thailand  Name
				if( m_nLanguage == LANG_THA ) _tcscpy( m_aStructure[ id ].szName, script.token );
				script.GetToken(); // Taiwan  Name
				if( m_nLanguage == LANG_TWN ) _tcscpy( m_aStructure[ id ].szName, script.token );
				//script.GetToken(); // name
				//strcpy( m_aStructure[ id ].szName, script.token );
				//script.GetToken(); // ename
				//strcpy( m_aStructure[ id ].szEName, script.token );
				id = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "Guild" ) )
		{
			script.GetToken(); // skip {
			id = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken(  );	// korean Name 
				if( m_nLanguage == LANG_KOR ) _tcscpy( m_aGuildAppell[ id ].szName, script.token );
				script.GetToken(  );	// english Name 
				if( m_nLanguage == LANG_ENG ) _tcscpy( m_aGuildAppell[ id ].szName, script.token );
				script.GetToken(  ); // Japan Name
				if( m_nLanguage == LANG_JAP ) _tcscpy( m_aGuildAppell[ id ].szName, script.token );
				script.GetToken(  ); // Chinese Name
				if( m_nLanguage == LANG_CHI ) _tcscpy( m_aGuildAppell[ id ].szName, script.token );
				script.GetToken(  ); // Thailand  Name
				if( m_nLanguage == LANG_THA ) _tcscpy( m_aGuildAppell[ id ].szName, script.token );
				script.GetToken(); // Taiwan  Name
				if( m_nLanguage == LANG_TWN ) _tcscpy( m_aGuildAppell[ id ].szName, script.token );
				id = script.GetNumber();
			}
		}
	} while( script.tok != FINISHED );
	return TRUE;
}

BOOL CProject::LoadMotionProp( LPCTSTR lpszFileName )
{// 모션 인덱스  요구Lv  요구경험치  표시단어 루트명 연결단어 
	CScript script;
	if( script.Load( lpszFileName, FALSE ) == FALSE )
		return FALSE;
	MotionProp MotionProp;
	int i = script.GetNumber();  // id
	while( script.tok != FINISHED )
	{
		//pMotionProp = &m_aMotionProp[ i ];
		ZeroMemory( &MotionProp, sizeof( MotionProp ) );
		MotionProp.dwID = i;
		MotionProp.dwMotion = script.GetNumber();
		script.GetToken();  
		script.GetToken();  
		if( strlen(script.m_mszToken) >= sizeof(MotionProp.szIconName) )
			Error( "LoadMotionProp : 메모리침범. %s 버퍼크기(%d), 스트링길이(%d)", script.m_mszToken, sizeof(MotionProp.szIconName), strlen(script.m_mszToken) );
		strcpy( MotionProp.szIconName, script.m_mszToken );
		script.GetToken();  
		MotionProp.dwPlay = (DWORD)script.GetNumber();

		//script.GetToken();  
		//strcpy( MotionProp.szName, script.Token );

		script.GetToken();  
		script.GetToken();	// korean Name 
		if( m_nLanguage == LANG_KOR ) _tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken();	// english Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();  
		script.GetToken();
		script.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();
		//script.GetToken();  
		//strcpy( MotionProp.szDesc, script.Token );

		script.GetToken();  
		script.GetToken();	// korean Name 
		if( m_nLanguage == LANG_KOR ) _tcscpy( MotionProp.szDesc, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken();	// english Name 
		if( m_nLanguage == LANG_ENG ) _tcscpy( MotionProp.szDesc, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken(); // Japan Name
		if( m_nLanguage == LANG_JAP ) _tcscpy( MotionProp.szDesc, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken(); // Chinese Name
		if( m_nLanguage == LANG_CHI ) _tcscpy( MotionProp.szDesc, script.m_mszToken );
		script.GetToken();  
		script.GetToken();  
		script.GetToken(); // Thailand  Name
		if( m_nLanguage == LANG_THA ) _tcscpy( MotionProp.szDesc, script.m_mszToken );
		script.GetToken();  		
		script.GetToken();
		script.GetToken(); // Taiwan  Name
		if( m_nLanguage == LANG_TWN ) _tcscpy( MotionProp.szDesc, script.m_mszToken );
		script.GetToken();

		//MotionProp.dwAction = script.GetNumber();      
		
	
		/*
		MotionProp.dwRequireLv   = script.GetNumber( TRUE );            
		MotionProp.dwRequireExp  = script.GetNumber( TRUE );      
		script.GetToken( TRUE );  // Root
		_tcscpy( MotionProp.szRoot, script.Token );
		script.GetToken( TRUE );  // Name 
		_tcscpy( MotionProp.szName, script.Token );
		script.GetToken( TRUE );  // Link
		_tcscpy( MotionProp.szLink, script.Token );
		*/
		m_aPropMotion.SetAtGrow( i, &MotionProp);
		i = script.GetNumber(); 
	}
	m_aPropMotion.Optimize();
	return TRUE;
}
#ifdef __CLIENT
BOOL CProject::LoadHelp( LPCTSTR lpszFileName )
{
	CScript scanner;
	if(scanner.Load(lpszFileName)==FALSE)
		return FALSE;

	CString strKeyword;
	int nLang;
	nLang = scanner.GetNumber(); // language
	int n = 0;
	do {
		scanner.GetLastFull(); // keyword
		if( nLang == g_Option.m_nLanguage )
			strKeyword = scanner.token;
		nLang = scanner.GetNumber(); // language
		if( *scanner.token == '{' ) // {이면 string은 keyword이다 
		{
			CString string = "   ";
#ifdef _DEBUG			
			if( scanner.GetLineNum() > 255 )
			{
				int a = 0;
			}
#endif
			nLang = scanner.GetNumber(); // language
			do
			{
				scanner.GetLastFull(); // keyword
				n++;
#ifdef _DEBUG			
				if( n == 141 )
				{
					int a = 0;
				}
#endif
				if( nLang == g_Option.m_nLanguage )
				{
					string += scanner.Token;
					string += "\n\n   ";
				}
				nLang = scanner.GetNumber(); // language
#ifdef _DEBUG			
				if( scanner.GetLineNum() > 255 )
				{
					int a = 0;
				}
#endif 
			} while( *scanner.token != '}' );

			m_mapHelp.SetAt( strKeyword, string );
			/*
			m_mapFAQ.SetAt( strKeyword, string );
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LIST1 );
			CString strFAQ;
			strFAQ = "Q.";
			strFAQ += strKeyword;
			pWndListBox->AddString( strFAQ );
			*/
			
			nLang = scanner.GetNumber(); // language
#ifdef _DEBUG			
			if( scanner.GetLineNum() > 255 )
			{
				int a = 0;
			}
#endif

		}
		//	nLang = scanner.GetNumber(); // language
		//scanner.GetToken(); // keyword
		
	} while( scanner.tok != FINISHED );
/*	





	CString strKeyword;
	scanner.GetToken(); // keyword
	do {
		strKeyword = scanner.token;
		CString string;
		scanner.GetToken(); 
		if( *scanner.token == '{' ) // {이면 string은 keyword이다 
		{
			scanner.GetToken(); // keyword
			do
			{
				string += scanner.Token;
				scanner.GetToken(); // keyword
			} while( *scanner.token != '}' );
			scanner.GetToken(); // keyword
		}
		m_mapHelp.SetAt( strKeyword, string );
	} while( scanner.tok != FINISHED );
	*/
	return TRUE;
}
CString CProject::GetHelp( LPCTSTR lpStr )
{
	/*
	CStringArray* pStrArray;
	if( m_mapHelp.Lookup( lpStr, (void*&)pStrArray ) )
		return pStrArray;
	return NULL;
	*/
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
		strcpy( filter.m_szSrc, scanner.Token );
		scanner.GetToken();
		strcpy( filter.m_szDst, scanner.Token );
		m_aWordFilter.Add( &filter );
		scanner.GetToken(); 
	}
	m_aWordFilter.Optimize();
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
		strWord = scanner.Token;//strcpy( filter.m_szSrc, scanner.Token );
		scanner.GetToken();
		m_mapWordToolTip.SetAt( strWord, scanner.token );
		scanner.GetToken(); 
	}
	return TRUE;
}
#endif

CString CProject::GetLangScript( CScript& script )
{
	CString string;
	int nLang = 0;
	do
	{
		nLang = script.GetNumber(); // lang
		script.GetToken(); // ,
		if( *script.token != ',' )
			script.SntxErr( NULL, 0 );
		script.GetToken(); // string
		if( m_nLanguage == nLang )
			string = script.Token;
		script.GetToken(); // ,
		if( *script.token != ',' && *script.token != ')' )
			script.SntxErr( NULL, 0 );
	} while( *script.token != ')' );
	script.GetToken(); // ;
	if( string.IsEmpty() )
		string = " ";
	return string;
}


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
				lstrcpy( prop.szTitle, s.Token );
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
		//		else if( s.Token == "time" )
		//		{
		//			s.GetToken();	// =
		//			prop.dwTime		= s.GetNumber();
		//		}
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
						lstrcpy( prop.szDesc[nState], s.Token );
					}
					s.GetToken();	// key or anything
				}
			}
			s.GetToken();
		}
		m_aPropGuildQuest.SetAtGrow( nQuestId, &prop );
		nQuestId	= s.GetNumber();	// id
	}
	m_aPropGuildQuest.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropQuest_All( LPCTSTR lpszFileName )
{
	LoadPropQuest( lpszFileName, FALSE );
	struct _finddata_t c_file;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpszFileName, drive, dir, name, ext );
	CString string;
	string.Format( "%s%s-*%s", dir, name, ext );
	/* Find first .c file in current directory */
	CFileFinder ff;
	if( ff.FindFirst( string, &c_file ) )
	{
		if( !(c_file.attrib & _A_SUBDIR ) )
		{
			string.Format( "%s%s", dir, c_file.name );
			LoadPropQuest( string, FALSE );
		}
		/* Find the rest of the .c files */
		while( ff.FindNext( &c_file ) )
		{
			//strcpy( name, dir );
			//strcat( name, c_file.name );
			if( !(c_file.attrib & _A_SUBDIR ) )
			{
				string.Format( "%s%s", dir, c_file.name );
				LoadPropQuest( string, FALSE );
			}
		}
		ff.FindClose();
	}	
	m_aPropQuest.Optimize();
	return TRUE;
}
BOOL CProject::LoadPropQuest( LPCTSTR lpszFileName, BOOL bOptimize )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	QuestProp propQuest;
	TCHAR szLinkChar[ 10 ][ 64 ];
	int szLinkCharNum;
	int szEndCondCharNum;
	int nQuest = script.GetNumber();  // id
	while( script.tok != FINISHED )
	{
		ZeroMemory( &propQuest, sizeof( propQuest ) );
#ifdef __V050322_CACHEQUEST
		BOOL bEndCondCharacter = FALSE;
		propQuest.m_nBeginCondSex = -1;
		propQuest.m_wId = nQuest;

		QuestPropItem aEndRewardItem[ MAX_QUESTCONDITEM ]; 
		int nEndRewardItemNum = 0;
		ZeroMemory( aEndRewardItem, sizeof( aEndRewardItem ) );

		QuestPropItem aBeginCondItem[ MAX_QUESTCONDITEM ]; 
		int nBeginCondItemNum = 0;
		ZeroMemory( aBeginCondItem, sizeof( aBeginCondItem ) );

		QuestPropItem aEndCondItem[ MAX_QUESTCONDITEM ]; 
		int nEndCondItemNum = 0;
		ZeroMemory( aEndCondItem, sizeof( aEndCondItem ) );

		ZeroMemory( szLinkChar, sizeof( szLinkChar ) );
		szLinkCharNum = 0;
		szEndCondCharNum = 0;

#if defined( __SINGLE ) || defined( __WORLDSERVER ) 
		ZeroMemory( propQuest.m_apQuestDialog, sizeof( propQuest.m_apQuestDialog ) );
#endif
#else
		//propQuest.m_nBeginCondPreviousQuest = -1;
		propQuest.m_nBeginCondLevelMin = -1;
		propQuest.m_nBeginCondLevelMax = -1;
		propQuest.m_nBeginCondGroup = 0;
		propQuest.m_nBeginCondGroupNum = 0;
		ZeroMemory( propQuest.m_apQuestDialog, sizeof( propQuest.m_apQuestDialog ) );
#endif
		//i = script.GetNumber(); 
		script.GetToken();
		int nBlock = 1;
		while( nBlock && script.tok != FINISHED )
		{
			script.GetToken();
			if( script.Token == "{" )			
				nBlock++;
			else
			if( script.Token == "}" )			
				nBlock--;
			else
			if( script.Token == "SetTitle" )
			{
				script.GetToken(); // (
				strcpy(	propQuest.m_szTitle, GetLangScript( script ) );
			}
			else
			if( script.Token == "m_szTitle" )
			{
				script.GetToken(); // (
				script.GetToken();
				strcpy(	propQuest.m_szTitle, script.Token );
			}
			else
			if( script.Token == "SetNPCName" )
			{
				script.GetToken(); // (
//				GetLangScript( script );
				strcpy(	propQuest.m_szNpcName, GetLangScript( script ) );
			}
			else
			if( script.Token == "m_szNPC" )
			{
				script.GetToken();
				script.GetToken();
				//strcpy(	propQuest.m_szNpcName, script.Token );
			}
			else
#ifdef __V050322_CACHEQUEST
			if( script.Token == "SetCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // key
				szLinkCharNum = 0;
				strcpy( szLinkChar[ szLinkCharNum ], script.token ); 
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum++ ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awSrcQuest.Add( nQuest );
					lpCharacter->m_anSrcQuestItem.Add( 0 );
				}
			}
			else
			if( script.Token == "SetMultiCharacter" )
			{
				script.GetToken(); // (
				szLinkCharNum = 0;
				do
				{
					script.GetToken(); // key
					strcpy( szLinkChar[ szLinkCharNum ], script.token ); 
					script.GetToken(); // ,
					int nItem = script.GetNumber();
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ szLinkCharNum ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awSrcQuest.Add( nQuest );
						lpCharacter->m_anSrcQuestItem.Add( nItem );
					}
					else
					{
						Error( "%s(%d) : %s가 존재하지 않음", lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
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
				propQuest.m_nBeginCondSex  = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondSkillLvl" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondSkillIdx = script.GetNumber(); 
				script.GetToken(); // ,
				propQuest.m_nBeginCondSkillLvl = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondKarmaPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondKarmaComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondKarmaPoint = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondChaotic" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondChaotic = script.GetNumber();
			}
			else
			if( script.Token == "SetBeginCondLevel" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondLevelMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondLevelMax = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondParty" ) 
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondParty = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondPartyNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondPartyNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nBeginCondPartyLeader = script.GetNumber(); 
			}
			else 
			if( script.Token == "SetBeginCondGuild" ) 
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondGuild = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGuildNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGuildNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nBeginCondGuildLeader = script.GetNumber(); 
			}
			else
			if( script.Token == "SetBeginCondJob" )
			{
				script.GetToken(); // (
				do
				{
					propQuest.m_nBeginCondJob[ propQuest.m_nBeginCondJobNum++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && propQuest.m_nBeginCondJobNum < MAX_JOB );
			}
			else
			if( script.Token == "SetBeginCondPreviousQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondPreviousQuestType = script.GetNumber(); // 0 시작중, 1 이전 퀘스트 완료
				script.GetToken(); // ,
				int nCnt = 0;
				do
				{
					propQuest.m_anBeginCondPreviousQuest[ nCnt++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && nCnt < 6 );
			}
			else
			if( script.Token == "SetBeginCondExclusiveQuest" )
			{
				script.GetToken(); // (
				int nCnt = 0;
				do
				{
					propQuest.m_anBeginCondExclusiveQuest[ nCnt++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && nCnt < 6 );
			}
			else
			if( script.Token == "SetBeginCondItem" )
			{
				script.GetToken(); // (
#ifdef __V050524_QUEST
				int nSex = script.GetNumber();
				script.GetToken(); // ,
#endif
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetBeginCondItem 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
#ifdef __V050524_QUEST
				aBeginCondItem[ nBeginCondItemNum ].m_nSex = nSex;
				aBeginCondItem[ nBeginCondItemNum ].m_nType = nType;
				aBeginCondItem[ nBeginCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aBeginCondItem[ nBeginCondItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_nItemNum = script.GetNumber();
#else
//{{AFX
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetBeginCondItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_nType = nType;
				aBeginCondItem[ nBeginCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aBeginCondItem[ nBeginCondItemNum ].m_anItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				aBeginCondItem[ nBeginCondItemNum ].m_anItemNum[ nIdx ] = script.GetNumber();
//}}AFX
#endif
				nBeginCondItemNum++;
			}
			else
			if( script.Token == "SetBeginCondDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondDisguiseMoverIndex = script.GetNumber();
			}
			else
			//////////////////////////////////////////////////////////////////
			// 시작 - 추가 및 변경 
			//////////////////////////////////////////////////////////////////
			if( script.Token == "SetBeginSetDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginSetDisguiseMoverIndex = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginSetAddGold" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginSetAddGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginSetAddItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetBeginSetAddItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nBeginSetAddItemIdx[nIdx] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginSetAddItemNum[nIdx] = script.GetNumber();
			}
			else 
			////////////////////////// 종료 - 조건 
			if( script.Token == "SetEndCondParty" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondParty = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondPartyNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondPartyNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nEndCondPartyLeader = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondGuild" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondGuild = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondGuildNumComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondGuildNum = script.GetNumber();
				script.GetToken(); // , 
				propQuest.m_nEndCondGuildLeader = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondState" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondState = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondCompleteQuest" ) 
			{
				script.GetToken(); // (
				propQuest.m_nEndCondCompleteQuestOper = script.GetNumber();
				script.GetToken(); // ,
				int nIdx = 0;
				while( *script.token != ')' )
				{
					propQuest.m_nEndCondCompleteQuest[ nIdx++ ] = script.GetNumber();
					script.GetToken(); // ,
				}
			}
			else
			if( script.Token == "SetEndCondSkillLvl" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondSkillIdx = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondSkillLvl = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondKarmaPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondKarmaComp = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondKarmaPoint = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondChaotic" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondChaotic = script.GetNumber();
			}
			else
			if( script.Token == "SetEndCondLimitTime" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondLimitTime = script.GetNumber();
			}
			else
			// SetEndCondItem( int nIdx, DWORD dwItemIdx, DWORD dwItemNum );
			if( script.Token == "SetEndCondItem" )
			{
				script.GetToken(); // (
#ifdef __V050524_QUEST
				int nSex = script.GetNumber();
				script.GetToken(); // ,
#endif
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndCondItem 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
#ifdef __V050524_QUEST
				aEndCondItem[ nEndCondItemNum ].m_nSex = nSex;
				aEndCondItem[ nEndCondItemNum ].m_nType = nType;
				aEndCondItem[ nEndCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndCondItem[ nEndCondItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndCondItem[ nEndCondItemNum ].m_nItemNum = script.GetNumber();
#else
//{{AFX
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndCondItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				aEndCondItem[ nEndCondItemNum ].m_nSex = nSex;
				aEndCondItem[ nEndCondItemNum ].m_nType = nType;
				aEndCondItem[ nEndCondItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndCondItem[ nEndCondItemNum ].m_anItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				aEndCondItem[ nEndCondItemNum ].m_anItemNum[ nIdx ] = script.GetNumber();
//}}AFX
#endif
				nEndCondItemNum++;
			}
			else 
			// SetEndCondKillNPC( int nIdx, DWORD dwNPCIdx, DWORD dwNPCNum );
			if( script.Token == "SetEndCondKillNPC" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 1 )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndCondKillNPC에서 Idx 값 범위 초과(0~1)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nEndCondKillNPCIdx[nIdx] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondKillNPCNum[nIdx] = script.GetNumber();
			}
			else
			// SetEndCondPatrol( DWORD dwWorldID, int left, int top, int right, int bottom );
			if( script.Token == "SetEndCondPatrolZone" )
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
			}
			else
			if( script.Token == "SetEndCondCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // "key"
				// 스트링이 없으면 szLinkChar에 세팅된 것으로 대체한다. 즉 자기 자신이다.
				if( strlen( script.token ) == 0 )
					strcpy( propQuest.m_szEndCondCharacter, szLinkChar[ 0 ] );
				else
					strcpy( propQuest.m_szEndCondCharacter, script.token );
				LPCHARACTER lpCharacter = GetCharacter( propQuest.m_szEndCondCharacter );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( nQuest );
					lpCharacter->m_anDstQuestItem.Add( 0 );
				}
				bEndCondCharacter = TRUE;
			}
			else
			if( script.Token == "SetEndCondMultiCharacter" )
			{
				script.GetToken(); // (
				szEndCondCharNum = 0;
				propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
				ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
				do
				{
					script.GetToken(); // key
					strcpy( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ], script.token ); 

					script.GetToken(); // ,
					int nItem = script.GetNumber();

					LPCHARACTER lpCharacter = GetCharacter( &propQuest.m_lpszEndCondMultiCharacter[ szEndCondCharNum * 64 ] );
					if( lpCharacter ) 
					{
						lpCharacter->m_awDstQuest.Add( nQuest );
						lpCharacter->m_anDstQuestItem.Add( nItem );
					}
					else
					{
						Error( "%s(%d) : %s가 존재하지 않음", lpszFileName, script.GetLineNum(), szLinkChar[ szLinkCharNum ] );
					}
					szEndCondCharNum++;
					script.GetToken(); // , or )
				} while( *script.token != ')' && szEndCondCharNum < 10 );
				bEndCondCharacter = TRUE;
			}
			else
			if( script.Token == "SetEndCondDialog" )
			{
				script.GetToken(); // (
				script.GetToken(); 
				strcpy( propQuest.m_szEndCondDlgCharKey, script.token );
				script.GetToken(); // ,
				script.GetToken(); 
				strcpy( propQuest.m_szEndCondDlgAddKey, script.token );
			}
			else
			if( script.Token == "SetEndCondDisguise" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondDisguiseMoverIndex = script.GetNumber();
			}
			else
			if( script.Token == "SetParam" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetParam Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nParam[ nIdx ] = script.GetNumber();
			}
			else
			////////////////////////// 대화 - 보상 
			if( script.Token == "SetDlgRewardItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetDlgRewardItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nDlgRewardItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nDlgRewardItemNum[ nIdx ] = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardItem" )
			{
				script.GetToken(); // ( 
#ifdef __V050524_QUEST
				int nSex = script.GetNumber();
				script.GetToken(); // ,
#endif
				int nType = script.GetNumber();
				script.GetToken(); // ,
				int nJobOrItem = script.GetNumber();
				if( nType == 0 && nJobOrItem >= MAX_QUESTCONDITEM )
				{
					nJobOrItem = -1;
					Error( "%s(%d) SetEndRewardItem 값 범위 초과(0~%d)", lpszFileName, script.GetLineNum(), MAX_QUESTCONDITEM ); 
				}
				script.GetToken(); // ,
#ifdef __V050524_QUEST
				aEndRewardItem[ nEndRewardItemNum ].m_nSex = nSex;
				aEndRewardItem[ nEndRewardItemNum ].m_nType = nType;
				aEndRewardItem[ nEndRewardItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndRewardItem[ nEndRewardItemNum ].m_nItemIdx = script.GetNumber();
				script.GetToken(); // ,
				aEndRewardItem[ nEndRewardItemNum ].m_nItemNum = script.GetNumber();
#else
//{{AFX
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "SetEndRewardItem Idx 값 범위 초과(0~3)" ); 
				}
				script.GetToken(); // ,
				aEndRewardItem[ nEndRewardItemNum ].m_nType = nType;
				aEndRewardItem[ nEndRewardItemNum ].m_nJobOrItem =  nJobOrItem;
				aEndRewardItem[ nEndRewardItemNum ].m_anItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				aEndRewardItem[ nEndRewardItemNum ].m_anItemNum[ nIdx ] = script.GetNumber();

//}}AFX
#endif				
				nEndRewardItemNum++;
			}
			else
			if( script.Token == "SetEndRewardGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardGoldMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardGoldMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardExp" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardExpMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardExpMax = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardKarmaPoint" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRewardKarmaStyle = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRewardKarmaPoint = script.GetNumber();
			}
			else
			if( script.Token == "SetEndRewardHide" )
			{
				script.GetToken(); // (
				propQuest.m_bEndRewardHide = ( script.GetNumber() != 0 );
			}
			else
			// 종료 퀘스트 제거 
			if( script.Token == "SetEndRemoveQuest" )
			{
				script.GetToken(); // (
				int nRemoveQuestIdx = 0;
				do
				{
					propQuest.m_anEndRemoveQuest[ nRemoveQuestIdx++ ] = script.GetNumber();;
					script.GetToken(); // , or )
				} while( *script.token != ')' && nRemoveQuestIdx < MAX_QUESTREMOVE );
			}
			else
			// 종료 아이템 제거 
			if( script.Token == "SetEndRemoveItem" )
			{
				script.GetToken(); // (
				int nIdx = script.GetNumber();
				if( nIdx < 0 || nIdx > 3 )
				{
					nIdx = 0;
					Error( "%s(%d) SetEndRemoveItem Idx 값 범위 초과(0~3)", lpszFileName, script.GetLineNum() ); 
				}
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemIdx[ nIdx ] = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemNum[ nIdx ] = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndRemoveGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetRepeat" )
			{
				script.GetToken(); // (
				propQuest.m_bRepeat = ( script.GetNumber() != 0 );
			}
			else 
			if( script.Token == "QuestItem" )
			{
				QUESTITEM qi;
				script.GetToken();	// (
				DWORD dwMoverIdx = script.GetNumber();
				qi.dwQuest	= nQuest;//script.GetNumber();
				qi.dwState	= 0;//script.GetNumber();
				script.GetToken();	// ,
				qi.dwIndex	= script.GetNumber();
				ASSERT( qi.dwIndex );
				script.GetToken();	// ,
				qi.dwProbability	= script.GetNumber();	//
				script.GetToken();	// ,
				qi.dwNumber	= script.GetNumber();	//
				script.GetToken();	// )
				MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
				pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
			}
			else
#else // __V050322_CACHEQUEST
//{{AFX
			if( script.Token == "SetLinkCharacter" )
			{
				script.GetToken(); // (
				script.GetToken(); // key
				strcpy( propQuest.m_szLinkChar, script.token );
#ifdef __V050322_CACHEQUEST
				LPCHARACTER lpCharacter = GetCharacter( propQuest.m_szLinkChar );
				if( lpCharacter ) lpCharacter->m_awSrcQuest.Add( nQuest );
#endif
			}
			// 시작 - 조건 
			if( script.Token == "SetBeginCondLevel" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondLevelMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondLevelMax = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondGroup" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondGroup = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondGroupNum = script.GetNumber();
			}
			else
			if( script.Token == "SetBeginCondJob" )
			{
				script.GetToken(); // (
				do
				{
					propQuest.m_nBeginCondJob[ propQuest.m_nBeginCondJobNum++ ] = script.GetNumber();
					script.GetToken(); // , or )
				} while( *script.token != ')' && propQuest.m_nBeginCondJobNum < MAX_JOB );

			}
			else
			if( script.Token == "SetBeginCondPreviousQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondPreviousQuest = script.GetNumber();
			}
			else
			if( script.Token == "SetBeginCondItem1" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondItem1 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondItemNum1 = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondItem2" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondItem2 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondItemNum2 = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginCondItem3" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginCondItem3 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginCondItemNum3 = script.GetNumber();
			}
			////////////////////////// 시작 - 추가 
			if( script.Token == "SetBeginAddItem1" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginAddItem1 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginAddItemNum1 = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginAddItem2" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginAddItem2 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginAddItemNum2 = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginAddItem3" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginAddItem3 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nBeginAddItemNum3 = script.GetNumber();
			}
			else 
			if( script.Token == "SetBeginAddGold" )
			{
				script.GetToken(); // (
				propQuest.m_nBeginAddGold = script.GetNumber();
			}
			else 
			////////////////////////// 종료 - 조건 
			if( script.Token == "SetEndCondItem1" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondItem1 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondItemNum1 = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondItem2" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondItem2 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondItemNum2 = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndCondItem3" )
			{
				script.GetToken(); // (
				propQuest.m_nEndCondItem3 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndCondItemNum3 = script.GetNumber();
			}
			else
			////////////////////////// 종료 - 제거 
			if( script.Token == "SetEndRemoveItem1" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveItem1 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemNum1 = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndRemoveItem2" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveItem2 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemNum2 = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndRemoveItem3" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveItem3 = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nEndRemoveItemNum3 = script.GetNumber();
			}
			else 
			if( script.Token == "SetEndRemoveGold" )
			{
				script.GetToken(); // (
				propQuest.m_nEndRemoveGold = script.GetNumber();
			}
			else 
			if( script.Token == "SetParam1" )
			{
				script.GetToken(); // (
				propQuest.m_nParam1 = script.GetNumber();
			}
			else
			if( script.Token == "SetParam2" )
			{
				script.GetToken(); // (
				propQuest.m_nParam2 = script.GetNumber();
			}
			else
			if( script.Token == "SetParam3" )
			{
				script.GetToken(); // (
				propQuest.m_nParam3 = script.GetNumber();
			}
			else
			if( script.Token == "SetParam4" )
			{
				script.GetToken(); // (
				propQuest.m_nParam4 = script.GetNumber();
			}
			else
			if( script.Token == "SetRewardItem" )
			{
				script.GetToken(); // (
				propQuest.m_nRewardItemIdx = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nRewardItemMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nRewardItemMax = script.GetNumber();
			}
			else
			if( script.Token == "SetRewardGold" )
			{
				script.GetToken(); // (
				propQuest.m_nRewardGoldMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nRewardGoldMax = script.GetNumber();
			}
			else
			if( script.Token == "SetRewardExp" )
			{
				script.GetToken(); // (
				propQuest.m_nRewardExpMin = script.GetNumber();
				script.GetToken(); // ,
				propQuest.m_nRewardExpMax = script.GetNumber();
			}
			else
//}}AFX
#endif // __V050322_CACHEQUEST
			//////////////////////////
#ifdef __V050322_CACHEQUEST 
			if( script.Token == "SetDialog" )
			{
				script.GetToken(); // (
				int nNum = script.GetNumber();
				script.GetToken(); // ,
				CString string = GetLangScript( script );
#if defined( __SINGLE ) || defined( __WORLDSERVER )
				if( nNum < 32 )
				{
					propQuest.m_apQuestDialog[ nNum ] = new CHAR[ string.GetLength() + 1 ];
					strcpy(	propQuest.m_apQuestDialog[ nNum ], string );
				}
#endif 
			}
			if( script.Token == "SetPatrolZoneName" )
			{
				script.GetToken(); // (
				//int nNum = script.GetNumber();
				//script.GetToken(); // ,
				CString string = GetLangScript( script );
				if( string.GetLength() < 64 )
					strcpy( propQuest.m_szPatrolZoneName, string );
				else
					Error( "%s(%d) SetPatrolZoneName 스트링 64바이트 초과", lpszFileName, script.GetLineNum() ); 
			}
#else
//{{AFX
			if( script.Token == "SetDialog" )
			{
				script.GetToken(); // (
				int nNum = script.GetNumber();
				script.GetToken(); // ,
				CString string = GetLangScript( script );
				if( nNum < 32 )
				{
					propQuest.m_apQuestDialog[ nNum ] = new CHAR[ string.GetLength() + 1 ];
					strcpy(	propQuest.m_apQuestDialog[ nNum ], string );
				}
			}
//}}AFX
#endif			
			else
			if( script.Token == "SetHeadQuest" )
			{
				script.GetToken(); // (
				propQuest.m_nHeadQuest = script.GetNumber();
			}
			else
			if( script.Token == "SetQuestType" )
			{
				script.GetToken(); // (
				propQuest.m_nQuestType = script.GetNumber();
			}
			else
			if( script.Token == "SetRemove" )
			{
				script.GetToken(); // (
				propQuest.m_bNoRemove = !script.GetNumber();
			}
			else
			if( script.Token == "state" )
			{
				int nState = script.GetNumber();
#if defined( __WRITE_STATE_QUEST ) || defined( __CHECK_STATE_QUEST )
				if( nState != 0 && nState != QS_END )
				{
					for( int i = 0; i < m_aStateQuest.GetSize(); i++ )
					{
						if( m_aStateQuest.GetAt( i ) == nQuest )
							break;
					}
#			ifdef __WRITE_STATE_QUEST  
					if( i == m_aStateQuest.GetSize() )
						m_aStateQuest.Add( nQuest );
#			else
#			ifdef __CHECK_STATE_QUEST
					if( i == m_aStateQuest.GetSize() )
					{
						CString strQuest;
						strQuest.Format( "%d Quest에서 %d State 사용됐음! 누가 Quest에서 State 사용하라고 했어! 새 퀘스트에는 사용하지좀 마!", nQuest, nState );
						AfxMessageBox( strQuest );
					}
#			endif
#			endif
				}
#endif
#ifdef __V050322_CACHEQUEST 
				QuestState* pQuestState = new QuestState;
				propQuest.m_questState[ nState ] = pQuestState;
				pQuestState->m_szDesc[ 0 ] = 0;
#else // __V050322_CACHEQUEST

				//int i = script.GetNumber();  // id
				QuestState* pQuestState = &propQuest.m_questState[ nState ];

#endif // __V050322_CACHEQUEST
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
						MoverProp* pMoverProp = GetMoverProp( dwMoverIdx );
						pMoverProp->m_QuestItemGenerator.AddTail( qi );	// copy
					}
					else
					if( script.Token == "SetDesc" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
						if( string.GetLength() < 512 ) // null 때문에 255
							strcpy(	pQuestState->m_szDesc, string );
						else
							Error( "%s(%d) LoadPropQuest의 SetDesc 길이 초과", lpszFileName, script.GetLineNum() ); 

					}
					else
					if( script.Token == "SetCond" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
#ifndef __V050322_CACHEQUEST 
						if( string.GetLength() < 255 ) // null 때문에 255
							strcpy(	pQuestState->m_szCond, string );
						else
							Error( "%s(%d) LoadPropQuest의 SetCond 길이 초과", lpszFileName, script.GetLineNum() ); 
#endif
					}
					else
					if( script.Token == "SetStatus" )
					{
						script.GetToken(); // (
						CString string = GetLangScript( script );
#ifndef __V050322_CACHEQUEST 
						if( string.GetLength() < 255 ) // null 때문에 255
							strcpy(	pQuestState->m_szStatus, string );
						else
							Error( "%s(%d) LoadPropQuest의 SetStatus 길이 초과", lpszFileName, script.GetLineNum() ); 
#endif
					}
					script.GetToken(); // key or anything
				}
			}
		}
		// EndCondCharacter세팅이 안됐다면 LinkChar로 대체한다.
#ifdef __V050322_CACHEQUEST
		if( bEndCondCharacter == FALSE )
		{
			// 스트링이 없으면 szLinkChar에 세팅된 것으로 대체한다. 즉 자기 자신이다.
			if( szLinkCharNum == 1 )
			{
				LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ 0 ] );
				if( lpCharacter ) 
				{
					lpCharacter->m_awDstQuest.Add( nQuest );
					lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( 0 ) );
				}
				strcpy( propQuest.m_szEndCondCharacter, szLinkChar[ 0 ] );
			}
			else
			if( szLinkCharNum > 1 )
			{
				propQuest.m_lpszEndCondMultiCharacter = new CHAR[ 64 * 10 ];
				ZeroMemory( propQuest.m_lpszEndCondMultiCharacter, 64 * 10 );
				for( int i = 0; i < szLinkCharNum; i++ )
				{
					strcpy( &propQuest.m_lpszEndCondMultiCharacter[ i * 64 ], szLinkChar[ 0 ] );
					LPCHARACTER lpCharacter = GetCharacter( szLinkChar[ i ] );
					if( lpCharacter )
					{
						lpCharacter->m_awDstQuest.Add( nQuest );
						lpCharacter->m_anDstQuestItem.Add( lpCharacter->m_anSrcQuestItem.GetAt( i ) );
					}
				}
			}
		}
		if( nBeginCondItemNum )
		{
			propQuest.m_paBeginCondItem = new QuestPropItem[ nBeginCondItemNum ];
			memcpy( propQuest.m_paBeginCondItem, aBeginCondItem, sizeof( QuestPropItem ) * nBeginCondItemNum );
			propQuest.m_nBeginCondItemNum = nBeginCondItemNum;
		}
		if( nEndCondItemNum )
		{
			propQuest.m_paEndCondItem = new QuestPropItem[ nEndCondItemNum ];
			memcpy( propQuest.m_paEndCondItem, aEndCondItem, sizeof( QuestPropItem ) * nEndCondItemNum );
			propQuest.m_nEndCondItemNum = nEndCondItemNum;
		}
		if( nEndRewardItemNum )
		{
			propQuest.m_paEndRewardItem = new QuestPropItem[ nEndRewardItemNum ];
			memcpy( propQuest.m_paEndRewardItem, aEndRewardItem, sizeof( QuestPropItem ) * nEndRewardItemNum );
			propQuest.m_nEndRewardItemNum = nEndRewardItemNum;
		}
#endif
		m_aPropQuest.SetAtGrow( nQuest, &propQuest );
		nQuest = script.GetNumber();  // id
	}
	if( bOptimize )
		m_aPropQuest.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropAddSkill( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	AddSkillProp propAddSkill;
	int i = script.GetNumber( TRUE ); // id
	while ( script.tok != FINISHED )
	{
		if( i == 0 )			// 이거 없으니까 파일뒤에 공백있는것도 줄줄히 읽더라
			break;
#ifdef _DEBUG
		if( i == 4079 )
		{
			int a = 0;
		}
#endif
		propAddSkill.dwID = i;
		propAddSkill.dwName = script.GetNumber( TRUE );
#ifdef __XUZHU
		if( atoi( script.m_strDefine ) != 0 )		// SI_어쩌구로 되어 있었으면 변환하면 0이된다 0이 정상.
			Error( "SkillPropAdd Read Error\nID:%d\nSkillID:%d\ndwAbilityMin:%d\ndwAbilityMax:%d", propAddSkill.dwID, propAddSkill.dwName, propAddSkill.dwAbilityMin, propAddSkill.dwAbilityMax );
		
#endif
		propAddSkill.dwSkillLvl = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMin = script.GetNumber( TRUE );
		propAddSkill.dwAbilityMax = script.GetNumber( TRUE );
		propAddSkill.dwAttackSpeed = script.GetNumber( TRUE );
		propAddSkill.dwDmgShift = script.GetNumber( TRUE );
		propAddSkill.nProbability = script.GetNumber( TRUE );
		propAddSkill.dwTaunt = script.GetNumber( TRUE );
		propAddSkill.dwDestParam1 = script.GetNumber( TRUE );
		propAddSkill.dwDestParam2 = script.GetNumber( TRUE );
		propAddSkill.nAdjParamVal1 = script.GetNumber( TRUE );
		propAddSkill.nAdjParamVal2 = script.GetNumber( TRUE );
		propAddSkill.dwChgParamVal1 = script.GetNumber( TRUE );
		propAddSkill.dwChgParamVal2 = script.GetNumber( TRUE );
		propAddSkill.nDestData1[0] = script.GetNumber( TRUE );
		propAddSkill.nDestData1[1] = script.GetNumber( TRUE );
		propAddSkill.nDestData1[2] = script.GetNumber( TRUE );
#ifdef __XSKILL0425
		propAddSkill.dwActiveSkill = script.GetNumber( TRUE );
		propAddSkill.dwActiveSkillRate = script.GetNumber( TRUE );
#endif
		propAddSkill.nReqMp = script.GetNumber( TRUE );
		propAddSkill.nReqFp = script.GetNumber( TRUE );
		propAddSkill.dwSkillReady = script.GetNumber( TRUE );
		propAddSkill.dwSkillRange = script.GetNumber( TRUE );
		propAddSkill.dwCircleTime = script.GetNumber( TRUE );
		propAddSkill.dwPainTime  = script.GetNumber( TRUE );
		propAddSkill.dwSkillTime = script.GetNumber( TRUE );
		propAddSkill.nSkillCount = script.GetNumber( TRUE );
		propAddSkill.dwSkillExp = script.GetNumber( TRUE );
		
		propAddSkill.dwExp = script.GetNumber( TRUE );
		propAddSkill.dwComboSkillTime = script.GetNumber( TRUE );
		
		m_aPropAddSkill.SetAtGrow( i, &propAddSkill );

		TRACE( "PropAddSkill : %d %d %d\r\n", i, propAddSkill.dwName, propAddSkill.dwSkillLvl );
		i = script.GetNumber( TRUE ); 
#ifdef __XUZHU
		if( atoi( script.m_strDefine ) != 0 )		// SI_어쩌구로 되어 있었으면 변환하면 0이된다 0이 정상.
			Error( "2SkillPropAdd Read Error\nID:%d\nSkillID:%d\ndwAbilityMin:%d\ndwAbilityMax:%d", propAddSkill.dwID, propAddSkill.dwName, propAddSkill.dwAbilityMin, propAddSkill.dwAbilityMax );
		
#endif
	}
	m_aPropAddSkill.Optimize();
	return TRUE;
}

BOOL CProject::LoadPropObj( LPCTSTR lpszFileName, CFixedArray< ObjProp >* apObjProp )
{
	CScript scanner( TRUE );
	if(scanner.Load(lpszFileName)==FALSE)
		return FALSE;
	ObjProp objProp;
	int i = scanner.GetNumber( TRUE );  // id
	while( scanner.tok != FINISHED )
	{
		objProp.dwID = i;    
		scanner.GetToken( );  // Name 
		_tcscpy( objProp.szName, scanner.Token );
		scanner.GetToken( );  // Name 
		_tcscpy( objProp.szNameEn, scanner.Token );
		objProp.dwType = scanner.GetNumber();            
		objProp.dwAI   = scanner.GetNumber();      
		objProp.dwHP   = scanner.GetNumber();      
		scanner.GetToken( );  // 설명 
		apObjProp->SetAtGrow( i, &objProp);
		i = scanner.GetNumber(); 
	}
	apObjProp->Optimize();
	return TRUE;
}

BOOL CProject::LoadPropMoverEx( LPCTSTR szFileName )
{
	CScript script;
	int		nBlock = 0;
 
	if(script.Load(szFileName)==FALSE)
		return FALSE;
	CString string;
	int nVal;
	nVal = script.GetNumber(); // Mover Id
	do {
		MoverProp* pProp = GetMoverProp( nVal );

		if(pProp == NULL)
		{
			TRACE("MoverAddProp에서 존재해야될 base Property %s가 없다. %s\n",script.Token);
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

		int nCallNum = 0;

		pProp->m_nChangeTargetRand = 10;
		pProp->m_dwAttackMoveDelay = 0;
		pProp->m_bPartyAttack      = 0;
		pProp->m_dwRunawayDelay    = SEC( 1 );
#ifdef __V050324_AISCAN
		pProp->m_nAttackFirstRange = 3;
#else
		pProp->m_nAttackFirstRange = 10;		
#endif
		script.GetToken(); // {
		script.GetToken(); 
		while(*script.token != '}')
		{
			if(script.Token == ";") 
			{ 
				script.GetToken(); 
				continue; 
			} 
#if !defined( __BEAST ) && defined( __XAI1013 )

			if( strcmpi( script.Token, "AI" ) == 0 )		// AI블럭
			{
				BOOL bRet = LoadPropMoverEx_AI( szFileName, script, nVal );
				if ( bRet == FALSE )
				{
					Error( "%s : MoverID %s(%d)의 AI{} 블럭 읽기 실패", szFileName, pProp->szName, nVal );
					return FALSE;
				}
			}
#else // xAI1013
			if( strcmpi( script.Token, "AI" ) == 0 )		// AI블럭 건너뜀.
			{
				script.GetToken();		// {읽음.
				if( script.token[0] != '{' )	// {가 아니면 첨으로 돌아간다.
				{
					Message( "LoadPropMoverEx : %s의 AI명령 다음에 { 블럭이 없다", pProp->szName );
					continue;
				}
				nBlock = 0;
				while(1)
				{
					if( script.token[0] == '{' )
						nBlock ++;
					else
					if( script.token[0] == '}' )
						nBlock --;
					if( nBlock == 0 )
						break;
					
					script.GetToken();	// 다음 토큰 읽음.
				}
			}
			
#endif // not xAI1013
			if(script.Token == "m_nAttackFirstRange")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackFirstRange  = script.GetNumber();
			}
			else
			if(script.Token == "SetEvasion")
			{
				script.GetToken(); // skip (
				pProp->m_nEvasionHP  = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nEvasionSec = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetRunAway")
			{
				script.GetToken(); // skip (
				pProp->m_nRunawayHP = script.GetNumber();
				script.GetToken(); // skip ,
				if( script.Token == "," )
				{
					//pProp->m_nRunawayHelperIdx = script.GetNumber();
					script.GetNumber();
					script.GetToken(); // skip ,
					//pProp->m_nRunawayHelperNum = script.GetNumber();
					script.GetNumber();
					script.GetToken(); // skip )
				}
			}
			else
			if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				pProp->m_nCallHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperIdx[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperNum[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_bCallHelperParty[ pProp->m_nCallHelperMax ] = script.GetNumber();
				script.GetToken(); // skip )
				pProp->m_nCallHelperMax++;
				if( pProp->m_nCallHelperMax > 5 )
				{
					char pszText[100];
					sprintf( pszText, "ID = %d: helper too many", pProp->dwID );
					AfxMessageBox( pszText );
				}
			}
			else
			if(script.Token == "m_nAttackItemNear")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemNear  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemFar")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemFar  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem1")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem1  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem2")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem2  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem3")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem3  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem4")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem4  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemSec")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemSec = script.GetNumber();
			}
			else
			if(script.Token == "m_nMagicReflection")
			{
				script.GetToken(); // skip =
				pProp->m_nMagicReflection = script.GetNumber();
			}
			else
			if(script.Token == "m_nImmortality")
			{
				script.GetToken(); // skip =
				pProp->m_nImmortality = script.GetNumber();
			}
			else
			if(script.Token == "m_bBlow")
			{
				script.GetToken(); // skip =
				pProp->m_bBlow = script.GetNumber();
			}
			else
			if(script.Token == "m_nChangeTargetRand")
			{
				script.GetToken(); // skip =
				pProp->m_nChangeTargetRand = script.GetNumber();
			}
			else
			if(script.Token == "m_dwAttackMoveDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwAttackMoveDelay = script.GetNumber();
			}
			else
			if(script.Token == "m_dwRunawayDelay")
			{
				script.GetToken(); // skip =
				pProp->m_dwRunawayDelay = script.GetNumber();
			}
			else
			if(script.Token == "randomItem")
				InterpretRandomItem( &pProp->m_randomItem, script );
			else 
			if( script.Token == "Maxitem" )
			{
				script.GetToken();	// =
				pProp->m_DropItemGenerator.m_dwMax	= script.GetNumber();
			}
			else 
			if( script.Token == "DropItem" )
			{
				DROPITEM di;
				memset( &di, 0, sizeof(DROPITEM) );
				di.dtType = DROPTYPE_NORMAL;
				script.GetToken();	// (
				di.dwIndex	= script.GetNumber();	// specific item index
				ASSERT( di.dwIndex != 0 );
				script.GetToken();	// ,
				di.dwProbability	= script.GetNumber();	// probability
				script.GetToken();	// ,
				di.dwLevel	= script.GetNumber();	// level
				script.GetToken();	// ,
				di.dwNumber	= script.GetNumber();	// number
				script.GetToken();	// )
				pProp->m_DropItemGenerator.AddTail( di );	// copy
				di.dwNumber2 = 0;
			}
			if( script.Token == "DropKind" )
			{
				DROPKIND dropKind;
				script.GetToken();	// (
				dropKind.dwIK3	= script.GetNumber();	//	dwIK3
				script.GetToken();	// ,
				dropKind.nMinUniq	= script.GetNumber();	//	nMinUniq
				script.GetToken();	// ,
				dropKind.nMaxUniq	= script.GetNumber();	// nMaxUniq
				script.GetToken();	// )
				pProp->m_DropKindGenerator.AddTail( dropKind );	// copy
			}
			else
			if( script.Token == "DropGold" )
			{
				DROPITEM di;
				memset( &di, 0, sizeof(DROPITEM) );	// clear
				di.dtType = DROPTYPE_SEED;
				di.dwProbability = 0xFFFFFFFF;	// 무조건 나오게.
				script.GetToken();	// (
				di.dwNumber = script.GetNumber();	// gold min
				script.GetToken();	// ,
				di.dwNumber2 = script.GetNumber();	// gold max
				script.GetToken();	// )

				pProp->m_DropItemGenerator.AddTail( di );	// copy
			}
			/*
			else
			{
				CString string;
				string.Format("%s : %s Not Found", szFileName, script.Token );
				AfxMessageBox( string );
			}
			*/
			script.GetToken();
		} 

		nVal = script.GetNumber(); // Mover Id
	} while(script.tok != FINISHED);

	return TRUE;
}
BOOL CProject::LoadCharacter_All( LPCTSTR lpszFileName )
{
	LoadCharacter( lpszFileName );
	struct _finddata_t c_file;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpszFileName, drive, dir, name, ext );
	CString string;
	string.Format( "%s%s-*%s", dir, name, ext );
	/* Find first .c file in current directory */
	CFileFinder ff;
	if( ff.FindFirst( string, &c_file ) )
	{
		if( !(c_file.attrib & _A_SUBDIR ) )
		{
			string.Format( "%s%s", dir, c_file.name );
			LoadCharacter( string );
		}
		/* Find the rest of the .c files */
		while( ff.FindNext( &c_file ) )
		{
			//strcpy( name, dir );
			//strcat( name, c_file.name );
			if( !(c_file.attrib & _A_SUBDIR ) )
			{
				string.Format( "%s%s", dir, c_file.name );
				LoadCharacter( string );
			}
		}
		ff.FindClose();
	}	
	return TRUE;
}
BOOL CProject::LoadCharacter( LPCTSTR szFileName )
{
	CScript script;
 
	if(script.Load(szFileName)==FALSE)
		return FALSE;

	//CHAR asd[10][10] = { "asd", "asd" };

	CString strName;
	LPCHARACTER lpCharacter;
	script.GetToken(); // Mover name
	strName = script.Token;
	while( script.tok != FINISHED)
	{
		lpCharacter = new CHARACTER;
		lpCharacter->Clear();
		// 맵에 추가되는 것은 lowercase다. 그런데, 유일한 key 스트링이 lowercase로 맵에 추가되면


		// 원본 자체가 손상되는 결과구 그 리스트를 뽑아낼 때도 모두 lowercase가 되므로 원본을 m_szKey에
		// 보관해두는 것이다.
		_tcscpy( lpCharacter->m_szKey, strName );
		strName.MakeLower();
#ifdef _DEBUG
		if( m_mapCharacter.Lookup( strName, (void*&)lpCharacter ) )
		{
			Error( "%s에서 동일한 %s 발견", szFileName, strName );
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
			else
			if( script.Token == "}" )			
				nBlock--;
			else
			if(script.Token == "randomItem")
			{
				InterpretRandomItem(&lpCharacter->m_randomItem,script);
			}
			else
#ifdef __V050322_CACHEQUEST
			if(script.Token == "SetEquip")
#else // __V050322_CACHEQUEST
//{{AFX
			if(script.Token == "m_aEquip")
//}}AFX
#endif // __V050322_CACHEQUEST
			{
				lpCharacter->m_nEquipNum = 0;
				//CHAR asd[10][10] = { "asd", "asd" };
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
			else
			if(script.Token == "m_szName")
			{
				script.GetToken(); // =
				script.GetToken(); // name
				lpCharacter->m_strName = script.Token; 
			}
			else
			if(script.Token == "SetName")
			{ 
				script.GetToken(); // (
				lpCharacter->m_strName = GetLangScript( script );
				if( strlen( lpCharacter->m_strName ) == 0 )
				{
					Message( _T( "Error SetName m_szName Not Data") );
				}
				
				//script.GetToken(); // =
				//script.GetToken(); // name
				//lpCharacter->m_strName = script.Token; 
			}
			else
			if( script.Token == "SetFigure" )
			{
				script.GetToken(); // (
			#ifdef __V050322_CACHEQUEST
				lpCharacter->m_dwMoverIdx = script.GetNumber();
				script.GetToken(); // ,
			#endif
				lpCharacter->m_dwHairMesh = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHairColor = script.GetNumber();
				script.GetToken(); // ,
				lpCharacter->m_dwHeadMesh = script.GetNumber();
			}

#ifdef __V050322_CACHEQUEST
			else
			if( script.Token == "SetMusic" )
			{
				script.GetToken(); // (
				lpCharacter->m_dwMusicId = script.GetNumber();
			}			
#endif
			/*
			else
			if(script.Token == "m_nColorSkin")
			{
				script.GetToken(); // =
				lpCharacter->m_nColorSkin = script.GetNumber(); 
			}
			else
			if(script.Token == "m_nRace")
			{
				script.GetToken(); // =
				lpCharacter->m_nRace = script.GetNumber(); 
			}
			else
			if(script.Token == "m_nAIInterface")
			{
				script.GetToken(); // =
				lpCharacter->m_nAIInterface = script.GetNumber(); 
			}
			else
			if(script.Token == "SetEvasion")
			{
				script.GetToken(); // skip (
				lpCharacter->m_nEvasionHP  = script.GetNumber();
				script.GetToken(); // skip ,
				lpCharacter->m_nEvasionSec = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetRunAway")
			{
				script.GetToken(); // skip (
				lpCharacter->m_nRunawayHP = script.GetNumber();
				script.GetToken(); // skip ,
				lpCharacter->m_bRunawayToHelper = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				lpCharacter->m_nCallHelperHP = script.GetNumber();
				script.GetToken(); // skip ,
				lpCharacter->m_nCallHelperSec = script.GetNumber();
				script.GetToken(); // skip ,
				lpCharacter->m_nCallHelperId = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "m_nAttackItemNear")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItemNear  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemFar")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItemFar  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem1")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItem1  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem2")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItem2  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem3")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItem3  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem4")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItem4  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemSec")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nAttackItemSec = script.GetNumber();
			}
			else
			if(script.Token == "m_nMagicReflection")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nMagicReflection = script.GetNumber();
			}
			else
			if(script.Token == "m_nImmortality")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nImmortality = script.GetNumber();
			}
			else
			if(script.Token == "m_bBlow")
			{
				script.GetToken(); // skip =
				lpCharacter->m_bBlow = script.GetNumber();
			}
			else
			if(script.Token == "m_nChangeTargetRand")
			{
				script.GetToken(); // skip =
				lpCharacter->m_nChangeTargetRand = script.GetNumber();
			}
			else
			if(script.Token == "m_nStr")
			{
				script.GetToken(); // =
				lpCharacter->m_nStr = script.GetNumber(); 
			}
			else
			if(script.Token == "m_nInt")
			{
				script.GetToken(); // =
				lpCharacter->m_nInt = script.GetNumber(); 
			}
			else
			if(script.Token == "m_nSta")
			{
				script.GetToken(); // =
				lpCharacter->m_nSta = script.GetNumber();
			}
			else
			if(script.Token == "m_nDex")
			{
				script.GetToken(); // =
				lpCharacter->m_nDex = script.GetNumber(); 
			}
			else
			if(script.Token == "m_nPartySide")
			{
				script.GetToken();
				lpCharacter->m_nPartySide = script.GetNumber(); 
			}
			else
			if(script.Token == "m_nAttitude")
			{
				script.GetToken();
				lpCharacter->m_nAttitude = script.GetNumber(); 
			}
			*/
			else
			if(script.Token == "m_nStructure")
			{
				script.GetToken();
				lpCharacter->m_nStructure = script.GetNumber();
			}
			else
/*			if(script.Token == "m_szChar")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szChar, script.Token );
			}
*/
			if(script.Token == "m_szChar") // "SetImage"랑 같다 차후 삭제 예정 
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				strcpy( lpCharacter->m_szChar, filename.GetBuffer(0) );
			}
			else
			if(script.Token == "m_szDialog")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szDialog, script.Token );
			}
			else
			if(script.Token == "m_szDlgQuest")
			{
				script.GetToken();
				script.GetToken();
				strcpy( lpCharacter->m_szDlgQuest, script.Token );
			}
			else
			if(script.Token == "SetImage" )
			{ 
				script.GetToken();
				CString filename = GetLangScript( script );
				strcpy( lpCharacter->m_szChar, filename.GetBuffer(0) );
			}
			else
			if(script.Token == "AddMenu" )
			{
				script.GetToken(); // (
				int nMMI = script.GetNumber(); 
				script.GetToken(); // ) 
				lpCharacter->m_abMoverMenu[ nMMI ] = TRUE;
			}
			else
			if(script.Token == "AddVenderSlot" ) // "AddVendorSlot"랑 같다 삭제 예정 
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); script.GetToken(); // 
				script.GetToken();
				lpCharacter->m_venderSlot[ nSlot ] = script.token;
				script.GetToken(); // 
			}
			else
			if(script.Token == "AddVendorSlot" )
			{
				script.GetToken(); // (
				int nSlot = script.GetNumber(); // slot
				script.GetToken(); // ,
				lpCharacter->m_venderSlot[ nSlot ] = GetLangScript( script );
			}
			else
			if( script.Token == "AddVenderItem" || script.Token == "AddVendorItem")
			{
				script.GetToken(); // (
				int nSlot      = script.GetNumber(); script.GetToken(); // 
				int nItemKind3  = script.GetNumber(); script.GetToken(); // 
				int	nItemJob	= script.GetNumber(); script.GetToken();	//
//				int nNumMax    = script.GetNumber(); script.GetToken(); //
				int nUniqueMin = script.GetNumber(); script.GetToken(); // 
				int nUniqueMax = script.GetNumber(); script.GetToken(); // 
				int nTotalNum  = script.GetNumber(); script.GetToken(); // 
				//int nMaterialCount  = script.GetNumber(); script.GetToken(); // 

				LPVENDOR_ITEM pVendorItem = new VENDOR_ITEM;
				pVendorItem->m_nItemkind3  = nItemKind3;
				pVendorItem->m_nItemJob	= nItemJob;
//				pVendorItem->m_nNumMax    = nNumMax;
				pVendorItem->m_nUniqueMin = nUniqueMin;
				pVendorItem->m_nUniqueMax = nUniqueMax;
				pVendorItem->m_nTotalNum  = nTotalNum;
				//pVendorItem->m_nMaterialCount  = nMaterialCount;
				lpCharacter->m_venderItemAry[ nSlot ].Add( pVendorItem );
			}
		}
		script.GetToken(); // Mover name
		strName = script.Token;
	} 
	return TRUE;
}	

void CProject::InterpretRandomItem(LPRANDOM_ITEM pRandomItem,CScript& script)
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

u_long CProject::GetPlayerID( const CHAR* lpszPlayer )
{
#ifdef __WORLDSERVER
//	CMclAutoLock	Lock( m_csPlayerID );
	m_csPlayerID.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER
	u_long idPlayer		= 0;
	STRING2ULONG::iterator i	= m_str2ulong.find( lpszPlayer );
	if( i != m_str2ulong.end() )
		idPlayer	= i->second;
//		return( i->second );
//	return 0;
#ifdef __WORLDSERVER
	m_csPlayerID.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER

	return idPlayer;
}

LPCSTR CProject::GetPlayerString( u_long idPlayer )
{
#ifdef __WORLDSERVER
//	CMclAutoLock	Lock( m_csPlayerID );
	m_csPlayerID.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER
	LPCSTR pszPlayer	= NULL;
	ULONG2STRING::iterator i	= m_ulong2str.find( idPlayer );
	if( i != m_ulong2str.end() )
		pszPlayer	= i->second.data();
//		return( i->second.data() );
//	return NULL;
#ifdef __WORLDSERVER
	m_csPlayerID.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER

	return pszPlayer;
}

BOOL CProject::SetPlayerID( u_long idPlayer, const CHAR* lpszPlayer )
{
	if( GetPlayerString( idPlayer ) == NULL )
	{
#ifdef __WORLDSERVER
//		CMclAutoLock	Lock( m_csPlayerID );
		m_csPlayerID.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER
//		m_str2ulong[lpszPlayer]		= idPlayer;
//		m_ulong2str[idPlayer]	= lpszPlayer;
		m_str2ulong.insert( STRING2ULONG::value_type( lpszPlayer, idPlayer ) );
		m_ulong2str.insert( ULONG2STRING::value_type( idPlayer, lpszPlayer ) );
#ifdef __WORLDSERVER
		m_csPlayerID.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
		return TRUE;
	}
	ASSERT( 0 );
//#ifdef __WORLDSERVER
//	m_csPlayerID.Leave( theLineFile );	// unlock1
//#endif	// __WORLDSERVER
	return FALSE;
}

BOOL CProject::RemovePlayerID( u_long idPlayer )
{
//#ifdef __WORLDSERVER
//	CMclAutoLock	Lock( m_csPlayerID );
//	m_csPlayerID.Enter( theLineFile );	// lock1
//#endif	// __WORLDSERVER
	const char* lpszPlayer	= GetPlayerString( idPlayer );
	if( NULL != lpszPlayer )
	{
#ifdef __WORLDSERVER
		m_csPlayerID.Enter( theLineFile );	// lock1
#endif	// __WORLDSERVER
		m_str2ulong.erase( lpszPlayer );
		m_ulong2str.erase( idPlayer );
#ifdef __WORLDSERVER
		m_csPlayerID.Leave( theLineFile );	// unlock1
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}
/*
void CProject::InterpretMoverAddProp(MoverProp* pMoverPropAry,CScript& script)
{
//	CWordArray* pWordArray;
	CString string;
	int nVal;
	script.GetToken(); // {
	nVal = script.GetNumber(); // Mover Id
	while(*script.token != '}')
	{
		MoverProp* pProp = &pMoverPropAry[nVal];
		if(pProp == NULL)
		{
			TRACE("MoverAddProp에서 존재해야될 base Property %s가 없다. %s\n",script.Token);
			continue;
		}
		script.GetToken(); // {
		script.GetToken(); 
		while(*script.token != '}')
		{
			if(script.Token == ";") 
			{ 
				script.GetToken(); 
				continue; 
			} 
			if(script.Token == "SetEvasion")
			{
				script.GetToken(); // skip (
				pProp->m_nEvasionHP  = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nEvasionSec = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetRunAway")
			{
				script.GetToken(); // skip (
				pProp->m_nRunawayHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_bRunawayToHelper = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "SetCallHelper")
			{
				script.GetToken(); // skip (
				pProp->m_nCallHelperHP = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperSec = script.GetNumber();
				script.GetToken(); // skip ,
				pProp->m_nCallHelperId = script.GetNumber();
				script.GetToken(); // skip )
			}
			else
			if(script.Token == "m_nAttackItemNear")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemNear  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemFar")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemFar  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem1")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem1  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem2")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem2  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem3")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem3  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItem4")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItem4  = script.GetNumber();
			}
			else
			if(script.Token == "m_nAttackItemSec")
			{
				script.GetToken(); // skip =
				pProp->m_nAttackItemSec = script.GetNumber();
			}
			else
			if(script.Token == "m_nMagicReflection")
			{
				script.GetToken(); // skip =
				pProp->m_nMagicReflection = script.GetNumber();
			}
			else
			if(script.Token == "m_nImmortality")
			{
				script.GetToken(); // skip =
				pProp->m_nImmortality = script.GetNumber();
			}
			else
			if(script.Token == "m_bBlow")
			{
				script.GetToken(); // skip =
				pProp->m_bBlow = script.GetNumber();
			}
			else
			if(script.Token == "m_nChangeTargetRand")
			{
				script.GetToken(); // skip =
				pProp->m_nChangeTargetRand = script.GetNumber();
			}
			else
			if(script.Token == "randomItem")
				;//InterpretRandomItem(&pProp->m_randomItem,script);
			script.GetToken();
		}
		nVal = script.GetNumber(); // Mover Id
	}
}
*/
/*
struct tagExpCharacter
{
	DWORD dwExp;
	DWORD dwMaxExp;
	DWORD dwLPPoint;

} EXPCHARACTER,* LPEXPCHARACTER

struct tagJobAbility
{
	DWORD dwStr;
	DWORD dwSta;
	DWORD dwDex;
	DWORD dwInt;

} JOBABILITY,* LPJOBABILITY


LPEXPCHARACTER m_apExpCharacter;
LPJOBABILITY m_apJobAbillity[ MAX_JOB ];
LPDWORD m_apExpLPPoint;
LPDWORD m_apExpSkill;

expCharacter // 1 based
{
	   0	0	  1    //1
	1000	0	  2    //2
	1818	0	  2    //3
}
expLPPoint // 0 based
{
1
1
2
}
jobAbillity
{
  JOB_MOOYEGA // 무예가 
  {
    1 2 // str
	1 1 // sta
	1 1 // dex
	2 1 // int
  }
  JOB_VAGRANT // 방랑자
  {
    1 1 // str
	1 1 // sta
	1 1 // dex
	1 1 // int
  }
}



expSkill // 1 based
{
0
4000
9300
}



*/

BOOL CProject::LoadExpTable( LPCTSTR lpszFileName )
{	
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
		return FALSE;

	DWORD dwVal, dwJob;
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
		else
		if( script.Token == _T( "expFlight" ) )
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
		else
		if( script.Token == _T( "expLPPoint" ) )
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
		else
		if( script.Token == _T( "expJobLevel" ) )
		{
			i = 15;

			ZeroMemory( m_aExpJobLevel, sizeof( m_aExpJobLevel ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aExpJobLevel[ i++ ] = dwVal;
				script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "jobAbillity" ) )
		{
			ZeroMemory( m_aJobAbillity, sizeof( m_aJobAbillity ) );
			script.GetToken(); // { 
			dwJob = script.GetNumber();
			while( *script.token != '}' )
			{
				script.GetToken(); // { 
				dwVal = script.GetNumber();
				while( *script.token != '}' )
				{
					m_aJobAbillity[ i ].dwStr = dwVal;
					m_aJobAbillity[ i ].dwStrAdd = script.GetNumber();
					m_aJobAbillity[ i ].dwSta = script.GetNumber();
					m_aJobAbillity[ i ].dwStaAdd = script.GetNumber();
					m_aJobAbillity[ i ].dwDex = script.GetNumber();
					m_aJobAbillity[ i ].dwDexAdd = script.GetNumber();
					m_aJobAbillity[ i ].dwInt = script.GetNumber();
					m_aJobAbillity[ i++ ].dwIntAdd = script.GetNumber();

					dwVal = script.GetNumber();
				}
				dwJob = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expSkill" ) )
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
		else
		if( script.Token == _T( "attackStyle" ) )
		{
			ZeroMemory( m_aAttackStyle, sizeof( m_aAttackStyle ) );
			script.GetToken(); // { 
			dwVal = script.GetNumber();
			while( *script.token != '}' )
			{
				m_aAttackStyle[ dwVal ].dwParam1 = script.GetNumber();
				m_aAttackStyle[ dwVal ].dwParam2 = script.GetNumber();
				dwVal = script.GetNumber();
			}
		}
		else
		if( script.Token == _T( "expParty" ) )
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
		else
		if( script.Token == _T( "expPartySkill" ) )
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
#ifdef __GUILD_CONST0713
		else if( script.Token == _T( "expCompanyTest" ) )
		{
		#if !defined(__BEAST)
			CGuildTable::GetInstance().ReadBlock( script );
		#endif
		}
#else	// __GUILD_CONST0713
		else if( script.Token == _T( "expCompany" ) )
		{
		#if !defined(__BEAST)
			CGuildTable::GetInstance().ReadBlock( script );
		#endif
		}
#endif	// __GUILD_CONST0713
		else if( script.Token == _T( "expDropLuck" ) )
		{
			memset( m_adwExpDropLuck, 0, sizeof(m_adwExpDropLuck) );
			script.GetToken();	// {
			DWORD* pdw	= &m_adwExpDropLuck[0][0];
			DWORD dwValue	= (DWORD)script.GetNumber();	//
			while( *script.token != '}' )
			{
#ifdef __EXPDROPLUCK0524
				ASSERT( pdw <= &m_adwExpDropLuck[121][10] );
#else	// __EXPDROPLUCK0524
				ASSERT( pdw <= &m_adwExpDropLuck[101][10] );
#endif	// __EXPDROPLUCK0524
				*pdw	= dwValue;
				pdw++;
				dwValue	= script.GetNumber();
			}
		}
#ifdef __J0608
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
#endif	// __J0608
		else
		if( script.Token == _T( "expUpitem" ) )
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
				
				i++;
			}

			if( j != 6 )
				Message( "CProject::LoadExpTable : expUpitem 블럭의 데이타 열개수가 맞지 않다. %d", j );
			if( i != 11 )
				Message( "CProject::LoadExpTable : expUpitem 블럭의 데이타 행개수가 맞지 않다. %d", j );
			

		} // expUpitem

		script.GetToken(); // type name or }
	}
	return TRUE;
}

#ifdef __WORLDSERVER
/*BOOL	CProject::LoadReNewEventTime( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load(lpszFileName) == FALSE )
	{
		Message( "LoadReNewEventTime 열수 없음." );
		return FALSE;	
	}
	int nCount = 0;

	while( script.tok != FINISHED )
	{	
		script.GetToken();
		strcpy( m_aReNewEventTime[nCount].strTime, script.Token );
		for( int i = 0 ; i < MAX_REALITEM ; i++ )
		{
			m_aReNewEventTime[nCount].nRealitem[i] = script.GetNumber();
		}
		++nCount;
	}

	return TRUE;
}
*/
BOOL	CProject::LoadReNewEventRealItem( LPCTSTR lpszFileName )
{
	m_nRealItem[ 0 ] = II_SYS_SYS_EVE_0504M1NOTEBOOK;
	m_nRealItem[ 1 ] = II_SYS_SYS_EVE_0504M2DCAMARA;
	m_nRealItem[ 2 ] = II_SYS_SYS_EVE_0504M3AIRSHIP;   
	m_nRealItem[ 3 ] = II_SYS_SYS_EVE_0504M4USBFANLIGHT;
	m_nRealItem[ 4 ] = II_SYS_SYS_EVE_0504M5BALLOON;
	m_nRealItem[ 5 ] = II_SYS_SYS_EVE_0504M6GIFTTICKET;
	m_nRealItem[ 6 ] = II_SYS_SYS_EVE_0504M7MOVIETICKET;
	m_nRealItem[ 7 ] = II_SYS_SYS_EVE_0504M8OST;
	
	ZeroMemory( m_aRealEventItem, sizeof( m_aRealEventItem ) );
	
	
	CScript script;


#ifdef __IDC
	char FileNameBuf[MAX_PATH] = {0,};
	sprintf( FileNameBuf, "c:\\neurospace\\script\\%s", lpszFileName );
	if( script.Load(FileNameBuf) == FALSE )
#else	// __IDC
	if( script.Load(lpszFileName) == FALSE )
#endif	// __IDC
	{
		Message( "0504Event_LevelUp_RealItemList 열수 없음." );
		return FALSE;	
	}

	m_nMaxReNewEventItem = 0;

	int nPercent = 0;
	
#ifdef __S0517
	int nEndTime = script.GetNumber();
#else // __S0517
	script.GetToken();
	CString BufStr = script.Token;	
#endif // __S0517
	int bufLevel = script.GetNumber();
	while( script.tok != FINISHED )
	{	
#ifdef __S0517
		m_aRealEventItem[ m_nMaxReNewEventItem ].EndTime = CTime::CTime( nEndTime / 10000, (nEndTime % 10000) / 100, (nEndTime % 10000) % 100, 0, 0, 0 );
#else // __S0517
		strcpy( m_aRealEventItem[ m_nMaxReNewEventItem ].strTime, BufStr );
#endif // __S0517
		m_aRealEventItem[ m_nMaxReNewEventItem ].nLevel = bufLevel;
		int nTotalPercent = 0;
		for( int i = 0 ; i < MAX_REALITEM ; i++ )
		{ 
			m_aRealEventItem[m_nMaxReNewEventItem].nRealitem[i] = script.GetNumber();
			m_aRealEventItem[m_nMaxReNewEventItem].nRealitemCount[i] = script.GetNumber();
			m_aRealEventItem[m_nMaxReNewEventItem].nSendRealitemCount[i] = 0;
#ifdef __S0512
			nPercent = script.GetFloat() * 100;
			m_aRealEventItem[m_nMaxReNewEventItem].nPercent[i] = nPercent;
#else // __S0512
			nTotalPercent += script.GetFloat() * 100;

			if( nTotalPercent > 10000 )
			{
				AfxMessageBox( "Real Event Percent 100 orverflow" );
			}
			m_aRealEventItem[m_nMaxReNewEventItem].nPercent[i] = nTotalPercent;
#endif // __S0512

#ifdef __S0517
			nEndTime = script.GetNumber();
#else // __S05717
			script.GetToken();
			BufStr = script.Token;
#endif // __S0517
			bufLevel = script.GetNumber();
			if( bufLevel != m_aRealEventItem[ m_nMaxReNewEventItem ].nLevel )
			{
				break;
			}
		}
		m_aRealEventItem[m_nMaxReNewEventItem].nNextIndex = m_nMaxReNewEventItem + 1;
		++m_nMaxReNewEventItem;
		
	} 
	
	return TRUE;
}
BOOL	CProject::LoadReNewEvent( LPCTSTR lpszFileName )
{
	CScript script;

	if( script.Load(lpszFileName) == FALSE )
	{
		Message( "LoadReNewEvent Not Open" );
		return FALSE;	
	}

	DWORD dwCount = 0;
	ZeroMemory( m_aReNewEvent, sizeof( m_aReNewEvent ) );
	ZeroMemory( m_nReNewItemFound, sizeof( m_nReNewItemFound ) );
	m_tReNewEventStartTime = CTime::GetCurrentTime();
	m_tReNewEventEndTime = CTime::GetCurrentTime();

	int nlastLevel = 0;
	float fTotalPercent = 10000;
	
	int nLevel = script.GetNumber();
	while( script.tok != FINISHED )
	{
		m_aReNewEvent[dwCount].nLevel = nLevel;
		if( nlastLevel != m_aReNewEvent[dwCount].nLevel )
		{
			if( fTotalPercent < 9000 )
			{
				Message( "LoadReNewEvent Not 100%" );
				return FALSE;
			}
			nlastLevel = m_aReNewEvent[dwCount].nLevel;
			m_nReNewItemFound[ nlastLevel ] = dwCount;
			fTotalPercent = 0;
		}

		m_aReNewEvent[dwCount].dwItemId = script.GetNumber();
		script.GetToken();
		if( strlen( script.Token ) > 64 )
		{
			Message( "LoadReNewEvent Not Name > 64 %s", script.Token );
			return FALSE;
		}
		strcpy( m_aReNewEvent[dwCount].strItemName, script.Token );
		fTotalPercent += ( script.GetFloat() * 100 );
		m_aReNewEvent[dwCount].fPercent = fTotalPercent;
		nLevel = script.GetNumber();
		dwCount++;
		if( MAX_RENEWEVENT <= dwCount )
		{
			Message( "LoadReNewEvent %d Overflow", MAX_RENEWEVENT );
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __WORLDSERVER

CCtrl* CProject::GetCtrl( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return pCtrl;
	return NULL;
}
CSfx* CProject::GetSfx( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( pCtrl->GetType() == OT_SFX ? (CSfx*)pCtrl : NULL );
	return NULL;
}
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


/*
CPartsMng::CPartsMng()
{
}
CPartsMng::~CPartsMng()
{
}
BOOL CPartsMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	//LPHEADSET lpHeadset;
	LPSKINSET lpSkinset;
	int nIndex1;
	int nIndex2;
	m_Gender[ 0 ].m_nHairMeshSize = 0;
	m_Gender[ 1 ].m_nHairMeshSize = 0;
	m_Gender[ 0 ].m_nSkinSetSize = 0;
	m_Gender[ 1 ].m_nSkinSetSize = 0;
	script.GetToken(); 
	int nSex = 0;
	while( script.tok != FINISHED )
	{
		if( script.Token == _T( "gender" ) )
			nSex = script.GetNumber();
		if( script.Token == _T( "hairMesh" ) )
		{
			script.GetToken(); // {
			nIndex1 = script.GetNumber();
			while( *script.token != '}' )
			{
				if( nIndex1 > m_Gender[ nSex ].m_nHairMeshSize )
					m_Gender[ nSex ].m_nHairMeshSize = nIndex1;
				script.GetToken();
				_tcscpy( m_Gender[ nSex ].m_szHairMeshName[ nIndex1 ],  script.token );
				nIndex1 = script.GetNumber();
			}
		}
		else

		if( script.Token == _T( "skinSet" ) )
		{
			script.GetToken(); // {
			nIndex1 = script.GetNumber();
			while( *script.token != '}' )
			{
				if( nIndex1 > m_Gender[ nSex ].m_nSkinSetSize )
					m_Gender[ nSex ].m_nSkinSetSize = nIndex1;
				lpSkinset = &m_Gender[ nSex ].m_skinSet[ nIndex1 ];
				script.GetToken(); // {
				script.GetToken(); _tcscpy( lpSkinset->szHeadMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szUpperMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szLowerMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szHandMeshName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szFootMeshName, script.token );

				script.GetToken(); _tcscpy( lpSkinset->szUpperTextureName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szLowerTextureName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szHandTextureName, script.token );
				script.GetToken(); _tcscpy( lpSkinset->szFootTextureName, script.token );

				//script.GetToken(); _tcscpy( lpSkinset->szUpperUnderName, script.token );
				//script.GetToken(); _tcscpy( lpSkinset->szLowerUnderName, script.token );

				nIndex2 = script.GetNumber();
				while( *script.token != '}' )
				{
					script.GetToken();
					_tcscpy( lpSkinset->szHeadTextureName[ nIndex2 ], script.token );
					nIndex2 = script.GetNumber();
				}
				nIndex1 = script.GetNumber();
			}
		}
		script.GetToken();
	}
	m_Gender[ 0 ].m_nHairMeshSize++;
	m_Gender[ 1 ].m_nHairMeshSize++;
	m_Gender[ 0 ].m_nSkinSetSize++;
	m_Gender[ 1 ].m_nSkinSetSize++;
	return TRUE;
}
LPCTSTR CPartsMng::GetHairMeshName( int nSex, DWORD dwHairMeshIdx )
{
	return m_Gender[ nSex ].m_szHairMeshName[ dwHairMeshIdx ];
}
LPSKINSET CPartsMng::GetSkinSet( int nSex, DWORD dwSkinIdx )
{
	return &m_Gender[ nSex ].m_skinSet[ dwSkinIdx ];
}
*/
#ifdef __WORLDSERVER

CUser* CProject::GetUser( OBJID objid )
{
	CCtrl* pCtrl;
	if( m_objmap.Lookup( objid, pCtrl ) )
		return ( ( pCtrl->GetType() == OT_MOVER && ( (CMover*)pCtrl )->IsPlayer() ) ? (CUser*)pCtrl : NULL );
	return NULL;
}
#endif	

ObjProp* CProject::GetProp( int nType, int nIndex )
{
	switch( nType )
	{
	case OT_OBJ: return NULL;//m_aPropObj.GetAt( nIndex ); 
	case OT_CTRL: return m_aPropCtrl.GetAt( nIndex ); 
	case OT_SFX: return m_aPropSfx.GetAt( nIndex ); 
	case OT_ITEM: return m_aPropItem.GetAt( nIndex ); 
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
	strcpy( m_aMonsterProp[ m_nMonsterPropSize ].szMonsterName, MonsterProp.szMonsterName );
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
		strcpy( m_aRemoveProp[m_nRemoveMonsterPropSize], lpszMonsterName );
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
			DROPITEM di;
			DWORD dwMinLevel, dwMaxLevel;
			memset( &di, 0, sizeof(DROPITEM) );
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

			for( int i = 0; i < m_nMoverPropSize; i++ )
			{
				MoverProp* pProp	= m_pPropMover + i;
				if( pProp->dwID && pProp->dwLevel >= dwMinLevel && pProp->dwLevel <= dwMaxLevel	)
					pProp->m_DropItemGenerator.AddTail( di );	// copy
			}
		}
	} while( s.tok != FINISHED );

	return TRUE;
}
#endif	// __WORLDSERVER

#endif	// not define __XTOOL

#ifdef __WORLDSERVER
CGiftboxMan::CGiftboxMan()
{
	m_nSize	= 0;
	memset( &m_giftbox, 0, sizeof(m_giftbox) );
}

CGiftboxMan* CGiftboxMan::GetInstance( void )
{
	static CGiftboxMan sGiftboxMan;
	return &sGiftboxMan;
}

BOOL CGiftboxMan::AddItem( DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum )
{
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwGiftbox );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_nSize++;
		m_mapIdx.insert( map<DWORD, int>::value_type( dwGiftbox, nIdx1 ) );
	}

	if( m_nSize >= MAX_GIFTBOX )
	{
		OutputDebugString( "TOO MANY GIFTBOX" );
		return FALSE;
	}

	m_giftbox[nIdx1].dwGiftbox	= dwGiftbox;
	int nIdx2	= m_giftbox[nIdx1].nSize++;
	m_giftbox[nIdx1].adwItem[nIdx2]	= dwItem;
	m_giftbox[nIdx1].anNum[nIdx2]	= nNum;
	m_giftbox[nIdx1].nSum	+= dwProbability;
	m_giftbox[nIdx1].adwProbability[nIdx2]	= m_giftbox[nIdx1].nSum;

	return TRUE;
}

BOOL CGiftboxMan::Open( DWORD dwGiftbox, LPDWORD pdwItem, int* pnNum )
{
#ifdef __GIFTBOX0715
	DWORD dwRand	= xRandom( 1000000 );
#else	// __GIFTBOX0715
	DWORD dwRand	= xRandom( 10000 );
#endif	// __GIFTBOX0715
	map<DWORD, int>::iterator i		= m_mapIdx.find( dwGiftbox );
	if( i == m_mapIdx.end() )
		return 0;
	int nIdx	= i->second;
	PGIFTBOX pBox	= &m_giftbox[nIdx];

	int low	= 0;
	for( int j = 0; j < pBox->nSize; j++ )
	{
		if( dwRand >= low && dwRand < pBox->adwProbability[j] )
		{
			*pdwItem	= pBox->adwItem[j];
			*pnNum	= pBox->anNum[j];
			return TRUE;
		}
	}
	return FALSE;
}

void CGiftboxMan::Verify( void )
{
	for( int i = 0; i < m_nSize; i++ )
	{
		TRACE( "GIFTBOX : %d, %d\n", m_giftbox[i].dwGiftbox, m_giftbox[i].nSum );
#ifdef __GIFTBOX0715
		m_giftbox[i].adwProbability[m_giftbox[i].nSize-1]	+= ( 1000000 - m_giftbox[i].nSum );
#else	// __GIFTBOX0715
		m_giftbox[i].adwProbability[m_giftbox[i].nSize-1]	+= ( 10000 - m_giftbox[i].nSum );
#endif	// __GIFTBOX0715
	}
}

BOOL CProject::LoadGiftbox( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwGiftbox, dwItem, dwProbability;
	int	nNum;

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
#ifdef __INSTANT_BANK0715
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability * 100, nNum ) )
#else	// __INSTANT_BANK0715
				if( !CGiftboxMan::GetInstance()->AddItem( dwGiftbox, dwItem, dwProbability, nNum ) )
#endif	// __INSTANT_BANK0715
					return FALSE;
				dwItem	= s.GetNumber();
			}
		}
#ifdef __INSTANT_BANK0715
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
#endif	// __INSTANT_BANK0715
		s.GetToken();
	}
	CGiftboxMan::GetInstance()->Verify();
	return TRUE;
}

#endif	// __WORLDSERVER

#ifdef __J0524
CPackItem::CPackItem()
{
	m_nSize	= 0;
	memset( &m_packitem, 0, sizeof(m_packitem) );
}

CPackItem* CPackItem::GetInstance( void )
{
	static CPackItem sPackItem;
	return &sPackItem;
}

BOOL CPackItem::AddItem( DWORD dwPackItem, DWORD dwItem, int nAbilityOption, int nNum )
{
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwPackItem );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_nSize++;
		m_mapIdx.insert( map<DWORD, int>::value_type( dwPackItem, nIdx1 ) );
	}

	if( m_nSize >= MAX_PACKITEM )
	{
		OutputDebugString( "TOO MANY PACKITEM" );
		return FALSE;
	}

	if( m_packitem[nIdx1].nSize == MAX_ITEM_PER_PACK )
	{
		OutputDebugString( "TOO MANY ITEM PER PACK" );
		return FALSE;
	}

	m_packitem[nIdx1].dwPackItem	= dwPackItem;
	int nIdx2	= m_packitem[nIdx1].nSize++;
	m_packitem[nIdx1].adwItem[nIdx2]	= dwItem;
	m_packitem[nIdx1].anAbilityOption[nIdx2]	= nAbilityOption;
	m_packitem[nIdx1].anNum[nIdx2]	= nNum;
	return TRUE;
}

PPACKITEMELEM CPackItem::Open( DWORD dwPackItem )
{
	map<DWORD, int>::iterator i		= m_mapIdx.find( dwPackItem );
	if( i != m_mapIdx.end() )
		return &m_packitem[i->second];	
	return NULL;
}

BOOL CProject::LoadPackItem( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;

	DWORD dwPackItem, dwItem;
	int nAbilityOption, nNum;

#ifdef __FINITEITEM0705
	int	nSpan;
#endif	// __FINITEITEM0705

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "PackItem" ) )
		{
			dwPackItem	= s.GetNumber();
#ifdef __FINITEITEM0705
			nSpan	= s.GetNumber();
#endif	// __FINITEITEM0705
			s.GetToken();	// {
			dwItem	= s.GetNumber();
			while( *s.token != '}' )
			{
				nAbilityOption	= s.GetNumber();
				nNum	= s.GetNumber();
				if( !CPackItem::GetInstance()->AddItem( dwPackItem, dwItem, nAbilityOption, nNum ) )
					return FALSE;
				dwItem	= s.GetNumber();
			}
#ifdef __FINITEITEM0705
			PPACKITEMELEM pPackItemElem	= CPackItem::GetInstance()->Open( dwPackItem );
			if( pPackItemElem )
				pPackItemElem->nSpan	= nSpan;
#endif	// __FINITEITEM0705
		}
		s.GetToken();
	}
	return TRUE;
}
#endif	// __J0524

#ifdef __J0608
PSETITEMAVAIL CProject::GetSetItemAvail( int nAbilityOption )
{
	if( nAbilityOption < 0 || nAbilityOption > 10 )
		return NULL;
	return &m_aSetItemAvail[nAbilityOption-1];
}
#endif	// __J0608

#ifdef __PIERCING_SM_TIME
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
#ifdef __SETITEM0624
		else if( s.Token == _T( "SetItem" ) )
		{
			int nId;
			char pszString[MAX_SETITEM_STRING];
			nId		= s.GetNumber();
			//
			s.GetToken();
			if( m_nLanguage == LANG_KOR )
				lstrcpy( pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_ENG )
				lstrcpy( pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_JAP )
				lstrcpy( pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_CHI )
				lstrcpy( pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_THA )
				lstrcpy( pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_TWN )
				lstrcpy( pszString, s.Token );

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
						pSetItem->AddSetItemElem( dwItemId, nParts );
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
#endif	// __SETITEM0624
#ifdef __RANDOMOPTITEM0628
		else if( s.Token == _T( "RandomOptItem" ) )
		{
			RANDOMOPTITEM randomOptItem;
			randomOptItem.nId	= s.GetNumber();
			//
			s.GetToken();
			if( m_nLanguage == LANG_KOR )
				lstrcpy( randomOptItem.pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_ENG )
				lstrcpy( randomOptItem.pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_JAP )
				lstrcpy( randomOptItem.pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_CHI )
				lstrcpy( randomOptItem.pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_THA )
				lstrcpy( randomOptItem.pszString, s.Token );
			s.GetToken();
			if( m_nLanguage == LANG_TWN )
				lstrcpy( randomOptItem.pszString, s.Token );

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
#endif	// __RANDOMOPTITEM0628
		s.GetToken();
	}
#ifdef __RANDOMOPTITEM0628
	CRandomOptItemGen::GetInstance()->Arrange();
#endif	// __RANDOMOPTITEM0628

	return TRUE;
}

CPiercingAvail::CPiercingAvail()
{
	m_nSize		= 0;
	memset( &m_pPiercingAvail, 0, sizeof(m_pPiercingAvail) );
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
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwItemId );
	int nIdx1	= 0;
	if( i != m_mapIdx.end() )
	{
		nIdx1	= i->second;
	}
	else
	{
		nIdx1	= m_nSize++;
		m_mapIdx.insert( map<DWORD, int>::value_type( dwItemId, nIdx1 ) );
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
	map<DWORD, int>::iterator i	= m_mapIdx.find( dwItemId );
	if( i != m_mapIdx.end() )
		return &m_pPiercingAvail[i->second];
	return NULL;
}
#endif	// __PIERCING_SM_TIME

#ifdef __SETITEM0624
CSetItem::CSetItem( int nId, const char* pszString )
{
	m_nId	= nId;
	lstrcpy( m_pszString, pszString );
	m_nElemSize		= 0;
}

BOOL CSetItem::AddSetItemElem( DWORD dwItemId, int nParts )
{
	if( m_nElemSize == MAX_SETITEM_ELEM )
	{
		TRACE( "too many setitem elements\n" );
		return FALSE;
	}
#ifdef _DEBUG
	for( int i = 0; i < m_nElemSize; i++ )
	{
		if( m_anParts[i] == nParts )
		{
			TRACE( "parts duplicated\n" );
			return FALSE;
		}
	}
#endif	// _DEBUG
	m_adwItemId[m_nElemSize]	= dwItemId;
	m_anParts[m_nElemSize]		= nParts;
	m_nElemSize++;
	return TRUE;
}

BOOL CSetItem::AddItemAvail( int nDstParam, int nAdjParam, int nEquiped )
{
	if( m_avail.nSize == MAX_ITEMAVAIL )
	{
		TRACE( "too many setitem avail\n" );
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
	for( map<int, CSetItem*>::iterator i	= m_mapSetId.begin(); i != m_mapSetId.end(); ++i )
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
	bool	bResult	= m_mapSetId.insert( map<int, CSetItem*>::value_type( pSetItem->m_nId, pSetItem ) ).second;
	if( !bResult )
	{
		//
		TRACE( "adding setitem failed\t// 0\n" );
	}
	for( int i = 0; i < pSetItem->m_nElemSize; i++ )
	{
		bResult	= m_mapItemId.insert( map<DWORD, CSetItem*>::value_type( pSetItem->m_adwItemId[i], pSetItem ) ).second;
		if( !bResult )
		{
			TRACE( "adding setitem failed\t// 1\n" );
		}
	}
}

CSetItem* CSetItemFinder::GetSetItem( int nSetItemId )
{
	map<int, CSetItem*>::iterator i	= m_mapSetId.find( nSetItemId );
	if( i != m_mapSetId.end() )
		return i->second;
	return NULL;
}

CSetItem* CSetItemFinder::GetSetItemByItemId( DWORD dwItemId )
{
	map<DWORD, CSetItem*>::iterator i	= m_mapItemId.find( dwItemId );
	if( i != m_mapItemId.end() )
		return i->second;
	return NULL;
}	
#endif	// __SETITEM0624

#ifdef __RANDOMOPTITEM0628
CRandomOptItemGen::CRandomOptItemGen()
{
	int m_nSize		= 0;
	memset( m_aRandomOptItem, 0, sizeof(m_aRandomOptItem) );
	memset( m_anIndex, 0, sizeof(m_anIndex) );
}

void CRandomOptItemGen::Free( void )
{
	m_mapid.clear();
}

BOOL CRandomOptItemGen::AddRandomOption( PRANDOMOPTITEM pRandomOptItem )
{
	if( m_nSize == MAX_RANDOMOPTITEM )
	{
		TRACE( "adding randomoptitem\t// 1\n" );
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
	for( i = 0; i  < m_nSize; i++ )
	{
		bool bResult	= m_mapid.insert( map<int, int>::value_type( m_aRandomOptItem[i].nId, i ) ).second;
		if( !bResult )
		{
			TRACE( "adding randomoptitem failed\t// 0\n" );
		}
		
		if( m_aRandomOptItem[i].nLevel > nLevel )
		{
			for( int j = nLevel; j < m_aRandomOptItem[i].nLevel; j++ )
				m_anIndex[j-1]	= nPrevious;
			nLevel	= m_aRandomOptItem[i].nLevel;
		}
		nPrevious	= i;
	}
	for( i = nLevel; i <= MAX_LEVEL; i++ )
		m_anIndex[i-1]	= nPrevious;
}

int CRandomOptItemGen::GenRandomOptItem( int nLevel, FLOAT fPenalty, ItemProp* pItemProp )
{
	if( !pItemProp )
		return 0;
	
	if( pItemProp->dwItemKind1 != IK1_WEAPON && pItemProp->dwItemKind1 != IK1_ARMOR )
		return 0;

	int i	= m_anIndex[nLevel];
	if( i != -1 )
	{
		int nIndex	= xRandom( i + 1 );
		DWORD dwRandom	= xRandom( 3000000000 );
		DWORD dwProbability	= (DWORD)(m_aRandomOptItem[nIndex].dwProbability * fPenalty);
		if( dwRandom < dwProbability )
			return m_aRandomOptItem[nIndex].nId;
	}
	return 0;
}

PRANDOMOPTITEM CRandomOptItemGen::GetRandomOptItem( int nId )
{
	map<int, int>::iterator i	= m_mapid.find( nId );
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
#endif	// __RANDOMOPTITEM0628

#ifndef __BEAST
BOOL CProject::IsGuildQuestRegion( const D3DXVECTOR3 & vPos )
{
	CRect rect;
	POINT point;
	for( int i = 0; i < m_aPropGuildQuest.GetSize(); i++ )
	{
		GUILDQUESTPROP* pProp	= m_aPropGuildQuest.GetAt( i );
		if( pProp == NULL )
			continue;
		
		rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
		point.x = (int)vPos.x;
		point.y = (int)vPos.z;
		if( rect.PtInRect( point ) )
			return TRUE;
	}
	return FALSE;
}
#endif //__BEAST

//raiders_060112
//LoadPropItem를 호출하고 후처리를 한다.
void CProject::OnAfterLoadPropItem()
{
	for( int i = 0; i < m_aPropItem.GetSize(); i++ )
	{
		ItemProp* pItemProp = (ItemProp*)m_aPropItem.GetAt( i );
		if( pItemProp && pItemProp->dwFlag == NULL_ID )
			pItemProp->dwFlag = 0;

		if( pItemProp && pItemProp->dwItemKind3 != NULL_ID )
		{
			m_itemKindAry[ pItemProp->dwItemKind3 ].Add( pItemProp );

			switch( pItemProp->dwItemKind3 )
			{
			case IK3_EVENTMAIN:
			case IK3_PET:
			case IK3_BINDS:
				pItemProp->dwFlag |= IP_FLAG_BINDS;
				break;
			}
		}
	}


#ifndef __V050515_PROPLASTFULL
	ItemProp* ptmp;
	for( i = 0; i < MAX_ITEM_KIND3; i++ )
	{
		// sort
		for( int j = 0; j < m_itemKindAry[i].GetSize() - 1; j++ )
		{
			for( int k = j + 1; k < m_itemKindAry[i].GetSize(); k++ )
			{
				if( ( (ItemProp*)m_itemKindAry[i].GetAt( k ) )->dwItemRare < ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
				{
					ptmp	= (ItemProp*)m_itemKindAry[i].GetAt( j );
					m_itemKindAry[i].SetAt( j, (void*)m_itemKindAry[i].GetAt( k ) );
					m_itemKindAry[i].SetAt( k, (void*)ptmp );
				}
			}
		}
		//
		DWORD dwItemRare	= (DWORD)-1;
		for( j = 0; j < m_itemKindAry[i].GetSize(); j++ )
		{
			if( dwItemRare != ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare )
			{
				dwItemRare	= ( (ItemProp*)m_itemKindAry[i].GetAt( j ) )->dwItemRare;
				if( dwItemRare != (DWORD)-1 )
				{
					m_minMaxIdxAry[i][dwItemRare].cx	= j;
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
				}
			}
			else
			{
				if( dwItemRare != (DWORD)-1 )
					m_minMaxIdxAry[i][dwItemRare].cy	= j;
			}
		}
	}
#endif
}