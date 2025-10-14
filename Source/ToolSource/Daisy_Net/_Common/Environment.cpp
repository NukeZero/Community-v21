#include "stdafx.h"
#include "Environment.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#endif // __CORESERVER

/*--------------------------------------------------------------------------------*/

CEnvironment::CEnvironment()
{
	m_bRain = m_bSnow = FALSE;
	m_tRainTime.Set( MIN( 10 ) );
	m_tSnowTime.Set( MIN( 10 ) );
#ifdef __CORESERVER
	m_Authority = FALSE;
	m_hWorker	= m_hCloseWorker	= NULL;
#endif // __CORESERVER
}

CEnvironment::~CEnvironment()
{
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER
}

void CEnvironment::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_bRain;
		ar << m_bSnow;
	}
	else
	{
		ar >> m_bRain;
		ar >> m_bSnow;
	}
}

#ifdef __CORESERVER
BOOL CEnvironment::CreateWorkers( void )
{
	DWORD dwThreadId;
	m_hCloseWorker	= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hWorker	= chBEGINTHREADEX( NULL, 0, _Worker, this, 0, &dwThreadId );
	if( m_hWorker == NULL )
		return FALSE;
	return TRUE;
}

void CEnvironment::CloseWorkers( void )
{
	CLOSE_THREAD( m_hWorker, m_hCloseWorker );
}

UINT CEnvironment::_Worker( LPVOID pParam )
{
	CEnvironment* pEnvironment	= (CEnvironment*)pParam;
	pEnvironment->Worker();
	return 0;
}

void CEnvironment::Worker( void )
{
	HANDLE hHandle	= m_hCloseWorker;
	while( WaitForSingleObject( hHandle, 60000 ) == WAIT_TIMEOUT )	// 10초	= 1000 * 10 //1000 * 60 * 5 = 60000
	{
		if( m_Authority )
		{
			if( m_bRain )
			{
				g_dpCoreSrvr.SendEnvironmentRain( m_bRain );
			}
			if( m_bSnow )
			{
				g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
			}
		}
		else
		{
			CTime timeCurr	= CTime::GetCurrentTime();
//			CString strMessage;
			
			int nMonth = timeCurr.GetMonth();
			if( 1 == nMonth || 2 == nMonth || 12 == nMonth ) // 눈내리는 달 12월 1월 2월  펄펄~~ 눈이옵니다. 하늘에서 눈이옵니다.
			{
				if( m_tSnowTime.IsTimeOut() )
				{
					BOOL bSnowBuf = m_bSnow;
					if( random(1440) < 5 ) // 1분에 한번씩 검사 1440 = 60분 * 24시간
					{
						m_tSnowTime.Reset();
						m_bSnow = TRUE;
//						strMessage.Format( "%02d월%02d일 %02d:%02d:%02d\t비가 주르륵 주르륵~~", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					else
					{
						m_bSnow = FALSE;
//						strMessage.Format( "%02d월%02d일 %02d:%02d:%02d\t비가 멈춤상태", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					
					if( bSnowBuf != m_bSnow )
					{
						g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
					}

/*					m_tSnowTime.Reset();
					m_bSnow = !m_bSnow;
					g_dpCoreSrvr.SendEnvironmentSnow( m_bSnow );
*/				}
			}
			else	// 비가 내리네~~ 비두내리는데 막걸리 한잔~~ ^^;
			{
				if( m_tRainTime.IsTimeOut() )
				{
					BOOL bRainBuf = m_bRain;

					if( random(1440) < 5 ) // 1분에 한번씩 검사 1440 = 60분 * 24시간
					{
						m_tRainTime.Reset();
						m_bRain = TRUE;
//						strMessage.Format( "%02d월%02d일 %02d:%02d:%02d\t눈이 펄펄~~", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					else
					{
						m_bRain = FALSE;
//						strMessage.Format( "%02d월%02d일 %02d:%02d:%02d\t눈이 멈춤상태", 
//							timeCurr.GetMonth(), timeCurr.GetDay(), timeCurr.GetHour(), timeCurr.GetMinute(), timeCurr.GetSecond() );
					}
					
					if( bRainBuf != m_bRain )
					{
						g_dpCoreSrvr.SendEnvironmentRain( m_bRain );
					}
				}
			}
//			if( strMessage.GetLength() > 10 )
//				WriteLog( strMessage );
		}

		TRACE( "CEnvironment Worker\n" );
	}
}
#endif // __CORESERVER

CEnvironment	g_Environment;

