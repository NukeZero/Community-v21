#include "stdafx.h"
#include "project.h"
//#include "company.h"
#include <lang.h>

#include "couplehelper.h"

#include <guild.h>
extern	CGuildMng	g_GuildMng;
//extern	CCompanyMng		g_CompanyMng;

#include <Tax.h>
#include <langman.h>
#include "tlord.h"
#include <honor.h>
#include <Quiz.h>

#include "FLSpec.h"

#ifdef __GUILD_HOUSE_MIDDLE
#include <GuildHouse.h>
#endif // __GUILD_HOUSE_MIDDLE

CProject::CProject()
{
	memset( m_apszWorld, 0, sizeof(m_apszWorld) );
//	for( int i = 0; i < MAX_WORLD; i++ )
//		m_apszWorld[i][0]	= '\0';
//	m_mapII.SetSize( 128, 128, 128 );
	m_nMoverPropSize	= 0;
	m_pPropMover	= new MoverProp[MAX_PROPMOVER];
//	m_pPropMover	= (MoverProp*)::VirtualAlloc( NULL, sizeof(MoverProp) * MAX_PROPMOVER, MEM_COMMIT, PAGE_READWRITE );

	memset( m_aJobSkill, 0, sizeof( m_aJobSkill ) );
	memset( m_aJobSkillNum, 0, sizeof( m_aJobSkillNum ) );


	m_fItemDropRate = 1.0f;
	m_fGoldDropRate = 1.0f;
	m_fMonsterExpRate = 1.0f;
	m_fMonsterHitRate = 1.0f;
	m_fShopCost	= 1.0f;
#ifdef __S1108_BACK_END_SYSTEM
	m_fMonsterRebirthRate = 1.0f;	
	m_fMonsterHitpointRate = 1.0f;	
	m_fMonsterAggressiveRate = 0.2f;
	m_fMonsterRespawnRate = 1.0f;
	m_bBaseGameSetting = FALSE;
	m_nMonsterRespawnSize = 0;
	m_nMonsterPropSize = 0;
	m_nAddMonsterRespawnSize = 0;
	m_nAddMonsterPropSize = 0;
	m_nRemoveMonsterPropSize = 0;
	m_bBackEndSystem = FALSE;
	ZeroMemory( m_chBackEndSystemTime, sizeof( m_chBackEndSystemTime ) );
	ZeroMemory( m_chBackEndSystemChatTime, sizeof( m_chBackEndSystemChatTime ) );
	ZeroMemory( m_chGMChat, sizeof( m_chGMChat ) );
	ZeroMemory( m_aRemoveProp, sizeof( m_aRemoveProp ) );
#endif // __S1108_BACK_END_SYSTEM

	m_bItemUpdate = FALSE;
	m_dwConvMode = 0;
}

CProject::~CProject()
{
	for( CMapStrToPtr::iterator i	= m_mapBeginPos.begin(); i != m_mapBeginPos.end(); ++i )
		SAFE_DELETE_CONST( (BEGINPOSARR*)i->second );
//	if( m_pPropMover )
//		::VirtualFree( m_pPropMover, sizeof(MoverProp) * MAX_PROPMOVER, MEM_DECOMMIT );
	SAFE_DELETE_ARRAY( m_pPropMover );
	for( int j = 0; j < m_colorText.GetSize(); j++ )
	{
		tagColorText* lpText = m_colorText.GetAt( j );
		if( lpText && lpText->lpszData )
			free( lpText->lpszData ); 
	}

	m_sInvalidNames.clear();
	m_sAllowedLetter.clear();
#ifdef __VENDOR_1106
	m_sAllowedLetter2.clear();
#endif	// __VENDOR_1106
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

BOOL CProject::OpenProject( LPCTSTR lpszFileName )
{
	CScanner s;
	if( s.Load( lpszFileName )	== FALSE )
		return FALSE;

	if( LoadPreFiles() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "FALIED LOAD PRE FILES" ) );
		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	// All Spec File Load
	if( g_xSpecManager->LoadSpecAll() == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "LoadSpecAll Failed." ) );
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////


	do {
		s.GetToken();

// 		if( s.Token == _T( "propItem" ) )
// 		{
// 			s.GetToken();
// 			LoadPropItem( s.token, &m_aPropItem );
// 		}
		if( s.Token == _T( "propMover" ) )
		{
			s.GetToken();
			LoadPropMover( s.token );
		}
		else if( s.Token == _T( "propSkill" ) )
		{
			s.GetToken();
			LoadPropSkill( s.token, &m_aPropSkill );
			memset( m_aJobSkillNum, 0, sizeof(m_aJobSkillNum ) );
			for( int i = 1; i < m_aPropSkill.GetSize(); i++ )
			{
				SkillProp* pSkillProp	= (SkillProp*)m_aPropSkill.GetAt( i );
				if( pSkillProp )
				{
					if( pSkillProp->dwSkillKind2 == NULL_ID )
					{
						FLERROR_LOG( PROGRAM_NAME, _T( "ItmeProp(%s) dwItemKind2 = -1" ), pSkillProp->szName );
						return FALSE;
					}

					if( pSkillProp->dwSkillKind1 == JTYPE_COMMON )
						continue;

					SkillProp** apJobSkill	= m_aJobSkill[pSkillProp->dwSkillKind2];
					apJobSkill[m_aJobSkillNum[pSkillProp->dwSkillKind2]]	= pSkillProp;
					m_aJobSkillNum[pSkillProp->dwSkillKind2]++;
				}
				else
				{
					FLTRACE_LOG( PROGRAM_NAME, _T( "PropSkill Prop NULL SkillIndex = %d now no problem" ), i );
				}
			}
			for( int i = 0; i < MAX_JOB; i++ )
			{
				SkillProp** apJobSkill	= m_aJobSkill[i];
				qsort( (void*)apJobSkill, (size_t)m_aJobSkillNum[i], sizeof(SkillProp*), SortJobSkill );
			}
		}
		else if( s.Token == _T( "world" ) )
		{
			s.GetToken();
			LoadDefOfWorld( s.token );
		}
		else if( s.Token == _T( "expTable" ) )
		{
			s.GetToken();
			LoadExpTable( s.token );
		}
	}	while( s.tok != FINISHED );

	LoadBeginPos();
	LoadJobItem( _T( "jobItem.inc" ) );

	m_EventLua.LoadScript();
	m_EventLua.CheckEventState();

	m_GuildCombat1to1.LoadScript();

	LoadInvalidName();
	LoadAllowedLetter();
#ifdef __VENDOR_1106
	LoadAllowedLetter( TRUE );
#endif	// __VENDOR_1106

	CPetProperty::GetInstance()->LoadScript( "pet.inc" );

	CTax::GetInstance()->LoadScript();

	CTitleManager::Instance()->LoadTitle("honorList.txt");
	//CTitleManager::Instance()->AddValue(MI_AIBATT1, HI_HUNT_MONSTER, 1);

	CCoupleHelper::Instance()->Initialize();

	CQuiz::GetInstance()->LoadScript();


	FLSpec::GetInstance().Load_CooperativeContributions();

#ifdef __GUILD_HOUSE_MIDDLE
	GuildHouseMng->LoadScript();
#endif // __GUILD_HOUSE_MIDDLE

	return TRUE;
}

void CProject::LoadBeginPos( void )
{
	WIN32_FIND_DATA ffdFoundData; 
	HANDLE hFound;
	CHAR lpFileName[MAX_PATH];

	BOOL bDoneWithHandle	= FALSE;
	hFound	= FindFirstFile( MakePath( DIR_WORLD, "*.*" ), (LPWIN32_FIND_DATA)&ffdFoundData );
	
	if( (HANDLE)( -1 ) == hFound )
		bDoneWithHandle	= TRUE;

	D3DXVECTOR3 vPos;
	DWORD dwIndex;

	while( !bDoneWithHandle )
	{
		if( ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) && ( 0 != strcmp( ".", ffdFoundData.cFileName ) ) && ( 0 != strcmp( "..", ffdFoundData.cFileName ) ) )
		{
			FLSPrintf( lpFileName, _countof( lpFileName ), "%s%s\\%s.rgn", DIR_WORLD, ffdFoundData.cFileName, ffdFoundData.cFileName );
			CScanner s;
			if( s.Load( lpFileName ) == TRUE )
			{
				s.GetToken();
				while( s.tok != FINISHED )
				{
					if( s.Token == _T( "region" ) ||  s.Token == _T( "region2" ) || s.Token == _T( "region3" ) )
					{
						BOOL bNewFormat = FALSE;
						BOOL bNewFormat3 = FALSE;
						if( s.Token == _T( "region2" ) )
							bNewFormat = TRUE;
						if( s.Token == _T( "region3" ) )
						{
							bNewFormat3 = TRUE;
							bNewFormat = TRUE;
						}

						s.GetNumber();	// type
						dwIndex		= s.GetNumber();
						vPos.x	= s.GetFloat();
						vPos.y	= s.GetFloat();
						vPos.z	= s.GetFloat();
						s.GetNumber();	s.GetNumber(); s.GetToken();	s.GetToken();	s.GetToken();	// attribute, id, script, music, sound
						s.GetToken();	s.GetFloat();	s.GetFloat();	s.GetFloat();	// teleport
						s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();	// rect
						s.GetToken();	s.GetNumber();	// key, target
						if( bNewFormat3 )
						{
							s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber();
							s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber(); s.GetNumber();
							s.GetNumber();
						}

						if( bNewFormat == FALSE )
						{
							char cbDesc	= static_cast< char >( s.GetNumber() );	// size
							for( int i = 0; i < cbDesc; i++ )
								s.GetToken();	// desc
						}
						else
						{
							s.GetToken(); // get "title"
							BOOL bDesc = s.GetNumber();
							if( bDesc )
							{
								CString string;
								s.GetToken(); // {
								do
									s.GetToken(); 
								while( *s.token != '}' );
							}
							s.GetToken(); // get "desc"
							bDesc = s.GetNumber();
							if( bDesc )
							{
								CString string;
								s.GetToken(); // {
								do
									s.GetToken(); 
								while( *s.token != '}' );
							}
						}
						if( REGION_INDEX( 12, RI_BEGIN ) == dwIndex ) 
						{
							FLStrLowercase( ffdFoundData.cFileName, _countof( ffdFoundData.cFileName ) );
							AddBeginPos( ffdFoundData.cFileName, vPos );
						}
					}
					else if( s.Token == _T( "respawn" ) )
					{
						// type, index, x, y, z, cb, time, left, top, right, bottom
						s.GetNumber();	s.GetNumber();	s.GetFloat();	s.GetFloat();	s.GetFloat();	s.GetNumber();	s.GetNumber();
						s.GetNumber();	// m_nActiveAttackNum
						s.GetNumber();	s.GetNumber();	s.GetNumber();	s.GetNumber();
					}
					s.GetToken();
				}
			}
		}
		bDoneWithHandle
			= !FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );
	}
	FindClose( hFound );
}

void CProject::AddBeginPos( const CHAR* lpszWorld, const D3DXVECTOR3 & vPos )
{
	// 맵이름으로 배열 찾기, 맵이름 없으면, 생성해서 넣은뒤, 첫번째 포스 배열에 좌표 넣기,

	CMapStrToPtr::iterator i	= m_mapBeginPos.find( lpszWorld );
	if( i == m_mapBeginPos.end() )
	{
		LPBEGINPOSARR lpBeginPosArr	= new BEGINPOSARR;
		lpBeginPosArr->uSize	= 0;
		lpBeginPosArr->avPos[lpBeginPosArr->uSize++]	= vPos;
		m_mapBeginPos[lpszWorld]	= (void*)lpBeginPosArr;
	}
	else
	{
		LPBEGINPOSARR lpBeginPosArr		= (LPBEGINPOSARR)i->second;
		lpBeginPosArr->avPos[lpBeginPosArr->uSize++]	= vPos;
	}
}

BOOL CProject::GetRandomBeginPos( DWORD dwWorldID, D3DXVECTOR3* pvOut )
{
	char* lpszWorld
		= m_apszWorld[dwWorldID];

	CMapStrToPtr::iterator i	= m_mapBeginPos.find( lpszWorld );
	if( i != m_mapBeginPos.end() )
	{
		LPBEGINPOSARR lpBeginPosArr		= (LPBEGINPOSARR)i->second;
		*pvOut	= lpBeginPosArr->avPos[xRandom( lpBeginPosArr->uSize )];
		return TRUE;
	}
	return FALSE;
}

BOOL CProject::LoadDefOfWorld( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return( FALSE );

	int nBrace	= 1;
	s.SetMark();
	int i	= s.GetNumber();	// folder or id

	while( nBrace )
	{
		if( MAX_WORLD <= i )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "LoadDefOfWorld i = %d Max값보다 많음" ), i );
			return FALSE;
		}

		if( *s.token == '}' || s.tok == FINISHED )
		{
			nBrace--;
			if( nBrace > 0 )
			{
				s.SetMark();
				i	= s.GetNumber();	// folder or id
				continue;
			}
			if( nBrace == 0 )
				continue;
		}
		s.GetToken();		// { or filename
		if( s.Token == "SetTitle" )
		{
			s.GetToken(); // (
			FLStrcpy( m_apszWorldName[i], _countof( m_apszWorldName[i] ), GetLangScript( s ) );
		}
		else
		{
			s.GoMark();
			i	= s.GetNumber(); // id
			
			s.GetToken();		// filename
			FLStrcpy( m_apszWorld[i], _countof( m_apszWorld[i] ), s.token );
			FLStrLowercase( m_apszWorld[i], _countof( m_apszWorld[i] ) );
		}
		s.SetMark();
		i	= s.GetNumber();	// texture fileName
	}

	return( TRUE );
}

BOOL CProject::LoadExpTable( LPCTSTR lpszFileName )
{
#ifdef __DBSERVER
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	int i	= 0;
	
	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		i	= 0;
		if( s.Token == _T( "expCompanyTest" ) )
		{
			CGuildTable::GetInstance().ReadBlock( s );
		}
		else if( s.Token == _T( "expParty" ) )
		{
			ZeroMemory( m_aExpParty, sizeof( m_aExpParty ) );
			s.GetToken();	// {
			DWORD dwVal	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_aExpParty[i].Exp	= dwVal;
				m_aExpParty[i++].Point	= s.GetNumber();
				dwVal	= s.GetNumber();
			}
		}

		s.GetToken();	// type name or }
	}
#endif	// __DBSERVER
	return TRUE;
}

DWORD	CProject::GetSkillPoint( const SkillProp* pSkillProp ) const
{
	DWORD dwPoint = 0;
	if( pSkillProp->dwSkillKind1 == JTYPE_BASE )
	{
		dwPoint = 1;
	}
	else if( pSkillProp->dwSkillKind1 == JTYPE_EXPERT )
	{
		dwPoint = 2;
	}

	else if( pSkillProp->dwSkillKind1 == JTYPE_PRO )
	{
		dwPoint = 3;
	}
	else if( pSkillProp->dwSkillKind1 == JTYPE_LEGEND_HERO )
	{
		dwPoint = 4;
	}
	else
	{
		dwPoint = 0;
	}

	return dwPoint;
}

CProject	prj;