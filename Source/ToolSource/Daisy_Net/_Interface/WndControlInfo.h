

#if _MSC_VER > 1000
	#pragma once
#endif 

#include "../Resource/WndStyle.h"

#define DEF_CTRL_CHECK 		"ButtCheck.bmp"
#define DEF_CTRL_RADIO 		"ButtRadio.bmp"
#define DEF_CTRL_GROUP 		"WndEditTile200.tga"
#define DEF_CTRL_TAB1  		"WndTabTile00.bmp"
#define DEF_CTRL_TAB2  		"WndTabTile10.bmp"
#define DEF_WNDBASE    		"WndTile00.tga"
#define DEF_CTRL_TEXT  		"WndEditTile00.tga"


//---------------------------------------------------------------------------------------------
// 윈도우 컨트롤 정보 구조체 ( 기존 구조체 개선 및 정리 ) 
// Author	: lenahyang
// Date		: 2011.11.21
//---------------------------------------------------------------------------------------------


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 기본 )
//----------------------------------------------------------
typedef struct tagWndCtrl
{
	DWORD 		dwWndId;
	DWORD 		dwWndType;
	DWORD 		dwWndStyle;
	DWORD 		dwHistorypCnt;
	DWORD 		dwWndHistoryType;
	
	CString 	strTitle;
	CString 	strDefine;
	CString 	strDescription;
	CString 	strToolTip;	
	CString 	strTexture;

	BOOL		m_bTile;
	BOOL 		m_bGroup;
	BOOL 		m_bTabStop;
	BOOL 		m_bVisible;
	BOOL 		m_bDisabled;

	int			m_nFontColorR;
	int			m_nFontColorG;
	int			m_nFontColorB;

	CRect		rect;
	CRect		m_stPreviewControlOffSet;
	CPoint		m_stPreviewControlTextPos;

	DWORD		m_dwPreviewControlBoxColor;
	DWORD		m_dwPreviewControlTextColor;


#ifdef __USE_STR_TBL0407
	char		m_szTitleKey[100];
	char		m_szToolTipKey[100];
#else	
	#ifdef __DAISY
		wchar_t awszTitle[ LANG_MAX ][ 64 ];
		wchar_t awszToolTip[ LANG_MAX ][ 256 ];
	#endif	
#endif	


#ifdef __USE_STR_TBL0407

	tagWndCtrl()
	:	dwWndId( NULL )
	,	dwWndType( WTYPE_BASE )
	,	dwWndStyle( 0x00000000L )
	,	dwHistorypCnt( NULL )
	,	dwWndHistoryType( NULL )
	,	strTitle(_T("None"))
	,	strDefine(_T("WIDC_"))
	,	strDescription( _T("Description"))
	,	strToolTip(_T(""))
	,	strTexture(_T(""))
	,	m_bTile( FALSE )
	, 	m_bGroup( FALSE )
	,	m_bTabStop( FALSE )
	,	m_bVisible( FALSE )
	,	m_bDisabled( FALSE )
	,	m_nFontColorR( NULL )
	,	m_nFontColorG( NULL )
	,	m_nFontColorB( NULL )
	,	rect( CRect( 0, 0, 0, 0 ) )
	,	m_stPreviewControlOffSet( CRect( 0, 0, 0, 0 ) )
	,	m_stPreviewControlTextPos( CPoint( 0, 0 ) )
	,	m_dwPreviewControlBoxColor( D3DCOLOR_ARGB(255, 255, 0, 0) )
	,	m_dwPreviewControlTextColor( D3DCOLOR_ARGB(255, 0, 0, 255) )
	{
		memset( m_szTitleKey, 0, sizeof(char) * 100  );
		memset( m_szToolTipKey, 0, sizeof(char) * 100 );
	}

#endif	

} WNDCTRL,* LPWNDCTRL; 




//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 버튼 )
//----------------------------------------------------------
typedef struct tagWndCtrl_Button : public tagWndCtrl
{
	tagWndCtrl_Button()
	{
		dwWndType					=  WTYPE_BUTTON;
		strTitle					= _T("Button");
		strDefine					= _T("WIDC_BUTTON");
		strDescription				= _T("버튼");
		rect						= CRect( 0, 0, 96, 24 );
		m_stPreviewControlOffSet	= CRect( -48, -15, 48, 15 );
		m_stPreviewControlTextPos	= CPoint( 17, 30 );
	}

} WNDCTRL_BUTTON, *LPWNDCTRL_BUTTON; 


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 체크박스 )
//----------------------------------------------------------
typedef struct tagWndCtrl_CheckBox : public tagWndCtrl
{
	tagWndCtrl_CheckBox()
	{
		dwWndType					= WTYPE_BUTTON;
		dwWndStyle					= WBS_CHECK;
		strTitle					= _T("Check Box");
		strDefine					= _T("WIDC_CHECK");
		strDescription				= _T("체크 박스");
		strTexture					= DEF_CTRL_CHECK;
		rect						= CRect( 0, 0, 96, 16 );
		m_stPreviewControlOffSet	= CRect( -48, -10, 48, 10 );
		m_stPreviewControlTextPos	= CPoint( 30, 23 );
	}

} WNDCTRL_CHECKBOX, *LPWNDCTRL_CHECKBOX; 


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 라디오 버튼 )
//----------------------------------------------------------
typedef struct tagWndCtrl_RadioButton : public tagWndCtrl
{
	tagWndCtrl_RadioButton()
	{
		dwWndType					= WTYPE_BUTTON;
		dwWndStyle					= WBS_RADIO;
		strTitle					= _T("Radio Button");
		strDefine					= _T("WIDC_RADIO");
		strDescription				= _T("라디오 버튼");
		strTexture					= DEF_CTRL_RADIO;
		rect						= CRect( 0, 0, 96, 16 );
		m_stPreviewControlOffSet	= CRect( -48, -10, 48, 10 );
		m_stPreviewControlTextPos	= CPoint( 30, 23 );
	}

} WNDCTRL_RADIOBUTTON, *LPWNDCTRL_RADIOBUTTON; 


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 정적 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_StaticBox : public tagWndCtrl
{
	tagWndCtrl_StaticBox()
	{
		dwWndType					= WTYPE_STATIC;
		strTitle					= _T("Static Box");
		strDefine					= _T("WIDC_STATIC");
		strDescription				= _T("스태틱 박스");
		rect						= CRect( 0, 0, 96, 16 );
		m_stPreviewControlOffSet	= CRect( -48, -10, 48, 10 );
		m_stPreviewControlTextPos	= CPoint( 15, 23 );
	}

} WNDCTRL_STATICBOX, *LPWNDCTRL_STATICBOX; 


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 그룹 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_GroupBox : public tagWndCtrl
{
	tagWndCtrl_GroupBox()
	{
		dwWndType					= WTYPE_STATIC;
		dwWndStyle					= WSS_GROUPBOX;
		strTitle					= _T("Group Box");
		strDefine					= _T("WIDC_STATIC");
		strDescription				= _T("그룹 박스");
		strTexture					= DEF_CTRL_GROUP;
		rect						= CRect( 0, 0, 96, 16 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -50, -10, 50, 12 );
		m_stPreviewControlTextPos	= CPoint( 30, 23 );
	}

} WNDCTRL_GROUPBOX, *LPWNDCTRL_GROUPBOX;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 그림 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_PictureBox : public tagWndCtrl
{
	tagWndCtrl_PictureBox()
	{
		dwWndType					= WTYPE_STATIC;
		dwWndStyle					= WSS_PICTURE;
		strTitle					= _T("Picture Box");
		strDefine					= _T("WIDC_STATIC");
		strDescription				= _T("픽쳐 박스");
		rect						= CRect( 0, 0, 96, 96 );
		m_nFontColorR 				= 46;
		m_nFontColorG 				= 112;
		m_nFontColorB 				= 169;
		m_stPreviewControlOffSet	= CRect( -50, -48, 50, 47 );
		m_stPreviewControlTextPos	= CPoint( 15, 60 );
	}

} WNDCTRL_PICTUREBOX, *LPWNDCTRL_PICTUREBOX;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 글 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_TextBox : public tagWndCtrl
{
	tagWndCtrl_TextBox()
	{
		dwWndType					= WTYPE_TEXT;
		dwWndStyle					= WBS_VSCROLL;
		strTitle					= _T("Text Box");
		strDefine					= _T("WIDC_TEXT");
		strDescription				= _T("텍스트 박스");
		strTexture					= DEF_CTRL_TEXT;
		rect						= CRect( 0, 0, 96, 48 );
		m_bTile						= TRUE;
		m_nFontColorR 				= 46;
		m_nFontColorG 				= 112;
		m_nFontColorB 				= 169;
		m_stPreviewControlOffSet	= CRect( -50, -28, 52, 27 );
		m_stPreviewControlTextPos	= CPoint( 15, 42 );
	}

} WNDCTRL_TEXTBOX, *LPWNDCTRL_TEXTBOX;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 편집 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_EditBox : public tagWndCtrl
{
	tagWndCtrl_EditBox()
	{
		dwWndType					= WTYPE_EDITCTRL;
		strTitle					= _T("Edit Box");
		strDefine					= _T("WIDC_EDIT");
		strDescription				= _T("에디트 박스");
		strTexture					= DEF_CTRL_TEXT;
		rect						= CRect( 5, 5, 96, 24 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -45, -7, 52, 20 );
		m_stPreviewControlTextPos	= CPoint( 10, 23 );
	}

} WNDCTRL_EDITBOX, *LPWNDCTRL_EDITBOX;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 목록 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_ListBox : public tagWndCtrl
{
	tagWndCtrl_ListBox()
	{
		dwWndType					= WTYPE_LISTBOX;
		dwWndStyle					= WBS_VSCROLL;
		strTitle					= _T("List Box");
		strDefine					= _T("WIDC_LISTBOX");
		strDescription				= _T("리스트 박스");
		strTexture					= DEF_CTRL_TEXT;
		rect						= CRect( 5, 5, 96, 96 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -45, -44, 55, 53 );
		m_stPreviewControlTextPos	= CPoint( 15, 60 );
	}

} WNDCTRL_LISTBOX, *LPWNDCTRL_LISTBOX;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 목록 컨트롤 )
//----------------------------------------------------------
typedef struct tagWndCtrl_ListControl : public tagWndCtrl
{
	tagWndCtrl_ListControl()
	{
		dwWndType					= WTYPE_LISTCTRL;
		dwWndStyle					= WBS_VSCROLL;
		strTitle					= _T("List Control");
		strDefine					= _T("WIDC_LISTCTRL");
		strDescription				= _T("리스트 컨트롤");
		strTexture					= DEF_CTRL_TEXT;
		rect						= CRect( 5, 5, 96, 96 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -45, -44, 55, 53 );
		m_stPreviewControlTextPos	= CPoint( 30, 60 );
	}

} WNDCTRL_LISTCONTROL, *LPWNDCTRL_LISTCONTROL;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 콤보 상자 )
//----------------------------------------------------------
typedef struct tagWndCtrl_ComboBox : public tagWndCtrl
{
	tagWndCtrl_ComboBox()
	{
		dwWndType					= WTYPE_COMBOBOX;
		strTitle					= _T("Combo Box");
		strDefine					= _T("WIDC_COMBOBOX");
		strDescription				= _T("콤보 박스");
		strTexture					= DEF_CTRL_TEXT;
		rect						= CRect( 5, 5, 96, 24 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -45, -7, 52, 20 );
		m_stPreviewControlTextPos	= CPoint( 30, 23 );
	}

} WNDCTRL_COMBOBOX, *LPWNDCTRL_COMBOBOX;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 탭 컨트롤 )
//----------------------------------------------------------
typedef struct tagWndCtrl_TabControl : public tagWndCtrl
{
	tagWndCtrl_TabControl()
	{
		dwWndType					= WTYPE_TABCTRL;
		strTitle					= _T("Tab Control");
		strDefine					= _T("WIDC_TABCTRL");
		strDescription				= _T("탭 컨트롤");
		rect						= CRect( 5, 5, 96, 96 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -45, -44, 55, 53 );
		m_stPreviewControlTextPos	= CPoint( 30, 60 );
	}

} WNDCTRL_TABCONTROL, *LPWNDCTRL_TABCONTROL;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 트리 컨트롤 )
//----------------------------------------------------------
typedef struct tagWndCtrl_TreeControl : public tagWndCtrl
{
	tagWndCtrl_TreeControl()
	{
		dwWndType					= WTYPE_TREECTRL;
		dwWndStyle					= WBS_VSCROLL;
		strTitle					= _T("Tree Control");
		strDefine					= _T("WIDC_TREECTRL");
		strDescription				= _T("트리 컨트롤");
		strTexture					= DEF_CTRL_TEXT;
		rect						= CRect( 5, 5, 96, 96 );
		m_bTile						= TRUE;
		m_stPreviewControlOffSet	= CRect( -45, -44, 55, 53 );
		m_stPreviewControlTextPos	= CPoint( 30, 60 );
	}

} WNDCTRL_TREECONTROL, *LPWNDCTRL_TREECONTROL;


//----------------------------------------------------------
// 컨트롤 정보 구조체 ( 커스텀 컨트롤 )
//----------------------------------------------------------
typedef struct tagWndCtrl_CustomControl : public tagWndCtrl
{
	tagWndCtrl_CustomControl()
	{
		dwWndType					= WTYPE_CUSTOM;
		strTitle					= _T("Custom Control");
		strDefine					= _T("WIDC_CUSTOM");
		strDescription				= _T("커스텀 컨트롤");
		rect						= CRect( 5, 5, 96, 96 );
		m_stPreviewControlOffSet	= CRect( -45, -44, 55, 53 );
		m_stPreviewControlTextPos	= CPoint( 17, 60 );
	}

} WNDCTRL_CUSTOMCONTROL, *LPWNDCTRL_CUSTOMCONTROL;

