
#pragma once 

namespace WndInventoryWear
{
	enum eWearType
	{
		eWearType_Normal = 0,
		eWearType_Costume,

		eWearType_Max,
	};
}

using namespace WndInventoryWear;

//---------------------------------------------------------------------------------------------
// 인벤토리 아이템 장착 윈도우
// # define MAX_HUMAN_PARTS : ProjectCmn.h에 정의되어 있음
//---------------------------------------------------------------------------------------------
class CWndInventoryWear		:	public CWndNeuz
{

private:

	eWearType									m_eCurrentWearType;
		
	CWndButton*									m_pButtonWearType[ eWearType_Max ];
	CTexture*									m_pTexWearTypeBackground[ eWearType_Max ];

	CRect										m_rtWearRect[MAX_HUMAN_PARTS];



public:


												CWndInventoryWear( void );
	virtual										~CWndInventoryWear( void );

	virtual BOOL 								Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);

	virtual BOOL								Process( void );
	virtual	void 								OnInitialUpdate( void );
	virtual void 								OnDraw( C2DRender* p2DRender );
	virtual BOOL								OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual BOOL								OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void								OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual BOOL								OnDropIcon( LPSHORTCUT pShortcut, CPoint point );


private:

	CRect										GetWndCtrlRect( DWORD dwWndId );
	void										SetInventorySlotRectInfo( void );

	void										LoadBackgroundTexture( void );
	void										SetBackgroundTextureFromWearType( void );

	void										SetSelectedButtonFromWearType( void );

	bool										IsDrawParts( UINT nPartsIndex );
	void										CheckTwoHandsWeapon( const UINT& nIndex, FLItemBase*& pItemWear, DWORD& dwRenderAlpha );
	void										DrawItemCount( const C2DRender* p2DRender, const UINT& nIndex, FLItemElem* pItemWearElem );
	void										DrawItemToolTip( void );
	void										DrawEmptySlotToolTip( UINT nPartsIndex, CPoint ptConvert, CRect rtConvert );

	bool										IsValidShortCutInfo( LPSHORTCUT pShortcut );
	bool										IsOpenTradeWindows( void );
	bool										IsOpenRepairWindows( void );


};