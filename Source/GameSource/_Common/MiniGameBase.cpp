#include "stdafx.h"

//sun: 13, 레인보우 레이스

#ifdef __WORLDSERVER
#include ".\minigamebase.h"
#include "../WorldServer/User.h"

CMiniGameBase::CMiniGameBase(void)
{
}

CMiniGameBase::~CMiniGameBase(void)
{
}

void CMiniGameBase::SendPacket( FLWSUser* pUser, __MINIGAME_PACKET MiniGamePacket )
{
	pUser->AddMiniGameState( MiniGamePacket );
}

void CMiniGameBase::SendExtPacket( FLWSUser* pUser, __MINIGAME_EXT_PACKET MiniGameExtPacket )
{
	pUser->AddMiniGameExtState( MiniGameExtPacket );
}
#endif // __WORLDSERVER

