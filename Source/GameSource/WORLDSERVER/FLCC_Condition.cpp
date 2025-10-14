#include "StdAfx.h"

#include <limits>

#include "FLCC_Condition.h"

#include "../_Common/FLCooperativeContributions_Common.h"

namespace nsCooperativeContributions {


FLItemEndCondition_Global::FLItemEndCondition_Global( const DWORD dwTextID, const DWORD dwItemID, const DWORD dwItemCountMax, const int nContributionUnit )
: m_dwTextID( dwTextID )
, m_dwItemID( dwItemID )
, m_dwItemMax( dwItemCountMax )
, m_dwItemContributed( 0 )
, m_nContributionUnit( nContributionUnit )
{
	FLASSERT( 0 < m_nContributionUnit && m_nContributionUnit <= INT_MAX );
}

FLItemEndCondition_Global::~FLItemEndCondition_Global()
{
}

//DWORD FLItemEndCondition_Global::GetItemID() const
//{
//	return m_dwItemID;
//}
//
//DWORD FLItemEndCondition_Global::GetContributed() const
//{
//	return m_dwItemContributed;
//}
//
//DWORD FLItemEndCondition_Global::GetConditionMax() const
//{
//	return m_dwItemMax;
//}

//size_t FLItemEndCondition_Global::GetContributionUnit() const
//{
//	return m_nContributionUnit;
//}


void FLItemEndCondition_Global::SerializeOut_Info( CAr & ar )
{
	ar << CONTRIBUTION_ITEM;
	FLCooperativeContributions_Common::SerializeInfo_GlobalItemEndCondition( ar, m_dwItemID, m_dwItemMax, m_nContributionUnit, min( m_dwItemMax, m_dwItemContributed ) );
}

BOOL FLItemEndCondition_Global::IsSameInfo( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind )
{
	return ( eContributionKind == CONTRIBUTION_ITEM && dwIndexOfKind == m_dwItemID );
}


BOOL FLItemEndCondition_Global::IsComplete( const DWORD /*dwContributionID*/ )
{
	if( m_dwItemMax > m_dwItemContributed )
		return FALSE;
	return TRUE;
}

BOOL FLItemEndCondition_Global::Contribute( FLWSUser & User )
{
	if( User.TryRemoveNotUsingItem( m_dwItemID, m_nContributionUnit ) == FALSE )
	{
		User.AddDefinedText( TID_MMI_COOPERATION_DONATION04 );
		return FALSE;
	}

	return TRUE; 
}

BOOL FLItemEndCondition_Global::Query_Contribute( const DWORD dwContributionID, const FLTm & tmStartDate, const FLTm & tmEndDate, const u_long idPlayer, const TCHAR * szName )
{
	T_W2DB_PACKET_CONTRIBUTE_REQ contribute;
	contribute.idPlayer			= idPlayer;
	FLStrcpy( contribute.szName, _countof( contribute.szName ), szName );
	contribute.dwContributionID	= dwContributionID;
	contribute.tmStart			= tmStartDate;
	contribute.tmEnd			= tmEndDate;
	contribute.eContributionKind= CONTRIBUTION_ITEM;
	contribute.dwIndexOfKind	= m_dwItemID;
	contribute.dwContiributionCount = static_cast< DWORD >( m_nContributionUnit );
	contribute.dwContiributionMax = m_dwItemMax;

	BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &contribute, sizeof( contribute ) );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	return TRUE;
}

void FLItemEndCondition_Global::SetContributed( const DWORD dwContributedCount )
{
	m_dwItemContributed	= dwContributedCount;
}

//DWORD FLItemEndCondition_Global::GetTextID() const
//{
//	return m_dwTextID;
//}

//------------------------------------------------------------------------------------------------------------------------------------//


FLGoldEndCondition_Global::FLGoldEndCondition_Global( const DWORD dwTextID, const int nGoldMax, const int nContributionUnit ) 
: m_dwTextID( dwTextID )
, m_nGoldMax( nGoldMax )
, m_dwContributionUnit( nContributionUnit )
, m_nContributedGold( 0 )
{
	FLASSERT( 0 < m_dwContributionUnit && m_dwContributionUnit <= INT_MAX );
}

FLGoldEndCondition_Global::~FLGoldEndCondition_Global()
{
}

void FLGoldEndCondition_Global::SerializeOut_Info( CAr & ar )
{
	ar << CONTRIBUTION_GOLD;
	FLCooperativeContributions_Common::SerializeInfo_GlobalGoldEndCondition( ar, m_nGoldMax, m_dwContributionUnit, min( m_nGoldMax, m_nContributedGold ) );
}


//int FLGoldEndCondition_Global::GetContributed() const
//{
//	return m_nContributedGold;
//}
//
//int FLGoldEndCondition_Global::GetConditionMax() const
//{
//	return m_nGoldMax;
//}


//size_t FLGoldEndCondition_Global::GetContributionUnit() const
//{
//	return m_dwContributionUnit;
//}


BOOL FLGoldEndCondition_Global::IsSameInfo( const E_CONTRIBUTION_KIND eContributionKind, const DWORD /*dwIndexOfKind*/ )
{
	return ( eContributionKind == CONTRIBUTION_GOLD );
}


BOOL FLGoldEndCondition_Global::IsComplete( const DWORD /*dwContributionID*/ )
{
	if( m_nContributedGold < m_nGoldMax )
		return FALSE;
	return TRUE;
}


BOOL FLGoldEndCondition_Global::Contribute( FLWSUser & User )
{
	if( User.GetGold() < static_cast< int >( m_dwContributionUnit ) )
	{
		User.AddDefinedText( TID_MMI_COOPERATION_DONATION05 );
		return FALSE;
	}

	return User.AddGold( -( int )m_dwContributionUnit );
}

void FLGoldEndCondition_Global::SetContributed( const DWORD dwContributedCount )
{
	m_nContributedGold	= dwContributedCount;
}

BOOL FLGoldEndCondition_Global::Query_Contribute( const DWORD dwContributionID, const FLTm & tmStartDate, const FLTm & tmEndDate, const u_long idPlayer, const TCHAR * szName )
{
	T_W2DB_PACKET_CONTRIBUTE_REQ contribute;
	contribute.idPlayer			= idPlayer;
	FLStrcpy( contribute.szName, _countof( contribute.szName ), szName );
	contribute.dwContributionID	= dwContributionID;
	contribute.tmStart			= tmStartDate;
	contribute.tmEnd			= tmEndDate;
	contribute.eContributionKind= CONTRIBUTION_GOLD;
	contribute.dwIndexOfKind	= 0;
	contribute.dwContiributionCount = static_cast< DWORD >( m_dwContributionUnit );
	contribute.dwContiributionMax = static_cast< DWORD >( m_nGoldMax );

	BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_CONTRIBUTE_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &contribute, sizeof( contribute ) );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	return TRUE;
}

//DWORD FLGoldEndCondition_Global::GetTextID() const
//{
//	return m_dwTextID;
//}

	//-----------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------------------------//

FLJobBeginCondition_User::FLJobBeginCondition_User()
{
	m_vecJob.reserve( MAX_JOB );				//퍼포먼스 향상 -_-
}

FLJobBeginCondition_User::~FLJobBeginCondition_User()
{
	m_vecJob.clear();
}

void FLJobBeginCondition_User::RegistJob( const int nJob )
{
	m_vecJob.push_back( nJob );
}

BOOL FLJobBeginCondition_User::IsValid( FLWSUser & User )
{
	for( JobVec::const_iterator it = m_vecJob.begin(); it != m_vecJob.end(); ++it )
	{
		const int nJob		= *it;
		if( User.GetJob() == nJob )
			return TRUE;
	}

	User.AddDefinedText( TID_MMI_COOPERATION_DONATION07 );
	return FALSE;
}

//-----------------------------------------------------------------------------------------------------//

FLLevelBeginConditioin_User::FLLevelBeginConditioin_User( const BYTE byMinLevel, const BYTE byMaxLevel ) 
: m_byMinLevel( byMinLevel ), m_byMaxLevel( byMaxLevel )
{
	RELEASE_ASSERT( m_byMinLevel <= m_byMaxLevel );
}

FLLevelBeginConditioin_User::~FLLevelBeginConditioin_User()
{
}

BOOL FLLevelBeginConditioin_User::IsValid( FLWSUser & User )
{
	const BOOL bRet = m_byMinLevel <= User.GetLevel() && User.GetLevel() <= m_byMaxLevel;
	if( bRet == FALSE )
	{
		User.AddDefinedText( TID_MMI_COOPERATION_DONATION06 );
	}

	return bRet;
}

} //nsCooperativeContributions