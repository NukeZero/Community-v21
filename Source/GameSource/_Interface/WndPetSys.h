//sun: 9차 전승관련 Clienet

#ifndef __WNDPETSYSTEM__H
#define __WNDPETSYSTEM__H

/*******************************
	펫 시스템 관련 Window
********************************/
struct PETCAMTABLE
{
	D3DXVECTOR3 CamPos;
	D3DXVECTOR3 CamLook;
	float Scale;
};

#define MAX_PET_CLASS 7

//sun: 12, 펫 각성
class CWndPetAwakCancel : public CWndNeuz 
{ 
public: 	
	CWndText* m_pText;
	FLItemElem* m_pItemElem;
	PT_ITEM_SPEC m_pEItemProp;
	CTexture* m_pTexture;

	CWndPetAwakCancel(); 
	~CWndPetAwakCancel(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDestroy();
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );

	void SetDescription();
}; 


//---------------------------------------------------------------------------------------------
// 펫 상태 윈도우
//---------------------------------------------------------------------------------------------
class CWndPetStatus : public CWndNeuz
{

public:


								CWndPetStatus(); 
	virtual						~CWndPetStatus();

	virtual BOOL				Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);

	virtual	void				OnInitialUpdate( void );
	virtual BOOL 				OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual BOOL 				OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void 				SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void 				OnLButtonDown( UINT nFlags, CPoint point );
	virtual void 				OnLButtonUp( UINT nFlags, CPoint point );
	virtual BOOL 				OnEraseBkgnd( C2DRender* p2DRender );
	virtual void 				OnMouseWndSurface( CPoint point );
	virtual	void 				PaintFrame( C2DRender* p2DRender );
	virtual void 				OnSize( UINT nType, int cx, int cy );
	virtual void				OnDraw( C2DRender* p2DRender );
	virtual BOOL 				Process( void );

	void						CalculationWindowPosition( void );
	void 						LockShowLevel( BOOL lFlag, int nLevel, int nPos );
	void 						SetExtension( BOOL eflag );
	void						MakeGaugeVertex( void );
	void 						SetPetCamTable( void );

	virtual HRESULT 			InvalidateDeviceObjects( void );
	virtual HRESULT 			RestoreDeviceObjects( void );
	virtual HRESULT 			DeleteDeviceObjects( void );


protected:


	void 						DrawPetInformation( C2DRender* p2DRender );
	void						DrawGauge( C2DRender* p2DRender );


public:


	LPDIRECT3DVERTEXBUFFER9 	m_pVBHPGauge;
	LPDIRECT3DVERTEXBUFFER9 	m_pVBEXPGauge;

	CModelObject*				m_pPetModel; 
	FLItemElem*					m_pPetElem;

	BOOL 						m_bVBHPGauge;
	BOOL 						m_bVBEXPGauge;

	BOOL						m_bHPVisible;
	BOOL						m_bExpVisible;

	BOOL						m_bExtension;

	int 						m_nHPWidth;
	int 						m_nEXPWidth;

	CTexture*					m_pTexPetLvBg;
	CTexture*					m_pTexPetStatusBg;

	DWORD						m_dwPropertyNameColor;
	DWORD						m_dwPropertyNameShadowColor;
	DWORD						m_dwPropertyValueColor;
	DWORD						m_dwGaugeValueColor;

	int							m_nDisplay;
	int							m_nChoiceLevel;

	CTexture 					m_texGaugeHP;
	CTexture 					m_texGaugeExp;

	CTexture* 					m_pGaugeHP;
	CTexture* 					m_pGaugeExp;
	CTexture* 					m_pGaugeBg;

	PETCAMTABLE					m_PetCameTable[MAX_PET_CLASS];

	int							m_aPetLv[6];
	CTexture*					m_pTexPetLv[6];
	
	int							m_nLockLevel[2];
	BYTE						m_nBackUpLevel[2];
	CString						m_strPathLvImage[9];



};


class CWndConfirmVis : public CWndNeuz 
{ 
public: 
	enum ConfirmVisSection
	{
		CVS_EQUIP_VIS,				// 비스착용 
		CVS_UNEQUIP_VIS,			// 비스 착용해제  
		CVS_EQUIP_VISKEY,			// 비스키 사용 
		CVS_PICKUP_TO_BUFF,			// 버프펫으로 변환

		ETC_PROTECT_AWAKE,				// 각성 보호 
		ETC_GAMEGUARD_ERR,

		ETC_BARUNASMELT_NOTICE,		// 바루나 제련 +2부터 ( +3이 깨질수있음 ) 경고메세지
		ETC_CASHITEM,				// 캐시 아이템 확인
		ETC_BARUNAELEMENT_SMELT_NOTICE,// 바루나 속성제련 +3부터 ( +4이 깨질수있음 ) 경고메세지
	};
	CWndConfirmVis(); 
	virtual ~CWndConfirmVis(); 

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 

	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

	void SendEquipPacket( );

 	DWORD m_dwItemId;
 	OBJID m_objid;
 	int m_nSlot;
	DWORD m_dwItemIndex;

	CString m_strTitleString;

	//hm.. need refactoring
	ConfirmVisSection m_eSection;
};



//---------------------------------------------------------------------------------------------
// 버프 팻 상태 윈도우
//---------------------------------------------------------------------------------------------
class CWndBuffPetStatus : public CWndNeuz
{

public:


						CWndBuffPetStatus( void );
	virtual				~CWndBuffPetStatus( void );
	
	virtual BOOL		Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);

	virtual HRESULT 	RestoreDeviceObjects( void );
	virtual HRESULT 	InvalidateDeviceObjects( void );
	virtual HRESULT 	DeleteDeviceObjects( void );

	virtual	void		OnInitialUpdate( void );
	virtual void		OnDestroy( void );
	virtual BOOL		Process( void );
	virtual void		OnDraw(C2DRender* p2DRender);
	
	virtual BOOL		OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void		OnLButtonDown( UINT nFlags, CPoint point );
	virtual void		OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void		OnMouseWndSurface( CPoint point );
	
	void				UpdateVisState( void );	
	BOOL				DoModal_ConfirmQuestion( DWORD dwItemId, OBJID objid, DWORD dwIndex, int nSlot = 0,  CWndConfirmVis::ConfirmVisSection eSection = CWndConfirmVis::CVS_EQUIP_VIS );			//OK => SendDoUseItem

	BOOL				IsVisItem( DWORD index );
	BOOL				IsFreeSlot( const int index );

	FLItemElem*			GetItemElemByIndex( const int index );
	int					GetSlotIndexByPoint( const CPoint& point );


private:


	void				DrawSlotItems( C2DRender* p2DRender );	
	void				DrawBackGround( C2DRender* p2DRender );
	void				DrawPetModelObject( C2DRender* p2DRender );

	void				SetRenderState( C2DRender* p2DRender );
	void				SetViewport( C2DRender* p2DRender, CRect rtRenderRect );
	void				SetProjection( C2DRender* p2DRender, CModelObject* pModel );
	void				RestoreRenderState( C2DRender* p2DRender );



private:


	CModelObject*		m_pPetModel; 
	CWndConfirmVis*		m_pWndConfirmVis;

	CTexture*			m_pTexPetStatusBg;

	float				m_fRadius;

	FLItemElem*			m_pItemElem[MAX_VIS];
	CTexture*			m_pTexture[MAX_VIS];
	int					m_nCtrlId[MAX_VIS];

	std::vector<BYTE>		m_cVisStates;

};
//---------------------------------------------------------------------------------------------


class CWndFoodConfirm : public CWndNeuz 
{ 
public: 
	FLItemElem* m_pItemElem;
	CWndEdit* m_pEdit;
	int m_nParent;
	
	CWndFoodConfirm();
	CWndFoodConfirm(int nParent);
	virtual ~CWndFoodConfirm(); 
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(DWORD dwObjId);
};

class CWndPetMiracle : public CWndNeuz 
{
public:
	int m_nMiracleLv[2];
	int m_nCount[2];
	int m_nDelay[2];			//회전시키는 Delay값
	int m_nStatus[2];			//현재의 상태 (Delay관련 적용 위해 사용)
	int m_nPreLvCount;
	int m_nCurLvCount;
	int m_nResPreLevel;
	int m_nResCurLevel;
	int m_nPetLevel;		//펫 레벨
	
	BOOL m_bReciveResult[2];	//서버로 부터 결과를 받았는지 확인
	BOOL m_bEnd;				//회전이 끝난 시점
	BOOL m_bLocked[2];			//상태창에 레벨고정을 하였었는지.

	DWORD m_dwObjId;
	
	CString m_strPetLevel[2];
	CString m_strPathLvImage[9];
	
	CWndText* m_pText;
	
public: 
	CWndPetMiracle(); 
	virtual ~CWndPetMiracle(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL Process ();
	
	void ReceiveResult(int nPreLevel, int nCurLevel);		//서버로 부터 결과 값을 받음
	void PreLevelImgProcess();
	void CurLevelImgProcess();
	void SetDescription(CHAR* szChar);
	void SetItem(DWORD dwObjId);
}; 

class CWndPetFoodMill : public CWndNeuz
{
public:
	OBJID m_dwObjId;		//상용화 아이템 objid
	FLItemElem* m_pItemElem;	//분쇄 되는 아이템
	CTexture*	m_pTexture;

	CWndFoodConfirm* m_pWndFoodConfrim;
	int m_nItemCount;
public:
	CWndPetFoodMill();
	virtual ~CWndPetFoodMill(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	void SetItem(DWORD dwObjId);
	void SetItemForFeed(FLItemElem* pItemElem, int nCount);
	void ReceiveResult(int nResult, int nCount);
};

class CWndPetLifeConfirm : public CWndNeuz 
{	
public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
	int m_nId;
	
protected:
	CString m_strText;
	CWndText m_wndText;
	
public:
	CWndPetLifeConfirm();
	~CWndPetLifeConfirm();
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	void SetItem(int nId);
}; 

//sun: 12, 펫 알 변환 기능 추가
#define MAX_TRANS_EGG 10

class CWndPetTransEggs : public CWndNeuz 
{
public:
	FLItemElem*	m_pItemElem[MAX_TRANS_EGG];
	int			m_nCtrlId[MAX_TRANS_EGG];
	CWndText*	m_pText;
	CTexture*	m_pEggTexture;
	
public: 
	CWndPetTransEggs(); 
	virtual ~CWndPetTransEggs(); 
	
	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnMouseWndSurface(CPoint point);

	void SetDescription(CHAR* szChar);
	void SetItem(FLItemElem* pItemElem);

private:
	void ResetEgg();
	void CheckFull();
}; 

#endif //__WNDPETSYSTEM__H
