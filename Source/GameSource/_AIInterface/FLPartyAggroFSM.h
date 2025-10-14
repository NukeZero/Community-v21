#pragma once

#ifdef __WORLDSERVER

#include "FLAggroNormalFSM.h"
#include "FLBehaviorFunction.h"


// 이동만 새로 작성.
class FLPartyAggroSubMonsterFSM : public FLAggroNormalFSM
{
public:
	FLPartyAggroSubMonsterFSM( CMover & Owner, const DWORD dwAggroID );
	virtual ~FLPartyAggroSubMonsterFSM();

	virtual FLAggro *		GetAggroMng();

private:
	virtual void			sendAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 );
};



#endif //__WORLDSERVER