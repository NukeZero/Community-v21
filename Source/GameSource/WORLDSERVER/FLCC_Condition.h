#ifndef __FLCOOPERATIVE_CONTRIBUTIONS_CONDITION_H__
#define __FLCOOPERATIVE_CONTRIBUTIONS_CONDITION_H__


class FLWSUser;

//#include "FLCooperativeContributions.h"

#include "FLQuestInterface.h"

namespace nsCooperativeContributions {


class FLItemEndCondition_Global : public IGlobalEndCondition
{
public:
	FLItemEndCondition_Global( const DWORD dwTextID, const DWORD dwItemID, const DWORD dwItemCountMax, const int nContributionUnit );
	virtual ~FLItemEndCondition_Global();

	virtual void			SerializeOut_Info( CAr & ar );							

	virtual BOOL			Contribute( FLWSUser & User );						
	virtual void			SetContributed( const DWORD dwContributedCount );	
	
	virtual BOOL			IsSameInfo( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind );	
	virtual BOOL			IsComplete( const DWORD dwContributionID );			

	virtual BOOL			Query_Contribute( const DWORD dwContributionID, const FLTm & tmStartDate, const FLTm & tmEndDate, const u_long idPlayer, const TCHAR * szName );

private:
	const DWORD				m_dwTextID;
	/*const*/ DWORD				m_dwItemID;
	/*const*/ DWORD				m_dwItemMax;
	/*const*/ size_t			m_nContributionUnit;

	DWORD					m_dwItemContributed;
};


//-------------------------------------------------------------------------------------------------------------------//


class FLGoldEndCondition_Global : public IGlobalEndCondition
{
public:
	FLGoldEndCondition_Global( const DWORD dwTextID, const int nGoldMax, const int nContributionUnit );
	virtual ~FLGoldEndCondition_Global();

	virtual void			SerializeOut_Info( CAr & ar );
	virtual BOOL			Contribute( FLWSUser & User );
	virtual void			SetContributed( const DWORD dwContributedCount );

	virtual BOOL			IsSameInfo( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind );
	virtual BOOL			IsComplete( const DWORD nContributionID );

	virtual BOOL			Query_Contribute( const DWORD dwContributionID, const FLTm & tmStartDate, const FLTm & tmEndDate, const u_long idPlayer, const TCHAR * szName );

private:
	const DWORD				m_dwTextID;
	/*const*/ int				m_nGoldMax;
	/*const*/ size_t			m_dwContributionUnit;

	int						m_nContributedGold;
};



//------------------------------------------------------------------------------------------------------------------------------------//

class FLJobBeginCondition_User : public IUserBeginCondition
{
public:
	FLJobBeginCondition_User();
	virtual ~FLJobBeginCondition_User();

	void					RegistJob( const int nJob );
	virtual BOOL			IsValid( FLWSUser & User );

private:
	typedef					std::vector< int >		JobVec;
	JobVec					m_vecJob;
};


//------------------------------------------------------------------------------------------------------------------------------------//

class FLLevelBeginConditioin_User : public IUserBeginCondition
{
public:
	FLLevelBeginConditioin_User( const BYTE byMinLevel, const BYTE byMaxLevel );
	virtual ~FLLevelBeginConditioin_User();

	virtual BOOL			IsValid( FLWSUser & User );

private:
	const BYTE				m_byMinLevel;
	const BYTE				m_byMaxLevel;
};

//------------------------------------------------------------------------------------------------------------------------------------//


}	//nsCooperativeContributions


#endif //__FLCOOPERATIVE_CONTRIBUTIONS_CONDITION_H__