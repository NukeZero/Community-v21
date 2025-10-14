
#ifndef	__FLGENERICDUMPER_H__
#define	__FLGENERICDUMPER_H__


#include "FLDumpCommon.h"
#include "FLDumper.h"


class	FLGenericDumper : public FLDumper
{
public:

	static	FLGenericDumper*	GetInstance();

public:

	FLGenericDumper();

	void			SetDumpFolder( const TCHAR* szDumpFolder );
	const TCHAR*	GetDumpFolder()	const;

	void			SetPrefix( const TCHAR* szPrefix );
	const TCHAR*	GetPrefix()	const;

private:

	bool	DoDump( T_DUMP_PARAM* pParam );

private:

	TCHAR	m_szDumpFolder[MAX_PATH];
	TCHAR	m_szPrefix[MAX_PATH];

};







#endif