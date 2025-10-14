
#if _MSC_VER > 1000
#pragma once
#endif 

#include "WndBase.h"

class C2DRender;

class CWndCustom : public CWndBase
{


public:


			CWndCustom(); 
			~CWndCustom();

	void	OnDraw( C2DRender* p2DRender );
	BOOL	Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR, int nFontColorG, int nFontColorB );
};
