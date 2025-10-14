
//note: associate XTrap
//date: 2011/05/25
//author: gmpbigsun

#pragma  once

#ifdef _USE_XTRAP

#include "urlmon.h"
#pragma  comment( lib, "urlmon" )
#pragma  comment( lib, "wininet" )

#include "xtrap/20110309_B5000012_Xtrap_L_Interface.h"		//Launcher
#include "xtrap/20110503_522_Xtrap_C_Interface.h"			//Client

//runtime library : multi thread 
#if ( defined _MT )
	#if ( defined _DLL ) 
		#pragma comment( lib, "20110503_522_XTrap4Client_mtDll.lib" )				//런쳐 라이브러리와 함수중복때문에 젤먼져 로드해야한다.
		#pragma comment( lib, "20110503_522_XTrap4Client_ClientPart_mtDll.lib" )
		#pragma comment( lib, "20110503_522_XTrap4Client_ServerPart_mtDll.lib" )
		#pragma comment( lib, "20110503_522_XTrap_Unipher_mtDll.lib" )
		#pragma comment( lib, "20110309_B5000012_XTrap4Launcher_mtDll.lib" )
	#else
		#pragma comment( lib, "20110503_522_XTrap4Client_mt.lib" )				//런쳐 라이브러리와 함수중복때문에 젤먼져 로드해야한다.
		#pragma comment( lib, "20110503_522_XTrap4Client_ClientPart_mt.lib" )
		#pragma comment( lib, "20110503_522_XTrap4Client_ServerPart_mt.lib" )
		#pragma comment( lib, "20110503_522_XTrap_Unipher_mt.lib" )
		#pragma comment( lib, "20110309_B5000012_XTrap4Launcher_mt.lib" )
	#endif 
#else 
	#error to do coding
#endif 

void InitXTrap( )
{
//  http://xtrap.patch.gamecyber.net/flyff
//	char szArgv[] = "660970B4CA29DC1490256D9844CFE86267535E09A5C4705789C5584BF3915F5AAD73CB9D4147C363E590CF67ACC1D421DFEA34C79E5EB5382EA2AB389A39E43A0F7D04245246E41FB1670C4E95407619E96980387320B6505436FCED87B1E5752B817F75F0ED4F";		//WISELOGIC 지원팀에게  받은 데이터 ( Launcher와 Client 동일한값 )
//	char szArgv[] = "660970B4CA29DC14C5266D9844CFED6242CAB65D14E4D8D909F7C70145FD173A4BEF5F3530DEB1E9B11C741F5D3DA7C7D03BBA55AADDB8051ECA4F265D5BE6040F7D04245246E41FB1670C4E95407619E96980387320B6505436FCED87B1E5752B817F75F0ED";

	TCHAR szArgv[512]	= { 0, };

 	if( g_xFlyffConfig->GetMainLanguage() == LANG_HK )
 	{
 		FLStrcpy( szArgv, _countof( szArgv ), _T( "660970B4CA29DC14C5266D9844CFED6242CAB65D14E4D8D909F7C70145FD173A4BEF5F3530DEB1E9B11C741F5D3DA7C7D03BBA55AADDB8051ECA4F265D5BE6040F7D04245246E41FB1670C4E95407619E96980387320B6505436FCED87B1E5752B817F75F0ED" ) );
 	}
 	else if( g_xFlyffConfig->GetMainLanguage() == LANG_ID )
	{
		FLStrcpy( szArgv, _countof( szArgv ), _T( "660970B4C929DCB0C5206D9849CFED6210B855A043CC0B46F75B65135F84F903069A15E5C9FE9807F7411CCE175779E9BDA9F30E042F566D9E3D8402BC1A5A250F7D04245246E41FB1670C4E95567B14F008CF84341DEAE326E156202951E7" ) );
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "XTRAP PATCH ..." ) );
	XTrap_L_Patch( szArgv, NULL, 60 ); 
	FLERROR_LOG( PROGRAM_NAME, _T( "XTRAP PATCH OK" ) );

	FLERROR_LOG( PROGRAM_NAME, _T( "XTRAP START" ) );
	XTrap_C_Start( ( LPCSTR )szArgv, NULL );		//arg2: xtrap경로
	FLERROR_LOG( PROGRAM_NAME, _T( "XTRAP START OK" ) );

}

#endif //_USE_XTRAP