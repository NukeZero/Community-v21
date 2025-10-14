//
// FILE: CMclAutoLock.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __FLAUTOLOCK_H__
#define __FLAUTOLOCK_H__


#include "FLThreadCommon.h"
#include "FLCriticalSection.h"


class	CMclAutoLock
{
private:

    CMclCritSec*	m_pcCritSec;

public:

    CMclAutoLock( CMclCritSec& rCMclCritSec )
	{
		m_pcCritSec = &rCMclCritSec;
		m_pcCritSec->Enter();
	}

    ~CMclAutoLock()
	{
		m_pcCritSec->Leave();
	}
};

#endif

