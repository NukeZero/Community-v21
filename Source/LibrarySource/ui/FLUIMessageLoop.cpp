
#include "FLUIMessageLoop.h"


//////////////////////////////////////////////////////////////////////////
static	bool	FLIsIdleMessage( MSG* pMsg )
{
	// These messages should NOT cause idle processing
	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
	case WM_PAINT:
	case 0x0118:	// WM_SYSTIMER (caret blink)
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool	FLUIIdleHandler::OnIdle()
{
	return DoOnIdle();
}

//////////////////////////////////////////////////////////////////////////

bool	FLUIMessageFilter::PreTranslateMessage( MSG* pMsg )
{
	return DoPreTranslateMessage( pMsg );
}
//////////////////////////////////////////////////////////////////////////


FLUIMessageLoop::FLUIMessageLoop()
{
	m_pIdleHandler = NULL;
	m_pMessageFilter = NULL;
}

FLUIMessageLoop::~FLUIMessageLoop()
{
	m_pIdleHandler = NULL;
	m_pMessageFilter = NULL;
}

void	FLUIMessageLoop::SetIdleHandler( FLUIIdleHandler* pHandler )
{
	m_pIdleHandler = pHandler;
}

void	FLUIMessageLoop::SetMessageFilter( FLUIMessageFilter* pFilter )
{
	m_pMessageFilter = pFilter;
}

int		FLUIMessageLoop::Run()
{
	bool bDoIdle = true;
	BOOL bRet = FALSE;
	MSG tMsg;

	for(;;)
	{
		while( bDoIdle && (::PeekMessage( &tMsg, NULL, 0, 0, PM_NOREMOVE ) == FALSE) )
		{
			bDoIdle = OnIdle();
		}

		bRet = ::GetMessage( &tMsg, NULL, 0, 0 );
		if( bRet == -1 )
		{
			continue;   // error, don't process
		}
		else if( bRet == FALSE )
		{
			break;   // WM_QUIT, exit message loop
		}

		if( PreTranslateMessage( &tMsg ) == false )
		{
			::TranslateMessage( &tMsg );
			::DispatchMessage( &tMsg );
		}

		if( FLIsIdleMessage( &tMsg ) == true )
		{
			bDoIdle = true;
		}
	}

	return static_cast< int >( tMsg.wParam );
}

bool	FLUIMessageLoop::PreTranslateMessage( MSG* pMsg )
{
	if( m_pMessageFilter != NULL )
	{
		return m_pMessageFilter->PreTranslateMessage( pMsg );
	}

	return false;
}

bool	FLUIMessageLoop::OnIdle()
{
	if( m_pIdleHandler != NULL )
	{
		return m_pIdleHandler->OnIdle();
	}

	return false;
}
