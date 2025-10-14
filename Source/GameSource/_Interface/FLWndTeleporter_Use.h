#pragma  once

class CFLWndChangeName;

class CFLWndTeleporter_Use : public CFLWndTeleporterMng
{
public:
	CFLWndTeleporter_Use(CTexture* pTexMap, DWORD dwItemObjID, std::vector< T_TELEPORTMAP_POINT > vecUserPoint);
	~CFLWndTeleporter_Use();

	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender,BYTE BySelMapLocationID,CPoint pointMouse  ); 
	virtual	void PaintFrame		( C2DRender* p2DRender );
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnRButtonDown  ( UINT nFlags, CPoint point );
	virtual void OnLButtonDown  ( UINT nFlags, CPoint point );
	virtual	BOOL Process		( void );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnMouseWndSurface( CPoint point );

	void SetAddUserPoint(T_TELEPORTMAP_POINT tUsePoint);
	void SetDelUserPoint(T_TELEPORTMAP_POINT tUsePoint);
	void SetDelAllUserPoint();
	void SetChangeUserPointName(T_TELEPORTMAP_POINT tUsePoint);
	void OpenChangeName();
	T_TELEPORTMAP_POINT GetetAddUserPoint();
	void SendPacket_AddPoint();
	void RenderAddUserPoint( C2DRender* p2DRender, 
  							BYTE BySelMapLocationID,
							CPoint pointMouse,
							CRect rectRevisedMapPosition,
							float fRevisedMapSizeRatio );

	void Req_ChangePointName(CString strChangeName);

private:
	void InitializeAddPointInformation();
	void SendPacket();// 텔레포트 이동을 요청함 
	void Req_DelPoint();
	void Req_DelAllPoint();

private:
	DWORD					m_dwItemObjID;
	CTexture*				m_pTexTeleport_User;
	CWndMenu				m_menuUsePosition;				//팝업메뉴 
	int						m_nTelID;						// 텔레포트 인덱스 	
	int						m_nChangeId;
	CRect					m_rectIconPT;					// 아이콘 좌표
	T_TELEPORTMAP_POINT		m_tUserPoint;
	BOOL					m_bIsChangeName;
	CString					m_strChangeName;
	FLPacketTeleportMapToPointReq		m_kTeleporter;		//텔레포트 요청 
	FLPacketTeleportMapAddMyPointReq	m_kAddMyPoint;		//유저 위치 등록 요청
	FLPacketTeleportMapDelPointReq		m_kDelPoint;		//삭제요청
	FLPacketTeleportMapDelAllPointReq	m_kDelAllPoint;		//모두 삭제 요청
	FLPacketTeleportMapChgPointNameReq	m_kChangePointName;	//이름 수정 요청

	TeleportationIconInfo*				m_pTeleportIconInfo;	//띄울때 툴팁에 필요한 정보

	std::vector< TeleportationIconInfo* >	m_vecTeleportationPositionRect_User;
	std::vector< T_TELEPORTMAP_POINT >		m_vecUserPoints;
	CFLWndChangeName*						m_pWndChangeName;
};

class CFLWndChangeName : public CWndNeuz
{
public:
    CFLWndChangeName();
	~CFLWndChangeName();

	virtual	void OnInitialUpdate( );
	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	CString		GetChageName(){return m_strChangeName;}

private:
	CString		m_strChangeName;
	CWndEdit* m_pWndEditChangeName;
	
};