#ifndef __WNDCHANGENAME__H
#define __WNDCHANGENAME__H

#ifdef __J0519

class CWndChangeName : public CWndNeuz 
{ 
private:
	DWORD	m_dwData;
	set<string>		m_sInvalidNames;
public: 
	CWndChangeName(); 
	~CWndChangeName(); 

	void	SetData( WORD wId, WORD wReset );
	BOOL	LoadInvalidName( void );
	BOOL	IsInvalidName( LPCSTR lpszPlayer);

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif	// __J05197

#endif	// __WNDCHANGENAME__H

