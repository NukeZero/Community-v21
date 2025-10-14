
#ifndef	__FLDUMPER_H__
#define	__FLDUMPER_H__


#include "FLDumpCommon.h"
#include "FLDumpParam.h"



class	FLDumper
{
public:

	virtual ~FLDumper();

	bool	Dump( T_DUMP_PARAM* pParam );

private:

	virtual bool	DoDump( T_DUMP_PARAM* pParam )	= 0;

};




#endif