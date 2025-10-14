#include "FLRandomRespawnNPC.h"

#include "../_CommonDefine/FLTm.h"

#include <string>

#include "worldmng.h"
extern	CWorldMng	g_WorldMng;

#include "defineObj.h"


//----------------------------------------------------------------------------------------------------------------------------------------------//
template< class _BaseClass >
FLRandomRespawnNPCAlarm< _BaseClass >::FLRandomRespawnNPCAlarm()
{
	m_data.m_dwMoverIndex	= NULL_ID;
	m_data.m_bActiveAttack	= FALSE;
	

	m_objid					= NULL_ID;
}

template< class _BaseClass >
FLRandomRespawnNPCAlarm< _BaseClass >::~FLRandomRespawnNPCAlarm()
{
	m_data.m_vecRandomPos.clear();
}

template< class _BaseClass >
BOOL FLRandomRespawnNPCAlarm< _BaseClass >::SetData( const std::string & szAlarmName, const FLTm & startDate, const FLTm & endDate, const FLTm & startTime, const FLTm & endTime
	, const DWORD dwMoverIndex, const std::string & strCharacterKey, const BOOL bActiveAttack, FLRandomRespawnNPC_Data::VecRandomPos & vecPos )
{
	m_strName					= szAlarmName;

	if( Initialize( startDate, endDate, startTime, endTime ) == FALSE )
		return FALSE;

	m_data.m_dwMoverIndex		= dwMoverIndex;
	m_data.m_strCharacterKey	= strCharacterKey;
	m_data.m_bActiveAttack		= bActiveAttack;
	m_data.m_vecRandomPos	= vecPos;
	return TRUE;
}

template< class _BaseClass >
BOOL FLRandomRespawnNPCAlarm< _BaseClass >::OnStart()
{
	//예외 처리
	if( m_data.m_dwMoverIndex == NULL_ID )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "[ INVALID MOVER INDEX ][ MoverIndex(%u), sdate(%s), edate(%s), stime(%s), etime(%s) ]" ), m_data.m_dwMoverIndex
			, FLTmToString( getStartDate(), FLTm::E_FORMAT::YYYYMMDD ).c_str(), FLTmToString( getEndDate(), FLTm::E_FORMAT::YYYYMMDD ).c_str()
			, FLTmToString( getStartTime(), FLTm::E_FORMAT::HHMMSS ).c_str(), FLTmToString( getEndTime(), FLTm::E_FORMAT::HHMMSS ).c_str() );
		return FALSE;
	}
	
	MoverProp * pProp = prj.GetMoverProp( m_data.m_dwMoverIndex );	
	if( pProp == NULL )
	{
		FLTRACE_LOG( PROGRAM_NAME, _T( "[ INVALID MOVER INDEX ][ MoverIndex(%u), sdate(%s), edate(%s), stime(%s), etime(%s) ]" ), m_data.m_dwMoverIndex
			, FLTmToString( getStartDate(), FLTm::E_FORMAT::YYYYMMDD ).c_str(), FLTmToString( getEndDate(), FLTm::E_FORMAT::YYYYMMDD).c_str()
			, FLTmToString( getStartTime(), FLTm::E_FORMAT::HHMMSS ).c_str(), FLTmToString( getEndTime(), FLTm::E_FORMAT::HHMMSS ).c_str() );
		return FALSE;
	}

	// 몬스터가 아직 살아있으면 end알람만 호출 하면 됨( 기획적으로 리스폰 안해두 된다 )
	if( m_objid != NULL_ID )
	{
		CMover * pMover			= prj.GetMover( m_objid );
		if( pMover != NULL )
		{
			pMover->SetDelete_IntoIdleMode( FALSE );			//전투후 삭제 예약 된거 취소
			return TRUE;
		}
	}

	// spawn 처리
	const size_t index	= ::xRandom( m_data.m_vecRandomPos.size() );
	FLRandomRespawnNPC_Data::Pos & flpos		= m_data.m_vecRandomPos.at( index );

	CWorld * pWorld		= g_WorldMng.GetWorld( flpos.worldID );
	if( pWorld == NULL )
		return FALSE;

	flpos.pos.y	= pWorld->GetLandHeight( flpos.pos.x, flpos.pos.z );

	CMover * pMover		= NULL;
	switch( pProp->dwAI )
	{
	case AII_NONE:
		{
#ifdef __ENCHANT_BARUNA16
			pMover = CreateNPC( NULL_ID, *pWorld, m_data.m_strCharacterKey.c_str()/*_T( "Pet_Seltpiyo" )*/, flpos.pos, m_data.m_dwMoverIndex, 1, 0 );
#endif // __ENCHANT_BARUNA16
		}
		break;

	default:
		{
			pMover = static_cast< CMover * >( CreateObj( D3DDEVICE, OT_MOVER, m_data.m_dwMoverIndex ) );		//AI알아서 집어넣음.
			if( NULL == pMover )	
				break;

			pMover->SetPos( flpos.pos );
			pMover->InitMotion( MTI_STAND );
			pMover->UpdateLocalMatrix();
			pMover->m_bActiveAttack		= m_data.m_bActiveAttack;			//MoverProp에 없음
			pMover->AddItToGlobalId();

			const int DEFAULT_LAYER_ID	= 0;		//@@@@@@@@@@
			if( pWorld->ADDOBJ( pMover, FALSE, DEFAULT_LAYER_ID ) == FALSE )
			{
				SAFE_DELETE( pMover );
			}
		}
		break;
	}

	if( pMover == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAIL RANDOM RESPAWN NPC : MonsterID(%u) CharacterKey(%s) ]" ), m_data.m_dwMoverIndex, m_data.m_strCharacterKey.c_str() );
		return FALSE;
	}

	m_objid			= pMover->GetId();


#ifdef _DEBUG
	const FLTm curr = DATE_TIMER().GetCurrentTime();
	FLTRACE_LOG( PROGRAM_NAME, _T( "[ ************************************************************************************************************************** ]" ) );
	FLTRACE_LOG( PROGRAM_NAME, _T( "[ Start curr: (%u) y(%I64u) month(%I64u) day(%I64u) hour(%I64u) minute(%I64u) second(%I64u) ]" ), (DWORD)this, curr.year, curr.month, curr.day, curr.hour, curr.minute, curr.second );
#endif //_DEBUG
	return TRUE;
}

template< class _BaseClass >
BOOL FLRandomRespawnNPCAlarm< _BaseClass >::OnEnd()
{
	if( m_objid == NULL_ID )
		return TRUE;

	CMover * pMover			= prj.GetMover( m_objid );
	if( IsValidObj( pMover ) == FALSE )
	{
		m_objid					= NULL_ID;
	}
	else 
	{
		// ai가 없는 경우
		if( pMover->m_pAIInterface == NULL )
		{
			pMover->Delete();
			m_objid					= NULL_ID;
		}	
		else	//ai가 있는 경우. 
		{
			if( pMover->m_pAIInterface->IsIdleMode() == TRUE )		// 전투중이 아니라면
			{
				pMover->Delete();
				m_objid					= NULL_ID;
			}
			else													// 전투중이라면
			{
				pMover->SetDelete_IntoIdleMode( TRUE );
			}
		}
	}

#ifdef _DEBUG
	const FLTm curr = DATE_TIMER().GetCurrentTime();
	FLTRACE_LOG( PROGRAM_NAME, _T( "[ End : (%u) y(%I64u) month(%I64u) day(%I64u) hour(%I64u) minute(%I64u) second(%I64u) ]" ), (DWORD)this, curr.year, curr.month, curr.day, curr.hour, curr.minute, curr.second );
#endif //_DEBUG		

	return TRUE;
}

template< class _BaseClass >
void FLRandomRespawnNPCAlarm< _BaseClass >::OnExpired()
{
#ifdef _DEBUG
//	const FLTm curr = DATE_TIMER().GetCurrentTime();
//	FLTRACE_LOG( PROGRAM_NAME, _T( "[ Expired : (%u) y(%I64u) month(%I64u) day(%I64u) hour(%I64u) minute(%I64u) second(%I64u) ]" ), (DWORD)this, curr.year, curr.month, curr.day, curr.hour, curr.minute, curr.second );
//	FLTRACE_LOG( PROGRAM_NAME, _T( "[ /-----------------------------------------------------------------------------------------------------------------------------------------/ ]" ) );
#endif //_DEBUG		
	__noop;
}

template< class _BaseClass >
void FLRandomRespawnNPCAlarm< _BaseClass >::OnError()
{
#ifdef _DEBUG
	//const FLTm curr = DATE_TIMER().GetCurrentTime();
	//FLTRACE_LOG( PROGRAM_NAME, _T( "[ Error : (%u) y(%I64u) month(%I64u) day(%I64u) hour(%I64u) minute(%I64u) second(%I64u) ]" ), (DWORD)this, curr.year, curr.month, curr.day, curr.hour, curr.minute, curr.second );
#endif //_DEBUG		
}