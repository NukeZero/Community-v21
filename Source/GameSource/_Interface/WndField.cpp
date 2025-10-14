// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "WndField2.h"
#include "Party.h"
#include "WndManager.h"
#include "defineNeuz.h"

#include "MsgHdr.h"
#include "wndvendor.h"
#include "wndwebbox.h"

#include "WndBagEx.h"	//sun: 11, 주머니
#include "playerdata.h"	//sun: 11, 캐릭터 정보 통합

#include "GuildHouse.h"

extern int g_nSkillCurSelect;
extern float g_fHairLight;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;

#include "../Neuz/dpcertified.h"
extern	CDPCertified	g_dpCertified;

#include "WndHelperWebBox.h"
#include "WndMapEx.h"
#include "resdata.h"

#include "../_Common/FLServerDescription.h"

#define WND_WIDTH 210
#define TASKBAR_HEIGHT 44

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;


#define MAX_GUILDCOMBAT_LIST_PER_PAGE 11
#define MAX_MAIL_LIST_PER_PAGE 6
#define MAX_GUILDCOMBAT_LIST		  100

#define ENCHANTWAITTIME		500

//////////////////////////////////////////////////////////////////////////
BOOL	IsNeedTarget( PT_ITEM_SPEC pItemProp )
{	// 아이템을 사용하기 위해 더블 클릭 했을 때
	// 커서가 바뀌면서 인벤토리 내 다른 아이템 클릭이 필요한 아이템인가?
	//sun: 12, 펫 각성
	// 아이템 식별자 추가가 번거로와 속성 확인으로 변경
	return( pItemProp->dwExeTarget == EXT_ITEM );
}
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 애플렛 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
			FLIntToString( m_pItemElem->m_nItemNum, szNumberbuf, _countof( szNumberbuf ), 10 );
			m_pEdit->SetString( szNumberbuf );
		}
		for( int i = 0 ; i < 8 ; i++ )
		{
			char szNumberbuf[8] = {0, };
			FLStrncpy( szNumberbuf, _countof( szNumberbuf ), szNumber, 8 );
			
			// 0 : 공백, 48 : 숫자 0, 57 : 숫자 9
			if( 47 >= szNumberbuf[i] || szNumberbuf[i] >= 58 )
			{
				if( szNumberbuf[i] != 0 )
				{
					nNumber = m_pItemElem->m_nItemNum;
					FLIntToString( m_pItemElem->m_nItemNum, szNumberbuf, _countof( szNumberbuf ), 10 );
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
		FLIntToString( m_pItemElem->m_nItemNum, szNumber, _countof( szNumber ), 10 );
		m_pEdit->SetString( szNumber );
	}	

	// 윈도를 중앙으로 옮기는 부분.
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

BOOL CWndDropItem::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}

BOOL CWndDropItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		int DropNum = 0;

		if( m_pItemElem->m_nItemNum >= 1 )
		{
			DropNum = atoi( m_pEdit->GetString() );
		}
		if( DropNum != 0 )
		{
			g_DPlay.SendDropItem( 0, m_pItemElem->m_dwObjId, DropNum, m_vPos );
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

BOOL CWndDropConfirm::Process( void )
{
	if(m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}

BOOL CWndDropConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES || message == EN_RETURN )
	{
		g_DPlay.SendDropItem( 0, m_pItemElem->m_dwObjId, 1, m_vPos );
		Destroy();
	}
	else if( nID == WIDC_NO )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_RANDOMSCROLL_CONFIRM - 랜덤 옵션
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_YES - Button
  CtrlId : WIDC_NO - Button
****************************************************/

CWndRandomScrollConfirm::CWndRandomScrollConfirm() 
{ 
	bFlag = FALSE;
	objid = objid1 = NULL_ID;
} 
CWndRandomScrollConfirm::~CWndRandomScrollConfirm() 
{ 
} 
void CWndRandomScrollConfirm::SetItem( OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf )
{
	objid = objidBuf;
	objid1 = objid1Buf;
	bFlag = bFlagBuf;
	
}
void CWndRandomScrollConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRandomScrollConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	FLItemElem* pItemElem0;
	FLItemElem* pItemElem1;

	pItemElem0	= g_pPlayer->m_Inventory.GetAtId( objid );
	pItemElem1	= g_pPlayer->m_Inventory.GetAtId( objid1 );

	if( pItemElem0 && pItemElem1 )
	{
		CString strMessage;
		if( bFlag )
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_CONFIRM1 ), pItemElem0->GetProp()->szName );
		else
			strMessage.Format( prj.GetText( TID_GAME_RANDOMSCROLL_CONFIRM ), pItemElem0->GetProp()->szName );

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		pWndText->SetString( strMessage );
		pWndText->EnableWindow( FALSE );
	}
	else
	{
		Destroy();
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RANDOMSCROLL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndRandomScrollConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndRandomScrollConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRandomScrollConfirm::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRandomScrollConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRandomScrollConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRandomScrollConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		g_DPlay.SendRandomScroll( objid, objid1 );
	}
	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 인벤토리 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "WndShop.h"
//////////////////////////////////////////////
CWndQueryEquip::CWndQueryEquip()
{
	m_ObjID = NULL_ID;
	memset( m_InvenRect, 0, sizeof(m_InvenRect) );

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
		if( pMover->m_aEquipInfo[i].dwItemID == 0 )
			continue;
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// 툴팁
		if( DrawRect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			
			FLItemElem itemElem;

			itemElem = m_aEquipInfoAdd[i];

// 			itemElem.m_dwItemId	= pMover->m_aEquipInfo[i].dwItemID;
// 			itemElem.m_byFlag	= pMover->m_aEquipInfo[i].byFlag;
// 			itemElem.SetAbilityOption( pMover->m_aEquipInfo[i].nOption & 0xFF );
//			itemElem.m_nResistAbilityOption = m_aEquipInfoAdd[i].nResistAbilityOption;
//			itemElem.m_byItemResist	= m_aEquipInfoAdd[i].byItemResist;
//			itemElem.SetRandomOptItemId( m_aEquipInfoAdd[i].iRandomOptItemId );
//			itemElem.CopyPiercingOption( m_aEquipInfoAdd[i].kPiercingOption );
//			itemElem.CopyRandomOption( m_aEquipInfoAdd[i].kRandomOption );
//			itemElem.SetRandomOptionOriginID( m_aEquipInfoAdd[i].wRandomOptionOriginId );
//			itemElem.SetLevelDown( m_aEquipInfoAdd[i].nLevelDown );
			
			// 장비창에 있는것 툴팁
			g_WndMng.PutToolTip_Item( (FLItemBase*)&itemElem, point2, &DrawRect, APP_QUERYEQUIP );
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

		if( pMover->m_aEquipInfo[i].dwItemID == 0 )
			continue;

		PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( pMover->m_aEquipInfo[i].dwItemID );
		if( !pItemProp )
			continue;

//		if( m_aEquipInfoAdd[i].pTexture == NULL )
		if( m_aEquipInfoAdd[i].m_pTexture == NULL )
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

		if( pMover->m_aEquipInfo[i].byFlag & FLItemElem::expired )
		{
//			p2DRender->RenderTexture2( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, sx, sy, D3DCOLOR_XRGB( 255, 100, 100 ) );
			p2DRender->RenderTexture2( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].m_pTexture, sx, sy, D3DCOLOR_XRGB( 255, 100, 100 ) );
		}
		else
		{
//			p2DRender->RenderTexture( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].pTexture, dwAlpha, sx, sy );
			p2DRender->RenderTexture( DrawRect.TopLeft()+cpAdd, m_aEquipInfoAdd[i].m_pTexture, dwAlpha, sx, sy );
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
/*
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/		
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
		int nVisibleParts = PARTS_HAT;
		int nInVisibleParts = PARTS_CAP;
		if(!pMover->IsOptionRenderCostume(0) )
		{
			nVisibleParts = PARTS_CAP;
			nInVisibleParts = 0;
		}	
	// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 없는경우
			DWORD dwId	= pMover->m_aEquipInfo[nInVisibleParts].dwItemID;
			O3D_ELEMENT*	pElement = NULL;
			PT_ITEM_SPEC		pItemProp = NULL;
			pElement = m_pModel->GetParts(PARTS_HAIR);
			if( pElement )
				pElement->m_nEffect &= ~XE_HIDE;
			if( dwId != 0 )
			{
				pItemProp	= g_xSpecManager->GetSpecItem( dwId );
				
				if( pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID )
				{
					pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
				}
				
				// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
				DWORD dwParts = nVisibleParts;
				dwId	= pMover->m_aEquipInfo[dwParts].dwItemID;

				//gmpbigsun( 20111212 ) : 기획요청 CAP룰
				if( 0 == dwId )
				{
					dwParts = nInVisibleParts;
					dwId = pMover->m_aEquipInfo[ dwParts ].dwItemID;
				}

				if( dwId != 0 )
				{
					if( !(pMover->m_aEquipInfo[dwParts].byFlag & FLItemElem::expired) )
					{
						pItemProp	= g_xSpecManager->GetSpecItem( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID )
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
									
			}
			else
			{
				// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
				dwId	= pMover->m_aEquipInfo[nVisibleParts].dwItemID;
				if( dwId != 0 )
				{
					if( !(pMover->m_aEquipInfo[nVisibleParts].byFlag & FLItemElem::expired) )
					{						
						pItemProp	= g_xSpecManager->GetSpecItem( dwId );
						if( pItemProp && pItemProp->dwBasePartsIgnore != NULL_ID )
						{
							if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
								((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

							((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
						}
					}
				}							
			}
			
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[0] = pMover->m_fHairColorR;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[1] = pMover->m_fHairColorG;
		m_pModel->GetObject3D(PARTS_HAIR)->m_fAmbient[2] = pMover->m_fHairColorB;
			
		m_pModel->SetGroup( 0 );	
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
		int nMover = (GetMover()->GetSex() == SEX_MALE ? OBJECT_INDEX( 11, MI_MALE ) : OBJECT_INDEX( 12, MI_FEMALE ));
		m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
		CMover::UpdateParts( GetMover()->GetSex(), GetMover()->m_dwSkinSet, GetMover()->m_dwFace, GetMover()->m_dwHairMesh, GetMover()->m_dwHeadMesh,GetMover()->m_aEquipInfo, m_pModel, NULL );
		m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	}
}

//void CWndQueryEquip::SetEquipInfoAdd( EQUIP_INFO_ADD* aEquipInfoAdd )
void CWndQueryEquip::SetEquipInfoAdd( FLItemElem* aEquipInfoAdd )
{
//	memcpy( m_aEquipInfoAdd, aEquipInfoAdd, sizeof(EQUIP_INFO_ADD) * MAX_HUMAN_PARTS );
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
		m_aEquipInfoAdd[i]	= aEquipInfoAdd[i];

	CMover* pMover = GetMover();
	
	if( IsInvalidObj(pMover) )
		return ;
	
#ifdef __CLIENT
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( m_aEquipInfoAdd[ i ].m_dwItemId != 0 )
		{
			PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( m_aEquipInfoAdd[ i ].m_dwItemId );
			if( pItemProp )
			{
				m_aEquipInfoAdd[i].SetTexture( CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff ) );
			}
		}

// 		if( pMover->m_aEquipInfo[i].dwItemID != 0 )
// 		{
// 			PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( pMover->m_aEquipInfo[i].dwItemID );
// 			if( pItemProp )
// //				m_aEquipInfoAdd[i].pTexture	= CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
// 				m_aEquipInfoAdd[i].SetTexture( CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff ) );
// 		}
	}
#endif	// __CLIENT
}

BOOL CWndQueryEquip::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();

	CRect rect( 792, 130, 792 + 232, 130 + 405 + 20 ); // 1024 768

	// 인벤토리 출력 위치 지정
	memset( m_InvenRect, 0, sizeof(m_InvenRect) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}






//---------------------------------------------------------------------------------------------
// 윈도우 인벤토리
//---------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndInventory::CWndInventory()
:	m_pModel( NULL )
,	m_pTabCtrl( NULL )
,	m_pSelectItem( NULL )
,	m_TexRemoveItem( NULL )
,	m_pUnSealKeyItem( NULL )
,	m_pUnSealItem( NULL )
,	m_fRot( 0.0f ) 
,	m_OldPos( 0, 0 )
,	m_bReport( FALSE )
,	m_bLButtonDownRot( FALSE )
,	m_pSfxUpgrade( NULL )
,	m_pUpgradeItem( NULL )
,	m_pUpgradeMaterialItem( NULL )
,	m_pWndConfirmBuy( NULL )
,	m_bIsUpgradeMode( FALSE )
,	m_dwEnchantWaitTime( 0xFFFFFFFF )
,	m_bRemoveJewel( FALSE )
,	m_pWndRemoveJewelConfirm( NULL )
{
	memset( m_InvenRect, NULL, sizeof(CRect) * MAX_HUMAN_PARTS );
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndInventory::~CWndInventory()
{
	SAFE_DELETE( m_pModel );	
	SAFE_DELETE( m_pWndConfirmBuy );

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}

	SAFE_DELETE( m_pWndRemoveJewelConfirm );	//sun: 9차 전승관련 Client
}


//---------------------------------------------------------------------------------------------
// 봉인 해제를 실행한다.
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventory::DoUnSeal()
{
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pUnSealKeyItem,	_T("[윈도우 인벤토리] 봉인해제 키 아이템 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pUnSealItem,		_T("[윈도우 인벤토리] 봉인해제 아이템 포인터가 유효하지 않습니다.") );

	DWORD dwUnSealKeyItemIndex( m_pUnSealKeyItem->m_dwObjId );
	DWORD dwUnSealItemIndex( m_pUnSealItem->m_dwObjId );

	g_DPlay.SendUnSealItem(  dwUnSealKeyItemIndex, dwUnSealItemIndex );	

	CancelUnSeal();
}


//---------------------------------------------------------------------------------------------
// 봉인 해제를 취소한다.
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndInventory::CancelUnSeal()
{
	m_pUnSealKeyItem = NULL;
	m_pUnSealItem = NULL;
}


void CWndInventory::OnDestroy( void )
{
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pWndConfirmBuy );	

	CWndSummonAngel* pWndAngel = (CWndSummonAngel*)GetWndBase( APP_SUMMON_ANGEL );		//sun: 8차 엔젤 소환 Neuz, World, Trans
	if(pWndAngel != NULL)
		pWndAngel->Destroy();

//sun: 10, __EVE_MINIGAME
	CWndFindWordGame* pWndWordGame = (CWndFindWordGame*)GetWndBase( APP_MINIGAME_WORD );
	if(pWndWordGame != NULL)
		pWndWordGame->Destroy();
	
	CWndPuzzleGame* pWndPuzzleGame = (CWndPuzzleGame*)GetWndBase( APP_MINIGAME_PUZZLE );
	if(pWndPuzzleGame != NULL)
		pWndPuzzleGame->Destroy();

//sun: 9차 전승관련 Clienet
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
	if(pWndMixJewel != NULL)
		pWndMixJewel->Destroy();

#ifdef COSTUME_UPGRADE_MIX
	CWndItemMix* pWndItemMix = (CWndItemMix*)GetWndBase( APP_SMELT_MIXCOSTUME );
	if(pWndItemMix != NULL)
		pWndItemMix->Destroy();	
#endif

	CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase( APP_SMELT_EXTRACTION );
	if(pWndExtraction != NULL)
		pWndExtraction->Destroy();

	CWndPiercing* pWndPiercing = (CWndPiercing*)GetWndBase( APP_PIERCING );
	if(pWndPiercing != NULL)
		pWndPiercing->Destroy();
	CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase( APP_REMOVE_ATTRIBUTE );
	if(pWndRemoveAttribute != NULL)
		pWndRemoveAttribute->Destroy();
	CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
	if(pWndRemovePiercing != NULL)
		pWndRemovePiercing->Destroy();
	CWndRemoveJewel* pWndRemoveJewel = (CWndRemoveJewel*)GetWndBase( APP_SMELT_REMOVE_JEWEL );
	if(pWndRemoveJewel != NULL)
		pWndRemoveJewel->Destroy();
	CWndLvReqDown* pWndLvReqDown = (CWndLvReqDown*)GetWndBase( APP_LVREQDOWN );
	if(pWndLvReqDown != NULL)
		pWndLvReqDown->Destroy();
	CWndBlessingCancel* pWndBlessingCancel = (CWndBlessingCancel*)GetWndBase( APP_CANCEL_BLESSING );
	if(pWndBlessingCancel != NULL)
		pWndBlessingCancel->Destroy();
	CWndUpgradeBase* pWndUpgradeBase = (CWndUpgradeBase*)GetWndBase( APP_TEST );
	if(pWndUpgradeBase != NULL)
		pWndUpgradeBase->Destroy();

	CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase( APP_SMELT_SAFETY );
	if(pWndSmeltSafety != NULL)
		pWndSmeltSafety->Destroy();

	CWndSmeltSafetyConfirm* pWndSmeltSafetyConfirm = (CWndSmeltSafetyConfirm*)GetWndBase( APP_SMELT_SAFETY_CONFIRM );
	if(pWndSmeltSafetyConfirm != NULL)
		pWndSmeltSafetyConfirm->Destroy();

	CWndEquipBindConfirm* pWndEquipBindConfirm = (CWndEquipBindConfirm*)GetWndBase(APP_EQUIP_BIND_CONFIRM);
	if(pWndEquipBindConfirm != NULL)
		pWndEquipBindConfirm->Destroy();

	CWndRestateConfirm* pWndRestateConfirm = (CWndRestateConfirm*)GetWndBase(APP_RESTATE_CONFIRM);
	if(pWndRestateConfirm != NULL)
		pWndRestateConfirm->Destroy();

	CWndPetFoodMill* pWndPetFoodMill = ( CWndPetFoodMill* )GetWndBase( APP_PET_FOODMILL );
	if(pWndPetFoodMill != NULL)
		pWndPetFoodMill->Destroy();

	// _JIN_SHOP_CART
	switch( _GetContentState(CT_SHOP_CART_17) )
	{
	case CS_VER1:
		{
			CWndShop* pWndShop = ( CWndShop* )GetWndBase( APP_SHOP_ );
			if( pWndShop != NULL )
				pWndShop->Destroy();
		}
		break;

	case CS_VER2:
		{
			CFLWndShop_Cart* pWndShop_Cart = ( CFLWndShop_Cart* )GetWndBase( APP_SHOP_CART_ );
			if( pWndShop_Cart != NULL )
				pWndShop_Cart->Destroy();
		}
		break;

	default:
		FLASSERT( 0 );
		break;
	}


	CWndBank* pWndBank = ( CWndBank* )GetWndBase( APP_COMMON_BANK );
	if( pWndBank != NULL )
		pWndBank->Destroy();

	g_toolTip.CancelToolTip();
}

void CWndInventory::OnMouseWndSurface( CPoint point )
{
	int nTemp = 0;
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		FLItemBase* pItemBase = NULL;
		
		pItemBase = g_pPlayer->GetEquipItem( i );
		
		CRect DrawRect = m_InvenRect[i];
		
		CPoint point = GetMousePoint();
		// 툴팁
		if( DrawRect.PtInRect( point ) )
		{
			nTemp = i;
			
			if( i == PARTS_LWEAPON )
			{
				pItemBase = g_pPlayer->GetEquipItem( PARTS_SHIELD );
				
				if( pItemBase )
				{
					nTemp = PARTS_SHIELD;
				}
				else
				{
					pItemBase = g_pPlayer->GetEquipItem( PARTS_LWEAPON );
					
					if( pItemBase == NULL )
						pItemBase = g_pPlayer->GetEquipItem( PARTS_RWEAPON );
				}
			}			

			if( pItemBase )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				// 장비창에 있는것 툴팁
				g_toolTip.SetSubToolTipNumber( 0 );
				g_WndMng.PutToolTip_Item( pItemBase, point2, &DrawRect, APP_INVENTORY );
				break;
			}
			else
			{
				// 장비창에 없는것 툴팁
				
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &DrawRect );
				
				CString strText;

				switch( nTemp )
				{
					case PARTS_UPPER_BODY:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_SUIT);
						break;
					case PARTS_HAND:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_GAUNTLET);
						break;
					case PARTS_FOOT:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_BOOTS );
						break;
					case PARTS_CAP:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_HELMET );
						break;
					case PARTS_CLOAK:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLO);
						break;
					case PARTS_LWEAPON:
					case PARTS_RWEAPON:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_WEAPON);
						break;
					case PARTS_SHIELD:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_GUARD  );
						break;
					case PARTS_MASK:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_MASK );
						break;
					case PARTS_RIDE:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_RID );
						break;
					case PARTS_NECKLACE1:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_NECKLACE  );
						break;
					case PARTS_RING1:
					case PARTS_RING2:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_RING   );
						break;
					case PARTS_EARRING1:
					case PARTS_EARRING2:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_EARRING   );
						break;
					case PARTS_BULLET:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_BULLET    );
						break;							
					case PARTS_HAT:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_01     );
						break;	
					case PARTS_CLOTH:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_02     );
						break;	
					case PARTS_GLOVE:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_03     );
						break;	
					case PARTS_BOOTS:
						strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_04     );
						break;							
				}

				g_toolTip.PutToolTip( 100000, strText, DrawRect, point2 );
				break;
			}
//			break;
		}
	}
}
void CWndInventory::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = g_pPlayer;
	if( !pMover )
		return;

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

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM21 );
	if( lpWndCtrl1 && m_TexRemoveItem )
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if( lpWndCtrl1->rect.PtInRect( point ) )
		{
			fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = lpWndCtrl1->rect;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			g_toolTip.PutToolTip( 100, prj.GetText(TID_GAME_INVEONTORY_REMOVE_TOOL), DrawRect, point2, 0 );
		}
		if(m_TexRemoveItem)
			m_TexRemoveItem->Render( p2DRender, lpWndCtrl1->rect.TopLeft(), CPoint( 27, 27 ), 255, fScal, fScal );
	}
#ifdef INVENTORY_ITEM_ALIGN
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM22 );
	if( lpWndCtrl2 && m_TexAlignItem )
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if( lpWndCtrl2->rect.PtInRect( point ) )
		{
			fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = lpWndCtrl2->rect;
			ClientToScreen( &point2 );
			ClientToScreen( &DrawRect );
			g_toolTip.PutToolTip( 100, prj.GetText(TID_GAME_INVENTORY_ALIGN_TOOL), DrawRect, point2, 0 );
		}
		if(m_TexAlignItem)
			m_TexAlignItem->Render( p2DRender, lpWndCtrl2->rect.TopLeft(), CPoint( 27, 27 ), 255, fScal, fScal );
	}
#endif	// INVENTORY_ITEM_ALIGN
	pGoldNum->SetTitle( strGoldNum );

	DWORD dwColor1 = D3DCOLOR_ARGB( 100, 0, 0,  0 );//D3DCOLOR_TEMP( 255,   0,   0,  50 );//
	DWORD dwColor2 = D3DCOLOR_ARGB( 255, 240, 240,  240 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//
	DWORD dwColor3 = D3DCOLOR_ARGB( 100, 200, 200,  200 );//D3DCOLOR_TEMP( 255,  80,  80, 120 );//

	PFONTCOLOR_WNDINVENTORY pFontColorWndInventory = g_WndFontColorManager->GetWndInventory();

	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		DWORD dwAlpha = 255;
		FLItemBase* pItemBase = g_pPlayer->GetEquipItem( i );
		if( i == PARTS_LWEAPON )		// 왼손무기 그릴타이밍일때
		{
			FLItemBase* pRWeapon = g_pPlayer->GetEquipItem( PARTS_RWEAPON );		// 오른손 무기를 꺼내보고
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
			if( ((FLItemElem*)pItemBase)->IsFlag( FLItemElem::expired ) )
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_XRGB( 255, 100, 100 ), sx, sy );
			}
			else
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft()+cpAdd, D3DCOLOR_ARGB( dwAlpha, 255, 255, 255 ), sx, sy );
			}

			FLItemElem *pItemElem = (FLItemElem *)pItemBase;
			if( pItemElem->GetProp()->dwPackMax > 1 )		// 묶음 아이템이냐?
			{
				int nItemNum	= pItemElem->m_nItemNum;

				TCHAR szTemp[ 32 ];
				FLSPrintf( szTemp, _countof( szTemp ), "%d", nItemNum );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				int x = DrawRect.left;	
				int y = DrawRect.top;

				m_p2DRender->TextOut( x + 42 - size.cx, y + 42 - size.cy, szTemp, pFontColorWndInventory->m_stItemCountShadow.GetFontColor() );
				m_p2DRender->TextOut( x + 41 - size.cx, y + 41 - size.cy, szTemp, pFontColorWndInventory->m_stItemCount.GetFontColor() );
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
/*		
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/		
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
	int nVisibleParts = PARTS_HAT;
	int nInVisibleParts = PARTS_CAP;
	if(!pMover->IsOptionRenderCostume(0) )
	{
		nVisibleParts = PARTS_CAP;
		nInVisibleParts = 0;
	}	
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		FLItemElem* pItemElem	= g_pPlayer->GetEquipItem( nInVisibleParts );

		O3D_ELEMENT* pElement = NULL;
		pElement = m_pModel->GetParts(PARTS_HAIR);
		if( pElement )
			pElement->m_nEffect &= ~XE_HIDE;

		if( pItemElem )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			FLItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( nVisibleParts );

			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				//외형변경이 아닌경우만 적용시켜야함.
				if( 0 == pItemElem->GetLooksChangeItemID() && NULL == pItemElemOvercoat )	//gmpbigsun( 20120604 ), #24885 : 얼굴사라짐 버그수정
					pElement = m_pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}

			if( NULL == pItemElemOvercoat )
				pItemElemOvercoat = pItemElem;
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( FLItemElem::expired )) )
				{
					PT_ITEM_SPEC pItemPropOC = NULL;
					
					//외형변경인 경우 해당아이템으로!
					if( 0 != pItemElemOvercoat->GetLooksChangeItemID() )
						pItemPropOC = g_xSpecManager->GetSpecItem( pItemElemOvercoat->GetLooksChangeItemID() );
					else 
						pItemPropOC = pItemElemOvercoat->GetProp();

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
		}
		else
		{
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			FLItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( nVisibleParts );
			
			if( pItemElemOvercoat )
			{
				if( !(pItemElemOvercoat->IsFlag( FLItemElem::expired )) )
				{
					PT_ITEM_SPEC pItemPropOC = NULL;

					//외형변경인 경우 해당아이템으로!
					if( 0 != pItemElemOvercoat->GetLooksChangeItemID() )
						pItemPropOC = g_xSpecManager->GetSpecItem( pItemElemOvercoat->GetLooksChangeItemID() );
					else 
						pItemPropOC = pItemElemOvercoat->GetProp();
					
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							m_pModel->SetEffect(PARTS_HAIR, XE_HIDE );
						
						m_pModel->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE);
					}
				}
			}
		}
		
		m_pModel->Render( p2DRender->m_pd3dDevice, &matWorld );
	}


	return;
}

void CWndInventory::UpDateModel()
{
	SAFE_DELETE( m_pModel );
	
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? OBJECT_INDEX( 11, MI_MALE ) : OBJECT_INDEX( 12, MI_FEMALE ));
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );	
}


void CWndInventory::OnInitialUpdate()
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


	
	CWndButton* pWndCheck = NULL;
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->SetCheck(g_pPlayer->IsOptionRenderCostume(0));

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	pWndCheck->SetCheck(g_pPlayer->IsOptionRenderCostume(1));

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	pWndCheck->SetCheck(g_pPlayer->IsOptionRenderCostume(2));

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	pWndCheck->SetCheck(g_pPlayer->IsOptionRenderCostume(3));

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	pWndCheck->SetVisible(false);


	SAFE_DELETE( m_pModel );

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? OBJECT_INDEX( 11, MI_MALE ) : OBJECT_INDEX( 12, MI_FEMALE ));
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
	prj.m_modelMng.LoadMotion( m_pModel,  OT_MOVER, nMover, MTI_STAND );
	UpdateParts();
	m_pModel->InitDeviceObjects( g_Neuz.GetDevice() );
	
	//_JIN_NEW_INVENTORY
	m_pTabCtrl = (CWndTabCtrl*) GetDlgItem( WIDC_INVENTORY );
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			//기존버전 -------------------------------
			m_wndItemCtrl.Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), m_pTabCtrl, 11 );
			m_wndItemCtrl.InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );

			WTCITEM tabTabItem;
			tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
			tabTabItem.pszText = GETTEXT( TID_GAME_ITEM );//"아이템";
			tabTabItem.pWndBase = &m_wndItemCtrl;
			m_pTabCtrl->InsertItem( 0, &tabTabItem );
			//----------------------------------------
		}
		break;
	case CS_VER2:
		{
			//17차버전----------------------------------
			m_pTabCtrl->SetTabTitleAlign( CWndTabCtrl::ALIGN_CENTER );
			WTCITEM tabTabItem_;
			tabTabItem_.mask = WTCIF_TEXT | WTCIF_PARAM;
			for( int i = 0; i < MAX_VENDOR_NEW_INVENTAB; i++ )
			{
				m_wndItemCtrl_[i].Create( WLVS_ICON, CRect( 0, 0, 250, 250 ), m_pTabCtrl, 11 );
				m_wndItemCtrl_[i].InitItem( &g_pPlayer->m_Inventory, APP_INVENTORY );
				DWORD dwTextID = 0;
				dwTextID = 4551 + i;
				tabTabItem_.pszText = GETTEXT(dwTextID);
				tabTabItem_.pWndBase = &m_wndItemCtrl_[ i ];
				m_pTabCtrl->InsertItem( i, &tabTabItem_ );		
			}
			for( int i = m_pTabCtrl->GetSize(); i < 3; i++ )
			{
				tabTabItem_.pszText = "";
				tabTabItem_.pWndBase = NULL;
				m_pTabCtrl->InsertItem( i, &tabTabItem_ );
			}	
			//------------------------------------------
		}
		break;
	default:
		FLASSERT(0);
		break;

	}
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_GOLD );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_GOLD );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );

	CWndStatic* pGoldNum = (CWndStatic*) GetDlgItem( WIDC_GOLD_NUM );
	pGoldNum->AddWndStyle(WSS_MONEY);

	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_bIsUpgradeMode = FALSE;
	m_dwEnchantWaitTime = 0xffffffff;

	m_TexRemoveItem = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndInventoryGarbage.dds" ), 0xffff00ff );
#ifdef INVENTORY_ITEM_ALIGN
	m_TexAlignItem = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndInventoryAlign.dds" ), 0xffff00ff );
#endif	// INVENTORY_ITEM_ALIGN
	
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
	memset( m_InvenRect, 0, sizeof(m_InvenRect) );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
//	return CWndNeuz::Create( WBS_VIEW | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME, rect, pWndParent, dwWndId );
}

// 인벤토리에서 재료아이템을 클릭 후 대상 아이템으로 강화할때 시간 딜레이를 준 후 강화가 될떄 사용됩니다.
BOOL CWndInventory::Process()
{
	if( m_pModel )
		m_pModel->FrameMove();

	if( m_dwEnchantWaitTime < g_tmCurrent )
	{
		m_dwEnchantWaitTime = 0xffffffff;
		
		if( m_pSfxUpgrade )
		{
			m_pSfxUpgrade->Delete();
			m_pSfxUpgrade = NULL;
		}
		
		FLItemElem* pItemElem = (FLItemElem*)m_pUpgradeItem;
		if( pItemElem && m_pUpgradeMaterialItem )
		{
			FLItemElem* pItemMaterialElem = (FLItemElem*)m_pUpgradeMaterialItem;
			// 패킷 전송
			
			if( pItemMaterialElem->GetProp() )
			{
				// 인첸트에 관한 아이템이냐?
				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL )
				{
					g_DPlay.SendUltimateEnchantWeapon( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}
				else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SMELT_ACCESSORY_MATERIAL )
				{
					if( pItemElem->GetProp()->IsAccessory() == TRUE )
					{
						FLPacketItemUpgradeGeneralEnchantReq kPacket;

						kPacket.dwMainItemObjID			= pItemElem->m_dwObjId;
						kPacket.dwMaterialItemObjID		= m_pUpgradeMaterialItem->m_dwObjId;

						g_DPlay.SendPacket( &kPacket );

//						g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					}
					else if( pItemElem->IsCollector( TRUE ) == TRUE )
					{
						g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					}
				}
				else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_PIERDICE
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SMELT_COSTUME_MATERIAL
#endif
					)
				{
					g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}

// 				if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD 
// 					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ENCHANT
// 					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_PIERDICE	//sun: 9,10차 제련 __ULTIMATE
// 					)
// 				{
// 					if(pItemMaterialElem->m_dwItemId == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ))
// 					{
// 						g_DPlay.SendUltimateEnchantWeapon( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
// 					}
// 					else if(pItemMaterialElem->m_dwItemId == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemMaterialElem->m_dwItemId == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ))
// 					{
// 						if( pItemElem->IsCollector( TRUE ) || pItemElem->GetProp()->dwItemKind2 == IK2_JEWELRY )
// 						{
// 							g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
// 						}
// 					}
// 					else
// 					{
// 						g_DPlay.SendEnchant( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
// 					}
// 				}
				// 피어싱에 관한 아이템이냐?
				else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD2
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD3
#endif
					)
				{
					g_DPlay.SendPiercing( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
				}
				else if( IsNeedTarget( pItemMaterialElem->GetProp() ) )
				{
					if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_GENERAL_RANDOMOPTION_GEN
						|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_EATPET_RANDOMOPTION_GEN
						|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SYSTEMPET_RANDOMOPTION_GEN
						|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_COSTUME_RANDOMOPTION_GEN )
					{
						FLPacketItemUpgradeRandomOptionGenerateReq kPacket;

						kPacket.dwMainItemObjID			= pItemElem->m_dwObjId;
						kPacket.dwMaterialItemObjID		= m_pUpgradeMaterialItem->m_dwObjId;

						g_DPlay.SendPacket( &kPacket );
					}
					else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_GENERAL_RANDOMOPTION_INIT
						|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SYSTEMPET_RANDOMOPTION_INIT )
					{
						FLPacketItemUpgradeRandomOptionInitializeReq kPacket;

						kPacket.dwMainItemObjID			= pItemElem->m_dwObjId;
						kPacket.dwMaterialItemObjID		= m_pUpgradeMaterialItem->m_dwObjId;

						g_DPlay.SendPacket( &kPacket );
					}
					else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_DECREASE_EQUIP_LEVEL )
					{
						FLPacketItemUpgradeEquipLevelDecreaseReq kPacket;

						kPacket.dwMainItemObjID			= pItemElem->m_dwObjId;
						kPacket.dwMaterialItemObjID		= m_pUpgradeMaterialItem->m_dwObjId;

						g_DPlay.SendPacket( &kPacket );
					}
					else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_LOOKRESTOREITEM )
					{
						//added by gmpbigsun( 20111129 ) : 외형변경 취소
						FLPacketItemUpgradeLooksInitializeReq kPacket;

						kPacket.dwMainItemObjID = pItemElem->m_dwObjId;
						kPacket.dwMaterialItemObjID = m_pUpgradeMaterialItem->m_dwObjId;

						g_DPlay.SendPacket( &kPacket );
					}
					else
					{
						g_DPlay.SendDoUseItemTarget( m_pUpgradeMaterialItem->m_dwObjId, pItemElem->m_dwObjId );
					}
				}
				else if( pItemMaterialElem->GetProp()->dwItemKind3 == IK3_RANDOM_SCROLL )
				{
					SAFE_DELETE( g_WndMng.m_pWndRandomScrollConfirm );
					g_WndMng.m_pWndRandomScrollConfirm = new CWndRandomScrollConfirm;
					if( 0 < pItemElem->GetRandomOptionOriginID() )
					{
						g_WndMng.m_pWndRandomScrollConfirm->SetItem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId, TRUE );
					}
					else
					{
						g_WndMng.m_pWndRandomScrollConfirm->SetItem( pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId );
					}

					g_WndMng.m_pWndRandomScrollConfirm->Initialize();
				}
			}
		}
	}
	return CWndBase::Process();
}

BOOL CWndInventory::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	// 인벤토리로 아이템이 Drop되었다. 어디서 왔는지 확인한후 적절히 대응하라.
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if( lpShortcut->m_pFromWnd == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "m_pFromWnd 가 NULL" ) );
			return CWndNeuz::OnChildNotify( message, nID, pLResult );
		}
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

		if( pWndFrame == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "pWndFrame==NULL" ) );
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
			if( pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwIndex != ITEM_INDEX( 12, II_GOLD_SEED1 ) )
			{

				int nPart = -1;
				if( m_pSelectItem )
					nPart = m_pSelectItem->m_dwObjIndex;

				g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, lpShortcut->m_dwId ), 0, nPart );
				bForbid = FALSE;
			}
			else if( pWndFrame->GetWndId() == APP_SHOP_ )// 숍에서 온거라면 구입 
			{
				CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
				for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
				{
					int nItem = pWndItemCtrl->GetSelectedItem( i );
					pWndItemCtrl->GetItem( nItem );
				}
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
				CWndShop* pwndShop = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
				if(pwndShop)
				{
					LPCHARACTER lpCharacter = pwndShop->m_pMover->GetCharacter();
					if(lpCharacter->m_nVenderType == VENDOR_TYPE_CHIP)
					{
						if(g_pPlayer->m_Inventory.GetItemNumByItemId( ITEM_INDEX( 26460, II_CHP_RED ) ) - (int)((FLItemElem*)lpShortcut->m_dwData)->GetChipCost() >= 0) //구매하려는 품목의 칩개수 이상을 가지고 있는지 확인.
						{
							SAFE_DELETE( m_pWndConfirmBuy );
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (FLItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->m_nBuyType = 1;
							m_pWndConfirmBuy->m_pMover = pwndShop->m_pMover;
							m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_GAME_CANNT_BY_REDCHIP) ) );
					}
					else if(lpCharacter->m_nVenderType == VENDOR_TYPE_PENYA)
					{
						if( g_pPlayer->GetGold() - ( ( ( FLItemElem* )lpShortcut->m_dwData )->GetCost() * prj.m_fShopBuyRate ) >= 0 )
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
//						if( g_pPlayer->GetGold() - ((FLItemElem*)lpShortcut->m_dwData)->GetCost() >= 0)
						{
							SAFE_DELETE( m_pWndConfirmBuy );
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (FLItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->m_pMover = pwndShop->m_pMover;
							m_pWndConfirmBuy->Initialize( this, APP_CONFIRM_BUY_ );
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0048) ) );
//sun: 11, 칩으로 상점 이용 하기 (길드 대전 관련)
					}
				}

			}
//sun: 8, // __S8_VENDOR_REVISION
			else if( pWndFrame->GetWndId() == APP_VENDOR_REVISION )
//!			else if( pWndFrame->GetWndId() == APP_VENDOREX )
			{
				CWndVendor* pWndVendor	= (CWndVendor*)pWndFrame;
//sun: 8, // __S8_VENDOR_REVISION
				if( pWndVendor->m_pVendor == g_pPlayer )
				{
					g_DPlay.SendUnregisterPVendorItem( (BYTE)( lpShortcut->m_dwIndex ) );
				}
				else
				{
					SAFE_DELETE( pWndVendor->m_pWndVendorBuy );
					pWndVendor->m_pWndVendorBuy		= new CWndVendorBuy( (FLItemBase*)lpShortcut->m_dwData, lpShortcut->m_dwIndex/*nItem*/ );
					pWndVendor->m_pWndVendorBuy->Initialize( pWndVendor, APP_VENDOR_BUY );
				}
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
					
					for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					FLItemElem* itemElem = (FLItemElem*)lpShortcut->m_dwData;
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
						//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
						g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEITEM ) );

						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendGetItemBank( nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
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
					//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
					g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEMONEY ) );

					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
					CString strMain = prj.GetText( TID_GAME_MOVEPENYA );//"얼마를 이동하시겠습니까?";
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
				bForbid = FALSE;
			}
//sun: 11, 주머니
			// 휴대가방에서 온 경우
			if( pWndFrame->GetWndId() == APP_BAG_EX )
			{
				
				CWndBase* pWndFrom = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					if( pWndFrom->m_pParentWnd->GetWndId() == WIDC_BASIC )
					{					
						nSlot = 0;
					}
					else if( pWndFrom->m_pParentWnd->GetWndId() == WIDC_EXBAG1 )
					{
						nSlot = 1;
//						if(!g_pPlayer->m_Pocket.IsAvailable(1)) return FALSE;
					}
					else
					{
						nSlot = 2;
//						if(!g_pPlayer->m_Pocket.IsAvailable(2)) return FALSE;
					}
					
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					
					for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
					{
						int nItem = pWndItemCtrl->GetSelectedItem( i );
						pWndItemCtrl->GetItem( nItem );
					}

					FLItemElem* itemElem = (FLItemElem*)lpShortcut->m_dwData;
					if( itemElem->m_nItemNum > 1 )
					{ 
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BAG_EX; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = -1;
						g_WndMng.m_pWndTradeGold->m_nPutSlot = nSlot;
						
						g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
						g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEITEM ) );

						CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
						CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
						CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendMoveItem_Pocket( nSlot, lpShortcut->m_dwId, 1 , -1);
					}
				}
				
				bForbid = FALSE;
			}
			else
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
			if( pWndFrame->GetWndId() == APP_GUILD_BANK )
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if( pGuild && pGuild->IsGetItem( g_pPlayer->m_idPlayer ))
					{
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
						
						for( int i = 0; i < (int)( pWndItemCtrl->GetSelectedCount() ); i++ )
						{
							int nItem = pWndItemCtrl->GetSelectedItem( i );
							pWndItemCtrl->GetItem( nItem );
						}
						FLItemElem* itemElem = (FLItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;	// ****중요****
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							
							g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
							g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEITEM ) );
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendGetItemGuildBank( (BYTE)( lpShortcut->m_dwId ), 1, 1 );
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
						//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
						g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEMONEY ) );
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
	// 차일드로부터 더블클릭 알림 메시지가 왔다. 아이템 콘트롤에서 아이템을 사용하겠다는 메시지다. ( 20111109: R버튼 클릭도 똑같은 동작으로 추가댐 )
	if( WIN_DBLCLK == message ) //|| WIN_RCLK == message )
	{
		if( nID == 11 ) // item
		{
			if( m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE) );
				return 0;
			}

			if( GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01) );
				return 0;
			}

			if( GetWndBase(APP_COMMITEM_DIALOG) != NULL )
			{
				g_WndMng.PutString( prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR) );
				return 0;
			}
			
			CCtrl* pCtrl = (CCtrl*)g_WorldMng()->GetObjFocus();
			DWORD dwObjId = NULL_ID;
			if( pCtrl && pCtrl->GetType() != OT_OBJ )
				dwObjId = pCtrl->GetId();
			FLItemBase* pFocusItem = (FLItemBase*) pLResult;
			BOOL	bAble = TRUE;
			if( pFocusItem )
			{
				PT_ITEM_SPEC pProp = pFocusItem->GetProp();
				// 소비아이템이 1개 남았을때 더블클릭하고 서버로 갔다가 
				// 삭제되기 전에 또 더블클릭하면 pFocusItem->m_dwItemId가 0이 되면서
				// 프로퍼티가 널되서 죽는다.
				if( pProp )
				{	
					//gmpbigsun( 20111110 ) : 사용의사를 밝힌 아이템에 대해서 처리를 하고 슬롯에 올려졌다는등의 행위로 바로 
					//패킷을 보내지 말아야 하는경우 리턴해버려야 함.
					BOOL bExcuted = ExcuteItem( pFocusItem );
					if( TRUE == bExcuted )
						return TRUE;

					//sun: 8차 엔젤 소환 Neuz, World, Trans
					if(g_WndMng.GetWndBase( APP_SUMMON_ANGEL ))
					{
						if(pProp->dwID == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pProp->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) ||
							pProp->dwID == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 ) || pProp->dwID == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ))
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{
								CWndSummonAngel* pWndSummonAngel = (CWndSummonAngel*)g_WndMng.GetWndBase( APP_SUMMON_ANGEL );
								pWndSummonAngel->SetDie((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}

//sun: 10, __EVE_MINIGAME
					if(g_WndMng.GetWndBase( APP_MINIGAME_WORD ))
					{
						if(pProp->dwID >= ITEM_INDEX( 6370, II_SYS_SYS_EVE_A_CARD ) && pProp->dwID <= ITEM_INDEX( 6395, II_SYS_SYS_EVE_Z_CARD ) )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{								
								CWndFindWordGame* pWndFindWordGame = (CWndFindWordGame*)g_WndMng.GetWndBase( APP_MINIGAME_WORD );
								pWndFindWordGame->SetWord((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
					if(g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndPuzzleGame* pWndPuzzleGame = (CWndPuzzleGame*)g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE );
							pWndPuzzleGame->SetPicture((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}

//sun: 10, 속성제련 제거(10차로 변경)
					if(g_WndMng.GetWndBase( APP_REMOVE_ATTRIBUTE ))
					{
						if( FLItemElem::IsEleRefineryAble(pProp) )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase( APP_REMOVE_ATTRIBUTE );
								pWndRemoveAttribute->SetWeapon((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}

//sun: 11, 피어싱 옵션 제거
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_PIERCING_EX ))
					{
						if( FLItemElem::IsEleRefineryAble(pProp) 
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
							|| pProp->dwItemKind3 == IK3_BARUNA_GEN_PIC_MAT_CANCEL
#endif
							)
						{
//							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							if( IsUsableItem( pFocusItem ) )
							{			
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
								CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase( APP_SMELT_REMOVE_PIERCING_EX );
								pWndRemovePiercing->SetItem((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
					if(g_WndMng.GetWndBase( APP_SMELT_REMOVE_JEWEL ))
					{
						if( pProp->dwReferStat1 == WEAPON_ULTIMATE || pProp->dwItemKind2 == IK2_MATERIAL 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
							|| pProp->IsCostumeEnhanceParts()
#endif

#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
							|| pProp->IsBarunaWeapon()
#endif
							)
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndRemoveJewel* pWndRemoveJewel = (CWndRemoveJewel*)GetWndBase( APP_SMELT_REMOVE_JEWEL );
								pWndRemoveJewel->SetItem((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
//sun: 12, 펫 알 변환 기능 추가
					if(g_WndMng.GetWndBase( APP_PET_TRANS_EGGS ))
					{	
						if(g_pPlayer->IsUsing((FLItemElem*)pFocusItem))
						{
							g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR2 ) );
							return TRUE;
						}
							
						if( (pProp->dwItemKind3 == IK3_EGG && ((FLItemElem*)pFocusItem)->m_pPet == NULL) ||
							(pProp->dwItemKind3 == IK3_EGG && ((FLItemElem*)pFocusItem)->m_pPet && ((FLItemElem*)pFocusItem)->m_pPet->GetLevel() == PL_EGG) )
						{
							if( IsUsableItem( pFocusItem ) )
							{			
								CWndPetTransEggs* pWndPetTransEggs = (CWndPetTransEggs*)GetWndBase( APP_PET_TRANS_EGGS );
								pWndPetTransEggs->SetItem((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
						else
						{
							g_WndMng.PutString( prj.GetText( TID_GAME_TRANS_EGGS_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_TRANS_EGGS_ERROR1 ) );
							return TRUE;
						}
					}

					if(g_WndMng.GetWndBase( APP_SMELT_SAFETY ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase( APP_SMELT_SAFETY );
							pWndSmeltSafety->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}
					
					if(g_WndMng.GetWndBase( APP_CARD_UPGRADE ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndCardUpgrade* pWndCardUpgrade = (CWndCardUpgrade*)GetWndBase( APP_CARD_UPGRADE );
							pWndCardUpgrade->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}


					if(g_WndMng.GetWndBase( APP_BARUNA_ENCHANT ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndEnchantBaruna* pWndEnchantBaruna = (CWndEnchantBaruna*)GetWndBase( APP_BARUNA_ENCHANT );
							pWndEnchantBaruna->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}

					if(g_WndMng.GetWndBase( APP_BARUNA_EXTRACT_OFFER ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndExtractOffer* pWndExtractOffer = (CWndExtractOffer*)GetWndBase( APP_BARUNA_EXTRACT_OFFER );
							pWndExtractOffer->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}

					if(g_WndMng.GetWndBase( APP_BARUNA_OFFER ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndProduceOffer* pWndProduceOffer = (CWndProduceOffer*)GetWndBase( APP_BARUNA_OFFER );
							pWndProduceOffer->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}

					if(g_WndMng.GetWndBase( APP_BARUNA_SEED ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndProduceSeed* pWndProduceSeed = (CWndProduceSeed*)GetWndBase( APP_BARUNA_SEED );
							pWndProduceSeed->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}

					if(g_WndMng.GetWndBase( APP_BARUNA_OFFERSEED ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CWndProduceOfferSeed* pWndProduceOfferSeed = (CWndProduceOfferSeed*)GetWndBase( APP_BARUNA_OFFERSEED );
							pWndProduceOfferSeed->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}

					if(g_WndMng.GetWndBase( APP_BARUNA_PIERCE ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							FLWndBarunaPearcing* pWndBarunaPearcing = (FLWndBarunaPearcing*)GetWndBase( APP_BARUNA_PIERCE );
							pWndBarunaPearcing->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}
					
					if(g_WndMng.GetWndBase( APP_BARUNA_ELEMENT_SMELT ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CFLWndBaruna_Element_Smelt* pWndBaruna_Element_Smelt = (CFLWndBaruna_Element_Smelt*)GetWndBase( APP_BARUNA_ELEMENT_SMELT );
							pWndBaruna_Element_Smelt->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}
					
					if(g_WndMng.GetWndBase( APP_BARUNA_WAKEUP ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CFLWndBaruna_Wakeup* pWndBaruna_Wakeup = (CFLWndBaruna_Wakeup*)GetWndBase( APP_BARUNA_WAKEUP );
							pWndBaruna_Wakeup->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}
					
					if(g_WndMng.GetWndBase( APP_BARUNA_WAKEUP_CANCEL ))
					{
						if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
						{			
							CFLWndBaruna_Wakeup_Cancel* pWndBaruna_Wakeup_Cancel = (CFLWndBaruna_Wakeup_Cancel*)GetWndBase( APP_BARUNA_WAKEUP_CANCEL );
							pWndBaruna_Wakeup_Cancel->SetItem((FLItemElem*)pFocusItem);
							return TRUE;
						}
					}
					
					
//sun: 9차 전승관련 Clienet
					if(g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL ))
					{
						if(pProp->dwID == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pProp->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) ||
							pProp->dwID == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 ) || pProp->dwID == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ))
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
								pWndMixJewel->SetJewel((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#ifdef COSTUME_UPGRADE_MIX
					if(g_WndMng.GetWndBase( APP_SMELT_MIXCOSTUME ))
					{
						CWndItemMix* pWndItemMix = (CWndItemMix*)GetWndBase( APP_SMELT_MIXCOSTUME );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
						if( pWndItemMix && (pWndItemMix->m_eWndMode == CWndItemMix::WND_COSTUME && (pProp->IsCostumeEnhanceParts() && ((FLItemElem*)pFocusItem)->m_dwKeepTime == 0 && ((FLItemElem*)pFocusItem)->IsBindState() == false )
							|| (pWndItemMix->m_eWndMode == CWndItemMix::WND_MATERIAL && ( pProp->dwID >= ITEM_INDEX( 25435, II_SYS_SYS_SCR_ENCHANT_ABSOLUTE06 ) && pProp->dwID < ITEM_INDEX( 25439, II_SYS_SYS_SCR_ENCHANT_ABSOLUTE10 )))
							))

#endif
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								
								pWndItemMix->SetJewel((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#endif
					if(g_WndMng.GetWndBase( APP_SMELT_JEWEL ))
					{
						if( pProp->dwID == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ) ||
							pProp->dwID == ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ) ||
							pProp->dwID == ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ) ||
							pProp->dwID == ITEM_INDEX( 2032, II_GEN_MAT_RUBY ) ||
							pProp->dwID == ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ ) )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)GetWndBase( APP_SMELT_JEWEL );
								pWndSmeltJewel->SetJewel((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
					if(g_WndMng.GetWndBase( APP_SMELT_JEWEL_NEW ))
					{
						if( pProp->IsCostumeEnhanceParts() ||
							pProp->dwID == ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) ||
							pProp->dwID == ITEM_INDEX( 25339, II_GEN_MAT_TURQUOISE ) ||
							pProp->dwID == ITEM_INDEX( 25340, II_GEN_MAT_AQUAMARINE ) ||
							pProp->dwID == ITEM_INDEX( 25341, II_GEN_MAT_AMETHYST ) )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndNewSmeltJewel* pWndNewSmeltJewel = (CWndNewSmeltJewel*)GetWndBase( APP_SMELT_JEWEL_NEW );
								pWndNewSmeltJewel->SetJewel((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
#endif
					if(g_WndMng.GetWndBase( APP_SMELT_CHANGEWEAPON ))
					{
						if( pProp->dwItemKind1 == IK1_WEAPON ||
							pProp->dwID == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ) ||
							pProp->dwID == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ) ||
							pProp->dwID == ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ) ||
							pProp->dwID == ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ) ||
							pProp->dwID == ITEM_INDEX( 2032, II_GEN_MAT_RUBY ) ||
							pProp->dwID == ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ ) )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndChangeWeapon* pWndChangeWeapon = (CWndChangeWeapon*)GetWndBase( APP_SMELT_CHANGEWEAPON );
								pWndChangeWeapon->SetItem((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
//sun: 10차 전승시스템	Neuz, World, Trans
					if(g_WndMng.GetWndBase( APP_HERO_SKILLUP ))
					{
						if( pProp->dwID == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ) ||
							pProp->dwID == ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ) ||
							pProp->dwID == ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ) ||
							pProp->dwID == ITEM_INDEX( 2032, II_GEN_MAT_RUBY ) ||
							pProp->dwID == ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ ) )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndHeroSkillUp* pWndHeroSkillUp = (CWndHeroSkillUp*)GetWndBase( APP_HERO_SKILLUP );
								pWndHeroSkillUp->SetJewel((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
					
					if(g_WndMng.GetWndBase( APP_SMELT_EXTRACTION ))
					{
						if( pProp->dwItemKind1 == IK1_WEAPON )
						{
							if(pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum)
							{			
								CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase( APP_SMELT_EXTRACTION );
								pWndExtraction->SetWeapon((FLItemElem*)pFocusItem);
								return TRUE;
							}
						}
					}
					// 속성카드나 주나위를 더블클릭했을경우 인첸트모드로 변경한다.
					if( (pFocusItem->GetExtra() < ((FLItemElem*)pFocusItem)->m_nItemNum) 
						&& ( 
							pProp->dwItemKind3 == IK3_ELECARD
//							|| pProp->dwItemKind3 == IK3_ENCHANT 
							|| pProp->dwItemKind3 == IK3_SOCKETCARD
							|| pProp->dwItemKind3 == IK3_RANDOM_SCROLL
							|| pProp->dwItemKind3 == IK3_PIERDICE 
							|| IsNeedTarget( pProp )					//sun: 11,
							|| pProp->dwItemKind3 == IK3_SOCKETCARD2	//sun: 12, 무기 피어싱
							|| pProp->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL 
							|| pProp->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL 
							|| pProp->dwItemKind3 == IK3_SMELT_ACCESSORY_MATERIAL 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
							|| pProp->dwItemKind3 == IK3_SMELT_COSTUME_MATERIAL 
#endif							
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
							|| pProp->dwItemKind3 == IK3_SOCKETCARD3
#endif
							) 
						)
					{
						if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
						{
							g_WndMng.PutString( prj.GetText( TID_SBEVE_NOTUSEITEM ), NULL, prj.GetTextColor( TID_SBEVE_NOTUSEITEM ) );
							return FALSE;
						}

						bAble = FALSE;
						m_bIsUpgradeMode = TRUE;
						m_pUpgradeMaterialItem = pFocusItem;	

						if((g_pPlayer->IsMode( TRANSPARENT_MODE ) ) == 0)
							m_pSfxUpgrade = CreateSfx( g_Neuz.m_pd3dDevice, XI_INDEX( 1716, XI_INT_INCHANT ), g_pPlayer->GetPos(), g_pPlayer->GetId(), g_pPlayer->GetPos(), g_pPlayer->GetId(), -1 );
					}
					
					if( pProp->dwItemKind1 == IK1_RIDE )	// 사용한 아이템이 탈것인가.
					{
						if( g_pPlayer->m_pActMover->IsFly() == FALSE )		// Ground상태인가?
							if( g_pPlayer->m_pActMover->m_bGround == 0 )		// 땅에 발을 안붙이고 있다면 못탄다.
								bAble = FALSE;	// 아이템사용 실패.
					}

					if( pFocusItem->m_dwItemId == ITEM_INDEX( 4801, II_SYS_SYS_MAP_FLARIS ) )
					{
						g_WndMng.OpenMap( "map_flaris.tga" );	// 파일이 없어서 잠시 막아둠
					}
					else
					if( pFocusItem->m_dwItemId == ITEM_INDEX( 4802, II_SYS_SYS_MAP_SAINTMORNING ) )
					{
						g_WndMng.OpenMap( "map_saintmorning.tga" );
					}
					
//sun:13, 제련 확장(속성, 일반)
					if( pFocusItem->m_dwItemId == ITEM_INDEX( 20037, II_SYS_SYS_SCR_SOKCHANG ) )
					{
						if(g_WndMng.m_pWndChangeAttribute)
							SAFE_DELETE(g_WndMng.m_pWndChangeAttribute);

						g_WndMng.m_pWndChangeAttribute = new CWndChangeAttribute;
						g_WndMng.m_pWndChangeAttribute->SetChangeItem((FLItemElem*)pFocusItem);
						g_WndMng.m_pWndChangeAttribute->Initialize(&g_WndMng, APP_COMMITEM_DIALOG);
						bAble = FALSE;
					}

					if(pProp && pProp->dwID == ITEM_INDEX( 10434, II_SYS_SYS_SCR_RECCURENCE ) || pProp->dwID == ITEM_INDEX( 26316, II_SYS_SYS_SCR_RECCURENCE_LINK ) )
					{	// 리스킬 아이템은 모두 확인 창 띄운다.
						if(g_pPlayer->m_nSkillPoint < g_pPlayer->GetCurrentMaxSkillPoint())
						{
							SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
							g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
							g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
							g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
						}
						else
							g_WndMng.PutString( prj.GetText(TID_GAME_ERROR_SKILLRECCURENCE), NULL, prj.GetTextColor(TID_GAME_ERROR_SKILLRECCURENCE) );

						bAble = FALSE;
					}

					if( pFocusItem->m_dwItemId == ITEM_INDEX( 10211, II_CHR_SYS_SCR_RESTATE ) || 
						pFocusItem->m_dwItemId == ITEM_INDEX( 30151, II_CHR_SYS_SCR_RESTATE_STR ) || 
						pFocusItem->m_dwItemId == ITEM_INDEX( 30153, II_CHR_SYS_SCR_RESTATE_STA ) || 
						pFocusItem->m_dwItemId == ITEM_INDEX( 30152, II_CHR_SYS_SCR_RESTATE_DEX ) || 
						pFocusItem->m_dwItemId == ITEM_INDEX( 30154, II_CHR_SYS_SCR_RESTATE_INT )
						|| pFocusItem->m_dwItemId == ITEM_INDEX( 30337, II_CHR_SYS_SCR_RESTATE_STR_LOW )
						|| pFocusItem->m_dwItemId == ITEM_INDEX( 30339, II_CHR_SYS_SCR_RESTATE_STA_LOW )
						|| pFocusItem->m_dwItemId == ITEM_INDEX( 30338, II_CHR_SYS_SCR_RESTATE_DEX_LOW )
						|| pFocusItem->m_dwItemId == ITEM_INDEX( 30340, II_CHR_SYS_SCR_RESTATE_INT_LOW )
						)
					{
						SAFE_DELETE( g_WndMng.m_pWndRestateConfirm );
						g_WndMng.m_pWndRestateConfirm = new CWndRestateConfirm(pFocusItem->m_dwItemId);
						g_WndMng.m_pWndRestateConfirm->SetInformation(MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, pFocusItem->GetProp()->dwParts);
						g_WndMng.m_pWndRestateConfirm->Initialize(NULL);
						bAble = FALSE;
					}

					// 엘딘의 항아리 ( 이프문 토한다 ... 함수로 빼자... )
					if( CS_VER1 == _GetContentState( CT_ELLDINS_JAR ) )
					{
						FLERROR_ASSERT_LOG_RETURN( pFocusItem, FALSE, _T("[윈도우 인벤토리] 선택되어 있는 아이템이 없습니다.") );
						
						if( pFocusItem->m_dwItemId == ITEM_INDEX( 23773, II_GEN_MAT_ELL_ELLDINKEY ) )
						{
							m_pUnSealKeyItem = pFocusItem;	
							bAble = FALSE;
						}
					}

					//_JIN_ITEM_CANCELLATION
 					switch(_GetContentState(CT_CANCELLATION))
					{
					case CS_VER1:
						{
							if( pFocusItem->m_dwItemId == ITEM_INDEX( 20793, II_SYS_SYS_SCR_CANCELLATION ) )
							{
								SAFE_DELETE(g_WndMng.m_pWndCancellation);

								g_WndMng.m_pWndCancellation = new CFLWndCancellation;
								g_WndMng.m_pWndCancellation->Initialize(&g_WndMng,APP_CANCELLATION);
							}
						}
						break;
					default:
						//FLASSERT( 0 );
						break;
					}

					if(_GetContentState(CT_SERVER_MOVEMENT) == CS_VER1)
					{
						if( pFocusItem->m_dwItemId == ITEM_INDEX( 23708, II_SYS_SYS_SCR_CHARACTERSERVERMOVEITEM ) )
						{
							SAFE_DELETE( g_WndMng.m_pWndCharServerMovement_Warning);
							g_WndMng.m_pWndCharServerMovement_Warning = new CFLWndCharServerMovement_Warning();
							g_WndMng.m_pWndCharServerMovement_Warning->SetItemID(pFocusItem->m_dwObjId, pFocusItem->m_dwItemId);
							g_WndMng.m_pWndCharServerMovement_Warning->Initialize(&g_WndMng, APP_SERVERMOVE_WARNING);
							return FALSE;
						}
					}			
					if( ((FLItemElem*)pFocusItem)->m_bCharged != 1 )
					{
						switch( pProp->dwID )
						{
						case ITEM_INDEX( 10430, II_SYS_SYS_SCR_BLESSING ):
						case ITEM_INDEX( 25197, II_SYS_SYS_SCR_BLESSING_02 ):
							{
								if( g_pPlayer->IsSMMode( SM_REVIVAL ) || g_pPlayer->IsSMMode( SM_REVIVAL2 ) )
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

//						case ITEM_INDEX( 10434, II_SYS_SYS_SCR_RECCURENCE ):
//						case ITEM_INDEX( 26316, II_SYS_SYS_SCR_RECCURENCE_LINK ):
//							{
//								SAFE_DELETE( g_WndMng.m_pWndCommItemDlg );
//								g_WndMng.m_pWndCommItemDlg = new CWndCommItemDlg;
//								g_WndMng.m_pWndCommItemDlg->Initialize( &g_WndMng, APP_COMMITEM_DIALOG );
//								g_WndMng.m_pWndCommItemDlg->SetItem( TID_GAME_SKILLINIT, pFocusItem->m_dwObjId, dwObjId );
//								bAble = FALSE;
//							}
//							break;
						case ITEM_INDEX( 10425, II_SYS_SYS_SCR_CHACLA ):
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
										FLItemElem* pArmor;
										pArmor	= g_pPlayer->m_Inventory.GetEquip( dwParts );
										
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
				if( g_WndMng.GetWndBase( APP_SHOP_ )  ||
					g_WndMng.GetWndBase( APP_BANK )  ||
					g_WndMng.GetWndBase( APP_TRADE ) ||
					g_WndMng.GetWndBase( APP_SHOP_CART_ )||
					g_WndMng.GetWndBase( APP_VENDOR_REVISION ))
             	{
					//g_WndMng.PutString( "거래중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
				else if(g_WndMng.GetWndBase(APP_INVEN_REMOVE_ITEM)||
						g_WndMng.GetWndBase(APP_DIALOG)||
						g_WndMng.GetWndBase(APP_DIALOG_EX)||
						g_WndMng.GetWndBase(APP_DIALOG_EVENT))
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE), NULL, 0xffff0000 );				
				}
				//sun: 8차 엔젤 소환 Neuz, World, Trans
				else if(g_WndMng.GetWndBase( APP_SUMMON_ANGEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}

//sun: 10, __EVE_MINIGAME
				else if(g_WndMng.GetWndBase( APP_MINIGAME_DICE ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_KAWIBAWIBO_WIN ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_PUZZLE ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_MINIGAME_WORD ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}

//sun: 9차 전승관련 Clienet
				else if(g_WndMng.GetWndBase( APP_SMELT_EXTRACTION ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_JEWEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_MIXJEWEL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				else if(g_WndMng.GetWndBase( APP_SMELT_JEWEL_NEW ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#endif
#ifdef COSTUME_UPGRADE_MIX
				else if(g_WndMng.GetWndBase( APP_SMELT_MIXCOSTUME ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
#endif
				else if(g_WndMng.GetWndBase( APP_PET_FOODMILL ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if(g_WndMng.GetWndBase( APP_SMELT_SAFETY ))
				{
					g_WndMng.PutString( prj.GetText(TID_SBEVE_NOTUSEITEM), NULL, prj.GetTextColor(TID_SBEVE_NOTUSEITEM) );
				}
				else if( g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_QUIZ )
				{
					g_WndMng.PutString( prj.GetText( TID_SBEVE_NOTUSEITEM ), NULL, prj.GetTextColor( TID_SBEVE_NOTUSEITEM ) );
				}
				else if( g_WndMng.GetWndBase( APP_REPAIR ) )
				{
					//g_WndMng.PutString( "수리중에 아이템 사용은 불가능해요.", NULL, 0xffff0000 );
					g_WndMng.PutString( prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING) );
				}
				else
				{
					FLItemElem* pItemElem = (FLItemElem*)pFocusItem;
					if( pItemElem != NULL )
					{
						PT_ITEM_SPEC pItemProp = pFocusItem->GetProp();
						if( pItemProp != NULL )
						{
							//if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( FLItemElem::binds ) )
							if( pItemProp->nBindCondition == BIND_CONDITION_USE
								&& pItemProp->dwParts != NULL_ID
								&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
								&& pItemElem->IsFlag( FLItemElem::reset_bind ) == FALSE )
							{
								SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
									g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK);
								g_WndMng.m_pWndEquipBindConfirm->SetInformation(pFocusItem, dwObjId);
								g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
							}
							else if( pItemProp->nResetBindCondition == BIND_CONDITION_USE
								&& pItemProp->dwParts != NULL_ID
								&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
								&& pItemElem->IsFlag( FLItemElem::reset_bind ) == TRUE )
							{
								SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
									g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DOUBLE_CLICK);
								g_WndMng.m_pWndEquipBindConfirm->SetInformation(pFocusItem, dwObjId);
								g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
							}
							else
							{
								// 탈착 명령일 경우, nPart는 실제 장착되어 있는 부분과 일치해야 하므로 프로퍼티에서 꺼내지 않는다.
								int nPart	= pItemProp->dwParts;

								BOOL bEquiped	= g_pPlayer->m_Inventory.IsEquip( pFocusItem->m_dwObjId );
								if( bEquiped )
									nPart	= pFocusItem->m_dwObjIndex;// - g_pPlayer->m_Inventory.m_dwIndexNum;
								
								if( !g_WndMng.CheckConfirm( pFocusItem ) )		//gmpbigsun: 패킷보내기전 확인등의 처리담당 
								{
									//장비장착을 위해 더블클릭한 경우도 여기서 보낸다.
									g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pFocusItem->m_dwObjId ), dwObjId, nPart );
								}
							}
						}
					}
				}
			}
		}
	}
	CWndButton* pWndButton		= (CWndButton*)*pLResult;
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
			case WIDC_CHECK1:
				pWndButton = (CWndButton*)GetDlgItem( nID );
				g_DPlay.SendOptionEnableRenderCostume( 0, pWndButton->GetCheck() );
				break;
			case WIDC_CHECK2:
				pWndButton = (CWndButton*)GetDlgItem( nID );
				g_DPlay.SendOptionEnableRenderCostume( 1, pWndButton->GetCheck() );
				break;
			case WIDC_CHECK3:;
				pWndButton = (CWndButton*)GetDlgItem( nID );
				g_DPlay.SendOptionEnableRenderCostume( 2, pWndButton->GetCheck() );
				break;
			case WIDC_CHECK4:
				pWndButton = (CWndButton*)GetDlgItem( nID );
				g_DPlay.SendOptionEnableRenderCostume( 3, pWndButton->GetCheck() );
				break;
			case WIDC_CHECK5:
				pWndButton = (CWndButton*)GetDlgItem( nID );
				g_DPlay.SendOptionEnableRenderCostume( 4, pWndButton->GetCheck() );
				break;
		}
	}
	else if( message == WIN_RCLK )
	{

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
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();
	
}
void CWndInventory::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM20 );
	if( lpWndCtrl->rect.PtInRect( point ) )
	{
		m_OldPos          = point;
		m_bLButtonDownRot = TRUE;
	}
		
	if( m_bLButtonDown )
	{
		for( int i=2; i<MAX_HUMAN_PARTS; i++ )
		{
			CRect DrawRect = m_InvenRect[i];
			
			if( DrawRect.PtInRect( point ) )
			{
				FLItemBase* pItemBase = NULL;

				pItemBase = g_pPlayer->GetEquipItem( i );

				if( pItemBase )
				{
					m_pSelectItem  = (FLItemElem*)pItemBase;
					return;
				}
			} 
		}
	}
#ifdef INVENTORY_ITEM_ALIGN
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl( WIDC_CUSTOM22 );
	if( lpWndCtrl2->rect.PtInRect( point ) )
	{
		g_DPlay.SendAlignItem(m_pTabCtrl->GetCurSel()+1);
	}
#endif	// INVENTORY_ITEM_ALIGN
	m_pSelectItem = NULL;
}

void CWndInventory::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	for( int i=2; i<MAX_HUMAN_PARTS; i++ )
	{
		CRect DrawRect = m_InvenRect[i];

		if( DrawRect.PtInRect( point ) )
		{
			FLItemElem* pItemElem = NULL;

			pItemElem = (FLItemElem*)g_pPlayer->GetEquipItem( i );
			
			if( pItemElem )
			{
				g_DPlay.SendDoEquip( pItemElem, i );		// 파츠번호를 같이 보낸다. 쌍칼땜시.
				// 장착인벤 영역을 더블클릭해서 벗기거나
				// 드래그&드롭으로 장착하려 하거나 벗기려고 할때 파츠번호를 보낸다.
			}
		}
	}
}

void CWndInventory::OnRButtonDown(UINT nFlags, CPoint point)
{
	BaseMouseCursor();
}

BOOL CWndInventory::OnSetCursor ( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	ChangeMouseCursor();
	return TRUE;
}

void CWndInventory::ChangeMouseCursor()
{
	if( m_pUnSealKeyItem )
	{
		SetMouseCursor( CUR_SELECT );
		return;
	}

	if( m_bIsUpgradeMode )			// 인벤창이 열려있고 인첸트 모드이면 커서모양 변경
	{
		SetMouseCursor( CUR_HAMMER );
		return;
	}

	SetMouseCursor( CUR_BASE );		
}


void CWndInventory::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDownRot )
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}
	
	m_OldPos = point;
	
	if( m_bLButtonDown && IsPush() )
	{
		if( m_bPickup == TRUE )
		{
			m_pSelectItem = NULL;
			return;
		}

		if( m_pSelectItem && m_pSelectItem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwType     = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId       = m_pSelectItem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_pTexture   = m_pSelectItem->GetTexture();
			m_GlobalShortcut.m_dwData     = (DWORD) m_pSelectItem;
			FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), m_pSelectItem->GetProp()->szName);
		}
	}
}
BOOL CWndInventory::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if(pWndFrame == NULL)
		return FALSE;
	
	if( g_WndMng.GetWndBase( APP_SHOP_ ) ||
		g_WndMng.GetWndBase( APP_BANK ) ||
		g_WndMng.GetWndBase( APP_TRADE )||
		g_WndMng.GetWndBase( APP_SHOP_CART_ ) )// _JIN_SHOP_CART
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
	else if( g_WndMng.GetWndBase( APP_INVENTORY ) )
	{
		if( pWndFrame->GetWndId() == APP_INVENTORY )
		{
			LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM21 );
			if( lpWndCtrl1->rect.PtInRect( point ) )
			{
				if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData ) // dwData가 0이면 골드
				{
					FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					if( !pItemElem )
						return FALSE;
					if(pItemElem->GetExtra() > 0)
						return FALSE;
					
					if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) ) 
						return FALSE;
			
					if( g_pPlayer->IsUsing( pItemElem ) )
						return FALSE;
					if( pItemElem->IsUndestructable() == TRUE )
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_ERROR_UNDESTRUCTABLE_ITEM ), NULL, prj.GetTextColor( TID_GAME_ERROR_UNDESTRUCTABLE_ITEM ) );
						return FALSE;
					}

					//sun, 11 케릭터 봉인 거래 기능 world,database,neuz
					if( pItemElem && pItemElem->m_dwItemId == ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER )  )
						return FALSE;

					SAFE_DELETE( g_WndMng.m_pWndInvenRemoveItem );
					g_WndMng.m_pWndInvenRemoveItem = new CWndInvenRemoveItem;
					g_WndMng.m_pWndInvenRemoveItem->Initialize( &g_WndMng, APP_INVEN_REMOVE_ITEM );
					g_WndMng.m_pWndInvenRemoveItem->InitItem( pItemElem );
					return TRUE;
				}				
			}
		}
	}
	
	if( pShortcut->m_dwShortcut == SHORTCUT_ITEM && pShortcut->m_pFromWnd != this && pWndFrame->GetWndId() == APP_INVENTORY )
	{
		if( pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData ) // dwData가 0이면 골드 
		{
			FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
			if( pItemElem && pItemElem->GetProp()->dwParts != NULL_ID ) //&& pItemElem->GetProp()->dwParts == i * 3 + j )
			{
				//인벤토리 테두리에 올려지는 아이템은 장착이 되지 않도록 수정.
				CRect rect;
				LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
				rect = lpWndCtrl->rect;
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
				rect.bottom = lpWndCtrl->rect.bottom;
				rect.right = lpWndCtrl->rect.right;

				if(rect.PtInRect( point ))
				{
					PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
					if( pItemProp != NULL )
					{
						//if( pItemElem->GetProp() && ( pItemElem->GetProp()->dwFlag & IP_FLAG_EQUIP_BIND ) && !pItemElem->IsFlag( FLItemElem::binds ) )
						if( pItemProp->nBindCondition == BIND_CONDITION_USE
							&& pItemProp->dwParts != NULL_ID
							&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
							&& pItemElem->IsFlag( FLItemElem::reset_bind ) == FALSE )
						{
							SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
								g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DRAG_AND_DROP);
							g_WndMng.m_pWndEquipBindConfirm->SetInformation(pItemElem);
							g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
						}
						else if( pItemProp->nResetBindCondition == BIND_CONDITION_USE
							&& pItemProp->dwParts != NULL_ID
							&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
							&& pItemElem->IsFlag( FLItemElem::reset_bind ) == TRUE )
						{
							SAFE_DELETE(g_WndMng.m_pWndEquipBindConfirm)
								g_WndMng.m_pWndEquipBindConfirm = new CWndEquipBindConfirm(CWndEquipBindConfirm::EQUIP_DRAG_AND_DROP);
							g_WndMng.m_pWndEquipBindConfirm->SetInformation(pItemElem);
							g_WndMng.m_pWndEquipBindConfirm->Initialize(NULL);
						}
						else
						{
							g_DPlay.SendDoEquip( pItemElem );
						}
					}
					
					return TRUE;
				}
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
void CWndInventory::UpdateTooltip( void )	
{
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
		{
			m_wndItemCtrl.UpdateTooltip();	
		}
		break;
	case CS_VER2:
		{
			for( int i = 0; i < MAX_VENDOR_NEW_INVENTAB; i++ )
			{
				m_wndItemCtrl_[i].UpdateTooltip();	
			}
		}
		break;
	default:
		FLASSERT(0);
		break;
	}
}

void CWndInventory::CISetDieFlag_ItemCtrl( const BOOL bFlag )
{
	switch(_GetContentState(CT_NEW_INVENTORY))
	{
	case CS_VER1:
			m_wndItemCtrl.SetDieFlag2( bFlag );
			break;

	case CS_VER2:
			for( int nTabCnt = 0; nTabCnt <= MAX_VENDOR_NEW_INVENTAB; ++nTabCnt )
				m_wndItemCtrl_[nTabCnt].SetDieFlag2( bFlag );

			break;
	default:
		FLASSERT(0);
		break;
	}
}

BOOL CWndInventory::ExcuteItem( FLItemBase* pItem )
{
	//SWORD : 더블클릭, 등록
	//처리가 된경우 return TRUE

	//경매장 ( 등록창 ) 이용중인가? 
	BOOL bRst = FALSE;

	BOOL bOK = g_WndMng.IsConsignmentMarket_Register( );
	if( TRUE == bOK )
	{
		bRst = g_WndMng.DoModalConsignmentMarket_Register( pItem->m_dwObjId );
		return bRst;
	}

	//양손무기 합성
	if( NULL != g_WndMng.m_pWndComposeTwoWeapon )
	{
		bRst = g_WndMng.m_pWndComposeTwoWeapon->RegItem( pItem->m_dwObjId );
		return bRst;
	}

	//외형변경
	if( NULL != g_WndMng.m_pWndChangeLook )
	{
		bRst = g_WndMng.m_pWndChangeLook->RegItem( pItem->m_dwObjId );
		return bRst;
	}
		
	return FALSE;
}

//sun: 13, 캐릭터정보창 리뉴얼
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 캐릭터 통합 정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndCharInfo::CWndCharInfo()
:	m_pWndChangeJob( NULL )
,	m_fWaitingConfirm( FALSE )
,	m_bExpert( FALSE )
,	m_nDisplay( -1 )
,	m_nStrCount( NULL )
,	m_nStaCount( NULL )
,	m_nDexCount( NULL )
,	m_nIntCount( NULL )
,	m_nGpPoint( NULL )
,	m_nATK( NULL )
,	m_nDEF( NULL )
,	m_nCritical( NULL )
,	m_nATKSpeed( NULL )
{
	PFONTCOLOR_WNDCHARINFO pFontColorCharInfo = g_WndFontColorManager->GetWndCharInfo(); 	

	m_dwPropertyNameColor			= pFontColorCharInfo->m_stPropertyNameColor.GetFontColor();
	m_dwPropertyNumberColor			= pFontColorCharInfo->m_stPropertyNumberColor.GetFontColor();
	m_dwIncreasePropertyNumberColor = pFontColorCharInfo->m_stIncreasePropertyNumberColor.GetFontColor();
	m_dwIncreaseAbilityNumberColor	= pFontColorCharInfo->m_stIncreaseAbilityNumberColor.GetFontColor();
	m_dwDecreaseAbilityNumberColor	= pFontColorCharInfo->m_stDecreaseAbilityNumberColor.GetFontColor();

}


CWndCharInfo::~CWndCharInfo()
{
	SAFE_DELETE(m_pWndChangeJob);
}

void CWndCharInfo::PaintFrame(C2DRender* p2DRender)
{
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		if( m_pTexture )
			RenderWnd();
	}
	
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		CWndBase::PaintFrame( p2DRender );

}

void CWndCharInfo::OnDraw(C2DRender* p2DRender)
{

	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int			y = 0, nNext = 15, nyAdd3 = 21;
//sun: 9차 전승관련 Clienet
	BYTE		checkhero = g_pPlayer->GetLegendChar();
	int			xpos = 0;
	int			ypos = 0;
	CTexture*	pTexture;
	CString		strPath;
	CPoint		point;

	y = 13;
	p2DRender->TextOut( 60, y, prj.GetText((TID_APP_CHARACTER_BASE)), m_dwPropertyNumberColor );
	y = 10 + nyAdd3;
	if( TRUE ) //g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , m_dwPropertyNumberColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , m_dwPropertyNumberColor ); y += nNext;
//sun: 9차 전승관련 Clienet
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}

		if(checkhero == LEGEND_CLASS_HERO)
		{
			xpos = 103;
		}
		else if( checkhero == LEGEND_CLASS_LEGEND_HERO )
		{
			xpos = 103;
		}
		p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , m_dwPropertyNumberColor ); y += nNext;

		y = 81+ nyAdd3;
	}
	else
	{
		p2DRender->TextOut( 50, y, g_pPlayer->GetName()       , m_dwPropertyNumberColor); y += nNext;
		p2DRender->TextOut( 50, y, g_pPlayer->GetJobString()  , m_dwPropertyNumberColor ); y += nNext;
//sun: 9차 전승관련 Clienet
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}

		if(checkhero == LEGEND_CLASS_HERO)
			xpos = 73;
		p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , m_dwPropertyNumberColor ); y += nNext;
	
		y = 81 + nyAdd3;
	}

//sun: 9차 전승관련 Clienet
	point.x = xpos;
	point.y = ypos - 2;
	if(checkhero == LEGEND_CLASS_MASTER) //전승을 했을 경우.
	{
		if(/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70) //Level Down될 경우를 생각해서 주석처리
			strPath = MakePath( DIR_ICON, "Icon_MasterMark1.dds");
		else if(g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark2.dds");
		else if(g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark3.dds");
		else if(g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark4.dds");
		else if(g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark5.dds");
		else if(g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );			
		
	}
	else if(checkhero == LEGEND_CLASS_HERO) //영웅일 경우.
	{
		strPath = MakePath( DIR_ICON, "Icon_HeroMark.dds");
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}
	else if( checkhero == LEGEND_CLASS_LEGEND_HERO )
	{
		strPath = MakePath( DIR_ICON, "Icon_HeroMark.dds");
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}

	const T_SERVER_DESC* pServerDesc = g_pServerDescription->FindByNth( NULL_ID, g_Option.m_nSer );
	if( pServerDesc != NULL )
	{
		y = 55 + nyAdd3;
		p2DRender->TextOut( 80, y, pServerDesc->szName, m_dwPropertyNumberColor );
		y += nNext;

		const T_SERVER_DESC* pChannelDesc = g_pServerDescription->FindByNth( pServerDesc->dwMyID, g_Option.m_nMSer );
		if( pChannelDesc != NULL )
		{
			p2DRender->TextOut( 80, y, pChannelDesc->szName, m_dwPropertyNumberColor );
		}
	}


	
//	이름
//	레벨
//	직업레벨
//	비행 레벨


	y = 10+ nyAdd3;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_01),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_APP_CHARACTER_JOB),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_02),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVER),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME),		m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 60, 113, prj.GetText((TID_APP_CHARACTER_DETAIL)),	m_dwPropertyNameColor );


	/////////////////////////// detail begin //////////////////////////////////
	int nyAdd = 121;
	int x = 5, nNextX = 100;

	x = 55; y = 10 + nyAdd;
	nNext = 15;
	// 공격력
	RenderATK( p2DRender, x, y );
	y += nNext;

	//방어력
	if(	m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense( FALSE ) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			p2DRender->TextOut( x , y, GetVirtualDEF(), m_dwIncreasePropertyNumberColor );
			y += nNext;
		}
		else
		{
			p2DRender->TextOut( x , y, GetVirtualDEF(), m_dwPropertyNumberColor );
			y += nNext;
		}
	}
	else
	{
		p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), m_dwPropertyNumberColor );
		y += nNext;
	}

#ifdef ADD_CHARACTER_INFO_DISPLAY
	CString strMsg( _T("") );
	//add move speed
	strMsg.Format( "%d%%", int( g_pPlayer->GetAdjParam( DST_SPEED ) ) );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;

	//HIt Rate
	strMsg.Format( "%d%%", g_pPlayer->GetAdjHitRate() );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;

	//Parrying
	strMsg.Format( "%d%%", g_pPlayer->GetParrying() );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;

	//Reflect Damage
	strMsg.Format( "%d%%", g_pPlayer->GetParam( DST_REFLECT_DAMAGE, 0 ) );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;
#endif
	x = 155; y = 10 + nyAdd;

	//크리티컬

	if(	m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			strMsg.Format( "%d%%", GetVirtualCritical() );
			p2DRender->TextOut( x , y, strMsg, m_dwIncreasePropertyNumberColor );
			y += nNext;
		}
		else
		{
			strMsg.Format( "%d%%", GetVirtualCritical() );
			p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor );
			y += nNext;
		}
	}
	else
	{
		strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
		p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor );
		y += nNext;
	}


	//공격속도	
	float fAttackSpeed;

	if(	m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			fAttackSpeed = GetVirtualATKSpeed();
			strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
			p2DRender->TextOut( x , y, strMsg, m_dwIncreasePropertyNumberColor ); y += nNext;
		}
		else
		{
			fAttackSpeed = GetVirtualATKSpeed();
			strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
			p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); y += nNext;
		}
	}
	else
	{
		fAttackSpeed = g_pPlayer->GetAttackSpeed();
		strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
		p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); y += nNext;
	}	

#ifdef ADD_CHARACTER_INFO_DISPLAY
	//magic spell velocity
	strMsg.Format( "%d%%", int(ceil( g_pPlayer->GetCastingAniSpeed() * 100.0f )) );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;

	//Melee Block
	strMsg.Format( "%d%%", g_pPlayer->GetParam( DST_BLOCK_MELEE, 0 ) );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;

	//Range Block
	strMsg.Format( "%d%%", g_pPlayer->GetParam( DST_BLOCK_RANGE, 0 ) );
	p2DRender->TextOut( x , y, strMsg, m_dwPropertyNumberColor ); 
	y += nNext;
#endif
	
	x =15; nNextX = 60;
	// 아래부터 능력치 관련 
#ifdef ADD_CHARACTER_INFO_DISPLAY
	y = 112 + nyAdd;
#else
	y = 52 + nyAdd;
#endif

	int StatYPos = 50;

	const int nOffsetX_Dice = 30;
	const int nOffsetX_GP   = 57;

	int nStr = std::max<int>( m_RenderStat.GetStr() + g_pPlayer->GetParam( DST_STR, 0 ), 1 );
	int nSta = std::max<int>( m_RenderStat.GetSta() + g_pPlayer->GetParam( DST_STA, 0 ), 1 );
	int nDex = std::max<int>( m_RenderStat.GetDex() + g_pPlayer->GetParam( DST_DEX, 0 ), 1 );
	int nInt = std::max<int>( m_RenderStat.GetInt() + g_pPlayer->GetParam( DST_INT, 0 ), 1 );

	if( m_RenderStat.GetOriginStr() == nStr )
	{
		p2DRender->TextOut( StatYPos, y, nStr, m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceStr(), m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginStr() - DEFAULT_ORIGIN_STAT_VALUE , m_dwPropertyNumberColor );
		y += nNext;

	}
	else if( m_RenderStat.GetOriginStr() < nStr )
	{
		p2DRender->TextOut( StatYPos, y, nStr, m_dwIncreaseAbilityNumberColor ); 
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceStr(), m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginStr() - DEFAULT_ORIGIN_STAT_VALUE , m_dwIncreaseAbilityNumberColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( StatYPos, y, nStr, m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceStr(), m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginStr() - DEFAULT_ORIGIN_STAT_VALUE , m_dwDecreaseAbilityNumberColor );
		y += nNext;
	}

	if( m_RenderStat.GetOriginSta() == nSta )
	{	
		p2DRender->TextOut( StatYPos, y, nSta, m_dwPropertyNumberColor ); 
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceSta(), m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginSta() - DEFAULT_ORIGIN_STAT_VALUE , m_dwPropertyNumberColor );
		y += nNext;
	}
	else if( m_RenderStat.GetOriginSta() < nSta )
	{
		p2DRender->TextOut( StatYPos, y, nSta, m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceSta(), m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginSta() - DEFAULT_ORIGIN_STAT_VALUE , m_dwIncreaseAbilityNumberColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( StatYPos, y, nSta, m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceSta(), m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginSta() - DEFAULT_ORIGIN_STAT_VALUE , m_dwDecreaseAbilityNumberColor );
		y += nNext;
	}

	if( m_RenderStat.GetOriginDex() == nDex )
	{
		p2DRender->TextOut( StatYPos, y, nDex, m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceDex(), m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginDex() - DEFAULT_ORIGIN_STAT_VALUE , m_dwPropertyNumberColor );
		y += nNext;
	}
	else if( m_RenderStat.GetOriginDex() < nDex )
	{
		p2DRender->TextOut( StatYPos, y, nDex, m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceDex(), m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginDex() - DEFAULT_ORIGIN_STAT_VALUE , m_dwIncreaseAbilityNumberColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( StatYPos, y, nDex, m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceDex(), m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginDex() - DEFAULT_ORIGIN_STAT_VALUE , m_dwDecreaseAbilityNumberColor );
		y += nNext;
	}

	if( m_RenderStat.GetOriginInt() == nInt )
	{
		p2DRender->TextOut( StatYPos, y, nInt, m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceInt(), m_dwPropertyNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginInt() - DEFAULT_ORIGIN_STAT_VALUE , m_dwPropertyNumberColor );
		y += nNext;
	}
	else if( m_RenderStat.GetOriginInt() < nInt )
	{
		p2DRender->TextOut( StatYPos, y, nInt, m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceInt(), m_dwIncreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginInt() - DEFAULT_ORIGIN_STAT_VALUE , m_dwIncreaseAbilityNumberColor );
		y += nNext;
	}
	else
	{
		p2DRender->TextOut( StatYPos, y, nInt, m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_Dice, y, m_RenderStat.GetDiceInt(), m_dwDecreaseAbilityNumberColor );
		p2DRender->TextOut( StatYPos + nOffsetX_GP, y, m_RenderStat.GetOriginInt() - DEFAULT_ORIGIN_STAT_VALUE , m_dwDecreaseAbilityNumberColor );
		y += nNext;
	}

	if(m_nGpPoint)
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			p2DRender->TextOut( 105, y, m_nGpPoint, m_dwIncreasePropertyNumberColor );
			y += nNext;
		}
		else
		{
			p2DRender->TextOut( 105, y, m_nGpPoint, m_dwIncreasePropertyNumberColor );
			y += nNext;
		}
	}
	else
		p2DRender->TextOut( 105, y, m_nGpPoint, m_dwPropertyNumberColor ); y += nNext;


	CRect rectHittest[5];

#ifdef ADD_CHARACTER_INFO_DISPLAY
	rectHittest[0].SetRect( 10, 112+ nyAdd, 80, 125 + nyAdd);
	rectHittest[1].SetRect( 10, 127+ nyAdd, 80, 140 + nyAdd);
	rectHittest[2].SetRect( 10, 142+ nyAdd, 80, 155 + nyAdd);
	rectHittest[3].SetRect( 10, 157+ nyAdd, 80, 170 + nyAdd);
	rectHittest[4].SetRect( 10, 172+ nyAdd, 160, 185 + nyAdd);
#else
	rectHittest[0].SetRect( 10, 52+ nyAdd, 80, 65 + nyAdd);
	rectHittest[1].SetRect( 10, 67+ nyAdd, 80, 80 + nyAdd);
	rectHittest[2].SetRect( 10, 82+ nyAdd, 80, 95 + nyAdd);
	rectHittest[3].SetRect( 10, 97+ nyAdd, 80, 110 + nyAdd);
	rectHittest[4].SetRect( 10, 112+ nyAdd, 160, 125 + nyAdd);
#endif


	CRect rectTemp;
	CPoint ptTemp;
	// 툴팁 나오게 하기( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );
			DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else // GP
				strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_GPPOINT ) );
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10 + nyAdd;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13),	m_dwPropertyNameColor ); 
	p2DRender->TextOut( 90, y, prj.GetText(TID_GAME_CHARACTER_14),	m_dwPropertyNameColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE),		m_dwPropertyNameColor ); 
	p2DRender->TextOut( 90, y, prj.GetText(TID_GAME_CHARACTER_15),	m_dwPropertyNameColor ); y += nNext;

#ifdef ADD_CHARACTER_INFO_DISPLAY
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_SPEED),		m_dwPropertyNameColor ); 
	p2DRender->TextOut( 90, y, prj.GetText(TID_TOOLTIP_DST_SPELL_RATE01),	m_dwPropertyNameColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_HITRATE),		m_dwPropertyNameColor ); 
	p2DRender->TextOut( 90, y, prj.GetText(TID_GAME_TOOLTIP_BLOCK01),	m_dwPropertyNameColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_PARRY),		m_dwPropertyNameColor ); 
	p2DRender->TextOut( 90, y, prj.GetText(TID_GAME_TOOLTIP_BLOCKRANGE01),	m_dwPropertyNameColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DST_REFLECT_DAMAGE01),		m_dwPropertyNameColor ); y += nNext;
#endif

	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT),			m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07),	m_dwPropertyNameColor ); y += nNext;
	
	//능력치 상태 Tooltip
#ifdef ADD_CHARACTER_INFO_DISPLAY
	rect.SetRect( 7, 70+ nyAdd, 160, 98 + nyAdd);
#else
	rect.SetRect( 7, 10+ nyAdd, 160, 38 + nyAdd);
#endif
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS1 ) );
		else
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS2 ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
	//Edit Tooltip
#ifdef ADD_CHARACTER_INFO_DISPLAY
	rect.SetRect(90, 112+ nyAdd, 160, 170+ nyAdd);
#else
	rect.SetRect(90, 52+ nyAdd, 160, 110+ nyAdd);
#endif
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_EDIT ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_APPLY ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	rect = m_wndReset.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_RESET ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}

	//////////////// pvp /////////////////////////
	
	if( g_pPlayer->IsBaseJob() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}
	else if( g_pPlayer->IsExpert() )
	{
		if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL + MAX_EXP_LEVEL )
			m_wndChangeJob.EnableWindow( TRUE );
		else
			m_wndChangeJob.EnableWindow( FALSE );
	}

	//CRect rect = GetClientRect();
	//rect.bottom -= 30;
#ifdef ADD_CHARACTER_INFO_DISPLAY
	int nyAdd2 = 344;
#else
	int nyAdd2 = 284;
#endif

	y = 15 + nyAdd2, nNext = 15;
	char szBuff[32];
	int gap1 = 0;
	int gap2 = 0;
	gap1 -= 10;
	gap2 -= 10;

#ifdef ADD_CHARACTER_INFO_DISPLAY
	p2DRender->TextOut( 60, 341, prj.GetText((TID_GAME_CHARACTTER_PVP0)), m_dwPropertyNumberColor );
#else
p2DRender->TextOut( 60, 281, prj.GetText((TID_GAME_CHARACTTER_PVP0)), m_dwPropertyNumberColor );
#endif

	FLStrcpy( szBuff, _countof( szBuff ), g_pPlayer->GetFameName() );
	if( IsEmpty(szBuff) ) {	szBuff[0] = '-'; szBuff[1] = NULL; }
	p2DRender->TextOut( 100+gap1, y, szBuff  , m_dwPropertyNumberColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->m_nFame	, m_dwPropertyNumberColor ); y += nNext;
//sun: 8, // __S8_PK

#ifdef ADD_CHARACTER_INFO_DISPLAY
	y += 4;
#else
	y += 20;
#endif

	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetPKValue()	, m_dwPropertyNumberColor ); y += nNext;
	y += 4;
	p2DRender->TextOut( 100+gap2, y, g_pPlayer->GetPKPropensity()	, m_dwPropertyNumberColor ); y += nNext;
	
	y = 13 + nyAdd2;
	nNext = 19;
//sun: 8, // __S8_PK
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP1), m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP2), m_dwPropertyNameColor ); y += nNext;
#ifdef ADD_CHARACTER_INFO_DISPLAY
	y += 4;
#else
	y += 20;
#endif

	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP3), m_dwPropertyNameColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTTER_PVP4), m_dwPropertyNameColor ); y += nNext;
}


void CWndCharInfo::OnInitialUpdate()
{
 	CWndBase::OnInitialUpdate();
	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "WndNewCharacter01.tga" ) ), TRUE );

	// 아래부터 능력치 관련 
#ifdef ADD_CHARACTER_INFO_DISPLAY
	int nyAdd = 181;
#else
	int nyAdd = 121;
#endif

	int posY = 49 + nyAdd;
	int posX = 165;

	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 30, posY, posX + 4, posY + 16 ), this, 100 );
	m_wndStrPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 101 );
	m_wndStrMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 30, posY, posX + 4, posY + 16 ), this, 103 );
	m_wndStaPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 104 );
	m_wndStaMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 30, posY, posX + 4, posY + 16 ), this, 106 );
	m_wndDexPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 107 );
	m_wndDexMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 30, posY, posX + 4, posY + 16 ), this, 109 );
	m_wndIntPlus.Create  ( "", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 110 );
	m_wndIntMinus.Create  ( "", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 111 );

	m_wndStrPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStrMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndStaPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStaMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndDexPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDexMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndIntPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndIntMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharMinus.bmp" ) ), TRUE );

	posY += 36;

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_wndApply.Create  ( _T("Apply"), 0, CRect( posX - 108, posY, posX - 58, posY + 22 ), this, 112 );
		m_wndReset.Create  ( _T("Reset"), 0, CRect( posX - 30, posY, posX + 20, posY + 22 ), this, 113 );

		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
		{
			m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButStateOk.tga" ) ), TRUE );
			m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButStateCancel.tga" ) ), TRUE );
		}
		else
		{
			m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharApply.tga" ) ), TRUE );
			m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharReset.tga" ) ), TRUE );
		}
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_wndApply.Create  ( prj.GetText( TID_MMI_CHARACTERINFO_BUTTON01 ), 0, CRect( posX - 108, posY - 2, posX - 58, posY + 22 ), this, 112 );
		m_wndReset.Create  ( prj.GetText( TID_MMI_CHARACTERINFO_BUTTON02 ), 0, CRect( posX - 30, posY - 2, posX + 20, posY + 22 ), this, 113 );

		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtNormal05.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtNormal05.tga" ) ), TRUE );
		m_wndApply.FitTextureSize();
		m_wndReset.FitTextureSize();
	}

 	m_nGpPoint = g_pPlayer->GetRemainGP();
	//스탯 증가량 한적 없으므로 Minus Button Default는 False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if(g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}
	int nyAdd2 = 280;
	int y = 105 + nyAdd2;
	if( g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
		m_wndChangeJob.Create( ">", 0, CRect( 130, y, 135+40, y + 13 ), this, 10  ); 

	RefreshStatPoint();
	MakeVertexBuffer();
}


BOOL CWndCharInfo::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->GetWindowRect();
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);

}


BOOL CWndCharInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}


BOOL CWndCharInfo::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	int editnum = 0;
	
	if( nID == 100 || nID == 103 || nID == 106 || nID == 109 )
	{
		CString tempnum = m_editStrCount.GetString();
		for( int i = 0 ; i < tempnum.GetLength() ; i++ )
		{
			if( isdigit2( tempnum.GetAt(i) ) == FALSE )
				return FALSE;
		}
	}
	switch(nID) 
	{
		case 100:
			editnum = atoi( m_editStrCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi( m_editStaCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi( m_editDexCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi( m_editIntCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if( message == WNM_CLICKED )		
	{
		switch(nID) 
		{
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				{
					SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;

			case JOB_MERCENARY: 
			case JOB_ACROBAT: 
			case JOB_ASSIST: 
			case JOB_MAGICIAN: 
			case JOB_PUPPETEER:
				{
					if( nID != g_pPlayer->GetJob() )
					{
						//"자신의 직업만 올릴수 있습니다"
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0037) ) );
					}
					if( m_fWaitingConfirm == FALSE ) 
					{
						g_DPlay.SendIncJobLevel( nID );
						m_fWaitingConfirm = TRUE;
					}
					break;
				}
			case 10: // 전직 
				// 방랑자의 레벨이 15이상인것을 찾음
				if( g_pPlayer->GetLevel() >= MAX_JOB_LEVEL )
				{
					SAFE_DELETE(m_pWndChangeJob);
					m_pWndChangeJob = new CWndChangeJob( g_pPlayer->GetJob() ); 
					m_pWndChangeJob->Initialize( this, 1106 );
				}
				break;
		}
	}

	if(nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if(g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if(g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}

	return CWndBase::OnChildNotify( message, nID, pLResult );
}


void CWndCharInfo::OnSize(UINT nType, int cx, int cy)
{

}


void CWndCharInfo::OnLButtonUp(UINT nFlags, CPoint point)
{

}


void CWndCharInfo::OnLButtonDown(UINT nFlags, CPoint point)
{

}


void CWndCharInfo::OnMouseWndSurface( CPoint point )
{

}


BOOL CWndCharInfo::Process()
{
	//Control Button Plus or Minus
	if(m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if(m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if(m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if(m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if(m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);
		
		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
		
		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if(m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return CWndBase::Process();
}


void CWndCharInfo::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}


void CWndCharInfo::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;
	
	*pnMin = 0;
	*pnMax = 0;
	
	if( g_pPlayer )
	{
		PT_ITEM_SPEC pItemProp = g_pPlayer->GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;
		
		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;
		
		*pnMin = g_pPlayer->GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = g_pPlayer->GetParam( DST_ABILITY_MAX, *pnMax );
		
		int nATK = 0;
		switch( pItemProp->dwWeaponType )
		{
			case WT_MELEE_SWD:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_AXE:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MELEE_STAFF:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_STICK:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)) );
				break;
			case WT_MELEE_KNUCKLE:
				nATK = (int)( float( (g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MAGIC_WAND:
				nATK = (int)( ( g_pPlayer->GetInt() + m_nIntCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f );
				break;
			case WT_MELEE_YOYO:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_RANGE_BOW:
				nATK = (int)( (((g_pPlayer->GetDex() + m_nDexCount -14)*4.0f + (g_pPlayer->GetLevel()*1.3f) + ((g_pPlayer->GetStr()+m_nStrCount)*0.2f)) * 0.7f) );
				break;
		}
		
		nATK += g_pPlayer->GetPlusWeaponATK( pItemProp->dwWeaponType );	// 무기의 추가 공격력를 구한다.

		int nPlus = nATK + g_pPlayer->GetParam( DST_CHR_DMG, 0 );

		*pnMin += nPlus;
		*pnMax += nPlus;
		
		FLItemElem *pWeapon = g_pPlayer->GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = g_pPlayer->GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}
		
		if( pWeapon )
		{
			int nOption = pWeapon->GetAbilityOption();
			if( nOption > 0 )
			{
				int nValue = (int)pow( (float)( nOption ), 1.5f );

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}


int CWndCharInfo::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info  = { 0 };
	//mem_set( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;
	
	float fFactor = 1.0f;
	if( g_pPlayer )
	{
		JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
		FLASSERT( pProperty );
		fFactor = pProperty->fFactorDef;
	}
	nDefense	= (int)( ((((g_pPlayer->GetLevel()*2) + ((g_pPlayer->GetSta()+m_nStaCount)/2)) / 2.8f ) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor) );
	nDefense	= nDefense + (g_pPlayer->GetDefenseByItem( FALSE ) / 4);
	nDefense	= nDefense + ( g_pPlayer->GetParam( DST_ADJDEF, 0 ) );

	nDefense	= (int)( nDefense * g_pPlayer->GetDEFMultiplier( &info ) );

	if( nDefense < 0 )
		nDefense	= 0;

	return nDefense;
}


int CWndCharInfo::GetVirtualCritical()
{
	int nCritical;
	nCritical	= ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical	= (int)( nCritical * g_pPlayer->GetJobPropFactor( JOB_PROP_CRITICAL ) );
	nCritical	= g_pPlayer->GetParam( DST_CHR_CHANCECRITICAL, nCritical );	// 크리티컬 확률을 높여주는 스킬관련 
	if( nCritical < 0 )
		nCritical	= 0;
	
	if( g_pPlayer )
	{
		if( g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL) )	// 파티 && 1회 크리티컬 발동?
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				nCritical += (g_Party.m_nSizeofMember / 2);	
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}
	}
	return nCritical;
}


float CWndCharInfo::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;
	
	PT_ITEM_SPEC pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;
	
	JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
	FLASSERT( pProperty );
	
	// A = int( 캐릭터의 공속 + ( 무기의 공속 * ( 4 * 덱스 + ( 레벨 / 8 ) ) ) - 3 )
	// 공격속도 = ( ( 50 / 200 - A ) / 2 ) + 가중치 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f ) ) - 3.0f );

	if( 187.5f <= A )
		A	= (int)( 187.5f );

	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;
	
	int nIndex = A / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );
	
	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};
	
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + fPlusValue[nIndex];
	
	float fDstParam = g_pPlayer->GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( 0 < g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) )
		fSpeed = fSpeed + ( fSpeed * g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) / 100 );
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	
	return fSpeed;
}


void CWndCharInfo::RenderATK( C2DRender* p2DRender, int x, int y )
{
	char	szText[32] = { NULL, };

	int		nMin( NULL );
	int		nMax( NULL );
	int		nTemp1( NULL );
	int		nTemp2( NULL );

	DWORD	dwColor( m_dwPropertyNumberColor );

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax( &nMin, &nMax );

	if(	(m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = m_dwIncreasePropertyNumberColor;
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax( &nMin, &nMax );
	
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK += g_pPlayer->GetParam( DST_ATKPOWER, 0 );

	if( _GetContentState( CT_COMPOSE_TWOWEAPON19 ) == CS_VER1 )
	{
		FLItemElem* pWeapon	= g_pPlayer->GetWeaponItem();
		if( pWeapon != NULL )
		{
			nATK += pWeapon->GetCombinedAddDamage();
		}
	}

	if( nATK < 0 )
		nATK	= 0;

	FLSPrintf( szText, _countof( szText ), "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor );
}


CWndHonor::CWndHonor()
{
	m_nSelectedId = 0;
	m_vecTitle.clear();	
}

CWndHonor::~CWndHonor()
{

}

void CWndHonor::OnDraw( C2DRender* p2DRender )
{
//sun: 13, 달인
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	LPWNDCTRL	pCustom = NULL;
	int			nIndex = 0;
	pCustom = GetWndCtrl( WIDC_LISTBOX1 );

	if(m_vecTitle.empty() != false)
		p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*16,prj.GetText(TID_GAME_NO_TITLE), m_dwColor );
}


void CWndHonor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// 타이틀을 검색해서 리스트에 추가하고, 없으면 없다는 메시지 출력
	RefreshList();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);
}

void CWndHonor::RefreshList()
{
//sun: 13, 달인
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->ResetContent();
	pWndButton1->EnableWindow(FALSE);

	m_vecTitle.clear();
	m_vecTitle	= CTitleManager::Instance()->m_vecEarned;

	if(g_pPlayer)
		m_nSelectedId = g_pPlayer->m_nHonor;

	if(m_vecTitle.size() > 0)
	{
		std::vector<EarnedTitle>::iterator	iter;
		pWndListBox->AddString(prj.GetText(TID_GAME_NOT_SELECTED_TITLE));
		for(iter = m_vecTitle.begin(); iter != m_vecTitle.end(); ++iter)
		{
			//	100805	CSTRING GETBUFFER
			pWndListBox->AddString(iter->strTitle.GetString());
		}
	}
}


BOOL CWndHonor::Initialize( CWndBase* pWndParent,DWORD dwWndId )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HONOR, 0, CPoint( 0, 0 ), pWndParent );
}


BOOL CWndHonor::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{

	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}


BOOL CWndHonor::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
//sun: 13, 달인
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	switch(nID)
	{
	case WIDC_LISTBOX1: // view ctrl
		{
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			if(pWndListBox->GetCurSel() == 0)
			{
				m_nSelectedId = -1;
				pWndButton1->EnableWindow(TRUE);
			}
			else
			{
				m_nSelectedId = m_vecTitle[pWndListBox->GetCurSel() - 1].nId;
				pWndButton1->EnableWindow(TRUE);
			}
			break;
		}
	case WIDC_BUTTON1:// 선택된 타이틀을 사용하게 한다
		if(g_pPlayer)
		{
			pWndButton1->EnableWindow(FALSE);
			g_DPlay.SendReqHonorTitleChange(m_nSelectedId);
			break;
		}
	};

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


void CWndHonor::OnSize(UINT nType, int cx, int cy)
{

	CWndNeuz::OnSize(nType,cx,cy);
}


void CWndHonor::OnLButtonUp( UINT nFlags, CPoint point )
{


}


void CWndHonor::OnLButtonDown( UINT nFlags, CPoint point )
{


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
//sun: 13, 캐릭터정보창 리뉴얼
		if(nCurSel > 1) 
			nCurSel = 0;

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

	//CRect rectRoot = m_pWndRoot->GetLayoutRect();
	//CPoint point( rectRoot.left, rectRoot.top + 96 + 16 );
	//Move( point );
	
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		lpTapCtrl->SetTabTitleAlign( CWndTabCtrl::ALIGN_CENTER );

	//m_wndTabCtrl.Create( WBS_NOFRAME, rect, this, 12 );
//sun: 13, 캐릭터정보창 리뉴얼
	m_wndCharInfo.Create(WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, 100000);
	m_wndHonor.Create(WBS_CHILD | WBS_NOFRAME , rect, lpTapCtrl, APP_HONOR);
	m_wndHonor.AddWndStyle( WBS_NOFRAME );
	m_wndHonor.AddWndStyle( WBS_NODRAWFRAME );
	m_wndCharInfo.AddWndStyle( WBS_NOFRAME );
	//m_wndCharInfo.AddWndStyle( WBS_NODRAWFRAME );
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_CHAR_INFO);//"기본";
	tabTabItem.pWndBase = &m_wndCharInfo;
	lpTapCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_HONOR);//"달인";
	tabTabItem.pWndBase = &m_wndHonor;
	lpTapCtrl->InsertItem( 1, &tabTabItem );
	lpTapCtrl->SetCurSel( 0 );

	m_wndCharInfo.m_RenderStat = g_pPlayer->m_Stat;

	//lpTapCtrl->SetButtonLength( 60 );
}

BOOL CWndCharacter::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	//CRect rect( 240, 0, 240 + 260, 255 - 105 + 20 ); 
	CRect rect( 0, 90, WND_WIDTH, 260 );//255 - 105 + 20 ); 
	//CRect rect( 50, 50, 50+210, 50+300 );
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	//return CWndNeuz::Create(WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(0, 0), pWndParent );
}
BOOL CWndCharacter::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{ 
	if( message == WNM_CLICKED )
	{
	/*f( nID == WTBID_EXTENSION )
		{
			CRect rect = GetWindowRect( TRUE ); 
			if( m_wndTitleBar.GetTitleBarButton( WTBID_EXTENSION )->GetCheck() )
				rect.bottom += 135ww
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

//sun: 9차 상태창 변경 관련 Neuz
	strMessage.Format( prj.GetText( TID_GAME_CHARSTATUS_APPLY_Q ) );

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
//sun: 13, 캐릭터정보창 리뉴얼
	if( nID == WIDC_YES )
	{
		CWndCharacter* pWndBase = (CWndCharacter*)g_WndMng.GetWndBase( APP_CHARACTER3 );
		if( pWndBase )
		{
			CWndCharInfo* pInfo = &pWndBase->m_wndCharInfo;
			if( pInfo )
			{
				g_DPlay.SendModifyStatus(pInfo->m_nStrCount, pInfo->m_nStaCount, pInfo->m_nDexCount, pInfo->m_nIntCount);
				pInfo->m_nStrCount = pInfo->m_nStaCount = pInfo->m_nDexCount = pInfo->m_nIntCount = 0;
				pInfo->RefreshStatPoint();
				g_WndMng.m_pWndWorld->GetAdvMgr()->DeleteButton(APP_CHARACTER3);
				Destroy();
			}
		}
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		CWndBase* pWndBase	= g_WndMng.GetWndBase( APP_CHARACTER3 );
		if( pWndBase ) 
		{
			( (CWndCharacter*)pWndBase )->m_wndCharInfo.m_fWaitingConfirm	= FALSE;
		}
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndStateConfirm::SendYes( void )
{
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
//sun: 9차 전승관련 Clienet
	BYTE checkhero = g_pPlayer->GetLegendChar();
	int xpos = 0;
	int ypos = 0;
	CTexture* pTexture;
	CString strPath;
	CPoint point;

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

	if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
	{
		p2DRender->TextOut( 80, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 80, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
//sun: 9차 전승관련 Clienet
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}

		p2DRender->TextOut( 80, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;

		y = 81;
	}
	else
	{
		p2DRender->TextOut( 50, y, g_pPlayer->GetName()       , dwColor); y += nNext;
		p2DRender->TextOut( 50, y, g_pPlayer->GetJobString()  , dwColor ); y += nNext;
//sun: 9차 전승관련 Clienet
		ypos = y;
		if(checkhero == LEGEND_CLASS_MASTER)
		{
			if(g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
		p2DRender->TextOut( 50, y, g_pPlayer->GetLevel()      , dwColor ); y += nNext;

		y = 81;
	}

//sun: 9차 전승관련 Clienet
	point.x = xpos;
	point.y = ypos - 2;
	if(checkhero == LEGEND_CLASS_MASTER) //전승을 했을 경우.
	{
		if(/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70) //Level Down될 경우를 생각해서 주석처리
			strPath = MakePath( DIR_ICON, "Icon_MasterMark1.dds");
		else if(g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark2.dds");
		else if(g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark3.dds");
		else if(g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark4.dds");
		else if(g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark5.dds");
		else if(g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath( DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );			
		
	}
	else if(checkhero == LEGEND_CLASS_HERO) //영웅일 경우.
	{
		// 15차 히어로 레벨확장
		strPath = MakePath( DIR_ICON, "Icon_Hero.dds");
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, point );
	}

	// 서버 정보
	y = 55;
	const T_SERVER_DESC* pServerDesc = g_pServerDescription->FindByNth( NULL_ID, g_Option.m_nSer );
	if( pServerDesc != NULL )
	{
		if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
		{
			p2DRender->TextOut( 80, y, pServerDesc->szName, dwColor );
			y += nNext;
		}
		else
		{
			p2DRender->TextOut( 50, y, pServerDesc->szName, dwColor );
			y += nNext;
		}

		// 채널 정보
		const T_SERVER_DESC* pChannelDesc = g_pServerDescription->FindByNth( pServerDesc->dwMyID, g_Option.m_nMSer );
		if( pChannelDesc != NULL )
		{
			if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
			{
				p2DRender->TextOut( 80, y, pChannelDesc->szName, dwColor );
			}
			else
			{
				p2DRender->TextOut( 50, y, pChannelDesc->szName, dwColor );
			}
		}
	}

	y = 96;
	
	p2DRender->TextOut( 96, y, g_pPlayer->GetFlightLv(), dwColor ); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[ g_pPlayer->GetFlightLv() + 1 ].dwFxp;
		FLSPrintf( szBuff, _countof( szBuff ), "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp );
		p2DRender->TextOut( 90, y, szBuff, dwColor ); y += nNext;
	}
	
	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor ); y += nNext;
	//p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_03), dwColor ); y += nNext;
	//직업레벨 -> 서버/채널 정보
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor ); y += nNext;
	
	y += 10;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_04), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_05), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_06), dwColor );	
}
void CWndCharacterBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

//sun: 9차 상태창 변경 관련 Neuz
	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "WndCharacter2_1.tga" ) ), TRUE );

	FitTextureSize();
	
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

	m_nCnt = 0;
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

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK	+= g_pPlayer->GetParam( DST_ATKPOWER, 0 );

	if( nATK < 0 )
		nATK	= 0;

	FLSPrintf( szText, _countof( szText ), "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail::OnDraw(C2DRender* p2DRender)
{
	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	RenderATK( p2DRender, x, y );		// 공격력
	y += nNext;

	p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext; //방어력
	x = 100; y = 5;

	x = 130; y = 10;
	CString strMsg;
	strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;		// 크리티컬
	
	float fAttackSpeed = g_pPlayer->GetAttackSpeed();
	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	x =15; y = 40; nNextX = 60;
	// 아래부터 능력치 관련 
	y = 52;

	if( g_pPlayer->m_Stat.GetOriginStr() == g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginStr() < g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_Stat.GetOriginSta() == g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginSta() < g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_Stat.GetOriginDex() == g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginDex() < g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_Stat.GetOriginInt() == g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginInt() < g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( 65, y, g_pPlayer->GetInt(), dwColor ); y += nNext;

//sun: 10, __S_ADD_RESTATE
	if( m_fWaitingConfirm  == FALSE )
	{
		m_wndStr.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndSta.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndDex.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
		m_wndInt.EnableWindow( ( (int)g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	}
	else
	{
		m_wndStr.EnableWindow( FALSE );
		m_wndSta.EnableWindow( FALSE );
		m_wndDex.EnableWindow( FALSE );
		m_wndInt.EnableWindow( FALSE );
	}
	y = 52;
//sun: 10, __S_ADD_RESTATE
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, 1, dwColor ); y += nNext;

	//
	// m_nCnt같은 글로벌성 카운터는 g_nRenderCnt나 g_nProcessCnt를 쓰면 됩니다.
	// g_nProcessCnt는 매 프로세스(1/60)마다 한번씩 올라가는 카운트고
	// g_nRenderCnt는 매 렌더화면마다 한번씩 카운팅 되는 값입니다.  -xuzhu-
	// 
	if( g_pPlayer->GetRemainGP() )
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
			m_nCnt = 0;
		}
	}

	p2DRender->TextOut( 105, y, g_pPlayer->GetRemainGP(), dwColor ); y += nNext;

	CRect rectHittest[5];
	rectHittest[0].SetRect( 10, 52, 160, 65 );
	rectHittest[1].SetRect( 10, 67, 160, 80 );
	rectHittest[2].SetRect( 10, 82, 160, 95 );
	rectHittest[3].SetRect( 10, 97, 160, 110 );
	rectHittest[4].SetRect( 10, 112, 160, 125 );

	CRect rectTemp;
	CPoint ptTemp;
	// 툴팁 나오게 하기( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );
			DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else	// GP
			{
				szString.Format( "GP  %d\n ", g_pPlayer->GetRemainGP() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_GP0 ) );
			}
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;

	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;	
}
void CWndCharacterDetail::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "wndCharacter2.tga" ) ), TRUE );
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

	m_wndStr.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndSta.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDex.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndInt.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );

//sun: 10, __S_ADD_RESTATE
	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() > 0 ) ? TRUE : FALSE );
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
		if( m_fWaitingConfirm == FALSE ) {
			m_fWaitingConfirm	= TRUE;
			SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
			g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
			g_WndMng.m_pWndStateConfirm->OnSetState( nID );
			g_WndMng.m_pWndStateConfirm->Initialize();
		}
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
void CWndCharacterDetail::OnMouseWndSurface( CPoint point )
{
	// 직업 아이콘 위치에 커서를 가져가면 접속한 서버의 정보(서버이름과 멀티서버 번호)를 보여준다.
/*	CRect rectHittest( 10, 52, 100, 64 );	
	if( rectHittest.PtInRect( point ) )
	{
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );
		CString szEditString;
		szEditString.Format( "%d, %d", point.x, point.y );
		g_toolTip.PutToolTip( 100, szEditString, rectHittest, point2 );
		g_WndMng.PutString( szEditString );
//		g_toolTip.PutToolTip( (DWORD)this, m_strTooltip, rectHittest, point2, 0 );
	}
*/}
	
//////////////////////////////////////////////////////////////////////////
// CWndCharacterDetail2 Class
//////////////////////////////////////////////////////////////////////////
	
//sun: 9차 상태창 변경 관련 Neuz
	
CWndCharacterDetail2::CWndCharacterDetail2()
{
	m_nStrCount = 0;
	m_nStaCount = 0;
	m_nDexCount = 0;
	m_nIntCount = 0;
	m_nGpPoint = 0;

	m_nATK = 0;
	m_nDEF = 0;
	m_nCritical = 0;
	m_nATKSpeed = 0;
}

CWndCharacterDetail2::~CWndCharacterDetail2()
{
}

void CWndCharacterDetail2::RenderATK( C2DRender* p2DRender, int x, int y )
{
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);
	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax( &nMin, &nMax );

	if(	(m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax( &nMin, &nMax );
	
	int nATK = ( nMin + nMax ) / 2;

	if( g_pPlayer->IsSMMode( SM_ATTACK_UP1 ) || g_pPlayer->IsSMMode( SM_ATTACK_UP ) )
		nATK	= (int)( nATK * 1.2f );
	
	if( 0 < g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) )
		nATK = nATK + ( nATK * g_pPlayer->GetParam( DST_ATKPOWER_RATE, 0 ) / 100 );

	nATK += g_pPlayer->GetWeaponPlusDamage( 1, FALSE );
	nATK += g_pPlayer->GetParam( DST_ATKPOWER, 0 );

	if( nATK < 0 )
		nATK	= 0;

	FLSPrintf( szText, _countof( szText ), "%d", nATK );

	p2DRender->TextOut( x, y, szText, dwColor ); 
}

void CWndCharacterDetail2::OnDraw(C2DRender* p2DRender)
{
	if(g_pPlayer == NULL)
		return;

	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255,0,0,0);

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	// 공격력
	RenderATK( p2DRender, x, y );
	y += nNext;

	//방어력
	if(	m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense( FALSE ) ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		p2DRender->TextOut( x , y, GetVirtualDEF(), dwColor ); y += nNext;
	}
	else
		p2DRender->TextOut( x , y, g_pPlayer->GetShowDefense( FALSE ), dwColor ); y += nNext;

	x = 130; y = 10;

	//크리티컬
	CString strMsg;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	if(	m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		strMsg.Format( "%d%%", GetVirtualCritical() );
	}
	else
		strMsg.Format( "%d%%", g_pPlayer->GetCriticalProb() );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	//공격속도	
	float fAttackSpeed;
	dwColor = D3DCOLOR_ARGB(255,0,0,0);
	if(	m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed() ) //임의 스탯이 변경되고 현재 능력치랑 다를 경우
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format( "%d%%", int( fAttackSpeed*100.0f ) /2 );
	p2DRender->TextOut( x , y, strMsg, dwColor ); y += nNext;

	x =15; y = 40; nNextX = 60;
	// 아래부터 능력치 관련 
	y = 52;
	int StatYPos = 50;

	if( g_pPlayer->m_Stat.GetOriginStr() == g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginStr() < g_pPlayer->GetStr() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetStr(), dwColor ); y += nNext;

	if( g_pPlayer->m_Stat.GetOriginSta() == g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginSta() < g_pPlayer->GetSta() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetSta(), dwColor ); y += nNext;

	if( g_pPlayer->m_Stat.GetOriginDex() == g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginDex() < g_pPlayer->GetDex() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetDex(), dwColor ); y += nNext;

	if( g_pPlayer->m_Stat.GetOriginInt() == g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,0);
	else
	if( g_pPlayer->m_Stat.GetOriginInt() < g_pPlayer->GetInt() )
		dwColor = D3DCOLOR_ARGB(255,0,0,255);
	else
		dwColor = D3DCOLOR_ARGB(255,255,0,0);

	p2DRender->TextOut( StatYPos, y, g_pPlayer->GetInt(), dwColor ); y += nNext;
/*
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	y = 52;

	p2DRender->TextOut( 135, y, pJobAbillity->dwStr, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwSta, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwDex, dwColor ); y += nNext;
	p2DRender->TextOut( 135, y, pJobAbillity->dwInt, dwColor ); y += nNext;
*/
	if(m_nGpPoint)
	{
		if( (g_nRenderCnt / 8) & 1 )	
		{
			dwColor = D3DCOLOR_ARGB(255,255,0,0);
		}
	}

	p2DRender->TextOut( 105, y, m_nGpPoint, dwColor ); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect( 10, 52, 80, 65 );
	rectHittest[1].SetRect( 10, 67, 80, 80 );
	rectHittest[2].SetRect( 10, 82, 80, 95 );
	rectHittest[3].SetRect( 10, 97, 80, 110 );
	rectHittest[4].SetRect( 10, 112, 160, 125 );

	CRect rectTemp;
	CPoint ptTemp;
	// 툴팁 나오게 하기( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	for( int iC = 0 ; iC < 5 ; ++iC )
	{
		if( rectHittest[iC].PtInRect( ptMouse ) )
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rectHittest[iC] );
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB( 0, 93, 0 );
			DWORD dwColorCommand = D3DCOLOR_XRGB( 180, 0, 0 );
			if( iC == 0 )	// STR
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STR ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetStr() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR0 ) );
				strEdit.AddString( "(" );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STR1 ), dwColorCommand );
				strEdit.AddString( ")" );
			}
			else if( iC == 1 ) // STA
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_STA ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetSta() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_STA1 ) );
			}
			else if( iC == 2 ) // DEX
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_DEX ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetDex() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX1 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX2 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_DEX3 ), dwColorCommand );
			}
			else if( iC == 3 ) // INT
			{
				szString.Format( "%s", prj.GetText( TID_TOOLTIP_INT ) );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				szString.Format( "  %d\n ", g_pPlayer->GetInt() );
				strEdit.AddString( szString, dwColorName , ESSTY_BOLD );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT0 ) );
				strEdit.AddString( "\n " );
				strEdit.AddString( prj.GetText( TID_TOOLTIP_COMMAND_INT1 ) );
			}
			else // GP
				strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_GPPOINT ) );
			
			g_toolTip.PutToolTip( 100, strEdit, rectHittest[iC], ptMouse, 3 );
			break;
		}
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor ); y += nNext;

	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor ); 
	p2DRender->TextOut( 75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor ); y += nNext;
	y += 12;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STR), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_STA), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_TOOLTIP_INT), dwColor ); y += nNext;
	p2DRender->TextOut( 7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor ); y += nNext;
	
	//능력치 상태 Tooltip
	CRect rect;
	rect.SetRect( 7, 10, 160, 38 );
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS1 ) );
		else
			strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_STATUS2 ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
	//Edit Tooltip
	rect.SetRect(90, 52, 160, 110);
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_EDIT ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_APPLY ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}	
	rect = m_wndReset.m_rectWindow;
	if( rect.PtInRect( ptMouse ) )
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		CEditString strEdit;
		strEdit.AddString( prj.GetText( TID_TOOLTIP_CHARSTATUS_RESET ) );
		g_toolTip.PutToolTip( 100, strEdit, rect, ptMouse, 3 );
	}
}

void CWndCharacterDetail2::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "WndCharacter2_2.tga" ) ), TRUE );
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
	int posY = 49;
	int posX = 128;

	m_editStrCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 100 );
	m_wndStrPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 101 );
	m_wndStrMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 102 ); posY += 15;

	m_editStaCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 103 );
	m_wndStaPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 104 );
	m_wndStaMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 105 ); posY += 15;

	m_editDexCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 106 );
	m_wndDexPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 107 );
	m_wndDexMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 108 ); posY += 15;

	m_editIntCount.Create  ( g_Neuz.GetSafeHwnd(), 0, CRect( posX - 38, posY, posX - 4, posY + 16 ), this, 109 );
	m_wndIntPlus.Create  ( "<", 0, CRect( posX, posY + 2, posX + 14, posY + 18 ), this, 110 );
	m_wndIntMinus.Create  ( "<", 0, CRect( posX + 16, posY + 2, posX + 30, posY + 18 ), this, 111 );

	m_wndStrPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStrMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndStaPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndStaMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndDexPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndDexMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharMinus.bmp" ) ), TRUE );
	m_wndIntPlus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharPlus.bmp" ) ), TRUE );
	m_wndIntMinus.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T( "ButtCharMinus.bmp" ) ), TRUE );

	posY += 36;
	m_wndApply.Create  ( "Apply", 0, CRect( posX - 108, posY, posX - 58, posY + 22 ), this, 112 );
	m_wndReset.Create  ( "Reset", 0, CRect( posX - 30, posY, posX + 20, posY + 22 ), this, 113 );

	if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButStateOk.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButStateCancel.tga" ) ), TRUE );
	}
	else
	{
		m_wndApply.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharApply.tga" ) ), TRUE );
		m_wndReset.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),_T( "ButtCharReset.tga" ) ), TRUE );
	}

	//모든 스탯의 증가량 1로 고정이므로 아래 삭제
	/*
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
	*/

	m_nGpPoint = g_pPlayer->GetRemainGP();
	//스탯 증가량 한적 없으므로 Minus Button Default는 False
	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if(g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}

	RefreshStatPoint();
}

BOOL CWndCharacterDetail2::Process()
{
	//Control Button Plus or Minus
	if(m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if(m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if(m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if(m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if(m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);
		
		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
		
		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if(m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}

void CWndCharacterDetail2::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}

void CWndCharacterDetail2::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;
	
	*pnMin = 0;
	*pnMax = 0;
	
	if( g_pPlayer )
	{
		PT_ITEM_SPEC pItemProp = g_pPlayer->GetActiveHandItemProp( nParts );
		if( pItemProp == NULL )
			return;
		
		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;
		
		*pnMin = g_pPlayer->GetParam( DST_ABILITY_MIN, *pnMin );
		*pnMax = g_pPlayer->GetParam( DST_ABILITY_MAX, *pnMax );
		
		int nATK = 0;
		switch( pItemProp->dwWeaponType )
		{
			case WT_MELEE_SWD:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_AXE:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MELEE_STAFF:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_MELEE_STICK:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)) );
				break;
			case WT_MELEE_KNUCKLE:
				nATK = (int)( float( (g_pPlayer->GetStr() + m_nStrCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)) );
				break;
			case WT_MAGIC_WAND:
				nATK = (int)( ( g_pPlayer->GetInt() + m_nIntCount - 10 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f );
				break;
			case WT_MELEE_YOYO:
				nATK = (int)( float(( g_pPlayer->GetStr() + m_nStrCount - 12 ) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)) );
				break;
			case WT_RANGE_BOW:
				nATK = (int)( (((g_pPlayer->GetDex() + m_nDexCount -14)*4.0f + (g_pPlayer->GetLevel()*1.3f) + ((g_pPlayer->GetStr()+m_nStrCount)*0.2f)) * 0.7f) );
				break;
		}
		
		nATK += g_pPlayer->GetPlusWeaponATK( pItemProp->dwWeaponType );	// 무기의 추가 공격력를 구한다.

		int nPlus = nATK + g_pPlayer->GetParam( DST_CHR_DMG, 0 );

		*pnMin += nPlus;
		*pnMax += nPlus;
		
		FLItemElem *pWeapon = g_pPlayer->GetWeaponItem( nParts );
		if( pWeapon && pWeapon->GetProp() )
		{
			float f = g_pPlayer->GetItemMultiplier( pWeapon );
			*pnMin = (int)( *pnMin * f );
			*pnMax = (int)( *pnMax * f );
		}
		
		if( pWeapon )
		{
			int nOption = pWeapon->GetAbilityOption();
			if( nOption > 0 )
			{
				int nValue = (int)pow( (float)( nOption ), 1.5f );

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}

int CWndCharacterDetail2::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info = { 0 };
	//mem_set( &info, 0x00, sizeof(info) );
	info.dwAtkFlags = AF_GENERIC;
	
	float fFactor = 1.0f;
	if( g_pPlayer )
	{
		JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
		FLASSERT( pProperty );
		fFactor = pProperty->fFactorDef;
	}
	nDefense	= (int)( ((((g_pPlayer->GetLevel()*2) + ((g_pPlayer->GetSta()+m_nStaCount)/2)) / 2.8f ) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor) );
	nDefense	+= (g_pPlayer->GetDefenseByItem( FALSE ) / 4);
	nDefense	+= g_pPlayer->GetParam( DST_ADJDEF, 0 );
	nDefense	= (int)( nDefense * g_pPlayer->GetDEFMultiplier( &info ) );

	if( nDefense < 0 )
		nDefense	= 0;

	return nDefense;
}

int CWndCharacterDetail2::GetVirtualCritical()
{
	int nCritical;
	nCritical	= ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical	= (int)( nCritical * g_pPlayer->GetJobPropFactor( JOB_PROP_CRITICAL ) );
	nCritical	= g_pPlayer->GetParam( DST_CHR_CHANCECRITICAL, nCritical );	// 크리티컬 확률을 높여주는 스킬관련 
	if( nCritical < 0 )
		nCritical	= 0;
	
	if( g_pPlayer )
	{
		if( g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL) )	// 파티 && 1회 크리티컬 발동?
		{
			if( g_Party.IsMember( g_pPlayer->m_idPlayer ) )
				nCritical += (g_Party.m_nSizeofMember / 2);	
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}
	}
	return nCritical;
}

float CWndCharacterDetail2::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;
	
	PT_ITEM_SPEC pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if( pWeaponProp )
		fItem = pWeaponProp->fAttackSpeed;
	
	JobProp* pProperty = prj.GetJobProp( g_pPlayer->GetJob() ); 
	FLASSERT( pProperty );
	
	// A = int( 캐릭터의 공속 + ( 무기의 공속 * ( 4 * 덱스 + ( 레벨 / 8 ) ) ) - 3 )
	// 공격속도 = ( ( 50 / 200 - A ) / 2 ) + 가중치 
	int A = int( pProperty->fAttackSpeed + ( fItem * ( 4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f ) ) - 3.0f );

	if( 187.5f <= A )
		A	= (int)( 187.5f );

	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;
	
	int nIndex = A / 10;
	nIndex = max( nIndex, 0 );
	nIndex = min( nIndex, (MAX_ATTACK_SPEED_PLUSVALUE-1) );
	
	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};
	
	fSpeed = ( ( 50.0f / (200.f - A) ) / 2.0f ) + fPlusValue[nIndex];
	
	float fDstParam = g_pPlayer->GetParam( DST_ATTACKSPEED, 0 ) / 1000.0f;
	fSpeed += fDstParam;
	
	if( 0 < g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) )
		fSpeed = fSpeed + ( fSpeed * g_pPlayer->GetParam( DST_ATTACKSPEED_RATE, 0 ) / 100 );
	
	if( fSpeed < 0.0f )
		fSpeed = 0.1f;
	if( fSpeed > 2.0f )
		fSpeed = 2.0f;
	
	return fSpeed;
}

BOOL CWndCharacterDetail2::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 240, 0, 240 + 330, 255 - 135 ); 
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_EXTENSION,rect,pWndParent,dwWndId);
}

BOOL CWndCharacterDetail2::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	int editnum = 0;
	
	if( nID == 100 || nID == 103 || nID == 106 || nID == 109 )
	{
		CString tempnum = m_editStrCount.GetString();
		for( int i = 0 ; i < tempnum.GetLength() ; i++ )
		{
			if( isdigit2( tempnum.GetAt(i) ) == FALSE )
				return FALSE;
		}
	}
	switch(nID) 
	{
		case 100:
			editnum = atoi( m_editStrCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
			m_nStrCount = editnum;
			break;
		case 103:
			editnum = atoi( m_editStaCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
			m_nStaCount = editnum;
			break;
		case 106:
			editnum = atoi( m_editDexCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
			m_nDexCount = editnum;
			break;
		case 109:
			editnum = atoi( m_editIntCount.GetString() );
			if(editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
				editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
			m_nIntCount = editnum;
			break;
	}

	if( message == WNM_CLICKED )		
	{
		switch(nID) 
		{
			case 101: //Str Plus
				m_nStrCount++;
				break;
			case 102: //Str Minus
				m_nStrCount--;
				break;
			case 104: //Sta Plus
				m_nStaCount++;
				break;
			case 105: //Sta Minus
				m_nStaCount--;
				break;
			case 107: //Dex Plus
				m_nDexCount++;
				break;
			case 108: //Dex Minus
				m_nDexCount--;
				break;
			case 110: //Int Plus
				m_nIntCount++;
				break;
			case 111: //Int Minus
				m_nIntCount--;
				break;
			case 112: //Apply
				if(m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
				{
					SAFE_DELETE( g_WndMng.m_pWndStateConfirm );
					g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
					g_WndMng.m_pWndStateConfirm->Initialize();
				}
				break;
			case 113: //Reset
				m_nStrCount = 0;
				m_nStaCount = 0;
				m_nDexCount = 0;
				m_nIntCount = 0;
				RefreshStatPoint();
				break;
		}
	}

	if(nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if(g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if(g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}
	return CWndBase::OnChildNotify( message, nID, pLResult );
}

BOOL CWndCharacterDetail2::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndBase::OnCommand(nID,dwMessage,pWndBase);
}

void CWndCharacterDetail2::OnSize(UINT nType, int cx, int cy)
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

void CWndCharacterDetail2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

void CWndCharacterDetail2::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;
}

void CWndCharacterDetail2::OnMouseWndSurface( CPoint point )
{
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
		m_wndExpert[ i ].SetFontColor( 255, 255, 255 );
	}
	m_wndOk.SetFontColor(  255, 255, 255  );
	m_wndCancel.SetFontColor(  255, 255, 255  );

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

	m_atexTopDown[0] = NULL;
	m_atexTopDown[1] = NULL;
	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;

	m_bSlot[3] = TRUE;	//sun: 9차 전승관련 Clienet
	m_bLegend = FALSE;

	m_nCurrSkillPoint = 0;
}
CWndSkillTreeEx::~CWndSkillTreeEx()
{
	DeleteDeviceObjects();
}

void CWndSkillTreeEx::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	if( ar.IsLoading() )
	{
//sun: 9차 전승관련 Clienet
		ar >> m_bSlot[0] >> m_bSlot[1] >> m_bSlot[2] >> m_bSlot[3];
		BOOL bFlag[4];

		bFlag[0] = m_bSlot[0];
		bFlag[1] = m_bSlot[1];
		bFlag[2] = m_bSlot[2];
		bFlag[3] = m_bSlot[3];
		
		m_bSlot[0] = m_bSlot[1] = m_bSlot[2] = m_bSlot[3] = TRUE;
		
		SetActiveSlot( 0, bFlag[0] );
		SetActiveSlot( 1, bFlag[1] );
		SetActiveSlot( 2, bFlag[2] );
		if(m_bLegend)
		{
			if(FULLSCREEN_HEIGHT == 600)
			{
				if(bFlag[3])
					SetActiveSlot( 2, FALSE );
			}
			else
				SetActiveSlot( 3, bFlag[3] );
		}
	}
	else
	{
		dwVersion = 0;
//sun: 9차 전승관련 Clienet
		ar << m_bSlot[0] << m_bSlot[1] << m_bSlot[2] << m_bSlot[3];

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
//sun: 9차 전승관련 Clienet
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );	
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );	
			}

			int n = rect.Height();
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC3 );
			CRect crect = lpWndStatic->GetWindowRect(TRUE);
			crect.OffsetRect( 0, ((-1) * (int)( dwValue )) );
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

//sun: 9차 전승관련 Clienet
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}

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

//sun: 9차 전승관련 Clienet
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );
			}

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

//sun: 9차 전승관련 Clienet
			if(m_bLegend)
			{
				crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
			
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

//sun: 9차 전승관련 Clienet
			if(m_bLegend)
			{
				CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top+dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top+dwValue );
			}
			
			rect.bottom += dwValue;
		}
		else
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC7 );
			lpWndStatic->SetVisible(FALSE);

//sun: 9차 전승관련 Clienet
			if(m_bLegend)
			{
				CRect crect = m_pWndHeroStatic[0]->GetWindowRect(TRUE);
				m_pWndHeroStatic[0]->Move( crect.left, crect.top-dwValue );
				
				crect = m_pWndHeroStatic[1]->GetWindowRect(TRUE);
				m_pWndHeroStatic[1]->Move( crect.left, crect.top-dwValue );
			}
			
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[2] = bFlag;
	}
//sun: 9차 전승관련 Clienet
	else if(nSlot == 3)
	{
		if( m_bSlot[3] == bFlag )
			return;
		
		DWORD dwValue = 48;
		
		if( bFlag )
		{
			m_pWndHeroStatic[1]->SetVisible(TRUE);
			rect.bottom += dwValue;
		}
		else
		{
			m_pWndHeroStatic[1]->SetVisible(FALSE);
			rect.bottom -= dwValue;
		}
		
		SetWndRect(rect);
		
		m_bSlot[3] = bFlag;
	}

	if( rect.bottom > (int)( FULLSCREEN_HEIGHT-48 ) )
	{
		rect.OffsetRect( 0, (int)(FULLSCREEN_HEIGHT-48)-rect.bottom );
		SetWndRect(rect);
	}
		
	AdjustWndBase();
}


BOOL CWndSkillTreeEx::IsDownPoint(DWORD skill)
{
	LPSKILL pSkill = GetdwSkill( skill );
	if( pSkill == NULL )
		return FALSE;

	LPSKILL pSkillMy = g_pPlayer->GetSkill( pSkill->dwSkill );
	if( pSkillMy == NULL )
		return FALSE;

	if(  pSkill->dwLevel <= pSkillMy->dwLevel )
		return FALSE;

	return TRUE;
}

void CWndSkillTreeEx::SubSkillPointDown( LPSKILL lpSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == NULL_ID )
			continue;
		
		LPSKILL pSkill2 = &m_apSkill[i];
		
		if( pSkill2 == NULL )
			continue;
		
		if( pSkill2->GetProp()->dwReqSkill1 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReqSkillLevel1 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReqSkillLevel1 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
		
		if( pSkill2->GetProp()->dwReqSkill2 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReqSkillLevel2 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReqSkillLevel2 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
	}
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

LPSKILL CWndSkillTreeEx::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];
	
	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	if( pSkill->GetProp() == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Not Prop : %d" ), i );
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
	
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL )
		return FALSE;

//sun: 10차 전승시스템	Neuz, World, Trans

	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV )
		&& g_pPlayer->IsMaster() == FALSE && g_pPlayer->IsHero() == FALSE && g_pPlayer->IsLegendHero() == FALSE )
	{
		return FALSE;
	}
				
	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReqSkill1 );

		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
		}
	}
	
	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReqSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
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
	
	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		SAFE_DELETE_ARRAY( m_apSkill );
	}
	else
		m_apSkill = NULL;
	
	return S_OK;
}

void CWndSkillTreeEx::InitItem(int nJob, LPSKILL apSkill, BOOL bReset )
{
	if( g_pPlayer == NULL )
		return;

	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy( m_apSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );
	}
	else
	{
		m_nCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}

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
	case JOB_KNIGHT_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_KNIGHT_HERO:
		m_nCount  = 26;
		strTex[0] = "Back_Me.TGA";
		strTex[1] = "Back_Night.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		break;			
	case JOB_BLADE:
	case JOB_BLADE_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_BLADE_HERO:
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
	case JOB_BILLPOSTER_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_BILLPOSTER_HERO:
		m_nCount  = 28;
		strTex[0] = "Back_As.TGA";
		strTex[1] = "Back_Bill.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		break;			
	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_RINGMASTER_HERO:
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
	case JOB_ELEMENTOR_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_ELEMENTOR_HERO:
		m_nCount  = 39;
		strTex[0] = "Back_Ma.TGA";
		strTex[1] = "Back_Ele.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		break;			
	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_PSYCHIKEEPER_HERO:
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
	case JOB_ACROBAT:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( "" );
		break;	
	case JOB_JESTER:
	case JOB_JESTER_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_JESTER_HERO:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Jst.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		break;	
	case JOB_RANGER:
	case JOB_RANGER_MASTER:	//sun: 9차 전승관련 Clienet
	case JOB_RANGER_HERO:
		m_nCount  = 20;
		strTex[0] = "Back_Acr.tga";
		strTex[1] = "Back_Rag.TGA";//"Back_Lower.TGA";
		lpWndStatic1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic3->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		break;

	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "알수없는 직업 : %d" ), m_nJob );
		return;
	}
	
//sun: 9차 전승관련 Clienet
	//Master Skill은 시작부터 1Lv이므로 배경 이미지 제외.
	switch( m_nJob )
	{
		case JOB_KNIGHT_HERO:
			m_strHeroSkilBg = "Back_Hero_KntDrawing.tga";
			break;
		case JOB_BLADE_HERO:
			m_strHeroSkilBg = "Back_Hero_BldDefence.tga";
			break;
		case JOB_BILLPOSTER_HERO:
			m_strHeroSkilBg = "Back_Hero_BilDisEnchant.tga";
			break;
		case JOB_RINGMASTER_HERO:
			m_strHeroSkilBg = "Back_Hero_RigReturn.tga";
			break;
		case JOB_ELEMENTOR_HERO:
			m_strHeroSkilBg = "Back_Hero_EleCursemind.tga";
			break;
		case JOB_PSYCHIKEEPER_HERO:
			m_strHeroSkilBg = "Back_Hero_PsyStone.tga";
			break;
		case JOB_JESTER_HERO:
			m_strHeroSkilBg = "Back_Hero_JstSilence.tga";
			break;
		case JOB_RANGER_HERO:
			m_strHeroSkilBg = "Back_Hero_RagHawkeye.tga";
			break;
	}

	SAFE_DELETE( m_atexJobPannel[0] );
	SAFE_DELETE( m_atexJobPannel[1] );

	if( !strTex[0].IsEmpty() )
	{
		m_atexJobPannel[0] = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strTex[0] ), m_atexJobPannel[0] ) == FALSE )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s Open1 실패" ), strTex[0] );

	}

	if( !strTex[1].IsEmpty() )
	{
		m_atexJobPannel[1] = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strTex[1] ), m_atexJobPannel[1] ) == FALSE )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s Open1 실패" ), strTex[1] );

	}
	AdjustWndBase();
	
	// 소스 아이템을 입력
	LoadTextureSkillicon();

	g_nSkillCurSelect = -1;

	m_dwMouseSkill = NULL_ID;

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
	
	int nProGapX = 8;
	int nProGapY = 228;

	SkillProp* pPropSkill = prj.GetSkillProp( dwSkillID );

	if( pPropSkill )
	{
		CPoint pt;

		LPWNDCTRL lpWndCtrl;
		LPWNDCTRL lpWndCtrlUpper = GetWndCtrl( WIDC_STATIC5 );
		LPWNDCTRL lpWndCtrlLower = GetWndCtrl( WIDC_STATIC7 );

		switch( pPropSkill->dwSkillKind1 )
		{
			case JTYPE_BASE  : pt = 0; break;
			case JTYPE_EXPERT: pt = lpWndCtrlUpper->rect.TopLeft(); break;
			case JTYPE_PRO   : pt = lpWndCtrlLower->rect.TopLeft(); break;
			case JTYPE_MASTER : pt = 0; break;	//sun: 9차 전승관련 Clienet
			case JTYPE_HERO  : pt = 0; break;
		}
		int nRectX, nRectY, nJobKind;
		nRectX = nRectY = 0;
		nJobKind = MAX_JOBBASE;

		int nLegendSkill = 0;	//sun: 9차 전승관련 Clienet

		switch(dwSkillID)
		{
			case SKILL_INDEX( 1, SI_VAG_ONE_CLEANHIT ):
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SKILL_INDEX( 2, SI_VAG_ONE_BRANDISH ):
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
			case SKILL_INDEX( 3, SI_VAG_ONE_OVERCUTTER ):
				{
					lpWndCtrl = GetWndCtrl( WIDC_CUSTOM4 );
					rect = lpWndCtrl->rect;
					rect.top += 1;
				}
				break;
	//머서너리
			case SKILL_INDEX( 4, SI_MER_ONE_SPLMASH ):				nRectX =  34, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 9, SI_MER_SHIELD_PROTECTION ):			nRectX =  34, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 5, SI_MER_ONE_KEENWHEEL ):				nRectX =  84, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 112, SI_MER_ONE_BLOODYSTRIKE ):			nRectX =  84, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 10, SI_MER_SHIELD_PANBARRIER ):			nRectX =  84, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 111, SI_MER_SUP_IMPOWERWEAPON ):			nRectX =  84, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 6, SI_MER_ONE_BLINDSIDE ):				nRectX = 134, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 14, SI_MER_ONE_REFLEXHIT ):				nRectX = 134, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 13, SI_MER_ONE_SNEAKER ):				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 109, SI_MER_SUP_SMITEAXE ):				nRectX = 134, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 108, SI_MER_SUP_BLAZINGSWORD ):			nRectX = 134, nRectY = 201, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 11 ,SI_MER_ONE_SPECIALHIT ):				nRectX = 184, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 12, SI_MER_ONE_GUILOTINE ):				nRectX = 184, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 8, SI_MER_SUP_AXEMASTER ):				nRectX = 184, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 7, SI_MER_SUP_SWORDMASTER ):			nRectX = 184, nRectY = 201, nJobKind = MAX_EXPERT; break;
	// 나이트
			case SKILL_INDEX( 133, SI_KNT_TWOSW_CHARGE ):				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 134, SI_KNT_TWOAX_PAINDEALER ):			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 128, SI_KNT_SUP_GUARD ):					nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 132, SI_KNT_TWOSW_EARTHDIVIDER ):			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 135, SI_KNT_TWOAX_POWERSTUMP ):			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 130, SI_KNT_SUP_RAGE ):					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 131, SI_KNT_TWO_POWERSWING ):				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 129, SI_KNT_SUP_PAINREFLECTION ):			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	// 블레이드
			case SKILL_INDEX( 138, SI_BLD_DOUBLESW_SILENTSTRIKE ):		nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 140, SI_BLD_DOUBLEAX_SPRINGATTACK ):		nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 137, SI_BLD_DOUBLE_ARMORPENETRATE ):		nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 139, SI_BLD_DOUBLESW_BLADEDANCE ):		nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 141, SI_BLD_DOUBLEAX_HAWKATTACK ):		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 143, SI_BLD_SUP_BERSERK ):				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 136, SI_BLD_DOUBLE_CROSSSTRIKE ):			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 142, SI_BLD_DOUBLE_SONICBLADE ):			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
				
	// 어시스트	
			case SKILL_INDEX( 44, SI_ASS_HEAL_HEALING ):				nRectX =  18, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 46, SI_ASS_HEAL_PATIENCE ):				nRectX =  64, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 114, SI_ASS_CHEER_QUICKSTEP ):			nRectX =  64, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 52, SI_ASS_CHEER_MENTALSIGN ):			nRectX =  64, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 104, SI_ASS_KNU_TAMPINGHOLE ):			nRectX =  64, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 45, SI_ASS_HEAL_RESURRECTION ):			nRectX = 110, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 20, SI_ASS_CHEER_HASTE ):				nRectX = 110, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 49, SI_ASS_CHEER_HEAPUP ):				nRectX = 110, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 113, SI_ASS_CHEER_STONEHAND ):			nRectX = 110, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 51, SI_ASS_CHEER_CIRCLEHEALING ):		nRectX = 156, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 115, SI_ASS_CHEER_CATSREFLEX ):			nRectX = 156, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 53, SI_ASS_CHEER_BEEFUP ):				nRectX = 156, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 105, SI_ASS_KNU_BURSTCRACK ):				nRectX = 156, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 48, SI_ASS_HEAL_PREVENTION ):			nRectX = 202, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 50, SI_ASS_CHEER_CANNONBALL ):			nRectX = 202, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 116, SI_ASS_CHEER_ACCURACY ):				nRectX = 202, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 117, SI_ASS_KNU_POWERFIST ):				nRectX = 202, nRectY = 187, nJobKind = MAX_EXPERT; break;
	//링마스터
			case SKILL_INDEX( 146, SI_RIN_SUP_PROTECT ):				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 145, SI_RIN_SUP_HOLYCROSS ):				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 149, SI_RIN_HEAL_GVURTIALLA ):			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 147, SI_RIN_SUP_HOLYGUARD ):				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 148, SI_RIN_SUP_SPIRITUREFORTUNE ):		nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 144, SI_RIN_HEAL_HEALRAIN ):				nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 150, SI_RIN_SQU_GEBURAHTIPHRETH ):		nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 151, SI_RIN_SUP_MERKABAHANZELRUSHA ):		nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//빌포스터
			case SKILL_INDEX( 152, SI_BIL_KNU_BELIALSMESHING ):			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 156, SI_BIL_PST_ASMODEUS ):				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 154, SI_BIL_KNU_BLOODFIST ):				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 157, SI_BIL_PST_BARAQIJALESNA ):			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 153, SI_BIL_KNU_PIERCINGSERPENT ):		nRectX = 134, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 158, SI_BIL_PST_BGVURTIALBOLD ):			nRectX = 134, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 155, SI_BIL_KNU_SONICHAND ):				nRectX = 184, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ):			nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
								
	// 매지션
			case SKILL_INDEX( 121, SI_MAG_MAG_MENTALSTRIKE ):			nRectX =  34, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 107, SI_MAG_MAG_BLINKPOOL ): 				nRectX =  34, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 64, SI_MAG_FIRE_BOOMERANG ):				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 69, SI_MAG_WIND_SWORDWIND ):				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 118, SI_MAG_WATER_ICEMISSILE ):			nRectX =  84, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 119, SI_MAG_ELECTRICITY_LIGHTINGBALL ):	nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 120, SI_MAG_EARTH_SPIKESTONE ):			nRectX =  84, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 65, SI_MAG_FIRE_HOTAIR ):				nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 70, SI_MAG_WIND_STRONGWIND ):			nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 32, SI_MAG_WATER_WATERBALL ):			nRectX = 134, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 34, SI_MAG_ELECTRICITY_LIGHTINGPARM ):	nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 36, SI_MAG_EARTH_ROCKCRASH ):			nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 30, SI_MAG_FIRE_FIRESTRIKE ):			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 31, SI_MAG_WIND_WINDCUTTER ):			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 33, SI_MAG_WATER_SPRINGWATER ):			nRectX = 184, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 35, SI_MAG_ELECTRICITY_LIGHTINGSHOCK ):	nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 37, SI_MAG_EARTH_LOOTING ):				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
	// 엘리멘터
			case SKILL_INDEX( 168, SI_ELE_FIRE_FIREBIRD ):				nRectX =  34, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 174, SI_ELE_EARTH_STONESPEAR ):			nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 180, SI_ELE_WIND_VOID ):					nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 171, SI_ELE_ELECTRICITY_THUNDERSTRIKE ):	nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 177, SI_ELE_WATER_ICESHARK ):				nRectX =  34, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 170, SI_ELE_FIRE_BURINGFIELD ):			nRectX =  84, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 176, SI_ELE_EARTH_EARTHQUAKE ):			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 182, SI_ELE_WIND_WINDFIELD ):				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 173, SI_ELE_ELECTRICITY_ELETRICSHOCK ):	nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 179, SI_ELE_WATER_POISONCLOUD ):			nRectX =  84, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 183, SI_ELE_MULTY_METEOSHOWER ):			nRectX = 134, nRectY = 246, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 185, SI_ELE_MULTY_SANDSTORM ):			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 184, SI_ELE_MULTY_LIGHTINGSTORM ):		nRectX = 134, nRectY = 298, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 186, SI_ELE_MULTY_AVALANCHE ):			nRectX = 134, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 169, SI_ELE_FIRE_FIREMASTER ):			nRectX = 184, nRectY = 234, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 175, SI_ELE_EARTH_EARTHMASTER ):			nRectX = 184, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 181, SI_ELE_WIND_WINDMASTER ):			nRectX = 184, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 172, SI_ELE_ELECTRICITY_LIGHTINGMASTER ):	nRectX = 184, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 178, SI_ELE_WATER_WATERMASTER ):			nRectX = 184, nRectY = 338, nJobKind = MAX_PROFESSIONAL; break;
	// 사이키퍼
			case SKILL_INDEX( 160, SI_PSY_NLG_DEMONOLGY ):				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 162, SI_PSY_PSY_PSYCHICBOMB ):			nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 165, SI_PSY_NLG_CRUCIOSPELL ):			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 161, SI_PSY_NLG_SATANOLGY ):				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 164, SI_PSY_PSY_SPRITBOMB ):				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 166, SI_PSY_PSY_MAXIMUMCRISIS ):			nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 163, SI_PSY_PSY_PSYCHICWALL ):			nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 167, SI_PSY_PSY_PSYCHICSQUARE ):			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	////////////////////////////////////////////////////////////////////////////////////
	// 아크로벳
			case SKILL_INDEX( 206, SI_ACR_YOYO_PULLING ):				nRectX =  34, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 197, SI_ACR_SUP_SLOWSTEP ):				nRectX =  34, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 194, SI_ACR_BOW_JUNKBOW ):				nRectX =  34, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 195, SI_ACR_SUP_FASTWALKER ):				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 191, SI_ACR_SUP_YOYOMASTER ):				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 192, SI_ACR_SUP_BOWMASTER ): 				nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ):			nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 203, SI_ACR_YOYO_SNITCH ):				nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 201, SI_ACR_YOYO_CROSSLINE ):				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 198, SI_ACR_BOW_SILENTSHOT ):				nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 196, SI_ACR_BOW_AIMEDSHOT ):				nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 199, SI_ACR_YOYO_ABSOLUTEBLOCK ):			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 205, SI_ACR_YOYO_DEADLYSWING ):			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 204, SI_ACR_YOYO_COUNTER ):				nRectX = 184, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 202, SI_ACR_BOW_AUTOSHOT ):				nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 200, SI_ACR_BOW_ARROWRAIN ):				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
	//제스터
			case SKILL_INDEX( 208, SI_JST_SUP_POISON ):					nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 209, SI_JST_SUP_BLEEDING ):				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 213, SI_JST_YOYO_ESCAPE ):				nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 207, SI_JST_YOYO_CRITICALSWING ):			nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 211, SI_JST_YOYO_BACKSTAB ):				nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 210, SI_JST_SUP_ABSORB ):					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 214, SI_JST_YOYO_VATALSTAB ):				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 212, SI_JST_YOYO_HITOFPENYA ):			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
	//레인저
			case SKILL_INDEX( 216, SI_RAG_BOW_ICEARROW ):				nRectX =  34, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 217, SI_RAG_BOW_FLAMEARROW ):				nRectX =  34, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 219, SI_RAG_BOW_POISONARROW ):			nRectX =  34, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 215, SI_RAG_SUP_FASTATTACK ):				nRectX =  84, nRectY = 260, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 218, SI_RAG_BOW_PIRCINGARROW ):			nRectX =  84, nRectY = 286, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 221, SI_RAG_SUP_NATURE ):					nRectX =  84, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 222, SI_RAG_BOW_TRIPLESHOT ):				nRectX = 134, nRectY = 272, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 220, SI_RAG_BOW_SILENTARROW ):			nRectX = 134, nRectY = 312, nJobKind = MAX_PROFESSIONAL; break;

			//sun: 9차 전승관련 Clienet				
			//마스터 스킬
			case SKILL_INDEX( 309, SI_BLD_MASTER_ONEHANDMASTER ):		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 310, SI_KNT_MASTER_TWOHANDMASTER ):		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 311, SI_JST_MASTER_YOYOMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 312, SI_RAG_MASTER_BOWMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 313, SI_ELE_MASTER_INTMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 314, SI_PSY_MASTER_INTMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 315, SI_BIL_MASTER_KNUCKLEMASTER ):		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 316, SI_RIG_MASTER_BLESSING ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			//영웅 스킬			
			case SKILL_INDEX( 237, SI_BLD_HERO_DEFFENCE ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 238, SI_KNT_HERO_DRAWING ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 239, SI_JST_HERO_SILENCE ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 240, SI_RAG_HERO_HAWKEYE ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 241, SI_ELE_HERO_CURSEMIND ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 242, SI_PSY_HERO_STONE ):					nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 243, SI_BIL_HERO_DISENCHANT ):			nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 244, SI_RIG_HERO_RETURN ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			default:
				return FALSE;
		}

		if( nJobKind == MAX_EXPERT )
			rect = CRect(  nRectX-nExpertGapX ,  nRectY-nExpertGapY, nRectX+24-nExpertGapX, nRectY+24-nExpertGapY );
		else if( nJobKind == MAX_PROFESSIONAL )
			rect = CRect(  nRectX-nProGapX,  nRectY-nProGapY, nRectX+24-nProGapX, nRectY+24-nProGapY );
//sun: 9차 전승관련 Clienet
		else if( nJobKind == MAX_MASTER || nJobKind == MAX_HERO )
		{
			LPWNDCTRL lpWndCtrl;
			if(nLegendSkill == 0)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
			else if(nLegendSkill == 1)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );

			rect = lpWndCtrl->rect;
		}
		
		rect += pt;
	}
/*
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
*/
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
			SkillProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_atexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}
	m_textPackNum.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "icon_IconSkillLevel.inc" ) ) );
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
		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType  = 0;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId       = g_nSkillCurSelect; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture = m_atexSkill[ g_nSkillCurSelect ];//L;//pItemElem->m_pTexture;
		FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pSkillProp->szName);
	}
}

BOOL CWndSkillTreeEx::Process()
{
	if( m_apSkill == NULL || g_pPlayer == NULL)
		return FALSE;
	
//sun: 9차 전승관련 Clienet
	if(!m_bLegend)
	{
		if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		{
			m_bLegend = TRUE;
			//Hero SkillTree
			m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			m_pWndHeroStatic[0]->EnableWindow(TRUE);
			m_pWndHeroStatic[0]->SetVisible(TRUE);
			
			m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
			m_pWndHeroStatic[1]->EnableWindow(TRUE);
			m_pWndHeroStatic[1]->SetVisible(TRUE);

			CRect rect = GetWindowRect();
			rect.bottom += 90;
			m_bSlot[3] = TRUE;
			
			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();
			
			CPoint point( x, y );
			Move( point );
			return TRUE;
		}
	}
	else
	{
		if(g_pPlayer->GetLegendChar() < LEGEND_CLASS_MASTER)
		{
			m_bLegend = FALSE;
			//Normal SkillTree
			m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
			m_pWndHeroStatic[0]->EnableWindow(FALSE);
			m_pWndHeroStatic[0]->SetVisible(FALSE);
			
			m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
			m_pWndHeroStatic[1]->EnableWindow(FALSE);
			m_pWndHeroStatic[1]->SetVisible(FALSE);
			
			CRect rect = GetWindowRect();
			rect.bottom -= 100;
			m_bSlot[3] = FALSE;
			
			SetWndRect(rect);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			int x = rectRoot.right - rect.Width();
			int y = rectRoot.bottom - rect.Height();
			
			CPoint point( x, y );
			Move( point );
			return TRUE;
		}
	}

	m_pWndButton[0]->EnableWindow( FALSE );
	m_pWndButton[1]->EnableWindow( FALSE );
	m_pWndButton[2]->EnableWindow( FALSE );
	m_pWndButton[3]->EnableWindow( FALSE );
	
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		LPSKILL lpSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
		SkillProp* pSkillProp = m_pFocusItem->GetProp();
		if( pSkillProp == NULL || lpSkillUser == NULL )
			return TRUE;
		
		int nPoint = prj.GetSkillPoint( pSkillProp );
		if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint )
			m_pWndButton[0]->EnableWindow( TRUE );	

		if( m_pFocusItem->dwLevel != lpSkillUser->dwLevel )
			m_pWndButton[1]->EnableWindow( TRUE );	

		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
		}

//sun: 9차 전승관련 Clienet
		pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if(pSkillProp->dwSkillKind1 == JTYPE_MASTER || pSkillProp->dwSkillKind1 == JTYPE_HERO )
		{
			m_pWndButton[0]->EnableWindow( FALSE );
			m_pWndButton[1]->EnableWindow( FALSE );
			m_pWndButton[2]->EnableWindow( FALSE );
			m_pWndButton[3]->EnableWindow( FALSE );
		}
	}
	return TRUE;
}

void CWndSkillTreeEx::OnMouseWndSurface( CPoint point )
{
	DWORD dwMouseSkill = NULL_ID;
	for( int i = 0; i < MAX_SKILL_JOB; i++ )
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL )
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_BASE )
					continue;
			}

			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_EXPERT )
					continue;
			}

			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_PRO )
					continue;
			}
//sun: 9차 전승관련 Clienet
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_HERO )
					continue;
			}
			GetCalcImagePos( pSkillProp->dwSkillKind1 );
		}

		if( dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) )
			{
				rect.top  -= 2;
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
				rect.OffsetRect( 0, m_nTopDownGap );

				if( rect.PtInRect(point) )
				{
					dwMouseSkill = dwSkill;

					ClientToScreen( &point );
					ClientToScreen( &rect );
					g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point, &rect, CheckSkill( i ) );
					break;
				}
			}
		}
	}
	m_dwMouseSkill = dwMouseSkill;
}

int CWndSkillTreeEx::GetCalcImagePos(int nIndex)
{
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
//sun: 9차 전승관련 Clienet
	if( m_bSlot[3] )
	{
		if( nIndex == JTYPE_MASTER || nIndex == JTYPE_HERO )
		{
			if( m_bSlot[0] )
			{
				if( m_bSlot[1] )
				{
					if( m_bSlot[2] )
						m_nTopDownGap = 0;
					else
						m_nTopDownGap -= 144;
				}
				else
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= 144;
					else
						m_nTopDownGap -= (144+144);
				}
			}
			else
			{
				if( m_bSlot[1] )
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= 48;
					else
						m_nTopDownGap -= (144+48);
				}
				else
				{
					if( m_bSlot[2] )
						m_nTopDownGap -= (144+48);
					else
						m_nTopDownGap -= (144+144+48);
				}
			}
		}
	}

	return 0;
}

void CWndSkillTreeEx::OnDraw(C2DRender* p2DRender)
{
	if( m_apSkill == NULL )
		return;

	m_nAlphaCount = m_nAlpha;
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

//sun: 9차 전승관련 Clienet
	if(m_bLegend && m_bSlot[3])
	{
		//Master Skill은 시작부터 1Lv이므로 배경 이미지 제외.
/*		if( !m_strHeroSkilBg[0].IsEmpty() )
		{
			CTexture* pTexture;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM5 );
			rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_MASTER);
			rect.OffsetRect( 0, m_nTopDownGap );
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, m_strHeroSkilBg[0]), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
*/		
		if( !m_strHeroSkilBg.IsEmpty() )
		{
			CTexture* pTexture;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl;
			
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM6 );
			rect = lpWndCtrl->rect;
			GetCalcImagePos(JTYPE_HERO);
			rect.OffsetRect( 0, m_nTopDownGap );
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), m_strHeroSkilBg), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
	}

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_PRO )
					continue;
			}		
//sun: 9차 전승관련 Clienet
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_MASTER || pSkillProp->dwSkillKind1 == JTYPE_HERO )
					continue;
			}
			GetCalcImagePos( pSkillProp->dwSkillKind1 );
		}
		
		if( pSkillProp && dwSkill != NULL_ID )
		{
			// 스킬 아이콘 출력 
			if( m_atexSkill[ i ] && CheckSkill( i ) && 0 < pSkill->dwLevel )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					rect.OffsetRect( 0, m_nTopDownGap );
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill( pSkill->dwSkill );
					if( pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel )
						nAddNum = 20;
					if( pSkill->dwLevel < pSkillProp->dwExpertMax )
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), pSkill->dwLevel-1 + nAddNum );
					else
						m_textPackNum.Render( p2DRender, rect.TopLeft()-CPoint(2,2), 19 + nAddNum );
				}
			}	
			else if( m_dwMouseSkill == dwSkill && m_atexSkill[ i ] && CheckSkill( i ) )
			{
				CRect rect;
				if( GetSkillPoint( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					rect.OffsetRect( 0, m_nTopDownGap );
					m_atexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );				
				}
			}
		}
	}

	CWndStatic* lpWndStatic9 = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );
	CString strSP;
	strSP.Format( "%d", m_nCurrSkillPoint );
	lpWndStatic9->SetTitle( strSP );

	// 선택된 스킬이 있을시 위 창에 출력
	if( m_pFocusItem && g_nSkillCurSelect > -1 )
	{
		if( m_pFocusItem->dwSkill == -1 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "스킬쓴넘 직업 %d\n스킬 인덱스가 %d인값이 나옴 : 레벨 %d, CurSelect %d" ), g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, g_nSkillCurSelect );
		}

		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_atexSkill[ g_nSkillCurSelect ]->Render( p2DRender, lpWndCtrl->rect.TopLeft() );

		SkillProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp && 0 < m_pFocusItem->dwLevel )
		{
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
			if( pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel )
				nAddNum = 20;

			if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
				m_textPackNum.Render( p2DRender, lpWndCtrl->rect.TopLeft(), m_pFocusItem->dwLevel-1 + nAddNum );
			else
				m_textPackNum.Render( p2DRender, lpWndCtrl->rect.TopLeft(), 19 + nAddNum );
		}
	}
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
	
	///////////////////////////////////////////////////////////////////////////////////////
//sun: 9차 전승관련 Clienet	
	if(m_bLegend)
	{
		pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		rect = pStatic->GetWindowRect(TRUE);
		
		rect.left = rect.right - 30;
		
		if( m_bSlot[3] )
			m_atexTopDown[ 0 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
		else
			m_atexTopDown[ 1 ]->Render( p2DRender, rect.TopLeft(), CPoint( 20, 20 ) );
	}

/*
	{
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM7 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM8 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM9 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM10 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM11 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM12 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM13 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	lpWndCtrl = GetWndCtrl( WIDC_CUSTOM14 );
	FLTRACE_LOG( PROGRAM_NAME, _T( "rect = CRect( %d, %d, %d, %d ); break;" ), lpWndCtrl->rect.left, lpWndCtrl->rect.top, lpWndCtrl->rect.right, lpWndCtrl->rect.bottom );
	}
	/**/	
}
void CWndSkillTreeEx::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauFillNormal.bmp" ), 0xffff00ff, TRUE );

	m_aSkillLevel[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtSkillLevelHold01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtSkillLevelup01.tga" ), 0xffff00ff );
	m_aSkillLevel[ 2 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "ButtSkillLevelHold02.tga" ), 0xffff00ff );
	
	InitItem( g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );


	m_atexTopDown[ 0 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"LvUp.bmp" ), 0xffff00ff );
	m_atexTopDown[ 1 ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"LvDn.bmp" ), 0xffff00ff );

	m_nTopDownGap = 0;
	m_bSlot[0] = TRUE;
	m_bSlot[1] = TRUE;
	m_bSlot[2] = TRUE;
	m_bSlot[3] = TRUE;		//sun: 9차 전승관련 Clienet
	
	if( g_nSkillCurSelect >= 0 )
		m_pFocusItem = &m_apSkill[ g_nSkillCurSelect ];

	m_pWndButton[0] = (CWndButton*) GetDlgItem( WIDC_BUTTON1 );	// + 버튼
	m_pWndButton[1] = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );	// - 버튼
	m_pWndButton[2] = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );	// Reset 버튼
	m_pWndButton[3] = (CWndButton*) GetDlgItem( WIDC_BUTTON4 );	// Finish 버튼
	
//sun: 9차 전승관련 Clienet
	if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		m_bLegend = TRUE;
	else
		m_bLegend = FALSE;

	if(m_bLegend)
	{
		m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		m_pWndHeroStatic[0]->EnableWindow(TRUE);
		m_pWndHeroStatic[0]->SetVisible(TRUE);

		m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
		m_pWndHeroStatic[1]->EnableWindow(TRUE);
		m_pWndHeroStatic[1]->SetVisible(TRUE);
	}
	else
	{
		m_pWndHeroStatic[0] = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		m_pWndHeroStatic[0]->EnableWindow(FALSE);
		m_pWndHeroStatic[0]->SetVisible(FALSE);
		
		m_pWndHeroStatic[1] = (CWndStatic*)GetDlgItem( WIDC_STATIC11 );
		m_pWndHeroStatic[1]->EnableWindow(FALSE);
		m_pWndHeroStatic[1]->SetVisible(FALSE);
		
		CRect rect = GetWindowRect();
		rect.bottom -= 100;
		m_bSlot[3] = FALSE;
		
		SetWndRect(rect);
	}

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
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		SkillProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp )
		{
			int nPoint = prj.GetSkillPoint( pSkillProp );
			switch( nID )
			{
			case WIDC_BUTTON1:	// + 버튼
				{
					if( nPoint <= m_nCurrSkillPoint )
					{
						if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
						{
							m_nCurrSkillPoint -= nPoint;
							++m_pFocusItem->dwLevel;
						}
						else
						{
							break;							
						}
					}					
				}
				break;
			case WIDC_BUTTON2:	// - 버튼
				{
					if( IsDownPoint(m_pFocusItem->dwSkill) )
					{
						m_nCurrSkillPoint += nPoint;
						--m_pFocusItem->dwLevel;
						SubSkillPointDown( m_pFocusItem );
					}
				}
				break;
			case WIDC_BUTTON3:	// Reset 버튼
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
						InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
					m_pFocusItem = NULL;
				}
				break;
			case WIDC_BUTTON4:	// Finish 버튼
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
					{
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					}
				}
				break;
			}
		}
	}

	if( nID == 10000 )
	{
		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase( APP_QUEITMWARNING );	
			if( pWndBase == NULL )
			{
				SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				g_WndMng.m_pWndReSkillWarning->InitItem( TRUE );
			}
			return TRUE;
		}
	}
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

	CPoint ptMouse = GetMousePoint();


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
//sun: 9차 전승관련 Clienet
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(2, TRUE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
		else
		{
			SetActiveSlot(1, TRUE );
//sun: 9차 전승관련 Clienet
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(2, FALSE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
	}

	pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	rect = pStatic->GetWindowRect(TRUE);
	if( rect.PtInRect( point ) ) 
	{
		if( m_bSlot[2] )
		{
			SetActiveSlot(2, FALSE );
//sun: 9차 전승관련 Clienet
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(1, TRUE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
		else
		{
			SetActiveSlot(2, TRUE );
//sun: 9차 전승관련 Clienet
			if(FULLSCREEN_HEIGHT == 600 && m_bLegend)
			{
				SetActiveSlot(1, FALSE );
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect();
				int x = this->m_rectWindow.left;
				int y = rectRoot.bottom - rect.Height();
				
				CPoint point( x, y );
				Move( point );
			}
		}
	}

//sun: 9차 전승관련 Clienet
	if(m_bLegend)
	{
		pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC1 );
		rect = pStatic->GetWindowRect(TRUE);
		if( rect.PtInRect( point ) ) 
		{
			if( m_bSlot[3] )
				SetActiveSlot(3, FALSE );
			else
				SetActiveSlot(3, TRUE );
		}
	}
				
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_PRO )
					continue;
			}		
			
//sun: 9차 전승관련 Clienet
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_MASTER || pSkillProp->dwSkillKind1 == JTYPE_HERO )
					continue;
			}
			GetCalcImagePos( pSkillProp->dwSkillKind1 );
		}

		if( pSkillProp && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillPoint( pSkillProp->dwID, rect ) && CheckSkill( i ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;

				rect.OffsetRect( 0, m_nTopDownGap );
				
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
}
void CWndSkillTreeEx::OnRButtonDblClk( UINT nFlags, CPoint point)
{

}	

void CWndSkillTreeEx::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	// 스킬바 등록 및 스킬레벨 업
	if( m_apSkill == NULL )
		return;

	// 스킬창에서 더블클릭하면 자동으로 스킬바에 등록된다.
	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;
		
		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		if( pSkillProp )
		{
			if( !m_bSlot[0] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_BASE )
					continue;
			}
			
			if( !m_bSlot[1] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_EXPERT )
					continue;
			}
			
			if( !m_bSlot[2] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_PRO )
					continue;
			}		
//sun: 9차 전승관련 Clienet
			if( !m_bSlot[3] )
			{
				if( pSkillProp->dwSkillKind1 == JTYPE_MASTER || pSkillProp->dwSkillKind1 == JTYPE_HERO )
					continue;
			}
			
			GetCalcImagePos( pSkillProp->dwSkillKind1 );
		}

		if( pSkillProp && dwSkill != NULL_ID )
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

					if( g_pPlayer->CheckSkill( m_pFocusItem->dwSkill ) == FALSE )
						return;
					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if( pTaskBar->m_nExecute == 0 )		// 스킬큐가 사용되지 않을때만 등록됨.
						pTaskBar->SetSkillQueue( pTaskBar->m_nCurQueueNum, 0, i, m_atexSkill[i] );
					break;
				}			
			}	
		}
	}
}

void CWndSkillTreeEx::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;
 
	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	
	lpWndCtrl = GetWndCtrl( WIDC_STATIC5 );
	pt = lpWndCtrl->rect.TopLeft() + pt2;

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
BOOL CWndTradeGold::Process( void )
{
	FLItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
	if( pItemBase == NULL )
		return TRUE;

	if( ( ( FLItemElem* )pItemBase)->GetExtra() > 0 )
	{
		Destroy();
	}
	return TRUE;
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
	FLIntToString( m_dwGold, szNumber, _countof( szNumber ), 10 );
	pWndEdit->SetString( szNumber );
}

BOOL CWndTradeGold::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, WBS_MODAL, 0, pWndParent );
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
			if( nCost > g_pPlayer->GetGold() )
				nCost = g_pPlayer->GetGold();

			g_DPlay.SendDropGold( nCost, g_pPlayer->GetPos(), m_vPos );
		}
		else
		if( m_nIdWndTo == APP_TRADE )
		{
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
				FLItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
					}

					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendTradePut( (BYTE)( m_Shortcut.m_dwData ), (BYTE)( m_Shortcut.m_dwType ), m_Shortcut.m_dwId, nCost );
				}
			}
		}
		else
		if( m_nIdWndTo == APP_BANK )
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > g_pPlayer->GetGold() )
					nCost = g_pPlayer->GetGold();

				g_DPlay.SendPutGoldBank( m_nSlot, nCost );
			}
			else // 아이템
			{
				FLItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
					}
					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendPutItemBank( m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
				}
			}
		}
//sun: 11, 주머니
		else
		if( m_nIdWndTo == APP_BAG_EX )
		{
			if( m_Shortcut.m_dwData != 0 )
			{
				
				if( nCost > (int)( ( m_dwGold ) ) )
				{
					nCost = m_dwGold;
				}
				m_Shortcut.m_dwData -= 100;
				//g_DPlay.SendPutItemBank( m_nSlot, m_Shortcut.m_dwId, nCost );
				g_DPlay.SendMoveItem_Pocket( m_nPutSlot, m_Shortcut.m_dwId, nCost , m_nSlot);
				
			}
		}
		// 자신의 인벤토리에서 길드창고로 아이템을 이동
		else
		if (m_nIdWndTo == APP_GUILD_BANK)
		{
			if( m_Shortcut.m_dwData != 0 ) // 아이템
			{
				FLItemBase* pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
				if( pItemBase )
				{
					if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
					{
						nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
					}
					
					m_Shortcut.m_dwData -= 100;

					g_DPlay.SendPutItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 1 );

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
					if( (DWORD)nCost > ( g_pPlayer->GetGuild()->m_nGoldGuild ) )
						nCost = g_pPlayer->GetGuild()->m_nGoldGuild;
					
					if( nCost > 0 )
					{
						g_DPlay.SendGetItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 0); // 0은 페냐가 변경됐다는 것을 의미한다.
					}
				}
				else // 아이템
				{
					FLItemBase* pItemBase = NULL;
					if( m_Shortcut.m_dwType == ITYPE_ITEM )
					{
						pItemBase = g_pPlayer->GetGuild()->m_GuildBank.GetAtId( m_Shortcut.m_dwId );
					}
					else
					{
						//					FLASSERT(0);
					}

					if( pItemBase )
					{
						if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemGuildBank( (BYTE)( m_Shortcut.m_dwId ), nCost, 1 ); // 1은 아이템이 변경됐다는 것을 의미한다.
					}
				}
			}
		}

		else
		// 인벤토리 인데 쓰고 있어서 Gold로 씀.
		if( m_nIdWndTo == APP_GOLD ) // 뱅크에서 인벤토리로 빼옴
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > (int)( g_pPlayer->m_dwGoldBank[m_nSlot] ) )
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
				FLItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemBankId( m_nSlot, m_Shortcut.m_dwId );

					if( pItemBase )
					{
						if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemBank( m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
					}
				}
				else
				{
					//					FLASSERT(0);
				}
			}
		}
		else
		if( m_nIdWndTo == APP_COMMON_BANK )
		{
			if( m_Shortcut.m_dwData == 0 ) // 돈
			{
				if( nCost > (int)( g_pPlayer->m_dwGoldBank[m_nPutSlot] ) )
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
				FLItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemBankId( m_nPutSlot, m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
						}
						
						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendPutItemBankToBank( m_nPutSlot, m_nSlot, (BYTE)( m_Shortcut.m_dwId ), nCost );
					}
				}
				else
				{
					//					FLASSERT(0);
				}
			}
		}
		else
		if( m_nIdWndTo == APP_POST_SEND )
		{
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
				FLItemBase* pItemBase = NULL;
				if( m_Shortcut.m_dwType == ITYPE_ITEM )
				{
					pItemBase = g_pPlayer->GetItemId( m_Shortcut.m_dwId );
					if( pItemBase )
					{				
						if( nCost > ( (FLItemElem*)pItemBase )->m_nItemNum )
						{
							nCost = ( (FLItemElem*)pItemBase )->m_nItemNum;
						}
						
						pItemBase->SetExtra( nCost );		

						m_Shortcut.m_dwData -= 100;

						if( g_WndMng.m_pWndPost )
						{
							g_WndMng.m_pWndPost->m_PostTabSend.SetItemId( (BYTE)( m_Shortcut.m_dwId ) );
							g_WndMng.m_pWndPost->m_PostTabSend.SetCount(nCost);
						}						
					}
				}
				else
				{
					//					FLASSERT(0);
				}
			}
		}			
		Destroy();
	}
	if( nID == WIDC_CANCEL )
	{
		if( m_nIdWndTo == APP_POST_SEND )
		{
			g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
			g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
		}
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
	g_DPlay.SendTradeCancel();
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
}

void CWndTrade::OnDraw(C2DRender* p2DRender)
{
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
	if( pMover && pMover->GetType() == OT_MOVER )
	{
		CWndStatic* pWndNameYou = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		CWndStatic* pWndNameI = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
		CWndStatic* pWndGoldYou = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
		CWndStatic* pWndGoldI = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

		pWndNameYou->SetTitle( pMover->GetName( TRUE ) );
		pWndNameI->SetTitle( g_pPlayer->GetName() );

		CString string;
		int nLength;
		string.Format( "%d", m_nGoldYou );
		
		nLength = string.GetLength();
		while(nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldYou->SetTitle( string );

		string.Format( "%d", m_nGoldI );
		nLength = string.GetLength();
		while(nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldI->SetTitle( string );
		
	}
	else
	{
		g_pPlayer->m_vtInfo.TradeClear();
	}
}
void CWndTrade::OnInitialUpdate()
{
	m_wndItemCtrlYou.Create( WLVS_ICON|WBS_NODRAWFRAME, CRect(   5, 25 + 15,   5 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 ), this, 10001 );
	m_wndItemCtrlI.  Create( WLVS_ICON|WBS_NODRAWFRAME, CRect( 175, 25 + 15, 175 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15 ), this, 10002 );

	m_nGoldI = 0;
	m_nGoldYou = 0;

	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();

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

//sun: 11, <레벨??> 삭제, 채팅창 수정, 페냐거래 취소버튼 삭제
	CWndButton* pClearButton = (CWndButton*)GetDlgItem(WIDC_CLEAR);
	if(pClearButton)
	{
		pClearButton->EnableWindow(FALSE);
		pClearButton->SetVisible(FALSE);
	}

//sun: 11, 주머니
	if(GetWndBase( APP_BAG_EX )) 
		GetWndBase( APP_BAG_EX )->Destroy();

	if( GetWndBase(APP_WEBBOX) 
		|| g_WndMng.m_pWndShop 
		||g_WndMng.m_pWndShop_Cart
		|| g_WndMng.m_pWndBank 
		|| g_WndMng.m_pWndGuildBank 
		|| g_WndMng.GetWndVendorBase() 
		|| g_WndMng.m_pWndTreasurechest
		|| g_WndMng.m_pWndBarunaNPC )
	{
		Destroy();
		return;
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover	= g_pPlayer->m_vtInfo.GetOther();
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
	if( message == WIN_ITEMDROP && ( nID == 10001 || nID == 10002 ) )
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
			FLItemBase* pItemBase = g_pPlayer->GetItemId( g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwId );
			g_WndMng.m_pWndTradeGold->m_dwGold = ( (FLItemElem*)pItemBase )->m_nItemNum;
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
				//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
				g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEMONEY ) );
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
			//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
			g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEITEM ) );
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
		{
			if( nID == WIDC_OK_I )	// 확인
			{
				g_DPlay.SendTradeOk();
			}
			else if( nID == WIDC_CANCEL || nID == WTBID_CLOSE ) // 취소 
			{
				g_DPlay.SendTradeCancel();
				return( TRUE );
			}
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

	m_bVBHPGauge = TRUE;
	m_bVBMPGauge = TRUE;
	m_bVBFPGauge = TRUE;
	
	m_bVBAEXPGauge = TRUE;
	m_pVBAEXPGauge = NULL;

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
		nWidth	= (int)( ( (float) nWidthClient / pMover->GetMaxExp1() ) * pMover->GetExp1() );

		if( m_nEXPWidth != nWidth ) 
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;//rect.SetRect( 105, 48, 105 + nWidthClient, 48 + 8 );
			rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x847070ff, m_pVBEXPGauge, &m_texGauFillNormal );
		}

		if( m_nPXPWidth != GuildHouse->m_nExtraExp )
		{
			m_nPXPWidth = GuildHouse->m_nExtraExp;
			rect = lpPxp->rect;
			rectTemp = rect;
			
			if( 0 < m_nPXPWidth && m_nPXPWidth < 9 )
				m_nPXPWidth = 9;
			rectTemp.right = rectTemp.left + m_nPXPWidth;
			ClientToScreen( rectTemp );
			m_bVBAEXPGauge = m_pTheme->MakeGaugeVertex( m_pApp->m_pd3dDevice, &rectTemp, 0x84e6ce19, m_pVBAEXPGauge, &m_texGauFillSmall );
		}
	}
}
void CWndStatus::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();

	if( m_pTexture && g_pPlayer )
	{
		RenderWnd();

		if( IsWndStyle( WBS_CAPTION ) )	
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
			p2DRender->TextOut( 10, 4, m_strTitle, m_dwColor );

			char szNameLevel[128] = { NULL, };
			FLSPrintf( szNameLevel, _countof( szNameLevel ), prj.GetText( TID_GAME_WND_STATUS_PLAYER_INFORMATION ), g_pPlayer->GetName(), g_pPlayer->GetLevel() );
			SetTitle( szNameLevel );
			
			p2DRender->SetFont( pOldFont );
		}				
	}
	else
	{
		m_pTheme->RenderWndBaseFrame( p2DRender, &rect );

		if( IsWndStyle( WBS_CAPTION ) )
		{
			rect.bottom = 21;
			m_pTheme->RenderWndBaseTitleBar( p2DRender, &rect, 17, 7, m_strTitle, m_dwColor );
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
	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM6 );

	LPWNDCTRL lpAExp = GetWndCtrl( WIDC_CUSTOM5 );
	
	if( pMover )
	{
		MakeGaugeVertex();

		if( m_bVBHPGauge )
		{
			m_bHPVisible = TRUE;
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

//sun: 8, // 8차 스킬경험치다운변경
		if( m_bVBEXPGauge )
		{
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillNormal );
		}
		if( m_bExpVisible )
		{
		}

		if( m_bVBAEXPGauge )
			m_pTheme->RenderGauge( p2DRender->m_pd3dDevice, m_pVBAEXPGauge, &m_texGauFillSmall );
		

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

	int nMaxHeight = p2DRender->m_pFont->GetMaxHeight();
	int nTopGap = 0;
	if( g_xFlyffConfig->GetMainLanguage() == LANG_FRE || g_xFlyffConfig->GetMainLanguage() == LANG_GER )
	{
		nTopGap = 2;
	}
	else
	{
		if(nMaxHeight > 14)
		{
			nTopGap = nMaxHeight - 14;
		}
	}

//BEGIN cmt-- _SUN_LOCALIZE_WNDSTATUS
//	static const int INITIAL_TEXT_REVISION_INTERVAL = 24;
//	p2DRender->TextOut( 
//		lpHP->rect.left - INITIAL_TEXT_REVISION_INTERVAL, 
//		lpHP->rect.top - nTopGap, 
//		prj.GetText( TID_GAME_WND_STATUS_TEXT_HP ), 
//		prj.GetTextColor( TID_GAME_WND_STATUS_TEXT_HP ), 
//		0xff000000 );
//	p2DRender->TextOut( 
//		lpMP->rect.left - INITIAL_TEXT_REVISION_INTERVAL, 
//		lpMP->rect.top - nTopGap, 
//		prj.GetText( TID_GAME_WND_STATUS_TEXT_MP ), 
//		prj.GetTextColor( TID_GAME_WND_STATUS_TEXT_MP ), 
//		0xff000000 );
//	p2DRender->TextOut( 
//		lpFP->rect.left - INITIAL_TEXT_REVISION_INTERVAL, 
//		lpFP->rect.top - nTopGap, 
//		prj.GetText( TID_GAME_WND_STATUS_TEXT_FP ), 
//		prj.GetTextColor( TID_GAME_WND_STATUS_TEXT_FP ), 
//		0xff000000 );
//	p2DRender->TextOut( 
//		lpExp->rect.left - INITIAL_TEXT_REVISION_INTERVAL, 
//		lpExp->rect.top - nTopGap, 
//		prj.GetText( TID_GAME_WND_STATUS_TEXT_EXP ), 
//		prj.GetTextColor( TID_GAME_WND_STATUS_TEXT_EXP ), 
//		0xff000000 );
//
//
//	p2DRender->TextOut( 
//		lpAExp->rect.left - INITIAL_TEXT_REVISION_INTERVAL, 
//		lpAExp->rect.top - nTopGap, 
//		prj.GetText( TID_GAME_WND_STATUS_TEXT_BT ), 
//		prj.GetTextColor( TID_GAME_WND_STATUS_TEXT_BT ), 
//		0xff000000 );
//
//END cmt--

	if( m_nDisplay != 0 )
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255 );
		//p2DRender->SetFont( m_pTheme->m_pFontWndTitle );

		char cbufHp[16] = {0,};
		char cbufMp[16] = {0,};
		char cbufFp[16] = {0,};
		char cbufExp[16] = {0,};
		char cbufPxp[16] = {0,};
		
		int nCharHP, nCharMP, nCharFP, nCharEXP; //, nCharPXP;
		CSize size = p2DRender->m_pFont->GetTextExtent("8");

		if(m_nDisplay == 1)
		{
			FLSPrintf(cbufHp, _countof( cbufHp ), "%d%%", pMover->GetHitPointPercent());
			FLSPrintf(cbufMp, _countof( cbufMp ), "%d%%", pMover->GetManaPointPercent());
			FLSPrintf(cbufFp, _countof( cbufFp ), "%d%%", pMover->GetFatiguePointPercent());

			p2DRender->TextOut( lpHP->rect.right - 90, lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - 90, lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - 90, lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );

			nCharHP = FLSPrintf(cbufHp, _countof( cbufHp ), prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetHitPoint());
			nCharMP = FLSPrintf(cbufMp, _countof( cbufMp ), prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetManaPoint());
			nCharFP = FLSPrintf(cbufFp, _countof( cbufFp ), prj.GetText( TID_GAME_WND_STATUS_PERCENT_POINT ), pMover->GetFatiguePoint());

			int x = lpHP->rect.right-17; // -40
			p2DRender->TextOut( x - (int)(nCharHP*5.8f) , lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharMP*5.8f), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(nCharFP*5.8f), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
		}
		else
		{
			nCharHP = FLSPrintf(cbufHp, _countof( cbufHp ), "%d", pMover->GetHitPoint());
			nCharMP = FLSPrintf(cbufMp, _countof( cbufMp ), "%d", pMover->GetManaPoint());
			nCharFP = FLSPrintf(cbufFp, _countof( cbufFp ), "%d", pMover->GetFatiguePoint());

			int x = lpHP->rect.right - 82;
			p2DRender->TextOut( x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
			
			nCharHP = FLSPrintf(cbufHp, _countof( cbufHp ), "%d", pMover->GetMaxHitPoint());
			nCharMP = FLSPrintf(cbufMp, _countof( cbufMp ), "%d", pMover->GetMaxManaPoint());
			nCharFP = FLSPrintf(cbufFp, _countof( cbufFp ), "%d", pMover->GetMaxFatiguePoint());
			
			x = lpHP->rect.right - 30;
			p2DRender->TextOut( x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000 );
			p2DRender->TextOut( x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000 );
				
			//LPWNDCTRL lpHP = GetWndCtrl( WIDC_CUSTOM1 );
			int nWidthCustom = lpHP->rect.Width();
			int nGap = (int)( ((nWidthCustom / 2.0f) + (size.cx / 2.0f)) );

			p2DRender->TextOut( lpHP->rect.right - nGap, lpHP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpMP->rect.right - nGap, lpMP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
			p2DRender->TextOut( lpFP->rect.right - nGap, lpFP->rect.top - nTopGap, "/", dwColor, 0xff000000 );
		}
		EXPINTEGER	nExpResult = pMover->GetExp1() * (EXPINTEGER)10000 / pMover->GetMaxExp1();
		float fExp = (float)nExpResult / 100.0f;
		if( fExp >= 99.99f )
			nCharEXP = FLSPrintf( cbufExp, _countof( cbufExp ), "99.99%%" );		// sprintf함수 내부에서 반올림되어 100.00으로 표시되는 것을 막기 위해서 
		else
			nCharEXP = FLSPrintf( cbufExp, _countof( cbufExp ), "%.2f%%", fExp );

		int x = lpHP->rect.right-7; // -40
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );

		nCharEXP = FLSPrintf( cbufExp, _countof( cbufExp ), "%d%%", GuildHouse->m_nExtraExp );
		p2DRender->TextOut( x - (int)(nCharEXP*5.8f), lpAExp->rect.top - 0, cbufExp, dwColor, 0xff000000 );
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
/*
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4.0f, fAspect, CWorld::m_fNearPlane - 0.01f, CWorld::m_fFarPlane );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
*/
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

	{	
		D3DXVECTOR3 vecLookAt( 0.0f, -0.0f, 3.0f );
		D3DXVECTOR3 vecPos(  0.0f, 0.7f, -3.5f );
		D3DXMatrixLookAtLH( &matView, &vecPos, &vecLookAt, &D3DXVECTOR3(0.0f,1.0f,0.0f) );
		pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

		D3DXMatrixScaling(&matScale, 6.0f, 6.0f, 6.0f);
		D3DXMatrixTranslation(&matTrans,0.0f,-7.8f,0.0f);

		D3DXMatrixRotationY( &matRot1, D3DXToRadian( -20 ) );
	}
	
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
			pElem->m_pObject3D->m_fAmbient[0] = g_pPlayer->m_fHairColorR; // / (CWorld::m_light.Diffuse.r*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[1] = g_pPlayer->m_fHairColorG; // / (CWorld::m_light.Diffuse.g*g_fHairLight);
			pElem->m_pObject3D->m_fAmbient[2] = g_pPlayer->m_fHairColorB; // / (CWorld::m_light.Diffuse.b*g_fHairLight);
		}

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

	if( g_pPlayer )
		g_pPlayer->OverCoatItemRenderCheck(g_pBipedMesh);
	int nVisibleParts = PARTS_HAT;
	int nInVisibleParts = PARTS_CAP;
	if(g_pPlayer && !g_pPlayer->IsOptionRenderCostume(0) )
	{
		nVisibleParts = PARTS_CAP;
		nInVisibleParts = 0;
	}		
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		O3D_ELEMENT* pElement   = NULL;
		FLItemElem* pItemElem	= g_pPlayer->GetEquipItem( nInVisibleParts );
		pElement = g_pBipedMesh->GetParts(PARTS_HAIR);
		if( pElement )
			pElement->m_nEffect &= ~XE_HIDE;
		if( pItemElem )
		{
			PT_ITEM_SPEC pItemProp = pItemElem->GetProp();

			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			FLItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( nVisibleParts );

			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				//외형변경이 아닌경우만 적용시켜야함.
				if( 0 == pItemElem->GetLooksChangeItemID() && NULL == pItemElemOvercoat )	//gmpbigsun( 20120604 ), #24885 : 얼굴사라짐 버그수정
					pElement = g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}
			/*else
			{
				pElement = g_pBipedMesh->GetParts(PARTS_HAIR);
				if( pElement )
						pElement->m_nEffect &= ~XE_HIDE;
			}*/

			
			//gmpbigsun( 20111212 ) : 기획요청 CAP룰
			if( NULL == pItemElemOvercoat )
			{
				pItemElemOvercoat = pItemElem;	
			}
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( FLItemElem::expired ) )
				{
					PT_ITEM_SPEC pItemPropOC = NULL;
					
					//외형변경인 경우 해당아이템으로!
					if( 0 != pItemElemOvercoat->GetLooksChangeItemID() )
						pItemPropOC = g_xSpecManager->GetSpecItem( pItemElemOvercoat->GetLooksChangeItemID() );
					else 
						pItemPropOC = pItemElemOvercoat->GetProp();
					
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
				//else
				//{
				//	//g_pBipedMesh->SetEffectOff(PARTS_HAIR, XE_HIDE );
				//	if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
				//	{
				//		g_pBipedMesh->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
				//	}
				//	
				//	g_pBipedMesh->SetEffectOff(PARTS_HEAD, XE_HIDE );					
				//}
			}
						
			
		}
		else
		{
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			FLItemElem* pItemElemOvercoat	= g_pPlayer->GetEquipItem( nVisibleParts );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( FLItemElem::expired ) )
				{					
					PT_ITEM_SPEC pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							g_pBipedMesh->SetEffect(PARTS_HAIR, XE_HIDE );
						
						g_pBipedMesh->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}
							
		}

//gmpbigsun : 윈도 페이스 고정라이트
	::SetLight( FALSE );
 	::SetFog( FALSE );
 	SetDiffuse( 1.0f, 1.0f, 1.0f );
 	SetAmbient( 1.0f, 1.0f, 1.0f );

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
		m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBAEXPGauge, NULL );

		m_nHPWidth = -1;
		m_nMPWidth = -1;
		m_nFPWidth = -1;
		m_nEXPWidth = -1;
		m_nPXPWidth = -1;
	}
	
	return S_OK;
}
HRESULT CWndStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBHPGauge );
    SAFE_RELEASE( m_pVBMPGauge );
    SAFE_RELEASE( m_pVBFPGauge );
    SAFE_RELEASE( m_pVBEXPGauge );
	SAFE_RELEASE( m_pVBAEXPGauge );

	return S_OK;
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
	CWndNeuz::OnInitialUpdate();
	
	RestoreDeviceObjects();
	//m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( DRAWVERTEX ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauEmptySmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "GauEmptyNormal.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillSmall.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	
	//m_texGauEmptySmall  
	//m_texGauFillNormal  
	//m_texGauFillSmall   
		
	// 장착, 게이지에 나올 캐릭터 오브젝트 설정 
	if( g_pBipedMesh == NULL )
	{
		int nMover = (g_pPlayer->GetSex() == SEX_MALE ? OBJECT_INDEX( 11, MI_MALE ) : OBJECT_INDEX( 12, MI_FEMALE ));
		g_pBipedMesh = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE );
		prj.m_modelMng.LoadMotion( g_pBipedMesh,  OT_MOVER, nMover, 0 );
		CMover::UpdateParts( g_pPlayer->GetSex(), 
			g_pPlayer->m_dwSkinSet, 
			g_pPlayer->m_dwFace, 
			g_pPlayer->m_dwHairMesh, 
			g_pPlayer->m_dwHeadMesh, 
			g_pPlayer->m_aEquipInfo, 
			g_pBipedMesh, 
			&g_pPlayer->m_Inventory );

		
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( rectRoot.left, rectRoot.top );
	Move( point );

	m_bLocalizeNoTileIMG = TRUE;	//_SUN_LOCALIZE_WNDSTATUS

	// 19차를 위해서 임시로 그리지 않는다.
//	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
//		SetVisible(FALSE);//05
}

BOOL CWndStatus::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	return InitDialog( g_Neuz.GetSafeHwnd(), APP_STATUS1 );
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

void CWndStatus::OnMouseWndSurface( CPoint point )
{
	static LPWNDCTRL lpPxp  = GetWndCtrl( WIDC_CUSTOM5 );
	BOOL bInPoint = FALSE;
	if( lpPxp )
		bInPoint = PtInRect( &lpPxp->rect, point );
	else return;

	if( bInPoint )	
	{
		CRect DrawRect = lpPxp->rect;
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		g_toolTip.PutToolTip( 100000, GETTEXT( TID_TOOLTIP_RESTPOINT_GAGE ), DrawRect, point2 );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 퀘스트 아이템 정보
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndQuestItemInfo::CWndQuestItemInfo() : m_pItemBase(NULL)
{
}

CWndQuestItemInfo::~CWndQuestItemInfo()
{
}

BOOL CWndQuestItemInfo::Initialize( CWndBase* pWndParent, FLItemBase* pItemBase )
{
	return Create( pItemBase, APP_QUESTITEM_INFO, pWndParent );//RUE;
}
BOOL CWndQuestItemInfo::Create( FLItemBase* pItemBase, UINT nID, CWndBase* pWndParent )
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
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
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 스타일
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
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
*/
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

//EMPTY_WNDCLASSMEMBER( CWndFriend, _T( "친구" ) )

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

//EMPTY_WNDCLASSMEMBER( CWndClub, _T( "클럽" ) )

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

//EMPTY_WNDCLASSMEMBER( CWndEMail, _T( "이메일" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndBBS, _T( "게시판" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchPeople, _T( "사람찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchGuild, _T( "길드찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchClub, _T( "클럽찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSearchShop, _T( "상점찾기" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


//EMPTY_WNDCLASSMEMBER( CWndInfoEvent, _T( "이벤트" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndInfoVersion, _T( "버전" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndInfoSchedule, _T( "개발 스케쥴" ) )

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

//EMPTY_WNDCLASSMEMBER( CWndJoinVote, _T( "전자투표" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndJoinEvent, _T( "이벤트 참여 신청" ) )

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 테마
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//EMPTY_WNDCLASSMEMBER( CWndSequence, _T( "시퀀스" ) )


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 카드 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWndLogOut : 계정 화면으로, 캐릭터 선택 화면으로 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndLogOut::CWndLogOut(LPCSTR lpszMessage, int nType) 
{
	m_nType = nType;
	m_lpszMessage = lpszMessage;
}
CWndLogOut::~CWndLogOut() 
{

}

BOOL CWndLogOut::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
/*
	Create( _T( "매시지 박스" ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
	m_wndText.SetString( _T( "접속을 종료하시겠습니까?" ) );
*/
	switch(m_nType)
	{
	case LOGOUT_TYPE1:
		{
			Create( _T( prj.GetText(TID_DIAG_0068) ), MB_OKCANCEL, rect, APP_MESSAGEBOX );//dwWndId );
			m_wndText.SetString( _T( prj.GetText(TID_DIAG_0069) ) );
			m_wndText.ResetString();
		}
		break;
	case LOGOUT_TYPE2:
		{
			Create( "", MB_OK, rect, APP_MESSAGEBOX );//dwWndId );
			m_wndText.SetString( m_lpszMessage );
			m_wndText.ResetString();
		}
		break;
	}
	
	
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
				if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
				{
					if( g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}

				g_Neuz.ResetStaticValues( );

				if( g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		

				if( g_pPlayer->IsRegionAttr( RA_SAFETY ) || g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
						g_WndMng.m_pLogOutWaitting->Initialize( NULL );	// 康

						g_WndMng.m_pLogOutWaitting->SetIsLogOut(TRUE);
						SetVisible(FALSE);
					}
				}

				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


CWndLogOutWaitting::CWndLogOutWaitting() 
{
	m_nMsgType = MSGTYPE_NONE;
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
	DWORD dwCurrentTime = g_tmCurrent;
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	DWORD dwEndWaitTime = dwTime - dwCurrentTime;
	dwEndWaitTime /= 1000;

	CString str;
	
	if( MSGTYPE_NONE == m_nMsgType )
	{
		str.Format( prj.GetText( TID_PK_TIME_END ), dwEndWaitTime );	
	}
	else if( MSGTYPE_SHUTDOWN == m_nMsgType )
	{
		//str.Format( prj.GetText( TID_MMI_SHUTDOWNNOTICE02 ) );                                             
		str.Format( "셧다운 제도로 인해 종료합니다 : %d초", dwEndWaitTime );
	}
	
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
	
	dwTime = g_tmCurrent+SEC(TIMEWAIT_CLOSE);

	// 게이지 위쪽으로 나오게 하기
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );

	// 종료 패킷 서버 전송
	g_DPlay.SendReqLeave();	
}

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
	MoveParentCenter();	//gmpbigsun(20101021)

	m_bFlag = FALSE;
	
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
				if( m_bFlag )
					return TRUE;
                
				m_bFlag = TRUE;

				//_SUN_JAPAN_HANGAME_UI
				if( 0 && !g_pPlayer )
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
					break;
				}

				if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
				{
					if( g_pPlayer && g_pPlayer->GetSex() == SEX_MALE )
						PLAYSND( "VocMale-logout.wav" );
					else
						PLAYSND( "VocFemale-logout.wav" );
				}

				if( g_pPlayer && g_pPlayer->EndMotion() )
					g_Neuz.m_timerQuit.Set( SEC( 3 ) );		
				
				if( g_pPlayer && g_pPlayer->IsRegionAttr( RA_SAFETY ))
				{
					::PostMessage( g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0 );
				}
				else
				{
					if( g_WndMng.m_pLogOutWaitting == NULL )
					{
						g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
						g_WndMng.m_pLogOutWaitting->Initialize( NULL );	// 康

						g_WndMng.m_pLogOutWaitting->SetIsLogOut(FALSE);
						SetVisible(FALSE);
					}
				}

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
	m_pShop = NULL;		//sun: 9차 추가분
	m_pReChallenge = NULL;

	m_bColosseumAllDie = FALSE;
	m_fTime = 0.0f;
}
CWndRevival::~CWndRevival() 
{
} 
void CWndRevival::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRevival::OnInitialUpdate() 
{
	CWndNeuz::OnInitialUpdate(); 

	m_pLodeLight = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODELIGHT );
	m_pLodeStar = (CWndButton*)GetDlgItem( WIDC_REVIVAL_TO_LODESTAR );
	m_pRevival = (CWndButton*)GetDlgItem( WIDC_REVIVAL_STAND );
	m_pShop = (CWndButton*)GetDlgItem( WIDC_REVIVAL_SHOP );		//sun: 9차 추가분
	m_pReChallenge = (CWndButton*)GetDlgItem( WIDC_REVIVAL_RECHALLENGE );	

	m_pLodeLight->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODELIGHT) );//"로드라이트는 여행자가 직접 지정한 부활 위치입니다." );
	m_pLodeStar->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_LODESTAR) );//"로드스타는 인근 마을의 부활 위치입니다." );
	m_pRevival->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_OGNPOINT) );
	m_pShop->m_strToolTip = _T( prj.GetText(TID_GAME_TOOLTIP_SHOP) );		//sun: 9차 추가분

	BOOL	bArena	= g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena(); 
	BOOL	bRevivalItem = g_pPlayer->m_Inventory.GetAtByItemId( ITEM_INDEX( 10431, II_SYS_SYS_SCR_RESURRECTION ) ) || g_pPlayer->m_Inventory.GetAtByItemId( ITEM_INDEX( 25196, II_SYS_SYS_SCR_RESURRECTION_02 ) );
	if ( !bRevivalItem || bArena )		// 부활 두루마리 아이템이 없거나 아레나일때 제자리 부활 버튼을 disable시킵니다.
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );

	CWndWebBox* pWndWebBox = (CWndWebBox*)g_WndMng.GetApplet( APP_WEBBOX );

	if(pWndWebBox)
		pWndWebBox->Destroy();

	CWndHelperWebBox* pWndHelperWebBox = ( CWndHelperWebBox* )g_WndMng.GetApplet( APP_WEBBOX2 );

	if( pWndHelperWebBox )
		pWndHelperWebBox->Destroy();

	m_wndTitleBar.SetVisible( FALSE );

//sun: 9차 추가분
	if( m_pLodeLight )
	{
		CRect RevivalRect = m_pRevival->GetWindowRect(TRUE);
		CRect LodeLightRect = m_pLodeLight->GetWindowRect(TRUE);
		m_pRevival->SetWndRect( LodeLightRect, TRUE );
		m_pShop->SetWndRect( RevivalRect, TRUE );

		m_pLodeLight->EnableWindow( FALSE );
		m_pLodeLight->SetVisible( FALSE );		
	}
	
	//콜로세움, 재도전 버튼은 평상시 나오면 안되고 콜로세움 월드에서만 보여야 한다.
	if( g_pPlayer )
	{
		CWorld* pWorld = g_pPlayer->GetWorld( );
		if( pWorld )
		{
			if( pWorld->GetID() == WI_WORLD_COLOSSEUM )
			{
				CRect rect = this->GetWndRect( );
				this->SetWndSize( rect.Width(), rect.Height() + 20 );

				if( m_pReChallenge )
				{
					CRect chRect = m_pReChallenge->GetWndRect( );
					m_pReChallenge->Move( chRect.left, chRect.top - 18 );
					m_pReChallenge->SetVisible( TRUE );

					m_pReChallenge->EnableWindow( FALSE );
				}
			}
		}
	}

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
void CWndRevival::EnableButton( int nButton, BOOL bEnable )
{
	switch( nButton )
	{
	case WIDC_REVIVAL_TO_LODELIGHT:
		{
			m_pLodeLight->EnableWindow(bEnable);
			break;
		}
	case WIDC_REVIVAL_TO_LODESTAR:
		{
			m_pLodeStar->EnableWindow(bEnable);
			break;
		}
	}
}

void CWndRevival::SetRemainTime( float fTime )
{
	if( fTime <= 0.0f )
		return;

	m_fTime = fTime;

	CRect rect = this->GetWndRect( );
	this->SetWndSize( rect.Width(), rect.Height() + 15 );

	if( m_pReChallenge )
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		if( !pWndStatic )
			return;

		CRect chRect = m_pReChallenge->GetWndRect( );
		pWndStatic->Move( chRect.left, chRect.bottom + 5 );
		pWndStatic->SetVisible( TRUE );
	}
}

BOOL CWndRevival::Process()
{
	// 음 기존코드, 부활유료아이템이 있을경우만 활성화
	BOOL	bArena	= g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena(); 
	BOOL	bRevivalItem = g_pPlayer->m_Inventory.GetAtByItemId( ITEM_INDEX( 10431, II_SYS_SYS_SCR_RESURRECTION ) ) || g_pPlayer->m_Inventory.GetAtByItemId( ITEM_INDEX( 25196, II_SYS_SYS_SCR_RESURRECTION_02 ) );

	if( !bRevivalItem || bArena )
		m_pRevival->EnableWindow( FALSE );
	else
		m_pRevival->EnableWindow( TRUE );

	//콜로세움으로 인한 추가코드
	if( m_fTime > 0.0f )
	{
		m_fTime -= _GetDeltaTimeSec( );

		int nTotalSec = (int)m_fTime;
		FLASSERT( nTotalSec < ( 24 * 60 * 60 ) );		//하루를 넘길수없다고 가정함.

		DWORD dwColor = m_dwColor;
		if( nTotalSec < 11 )
			dwColor = 0xffff1111;

		int nHour, nMin, nSec;
		_GetHMS_bySec( nTotalSec, nHour, nMin, nSec );

		CString str;
		str.Format( "%s %02d:%02d", prj.GetText( TID_COLOSSEUM_OUTTIMEMASSAGE ), nMin, nSec );	//남은시간

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		if( pWndStatic )
		{
			pWndStatic->SetTileColor( dwColor );
			pWndStatic->SetTitle( str );
		}
	}

	//콜로세움 : 파티장이고, 메르데르의 가보 아이템이 있다면 재도전 버튼 활성화
	CWorld* pWorld = g_pPlayer->GetWorld();
	if( pWorld )
	{
		if( pWorld->GetID() == WI_WORLD_COLOSSEUM && m_bColosseumAllDie )
		{
			//콜로세움 전원사망일경우 부활아이템 사용금지
			if( m_pRevival )
				m_pRevival->EnableWindow( FALSE );

			if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
			{
				FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtByItemKind3( IK3_COLOSSEUM_RETRY );
				if( pItem )
				{
					//메르데르의 가보가 있다면 재도전 활성
					m_pReChallenge->EnableWindow( TRUE );
				}
				else
					m_pReChallenge->EnableWindow( FALSE );
				
			}
			else
			{
				//파티원들은 재도전 버튼 비활성
				if( m_pReChallenge )
					m_pReChallenge->EnableWindow( FALSE );
			}
		}
		else
		{
			m_pReChallenge->EnableWindow( FALSE );
		}
	}

	return TRUE;
}

BOOL CWndRevival::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bClose = TRUE;
	switch( nID )
	{
		case WIDC_REVIVAL_TO_LODELIGHT:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODELIGHT );
				
				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );

				if( pWndBase )
					g_DPlay.SendReqRevivalBySkill( false );

				break;
			}
		case WIDC_REVIVAL_TO_LODESTAR:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_LODESTAR );

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendReqRevivalBySkill( false );

				break;
			}
		case WIDC_REVIVAL_STAND:
			{
				g_DPlay.SendHdr( PACKETTYPE_REVIVAL_TO_CURRENT_POS );

				CWndBase* pWndBase = g_WndMng.GetWndBase( APP_RESURRECTION );
				
				if( pWndBase )
					g_DPlay.SendReqRevivalBySkill( false );

				break;
			}
//sun: 9차 추가분
		case WIDC_REVIVAL_SHOP:
			{
				bClose = FALSE;
				g_WndMng.ObjectExecutor( SHORTCUT_APPLET, APP_WEBBOX );
				break;
			}

		//콜로세움
		case WIDC_REVIVAL_RECHALLENGE:
			{
				bClose = TRUE;
				//버튼 비활성화 및 Sendpacket, 응답패킷 받아서 성공일때 Destroy!
				
				//IK3_COLOSSEUM_RETRY

				FLItemElem* pItem = g_pPlayer->m_Inventory.GetAtByItemKind3( IK3_COLOSSEUM_RETRY );
				if( pItem )
					g_DPlay.SendColosseumRetry( pItem->m_dwObjId );
				
				break;
			}
	}	
	if( bClose )
		Destroy();
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
	return CWndMessageBox::Initialize( prj.GetText(TID_PK_INPUT_TIMEWARN), 
	                                   pWndParent, 
									   MB_OKCANCEL );

}

BOOL CWantedMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendWantedGold( m_nGold, m_strMsg );
			Destroy();
		}
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
	if( szMsg[0] == '\0' )
	{
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
		FLSPrintf( szWarning, _countof( szWarning ), prj.GetText(TID_PK_MONEY_RANGE), MIN_INPUT_REWARD, MAX_INPUT_REWARD );		
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
	FLStrcpy( m_szName, _countof( m_szName ), szName );
	
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
				if( strlen(m_szName) > 0 )
				{
					g_DPlay.SendWantedInfo(m_szName);
				}
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
		m_pWantedConfirm->SetInfo( m_aList[m_nSelect].szName, (int)( m_aList[m_nSelect].nGold ) );
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
		FLERROR_LOG( PROGRAM_NAME, _T( "range over" ) );
		return;
	}

	FLStrcpy( m_aList[m_nMax].szName, _countof( m_aList[m_nMax].szName ), szName );
	m_aList[m_nMax].nGold = nGold;

	CTime cTime( (time_t)nTime );		
	SYSTEMTIME st;
	st.wYear= cTime.GetYear(); 
	st.wMonth= cTime.GetMonth(); 
	st.wDay= cTime.GetDay();  

	if( GetDateFormat( LOCALE_USER_DEFAULT, 0, &st, NULL, m_aList[m_nMax].szDate, sizeof(m_aList[m_nMax].szDate) ) == 0 )
		m_aList[m_nMax].szDate[0] = '\0';

	FLStrcpy( m_aList[m_nMax].szMsg, _countof( m_aList[m_nMax].szMsg ), szMsg );
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
		FLTRACE_LOG( PROGRAM_NAME, _T( "aa" ) );
	}

	for( int i=nBase; i<nBase + MAX_WANTED_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		FLSPrintf( szNum, _countof( szNum ), "%3d", i+1 );
		FLSPrintf( szGold, _countof( szGold ), "%I64d", m_aList[i].nGold );
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
	
	m_wndTitleBar.SetVisible( FALSE );

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
			g_DPlay.SendReqRevivalBySkill( true );
			Destroy();
		}
		break;
	case  10000:
	case  WIDC_BUTTON2:
		{
			g_DPlay.SendReqRevivalBySkill( false );
			Destroy();
		}
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


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

	CWndButton* pOk   = (CWndButton*)GetDlgItem( WIDC_BUTTON_OK );
	pOk->SetDefault( TRUE );

	// 윈도를 중앙으로 옮기는 부분.
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
			g_WndMng.GetWndBase( APP_TRADE ) ||
			g_WndMng.GetWndBase( APP_SHOP_CART_ ) )
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
			FLItemElem *pItemUsing = g_pPlayer->m_Inventory.GetAtId( m_dwObjId );
			if( pItemUsing->IsFlag( FLItemElem::is_using ) != FALSE )
			{
				if(_GetContentState(CT_TICKETITEM) == CS_VER1)
				{
					FLPacketChargeZoneTicketLeaveReq	pPacket;
					pPacket.dwTicketObjID = m_dwObjId;
					g_DPlay.SendPacket(&pPacket);
				}			
			}	
			else if( m_dwObjId == ITEM_INDEX( 10425, II_SYS_SYS_SCR_CHACLA ) )
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
//sun: 9차 전승관련 Clienet
			else if( m_dwCtrlId == ITEM_INDEX( 21030, II_SYS_SYS_SCR_PET_TAMER_MISTAKE ) )
			{
				g_DPlay.SendPetTamerMistake(m_dwObjId);
			}
			else
			{
				FLItemElem* pItem	= static_cast<FLItemElem*>( g_pPlayer->GetItemId( m_dwObjId ) );
				if( pItem )
				{
					g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_dwObjId ), m_dwCtrlId, -1, FALSE );
				}
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
	MoveParentCenter();
} 

void CWndChangeClass1::SetJob()
{
	CWndButton* pWndButton;

	BOOL bJob[MAX_EXPERT - MAX_JOBBASE] = { 0 };
	//mem_set( bJob, 0, sizeof( bJob ) );
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
			g_DPlay.SendChangeJob( nJob, FALSE );
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
	MoveParentCenter();
} 
void CWndChangeClass2::SetJob()
{
	CWndButton* pWndButton;

	BOOL bJob[MAX_PROFESSIONAL - MAX_EXPERT] = { 0 };
	//mem_set( bJob, 0, sizeof( bJob ) );
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
			g_DPlay.SendChangeJob( nJob, FALSE );
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


void CWndInventory::RunUpgrade( FLItemBase* pItem )
{
	if( m_bIsUpgradeMode )
	{
		m_bIsUpgradeMode = FALSE;

		if( pItem == NULL || m_pUpgradeMaterialItem == NULL )
		{
			g_WndMng.PutString( prj.GetText( TID_UPGRADE_CANCLE ), NULL, prj.GetTextColor( TID_UPGRADE_CANCLE ) );
			BaseMouseCursor();
			return;
		}
		
		PT_ITEM_SPEC pItemProp = m_pUpgradeMaterialItem->GetProp();

		if( !pItemProp )
			return;

//sun: 12, 무기 피어싱
		if( pItemProp->dwItemKind3 == IK3_SOCKETCARD || pItemProp->dwItemKind3 == IK3_SOCKETCARD2
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
 || pItemProp->dwItemKind3 == IK3_SOCKETCARD3
#endif
)
		{
			if( !((FLItemElem*)pItem)->IsPierceAble( pItemProp->dwItemKind3 ) )
			{
				g_WndMng.PutString( prj.GetText( TID_PIERCING_POSSIBLE_ITEM ) );
				BaseMouseCursor();
				return;
			}
			
			int nCount = 0;
			for( size_t j = 0; j < ( (FLItemElem*)pItem )->GetGeneralPiercingSize(); j++ )
			{
				if( ( (FLItemElem*)pItem )->GetGeneralPiercingItemID( j ) != 0 )
					nCount++;
			}

			// 빈곳이 없으면 중단
			if( nCount == ( (FLItemElem*)pItem )->GetGeneralPiercingSize() )
			{
				g_WndMng.PutString( prj.GetText(TID_PIERCING_ERROR_NOPIERCING), NULL, prj.GetTextColor(TID_PIERCING_ERROR_NOPIERCING));
				BaseMouseCursor();
				return;
			}
		}
		else if( pItemProp->dwItemKind3 == IK3_ELECARD )
		{
			if( !FLItemElem::IsEleRefineryAble( pItem->GetProp()) )
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;
			}

			FLItemElem* pItemElem = (FLItemElem*)pItem;

			if( pItemElem->m_byItemResist != SAI79::NO_PROP && pItemElem->m_byItemResist != pItemProp->eItemType )
			{
				g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_TWOELEMENT ), NULL, prj.GetTextColor( TID_UPGRADE_ERROR_TWOELEMENT ) );
				BaseMouseCursor();
				return;
			}

			DWORD dwWantedCard	= 0;
			DWORD dwItemAO	= pItemElem->m_nResistAbilityOption;

//sun: 12, 각성, 축복 가능 파츠 추가
			switch( pItemProp->eItemType )
			{
			case SAI79::FIRE:
				dwWantedCard	= ITEM_INDEX( 3206, II_GEN_MAT_ELE_FLAME );	break;
			case SAI79::WATER:
				dwWantedCard	= ITEM_INDEX( 3211, II_GEN_MAT_ELE_RIVER );	break;
			case SAI79::ELECTRICITY:
				dwWantedCard	= ITEM_INDEX( 3216, II_GEN_MAT_ELE_GENERATOR );	break;
			case SAI79::EARTH:
				dwWantedCard	= ITEM_INDEX( 3221, II_GEN_MAT_ELE_DESERT );	break;
			case SAI79::WIND:
				dwWantedCard	= ITEM_INDEX( 3226, II_GEN_MAT_ELE_CYCLON ); break;
			default:
				dwWantedCard	= 0;	break;
			}
			if( pItemProp->dwID != dwWantedCard )
			{
				g_WndMng.PutString( prj.GetText( TID_UPGRADE_ERROR_WRONGUPLEVEL ), NULL, prj.GetTextColor( TID_UPGRADE_ERROR_WRONGUPLEVEL ) );
				BaseMouseCursor();
				return;					
			}
		}
//		else if( pItemProp->dwItemKind3 == IK3_ENCHANT )
		else if( pItemProp->dwItemKind3 == IK3_SMELT_ULTIMATE_MATERIAL
			|| pItemProp->dwItemKind3 == IK3_SMELT_GENERAL_MATERIAL
			)
		{
			if(m_pWndRemoveJewelConfirm != NULL)
			{
				if(m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId)
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
			}
			
			if(m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ))
			{
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if(pWndSmeltJewel != NULL)
				{
					if(pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
			
				if(pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
			}
			else if( !FLItemElem::IsDiceRefineryAble( pItem->GetProp()) )
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;
			}

			FLItemElem* pItemElem = (FLItemElem*)pItem;
		}
		else if( pItemProp->dwItemKind3 == IK3_RANDOM_SCROLL )
		{
			if( !( pItem->GetProp()->dwItemKind1 == IK1_WEAPON || pItem->GetProp()->dwItemKind2 == IK2_ARMOR || pItem->GetProp()->dwItemKind2 == IK2_ARMORETC ) )
			{
				BaseMouseCursor();
				g_WndMng.PutString( prj.GetText( TID_GAME_RANDOMSCROLL_ERROR ), NULL, prj.GetTextColor( TID_GAME_RANDOMSCROLL_ERROR ) );			
				return;	
			}
		}
		else if( IsNeedTarget( pItemProp ) )
		{
			m_pUpgradeItem	= pItem;
			m_dwEnchantWaitTime		= g_tmCurrent + ENCHANTWAITTIME;
			return;
		}

//sun: 9차 전승관련 Clienet // __CSC_VER9_1 //오리칼쿰, 문스톤 얼터멋 웨폰 제련 관련 변경.
//		else if( pItemProp->dwItemKind3 == IK3_PIERDICE )
		else if( pItemProp->dwItemKind3 == IK3_SMELT_ACCESSORY_MATERIAL )
		{
			if(m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ))
			{
//sun: 11, 채집 시스템
				if( ( static_cast<FLItemElem*>( pItem ) )->IsCollector( TRUE ) || pItem->GetProp()->dwItemKind2 == IK2_JEWELRY )
				{
					m_pUpgradeItem = pItem;
					m_dwEnchantWaitTime = g_tmCurrent + ENCHANTWAITTIME;
					return;
				}

//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
				BaseMouseCursor();
				return;			

				BOOL checkJewel = FALSE;
					
				for(int i=0; i<5; i++)
				{
//sun: 12, 무기 피어싱
					if(((FLItemElem*)pItem)->GetUltimatePiercingItemID( i ) != 0)
						checkJewel = TRUE;
				}
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase( APP_SMELT_JEWEL );
				if(pWndSmeltJewel != NULL)
				{
					if(pWndSmeltJewel->m_pItemElem && pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
				if(m_pWndRemoveJewelConfirm != NULL)
				{
					if(m_pWndRemoveJewelConfirm->m_pUpgradeItem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
				if( pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
				else
				{
					if(!checkJewel)
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
						BaseMouseCursor();
						return;
					}
				}
				
				if(m_pWndRemoveJewelConfirm == NULL)
				{
					m_pWndRemoveJewelConfirm = new CWndRemoveJewelConfirm;
					m_pWndRemoveJewelConfirm->Initialize(this);
					m_pWndRemoveJewelConfirm->SetItem(pItem);

					m_bRemoveJewel = TRUE;
					return;
				}
			}
		}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		else if( pItemProp->dwItemKind3 == IK3_SMELT_COSTUME_MATERIAL )
		{
			if(m_pUpgradeMaterialItem->m_dwItemId == ITEM_INDEX( 25334, II_GEN_MAT_LIGHTNINGSTONE ))
			{
				FLItemElem* pItemElem = (FLItemElem*)pItem;

				if( !pItemElem->GetProp()->IsCostumeEnhanceParts() || pItemElem->m_dwKeepTime != 0 )
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_NOTEQUALITEM ), NULL, prj.GetTextColor( TID_GAME_NOTEQUALITEM ) );
					BaseMouseCursor();
					return;
				}
			}
		}
#endif

		// 인첸트가 되는 아이템 - 방어구 등등
		m_pUpgradeItem = pItem;
		m_dwEnchantWaitTime = g_tmCurrent + ENCHANTWAITTIME;
	}
}

void CWndInventory::BaseMouseCursor()
{
	m_bIsUpgradeMode = FALSE;

	if( m_pSfxUpgrade )
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
}

void CWndInventory::UpdateParts()
{
	CMover::UpdateParts( g_pPlayer->GetSex(), 
						g_pPlayer->m_dwSkinSet, 
						g_pPlayer->m_dwFace, 
						g_pPlayer->m_dwHairMesh, 
						g_pPlayer->m_dwHeadMesh,
						g_pPlayer->m_aEquipInfo, 
						m_pModel, 
						&g_pPlayer->m_Inventory );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 스킬 재분배 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	m_wndTitleBar.SetVisible( FALSE );	
	pWndText->ResetString();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndReSkillControl::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESKILL_CONTROL1, 0, CPoint( 0, 0 ), pWndParent );
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
#ifdef __SKILL_UI16
		CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase( APP_SKILL4 );
#else //sun: 9차 전승관련 Clienet
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif //__SKILL_UI16
		
		if( pSkillTree )
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
	}
	else if( nID == WIDC_BUTTON2 )
	{
		SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
		g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
		g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
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
void CWndReSkillWarning::InitItem( BOOL bParentDestroy )
{
	m_bParentDestroy = bParentDestroy;
}
void CWndReSkillWarning::OnDestroy()
{
	if( m_bParentDestroy )
	{
#ifdef __SKILL_UI16
		CWndBase* pWndBase1 = ( CWndBase* )g_WndMng.GetWndBase( APP_SKILL4 );
#else //sun: 9차 전승관련 Clienet
		CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase( APP_SKILL3 );
#endif //__SKILL_UI16
		pWndBase1->Destroy();
	}
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
		pWndEdit->SetString( _T( prj.GetText( TID_GAME_SKILLLEVEL_CHANGE ) ) );
		pWndEdit->EnableWindow( FALSE );	
	}

	m_bParentDestroy = FALSE;

	// 윈도를 중앙으로 옮기는 부분.
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
#ifdef __SKILL_UI16
		CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase( APP_SKILL4 );
		g_WndMng.m_pWndWorld->GetAdvMgr()->DeleteButton(APP_SKILL4);
#else //sun: 9차 전승관련 Clienet
		CWndSkillTreeEx* pSkillTree = (CWndSkillTreeEx*)g_WndMng.GetWndBase( APP_SKILL3 );
		g_WndMng.m_pWndWorld->GetAdvMgr()->DeleteButton(APP_SKILL3);
#endif //__SKILL_UI16
		if( pSkillTree )
			g_DPlay.SendDoUseSkillPoint( pSkillTree->GetSkill() );

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
	
	if( m_nCurrSkillPoint < g_pPlayer->m_nSkillPoint )
		return TRUE;

	return FALSE;	
}

CWndPostItemWarning::CWndPostItemWarning() 
{ 
	m_nMailIndex = -1;
} 
CWndPostItemWarning::~CWndPostItemWarning() 
{ 
} 
void CWndPostItemWarning::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPostItemWarning::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndPostItemWarning::SetString( char* string )
{
	CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_CONTEXT );
	
	if( pWndEdit )
	{
		pWndEdit->SetString( string );
		pWndEdit->EnableWindow( FALSE );	
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPostItemWarning::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_QUEITMWARNING );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_QUEITMWARNING ); 
} 
BOOL CWndPostItemWarning::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostItemWarning::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPostItemWarning::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPostItemWarning::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPostItemWarning::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BTN_YES || message == EN_RETURN )
	{
		g_DPlay.SendQueryGetMailItem( m_nMailIndex );					
		Destroy();
	}
	else if( nID == WIDC_BTN_NO )
	{
		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 






CWndPost::CWndPost() 
{ 
	m_pWndMailRequestingBox = NULL;
} 
CWndPost::~CWndPost() 
{ 
	//SaveLastMailBox();
	CloseMailRequestingBox();
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

//sun: 11, 주머니
	if(GetWndBase( APP_BAG_EX )) 
		GetWndBase( APP_BAG_EX )->Destroy();

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

	pWndTabCtrl->SetCurSel(1);

	MoveParentCenter();	

	// 도착한 편지 목록 요청
	g_DPlay.SendQueryMailBox();	

	EnableWindow( FALSE );

	//CMailBox* pMailBox = CMailBox::GetInstance();
	//if( pMailBox )
	//{
	//	pMailBox->Clear();
	//}
	//LoadLastMailBox();
	//m_PostTabReceive.UpdateScroll();

	CloseMailRequestingBox();
	m_pWndMailRequestingBox = new CWndMailRequestingBox;
	m_pWndMailRequestingBox->Initialize();
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

void CWndPost::LoadLastMailBox( void )
{
	CString strFileName = _T( "" );
	if( g_pPlayer == NULL )
	{
		return;
	}
	strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
	FILE* fp = FLStdFileOpen( strFileName, "rb" );
	if( fp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "fp == NULL" ) );
		return;
	}

	while( feof( fp ) == 0 )
	{
		CMail* pMail = new CMail;
		fread( &( pMail->m_nGold ), sizeof( DWORD ), 1, fp );
		fread( &( pMail->m_byRead ), sizeof( BYTE ), 1, fp );
		fread( &( pMail->m_szTitle ), sizeof( char ), MAX_MAILTITLE, fp );
		CMailBox::GetInstance()->AddMail( pMail );
	}

	fclose( fp );
}

void CWndPost::SaveLastMailBox( void )
{
	CMailBox* pMailBox = CMailBox::GetInstance();
	if( pMailBox == NULL )
	{
		return;
	}

	CMailBox& MailBox = *pMailBox;
	if( MailBox.empty() == true )
	{
		CString strFileName = _T( "" );
		if( g_pPlayer == NULL )
		{
			return;
		}
		strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
		DeleteFile( strFileName );

		return;
	}

	CString strFileName = _T( "" );
	if( g_pPlayer == NULL )
	{
		return;
	}
	strFileName.Format( "%s_MailData.Temp", g_pPlayer->GetName() );
	//	100805	CSTRING GETBUFFER
	FILE* fp = FLStdFileOpen( strFileName.GetString(), "wb" );
	if( fp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "fp == NULL" ) );
		return;
	}

	for( size_t i = 0; i < MailBox.size(); ++i )
	{
		fwrite( &( MailBox[ i ]->m_nGold ), sizeof( DWORD ), 1, fp );
		fwrite( &( MailBox[ i ]->m_byRead ), sizeof( BYTE ), 1, fp );
		fwrite( &( MailBox[ i ]->m_szTitle ), sizeof( char ), MAX_MAILTITLE, fp );
	}

	fclose( fp );
}
void CWndPost::CloseMailRequestingBox( void )
{
	if( m_pWndMailRequestingBox )
	{
		SAFE_DELETE( m_pWndMailRequestingBox );
	}
}

void CWndPost::EnableWindow_Receive( BOOL bEnable )
{
	m_PostTabReceive.EnableWindow( bEnable );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CWndPostSendMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndPostSendMessageBox : public CWndMessageBox
{ 
public: 
	CWndText	m_wndText;
	
	DWORD		m_dwItemObjID;
	int			m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
	void		SetValue( DWORD dwItemObjID, int nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndPostSendMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CString str;
	str.Format( prj.GetText(TID_MAIL_SEND_CONFIRM), MAX_KEEP_MAX_DAY );
	
	return CWndMessageBox::Initialize( str, 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndPostSendMessageBox::SetValue( DWORD dwItemObjID, int nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText )
{
	m_dwItemObjID		= dwItemObjID;
	m_nItemNum	= nItemNum;
	FLStrcpy( m_szReceiver, _countof( m_szReceiver ), lpszReceiver );
	FLStrcpy( m_szTitle, _countof( m_szTitle ), lpszTitle );
	FLStrcpy( m_szText, _countof( m_szText ), lpszText );
	m_nGold		= nGold;
}

BOOL CWndPostSendMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 1 )
	{
		if( g_pPlayer )
		{
			g_DPlay.SendQueryPostMail( m_dwItemObjID, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText );			
			
			CWndPost * pWndPost = (CWndPost *)g_WndMng.GetWndBase( APP_POST );
			
			if( pWndPost )
			{
				pWndPost->m_PostTabSend.ClearData();
			}
		}
		
		Destroy();
	}
	else if( nID == 2 )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



CWndPostSend::CWndPostSend():m_nItem(0xff), m_nCost(0), m_nCount(0)
{ 
//	m_pWndPostSendConfirm = NULL;
} 
CWndPostSend::~CWndPostSend() 
{ 
	ClearData();
//	SAFE_DELETE(m_pWndPostSendConfirm);	
} 

void CWndPostSend::ClearData()
{
	FLItemElem* pItemElem;
	pItemElem  = (FLItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
	
	if(pItemElem)
	{
		// 트레이드에 걸린 아이템은 무시
		if( !g_pPlayer->m_vtInfo.IsTrading( pItemElem ) )
			pItemElem->SetExtra( 0 );
	}
	
	m_nItem		= 0xff;		
	m_nCost		= 0;
	m_nCount	= 0;
	/*
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
	
	pWndEdit2->SetString("");
	pWndEdit3->SetString("");
	pWndEdit4->SetString("");
	*/
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
		FLItemElem* pItemElem;
		
		pItemElem  = (FLItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if( pItemElem )
		{
			LPWNDCTRL pCustom = NULL;
			pCustom = GetWndCtrl( WIDC_STATIC3 );
			
			pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

			if( pItemElem->GetProp()->dwPackMax > 1 && pItemElem->GetExtra() > 0 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
				TCHAR szTemp[32] = { NULL, };
				FLSPrintf( szTemp, _countof( szTemp ), "%d", pItemElem->GetExtra() );
				
				PFONTCOLOR_WNDPOSTSEND pFontColorWndPostSend = g_WndFontColorManager->GetWndPostSend();
				
				p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, pFontColorWndPostSend->m_stItemCountShadow.GetFontColor() );
				p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, pFontColorWndPostSend->m_stItemCount.GetFontColor() );

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
				
				g_WndMng.PutToolTip_Item( (FLItemBase*)pItemElem, point2, &hitrect );
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
	pNotice->AddWndStyle( EBS_AUTOVSCROLL );

	// 아래는 가라로 막은것이다. CWndEdit의 SetWndRect에서 스크롤바의 visible상태에 따라 셋팅하고 나서
	// 부모클래스인 CWndText의 OnInitialUpdate 호출되어 스크롤바의 visible상태를 설정한다. 그래서 SetWndRect를 재호출....ㅡㅡ;;;; 길드공지도 동일하게 막았다.
	// 정상동작하는 메신저 메세지 창의 경우 OnSize에서 SetWndRect를 호출해서 정상적으로 동작했던 것.
	LPWNDCTRL pWndCtrl = GetWndCtrl( WIDC_EDIT3 );
	if(pWndCtrl)
		pNotice->SetWndRect(pWndCtrl->rect);

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndCombo->SetTabStop( TRUE );
	pWndEdit2->SetTabStop( TRUE );
	pWndEdit3->SetTabStop( TRUE );

	for( std::map<u_long, Friend>::iterator i	= g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i )
	{
		u_long idPlayer	= i->first;
		Friend* pFriend	= &i->second;
//		if( pFriend )
		{
			pWndCombo->AddString( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );	//sun: 11, 캐릭터 정보 통합
		}
	}

	if( g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
	{
		CWndStatic* pStatic2	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
		CRect rect	= pStatic2->GetWndRect();
		rect.left	-= 14;
		rect.right	-= 14;
		pStatic2->SetWndRect( rect, TRUE );

		CWndEdit* pEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
		rect	= pEdit4->GetWndRect();
		rect.left	-= 14;
		rect.right	+= 10;
		pEdit4->SetWndRect( rect, TRUE );
	}

	//pWndCombo->SetFocus();
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
		FLItemElem* pItemElem;
		pItemElem  = (FLItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );

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
	FLItemElem* pItemElem;
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT) );
		SetForbid( TRUE );
		return FALSE;
	}

	pItemElem  = (FLItemElem*)g_pPlayer->m_Inventory.GetAt( pShortcut->m_dwIndex );

	if(pItemElem == NULL)
		return FALSE;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		if( pItemElem->IsCharged() )//&& pItemElem->GetProp()->dwItemRare == 200 || pItemElem->GetProp()->dwItemRare == 300 )
		{
			g_WndMng.PutString( prj.GetText(TID_GAME_CHARGED_NOTUSE), NULL, prj.GetTextColor(TID_GAME_CHARGED_NOTUSE) );
			pItemElem = NULL;
			return FALSE;
		}
		
		if( pItemElem->m_nItemNum > 1 )
		{ 
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );

			g_WndMng.m_pWndTradeGold = new CWndTradeGold;
			memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT) );
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemElem->m_nItemNum;
			g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_POST_SEND; // 인벤토리 인데 쓰고 있어서 Gold로 씀.
			g_WndMng.m_pWndTradeGold->m_pWndBase = this;
			g_WndMng.m_pWndTradeGold->m_nSlot = 0;
			
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->AddWndStyle( WBS_MODAL );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();
			//flyingjin : 아이템 이동시 페냐 이동으로 보여지는 현상 수정
			g_WndMng.m_pWndTradeGold->SetTitle( prj.GetText( TID_MMI_BANK_SAVEITEM ) );
			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"몇개를 이동하시겠습니까?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" 갯수 : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( _stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
		else
		{
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );
			pItemElem->SetExtra( 1 );				

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( _stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
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
		FLItemElem* pItemElem;

		pItemElem  = (FLItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
	
		if(pItemElem)
		{
			pItemElem->SetExtra( 0 );				
		}

		m_nItem = 0xff;	
	}		
}

void CWndPostSend::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
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
					if( (isdigit2( szstr4[i] ) == FALSE) )
					{
						bisdigit = FALSE;
						break;
					}
				}
			}
			
			if( bisdigit == FALSE )
			{
				//g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_GUILDONLYNUMBER) );
				return FALSE;
			}

			__int64 liGold	= _atoi64(szstr4);
			if( liGold < 0 || liGold > 1000000000 )		//_SUN_CHECKDATA_(20100705) : added checking ( liGold < 0 || 
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LIMITPENYA) );
				return FALSE;				
			}
			
			if( strlen(szstr1) < 3 || strlen(szstr1) > 16 )
			{
				//이름이 너무 깁니다. 다시 입력해주십시오.
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_0057) );
				return FALSE;				
			}

			if( strlen(szstr3) > ( MAX_MAILTEXT - 1 ) )		//gmpbigsun: 허용범위 오류수정 
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TEXT_MAX), MAX_MAILTEXT );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			if( strlen(szstr2) > ( MAX_MAILTITLE - 1 ) )	//gmpbigsun: 허용범위 오류수정
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TITLE_MAX), MAX_MAILTITLE );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			CWndPostSendMessageBox* pBox = new CWndPostSendMessageBox;

			if( pBox )
			{
				g_WndMng.OpenCustomBox( "", pBox );
				
				FLItemElem* pItemElem;
				
				pItemElem  = (FLItemElem*)g_pPlayer->m_Inventory.GetAtId( m_nItem );
								
				if( pItemElem )
				{
					pBox->SetValue( m_nItem, pItemElem->GetExtra(), (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
				else
				{
					pBox->SetValue( -1, 0, (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
			}
				
			break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostRead::CWndPostRead():m_nMailIndex(-1), m_bDrag(0), m_pWndPostItemWarning(0)
{ 
	m_pDeleteConfirm = NULL;
	m_bRequested = FALSE;
} 
CWndPostRead::~CWndPostRead() 
{ 
	ClearData();
//	SAFE_DELETE(m_pWndPostItemWarning);
} 

void CWndPostRead::ClearData()
{
	m_nMailIndex = -1;
	m_bDrag = FALSE;
}

void CWndPostRead::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( !mailbox[m_nMailIndex] )
		return;
	
	FLItemElem* m_pItemElem = mailbox[m_nMailIndex]->m_pItemElem;

	if( m_pItemElem && m_pItemElem->GetTexture() )
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
			g_WndMng.PutToolTip_Item( (FLItemBase*)m_pItemElem, point2, &hitrect );
		}

		m_pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

		if( m_pItemElem->GetProp()->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
			
			TCHAR szTemp[32];
			FLSPrintf( szTemp, _countof( szTemp ), "%d", m_pItemElem->m_nItemNum  );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );

			PFONTCOLOR_WNDPOSTREAD pFontColorWndPostRead = g_WndFontColorManager->GetWndPostRead();

			p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, pFontColorWndPostRead->m_stItemCountShadow.GetFontColor() );
			p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, pFontColorWndPostRead->m_stItemCount.GetFontColor() );

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

	CRect rect	= pWndEdit3->GetWndRect();
	rect.right	+= 32;
	pWndEdit3->SetWndRect( rect, TRUE );

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	

	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);
//	pWndText->EnableWindow(FALSE);

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
	m_bDrag = FALSE;
} 
void CWndPostRead::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	
	
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// 아이템( 방어구, 무기구 )
	if( PtInRect(&(pCustom->rect), point) )
	{
		m_bDrag = TRUE;
	}
} 
void CWndPostRead::SetValue( int nMailIndex )
{
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	//gmpbigsun(20120118)
	if( nMailIndex < 0 || (size_t)nMailIndex >= mailbox.size() )
		return;

	m_nMailIndex	= nMailIndex;
	CMail* pMail = mailbox[m_nMailIndex];

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

//sun: 11, 캐릭터 정보 통합
	LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
	char lpszPlayer[MAX_PLAYER]		= { 0,};
	if( pMail->m_idSender == 0 )
		FLStrcpy( lpszPlayer, _countof( lpszPlayer ), "FLYFF" );
	else
		FLStrcpy( lpszPlayer, _countof( lpszPlayer ), lpszPlayerString );

	TCHAR szCutTitle[128] = { 0 };
	GetStrCut( lpszPlayer, szCutTitle, 16 );
	
	if( GetStrLen( lpszPlayer ) > 16 )
		FLStrcat( szCutTitle, _countof( szCutTitle ), "..." );
	
	if( lstrlen( lpszPlayer ) > 0 )
		pWndEdit1->SetString(szCutTitle);
	else
		pWndEdit1->SetString("Unknown");
	
	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( pMail->m_szTitle, szCutTitle, 13 );
	
	if( GetStrLen( pMail->m_szTitle ) >= 13 )
	{
		FLStrcat( szCutTitle, _countof( szCutTitle ), "..." );
	}
	
	if( lstrlen( lpszPlayer ) > 0 )
	{
		pWndEdit2->SetString(szCutTitle);
		pWndText->SetString(pMail->m_szText);
	}
	else
	{
		pWndEdit2->SetString( "" );
		pWndText->SetString( "" );
	}

	g_DPlay.SendQueryReadMail( pMail->m_nMail );
	m_bRequested = TRUE;

	char szbuffer[128] = {0};
	FLSPrintf( szbuffer, _countof( szbuffer ), "%d", pMail->m_nGold );
	pWndEdit3->SetString(GetNumberFormatEx(szbuffer));
}
BOOL CWndPostRead::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndPostRead::OnMouseMove(UINT nFlags, CPoint point )
{
	if( m_bDrag )
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
			FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pMail->m_pItemElem->GetProp()->szName);
		}
	}	
}

void CWndPostRead::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
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
		// 기본 보관일수 지났는지를 검사하여 보관료 부과한다.
		int nDay = 0;
		DWORD dwTime = 0;
		pMail->GetMailInfo( &nDay, &dwTime );

		// 기본 보관일수가 지났다!!!
		if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
		{
			FLOAT fCustody = 0.0f;
			FLOAT fPay = 0.0f;
			fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );

			m_bDrag = FALSE;
//			SAFE_DELETE(m_pWndPostItemWarning);
			m_pWndPostItemWarning = new CWndPostItemWarning;
			m_pWndPostItemWarning->Initialize( this, 0 );
			m_pWndPostItemWarning->SetIndex( pMail->m_nMail );

			fPay = pMail->m_pItemElem->GetCost() * fCustody;
			if( fPay < 0.0f )
				fPay = 0.0f;

			TCHAR szChar[256] = { 0 };
			FLSPrintf( szChar, _countof( szChar ), prj.GetText(TID_GAME_MAILBOX_KEEP_PAY), MAX_KEEP_BASIC_DAY, (int)fPay );

			m_pWndPostItemWarning->SetString( szChar );
		}
		else
		{
			g_DPlay.SendQueryGetMailItem( pMail->m_nMail );								
		}
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
	
	if( pMail && pMail->m_nMail > 0 && pMail->m_nGold > 0 )
	{
		//gmpbigsun( 20100803 ) : 인벤토리 합산금액이 MAX_PEYNA_PERSON을 넘을경우 메세지 출력하고 return
		DWORD dwCheckGold = pMail->m_nGold + g_pPlayer->GetGold( );
		if( MAX_PEYNA_PERSON < dwCheckGold )
		{
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_TOOLTIP_PENAERROR01 ) ); //인벤소지금과 메일합산금액이 최대금액을 넘어섬
			return;
		}

		g_DPlay.SendQueryGetMailGold( pMail->m_nMail );		
		
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		pWndEdit->SetString("0");
	}	
}
BOOL CWndPostRead::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];

	if( !pMail )
		return FALSE;
	
	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

	if( pWndPost == NULL )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON1:	
		{
//			SAFE_DELETE(m_pDeleteConfirm);

			if(pMail->m_pItemElem && pMail->m_pItemElem->m_dwItemId == ITEM_INDEX( 26476,II_SYS_SYS_SCR_SEALCHARACTER ) )
				return FALSE;

			//요청중이면 pass
			if( m_bRequested )
				break;

			m_pDeleteConfirm = new CWndPostDeleteConfirm;

			if( m_pDeleteConfirm )
			{
				m_pDeleteConfirm->Initialize(this);
				m_pDeleteConfirm->SetValue(m_nMailIndex);			
			}
		}
		break;
	case WIDC_BUTTON2:	
		{
			CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			
			if( pWndEdit1 && pWndEdit2 )
			{
				if( _stricmp( pWndEdit1->GetString(), "Unknow" ) == 0 )
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

//sun: 11, 캐릭터 정보 통합
				LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
				char	lpszPlayer[MAX_PLAYER]	= { 0, };
				if( pMail->m_idSender == 0 )
					FLStrcpy( lpszPlayer, _countof( lpszPlayer ), "FLYFF" );
				else
					FLStrcpy( lpszPlayer, _countof( lpszPlayer ), lpszPlayerString );

				pWndPost->m_PostTabSend.SetReceive( (char*)lpszPlayer );

				CString str = "Re:";

				//	100805	CSTRING GETBUFFER
				str += CString(pWndEdit2->GetString());
				pWndPost->m_PostTabSend.SetTitle( (LPSTR)str.GetString());

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

	CWndText* pWndText = (CWndText*)(GetDlgItem( WIDC_TEXT1 ));
	pWndText->SetString(_T( prj.GetText(TID_MAIL_DELETE_CONFIRM) ));	
	
	MoveParentCenter();
} 

BOOL CWndPostDeleteConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_DELETE_CONFIRM );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_DELETE_CONFIRM ); 
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

			//added by gmpbigsun( 20120111 )
			CWndPost* pWndBase = (CWndPost *)g_WndMng.GetWndBase( APP_POST );
			if( pWndBase )
				pWndBase->EnableWindow_Receive( FALSE );

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
	DeleteDeviceObjects();
} 
HRESULT CWndPostReceive::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_Texture[0].DeleteDeviceObjects();
	m_Texture[1].DeleteDeviceObjects();
	m_Texture[2].DeleteDeviceObjects();

	SAFE_DELETE( m_wndPostRead );

	return TRUE;
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
	
	sx = 9;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		FLTRACE_LOG( PROGRAM_NAME, _T( "aa" ) );
	}
	
	TCHAR szTemp[32];

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture[0], 150  );

	// 기간 색상 변화
	D3DXVECTOR2 v2_1 = D3DXVECTOR2( 255, 0 );
	D3DXVECTOR2 v2_2 = D3DXVECTOR2( 0, 150 );
	D3DXVECTOR2 v2Result;

	TCHAR szCutTitle[128];
	

	COleDateTime dt;
	CString strDateTime;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( (int)( mailbox.size() ) <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

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
				
				FLSPrintf( szTemp, _countof( szTemp ), "%d", mailbox[i]->m_pItemElem->m_nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				
				PFONTCOLOR_WNDPOSTRECEIVE pFontColorWndPostReceive = g_WndFontColorManager->GetWndPostReceive();
				
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, pFontColorWndPostReceive->m_stItemCountShadow.GetFontColor() );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, pFontColorWndPostReceive->m_stItemCount.GetFontColor() );
				
				p2DRender->SetFont( pOldFont );			
			}
		}
		else
		// 아이템은 없고 돈만 있으면 돈모양 출력
		if( mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold > 0 )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), &m_Texture[2] );
		}

		// 편지를 읽었다면 읽은 표시
		if( mailbox[i]->m_byRead )
		{
			p2DRender->RenderTexture( CPoint( sx+10, sy+1 ), &m_Texture[1], 110 );
		}		

//sun: 11, 캐릭터 정보 통합
		const char* lpszPlayerString	= CPlayerDataCenter::GetInstance()->GetPlayerString( mailbox[i]->m_idSender );
		char lpszPlayer[MAX_PLAYER]	= { 0, };
		if( mailbox[i]->m_idSender == 0 )
			FLStrcpy( lpszPlayer, _countof( lpszPlayer ), "FLYFF" );
		else
			FLStrcpy( lpszPlayer, _countof( lpszPlayer ), lpszPlayerString );
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( lpszPlayer, szCutTitle, 16 );
		
		if( GetStrLen( lpszPlayer ) > 16 )
			FLStrcat( szCutTitle, _countof( szCutTitle ), "..." );

		if( lstrlen( lpszPlayer ) > 0 )
		{
			p2DRender->TextOut( sx + 60,  sy+3, szCutTitle ,  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 60,  sy+3, "Unknown",  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}

		int nDay = 0;
		DWORD dwTime = 0;
		mailbox[i]->GetMailInfo( &nDay, &dwTime );  // 보관일자 검사

		CString szDay;

		// 하루 이하 남으면 시간으로 표시해준다.
		if( nDay <= 1 )
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_HOUR), dwTime );
		}
		else
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_DAY), nDay );
		}
		
		FLOAT f = (FLOAT)( (FLOAT)nDay / (FLOAT)MAX_KEEP_BASIC_DAY );
		D3DXVec2Lerp( &v2Result, &v2_1, &v2_2, f );

		if( nDay <= 0 )
			p2DRender->TextOut( sx + 190,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		else
			p2DRender->TextOut( sx + 193,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( mailbox[i]->m_szTitle, szCutTitle, 13 );
		
		if( GetStrLen( mailbox[i]->m_szTitle ) >= 13 )
		{
			FLStrcat( szCutTitle, _countof( szCutTitle ), "..." );
		}
		
//		if( GetFocusWnd() == this )
		{
			CRect  rect;
			rect.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			CPoint point	= GetMousePoint();
			if( PtInRect( rect, point ) )
			{
				dt = mailbox[i]->m_tmCreate;
				if( dt.GetStatus() != 1 )
				{
					ClientToScreen( &point );
					ClientToScreen( &rect );
					strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
					g_toolTip.PutToolTip( (DWORD)this, strDateTime, rect, point );
				}
			}		
		}
		
		if( lstrlen( lpszPlayer ) > 0 )
			p2DRender->TextOut( sx + 60,  sy+18, szCutTitle,  D3DCOLOR_XRGB( 0, 0, 190 ) );
		else
			p2DRender->TextOut( sx + 60,  sy+18, "",  D3DCOLOR_XRGB( 0, 0, 190 ) );

		sy += 40;
	}	
} 

void CWndPostReceive::OnMouseWndSurface( CPoint point )
{
	int	sx( 15 ), sy( 30 );
	
	CRect rc( sx, 5, sx + 310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		FLTRACE_LOG( PROGRAM_NAME, _T( "aa" ) );
	}
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	COleDateTime	dt;
	CString			strDateTime( _T("") );

	CPoint			cpoint( point );

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( (int)( mailbox.size() ) <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

		rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
		if( PtInRect( rc, cpoint ) )
		{
			dt = mailbox[i]->m_tmCreate;
			if( dt.GetStatus() != 1 )
			{
				ClientToScreen( &cpoint );
				ClientToScreen( &rc );
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rc, cpoint );
			}
			break;
		}		
		
		sy += 40;
	}	
}

void CWndPostReceive::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_nMax <= 0 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	//added gmpbigsun(20120118) 
	size_t uSize = mailbox.size();
	if( 0 == uSize )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect < 0 || (size_t)nSelect >= uSize )		
		return;
	
	m_nSelect = nSelect;	
	
	if( mailbox[m_nSelect] == NULL )
		return;

	SAFE_DELETE( m_wndPostRead );

	m_wndPostRead = new CWndPostRead;

	if( m_wndPostRead )
	{
		m_wndPostRead->Initialize( this );

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
	
	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), "WndPostTable.tga" ), 0xffff00ff );	
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndNotUse.tga" ), 0xffff00ff );	
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "item\\", "itm_GolGolSeed.dds" ), 0xffff00ff );	
	
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

///////////////////////////////////////////////////////////////////////////
// CWndGuildCombatOfferMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndGuildCombatOfferMessageBox : public CWndMessageBox
{ 
public: 
	DWORD m_dwCost;
	void	SetValue( CString str, DWORD dwCost );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndGuildCombatOfferMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( "", 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndGuildCombatOfferMessageBox::SetValue( CString str, DWORD dwCost )
{
	m_wndText.SetString( str );
	m_dwCost = dwCost;	
}

BOOL CWndGuildCombatOfferMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case IDOK:
		{
			g_DPlay.SendGuildCombatApp( m_dwCost );
			SAFE_DELETE( g_WndMng.m_pWndGuildCombatOffer);
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



// 길드 컴뱃 신청하기
CWndGuildCombatOffer::CWndGuildCombatOffer() 
{
	m_dwMinGold    = 0;
	m_dwBackupGold = 0;
	m_dwReqGold    = 0;
}

CWndGuildCombatOffer::~CWndGuildCombatOffer() 
{
	
}

void CWndGuildCombatOffer::OnDraw( C2DRender* p2DRender ) 
{	
}

void CWndGuildCombatOffer::EnableAccept( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_ACCEPT );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
	}
}

void CWndGuildCombatOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( prj.GetText(TID_GAME_GUILDCOMBAT_PRESENT_TEXT) );
	pWndText->EnableWindow( TRUE );
	
	MoveParentCenter();
} 

BOOL CWndGuildCombatOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_OFFER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatOffer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatOffer::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatOffer::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatOffer::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatOffer::SetGold( DWORD nCost )
{
	//_SUN_CHECKDATA_
	if( nCost > MAX_PEYNA_PERSON )
	{
//		g_DPlay.SendError( CLERR_PENYA_OVERLIMIT_P, nCost );
		FLERROR_LOG( PROGRAM_NAME, _T( "penya %x" ), nCost );
		return;
	}

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
	{
		CString str;
		str.Format( "%d", nCost );
		pWndEdit->SetString(str);
	}				
}

void CWndGuildCombatOffer::SetTotalGold( __int64 nCost )
{
	CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC6 );
	pStatic->AddWndStyle(WSS_MONEY);
	
	if( pStatic )
	{
		CString str;
		str.Format( "%I64d", nCost );
		pStatic->SetTitle(str);
	}
}

BOOL CWndGuildCombatOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ACCEPT )
	{
		if( g_pPlayer )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
			DWORD	nCost = 0;
			CString str = pWndEdit->GetString();
			nCost = atoi( str );

			if( m_dwReqGold != 0 )
			{
				if( nCost <= m_dwBackupGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_CURRENT_REQUEST) ); //기존 페냐보다 더 많은 금액으로 신청을 하시기 바랍니다.
					return FALSE;
				}
				
			}
			else
			{
				if( nCost < m_dwMinGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_MIN) ); //최소금액보다 더 많은 페냐로 신청하시기 바랍니다.
					return FALSE;
				}
			}


			CWndGuildCombatOfferMessageBox* pMsg = new CWndGuildCombatOfferMessageBox;
			if( pMsg )
			{
				g_WndMng.OpenCustomBox( "", pMsg, this );
				CString str;

				if( m_dwReqGold == 0 )
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), 0, nCost ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
				}
				else
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), m_dwBackupGold, nCost-m_dwBackupGold ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
				}

				pMsg->SetValue( str, nCost );
			}
		}
		
//		Destroy();
	}
	else if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드 정보출력
//sun: 11, 일대일 길드 대전
CWndGuildCombatBoard::CWndGuildCombatBoard(int nCombatType)
{
	m_nCombatType = nCombatType;
}

CWndGuildCombatBoard::~CWndGuildCombatBoard() 
{
}

//sun: 11, 일대일 길드 대전
void CWndGuildCombatBoard::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_BOARD) );
		else if(m_nCombatType == 1)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_BOARD) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildCombatBoard::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndGuildCombatBoard::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	MoveParentCenter();
} 

BOOL CWndGuildCombatBoard::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_BOARD, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatBoard::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatBoard::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatBoard::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatBoard::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatBoard::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndGuildCombatBoard::SetString( CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar );
	pWndText->ResetString();	
}

// 길드컴뱃 관련 정보창...시간별 뜨는 메세지창...
//sun: 11, 일대일 길드 대전
CGuildCombatInfoMessageBox::CGuildCombatInfoMessageBox(int nCombatType)
{
	m_nCombatType = nCombatType;
}


CGuildCombatInfoMessageBox::~CGuildCombatInfoMessageBox()
{
}

//sun: 11, 일대일 길드 대전
void CGuildCombatInfoMessageBox::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE) );
		else if(m_nCombatType == 1)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

BOOL CGuildCombatInfoMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_MSG, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CGuildCombatInfoMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_YES:
		{
//sun: 11, 일대일 길드 대전
			if(m_nCombatType == 0)
				g_DPlay.SendGCTele();
			else if(m_nCombatType == 1)
				g_DPlay.SendGC1to1TeleportToNPC();
			Destroy();
		}
		break;
	case WIDC_NO:
		{
			Destroy();
		}
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CGuildCombatInfoMessageBox::SetString( CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}

//sun: 11, 일대일 길드 대전
void CGuildCombatInfoMessageBox::SetString( CString strMsg )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( strMsg  );
	pWndText->ResetString();	
}

void CGuildCombatInfoMessageBox::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	MoveParentCenter();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// 길드컴뱃 관련 정보창...시간별 뜨는 메세지창...
CGuildCombatInfoMessageBox2::CGuildCombatInfoMessageBox2()
{
}
CGuildCombatInfoMessageBox2::~CGuildCombatInfoMessageBox2()
{
}
void CGuildCombatInfoMessageBox2::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();	
}

BOOL CGuildCombatInfoMessageBox2::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_MSG2, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CGuildCombatInfoMessageBox2::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			Destroy();
		}
		break;
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CGuildCombatInfoMessageBox2::SetString( CHAR* szChar  )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	
	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////




// 길드 참가자 구성 리셋 컨펌
BOOL CGuildCombatSelectionClearMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_GUILDCOMBAT_REMAKE_MAKEUP), //명단작성을 다시 하시겠습니까?
		pWndParent, 
		MB_OKCANCEL );
}

BOOL CGuildCombatSelectionClearMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case IDOK:
		{
			CWndGuildCombatSelection *pGuildCombatSelection = (CWndGuildCombatSelection*)g_WndMng.GetWndBase( APP_GUILDCOMBAT_SELECTION );

			if( pGuildCombatSelection )
				pGuildCombatSelection->Reset();

			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
////////////////////////////////////////////////////////////////////////////////////////////////////




// 길드 컴뱃 참가자 구성
CWndGuildCombatSelection::CWndGuildCombatSelection() 
{
	m_vecGuildList.clear();
	m_mapSelectPlayer.clear();
	m_vecSelectPlayer.clear();
	m_uidDefender = -1;	

	nMaxJoinMember = 0;
	nMaxWarMember  = 0;
}

CWndGuildCombatSelection::~CWndGuildCombatSelection() 
{
	
}

void CWndGuildCombatSelection::SetDefender( u_long uiPlayer ) 
{
	m_uidDefender = uiPlayer;
}

void CWndGuildCombatSelection::AddCombatPlayer( u_long uiPlayer ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	m_vecSelectPlayer.push_back( uiPlayer );

	CGuild* pGuild = g_pPlayer->GetGuild();
	
	std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find( uiPlayer );
	CGuildMember* pMember = i->second;
				
	CString str;
//sun: 11, 캐릭터 정보 통합
	PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
	str.Format( "%s%.2d	%.16s %.10s", prj.GetText(TID_SKILL_BUFFICONLEVEL01),pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );

	pWndList->AddString( str );			
} 

void CWndGuildCombatSelection::SetMemberSize( int nMaxJoin,  int nMaxWar ) 
{ 
	nMaxJoinMember = nMaxJoin; 
	nMaxWarMember  = nMaxWar; 

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	
	CString str;
	str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_OFFER_INFO), nMaxWarMember, nMaxJoinMember );
	pWndStatic->SetTitle( str );	
}


void CWndGuildCombatSelection::AddGuildPlayer( u_long uiPlayer ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	m_vecGuildList.push_back( uiPlayer );
	
	CGuild* pGuild = g_pPlayer->GetGuild();
	
	std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find( uiPlayer );
	CGuildMember* pMember = i->second;
				
	CString str;
//sun: 11, 캐릭터 정보 통합
	PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
	str.Format( "%s%.2d	%.16s %.10s", prj.GetText(TID_SKILL_BUFFICONLEVEL01),pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );

	pWndList->AddString( str );			
} 

void CWndGuildCombatSelection::RemoveGuildPlayer( int nIndex ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	pWndList->DeleteString( nIndex );

	std::vector<u_long>::iterator erase_pos = m_vecGuildList.begin();
	std::advance( erase_pos, nIndex );

	m_vecGuildList.erase( erase_pos );	
} 

void CWndGuildCombatSelection::RemoveCombatPlayer( int nIndex ) 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	
	pWndList->DeleteString( nIndex );

	std::vector<u_long>::iterator erase_pos = m_vecSelectPlayer.begin();
	std::advance( erase_pos, nIndex );

	if( m_vecSelectPlayer[nIndex] == m_uidDefender )
		SetDefender( -1 );

	m_vecSelectPlayer.erase( erase_pos );	
} 


void CWndGuildCombatSelection::UpDateGuildListBox() 
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	if( pWndList )
	{
		pWndList->ResetContent();
		
		m_vecGuildList.clear();
		m_mapSelectPlayer.clear();

		CGuild* pGuild = g_pPlayer->GetGuild();
		if( pGuild )
		{
			// 레벨별로 소팅
			CGuildMember* pMember;
			for( std::map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
//sun: 11, 캐릭터 정보 통합
				PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
				if( pPlayerData->data.uLogin > 0 )
					m_mapSelectPlayer.insert(std::make_pair( pPlayerData->data.nLevel, pMember) );

			}

			// 리스트에 추가			
			CString str;
			for(std::multimap<int, CGuildMember*>::iterator j = m_mapSelectPlayer.begin(); j != m_mapSelectPlayer.end(); ++j )
			{
				pMember		= j->second;		
//sun: 11, 캐릭터 정보 통합
				PlayerData* pPlayerData		= CPlayerDataCenter::GetInstance()->GetPlayerData( pMember->m_idPlayer );
				if( pPlayerData->data.uLogin > 0 )
				{
					//sun: 11, 캐릭터 정보 통합
					str.Format( "%s%.2d	%.16s %.10s", prj.GetText(TID_SKILL_BUFFICONLEVEL01),pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[ pPlayerData->data.nJob ].szName );

					pWndList->AddString( str );	
					m_vecGuildList.push_back( pMember->m_idPlayer );
				}
			}
		}
	}
}

u_long CWndGuildCombatSelection::FindCombatPlayer(u_long uiPlayer)
{
	for( int i = 0; i < (int)( m_vecSelectPlayer.size() ); i++ )
	{
		if( m_vecSelectPlayer[i] == uiPlayer )
			return m_vecSelectPlayer[i];
	}
	
	return -1;
}

u_long CWndGuildCombatSelection::FindGuildPlayer(u_long uiPlayer)
{
	for( int i = 0; i < (int)( m_vecGuildList.size() ); i++ )
	{
		if( m_vecGuildList[i] == uiPlayer )
			return m_vecGuildList[i];
	}
	
	return -1;
}

void CWndGuildCombatSelection::OnDraw( C2DRender* p2DRender ) 
{
	CWndButton* pWndStatic = (CWndButton*)GetDlgItem( WIDC_BUTTON8 );

	if( pWndStatic )
	{
		pWndStatic->EnableWindow( FALSE );

		if( m_uidDefender <= 0 )
		{
			pWndStatic->SetVisible( FALSE );
		}
		else
		{
			pWndStatic->SetVisible( TRUE );

			int i=0;
			for( ; i<(int)( m_vecSelectPlayer.size() ); i++ )
			{
				if( m_uidDefender == m_vecSelectPlayer[i] )
				{
					break;
				}
			}

			pWndStatic->Move( 330, ( i * 17 ) + 75 );			
		}
	}

	CRect crect;
	crect.left = 349;
	crect.top  = 74;
	crect.right = 587;
	crect.bottom = crect.top+(nMaxWarMember*17);
	p2DRender->RenderFillRect( crect, D3DCOLOR_ARGB( 40, 0, 0, 220 ) );

	crect.left = 349;
	crect.top  = crect.bottom;
	crect.right = 587;
	crect.bottom = 377;
	p2DRender->RenderFillRect( crect, D3DCOLOR_ARGB( 40, 220, 0, 0 ) );
	
}

void CWndGuildCombatSelection::EnableFinish( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_FINISH );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );	
	}
}

void CWndGuildCombatSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 시간 지났는지를 판단
	if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE )
	{
		g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //지금은 명단작성을 할 수 없습니다.
		Destroy();
		return;
	}
	
	MoveParentCenter();

	UpDateGuildListBox();
} 

BOOL CWndGuildCombatSelection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_SELECTION, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatSelection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatSelection::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatSelection::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatSelection::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 


void CWndGuildCombatSelection::Reset()
{
	m_uidDefender = 0;
	UpDateGuildListBox();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	pWndListBox->ResetContent();
	m_vecSelectPlayer.clear();	
}

BOOL CWndGuildCombatSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	// 출전자 등록
	if( nID == WIDC_BUTTON1 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		if( nMaxJoinMember < (int)( m_vecSelectPlayer.size() ) )
		{
			CString str;
			str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_SELECTION_MAX), nMaxJoinMember );
			g_WndMng.OpenMessageBox( str );
			return FALSE;
		}

		CGuild *pGuild = g_pPlayer->GetGuild();

		if( pGuild )
		{
			CGuildMember* pGuildMember = pGuild->GetMember( m_vecGuildList[nCurSel] );

			if( pGuildMember )
			{
//sun: 11, 캐릭터 정보 통합
				if( CPlayerDataCenter::GetInstance()->GetPlayerData( pGuildMember->m_idPlayer )->data.nLevel < 30 )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_LEVEL_NOTICE) ); //출전자 등록은 레벨 30이상이 되어야 합니다.
					return FALSE;
				}
			}	
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_MEMBER) );	//길드대전에 참가하는 길드의 맴버가 아닙니다.			
				return FALSE;
			}
		}

		u_long uiPlayer;
		uiPlayer = FindCombatPlayer( m_vecGuildList[nCurSel] );

		if( uiPlayer != -1 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_ALREADY_ENTRY) ); //이미 등록되어 있습니다. 다시 등록해주세요.
			return FALSE;
		}
 
		AddCombatPlayer( m_vecGuildList[nCurSel] );		
		RemoveGuildPlayer( nCurSel );		
	}
	else
	// 출전자 취소
	if( nID == WIDC_BUTTON2 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		u_long uiPlayer;
		uiPlayer = FindGuildPlayer( m_vecSelectPlayer[nCurSel] );
		
		if( uiPlayer == -1 )
		{
			// 길드리스트에 없다면 추가 
			AddGuildPlayer( m_vecSelectPlayer[nCurSel] );		
			RemoveCombatPlayer( nCurSel );		
		}
		else
		{
			RemoveCombatPlayer( nCurSel );		
		}		
	}
	else
	// 출전자 순서 위로
	if( nID == WIDC_BUTTON3 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 || nCurSel == 0 )
			return FALSE;
		
		CString temp1, temp2;
		pWndListBox->GetText( nCurSel-1, temp1 );
		pWndListBox->GetText( nCurSel,   temp2 );

		pWndListBox->SetString( nCurSel-1, temp2 );
		pWndListBox->SetString( nCurSel, temp1 );
		
		u_long uiTemp = 0;

		uiTemp						 = m_vecSelectPlayer[nCurSel];
		m_vecSelectPlayer[nCurSel]   = m_vecSelectPlayer[nCurSel-1];
		m_vecSelectPlayer[nCurSel-1] = uiTemp;

		m_nDefenderIndex = nCurSel-1;
	}
	else
	// 출전자 순서 아래
	if( nID == WIDC_BUTTON4 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 || nCurSel == pWndListBox->GetCount()-1 )
			return FALSE;
		
		CString temp1, temp2;
		pWndListBox->GetText( nCurSel, temp1 );
		pWndListBox->GetText( nCurSel+1,   temp2 );
		
		pWndListBox->SetString( nCurSel, temp2 );
		pWndListBox->SetString( nCurSel+1, temp1 );
		
		u_long uiTemp = 0;
		
		uiTemp						 = m_vecSelectPlayer[nCurSel];
		m_vecSelectPlayer[nCurSel]   = m_vecSelectPlayer[nCurSel+1];
		m_vecSelectPlayer[nCurSel+1] = uiTemp;

		m_nDefenderIndex = nCurSel+1;
	}
	else
	if( nID == WIDC_RESET )
	{
		CGuildCombatSelectionClearMessageBox* pBox = new CGuildCombatSelectionClearMessageBox;
		g_WndMng.OpenCustomBox( "", pBox );
	}
	else
	if( nID == WIDC_FINISH )
	{
#ifndef _DEBUG
		if( m_uidDefender == -1 || m_uidDefender == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_ASSIGN_DEFENDER) ); //디펜더가 지정되지 않았습니다. 길드마스터를 제외한 1명의 디펜더를 지정해 주세요.
			return FALSE;
		}
#endif //_DEBUG

		// 시간 지났는지를 판단
		if( g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP) ); //지금은 명단작성을 할 수 없습니다.
			Destroy();
			return FALSE;
		}
		
		if( m_vecSelectPlayer.size() == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_PLAYER) ); //출전자가 없습니다. 출전자를 선택해주세요.
			return FALSE;
		}
		
		CGuild *pGuild = g_pPlayer->GetGuild();
		CGuildMember* pGuildMemberl;
		
		if( pGuild )
		{
			BOOL bSkip = FALSE;

			// 출전자 맴버중에 마스터가 킹핀이 있는지 검사를한다.
			// 둘중 하나라도 없으면 출전 불가능...
			for( int i=0; i<(int)( m_vecSelectPlayer.size() ); i++ )
			{
				pGuildMemberl = pGuild->GetMember( m_vecSelectPlayer[i] );

				if( pGuildMemberl )
				{
					if( pGuildMemberl->m_nMemberLv == GUD_MASTER || pGuildMemberl->m_nMemberLv == GUD_KINGPIN )
					{
						bSkip = TRUE;
						break;
					}
				}
			}

			if( bSkip )
			{
				g_DPlay.SendGCSelectPlayer( m_vecSelectPlayer, m_uidDefender );
				Destroy();
			}
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_MASTER) ); //출전자 명단에 길드마스터나 킹핀이 존재하지 않습니다.
				return FALSE;
			}
		}
	}
	else
	// 디펜더 지정
	if( nID == WIDC_BUTTON7 )
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
		
		int nCurSel = pWndListBox->GetCurSel();
		if( nCurSel == -1 )
			return FALSE;

		CGuild *pGuild = g_pPlayer->GetGuild();

		if( pGuild )
		{
			if( m_vecSelectPlayer.size() > 1 &&  pGuild->IsMaster( m_vecSelectPlayer[nCurSel] ) )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_NOT_ASSIGN_DEFENDER) ); //길드마스터는 디펜더가 될 수 없습니다.
				return FALSE;
			}
		}
		
		m_nDefenderIndex = nCurSel;
		SetDefender( m_vecSelectPlayer[nCurSel] );
	}
	else
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}



//길드대전 참가 길드 명단 출력
//sun: 11, 일대일 길드 대전
CWndGuildCombatState::CWndGuildCombatState(int nCombatType)
{ 
	Init( 0 );

	m_tEndTime    = 0;
	m_tCurrentTime = 0;
	m_nCombatType = nCombatType;
}


CWndGuildCombatState::~CWndGuildCombatState() 
{ 
} 

//sun: 11, 일대일 길드 대전
void CWndGuildCombatState::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE) );
		else if(m_nCombatType == 1)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_OFFERSTATE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
}

void CWndGuildCombatState::Init( time_t lTime )
{
}

void CWndGuildCombatState::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 

void CWndGuildCombatState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndGuildCombatState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
//sun: 11, 일대일 길드 대전
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_1TO1_OFFERSTATE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndGuildCombatState::GetSelectIndex( const CPoint& point )
{
	return -1;
}

void CWndGuildCombatState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndGuildCombatState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	else
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
//sun: 11, 일대일 길드 대전
		if(m_nCombatType == 0)
			g_DPlay.SendGuildCombatWindow();
		else if(m_nCombatType == 1)
			g_DPlay.SendGC1to1TenderOpenWnd();
	}
	else
	if( nID == WIDC_BUTTON2 )
	{
//sun: 11, 일대일 길드 대전
		if(m_nCombatType == 0)
			g_DPlay.SendGCRequestStatusWindow();
		else if(m_nCombatType == 1)
			g_DPlay.SendGC1to1TenderView();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndGuildCombatState::InsertGuild( const char szGuild[], const char szName[], int nNum, int nSelect )
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	CString str;
	/*if ( nSelect )
		str.Format( "No.%.2d		%s ( %d/10 )", nNum, szGuild, nSelect );
	else*/
	str.Format( "No.%.2d		%s", nNum, szGuild );
	pWndList->AddString(str);
}

void CWndGuildCombatState::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_tCurrentTime > 0 )
	{
		CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );

		CString str1;
		CString str2;
		CString str3;
		CString str4;
		CString str;
		
		str1.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(m_ct.GetDays()) );
		str2.Format(prj.GetText(TID_PK_LIMIT_HOUR), m_ct.GetHours() );
		str3.Format(prj.GetText(TID_PK_LIMIT_MINUTE), m_ct.GetMinutes() );
		str4.Format(prj.GetText(TID_PK_LIMIT_SECOND), m_ct.GetSeconds() );

		str.Format( "%s %s %s %s", str1, str2, str3, str4 );
		pStatic->SetTitle(str);
	}
} 

void CWndGuildCombatState::SetRate( int nRate )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	
	CString str;
	str.Format( "%d", nRate );
	
	if( nRate == -1 )
		str = " ";

	pWndStatic->SetTitle( str );
	if( 8 < nRate )
		pWndStatic->m_dwColor = 0xFFFF0000;	
}

BOOL CWndGuildCombatState::Process() 
{
	m_tCurrentTime = ( m_tEndTime - time_null() );
	
	if( m_tEndTime && m_tCurrentTime > 0 )
	{
		m_ct = m_tCurrentTime;//long)(m_dwCurrentTime / 1000.0f) );
	}
	
	return CWndBase::Process();
}

void CWndGuildCombatState::SetGold( int nGold )
{
	//_SUN_CHECKDATA_
	DWORD dwCheckGold = nGold;
	if( dwCheckGold > MAX_PEYNA_PERSON )
	{
//		g_DPlay.SendError( CLERR_PENYA_OVERLIMIT_P, dwCheckGold );
		FLERROR_LOG( PROGRAM_NAME, _T( "penya %x" ), dwCheckGold );
		return;
	}

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pWndStatic->AddWndStyle(WSS_MONEY);	

	CString str;
	str.Format( "%d", nGold );
	pWndStatic->SetTitle( str );	
}



// 길드전장 입장지역 선택 창
CWndGuildCombatJoinSelection::CWndGuildCombatJoinSelection() 
{
	m_nMapNum = 99;
	m_dwOldTime = 0xffffffff;
}

CWndGuildCombatJoinSelection::~CWndGuildCombatJoinSelection() 
{
	
}

void CWndGuildCombatJoinSelection::OnDraw( C2DRender* p2DRender ) 
{
	DWORD dwLeftTime = m_dwOldTime - g_tmCurrent;

	CWndStatic* pStatic = (CWndStatic*) GetDlgItem( WIDC_STATIC2 );
	
	CString str;
	CString strstr;
	
	if( m_nMapNum == 99 )
	{
		strstr = "Random";
	}
	else
	{
		strstr.Format( "%d", m_nMapNum+1 );
	}

	str.Format( prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_MESSAGE), (dwLeftTime/1000)+1, strstr );
	
	pStatic->SetTitle( str );

	if( m_nMapNum != 99 )
	{
		CWndButton* pWndButton = NULL;

		if( m_nMapNum == 0 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON1 );
		}
		else
		if( m_nMapNum == 1 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON2 );
		}
		else
		if( m_nMapNum == 2 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON3 );
		}
		else
		if( m_nMapNum == 3 )
		{
			pWndButton = (CWndButton*) GetDlgItem( WIDC_BUTTON4 );
		}
		
		if( pWndButton )
		{
			CRect rect = pWndButton->GetWndRect();
			rect.DeflateRect( 1, 1, 1, 1 );
			p2DRender->RenderRect( rect, 0xFF0000FF );
		}
	}
}

void CWndGuildCombatJoinSelection::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();

	m_wndTitleBar.SetVisible( FALSE );	
} 

BOOL CWndGuildCombatJoinSelection::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_SELSTART, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndGuildCombatJoinSelection::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndGuildCombatJoinSelection::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndGuildCombatJoinSelection::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndGuildCombatJoinSelection::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndGuildCombatJoinSelection::Process ()
{
	if( m_dwOldTime <= g_tmCurrent )
	{
		g_DPlay.SendGCSelectMap(m_nMapNum);		
		Destroy();
	}

	return 1;
}

BOOL CWndGuildCombatJoinSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		g_DPlay.SendGCSelectMap(0);	
		m_nMapNum = 0;
	}
	else if( nID == WIDC_BUTTON2 )
	{
		g_DPlay.SendGCSelectMap(1);		
		m_nMapNum = 1;
	}
	else if( nID == WIDC_BUTTON3 )
	{
		g_DPlay.SendGCSelectMap(2);		
		m_nMapNum = 2;
	}
	else if( nID == WIDC_BUTTON4 )
	{
		g_DPlay.SendGCSelectMap(3);		
		m_nMapNum = 3;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_APP) ) );
	m_wndText.ResetString();
	
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
			g_DPlay.SendGuildCombatApp( 80000 );

		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드대전 취소
//sun: 11, 일대일 길드 대전
CWndGuildWarCancelConfirm::CWndGuildWarCancelConfirm(int nCombatType)
{
	m_nCombatType = nCombatType;
}

CWndGuildWarCancelConfirm::~CWndGuildWarCancelConfirm() 
{
}

//sun: 11, 일대일 길드 대전
void CWndGuildWarCancelConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL_CONFIRM) );
		else if(m_nCombatType == 1)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_CANCEL_CONFIRM) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
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
	m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_CANCEL) ) );//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();
	
	MoveParentCenter();
} 

BOOL CWndGuildWarCancelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_CANCEL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
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
//sun: 11, 일대일 길드 대전
		if(m_nCombatType == 0)
		{
			if( g_pPlayer )
				g_DPlay.SendGuildCombatCancel();
		}
		else if(m_nCombatType == 1)
		{
			if( g_pPlayer )
				g_DPlay.SendGC1to1TenderCancel();
		}
		
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

// 길드대전 입장
//sun: 11, 일대일 길드 대전
CWndGuildWarJoinConfirm::CWndGuildWarJoinConfirm(int nCombatType) 
{
	m_nCombatType = nCombatType;
}

CWndGuildWarJoinConfirm::~CWndGuildWarJoinConfirm() 
{	
}

//sun: 11, 일대일 길드 대전
void CWndGuildWarJoinConfirm::PaintFrame( C2DRender* p2DRender )
{
	CRect rect = GetWindowRect();
	RenderWnd();
	// 여기는 타이틀 바의 텍스트를 출력하는 곳 
	if( IsWndStyle( WBS_CAPTION ) )	
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, y, m_strTitle, m_dwColor );

		char szNameLevel[128] = {0,};

		if(m_nCombatType == 0)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_ENTRANCE) );
		else if(m_nCombatType == 1)
			FLSPrintf( szNameLevel, _countof( szNameLevel ), "%s", prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE) );

		SetTitle( szNameLevel );
		
		p2DRender->SetFont( pOldFont );
	}
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
//sun: 11, 일대일 길드 대전
	if(m_nCombatType == 0)
		m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDWAR_JOIN) ), prj.GetTextColor(TID_GAME_GUILDWAR_JOIN) );
	else if(m_nCombatType == 1)
		m_wndText.SetString( _T( prj.GetText(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE_MSG) ), prj.GetTextColor(TID_GAME_GUILDCOMBAT_1TO1_ENTRANCE_MSG) );

	m_wndText.ResetString();
	
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

void CWndGuildWarJoinConfirm::OnSize( UINT nType, int cx, int cy )
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
		{
			if( g_pPlayer->IsBaseJob() )
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT_NOTENTER_VAGRANT) ); //방랑자는 입장할수 없습니다. 1차 전직 후에 입장을 해주시기 바랍니다.
				Destroy( TRUE );
				return FALSE;
			}
//sun: 11, 일대일 길드 대전
			if(m_nCombatType == 0)
				g_DPlay.SendGCJoin();
			else if(m_nCombatType == 1)
			{
				if(g_WndMng.m_pWndGuildCombatInfoMessageBox)
					SAFE_DELETE( g_WndMng.m_pWndGuildCombatInfoMessageBox );
				
				g_DPlay.SendGC1to1TeleportToStage();
			}
		}
		
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
		FLERROR_LOG( PROGRAM_NAME, _T( "range over" ) );
		return;
	}
	
	if( nNum == 0 )
		return;

	GetStrCut( szGuild, m_aList[m_nMax].szGuild, 8 );

	if( GetStrLen( szGuild ) >= 8 )
	{
		FLStrcat( m_aList[m_nMax].szGuild, _countof( m_aList[m_nMax].szGuild ), "..." );
	}
	else
	{
		FLStrcpy( m_aList[m_nMax].szGuild, _countof( m_aList[m_nMax].szGuild ), szGuild );
	}

	FLStrcpy( m_aList[m_nMax].szName, _countof( m_aList[m_nMax].szName ), szName );
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
		FLTRACE_LOG( PROGRAM_NAME, _T( "aa" ) );
	}

	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;
		
		FLSPrintf( szNum, _countof( szNum ), "%3d", i+1 );
		FLSPrintf( szCount, _countof( szCount ), "%2d", m_aList[i].nNum );

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
		FLERROR_LOG( PROGRAM_NAME, _T( "range over" ) );
		return;
	}

	GUILDNAME p = { 0 };
	//mem_set( &p, 0, sizeof(p) );
	GetStrCut( szGuild, p.szGuild, 8 );
	
	if( GetStrLen( szGuild ) >= 8 )
	{
		FLStrcat( p.szGuild, _countof( p.szGuild ), "..." );
	}
	else
	{
		FLStrcpy( p.szGuild, _countof( p.szGuild ), szGuild );
	}
	
	m_multimapRanking.insert(std::make_pair(nWinCount, p) );

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
		FLTRACE_LOG( PROGRAM_NAME, _T( "aa" ) );
	}

	int nRanking = 0;
	int nOldRanking = -1;
	std::multimap< int, GUILDNAME >::reverse_iterator iterRobotBegin = m_multimapRanking.rbegin();
	
	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		std::multimap< int, GUILDNAME >::value_type& refValue = *iterRobotBegin;
		
		if( nOldRanking != refValue.first )
			nRanking++;

		FLSPrintf( szNum, _countof( szNum ), "%3d", nRanking );
		FLSPrintf( szCount, _countof( szCount ), "%2d", refValue.first );

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
			p2DRender->TextOut( sx + 5,    sy, "   ",  dwColor );
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


/****************************************************
  WndId : APP_GUILDCOMBAT_RESULT_POINT - 길드대전 결과창
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuildCombatResult::CWndGuildCombatResult() 
{ 
} 
CWndGuildCombatResult::~CWndGuildCombatResult() 
{ 
} 
void CWndGuildCombatResult::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatResult::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	pWndTabCtrl->SetButtonLength( 100 );

	m_WndGuildCombatTabResultRate.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT );
	m_WndGuildCombatTabResultLog.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT_LOG );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultRate;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG2);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultLog;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();

	{
//		CRect r = GetWindowRect( TRUE );
//		m_WndGuildTabMember.m_rectParent = r;
	}
	
	CWndListBox* pWndList1 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX1 ));
	CWndListBox* pWndList2 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX2 ));
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_TEXT1 ));
	
	pWndList1->ResetContent();
	pWndList2->ResetContent();	
	pWndText->ResetString();
} 
BOOL CWndGuildCombatResult::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RESULT_POINT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatResult::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildCombatResult::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildCombatResult::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildCombatResult::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildCombatResult::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatResult::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatResult::InsertGuildRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX1 ));
	pWndList->AddString( str );	
}

void CWndGuildCombatResult::InsertPersonRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem( WIDC_LISTBOX2 ));
	pWndList->AddString( str );		
}

void CWndGuildCombatResult::InsertLog(CString str)
{
//	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_LISTBOX1 ));
//	pWndList->AddString( str );	
	
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_TEXT1 ));
	pWndText->AddString( str );			
//	pWndList->AddString( str );			
	
}


/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT
****************************************************/
CWndGuildCombatTabResultRate::CWndGuildCombatTabResultRate() 
{ 
} 
CWndGuildCombatTabResultRate::~CWndGuildCombatTabResultRate() 
{ 
} 
void CWndGuildCombatTabResultRate::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatTabResultRate::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatTabResultRate::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_TAB_RESULT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatTabResultRate::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatTabResultRate::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
	
/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT_LOG
****************************************************/
CWndGuildCombatTabResultLog::CWndGuildCombatTabResultLog() 
{ 
} 
CWndGuildCombatTabResultLog::~CWndGuildCombatTabResultLog() 
{ 
} 
void CWndGuildCombatTabResultLog::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatTabResultLog::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatTabResultLog::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_TAB_RESULT_LOG, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatTabResultLog::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatTabResultLog::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		CTime time = CTime::GetCurrentTime();
		CString str;

		str.Format( "GuildLog_%d%.2d%.2d.txt", time.GetYear(), time.GetMonth(), time.GetDay() );

		CFileIO file;

		if( file.Open( str, _T( "wt" ) ) == FALSE )
			return FALSE;

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
				
		file.PutString( _T( pWndText->m_string ) ); 

		CString strTemp;
		strTemp.Format( prj.GetText(TID_GAME_GUILDCOMBAT_LOG_SAVE), str );

		g_WndMng.OpenMessageBox( strTemp );
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


/****************************************************
  WndId : APP_GUILDCOMBAT_RANK_P - 길드대전 랭킹창
  CtrlId : WIDC_TABCTRL1 - TabCtrl
****************************************************/
CWndGuildCombatRank_Person::CWndGuildCombatRank_Person() 
{ 
	m_mTotalRanking.clear();
} 
CWndGuildCombatRank_Person::~CWndGuildCombatRank_Person() 
{ 
} 
void CWndGuildCombatRank_Person::OnDraw( C2DRender* p2DRender ) 
{ 
} 

void CWndGuildCombatRank_Person::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );
	
	pWndTabCtrl->SetButtonLength( 100 );

	m_WndGuildCombatTabClass_Tot.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mer.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Mag.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Acr.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	m_WndGuildCombatTabClass_Ass.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Tot;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 1 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mer;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 4 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mag;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );
	
	tabTabItem.pszText = prj.m_aJob[ 2 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Acr;
	pWndTabCtrl->InsertItem( 3, &tabTabItem );

	tabTabItem.pszText = prj.m_aJob[ 3 ].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Ass;
	pWndTabCtrl->InsertItem( 4, &tabTabItem );


	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndGuildCombatRank_Person::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANK_P, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatRank_Person::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndGuildCombatRank_Person::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndGuildCombatRank_Person::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndGuildCombatRank_Person::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndGuildCombatRank_Person::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatRank_Person::OnMouseMove(UINT nFlags, CPoint point )
{
}

void CWndGuildCombatRank_Person::InsertRank( int nJob, u_long	uidPlayer, int nPoint )
{
	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	GcRankInfo.nJob      = nJob;
	GcRankInfo.uidPlayer = uidPlayer;
	m_mTotalRanking.insert(std::make_pair( nPoint, GcRankInfo ) );
}

void CWndGuildCombatRank_Person::DivisionList()
{
	if( m_mTotalRanking.size() <= 0 )
		return;
	
	int nPoint;

	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	CWndGuildCombatRank_Class * pRankTot;
	CWndGuildCombatRank_Class * pRank;

	// 전체목록에 등록
	pRankTot = &(m_WndGuildCombatTabClass_Tot);
	std::multimap< int, __GUILDCOMBAT_RANK_INFO >::reverse_iterator i;

	// 각지업별 등록
	pRank = NULL;
	for( i = m_mTotalRanking.rbegin(); i != m_mTotalRanking.rend(); ++i )
	{ 
		nPoint			= i->first;
		GcRankInfo		= i->second;
		
		pRank = __GetJobKindWnd( GcRankInfo.nJob );
		
		if( pRank == NULL )
			continue;

		pRank->InsertRank( GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint );
		pRankTot->InsertRank( GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint );
	}
}

void CWndGuildCombatRank_Person::UpdateList()
{

}

void CWndGuildCombatRank_Person::UpdatePlayer(int nJob, u_long idPlayer)
{

}

CWndGuildCombatRank_Class* CWndGuildCombatRank_Person::__GetJobKindWnd(int nJob)
{
	switch(nJob) 
	{
	case JOB_MERCENARY:
	case JOB_KNIGHT:
	case JOB_BLADE:
 	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
	case JOB_LORDTEMPLER_HERO:
	case JOB_STORMBLADE_HERO:
		return &m_WndGuildCombatTabClass_Mer;
		break;
		
	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
	case JOB_WINDLURKER_HERO:
	case JOB_CRACKSHOOTER_HERO:
		return &m_WndGuildCombatTabClass_Acr;
		break;
		
	case JOB_ASSIST:
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_RINGMASTER_HERO:
	case JOB_BILLPOSTER_HERO:
	case JOB_FLORIST_HERO:
	case JOB_FORCEMASTER_HERO:
		return &m_WndGuildCombatTabClass_Ass;
		break;
		
	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
	case JOB_ELEMENTOR_HERO:
	case JOB_MENTALIST_HERO:
	case JOB_ELEMENTORLORD_HERO:
		return &m_WndGuildCombatTabClass_Mag;
		break;
		
	default:
		break;
	}	

	return NULL;
}

/****************************************************
  WndId : APP_GUILDCOMBAT_RANKINGCLASS
****************************************************/
CWndGuildCombatRank_Class::CWndGuildCombatRank_Class() 
{ 
	m_nRate = 0;
	m_nOldRate = -1;
	m_nMax = 0;
	m_nSelect = -1;	
}

CWndGuildCombatRank_Class::~CWndGuildCombatRank_Class() 
{ 
} 
void CWndGuildCombatRank_Class::OnDraw( C2DRender* p2DRender ) 
{ 
	PFLCOLOR_WNDGUILDCOMBATRANK_CLASS pColorWndGuildCombatRank_Class = g_FLColorManager->GetWndGuildCombatRank_Class();

	DWORD dwColor( m_dwColor );
	int	sx, sy;
	char szNum[8], szCount[8];
	
	sx = 8;
	sy = 35;	
	
	CRect rc( sx, 5, sx+330, 7 ); 	
	p2DRender->RenderFillRect( rc , pColorWndGuildCombatRank_Class->m_stLineUp.GetColor() );
	
	rc += CPoint( 0, 20 );
	p2DRender->RenderFillRect( rc , pColorWndGuildCombatRank_Class->m_stLineDown.GetColor() );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		FLTRACE_LOG( PROGRAM_NAME, _T( "aa" ) );
	}
	
	int nRanking = 0;
	int nOldRanking = -1;
	
	__GUILDCOMBAT_RANK_INFO2 GCRankInfo;
	__GUILDCOMBAT_RANK_INFO2 GCRankInfoMy;
	int	nMyRanking = 0;
	
	CString szName;
	CString szJob;

	BOOL	bMyRanking = FALSE;
	
	p2DRender->TextOut( sx + 4,      10, prj.GetText(TID_GAME_RATE),	dwColor );
	p2DRender->TextOut( sx + 40,     10, prj.GetText(TID_GAME_NAME),	dwColor );
	p2DRender->TextOut( sx + 180,    10, prj.GetText(TID_GAME_JOB),		dwColor );
	p2DRender->TextOut( sx + 275,    10, prj.GetText(TID_GAME_POINT),	dwColor );
	
	for( int k = 0; k < nBase; k++ )
	{
		GCRankInfo = m_listRank[k];

		if( nOldRanking != GCRankInfo.nPoint )
			nRanking++;

		if( GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer ) 
		{
			bMyRanking   = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking   = nRanking;
		}		

		nOldRanking = GCRankInfo.nPoint;					
	}
	
	PFONTCOLOR_WNDGUILDCOMBATRANK_CLASS pFontColorWndGuildCombatRank_Class = g_WndFontColorManager->GetWndGuildCombatRank_Class();

	for( int i=nBase; i<nBase + MAX_GUILDCOMBAT_RANK_PER_PAGE; ++i )
	{
		dwColor = m_dwColor;

		if( i >= m_nMax )	
			break;
		
		GCRankInfo = m_listRank[i];

		if( nOldRanking != GCRankInfo.nPoint )
			nRanking++;

		if( GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer ) 
		{
			bMyRanking   = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking   = nRanking;
		}

		FLSPrintf( szNum, _countof( szNum ), "%3d", nRanking );
		FLSPrintf( szCount, _countof( szCount ), "%2d", GCRankInfo.nPoint );
		
		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx, sy - 4, sx+320, sy + 16 ); 	
			p2DRender->RenderFillRect( rc , pColorWndGuildCombatRank_Class->m_stSelectBoxFill.GetColor() );
		}
		
		if( i == 0 )
			dwColor = pFontColorWndGuildCombatRank_Class->m_st1stPlayer.GetFontColor();
		
		if( nOldRanking != GCRankInfo.nPoint )
			p2DRender->TextOut( sx + 4, sy, szNum,  dwColor );
		else
			p2DRender->TextOut( sx + 5, sy, "   ",  dwColor );

		CString strName( _T("") );

		const char* lpName	= CPlayerDataCenter::GetInstance()->GetPlayerString( GCRankInfo.uidPlayer );
		if( lpName )
			strName	= lpName;

		if( i == 0 )
		{
			p2DRender->TextOut( sx + 40,  sy, strName,				pFontColorWndGuildCombatRank_Class->m_st1stPlayer.GetFontColor() );			
			p2DRender->TextOut( sx + 180, sy, GCRankInfo.strJob,	pFontColorWndGuildCombatRank_Class->m_st1stPlayer.GetFontColor() );
		}
		else
		{
			p2DRender->TextOut( sx + 40,  sy, strName,				m_dwColor );					
			p2DRender->TextOut( sx + 180, sy, GCRankInfo.strJob,    m_dwColor );
		}
		
		p2DRender->TextOut( sx + 275,  sy, szCount,    dwColor );
		
		sy += 18;
		
		nOldRanking = GCRankInfo.nPoint;					
	}	

	// 나의 랭킹 표시...
	if( bMyRanking )
	{
		FLSPrintf( szNum, _countof( szNum ), "%3d", nMyRanking );
		FLSPrintf( szCount, _countof( szCount ), "%2d", GCRankInfoMy.nPoint );
		
		dwColor = pFontColorWndGuildCombatRank_Class->m_stSelfPlayer.GetFontColor();
		
		p2DRender->TextOut( sx + 4,   250, szNum,					dwColor );
	
		p2DRender->TextOut( sx + 40,  250, GCRankInfoMy.strName,	dwColor );
		p2DRender->TextOut( sx + 180, 250, GCRankInfoMy.strJob,		dwColor );		
		p2DRender->TextOut( sx + 275, 250, szCount,					dwColor );
	}
} 

// 선택된 인덱스를 얻는다.
int CWndGuildCombatRank_Class::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;
	
	if( 0 <= nIndex && nIndex < MAX_GUILDCOMBAT_RANK_PER_PAGE ) // 0 - 19사이 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
BOOL CWndGuildCombatRank_Class::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
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

void CWndGuildCombatRank_Class::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;
} 

void CWndGuildCombatRank_Class::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CRect rect = GetWindowRect();
	rect.top    += 30;
	rect.bottom -= 80;
	rect.left    = rect.right - 30;
	rect.right  -= 30;
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_RANK_PER_PAGE );
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
BOOL CWndGuildCombatRank_Class::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANKINGCLASS, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndGuildCombatRank_Class::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndGuildCombatRank_Class::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndGuildCombatRank_Class::InsertRank( int nJob, u_long uidPlayer, int nPoint )
{
	if( m_nMax >= MAX_GUILDCOMBAT_RANK )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "range over" ) );
		return;
	}

	m_listRank[m_nMax].strName    = CPlayerDataCenter::GetInstance()->GetPlayerString( uidPlayer );		//sun: 11, 캐릭터 정보 통합

	m_listRank[m_nMax].strJob     = prj.m_aJob[ nJob ].szName;	
	m_listRank[m_nMax].uidPlayer  = uidPlayer;
	m_listRank[m_nMax].nPoint     = nPoint;
	
	m_nMax++;	

	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_GUILDCOMBAT_RANK_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}		
}


/****************************************************
  WndId : APP_FONTEDIT
****************************************************/
CWndFontEdit::CWndFontEdit() 
{ 
	m_pTexture = NULL;
	m_pWndText = NULL;
	memset( m_ColorRect, 0, sizeof(m_ColorRect) );	
} 
CWndFontEdit::~CWndFontEdit() 
{ 
} 
void CWndFontEdit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"yellowbuttten.tga" ), 0xffff00ff, TRUE );

	m_ColorRect[0].left   = 38;
	m_ColorRect[0].top    = 46;
	m_ColorRect[0].right  = 156;
	m_ColorRect[0].bottom = 63;
	
	m_ColorRect[1].left   = 38;
	m_ColorRect[1].top    = 68;
	m_ColorRect[1].right  = 156;
	m_ColorRect[1].bottom = 83;
	
	m_ColorRect[2].left   = 38;
	m_ColorRect[2].top    = 89;
	m_ColorRect[2].right  = 156;
	m_ColorRect[2].bottom = 103;

	m_ColorScrollBar[0] = 0;
	m_ColorScrollBar[1] = 0;
	m_ColorScrollBar[2] = 0;

	m_bLButtonClick = FALSE;

	m_fColor[0] = m_fColor[1] = m_fColor[2] = 0.0f;

	ReSetBar( m_fColor[0], m_fColor[1], m_fColor[2] );	
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
BOOL CWndFontEdit::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FONTEDIT, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndFontEdit::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
BOOL CWndFontEdit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( m_pWndText == NULL )
		return FALSE;

	CWndButton* pWndCheck = NULL;

	switch( nID )
	{
		case WIDC_CHECK1:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_BOLD);
//				m_pWndText->BlockInsert( "#b", "#nb" );
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_BOLD);
//				m_pWndText->BlockDelete( "#b", "#nb" );
			}
			break;
		case WIDC_CHECK2:
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);			

			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_UNDERLINE);
//				m_pWndText->BlockInsert( "#u", "#nu" );				
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_UNDERLINE);
//				m_pWndText->BlockDelete( "#u", "#nu" );				
			}
			break;
		case WIDC_CHECK3:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);	
			
			if( pWndCheck->GetCheck() )
			{
				m_pWndText->BlockSetStyle(ESSTY_STRIKETHROUGH);
//				m_pWndText->BlockInsert( "#s", "#ns" );				
			}
			else
			{
				m_pWndText->BlockClearStyle(ESSTY_STRIKETHROUGH);
//				m_pWndText->BlockDelete( "#s", "#ns" );				
			}
			break;
		case WIDC_CHECK4:			
			pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);	

			if( pWndCheck->GetCheck() )
			{
				DWORD dwR, dwG, dwB, dwColor;
				dwR = (DWORD)( m_fColor[0] * 255 );
				dwG = (DWORD)( m_fColor[1] * 255 );
				dwB = (DWORD)( m_fColor[2] * 255 );
				
				dwColor = D3DCOLOR_XRGB(dwR, dwG, dwB);
				
				CString str;
				str.Format( "#c%x", dwColor );
						
				m_pWndText->BlockSetColor( dwColor );
//				m_pWndText->BlockInsert( str, "#nc" );				
			}
			else
			{
				m_pWndText->BlockSetColor( 0xff000000 );
//				m_pWndText->BlockDelete( "#c", "#nc" );				
			}

			break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndFontEdit::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bLButtonClick = FALSE;
}
void CWndFontEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bLButtonClick = TRUE;
}
void CWndFontEdit::OnMouseWndSurface( CPoint point )
{
	if( g_pPlayer == NULL )
		return;
	
	CRect rect = CRect( 17, 17, 186, 148 );
	
	if( !rect.PtInRect( point ) )
		m_bLButtonClick = FALSE;
	
	for( int i=0; i<3; i++ )
	{
		CRect DrawRect = m_ColorRect[i];
		
		DrawRect.top    -= 22;
		DrawRect.bottom -= 22;
		
		if( DrawRect.PtInRect( point ) && m_bLButtonClick )
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
			
			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos   = point.x - DrawRect.left;
			
			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));
			
			D3DXVECTOR2 vec1= D3DXVECTOR2( 0.0f, 1.0f );
			D3DXVECTOR2 vec2= D3DXVECTOR2( 1.0f, 1.0f );
			D3DXVECTOR2 vec3;
			
			D3DXVec2Lerp( &vec3, &vec1, &vec2, p );
			
			m_fColor[i] = vec3.x;
			
			m_ColorScrollBar[i].x = point.x;
		}
	}
}

void CWndFontEdit::OnDraw( C2DRender* p2DRender )
{	
	for( int i=0; i<3; i++ )
	{
		CPoint pt = CPoint( m_ColorScrollBar[i].x - ( m_pTexture->m_size.cx / 2 ), m_ColorScrollBar[i].y );
		
		m_pTexture->Render( p2DRender, pt );
	}
/*	
	for( int j=0; j<3; j++ )
	{
		if( m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x )
			m_pTexture->Render( p2DRender, CPoint( m_OriginalColorScrollBar[j].x - ( m_pTexture->m_size.cx / 2 ), m_OriginalColorScrollBar[j].y ), 160 );
	}
*/

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	DWORD dwR, dwG, dwB;
	dwR = (DWORD)( m_fColor[0] * 255 );
	dwG = (DWORD)( m_fColor[1] * 255 );
	dwB = (DWORD)( m_fColor[2] * 255 );

	p2DRender->RenderFillRect( lpFace->rect, D3DCOLOR_XRGB( dwR, dwG, dwB ) );
}	

void CWndFontEdit::ReSetBar( FLOAT r, FLOAT g, FLOAT b )
{
	FLOAT fR = ((r-0.0f)/(1.0f - 0.0f)) * 100.0f;
	FLOAT fG = ((g-0.0f)/(1.0f - 0.0f)) * 100.0f;
	FLOAT fB = ((b-0.0f)/(1.0f - 0.0f)) * 100.0f;
	
	m_ColorScrollBar[0].x = (LONG)( (((m_ColorRect[0].right-m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left );
	m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	m_ColorScrollBar[1].x = (LONG)( (((m_ColorRect[1].right-m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left );
	m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	m_ColorScrollBar[2].x = (LONG)( (((m_ColorRect[2].right-m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left );
	m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
}

// sun 8차 버프창 관련 적용
CWndBuffStatus::CWndBuffStatus() 
{ 
	m_BuffIconViewOpt = g_Option.m_BuffStatusMode;
} 

CWndBuffStatus::~CWndBuffStatus() 
{ 
} 

void CWndBuffStatus::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	this->DelWndStyle(WBS_CAPTION);
	
	m_wndTitleBar.SetVisible( FALSE );

	SetBuffIconInfo();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point( (rectRoot.right - rectRoot.left) / 3, (rectRoot.bottom - rectRoot.top) / 3);
	Move( point );
} 
BOOL CWndBuffStatus::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_BUFF_STATUS, WBS_NOFOCUS, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndBuffStatus::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

BOOL CWndBuffStatus::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndBuffStatus::OnLButtonUp( UINT nFlags, CPoint point )
{
	this->m_pWndFocus = this;
}

void CWndBuffStatus::OnLButtonDown( UINT nFlags, CPoint point )
{
	if(!GetHitTestResult())
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown( nFlags, p );
			this->m_pWndFocus = g_WndMng.m_pWndWorld;
		}
	}
}

void CWndBuffStatus::OnRButtonUp( UINT nFlags, CPoint point )
{
}

void CWndBuffStatus::OnRButtonDown( UINT nFlags, CPoint point )
{
	if(!GetHitTestResult())
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown( nFlags, p );
		}
	}
}

void CWndBuffStatus::OnMouseWndSurface( CPoint point )
{
}

void CWndBuffStatus::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(GetHitTestResult())
	{
		if(m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if(m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDblClk( nFlags, p );
		}
	}
}

void CWndBuffStatus::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	if(GetHitTestResult())
	{
		if(m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if(m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if( g_WndMng.m_pWndWorld )
		{
			CRect rect = GetWindowRect( TRUE );
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDblClk( nFlags, p );
		}
	}
}

BOOL CWndBuffStatus::GetHitTestResult()
{
	BOOL rtn_val = FALSE;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;
	
	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for( BuffItr i = g_pPlayer->m_buffs.m_listBuffs.begin(); i!= g_pPlayer->m_buffs.m_listBuffs.end(); ++i )
	{
		IBuff* pBuff	= *i;
		if( pBuff != NULL && pBuff->GetType() == BUFF_SKILL )
		{
			buffinfo  = *it;
			SetRect( &rectHittest, buffinfo.pt.x, buffinfo.pt.y, buffinfo.pt.x+34, buffinfo.pt.y+34 );
			ClientToScreen( &rectHittest );
			if( PtInRect(&rectHittest, ptMouse ) )
				rtn_val = TRUE;
			it++;
		}
	}
	return rtn_val;
}

void CWndBuffStatus::SetBuffIconInfo()
{
	BUFFICON_INFO buffinfo;
	int x = 0;
	int y = 0;
	int i;
	CRect rect;
	rect = GetWindowRect(TRUE);
	
	if(!m_pBuffIconInfo.empty())
		m_pBuffIconInfo.clear();

	if(m_BuffIconViewOpt == 0)
	{
		for(i=0; i<MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint( x, y );
			m_pBuffIconInfo.push_back(buffinfo);
			x += 34;
			if(i == 6)
			{
				x = 0;
				y = 34;
			}
#ifdef SKILL_BUFF21
			if(i == 13)
			{
				x = 0;
				y = 68;
			}
#endif	// SKILL_BUFF21
		}
		//widht 238, heigth = 68
#ifdef SKILL_BUFF21
		rect.bottom = 102 + rect.top;
#else
		rect.bottom = 68 + rect.top;
#endif	// SKILL_BUFF21
		rect.right = 238 + rect.left;
	}
	else if(m_BuffIconViewOpt == 1)
	{
		for(i=0; i<MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint( x, y );
			m_pBuffIconInfo.push_back(buffinfo);
			y += 34;
			if(i == 6)
			{
				y = 0;
				x = 34;
			}
#ifdef SKILL_BUFF21
			if(i == 13)
			{
				y = 0;
				x = 68;
			}
#endif	// SKILL_BUFF21
		}
		//widht 54, heigth = 238
		rect.bottom = 238 + rect.top;
#ifdef SKILL_BUFF21
		rect.right = 102 + rect.left;
#else
		rect.right = 68 + rect.left;
#endif	// SKILL_BUFF21
	}
	SetWndRect(rect);
	AdjustWndBase();
}

void CWndBuffStatus::PaintFrame( C2DRender* p2DRender )
{
}

void CWndBuffStatus::OnDraw( C2DRender* p2DRender )
{	
	if( g_pPlayer == NULL )
		return;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen( &ptMouse );
	BUFFICON_INFO buffinfo;

	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for( BuffItr i = g_pPlayer->m_buffs.m_listBuffs.begin(); i != g_pPlayer->m_buffs.m_listBuffs.end(); ++i )
	{
		IBuff* pBuff	= *i;
		if( pBuff != NULL && pBuff->GetType() == BUFF_SKILL )
		{
			buffinfo  = *it;
			RenderBuffIcon( p2DRender, pBuff, TRUE, &buffinfo, ptMouse );
			it++;
		}
	}
}

void CWndBuffStatus::RenderBuffIcon( C2DRender *p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse )
{
	int nTexture;
	RECT rectHittest;	
	std::multimap< DWORD, BUFFSKILL >::iterator iter;

	PFONTCOLOR_WNDBUFFSTATUS pFontColorWndBuffStatus = g_WndFontColorManager->GetWndBuffStatus();

	if( bPlayer )
		nTexture = 0;
	else
		nTexture = 1;

	WORD wID = pBuff->GetId();
	if( pBuff->GetType() == BUFF_SKILL )
	{
		SkillProp* pSkillProp = NULL;
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].find( wID );

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].end() )
			return;

		if( bPlayer )
			pSkillProp = prj.GetSkillProp( wID );
		else
			pSkillProp = prj.GetPartySkill( wID );

		std::multimap< DWORD, BUFFSKILL >::value_type* pp;
		pp = &(*iter);

		if( pSkillProp == NULL )
		{
			return;
		}

		if( pp->second.m_pTexture == NULL )
			return;

		BOOL bFlash = FALSE;
		DWORD dwOddTime = 0;

		if( pBuff->GetTotal() > 0 )
		{	
			dwOddTime = pBuff->GetTotal() - ( g_tmCurrent - pBuff->GetInst() );
			bFlash = ( dwOddTime < 20 * 1000 );		// 20초 이하 남았으면 깜빡거림

			if( pBuff->GetTotal() < ( g_tmCurrent - pBuff->GetInst() ) ) // - 가 되면 0으로 처리
				dwOddTime = 0;
		}

		D3DXCOLOR color;

		if( bFlash )		
		{		
			if( pp->second.m_bFlsh == TRUE )
			{
				pp->second.m_nAlpha+=6;

				if( pp->second.m_nAlpha > 192 )
				{
					pp->second.m_nAlpha = 192;
					pp->second.m_bFlsh = FALSE;
				}
			}
			else
			{
				pp->second.m_nAlpha-=6;

				if( pp->second.m_nAlpha < 64 )
				{
					pp->second.m_nAlpha = 64;
					pp->second.m_bFlsh = TRUE;
				}
			}

			if( pSkillProp->IsDebuff() == TRUE )							// 나쁜마법은
				color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 120, 255 );		// 빨간 색 
			else
				color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 255, 255 );

			p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );		
		}
		else
		{
			if( pSkillProp->IsDebuff() == TRUE )							// 나쁜마법은
				color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// 빨간 색 
			else
				color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );

			p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );
		}

		SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+28, pInfo->pt.y+28 );
		ClientToScreen( &rectHittest );

		CEditString strEdit;
		CString strLevel;
		strLevel.Format("  %s %d",prj.GetText(TID_SKILL_BUFFICONLEVEL01), pBuff->GetLevel() );
		
		/*
		TODO SKILLRARE
		if( pSkillProp->dwSkillRare_ == 102 )
		{
			strEdit.AddString( pSkillProp->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
			strEdit.AddString( strLevel, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		}
		else if( pSkillProp->dwSkillRare_ == 103 )
		{
			strEdit.AddString( pSkillProp->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
			strEdit.AddString( strLevel, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		}
		else*/
		{
			strEdit.AddString( pSkillProp->szName, 0xff2fbe6d, ESSTY_BOLD );
			strEdit.AddString( strLevel, 0xff2fbe6d, ESSTY_BOLD );
		}

		CString strTemp;
		strTemp.Format( "\n%s", pSkillProp->szCommand );

		strEdit.AddString( strTemp );

		if( PtInRect(&rectHittest, ptMouse ) )
		{
			g_WndMng.PutDestParam( pSkillProp->dwDestParam[0], pSkillProp->dwDestParam[1],
				pSkillProp->nAdjParamVal[0], pSkillProp->nAdjParamVal[1], strEdit );

			if( pBuff->GetType() == BUFF_SKILL && 
				pBuff->GetId() != SKILL_INDEX( 316, SI_RIG_MASTER_BLESSING ) && 
				pBuff->GetId() != SKILL_INDEX( 113, SI_ASS_CHEER_STONEHAND ) && 
				pBuff->GetId() != SKILL_INDEX( 37, SI_MAG_EARTH_LOOTING ) && 
				pBuff->GetId() != SKILL_INDEX( 48, SI_ASS_HEAL_PREVENTION ) ) //현자의 축복, 스톤 핸드, 루팅, 프리벤션 제외
			{
				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp( pSkillProp->dwSubDefine, pBuff->GetLevel() );

				if( pAddSkillProp )
				{
					g_WndMng.PutDestParam( pAddSkillProp->dwDestParam[0], pAddSkillProp->dwDestParam[1],
						pAddSkillProp->dwAdjParamVal[0], pAddSkillProp->dwAdjParamVal[1], strEdit );
				}
			}
		}

		CString str;

		if( pBuff->GetTotal() > 0 )
		{
			CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌

			if( ct.GetDays() != 0 )
			{
				str.Format( prj.GetText( TID_MMI_BUFFTIME_DAY_HOUR_MINUTE_SECOND ), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
			}
			else
			{
				if( ct.GetHours() >= 1 )
				{
					str.Format( prj.GetText( TID_MMI_BUFFTIME_HOUR_MINUTE_SECOND01 ), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
				}
				else
				{
					str.Format( prj.GetText( TID_MMI_BUFFTIME_MINUTE_SECOND01 ), ct.GetMinutes(), ct.GetSeconds() );						// 분초
				}
			}
			RenderOptBuffTime( p2DRender, pInfo->pt, ct, pFontColorWndBuffStatus->m_stBuffSkillTime.GetFontColor() );

			strEdit.AddString( "\n" );
			strEdit.AddString( str );
		}

		g_toolTip.PutToolTip( wID, strEdit, rectHittest, ptMouse, 1 );	

		++pInfo->nCount;
	}
	else	// BUFF_ITEM
	{
		PT_ITEM_SPEC pItemProp = NULL;
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].find( wID );

		if( iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].end() )
			return;

		pItemProp = g_xSpecManager->GetSpecItem( wID );

		std::multimap< DWORD, BUFFSKILL >::value_type* pp;
		pp = &(*iter);

		if( pItemProp == NULL )
		{
			return;
		}

		if( pp->second.m_pTexture == NULL )
			return;

		BOOL bFlash = FALSE;
		DWORD dwOddTime = 0;

		if( pBuff->GetTotal() > 0 )
		{	
			dwOddTime = pBuff->GetTotal() - ( g_tmCurrent - pBuff->GetInst() );
			bFlash = ( dwOddTime < 20 * 1000 );		// 20초 이하 남았으면 깜빡거림

			if( pBuff->GetTotal() < ( g_tmCurrent - pBuff->GetInst() ) ) // - 가 되면 0으로 처리
				dwOddTime = 0;
		}

		D3DXCOLOR color;

		if( bFlash )		
		{		
			if( pp->second.m_bFlsh == TRUE )
			{
				pp->second.m_nAlpha+=6;

				if( pp->second.m_nAlpha > 192 )
				{
					pp->second.m_nAlpha = 192;
					pp->second.m_bFlsh = FALSE;
				}
			}
			else
			{
				pp->second.m_nAlpha-=6;

				if( pp->second.m_nAlpha < 64 )
				{
					pp->second.m_nAlpha = 64;
					pp->second.m_bFlsh = TRUE;
				}
			}

			if( pItemProp->IsDebuff() == TRUE )							// 나쁜마법은
				color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 120, 255 );		// 빨간 색 
			else
				color =  D3DCOLOR_ARGB( pp->second.m_nAlpha, 255, 255, 255 );

			p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );		
		}
		else
		{
			if( pItemProp->IsDebuff() == TRUE )							// 나쁜마법은
				color =  D3DCOLOR_ARGB( 192, 255, 120, 255 );		// 빨간 색 
			else
				color =  D3DCOLOR_ARGB( 192, 255, 255, 255 );

			p2DRender->RenderTexture2( pInfo->pt, pp->second.m_pTexture, 1.2f, 1.2f, color );
		}

		SetRect( &rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x+28, pInfo->pt.y+28 );
		ClientToScreen( &rectHittest );

		CEditString strEdit;
		CString strLevel;
	
		strLevel.Format("   %s %d", prj.GetText(TID_SKILL_BUFFICONLEVEL01), pBuff->GetLevel() );

		if( pItemProp->dwItemRare == 102 )
		{
			strEdit.AddString( pItemProp->szName, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
			strEdit.AddString( strLevel, D3DCOLOR_XRGB( 0, 93, 0 ), ESSTY_BOLD );
		}
		else if( pItemProp->dwItemRare == 103 )
		{
			strEdit.AddString( pItemProp->szName, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
			strEdit.AddString( strLevel, D3DCOLOR_XRGB( 182, 0, 255 ), ESSTY_BOLD );
		}
		else
		{
			strEdit.AddString( pItemProp->szName, 0xff2fbe6d, ESSTY_BOLD );
			strEdit.AddString( strLevel, 0xff2fbe6d, ESSTY_BOLD );
		}

		CString str;

		if( pBuff->GetTotal() > 0 )
		{
			CTimeSpan ct( (long)(dwOddTime / 1000.0f) );		// 남은시간을 초단위로 변환해서 넘겨줌

			if( ct.GetDays() != 0 )
			{
				//str.Format( "\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
				str.Format( prj.GetText(TID_MMI_BUFFTIME_DAY_HOUR_MINUTE_SECOND), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
			}
			else
			{
// 				if( ct.GetHours() >= 1 )
// 					str.Format( "\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
// 				else
// 					str.Format( "\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds() );						// 분초
				if( ct.GetHours() >= 1 )
					str.Format( prj.GetText(TID_MMI_BUFFTIME_HOUR_MINUTE_SECOND01), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds() );	//시분초 
				else
					str.Format( prj.GetText(TID_MMI_BUFFTIME_MINUTE_SECOND01), ct.GetMinutes(), ct.GetSeconds() );						// 분초

			}
			RenderOptBuffTime( p2DRender, pInfo->pt, ct, pFontColorWndBuffStatus->m_stBuffSkillTime.GetFontColor() );
		}

		CString strTemp;
		strTemp.Format( "\n%s", pItemProp->szCommand );

		strEdit.AddString( strTemp );

		if( PtInRect(&rectHittest, ptMouse ) )
		{
			g_WndMng.PutDestParam( pItemProp->dwDestParam[0], pItemProp->dwDestParam[1],
				pItemProp->nAdjParamVal[0], pItemProp->nAdjParamVal[1], strEdit );
		}

		strEdit.AddString( str );

		g_toolTip.PutToolTip( wID, strEdit, rectHittest, ptMouse, 1 );	

		++pInfo->nCount;
	}
}

void CWndBuffStatus::RenderOptBuffTime(C2DRender *p2DRender, CPoint& point, CTimeSpan &ct, DWORD dwColor )
{
	PFONTCOLOR_WNDBUFFSTATUS pFontColorWndBuffStatus = g_WndFontColorManager->GetWndBuffStatus();

	if(g_Option.m_bVisibleBuffTimeRender)
	{
		CString str;
		str.Format( "%d", ct.GetTotalSeconds() );
		p2DRender->TextOut(  point.x+4, point.y+22, str, dwColor, pFontColorWndBuffStatus->m_stBuffSkillTimeShadow.GetFontColor() );
	}
}

//sun: 9차 전승관련 Clienet
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewel
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndMixJewel::CWndMixJewel() 
{
	m_nSelectCtrl = -1;
	m_nOrichalcum = 0;
	m_nMoonstone = 0;
	m_pText = NULL;
	m_nitemcount = 0;
	m_pWndInventory = NULL;
	m_pConfirm = NULL;
	m_bStart = FALSE;

	// cr : uw : initialize
	memset( m_MatJewel, 0, sizeof(m_MatJewel) );
	memset( m_ItemInfo, 0, sizeof(m_ItemInfo) );
}

CWndMixJewel::~CWndMixJewel() 
{ 
	DeleteDeviceObjects();
	SAFE_DELETE(m_pConfirm);	// if(m_pConfirm != NULL)
} 

void CWndMixJewel::OnDestroy()
{
	for(int i=0; i<MAX_JEWEL; i++)
	{
		if(m_MatJewel[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_MatJewel[i].pItemElem ) )
				m_MatJewel[i].pItemElem->SetExtra(0);
		}
	}

	if(m_pWndInventory != NULL)
		m_pWndInventory->CISetDieFlag_ItemCtrl( FALSE );
}

void CWndMixJewel::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
}

void CWndMixJewel::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pConfirm == pWndChild )
		SAFE_DELETE( m_pConfirm );
}

void CWndMixJewel::OnDraw( C2DRender* p2DRender ) 
{ 
	//Render Icon
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;
	BOOL bCheckSlot = TRUE;
	
	CPoint point = GetMousePoint();
	int testnum = HitTest( point );
	if( testnum != -1)
	{
		if( CWndBase::m_GlobalShortcut.m_dwData )
		{
			m_nSelectCtrl = testnum;
			CRect rect;
			rect = m_MatJewel[m_nSelectCtrl].wndCtrl->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
	}
	else
		m_nSelectCtrl = -1;
	
	for(int i=0; i<MAX_JEWEL; i++)
	{
		if(m_MatJewel[i].isUse)
		{
			if(m_MatJewel[i].pItemElem)
			{
				pItemProp = g_xSpecManager->GetSpecItem( m_MatJewel[i].pItemElem->GetProp()->dwID );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( m_MatJewel[i].wndCtrl->rect.left, m_MatJewel[i].wndCtrl->rect.top ) );
				}
			}
		}
		else
		{
			if( i < 5 )
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) );
			else 
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) );
			if(pItemProp != NULL)
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( m_MatJewel[i].wndCtrl->rect.left, m_MatJewel[i].wndCtrl->rect.top ), 50 );
				bCheckSlot = FALSE;
			}
		}
	}

	if(!m_bStart)
		SetStartBtn(bCheckSlot);
	else
		SetStartBtn(FALSE);
}

void CWndMixJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	//Ctrl Initialize.
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	int StaticID[20] = {WIDC_PIC_SLOT1, WIDC_PIC_SLOT2, WIDC_PIC_SLOT3, WIDC_PIC_SLOT4, WIDC_PIC_SLOT5, 
						WIDC_PIC_SLOT6,	WIDC_PIC_SLOT7, WIDC_PIC_SLOT8, WIDC_PIC_SLOT9, WIDC_PIC_SLOT10};

	for(int i=0; i<MAX_JEWEL; i++)
	{
		m_MatJewel[i].wndCtrl = GetWndCtrl( StaticID[i] );
		m_MatJewel[i].staticNum = StaticID[i];
		m_MatJewel[i].isUse = FALSE;
		m_MatJewel[i].pItemElem = NULL;
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}

	SetDescription(NULL);

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);

	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
    if(m_pWndInventory != NULL)
		m_pWndInventory->CISetDieFlag_ItemCtrl( TRUE );
	
	MoveParentCenter();
}

void CWndMixJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltMixJewel.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMixJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_MIXJEWEL, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndMixJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMixJewel::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndMixJewel::OnMouseMove( UINT nFlags, CPoint point )
{
}

void CWndMixJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndMixJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

void CWndMixJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int choicenum = HitTest( point );
	if(choicenum > -1 && m_MatJewel[choicenum].isUse)
	{
		m_MatJewel[choicenum].isUse = FALSE;

		if(choicenum >= 0 && choicenum < 10)
			m_nOrichalcum--;
		else if(choicenum > 9 && choicenum < 20)
			m_nMoonstone--;

		m_MatJewel[choicenum].pItemElem->SetExtra(m_MatJewel[choicenum].pItemElem->GetExtra()-1);
	}
}

void CWndMixJewel::OnRButtonDblClk( UINT nFlags, CPoint point )
{
}

BOOL CWndMixJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int isValid = ITEM_INVALID;
	FLItemElem* pItemElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( m_nSelectCtrl > -1 && !m_MatJewel[m_nSelectCtrl].isUse)
	{
		if( m_nSelectCtrl < 5 && (pItemElem->GetProp()->dwID == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pItemElem->GetProp()->dwID == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 )))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}
		else if( m_nSelectCtrl > 4 && m_nSelectCtrl < 10 && (pItemElem->GetProp()->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemElem->GetProp()->dwID == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 )))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}

	}	
	switch (isValid)
	{
		case ITEM_VALID:
			SetJewel(pItemElem);
			break;					
		case ITEM_MAX_OVERFLOW:
			// Max overflow Error Msg.
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR1 ) );
			break;
		case ITEM_INVALID:
			break;
	}
	return TRUE;
}

void CWndMixJewel::SetJewel(FLItemElem* pItemElem)
{
	int ptcount, extslotcount, usableitemnum, insertnum, i;
	int slotnum[10];
	ptcount = 0;
	extslotcount = 0;
	usableitemnum = 0;
	insertnum = 0;

	if(pItemElem != NULL)
	{
		if(m_nSelectCtrl > -1)
		{
			if(m_nSelectCtrl >=0 && m_nSelectCtrl < 5)
				ptcount = 0;
			else if(m_nSelectCtrl >= 5 && m_nSelectCtrl < 10)
				ptcount = 5;

			if(g_WndMng.m_pWndWorld->m_bShiftPushed)
			{
				int maxcount = ptcount + 5;
				for(i=ptcount; i<maxcount; i++)
				{
					if(!m_MatJewel[i].isUse)
					{
						slotnum[extslotcount] = i;
						extslotcount++;
					}
				}
				usableitemnum = pItemElem->m_nItemNum - pItemElem->GetExtra();
				
				if(extslotcount > 0 && usableitemnum > 0)
				{
					if(extslotcount > usableitemnum) //남은 Slot과 사용가능한 Item갯 수를 파악하여 한번에 넣을 갯 수를 지정.
						insertnum = usableitemnum;
					else
						insertnum = extslotcount;

					for(i=0; i<insertnum; i++)
					{
						m_MatJewel[slotnum[i]].isUse = TRUE;
						m_MatJewel[slotnum[i]].pItemElem = pItemElem;
					}
					pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);
				}
			}
			else
			{
				insertnum++;
				m_MatJewel[m_nSelectCtrl].isUse = TRUE;
				m_MatJewel[m_nSelectCtrl].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);	
			}

			switch(ptcount)
			{
			case 0:
				m_nOrichalcum += insertnum;
				break;
			case 5:
				m_nMoonstone += insertnum;
				break;
			}
		}
		else //Inventory Dbl Clk...
		{
			int nSelect = -1;
			int count = 0;
			BOOL stopcheck = FALSE;
			if(pItemElem->GetProp()->dwID == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pItemElem->GetProp()->dwID == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 ))
				ptcount = 0;
			else if(pItemElem->GetProp()->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemElem->GetProp()->dwID == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ))
				ptcount = 5;

			count += ptcount;
			while(!stopcheck && count < ptcount + 5)
			{
				if(!m_MatJewel[count].isUse)
				{
					stopcheck = TRUE;
					nSelect = count;
				}
				count++;					
			}
			
			if(nSelect > -1)
			{
				m_MatJewel[nSelect].isUse = TRUE;
				m_MatJewel[nSelect].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				if(ptcount == 0)
					m_nOrichalcum++;
				else if(ptcount == 5)
					m_nMoonstone++;
			}
		}
	}
}

BOOL CWndMixJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		if(	m_nOrichalcum + m_nMoonstone < 10 )
		{
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR2 ) );
		}
		else
		{	
			m_bStart = TRUE;
			
			m_nitemcount = 0;
			for(int i=0; i<MAX_JEWEL; i++)
			{
				if(m_MatJewel[i].isUse)
				{
					if(m_MatJewel[i].pItemElem != NULL && m_MatJewel[i].pItemElem->GetExtra() > 0)
					{
						BOOL equalflag = FALSE;
						for(int j=0; j<m_nitemcount; j++)
						{
							if(m_ItemInfo[j].itemid == m_MatJewel[i].pItemElem->m_dwObjId)
							{
								m_ItemInfo[j].extracount++;
								equalflag = TRUE;
							}
						}
						if(!equalflag)
						{
							m_ItemInfo[m_nitemcount].itemid = m_MatJewel[i].pItemElem->m_dwObjId;
							m_ItemInfo[m_nitemcount].extracount++;
							m_nitemcount++;
						}
					}
				}		
			}
			
			CString sendstr;
			OBJID itemobjid[MAX_JEWEL];
			int index = 0;

			for(int i=0; i<m_nitemcount; i++)
			{
				for(int j=0; j<m_ItemInfo[i].extracount; j++)
				{
					itemobjid[index] = m_ItemInfo[i].itemid;
					index++;
				}
			}

			g_DPlay.SendUltimateMakeItem(itemobjid);
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

int CWndMixJewel::HitTest( CPoint point )
{
	int rtn_val = -1;
	CRect rect;
	for(int i=0; i<MAX_JEWEL; i++)
	{
		rect = m_MatJewel[i].wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = MAX_JEWEL;
		}
	}
	return rtn_val;
}

void CWndMixJewel::ReceiveResult(int nResult)
{
	//Server에서 결과를 받으면 초기화를 진행하고 창을 띄운다.
	//초기화
	for(int i=0; i<MAX_JEWEL; i++)
	{
		m_MatJewel[i].isUse = FALSE;
		m_MatJewel[i].pItemElem->SetExtra(0);
		m_MatJewel[i].pItemElem = NULL;
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}
	m_bStart = FALSE;
	
	switch(nResult) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			{
				SetStartBtn(FALSE);
				SAFE_DELETE( m_pConfirm );	// cr : uw : leak
				m_pConfirm = new CWndMixJewelConfirm();
				m_pConfirm->Initialize( this );		// cr : uw : 
			}
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
		case CUltimateWeapon::ULTIMATE_CANCEL:
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
			break;			
	}
}

void CWndMixJewel::SetStartBtn(BOOL buse)
{
	CWndButton* pbutton = (CWndButton*)GetDlgItem(WIDC_START);
	pbutton->EnableWindow(buse);		
}

void CWndMixJewel::SetConfirmInit()
{
	m_pConfirm = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewelConfirm Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndMixJewelConfirm::CWndMixJewelConfirm() 
{ 
}
 
CWndMixJewelConfirm::~CWndMixJewelConfirm() 
{ 
} 

void CWndMixJewelConfirm::OnDestroy()
{
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase( APP_SMELT_MIXJEWEL );
	if(pWndMixJewel != NULL)
		pWndMixJewel->SetConfirmInit();
}

void CWndMixJewelConfirm::OnDraw( C2DRender* p2DRender ) 
{
	int itemID = ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 );
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;

	pItemProp = g_xSpecManager->GetSpecItem( itemID );
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
} 

void CWndMixJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pText->SetString(prj.GetText( TID_GAME_MIXJEWEL_SUCCESS ));
	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndMixJewelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_MIXJEWEL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndMixJewelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndMixJewelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndMixJewelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndMixJewelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndMixJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#ifdef COSTUME_UPGRADE_MIX
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndItemMix
////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndItemMix::CWndItemMix(WndMode eWndMode) : m_eWndMode( eWndMode )
{
	m_nSelectCtrl = -1;
	m_pText = NULL;
	m_nitemcount = 0;
	m_pWndInventory = NULL;
	m_pConfirm = NULL;
	m_bStart = FALSE;

	// cr : uw : initialize
	memset( m_Material, 0, sizeof(m_Material) );
	memset( m_ItemInfo, 0, sizeof(m_ItemInfo) );
	
}

CWndItemMix::~CWndItemMix() 
{ 
	DeleteDeviceObjects();
	SAFE_DELETE(m_pConfirm);	// if(m_pConfirm != NULL)
} 

void CWndItemMix::OnDestroy()
{
	for(int i=0; i<MAX_MIXCOSTUME; i++)
	{
		if(m_Material[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Material[i].pItemElem ) )
				m_Material[i].pItemElem->SetExtra(0);
		}
	}

	if(m_pWndInventory != NULL)
		m_pWndInventory->CISetDieFlag_ItemCtrl( FALSE );
}

void CWndItemMix::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
}

void CWndItemMix::OnDestroyChildWnd( CWndBase* pWndChild )
{
	if( m_pConfirm == pWndChild )
		SAFE_DELETE( m_pConfirm );
}

void CWndItemMix::OnDraw( C2DRender* p2DRender ) 
{ 
	//Render Icon
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;
	BOOL bCheckSlot = TRUE;
	
	CPoint point = GetMousePoint();
	int testnum = HitTest( point );
	if( testnum != -1)
	{
		if( CWndBase::m_GlobalShortcut.m_dwData )
		{
			m_nSelectCtrl = testnum;
			CRect rect;
			rect = m_Material[m_nSelectCtrl].wndCtrl->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
	}
	else
		m_nSelectCtrl = -1;
	
	int nCount = 0;
	for(int i=0; i<MAX_MIXCOSTUME; i++)
	{
		if(m_Material[i].isUse)
		{
			if(m_Material[i].pItemElem)
			{
				pItemProp = g_xSpecManager->GetSpecItem( m_Material[i].pItemElem->GetProp()->dwID );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( m_Material[i].wndCtrl->rect.left, m_Material[i].wndCtrl->rect.top ) );
				}
			}
			nCount++;
		}
	}
	int nItemMinCount = 3;

	if( m_eWndMode == WND_MATERIAL )
		nItemMinCount = 5;

	if( nCount < nItemMinCount )
		bCheckSlot = FALSE;

	if(!m_bStart)
		SetStartBtn(bCheckSlot);
	else
		SetStartBtn(FALSE);
}

void CWndItemMix::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	//Ctrl Initialize.
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	int StaticID[5] = {WIDC_STATIC1, WIDC_STATIC2, WIDC_STATIC3, WIDC_STATIC4, WIDC_STATIC5};

	for(int i=0; i<MAX_MIXCOSTUME; i++)
	{
		m_Material[i].wndCtrl = GetWndCtrl( StaticID[i] );
		m_Material[i].staticNum = StaticID[i];
		m_Material[i].isUse = FALSE;
		m_Material[i].pItemElem = NULL;
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}

	SetDescription(NULL);

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);

	m_pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
    if(m_pWndInventory != NULL)
		m_pWndInventory->CISetDieFlag_ItemCtrl( TRUE );

	switch(m_eWndMode)
	{
		case WND_COSTUME:
			SetTitle(prj.GetText(TID_MMI_SMELT_MIXCOSTUME));
			break;
		case WND_MATERIAL:
			SetTitle(prj.GetText(TID_MMI_SMELT_MIXMATERIAL));
			break;
	}
	
	MoveParentCenter();

}

void CWndItemMix::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	if( m_eWndMode == WND_COSTUME )
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltMixCostume.inc" ) ));
	else if( m_eWndMode == WND_MATERIAL )
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltMixMaterial.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndItemMix::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_MIXCOSTUME, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndItemMix::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndItemMix::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndItemMix::OnMouseMove( UINT nFlags, CPoint point )
{
}

void CWndItemMix::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndItemMix::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
}

void CWndItemMix::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int choicenum = HitTest( point );
	if(choicenum > -1 && m_Material[choicenum].isUse)
	{
		m_Material[choicenum].isUse = FALSE;
		m_Material[choicenum].pItemElem->SetExtra(m_Material[choicenum].pItemElem->GetExtra()-1);
	}
}

void CWndItemMix::OnRButtonDblClk( UINT nFlags, CPoint point )
{
}

BOOL CWndItemMix::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int isValid = ITEM_INVALID;
	FLItemElem* pItemElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
	if( m_nSelectCtrl > -1 && !m_Material[m_nSelectCtrl].isUse)
	{
		SetJewel(pItemElem);
		/*if( m_nSelectCtrl < 5 && (pItemElem->GetProp()->dwID == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pItemElem->GetProp()->dwID == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 )))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}
		else if( m_nSelectCtrl > 4 && m_nSelectCtrl < 10 && (pItemElem->GetProp()->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemElem->GetProp()->dwID == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 )))
		{
			if( pItemElem->m_nItemNum > pItemElem->GetExtra() )
				isValid = ITEM_VALID;
			else
				isValid = ITEM_MAX_OVERFLOW;
		}*/

	}	
	//switch (isValid)
	//{
	//	case ITEM_VALID:
	//		SetJewel(pItemElem);
	//		break;					
	//	case ITEM_MAX_OVERFLOW:
	//		// Max overflow Error Msg.
	//		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR1 ) );
	//		break;
	//	case ITEM_INVALID:
	//		break;
	//}
	return TRUE;
}

void CWndItemMix::SetJewel(FLItemElem* pItemElem)
{
	int ptcount, extslotcount, usableitemnum, insertnum;
	ptcount = 0;
	extslotcount = 0;
	usableitemnum = 0;
	insertnum = 0;

	if(pItemElem != NULL)
	{
		if(m_nSelectCtrl >= 0)
		{
			insertnum++;
			m_Material[m_nSelectCtrl].isUse = TRUE;
			m_Material[m_nSelectCtrl].pItemElem = pItemElem;
			pItemElem->SetExtra(pItemElem->GetExtra()+insertnum);	
		}
		else //Inventory Dbl Clk...
		{
			int nSelect = -1;
			int count = 0;
			BOOL stopcheck = FALSE;

			while(!stopcheck && count < MAX_MIXCOSTUME)
			{
				if(!m_Material[count].isUse)
				{
					stopcheck = TRUE;
					nSelect = count;
				}
				count++;					
			}
			
			if(nSelect >= 0)
			{
				m_Material[nSelect].isUse = TRUE;
				m_Material[nSelect].pItemElem = pItemElem;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
			}
		}
	}
}

BOOL CWndItemMix::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
	
		m_bStart = TRUE;
		
		m_nitemcount = 0;
		for(int i=0; i<MAX_MIXCOSTUME; i++)
		{
			if(m_Material[i].isUse)
			{
				if(m_Material[i].pItemElem != NULL && m_Material[i].pItemElem->GetExtra() > 0)
				{
					BOOL equalflag = FALSE;
					for(int j=0; j<m_nitemcount; j++)
					{
						if(m_ItemInfo[j].itemid == m_Material[i].pItemElem->m_dwObjId)
						{
							m_ItemInfo[j].extracount++;
							equalflag = TRUE;
						}
					}
					if(!equalflag)
					{
						m_ItemInfo[m_nitemcount].itemid = m_Material[i].pItemElem->m_dwObjId;
						m_ItemInfo[m_nitemcount].extracount++;
						m_nitemcount++;
					}
				}
			}		
		}
		
		CString sendstr;
		OBJID itemobjid[MAX_MIXCOSTUME];
		int index = 0;

		for(int i=0; i<m_nitemcount; i++)
		{
			for(int j=0; j<m_ItemInfo[i].extracount; j++)
			{
				itemobjid[index] = m_ItemInfo[i].itemid;
				index++;
			}
		}

		//g_DPlay.SendUltimateMakeItem(itemobjid);
		SendPacket();
		
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

int CWndItemMix::HitTest( CPoint point )
{
	int rtn_val = -1;
	CRect rect;
	for(int i=0; i<MAX_MIXCOSTUME; i++)
	{
		rect = m_Material[i].wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = MAX_JEWEL;
		}
	}
	return rtn_val;
}

void CWndItemMix::SendPacket()
{
	FLPacketItemUpgradeMixReq kPacket;
	
	for(int i = 0; i < 5; i++)
	{
		if( m_Material[i].pItemElem != NULL )
		kPacket.dwMaterialItemObjID[i] = m_Material[i].pItemElem->m_dwObjId;
	}
	g_DPlay.SendPacket( &kPacket );
}

void CWndItemMix::ReceiveResult(int nResult, int nItemID)
{
	//Server에서 결과를 받으면 초기화를 진행하고 창을 띄운다.
	//초기화
	for(int i=0; i<MAX_MIXCOSTUME; i++)
	{
		m_Material[i].isUse = FALSE;
		if( m_Material[i].pItemElem != NULL )
		{
			m_Material[i].pItemElem->SetExtra(0);
			m_Material[i].pItemElem = NULL;
		}
		m_ItemInfo[i].extracount = 0;
		m_ItemInfo[i].itemid = -1;
	}
	m_bStart = FALSE;
	
	//switch(nResult) 
	//{
	//	case CUltimateWeapon::ULTIMATE_SUCCESS:
	//		{
	//			SetStartBtn(FALSE);
	//			SAFE_DELETE( m_pConfirm );	// cr : uw : leak
	//			m_pConfirm = new CWndItemMixConfirm(nItemID);
	//			m_pConfirm->Initialize( this );		// cr : uw : 
	//		}
	//		break;
	//	case CUltimateWeapon::ULTIMATE_FAILED:
	//	case CUltimateWeapon::ULTIMATE_CANCEL:
	//		Destroy();
	//		break;
	//	case CUltimateWeapon::ULTIMATE_INVENTORY:
	//		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
	//		break;			
	//}
	switch(nResult)
	{
		case FSC_ITEMUPGRADE_UPGRADE_SUCCESS:
			{
				SetStartBtn(FALSE);
				SAFE_DELETE( m_pConfirm );	// cr : uw : leak
				m_pConfirm = new CWndItemMixConfirm(nItemID);
				m_pConfirm->Initialize( this );		// cr : uw
			}
			break;
		case FSC_ITEMUPGRADE_UPGRADE_FAIL:
			Destroy();
			break;
	}
	
}

void CWndItemMix::SetStartBtn(BOOL buse)
{
	CWndButton* pbutton = (CWndButton*)GetDlgItem(WIDC_START);
	pbutton->EnableWindow(buse);		
}

void CWndItemMix::SetConfirmInit()
{
	m_pConfirm = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndMixJewelConfirm Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndItemMixConfirm::CWndItemMixConfirm(int nResultItemID) : m_nResultItemID(nResultItemID)
{ 
}
 
CWndItemMixConfirm::~CWndItemMixConfirm() 
{ 
} 

void CWndItemMixConfirm::OnDestroy()
{
	CWndItemMix* pWndItemMix = (CWndItemMix*)GetWndBase( APP_SMELT_MIXCOSTUME );
	if(pWndItemMix != NULL)
		pWndItemMix->SetConfirmInit();
}

void CWndItemMixConfirm::OnDraw( C2DRender* p2DRender ) 
{
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;

	pItemProp = g_xSpecManager->GetSpecItem( m_nResultItemID );
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
} 

void CWndItemMixConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_nResultItemID );
	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pText->SetString( pItemProp->szName );
	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndItemMixConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_MIXJEWEL_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndItemMixConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndItemMixConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndItemMixConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndItemMixConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndItemMixConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndExtraction Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndExtraction::CWndExtraction() 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
}

CWndExtraction::~CWndExtraction() 
{ 
} 

void CWndExtraction::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

void CWndExtraction::OnDraw( C2DRender* p2DRender ) 
{
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}

void CWndExtraction::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	//Window Position
/*	CWndInventory* pWndInventory;
	pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(pWndInventory != NULL)
	{
		pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
		rectInventory = pWndInventory->GetWindowRect( TRUE );
	}
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
*/
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndExtraction::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_EXTRACTION, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndExtraction::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndExtraction::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndExtraction::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndExtraction::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndExtraction::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
		}
	}
}

BOOL CWndExtraction::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		//무기 아이템만 올려놓을 수 있다.
		PT_ITEM_SPEC pItemProp;
		FLItemElem* pTempElem;
		pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
			{
				if( (pItemProp->dwReferStat1 == WEAPON_GENERAL && pItemProp->nMinLimitLevel >= 60) || pItemProp->dwReferStat1 == WEAPON_UNIQUE )
				{
					m_pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					m_pEItemProp = m_pItemElem->GetProp();
					m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
					pButton->EnableWindow(TRUE);
				}
			}
		}
	}
	return TRUE;
}

void CWndExtraction::SetWeapon(FLItemElem* pItemElem)
{
	if(m_pItemElem == NULL && pItemElem != NULL)
	{
		PT_ITEM_SPEC pProp = pItemElem->GetProp();
		
		if( pProp->dwItemKind1 == IK1_WEAPON )
		{
			if( (pProp->dwReferStat1 == WEAPON_GENERAL && pProp->nMinLimitLevel >= 60) || pProp->dwReferStat1 == WEAPON_UNIQUE )
			{
				m_pItemElem = pItemElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
			}
		}
	}
}

BOOL CWndExtraction::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);

			g_DPlay.SendUltimateMakeGem(m_pItemElem->m_dwObjId);
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndExtraction::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltExtraction.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndExtraction::ReceiveResult(int result, int nCount)
{
	//결과 값에 따른 처리
	//1. 성공 : Destroy Window
	//2. 실패 : Destroy Window
	//3. 취소 : 인벤토리 여유 공간 부족 및 기타 에러 사항.
	
	// ULTIMATE_SUCCESS : 성공
	// ULTIMATE_FAILED : 실패
	// ULTIMATE_CANCEL : 예외 처리
	// ULTIMATE_ISULTIMATE : 무기가 아니거나 일반, 유니크가 아닐 때
	// ULTIMATE_INVENTORY : 인벤토리가 부족 할 때
	// ULTIMATE_ISNOTULTIMATE : 얼터멋 웨폰이 아닐 때

	
	switch(result) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			{
				CString message;
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( prj.m_UltimateWeapon.GetGemKind( static_cast<DWORD>( m_pEItemProp->nMinLimitLevel ) ) );
				message.Format(prj.GetText( TID_GAME_EXTRACTION_SUCCESS ), pItemProp->szName, nCount);
				g_WndMng.OpenMessageBox( message );
				m_pItemElem = NULL;
			}
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
		case CUltimateWeapon::ULTIMATE_CANCEL:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_FAILED ) );
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );

			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(TRUE);
			break;			
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSmeltJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndSmeltJewel::CWndSmeltJewel(WndMode eWndMode) : m_eWndMode(eWndMode)
{
	m_pMainItem = NULL;
	m_pJewelElem = NULL;
	m_pItemElem = NULL;
	m_pText = NULL;
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;
	
	for(int i=0; i<5; i++)
	{
		m_nJewelSlot[i] = -1;
		m_dwJewel[i] = -1;
	}
	m_fRotate = 0.0f;
	m_fAddRot = 2.0f;
	
	m_nStatus = 0;
	m_nCount = 0;
	m_nDelay = 25;
	m_nAlpha = 0;
	m_nEyeYPos = 0.0f;
	m_bFlash = FALSE;
}

CWndSmeltJewel::~CWndSmeltJewel()
{
// Todo::Model 초기화 필요
//	if(m_pMainItem != NULL)
//		SAFE_DELETE(m_pMainItem);
}

void CWndSmeltJewel::OnDestroy()
{
	if(m_pJewelElem)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJewelElem ) )
			m_pJewelElem->SetExtra(0);
	}
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);		
	}
}

void CWndSmeltJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 윈도를 중앙으로 옮기는 부분.
	m_nJewelSlot[0] = WIDC_STATIC1;
	m_nJewelSlot[1] = WIDC_STATIC2;
	m_nJewelSlot[2] = WIDC_STATIC3;
	m_nJewelSlot[3] = WIDC_STATIC4;
	m_nJewelSlot[4] = WIDC_STATIC5;

	for(int i=0; i<5; i++)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(m_nJewelSlot[i]);
		pStatic->EnableWindow(FALSE);
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);

	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	MoveParentCenter();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSmeltJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_JEWEL, 0, CPoint( 0, 0 ), pWndParent );
} 


BOOL CWndSmeltJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndSmeltJewel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}

void CWndSmeltJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}

void CWndSmeltJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

BOOL CWndSmeltJewel::Process()
{
	if(m_pItemElem != NULL)
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem(WIDC_START);
		
		if(m_nStatus != 1)
		{
			if( (m_nUsableSlot >= 0 && m_nUsableSlot < 5) && m_dwJewel[m_nUsableSlot] != -1)
				pButton->EnableWindow(TRUE);
			else
				pButton->EnableWindow(FALSE);
		}
		else
			pButton->EnableWindow(FALSE);
	}

	if(m_nStatus == 1) //Start!
	{
		if(m_nStatus == 1) //Start버튼 누를 경우 컴퓨터의 선택이 회전하도록 함.
		{
			if(m_nCount > m_nDelay)
			{
				m_fAddRot += 4.0f;
				//( m_fRotate < 1 ) ? m_fRotate = 1 : m_fRotate;
				
				if(m_nDelay <= 25 && m_nDelay > 16) //회전이 끊기는 부분에서 플래쉬를 주자.
					m_bFlash = TRUE;
				
				if(m_nDelay < 10)
				{
					m_nEyeYPos -= 0.4f;
					m_nAlpha += 18;
					(m_nAlpha > 230 ) ? m_nAlpha = 230 : m_nAlpha;
				}

				m_nDelay -= 1;				
				if(m_nDelay < 0)
				{
					m_nStatus = 0;
					m_nDelay = 25;
					m_fRotate = 0.0f;
					m_fAddRot = 2.0f;
					m_nAlpha = 0;
					m_nEyeYPos = 0.0f;
					
					if(m_pItemElem != NULL && m_objJewelId != -1)
						g_DPlay.SendUltimateSetGem(m_pItemElem->m_dwObjId, m_objJewelId);
				}
				
				m_nCount = 0;
			}
			m_nCount++;
		}
	}

	return TRUE;
}

void CWndSmeltJewel::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem == NULL )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

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

	pd3dDevice->SetRenderState( D3DRS_AMBIENT,  D3DCOLOR_ARGB( 255,255,255,255 ) );
	
	CRect rect = GetClientRect();

	// 뷰포트 세팅 
	D3DVIEWPORT9 viewport;

	// 월드 
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScale;
	D3DXMATRIXA16 matRot;
	D3DXMATRIXA16 matTrans;

	// Color
	D3DXCOLOR color;
	
	// 초기화
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matWorld);
	
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	// 무기에 따른 Camera Angle조정
	if(m_pItemElem->GetProp()->dwItemKind3 == IK3_AXE ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_SWD)
	{
		if(m_pItemElem->GetProp()->dwHanded == HD_ONE)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 3.0f;
			vEyePt.z = 0.0f;
			
			vLookatPt.x = 0.6f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
		else if(m_pItemElem->GetProp()->dwHanded == HD_TWO)
		{
			vEyePt.x = 1.0f;
			vEyePt.y = 5.0f;
			vEyePt.z = 0.0f;
			
			vLookatPt.x = 1.2f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_YOYO ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_BOW
#ifdef __ADDWEAPON_CROSSBOW16
			|| m_pItemElem->GetProp()->dwItemKind3 == IK3_CROSSBOW 
#endif //__ADDWEAPON_CROSSBOW16
			)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_WAND)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.4f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if(m_pItemElem->GetProp()->dwItemKind3 == IK3_CHEERSTICK ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_STAFF)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 4.0f;
		vEyePt.z = 0.0f;
		
		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}

	if(m_nStatus == 1) //Start후 이미지 회전 시 시점을 당기기 위함.
	{
		vEyePt.y += m_nEyeYPos;
	}

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Rotation
	m_fRotate += m_fAddRot;
	D3DXMatrixRotationX(&matRot, D3DXToRadian( m_fRotate ) );
	
	// Scaling
	D3DXMatrixScaling(&matScale, 1.5f, 1.5f, 1.5f);	

	LPWNDCTRL lpFace = GetWndCtrl( WIDC_CUSTOM1 );

	// 렌더링 위치 설정
	viewport.X      = p2DRender->m_ptOrigin.x + lpFace->rect.left;
	viewport.Y      = p2DRender->m_ptOrigin.y + lpFace->rect.top;
	viewport.Width  = lpFace->rect.Width();
	viewport.Height = lpFace->rect.Height();

	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffa08080, 1.0f, 0 );

	// Matrix Multiply
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matRot );
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	
	// 랜더링 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
	::SetLight( FALSE );
	::SetFog( FALSE );
	SetDiffuse( 1.0f, 1.0f, 1.0f );
	SetAmbient( 1.0f, 1.0f, 1.0f );

	D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 1.0f );
	pd3dDevice->SetVertexShaderConstantF( 95, (float*)&vConst, 1 );
	::SetTransformView( matView );
	::SetTransformProj( matProj );
	
/*	if(m_nStatus == 1) //깜빡임.
	{
		if(m_bFlash)
		{
			color =  D3DCOLOR_ARGB( 255, 255, 255, 255 );
			p2DRender->RenderFillRect( rect, color );
			m_bFlash = FALSE;
		}
	}
*/
	m_pMainItem->Render( p2DRender->m_pd3dDevice, &matWorld );

	viewport.X      = p2DRender->m_ptOrigin.x;
	viewport.Y      = p2DRender->m_ptOrigin.y;
	viewport.Width  = p2DRender->m_clipRect.Width();
	viewport.Height = p2DRender->m_clipRect.Height();
	viewport.MinZ   = 0.0f;
	viewport.MaxZ   = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );	

	//Jewel Rendering
	CTexture* pTexture;
	PT_ITEM_SPEC pItemProp;
	if(m_pItemElem != NULL)
	{
		for(size_t i=0; i<5; i++)
		{
			LPWNDCTRL pWndCtrl = GetWndCtrl( m_nJewelSlot[i] );
//sun: 12, 무기 피어싱
			if( i < m_pItemElem->GetUltimatePiercingSize() ) //뚫린 소켓
			{
				if(m_dwJewel[i] != -1) //박힌 보석
				{
					if(i != m_nUsableSlot) //새로 박은 보석위에는 색칠하지 않음.
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndDisableBlue.bmp"), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
						//p2DRender->RenderFillRect( pWndCtrl->rect, 0x609370db );
					}

					pItemProp = g_xSpecManager->GetSpecItem( m_dwJewel[i] );
					if(pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
					}
				}
				else
				{
					if(i != m_nUsableSlot) //뚫린 소켓 중 보석을 박을 수 있는 첫번째 슬롯만 빼고 나머지는 회색으로.
					{
						pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndDisableBlack.bmp"), 0xffff00ff );
						if(pTexture != NULL)
							pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
						//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0a8a8a8 );
					}
				}
			}
			else //안 뚫린 소켓
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndDisableRed.bmp"), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
				//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0ff0000 );
			}
		}
	}

	if(m_nStatus == 1)
	{
		color =  D3DCOLOR_ARGB( m_nAlpha, 240, 255, 255 );
				
		CRect rect;
		LPWNDCTRL lpWndCtrl;
					
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		rect = lpWndCtrl->rect;
		p2DRender->RenderFillRect( rect, color );
	}
}

BOOL CWndSmeltJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if(m_nStatus != 0)
		return FALSE;

	FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if ( m_eWndMode == WND_ULTIMATE )
		OnUltimateDropIcon( pShortcut, point );
	else if ( m_eWndMode == WND_BARUNA)
		OnBarunaDropIcon( pShortcut, point );
	
	
	return TRUE;
}

void CWndSmeltJewel::OnUltimateDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CUSTOM1 );		

	//Set Weapon
	FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(pItemElem && (pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC) &&
		(pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE 
//#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
//|| pItemElem->GetProp()->dwReferStat1 == BARUNA_D
//#endif
))
	{	
		if( wndCtrl->rect.PtInRect( point ) )
		{
			//초기화.
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);
			
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra()+1);
			
			m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemElem->m_dwItemId );
			m_pMainItem->InitDeviceObjects( g_Neuz.GetDevice() );
		}
	}
	 
	//SetJewel
//sun: 12, 무기 피어싱
	if(m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
	{
		if( m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
		{
			if(pItemElem && (pItemElem->m_dwItemId == ITEM_INDEX( 2032, II_GEN_MAT_RUBY ) || 
				pItemElem->m_dwItemId == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ )))
			{
				int checkslot = -1;
				for(int i=0; i<5; i++)
				{
					LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[i] );
					if(wndCtrl->rect.PtInRect( point ))
					{
						checkslot = i;
						i = 5;
					}
				}
				if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
				{
					m_dwJewel[checkslot] = pItemElem->m_dwItemId;
					m_objJewelId = pItemElem->m_dwObjId;
					pItemElem->SetExtra(pItemElem->GetExtra()+1);
					m_pJewelElem = pItemElem;
				}
			}
		}
//#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
//		else if( m_pItemElem->GetProp()->dwReferStat1 == BARUNA_D )
//		{
//			if(pItemElem && pItemElem->m_dwItemId == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ))
//			{
//				int checkslot = -1;
//				for(int i=0; i<5; i++)
//				{
//					LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[i] );
//					if(wndCtrl->rect.PtInRect( point ))
//					{
//						checkslot = i;
//						i = 5;
//					}
//				}
//				if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
//				{
//					m_dwJewel[checkslot] = pItemElem->m_dwItemId;
//					m_objJewelId = pItemElem->m_dwObjId;
//					pItemElem->SetExtra(pItemElem->GetExtra()+1);
//					m_pJewelElem = pItemElem;
//				}
//			}
//		}
//#endif
	}
}

#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
void CWndSmeltJewel::OnBarunaDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CUSTOM1 );		

	//Set Weapon
	FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(pItemElem && pItemElem->GetProp()->dwReferStat1 == BARUNA_D && pItemElem->GetProp()->dwItemKind1 == IK1_WEAPON)
	{	
		if( wndCtrl->rect.PtInRect( point ) )
		{
			//초기화.
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);
			
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra()+1);
			
			m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemElem->m_dwItemId );
			m_pMainItem->InitDeviceObjects( g_Neuz.GetDevice() );
		}
	}
	 
	//SetJewel
//sun: 12, 무기 피어싱
	if(m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
	{
		if( m_pItemElem->GetProp()->dwReferStat1 == BARUNA_D )
		{
			if(pItemElem && pItemElem->m_dwItemId == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ))
			{
				int checkslot = -1;
				for(int i=0; i<5; i++)
				{
					LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[i] );
					if(wndCtrl->rect.PtInRect( point ))
					{
						checkslot = i;
						i = 5;
					}
				}
				if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
				{
					m_dwJewel[checkslot] = pItemElem->m_dwItemId;
					m_objJewelId = pItemElem->m_dwObjId;
					pItemElem->SetExtra(pItemElem->GetExtra()+1);
					m_pJewelElem = pItemElem;
				}
			}
		}
	}
}
#endif

void CWndSmeltJewel::SetJewel(FLItemElem* pItemElem)
{
	//SetJewel
//sun: 12, 무기 피어싱
	if( m_nStatus == 0 && m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0 )
	{
		if( m_eWndMode == WND_ULTIMATE && m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
		{
			if(pItemElem && (pItemElem->m_dwItemId == ITEM_INDEX( 2032, II_GEN_MAT_RUBY ) || 
				pItemElem->m_dwItemId == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ )))
			{
				if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1)
				{
					m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
					m_objJewelId = pItemElem->m_dwObjId;
					pItemElem->SetExtra(pItemElem->GetExtra()+1);
					m_pJewelElem = pItemElem;
				}
			}
		}
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		else if( m_eWndMode == WND_BARUNA && m_pItemElem->GetProp()->IsBarunaWeapon())
		{
			if(pItemElem && pItemElem->m_dwItemId == ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ))
			{
				if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1)
				{
					m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
					m_objJewelId = pItemElem->m_dwObjId;
					pItemElem->SetExtra(pItemElem->GetExtra()+1);
					m_pJewelElem = pItemElem;
				}
			}
		}
#endif
	}
}

void CWndSmeltJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[m_nUsableSlot] );		
	if( m_nStatus == 0 && wndCtrl != NULL && wndCtrl->rect.PtInRect( point ) )
	{
		if(m_dwJewel[m_nUsableSlot] != -1)
		{
			if(m_pJewelElem)
				m_pJewelElem->SetExtra(0);
			m_dwJewel[m_nUsableSlot] = -1;
		}
	}
}

void CWndSmeltJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	if(m_eWndMode == WND_ULTIMATE)
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltJewel.inc" ) ));
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
	else if (m_eWndMode == WND_BARUNA)
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltJewelBaruna.inc" ) ));
#endif
	
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

BOOL CWndSmeltJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_START) 
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);
			
			m_nStatus = 1;
			PLAYSND( "PcSkillD-Counter01.wav" );
		}
	}
			
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSmeltJewel::ReceiveResult(int result)
{
	//결과 값에 따른 처리
	//1. 성공 : 초기화
	//2. 실패 : 초기화
	//3. 취소 : 기타.
	
	switch(result) 
	{
	case CUltimateWeapon::ULTIMATE_SUCCESS:
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_SUCCESS ) );
		PLAYSND( "InfUpgradeSuccess.wav" );
		//초기화.
		InitializeJewel(m_pItemElem);
		Destroy();
		break;
	case CUltimateWeapon::ULTIMATE_FAILED:
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_FAIL ) );
		//초기화.
		InitializeJewel(m_pItemElem);		
		break;
	case CUltimateWeapon::ULTIMATE_CANCEL:
		Destroy();
		break;			
	}
}

void CWndSmeltJewel::InitializeJewel(FLItemElem* pItemElem)
{
	//기존 데이타 초기화.
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;

	m_pItemElem = pItemElem;
	
	for(size_t i=0; i<5; i++)
	{
		m_dwJewel[i] = -1;
//sun: 12, 무기 피어싱
		if(i < m_pItemElem->GetUltimatePiercingSize() )
		{
			if(m_pItemElem->GetUltimatePiercingItemID( i ) != 0)
			{
				m_dwJewel[i] = m_pItemElem->GetUltimatePiercingItemID( i );
				m_nJewelCount++;
			}
		}
	}

	//빈 슬롯이 남았는지 확인하여 사용가능 슬롯 번호 저장.
	int m_nSlot = pItemElem->GetUltimatePiercingSize();

	////빈 슬롯이 남았는지 확인하여 사용가능 슬롯 번호 저장.
	//int m_nSlot = pItemElem->GetGeneralPiercingSize();

	if(m_nJewelCount < m_nSlot)
		m_nUsableSlot = m_nJewelCount;
	else
		m_nUsableSlot = -1;
	
	if(m_pJewelElem)
		m_pJewelElem->SetExtra(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndNewSmeltJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
CWndNewSmeltJewel::CWndNewSmeltJewel(WndMode eWndMode) : m_eWndMode(eWndMode)
{
	m_pMainItem = NULL;
	m_pJewelElem = NULL;
	m_pItemElem = NULL;
	m_pText = NULL;
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;

	m_dwMain = -1;

	
	for(int i=0; i<5; i++)
	{
		m_nJewelSlot[i] = -1;
		m_dwJewel[i] = -1;
	}
	m_fRotate = 0.0f;
	m_fAddRot = 2.0f;
	
	m_nStatus = 0;
	m_nCount = 0;
	m_nDelay = 25;
	m_nAlpha = 0;
	m_nEyeYPos = 0.0f;
	m_bFlash = FALSE;
}

CWndNewSmeltJewel::~CWndNewSmeltJewel()
{
// Todo::Model 초기화 필요
//	if(m_pMainItem != NULL)
//		SAFE_DELETE(m_pMainItem);
}

void CWndNewSmeltJewel::OnDestroy()
{
	if(m_pJewelElem)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJewelElem ) )
			m_pJewelElem->SetExtra(0);
	}
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);		
	}
}

void CWndNewSmeltJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 윈도를 중앙으로 옮기는 부분.
	m_nJewelSlot[0] = WIDC_STATIC1;
	m_nJewelSlot[1] = WIDC_STATIC2;
	m_nJewelSlot[2] = WIDC_STATIC3;
	m_nJewelSlot[3] = WIDC_STATIC4;
	m_nJewelSlot[4] = WIDC_STATIC5;

	for(int i=0; i<5; i++)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(m_nJewelSlot[i]);
		pStatic->EnableWindow(FALSE);
	}

	CWndStatic* pMainStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	pMainStatic->EnableWindow(FALSE);

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);

	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	MoveParentCenter();
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndNewSmeltJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_JEWEL_NEW, 0, CPoint( 0, 0 ), pWndParent );
} 


BOOL CWndNewSmeltJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
}

void CWndNewSmeltJewel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}

void CWndNewSmeltJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
}

void CWndNewSmeltJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

BOOL CWndNewSmeltJewel::Process()
{
	if(m_pItemElem != NULL)
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem(WIDC_START);
		
		if(m_nStatus != 1)
		{
			if( (m_nUsableSlot >= 0 && m_nUsableSlot < 5) && m_dwJewel[m_nUsableSlot] != -1)
				pButton->EnableWindow(TRUE);
			else
				pButton->EnableWindow(FALSE);
		}
		else
			pButton->EnableWindow(FALSE);
	}


					
	
		//g_DPlay.SendUltimateSetGem(m_pItemElem->m_dwObjId, m_objJewelId);

	return TRUE;
}

void CWndNewSmeltJewel::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_pItemElem == NULL )
		return;

	CRect rect = GetClientRect();

	//Jewel Rendering
	CTexture* pTexture;
	PT_ITEM_SPEC pItemProp;

	LPWNDCTRL pWndMainCtrl = GetWndCtrl( WIDC_STATIC6 );

	pItemProp = g_xSpecManager->GetSpecItem( m_dwMain );
	if(pItemProp != NULL)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( pWndMainCtrl->rect.left, pWndMainCtrl->rect.top ) );
	}

	for(size_t i=0; i<5; i++)
	{
		LPWNDCTRL pWndCtrl = GetWndCtrl( m_nJewelSlot[i] );
//sun: 12, 무기 피어싱
		if( i < m_pItemElem->GetUltimatePiercingSize() ) //뚫린 소켓
		{
			if(m_dwJewel[i] != -1) //박힌 보석
			{
				if(i != m_nUsableSlot) //새로 박은 보석위에는 색칠하지 않음.
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndDisableBlue.bmp"), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
					//p2DRender->RenderFillRect( pWndCtrl->rect, 0x609370db );
				}

				DWORD dwJewel = ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + m_pItemElem->GetProp()->dwParts - PARTS_HAT;

				pItemProp = g_xSpecManager->GetSpecItem( dwJewel );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
				}
			}
			else
			{
				if(i != m_nUsableSlot) //뚫린 소켓 중 보석을 박을 수 있는 첫번째 슬롯만 빼고 나머지는 회색으로.
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndDisableBlack.bmp"), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );
					//p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0a8a8a8 );
				}
			}
		}
		else //안 뚫린 소켓
		{
			/*pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndDisableRed.bmp"), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( pWndCtrl->rect.left, pWndCtrl->rect.top ) );*/
			p2DRender->RenderFillRect( pWndCtrl->rect, 0xa0ff0000 );
		}
	}
	

	
}

BOOL CWndNewSmeltJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	if(m_nStatus != 0)
		return FALSE;
	if ( m_eWndMode == WND_COSTUME )
		OnCostumeDropIcon( pShortcut, point );
	
	return TRUE;
}

void CWndNewSmeltJewel::OnCostumeDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	LPWNDCTRL wndMainCtrl = GetWndCtrl( WIDC_STATIC6 );		

	//Set Weapon
	FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	if(pItemElem && (pItemElem->GetProp()->IsCostumeEnhanceParts()))
	{	
		if( wndMainCtrl->rect.PtInRect( point ) )
		{
			//초기화.
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);
			
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra()+1);

			m_dwMain = pItemElem->m_dwItemId;
			
			//m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel( g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemElem->m_dwItemId );
			//m_pMainItem->InitDeviceObjects( g_Neuz.GetDevice() );
		}
	}
	 
	
	//SetJewel
// 코스튬 피어싱
	if(m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
	{
		if(pItemElem && (pItemElem->m_dwItemId == ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) || 
			pItemElem->m_dwItemId == ITEM_INDEX( 25339, II_GEN_MAT_TURQUOISE ) ||
			pItemElem->m_dwItemId == ITEM_INDEX( 25340, II_GEN_MAT_AQUAMARINE ) ||
			pItemElem->m_dwItemId == ITEM_INDEX( 25341, II_GEN_MAT_AMETHYST )))
		{
			int checkslot = -1;
			for(int i=0; i<5; i++)
			{
				LPWNDCTRL wndCtrl = GetWndCtrl( m_nJewelSlot[i] );
				if(wndCtrl->rect.PtInRect( point ))
				{
					checkslot = i;
					i = 5;
				}
			}
			if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
			{
				m_dwJewel[checkslot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra()+1);
				m_pJewelElem = pItemElem;
			}
		}
	}
}


void CWndNewSmeltJewel::SetJewel(FLItemElem* pItemElem)
{

	if ( m_nStatus == 0 )
	{
		if(pItemElem->GetProp()->IsCostumeEnhanceParts())
		{	
			//초기화.
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);
			
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra()+1);

			m_dwMain = pItemElem->m_dwItemId;
			
			
		}
		else if( m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0 )
		{
			if((pItemElem->m_dwItemId == ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) || 
				pItemElem->m_dwItemId == ITEM_INDEX( 25339, II_GEN_MAT_TURQUOISE ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 25340, II_GEN_MAT_AQUAMARINE ) ||
				pItemElem->m_dwItemId == ITEM_INDEX( 25341, II_GEN_MAT_AMETHYST )))
			{
				if(m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1)
				{
					m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
					m_objJewelId = pItemElem->m_dwObjId;
					pItemElem->SetExtra(pItemElem->GetExtra()+1);
					m_pJewelElem = pItemElem;
				}
			}
		}
	}
}

void CWndNewSmeltJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_nStatus != 0 )
		return;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC6 );

	if( wndCtrl != NULL && wndCtrl->rect.PtInRect( point ) )
	{
		if(m_dwMain != -1)
		{
			if(m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_dwMain = -1;

			m_nJewelCount = 0;
			m_nUsableSlot = -1;
			m_objJewelId = -1;
			for(size_t i=0; i<5; i++)
			{
				m_dwJewel[i] = -1;
			}
			if(m_pJewelElem)
				m_pJewelElem->SetExtra(0);
		}
	}

	wndCtrl = GetWndCtrl( m_nJewelSlot[m_nUsableSlot] );
	if( wndCtrl != NULL && wndCtrl->rect.PtInRect( point ) )
	{
		if(m_dwJewel[m_nUsableSlot] != -1)
		{
			if(m_pJewelElem)
				m_pJewelElem->SetExtra(0);
			m_dwJewel[m_nUsableSlot] = -1;
		}
	}
}

void CWndNewSmeltJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	if ( m_eWndMode == WND_COSTUME )
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltJewelCostume.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

BOOL CWndNewSmeltJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(message == WNM_CLICKED)
	{
		if(nID == WIDC_START) 
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);
			
			m_nStatus = 1;
			PLAYSND( "PcSkillD-Counter01.wav" );

			if(m_pItemElem != NULL && m_objJewelId != -1)
				SendPacket();
		}
	}
			
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndNewSmeltJewel::SendPacket()
{
	FLPacketItemUpgradeSetGemReq kPacket;
	kPacket.dwMainItemObjID = m_pItemElem->m_dwObjId;
	kPacket.dwGemItemObjID = m_objJewelId;
	g_DPlay.SendPacket( &kPacket );
}

void CWndNewSmeltJewel::ReceiveResult(int result)
{
	//결과 값에 따른 처리
	//1. 성공 : 초기화
	//2. 실패 : 초기화
	//3. 취소 : 기타.
	
	//switch(result) 
	//{
	//case CUltimateWeapon::ULTIMATE_SUCCESS:
	//	g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_SUCCESS ) );
	//	PLAYSND( "InfUpgradeSuccess.wav" );
	//	//초기화.
	//	InitializeJewel(m_pItemElem);
	//	Destroy();
	//	break;
	//case CUltimateWeapon::ULTIMATE_FAILED:
	//	g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_FAIL ) );
	//	//초기화.
	//	InitializeJewel(m_pItemElem);		
	//	break;
	//case CUltimateWeapon::ULTIMATE_CANCEL:
	//	Destroy();
	//	break;			
	//}
	if ( result == FSC_ITEMUPGRADE_UPGRADE_SUCCESS )
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_SUCCESS ) );
		PLAYSND( "InfUpgradeSuccess.wav" );
		//	//초기화.
		InitializeJewel(m_pItemElem);
		Destroy();
	}
	else
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SMELTJEWEL_FAIL ) );
	//	//초기화.
		InitializeJewel(m_pItemElem);
	}
}

void CWndNewSmeltJewel::InitializeJewel(FLItemElem* pItemElem)
{
	//기존 데이타 초기화.
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;

	m_pItemElem = pItemElem;
	
	for(size_t i=0; i<5; i++)
	{
		m_dwJewel[i] = -1;
//sun: 12, 무기 피어싱
		if(i < m_pItemElem->GetUltimatePiercingSize() )
		{
			if(m_pItemElem->GetUltimatePiercingItemID( i ) != 0)
			{
				m_dwJewel[i] = m_pItemElem->GetUltimatePiercingItemID( i );
				m_nJewelCount++;
			}
		}
	}

	//빈 슬롯이 남았는지 확인하여 사용가능 슬롯 번호 저장.
	int m_nSlot = pItemElem->GetUltimatePiercingSize();

	////빈 슬롯이 남았는지 확인하여 사용가능 슬롯 번호 저장.
	//int m_nSlot = pItemElem->GetGeneralPiercingSize();

	if(m_nJewelCount < m_nSlot)
		m_nUsableSlot = m_nJewelCount;
	else
		m_nUsableSlot = -1;
	
	if(m_pJewelElem)
		m_pJewelElem->SetExtra(0);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndChangeWeapon Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndChangeWeapon::CWndChangeWeapon(int nType) 
{
	m_pWItemElem = NULL;
	m_pJItemElem[0] = NULL;
	m_pJItemElem[1] = NULL;
	m_nWeaponType = nType;
	m_bIsSendChange = FALSE;
}

CWndChangeWeapon::~CWndChangeWeapon() 
{ 
} 

void CWndChangeWeapon::OnDestroy()
{
	if(m_pWItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pWItemElem ) )
			m_pWItemElem->SetExtra(0);
	}
	if(m_pJItemElem[0] != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJItemElem[0] ) )
			m_pJItemElem[0]->SetExtra(0);
	}
	if(m_pJItemElem[1] != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pJItemElem[1] ) )
			m_pJItemElem[1]->SetExtra(0);
	}
}

void CWndChangeWeapon::OnDraw( C2DRender* p2DRender ) 
{
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;
	
	if(m_pWItemElem != NULL)
	{
		pItemProp = m_pWItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	
	if(m_pJItemElem[0] != NULL)
	{
		pItemProp = m_pJItemElem[0]->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC2 );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
	
	if(m_pJItemElem[1] != NULL)
	{
		pItemProp = m_pJItemElem[1]->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC3 );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}

void CWndChangeWeapon::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{	
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);
	m_pText1 = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	m_pText2 = (CWndText*)GetDlgItem( WIDC_TEXT2 );

	SetDescription(NULL);

	//Set Caption
	if(m_nWeaponType  == WEAPON_GENERAL)
		SetTitle(prj.GetText(TID_GAME_CHANGEWEAPON_UNIQUE));
	else if(m_nWeaponType  == WEAPON_UNIQUE)
		SetTitle(prj.GetText(TID_GAME_CHANGEWEAPON_ULTIMATE));
	
	//Window Position
/*	CWndInventory* pWndInventory;
	pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(pWndInventory != NULL)
		rectInventory = pWndInventory->GetWindowRect( TRUE );

	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
*/
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndChangeWeapon::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_CHANGEWEAPON, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndChangeWeapon::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndChangeWeapon::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndChangeWeapon::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndChangeWeapon::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndChangeWeapon::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pWItemElem)m_pWItemElem->SetExtra(0);
		m_pWItemElem = NULL;
	}

	wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pJItemElem[0]) m_pJItemElem[0]->SetExtra(0);
		m_pJItemElem[0] = NULL;
	}

	wndCtrl = GetWndCtrl( WIDC_STATIC3 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pJItemElem[1])m_pJItemElem[1]->SetExtra(0);
		m_pJItemElem[1] = NULL;
	}
}

void CWndChangeWeapon::OnMouseWndSurface(CPoint point)
{
	CRect rect1, rect2, rect3;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect1 = wndCtrl->rect;
	wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect2 = wndCtrl->rect;
	wndCtrl = GetWndCtrl( WIDC_STATIC3 );
	rect3 = wndCtrl->rect;

	if( rect1.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect1 );
		CString tooltip;
		if(m_nWeaponType  == WEAPON_GENERAL)
			tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_GENERAL ) );
		else if(m_nWeaponType  == WEAPON_UNIQUE)
			tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_UNIQUE ) );		
		g_toolTip.PutToolTip( (DWORD)this, tooltip, rect1, point );
	}
	else if(rect2.PtInRect( point ))
	{
		ClientToScreen( &point );
		ClientToScreen( &rect2 );
		CString tooltip;
		tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_ORICALCUM ) );
		g_toolTip.PutToolTip( (DWORD)this, tooltip, rect2, point );
	}
	else if(rect3.PtInRect( point ))
	{
		ClientToScreen( &point );
		ClientToScreen( &rect3 );
		CString tooltip;
		tooltip.Format( "%s", prj.GetText( TID_TOOLTIP_CHANGEW_JEWEL ) );
		g_toolTip.PutToolTip( (DWORD)this, tooltip, rect3, point );
	}
}

BOOL CWndChangeWeapon::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	//Set Weapon
	CRect rect;
	PT_ITEM_SPEC pItemProp;
	FLItemElem* pTempElem;
	pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(pTempElem != NULL && m_pWItemElem == NULL)
		{
			pItemProp = pTempElem->GetProp();

			if( pItemProp->dwItemKind1 == IK1_WEAPON && pItemProp->nMinLimitLevel >= 60 ) //60레벨 제한 이상 무기만 적용
			{
				if(m_nWeaponType == pItemProp->dwReferStat1) //창에 따라 유니크인지 일반인지를 확인한다.
				{
					if(m_nWeaponType == WEAPON_UNIQUE)
					{
						if(pTempElem->GetAbilityOption() >= 10) //유니크 무기의 경우 +10 이상인지를 확인 한다.
						{
							m_pWItemElem = pTempElem;
							m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
						}
					}
					else if(m_nWeaponType == WEAPON_GENERAL)
					{
						m_pWItemElem = pTempElem;
						m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
					}
				}
			}
		}
	}
	
	//Set Ligthing Oricalcum
	wndCtrl = GetWndCtrl( WIDC_STATIC2 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(pTempElem && (pTempElem->m_dwItemId == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 )))
		{
			m_pJItemElem[0] = pTempElem;
			m_pJItemElem[0]->SetExtra(m_pJItemElem[0]->GetExtra()+1);
		}
	}
	
	//Set Jewel
	wndCtrl = GetWndCtrl( WIDC_STATIC3 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{	
		if((pTempElem != NULL && m_pWItemElem != NULL) &&
			(pTempElem->m_dwItemId == prj.m_UltimateWeapon.GetGemKind( static_cast<DWORD>( m_pWItemElem->GetProp()->nMinLimitLevel ) ) ) )
		{
			m_pJItemElem[1] = pTempElem;
			m_pJItemElem[1]->SetExtra(m_pJItemElem[1]->GetExtra()+1);
		}
	}

	return TRUE;
}

void CWndChangeWeapon::SetItem(FLItemElem* pItemElem)
{
	if(pItemElem != NULL)
	{
		PT_ITEM_SPEC pProp = pItemElem->GetProp();
		
		if(m_pWItemElem == NULL && pProp->dwItemKind1 == IK1_WEAPON)
		{
			if(pProp->nMinLimitLevel >= 60) //60레벨 제한 이상 무기만 적용
			{
				if(m_nWeaponType == pProp->dwReferStat1) //창에 따라 유니크인지 일반인지를 확인한다.
				{
					if(m_nWeaponType == WEAPON_UNIQUE)
					{
						if(pItemElem->GetAbilityOption() >= 10) //유니크 무기의 경우 +10 이상인지를 확인 한다.
						{
							m_pWItemElem = pItemElem;
							m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
						}
					}
					else if(m_nWeaponType == WEAPON_GENERAL)
					{
						m_pWItemElem = pItemElem;
						m_pWItemElem->SetExtra(m_pWItemElem->GetExtra()+1);
					}
				}
			}
		}
		else if(m_pJItemElem[0] == NULL && pProp->dwID == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ))
		{
			m_pJItemElem[0] = pItemElem;
			m_pJItemElem[0]->SetExtra(m_pJItemElem[0]->GetExtra()+1);
		}
		else if( m_pJItemElem[1] == NULL )
		{
			if(m_pWItemElem != NULL)
			{
				if(pItemElem->m_dwItemId == prj.m_UltimateWeapon.GetGemKind( static_cast<DWORD>( m_pWItemElem->GetProp()->nMinLimitLevel ) ) )
				{
					m_pJItemElem[1] = pItemElem;
					m_pJItemElem[1]->SetExtra(m_pJItemElem[1]->GetExtra()+1);
				}
			}
		}
	}
}

BOOL CWndChangeWeapon::Process()
{
	if(!m_bIsSendChange && m_pWItemElem != NULL && m_pJItemElem[0] != NULL && m_pJItemElem[1] != NULL)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}
	else
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
	}

	return TRUE;
}

BOOL CWndChangeWeapon::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(!m_bIsSendChange && m_pWItemElem != NULL && m_pJItemElem[0] != NULL && m_pJItemElem[1] != NULL)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_bIsSendChange = TRUE;
			
			g_DPlay.SendUltimateTransWeapon(m_pWItemElem->m_dwObjId, m_pJItemElem[1]->m_dwObjId, m_pJItemElem[0]->m_dwObjId);
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndChangeWeapon::SetDescription( CHAR* szChar )
{
	CScript scanner1, scanner2;
	BOOL checkflag;
	CHAR* szInfo;

	if(m_nWeaponType == WEAPON_GENERAL)
	{
		checkflag = scanner1.Load( MakePath( DIR_CLIENT,  _T( "SmeltChangeUniqueWeapon.inc" ) ));
		checkflag = scanner2.Load( MakePath( DIR_CLIENT,  _T( "ChangeUniqueWeaponInfo.inc" ) ));
	}
	else if(m_nWeaponType == WEAPON_UNIQUE)
	{
		checkflag = scanner1.Load( MakePath( DIR_CLIENT,  _T( "SmeltChangeUltimateWeapon.inc" ) ));
		checkflag = scanner2.Load( MakePath( DIR_CLIENT,  _T( "ChangeUltimateWeaponInfo.inc" ) ));
	}

	szChar = scanner1.m_pProg;
	szInfo = scanner2.m_pProg;

	if(m_pText1 != NULL && checkflag)
	{
		m_pText1->m_string.AddParsingString( szChar );
		m_pText1->ResetString();	
	}

	if(m_pText2 != NULL && checkflag)
	{
		m_pText2->m_string.AddParsingString( szInfo );
		m_pText2->ResetString();	
	}
}

void CWndChangeWeapon::ReceiveResult(int result)
{
	//결과 값에 따른 처리
	//1. 성공 : Destroy Window
	//2. 실패 : Destroy Window
	//3. 취소 : 인벤토리 여유 공간 부족 및 기타 에러 사항.
	
	switch(result) 
	{
		case CUltimateWeapon::ULTIMATE_SUCCESS:
			if(m_nWeaponType == WEAPON_UNIQUE)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CUWEAPON_SUCCESS ) );
			else if(m_nWeaponType == WEAPON_GENERAL)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CGWEAPON_SUCCESS ) );
			break;
		case CUltimateWeapon::ULTIMATE_FAILED:
			if(m_nWeaponType == WEAPON_UNIQUE)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CUWEAPON_FAILED ) );
			else if(m_nWeaponType == WEAPON_GENERAL)
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_CGWEAPON_FAILED ) );
			break;
		case CUltimateWeapon::ULTIMATE_CANCEL:
			Destroy();
			break;
		case CUltimateWeapon::ULTIMATE_INVENTORY:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_EXTRACTION_ERROR ) );
			break;			
	}

	if(m_pJItemElem[0] != NULL)
		m_pJItemElem[0]->SetExtra(0);
	if(m_pJItemElem[1] != NULL)
		m_pJItemElem[1]->SetExtra(0);
	if(m_pWItemElem != NULL)
		m_pWItemElem->SetExtra(0);
	
	m_pJItemElem[0] = NULL;
	m_pJItemElem[1] = NULL;
	m_pWItemElem = NULL;
	m_bIsSendChange = FALSE;
}

//////////////////////////////////////////////////////////////////////////
//	CWndRemoveJewelConfirm
//////////////////////////////////////////////////////////////////////////

CWndRemoveJewelConfirm::CWndRemoveJewelConfirm() 
{
	m_pInventory = NULL;
	m_pUpgradeItem = NULL;
}
 
CWndRemoveJewelConfirm::~CWndRemoveJewelConfirm() 
{
} 

void CWndRemoveJewelConfirm::OnDestroy()
{
	if(m_pInventory != NULL)
	{
		m_pInventory->m_pWndRemoveJewelConfirm = NULL;
		m_pInventory->m_bRemoveJewel = FALSE;
	}
}

void CWndRemoveJewelConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRemoveJewelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pText->m_string.AddParsingString( prj.GetText( TID_GAME_REMOVEJEWEL ) );
	pText->ResetString();
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveJewelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	m_pInventory = (CWndInventory*)pWndParent;
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRemoveJewelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveJewelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemoveJewelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemoveJewelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRemoveJewelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(m_pInventory != NULL)
		{
			// 인첸트가 되는 아이템 - 방어구 등등
			m_pInventory->m_pUpgradeItem = m_pUpgradeItem;
			m_pInventory->m_dwEnchantWaitTime = g_tmCurrent + ENCHANTWAITTIME;
		}
		Destroy();
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		if(m_pInventory != NULL)
			m_pInventory->BaseMouseCursor();
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveJewelConfirm::SetItem(FLItemBase*	m_pItem)
{
	m_pUpgradeItem = m_pItem;
}


//sun: 10차 전승시스템	Neuz, World, Trans

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeroSkillUp Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeroSkillUp::CWndHeroSkillUp() 
{
	for(int i=0; i<5; i++)
	{
		m_pItemElem[i] = NULL;
		m_JewelID[i] = -1;
		m_SlotID[i] = -1;
		m_PicJewel[i] = -1;
	}

	m_bSendHeroSkillup = FALSE;
}

CWndHeroSkillUp::~CWndHeroSkillUp() 
{ 
} 

void CWndHeroSkillUp::OnDestroy()
{
	for(int i=0; i<5; i++)
	{
		if(m_pItemElem[i] != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem[i] ) )
				m_pItemElem[i]->SetExtra(0);
		}
	}
}

void CWndHeroSkillUp::OnDraw( C2DRender* p2DRender )
{
	//Render Icon
	PT_ITEM_SPEC pItemProp;
	CTexture* pTexture;
	BOOL bCheckSlot = TRUE;
	
	CPoint point = GetMousePoint();
	int testnum = HitTest( point );
	if( testnum != -1)
	{
		if( CWndBase::m_GlobalShortcut.m_dwData )
		{
			CRect rect;
			LPWNDCTRL wndCtrl = GetWndCtrl( m_SlotID[testnum] );
			rect = wndCtrl->rect;
			p2DRender->RenderFillRect( rect, 0x60ffff00 );
		}
	}
	
	for(int i=0; i<5; i++)
	{
		//Default Jewel
		pItemProp = g_xSpecManager->GetSpecItem( m_JewelID[i] );
		LPWNDCTRL wndCtrl = GetWndCtrl( m_PicJewel[i] );
		if(pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
		
		//Slot Jewel
		if(m_pItemElem[i] != NULL)
		{
			pItemProp = m_pItemElem[i]->GetProp();
			wndCtrl = GetWndCtrl( m_SlotID[i] );
			if(pItemProp != NULL)
			{
				pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				if(pTexture != NULL)
					pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
			}
		}
		else
			bCheckSlot = FALSE;	
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(bCheckSlot);
}

void CWndHeroSkillUp::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(FALSE);

	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	SetDescription(NULL);

	m_SlotID[0] = WIDC_PIC_SLOT1;
	m_SlotID[1] = WIDC_PIC_SLOT2;
	m_SlotID[2] = WIDC_PIC_SLOT3;
	m_SlotID[3] = WIDC_PIC_SLOT4;
	m_SlotID[4] = WIDC_PIC_SLOT5;

	m_PicJewel[0] = WIDC_CUSTOM1;
	m_PicJewel[1] = WIDC_CUSTOM2;
	m_PicJewel[2] = WIDC_CUSTOM3;
	m_PicJewel[3] = WIDC_CUSTOM4;
	m_PicJewel[4] = WIDC_CUSTOM5;

	m_JewelID[0] = ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND );
	m_JewelID[1] = ITEM_INDEX( 2030, II_GEN_MAT_EMERALD );
	m_JewelID[2] = ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE );
	m_JewelID[3] = ITEM_INDEX( 2032, II_GEN_MAT_RUBY );
	m_JewelID[4] = ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ );
	
	//Window Position
/*	CWndInventory* pWndInventory;
	pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	CRect rectInventory;
	if(pWndInventory != NULL)
		rectInventory = pWndInventory->GetWindowRect( TRUE );

	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
*/
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHeroSkillUp::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HERO_SKILLUP, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndHeroSkillUp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHeroSkillUp::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndHeroSkillUp::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndHeroSkillUp::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndHeroSkillUp::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	int choicenum = HitTest( point );
	if(choicenum > -1 && m_pItemElem[choicenum] != NULL)
	{
		m_pItemElem[choicenum]->SetExtra(0);
		m_pItemElem[choicenum] = NULL;
	}
}

BOOL CWndHeroSkillUp::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	int nSelect = HitTest(point);
	if(nSelect > -1)
	{
		FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

		if(m_pItemElem[nSelect] == NULL)
		{
			if(pItemElem->GetProp()->dwID == m_JewelID[nSelect])
			{
				m_pItemElem[nSelect] = pItemElem;
				m_pItemElem[nSelect]->SetExtra(m_pItemElem[nSelect]->GetExtra()+1);
			}
		}
	}
	
	return TRUE;
}

BOOL CWndHeroSkillUp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK )
	{
		if(!m_bSendHeroSkillup)
		{
			//서버로 시작을 알린다.
			BOOL checkall = TRUE;
			OBJID itemobjId[5];
			for(int i=0; i<5; i++)
			{
				if(m_pItemElem[i] == NULL)
					checkall = FALSE;
				else
					itemobjId[i] = m_pItemElem[i]->m_dwObjId;				
			}

			if(checkall)
			{
				m_bSendHeroSkillup = TRUE;
				g_DPlay.SendLegendSkillUp(itemobjId, 5);
			}
		}
	}
	else if( nID == WIDC_CANCEL )
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

int CWndHeroSkillUp::HitTest( CPoint point )
{
	int rtn_val = -1;
	CRect rect;
	for(int i=0; i<5; i++)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl( m_SlotID[i] );		
		rect = wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			rtn_val = i;
			i = MAX_JEWEL;
		}
	}
	return rtn_val;
}

void CWndHeroSkillUp::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "HeroSkillUp.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndHeroSkillUp::SetJewel(FLItemElem* pItemElem)
{
	PT_ITEM_SPEC pProp = pItemElem->GetProp();
	int nSelect = -1;

	switch(pProp->dwID) 
	{
		case ITEM_INDEX( 2029, II_GEN_MAT_DIAMOND ):
			nSelect = 0;
			break;
		case ITEM_INDEX( 2030, II_GEN_MAT_EMERALD ):
			nSelect = 1;
			break;
		case ITEM_INDEX( 2031, II_GEN_MAT_SAPPHIRE ):
			nSelect = 2;
			break;
		case ITEM_INDEX( 2032, II_GEN_MAT_RUBY ):
			nSelect = 3;
			break;
		case ITEM_INDEX( 2033, II_GEN_MAT_TOPAZ ):
			nSelect = 4;
			break;
	}

	if(nSelect > -1)
	{
		if(m_pItemElem[nSelect] == NULL)
		{
			if(pItemElem->GetProp()->dwID == m_JewelID[nSelect])
			{
				m_pItemElem[nSelect] = pItemElem;
				m_pItemElem[nSelect]->SetExtra(m_pItemElem[nSelect]->GetExtra()+1);
			}
		}
	}
}

void CWndHeroSkillUp::ReceiveResult(int nresult)
{
	switch(nresult) 
	{
		case -1:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_MAX ) );
			break;
		case 0:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_FAIL ) );
			break;
		case 1:
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_HEROSKILLUP_SUCCESS ) );
			break;
	}

	Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeavenTower Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeavenTower::CWndHeavenTower() 
{
	m_nChoiceNum = -1;
	m_nListCount = 0;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( "123" );

	m_nGap = size.cy;

	if(m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndHeavenTower::~CWndHeavenTower() 
{ 
} 

void CWndHeavenTower::OnDestroy()
{
}

void CWndHeavenTower::OnDraw( C2DRender* p2DRender ) 
{
	if(m_nChoiceNum > -1)
	{
		CRect		rect( 0, 0, 0, 0 );
		LPWNDCTRL	lpWndCtrl( NULL );
		D3DXCOLOR	color( D3DCOLOR_ARGB( 60, 240, 0, 0 ) );

		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
		rect = lpWndCtrl->rect;
		
		rect.top += m_nChoiceNum * m_nGap + m_nListOffset;
		rect.bottom += m_nChoiceNum * m_nGap + m_nListOffset;
		//영어 때문에 창 크기 늘임
		rect.right += 80;
		p2DRender->RenderFillRect( rect, color );
	}
}

void CWndHeavenTower::InitText()
{
	m_nDescList.push_back(TID_TOOLTIP_HEAVENTOWER);

	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR01);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR02);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR03);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR04);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR05);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR06);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR07);

	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR01_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR02_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR03_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR04_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR05_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR06_MSG);

	m_nCost.push_back(10000);
	m_nCost.push_back(30000);
	m_nCost.push_back(50000);
	m_nCost.push_back(70000);
	m_nCost.push_back(100000);

	m_nListCount = m_nVecList.size();

	//Set List
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	for(int i=0; i<m_nListCount; i++)
	{
		CString strText;
		strText.Format("%d. %s", i+1, prj.GetText(m_nVecList[i]));
		pWndList->AddString(strText);
	}

	//Set Description
	m_nDescCount = m_nDescList.size();
	if( m_nDescCount == 0 )
		return;
	
	if(m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString( prj.GetText(m_nDescList[m_nCurrentPage]) );
	pText->ResetString();
}

void CWndHeavenTower::OnInitialUpdate()
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	InitText();

	if( m_nListCount == 0 )
	{	
		Destroy();
		return;
	}
	InitWnd();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHeavenTower::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HEAVEN_TOWER, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndHeavenTower::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID) 
	{
		case WIDC_BUTTON1:
			//Text Page Down
			if(m_nCurrentPage < m_nDescCount - 1)
			{
				m_nCurrentPage++;
				CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
				pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));
				
				if(m_nCurrentPage == m_nDescCount - 1)
				{
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
					pButton->SetVisible(FALSE);
					pButton->EnableWindow(FALSE);
				}
			}
			break;
		case WIDC_LISTBOX1:
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			int nCurSel = pWndListBox->GetCurSel();
			if( nCurSel != -1 )
			{
				m_nChoiceNum = nCurSel;

				if(m_nChoiceNum == 6)
				{
					Destroy();
					break;
				}

				CString strMsg;
				CWndHeavenTowerEntranceConfirm* pWndEntranceConfirm = new CWndHeavenTowerEntranceConfirm;
				if( pWndEntranceConfirm )
				{
					g_WndMng.OpenCustomBox( "", pWndEntranceConfirm, this );
					
					if(m_nChoiceNum != 5)
					{
						strMsg.Format( prj.GetText(m_nMsgList[m_nChoiceNum]), m_nCost[m_nChoiceNum] );
						pWndEntranceConfirm->SetValue( strMsg, m_nChoiceNum+1 );
					}
					else
					{
						strMsg.Format( prj.GetText(m_nMsgList[m_nChoiceNum]) );
						pWndEntranceConfirm->SetValue( strMsg, m_nChoiceNum+1 );
					}

				}
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndHeavenTower::InitWnd()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	//Size Control
	if(m_nListCount <= MAX_FLOOR_COUNT)
	{
		CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
		lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
		rect = lpWndCtrl->rect;

		int nListBoxBottom = 0;
		int nListBoxHeight = 0;
		int nButtonHeight = 0;
		//ListBox
		rect.bottom = rect.top + m_nListCount * m_nGap + 10;
		nListBoxBottom = rect.bottom;
		nListBoxHeight = rect.bottom - rect.top;
		//영어 때문에 창 크기 늘임
		rect.right += 80;

		pWndList->SetWndRect(rect);
		//Window
		rect = GetWindowRect(TRUE);
		lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
		CRect rectText = lpWndCtrl->rect;
		rect.bottom = rect.top + nListBoxHeight + (rectText.bottom - rectText.top) + 60;
		SetWndRect(rect);
	}

	//영어 때문에 창 크기 늘임
	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );		
	lpWndCtrl = GetWndCtrl( WIDC_BUTTON1 );
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	lpWndCtrl = GetWndCtrl( WIDC_TEXT1 );
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

//////////////////////////////////////////////////////////////////////////
// CWndHeavenTowerEntranceConfirm Class
//////////////////////////////////////////////////////////////////////////

BOOL CWndHeavenTowerEntranceConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	m_nFloor = 0;
	return CWndMessageBox::Initialize( "", pWndParent, MB_OKCANCEL );	
}

void CWndHeavenTowerEntranceConfirm::SetValue( CString str, DWORD nFloor )
{
	m_wndText.SetString( str );
	m_nFloor = nFloor;	
}

BOOL CWndHeavenTowerEntranceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			if(m_nFloor > 0 && m_nFloor != 6)
			{
				g_DPlay.SendTeleportToHeavenTower(m_nFloor);
				SAFE_DELETE(g_WndMng.m_pWndHeavenTower);
			}
			Destroy();
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//sun: 10, 속성제련 제거(10차로 변경)
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveAttribute Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemoveAttribute::CWndRemoveAttribute() 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pWndConfirm = NULL;
	m_pTexture = NULL;
}

CWndRemoveAttribute::~CWndRemoveAttribute() 
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}

	if(m_pWndConfirm != NULL)
		m_pWndConfirm->Destroy();

	SAFE_DELETE(m_pWndConfirm);
}

void CWndRemoveAttribute::OnDestroy()
{
}

void CWndRemoveAttribute::OnDraw( C2DRender* p2DRender ) 
{
	PT_ITEM_SPEC pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}
	} 
}

void CWndRemoveAttribute::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}
	
	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveAttribute::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_REMOVE_ATTRIBUTE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndRemoveAttribute::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveAttribute::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndRemoveAttribute::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndRemoveAttribute::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndRemoveAttribute::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;
	}
}

BOOL CWndRemoveAttribute::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		//속성제련이 된 무기만 올려놓을 수 있다.
		PT_ITEM_SPEC pItemProp;
		FLItemElem* pTempElem;
		pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if( pTempElem != NULL && pTempElem->m_nResistAbilityOption > 0 )
		{
			pItemProp = pTempElem->GetProp();
			if( FLItemElem::IsEleRefineryAble(pItemProp) )
			{
//				if( pItemProp->dwReferStat1 == WEAPON_GENERAL )
//				{
					if(m_pItemElem != NULL)
						m_pItemElem->SetExtra(0);

					m_pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					m_pEItemProp = m_pItemElem->GetProp();
					m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
					m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
					pButton->EnableWindow(TRUE);
//				}
			}
		}
		else
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_REMOVE_ERROR ) ); //Error Message
	}
	return TRUE;
}

void CWndRemoveAttribute::SetWeapon(FLItemElem* pItemElem)
{
	if( pItemElem != NULL && pItemElem->m_nResistAbilityOption > 0 )
	{
		PT_ITEM_SPEC pProp = pItemElem->GetProp();

//		if( pProp->dwItemKind1 == IK1_WEAPON )
		if( FLItemElem::IsEleRefineryAble(pProp) )
		{
//			if( pProp->dwReferStat1 == WEAPON_GENERAL )
//			{
				if(m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = pItemElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
//			}
		}
	}
	else
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_REMOVE_ERROR ) ); //Error Message
}

BOOL CWndRemoveAttribute::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		if(m_pWndConfirm == NULL)
		{
			m_pWndConfirm = new CWndRemoveAttributeConfirm;
			m_pWndConfirm->Initialize(this);
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveAttribute::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "RemoveAttribute.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndRemoveAttribute::ReceiveResult(BOOL result)
{
	if(result)
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_REMOVE_ATTRIBUTE_CONFIRM ) );

	Destroy();
}

//////////////////////////////////////////////////////////////////////////
//	CWndRemoveAttributeConfirm
//////////////////////////////////////////////////////////////////////////

CWndRemoveAttributeConfirm::CWndRemoveAttributeConfirm() 
{
}
 
CWndRemoveAttributeConfirm::~CWndRemoveAttributeConfirm() 
{
} 

void CWndRemoveAttributeConfirm::OnDestroy()
{
}

void CWndRemoveAttributeConfirm::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndRemoveAttributeConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pText->m_string.AddParsingString( prj.GetText( TID_GAME_REMOVE_ATTRIBUTE ) );
	pText->ResetString();	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveAttributeConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_REMOVE_ATTRIBUTE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRemoveAttributeConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveAttributeConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemoveAttributeConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemoveAttributeConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndRemoveAttributeConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	 CWndRemoveAttribute* pParentwnd = (CWndRemoveAttribute*)this->m_pParentWnd;

	if( nID == WIDC_YES )
	{
		//SEND to Server
		if(pParentwnd && pParentwnd->m_pItemElem)
			g_DPlay.SendRemoveAttribute(pParentwnd->m_pItemElem->m_dwObjId);
	}
	else if( nID == WIDC_NO || nID == WTBID_CLOSE )
	{
		pParentwnd->m_pWndConfirm = NULL;
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



//sun: 11, 피어싱 옵션 제거

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemovePiercing Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemovePiercing::CWndRemovePiercing(WndMode eWndMode) : m_eWndMode(eWndMode) 
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	m_pMaterial = NULL;
	m_pTextureMat = NULL;
#endif
}

CWndRemovePiercing::~CWndRemovePiercing() 
{ 
} 

void CWndRemovePiercing::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	if(m_pMaterial)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pMaterial ) )
			m_pMaterial->SetExtra(0);
	}
#endif
}

void CWndRemovePiercing::OnDraw( C2DRender* p2DRender ) 
{
	PT_ITEM_SPEC pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{
			int nPlusY = 0;
			if(m_eWndMode == WND_GENERAL )
				nPlusY = 20;
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top + nPlusY ) );
			
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
			size_t nMaxPiercing;
			CRect rect;
			LPWNDCTRL lpWndCtrl;

			// Suit - 4 Slot Use
			if(pItemProp->dwItemKind3 == IK3_SUIT)
			{
				nMaxPiercing = MAX_PIERCING_SUIT;
				D3DXCOLOR color =  D3DCOLOR_ARGB( 60, 240, 0, 0 );

				for(int i=MAX_PIERCING_SUIT; i<MAX_PIERCING; i++)
				{
					lpWndCtrl = GetWndCtrl( m_nInfoSlot[i] );
					rect = lpWndCtrl->rect;
					p2DRender->RenderFillRect( rect, color );
				}
			}
			else
				nMaxPiercing = 10;

			// Render Piercing Options
			CString textOpt;
			CString strTemp;

			size_t nPiercingSize = m_pItemElem->GetGeneralPiercingSize();

			for(size_t i=0; i<nPiercingSize; i++)
			{
				if(nPiercingSize > nMaxPiercing)
					break;

				PPIERCINGAVAIL ptr = NULL;
				ptr = CPiercingAvail::GetInstance()->GetPiercingAvail( m_pItemElem->GetGeneralPiercingItemID( i ) );

				if(ptr != NULL)
				{
					for(int j=0; j<ptr->nSize; j++)
					{
						int nDst = (int)ptr->anDstParam[j];
						int nAdj = (int)ptr->anAdjParam[j];
						
						if( g_WndMng.IsDstRate(nDst) )
						{
							if( nDst == DST_ATTACKSPEED )
								strTemp.Format( "%s%+d%%  ", prj.GetText(g_WndMng.GetDSTStringId( nDst )), nAdj / 2 / 10 );
							else
								strTemp.Format( "%s%+d%%  ", prj.GetText(g_WndMng.GetDSTStringId( nDst )), nAdj );
						}
						else
							strTemp.Format( "%s+%d  ", prj.GetText(g_WndMng.GetDSTStringId( nDst )), nAdj );

						textOpt += strTemp;
					}
					
					lpWndCtrl = GetWndCtrl( m_nInfoSlot[i] );
					
					if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
						p2DRender->TextOut( lpWndCtrl->rect.left + 10, lpWndCtrl->rect.top + 8, textOpt, m_dwColor );

					if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
						p2DRender->TextOut( lpWndCtrl->rect.left + 10, lpWndCtrl->rect.top + 6, textOpt, m_dwColor );

					textOpt = "";
					strTemp = "";
				}
			}
		}

	}
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	if (m_pMaterial != NULL)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT2 );
		if(m_pTextureMat != NULL)
				m_pTextureMat->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
#endif
}

void CWndRemovePiercing::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	if(m_eWndMode == WND_GENERAL )
	{
		CWndStatic* pPic1    = (CWndStatic*)GetDlgItem( WIDC_PIC_SLOT );
		CRect rt = pPic1->GetWndRect();
		rt.top += 20;
		rt.bottom += 20;
		pPic1->SetWndRect(rt, TRUE);
		
		CWndStatic* pPic2    = (CWndStatic*)GetDlgItem( WIDC_PIC_SLOT2 );
		pPic2->SetVisible(FALSE);
	}
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription(NULL);

	
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
	m_nInfoSlot[0] = WIDC_STATIC_PIERCING1;
	m_nInfoSlot[1] = WIDC_STATIC_PIERCING2;
	m_nInfoSlot[2] = WIDC_STATIC_PIERCING3;
	m_nInfoSlot[3] = WIDC_STATIC_PIERCING4;
	m_nInfoSlot[4] = WIDC_STATIC_PIERCING5;
	m_nInfoSlot[5] = WIDC_STATIC_PIERCING6;
	m_nInfoSlot[6] = WIDC_STATIC_PIERCING7;
	m_nInfoSlot[7] = WIDC_STATIC_PIERCING8;
	m_nInfoSlot[8] = WIDC_STATIC_PIERCING9;
	m_nInfoSlot[9] = WIDC_STATIC_PIERCING10;

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemovePiercing::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_PIERCING_EX, 0, CPoint( 0, 0 ), pWndParent );

} 
BOOL CWndRemovePiercing::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemovePiercing::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemovePiercing::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemovePiercing::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndRemovePiercing::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
			m_pTexture = NULL;
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
			m_pTextureMat = NULL;
			if(m_pMaterial)
			{
				m_pMaterial->SetExtra(0);
			}
#endif
		}
	}
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT2 );
	rect = wndCtrl->rect;

	if( rect.PtInRect( point ) )
	{
		if(m_pMaterial)
		{
			m_pMaterial->SetExtra(0);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_pTextureMat = NULL;
		}
	}
#endif
}

//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
void CWndRemovePiercing::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(TID_GAME_TOOLTIP_PIERCINGITEM), rect, point );
	}
}

BOOL CWndRemovePiercing::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		PT_ITEM_SPEC pItemProp;
		FLItemElem* pTempElem;
		pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
			if(m_eWndMode == WND_BARUNA )
			{
				if(!pItemProp->IsBarunaArmor())
					return FALSE;
			}
			else
			{
				if(pItemProp->IsBaruna())
					return FALSE;
			}
#endif
//sun: 12, 무기 피어싱
			if( pTempElem->IsPierceAble() && pTempElem->GetGeneralPiercingItemID( 0 ) != 0 && !pTempElem->IsEatPet() )
			{
				m_pItemElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
				
			}
			else
			{
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR_EX ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR_EX ) );
				return FALSE;
			}
		}
	}

#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	if(m_eWndMode == WND_BARUNA )
	{
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT2 );
		rect = wndCtrl->rect;
		if( rect.PtInRect( point ) )
		{		
			PT_ITEM_SPEC pItemProp;
			FLItemElem* pTempElem;
			pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
			
			if(m_pItemElem != NULL && pTempElem != NULL)
			{
				pItemProp = pTempElem->GetProp();
//sun: 12, 무기 피어싱
				if( pTempElem->GetProp()->dwItemKind3 == IK3_BARUNA_GEN_PIC_MAT_CANCEL )
				{
					m_pMaterial = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
					m_pTextureMat = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pMaterial->GetProp()->szIcon), 0xffff00ff );
					m_pMaterial->SetExtra(m_pItemElem->GetExtra());
					
				}
				else
				{
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
					g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR_EX ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR_EX ) );
					return FALSE;
				}
			}
		}
		if(m_pItemElem && m_pMaterial)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(TRUE);
		}
	}
	else
#endif	
	{
		if(m_pItemElem)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(TRUE);
		}
	}



	return TRUE;
}

//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
void CWndRemovePiercing::SetItem(FLItemElem* pItemElem)
{
	if(m_pItemElem == NULL && pItemElem != NULL)
	{
		PT_ITEM_SPEC pProp = pItemElem->GetProp();
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
		if(m_eWndMode == WND_BARUNA )
		{
			if(!pProp->IsBarunaArmor())
				return;
		}
		else
		{
			if(pProp->IsBaruna())
				return;
		}
#endif
//sun: 12, 무기 피어싱
		if( pItemElem->IsPierceAble() && pItemElem->GetGeneralPiercingItemID( 0 ) != 0 )
		{
			m_pItemElem = pItemElem;
			m_pEItemProp = m_pItemElem->GetProp();
			m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
			m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			
		}
		else
		{
//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
			g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR_EX ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR_EX ) );			
			return;
		}
	}
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	if(m_eWndMode == WND_BARUNA )
	{
		if(m_pItemElem != NULL && pItemElem != NULL)
		{
//sun: 12, 무기 피어싱
			if( pItemElem->GetProp()->dwItemKind3 == IK3_BARUNA_GEN_PIC_MAT_CANCEL )
			{
				m_pMaterial = pItemElem;
				m_pTextureMat = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pMaterial->GetProp()->szIcon), 0xffff00ff );
				m_pMaterial->SetExtra(m_pMaterial->GetExtra()+1);
				
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_PIERCING_ERROR_EX ), NULL, prj.GetTextColor( TID_GAME_REMOVE_PIERCING_ERROR_EX ) );
				return;
			}
		}

		if(m_pItemElem && m_pMaterial)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(TRUE);
		}
		
	}
	else
#endif
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}
}

BOOL CWndRemovePiercing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL && !m_pItemElem->IsEatPet() )
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
			if(m_eWndMode == WND_BARUNA )
			{
				g_DPlay.SendPiercingRemove(m_pItemElem->m_dwObjId, m_pMaterial->m_dwObjId);
			}
			else
#endif
			g_DPlay.SendPiercingRemove(m_pItemElem->m_dwObjId, 0);
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemovePiercing::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltRemovePiercing.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

//sun: 12, 피어싱 제거 창 개선 및 얼터멋 보석 제거 창 추가
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveJewel Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemoveJewel::CWndRemoveJewel(WndMode eWndMode) : m_eWndMode(eWndMode)
{
	m_pItemElem = NULL;
	m_pMaterial = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
	m_pMaterialTex = NULL;

	ResetJewel();
}

CWndRemoveJewel::~CWndRemoveJewel() 
{ 
} 

void CWndRemoveJewel::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}

	if(m_pMaterial != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pMaterial ) )
			m_pMaterial->SetExtra(0);
	}
}

void CWndRemoveJewel::OnDraw( C2DRender* p2DRender ) 
{
	
	if( m_eWndMode == WND_ULTIMATE || m_eWndMode == WND_BARUNA)
		DrawUltimate(p2DRender);
	else if ( m_eWndMode == WND_COSTUME)
		DrawCostume(p2DRender);
}

void CWndRemoveJewel::DrawUltimate( C2DRender* p2DRender )
{
	PT_ITEM_SPEC pItemProp;
	LPWNDCTRL wndCtrl;
	
	// Render Material Bg

	if(m_pMaterial != NULL)
	{
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
		m_pMaterialTex->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
	else
	{
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
		m_pMaterialTex->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), 50 );
	}


	// Render Item
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{	
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}

		// Render Jewel Opt
		for(int i=0; i<m_nJewelCount; i++)
		{
			if(m_nJewelCount > 5)
				break;
			
			//Jewel Render
			if(m_nJewelID[i] != 0)
			{
				wndCtrl = GetWndCtrl( m_nJewelSlot[i] );

				if(m_pJewelTex[i])
					m_pJewelTex[i]->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );

				//Text Render
				CString DstText;
				DWORD dwColorbuf;
				
				if(m_nJewelID[i] >= ITEM_INDEX( 2037, II_GEN_MAT_DIAMOND01 ) && m_nJewelID[i] <= ITEM_INDEX( 2041, II_GEN_MAT_TOPAZ01 ) ) //HP 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt1;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_HP_MAX) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2042, II_GEN_MAT_DIAMOND02 ) && m_nJewelID[i] <= ITEM_INDEX( 2046, II_GEN_MAT_TOPAZ02) ) //공격력 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt2;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ATKPOWER) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2047, II_GEN_MAT_DIAMOND03 ) && m_nJewelID[i] <= ITEM_INDEX( 2051, II_GEN_MAT_TOPAZ03 ) ) //방어력 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt3;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ADJDEF) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2052, II_GEN_MAT_DIAMOND04 ) && m_nJewelID[i] <= ITEM_INDEX( 2056, II_GEN_MAT_TOPAZ04 ) ) //흡혈
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt4;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_MELEE_STEALHP) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2057, II_GEN_MAT_DIAMOND05 ) && m_nJewelID[i] <= ITEM_INDEX( 2061, II_GEN_MAT_TOPAZ05 ) ) //PvP시 대미지 증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt5;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_PVP_DMG) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2062, II_GEN_MAT_DIAMOND06 ) && m_nJewelID[i] <= ITEM_INDEX( 2066, II_GEN_MAT_TOPAZ06 ) ) //힘증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt6;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STR) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX(2067, II_GEN_MAT_DIAMOND07 ) && m_nJewelID[i] <= ITEM_INDEX( 2071, II_GEN_MAT_TOPAZ07 ) ) //체력증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STA) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2072, II_GEN_MAT_DIAMOND08 ) && m_nJewelID[i] <= ITEM_INDEX( 2076, II_GEN_MAT_TOPAZ08 ) ) //민첩증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt8;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DEX) );
				}
				else if(m_nJewelID[i] >= ITEM_INDEX( 2077, II_GEN_MAT_DIAMOND09 ) && m_nJewelID[i] <= ITEM_INDEX( 2081, II_GEN_MAT_TOPAZ09 ) ) //지능증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_INT) );
				}

				wndCtrl = GetWndCtrl( m_nInfoSlot[i] );
				p2DRender->TextOut( wndCtrl->rect.left + 10, wndCtrl->rect.top + 10, DstText, dwColorbuf );					
			}
		}
	}
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
void CWndRemoveJewel::DrawCostume( C2DRender* p2DRender )
{
	PT_ITEM_SPEC pItemProp;
	LPWNDCTRL wndCtrl;
	
	// Render Material Bg
	if(m_pMaterial != NULL)
	{
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
		m_pMaterialTex->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
	}
	else
	{
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
		m_pMaterialTex->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ), 50 );
	}

	// Render Item
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
		if(pItemProp != NULL)
		{	
			if(m_pTexture != NULL)
				m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		}

		// Render Jewel Opt
		for(int i=0; i<m_nJewelCount; i++)
		{
			if(m_nJewelCount > 5)
				break;
			
			//Jewel Render
			if(m_nJewelID[i] != 0)
			{
				wndCtrl = GetWndCtrl( m_nJewelSlot[i] );

				if(m_pJewelTex[i])
					m_pJewelTex[i]->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );

				//Text Render
				CString DstText;
				DWORD dwColorbuf;
				
				if(m_nJewelID[i] == DST_ATKPOWER ) //공격력 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt2;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ATKPOWER) );
				}
				else if(m_nJewelID[i] == DST_ADJDEF ) //방어력 상승
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt3;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_ADJDEF) );
				}
				else if(m_nJewelID[i] == DST_MELEE_STEALHP ) //흡혈
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt4;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_MELEE_STEALHP) );
				}
				else if(m_nJewelID[i] == DST_PVP_DMG ) //PvP시 대미지 증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt5;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_PVP_DMG) );
				}
				else if(m_nJewelID[i] == DST_STR ) //힘증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt6;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STR) );
				}
				else if(m_nJewelID[i] == DST_STA ) //체력증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt7;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_STA) );
				}
				else if(m_nJewelID[i] == DST_DEX ) //민첩증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt8;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DEX) );
				}
				else if(m_nJewelID[i] == DST_INT ) //지능증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_INT) );
				}
				else if(m_nJewelID[i] == DST_ATTACKSPEED_RATE ) //공격속도증가
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_ATTACKSPEED_RATE) );
				}
				else if(m_nJewelID[i] == DST_SPELL_RATE ) //마법 시전 시간 감소율
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_TOOLTIP_DST_SPELL_RATE) );
				}
				else if(m_nJewelID[i] == DST_BLOCK_RANGE ) //원거리 블록율
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_GAME_TOOLTIP_BLOCKRANGE) );
				}
				else if(m_nJewelID[i] == DST_BLOCK_MELEE ) //근거리 블록율
				{
					dwColorbuf = g_WndMng.dwItemColor[g_Option.m_nToolTipText].dwAddedOpt9;
					DstText.Format( "%s", prj.GetText(TID_GAME_TOOLTIP_BLOCK) );
				}

				wndCtrl = GetWndCtrl( m_nInfoSlot[i] );
				p2DRender->TextOut( wndCtrl->rect.left + 10, wndCtrl->rect.top + 10, DstText, dwColorbuf );					
			}
		}
	}
}
#endif

void CWndRemoveJewel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	PT_ITEM_SPEC pItemProp;

	if( m_eWndMode == WND_ULTIMATE )
		pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) );
	else if ( m_eWndMode == WND_COSTUME)
		pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 25337, II_GEN_MAT_COSTUME_GEM_CANCEL ) );
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
	else if ( m_eWndMode == WND_BARUNA)
		pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 25584, II_GEN_MAT_BARUNA_CANCEL ) );
#endif
	if(pItemProp)
		m_pMaterialTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	m_nJewelSlot[0] = WIDC_JEWEL_SLOT1;
	m_nJewelSlot[1] = WIDC_JEWEL_SLOT2;
	m_nJewelSlot[2] = WIDC_JEWEL_SLOT3;
	m_nJewelSlot[3] = WIDC_JEWEL_SLOT4;
	m_nJewelSlot[4] = WIDC_JEWEL_SLOT5;

	m_nInfoSlot[0] = WIDC_JEWEL_SLOT_INFO1;
	m_nInfoSlot[1] = WIDC_JEWEL_SLOT_INFO2;
	m_nInfoSlot[2] = WIDC_JEWEL_SLOT_INFO3;
	m_nInfoSlot[3] = WIDC_JEWEL_SLOT_INFO4;
	m_nInfoSlot[4] = WIDC_JEWEL_SLOT_INFO5;

	SetDescription(NULL);

	switch(m_eWndMode)
	{
		case WND_ULTIMATE:
			SetTitle( prj.GetText( TID_MMI_SMELT_REMOVE_JEWEL ) );
			break;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
			SetTitle( prj.GetText( TID_MMI_SMELT_REMOVE_JEWEL_COSTUME ));
			break;
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		case WND_BARUNA:
			SetTitle( prj.GetText( TID_MMI_SMELT_REMOVE_JEWEL_BARUNA ));
			break;
#endif
	}

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndRemoveJewel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_JEWEL, 0, CPoint( 0, 0 ), pWndParent );
} 
BOOL CWndRemoveJewel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndRemoveJewel::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndRemoveJewel::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndRemoveJewel::OnLButtonDown( UINT nFlags, CPoint point ) 
{
}

void CWndRemoveJewel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
			m_pTexture = NULL;

			ResetJewel();
		}
	}

	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pMaterial)
		{
			m_pMaterial->SetExtra(0);
			m_pMaterial = NULL;
		}
	}

	if(m_pItemElem == NULL || m_pMaterial == NULL)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
	}
}

void CWndRemoveJewel::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;

	int nText = TID_GAME_TOOLTIP_REMOVEJEWEL1;

	if( m_eWndMode == WND_COSTUME ) 
		nText = TID_GAME_TOOLTIP_REMOVEJEWEL4;


	if( rect.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(nText), rect, point );	// 얼터멋 웨폰 보석 옵션 제거를 원하는 아이템을 올려놓으세요.
	}

	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(nText+1), rect, point );	// 보석옵션 제거를 위해서 문스톤을 올려놓는 공간입니다.
	}

	wndCtrl = GetWndCtrl( WIDC_JEWEL_SLOT1 );
	rect.left = wndCtrl->rect.left;
	rect.top = wndCtrl->rect.top;
	wndCtrl = GetWndCtrl( WIDC_JEWEL_SLOT_INFO5 );
	rect.right = wndCtrl->rect.right;
	rect.bottom = wndCtrl->rect.bottom;
	if( rect.PtInRect( point ) )
	{		
		ClientToScreen( &point );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( (DWORD)this, prj.GetText(nText+2), rect, point );	// 해당 얼터멋 웨폰에 적용되어 있는 옵션의 순서이며, 아래쪽에 있을수록 최근에 적용시킨 옵션 입니다.
	}
}

BOOL CWndRemoveJewel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect;
	PT_ITEM_SPEC pItemProp;
	FLItemElem* pTempElem;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_PIC_SLOT );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
			if ( m_eWndMode == WND_BARUNA )
			{
				if ( !pItemProp->IsBaruna() )
					return FALSE;
			}
			else
			{
				if ( pItemProp->IsBaruna() )
					return FALSE;
			}
#endif
			
			if(pTempElem && (((pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) &&
				(pItemProp->dwReferStat1 == WEAPON_ULTIMATE
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
				|| pItemProp->IsBarunaWeapon()
#endif
				)) 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				|| pItemProp->IsCostumeEnhanceParts()
#endif					
				)&& pTempElem->GetUltimatePiercingSize() > 0)
			{
				m_pItemElem = pTempElem;
				m_pEItemProp = pItemProp;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);

				m_nJewelCount = pTempElem->GetUltimatePiercingSize();
				int nJewelID = 0;
				for(int i=0; i<m_nJewelCount; i++)
				{
					m_nJewelID[i] = m_pItemElem->GetUltimatePiercingItemID(i);
					nJewelID = m_nJewelID[i];
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
					if( m_eWndMode == WND_COSTUME ) 
						nJewelID =ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + m_pItemElem->GetProp()->dwParts - PARTS_HAT;
#endif
					pItemProp = g_xSpecManager->GetSpecItem( nJewelID );
					if(pItemProp != NULL)
						m_pJewelTex[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				}
			}
			else
			{
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
				if( m_eWndMode == WND_BARUNA ) 
				{
				}
				else
#endif
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				if( m_eWndMode == WND_COSTUME ) 
				{
				}
				else
#endif
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR2 ) );
			}
		}
	}

	wndCtrl = GetWndCtrl( WIDC_PIC_SLOT1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pMaterial == NULL && pTempElem != NULL)
		{
			PT_ITEM_SPEC pItemProp = pTempElem->GetProp();

						bool bSetMaterial = false;
			
			switch(m_eWndMode)
			{
			case WND_ULTIMATE:
				if(pItemProp->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemProp->dwID ==ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ))
					bSetMaterial = true;
				break;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM	
			case WND_COSTUME:
				if( pItemProp->dwID ==ITEM_INDEX( 25337, II_GEN_MAT_COSTUME_GEM_CANCEL ) )
					bSetMaterial = true;
				break;
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
			case WND_BARUNA:
				if( pItemProp->dwID ==ITEM_INDEX( 25584, II_GEN_MAT_BARUNA_CANCEL ) )
					bSetMaterial = true;
				break;
#endif
			}

			if( bSetMaterial )
			{
				m_pMaterial = pTempElem;
				m_pMaterial->SetExtra(m_pMaterial->GetExtra()+1);
			}
			else
			{
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				if( m_eWndMode == WND_COSTUME || m_eWndMode == WND_BARUNA) 
				{
				}
				else
#endif
					g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR1 ) );
			}
		}
	}

	if(m_pItemElem && m_pMaterial)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}

	return TRUE;
}

void CWndRemoveJewel::SetItem(FLItemElem* pItemElem)
{
	if(pItemElem != NULL)
	{
		PT_ITEM_SPEC pItemProp = pItemElem->GetProp();

		if(m_pMaterial == NULL && pItemProp->dwItemKind2 == IK2_MATERIAL)
		{
			bool bSetMaterial = false;
			
			switch(m_eWndMode)
			{
			case WND_ULTIMATE:
				if(pItemProp->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemProp->dwID ==ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ) )
					bSetMaterial = true;
				break;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM	
			case WND_COSTUME:
				if( pItemProp->dwID ==ITEM_INDEX( 25337, II_GEN_MAT_COSTUME_GEM_CANCEL ) )
					bSetMaterial = true;
				break;
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
			case WND_BARUNA:
				if( pItemProp->dwID ==ITEM_INDEX( 25584, II_GEN_MAT_BARUNA_CANCEL ) )
					bSetMaterial = true;
				break;
#endif
			}


			if( bSetMaterial )
			{
				m_pMaterial = pItemElem;
				m_pMaterial->SetExtra(m_pMaterial->GetExtra()+1);
			}
			else
			{
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				if( m_eWndMode == WND_COSTUME || m_eWndMode == WND_BARUNA) 
				{
				}
				else
#endif
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR1 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR1 ) );
			}
		}
		else if(m_pItemElem == NULL)
		{
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
			if ( m_eWndMode == WND_BARUNA )
			{
				if ( !pItemProp->IsBaruna() )
					return;
			}
			else
			{
				if ( pItemProp->IsBaruna() )
					return;
			}
#endif
			if(pItemElem && (((pItemProp->dwItemKind2 == IK2_WEAPON_DIRECT || pItemProp->dwItemKind2 == IK2_WEAPON_MAGIC) &&
				(pItemProp->dwReferStat1 == WEAPON_ULTIMATE
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
				|| pItemProp->IsBarunaWeapon()
#endif
				)) 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				|| pItemProp->IsCostumeEnhanceParts()
#endif					
				) && pItemElem->GetUltimatePiercingSize() > 0)
			{
				m_pItemElem = pItemElem;
				m_pEItemProp = pItemProp;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);

				m_nJewelCount = pItemElem->GetUltimatePiercingSize();
				int nJewelID = 0;
				for(int i=0; i<m_nJewelCount; i++)
				{
					m_nJewelID[i] = m_pItemElem->GetUltimatePiercingItemID(i);
					nJewelID = m_nJewelID[i];
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
					if( m_eWndMode == WND_COSTUME ) 
						nJewelID =ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + m_pItemElem->GetProp()->dwParts - PARTS_HAT;
#endif

					pItemProp = g_xSpecManager->GetSpecItem( nJewelID );
					if(pItemProp != NULL)
						m_pJewelTex[i] = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				}
			}
			else
			{
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				if( m_eWndMode == WND_COSTUME ) 
				{
				}
				else
#endif
				g_WndMng.PutString( prj.GetText( TID_GAME_REMOVE_JEWEL_ERROR2 ), NULL, prj.GetTextColor( TID_GAME_REMOVE_JEWEL_ERROR2 ) );
			}
		}
	}
	
	if(m_pItemElem && m_pMaterial)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
	}
}

BOOL CWndRemoveJewel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_START )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL && m_pMaterial != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);
			if( m_eWndMode == WND_ULTIMATE || m_eWndMode == WND_BARUNA) 
				g_DPlay.SendUltimateRemoveGem( m_pItemElem->m_dwObjId, m_pMaterial->m_dwObjId );
			else
				SendPacket();


			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndRemoveJewel::SetDescription( CHAR* szChar )
{
	CScript scanner;
	BOOL checkflag;

	if( m_eWndMode == WND_ULTIMATE ) 
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltRemoveJewel.inc" ) ));
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	else if( m_eWndMode == WND_COSTUME ) 
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltRemoveJewelCostume.inc" ) ));
#endif
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
	else if( m_eWndMode == WND_BARUNA ) 
		checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "SmeltRemoveJewelBaruna.inc" ) ));
#endif
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

void CWndRemoveJewel::ResetJewel()
{
	m_nJewelCount = 0;

	for(int i=0; i<5; i++)
	{
		m_nJewelID[i] = 0;
		m_pJewelTex[i] = NULL;
	}
}

void CWndRemoveJewel::SendPacket()
{
	FLPacketItemUpgradeRemoveGemReq kPacket;
	kPacket.dwMainItemObjID = m_pItemElem->m_dwObjId;
	kPacket.dwMaterialItemObjID = m_pMaterial->m_dwObjId;
	g_DPlay.SendPacket( &kPacket );
}


//sun:13, 제련 확장(속성, 일반)

//////////////////////////////////////////////////////////////////////////
// Change Attribute Window
//////////////////////////////////////////////////////////////////////////

CWndChangeAttribute::CWndChangeAttribute()
{
	m_pText = NULL;
	m_nAttributeNum = -1;
	m_pItemElem = NULL;
	m_pChangeItem = NULL;
	m_pTexture = NULL;
}

CWndChangeAttribute::~CWndChangeAttribute()
{
	if( m_pItemElem )
		m_pItemElem->SetExtra( 0 );
	if( m_pChangeItem )
		m_pChangeItem->SetExtra( 0 );
}

void CWndChangeAttribute::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "ChangeAttribute.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_nAttributeStaticID[0] = WIDC_CUSTOM1;
	m_nAttributeStaticID[1] = WIDC_CUSTOM2;
	m_nAttributeStaticID[2] = WIDC_CUSTOM3;
	m_nAttributeStaticID[3] = WIDC_CUSTOM4;
	m_nAttributeStaticID[4] = WIDC_CUSTOM5;

	m_nTooltipTextIndx[0] = TID_TOOLTIP_CA_INSLOT;
	m_nTooltipTextIndx[1] = TID_TOOLTIP_CA_FIRE;
	m_nTooltipTextIndx[2] = TID_TOOLTIP_CA_WATER;
	m_nTooltipTextIndx[3] = TID_TOOLTIP_CA_ELEC;
	m_nTooltipTextIndx[4] = TID_TOOLTIP_CA_WIND;
	m_nTooltipTextIndx[5] = TID_TOOLTIP_CA_EARTH;

	MoveParentCenter();
} 

BOOL CWndChangeAttribute::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CHANGE_ATTRIBUTE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndChangeAttribute::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(nID == WIDC_OK)
	{
		if(m_pItemElem != NULL && m_pChangeItem != NULL && m_nAttributeNum > -1 && m_nAttributeNum < 5)
			g_DPlay.SendChangeAttribute(m_pItemElem->m_dwObjId, m_pChangeItem->m_dwObjId, m_nAttributeNum + 1);

		if(m_pItemElem)
			m_pItemElem->SetExtra(0);

		Destroy();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndChangeAttribute::OnDraw(C2DRender* p2DRender)
{
	// Draw Item
	if(m_pItemElem != NULL)
	{
		if(m_pTexture != NULL)
			m_pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC1 )->rect.left, GetWndCtrl( WIDC_STATIC1 )->rect.top ) );
	}

	// Draw Attribute & Draw Rect Round Lamp
	for(int i=0; i<5; i++)
	{
		CRect rect = GetWndCtrl( m_nAttributeStaticID[i] )->rect;
		g_WndMng.m_pWndWorld->m_texAttrIcon.Render( p2DRender, CPoint(rect.left, rect.top), i, 255, 1.5f, 1.5f );
		
		if(i == m_nAttributeNum)
			FillRect( p2DRender, GetWndCtrl( m_nAttributeStaticID[i] )->rect, 0xffbb00ff, 0x00bb00ff );
	}
}

BOOL CWndChangeAttribute::Process()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	CPoint ptMouse = GetMousePoint();

	lpWndCtrl = GetWndCtrl(WIDC_STATIC1);
	rect = lpWndCtrl->rect;
	if(rect.PtInRect(ptMouse))
	{
		ClientToScreen( &ptMouse );
		ClientToScreen( &rect );
		g_toolTip.PutToolTip( 10000, prj.GetText(m_nTooltipTextIndx[0]), rect, ptMouse, 1 );
	}

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if(rect.PtInRect(ptMouse))
		{
			ClientToScreen( &ptMouse );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 10000, prj.GetText(m_nTooltipTextIndx[i+1]), rect, ptMouse, 1 );
		}
	}
	
	return TRUE;
}

void CWndChangeAttribute::OnLButtonUp( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if(rect.PtInRect(point))
			m_nAttributeNum = i;
	}
}

BOOL CWndChangeAttribute::OnSetCursor( CWndBase* pWndBase, UINT nHitTest, UINT message )
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for(int i=0; i<5; i++) 
	{
		lpWndCtrl = GetWndCtrl( m_nAttributeStaticID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) )
			bOnTitle = TRUE;
	}

	if(bOnTitle)
		SetMouseCursor( CUR_SELECT );
	else
	{
		SetMouseCursor( CUR_BASE );
		CWndBase::OnSetCursor( pWndBase, nHitTest, message );
	}

	return TRUE;
}

void CWndChangeAttribute::SetChangeItem( FLItemElem* pItemElem )
{
	pItemElem->SetExtra( pItemElem->GetExtra() + 1 );
	m_pChangeItem = pItemElem;
}

void CWndChangeAttribute::FillRect(C2DRender *p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	CPoint Point = CPoint( rectBg.TopLeft().x, rectBg.TopLeft().y );
	float fVar = 0.04f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	float fLerp = 1.0f;

	D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp );
	int nthick = 8;	
	CRect Rect = CRect( 0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.top-2, Point.x+Rect.left+nthick-2, Point.y+Rect.bottom+2 ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.top+nthick-2, Point.x+Rect.right+2, Point.y+Rect.top-2 ),
		dwColor1, dwColor1, dwColor2, dwColor2 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.right+2, Point.y+Rect.top-2, Point.x+Rect.right-nthick+2, Point.y+Rect.bottom+2 ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	p2DRender->RenderFillRect( CRect( Point.x+Rect.left-2, Point.y+Rect.bottom+2, Point.x+Rect.right+2, Point.y+Rect.bottom-nthick+2 ),
		dwColor2, dwColor2, dwColor1, dwColor1 );
}

BOOL CWndChangeAttribute::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame == NULL )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		SetForbid( TRUE );
		return FALSE;
	}

	CRect rect;
	PT_ITEM_SPEC pItemProp;
	FLItemElem* pTempElem;

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{		
		pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		
		if(m_pItemElem == NULL && pTempElem != NULL)
		{
			pItemProp = pTempElem->GetProp();
			
			if(pTempElem && FLItemElem::IsEleRefineryAble(pItemProp) && pTempElem->m_nResistAbilityOption > 0)
			{
				m_pItemElem = pTempElem;
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
				m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			}
			else
				g_WndMng.PutString( prj.GetText( TID_GAME_NOTELEMENT  ), NULL, prj.GetTextColor( TID_GAME_NOTELEMENT  ) );
		}
	}

	return TRUE;
}

void CWndChangeAttribute::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		if(m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_pTexture = NULL;
		}
	}
}


//sun: 13, 커플 시스템

//////////////////////////////////////////////////////////////////////////
// Couple Message Window
//////////////////////////////////////////////////////////////////////////
BOOL CWndCoupleMessage::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( m_strText, pWndParent, MB_OKCANCEL );
}

BOOL CWndCoupleMessage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( m_nMode )
	{
		case CM_SENDPROPOSE:
			{
				if(nID == IDOK)
				{
					if(m_pTargetObj)
						g_DPlay.SendPropose(((CMover*)m_pTargetObj)->GetName());
					Destroy();
				}
				else if(nID == IDCANCEL)
					Destroy();					
			}
			break;
		case CM_RECEIVEPROPOSE:
			{
				if(nID == IDOK)
				{
					g_DPlay.SendCouple();
					Destroy();
				}
				else if(nID == IDCANCEL)
				{
					g_DPlay.SendRefuse();
					Destroy();
				}
			}
			break;
		case CM_CANCELCOUPLE:
			{
				if(nID == IDOK)
				{
					g_DPlay.SendDecouple();
					Destroy();
				}
				else if(nID == IDCANCEL)
					Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleMessage::SetMessageMod(CString strText, int nMode, CObj* pTargetObj)
{
	m_strText = strText;
	m_nMode = nMode;
	m_pTargetObj = pTargetObj;
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Tab Information Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleTabInfo::CWndCoupleTabInfo()
:	m_pVBGauge( NULL )
,	m_pCouple( NULL )
,	m_pGaugeExp( NULL )
,	m_pGaugeBg( NULL )
{
}

CWndCoupleTabInfo::~CWndCoupleTabInfo()
{
}

HRESULT CWndCoupleTabInfo::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndCoupleTabInfo::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}
HRESULT CWndCoupleTabInfo::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

void CWndCoupleTabInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_texGauEmptyNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("GauEmptyNormal.bmp") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_texGauFillNormal.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T("GauFillNormal.bmp") ), WNDCOLOR_DEFAULT_KEY, TRUE );

		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		if(pWndButton)
		{
			pWndButton->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T("ButtBreakUp.bmp") ), WNDCOLOR_DEFAULT_KEY );
		}
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_pGaugeExp	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T("BarSky.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_pGaugeBg	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), _T("BarBg.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
	}
	
	MoveParentCenter();
} 

BOOL CWndCoupleTabInfo::Initialize( CWndBase* pWndParent, DWORD  ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COUPLE_TAB_INFO, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndCoupleTabInfo::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_BUTTON1)
	{
		if(m_pCouple)
		{
			// 프러포즈 해제 메세지 창 출력
			if(g_WndMng.m_pWndCoupleMessage)
				SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);

			g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
			if(g_WndMng.m_pWndCoupleMessage)
			{
				u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
				PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

				if(pData)
				{
					CString strText;
					strText.Format(prj.GetText(TID_GAME_COUPLECANCEL), pData->szPlayer);
					g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_CANCELCOUPLE);
					g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleTabInfo::OnDraw(C2DRender* p2DRender)
{
	TEXTUREVERTEX2* pVertex = new TEXTUREVERTEX2[6];
	TEXTUREVERTEX2* pVertices = pVertex;

	//Couple Info & Exp Gauge Draw
	CRect rect, rectTemp;
	LPWNDCTRL lpStatic = GetWndCtrl( WIDC_STATIC4 );
	rect.TopLeft().y = lpStatic->rect.top + 36;
	rect.TopLeft().x = lpStatic->rect.left + 70;
	rect.BottomRight().y = lpStatic->rect.top + 48;
	rect.BottomRight().x = lpStatic->rect.right - 10;

	m_pCouple = CCoupleHelper::Instance()->GetCouple();
	if(m_pCouple)
	{
		u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
		PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

		if(pData)
		{
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			{
				CString strTemp( _T("") );
				CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
				pWndStatic->m_dwColor = 0xff3f3f3f;
				pWndStatic->SetTitle(pData->szPlayer);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
				pWndStatic->m_dwColor = 0xff3f3f3f;
				pWndStatic->SetTitle(prj.m_aJob[ pData->data.nJob ].szName);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
				pWndStatic->m_dwColor = 0xff3f3f3f;
				strTemp.Format("%d", pData->data.nLevel);
				pWndStatic->SetTitle(strTemp);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
				pWndStatic->m_dwColor = 0xff3f3f3f;
				strTemp.Format("%d", m_pCouple->GetLevel());
				pWndStatic->SetTitle(strTemp);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
				pWndStatic->m_dwColor = 0xff3f3f3f;
				strTemp.Format("%d", m_pCouple->GetLevel());
				pWndStatic->SetTitle(strTemp);
			}

			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				CString strTemp( _T("") );
				CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
				pWndStatic->SetTitle(pData->szPlayer);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
				pWndStatic->SetTitle(prj.m_aJob[ pData->data.nJob ].szName);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
				strTemp.Format("%d", pData->data.nLevel);
				pWndStatic->SetTitle(strTemp);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
				strTemp.Format("%d", m_pCouple->GetLevel());
				pWndStatic->SetTitle(strTemp);
				pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
				strTemp.Format("%d", m_pCouple->GetLevel());
				pWndStatic->SetTitle(strTemp);
			}

			// Draw Master/Hero Icon
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

			if(pWndWorld)
			{
				LPWNDCTRL lpCtrl = GetWndCtrl(WIDC_LEVEL);
				CPoint ptJobType;
				ptJobType.x = lpCtrl->rect.right - 60;
				ptJobType.y = lpCtrl->rect.top - 1;

				if( prj.m_aJob[ pData->data.nJob ].dwJobType == JTYPE_MASTER )
				{
					int nMasterIndex = 27;
					if(/*m_nLevel >= 60 && */pData->data.nLevel < 70) //Level Down될 경우를 생각해서 주석처리.
						nMasterIndex = 27;
					else if(pData->data.nLevel >= 70 && pData->data.nLevel < 80)
						nMasterIndex = 28;
					else if(pData->data.nLevel >= 80 && pData->data.nLevel < 90)
						nMasterIndex = 29;
					else if(pData->data.nLevel >= 90 && pData->data.nLevel < 100)
						nMasterIndex = 30;
					else if(pData->data.nLevel >= 100 && pData->data.nLevel < 110)
						nMasterIndex = 31;
					else if(pData->data.nLevel >= 110 && pData->data.nLevel <= 120)
						nMasterIndex = 32;

					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, ptJobType,  nMasterIndex, &pVertices, WNDCOLOR_WHITE );
				}
				else if( prj.m_aJob[ pData->data.nJob ].dwJobType == JTYPE_HERO )
					pWndWorld->m_texPlayerDataIcon.MakeVertex( p2DRender, ptJobType,  33, &pVertices, WNDCOLOR_WHITE );

				pWndWorld->m_texPlayerDataIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
			}
		}

		float fRate = CCoupleProperty::Instance()->GetExperienceRate( m_pCouple->GetLevel(), m_pCouple->GetExperience() );

		//처음에 게이지 안차서 보정
		if(fRate > 0.00f && fRate < 0.04f)
			fRate = 0.05f;
		else if(fRate >= 0.04f && fRate <= 0.075f)
			fRate = 0.075f;

		int nWidth	= (int)( rect.Width() * fRate );
		rectTemp = rect; 
		rectTemp.right = rectTemp.left + nWidth;
		if( rect.right < rectTemp.right )
			rectTemp.right = rect.right;
	}
	else
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
	}

	if(m_pVBGauge)
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_pTheme->RenderGauge( p2DRender, &rect, WNDCOLOR_WHITE, m_pVBGauge, &m_texGauEmptyNormal );
			m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
		}
	
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_pTheme->RenderGauge( p2DRender, &rect,	WNDCOLOR_WHITE, m_pVBGauge, m_pGaugeBg );
			m_pTheme->RenderGauge( p2DRender, &rectTemp, WNDCOLOR_WHITE, m_pVBGauge, m_pGaugeExp );
		}
	}

	SAFE_DELETE_ARRAY( pVertex );
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Tab Skill Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleTabSkill::CWndCoupleTabSkill()
{
	m_pText = NULL;
	m_pSkillBgTexture = NULL;
}

CWndCoupleTabSkill::~CWndCoupleTabSkill()
{
}

void CWndCoupleTabSkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_pText = (CWndText *)GetDlgItem( WIDC_TEXT1 );
	
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "CoupleSkillInfo.inc" ) ));
	CHAR* szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();	
	}

	m_pSkillBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Bg_Couple_Skill.tga"), 0xffff00ff );

	MoveParentCenter();
} 

BOOL CWndCoupleTabSkill::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COUPLE_TAB_SKILL, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndCoupleTabSkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndCoupleTabSkill::OnDraw(C2DRender* p2DRender)
{
	CPoint point;

	LPWNDCTRL lpWndStatic = GetWndCtrl(WIDC_STATIC1);
	
	if(lpWndStatic)
	{
		point.x = lpWndStatic->rect.left + 4;
		point.y = lpWndStatic->rect.top + 2;
	}

	// Draw Background Image
	if(m_pSkillBgTexture)
		p2DRender->RenderTexture( point, m_pSkillBgTexture );

	// Draw Active SKill
	int nLevel = 0;
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple();
	if(pCouple)
		nLevel = pCouple->GetLevel();

	VS& vSkills = CCoupleProperty::Instance()->GetSkill( nLevel );
	VSK& vSkillKinds = CCoupleProperty::Instance()->GetSKillKinds();

	if(vSkills.size() == vSkillKinds.size())
	{
		for(int i=0; i<(int)( vSkills.size() ); i++)
		{
			if(vSkills[i] > 0)
			{
				PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( vSkillKinds.at( i ) );
				if(pItemProp)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pItemProp->szIcon), 0xffff00ff );

					if(pTex)
						p2DRender->RenderTexture( point, pTex );
				}
			}
			point.x += 32;
		}
	}

	// Make Tooltip
	CPoint ptMouse = GetMousePoint();
	CRect rect = lpWndStatic->rect;

	rect.left += 4;
	rect.top += 2;
	rect.right = rect.left + 32;
	rect.bottom = rect.top + 32;

	for(int i=0; i<(int)( vSkillKinds.size() ); i++)
	{
		if(rect.PtInRect(ptMouse))
		{
			// Tooltip
			CString strSkillName;
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( vSkillKinds.at( i ) );

			if(pItemProp)
				strSkillName.Format("%s", pItemProp->szName);

			ClientToScreen( &ptMouse );
			ClientToScreen( &rect );
			g_toolTip.PutToolTip( 10000, strSkillName, rect, ptMouse, 1 );
		}

		rect.left += 32;
		rect.right += 32;
	}
}

//////////////////////////////////////////////////////////////////////////
// Couple Manager Window
//////////////////////////////////////////////////////////////////////////
CWndCoupleManager::CWndCoupleManager()
{
}

CWndCoupleManager::~CWndCoupleManager()
{
}

void CWndCoupleManager::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if(pWndTabCtrl)
	{
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndTabCtrl->SetTabTitleAlign( CWndTabCtrl::ALIGN_CENTER );
	}
	
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	
	m_wndCoupleTabInfo.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_INFO );
	m_wndCoupleTabSkill.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_SKILL );
	
	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_INFO);
	tabTabItem.pWndBase = &m_wndCoupleTabInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_SKILL);
	tabTabItem.pWndBase = &m_wndCoupleTabSkill;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	MoveParentCenter();
} 

BOOL CWndCoupleManager::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_COUPLE_MAIN, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndCoupleManager::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


CWndFunnyCoinConfirm::CWndFunnyCoinConfirm() 
{
	m_dwItemId = 0;
	m_pItemElem = NULL;
}
 
CWndFunnyCoinConfirm::~CWndFunnyCoinConfirm() 
{ 
} 

void CWndFunnyCoinConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	if(m_pItemElem != NULL)
	{
		CWndText* pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		if(pText)
		{
			CString strText;
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_pItemElem->m_dwItemId );
			if(pItemProp)
				strText.Format(prj.GetText( TID_GAME_FUNNYCOIN_ASKUSE ), pItemProp->szName);

			pText->AddString(strText);
		}
	}
	else
		Destroy();

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndFunnyCoinConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FUNNYCOIN_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndFunnyCoinConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if(static_cast<int>(m_dwItemId) > -1)
			g_DPlay.SendDoUseItem(MAKELONG( ITYPE_ITEM, m_dwItemId ), NULL_ID, -1, FALSE);
	}

	Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndFunnyCoinConfirm::SetInfo(DWORD dwItemId, FLItemElem* pItemElem)
{
	m_dwItemId = dwItemId;
	m_pItemElem = pItemElem;
}



CWndSmeltSafety::CWndSmeltSafety(CWndSmeltSafety::WndMode eWndMode, bool bBaruna) : 
m_eWndMode(eWndMode), 
m_bBaruna(bBaruna),
m_pItemElem(NULL), 
m_pItemTexture(NULL), 
m_pNowGaugeTexture(NULL), 
m_pSuccessTexture(NULL), 
m_pFailureTexture(NULL), 
m_nMaterialCount(0), 
m_nScroll1Count(0), 
m_nScroll2Count(0), 
m_nResultCount(0), 
m_bStart(FALSE), 
m_bResultSwitch(false), 
m_dwEnchantTimeStart(0xffffffff), 
m_dwEnchantTimeEnd(0xffffffff), 
m_fGaugeRate(0), 
m_nValidSmeltCounter(0), 
m_nCurrentSmeltNumber(0), 
m_pVertexBufferGauge(NULL), 
m_pVertexBufferSuccessImage(NULL), 
m_pVertexBufferFailureImage(NULL)
, 
m_pSelectedElementalCardItemProp( NULL )
{
}

CWndSmeltSafety::~CWndSmeltSafety()
{
	for(int i = 0; i < SMELT_MAX; ++i)
	{
		if(m_Material[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Material[i].pItemElem ) )
				m_Material[i].pItemElem->SetExtra(0);
		}

		if(m_Scroll1[i].pItemElem != NULL)
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Scroll1[i].pItemElem ) )
				m_Scroll1[i].pItemElem->SetExtra(0);
		}

		if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_Scroll2[i].pItemElem != NULL )
		{
			if( !g_pPlayer->m_vtInfo.IsTrading( m_Scroll2[i].pItemElem ) )
				m_Scroll2[i].pItemElem->SetExtra(0);
		}
	}

	if(m_pItemElem != NULL)
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
		pWndInventory->CISetDieFlag_ItemCtrl( FALSE );
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSmeltSafety::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_SAFETY, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndSmeltSafety::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	// 여기에 코딩하세요

	int StaticMaterialID[SMELT_MAX] = {WIDC_STATIC11, WIDC_STATIC12, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16,
								WIDC_STATIC17, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20};
	int StaticScrollID1[SMELT_MAX] = {WIDC_STATIC41, WIDC_STATIC42, WIDC_STATIC43, WIDC_STATIC44, WIDC_STATIC45, WIDC_STATIC46,
								WIDC_STATIC47, WIDC_STATIC48, WIDC_STATIC49, WIDC_STATIC50};
	int StaticScrollID2[SMELT_MAX] = {WIDC_STATIC61, WIDC_STATIC62, WIDC_STATIC63, WIDC_STATIC64, WIDC_STATIC65, WIDC_STATIC66,
								WIDC_STATIC67, WIDC_STATIC68, WIDC_STATIC69, WIDC_STATIC70};

	for(int i = 0; i < SMELT_MAX; ++i)
	{
		m_Material[i].wndCtrl = GetWndCtrl( StaticMaterialID[i] );
		m_Material[i].staticNum = StaticMaterialID[i];
		m_Material[i].isUse = FALSE;
		m_Material[i].pItemElem = NULL;
		m_Scroll1[i].wndCtrl = GetWndCtrl( StaticScrollID1[i] );
		m_Scroll1[i].staticNum = StaticScrollID1[i];
		m_Scroll1[i].isUse = FALSE;
		m_Scroll1[i].pItemElem = NULL;
		m_Scroll2[i].wndCtrl = GetWndCtrl( StaticScrollID2[i] );
		m_Scroll2[i].staticNum = StaticScrollID2[i];
		m_Scroll2[i].isUse = FALSE;
		m_Scroll2[i].pItemElem = NULL;
	}
	if(m_bBaruna)
	{
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pPic    = (CWndStatic*)GetDlgItem( StaticScrollID2[i] );
			pPic->SetVisible(FALSE);
		}
	}
	m_nResultStaticID[0] = WIDC_STATIC31;
	m_nResultStaticID[1] = WIDC_STATIC32;
	m_nResultStaticID[2] = WIDC_STATIC33;
	m_nResultStaticID[3] = WIDC_STATIC34;
	m_nResultStaticID[4] = WIDC_STATIC35;
	m_nResultStaticID[5] = WIDC_STATIC36;
	m_nResultStaticID[6] = WIDC_STATIC37;
	m_nResultStaticID[7] = WIDC_STATIC38;
	m_nResultStaticID[8] = WIDC_STATIC39;
	m_nResultStaticID[9] = WIDC_STATIC40;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
	FLASSERT(pWndStatic != NULL);
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY || m_eWndMode == WND_ELEMENT 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| m_eWndMode == WND_COSTUME
#endif		
		)
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_GRADE));
	}
	else if(m_eWndMode == WND_PIERCING)
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_PIERCING));
	}

	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
	FLASSERT(pWndStatic != NULL);
	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY || m_eWndMode == WND_ELEMENT 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| m_eWndMode == WND_COSTUME
#endif		
		)
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_GRADE));
	}
	else
	{
		pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_PIERCING));
	}

	RefreshInformation();

	switch(m_eWndMode)
	{
		case WND_NORMAL:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_NORMAL));
			break;
		case WND_ACCESSARY:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_ACCESSARY));
			break;
		case WND_PIERCING:
			SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_PIERCING));
			break;
		case WND_ELEMENT:
			SetTitle( prj.GetText( TID_GAME_SMELTSAFETY_ELEMENT ) );
			break;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
			SetTitle(prj.GetText(TID_MMI_SMELT_SAFETY_COSTUME));
			break;
#endif
	}

	if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
	{
		CRect rect;
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_nResultStaticID[i]);
			FLASSERT(pWndStatic != NULL);
			rect = pWndStatic->GetWndRect();
			pWndStatic->Move(rect.left + EXTENSION_PIXEL, rect.top);
		}

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
		FLASSERT(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
		FLASSERT(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
		FLASSERT(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
		FLASSERT(pWndStatic != NULL);
		rect = pWndStatic->GetWndRect();
		pWndStatic->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
		FLASSERT(pWndEdit != NULL);
		rect = pWndEdit->GetWndRect();
		pWndEdit->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_PLUS);
		FLASSERT(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_MINUS);
		FLASSERT(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_START);
		FLASSERT(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_STOP);
		FLASSERT(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_RESET);
		FLASSERT(pWndButton != NULL);
		rect = pWndButton->GetWndRect();
		pWndButton->Move(rect.left + HALF_EXTENSION_PIXEL, rect.top);

		CRect wndrect = GetWndRect();
		wndrect.right = wndrect.right + EXTENSION_PIXEL;
		SetWndRect(wndrect);
	}
	else
	{
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_Scroll2[i].staticNum);
			FLASSERT(pWndStatic != NULL);
			pWndStatic->SetVisible(FALSE);
			pWndStatic->EnableWindow(FALSE);
		}
	}

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	FLASSERT(pWndEdit != NULL);
	pWndEdit->AddWndStyle(EBS_NUMBER);
	pWndEdit->EnableWindow(FALSE);

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	if( pWndInventory )
		pWndInventory->CISetDieFlag_ItemCtrl( TRUE );

	m_pNowGaugeTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "SafetyGauge.bmp"), 0xffff00ff);
	m_pSuccessTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "SafetySuccess.bmp"), 0xffff00ff);
	m_pFailureTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"SafetyFailure.bmp"), 0xffff00ff);

	MoveParentCenter();
}

BOOL CWndSmeltSafety::Process()
{
	if( CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor() )
	{
		Destroy();
	}
	
	if(m_bStart != FALSE && m_pItemElem != NULL)
	{
		if(m_bResultSwitch != false)
		{
			m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwEnchantTimeStart) / static_cast<float>(m_dwEnchantTimeEnd - m_dwEnchantTimeStart);
		}
		else
		{
			m_fGaugeRate = 0.0f;
		}

		if(m_nValidSmeltCounter == 0 || GetNowSmeltValue() == GetDefaultMaxSmeltValue())
		{
			StopSmelting();
		}

		if(m_eWndMode == WND_PIERCING)
		{
			CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
			FLASSERT(pWndInventory != NULL);
			CWndStatic* pGoldNumberStatic = (CWndStatic*)pWndInventory->GetDlgItem(WIDC_GOLD_NUM);
			FLASSERT(pGoldNumberStatic != NULL);
			int nGoldNumber(atoi(pGoldNumberStatic->GetTitle()));
			if(nGoldNumber < 100000)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_LACKMONEY), NULL, prj.GetTextColor(TID_GAME_LACKMONEY));
				StopSmelting();
			}
		}
		if(m_dwEnchantTimeEnd < g_tmCurrent)
		{
			m_bResultSwitch = false;
			m_dwEnchantTimeStart = 0xffffffff;
			m_dwEnchantTimeEnd = 0xffffffff;

			// Send to Server...
			GENMATDIEINFO* pTargetMaterial = &m_Material[m_nCurrentSmeltNumber];
			GENMATDIEINFO* pTargetScroll1 = &m_Scroll1[m_nCurrentSmeltNumber];
			GENMATDIEINFO* pTargetScroll2 = &m_Scroll2[m_nCurrentSmeltNumber];
			if(pTargetMaterial->isUse != FALSE && m_Scroll1[m_nCurrentSmeltNumber].isUse != FALSE)
			{
				g_DPlay.SendSmeltSafety(
					m_pItemElem->m_dwObjId, 
					pTargetMaterial->pItemElem->m_dwObjId, 
					pTargetScroll1->pItemElem->m_dwObjId, 
					pTargetScroll2->isUse != FALSE ? pTargetScroll2->pItemElem->m_dwObjId : NULL_ID);
				
				pTargetMaterial->isUse = FALSE;
				if(pTargetMaterial->pItemElem != NULL)
				{
					pTargetMaterial->pItemElem->SetExtra(pTargetMaterial->pItemElem->GetExtra() - 1);
					pTargetMaterial->pItemElem = NULL;
				}
				pTargetScroll1->isUse = FALSE;
				if(pTargetScroll1->pItemElem != NULL)
				{
					pTargetScroll1->pItemElem->SetExtra(pTargetScroll1->pItemElem->GetExtra() - 1);
					pTargetScroll1->pItemElem = NULL;
				}
				if(pTargetScroll2->isUse != FALSE)
				{
					pTargetScroll2->pItemElem->SetExtra(pTargetScroll2->pItemElem->GetExtra() - 1);
					pTargetScroll2->isUse = FALSE;
					if(pTargetScroll2->pItemElem != NULL)
					{
						pTargetScroll2->pItemElem = NULL;
					}
				}
			}
			m_dwEnchantTimeStart = g_tmCurrent;
			m_dwEnchantTimeEnd = g_tmCurrent + ENCHANTWAITTIME;
		}
	}
	else
	{
		m_dwEnchantTimeStart = 0xffffffff;
		m_dwEnchantTimeEnd = 0xffffffff;
	}

	return TRUE;
}

void CWndSmeltSafety::OnDraw(C2DRender* p2DRender)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	FLASSERT(pWndStatic != NULL);
	CRect rectSmeltItem = pWndStatic->GetWndRect();

	if(m_pItemElem != NULL && m_pItemTexture != NULL)
	{
		// Draw Item
		m_pItemTexture->Render( p2DRender, CPoint( rectSmeltItem.left, rectSmeltItem.top ) );

		// Draw Enchant Effect
		if( m_dwEnchantTimeStart != 0xffffffff && m_dwEnchantTimeEnd != 0xffffffff )
		{
			RenderEnchant( p2DRender, CPoint(rectSmeltItem.left, rectSmeltItem.top) );
		}
	}

	// Draw Tooltip
	CPoint pointMouse = GetMousePoint();
	if(rectSmeltItem.PtInRect(pointMouse) != FALSE)
	{
		if(m_pItemElem != NULL)
		{
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectSmeltItem );
			g_WndMng.PutToolTip_Item((FLItemBase*)m_pItemElem, pointMouse, &rectSmeltItem);
		}
		else
		{
			ClientToScreen( &pointMouse );
			ClientToScreen( &rectSmeltItem );
			CString strEmptyTooltip;
			strEmptyTooltip.Format("%s", prj.GetText(TID_GAME_TOOLTIP_SMELT_SAFETY_ITEM));
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectSmeltItem, pointMouse);
		}
	}

	DrawListItem(p2DRender);

	for(int i = 0; i < m_nCurrentSmeltNumber; ++i)
	{
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_nResultStaticID[i]);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		rectStaticTemp.BottomRight().x = lpStatic->rect.right + nExtensionPixel;
		if(m_bResultStatic[i] != false)
		{
			m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, m_pSuccessTexture);
		}
		else
		{
			m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferSuccessImage, m_pFailureTexture);
		}
	}

	if(m_bStart != NULL && m_bResultSwitch != false)
	{
		const int nExtensionPixel( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) ? EXTENSION_PIXEL : 0 );
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(m_nResultStaticID[m_nCurrentSmeltNumber]);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth + nExtensionPixel;
		FLASSERT(m_pVertexBufferGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, 0xffffffff, m_pVertexBufferGauge, m_pNowGaugeTexture);
	}
}

BOOL CWndSmeltSafety::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WIDC_BUTTON_START:
		{
			if(m_bStart != FALSE)
				break;

			if(m_pItemElem == NULL)
			{
				// 먼저 안전 제련을 시도할 아이템을 등록해야 합니다.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR07), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR07));
				break;
			}

			if(m_nValidSmeltCounter > 0)
			{
				if(m_eWndMode == WND_PIERCING)
				{
					CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
					FLASSERT(pWndInventory != NULL);
					CWndStatic* pGoldNumberStatic = (CWndStatic*)pWndInventory->GetDlgItem(WIDC_GOLD_NUM);
					FLASSERT(pGoldNumberStatic != NULL);
					int nGoldNumber(atoi(pGoldNumberStatic->GetTitle()));
					if(nGoldNumber < 100000)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_LACKMONEY), NULL, prj.GetTextColor(TID_GAME_LACKMONEY));
						break;
					}
				}
				m_bStart = TRUE;
				m_bResultSwitch = true;
				m_dwEnchantTimeStart = g_tmCurrent;
				m_dwEnchantTimeEnd = g_tmCurrent + ENCHANTWAITTIME;
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				FLASSERT(pWndEdit != NULL);
				pWndEdit->EnableWindow(FALSE);
			}
			else
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				FLASSERT(pWndEdit != NULL);

				if(GetNowSmeltValue() == GetDefaultMaxSmeltValue())
				{
					// 이미 최대치까지 제련되어 더 이상 제련할 수 없습니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR13), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR13));
				}
				else if(m_nCurrentSmeltNumber >= SMELT_MAX)
				{
					// 제련 공간이 부족합니다. 계속 제련하기를 원하시면 Reset 버튼을 눌러 초기화해야 합니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR15), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR15));
				}
				else if(m_nMaterialCount <= m_nCurrentSmeltNumber || m_nScroll1Count <= m_nCurrentSmeltNumber)
				{
					switch(m_eWndMode)
					{
					case WND_NORMAL:
						{
							if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
							{
								// 오리칼쿰과 일반 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
								g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR08), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR08));
							}
							else
							{
								// 빛나는 오리칼쿰과 최상급 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
								g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR09), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR09));
							}
							break;
						}
					case WND_ACCESSARY:
						{
							// 문스톤과 액세서리 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR10), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR10));
							break;
						}
					case WND_PIERCING:
						{
							// 문스톤과 피어싱 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR11), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR11));
							break;
						}
					case WND_ELEMENT:
						{
							// 속성 카드와 일반 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR20 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR20 ) );
							break;
						}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
					case WND_COSTUME:
						{
							// 번개석과 코스튬 보호의 두루마리가 하나라도 등록된 상태가 아니면 수행할 수 없습니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR21), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR21));
							break;
						}
#endif

					}
				}
				else if(GetNowSmeltValue() >= atoi(pWndEdit->GetString()))
				{
					// 제련을 수행할 수 없는 조건입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR14), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR14));
				}
			}
			break;
		}
	case WIDC_BUTTON_STOP:
		{
			if(m_bStart != FALSE)
			{
				StopSmelting();
			}
			break;
		}
	case WIDC_BUTTON_RESET:
		{
			if(m_bStart == FALSE)
			{
				ResetData();
				RefreshInformation();
			}
			break;
		}
	case WIDC_EDIT_MAX_GRADE:
		{
			if(m_bStart == FALSE && m_pItemElem != NULL)
			{
				CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
				FLASSERT(pWndEdit != NULL);
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = min(nMaxSmeltNumber, GetDefaultMaxSmeltValue());
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	case WIDC_BUTTON_PLUS:
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			FLASSERT(pWndEdit != NULL);
			if(pWndEdit->IsWindowEnabled() != FALSE && m_bStart == FALSE && m_pItemElem != NULL)
			{
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = (++nMaxSmeltNumber < GetDefaultMaxSmeltValue()) ? nMaxSmeltNumber : GetDefaultMaxSmeltValue();
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	case WIDC_BUTTON_MINUS:
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			FLASSERT(pWndEdit != NULL);
			if(pWndEdit->IsWindowEnabled() != FALSE && m_bStart == FALSE && m_pItemElem != NULL)
			{
				int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
				nMaxSmeltNumber = (--nMaxSmeltNumber > 0) ? nMaxSmeltNumber : 0;
				CString strMaxSmeltNumber;
				strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
				pWndEdit->SetString(strMaxSmeltNumber);
				RefreshValidSmeltCounter();
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

BOOL CWndSmeltSafety::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if( pWndFrame == NULL )
		return FALSE;

	if( pWndFrame->GetWndId() != APP_INVENTORY )
	{
		SetForbid( TRUE );
		return FALSE;
	}

	CRect WndRect = GetClientRect();
	if(WndRect.PtInRect(point))
	{
		FLItemElem* pTempElem = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );
		if(pTempElem == NULL)
			return TRUE;

		PT_ITEM_SPEC pItemProp = pTempElem->GetProp();
		if(pItemProp == NULL)
			return TRUE;

		SetItem(pTempElem);
	}

	return TRUE;
}

void CWndSmeltSafety::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(m_bStart != FALSE)
		return;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if(pWndStatic == NULL)
		return;

	CRect rect = pWndStatic->GetWndRect();
	if(rect.PtInRect(point))
	{
		// 제련 아이템을 더블클릭하면 Reset 버튼으로 포커스를 강제로 옮긴다
		CWndBase* pWndResetButtonBase = GetDlgItem(WIDC_BUTTON_RESET);
		FLASSERT(pWndResetButtonBase != NULL);
		pWndResetButtonBase->SetFocus();

		if(m_pItemElem != NULL)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
		}
		m_pItemTexture = NULL;

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
		FLASSERT(pWndEdit != NULL);
		pWndEdit->SetString(TEXT(""));
		pWndEdit->EnableWindow(FALSE);

		ResetData();
		RefreshInformation();
	}
	else
	{
		if(IsDropMaterialZone(point) != FALSE && m_nMaterialCount > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nMaterialCount > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Material[m_nMaterialCount - 1]);
					--m_nMaterialCount;

					while(m_nScroll1Count > m_nMaterialCount)
					{
						SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
						--m_nScroll1Count;
					}

					if( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT )
					{
						while(m_nScroll2Count > m_nMaterialCount)
						{
							SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
							--m_nScroll2Count;
						}
					}
				}
			}
			else
			{
				SubtractListItem(&m_Material[m_nMaterialCount - 1]);
				--m_nMaterialCount;

				if(m_nScroll1Count > m_nMaterialCount)
				{
					SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
					--m_nScroll1Count;
				}

				if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && m_nScroll2Count > m_nMaterialCount )
				{
					SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
					--m_nScroll2Count;
				}
			}
			RefreshValidSmeltCounter();
			if( m_nMaterialCount == m_nCurrentSmeltNumber )
				m_pSelectedElementalCardItemProp = NULL;
		}
		else if(IsDropScroll1Zone(point) != FALSE && m_nScroll1Count > m_nCurrentSmeltNumber)
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll1Count > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
					--m_nScroll1Count;
				}
			}
			else
			{
				SubtractListItem(&m_Scroll1[m_nScroll1Count - 1]);
				--m_nScroll1Count;
			}
			RefreshValidSmeltCounter();
		}
		else if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && IsDropScroll2Zone(point) != FALSE && m_nScroll2Count > m_nCurrentSmeltNumber )
		{
			if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
			{
				while(m_nScroll2Count > m_nCurrentSmeltNumber)
				{
					SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
					--m_nScroll2Count;
				}
			}
			else
			{
				SubtractListItem(&m_Scroll2[m_nScroll2Count - 1]);
				--m_nScroll2Count;
			}
			RefreshValidSmeltCounter();
		}
	}
}

HRESULT CWndSmeltSafety::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	FLASSERT(m_pVertexBufferGauge != NULL);

	if( m_pVertexBufferSuccessImage == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferSuccessImage, 
												 NULL);
	FLASSERT(m_pVertexBufferSuccessImage != NULL);

	if( m_pVertexBufferFailureImage == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferFailureImage, 
												 NULL);
	FLASSERT(m_pVertexBufferFailureImage != NULL);
	return S_OK;
}
HRESULT CWndSmeltSafety::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	SAFE_RELEASE( m_pVertexBufferSuccessImage );
	SAFE_RELEASE( m_pVertexBufferFailureImage );
	return S_OK;
}
HRESULT CWndSmeltSafety::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	SAFE_RELEASE( m_pVertexBufferSuccessImage );
	SAFE_RELEASE( m_pVertexBufferFailureImage );
	return S_OK;
}

void CWndSmeltSafety::SetItem(FLItemElem* pItemElem)
{
	FLASSERT(pItemElem != NULL);

	if(m_bStart != FALSE)
	{
		// 안전 제련 도중에는 아이템을 등록할 수 없습니다.
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR12), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR12));
		return;
	}

	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	if(pItemProp == NULL)
		return;

	

	if(m_pItemElem == NULL) // 제련 아이템이 올려져 있지 않으면
	{
		BOOL bAcceptableItem = FALSE;
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
#ifdef BARUNA_UPGRADE_SMELT_SAFETY
				if( m_bBaruna )
				{
					if( !pItemProp->IsBaruna() )
					{
						// 제련할 수 없는 아이템입니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
						return;
					}
				}
				else
				{
					if( pItemProp->IsBaruna() )
					{
						// 제련할 수 없는 아이템입니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
						return;
					}
				}
#endif
				if(FLItemElem::IsDiceRefineryAble(pItemProp))
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// 제련할 수 없는 아이템입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
				}
				break;
			}
		case WND_ACCESSARY:
			{
				//if( pItemElem->IsCollector(TRUE) || pItemProp->dwItemKind2 == IK2_JEWELRY)		//gmpbigsun( 20111129 ) : 콜렉터 제외됨.
				if( pItemProp->dwItemKind2 == IK2_JEWELRY)
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// 제련할 수 없는 아이템입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
				}
				break;
			}
		case WND_PIERCING:
			{
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
				if( m_bBaruna )
				{
					if( !pItemProp->IsBarunaArmor() )
					{
						// 제련할 수 없는 아이템입니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
						return;
					}
				}
				else
				{
					if( pItemProp->IsBaruna() )
					{
						// 제련할 수 없는 아이템입니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
						return;
					}
				}
#endif
				if(pItemElem->IsPierceAble())
				{
					bAcceptableItem = TRUE;
				}
				else
				{
					// 피어싱 할 수 없는 아이템입니다.
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR02), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR02));
				}
				break;
			}
		case WND_ELEMENT:
			{
				if( FLItemElem::IsEleRefineryAble( pItemProp ) )
					bAcceptableItem = TRUE;
				else
				{
					// 제련할 수 없는 아이템입니다.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR01 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR01 ) );
				}
				break;
			}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
			if( (pItemProp->IsCostumeEnhanceParts()) && pItemElem->m_dwKeepTime == 0 )
			{
				bAcceptableItem = TRUE;
			}
			else
			{
				// 제련할 수 없는 아이템입니다.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR01), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR01));
			}
#endif
		}

		if(bAcceptableItem && m_pItemElem == NULL && pItemElem != NULL)
		{
			m_pItemElem = pItemElem;
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);

			FLASSERT(pItemProp != NULL);
			m_pItemTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );

			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			FLASSERT(pWndEdit != NULL);
			CString strDefaultSmeltMaxValue;
			strDefaultSmeltMaxValue.Format("%d", GetDefaultMaxSmeltValue());
			pWndEdit->EnableWindow(TRUE);
			pWndEdit->SetString(strDefaultSmeltMaxValue);

			RefreshInformation();
		}
	}
	else
	{
		if(IsAcceptableMaterial(pItemProp) != FALSE)
		{
			if(m_nMaterialCount < SMELT_MAX)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nMaterialCount < SMELT_MAX && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Material[m_nMaterialCount], pItemElem);
						++m_nMaterialCount;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Material[m_nMaterialCount], pItemElem);
						++m_nMaterialCount;
					}
				}
			}
		}
		else if(IsAcceptableScroll1(pItemProp) != FALSE)
		{
			if(m_nScroll1Count < m_nMaterialCount)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					while(m_nScroll1Count < m_nMaterialCount && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll1[m_nScroll1Count], pItemElem);
						++m_nScroll1Count;
					}
				}
				else
				{
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll1[m_nScroll1Count], pItemElem);
						++m_nScroll1Count;
					}
				}
			}
			else if(m_nScroll1Count != SMELT_MAX)
			{
				switch(m_eWndMode)
				{
				case WND_NORMAL:
					{
						if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
						{
							// 먼저 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR03), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR03));
						}
						else
						{
							// 먼저 빛나는 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR04), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR04));
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// 먼저 문스톤을 등록해야 합니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR05), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR05));
						break;
					}
				case WND_ELEMENT:
					{
						// 먼저 속성 카드를 등록해야 합니다.
						g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR17 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR17 ) );
						break;
					}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				case WND_COSTUME:
					{
						// 먼저 번개석을 등록해야 합니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR22), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR22));
						break;
					}
#endif
				}
			}
		}
		else if( ( m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT ) && IsAcceptableScroll2( pItemProp ) != FALSE )
		{
			if(m_nScroll2Count < m_nMaterialCount)
			{
				if(g_WndMng.m_pWndWorld->m_bShiftPushed != FALSE)
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					while(m_nScroll2Count < m_nMaterialCount && pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll2[m_nScroll2Count], pItemElem);
						++m_nScroll2Count;
					}
				}
				else
				{
					m_nScroll2Count = (m_nCurrentSmeltNumber > m_nScroll2Count) ? m_nCurrentSmeltNumber : m_nScroll2Count;
					if(pItemElem->GetExtra() < pItemElem->m_nItemNum)
					{
						AddListItem(&m_Scroll2[m_nScroll2Count], pItemElem);
						++m_nScroll2Count;
					}
				}
			}
			else if(m_nScroll2Count != SMELT_MAX)
			{
				switch(m_eWndMode)
				{
				case WND_NORMAL:
					{
						if(m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
						{
							// 먼저 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR03), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR03));
						}
						else
						{
							// 먼저 빛나는 오리칼쿰을 등록해야 합니다.
							g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR04), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR04));
						}
						break;
					}
				case WND_ACCESSARY:
				case WND_PIERCING:
					{
						// 먼저 문스톤을 등록해야 합니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR05), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR05));
						break;
					}
				case WND_ELEMENT:
					{
						// 먼저 속성 카드를 등록해야 합니다.
						g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR17 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR17 ) );
						break;
					}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				case WND_COSTUME:
					{
						// 먼저 번개석을 등록해야 합니다.
						g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR22), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR22));
						break;
					}
#endif
				}
			}
		}
		else
		{
			if( m_eWndMode == WND_ELEMENT && FLItemElem::IsElementalCard( pItemProp->dwID ) == TRUE )
			{
				if( m_pItemElem->GetItemResist() != SAI79::NO_PROP )
				{
					// 이미 아이템에 속성 제련이 되어 있습니다. 한 아이템에 두 속성이 들어갈 수 없습니다.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR18 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR18 ) );
				}
				else if( pItemProp != m_pSelectedElementalCardItemProp )
				{
					// 이미 다른 종류의 속성 카드가 등록되어 있습니다.
					g_WndMng.PutString( prj.GetText( TID_GAME_SMELT_SAFETY_ERROR19 ), NULL, prj.GetTextColor( TID_GAME_SMELT_SAFETY_ERROR19 ) );
				}
			}
			else
			{
				// 제련 아이템에 맞는 재료나 두루마리가 아닙니다.
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR06), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR06));
			}
		}
		RefreshValidSmeltCounter();
	}
}

void CWndSmeltSafety::RefreshInformation(void)
{
	RefreshText();
	RefreshValidSmeltCounter();
}

void CWndSmeltSafety::RefreshText(void)
{
	CString strItemAbility;
	if(m_pItemElem != NULL)
	{
		strItemAbility.Format("%d", GetNowSmeltValue());
	}
	else
	{
		strItemAbility.Format(TEXT("--"));
	}
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
	FLASSERT(pWndStatic != NULL);
	pWndStatic->SetTitle(strItemAbility);
}

void CWndSmeltSafety::RefreshValidSmeltCounter(void)
{
	// 유효한 제련 카운터를 (다시) 계산하여 갱신하는 코드
	m_nValidSmeltCounter = m_nMaterialCount;
	m_nValidSmeltCounter = (m_nScroll1Count < m_nValidSmeltCounter) ? m_nScroll1Count : m_nValidSmeltCounter;
	m_nValidSmeltCounter -= m_nCurrentSmeltNumber;
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	FLASSERT(pWndEdit != NULL);
	int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
	int nNowSmeltNumber(GetNowSmeltValue());
	int nSmeltNumber(nMaxSmeltNumber - nNowSmeltNumber);
	nSmeltNumber = (nSmeltNumber > 0) ? nSmeltNumber : 0;
	m_nValidSmeltCounter = (nSmeltNumber < m_nValidSmeltCounter) ? nSmeltNumber : m_nValidSmeltCounter;
}

void CWndSmeltSafety::StopSmelting(void)
{
	m_bStart = FALSE;
	m_bResultSwitch = false;
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	FLASSERT(pWndEdit != NULL);
	pWndEdit->EnableWindow(TRUE);
}

void CWndSmeltSafety::DisableScroll2(void)
{
	FLASSERT(m_pItemElem != NULL);
	if( ( m_eWndMode == WND_NORMAL && ( m_pItemElem->GetAbilityOption() >= GENERAL_NON_USING_SCROLL2_LEVEL || m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE ) ) || 
		( m_eWndMode == WND_ELEMENT && m_pItemElem->GetResistAbilityOption() >= ELEMENTAL_NON_USING_SCROLL2_LEVEL ) )
	{
		m_nScroll2Count = 0;
		for(int i = 0; i < SMELT_MAX; ++i)
		{
			GENMATDIEINFO* pTargetScroll2 = &m_Scroll2[i];
			if(pTargetScroll2->pItemElem != NULL)
			{
				pTargetScroll2->isUse = FALSE;
				pTargetScroll2->pItemElem->SetExtra(pTargetScroll2->pItemElem->GetExtra() - 1);
				pTargetScroll2->pItemElem = NULL;
			}
		}
	}
}

void CWndSmeltSafety::ResetData(void)
{
	m_nMaterialCount = 0;
	m_nScroll1Count = 0;
	m_nScroll2Count = 0;
	m_nCurrentSmeltNumber = 0;
	for(int i = 0; i < SMELT_MAX; ++i)
	{
		m_Material[i].isUse = FALSE;
		if(m_Material[i].pItemElem != NULL)
		{
			m_Material[i].pItemElem->SetExtra(0);
			m_Material[i].pItemElem = NULL;
		}
		m_Scroll1[i].isUse = FALSE;
		if(m_Scroll1[i].pItemElem != NULL)
		{
			m_Scroll1[i].pItemElem->SetExtra(0);
			m_Scroll1[i].pItemElem = NULL;
		}
		m_Scroll2[i].isUse = FALSE;
		if(m_Scroll2[i].pItemElem != NULL)
		{
			m_Scroll2[i].pItemElem->SetExtra(0);
			m_Scroll2[i].pItemElem = NULL;
		}

		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(m_nResultStaticID[i]);
		FLASSERT(pWndStatic != NULL);
		pWndStatic->SetTitle("");
	}
	m_pSelectedElementalCardItemProp = NULL;
}

void CWndSmeltSafety::AddListItem(GENMATDIEINFO* pListItem, FLItemElem* pItemElem)
{
	FLASSERT(pListItem->isUse == FALSE && pListItem->pItemElem == NULL);
	pListItem->isUse = TRUE;
	pListItem->pItemElem = pItemElem;
	pListItem->pItemElem->SetExtra(pItemElem->GetExtra() + 1);
}
void CWndSmeltSafety::SubtractListItem(GENMATDIEINFO* pListItem)
{
	FLASSERT(pListItem->isUse != FALSE && pListItem->pItemElem != NULL);
	pListItem->isUse = FALSE;
	pListItem->pItemElem->SetExtra(pListItem->pItemElem->GetExtra() - 1);
	pListItem->pItemElem = NULL;
}

void CWndSmeltSafety::DrawListItem(C2DRender* p2DRender)
{
	if((m_eWndMode == WND_NORMAL || m_eWndMode == WND_PIERCING) && (m_pItemElem == NULL || m_pItemTexture == NULL))
		return;

	static const int NORMAL_ALPHA(255);
	static const int TRANSLUCENT_ALPHA(75);
	static int nAlphaBlend(NORMAL_ALPHA);
	PT_ITEM_SPEC pItemProp = NULL;
	CTexture* pTexture = NULL;

	for(int i = m_nCurrentSmeltNumber; i < SMELT_MAX; ++i)
	{
		FLASSERT(m_Material[i].wndCtrl != NULL);
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				if( m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
					pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ));
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				else if( m_pItemElem->GetProp()->IsBaruna())
					pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 7049, II_GEN_MAT_OPERCID ));
#endif
				else
					pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ));
				break;
			}
		case WND_ACCESSARY:
			{
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) );
				break;
			}
		case WND_PIERCING:
			{
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
				pItemProp = (m_pItemElem->GetProp()->IsBarunaArmor()) ? g_xSpecManager->GetSpecItem( ITEM_INDEX( 25580, II_GEN_MAT_COSMOSTONE ) ) : g_xSpecManager->GetSpecItem( ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) );
#else
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) );
#endif
				break;
			}
		case WND_ELEMENT:
			{
				pItemProp = m_pSelectedElementalCardItemProp;
				if( m_pItemElem )
				{
					switch( m_pItemElem->GetItemResist() )
					{
					case SAI79::FIRE:
						{
							pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 3206, II_GEN_MAT_ELE_FLAME ) );
							break;
						}
					case SAI79::WATER:
						{
							pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 3211, II_GEN_MAT_ELE_RIVER ) );
							break;
						}
					case SAI79::ELECTRICITY:
						{
							pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 3216, II_GEN_MAT_ELE_GENERATOR ) );
							break;
						}
					case SAI79::WIND:
						{
							pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 3226, II_GEN_MAT_ELE_CYCLON ) );
							break;
						}
					case SAI79::EARTH:
						{
							pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 3221, II_GEN_MAT_ELE_DESERT ) );
							break;
						}
					}
				}
				break;
			}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
			{
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 25334, II_GEN_MAT_LIGHTNINGSTONE ) );
				break;
			}
#endif
		}
		if( m_eWndMode != WND_ELEMENT || pItemProp )
		{
			FLASSERT(pItemProp != NULL);
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			FLASSERT(pTexture != NULL);
			nAlphaBlend = (m_Material[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Material[i].wndCtrl->rect.left, m_Material[i].wndCtrl->rect.top ), nAlphaBlend );
		}

		FLASSERT(m_Scroll1[i].wndCtrl != NULL);
		switch(m_eWndMode)
		{
		case WND_NORMAL:
			{
				if( m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE )
					pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ));
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
				else if( m_pItemElem->GetProp()->IsBaruna())
					pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 20486, II_SYS_SYS_SCR_BARUNASMELPROT ));
#endif
				else
					pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ));

				break;
			}
		case WND_ACCESSARY:
			{
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 26473, II_SYS_SYS_SCR_SMELPROT4 ) );
				break;
			}
		case WND_PIERCING:
			{
				pItemProp = (m_pItemElem->GetProp()->IsBaruna()) ? g_xSpecManager->GetSpecItem( ITEM_INDEX( 25581, II_SYS_SYS_SCR_PIEPROT3 ) ) : g_xSpecManager->GetSpecItem( ITEM_INDEX( 10465, II_SYS_SYS_SCR_PIEPROT ) );
				break;
			}
		case WND_ELEMENT:
			{
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) );
				break;
			}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
			{
				pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 25336, II_SYS_SYS_SCR_COSTUMEPROT ) );
				break;
			}
#endif
		}
		FLASSERT(pItemProp != NULL);
#ifdef BARUNA_UPGRADE_SMELT_SAFETY
		if(m_Scroll1[i].pItemElem != NULL)
		{
			pItemProp = m_Scroll1[i].pItemElem->GetProp();
		}
#endif
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		FLASSERT(pTexture != NULL);
		nAlphaBlend = (m_Scroll1[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render( p2DRender, CPoint( m_Scroll1[i].wndCtrl->rect.left, m_Scroll1[i].wndCtrl->rect.top ), nAlphaBlend );

		if( m_eWndMode == WND_NORMAL && !m_bBaruna && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE && m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL )
		{
			FLASSERT(m_Scroll2[i].wndCtrl != NULL);
			pItemProp = g_xSpecManager->GetSpecItem(ITEM_INDEX( 10468, II_SYS_SYS_SCR_SMELTING ));
			FLASSERT(pItemProp != NULL);
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			FLASSERT(pTexture != NULL);
			nAlphaBlend = (m_Scroll2[i].isUse != FALSE) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Scroll2[i].wndCtrl->rect.left, m_Scroll2[i].wndCtrl->rect.top ), nAlphaBlend );
		}

		if( m_eWndMode == WND_ELEMENT && m_pItemElem && m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
		{
			FLASSERT( m_Scroll2[ i ].wndCtrl != NULL );
			pItemProp = g_xSpecManager->GetSpecItem( ITEM_INDEX( 10489, II_SYS_SYS_SCR_SMELTING2 ) );
			FLASSERT( pItemProp != NULL );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );
			FLASSERT( pTexture != NULL );
			nAlphaBlend = ( m_Scroll2[ i ].isUse != FALSE ) ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
			pTexture->Render( p2DRender, CPoint( m_Scroll2[ i ].wndCtrl->rect.left, m_Scroll2[ i ].wndCtrl->rect.top ), nAlphaBlend );
		}
	}
}

BOOL CWndSmeltSafety::IsDropMaterialZone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Material[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsDropScroll1Zone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Scroll1[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsDropScroll2Zone(CPoint point)
{
	BOOL rtnval = FALSE;
	for(int i=0; i<SMELT_MAX; i++)
	{
		if(m_Scroll2[i].wndCtrl->rect.PtInRect(point))
		{
			rtnval = TRUE;
			i = SMELT_MAX;
		}
	}
	return rtnval;
}

BOOL CWndSmeltSafety::IsAcceptableMaterial(PT_ITEM_SPEC pItemProp)
{
	FLASSERT(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			if(m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
			{
				if(pItemProp->dwID == ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ))
					bAcceptableItem = TRUE;
				
			}
			else if(m_pItemElem->GetProp()->IsBaruna())
			{
				if(pItemProp->dwItemKind3 == IK3_OPERCID)
					bAcceptableItem = TRUE;
			}
			else
			{
				if(pItemProp->dwID == ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 ) || pItemProp->dwID == ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 ))
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ACCESSARY:
	case WND_PIERCING:
		{
			if(pItemProp->dwID == ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE ) || pItemProp->dwID == ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 )
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
				 || pItemProp->dwID == ITEM_INDEX( 25580, II_GEN_MAT_COSMOSTONE )
#endif
)
				bAcceptableItem = TRUE;
			break;
		}
	case WND_ELEMENT:
		{
			switch( m_pItemElem->GetItemResist() )
			{
			case SAI79::FIRE:
				{
					if( pItemProp->dwID == ITEM_INDEX( 3206, II_GEN_MAT_ELE_FLAME ) )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::WATER:
				{
					if( pItemProp->dwID == ITEM_INDEX( 3211, II_GEN_MAT_ELE_RIVER ) )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::ELECTRICITY:
				{
					if( pItemProp->dwID == ITEM_INDEX( 3216, II_GEN_MAT_ELE_GENERATOR ) )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::WIND:
				{
					if( pItemProp->dwID == ITEM_INDEX( 3226, II_GEN_MAT_ELE_CYCLON ) )
						bAcceptableItem = TRUE;
					break;
				}
			case SAI79::EARTH:
				{
					if( pItemProp->dwID == ITEM_INDEX( 3221, II_GEN_MAT_ELE_DESERT ) )
						bAcceptableItem = TRUE;
					break;
				}
			default:
				{
					if( m_nMaterialCount == m_nCurrentSmeltNumber )
					{
						if( FLItemElem::IsElementalCard( pItemProp->dwID ) == TRUE )
						{
							m_pSelectedElementalCardItemProp = pItemProp;
							bAcceptableItem = TRUE;
						}
					}
					else
					{
						if( pItemProp == m_pSelectedElementalCardItemProp )
							bAcceptableItem = TRUE;
					}
				}
			}
			break;
		}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
		{
			if(pItemProp->dwID == ITEM_INDEX( 25334, II_GEN_MAT_LIGHTNINGSTONE ))
				bAcceptableItem = TRUE;
			break;
		}
#endif
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll1(PT_ITEM_SPEC pItemProp)
{
	FLASSERT(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
			if(m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
			{
				if(pItemProp->dwID == ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ))
					bAcceptableItem = TRUE;
			}
			else if(m_pItemElem->GetProp()->IsBaruna())
			{
				if(pItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION || pItemProp->dwItemKind3 == IK3_BARUNA_PROTECTION_KEEP)
					bAcceptableItem = TRUE;
			}
			else
			{
				if(pItemProp->dwID == ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ))
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ACCESSARY:
		{
			if(pItemProp->dwID == ITEM_INDEX( 26473, II_SYS_SYS_SCR_SMELPROT4 ) || pItemProp->dwID == ITEM_INDEX( 25233, II_SYS_SYS_SCR_SMELPROT6 ))
				bAcceptableItem = TRUE;
			break;
		}
	case WND_PIERCING:
		{
			if(pItemProp->dwID == ITEM_INDEX( 10465, II_SYS_SYS_SCR_PIEPROT ) || pItemProp->dwID == ITEM_INDEX( 25234, II_SYS_SYS_SCR_PIEPROT2 )
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
				|| pItemProp->dwID == ITEM_INDEX( 25581, II_SYS_SYS_SCR_PIEPROT3 )
#endif
)
				bAcceptableItem = TRUE;
			break;
		}
	case WND_ELEMENT:
		{
			if( pItemProp->dwID == ITEM_INDEX( 10464, II_SYS_SYS_SCR_SMELPROT ) )
				bAcceptableItem = TRUE;
			break;
		}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		case WND_COSTUME:
		{
			if(pItemProp->dwID == ITEM_INDEX( 25336, II_SYS_SYS_SCR_COSTUMEPROT ))
				bAcceptableItem = TRUE;
			break;
		}
#endif
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll2(PT_ITEM_SPEC pItemProp)
{
	FLASSERT(m_pItemElem != NULL);
	BOOL bAcceptableItem(FALSE);
	switch( m_eWndMode )
	{
	case WND_NORMAL:
		{
			if( m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE )
			{
				if( pItemProp->dwID == ITEM_INDEX( 10468, II_SYS_SYS_SCR_SMELTING ) )
					bAcceptableItem = TRUE;
			}
			break;
		}
	case WND_ELEMENT:
		{
			if( m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL )
			{
				if( pItemProp->dwID == ITEM_INDEX( 10489, II_SYS_SYS_SCR_SMELTING2 ) )
					bAcceptableItem = TRUE;
			}
			break;
		}
	}
	return bAcceptableItem;
}

int CWndSmeltSafety::GetNowSmeltValue(void)
{
	int nNowSmeltValue(0);
	if(m_pItemElem != NULL)
	{
		if(m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
			|| m_eWndMode == WND_COSTUME 
#endif			
			)
		{
			nNowSmeltValue = m_pItemElem->GetAbilityOption();
		}
		else if(m_eWndMode == WND_PIERCING)
		{
			nNowSmeltValue = m_pItemElem->GetGeneralPiercingSize();
		}
		else if( m_eWndMode == WND_ELEMENT )
			nNowSmeltValue = m_pItemElem->GetResistAbilityOption();
	}
	return nNowSmeltValue;
}

int CWndSmeltSafety::GetDefaultMaxSmeltValue(void)
{
	FLASSERT(m_pItemElem != NULL);
	int nDefaultMaxSmeltValue(0);
	switch(m_eWndMode)
	{
	case WND_NORMAL:
		{
#ifdef BARUNA_UPGRADE_SMELT_SAFETY
			if(m_pItemElem->GetProp()->IsBaruna())
				nDefaultMaxSmeltValue = 20;
			else
#endif
				nDefaultMaxSmeltValue = 10;
			break;
		}
	case WND_ACCESSARY:
		{
			nDefaultMaxSmeltValue = 20;
			break;
		}
	case WND_PIERCING:
		{
			if(m_pItemElem->GetProp()->dwItemKind3 == IK3_SUIT)
			{
				nDefaultMaxSmeltValue = 4;
			}
			else
			{
				nDefaultMaxSmeltValue = 10;
			}
			break;
		}
	case WND_ELEMENT:
		{
			nDefaultMaxSmeltValue = 20;
			break;
		}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	case WND_COSTUME:
		{
			nDefaultMaxSmeltValue = 10;
			break;
		}
#endif
	}
	return nDefaultMaxSmeltValue;
}

CWndSmeltSafetyConfirm::CWndSmeltSafetyConfirm(ErrorMode eErrorMode) : 
m_eErrorMode(eErrorMode), 
m_pItemElem(NULL)
{
}

CWndSmeltSafetyConfirm::~CWndSmeltSafetyConfirm()
{
}

BOOL CWndSmeltSafetyConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SMELT_SAFETY_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndSmeltSafetyConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_CONFIRM_TEXT);
	FLASSERT(pWndText != NULL);

	switch(m_eErrorMode)
	{
	case WND_ERROR1:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_NORMAL));
			break;
		}
	case WND_ERROR2:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_HIGHEST));
			break;
		}
	case WND_ERROR3:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_ACCESSARY));
			break;
		}
	case WND_ERROR4:
		{
			pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_COSTUME));
			break;
		}
	}

	MoveParentCenter();
}

BOOL CWndSmeltSafetyConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);

	if(pWndInventory != NULL)
	{
		if(nID == WIDC_SMELT_YES)
		{
			if(m_pItemElem != NULL)
				pWndInventory->RunUpgrade(m_pItemElem);

			Destroy();
		}
		else if(nID == WIDC_SMELT_NO)
		{
			pWndInventory->BaseMouseCursor();
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSmeltSafetyConfirm::OnDestroy()
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
		pWndInventory->BaseMouseCursor();
}

void CWndSmeltSafetyConfirm::SetWndMode(FLItemElem* pItemElem)
{
	m_pItemElem = pItemElem;	
}

CWndEquipBindConfirm::CWndEquipBindConfirm(EquipAction eEquipAction) : 
m_eEquipAction(eEquipAction), 
m_pItemBase(NULL), 
m_dwObjId(0), 
m_pItemElem( NULL )
{
}

CWndEquipBindConfirm::~CWndEquipBindConfirm(void)
{
}

BOOL CWndEquipBindConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EQUIP_BIND_CONFIRM, WBS_MODAL | WBS_KEY, CPoint( 0, 0 ), pWndParent );
}

void CWndEquipBindConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_EQUIP_BIND_TEXT);
	FLASSERT(pWndText != NULL);
	pWndText->AddString(prj.GetText(TID_TOOLTIP_EQUIPBIND_CONFIRM));

	CWndEdit* pWndEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT_FOCUS );
	FLASSERT( pWndEdit );
	pWndEdit->Move( -100, -100 );
	pWndEdit->SetFocus();

	CWndButton* pWndButton = ( CWndButton* )GetDlgItem( WIDC_EQUIP_BIND_YES );
	FLASSERT( pWndButton );
	pWndButton->SetDefault( TRUE );

	MoveParentCenter();
}

BOOL CWndEquipBindConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT_FOCUS )
	{
		EquipItem();
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}

	switch( nID )
	{
	case WIDC_EQUIP_BIND_YES:
		{
			switch( m_eEquipAction )
			{
			case EQUIP_DOUBLE_CLICK:
				{
					EquipItem();
					Destroy();
					break;
				}
			case EQUIP_DRAG_AND_DROP:
				{
					g_DPlay.SendDoEquip( m_pItemElem );
					Destroy();
					break;
				}
			default:
				{
					Destroy();
				}
			}
			break;
		}
	case WIDC_EQUIP_BIND_NO:
		{
			Destroy();
			break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndEquipBindConfirm::SetInformation(FLItemBase* pItemBase, DWORD dwObjId)
{
	m_eEquipAction = EQUIP_DOUBLE_CLICK;
	m_pItemBase = pItemBase;
	FLASSERT(m_pItemBase != NULL);
	m_dwObjId = dwObjId;
}

void CWndEquipBindConfirm::SetInformation(FLItemElem* pItemElem)
{
	m_eEquipAction = EQUIP_DRAG_AND_DROP;
	m_pItemElem = pItemElem;
	FLASSERT(m_pItemElem != NULL);
}

void CWndEquipBindConfirm::EquipItem( void )
{
	PT_ITEM_SPEC pItemProp = m_pItemBase->GetProp();
	if( pItemProp != NULL)
	{
		int nPart = pItemProp->dwParts;
		BOOL bEquiped = g_pPlayer->m_Inventory.IsEquip( m_pItemBase->m_dwObjId );
		if( bEquiped == TRUE )
			nPart = m_pItemBase->m_dwObjIndex;// - g_pPlayer->m_Inventory.m_dwIndexNum;

		g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, m_pItemBase->m_dwObjId ), m_dwObjId, nPart );
	}
}


CWndRestateConfirm::CWndRestateConfirm(DWORD dwItemID) : 
m_dwItemID(dwItemID), 
m_ObjID(0), 
m_nPart(-1)
{
}

CWndRestateConfirm::~CWndRestateConfirm(void)
{
}

BOOL CWndRestateConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_RESTATE_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndRestateConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_RESTATE_CONFIRM_TEXT);
	FLASSERT(pWndText != NULL);
	switch(m_dwItemID)
	{
	case ITEM_INDEX( 10211, II_CHR_SYS_SCR_RESTATE ):
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_ALL_CONFIRM));
			break;
		}
	case ITEM_INDEX( 30151, II_CHR_SYS_SCR_RESTATE_STR ):
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STR_CONFIRM));
			break;
		}
	case ITEM_INDEX( 30153, II_CHR_SYS_SCR_RESTATE_STA ):
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STA_CONFIRM));
			break;
		}
	case ITEM_INDEX( 30152, II_CHR_SYS_SCR_RESTATE_DEX ):
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_DEX_CONFIRM));
			break;
		}
	case ITEM_INDEX( 30154, II_CHR_SYS_SCR_RESTATE_INT ):
		{
			pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_INT_CONFIRM));
			break;
		}
	case ITEM_INDEX( 30337, II_CHR_SYS_SCR_RESTATE_STR_LOW ):
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_STR_LOW_CONFIRM ) );
			break;
		}
	case ITEM_INDEX( 30339, II_CHR_SYS_SCR_RESTATE_STA_LOW ):
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_STA_LOW_CONFIRM ) );
			break;
		}
	case ITEM_INDEX( 30338, II_CHR_SYS_SCR_RESTATE_DEX_LOW ):
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_DEX_LOW_CONFIRM ) );
			break;
		}
	case ITEM_INDEX( 30340, II_CHR_SYS_SCR_RESTATE_INT_LOW ):
		{
			pWndText->AddString( prj.GetText( TID_GAME_RESTATE_INT_LOW_CONFIRM ) );
			break;
		}
	}

	MoveParentCenter();
}

BOOL CWndRestateConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if(pWndInventory != NULL)
	{
		switch(nID)
		{
		case WIDC_RESTATE_CONFIRM_YES:
			{
				g_DPlay.SendDoUseItem(m_dwItemObjID, m_ObjID, m_nPart);
				Destroy();
				break;
			}
		case WIDC_RESTATE_CONFIRM_NO:
			{
				Destroy();
				break;
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndRestateConfirm::SetInformation(DWORD dwItemObjID, OBJID m_ObjID, int nPart)
{
	m_dwItemObjID = dwItemObjID;
	m_ObjID = m_ObjID;
	m_nPart = nPart;
}

//-----------------------------------------------------------------------------
CWndCampusInvitationConfirm::CWndCampusInvitationConfirm( u_long idSender, const CString& rstrSenderName ) : 
m_idSender( idSender ), 
m_strSenderName( rstrSenderName )
{
}
//-----------------------------------------------------------------------------
CWndCampusInvitationConfirm::~CWndCampusInvitationConfirm( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampusInvitationConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_ENTER, WBS_KEY, 0, pWndParent );
}
//-----------------------------------------------------------------------------
void CWndCampusInvitationConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	if( pWndText )
	{
		CString strMessage = _T( "" );
		strMessage.Format( prj.GetText( TID_GAME_CAMPUS_INVITATION_CONFIRM ), m_strSenderName ); // %s 님이 사제 신청을 하셨습니다. 수락하시겠습니까?
		pWndText->AddString( strMessage );
	}

	CWndButton* pOk = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
	if( pOk )
		pOk->SetTitle( prj.GetText( TID_MMI_UIBUTTON_BUTTON04 ) );

	CWndButton* pCancel = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );
	if( pCancel )
		pCancel->SetTitle( prj.GetText( TID_MMI_UIBUTTON_BUTTON05 ) );

	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	AddWndStyle( WBS_MODAL );
	pOk->SetDefault( TRUE );

	//에디트창을 안 보이는 곳으로 보내 버리고 ENTER 받을 준비
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndCampusInvitationConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT1 )
	{
		g_DPlay.SendAcceptCampusMember( m_idSender );
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			g_DPlay.SendAcceptCampusMember( m_idSender );
			Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			g_DPlay.SendRefuseCampusMember( m_idSender );
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
CWndCampusSeveranceConfirm::CWndCampusSeveranceConfirm( u_long idTarget, const CString& rstrTargetName ) : 
m_idTarget( idTarget ), 
m_strTargetName( rstrTargetName )
{
}
//-----------------------------------------------------------------------------
CWndCampusSeveranceConfirm::~CWndCampusSeveranceConfirm( void )
{
}
//-----------------------------------------------------------------------------
BOOL CWndCampusSeveranceConfirm::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CONFIRM_ENTER, WBS_KEY, 0, pWndParent );
}
//-----------------------------------------------------------------------------
void CWndCampusSeveranceConfirm::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = ( CWndText* )GetDlgItem( WIDC_TEXT1 );
	if( pWndText )
	{
		CString strMessage = _T( "" );
		strMessage.Format( prj.GetText( TID_GAME_CAMPUS_SEVERANCE_CONFIRM ), m_strTargetName ); // %s 님과 사제 관계를 끊겠습니까?
		pWndText->AddString( strMessage );
	}

	CWndButton* pOk = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );

	if( pOk )
		pOk->SetTitle( prj.GetText( TID_MMI_UIBUTTON_BUTTON04 ) );

	CWndButton* pCancel = ( CWndButton* )GetDlgItem( WIDC_BUTTON2 );

	if( pCancel )
		pCancel->SetTitle( prj.GetText( TID_MMI_UIBUTTON_BUTTON05 ) );

	CWndEdit* pEdit = ( CWndEdit* )GetDlgItem( WIDC_EDIT1 );
	AddWndStyle( WBS_MODAL );
	pOk->SetDefault( TRUE );

	//에디트창을 안 보이는 곳으로 보내 버리고 ENTER 받을 준비
	pEdit->Move( -100, -100 );
	pEdit->SetFocus( );

	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndCampusSeveranceConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( message == EN_RETURN && nID == WIDC_EDIT1 )
	{
		g_DPlay.SendRemoveCampusMember( m_idTarget );
		Destroy();
		return CWndNeuz::OnChildNotify( message, nID, pLResult );
	}
	
	switch( nID )
	{
	case WIDC_BUTTON1:
		{
			g_DPlay.SendRemoveCampusMember( m_idTarget );
			Destroy();
			break;
		}
	case WIDC_BUTTON2:
		{
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------

#ifdef __SKILL_UI16

CWndSkill_16::CWndSkill_16( ) : 
m_nMaxCurrJob(0),
m_nCurrSkillPoint(0),
m_apSkill(NULL),
m_pFocusItem(NULL),
m_nJob(0),
m_dwMouseSkill(NULL_ID),
m_bLegend( FALSE ),
m_bDrag( FALSE ),
m_nSelectedClassLevel( MAX_JOBBASE ),
m_pTexSeletionBack( NULL )
{
	memset( m_pTexSkill, 0, sizeof(m_pTexSkill) );
	m_pTexJobPannel = NULL;

	m_pWndButton[0] = NULL;
	m_pWndButton[1] = NULL;
	m_pWndButton[2] = NULL;
	m_pWndButton[3] = NULL;
}
//-----------------------------------------------------------------------------
CWndSkill_16::~CWndSkill_16( )
{
	DeleteDeviceObjects();
}
//-----------------------------------------------------------------------------
BOOL CWndSkill_16::Initialize( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SKILL4, WBS_KEY, 0, pWndParent );
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnInitialUpdate( void )
{
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();

	if( NULL == g_pPlayer )
	{ 
		FLERROR_LOG( PROGRAM_NAME, _T( "g_pPlayer is NULL" ) );
		return;
	}

	InitItem( g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill );

	m_pWndButton[0] = (CWndButton*) GetDlgItem( WIDC_BUTTON_PLUS );			// + 버튼
	m_pWndButton[1] = (CWndButton*) GetDlgItem( WIDC_BUTTON_MINUS );		// - 버튼
	m_pWndButton[2] = (CWndButton*) GetDlgItem( WIDC_BUTTON_OK );			// ok 버튼
	m_pWndButton[3] = (CWndButton*) GetDlgItem( WIDC_BUTTON_BACK );			// cancel 버튼( 초기화 )

	if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		m_bLegend = TRUE;
	else
		m_bLegend = FALSE;

	m_pTexSeletionBack = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "WndPosMark.tga" ), 0xffff00ff );
	FLASSERT( m_pTexSeletionBack );
	
	MoveParentCenter();
}
//-----------------------------------------------------------------------------
BOOL CWndSkill_16::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	//class button init
	//switch( nID )
	//{
	//case WIDC_BUTTON_C1:
	//case WIDC_BUTTON_C2:
	//case WIDC_BUTTON_C3:
	//case WIDC_BUTTON_C4:
	//	{
	//		CWndButton* pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C1 );
	//		CWndButton* pWndButton2 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C2 );
	//		CWndButton* pWndButton3 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C3 );
	//		CWndButton* pWndButton4 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C4 );

	//		pWndButton1->SetCheck( 0 );
	//		pWndButton2->SetCheck( 0 );
	//		pWndButton3->SetCheck( 0 );
	//		pWndButton4->SetCheck( 0 );
	//	}
	//	
	//	CWndButton* pBtn = (CWndButton*)GetDlgItem( nID );
	//	if( pBtn )
	//		pBtn->SetCheck( 2 );

	//	break;
	//}

	//class button
	switch( nID )
	{
	case WIDC_BUTTON_C1:	
		m_nSelectedClassLevel = MAX_JOBBASE;	
		InitItem( -2, g_pPlayer->m_aJobSkill );		//-2는 의미없는 값이다라는 의미임.  
		break;
	case WIDC_BUTTON_C2:	
        m_nSelectedClassLevel = MAX_EXPERT;
		InitItem( -2, g_pPlayer->m_aJobSkill );
		break;
	case WIDC_BUTTON_C3:	
		m_nSelectedClassLevel = MAX_PROFESSIONAL;
		InitItem( -2, g_pPlayer->m_aJobSkill );
		break;
	case WIDC_BUTTON_C4:	
		m_nSelectedClassLevel = MAX_LEGEND_HERO;
		InitItem( -2, g_pPlayer->m_aJobSkill );
		break;
	}

	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		SkillProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp )
		{
			int nPoint = prj.GetSkillPoint( pSkillProp );
			switch( nID )
			{
			case WIDC_BUTTON_PLUS:	// + 버튼
				if( nPoint <= m_nCurrSkillPoint )
				{
					if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
					{
						m_nCurrSkillPoint -= nPoint;
						++m_pFocusItem->dwLevel;
					}
					else
					{
						break;							
					}
				}					
				break;

			case WIDC_BUTTON_MINUS:	// - 버튼
				if( CanDownPoint(m_pFocusItem->dwSkill) )
				{
					m_nCurrSkillPoint += nPoint;
					--m_pFocusItem->dwLevel;

					//이 버젼에서는 선택된 스킬만 조작할수있다. 즉 조작하는 스킬을 레벨다운 시켜도 연결된 스킬은 무시함.
					//SubSkillPointDown( m_pFocusItem );
				}
				break;

			case WIDC_BUTTON_BACK:	// Reset 버튼
				if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
					InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE );
				m_pFocusItem = NULL;
				break;

			case WIDC_BUTTON_OK:	// Finish 버튼
				{
					if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
					{
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
					}
				}
				break;
			}

		}
		
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
//-----------------------------------------------------------------------------
void CWndSkill_16::InitItem(int nJob, LPSKILL apSkill, BOOL bReset )
{
	//nJob 
	//-1 : 잘못된 값
	//-2 : 의도적인 의미없는 값( 현재 직업을 바꾸지 말고 세팅하라는 의미 )
	if( NULL == g_pPlayer )
		return;

	if( -1 == nJob )
		return;

	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy( m_apSkill, apSkill, sizeof(SKILL)*MAX_SKILL_JOB );
	}
	else
	{
		m_nCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}

	if( NULL == m_apSkill )
		return;

	if( -2 == nJob )
	{
		//-2는 의미가 없는 값이며, UI 상에서 선택한 잡이다.
	}
	else if( m_nJob != nJob )
	{
		//직업이 바뀌엇다.
		m_nSelectedClassLevel = GetJobLevelByJob( nJob );		//선택된 잡 레벨 현재직업에 맞게
		m_nJob = nJob;
	}

	//현재 선택되어진 Job을 뽑아내서 출력해줘야 한다.
	int nSelectdJob = GetJobByJobLevel( m_nSelectedClassLevel );

	//선택한 클래스 이름 
	CWndStatic* lpWndStatic1 = (CWndStatic*) GetDlgItem( WIDC_STATIC_CLASSNAME );
	FLASSERT( lpWndStatic1 );

	lpWndStatic1->SetTitle( prj.m_aJob[nSelectdJob].szName );

	//class name wnd
	CWndStatic* lpWndStatic_C1 = (CWndStatic*) GetDlgItem( WIDC_STATIC_C1 );
	CWndStatic* lpWndStatic_C2 = (CWndStatic*) GetDlgItem( WIDC_STATIC_C2 );
	CWndStatic* lpWndStatic_C3 = (CWndStatic*) GetDlgItem( WIDC_STATIC_C3 );
	CWndStatic* lpWndStatic_C4 = (CWndStatic*) GetDlgItem( WIDC_STATIC_C4 );
	
	m_nMaxCurrJob = 0;
	CString strTex;

	//스킬트리 이미지 파일명 뽑아냄
	switch( nSelectdJob )
	{
	case JOB_VAGRANT:
		m_nMaxCurrJob  = 3;
		strTex = "ImgSkillVagrant.tga";
		break;			

	case JOB_KNIGHT:
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT_HERO:
		m_nMaxCurrJob  = 26;
		strTex = "Back_Night.TGA";
		break;			

	case JOB_BLADE:
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
		m_nMaxCurrJob  = 26;
		strTex = "Back_Blade.TGA";
		break;			

	case JOB_MERCENARY:
		m_nMaxCurrJob  = 18;
		strTex = "Back_Me.TGA";
		break;			

	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		m_nMaxCurrJob  = 28;
		strTex = "Back_Bill.TGA";
		break;			
	
	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		m_nMaxCurrJob  = 28;
		strTex = "Back_Ring.TGA";
		break;			

	case JOB_ASSIST:
		m_nMaxCurrJob  = 20;
		strTex = "Back_As.TGA";
		break;			

	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		m_nMaxCurrJob  = 39;
		strTex = "Back_Ele.TGA";
		break;			

	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		m_nMaxCurrJob  = 28;
		strTex = "Back_Psy.TGA";
		break;			

	case JOB_MAGICIAN:
		m_nMaxCurrJob  = 20;
		strTex = "Back_Ma.TGA";
		break;	

	case JOB_ACROBAT:
		m_nMaxCurrJob  = 20;
		strTex = "Back_Acr.tga";
		break;	

	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
		m_nMaxCurrJob  = 20;
		strTex = "Back_Jst.TGA";
		break;	

	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
		m_nMaxCurrJob  = 20;
		strTex = "Back_Rag.TGA";
		break;

	//3차 직업
	//gmpbigsun : 아무 tga로 넣어서 테스트 할것
	case JOB_LORDTEMPLER_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeLord.tga";
		break;

	case JOB_STORMBLADE_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeStormb.tga";
		break;

	case JOB_WINDLURKER_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeWindI.tga";
		break;

	case JOB_CRACKSHOOTER_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeCracks.tga";
		break;

	case JOB_FLORIST_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeFlor.tga";
		break;

	case JOB_FORCEMASTER_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeForcm.tga";
		break;

	case JOB_MENTALIST_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeMent.tga";
		break;

	case JOB_ELEMENTORLORD_HERO:
		m_nMaxCurrJob  = 0;
		strTex = "SkillTreeElel.tga";
		break;

	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "알수없는 직업 : %d" ), m_nJob );
		return;
	}

	
	//클래스 이름 채움
	switch( m_nJob )
	{
	case JOB_VAGRANT:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( "CLOSED" );
		lpWndStatic_C3->SetTitle( "CLOSED" );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_KNIGHT:
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_BLADE:
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_MERCENARY:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic_C3->SetTitle( "CLOSED" );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			
	
	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_ASSIST:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic_C3->SetTitle( "CLOSED" );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;			

	case JOB_MAGICIAN:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic_C3->SetTitle( "CLOSED" );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;	

	case JOB_ACROBAT:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic_C3->SetTitle( "CLOSED" );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;	

	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;	

	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		lpWndStatic_C4->SetTitle( "CLOSED" );
		break;

	case JOB_LORDTEMPLER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_KNIGHT].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_LORDTEMPLER_HERO].szName );
		break;

	case JOB_STORMBLADE_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MERCENARY].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_BLADE].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_STORMBLADE_HERO].szName );
		break;

	case JOB_WINDLURKER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_JESTER].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_WINDLURKER_HERO].szName );
		break;

	case JOB_CRACKSHOOTER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ACROBAT].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_RANGER].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_CRACKSHOOTER_HERO].szName );
		break;

	case JOB_FLORIST_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_RINGMASTER].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_FLORIST_HERO].szName );
		break;

	case JOB_FORCEMASTER_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_ASSIST].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_BILLPOSTER].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_FORCEMASTER_HERO].szName );
		break;

	case JOB_MENTALIST_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_PSYCHIKEEPER].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_MENTALIST_HERO].szName );
		break;

	case JOB_ELEMENTORLORD_HERO:
		lpWndStatic_C1->SetTitle( prj.m_aJob[JOB_VAGRANT].szName );
		lpWndStatic_C2->SetTitle( prj.m_aJob[JOB_MAGICIAN].szName );
		lpWndStatic_C3->SetTitle( prj.m_aJob[JOB_ELEMENTOR].szName );
		lpWndStatic_C4->SetTitle( prj.m_aJob[JOB_ELEMENTORLORD_HERO].szName );
		break;

	default:
		FLERROR_LOG( PROGRAM_NAME, _T( "알수없는 직업 : %d" ), m_nJob );
		return;
	}
	
	m_strHeroSkilBg.Empty();
	//Master Skill은 시작부터 1Lv이므로 배경 이미지 제외. ( 스킬 아이콘 이미지다... )
	switch( nSelectdJob )
	{
		case JOB_KNIGHT_HERO:
		case JOB_LORDTEMPLER_HERO:
			m_strHeroSkilBg = "Back_Hero_KntDrawing.tga";
			break;
		case JOB_BLADE_HERO:
		case JOB_STORMBLADE_HERO:
			m_strHeroSkilBg = "Back_Hero_BldDefence.tga";
			break;
		case JOB_BILLPOSTER_HERO:
		case JOB_FORCEMASTER_HERO:
			m_strHeroSkilBg = "Back_Hero_BilDisEnchant.tga";
			break;
		case JOB_RINGMASTER_HERO:
		case JOB_FLORIST_HERO:
			m_strHeroSkilBg = "Back_Hero_RigReturn.tga";
			break;
		case JOB_ELEMENTOR_HERO:
		case JOB_ELEMENTORLORD_HERO:
			m_strHeroSkilBg = "Back_Hero_EleCursemind.tga";
			break;
		case JOB_PSYCHIKEEPER_HERO:
		case JOB_MENTALIST_HERO:
			m_strHeroSkilBg = "Back_Hero_PsyStone.tga";
			break;
		case JOB_JESTER_HERO:
		case JOB_WINDLURKER_HERO:
			m_strHeroSkilBg = "Back_Hero_JstSilence.tga";
			break;
		case JOB_RANGER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			m_strHeroSkilBg = "Back_Hero_RagHawkeye.tga";
			break;
	}

	SAFE_DELETE( m_pTexJobPannel );

	if( !strTex.IsEmpty() )
	{
		m_pTexJobPannel = new IMAGE;
		if( LoadImage( MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), strTex ), m_pTexJobPannel ) == FALSE )
			FLERROR_LOG( PROGRAM_NAME, _T( "%s Open1 실패" ), strTex );

	}
	
	AdjustWndBase();
	
	//아이콘 읽기
	LoadTextureSkillIcon();

	//클래스 버튼 설정 
	AutoControlClassBtn();

	//선택된 스킬 -1이면 선택없음( 빈슬롯 )
	g_nSkillCurSelect = -1;

	m_pFocusItem = NULL;
	m_dwMouseSkill = NULL_ID;
}
//-----------------------------------------------------------------------------
void CWndSkill_16::LoadTextureSkillIcon()
{
	if( m_apSkill == NULL )
		return;

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL lpSkill;
		lpSkill = &m_apSkill[ i ];
		if( lpSkill->dwSkill != NULL_ID )
		{
			SkillProp* pSkillProp = prj.m_aPropSkill.GetAt( lpSkill->dwSkill );
			if( pSkillProp )
				m_pTexSkill[ i ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
		}
	}

	//1 2 3 4 ~~max 아이콘 불러오기 
	m_kTexLevel.DeleteDeviceObjects( );
	m_kTexLevel.LoadScript( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, _T( "icon_IconSkillLevel.inc" ) ) );
}
//-----------------------------------------------------------------------------
BOOL CWndSkill_16::CheckToLearnSkill( int nSkill )
{
	//이 스킬을 배울수 있는가?

	LPSKILL pSkill = &m_apSkill[ nSkill ];

	if( pSkill->dwSkill == NULL_ID || g_pPlayer == NULL ) 
		return FALSE;
	
	DWORD dwSkill = pSkill->dwSkill;
	
	SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );

	if( pSkillProp == NULL )
		return FALSE;

	// 9차 전승시스템	Neuz, World, Trans
	if( g_pPlayer->GetLevel() < (int)( pSkillProp->dwReqDisLV ) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() )
		return FALSE;
				
	if( pSkillProp->dwReqSkill1 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReqSkill1 );

		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel1 )
			{
				return FALSE;
			}
		}
		else
		{
		}
	}
	
	if( pSkillProp->dwReqSkill2 != NULL_ID )
	{
		LPSKILL pSkillBuf = GetdwSkill( pSkillProp->dwReqSkill2 );
		if( pSkillBuf )
		{
			if( pSkillBuf->dwLevel < pSkillProp->dwReqSkillLevel2 )
			{
				return FALSE;
			}				
		}
		else
		{
		}
	}	
	
	return TRUE;
}
//-----------------------------------------------------------------------------
SKILL* CWndSkill_16::GetdwSkill( DWORD dwSkill )
{
	for( int i = 0; i < MAX_SKILL_JOB; ++i )	
	{
		if( m_apSkill[i].dwSkill == dwSkill )
			return &m_apSkill[i];
	}
	return NULL;
}
//-----------------------------------------------------------------------------
BOOL CWndSkill_16::GetSkillIconRect(DWORD dwSkillID, CRect& rect )				
{
	SkillProp* pPropSkill = prj.GetSkillProp( dwSkillID );
	if( pPropSkill )
	{
		int nRectX = 0, nRectY = 0, nJobKind = MAX_JOBBASE;
		int nLegendSkill = 0;
                
		switch(dwSkillID)
		{
			case SKILL_INDEX( 1, SI_VAG_ONE_CLEANHIT ): nRectX = 94, nRectY = 149, nJobKind = MAX_JOBBASE; break;
			case SKILL_INDEX( 2, SI_VAG_ONE_BRANDISH ): nRectX = 144, nRectY = 149, nJobKind = MAX_JOBBASE; break;
			case SKILL_INDEX( 3, SI_VAG_ONE_OVERCUTTER ): nRectX = 194, nRectY = 149, nJobKind = MAX_JOBBASE; break;
	
	//머서너리
			case SKILL_INDEX( 4, SI_MER_ONE_SPLMASH ):				nRectX =  34, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 9, SI_MER_SHIELD_PROTECTION ):			nRectX =  34, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 5, SI_MER_ONE_KEENWHEEL ):				nRectX =  84, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 112, SI_MER_ONE_BLOODYSTRIKE ):			nRectX =  84, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 10, SI_MER_SHIELD_PANBARRIER ):			nRectX =  84, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 111, SI_MER_SUP_IMPOWERWEAPON ):			nRectX =  84, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 6, SI_MER_ONE_BLINDSIDE ):				nRectX = 134, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 14, SI_MER_ONE_REFLEXHIT ):				nRectX = 134, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 13, SI_MER_ONE_SNEAKER ):				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 109, SI_MER_SUP_SMITEAXE ):				nRectX = 134, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 108, SI_MER_SUP_BLAZINGSWORD ):			nRectX = 134, nRectY = 201, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 11, SI_MER_ONE_SPECIALHIT ):				nRectX = 184, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 12, SI_MER_ONE_GUILOTINE ):				nRectX = 184, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 8, SI_MER_SUP_AXEMASTER ):				nRectX = 184, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 7, SI_MER_SUP_SWORDMASTER ):			nRectX = 184, nRectY = 201, nJobKind = MAX_EXPERT; break;
	// 나이트
			case SKILL_INDEX( 133, SI_KNT_TWOSW_CHARGE ):				nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 134, SI_KNT_TWOAX_PAINDEALER ):			nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 128, SI_KNT_SUP_GUARD ):					nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 132, SI_KNT_TWOSW_EARTHDIVIDER ):			nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 135, SI_KNT_TWOAX_POWERSTUMP ):			nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 130, SI_KNT_SUP_RAGE ):					nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 131, SI_KNT_TWO_POWERSWING ):				nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 129, SI_KNT_SUP_PAINREFLECTION ):			nRectX = 134, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
	// 블레이드
			case SKILL_INDEX( 138, SI_BLD_DOUBLESW_SILENTSTRIKE ):		nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 140, SI_BLD_DOUBLEAX_SPRINGATTACK ):		nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 137, SI_BLD_DOUBLE_ARMORPENETRATE ):		nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 139, SI_BLD_DOUBLESW_BLADEDANCE ):		nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 141, SI_BLD_DOUBLEAX_HAWKATTACK ):		nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 143, SI_BLD_SUP_BERSERK ):				nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 136, SI_BLD_DOUBLE_CROSSSTRIKE ):			nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 142, SI_BLD_DOUBLE_SONICBLADE ):			nRectX = 134, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
				
	// 어시스트	
			case SKILL_INDEX( 44, SI_ASS_HEAL_HEALING ):				nRectX =  18, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 46, SI_ASS_HEAL_PATIENCE ):				nRectX =  64, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 114, SI_ASS_CHEER_QUICKSTEP ):			nRectX =  64, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 52, SI_ASS_CHEER_MENTALSIGN ):			nRectX =  64, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 104, SI_ASS_KNU_TAMPINGHOLE ):			nRectX =  64, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 45, SI_ASS_HEAL_RESURRECTION ):			nRectX = 110, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 20, SI_ASS_CHEER_HASTE ):				nRectX = 110, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 49, SI_ASS_CHEER_HEAPUP ):				nRectX = 110, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 113, SI_ASS_CHEER_STONEHAND ):			nRectX = 110, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 51, SI_ASS_CHEER_CIRCLEHEALING ):		nRectX = 156, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 115, SI_ASS_CHEER_CATSREFLEX ):			nRectX = 156, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 53, SI_ASS_CHEER_BEEFUP ):				nRectX = 156, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 105, SI_ASS_KNU_BURSTCRACK ):				nRectX = 156, nRectY = 187, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 48, SI_ASS_HEAL_PREVENTION ):			nRectX = 202, nRectY = 109, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 50, SI_ASS_CHEER_CANNONBALL ):			nRectX = 202, nRectY = 135, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 116, SI_ASS_CHEER_ACCURACY ):				nRectX = 202, nRectY = 161, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 117, SI_ASS_KNU_POWERFIST ):				nRectX = 202, nRectY = 187, nJobKind = MAX_EXPERT; break;
	//링마스터
			case SKILL_INDEX( 146, SI_RIN_SUP_PROTECT ):				nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 145, SI_RIN_SUP_HOLYCROSS ):				nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 149, SI_RIN_HEAL_GVURTIALLA ):			nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 147, SI_RIN_SUP_HOLYGUARD ):				nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 148, SI_RIN_SUP_SPIRITUREFORTUNE ):		nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 144, SI_RIN_HEAL_HEALRAIN ):				nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 150, SI_RIN_SQU_GEBURAHTIPHRETH ):		nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 151, SI_RIN_SUP_MERKABAHANZELRUSHA ):		nRectX = 134, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
	//빌포스터
			case SKILL_INDEX( 152, SI_BIL_KNU_BELIALSMESHING ):			nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 156, SI_BIL_PST_ASMODEUS ):				nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 154, SI_BIL_KNU_BLOODFIST ):				nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 157, SI_BIL_PST_BARAQIJALESNA ):			nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 153, SI_BIL_KNU_PIERCINGSERPENT ):		nRectX = 134, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 158, SI_BIL_PST_BGVURTIALBOLD ):			nRectX = 134, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 155, SI_BIL_KNU_SONICHAND ):				nRectX = 184, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 159, SI_BIL_PST_ASALRAALAIKUM ):			nRectX = 184, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
								
	// 매지션
			case SKILL_INDEX( 121, SI_MAG_MAG_MENTALSTRIKE ):			nRectX =  34, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 107, SI_MAG_MAG_BLINKPOOL ): 				nRectX =  34, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 64, SI_MAG_FIRE_BOOMERANG ):				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 69, SI_MAG_WIND_SWORDWIND ):				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 118, SI_MAG_WATER_ICEMISSILE ):			nRectX =  84, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 119, SI_MAG_ELECTRICITY_LIGHTINGBALL ):	nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 120, SI_MAG_EARTH_SPIKESTONE ):			nRectX =  84, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 65, SI_MAG_FIRE_HOTAIR ):				nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 70, SI_MAG_WIND_STRONGWIND ):			nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 32, SI_MAG_WATER_WATERBALL ):			nRectX = 134, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 34, SI_MAG_ELECTRICITY_LIGHTINGPARM ):	nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 36, SI_MAG_EARTH_ROCKCRASH ):			nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 30, SI_MAG_FIRE_FIRESTRIKE ):			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 31, SI_MAG_WIND_WINDCUTTER ):			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 33, SI_MAG_WATER_SPRINGWATER ):			nRectX = 184, nRectY = 148, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 35, SI_MAG_ELECTRICITY_LIGHTINGSHOCK ):	nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 37, SI_MAG_EARTH_LOOTING ):				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;
	// 엘리멘터
			case SKILL_INDEX( 168, SI_ELE_FIRE_FIREBIRD ):				nRectX =  34, nRectY = 234 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 174, SI_ELE_EARTH_STONESPEAR ):			nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 180, SI_ELE_WIND_VOID ):					nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 171, SI_ELE_ELECTRICITY_THUNDERSTRIKE ):	nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 177, SI_ELE_WATER_ICESHARK ):				nRectX =  34, nRectY = 338 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 170, SI_ELE_FIRE_BURINGFIELD ):			nRectX =  84, nRectY = 234 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 176, SI_ELE_EARTH_EARTHQUAKE ):			nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 182, SI_ELE_WIND_WINDFIELD ):				nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 173, SI_ELE_ELECTRICITY_ELETRICSHOCK ):	nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 179, SI_ELE_WATER_POISONCLOUD ):			nRectX =  84, nRectY = 338 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 183, SI_ELE_MULTY_METEOSHOWER ):			nRectX = 134, nRectY = 246 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 185, SI_ELE_MULTY_SANDSTORM ):			nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 184, SI_ELE_MULTY_LIGHTINGSTORM ):		nRectX = 134, nRectY = 298 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 186, SI_ELE_MULTY_AVALANCHE ):			nRectX = 134, nRectY = 338 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 169, SI_ELE_FIRE_FIREMASTER ):			nRectX = 184, nRectY = 234 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 175, SI_ELE_EARTH_EARTHMASTER ):			nRectX = 184, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 181, SI_ELE_WIND_WINDMASTER ):			nRectX = 184, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 172, SI_ELE_ELECTRICITY_LIGHTINGMASTER ):	nRectX = 184, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 178, SI_ELE_WATER_WATERMASTER ):			nRectX = 184, nRectY = 338 - 138, nJobKind = MAX_PROFESSIONAL; break;
	// 사이키퍼
			case SKILL_INDEX( 160, SI_PSY_NLG_DEMONOLGY ):				nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 162, SI_PSY_PSY_PSYCHICBOMB ):			nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 165, SI_PSY_NLG_CRUCIOSPELL ):			nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 161, SI_PSY_NLG_SATANOLGY ):				nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 164, SI_PSY_PSY_SPRITBOMB ):				nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 166, SI_PSY_PSY_MAXIMUMCRISIS ):			nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 163, SI_PSY_PSY_PSYCHICWALL ):			nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 167, SI_PSY_PSY_PSYCHICSQUARE ):			nRectX = 134, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
	////////////////////////////////////////////////////////////////////////////////////
	// 아크로벳
			case SKILL_INDEX( 206, SI_ACR_YOYO_PULLING ):				nRectX =  34, nRectY =  97, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 197, SI_ACR_SUP_SLOWSTEP ):				nRectX =  34, nRectY = 123, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 194, SI_ACR_BOW_JUNKBOW ):				nRectX =  34, nRectY = 175, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 195, SI_ACR_SUP_FASTWALKER ):				nRectX =  84, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 191, SI_ACR_SUP_YOYOMASTER ):				nRectX =  84, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 192, SI_ACR_SUP_BOWMASTER ): 				nRectX =  84, nRectY = 174, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 193, SI_ACR_SUP_DARKILLUSION ):			nRectX = 134, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 203, SI_ACR_YOYO_SNITCH ):				nRectX = 134, nRectY = 122, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 201, SI_ACR_YOYO_CROSSLINE ):				nRectX = 134, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SKILL_INDEX( 198, SI_ACR_BOW_SILENTSHOT ):				nRectX = 134, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SKILL_INDEX( 196, SI_ACR_BOW_AIMEDSHOT ):				nRectX = 134, nRectY = 200, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 199, SI_ACR_YOYO_ABSOLUTEBLOCK ):			nRectX = 184, nRectY =  96, nJobKind = MAX_EXPERT; break;
			case SKILL_INDEX( 205, SI_ACR_YOYO_DEADLYSWING ):			nRectX = 184, nRectY = 122, nJobKind = MAX_EXPERT; break;		
			case SKILL_INDEX( 204, SI_ACR_YOYO_COUNTER ):				nRectX = 184, nRectY = 149, nJobKind = MAX_EXPERT; break;		
			case SKILL_INDEX( 202, SI_ACR_BOW_AUTOSHOT ):				nRectX = 184, nRectY = 174, nJobKind = MAX_EXPERT; break;		
			case SKILL_INDEX( 200, SI_ACR_BOW_ARROWRAIN ):				nRectX = 184, nRectY = 200, nJobKind = MAX_EXPERT; break;		
	//제스터
			case SKILL_INDEX( 208, SI_JST_SUP_POISON ):					nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 209, SI_JST_SUP_BLEEDING ):				nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 213, SI_JST_YOYO_ESCAPE ):				nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 207, SI_JST_YOYO_CRITICALSWING ):			nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 211, SI_JST_YOYO_BACKSTAB ):				nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 210, SI_JST_SUP_ABSORB ):					nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 214, SI_JST_YOYO_VATALSTAB ):				nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 212, SI_JST_YOYO_HITOFPENYA ):			nRectX = 134, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
	//레인저
			case SKILL_INDEX( 216, SI_RAG_BOW_ICEARROW ):				nRectX =  34, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 217, SI_RAG_BOW_FLAMEARROW ):				nRectX =  34, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 219, SI_RAG_BOW_POISONARROW ):			nRectX =  34, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 215, SI_RAG_SUP_FASTATTACK ):				nRectX =  84, nRectY = 260 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 218, SI_RAG_BOW_PIRCINGARROW ):			nRectX =  84, nRectY = 286 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 221, SI_RAG_SUP_NATURE ):					nRectX =  84, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 222, SI_RAG_BOW_TRIPLESHOT ):				nRectX = 134, nRectY = 272 - 138, nJobKind = MAX_PROFESSIONAL; break;
			case SKILL_INDEX( 220, SI_RAG_BOW_SILENTARROW ):			nRectX = 134, nRectY = 312 - 138, nJobKind = MAX_PROFESSIONAL; break;
				
//sun: 9차 전승관련 Clienet
			//마스터 스킬
			case SKILL_INDEX( 309, SI_BLD_MASTER_ONEHANDMASTER ):		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 310, SI_KNT_MASTER_TWOHANDMASTER ):		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 311, SI_JST_MASTER_YOYOMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 312, SI_RAG_MASTER_BOWMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 313, SI_ELE_MASTER_INTMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 314, SI_PSY_MASTER_INTMASTER ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 315, SI_BIL_MASTER_KNUCKLEMASTER ):		nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			case SKILL_INDEX( 316, SI_RIG_MASTER_BLESSING ):			nLegendSkill = 0, nJobKind = MAX_MASTER; break;
			//영웅 스킬			
			case SKILL_INDEX( 237, SI_BLD_HERO_DEFFENCE ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 238, SI_KNT_HERO_DRAWING ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 239, SI_JST_HERO_SILENCE ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 240, SI_RAG_HERO_HAWKEYE ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 241, SI_ELE_HERO_CURSEMIND ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 242, SI_PSY_HERO_STONE ):					nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 243, SI_BIL_HERO_DISENCHANT ):			nLegendSkill = 1, nJobKind = MAX_HERO; break;
			case SKILL_INDEX( 244, SI_RIG_HERO_RETURN ):				nLegendSkill = 1, nJobKind = MAX_HERO; break;

			//gmpbigsun_WKG: 3차 스킬 추가 
			//로드 템플러
			case SKILL_INDEX( 400, SI_LOD_SUP_PULLING ):				nRectX = 34, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 401, SI_LOD_ONE_GRANDRAGE ):				nRectX = 34, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 404, SI_LOD_SUP_HOLYARMOR ):				nRectX = 34, nRectY = 174, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 405, SI_LOD_SUP_SCOPESTRIKE ):			nRectX = 84, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 403, SI_LOD_SUP_ANGER ):					nRectX = 84, nRectY = 174, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 402, SI_LOD_ONE_SHILDSTRIKE ):			nRectX = 134, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;

			//스톰 블레이드
			case SKILL_INDEX( 406, SI_STORM_DOUBLE_CROSSBLOOD ):		nRectX = 34, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 408, SI_STORM_SUP_POWERINCREASE ):		nRectX = 34, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 407, SI_STORM_DOUBLE_STORMBLAST ):		nRectX = 84, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 436, SI_STORM_DOUBLE_HOLDINGSTORM ):		nRectX = 84, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;

			//윈드러커
			case SKILL_INDEX( 410, SI_WIN_SUP_EVASIONINCREASE ):		nRectX = 34, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 409, SI_WIN_YOYO_MADHURRICANE ):			nRectX = 34, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 411, SI_WIN_SUP_CONTROLINCREASE ):		nRectX = 84, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 412, SI_WIN_YOYO_BACKSTEP ):				nRectX = 84, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;

			//크랙슈터
			case SKILL_INDEX( 414, SI_CRA_SUP_POWERINCREASE ):			nRectX = 34, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 413, SI_CRA_CRBOW_RANGESTRIKE ):			nRectX = 34, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 415, SI_CRA_SUP_CONTROLINCREASE ):		nRectX = 84, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 416, SI_CRA_SUP_HAWKEYE ):				nRectX = 134, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;

			//플로리스트
			case SKILL_INDEX( 417, SI_FLO_HEAL_DOT ):					nRectX = 34, nRectY = 122 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 418, SI_FLO_SQU_BLESSSTEP ):				nRectX = 84 - 4, nRectY = 96 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 419, SI_FLO_SQU_BLESSBODY ):				nRectX = 84 - 4, nRectY = 122 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 420, SI_FLO_SQU_BLESSARMOR ):				nRectX = 84 - 4, nRectY = 148 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 422, SI_FLO_CHEER_PATTERS ):				nRectX = 84 - 4, nRectY = 174 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 421, SI_FLO_SUP_ABSOLUTE ):				nRectX = 134 - 8, nRectY = 174 + 13, nJobKind = MAX_LEGEND_HERO; break;

			//포스마스터
			case SKILL_INDEX( 423, SI_FOR_SQU_FORCETENACITY ):			nRectX = 34, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 424, SI_FOR_SQU_FORCEANGER ):				nRectX = 84, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 425, SI_FOR_SQU_FORCESPEED ):				nRectX = 34, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 426, SI_FOR_SQU_FORCEMAD ):				nRectX = 84, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;

			//멘탈리스트
			case SKILL_INDEX( 427, SI_MEN_WAN_FEARSCREAM ):				nRectX = 34 - 0, nRectY = 122 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 429, SI_MEN_WAN_ATKDECREASE ):			nRectX = 84 - 4, nRectY = 96 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 430, SI_MEN_WAN_DEFDECREASE ):			nRectX = 84 - 4, nRectY = 122 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 431, SI_MEN_WAN_SPEDECREASE ):			nRectX = 84 - 4, nRectY = 148 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 428, SI_MEN_WAN_DARKNESSLAKE ):			nRectX = 84 - 4, nRectY = 174 + 13, nJobKind = MAX_LEGEND_HERO; break;
													
			//엘리멘탈로드
			case SKILL_INDEX( 433, SI_ELE_STF_FINALSPEAR ):				nRectX = 34, nRectY = 122, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 434, SI_ELE_STF_COSMICELEMENT ):			nRectX = 34, nRectY = 148, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 432, SI_ELE_STF_THUNDERBOLT ):			nRectX = 84, nRectY = 122 + 13, nJobKind = MAX_LEGEND_HERO; break;
			case SKILL_INDEX( 435, SI_ELE_STF_SLIPPING ):				nRectX = 84, nRectY = 148 + 26, nJobKind = MAX_LEGEND_HERO; break;

			default:	
				return FALSE;
		}

		LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC_PANNEL );
		FLASSERT( lpWndCtrl );

		int iGapX = lpWndCtrl->rect.left;
		int iGapY = lpWndCtrl->rect.top;

		//현재 선택 직업레벨이 아니면 해당사항 없음
		if( m_nSelectedClassLevel != nJobKind )
		{
			iGapX = 10000;
			iGapY = 10000;
		}

		//음....
		iGapX -= 13;
		iGapY -= 88;
	
		if( nJobKind == MAX_JOBBASE )
			rect = CRect( nRectX+iGapY, nRectY+iGapY, nRectX+24+iGapX, nRectY+24+iGapY );
		else if( nJobKind == MAX_EXPERT )
			rect = CRect(  nRectX+iGapX ,  nRectY+iGapY, nRectX+24+iGapX, nRectY+24+iGapY );
		else if( nJobKind == MAX_PROFESSIONAL )
			rect = CRect(  nRectX+iGapX,  nRectY+iGapY, nRectX+24+iGapX, nRectY+24+iGapY );
		else if( nJobKind == MAX_MASTER || nJobKind == MAX_HERO )
		{
			LPWNDCTRL lpWndCtrl = NULL; 

			//오른쪽 마스터, 히어로 슬롯 2개
			if(nLegendSkill == 0)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM2 );
			else if(nLegendSkill == 1)
				lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );

			rect = lpWndCtrl->rect;
		}
		else if( nJobKind == MAX_LEGEND_HERO )
		{
			rect = CRect( nRectX+iGapX, nRectY+iGapY, nRectX+24+iGapX, nRectY+24+iGapY );
		}

	}

	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnDraw(C2DRender* p2DRender)
{
	if( m_apSkill == NULL )
		return;

	CPoint pt(0,0);
	LPWNDCTRL lpWndCtrl = NULL;

	if( m_bLegend )
	{
		if( !m_strHeroSkilBg.IsEmpty() )
		{
			CTexture* pTexture = NULL;
			CRect rect;
			CPoint point;
			LPWNDCTRL lpWndCtrl = NULL;
			
			//히어로 슬롯
			lpWndCtrl = GetWndCtrl( WIDC_CUSTOM3 );
			rect = lpWndCtrl->rect;
		
			point = rect.TopLeft();
			point.y -= 2;
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),m_strHeroSkilBg), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender, point, CPoint( 27, 27 ) );				
		}
	}

	for( int i = 0; i < MAX_SKILL_JOB; ++i ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;

		DWORD dwSkill = pSkill->dwSkill;

		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
		if( !pSkillProp )
			FLERROR_LOG( PROGRAM_NAME, _T( "pSkillProp is NULL, dwSkill %d" ), dwSkill );

		//선택된 스킬의 백그라운드 출력
		BOOL bDrawSelectedIcon = FALSE;		
		if( g_nSkillCurSelect == i )
		{
			CRect backRect; 
			if( GetSkillIconRect( pSkillProp->dwID, backRect ) )
				m_pTexSeletionBack->Render( p2DRender, backRect.TopLeft() - CPoint( 4, 4 ), CPoint( 31, 31 ) );

			//만일 선택된 스킬이 아직 배우지 않았지만 배울수 있는 경우는 아이콘 출력
			if( CheckToLearnSkill( i ) && pSkill->dwLevel == 0 )
				bDrawSelectedIcon = TRUE;
		}
	
		if( pSkillProp && ( dwSkill != NULL_ID || bDrawSelectedIcon ) )
		{
			// 스킬 아이콘 출력 
			if( m_pTexSkill[ i ] && ( CheckToLearnSkill( i ) || g_pPlayer->isLearnSkill( i ) ) && ( 0 < pSkill->dwLevel || bDrawSelectedIcon ) )
			{
				CRect rect;
				if( GetSkillIconRect( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					
					m_pTexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill( pSkill->dwSkill );
					if( pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel )
						nAddNum = 20;

					//스킬 레벨 찍자
					if( pSkill->dwLevel != 0 )
					{
						if( pSkill->dwLevel < pSkillProp->dwExpertMax )
							m_kTexLevel.Render( p2DRender, rect.TopLeft()-CPoint(2,2), pSkill->dwLevel-1 + nAddNum );
						else
							m_kTexLevel.Render( p2DRender, rect.TopLeft()-CPoint(2,2), 19 + nAddNum );
					}
				}
			}	
			else if( m_dwMouseSkill == dwSkill && m_pTexSkill[ i ] && ( CheckToLearnSkill( i ) || g_pPlayer->isLearnSkill( i ) ) )
			{
				CRect rect;
				if( GetSkillIconRect( pSkillProp->dwID, rect ) )
				{
					rect.top -= 2;
					m_pTexSkill[ i ]->Render( p2DRender, rect.TopLeft(), CPoint( 27, 27 ) );				
				}
			}
		}
	}

	//남은 스킬 포인트
	CWndStatic* lpWndStaticSP = (CWndStatic*)GetDlgItem( WIDC_STATIC_SP );
	CString strTemp;
	strTemp.Format( "%d", m_nCurrSkillPoint );
	lpWndStaticSP->SetTitle( strTemp );

	// 선택된 스킬이 있을시 위 창에 출력
	if( m_pFocusItem && g_nSkillCurSelect > -1 )
	{
		if( m_pFocusItem->dwSkill == -1 )
		{
			CString str;
			str.Format( "스킬쓴넘 직업 %d\n스킬 인덱스가 %d인값이 나옴 : 레벨 %d, CurSelect %d", g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, g_nSkillCurSelect );
			FLERROR_LOG( PROGRAM_NAME, str );
		}

		//선택한 스킬 표현슬롯
		lpWndCtrl = GetWndCtrl( WIDC_CUSTOM1 );
		m_pTexSkill[ g_nSkillCurSelect ]->Render( p2DRender, lpWndCtrl->rect.TopLeft() - CPoint( 0, 2 ), CPoint( 36, 36 ) );

		SkillProp* pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if( pSkillProp && 0 < m_pFocusItem->dwLevel )
		{
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
			if( pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel )
				nAddNum = 20;

			//스킬당 레벨 표시
			if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax )
				m_kTexLevel.Render( p2DRender, lpWndCtrl->rect.TopLeft(), m_pFocusItem->dwLevel-1 + nAddNum );
			else
				m_kTexLevel.Render( p2DRender, lpWndCtrl->rect.TopLeft(), 19 + nAddNum );
		}

		//선택한 스킬의 필요sp 출력
		CWndStatic* lpWndStaticSP = (CWndStatic*)GetDlgItem( WIDC_STATIC_NEEDSP );
		DWORD dwNeedSp = prj.GetSkillPoint( m_pFocusItem->GetProp() );
		strTemp.Format( "%d", dwNeedSp );
		lpWndStaticSP->SetTitle( strTemp );

	}
}

BOOL CWndSkill_16::Process()
{
	if( m_apSkill == NULL || g_pPlayer == NULL)
		return FALSE;

	if( !m_bLegend )
	{
		if(g_pPlayer->GetLegendChar() >= LEGEND_CLASS_MASTER)
		{
			m_bLegend = TRUE;
			return TRUE;
		}
	}
	else
	{
		if(g_pPlayer->GetLegendChar() < LEGEND_CLASS_MASTER)
		{
			m_bLegend = TRUE;
			return TRUE;
		}
	}

	m_pWndButton[0]->EnableWindow( FALSE );			//plus
	m_pWndButton[1]->EnableWindow( FALSE );			//minus
	m_pWndButton[2]->EnableWindow( FALSE );			//ok
	m_pWndButton[3]->EnableWindow( FALSE );			//cancel

	
	if( m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint )
	{
		LPSKILL lpSkillUser = g_pPlayer->GetSkill( m_pFocusItem->dwSkill );
		SkillProp* pSkillProp = m_pFocusItem->GetProp();
		if( pSkillProp == NULL || lpSkillUser == NULL )
			return TRUE;
		
		int nPoint = prj.GetSkillPoint( pSkillProp );
		if( m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint )
			m_pWndButton[0]->EnableWindow( TRUE );	

		if( m_pFocusItem->dwLevel != lpSkillUser->dwLevel )
			m_pWndButton[1]->EnableWindow( TRUE );	

		if( m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint )
		{
			m_pWndButton[2]->EnableWindow( TRUE );
			m_pWndButton[3]->EnableWindow( TRUE );
		}

		//전승 스킬의 레벨업은 NPC를 통해서 하기 때문에 막자.
		pSkillProp = prj.GetSkillProp( m_pFocusItem->dwSkill );
		if(pSkillProp->dwSkillKind1 == JTYPE_MASTER || pSkillProp->dwSkillKind1 == JTYPE_HERO)
		{
			m_pWndButton[0]->EnableWindow( FALSE );
			m_pWndButton[1]->EnableWindow( FALSE );
			m_pWndButton[2]->EnableWindow( FALSE );
			m_pWndButton[3]->EnableWindow( FALSE );
		}
	}

	return CWndBase::Process();
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnMouseWndSurface( CPoint point )
{
	DWORD dwMouseSkill = NULL_ID;

	//포커스 슬롯 처리
	if( m_pFocusItem )
	{
		LPWNDCTRL lpWndCtrl1 = GetWndCtrl( WIDC_CUSTOM1 );  //포커스 슬롯
		if( lpWndCtrl1 )
		{
			CRect rectFocus = lpWndCtrl1->rect;
			if( rectFocus.PtInRect( point ) )
			{
				dwMouseSkill = m_pFocusItem->dwSkill;

				ClientToScreen( &point );
				ClientToScreen( &rectFocus );
				g_WndMng.PutToolTip_Skill( m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, point, &rectFocus, TRUE );
				
				m_dwMouseSkill = dwMouseSkill;
				return; //포커스 슬롯에 마우스가 온경우는 해당 스킬의 도움말을 보여주고 끝임. 트리쪽 검사 불필요
			}
		}
	}

	//스킬 트리 처리
	for( int i = 0; i < MAX_SKILL_JOB; i++ )
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL )
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );

		if( dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillIconRect( pSkillProp->dwID, rect ) )
			{
				rect.top  -= 2;
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;
			
				if( rect.PtInRect(point) )
				{
					dwMouseSkill = dwSkill;

					ClientToScreen( &point );
					ClientToScreen( &rect );
					g_WndMng.PutToolTip_Skill( dwSkill, pSkill->dwLevel, point, &rect, ( CheckToLearnSkill( i ) || g_pPlayer->isLearnSkill( i ) ) );
					break;
				}
			}
		}
	}

	m_dwMouseSkill = dwMouseSkill;	
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	CPoint ptMouse = GetMousePoint();

	for( int i = 0; i < MAX_SKILL_JOB; i++ ) 
	{
		LPSKILL pSkill = GetSkill( i );
		if( pSkill == NULL ) 
			continue;
		DWORD dwSkill = pSkill->dwSkill;

		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
//		if( pSkillProp )
//		{
//			GetCalcImagePos( pSkillProp->dwItemKind1 );
//		}

		if( pSkillProp && dwSkill != NULL_ID )
		{
			CRect rect;
			if( GetSkillIconRect( pSkillProp->dwID, rect ) && ( CheckToLearnSkill( i ) || g_pPlayer->isLearnSkill( i ) ) )
			{
				rect.top  -= 2;
				
				rect.right  = rect.left+27;
				rect.bottom = rect.top+27;

			//	rect.OffsetRect( 0, m_nTopDownGap );
				
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
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}
//-----------------------------------------------------------------------------
void CWndSkill_16::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_apSkill == NULL )
		return;

	if( m_bDrag == FALSE )
		return;
	
	CPoint pt( 3, 3 );
	CRect rect;
	if( g_pPlayer->isLearnSkill( g_nSkillCurSelect ) )
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[ g_nSkillCurSelect ].dwSkill;
		SkillProp* pSkillProp = prj.GetSkillProp( dwSkill );
		
		m_GlobalShortcut.m_pFromWnd    = this;
		m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType  = 0;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId       = g_nSkillCurSelect;				// 컬런트 셀렉트가 곧 ID나 마찬가지임.
		m_GlobalShortcut.m_pTexture = m_pTexSkill[ g_nSkillCurSelect ];	//L;//pItemElem->m_pTexture;
		FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pSkillProp->szName);
	}
}
//-----------------------------------------------------------------------------
LPSKILL CWndSkill_16::GetSkill( int i )
{
	LPSKILL pSkill = &m_apSkill[ i ];
	FLERROR_ASSERT_LOG_RETURN( pSkill, NULL, _T("[윈도우 스킬] 스킬 포인터가 유효하지 않습니다.") );	

	if( pSkill->dwSkill == NULL_ID ) 
		return NULL;
	if( pSkill->GetProp() == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Not Prop : %d" ), i );
		return NULL;
	}
	
	return pSkill;
}
//-----------------------------------------------------------------------------
BOOL CWndSkill_16::CanDownPoint( DWORD dwSkill )
{
	LPSKILL pSkill = GetdwSkill( dwSkill );
	if( pSkill == NULL )
		return FALSE;

	LPSKILL pMySkill = g_pPlayer->GetSkill( pSkill->dwSkill );
	if( pMySkill == NULL )
		return FALSE;

	if( pSkill->dwLevel <= pMySkill->dwLevel )
		return FALSE;

	return TRUE;
}
//-----------------------------------------------------------------------------
void CWndSkill_16::SubSkillPointDown( LPSKILL lpSkill )
{
	//이 스킬이 내가 가지고 있는 스킬의 선행스킬일 경우, 선행스킬의 레벨이 필요량보다 적으면 ( 마이너스 시키다 보면 ) 내스킬 자동 취소시킴

	for( int i = 0; i < MAX_SKILL_JOB; i++ )	
	{
		if( m_apSkill[i].dwSkill == NULL_ID )
			continue;
		
		LPSKILL pSkill2 = &m_apSkill[i];
		
		if( pSkill2 == NULL )
			continue;
		
		
		if( pSkill2->GetProp()->dwReqSkill1 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReqSkillLevel1 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReqSkillLevel1 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
		
		if( pSkill2->GetProp()->dwReqSkill2 == lpSkill->dwSkill )
		{
			if( pSkill2->GetProp()->dwReqSkillLevel2 )
			{
				if( lpSkill->dwLevel < pSkill2->GetProp()->dwReqSkillLevel2 )
				{
					if( pSkill2->dwLevel > 0 )
					{
						m_nCurrSkillPoint += ( prj.GetSkillPoint( pSkill2->GetProp() ) * pSkill2->dwLevel );
						pSkill2->dwLevel = 0;
						SubSkillPointDown( pSkill2 );
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
void CWndSkill_16::SerializeRegInfo ( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
}
//-----------------------------------------------------------------------------
void CWndSkill_16::AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat )
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl = NULL;
	
	lpWndCtrl = GetWndCtrl( WIDC_STATIC_PANNEL );

	pt = lpWndCtrl->rect.TopLeft();
	pt.y += 26;

	if( m_pTexJobPannel ) 
		PaintTexture( pDest, m_pTexJobPannel, pt, size );
}
//-----------------------------------------------------------------------------
int CWndSkill_16::GetJobByJobLevel( int jobLevel )
{
	if( MAX_JOBBASE == jobLevel )
		return JOB_VAGRANT;
	else if( MAX_EXPERT == jobLevel )
	{
		//1차 직업군중에 머냐고 물었는데 방랑자임
		if( JOB_VAGRANT == m_nJob )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "jobLevel %d, m_nJob %d" ), jobLevel, m_nJob );
			return -1;
		}

		if( MAX_EXPERT > m_nJob ) 
		{
			//현재 1차 직업군임, 이경우는 걍 m_nJob이 1단계 직업
			return m_nJob;
		}

		switch( m_nJob )
		{
		case JOB_KNIGHT:
		case JOB_KNIGHT_MASTER:
		case JOB_KNIGHT_HERO:
		case JOB_LORDTEMPLER_HERO:

		case JOB_BLADE:
		case JOB_BLADE_MASTER:
		case JOB_BLADE_HERO:
		case JOB_STORMBLADE_HERO:
			return JOB_MERCENARY;

		case JOB_JESTER:
		case JOB_JESTER_MASTER:
		case JOB_JESTER_HERO:
		case JOB_WINDLURKER_HERO:

		case JOB_RANGER:
		case JOB_RANGER_MASTER:
		case JOB_RANGER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			return JOB_ACROBAT;

		case JOB_RINGMASTER:
		case JOB_RINGMASTER_MASTER:
		case JOB_RINGMASTER_HERO:
		case JOB_FLORIST_HERO:

		case JOB_BILLPOSTER:
		case JOB_BILLPOSTER_MASTER:
		case JOB_BILLPOSTER_HERO:
		case JOB_FORCEMASTER_HERO:
			return JOB_ASSIST;

		case JOB_PSYCHIKEEPER:
		case JOB_PSYCHIKEEPER_MASTER:
		case JOB_PSYCHIKEEPER_HERO:
		case JOB_MENTALIST_HERO:

		case JOB_ELEMENTOR:
		case JOB_ELEMENTOR_MASTER:
		case JOB_ELEMENTOR_HERO:
		case JOB_ELEMENTORLORD_HERO:
			return JOB_MAGICIAN;

		default:
			FLERROR_LOG( PROGRAM_NAME, _T( "m_nJob error" ) );
			return -1;
			break;
			
		}
	}
	else if( MAX_PROFESSIONAL == jobLevel )
	{
		if( MAX_EXPERT > m_nJob )
		{
			//2차 직업군중에 머냐고 물었는데 현재 직업이 1차이하다.
			FLERROR_LOG( PROGRAM_NAME, _T( "joblevel %d, m_nJob %d" ), jobLevel, m_nJob );
			return -1;
		}
		else if( MAX_HERO > m_nJob )
		{
			//2차 직업군이 머냐고 물었는데 현재 2차직업이다. 이경우는 걍 m_nJob ( 전승 포함 )
			return m_nJob;
		}
		else if( MAX_LEGEND_HERO > m_nJob )
		{
			switch( m_nJob )
			{
			case JOB_LORDTEMPLER_HERO:
				return JOB_KNIGHT;

			case JOB_STORMBLADE_HERO:
				return JOB_BLADE;

			case JOB_WINDLURKER_HERO:
				return JOB_JESTER;

			case JOB_CRACKSHOOTER_HERO:
				return JOB_RANGER;

			case JOB_FLORIST_HERO:
				return JOB_RINGMASTER;

			case JOB_FORCEMASTER_HERO:
				return JOB_BILLPOSTER;

			case JOB_MENTALIST_HERO:
				return JOB_PSYCHIKEEPER;

			case JOB_ELEMENTORLORD_HERO:
				return JOB_ELEMENTOR;
			}
		}
	}
	else if ( MAX_LEGEND_HERO == jobLevel )
	{
		//3차 직업군을 요청했는데 2차 직업군이다.
		if( MAX_HERO > m_nJob )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "joblevel %d, m_nJob %d" ), jobLevel, m_nJob );
			return -1;
		}

		return m_nJob;
	}

	return -1;
}
//-----------------------------------------------------------------------------
void CWndSkill_16::AutoControlClassBtn( )
{
	int jobLevel = GetJobLevelByJob( m_nJob );

	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C1 );
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C2 );
	CWndButton* pWndButton3 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C3 );
	CWndButton* pWndButton4 = (CWndButton*)GetDlgItem( WIDC_BUTTON_C4 );

	FLASSERT( pWndButton1 && pWndButton2 && pWndButton3 && pWndButton4 );

	pWndButton1->EnableWindow( FALSE );
	pWndButton2->EnableWindow( FALSE );
	pWndButton3->EnableWindow( FALSE );
	pWndButton4->EnableWindow( FALSE );
	
	CString strBuffer;

	//뚫리지 않는 직업에 대해 기본이미지 적용
	switch( jobLevel )
	{
	case MAX_JOBBASE: pWndButton2->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "SlotVan.bmp" ) );
	case MAX_EXPERT:  pWndButton3->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"SlotVan.bmp" ) );
	case MAX_PROFESSIONAL: pWndButton4->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "SlotVan.bmp" ) );
	case MAX_LEGEND_HERO: break;
	default: break;
	}

	switch( jobLevel )
	{
	case MAX_LEGEND_HERO:
		pWndButton4->EnableWindow( TRUE );
		GetFileNameClassBtn( MAX_LEGEND_HERO, strBuffer );
		pWndButton4->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strBuffer.GetString() ) );
		//no break;

	case MAX_PROFESSIONAL:
		pWndButton3->EnableWindow( TRUE );
		GetFileNameClassBtn( MAX_PROFESSIONAL, strBuffer );
		pWndButton3->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), strBuffer.GetString() ) );
		//no break;

	case MAX_EXPERT:
		pWndButton2->EnableWindow( TRUE );
		GetFileNameClassBtn( MAX_EXPERT, strBuffer );
		pWndButton2->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),strBuffer.GetString() ) );
		//no break;

	case MAX_JOBBASE:
		pWndButton1->EnableWindow( TRUE );
		GetFileNameClassBtn( MAX_JOBBASE, strBuffer );
		pWndButton1->SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), strBuffer.GetString() ) );
		break;

	default: FLERROR_LOG( PROGRAM_NAME, _T( "CWndSkill_16::AutoControlClassBtn" ) ); break;
	}

	pWndButton1->SetCheck( 0 );
	pWndButton2->SetCheck( 0 );
	pWndButton3->SetCheck( 0 );
	pWndButton4->SetCheck( 0 );

	switch( m_nSelectedClassLevel )
	{
	case MAX_JOBBASE:		pWndButton1->SetCheck( 2 );	break;
	case MAX_EXPERT:		pWndButton2->SetCheck( 2 );	break;
	case MAX_PROFESSIONAL:	pWndButton3->SetCheck( 2 );	break;
	case MAX_LEGEND_HERO:	pWndButton4->SetCheck( 2 );	break;
	}

}

BOOL CWndSkill_16::GetFileNameClassBtn( const int nJobLevel, OUT CString& strBuffer )
{
	int nJob = GetJobByJobLevel( nJobLevel );
	if( -1 == nJob )
		return FALSE;

	switch( nJob )
	{
	//0차
	case JOB_VAGRANT:		strBuffer = "SlotVan.bmp";	break;
	//1차
	case JOB_MERCENARY:		strBuffer = "SlotMer.bmp";	break;
	case JOB_ACROBAT:		strBuffer = "SlotArc.bmp";	break;
	case JOB_ASSIST:		strBuffer = "SlotAs.bmp";	break;
	case JOB_MAGICIAN:		strBuffer = "SlotMag.bmp";	break;
	case JOB_PUPPETEER:		//없어진 직업
		return FALSE;		
	
	case JOB_KNIGHT:		
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT_HERO:
		strBuffer = "SlotKnight.bmp";	
		break;

	case JOB_BLADE:	
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO :
		strBuffer = "SlotBlad.bmp";		
		break;

	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
		strBuffer = "SlotJes.bmp";		
		break;

	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
		strBuffer = "SlotRan.bmp";		
		break;

	case JOB_RINGMASTER:	
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		strBuffer = "SlotRingm.bmp";	
		break;

	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		strBuffer = "SlotBillfor.bmp";	
		break;

	case JOB_PSYCHIKEEPER:	
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		strBuffer = "SlotPsy.bmp";		
		break;

	case JOB_ELEMENTOR:		
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		strBuffer = "SlotElem.bmp";		
		break;

	case JOB_GATEKEEPER:	//없어진 직업
		return FALSE;	
	case JOB_DOPPLER:		//없어진 직업
		return FALSE;

	case JOB_LORDTEMPLER_HERO:		strBuffer = "SlotLord.bmp";		break;
	case JOB_STORMBLADE_HERO:		strBuffer = "SlotStormb.bmp";	break;
	case JOB_WINDLURKER_HERO:		strBuffer = "SlotWindI.bmp";	break;
	case JOB_CRACKSHOOTER_HERO:		strBuffer = "SlotCracks.bmp";	break;
	case JOB_FLORIST_HERO:			strBuffer = "SlotFlor.bmp";		break;
	case JOB_FORCEMASTER_HERO:		strBuffer = "SlotForcem.bmp";	break;
	case JOB_MENTALIST_HERO:		strBuffer = "SlotMent.bmp";		break;
	case JOB_ELEMENTORLORD_HERO:	strBuffer = "SlotElel.bmp";		break;

	default: FLERROR_LOG( PROGRAM_NAME, _T( "unknown job" ) );  break;
	}

	return TRUE;
}
//-----------------------------------------------------------------------------
int	CWndSkill_16::GetJobLevelByJob( int nJob )
{
	if( MAX_JOBBASE > nJob )
		return MAX_JOBBASE;
	else if( MAX_EXPERT > nJob )
		return MAX_EXPERT;
	else if( MAX_HERO > nJob )
		return MAX_PROFESSIONAL;
	else if( MAX_LEGEND_HERO > nJob )
		return MAX_LEGEND_HERO;

	FLERROR_LOG( PROGRAM_NAME, _T( "job is %d" ), nJob );
	return -1;
}
//-----------------------------------------------------------------------------
HRESULT CWndSkill_16::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CWndSkill_16::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();

	return S_OK;
}
//-----------------------------------------------------------------------------
HRESULT CWndSkill_16::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	SAFE_DELETE( m_pTexJobPannel );
		
	if( g_pPlayer->m_nSkillPoint > 0 )
	{
		SAFE_DELETE_ARRAY( m_apSkill );
	}
	else
		m_apSkill = NULL;
	
	return S_OK;
}
//-----------------------------------------------------------------------------

CRenderStatDice::CRenderStatDice() : m_nResult( 0 ), m_nRenderIndex( 0 ), m_bViewResult( false ), m_bFinish( false )
{

}

CRenderStatDice::~CRenderStatDice()
{

}

void CRenderStatDice::Init( )
{
	for( int i=0; i<nTextureMax; ++i )
	{
		CString strTexture;

		strTexture.Format( "WndStatDiceRoulette_%.2d.tga", i );
		BOOL bResult = m_StatDiceTexture[i].LoadTexture( g_Neuz.GetDevice(), MakePath( "Theme\\Default\\", strTexture ), dwTextureKeyColor, TRUE );
		FLASSERT(bResult);
	}

	m_tmFlash.Set( 0 );
}

void CRenderStatDice::Release()
{
	for( int i=0; i < nTextureMax; ++i )
		m_StatDiceTexture[i].DeleteDeviceObjects();
}

void CRenderStatDice::SetTime( float fTime )
{
	m_tmFlash.Set( fTime );
}

void CRenderStatDice::Process( float fTime )
{
}

void CRenderStatDice::Render( C2DRender* p2DRender, CRect rect )
{
	if( m_tmFlash.IsTimeOut() )
	{
		m_nRenderIndex++;
		m_tmFlash.Reset();
	}

	if( m_nRenderIndex >= nTextureMax )
		m_nRenderIndex = 0;

	if( m_bViewResult )
	{
		if( m_nResult < 0 ) {
			m_nRenderIndex = 6;
			m_bFinish = true;
		}
		else
		if( m_nResult > 0 ) {
			m_nRenderIndex = 20;
			m_bFinish = true;
		}
	}

	CPoint cpt = rect.TopLeft();
	cpt.x += static_cast<LONG>( rect.Width() * 0.5f );
 	cpt.x -= static_cast<LONG>( m_StatDiceTexture[m_nRenderIndex].m_size.cx * 0.5f );
 	cpt.y += static_cast<LONG>( m_StatDiceTexture[m_nRenderIndex].m_size.cy * 0.7f );
	m_StatDiceTexture[m_nRenderIndex].Render( p2DRender, cpt );

	if( m_bFinish )
	{
	}
}

void CRenderStatDice::ViewResult( bool bFlag )
{
	m_bViewResult = bFlag;
}

CWndStatDice::CWndStatDice() 
{
	m_bUseCoupon = FALSE;
	m_checkClose = TRUE;
	m_TargetWndId = -1;
	m_nResultCount = 0;
}

CWndStatDice::~CWndStatDice() 
{ 
} 

void CWndStatDice::OnDestroy()
{

}

void CWndStatDice::SetFinish()
{
	m_tmStart.Set( 0 );
	m_tmStartTab.Set( 0 );

	for( int i=0; i<nResultMax; ++i )
		m_pStatDice[i].ViewResult( 1 );
}

void CWndStatDice::OnDraw( C2DRender* p2DRender ) 
{ 
 	if( m_tmStart.IsTimeOut() )
	{
		if( m_tmStartTab.IsTimeOut() )
		{
			if( m_nResultCount < nResultMax )
				m_pStatDice[m_nResultCount++].ViewResult( 1 );
			m_tmStartTab.Reset();
		}
	}

	for( int i=0; i<nResultMax; ++i )
	{
		m_pStatDice[i].Render( p2DRender, m_DiceRect[i] );
	}

	if( IsFinish() )
	{
 		/// 캐릭터창 업데이트 반영
		CWndCharacter* pWndCHAR = (CWndCharacter*)g_WndMng.GetWndBase( APP_CHARACTER3 );
			if( pWndCHAR )
				pWndCHAR->m_wndCharInfo.m_RenderStat = g_pPlayer->m_Stat;
	}
} 

void CWndStatDice::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_bNoCloseButton	= TRUE;

	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl( WIDC_STATIC1 );
	m_DiceRect[0] = lpWndCtrl->rect;
	lpWndCtrl = GetWndCtrl( WIDC_STATIC2 );
	m_DiceRect[1] = lpWndCtrl->rect;
	lpWndCtrl = GetWndCtrl( WIDC_STATIC3 );
	m_DiceRect[2] = lpWndCtrl->rect;
	lpWndCtrl = GetWndCtrl( WIDC_STATIC4 );
	m_DiceRect[3] = lpWndCtrl->rect;

	m_pStatDice = new CRenderStatDice[ nResultMax ];
	m_pStatDice[0].Init( );
	m_pStatDice[1].Init( );
	m_pStatDice[2].Init( );
	m_pStatDice[3].Init( );

	MoveParentCenter();

	m_tmStart.Set( 2000 );
	m_tmStartTab.Set( 1000 );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndStatDice::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_STAT_DICE, 0, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndStatDice::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndStatDice::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndStatDice::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndStatDice::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

bool CWndStatDice::IsFinish( )		const
{ 
	for( int i=0; i<nResultMax; ++i )
	{
		if( !m_pStatDice[i].IsFinish() )
			return false;
	}

	return true;
} 


BOOL CWndStatDice::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == 10000 )
		return false;

 	if( nID == WIDC_OK && IsFinish() )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndStatDice::SetResult( int nStr, int nSta, int nDex, int nInt )
{
	m_pStatDice[0].SetResult( nStr );
	m_pStatDice[1].SetResult( nSta );
	m_pStatDice[2].SetResult( nDex );
	m_pStatDice[3].SetResult( nInt );
}

#ifdef CARD_UPGRADE_SYSTEM
CWndCardUpgrade::CWndCardUpgrade() :
m_bSendPacket(FALSE)
{
	memset( m_objID_Item, NULL_ID, sizeof( m_objID_Item ) );
	m_pItemElem = NULL;
	m_nResultItemIdx = NULL_ID;
} 
CWndCardUpgrade::~CWndCardUpgrade() 
{ 
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase( APP_INVENTORY );
	if(pWndInventory != NULL)
		pWndInventory->CISetDieFlag_ItemCtrl( FALSE );

	if ( m_pItemElem )
	{
		m_pItemElem->SetExtra(0);
	}
}

void CWndCardUpgrade::OnLButtonDblClk( UINT nFlags, CPoint point)
{
	if(m_bSendPacket == FALSE)
	{
		if( PtInRect( &m_rectSlot_Item[N_COMBINE], point ) )
		{
			m_objID_Item[N_COMBINE] = NULL_ID;
			if ( m_pItemElem )
			{
				m_pItemElem->SetExtra(0);
			}
			CWndStatic* pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
			pWnd->m_strTitle = "";
			pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
			pWnd->m_strTitle = "0";
			pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
			pWnd->m_strTitle = "";
		}
		else if( PtInRect( &m_rectSlot_Item[N_CASH], point ) )
		{
			m_objID_Item[N_CASH] = NULL_ID;
			return;
		}else return;

		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
		if( m_objID_Item[N_COMBINE] != NULL_ID  )
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( TRUE );
		}
		else 
		{
			if( pBtnOK ) 
				pBtnOK->EnableWindow( FALSE );
		}
		m_nResultItemIdx = NULL_ID;
	}
}

void CWndCardUpgrade::OnDraw( C2DRender* p2DRender ) 
{
	
	
	for( int i = 0; i < 2; i++ )
	{
		if( m_objID_Item[ i ] != NULL_ID )
		{
			FLItemElem* pItem;
		
			pItem = g_pPlayer->m_Inventory.GetAtId( m_objID_Item[ i ] );
		
			//if( IsUsableItem( pItem ) )
			if ( pItem )
				p2DRender->RenderTexture( m_rectSlot_Item[i].TopLeft(), pItem->GetTexture() );
		}
	}
	PT_ITEM_SPEC pSpec	= g_xSpecManager->GetSpecItem( m_nResultItemIdx );
	CTexture* pTexture;
	if ( pSpec )
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pSpec->szIcon), 0xffff00ff );
		p2DRender->RenderTexture( m_rectSlot_Item[N_RESULT].TopLeft(), pTexture );
	}


}
void CWndCardUpgrade::InitCtrl()
{
	memset( m_objID_Item, NULL_ID, sizeof( m_objID_Item ) );

	CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BT_OK );
	if( !pBtnOK )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndCardUpgrade::InitCtrl()" ) );

	pBtnOK->EnableWindow( FALSE );
}

void CWndCardUpgrade::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndStatic* pWndSlot1 = (CWndStatic*)GetDlgItem( WIDC_STATIC2 );
	CWndStatic* pWndSlot2 = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	CWndStatic* pWndSlot3 = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );

	if( !( pWndSlot1 && pWndSlot2 && pWndSlot3 ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndCardUpgrade::OnInitialUpdate()" ) );

	m_rectSlot_Item[N_COMBINE] = pWndSlot1->GetWndRect();
	m_rectSlot_Item[N_CASH] = pWndSlot2->GetWndRect();
	m_rectSlot_Item[N_RESULT] = pWndSlot3->GetWndRect();

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	if( pWndInventory )
		pWndInventory->CISetDieFlag_ItemCtrl( TRUE );
	

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndCardUpgrade::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CARD_UPGRADE, 0, CPoint( 0, 0 ), pWndParent );
}

  //직접 윈도를 열때 사용 
/*BOOL CWndCardUpgrade::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
}*/

BOOL CWndCardUpgrade::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( EN_RETURN == message )
	{
		DoModalConfirm( );
		return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
	}

	switch( nID )
	{
	case WIDC_BUTTON1:
		SendPacket( );
		break;

	case WIDC_BUTTON2:
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndCardUpgrade::DoModalConfirm( )
{
}

void CWndCardUpgrade::SendPacket( )
{
	FLPacketItemUpgradeCardReq kPacket;
	kPacket.dwMainItemObjID = m_objID_Item[ N_COMBINE ];
	kPacket.dwProtectionItemObjID = m_objID_Item[ N_CASH ];
	g_DPlay.SendPacket( &kPacket );
}


void CWndCardUpgrade::ReceivedPacket(const DWORD dwResult)
{
	if( FSC_ITEMUPGRADE_UPGRADE_SUCCESS == dwResult )
	{
		//합성결과 : 성공
		/*ResetSlot( SLOT_1 );
		ResetSlot( SLOT_2 );
		ResetSlot( SLOT_3 );*/
	}
	else if( FSC_ITEMUPGRADE_UPGRADE_FAIL == dwResult )
	{
		//합성결과 : 실패
		//if( NULL_ID != m_dwSlotItem[ SLOT_3 ] )
		//{
		//	//ok
		//}
		//else 
		//{
		//	ResetSlot( SLOT_2 ); 
		//}
	}
	m_objID_Item[N_COMBINE] = NULL_ID;
	if ( m_pItemElem )
	{
		m_pItemElem->SetExtra(0);
	}
	m_objID_Item[N_CASH] = NULL_ID;
	m_nResultItemIdx = NULL_ID;
	CWndStatic* pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pWnd->m_strTitle = "";
	pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	pWnd->m_strTitle = "0";
	pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
	pWnd->m_strTitle = "";
}


BOOL CWndCardUpgrade::Process()
{
	return CWndBase::Process( );
}

BOOL CWndCardUpgrade::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame	= pShortcut->m_pFromWnd->GetFrameWnd();
	if( !pWndFrame )
	{
		FLASSERT( 0 );
		return FALSE;
	}

	// 인벤에서 온 아이템만 허용
	//gmpbigsun( 20101116 ) : 휴대가방 검사 추가
	if( APP_INVENTORY != pWndFrame->GetWndId( ) && APP_BAG_EX != pWndFrame->GetWndId( ) )			
	{
		return FALSE;
	}
	
	//check valid item
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );

	if( !IsUsableItem( pItem ) )
		return FALSE;
	

	PT_ITEM_SPEC pProp = pItem->GetProp();
	if( NULL == pProp )
		return FALSE;
	
	//check slot
	if( pProp->dwReferTarget1 != NULL_ID && pItem->m_nItemNum >=10 && PtInRect( &m_rectSlot_Item[N_COMBINE], point ) )		// 합성아이템
	{
		CWndStatic* pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
		pWnd->m_strTitle = pProp->szName;
		//if( IK3_BARUNA_PROTECTION == pProp->dwItemKind3 || IK3_BARUNA_PROTECTION_KEEP == pProp->dwItemKind3 )
			m_objID_Item[N_COMBINE] = pShortcut->m_dwId;
			//m_objID_Item[N_RESULT= pProp
		//else
		//{
		//	g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_BARUNASMELPROT) ); //바루나의 가호 등록
		//	return FALSE;
		//}
		if ( m_pItemElem )
			m_pItemElem->SetExtra ( 0 );
		m_pItemElem = pItem;
		m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 10);
		m_nResultItemIdx = pProp->dwReferTarget1;
		//PT_ITEM_SPEC pSpec	= g_xSpecManager->GetSpecItem( pProp->dwReferTarget1 );
		//pSpec->szIcon
		//dwReferTarget2
		pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
		pWnd->m_strTitle.Format("%d", pProp->dwReferTarget2);
		pWnd->AddWndStyle(WSS_MONEY);
		
		PT_ITEM_SPEC pSpec	= g_xSpecManager->GetSpecItem( pProp->dwReferTarget1 );
		pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
		if ( pSpec ) pWnd->m_strTitle = pSpec->szName;

	}
	else 
	if( pItem->m_dwItemId == ITEM_INDEX( 24843, II_SYS_SYS_SCR_SMELPROT5 ) && PtInRect( &m_rectSlot_Item[N_CASH], point ) )		//cash item
	{
		//if( pProp->IsBaruna( ) )	
			m_objID_Item[N_CASH] = pShortcut->m_dwId;
		//else						
		//{
		//	g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT01) ); //"바루나 아이템을 등록해야합니다 
		//	return FALSE; 
		//}
	}


	
	if( m_objID_Item[N_COMBINE] != NULL_ID )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
	else 
		return TRUE;

	return TRUE;
}
void CWndCardUpgrade::SetItem(FLItemElem* pItemElem)
{
	FLASSERT(pItemElem != NULL);

	PT_ITEM_SPEC pProp = pItemElem->GetProp();
	if( NULL == pProp )
		return;
	
	//check slot
	if( pProp->dwReferTarget1 != NULL_ID && pItemElem->m_nItemNum >=10 && ( IK3_SOCKETCARD == pProp->dwItemKind3 || IK3_SOCKETCARD2 == pProp->dwItemKind3 ) )		// 합성아이템
	{
		CWndStatic* pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
		pWnd->m_strTitle = pProp->szName;
		//if( IK3_BARUNA_PROTECTION == pProp->dwItemKind3 || IK3_BARUNA_PROTECTION_KEEP == pProp->dwItemKind3 )
			m_objID_Item[N_COMBINE] = pItemElem->m_dwObjId;
		//else
		//{
		//	g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_GHOUSE_BARUNASMELPROT) ); //바루나의 가호 등록
		//	return FALSE;
		//}
		if ( m_pItemElem )
			m_pItemElem->SetExtra ( 0 );
		m_pItemElem = pItemElem;
		m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 10);

		m_nResultItemIdx = pProp->dwReferTarget1;
		
		pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
		pWnd->m_strTitle.Format("%d", pProp->dwReferTarget2);
		pWnd->AddWndStyle(WSS_MONEY);

		PT_ITEM_SPEC pSpec	= g_xSpecManager->GetSpecItem( pProp->dwReferTarget1 );
		pWnd = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
		if ( pSpec )	pWnd->m_strTitle = pSpec->szName;
	}
	else 
	if(  pItemElem->m_dwItemId == ITEM_INDEX( 24843, II_SYS_SYS_SCR_SMELPROT5 ) )		//cash item
	{
		//if( pProp->IsBaruna( ) )	
			m_objID_Item[N_CASH] = pItemElem->m_dwObjId;
		//else						
		//{
		//	g_WndMng.OpenMessageBox( prj.GetText(TID_MMI_NEWSMELT_NEWSMELT01) ); //"바루나 아이템을 등록해야합니다 
		//	return FALSE; 
		//}
	}


	
	if( m_objID_Item[N_COMBINE] != NULL_ID )
	{
		CWndButton* pBtnOK  = ( CWndButton* )GetDlgItem( WIDC_BUTTON1 );
		if( pBtnOK )
			pBtnOK->EnableWindow( TRUE );
	}
}


#endif	// CARD_UPGRADE_SYSTEM

#endif