
#ifndef	__FLCOMMONDUMPER_H__
#define	__FLCOMMONDUMPER_H__


#include <dump/FLDumper.h>
#include <dump/FLGenericDumper.h>


class	FLCommonDumper : public FLDumper
{
public:

	static FLCommonDumper*	GetInstance()
	{
		static FLCommonDumper kCommonDumper( NULL );
		return &kCommonDumper;
	}

public:

	explicit FLCommonDumper( const TCHAR* szPrefix )
	{
		m_kGenericDumper.SetPrefix( szPrefix );
	}

private:

	bool	DoDump( T_DUMP_PARAM* pParam )
	{
		FLLOG_FLUSH();

		return m_kGenericDumper.Dump( pParam );
	}

private:

	FLGenericDumper	m_kGenericDumper;

};






#endif