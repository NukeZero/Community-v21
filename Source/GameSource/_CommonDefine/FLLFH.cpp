#include "StdAfx.h"



#include "FLLFH.h"



FLLFH::FLLFH()
{
}

FLLFH::~FLLFH()
{

}

FLLFH & FLLFH::GetInstance()
{
	static FLLFH s_LFH;
	return s_LFH;
}


void FLLFH::Install()
{
	//const size_t MAX_HEAP_HANDLE = 1025;

	//HANDLE hHeapArray[ MAX_HEAP_HANDLE ];
	//const size_t uCount				= ::GetProcessHeaps( 1024, hHeapArray );
	//if( uCount == 0 )
	//{
	//	FLERROR_LOG( PROGRAM_NAME, _T( "[ LFH - ::GetProcessHeaps Error(%d) ]" ), ::GetLastError() );
	//	return;
	//}

	//for( DWORD i = 0; i < uCount; ++i )
	//{
	//	ULONG uHeapFragValue = 2;

	//	if( ::HeapSetInformation( hHeapArray[ i ], HeapCompatibilityInformation, &uHeapFragValue, sizeof( uHeapFragValue ) ) == FALSE )
	//	{
	//		FLERROR_LOG( PROGRAM_NAME, _T( "[ LFH - FAIL APPLY : TOT INDEX(%u) HEAP INDEX(%u) ERROR(%d) ]" ), uCount, i, ::GetLastError() );
	//		continue;
	//	}
	//}
}

