#pragma once

#include "Query.h"

class FLQueryCommand;


class FLDBChild;

class FLDataBase
{
public:
	FLDataBase();
	virtual ~FLDataBase();
#ifdef __DbStuff	
	bool Start(size_t udbThreadCount, CQuery::SqlDatabase dbType);
#else
	BOOL					Start( const size_t uDBThreadCount, const TCHAR * szDSNName, const TCHAR * szID, const TCHAR * szPW );
#endif

	void					Stop();

	void					PostQueryCommand( const unsigned long uKey, FLQueryCommand * pCommand );
	CQuery *				GetQuery( const unsigned long uKey );			//간혹 blocking 모드 쿼리 실행할때 필요. 서버 Init할때 load하는 쿼리들.. 위험하니 사용 자제

private:
	FLDBChild *				m_parDB;
	size_t					m_uDBThreadCount;
};



class FLDBChild
{
	friend FLDataBase;

	enum					{ 	MAX_CONNECT_STRING = 256,	};
	enum					E_COMMAND
	{
		DB_EXIT		= 0,
		DB_QUERY,
	};

public:
	FLDBChild();
	virtual ~FLDBChild();

	//재시도 루틴
#ifdef __DbStuff
	bool Start(CQuery::SqlDatabase dbType);
#else
	BOOL					Start( const TCHAR * szDSNName, const TCHAR * szID, const TCHAR * szPW );
#endif
	void					Stop();

	void					PostQueryCommand( FLQueryCommand * pCommand );



protected:
	BOOL					connect( const TCHAR * szDSNName, const TCHAR * szID, const TCHAR * szPW );
	static unsigned	WINAPI	ThreadProc( void * pVoid );
	
private:
	HANDLE					m_hThread;
	HANDLE					m_hIOCP;
    
	TCHAR					m_szDSNName[ MAX_CONNECT_STRING ];
	TCHAR					m_szID[ MAX_CONNECT_STRING ];
	TCHAR					m_szPW[ MAX_CONNECT_STRING ];
	CQuery					m_Query;
};



class FLCharacterDB : public FLDataBase
{
public:
	static FLCharacterDB &		GetInstance() {			static FLCharacterDB inst;	return inst;	}
};
inline static FLCharacterDB &	CHARACTER_DB(){		return FLCharacterDB::GetInstance();		}


class FLLogDB : public FLDataBase
{
public:	
	static FLLogDB &			GetInstance() {			static FLLogDB inst;	return inst;		}
};
inline static FLLogDB &			LOG_DB(){			return FLLogDB::GetInstance();				}


