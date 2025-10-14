#ifndef __WNDCOMPANYINVITE__H
#define __WNDCOMPANYINVITE__H

#include "company.h"

class CWndCompanyInvite : public CWndNeuz 
{ 
public:
	u_long m_idCompany;
	char m_szCompany[MAX_COMPANY_NAME];
	u_long m_idCmdr;
	char m_szCmdr[MAX_PLAYER];
public: 
//	Constructions
	CWndCompanyInvite( CCompany* pCompany ); 
	~CWndCompanyInvite();
//	Overrides
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
//	Operations
};

#endif	// __WNDCOMPANYINVITE__H