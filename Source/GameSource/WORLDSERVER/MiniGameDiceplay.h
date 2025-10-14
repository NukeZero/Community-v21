#pragma once

#include <minigamebase.h>

class CMiniGameDiceplay : public CMiniGameBase
{
public:
	CMiniGameDiceplay(void);
	CMiniGameDiceplay( CMiniGameBase* pMiniGame );
	virtual ~CMiniGameDiceplay(void);

	virtual BOOL Excute( FLWSUser* pUser, __MINIGAME_PACKET* pMiniGamePacket );

private:
	int m_nTargetNum;
};

