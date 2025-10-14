#include "StdAfx.h"

#include "FLFSM.h"

void IFSM::PostAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 )
{
	switch( dwMessage )
	{
	case AIMSG_ARRIVAL:
		{
			AIMSG msg;
			msg.dwMessage	= dwMessage;		msg.dwParam1	= dwParam1;		msg.dwParam2	= dwParam2;
			m_MsgQ.push( msg );
			break;
		}
	default:	
		sendAIMsg( dwMessage, dwParam1, dwParam2 );	
		break;
	}
}

void IFSM::sendAIMsg( const DWORD dwMessage, const DWORD dwParam1, const DWORD dwParam2 )
{
	UpdateAI( dwMessage, dwParam1, dwParam2 );
	//AIMSG msg;
	//msg.dwMessage			= dwMessage;
	//msg.dwParam1			= dwParam1;
	//msg.dwParam2			= dwParam2;
	//UpdateAI( msg );
}

