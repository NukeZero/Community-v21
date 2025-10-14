#include "StdAfx.h"

#include "FLCC_Contribution.h"

#include "FLCC_Condition.h"

namespace nsCooperativeContributions
{

//---------------------------------------------------------------------------------------------------------------------------------------//

FLContribution::FLContribution( const DWORD dwContributionID ) 
: m_dwID( dwContributionID )
, m_tmStartDate( INVALID_DATETIME )
, m_tmEndDate( INVALID_DATETIME )
, m_bRewarded( FALSE )
, m_dwDescription_TextID( 0 )
, m_dwEndCond_TextID( 0 )

{
}

FLContribution::~FLContribution()
{
	for( UserBeginConditionVec::iterator kItr = m_vecBeginCondition_User.begin(); kItr != m_vecBeginCondition_User.end(); ++kItr )
	{
		SAFE_DELETE( *kItr );
	}
	m_vecBeginCondition_User.clear();

	for( UserRewardVec::iterator kItr = m_vecReward_User.begin(); kItr != m_vecReward_User.end(); ++kItr )
	{
		SAFE_DELETE( *kItr );
	}
	m_vecReward_User.clear();

	for( GlobalRewardVec::iterator kItr = m_vecReward_Global.begin(); kItr != m_vecReward_Global.end(); ++kItr )
	{
		SAFE_DELETE( *kItr );
	}
	m_vecReward_Global.clear();

	for( GlobalConditionVec::iterator kItr = m_vecGlobalCondition.begin(); kItr != m_vecGlobalCondition.end(); ++kItr )
	{
		SAFE_DELETE( *kItr );
	}
	m_vecGlobalCondition.clear();
}

IGlobalEndCondition *	FLContribution::GetGlobalEndCondition()
{
	if( m_vecGlobalCondition.size() == 0 )
		return NULL;

	//for( GlobalConditionVec::iterator it = m_vecGlobalCondition.begin(); it != m_vecGlobalCondition.end(); ++it )
	//{
	//	IGlobalEndCondition * p = *it;
	//	const BOOL bRet = p->IsSameInfo( eContributionKind, dwIndexOfKind );
	//	if( bRet == TRUE )
	//		return p;
	//}
	GlobalConditionVec::iterator it		= m_vecGlobalCondition.begin();
	IGlobalEndCondition * p = *it;
	return p;
}


IGlobalEndCondition *	FLContribution::GetGlobalEndCondition(  const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind )
{
	for( GlobalConditionVec::iterator it = m_vecGlobalCondition.begin(); it != m_vecGlobalCondition.end(); ++it )
	{
		IGlobalEndCondition * p = *it;
		const BOOL bRet = p->IsSameInfo( eContributionKind, dwIndexOfKind );
		if( bRet == TRUE )
			return p;
	}
	return NULL;
}


void FLContribution::TryRewardAllUser()
{
	if( m_bRewarded == TRUE )
		return;
		
	typedef		FLContribution::GlobalRewardVec		GlobalVec;
	for( GlobalVec::const_iterator it = m_vecReward_Global.begin(); it != m_vecReward_Global.end(); ++it )
	{
		IGlobalReward * p	= *it;
		p->TryReward();
	}

	//for( g_xWSUserManager.m_users)
	for( std::map<u_long, CMover*>::const_iterator itUser = prj.m_idPlayerToUserPtr.begin(); itUser != prj.m_idPlayerToUserPtr.end(); ++itUser )
	{
		FLWSUser * pUser	= static_cast< FLWSUser * >( itUser->second );
		if( pUser == NULL )
			continue;

		//개인 보상
		for( FLContribution::UserRewardVec::iterator itUserReward = m_vecReward_User.begin(); itUserReward != m_vecReward_User.end(); ++itUserReward )
		{
			IUserReward * p = *itUserReward;
			p->TryReward( m_dwID, *pUser );
		}

		//pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION10 );
		pUser->AddDefinedText( TID_MMI_COOPERATION_DONATION11 );
	}

	m_bRewarded				= TRUE;
}


BOOL FLContribution::IsBeginCondition( FLWSUser & User ) const
{
	typedef FLContribution::UserBeginConditionVec	UserVec;
	for( UserVec::const_iterator it = m_vecBeginCondition_User.begin(); it != m_vecBeginCondition_User.end(); ++it )
	{
		IUserBeginCondition * p	= *it;
		if( p->IsValid( User ) == FALSE )
			return FALSE;
	}

	return TRUE;
}


BOOL FLContribution::IsComplete() 
{
	if( m_vecGlobalCondition.size() == 0 )
		return FALSE;

	for( GlobalConditionVec::iterator it = m_vecGlobalCondition.begin(); it != m_vecGlobalCondition.end(); ++it )
	{
		IGlobalEndCondition * p = *it;
		const BOOL bRet = p->IsComplete( m_dwID );
		if( bRet == FALSE )
			return FALSE;
	}
	return TRUE;
}

BOOL FLContribution::IsEventPeriod() const
{
	const FLTm tmCurr		= DATE_TIMER().GetCurrentTime();
	//const FLTm tmCurrDate	= FLExtractDate( tmCurr );

	return ( m_tmStartDate <= tmCurr && tmCurr <= m_tmEndDate ) ? TRUE : FALSE;
}

BOOL FLContribution::IsExpired() const
{
	const FLTm tmCurr		= DATE_TIMER().GetCurrentTime();
	return ( tmCurr > m_tmEndDate ) ? TRUE : FALSE;

//	const FLTm tmCurr		= DATE_TIMER().GetCurrentTime();
//	const FLTm tmCurrDate	= FLExtractDate( tmCurr );

//	return ( tmCurrDate > m_tmEndDate ) ? TRUE : FALSE;
}

//void FLContribution::Query_TryRewardItem()
//{
//	T_W2DB_PACKET_TRY_REWARD_ITEM_REQ req;
//	req.dwContributionID	= m_dwID;
//	req.tmStart				= m_tmStartDate;
//	req.tmEnd				= m_tmEndDate;
//
//	BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_TRY_REWARD_ITEM_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
//	ar.Write( &req, sizeof( req ) );
//	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
//}

} /* nsCooperativeContributions */