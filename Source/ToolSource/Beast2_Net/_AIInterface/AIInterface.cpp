#include "stdafx.h"
#include "AIInterface.h"

#ifdef __ACROBAT_0504
#include "DefineSkill.h"
#endif //__ACROBAT_0504

#ifdef __WORLDSERVER
#include "User.h"
#endif // WORLDSERVER

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_AISTATE_MAP( CAIInterface, CAIInterface )
END_AISTATE_MAP()

CAIInterface::CAIInterface()
{
	m_pObj = NULL;
//	m_dwState = 0;
//	m_bProcess = FALSE;
	m_pStateFunc = NULL;
	m_dwCurrentState = 0;
}
CAIInterface::CAIInterface( CObj* pObj, BYTE byAIInterface ) 
{ 
	m_pObj = pObj;
//	m_dwState = 0;
//	m_bProcess = FALSE;
	m_pStateFunc = NULL;
	m_dwCurrentState = 0;
}

CAIInterface::~CAIInterface()
{
}

#ifndef __POSTAIMSG1008
#ifdef __DEADLOCK__
void CAIInterface::RouteMessage( long lIdx )
#else	// __DEADLOCK__
void CAIInterface::RouteMessage()
#endif	// __DEADLOCK__
{
	ADDERRORSEQ( lIdx, 2000 );
#ifdef __POSTAIMSG0927
	while( 1 )
    {
		ADDERRORSEQ( lIdx, 2001 );
		m_AddRemoveLock.Enter( theLineFile );
		ADDERRORSEQ( lIdx, 2002 );
		if( m_MsgQueue.empty() )
		{
			ADDERRORSEQ( lIdx, 2003 );
			m_AddRemoveLock.Leave( theLineFile );
			ADDERRORSEQ( lIdx, 2004 );
			break;
		}
		ADDERRORSEQ( lIdx, 2005 );
		AIMSG AIMsg = m_MsgQueue.front();
		ADDERRORSEQ( lIdx, 2006 );
		m_MsgQueue.pop();
		ADDERRORSEQ( lIdx, 2007 );
		m_AddRemoveLock.Leave( theLineFile );
		ADDERRORSEQ( lIdx, 2008 );
		m_AIMsg		= AIMsg;

#ifdef __DEADLOCK__
		m_AIMsg.lIdx	= lIdx;
#endif	// __DEADLOCK__
		
		if( AIMsg.dwMessage == AIMSG_SETSTATE )
		{
		   m_dwCurrentState		= AIMsg.dwParam1;
		   m_AIMsg.dwMessage	= AIMSG_INIT;
		   ADDERRORSEQ( lIdx, 2009 );
		   m_pStateFunc		= GetAIStateFunc( AIMsg.dwParam1 );
		   ADDERRORSEQ( lIdx, 2010 );
		}
		ADDERRORSEQ( lIdx, 2011 );
		if( m_pStateFunc )
			( this->*( m_pStateFunc ) )( );
		ADDERRORSEQ( lIdx, 2012 );
    }

	ADDERRORSEQ( lIdx, 2013 );
	if( m_pStateFunc )
	{
		m_AIMsg.dwMessage	= AIMSG_PROCESS;
		m_AIMsg.dwParam1	= 0;
		m_AIMsg.dwParam2	= 0;
		( this->*( m_pStateFunc ) )( );
	}
	ADDERRORSEQ( lIdx, 2014 );
#else	// __POSTAIMSG0927
	ADDERRORSEQ( lIdx, 2101 );
	if( m_pStateFunc )
	{
		m_AIMsg.dwMessage = AIMSG_PROCESS;
		m_AIMsg.dwParam1 = 0;
		m_AIMsg.dwParam2 = 0;
#ifdef __DEADLOCK__
		m_AIMsg.lIdx	= lIdx;
#endif	// __DEADLOCK__
		ADDERRORSEQ( lIdx, 2102 );
		if( m_pStateFunc )
			( this->*( m_pStateFunc ) )( );
		ADDERRORSEQ( lIdx, 2103 );
	}
	ADDERRORSEQ( lIdx, 2104 );
#endif	// __POSTAIMSG0927
	ADDERRORSEQ( lIdx, 2112 );
}
#endif	// __POSTAIMSG1008

FUNCAISTATE CAIInterface::GetAIStateFunc( DWORD dwState)
{
	const STATEMAP* lpCurrentStateMap = GetStateMap();
	const STATEMAP_ENTRIES* lpEntries = lpCurrentStateMap->lpStateMapEntries;
	// 하부 객체부터 일치하는 매시지를 찾는다. 
	while( lpCurrentStateMap->lpBaseStateMap->lpStateMapEntries != lpEntries )
	{
		int i = 0;
		while(  lpEntries[ i ].pStateFunc )
		{
			if( lpEntries[ i ].dwAIStateId == dwState )
			{
				return lpEntries[ i ].pStateFunc;
			}
			i++;
		}
		// 발견되지 않았다! 상위 객체로 포인터를 바꾼다.
		lpCurrentStateMap = lpCurrentStateMap->lpBaseStateMap;
		lpEntries = lpCurrentStateMap->lpStateMapEntries;
	}
	return NULL;
}

#ifdef __POSTAIMSG1008

void CAIInterface::RouteMessage( long lIdx )
{
	AIMSG msg;
#ifdef __DEADLOCK__
	msg.lIdx	= lIdx;
#endif	// __DEADLOCK__
	m_AddRemoveLock.Enter( theLineFile );
	while( !m_MsgQueue.empty() )
	{
		msg	= m_MsgQueue.front();
		m_MsgQueue.pop();
		if( msg.dwMessage == AIMSG_SETSTATE )
		{
			m_dwCurrentState	= msg.dwParam1;
			msg.dwMessage	= AIMSG_INIT;
			m_pStateFunc	= GetAIStateFunc( msg.dwParam1 );
		}
		if( m_pStateFunc )
			( this->*( m_pStateFunc ) ) ( msg );
	}
	if( m_pStateFunc )
	{
		msg.dwMessage	= AIMSG_PROCESS;
		msg.dwParam1	= 0;
		msg.dwParam2	= 0;
		( this->*( m_pStateFunc ) ) ( msg );
	}
	m_AddRemoveLock.Leave( theLineFile );
}

void CAIInterface::SendAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
{
	AIMSG msg;
	msg.dwMessage	= dwMessage;
	msg.dwParam1	= dwParam1;
	msg.dwParam2	= dwParam2;
	if( dwMessage == AIMSG_SETSTATE )
	{
		m_dwCurrentState	= dwParam1;
		msg.dwMessage	= AIMSG_INIT;
		m_pStateFunc	= GetAIStateFunc( dwParam1 );
	}
	if( m_pStateFunc )
		( this->*( m_pStateFunc ) ) ( msg );
}

void CAIInterface::PostAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
{
	AIMSG msg;
	msg.dwMessage	= dwMessage;
	msg.dwParam1	= dwParam1;
	msg.dwParam2	= dwParam2;
	m_AddRemoveLock.Enter( theLineFile );
	m_MsgQueue.push( msg );
	m_AddRemoveLock.Leave( theLineFile );
}
#else	// __POSTAIMSG1008
void CAIInterface::SendAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
{
#ifdef __POSTAIMSG0927
	PostAIMsg( dwMessage, dwParam1, dwParam2 );
#else	// __POSTAIMSG0927
	//AIMSG AIMsg;
	//m_MsgQueue.push( AIMsg );
	// 상태를 세팅하는 매시지라면 초기화 매시지를 추가한다.
	if( dwMessage == AIMSG_SETSTATE )
	{
		m_dwCurrentState = dwParam1;
		m_AIMsg.dwMessage = AIMSG_INIT;
		m_AIMsg.dwParam1 = dwParam1;
		m_AIMsg.dwParam2 = dwParam2;
		m_pStateFunc = GetAIStateFunc( dwParam1 );
	}
	else
	{
		m_AIMsg.dwMessage = dwMessage;
		m_AIMsg.dwParam1 = dwParam1;
		m_AIMsg.dwParam2 = dwParam2;

	}
	if( m_pStateFunc )
		( this->*( m_pStateFunc ) )( );
//		SendAIMsg( AIMSG_INIT );
#endif	// __POSTAIMSG0927
}

void CAIInterface::PostAIMsg( DWORD dwMessage, DWORD dwParam1, DWORD dwParam2 )
{
#ifdef __POSTAIMSG0927
	AIMSG	AIMsg;
	AIMsg.dwMessage		= dwMessage;
	AIMsg.dwParam1	= dwParam1;
	AIMsg.dwParam2	= dwParam2;
	m_AddRemoveLock.Enter( theLineFile );
	m_MsgQueue.push( AIMsg );
	m_AddRemoveLock.Leave( theLineFile );
#else	// __POSTAIMSG0927
	AIMSG AIMsg;
	AIMsg.dwMessage = dwMessage;
	AIMsg.dwParam1 = dwParam1;
	AIMsg.dwParam2 = dwParam2;
	m_MsgQueue.push( AIMsg );
	// 상태를 세팅하는 매시지라면 초기화 매시지를 추가한다.
	if( dwMessage == AIMSG_SETSTATE )
		SendAIMsg( AIMSG_INIT );
#endif	// __POSTAIMSG0927
}
#endif	// __POSTAIMSG1008

// pObjCenter를 중심으로 nRangeMeter반경내에 들어오는넘들중 가장먼저 걸리는 넘 골라냄/
#ifdef __XAI1013
// nJobCond : 해당 직업만 감시함. 직업검사 안함.
#ifdef __V050324_AISCAN
CMover* CAIInterface::ScanTarget( CObj* pObjCenter, int nRangeMeter, int nJobCond, DWORD dwQuest, DWORD dwItem, int nChao )
#else // __V050324_AISCAN
//{{AFX
CMover* CAIInterface::ScanTarget( CObj* pObjCenter, int nRangeMeter, int nJobCond )
#endif // __V050324_AISCAN
#else
CMover* CAIInterface::ScanTarget( CObj* pObjCenter, int nRangeMeter  )
//}}AFX
#endif
{
	CObj* pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
#ifdef __XAI1013
	CMover *pTarget = NULL;
#ifdef __V050324_AISCAN
	int   nAble = 0;
	BOOL bCondScan = FALSE;
	if( nJobCond || dwQuest || dwItem || nChao )
		bCondScan = TRUE;
#else
	BOOL	bAble = TRUE;
#endif
#endif
	// 지름 
	int nRadius = nRangeMeter * nRangeMeter;

	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, nRangeMeter, CObj::linkPlayer )
	{
#ifdef __XAI1013
#ifdef __V050324_AISCAN
		// 조건 스켄이 아니라면 늘 가능하다.
		nAble = 0;
		// 조건 스켄이라면 프랍과 일치하는 놈만 가능하다.
#ifdef __GM_COMMAND_ITEM
		if( bCondScan == TRUE && pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPlayer() && ((CMover*)pObj)->IsLive() && !((CMover*)pObj)->m_SkillState.HasLikeItemBuf( IK3_TEXT_DISGUISE ) ) 
#else // __GM_COMMAND_ITEM
		if( bCondScan == TRUE && pObj->GetType() == OT_MOVER && ((CMover *)pObj)->IsPlayer() && ((CMover*)pObj)->IsLive() ) 
#endif // __GM_COMMAND_ITEM
		{
			pTarget = (CMover *)pObj;
			// 직업 조건 체크 1
			if( nJobCond == 0 )
				nAble++;
			else
			if( pTarget->GetJob() == nJobCond || nJobCond == JOB_ALL )		// JOB_ALL : 모든직업
				nAble++;
			// 아이템 조건 체크  2
			if( dwItem == 0 )
				nAble++;
			else
			if( pTarget->GetItemNum( dwItem ) != 0 )	
				nAble++;
			// 퀘스트 조건 체크  3
			if( dwQuest == 0 )
				nAble++;
			else
			if( pTarget->GetQuest( dwQuest ) != NULL )
				nAble++;	
 #ifdef __NPP_050308
			// 카오 조건 체크  4
			if( nChao == 0 )
				nAble++;
			else
			{
				// 100 = 카오유저, 101 = 비카오
				if( nChao == 100 )
				{
					if( pTarget->IsGuardReactionChao() )
						nAble++;
				}
				else
				if( nChao == 101 )
				{
					if( pTarget->IsGuardReactionNormal() )
						nAble++;
				}
			}
		}
		if( nAble == 4 )
 #else //__NPP_050308
		}
		if( nAble == 3 )
 #endif
#else // __V050324_AISCAN
//{{AFX
		bAble = TRUE;
		if( pObj->GetType() == OT_MOVER )
		{
			pTarget = (CMover *)pObj;
			if( pTarget->IsPlayer() )	// 상대가 플레이어 && 직업검사 조건이 있고.
			{
				if( nJobCond )
				{
#ifdef __NPP_050308
					// 100 = 카오유저, 101 = 비카오
					if( nJobCond == 100 )
					{
						if( !pTarget->IsChaotic() )
							bAble = FALSE;
					}
					else
					if( nJobCond == 101 )
					{
						if( pTarget->IsChaotic() )
							bAble = FALSE;
					}
					else
#endif //__NPP_050308
					if( pTarget->GetJob() != nJobCond )	// 상대가 플레이어고 타겟이 AI조건과 다르면
					{
						bAble = FALSE;									// 검사안함.
					}
				}
			}
		}
		if( bAble )
//}}AFX
#endif //__V050324_AISCAN
#endif // xAI1013
		{
			if( (GetMover()->IsFlyingNPC() && ((CMover*)pObj)->IsFly()) ||	// 비행몹은 비행플레이어만 공격
				(GetMover()->IsFlyingNPC() == FALSE && ((CMover*)pObj)->IsFly() == FALSE) 
	#ifdef __NPP_050308
				|| GetMover()->GetProp()->dwClass == RANK_GUARD		// 가드는 무조건 공격
	#endif
				)	// 지상몹은 지상플레이어만만 공격.
			{
				vDist = pObj->GetPos() - vPos;	// 두좌표간 벡터
	//			vDist.y = 0.0f;
				float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
				
				if( fDistSq < nRadius )	
				{
					if(  !( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) ) )
						return (CMover*)pObj;
				}
			}
		}
	}
	END_LINKMAP
//EXIT:
	//return aMover[ xRandom( nCount ) ();
	return NULL;
}

// 반경내에서 가장 강한넘(레벨로)을 골라냄.
CMover* CAIInterface::ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter  )
{
	CObj *pObj;
	CObj *pObjMax = NULL;	// 가장쎈넘 포인터.
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	//	CMover aMover[ 100 ];
	//	int nCount = 0;
	
	// 지름 
	FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, fRangeMeter / MPU, CObj::linkPlayer )
	{
		vDist = pObj->GetPos() - vPos;	// 두좌표간 벡터
//		vDist.y = 0.0f;
		float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
		if( fDistSq < fRadius )	
		{
			if(  !( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) ) )
			{
				if( pObjMax )
				{
					if( ((CMover *)pObj)->GetLevel() > ((CMover *)pObjMax)->GetLevel() )		// 더 쎈넘을 찾았다.
						pObjMax = pObj;
				} else
					pObjMax = pObj;

			}
		}
	}
	END_LINKMAP

	return (CMover *)pObjMax;
}

// 반경내에서 오버힐 하는 어시 찾아서 죽이자.
CMover* CAIInterface::ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  )
{
#ifdef __WORLDSERVER
	CObj *pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	
	// 지름 
	FLOAT fRadius = fRangeMeter * fRangeMeter;
	
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, fRangeMeter / MPU, CObj::linkPlayer )
	{
		vDist = pObj->GetPos() - vPos;	// 두좌표간 벡터
//		vDist.y = 0.0f;
		float fDistSq = D3DXVec3LengthSq( &vDist );		// 두오브젝트간의 거리Sq
		if( fDistSq < fRadius )	
		{
			if(  !( ((CMover*)pObj)->IsMode( TRANSPARENT_MODE ) ) )
			{
				CMover *pMover = (CMover *)pObj;

				if( pMover->IsPlayer() )
					if( ((CUser *)pMover)->m_nOverHeal > 0 )		// 오버힐러를 찾았다.
						return pMover;
			}
		}
	}
	END_LINKMAP
#endif //WORLDSERVER		
	return NULL;
}


CMover* CAIInterface::ScanNpc( CObj* pObjCenter, DWORD dwObjIdx, int nRangeMeter  )
{
	CObj* pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	FOR_LINKMAP( pObjCenter->GetWorld(), vPos, pObj, nRangeMeter / MPU, CObj::linkDynamic )
	{
		if( pObj != pObjCenter && pObj->GetType() == OT_MOVER && pObj->GetIndex() == dwObjIdx )
			return (CMover*)pObj;
	}
	END_LINKMAP
	return NULL;
}
BOOL CAIInterface::IsValidRange( D3DXVECTOR3* pvPosBegin, D3DXVECTOR3* pvPosCur, FLOAT fRange  )
{
	if( pvPosBegin->x - fRange > pvPosCur->x && pvPosBegin->x + fRange < pvPosCur->x &&
		 pvPosBegin->y - fRange > pvPosCur->y && pvPosBegin->y + fRange < pvPosCur->y )
		 return TRUE;
	return FALSE;
}