// EventMonster.cpp: implementation of the CEventMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventMonster.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef __EVENT_MONSTER

CEventMonster::CEventMonster()
{

}

CEventMonster::~CEventMonster()
{

}

CEventMonster* CEventMonster::GetInstance( void )
{	
	static CEventMonster sEventMonster;
	return &sEventMonster;
}

void CEventMonster::LoadScript()
{
	if( _GetContentState( CT_DROP_ITEM_REFORM ) == CS_VER1 )
		return;


	CLuaBase lua;

	const TCHAR szFileName[]	= _T( "EventMonster.lua" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( lua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		FLASSERT(0);
	}

	lua.GetGloabal( "tEventMonster" );
	lua.PushNil();
	while( lua.TableLoop( -2 ) )
	{
		__EVENTMONSTER EventMonster;

		DWORD	dwId = CScript::GetDefineNum( lua.GetFieldToString( -1, "strMonsterId" ) );
		EventMonster.nLevel = static_cast<int>( lua.GetFieldToNumber( -1, "nLevel" ) );
		EventMonster.dwLootTime = static_cast<DWORD>( lua.GetFieldToNumber( -1, "nLootTime" ) );
		EventMonster.fItemDropRange = static_cast<float>( lua.GetFieldToNumber( -1, "fItemDropRange" ) );
		EventMonster.bPet = lua.GetFieldToBool( -1, "bPet" );
		EventMonster.bGiftBox = lua.GetFieldToBool( -1, "bGiftBox" );
		
		m_mapEventMonster.insert( make_pair( dwId, EventMonster ) );
		lua.Pop( 1 );
	}

	lua.Pop(0);
}

BOOL CEventMonster::IsEventMonster( DWORD dwId )
{
	std::map< DWORD, __EVENTMONSTER >::iterator it = m_mapEventMonster.find( dwId );
	if( it != m_mapEventMonster.end() )
		return TRUE;
	
	return FALSE;
}

BOOL CEventMonster::SetEventMonster( DWORD dwId )
{
	m_it = m_mapEventMonster.find( dwId );
	if( m_it != m_mapEventMonster.end() )
		return TRUE;

	return FALSE;	
}

BOOL CEventMonster::IsPetAble()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.bPet;
	
	return FALSE;	
}

BOOL CEventMonster::IsGiftBoxAble()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.bGiftBox;

	return FALSE;	
}


DWORD CEventMonster::GetLootTime()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.dwLootTime;
	
	return 0;
}

int CEventMonster::GetLevelGap()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.nLevel;
	
	return MAX_CHARACTER_LEVEL;
}

float CEventMonster::GetItemDropRange()
{
	if( m_it != m_mapEventMonster.end() )
		return m_it->second.fItemDropRange;

	return 2.0f;
}
#endif // __EVENT_MONSTER