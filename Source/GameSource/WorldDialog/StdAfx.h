// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F7468BB9_04DB_437A_95C9_3F2EEBF4511A__INCLUDED_)
#define AFX_STDAFX_H__F7468BB9_04DB_437A_95C9_3F2EEBF4511A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <string>
#include <vector>
#include <map>

#include <tchar.h>

#pragma warning( push )
#pragma warning(disable:4996)
#include <strsafe.h>
#pragma warning( pop )

//#include <atltypes.h>
//#include <atlstr.h>

using namespace std;

#include <define.h>
#include <defineEvent.h>
//#include "defineFlyffEvent.h"
#include <defineItem.h>
#include <defineItemkind.h>
#include <defineJob.h>
#include <defineNeuz.h>
#include <defineObj.h>
#include <definequest.h>
#include <defineSound.h>
#include <defineText.h>
#include <defineWorld.h>
#include <lang.h>


#include "FLDefine.h"

#ifndef	_DEBUG
#define	__FLLOG_EXCLUDE_DEBUG_TRACE
#endif

#include <logging/FLLogFacade.h>

#if	!defined( _MT )
#error	Please use the /MT(d) or /MD(d) switch
#endif

#ifdef	_DLL
#ifdef	_DEBUG
#pragma comment( lib, "DebugDll/FLCommon.lib" )
#pragma comment( lib, "DebugDll/FLCrypto.lib" )
#pragma comment( lib, "DebugDll/FLDump.lib" )
#pragma comment( lib, "DebugDll/FLLogging.lib" )
#pragma comment( lib, "DebugDll/FLMemory.lib" )
#pragma comment( lib, "DebugDll/FLNetwork.lib" )
#pragma comment( lib, "DebugDll/FLUI.lib" )
#else
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
#pragma comment( lib, "Debug/FLCommon.lib" )
#pragma comment( lib, "Debug/FLCrypto.lib" )
#pragma comment( lib, "Debug/FLDump.lib" )
#pragma comment( lib, "Debug/FLLogging.lib" )
#pragma comment( lib, "Debug/FLMemory.lib" )
#pragma comment( lib, "Debug/FLNetwork.lib" )
#pragma comment( lib, "Debug/FLUI.lib" )
#else
#pragma comment( lib, "Release/FLCommon.lib" )
#pragma comment( lib, "Release/FLCrypto.lib" )
#pragma comment( lib, "Release/FLDump.lib" )
#pragma comment( lib, "Release/FLLogging.lib" )
#pragma comment( lib, "Release/FLMemory.lib" )
#pragma comment( lib, "Release/FLNetwork.lib" )
#pragma comment( lib, "Release/FLUI.lib" )
#endif
#endif




extern	TCHAR	PROGRAM_NAME[128];

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F7468BB9_04DB_437A_95C9_3F2EEBF4511A__INCLUDED_)
