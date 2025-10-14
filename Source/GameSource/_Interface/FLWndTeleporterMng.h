#pragma once

#include "TeleportationIconInfo.h"
#include "WndUserMarkNameChanger.h"

class CFLWndTeleporter_General;
class CFLWndTeleporter_Use;

class CFLWndTeleporterMng : public CWndNeuz 
{
public:
	CFLWndTeleporterMng();
	~CFLWndTeleporterMng();

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnRButtonDown  ( UINT nFlags, CPoint point );
	virtual void OnLButtonDown  ( UINT nFlags, CPoint point );
	virtual	BOOL Process		( void );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnMouseWndSurface( CPoint point );

	void SendPacket( const FLPacket* pkPacket );
	void ReceivedPacket( FLSnapshotTeleportMapOpenNoti* pkOpenTeleport);
	void RecComplatePacket( FLSnapshotTeleportMapToPointAck* pkComplateTeleport);
	void RecAddPointPacket( FLSnapshotTeleportMapAddMyPointAck* pkAddPointTeleport);
	void RecDelPointPacket( FLSnapshotTeleportMapDelPointAck* pkDelPointTeleport);
	void RecDelAllPointPacket( FLSnapShotTeleportMapDelAllPointAck* pkDelAllPointTeleport);
	void RecChangePointNamePacket( FLSnapshotTeleportMapChgPointNameAck* pkChangePointNameTeleport);

	void RenderMap( C2DRender* p2DRender );		//기존 맵 정보 
	void ProcessMapSizeInformation();
	BOOL IsPositionInSelectedMapArea( const D3DXVECTOR3& vPosition );	
	void ResetMapInfo();
	void SetPointState(int nState){m_nPointState = nState;}
	int	 GetPointState(){return m_nPointState;}

	const D3DXVECTOR3& ConvertPosition( 
		D3DXVECTOR3& vDestination, 
		const D3DXVECTOR3& vSource, 
		BYTE byLocationID,
		CRect rectRevisedMapPosition );

	const D3DXVECTOR3& ReconvertPosition( 
		D3DXVECTOR3& vDestination, 
		const D3DXVECTOR3& vSource, 
		BYTE byLocationID,
		CRect rectRevisedMapPosition);
	
	const CRect& ReviseScriptRectInformation( 
		CRect& rectDestination, 
		const CRect& rectSource,
		float fRevisedMapSizeRatio,
		CRect rectRevisedMapPosition);
	
	
	FLOAT CalculateMapIconRectFromPoint( CRect& rectDestinationIcon, 
		FLOAT fIconPositionX, 
		FLOAT fIconPositionY, 
		const CTexture* const pIconTexture, 
		FLOAT fIconSizeRatio,
		float fRevisedMapSizeRatio);

	FLOAT CalculateMapIconStartPosition( 
		CPoint& pointDestinationPosition, 
		FLOAT fIconPositionX, 
		FLOAT fIconPositionY, 
		const CTexture* const pIconTexture, 
		FLOAT fIconSizeRatio,
		float fRevisedMapSizeRatio );

	BYTE GetMapArea( const D3DXVECTOR3& vPosition );

	static const FLOAT SOURCE_MAP_SIZE_X;
	static const FLOAT SOURCE_MAP_SIZE_Y;
	static const FLOAT TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT EXTEND_RATIO;
	CFLWndTeleporter_General* GetTelGeneral() {return m_pTelGeneral;}
	CFLWndTeleporter_Use* GetTelUse() {return m_pTelUse;}
private:
	enum { NORMAL_STATE_ALPHA = 255, TRANSPARENT_STATE_ALPHA = 125 };

private:
	CTexture*		m_pMapTexture;
	CTexture*		m_pTexTeleport_General;
	CTexture*		m_pTexTeleport_Use;
	
	CRect			m_rectRevisedMapPosition;
	BYTE			m_byTransparentStateAlpha;
	float			m_fRevisedMapSizeRatio;
	BYTE			m_bySelectedMapLocationID;
	D3DXVECTOR3		m_vConvertedPosition;
	
	int				m_nPointState;

	DWORD			m_dwItemObjID;	
	BYTE			m_byItemType;	// 일반형 or 유저등록형 

	std::vector< T_TELEPORTMAP_POINT >	m_vecUserPoints;
	
	CFLWndTeleporter_General*	m_pTelGeneral;
	CFLWndTeleporter_Use	*   m_pTelUse;


};