#include "stdafx.h"
#include "defineSkill.h"
#include "defineText.h"
#ifdef __CLIENT
#include "AppDefine.h"
#endif

#include "ModelObject.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#else	// __CLIENT
#ifdef __WORLDSERVER
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER
#endif	// __CLIENT

#include "ship.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*--------------------------------------------------------------------------------------------------------


											Mover act command


----------------------------------------------------------------------------------------------------------*/
int g_1 = 0;
int g_2 = 0;
int g_3 = 0;
void	CMover::SetCmd( OBJACT cmd, int nParam1, int nParam2, int nParam3 )
{
	m_oaCmd = cmd;
	m_nCParam[0] = nParam1;		// 명령 파라메터.
	m_nCParam[1] = nParam2;
	m_nCParam[2] = nParam3;

//	if( cmd == OBJACT_USESKILL )
//	{
//		if( g_1 == 7 && nParam1 == 7 )
//		{
//			int a = 0;
//		}
//		g_1 = nParam1;
//	}
//	TRACE( "cmd:%d %d   ", (int)cmd, nParam1 );
/*	g_3 = g_2;
	g_2 = g_1;
	g_1 = nParam1;

	if( g_1 == 7 && g_3 == 7 )
	{
		int a= 0;
	}
*/	
}

//
// fRange = 도달범위.  디폴트는 0.0f은 상대방의 반경까지 붙는거다.
void CMover::SetDestObj( OBJID objid, float fRange, BOOL bSend )
{
#ifdef __WORLDSERVER
	if( FALSE == IsPlayer() && m_idDest != objid )
		m_nCorr		= -1;
#else	// __WORLDSERVER
 #ifdef __ONLINE
	if( bSend && IsActiveMover() && m_idDest != objid )
	{
		g_DPlay.SendPlayerDestObj( objid, fRange );
		m_nCorr	= -1;
	}
 #endif	// __ONLINE
#endif	// not __WORLDSERVER
	m_idDest	= objid;
	m_fArrivalRange = fRange;
	ClearDestPos();

#ifdef __XSYNC0711
#ifdef __WORLDSERVER
	if( bSend )
		g_UserMng.AddMoverSetDestObj( this, objid, fRange, TRUE );		// 서버에서 SetDestObj를 실행하면 클라들에게 보내주는게 없어서 이걸 넣음.
#endif
#endif

#ifdef __CLIENT
 #ifdef __XUZHU
//	if( IsActiveMover() && m_oaCmd != OBJACT_NONE )
//		g_WndMng.PutString( "명령제거", NULL, 0xffffffff );
 #endif
	m_oaCmd = OBJACT_NONE;		// CMD_ 시리즈를 거치지 않고 이것만 호출되는 경우는 Cmd를 해제시키자.
#endif // Client

	if( IsPlayer() )
	{
		TRACE( "SetDestObj %08x\n", objid );
	}
}

//
//
void CMover::SetDestPos( CONST D3DXVECTOR3 & vDestPos, bool bForward, BOOL fTransfer )
{
	m_vDestPos	= vDestPos;
#ifdef _DEBUG
	if( IsPlayer() )
	{
		int a = 0;
	}
#endif	
	
	D3DXVECTOR3	vPos	= GetPos();
	
	m_bForward	= bForward;

	m_bPositiveX	= ( vPos.x - vDestPos.x > 0 );
	m_bPositiveZ	= ( vPos.z - vDestPos.z > 0 );
	
	ClearDestObj();
#ifdef __CLIENT
	if( IsActiveMover() && fTransfer ) 
	{
		g_DPlay.PutPlayerDestPos( vDestPos, bForward );
	}
#endif
}

// IA오브젝트 위로 이동클릭을 했을때....
void CMover::SetDestPos( CShip *pIAObj, const D3DXVECTOR3 &vDestPos )
{
	m_pIADestPos = pIAObj;
	
	D3DXMATRIX mTrans, mInvIA, mLocal;
	D3DXMatrixTranslation( &mTrans, vDestPos.x, vDestPos.y, vDestPos.z );		// 월드좌표를 매트릭스로 변환
	D3DXMatrixInverse( &mInvIA, NULL, pIAObj->GetMatrixWorldPtr() );			// IAObj의 역행렬.
	D3DXMatrixMultiply( &mLocal, &mTrans, GetIAObjLink()->GetInvTM() );	// IA오브젝트로부터의 로컬 행렬.

	D3DXVECTOR3 vLocal;
	vLocal.x = mLocal._41;
	vLocal.y = mLocal._42;
	vLocal.z = mLocal._43;
	
	SetDestPos( vLocal, true, FALSE );	// do not transfer

#ifdef __IAOBJ0622
#ifdef __CLIENT
	if( IsActiveMover() )
	{
		g_DPlay.PutPlayerDestPos( vLocal, true, 0, pIAObj->GetId() );
	}
#endif	// __CLIENT
#endif	// __IAOBJ0622
}

FLOAT	CMover::GetAttackRange( DWORD dwAttackRange )
{
	switch( dwAttackRange )
	{
	case AR_SHORT:	return 2.0f;	
	case AR_LONG:	return 3.0f;	
	case AR_FAR:	return 4.0f;	
	case AR_RANGE:	return 10.0f;	
	case AR_WAND:	return 15.0f;	
#ifdef __ACROBAT_0504
	case AR_HRANGE: return 6.0f;
#endif
	default:		return 0.0f;	
	}
	return .0f;
}

//
// 외부에서 UseSkill을 명령할땐 이것으로 호출하자.
// sutType : 스킬을 사용할때 스킬큐에서 연타로 사용한건가 일반적인 사용을 한건가.c
//
int	CMover::CMD_SetUseSkill( OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType )
{
	m_oaCmd = OBJACT_NONE;
	if( m_pActMover->IsFly() )	return 0;				// 비행중엔 스킬사용 금지.
	if( m_pActMover->IsActAttack() )	return 0;
	if( m_pActMover->IsActJump() )	return 0;			// 점프중엔 사용금지.
	if( m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL )		return 0;	// 데미지 플라이중엔 스킬사용금지.
	if( IsDie() )	return 0;		// 죽었을때 사용금지.

	LPSKILL pSkill	= GetSkill( 0, nSkillIdx );		// this가 가진 스킬중 nIdx에 해당하는 스킬을 꺼낸다.
	if( pSkill == NULL )
	{
		Error( "CMD_SetUseSkill : %s skill(%d) not found", m_szName, nSkillIdx );
		return 0;	// 
	}

	ItemProp* pSkillProp = pSkill->GetProp();
	if( pSkillProp == NULL )		// JobSkill 리스트에서 꺼낸 스킬의 프로퍼티를 꺼냄.
	{
		Error( "CMD_SetUseSkill : %s. skill(%d) property not found", m_szName, pSkill->dwSkill );
		return 0;	// 
	}
	
#ifdef __S0504_FLY
	if( IsPlayer() && IsStateMode( STATE_BASEMOTION_MODE ) )	// 시전중(준비시간)일땐 사용금지.
	{
 #ifdef __CLIENT
		g_DPlay.SendStateModeCancel( STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL );
 #endif // __CLIENT
		return 0;		
	}
#endif // __S0504_FLY
	// 도달범위 - 얼마나 가까이 근접해야하는가. 미터단위
	float	fArrivalRange = 0.0f;		
	fArrivalRange = GetAttackRange( pSkillProp->dwAttackRange );
	
	switch( pSkillProp->dwUseChance )
	{
	case WUI_NOW:	// 타겟팅과 상관없이 자기자신에게 쓰는 방식.
		idTarget = GetId();
		break;
	case WUI_TARGETINGOBJ:	// 셀렉트 되어 있는 타겟에게 사용.
		{
#ifdef __CLIENT
		CObj *pFocusObj = GetWorld()->GetObjFocus();
		if( pFocusObj && pFocusObj->GetType() == OT_MOVER )
			idTarget = ((CMover*)pFocusObj)->GetId();
#else
 #ifdef __XSYNC0711
		if( IsPlayer() )
			idTarget = ((CUser *)this)->m_idSetTarget;
 #endif
#endif // __CLIENT
		}

		break;
#ifdef __CLIENT
	case WUI_TARGETCURSORPTZ:
		{
			idTarget = GetId();
			
			CRect rect;
			D3DXVECTOR3 vPos;
			CWndWorld* pWndWorld;
			
			pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			rect = pWndWorld->GetClientRect();

			if( GetWorld()->ClientPointToVector( NULL, rect, pWndWorld->GetMousePoint(), &GetWorld()->m_matProj, &GetWorld()->GetCamera()->m_matView, &vPos, TRUE ) )
			{
				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );		// UseSkill에서 사용한 스킬의 프로퍼티 꺼냄
				
				if( pAddSkillProp == NULL )
				{
					Error( "CMover::OnMagicSkill : %s. add스킬(%d)의 프로퍼티가 없다.", m_szName, nSkillIdx );
					return 0;	// property not found
				}
				
				FLOAT	fDist;
				FLOAT	fMaxDistSq;
				D3DXVECTOR3	vDist;
				fMaxDistSq  = (float)pAddSkillProp->dwSkillRange;
				fMaxDistSq *= fMaxDistSq;
				vDist = vPos - GetPos();
				fDist = D3DXVec3LengthSq( &vDist );	

				SetAngle( GetDegree(vPos, GetPos()) );		// 목표쪽으로 몸을 돌림.

				/*
				if( GetWorld()->IntersectObjLine( NULL, GetPos(), vPos, FALSE, FALSE ) )
				{
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
					g_WndMng.PutString( prj.GetText(TID_GAME_NOMOVING), NULL, prj.GetTextColor(TID_GAME_NOMOVING) );
					return 0;
				}
				*/
				
				// 텔레포트 할 위치가 멀경우 현제 스킬에 해당하는 거리로 바꿔준다
				if( fDist > fMaxDistSq )
				{
					FLOAT fLength;
					D3DXVECTOR3 vDirNor;
					D3DXVec3Normalize( &vDirNor, &vDist );
					fLength = (float)pAddSkillProp->dwSkillRange;
					float y = vPos.y;
					vPos = GetPos() + (vDirNor * fLength);
					vPos.y	= y;

					// 스킬에 해당하는 거리로 바꾼곳이 못가는 지역이라면 갈수 있는 지역을 검사한다.
					int nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );

					if( nAttr != HATTR_NONE ) 
					{
						while( nAttr != HATTR_NONE )
						{
							fLength -= 1.0f; // 1미터씩 줄여가며 계산한다.
							vPos  = GetPos() + (vDirNor * fLength);
							nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );
						}
						
						// 한번더 줄여줌
						fLength -= 1.0f; 
						vPos = GetPos() + (vDirNor * fLength);
					}
				}
				else // 텔레포트 할 위치가 해당스킬 거리보다 작을경우
				{
					int nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );

					FLOAT fLength;
					D3DXVECTOR3 vDirNor;
					D3DXVec3Normalize( &vDirNor, &vDist );
					fLength = 0.0f;
					//fLength = (float)fDist/5.0f;

					while( nAttr != HATTR_NONE )
					{
						fLength += 1.0f;
						vPos = GetPos() + (vDirNor * fLength);
						
						nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );
					}
/*
					// 이동할곳이 못가는 지역이라면 갈수있는 위치 계산...
					if( (nAttr == HATTR_NOWALK || nAttr == HATTR_NOFLY || nAttr == HATTR_NOMOVE) ) 
					{
						while( nAttr )
						{
							fLength -= 1.0f;
							vPos = GetPos() + (vDirNor * fLength);
							//FLOAT fy = GetWorld()->GetLandHeight_Fast( vPos.x, vPos.z );
							//vPos.y = fy;
							
							nAttr = GetWorld()->GetHeightAttribute( vPos.x, vPos.z );
						}

						// 한번더 줄여줌
						fLength -= 1.0f; 
						vPos = GetPos() + (vDirNor * fLength);
					}
*/
				}

#ifdef __SCHOOL0722
				if( IsActiveMover() && g_eLocal.GetState( EVE_SCHOOL ) )	// 학교이벤섭이면.
				{
					D3DXVECTOR3 v1, v2;
					v1 = GetPos();	v1.y += 0.1f;
					v2 = vPos;		v2.y += 0.1f;
					if( GetWorld()->IntersectObjLine( NULL, v1, v2, FALSE, FALSE ) )	// 텔레포트는 라인체크함.
					{
						g_WndMng.PutString( prj.GetText(TID_GAME_NOMOVING), NULL, prj.GetTextColor(TID_GAME_NOMOVING) );
						g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
						return 0;
					}
				}
#endif
				pWndWorld->m_vTelePos = vPos;
			}
			else
			{
				g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
				g_WndMng.PutString( prj.GetText(TID_GAME_NOMOVING), NULL, prj.GetTextColor(TID_GAME_NOMOVING) );
				return 0;
			}
		}
		break;
#endif // __CLIENT
	}

	// 타인에게 쓰는경우에만 검사...
	if( idTarget != GetId() )		
	{
		CMover *pTarget = prj.GetMover( idTarget );
		if( IsValidObj(pTarget) )
		{
			if( pSkillProp->nEvildoing < 0 )	// 나쁜 스킬은 
				if( IsAttackAble(pTarget) == FALSE )	// 공격허용이 되지 않으면 사용할 수 없음.
					return 0;
				
			if( pSkill->dwSkill == SI_ASS_HEAL_RESURRECTION )	// 부활을 사용했을때
			{
				if( pTarget->IsNPC() || pTarget->IsDie() == FALSE )		// 상대가 NPC거나 상대가 죽어있지 않다면 취소
					return 0;
			} else
			{
				if( pTarget->IsDie() )		// 부활이 아닌 스킬을 사용했을때 상대가 죽어있으면 사용안됨.
					return 0;
			}
		}
	}

	// 타겟 근접 방식.
	switch( pSkillProp->dwExeTarget )
	{
	case EXT_SELFCHGPARAMET:	// 시전자 자신에게 사용하는 종류
		idTarget = GetId();		// 타겟을 자기자신으로 설정.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정. << 이게 왜 필요하지? ㅡ.ㅡ?
		break;
	case EXT_MAGICATKSHOT:
	case EXT_MAGICATK:			// 원거리에서 마법으로 타겟을 공격
#ifdef __XSKILL0425
	case EXT_MAGICSHOT:
#endif
		if( idTarget == NULL_ID )	return 0;			// 타겟이 없거나 유효하지 않으면 실패.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정. 다가가는 범위는 fArrivalRange값으로..
		break;
	case EXT_ANOTHERWITH:		// 나 혹은 다른사람에게 시전
		if( idTarget == NULL_ID )	// 타겟이 잡혀있지 않으면
			idTarget = GetId();		// 자신을 타겟으로 잡음
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정.
		break;
	case EXT_AROUNDATK:			// 내 주위적들을 대상.
		idTarget = GetId();		// 타겟을 자기자신으로 설정.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정.
		break;
	case EXT_OBJCHGPARAMET:		// 타인에게 사용
	default:	// 그외는 모두 근접하자.
		if( idTarget == NULL_ID )	return 0;			// 타겟이 없거나 유효하지 않으면 실패.
		SetDestObj( idTarget, fArrivalRange, TRUE );		// 이동할 목표물을 idTarget으로 설정.
		break;
	}
	
	ClearActParam();
	SetCmd( OBJACT_USESKILL, nSkillIdx, idTarget, sutType );		// 사정거리가 되었을때 실행할 명령 셋팅.
	TRACE( "\nCMD_SetUseSkill\n" );

#ifdef __CLIENT
//	((CWndWorld *)g_WndMng.m_pWndWorld)->m_bAutoAttack = FALSE;		// 스킬사용하면 자동공격 멈춤.
#endif
//	DEBUG_CHATMSG( "스킬사용", NULL, 0xffffffff );

	return 1;
}

//OBJID g_idLastTarget = NULL_ID;
class CWndWorld;
extern int		g_nItemDelay;		// 땜빵.

void	CMover::CMD_SetMeleeAttack( OBJID idTarget, FLOAT fRange )
{
#if defined(__CLIENT ) && !defined(__ITEMCOOLTIME_VER2_050308) 
	if( g_nItemDelay > 0 )
		return;
#endif

	ItemProp *pProp = GetActiveHandItemProp();
	if( pProp->dwItemKind2 == IK2_TOOLS )			// 툴을 들고선 공격못한다,.
		return;

	CMover *pTarget = prj.GetMover( idTarget );
	if( IsValidObj( pTarget ) )
	{
	}
	if( m_pActMover->IsActAttack() )	return;
	if( IsDie() )	return;		// 죽었을때 사용금지.

#ifdef __ACROBAT_0504
	// 요요는 밀리어택이긴 하나 약간 거리를 두고 공격한다.( 프로퍼티 참조함 )
	if( pProp->dwItemKind3 == IK3_YOYO )
	{
		fRange = GetAttackRange( pProp->dwAttackRange );
	}
#endif //__ACROBAT_0504
	
#ifdef __CLIENT
	ClearActParam();
	SetDestObj( idTarget, fRange );		// 이동할 목표물을 idTarget으로 설정.
	SetCmd( OBJACT_MELEE_ATTACK, idTarget );		// 사정거리가 되었을때 실행할 명령 셋팅.
#elif defined(__WORLDSERVER)
	SetDestObj( idTarget, fRange );		// 이동할 목표물을 idTarget으로 설정.
//	g_DPCoreClient.SendMoverSetDestObj( this, idTarget, fRange );
	g_UserMng.AddMoverSetDestObj( this, idTarget, fRange );
#endif // WORLDSERVER
}

void	CMover::CMD_SetCollect( CObj *pObj )
{
	if( IsInvalidObj( pObj ) )	return;
	if( pObj->GetType() != OT_MOVER )	return;
	if( m_pActMover->IsActAttack() )	return;
	if( m_pActMover->IsActJump() )		return;
	if( m_pActMover->IsActDamage() )		return;
	if( IsDie() )	return;		// 죽었을때 사용금지.
	
	CMover *pTarget = (CMover *)pObj;
	ClearActParam();
	SetDestObj( pTarget->GetId(), 3.0f );		// 이동할 목표물을 idTarget으로 설정.
	SetCmd( OBJACT_COLLECT, pTarget->GetId() );		// 사정거리가 되었을때 실행할 명령 셋팅.
}
//
//
//
void	CMover::CMD_SetRangeAttack( OBJID idTarget, int nPower )
{
	TRACE("CMD_SetRangeAttack(%d, %d)\n", idTarget, nPower );

#ifdef __ACROBAT_0504
	if( m_pActMover->IsActAttack() )		// 이미 어택중이면 취소.
		return;

	if( IsDie() )	
		return;								// 죽었을때 사용금지.
	
	ItemProp *pItemProp = GetActiveHandItemProp();
	if( pItemProp == NULL )		
		return;
	
	if( pItemProp->dwItemKind3 != IK3_BOW )	// 들고있는 무기가 보우가 아니면 에러
		return;
	
	if( IsBullet( pItemProp ) == FALSE )
		return;
	
	ClearActParam();
	SetDestObj( idTarget, GetAttackRange( pItemProp->dwAttackRange ) );		// 이동할 목표물(idTarget)과 그 거리를 설정 
	SetCmd( OBJACT_RANGE_ATTACK, idTarget, nPower, 0 );		// 사정거리가 되었을때 실행할 명령 셋팅.
#endif //__ACROBAT_0504
}

void	CMover::CMD_SetMagicAttack( OBJID idTarget, int nMagicPower )
{
	TRACE("CMD_SetMagicAttack(%d, %d)\n", idTarget, nMagicPower );

	if( m_pActMover->IsActAttack() )		// 이미 어택중이면 취소.
		return;
	if( IsDie() )	return;		// 죽었을때 사용금지.
	
	// 손에 들고 있는 완드의 프로퍼티 꺼냄
	// 사정거리 계산함
	ItemProp *pItemProp = GetActiveHandItemProp();
	if( pItemProp == NULL )		return;

	if( pItemProp->dwItemKind3 != IK3_WAND /*&& pItemProp->dwItemKind3 != IK3_STAFF*/ )	//Staff->ata2k	// 들고있는 무기가 완드가 아니면 취소
		return;
	
	// 도달범위 - 얼마나 가까이 근접해야하는가. 미터단위
	float	fArrivalRange = GetAttackRange( AR_WAND );
	
	ClearActParam();
	SetDestObj( idTarget, fArrivalRange );		// 이동할 목표물을 idTarget으로 설정.
	SetCmd( OBJACT_MAGIC_ATTACK, idTarget, nMagicPower );		// 사정거리가 되었을때 실행할 명령 셋팅.
}

// vPos위치로 이동명령
void	CMover::CMD_SetMoveToPos( const D3DXVECTOR3 &vPos )
{
	SetDestPos( vPos );
}

// idObj위치로 이동
void	CMover::CMD_SetMoveToObj( OBJID idObj )
{
	SetDestObj( idObj );
}

void	CMover::CMD_SetUseItem( CCtrl *pCtrl )
{
	ClearActParam();
	if( m_pActMover->IsFly() )
		SetDestObj( pCtrl, 20.0f );
	else
		SetDestObj( pCtrl );
	SetCmd( OBJACT_USEITEM );		// 사정거리가 되었을때 실행할 명령 셋팅.
}



/*--------------------------------------------------------------------------------------------------------


											Mover act parameter
											

----------------------------------------------------------------------------------------------------------*/

void	CMover::SetActParam( OBJACT act, int nParam1, int nParam2, int nParam3 )
{
	m_oaAct = act;				
	m_nAParam[0] = nParam1;		// 명령 파라메터.
	m_nAParam[1] = nParam2;
	m_nAParam[2] = nParam3;
}


