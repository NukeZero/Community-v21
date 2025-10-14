
#include "StdAfx.h"
#include "ConnectionIPCheck.h"


CConnectionIPCheck::CConnectionIPCheck()
{
}

CConnectionIPCheck::~CConnectionIPCheck()
{
}

CConnectionIPCheck*	CConnectionIPCheck::GetInstance()
{
	static CConnectionIPCheck	kConnectionIPCheck;

	return & kConnectionIPCheck;
}

bool	CConnectionIPCheck::Load( const TCHAR* pFileName )
{
	CScanner scanner;
	if( !scanner.Load( pFileName ) ) {
		return false;
	}

	scanner.GetToken();

	m_lock.Enter();

	m_permitedIP.clear();

	while( scanner.tok != FINISHED ) {
		m_permitedIP.push_back( std::string( scanner.Token ) );
		scanner.GetToken();
	}

	m_lock.Leave();

	return true;
}

bool	CConnectionIPCheck::IsPermitedInternalConnectionIP( const std::string & address )
{
	m_lock.Enter();

	bool permited	= false;

	for( PermitedIP::const_iterator pos = m_permitedIP.begin(); pos != m_permitedIP.end(); ++pos ) {
		if( address.find( (*pos) ) != std::string::npos ) {
			permited	= true;
			break;
		}
	}

	m_lock.Leave();

	return permited;
}
