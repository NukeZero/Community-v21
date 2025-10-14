#include "stdafx.h"
#include "defineText.h"
#include "defineSound.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "Sfx.h"
 
#ifdef __CLIENT
 #include "DPClient.h"
 #include "DialogMsg.h"
 extern	CDPClient	g_DPlay;
#else	// __CLIENT
 #ifdef __WORLDSERVER
  #include "User.h"
  #include "WorldMng.h"
  #include "DPSrvr.h"
  #include "DPCoreClient.h"
  #include "dpdatabaseclient.h"
  extern	CUserMng	g_UserMng;
  extern	CWorldMng	g_WorldMng;
  extern	CDPSrvr		g_DPSrvr;
  extern	CDPDatabaseClient	g_dpDBClient;
  extern	CDPCoreClient	g_DPCoreClient;
 #endif	// __WORLDSERVER
#endif	// __CLIENT

#include "party.h"
extern	CPartyMng	g_PartyMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __CLIENT

void CMover::PlayCombatMusic()
{
#ifdef __V041122_MUSIC

	#ifdef __GUILDCOMBAT
		if( GetWorld()->GetID() == WI_WORLD_GUILDWAR )
			return;
	#endif //__GUILDCOMBAT
	
	if( GetWorld()->m_bIsIndoor == FALSE )
	{
		FLOAT x = GetPos().x;
		FLOAT z = GetPos().z;
		CRect rectSaintmorning( 7535, 1846, 9283, 3134 );

		if( rectSaintmorning.PtInRect( CPoint( x, z ) ) )
			PlayMusic( BGM_BA_SAINTMORNING );
		else
		if( x < 4609 )
			PlayMusic( BGM_BA_DARKON );
		else
		if( x < 6309 )
			PlayMusic( BGM_BA_CRISIS );
		else
		if( x < 8138 )
			PlayMusic( BGM_BA_FLARIS );
		else
			PlayMusic( BGM_BA_SAINTMORNING );
	}
#endif
}
//
//
// 클라이언트용
void CMover::ProcessMoveArrival( CCtrl *pObj )
{
	// 클라이언트 처리
	if( IsActiveMover() )
	{
#ifdef __XUZHU
//		g_WndMng.PutString( "목표도착.", NULL, 0xffffffff );
#endif
		switch( m_oaCmd )	// 목표에 도착한 후의 명령 처리.
		{
		case OBJACT_USESKILL:
			if( pObj->GetType() == OT_MOVER )
			{
				CWorld *pWorld = GetWorld();
				D3DXVECTOR3 vStart = GetPos();			vStart.y += 0.5f;
				D3DXVECTOR3 vEnd   = pObj->GetPos();	vEnd.y += 0.5f;

				if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
					break;
				}


				//#ifdef __NEWMUSIC
#ifdef __V041122_MUSIC
				PlayCombatMusic();
#else
				if( IsRegionAttr( RA_DANGER ) )
					PlayMusic( BGM_BATTLE02 );
				else
					PlayMusic( BGM_BATTLE01 );
#endif
				int nSkillIdx = GetCmdParam(0);
				OBJID idTarget = (OBJID)GetCmdParam(1);
				SKILLUSETYPE sutType = (SKILLUSETYPE)GetCmdParam(2);
				if( (m_dwReqFlag & REQ_USESKILL) == 0 )	// 응답 요청중일땐 다시 보내선 안된다.
				{
					LPSKILL pSkill	= GetSkill( 0, nSkillIdx );		// this가 가진 스킬중 nIdx에 해당하는 스킬을 꺼낸다.
					if( pSkill == NULL )
					{
						Error( "CMD_SetUseSkill : %s skill(%d) not found", m_szName, nSkillIdx );
						return;	// skill not found
					}

					if( pSkill->dwSkill == SI_MAG_MAG_BLINKPOOL )
					{
						CWndWorld* pWndWorld;
						pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;		
						//if( prj.m_nLanguage == LANG_THA )	// 태국버전일땐 블링크풀 쓸때 교차검사한번함.
						{
							vStart = GetPos();		vStart.y += 1.0f;
							vEnd = pWndWorld->m_vTelePos;
							if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
							{
								g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
								g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
								break;
							}
						}
						
						if(g_pMoveMark!=NULL) g_pMoveMark->m_pSfxObj->m_nCurFrame=180;
						CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_MOVEMARK01,pWndWorld->m_vTelePos);
					}
					

#ifdef 	__ACROBAT_0504
					// 뒤에서 공격가능한 스킬인지 판단한다
					// 강탈 스킬은 뒤에서 사용가능(일단 클라에서 판정하자~)
					if( pSkill->GetProp() && pSkill->GetProp()->dwAtkStyle == AS_BACK )
					{						
						D3DXVECTOR3 v3Pos;
						D3DXVECTOR3 v3PosSrc;
						D3DXVECTOR3 v3PosDest;
						
						// 방향벡터 1
						v3PosSrc = pObj->GetPos() - GetPos();
						D3DXVec3Normalize( &v3PosSrc, &v3PosSrc );
						
						// 방향벡터 2
						AngleToVectorXZ( &v3Pos, pObj->GetAngle(), 3.0f );
						v3PosDest = (pObj->GetPos()+v3Pos) - pObj->GetPos();
						D3DXVec3Normalize( &v3PosDest, &v3PosDest );
						
						FLOAT fDir = D3DXVec3Dot( &v3PosSrc, &v3PosDest );

						// 뒤가 아니면 스킬 사용 불가!
						if( fDir < 0.3f )
						{
							g_WndMng.PutString( prj.GetText(TID_GAME_NEVERKILLSTOP) );
							break;
						}
					}
#endif //__ACROBAT_0504
					
					TRACE( "OBJACT_USESKILL %d\n", nSkillIdx );
					g_DPlay.SendUseSkill( 0, nSkillIdx, idTarget, sutType );	// 목표지점에 도착하면 스킬쓴다고 알림.
#ifdef __XUZHU
					g_WndMng.PutString( "스킬전송", NULL, 0xffffffff );
#endif
					m_dwReqFlag |= REQ_USESKILL;	// 응답 요청중
					
				}
				ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
				SendActMsg( OBJMSG_STOP );
//				DoUseSkill( 0, nSkillIdx, idTarget );	// 선실행
//				// DestObj는 클리어 하지 않았으므로 서버에서 응답이 올때까지 목표를 여전히 추적하고 있다.
//				// 서버에서 스킬을 쓰라고 응답이 올때 추적을 멈추자.
			}
			break;
		//------------------------------------------
		case OBJACT_MELEE_ATTACK:
			if( pObj->GetType() == OT_MOVER )
			{
#ifdef 	__ACROBAT_0504
				ItemProp *pItemProp = GetActiveHandItemProp(); 
				if( pItemProp && pItemProp->dwItemKind3 == IK3_YOYO )
				{
					CWorld *pWorld = GetWorld();
					D3DXVECTOR3 vStart = GetPos();			vStart.y += 0.5f;
					D3DXVECTOR3 vEnd   = pObj->GetPos();	vEnd.y += 0.5f;
					
					if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_BLOCKTARGETING ), NULL, prj.GetTextColor( TID_GAME_BLOCKTARGETING ) );
						break;
					}
				}
#endif //__ACROBAT_0504
				
				DoAttackMelee( (CMover *)pObj );		// pObj를 일반공격.
			}
			break;
		//---------------------------------------------
		case OBJACT_MAGIC_ATTACK:
			if( pObj->GetType() == OT_MOVER )
			{
//#ifdef __NEWMUSIC
#ifdef __V041122_MUSIC
				PlayCombatMusic();
#else
				if( IsRegionAttr( RA_DANGER ) )
					PlayMusic( BGM_BATTLE02 );
				else
					PlayMusic( BGM_BATTLE01 );
#endif
//#endif
				OBJID	idTarget = GetCmdParam(0);
				int		nMagicPower = GetCmdParam(1);
				CMover *pTarget = prj.GetMover( idTarget );		// 타겟의 아이디를 포인터로 읽음.
				if( IsInvalidObj(pTarget) )		break;			// 타겟이 거시기한 포인터면 취소시킴.
				
				SendActMsg( OBJMSG_STAND );
				ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
				DoAttackMagic( pTarget, nMagicPower );
			}
			break;
		case OBJACT_RANGE_ATTACK:
			{
#ifdef __ACROBAT_0504
				if( pObj->GetType() == OT_MOVER )
				{
	#ifdef __V041122_MUSIC
					PlayCombatMusic();
	#else
					if( IsRegionAttr( RA_DANGER ) )
						PlayMusic( BGM_BATTLE02 );
					else
						PlayMusic( BGM_BATTLE01 );
	#endif	// __V041122_MUSIC
					OBJID	idTarget = GetCmdParam(0);
					int		nPower = GetCmdParam(1);
					CMover *pTarget = prj.GetMover( idTarget );		// 타겟의 아이디를 포인터로 읽음.
					if( IsInvalidObj(pTarget) )		break;			// 타겟이 거시기한 포인터면 취소시킴.
					
					SendActMsg( OBJMSG_STAND );
					ClearDestObj();									// 목표에 도달하면 추적을 멈춤.

					DoAttackRange( pTarget, nPower, 0 );			// nPower를 dwItemID에 넣는다.
				}				
#endif //__ACROBAT_0504
			}
			break;
		//---------------------------------------------
		case OBJACT_USEITEM:
			ClearDestObj();	// 그외는 목표에 도착하면 멈춤.
			SendActMsg( OBJMSG_STAND );
			SetAngle( GetDegree(pObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
			break;
		//---------------------------------------------
		case OBJACT_COLLECT:
			ClearDestObj();	// 그외는 목표에 도착하면 멈춤.
			SendActMsg( OBJMSG_STOP );
			SetAngle( GetDegree(pObj->GetPos(), GetPos()) );		// 목표쪽으로 몸을 돌림.
#ifdef __ONLINE
			g_DPlay.SendDoCollect( pObj );						// 서버로 보냄.
#endif 
			break;
		//---------------------------------------------
		default:
			ClearDestObj();	// 그외는 목표에 도착하면 멈춤.
			SendActMsg( OBJMSG_STOP );
			break;
			
		}
		SetCmd( OBJACT_NONE );
	}
	else
	{
		ClearDestObj();	// 그외는 목표에 도착하면 멈춤.
		SendActMsg( OBJMSG_STOP );
#ifdef __POSTAIMSG1008
		PostAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
#else	// __POSTAIMSG1008
		SendAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
#endif	// __POSTAIMSG1008
		g_DPlay.SendQueryGetDestObj( this );
	}
}
#else // Client
//
//
// 월드서버용
void CMover::ProcessMoveArrival( CCtrl *pObj )
{
#ifdef __XSYNC0711
	switch( m_oaCmd )	// 목표에 도착한 후의 명령 처리.
	{
	case OBJACT_USESKILL:
		if( pObj->GetType() == OT_MOVER )	// 타겟이 무버일때면 처리함.
		{
			int nSkillIdx = GetCmdParam(0);
			OBJID idTarget = (OBJID)GetCmdParam(1);
			SKILLUSETYPE sutType = (SKILLUSETYPE)GetCmdParam(2);

			LPSKILL pSkill	= GetSkill( 0, nSkillIdx );		// this가 가진 스킬중 nIdx에 해당하는 스킬을 꺼낸다.
			if( pSkill == NULL )
			{
				Error( "CMD_SetUseSkill : %s는 nIdx(%d)에 스킬을 가지고 있지 않다. ", m_szName, nSkillIdx );
				return;	// skill not found
			}

			if( pSkill->dwSkill == SI_MAG_MAG_BLINKPOOL )
				return;		// 아직 서버명령으로는 블링크풀 사용못함.

			CWorld *pWorld = GetWorld();
			D3DXVECTOR3 vStart = GetPos();			vStart.y += 0.5f;
			D3DXVECTOR3 vEnd   = pObj->GetPos();	vEnd.y += 0.5f;

			if( pWorld->IntersectObjLine( NULL, vStart, vEnd, FALSE, FALSE ) )	// 시전자와 타겟사이 장애물을 검사.
			{
				if( IsPlayer() )
					((CUser *)this)->AddDefinedText( TID_GAME_BLOCKTARGETING, "" );
//				g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );  << 이건 어찌처리해야할까. 클라에도 보내야 할까.
				break;
			}

			TRACE( "OBJACT_USESKILL %d\n", nSkillIdx );
			BOOL bSuccess = DoUseSkill( 0, nSkillIdx, idTarget, sutType );		// 목표지점에 도착하면 스킬 사용시작.
			if( bSuccess == FALSE )
				if( IsPlayer() )
					((CUser *)this)->m_playTaskBar.OnEndSkillQueue( (CUser *)this );

			ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
			SendActMsg( OBJMSG_STOP );
		}
		break;
	default:
		SendActMsg( OBJMSG_STOP );
		ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
		#ifdef __POSTAIMSG1008
		PostAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
		#else	// __POSTAIMSG1008
		SendAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
		#endif	// __POSTAIMSG1008
	} // switch

	SetCmd( OBJACT_NONE );
	
	if( IsPlayer() )
		((CUser*)this)->AddQueryGetDestObj( NULL_ID );
#else // XSYNC0711
	SendActMsg( OBJMSG_STOP );
	ClearDestObj();		// 목표에 도달하면 추적을 멈춤.
	#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
	#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
	#endif	// __POSTAIMSG1008
	if( IsPlayer() )
		((CUser*)this)->AddQueryGetDestObj( NULL_ID );
#endif // not xSync0711
}
#endif // not CLIENT

// 이 함수.. 오브젝트 서브 클래스로 분리 예정.. 
void CMover::ProcessMove()
{
#ifndef __BEAST
	if( m_pActMover->IsSit() )	return;

	EnforcedGhostCorr();	// 고스트의 강제 동기가 필요하다면 실행
	ApproachGhostAngle();	// 고스트의 목표 각도로의 점진적인 각도의 변경
#ifdef _DEBUG
	if( IsPlayer() )
	{
//		TRACE( "%d, %f, %f, %f\n", GetWorld()->GetID(), GetPos().x, GetPos().z, GetPos().y );
		int a = 0;
	}
#endif

	if( IsEmptyDest() )
	{
		return;
	}
//	if( m_pActMover->IsActJump() )
//		return;
	if( m_pActMover->IsActAttack() )
		return;
	
	D3DXVECTOR3 vPos = GetPos(); 
	D3DXVECTOR3 vDestPos	= m_vDestPos; 

	if( !IsEmptyDestPos() )	// 좌표
	{
		if( ( ( vPos.x - m_vDestPos.x > 0 ) != m_bPositiveX
			|| ( vPos.z - m_vDestPos.z > 0 ) != m_bPositiveZ ) ) {
			OnArriveAtPos();	// 좌표에 도착했을 때의 처리
			return;
		}
	}
	else	// 오브젝트
	{
		CCtrl* pObj = prj.GetCtrl( m_idDest );
		if( pObj == NULL )
		{
			SendActMsg( OBJMSG_STAND );
			return;
		}
		vDestPos = pObj->GetPos(); 
		if( m_pActMover->IsFly() )
		{
			{
				BOOL bRangeObj = pObj->IsRangeObj( this, m_fArrivalRange );
				if( bRangeObj == TRUE )
				{
					ClearDestObj();	// 그외는 목표에 도착하면 멈춤.
#ifdef __WORLDSERVER
#ifdef __POSTAIMSG1008
					PostAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
#else	// __POSTAIMSG1008
					SendAIMsg( AIMSG_ARRIVAL, pObj->GetId() );
#endif	// __POSTAIMSG1008
#endif	// __WORLDSERVER
				}
			}
		}
		else 
		{
			BOOL bRangeObj = pObj->IsRangeObj( this, m_fArrivalRange );
			// 3D 충돌에 실패했지만
			if( bRangeObj == TRUE )
			{
				ProcessMoveArrival( pObj );
				return;		
			} else
			{
				int a = 0;
			}
		}
	}
//	if( IsPlayer() && m_pActMover->IsActDamage() )	// 플레이어가 아닌 몬스터는 데미지중에도 걸어갈수 있게 했다.
//		return;
	// 공중 추적 
	if( m_pActMover->IsFly() )
	{
		if( m_uRemnantCorrFrm > 0 ) {
			D3DXVECTOR3 v	= vDestPos - vPos;
			m_pActMover->m_fDistance	= D3DXVec3Length( &v );
			SendActMsg( OBJMSG_TURNMOVE, (int)GetDegree( vDestPos, vPos ), (int)GetDegreeX( vDestPos, vPos ), 0 );
//			TRACE( "%d\n", (int)GetDegreeX( vDestPos, vPos ) );
			m_uRemnantCorrFrm--;
		}
//		else
//		{
//			SendActMsg( OBJMSG_TURNMOVE, (int)GetDegree( vDestPos, vPos ), (int)GetDegreeX( vDestPos, vPos ), 1 );
//		}
	}
	else
	{
		if( m_uRemnantCorrFrm > 0 )
			m_uRemnantCorrFrm--;

		if( m_bForward ) {
			SendActMsg( OBJMSG_TURNMOVE, (int)GetDegree( vDestPos, vPos ), (int)GetDegreeX( vDestPos, vPos ) );
		}
		else {
			SendActMsg( OBJMSG_TURNMOVE2, (int)GetDegree( vPos, vDestPos ),  (int)GetDegreeX( vPos, vDestPos ) );
		}
	}
#endif  // not beast
}  // ProcessMove()

// 패턴이동중.
#ifdef __XFLYAI0524
void	CMover::ProcessMovePattern( void )
{
	if( m_nMovePattern == 1 )	// 8자이동 패턴.
	{
		switch( m_nMoveEvent )
		{
		case 0:			
			m_nMoveEvent ++;
			m_nMoveEventCnt = 0;
			// break;		// break 넣지 말것.
		case 1:		// S - 1 구간 : n초간 좌회전중.
			SendActMsg( OBJMSG_FORWARD );
			if( (m_nMoveEventCnt & 3) == 0 )
				SendActMsg( OBJMSG_LTURN );
			else
				SendActMsg( OBJMSG_STOP_TURN );
			
			if( ++m_nMoveEventCnt > SEC1 * 2 )
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
//				SendActMsg( OBJMSG_STOP );
			}
			break;
		case 2:		// 1 - 2구간 : n초간 우회전하며 상승했다 하강.
			SendActMsg( OBJMSG_FORWARD );
			if( (m_nMoveEventCnt & 3) == 0 )
				SendActMsg( OBJMSG_RTURN );
			else
				SendActMsg( OBJMSG_STOP_TURN );
			if( m_nMoveEventCnt < (SEC1 * 5) / 2 )
			{
				SendActMsg( OBJMSG_LOOKUP );
			} else
			if( m_nMoveEventCnt == (SEC1 * 5) / 2 )
			{
				SendActMsg( OBJMSG_STOP_LOOK );
				SetAngleX( 0 );		// 수평으로 맞춤.
			} else
			{
				SendActMsg( OBJMSG_LOOKDOWN );
			}

			if( ++m_nMoveEventCnt > SEC1 * 5 )
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SendActMsg( OBJMSG_STOP );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
		case 3:		// 2 - 3구간 : 우회전 하다가 좌회전.
			SendActMsg( OBJMSG_FORWARD );
			if( m_nMoveEventCnt < (SEC1 * 5) / 2 )		// 
			{
				if( (m_nMoveEventCnt & 3) == 0 )
					SendActMsg( OBJMSG_RTURN );
				else
					SendActMsg( OBJMSG_STOP_TURN );
			} else
			{
				if( (m_nMoveEventCnt & 3) == 0 )
					SendActMsg( OBJMSG_LTURN );
				else
					SendActMsg( OBJMSG_STOP_TURN );
			}
				
			if( ++m_nMoveEventCnt > SEC1 * 5 )
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
		case 4:	// 3 - S구간 : 좌회전하면서 하강하다 상승
			SendActMsg( OBJMSG_FORWARD );
			if( (m_nMoveEventCnt & 3) == 0 )
				SendActMsg( OBJMSG_LTURN );
			else
				SendActMsg( OBJMSG_STOP_TURN );
			if( m_nMoveEventCnt < (SEC1 * 5) / 2 )
			{
				SendActMsg( OBJMSG_LOOKDOWN );
			} else
			if( m_nMoveEventCnt == (SEC1 * 5) / 2 )
			{
				SendActMsg( OBJMSG_STOP_LOOK );
				SetAngleX( 0 );		// 수평으로 맞춤.
			} else
			{
				SendActMsg( OBJMSG_LOOKUP );
			}
				
			if( ++m_nMoveEventCnt > SEC1 * 5 )
			{
				m_nMoveEventCnt = 0;
				m_nMoveEvent = 99;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SendActMsg( OBJMSG_STOP );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
				
		case 99:
			break;
		}
	} else	// movePattern 1
	if( m_nMovePattern == 2 )		// 비행 패턴 2
	{
		switch( m_nMoveEvent )
		{
		case 0:			
			m_nMoveEvent ++;
			m_nMoveEventCnt = 0;
			// break;		// break 넣지 말것.
		case 1:		// S - 1 구간중 직진 코스
			SendActMsg( OBJMSG_FORWARD );
			
			if( ++m_nMoveEventCnt > SEC1 )		// 1초가 지나면 급 우회전.
			{
				FLOAT fAngle = GetAngle();
				SetAngle( fAngle + 135.0f );
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
			}
			break;
		case 2:		// 1구간중 우/하로 이동
			SendActMsg( OBJMSG_FORWARD );

			if( ++m_nMoveEventCnt > SEC1 * 2 )		// 2초간 직진하다가 다시 좌로 90도 회전.
			{
				FLOAT fAngle = GetAngle();
				SetAngle( fAngle - 90.0f );
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SendActMsg( OBJMSG_STOP );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
		case 3:		// 1구간중 3번째 구간
			SendActMsg( OBJMSG_FORWARD );
				
			if( ++m_nMoveEventCnt > SEC1 * 2 )
			{
				FLOAT fAngle = GetAngle();
				SetAngle( fAngle - 45.0f );
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
		case 4:	// 2-1 구간
			SendActMsg( OBJMSG_FORWARD );
				
			if( ++m_nMoveEventCnt > SEC1 * 1 )
			{
				FLOAT fAngle = GetAngle();
				SetAngle( fAngle - 45.0f );
				m_nMoveEventCnt = 0;
				m_nMoveEvent ++;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SendActMsg( OBJMSG_STOP );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
		case 5:	// 2-2 구간
			SendActMsg( OBJMSG_FORWARD );
			
			if( ++m_nMoveEventCnt > SEC1 * 3 )
			{
				FLOAT fAngle = GetAngle();
				SetAngle( fAngle + 45.0f );
				m_nMoveEventCnt = 0;
				m_nMoveEvent = 99;
				SendActMsg( OBJMSG_STOP_LOOK );
				SendActMsg( OBJMSG_STOP_TURN );
				SendActMsg( OBJMSG_STOP );
				SetAngleX( 0 );		// 수평으로 맞춤.
			}
			break;
			
		case 99:
			break;
		}
	} // MovePattern 2
}
#endif // xFlyAI0524