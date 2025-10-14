#include "stdafx.h"
#include "NpcScript.h"

BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                       DWORD  /*ul_reason_for_call*/, 
                       LPVOID /*lpReserved*/
					 )
{
/*	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		if( InitDLL( "WorldDialog.txt" ) == FALSE )
		{
		#ifdef _DEBUG
			MessageBox( NULL, "InitDLL return FALSE", "error", MB_OK );
		#else
			OutputDebugString( "InitDLL(WorldDialog.txt) return FALSE" );
		#endif
			return FALSE;
		} 
	}
*/

    return TRUE;
}

INT APIENTRY RunDialog( LPCTSTR szNpc, LPCTSTR szKey, NPCDIALOG_INFO* pInfo )
{


#ifdef _DEBUG
	//char szBuffer[256] = { 0, };
	//StringCchPrintfEx( szBuffer, sizeof( szBuffer ) /sizeof( szBuffer[0] ), NULL, NULL, 0, "RunDialog(%s, %s)\n", szNpc, szKey );
	//OutputDebugString( szBuffer );
#endif

	if( szNpc == NULL || szKey == NULL || pInfo == NULL )
	{
		return 0;
	}

	//CNpcScript obj;
	//obj.m_pInfo = pInfo;
	//obj.DoExec( szNpc, szKey );			//@@@@@@@@@@ 여기서 내가 만든 스크립트 호출.
	//obj.m_pInfo = NULL;
	
	//CNpcScript::GetInstance().m_pInfo = pInfo;
	//CNpcScript::GetInstance().DoExec( szNpc, szKey );
	//CNpcScript::GetInstance().m_pInfo = NULL;

	GetNPCScriptInstance().m_pInfo		= pInfo;
	GetNPCScriptInstance().DoExec( szNpc, szKey );
	GetNPCScriptInstance().m_pInfo		= NULL;


	return 1;
}

INT APIENTRY FindScriptKey( LPCTSTR szNpc, LPCTSTR szKey )
{
	if( szNpc == NULL || szKey == NULL )
	{
		return 0;
	}

	//CNpcScript obj;
	//obj.m_pInfo = NULL;
	//return obj.Find( szNpc, szKey );
	//CNpcScript::GetInstance().m_pInfo = NULL;
	//return CNpcScript::GetInstance().Find( szNpc, szKey );

	//GetNPCScriptInstance().m_pInfo		= NULL;
	return GetNPCScriptInstance().Find( szNpc, szKey );
}

void APIENTRY SetLanguage(INT nLanguage)
{
	//CNpcScript script;
	//script.SetLang(nLanguage);
	//CNpcScript::GetInstance().SetLang( nLanguage );

	GetNPCScriptInstance().SetLang( nLanguage );
}

BOOL APIENTRY InitWorldDialog()
{
	FLINFO_LOG( PROGRAM_NAME, _T( "[ START - DLL LOG ]" ) );


	if( InitDLL( "WorldDialog.txt" ) == FALSE )
	{
#ifndef _DEBUG
//		MessageBox( NULL, "InitDLL return FALSE", "error", MB_OK );
#else
		OutputDebugString( "InitDLL(WorldDialog.txt) return FALSE" );
#endif
		FLERROR_LOG( PROGRAM_NAME, "[ InitDLL(WorldDialog.txt) return FALSE ]" );
		DEBUG_BREAK;
		return FALSE;
	}


	FLCStyleReader script( "NpcScript.txt" );
	if( CompileScript( script ) == false )
	{
		FLERROR_LOG( PROGRAM_NAME, "[ FAIL - LOAD SCRIPT ]" );
		DEBUG_BREAK;
		return false;
	}

	return TRUE;
}


void APIENTRY CloseWorldDialog()
{
	FLLOG_FLUSH();
}