#include "stdafx.h"
#include "WndMap.h"
#include "../Neuz/WorldMap.h"

/****************************************************
  WndId : APP_MAP - Map
****************************************************/

CWndMap::CWndMap() 
{ 
} 
CWndMap::~CWndMap() 
{ 
} 
void CWndMap::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndMap::OnInitialUpdate() 
{ 
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMap::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
//sun: 9, 스타트메뉴에 전체지도 메뉴 추가
	CWorldMap* pWorldMap = CWorldMap::GetInstance();


	// 렌더링중이면 끄고 아니면 켠다
	if(pWorldMap->IsRender())	pWorldMap->DeleteWorldMap();
	else						pWorldMap->LoadWorldMap();

	return FALSE;
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndMap::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndMap::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMap::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMap::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMap::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
	Destroy();
} 
void CWndMap::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMap::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

