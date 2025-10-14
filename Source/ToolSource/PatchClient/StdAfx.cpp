// stdafx.cpp : source file that includes just the standard includes
//	BetaPatchClient.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


#ifdef __CHINA
 #ifdef _DEBUG
  #pragma comment(lib, "../LIB/EtcPC_debug.lib") 
 #else // DEBUG
  #pragma comment(lib, "../LIB/EtcPC_release.lib") 
 #endif // not DEBUG
#endif // CHINA

TCHAR	PROGRAM_NAME[128] = _T( "PatchClient" );
TCHAR	NETWORK_LOG_NAME[128] = _T( "PCNetwork" );

CString _fnLoadString( UINT id, ... )
{
    CString sID;
    sID.LoadString(id);

    va_list vl;
    va_start( vl, id );     /* Initialize variable arguments. */

    CString sMsg;
    sMsg.FormatV( sID, vl );

    va_end( vl );              /* Reset variable arguments. */

    return sMsg;
}
