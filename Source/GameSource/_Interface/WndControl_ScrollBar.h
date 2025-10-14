
#pragma once

#include "WndBase.h"
#include "WndControl_Button.h"

#define VSCROLL_WIDTH	16
#define GETRANGE		( ( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight() )

//---------------------------------------------------------------------------------------------
// 윈도우 스크롤 바 클레스 
//---------------------------------------------------------------------------------------------
class CWndScrollBar : public CWndBase
{
protected:
	int        m_nScrollPos;
	int        m_nScrollMinPos;
	int        m_nScrollMaxPos;
	int        m_nScrollBarIdx;
	int        m_nScrollPage;
	int        m_nPos;
	BOOL       m_bPushPad; 
	CRect      m_rectStick;
	CWndButton m_wndArrow1;
	CWndButton m_wndArrow2;

public:
	CTexture* m_pTexButtVScrUp   ;
	CTexture* m_pTexButtVScrDown ;
	CTexture* m_pTexButtVScrBar  ;
	CTexture* m_pTexButtVScrPUp  ;
	CTexture* m_pTexButtVScrPDown;
	CTexture* m_pTexButtVScrPBar ;

	CWndScrollBar();
	~CWndScrollBar();

	BOOL Create(DWORD dwScrollStyle,const RECT& rect,CWndBase* pParentWnd,UINT nID);//,CSprPack* pSprPack,int nSprIdx);

	int  GetMinScrollPos();
	int  GetMaxScrollPos();

	void SetMinScrollPos();
	void SetMaxScrollPos();
	int  GetScrollPos() const; // Retrieves the current position of a scroll box. 
	int  SetScrollPos(int nPos, BOOL bRedraw = TRUE); // Sets the current position of a scroll box. 
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const; // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar. 
	int  GetScrollRange() const;
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE); // Sets minimum and maximum position values for the given scroll bar. 
	void SetScrollPage(int nPage);
	int  GetScrollPage() const;
	int  GetScrollAbiliableRange();
	int  GetScrollAbiliablePos();
	int  GetScrollAbiliablePage(); 
	void ShowScrollBar(BOOL bShow = TRUE); // Shows or hides a scroll bar. 
	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH); // Enables or disables one or both arrows of a scroll bar. 
	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE); // Sets information about the scroll bar. 
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL); // Retrieves information about the scroll bar. 
	int  GetScrollLimit(); // Retrieves the limit of the scroll bar
	void SetScrollDown();
	SIZE GetArrowUpButtomTextureSize( void );
	SIZE GetArrowDownButtomTextureSize( void );



	virtual void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual	void PaintFrame(C2DRender* p2DRender);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
};
