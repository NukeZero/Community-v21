// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDWORLD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDWORLD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 헬프 이중생성 방지
extern vector<CString> g_vecHelpInsKey;

typedef struct tagCAPTION
{
	TCHAR m_szCaption[ 512 ];
	CD3DFontAPI* m_pFont;

	CTexture m_texture;
	CSize m_size;
	FLOAT m_fXScale;
	FLOAT m_fYScale;
	FLOAT m_fAddScale;
	int m_nAlpha;

} CAPTION,* LPCAPTION;

class CCaption
{
public:
	BOOL m_bEnd;
	CTimer m_timer;
	int m_nCount;
	CPtrArray m_aCaption;
	CCaption();
	~CCaption();

	void RemoveAll();
	void Process();

	void Render( CPoint ptBegin, C2DRender* p2DRender );
	void AddCaption( LPCTSTR lpszCaption, CD3DFontAPI* pFont );

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT DeleteDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
};
#ifdef __V050322_CACHEQUEST
class CCapTime 
{
public:
	CD3DFontAPI* m_pFont;
	CTexture m_texture;
	CSize m_size;

	FLOAT m_fXScale;
	FLOAT m_fYScale;
	FLOAT m_fAddScale;
	int m_nAlpha;
	int m_nTime;
	int m_nStep;
	BOOL m_bRender;
	CCapTime();
	~CCapTime();

	void Process();
	void Render( CPoint ptBegin, C2DRender* p2DRender );
	void SetTime( int nTime, CD3DFontAPI* pFont );
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT DeleteDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
};
#endif
#ifdef __V050322_CACHEQUEST
extern CCapTime g_CapTime;
#endif //__V050322_CACHEQUEST
extern CCaption g_Caption1;
//////////////////////////////////////////////////////////////////////////////////////
// 월드 
//
#define NEXTSKILL_NONE				0xffffffff	// 다음스킬 사용하지 않음
#define NEXTSKILL_ACTIONSLOT		0x7fffffff	// 다음스킬 액션슬롯으로 사용
// 그외는 스킬인덱스.

typedef struct BUFFSKILL
{
	BOOL m_bFlsh;
	int   m_nAlpha;
	CTexture* m_pTexture;
} BUFFSKILL;

typedef	struct	_SET_NAVIGATOR
{
	DWORD dwWorldId;
	D3DXVECTOR3 vPos;
	_SET_NAVIGATOR()
	{
		dwWorldId	= 0;
	}
}
SET_NAVIGATOR;


struct BUFFICON_INFO
{
	CPoint	pt;			// render위치 
	int		nCount;		// 그려진 갯수 
	int		nDelta;		// 이동할 좌표 
};

struct BUFFICONRECT_INFO
{
	RECT    rc;
	DWORD	dwID;
};

class CWndWorld : public CWndNeuz
{
	BOOL m_bBGM;
	FLOAT m_fHigh;
	FLOAT m_fLow;
	CPoint m_ptMouseOld;
	CPoint m_ptMouseSpot;
	BOOL m_bFreeMove;
	BOOL m_bFlyMove;
	BOOL m_bSelectTarget;
#ifdef __VRCAMERA
	BOOL m_bCameraMode;
#endif
public:

#ifdef __Y_POST_0912
	BOOL	m_IsMailTexRender;
#endif //__Y_POST_0912


	DWORD m_dwIdBgmMusic;
	CTexturePack m_texTarget;		// 지상에서의 4귀퉁이 타겟그림
	CTexturePack m_texTargetFly;	// 비행중에서의 4귀퉁이 타겟그림.
	CTexturePack m_texTargetArrow;	// 타겟이 화면을 벗어났을때 화살표방향표시.
	CTexturePack m_texGauFlight;	// 비행 게이지 인터페이스.

	BOOL s_bUped, s_bDowned, s_bLefted, s_bRighted;
	BOOL s_bCombatKeyed, s_bFlyKeyed, s_bAccKeyed, s_bSitKeyed;
	BOOL s_bTempKeyed, m_bTemp2ed, m_bTemp3ed;
	BOOL s_bBoarded;

	CTimer m_timerFocusAttack;
	CTimer m_timerLButtonDown;
	CTimer m_timerRButtonDown;
	//CTimer m_timerSpell;
	DWORD  m_dwPowerTick;				// 마우스를 누른 상태로 힘을 모아 공격하는 패턴에 사용하는 시작시각 
#ifdef __S0126_MOTION_AUTORUN
	CTimer m_timerAutoRun;
	CTimer m_timerAutoRunPush;
	CTimer m_timerAutoRunBlock;
	BOOL   m_nDubleUp;
#endif // __S0126_MOTION_AUTORUN
//	CTexture m_texFlaris;

	//CObj* m_pCaptureObj;

	CWndMenu m_wndMenuMover;

	//ect m_rectBound;

	CTexture m_texLvUp;
	CTexture m_texLvDn;
	CTexture m_texLvUp2;
	CTexture m_texLvDn2;
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal ;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	//CWndTradeGold* m_pWndTradeGold;
	int		m_nMouseMode;		// 비행중일때. 0:우버튼+드래그 빗자루회전   1:걍드래그 빗자루회전.
	
	int		ControlPlayer( DWORD dwMessage, CPoint point );
	int		ControlGround( DWORD dwMessage, CPoint point );
	int		ControlFlying( DWORD dwMessage, CPoint point );
	int		ControlShip( DWORD dwMessage, CPoint point );
	
	//CTexture m_texTargetGauge;
	CModelObject m_meshArrow; // 타겟의 방향을 알려주는 화살표 오브젝트이다.
	
  #ifdef __NPP_050308
	CModelObject	m_meshArrowWanted; // 타겟의 방향을 알려주는 화살표 오브젝트이다.
	BOOL			m_bRenderArrowWanted;
	DWORD			m_dwRenderArrowTime;
	D3DXVECTOR3		m_v3Dest;
	void			SetRenderArrowWanted( BOOL b, D3DXVECTOR3 vDest ) { m_v3Dest = vDest; m_bRenderArrowWanted = b; m_dwRenderArrowTime = timeGetTime(); }
  #endif //__NPP_050308
	
	void RenderArrow();
	BOOL UseSkillToFocusObj( CCtrl* pFocusObj );
	void GetBoundRect( CObj* pObj, CRect* pRect );
	
	CDWordArray		m_aFlyTarget;

	DWORD	m_dwNextSkill;		// 치고있는중에 스킬치기 예약.
	vector <BUFFICONRECT_INFO> m_rcCheck;
public:
	BOOL UpdateMailRender();
	BOOL GetBuffIconRect( DWORD dwID, const CPoint& point );
	int  GetBuffTimeGap();
	void RenderOptBuffTime( C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor );
	void RenderWantedArrow();
#ifdef __YMONSTER_BUFFRENDER
	void RenderMoverBuff( CMover* pMover, C2DRender *p2DRender );
#endif //__YMONSTER_BUFFRENDER


	CD3DFontAPI* m_pFontAPICaption;
	CD3DFontAPI* m_pFontAPITitle;
#ifdef __V050322_CACHEQUEST
	CD3DFontAPI* m_pFontAPITime;
#endif

	CTexturePack	m_texMsgIcon;
	SET_NAVIGATOR	m_stnv;

#ifdef __YATTRICON
	CTexturePack	m_texAttrIcon;
#endif //_YATTRICON	

	
	CTexture			m_pTextureLogo[CUSTOM_LOGO_MAX];
	
	DWORD				m_dwOneSecCount;
	ADDSMMODE			m_AddSMMode;
	CTexture*			m_dwSMItemTexture[SM_MAX ];
	CTexture*			m_dwSMResistItemTexture[10];
	BOOL				m_bSMFlsh[SM_MAX];
	int					m_nSMAlpha[SM_MAX];
	
#ifdef __YGUIDESYSTEM0923
	CWndGuideSystem*    m_pWndGuideSystem;	
#endif //CWndCGuideSystem 
	
	CWndReSkillPoint_Text* m_pWndReSkillPoint_Text;
	CWndReSkillPoint*   m_pWndReSkillPoint;


	void UseSkill();
	DWORD m_dwDropTime;
	CTexturePack m_texFontDigital;	// 디지탈 모양의 폰트(비행속도에 사용)
	
	static D3DXVECTOR3 m_vTerrainPoint;
	
	CRect m_rectSelectBegine;
	BOOL m_bNewSelect;
	//CObj* m_pSelectObj;
	BOOL m_bAutoAttack;
#ifdef __NPP_050308
	OBJID	m_idLastTarget;
	BOOL	IsLastTarget( OBJID objid )	{	return( m_idLastTarget == objid );	}
	void	SetLastTarget( OBJID objid )	{	m_idLastTarget	= objid;	}
#endif	// __NPP_050308
	DWORD	GetNextSkill( void ) { return m_dwNextSkill; }
	void	SetNextSkill( DWORD dwNextSkill )
	{
		m_dwNextSkill = dwNextSkill;
	}

	float	m_fRollAng;		// 롤링 앵글.

#ifdef __GUILDCOMBAT	
	int		n_nMoverSelectCount;
	DWORD	m_dwGuildCombatTime;
	char	m_szGuildCombatStr[64];
	struct __GuildCombatJoin
	{
		u_long uidGuild;
		char szJoinGuildName[MAX_NAME];
		int nJoinSize;
		int nOutSize;
	};
	map< u_long, __GuildCombatJoin > m_GuildCombatJoin;
	vector< __GuildCombatJoin > m_vGuildCombatSort;
#endif //__GUILDCOMBAT

	CWndButton m_wndMenu;

	D3DXVECTOR3		m_vTelePos;
	void SetMouseMode( int nMode ) 
	{
		m_nMouseMode = nMode;
		if( nMode == 0 )
			ClipCursor( NULL ); // 커서가 다시 윈도우를 벗어나게 한다.
	}
	int	GetMouseMode( void ) { return m_nMouseMode; }

	void Projection( LPDIRECT3DDEVICE9 pd3dDevice );

	CObj* PickObj( POINT point, BOOL bOnlyNPC = FALSE );
	CObj* SelectObj( POINT point );
	CObj* HighlightObj( POINT point );

	int		m_nSelect;		// 현재 선택된 타겟 인덱스.
	void	ClearFlyTarget( void )
	{
		m_aFlyTarget.RemoveAll();
	}

	void	AddFlyTarget( OBJID idMover )
	{
		m_aFlyTarget.Add( idMover );
	}
	
	OBJID	m_objidTracking;

	CWndWorld();
	virtual ~CWndWorld();
	void RenderAltimeter();
	void RenderSelectObj( C2DRender* pRender );
	void RenderFocusObj( CRect rect, DWORD dwColor1, DWORD dwColor2 );
	void RenderFocusArrow( CPoint pt );
	void RenderGauFlight( C2DRender* p2DRender );		// 비행모드시 게이지 인터페이스 Draw

	BOOL						m_bFirstFlying;
	int                         m_nLimitBuffCount;
	CSkillInfluence             m_SkillState;
	vector< multimap<DWORD, BUFFSKILL> >	m_pBuffTexture;

	void RenderBuff( C2DRender* p2DRender );
#ifdef __OVERLAPPING_EXPITEM_S0601
	void RenderExpBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BUFFICON_INFO* pInfo, CPoint ptMouse, DWORD dwItemID );
#endif // __OVERLAPPING_EXPITEM_S0601
	void RenderBuffIcon( C2DRender *p2DRender, SKILLINFLUENCE* pSkill, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse );
	void RenderSMBuff( C2DRender *p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse );
	void RenderCasting(C2DRender *p2DRender);

	virtual void OnDraw(C2DRender* p2DRender); 
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnMButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL Process();
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual LRESULT WndMsgProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnMouseWndSurface( CPoint point );
	
	BOOL UseFocusObj( CCtrl* pFocusObj );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

private:
	void	ShowMoverMenu( CMover* pTarget );
	int		GetGaugePower( int* pnValue );

};
#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
