#pragma  once 

#define N_MAX 4


class CFLWndCostume_Mix: public CWndNeuz
{
public:
	enum
	{ 
		ERROR_SLOT = -1,
		A_SLOT = 0,
		B_SLOT = 1,
		MAX_SLOT,
	};

	CFLWndCostume_Mix();
	~CFLWndCostume_Mix();

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnRButtonDown  ( UINT nFlags, CPoint point );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnMouseWndSurface( CPoint point );
	virtual	void OnDestroy( void );

	void		OnStart();
	FLItemElem*	GetSelCostumeItem() { return m_pItemElem; }
	void		SetSelCostumeItem( FLItemElem* pItemElme ) { m_pItemElem = pItemElme; }
	void		ReceivedPacket( BOOL bResult );
	void		EnableChildWnd( DWORD dwWndId, BOOL bEnable );

private:

	FLItemElem	*m_pItemElem;
	//Group_A
	CRect		m_rect_Slot_A[N_MAX];
	//Group_B
	CRect		m_rect_Slot_B[N_MAX];
	//Group_Result
	CRect		m_rect_Slot_Result[N_MAX];

	OBJID		m_objID_Slot_A[N_MAX];
	OBJID		m_objID_Slot_B[N_MAX];
	OBJID		m_objID_Slot_Result[N_MAX];
	int			m_nSlotInput[N_MAX];	

	BOOL			SendPacket();

public:
	static DWORD	IsCostume(PT_ITEM_SPEC pProp);
};


class CFLWndCostume_NPC: public CWndNeuz
{
public:
	CFLWndCostume_NPC();
	~CFLWndCostume_NPC();
	enum COSTUME_WND
	{	
		COSTUME_BUY,		//재료 구입
		COSTUME_SELL,		//재료 판매
		COSTUME_MIX,		//합성 
	};
	virtual void OnRButtonDown  ( UINT nFlags, CPoint point );
	CWndNeuz* CreateWnd		( COSTUME_WND  eWnd );
    CFLWndCostume_Mix*	GetCostumeMix(){ return m_pCostume_Mix; }
	void ReceivedPacket( COSTUME_WND  eWnd , BOOL bResult );
protected:
    CFLWndCostume_Mix* m_pCostume_Mix;
};