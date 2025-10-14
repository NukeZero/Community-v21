// AccountMgr.h: interface for the CAccountMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNTMGR_H__57A5D903_C611_4722_99DD_AAA9FB5A8D57__INCLUDED_)
#define AFX_ACCOUNTMGR_H__57A5D903_C611_4722_99DD_AAA9FB5A8D57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FLDBOverlappedPlus.h"

enum ACCOUNT_CHECK {
	CHECK_OK,				// 틀린 적이 없거나, 처음 
	CHECK_1TIMES_ERROR,		// 1회 틀림
	CHECK_3TIMES_ERROR,		// 3회 틀림 
};

struct ACCOUNT_CACHE 
{
	TCHAR	szAccount[MAX_ACCOUNT];
	DWORD	m_dwIP;
	int		m_nError;
	time_t	m_tmError;

	ACCOUNT_CACHE()
	{
		szAccount[0]	= '\0';
		m_dwIP			= 0;
		m_nError		= 0;
		m_tmError		= 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// 특정 횟수 이상 비밀번호 오류를 관리하는 클래스
// 클라이언트의 공인 IP만 기준으로 하기 때문에 미묘한 오류가 있다.
// 만일 공유기 내에 존재하는 여러개의 클라이언트들이 비밀번호 오류를 낸다면,
// 같은 공인 IP이므로 오류 횟수 값을 공유한다.
// (물론 Account명으로 DB스레드를 분리하므로 같은 스레드에서 실행되어야 한다는 조건이 있다.)
// 오류 검출이 각 Account별인지 각 머신별인지 정확히 명시되어야 정확한 로직을 구성할 수 있을 것 같다.
class CAccountMgr
{
public:

	std::list< ACCOUNT_CACHE* >	m_cache;		// LRU로 유지되는 캐쉬 정보 

public:

	CAccountMgr();
	~CAccountMgr();

	ACCOUNT_CHECK	Check( const AUTH_INFO & kAuthInfo );

	void			IncErrorCount();
	void			ResetErrorCount();

};

#endif // !defined(AFX_ACCOUNTMGR_H__57A5D903_C611_4722_99DD_AAA9FB5A8D57__INCLUDED_)
