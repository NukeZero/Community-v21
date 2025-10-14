#ifndef __WNDDIALOG
#define __WNDDIALOG

#include "DaisyVer.h"

#ifdef __DAISY_UPDATE_1
#define MAX_CLIPED_WINDOWS	10
#endif //__DAISY_UPDATE_1

class CWndManager : public CWndBase
{
	CRect m_rectOld;
	CPoint m_pointOld;
	int m_nResizeDir;
	int GetResizeDir( CRect rect,CPoint ptClient );
	
public:
	BOOL m_bShowGrid;
	static CWndBase* m_pWndFocusDialog;

#ifdef __DAISY_UPDATE_1
	CWndBase* m_pClipedWnd[MAX_CLIPED_WINDOWS];
	int m_nCountClip;
#endif //__DAISY_UPDATE_1
	
	CWndManager(); 
	virtual ~CWndManager();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void PaintRoot( C2DRender* p2DRender );
	virtual void OnDestroyChildWnd( CWndBase* pWndChild );
	virtual	BOOL Process();
#ifdef __DAISY_UPDATE_1
	//Cliped Window
	virtual LRESULT WindowRootProc( UINT message, WPARAM wParam, LPARAM lParam );
	void InitializeClipedWnd();
	void ClipWindow(CWndBase* pWndBase);
	void RemoveClipedWnd(CWndBase* pWndBase);
	BOOL IsClipedWindow(CWndBase* pWndBase);
#ifdef __DAISY_UPDATE_3
	void UpdateClipedWindow(LPWNDCTRL lpWndCtrl, int nCount);
#endif //__DAISY_UPDATE_3
	//Edit Window - Align, Size control
	void AlignClipWndLeft();
	void AlignClipWndRight();
	void AlignClipWndTop();
	void AlignClipWndBottom();
	void MakeClipWndSameWidht();
	void MakeClipWndSameHeight();
	void MakeClipWndSameSize();
#endif //__DAISY_UPDATE_1
	
};

class CWndDialog : public CWndNeuz
{
	/*
	CWndText m_wndText;
	CWndButton m_wndMusic;
	CWndButton m_wndSound;
	CWndComboBox m_wndComboBox;
	CWndListCtrl m_wndlistCtrl;
	CWndButton   m_wndButtonIcon;
	CWndButton   m_wndButtonReport;
	CWndButton   m_wndButtonList;
	*/
	CRect m_rectOld;
	CPoint m_pointOld;
	int m_nResizeDir;
	int GetResizeDir( CRect rect,CPoint ptClient );
public:

	CWndDialog(); 
	virtual ~CWndDialog();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual BOOL OnChildNotify( UINT nCode, UINT nID, LRESULT* pLResult );
	BOOL IsPickupSpace(CPoint ptWindow);

	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	LPWNDAPPLET m_lpWndApplet;
};
#endif