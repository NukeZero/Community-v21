#ifndef __FLQUEST_INTERFACE_H__
#define __FLQUEST_INTERFACE_H__

#include <boost/noncopyable.hpp>

// user 시작 조건.
struct IUserBeginCondition : boost::noncopyable	{
	virtual	BOOL 		IsValid( FLWSUser & User ) = 0;
};

// Global 변수 - 완료 조건
struct IGlobalEndCondition : boost::noncopyable	{
	virtual void		SerializeOut_Info( CAr & ar ) = 0;						// 기부 목표 정보 패킷 Seralizer
	//virtual DWORD		GetTextID() const = 0;									// 기부 목표 설명 text

	virtual BOOL		Query_Contribute( const DWORD dwContributionID, const FLTm & tmStartDate, const FLTm & tmEndDate, const u_long idPlayer, const TCHAR * szName ) = 0; //기부 쿼리
	virtual BOOL		Contribute( FLWSUser & User ) = 0;						// 기부
	virtual void		SetContributed( const DWORD dwContiributedCount ) = 0;	// 기부 정보 세팅

	//virtual size_t		GetContributionUnit() const = 0;						// 한번에 기부하는 단위
	virtual BOOL		IsSameInfo( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind ) = 0;	// 같은 기부 정보인가 체크
	virtual BOOL		IsComplete( const DWORD dwContributionID ) = 0;			// 완료 되었나?
	//virtual BOOL		TrySetContributedCount( const E_CONTRIBUTION_KIND eContributionKind, const DWORD dwIndexOfKind, const DWORD dwContributedCount ) = 0;
};

// User 개개인에게 보상해줘야하는 보상
struct IUserReward : boost::noncopyable		{
	virtual void		SerializeOut_Info( CAr & ar ) = 0;						// 보상 정보 패킷 Seralizer

//	virtual DWORD		GetRewardTextID() const	= 0;							// 보상 정보 text
	virtual void		Query_TryWriteRewardInfo( const DWORD dwContributionID, const FLTm & tmStart, const FLTm & tmEnd ) = 0;		//보상 받을 아이템 정보를 DB에 쓴다.
	virtual BOOL		TryReward( const DWORD dwContributionID, FLWSUser & User ) = 0;		// 기부 시도를 한다.
};



// Global 변수로 세팅되어 값만 세팅되면 알아서 보상해줌
struct IGlobalReward : boost::noncopyable	{
	virtual void		SerializeOut_Info( CAr & ar ) = 0;						// 보상 정보 패킷 Seralizer
	//virtual DWORD		GetRewardTextID() const	= 0;							// 보상 정보 text

	virtual BOOL		TryReward() = 0;										// 보상 시도
	virtual void		Clean_ExpiredReward() = 0;									// 만료 체크 및 처리

	//virtual void		RewardNotify( FLWSUser & User )	= 0;					// 보상 패킷 통보
};





// User 개개인에게 보상해줘야하는 여러개의 보상
//struct IUserRewards	: boost::noncopyable	{
//	typedef				std::vector< DWORD >		TextIDVec;
//
//public:
//	virtual BOOL		TryReward( const DWORD dwContributionID, FLWSUser & User ) = 0;
//	const TextIDVec &	GetRewardTextIDList() const		{		return m_vecTextID;		}
//
//protected:
//	void				pushTextID( const DWORD dwTextID )		{		m_vecTextID.push_back( dwTextID );	}
//protected:
//	TextIDVec			m_vecTextID;
//};

#endif //__FLQUEST_INTERFACE_H__