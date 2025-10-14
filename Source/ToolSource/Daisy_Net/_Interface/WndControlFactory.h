
#if _MSC_VER > 1000
	#pragma once
#endif 

//---------------------------------------------------------------------------------------------
// 윈도우 컨트롤 생성 클레스 
// Author	: lenahyang
// Date		: 2011.11.21
//---------------------------------------------------------------------------------------------

class CWndControlFactory
{


public:


				CWndControlFactory();
				~CWndControlFactory();

	WNDCTRL		CreateControl( DWORD dwControlType, CWndDialog* pWndDialog );	
	bool		RenderPreviewControl( DWORD dwControlWndType, C2DRender* p2DRender, CPoint* pMousePoint );


	CString		GetDescription( DWORD dwWndType, DWORD dwWndStyle );


private:


	LPWNDCTRL	GetControlByWndType( DWORD dwControlWndType );


};