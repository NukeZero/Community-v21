#pragma once


class CFLWndTeleporter_General : public CFLWndTeleporterMng
{
public:
	CFLWndTeleporter_General(CTexture* pTexMap,DWORD dwItemObjID);
	~CFLWndTeleporter_General();

	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate( );
	virtual void OnRButtonDown  ( UINT nFlags, CPoint point );
	virtual void OnLButtonDown  ( UINT nFlags, CPoint point );
	virtual	BOOL Process		( void );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnMouseWndSurface( CPoint point );

	void SendPacket();
	FLPacketTeleportMapToPointReq GetPacket(){return m_kTeleporter;}
	void RenderTeleportationPosition( C2DRender* p2DRender, 
									  BYTE BySelMapLocationID, 
									  CPoint pointMouse,
									  CRect rectRevisedMapPosition,
									  float fRevisedMapSizeRatio);

private:
	
	void InitializeTeleportInformation();								//텔레포트 지도 정보

private:
	CTexture*		m_pTexTeleport_General;
	FLPacketTeleportMapToPointReq m_kTeleporter;

	std::vector< TeleportationIconInfo* > m_vecTeleportationPositionRect;
	TeleportationIconInfo*				m_pTeleportIconInfo;	//띄울때 툴팁에 필요한 정보

	DWORD			m_dwItemObjID;	
	CRect			m_rectIconPT;	// 아이콘 좌표
	BYTE			m_byItemType;	// 일반형 or 유저등록형 
	int				m_nTelID;		// 텔레포트 인덱스 	
};
