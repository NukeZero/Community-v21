// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineObj.h"
#include "DefineItem.h"
#include "defineText.h"
#include "defineskill.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "Party.h"
#include "WndManager.h"

#ifdef __ONLINE
#include "DPClient.h"
#include "MsgHdr.h"
extern	CDPClient	g_DPlay;
#endif

#include "wndvendor.h"
#include "wndwebbox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_nSkillCurSelect;
extern float g_fHairLight;


//extern TCHAR g_szExpertName[ MAX_EXPERT ][ 32 ];
//extern TCHAR g_szJobName[ MAX_JOB ][ 32 ];
//extern const JOB prj.m_aJob[ MAX_JOB ];

#define WND_WIDTH 210
#define TASKBAR_HEIGHT 44

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;


#define MAX_GUILDCOMBAT_LIST_PER_PAGE 11
#define MAX_MAIL_LIST_PER_PAGE 6
#define MAX_GUILDCOMBAT_LIST		  100
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 애플렛 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndApplet::CWndApplet()
{
}
CWndApplet::~CWndApplet()
{
}
void CWndApplet::OnDraw(C2DRender* p2DRender)
{
	/*
	DWORD dwColor = D3DCOLOR_ARGB(255,50,50,150);
	int nCount = 0;
	while( g_WndMng.m_appletFunc[ nCount ].m_pFunc )
	{
		p2DRender->TextOut( 5, nCount * 15, g_WndMng.m_appletFunc[nCount].m_pAppletName ,dwColor );
		nCount++;
	}
	*/
}
void CWndApplet::OnInitialUpdate()
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//리스트 아이콘 작은아이콘 
	//m_wndButton1.Create( "리스트"      , 0, rect3_1, this, IDABORT );
	//m_wndButton2.Create( "아이콘"      , 0, rect3_2, this, IDRETRY );
	//m_wndButton3.Create( "작은아이콘"  , 0, rect3_3, this, IDIGNORE);

	m_wndList.Create( WBS_CHILD, GetClientRect(), this, 0);
		/*vam
	int nCount = 0;
	while( g_WndMng.m_appletFunc[ nCount ].m_pFunc )
	{
		//p2DRender->TextOut( 5, nCount * 15, g_WndMng.m_appletFunc[nCount].m_pAppletName ,dwColor );
		DWORD dwIndex = m_wndList.AddString( g_WndMng.m_appletFunc[nCount].m_pAppletName );
		m_wndList.SetItemData( dwIndex, g_WndMng.m_appletFunc[nCount].m_dwIdApplet );
		nCount++;
	}*/
	DWORD dwIdApplet;
	AppletFunc* pApplet;
	POSITION pos = g_WndMng.m_mapAppletFunc.GetStartPosition();
	while( pos )
	{
		g_WndMng.m_mapAppletFunc.GetNextAssoc( pos, dwIdApplet, (void*&)pApplet );
		DWORD dwIndex = m_wndList.AddString( pApplet->m_pAppletName );
		m_wndList.SetItemData( dwIndex, pApplet->m_dwIdApplet );
	}

	CWndNeuz::OnInitialUpdate();
}
BOOL CWndApplet::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50, 50, 300, 300 );
	SetTitle( _T( "Applet Folder" ) );
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_TOPMOST | WBS_THICKFRAME, rect, pWndParent, dwWndId );
}
BOOL CWndApplet::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 0 && message == WNM_DBLCLK) // list
	{
		DWORD dwIdApplet = m_wndList.GetItemData( m_wndList.GetCurSel());
		SHORTCUT shortcut;
		shortcut.m_dwShortcut = SHORTCUT_APPLET;
		shortcut.m_dwId = dwIdApplet;
		g_WndMng.ObjectExecutor( &shortcut );//SHORTCUT_APPLET, dwIdApplet);
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndApplet::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 0: // wnd1 
			g_WndMng.OpenWnd1();
			break;
		case 1: // wnd2
			g_WndMng.OpenCharacter();
			break;
		case 2: // 챗 
			g_WndMng.OpenChat();
			break;
		case 3:
			g_clientMsg.PutMessage(&g_Neuz.m_2DRender,0xffffffff,"fuck");
			break;
		case 9:
			g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndApplet::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//m_wndButton1.SetWndRect( rect3_1 );
	//m_wndButton2.SetWndRect( rect3_2 );
	//m_wndButton3.SetWndRect( rect3_3 );

	m_wndList.SetWndRect( GetClientRect() );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndApplet::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndApplet::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 드롭아이템
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndDropItem::CWndDropItem() 
{ 
	m_pItemElem = NULL;
	m_pEdit = NULL;
} 
CWndDropItem::~CWndDropItem() 
{ 
} 
void CWndDropItem::OnDraw( C2DRender* p2DRender ) 
{ 
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi( szNumber );

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
	}
	else
	{
		if( m_pItemElem->m_nItemNum < nNumber )
		{
			char szNumberbuf[16] = {0, };
			nNumber = m_pItemElem->m_nItemNum;
			_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		for( int i = 0 ; i < 8 ; i++ )
		{
			char szNumberbuf[8] = {0, };
			strncpy( szNumberbuf, szNumber, 8 );
			
			// 0 : 공백, 48 : 숫자 0, 57 : 숫자 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = m_pItemElem->m_nItemNum;
					_itoa( m_pItemElem->m_nItemNum, szNumberbuf, 10 );
					m_pEdit->SetString( szNumberbuf );
					break;												
				}
			}
		}
	}
} 
void CWndDropItem::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	m_pEdit   = (CWndEdit  *)GetDlgItem( WIDC_EDIT1 );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault( TRUE );
	m_pEdit->SetFocus();

	if( m_pItemElem->m_nItemNum == 1 )
	{
		m_pEdit->SetString( "1" );
		//		m_pEdit->SetVisible( FALSE );
	}
	else
	{
		TCHAR szNumber[ 64 ];
		_itot( m_pItemElem->m_nItemNum, szNumber, 10 );
		m_pEdit->SetString( szNumber );
	}	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDropItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DROP_ITEM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndDropItem::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndDropItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDropItem::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDropItem::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDropItem::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndDropItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		short DropNum = 0;

		if( m_pItemElem->m_nItemNum >= 1 )
		{
			DropNum = atoi( m_pEdit->GetString() );
		}
		if( DropNum != 0 )
		{
#ifdef __ONLINE	
			g_DPlay.SendDropItem( 0, m_pItemElem->m_dwObjId, DropNum, m_vPos );
#endif
		}

		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 드롭확인
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndDropConfirm::CWndDropConfirm() 
{ 
	m_pItemElem = NULL;
	m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
} 
CWndDropConfirm::~CWndDropConfirm() 
{ 
} 
void CWndDropConfirm::OnDraw( C2DRender* p2DRender ) 
{ 

} 
void CWndDropConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_YES );	
	pWndOk->SetDefault( TRUE );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndDropConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DROP_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndDropConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndDropConfirm::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndDropConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndDropConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndDropConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES || message == EN_RETURN )
	{
#ifdef __ONLINE	
		g_DPlay.SendDropItem( 0, m_pItemElem->m_dwObjId, 1, m_vPos );
#endif
		Destroy();
	}
	else if( nID == WIDC_NO )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#ifdef __S050504_QUESTITEM
/****************************************************
WndId : APP_QUEITMWARNING - 아이템 경고
CtrlId : WIDC_CONTEXT - 
CtrlId : WIDC_BTN_YES - Button
CtrlId : WIDC_BTN_NO - Button
CtrlId : WIDC_WARNING - Picture
****************************************************/

CWndQuestItemWarning::CWndQuestItemWarning() 
{ 
	m_pItemElem = NULL;
	m_vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
} 
CWndQuestItemWarning::~CWndQuestItemWarning() 
{ 
} 
void CWndQuestItemWarning::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndQuestItemWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( _T( prj.GetText( TID_GAME_QUEITMWARNING ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEITMWARNING, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndQuestItemWarning::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndQuestItemWarning::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndQuestItemWarning::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndQuestItemWarning::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndQuestItemWarning::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndQuestItemWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		if( m_pItemElem->m_nItemNum == 1 )	// 갯수가 하나라면 그냥 떨군다.
		{
			g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
			g_WndMng.m_pWndDropConfirm->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropConfirm->m_vPos = m_vPos;
			g_WndMng.m_pWndDropConfirm->Initialize( NULL, APP_DROP_CONFIRM );
		}
		else
		{
			//SAFE_DELETE( g_WndMng.m_pWndDropItem );
			g_WndMng.m_pWndDropItem = new CWndDropItem;
			g_WndMng.m_pWndDropItem->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropItem->m_vPos = m_vPos;
			g_WndMng.m_pWndDropItem->Initialize( NULL, APP_DROP_ITEM );
		}
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif // __S050504_QUESTITEM
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 인벤토리 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "WndShop.h"

CWndGold::CWndGold()
{
}
CWndGold::~CWndGold()
{
}
void CWndGold::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_pTexture = &m_texture; //.m_pFromWnd   = this;
	m_GlobalShortcut.m_dwData = 0;
}
void CWndGold::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndGold::OnInitialUpdate()
{
	m_texture.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_ITEM, "itm_GolGolSeed.dds" ), 0xffff00ff );
}

#ifdef __QUERYEQUIP0630
//////////////////////////////////////////////
CWndQueryEquip::CWndQueryEquip()
{
	m_ObjID = NULL_ID;
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );

	m_pModel = NULL;
	m_OldPos = CPoint(0,0);
}
CWndQueryEquip::~CWndQueryEquip()
{
	SAFE_DELETE( m_pModel );
}

BOOL CWndQueryEquip::Process()
{
	CMover* pMover = GetMover();

	if( IsInvalidObj(pMover) )
	{
		Destroy();
		return FALSE;
	}

	D3DXVECTOR3	vDist = pMover->GetPos() - g_pPlayer->GetPos();	// 타겟과의 거리.
	FLOAT		fDistSq = D3DXVec3LengthSq( &vDist );
	
	int nHalfRange = 20 / 2;
	if( fDistSq > (nHalfRange * nHalfRange) )
	{
		Destroy();
		return FALSE;		
	}

	if( m_pModel )
		m_pModel->FrameMove();
		
	return TRUE;
}	
void CWndQueryEquip::OnMouseWndSurface( CPoint point )
{
	CMover* pMover = GetMover();

	if( IsInvalidObj(pMover) )
		return ;
	
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId == NULL_ID )
			continue;
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// 툴팁
		if( DrawRect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			
			CItemElem itemElem;
			itemElem.m_dwItemId	= pMover->m_aEquipInfo[i].dwId;
			itemElem.m_byFlag	= pMover->m_aEquipInfo[i].byFlag;
			itemElem.SetAbilityOption( pMover->m_aEquipInfo[i].nOption & 0xFF );
			itemElem.m_nResistAbilityOption = m_aEquipInfoAdd[i].nResistAbilityOption;
			itemElem.m_bItemResist	= m_aEquipInfoAdd[i].bItemResist;
			itemElem.m_nRandomOptItemId	= m_aEquipInfoAdd[i].nRandomOptItemId;
			memcpy( &itemElem.m_piercingInfo, &(m_aEquipInfoAdd[i].piercingInfo), sizeof(PIERCINGINFO) );

			
			// 장비창에 있는것 툴팁
			g_WndMng.PutToolTip_Item( (CItemBase*)&itemElem, point2, &DrawRect, APP_QUERYEQUIP );
		}
	}
}
void CWndQueryEquip::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;//100;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;//105;
	viewport.Height = 182;//p2DRender->m_clipRect.Height() - 200;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		FLOAT sx = 1.0f, sy = 1.0f;

		DWORD dwAlpha = 255;

		if( pMover->m_aEquipInfo[i].dwId == NULL_ID )
			continue;

		ItemProp* pItemProp	= prj.GetItemProp( pMover->m_aEquipInfo[i].dwId );
		if( !pItemProp )
			continue;

		if( m_aEquipInfoAdd[i].pTexture == NULL )
			continue;

		if( i == PARTS_LWEAPON )		// 왼손무기 그릴타이밍일때
		{
			if( pItemProp->dwHanded == HD_TWO )	// 투핸드 무기면
			{
				dwAlpha   = 100;
			}
		}
		
		CRect DrawRect = m_InvenRect[i];
		CPoint cpAdd = CPoint(6,6);
		
		if( i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2 )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.8f;
			sy = 0.8f;
		}
		else
		if( i >= PARTS_HAT && i <= PARTS_BOOTS )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.9f;
			sy = 0.9f;
		}

#ifdef __FINITEITEM0705
		if( pMover->m_aEquipInfo[i].byFlag & CItemElem::expired )
		{
			p2DRender->RenderTexture2( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, sx, sy, D3DCOLOR_XRGB( 255, 100, 100 ) );
		}
		else
#endif // __FINITEITEM0705
		{
			p2DRender->RenderTexture( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, dwAlpha, sx, sy );
		}
	}

	if( m_pModel == NULL )
		return;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
	#endif //__YENV
	
	// 왼쪽 원본 모델 랜더링
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM20 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width  = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
		
		D3DXMatrixScaling(&matScale,1.2f,1.2f,1.2f);
		D3DXMatrixTranslation(&matTrans,0.0f,-0.6f,0.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
	pMover->OverCoatItemRenderCheck(m_pModel);
		
	#ifdef __YHAIR_HIDE
	// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 없는경우
		#ifdef __FINITEITEM0705
			DWORD dwId	= pMover->m_aEquipInfo[PARTS_CAP].dwId;
		#else	// __FINITEITEM0705
			DWORD dwId	= pMover->m_adwEquipment[PARTS_CAP];
		#endif	// __FINITEITEM0705
			O3D_ELEMENT*	pElement = NULL;
			ItemProp*		pItemProp = NULL;
			
			if( dwId != NULL_ID )
			{
				pItemProp	= prj.GetItemProp( dwId );
				
				if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
				{
					pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
				}
				
		#ifdef	__YPARTS_EX
				// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
		#ifdef __FINITEITEM0705
				dwId	= pMover->m_aEquipInfo[PARTS_HAT].dwId;
		#else	// __FINITEITEM0705
				dwId	= pMover->m_adwEquipment[PARTS_HAT];
		#endif	// __FINITEITEM0705
				if( dwId != NULL_ID )
				{
		#ifdef __FINITEITEM0705
					if( !(pMover->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
		#else	// __FINITEITEM0705
					if( !(pMover->m_adwEquipment[PARTS_HAT].byFlag & CItemElem::expired) )
		#endif	// __FINITEITEM0705
					{
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
							
							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
						else
						{
							if( pElement )
								pElement->m_nEffect &= ~XE_HIDE;
						}
					}
				}
		#endif //__YPARTS_EX									
			}
			else
			{
		#ifdef	__YPARTS_EX
				// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
		#ifdef __FINITEITEM0705
				dwId	= pMover->m_aEquipInfo[PARTS_HAT].dwId;
		#else	// __FINITEITEM0705
				dwId	= pMover->m_adwEquipment[PARTS_HAT];
		#endif	// __FINITEITEM0705
				if( dwId != NULL_ID )
				{
		#ifdef __FINITEITEM0705
					if( !(pMover->m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
		#else	// __FINITEITEM0705
					if( !(pMover->m_adwEquipment[PARTS_HAT].byFlag & CItemElem::expired) )
		#endif	// __FINITEITEM0705
					{						
						pItemProp	= prj.GetItemProp( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
					}
				}
		#endif //__YPARTS_EX									
			}
	#endif //__YHAIR_HIDE
			
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = pMover->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = pMover->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = pMover->m_fHairColorB;
			
		
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}

	return;
}
void CWndQueryEquip::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
}
void CWndQueryEquip::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
	
}
void CWndQueryEquip::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
}	
	
void CWndQueryEquip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;
	
	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM6 );
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM7 );
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM8 );
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM9 );
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl( WIDC_CUSTOM10 );
	
	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;
	
	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;
	
	lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM11 );
	lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM12 );
	lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM13 );
	lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM14 );
	lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM15 );
	lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM16 );
	lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM17 );
	lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM18 );
	lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM19 );
	
	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;
	
	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;
	
	MoveParentCenter();
}

void CWndQueryEquip::SetMover( DWORD ObjID )
{
	m_ObjID = ObjID;

	SAFE_DELETE( m_pModel );
	
	if( GetMover() )
	{
		int nMover = (GetMover()->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
#ifdef __FINITEITEM0705
		CMover::UpdateParts( GetMover()->GetSex(), GetMover()->m_dwSkinSet, GetMover()->m_dwFace, GetMover()->m_dwHairMesh, GetMover()->m_dwHeadMesh,GetMover()->m_aEquipInfo, m_pModel, NULL );
#else	// __FINITEITEM0705
		CMover::UpdateParts( GetMover()->GetSex(), GetMover()->m_dwSkinSet, GetMover()->m_dwFace, GetMover()->m_dwHairMesh, GetMover()->m_dwHeadMesh,GetMover()->m_adwEquipment, m_pModel, NULL );
#endif	// __FINITEITEM0705
		m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	}
}

void CWndQueryEquip::SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd )
{
	memcpy( m_aEquipInfoAdd, aEquipInfoAdd, sizeof(EQUIP_INFO_ADD) * MAX_HUMAN_PARTS );

	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
#ifdef __CLIENT
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( pMover->m_aEquipInfo[i].dwId != NULL_ID )
		{
			ItemProp* pItemProp	= prj.GetItemProp( pMover->m_aEquipInfo[i].dwId );
			if( pItemProp )
				m_aEquipInfoAdd[i].pTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		}
	}
#endif	// __CLIENT
}

BOOL CWndQueryEquip::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();

	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// 인벤토리 출력 위치 지정
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}

#endif //__QUERYEQUIP0630






//////////////////////////////////////////////
CWndInventory::CWndInventory()
{
	m_bReport = FALSE;
	m_pSelectItem = NULL;
	m_pWndConfirmBuy = NULL;

#ifdef __YENCHANT_0608
	m_bIsUpgradeMode = FALSE;
	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_pSfxUpgrade = NULL;
	m_dwEnchantWaitTime = 0xffffffff;
#endif //__YENCHANT_0608
	
#ifdef __YINVENTORY_3D
	m_pModel = NULL;
	m_OldPos = CPoint(0,0);
#endif //__YINVENTORY_3D
	
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
}
CWndInventory::~CWndInventory()
{
#ifdef __YINVENTORY_3D
	SAFE_DELETE( m_pModel );
#endif //__YINVENTORY_3D	
	
	SAFE_DELETE( m_pWndConfirmBuy );

#ifdef __YENCHANT_0608
	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
#endif //__YENCHANT_0608
}

void CWndInventory::OnDestroy( void )
{
#ifdef __YINVENTORY_3D
	SAFE_DELETE( m_pModel );
#endif //__YINVENTORY_3D	

	SAFE_DELETE( m_pWndConfirmBuy );	
}
void CWndInventory::OnMouseWndSurface( CPoint point )
{
#ifndef __YINVENTORY_3D
	CRect rect = GetClientRect();

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	CRect rectCtrl = lpWndCtrl->rect;
	
	int GapX, GapY;
	
	GapX = rectCtrl.left / 2;
	GapY = rectCtrl.top + 32;
#endif //__YINVENTORY_3D

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CItemBase* pItemBase = NULL;
		
		pItemBase = g_pPlayer->GetEquipItem( i );
		
		CRect DrawRect = m_InvenRect[i];
		
#ifndef __YINVENTORY_3D
		DrawRect.left -= GapX;
		DrawRect.top  -= GapY;
		
		DrawRect.right   -= GapX;
		DrawRect.bottom  -= GapY;
#endif //__YINVENTORY_3D		

		if( pItemBase )
		{
			CPoint point = GetMousePoint();
			// 툴팁
			if( DrawRect.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				// 장비창에 있는것 툴팁
				g_WndMng.PutToolTip_Item( pItemBase, point2, &DrawRect, APP_INVENTORY );
			}
		} 
	}
}
void CWndInventory::OnDraw(C2DRender* p2DRender)
{
#ifdef __YINVENTORY_3D
	CMover* pMover = g_pPlayer;
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;
	viewport.Height = 182;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	CString strGoldNum;
	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	CWndTrade * pWndTrade = (CWndTrade *)g_WndMng.GetWndBase( APP_TRADE );
	strGoldNum.Format( _T( "%d" ), g_pPlayer->GetGold() );

	pGoldNum->SetTitle( strGoldNum );

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		DWORD dwAlpha = 255;
		CItemBase* pItemBase = g_pPlayer->GetEquipItem( i );
		if( i == PARTS_LWEAPON )		// 왼손무기 그릴타이밍일때
		{
			CItemBase* pRWeapon = g_pPlayer->GetEquipItem( PARTS_RWEAPON );		// 오른손 무기를 꺼내보고
			if( pRWeapon && pRWeapon->GetProp()->dwHanded == HD_TWO )	// 투핸드 무기면
			{
				pItemBase = pRWeapon;	// 오른손무기랑 같은걸 그리자.
				dwAlpha   = 100;
			}
		}

		FLOAT sx = 1.0f;
		FLOAT sy = 1.0f;

		CPoint cpAdd = CPoint(6,6);
		CRect DrawRect = m_InvenRect[i];

		if( i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2 )
		{
			cpAdd = CPoint(0,0);

			sx = 0.8f;
			sy = 0.8f;
		}
		else
		if( i >= PARTS_HAT && i <= PARTS_BOOTS )
		{
			cpAdd = CPoint(0,0);
			
			sx = 0.9f;
			sy = 0.9f;
		}
		
		
		if( pItemBase && pItemBase->GetTexture() )
		{
		#ifdef __FINITEITEM0705
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_XRGB( 255, 100, 100 ), sx, sy );
			}
			else
		#endif // __FINITEITEM0705
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_ARGB( dwAlpha, 255, 255, 255 ), sx, sy );
			}

			CItemElem *pItemElem = (CItemElem *)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )		// 묶음 아이템이냐?
			{
				short nItemNum	= pItemElem->m_nItemNum;

				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				int x = DrawRect.left;	
				int y = DrawRect.top;
				m_p2DRender->TextOut( x + 42 - size.cx, y + 42 - size.cy, szTemp, 0xff0000ff );
				m_p2DRender->TextOut( x + 41 - size.cx, y + 41 - size.cy, szTemp, 0xffb0b0f0 );
			}
		} 
	}

	if( g_pPlayer == NULL || m_pModel == NULL )
		return;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255, 255,255,255) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 카메라 
	D3DXMATRIX  matView;
	D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 3.0f );
	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
	
	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
	
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	#ifdef __YENV
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
	SetLightVec( vDir );
	#endif //__YENV
	
	{
		LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM20 );

		viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
		viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
		viewport.Width  = lpFace->rect.Width();
		viewport.Height = lpFace->rect.Height();

		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
		FLOAT fov = D3DX_PI/4.0f;//796.0f;
		FLOAT h = cos(fov/2) / sin(fov/2);
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		
	    D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixRotationY( &matRot, D3DXToRadian( m_fRot ) );
		
		D3DXMatrixScaling(&matScale,1.2f,1.2f,1.2f);
		D3DXMatrixTranslation(&matTrans,0.0f,-0.6f,0.0f);
		
		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );//m_bViewLight );
		
		::SetLight( FALSE );
		::SetFog( FALSE );
		SetDiffuse( 1.0f, 1.0f, 1.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );

		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = g_pPlayer->m_fHairColorB;

		D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	#ifdef __YENV
		g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vConst );
	#else //__YENV						
		pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	#endif //__YENV
		::SetTransformView( matView );
		::SetTransformProj( matProj );
	
	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(m_pModel);
		
	#ifdef __YHAIR_HIDE
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );
		if( pItemElem )
		{
			O3D_ELEMENT* pElement = NULL;
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( CItemElem::expired )) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					//m_pModel->SetEffectOff(PARTS_HAIR, XE_HIDE );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					
					m_pModel->SetEffectOff(PARTS_HEAD, XE_HIDE );
				}
			}
		#endif //__YPARTS_EX								
		}
		else
		{
		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( CItemElem::expired )) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		#endif //__YPARTS_EX										
		}
	#endif //__YHAIR_HIDE
		
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}

#else //__YINVENTORY_3D

	CMover* pMover = g_pPlayer;
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + 5;//100;// + 5;
	viewport.Y      = p2DRender->m_ptOrigin.y + 5;// + 5;
	viewport.Width  = p2DRender->m_clipRect.Width() - 100;//105;
	viewport.Height = 182;//p2DRender->m_clipRect.Height() - 200;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	CString strGoldNum;
	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	strGoldNum.Format( _T( "%d" ), g_pPlayer->GetGold() );

	pGoldNum->SetTitle( strGoldNum );

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	CRect rect = GetClientRect();

	rect.DeflateRect( 5 , 5 );
	rect.bottom = viewport.Height + 5;//;

//	m_pTheme->RenderEdge( p2DRender, &rect );

	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	CRect rectCtrl = lpWndCtrl->rect;

	int GapX, GapY;

	GapX = rectCtrl.left / 2;
	GapY = rectCtrl.top + 32;

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		FLOAT sx = 1.0f, sy = 1.0f;

		DWORD dwAlpha = 255;
		CItemBase* pItemBase = g_pPlayer->GetEquipItem( i );
		if( i == PARTS_LWEAPON )		// 왼손무기 그릴타이밍일때
		{
			CItemBase* pRWeapon = g_pPlayer->GetEquipItem( PARTS_RWEAPON );		// 오른손 무기를 꺼내보고
			if( pRWeapon && pRWeapon->GetProp()->dwHanded == HD_TWO )	// 투핸드 무기면
			{
				pItemBase = pRWeapon;	// 오른손무기랑 같은걸 그리자.
				dwAlpha   = 100;
			}
		}
		
		CRect DrawRect = m_InvenRect[i];

		DrawRect.left -= GapX;
		DrawRect.top  -= GapY;

		DrawRect.right   -= GapX;
		DrawRect.bottom  -= GapY;
		
		if( i>=PARTS_RING1 && i<= PARTS_EARRING2 )
		{
			DrawRect.top    -= 5.0f;
			DrawRect.bottom -= 5.0f;

			sx = 0.8f;
			sy = 0.8f;
		} 
		
		if( pItemBase && pItemBase->m_pTexture)
		{
#ifdef __FINITEITEM0705
			if( ((CItemElem*)pItemBase)->IsFlag( CItemElem::expired ) )
			{
				pItemBase->m_pTexture->Render2(p2DRender, DrawRect.TopLeft(), D3DCOLOR_XRGB( 255, 100, 100 ), sx, sy );
			}
			else
#endif // __FINITEITEM0705
			{
				pItemBase->m_pTexture->RenderScal( p2DRender, DrawRect.TopLeft(), dwAlpha, sx, sy );
			}

			CItemElem *pItemElem = (CItemElem *)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )		// 묶음 아이템이냐?
			{
				short nItemNum	= pItemElem->m_nItemNum;

				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, "%d", nItemNum );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				int x = DrawRect.left;	
				int y = DrawRect.top;
				m_p2DRender->TextOut( x + 32 - size.cx, y + 32 - size.cy, szTemp, 0xff0000ff );
				m_p2DRender->TextOut( x + 31 - size.cx, y + 31 - size.cy, szTemp, 0xffb0b0f0 );
			}
		} 
	}

#endif //__YINVENTORY_3D

	return;
}

void CWndInventory::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __YINVENTORY_3D
	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM6 );
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM7 );
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM8 );
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM9 );
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl( WIDC_CUSTOM10 );
	
	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM11 );
	lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM12 );
	lpWndCtrl3 = GetWndCtrl( WIDC_CUSTOM13 );
	lpWndCtrl4 = GetWndCtrl( WIDC_CUSTOM14 );
	lpWndCtrl5 = GetWndCtrl( WIDC_CUSTOM15 );
	lpWndCtrl6 = GetWndCtrl( WIDC_CUSTOM16 );
	lpWndCtrl7 = GetWndCtrl( WIDC_CUSTOM17 );
	lpWndCtrl8 = GetWndCtrl( WIDC_CUSTOM18 );
	lpWndCtrl9 = GetWndCtrl( WIDC_CUSTOM19 );
	
	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;

	SAFE_DELETE( m_pModel );
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	
	
#else //__YINVENTORY_3D
	
	m_InvenRect[2] = m_InvenRect[15] = CRect(90, 137, 90+24, 137+24 );
	m_InvenRect[4] = m_InvenRect[17] = CRect(119, 164, 119+24, 164+24 );
	m_InvenRect[5] = m_InvenRect[18] = CRect(90, 190, 90+24, 190+24 );
	m_InvenRect[6] = m_InvenRect[14] = CRect(90, 85, 90+24, 85+24 );
		
	m_InvenRect[8] = CRect(121, 111, 121+24, 111+24 );
	m_InvenRect[9] = m_InvenRect[11] = CRect(148, 138, 148+24, 138+24 );
	m_InvenRect[10] = CRect(31, 140, 31+24, 140+24 );
	m_InvenRect[12] = CRect(58, 112, 58+24, 112+24 );
	m_InvenRect[13] = CRect(150, 190, 150+24, 190+24 );

	m_InvenRect[19] = CRect(90, 40, 90+24, 40+24 );

	m_InvenRect[20] = CRect(54, 81, 54+24, 81+24 );
	m_InvenRect[21] = CRect(132, 81, 132+24, 81+24 );
	m_InvenRect[22] = CRect(65, 60, 65+24, 60+24 );	
	m_InvenRect[23] = CRect(119, 60, 119+24, 60+24 );

	m_InvenRect[24] = CRect(25, 190, 25+24, 190+24 );
	m_InvenRect[25] = CRect(60, 164, 60+24, 164+24 );

#ifdef __YPARTS_EX	
	m_InvenRect[26] = CRect(32, 239, 32+24, 239+24 );
	m_InvenRect[27] = CRect(72, 239, 72+24, 239+24 );
	m_InvenRect[28] = CRect(110, 239, 110+24, 239+24 );
	m_InvenRect[29] = CRect(150, 239, 150+24, 239+24 );
	m_InvenRect[30] = CRect(93, 239, 93+24, 239+24 );	
#endif //__YPARTS_EX	

#endif //__YINVENTORY_3D
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
//	CWndButton*  pButton  = (CWndButton *) GetDlgItem( WIDC_GOLD );

	m_wndItemCtrl.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), pTabCtrl, 11 );
	m_wndItemCtrl.InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = GETTEXT( TID_GAME_ITEM );//"아이템";
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = "카드";
	tabTabItem.pWndBase = &m_wndCardCtrl;
	//pTabCtrl->InsertItem( 1, &tabTabItem );
  
	tabTabItem.pszText = "큐브";
	tabTabItem.pWndBase = &m_wndCubeCtrl;
	//pTabCtrl->InsertItem( 2, &tabTabItem );
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_GOLD );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_GOLD );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );

	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
#ifdef	__YSTATIC_MONEY1028
	pGoldNum->AddWndStyle(WSS_MONEY);
#endif //__YSTATIC_MONEY1028

#ifdef __YENCHANT_0608
	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_bIsUpgradeMode = FALSE;
	m_dwEnchantWaitTime = 0xffffffff;
#endif //__YENCHANT_0608
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 112 + 48 );
	Move( point );
}
BOOL CWndInventory::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// 인벤토리 출력 위치 지정
	memset( m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
//	return CWndNeuz::Create( WBS_VIEW | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME, rect, pWndParent, dwWndId );
}

BOOL CWndInventory::Process()
{
#ifdef __YINVENTORY_3D
	if( m_pModel )
		m_pModel->FrameMove();
#endif //__YINVENTORY_3D

#ifdef __ONLINE
#ifdef __YENCHANT_0608
	if( m_dwEnchantWaitTime < timeGetTime() )
	{
		m_dwEnchantWaitTime = 0xffffffff;
		
		if( m_pSfxUpgrade )
		{
			m_pSfxUpgrade->Delete();
			m_pSfxUpgrade = NULL;
		}
		
		CItemElem* pItemElem = (CItemElem*)m_pUpgradeItem;
		if( pItemElem && m_pUpgradeMaterialItem )
		{
			CItemElem* pItemMaterialElem = (CItemElem*)m_pUpgradeMaterialItem;
			// 패킷 전송
			
			if( pItemMaterialElem->GetProp() )
			{
				// 인첸트에 관한 아이템이냐?
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD || pItemMaterialElem->GetProp()->dwItemKind3 == IK3_DICE )
				{
					g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}
				else
				// 피어싱에 관한 아이템이냐?
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD )
				{
					g_DPlay.SendPiercing( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}
			}
		}
	}
#endif //__YENCHANT_0608
#endif // __ONLINE
	return TRUE;
}

BOOL CWndInventory::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	// 인벤토리로 아이템이 Drop되었다. 어디서 왔는지 확인한후 적절히 대응하라.
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if( lpShortcut->m_pFromWnd == NULL )
		{
			ADDERRORMSG( "CWndInventory::OnChildNotify : m_pFromWnd 가 NULL " );
			return CWndNeuz::OnChildNotify( message, nID, pLResult );
		}
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

		if( pWndFrame == NULL )
		{
			LPCTSTR szErr = Error( "CWndInventory::OnChildNotify : pWndFrame==NULL" );
			ADDERRORMSG( szErr );
		}
		BOOL bForbid = TRUE;
		if( pWndFrame && nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwData == 0 )
			{
				SetForbid( TRUE );
				return FALSE;
			}
			
			// 장착과 탈착이 이루어질 것이다.
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwIndex != II_GOLD_SEED1 )
			{
#ifndef __ONLINE
				g_pPlayer->DoUseItem( MAKELONG( ITYPE_ITEM, lpShortcut->m_dwId ), 0 );
#else

				int nPart = -1;
				if( m_pSelectItem )
					nPart = m_pSelectItem->m_dwObjIndex - MAX_INVENTORY;

				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, lpShortcut->m_dwId ), 0, nPart );
#endif
				bForbid = FALSE;
			}
			else
			// 숍에서 온거라면 구입 
			
			if( pWndFrame->GetWndId() == APP_SHOP_ )
			{
				CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
				for( int i = 0; i < pWndItemCtrl->GetSelectedCount(); i++ )
				{
					int nItem = pWndItemCtrl->GetSelectedItem( i );
					pWndItemCtrl->GetItem( nItem );
				}
				if( g_pPlayer->GetGold() - ((CItemElem*)lpShortcut->m_dwData)->GetCost() >= 0)
				{
					SAFE_DELETE( m_pWndConfirmBuy );
					m_pWndConfirmBuy = new CWndConfirmBuy;
					m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
					m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
					bForbid = FALSE;
				}
				else
				{
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0048) ) );
//					g_WndMng.OpenMessageBox( "페냐가 부족하여 살수 없습니다." );
				}
			}
			else if( pWndFrame->GetWndId() == APP_VENDOREX )
			{
				CWndVendor* pWndVendor	= (CWndVendor*)pWndFrame;

				SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
				pWndVendor->m_pWndVendorBuy		= new CWndVendorBuy( (CItemBase*)lpShortcut->m_dwData, lpShortcut->m_dwIndex/*nItem*/ );
				pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );

				bForbid		= FALSE;
			}
			else
			// 뱅크에서 온거라면 빼옴
			if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				CWndBase* pWndTaget = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[0]) )
					{					
						nSlot = 0;
					}
					else if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[1]) )
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}
					
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					
					for( int i = 0; i < pWndItemCtrl->GetSelectedCount(); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if( itemElem->m_nItemNum > 1 )
					{ 
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
#ifdef __ONLINE
						g_DPlay.SendGetItemBank( nSlot, lpShortcut->m_dwId, 1 );
#endif
					}
				}
				else
				{
					if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[0]) )
					{					
						nSlot = 0;
					}
					else if( pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[1]) )
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}

					// 페냐 (돈)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nSlot];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD;	// 인벤토리 인데 쓰고 있어서 Gold로 씀.
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					
					g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
					CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
				bForbid = FALSE;
			}
			else
		#ifdef __Y_POST_0912
			if( pWndFrame->GetWndId() == APP_POST_READ )
			{
				CWndPostRead* pWndPostRead = (CWndPostRead*)pWndFrame;
				
				if( pWndPostRead )
				{
					if( lpShortcut->m_dwData != 0 )
					{
						pWndPostRead->MailReceiveItem();
					}
					else
					{
						pWndPostRead->MailReceiveGold();
					}
					
					bForbid = FALSE;
				}
			}
			else	
		#endif //__Y_POST_0912				
			if( pWndFrame->GetWndId() == APP_GUILD_BANK )
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetItem( g_pPlayer->m_idPlayer ))
					{
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
						
						for( int i = 0; i < pWndItemCtrl->GetSelectedCount(); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// ****중요****
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							
							g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
						#ifdef __ONLINE							
							g_DPlay.SendGetItemGuildBank( lpShortcut->m_dwId, 1, 1 );
						#endif //__ONLINE
						}
					}
				}
				else
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetPenya( g_pPlayer->m_idPlayer ))
					{
						// 페냐 (돈)
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = pGuild->m_nGoldGuild;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// 인벤토리 인데 쓰고 있어서 Gold로 씀.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
				}
				bForbid = FALSE;
			}
		}
		SetForbid( bForbid );
	}
	else
	// 차일드로부터 더블클릭 알림 메시지가 왔다. 아이템 콘트롤에서 아이템을 사용하겠다는 메시지다.
	if( message == WIN_DBLCLK )
	{
		if( nID == 11 ) // item
		{
#ifdef __YENCHANT_0608
			if( m_dwEnchantWaitTime != 0xffffffff )
			{
				g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );
				return 0;
			}
#endif //__YENCHANT_0608
			
			CCtrl* pCtrl = (CCtrl*)g_WorldMng()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();
			CItemBase* pFocusItem = (CItemBase*) pLResult;
			BOOL	bAble = TRUE;
			if( pFocusItem )
			{
				ItemProp *pProp = pFocusItem->GetProp();
				// 소비아이템이 1개 남았을때 더블클릭하고 서버로 갔다가 
				// 삭제되기 전에 또 더블클릭하면 pFocusItem->m_dwItemId가 0이 되면서
				// 프로퍼티가 널되서 죽는다.
				if( pProp )
				{
#ifdef __YENCHANT_0608
					// 속성카드나 주나위를 더블클릭했을경우 인첸트모드로 변경한다.
					if(  (pProp->dwItemKind3 == IK3_ELECARD || pProp->dwItemKind3 == IK3_DICE ||
					     pProp->dwItemKind3 == IK3_SOCKETCARD) )
					{
						bAble = FALSE;
						m_bIsUpgradeMode = TRUE;
						m_pUpgradeMaterialItem = pFocusItem;	
						
						if((g_pPlayer->IsMode( TRANSPARENT_MODE ) ) == 0)
							m_pSfxUpgrade = CreateSfx( g_Neuz.m_pd3dDevice, XI_INT_INCHANT, g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
					}

#endif //__YENCHANT_0608
					
					if( pProp->dwItemKind1 == IK1_RIDE )	// 사용한 아이템이 탈것인가.
					{
						if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground상태인가?
							if( g_pPlayer->m_pActMover->m_bGround == 0 )		// 땅에 발을 안붙이고 있다면 못탄다.
								bAble = FALSE;	// 아이템사용 실패.
					}

					if( pFocusItem->m_dwItemId == II_SYS_SYS_MAP_FLARIS )
					{
						g_WndMng.OpenMap( "map_flaris.tga" );	// 파일이 없어서 잠시 막아둠
					}
					else
					if( pFocusItem->m_dwItemId == II_SYS_SYS_MAP_SAINTMORNING )
					{
						g_WndMng.OpenMap( "map_saintmorning.tga" );
					}
					
#ifdef __S050524_SM_ITEM
#ifdef __PIERCING_SM_TIME
					if( ((CItemElem*)pFocusItem)->m_bCharged != 1 )
#else // __PIERCING_SM_TIME
					if( pProp->dwItemKind1 == IK1_CHARGED )
#endif // __PIERCING_SM_TIME
					{
						switch( pProp->dwID )
						{
						case II_SYS_SYS_SCR_BLESSING:
							{
								if( g_pPlayer->IsSMMode( SM_REVIVAL ) )
								{
									g_WndMng.PutString( prj.GetText( TID_GAME_LIMITED_USE ), NULL, prj.GetTextColor(TID_GAME_LIMITED_USE) );
								}
								else
								{
									SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
									g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
									g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
									g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_DEPEN_USE, pFocusItem->m_dwObjId, dwObjId );
								}
								bAble = FALSE;
							}
							break;
						case II_SYS_SYS_SCR_RECCURENCE:
							{
								if( g_pPlayer->m_dwSkillPoint )
								{
									g_WndMng.PutString( prj.GetText( TID_RESKILLPOINT_NOTDIALOG ), NULL, prj.GetTextColor( TID_RESKILLPOINT_NOTDIALOG ) );									
								}
								else
								{
									if( g_pPlayer->IsBaseJob() )
									{
										g_WndMng.PutString( prj.GetText( TID_GAME_NOTUSEVAG ), NULL, prj.GetTextColor( TID_GAME_NOTUSEVAG ) );
									}
									else
									{
										SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
										g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
										g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
										g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
									}
								}
								bAble = FALSE;
							}
							break;
						case II_SYS_SYS_SCR_CHACLA:
							{
								if( g_pPlayer->IsBaseJob() )
								{
									g_WndMng.PutString( prj.GetText( TID_GAME_NOTUSEVAG ), NULL, prj.GetTextColor( TID_GAME_NOTUSEVAG ) );
									bAble = FALSE;
								}
								else
								{
									for( DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++ )
									{
										if( dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_RIDE )
											continue;
										CItemElem* pArmor	= g_pPlayer->m_Inventory.GetEquip( dwParts );
										if( pArmor )
										{
											g_WndMng.PutString( prj.GetText( TID_GAME_CHECK_EQUIP ), NULL, prj.GetTextColor( TID_GAME_CHECK_EQUIP ) );
											bAble = FALSE;
										}
									}
								}

								if( bAble )
								{
									SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
									g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
									g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
									g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_WARNINGCCLS, pProp->dwID, pProp->dwID );
									bAble = FALSE;
								}
							}
							break;
						}
					}
#endif // __S050524_SM_ITEM
				}
#ifdef _DEBUG
				else
				{
					int a = 0;
				}
#endif
			}
			if( bAble )	// 성공판정 났을때만 보냄.
			{
	#ifndef __ONLINE
				g_pPlayer->DoUseItem( MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId );
	#else	// __ONLINE
				if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
					g_WndMng.GetWndBase( APP_BANK )  ||
					g_WndMng.GetWndBase( APP_TRADE ) )
				{
					//g_WndMng.PutString( "거래중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
				else
				if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					//g_WndMng.PutString( "수리중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
				else
				{
					g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId );
				}
	#endif	// __ONLINE
			}
		}
		if( nID == 12 ) // card
		{
		}
		if( nID == 13 ) // cube
		{

		}
	}
	if( message == WNM_CLICKED )
	{
		switch( nID )
		{
			case 100: // icon
			//	m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
				break;
			//case 101: // report
			//	m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
			//	m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
			//	break;
			case WTBID_REPORT:
				/*
				if( m_bReport )
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				}
				else
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
				}
				m_bReport = !m_bReport;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
*/
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndInventory::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 0: // wnd1 
			g_WndMng.OpenWnd1();
			break;
		case 1: // wnd2
			g_WndMng.OpenCharacter();
			break;
		case 2: // 챗 
			g_WndMng.OpenChat();
			break;
		case 3:
			g_clientMsg.PutMessage(&g_Neuz.m_2DRender,0xffffffff,"fuck");
			break;
		case 9:
			g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndInventory::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//m_wndButton1.SetWndRect( rect3_1 );
	//m_wndButton2.SetWndRect( rect3_2 );
	//m_wndButton3.SetWndRect( rect3_3 );

	CRect rect = GetClientRect();
	rect.DeflateRect( 5, 5 );
	rect.top    += 187;//203;
	rect.bottom -= 20;
	//rect.bottom -= 2;//40;
	//m_wndListCtrl.SetWndRect( rect );
//	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );

	//rect.bottom += 20;
//	pTabCtrl->SetWndRect( rect );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndInventory::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __YINVENTORY_3D
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
#endif //__YINVENTORY_3D
	
}
void CWndInventory::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef __YINVENTORY_3D
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
#endif //__YINVENTORY_3D
		
	if( m_bLButtonDown )
	{
	#ifndef __YINVENTORY_3D
		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		CRect rectCtrl = lpWndCtrl->rect;
		
		int GapX, GapY;
		
		GapX = rectCtrl.left / 2;
		GapY = rectCtrl.top + 32;
	#endif //__YINVENTORY_3D		

		for( int i=2; i<MAX_HUMAN_PARTS; i++ )
		{
			CRect DrawRect = m_InvenRect[i];
			
	#ifndef __YINVENTORY_3D
			DrawRect.left -= GapX;
			DrawRect.top  -= GapY;
			
			DrawRect.right   -= GapX;
			DrawRect.bottom  -= GapY;
	#endif //__YINVENTORY_3D			

			if( DrawRect.PtInRect( point ) )
			{
				CItemBase* pItemBase = NULL;

				pItemBase = g_pPlayer->GetEquipItem( i );

				if( pItemBase )
				{
					m_pSelectItem  = (CItemElem*)pItemBase;
/*
#ifdef __YENCHANT_0608
					RunUpgrade( pItemBase );
#endif //__YENCHANT_0608
*/					
					return;
				}
			} 
		}
	}
	m_pSelectItem = NULL;
}

void CWndInventory::OnLButtonDblClk( UINT nFlags, CPoint point)
{
#ifndef __YINVENTORY_3D
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	CRect rectCtrl = lpWndCtrl->rect;
	
	int GapX, GapY;
	
	GapX = rectCtrl.left / 2;
	GapY = rectCtrl.top + 32;
#endif //__YINVENTORY_3D
	
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CRect DrawRect = m_InvenRect[i];

#ifndef __YINVENTORY_3D		
		DrawRect.left -= GapX;
		DrawRect.top  -= GapY;
		
		DrawRect.right   -= GapX;
		DrawRect.bottom  -= GapY;
#endif //__YINVENTORY_3D
		
		if( DrawRect.PtInRect( point ) )
		{
			CItemElem* pItemElem = NULL;

			pItemElem = (CItemElem*)g_pPlayer->GetEquipItem( i );
			
			if( pItemElem )
			{
#ifndef __ONLINE
				g_pPlayer->DoEquip( pItemElem, FALSE );
				CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_adwEquipment, g_pBipedMesh, &g_pPlayer->m_Inventory );
#else
				g_DPlay.SendDoEquip( pItemElem, i );		// 파츠번호를 같이 보낸다. 쌍칼땜시.
				// 장착인벤 영역을 더블클릭해서 벗기거나
				// 드래그&드롭으로 장착하려 하거나 벗기려고 할때 파츠번호를 보낸다.
#endif
			}
		}
	}
}

#ifdef	__NQUESTITEM_INFO_040917
void CWndInventory::OnRButtonDown(UINT nFlags, CPoint point)
{
	BaseMouseCursor();
}
#endif//__NQUESTITEM_INFO_040917

BOOL CWndInventory::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	SetEnchantCursor();

	return TRUE;
}

void CWndInventory::SetEnchantCursor()
{
#ifdef __YENCHANT_0608
	// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	if( m_bIsUpgradeMode )
	{
		SetMouseCursor( CUR_HAMMER );
	}
	else
	{
		SetMouseCursor( CUR_BASE );		
	}
#endif //__YENCHANT_0608	
}


void CWndInventory::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __YINVENTORY_3D	
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
#endif //__YINVENTORY_3D
	
	if( m_bLButtonDown && IsPush() )
	{
		if( m_pSelectItem && m_pSelectItem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId       = m_pSelectItem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_pTexture   = m_pSelectItem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectItem;
			_tcscpy( m_GlobalShortcut.m_szString, m_pSelectItem->GetProp()->szName);
		}
		else
		{

		}
	}
}
BOOL CWndInventory::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	
	if( g_WndMng.GetWndBase( APP_SHOP_ ) ||
		g_WndMng.GetWndBase( APP_BANK ) ||
		g_WndMng.GetWndBase( APP_TRADE ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			//g_WndMng.PutString( "거래중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
			return FALSE;
		}
	}
	else if( g_WndMng.GetWndBase( APP_REPAIR ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			SetForbid( TRUE );
			//g_WndMng.PutString( "수리중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
			
			return FALSE;
		}
	}
	
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM && pShortcut->m_pFromWnd != this && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData ) // dwData가 0이면 골드 
		{
			CItemElem* pItemElem = (CItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
			if( pItemElem && pItemElem->GetProp()->dwParts != NULL_ID ) //&& pItemElem->GetProp()->dwParts == i * 3 + j )
			{
#ifndef __ONLINE
				g_pPlayer->DoEquip( pItemElem, TRUE );
				CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_adwEquipment, g_pBipedMesh, &g_pPlayer->m_Inventory );
#else
				g_DPlay.SendDoEquip( pItemElem );
#endif
				return TRUE;
			}


		}
	}
	return CWndBase::OnDropIcon( pShortcut, point );
}
void CWndInventory::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( pWndChild == m_pWndConfirmBuy )
		SAFE_DELETE( m_pWndConfirmBuy );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 비디오 캡춰
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndVideoCapture::CWndVideoCapture()
{
}
CWndVideoCapture::~CWndVideoCapture()
{
}
void CWndVideoCapture::OnDraw(C2DRender* p2DRender)
{
	/*
	if(g_videoCapture.m_pGrabber)
	{
		long cbBuffer = 0;
		static char* pBuffer1 = NULL;
		//static char* pBuffer2 = NULL;
		//char* pBuffer;
		g_videoCapture.m_pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
		if(pBuffer1 == NULL && cbBuffer)
		{
			pBuffer1 = new char[cbBuffer];
			//pBuffer2 = new char[cbBuffer];
		}
		if(pBuffer1) 
		{
			g_videoCapture.m_pGrabber->GetCurrentBuffer(&cbBuffer, reinterpret_cast<long*>(pBuffer1));

			IDirect3DSurface8* pDestSurface;
			p2DRender->m_pd3dDevice->GetRenderTarget(&pDestSurface);
			//if(m_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pDestSurface) == D3D_OK)
			{
				D3DLOCKED_RECT lockedRect;
				CRect rectIn (0,0,cb.bih.biWidth,abs(cb.bih.biHeight));
				if(pDestSurface->LockRect(&lockedRect,rectIn,0) == D3D_OK)// Lock
				{
					LPWORD pWord = (LPWORD)lockedRect.pBits;
					LPBYTE pByte = (LPBYTE)pBuffer1;
					CPoint pt = p2DRender->m_ptOrigin;
					pt.x += 5;
					pt.y += 5;
					for(int y = abs(cb.bih.biHeight) - 1 + pt.y; y >= pt.y; y--)
					{
						for(int x = pt.x; x < cb.bih.biWidth + pt.x; x++)
						{
							WORD byData1 = ((WORD)*pByte++) >> 3;
							WORD byData2 = ((WORD)*pByte++) >> 3;
							WORD byData3 = ((WORD)*pByte++) >> 3;
							pWord[ y * ( lockedRect.Pitch / 2 ) + x ] = 
								( byData1  ) |
								( byData2  ) << 6 |
								( byData3  ) << 11 ;
						}
					}
					//g_pMemDC->SelectSurface(lockedRect.pBits,lockedRect.Pitch/2);
					//g_pWndMgr->Paint(g_pMemDC);
					pDestSurface->UnlockRect();

				}
			}
			pDestSurface->Release();
			//g_pMemDC->PaintImage(pBuffer1,CPtSz(0,0,cb.bih.biWidth,abs(cb.bih.biHeight)));
			//g_bOneShot = TRUE;
		} 
	}
	p2DRender->RenderRoundRect(CRect( 4, 4, 128 + 6, 96+6),D3DCOLOR_ARGB(255,150,150,250));
	*/
}
void CWndVideoCapture::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndVideoCapture::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	//CRect rect = m_pWndRoot->MakeCenterRect(250,130);
	CRect rect = m_pWndRoot->MakeCenterRect(142,128);
	//SetTitle( _T( "Video Capture" ) );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}

BOOL CWndVideoCapture::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndVideoCapture::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndVideoCapture::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndVideoCapture::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// EMAIL
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMail::CWndMail()
{
}

CWndMail::~CWndMail()
{

}
void CWndMail::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	// 테두리 박스 
	DWORD dwColor1t = D3DCOLOR_ARGB( 255,  91, 104, 205 );
	DWORD dwColor2t = D3DCOLOR_ARGB( 255, 116, 128, 220 );
	DWORD dwColor3t = D3DCOLOR_ARGB( 255, 250, 250, 255 );
	DWORD dwColor4t = D3DCOLOR_ARGB( 255, 143, 173, 245 );
	DWORD dwColor1b = D3DCOLOR_ARGB( 255,  41, 104, 155 );
	DWORD dwColor2b = D3DCOLOR_ARGB( 255,  66,  78, 170 );
	DWORD dwColor3b = D3DCOLOR_ARGB( 255, 200, 200, 210 );
	DWORD dwColor4b = D3DCOLOR_ARGB( 255, 143, 173, 245 );
	
	p2DRender->RenderRoundRect( rect, dwColor1t, dwColor1t, dwColor1b, dwColor1b );
	rect.DeflateRect(1,1);
	p2DRender->RenderRoundRect( rect, dwColor2t, dwColor2t, dwColor2b, dwColor2b );
	rect.DeflateRect(1,1);

	CRect rect1 = rect; rect1.bottom = rect1.top + 40;
	CRect rect2 = rect; rect2.top    = rect2.top + 40;

	p2DRender->RenderFillRect( rect1, dwColor3t, dwColor3t, dwColor3b, dwColor3b );
	p2DRender->RenderFillRect( rect2, dwColor3b, dwColor3b, dwColor4b, dwColor4b );

	p2DRender->RenderLine( CPoint( 10, 5 * 22 + 54 ), CPoint( 140, 5 * 22 + 54 ), dwColor1t );
	// client 영역 칠하기 
	//OnEraseBkgnd(p2DRender);
	//.OffsetRect(-rect.TopLeft());
	//p2DRender->PaintRect( rect,MKHIGHRGB(255>>3,255>>3,255>>3) );
	p2DRender->TextOut( 5, 5, _T( "Amazing World Navigatoe 1.0" ), dwColor1t );
	//p2DRender->TextOut( 5,20,"HP : 100 / 100", dwColor1t );
	//p2DRender->TextOut( 5,35,"MP : 100 / 100", dwColor1t );
}
void CWndMail::OnInitialUpdate()
{
//	m_wndMenu.Create("Vampyre",0,CRect(0,rect.Height()-500,150,rect.Height()),this,100);//,m_pSprPack,9);
/*
	for(int i = 0; i < 10; i++)
	{
		if(i < 5)
			m_pWndCommand[i].Create(" ",WBS_HIGHLIGHT,CRect(5,50+i*22,145,50+20+i*22),this,i);
		else
			m_pWndCommand[i].Create(" ",WBS_HIGHLIGHT,CRect(5,60+i*22,145,60+20+i*22),this,i);
	}
	m_pWndCommand[0].SetTitle("Inventory");
	m_pWndCommand[1].SetTitle("Character");
	m_pWndCommand[2].SetTitle("Equipment");
	m_pWndCommand[3].SetTitle("Status");
	m_pWndCommand[4].SetTitle("Skill");
	m_pWndCommand[5].SetTitle("프로그램");
	m_pWndCommand[6].SetTitle("설정");
	m_pWndCommand[7].SetTitle("도움말");
	m_pWndCommand[8].SetTitle("로그아웃");
	m_pWndCommand[9].SetTitle("종료");
	*/
}
BOOL CWndMail::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 0, rectWindow.Height() - (22 * 10 + 40 + 30), 150, rectWindow.Height() );

	//m_strMessage = lpszMessage;
	//m_dwType = nType;
	//SetTitle( _T( "Mail" ) );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_MODAL | WBS_SOUND | WBS_THICKFRAME, rect, &g_WndMng, dwWndId );
}

BOOL CWndMail::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	/*
	switch(nID)
	{
		case 0: // wnd1 
			g_WndMng.OpenWnd1();
			break;
		case 1: // wnd2
			g_WndMng.OpenCharacter();
			break;
		case 2: // 챗 
			g_WndMng.OpenChat();
			break;
		case 3:
			g_clientMsg.PutMessage(&g_Neuz.m_2DRender,0xffffffff,"fuck");
			break;
		case 9:
			g_WndMng.OpenCustomBox("종료하시겠습니까?",new CWndExitBox);
			break;
	}
	*/
	return TRUE;
}
void CWndMail::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndMail::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{//
	//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
		//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndMail::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 캐릭터 정보  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndCharacter::CWndCharacter()
{
}
CWndCharacter::~CWndCharacter()
{
}
void CWndCharacter::OnDraw(C2DRender* p2DRender)
{
}
void CWndCharacter::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		int nCurSel;
		ar >> nCurSel;
		lpTabCtrl->SetCurSel( nCurSel );
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}
void CWndCharacter::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	Move( point );
	
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;
	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
	m_wndStateBase.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 100000 );
	m_wndStateDetail.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 1000001 );
	m_wndPvp.Create( WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 1000002 );
	
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_BASE);//"기본";
	tabTabItem.pWndBase = &m_wndStateBase;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	//lpTapCtrl->InsertTexture( 0 ,MakePath( DIR_THEME, "TabCharacter1.bmp" ) );

	tabTabItem.pszText = prj.GetText(TID_APP_CHARACTER_DETAIL);//"자세히";
	tabTabItem.pWndBase = &m_wndStateDetail;
	lpTapCtrl->InsertItem( 1, &tabTabItem );
	//lpTapCtrl->InsertTexture( 1 ,MakePath( DIR_THEME, "TabCharacter2.bmp" ) );

	
	tabTabItem.pszText = prj.GetText(TID_APP_PVP);//"PVP";
	tabTabItem.pWndBase = &m_wndPvp;

	lpTapCtrl->InsertItem( 2, &tabTabItem );
	//lpTapCtrl->InsertTexture( 2 ,MakePath( DIR_THEME, "TabCharacter3.bmp" ) );

	lpTapCtrl->SetCurSel( 1 );

	lpTapCtrl->SetButtonLength( 55 );

}
BOOL CWndCharacter::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 240, 0, 240 + 260, 255 - 105 + 20 ); 
	CRect rect( 0, 90, WND_WIDTH, 260 );//255 - 105 + 20 ); 
	//CRect rect( 50, 50, 50+210, 50+300 );
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	//return CWndNeuz::Create(WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndCharacter::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
	/*f( nID == WTBID_EXTENSION )
		{
			CRect rect = GetWindowRect( TRUE ); 
			if( m_wndTitleBar.GetTitleBarButton( WTBID_EXTENSION )->GetCheck() )
				rect.bottom += 135;
			else
				rect.bottom -= 135;
			SetWndRect( rect );
		}
		*/
	}	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCharacter::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCharacter::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25 );

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndCharacter::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCharacter::OnLButtonDown(UINT nFlags, CPoint point)
{
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CWndStateConfirm::CWndStateConfirm() 
{ 
	m_nId = 0;
} 
CWndStateConfirm::~CWndStateConfirm() 
{ 
} 
void CWndStateConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndStateConfirm::OnSetState( UINT nId )
{
	m_nId = nId;
}
void CWndStateConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
//	strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST)
	CString strMessage;
	switch( m_nId )
	{
	case 100:	// str
		strMessage.Format( prj.GetText( TID_DIAG_0076 ) );
		break;
	case 101:	// sta
		strMessage.Format( prj.GetText( TID_DIAG_0077 ) );
		break;
	case 102:	// dex
		strMessage.Format( prj.GetText( TID_DIAG_0078 ) );
		break;
	case 103:	// int
		strMessage.Format( prj.GetText( TID_DIAG_0079 ) );
		break;
	default:
		break;
	}

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	// 1차 전직인지 확인
	if( g_pPlayer->IsBaseJob() == TRUE )
	{
		// 방랑자
		CString strAddMessage;
		strAddMessage.Format( prj.GetText( TID_DIAG_0082 ) );
		strMessage += '\n';
		strMessage += strAddMessage;
	}
	pWndText->SetString( strMessage );
	pWndText->EnableWindow( FALSE );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndStateConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_STATE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndStateConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndStateConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndStateConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndStateConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndStateConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndStateConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		SendYes();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER );
		if( pWndBase ) {
			( (CWndCharacter*)pWndBase )->m_wndStateDetail.m_fWaitingConfirm	= FALSE;
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndStateConfirm::SendYes( void )
{
#ifdef __ONLINE
	g_DPlay.SendIncStatLevel( (CHAR)m_nId );
#endif
	Destroy();
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


CWndCharacterBase::CWndCharacterBase()
{
}
CWndCharacterBase::~CWndCharacterBase()
{
}
void CWndCharacterBase::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetClientRect();
	rect.bottom -= 30;
//	p2DRender->RenderFillRect( rect, 0xff00ffff );
	int y = 0, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
//	TCHAR szString[ 32 ];
//	p2DRender->TextOut( 5, y, _T( "Name"  ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Style" ) ); y += nNext;
//	p2DRender->TextOut( 5, y, _T( "Job"   ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Guild" ) ); y += nNext;
//	p2DRender->TextOut( 5, y, _T( "Level" ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp1." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Exp2." ) ); y += nNext;
	//p2DRender->TextOut( 5, y, _T( "Gold"  ) ); y += nNext;

	y = 10;

	if( g_Option.m_nLanguage == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;

	#ifdef __RENDER_JOBLV0527
//		p2DRender->TextOut( 80, y, g_pPlayer->GetJobLevel()      , dwColor ); y += nNext;
		y = 81;
	#else
		y = 70;
	#endif
	}
	else
	{
		p2DRender->TextOut( 65, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 65, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
		p2DRender->TextOut( 65, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;
		
#ifdef __RENDER_JOBLV0527
//		p2DRender->TextOut( 65, y, g_pPlayer->GetJobLevel()      , dwColor ); y += nNext;
		y = 81;
#else
		y = 70;
#endif
	}

	p2DRender->TextOut( 96, y, g_pPlayer->GetFlightLevel()	, dwColor ); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[ g_pPlayer->GetFlightLevel() + 1 ].dwFxp;
		sprintf( szBuff, "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp );
		p2DRender->TextOut( 90, y, szBuff, dwColor ); y += nNext;
	}
}
void CWndCharacterBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	/*
	CString strDir = "Theme\\";
	switch( prj.m_nLanguage )
	{
	case LANG_KOR:
		strDir += "Korean\\";
		break;
	case LANG_ENG:
		strDir += "English\\";
		break;
	case LANG_JAP:
		strDir += "Japanese\\";
		break;
	case LANG_CHI:
		strDir += "Chinese\\";
		break;
	case LANG_THA:
		strDir += "Thailand\\";
		break;
	case LANG_TWN:
		strDir += "Taiwan\\";
		break;
	}

	SetTexture( m_pApp->m_pd3dDevice, MakePath( strDir, _T( "wndCharacter1.tga" ) ), TRUE );
	*/
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", prj.m_nLanguage, _T( "wndCharacter1.tga" ) ), TRUE );
	FitTextureSize();
	
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

//	m_txInfo.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "txInfo.bmp" ) ), 0 );
/*
	y = 53;
	m_wndStr.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 100 ); y += 13;
	m_wndSta.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 101 ); y += 13;
	m_wndDex.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 102 ); y += 13;
	m_wndInt.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 103 ); y += 13;

	m_wndStr.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );
	m_wndSta.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );
	m_wndDex.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );
	m_wndInt.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "wndPlus.bmp" ) ), TRUE );

	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
*/	
	MakeVertexBuffer();
}
BOOL CWndCharacterBase::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndCharacterBase::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCharacterBase::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCharacterBase::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
}
void CWndCharacterBase::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCharacterBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CWndCharacterDetail::CWndCharacterDetail()
{
	if( !g_WndMng.m_pWndStateConfirm )
		m_fWaitingConfirm	= FALSE;

}
CWndCharacterDetail::~CWndCharacterDetail()
{
}

void CWndCharacterDetail::RenderATK( C2DRender* p2DRender, int x, int y )
{
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szText[32];
	int nMin, nMax;
	g_pPlayer->GetHitMinMax( &nMin, &nMax );
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK *= 1.2f;

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	wsprintf( szText, "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail::OnDraw(C2DRender* p2DRender)
{
	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
//	TCHAR szString[ 32 ];

	y = 15;
	int x = 5, nNextX = 100;
	//p2DRender->TextOut( x, y, _T( "Attack" ) ); y += nNext;
	//p2DRender->TextOut( x, y, _T( "Parrying" ) ); y += nNext;

	x = 50; y = 10;
	RenderATK( p2DRender, x, y );
	y += nNext;

	p2DRender->TextOut( x, y, g_pPlayer->GetParrying(), dwColor ); y += nNext;

	x = 100; y = 5;
	//p2DRender->TextOut( x, y, _T( "Defense" ) ); y += nNext;
	//p2DRender->TextOut( x, y, _T( "Resist"  ) ); y += nNext;

	x = 125; y = 10;
//	p2DRender->TextOut( x , y, g_pPlayer->m_nDefenseMin + g_pPlayer->m_nDefenseMax / 2, dwColor ); y += nNext;
	
	p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext;
	p2DRender->TextOut( x , y, g_pPlayer->GetResistMagic()  , dwColor ); y += nNext;

	x =15; y = 40; nNextX = 60;
	//p2DRender->TextOut( x, y, _T( "Attack Rate" ) ); y += nNext + 5;
	//p2DRender->TextOut( x, y, _T( "HOR" ) ); y += nNext;
	//p2DRender->TextOut( x, y, _T( "VER" ) ); y += nNext;
	//p2DRender->TextOut( x, y, _T( "DIA" ) ); y += nNext;
	//p2DRender->TextOut( x, y, _T( "THR" ) ); y += nNext;
	// 아래부터 능력치 관련 
	y = 52;
	
#ifdef __YSTAT_COLOR
	if( g_pPlayer->m_nStr == g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nStr < g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);
#endif //__YSTAT_COLOR
	p2DRender->TextOut( 65, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

#ifdef __YSTAT_COLOR
	if( g_pPlayer->m_nSta == g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nSta < g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);
#endif //__YSTAT_COLOR
	p2DRender->TextOut( 65, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

#ifdef __YSTAT_COLOR
	if( g_pPlayer->m_nDex == g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nDex < g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);
#endif //__YSTAT_COLOR
	p2DRender->TextOut( 65, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

#ifdef __YSTAT_COLOR
	if( g_pPlayer->m_nInt == g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_nInt < g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);
#endif //__YSTAT_COLOR
	p2DRender->TextOut( 65, y, g_pPlayer->GetInt(), dwColor ); y += nNext;

	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	if( m_fWaitingConfirm  == FALSE ) {
		m_wndStr.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwStr ) ? TRUE : FALSE );
		m_wndSta.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwSta ) ? TRUE : FALSE );
		m_wndDex.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwDex ) ? TRUE : FALSE );
		m_wndInt.EnableWindow( ( (int)g_pPlayer->GetRemainGP() >= (int)pJobAbillity->dwInt ) ? TRUE : FALSE );
		
	}
	else {
		m_wndStr.EnableWindow( FALSE );
		m_wndSta.EnableWindow( FALSE );
		m_wndDex.EnableWindow( FALSE );
		m_wndInt.EnableWindow( FALSE );
	}
	y = 52;
	p2DRender->TextOut( 135, y, pJobAbillity->dwStr, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwSta, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwDex, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwInt, dwColor ); y += nNext;
	
	p2DRender->TextOut( 105, y, g_pPlayer->GetRemainGP(), dwColor ); y += nNext;
}
void CWndCharacterDetail::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	/*
	CString strDir = "Theme\\";
	switch( prj.m_nLanguage )
	{
	case LANG_KOR:
		strDir += "Korean\\";
		break;
	case LANG_ENG:
		strDir += "English\\";
		break;
	case LANG_JAP:
		strDir += "Japanese\\";
		break;
	case LANG_CHI:
		strDir += "Chinese\\";
		break;
	case LANG_THA:
		strDir += "Thailand\\";
		break;
	case LANG_TWN:
		strDir += "Taiwan\\";
		break;
	}

	SetTexture( m_pApp->m_pd3dDevice, MakePath( strDir, _T( "wndCharacter2.tga" ) ), TRUE );
	*/
	SetTexture( m_pApp->m_pd3dDevice, MakePath( "Theme\\", prj.m_nLanguage, _T( "wndCharacter2.tga" ) ), TRUE );
	FitTextureSize();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );


	// 아래부터 능력치 관련 
	y = 52;
	m_wndStr.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 100 ); y += 14;
	m_wndSta.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 101 ); y += 14;
	m_wndDex.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 102 ); y += 14;
	m_wndInt.Create  ( "<", 0, CRect( 100,   y, 145 + 20, y + 16 ), this, 103 ); y += 14;

	m_wndStr.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndSta.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDex.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndInt.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "ButtCharPlus.bmp" ) ), TRUE );

	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];
	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );

}
BOOL CWndCharacterDetail::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}
BOOL CWndCharacterDetail::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
#ifndef __ONLINE
		switch( nID )
		{
			case 100: g_pPlayer->IncStrLevel(); break;
			case 101: g_pPlayer->IncStaLevel(); break;
			case 102: g_pPlayer->IncDexLevel(); break;
			case 103: g_pPlayer->IncIntLevel(); break;
		}
#else	// __ONLINE
		if( m_fWaitingConfirm == FALSE ) {
			m_fWaitingConfirm	= TRUE;
			SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
			g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
			g_WndMng.m_pWndStateConfirm->OnSetState( nID );
			g_WndMng.m_pWndStateConfirm->Initialize();
		}
#endif	// __ONLINE
		/*
		if( nID == WTBID_EXTENSION )
		{
			CRect rect = GetWindowRect( TRUE ); 
			if( m_wndTitleBar.m_awndButton[ WTBID_EXTENSION ].GetCheck() )
				rect.bottom += 135;
			else
				rect.bottom -= 135;
			SetWndRect( rect );
		}
		*/
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCharacterDetail::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCharacterDetail::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize( 70, 25 );

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndBase::OnSize(nType,cx,cy);
}
void CWndCharacterDetail::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}
void CWndCharacterDetail::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 직업  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndChangeJob::CWndChangeJob( int nJob ) 
{ 
	m_nJob = nJob;
} 
CWndChangeJob::~CWndChangeJob() 
{ 
} 
void CWndChangeJob::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChangeJob::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	for( int i  = 0 ; i < 6 ; ++i )
	{
		m_wndExpert[ i ].SetFontColor( 0xffffffff );
	}
	m_wndOk.SetFontColor( 0xffffffff );
	m_wndCancel.SetFontColor( 0xffffffff );

	if( m_nJob == 0 )	{
		m_wndExpert[ 0 ].Create( prj.m_aJob[ MAX_JOBBASE + 0 ].szName, 0, CRect( 10, 10, 230, 35 ), this, 10 );
		m_wndExpert[ 1 ].Create( prj.m_aJob[ MAX_JOBBASE + 1 ].szName, 0, CRect( 10, 40, 230, 65 ), this, 11 );
		m_wndExpert[ 2 ].Create( prj.m_aJob[ MAX_JOBBASE + 2 ].szName, 0, CRect( 10, 70, 230, 95 ), this, 12 );
		m_wndExpert[ 3 ].Create( prj.m_aJob[ MAX_JOBBASE + 3 ].szName, 0, CRect( 10, 100, 230, 125 ), this, 13 );
		m_wndExpert[ 4 ].Create( prj.m_aJob[ MAX_JOBBASE + 4 ].szName, 0, CRect( 10, 130, 230, 155 ), this, 14 );
	}
	else	{
		m_wndExpert[ 0 ].Create( prj.m_aJob[ m_nJob * 2 + 4 ].szName, 0, CRect( 10, 10, 230, 35  ), this, 10 ); 
		m_wndExpert[ 1 ].Create( prj.m_aJob[ m_nJob * 2 + 5 ].szName, 0, CRect( 10, 40, 230, 65  ), this, 11 ); 
	}
	m_wndOk.Create( _T( "OK" )  , 0, CRect( 60, 180, 110, 200), this, IDOK ); 
	m_wndCancel.Create( _T( "Cancel" ) , 0, CRect( 130, 180, 180, 200), this, IDCANCEL ); 

} 
BOOL CWndChangeJob::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	//SetTitle( _T( "전직" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, dwWndId ); 
} 
BOOL CWndChangeJob::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeJob::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeJob::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeJob::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeJob::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_CLICKED )
	{
		if( 10 <= nID && nID <= 14 )
		{
#ifdef __ONLINE
			if( g_pPlayer->IsBaseJob() )
			{
				if( 10 == nID || 11 == nID || 12 == nID || 13 == nID )
				{
					g_DPlay.SendChangeJob( nID - 9 );
				}
				else
				{
					g_WndMng.PutString( "Not Change Job" );
				}
			}
			else
			if( g_pPlayer->IsExpert() )
			{
				if( nID == 10 )
					g_DPlay.SendChangeJob( m_nJob * 2 + 4 );
				else
				if( nID == 11 )
					g_DPlay.SendChangeJob( m_nJob * 2 + 5 );
			}
#else
			#ifdef __Y0216
				if( g_pPlayer->m_pActMover && ( g_pPlayer->m_pActMover->IsState( OBJSTA_STAND ) || g_pPlayer->m_pActMover->IsState( OBJSTA_STAND2 )) )
					g_pPlayer->SetMotion( MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED );
			#endif // __Y0216
			CreateSfx(g_Neuz.m_pd3dDevice,XI_GEN_LEVEL_UP01,g_pPlayer->GetPos(),g_pPlayer->GetId());
			//PlayMusic( BGM_INSTANT04 );
			if( g_pPlayer->SetExpert( nID - 9 ) )
			{

				CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL );
				
				if( pSkillTree )
					pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );
			}
#endif
			Destroy();
		}
		else
		{
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 




/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 인벤토리 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndSkill::CWndSkill()
{
	m_bReport = FALSE;
}
CWndSkill::~CWndSkill()
{
}
void CWndSkill::OnDraw(C2DRender* p2DRender)
{
}
/*
MERCENARY    
KNIGHT       
SPEARMAN     
TUMBLER      
PIERROT      
BEASTMASTER  
HEALER       
FORTUNETELLER
RINGMASTER   
JACKMETA     
JACKFLAM     
JACKEMITER   
PUPPETEER    
STRONGMAN    
STRINGMASTER 
*/

void CWndSkill::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CRect rectJob( 0, 0, 32, 32 );

	rectJob += CPoint( 6, 5 );

	// 탭 콘트롤 생성 
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	//lpTabCtrl->Create( 0, CRect( 0, 0, 250, 250 ), this, 12 );
	lpTabCtrl->AddWndStyle( WBS_NODRAWFRAME );

	WTCITEM tabTabItem;
	// 스킬 콘트롤 생성 

	m_wndSkillCtrl[ 0 ].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), lpTabCtrl, 11 );
	m_wndSkillCtrl[ 0 ].AddWndStyle( WBS_NODRAWFRAME );
	m_wndSkillCtrl[ 0 ].InitItem( g_pPlayer->m_nJob, g_pPlayer->m_nJob, g_pPlayer->m_aJobSkill );
	m_wndSkillCtrl[ 0 ].SetVisible( FALSE );
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = _T( prj.GetText(TID_GAME_TOOLTIP_JOBSKILL) ); //"직업스킬" );//g_szJobName[ g_pPlayer->GetJob() ];//"머셔너리";
	tabTabItem.pWndBase = &m_wndSkillCtrl[ 0 ];
	lpTabCtrl->InsertItem( 0, &tabTabItem );

	m_wndSkillCtrl[ 0 ].SetVisible( TRUE ); 
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 112 + 48 );
	Move( point );
}
BOOL CWndSkill::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 50, 50, 50 + 210, 50 + 300 );
	//CRect rect( 0, 260, WND_WIDTH, 560 );
//	CRect rect( 814, 129, 814+210, 129+300 );
	//CRect rect( 792-224, 130, 792 + 232 - 224, 130 + 405 + 20 ); // 800 600
	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768
	//m_wndUserArray.SetSize(5);
	//SetTitle( GETTEXT( TID_APP_SKILL )  );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
//		return CWndNeuz::Create( WBS_VIEW | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME, rect, pWndParent, dwWndId );
}
BOOL CWndSkill::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
	{
		if( nID >= 100 && nID <= 105 )
		{
			for( int i = 0; i < MAX_JOB; i++ )
				m_wndSkillCtrl[ i ].SetVisible( FALSE );
			m_wndSkillCtrl[ nID - 100 ].SetVisible( TRUE );
		}
		switch( nID )
		{
		case 100:
			break;
		case 101:
			break;
		case 102:
			break;
		case 103:
			break;
		case 104:
			break;
		case 105:
			break;
		}
		/*
		switch( nID )
		{
			case 100: // icon
				m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
				break;
			case 101: // report
				m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );
				break;
			case WTBID_REPORT:
				if( m_bReport )
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_ICON;
				}
				else
				{
					m_wndItemCtrl.m_dwListCtrlStyle = WLVS_REPORT;
				}
				m_bReport = !m_bReport;
				m_wndItemCtrl.SetWndRect( m_wndItemCtrl.GetWindowRect( TRUE ) );

				break;
				*/
				/*
#define WTBID_CLOSE  10000
#define WTBID_MAX    10001
#define WTBID_MIN    10002
#define WTBID_HELP   10003
#define WTBID_REPORT 10004
#define WTBID_PIN    10005
*/
		//}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndSkill::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSkill::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CRect rect = GetClientRect();
	rect.DeflateRect( 4, 4 );
	//rect.top += 40;
	//m_wndListCtrl.SetWndRect( rect );
//	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

	//rect.bottom += 20;
//	lpTabCtrl->SetWndRect( rect );

	//for( int i = 0; i < 2; i++ )
	//{
	//	m_wndSkillCtrl[ i ].SetWndRect( rect );
	//}
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndSkill::SetExpert( int nExpert )
{
	// 직업 스킬 i = 0, 라이선스 스킬 i = 1
	for( int i = 0; i < 1; i++ )
	{
		m_wndSkillCtrl[ i ].SetExpert( nExpert );
	}
}

void CWndSkill::SetPro( int nProJob )
{
	for( int i = 0; i < 1; i++ )
	{
		m_wndSkillCtrl[ i ].SetPro( nProJob );
	}
}
void CWndSkill::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndSkill::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndSkill::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}


CWndSkillTreeEx::CWndSkillTreeEx()
{
	m_nCount     = 0;
//	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_bDrag = FALSE;
	m_apSkill = NULL;
	m_nJob = -1;
	m_pVBGauge = NULL;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
		m_atexSkill[ i ] = NULL;

	m_aSkillLevel[ 0 ] = NULL;
	m_aSkillLevel[ 1 ] = NULL;
	m_aSkillLevel[ 2 ] = NULL;

	m_atexJobPannel[ 0 ] = NULL;
	m_atexJobPannel[ 1 ] = NULL;

#ifdef __YTREE_TOPDOWN_050103
	m_atexTopDown[0] = NULL;
	m_atexTopDown[1] = NULL;
	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
#endif //__YTREE_TOPDOWN_050103	

#ifdef __YADV_RESKILLPOINT
	m_pWndReSkillControl = NULL;
	m_dwCurrSkillPoint = 0;
#endif //__YADV_RESKILLPOINT
}
CWndSkillTreeEx::~CWndSkillTreeEx()
{
	DeleteDeviceObjects();
}

#ifdef __YTREE_TOPDOWN_050103
void CWndSkillTreeEx::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		ar >> m_bSlot[0] >> m_bSlot[1] >> m_bSlot[2];

		BOOL bFlag[3];

		bFlag[0] = m_bSlot[0];
		bFlag[1] = m_bSlot[1];
		bFlag[2] = m_bSlot[2];
		
		m_bSlot[0] = m_bSlot[1] = m_bSlot[2] = TRUE;

		SetActiveSlot( 0, bFlag[0] );
		SetActiveSlot( 1, bFlag[1] );
		SetActiveSlot( 2, bFlag[2] );
	}
	else
	{
		dwVersion = 0;
		ar << m_bSlot[0] << m_bSlot[1] << m_bSlot[2] ;
	}
}

void CWndSkillTreeEx::SetActiveSlot(int nSlot, BOOL bFlag)
{
	CRect rect = GetWindowRect(TRUE);

	if( nSlot == 0 )
	{
		if( m_bSlot[0] == bFlag )
			return;

		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );

		DWORD dwValue = 48;

		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, dwValue );
			lpWndStatic->SetWndRect( crect, TRUE );

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC4 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );

			int n = rect.Height();
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, -dwValue );
			lpWndStatic->SetWndRect( crect, TRUE );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC4 );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC8 );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);

			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

			int n = rect.Height();
			rect.bottom -= dwValue;

		}

		SetWndRect(rect);

		m_bSlot[0] = bFlag;
	}
	else
	if( nSlot == 1 )
	{
		if( m_bSlot[1] == bFlag )
			return;

		DWORD dwValue = 144;
		
		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			lpWndStatic->SetVisible(TRUE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top+dwValue );
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC5 );
			lpWndStatic->SetVisible(FALSE);
			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );

			
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			crect = lpWndStatic->GetWindowRect(TRUE);
			lpWndStatic->Move( crect.left, crect.top-dwValue );
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[1] = bFlag;
	}
	else
	if( nSlot == 2 )
	{
		if( m_bSlot[2] == bFlag )
			return;

		DWORD dwValue = 144;
		
		if( bFlag )
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(TRUE);
			
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(FALSE);
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[2] = bFlag;
	}

	if( rect.bottom > FULLSCREEN_HEIGHT-48 )
	{
		rect.OffsetRect( 0, (int)(FULLSCREEN_HEIGHT-48)-rect.bottom );
		SetWndRect(rect);
	}
		
	AdjustWndBase();
}

#endif //#ifdef __YTREE_TOPDOWN_050103

#ifdef __YADV_RESKILLPOINT
BOOL CWndSkillTreeEx::IsDownPoint(DWORD skill)
{
	LPSKILL pSkill = GetdwSkill( skill );

	if( pSkill == NULL )
		return FALSE;

	if( pSkill->dwLevel == 1 )
		return FALSE;

	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == NULL_ID )
			continue;
	
		LPSKILL pSkill2 = &m_apSkill[i];

		if( pSkill2 == NULL )
			continue;

		if( pSkill2->GetProp()->dwReSkill1 == skill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel1 )
			{
				if( pSkill->dwLevel-1 < pSkill2->GetProp()->dwReSkillLevel1 )
				{
					if( pSkill2->dwLevel > 1 )
					{
						g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_RESKILLPOINT_TEXT5) );
						return FALSE;
					}
				}
			}
		}
			
		if( pSkill2->GetProp()->dwReSkill2 == skill )
		{
			if( pSkill2->GetProp()->dwReSkillLevel2 )
			{
				if( pSkill->dwLevel-1 < pSkill2->GetProp()->dwReSkillLevel2 )
				{
					if( pSkill2->dwLevel > 1 )
					{
						g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_RESKILLPOINT_TEXT5) );
						return FALSE;
					}
				}
			}
		}
	}
	
	return TRUE;
}

LPSKILL CWndSkillTreeEx::GetdwSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == dwSkill )
			return &m_apSkill[i];
	}
	return NULL;
}

#endif //__YADV_RESKILLPOINT

LPSKILL CWndSkillTreeEx::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];
	
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	int nJobLevel = g_pPlayer->m_aJobLv[g_pPlayer->GetJob()];
	if( pSkill->GetProp() == NULL )
	{
		Message( "GetSkill Error Not Prop : %d", i );
		return NULL;
	}
	
	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}
void CWndSkillTreeEx::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();
	
	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );
	
	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );
	
	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );
	
	CRect rect = GetClientRect();
	rect.DeflateRect( 4, 4 );
	//rect.top += 40;
	//m_wndListCtrl.SetWndRect( rect );
	//	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	
	//rect.bottom += 20;
	//	lpTabCtrl->SetWndRect( rect );
	
	//for( int i = 0; i < 2; i++ )
	//{
	//	m_wndSkillCtrl[ i ].SetWndRect( rect );
	//}
	CWndNeuz::OnSize(nType,cx,cy);
}

BOOL CWndSkillTreeEx::CheckSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];

	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL || pSkillProp->nLog == 1 )
		return FALSE;

	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
		return FALSE;
				
	if( pSkillProp->dwReSkill1 != 0xffffffff )
	{
#ifdef __YADV_RESKILLPOINT
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill1 );
#else //__YADV_RESKILLPOINT
		LPSKILL pSkillBuf = g_pPlayer->GetSkill( pSkillProp->dwReSkill1 );
#endif //__YADV_RESKILLPOINT

		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
			//ssage( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
		}
	}
	
	if( pSkillProp->dwReSkill2 != 0xffffffff )
	{
#ifdef __YADV_RESKILLPOINT
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReSkill2 );
#else //__YADV_RESKILLPOINT
		LPSKILL pSkillBuf = g_pPlayer->GetSkill( pSkillProp->dwReSkill2 );
#endif //__YADV_RESKILLPOINT
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
			//ssage( "DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2 );
		}
	}	
	
	return TRUE;
}

HRESULT CWndSkillTreeEx::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndSkillTreeEx::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndSkillTreeEx::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );

	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );
	
#ifdef __YADV_RESKILLPOINT
	SAFE_DELETE(m_pWndReSkillControl);

	if( g_pPlayer->m_dwSkillPoint > 0 )
		delete [] m_apSkill;
	else
#endif //__YADV_RESKILLPOINT
	m_apSkill = NULL;
	
	return S_OK;
}

void CWndSkillTreeEx::InitItem(int nJob, LPSKILL apSkill)
{
#ifdef __YADV_RESKILLPOINT
	if( g_pPlayer == NULL )
		return;

	if( g_pPlayer->m_dwSkillPoint > 0 )
	{
		m_dwCurrSkillPoint = g_pPlayer->m_dwSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy( m_apSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );

		if( !m_pWndReSkillControl )
		{
			SAFE_DELETE( m_pWndReSkillControl );
			m_pWndReSkillControl = new CWndReSkillControl;
			m_pWndReSkillControl->Initialize();
		}
	}
	else
	{
		m_dwCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}
#else //__YADV_RESKILLPOINT	
	m_apSkill = apSkill;
#endif //__YADV_RESKILLPOINT
	m_pFocusItem = NULL;
	m_nJob = nJob;

	if( m_apSkill == NULL )
		return;


	m_nCount = 0;
	CString strTex[2];
	
	CWndStatic* lpWndStatic1 = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	CWndStatic* lpWndStatic2 = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
	CWndStatic* lpWndStatic3 = (CWndStatic*) GetDlgItem( WIDC_STATIC6 );

	switch( m_nJob )
	{
	case JOB_VAGRANT:
		m_nCount  = 3;
		strTex[0] = "";//"Back_Upper.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( "" );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_KNIGHT:
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Night.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		break;			
	case JOB_BLADE:
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Blade.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		break;			
	case JOB_MERCENARY:
		m_nCount  = 18;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_BILLPOSTER:
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Bill.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		break;			
	case JOB_RINGMASTER:
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Ring.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		break;			
	case JOB_ASSIST:
		m_nCount  = 20;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( "" );
		break;			
	case JOB_ELEMENTOR:
		m_nCount  = 39;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Ele.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		break;			
	case JOB_PSYCHIKEEPER:
		m_nCount  = 28;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Psy.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		break;			
	case JOB_MAGICIAN:
		m_nCount  = 20;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
#ifdef __ACROBAT_0504
	case JOB_ACROBAT:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
	case JOB_JESTER:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Jst.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		break;	
	case JOB_RANGER:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Rag.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		break;			
#endif //__ACROBAT_0504		
	default:
		Error( "CWndSkillTreeEx::InitItem 알수없는 직업 : %d", m_nJob );
		return;
	}
/*
	m_atexJobPannel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, strTex[0] ), 0xffff00ff );
	m_atexJobPannel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, strTex[1] ), 0xffff00ff );
*/
	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );

	if( !strTex[0].IsEmpty() )
	{
		m_atexJobPannel[0] = new IMAGE;
		LoadImage( MakePath( DIR_THEME, strTex[0] ), m_atexJobPannel[0] );
	}

	if( !strTex[1].IsEmpty() )
	{
		m_atexJobPannel[1] = new IMAGE;
		LoadImage( MakePath( DIR_THEME, strTex[1] ), m_atexJobPannel[1] );
	}
	AdjustWndBase();
	
	// 소스 아이템을 입력
	LoadTextureSkillicon();

	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];
}
BOOL CWndSkillTreeEx::GetSkillPoint(DWORD dwSkillID, CRect& rect )
{
	/*
	LPWNDCTRL lpWndCtrl;

	CPoint ptUpperAdd, ptLowerAdd;

	ptUpperAdd.x = 0;
	ptUpperAdd.y = 0;

	ptLowerAdd.x = 0;
	ptLowerAdd.y = 139;
	
	switch(dwSkillID)
	{
		case SI_VAG_ONE_CLEANHIT:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
				rect = lpWndCtrl->rect;
			}
			break;
		case SI_VAG_ONE_BRANDISH:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
				rect = lpWndCtrl->rect;
			}
			break;
		case SI_VAG_ONE_OVERCUTTER:
			{
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
				rect = lpWndCtrl->rect;
			}
			break;

//머서너리
		case SI_MER_ONE_SPLMASH:		rect = CRect( 34, 109, 58, 133 ); break;
		case SI_MER_SHIELD_PROTECTION:	rect = CRect( 34, 161, 58, 185 ); break;
		case SI_MER_ONE_KEENWHEEL:		rect = CRect( 84, 97, 108, 121 ); break;
		case SI_MER_ONE_BLOODYSTRIKE:	rect = CRect( 84, 123, 108, 147 ); break;
		case SI_MER_SHIELD_PANBARRIER:	rect = CRect( 84, 149, 108, 173 ); break;
		case SI_MER_SUP_IMPOWERWEAPON:	rect = CRect( 84, 175, 108, 199 ); break;
		case SI_MER_ONE_BLINDSIDE:		rect = CRect( 134, 97, 158, 121 ); break;
		case SI_MER_ONE_REFLEXHIT:		rect = CRect( 134, 123, 158, 147 ); break;
		case SI_MER_ONE_SNEAKER:		rect = CRect( 134, 149, 158, 173 ); break;
		case SI_MER_SUP_SMITEAXE:		rect = CRect( 134, 175, 158, 199 ); break;
		case SI_MER_SUP_BLAZINGSWORD:	rect = CRect( 134, 201, 158, 225 ); break;
		case SI_MER_ONE_SPECIALHIT:		rect = CRect( 184, 97, 208, 121 ); break;
		case SI_MER_ONE_GUILOTINE:		rect = CRect( 184, 123, 208, 147 ); break;
		case SI_MER_SUP_AXEMASTER:		rect = CRect( 184, 175, 208, 199 ); break;
		case SI_MER_SUP_SWORDMASTER:	rect = CRect( 184, 201, 208, 225 ); break;
// 나이트
		case SI_KNT_TWOSW_CHARGE:		rect = CRect( 34, 260, 58, 284 ); break;
		case SI_KNT_TWOAX_PAINDEALER:	rect = CRect( 34, 286, 58, 310 ); break;
		case SI_KNT_SUP_GUARD:			rect = CRect( 34, 312, 58, 336 ); break;
		case SI_KNT_TWOSW_EARTHDIVIDER:	rect = CRect( 84, 260, 108, 284 ); break;
		case SI_KNT_TWOAX_POWERSTUMP:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_KNT_SUP_RAGE:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_KNT_TWO_POWERSWING:		rect = CRect( 134, 272, 158, 296 ); break;
		case SI_KNT_SUP_PAINREFLECTION:	rect = CRect( 134, 312, 158, 336 ); break;
// 블레이드
		case SI_BLD_DOUBLESW_SILENTSTRIKE:	rect = CRect( 34, 260, 58, 284 ); break;
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	rect = CRect( 34, 286, 58, 310 ); break;
		case SI_BLD_DOUBLE_ARMORPENETRATE:	rect = CRect( 34, 312, 58, 336 ); break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	rect = CRect( 84, 260, 108, 284 ); break;
		case SI_BLD_DOUBLEAX_HAWKATTACK:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_BLD_SUP_BERSERK:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_BLD_DOUBLE_CROSSSTRIKE:		rect = CRect( 134, 272, 158, 296 ); break;
		case SI_BLD_DOUBLE_SONICBLADE:		rect = CRect( 134, 312, 158, 336 ); break;
		
			
// 어시스트	
		case SI_ASS_HEAL_HEALING:			rect = CRect( 18, 136, 42, 160 ); break;
		case SI_ASS_HEAL_PATIENCE:			rect = CRect( 64, 110, 88, 134 ); break;
		case SI_ASS_CHEER_QUICKSTEP:		rect = CRect( 64, 136, 88, 160 ); break;
		case SI_ASS_CHEER_MENTALSIGN:		rect = CRect( 64, 162, 88, 186 ); break;
		case SI_ASS_KNU_TAMPINGHOLE:		rect = CRect( 64, 188, 88, 212 ); break;
		case SI_ASS_HEAL_RESURRECTION:		rect = CRect( 108, 110, 132, 134 ); break;
		case SI_ASS_CHEER_HASTE:			rect = CRect( 110, 136, 134, 160 ); break;
		case SI_ASS_CHEER_HEAPUP:			rect = CRect( 110, 162, 134, 186 ); break;
		case SI_ASS_CHEER_STONEHAND:		rect = CRect( 110, 188, 134, 212 ); break;		
		case SI_ASS_CHEER_CIRCLEHEALING:	rect = CRect( 156, 110, 180, 134 ); break;
		case SI_ASS_CHEER_CATSREFLEX:		rect = CRect( 156, 136, 180, 160 ); break;
		case SI_ASS_CHEER_BEEFUP:			rect = CRect( 156, 162, 180, 186 ); break;
		case SI_ASS_KNU_BURSTCRACK:			rect = CRect( 156, 188, 180, 212 ); break;		
		case SI_ASS_HEAL_PREVENTION:		rect = CRect( 202, 110, 226, 134 ); break;
		case SI_ASS_CHEER_CANNONBALL:		rect = CRect( 202, 136, 226, 160 ); break;
		case SI_ASS_CHEER_ACCURACY:			rect = CRect( 202, 162, 226, 186 ); break;
		case SI_ASS_KNU_POWERFIST:			rect = CRect( 202, 188, 226, 212 ); break;		
//링마스터
		case SI_RIN_SUP_PROTECT:            rect = CRect( 34, 260, 58, 284 ); break;
		case SI_RIN_SUP_HOLYCROSS:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_RIN_HEAL_GVURTIALLA:		rect = CRect( 34, 312, 58, 336 ); break;
		case SI_RIN_SUP_HOLYGUARD:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_RIN_SUP_SPIRITUREFORTUNE:	rect = CRect( 84, 286, 108, 310 ); break;
		case SI_RIN_HEAL_HEALRAIN:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_RIN_SQU_GEBURAHTIPHRETH:	rect = CRect( 134, 272, 158, 296 ); break;
		case SI_RIN_SUP_MERKABAHANZELRUSHA:	rect = CRect( 134, 312, 158, 336 ); break;
//빌포스터
		case SI_BIL_KNU_BELIALSMESHING:		rect = CRect( 34, 260, 58, 284 ); break;
		case SI_BIL_PST_ASMODEUS:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_BIL_KNU_BLOODFIST:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_BIL_PST_BARAQIJALESNA:		rect = CRect( 84, 286, 108, 310 ); break;
		case SI_BIL_KNU_PIERCINGSERPENT:	rect = CRect( 134, 260, 158, 284 ); break;
		case SI_BIL_PST_BGVURTIALBOLD:		rect = CRect( 134, 286, 158, 310 ); break;
		case SI_BIL_KNU_SONICHAND:			rect = CRect( 184, 272, 208, 296 ); break;
		case SI_BIL_PST_ASALRAALAIKUM:		rect = CRect( 184, 312, 208, 336 ); break;
			
			
// 매지션
		case SI_MAG_MAG_MENTALSTRIKE:			rect = CRect( 34, 96, 58, 120 );	break;
		case SI_MAG_MAG_BLINKPOOL: 				rect = CRect( 34, 148, 58, 172 );	break;
		case SI_MAG_FIRE_BOOMERANG:				rect = CRect( 84, 96, 108, 120 );   break;
		case SI_MAG_WIND_SWORDWIND:				rect = CRect( 84, 122, 108, 146 );	break;
		case SI_MAG_WATER_ICEMISSILE:			rect = CRect( 84, 148, 108, 172 );	break;			
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:	rect = CRect( 84, 174, 108, 198 );	break;
		case SI_MAG_EARTH_SPIKESTONE:			rect = CRect( 84, 200, 108, 224 );	break;	
		case SI_MAG_FIRE_HOTAIR:				rect = CRect( 134, 96, 158, 120 );		break;		
		case SI_MAG_WIND_STRONGWIND:			rect = CRect( 134, 122, 158, 146 );		break;		
		case SI_MAG_WATER_WATERBALL:			rect = CRect( 134, 148, 158, 172 );		break;		
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:	rect = CRect( 134, 174, 158, 198 );		break;		
		case SI_MAG_EARTH_ROCKCRASH:			rect = CRect( 134, 200, 158, 224 );		break;		
		case SI_MAG_FIRE_FIRESTRIKE:			rect = CRect( 184, 96, 208, 120 );		break;		
		case SI_MAG_WIND_WINDCUTTER:			rect = CRect( 184, 122, 208, 146 );		break;		
		case SI_MAG_WATER_SPRINGWATER:			rect = CRect( 184, 148, 208, 172 );		break;		
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	rect = CRect( 184, 174, 208, 198 );		break;		
		case SI_MAG_EARTH_LOOTING:				rect = CRect( 184, 200, 208, 224 );		break;
// 엘리멘터
		case SI_ELE_FIRE_FIREBIRD:				rect = CRect( 34, 234, 58, 258 ); break;
		case SI_ELE_EARTH_STONESPEAR:			rect = CRect( 34, 260, 58, 284 ); break;
		case SI_ELE_WIND_VOID:					rect = CRect( 34, 286, 58, 310 ); break;
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	rect = CRect( 34, 312, 58, 336 ); break;
		case SI_ELE_WATER_ICESHARK:				rect = CRect( 34, 338, 58, 362 ); break;
		case SI_ELE_FIRE_BURINGFIELD:			rect = CRect( 84, 234, 108, 258 ); break;
		case SI_ELE_EARTH_EARTHQUAKE:			rect = CRect( 84, 260, 108, 284 ); break;
		case SI_ELE_WIND_WINDFIELD:				rect = CRect( 84, 286, 108, 310 ); break;
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:	rect = CRect( 84, 312, 108, 336 ); break;
		case SI_ELE_WATER_POISONCLOUD:			rect = CRect( 84, 338, 108, 362 ); break;
		case SI_ELE_MULTY_METEOSHOWER:			rect = CRect( 134, 246, 158, 270 ); break;
		case SI_ELE_MULTY_SANDSTORM:			rect = CRect( 134, 272, 158, 296 ); break;
		case SI_ELE_MULTY_LIGHTINGSTORM:		rect = CRect( 134, 298, 158, 322 ); break;
		case SI_ELE_MULTY_AVALANCHE:			rect = CRect( 134, 338, 158, 362 ); break;
		case SI_ELE_FIRE_FIREMASTER:			rect = CRect( 184, 234, 208, 258 ); break;
		case SI_ELE_EARTH_EARTHMASTER:			rect = CRect( 184, 260, 208, 284 ); break;
		case SI_ELE_WIND_WINDMASTER:			rect = CRect( 184, 286, 208, 310 ); break;
		case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	rect = CRect( 184, 312, 208, 336 ); break;
		case SI_ELE_WATER_WATERMASTER:			rect = CRect( 184, 338, 208, 362 ); break;
// 사이키퍼
		case SI_PSY_NLG_DEMONOLGY:				rect = CRect( 34, 260, 58, 284 ); break;
		case SI_PSY_PSY_PSYCHICBOMB:			rect = CRect( 34, 286, 58, 310 ); break;
		case SI_PSY_NLG_CRUCIOSPELL:			rect = CRect( 34, 312, 58, 336 ); break;
		case SI_PSY_NLG_SATANOLGY:				rect = CRect( 84, 260, 108, 284 ); break;
		case SI_PSY_PSY_SPRITBOMB:				rect = CRect( 84, 286, 108, 310 ); break;
		case SI_PSY_PSY_MAXIMUMCRISIS:			rect = CRect( 84, 312, 108, 336 ); break;
		case SI_PSY_PSY_PSYCHICWALL:			rect = CRect( 134, 272, 158, 296 ); break;
		case SI_PSY_PSY_PSYCHICSQUARE:			rect = CRect( 134, 312, 158, 336 ); break;
////////////////////////////////////////////////////////////////////////////////////
			
		default:
			return FALSE;
	}

*/
	int nExpertGapX = 9;
	int nExpertGapY = 90;
	
#ifdef __YTREE_TOPDOWN_050103
	int nProGapX = 8;
	int nProGapY = 228;
#else //__YTREE_TOPDOWN_050103
	int nProGapX = 8;
	int nProGapY = 230;
#endif //__YTREE_TOPDOWN_050103

	ItemProp* pPropSkill = prj.GetSkillProp( dwSkillID );

	if( pPropSkill )
	{
		CPoint pt;

		LPWNDCTRL lpWndCtrl;
		LPWNDCTRL lpWndCtrlUpper = GetWndCtrl( WIDC_STATIC5 );
		LPWNDCTRL lpWndCtrlLower = GetWndCtrl( WIDC_STATIC7 );

		switch( pPropSkill->dwItemKind1 )
		{
			case JTYPE_BASE  : pt = 0; break;
			case JTYPE_EXPERT: pt = lpWndCtrlUpper->rect.TopLeft(); break;
			case JTYPE_PRO   : pt = lpWndCtrlLower->rect.TopLeft(); break;
		}
		int nRectX, nRectY, nJobKind;
		nRectX = nRectY = 0;
		nJobKind = MAX_JOBBASE;
		switch(dwSkillID)
		{
			case SI_VAG_ONE_CLEANHIT:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SI_VAG_ONE_BRANDISH:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SI_VAG_ONE_OVERCUTTER:
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
	//머서너리
			case SI_MER_ONE_SPLMASH:				nRectX =  34, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PROTECTION:			nRectX =  34, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_KEENWHEEL:				nRectX =  84, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLOODYSTRIKE:			nRectX =  84, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SHIELD_PANBARRIER:			nRectX =  84, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_IMPOWERWEAPON:			nRectX =  84, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_BLINDSIDE:				nRectX = 134, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_REFLEXHIT:				nRectX = 134, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SNEAKER:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SMITEAXE:				nRectX = 134, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_BLAZINGSWORD:			nRectX = 134, nRectY = 201, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_SPECIALHIT:				nRectX = 184, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_MER_ONE_GUILOTINE:				nRectX = 184, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_AXEMASTER:				nRectX = 184, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_MER_SUP_SWORDMASTER:			nRectX = 184, nRectY = 201, nJobKind = MAX_EXPERT; break;
	// 나이트
			case SI_KNT_TWOSW_CHARGE:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_PAINDEALER:			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_GUARD:					nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOSW_EARTHDIVIDER:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWOAX_POWERSTUMP:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_RAGE:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_TWO_POWERSWING:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_KNT_SUP_PAINREFLECTION:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	// 블레이드
			case SI_BLD_DOUBLESW_SILENTSTRIKE:		nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_SPRINGATTACK:		nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_ARMORPENETRATE:		nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLESW_BLADEDANCE:		nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLEAX_HAWKATTACK:		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_SUP_BERSERK:				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_CROSSSTRIKE:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BLD_DOUBLE_SONICBLADE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				
	// 어시스트	
			case SI_ASS_HEAL_HEALING:				nRectX =  18, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PATIENCE:				nRectX =  64, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_QUICKSTEP:			nRectX =  64, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_MENTALSIGN:			nRectX =  64, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_TAMPINGHOLE:			nRectX =  64, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_RESURRECTION:			nRectX = 110, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HASTE:				nRectX = 110, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_HEAPUP:				nRectX = 110, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_STONEHAND:			nRectX = 110, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CIRCLEHEALING:		nRectX = 156, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CATSREFLEX:			nRectX = 156, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_BEEFUP:				nRectX = 156, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_BURSTCRACK:				nRectX = 156, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SI_ASS_HEAL_PREVENTION:			nRectX = 202, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_CANNONBALL:			nRectX = 202, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SI_ASS_CHEER_ACCURACY:				nRectX = 202, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SI_ASS_KNU_POWERFIST:				nRectX = 202, nRectY = 187, nJobKind = MAX_EXPERT; break;
	//링마스터
			case SI_RIN_SUP_PROTECT:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYCROSS:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_GVURTIALLA:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_HOLYGUARD:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_SPIRITUREFORTUNE:		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_HEAL_HEALRAIN:				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SQU_GEBURAHTIPHRETH:		nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RIN_SUP_MERKABAHANZELRUSHA:		nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//빌포스터
			case SI_BIL_KNU_BELIALSMESHING:			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASMODEUS:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_BLOODFIST:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BARAQIJALESNA:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_PIERCINGSERPENT:		nRectX = 134, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_BGVURTIALBOLD:			nRectX = 134, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_KNU_SONICHAND:				nRectX = 184, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_BIL_PST_ASALRAALAIKUM:			nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
								
	// 매지션
			case SI_MAG_MAG_MENTALSTRIKE:			nRectX =  34, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_MAG_BLINKPOOL: 				nRectX =  34, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_BOOMERANG:				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_SWORDWIND:				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_ICEMISSILE:			nRectX =  84, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGBALL:	nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_SPIKESTONE:			nRectX =  84, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_HOTAIR:				nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_STRONGWIND:			nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_WATERBALL:			nRectX = 134, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGPARM:	nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_ROCKCRASH:			nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_MAG_FIRE_FIRESTRIKE:			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WIND_WINDCUTTER:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_MAG_WATER_SPRINGWATER:			nRectX = 184, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_MAG_EARTH_LOOTING:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
	// 엘리멘터
			case SI_ELE_FIRE_FIREBIRD:				nRectX =  34, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_STONESPEAR:			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_VOID:					nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_ICESHARK:				nRectX =  34, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_BURINGFIELD:			nRectX =  84, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHQUAKE:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDFIELD:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_ELETRICSHOCK:	nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_POISONCLOUD:			nRectX =  84, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_METEOSHOWER:			nRectX = 134, nRectY = 246, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_SANDSTORM:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_LIGHTINGSTORM:		nRectX = 134, nRectY = 298, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_MULTY_AVALANCHE:			nRectX = 134, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_FIRE_FIREMASTER:			nRectX = 184, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_EARTH_EARTHMASTER:			nRectX = 184, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WIND_WINDMASTER:			nRectX = 184, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_ELE_WATER_WATERMASTER:			nRectX = 184, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
	// 사이키퍼
			case SI_PSY_NLG_DEMONOLGY:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICBOMB:			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_CRUCIOSPELL:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_NLG_SATANOLGY:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_SPRITBOMB:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_MAXIMUMCRISIS:			nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICWALL:			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_PSY_PSY_PSYCHICSQUARE:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	////////////////////////////////////////////////////////////////////////////////////
	// 아크로벳
#ifdef __ACROBAT_0504
			case SI_ACR_YOYO_PULLING:				nRectX =  34, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_SLOWSTEP:				nRectX =  34, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SI_ACR_BOW_JUNKBOW:				nRectX =  34, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_FASTWALKER:				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_YOYOMASTER:				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_BOWMASTER: 				nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SI_ACR_SUP_DARKILLUSION:			nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_SNITCH:				nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_CROSSLINE:				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_SILENTSHOT:				nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_AIMEDSHOT:				nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_ABSOLUTEBLOCK:			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SI_ACR_YOYO_DEADLYSWING:			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_YOYO_COUNTER:				nRectX = 184, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_AUTOSHOT:				nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SI_ACR_BOW_ARROWRAIN:				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;		
	//제스터
			case SI_JST_SUP_POISON:					nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_BLEEDING:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_ESCAPE:				nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_CRITICALSWING:			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_BACKSTAB:				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_SUP_ABSORB:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_VATALSTAB:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_JST_YOYO_HITOFPENYA:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//레인저
			case SI_RAG_BOW_ICEARROW:				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_FLAMEARROW:				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_POISONARROW:			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_FASTATTACK:				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_PIRCINGARROW:			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_SUP_NATURE:					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_TRIPLESHOT:				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SI_RAG_BOW_SILENTARROW:			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
#endif // __ACROBAT_0504				
			default:
				return FALSE;
		}

		if( nJobKind == MAX_EXPERT )
			rect = CRect(  nRectX-nExpertGapX ,  nRectY-nExpertGapY, nRectX+24-nExpertGapX, nRectY+24-nExpertGapY );
		else if( nJobKind == MAX_PROFESSIONAL )
			rect = CRect(  nRectX-nProGapX,  nRectY-nProGapY, nRectX+24-nProGapX, nRectY+24-nProGapY );
		
		rect += pt;
	}


	switch( m_nJob )
	{
	case JOB_KNIGHT:
		break;			
	case JOB_BLADE:
		break;			
	case JOB_MERCENARY:
		break;			
	case JOB_BILLPOSTER:
		break;			
	case JOB_RINGMASTER:
		break;			
	case JOB_ASSIST:
		break;			
	case JOB_ELEMENTOR:
		break;			
	case JOB_PSYCHIKEEPER:
		break;			
	case JOB_MAGICIAN:
		break;	
	}
	
	return TRUE;
}

void CWndSkillTreeEx::LoadTextureSkillicon()
{
	if( m_apSkill == NULL )
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill != NULL_ID )
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}
}
void CWndSkillTreeEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	if( m_bDrag == FALSE )
		return;
	
	CPoint pt( 3, 3 );
	CRect rect;
	if( CheckSkill( g_nSkillCurSelect ) )
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[ g_nSkillCurSelect ].dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType  = 0;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId       = g_nSkillCurSelect; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture = m_atexSkill[ g_nSkillCurSelect ];//L;//pItemElem->m_pTexture;
		_tcscpy( m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}
BOOL CWndSkillTreeEx::Process()
{
	if( m_apSkill == NULL )
		return FALSE;
	

#ifdef __YADV_RESKILLPOINT	
	if( m_pWndReSkillControl )
	{
		if( IsReSkillPointDone() )
		{
			CWndButton* pButton = (CWndButton*)(m_pWndReSkillControl->GetDlgItem(WIDC_BUTTON2));
			if(pButton)
				pButton->EnableWindow(TRUE);
		}
		else
		{
			CWndButton* pButton = (CWndButton*)(m_pWndReSkillControl->GetDlgItem(WIDC_BUTTON2));	
			if(pButton)
				pButton->EnableWindow(FALSE);
		}
	}
#endif //__YADV_RESKILLPOINT
	
	
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
#ifdef __YTREE_TOPDOWN_050103
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
#endif //__YTREE_TOPDOWN_050103

		if( dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				CPoint ptMouse = GetMousePoint();
				
#ifdef __YTREE_TOPDOWN_050103
				rect.OffsetRect( 0, m_nTopDownGap );
#endif //__YTREE_TOPDOWN_050103				

				if( rect.PtInRect(ptMouse) )
				{
					CPoint point2 = ptMouse;
					ClientToScreen( &point2 );
					ClientToScreen( &rect );
#ifdef __YADV_RESKILLPOINT
					if( g_pPlayer->m_dwSkillPoint )
						PutToolTip_Skill( dwSkill, point2, &rect );
					else
#endif //__YADV_RESKILLPOINT
						g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point2, &rect, CheckSkill( i ) );
					
					break;
				}			
			}	
		}
	}
	
	return TRUE;
}


int CWndSkillTreeEx::GetCalcImagePos(int nIndex)
{
#ifdef __YTREE_TOPDOWN_050103

	m_nTopDownGap = 0;

	if( m_bSlot[0] )
	{
		if( nIndex == JTYPE_BASE )
			m_nTopDownGap = 0;
	}
	
	if( m_bSlot[1] )
	{
		if( nIndex == JTYPE_EXPERT )
		{
			if( m_bSlot[0] )
				m_nTopDownGap = 0;
			else
				m_nTopDownGap -= 48;
		}
	}

	if( m_bSlot[2] )
	{
		if( nIndex == JTYPE_PRO )
		{
			if( m_bSlot[0] )
			{
				if( m_bSlot[1] )
					m_nTopDownGap = 0;
				else
					m_nTopDownGap -= 144;
			}
			else
			{
				if( m_bSlot[1] )
					m_nTopDownGap -= 48;
				else
					m_nTopDownGap -= (144+48);
			}
		}
	}
#endif //__YTREE_TOPDOWN_050103

	return 0;
}

#ifdef __YADV_RESKILLPOINT
int CWndSkillTreeEx::GetSkillPxp(DWORD dwID, DWORD dwLv)
{
	AddSkillProp*     pAddSkillProp  = prj.GetAddSkillProp( dwID, dwLv );
	
	if( pAddSkillProp )
		return ( prj.m_aExpSkill[dwLv] * pAddSkillProp->dwSkillExp );
	
	return 0;
}
#endif //__YADV_RESKILLPOINT

void CWndSkillTreeEx::OnDraw(C2DRender* p2DRender)
{
	if( m_apSkill == NULL )
		return;

	m_nAlphaCount = m_nAlpha;
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );

	#ifdef __YTREE_TOPDOWN_050103
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
	#endif //__YTREE_TOPDOWN_050103
		
		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			// 스킬 아이콘 출력 
			if( m_atexSkill[ i ] && CheckSkill( i ) )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
			#ifdef __YTREE_TOPDOWN_050103
					rect.OffsetRect( 0, m_nTopDownGap );
			#endif //__YTREE_TOPDOWN_050103					
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );

			#ifdef __YADV_RESKILLPOINT
					if( g_pPlayer->m_dwSkillPoint )
					{
						char buff[4] = { 0 };
						sprintf( buff, "%2.d", pSkill->dwLevel );
						int nX = rect.right+2;
						int nY = rect.top;
						p2DRender->RenderRoundRect( CRect( nX, nY-4, nX+16, nY+15 ), D3DCOLOR_ARGB( 100, 100, 100, 100 ) );
						p2DRender->TextOut( nX, nY, buff, D3DCOLOR_XRGB( 0, 0, 255 ) );
					}
			#endif //__YADV_RESKILLPOINT
				}
			}	
		}
	}

	if( m_pFocusItem )
	{
		if( m_pFocusItem->dwSkill == -1 )
		{
			CString str;
			str.Format( "스킬쓴넘 직업 %d\n스킬 인덱스가 %d인값이 나옴 : 레벨 %d, EXP %d, CurSelect %d", g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, m_pFocusItem->dwExp, g_nSkillCurSelect );
			ADDERRORMSG( str );
		}
		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_atexSkill[ g_nSkillCurSelect ]->Render( p2DRender, lpWndCtrl->rect.TopLeft() );

		// 게이지 출력 
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
		
		CRect GaugeRect = lpWndCtrl->rect;
		GaugeRect.right += 60;
		GaugeRect.OffsetRect( 30, 17 );
		int nWidth	= ((float)m_pFocusItem->dwExp * 100 / (float)prj.m_aExpSkill[m_pFocusItem->dwLevel + 1]) * GaugeRect.Width() / 100;
		
		CRect rectTemp = GaugeRect; 
		rectTemp.right = rectTemp.left + nWidth;
		
		if( GaugeRect.right < rectTemp.right )
			rectTemp.right = GaugeRect.right;
		
		ItemProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );

		if( pSkillProp && m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
		{
#ifdef __YADV_RESKILLPOINT
			if( !g_pPlayer->m_dwSkillPoint )
#endif //__YADV_RESKILLPOINT
			{
				m_pTheme->RenderGauge( p2DRender, &GaugeRect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
				m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
			}
		}
		
		CRect rect = lpWndCtrl->rect;
		DWORD dwColor = 0xff6060ff;
		
#ifdef __YADV_RESKILLPOINT
		if( g_pPlayer->m_dwSkillPoint )
		{
			char buff[32] = { 0 };
//			sprintf( buff, "남은 포인트 : %d", m_dwCurrSkillPoint );
			sprintf( buff, "Remaining Point : %d", m_dwCurrSkillPoint );
			CPoint cPt;
			cPt.x = 80;
			cPt.y = 35;
			p2DRender->TextOut( cPt.x, cPt.y, buff, D3DCOLOR_XRGB( 0, 0, 255 ) );
			
			if( m_pFocusItem->dwLevel >= pSkillProp->dwExpertMax )
			{
				p2DRender->TextOut( 80, 50, "Master", 0xffff0000 );
			}	
		}
		else
#endif //__YADV_RESKILLPOINT
		{
			// 스킬 레벨 출력 

			TCHAR szNum[ 32 ]; 
			sprintf( szNum, "Lv%2d", m_pFocusItem->dwLevel );
			p2DRender->TextOut( rect.left, rect.top + 17, szNum, dwColor );
			
			if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
			{
				char cSkillExp[16] = {0,};
				sprintf(cSkillExp, "%7.2f %%", (float)m_pFocusItem->dwExp * 100 / (float)prj.m_aExpSkill[m_pFocusItem->dwLevel + 1] );
				p2DRender->TextOut( rect.left + 60, rect.top + 15, cSkillExp, dwColor);
				
				// 스킬 스톱버튼 출력
				if( g_pPlayer->m_dwSkillPoint > 0 )
					m_aSkillLevel[ m_pFocusItem->bStop ]->Render( p2DRender, CPoint( rect.left, rect.top ), 60 );
				else
					m_aSkillLevel[ m_pFocusItem->bStop ]->Render( p2DRender, CPoint( rect.left, rect.top ) );
			}
			else
			{ 
				p2DRender->TextOut( rect.left + 30, rect.top + 17, "Master", 0xffff0000 );
			}
		}	
		
		// 스킬명 출력 
		p2DRender->TextOut( rect.left + 30, rect.top, pSkillProp->szName, dwColor ); 				
	}

#ifdef __YTREE_TOPDOWN_050103
	CRect rect;
	CWndStatic* pStatic;
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	rect = pStatic->GetWindowRect(TRUE);

	rect.left = rect.right - 30;
	
	if( m_bSlot[0] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	rect = pStatic->GetWindowRect(TRUE);
	
	rect.left = rect.right - 30;
	
	if( m_bSlot[1] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	///////////////////////////////////////////////////////////////////////////////////////

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	
	rect.left = rect.right - 30;
	
	if( m_bSlot[2] )
		m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	else
		m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	
#endif //__YTREE_TOPDOWN_050103
	


/*
	{
	CString str;
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM12 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM13 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM14 );
	str.Format( "rect = CRect( %d, %d, %d, %d ); break;\n", lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	OutputDebugString(str);
	}
	/**/	
}
void CWndSkillTreeEx::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	m_aSkillLevel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelup01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 2 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "ButtSkillLevelHold02.tga" ), 0xffff00ff );
	
	InitItem( g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );

#ifdef __YADV_RESKILLPOINT

#else //__YADV_RESKILLPOINT
	if( g_pPlayer->m_dwSkillPoint )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_NOTUSESKILL) );
	}
#endif //__YADV_RESKILLPOINT

#ifdef __YTREE_TOPDOWN_050103
	m_atexTopDown[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "LvUp.bmp" ), 0xffff00ff );
	m_atexTopDown[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "LvDn.bmp" ), 0xffff00ff );

	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
#endif //__YTREE_TOPDOWN_050103	
	
	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - rect.Height();	
	CPoint point( x, y );
	Move( point );
	
}
BOOL CWndSkillTreeEx::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}
BOOL CWndSkillTreeEx::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndSkillTreeEx::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSkillTreeEx::SetJob( int nJob )
{
	m_nJob = nJob;
}
void CWndSkillTreeEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}
void CWndSkillTreeEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

#ifdef __YADV_RESKILLPOINT

#else //__YADV_RESKILLPOINT
	if( g_pPlayer->m_dwSkillPoint )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_NOTUSESKILL) );
		return;
	}
#endif //__YADV_RESKILLPOINT

	CPoint ptMouse = GetMousePoint();

#ifdef __YTREE_TOPDOWN_050103
	
	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
	CRect rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[0] )
		{
			SetActiveSlot(0, FALSE );
		}
		else
		{
			SetActiveSlot(0, TRUE );
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC3 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[1] )
		{
			SetActiveSlot(1, FALSE );
		}
		else
		{
			SetActiveSlot(1, TRUE );
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[2] )
		{
			SetActiveSlot(2, FALSE );
		}
		else
		{
			SetActiveSlot(2, TRUE );
		}
	}
	
#endif //__YTREE_TOPDOWN_050103
				
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
#ifdef __YTREE_TOPDOWN_050103
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
#endif //__YTREE_TOPDOWN_050103

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;

#ifdef __YTREE_TOPDOWN_050103
				rect.OffsetRect( 0, m_nTopDownGap );
#endif //__YTREE_TOPDOWN_050103
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];
					m_bDrag = TRUE;
					break;
				}			
			}	
		}
	}

#ifdef __YADV_RESKILLPOINT
	if( g_pPlayer->m_dwSkillPoint )
		m_bDrag = FALSE;
#endif //__YADV_RESKILLPOINT

	if( m_pFocusItem )
	{
		LPWNDCTRL lpWndCtrl;		
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
		
		CRect Rect = lpWndCtrl->rect;

		Rect.right  = Rect.left+24;
		Rect.bottom = Rect.top+12;
#ifdef __ONLINE		
		if( Rect.PtInRect( point ) ) 
			g_DPlay.SendSkillFlag( m_pFocusItem->dwSkill );
#endif
	}
	
}
void CWndSkillTreeEx::OnRButtonDblClk( UINT nFlags, CPoint point)
{
#ifdef __YADV_RESKILLPOINT

	if( m_apSkill == NULL )
		return;

	if( g_pPlayer->m_dwSkillPoint <= 0 )
		return;
	
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
		
		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				CPoint ptMouse = GetMousePoint();
				
				rect.OffsetRect( 0, m_nTopDownGap );
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];
					
					if( g_pPlayer->m_dwSkillPoint )
					{
						if( pSkillProp->dwItemKind1 != JTYPE_BASE )
						{
							if( IsDownPoint(dwSkill) )
							{
								int nPoint = GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel );

								m_dwCurrSkillPoint += nPoint;
								pSkill->dwLevel--;
							}
						}
						
						break;
					}
				}			
			}	
		}
	}	
#endif //__YADV_RESKILLPOINT
}	

#ifdef __YADV_RESKILLPOINT
void CWndSkillTreeEx::PutToolTip_Skill( DWORD dwSkill, CPoint point, CRect* pRect )
{
	ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
	if( pSkillProp == NULL )
	{
		Error( "CWndReSkillPoint::PutToolTip_Skill : 스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}

	LPSKILL pSkill = GetdwSkill(dwSkill);

	if( pSkill == NULL )
	{
		Error( "CWndReSkillPoint::PutToolTip_Skill : pSkill스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}
	
	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pSkill->dwLevel );
	if( pAddSkillProp == NULL )
	{
		Error( "CWndReSkillPoint::PutToolTip_Skill : add스킬(%d)의 프로퍼티가 업다.", dwSkill );
		return;	// property not found
	}
	
	CString string;
	string.Format( "#cff0000ff%s#nc", pSkillProp->szName );

	CString str;
	CString strstr;


	{
		str.Format( prj.GetText(TID_GAME_SKILLLEVEL), pSkill->dwLevel );
		string += '\n';
		string += str;
	}

	
	strstr.Format( prj.GetText(TID_GAME_BASESKILLLEVEL), pSkillProp->dwReqDisLV );
	if( g_pPlayer->GetLevel() < pSkillProp->dwReqDisLV )
		str.Format( "\n#cffff0000%s#nc", strstr );
	else
		str.Format( "\n%s", strstr );
	string += str;
	
	if( pSkillProp->dwReSkill1 != -1 )
	{
		LPSKILL pskill = GetdwSkill(pSkillProp->dwReSkill1);
		
		if( pskill )
		{
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel1 )
			{
				strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel1 );
				str.Format( "\n#cffff0000%s#nc", strstr );
				string += str;
			}
		}
	}
	if( pSkillProp->dwReSkill2 != -1 )
	{
		LPSKILL pskill = GetdwSkill(pSkillProp->dwReSkill2);
		
		if( pskill )
		{
			if( pskill->dwLevel < pSkillProp->dwReSkillLevel2 )
			{
				strstr.Format( prj.GetText(TID_GAME_SKILLDIS), pskill->GetProp()->szName, pSkillProp->dwReSkillLevel2 );
				str.Format( "\n#cffff0000%s#nc", strstr );
				string += str;
			}
		}
	}
	
	if( pSkill->dwLevel+1 <= pSkillProp->dwExpertMax )
	{
		int nPxp = GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel+1 );

#ifdef __YPERSENTTOPOINT
		CString strstr;
		string += '\n';
		str.Format( prj.GetText(TID_RESKILLPOINT_PXP), nPxp);
		strstr.Format( "#cff009c00%s#nc", str );
		string += strstr;	
#else //__YPERSENTTOPOINT
		float fPersent = ((float)nPxp / (float)g_pPlayer->m_dwSkillPoint ) * 100.0f;
		
		string += '\n';
		str.Format( prj.GetText(TID_RESKILLPOINT_PXP), fPersent);
		string += str;	
#endif //__YPERSENTTOPOINT		
	}
	
	if( _stricmp( pSkillProp->szCommand, "=" ) )
	{
		string += "\n";
		string += pSkillProp->szCommand;
	}
	
	
	
	CEditString strEdit;
	strEdit.SetParsingString( string );
	
	g_toolTip.PutToolTip( dwSkill, strEdit, *pRect, point, 0 );
}
#endif //__YADV_RESKILLPOINT

void CWndSkillTreeEx::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

#ifdef __YADV_RESKILLPOINT
	; 
#else //__YADV_RESKILLPOINT
	if( g_pPlayer->m_dwSkillPoint )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_TIP_RESKILLPOINT_ALERT2) );
		return;

	//	EnableWindow(FALSE);
	}
#endif //__YADV_RESKILLPOINT

	// 스킬창에서 더블클릭하면 자동으로 스킬바에 등록된다.
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
#ifdef __YTREE_TOPDOWN_050103
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwItemKind1 == JTYPE_PRO )
					continue;
			}		
			
			GetCalcImagePos( pSkillProp->dwItemKind1 );
		}
#endif //__YTREE_TOPDOWN_050103

		if( pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				
				CPoint ptMouse = GetMousePoint();

		#ifdef __YTREE_TOPDOWN_050103
				rect.OffsetRect( 0, m_nTopDownGap );
		#endif //__YTREE_TOPDOWN_050103
				
				if( rect.PtInRect(ptMouse) )
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[ i ];

			#ifdef __YADV_RESKILLPOINT
					if( g_pPlayer->m_dwSkillPoint )
					{
						if( pSkillProp->dwItemKind1 != JTYPE_BASE )
						{
							if( pSkill->dwLevel < pSkillProp->dwExpertMax )
							{
								int nPoint = GetSkillPxp( pSkillProp->dwSubDefine, pSkill->dwLevel+1 );

								if( int(m_dwCurrSkillPoint - nPoint) >= 0 )
								{
									m_dwCurrSkillPoint -= nPoint;
									pSkill->dwLevel++;
								}
							}
						}

						break;
					}
			#endif //__YADV_RESKILLPOINT
					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if( pTaskBar->m_nExecute == 0 )		// 스킬큐가 사용되지 않을때만 등록됨.
						pTaskBar->SetSkillQueue( pTaskBar->m_nCurQueueNum, 0, i, m_atexSkill[i] );
					break;
				}			
			}	
		}
	}
}
#ifdef __V050322_CACHEQUEST
void CWndSkillTreeEx::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
#else
void CWndSkillTreeEx::AdditionalSkinTexture( LPWORD pDest, CSize size )
#endif
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
 
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_STATIC5 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;

#ifdef __YTREE_TOPDOWN_050103	
	pt.y += 2;
	
	GetCalcImagePos(JTYPE_EXPERT);
	pt.y += m_nTopDownGap;

	if( m_atexJobPannel[ 0 ] && m_bSlot[1] ) PaintTexture( pDest, m_atexJobPannel[ 0 ], pt, size );

	lpWndCtrl = GetWndCtrl( WIDC_STATIC7 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	GetCalcImagePos(JTYPE_PRO);
	pt.y += 2;
	pt.y += m_nTopDownGap;
	
	if( m_atexJobPannel[ 1 ] && m_bSlot[2] ) PaintTexture( pDest, m_atexJobPannel[ 1 ], pt, size );

#else //__YTREE_TOPDOWN_050103
	pt.y += 2;
	
	if( m_atexJobPannel[ 0 ] ) PaintTexture( pDest, m_atexJobPannel[ 0 ], pt, size );
	
	lpWndCtrl = GetWndCtrl( WIDC_STATIC7 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;
	if( m_atexJobPannel[ 1 ]  ) PaintTexture( pDest, m_atexJobPannel[ 1 ], pt, size );
	
#endif //__YTREE_TOPDOWN_050103

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 돈 거래 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTradeGold::CWndTradeGold() 
{ 
	m_dwGold = 0;
	m_nIdWndTo = APP_INVENTORY;
	m_pWndBase = NULL;
	m_nSlot = 0;
	m_nPutSlot = 100;
} 
CWndTradeGold::~CWndTradeGold() 
{ 
} 
void CWndTradeGold::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTradeGold::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );
	CWndButton* pWndOk = (CWndButton *)GetDlgItem( WIDC_OK );	
	pWndOk->SetDefault( TRUE );
	pWndEdit->SetFocus();

	if( m_dwGold > INT_MAX )
		m_dwGold = INT_MAX;
	
	TCHAR szNumber[ 64 ];
	_itot( m_dwGold, szNumber, 10 );
	pWndEdit->SetString( szNumber );
} 
BOOL CWndTradeGold::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, 0, 0, pWndParent );
}
BOOL CWndTradeGold::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTradeGold::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTradeGold::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTradeGold::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTradeGold::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CRect rect = GetClientRect();
		CWorld* pWorld	= g_WorldMng.Get();
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT );

		CString string = pWndEdit->GetString();
		int nCost = _ttoi( string );
		if( nCost < 1 )
			nCost = 1;
		if( m_nIdWndTo == APP_INVENTORY )
		{
#ifdef __ONLINE
			if( nCost > g_pPlayer->GetGold() )
				nCost = g_pPlayer->GetGold();

			g_DPlay.SendDropGold( nCost, g_pPlayer->GetPos(), m_vPos );
#else	// __ONLINE
			CItem* pItem = new CItem;
			CItemElem* pItemElem = new CItemElem;
			pItem->m_pItemBase = pItemElem;
			pItemElem->m_dwItemId = II_GOLD_SEED1;
			pItemElem->m_nItemNum = nCost;
			g_pPlayer->AddGold( -nCost, FALSE );

			pItem->SetIndex( g_Neuz.m_pd3dDevice, II_GOLD_SEED1 );
			pItem->SetPos( m_vPos );
			pItem->SetAngle( xRandom( 360 ) );
			pWorld->AddObj( pItem, TRUE );
#endif	// __ONLINE
		}
		else
		if( m_nIdWndTo == APP_TRADE )
		{
#ifdef __ONLINE
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				if( nCost > 0 )
				{
					g_DPlay.SendTradePutGold( nCost );
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
					}

					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendTradePut( m_Shortcut.m_dwData, m_Shortcut.m_dwType, m_Shortcut.m_dwId, nCost );
				}
			}
#else	// __ONLINE
			CWndTrade* pWndTrade = (CWndTrade*)m_pWndBase;
			pWndTrade->m_nGoldI = nCost;
			g_pPlayer->m_vtInfo.TradeSetGold( nCost );
#endif	// __ONLINE
		}
		else
		if( m_nIdWndTo == APP_BANK )
		{
#ifdef __ONLINE
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				g_DPlay.SendPutGoldBank( m_nSlot, nCost );
			}
			else // 아이템
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
					}
					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendPutItemBank( m_nSlot, m_Shortcut.m_dwId, nCost );
				}
			}
#endif	// __ONLINE
		}
		// 자신의 인벤토리에서 길드창고로 아이템을 이동
		else
		if (m_nIdWndTo == APP_GUILD_BANK)
		{
			if( m_Shortcut.m_dwData != 0 ) // 아이템
			{
				CItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
					}
					
					m_Shortcut.m_dwData -= 100;
#ifdef __ONLINE
					g_DPlay.SendPutItemGuildBank( m_Shortcut.m_dwId, nCost, 1 );
#endif
				}
			}
		}
		//	길드창고에서 자신의 인벤으로 가져올때
		//	APP_MESSENGER로 그냥 설정
		else
		if (m_nIdWndTo == APP_MESSENGER)
		{
			if (g_pPlayer->GetGuild())
			{
				if( m_Shortcut.m_dwData == 0 ) // 돈
				{
					if( nCost > g_pPlayer->GetGuild()->m_nGoldGuild )
						nCost = g_pPlayer->GetGuild()->m_nGoldGuild;
					
					if( nCost > 0 )
					{
					#ifdef __ONLINE
						g_DPlay.SendGetItemGuildBank( m_Shortcut.m_dwId, nCost, 0); // 0은 페냐가 변경됐다는 것을 의미한다.
					#endif //__ONLINE
					}
					else
					{
						// error message
					}
				}
				else // 아이템
				{
					CItemBase* pItemBase = NULL;
					if( m_Shortcut.m_dwType == ITYPE_ITEM )
					{
						pItemBase = g_pPlayer->GetGuild()->m_GuildBank.GetAtId( m_Shortcut.m_dwId );
					}
					else
					{
						//					assert(0);
					}

					if( pItemBase )
					{
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
					#ifdef __ONLINE
						g_DPlay.SendGetItemGuildBank( m_Shortcut.m_dwId, nCost, 1 ); // 1은 아이템이 변경됐다는 것을 의미한다.
					#endif //__ONLINE
					}
				}
			}
		}

		else
		// 인벤토리 인데 쓰고 있어서 Gold로 씀.
		if( m_nIdWndTo == APP_GOLD ) // 뱅크에서 인벤토리로 빼옴
		{
#ifdef __ONLINE
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->m_dwGoldBank[m_nSlot] )
					nCost = g_pPlayer->m_dwGoldBank[m_nSlot];
				
				if( nCost > 0 )
				{
					g_DPlay.SendGetGoldBank( m_nSlot, nCost );
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemBankId( m_nSlot, m_Shortcut.m_dwId );

					if( pItemBase )
					{
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemBank( m_nSlot, m_Shortcut.m_dwId, nCost );
					}
				}
				else
				{
					//					assert(0);
				}
			}
#endif	// __ONLINE
		}
		else
		if( m_nIdWndTo == APP_COMMON_BANK )
		{
#ifdef __ONLINE
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->m_dwGoldBank[m_nPutSlot] )
					nCost = g_pPlayer->m_dwGoldBank[m_nPutSlot];
				
				if( nCost > 0 ) 
				{
					g_DPlay.SendPutGoldBankToBank( m_nPutSlot, m_nSlot, nCost );
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemBankId( m_nPutSlot, m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendPutItemBankToBank( m_nPutSlot, m_nSlot, m_Shortcut.m_dwId, nCost );
					}
				}
				else
				{
					//					assert(0);
				}
			}
#endif	// __ONLINE
		}
#ifdef __Y_POST_0912
		else
		if( m_nIdWndTo == APP_POST_SEND )
		{
	#ifdef __ONLINE
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold()  )
					nCost = g_pPlayer->GetGold();
				
				if( nCost > 0 ) 
				{
					// 보내기 창에 넣는다.
					if( g_WndMng.m_pWndPost )
					{
						g_WndMng.m_pWndPost->m_PostTabSend.SetCost(nCost);
					}
				}
				else
				{
					// error message
				}
			}
			else // 아이템
			{
				CItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if( nCost > ( (CItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (CItemElem*)pItemBase )->m_nItemNum;
						}
						
						pItemBase->SetExtra( nCost );		

						m_Shortcut.m_dwData -= 100;

						if( g_WndMng.m_pWndPost )
						{
							g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(m_Shortcut.m_dwId);
							g_WndMng.m_pWndPost->m_PostTabSend.SetCount(nCost);
						}						
					}
				}
				else
				{
					//					assert(0);
				}
			}
	#endif	// __ONLINE
		}			
#endif //__Y_POST_0912

		Destroy();
	}
	if( nID == WIDC_CANCEL )
	{
#ifdef __Y_POST_0912
		if( m_nIdWndTo == APP_POST_SEND )
		{
			g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
			g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
		}
#endif //__Y_POST_0912
				
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 거래 확인
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmTrade::CWndConfirmTrade() 
{ 
	m_objid = 0;
} 
CWndConfirmTrade::~CWndConfirmTrade() 
{ 
} 
void CWndConfirmTrade::OnSetName( const char* szName, OBJID objid )
{
	m_objid = objid;
	CWndStatic *m_pName   = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CString sName;
	sName = szName;
	sName.Format( prj.GetText(TID_GAME_FROM),  szName ); // " 님에게"
	m_pName->SetTitle( sName );
}
void CWndConfirmTrade::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndConfirmTrade::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndConfirmTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	m_objid = 0;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_TRADE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndConfirmTrade::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndConfirmTrade::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndConfirmTrade::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndConfirmTrade::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndConfirmTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#ifdef __ONLINE
	if( nID == WIDC_OK )
	{
		CMover* pTrader	= prj.GetMover( m_objid );
		if( pTrader != NULL )
		{
			g_DPlay.SendTrade( pTrader );
		}
		Destroy();		// 수동파괴로 바꿈.
	}
	else 
	if( nID == WIDC_CANCEL || nID == WTBID_CLOSE )
	{
		g_DPlay.SendConfirmTradeCancel( m_objid );
		Destroy();
	}
#endif
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 거래 최종 확인
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndTradeConfirm::CWndTradeConfirm() 
{ 
} 
CWndTradeConfirm::~CWndTradeConfirm() 
{ 
} 
void CWndTradeConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndTradeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndButton * pWndButtonOk = (CWndButton*)GetDlgItem( WIDC_YES );
	pWndButtonOk->SetVisible( TRUE );
	CWndButton * pWndButtonNO = (CWndButton*)GetDlgItem( WIDC_NO );
	pWndButtonNO->SetVisible( TRUE );

	bMsg = FALSE;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndStatic->SetTitle( prj.GetText( TID_DIAG_0083 ) );
				

	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTradeConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTradeConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndTradeConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndTradeConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndTradeConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndTradeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
#ifdef __ONLINE
	if( bMsg )
	{
		return( TRUE );
	}

	if( nID == WIDC_NO || nID == WTBID_CLOSE ) // 취소 
	{
		g_DPlay.SendTradeCancel();
	}
	else if ( nID == WIDC_YES )
	{
		g_DPlay.SendTradeConfirm();
	}
#endif // __ONLINE
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 거래 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndTrade::CWndTrade()
{
	SetPutRegInfo( FALSE );
}
CWndTrade::~CWndTrade()
{
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
}

void CWndTrade::OnDraw(C2DRender* p2DRender)
{
	CWorld* pWorld = g_WorldMng.Get();
#ifdef __ONLINE
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
#else
	CMover* pMover = (CMover*)pWorld->GetObjFocus(); 
#endif
	if( pMover && pMover->GetType() == OT_MOVER )
	{
		CWndStatic* pWndNameYou = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		CWndStatic* pWndNameI = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		CWndStatic* pWndGoldYou = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		CWndStatic* pWndGoldI = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

		pWndNameYou->SetTitle( pMover->GetName( TRUE ) );
		pWndNameI->SetTitle( g_pPlayer->GetName() );

		CString string;
		string.Format( "%d", m_nGoldYou );
		pWndGoldYou->SetTitle(string );
		string.Format( "%d", m_nGoldI );
		pWndGoldI->SetTitle( string );
		
	}
	else
	{
		g_pPlayer->m_vtInfo.TradeClear();
	}
}
void CWndTrade::OnInitialUpdate()
{
	m_wndItemCtrlYou.Create( WLVS_ICON|WBS_NODRAWFRAME, CRect(   5, 25 + 15,   5 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 ), this, 103 );
	m_wndItemCtrlI.  Create( WLVS_ICON|WBS_NODRAWFRAME, CRect( 175, 25 + 15, 175 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 ), this, 104 );

	m_nGoldI = 0;
	m_nGoldYou = 0;

	CWorld* pWorld = g_WorldMng.Get();

#ifdef __ONLINE
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
#else
	CMover* pMover = (CMover*)pWorld->GetObjFocus(); 
#endif

	if( pMover )
	{
		pMover->m_vtInfo.TradeClear();
		pMover->m_vtInfo.SetOther( g_pPlayer );
		m_wndItemCtrlYou.InitItem( pMover );
		m_wndItemCtrlI.InitItem( g_pPlayer );
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
	
	CWndNeuz::OnInitialUpdate();

#ifdef __BUGFIX_CLONE0607
	if( g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndBase(APP_VENDOREX)  )
	{
		Destroy();
		return;
	}
#endif	// __BUGFIX_CLONE0607
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	CWorld* pWorld = g_WorldMng.Get();
#ifdef __ONLINE
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
#else
	CMover* pMover = (CMover*)pWorld->GetObjFocus();
#endif

	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase( APP_TRADE_CONFIRM );
	if( pWndTradeConfirm )
	{
		return( TRUE );
	}
	if( message == WIN_ITEMDROP && nID == 104 )
	{
		BOOL TradeGold = FALSE;
		SAFE_DELETE( g_WndMng.m_pWndTradeGold );
		g_WndMng.m_pWndTradeGold = new CWndTradeGold;
		memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
		if( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwData == 0 ) // 돈
		{
			TradeGold = TRUE;
 			g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
		}
		else // 아이템
		{
			CItemBase* pItemBase = g_pPlayer->GetItemId( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwId );
			g_WndMng.m_pWndTradeGold->m_dwGold = ( (CItemElem*)pItemBase )->m_nItemNum;
		}
		g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_TRADE;
		g_WndMng.m_pWndTradeGold->m_pWndBase = this;
		
		if( TradeGold )
		{
			if( m_nGoldI == 0 )
			{
				g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
				g_WndMng.m_pWndTradeGold->MoveParentCenter();
				CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
				CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
				g_WndMng.m_pWndTradeGold->SetTitle( "Panay" );
				CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
				CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
				pStatic->m_strTitle = strMain;
				pStaticCount->m_strTitle = strCount;
			}
			else
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
			}
		}
		else
		{
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();
			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			g_WndMng.m_pWndTradeGold->SetTitle( "Item" );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
		}
	}
	else
	{
		if( nID == WIDC_CLEAR )	// clear
		{
		}
		else
			if( nID == WIDC_OK_I )	// 확인
			{
#ifdef __ONLINE
				g_DPlay.SendTradeOk();
#endif
			}
			else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE ) // 취소 
			{
#ifdef __ONLINE
				g_DPlay.SendTradeCancel();
				return( TRUE );
#else	// __ONLINE
				DoCancel();
#endif	// __ONLINE
			}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndTrade::DoCancel()
{
	CMover* pTrader	= g_pPlayer->m_vtInfo.GetOther();
	if( pTrader )
		pTrader->m_vtInfo.TradeClear();
	g_pPlayer->m_vtInfo.TradeClear();
	
	Destroy();
}

BOOL CWndTrade::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}

void CWndTrade::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndTrade::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}
void CWndTrade::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 네비게이터 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OBJFILTER_PLAYER   0
#define OBJFILTER_PARTY    1 
#define OBJFILTER_NPC      2
#define OBJFILTER_MONSTER  3 

CWndNavigator::CWndNavigator()
{
	m_bObjFilterPlayer  = TRUE;
	m_bObjFilterParty   = TRUE;
	m_bObjFilterNPC     = TRUE;
	m_bObjFilterMonster = TRUE;
	m_szName[ 0 ] = 0;
	m_iFrame = 0;
}
CWndNavigator::~CWndNavigator()
{

}
BOOL CWndNavigator::OnEraseBkgnd(C2DRender* p2DRender)
{
	if( g_pPlayer == NULL ) return TRUE;
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	CWorld* pWorld	= g_WorldMng();
	CRect rect = GetClientRect();

	// 포커스 오브젝트 출력 
	CObj* pObjFocus = pWorld->GetObjFocus();
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	CLandscape* pLand = pWorld->GetLandscape( pWorld->m_pCamera->m_vPos );
	if( pLand == NULL ) return 1;
	int nImageBlock = pLand->m_texMiniMap.m_size.cx;

	CRectClip clipRect( 0, 0, p2DRender->m_clipRect.Width(), p2DRender->m_clipRect.Height() );
	CPoint point;
	CRect rectCur;

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );
	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = vCenter.x;
	int yCenter = - vCenter.z;
	int i;
	
	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			point = CPoint ( x * nImageBlock, z * nImageBlock );
			point.y -= ( nImageBlock * pWorld->m_nLandWidth );
			point.x += rect.Width() / 2;
			point.y += rect.Height() / 2;
			point.x -= xCenter;
			point.y -= yCenter;
			rectCur.SetRect( point.x, point.y, point.x + nImageBlock, point.y + nImageBlock );
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
			{
				pLand->m_texMiniMap.m_size = m_size;
				if( clipRect.RectLapRect( rectCur ) )
				{
					if( pLand->m_texMiniMap.m_pTexture )
					{
//						pLand->m_texMiniMap.m_size = m_size;
						pLand->m_texMiniMap.Render( p2DRender, point, 200 );//CWndBase::m_nAlpha );
					}
				} 
			}
		}
	}

	AccuFrame();	//	간단한 프레임 계산을 해준다.

	if( pWorld->m_bIsIndoor == FALSE )
	{
		// 객체 출력을 위한 설정 
		TEXTUREVERTEX vertex[ 1000 * 6 ];
		TEXTUREVERTEX* pVertices = vertex; 
		int nCount = 0;
		int xu = 0, yv = 0;
		CRect aRcArray[ 100 ];
		int nRcCount = 0;
		
		////////////////////////////////////////////////////
		// 오브젝트 출력 
		////////////////////////////////////////////////////
		CObj* pObj;
		int nIndex = 0;
		int nPartyMap[ MAX_PTMEMBER_SIZE ];
#ifdef __V050524_QUESTEMOTICON
		CMover* apQuest[ 100 ];
		int nQuestNum = 0;
#endif
		ZeroMemory( nPartyMap, sizeof(nPartyMap) );
		int nPartyMapCount = 0;
		float fDistMap = rect.Width() / 2 / fx;

		FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
		{
			FOR_OBJ( pLand, pObj, OT_MOVER )
			{
				BOOL bPartyMap = FALSE;
				if( pObj != g_pPlayer )
				{
					CMover* pMover = (CMover*)pObj;						

					if( g_pPlayer->m_idMurderer != pMover->m_idPlayer )	// 찍을 타겟이 날 죽인넘이면 조건없이 무조건 찍는다.
					{
						// 플레이어필터가 아닌데, 플레이어라면 
						if( m_bObjFilterPlayer == FALSE && pMover->IsPlayer() == TRUE )
						{
							// 게다가 파티필터도 아니라면 스킵 
							if( m_bObjFilterParty == FALSE )
								continue;
							if( g_Party.IsMember( pMover->m_idPlayer ) == FALSE ) 
								continue;
						}
						if( m_bObjFilterMonster == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == FALSE ) )
							continue;
						if( m_bObjFilterNPC == FALSE && ( pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == TRUE ) )
						{
							// 상점이 아니라면 컨티뉴.
							LPCHARACTER lpCharacter = pMover->GetCharacter();
							if( lpCharacter == NULL || lpCharacter->m_nStructure == -1 ) 
								continue;
								
						}
						if( pMover->IsMode( TRANSPARENT_MODE ) )	// 대상이 투명모드일땐 타겟 안됨.
							continue;	
					}
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					point.x = ( rect.Width() / 2 ) + vPos.x;
					point.y = ( rect.Height() / 2 ) - vPos.z;
					point.x -= xCenter;
					point.y -= yCenter;
					
					CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);
					//rectHittest.InflateRect( 4, 4 );
					CPoint ptMouse = GetMousePoint();
					if( rectHittest.PtInRect( ptMouse ) )
					{
						ClientToScreen( &ptMouse );
						ClientToScreen( &rectHittest );
						g_toolTip.PutToolTip( 10000 + ((CMover*)pObj)->GetId(), ((CMover*)pObj)->GetName( TRUE ), rectHittest, ptMouse, 0 );
					}
#ifdef __V050524_QUESTEMOTICON
					// 퀘스트 이모티콘 
					if( pMover->IsNPC() && pMover->m_nQuestEmoticonIndex != -1 )
					{
						apQuest[ nQuestNum++ ] = pMover;
					}
					else
#endif // __V050524_QUESTEMOTICON

					if( g_pPlayer->m_idMurderer && g_pPlayer->m_idMurderer == pMover->m_idPlayer )		// 찍을 타겟이 날 죽인넘이면 빨갛게 표시
						nIndex = 5;
					else
					if( pMover->IsNPC() && pMover->IsPeaceful() )
					{
						nIndex = 1;
					}
					else
					if( g_pPlayer->m_idGuild && g_pPlayer->m_idGuild == pMover->m_idGuild )
						nIndex = 3;
					else
					if( pMover->IsNPC() && pMover->IsPeaceful() == FALSE )
						nIndex = 2;
						//xu = 10, yv = 0;
					else
					if( pMover->IsPlayer() )
					{
						nPartyMap[nPartyMapCount] = g_Party.FindMember( pMover->m_idPlayer );
						if( nPartyMap[nPartyMapCount] != -1 )
						{
	#ifdef __S1005_PARTY_MAP
							g_Party.m_aMember[ nPartyMap[nPartyMapCount] ].m_vPos = pMover->GetPos();
							++nPartyMapCount;
							bPartyMap = TRUE;
	#endif // __S1005_PARTY_MAP
							nIndex = 4;
						}
						else
							nIndex = 0;
					}
					if( nCount < 1000 )
					{
						nCount++;
						LPCHARACTER lpCharacter = pMover->GetCharacter();
						if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
							m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpCharacter->m_nStructure, &pVertices );
						else
						{
	#ifdef	__S1005_PARTY_MAP
							if( bPartyMap )
							{
								D3DXVECTOR3 vPosBuf = g_pPlayer->GetPos() - pMover->GetPos();
								float fDist = D3DXVec3Length( &vPosBuf );
								
								if( fDist > fDistMap )
								{
									RenderPartyMember( p2DRender, &pVertices, rect, pMover->GetPos(), pMover->GetId(), pMover->GetName( TRUE ) );
								}
								else
								{
									m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), 3, &pVertices );
								}
							}
							else
	#endif // __S1005_PARTY_MAP
#ifdef __V050524_QUESTEMOTICON
							// 퀘스트가 없는 것만 출력 
							if( pMover->m_nQuestEmoticonIndex == -1 )
#endif
								m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), nIndex, &pVertices );
						}
					}
				}
			}
			END_OBJ
		}
		END_LAND
		////////////////////////////////////////////////////////
		// 리젼 오브젝트로 설정된 스트럭쳐 이모티콘 출력 
		////////////////////////////////////////////////////////
		int nSize = pWorld->m_aStructure.GetSize();
		for( i = 0; i < nSize; i++ )
		{
			LPREGIONELEM lpRegionElem = pWorld->m_aStructure.GetAt( i );
			vPos = lpRegionElem->m_vPos;
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = ( rect.Width() / 2 ) + vPos.x;
			point.y = ( rect.Height() / 2 ) - vPos.z;
			point.x -= xCenter;
			point.y -= yCenter;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 8, point.y - 8 ), 6 + lpRegionElem->m_dwStructure, &pVertices );
		}
#ifdef __V050524_QUESTEMOTICON
		////////////////////////////////////////////////////////
		// 퀘스트 이모티콘 출력 
		// 모든 아이콘 보다 맨 위에 찍혀야 되므로 여가서 몰아서 찍는다.
		////////////////////////////////////////////////////////
		for( i = 0; i < nQuestNum; i++ )
		{
			CMover* pMover = apQuest[ i ];
			vPos = pMover->GetPos();
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = ( rect.Width() / 2 ) + vPos.x;
			point.y = ( rect.Height() / 2 ) - vPos.z;
			point.x -= xCenter;
			point.y -= yCenter;

			nIndex = pMover->m_nQuestEmoticonIndex + 19;
			m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );		
/*			
			
			LPCHARACTER lpCharacter = pMover->GetCharacter();
			if( pMover->m_nQuestEmoticonIndex == 0 || pMover->m_nQuestEmoticonIndex == 2 ) 
			{
				if( lpCharacter && lpCharacter->m_nStructure == -1 )
				{
					nIndex = pMover->m_nQuestEmoticonIndex + 20;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );
				}
			}
			else
			if( pMover->m_nQuestEmoticonIndex == 1 || pMover->m_nQuestEmoticonIndex == 3 ) 
			{
				//if( lpCharacter && lpCharacter->m_nStructure != -1 )
				{
					nIndex = pMover->m_nQuestEmoticonIndex + 20;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 5, point.y - 5 ), nIndex, &pVertices );
				}
			}
*/
		}
#endif
	#ifdef __S1005_PARTY_MAP
		nPartyMap[nPartyMapCount] = g_Party.FindMember( g_pPlayer->m_idPlayer );
		if( nPartyMap[nPartyMapCount] != -1 )
			++nPartyMapCount;
		for( i = 0 ; i < g_Party.GetSizeofMember() && g_Party.GetSizeofMember() != nPartyMapCount ; ++i )
		{
			BOOL bDisplay = TRUE;
			for( int j = 0 ; j < nPartyMapCount ; ++j )
			{
				if( nPartyMap[j] == i )
				{
					bDisplay = FALSE;
					break;
				}
			}
					
			if( bDisplay )
			{
				D3DXVECTOR3 vPosBuf = g_pPlayer->GetPos() - g_Party.GetPos( i );
				float fDist = D3DXVec3Length( &vPosBuf );

				
				if( fDist > fDistMap )
				{
					RenderPartyMember( p2DRender, &pVertices, rect, g_Party.GetPos( i ), g_Party.m_aMember[i].m_uPlayerId, g_Party.m_aMember[i].m_szName );
				}
				else
				{
					vPos = g_Party.GetPos( i );
					vPos.x *= fx;
					vPos.z *= fy;
					point.x = ( rect.Width() / 2 ) + vPos.x;
					point.y = ( rect.Height() / 2 ) - vPos.z;
					point.x -= xCenter;
					point.y -= yCenter;
					m_texNavObjs.MakeVertex( p2DRender, CPoint( point.x - 2, point.y - 2 ), 3, &pVertices );

					CRect rectHittest( point.x, point.y, point.x + 5, point.y + 5);
					CPoint ptMouse = GetMousePoint();
					if( rectHittest.PtInRect( ptMouse ) )
					{
						ClientToScreen( &ptMouse );
						ClientToScreen( &rectHittest );
						g_toolTip.PutToolTip( 10000 + g_Party.m_aMember[i].m_uPlayerId, g_Party.m_aMember[i].m_szName, rectHittest, ptMouse, 0 );
					}
				}
			}
		}
	#endif // __S1005_PARTY_MAP
		////////////////////////////////////////////////////
		// 포커스 오브젝트 출력 (좀더 분멸하게 보이도록)
		////////////////////////////////////////////////////
		if( pObjFocus )
		{
			vPos = pObjFocus->GetPos();
			vPos.x *= fx;
			vPos.z *= fy;
			point.x = ( rect.Width() / 2 ) + vPos.x;
			point.y = ( rect.Height() / 2 ) - vPos.z;
			point.x -= xCenter;
			point.y -= yCenter;

			xu = 9, yv = 9;

			point.x -= 3;
			point.y -= 3;

			if( nCount < 1000 )
			{
				nCount++;
				m_texNavObjs.MakeVertex( p2DRender, point, 5, &pVertices );
			}
		}
		m_texNavObjs.Render( m_pApp->m_pd3dDevice, vertex, ( (int) pVertices - (int) vertex ) / sizeof( TEXTUREVERTEX ) );
	}
	else
	{
		m_texDunFog.m_size = CSize( rect.Width(), rect.Height() );
		p2DRender->RenderTexture( CPoint( 0, 0 ), &m_texDunFog, 255 );
	}
	m_wndPlace.EnableWindow( !pWorld->m_bIsIndoor );

	RenderMarkAll(p2DRender, g_pPlayer);

	// 화면 비율 때문에 임의로 정사각형 뷰포트를 지정해 놓는다. 안그러면 화살표 모양이 찌그러짐.
	D3DVIEWPORT9 viewport;
	viewport.X      = p2DRender->m_clipRect.left + ( p2DRender->m_clipRect.Width()  / 2 );
	viewport.Y      = p2DRender->m_clipRect.top  + ( p2DRender->m_clipRect.Height() / 2 );
	viewport.Width  = 32;
	viewport.Height = 32;
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	viewport.X      -= 16;
	viewport.Y      -= 16;

	pd3dDevice->SetViewport( &viewport );

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	// 카메라 
	D3DXMATRIX  matView;
	D3DXMatrixIdentity( &matView );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXMatrixScaling( &mat, 1.2f, 1.2f, 1.2f );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	FLOAT fAngle	= ( g_pPlayer != NULL ? g_pPlayer->GetAngle() - 180 : 0 );
	if( fAngle < 0 ) fAngle = 360 + fAngle;
	FLOAT fTheta = D3DXToRadian( fAngle );// (2*D3DX_PI*g_pPlayer->m_nAngle)/(360);//m_nAngle
	D3DXMatrixRotationZ( &mat, fTheta );
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		

	// 화살표 출력 
	m_billArrow.Render( pd3dDevice );

	return TRUE;
}

void CWndNavigator::RenderMark( C2DRender* p2DRender, CMover* Player )
{
	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	if ( Player->m_nvPoint.Time != 0 )
	{	
		D3DXVECTOR3 Pos = g_pPlayer->GetPos() - Player->m_nvPoint.Pos;
		Pos.x *= fx ;
		Pos.z *= -fy;
		Player->m_nvPoint.Time--;

		CPoint point;
		CRect rect = GetClientRect();
		point.x = ( rect.Width() / 2 ) - ( Pos.x + 2 ) ;
		point.y = ( rect.Height() / 2 ) - ( Pos.z + 2 );
		
		//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
		m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

		CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
		CPoint ptMouse = GetMousePoint();
		if( rectHit.PtInRect( ptMouse ) )
		{
			CString toolTip("Mark : ");
			toolTip += Player->GetName( TRUE );
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHit );
			g_toolTip.PutToolTip( 10000 + Player->GetId(), toolTip, rectHit, ptMouse, 0 );
		}
	}
}

//	이걸 넣어 준다음에 정리를 위해서 NaviPoint형을 참조나 포인터로 받는 형태로 다시 만들어 주어야 할것이다.
void CWndNavigator::RenderMarkAll( C2DRender* p2DRender , CMover * Player )
{
	RenderMark( p2DRender, Player );

	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	V_NaviPoint::iterator nvi = g_pPlayer->m_vOtherPoint.begin();
	for ( int i = 0 ; i < g_pPlayer->m_vOtherPoint.size() ; )
	{
		if ( g_pPlayer->m_vOtherPoint[i].Time != 0 )
		{
			D3DXVECTOR3 Pos = g_pPlayer->GetPos() - g_pPlayer->m_vOtherPoint[i].Pos;
			Pos.x *= fx ;
			Pos.z *= -fy;
			g_pPlayer->m_vOtherPoint[i].Time--;

			CPoint point;
			CRect rect = GetClientRect();
			point.x = ( rect.Width() / 2 ) - ( Pos.x + 2 ) ;
			point.y = ( rect.Height() / 2 ) - ( Pos.z + 2 );
			
			//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
			m_texNavPos.Render( p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

			CRect rectHit( point.x - 8, point.y - 8, point.x + 8, point.y + 8);
			CPoint ptMouse = GetMousePoint();
			if( rectHit.PtInRect( ptMouse ) )
			{
				CString toolTip("Mark : ");
				toolTip += g_pPlayer->m_vOtherPoint[i].Name.c_str();
				ClientToScreen( &ptMouse );
				ClientToScreen( &rectHit );
				g_toolTip.PutToolTip( 10000 + g_pPlayer->m_vOtherPoint[i].objid, toolTip, rectHit, ptMouse, 0 );
			}
			nvi++;
			i++;
		}
		else
		{
			g_pPlayer->m_vOtherPoint.erase(nvi);
		}
		
	}
}

void CWndNavigator::RenderPartyMember( C2DRender* p2DRender, TEXTUREVERTEX** pVertices, CRect rect, D3DXVECTOR3 vPos, u_long uIdPlayer, LPCTSTR lpStr )
{
	float fDistx = g_pPlayer->GetPos().x - vPos.x;
	float fDistz = g_pPlayer->GetPos().z - vPos.z;
	float fRadian = atan2( fDistx, fDistz );
	float fDegree = D3DXToDegree( fRadian );
	CPoint PointBuf;
	
	if( fDegree < 0 )
	{
		if( fDegree > -45 )
			PointBuf = CPoint( rect.Width() / 2 + -fDegree, rect.Height() - 10 );
		else if( fDegree > -135 )
			PointBuf = CPoint( rect.Width() - 10, rect.Height() / 2 + 90 + fDegree );
		else
			PointBuf = CPoint( rect.Width() / 2 + (180 + fDegree), 5 );
	}
	else
	{
		if( fDegree < 45 )
			PointBuf = CPoint( rect.Width() / 2 + -fDegree, rect.Height() - 10 );
		else if( fDegree < 135 )
			PointBuf = CPoint( 8, rect.Height() / 2 + 90 -fDegree );
		else
			PointBuf = CPoint( rect.Width() / 2 + -(180 - fDegree), 5 );
	}

	m_texNavObjs.MakeVertex( p2DRender, PointBuf, 3, pVertices );

	CRect rectHittest( PointBuf.x, PointBuf.y, PointBuf.x + 5, PointBuf.y + 5);
	CPoint ptMouse = GetMousePoint();
	if( rectHittest.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rectHittest );
		g_toolTip.PutToolTip( 10000 + uIdPlayer, lpStr, rectHittest, ptMouse, 0 );
	}
}
HRESULT CWndNavigator::DeleteDeviceObjects()
{
	//CWndNeuz::DeleteDeviceObjects();
	m_texArrow.DeleteDeviceObjects();
	m_texNavObjs.DeleteDeviceObjects();
	m_billArrow.DeleteDeviceObjects();
	m_texNavPos.DeleteDeviceObjects();
#ifdef __YDEBUG
	m_texDunFog.DeleteDeviceObjects();
#endif //__YDEBUG
	
	return CWndNeuz::DeleteDeviceObjects();
}
HRESULT CWndNavigator::InvalidateDeviceObjects()
{
	m_texNavPos.InvalidateDeviceObjects();
#ifdef __YDEBUG
	m_texArrow.Invalidate();
	m_texDunFog.Invalidate();
	m_texNavObjs.InvalidateDeviceObjects();
#endif //__YDEBUG
	
	m_billArrow.InvalidateDeviceObjects();
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndNavigator::RestoreDeviceObjects()
{
	m_texNavPos.RestoreDeviceObjects( m_pApp->m_pd3dDevice );
#ifdef __YDEBUG
	m_texArrow.SetInvalidate(m_pApp->m_pd3dDevice);
	m_texDunFog.SetInvalidate(m_pApp->m_pd3dDevice);
	m_texNavObjs.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG

	ResizeMiniMap();
	m_billArrow.RestoreDeviceObjects();
	return CWndNeuz::RestoreDeviceObjects();
}
void CWndNavigator::OnDraw(C2DRender* p2DRender)
{
}
void CWndNavigator::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
		}
		else
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
			ar >> m_size.cx >> m_size.cy;
		}
		m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
		m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
		m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
		m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );
	}
	else
	{
		dwVersion = 1;
		ar << m_bObjFilterPlayer << m_bObjFilterParty << m_bObjFilterNPC << m_bObjFilterMonster;
		ar << m_size.cx << m_size.cy;
	}
}
void CWndNavigator::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectClient = GetClientRect();
	rectClient.right = 13;
	m_wndPlace.Create  ( "P", 0, CRect( rectClient.left,   1, rectClient.left + 16,   1 + 16 ), this, 100000 );
//	m_wndMover.Create  ( "U", 0, CRect( rectClient.left,  17, rectClient.left + 16,  17 + 16 ), this, 100001 );
//	m_wndMover.Create  ( "M", 0, CRect( rectClient.left,  36, rectClient.left + 16,  36 + 16 ), this, 102 );
	m_wndZoomIn.Create ( "+", 0, CRect( rectClient.left,  54, rectClient.left + 16,  54 + 16 ), this, 100005 );
	m_wndZoomOut.Create( "-", 0, CRect( rectClient.left,  70, rectClient.left + 16,  70 + 16 ), this, 100006 );

	m_wndPlace.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtNavLeft.tga" ), TRUE );
	m_wndPlace.FitTextureSize();
	m_wndZoomIn.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtNavZoomIn.tga" ), TRUE );
	m_wndZoomIn.FitTextureSize();
	m_wndZoomOut.SetTexture( D3DDEVICE, MakePath( DIR_THEME, "ButtNavZoomOut.tga" ), TRUE );
	m_wndZoomOut.FitTextureSize();

	m_texDunFog.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "NavDunFog.tga" ), 0 , 1 );

	m_wndMenuPlace.CreateMenu( this );	
	/*
	m_wndMenuPlace.AppendMenu( 0, 0 ,_T("플레이어" ) );
	m_wndMenuPlace.AppendMenu( 0, 1 ,_T("극단"     ) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 ,_T("NPC"      ) );
	m_wndMenuPlace.AppendMenu( 0, 3 ,_T("몬스터"   ) ); 
	*/
	m_wndMenuPlace.AppendMenu( 0, 0 , prj.GetText(TID_GAME_PLAYER) );
	m_wndMenuPlace.AppendMenu( 0, 1 , prj.GetText(TID_GAME_PARTYTEXT) ); 
	m_wndMenuPlace.AppendMenu( 0, 2 , prj.GetText(TID_GAME_NPC) );
	m_wndMenuPlace.AppendMenu( 0, 3 , prj.GetText(TID_GAME_MONSTER) ); 
	

	m_wndMenuPlace.CheckMenuItem( 0, m_bObjFilterPlayer );
	m_wndMenuPlace.CheckMenuItem( 1, m_bObjFilterParty  );
	m_wndMenuPlace.CheckMenuItem( 2, m_bObjFilterNPC    );
	m_wndMenuPlace.CheckMenuItem( 3, m_bObjFilterMonster );

/*
	m_wndMenuPlace.AppendMenu( 0, 0 ,_T("무기상점" ) );
	m_wndMenuPlace.AppendMenu( 0, 1 ,_T("잡화점"   ) );
	m_wndMenuPlace.AppendMenu( 0, 2 ,_T("방어구점" ) ); 
	m_wndMenuPlace.AppendMenu( 0, 3 ,_T("술집"     ) );
	m_wndMenuPlace.AppendMenu( 0, 4 ,_T("도시 "    ) );
	m_wndMenuPlace.AppendMenu( 0, 5 ,_T("무기상점" ) );
	m_wndMenuPlace.AppendMenu( 0, 6 ,_T("잡화점"   ) );
	m_wndMenuPlace.AppendMenu( 0, 7 ,_T("방어구점" ) ); 
	m_wndMenuPlace.AppendMenu( 0, 8 ,_T("술집"     ) );
	m_wndMenuPlace.AppendMenu( 0, 9 ,_T("-- 장소추가 --" ));
*/
	m_wndMenuMover.CreateMenu( this );	
	/*
	CRect rect = GetClientRect();
	D3DXVECTOR3 vPos = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x
	FLOAT fx = (FLOAT)m_texture.m_size.cx / 256.0f * 2;
	FLOAT fy = (FLOAT)m_texture.m_size.cy / 256.0f * 2;
	vPos.x *= fx;
	vPos.z *= fy;
	CObj* pObj;
	D3DXVECTOR3 vCenter	= ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0, 0 ) );
	*/

	//m_size = CSize( MINIMAP_SIZE, MINIMAP_SIZE );
	m_size = CSize( 256, 256) ;//MINIMAP_SIZE, MINIMAP_SIZE );
	m_nSizeCnt = 0;
	//m_wndMinimize.Create("_",0,CRect(00,0,14,14),this,10000);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), rectRoot.top );
	Move( point );

	ResizeMiniMap();

}
BOOL CWndNavigator::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 0, 0, 115, 110 ); // 1024 768

	m_texNavObjs.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"Navigator.inc") );
	m_texArrow.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,"ImgNavArrow.bmp"), 0xffff00ff );
	m_texNavPos.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "NavPosition.inc") );
	ZeroMemory( &m_billboard, sizeof( m_billboard ) );
	m_billboard.rect.SetRect( 0, 0, m_texArrow.m_size.cx, m_texArrow.m_size.cy );
	m_billboard.ptCenter = CPoint( m_texArrow.m_size.cx / 2, m_texArrow.m_size.cy / 2 );
	m_billArrow.InitDeviceObjects( g_Neuz.m_pd3dDevice, &m_billboard, &m_texArrow );
	m_billArrow.RestoreDeviceObjects();

	SetTitle( GETTEXT( TID_APP_NAVIGATOR ) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );

//	return CWndNeuz::Create( WBS_MOVE|WBS_SOUND|WBS_CAPTION, rect, &g_WndMng, dwWndId );
}
void CWndNavigator::ResizeMiniMap()
{
	CWorld* pWorld	= g_WorldMng();
	if( pWorld == NULL )
		return;
	for( int z = 0; z < pWorld->m_nLandHeight; z++ )
	{
		for( int x = 0; x < pWorld->m_nLandWidth; x++ )
		{
			CLandscape* pLand = pWorld->GetLandscape( x, pWorld->m_nLandHeight - z - 1);
			if( pLand )
				pLand->m_texMiniMap.m_size = m_size;
		}
	}
}
BOOL CWndNavigator::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CRect rect = GetWindowRect( TRUE );
	//int nImageBlock = pLand->m_texMiniMap.m_size.cx;

	CWndBase* pWndBase = (CWndBase*) pLResult;

	if( pWndBase->m_pParentWnd == &m_wndMenuPlace )
	{
	//	g_WndMng.m_nObjectFilter = nID;
		//m_wndMenuPlace.CheckMenuRadioItem( 0, 2, nID, TRUE );
		int nState = !m_wndMenuPlace.GetMenuState( nID, 0 );
		m_wndMenuPlace.CheckMenuItem( nID, nState );
		SetFocus();
		switch( nID )
		{
		case OBJFILTER_PLAYER:
			m_bObjFilterPlayer = nState;
			break;
		case OBJFILTER_PARTY:
			m_bObjFilterParty = nState;
			break;
		case OBJFILTER_NPC:
			m_bObjFilterNPC = nState;
			break;
		case OBJFILTER_MONSTER:
			m_bObjFilterMonster = nState;
			break;
		}
	}
	else
	if( message == WNM_CLICKED )
	{
		switch(nID)
		{
			case 100000: // 장소 찾기
				m_wndMenuPlace.Move( CPoint( rect.left - m_wndMenuPlace.GetWindowRect().Width(), rect.top ) );
				m_wndMenuPlace.SetVisible( !m_wndMenuPlace.IsVisible() );
				m_wndMenuPlace.SetFocus();
				break;
			case 100001: // 장소 찾기
				{
					m_wndMenuMover.DeleteAllMenu();
					CWorld* pWorld	= g_WorldMng();
					CLandscape* pLand;
					CObj* pObj;
					CWndButton* pWndButton;
					int i = 0;

					FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
					{
						FOR_OBJ( pLand, pObj, OT_MOVER )
						{
							pWndButton = m_wndMenuMover.AppendMenu( i++, ((CMover*)pObj)->GetId() , ((CMover*)pObj)->GetName( TRUE ) );
							pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_OBJECT;
						}
						END_OBJ
					}
					END_LAND
					m_wndMenuMover.Move( CPoint( rect.left - m_wndMenuMover.GetWindowRect().Width(), rect.top ) );
					m_wndMenuMover.SetVisible( !m_wndMenuMover.IsVisible() );
					m_wndMenuMover.SetFocus();
				}
				break;
			case 100005: // zoom in
				m_nSizeCnt = 1;
				m_size.cx += 32;
				m_size.cy += 32;
				if(m_size.cx > 352)
					m_size.cx = 352;
				if(m_size.cy > 352 )
					m_size.cy = 352;
				/*
				if(m_size.cx > 1024)
					m_size.cx = 1024;
				if(m_size.cy > 1024 )
					m_size.cy = 1024;
					*/
				ResizeMiniMap();
				break;
			case 100006: // zoom out 
				m_nSizeCnt = -1;
				m_size.cx -= 32;
				m_size.cy -= 32;
				if( m_size.cx < 128 )
					m_size.cx = 128;
				if( m_size.cy < 128 )
					m_size.cy = 128;
				ResizeMiniMap();
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndNavigator::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if( g_pPlayer == NULL ) 
		return;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CRect rect = GetClientRect();
		CPoint pt = point;
		
		pt.x -= ( rect.Width() / 2 );
		pt.y -= ( rect.Height() / 2 );
		
		FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );//(FLOAT)m_size.cx / (FLOAT)MINIMAP_SIZE;// * 2;
		FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );//(FLOAT)m_size.cy / (FLOAT)MINIMAP_SIZE;// * 2;
		
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		vPos.x += ( pt.x / fx );
		vPos.z -= ( pt.y / fy );
#ifdef __ONLINE
		CString string;
		string.Format( "/teleport %d %f %f", g_WorldMng()->m_dwWorldID, vPos.x, vPos.z );
		g_DPlay.SendChat( string );
#else
		g_DialogMsg.RemoveAll();
		vPos.y = 200.0f;
		g_WorldMng()->SetObjFocus( NULL );
		g_WorldMng()->RemoveObj( g_pPlayer );
		g_WorldMng()->ReadWorld( vPos, TRUE );
		g_pPlayer->SetPos( vPos );
		g_WorldMng()->AddObj( g_pPlayer );
		g_WorldMng()->m_pCamera->m_vPos = vPos;
#endif
		//
		//		g_DialogMsg.RemoveAll();
		//		g_WorldMng()->ReadWorld( D3DXVECTOR3( (FLOAT)x, 130, (FLOAT)z ), FALSE );
		//		g_pPlayer->SetPos( D3DXVECTOR3( (FLOAT)x, 130, (FLOAT)z ) );
	}
}

BOOL CWndNavigator::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	if( pWndBase == &m_wndMenuMover )
	{
		if( nID >= 0 && nID < 100000 )
		{
			CMover* pMover = prj.GetMover( nID );
			g_WorldMng()->SetObjFocus( pMover );
			SetFocus();
		}
	}
	/*
	CRect rect = GetWindowRect( TRUE );
	
	switch(nID)
	{
		case 100: // wnd1 
			m_wndMenu.Move( CPoint( rect.right, rect.top ) );
			m_wndMenu.SetVisible( !m_wndMenu.IsVisible() );
			m_wndMenu.SetFocus();
			break;
	}
	*/
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndNavigator::SetWndRect( CRect rectWnd, BOOL bOnSize )
{
	AdjustMinRect( &rectWnd, 16 * 6, 16 * 7 );
	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 13 );
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndNavigator::OnSize(UINT nType, int cx, int cy)
{
	m_wndPlace.Move( CPoint( 0, 0 ) );
	m_wndZoomIn.Move( CPoint(0, cy - 32 ) );
	m_wndZoomOut.Move( CPoint(0, cy - 16 ) );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndNavigator::OnRButtonDown(UINT nFlags, CPoint point)
{				
	CWorld* pWorld = g_WorldMng();
	pWorld->SetObjFocus(  NULL );
	g_pPlayer->ClearDest();
}
void CWndNavigator::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndNavigator::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetClientRect();
	D3DXVECTOR3 vPos, vCenter = ( g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3( 0, 0 , 0 ) );
	// 128 : m_texture.m_size.cx = 1 : x
	FLOAT fx = (FLOAT)m_size.cx / ( MAP_SIZE * MPU );
	FLOAT fy = (FLOAT)m_size.cy / ( MAP_SIZE * MPU );

	CWorld* pWorld	= g_WorldMng();

#ifdef __ONLINE
		g_pPlayer->m_nvPoint.Pos.x = vCenter.x + ( (float)( point.x - (rect.right / 2) ) / fx );
		g_pPlayer->m_nvPoint.Pos.z = vCenter.z - ( (float)( point.y - (rect.bottom / 2) ) / fy );
		//	이 시간은 네비게이터에 남아 있는 시간으로 디파인이나 파일로 뺄수도 있을듯하지만 우선 하드 코딩이닷
		g_pPlayer->m_nvPoint.Time = 200;	
		if( pWorld )
		{
			CObj* pObj	= pWorld->GetObjFocus();
			if( IsValidObj( pObj ) && pObj->GetType() == OT_MOVER && ( (CMover*)pObj )->IsPlayer() )
			{
				g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, ( (CCtrl*)pObj )->GetId() );
			}
			else
			{
				if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
					g_DPlay.SendSetNaviPoint( g_pPlayer->m_nvPoint.Pos, NULL_ID );
			}
		}
#endif // __ONLINE
	vCenter.x *= fx;
	vCenter.z *= fy;
	int xCenter = vCenter.x;
	int yCenter = - vCenter.z;
	
	CObj* pObj;
	CLandscape* pLand;
//	CWorld* pWorld	= g_WorldMng();
	CMover *pMover;
	FOR_LAND( pWorld, pLand, pWorld->m_nVisibilityLand, FALSE )
	{
		FOR_OBJ( pLand, pObj, OT_MOVER )
		{
			if( pObj->GetType() == OT_MOVER )
			{
				pMover = (CMover *)pObj;
				if( pMover->IsMode( TRANSPARENT_MODE ) == FALSE )
				{
					vPos = pObj->GetPos();
					vPos.x *= fx;
					vPos.z *= fy;
					int x = ( rect.Width() / 2 ) + vPos.x;
					int y = ( rect.Height() / 2 ) - vPos.z;
					x -= xCenter + 2;
					y -= yCenter + 2;
					CRect rectHittest( x, y, x + 5, y + 5);
					if( rectHittest.PtInRect( point ) )
					{
						if( CMover::GetActiveMover() != pObj )
							pWorld->SetObjFocus( pObj );
						return;
					}
				}
			}
		}
		END_OBJ
	}
	END_LAND

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 게이지 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


CWndStatus::CWndStatus()
{
	m_nDisplay = 2;
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;

	m_pVBHPGauge = NULL;
	m_pVBMPGauge = NULL;
	m_pVBFPGauge = NULL;
	m_pVBEXPGauge = NULL;
	m_pVBPXPGauge = NULL;

	m_bVBHPGauge = TRUE;
	m_bVBMPGauge = TRUE;
	m_bVBFPGauge = TRUE;
	m_bVBEXPGauge = TRUE;
	m_bVBPXPGauge = TRUE;

	m_bHPVisible = TRUE;
	m_bExpVisible = TRUE;
	
}
CWndStatus::~CWndStatus()
{
	DeleteDeviceObjects();
}
void CWndStatus::MakeGaugeVertex()
{
	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpFP   = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );
	CMover* pMover = CMover::GetActiveMover();

	if( pMover )
	{
		CRect rect = GetClientRect();
		CRect rectTemp;
		
		//	pMover->m_nHitPoint = 50;
		int nWidthClient = lpHP->rect.Width();
		//100 : rect = hp : x
		int nWidth;

		// HP
		nWidth = nWidthClient * pMover->GetHitPoint() / pMover->GetMaxHitPoint();
		if( m_nHPWidth != nWidth ) 
		{
			m_nHPWidth = nWidth;
			rect = lpHP->rect;//.SetRect( 105, 6, 105 + nWidthClient, 6 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal );
		}
		//else m_bVBHPGauge = TRUE;
		// MP
		nWidth	= nWidthClient * pMover->GetManaPoint() / pMover->GetMaxManaPoint();
		if( m_nMPWidth != nWidth ) 
		{
			m_nMPWidth = nWidth;
			rect = lpMP->rect;//rect.SetRect( 105, 20, 105 + nWidthClient, 20 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBMPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x640000ff, m_pVBMPGauge, &m_texGauFillNormal );
		}
		//else m_bVBMPGauge = TRUE;
		// FP
		nWidth	= nWidthClient * pMover->GetFatiguePoint() / pMover->GetMaxFatiguePoint();
		if( m_nFPWidth != nWidth ) 
		{
			m_nFPWidth = nWidth;
			rect = lpFP->rect;//rect.SetRect( 105, 32, 105 + nWidthClient, 32 + 12 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBFPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x6400ff00, m_pVBFPGauge, &m_texGauFillNormal );
		}
	//lse m_bVBFPGauge = TRUE;
		//nWidth	= ( ( hyper ) nWidthClient * pMover->GetExp1() ) / pMover->GetMaxExp1();
		//nWidth	= ( ( hyper ) nWidthClient * pMover->GetExp1() ) / pMover->GetMaxExp1();
		nWidth	= ( (float) nWidthClient / pMover->GetMaxExp1() ) * pMover->GetExp1();

		if( m_nEXPWidth != nWidth ) 
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;//rect.SetRect( 105, 48, 105 + nWidthClient, 48 + 8 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x847070ff, m_pVBEXPGauge, &m_texGauFillSmall );
		}
		//else m_bVBEXPGauge = TRUE;
		// 경험치 2
		//nWidth	= nWidthClient * pMover->GetExp2() / pMover->GetMaxExp2();
		nWidth	= ( (float) nWidthClient / pMover->GetMaxExp2() ) * pMover->GetExp2();
		if( m_nPXPWidth != nWidth ) 
		{
			m_nPXPWidth = nWidth;
			rect = lpPxp->rect;//rect.SetRect( 105, 58, 105 + nWidthClient, 58 + 8 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBPXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x847070ff, m_pVBPXPGauge, &m_texGauFillSmall );
		}
		//else m_bVBPXPGauge = TRUE;
	}
}
void CWndStatus::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	if( m_pTexture )
	{
		RenderWnd();
		// 여기는 타이틀 바의 텍스트를 출력하는 곳 
		if( IsWndStyle( WBS_CAPTION ) )	
		{
			int y = 0;
#ifndef _MULTI_LANGUAGE_
			if( m_resMng.m_nLanguage ) // 한글 버젼만 Black Arial 폰트 세로가 이상해서...
#endif
				y = 4;
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

			CHAR szNum[32];
			itoa( g_pPlayer->GetLevel(), szNum, 10 );
			
			CHAR szLevel[32];
			strcpy( szLevel, " Lv. " );
			strcat( szLevel, szNum );

			p2DRender->TextOut( 57, y, szLevel, 0xffffffd0 );
			p2DRender->SetFont( pOldFont );
		}				
	}
	else
	{
		m_pTheme->RenderWndBaseFrame( p2DRender, &rect );
		if( IsWndStyle( WBS_CAPTION ) )
		{
			// 타이틀 바 
			rect.bottom = 21;
			{
				m_pTheme->RenderWndBaseTitleBar( p2DRender, &rect, m_strTitle, m_dwColor );
			}
		}
	}
}
void CWndStatus::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = CMover::GetActiveMover();
	if( pMover == NULL )
		return;

	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;// - 30;

	CRect rectTemp;
	//SetTitle( "" );
	/////////////////////////////////////

	LPWNDCTRL lpHP   = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL lpMP   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL lpFP   = GetWndCtrl( WIDC_CUSTOM3 );
	LPWNDCTRL lpExp  = GetWndCtrl( WIDC_CUSTOM4 );
	LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );
	
	if( pMover )
	{
		MakeGaugeVertex();

		if( m_bVBHPGauge )
		{
//#ifdef __VCRITICAL
			int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
			if( nHitPercent < CRITICAL_BERSERK_HP )
				m_bHPVisible = !m_bHPVisible;
			else
				m_bHPVisible = TRUE;
//#else
//			m_bHPVisible = TRUE;
//#endif
			if( m_bHPVisible )
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauFillNormal );
		}
		if( m_bVBMPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBMPGauge, &m_texGauFillNormal );
		if( m_bVBFPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBFPGauge, &m_texGauFillNormal );
		if( g_pPlayer->IsAfterDeath() )
			m_bExpVisible = !m_bExpVisible;
		else
			m_bExpVisible = TRUE;

		if( m_bExpVisible )
		{
			if( m_bVBEXPGauge )
			{
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillSmall );
			}
			if( m_bVBPXPGauge )
			{
				m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBPXPGauge, &m_texGauFillSmall );

			}
		}

/*
		//p2DRender->TextOut( 10, 65, 1, 1, "Lv. : ", 0xff101010 );
		CHAR szNum[32];
		itoa( g_pPlayer->GetLevel(), szNum, 10 );
		//p2DRender->TextOut( 35, 65, 1, 1, szNum, 0xff606060 );

		CHAR szLevel[32];
		strcpy( szLevel, "Status - Lv. " );
		strcat( szLevel, szNum );

		m_strTitle = szLevel;
*/
	}		
	if( m_nDisplay != 0 )
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255 );
		//p2DRender->SetFont( m_pTheme->m_pFontWndTitle );

		char cbufHp[16] = {0,};
		char cbufMp[16] = {0,};
		char cbufFp[16] = {0,};
		char cbufExp[16] = {0,};
		char cbufPxp[16] = {0,};
		
		int nCharHP, nCharMP, nCharFP, nCharEXP, nCharPXP;

		if(m_nDisplay == 1)
		{
			wsprintf(cbufHp, "%d%%", pMover->GetHitPointPercent());
			wsprintf(cbufMp, "%d%%", pMover->GetManaPointPercent());
			wsprintf(cbufFp, "%d%%", pMover->GetFatiguePointPercent());

			p2DRender->TextOut( lpHP->rect.right - 75, lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - 75, lpMP->rect.top - 0, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - 75, lpFP->rect.top - 0, cbufFp, dwColor, 0xff000000 );

			nCharHP = wsprintf(cbufHp, "%dp", pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, "%dp", pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, "%dp", pMover->GetFatiguePoint());

			int x = lpHP->rect.right-7; // -40
			p2DRender->TextOut( x - (int)(nCharHP*5.8f) , lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharMP*5.8f), lpMP->rect.top - 0, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharFP*5.8f), lpFP->rect.top - 0, cbufFp, dwColor, 0xff000000 );
		}
		else
		{
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetFatiguePoint());

			int x = lpHP->rect.right - 80 + 15;
			p2DRender->TextOut( x - (int)(nCharHP*2.6f), lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharMP*2.6f), lpMP->rect.top - 0, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharFP*2.6f), lpFP->rect.top - 0, cbufFp, dwColor, 0xff000000 );
			
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetMaxHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetMaxManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetMaxFatiguePoint());
			
			x = lpHP->rect.right - 42 + 15;
			p2DRender->TextOut( x - nCharHP*2, lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - nCharMP*2, lpMP->rect.top - 0, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - nCharFP*2, lpFP->rect.top - 0, cbufFp, dwColor, 0xff000000 );
			
			p2DRender->TextOut( lpHP->rect.right - 48, lpHP->rect.top - 0, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - 48, lpMP->rect.top - 0, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - 48, lpFP->rect.top - 0, "/", dwColor, 0xff000000 );
		}
		
		float fExp = (float)pMover->GetExp1() * 100.0f / (float)pMover->GetMaxExp1();
		if( fExp >= 99.99f )
			nCharEXP = sprintf( cbufExp, "99.99%%" );		// sprintf함수 내부에서 반올림되어 100.00으로 표시되는 것을 막기 위해서 
		else
			nCharEXP = sprintf( cbufExp, "%.2f%%", fExp );

		float fPxp = (float)pMover->GetExp2() * 100.0f / (float)pMover->GetMaxExp2();
		
		nCharPXP = sprintf( cbufPxp, "%.2f%%", fPxp );
		
		int x = lpHP->rect.right-7; // -40
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );
		p2DRender->TextOut( x - (int)(nCharPXP*5.8f), lpPxp->rect.top - 0, cbufPxp, dwColor, 0xff000000 );
	}


	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		


//	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255) );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;//2;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;//5;
	viewport.Width  = lpFace->rect.Width();//p2DRender->m_clipRect.Width();
	viewport.Height = lpFace->rect.Height();// - 10;//p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;

	pd3dDevice->SetViewport(&viewport);

	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	FLOAT fov = D3DX_PI/4.0f;//796.0f;
	FLOAT h = cos(fov/2) / sin(fov/2);
	FLOAT w = h * fAspect;
	D3DXMatrixOrthoLH( &matProj, w, h, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX  matView;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot1, matRot2;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot1);
	D3DXMatrixIdentity(&matRot2);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);

//#ifdef __VCRITICAL
	int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
	if( nHitPercent < CRITICAL_BERSERK_HP )
	{
		D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
		D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
		D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		
		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		D3DXMatrixTranslation(&matTrans,0.0f,-7.75f,0.0f);
		
		D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
		D3DXMatrixRotationX( &matRot2, D3DXToRadian( -5 ) );
		matRot1 *= matRot2;
	}
	else
	{	
		D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
		D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
		D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		D3DXMatrixTranslation(&matTrans,0.0f,-7.8f,0.0f);

		D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
	}
//#else
//	D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
//	D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
//	D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
//	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
//
//	D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
//	D3DXMatrixTranslation(&matTrans,0.0f,-7.8f,0.0f);
//	
//	D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
//#endif
	
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot1);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	/*
	// 프로젝션 
	D3DXMATRIX matProj;
	D3DXMatrixIdentity( &matProj );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// 카메라 
	D3DXMATRIX  matView;
	D3DXMatrixIdentity( &matView );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// 초기화 
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matScale,6.5f,6.5f,6.5f);
	D3DXMatrixTranslation(&matTrans,0.2f,-9.7f,0.9f);
	D3DXMatrixRotationY( &matRot, D3DXToRadian( -30 ) );

	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
*/
	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, CWorld::m_dwBgColor, 1.0f, 0 ) ;
	if( g_pBipedMesh )
	{
		//m_pModel->FrameMove();
		g_pBipedMesh->SetGroup( 0 );
		::SetTransformView( matView );
		::SetTransformProj( matProj );

		O3D_ELEMENT* pElem = g_pBipedMesh->GetParts( PARTS_HAIR );
		
		if( pElem && pElem->m_pObject3D && g_pPlayer )
		{
			pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR / (CWorld::m_light.Diffuse.r*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG / (CWorld::m_light.Diffuse.g*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB / (CWorld::m_light.Diffuse.b*g_fHairLight);
		}
//#ifdef __VCRITICAL
		if( g_pPlayer->IsDie() )
		{
			D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
			::SetLight( TRUE );
			SetDiffuse( 1.0f, 0.3f, 0.3f );
#ifdef __YENV
			SetAmbient( 0.9f, 0.9f, 0.9f );
#else //__YENV
			SetAmbient( 0.3f, 0.3f, 0.3f );
#endif //__YENV
			SetLightVec( vDir );		}
		else
		{	
			int nHitPercent = g_pPlayer->GetHitPointPercent( 100 );
			if( nHitPercent < CRITICAL_BERSERK_HP )
			{
				::SetLight( TRUE );
				SetDiffuse( 1.0f, 0.3f, 0.3f );
#ifdef __YENV
				D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
				SetAmbient( 1.0f, 1.0f, 1.0f );
#else //__YENV
				D3DXVECTOR3 vDir( 0.0f, 1.0f, 0.0f );
				SetAmbient( 0.55f, 0.55f, 0.55f );
#endif //__YENV
				SetLightVec( vDir );
			}
			else
			{	
#ifdef __YENV
				D3DXVECTOR3 vDir( 0.0f, 0.0f, 1.0f );
				SetLightVec( vDir );
				SetDiffuse( 1.0f, 1.0f, 1.0f );
#else //__YENV
				SetDiffuse( 0.0f, 0.0f, 0.0f );
#endif //__YENV
				
				SetAmbient( 1.0f, 1.0f, 1.0f );
			}			
		}
//#else
//		SetDiffuse( 0.0f, 0.0f, 0.0f );
//		SetAmbient( 1.0f, 1.0f, 1.0f );
//#endif
#ifdef __YENV
		if( g_pPlayer->m_pModel )
		{
			O3D_ELEMENT *pElem = ((CModelObject*)g_pPlayer->m_pModel)->GetParts( PARTS_HAIR );
			
			if( pElem )
			{
				if( pElem->m_pObject3D )
				{
					pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR;
					pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG;
					pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB;
				}
			}
		}	
#endif //__YENV

/*
#ifdef __YPARTS_EX
		// 외투의상을 입었다면 주방어구 의상은 숨긴다...	
		int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
		int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
		
		O3D_ELEMENT *pElem1 = NULL;
		O3D_ELEMENT *pElem2 = NULL;
		
		for( int nArry=0; nArry<5; nArry++ )
		{
			pElem1 = g_pBipedMesh->GetParts( nArryEquip1[nArry] );
			pElem2 = g_pBipedMesh->GetParts( nArryEquip2[nArry] );
			
			if( pElem2 )
			{
				if( pElem1 )
					pElem1->m_nEffect |= XE_HIDE;
			}
		}
#endif //__YPARTS_EX	
*/
	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(g_pBipedMesh);
		
	#ifdef __YHAIR_HIDE
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		O3D_ELEMENT* pElement   = NULL;
		CItemElem* pItemElem	= g_pPlayer->GetEquipItem( PARTS_CAP );

		if( pItemElem )
		{
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
				else
				{
					//g_pBipedMesh->SetEffectOff(PARTS_HAIR, XE_HIDE );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
					}
					
					g_pBipedMesh->SetEffectOff(PARTS_HEAD, XE_HIDE );					
				}
			}
		#endif //__YPARTS_EX							
			
		}
		else
		{
		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{					
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}
		#endif //__YPARTS_EX							
		}
	#endif //__YHAIR_HIDE

		g_pBipedMesh->Render(pd3dDevice,&matWorld);

		SetDiffuse( 0.0f, 0.0f, 0.0f );
		SetAmbient( 1.0f, 1.0f, 1.0f );
	}

	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}
HRESULT CWndStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBHPGauge == NULL )
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBMPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBFPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL );
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBPXPGauge, NULL );
		m_nHPWidth = -1;
		m_nMPWidth = -1;
		m_nFPWidth = -1;
		m_nEXPWidth = -1;
		m_nPXPWidth = -1;
	}

#ifdef __YDEBUG
	m_texGauEmptyNormal.SetInvalidate(m_pApp->m_pd3dDevice); 
	m_texGauEmptySmall.SetInvalidate(m_pApp->m_pd3dDevice);  
	m_texGauFillNormal.SetInvalidate(m_pApp->m_pd3dDevice);  
	m_texGauFillSmall.SetInvalidate(m_pApp->m_pd3dDevice);   
#endif //__YDEBUG
	
	return S_OK;
}
HRESULT CWndStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBHPGauge );
    SAFE_RELEASE( m_pVBMPGauge );
    SAFE_RELEASE( m_pVBFPGauge );
    SAFE_RELEASE( m_pVBEXPGauge );
    SAFE_RELEASE( m_pVBPXPGauge );

#ifdef __YDEBUG
	m_texGauEmptyNormal.Invalidate(); 
	m_texGauEmptySmall.Invalidate(); 
	m_texGauFillNormal.Invalidate(); 
	m_texGauFillSmall.Invalidate(); 
#endif //__YDEBUG
	
	return S_OK;//return S_OK;
}
HRESULT CWndStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

/*
void CWndStatus::RenderGauge( C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVBGauge, CTexture* pTexture )
{
//	pTexture = &m_texGauEmptyNormal;
    LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice; 
	CPoint pt = pRect->TopLeft();
	pt += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	///////////////////////////////////////////////////////////////
	int nTexWidth = ( pTexture->m_size.cx ) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left   = pt.x;
	FLOAT top    = pt.y;
	FLOAT right  = pt.x + nTexWidth;//( pTexture->m_size.cx );
	FLOAT bottom = pt.y + nTexHeight;//( pTexture->m_size.cy );

	int nWidth = ( pRect->Width() / nTexWidth );// - 2;

	// 기본 패턴으로 완성될 수 있는건 2이다. 2보다 작으면 이미지가 깨질 수 있으니 리턴.
	if( nWidth < 2 )
		return;
	
	int nTileNum = 3;
	int nVertexNum = 3 * 6;
	
	DRAWVERTEX* pVertices,* pVertices_;
	HRESULT hr = m_pVBGauge->Lock( 0, sizeof(DRAWVERTEX) * nVertexNum, (void**) &pVertices_, 0 );
	if(hr != D3D_OK) return;
	{
		SIZE size = m_texGauEmptyNormal.m_size;//
		SIZE sizePitch = m_texGauEmptyNormal.m_sizePitch;//
		size.cx /= 3;
		
		pVertices = pVertices_;
		for( int i = 0; i < nVertexNum; i++ )
		{
			pVertices->z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left   = pt.x;
		right  = pt.x + nTexWidth;
			
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		//////////////////////////////////
		
		left   = pt.x + nTexWidth;
		right  = pt.x + nTexWidth + ( ( nWidth - 2 ) * nTexWidth );
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 1.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

		//////////////////////////////////
		left   = pt.x + ( ( nWidth - 1 ) * nTexWidth );
		right  = pt.x + ( ( nWidth ) * nTexWidth );

		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)top - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)left - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 2.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;
		
		pVertices->x = (FLOAT)right - 0.5f;
		pVertices->y = (FLOAT)bottom - 0.5f;
		pVertices->u = ( (FLOAT)size.cx / sizePitch.cx ) * 3.0f;
		pVertices->v = ( (FLOAT)size.cy / sizePitch.cy ) * 1.0f;
		pVertices++;

	}
	m_pVBGauge->Unlock(); 

	///////////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, 1 );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	












	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetTexture( 0, pTexture->m_pTexture );
	pd3dDevice->SetFVF( D3DFVF_DRAWVERTEX );
	pd3dDevice->SetStreamSource( 0, m_pVBGauge, 0,sizeof( DRAWVERTEX ) );
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, nVertexNum / 3 );
}				
*/
void CWndStatus::OnInitialUpdate()
{
//	m_pModel = prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice,g_pPlayer->m_dwType,g_pPlayer->m_dwIndex);
//	prj.m_modelMng.LoadMotion(m_pModel,g_pPlayer->m_dwType,g_pPlayer->m_dwIndex,0);
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	//m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( DRAWVERTEX ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	

	//m_texGauEmptySmall  
	//m_texGauFillNormal  
	//m_texGauFillSmall   

	
	// 장착, 게이지에 나올 캐릭터 오브젝트 설정 
	if( g_pBipedMesh == NULL )
	{
		int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		g_pBipedMesh = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( g_pBipedMesh,  OT_MOVER, nMover, 0 );
#ifdef __FINITEITEM0705
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, g_pBipedMesh, &g_pPlayer->m_Inventory );
#else	// __FINITEITEM0705
		CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_adwEquipment, g_pBipedMesh, &g_pPlayer->m_Inventory );
#endif	// __FINITEITEM0705
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top );
	Move( point );

}

BOOL CWndStatus::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_STATUS );

//	CRect rect( 0, 0, WND_WIDTH, 90 ); 
//	CMover* pMover = CMover::GetActiveMover();
//	SetTitle( pMover->m_szName);
//	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, &g_WndMng, dwWndId);
}

BOOL CWndStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 100: // wnd2
			/*
			g_Music.m_nMusicVolume = !g_Music.m_nMusicVolume;
			if ( g_Music.m_nMusicVolume == 0 )
				g_Music.Stop();
			else
				g_Music.Play();
				*/
			break;
		case 101: // wnd1 
			g_SoundMng.m_nSoundVolume = !g_SoundMng.m_nSoundVolume;
			break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndStatus::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndStatus::SetWndRect( CRect rectWnd, BOOL bOnSize  )
{
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;
	
	CWndNeuz::SetWndRect( rectWnd, bOnSize );
}
void CWndStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndStatus::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if(++m_nDisplay > 2)
		m_nDisplay = 0;
}
BOOL CWndStatus::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd( p2DRender );
	//oint pt = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	CRect rect = GetClientRect();
	p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );


	//p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 255, 70, 70, 170 ) );
	p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 100, 0, 0, 0 ) );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 퀘스트 아이템 정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef	__NQUESTITEM_INFO_040917
CWndQuestItemInfo::CWndQuestItemInfo() : m_pItemBase(NULL)
{
}

CWndQuestItemInfo::~CWndQuestItemInfo()
{
}

BOOL CWndQuestItemInfo::Initialize( CWndBase* pWndParent, CItemBase* pItemBase )
{
	return Create( pItemBase, APP_QUESTITEM_INFO, pWndParent );//RUE;
}
BOOL CWndQuestItemInfo::Create( CItemBase* pItemBase, UINT nID, CWndBase* pWndParent )
{
	m_pItemBase = pItemBase;
	CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), nID, 0, m_Position, pWndParent );
	((CWndEdit*)GetDlgItem( WIDC_EDIT1 ))->SetVisible( FALSE );

	MoveParentCenter();
	return 1;
}

BOOL CWndQuestItemInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndQuestItemInfo::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_EDIT1 );
	CRect rectCtrl = lpWndCtrl->rect;
	rectCtrl.top += 10;
	rectCtrl.left += 10;
	rectCtrl.right -= 10;
	rectCtrl.bottom -= 10;

	m_pItemBase->GetTexture()->Render(p2DRender, rectCtrl.TopLeft(), 200);
	p2DRender->TextOut(rectCtrl.left + 34, rectCtrl.top + 16 - 5, 1, 1, m_pItemBase->GetProp()->szName, 0xff0000ff);

	CEditString	string = "";
	string.Init( m_pFont, &rectCtrl );
	string = m_pItemBase->GetProp()->szCommand;
	p2DRender->TextOut_EditString( rectCtrl.left , rectCtrl.top + 40/* '+40'의 의미는 아이콘있는 곳에서 한줄 아래에 찍겠다는 의미 */, string);
}

void CWndQuestItemInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
		pWndEdit->EnableWindow( FALSE );
}

BOOL CWndQuestItemInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase *pWndBase)
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndQuestItemInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndQuestItemInfo::OnLButtonUp( UINT nFlags, CPoint point )
{
}

void CWndQuestItemInfo::OnLButtonDown( UINT nFlags, CPoint point )
{
}

#endif	//__NQUESTITEM_INFO_040917
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMacro::CWndMacro()
{
}
CWndMacro::~CWndMacro()
{
}
void CWndMacro::OnDraw( C2DRender* p2DRender )
{

}
void CWndMacro::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndMacro::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	//SetTitle( GETTEXT( TID_APP_MACRO )  );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndMacro::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndMacro::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndMacro::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndMacro::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndMacro::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 스타일
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndStyle::CWndStyle()
{
}
CWndStyle::~CWndStyle()
{
}
void CWndStyle::OnDraw( C2DRender* p2DRender )
{

}
void CWndStyle::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndStyle::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	//SetTitle( GETTEXT( TID_APP_STYLE )  );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndStyle::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndStyle::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndStyle::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndStyle::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndStyle::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 내정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
CWndOptMyInfo::CWndOptMyInfo()
{
}
CWndOptMyInfo::~CWndOptMyInfo()
{
}
void CWndOptMyInfo::OnDraw( C2DRender* p2DRender )
{

}
void CWndOptMyInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndOptMyInfo::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( GETTEXT( TID_APP_OPTION_MYINFO )  );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndOptMyInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndOptMyInfo::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndOptMyInfo::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndOptMyInfo::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndOptMyInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 단축키
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndOptHotkey::CWndOptHotkey()
{
}
CWndOptHotkey::~CWndOptHotkey()
{
}
void CWndOptHotkey::OnDraw( C2DRender* p2DRender )
{

}
void CWndOptHotkey::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndOptHotkey::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	SetTitle( GETTEXT( TID_APP_OPTION_HOTKEY ) );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndOptHotkey::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndOptHotkey::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndOptHotkey::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndOptHotkey::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndOptHotkey::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndOptTheme::CWndOptTheme()
{
}
CWndOptTheme::~CWndOptTheme()
{
}
void CWndOptTheme::OnDraw( C2DRender* p2DRender )
{

}
void CWndOptTheme::OnInitialUpdate()
{
	CRect rect = GetClientRect();
	m_wndPaper1.Create( _T( "바람부는 들판" ), 0, CRect( 5,  5, rect.Width() - 5, 28), this, 100 );
	m_wndPaper2.Create( _T( "붉은 꽃"       ), 0, CRect( 5, 30, rect.Width() - 5, 53), this, 101 );
	m_wndPaper3.Create( _T( "비누 방울"     ), 0, CRect( 5, 55, rect.Width() - 5, 78), this, 102 );
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndOptTheme::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 50 ,50, 300, 300 );
	//SetTitle( GETTEXT( TID_APP_OPTION_THEME ) );
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndOptTheme::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndOptTheme::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndOptTheme::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndOptTheme::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndOptTheme::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case 100: m_pTheme->m_texWndPaper.DeleteDeviceObjects(); m_pTheme->m_texWndPaper.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "바람부는 들판.bmp" ), 0xff0000 ); break;
	case 101: m_pTheme->m_texWndPaper.DeleteDeviceObjects(); m_pTheme->m_texWndPaper.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "붉은 꽃.bmp" ), 0xff0000 ); break;
	case 102: m_pTheme->m_texWndPaper.DeleteDeviceObjects(); m_pTheme->m_texWndPaper.LoadTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "비누 방울.bmp" ), 0xff0000 ); break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndEmotion::CWndEmotion() 
{ 
	m_lpSelectTreeElem = NULL;
} 
CWndEmotion::~CWndEmotion() 
{ 
} 
void CWndEmotion::OnDraw( C2DRender* p2DRender ) 
{ 
	CRect rect = GetClientRect();
/*
	CD3DFont* pFont = p2DRender->m_pFont;//GetFont();
	p2DRender->m_pFont = m_pTheme->m_pFontGameTitle;
	p2DRender->TextOut( 5, 10, _T( "Lv.0 기본 제스춰" ) );
	p2DRender->TextOut( 5, 40, _T( "Lv.1 생활 표현" ) );
	p2DRender->TextOut( 5, 70, _T( "Lv.2 도발" ) );
	p2DRender->TextOut( 5,100, _T( "Lv.3 춤" ) );
	p2DRender->TextOut( 5,130, _T( "Lv.4 곡예" ) );
	p2DRender->TextOut( 5,160, _T( "Lv.5 무술" ) );

	p2DRender->m_pFont = pFont;
	*/
	p2DRender->TextOut( 5, rect.Height() - 50, _T( "연결 대사" ) );
} 
void CWndEmotion::OnInitialUpdate() 
{ 
/*
	m_wndViewCtrl.Create( WBS_CHILD, CRect( 5, 5, m_rectClient.Width() - 5,m_rectClient.Height() - 60 ), this, 1005 );//,m_pSprPack,-1);//m_pSprPack,16);

	PropMotion* pProp;
	for( int i = 0; i < prj.m_aPropMotion.GetSize(); i++ )
	{
		pProp = prj.GetPropMotion( i );
		if( pProp && pProp->dwRequireLv )
		{
			LPTREEELEM lpTreeElem = m_wndViewCtrl.FindTreeElem( pProp->szRoot );
			if( lpTreeElem == NULL )
			{
				lpTreeElem = m_wndViewCtrl.InsertItem( NULL, pProp->szRoot, 0 ); // root
				m_wndViewCtrl.InsertItem( lpTreeElem, pProp->szName, pProp->dwID ) ;
			}
			else
				m_wndViewCtrl.InsertItem( lpTreeElem, pProp->szName, pProp->dwID );
		}
	}
	CRect rect = GetClientRect();
	rect.top = rect.bottom - 55;
	rect.bottom = rect.top + 20;
	rect.left = 70;
	rect.right -= 5;
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), 0, rect, this, 1001 );

	rect = GetClientRect();
	rect.top = rect.bottom - 30;
	rect.bottom = rect.top + 25;
	rect.left = rect.right - 80;
	rect.right -= 5;
	m_wndDefault.Create( _T( "Default" ), 0, rect, this, 1002 );
*/
	CWndNeuz::OnInitialUpdate(); 
} 
BOOL CWndEmotion::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	//SetTitle( _T( "감정표현" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
BOOL CWndEmotion::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndEmotion::OnSize( UINT nType, int cx, int cy ) 
{ 
	m_wndViewCtrl.SetWndRect( CRect( 5, 5, m_rectClient.Width() - 5, m_rectClient.Height() - 60 ) );

	CRect rect = GetClientRect();
	rect.top = rect.bottom - 55;
	rect.bottom = rect.top + 20;
	rect.left = 70;
	rect.right -= 5;
	m_wndEdit.SetWndRect( rect );

	rect = GetClientRect();
	rect.top = rect.bottom - 30;
	rect.bottom = rect.top + 25;
	rect.left = rect.right - 80;
	rect.right -= 5;
	m_wndDefault.SetWndRect( rect );

	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndEmotion::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndEmotion::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndEmotion::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	/*
	switch( nID )
	{
	case 1001:
		if( message == EN_CHANGE && m_lpSelectTreeElem )
		{
			_tcscpy( prj.GetPropMotion( m_lpSelectTreeElem->m_dwData )->szLink, m_wndEdit.m_string );
		}
	case 1005: // view ctrl
		{
			if( message == WNM_DBLCLK )
			{
				LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
				if( lpTreeElem )
				{
					if( lpTreeElem->m_dwData )
					{
						g_pPlayer->SendActMsg( OBJMSG_MOTION, lpTreeElem->m_dwData );
						m_wndEdit.SetString( prj.GetPropMotion( lpTreeElem->m_dwData )->szLink );
						m_lpSelectTreeElem = lpTreeElem;
					}
					else
						;//g_pPlayer->SetMotion( lpTreeElem->m_dwData );
				}
			}
		}
	}
	*/
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndFriend, _T( "친구" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndGuild, _T( "길드" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndClub, _T( "클럽" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndMessage, _T( "매신저" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndEMail, _T( "이메일" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndBBS, _T( "게시판" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndSearchPeople, _T( "사람찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndSearchGuild, _T( "길드찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndSearchClub, _T( "클럽찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndSearchShop, _T( "상점찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


EMPTY_WNDCLASSMEMBER( CWndInfoEvent, _T( "이벤트" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndInfoVersion, _T( "버전" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndInfoSchedule, _T( "개발 스케쥴" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndJoinVote, _T( "전자투표" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndJoinEvent, _T( "이벤트 참여 신청" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

EMPTY_WNDCLASSMEMBER( CWndSequence, _T( "시퀀스" ) )


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 카드 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


CTextureMng CWndCard::m_textureMng;
CTexture* CWndCard::m_pTexCardFront = NULL;
CTexture* CWndCard::m_pTexCardBack = NULL;

CWndCard::CWndCard()
{
	m_bFront = TRUE;
//	m_pCardElem = NULL;
	m_pModel = NULL;
}
CWndCard::~CWndCard()
{
}
BOOL CWndCard::OnEraseBkgnd(C2DRender* p2DRender)
{
	if( m_bFront )
	{
		// 카드 
		p2DRender->RenderTexture( CPoint( 0, 0 ), m_pTexCardFront );

		//p2DRender->TextOut( 7, 7, _T( "Justice" ), 0xff000000 );
		p2DRender->TextOut( 5, 5, _T( "Justice" ), D3DCOLOR_ARGB( 255, 180, 180, 255 ) );
		p2DRender->TextOut( 6, 5, _T( "Justice" ), D3DCOLOR_ARGB( 255, 180, 180, 255 ) );

		CMover* pMover = g_pPlayer;
		LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );		
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );		

		pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255) );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		CRect rect = GetClientRect();

		// 뷰포트 세팅 
		D3DVIEWPORT9 viewport;

		viewport.X      = p2DRender->m_ptOrigin.x + 5;//100;// + 5;
		viewport.Y      = p2DRender->m_ptOrigin.y + 35;// + 5;
		viewport.Width  = p2DRender->m_clipRect.Width() - 10;//105;
		viewport.Height = p2DRender->m_clipRect.Height() - 55;
		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;

//		if(viewport.Width > p2DRender->m_clipRect.Width())
//		;//ewport.Width = p2DRender->m_clipRect.Width();
//		if(viewport.Height > p2DRender->m_clipRect.Height())
//			;//viewport.Height = p2DRender->m_clipRect.Height();

		pd3dDevice->SetViewport(&viewport);
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 ) ;

		// 프로젝션 
		/*
		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

		FLOAT fAspect = ((FLOAT)viewport.Width) / (FLOAT)viewport.Height;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		*/
		D3DXMATRIX matProj;
		D3DXMatrixIdentity( &matProj );
		FLOAT fAspect = ( (FLOAT) viewport.Width ) / (FLOAT) viewport.Height ;
		FLOAT fov = D3DX_PI / 4.0f;//796.0f;
		FLOAT h = cos( fov / 2 ) / sin( fov / 2 );
		FLOAT w = h * fAspect;
		D3DXMatrixOrthoLH( &matProj, w, h, 1.0f, 10.0f );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		// 카메라 
		D3DXMATRIX  matView;
		D3DXVECTOR3 vecLookAt( 0.0f, 0.0f, 2.5f );
		D3DXVECTOR3 vecPos(  0.0f, 0.0f, -3.0f );
		D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );

		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

		// 월드 
		D3DXMATRIXA16 matWorld;
		D3DXMATRIXA16 matScale;
		D3DXMATRIXA16 matRot;
		D3DXMATRIXA16 matTrans;

		// 초기화 
		D3DXMatrixIdentity(&matScale);
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matWorld);
		
		D3DXMatrixScaling(&matScale,1.7f,1.7f,1.7f);
		D3DXMatrixTranslation(&matTrans,0.0f,-1.5f,0.0f);
		//D3DXMatrixRotationY( &matRot, D3DXToRadian( -20 ) );

		D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
		D3DXMatrixMultiply(&matWorld, &matWorld,&matRot);
		D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		// 랜더링 
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

		m_pModel->FrameMove();
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );

		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		p2DRender->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		viewport.X      = p2DRender->m_ptOrigin.x;// + 5;
		viewport.Y      = p2DRender->m_ptOrigin.y;// + 5;
		viewport.Width  = p2DRender->m_clipRect.Width();
		viewport.Height = p2DRender->m_clipRect.Height();
		viewport.MinZ   = 0.0f;
		viewport.MaxZ   = 1.0f;
		pd3dDevice->SetViewport(&viewport);

	}
	else
		p2DRender->RenderTexture( CPoint( 0, 0 ), m_pTexCardBack );

	return TRUE;
}
void CWndCard::OnDraw(C2DRender* p2DRender)
{
}

void CWndCard::OnInitialUpdate()
{
	m_pTexCardFront = m_textureMng.AddTexture( g_Neuz.m_2DRender.m_pd3dDevice, MakePath( DIR_ITEM, "cardFront.tga" ), (DWORD)0 );
	m_pTexCardBack  = m_textureMng.AddTexture( g_Neuz.m_2DRender.m_pd3dDevice, MakePath( DIR_ITEM, "cardBack.tga" ), (DWORD)0 );

	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, g_pPlayer->m_dwType, g_pPlayer->m_dwIndex, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel, g_pPlayer->m_dwType, g_pPlayer->m_dwIndex, MTI_STAND );

	m_wndMessage. Create( _T( "Message"  ), WBS_TEXT|WBS_HIGHLIGHTPUSH|WBS_HIGHLIGHT, CRect( 5, 240, 60, 255), this, 1002 );
	m_wndTeleport.Create( _T( "Teleport" ), WBS_TEXT|WBS_HIGHLIGHTPUSH|WBS_HIGHLIGHT, CRect( 70, 240, 127, 255), this, 1003 );

	m_wndMessage.SetFontColor( D3DCOLOR_ARGB( 255, 180, 180, 255 ) );
	m_wndMessage.SetHighLightColor( D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
	m_wndTeleport.SetFontColor( D3DCOLOR_ARGB( 255, 180, 180, 255 ) );
	m_wndTeleport.SetHighLightColor( D3DCOLOR_ARGB( 255, 255, 255, 0 ) );

	CWndNeuz::OnInitialUpdate();
}
BOOL CWndCard::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 240, 0, 240 + 240, 90 ); 
	CRect rect( 0, 0, 128, 256 ); 
	//m_wndUserArray.SetSize(5);
//	SetTitle( GETTEXT( TID_APP_JOB )  );
	return CWndNeuz::Create(WBS_MOVE|WBS_SOUND|WBS_NOFRAME,rect,pWndParent,dwWndId);
}
BOOL CWndCard::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{

	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndCard::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndCard::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1( x - ( size.cx / 2), y, ( x - ( size.cx / 2 ) ) + size.cx, y + size.cy );

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	//m_wndButton1.SetWndRect( rect3_1 );
	//m_wndButton2.SetWndRect( rect3_2 );
	//m_wndButton3.SetWndRect( rect3_3 );

	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndCard::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCard::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndCard::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	m_bFront = !m_bFront;
}
void CWndCard::OnRButtonUp(UINT nFlags, CPoint point)
{
	Destroy();
}
BOOL CWndCard::IsPickupSpace(CPoint ptWindow)
{
	CRect rect = GetWindowRect();
	if(rect.PtInRect(ptWindow))
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndLogOut : 계정 화면으로, 캐릭터 선택 화면으로 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWndLogOut::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
/*
	Create( _T( "매시지 박스" ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( "접속을 종료하시겠습니까?" ) );
*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0069) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
	
	return CWndMessageBox::Initialize( pWndParent, dwWndId );
}
BOOL CWndLogOut::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:     
				if( g_Option.m_nLanguage == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}
#ifdef __NPP_050308
				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		

				if( g_pPlayer->IsRegionAttr( RA_SAFETY ))
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
						g_WndMng.m_pLogOutWaitting->Initialize(this);
						g_WndMng.m_pLogOutWaitting->SetIsLogOut(TRUE);
						SetVisible(FALSE);
					}
				}
#else //__NPP_050308
				::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
#endif //__NPP_050308
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


#ifdef __NPP_050308
CWndLogOutWaitting::CWndLogOutWaitting() 
{ 
} 
CWndLogOutWaitting::~CWndLogOutWaitting() 
{ 
} 

BOOL CWndLogOutWaitting::Initialize( CWndBase* pWndParent ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_MESSAGE_TEXT );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_MESSAGE_TEXT ); 
} 


BOOL CWndLogOutWaitting::Process()
{
	DWORD dwCurrentTime = timeGetTime();
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	DWORD dwEndWaitTime = dwTime - dwCurrentTime;
	dwEndWaitTime /= 1000;

	CString str;
	str.Format( prj.GetText( TID_PK_TIME_END ), dwEndWaitTime );
	
	if( dwEndWaitTime > 0 )
	{
		pWndText->SetString( str );
	}
	
	if( dwCurrentTime > dwTime )
	{
		if( m_bIsLogOut )
		{
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
		}
		else
		{
			::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
		}

		Destroy(); 
	}
	return TRUE;
}

BOOL CWndLogOutWaitting::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndLogOutWaitting::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate(); 

	m_wndTitleBar.SetVisible( FALSE );
	
	m_bIsLogOut = FALSE;
	
	dwTime = timeGetTime()+SEC(TIMEWAIT_CLOSE);

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

#ifdef __ONLINE
	// 종료 패킷 서버 전송
	g_DPlay.SendReqLeave();	
#endif //__ONLINE
}
#endif //__NPP_050308

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndQuit  : 프로그램 종료
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CWndQuit::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	/*
	Create( _T( "매시지 박스" ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( "프로그램을 종료하시겠습니까?" ) );
	*/
	Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( prj.GetText(TID_DIAG_0070) ) );
	m_wndText.ResetString();
	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
	
	return CWndMessageBox::Initialize( pWndParent, dwWndId );
}
BOOL CWndQuit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == WNM_CLICKED  )
	{
		switch(nID)
		{
			case IDCANCEL:   
				Destroy(); 
				break;
			case IDOK:       
				if( g_Option.m_nLanguage == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}
#ifdef __NPP_050308
				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		
				
				if( g_pPlayer->IsRegionAttr( RA_SAFETY ))
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
						g_WndMng.m_pLogOutWaitting->Initialize(this);
						g_WndMng.m_pLogOutWaitting->SetIsLogOut(FALSE);
						SetVisible(FALSE);
					}
				}
#else //__NPP_050308				
				::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
#endif //__NPP_050308
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndRevival : 부활 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRevival::CWndRevival() 
{ 
	m_pLodeLight = NULL;
	m_pLodeStar = NULL;
	m_pRevival = NULL;
} 
CWndRevival::~CWndRevival() 
{ 
} 
void CWndRevival::OnDraw( C2DRender* p2DRender ) 
{ 
#ifdef __S050524_SM_ITEM
	if( NULL == g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION ) )
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );
#endif // __S050524_SM_ITEM
} 
void CWndRevival::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pLodeLight = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODELIGHT );
	m_pLodeStar = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODESTAR );
#ifdef __S050524_SM_ITEM
	m_pRevival = (CWndButton*)GetDlgItem( WIDC_REVIVAL_STAND );
#endif // __S050524_SM_ITEM

	//pLodeLight->AddWndStyle( WBS_NODRAWFRAME );
	//pLodeStar->AddWndStyle( WBS_NODRAWFRAME );

	m_pLodeLight->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODELIGHT) );//"로드라이트는 여행자가 직접 지정한 부활 위치입니다." );
	m_pLodeStar->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODESTAR) );//"로드스타는 인근 마을의 부활 위치입니다." );
#ifdef __S050524_SM_ITEM
	m_pRevival->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_OGNPOINT) );
	if( NULL == g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_SCR_RESURRECTION ) )
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );
#endif // __S050524_SM_ITEM
	
	CWndWebBox* pWndWebBox = (CWndWebBox*)g_WndMng.GetApplet( APP_WEBBOX );

	if(pWndWebBox)
		pWndWebBox->Destroy();
	
	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
BOOL CWndRevival::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_REVIVAL, WBS_MODAL );

	if( g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->GetMouseMode() == 1 )	// FPS모드일때
	{
		g_WndMng.m_pWndWorld->SetMouseMode( 0 );
	}
	
	return TRUE;
} 
BOOL CWndRevival::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRevival::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRevival::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRevival::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRevival::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_REVIVAL_TO_LODELIGHT:
			{
#ifdef __ONLINE
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODELIGHT );
#else	// __ONLINE
				g_pPlayer->m_pActMover->ClearState();
				g_pPlayer->m_nHitPoint	= g_pPlayer->GetMaxHitPoint();
#endif
				break;
			}
		case WIDC_REVIVAL_TO_LODESTAR:
			{
#ifdef __ONLINE
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODESTAR );
#else	// __ONLINE
				g_pPlayer->m_pActMover->ClearState();
				g_pPlayer->m_nHitPoint	= g_pPlayer->GetMaxHitPoint();
#endif
				break;
			}
#ifdef __S050524_SM_ITEM
		case WIDC_REVIVAL_STAND:
			{
#ifdef __ONLINE
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL );
#endif	// __ONLINE			
				break;
			}
#endif // __S050524_SM_ITEM
	}	
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

#ifdef __NPP_050308

CWndRecoverChao::CWndRecoverChao() 
{ 
} 
CWndRecoverChao::~CWndRecoverChao() 
{ 
} 
void CWndRecoverChao::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRecoverChao::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	pWndText->SetString(prj.GetText(TID_PK_DEATH_MESSAGE1));

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
BOOL CWndRecoverChao::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX_RECOVERCHAO, WBS_MODAL );

	return TRUE;
} 
BOOL CWndRecoverChao::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRecoverChao::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRecoverChao::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRecoverChao::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRecoverChao::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bLodeFlag = FALSE;

	switch( nID )
	{
		case WIDC_BUTTON1:
			{
#ifdef __ONLINE
				g_DPlay.SendRecoverChaoMode();
#endif //__ONLINE
				g_WndMng.PutString( prj.GetText(TID_PK_DEATH_MESSAGE2) );
				
				Destroy();			
			}
			break;

		case WIDC_BUTTON2:
			{
				Destroy();			
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}



///////////////////////////////////////////////////////////////////////////
// CWantedMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWantedMessageBox : public CWndMessageBox
{ 
public: 
	int m_nGold;
	CString m_strMsg;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWantedMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
//	return CWndMessageBox::Initialize( "공소시효가 지나면 현상금은 소멸됩니다. "
//		                               "정말로 현상금을 거시겠습니까?", 
//	                                   pWndParent, 
//									   MB_OKCANCEL );
	return CWndMessageBox::Initialize( prj.GetText(TID_PK_INPUT_TIMEWARN), 
	                                   pWndParent, 
									   MB_OKCANCEL );

}

BOOL CWantedMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
#ifdef __ONLINE
		{
			g_DPlay.SendWantedGold( m_nGold, m_strMsg );
			Destroy();
		}
#endif
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

///////////////////////////////////////////////////////////////////////////
// CWndReWanted
//////////////////////////////////////////////////////////////////////////////
CWndReWanted::CWndReWanted() 
{ 
} 

CWndReWanted::~CWndReWanted() 
{ 
} 
void CWndReWanted::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndReWanted::SetWantedName( LPCTSTR szWanted )
{
	m_strWanted = szWanted;
}

void CWndReWanted::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetFocus();	

	GetDlgItem( WIDC_STATIC_TARGETNAME )->SetTitle( m_strWanted );

	MoveParentCenter();	// 윈도를 중앙으로 옮기는 부분.
} 

BOOL CWndReWanted::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_REWARD_INPUT );
	return TRUE;
} 

void CWndReWanted::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndReWanted::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndReWanted::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndReWanted::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

BOOL CWndReWanted::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 || message == EN_RETURN )
	{
		CWndEdit* pWndGold = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndEdit* pWndMsg  = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		if( pWndGold && pWndMsg && g_pPlayer )
		{
			if( strlen(pWndGold->GetString()) <= 0 )
				return FALSE;
			CString strMsg = pWndMsg->GetString();
			int nGold = atoi( pWndGold->GetString() ); 
			
			g_WndMng.WordChange( strMsg );

			if( CheckWantedInfo( nGold, strMsg ) == TRUE )
			{
				CWantedMessageBox* pBox = new CWantedMessageBox;
				pBox->m_nGold = nGold;
				pBox->m_strMsg = strMsg;
				g_WndMng.OpenCustomBox( "", pBox );
				Destroy();
			}
			else
				return FALSE;
		}

		Destroy();
	}
	else if( nID == WIDC_BUTTON2 )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

BOOL CWndReWanted::CheckWantedInfo( int nGold, LPCTSTR szMsg )
{
//  서버에서 체크 
//	if( g_pPlayer->m_idMurderer == 0 )		// 나를 죽인자가 없었으면 현상금을 걸 수 없다.
//	{
//		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_PK_REWARD_NO) );
//		return FALSE;
//	}

	if( szMsg[0] == '\0' )
	{
//		g_WndMng.OpenMessageBoxUpper( "십자평을 입력해 주십시요." );
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_PK_REPU_INPUT) );
		return FALSE;
	}

	if( strlen(szMsg) > WANTED_MSG_MAX )
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GUILD_NOTICE_ERROR) );
		return FALSE;
	}

	if( nGold < MIN_INPUT_REWARD || nGold > MAX_INPUT_REWARD )			// 현상금은 최소 1000패냐에서 최대 2억 패냐까지 걸 수 있다. 
	{
		char szWarning[256];
//		wsprintf( szWarning, "입력한 금액이 범위(%d - %d)를 벗어났습니다.", MIN_INPUT_REWARD, MAX_INPUT_REWARD );		
		wsprintf( szWarning, prj.GetText(TID_PK_MONEY_RANGE), MIN_INPUT_REWARD, MAX_INPUT_REWARD );		
		g_WndMng.OpenMessageBoxUpper( szWarning );
		return FALSE;
	}

	int nTax = MulDiv( nGold, 10, 100 );					// 건 현상금의 10%는 수수료로 지급된다. 
	if( g_pPlayer->GetGold() < (nGold + nTax) ) 
	{
		g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LACKMONEY) );	// 인벤에 돈이부족
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWantedConfirm
////////////////////////////////////////////////////////////////////////////////////////
CWndWantedConfirm::CWndWantedConfirm() 
{ 
	m_nGold = 0;
	memset( m_szName, 0, sizeof(m_szName) );
} 
CWndWantedConfirm::~CWndWantedConfirm() 
{ 
} 
void CWndWantedConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndWantedConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndWantedConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_WANTED_CONFIRM, WBS_MODAL );
	
	return TRUE;
} 
BOOL CWndWantedConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndWantedConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndWantedConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndWantedConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndWantedConfirm::SetInfo(const char szName[], const int nGold)
{
	m_nGold  = nGold;
	_tcscpy( m_szName, szName );
	
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	pWndStatic->SetTitle(szName);

	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );

	CString strMsg1, strMsg2;

	strMsg1 = prj.GetText( TID_PK_POINT_SHOW );
	strMsg2.Format( prj.GetText(TID_PK_WASTE_SHOW), REQ_WANTED_GOLD );

	pWndStatic->SetTitle( strMsg1 + strMsg2 );	

	 
}
BOOL CWndWantedConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case  WIDC_BUTTON1:
			{
#ifdef __ONLINE
				if( strlen(m_szName) > 0 )
				{
					g_DPlay.SendWantedInfo(m_szName);
				}
#endif //__ONLINE
				Destroy();
			}
			break;
		case  WIDC_BUTTON2:
			{
				Destroy();
			}
			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

////////////////////////////////////////////////////////////////////////////////////////
// CWndWanted
////////////////////////////////////////////////////////////////////////////////////////

const int MAX_WANTED_PER_PAGE = 20;

CWndWanted::CWndWanted() 
{ 
	Init( 0 );
} 

CWndWanted::~CWndWanted() 
{ 
	SAFE_DELETE(m_pWantedConfirm);
} 

void CWndWanted::Init( time_t lTime )
{
	memset( m_aList, 0, sizeof(m_aList) );
	m_recvTime = lTime;
	m_pWantedConfirm  = NULL;
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndWanted::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_WANTED_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_WANTED_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	CString strTitle = GetTitle();
	CTime tm( m_recvTime );
//	strTitle += tm.Format( " - %H시 %M분 %S초의 데이타입니다." );
	strTitle += tm.Format( prj.GetText(TID_PK_DATA_SHOWTIME) );
	SetTitle( strTitle );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndWanted::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_WANTED, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndWanted::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndWanted::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndWanted::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndWanted::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_WANTED_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndWanted::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndWanted::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	// 현재 더블클릭된 좌표가 선택된 인덱스인가?
	if( m_nSelect >= 0 && GetSelectIndex( point ) == m_nSelect )	
	{
		SAFE_DELETE(m_pWantedConfirm);
		m_pWantedConfirm = new CWndWantedConfirm;
		m_pWantedConfirm->Initialize( this, 0 );

		// 표시할 이름, 현상금정보 전달
		m_pWantedConfirm->SetInfo( m_aList[m_nSelect].szName, m_aList[m_nSelect].nGold );
	}
}

void CWndWanted::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndWanted::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndWanted::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndWanted::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndWanted::OnMouseWndSurface( CPoint point )
{
}	

BOOL CWndWanted::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	return TRUE;
}

void CWndWanted::InsertWanted( const char szName[], __int64 nGold, int nTime, const char szMsg[] )
{
	if( m_nMax >= MAX_WANTED_LIST )
	{
		Error( "CWndWanted::InsertWanted - range over" );
		return;
	}

	_tcscpy( m_aList[m_nMax].szName, szName );
	m_aList[m_nMax].nGold = nGold;

	CTime cTime( (time_t)nTime );		
	SYSTEMTIME st;
	st.wYear= cTime.GetYear(); 
	st.wMonth= cTime.GetMonth(); 
	st.wDay= cTime.GetDay();  

	if( GetDateFormat( LOCALE_USER_DEFAULT, 0, &st, NULL, m_aList[m_nMax].szDate, sizeof(m_aList[m_nMax].szDate) ) == 0 )
		m_aList[m_nMax].szDate[0] = '\0';

	_tcscpy( m_aList[m_nMax].szMsg, szMsg );
	m_nMax++;
}

void CWndWanted::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szGold[16];
	CString strGold;

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+570, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for( int i=nBase; i<nBase + MAX_WANTED_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		sprintf( szNum, "%3d", i+1 );
		sprintf( szGold, "%I64d", m_aList[i].nGold );
		strGold = GetNumberFormatEx( szGold );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+570, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		p2DRender->TextOut( sx + 4,    sy, szNum,             dwColor );
		p2DRender->TextOut( sx + 33,   sy, m_aList[i].szName, dwColor );
		p2DRender->TextOut( sx + 190,  sy, strGold,           dwColor );
		p2DRender->TextOut( sx + 268,  sy, m_aList[i].szDate, dwColor );
		p2DRender->TextOut( sx + 350,  sy, m_aList[i].szMsg,  dwColor );

		sy += 18;
	}
} 

///////////////////////////////////////////////////////////////////////////////////////////
// CWndResurrectionConfirm
///////////////////////////////////////////////////////////////////////////////////////////
CWndResurrectionConfirm::CWndResurrectionConfirm() 
{ 
} 
CWndResurrectionConfirm::~CWndResurrectionConfirm() 
{ 
} 
void CWndResurrectionConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndResurrectionConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
} 
BOOL CWndResurrectionConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	InitDialog( g_Neuz.GetSafeHwnd(), APP_RESURRECTION, WBS_MODAL );
	return TRUE;
} 
BOOL CWndResurrectionConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndResurrectionConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndResurrectionConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndResurrectionConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndResurrectionConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case  WIDC_BUTTON1:
		{
#ifdef __ONLINE
			g_DPlay.SendResurrectionOK();
#endif //__ONLINE
			Destroy();
		}
		break;
	case 10000:
	case  WIDC_BUTTON2:
		{
#ifdef __ONLINE
			g_DPlay.SendResurrectionCancel();
#endif //__ONLINE
			Destroy();
		}
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

#endif //__NPP_050308

#ifdef __S050524_SM_ITEM
/****************************************************
  WndId : APP_COMMITEM_DIALOG - 아이템 사용
  CtrlId : WIDC_EDIT_COMMUSE - 
  CtrlId : WIDC_BUTTON_OK - Button
****************************************************/

CWndCommItemDlg::CWndCommItemDlg() 
{ 
	m_pWndEdit = NULL;
	m_dwObjId = NULL_ID;
	m_dwCtrlId = NULL_ID;
} 
CWndCommItemDlg::~CWndCommItemDlg() 
{ 
} 
void CWndCommItemDlg::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndCommItemDlg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_COMMUSE );
	if( m_pWndEdit )
		m_pWndEdit->EnableWindow( FALSE );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}
void CWndCommItemDlg::SetItem( DWORD dwDefindText, DWORD dwObjId, DWORD dwCtrlId )
{
	m_pWndEdit->AddString( prj.GetText( dwDefindText ) );
	m_dwObjId = dwObjId;
	m_dwCtrlId = dwCtrlId;
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCommItemDlg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COMMITEM_DIALOG, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndCommItemDlg::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndCommItemDlg::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndCommItemDlg::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndCommItemDlg::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndCommItemDlg::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndCommItemDlg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK || message == EN_RETURN )
	{
		if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
			g_WndMng.GetWndBase( APP_BANK )  ||
			g_WndMng.GetWndBase( APP_TRADE ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		}
		else
		if( g_WndMng.GetWndBase( APP_REPAIR ) )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
		}
		else
		{
			if( m_dwObjId == II_SYS_SYS_SCR_CHACLA )
			{
				if( g_pPlayer->IsExpert() )
				{
					SAFE_DELETE( g_WndMng.m_pWndChangeClass1 );
					g_WndMng.m_pWndChangeClass1 = new CWndChangeClass1;
					g_WndMng.m_pWndChangeClass1->Initialize( &g_WndMng, APP_CHANGECLASS_1 );
				}
				else
				{
					SAFE_DELETE( g_WndMng.m_pWndChangeClass2 );
					g_WndMng.m_pWndChangeClass2 = new CWndChangeClass2;
					g_WndMng.m_pWndChangeClass2->Initialize( &g_WndMng, APP_CHANGECLASS_2 );
				}
			}
			else
			{
#ifdef __ONLINE
#ifdef __SM_ITEM_2ND
				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId, -1, FALSE );
#else	// __SM_ITEM_2ND
				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId );
#endif	// __SM_ITEM_2ND
#endif
			}
		}
		Destroy();
	}
	else if( nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_CHANGECLASS_1 - 직업변경(1차)
CtrlId : WIDC_RADIO_MER - 머서너리
CtrlId : WIDC_RADIO_ACR - 아크로뱃
CtrlId : WIDC_RADIO_MAG - 매지션
CtrlId : WIDC_RADIO_ASS - 어시스트
CtrlId : WIDC_STATIC1 - 변경을 원하는 직업을 선택하십시요
CtrlId : WIDC_STATIC2 - 직업 목록
CtrlId : WIDC_BUTTON_OK - Button
CtrlId : WIDC_BUTTON_CANCEL - 
****************************************************/

CWndChangeClass1::CWndChangeClass1() 
{ 
} 
CWndChangeClass1::~CWndChangeClass1() 
{ 
} 
void CWndChangeClass1::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChangeClass1::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	nJob = g_pPlayer->GetJob();
	SetJob();

	CWndButton* pWndButton;
	int nJobBuf = nJob - 1;
	if( nJobBuf == 0 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MER );
	else if( nJobBuf == 1 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ACR );
	else if( nJobBuf == 2 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ASS );
	else if( nJobBuf == 3 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MAG );

	pWndButton->EnableWindow( FALSE );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

void CWndChangeClass1::SetJob()
{
	CWndButton* pWndButton;

	BOOL bJob[MAX_EXPERT - MAX_JOBBASE];
	memset( bJob, 0, sizeof( bJob ) );
	bJob[nJob - 1] = TRUE;
 
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MER );
	pWndButton->SetCheck( bJob[0] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ACR );
	pWndButton->SetCheck( bJob[1] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ASS );
	pWndButton->SetCheck( bJob[2] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_MAG );
	pWndButton->SetCheck( bJob[3] );
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeClass1::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGECLASS_1, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndChangeClass1::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndChangeClass1::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeClass1::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeClass1::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeClass1::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeClass1::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK || message == EN_RETURN )
	{
		if( g_pPlayer->GetJob() == nJob )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_EQUALJOB), NULL, prj.GetTextColor(TID_GAME_EQUALJOB) );
		}
		else
		{
#ifdef __ONLINE
			g_DPlay.SendChangeJob( nJob, FALSE );
#endif
			Destroy();
		}
	}
	else if( nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	else if( nID == WIDC_RADIO_MER )
	{
		nJob = JOB_MERCENARY;
		SetJob();
	}
	else if( nID == WIDC_RADIO_ACR )
	{
		nJob = JOB_ACROBAT;
		SetJob();
	}
	else if( nID == WIDC_RADIO_ASS )
	{
		nJob = JOB_ASSIST;
		SetJob();
	}
	else if( nID == WIDC_RADIO_MAG )
	{
		nJob = JOB_MAGICIAN;
		SetJob();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
WndId : APP_CHANGECLASS_2 - 직업변경(2차)
CtrlId : WIDC_STATIC1 - 변경을 원하는 직업을 선택하십시요
CtrlId : WIDC_STATIC2 - 직업 목록
CtrlId : WIDC_RADIO_KNI - 나이트
CtrlId : WIDC_RADIO_BLA - 블레이드
CtrlId : WIDC_RADIO_RIN - 링마스터
CtrlId : WIDC_RADIO_PSY - 사이키퍼
CtrlId : WIDC_RADIO_ELE - 엘리멘터
CtrlId : WIDC_RADIO_JES - 제스터
CtrlId : WIDC_RADIO_RAN - 레인저
CtrlId : WIDC_RADIO_BIL - 빌포스터
CtrlId : WIDC_BUTTON_OK2 - Button
CtrlId : WIDC_BUTTON_CANCEL2 - Button
****************************************************/

CWndChangeClass2::CWndChangeClass2() 
{ 
} 
CWndChangeClass2::~CWndChangeClass2() 
{ 
} 
void CWndChangeClass2::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndChangeClass2::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	nJob = g_pPlayer->GetJob();
	SetJob();

	CWndButton* pWndButton;
	int nJobBuf = nJob - 6;
	if( ( nJobBuf ) == 0 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_KNI );
	else if( nJobBuf == 1 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BLA );
	else if( nJobBuf == 2 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_JES );
	else if( nJobBuf == 3 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RAN );
	else if( nJobBuf == 4 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RIN );
	else if( nJobBuf == 5 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BIL );
	else if( nJobBuf == 6 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_PSY );
	else if( nJobBuf == 7 )
		pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ELE );	
	
	pWndButton->EnableWindow( FALSE );
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
void CWndChangeClass2::SetJob()
{
	CWndButton* pWndButton;

	BOOL bJob[MAX_PROFESSIONAL - MAX_EXPERT];
	memset( bJob, 0, sizeof( bJob ) );
	bJob[nJob - 6] = TRUE;

	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_KNI );
	pWndButton->SetCheck( bJob[0] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BLA );
	pWndButton->SetCheck( bJob[1] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_JES );
	pWndButton->SetCheck( bJob[2] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RAN );
	pWndButton->SetCheck( bJob[3] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_RIN );
	pWndButton->SetCheck( bJob[4] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_BIL );
	pWndButton->SetCheck( bJob[5] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_PSY );
	pWndButton->SetCheck( bJob[6] );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO_ELE );
	pWndButton->SetCheck( bJob[7] );	
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeClass2::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGECLASS_2, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
직접 윈도를 열때 사용 
BOOL CWndChangeClass2::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
CRect rectWindow = m_pWndRoot->GetWindowRect(); 
CRect rect( 50 ,50, 300, 300 ); 
SetTitle( _T( "title" ) ); 
return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndChangeClass2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeClass2::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeClass2::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeClass2::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndChangeClass2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON_OK2 || message == EN_RETURN )
	{
		if( g_pPlayer->GetJob() == nJob )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_EQUALJOB), NULL, prj.GetTextColor(TID_GAME_EQUALJOB) );
		}
		else
		{
#ifdef __ONLINE
			g_DPlay.SendChangeJob( nJob, FALSE );
#endif
			Destroy();
		}
	}
	else if( nID == WIDC_BUTTON_CANCEL2 || nID == WTBID_CLOSE )
	{
		Destroy();
	}
	else if( nID == WIDC_RADIO_KNI )
	{
		nJob = JOB_KNIGHT;
		SetJob();
	}
	else if( nID == WIDC_RADIO_BLA )
	{
		nJob = JOB_BLADE;
		SetJob();
	}
	else if( nID == WIDC_RADIO_JES )
	{
		nJob = JOB_JESTER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_RAN )
	{
		nJob = JOB_RANGER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_RIN )
	{
		nJob = JOB_RINGMASTER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_BIL )
	{
		nJob = JOB_BILLPOSTER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_PSY )
	{
		nJob = JOB_PSYCHIKEEPER;
		SetJob();
	}
	else if( nID == WIDC_RADIO_ELE )
	{
		nJob = JOB_ELEMENTOR;
		SetJob();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
#endif // __S050524_SM_ITEM


void CWndInventory::RunUpgrade( CItemBase* pItem, int x, int y )
{
#ifdef __YENCHANT_0608
	if( m_bIsUpgradeMode )
	{
		m_bIsUpgradeMode = FALSE;

		if( pItem == NULL || m_pUpgradeMaterialItem == NULL )
		{
			g_WndMng.PutString( prj.GetText( TID_UPGRADE_CANCLE ) );
			BaseMouseCursor();
			return;
		}
		
		ItemProp* pItemProp = m_pUpgradeMaterialItem->GetProp();

		if( !pItemProp )
			return;

		if( pItemProp->dwItemKind3 == IK3_SOCKETCARD )
		{
#ifdef __S_ADD_PIERCING_ADD_CARD
			// 카드가 들어갈 아이템이( dwReferStat1 값이 PIERCING_1, PIERCING_2 ) 인지 검사
			if( !( pItem->GetProp()->dwReferStat1 == PIERCING_1 || pItem->GetProp()->dwReferStat1 == PIERCING_2 ) )
			{
				g_WndMng.PutString( prj.GetText(TID_PIERCING_POSSIBLE_ITEM), NULL, prj.GetTextColor(TID_PIERCING_POSSIBLE_ITEM) );
				BaseMouseCursor();
				return;
			}
#else // __S_ADD_PIERCING_ADD_CARD
			if( pItem->GetProp()->dwItemKind3 != IK3_SUIT )
			{
				g_WndMng.PutString( prj.GetText( TID_PIERCING_POSSIBLE_ITEM ) );
				BaseMouseCursor();
				return;
			}
#endif // __S_ADD_PIERCING_ADD_CARD
			
			PIERCINGINFO piercinginfo = ((CItemElem*)pItem)->m_piercingInfo;
			int nSize = piercinginfo.nPiercedSize;
			
			int nCount = 0;
			for( int j = 0; j < nSize; j++ )
			{
				if( piercinginfo.adwItemId[j] != 0 )
					nCount++;
			}

			// 빈곳이 없으면 중단
			if( nCount == nSize )
			{
				g_WndMng.PutString( prj.GetText(TID_PIERCING_ERROR_NOPIERCING) );
				BaseMouseCursor();
				return;
			}
		}
		else
		if( pItemProp->dwItemKind3 == IK3_ELECARD )
		{
			if( !CItemElem::IsEleRefineryAble( pItem->GetProp()) )
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;
			}

			CItemElem* pItemElem = (CItemElem*)pItem;

			if( pItemElem->m_bItemResist != SAI79::NO_PROP && pItemElem->m_bItemResist != pItemProp->eItemType )
			{
				g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_TWOELEMENT ) );
				BaseMouseCursor();
				return;
			}		
			
			DWORD dwWantedCard = 0;
			DWORD dwItemAO = pItemElem->m_nResistAbilityOption;
			switch( pItemProp->eItemType )
			{
			case SAI79::FIRE:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_TOUCH: II_GEN_MAT_ELE_FLAME; break;
			case SAI79::WATER:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_LAKE: II_GEN_MAT_ELE_RIVER; break;
			case SAI79::ELECTRICITY:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_VOLTAGE: II_GEN_MAT_ELE_GENERATOR; break;
			case SAI79::EARTH:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_STONE: II_GEN_MAT_ELE_DESERT; break;
			case SAI79::WIND:
				dwWantedCard = ( dwItemAO <= 4 ) ? II_GEN_MAT_ELE_GALE: II_GEN_MAT_ELE_CYCLON; break;
			default:
				dwWantedCard = 0; break;
			}

			if( pItemProp->dwID != dwWantedCard )
			{
				g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );
				BaseMouseCursor();
				return;					
			}
			
			
		}
		else
		if( pItemProp->dwItemKind3 == IK3_DICE )
		{
			if( !CItemElem::IsDiceRefineryAble( pItem->GetProp()) )
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;
			}

			CItemElem* pItemElem = (CItemElem*)pItem;

			// 주사위일 경우 조건검사
			if( pItemProp->dwID != II_GEN_MAT_DIE_TWELVE )
			{
				if( pItemElem->GetAbilityOption() <= 4 )
				{
					if( pItemProp->dwID != II_GEN_MAT_DIE_FOUR )
					{
						BaseMouseCursor();
						g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );			
						return;					
					}
				}
				else
				{
					if( pItemProp->dwID != II_GEN_MAT_DIE_SIX )
					{
						BaseMouseCursor();
						g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );			
						return;					
					}
				}
			}
		}
			
		// 인첸트가 되는 아이템 - 방어구 등등
		m_pUpgradeItem = pItem;
		m_dwEnchantWaitTime = timeGetTime() + SEC(4);
	}
#endif //__YENCHANT_0608	
}

void CWndInventory::BaseMouseCursor()
{
#ifdef __YENCHANT_0608
	m_bIsUpgradeMode = FALSE;

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
	
#endif //__YENCHANT_0608	
}

#ifdef __YINVENTORY_3D
void CWndInventory::UpdateParts()
{
#ifdef __FINITEITEM0705
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory );
#else	// __FINITEITEM0705
	CMover::UpdateParts( g_pPlayer->GetSex(), g_pPlayer->m_dwSkinSet, g_pPlayer->m_dwFace, g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh,g_pPlayer->m_adwEquipment, m_pModel, &g_pPlayer->m_Inventory );
#endif	// __FINITEITEM0705
}
#endif //__YINVENTORY_3D



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 스킬 재분배 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __YADV_RESKILLPOINT

CWndReSkillControl::CWndReSkillControl() 
{ 
} 
CWndReSkillControl::~CWndReSkillControl() 
{ 
} 
void CWndReSkillControl::OnDraw( C2DRender* p2DRender ) 
{ 
	
} 
void CWndReSkillControl::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( prj.GetText( TID_RESKILLPOINT_CONTROL1 )  );
	pWndText->m_string.AddParsingString( prj.GetText( TID_RESKILLPOINT_CONTROL2 ), 0xffff0000  );
	pWndText->ResetString();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndReSkillControl::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILL_CONTROL, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndReSkillControl::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndReSkillControl::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndReSkillControl::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndReSkillControl::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndReSkillControl::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL );
		
		if( pSkillTree )
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );
	}
	else if( nID == WIDC_BUTTON2 )
	{
		CWndReSkillWarning* pReSkill = new CWndReSkillWarning;
		pReSkill->Initialize(this);
	}
	else if( nID == 10000 )
	{
		return TRUE;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



CWndReSkillWarning::CWndReSkillWarning() 
{ 
} 
CWndReSkillWarning::~CWndReSkillWarning() 
{ 
	Destroy();	
} 
void CWndReSkillWarning::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndReSkillWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( _T( prj.GetText( TID_RESKILLPOINT_WARNING ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}
	
	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
BOOL CWndReSkillWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUEITMWARNING, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndReSkillWarning::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndReSkillWarning::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndReSkillWarning::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndReSkillWarning::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndReSkillWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL );
		
		if( pSkillTree )
		{
	#ifdef __ONLINE
			g_DPlay.SendDoUseSkillPoint( pSkillTree->GetSkill(), NULL, 0 );
	#endif //__ONLINE
			Destroy();
			pSkillTree->GetSkillControl()->Destroy();
		}
		
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


BOOL CWndSkillTreeEx::IsReSkillPointDone()
{
	if( m_apSkill == NULL )
		return FALSE;
	
	int nPoint = m_dwCurrSkillPoint;
	DWORD dwMinPoint = 0xffffffff;
	BOOL bMaster = FALSE;
	
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		ItemProp* pSkillProp = prj.GetSkillProp( dwSkill );
				
		if( dwSkill != NULL_ID )
		{
			if( pSkillProp->dwItemKind1 != JTYPE_BASE && CheckSkill(i) )
			{
				if( pSkill->dwLevel == pSkillProp->dwExpertMax )
					bMaster = TRUE;
				else
					bMaster = FALSE;

				int nPxpPoint = GetSkillPxp(pSkillProp->dwSubDefine, (pSkill->dwLevel != pSkillProp->dwExpertMax) ? pSkill->dwLevel+1 : pSkill->dwLevel ); 
				
				if( nPxpPoint && bMaster == FALSE )
				{
					if( dwMinPoint > nPxpPoint )
						dwMinPoint = nPxpPoint;
				}
			}		
		}
	}

	if( dwMinPoint != 0xffffffff )
	{
		if( m_dwCurrSkillPoint < dwMinPoint )
			return TRUE;
	}

	return FALSE;	
}
#endif //__YADV_RESKILLPOINT


#ifdef __Y_POST_0912

CWndPost::CWndPost() 
{ 
} 
CWndPost::~CWndPost() 
{ 
} 
void CWndPost::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPost::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	//	여기에 코딩하면 됩니다
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_PostTabSend.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_SEND );
	m_PostTabReceive.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_RECEIVE );

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = prj.GetText(TID_APP_POST_SEND);
	tabTabItem.pWndBase = &m_PostTabSend;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_APP_POST_RECEIVE);
	tabTabItem.pWndBase = &m_PostTabReceive;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	MoveParentCenter();	

	// 도착한 편지 목록 요청
	g_DPlay.SendQueryMailBox();	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPost::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPost::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPost::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPost::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPost::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPost::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_SELCHANGE )
	{
		CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

		if( pWndTabCtrl->GetCurSel() == 0 )
		{
		}
		else
		{
			g_DPlay.SendQueryMailBox();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////


CWndPostSend::CWndPostSend():m_nItem(0xff), m_nCost(0), m_nCount(0)
{ 

} 
CWndPostSend::~CWndPostSend() 
{ 
} 

void CWndPostSend::ClearData()
{
	CItemElem* pItemElem;
	pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
	
	if(pItemElem)
	{
		pItemElem->SetExtra( 0 );
	}
	
	m_nItem		= 0xff;		
	m_nCost		= 0;
	m_nCount	= 0;

	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
	
	pWndEdit2->SetString("");
	pWndEdit3->SetString("");
	pWndEdit4->SetString("");
}

void CWndPostSend::SetReceive( char* pchar )
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );	
	pWndCombo->SetString( pchar );
}
void CWndPostSend::SetTitle( char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::SetText( char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nItem != 0xff )
	{
		CItemElem* pItemElem;
		pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if( pItemElem )
		{
			LPWNDCTRL pCustom = NULL;
			pCustom = GetWndCtrl( WIDC_STATIC3 );
			
			pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

			if( pItemElem->GetProp()->dwPackMax > 1 && pItemElem->GetExtra() > 0 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
				TCHAR szTemp[32];
				_stprintf( szTemp, "%d", pItemElem->GetExtra() );
				p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff0000ff );
				p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xffb0b0f0 );

				p2DRender->SetFont( pOldFont );				
			}	
			
			//*
			CRect hitrect = pCustom->rect;
			CPoint point = GetMousePoint();
			if( hitrect.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( (CItemBase*)pItemElem, point2, &hitrect );
			}
			/**/
		}
	}
} 
void CWndPostSend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pNotice = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
	pNotice->AddWndStyle( EBS_WANTRETURN );

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndCombo->SetTabStop( TRUE );
	pWndEdit2->SetTabStop( TRUE );
	pWndEdit3->SetTabStop( TRUE );

	C2FriendPtr::iterator iter = g_WndMng.m_Messenger.m_aFriend.begin();
	for( ; iter != g_WndMng.m_Messenger.m_aFriend.end() ; ++iter )
	{
		LPFRIEND lpFriend = (LPFRIEND)iter->second;

		if( lpFriend )
		{
			pWndCombo->AddString( lpFriend->szName );
		}
	}

	pWndCombo->SetFocus();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostSend::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_POST_SEND, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPostSend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostSend::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostSend::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostSend::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndPostSend::SetItemId( BYTE nId )
{ 
	// 기존 아이템이 있었으면 확장 데이타를 초기화 한다.
	if( nId != m_nItem && m_nItem != 0xff )
	{
		CItemElem* pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		if( pItemElem )
		{
			pItemElem->SetExtra( 0 );				
		}
	}
	
	m_nItem = nId; 
}

BOOL CWndPostSend::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	
	// 아이템이 인벤토리에서 왔는가?
	if( !(pShortcut->m_dwShortcut == SHORTCUT_ITEM) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;

	if( pShortcut->m_dwData == 0 )
	{
		SetForbid( TRUE );		
		return FALSE;
	}
	
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT) );
		SetForbid( TRUE );
		return FALSE;
	}
	
	CItemElem* pItemElem;
	pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAt( pShortcut->m_dwIndex );
	
	if(pItemElem == NULL)
		return FALSE;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		if( pItemElem->m_nItemNum > 1 )
		{ 
			SetItemId( pItemElem->m_dwObjId );

			g_WndMng.m_pWndTradeGold = new CWndTradeGold;
			memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT) );
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemElem->m_nItemNum;
			g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_POST_SEND; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
			g_WndMng.m_pWndTradeGold->m_pWndBase = this;
			g_WndMng.m_pWndTradeGold->m_nSlot = 0;
			
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();
			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
		else
		{
			SetItemId( pItemElem->m_dwObjId );
			pItemElem->SetExtra( 1 );				

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}			
		}
	}		
			
	return TRUE;
}

void CWndPostSend::OnRButtonUp( UINT nFlags, CPoint point )
{
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		CItemElem* pItemElem;
		pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if(pItemElem)
		{
			pItemElem->SetExtra( 0 );				
		}

		m_nItem = 0xff;	
	}		
}

#define MAX_BYTE_POST		128

BOOL CWndPostSend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_SEND:	
		{
			CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
			CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
			
			LPCTSTR szstr1 = pWndCombo->GetString();
			LPCTSTR szstr2 = pWndEdit2->GetString();
			LPCTSTR szstr3 = pWndEdit3->GetString();
			LPCTSTR szstr4 = pWndEdit4->GetString();
			
			// 제목과 받는사람이 없으면 못보냄
			if( strlen( szstr1 ) <=0 || strlen( szstr2 ) <=0 )
			{
				return FALSE;
			}
			
			// 아이템과 페냐 두개중 하나가 없어도 못보냄
			if( m_nItem == NULL_ID && strlen( szstr4 ) <=0 )
			{
				return FALSE;
			}

			// 금액란에 숫자가 아니면 못보냄
			int nlen = strlen(szstr4);
			BOOL bisdigit = TRUE;

			if( nlen > 0 )
			{
				for( int i = 0 ; i < nlen ; i++ )
				{
					if( (isdigit( szstr4[i] ) == FALSE) )
					{
						bisdigit = FALSE;
						break;
					}
				}
			}
			
			if( bisdigit == FALSE )
			{
				return FALSE;
			}

			CWndPostSendConfirm* pWndPostSendConfirm = new CWndPostSendConfirm;
			
			if(pWndPostSendConfirm)
			{
				pWndPostSendConfirm->Initialize( this );
			
				CItemElem* pItemElem;
				pItemElem  = (CItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );

				if( pItemElem )
				{
					pWndPostSendConfirm->SetValue( m_nItem, pItemElem->GetExtra(), (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
				else
				{
					pWndPostSendConfirm->SetValue( -1, 0, (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
			
			}

			break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostRead::CWndPostRead():m_nMailIndex(-1)
{ 

} 
CWndPostRead::~CWndPostRead() 
{ 
	ClearData();
} 

void CWndPostRead::ClearData()
{
	m_nMailIndex = -1;
}

void CWndPostRead::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CItemElem* m_pItemElem = mailbox[m_nMailIndex]->m_pItemElem;

	if( m_pItemElem )
	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC3 );

		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
			g_WndMng.PutToolTip_Item( (CItemBase*)m_pItemElem, point2, &hitrect );
		}

		m_pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

		if( m_pItemElem->GetProp()->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
			
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_pItemElem->m_nItemNum  );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff0000ff );
			p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xffb0b0f0 );

			p2DRender->SetFont( pOldFont );				
		}	
	}

	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC4 );
		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) && mailbox[m_nMailIndex]->m_nGold > 0 )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
		}
	}
} 
void CWndPostRead::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	

	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);
	pWndText->EnableWindow(FALSE);

	pWndButton->SetFocus();
	MoveParentCenter();
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC4 );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_STATIC4 );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );
	
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostRead::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_READ );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_READ ); 
} 

BOOL CWndPostRead::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostRead::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostRead::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostRead::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostRead::SetValue( int nMailIndex )
{
	m_nMailIndex	= nMailIndex;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail == NULL )
		return;

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	

	LPCSTR lpszPlayer	= prj.GetPlayerString( pMail->m_idSender );
	
	if( lpszPlayer )
		pWndEdit1->SetString(lpszPlayer);
	else
		pWndEdit1->SetString("Unknow");
	
	pWndEdit2->SetString(pMail->m_szTitle);
	pWndText->SetString(pMail->m_szText);

#ifdef __YDECIMAL1223			
	char szbuffer[128] = {0};
	sprintf( szbuffer, "%d", pMail->m_nGold );
	pWndEdit3->SetString(GetNumberFormatEx(szbuffer));
#endif //__YDECIMAL1223
}
BOOL CWndPostRead::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndPostRead::OnMouseMove(UINT nFlags, CPoint point )
{
	if( m_bLButtonDown && IsPush() )
	{
		if( m_nMailIndex == -1 )
			return;
		
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nMailIndex];
		
		if( pMail && pMail->m_pItemElem && pMail->m_pItemElem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId       = pMail->m_pItemElem->m_dwObjId;
			m_GlobalShortcut.m_pTexture   = pMail->m_pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) pMail->m_pItemElem;
			_tcscpy( m_GlobalShortcut.m_szString, pMail->m_pItemElem->GetProp()->szName);
		}
		else
		{
			
		}
	}	
}

void CWndPostRead::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_nMailIndex == -1 )
		return;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveItem();
	}
	
	pCustom = GetWndCtrl( WIDC_STATIC4 );

	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveGold();
	}
}
void CWndPostRead::MailReceiveItem()
{
	// 해당 아이템을 메일에서 갖고온다.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail && pMail->m_pItemElem )
	{
		g_DPlay.SendQueryGetMailItem( pMail->m_nMail );			
	}
}
void CWndPostRead::MailReceiveGold()
{
	// 해당 페냐를 메일에서 갖고온다.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( pMail && pMail->m_nMail > 0 )
	{
		g_DPlay.SendQueryGetMailGold( pMail->m_nMail );		
		
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		pWndEdit->SetString("0");
	}	
}
BOOL CWndPostRead::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

	if( pWndPost == NULL )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON1:	
		{
			CWndPostDeleteConfirm* pDeleteConfirm = new CWndPostDeleteConfirm;

			if( pDeleteConfirm )
			{
				pDeleteConfirm->Initialize(this);
				pDeleteConfirm->SetValue(m_nMailIndex);			
			}
		}
		break;
	case WIDC_BUTTON2:	
		{
			CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			
			if( pWndEdit1 && pWndEdit2 )
			{
				if( stricmp( pWndEdit1->GetString(), "Unknow" ) == 0 )
				{
					g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_MAIL_UNKNOW) );
					return FALSE;
				}

				pWndPost->m_PostTabSend.SetItemId(0xff);
				pWndPost->m_PostTabSend.SetReceive("");
				pWndPost->m_PostTabSend.SetText("");
				pWndPost->m_PostTabSend.SetTitle("");
				
				CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)pWndPost->GetDlgItem( WIDC_TABCTRL1 );
				pWndTabCtrl->SetCurSel(0);	
				pWndPost->m_PostTabSend.SetReceive( (char*)pWndEdit1->GetString() );

				CString str = "Re:";
				str += pWndEdit2->GetString();
				pWndPost->m_PostTabSend.SetTitle(str.GetBuffer(0));
				Destroy();
			}
		}
		break;
	case WIDC_BUTTON3:	
		{
			Destroy();
		}
		break;
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostSendConfirm::CWndPostSendConfirm() 
{
}

CWndPostSendConfirm::~CWndPostSendConfirm() 
{
	
}

void CWndPostSendConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndPostSendConfirm::SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText )
{
	m_nItem		= nItem;
	m_nItemNum	= nItemNum;
	strcpy( m_szReceiver, lpszReceiver );
	strcpy( m_szTitle, lpszTitle );
	strcpy( m_szText, lpszText );
	m_nGold		= nGold;
}


void CWndPostSendConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_MAIL_SEND_CONFIRM) ), 0xff000000 );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndPostSendConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_CLOSE_EXISTING_CONNECTION );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_CLOSE_EXISTING_CONNECTION ); 
} 

BOOL CWndPostSendConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndPostSendConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndPostSendConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndPostSendConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndPostSendConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
		{
			g_DPlay.SendQueryPostMail( m_nItem, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText );			

			CWndPost * pWndPost = (CWndPost *)g_WndMng.GetWndBase( APP_POST );

			if( pWndPost )
			{
				pWndPost->m_PostTabSend.ClearData();
			}

			g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_MAIL_SEND_OK) );
		}
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPostDeleteConfirm::CWndPostDeleteConfirm():m_nIndex(0)
{
}

CWndPostDeleteConfirm::~CWndPostDeleteConfirm() 
{
	
}

void CWndPostDeleteConfirm::OnDraw( C2DRender* p2DRender ) 
{
}
void CWndPostDeleteConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_MAIL_DELETE_CONFIRM) ), 0xff000000 );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndPostDeleteConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_CLOSE_EXISTING_CONNECTION );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_CLOSE_EXISTING_CONNECTION ); 
} 

BOOL CWndPostDeleteConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndPostDeleteConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndPostDeleteConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndPostDeleteConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndPostDeleteConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nIndex];
		
		if( pMail )
		{
			g_DPlay.SendQueryRemoveMail( pMail->m_nMail );						
			Destroy(TRUE);

			CWndPostRead* pWndPost = (CWndPostRead*)g_WndMng.GetWndBase(APP_POST_READ);
			if(pWndPost)
				pWndPost->Destroy();
			
		}
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPostReceive::CWndPostReceive():m_wndPostRead(NULL)
{ 
	m_nMax = 0;
	m_nSelect = -1;
} 
CWndPostReceive::~CWndPostReceive() 
{ 
} 
HRESULT CWndPostReceive::DeleteDeviceObjects()
{
	m_Texture[0].DeleteDeviceObjects();
	m_Texture[1].DeleteDeviceObjects();
	m_Texture[2].DeleteDeviceObjects();

	return CWndNeuz::DeleteDeviceObjects();
}
int CWndPostReceive::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y-23) / 40;
	
	if( 0 <= nIndex && nIndex < MAX_MAIL_LIST_PER_PAGE ) 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
void CWndPostReceive::UpdateScroll()
{
	CMailBox* pMailBox	= CMailBox::GetInstance();
	m_nMax = pMailBox->size();
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}	
}
void CWndPostReceive::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	
	sx = 15;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	TCHAR szTemp[32];

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture[0], 150  );
		
	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( mailbox.size() <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

		if( mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold == 0 )
		{
			p2DRender->RenderTexture( CPoint( sx+10, sy+1 ), &m_Texture[1] );
		}

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 32, 190, 0 , 0 ) );
		}
		
		// 아이템이 있으면 출력
		if( mailbox[i]->m_pItemElem && mailbox[i]->m_pItemElem->m_pTexture )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), mailbox[i]->m_pItemElem->m_pTexture );

			if( mailbox[i]->m_pItemElem->GetProp()->dwPackMax > 1 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
				_stprintf( szTemp, "%d", mailbox[i]->m_pItemElem->m_nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xff0000ff );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xffb0b0f0 );
				
				p2DRender->SetFont( pOldFont );			
			}
		}
		else
		// 아이템은 없고 돈만 있으면 돈모양 출력
		if( mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold > 0 )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), &m_Texture[2] );
		}

		LPCSTR lpszPlayer	= prj.GetPlayerString( mailbox[i]->m_idSender );
		
		if( lpszPlayer )
		{
			p2DRender->TextOut( sx + 75,  sy+3, lpszPlayer ,  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 75,  sy+3, "Unknow",  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}

		p2DRender->TextOut( sx + 75,  sy+18, mailbox[i]->m_szTitle,  D3DCOLOR_XRGB( 0, 0, 190 ) );
		
		sy += 40;
	}	
} 

void CWndPostReceive::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;	

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( mailbox[m_nSelect] == NULL )
		return;
	
	m_wndPostRead = new CWndPostRead;

	if( m_wndPostRead )
	{
		m_wndPostRead->Initialize();

		m_wndPostRead->SetValue( m_nSelect );
	}
}

void CWndPostReceive::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 15;
	rect.bottom -= 50;
	rect.left    = rect.right - 40;
	rect.right   -= 30;
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}
	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
	
	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\Default\\", "WndPostTable.tga" ), 0xffff00ff, FALSE );	
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "Theme\\Default\\", "WndNotUse.tga" ), 0xffff00ff, FALSE );	
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "item\\", "itm_GolGolSeed.dds" ), 0xffff00ff, FALSE );	
	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostReceive::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_POST_RECEIVE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndPostReceive::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostReceive::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostReceive::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostReceive::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;	
} 
BOOL CWndPostReceive::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndPostReceive::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

#endif//__Y_POST_0912


#ifdef __GUILDCOMBAT
// 길드대전 신청
CWndGuildWarAppConfirm::CWndGuildWarAppConfirm() 
{
	
}

CWndGuildWarAppConfirm::~CWndGuildWarAppConfirm() 
{
	
}

void CWndGuildWarAppConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildWarAppConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_APP) ), 0xff000000 );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndGuildWarAppConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarAppConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarAppConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarAppConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarAppConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarAppConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
			g_DPlay.SendGuildCombatApp( g_pPlayer->GetId() );

		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드대전 취소
CWndGuildWarCancelConfirm::CWndGuildWarCancelConfirm() 
{
	
}

CWndGuildWarCancelConfirm::~CWndGuildWarCancelConfirm() 
{
	
}

void CWndGuildWarCancelConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildWarCancelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_CANCEL) ), 0xff000000 );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndGuildWarCancelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarCancelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarCancelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarCancelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarCancelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarCancelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
			g_DPlay.SendGuildCombatCancel( g_pPlayer->GetId() );
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드대전 입장
CWndGuildWarJoinConfirm::CWndGuildWarJoinConfirm() 
{
	
}

CWndGuildWarJoinConfirm::~CWndGuildWarJoinConfirm() 
{
	
}

void CWndGuildWarJoinConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildWarJoinConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;
	
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_JOIN) ), prj.GetTextColor(TID_GAME_GUILDWAR_JOIN) );
	m_wndText.ResetString();
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 

BOOL CWndGuildWarJoinConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarJoinConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarJoinConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarJoinConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarJoinConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarJoinConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
			g_DPlay.SendGuildCombatJoin( g_pPlayer->GetId() );
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
	

//길드대전 참가 길드 명단 출력

CWndGuildWarState::CWndGuildWarState() 
{ 
	Init( 0 );
} 

CWndGuildWarState::~CWndGuildWarState() 
{ 
} 

void CWndGuildWarState::Init( time_t lTime )
{
	memset( m_aList, 0, sizeof(m_aList) );
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildWarState::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndGuildWarState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndGuildWarState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_WAR_STATE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildWarState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildWarState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildWarState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndGuildWarState::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndGuildWarState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildWarState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildWarState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildWarState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildWarState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildWarState::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildWarState::OnMouseWndSurface( CPoint point )
{
}	
void CWndGuildWarState::InsertGuild( const char szGuild[], const char szName[], int nNum )
{
	if( m_nMax >= MAX_GUILDCOMBAT_LIST )
	{
		Error( "CWndGuildWarState::InsertGuild - range over" );
		return;
	}
	
	if( nNum == 0 )
		return;

	GetStrCut( szGuild, m_aList[m_nMax].szGuild, 8 );

	if( GetStrLen( szGuild ) >= 8 )
	{
		_tcscat( m_aList[m_nMax].szGuild, "..." );
	}
	else
	{
		_tcscpy( m_aList[m_nMax].szGuild, szGuild );
	}

	_tcscpy( m_aList[m_nMax].szName, szName );
	m_aList[m_nMax].nNum = nNum;

	m_nMax++;
}

void CWndGuildWarState::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+310, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		sprintf( szNum, "%3d", i+1 );
		sprintf( szCount, "%2d", m_aList[i].nNum );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+295, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		p2DRender->TextOut( sx + 4,    sy, szNum,             dwColor );
		p2DRender->TextOut( sx + 30,   sy, m_aList[i].szGuild, dwColor );
		p2DRender->TextOut( sx + 160,  sy, m_aList[i].szName,  dwColor );
		p2DRender->TextOut( sx + 260,  sy, szCount,    dwColor );

		sy += 18;
	}
} 
BOOL CWndGuildWarState::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}




//길드컴뱃 랭킹

CWndGuildCombatRanking::CWndGuildCombatRanking() 
{ 
	Init( 0 );
} 

CWndGuildCombatRanking::~CWndGuildCombatRanking() 
{ 
} 

void CWndGuildCombatRanking::Init( time_t lTime )
{
	m_multimapRanking.clear();
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildCombatRanking::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 53;
	rect.bottom -= 10;
	rect.left    = rect.right - 30;
	rect.right   -= 10;

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}

	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

BOOL CWndGuildCombatRanking::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANKING, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatRanking::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatRanking::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatRanking::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndGuildCombatRanking::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}

void CWndGuildCombatRanking::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildCombatRanking::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRanking::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatRanking::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatRanking::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildCombatRanking::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatRanking::OnMouseWndSurface( CPoint point )
{
}	
void CWndGuildCombatRanking::InsertGuild( const char szGuild[], int nWinCount )
{
	if( m_nMax >= MAX_GUILDCOMBAT_LIST )
	{
		Error( "CWndGuildWarState::InsertGuild - range over" );
		return;
	}

	GUILDNAME p;
	memset( &p, 0, sizeof(GUILDNAME) );
	GetStrCut( szGuild, p.szGuild, 8 );
	
	if( GetStrLen( szGuild ) >= 8 )
	{
		_tcscat( p.szGuild, "..." );
	}
	else
	{
		_tcscpy( p.szGuild, szGuild );
	}
	
	m_multimapRanking.insert( make_pair(nWinCount, p) );

	m_nMax++;
}

void CWndGuildCombatRanking::OnDraw( C2DRender* p2DRender ) 
{ 
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;	

	CRect rc( sx, 5, sx+230, 7 ); 	
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 128, 0 , 0 , 0 ) );

	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}

	int nRanking = 0;
	int nOldRanking = -1;
	multimap< int, GUILDNAME >::reverse_iterator iterRobotBegin = m_multimapRanking.rbegin();
	
	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		multimap< int, GUILDNAME >::value_type& refValue = *iterRobotBegin;
		
		if( nOldRanking != refValue.first )
			nRanking++;

		sprintf( szNum, "%3d", nRanking );
		sprintf( szCount, "%2d", refValue.first );

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+215, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 64, 0 , 0 , 0 ) );
		}

		if( i == 0 )
			dwColor = D3DCOLOR_XRGB( 200 , 0 , 0 );
		else
			dwColor = D3DCOLOR_XRGB( 0 , 0 , 0 );		

		if( nOldRanking != refValue.first )
		{
			p2DRender->TextOut( sx + 4,    sy, szNum,  dwColor );
		}
		else
		{
			p2DRender->TextOut( sx + 5,    sy, " 〃",  dwColor );
		}

		if( i == 0 )
		{
			p2DRender->TextOut( sx + 40,  sy, refValue.second.szGuild,  D3DCOLOR_XRGB( 200 , 0 , 0 ) );
			p2DRender->TextOut( sx + 180,  sy, szCount,    D3DCOLOR_XRGB( 200 , 0 , 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 40,  sy, refValue.second.szGuild,  dwColor );
			p2DRender->TextOut( sx + 180,  sy, szCount,    dwColor );
		}
		

		sy += 18;

		iterRobotBegin++;
		nOldRanking = refValue.first;			
	}
} 
BOOL CWndGuildCombatRanking::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}

void CWndGuildCombatRanking::SortRanking()
{

}

#endif //__GUILDCOMBAT



