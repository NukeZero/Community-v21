
#ifndef __FLDUMPINSTALLER_H__
#define __FLDUMPINSTALLER_H__


#include "FLDumpCommon.h"
#include "FLDumper.h"
#include "FLGenericDumper.h"

#include <cstdlib>


class	FLDumpInstaller
{
public:

	static bool	Install( FLDumper* pDumper = FLGenericDumper::GetInstance(), int nLevel = FLDUMP_LEVEL_HEAVY );
	static void	Uninstall();

	static void	InstallThreadExceptionHandler();

	static void	InstallFLAssertHooker();
	static void	UninstallFLAssertHooker();

private:

	static LONG WINAPI	TopLevelFilter( EXCEPTION_POINTERS* pException );

	static void	CRTPurecallHandler();

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
	static void	CRTInvalidParameterHandler( const wchar_t* szExpression, const wchar_t* szFunction, const wchar_t* szFile, unsigned int nLine, uintptr_t nReserved );
#endif

	static bool	FLAssertDumpHooker( const wchar_t* szExpression, const wchar_t* szDescription, const wchar_t* szFunction, const wchar_t* szFile, int nLine, bool* pIgnoreAlways );

	static void	CRTTerminateHandler();
	static void	CRTUnexpectedHandler();
	static int	CRTNewHandler( size_t );

#if defined(_MSC_VER) && (_MSC_VER >= 1300) && (_MSC_VER < 1400)
	static void	CRTSecurityErrorHandler( int nCode, void* /*pUnused*/ );
#endif

private:

	static int							s_nDumpLevel;
	static FLDumper*					s_pDumper;

	static PTOP_LEVEL_EXCEPTION_FILTER	s_pPrevUEFilter;
	static _purecall_handler			s_pPrevPCHandler;
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
	static _invalid_parameter_handler	s_pPrevIPHandler;
#endif

	static FLAssertHooker				s_pPrevFLAHooker;

private:

	FLDumpInstaller();
	~FLDumpInstaller();

};



#endif

