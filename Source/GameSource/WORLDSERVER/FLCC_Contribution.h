#ifndef __FLCONTRIBUTION_H__
#define __FLCONTRIBUTION_H__


#include "../_CommonDefine/FLTm.h"
#include "../_CommonDefine/FLStdContainer.h"
#include "../_Common/post.h"
#include "FLQuestInterface.h"

#include <boost/noncopyable.hpp>

namespace nsCooperativeContributions
{

// 기부 조건 // 보상 // 기부된 정보
class FLContribution : boost::noncopyable
{
public:
	FLContribution( const DWORD dwContributionID );
	virtual ~FLContribution();

	//BOOL					HasEndCondition( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind );
	IGlobalEndCondition *	GetGlobalEndCondition();
	IGlobalEndCondition *	GetGlobalEndCondition( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind );	//처음 기획시 여러 조건이 들어갈 수 있다는 조건이 붙었을때 코드

	void					TryRewardAllUser();								// 모든 유저에게 보상 

	BOOL					IsBeginCondition( FLWSUser & User ) const;		// 기부 조건이 되는가?
	BOOL					IsComplete();									// 기부 완료가 되었나?

	BOOL					IsEventPeriod() const;							// 이벤트 기간인가?
	BOOL					IsExpired() const;								// 이벤트 만료 인가?

	//void					Query_TryRewardItem();							// db의 한계로 기부 완료가 되었는지 체크를 못해 완료될때마다 시도해줘야함
	//bool					IsValid_EndCond() const;

//private:
//	void					tryRewardOneUser( FLWSUser & User );

public:
	typedef					FLVector_autoDelete< IUserBeginCondition * >	UserBeginConditionVec;
	typedef					FLVector_autoDelete< IUserReward * >			UserRewardVec;
	//typedef					FLVector_autoDelete< IUserRewards * >			UserRewardsVec;
	typedef					FLVector_autoDelete< IGlobalReward * >			GlobalRewardVec;
	typedef					FLVector_autoDelete< IGlobalEndCondition * >	GlobalConditionVec;			

public:
	// id
	DWORD					m_dwID;

	DWORD					m_dwDescription_TextID;
	DWORD					m_dwEndCond_TextID;

	BOOL					m_bRewarded;

	FLTm					m_tmStartDate;
	FLTm					m_tmEndDate;

	UserBeginConditionVec	m_vecBeginCondition_User;
	UserRewardVec			m_vecReward_User;
	//UserRewardsVec			m_vecRewards_User;
	GlobalRewardVec			m_vecReward_Global;
	GlobalConditionVec		m_vecGlobalCondition;
};


} /* nsCooperativeContributions */
//-------------------------------------------------------------------------------------------------------------------//


#endif //__FLCONTRIBUTION_H__