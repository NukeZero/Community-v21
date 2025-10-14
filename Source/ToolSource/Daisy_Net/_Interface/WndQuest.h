#ifndef __WNDQUEST__H
#define __WNDQUEST__H


class CWndRemoveQuest : public CWndMessageBox
{
public:
	int m_nRemoveQuestId;
	CWndRemoveQuest() { m_nRemoveQuestId = -1; }
	virtual ~CWndRemoveQuest() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};

class CWndQuest : public CWndNeuz 
{ 
public: 
	CWndQuest(); 
	~CWndQuest();

#ifdef __V050322_CACHEQUEST
	int m_idSelQuest;
	CWordArray m_aOpenTree;
	void Update( int nNewQuestId = -1 );
#else // __V050322_CACHEQUEST
	CTexture m_texChar;
	void Update();
#endif // __V050322_CACHEQUEST
	void UpdateText();

#ifdef __V050322_CACHEQUEST
	void TreeOpen();
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	CString MakeQuestString( CString& string, BOOL bCond ); 
#endif
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
