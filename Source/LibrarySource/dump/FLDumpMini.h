#ifndef __FLDUMPMINI_H__
#define __FLDUMPMINI_H__


#include "FLDumpCommon.h"
#include "FLDumper.h"


class	FLDumpMini : public FLDumper
{
public:

	FLDumpMini( const TCHAR* szDumpFolder, const TCHAR* szPrefix );

private:

	bool	DoDump( T_DUMP_PARAM* pParam );

private:

	const TCHAR*	m_szDumpFolder;
	const TCHAR*	m_szPrefix;

};



#endif
