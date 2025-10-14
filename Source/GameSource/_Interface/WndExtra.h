#ifndef __WNDEXTRACAPTURE__H
#define __WNDEXTRACAPTURE__H

class CWndExtraCapture : public CWndNeuz 
{ 
public: 
	CWndExtraCapture(); 
	~CWndExtraCapture(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


//gmpbigsun: 앞으로 이벤트등의 본 컨텐츠이외의 윈도는 여기다 만들자

class CWndCoupon100722 : public CWndNeuz
{
public:
	CWndCoupon100722( );
	virtual ~CWndCoupon100722( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 

	virtual BOOL Process ();

protected:
	void AutoFocus( );
	BOOL SendPacket( );
	int IsTextSize();
protected:
	BOOL m_b3LenTo4Len;
	CD3DFont* m_pFont;
};


#endif
