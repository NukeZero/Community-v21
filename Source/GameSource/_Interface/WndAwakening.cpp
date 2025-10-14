
#include "stdafx.h"
#include "WndAwakening.h"
#include "WndManager.h"

/****************************************************
  WndId : APP_AWAKENING - 아이템 각성 창
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_STATIC1 - 
  CtrlId : WIDC_BUTTON1 - Button
****************************************************/
//sun: 11, 각성, 축복

// gmpbigsun ( 10_04_05 ) : CWndAwakening class는 현재 쓰이지 않음
CWndAwakening::CWndAwakening() 
{
	m_pItemElem  = NULL;
	m_pEItemProp = NULL;
	m_pText		 = NULL;
	m_pTexture	 = NULL;
} 
CWndAwakening::~CWndAwakening() 
{ 
} 
void CWndAwakening::OnDraw( C2DRender* p2DRender ) 
{ 

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
			
	if(m_pTexture)
		m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
		
} 

BOOL CWndAwakening::process()
{
/*
	PT_ITEM_SPEC pItemProp;
	
	if(m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(pItemProp != NULL)
		{
			m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		}
	} 
*/
	return CWndBase::Process();

}

void CWndAwakening::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 여기에 코딩하세요
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
			pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), _T( "ButOk2.bmp" ) ), TRUE);
	}

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	SetDescription();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAwakening::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_AWAKENING, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWnd::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/

void CWndAwakening::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndAwakening::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndAwakening::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndAwakening::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndAwakening::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndAwakening::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 

	if( nID == WIDC_BUTTON1 )
	{
		//서버로 시작을 알린다.
		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
			pButton->EnableWindow(FALSE);

			// 서버에 처리 요청하는 함수 호출해야함
			if(m_pItemElem)
			{
				g_DPlay.SendAwakening(m_pItemElem->m_dwObjId);
				Destroy();
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndAwakening::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture	= NULL;
	}
}

BOOL CWndAwakening::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	FLItemElem* pTempElem;
	pTempElem  = (FLItemElem*)g_pPlayer->GetItemId( pShortcut->m_dwId );

	PT_ITEM_SPEC pItemProp = pTempElem->GetProp();
	if( pItemProp->IsWeaponParts() == FALSE && pItemProp->IsArmorParts() == FALSE )
// 	int nRandomOptionKind	= g_xRandomOptionProperty->GetRandomOptionKind( pTempElem );
// 	if( nRandomOptionKind  != CRandomOptionProperty::eAwakening )
	{
		// 적절한 대상이 아닙니다.
		//g_WndMng.PutString( prj.GetText( TID_GAME_INVALID_TARGET_ITEM ), NULL, prj.GetTextColor( TID_GAME_INVALID_TARGET_ITEM  ) );
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_INVALID_TARGET_ITEM ) );
		return FALSE;
	}

	if( pTempElem->IsSetRandomOptionExtension() == true )
//	if( g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() ) > 0 )
	{
		// 이미 각성된 아이템입니다.
		//g_WndMng.PutString( prj.GetText( TID_GAME_AWAKE_OR_BLESSEDNESS01 ), NULL, prj.GetTextColor( TID_GAME_AWAKE_OR_BLESSEDNESS01  ) );
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_AWAKE_OR_BLESSEDNESS01 ) );
		return FALSE;
	}
	
	if(pTempElem != NULL)
	{
		if(m_pItemElem) m_pItemElem->SetExtra(0);
		m_pItemElem = pTempElem;
		m_pEItemProp = m_pItemElem->GetProp();
		m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(TRUE);
		
		LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
		if(m_pEItemProp != NULL)
		{
			m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
		}
	}

	return TRUE;

}

void CWndAwakening::SetDescription()
{
	CScript scanner;
	BOOL checkflag;
	CHAR* szChar;


	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "ItemAwakening.inc" ) ));
	szChar = scanner.m_pProg;
	
	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();	
	}
}

CWndSelectAwakeCase::CWndSelectAwakeCase( )
: m_dwDeltaTime( 0 ),
m_dwItemIndex( 0 ),
m_pTexture( NULL ),
m_pVertexBufferGauge(NULL), 
m_pTexGuage( NULL )
{
	m_dwTargetObjId = 0;
	m_iSerialNum = 0;
}

CWndSelectAwakeCase::~CWndSelectAwakeCase( )
{
	if( m_iSerialNum != 0 ) {
		g_DPlay.SendSelectedAwakeningValue( m_dwTargetObjId, m_iSerialNum, _AWAKE_OLD_VALUE );
	}
	DeleteDeviceObjects();
}
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSelectAwakeCase::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_AWAKE_SELECTCASE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndSelectAwakeCase::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 여기에 코딩하세요

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( m_dwItemIndex );
	if( pProp )
		m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon ), 0xffff00ff );
	
	m_pTexGuage = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "Wndguage.tga"   ), 0xffff00ff );

	if( !m_pTexGuage )
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pTexGuage(Wndguage.tga) is NULL" ) );

	AddWndStyle( WBS_MODAL );

	RestoreDeviceObjects( );
} 


BOOL CWndSelectAwakeCase::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 || nID == WIDC_BUTTON2 ) {
		const BYTE selectFlag = nID == WIDC_BUTTON1 ? _AWAKE_OLD_VALUE : _AWAKE_NEW_VALUE;
		g_DPlay.SendSelectedAwakeningValue( m_dwTargetObjId, m_iSerialNum, selectFlag );
		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//선택됨
		m_dwTargetObjId = m_iSerialNum = 0;
		Destroy();
	}
// 	if( nID == WIDC_BUTTON1 )
// 	{
// 		g_DPlay.SendSelectedAwakeningValue( m_dwTargetObjId, m_iSerialNum, _AWAKE_OLD_VALUE );
// 		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//선택됨
// 		Destroy();
// 	}
// 	else if( nID == WIDC_BUTTON2 )
// 	{
// 		g_DPlay.SendSelectedAwakeningValue( m_dwTargetObjId, m_iSerialNum, _AWAKE_NEW_VALUE );
// 		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//선택됨
// 		Destroy();
// 	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

const static DWORD AWAKE_KEEP_TIME = 60000;	//60초

BOOL CWndSelectAwakeCase::process()
{
	return TRUE;
}

void CWndSelectAwakeCase::OnDraw( C2DRender* p2DRender ) 
{ 
	if( !g_pPlayer )
		return;

	FLItemElem* pItemElem = (FLItemElem*)g_pPlayer->GetItemId( m_dwTargetObjId );
	if( !pItemElem )
	{
		FLASSERT( 0 );
		return;
	}

	m_dwDeltaTime += _GetDeltaTime( );

	if( m_dwDeltaTime > AWAKE_KEEP_TIME )
	{
		//제한시간이 지나면 현재옵션으로 요청 
		g_DPlay.SendSelectedAwakeningValue( m_dwTargetObjId, m_iSerialNum, _AWAKE_OLD_VALUE );

		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE02 ) );	//선택시간 초과
		
		m_dwDeltaTime = 0;
		Destroy();
	}

	//old
	OutputOptionString( p2DRender, pItemElem );

	//new
	OutputOptionString( p2DRender, pItemElem, TRUE );


	CRect rect = GetWindowRect();
	rect.left += 14;
	rect.top += 254;

	// 시간 / 100 * 360(총길이) / 600 (총시간)
	rect.right = LONG(( rect.left + (AWAKE_KEEP_TIME - m_dwDeltaTime) / 100 ) * 0.6f) ;		//귀찮으니 걍 바의 길이를 최대초로...
	rect.bottom = rect.top + 20;

//	if( m_pTexGuage )
//		p2DRender->RenderFillRect( rect, 0xffff0000, m_pTexGuage->m_pTexture  );

	m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);

	// draw icon
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	FLASSERT( wndCtrl );
			
	if(m_pTexture)
		m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );
} 

extern BOOL IsDst_Rate( int nDstParam );
extern char *FindDstString( int nDstParam );
void CWndSelectAwakeCase::OutputOptionString( C2DRender* p2DRender, FLItemElem* pItemElem, BOOL bNew )
{
	// 옵션 문자열 그리기
	if( !pItemElem )
		return;

	if( bNew == TRUE )
	{
		if( pItemElem->IsSetNewRandomOptionExtension() == true )
		{
			CString str;

			// option
			for( size_t Nth = 0; Nth < pItemElem->GetNewRandomOptionExtensionSize(); ++Nth )
			{
				WORD wDstID = pItemElem->GetNewRandomOptionExtensionDstID( Nth );
				short shAdjValue = pItemElem->GetNewRandomOptionExtensionAdjValue( Nth );

				if( IsDst_Rate( wDstID ) )
				{
					if( wDstID == DST_ATTACKSPEED )
					{
						str.Format( "\n%s %c%d%% ", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue / 2 / 10 ) );
					}
					else
					{
						str.Format( "\n%s %c%d%%", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
					}
				}
				else
				{
					str.Format( "\n%s %c%d", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
				}

				p2DRender->TextOut( 70, 130 + Nth * 17/*20*/, str, m_dwColor );
			}
		}
	}
	else
	{
		if( pItemElem->IsSetRandomOptionExtension() == true )
		{
			CString str;

			// option
			for( size_t Nth = 0; Nth < pItemElem->GetRandomOptionExtensionSize(); ++Nth )
			{
				WORD wDstID = pItemElem->GetRandomOptionExtensionDstID( Nth );
				short shAdjValue = pItemElem->GetRandomOptionExtensionAdjValue( Nth );

				if( IsDst_Rate( wDstID ) )
				{
					if( wDstID == DST_ATTACKSPEED )
					{
						str.Format( "\n%s %c%d%% ", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue / 2 / 10 ) );
					}
					else
					{
						str.Format( "\n%s %c%d%%", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
					}
				}
				else
				{
					str.Format( "\n%s %c%d", FindDstString( wDstID ), ( shAdjValue > 0? '+': '-' ), ::abs( shAdjValue ) );
				}

				p2DRender->TextOut( 70, 18 + Nth * 17/*20*/, str, m_dwColor );
			}
		}
	}
}

void CWndSelectAwakeCase::SetData( DWORD dwObjId, SERIALNUMBER iSerialNum )
{ 
	//server로 전송할 데이터를 유지하고, 아이템 Index를 뽑아서 아이콘을 그려줄 준비
	m_dwTargetObjId = dwObjId;
	m_iSerialNum = iSerialNum;

	if( !g_pPlayer )
	{
		FLASSERT( 0 );
		return;
	}

	FLItemBase* pItemBase = g_pPlayer->GetItemId( m_dwTargetObjId );
	if( pItemBase )
	{
		PT_ITEM_SPEC pProp = pItemBase->GetProp( );
		if(pProp)
			m_dwItemIndex = pProp->dwID;
	}
}

HRESULT CWndSelectAwakeCase::RestoreDeviceObjects()
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
	
	return S_OK;
}
HRESULT CWndSelectAwakeCase::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
HRESULT CWndSelectAwakeCase::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
