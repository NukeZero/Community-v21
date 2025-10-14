#ifndef __WNDGUILDBANK__H
#define __WNDGUILDBANK__H

#include "WndGuildMerit.h"

class CWndGuildBank : public CWndNeuz 
{ 
	CWndGuildMerit* m_pwndGuildMerit;		// ±æµå°øÇå.
public: 
	CWndItemCtrl m_wndItemCtrl;
	CWndGold     m_wndGold;

public:
	CWndGuildBank(); 
	~CWndGuildBank(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif
