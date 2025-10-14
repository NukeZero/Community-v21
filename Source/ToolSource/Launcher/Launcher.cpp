// Launcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "launcher.h"
#include "peer.h"

#include <dump/FLDumpInstaller.h>
#include "../_CommonDefine/FLCommonDumper.h"
#include <FLIncludeAllLibrary.h>

/////////////////////////////////////////////////////////////////////////////
CWinApp theApp;

using namespace std;

int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	FLDumpInstaller::Install( FLCommonDumper::GetInstance() );

#if	defined(FLASSERT_ENABLE) && defined(FLASSERT_HOOKER_ENABLE)
	FLDumpInstaller::InstallFLAssertHooker();
#endif

	CTime time	= CTime::GetCurrentTime();
	printf( "%s\n", time.Format( "%Y/%m/%d %H:%M:%S" ) );

	FLLOG_ADD_NETWORK( NETWORK_LOG_NAME );
	if( InitializeNetLib() == FALSE )
	{
		return 0;
	}

	CPeer peer;

	if( AfxWinInit( GetModuleHandle( NULL ), NULL, GetCommandLine(), 0) && peer.InitializeMyProcess() )
	{
//		InitInstance
		if( FALSE == peer.ConnectToServer( peer.m_lpAddr, PN_MONITOR_2, FLNormalProtocol::GetInstance(), FALSE ) )
			goto lbl_exit;
		peer.SendMyProcess();

		char buffer[256];
		while( 1 )
		{
			cin >> buffer;
				
			if( !strcmp( buffer, "q" ) )
			{
				break;
			}
			else if( ::strcmp( buffer, "dump" ) == 0 )
			{
				peer.DumpMyProcess( FLDUMP_LEVEL_MEDIUM );
			}
			else if( ::strcmp( buffer, "fulldump" ) == 0 )
			{
				peer.DumpMyProcess( FLDUMP_LEVEL_HEAVY );
			}
		}
	}
lbl_exit:
//	ExitInstance
	peer.DeleteDPObject();

	UninitializeNetLib();

#if	defined(FLASSERT_ENABLE) && defined(FLASSERT_HOOKER_ENABLE)
	FLDumpInstaller::UninstallFLAssertHooker();
#endif

	FLDumpInstaller::Uninstall();
	FLLOG_FLUSH();
	return 0;
}