#ifndef __WNDCOMPANY__H
#define __WNDCOMPANY__H

typedef	struct	_COMPANYMEMBER
{
	u_long	idPlayer;
	_COMPANYMEMBER()
	{
		idPlayer	= 0;
	}
}
COMPANYMEMBER, *PCOMPANYMEMBER;

class CWndCompany : public CWndNeuz 
{
public: 
	CWndCompany(); 
	~CWndCompany(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void	UpdateData( void );
	void	AddCompanyMember( u_long idMember );
	void	RemoveCompanyMember( u_long idMember );
	void	Clear( void );
}; 
#endif
