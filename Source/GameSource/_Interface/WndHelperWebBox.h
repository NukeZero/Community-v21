#ifndef __WND_HELPER_WEB_BOX_H__
#define __WND_HELPER_WEB_BOX_H__


class CWndHelperWebBox : public CWndNeuz
{
public:
	enum 
	{ 
		TASKBAR_HEIGHT = 48,
	};

public:
	CWndHelperWebBox( void );
	~CWndHelperWebBox( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL Process( void );
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual HRESULT RestoreDeviceObjects( void );

	//added gmpbigsun
	virtual void OnSize( UINT nType, int cx, int cy );
	enum WEB_MODE
	{
		WMODE_HELPER,
		WMODE_CERTIFICATION,
		WMODE_FACEBOOK,
		WMODE_GOOGLE
	};
	void SetMode( WEB_MODE mode );

public:
	void Release( void );
};


#endif // __WND_HELPER_WEB_BOX_H__