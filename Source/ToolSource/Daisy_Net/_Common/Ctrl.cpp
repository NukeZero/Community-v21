// Ctrl.cpp: implementation of the CCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "defineText.h"
#include "ResData.h"
#ifdef __WORLDSERVER
#include "CtrlObjs.h"
#endif
#include "Ship.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif  

#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
#include "dpsrvr.h"

#ifdef __WORLDSERVER
 extern	CUserMng	g_UserMng;
 extern	CDPCoreClient	g_DPCoreClient;
 extern	CDPSrvr		g_DPSrvr;
 #include "Party.h"
 extern	CPartyMng	g_PartyMng;
 #include "dpDatabaseClient.h"
 extern	CDPDatabaseClient	g_dpDBClient;
#endif // Worldserver

WORD	CCtrl::m_nPrjsCtrlID	= 0;
#endif

extern CMapStrToObjId g_MapStrToObjId;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrl::CCtrl()
{
	m_dwType = OT_CTRL;
	m_objid		= NULL_ID;

#if !defined(__BEAST) && !defined(__ONLINE)
	m_lRespawn	= -1;
#ifdef __V050322_CACHEQUEST
	m_nRespawnType = 0;
#endif
#endif

#ifdef __WORLDSERVER
	m_2pc.SetSize( 16, 128, 16 );
	m_nOldAttr	= 0;
	memset( m_nOldCenter, 0, sizeof(int) * MAX_LINKLEVEL );
#endif	// __WORLDSERVER
	m_pIAObjLink = NULL;
	D3DXMatrixIdentity( &m_mInvTM );
}

CCtrl::~CCtrl()
{
#ifdef __WORLDSERVER
	if( IsVirtual() )
		return;

	RemoveItFromView();
#endif	// __WORLDSERVER

	RemoveItFromGlobalId();
}

void CCtrl::Process( FLOAT fElapsedTime )
{
	CObj::Process( fElapsedTime );
#ifdef __V050426_REMOVERESPAWN  
	ProcessDeleteRespawn();
#endif
}
#ifdef __V050426_REMOVERESPAWN  
BOOL CCtrl::ProcessDeleteRespawn()
{
	// 리스폰이 Remove이면 삭제 명령 
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo( GetRespawn(), m_nRespawnType );
	if( pRespawnInfo && pRespawnInfo->m_bRemove )
	{
		Delete();
		return TRUE;
	}
	return FALSE;
}
#endif // __V050426_REMOVERESPAWN

void CCtrl::Execute()
{

}

void CCtrl::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	CObj::Render( pd3dDevice );
}

BOOL CCtrl::Read( CFileIO* pFile )
{
	return CObj::Read( pFile );
}

BOOL CCtrl::Write( CFileIO* pFile )
{
	return CObj::Write( pFile );
}

// this가 pIA에 링크되었다.
// pIA가 NULL이면 링크해제.
void CCtrl::SetIAObjLink( CShip *pIA )
{
#ifndef __BEAST
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
#endif
}

void CCtrl::AddItToGlobalId( void )
{
	prj.m_objmap.Add( this );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() )
	{
		bool bResult = prj.m_idPlayerToUserPtr.insert( map<u_long, CMover*>::value_type( ( (CMover*)this )->m_idPlayer, (CMover*)this ) ).second;
		if( bResult == false )
		{
			Error("AddItToGlobalId id:%d duplicated.", ((CMover*)this)->m_idPlayer );
		}
	}

	if( GetType() == OT_CTRL )
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		
		if( _tcslen( pCommonCtrl->m_CtrlElem.m_strCtrlKey ) != NULL )
			g_MapStrToObjId.insert( MapStrToObjIdType(pCommonCtrl->m_CtrlElem.m_strCtrlKey, pCommonCtrl->GetId() ) );
	}
}

void CCtrl::RemoveItFromGlobalId( void )
{
	if( m_objid == NULL_ID )	return;
	prj.m_objmap.RemoveKey( m_objid );

	if( OT_MOVER == GetType() && ( (CMover*)this )->IsPlayer() && ( (CMover*)this )->m_idPlayer != (u_long)0xffffffff )
		prj.m_idPlayerToUserPtr.erase( ( (CMover*)this )->m_idPlayer );
}

#ifdef __WORLDSERVER
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void CCtrl::RemoveItFromView( BOOL bRemoveall )
{
	if( !GetWorld() )	return;

	CCtrl* pCtrl;
	CUser* pUser;
	CCtrlBucket* pBucket;

	if( GetType() == OT_MOVER && ( (CMover*)this )->IsPlayer() )
	{
		m_2pc.Lock();
		pBucket		= m_2pc.GetFirstActive();
		while( pBucket )
		{
			pUser	= (CUser*)pBucket->m_value;	// this user is valid, at  this point of time
			if( pUser != this && pUser->PCRemoveKey( GetId() ) )
				pUser->AddRemoveObj( GetId() );
			pBucket		= pBucket->pNext;
		}
		if( bRemoveall )
			m_2pc.RemoveAll();
		m_2pc.Unlock();
	}
	else
	{
		m_2pc.Lock();
		pBucket		= m_2pc.GetFirstActive();
		while( pBucket )
		{
			pUser	= (CUser*)pBucket->m_value;	// this user is valid, at  this point of time
			if( pUser->NPCRemoveKey( GetId() ) )
				pUser->AddRemoveObj( GetId() );

			pBucket		= pBucket->pNext;
		}
		if( bRemoveall )
			m_2pc.RemoveAll();
		m_2pc.Unlock();
	}
}

void CCtrl::SetPosAttr( void )
{
	chASSERT( m_dwType == OT_SHIP || m_dwType == OT_MOVER );
	chASSERT( GetWorld() );

	GetWorld()->ModifyView( this );
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#endif	// WorldServer

#ifdef __WORLDSERVER

BOOL CCtrl::IsNearPC( CUser* pUser )
{
	return IsNearPC( pUser->GetId() );
}

BOOL CCtrl::IsNearPC( OBJID objid )
{
	CCtrl* pUsertmp;
	CMclAutoLock	Lock( m_2pc.m_AddRemoveLock );
	return m_2pc.Lookup( objid, pUsertmp );
}

#endif	// __WORLDSERVER

BOOL	CCtrl::GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr )
{
	ItemProp* pSkillProp = prj.GetSkillProp( nSkill );	// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
	if( pSkillProp == NULL )
	{
		Error( "%s : %d. 스킬(%d)의 프로퍼티가 없다.", szErr, GetIndex(), nSkill );
		return FALSE;
	}
	
	
	AddSkillProp *pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "%s : %d. Add스킬(%d) Lv%d의 프로퍼티가 없다.", szErr, GetIndex(), nSkill, dwLevel );
		return FALSE;
	}
#ifdef __XSKILL0425
	if( pAddSkillProp->dwName != pSkillProp->dwID )
		Message( "%s : %d. Add스킬(%d) AddSkill->dwName과 Skill->dwID가 다르다.", szErr, GetIndex(), nSkill );
#endif
	
	*ppSkillProp = pSkillProp;
	*ppAddSkillProp = pAddSkillProp;
	
	return TRUE;
}


// vPos를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
// pCenter : 광역 지속스킬의 타겟. NULL일수도 있다
void	CCtrl::ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, CCtrl* pCenter )
{
#ifndef __NPP_050308						
	UNUSED_ALWAYS( pCenter );
#endif // __NPP_050308						

#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	float fDistSq;
	CObj* pObj;
	CCtrl *pSrc = this;
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		Message( "CCtrl::ApplySkillRegion : pSkillProp == NULL A:%d-%d", pSrc->GetType(), pSrc->GetIndex() );

	FLOAT fRange = 4.0f;
	if( pAddSkillProp )										// 애드스킬이 있으면 거기서 꺼냄
	{
		fRange = (float)pAddSkillProp->dwSkillRange;
		if( pAddSkillProp->dwSkillRange == NULL_ID )
			Message( "경고 : 스킬 %d의 dwSkillRange가 지정되지 않음", pAddSkillProp->dwID );
	}
	else
	{
		fRange = (float)pSkillProp->_dwSkillRange;			// 없으면 스킬프로퍼티에서 꺼냄.
		if( pSkillProp->_dwSkillRange == NULL_ID )
			Message( "경고 : 스킬 %d의 dwSkillRange가 지정되지 않음", pSkillProp->dwID );
	}

	if( fRange <= 4.0f )		nRange = 4;
	else if( fRange <= 8.0f )	nRange = 8;
	else if( fRange <= 16.0f )	nRange = 16;
	else						nRange = 32;

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Message( "CCtrl::ApplySkillRegion : D:%d A:%d-%d %d %f", GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange );

	BOOL	bApply = FALSE;
	BOOL	bTarget = TRUE;

	//------------ 적용대상이 플레이어인가 
	if( nApplyType & OBJTYPE_PLAYER )	
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer )
		{
			bApply = FALSE;	// 일단 FALSE로 초기화

			if( pObj->GetType() != OT_MOVER )
				Message( "ApplySkillRegion : pObj가 MOVER가 아니다 %d", pObj->GetType() );

			if( pSrc->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
#ifdef __NPP_050308
					bApply = TRUE;
#else	// __NPP_050308
				if( pAttacker->m_nDuel == 2 )	// 공격자가 파티듀얼중일땐 
					{
						if( ((CMover*)pObj)->m_idparty == pAttacker->m_idDuelParty )	// 상대 파티원에게만 데미지
							bApply = TRUE;
					} else
					if( pAttacker->m_nDuel == 1 ) // 공격자가 그냥 듀얼중일땐.
					{
						if( ((CMover*)pObj)->GetId() == pAttacker->m_idDuelOther )	// 듀얼상대에게만
							bApply = TRUE;
					}
#endif	// __NPP_050308
				} else
				{	// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			} else
			{	// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply )
			{
				if( pObj != pSrc )		// 어태커는 검색대상에서 제외.
				{
					if( pObj->IsRangeObj( vPos, fRange ) )	// vCenter + fRange와 pObj의 스피어가 충돌했느냐
					{
						CMover *pTarget = (CMover *)pObj;		// <- 여긴 무조건 pObj는 플레이어므로 해도됨.
#ifdef __NPP_050308						
						if( pCenter )
							bTarget = (pCenter == pTarget);
						else
							bTarget = FALSE;
#endif //__NPP_050308
						if( IsValidObj( pTarget ) && pTarget->IsLive() )
						{
							pTarget->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg, bTarget );		// 대상에게 효과를 적용함.
						}
					}
				}
				
				bApply = FALSE;	// 다음 루프를 위해서 초기화.
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER

	// 적용대상이 몬스터/컨트롤 인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic )	// linkDynamic을 쓴다
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
			} else
			// 대상이 컨트롤이면.
			if( pObj->GetType() == OT_CTRL )
			{
				if( pSrc->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
					bApply = TRUE;							
			}

			if( bApply )
			{
				if( pObj != pSrc )		// 공격자는 검사대상에서 제외.
				{
					if( pObj->IsRangeObj( vPos, fRange ) )	// vCenter + fRange와 pObj의 스피어가 충돌했느냐
					{
						if( pObj->GetType() == OT_MOVER )	// 타겟이 무버냐?
						{
							if( IsValidObj( pObj ) && ((CMover *)pObj)->IsLive() )		// 타겟이 무버면 살아있는지 검사.
								((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
						} else
						{
							// 타겟이 무버가 아니냐?
							((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // ApplySkillRegion()


// this를 중심으로 주변에 효과를 적용한다.
// ------------------------------------------
// 트랩이 쓸수도 있고. 몬스터가쓸수도있다(뱅), 주인공이 스킬로 사용할수도 있다.
// 효과는 반드시 SkillProp을 만들어서 그 아이디를 넘겨야 한다.
//
// 경우에따라 pAddSkillProp은 NULL일수도 있다.  아이템에서 사용할수도 있기때문. 아이템은 애드스킬프롭이 없다.
void	CCtrl::ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	float fDistSq;
	CObj* pObj;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		Message( "CCtrl::ApplySkillAround : pSkillProp == NULL D:%d A:%d-%d", GetIndex(), pSrc->GetType(), pSrc->GetIndex() );

	FLOAT fRange = 4.0f;
	if( pAddSkillProp )										// 애드스킬이 있으면 거기서 꺼냄
		fRange = (float)pAddSkillProp->dwSkillRange;
	else
		fRange = (float)pSkillProp->_dwSkillRange;			// 없으면 스킬프로퍼티에서 꺼냄.

	if( fRange <= 4.0f )		nRange = 4;
	else if( fRange <= 8.0f )	nRange = 8;
	else if( fRange <= 16.0f )	nRange = 16;
	else						nRange = 32;

	FLOAT fTargetRatio = 0.0f;
	if( pSkillProp->dwSpellRegion == SRO_REGION )	// REGION속성일땐 타겟도 데미지를 먹음.
		fTargetRatio = 1.0f;

	if( fRange <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Message( "CCtrl::ApplySkillAround : D:%d A:%d-%d %d %f", GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange );

	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;

	//------------ 적용대상이 플레이어인가 
	if( nApplyType & OBJTYPE_PLAYER )	
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer )
		{
			bApply = FALSE;	// 일단 FALSE로 초기화

			if( pObj->GetType() != OT_MOVER )
				Message( "ApplySkillAround : pObj가 MOVER가 아니다 %d", pObj->GetType() );

			if( pSrc->GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)pSrc;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
#ifdef __NPP_050308
					bApply	= TRUE;
#else	// __NPP_050308
					if( pAttacker->m_nDuel == 2 )	// 공격자가 파티듀얼중일땐 
					{
						if( ((CMover*)pObj)->m_idparty == pAttacker->m_idDuelParty )	// 상대 파티원에게만 데미지
							bApply = TRUE;
					} else
					if( pAttacker->m_nDuel == 1 ) // 공격자가 그냥 듀얼중일땐.
					{
						if( ((CMover*)pObj)->GetId() == pAttacker->m_idDuelOther )	// 듀얼상대에게만
							bApply = TRUE;
					}
#endif	// __NPP_050308
				} else
				{	// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			} else
			{	// 공격자가 무버가 아니다.
				bApply = TRUE;
			}

			if( bApply )
			{
				if( pObj->IsRangeObj( this, fRange ) )	// this(Center)의 스피어와 pObj의 스피어가 충돌했느냐
				{
					if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					} else
					{
						if( pObj != pSrc )		// 어태커는 검색대상에서 제외.
						{
							CMover *pTarget = (CMover *)pObj;		// <- 여긴 무조건 pObj는 플레이어므로 해도됨.
							bTarget = (this == pTarget);
							if( IsValidObj( pTarget ) && pTarget->IsLive() )
								pTarget->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg, bTarget );		// 대상에게 효과를 적용함.
						}
					}
				}
				
				bApply = FALSE;	// 다음 루프를 위해서 초기화.
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER

	// 적용대상이 몬스터/컨트롤 인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic )	// linkDynamic을 쓴다
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
			} else
			// 대상이 컨트롤이면.
			if( pObj->GetType() == OT_CTRL )
			{
				if( pSrc->GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
					bApply = TRUE;							
			}

			if( bApply )
			{
				if( pObj->IsRangeObj( this, fRange ) )	// this(Center)의 스피어와 pObj의 스피어가 충돌했느냐
				{
					if( pObj == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
					{
					} else
					{
						if( pObj != pSrc )		// 공격자는 검사대상에서 제외.
						{
							if( pObj->GetType() == OT_MOVER )	// 타겟이 무버냐?
							{
								if( IsValidObj( pObj ) && ((CMover *)pObj)->IsLive() )		// 타겟이 무버면 살아있는지 검사.
									((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
							} else
							{
								// 타겟이 무버가 아니냐?
								((CCtrl*)pObj)->ApplySkill( pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg );		// 대상에게 효과를 적용함.
							}
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // ApplySkillAround()

// 
// 폭, 높이, 길이를 설정하여 육면체 형태의 데미지 영역을 생성.
// nDmgType : AF_???
// nApplyType : OBJTYPE_PLAYER , MONSTER
// nAttackID : 스킬이나 아이템 ID
// fWidth, fHeight, fDepth : 폭, 높이, 길이.
// this == Attacker
void	CCtrl::ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProb )
{
#ifdef __WORLDSERVER
	int nRange	= 4;	// 4m
	float fDistSq;
	CObj* pObj;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if( pSkillProp == NULL )
		Message( "CCtrl::ApplySkillLine : pSkillProp == NULL %d", GetIndex() );
	
	// 데미지 영역 Local AABB.
	D3DXVECTOR3 vMin1 = D3DXVECTOR3( -fWidth / 2.0f, -fHeight / 2.0f, -fDepth );
	D3DXVECTOR3 vMax1 = D3DXVECTOR3(  fWidth / 2.0f,  fHeight / 2.0f, 0 );

	D3DXMATRIX mInv;		// 공격자측 월드역행렬
	D3DXMATRIX m1, m2, m3;
	D3DXMatrixRotationY( &m1, D3DXToRadian( -GetAngle() ) );
	D3DXMatrixTranslation( &m2, GetPos().x, GetPos().y, GetPos().z );
	D3DXMatrixMultiply( &m3, &m1, &m2 );	// world = rot * trans
	
	D3DXMatrixInverse( &mInv, NULL, &m3 );		// 공격자의 역행렬 만듬.

	// 타겟의 좌표를 공격자측 기준으로 역변환 한것.
	D3DXVECTOR3 vDestLocal;

	// 일반적으로 fDepth가 가장 길기때문에 검사 영역은 fDepth로 했다. 
	if( fDepth <= 4.0f )		nRange = 4;
	else if( fDepth <= 8.0f )	nRange = 8;
	else if( fDepth <= 16.0f )	nRange = 16;
	else						nRange = 32;
	
	if( fDepth <= 0 )	// 범위가 0이거나 음수일수는 없다.
		Message( "CCtrl::ApplySkillLine : %d %d", GetIndex(), fDepth );

	
	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;

	if( nApplyType & OBJTYPE_PLAYER )	// 적용대상이 플레이어인가 
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkPlayer )
		{
			bApply = FALSE;	// 시작은 FALSE

			if( pObj->GetType() != OT_MOVER )
				Message( "ApplySkillLine : pObj가 MOVER가 아니다 %d", pObj->GetType() );

			if( GetType() == OT_MOVER )		// 공격자가 무버냐.
			{
				CMover *pAttacker = (CMover *)this;
				if( pAttacker->IsPlayer() )			// 공격자가 플레이어냐
				{
#ifdef __NPP_050308
					bApply = TRUE;
#else // __NPP_050308
					if( pAttacker->m_nDuel == 2 )	// 공격자가 파티듀얼중일땐 
					{
						if( ((CMover*)pObj)->m_idparty == pAttacker->m_idDuelParty )	// 상대 파티원에게만 데미지
							bApply = TRUE;
					} else
						if( pAttacker->m_nDuel == 1 ) // 공격자가 그냥 듀얼중일땐.
						{
							if( ((CMover*)pObj)->GetId() == pAttacker->m_idDuelOther )	// 듀얼상대에게만
								bApply = TRUE;
						}
#endif // __NPP_050308
				} else
				{
					// 공격자가 몬스터면
					bApply = TRUE;		// 
				}
			} else
			{
				// 공격자가 무버가 아니다.
				bApply = TRUE;
			}
			
			if( bApply && pObj != this )		// this는 공격대상에서 제외.
			{
				CMover *pTarget = (CMover *)pObj;
				if( IsValidObj( pTarget ) && pTarget->IsLive() )
				{
					// 타겟측 좌표를 역변환.
					D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
					// 타겟측 AABB
					D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
					D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
					if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
					{
						pTarget->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb, 0, FALSE, bTarget );		// bTarget 타겟이 없으므로 FALSE로 넣음. PK시 선빵을 칠수없음
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

	// 적용대상이 몬스터인가.
	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
	{
		FOR_LINKMAP( GetWorld(), vPos, pObj, nRange, CObj::linkDynamic )
		{
			bApply = FALSE;
			if( pObj->GetType() == OT_MOVER )				// 대상이 무버면.
			{
				CMover *pTarget = (CMover *)pObj;
				if( pTarget->IsPeaceful() == FALSE )		// NPC가 아닌경우만 적용
					bApply = TRUE;
			} else
			// 대상이 컨트롤이면
			if( pObj->GetType() == OT_CTRL )
			{
				if( GetType() == OT_MOVER )			// 공격자가 무버일때만 적용. 컨트롤이 컨트롤에게 영향을 줄순 없다.
					bApply = TRUE;							
			}
			
			
			if( bApply )
			{
				if( pObj->GetType() == OT_MOVER )
				{
					CMover *pTarget = (CMover *)pObj;

					if( IsValidObj( pTarget ) && pTarget->IsLive() )
					{
						// 타겟측 좌표를 역변환.
						D3DXVec3TransformCoord( &vDestLocal, &pTarget->GetPos(), &mInv );
						// 타겟측 AABB
						D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
						D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
						if( ::IsTouchAABB( vMin1, vMax1, vMin2, vMax2 ) )		// AABB충돌검사.
						{
							((CCtrl*)pObj)->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );		// 대상에게 효과를 적용함.
						}
					}
				} else
					((CCtrl*)pObj)->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );		// 대상에게 효과를 적용함.
			}
		}
		END_LINKMAP
	}
		
#endif // WORLDSERVER		
} // ApplySkillLine()

//
// idParty 극단대상 범위 효과.
//
int		CCtrl::ApplySkillAroundTroupe( u_long idParty, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb )
{
	BOOL bRet = FALSE;
#ifdef __WORLDSERVER
	if( GetType() != OT_MOVER )		return bRet;	// 무버가 아닌게 극단스킬을 썼을리 없다.
	if( ((CMover *)this)->IsPlayer() == FALSE )		return bRet;	// 플레이어가 아닌게 극단스킬을 썼을리 없다.

	g_PartyMng.m_AddRemoveLock.Enter( theLineFile );		// lock1
	
	CParty *pParty	= g_PartyMng.GetParty( idParty );
	if( pParty )
	{
		int i;
		CUser *pMember;
		D3DXVECTOR3	vDist;
		float	fDist;
		float	fMaxDistSq = (float)pAddSkillProp->dwSkillRange;
		fMaxDistSq *= fMaxDistSq;
		FLOAT fTargetRatio = 0.0f;
		if( pSkillProp->dwSpellRegion == SRO_REGION )
			fTargetRatio = 1.0f;
		for( i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember	= g_UserMng.GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			if( IsValidObj( pMember ) && !pMember->IsDie() )
			{
				if( pMember == this && fTargetRatio == 0.0f )	// 타겟데미지 비율이 0이면 자기자신(타겟)은 데미지를 주지않음.
				{
				} else
				{
					vDist = pMember->GetPos() - GetPos();
					fDist = D3DXVec3LengthSq( &vDist );			// 파티 멤버와 시전자와의 거리.
					if( fDist < fMaxDistSq )					// xx미터보다 작으면
					{
						/*bNew = */pMember->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );	// 멤버 각각에게 효과 적용.
						bRet = TRUE;
	//					if( bNew && pSkillProp->dwSfxObj3 != NULL_ID && this != pMember )		// 일단 자기자신에게는 sfx안보냄 아래에 sfx생성땜에 중복될수있음.
	//						g_UserMng.AddCreateSfxObj( pMember, pSkillProp->dwSfxObj3 );
					}
				}
			}
		}
	}
	else
	{
		// 파티가 없음. 자기 자신에게만 사용. - 사용법이 바뀜. 파티가 없으면 자기에겐 써지지 않음.
//		if( GetType() == OT_MOVER )
//			ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb );
	}
	
	g_PartyMng.m_AddRemoveLock.Leave( theLineFile );		// unlock1
#endif // WorldServer
	return bRet;
} // ApplySkillAroundTroupe

//
// ApplySkill 내부에서 스킬의 하드코딩 처리부.
// this : Target
// pSrc : 시전자.
//
BOOL CCtrl::ApplySkillHardCoding( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __WORLDSERVER
	switch( pSkillProp->dwID )
	{
	case SI_ASS_HEAL_RESURRECTION:		// 부활
		if( GetType() == OT_MOVER )
		{
			CMover *pTarget = (CMover *)this;
			if( pTarget->IsPlayer() && pTarget->IsDie() )	// 플레이어이고 타겟이 죽어있을때
			{
				if( pTarget->m_pActMover->GetDmgState() == OBJSTA_RESURRECTION )
				{
					return FALSE;		// 이미 부활중인 사람에게 쓴거라면 취소.
				} else
				{
					g_dpDBClient.SendLogLevelUp( pTarget, 10 );	// 부활 스킬 로그
 #ifdef __NPP_050308
					if( ((CUser *)this)->m_Resurrection_Data.bUseing != TRUE )
					{
						((CUser *)this)->AddResurrectionMessage();
					
						((CUser *)this)->m_Resurrection_Data.bUseing        = TRUE;
						((CUser *)this)->m_Resurrection_Data.dwPlayerID     = ((CMover*)pSrc)->m_idPlayer;
						((CUser *)this)->m_Resurrection_Data.pSkillProp     = pSkillProp;
						((CUser *)this)->m_Resurrection_Data.pAddSkillProp  = pAddSkillProp;
					}

					break;
 #else //__NPP_050308					
					g_UserMng.AddHdr( pTarget, SNAPSHOTTYPE_RESURRECTION );		// 부활일땐 따로 보내자.
					pTarget->m_pActMover->SendActMsg( OBJMSG_RESURRECTION );	// 부활모드로 전환.
 #endif //__NPP_050308
				}

				// 부활때 DST_RECOVERY_EXP가 있으면 겸치가 조금 깎임.
				if( pAddSkillProp->dwDestParam2 == DST_RECOVERY_EXP )
					pTarget->SubDieDecExp(TRUE, pAddSkillProp->nAdjParamVal2 );	// 부활이 되면서 겸치가 조금 깎임.
			}
		}
		break;
	} // switch

 #ifdef __XITEM0722_2
	// 밀리는 양이 있는가?
	if( this->GetType() == OT_MOVER )
	{
		CMover *pTarget = (CMover *)this;
		DWORD dwDmgShift;
		if( pAddSkillProp )
			dwDmgShift = pAddSkillProp->dwDmgShift;
		else
			dwDmgShift = pSkillProp->dwDmgShift;
		if( dwDmgShift != NULL_ID )
		{
			FLOAT fAngle = GetDegree( pTarget->GetPos(), pSrc->GetPos() );	// 시전자와 타겟의 각도를 구함
			AngleToVectorXZ( &pTarget->m_pActMover->m_vDeltaE, fAngle, 0.85f );
			g_UserMng.AddPushPower( pTarget, pTarget->GetPos(), pTarget->GetAngle(), fAngle, 0.85f );
		}
		
  #ifdef __ACROBAT_0504
		// 상대방을 끌어당긴다
		if( pSkillProp->dwID == SI_ACR_YOYO_PULLING )
		{
			if( pAddSkillProp && pAddSkillProp->nDestData1[2] != NULL_ID )
			{
				// 당겨질 거리
				FLOAT fPullingLen = (FLOAT)pAddSkillProp->nDestData1[2] * 0.001f; 
				
				// 타겟과의 거리
				D3DXVECTOR3 v3Len = pTarget->GetPos() - pSrc->GetPos();
				FLOAT fTotalLen = D3DXVec3LengthSq( &v3Len ) * 0.1f;
				
				{
					if( fTotalLen > ((CMover*)this)->GetAttackRange( AR_HRANGE ) )
						fTotalLen = ((CMover*)this)->GetAttackRange( AR_HRANGE );
					
					// 거리에 따라 댕겨지는 힘이 다르게 적용됨
					FLOAT fDest = fTotalLen / ((CMover*)this)->GetAttackRange( AR_HRANGE );
					//fDest *= 1.4f; // 약간 더 줄이자
					
					{
						// 실제땡겨질 거리 계산
						FLOAT fAngle = GetDegree( pTarget->GetPos(), pSrc->GetPos() );	// 시전자와 타겟의 각도를 구함
						AngleToVectorXZ( &pTarget->m_pActMover->m_vDeltaE, fAngle, fPullingLen * -fDest);
						g_UserMng.AddPushPower( pTarget, pTarget->GetPos(), pTarget->GetAngle(), fAngle, fPullingLen * -fDest );
					}
				}
			}
		}
  #endif //__ACROBAT_0504
	}
 #endif // xItem0722
#endif // WorldServer
	return TRUE;
}

void CCtrl::CreateYoyoSkill( CSfx* pSfx, CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __CLIENT
#ifdef __ACROBAT_0504	
	ItemProp* pItemProp = ((CMover *)this)->GetActiveHandItemProp();
	
	if(pItemProp)
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vLocal;
		D3DXVECTOR3 vPosSrc   = GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 발사 지점을 임의로 올려준다. 땜빵 
		D3DXVECTOR3 vPosDest  = pTarget->GetPos() + D3DXVECTOR3( 0.0f, 1.0f, 0.0f ); // 목표 지점을 임의로 올려준다. 땜빵 
		
		CModelObject *pModel = (CModelObject *)m_pModel;
		
		int nSkill = pSkillProp->dwID;
		
		switch( nSkill )
		{
		case SI_ACR_SUP_SLOWSTEP:
		case SI_JST_YOYO_HITOFPENYA:
		case SI_JST_YOYO_VATALSTAB:
			{
				pModel->GetHandPos( &vPos, PARTS_RWEAPON, GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);
			}
			break;
		case SI_ACR_YOYO_DEADLYSWING:
		case SI_ACR_YOYO_CROSSLINE:
			{
				pModel->GetHandPos( &vPos, PARTS_RWEAPON, GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);

				pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );
				pSfx = CreateSfxYoYo( D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest );	
				((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_LWEAPON);
			}
			break;							
		}
	}
#endif //__CLIENT
#endif //__ACROBAT_0504		
}

//
// 스킬을 썼을때 클라에서 생성해야할 sfx 처리
//
void CCtrl::CreateSkillSfx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __CLIENT
		
#ifndef __XSKILL0425	
	// Skill중에서 발사형태.
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT )	// 발사체 sfx 생성.
	{
	}  // MAGICATKSHOT
	else
#endif // not xSkill0425
	{
		// 발사체 형태가 아니다.
		CSfx *pSfx = NULL;
		if( (int)pAddSkillProp->dwSkillTime > 0 )	// 지속시간이 있는 스킬은
		{
			if( pSkillProp->dwSfxObj4 == NULL_ID )		// 이펙이 유지되는 스킬이 아닐경우에만.
			{
#ifdef __ACROBAT_0504
				if( pSkillProp->dwExeTarget == EXT_MELEEATK )
#else //__ACROBAT_0504
				if( pSkillProp->dwExeTarget == EXT_SPECIALATK )
#endif //__ACROBAT_0504
				{
					D3DXVECTOR3 vLocal;
					
	#ifdef __ACROBAT_0504
					if( pSkillProp->dwLinkKind == IK3_YOYO )
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
	#endif //__ACROBAT_0504
					{
						AngleToVectorXZ( &vLocal, GetAngle(), 1.3f );	// 때리는방향 1미터앞.
						vLocal += GetPos();
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vLocal, pTarget->GetId(), 0 );
					
					if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), this->GetPos(), this->GetId(), 0 );  // 5번 이펙트를 출력함.
				} else
				{
					if( pSkillProp->dwSfxObj3 != NULL_ID )		// 타겟에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), pTarget->GetPos(), pTarget->GetId(), 0 );  // 3번 이펙트를 출력함.
					if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
						pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), this->GetPos(), this->GetId(), 0 );  // 5번 이펙트를 출력함.
				}

#ifdef __ACROBAT_0504
				if( pSkillProp->dwLinkKind == IK3_YOYO )  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
				{
					CreateYoyoSkill( pSfx, pTarget, pSkillProp, pAddSkillProp );
				}
#endif //__ACROBAT_0504
// 				CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), D3DXVECTOR3( 0, 0, 0), pTarget->GetId(), 0 );  // 3번 이펙트를 출력함.
			} else
			{
#ifdef __XITEMBUFF
				if( GetType() == OT_MOVER )
				{
					if( pSkillProp->dwSkillType == NULL_ID )	// NULL_ID면 아이템들
					{
						if( ((CMover *)this)->m_SkillState.HasSkill( BUFF_ITEM, pSkillProp->dwID ) == FALSE )	// 이미 실행되고 있지 않을때만 이펙 출력
							CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, this->GetPos(), this->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );
					} else
					{
						if( ((CMover *)this)->m_SkillState.HasSkill( BUFF_SKILL, pSkillProp->dwID ) == FALSE )	// 이미 실행되고 있지 않을때만 이펙 출력
							CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, this->GetPos(), this->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );
					}
				}
#else
				if( ((CMover *)this)->m_SkillState.HasSkill( 0, pSkillProp->dwID ) == FALSE )	// 이미 실행되고 있지 않을때만 이펙 출력
					CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj4, this->GetPos(), this->GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f) );
#endif
			}
		} else
		{	// 지속시간 없는 스킬
#ifdef __XSKILL0425
			if( pSkillProp->dwExeTarget == EXT_MELEEATK )	// 근접공격 스킬의 타격 이펙트.
#else 
			if( pSkillProp->dwSkillType == KT_SKILL )	// 근접공격 스킬의 타격 이펙트.
#endif // not xSkill0425
			{
				if( pSkillProp->dwSfxObj3 != NULL_ID )	
				{
					D3DXVECTOR3 vLocal;

#ifdef __ACROBAT_0504
					if( pSkillProp->dwLinkKind == IK3_YOYO )
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
#endif //__ACROBAT_0504
					{
						AngleToVectorXZ( &vLocal, GetAngle(), 1.3f );	// 때리는방향 1미터앞.
						vLocal += GetPos();
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vLocal, pTarget->GetId(), 0 );
				}
			} else
			{
				if( pSkillProp->dwSfxObj3 != NULL_ID )
				{
					D3DXVECTOR3 vPos = pTarget->GetPos();
					vPos.y += 1.0f;
					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vPos, pTarget->GetId(), 0 );
//					pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), D3DXVECTOR3(0,0,0), pTarget->GetId(), 0 );
				}
			}

#ifdef __ACROBAT_0504
			if( pSkillProp->dwLinkKind == IK3_YOYO )  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
			{
				CreateYoyoSkill( pSfx, pTarget, pSkillProp, pAddSkillProp );
			}
			else
#endif //__ACROBAT_0504				
			if( pSkillProp->dwSfxObj5 != NULL_ID )		// 시전자에게 발생
				pSfx = CreateSfx( D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), this->GetPos(), this->GetId(), 0 );  // 5번 이펙트는 시전자 자신에게 출력
		}
//		if( GetType() == OT_MOVER )		// 사람이 쓴것일땐 들고있는 무기를 판단해서 소리냄.
//		{
//			ItemProp* pItemProp = ((CMover *)this)->GetActiveHandItemProp();
//			if( pItemProp )
//				PLAYSND( pItemProp->dwSndAttack1, &GetPos() );
//		}
		if( pSfx )
		{
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
		}
			
		//dwSkillRange
		if( pSkillProp->dwID == SI_GEN_FLASH && pSfx )
		{
			FLOAT fRange = 1.5f;
			pSfx->SetScale(  D3DXVECTOR3(fRange, fRange, fRange) );
		}
	}
	
#endif // Client
} // CreateSkillSfx


// ApplySkill 내부에서 데미지 주는부분만 빠짐.
int		CCtrl::ApplyDamage( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) 
{
	int nDamage = 0;
	// 데미지류 스킬의 데미지 전달.
	switch( pSkillProp->dwExeTarget )
	{
#ifdef __ACROBAT_0504
	case EXT_MELEEATK:		// 근접스킬데미지공격
#else
	case EXT_SPECIALATK:		// 근접스킬데미지공격
#endif
	case EXT_MAGICATK:			// 즉시마법데미지공격
	case EXT_MAGICATKSHOT:
		//		if( pSkillProp->dwID != SI_MAG_MAG_MENTALSTRIKE )	// 멘탈 스트라이크는 예외.
		{
			if( pSkillProp->dwID == SI_MAG_FIRE_HOTAIR )	// 핫에어 땜빵처리.
			{
				nDamage = SendDamage( AF_MAGICSKILL, pSrc->GetId(), nParam, bTarget );
			} else
			{
				int nAtkFlag = 0;
				if( pSkillProp->dwSkillType == KT_MAGIC )
					nAtkFlag = AF_MAGICSKILL;
				else
					nAtkFlag = AF_MELEESKILL;
				
#ifdef __XSKILL0425
				if( pSkillProp->dwComboStyle == CT_FINISH )		nDamage = SendDamageForce( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget );
				else											nDamage = SendDamage( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget );
#else
				if( pSkillProp->dwComboStyle == CT_FINISH )		nDamage = SendDamageForce( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16), bTarget );
				else											nDamage = SendDamage( nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16), bTarget );
#endif
			}
		}
		break;
	}

	return nDamage;
}

// 적용가능한 스킬이냐?
BOOL CCtrl::IsPossibleApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __XITEM0722_2
#if 0 
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_OBJCHGPARAMET:
	case EXT_MAGIC:
	case EXT_ANOTHER:
	case EXT_ANOTHERWITH:
		{
			DWORD	dwDestParam, dwSkillTime, dwPainTime, dwAbilityMin;
			int		nAdjParam, nChgParam;
			int		nDestData[3];
			for( int i = 0; i < 2; i ++ )
			{
				if( pAddSkillProp )
				{
					dwDestParam = pAddSkillProp->dwDestParam[i];
					dwSkillTime = pAddSkillProp->dwSkillTime;
					dwPainTime = pAddSkillProp->dwPainTime;
					dwAbilityMin = pAddSkillProp->dwAbilityMin;
					nAdjParam	= pAddSkillProp->nAdjParamVal[i];
					nChgParam	= (int)pAddSkillProp->dwChgParamVal[i];
					nDestData[0] = pAddSkillProp->nDestData1[0];
					nDestData[1] = pAddSkillProp->nDestData1[1];
					nDestData[2] = pAddSkillProp->nDestData1[2];
				} else
				{	
					dwDestParam = pSkillProp->dwDestParam[i];
					dwSkillTime = pSkillProp->dwSkillTime;
					dwPainTime  = pSkillProp->tmContinuousPain;
					dwAbilityMin = pSkillProp->dwAbilityMin;
					nAdjParam	= pSkillProp->nAdjParamVal[i];
					nChgParam	= (int)pSkillProp->dwChgParamVal[i];
					nDestData[0] = pSkillProp->nDestData1[0];
					nDestData[1] = pSkillProp->nDestData1[1];
					nDestData[2] = pSkillProp->nDestData1[2];
				}
				// destParam 1
				switch( dwDestParam )
				{
				case DST_CHRSTATE:		// 상태 변경
					if( nAdjParam & CHS_POISON )	// 독감염
						if( GetAdjParam( DST_IMMUNITY ) & CHS_POISON ) 
							return;		// 독에 면역상태다.
						break;
				}
			} // for
		}
		break;
	} // switch
#endif // 0
#endif // xItem0722
	return TRUE;
}


void	CCtrl::ApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bTarget )
{
#ifndef __BEAST
	// 적용가능한 스킬이냐?
	if( IsPossibleApplySkill( pSrc, pSkillProp, pAddSkillProp ) == FALSE )
		return;

	// 데미지속성이 있을경우 그것을 처리.
	int nDamage = ApplyDamage( pSrc, pSkillProp, pAddSkillProp, nParam, bTarget );
	if( bOnlyDmg == TRUE )		
		return;

#if defined(__NPP_050308) && defined(__WORLDSERVER)
	if( this != pSrc && pSrc->GetType() == OT_MOVER && GetType() == OT_MOVER )		// 광역 디버프 중 타겟이아니면 skip
	{
		if( ((CMover *)this)->IsDie() == FALSE )
		{
			if( pSkillProp->nEvildoing < 0 )													// 나쁜마법은
				if( ((CMover *)pSrc)->GetHitType2(((CMover *)this), bTarget ) == HITTYPE_FAIL )  	// 평화적인상대에게
					return;																			// 적용안됨
			if( pSkillProp->nEvildoing > 0 )													// 좋은 마법은
				if( ((CMover *)pSrc)->GetHitType2(((CMover *)this), bTarget ) != HITTYPE_FAIL )  	// 적대적인상대에게
					return;																			// 적용안됨
		}
	}
#endif //defined(__NPP_050308) && defined(__WORLDSERVER)

	// 지속시간이 있는 스킬의 처리.
	int nProb = 0;
	if( pAddSkillProp )		
		nProb = pAddSkillProp->nProbability;
	else					
		nProb = pSkillProp->nProbability;

	if( bIgnoreProb )		// 확률무시옵이 있으면 무조건 100%
		nProb = NULL_ID;
	
	if( nProb == NULL_ID || xRandom(100) < nProb  )		// 적용 확률, =로 되어 있으면 100% 적용
	{
		if( GetType() == OT_MOVER )		// 타겟이 무버일때만 지속시간 처리
		{
			int nSkillTime = 0;
			if( pAddSkillProp )		
				nSkillTime = (int)pAddSkillProp->dwSkillTime;
			else					
				nSkillTime = (int)pSkillProp->dwSkillTime;

			BOOL bNew = TRUE;	// 스킬이 첨 적용됐는가.
			if( nSkillTime > 0 )	// 지속시간이 있는 스킬(프로텍션 등)
			{
				DWORD dwTime1 = 0, dwTime2 = 0;
				if( pSkillProp->dwReferTarget1 == RT_TIME || pSkillProp->dwReferTarget2 == RT_TIME )
				{
					if( pSrc->GetType() == OT_MOVER )		// 시전자가 무버일때만.
						((CMover *)pSrc)->SubReferTime( &dwTime1, &dwTime2, pSkillProp, pAddSkillProp );
				}

				WORD wType;
				if( pSkillProp->dwSkillType == NULL_ID )	
					wType = BUFF_ITEM;
				else										
					wType = BUFF_SKILL;

				// pTarget은 this가 될수도있고 타인이 될수도 있다.
			#if defined(__WORLDSERVER)
				int nLevel = 0;
				if( pAddSkillProp )		
					nLevel = pAddSkillProp->dwSkillLvl;
				else					
					nLevel = 0;

				// dwSkillTime이 스킬자체의 지속시간으로도 쓰이고 부가효과의 지속시간으로도 쓰이기때문에 이런구조는 맞지 않다
				if( pSkillProp->dwID == SI_ELE_FIRE_BURINGFIELD )
					bNew = FALSE;	//  버닝필드는 예외처리 
				else
					bNew = ((CMover *)this)->m_SkillState.Set( wType, pSkillProp->dwID, nLevel, nSkillTime + (int)dwTime1 + (int)dwTime2 );	

				
			#elif defined(__CLIENT)
				bNew = ((CMover *)this)->m_SkillState.HasSkill( wType, pSkillProp->dwID ) ? FALSE : TRUE;
			#endif // Client
			}
			
			// 최초 적용된 스킬에만
			if( bNew )	// 이미 걸려있는 상태에서 또 쓰면 bNew는 FALSE가 된다.
			{
			#ifdef __WORLDSERVER
				// 스킬중 예외처리 해야할것을 처리함.
				if( ApplySkillHardCoding( pSrc, pSkillProp, pAddSkillProp ) == FALSE )
					return;

				// 부활스킬셋팅이 되어있다면 아랫것들 무시
				if( ((CMover *)this)->m_Resurrection_Data.bUseing )
					return;

				// 데미지도 주면서 파라메터도 변하는 스킬이 있기땜에 데미지랑 따로 처리.
				switch( pSkillProp->dwExeTarget )
				{
				case EXT_SELFCHGPARAMET:		// 시전자 자신.
					if( pSrc->GetType() == OT_MOVER )	// 타겟(여기선 타겟이 시전자)이 무버일때만...
						((CMover *)pSrc)->ApplyParam( (CMover *)pSrc, pSkillProp, pAddSkillProp, TRUE, nDamage );
					break;
				case EXT_OBJCHGPARAMET:
				case EXT_MAGIC:
				case EXT_ANOTHER:
				case EXT_ANOTHERWITH:
				default:
					if( this->GetType() == OT_MOVER )	// 타겟이 무버일때만.
						((CMover *)this)->ApplyParam( (CMover *)pSrc, pSkillProp, pAddSkillProp, TRUE , nDamage );
					break;
				}

				// 밀리는 양이 있는가?
				if( this->GetType() == OT_MOVER )
				{
					CMover *pTarget = (CMover *)this;
					DWORD dwDmgShift;
					if( pAddSkillProp )
						dwDmgShift = pAddSkillProp->dwDmgShift;
					else
						dwDmgShift = pSkillProp->dwDmgShift;

					if( dwDmgShift != NULL_ID && pTarget->GetProp()->dwClass != RANK_SUPER )
					{
						FLOAT fAngle = GetDegree( pTarget->GetPos(), pSrc->GetPos() );	// 시전자와 타겟의 각도를 구함
						AngleToVectorXZ( &pTarget->m_pActMover->m_vDeltaE, fAngle, 0.85f );
						g_UserMng.AddPushPower( pTarget, pTarget->GetPos(), pTarget->GetAngle(), fAngle, 0.85f );
					}
					
					// 상대방을 끌어당긴다
					if( pSkillProp->dwID == SI_ACR_YOYO_PULLING )
					{
						if( pAddSkillProp && pAddSkillProp->nDestData1[2] != NULL_ID )
						{
							// 당겨질 거리
							FLOAT fPullingLen = (FLOAT)pAddSkillProp->nDestData1[2] * 0.001f; 
							
							// 타겟과의 거리
							D3DXVECTOR3 v3Len = pTarget->GetPos() - pSrc->GetPos();
							FLOAT fTotalLen = D3DXVec3LengthSq( &v3Len ) * 0.1f;
							{
								if( fTotalLen > ((CMover*)this)->GetAttackRange( AR_HRANGE ) )
									fTotalLen = ((CMover*)this)->GetAttackRange( AR_HRANGE );

								// 거리에 따라 댕겨지는 힘이 다르게 적용됨
								FLOAT fDest = fTotalLen / ((CMover*)this)->GetAttackRange( AR_HRANGE );
								{
									// 실제땡겨질 거리 계산
									FLOAT fAngle = GetDegree( pTarget->GetPos(), pSrc->GetPos() );	// 시전자와 타겟의 각도를 구함
									AngleToVectorXZ( &pTarget->m_pActMover->m_vDeltaE, fAngle, fPullingLen * -fDest);
									g_UserMng.AddPushPower( pTarget, pTarget->GetPos(), pTarget->GetAngle(), fAngle, fPullingLen * -fDest );
								}
							}
						}
					}
				}
			#endif // __WORLDSERVER
			} // if( bNew )

		#ifdef __WORLDSERVER
			if( pSkillProp->dwID == SI_GEN_FLASH && GetType() == OT_MOVER && ((CMover *)this)->IsPlayer() )
			{
				((CUser *)this)->AddCommonSkill( pSkillProp->dwID, pAddSkillProp->dwSkillLvl );
			}
		#endif // __WORLDSERVER
		
		}	// this == OT_MOVER
	}	// if( nProb == NULL_ID || xRandom(100) < nProb  )
	
	#ifdef __WORLDSERVER
		switch( pSkillProp->dwSpellRegion )
		{
		case SRO_AROUND:
		case SRO_REGION:	// 범위공격일땐 여기서 타겟들 각각에 sfxobj3를 뿌려줌.
			if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT )
				if( pSkillProp->dwSfxObj2 != NULL_ID )
					g_UserMng.AddCreateSfxObj( this, pSkillProp->dwSfxObj2 );
			break;
		}

		// 예외처리: 강탈스킬 - 페냐를 훔치면서 데미지를 준다
		if( pSkillProp->dwID == SI_ACR_YOYO_SNITCH )
		{
			// pSrc - 공격자 플레이어, this - 피공격자 몬스터 
			// 피공격자 잔액이 0이 아니면 강탈 한다.
			CUser* pUser = (CUser*)pSrc;
			CUser* pDest = (CUser*)this;
			if( pDest->GetType() == OT_MOVER && pDest->GetGold() > 0 )
			{
				int nPlus = (pDest->GetLevel()*20) - (pUser->GetLevel()*10);
				
				if( pUser->GetLevel() <= 20 )
				{
				}
				else
				if( pUser->GetLevel() <= 30 )
				{
					nPlus *= 0.7f;
				}
				else
				if( pUser->GetLevel() <= 40 )
				{
					nPlus *= 0.5f;
				}
				else
				{
					nPlus *= 0.3f;
				}
		
				if( nPlus > 0 )
				{				
					nPlus = min( nPlus, pDest->GetGold() );		// 피공격자 돈 보다는 많지 않게 
					pUser->AddGold( nPlus );
					pDest->AddGold( -nPlus );

					pUser->AddGoldText( nPlus );
				}
			}		
		}
	#endif	//__WORLDSERVER
#endif	// __BEAST	
} // ApplySkill

#ifdef __XSYNC0727
// 발사형태의 스킬의 발사체를 생성하는 부분
// 이 발사체가 타겟에게 맞았을때 DoApplySkill()이 발동된다.
int		CCtrl::ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __CLIENT
	// 발사형태가 아니면 리턴
	if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT && pSkillProp->dwExeTarget != EXT_MAGICSHOT )	// 발사체 sfx 생성.
		return 0;

	DWORD dwShootObj = pSkillProp->dwSfxObj2;

	if( dwShootObj != NULL_ID )
	{
		D3DXVECTOR3 vPos = GetPos();
		
	#ifdef __ACROBAT_0504
		if( ((CMover*)this)->IsPlayer() )		
		{
			// 보우에서 나가는 화살은 몇가지 셋팅을 해주자
			if( pSkillProp->dwLinkKind == IK3_BOW )
			{
				// 화살은 왼손에 연결...
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );		// 주먹 월드좌표 구함		
				vPos.y -=	1.0f;
			}
		}
	#endif //__ACROBAT_0504
		
		CSfxShoot *pShootSfx = CreateShootSfx( D3DDEVICE, dwShootObj, vPos, GetId(), D3DXVECTOR3(0,0,0), pTarget->GetId() );
		if( pShootSfx )
		{
	#ifdef __ACROBAT_0504
			/*
			// 보우화살 셋팅
			if( pSkillProp->dwLinkKind == IK3_BOW )
			{
				pShootSfx->ShootSfx( GetAngle(), -GetAngleX(), 1.2f );
			}
			*/
	#endif //__ACROBAT_0504
			
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
			
			if( GetType() == OT_MOVER )
			{
				if( ((CMover *)this)->IsActiveMover() )		// g_pPlayer가 쏠때만 생성해서 전송.
				{
					int idSfxHit = ++(((CMover *)this)->m_idSfxHit);	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
					int		nMaxDmgCnt = 1;
					if( pSkillProp->tmContinuousPain != NULL_ID )		// 지속스킬이 있는 데미지 형태면
						nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// 몇번 데미지를 먹냐.
					
					if( pSkillProp->dwSkillType == KT_SKILL )
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MELEESKILL, NULL_ID, nMaxDmgCnt );
					else
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MAGICSKILL, NULL_ID, nMaxDmgCnt );
					pShootSfx->m_idSfxHit	= idSfxHit;
				}
			}
			pShootSfx->SetSkill( pSkillProp->dwID );
//			pShootSfx->m_dwSkillLevel	= pAddSkillProp->dwSkillLvl;
			pShootSfx->m_nMagicPower	= pAddSkillProp->dwSkillLvl;	// 스킬레벨은 MagicPower변수로 같이써도 될거 같다.
			pShootSfx->m_dwSfxHit = pSkillProp->dwSfxObj3;	// 폭발 이펙트 등록.
		}
		// 지속시간동안 이펙트도 유지되어야 하는것은 CSkillInfluence::Process()에서 처리한다.
	} else
		Message( "ShootSkill MAGICATKSHOT : %s dwSfxObj2가 지정되지 않음", pSkillProp->szName );
#endif // CLIENT
	return 1;
}
#else // xSync0727
//{{AFX
// 발사형태의 스킬의 발사체를 생성하는 부분
// 이 발사체가 타겟에게 맞았을때 DoApplySkill()이 발동된다.
int		CCtrl::ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp )
{
#ifdef __CLIENT
	// 발사형태가 아니면 리턴
	if( pSkillProp->dwExeTarget != EXT_MAGICATKSHOT 
 #ifdef __XSKILL0425
		&& pSkillProp->dwExeTarget != EXT_MAGICSHOT 
 #endif
		)	// 발사체 sfx 생성.
		return 0;

	DWORD dwShootObj;
/*
	#ifdef __ACROBAT_0504
	if( pSkillProp->dwLinkKind == IK3_BOW )
	{
		CItemElem *pItemElem = ((CMover*)this)->GetEquipItem( PARTS_BULLET );  // 화살아이템을 참조로 화살이펙트 생성
		if( pItemElem && pItemElem->GetProp() )
		{
			dwShootObj = pItemElem->GetProp()->dwSfxObj2;
		}
	}
	else
	#endif //__ACROBAT_0504
*/
	{
		dwShootObj = pSkillProp->dwSfxObj2;
	}

	if( dwShootObj != NULL_ID )
	{
		D3DXVECTOR3 vPos = GetPos();
		
	#ifdef __ACROBAT_0504
		if( ((CMover*)this)->IsPlayer() )		
		{
			// 보우에서 나가는 화살은 몇가지 셋팅을 해주자
			if( pSkillProp->dwLinkKind == IK3_BOW )
			{
				// 화살은 왼손에 연결...
				CModelObject *pModel = (CModelObject *)m_pModel;
				pModel->GetHandPos( &vPos, PARTS_LWEAPON, GetMatrixWorld() );		// 주먹 월드좌표 구함		
				vPos.y -=	1.0f;
			}
		}
	#endif //__ACROBAT_0504
		
		CSfxShoot *pShootSfx = CreateShootSfx( D3DDEVICE, dwShootObj, vPos, GetId(), D3DXVECTOR3(0,0,0), pTarget->GetId() );
		if( pShootSfx )
		{
	#ifdef __ACROBAT_0504
			/*
			// 보우화살 셋팅
			if( pSkillProp->dwLinkKind == IK3_BOW )
			{
				pShootSfx->ShootSfx( GetAngle(), -GetAngleX(), 1.2f );
			}
			*/
	#endif //__ACROBAT_0504
			
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND( pSkillProp->dwSndAttack1, &vPos );		// 발사 효과음.
			
			if( GetType() == OT_MOVER )
			{
				if( ((CMover *)this)->IsActiveMover() )		// g_pPlayer가 쏠때만 생성해서 전송.
				{
					int idSfxHit = ++(((CMover *)this)->m_idSfxHit);	// 클라에서 발사체가 생성된 즉시 ID를 만들고 그것을 서버로 보냄.
					int		nMaxDmgCnt = 1;
					if( pSkillProp->tmContinuousPain != NULL_ID )		// 지속스킬이 있는 데미지 형태면
						nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;		// 몇번 데미지를 먹냐.
					
					if( pSkillProp->dwSkillType == KT_SKILL )
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MELEESKILL, NULL_ID, nMaxDmgCnt );
					else
						g_DPlay.SendSfxID( pTarget->GetId(), idSfxHit, AF_MAGICSKILL, NULL_ID, nMaxDmgCnt );
					pShootSfx->m_idSfxHit	= idSfxHit;
				}
			}
			pShootSfx->SetSkill( pSkillProp->dwID );
//			pShootSfx->m_dwSkillLevel	= pAddSkillProp->dwSkillLvl;
			pShootSfx->m_nMagicPower	= pAddSkillProp->dwSkillLvl;	// 스킬레벨은 MagicPower변수로 같이써도 될거 같다.
			pShootSfx->m_dwSfxHit = pSkillProp->dwSfxObj3;	// 폭발 이펙트 등록.
		}
		// 지속시간동안 이펙트도 유지되어야 하는것은 CSkillInfluence::Process()에서 처리한다.
	} else
		Message( "ShootSkill MAGICATKSHOT : %s dwSfxObj2가 지정되지 않음", pSkillProp->szName );
#endif // CLIENT
	return 1;
}
//}}AFX
#endif // not xSync0727

// 범위적용시 타겟의 종류에 따라 주변 타겟의 종류를 선택함.
int		CCtrl::TargetSelecter( CCtrl *pTarget )
{
	int nApplyType = 0;
	CCtrl *pSrc = this;

	if( pTarget->GetType() == OT_MOVER )	// 타겟이 무버일때
	{
#ifdef __NPP_050308
		nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;	// 주위 플레이어들/몬스터에게 적용
#else	// __NPP_050308
		if( ((CMover *)pTarget)->IsPlayer() )			// 타겟이 플레이어면.
			nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;	// 주위 플레이어들/몬스터에게 적용
		else
			nApplyType = OBJTYPE_MONSTER;	// 타겟이 몬스터면 주위 몬스터에게만 적용.
#endif	// __NPP_050308
	} else
	{
		// 타겟이 컨트롤일땐
		if( pSrc->GetType() == OT_MOVER )	// 공격자가 무버냐
		{
			if( ((CMover *)pSrc)->IsPlayer() )		// 공격자가 플레이어면
#ifdef __NPP_050308
				nApplyType = OBJTYPE_PLAYER | OBJTYPE_CTRL | OBJTYPE_MONSTER;	// 주위 몬스터와 컨트롤에게 적용.
#else	// __NPP_050308
				nApplyType = OBJTYPE_CTRL | OBJTYPE_MONSTER;	// 주위 몬스터와 컨트롤에게 적용.
#endif	// __NPP_050308
		}
		else
		{	// 공격자가 컨트롤이냐
			nApplyType = OBJTYPE_PLAYER;	// 플레이어에게 적용
		}
	}

	return nApplyType;
}

int		CCtrl::DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg )
{
	int nResult = DoApplySkillEx( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, nParam, bOnlyDmg );
	if( nResult == 1 )
	{
	#if defined(__NPP_050308) && defined(__WORLDSERVER)	
		if( this != pTarget && GetType() == OT_MOVER && pTarget->GetType() == OT_MOVER )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜마법을 쓰면 공격과 같다고 간주한다. 
				((CMover*)pTarget)->OnAttacked( (CMover*)this, 0, TRUE );	// TRUE는 타겟이다.
		}
	#endif 
	}

	return nResult;
}

// 
// 지정된 스킬의 효과를 pTarget에게 적용한다.
// 지정범위에 따라서 pTarget을 중심으로 다른 target에도 적용된다.
// 이것을 사용하는 주체는 사람이 아닐수도 있다
// 여기서는 SpellRegion에 따라 적용대상을 분류만 하고 실제 효과적용은
// ApplySkill에서 처리한다.
// bOnlyDmg : 오직 데미지적용시키는것만 한다.  광역 지속데미지개체에서 사용함.
int		CCtrl::DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg )
{
	int		nRet = 1;
	CCtrl	*pSrc = this;

	switch( pSkillProp->dwSpellRegion )
	{
	case SRO_AROUND:	// 대상 0% 주위 100%
	case SRO_REGION:	// 대상 100% 주위 100%
		{
			if( pSkillProp->dwExeTarget != EXT_TROUPE )	// 극단대상이 아닐때.
			{
				int nApplyType = 0;
				nApplyType = pSrc->TargetSelecter( pTarget );
				pTarget->ApplySkillAround( this, nApplyType, pSkillProp, pAddSkillProp, bIgnoreProb, bOnlyDmg );
			} else
			{
				// 극단 대상 범위 효과.
				if( GetType() == OT_MOVER )
				{
					if( ((CMover *)this)->IsPlayer() )
					{
						if( ApplySkillAroundTroupe( ((CMover *)this)->m_idparty, pSkillProp, pAddSkillProp, bIgnoreProb ) == FALSE )
							return 0;  // 사용실패.
					}
				}
			}
#ifdef __WORLDSERVER
			if( pSkillProp->tmContinuousPain != NULL_ID )	// 지속데미지 형태냐?
			{
				CContDamageCtrl *pDmgCtrl = new CContDamageCtrl;	// 지속데미지를 주는 가상오브젝트 생성 - 메모리풀링 처리 할것
				if( pDmgCtrl )
				{
					pDmgCtrl->m_pSkillProp = pSkillProp;
					pDmgCtrl->m_pAddSkillProp = pAddSkillProp;
					pDmgCtrl->m_idSrc = pSrc->GetId();
					pDmgCtrl->m_idTarget = pTarget->GetId();
					pDmgCtrl->SetPos( pTarget->GetPos() );		// 중심이 되는 타겟의 좌표로...
					GetWorld()->AddObj( pDmgCtrl, FALSE );
				}
			}
#endif
		}
		break;
	case SRO_LINE:		// this를 중심으로 m_fAngle라인.
		{
			int nApplyType = 0;
			nApplyType = pSrc->TargetSelecter( pTarget );
			ApplySkillLine( nApplyType, pSkillProp, pAddSkillProp, 2.5f, 2.5f, (float)pAddSkillProp->dwSkillRange, bIgnoreProb );
		}
		break;
	case SRO_DIRECT:	// 대상 하나에만 적용
	default:
		if( pSkillProp->dwID != SI_PSY_PSY_PSYCHICWALL )		// 사이킥월은 처리 안함.
			pTarget->ApplySkill( this, pSkillProp, pAddSkillProp, bIgnoreProb, nParam );
		break;
	} 

#ifdef __WORLDSERVER		
	// 싸이킥월의 벽을 만들어냄.
	if( pSkillProp->dwID == SI_PSY_PSY_PSYCHICWALL )
	{
		BOOL bCreate = FALSE;
		int i = 0;
		if( GetType() == OT_MOVER )
		{
			if( ((CMover *)this)->IsPlayer() )
			{
				CUser *pUser = (CUser *)this;
				for( i = 0; i < 2; i ++ )		// 한번에 2개이상 생성되지 않게함.
				{
					if( pUser->m_pWall[i] == NULL )
					{
						CObj *pObj = CreateObj( D3DDEVICE, OT_CTRL, CI_PSYCHICWALL );		// 벽 컨트롤 생성.
						if( pObj )
						{
							pObj->SetPos( this->GetPos() );
							pObj->SetAngle( -GetAngle() );
							((CCommonCtrl *)pObj)->m_pSkillProp = pSkillProp;
							((CCommonCtrl *)pObj)->m_pAddSkillProp = pAddSkillProp;
							((CCommonCtrl *)pObj)->m_idAttacker = GetId();
							GetWorld()->AddObj( pObj, TRUE );
							pUser->m_pWall[i] = pObj;
							bCreate = TRUE;
							break;
						}
					}
				}
			}
		}
		if( bCreate == FALSE )	// 생성못했으면
			nRet = 0;			// 실패 리턴

	}


#ifdef __ACROBAT_0504
	//*
	if( pSrc->GetType() == OT_MOVER )
	{
 #ifdef __XITEMBUFF
		// 모든 스킬사용시 다크일루젼상태이면 해제한다!!~
		if( pSkillProp->dwID != SI_ACR_SUP_DARKILLUSION )
		{
			if( ((CMover *)pSrc)->m_SkillState.HasSkill( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION ) )
			{
				((CMover *)pSrc)->SetDarkCover( FALSE );
				((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_ACR_SUP_DARKILLUSION );		// 해제.
			}
		}
		/**/

		// 제스터 버프는 특정 버프가 있으면 해제하고 걸어야함
		if( pSkillProp->dwID == SI_JST_SUP_POISON )
		{
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_JST_SUP_BLEEDING );
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_JST_SUP_ABSORB );
		}
		else
		if( pSkillProp->dwID == SI_JST_SUP_BLEEDING )
		{
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_JST_SUP_POISON );
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_JST_SUP_ABSORB );
		}
		else		
		if( pSkillProp->dwID == SI_JST_SUP_ABSORB )
		{
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_JST_SUP_BLEEDING );
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( BUFF_SKILL, SI_JST_SUP_POISON );
		}
 #else // xItemBuff
		// 모든 스킬사용시 다크일루젼상태이면 해제한다!!~
		if( pSkillProp->dwID != SI_ACR_SUP_DARKILLUSION )
		{
			if( ((CMover *)pSrc)->m_SkillState.HasSkill( 0, SI_ACR_SUP_DARKILLUSION ) )
			{
				((CMover *)pSrc)->SetDarkCover( FALSE );
				((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_ACR_SUP_DARKILLUSION );		// 해제.
			}
		}
		/**/
		
		// 제스터 버프는 특정 버프가 있으면 해제하고 걸어야함
		if( pSkillProp->dwID == SI_JST_SUP_POISON )
		{
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_JST_SUP_BLEEDING );
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_JST_SUP_ABSORB );
		}
		else
		if( pSkillProp->dwID == SI_JST_SUP_BLEEDING )
		{
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_JST_SUP_POISON );
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_JST_SUP_ABSORB );
		}
		else		
		if( pSkillProp->dwID == SI_JST_SUP_ABSORB )
		{
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_JST_SUP_BLEEDING );
			((CMover *)pSrc)->m_SkillState.RemoveSkillInfluence( SI_JST_SUP_POISON );
		}
#endif // not xItemBuff
	}
	
#endif //__ACROBAT_0504	
	
	
#endif // WorldServer
	
	// 스킬에 필요한 sfx를 생성.
	CreateSkillSfx( pTarget, pSkillProp, pAddSkillProp );
	
	// 2차 스킬 발동. ActiveSkill
#ifdef __XSKILL0425
	if( pSkillProp->dwActiveSkill != NULL_ID )
	{
		if( pSkillProp->dwActiveSkillRate == NULL_ID || xRandom(100) <= pSkillProp->dwActiveSkillRate )	// 발동확률에 걸렸는가.
			pSrc->DoActiveSkill( pSkillProp->dwActiveSkill, 1, pTarget );		// dwActiveSkill Lv1이 발동됨.
			
	}
#endif // xSkill0425

	return nRet;
}

// nLevel의 dwSkill이 pTarget을 향하여 발동됨.  어태커는 this
// bIgnoreProb이 TRUE이면 내부에서 확률계산을 무시하고 100% 실행되게 한다.
void CCtrl::DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb )
{
#ifdef __XSKILL0425
	ItemProp *pSkillProp;
	AddSkillProp *pAddSkillProp;
	GetSkillProp( &pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoActiveSkill" );
	
	int nRet;
	if( pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || 
		pSkillProp->dwExeTarget == EXT_MAGICSHOT )		// 쏘는 형태면 sfx만 발생
		nRet = ShootSkill( pTarget, pSkillProp, pAddSkillProp );
	else
	{
		nRet = DoApplySkill( pTarget, pSkillProp, pAddSkillProp, bIgnoreProb );
 #ifdef __WORLDSERVER		
		g_UserMng.AddDoApplySkill( this, pTarget->GetId(), dwSkill, nLevel );
 #endif
	}
#endif // xSkill0425
}


