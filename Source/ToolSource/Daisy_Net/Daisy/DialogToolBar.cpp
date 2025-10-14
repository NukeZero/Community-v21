// DialogToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "daisy.h"
#include "DialogToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogToolBar

CDialogToolBar::CDialogToolBar()
{
}

CDialogToolBar::~CDialogToolBar()
{
}


BEGIN_MESSAGE_MAP(CDialogToolBar, CToolBar)
	//{{AFX_MSG_MAP(CDialogToolBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogToolBar message handlers

int CDialogToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
/*
!m_DialogToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_DialogToolBar.LoadToolBar(IDR_EDIT_DIALOG))
*/
BOOL CDialogToolBar::Create(CWnd* pParentWnd)
{
	// start out with no borders
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_SIZE_FIXED | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_GRIPPER;
	
	if(!CToolBar::CreateEx(pParentWnd, TBSTYLE_FLAT, dwStyle)) //, IDW_EDIT_FIELD_BAR))
	{
		return FALSE;
	}
	if(!LoadToolBar(IDR_EDIT_DIALOG))
		return FALSE;

	for( int i = 0; i < 26; i++ )
	{
		if( i % 2 )
			SetButtonStyle(i ,GetButtonStyle(i)|TBBS_WRAPPED);

	}
	/*
// 0~25
	//SetButtonStyle(0 ,GetButtonStyle(0 )|TBBS_WRAPPED);
	SetButtonStyle(1 ,GetButtonStyle(1 )|TBBS_WRAPPED);
	//SetButtonStyle(2 ,GetButtonStyle(2 )|TBBS_WRAPPED);
	SetButtonStyle(3 ,GetButtonStyle(3 )|TBBS_WRAPPED);
	//SetButtonStyle(4 ,GetButtonStyle(4 )|TBBS_WRAPPED);
	SetButtonStyle(5 ,GetButtonStyle(5 )|TBBS_WRAPPED);
	//SetButtonStyle(6 ,GetButtonStyle(6 )|TBBS_WRAPPED);
	SetButtonStyle(7 ,GetButtonStyle(7 )|TBBS_WRAPPED);
	//SetButtonStyle(8 ,GetButtonStyle(8 )|TBBS_WRAPPED);
	SetButtonStyle(9 ,GetButtonStyle(9 )|TBBS_WRAPPED);
	//SetButtonStyle(10,GetButtonStyle(10)|TBBS_WRAPPED);
	SetButtonStyle(11,GetButtonStyle(11)|TBBS_WRAPPED);
	//SetButtonStyle(12,GetButtonStyle(12)|TBBS_WRAPPED);
	SetButtonStyle(13,GetButtonStyle(13)|TBBS_WRAPPED);
	//SetButtonStyle(14,GetButtonStyle(14)|TBBS_WRAPPED);
	SetButtonStyle(15,GetButtonStyle(15)|TBBS_WRAPPED);
	//SetButtonStyle(16,GetButtonStyle(16)|TBBS_WRAPPED);
	SetButtonStyle(17,GetButtonStyle(17)|TBBS_WRAPPED);
	//SetButtonStyle(18,GetButtonStyle(18)|TBBS_WRAPPED);
	SetButtonStyle(19,GetButtonStyle(19)|TBBS_WRAPPED);
	//SetButtonStyle(20,GetButtonStyle(20)|TBBS_WRAPPED);
	SetButtonStyle(21,GetButtonStyle(21)|TBBS_WRAPPED);
	//SetButtonStyle(22,GetButtonStyle(22)|TBBS_WRAPPED);
	SetButtonStyle(23,GetButtonStyle(23)|TBBS_WRAPPED);
	//SetButtonStyle(24,GetButtonStyle(24)|TBBS_WRAPPED);
	SetButtonStyle(25,GetButtonStyle(25)|TBBS_WRAPPED);
*/
	//SetButtonStyle(0,GetButtonStyle(0)|TBBS_CHECKBOX);
	//SetButtonStyle(1,GetButtonStyle(1)|TBBS_CHECKBOX);
	//SetButtonStyle(2,GetButtonStyle(2)|TBBS_CHECKBOX);

		//SetButtonStyle(0,GetButtonStyle(0)|TBBS_WRAPPED);
		//SetButtonStyle(1,GetButtonStyle(1)|TBBS_WRAPPED);
		//SetButtonStyle(2,GetButtonStyle(2)|TBBS_WRAPPED);

//	for(int i = 0; i < 14; i++)
	//	SetButtonStyle(i,GetButtonStyle(i)|TBBS_WRAPPED);
/*
		SetButtonStyle(1,GetButtonStyle(1)|TBBS_WRAPPED);
		SetButtonStyle(2,GetButtonStyle(2)|TBBS_WRAPPED);
		SetButtonStyle(3,GetButtonStyle(3)|TBBS_WRAPPED);
		SetButtonStyle(4,GetButtonStyle(4)|TBBS_WRAPPED);
		SetButtonStyle(5,GetButtonStyle(5)|TBBS_WRAPPED);
		SetButtonStyle(6,GetButtonStyle(6)|TBBS_WRAPPED);
		SetButtonStyle(7,GetButtonStyle(7)|TBBS_WRAPPED);
		SetButtonStyle(8,GetButtonStyle(8)|TBBS_WRAPPED);
		SetButtonStyle(9,GetButtonStyle(9)|TBBS_WRAPPED);
		SetButtonStyle(7,GetButtonStyle(7)|TBBS_WRAPPED);
		SetButtonStyle(8,GetButtonStyle(8)|TBBS_WRAPPED);


		SetButtonStyle(10,GetButtonStyle(10)|TBBS_WRAPPED);
		SetButtonStyle(12,GetButtonStyle(12)|TBBS_WRAPPED);
		*/
//SetButtonStyle(14,GetButtonStyle(14)|TBBS_WRAPPED);
//		SetButtonStyle(16,GetButtonStyle(16)|TBBS_WRAPPED);

	//	SetButtonStyle(11,GetButtonStyle(11)|TBBS_WRAPPED);
	//	SetButtonStyle(13,GetButtonStyle(13)|TBBS_WRAPPED);
//		SetButtonStyle(15,GetButtonStyle(15)|TBBS_WRAPPED);
/*
	CRect rect(0,0,COMBOBOX_WIDTH,COMBOBOX_HEIGHT);
	// The ID of the ComboBox is important for two reasons.  One, so you
	// can receive notifications from the control.  And also for ToolTips.
	// During HitTesting if the ToolBar sees that the mouse is one a child
	// control, the toolbar will lookup the controls ID and search for a
	// string in the string table with the same ID to use for ToolTips
	// and StatusBar info.
	if(!m_staticSpr.Create("",WS_CHILD| WS_VISIBLE | CBRS_BOTTOM,rect,this,ID_STATIC_SPRITE))
	{
		return FALSE;
	} //Create
	//m_staticSpr.GetClientRect
	/*
	if (!m_SearchBox.Create(WS_CHILD | CBS_DROPDOWN |
		CBS_AUTOHSCROLL | WS_VSCROLL | CBS_HASSTRINGS, rect, this,
		IDC_EDITBAR_SEARCHBOX))
	{
		return FALSE;
	}
*/
	/*
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
		hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);

  	m_SearchBox.SendMessage(WM_SETFONT, (WPARAM)hFont);

	if (m_staticSpr.m_hWnd != NULL)
	{
		CRect rect;
		GetItemRect(COMBOBOX_INDEX, rect);
		
		m_staticSpr.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
		m_staticSpr.ShowWindow(SW_SHOW);
	}
	SetButtonInfo(COMBOBOX_INDEX, ID_STATIC_SPRITE, TBBS_SEPARATOR, 70);
/*
	if(!SetHorizontal())
	//if(!SetVertical())
		return FALSE;
*/
	return TRUE;
}
