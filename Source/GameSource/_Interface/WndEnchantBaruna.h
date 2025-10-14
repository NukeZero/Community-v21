
//Date : 20100727
//Author : gmpbigsun
//Note : all about BARUNA ( new enchant )

#pragma  once 

#ifdef __ENCHANT_BARUNA16

#define _EBARUNA_SAFE_LEVEL 2
#define MAX_BARUNA_ENCHANT_LEVEL 20

//---------------------------------------------------------------------------------------------------------
class CWndEnchantBaruna : public CWndNeuz
{
	//바루나 제련 
public:
	CWndEnchantBaruna( );
	virtual ~CWndEnchantBaruna( );

	virtual void OnDestroy();

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual BOOL Process ();

	BOOL SetItem(FLItemElem* pItemElem);

	BOOL SendPacket( );
//#ifdef BARUNA_ULTIMATE_UPDATE
//	BOOL SendUltimatePacket();
//#endif
	void ReceivedPacket( );

protected:
	void DoModalConfirm( );
	void InitCtrl();

protected:
	enum {  N_CHASHITEM = 0,
			N_OFFERSEED,
			N_ITEM,
            N_MAX,};	
	CRect	m_rectSlot_EnchantBaruna[N_MAX];		//캐시아이템, 오퍼시드, 아이템
	FLItemElem* m_pItem[N_MAX];

	BOOL m_bSendPacket;
	DWORD m_dwDeltaTime;
};
#ifdef BARUNA_ULTIMATE_UPDATE
//---------------------------------------------------------------------------------------------------------
class CWndBarunaUltimateTrans : public CWndNeuz
{
	//바루나 제련 
public:
	CWndBarunaUltimateTrans( );
	virtual ~CWndBarunaUltimateTrans( );

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual BOOL Process ();

	BOOL SendPacket( );

	void ReceivedPacket( );

protected:
	void DoModalConfirm( );
	void InitCtrl();

protected:
	enum {  N_OFFERSEED = 0,
			N_ITEM,
			N_JEWELRY,
            N_MAX,};	
	CRect	m_rectSlot_Baruna[N_MAX];		//캐시아이템, 오퍼시드, 아이템
	OBJID	m_objID_Baruna[N_MAX];

	FLItemElem* m_pItem[N_MAX];

	BOOL m_bSendPacket;
	DWORD m_dwDeltaTime;
};
#endif

//---------------------------------------------------------------------------------------------------------
class CWndProduceOfferSeed : public CWndNeuz
{
	//오퍼 시드 제작
public:
	CWndProduceOfferSeed( );
	virtual ~CWndProduceOfferSeed( );

	virtual void OnDestroy();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL Process ();

	void ReceivedPacket( );

	BOOL SetItem(FLItemElem* pItem);

protected:
	BOOL SendPacket( );

protected:
	enum {  N_OFFER = 0,
			N_CID,
			N_MEAL,
			N_MAX,};	
	CRect m_rectSlot[N_MAX];	//오퍼, 시드, 바륨의 가루
	//OBJID m_objID[N_MAX];

	FLItemElem* m_pItem[N_MAX];
	BOOL m_bSendPacket;
};

//---------------------------------------------------------------------------------------------------------
class CWndProduceOffer : public CWndNeuz
{
	//오퍼 제작
public:
	CWndProduceOffer( );
	virtual ~CWndProduceOffer( );

	virtual void OnDestroy();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL Process ();

	void ReceivedPacket( );

	BOOL SetItem(FLItemElem* pItem);

protected:
	BOOL SendPacket( );

protected:
    enum {  N_OFFER = 0,
		    N_OFFERMIX,
			N_MAX,};	
	CRect m_rectSlot[N_MAX];

	FLItemElem* m_pItem[N_MAX];
	BOOL m_bSendPacket;
};

//---------------------------------------------------------------------------------------------------------
class CWndProduceSeed : public CWndNeuz
{
	//시드 제작 및 업그레이드
	//시드레벨이 2등급 이하일경우는 업그레이드다.
public:
	CWndProduceSeed( );
	virtual ~CWndProduceSeed( );

	virtual void OnDestroy();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL Process ();

	void ReceivedPacket( );

	BOOL SetItem(FLItemElem* pItem);

protected:
	BOOL SendPacket( );

protected:
	enum { N_CID = 0,
		   N_CIDMIX,
		   N_MAX,};	
	CRect m_rectSlot[N_MAX];
	//OBJID m_objID[N_MAX];

	FLItemElem* m_pItem[N_MAX];
	BOOL m_bSendPacket;
};

//---------------------------------------------------------------------------------------------------------
class CWndExtractOffer : public CWndNeuz
{
	//오퍼 추출
public:
	CWndExtractOffer( );
	virtual ~CWndExtractOffer( );

	virtual void OnDestroy();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );

	virtual BOOL Process ();

	BOOL SetItem(FLItemElem* pItemElem);

	void ReceivedPacket( );	

protected:
	BOOL SendPacket( );

protected:
	enum { N_EXTOFFER = 0,
		   N_MAX,};	
	CRect m_rectSlot[N_MAX];
	//OBJID m_objID[N_MAX];
	
	FLItemElem* m_pItem[N_MAX];
	BOOL m_bSendPacket;
};

//---------------------------------------------------------------------------------------------------------
class FLWndBarunaPearcing : public CWndNeuz
{
public:
	FLWndBarunaPearcing();
	virtual ~FLWndBarunaPearcing();

	virtual void OnDestroy();
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL Process		( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase );

	BOOL SendPacket( );
	void ReceivedPacket( const DWORD dwResult );

	BOOL SetItem(FLItemElem* pItem);

private:
	enum { N_PEARCINGITEM = 0,		// 피어싱할 아이템
		   N_RUNE,					// 고대 문자석
		   N_PROTECT,				// 문자석 보호 아이템
		   N_ITEM,					// 성공확률 증가 아이템
		   N_MAX = 4,};	
	CRect m_rectSlot_Pearcing[N_MAX];	//item,고대문자석,문자석보호아이템,성공확률증가아이템
	//OBJID m_objID_Pearcing[N_MAX];

	FLItemElem* m_pItem[N_MAX];
	BOOL m_bSendPacket;

	void StartPearcing();
};

//---------------------------------------------------------------------------------------------------------
class CFLWndBaruna_Wakeup: public CWndNeuz
{
public:
	CFLWndBaruna_Wakeup();
	virtual ~CFLWndBaruna_Wakeup();
	virtual void OnDestroy();

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL Process		( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	void	ReceivedPacket( );	

	BOOL SetItem(FLItemElem* pItem);

private:

	CRect m_rectSlot_BrunaITem;	//각성할 아이템 
	//OBJID m_objID_BrunaITem[N_MAX];

	FLItemElem* m_pItem;

	//필요페냐
	CWndStatic* m_pWndBarunaPenya;
	CString m_strPenya;

    //게이지바 관련 
	CTexture* m_pTexGuage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	int m_nCurrGauage;
	HRESULT RestoreDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	BOOL	m_bGauageBarState;
	BOOL	m_bSend;
	
	void	InitCtrl();
	void	SetWakeup(BOOL bSend);
};
//---------------------------------------------------------------------------------------------------------
class CFLWndBaruna_Wakeup_Cancel: public CWndNeuz
{
public:
	CFLWndBaruna_Wakeup_Cancel();
	virtual ~CFLWndBaruna_Wakeup_Cancel();
	virtual void OnDestroy();

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL Process		( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	void	ReceivedPacket( );	

	BOOL SetItem(FLItemElem* pItem);

private:
	enum { N_WAKEUP_CANCEL = 0,//각성취소할 아이템 
			N_OBLIVION = 1,	//망각제
			N_MAX,};	
	CRect m_rectSlot_BrunaITem_Cancel[N_MAX];	
	//OBJID m_objID_BrunaITem_Cancel[N_MAX];

	FLItemElem* m_pItem[N_MAX];

	// 게이지바 관련 
	HRESULT RestoreDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	int m_nCurrGauage;
	CTexture* m_pTexGuage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;

	BOOL	m_bGauageBarState;
	BOOL	m_bSend;
	void	InitCtrl();
	void	SendWakeupCancel(BOOL bSend);
};
//---------------------------------------------------------------------------------------------------------
class CFLWndBaruna_Element_Smelt: public CWndNeuz
{
public:
	CFLWndBaruna_Element_Smelt();
	virtual ~CFLWndBaruna_Element_Smelt();
	virtual void OnDestroy();

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL Process		( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );
	virtual BOOL OnCommand		( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

	void	ReceivedPacket( );	
	void	SendPacket(BOOL bSend);
	void	SetGauageBarState(BOOL bState){m_bGauageBarState = bState;}
	BOOL	GetGauageBarState(){return m_bGauageBarState;}

	BOOL SetItem(FLItemElem* pItem);

private:
	enum {  N_BARUNA_ITEM = 0,	//바루나 장비
			N_ORB,			//속성오브
			N_GRACE,		//바루나의 은총
			N_REJU,			//원소 활력제
			N_MAX,};

	HRESULT RestoreDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	CTexture* m_pTexGuage;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge;
	int		m_nCurrGauage;
	BOOL	m_bGauageBarState;
	BOOL	m_bSend;
	
	OBJID	m_objID_Confirm;
	CRect	m_rect_Element_Smelt[N_MAX];
	//OBJID	m_objID_Element_Smelt[N_MAX];

	FLItemElem* m_pItem[N_MAX];

	BOOL m_bSendPacket;
	
	void	StartSmelt();
	void	InitCtrl();
	void	Element_Smelt_Check_Orb(FLItemElem* pItemBaruna);
	void	ItemRegister(int nSlotID, FLItemElem* pItem);
	void	ItemConfirmRegister(DWORD dwItemID);
};
//---------------------------------------------------------------------------------------------------------

class CWndBarunaNPC : public CWndNeuz
{
//제련관련 서브 윈도우를 모두 관리한다.
public:
	enum BARUNA_WNDS
	{
		BW_BARUNA,	//바루나 제련
		BW_OPERCID, //오퍼시드
		BW_OPER,	//오퍼
		BW_CID,		//시드
		BW_EXTRACT_OPER, //오퍼추출
		BW_BARUNA_PEARCING, //바루나 피어싱
		BW_BARUNA_WAKEUP,		//바루나 각성
		BW_BARUNA_WAKEUP_CANCEL,//바루나 각성 취소
		BW_BARUNA_ELEMENT_SMELT,//바루나 속성 제련
#ifdef BARUNA_ULTIMATE_UPDATE
		BW_BARUNA_ULTIMATE_UPDATETRANS
#endif	// BARUNA_ULTIMATE_UPDATE
	};
	CWndBarunaNPC( );
	virtual ~CWndBarunaNPC( );

	virtual BOOL Initialize		( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw			( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate( );
	virtual BOOL Process		( );
	virtual BOOL OnDropIcon		( LPSHORTCUT pShortcut, CPoint point );

		
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void		MakeGaugeRect	( );

	void		SetNPCInfo		( const OBJID id, DWORD level, DWORD exp );
	CWndNeuz*	CreateWnd		( BARUNA_WNDS eWnd );
	void		ReceivedPacket	( BARUNA_WNDS eWnd, const DWORD dwResult = FSC_ITEMUPGRADE_SYSTEM_ERROR );

	OBJID		GetNpcID( )		{ return m_idNPC; }

protected:
	OBJID	m_idNPC;
	DWORD   m_dwLevel;
	DWORD   m_dwExpRatio;	//exp 백분율, 0~100
	BOOL	m_bWait;		//객체가 윈도우보다 나중에 생성되므로, 기다렸다 갱신한다.
	
	CTexture m_texGauFillNormal  ;
	CRect	m_rectEXP;
	int		m_nWidth;
	LPDIRECT3DVERTEXBUFFER9 m_pGaugeEXP;

	//child wnds
	CWndEnchantBaruna*				m_pEnchantBaruna;
	CWndProduceOfferSeed*			m_pOperCid;
	CWndProduceOffer*				m_pOper;
	CWndProduceSeed*				m_pCid;
	CWndExtractOffer*				m_pExtractOper;
	FLWndBarunaPearcing*			m_pPearcing;				//바루나 피어싱
	CFLWndBaruna_Wakeup*			m_pWakeup;					//바루나 각성
	CFLWndBaruna_Wakeup_Cancel*		m_pWakeup_Cancel;			//바루나 각성 취소
	CFLWndBaruna_Element_Smelt*		m_pElement_Smelt;			//바루나 속성 제련 
#ifdef BARUNA_ULTIMATE_UPDATE
	CWndBarunaUltimateTrans*				m_pUltimateTrans;			//바루나 얼터멋 변환
#endif
};


#endif //__ENCHANT_BARUNA16