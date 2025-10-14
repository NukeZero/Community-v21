#pragma once

#include <minigamebase.h>

class CMiniGameLadder :	public CMiniGameBase
{
public:
	CMiniGameLadder(void);
	CMiniGameLadder( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameLadder(void);

	virtual BOOL Excute( FLWSUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );
};

