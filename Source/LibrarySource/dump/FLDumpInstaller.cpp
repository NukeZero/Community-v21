
#include "FLDumpInstaller.h"
#include "FLDumpRunner.h"

#include "../common/FLStringFunction.h"

#include <exception>
#include <new.h>


namespace
{
	void	FLConvertTChar( const wchar_t* szExpression, const wchar_t* szFunction, const wchar_t* szFile,
							TCHAR* szTempExpression, size_t cchTempExpression,
							TCHAR* szTempFunction, size_t cchTempFunction,
							TCHAR* szTempFile, size_t cchTempFile )
	{
#if !defined(_UNICODE) && !defined(UNICODE)
		if( cchTempExpression > INT_MAX || cchTempFunction > INT_MAX || cchTempFile > INT_MAX )
		{
			return;
		}

		FLWideCharToMultByte( CP_ACP, szExpression, -1, szTempExpression, static_cast< int >( cchTempExpression ) );
		FLWideCharToMultByte( CP_ACP, szFunction, -1, szTempFunction, static_cast< int >( cchTempFunction ) );
		FLWideCharToMultByte( CP_ACP, szFile, -1, szTempFile, static_cast< int >( cchTempFile ) );
#else
		FLStrcpy( szTempExpression, cchTempExpression, szExpression );
		FLStrcpy( szTempFunction, cchTempFunction, szFunction );
		FLStrcpy( szTempFile, cchTempFile, szFile );
#endif
	}
}

//////////////////////////////////////////////////////////////////////////
FLDumper*					FLDumpInstaller::s_pDumper			= NULL;
int							FLDumpInstaller::s_nDumpLevel		= FLDUMP_LEVEL_NONE;

PTOP_LEVEL_EXCEPTION_FILTER	FLDumpInstaller::s_pPrevUEFilter	= NULL;
_purecall_handler			FLDumpInstaller::s_pPrevPCHandler	= NULL;
#if	defined(_MSC_VER) && (_MSC_VER > 1310)
_invalid_parameter_handler	FLDumpInstaller::s_pPrevIPHandler	= NULL;
#endif

FLAssertHooker				FLDumpInstaller::s_pPrevFLAHooker	= NULL;

bool	FLDumpInstaller::Install( FLDumper* pDumper, int nLevel )
{
	s_pDumper = pDumper;
	s_nDumpLevel = nLevel;

	// win32 api handler
	s_pPrevUEFilter = ::SetUnhandledExceptionFilter( FLDumpInstaller::TopLevelFilter );

	// crt handler
	s_pPrevPCHandler = ::_set_purecall_handler( FLDumpInstaller::CRTPurecallHandler );

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
	s_pPrevIPHandler = ::_set_invalid_parameter_handler( FLDumpInstaller::CRTInvalidParameterHandler );
#endif


//#ifndef	_DEBUG
//	::_set_error_mode( _OUT_TO_STDERR );
//#endif

	::_set_new_mode( 1 ); // Force malloc() to call new handler too
	::_set_new_handler( FLDumpInstaller::CRTNewHandler );

#if defined(_MSC_VER) && (_MSC_VER >= 1300) && (_MSC_VER < 1400)
	// The _set_security_error_handler is deprecated in VC8 C++ run time library
	::_set_security_error_handler( FLDumpInstaller::CRTSecurityErrorHandler );
#endif

	// 이전과 호환을 위해
	InstallThreadExceptionHandler();

	FLDumpRunner* pRunner = FLDumpRunner::GetInstance();
	if( pRunner == NULL )
	{
		return false;
	}

	if( pRunner->Start() == false )
	{
		return false;
	}

	return true;
}

void	FLDumpInstaller::Uninstall()
{
	FLDumpRunner* pRunner = FLDumpRunner::GetInstance();
	if( pRunner != NULL )
	{
		pRunner->Stop();
	}

	if( s_pPrevUEFilter != NULL )
	{
		::SetUnhandledExceptionFilter( s_pPrevUEFilter );
		s_pPrevUEFilter = NULL;
	}

	if( s_pPrevPCHandler != NULL )
	{
		::_set_purecall_handler( s_pPrevPCHandler );
		s_pPrevPCHandler = NULL;
	}

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
	if( s_pPrevIPHandler != NULL )
	{
		::_set_invalid_parameter_handler( s_pPrevIPHandler );
		s_pPrevIPHandler = NULL;
	}
#endif

	s_pDumper = NULL;
	s_nDumpLevel = FLDUMP_LEVEL_NONE;
}
#include <eh.h>
void	FLDumpInstaller::InstallThreadExceptionHandler()
{
	std::set_terminate( FLDumpInstaller::CRTTerminateHandler );
	set_unexpected(FLDumpInstaller::CRTUnexpectedHandler );
}

void	FLDumpInstaller::InstallFLAssertHooker()
{
	s_pPrevFLAHooker = FLSetAssertHooker( FLDumpInstaller::FLAssertDumpHooker );
}

void	FLDumpInstaller::UninstallFLAssertHooker()
{
	FLSetAssertHooker( s_pPrevFLAHooker );
	s_pPrevFLAHooker = NULL;
}

LONG	FLDumpInstaller::TopLevelFilter( EXCEPTION_POINTERS* pException )
{
	if( FLDumpRunner::Execute( s_pDumper, s_nDumpLevel, ::GetCurrentProcess(), ::GetCurrentProcessId(), ::GetCurrentThread(), ::GetCurrentThreadId(), pException, NULL, NULL, NULL, 0 ) == false )
	{
		if( s_pPrevUEFilter != NULL )
		{
			return s_pPrevUEFilter( pException );
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void	FLDumpInstaller::CRTPurecallHandler()
{
	if( FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, _T( "Purecall" ), _T( "CRTPurecallHandler" ), NULL, 0 ) == false )
	{
		if( s_pPrevPCHandler != NULL )
		{
			s_pPrevPCHandler();
		}
	}
}

#if	defined(_MSC_VER) && (_MSC_VER > 1310)
void	FLDumpInstaller::CRTInvalidParameterHandler( const wchar_t* szExpression, const wchar_t* szFunction, const wchar_t* szFile, unsigned int nLine, uintptr_t nReserved )
{
	//////////////////////////////////////////////////////////////////////////
	TCHAR szTempExpression[256] = { 0, };
	TCHAR szTempFunction[256] = { 0, };
	TCHAR szTempFile[MAX_PATH] = { 0, };

	FLConvertTChar( szExpression, szFunction, szFile,
					szTempExpression, _countof( szTempExpression ),
					szTempFunction, _countof( szTempFunction ),
					szTempFile, _countof( szTempFile ) );
	//////////////////////////////////////////////////////////////////////////

	if( FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, szTempExpression, szTempFunction, szTempFile, nLine ) == false )
	{
		if( s_pPrevIPHandler != NULL )
		{
			s_pPrevIPHandler( szExpression, szFunction, szFile, nLine, nReserved );
		}
	}
}
#endif

bool	FLDumpInstaller::FLAssertDumpHooker( const wchar_t* szExpression, const wchar_t* szDescription, const wchar_t* szFunction, const wchar_t* szFile, int nLine, bool* pIgnoreAlways )
{
	(szDescription);

	//////////////////////////////////////////////////////////////////////////
	TCHAR szTempExpression[256] = { 0, };
	TCHAR szTempFunction[256] = { 0, };
	TCHAR szTempFile[MAX_PATH] = { 0, };

	FLConvertTChar( szExpression, szFunction, szFile,
					szTempExpression, _countof( szTempExpression ),
					szTempFunction, _countof( szTempFunction ),
					szTempFile, _countof( szTempFile ) );
	//////////////////////////////////////////////////////////////////////////

	FLPrintCallStackHelper( _T( "FLAssertLog" ), szTempExpression, szTempFunction, szTempFile, nLine );

	if( pIgnoreAlways != NULL )
	{
		*pIgnoreAlways = true;
	}

	return true;
}

void __cdecl	FLDumpInstaller::CRTTerminateHandler()
{
	FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, _T( "Terminate" ), _T( "CRTTerminateHandler" ), NULL, 0 );
}

void __cdecl	FLDumpInstaller::CRTUnexpectedHandler()
{
	FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, _T( "Unexpected" ), _T( "CRTUnexpectedHandler" ), NULL, 0 );
}

int __cdecl	FLDumpInstaller::CRTNewHandler( size_t /*nSize*/ )
{
	FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, _T( "New" ), _T( "CRTNewHandler" ), NULL, 0 );
	return 0;
}

#if defined(_MSC_VER) && (_MSC_VER >= 1300) && (_MSC_VER < 1400)
void __cdecl	FLDumpInstaller::CRTSecurityErrorHandler( int nCode, void* /*pUnused*/ )
{
	if( nCode == _SECERR_BUFFER_OVERRUN )
	{
		FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, _T( "BufferOverrun" ), _T( "CRTSecurityErrorHandler" ), NULL, 0 );
	}
	else
	{
		FLDumpRunner::DumpNow( s_pDumper, s_nDumpLevel, EXCEPTION_NONCONTINUABLE_EXCEPTION, _T( "SecurityError" ), _T( "CRTSecurityErrorHandler" ), NULL, 0 );
	}
}
#endif
