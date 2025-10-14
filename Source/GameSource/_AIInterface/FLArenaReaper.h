#pragma once

#include "FLStalkerFSM.h"

class CMover;

class FLArenaReaper : public FLStalkerFSM
{
public:
	FLArenaReaper( CMover & Owner );
	virtual ~FLArenaReaper();
};