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

//sun: 시스템 메세지 윈도우화... Neuz
class CWndChatLog : public CWndNeuz
{
	CWndMenu	 m_wndMenuPlace;
	CWndTextChat m_wndText;

public:
	CWndChatLog();   
	virtual ~CWndChatLog(); 
	void  PutString( LPCTSTR lpszString ); 
	void  PutString( LPCTSTR lpszString, DWORD dwColor ); 
	void  PutString( LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle ); 
//	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
//	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void OnDestroy();
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnSetFocus(CWndBase* pOldWnd);
	virtual void OnKillFocus(CWndBase* pNewWnd);
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat );
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual BOOL Process ();	
};

#ifdef __HYPERLINK_ITEM16

struct HLITEM_DATA
{
	HLITEM_DATA() { reset(); }
	~HLITEM_DATA( ) { SAFE_DELETE( pItem ); }
	
	void reset() { _masterID = _itemID = _index = _level = 0, _wLine = _wSeq = 0, pItem = NULL; }

//	char _szOrg[ 64 ];
	CString _szOrg;		//original string ( $i........$ni )
	DWORD _masterID;	//master mover ID
	DWORD _itemID;		//item oid
	int _index;			//item index
	int _level;			//item level

	WORD _wLine;		//line
	WORD _wSeq;			//sequence of line 
    
	// 서버로부터 아이템정보 요청후 생성할 tool-tip용 아이템, 이 포인터가 NULL일 경우만 요청한다.
	FLItemElem* pItem;	
};

typedef std::map< DWORD, HLITEM_DATA >			HyperLinkItemContainer;		//key = _wLine | _wSeq
typedef HyperLinkItemContainer::iterator	HyperLinkItemIter;

// 링크아이템 순서도
// 1. 쉬프트 + 클릭으로 아이템을 선택한다
// 2. 임시 HLITEM_DATA 에 Data를 채운다.
// 3. 엔터를 눌렀다면 해당 문자열의 아이템링크부분을 원본으로 교체해서 전송한다.

// 4. 전송받은 채팅 문자열에서 아이템링크가 있는지 체크하고 있다면 적절한 문자열로 교체한후 HLITEM_DATA를 만든다
// 5. 이때 Key는 해당 문자열의 라인번호 | 순서 로 설정한다.
// 6. 유저가 해당 아이템을 클릭할경우 HLITEM_DATA container에서 찾은후 서버로 아이템데이터를 요청한다.
// 7. 받은 아이템 데이터 tooltip을 만들어 뿌려준다.

#endif //__HYPERLINK_ITEM16


class CWndChat : public CWndNeuz
{
	IMAGE m_wndChatEdit[3];
	//CSize m_sizeWndChatEdit[3];

	CWndMacroChat m_wndMacroChat;
	CTimer m_timerDobe;
	CTexture m_texEdit;
	CTimer m_timerInsMsg;
	CTimer m_timerAlertGGrade;	//sun: 10, __CSC_GAME_GRADE

	int m_nInsMsgCnt;
	int				m_nHistoryIndex;
	std::vector<CString> m_strHistory;
	CTimer m_timerInputTimeOut;

public:
#ifdef __HYPERLINK_ITEM16
	HLITEM_DATA				m_kTempHLData_Send;		//보내기위한 임시 저장소
	HLITEM_DATA				m_kTempHLData_Receive;	//컨테이너에 넣기위해 보관하는 임시 저장소
	HyperLinkItemContainer	m_cHyperLinkItems;		//받은 아이템에 대해서... 

	int GetLineCount( );
	BOOL UpdateLineForHLItems( const int nBegin, const int nCount );	// 라인이 어떠한 이유로 삭제되면 데이터를 재구축한다.
#endif //__HYPERLINK_ITEM16

	BOOL m_bChatLock;

	BOOL m_bChatLog;	//sun: 8, //__CSC_VER8_1

	BOOL m_bMoveLock;
	static int m_nChatChannel;
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
#ifdef __IMPROVE_MAP_SYSTEM
	enum EHyperlinkMode
	{
		HL_None, HL_Item, HL_UserMark
	};

	BOOL m_bHyperlinked;
	EHyperlinkMode m_eHyperlinkMode;
	DWORD m_dwHyperlinkTextStyle;
	int m_nHyperlinkTextStartPosition;
	int m_nHyperlinkTextLeftPosition;
	int m_nHyperlinkTextRightPosition;
	CString m_strHyperlinkMarkTitle;
	FLOAT m_fHyperlinkMarkPositionX;
	FLOAT m_fHyperlinkMarkPositionY;

	//마우스 포인트가 링크아이템문자 위라면 해당 라인을 저장한다. 왼쪽버튼을 눌렀을시 이변수를 추가검시 없이 바로 이용
	int m_nSelectedLineForHLITEM;		
#endif // __IMPROVE_MAP_SYSTEM

	CWndChat();   
	virtual ~CWndChat(); 
	void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	void  Parsing( CString string );

	void  PutString( LPCTSTR lpszString );
	void  PutString( LPCTSTR lpszString, DWORD dwColor ); 
	void  PutString( LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle ); 

#ifdef __IMPROVE_MAP_SYSTEM
private:
	BOOL CalculateItemHyperlink( int nValidTextLine, const CPoint& pointMouse );
	BOOL CalculateUserMarkHyperlink( int nValidTextLine, const CPoint& pointMouse );
	BOOL IsUserMarkHyperlinkParsingValid( 
		const CString& strParsing, 
		int nParsing1stCharacterCounter, 
		int nParsing2ndCharacterCounter, 
		int nParsing3rdCharacterCounter, 
		CString& strParsingTitle, 
		FLOAT& fParsingPositionX, 
		FLOAT& fParsingPositionY );
public:
#endif // __IMPROVE_MAP_SYSTEM
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	void SetChannel( BOOL bForced = FALSE );
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
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat );
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Process ();	
#ifdef __IMPROVE_MAP_SYSTEM
	virtual void OnMouseWndSurface( CPoint point );
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
#endif // __IMPROVE_MAP_SYSTEM

	enum
	{
		MODE_NORMAL,
		MODE_TARNSPARENT,
	};
	void SwitchMode( int nMode );
	int m_nMode;
};



class CFLWndChatNotice : public CWndNeuz
{
public:
	CFLWndChatNotice();
	~CFLWndChatNotice();
	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual	void OnInitialUpdate( );
	virtual	BOOL Process( );
	virtual BOOL IsPickupSpace(CPoint point); 
	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	void SetNoticeText(CString strNotice);

private:
	std::vector<CString>::iterator	m_itrNotice;
	std::vector<CString>	m_vecNotice;
	std::vector<CString>	m_vecTempNotice;

	CString m_szNotice;
	CWndStatic* m_pNotice;
	int m_nIndex;
};

#endif
