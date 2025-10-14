#ifndef __WND_MAP_EX_H__
#define __WND_MAP_EX_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "TeleportationIconInfo.h"
#include "WndUserMarkNameChanger.h"


namespace WndMapEx
{
	enum eQuestEmoticonState
	{
		eState_None = 0,
		eState_PossibleStart,
		eState_Progress,
		eState_Complete,
	
		eState_Max,	
	};
}

using namespace WndMapEx;

class CWndMapEx : public CWndNeuz
{
public:
	enum ConstructionMode { NORMAL, TELEPORTATION, DESTINATION, HYPERLINK };

public:
	CWndMapEx( void );
	~CWndMapEx( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual	BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove( UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );

public:
	void SetConstructionMode( ConstructionMode eConstructionMode );
	void SetHyperlinkMarkPosition( 
		const CString& strHyperlinkMarkTitle, 
		FLOAT fHyperlinkMarkPositionX, 
		FLOAT fHyperlinkMarkPositionY );
	void InitializeTeleportationInformation( CMover* const pFocusMover );

	void OnBlindBuff( );	//닫혀진 지도 열기
	static BYTE GetMapArea( const D3DXVECTOR3& vPosition, BOOL bTownFirst = TRUE, BOOL bTownOnly = TRUE );

private:
	void ResetMapInformation( void );
	void ResetNPCPosition( void );
	void RearrangeComboBoxData( 
		DWORD dwParentSelectedListItemData, 
		DWORD dwComboBoxWIDC, 
		MapComboBoxDataVector* pMapComboBoxDataVector );
	void ProcessMapComboBoxInitialization( void );
	void ProcessMapSizeInformation( void );
	void ProcessMonsterInformationToolTip( void );
	void ProcessUserMarkToolTip( void );
	void ProcessHyperlinkMarkToolTip( void );
	void ProcessIconTextureAlpha( void );
	void ProcessVisualSetting( void );
	void RenderBackground( C2DRender* p2DRender );
	void RenderTeleportationPosition( C2DRender* p2DRender );
	void RenderDestinationPosition( C2DRender* p2DRender );
	void RenderNPCPosition( C2DRender* p2DRender );
	void RenderUserMarkPosition( C2DRender* p2DRender );
	void RenderHyperlinkMarkPosition( C2DRender* p2DRender );
	void RenderPlayerAndPartyPlayerPosition( C2DRender* p2DRender );
	void RenderPlayerPosition( 
		C2DRender* p2DRender, 
		CTexture* pArrowTexture, 
		const D3DXVECTOR3& vPlayerPosition, 
		const D3DXVECTOR3& vCameraPosition, 
		const CString& strName, DWORD dwNameColor );
	void RenderMapMonsterInformation( C2DRender* p2DRender );
	void RenderRainbowNPCInformation( C2DRender* p2DRender );
	FLOAT CalculateMapIconRectFromPoint( 
		CRect& rectDestinationIcon, 
		FLOAT fIconPositionX, 
		FLOAT fIconPositionY, 
		const CTexture* const pIconTexture, 
		FLOAT fIconSizeRatio );
	FLOAT CalculateMapIconStartPosition( 
		CPoint& pointDestinationPosition, 
		FLOAT fIconPositionX, 
		FLOAT fIconPositionY, 
		const CTexture* const pIconTexture, 
		FLOAT fIconSizeRatio );
	const D3DXVECTOR3& ConvertPosition( 
		D3DXVECTOR3& vDestination, 
		const D3DXVECTOR3& vSource, 
		BYTE byLocationID );
	const D3DXVECTOR3& ReconvertPosition( 
		D3DXVECTOR3& vDestination, 
		const D3DXVECTOR3& vSource, 
		BYTE byLocationID );
	const CRect& ReviseScriptRectInformation( 
		CRect& rectDestination, 
		const CRect& rectSource );
	BOOL IsPositionInSelectedMapArea( const D3DXVECTOR3& vPlayerPosition );
	void CalculateMaximumWindowTileLength( void );

	//_SUN_RENDER_QUESTEMOTICON
	BOOL ScanQuest( );		//모든 퀘스트에 대하여 현재 수행가능, 수행중, 완료 상태에 해당하는 퀘스트들을 모두 모은다.
	void RenderQuestEmoticon( C2DRender* p2DRender );
	std::map< int, int > m_mapMyQuestForEmoticon;	// queset index, queset state
	CTexturePack m_kTexPackQuest;

	static BYTE		GetMapLocationIDFromWorldID( const DWORD dwWorldID );

	float			CalculateQuestIconStartPosition(OUT CPoint&				ptDestinationPosition, 
													float					fIconPositionX, 
													float					fIconPositionY, 
													const CTexture* const	pIconTexture, 
													float					fIconSizeRatio );

	bool			GetQuestEmoticonPosition( OUT D3DXVECTOR3& vPos, OUT DWORD& dwWorldID, const int nQuestState, const QuestProp* const pQuestProp );
	bool			IsQuestNpcInSelectMapArea( const D3DXVECTOR3 vPos, const DWORD dwGoalWorldID );

private:
	enum { MINIMUM_ICON_TEXTURE_ALPHA = 0, MAXIMUM_ICON_TEXTURE_ALPHA = 255 };
	enum { NORMAL_STATE_ALPHA = 255, TRANSPARENT_STATE_ALPHA = 125 };
	enum
	{
		ID_USER_MARK_MENU_DELETE = 0, 
		ID_USER_MARK_MENU_DELETE_ALL = 1, 
		ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW = 2, 
		ID_USER_MARK_MENU_CHANGE_NAME = 3, 
	};

private:
	static const FLOAT SOURCE_MAP_SIZE_X;
	static const FLOAT SOURCE_MAP_SIZE_Y;
	static const FLOAT ANIMATION_SPEED;
	static const FLOAT PC_ARROW_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT EXTEND_RATIO;
	static const FLOAT EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT DESTINATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT NPC_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT USER_MARK_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT HYPERLINK_MARK_POSITION_TEXTURE_SIZE_RATIO;
	static const int WINDOW_TILE_TEXTURE_SIZE_XY;
	static const int MINIMUM_WINDOW_TILE_NUMBER_X;
	static const int MINIMUM_WINDOW_TILE_NUMBER_Y;

private:
	ConstructionMode m_eConstructionMode;
	CTexture* m_pPCArrowTexture;
	CTexture* m_pPartyPCArrowTexture;
	CTexture* m_pMapTexture;
	CTexture* m_pTeleportationPositionTexture;
	CTexture* m_pDestinationPositionTexture;
	CTexture* m_pNPCPositionTexture;
	CTexture* m_pUserMarkPositionTexture;
	CTexture* m_pHyperlinkMarkPositionTexture;
	FLOAT m_fRevisedMapSizeRatio;
	CRect m_rectRevisedMapPosition;
	DWORD m_dwSelectedMapID;
	BYTE m_bySelectedMapLocationID;
	BOOL m_bShowMonsterInformation;
	BOOL m_bMonsterInformationToolTip;
	int m_nSelectedMonsterIconIndex;
	DWORD m_dwSelectedUserMarkID;
	int m_nIconTextureAlpha;
	DWORD m_tmOld;
	BOOL m_bAlphaSwitch;
	OBJID m_idTeleporter;
	std::vector< TeleportationIconInfo* > m_vecTeleportationPositionRect;
	FLOAT m_fNPCPositionX;
	FLOAT m_fNPCPositionY;
	CString m_strHyperlinkMarkTitle;
	FLOAT m_fHyperlinkMarkPositionX;
	FLOAT m_fHyperlinkMarkPositionY;
	BOOL m_bMapComboBoxInitialization;
	BYTE m_byTransparentStateAlpha;
	CWndMenu m_WndMenuUserMark;
	CWndUserMarkNameChanger* m_pWndUserMarkNameChanger;
	int m_nMinimumWindowTileWidth;
	int m_nMinimumWindowTileHeight;
	int m_nMaximumWindowTileWidth;
	int m_nMaximumWindowTileHeight;
	int m_nWndTaskBarHeight;
};
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __WND_MAP_EX_H__