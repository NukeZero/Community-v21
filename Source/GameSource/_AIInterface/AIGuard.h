#ifndef __AI_GUARD_H
#define __AI_GUARD_H

#include <memory/FLMemPooler.h>
//#include "Mover.h"

class CObj;

class CAIGuard : public CAIMonster
{
	BOOL	MoveProcessIdle( const AIMSG & msg );
	BOOL	MoveProcessRage( const AIMSG & msg );
	BOOL	MoveProcessRunaway();

	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateWander( const AIMSG & msg );
	BOOL	StateRunaway( const AIMSG & msg );
	BOOL	StateEvade( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

public:

	CAIGuard();
	CAIGuard( CObj* pObj );
	virtual ~CAIGuard();

	virtual void			InitAI();
	virtual BOOL			IsIdleMode();

public:
	MEMPOOLER_DECLARE( CAIGuard );
	
	DECLARE_AISTATE( CAIGuard )
};

#endif

