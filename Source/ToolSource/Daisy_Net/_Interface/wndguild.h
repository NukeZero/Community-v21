#ifndef __WNDGUILD__H
#define __WNDGUILD__H

#include "WndGuildTabApp.h"
#include "WndGuildTabInfo.h"
#include "WndGuildTabMember.h"
#include "WndGuildTabWar.h"



class CWndGuild : public CWndNeuz 
{ 
public: 
	CWndGuild(); 
	~CWndGuild(); 

	CWndGuildTabApp     m_WndGuildTabApp;
	CWndGuildTabInfo    m_WndGuildTabInfo;
	CWndGuildTabMember  m_WndGuildTabMember;
	CWndGuildTabWar		m_WndGuildTabWar;

	void UpdateDataAll();

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );
}; 


class CWndGuildConfirm : public CWndNeuz 
{ 
public: 
	CWndGuildConfirm(); 
	~CWndGuildConfirm(); 

	u_long m_idMaster;
//	u_long m_idPlayer;
	
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
