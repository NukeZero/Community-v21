#include "stdafx.h"

#ifdef __WORLDSERVER
#include "guildquest.h"
#include "guild.h"
#include "../WorldServer/user.h"
extern	CGuildMng	g_GuildMng;

CGuildQuestProcessor::CGuildQuestProcessor()
{
	m_nRect	= 0;
}

CGuildQuestProcessor::~CGuildQuestProcessor()
{

}

void CGuildQuestProcessor::Process()
{
	DWORD dwTickCount	= GetTickCount();
	for( int i = 0; i < MAX_GUILD_QUEST; i++ )
	{
		PGUILDQUESTELEM pElem = &m_pElem[i];
		if( pElem->nId == i )
		{
			CGuild* pGuild	= g_GuildMng.GetGuild( pElem->idGuild );
//			if( !pGuild )
//				continue;

			GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( pElem->nId );
			if( !pProp )
				continue;

			if( pElem->dwEndTime < dwTickCount )
			{
				switch( pElem->nProcess )
				{
					case GQP_WORMON:
						{
							FLTRACE_LOG( PROGRAM_NAME, _T( "GQP_WORMON - r" ) );

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

							if( pGuild )
							{
								pGuild->SetQuest( pElem->nId, pElem->nf );
								g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, pElem->nId, pElem->nf );
								pGuild->ReplaceLodestar( rect );
							}

							CMover* pWormon	= prj.GetMover( pElem->objidWormon );
							if( pWormon )
								pWormon->Delete();
							RemoveGuildQuest( pElem->nId );
						}
						break;
					case GQP_GETITEM:
						{
							FLTRACE_LOG( PROGRAM_NAME, _T( "GQP_GETITEM - r" ) );

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );

							if( pGuild )
							{
								pGuild->ReplaceLodestar( rect );
							}

							RemoveGuildQuest( pElem->nId );
						}
						break;
					default:
						break;
				}
			}
			else
			{
				switch( pElem->nProcess )
				{
					case GQP_WORMON:
						{
							FLTRACE_LOG( PROGRAM_NAME, _T( "GQP_WORMON - p" ) );
							
							if( ++pElem->nCount < 10 )
								continue;

							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bsurvivor	= FALSE;	// is survivor exists?

							if( pGuild )
							{
								FLWSUser* pUser;
								CGuildMember* pMember;
								for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
								{
									pMember	= i->second;
									pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
									if( IsValidObj( pUser ) == TRUE && pUser->GetWorld() != NULL && pUser->GetWorld()->GetID() == pProp->dwWorldId )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) && pUser->IsLive() )
										{
											bsurvivor	= TRUE;
											break;
										}
									}
								}
							}

							if( !bsurvivor  )
							{
								if( pGuild )
								{
									pGuild->SetQuest( pElem->nId, pElem->nf );
									g_dpDBClient.SendUpdateGuildQuest( pGuild->m_idGuild, pElem->nId, pElem->nf );
									pGuild->ReplaceLodestar( rect );
								}

								CMover* pWormon	= prj.GetMover( pElem->objidWormon );
								if( pWormon )
									pWormon->Delete();
								RemoveGuildQuest( pElem->nId );
							}
						}
						break;
					case GQP_GETITEM:
						{
							FLTRACE_LOG( PROGRAM_NAME, _T( "GQP_GETITEM - p" ) );
							CRect rect;
							rect.SetRect( pProp->x1, pProp->y2, pProp->x2, pProp->y1 );
							BOOL bexist	= FALSE;	// is member here?

							if( pGuild )
							{
								FLWSUser* pUser;
								CGuildMember* pMember;
								for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
								{
									pMember	= i->second;
									pUser	= (FLWSUser*)prj.GetUserByID( pMember->m_idPlayer );
									if( IsValidObj( pUser ) == TRUE && pUser->GetWorld() != NULL && pUser->GetWorld()->GetID() == pProp->dwWorldId )
									{
										POINT point = { (int)pUser->GetPos().x, (int)pUser->GetPos().z	};
										if( rect.PtInRect( point ) )
										{
											bexist	= TRUE;
											break;
										}
									}
								}
							}

							if( !bexist )
							{
								if( pGuild )
								{
									pGuild->ReplaceLodestar( rect );
								}
								RemoveGuildQuest( pElem->nId );
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

void CGuildQuestProcessor::SetGuildQuest( int nQuestId, int nState, int ns, int nf, u_long idGuild, OBJID objidWormon )
{
	if( nQuestId >= MAX_GUILD_QUEST )
	{
		return;
	}
	GUILDQUESTPROP* pProp	= prj.GetGuildQuestProp( nQuestId );
	if( !pProp )
	{
		return;
	}

	FLTRACE_LOG( PROGRAM_NAME, _T( "SET_GUILD_QUEST, %d, %d, %d" ), nQuestId, idGuild, objidWormon );
	PGUILDQUESTELEM	pElem	= &m_pElem[nQuestId];
	pElem->nId	= nQuestId;
	pElem->nState	= nState;
	pElem->idGuild	= idGuild;
//	pElem->dwEndTime	= GetTickCount() + pProp->dwTime;
//	pElem->nProcess		=  nProcess;
//#ifdef __INTERNALSERVER
//	pElem->dwEndTime	= GetTickCount() + MIN( 10 );
//#else	// __INTERNALSERVER
	pElem->dwEndTime	= GetTickCount() + MIN( 60 );
//#endif	// __INTERNALSERVER
	
	pElem->nProcess		= GQP_WORMON;
	pElem->ns	= static_cast< BYTE >( ns );
	pElem->nf	= static_cast< BYTE >( nf );
	pElem->objidWormon	= objidWormon;
	pElem->nCount	= 0;
}

PGUILDQUESTELEM CGuildQuestProcessor::GetGuildQuest( int nQuestId )
{
	if( nQuestId >= MAX_GUILD_QUEST )
	{
		return NULL;
	}
	if( m_pElem[nQuestId].nId == -1 )
		return NULL;

	return &m_pElem[nQuestId];
}

void CGuildQuestProcessor::RemoveGuildQuest( int nQuestId )
{
	if( nQuestId >= MAX_GUILD_QUEST )
	{
		return;
	}

	FLTRACE_LOG( PROGRAM_NAME, _T( "REMOVE_GUILD_QUEST, %d" ), nQuestId );

	PGUILDQUESTELEM pElem	= &m_pElem[nQuestId];
	pElem->nId		= -1;
	pElem->nState	= 0;
	pElem->idGuild		= 0;
	pElem->nProcess	= GQP_READY;
	pElem->dwEndTime	= 0;
	pElem->nCount	= 0;
}

BOOL CGuildQuestProcessor::IsQuesting( int nQuestId )
{
	if( nQuestId >= MAX_GUILD_QUEST )
	{
		return FALSE;
	}

	if( m_pElem[nQuestId].nId != -1 )
		return TRUE;
	return FALSE;
}

CGuildQuestProcessor* CGuildQuestProcessor::GetInstance( void )
{
	static CGuildQuestProcessor sGuildQuestProcessor;
	return &sGuildQuestProcessor;
}

void CGuildQuestProcessor::AddQuestRect( int nId, int x1, int y1, int x2, int y2 )
{
	if( m_nRect >= MAX_GUILD_QUEST )
	{
		return;
	}

	m_pRect[m_nRect].nId	= nId;
	m_pRect[m_nRect].rect.SetRect( x1, y2, x2, y1 );
	m_nRect++;
}

int CGuildQuestProcessor::PtInQuestRect( const D3DXVECTOR3 & vPos )
{
	POINT point	= { (int)vPos.x, (int)vPos.z	};
	for( int i = 0; i < m_nRect; i++ )
	{
		
		if( m_pRect[i].rect.PtInRect( point ) )
			return m_pRect[i].nId;
	}
	return -1;
}

CRect* CGuildQuestProcessor::GetQuestRect( int nId )
{
	for( int i = 0; i < m_nRect; i++ )
	{
		if( m_pRect[i].nId == nId )
			return &m_pRect[i].rect;
	}
	return NULL;
}
#endif	// __WORLDSERVER
