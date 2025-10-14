
#ifndef	__FLLOGFACADE_H__
#define	__FLLOGFACADE_H__


#include "FLLogCommon.h"
#include "FLLogContext.h"
#include "FLLogLevelFilter.h"
#include "FLLogModule.h"
#include "FLLogPubEtc.h"
#include "FLLogPoster.h"
#include "FLLogPubFactory.h"
#include "FLLogGenericPubFactory.h"

#include <list>


class	FLLogFacade
{
public:

	static	FLLogFacade*	GetInstance();

private:

	FLLogFacade();
	~FLLogFacade();

	class	FLLog
	{
	public:

		FLLog();
		~FLLog();

		bool	Create( FLLogContext* pContext, const TCHAR* szName, FLLogPubFactory* pFactory );
		void	Destroy();

		void	AddPublisher( FLLogPublisher* pPublisher );
		void	DeletePublisher( FLLogPublisher* pPublisher );

		void	Push( FLLogEvent* pEvent );

		bool	IsEqual( const TCHAR* szName )	const;
		int		SetLevel( int nLevel );

		const TCHAR*	GetName()	const;

	private:

		TCHAR				m_szName[FLLOG_MAX_LOG_NAME];

		FLLogModule			m_cModule;
		FLLogLevelFilter	m_cLevelFilter;
		FLLogPubComposite	m_cPubComposite;

		FLLogPubFactory*	m_pFactory;
		FLLogPublisher*		m_pUserPublisher;

	};

public:

	bool	AddNetworkLog( const TCHAR* szName );
	bool	Add( const TCHAR* szName );

	void	AddPublisher( const TCHAR* szName, FLLogPublisher* pPublisher );
	void	DeletePublisher( const TCHAR* szName, FLLogPublisher* pPublisher );

	int		ChangeLevel( const TCHAR* szName, int nLevel );

	void	Push( const TCHAR* szName, int nLevel, const TCHAR* szFunction, const TCHAR* szFile, int nLine, const TCHAR* szFormat, ... );
	bool	Push( const TCHAR* szName, FLLogEvent* pEvent );

	void	Flush();

private:

	void	Start();
	void	Stop();

	FLLog*	SearchAndCreate( const TCHAR* szName );
	FLLog*	Search( const TCHAR* szName );

private:

	typedef	std::list< FLLog* >	LOG_LIST;
	typedef	LOG_LIST::iterator	LOG_ITER;

	FLLogContext			m_cContext;
	FLLogGenericPubFactory	m_cPubFactory;

	CMclCritSec				m_csLogs;
	LOG_LIST				m_listLogs;

	FLLog*					m_pLastHit;

};


#define	g_cLogFacade	FLLogFacade::GetInstance()


#ifdef	__FLLOG_NOT_LOGGING

#define	FLLOG_ADD( szName )								__noop
#define	FLLOG_ADD_NETWORK( szName )						__noop
#define	FLLOG_ADD_PUBLISHER( szName, pPublisher )		__noop
#define	FLLOG_DEL_PUBLISHER( szName, pPublisher )		__noop
#define	FLLOG_LEVEL( szName, nLevel )					__noop
#define	FLLOG_FLUSH()									__noop
#define	FLLOG_IMPL( nLevel, szFunction, szFile, nLine )	__noop
#define	FLLOG( nLevel )									__noop

#else

#define	FLLOG_ADD( szName )								g_cLogFacade->Add( szName )
#define	FLLOG_ADD_NETWORK( szName )						g_cLogFacade->AddNetworkLog( szName )
#define	FLLOG_ADD_PUBLISHER( szName, pPublisher )		g_cLogFacade->AddPublisher( szName, pPublisher )
#define	FLLOG_DEL_PUBLISHER( szName, pPublisher )		g_cLogFacade->DeletePublisher( szName, pPublisher )
#define	FLLOG_LEVEL( szName, nLevel )					g_cLogFacade->ChangeLevel( szName, nLevel )
#define	FLLOG_FLUSH()									g_cLogFacade->Flush()
#define	FLLOG_IMPL( nLevel, szFunction, szFile, nLine )	FLLogPoster( nLevel, szFunction, szFile, nLine )
#define	FLLOG( nLevel )									FLLOG_IMPL( nLevel, _T( __FUNCTION__ ), _T( __FILE__ ), __LINE__ )

#endif





#ifdef	__FLLOG_EXCLUDE_DEBUG_TRACE
#define	FLDEBUG_LOG		NOOP_FUNC//__noop
#define	FLTRACE_LOG		NOOP_FUNC//__noop
#else
#define	FLDEBUG_LOG		FLLOG( FLLOG_DEBUG )
#define	FLTRACE_LOG		FLLOG( FLLOG_TRACE )
#endif

#define	FLINFO_LOG		FLLOG( FLLOG_INFO )
#define	FLWARNING_LOG	FLLOG( FLLOG_WARNING )
#define	FLERROR_LOG		FLLOG( FLLOG_ERROR )
#define	FLFAULT_LOG		FLLOG( FLLOG_FAULT )



//////////////////////////////////////////////////////////////////////////
#if _MSC_VER >= 1400

	#define FLERROR_ASSERT_LOG( obj, ... ) \
		if( NULL == obj ) \
		{ \
			FLERROR_LOG( PROGRAM_NAME, ##__VA_ARGS__ ); \
			FLASSERT( obj ); \
			return; \
		} \

	#define FLERROR_ASSERT_LOG_RETURN_VOID( obj, ... ) \
		if( NULL == obj ) \
		{ \
			FLERROR_LOG( PROGRAM_NAME, ##__VA_ARGS__ ); \
			FLASSERT( obj ); \
			return; \
		} \

	#define FLERROR_ASSERT_LOG_RETURN( obj, return_value, ... ) \
		if( NULL == obj ) \
		{ \
			FLERROR_LOG( PROGRAM_NAME, ##__VA_ARGS__ ); \
			FLASSERT( obj ); \
			return return_value; \
		} \

	#define FLERROR_ASSERT_LOG_DO( obj, do_type, ... )\
		if( NULL == obj ) \
		{ \
			FLERROR_LOG( PROGRAM_NAME, ##__VA_ARGS__ ); \
			FLASSERT( obj ); \
			do_type; \
		} 
#endif 
//////////////////////////////////////////////////////////////////////////


#endif