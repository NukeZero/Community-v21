
#include "FLDumper.h"



FLDumper::~FLDumper()
{
}

bool	FLDumper::Dump( T_DUMP_PARAM* pParam )
{
	return DoDump( pParam );
}
