#pragma  once

// SINCE 2012_02_28
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"


// CWnd 의 기본모델
class CWndWidget3 : public CWndNeuz
{
public:
	CWndWidget3();
	virtual ~CWndWidget3();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL Process( void );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );

	//message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
};

// 각성 재굴림
class CWndReawakening : public CWndNeuz			
{
public:
	enum
	{
		MID_SLOTITEM_NUM = 2,
		MAX_SLOTITEM_NUM = 5,
	};
	CWndReawakening();
	virtual ~CWndReawakening();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL Process( void );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );

	//message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point );
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDown( UINT nFlags, CPoint point );

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void DropedIcon( DWORD dwItemId );
	void Reset( );
	void StartAnimation( );	
	void SetItemId( DWORD dwItemId );

	//received packet
	void ReceivedPacket_Start( const FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck& kData );
	void ReceivedPacket_Sync( );

protected:
	void SetSlotItem( DWORD dwItemId );
	void UpdateRadioButton( );
	void DrawSlotAnimation( C2DRender* p2DRender );
	BOOL CheckItem( DWORD dwItemId );
	void EnableRadioGroup( BOOL bEnable, BOOL bResetCheck = FALSE );

	void Finish( );

	//send packet
	void SendPacket_Start( );		
	void SendPacket_Sync( );

protected:
	DWORD m_dwSlotItem;
	DWORD m_dwMaterialItem;
	BOOL m_bRPushed;			//같은 영역에서 push 와 up이 일어났는가의 체크 flag
	BOOL m_bAnimationEnd;
	BOOL m_bStopForGoal;
	float m_fOffset;
	float m_fKeepingTime;
	float m_fKeepingTime2;
	int m_nGoalCount;
	CString m_strSelectedOption;

	FLSnapshotItemUpgradeRandomOptionGenerateRetryResultAck m_kSavedPacket;

	struct DATA
	{
		T_RANDOMOPTION_EXT _kData;
		CString _strOption;

		DATA( const T_RANDOMOPTION_EXT& kData );
	};

	std::deque< DATA > m_queVirtualSlotData;		// 실제데이터가 아닌 항상 갱신되는 가상 데이터
};

//극단 성장의 두루마리
class CWndPartyUpgradeScroll : public CWndNeuz
{
public:
	CWndPartyUpgradeScroll();
	virtual ~CWndPartyUpgradeScroll();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL Process( void );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );

	//message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

protected:
	void SendPacket( const CString& strNewName );
};