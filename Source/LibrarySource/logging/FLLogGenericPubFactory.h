
#ifndef	__FLLOGGENERICPUBFACTORY_H__
#define	__FLLOGGENERICPUBFACTORY_H__


#include "FLLogCommon.h"
#include "FLLogPubFactory.h"
#include "FLLogFormat.h"
#include "FLLogPubFile.h"
#include "FLLogPubEtc.h"


class	FLLogPubGeneric : public FLLogPublisher
{
public:

	FLLogPubGeneric();
	~FLLogPubGeneric();

	bool	Create( const TCHAR* szName, const TCHAR* szFolder, const TCHAR* szFormat, size_t nMaxLogFileSize );
	void	Destroy();

private:

	void	DoPublish( const FLLogEvent* pEvent );

private:

	FLLogDateTimeFileName	m_cLogFileName;
	FLLogFormat*			m_pFormater;
	FLLogPubFile			m_cPubFile;
#ifdef	_DEBUG
	FLLogPubDebugString		m_cPubDebugString;
#endif


};

class	FLLogGenericPubFactory : public FLLogPubFactory
{
public:

	enum
	{
		MAX_LOGFILE_SIZE	= (10*10*1024*1024),	// 100M
	};

public:

	FLLogGenericPubFactory();
	~FLLogGenericPubFactory();

	// szFormat는 미리 정의된 로그 출력 포맷을 지정합니다.
	// 지원 포맷 "CSV", "Generic". 대소문자를 구분하지 않습니다.
	// 기본 포맷은 "Generic" 입니다.
	void	SetFormater( const TCHAR* szFormat = _T( "Generic" ) );
	void	SetFolder( const TCHAR* szFolder = _T( "./Log" ) );
	void	SetMaxLogFileSize( size_t nMaxLogFileSize = MAX_LOGFILE_SIZE );

private:

	FLLogPublisher*	DoCreate( const TCHAR* szName );
	void	DoDestroy( FLLogPublisher* pPublisher );

private:

	TCHAR	m_szFormat[32];
	TCHAR	m_szFolder[MAX_PATH];
	size_t	m_nMaxLogFileSize;

};




#endif