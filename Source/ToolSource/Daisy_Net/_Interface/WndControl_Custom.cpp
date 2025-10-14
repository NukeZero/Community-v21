

#include "StdAfx.h"
#include "WndControl_Custom.h"

#define CUSTOM_CONTROL_FILL_COLOR	0xFF808080

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndCustom::CWndCustom()
{
	m_byWndType = WTYPE_CUSTOM;
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndCustom::~CWndCustom()
{
}


//---------------------------------------------------------------------------------------------
// Create ( 생성하기 )
// param	: ...
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndCustom::Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR, int nFontColorG, int nFontColorB )
{
	FLERROR_ASSERT_LOG_RETURN( pParentWnd,	NULL, _T("[윈도우 커스텀] 부모 윈도우 포인터 유효하지 않음.") );	

	if( NULL == lpszCaption )
	{
		FLERROR_LOG( PROGRAM_NAME, _T("[윈도우 커스텀] Caption 문자열 없음") );
		FLASSERT( lpszCaption );
	}	
	
	if( _tcslen(lpszCaption) )
		m_strTitle = lpszCaption;
	
	SetFontColor( nFontColorR, nFontColorG, nFontColorB );
	return CWndBase::Create( dwStyle | WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect,  pParentWnd, nID );
}


//---------------------------------------------------------------------------------------------
// On Draw ( 그리기 )
// param	: p2DRender
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndCustom::OnDraw( C2DRender* p2DRender )
{
	p2DRender->RenderFillRect( GetWindowRect(), CUSTOM_CONTROL_FILL_COLOR );
}
