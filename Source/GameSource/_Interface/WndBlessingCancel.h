#ifndef __WNDBLESSINGCANCEL__H
#define __WNDBLESSINGCANCEL__H
//sun: 11, 각성, 축복
class CWndBlessingCancel : public CWndNeuz 
{ 
public: 

	CWndText*  m_pText;
	FLItemElem* m_pItemElem;
	PT_ITEM_SPEC  m_pEItemProp;
	CTexture*  m_pTexture;

	CWndBlessingCancel(); 
	~CWndBlessingCancel(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnDestroy();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	void SetDescription();
}; 

#endif