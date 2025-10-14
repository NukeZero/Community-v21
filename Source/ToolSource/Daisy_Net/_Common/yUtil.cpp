#include "stdafx.h"
#include "yUtil.h"

#define MAX_ALPHA		230


CFLASH g_Flash;

CFLASH::CFLASH()  
{ 
	Init(); 
}
CFLASH::~CFLASH()
{
}
	
void CFLASH::Init()
{
	m_bAction		= FALSE;
	m_bFade			= FALSE;
	m_nAlphaCount	= 0;
	m_dwStopTime	= 0;
	m_dwTime		= 0;
	m_dwEndTime		= 0;
	m_bStopTime     = FALSE;
	m_bRunTime		= FALSE;
	m_nAlphaAcc     = 10;
	
}
void CFLASH::RenderFlash( C2DRender* p2DRender )
{
	if( m_bAction )
	{
		// 흰색으로 만든다.
		if( m_bFade == FALSE )
		{
			m_nAlphaCount+=m_nAlphaAcc;
			m_nAlphaAcc+=20;
		}
		// 페이드 아웃함( 정지시간, 지속시간 )
		else
		{
			// 처음에 한번만 정지 유지시간 입력
			if( !m_bStopTime )
			{
				m_dwStopTime = timeGetTime()+m_dwStopTime;
				m_bStopTime = TRUE;
			}

			if( timeGetTime() > m_dwStopTime )
			{
				//처음에 한번만 지속유지시간 입력
				DWORD dwSaveTime = timeGetTime();
				
				BOOL bBackup = m_bRunTime;

				if(	!m_bRunTime )
				{
					m_dwEndTime = dwSaveTime+m_dwTime;
					m_bRunTime = TRUE;
				}

				FLOAT fLerp = (FLOAT)(m_dwEndTime - dwSaveTime) / (FLOAT)m_dwTime;

				D3DXCOLOR dwColor;
				D3DXCOLOR dwStartColor = D3DCOLOR_RGBA( 255, 255, 255, 0 );
				D3DXCOLOR dwEndColor   = D3DCOLOR_RGBA( 255, 255, 255, 255  );
				
				// 지나온시간과 완료시간사이의 보간
				D3DXColorLerp( &dwColor, &dwStartColor, &dwEndColor, fLerp );
				
				m_nAlphaCount = dwColor.a*255;
				
				if( m_nAlphaCount <= 0 || m_dwEndTime < dwSaveTime )
					Init();
			}
			else
			{
			}
		}
		
		// 가장 밝은 색이 되면 페이드 아웃을 실시함
		if( m_nAlphaCount > MAX_ALPHA )
		{
			m_bFade = TRUE;
			m_nAlphaCount = MAX_ALPHA;
		}
		
		p2DRender->RenderFillRect( CRect( 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT-48 ), D3DCOLOR_ARGB( m_nAlphaCount, 255, 255, 255 ) );
	}
}

void CFLASH::SetTime( DWORD dwStopTime, DWORD dwTime, BOOL bAction )
{
	Init();
	SetAction( bAction );

	m_dwStopTime = dwStopTime;
	m_dwTime	 = dwTime;
}

void CFLASH::SetAction( BOOL bAction )
{
	m_bAction = bAction;
}

BOOL IsForceAttack()
{
	return ( GetAsyncKeyState(VK_CONTROL) & 0x8000 );
}
