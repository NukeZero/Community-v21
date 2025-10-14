#include "stdafx.h"
#include "dpsrvr.h"
#include "user.h"
#include <worldmng.h>
#include <misc.h>

#include <guild.h>
extern	CGuildMng	g_GuildMng;


#include "..\_Common\Ship.h"


#include "..\_aiinterface\AIPet.h"

#include <Party.h>
extern	CPartyMng			g_PartyMng;
extern	CWorldMng			g_WorldMng;

#include "..\_Network\ErrorCode.h"


void CDPSrvr::OnUseSkill( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	WORD wType		= 0;			// 직업스킬이냐, 라이선스스킬이냐 구분하는 변수 - 2005.10.04 의미없음 
	WORD wId		= 0;
	OBJID objid		= 0;
	int	 nUseType	= 0;
	BOOL bControl	= FALSE;

	ar >> wType >> wId >> objid >> nUseType >> bControl;
	wType = 0;


	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( (CObj*)pUser ) == FALSE )
	{
		return;
	}

	if( pUser->m_vtInfo.VendorIsVendor() )
	{
		pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
		return;
	}

	if( pUser->m_bAllAction == FALSE )
	{
		pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
		return;
	}

	const int nIdx = wId;
	if( nIdx < 0 || nIdx >= MAX_SKILL_JOB )
	{
		pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
		return;
	}

	if( _GetContentState( CT_NEWUI_19 ) == CS_VER1 )	// 18차 이하..
	{
		const DWORD dwResult	= pUser->DoUseSkill( wType, nIdx, objid, (SKILLUSETYPE)nUseType, bControl );
		if( dwResult == FSC_SKILLUSE_SUCCESS )	// 스킬사용에 성공했고
		{
			if( nUseType == SUT_QUEUESTART )	// 스킬큐로 실행하라고 한거였다.
			{
				pUser->m_playTaskBar.m_nUsedSkillQueue = SKILL_QUEUE_USE;		// 스킬큐 실행중인 표시 남김.
			}
		}
		else
		{
			pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
			return;
		}
	}
	else		// 19차 이상..
	{
		/*
		액션슬롯으로 스킬 사용 시 첫 스킬이 사용 실패가 되면 액션 슬롯 사용 자체가 안되므로
		첫 스킬이 쿨타임 때문에 사용 불가능일땐 액션 슬롯을 사용하였다고 판단하고 쿨타임 남은 스킬들은 스킵을 한다.
		모든 스킬이 쿨타임이 남아있을시엔 사용 불가..
		*/
		if( nUseType == SUT_QUEUESTART ) {
			if( pUser->m_playTaskBar.CanUseSkillQueue( pUser ) == false || pUser->m_playTaskBar.ExistCanUseSkillByActionSlot( pUser ) == false ) {
				pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
				return;
			}

			const DWORD dwResult = pUser->DoUseSkill( wType, nIdx, objid, (SKILLUSETYPE)nUseType, bControl );

			if( dwResult == FSC_SKILLUSE_SUCCESS ) {
				pUser->m_playTaskBar.m_nUsedSkillQueue = SKILL_QUEUE_USE;		// 스킬큐 실행중인 표시 남김.
				pUser->m_playTaskBar.SetUseSkillQueue();
			}
			else if( dwResult == FSC_SKILLUSE_SKIP ) {
				pUser->m_playTaskBar.m_nUsedSkillQueue = SKILL_QUEUE_USE;		// 스킬큐 실행중인 표시 남김.
				pUser->AddActionSlotSkillSkip_HardCording( SUT_QUEUESTART );
				if( pUser->m_playTaskBar.SetNextSkill( pUser ) == true ) {
					pUser->m_playTaskBar.SetUseSkillQueue();
				}
				else {
					pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_USE_ACTION_SLOT_NOTI );
				}
			}
			else {
				pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
				return;
			}
		}
		else {
			const DWORD dwResult	= pUser->DoUseSkill( wType, nIdx, objid, (SKILLUSETYPE)nUseType, bControl );
			if( dwResult != FSC_SKILLUSE_SUCCESS ) {
				pUser->AddHdr( GETID( pUser ), SNAPSHOTTYPE_CLEAR_USESKILL );
				return;
			}
		}
	}
}
/*
void CDPSrvr::OnDoCollect( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize )
{
}
*/
// 클라이언트로 부터 받은 에러코드를 로그로 남긴다.
void CDPSrvr::OnError( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/)
{
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		int		nCode, nData;
		ar >> nCode >> nData;
		switch( nCode )
		{
		case FE_GENERAL:
			break;
		case FE_INVALIDATTACKER:	// 클라이언트에서 어태커가 invalid한경우(유령몹 버그)
			{
#ifndef _DEBUG
				OBJID idAttacker = (OBJID)nData;
				CMover *pAttacker = prj.GetMover( idAttacker );
				if( IsValidObj(pAttacker) )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "FE_INVALIDATTACKER 피해자:%s(%f,%f,%f), 공격자:%s(%f,%f,%f)" ), pUser->GetName(), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z,
						pAttacker->GetName(), pAttacker->GetPos().x, pAttacker->GetPos().y, pAttacker->GetPos().z );
					pUser->AddCorrReq( pAttacker );	// 요청한 클라에게 인밸리드한 어태커를 다시 보내줌.
				}
				else
					FLERROR_LOG( PROGRAM_NAME, _T( "FE_INVALIDATTACKER 피해자:%s(%f,%f,%f) 이런젠장 서버에서도 pAttacker는 Invalid다. 0x%08x" ),
					pUser->GetName(), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z,
					nData );
				
#endif // not debug
				//		case NEXT:
				
			}
			break;
		}
	} else
		FLERROR_LOG( PROGRAM_NAME, _T( "pUser - Invalid %d %d" ), dpidCache, dpidUser );
}

void CDPSrvr::OnShipActMsg( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	DWORD dwMsg;
	OBJID idShip;
	int nParam1, nParam2;
	ar >> dwMsg >> nParam1 >> nParam2 >> idShip;
	
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		if( IsInvalidObj( pUser->GetIAObjLink() ) )		return;
		if( idShip != pUser->GetIAObjLink()->GetId() )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "클라에서 보내온 아이디(%d)와 서버에서의 아이디(%d)가 다르다" ), idShip, pUser->GetIAObjLink()->GetId() );
			return;
		}
		
		CShip *pShip = prj.GetShip( idShip );
		if( IsValidObj( pShip ) )
		{
			pShip->SendActMsg( (OBJMSG)dwMsg, nParam1, nParam2, 0 );
			//			g_DPCoreClient.SendShipActMsg( pUser, dwMsg, nParam1, nParam2 );
			g_xWSUserManager->AddShipActMsg( pUser, pShip, dwMsg, nParam1, nParam2 );
		}
	}
}

void CDPSrvr::OnLocalPosFromIA( CAr & ar, DPID dpidCache, DPID dpidUser, LPBYTE /*lpBuf*/, u_long /*uBufSize*/ )
{
	D3DXVECTOR3 vLocal;
	OBJID		idIA;
	ar >> vLocal >> idIA;
	
	FLWSUser* pUser	= g_xWSUserManager->GetUser( dpidCache, dpidUser );
	if( IsValidObj( pUser ) )
	{
		// 클라이언트로부터 착지한지점의 상대좌표를 받았다.
		// 이 좌표를 서버에서 동기화 하자
		CShip *pIA = prj.GetShip( idIA );
		if( IsInvalidObj( pIA ) )	return;

		
		D3DXVECTOR3 vPos = pIA->GetPos() + vLocal;		// 서버상에서의 IA오브젝트와 클라에서 받은 로컬좌표를 합쳐서 새로운 좌표생성
		pUser->SetPos( vPos );
		pUser->SetIAObjLink( pIA );
	}
}