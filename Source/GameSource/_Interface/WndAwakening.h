#ifndef __WND__H
#define __WND__H

//sun: 11, 각성, 축복
class CWndAwakening : public CWndNeuz 
{ 
public: 

	CWndText*  m_pText;
	FLItemElem* m_pItemElem;
	PT_ITEM_SPEC  m_pEItemProp;
	CTexture*  m_pTexture;

	CWndAwakening(); 
	~CWndAwakening(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDestroy();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL process();

	void SetDescription();
}; 

class CWndSelectAwakeCase : public CWndNeuz
{
	// 각성 보호 선택창 ( 두가지중에 하나 고름 < 각성전, 각성후 > 0
public:

	CWndSelectAwakeCase( );
	virtual ~CWndSelectAwakeCase( );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
//	virtual void OnDestroy();
//	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
//	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
//	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL process();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetItemIndex( const DWORD index )			{ m_dwItemIndex = index; }
	void SetData( DWORD dwObjId, SERIALNUMBER iSerialNum );
	void OutputOptionString( C2DRender* p2DRender, FLItemElem* pItemElem, BOOL bNew = FALSE );

protected:
	DWORD m_dwDeltaTime;

	DWORD m_dwItemIndex;
	CTexture*  m_pTexture;

	DWORD  m_dwTargetObjId;;
	SERIALNUMBER m_iSerialNum;

	CTexture* m_pTexGuage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
};


#endif
