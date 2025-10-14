//
// FILE: CMclCritSec.h
//
// Copyright (c) 1997 by Aaron Michael Cohen and Mike Woodring
//
/////////////////////////////////////////////////////////////////////////
#ifndef __FLCRITICALSECTION_H__
#define __FLCRITICALSECTION_H__


#include "FLThreadCommon.h"


class	CMclCritSec
{
private:

    CRITICAL_SECTION	m_CritSec;

public:

	CMclCritSec()
    {
		try
		{
			::InitializeCriticalSection( &m_CritSec );
		}
		catch (std::exception& e)
		{
			OutputDebugString(e.what());
			int* ptr = nullptr;
			*ptr = 0;
		}
	}
    ~CMclCritSec()
	{
		::DeleteCriticalSection( &m_CritSec );
	}

    // enter the critical section...
    void	Enter()
	{
		::EnterCriticalSection( &m_CritSec );
	}

    // leave the critical section...
    void	Leave()
	{
		::LeaveCriticalSection( &m_CritSec );
	}

};

#endif
