#include "stdafx.h"
#include "mover.h"

BOOL CMover::HasBuff( WORD wType, WORD wId )
{
	return m_buffs.HasBuff( wType, wId );
}

void CMover::RemoveBuff( WORD wType, WORD wId )
{
	m_buffs.RemoveBuff( wType, wId );
}

BOOL CMover::HasBuffByIk3( DWORD dwIk3 )
{
	return m_buffs.HasBuffByIk3( dwIk3 );
}

//////////////////////////////////////////////////////////////////////////
// mirchang_101101
bool	CMover::HasUseLimitGroupItemBuff( int nUseLimitGroup )
{
	return m_buffs.HasUseLimitGroupItemBuff( nUseLimitGroup );
}

int		CMover::GetCountBuffByItemGroup( int nItemGroup )
{
	return m_buffs.GetCountBuffByItemGroup( nItemGroup );
}

int		CMover::GetCountBuffByItemId( DWORD dwId )
{
	return m_buffs.GetCountBuffByItemId( dwId );
}

int		CMover::GetAdjValueDuplicationBuff( DWORD dwItemKind3 )
{
	return m_buffs.GetAdjValueDuplicationBuff( dwItemKind3 );
}

int		CMover::GetRemainCountBuff( const DWORD dwItemKind3 )
{
	return m_buffs.GetRemainCountBuff( dwItemKind3 );
}

IBuff*	CMover::GetBuffByIk3( DWORD dwItemKind3 )
{
	return m_buffs.GetBuffByIk3( dwItemKind3 );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void	CMover::DoApplyHitActiveSkill( CMover* pDest, const DWORD dwProb )
{
	if( HasBuffByIk3( IK3_USESKILL ) == TRUE && IsValidObj( pDest ) == TRUE )
	{
		m_buffs.DoApplyHitActiveSkill( pDest, dwProb );
	}
}

void	CMover::DoApplyDamageActiveSkill( CMover* pDest, const DWORD dwProb )
{
	if( HasBuffByIk3( IK3_USESKILL ) == TRUE && IsValidObj( pDest ) == TRUE )
	{
		m_buffs.DoApplyDamageActiveSkill( pDest, dwProb );
	}
}
//////////////////////////////////////////////////////////////////////////


#ifdef __WORLDSERVER
BOOL CMover::HasPet()
{
	return m_buffs.HasPet();
}

void CMover::RemovePet()
{
	m_buffs.RemoveBuffPet();
}
#endif	// __WORLDSERVER

DWORD CMover::GetDisguise()
{
	return m_buffs.GetDisguise();
}

BOOL CMover::AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTime, DWORD tmPain, BYTE byBuffTickType, OBJID oiAttacker )
{
	BOOL bOK = m_buffs.AddBuff( wType, wId, dwLevel, tmTime, tmPain, byBuffTickType, oiAttacker );

#ifdef __CLIENT
	if( this == g_pPlayer )
		g_WndMng.UpdateContentsByBuff( wType, wId );
#endif

	return bOK;
}

void CMover::RemoveCommonBuffs()
{
	m_buffs.RemoveBuffs( RBF_COMMON );
}

#ifdef __CLIENT
void CMover::ClearBuffInst()
{
	m_buffs.ClearInst();
}
#endif	// __CLIENT

void CMover::RemoveChrStateBuffs( DWORD dwChrState )
{
	m_buffs.RemoveBuffs( RBF_CHRSTATE, dwChrState );
}

void CMover::RemoveIk3Buffs( DWORD dwIk3 )
{
	m_buffs.RemoveBuffs( RBF_IK3, dwIk3 );
}

void CMover::RemoveAllBuff()
{
	m_buffs.RemoveBuffs( RBF_UNCONDITIONAL );
}

void CMover::ProcessBuff()
{
	m_buffs.Process();
}

void CMover::RemoveAttackerBuffs( OBJID oiAttacker )
{
	m_buffs.RemoveBuffs( RBF_ATTACKER, oiAttacker );
}

void CMover::RemoveDstParamBuffs( DWORD dwDstParam )
{
	m_buffs.RemoveBuffs( RBF_DSTPARAM, dwDstParam );
}

void CMover::RemoveDebufBuffs()
{
	m_buffs.RemoveBuffs( RBF_DEBUF );
}