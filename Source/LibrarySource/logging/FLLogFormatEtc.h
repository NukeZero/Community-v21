
#ifndef	__FLLOGFORMATETC_H__
#define	__FLLOGFORMATETC_H__


#include "FLLogCommon.h"
#include "FLLogFormat.h"


//////////////////////////////////////////////////////////////////////////

class	FLLogSampleFormat : public FLLogFormat
{
private:

	bool	DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount );

};

//////////////////////////////////////////////////////////////////////////

class	FLLogGenericFormat : public FLLogFormat
{
private:

	bool	DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount );

};

//////////////////////////////////////////////////////////////////////////

class	FLLogCSVFormat : public FLLogFormat
{
private:

	bool	DoFormat( const FLLogEvent* pEvent, TCHAR* szBuffer, size_t cchCount );

};

//////////////////////////////////////////////////////////////////////////



#endif