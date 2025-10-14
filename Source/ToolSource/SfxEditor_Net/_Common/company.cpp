#include "stdafx.h"

#ifdef __J1216

#include "company.h"
#ifdef __CORESERVER
#include "player.h"
#endif	// __CORESERVER
/*________________________________________________________________________________*/
MemPooler<CCompanyMember>*	CCompanyMember::m_pPool		= new MemPooler<CCompanyMember>( 512 );
CCompanyMember::CCompanyMember()
{
	m_idPlayer	= 0;
}

CCompanyMember::CCompanyMember( u_long idPlayer )
{
	m_idPlayer	= idPlayer;
}

CCompanyMember::~CCompanyMember()
{
}

void CCompanyMember::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idPlayer;
	}
	else
	{
		ar >> m_idPlayer;
	}
}

/*________________________________________________________________________________*/
MemPooler<CCompany>*	CCompany::m_pPool		= new MemPooler<CCompany>( 128 );
CCompany::CCompany()
{
	m_idCompany	= 0;
	m_idLeader	= 0;
	*m_szCompany	= '\0';
#ifdef __J0103
	m_nLevel	= 0;
#endif	// __J0103
}

CCompany::CCompany( u_long idCompany, const char* szCompany, u_long idLeader )
{
	m_idCompany	= idCompany;
	strcpy( m_szCompany, szCompany );
	m_idLeader	= idLeader;
#ifdef __J0103
	m_nLevel	= 0;
#endif	// __J0103
}

CCompany::~CCompany()
{
	Clear();
}

void CCompany::Clear( void )
{
	CCompanyMember*	pMember;
	for( map<u_long, CCompanyMember*>::iterator i = m_mapMemberPtr.begin(); i != m_mapMemberPtr.end(); ++i )
	{
		pMember	= i->second;
		SAFE_DELETE( pMember );
	}
	m_mapMemberPtr.clear();
}

void CCompany::AddMember( u_long idPlayer )
{
	CCompanyMember* pMember		= new CCompanyMember( idPlayer );
	m_mapMemberPtr.insert( map<u_long, CCompanyMember*>::value_type( idPlayer, pMember ) );
}

BOOL CCompany::RemoveMember( u_long idPlayer )
{
	CCompanyMember* pMember	= GetMember( idPlayer );
	if( pMember )
	{
		delete pMember;
		m_mapMemberPtr.erase( idPlayer );
		return TRUE;
	}
	return FALSE;
}

CCompanyMember* CCompany::GetMember( u_long idPlayer )
{
	map<u_long, CCompanyMember*>::iterator i	= m_mapMemberPtr.find( idPlayer );
	if( i != m_mapMemberPtr.end() )
		return i->second;
	return NULL;
}

void CCompany::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idCompany;
		ar.WriteString( m_szCompany );
		ar << m_idLeader;
#ifdef __J0103
		ar << (BYTE)m_nLevel;
		ar << (short)GetSize();
#else	// __J0103
		ar << GetSize();
#endif	// __J0103
		for( map<u_long, CCompanyMember*>::iterator i = m_mapMemberPtr.begin(); i != m_mapMemberPtr.end(); ++ i )
		{
			ASSERT( i->second );
			( i->second )->Serialize( ar );
		}
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_idCompany;
		ar.ReadString( m_szCompany );
		ar >> m_idLeader;
#ifdef __J0103
		m_nLevel	= 0;
		ar >> (BYTE &)m_nLevel;
		int nSize	= 0;
		ar >> (short &)nSize;
#else	// __J0103
		int nSize;
		ar >> nSize;
#endif	// __J0103
		for( int i = 0; i < nSize; i++ )
		{
			CCompanyMember* pMember	= new CCompanyMember;
			pMember->Serialize( ar );
			m_mapMemberPtr.insert( map<u_long, CCompanyMember*>::value_type( pMember->m_idPlayer, pMember ) );
		}
	}
}

void CCompany::SerializeEx( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idCompany;
		ar.WriteString( m_szCompany );
		ar << m_idLeader;
#ifdef __J0103
		ar << (BYTE)m_nLevel;
#endif	// __J0103
	}
	else
	{
#ifdef __CLIENT
		Clear();
#endif	// __CLIENT
		ar >> m_idCompany;
		ar.ReadString( m_szCompany );
		ar >> m_idLeader;
#ifdef __J0103
		m_nLevel	= 0;
		ar >> (BYTE &)m_nLevel;
#endif	// __J0103
	}
}

/*________________________________________________________________________________*/
CCompanyMng::CCompanyMng()
#ifdef __INFOCRITSECT_DEBUG__
: m_AddRemoveLock( "CCompanyMng::m_AddRemoveLock" )
#endif	// __INFOCRITSECT_DEBUG__
{
	m_id	= 0;
#ifdef __J0103
	m_nMaxExp	= 0;
	ZeroMemory( m_aExp, sizeof(m_aExp) );
#endif	// __J0103
}

CCompanyMng::~CCompanyMng()
{
	Clear();
}

void CCompanyMng::Clear( void )
{
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	for( map<u_long, CCompany*>::iterator i	= m_mapCompanyPtr.begin(); i != m_mapCompanyPtr.end(); ++i )
		delete( i->second );
	m_mapCompanyPtr.clear();

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

u_long CCompanyMng::CreateCompany( u_long idPlayer, const char* szCompany, u_long idCompany )
{
//	locked
//	CMclAutoLock	Lock( m_AddRemoveLock );

	m_id	= ( idCompany != 0? idCompany: m_id + 1 );
	if( NULL == GetCompany( m_id ) )
	{
		CCompany* pCompany	= new CCompany( m_id, szCompany, idPlayer );
#ifdef __CLIENT
		if( g_pPlayer && g_pPlayer->m_idPlayer == idPlayer )
		{
			pCompany->AddMember( idPlayer );
		}
#else	// __CLIENT
		pCompany->AddMember( idPlayer );
#endif	// __CLIENT
		m_mapCompanyPtr.insert( map<u_long, CCompany*>::value_type( m_id, pCompany ) );
		m_mapCompanyPtr2.insert( map<string, CCompany*>::value_type( szCompany, pCompany ) );
		return m_id;
	}
	return 0;
}

BOOL CCompanyMng::DestroyCompany( u_long idCompany )
{
//	locked
//	CMclAutoLock	Lock( m_AddRemoveLock );

	CCompany* pCompany	= GetCompany( idCompany );
	if( pCompany )
	{
		m_mapCompanyPtr2.erase( pCompany->m_szCompany );
		m_mapCompanyPtr.erase( pCompany->m_idCompany );
		delete pCompany;
		return TRUE;
	}
	return FALSE;
}

CCompany* CCompanyMng::GetCompany( u_long idCompany )
{
	map<u_long, CCompany*>::iterator i	= m_mapCompanyPtr.find( idCompany );
	if( i != m_mapCompanyPtr.end() )
		return i->second;
	return NULL;
}

CCompany* CCompanyMng::GetCompany( const char* szCompany )
{
	map<string, CCompany*>::iterator i	= m_mapCompanyPtr2.find( szCompany );
	if( i != m_mapCompanyPtr2.end() )
		return i->second;
	return NULL;
}

void CCompanyMng::Serialize( CAr & ar )
{
//	CMclAutoLock	Lock( m_AddRemoveLock );
	m_AddRemoveLock.Enter( theLineFile );	// lock1

	if( ar.IsStoring() )
	{
#ifdef __J0103
		ar << (BYTE)m_nMaxExp;
		ar.Write( (const void*)m_aExp, sizeof(ExpCompany)*m_nMaxExp );
#endif	// __J0103
		ar << m_id;
		ar << GetSize();
		for( map<u_long, CCompany*>::iterator i = m_mapCompanyPtr.begin(); i != m_mapCompanyPtr.end(); ++ i )
		{
			ASSERT( i->second );
			( i->second )->Serialize( ar );
		}
	}
	else
	{
#ifdef __J0103
		m_nMaxExp	= 0;
		ar >> (BYTE &)m_nMaxExp;
		ar.Read( (void*)m_aExp, sizeof(ExpCompany)*m_nMaxExp );
#endif	// __J0103
		int nSize;
		ar >> m_id;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CCompany* pCompany	= new CCompany;
			pCompany->Serialize( ar );
			m_mapCompanyPtr.insert( map<u_long, CCompany*>::value_type( pCompany->m_idCompany, pCompany ) );
			m_mapCompanyPtr2.insert( map<string, CCompany*>::value_type( pCompany->m_szCompany, pCompany ) );
		}
	}

	m_AddRemoveLock.Leave( theLineFile );	// unlock1
}

void CCompanyMng::SerializeEx( CAr & ar )
{
//	CMclAutoLock	Lock( m_AddRemoveLock );
	if( ar.IsStoring() )
	{
		ar << GetSize();
		for( map<u_long, CCompany*>::iterator i = m_mapCompanyPtr.begin(); i != m_mapCompanyPtr.end(); ++ i )
		{
			ASSERT( i->second );
			( i->second )->SerializeEx( ar );
		}
	}
	else
	{
		Clear();

		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CCompany* pCompany	= new CCompany;
			pCompany->SerializeEx( ar );
			m_mapCompanyPtr.insert( map<u_long, CCompany*>::value_type( pCompany->m_idCompany, pCompany ) );
			m_mapCompanyPtr2.insert( map<string, CCompany*>::value_type( pCompany->m_szCompany, pCompany ) );
		}
	}
}

#ifdef __CORESERVER
#ifdef __J1216
void CCompanyMng::AddConnection( CPlayer* pPlayer )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CCompany* pCompany	= GetCompany( pPlayer->m_idCompany );
	if( !pCompany || !pCompany->IsMember( pPlayer->uKey ) )
	{
		pPlayer->m_idCompany	= 0;
	}
}

void CCompanyMng::RemoveConnection( CPlayer* pPlayer )
{
}
#endif	// __J1216
#endif	// __CORESERVER

#ifdef __J0103
#ifdef __DBSERVER
void CCompanyMng::AddExpTable( int nExp, int nPenya, int cbMax )
{
	m_aExp[m_nMaxExp].nExp		= nExp;
	m_aExp[m_nMaxExp].nPenya	= nPenya;
	m_aExp[m_nMaxExp++].cbMax	= cbMax;
}
#endif	// __DBSERVER
#endif	// __J0103

CCompanyMng		g_CompanyMng;

#endif	// __J1216