
#pragma once 

//---------------------------------------------------------------------------------------------
// 범용 개수 입력 메시지 박스
// 설명 :	APP_TRADE_GOLD 애플릿을 기본으로 만들어졌습니다. 새로 만들어서 진행하기에 Resource 낭비 및
//			작업시간을 단축시키기 위해서 입니다. 이해해주세요...
//---------------------------------------------------------------------------------------------
class CWndCommonInputCount	:	public CWndNeuz
{

private:


	CWndEdit*									m_pWndEditCount;

	BYTE										m_bySlot;		// 은행에서 빼온 슬롯 번호 저장 용도
	int											m_nCount;		// 입력 된 개수										

	SHORTCUT									m_stShortcut;

public:


												CWndCommonInputCount( void );
	virtual										~CWndCommonInputCount( void );

	virtual BOOL 								Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);

	virtual	void 								OnInitialUpdate( void );
	virtual BOOL								OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 

	void										SetInputCountInfo( const SHORTCUT* const pShortcut, const BYTE bySlot = 0 );


private:


	bool										IsValidShortCutInfo( void );

	BOOL										ProcessPrivateBank( void );
	bool										ProcessChildNotifyFromPrivateBank( void );										

	BOOL										ProcessGuildBank( void );
	bool										ProcessChildNotifyFromGuildBank( void );										

	BOOL										ProcessMobileBag( void );
	bool										ProcessChildNotifyFromMobileBag( void );										


};