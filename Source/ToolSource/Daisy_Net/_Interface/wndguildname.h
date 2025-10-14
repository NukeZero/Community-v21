#ifndef __WNDGUILDNAME__H
#define __WNDGUILDNAME__H

class CWndGuildName : public CWndNeuz 
{ 
public: 
	CWndGuildName(); 
	~CWndGuildName(); 

	set<string>		m_sInvalidNames;
	BOOL	LoadInvalidName( void );
	BOOL	IsInvalidName( LPCSTR sName );
#ifdef __J0524
	BYTE	m_nId;
	void	SetData( BYTE nId = 0xff )	{	m_nId	= nId;	}
#endif	// __J0524

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


class CWndGuildNickName : public CWndNeuz 
{ 
public: 
	u_long m_idPlayer;
	set<string>		m_sInvalidNames;
	BOOL	LoadInvalidName( void );
	BOOL	IsInvalidName( LPCSTR sName );

public:
	CWndGuildNickName(); 
	~CWndGuildNickName(); 
	
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
