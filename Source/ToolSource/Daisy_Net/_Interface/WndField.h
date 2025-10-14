// WndArcane.h: interface for the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __POST0906
#include "post.h"
#endif	// __POST0906

extern 	int g_nSkillCurSelect;


const int MAX_WANTED_LIST =	100;


//////////////////////////////////////////////////////////////////////////////////////
// 애플랫 윈도 
//
class CWndApplet : public CWndNeuz
{
public:
	CWndListBox m_wndList;
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	CWndButton m_wndButton3;

	CWndApplet();
	virtual ~CWndApplet();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

};

//////////////////////////////////////////////////////////////////////////////////////
// 드롭아이템 
//
class CWndDropItem : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;
	CWndEdit* m_pEdit;

	CWndDropItem(); 
	~CWndDropItem(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndDropConfirm : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;
	
	CWndDropConfirm(); 
	~CWndDropConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#ifdef __S050504_QUESTITEM
class CWndQuestItemWarning : public CWndNeuz 
{ 
public: 
	CItemElem* m_pItemElem;
	D3DXVECTOR3 m_vPos;

	CWndQuestItemWarning(); 
	~CWndQuestItemWarning(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif // __S050504_QUESTITEM

//////////////////////////////////////////////////////////////////////////////////////
//  퀘스트 아이템 정보 창
//
#ifdef __NQUESTITEM_INFO_040917
class CWndQuestItemInfo : public CWndNeuz
{
public:
	CWndQuestItemInfo();
	virtual ~CWndQuestItemInfo();

	CItemBase*	m_pItemBase;
	CPoint		m_Position;

	inline void SetItemBase(CItemBase* pItemBase = NULL) { m_pItemBase = pItemBase; };
	
	BOOL Create(CItemBase* pItemBase, UINT nID, CWndBase* pWndParent = NULL );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, CItemBase* pItemBase = NULL); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};
#endif	// __NQUESTITEM_INFO_040917

//////////////////////////////////////////////////////////////////////////////////////
// 인벤토리 
//

class CWndGold : public CWndButton
{
public:
	CTexture m_texture;
	CWndGold();
	~CWndGold();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnInitialUpdate();
};
class CWndConfirmBuy;

class CWndInventory : public CWndNeuz
{
#ifdef __YINVENTORY_3D
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject*		m_pModel;
	BOOL				m_bLButtonDownRot;
#endif //__YINVENTORY_3D
	
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	CModelObject m_meshStage;
	CItemElem* m_pSelectItem;
	BOOL m_bReport;
public:

#ifdef 	__YINVENTORY_3D
	void UpdateParts();
	CModelObject* GetModel() { return m_pModel; }
#endif //__YINVENTORY_3D

	void BaseMouseCursor();
	void RunUpgrade( CItemBase* pItem, int x, int y );
	void SetEnchantCursor();
	
#ifdef __YENCHANT_0608
	CSfx*		m_pSfxUpgrade;
	CItemBase*	m_pUpgradeItem;
	CItemBase*	m_pUpgradeMaterialItem;
	BOOL		m_bIsUpgradeMode;
	DWORD		m_dwEnchantWaitTime;
#endif //__YENCHANT_0608

	CWndConfirmBuy* m_pWndConfirmBuy;
	CWndGold     m_wndGold;
	CWndItemCtrl m_wndItemCtrl;
	CWndItemCtrl m_wndCardCtrl;
	CWndItemCtrl m_wndCubeCtrl;

	CWndInventory();
	virtual ~CWndInventory();
	virtual BOOL Process ();
	virtual void OnMouseWndSurface( CPoint point );
	//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
#ifdef	__NQUESTITEM_INFO_040917
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
#endif//__NQUESTITEM_INFO_040917
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual void OnDestroy( void );
	virtual BOOL OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message );
};

#ifdef __QUERYEQUIP0630
class CWndQueryEquip : public CWndNeuz
{
#ifdef __YINVENTORY_3D
	CPoint				m_OldPos;
	FLOAT				m_fRot;
	CModelObject*		m_pModel;
	BOOL				m_bLButtonDownRot;
#endif //__YINVENTORY_3D
	
	DWORD		 m_ObjID;
	CRect		 m_InvenRect[MAX_HUMAN_PARTS];

	EQUIP_INFO_ADD	m_aEquipInfoAdd[MAX_HUMAN_PARTS];
public:

	CMover* GetMover() 
	{ 
		if( m_ObjID != NULL_ID )
			return prj.GetMover( m_ObjID ); 

		return NULL;
	}
	void	SetMover( DWORD		 ObjID );
	void	SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd );
	
	CWndQueryEquip();
	virtual ~CWndQueryEquip();
	virtual BOOL Process ();
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual void OnMouseMove (UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
		
};
#endif //__QUERYEQUIP0630

/*
class CWndInventory : public CWndNeuz
{
	//CModel* m_pModel;
	//CModelObject* m_pModel;
	CWndConfirmBuy* m_pWndConfirmBuy;
	CModelObject m_meshStage;
	CItemElem* m_pSelectItem;
	BOOL m_bReport;
public:
	CWndGold     m_wndGold;
	CWndItemCtrl m_wndItemCtrl;
	CWndItemCtrl m_wndCardCtrl;
	CWndItemCtrl m_wndCubeCtrl;
	//CWndButton m_wndButton1;
	//CWndButton m_wndButton2;
	//CWndButton m_wndButton3;
	//CWndButton m_wndGold;
	
	CWndInventory();
	virtual ~CWndInventory();
	virtual void OnMouseWndSurface( CPoint point );
	//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
};
*/
//////////////////////////////////////////////////////////////////////////////////////
// 상태 
//
class CWndStateConfirm : public CWndNeuz 
{ 
public:
	UINT m_nId;
	void OnSetState( UINT nId );
	void SendYes( void );
public: 
	CWndStateConfirm(); 
	~CWndStateConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
class CWndCharacterBase : public CWndBase
{
	CTexture m_txInfo;

public:
	CWndCharacterBase();
	virtual ~CWndCharacterBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndCharacterDetail : public CWndBase
{
	CWndButton m_wndStr;
	CWndButton m_wndSta;
	CWndButton m_wndDex;
	CWndButton m_wndInt;
public:
	BOOL	m_fWaitingConfirm;
	CWndCharacterDetail();
	virtual ~CWndCharacterDetail();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	void RenderATK( C2DRender* p2DRender, int x, int y );
};
class CWndChangeJob;

class CWndPvpBase : public CWndBase
{
	CTexture m_txInfo;
	
public:
	CWndChangeJob* m_pWndChangeJob;
	BOOL	m_fWaitingConfirm;
	CWndButton m_wndChangeJob;
	BOOL m_bExpert;
	int m_nDisplay;			// 직업창 화면 옵션

	CWndPvpBase();
	virtual ~CWndPvpBase();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

class CWndChangeJob : public CWndNeuz
{
public:
	int m_nJob;
	CWndButton m_wndExpert[ 6 ];
	CWndButton m_wndOk;
	CWndButton m_wndCancel;

	CWndChangeJob( int nJob );
	virtual ~CWndChangeJob();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};
class CWndJob : public CWndBase
{
public:
	BOOL	m_fWaitingConfirm;
public:
	CWndJob();
	~CWndJob();
	CWndChangeJob* m_pWndChangeJob;
	BOOL m_bExpert;
	CTexture m_txInfo;
	CWndButton m_wndJob[ 5 ];
	CWndButton m_wndButton [ 6 ];
	CWndButton m_wndChangeJob;
	int m_nDisplay;			// 직업창 화면 옵션
	//CWndButton m_wndButton2[ 5 ];

	virtual	void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

class CWndCharacter : public CWndNeuz
{
	//CTexture m_txTab[ 3 ];
	//CTexture m_txInfo;
	//CWndButton m_wndStr;
	//CWndButton m_wndSta;
	//CWndButton m_wndDex;
	//CWndButton m_wndInt;
	//CWndTabCtrl m_wndTabCtrl;
public:
	CWndCharacterDetail m_wndStateDetail;
	CWndCharacterBase   m_wndStateBase;	
	CWndPvpBase         m_wndPvp;

public:
	CWndCharacter();
	virtual ~CWndCharacter();
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};
//////////////////////////////////////////////////////////////////////////////////////
// 스킬  
//
class CWndSkill : public CWndNeuz
{
	BOOL m_bReport;
public:
	
//	CWndButton m_wndJob[ 6 ];

	//CWndTabCtrl  m_wndTabCtrl;
	CWndSkillCtrl m_wndSkillCtrl[ 2 ];
	CWndSkill();
	virtual ~CWndSkill();
	void SetExpert( int nExpert );
	void SetPro( int nProJob );
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
};

#ifdef __YADV_RESKILLPOINT

class CWndReSkillControl : public CWndNeuz
{
public:
	CWndReSkillControl();
	virtual ~CWndReSkillControl();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

class CWndReSkillWarning : public CWndNeuz
{
public:
	CWndReSkillWarning();
	virtual ~CWndReSkillWarning();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL,DWORD dwWndId = 0 );
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

#endif //__YADV_RESKILLPOINT


//////////////////////////////////////////////////////////////////////////////////////
// 스킬 EX 
//
class CWndSkillTreeEx : public CWndNeuz
{
protected:
	int			  m_nCount;

	int           m_nCurSelect  ;
	BOOL          m_bDrag;
	LPSKILL       m_pFocusItem;
#ifdef __YADV_RESKILLPOINT
	CWndReSkillControl* m_pWndReSkillControl;
	DWORD		  m_dwCurrSkillPoint;
	LPSKILL       m_apSkill;
#else //__YADV_RESKILLPOINT
	LPSKILL       m_apSkill;	
#endif //__YADV_RESKILLPOINT
	CTexture*     m_atexSkill[ MAX_SKILL_JOB ];
	CTexture*	  m_aSkillLevel[ 3 ];
	//CTexture*     m_atexJobPannel[ 2 ];
	LPIMAGE       m_atexJobPannel[ 2 ];
	
	int           m_nJob         ;
	
	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

#ifdef __YTREE_TOPDOWN_050103
	CTexture*     m_atexTopDown[2];
	int 	m_nTopDownGap;
	BOOL	m_bSlot[3];
#endif //__YTREE_TOPDOWN_050103	
	
public:
#ifdef __YADV_RESKILLPOINT
	BOOL	IsReSkillPointDone();
	CWndReSkillControl* GetSkillControl() { return m_pWndReSkillControl; }
	DWORD	GetCurrSkillPoint() { return m_dwCurrSkillPoint; }
	LPSKILL GetSkill() { return m_apSkill; }
	void	PutToolTip_Skill( DWORD dwSkill, CPoint point, CRect* pRect );
	int		GetSkillPxp(DWORD dwID, DWORD dwLv);		
	BOOL	IsDownPoint(DWORD skill);
#endif //__YADV_RESKILLPOINT		
	int		GetCalcImagePos( int nIndex );
	int		GetCurSelect() { return g_nSkillCurSelect; }
	BOOL	GetSkillPoint(DWORD dwSkillID, CRect& rect );
	LPSKILL GetSkill( int i );
	LPSKILL GetdwSkill( DWORD dwSkill );
	BOOL	CheckSkill( int i );
	void LoadTextureSkillicon();
	void InitItem( int nJob, LPSKILL apSkill );
	CWndSkillTreeEx();
	virtual ~CWndSkillTreeEx();

#ifdef __V050322_CACHEQUEST
	void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
#else
	void AdditionalSkinTexture( LPWORD pDest, CSize size );
#endif

	void SetJob( int nJob );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL Process ();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk( UINT nFlags, CPoint point);
#ifdef __YTREE_TOPDOWN_050103
	void SetActiveSlot( int nSlot, BOOL bFlag );
	virtual void SerializeRegInfo ( CAr& ar, DWORD& dwVersion );
#endif //__YTREE_TOPDOWN_050103	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
};


//////////////////////////////////////////////////////////////////////////////////////
// 매신저  
//
class CWndVideoCapture;
class CWndVideoCapture : public CWndNeuz
{
public:
//	CWndMessanger::MESSAGE m_message;

	CWndVideoCapture();
	virtual ~CWndVideoCapture();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};
//////////////////////////////////////////////////////////////////////////////////////
// Mail
//
class CWndMail : public CWndNeuz
{
public:
	CWndMail();
	virtual ~CWndMail();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////////////////
// Navigator
//

class CWndNavigator : public CWndNeuz
{
	CWndButton m_wndPlace;
	//CWndButton m_wndUser;
	CWndButton m_wndMover;
	CWndButton m_wndZoomIn;
	CWndButton m_wndZoomOut;
	CWndMenu   m_wndMenuPlace;
	CWndMenu   m_wndMenuMover;
	BILLBOARD  m_billboard;
	//CTexture   m_texture;
	CTexture   m_texArrow;
	CTexture   m_texDunFog;
	//CTexture   m_texObj;
	CTexturePack m_texNavObjs;
	DWORD		 m_iFrame;
	DWORD		 m_iPastTime;
	CTexturePack m_texNavPos;
	inline void	 AccuFrame() { 
		DWORD CurTime = timeGetTime();
		if (CurTime - m_iPastTime > 120)
		{
			m_iFrame++; 
			m_iPastTime = CurTime;
		}
		if ( m_iFrame >= 4 )
			m_iFrame = 0;
	};
	void RenderMark( C2DRender* p2DRender, CMover* Player );	//	참조되는 변수는 플레이어와 파티플레이어를 얻을 수 있는 것이어야 하지만 
																//	현재는 자신만을 찍는 것을 하고 그 다음에 파티 플레이어들을 찾을수 있는 방법을 찾아 보자꾸나
	void RenderMarkAll( C2DRender* p2DRender , CMover* Player );
	CBillboard m_billArrow;
	CSize      m_size;
	int        m_nSizeCnt;
	TCHAR      m_szName[ 64 ];
	void ResizeMiniMap();
public:
	BOOL m_bObjFilterPlayer ;
	BOOL m_bObjFilterParty  ;
	BOOL m_bObjFilterNPC    ;
	BOOL m_bObjFilterMonster;
	

	void RenderPartyMember( C2DRender* p2DRender, TEXTUREVERTEX** pVertices, CRect rect, D3DXVECTOR3 vPos, u_long uIdPlayer, LPCTSTR lpStr );

	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	
	CWndNavigator(); 
	virtual ~CWndNavigator();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
};
//////////////////////////////////////////////////////////////////////////////////////
// CWndStatus
//

class CWndStatus : public CWndNeuz
{
	BOOL m_bHPVisible;
	BOOL m_bExpVisible;
public:
	CModel* m_pModel;
	int		m_nDisplay;
public:
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauEmptySmall  ;
	CTexture m_texGauFillNormal  ;
	CTexture m_texGauFillSmall   ;
	
	LPDIRECT3DVERTEXBUFFER9 m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBMPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBFPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBEXPGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVBPXPGauge;
	
	BOOL m_bVBHPGauge;
	BOOL m_bVBMPGauge;
	BOOL m_bVBFPGauge;
	BOOL m_bVBEXPGauge;
	BOOL m_bVBPXPGauge;
	
	int m_nHPWidth ;
	int m_nMPWidth ;
	int m_nFPWidth ;
	int m_nEXPWidth;
	int m_nPXPWidth;
	
	void MakeGaugeVertex();
	
	//void RenderGauge( CRect* pRect, C2DRender* p2DRender, LPDIRECT3DVERTEXBUFFER9 pVBGauge, CTexture* pTexture, DWORD dwColor );
	CWndStatus(); 
	virtual ~CWndStatus();
	//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual	void PaintFrame( C2DRender* p2DRender );
};

class CWndQuit : public CWndMessageBox
{
public:
	CWndQuit() { }
	virtual ~CWndQuit() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	
class CWndLogOut : public CWndMessageBox
{
public:
	CWndLogOut() { }
	virtual ~CWndLogOut() { }
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	

#ifdef __NPP_050308

class CWndLogOutWaitting : public CWndNeuz
{
	BOOL  m_bIsLogOut;
	DWORD dwTime;
public:
	CWndLogOutWaitting();
	virtual ~CWndLogOutWaitting();
	void	SetIsLogOut(BOOL bFlag) { m_bIsLogOut = bFlag; }
	virtual BOOL Process ();
	virtual BOOL Initialize( CWndBase* pWndParent );
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
};	

#endif //__NPP_050308

//DECLARE_WNDMESSAGEBOX( CWndLogOut    ) // 로그아웃 처리하는 메시지 박스 클레스 

BEGIN_WNDCLASS( CWndMacro )
END_WNDCLASS

BEGIN_WNDCLASS( CWndStyle )
END_WNDCLASS

class CWndTradeGold : public CWndNeuz 
{ 
public: 
	UINT m_nIdWndTo;
	SHORTCUT m_Shortcut;
	D3DXVECTOR3 m_vPos;
	DWORD m_dwGold;
	CWndBase* m_pWndBase;
	CWndStatic* pStatic;
	CWndStatic* pStaticCount;
	BYTE m_nSlot;
	BYTE m_nPutSlot;
	CWndTradeGold(); 
	~CWndTradeGold(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};

class CWndConfirmTrade : public CWndNeuz 
{ 
	OBJID m_objid;
public: 
	CWndConfirmTrade(); 
	~CWndConfirmTrade(); 

	void OnSetName( const char* szName, OBJID objid );
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndTradeConfirm : public CWndNeuz 
{ 
public: 
	BOOL bMsg;
	CWndTradeConfirm(); 
	~CWndTradeConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

BEGIN_WNDCLASS( CWndTrade )

	//CWndButton    m_wndOkYou      ;
	//CWndButton    m_wndOkI        ;
	//CWndButton    m_wndCancel     ;
	CWndTradeCtrl m_wndItemCtrlYou;
	CWndTradeCtrl m_wndItemCtrlI  ;
	CWndStatic    m_wndGold       ;
	//CWndButton    m_wndGoldClear   ;
	//CWndButton    m_wndGoldMinus  ;
	int           m_nGoldI, m_nGoldYou;

	void DoCancel();
	//void OnDestroyChildWnd( CWndBase* pWndChild );

END_WNDCLASS

//////////

BEGIN_WNDCLASS( CWndEmotion )
	CWndTreeCtrl  m_wndViewCtrl;
	CWndEdit      m_wndEdit;
	CWndButton    m_wndDefault;
	LPTREEELEM    m_lpSelectTreeElem;
END_WNDCLASS


/*
public:
	CWndCharacter();
	virtual ~CWndCharacter();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand(UINT nID,DWORD dwMessage);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};
*/
BEGIN_WNDCLASS( CWndFriend )
END_WNDCLASS

//BEGIN_WNDCLASS( CWndGuild )
//END_WNDCLASS

BEGIN_WNDCLASS( CWndClub )
END_WNDCLASS


BEGIN_WNDCLASS( CWndEMail )
END_WNDCLASS

BEGIN_WNDCLASS( CWndBBS )
END_WNDCLASS

BEGIN_WNDCLASS( CWndSearchPeople )
END_WNDCLASS

BEGIN_WNDCLASS( CWndSearchGuild )
END_WNDCLASS

BEGIN_WNDCLASS( CWndSearchClub )
END_WNDCLASS

BEGIN_WNDCLASS( CWndSearchShop )
END_WNDCLASS


BEGIN_WNDCLASS( CWndInfoEvent )
END_WNDCLASS

BEGIN_WNDCLASS( CWndInfoVersion )
END_WNDCLASS

BEGIN_WNDCLASS( CWndInfoSchedule )
END_WNDCLASS


BEGIN_WNDCLASS( CWndJoinVote )
END_WNDCLASS

BEGIN_WNDCLASS( CWndJoinEvent )
END_WNDCLASS

BEGIN_WNDCLASS( CWndSequence )
END_WNDCLASS


BEGIN_WNDCLASS( CWndCard )

	static CTextureMng m_textureMng;
	static CTexture* m_pTexCardFront;
	static CTexture* m_pTexCardBack ;

	CWndButton m_wndMessage;
	CWndButton m_wndTeleport;
	CModelObject* m_pModel;
	BOOL m_bFront;

	BOOL IsPickupSpace( CPoint ptWindow );
	void OnRButtonUp(UINT nFlags, CPoint point);
	BOOL OnEraseBkgnd(C2DRender* p2DRender);
	void OnLButtonDblClk( UINT nFlags, CPoint point);

END_WNDCLASS

class CWndRevival : public CWndNeuz 
{ 
public: 
	CWndButton* m_pLodeLight;
	CWndButton* m_pLodeStar;
	CWndButton* m_pRevival;
	CWndRevival(); 
	~CWndRevival(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

#ifdef __NPP_050308
class CWndRecoverChao : public CWndNeuz 
{ 
public: 
	int		m_nWhereBorn;   // 0번 : 로드스타, 1번 : 로드라이트

	CWndRecoverChao(); 
	~CWndRecoverChao(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


class CWndReWanted : public CWndNeuz 
{ 
public: 
	CWndReWanted(); 
	~CWndReWanted(); 

private:
	CString      m_strWanted;
	
public: 
	BOOL         CheckWantedInfo( int nGold, LPCTSTR szMsg );
	void		 SetWantedName( LPCTSTR szWanted );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndResurrectionConfirm : public CWndNeuz 
{ 
public: 
	CWndResurrectionConfirm(); 
	~CWndResurrectionConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndWantedConfirm : public CWndNeuz 
{ 
	int		 m_nGold;
	char	 m_szName[ MAX_NAME ];
public: 
	CWndWantedConfirm(); 
	~CWndWantedConfirm(); 
	
	void		 SetInfo( const char szName[], const int nGold );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 


struct WANTEDLIST
{
	char	 szName[ MAX_NAME ];		// 이름.
	__int64	 nGold;						// 현상금
	char     szDate[32];
	char	 szMsg[WANTED_MSG_MAX + 1];	// 십자평 
};

class CWndWanted : public CWndNeuz 
{ 
private: 
	CWndWantedConfirm*  m_pWantedConfirm;
	time_t				m_recvTime;
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	WANTEDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;

public: 
	CWndWanted(); 
	~CWndWanted(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertWanted( const char szName[], __int64 nGold,  int nTime, const char szMsg[] );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 




#endif //__NPP_050308

#ifdef __S050524_SM_ITEM
class CWndCommItemDlg : public CWndNeuz 
{ 
public: 
	CWndEdit*		m_pWndEdit;
	DWORD			m_dwObjId;
	DWORD			m_dwCtrlId;
	void			SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId );
	CWndCommItemDlg(); 
	~CWndCommItemDlg(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangeClass1 : public CWndNeuz 
{ 
public: 
	int			nJob;
	void		SetJob();

	CWndChangeClass1(); 
	~CWndChangeClass1(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangeClass2 : public CWndNeuz 
{ 
public: 
	int			nJob;
	void		SetJob();
	
	CWndChangeClass2(); 
	~CWndChangeClass2(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 
#endif // __S050524_SM_ITEM

#ifdef __Y_POST_0912
class CWndPostSend : public CWndNeuz
{
	int		m_nCost;
	int		m_nCount;
	BYTE    m_nItem;
public:	
	void ClearData();
	CWndPostSend(); 
	~CWndPostSend(); 
	
	BYTE GetItemId() { return m_nItem; }
	void SetItemId( BYTE nId );
	void SetCost( int nCost ) { m_nCost = nCost; }
	void SetCount( int nCount ) { m_nCount = nCount; }
	void SetReceive( char* pchar );
	void SetTitle( char* pchar );	
	void SetText( char* pchar );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};

class CWndPostRead : public CWndNeuz
{
	int					m_nMailIndex;
	CWndGold			m_wndGold;
	
public:	
	void MailReceiveItem();
	void MailReceiveGold();
	void ClearData();
	CWndPostRead(); 
	~CWndPostRead(); 
	
	void SetValue( int nMailIndex );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnMouseMove(UINT nFlags, CPoint point );	
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );		
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
};


class CWndPostReceive : public CWndNeuz
{
	CTexture			m_Texture[3];
	CWndScrollBar		m_wndScrollBar;
	int					m_nSelect;
	int					m_nMax;
//	CMail*				m_aMail[MAX_WANTED_LIST];
	CWndPostRead*		m_wndPostRead;
public:	
	void UpdateScroll();
	
	CWndPostReceive(); 
	~CWndPostReceive(); 

	int          GetSelectIndex( const CPoint& point );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual	void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual HRESULT DeleteDeviceObjects();
};
class CWndPost : public CWndNeuz
{
public:
	CWndPostSend		m_PostTabSend;
	CWndPostReceive		m_PostTabReceive;
	
	CWndPost(); 
	~CWndPost(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
};
class CWndPostSendConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	
	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
public: 
	CWndPostSendConfirm();
	~CWndPostSendConfirm();
	
	void			SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 

class CWndPostDeleteConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
	int		m_nIndex;
public: 
	CWndPostDeleteConfirm();
	~CWndPostDeleteConfirm();
	
	void			SetValue( int nIndex ) { m_nIndex = nIndex; }
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 

#endif //__Y_POST_0912

#ifdef __GUILDCOMBAT
class CWndGuildWarAppConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	CWndGuildWarAppConfirm();
	~CWndGuildWarAppConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 
class CWndGuildWarCancelConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	CWndGuildWarCancelConfirm();
	~CWndGuildWarCancelConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 
class CWndGuildWarJoinConfirm : public CWndNeuz
{
protected:
	CWndText	m_wndText;
public: 
	CWndGuildWarJoinConfirm();
	~CWndGuildWarJoinConfirm();
	
	virtual	BOOL	Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	BOOL	OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void	OnDraw( C2DRender* p2DRender );
	virtual	void	OnInitialUpdate();
	virtual	BOOL	OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual	void	OnSize( UINT nType, int cx, int cy );
	virtual void	OnLButtonUp( UINT nFlags, CPoint point );
	virtual	void	OnLButtonDown( UINT nFlags, CPoint point );
}; 

struct GUILDLIST
{
	char	 szGuild[ MAX_NAME ];		// 이름.
	char	 szName[ MAX_NAME ];		// 이름.
	int		 nNum;
};

class CWndGuildWarState : public CWndNeuz 
{ 
private: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	GUILDLIST			m_aList[MAX_WANTED_LIST];
	int					m_nSelect;
	
public: 
	CWndGuildWarState(); 
	~CWndGuildWarState(); 
	
	void		 InsertTitle( const char szTitle[] );
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], const char szName[], int nNum );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


struct GUILDNAME
{
	char	 szGuild[ MAX_NAME ];		// 이름.
};

class CWndGuildCombatRanking : public CWndNeuz 
{ 
private: 
	CWndScrollBar		m_wndScrollBar;
	int					m_nMax;
	multimap< int, GUILDNAME > m_multimapRanking;
	int					m_nSelect;
	
public: 
	void SortRanking();
	CWndGuildCombatRanking(); 
	~CWndGuildCombatRanking(); 
	
	int          GetSelectIndex( const CPoint& point );	
	void		 Init( time_t lTime );
	void		 InsertGuild( const char szGuild[], int nWinCount );	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual	void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);	
}; 


#endif //__GUILDCOMBAT

#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)

