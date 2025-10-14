#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

class CWndCommand;
class CWndChatFilter;
//////////////////////////////////////////////////////////////////////////////////////
// 채팅 윈도 
//
class CWndEditChat : public CWndEdit
{
public:
	CWndEditChat();
	~CWndEditChat();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
};
class CWndMacroChat : public CWndButton
{
public:
	CTexture* m_pTexMacro;
	CTexture m_texMacroChat;
	CWndMacroChat();
	~CWndMacroChat();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
};
class CWndTextChat : public CWndText
{
public:
	CWndTextChat();
	~CWndTextChat();
	virtual BOOL IsPickupSpace(CPoint point); 
};
class CWndChat : public CWndNeuz
{
	IMAGE m_wndChatEdit[3];
	//CSize m_sizeWndChatEdit[3];

	CWndMacroChat m_wndMacroChat;
	CTimer m_timerDobe;
	CTexture m_texEdit;
	CTimer m_timerInsMsg;
	int m_nInsMsgCnt;
	
	CString m_strChatBackup;
public:
#ifdef __V050322_CHATLOCK
	BOOL m_bChatLock;
#endif
#ifdef __X0804
	BOOL m_bMoveLock;
#endif
#ifdef __VBUGFIX_20050201
	static int m_nChatChannel;
#else
	int m_nChatChannel;
#endif
	CWndChatFilter* m_pWndChatFilter;
	CString m_strCharName;
	CWndTextChat m_wndText;
	CStringArray m_strArray;
	CWndCommand* m_pWndCommand;
//	CWndButton m_wndMenu;
	//CWndText m_wndText;
	//CWndEdit m_wndBeginning;
	CWndEditChat m_wndEdit;
	//CWndText m_wndReceiver;

//	CWndButton m_wndShout;
	//CWndButton m_wndParty;

	CWndChat();   
	virtual ~CWndChat(); 
#ifdef __V050322_CHATLOCK
	void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
#endif
	void  Parsing( CString string );
	void  PutString( LPCTSTR lpszString, DWORD dwColor = 0xffffffff ); //CObj* pObj );
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	void SetChannel();
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void OnDestroy();
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
#ifdef __V050322_CACHEQUEST
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat );
#else
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size );
#endif
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
		
};

#endif
