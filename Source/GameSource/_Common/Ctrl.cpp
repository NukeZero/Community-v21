// Ctrl.cpp: implementation of the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResData.h"
#ifdef __WORLDSERVER
#include "CtrlObjs.h"
#endif
#include "Ship.h"

#ifdef __CLIENT
#include "../Neuz/DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef __WORLDSERVER

#include "../WorldServer/user.h"
#include "Party.h"

extern	CPartyMng			g_PartyMng;

#endif // Worldserver

#include "commonctrl.h"

#include "FLSkillSystem.h"

extern std::map< std::string, DWORD > g_MapStrToObjId;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl::CCtrl()
{
	m_dwType = OT_CTRL;
	m_objid	 = NULL_ID;

#ifdef __WORLDSERVER
	m_lRespawn	= -1;
	m_nRespawnType = 0;

	//memset( m_nOldCenter, 0, sizeof(m_nOldCenter) );
#endif	// __WORLDSERVER

	m_pIAObjLink = NULL;
	D3DXMatrixIdentity( &m_mInvTM );
}

CCtrl::~CCtrl()
{
#ifdef __WORLDSERVER
	if( IsVirtual() )
		return;

	//RemoveItFromView();
	
#endif	// __WORLDSERVER

	RemoveItFromGlobalId();
}

void CCtrl::Process()
{
	CObj::Process();
#ifdef __WORLDSERVER
	ProcessDeleteRespawn();
#endif
}

void CCtrl::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CObj::Render( pd3dDevice );
}

BOOL CCtrl::Read( CFileIO* pFile )
{
	return CObj::Read( pFile );
}
// this가 pIA에 링크되었다.
// pIA가 NULL이면 링크해제.
void CCtrl::SetIAObjLink( CShip *pIA )
{
	if( pIA != NULL )	// IA에 링크시켜야 할때...
	{
		if( m_pIAObjLink != pIA )				// 기존에 링크랑 다른링크가 될때만...
			if( pIA->FindCtrl( GetId() ) == NULL )			// pIA에 이미 this가 등록되었는지 보고...
				pIA->AddCtrl( GetId() );			// IA오브젝트에 this를 등록시킴.
	} else
	{
		if( m_pIAObjLink )
			m_pIAObjLink->RemoveCtrl( GetId() );
	}

	m_pIAObjLink = pIA;
}

void CCtrl::AddItToGlobalId()
{
	prj.m_objmap.Add( this );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() )
	{
		bool bResult = prj.m_idPlayerToUserPtr.insert(std::make_pair( ( (CMover*)this )->m_idPlayer, (CMover*)this ) ).second;
		if( bResult == false )
			FLERROR_LOG( PROGRAM_NAME, _T( "id:%d duplicated." ), ((CMover*)this)->m_idPlayer );
	}

#ifdef __WORLDSERVER	
	if( GetType() == OT_CTRL )
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		if( pCommonCtrl->m_CtrlElem.m_strCtrlKey[0] != '\0' )
			g_MapStrToObjId.insert( std::map< string, DWORD >::value_type(pCommonCtrl->m_CtrlElem.m_strCtrlKey, pCommonCtrl->GetId() ) );
	}
#ifdef __AGGRO16
	FLAggro * pAggro			= static_cast< CMover * >( this	)->GetAggroMng();
	if( pAggro != NULL )
		pAggro->Initialize( GetId() );
#endif //__AGGRO16

	if( GetWorld() != NULL )
	{
		
	}

#endif // __WORLDSERVER
}

void CCtrl::RemoveItFromGlobalId()
{
	if( m_objid == NULL_ID )	
	{
// 		for( std::map<DWORD, CCtrl*>::iterator it = prj.m_objmap.m_map.begin(); it != prj.m_objmap.m_map.end(); ++it )
// 		{
// 			if( it->second == this )
// 				return;
// 		}

		return;
	}

	prj.m_objmap.RemoveKey( m_objid );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() && ( (CMover*)this )->m_idPlayer != (u_long)0xffffffff )
		prj.m_idPlayerToUserPtr.erase( ( (CMover*)this )->m_idPlayer );

#ifdef __WORLDSERVER	
	if( GetType() == OT_CTRL )
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		if( pCommonCtrl->m_CtrlElem.m_strCtrlKey[0] != '\0' )
			g_MapStrToObjId.erase( pCommonCtrl->m_CtrlElem.m_strCtrlKey );
	}
#endif // __WORLDSERVER
}

#ifdef __WORLDSERVER

BOOL CCtrl::ProcessDeleteRespawn()
{
	// 리스폰이 Remove이면 삭제 명령 
#ifdef __LAYER_1021
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo( GetRespawn(), m_nRespawnType, GetLayer() );
#else	// __LAYER_1021
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo( GetRespawn(), m_nRespawnType );
#endif	// __LAYER_1021
	if( pRespawnInfo && pRespawnInfo->m_bRemove )
	{
		Delete();
		return TRUE;
	}
	return FALSE;
}

//void CCtrl::RemoveItFromView( BOOL bRemoveall )
//{
//	if( !GetWorld() )	
//		return;
//
//	FLWSUser* pUser;
//	if( GetType() == OT_MOVER && ( (CMover*)this )->IsPlayer() )
//	{
//		std::map<DWORD, FLWSUser* >::iterator it = m_2pc.begin();
//		while( it != m_2pc.end() )
//		{
//			pUser = it->second;
//			if( pUser != this && pUser->PCRemoveKey( GetId() ) )
//			{
//				//		pUser->AddRemoveObj( GetId() );
//			}
//		
//			++it;
//		}
//
//		if( bRemoveall )
//			m_2pc.clear();
//	}
//	else
//	{
//		std::map<DWORD, FLWSUser* >::iterator it = m_2pc.begin();
//		while( it != m_2pc.end() )
//		{
//			pUser = it->second;
//			if( pUser->NPCRemoveKey( GetId() ) )
//			{
//		//		pUser->AddRemoveObj( GetId() );
//			}
//			++it;
//		}
//
//		if( bRemoveall )
//			m_2pc.clear();
//	}
//
//}

BOOL CCtrl::IsNearPC( FLWSUser* pUser )
{
	const float fRange		= 90.0f;		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	const D3DXVECTOR3 vDistant = GetPos() - pUser->GetPos();
	D3DXVECTOR3 vDist = vDistant;
	vDist.y	= 0.0f;
	const float fDistSq = D3DXVec3LengthSq( &vDist );	
	if( fDistSq > fRange * fRange )	
		return FALSE;
	return TRUE;
}
//
//BOOL CCtrl::IsNearPC( OBJID objid )
//{
//	const float fRange		= 90.0f;		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//	FLWSUser * pUser		= prj.Get
//
//	const D3DXVECTOR3 vDistant = GetPos() - pUser;
//	D3DXVECTOR3 vDist = vDistant;
//	vDist.y	= 0.0f;
//	const float fDistSq = D3DXVec3LengthSq( &vDist );	
//	if( fDistSq > fRange * fRange )	
//		return FALSE;
//	return TRUE;
//
//	//std::map<DWORD, FLWSUser* >::iterator it = m_2pc.find( objid );
//	//return ( it != m_2pc.end() ) ;
//}

#endif	// __WORLDSERVER

BOOL	CCtrl::GetSkillProp( SkillProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	SkillProp* pSkillProp = prj.GetSkillProp( nSkill );	// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
	if( pSkillProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s : %d. 스킬(%d)의 프로퍼티가 없다." ), szErr, GetIndex(), nSkill );
		return FALSE;
	}
	
	
	AddSkillProp *pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	if( pAddSkillProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s : %d. Add스킬(%d) Lv%d의 프로퍼티가 없다." ), szErr, GetIndex(), nSkill, dwLevel );
		return FALSE;
	}

	if( pAddSkillProp->dwSkillPropID != pSkillProp->dwID )
		FLERROR_LOG( PROGRAM_NAME, _T( "%s : %d. Add스킬(%d) AddSkill->dwName과 Skill->dwID가 다르다." ), szErr, GetIndex(), nSkill );

	*ppSkillProp = pSkillProp;
	*ppAddSkillProp = pAddSkillProp;	
	return TRUE;
}




// 범위적용시 타겟의 종류에 따라 주변 타겟의 종류를 선택함.
int		CCtrl::TargetSelecter( CCtrl *pTarget )
{
	int nApplyType = 0;
	CCtrl *pSrc = this;

	if( pTarget->GetType() == OT_MOVER )	// 타겟이 무버일때
	{
		nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;	// 주위 플레이어들/몬스터에게 적용
	}
	else
	{
		// 타겟이 컨트롤일땐
		if( pSrc->GetType() == OT_MOVER )	// 공격자가 무버냐
		{
			if( ((CMover *)pSrc)->IsPlayer() )		// 공격자가 플레이어면
				nApplyType = OBJTYPE_PLAYER | OBJTYPE_CTRL | OBJTYPE_MONSTER;	// 주위 몬스터와 컨트롤에게 적용.
		}
		else
		{	// 공격자가 컨트롤이냐
			nApplyType = OBJTYPE_PLAYER;	// 플레이어에게 적용
		}
	}

	return nApplyType;
}



