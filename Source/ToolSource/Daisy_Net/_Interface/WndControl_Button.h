
#if _MSC_VER > 1000
#pragma once
#endif 

#include "WndBase.h"
#include "WndControl_Menu.h"

class C2DRender;

class CWndButton : public CWndBase
{


public:


						CWndButton();
						~CWndButton();

	BOOL				Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR = 0, int nFontColorG = 0, int nFontColorB = 0 );
	BOOL				Create( CWndBase* pParentWnd, LPWNDCTRL lpWndCtrl );
	virtual void		PaintFrame( C2DRender* p2DRender );
	virtual	BOOL		Process( void );

	void				FitTextureSize( void );
	BOOL 				IsHighLight( void )					{	return m_bHighLight;					}

	int  				GetCheck( void ) const; 
	CWndBase*			GetWndExecute( void )				{	return m_pWndExecute;					}
	DWORD 				GetPushColor( void )				{	return m_nPushColor;					}
	DWORD 				GetDisableColor( void )				{	return m_nDisableColor;					}
	DWORD 				GetHighLightColor( void )			{	return m_nHighlightColor;				}


	void 				SetCheck( int nCheck ); 
	void 				SetPushTime(int nTime);				
	void				SetMenu( CWndMenu* pWndMenu );
	void 				SetString( CString strSndEffect ); 
	void				SetWndExecute( CWndBase* pWndBase );
	void 				SetPushColor(DWORD dwColor)			{	m_nPushColor		= (DWORD)dwColor;	}
	void 				SetDisableColor(DWORD dwColor)		{	m_nDisableColor		= (DWORD)dwColor;	}
	void 				SetHighLightColor(DWORD dwColor)	{	m_nHighlightColor	= (DWORD)dwColor;	}
	void				SetPushPoint(int x,int y)			{	m_ptPush = CPoint(x,y);					}


	virtual void 		OnMouseMove(UINT nFlags, CPoint point);
	virtual void 		OnLButtonUp(UINT nFlags, CPoint point);
	virtual void 		OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL 		OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

	virtual LRESULT 	DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);


protected:

	CWndBase* 			m_pWndExecute;  
	BOOL      			m_bCheck;
	int       			m_nTimePush;
	BOOL      			m_bHighLight;
	CPoint    			m_ptPush;
	DWORD     			m_nFontColor;
	DWORD     			m_nPushColor;
	DWORD     			m_nHighlightColor;
	DWORD     			m_nDisableColor;


public:


	CWndMenu*  			m_pWndMenu;
	BOOL       			m_bTopDown;
	CHAR				m_cHotkey;
	SHORTCUT   			m_shortcut;

};
