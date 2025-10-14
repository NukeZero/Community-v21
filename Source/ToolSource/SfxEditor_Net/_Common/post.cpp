#include "stdafx.h"
#include "post.h"

u_long	CMail::s_nMail	= 0;

CMail::CMail()
{
	m_nMail	= 0;
	m_idSender	= 0;
	m_pItemElem	= NULL;
	*m_szTitle	= '\0';
	*m_szText	= '\0';
}

CMail::CMail( u_long idSender, CItemElem* pItemElem, int nGold, char* szTitle, char* szText )
{
	m_nMail	= 0;
	m_idSender	= idSender;
	m_pItemElem	= pItemElem;
	lstrcpy( m_szTitle, szTitle );
	lstrcpy( m_szText, szText );
}

CMail::~CMail()
{
	Clear();
}

void CMail::Clear( void )
{
	SAFE_DELETE( m_pItemElem );
}

void CMail::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_nMail << m_idSender;
		if( m_pItemElem )
		{
			ar << (BYTE)1;
			m_pItemElem->Serialize( ar );
		}
		else
		{
			ar << (BYTE)0;
		}
		ar << m_nGold;
		ar.WriteString( m_szTitle );
		ar.WriteString( m_szText );
	}
	else
	{
		ar >> m_nMail >> m_idSender;
		BYTE byItemElem;
		ar >> byItemElem;
		if( byItemElem )
		{
			m_pItemElem	= new CItemElem;
			m_pItemElem->Serialize( ar );
		}
		ar >> m_nGold;
		ar.ReadString( m_szTitle );
		ar.ReadString( m_szText );
	}
}

CMailBox::CMailBox()
{
	m_idReceiver	= 0;
}

CMailBox::CMailBox( u_long idReceiver )
{
	m_idReceiver	= idReceiver;
}

CMailBox::~CMailBox()
{
	Clear();
}

CMailBox*	CMailBox::GetInstance( void )
{
	static CMailBox	sMailBox;
	return &sMailBox;
}

void CMailBox::Clear( void )
{
	for( vector<CMail*>::iterator i = begin(); i != end(); ++i )
		SAFE_DELETE( *i );
	clear();
}

u_long CMailBox::AddMail( CMail* pMail )
{
	if( 0 == pMail->m_nMail )
		pMail->m_nMail	= ++CMail::s_nMail;
	push_back( pMail );
	return pMail->m_nMail;
}

void CMailBox::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_idReceiver;
		ar << (int)size();
		for( vector<CMail*>::iterator i = begin(); i != end(); ++i )
		{
			CMail* pMail	= *i;
			pMail->Serialize( ar );
		}
	}
	else
	{
		ar >> m_idReceiver;
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CMail* pMail	= new CMail;
			pMail->Serialize( ar );
			AddMail( pMail );
		}
	}
}

vector<CMail*>::iterator CMailBox::Find( u_long nMail )
{
	vector<CMail*>::iterator i = begin();
	for( ; i != end(); ++i )
	{
		CMail* pMail	= *i;
		if( pMail->m_nMail == nMail )
			break;
	}
	return i;
}

CMail* CMailBox::GetMail( u_long nMail )
{
	vector<CMail*>::iterator i = Find( nMail );
	if( i != end() )
		return *i;
	return NULL;
}

BOOL CMailBox::RemoveMail( u_long nMail )
{
	vector<CMail*>::iterator i = Find( nMail );
	if( i != end() )
	{
		SAFE_DELETE( *i );
		erase( i );
		return TRUE;
	}
	return FALSE;
}

BOOL CMailBox::RemoveMailItem( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail && pMail->m_pItemElem )
	{
		SAFE_DELETE( pMail->m_pItemElem );
		return TRUE;
	}
	return FALSE;
}

BOOL CMailBox::RemoveMailGold( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail && pMail->m_nGold > 0 )
	{
		pMail->m_nGold	= 0;
		return TRUE;
	}
	return FALSE;
}

CPost::CPost()
{
}

CPost::~CPost()
{
	Clear();
}

void CPost::Clear( void )
{
	for( map<u_long, CMailBox*>::iterator i = m_mapMailBox.begin(); i != m_mapMailBox.end(); ++i )
	{
		CMailBox* pMailBox	= i->second;
		SAFE_DELETE( pMailBox );
	}
	m_mapMailBox.clear();
}

u_long CPost::AddMail( u_long idReceiver, u_long idSender, CItemElem* pItemElem, int nGold, char* szTitle, char* szText )
{
	CMailBox* pMailBox	= GetMailBox( idReceiver );
	if( !pMailBox )
	{
		pMailBox	= new CMailBox( idReceiver );
		AddMailBox( pMailBox );
	}
	CMail* pMail	= new CMail( idSender, pItemElem, nGold, szTitle, szText );
	return pMailBox->AddMail( pMail );
}

u_long CPost::AddMail( u_long idReceiver, CMail* pMail )
{
	CMailBox* pMailBox	= GetMailBox( idReceiver );
	if( !pMailBox )
	{
		pMailBox	= new CMailBox( idReceiver );
		AddMailBox( pMailBox );
	}
	return pMailBox->AddMail( pMail );
}

CMailBox* CPost::GetMailBox( u_long idReceiver )
{
	map<u_long, CMailBox*>::iterator i = m_mapMailBox.find( idReceiver );
	if( i != m_mapMailBox.end() )
		return i->second;
	return NULL;
}

BOOL CPost::AddMailBox( CMailBox* pMailBox )
{
	return m_mapMailBox.insert( map<u_long, CMailBox*>::value_type( pMailBox->m_idReceiver, pMailBox ) ).second;
}

void CPost::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_mapMailBox.size();
		for( map<u_long, CMailBox*>::iterator i = m_mapMailBox.begin(); i != m_mapMailBox.end(); ++i )
		{
			CMailBox* pMailBox	= i->second;
			pMailBox->Serialize( ar );
		}
	}
	else
	{
		int nSize;
		ar >> nSize;
		for( int i = 0; i < nSize; i++ )
		{
			CMailBox* pMailBox	= new CMailBox;
			pMailBox->Serialize( ar );
			AddMailBox( pMailBox );
		}
	}
}

CPost*	CPost::GetInstance( void )
{
	static CPost	sPost;
	return &sPost;
}