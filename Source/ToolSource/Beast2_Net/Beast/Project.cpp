// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "defineItem.h"
#include "lang.h"

#include "Ship.h"

#include "guild.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef		__XTOOL

// state를 사용하는 퀘스트를 저장한다
// 최신 버젼의 퀘스트 시스템은 state 사용을 지양한다.
// 따라서 기존에 state를 사용하던 테이블을 저장하여 향후 임의로 state가 사용될 때
// 이것과 비교하여 에러를 발생시켜, 개발자들로 하여금 state 사용을 원천 봉쇄한다.
// 만약 불가피하게 사용할 필요가 있다면 별도로 프로그래머들에게 요청한다.

CString GetLangFileName( int nLang, int nType )
{
	char* file[] = { "Filter", "InvalidName", "Notice" };
	CString fileName = file[nType];

	switch( nLang )
	{
	case LANG_KOR:
		fileName = fileName+"_KOR.inc";
		break;
	//case LANG_ENG:
	//	fileName = fileName+"_ENG.inc";
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
	ASSERT( nIndex < (int)( m_dwSize ) );
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
	ASSERT( nIndex < (int)( m_uSize ) );
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

	m_nMoverPropSize = 0;
//	m_pPropMover = new MoverProp[MAX_PROPMOVER];
	m_pPropMover = (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );
}

CProject::~CProject()
{

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
#ifdef __USE_STR_TBL0407
		else if( scanner.Token == _T( "string" ) )
		{
			scanner.GetToken();
			CScript script;
			if( script.Load( scanner.Token ) == TRUE )
				script.LoadString();
		}
#endif	// __USE_STR_TBL0407
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

			LoadPropCtrl( "propCtrl.txt", &m_aPropCtrl );
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
			BOOL	bResult = FALSE;

			scanner.GetToken();
			bResult = LoadCharacter( scanner.token );

			if( bResult == FALSE )
			{
				return	FALSE;
			}
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

	} while(scanner.tok!=FINISHED);

// 비스트에선 읽지 않는다.

	return TRUE;
}


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
#ifdef __USE_STR_TBL0407
			scanner.GetToken();
			CString str	= scanner.token;
			str.Replace( "\"", "" );
			if( str.IsEmpty() )
				str	= "Empty";
			#ifdef _DEBUG
			if( strArray.GetSize() > (int)( dwId ) )
				if( strArray.GetAt( dwId ).IsEmpty() == FALSE )
					Error( "CProject::LoadText : 같은 아이디 존재 %d - %s", dwId, str );						
			#endif	// _DEBUG	
			strArray.SetAtGrow( dwId, str );
			colorArray.SetAtGrow( dwId, dwColor );
			scanner.GetToken();	// }
#else	// __USE_STR_TBL0407
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
#endif	//__USE_STR_TBL0407
		}
		dwId = scanner.GetNumber();
#ifdef _DEBUG
		if( dwId == 925 )
		{
			int a = 0;
		}
#endif		
	} while( scanner.tok != FINISHED );

	for( DWORD i = 0; (int)( i ) < strArray.GetSize(); i++ )
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
			pColorText->lpszData = _strdup( strArray.GetAt( i ) ) ;
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
#ifdef __USE_STR_TBL0407
				script.GetToken();
				_tcscpy( m_aJob[ id ].szName, script.token );
#else	// __USE_STR_TBL0407
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
#endif	// __USE_STR_TBL0407
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
#ifdef __USE_STR_TBL0407
				script.GetToken();
				_tcscpy( m_aStructure[ id ].szName, script.token );
#else	// __USE_STR_TBL0407
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
#endif	// __USE_STR_TBL0407
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
#ifdef __USE_STR_TBL0407
				script.GetToken();
				_tcscpy( m_aGuildAppell[ id ].szName, script.token );
#else	// __USE_STR_TBL0407
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
#endif	// __USE_STR_TBL0407
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
#ifdef __USE_STR_TBL0407
		script.GetToken();
		_tcscpy( MotionProp.szName, script.m_mszToken );
		script.GetToken();
		_tcscpy( MotionProp.szDesc, script.m_mszToken );		
#else	// __USE_STR_TBL0407
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
#endif	// __USE_STR_TBL0407

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


CString CProject::GetLangScript( CScript& script )
{
#ifdef __USE_STR_TBL0407
	CString string;
	script.GetToken();
	string	= script.Token;
	script.GetToken();	// )
	script.GetToken(); // ;
	if( string.IsEmpty() )
		string = " ";
	return string;
#else	// __USE_STR_TBL0407
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
#endif	// __USE_STR_TBL0407
}


BOOL CProject::LoadPropGuildQuest( LPCTSTR lpszFilename )
{
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

			if( script.Token == "SetDialog" )
			{
				script.GetToken(); // (
				int nNum = script.GetNumber();
				script.GetToken(); // ,
				CString string = GetLangScript( script );
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

			if( _strcmpi( script.Token, "AI" ) == 0 )		// AI블럭
			{
				BOOL bRet = LoadPropMoverEx_AI( szFileName, script, nVal );
				if ( bRet == FALSE )
				{
					Error( "%s : MoverID %s(%d)의 AI{} 블럭 읽기 실패", szFileName, pProp->szName, nVal );
					return FALSE;
				}
			}
#else // xAI1013
			if( _strcmpi( script.Token, "AI" ) == 0 )		// AI블럭 건너뜀.
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
				strcpy( lpCharacter->m_szChar, filename.GetString() ); 
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
				strcpy( lpCharacter->m_szChar, filename.GetString() );
			}
			else
			if(script.Token == "AddMenu" )
			{
				script.GetToken(); // (
				int nMMI = script.GetNumber(); 
				if( nMMI < 0 || nMMI > MAX_MOVER_MENU )
				{
					CString str;
					str.Format( "LoadCharacter( %s ) 잘못된 MMI ID (%s)[%d] 입니다.", szFileName, script.GetDefine(), nMMI);
					AfxMessageBox( str );

					return	FALSE;
				}

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
	u_long idPlayer		= 0;
	STRING2ULONG::iterator i	= m_str2ulong.find( lpszPlayer );
	if( i != m_str2ulong.end() )
		idPlayer	= i->second;
//		return( i->second );
//	return 0;

	return idPlayer;
}

LPCSTR CProject::GetPlayerString( u_long idPlayer )
{
	LPCSTR pszPlayer	= NULL;
	ULONG2STRING::iterator i	= m_ulong2str.find( idPlayer );
	if( i != m_ulong2str.end() )
		pszPlayer	= i->second.data();
//		return( i->second.data() );
//	return NULL;

	return pszPlayer;
}

BOOL CProject::SetPlayerID( u_long idPlayer, const CHAR* lpszPlayer )
{
	if( GetPlayerString( idPlayer ) == NULL )
	{
//		m_str2ulong[lpszPlayer]		= idPlayer;
//		m_ulong2str[idPlayer]	= lpszPlayer;
		m_str2ulong.insert( STRING2ULONG::value_type( lpszPlayer, idPlayer ) );
		m_ulong2str.insert( ULONG2STRING::value_type( idPlayer, lpszPlayer ) );

		return TRUE;
	}
	ASSERT( 0 );

	return FALSE;
}

BOOL CProject::RemovePlayerID( u_long idPlayer )
{
	const char* lpszPlayer	= GetPlayerString( idPlayer );
	if( NULL != lpszPlayer )
	{
		m_str2ulong.erase( lpszPlayer );
		m_ulong2str.erase( idPlayer );
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

#endif	// not define __XTOOL


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

		s.GetToken();
	}

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