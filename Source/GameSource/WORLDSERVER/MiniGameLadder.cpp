#include "stdafx.h"

#include ".\minigameladder.h"

CMiniGameLadder::CMiniGameLadder(void)
{
}

CMiniGameLadder::CMiniGameLadder( CMiniGameBase* /*pMiniGame*/ )
{
	//CMiniGameLadder* pMiniGameLadder = static_cast<CMiniGameLadder*>( pMiniGame );
}

CMiniGameLadder::~CMiniGameLadder(void)
{
}

BOOL CMiniGameLadder::Excute( FLWSUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_LORD_RAINBOW_LADDER, pUser->GetWorld(), pUser->GetPos() ) == FALSE ) {
		return FALSE;
	}

	BOOL bReturn = FALSE;
	
	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		SendPacket( pUser, MP );
		return bReturn;
	}	

	
	if( xRandom( 10 ) == 0 ) // 1/10 확률로 성공한다.
	{
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}
	else
		MP.nState = MP_FAIL;

	SendPacket( pUser, MP );
	return bReturn;
}

