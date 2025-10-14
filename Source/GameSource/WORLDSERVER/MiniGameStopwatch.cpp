#include "stdafx.h"

#include ".\minigamestopwatch.h"

CMiniGameStopwatch::CMiniGameStopwatch(void)
: m_nTargetTime( 0 )
{
}

CMiniGameStopwatch::CMiniGameStopwatch( CMiniGameBase* /*pMiniGame*/ )
: m_nTargetTime( 0 )
{
	//CMiniGameStopwatch* pMiniGameStopwatch = static_cast<CMiniGameStopwatch*>( pMiniGame );
}

CMiniGameStopwatch::~CMiniGameStopwatch(void)
{
}

int	CMiniGameStopwatch::SetTargetTime()
{
	m_nTargetTime = xRandom( 300, 1000 ) * 10;
	return m_nTargetTime;
}

BOOL CMiniGameStopwatch::Excute( FLWSUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	if( CNpcChecker::GetInstance()->IsCloseNpc( MMI_LORD_RAINBOW_STOPWATCH, pUser->GetWorld(), pUser->GetPos() ) == FALSE ) {
		return FALSE;
	}

	BOOL bReturn = FALSE;

	__MINIGAME_PACKET MP( pMiniGamePacket->wNowGame );	
	if( pMiniGamePacket->nState == MP_OPENWND )
	{
		MP.nState = MP_OPENWND;
		MP.nParam1 = SetTargetTime();
		SendPacket( pUser, MP );
		return bReturn;
	}

	// 0.05초 오차범위 인정..
	if( GetTargetTime() <= (pMiniGamePacket->nParam1 + 30)
		&& GetTargetTime() >= (pMiniGamePacket->nParam1 - 30) )
	{
		MP.nState = MP_FINISH;
		bReturn = TRUE;
	}
	else
	{
		MP.nState = MP_FAIL;
		MP.nParam1 = SetTargetTime();
	}

	SendPacket( pUser, MP );
	return bReturn;
}
