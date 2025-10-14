// Billboard.cpp : implementation of the CBeastView class
//

#include "stdafx.h"

#include "AIMonster.h"
#include "lang.h"
#include "defineObj.h"
#include "dialogMsg.h"
#ifndef __WORLDSERVER
#include "DialogMsg.h"
extern CDialogMsg g_DialogMsg;
#else
#include "User.h"
#include "dpcoreclient.h"
extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
#endif

#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __ACROBAT_0504
#include "DefineSkill.h"
#endif //__ACROBAT_0504


#define RANGE_CANCEL 60.0f
#define RANGE_MOVE   15.0f
#define RANGE_ATTACK 25.0f

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_WANDER,
	STATE_PURSUE,
	STATE_EVADE,
	STATE_RUNAWAY,
	STATE_RAGE
};
BEGIN_AISTATE_MAP( CAIMonster, CAIInterface )

	ON_STATE( STATE_INIT   , StateInit   )
	ON_STATE( STATE_IDLE   , StateIdle   )
	ON_STATE( STATE_WANDER , StateWander )
	//ON_STATE( STATE_PURSUE , StatePursue )
	ON_STATE( STATE_EVADE  , StateEvade  )
	ON_STATE( STATE_RAGE   , StateRage   )
	ON_STATE( STATE_RUNAWAY, StateRunaway  )

END_AISTATE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
MemPooler<CAIMonster>*	CAIMonster::m_pPool		= new MemPooler<CAIMonster>( 512 );
#else	// __WORLDSERVER
 #ifndef __ONLINE
 MemPooler<CAIMonster>*	CAIMonster::m_pPool		= new MemPooler<CAIMonster>( 128 );
 #else	// not Online(Single)
 MemPooler<CAIMonster>*	CAIMonster::m_pPool		= NULL; //new MemPooler<CAIMonster>( 128 );
 #endif // onLine
#endif	// __WORLDSERVER

static int s_tmAttack = SEC(20);		//  m_timerAttack, 한국은 5초 일본은 10초
static int s_tmTrace = SEC(10);		//  m_timerAttack, 한국은 5초 일본은 10초

CAIMonster::CAIMonster()
{
	m_dwIdTarget = NULL_ID;
//	m_dwMoveState = 0;
	m_bReturnToBegin = FALSE;
	m_bGoTarget = FALSE;
	m_bTargetNoMovePos = FALSE;
	m_dwAtkMethod = 0;
	m_nCollisionCount = 0;
	m_bFirstRunaway = FALSE;
	m_timerAttack.Set( s_tmAttack );		// 공격 후 10초간 추적해옴.
	m_timerTrace.Set( s_tmTrace );		// 추적한지 15초가 지나면 추적 포기.
	//m_timerValidAttack.Set( SEC( 3 ) );
	m_vOldPos.x = m_vOldPos.y = m_vOldPos.z = 0;
	m_bRangeAttack = FALSE;
#ifdef __XAI1013
	memset( m_idSummon, NULL_ID, sizeof(m_idSummon) );
#endif
	m_tmSummon = m_tmHelp = 0;
	m_bLootMove = FALSE;
	m_idLootItem = NULL_ID;
	m_bRangeOut = FALSE;
	m_tmReturnToBegin = timeGetTime();
}
CAIMonster::CAIMonster( CObj* pObj ) : CAIInterface( pObj, 0 )
{
	m_dwIdTarget = NULL_ID;
//	m_dwMoveState = 0;
	m_bReturnToBegin = FALSE;
	m_bGoTarget = FALSE;
	m_bTargetNoMovePos = FALSE;
	m_dwAtkMethod = 0;
	m_nCollisionCount = 0;
	m_bFirstRunaway = FALSE;
	m_bCallHelper = FALSE;
	m_timerAttack.Set( s_tmAttack );		// 공격 후 10초간 추적해옴.
	m_timerTrace.Set( s_tmTrace );		// 추적한지 15초가 지나면 추적 포기.
	//m_timerValidAttack.Set( SEC( 3 ) );
	m_vOldPos.x = m_vOldPos.y = m_vOldPos.z = 0;
	m_bRangeAttack = FALSE;
#ifdef __XAI1013
	memset( m_idSummon, NULL_ID, sizeof(m_idSummon) );
#endif
	m_tmSummon = m_tmHelp = 0;
	m_bLootMove = FALSE;
	m_idLootItem = NULL_ID;
	m_bRangeOut = FALSE;
	m_tmReturnToBegin = timeGetTime();
}

CAIMonster::~CAIMonster()
{
}

void CAIMonster::InitAI()
{
	m_vPosBegin = GetMover()->GetPos();
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_SETSTATE, STATE_IDLE ); 
#else	// __POSTAIMSG1008
	SendAIMsg( AIMSG_SETSTATE, STATE_IDLE ); 
#endif	// __POSTAIMSG1008
}

// idTarget을 공격하게 시킨다. 외부에서 사용.
void CAIMonster::SetAttackTarget( OBJID idTarget ) 
{
	m_dwIdTarget = idTarget; 
	SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
}


void CAIMonster::MoveToDst(	D3DXVECTOR3 vDst )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	//pMover->SetMotion( MTI_WALK );		
	pMover->SetDestPos( vDst );
	pMover->m_nCorr		= -1;
#ifdef __WORLDSERVER
//	g_DPCoreClient.SendMoverSetDestPos( pMover, vDst, 1 );
	g_UserMng.AddSetDestPos( pMover, vDst, 1 );
#endif	// __WORLDSERVER
#ifdef __CLIENT
	// 이동할 좌표의 랜드가 있는가?
	//if( pWorld->GetLandscape( vDst ) )
#endif
	//	SetMove(); // 이동 상태로 세팅 
}
void CAIMonster::MoveToDst(	OBJID dwIdTarget )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestId() == dwIdTarget )
		return;
	pMover->SetDestObj( dwIdTarget ); // 목표를 재설정해준다.
#ifdef __WORLDSERVER
//	g_DPCoreClient.SendMoverSetDestObj( pMover, dwIdTarget, 0 );
	g_UserMng.AddMoverSetDestObj( (CMover*)pMover, dwIdTarget );
#endif	// __WORLDSERVER
	//pMover->SetMotion( MTI_WALK );		
//	SetMove(); // 이동 상태로 세팅 
}
/*
void CAIMonster::SetDst( OBJID dwIdTarget )
{
	CMover* pMover = GetMover();
	pMover->SetDestObj( dwIdTarget ); // 목표를 재설정해준다.
#ifdef __WORLDSERVER
	g_DPCoreClient.SendMoverSetDestObj( pMover, dwIdTarget, 0 );
	g_UserMng.AddMoverSetDestObj( (CMover*)pMover, dwIdTarget );
#endif	// __WORLDSERVER
}
*/
void CAIMonster::MoveToRandom( UINT nState )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

//	pMover->SetMotion( MTI_WALK );		
	D3DXVECTOR3 vPos = pMover->GetPos();
	DWORD x1 = xRand();
	DWORD x2 = xRand();

	vPos.x += int( x1 % 21 ) - 10;
	vPos.z += int( x2 % 21 ) - 10;

	float xDist = fabs( vPos.x - m_vPosBegin.x );
	float zDist = fabs( vPos.z - m_vPosBegin.z );
	float fRadiusCur = sqrt( xDist * xDist + zDist * zDist );
#ifdef _DEBUG
	DWORD dwId = 10142;

	if( pMover->GetId() == dwId )
	{
		int a = 0;
	}
#endif

	if( nState == STATE_IDLE )
	{
		int nRangeMove = RANGE_MOVE;
#ifdef __XFLYAI0524
		if( pMover->IsFlyingNPC() )		// 비행몹의 경우 거리 3배
			nRangeMove *= 3.0f;
#endif
		
		if( fRadiusCur < nRangeMove )  // 10미터 반경을 벗어나지 못함 
		{
			MoveToDst( vPos );
		}
		else
		{
			// 집에가는 중에 랜덤호출이면 충돌회피다. 회피하자.
			if( m_bReturnToBegin == TRUE )
				MoveToDst( vPos );
			else
			{
				// 만약 내가 이미 스폰 지역을 한참 벗어나있다면 돌아가도록 시도.
				D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
				if( IsInRange( vTargetTo, nRangeMove ) == FALSE )
					DoReturnToBegin();
			}
		}
	}
	else
		MoveToDst( vPos );
}
BOOL CAIMonster::IsInRange( D3DXVECTOR3 vDistant, FLOAT fRange )
{
#ifndef __XAI1013
	vDistant.y = 0;
#endif // not xAI1013
	float fDistSq = D3DXVec3LengthSq( &vDistant );	
	if( fDistSq > fRange * fRange )	
		return FALSE;
	return TRUE;
}

// 근처의 아이템을 집으러 간다.
BOOL CAIMonster::SubItemLoot( void )
{
#ifdef __XAI1013
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	D3DXVECTOR3 vPos = pMover->GetPos();
	CObj *pObj = NULL;
	int nRange = 0;
	D3DXVECTOR3 vDist;
	FLOAT fDistSq, fMinDist = 9999999.0f;
	CObj *pMinObj = NULL;

	// 근처의 아이템을 검색함.
	FOR_LINKMAP( pWorld, vPos, pObj, nRange, CObj::linkDynamic )
	{
		if( pObj->GetType() == OT_ITEM )
		{
			ItemProp* pItemProp	= ( (CItem*)pObj )->GetProp();
			if( pItemProp && pItemProp->dwItemKind3 != IK3_QUEST && pItemProp->dwItemKind1 != IK1_GOLD )
			{
				vDist = pObj->GetPos() - pMover->GetPos();
				fDistSq = D3DXVec3LengthSq( &vDist );		// 거리 구함.
				if( fDistSq < 10 * 10 && fDistSq < fMinDist )	// 10미터 이내고... 가장 거리가 가까운 아템을 찾음.
					pMinObj = pObj;
			}
		}
	}
	END_LINKMAP

	if( pMinObj )
	{
		MoveToDst( pMinObj->GetPos() );		// 목표쪽으로 이동.
		m_idLootItem = ((CItem *)pMinObj)->GetId();
		m_bLootMove = TRUE;
#ifdef __WORLDSERVER
		g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_LOOT );
#endif
		
	}
		
#endif// xAI1013
	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIMonster::StateInit( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIMonster::StateInit()
#endif	// __POSTAIMSG1008
{
	return TRUE;
}

// 태어난 지점으로 돌아가라.
void CAIMonster::DoReturnToBegin( void )
{
	m_bReturnToBegin = TRUE;
	m_tmReturnToBegin = timeGetTime();		// 돌아가라는 명령을 받은 시간 기록.
	MoveToDst( m_vPosBegin );
}

#ifdef __POSTAIMSG1008
BOOL CAIMonster::MoveProcessIdle( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIMonster::MoveProcessIdle()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	ADDERRORSEQ( _i, 1000201 );
	
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

#ifdef __CLIENT
	if( pWorld->GetLandscape( pMover->GetPos() ) == NULL )
		return FALSE;
#endif
	
	ADDERRORSEQ( _i, 1000202 );
	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	ADDERRORSEQ( _i, 1000203 );
	// 스폰 장소로 귀환 
	if( m_bReturnToBegin )
	{
		ADDERRORSEQ( _i, 1000204 );
		if( IsInRange( m_vPosBegin - pMover->GetPos(), 5.0f ) )		// 시작지점으로 돌아왔다.
		{
			ADDERRORSEQ( _i, 1000205 );
			m_dwIdTarget = NULL_ID;
			m_bReturnToBegin = FALSE;
			SendAIMsg( AIMSG_ARRIVAL, NULL_ID );
			ADDERRORSEQ( _i, 1000206 );
		}
		else
		if( m_dwIdTarget != NULL_ID )	// 놀고있던 중에 전에 쫒던 적이 있으면 재공격
		{
			if( ((int)g_tmCurrent - (int)m_tmReturnToBegin) > SEC(5) )		// 돌아가기 시작한지 5초는 지나야 다시 반격한다.
			{
				ADDERRORSEQ( _i, 1000210 );
				BOOL bAble = FALSE;
				int nAttackFirstRange = pMover->GetProp()->m_nAttackFirstRange;		//??
				ADDERRORSEQ( _i, 1000211 );
				CMover* pTarget = prj.GetMover( m_dwIdTarget );
				if( IsValidObj(pTarget) )
				{
					ADDERRORSEQ( _i, 1000212 );
					D3DXVECTOR3 vDist = pTarget->GetPos() - pMover->GetPos();
					FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
					ADDERRORSEQ( _i, 1000213 );
					if( fDistSq < (float)(nAttackFirstRange * nAttackFirstRange) )	// 타겟이 거리내에 들어오면
					{
						// this랑 타겟이랑 지상-지상, 비행-비행 이면 
						if( pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly() )	
							bAble = TRUE;	// 다시 공격
					}
					ADDERRORSEQ( _i, 1000214 );
				}			
				ADDERRORSEQ( _i, 1000215 );
				if( bAble )
					SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
				ADDERRORSEQ( _i, 1000216 );
			}
		}			
//		else
//		if( IsEndStop() )		<<- 버그발생의 위험이 있어 뺌.
		{
//			ADDERRORSEQ( _i, 1000207 );
//			float fDegree = GetDegree( m_vPosBegin, pMover->GetPos() );
//			ADDERRORSEQ( _i, 1000208 );
//			D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fDegree, 10.0f );
//			MoveToDst( vDst );
//			MoveToDst( m_vPosBegin );		// 태어난 지점으로 바로 뛰어간다.
//			ADDERRORSEQ( _i, 1000209 );
		} 

//#ifdef __XAI0419
		if( timeGetTime() - m_tmReturnToBegin >= SEC(10) )		// 집으로 돌아가기 시작한지 10초나 지났다.
		{
			// 제대로 못돌아가고 있다고 판단하고 시작지점으로 워프시킴.
			pMover->SetPos( m_vPosBegin );
#ifdef __WORLDSERVER			
			g_UserMng.AddSetPos( pMover, m_vPosBegin );
#endif
			m_tmReturnToBegin = timeGetTime();
		}
//#endif
	}
	else
	{
		ADDERRORSEQ( _i, 1000217 );
#ifdef __XFLYAI0524
		if( pMover->GetMovePattern() == 0 )	// 일반 패턴이동일때만 들어간다.
#endif
		{
			if( IsEndStop() )		// 제자리 대기 시간이 끝났으면
			{
				ADDERRORSEQ( _i, 1000218 );
				MoveToRandom( STATE_IDLE );		// 다음 목표를 지정.
				ADDERRORSEQ( _i, 1000219 );
			}
			// 아닌중에는 목표쪽으로 계속 이동중이다.
			ADDERRORSEQ( _i, 1000220 );
		}
#ifdef __XFLYAI0524
		else
		if( pMover->GetMovePattern() /*== 1*/ )	// 8자비행중
		{
			ADDERRORSEQ( _i, 1000221 );
			if( pMover->GetMoveEvent() == 99 )		// 패턴비행 끝.
			{
				ADDERRORSEQ( _i, 1000222 );
				pMover->SetMovePattern( 0 );		// 일반이동으로 전환.
				ADDERRORSEQ( _i, 1000223 );
			}
			ADDERRORSEQ( _i, 1000224 );
		}
#endif // xFlyAI0524

#ifdef __XAI1013
		// 루팅옵션이 있는 몹이다.
		if( pProp->m_nLoot )
		{
			if( (pMover->GetCount() & 63) == 0 )		// 가끔씩 주변을 스캔해서.(63이면 약 4.2초)
			{	
				if( pProp->m_nLootProb == 0 || xRandom(100) < pProp->m_nLootProb  )	// 디폴트값(0)은 100% / 이거나 루팅확률에 들어가면 루팅시도
					if( m_bLootMove == FALSE )		// 루팅하러 가고 있을때 또 체크하면 안뒘
						SubItemLoot();		// 아이템을 루팅함.
			}
		}

		if( m_bLootMove == TRUE )
		{
			CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		// 그아이템이 사라졌을수 있으니까 검사함.
			if( IsInvalidObj(pCtrl) )		// 아이템 집으러 이동중에 아템이 없어지면
			{
				MoveToDst( pMover->GetPos() );	// 제자리에 섬.
				m_idLootItem = NULL_ID;
				m_bLootMove = FALSE;
			}
		}
#endif // xAI1013

#ifdef __V050324_AISCAN
		CMover* pTarget = NULL;
		int nAttackFirstRange = pMover->GetProp()->m_nAttackFirstRange;
		ADDERRORSEQ( _i, 1000225 );
		if( nAttackFirstRange > 10 || nAttackFirstRange <= 0 )
		{
			Message( "m_nAttackFirstRange 이상:%d %s\n", pMover->GetProp()->m_nAttackFirstRange, pMover->GetName() );
			return TRUE;
		}
		ADDERRORSEQ( _i, 1000226 );
		// 어떤 조건에 해당하는 플레이어가 있는지 스켄한다.
	#ifdef __NPP_050308
		if( pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao )
			pTarget = ScanTarget( pMover, nAttackFirstRange, pProp->m_nScanJob, pProp->m_dwScanQuestId, pProp->m_dwScanItemIdx, pProp->m_nScanChao );
	#else //__NPP_050308
		if( pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx )
			pTarget = ScanTarget( pMover, nAttackFirstRange, pProp->m_nScanJob, pProp->m_dwScanQuestId, pProp->m_dwScanItemIdx );
	#endif //__NPP_050308

		// 조건에 맞는 플레이어가 없고 선공몹이면 선공몹 형태의 스켄을 한다.
		if( pTarget == NULL && GetMover()->m_bActiveAttack )
			pTarget = ScanTarget( pMover, nAttackFirstRange, JOB_ALL );
//			pTarget = ScanTarget( pMover, nAttackFirstRange, pProp->m_nScanJob );
		if( pTarget )
		{
			ADDERRORSEQ( _i, 1000227 );
			if( IsValidObj( (CObj*)pTarget ) )
			{
				ADDERRORSEQ( _i, 1000228 );
				
#ifdef __NPP_050308
				if( (pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE) || pMover->GetProp()->dwClass == RANK_GUARD )	// 가드는 무조건 공격.
#else
				if( (pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE) )		// this가 비행형 몬스터거나 || 타겟이 비행중이 아닐때만 공격.
#endif
				{
					m_dwIdTarget = pTarget->GetId();
					// 타겟이 존재하면 추적 모드로 변경 
					if( m_dwIdTarget != NULL_ID )	
					{
	#ifdef __WORLDSERVER
						g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
	#endif
						SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
					}
				}
				ADDERRORSEQ( _i, 1000229 );
			}

		}
#else // __V050324_AISCAN
//{{AFX
		// 선공 몹은 주변을 늘 감시한다.
		if( GetMover()->m_bActiveAttack )
		{
			ADDERRORSEQ( _i, 1000225 );
			int nAttackFirstRange = pMover->GetProp()->m_nAttackFirstRange;
#ifndef __V050324_AISCAN // 이 버젼에서는 전체적으로 길이가 짧아졌기 때문에 불필요함 
			if( prj.m_nLanguage == LANG_JAP )		// 일본버전은 반응거리가 1/3
				nAttackFirstRange /= 3;
#endif / __V050324_AISCAN
			if( pMover->GetProp()->m_nAttackFirstRange > 10 || pMover->GetProp()->m_nAttackFirstRange <= 0 )
			{
				Message( "m_nAttackFirstRange 이상:%d %s\n", pMover->GetProp()->m_nAttackFirstRange, pMover->GetName() );
				return TRUE;
			} 

			ADDERRORSEQ( _i, 1000226 );
#ifdef __XAI1013
			MoverProp *pProp = GetMover()->GetProp();
			CMover* pTarget = ScanTarget( pMover, nAttackFirstRange, pProp->m_nScanJob );
#else
			CMover* pTarget = ScanTarget( pMover, nAttackFirstRange );
#endif
			ADDERRORSEQ( _i, 1000227 );
			if( IsValidObj( (CObj*)pTarget ) )
			{
				ADDERRORSEQ( _i, 1000228 );
				// this가 비행형 몬스터거나 || 타겟이 비행중이 아닐때만 공격.
				if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )	
				{
					m_dwIdTarget = pTarget->GetId();
					// 타겟이 존재하면 추적 모드로 변경 
					if( m_dwIdTarget != NULL_ID )	
					{
#ifdef __WORLDSERVER
						g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_ATTACK );
#endif
						SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
					}
				}
				ADDERRORSEQ( _i, 1000229 );
			}
		}
//}}AFX
#endif  // __V050324_AISCAN
	}
	ADDERRORSEQ( _i, 1000230 );
	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIMonster::StateIdle( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIMonster::StateIdle()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	ADDERRORSEQ( _i, 10000 );

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		ADDERRORSEQ( _i, 10001 );
//		SendAIMsg( AIMSG_SETPROCESS, TRUE );
		D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
		int nRangeMove = RANGE_MOVE;
#ifdef __XFLYAI0524
		if( pMover->IsFlyingNPC() )
			nRangeMove *= 3.0f;
#endif
		if( IsInRange( vTargetTo, nRangeMove ) == FALSE )
			DoReturnToBegin();
		SetStop( SEC( 0 ) );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
		m_dwIdTarget = NULL_ID;
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		ADDERRORSEQ( _i, 10002 );
#ifdef __POSTAIMSG1008
		MoveProcessIdle( msg );
#else	// __POSTAIMSG1008
		MoveProcessIdle();
#endif	// __POSTAIMSG1008
		ADDERRORSEQ( _i, 1000299 );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		//DLGMESSAGE_DBG( GetMover(), _T( "분노 모드로 변경!" ), 0xffffffff );
		ADDERRORSEQ( _i, 10003 );
		if( m_bReturnToBegin == FALSE || (m_bReturnToBegin && (int)(g_tmCurrent - m_tmReturnToBegin) > SEC(5)) )
		{
	#ifdef __POSTAIMSG1008
		m_dwIdTarget = msg.dwParam1;
	#else	// __POSTAIMSG1008
			m_dwIdTarget = m_AIMsg.dwParam1;
	#endif	// __POSTAIMSG1008
			MoveToDst( m_dwIdTarget );
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
			m_timerAttack.Set( s_tmAttack );	// 대기중일때 공격받으면 10초 타이머 시작
		}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
		ADDERRORSEQ( _i, 10004 );
		//DLGMESSAGE_DBG( GetMover(), _T( "꽥!" ), 0xffffffff );
		SendAIMsg( AIMSG_EXIT );
#ifndef __XAI1013
		MoverProp* pMoverProp = pMover->GetProp();
		// Call HP가 0일 경우는 죽었을 때 몹을 부르는 경우다. 
		if( pMoverProp->m_nCallHP == 0 && pMoverProp->m_nCallHelperIdx )
		{
			CallHelper( pMoverProp );//->m_nCallHelperIdx, &pMoverProp->m_nCallHelperNum, pMoverProp->m_bCallHelperParty );
		}
#endif // not xAI1013

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		ADDERRORSEQ( _i, 10005 );
		//DLGMESSAGE_DBG( GetMover(), _T( "놀다가 충돌했음!" ), 0xffffffff );
		MoveToRandom( STATE_IDLE );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
		ADDERRORSEQ( _i, 10006 );
#ifdef __XFLYAI0524
		if( pMover->IsFlyingNPC() )		
		{
			// 비행몹은 도착하고 나서 멈추지 않는다. 8자비행으로 이동패턴이 바뀐다.
 #ifdef __POSTAIMSG1008
			if( msg.dwParam2 == 1 )
 #else	// __POSTAIMSG1008
			if( m_AIMsg.dwParam2 == 1 )
 #endif	// __POSTAIMSG1008
			{
				int nPattern = 2;
				pMover->SetMovePattern( nPattern );
				// 클라로 전송
 #ifdef __WORLDSERVER
				g_UserMng.AddSetMovePattern( pMover, nPattern, pMover->GetPos(), pMover->GetAngle(), pMover->GetAngleX() );
 #endif
			}
		} else
#endif // xFlyAI0524
		{
			if( m_bReturnToBegin )
				SetStop( xRandom( SEC( 1 ) ) ); // 1초 동안 멈춰라 
			else
				SetStop( SEC( 5 ) + xRandom( SEC( 1 ) ) ); // 1초 동안 멈춰라 

#ifdef __XAI1013
#ifdef __WORLDSERVER	
			if( m_bLootMove )	// 아이템 루팅모드였음.
			{
				CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		// 그아이템이 사라졌을수 있으니까 검사함.
				if( IsValidObj(pCtrl) )
				{
					CItem *pItem = (CItem *)pCtrl;
					D3DXVECTOR3 vDist = pCtrl->GetPos() - pMover->GetPos();
					FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
					if( fDistSq < 3.0f * 3.0f )		// 도착해서 거리 검사 한번더 해서 
					{
						pItem->Lock( theLineFile );	// lock
						if( pItem->IsDelete() )
						{
							pItem->Unlock( theLineFile );	// unlock
							return TRUE;
						}
						CItemElem* pItemElem = (CItemElem *)pItem->m_pItemBase;
						ItemProp *pItemProp = pItem->GetProp();
						BOOL bSuccess = FALSE;
						if( pItemProp->dwItemKind1 == IK1_GOLD )	// 주은아이템이 돈이냐?
						{
							pMover->AddGold( pItemElem->GetGold() );	// 돈증가.
							bSuccess = TRUE;
						} else
						// not GOLD
						{
							bSuccess = pMover->CreateItem( pItemElem );			// 주은 아이템 생성.
						}

						if( bSuccess )
						{
							pItem->Delete();		// 인벤에까지 넣는데 성공하면 바닥의 아이템은 지움.
						}
						pItem->Unlock( theLineFile );	// unlock
					}
				}
				m_bLootMove = FALSE;		// this가 아이템 집는중이라는거 해제.
				m_idLootItem = NULL_ID;
			}
#endif // WorldServer
#endif // xAI1013
		}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	
//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

	EndAIHandler( )
	ADDERRORSEQ( _i, 10007 );
	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIMonster::StateWander( const AIMSG & msg )
	{	return TRUE;	}
BOOL CAIMonster::StateEvade( const AIMSG & msg )
	{	return TRUE;	}
#else	// __POSTAIMSG1008
BOOL CAIMonster::StateWander()
	{	return TRUE;	}
BOOL CAIMonster::StateEvade()
	{	return TRUE;	}
#endif	// __POSTAIMSG1008
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void CAIMonster::CallHelper( MoverProp* pMoverProp ) 
{
	CMover* pMover = GetMover();
	D3DXVECTOR3 vPos = pMover->GetPos();
	int anCountMonster[ 5 ];
	int nCountPlayer = 0;

	ZeroMemory( anCountMonster, sizeof( anCountMonster ) );

	CMover* pTarget = NULL;
	CParty* pParty = NULL;
	BOOL bParty = FALSE;
#ifdef __WORLDSERVER
	CObj* pObj;
	#define MAX_ARRAY 40
	CMover* apMonster[ MAX_ARRAY ][ 5 ];
	CMover* apPlayer[ MAX_ARRAY ];
/*
	for( int i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
	{
		int nCallHelperIdx = pMoverProp->m_nCallHelperIdx  [ i ];
		int nCallHelperNum = pMoverProp->m_nCallHelperNum  [ i ]; 
		int bPartyAttack   = pMoverProp->m_bCallHelperParty[ i ];
		int nNumMonster = 0;

	// 파티 가능 여부 
	pTarget = prj.GetMover( m_dwIdTarget );
	for( int i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
		bParty |= pMoverProp->m_bCallHelperParty[ i ] ? TRUE : FALSE;

	if( bParty && IsValidObj( (CObj*)pTarget ) )
	{
		g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
		pParty = g_PartyMng.GetParty( pTarget->m_idparty );
		// 파티가 확실하다면 파티원을 apPlayer에 수집한다.
		if( pParty )
		{
			FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkPlayer )
			{
				if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && pParty->IsMember( ((CMover*)pObj)->m_idPlayer ) )
				{
					apPlayer[ nCountPlayer++ ] = (CMover*)pObj;
					if( nCountPlayer < MAX_ARRAY )
						goto NEXT2;
				}
			}
			END_LINKMAP
		}
NEXT2:

		if( bPartyAttack )
		{
			pTarget		= prj.GetMover( m_dwIdTarget );
			if( IsValidObj( (CObj*)pTarget ) )
			{
				g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
				pParty	= g_PartyMng.GetParty( pTarget->m_idparty );
				g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
			}
			if( pParty )	bParty	= TRUE;
		}
		if( bParty ) // 파티를 공격하는 설정이면? 
		{
			FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkDynamic )
			{
				if( pObj != pMover && pObj->GetType() == OT_MOVER )
				{
					if( pObj->GetIndex() == nCallHelperIdx )
					{
						apMonster[ nCountMonster++ ] = (CMover*)pObj;
						nNumMonster++;
						if( nCountMonster >= MAX_ARRAY || ( nNumMonster >= nCallHelperNum && nCallHelperNum != 0 ) )
							goto NEXT;
					}
				}
			}
			END_LINKMAP
NEXT:
			FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkPlayer )
			{
				if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() ) //&& pObj != pTarget )
				{
					if( nCountPlayer < MAX_ARRAY )
						apPlayer[ nCountPlayer++ ] = (CMover*)pObj;
				}
			}
			END_LINKMAP
			if( nCountPlayer )
			{
				for( int i = 0; i < nCountMonster; i++ )
				{
					// 각각의 몬스터에게 타겟을 랜덤으로 배정한다. 
					CMover* pNewTarget = (CMover*)apPlayer[ rand() % nCountPlayer ];
					if( pNewTarget )
						apMonster[ i ]->SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pNewTarget->GetId() );
				}
			}
		}
		else
		{ 
			FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkDynamic )
			{
				if( pObj != pMover && pObj->GetType() == OT_MOVER )
				{
					if( pObj->GetIndex() == nCallHelperIdx )
					{
						apMonster[ nCountMonster++ ] = (CMover*)pObj;
						nNumMonster++;
						if( nCountMonster >= MAX_ARRAY || ( nNumMonster >= nCallHelperNum && nCallHelperNum != 0 ) )
							goto NEXT2;
					}
				}
			}
			END_LINKMAP
NEXT2:
			for( int i = 0; i < nCountMonster; i++ )
				apMonster[ i ]->SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, m_dwIdTarget );
		}
	}
*/
	///////////////////////////////////////////////////////

	// 해당 조건에 맞는 몬스터를 수집한다.
	int nRange = 20;
#ifdef __XAI1013
	switch( pMoverProp->m_nHelpRangeMul )
	{
	case 0: nRange = pMoverProp->m_nAttackFirstRange / 2;	break;
	default: nRange = pMoverProp->m_nAttackFirstRange * pMoverProp->m_nHelpRangeMul;	break;	// 시야범위의 배수.
	}
#endif

	FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, nRange / MPU, CObj::linkDynamic )
	{
		if( pObj != pMover && pObj->GetType() == OT_MOVER )
		{
			// FOR_LINKMAP 자체가 느리기 때문에 이 루프를 안쪽에 넣었다.
			for( int i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
			{
				int nCallHelperIdx = pMoverProp->m_nCallHelperIdx[ i ];
				int nCallHelperNum = pMoverProp->m_nCallHelperNum[ i ];
#ifdef __XAI1013
				// 아무나 부르는것이거나 || 같은 종족만 부르는 옵션일때 같은 인덱스인가?
				// 그리고.............
				if( (pMoverProp->m_bHelpWho == 1 || (pMoverProp->m_bHelpWho == 2 && pObj->GetIndex() == pMover->GetIndex())) &&
					( anCountMonster[ i ] < nCallHelperNum || nCallHelperNum == 0 )	)	
				{
					apMonster[ anCountMonster[ i ] ][ i ] = (CMover*)pObj;
					anCountMonster[ i ]++;
					if( anCountMonster[ i ] >= MAX_ARRAY ) 
						goto NEXT;
				}
#else // xAI1013
				if( pObj->GetIndex() == nCallHelperIdx && ( anCountMonster[ i ] < nCallHelperNum || nCallHelperNum == 0 ) )
				{
					apMonster[ anCountMonster[ i ] ][ i ] = (CMover*)pObj;
					anCountMonster[ i ]++;
					if( anCountMonster[ i ] >= MAX_ARRAY ) 
						goto NEXT;
				}
#endif // not xAI1013
			}
		}
	}
	END_LINKMAP
NEXT:
	// 파티 가능 여부 
	pTarget = prj.GetMover( m_dwIdTarget );
	for( int i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
		bParty |= pMoverProp->m_bCallHelperParty[ i ] ? TRUE : FALSE;

	if( bParty && IsValidObj( (CObj*)pTarget ) )
	{
		g_PartyMng.m_AddRemoveLock.Enter( theLineFile );
		pParty = g_PartyMng.GetParty( pTarget->m_idparty );
		// 파티가 확실하다면 파티원을 apPlayer에 수집한다.
		if( pParty )
		{
			FOR_LINKMAP( pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkPlayer )
			{
				if( pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && pParty->IsMember( ((CMover*)pObj)->m_idPlayer ) )
				{
					apPlayer[ nCountPlayer++ ] = (CMover*)pObj;
					if( nCountPlayer >= MAX_ARRAY )
						goto NEXT2;
				}
			}
			END_LINKMAP
		}
NEXT2:
		g_PartyMng.m_AddRemoveLock.Leave( theLineFile );
		
	}
	for( i = 0; i < pMoverProp->m_nCallHelperMax; i++ )
	{
		if( pParty  )
		{
			if( nCountPlayer )
			{
				for( int j = 0; j < anCountMonster[ i ]; j++ )
				{
					// 각각의 몬스터에게 타겟을 랜덤으로 배정한다. 
					CMover* pNewTarget = (CMover*)apPlayer[ rand() % nCountPlayer ];
					if( pNewTarget )
#ifdef __POSTAIMSG1008
						apMonster[ j ][ i ]->PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, pNewTarget->GetId() );
#else	// __POSTAIMSG1008
						apMonster[ j ][ i ]->SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, pNewTarget->GetId() );
#endif	// __POSTAIMSG1008
				}
			}
		}
		else
		{ 
			for( int j = 0; j < anCountMonster[ i ]; j++ )
#ifdef __POSTAIMSG1008
				apMonster[ j ][ i ]->PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, m_dwIdTarget );
#else	// __POSTAIMSG1008
				apMonster[ j ][ i ]->SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, m_dwIdTarget );
#endif	// __POSTAIMSG1008
		}
	}
#endif
}

DWORD CAIMonster::GetAtkMethod_Far()
{
	CMover* pMover = GetMover();
	MoverProp* pMoverProp = pMover->GetProp();
	DWORD dwRandom = xRandom( 100 );
//	DWORD dwAtk;
	//return 3;
	// 필살 판정 성공!
	int nDeathblow = pMover->GetHitPointPercent() / 4.5f;
	if( nDeathblow )
	{
		if( !xRandom( nDeathblow ) )
			return 3;
	}
	switch( pMover->m_dwBelligerence )
	{
	case BELLI_MELEE2X:
		if( dwRandom < 50 )
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if( dwRandom < 30 )
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if( dwRandom < 20 )
			return 1;
		return 2;
	}
	return 1;
}
DWORD CAIMonster::GetAtkMethod_Near()
{
	CMover* pMover = GetMover();
	MoverProp* pMoverProp = pMover->GetProp();
	DWORD dwRandom = xRandom( 100 );
//	DWORD dwAtk;
	// 필살 판정 성공!
	//return 3;
	int nDeathblow = pMover->GetHitPointPercent() / 4.5f;
	//return 3;
	if( nDeathblow )
	{
		if( !xRandom( nDeathblow ) )
			return 3;
	}
	switch( pMover->m_dwBelligerence )
	{
	case BELLI_MELEE2X:
		if( dwRandom < 60 )
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if( dwRandom < 80 )
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if( dwRandom < 50 )
			return 1;
		return 2;
		break;
	}
	return 1;
}
DWORD CAIMonster::GetAtkRange( DWORD dwAtkMethod )
{
	ItemProp* pItemProp = GetAtkProp( dwAtkMethod );
	
#ifdef _DEBUG
	if( pItemProp == NULL )
	{
		Message( "CAIMonster::GetAtkRange : ItemProp->dwAtk%d이 NULL이다. (%s)", dwAtkMethod, GetMover()->GetName() );
	}
#endif

	return pItemProp->dwAttackRange;
}
ItemProp* CAIMonster::GetAtkProp( DWORD dwAtkMethod )
{
	return prj.GetItemProp( GetAtkItemId( dwAtkMethod ) );
}
DWORD CAIMonster::GetAtkItemId( DWORD dwAtkMethod )
{
	CMover* pMover = GetMover();
//	DWORD dwItemID;
//	ItemProp* pItemProp;
	switch( dwAtkMethod )
	{ 
	case 1:
		return pMover->GetProp()->dwAtk1;
	case 2:
		return pMover->GetProp()->dwAtk2;
	case 3:
		return pMover->GetProp()->dwAtk3;
	}
	return 0;
}

void CAIMonster::DoAttack( DWORD dwAtkMethod, CMover* pTarget )
{
	CMover* pMover = GetMover();
	if( IsInvalidObj( pMover ) || dwAtkMethod == 0 )	
		return;

	DWORD dwAtkItem = GetAtkItemId( dwAtkMethod );
	ItemProp* pItemProp = prj.GetItemProp( dwAtkItem );

	//m_timerValidAttack.Reset();

#ifdef __WORLDSERVER 
////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( pItemProp == NULL )
	{
		Error( "CAIMonster::DoAttack : %s %d %d", pMover->GetName(), dwAtkMethod, dwAtkItem );
		return;
	}
	// 장거리 공격은, 선택된 공격 방식(dwAtkMethod)의 해당 공격 아이템 속성에서 속성을 얻어 장거리 공격이라고 판단함.
	if( pItemProp->dwAttackRange == AR_RANGE || pItemProp->dwAttackRange == AR_WAND )
	{
		// 서버에서 idSfx를 만들어서 자기자신에게 등록시켜둠. 나중에 클라에서 맞아서 서버로 정보가 넘어오면 그것과 비교해서 데미지를 넘겨줌.
		// 장거리 공격을 하려면 아래 코드를 넣으세요.
  #ifdef __NEWATTACK0922
		int id = pMover->m_sfxHitArray.Add( 0, pTarget->GetId(), AF_GENERIC, 0 );	// 이부분은 DoAttackRange안으로 들어가야 할거 같다.
  #else
		int id = pMover->m_sfxHitArray.Add( 0, pTarget->GetId(), 0, 0 );	// 이부분은 DoAttackRange안으로 들어가야 할거 같다.
  #endif
//		if( id == -1 )
//			Message( "CAIMonster::DoAttack:%s가 Add를 실패함", pMover->GetName() );
		DWORD dwItemID = dwAtkItem;//pMover->GetProp()->dwAtk2;		// 2번공격으로 공격
		if( dwItemID == NULL_ID )
			Message( "경고 : %s의 dwAtk2가 지정되지 않았다", pMover->GetName() );
		if( pMover->DoAttackRange( pTarget, dwItemID, id ) != 1 )
			pMover->m_sfxHitArray.RemoveSfxHit( id );	// sfxHitArray.Add넣었던것을 다시뺌. 
	}
	else
	// 필살 공격은, 프로퍼티에서 정보를 얻어올 수 없으므로 공격 방식(dwAtkMethod)으로 결정.
	// 필살 공격 루틴은 내부에서 단거리 공격을 할 수도 있다.
	{
		DWORD dwItemID = dwAtkItem;
		if( dwItemID == NULL_ID )
			Message( "경고 : %s의 공격%d가 지정되지 않았다", pMover->GetName(), dwAtkMethod );
		if( dwAtkMethod == 3 )
			pMover->DoAttackSP( pTarget, dwItemID );
		else
		{
			int nError = pMover->SendActMsg( OBJMSG_ATK1, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, 0 ) );
			if( nError > 0 ) 
				g_UserMng.AddMeleeAttack( pMover, OBJMSG_ATK1, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );
			else
				nError = -1;		// 브레이크 포인트 잡으려고 해놓은거.
		}
	}

	// 원거리 공격 테스트하느라 잠시 막아둠.
	//int nError = pMover->SendActMsg( OBJMSG_ATK1, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, 0 ) );
	//if( nError > 0 ) g_UserMng.AddMeleeAttack( pMover, OBJMSG_ATK1, pTarget->GetId(), MAKELONG( 0, 0 ), MAKELONG( 0, (WORD)nError ) );
#else	// __WORLDSERVER
	if( dwAtkMethod == 1 )
		pMover->SendActMsg( OBJMSG_ATK1, pTarget->GetId(), 0, 0 );
	else
	if( dwAtkMethod == 2 )
		pMover->SendActMsg( OBJMSG_ATK1, pTarget->GetId(), 0, 0 );
	else
	if( dwAtkMethod == 3 )
		pMover->SendActMsg( OBJMSG_ATK1, pTarget->GetId(), 0, 0 );
#endif	// __WORLDSERVER

	m_timerAttack.Set( s_tmAttack );		// 공격 후 10초간 추적해옴.
}

#ifdef __XAI1013
//
// 어택 딜레이가 끝난후 다시 어택찬스가 돌아올때.
// 11월 신버전코드입니다. 구버전은 여기다 코딩하지 마셈~ -xuzhu-
#ifdef __POSTAIMSG1008
BOOL CAIMonster::SubAttackChance( const AIMSG & msg, CMover *pTarget )
#else
BOOL CAIMonster::SubAttackChance( CMover *pTarget )
#endif
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	int		nTargetJob = 0;
	if( pTarget->IsPlayer() )
		nTargetJob = pTarget->GetJob();		// 타겟의 직업.
	
	BOOL bAttack = TRUE;
	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	
	
	BOOL bRangeAttack = (pProp->m_bRangeAttack[ nTargetJob ] & 0x7F) ? TRUE : FALSE;	// 장거리 공격 AI가 있냐.

	if( pProp->m_bRecvCond )	// 회복 AI가 있느냐
	{
		// 이곳에 회복코드를 넣으시오.
		if( pMover->GetHitPointPercent( 100 ) <= pProp->m_nRecvCondMe )	// HP가 m_nRecvCondMe% 이하로 떨어졌는가?
		{
			if( pMover->GetManaPoint() > 0 )
			{
				int nHP = (int)(pMover->GetMaxHitPoint() * (pProp->m_nRecvCondHow / 100.0f));	// m_nHow% 만큼의 HP를 회복
				pMover->IncHitPoint( nHP );
	#ifdef __WORLDSERVER
				g_UserMng.AddCreateSfxObj( pMover, XI_GEN_CURE01, pMover->GetPos().x, pMover->GetPos().y, pMover->GetPos().z );	// 절대좌표로 하자.
	#endif
				if( pProp->m_nRecvCondMP > 0 )		// m_nMP%만큼의 마나를 소모.
				{
	//				int nMP = (int)(pMover->GetMaxManaPoint() * (pProp->m_nRecvCondMP / 100.0f));
					int nMP = pProp->m_nRecvCondMP;		// %에서 절대치로 바꼈다.
					pMover->IncManaPoint( -nMP );
				} else
				{
					int nMP = pMover->GetMaxManaPoint();		// 0으로 지정되어 있으면 몽땅 소모.
					pMover->IncManaPoint( -nMP );
				}

				bAttack = FALSE;		// 회복을 했으면 공격은 다음턴에.
			}
		}
		
		m_timerAttackDelay.Set( SEC( 3 ) );		// 3초후에 다시 공격기회.
	}


	if( bAttack )
	{
		ADDERRORSEQ( _i, 177 );
		if( m_bRangeAttack )
			m_dwAtkMethod = GetAtkMethod_Far();		// 공격 방법 세팅 
		else
			m_dwAtkMethod = GetAtkMethod_Near();		// 공격 방법 세팅 


		DWORD dwAtkRange = GetAtkRange( m_dwAtkMethod );
		
		// 장거리 공격 선택이라면?
		if( dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND )
		{
			// 유효 거리 판단
			ADDERRORSEQ( _i, 178 );
			FLOAT fRange = (float)(pProp->m_bRangeAttack[ nTargetJob ] & 0x7F);
			fRange += 1.0f;		// 사거리 +1미터 정도는 걍 쏜다.
			if( IsInRange( vTargetTo, fRange ) == TRUE )		// AI에서 지정한 공격거리까지 다가갔는가?
			{
				ADDERRORSEQ( _i, 179 );
				DoAttack( m_dwAtkMethod, (CMover*)pTarget );	// 공격
				m_timerAttackDelay.Set( SEC(3) );		// 장거리 상태이다. 3초 공격 딜레이 설정.
				ADDERRORSEQ( _i, 17901 );
			}
		} else
		{
			FLOAT fRange = pMover->GetAttackRange( dwAtkRange );
			if( pMover->IsRangeObj( pTarget, fRange ) )
//			if( IsInRange( vTargetTo, fRange ) == TRUE )	// 무기의 사정거리 안에 있는가.
			{
				DoAttack( m_dwAtkMethod, pTarget );		//  공격
				m_bGoTarget = TRUE;  // <- 이건 뭐하는거? 
				// 공격을 했으면 어택 딜레이 리셋.
				float fTime= (float)( pProp->dwReAttackDelay - SEC( 1 ) ) + xRandom( SEC( 2 ) );
				m_timerAttackDelay.Set( fTime );	
				TRACE( "AtkDelay:%d\n", (int)fTime );
			} else
			{
				int a = 0;
			}
		}
		// 근접공격은 ARRIVAL에서 처리한다. 여기서 처리해도 좋지 않을까?
	}
	
	return TRUE;
}
#else // xAI1013
//
// (구버전)
// 어택 딜레이가 끝난후 다시 어택찬스가 돌아올때.
#ifdef __POSTAIMSG1008
BOOL CAIMonster::SubAttackChance( const AIMSG & msg, CMover *pTarget )
#else
BOOL CAIMonster::SubAttackChance( CMover *pTarget )
#endif
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	
	
	ADDERRORSEQ( _i, 177 );
	// 공격 방법 결정 : 공격 상대가 3미터 이상의 거리차가 있다면 
	if( IsInRange( vTargetTo, 3 ) == FALSE )
	{
		// 장거리 상태이다. 3초 공격 딜레이 설정.
		m_timerAttackDelay.Set( SEC( 3 ) );
		// 공격 방법 세팅 
		m_dwAtkMethod = GetAtkMethod_Far();
	}
	else
	{
		// 단거리 상태이다. 지정된 수치로 공격 딜레이 설정 
		m_timerAttackDelay.Set( ( pProp->dwReAttackDelay - SEC( 1 ) ) + xRandom( SEC( 2 ) ) );
		// 공격 방법 세팅 
		m_dwAtkMethod = GetAtkMethod_Near();
	}
	//m_dwAtkMethod = 2;
	DWORD dwAtkRange = GetAtkRange( m_dwAtkMethod );
	
	// 장거리 공격 선택이라면?
	if( dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND )
	{
		// 유효 거리 판단
		ADDERRORSEQ( _i, 177 );
		FLOAT fRange = pMover->GetAttackRange( dwAtkRange );
		if( IsInRange( vTargetTo, fRange ) == TRUE )
			DoAttack( m_dwAtkMethod, (CMover*)pTarget );
	}
	ADDERRORSEQ( _i, 178 );
	MoveToDst( m_dwIdTarget );		// 타겟쪽으로는 계속 이동시도.
	ADDERRORSEQ( _i, 17801 );
	
	return TRUE;
}
#endif // not xAI1013

//
// AI에 따른 공격방식 선택.
//
// return : 0:근거리 1:원거리
int CAIMonster::SelectAttackType( CMover *pTarget )
{
	int nAttackType = 0;
#ifdef __XAI1013
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
	D3DXVECTOR3	vDist = pTarget->GetPos() - pMover->GetPos();	// 타겟과의 거리.
	FLOAT		fDistSq = D3DXVec3LengthSq( &vDist );

	int		nTargetJob = 0;
	int		nRange = pProp->m_bRangeAttack[ nTargetJob ] & 0x7F;
	
	if( pTarget->IsPlayer() )		nTargetJob = pTarget->GetJob();		// 타겟의 직업.

	BOOL bRange = ( pProp->m_bRangeAttack[ nTargetJob ] ) ? TRUE : FALSE;	// AI에 거리가 있으면 원거리 AI있음.
	BOOL bMelee = (BOOL)pProp->m_bMeleeAttack;								// AI에 근거리공격도 있는지.


	if( bRange && bMelee )		// 원거리/근거리 둘다 가지고 있음
	{
		int nHalfRange = nRange / 2;		// 사거리의 반.
		if( fDistSq < (nHalfRange * nHalfRange) )	// 너무 가까이 붙어있어 거리유지가 히들면.
			nAttackType = 0;	// 근접공격으로 하자.
		else
			nAttackType = 1;	// 거리에 여유가 있으므로 원거리 공격.
	} else
	if( bRange && bMelee == FALSE )		// 원거리만 가지고 있음
	{
		nAttackType = 1;
	} else
	if( bRange == FALSE && bMelee )		// 근거리만 가지고 있음
	{
		nAttackType = 0;
	}

#endif // xAI1013
	return nAttackType;
}

// SummonAI가 있을때 소환을 시도한다. xx초 마다.
void CAIMonster::SubSummonProcess( CMover *pTarget )
{
#ifdef __XAI1013
	if( timeGetTime() - m_tmSummon < SEC(20) )		// 20초마다 한번씩 소환을 시도.
		return;

	m_tmSummon = timeGetTime();
	
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	// 소환 AI
	if( pProp->m_nSummProb )	// 값이 있으면 소환 AI가 있다.
	{
		BOOL bEmpty = TRUE;
		for( int i = 0; i < MAX_SUMMON; i ++ )
		{
			if( m_idSummon[i] != NULL_ID )		
			{
				CMover *pZako = prj.GetMover( m_idSummon[i] );
				if( IsValidObj(pZako) )
				{
					if( pZako->IsLive() )	// 그넘들이 살아있는지까지도 검사해야한다.
						bEmpty = FALSE;		// 아직 소환되어 있는 애들이 있다.
					else
						m_idSummon[i] = NULL_ID;
				} else
					m_idSummon[i] = NULL_ID;
			}
		}
		if( bEmpty == TRUE )	// 소환되어 있는 애들이 없는가?
		{
			if( xRandom(100) < pProp->m_nSummProb )	// 소환 시도.
			{
				if( pProp->m_nSummNum < 0 || pProp->m_nSummNum >= MAX_SUMMON )
				{
					Message( "SubAttackChance : m_nSummNum의 값이 범위 초과 %d", pProp->m_nSummNum );
					pProp->m_nSummNum = 0;
				}
				CObj* pObj = NULL;
				D3DXVECTOR3 vLocal, vPos;
				for( i = 0; i < pProp->m_nSummNum; i ++ )
				{
					pObj	= CreateObj( D3DDEVICE, OT_MOVER, (DWORD)pProp->m_nSummID );
					if( NULL == pObj )	
					{
						Message( "SubAttackChance : 소환 실패 %d", pProp->m_nSummID );
						break;
					}
					vLocal.x = 1.5f - xRandomF( 3.0f );		// 보스 주위에 랜덤으로 생성.
					vLocal.y = 0;
					vLocal.z = 1.5f - xRandomF( 3.0f );
					vPos = pMover->GetPos();
					vPos += vLocal;
					pObj->SetPos( vPos );
					pObj->InitMotion( pObj->m_dwMotion );
					pObj->UpdateLocalMatrix();
					((CMover*)pObj)->m_bActiveAttack = TRUE;	// 부하는 선공 형으로 소환하자.
					((CMover*)pObj)->AddItToGlobalId();
					pWorld->AddObj( pObj, FALSE );
					m_idSummon[i] = ((CMover *)pObj)->GetId();	// 소환한넘을 기억함.
					//					((CMover*)pObj)->SetAttackTarget( 날때린놈 );
				}
				
			}
#ifdef __WORLDSERVER
			g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_SUMMON );
#endif
		}
	}
#endif // xAI1013
}



#ifdef __POSTAIMSG1008
BOOL CAIMonster::MoveProcessRage( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIMonster::MoveProcessRage()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	ADDERRORSEQ( _i, 170 );
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
#ifdef __CLIENT
	if( pWorld->GetLandscape( pMover->GetPos() ) == NULL )
		return FALSE;
#endif

	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( /*pMover->m_pActMover->IsActDamage() ||*/ pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;
//	CCtrl* pTarget = prj.GetCtrl( m_dwIdTarget ); // 목표를 재설정해준다.
	CMover* pTarget = prj.GetMover( m_dwIdTarget ); // 목표를 재설정해준다.
	if( IsValidObj( pTarget ) == FALSE )
	{
		ADDERRORSEQ( _i, 171 );
		m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
	}
	else
	{
		ADDERRORSEQ( _i, 172 );
		// 상태가 죽었다.
		if( ((CMover*)pTarget)->IsDie() )
		{
			m_dwIdTarget = NULL_ID;
			SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
			return TRUE;
		}
		
		ADDERRORSEQ( _i, 173 );
		if( pMover->IsFlyingNPC() == FALSE && ((CMover*)pTarget)->m_pActMover->IsFly() )	// this가 비행형이 아니고 상대가 비행중이라면
		{
			SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
		}

		if( m_timerAttack.TimeOut() )	// 공격 못한지 5초 지났냐?
		{
			ADDERRORSEQ( _i, 174 );
			m_timerAttack.Set( s_tmAttack );		// 타이머 리셋

			D3DXVECTOR3 vDist = pMover->GetPos() - m_vOldPos;
			vDist.y = 0;		// 옆으로 살짝움직였는데 산위로 확 올라갔을땐 거리가 엄청 멀수 있다. 그래서 이렇게 한것.
			FLOAT fLenSq = D3DXVec3LengthSq( &vDist );		// 5초전 좌표와 지금 좌표의 거리를 계산.
			if( fLenSq < 2.0f * 2.0f )		// 2미터를 못벗어났다면 어딘가에 갖혔다는 뜻.
			{
				if( pMover->GetSpeed( pMover->m_pActMover->m_fSpeed ) > 0 )		// 마법적으로 움직이지 못하게 된상태에선 워프하면 안됨.
				{
					D3DXVECTOR3 vTarget = pTarget->GetPos();
					pMover->SetPos( vTarget );		// 갖혔으면 타겟쪽으로 바로 워프.
					#ifdef __WORLDSERVER
						g_UserMng.AddSetPos( pMover, pMover->GetPos() );
					#endif
				} 
			} 
			else
			{
				m_tmReturnToBegin = timeGetTime();			// 다시 돌아갈땐 시간을 재기시작.  x초간은 맞아도 반격안하고 계속 간다.
				SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
			}
			m_vOldPos = pMover->GetPos();	// x초마다 현재 좌표를 저장함.
		}

		ADDERRORSEQ( _i, 176 );
		D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();	
		// 만약 상대가 공격할 수 없는 곳에 있다면 공격 포기하고 돌아가자.
		FLOAT fRange = RANGE_CANCEL;// - 5.0f;		// -5.0f은 뭘까?
//		if( pMover->IsFlyingNPC() )		// 비행형 몹일경우 이 거리를 두배로 늘리자.
//			fRange *= 2.0f;
		if( IsInRange( vTargetTo, fRange ) == FALSE )
		{
			m_dwIdTarget = NULL_ID;		// 타겟을 포기함.
			SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
		}
		else if( pTarget->GetType() == OT_MOVER && ((CMover *)pTarget)->IsMode( TRANSPARENT_MODE ) )		// 쫒고 있던 타겟이 투명화 상태가 되면 
		{
			m_dwIdTarget = NULL_ID;		// 타겟을 포기함.
			SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
		}
		else
		// 정지상태가 끝났을 때만 재 타게팅 한다. 즉 정지했을 때만 타케팅을 세팅함 
		// 정지 휴식기는 보장해야한다.
		//if( IsEndStop() )
		{
#ifdef __XAI1013
			// 이부분 이렇게 자주 호출안해도 되지 않을까 싶다.
			m_bRangeAttack = SelectAttackType( pTarget );		// AI에따른 공격방식 선택.

//#ifdef __XSYNC0711
//			if( (pMover->GetCount() & 31) == 0 )	// 이것때문인지는 모르겠으나 유저들이 몹이 너무 빨리 돌아간다고 해서 잠시 막음.
//#endif
			{				
				if( m_bRangeAttack )	// 원거리 공격으로 결정됐음.
				{
					int nTargetJob = pTarget->GetJob();
					if( nTargetJob < 0 )	nTargetJob = 0;		// 디폴트로 -1로 초기화 하기땜에 검사해야함.
					if( nTargetJob >= MAX_JOB )
					{
						Message( "CAIMonster::MoverProcessRage : 범위침범 %d", nTargetJob );
						nTargetJob = 0;
					}

					FLOAT	fRange = (FLOAT)(pProp->m_bRangeAttack[ nTargetJob ] & 0x7F);		// 사거리
					BOOL	bKeepRange = (pProp->m_bRangeAttack[ nTargetJob ] & 0x80) ? TRUE : FALSE;	// 상위 1비트가 온?
					FLOAT	fDist = D3DXVec3Length( &vTargetTo );		// 적과의 거리.
					if( bKeepRange )
					{
						if( fabs(fDist - fRange) < 1.0f )	// 적과의 거리와 사거리의 차이가 1미터 이내면 더이상 움직일 필요 없음.
						{
							if( pMover->IsEmptyDest() == FALSE )	// 이동중임.
								MoveToDst( pMover->GetPos() );		// 현재위치로 이동 - 서버에선 스탑과 같은 명령. 클라에선 서버의 위치로 이동.
						} else
						{
							// fRange의 거리를 유지하도록 ...
							if( fDist < fRange )	
							{
								FLOAT fAngle = GetDegreeX( pMover->GetPos(), pTarget->GetPos() );	// dest, src의 파라메터 위치를 바꾸면 반대방향이 나온다.
								D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, 10 );
								MoveToDst( vDst );		// 반대방향으로 달려서 거리를 벌림.
							} else
								MoveToDst( pTarget->GetId() );	// 목표와의 거리가 사거리보다 더 머니까 근접함.
						}
					} else
					// not keepRange
					{
						if( fDist < fRange )		// 사거리안에 있으면 제자리에 있음.
						{
							if( pMover->IsEmptyDest() == FALSE )	// 이동중임.
								MoveToDst( pMover->GetPos() );		// 현재위치로 이동 - 서버에선 스탑과 같은 명령. 클라에선 서버의 위치로 이동.
						} else
							MoveToDst( pTarget->GetId() );	// 사거리 밖이면 목표쪽으로 접근.
					}
				} else
				// 근거리
				{
					MoveToDst( pTarget->GetId() );		// 근거리는 무조건 목표쪽으로 접근.
				}
			} // m_nCount & 31

			if( pProp->m_nSummProb )			// Summon AI가 있느냐?
				SubSummonProcess( pTarget );	// 소환을 시도
#endif // xAI1013
			
			// 공격 찬스.
			if( m_timerAttackDelay.TimeOut() )
			{
#ifdef __POSTAIMSG1008
				if( SubAttackChance( msg, pTarget ) == FALSE )
#else
				if( SubAttackChance( pTarget ) == FALSE )
#endif
						return FALSE;
			}
		}
	}
	ADDERRORSEQ( _i, 179 );
	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIMonster::StateRage( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIMonster::StateRage()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int	_i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int	_i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	ADDERRORSEQ( _i, 100 );

#ifdef __XUZHU
#ifdef __POSTAIMSG1008
//	if( (g_nProcessCnt & 15) == 0 )
//	{	
//		CString strDbg;
//		strDbg.Format( "Rage %d", msg.dwMessage );
//		OutputDebugString( (LPCTSTR)strDbg );
//	}
#endif
#endif
	

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	if( IsInvalidObj(pMover) )	return FALSE;

	//TRACE( "Move State %d\n", pMover->m_pActMover->IsMove() );

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		ADDERRORSEQ( _i, 101 );
#ifdef __POSTAIMSG1008
		if( msg.dwParam2 != NULL_ID )
			m_dwIdTarget = msg.dwParam2;
#else	// __POSTAIMSG1008
		if( m_AIMsg.dwParam2 != NULL_ID )
			m_dwIdTarget = m_AIMsg.dwParam2;
#endif	// __POSTAIMSG1008
//		SendAIMsg( AIMSG_SETPROCESS, TRUE );
		m_timerAttack.Set( s_tmAttack );
		m_timerTrace.Set( s_tmTrace );
		m_timerAttackDelay.Set( 0 ); // 공격 딜레이를 초기화. 그렇지 않으면 처음 공격이 딜레이 먹고 들어간다.
		SetStop( 0 ); // 0으로 세팅하면 정지 상태가 바로 끝나게 되어 공격이 가능하게 된다. 
		              // 정지 상태에서는 정지 상태를 일정 시간동안 유지해주어야하는데, 그렇게 되면 공격을 못할 수 있다.

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		// 타겟을 클리어하고 대기모드로 돌아감.
		m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		ADDERRORSEQ( _i, 102 );
#ifdef __POSTAIMSG1008
		MoveProcessRage( msg );
#else	// __POSTAIMSG1008
		MoveProcessRage();
#endif	// __POSTAIMSG1008
		ADDERRORSEQ( _i, 102102 );	// 새로 넣었음.
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		ADDERRORSEQ( _i, 103 );
		m_nCollisionCount = 0;
//		m_timerAttack.Set( s_tmAttack );
		if( IsMove() && m_bGoTarget == FALSE )
		{
			if( xRandom( 2 ) == 0 )		// 1/2확률로 공격자를 때림.
			{
#ifdef __POSTAIMSG1008
				m_dwIdTarget = msg.dwParam1;
#else	// __POSTAIMSG1008
				m_dwIdTarget = m_AIMsg.dwParam1;
#endif	// __POSTAIMSG1008
				// 공격자에게 목표를 설정 
				ADDERRORSEQ( _i, 150 );
				MoveToDst( m_dwIdTarget );
				ADDERRORSEQ( _i, 151 );
			}
			m_bGoTarget = TRUE;
		}
		MoverProp* pMoverProp = pMover->GetProp();
		int nHPPercent = pMover->GetHitPointPercent( 100 );

		// 도주 HP 퍼센트보다 현재 HP퍼센트가 작을 경우 도망가자.
		if( nHPPercent < pMoverProp->m_nRunawayHP )
		{
			// 도주 모드로 세팅 
			SendAIMsg( AIMSG_SETSTATE, STATE_RUNAWAY );
  #ifdef __WORLDSERVER
			g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_EVADE );
  #endif
		}
		// Call HP 퍼센트보다 현재 HP퍼센트가 작을 경우 동료를 부르자.
 #ifdef __XAI1013
		if( pMoverProp->m_bHelpWho )	// 헬프AI가 있을경우.
		{
			if( timeGetTime() - m_tmHelp > pMoverProp->m_tmUnitHelp )	// 최초 m_tmHelp는 0이므로 헬프를 한다.
			{
				if( m_bCallHelper == FALSE )
				{
					CallHelper( pMoverProp );
  #ifdef __WORLDSERVER
					g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_HELPER );
  #endif
					m_tmHelp = timeGetTime();
					if( pMoverProp->m_tmUnitHelp == 0 )		// 0이면 한번만 부르고 안부름.
						m_bCallHelper = TRUE;
				}
			}
		}
		else
			m_bCallHelper = FALSE;
 #else // xAI1013
		if( nHPPercent < pMoverProp->m_nCallHP && pMoverProp->m_nCallHelperIdx )
		{
			// 연속해서 호출하지않게 하기 
			if( m_bCallHelper == FALSE )
				CallHelper( pMoverProp );//->m_nCallHelperIdx, pMoverProp->m_nCallHelperNum, pMoverProp->m_bCallHelperParty );
			m_bCallHelper = TRUE;
		}
		else
			m_bCallHelper = FALSE;
 #endif // not xAI1013

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		ADDERRORSEQ( _i, 104 );
		MoveToRandom( STATE_RAGE );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
		ADDERRORSEQ( _i, 105 );

#ifndef __XAI1013
		MoverProp* pMoverProp = pMover->GetProp();
		// Call HP가 0일 경우는 죽었을 때 몹을 부르는 경우다. 
		if( pMoverProp->m_nCallHP == 0 && pMoverProp->m_nCallHelperIdx )
		{
			CallHelper( pMoverProp );//->m_nCallHelperIdx, pMoverProp->m_nCallHelperNum, pMoverProp->m_bCallHelperParty );
		}
#endif // not xAI1013
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDAMAGE ) 
		ADDERRORSEQ( _i, 106 );
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		ADDERRORSEQ( _i, 107 );
#ifdef __POSTAIMSG1008
		if( m_dwIdTarget == msg.dwParam1 )
#else	// __POSTAIMSG1008
		if( m_dwIdTarget == m_AIMsg.dwParam1 )
#endif	// __POSTAIMSG1008
			m_dwIdTarget = NULL_ID;
		TRACE( "TARGET CLEARED\n" );
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	OnMessage( AIMSG_NOMOVE ) 
		ADDERRORSEQ( _i, 108 );
		// 타겟에 갈 수가 없는데, 상대가 날아 올랐다. 그렇다면 IDLE로 바꾸고 그렇지 않다면 도망가자(장거리 공격수가 장애물 위에 있는 상황)
		MoveToRandom( STATE_RAGE );

	OnMessage( AIMSG_BEGINMOVE )
		m_bGoTarget = FALSE; 

	OnMessage( AIMSG_ARRIVAL )
#ifndef __XAI1013
		ADDERRORSEQ( _i, 109 );
 #ifdef __POSTAIMSG1008
		CCtrl* pTarget = prj.GetCtrl( msg.dwParam1 );
 #else	// __POSTAIMSG1008
		CCtrl* pTarget = prj.GetCtrl( m_AIMsg.dwParam1 );
 #endif	// __POSTAIMSG1008
		if( IsValidObj( pTarget ) && pTarget->GetType() == OT_MOVER )
		{
			//if( m_timerAttackDelay.TimeOut() )
			{
				//m_timerAttackDelay.Set( pMover->GetProp()->dwReAttackDelay + xRandom( SEC( 1 ) ) );
				DoAttack( m_dwAtkMethod, (CMover*)pTarget );
			}
			//else
			//	MoveToDst( m_dwIdTarget );
			m_bGoTarget = TRUE;
		//	m_dwAtkMethod = GetAtkMethod_Near();
		}
		else
		{
			// 도착했는데, 수평 좌표만 맞고 수직 좌표가 안맞을 수 있다. 그럴 경우 귀환 
			if( 0 )// pMover->IsTouchObj( pTarget ) == FALSE )
			{
				m_dwIdTarget = NULL_ID;		// 타겟을 포기함.
				SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
			}
			else
			{
				MoverProp* pMoverProp = pMover->GetProp();
				if( pMoverProp->m_dwAttackMoveDelay )
					SetStop( pMover->GetProp()->m_dwAttackMoveDelay + xRandom( SEC( 1 ) ) ); // + xRandom은 약간의 가변 차이를 주기 위한 것.
				else
					SetStop( 0 );
			}
		}
#endif // not xAI1013
		//SendAIMsg( AIMSG_EXIT );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	
		ADDERRORSEQ( _i, 110 );
//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

	EndAIHandler( )
	ADDERRORSEQ( _i, 119 ); // 새로넣었음.
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CAIMonster::MoveProcessRunaway()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	#ifdef __CLIENT
	if( pWorld->GetLandscape( pMover->GetPos() ) == NULL )
		return FALSE;
	#endif
#ifdef _DEBUG
	DWORD dwId = 18385;

	if( pMover->GetId() == dwId )
	{
		int a = 0;
	}
#endif

	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	CCtrl* pTarget = prj.GetCtrl( m_dwIdTarget ); // 목표를 재설정해준다.

	// 타겟이 무효인가?
	if( IsValidObj( pTarget ) == FALSE )
	{
		m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
	}
	else
	// 멈춤 대기 시간이 끝났나?
	if( IsEndStop()  )
	{
		/*
		//m_dwAtkMethod = 2;
		DWORD dwAtkRange = GetAtkRange( m_dwAtkMethod );

		// 장거리 공격 선택이라면?
		if( dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND )
		{
			// 유효 거리 판단
			FLOAT fRange = pMover->GetAttackRange( dwAtkRange );
		*/
		CMover* pMoverTarget = (CMover*)pTarget;
		DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
		FLOAT fRange = pMoverTarget->GetAttackRange( dwAttackRange ) * 1.7f;

		// 나와 20미터 안쪽(근처) 거리에 존재하면 줄행랑!
		if( IsInRange( pTarget->GetPos() - pMover->GetPos(), fRange ) == TRUE )
		{
			// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
			pMover->AddAngle( 20 - xRandom( 40 ) );
			// 각도를 꺼낸 후에 연장선에 있는 좌표를 구한다.
			FLOAT fAngle = pMover->GetAngle();
			D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, fRange );
			MoveToDst( vDst );
		}
		if( m_bTargetNoMovePos == FALSE )
		{
			//else
			// 여기서 휴식 ; HP를 채워준다.
			if( pMover->m_timerRecovery.IsTimeOut() )	// 일정시간마다.
			{
				pMover->m_timerRecovery.Reset();
				pMover->IncHitPoint( pMover->GetHPRecovery() );
				// 체력이 80 % 이상 되면 반격 시도.
				if( pMover->GetHitPointPercent( 100 ) >= 50 )
					SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
			}
		}
	}
	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIMonster::StateRunaway( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIMonster::StateRunaway()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int	_i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int	_i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif	// __DEADLOCK__
	ADDERRORSEQ( _i, 11000 );

#ifdef __XUZHU
#ifdef __POSTAIMSG1008
	if( (g_nProcessCnt & 15) == 0 )
	{
		CString strDbg;
		strDbg.Format( "Runaway %d", msg.dwMessage );
		OutputDebugString( (LPCTSTR)strDbg );
	}
#endif
#endif
	
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
		ADDERRORSEQ( _i, 11001 );
		// 이경우 정상적인 옵션에 의한 도주가 아니라, 상대가 공격할 수 없는, 지리상 잇점이 있는 장소에
		// 있기 때문에 줄행랑을 치는 경우다. 
#ifdef __POSTAIMSG1008
		m_bTargetNoMovePos	= msg.dwParam2;
#else	// __POSTAIMSG1008
		m_bTargetNoMovePos	= m_AIMsg.dwParam2;
#endif	// __POSTAIMSG1008
		// 현재의 반대 방향으로 도주 시작 
		// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
		pMover->AddAngle( 180 + ( 20 - xRandom( 40 ) ) );
		// 각도를 꺼낸 후에 연장선에 있는 좌표를 구한다.
		CMover* pMoverTarget = (CMover*)prj.GetCtrl( m_dwIdTarget ); // 목표를 재설정해준다.
		if( IsValidObj(pMoverTarget) )
		{
			DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
			FLOAT fRange = pMoverTarget->GetAttackRange( dwAttackRange ) * 1.7f;
			FLOAT fAngle = pMover->GetAngle();
			D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, fRange );
			MoveToDst( vDst );
//			SendAIMsg( AIMSG_SETPROCESS, TRUE );
			m_bFirstRunaway = TRUE; // 처음 도주인가?
		}
		else
			SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		ADDERRORSEQ( _i, 11002 );
		MoveProcessRunaway();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 
		ADDERRORSEQ( _i, 11003 );
		if( m_bTargetNoMovePos == TRUE && m_bFirstRunaway == FALSE )
			SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
		ADDERRORSEQ( _i, 11004 );
		// 현재의 반대 방향으로 도주 시작 
		// 5도 폭을 가지고 각도 변경. 폭이 크면 클수록 지그재그 형태로 움직인다.
		pMover->AddAngle( 180 + ( 20 - xRandom( 40 ) ) );
		// 각도를 꺼낸 후에 연장선에 있는 좌표를 구한다.
		FLOAT fAngle = pMover->GetAngle();
		D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec( fAngle, 5 );
		MoveToDst( vDst );
		// m_bFirstRunaway = FALSE; <-- 04.10.08 데드락을 유발하는 것으로 생각됨.
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDAMAGE ) 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		ADDERRORSEQ( _i, 11005 );
#ifdef __POSTAIMSG1008
		if( m_dwIdTarget == msg.dwParam1 )
#else	// __POSTAIMSG1008
		if( m_dwIdTarget == m_AIMsg.dwParam1 )
#endif	// __POSTAIMSG1008
			m_dwIdTarget = NULL_ID;
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	OnMessage( AIMSG_ARRIVAL )
		ADDERRORSEQ( _i, 11006 );
		m_bFirstRunaway = FALSE;
		//SetStop( xRandom( SEC( 2 ) ) + SEC( 1 ) ); // 2초~3초 동안 멈춰라 
		MoverProp* pMoverProp = pMover->GetProp();
		if( pMoverProp->m_dwRunawayDelay )
			SetStop( pMover->GetProp()->m_dwRunawayDelay + xRandom( SEC( 1 ) ) ); // + xRandom은 약간의 가변 차이를 주기 위한 것.

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	
//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

	EndAIHandler( )

	ADDERRORSEQ( _i, 11007 );
	return TRUE;
}

