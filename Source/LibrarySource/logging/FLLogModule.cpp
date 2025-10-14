
#include "FLLogModule.h"
#include "../common/FLStringFunction.h"
#include "../thread/FLAutoLock.h"



FLLogModule::FLLogModule()
{
	m_pHead = NULL;
	m_pTail = NULL;

	m_pFilter = NULL;
	m_pPublisher = NULL;
}

FLLogModule::~FLLogModule()
{
	Destroy();
}

bool	FLLogModule::Create( FLLogPublisher* pPublisher, FLLogFilter* pFilter )
{
	m_pFilter = pFilter;
	m_pPublisher = pPublisher;
	return true;
}

void	FLLogModule::Destroy()
{
	Flush();

	m_pFilter = NULL;
	m_pPublisher = NULL;
}

void	FLLogModule::Push( FLLogEvent* pEvent )
{
	if( pEvent != NULL )
	{
		pEvent->pNext = NULL;

		CMclAutoLock Lock( m_csEvent );
		if( m_pHead == NULL )
		{
			m_pHead = pEvent;
		}
		else
		{
			m_pTail->pNext = pEvent;
		}

		m_pTail = pEvent;
	}
}

void	FLLogModule::DoFlush()
{
	FLLogPublisher* pPublisher = m_pPublisher;
	FLLogFilter* pFilter = m_pFilter;
	if( pPublisher != NULL )
	{
		FLLogEvent* pEvent = Fetch();
		while( pEvent != NULL )
		{
			if( pFilter != NULL )
			{
				if( pFilter->IsPublish( pEvent ) == true )
				{
					pPublisher->Publish( pEvent );
				}
			}

			FLLogEvent* pDelete = pEvent;
			pEvent = pEvent->pNext;

			FLLogEvent::Free( pDelete );
			pDelete = NULL;
		}

		pPublisher->DoneTick();
	}
}

FLLogEvent*	FLLogModule::Fetch()
{
	CMclAutoLock Lock( m_csEvent );

	FLLogEvent* pEvent = m_pHead;
	m_pHead = NULL;
	m_pTail = NULL;

	return pEvent;
}
