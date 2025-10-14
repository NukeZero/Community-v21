
#pragma once 


namespace WndMadrigalGift
{
	enum eGiftStep	: unsigned char
	{
		eStep_None = 0,
		eStep_1st,
		eStep_2nd,
		eStep_3rd,

		eStep_Max,
	};
}

using namespace WndMadrigalGift;


//---------------------------------------------------------------------------------------------
// 마드리갈의 선물
//---------------------------------------------------------------------------------------------
class CWndMadrigalGift		:	public CWndNeuz
{

public:


							CWndMadrigalGift();
	virtual					~CWndMadrigalGift();

	virtual BOOL 			Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);

	virtual BOOL			Process( void );
	virtual	void 			OnInitialUpdate( void );
	virtual void 			OnDraw( C2DRender* p2DRender );
	virtual BOOL			IsPickupSpace( CPoint point );
	virtual BOOL			OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 

	bool					IsGiftBoxEnable( void );
	void					SetGiftBoxButtonEnable( bool bEnable );

	void					SetStep( eGiftStep eStep )					{	m_eStep = eStep;						}
	void					SetPointPercent( BYTE byPointPercent )		{	m_byPointPercent = byPointPercent;		}					
	void					SetLevelForCompensation( WORD wlevel )		{	m_wLevelForCompensation = wlevel;		}
	void					SetSendPacket( bool bSend )					{	m_bSendPacket = bSend;					}			

private:

	
	void					SetGiftBoxTexture( void );
	DWORD					GetRoundGaugeColor( void );

	CEditString				GetToolTipText( void );
	CString					GetCurrentLevelForCompensationToolTipText( void );
	CString					GetNextLevelForCompensationToolTipText( void );

	bool					DrawRoundArrow( C2DRender* p2DRender );
	bool					DrawTooltip( C2DRender* p2DRender );


private:


	CWndButton*				m_pWndButtonGiftBox;

	LPWNDCTRL				m_pGiftBox;
	LPWNDCTRL				m_pRoundGauge;
	LPWNDCTRL				m_pRoundArrow;

	CRect					m_rtGiftBox;
	CRect					m_rtRoundGauge;
	CRect					m_rtRoundArrow;

	CPoint					m_ptRoundGaugeCenter;
	WORD					m_wRoundGaugeRadius;

	eGiftStep				m_eStep;					// 현재 단계
	BYTE					m_byPointPercent;			// 현재 포인트 퍼센트
	WORD					m_wLevelForCompensation;	// 보상을 위한 측정레벨
	
	CTexture*				m_pTexRoundArrow;
	CTexture*				m_pTexGiftBox[eStep_Max];	// 0번 택스쳐는 사용하지 않습니다.

	DWORD					m_dwRoundGaugeColor;
	bool					m_bSendPacket;
};