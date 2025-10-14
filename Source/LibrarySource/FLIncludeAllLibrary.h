
#ifndef	__FLINCLUDEALLLIBRARY_H__
#define	__FLINCLUDEALLLIBRARY_H__

#if	!defined( _MT )
	#error	Please use the /MT(d) or /MD(d) switch
#endif

#ifdef	_DLL
	#ifdef	_DEBUG
		#pragma comment( lib, "DebugDll/FLBugtrap.lib" )
		#pragma comment( lib, "DebugDll/FLCommon.lib" )
		#pragma comment( lib, "DebugDll/FLCrypto.lib" )
		#pragma comment( lib, "DebugDll/FLDump.lib" )
		#pragma comment( lib, "DebugDll/FLLogging.lib" )
		#pragma comment( lib, "DebugDll/FLMemory.lib" )
		#pragma comment( lib, "DebugDll/FLNetwork.lib" )
		#pragma comment( lib, "DebugDll/FLUI.lib" )
	#else
		#pragma comment( lib, "ReleaseDll/FLBugtrap.lib" )
		#pragma comment( lib, "ReleaseDll/FLCommon.lib" )
		#pragma comment( lib, "ReleaseDll/FLCrypto.lib" )
		#pragma comment( lib, "ReleaseDll/FLDump.lib" )
		#pragma comment( lib, "ReleaseDll/FLLogging.lib" )
		#pragma comment( lib, "ReleaseDll/FLMemory.lib" )
		#pragma comment( lib, "ReleaseDll/FLNetwork.lib" )
		#pragma comment( lib, "ReleaseDll/FLUI.lib" )
	#endif
#else
	#ifdef	_DEBUG
		#pragma comment( lib, "Debug/FLBugtrap.lib" )
		#pragma comment( lib, "Debug/FLCommon.lib" )
		#pragma comment( lib, "Debug/FLCrypto.lib" )
		#pragma comment( lib, "Debug/FLDump.lib" )
		#pragma comment( lib, "Debug/FLLogging.lib" )
		#pragma comment( lib, "Debug/FLMemory.lib" )
		#pragma comment( lib, "Debug/FLNetwork.lib" )
		#pragma comment( lib, "Debug/FLUI.lib" )
	#else
		#pragma comment( lib, "Release/FLBugtrap.lib" )
		#pragma comment( lib, "Release/FLCommon.lib" )
		#pragma comment( lib, "Release/FLCrypto.lib" )
		#pragma comment( lib, "Release/FLDump.lib" )
		#pragma comment( lib, "Release/FLLogging.lib" )
		#pragma comment( lib, "Release/FLMemory.lib" )
		#pragma comment( lib, "Release/FLNetwork.lib" )
		#pragma comment( lib, "Release/FLUI.lib" )
	#endif
#endif


#endif