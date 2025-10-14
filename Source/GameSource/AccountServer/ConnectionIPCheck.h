
#pragma once

class CConnectionIPCheck
{
public:
	CConnectionIPCheck();
	~CConnectionIPCheck();

	static CConnectionIPCheck*		GetInstance();

	bool	Load( const TCHAR* pFileName );

	bool	IsPermitedInternalConnectionIP( const std::string & address );

private:
	typedef std::vector< std::string >	PermitedIP;

	CMclCritSec		m_lock;
	PermitedIP		m_permitedIP;
};
