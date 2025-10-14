// AIPet.cpp : 
//

#include "stdafx.h"

#include "AIPet.h"

#include "lang.h"
#ifdef __WORLDSERVER
 #include "User.h"
 #include "dpcoreclient.h"
 extern	CUserMng	g_UserMng;
 extern	CDPCoreClient	g_DPCoreClient;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_RAGE
};
BEGIN_AISTATE_MAP( CAIPet, CAIInterface )

	ON_STATE( STATE_INIT   , StateInit   )
	ON_STATE( STATE_IDLE   , StateIdle   )
	ON_STATE( STATE_RAGE   , StateRage   )

END_AISTATE_MAP()

#define		PETSTATE_IDLE		0
#define		PETSTATE_TRACE		1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
MemPooler<CAIPet>*	CAIPet::m_pPool		= new MemPooler<CAIPet>( 512 );
#else	// __WORLDSERVER
 #ifndef __ONLINE
 MemPooler<CAIPet>*	CAIPet::m_pPool		= new MemPooler<CAIPet>( 128 );
 #else	// not Online(Single)
 MemPooler<CAIPet>*	CAIPet::m_pPool		= NULL; //new MemPooler<CAIPet>( 128 );
 #endif // onLine
#endif	// __WORLDSERVER

CAIPet::CAIPet()
{
	Init();
}
CAIPet::CAIPet( CObj* pObj ) : CAIInterface( pObj, 0 )
{
	Init();
}

CAIPet::~CAIPet()
{
	Destroy();
}

void CAIPet::Init( void )
{
	m_idOwner = NULL_ID;
	m_bLootMove = FALSE;				
	m_idLootItem = NULL_ID;				
	m_nState = PETSTATE_IDLE;
}

void CAIPet::Destroy( void )
{
	// 여기다 파괴코드를 넣을것.


	Init();
}

void CAIPet::InitAI()
{
#ifdef __POSTAIMSG1008
	PostAIMsg( AIMSG_SETSTATE, STATE_IDLE ); 
#else	
	SendAIMsg( AIMSG_SETSTATE, STATE_IDLE ); 
#endif	
}

// vDst쪽으로 이동.
void CAIPet::MoveToDst(	D3DXVECTOR3 vDst )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	pMover->SetDestPos( vDst );
	pMover->m_nCorr		= -1;
#ifdef __WORLDSERVER
	g_UserMng.AddSetDestPos( pMover, vDst, 1 );
#endif	// __WORLDSERVER
	
}

// idTarget쪽으로 이동.
void CAIPet::MoveToDst(	OBJID idTarget )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestId() == idTarget )
		return;
	pMover->SetDestObj( idTarget ); // 목표를 재설정해준다.
#ifdef __WORLDSERVER
	g_UserMng.AddMoverSetDestObj( (CMover*)pMover, idTarget );
#endif	// __WORLDSERVER
}


// 근처의 아이템을 집으러 간다.
BOOL CAIPet::SubItemLoot( void )
{
	CMover* pMover = GetMover();
	CMover* pOwner = prj.GetMover( m_idOwner );
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	D3DXVECTOR3 vPos = pMover->GetPos();
	CObj *pObj = NULL;
	int nRange = 0;
	D3DXVECTOR3 vDist;
	FLOAT fDistSq, fMinDist = 9999999.0f;
	CObj *pMinObj = NULL;

	vDist = pOwner->GetPos() - pMover->GetPos();
	fDistSq = D3DXVec3LengthSq( &vDist );
	if( fDistSq > 32.0f * 32.0f )	// 주인님과의 거리가 32미터가 넘으면 아이템 안집는다.
		return FALSE;
		

	// 근처의 아이템을 검색함. - 주인님꺼만 검색해야할듯...
	FOR_LINKMAP( pWorld, vPos, pObj, nRange, CObj::linkDynamic )
	{
		if( pObj->GetType() == OT_ITEM )	// 아템만 검색
		{
			CItem *pItem = (CItem *)pObj;
			ItemProp* pItemProp	= pItem->GetProp();
			// 이걸 따로 넣은이유는 StateIdle ARRIVAL에서 DoLoot()하고 난직후에 다시 SubItemLoot()을 호출했을때
			// Loot한 아이템이 아직 안지워져서 여기서 또 검색이 되더라고.. 그래서 중복되는 아이템은 검색 안되게 고쳐봤다.
//			if( pItem->GetId() != m_idLootItem )		
			if( pItem->IsDelete() == FALSE )
			{
				if( pItemProp )
				{
					if( pOwner->IsLoot( pItem ) )	// 루팅되는아이템인지 검사함.
					{
						vDist = pObj->GetPos() - pMover->GetPos();
						fDistSq = D3DXVec3LengthSq( &vDist );		// 거리 구함.
						if( fDistSq < 15 * 15 && fDistSq < fMinDist )	// 10미터 이내고... 가장 거리가 가까운 아템을 찾음.
							pMinObj = pObj;
					}
				}
			}
		}
	}
	END_LINKMAP

	if( pMinObj )
	{
		MoveToDst( pMinObj->GetPos() );		// 목표쪽으로 이동.
		m_idLootItem = ((CItem *)pMinObj)->GetId();
		m_bLootMove = TRUE;
		
	}
		
	return m_bLootMove;
}

#ifdef __POSTAIMSG1008
BOOL CAIPet::StateInit( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIPet::StateInit()
#endif	// __POSTAIMSG1008
{
	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIPet::MoveProcessIdle( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIPet::MoveProcessIdle()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int _i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int _i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif
	CMover *pMover = GetMover();
	CMover *pOwner = prj.GetMover( m_idOwner ); 
	CWorld *pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

#ifdef __CLIENT
	if( pWorld->GetLandscape( pMover->GetPos() ) == NULL )
		return FALSE;
#endif
	
	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	// 주인님이 사라졌으면 나도 사라지자.
	if( IsInvalidObj( pOwner ) )	
	{
		pMover->Delete();
		return FALSE;
	}

	if( m_bLootMove == FALSE )	// 루팅하러 갈땐 아래 처리를 하면 안되긋제...
	{
		// 주인님과의 거리가 멀어지면 주인님쪽으로 달려간다.
		if( m_nState == PETSTATE_IDLE )
		{
			D3DXVECTOR3 vDist = pOwner->GetPos() - pMover->GetPos();
			FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
			if( fDistSq > 1.0f * 1.0f )
			{
				MoveToDst( m_idOwner );
				m_nState = PETSTATE_TRACE;
			}
		} else
		if( m_nState == PETSTATE_TRACE )
		{
			if( pOwner->IsRangeObj( pMover, 0 ) == TRUE )		// 주인님 곁으로 다가갔다.
			{
				m_nState = PETSTATE_IDLE;			// 대기상태로 전환
				pMover->SendActMsg( OBJMSG_STOP );	// 모션 정지
				pMover->ClearDest();				// 이동목표 클리어.
			}
		}
	}

	// 아이템 스캔/루팅
	{
#ifdef __XAI1013
		{
			if( (pMover->GetCount() & 15) == 0 )		// 가끔씩 주변을 스캔해서... 서버니까 15가 1초
			{	
				if( m_bLootMove == FALSE )		// 루팅하러 가고 있을때 또 체크하면 안뒘
					if( SubItemLoot() )		// 아이템을 루팅함.
						m_nState = PETSTATE_IDLE;	// 루팅시작했으면 대기상태로 바꿔야함.
			}
		}

		if( m_bLootMove == TRUE )
		{
			CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		// 그아이템이 사라졌을수 있으니까 검사함.
			if( IsInvalidObj(pCtrl) )		// 아이템 집으러 이동중에 아템이 없어지면
			{
				MoveToDst( pMover->GetPos() );	// 제자리에 섬.
				m_bLootMove = FALSE;
				m_idLootItem = NULL_ID;
			}
		}
#endif // xAI1013
	}

	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIPet::StateIdle( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIPet::StateIdle()
#endif	// __POSTAIMSG1008
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

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 
//		SetStop( SEC( 0 ) );
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
//		m_dwIdTarget = NULL_ID;
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
#ifdef __POSTAIMSG1008
		MoveProcessIdle( msg );
#else	
		MoveProcessIdle();
#endif	

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
		SendAIMsg( AIMSG_EXIT );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
		{
#ifdef __WORLDSERVER	
			if( m_bLootMove )	// 아이템 루팅모드였음.
			{
				CMover *pOwner = prj.GetMover( m_idOwner );
				if( IsValidObj(pOwner) )
				{
					BOOL bSuccess = FALSE;
					CCtrl *pCtrl = prj.GetCtrl( m_idLootItem );		// 그아이템이 사라졌을수 있으니까 검사함.
					if( IsValidObj(pCtrl) )
					{
						CItem *pItem = (CItem *)pCtrl;
						D3DXVECTOR3 vDist = pCtrl->GetPos() - pMover->GetPos();
						FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
						if( fDistSq < 3.0f * 3.0f )		// 도착해서 거리 검사 한번더 해서 
						{
							pOwner->Lock( theLineFile );
							pItem->Lock( theLineFile );	// lock
							if( pItem->IsDelete() )
							{
								pItem->Unlock( theLineFile );	// unlock
								pOwner->Unlock( theLineFile );	// unlock
								return TRUE;
							}
							CItemElem* pItemElem = (CItemElem *)pItem->m_pItemBase;
							ItemProp *pItemProp = pItem->GetProp();
							// 주인이 아이템을 줍는것과 같은 효과를 냄.
							bSuccess = pOwner->DoLoot( pItem );
							
							pItem->Unlock( theLineFile );	// unlock
							pOwner->Unlock( theLineFile );
						}
					}
					if( bSuccess )
					{
						if( SubItemLoot() == FALSE )		// 아이템집고 주위를 한번더 스캔해서 먹을거 있으면 다시가고 없으면 주인에게 돌아간다.
						{
							m_bLootMove = FALSE;		// this가 아이템 집는중이라는거 해제.
							m_idLootItem = NULL_ID;
						}
					} else
					{
						m_bLootMove = FALSE;		// this가 아이템 집는중이라는거 해제.
						m_idLootItem = NULL_ID;
					}
				}
			}
#endif // WorldServer
		}
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



#ifdef __POSTAIMSG1008
BOOL CAIPet::MoveProcessRage( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIPet::MoveProcessRage()
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
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	return TRUE;
}

#ifdef __POSTAIMSG1008
BOOL CAIPet::StateRage( const AIMSG & msg )
#else	// __POSTAIMSG1008
BOOL CAIPet::StateRage()
#endif	// __POSTAIMSG1008
{
#ifdef __DEADLOCK__
#ifdef __POSTAIMSG1008
	int	_i	= msg.lIdx;
#else	// __POSTAIMSG1008
	int	_i	= m_AIMsg.lIdx;
#endif	// __POSTAIMSG1008
#endif

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	if( IsInvalidObj(pMover) )	return FALSE;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		// 타겟을 클리어하고 대기모드로 돌아감.
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
#ifdef __POSTAIMSG1008
		MoveProcessRage( msg );
#else	// __POSTAIMSG1008
		MoveProcessRage();
#endif	// __POSTAIMSG1008
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDAMAGE ) 
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );


	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_NOMOVE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_BEGINMOVE )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	
//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

	EndAIHandler( )
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

