#include "StdAfx.h"

#include "FLShutdownRule_Mng.h"

#include <../../_Common/FLShutdownRule.h>

FLShutdownRule_Mng & FLShutdownRule_Mng::GetInstance()
{
	static FLShutdownRule_Mng inst;
	return inst;
}


void FLShutdownRule_Mng::AddUser( const u_long uPlayerID, const CTime & tmBirthDate )
{
	if( FLShutdownRule::IsShutDownAge( tmBirthDate ) == false )
		return;

	const bool ret = m_mapDoubtUser.insert( make_pair( uPlayerID, tmBirthDate ) ).second;
	UNREFERENCED_PARAMETER( ret );
}

void FLShutdownRule_Mng::RemoveUser( const u_long uPlayerID )
{
	m_mapDoubtUser.erase( uPlayerID );
}

void FLShutdownRule_Mng::TryShutdown()
{
	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
		return;

	//셧다운 시간인가?
	if( FLShutdownRule::IsShutDownTime() == false )
		return;

	for( std::map< u_long, CTime >::iterator it = m_mapDoubtUser.begin(); it != m_mapDoubtUser.end(); ++it )
	{
		const u_long uPlayerID	= it->first;
		const CTime tmBirthDate = it->second;

		if( FLShutdownRule::IsShutDownAge( tmBirthDate ) == false )
			continue;

		FLWSUser *pUser			= g_xWSUserManager->GetUserByPlayerID( uPlayerID );	
		if( pUser == NULL )
			continue;

		pUser->AddDefinedText( TID_MMI_SHUTDOWNNOTICE02 );
		//g_xWSUserManager->RemoveUser( pUser->m_Snapshot.dpidUser );
		g_DPSrvr.QueryDestroyPlayer( pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer );
	}	

	m_mapDoubtUser.clear();
}

