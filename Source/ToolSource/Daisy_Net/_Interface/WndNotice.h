#ifndef __WNDNOTICE__H
#define __WNDNOTICE__H

class CWndInfoNotice : public CWndNeuz 
{ 
public: 
#ifdef __YBANNER
	vector<CString> m_vecStrBanner;	
	DWORD		 m_dwChangeBannerTime;
	LPIMAGE m_atexPannel;
#endif //__YBANNER

	CWndInfoNotice(); 
	~CWndInfoNotice(); 

#ifdef __V050322_CACHEQUEST
	void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
#else
	void AdditionalSkinTexture( LPWORD pDest, CSize size );
#endif	
	virtual BOOL Process ();
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
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
