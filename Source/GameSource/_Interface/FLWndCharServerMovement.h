#pragma  once


//_JIN_CHAR_SERVER_MOVEMENT
class CFLWndCharServerMovement_Warning : public CWndNeuz
{
public:
	CFLWndCharServerMovement_Warning();
	~CFLWndCharServerMovement_Warning();

	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent, DWORD dwWndId);
	virtual BOOL OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );

	void SetItemID(OBJID dwObjId,DWORD dwItemId);

private:
	OBJID			m_dwObjId;
	DWORD			m_dwItemId;	
};


class CFLWndCharServerMovement_Select : public CWndNeuz
{
public:
	CFLWndCharServerMovement_Select();
	~CFLWndCharServerMovement_Select();

	virtual	void OnInitialUpdate();
	virtual	BOOL Initialize(CWndBase* pWndParent, DWORD dwWndId);
	virtual BOOL OnChildNotify( UINT message,UINT nID,LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	void ReceivedPacket( FLSnapshotCharacterServerTransformNoti* pkcharServer);
	void ReceivedPacket_LogOut( FLPacketCharacterServerTransformAck *pkPacketLogOut );

private:
	std::vector< T_SERVER_DESC > m_vecServerDesc;

	CWndText*		m_pWndListMove;
	CWndListBox*	m_pWndListServer;
	DWORD			m_dwItemObjID;
	void SelectWorld();
};