
#ifndef	__FLLOGPUBFILE_H__
#define	__FLLOGPUBFILE_H__


#include "FLLogCommon.h"
#include "FLLogPublisher.h"
#include "FLLogFormat.h"
#include "FLLogFileName.h"

#include "../common/FLTextFile.h"


class	FLLogPubFile : public FLLogPublisher
{
public:

	FLLogPubFile();
	~FLLogPubFile();

	bool	Create( FLLogFileName* pLogFileName, FLLogFormat* pFormater, size_t nMaxSize );
	void	Destroy();

private:

	void	DoPublish( const FLLogEvent* pEvent );
	void	DoDoneTick();

	bool	NeedNewFile()	const;
	bool	CreateNewFile();
	void	WriteLogEvent( const FLLogEvent* pEvent );

private:

	FLLogFileName*	m_pLogFileName;
	FLTextFile		m_cTextFile;

	size_t			m_nMaxSize;
	FLLogFormat*	m_pFormater;

};







#endif