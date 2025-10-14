
#include "StdAfx.h"

#include "WndGold.h"

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndGold::CWndGold()
:	m_pTexture( NULL )
{
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndGold::~CWndGold()
{
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndGold::OnInitialUpdate()
{
	m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, _T( "itm_GolGolSeed.dds" ) ), WNDCOLOR_DEFAULT_KEY );
}


//---------------------------------------------------------------------------------------------
// 마우스 왼쪽 버튼 눌렀을 떄
// param	: 플레그, 마우스 위치
// return	: void
//---------------------------------------------------------------------------------------------
void CWndGold::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_GlobalShortcut.Empty();
	m_GlobalShortcut.m_dwShortcut	= SHORTCUT_ITEM;
	m_GlobalShortcut.m_pFromWnd		= this;
	m_GlobalShortcut.m_pTexture		= m_pTexture; 
	m_GlobalShortcut.m_dwData		= NULL;
}

