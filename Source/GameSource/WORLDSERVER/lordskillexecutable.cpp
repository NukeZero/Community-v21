#include "stdafx.h"

#include "lordskillexecutable.h"
#include <lord.h>
#include "user.h"
#include "slord.h"

void CLordSkillComponentItemBuf::Execute( u_long idPlayer, u_long idTarget, VOID* /*pParam*/ )
{
	Use();
	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( IsValidObj( pUser ) )
	{
		FLWSUser* pTarget	= pUser;
		if( GetTargetType() != SKILL_INDEX( 1, LTT_SELF ) )
			pTarget		= g_xWSUserManager->GetUserByPlayerID( idTarget );
		if( IsValidObj( pTarget ) && pTarget->GetType() == OT_MOVER && pTarget->IsPlayer() )
		{
			PT_ITEM_SPEC pItemProp		= g_xSpecManager->GetSpecItem( GetItem() );
			if( pItemProp )
			{
				// #motion
				g_xApplyItemEffect->DoApplyEffect( pUser, pTarget, pItemProp );
				// #print
				if( GetId() == SKILL_INDEX( 4, LI_RAGE ) )
				{
					pUser->AddDefinedText( TID_GAME_LORD_SKILL_RAGE_S001, "%s", pTarget->GetName() );
					pTarget->AddDefinedText( TID_GAME_LORD_SKILL_RAGE_S002 );
				}
				PutSkillLog( pTarget );
			}
		}
	}
}

void CLordSkillComponentSummon::Execute( u_long idPlayer, u_long idTarget, VOID* /*pParam*/ )
{
	Use();
	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	FLWSUser* pTarget	= g_xWSUserManager->GetUserByPlayerID( idTarget );
	if( IsValidObj( pTarget ) && IsValidObj( pUser ) && pUser->GetWorld() )
	{
		pTarget->AddDefinedText( TID_GAME_LORD_SKILL_SUMMON_S001 );
		pTarget->REPLACE( g_uIdofMulti, pUser->GetWorld()->GetID(), pUser->GetPos(), REPLACE_NORMAL, pUser->GetLayer() );
		PutSkillLog( pTarget );
	}
}

int CLordSkillComponentSummon::IsExecutable( FLWSUser* pUser, const char* szTarget, u_long &idTarget )
{
	int nText	= CLordSkillComponentExecutable::IsExecutable( pUser, szTarget, idTarget );
	return( nText > 0? nText: IsTeleportable( pUser, g_xWSUserManager->GetUserByPlayerID( idTarget ) ) );
}

void CLordSkillComponentTeleport::Execute( u_long idPlayer, u_long idTarget, VOID* /*pParam*/ )
{
	Use();
	FLWSUser* pUser	= g_xWSUserManager->GetUserByPlayerID( idPlayer );
	FLWSUser* pTarget	= g_xWSUserManager->GetUserByPlayerID( idTarget );
	if( IsValidObj( pUser ) && IsValidObj( pTarget ) && pTarget->GetWorld() )
	{
		pUser->REPLACE( g_uIdofMulti, pTarget->GetWorld()->GetID(), pTarget->GetPos(), REPLACE_NORMAL, pTarget->GetLayer() );
		PutSkillLog( pTarget );
	}
}

int CLordSkillComponentTeleport::IsExecutable( FLWSUser* pUser, const char* szTarget, u_long &idTarget )
{
	int nText	= CLordSkillComponentExecutable::IsExecutable( pUser, szTarget, idTarget );
	return( nText > 0? nText: IsTeleportable( pUser, g_xWSUserManager->GetUserByPlayerID( idTarget ) ) );	// 12-2 ˬ
}

namespace	lordskill
{
	int UseRequirements( FLWSUser* pUser, const char* szTarget, int nSkill, u_long & idTarget )
	{
		CLordSkill* pSkills		= CSLord::Instance()->GetSkills();
		CLordSkillComponentExecutable* pComponent	= pSkills->GetSkill( nSkill );
		if( !pComponent )
		{
			return TID_GAME_LORD_SKILL_USE_E002;
		}

		return pComponent->IsExecutable( pUser, szTarget, idTarget );
	}
};
