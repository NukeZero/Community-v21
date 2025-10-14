#ifndef __FLCOOPERATIVE_CONTRIBUTIONS_H__
#define __FLCOOPERATIVE_CONTRIBUTIONS_H__

#include "../_Common/post.h"
#include "../_CommonDefine/FLTm.h"
#include "../_CommonDefine/FLSingleton.h"
#include "../_CommonDefine/FLStdContainer.h"

#include "FLQuestInterface.h"
#include "FLCC_Contribution.h"


class FLWUser;


#include <boost/noncopyable.hpp>

//------------------------------------------------- 협동 기부 객체 --------------------------------------------------//
namespace nsCooperativeContributions {

class FLCooperativeContributions : boost::noncopyable//, public FLSingleton< FLCooperativeContributions >
{
public:
	FLCooperativeContributions();
	virtual ~FLCooperativeContributions();

	static FLCooperativeContributions & GetInstance();

	void					Load( const BYTE * pBuffer, const int nBufferSize );
	FLContribution *		FindContribution( const DWORD nContributionID );	//기부에 대한 정보와 기부된 정보관련 객체

	void					TryReward_LoginUser( FLWSUser & User );				//한 유저에게 보상 : Login시 쓰임
	void					TryClean_ExpiredGlobalReward();						//보상 기간 만료 체크 및 삭제

	void					Send_ContributionInfo( FLWSUser & User );

	// Query
	BOOL					Query_GetContributedInfo( /*const FLContribution & contribution*/ );
	BOOL					Query_GetRanking( const u_long idPlayer, const FLContribution & contribution );
	BOOL					Query_TryWriteRewardInfo();

private:
	typedef					FLMap_DeleteSecond< DWORD/* ContributionID */, FLContribution * >	ContributionMap;

	ContributionMap			m_mapContribution;			// 기부 조건 & 보상 & 기부 정보 리스트
};

} //nsCooperativeContributions


inline static				nsCooperativeContributions::FLCooperativeContributions & COOPERATIVE_CONTRIBUTIONS()		
{		
	return nsCooperativeContributions::FLCooperativeContributions::GetInstance();		
}

#endif //__FLCOOPERATIVE_CONTRIBUTIONS_H__
