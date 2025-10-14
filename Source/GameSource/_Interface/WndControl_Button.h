
#pragma once

#include "WndBase.h"
#include "WndControl_Menu.h"

namespace WndButton
{
	static const WORD	ICONIMAGE_SIZE( 22 );
	static const WORD	HOTKEY_OFFSET( 32 );
	static const WORD	STRING_OFFSET( 4 );
}

//---------------------------------------------------------------------------------------------
// 윈도우 버튼 클레스
//---------------------------------------------------------------------------------------------

class CWndButton : public CWndBase
{


public:


						CWndButton();
						~CWndButton();

	BOOL				Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID, int nFontColorR = 0, int nFontColorG = 0, int nFontColorB = 0 );
	virtual void		PaintFrame( C2DRender* p2DRender );
	virtual	BOOL		Process( void );

	void				FitTextureSize( void );
	BOOL 				IsHighLight( void )						{	return m_bHighLight;					}

	const CHAR			GetHotKeyChar( void	)					{	return m_cHotkey;						}				

	int  				GetCheck( void ) const; 
	CWndBase*			GetWndExecute( void )					{	return m_pWndExecute;					}
	const DWORD 		GetPushFontColor( void )				{	return m_nPushFontColor;				}
	const DWORD			GetShadowFontColor( void )				{	return m_nShadowFontColor;				}
	const DWORD 		GetDisableFontColor( void )				{	return m_nDisableFontColor;				}
	const DWORD			GetHighLightFontColor( void )			{	return m_nHighlightFontColor;			}

	void 				SetCheck( int nCheck ); 
	void 				SetPushTime( int nTime );				
	void				SetMenu( CWndMenu* pWndMenu );
	void 				SetString( CString strSndEffect ); 
	void				SetWndExecute( CWndBase* pWndBase );
	void 				SetPushColor( DWORD dwColor )			{	m_nPushFontColor = (DWORD)dwColor;		}
	void				SetPushPoint(int x,int y)				{	m_ptPush = CPoint(x,y);					}

	virtual void 		OnMouseMove( UINT nFlags, CPoint point );
	virtual void 		OnLButtonUp( UINT nFlags, CPoint point );
	virtual void 		OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL 		OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );

	virtual LRESULT 	DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);


protected:


	void				DrawIcon( C2DRender* p2DRender, CPoint ptStart );
	void				DrawText( C2DRender* p2DRender, CPoint ptStart );
	void				DrawMenuText( C2DRender* p2DRender, CPoint ptStart );
	void				DrawCheckIcon( C2DRender* p2DRender );
	void				DrawHighLight( C2DRender* p2DRender, CRect rtRect );


protected:


	CWndBase* 			m_pWndExecute;  
	BOOL      			m_bCheck;
	int       			m_nTimePush;
	BOOL      			m_bHighLight;
	CPoint    			m_ptPush;

	DWORD     			m_nPushFontColor;
	DWORD				m_nShadowFontColor;
	DWORD				m_nDisableFontColor;
	DWORD				m_nHighlightFontColor;


public:


	CWndMenu*  			m_pWndMenu;
	BOOL       			m_bTopDown;
	CHAR				m_cHotkey;
	SHORTCUT   			m_shortcut;
	BOOL				m_bForcedDrawTitle;


};
