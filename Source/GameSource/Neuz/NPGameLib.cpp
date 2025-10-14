#include "stdafx.h"

#include "timer.h"
#include "NPGameLib.h"

#ifdef __Gameguard
//#pragma comment( lib, "NPGameLib.lib" )

static	CTimer	g_timer( SEC( 10 ) );

CNPGameLib* GetNProtect()
{
//gmpbigsun: 최후의 안전스위치 
#ifdef _USE_XTRAP
	::AfxMessageBox( "Using the x-trap, turn off the nprotect" );
	FLERROR_LOG( PROGRAM_NAME, _T( "Using the x-trap, turn off the nprotect" ) );
	PostQuitMessage( 0 );
#endif //_USE_XTRAP

	static CNPGameLib npgl( g_xFlyffConfig->GetNProtectConfigFileName() );

	return &npgl;
}

BOOL CheckNProtect()
{
	if( g_timer.IsOver() )
	{
		g_timer.Reset();
		return ( GetNProtect()->Check() == NPGAMEMON_SUCCESS );
	}

	return TRUE;
}
#endif