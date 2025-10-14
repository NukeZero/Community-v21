#include "stdafx.h"



#include "guild.h"
#ifdef __CORESERVER
#include "player.h"
extern	CPlayerMng	g_PlayerMng;
#include "dpcachesrvr.h"
extern	CDPCacheSrvr	g_DPCacheSrvr;
#include "DPCoreSrvr.h"
extern  CDPCoreSrvr		g_dpCoreSrvr;
#endif	// __CORESERVER
#ifdef __WORLDSERVER
#include "DPDatabaseClient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#include "DPCoreClient.h"
extern  CDPCoreClient	g_DPCoreClient;
#include "user.h"
extern	CUserMng	g_UserMng;
#include "worldmng.h"
extern	CWorldMng	g_WorldMng;
#include "defineQuest.h"
#endif // __WORLDSERVER


#include "guildwar.h"
extern	CGuildWarMng	g_GuildWarMng;

//________________________________________________________________________________
CAr&  operator<<(CAr& ar, VOTE_INSERTED_INFO& info)
{
	ar << info.idGuild << info.idVote;
	ar.WriteString( info.szTitle );
	ar.WriteString( info.szQuestion );
	for( int i=0; i<4; ++i )
	{
		ar.WriteString( info.szSelections[i]);
	}			
	return ar;
}

CAr&  operator>>(CAr& ar, VOTE_INSERTED_INFO& info)
{
	ar >> info.idGuild >> info.idVote;
	ar.ReadString( info.szTitle );
	ar.ReadString( info.szQuestion );
	for( int i=0; i<4; ++i )
	{
		ar.ReadString( info.szSelections[i]);
	}			
	return ar;
}

//________________________________________________________________________________
CAr&  operator<<(CAr& ar, CONTRIBUTION_CHANGED_INFO& info)
{
	ar << info.idGuild << info.idPlayer << info.dwPxpCount << info.dwPenya;
	ar << info.dwGuildPxpCount << info.dwGuildPenya << info.nGuildLevel;
	return ar;
}

CAr&  operator>>(CAr& ar, CONTRIBUTION_CHANGED_INFO& info)
{
	ar >> info.idGuild >> info.idPlayer >> info.dwPxpCount >> info.dwPenya;
	ar >> info.dwGuildPxpCount >> info.dwGuildPenya >> info.nGuildLevel;
	return ar;
}

//________________________________________________________________________________

CGuildVote::CGuildVote()
{
	m_idVote = 0;				
	m_bCompleted = false;		
	m_szTitle[0] = '\0';		
	m_szQuestion[0] = '\0';		
	
	for( int i=0; i<4; ++i )
	{
		m_selects[i].szString[0] = '\0';
		m_selects[i].cbCount = 0;
	}
}

CGuildVote::~CGuildVote()
{
}

// VOTE_INSERTED_INFO 구조체를 이용해서 초기화하기 
void CGuildVote::Init( const VOTE_INSERTED_INFO& info, bool bCompleted, BYTE* cbCounts )
{
	m_idVote = info.idVote;						
	m_bCompleted = bCompleted;							
	memcpy( m_szTitle, info.szTitle, MAX_BYTE_VOTETITLE );			
	memcpy( m_szQuestion, info.szQuestion, MAX_BYTE_VOTEQUESTION );
	for( int i=0; i<4; i++ )
	{
		if( cbCounts )
			m_selects[i].cbCount = cbCounts[i];
		else
			m_selects[i].cbCount = 0;
		memcpy( m_selects[i].szString, info.szSelections[i], MAX_BYTE_VOTESELECT );
	}
}

// 투표하기 
void CGuildVote::Cast( BYTE cbSelect )
{
	ASSERT( cbSelect < 4 );
	m_selects[cbSelect].cbCount++;
}

void CGuildVote::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idVote << (BYTE)m_bCompleted;
		ar.WriteString( m_szTitle );
		ar.WriteString( m_szQuestion );

		for( int i=0; i<4; ++i )
		{
			ar.WriteString( m_selects[i].szString );
			ar << m_selects[i].cbCount;
		}	
	}
	else
	{
		BYTE complete;
		ar >> m_idVote >> complete;
		ar.ReadString( m_szTitle );
		ar.ReadString( m_szQuestion );

		for( int i=0; i<4; ++i )
		{
			ar.ReadString( m_selects[i].szString );
			ar >> m_selects[i].cbCount;
		}	

		m_bCompleted = (complete == 1);
	}
}

//________________________________________________________________________________
CGuildTable& CGuildTable::GetInstance()
{
	static CGuildTable table;
	return table;
}

CGuildTable::CGuildTable()
{
	m_nCount = 0;
}

CGuildTable::~CGuildTable()
{
}

// ar에 읽기, 쓰기 - database서버에서 core서버에 socket을 통해서 보내기 위해서 
void CGuildTable::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_nCount;
		for( int i=0; i<m_nCount; i++ )
		{
			ar << m_table[ i ].dwPxpCount;
			ar << m_table[ i ].dwPenya;
			ar << m_table[ i ].nMaxMember;
		}
	}
	else
	{
		ar >> m_nCount;
		for( int i=0; i<m_nCount; i++ )
		{
			ar >> m_table[ i ].dwPxpCount;
			ar >> m_table[ i ].dwPenya;
			ar >> m_table[ i ].nMaxMember;
		}
	}
}

// 스크립트 파일에서 읽기 
BOOL CGuildTable::ReadBlock( CScript & script )
{
	script.GetToken(); // { 
	DWORD dwVal = script.GetNumber();

	for( int i=0; *script.token != '}' ; ++i )
	{
		m_table[ i ].dwPxpCount = dwVal;
		m_table[ i ].dwPenya    = script.GetNumber();
		m_table[ i ].nMaxMember = script.GetNumber();

		dwVal = script.GetNumber();
	}

	ASSERT( i <= MAX_GUILD_LEVEL );
	m_nCount = i;
	return TRUE;
}

// 레벨업에 필요한 PXP공헌횟수  
DWORD CGuildTable::GetPxpCount(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].dwPxpCount;
}

// 레벨업에 필요한 페냐 
DWORD CGuildTable::GetPenya(WORD nLevel) const 
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].dwPenya;
}

// 최대인원수 
WORD CGuildTable::GetMaxMemeber(WORD nLevel) const
{
	ASSERT( 1 <= nLevel && nLevel <= m_nCount );
	return m_table[nLevel - 1].nMaxMember;
}


//________________________________________________________________________________
MemPooler<CGuildMember>*	CGuildMember::sm_pPool	= new MemPooler<CGuildMember>( 512 );
CGuildMember::CGuildMember()
{
	m_idPlayer		= 0;
	m_nPay			= 0;
	m_nGiveGold		= 0;
	m_dwGivePxpCount= 0;
	m_nWin			= 0;
	m_nLose			= 0;
	*m_szAlias		= '\0';
	m_nMemberLv		= 0;
	m_dwSex			= 0;
	m_nJob			= 0;
	m_nLogin		= 0;
	m_nMultiNo		= 100;
	m_nLevel		= 0;
	m_idSelectedVote = 0; 
	m_nSurrender	= 0;
	m_nClass = 0;
}

CGuildMember::~CGuildMember()
{

}

void CGuildMember::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idPlayer << m_nPay << m_nGiveGold << m_dwGivePxpCount << m_nWin << m_nLose << m_nMemberLv;
		ar << m_dwSex << m_nJob << m_nLevel << m_nLogin << m_nMultiNo;

		ar << m_idSelectedVote;
		ar << m_nSurrender;
		ar << m_nClass;
		ar.WriteString( m_szAlias );
	}
	else
	{
		ar >> m_idPlayer >> m_nPay >> m_nGiveGold >> m_dwGivePxpCount >> m_nWin >> m_nLose >> m_nMemberLv;
		ar >> m_dwSex >> m_nJob >> m_nLevel >> m_nLogin >> m_nMultiNo;

		ar >> m_idSelectedVote;
		ar >> m_nSurrender;
		ar >> m_nClass;
		ar.ReadString( m_szAlias );
	}
}

#ifdef __SCHOOL0701
CGuildMember & CGuildMember::operator = ( CGuildMember & source )
{
//	m_nGiveGold	= source.m_nGiveGold;
//	m_dwGivePxpCount	= source.m_dwGivePxpCount;
//	m_dwSex	= source.m_dwSex;
	m_idPlayer	= source.m_idPlayer;
//	m_idSelectedVote	= source.m_idSelectedVote;
//	m_nJob	= source.m_nJob;
//	m_nLevel	= source.m_nLevel;
//	m_nLogin	= source.m_nLogin;
//	m_nLose	= source.m_nLose;
	m_nMemberLv	= source.m_nMemberLv;
//	m_nMultiNo	= source.m_nMultiNo;
//	m_nPay	= source.m_nPay;
//	m_nSurrender	= source.m_nSurrender;
//	m_nWin	= source.m_nWin;
//	lstrcpy( m_szAlias, source.m_szAlias );
	return *this;
}
#endif	// __SCHOOL0701

//________________________________________________________________________________
MemPooler<CGuild>*	CGuild::sm_pPool		= new MemPooler<CGuild>( 128 );

#ifdef __GUILD_CONST0713
int	CGuild::sm_anMaxMemberSize[MAX_GUILD_LEVEL]	=
	{	30, 30, 32, 32, 34, 34, 36, 36, 38, 38, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 80 };
#else	// __GUILD_CONST0713
int	CGuild::sm_anMaxMemberSize[MAX_GUILD_LEVEL]	=
	{	10, 10, 12, 12, 14,	14,	16,	16,	18,	18,	20,	20,	22,	22,	24,	26,	30,	33,	36,	40	};
#endif	// __GUILD_CONST0713

int	CGuild::sm_anMaxMemberLvSize[MAX_GM_LEVEL]	=
	{	GM_MASTER, GM_KINGPIN, GM_CAPTAIN, GM_SUPPORTER, GM_ROOKIE	};

CGuild::CGuild()
{
	m_idGuild				= 0;
	*m_szGuild				= '\0';
	m_idMaster				= 0;
	m_nLevel				= 1;
	m_bActive				= FALSE;
	m_dwLogo				= 0;
	m_dwContributionPxp		= 0;
	*m_szNotice				= '\0';
	
	m_GuildBank.SetItemContainer( ITYPE_ITEM, MAX_GUILDBANK );
	
	ZeroMemory( m_adwPower, sizeof(m_adwPower) );
	ZeroMemory( m_adwPenya, sizeof(m_adwPenya) );
	
	m_nGoldGuild = 0;

	m_bSendPay = FALSE;
	m_idWar		= 0;
	m_nWin	= 0;
	m_nLose	= 0;
	m_nSurrender	= 0;
	m_idEnemyGuild = 0;
	m_nWinPoint = 0;
#ifdef __SCHOOL0719
	m_nDead		= 0;
#endif	// __SCHOOL0719

	ZeroMemory( m_aQuest, sizeof(m_aQuest) );
	m_nQuestSize	= 0;
	m_pQuest	= NULL;	

#ifdef __J0601
#ifdef __CORESERVER
	m_bLog	= FALSE;
#endif	// __CORESERVER
#endif	// __J0601
}

CGuild::~CGuild()
{
	Clear();
}

void CGuild::Clear( void )
{
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i	= m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		SAFE_DELETE( pMember );
	}
	m_mapPMember.clear();

	for (list <CGuildVote*>::iterator it = m_votes.begin() ;it != m_votes.end(); ++it )
		SAFE_DELETE( *it );
	m_votes.clear();
}

BOOL CGuild::AddMember( CGuildMember* pMember )
{
	if( GetMember( pMember->m_idPlayer ) )
		return FALSE;	// already exists
	m_mapPMember.insert( map<u_long, CGuildMember*>::value_type( pMember->m_idPlayer, pMember ) );
	return TRUE;	//
}

BOOL CGuild::RemoveMember( u_long idPlayer )
{
	CGuildMember* pMember	= GetMember( idPlayer );
	if( pMember )
	{
		delete pMember;
		m_mapPMember.erase( idPlayer );
		return TRUE;
	}
	return FALSE;
}

void CGuild::Serialize( CAr & ar, BOOL bDesc )
{
	if( ar.IsStoring() )
	{
		ar << m_idGuild << m_idMaster << m_nLevel;
		ar.WriteString( m_szGuild );
		ar << m_dwLogo;
		ar << m_nGoldGuild;
		ar << m_nWin << m_nLose << m_nSurrender;
		if( !bDesc )
		{
			ar.Write( m_adwPower, sizeof(m_adwPower) );
			ar.Write( m_adwPenya, sizeof(m_adwPenya) );
			ar.WriteString( m_szNotice );	// 공지사항 
			ar << m_dwContributionPxp;		// 공헌된 PXP
			ar << m_nLevel;					// 레벨 
			ar << m_idEnemyGuild;
			ar << (short)GetSize();
			for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
				( i->second )->Serialize( ar );

			ar << (short)m_votes.size();
			for ( list <CGuildVote*>::iterator it = m_votes.begin(); it!=m_votes.end(); ++it )
				(*it)->Serialize( ar );
			ar << m_nQuestSize;
			ar.Write( (const void*)m_aQuest, sizeof(GUILDQUEST) * m_nQuestSize );
		}
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_idGuild >> m_idMaster >> m_nLevel;
		ar.ReadString( m_szGuild );
		ar >> m_dwLogo;
		ar >> m_nGoldGuild;
		ar >> m_nWin >> m_nLose >> m_nSurrender;
		if( !bDesc )
		{
			ar.Read( m_adwPower, sizeof(m_adwPower) );
			ar.Read( m_adwPenya, sizeof(m_adwPenya) );
			ar.ReadString( m_szNotice );	// 공지사항 
			ar >> m_dwContributionPxp;		// 공헌된 PXP
			ar >> m_nLevel;					// 레벨 
			ar >> m_idEnemyGuild;

			short i, nSize	= 0;
			ar >> nSize;
			for( i = 0; i < nSize; i++ )
			{
				CGuildMember* pMember	= new CGuildMember;
				pMember->Serialize( ar );
				m_mapPMember.insert( map<u_long, CGuildMember*>::value_type( pMember->m_idPlayer, pMember ) );
			}

			ar >> nSize;
			for( i = 0; i < nSize; i++ )
			{
				CGuildVote* pVote = new CGuildVote;
				pVote->Serialize( ar );
				m_votes.push_back( pVote );
			}

			ar >> m_nQuestSize;
			ar.Read( (void*)m_aQuest, sizeof(GUILDQUEST) * m_nQuestSize );
		}
	}
}

CGuildMember* CGuild::GetMember( u_long idPlayer )
{
	map<u_long, CGuildMember*>::iterator i	= m_mapPMember.find( idPlayer );
	if( i != m_mapPMember.end() )
		return i->second;
	return NULL;
}

int CGuild::GetMemberLvSize( int nMemberLv )
{
	int cb	= 0;
	for( map<u_long, CGuildMember*>::iterator i	= m_mapPMember.begin();
		i != m_mapPMember.end(); ++i )
	{
		if( i->second->m_nMemberLv == nMemberLv )
			cb++;
	}
	return cb;
}

int CGuild::GetMaxMemberLvSize( int nMemberLv )
{
	if( nMemberLv < 0 || nMemberLv >= MAX_GM_LEVEL )
		return 0;
	if( nMemberLv == GUD_ROOKIE )
		return GetMaxMemberSize();
	return( CGuild::sm_anMaxMemberLvSize[nMemberLv] );
}

int	CGuild::GetMaxMemberSize( void )
{
//	if( m_nLevel < 1 || m_nLevel > MAX_GUILD_LEVEL )
//		return 0;
//	return( CGuild::sm_anMaxMemberSize[m_nLevel-1] );
	return (int)CGuildTable::GetInstance().GetMaxMemeber( m_nLevel );
}

BOOL CGuild::SetLogo( DWORD dwLogo )
{
	if( m_dwLogo )		// 로고는 한번만 setting가능 
		return FALSE;

	m_dwLogo = dwLogo;
	return TRUE;
}

void CGuild::SetContribution( CONTRIBUTION_CHANGED_INFO& info )
{
	m_nLevel			= info.nGuildLevel;
	m_nGoldGuild		= info.dwGuildPenya;
	m_dwContributionPxp	= info.dwGuildPxpCount;

	CGuildMember* pMember = GetMember( info.idPlayer );
	if( pMember == NULL )	// 멤버 탈퇴?
	{
		TRACE("CGuild::SetContribution - Guild[%d] idPlayer[%d] not found \n", m_idGuild, info.idPlayer );
		return;
	}
	pMember->m_dwGivePxpCount += info.dwPxpCount;
	pMember->m_nGiveGold += (int)info.dwPenya;
}

void CGuild::DecrementMemberContribution( u_long idPlayer, DWORD dwPenya, DWORD dwPxpCount )
{
	CGuildMember* pMember = GetMember( idPlayer );
	if( pMember == NULL )	// 멤버 탈퇴?
	{
		TRACE("CGuild::SetContribution - Guild[%d] idPlayer[%d] not found \n", m_idGuild, idPlayer );
		return;
	}
	pMember->m_dwGivePxpCount -= dwPxpCount;
	pMember->m_nGiveGold -= (int)dwPenya;
}

//공헌가능한가?
CONTRIBUTION_RESULT CGuild::CanContribute( DWORD dwPxp, DWORD dwPenya, u_long idPlayer )
{
	if( dwPxp > 0 && m_nLevel >= CGuildTable::GetInstance().GetMaxLevel() )  // pxp만 최대레벨에서 체크 
		return CONTRIBUTION_FAIL_MAXLEVEL;

	if( m_dwContributionPxp + dwPxp < m_dwContributionPxp )  
		return CONTRIBUTION_FAIL_GUILD_OVERFLOW_PXP;

	if( m_nGoldGuild + dwPenya < m_nGoldGuild )  
		return CONTRIBUTION_FAIL_GUILD_OVERFLOW_PENYA;
	
	CGuildMember* pMember = GetMember( idPlayer );
	if( pMember == NULL )
	{
		TRACE("CanContribute - Guild[%d] idPlayer[%d] not found \n", m_idGuild, idPlayer );
		return CONTRIBUTION_FAIL_INVALID_CONDITION;
	}

	if( pMember->m_dwGivePxpCount + dwPxp < pMember->m_dwGivePxpCount )  
		return CONTRIBUTION_FAIL_OVERFLOW_PXP;

	if( pMember->m_nGiveGold + (int)dwPenya < pMember->m_nGiveGold )  
		return CONTRIBUTION_FAIL_OVERFLOW_PENYA;

	return CONTRIBUTION_OK;
}

// 공헌하기 (CORE서버만 사용한다.)
BOOL CGuild::AddContribution( DWORD dwPxp, DWORD dwPenya, u_long idPlayer )
{
	if( CanContribute( dwPxp, dwPenya, idPlayer) != CONTRIBUTION_OK )
		return FALSE;

	CGuildMember* pMember = GetMember( idPlayer );
	pMember->m_dwGivePxpCount += dwPxp;
	pMember->m_nGiveGold += (int)dwPenya;

	m_dwContributionPxp += dwPxp;
	m_nGoldGuild += dwPenya;

	if( m_nLevel >= CGuildTable::GetInstance().GetMaxLevel() )
		return TRUE;
	
	DWORD dwMaxPxp = CGuildTable::GetInstance().GetPxpCount( m_nLevel + 1 );
	DWORD dwMaxPenya = CGuildTable::GetInstance().GetPenya( m_nLevel + 1);
	if( m_dwContributionPxp >= dwMaxPxp && m_nGoldGuild >= dwMaxPenya )
	{
		m_dwContributionPxp -= dwMaxPxp;
		m_nGoldGuild -= dwMaxPenya;
		++m_nLevel;  // LevelUP
	}

	return TRUE;
}

void CGuild::SetNotice( const char* szNotice )
{
	ASSERT( szNotice );
	strncpy(m_szNotice, szNotice, MAX_BYTE_NOTICE);
	m_szNotice[MAX_BYTE_NOTICE-1] = '\0';
}

// 투표삽입 
void CGuild::AddVote( const VOTE_INSERTED_INFO& info, bool bCompleted, BYTE* cbCounts )
{
	if( m_votes.size() >= MAX_VOTE_ENTRY )
	{
		delete m_votes.front();
		m_votes.pop_front();
	}

	CGuildVote* pVote = new CGuildVote;
	pVote->Init( info, bCompleted, cbCounts );
	m_votes.push_back( pVote );
}

// 투표 찾기 
CGuildVote* CGuild::FindVote( u_long idVote )
{
	list <CGuildVote*>::iterator it = m_votes.begin();
	for ( ; it != m_votes.end() ; ++it )
	{
		if ( (*it)->GetID() == idVote )
			return *it;
	}
	return NULL;
}

// idVote투표의 데이타를 변경하기 
bool CGuild::ModifyVote( u_long idVote, BYTE cbOperation, BYTE cbExtra )
{
	CGuildVote* pVote = FindVote( idVote );
	if( pVote == NULL )
		return false;

	switch( cbOperation )
	{
	case REMOVE_VOTE:
		//RemoveVote( idVote );
		{
			list <CGuildVote*>::iterator it = m_votes.begin();
			for ( ; it != m_votes.end() ; ++it )
			{
				if ( (*it)->GetID() == idVote )
				{
					m_votes.erase( it );
					break;
				}
			}
		}
		break;
	case CLOSE_VOTE:
		pVote->SetComplete();
		break;
	case CAST_VOTE:
		pVote->Cast( cbExtra );
		break;
	default:
		ASSERT( 0 );
	}

	return true;
}

CGuildWar* CGuild::GetWar( void )
{
// must lock g_GuildMng
	return g_GuildWarMng.GetWar( m_idWar );
}

#ifdef __SCHOOL0701
CGuild & CGuild::operator = ( CGuild & source )
{
//	memcpy( m_adwPenya, source.m_adwPenya, sizeof(DWORD)*MAX_GM_LEVEL );
//	memcpy( m_adwPower, source.m_adwPower, sizeof(DWORD)*MAX_GM_LEVEL );
//	m_bActive	= source.m_bActive;
//	m_bSendPay	= source.m_bSendPay;
//	m_dwContributionPxp	= source.m_dwContributionPxp;
//	m_dwFlag	= source.m_dwFlag;
//	m_dwLogo	= source.m_dwLogo;
//	m_idEnemyGuild	= source.m_idEnemyGuild;
	m_idGuild	= source.m_idGuild;
	m_idMaster	= source.m_idMaster;
//	m_idWar	= source.m_idWar;
//	m_nGoldGuild	= source.m_nGoldGuild;
//	m_nLevel	= source.m_nLevel;
//	m_nLose	= source.m_nLose;
//	m_nSurrender	= source.m_nSurrender;
//	m_nWin	= source.m_nWin;
//	m_nWinPoint	= source.m_nWinPoint;
	lstrcpy( m_szGuild, source.m_szGuild );
//	lstrcpy( m_szNotice, source.m_szNotice );
	for( map<u_long, CGuildMember*>::iterator i = source.m_mapPMember.begin(); i != source.m_mapPMember.end(); ++ i )
	{
		CGuildMember* pMember	= new CGuildMember;
		*pMember	= *( i->second );
		if( !AddMember( pMember ) )
			{	SAFE_DELETE( pMember );	}
	}
	return *this;
}
#endif	// __SCHOOL0701

//________________________________________________________________________________
CGuildMng::CGuildMng()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CGuildMng::m_AddRemoveLock" )
#endif	// __INFOCRITSECT_DEBUG__
{
	m_id	= 0;
#ifdef __WORLDSERVER
	m_bSendPay = FALSE;
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __WORLDSERVER
}

CGuildMng::~CGuildMng()
{
	Clear();
}

void CGuildMng::Clear( void )
{
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	for( map<u_long, CGuild*>::iterator i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
		delete( i->second );
	m_mapPGuild.clear();
	m_mapPGuild2.clear();

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

BOOL CGuildMng::SetName( CGuild* pGuild, const char* szName )
{
	CGuild* ptmp	= GetGuild( szName );
	if( ptmp )
		return FALSE;
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.erase( pGuild->m_szGuild );	// erase
	lstrcpy( pGuild->m_szGuild, szName );	// set
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.insert( map<string, CGuild*>::value_type( pGuild->m_szGuild, pGuild ) );	// insert
	return TRUE;
}

u_long CGuildMng::AddGuild( CGuild* pGuild )
{
//	locked
	m_id	= ( pGuild->m_idGuild != 0? pGuild->m_idGuild: m_id + 1 );
	if( GetGuild( m_id ) )
		return 0;
	pGuild->m_idGuild	= m_id;
	m_mapPGuild.insert( map<u_long, CGuild*>::value_type( m_id, pGuild ) );
	if( lstrlen( pGuild->m_szGuild ) > 0 )
		m_mapPGuild2.insert( map<string, CGuild*>::value_type( pGuild->m_szGuild, pGuild ) );
	return m_id;
}

BOOL CGuildMng::RemoveGuild( u_long idGuild )
{
//	locked
	CGuild* pGuild	= GetGuild( idGuild );
	if( pGuild )
	{
		m_mapPGuild.erase( pGuild->m_idGuild );
		if( lstrlen( pGuild->m_szGuild ) > 0 )
			m_mapPGuild2.erase( pGuild->m_szGuild );
		delete pGuild;
		return TRUE;
	}
	return FALSE;
}

CGuild* CGuildMng::GetGuild( u_long idGuild )
{
	map<u_long, CGuild*>::iterator i	= m_mapPGuild.find( idGuild );
	if( i != m_mapPGuild.end() )
		return i->second;
	return NULL;
}

CGuild* CGuildMng::GetGuild( const char* szGuild )
{
	map<string, CGuild*>::iterator i	= m_mapPGuild2.find( szGuild );
	if( i != m_mapPGuild2.end() )
		return i->second;
	return NULL;
}

void CGuildMng::Serialize( CAr & ar, BOOL bDesc )
{
	if( ar.IsStoring() )
	{
		ar << m_id;
		ar << GetSize();
		for( map<u_long, CGuild*>::iterator i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
			( i->second )->Serialize( ar, bDesc );
		
		// 길드랭크 정보 로드
		CGuildRank::Instance()->Serialize( ar );
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_id;
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CGuild* pGuild	= new CGuild;
			pGuild->Serialize( ar, bDesc );
			m_mapPGuild.insert( map<u_long, CGuild*>::value_type( pGuild->m_idGuild, pGuild ) );
			if( lstrlen( pGuild->m_szGuild ) > 0 )
				m_mapPGuild2.insert( map<string, CGuild*>::value_type( pGuild->m_szGuild, pGuild ) );
		}

		// 길드랭크 정보  쓰기
		CGuildRank::Instance()->Serialize( ar );
	}
}

#ifdef __CORESERVER
void CGuildMng::AddConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CGuild* pGuild	= GetGuild( pPlayer->m_idGuild );
	if( !pGuild || !pGuild->IsMember( pPlayer->uKey ) )
	{
		// 길드에 추가 되어 있지 않다
		pPlayer->m_idGuild	= 0;
	}
	else
	{
		if( pGuild->GetSize() > MAX_GM_SIZE )
			return;
		// 길드에 추가 되어 있음
		// 내가 들어왔으므로 길드원들에게 로그인 상태와 나의 멀티번호를 주어야 한다
		// 나에게는 길드원들의 로그인 상태와 멀티번호를 주어야 한다.
		int nMaxLogin = 0;
		u_long uLoginPlayerId[MAX_GM_SIZE];
		u_long uLoginGuildMulti[MAX_GM_SIZE];
		CGuildMember* pMember;
		CPlayer* pSendPlayer;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pSendPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
			if( pSendPlayer )
			{
				uLoginPlayerId[nMaxLogin]	= pSendPlayer->uKey;
				uLoginGuildMulti[nMaxLogin]	= pSendPlayer->m_uIdofMulti;
				++nMaxLogin;
				if( pMember->m_idPlayer != pPlayer->uKey )
					g_DPCacheSrvr.SendGuildMemberLogin( pSendPlayer, 1, pPlayer->uKey, pPlayer->m_uIdofMulti );
			}
		}
		if( nMaxLogin > 0 )
			g_DPCacheSrvr.SendGuildMemberGameJoin( pPlayer, nMaxLogin, uLoginPlayerId, uLoginGuildMulti );
	}
}

void CGuildMng::RemoveConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );
	
	CGuild* pGuild	= GetGuild( pPlayer->m_idGuild );
	if( pGuild )
	{
		// 내가 나가므로 길드원들에게 로그아웃 상태
		CGuildMember* pMember;
		CPlayer* pSendPlayer;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			if( pMember->m_idPlayer != pPlayer->uKey )
			{
				pSendPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
				// 클라들에게 통보
				if( pSendPlayer )
					g_DPCacheSrvr.SendGuildMemberLogin( pSendPlayer, 0, pPlayer->uKey, 100 );
			}
		}
		g_dpCoreSrvr.SendGuildMemberLogOut( pGuild->m_idGuild, pPlayer->uKey );
	}
}
#endif	// __CORESERVER

#ifdef __WORLDSERVER
BOOL CGuildMng::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return FALSE;
	return TRUE;
}

void CGuildMng::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT CGuildMng::_Worker( LPVOID pParam )
{
	CGuildMng* pGuildMng	= (CGuildMng*)pParam;
	pGuildMng->Worker();
	return 0;
}

void CGuildMng::Worker( void )
{
	HANDLE hHandle	= m_hCloseWorker;
	while( WaitForSingleObject( hHandle, 30000 ) == WAIT_TIMEOUT )	// 10초	= 1000 * 10 //1000 * 60 * 5
	{
		CTime timeCurr	= CTime::GetCurrentTime();

		int nHour = timeCurr.GetHour();
		int nMin = timeCurr.GetMinute();
		// 시간 검사 시간이 0시 == 24시 인지 확인
		if( m_bSendPay == FALSE && nHour == 21 )//0 )
		{
			prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
			m_AddRemoveLock.Enter( theLineFile );	// lock2
			
			m_bSendPay = TRUE;

			CGuild * pGuild;
			DWORD dwPay;
			for( map<u_long, CGuild*>::iterator i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
			{
				pGuild = i->second;

				// 0시에 한번이도 보냈는지 확인
				if( pGuild->m_bSendPay == FALSE )
				{
					dwPay = 0;		
					CGuildMember* pMember;
					for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
					{
						pMember		= i->second;
						dwPay += pGuild->m_adwPenya[ pMember->m_nMemberLv ];
					}
					
					if( 0 < dwPay && dwPay <= pGuild->m_nGoldGuild )
					{
						pGuild->m_bSendPay = TRUE;
						// DB에 월급을 지급해라라는 명령을 보냄
						pGuild->m_nGoldGuild -= dwPay;					
						g_dpDBClient.SendGuildGetPay( pGuild->GetGuildId(), pGuild->m_nGoldGuild, dwPay );
						g_DPCoreClient.SendGuildGetPay( pGuild->GetGuildId(), pGuild->m_nGoldGuild );
					}
					else
					{
						// 길드 창고에 돈이 모자르므로 월급을 못줌
					}
				}
			}
			m_AddRemoveLock.Leave( theLineFile );	// unlock2
			prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1	
		}
		else
		// 시간이 새벽 1시이면 pGuild->m_bSendPay값을 FALSE로 셑팅, m_bSendPay는 FALSE로 한번이라도 했으며 할필요가 없으므로 아웃
		if( m_bSendPay && nHour == 22 )//1 )
		{
			prj.m_AddRemoveLock.Enter( theLineFile );	// lock1
			m_AddRemoveLock.Enter( theLineFile );	// lock2
			
			CGuild * pGuild;
			for( map<u_long, CGuild*>::iterator i = m_mapPGuild.begin(); i != m_mapPGuild.end(); ++i )
			{
				pGuild = i->second;
				
				// 1시에 FALSE로 셑팅
				pGuild->m_bSendPay = FALSE;
			}

			m_bSendPay = FALSE;
			
			m_AddRemoveLock.Leave( theLineFile );	// unlock2
			prj.m_AddRemoveLock.Leave( theLineFile );	// unlock1	
		}
		//TRACE( "Worker Guild : %dHour/%dMin Pay:%d\n", nHour,  nMin, m_bSendPay );	
	}
}

#endif // __WORLDSERVER

void CGuild::SetQuest( int nQuestId, int nState )
{
	PGUILDQUEST pQuest	= FindQuest( nQuestId );
	if( !pQuest )
	{
		if( m_nQuestSize >= MAX_GUILD_QUEST )
		{
			Error( "OVER MAX_GUILD_QUEST" );
			return;
		}
		for( int i = 0; i < m_nQuestSize; i++ )
		{
			if( m_aQuest[i].nId == -1 )
			{
				pQuest	= m_pQuest	= &m_aQuest[i];
				break;
			}
		}
		if( !pQuest )
			pQuest	= m_pQuest	= &m_aQuest[m_nQuestSize++];
	}
	
	pQuest->nState	= nState;
	pQuest->nId		= nQuestId;
#ifdef __WORLDSERVER
	CUser* pUser;
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddSetGuildQuest( nQuestId, nState );
	}
#endif	// __WORLDSERVER
}

BOOL CGuild::RemoveQuest( int nQuestId )
{
	PGUILDQUEST pQuest	= FindQuest( nQuestId );
	if( !pQuest )
		return FALSE;
	pQuest->nId	= -1;
	return TRUE;
#ifdef __WORLDSERVER
	CUser* pUser;
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
			pUser->AddRemoveGuildQuest( nQuestId );
	}
#endif	// __WORLDSERVER
}

PGUILDQUEST CGuild::FindQuest( int nQuestId )
{
	if( m_pQuest && m_pQuest->nId == nQuestId )
		return m_pQuest;
	for( int i = 0; i < m_nQuestSize; i++ )
	{
		if( m_aQuest[i].nId == nQuestId )
		{
			m_pQuest	= &m_aQuest[i];
			return m_pQuest;
		}
	}
	return NULL;
}

#ifdef __WORLDSERVER
void CGuild::ReplaceLodestar( const CRect &rect )
{
// locked
	CWorld* pWorld;
	CUser* pUser;
	CGuildMember* pMember;
	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;
		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			CWorld* pWorld	= pUser->GetWorld();
			if( pWorld )
			{
				POINT point	= { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
				if( rect.PtInRect( point ) )
				{
					PRegionElem pRgnElem	= NULL;
					if( pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0 )
						pRgnElem	= g_WorldMng.GetRevivalPos( pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival );
					if( !pRgnElem )	// Find near revival pos
						pRgnElem	= g_WorldMng.GetNearRevivalPos( pWorld->GetID(), pUser->GetPos() );
					if( pRgnElem )
					{
#ifdef	__MSVR
						pUser->Replace( g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#else	// __MSVR
						pUser->Replace( pRgnElem->m_dwWorldId, pRgnElem->m_vPos );
#endif	// __MSVR
					}
				}
			}
		}
	}
}

void CGuild::Replace( DWORD dwWorldId, D3DXVECTOR3 & vPos, BOOL bMasterAround )
{
//	locked
	CUser* pUser;
	CGuildMember* pMember;
	CUser* pMaster	= NULL;
	if( bMasterAround )
	{
		pMaster	= (CUser*)prj.GetUserByID( m_idMaster );
		if( IsInvalidObj( (CObj*)pMaster ) )
			return;
	}

	for( map<u_long, CGuildMember*>::iterator i = m_mapPMember.begin(); i != m_mapPMember.end(); ++i )
	{
		pMember	= i->second;

		pUser	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
		if( IsValidObj( pUser ) )
		{
			if( pMaster && !pMaster->IsNearPC( pUser->GetId() ) )
				continue;

			if( GetQuest( QUEST_WARMON_LV1 ) != NULL ) // 클락워크 퀘스트면 비행 해제
				pUser->UnequipRide();

#ifdef __MSVR
			pUser->Replace( g_uIdofMulti, dwWorldId, vPos );
#else	// __MSVR
			pUser->Replace( dwWorldId, vPos );
#endif	// __MSVR
		}
	}
}
#endif	// __WORLDSERVER

CGuildRank* CGuildRank::Instance()
{
	static CGuildRank	sGuildRank;
	return &sGuildRank;
}


CGuildMng	g_GuildMng;

