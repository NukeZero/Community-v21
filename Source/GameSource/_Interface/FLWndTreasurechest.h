#pragma  once

class CFLWndTreasurechest_Open : public CWndNeuz
{
public:
	CFLWndTreasurechest_Open();
	virtual ~CFLWndTreasurechest_Open();

	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent, DWORD dwWndId);
	virtual BOOL OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	void ReceivedPacket( FLSnapshotTreasureChestOpenNoti* pkOpenTreasureChest );


private:
	CRect m_rectSlot_TreasureChest[2];	
	OBJID m_objID_TreasureChest[2];

	//게이지바 관련 
	CTexture* m_pTexGauge;
	CTexture* m_pTexGauge_Bg;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	
	DWORD m_dwItemId;

	BOOL	m_bGaugeBarState;
	BOOL	m_bSend;

	int		m_nCurrGauge;

	CWndStatic* m_pWndNoticeItem;
	CWndStatic* m_pWndKeyItem;
	CWndStatic* m_pWndGauge;

	HRESULT RestoreDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	void InitCtrl();
	void SendPacket();
};

class CFLWndTreasurechest_Result : public CWndNeuz
{
public:
	CFLWndTreasurechest_Result();
	virtual ~CFLWndTreasurechest_Result();

	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent, DWORD dwWndId);
	virtual BOOL OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	void ReceivedPacket( FLSnapshotTreasureChestOpenByKeyAck* pkOpenTreasureChest );

private:
	std::vector< T_TREASURE_ITEM >				m_vecTreasureItemInfo;
	std::vector< T_TREASURE_ITEM >::iterator	m_itrItemInfo;

	CRect m_rectSlot_TreasureChest_Result[3];	
	OBJID m_objID_TreasureChest_Result[3];
	CTexture*	   m_pTexItemEmptySLot;
};