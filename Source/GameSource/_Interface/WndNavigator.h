
#pragma once 

namespace WndNavigator
{
	enum eObjectFilter : BYTE
	{
		OBJFILTER_PLAYER = 0, 
		OBJFILTER_PARTY, 
		OBJFILTER_NPC,    
		OBJFILTER_MONSTER,

		OBJFILTER_MAX,
	};

	static const WORD		MAX_DRAW_OBJECT_COUNT( 1000 );
	static const WORD		VERTEXCOUNT_OF_ONE_RECTANGLE( 6 );
};

using namespace WndNavigator;

//---------------------------------------------------------------------------------------------
// 윈도우 네비게이션
//---------------------------------------------------------------------------------------------
class CWndNavigator : public CWndNeuz
{

public:


												CWndNavigator( void );
	virtual										~CWndNavigator( void );

	virtual BOOL 								Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);

	BOOL										UpdateRenderTarget( void );	
	void										SetRegionName( TCHAR *tszName );		

	virtual	void 								OnInitialUpdate( void );

	virtual HRESULT 							InvalidateDeviceObjects( void );
	virtual HRESULT 							RestoreDeviceObjects( void );
	virtual HRESULT 							DeleteDeviceObjects( void );

	virtual BOOL								OnEraseBkgnd( C2DRender* p2DRender );
	virtual	void								PaintFrame( C2DRender* p2DRender );
	virtual void 								OnDraw( C2DRender* p2DRender );

	virtual BOOL								Process( void );

	virtual BOOL								OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL								OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void								OnLButtonDown( UINT nFlags, CPoint point );
	virtual void								OnRButtonDown( UINT nFlags, CPoint point );
	virtual void								OnLButtonDblClk( UINT nFlags, CPoint point );

	virtual void								SerializeRegInfo( CAr& ar, DWORD& dwVersion );

	virtual	void 								SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void 								OnSize(UINT nType, int cx, int cy);


private:


	void										InitializeMenu( void );
	void										CalculationWindowPosition( void );

	void 										RenderMarkAll( C2DRender* p2DRender , CMover* Player );
	void 										RenderMark( C2DRender* p2DRender, CMover* Player );	
	void 										ResizeMiniMap( void );

	inline void									AccuFrame( void )
												{ 
													DWORD CurTime = g_tmCurrent;
												
													if (CurTime - m_iPastTime > 120)
													{
														m_iFrame++; 
														m_iPastTime = CurTime;
													}

													if ( m_iFrame >= 4 )
														m_iFrame = 0;
												};


	bool										IsPointInNavigationCircle( const CPoint ptDestPos );

	void										RenderRainbowRaceTime( C2DRender* p2DRender );
	bool										RenderLandScape( C2DRender* p2DRender );

	void										MakeVertexNormalObject( C2DRender* p2DRender, const CPoint ptCenter, int& nQuestNum, CMover** ppQuest, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices );
	void										MakeVertexRegionObject( C2DRender* p2DRender, const CPoint ptCenter, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices );
	void										MakeVertexFocusObject( C2DRender* p2DRender, const CPoint ptCenter, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices );
	void										MakeVertexQuestEmoticon( C2DRender* p2DRender, const CPoint ptCenter, const int nQuestNum, CMover** ppQuest, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices );

	void										RenderDestinationArrow( C2DRender* p2DRender, const CPoint ptCenter );
	void										RenderPlayerArrow( C2DRender* p2DRender );
	

public:


	BOOL 										m_bObjFilterMonster;
	BOOL 										m_bObjFilterPlayer;
	BOOL 										m_bObjFilterParty;
	BOOL 										m_bObjFilterNPC;


private:


	CTexture*									m_pDestinationPositionTexture;
	CTexture*  									m_pTexDungeonFog;
	CTexture*  									m_pTexArrow;
	CTexture* 									m_pTexAlpha;
	CTexture*									m_pTexBlind;

	DWORD		 								m_iFrame;
	DWORD		 								m_iPastTime;

	CSize										m_size;
	CSize										m_kSizeCircle;
	int        									m_nSizeCnt;
	TCHAR      									m_szName[ 64 ];

	CWndButton 									m_wndPlace;
	CWndButton 									m_wndMover;
	CWndButton 									m_wndZoomIn;
	CWndButton 									m_wndZoomOut;
	CWndButton 									m_wndNewClose;
	CWndButton 									m_wndNewHelp;

	CWndMenu   									m_wndMenuPlace;
	CWndMenu   									m_wndMenuMover;

	BILLBOARD  									m_billboard;

	CTexture									m_GuildCombatTextureMask;
	CTexture 									m_kTexRenderTarget;	

	CTexturePack 								m_texNavObjs;
	CTexturePack 								m_texNavPos;

	CBillboard									m_billArrow;

	TEXTUREVERTEX								m_arrayObjVertex[ MAX_DRAW_OBJECT_COUNT * VERTEXCOUNT_OF_ONE_RECTANGLE ];

};







