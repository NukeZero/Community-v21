#include "stdafx.h"
#include "DefineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"

#ifdef __CLIENT
#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif
#endif

#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
//#include "AttackArbiter.h"

extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CDPDatabaseClient g_dpDBClient;
extern	CDPSrvr g_DPSrvr;
	#ifdef __SCHOOL0719
		#include "guild.h"
		extern	CGuildMng	g_GuildMng;
	#endif	// __SCHOOL0719
#endif // __WORLDSERVER

#include "defineitem.h"
#include "party.h"
#ifdef __WORLDSERVER
#include "AttackArbiter.h"
#endif


extern	CPartyMng	g_PartyMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////////
// CActionMover
/////////////////////////////////////////////////////////////////////////////////////



DWORD CActionMover::_ProcessMsgDmg( DWORD dwMsg, CMover* pAttacker, DWORD dwAtkFlags, int nParam, BOOL bTarget )
{
#if		defined(__WORLDSERVER)
	CAttackArbiter arbiter( dwMsg, pAttacker, GetMover(), dwAtkFlags, nParam, bTarget );
	return arbiter.OnDamageMsgW();
#elif	defined(__CLIENT)
	return OnDamageMsgC( dwMsg, pAttacker, dwAtkFlags, nParam );
#else
	return 0;
#endif
}

#ifdef	__CLIENT
DWORD CActionMover::OnDamageMsgC( DWORD dwMsg, CMover* pAttacker, DWORD dwAtkFlags, int nParam )
{
#if	!defined(__ONLINE)
	return 100;
#endif
	CMover*	pMover = GetMover();
	BOOL bValid = IsValidObj( pMover ) && IsValidObj( pAttacker );

	if( !bValid || IsState( OBJSTA_DIE_ALL ) )
		return 0;

	if( IsSit() )										// 앉아있다가 맞으면 앉기해제 한다.
		ResetState( OBJSTA_MOVE_ALL );
	
	SendActMsg( OBJMSG_STAND );

	// 날때린놈에 대한 정보를 기록함.
	if( pMover->IsNPC() && pAttacker->IsPlayer() )		// 맞은놈은 NPC , 어태커가 플레이어 일때만 적용됨
	{
		pMover->m_idAttacker = pAttacker->GetId();		// 날 때린넘이 어떤놈인가를 기록함.
		pMover->m_idTargeter = pAttacker->GetId();	
	}
	pAttacker->m_idLastHitMover = pMover->GetId();		// 어태커가 마지막으로 때렸던넘이 나란걸 기록함.

#ifdef __XSKILL0425
	if( (dwAtkFlags & AF_GENERIC) )	
	{
		ItemProp* pAttackerProp = pAttacker->GetActiveHandItemProp();
		
		D3DXVECTOR3 vLocal;
	#ifdef __ACROBAT_0504
		if( pAttackerProp && pAttackerProp->dwItemKind3 == IK3_YOYO )
		{
			vLocal    = pMover->GetPos();
			vLocal.y += 1.0f;
		}
		else
	#endif //__ACROBAT_0504
		{
			AngleToVectorXZ( &vLocal, pAttacker->GetAngle(), 1.0f );
			vLocal += pAttacker->GetPos();
		}

		if( pAttackerProp && pAttackerProp->dwSfxObj3 != NULL_ID )
			CreateSfx( g_Neuz.m_pd3dDevice, pAttackerProp->dwSfxObj3, vLocal );
	}
#else
	if( (dwAtkFlags & AF_MAGICSKILL) )	
	{
		ItemProp* pAttackerProp = pAttacker->GetActiveHandItemProp();
		D3DXVECTOR3 vLocal;
		AngleToVectorXZ( &vLocal, pAttacker->GetAngle(), 1.0f );
		vLocal += pAttacker->GetPos();
		if( pAttackerProp && pAttackerProp->dwSfxObj2 != NULL_ID )
			CreateSfx( g_Neuz.m_pd3dDevice, pAttackerProp->dwSfxObj2, vLocal );
	}
#endif // not xSkill0425
	pMover->m_nDmgCnt = 10;								// 흔들리는 시간이다.

	pMover->SetDamagedMotion( pAttacker, dwAtkFlags );
	return 1;
}
#endif	// __CLIENT


