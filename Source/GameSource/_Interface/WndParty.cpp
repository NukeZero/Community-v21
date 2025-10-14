#include "stdafx.h"
#include "WndParty.h"
#include "WndManager.h"
#include "Party.h"
#include "playerdata.h"		//sun: 11, 캐릭터 정보 통합

/****************************************************
  WndId : APP_PARTY - 극단(Party)
  CtrlId : WIDC_TABCTRL1 - TabCtrl
  CtrlId : WIDC_LEAVE - 탈퇴
  CtrlId : WIDC_CHANGE - 극단전환
****************************************************/

CWndParty::CWndParty()

{ 
	m_WndPartyChangeTroup = NULL;
	m_pWndPartyQuick = NULL;			//sun: 8차 파티창 관련 적용 Neuz
	FLINFO_LOG( PROGRAM_NAME, _T( "CWndParty::CWndParty()" ) );
} 
CWndParty::~CWndParty() 
{ 
	SAFE_DELETE( m_WndPartyChangeTroup );

	DeleteDeviceObjects();
} 
void CWndParty::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	FLINFO_LOG( PROGRAM_NAME, _T( "IN" ) );

	CWndNeuz::SerializeRegInfo( ar, dwVersion );
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	if( ar.IsLoading() )
	{
		if( dwVersion == 0 )
		{
		}
		else
		if( dwVersion == 1 )
		{
			int nCurSel;
			ar >> nCurSel;
			lpTabCtrl->SetCurSel( nCurSel );
		}
	}
	else
	{
		dwVersion = 1;
		ar << lpTabCtrl->GetCurSel();
	}

	FLINFO_LOG( PROGRAM_NAME, _T( "OUT" ) );
}
void CWndParty::OnDraw( C2DRender* p2DRender ) 
{ 
	if( g_Party.GetSizeofMember() < 2 )
	{
		m_pWndLeave->EnableWindow( FALSE );
		m_pWndTransfer->EnableWindow( FALSE );
		m_pBtnPartyQuick->EnableWindow( FALSE );	//sun: 8차 파티창 관련 적용 Neuz
	}
	else
	{
		m_pWndLeave->EnableWindow( TRUE );
		m_pWndTransfer->EnableWindow( TRUE );
		m_pBtnPartyQuick->EnableWindow( TRUE );		//sun: 8차 파티창 관련 적용 Neuz
	}

	if( g_Party.GetSizeofMember() < 2 && g_Party.m_nLevel > 1 )
	{
		m_pWndLeave->EnableWindow( TRUE );
	}

	if( g_Party.GetLevel() >= 10 && g_Party.IsLeader( g_pPlayer->m_idPlayer ) )
		m_pWndChange->EnableWindow( TRUE );
	else
		m_pWndChange->EnableWindow( FALSE );

	// 파티 정보 출력
	CWndStatic* pWndStatic;
	CString strTemp;
	if( g_Party.GetKind()== PARTY_KIND_BASE )
	{
		strTemp.Format("%s",prj.GetText(TID_GAME_PARTY1));
	}
	else 
	{
		if( 0 == strlen( g_Party.m_sParty ) )
			strTemp.Format("%s",prj.GetText(TID_GAME_PARTY2));
		else
			strTemp.Format("%s",g_Party.m_sParty);
	}
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_NAME );
	pWndStatic->SetTitle(strTemp);
	strTemp.Format("%d",g_Party.m_nLevel);
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_LEVEL );
	pWndStatic->SetTitle(strTemp);
	
	if( g_Party.m_nLevel >= 10 )
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)( ( ( 50 + g_Party.GetLevel() ) * g_Party.GetLevel() / 13 ) * 10 ));
	}
	else
	{
		strTemp.Format("%.2f %%", (float)g_Party.m_nExp * 100 / (float)prj.m_aExpParty[g_Party.m_nLevel].Exp );
	}
	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_EXP );
	pWndStatic->SetTitle(strTemp);

	//gmpbigsun( 20120327 ) : 무제한 표시
	if( g_Party.IsActivatedEffect( PARTY_EFFECT_POINT_LIMITLESS ) )
		strTemp = CString( prj.GetText( TID_MMI_PARTYPOINTINFINITE ) );
	else
		strTemp.Format("%d",g_Party.m_nPoint);

	pWndStatic = (CWndStatic*)GetDlgItem( WIDC_POINT );
	pWndStatic->SetTitle(strTemp);

	// 아이템 경험치 분배방식 상태 갱신
	CWndButton* pWndButton;
	switch( g_Party.m_nTroupsShareExp) 
	{
	case 0:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(FALSE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(FALSE);
		}
		break;
	case 1:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(TRUE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_EXP_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO6 );
			pWndButton->SetCheck(FALSE);
//			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO7 );
//			pWndButton->SetCheck(TRUE);
		}
		break;
	}
	switch(g_Party.m_nTroupeShareItem) 
	{
	case 0:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 1:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(TRUE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(FALSE);
		}
		break;
	case 3:
		{
			pWndButton = (CWndButton*)GetDlgItem( WIDC_ITEM_SHARE );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO2 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO3 );
			pWndButton->SetCheck(FALSE);
			pWndButton = (CWndButton*)GetDlgItem( WIDC_RADIO4 );
			pWndButton->SetCheck(TRUE);
		}
		break;
	}
} 
void CWndParty::OnInitialUpdate() 
{ 
	FLINFO_LOG( PROGRAM_NAME, _T( "IN" ) );

	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	m_pWndLeave = (CWndButton*)GetDlgItem( WIDC_LEAVE );
	m_pWndChange = (CWndButton*)GetDlgItem( WIDC_CHANGE );
	m_pWndTransfer = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );
	m_pBtnPartyQuick = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );		//sun: 8차 파티창 관련 적용 Neuz

	//COMMENT BEGIN ( ver<8 )
	//m_pBtnPartyQuick = (CWndButton*)GetDlgItem( WIDC_BUTTON2 );
	//m_pBtnPartyQuick->EnableWindow(FALSE);
	//m_pBtnPartyQuick->SetVisible(FALSE);
	//COMMENT END

	if( g_Party.GetSizeofMember() < 2 )
	{
		m_pWndTransfer->EnableWindow( FALSE );
		m_pWndLeave->EnableWindow( FALSE );
		m_pBtnPartyQuick->EnableWindow( FALSE );		//sun: 8차 파티창 관련 적용 Neuz
	}
	m_pWndChange->EnableWindow( FALSE );
	
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

	if(pWndTabCtrl)
	{
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			pWndTabCtrl->SetTabTitleAlign( CWndTabCtrl::ALIGN_CENTER );
	}

	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	//gmpbigsun( 100416 ) : Error 제거 
	//FLINFO_LOG( PROGRAM_NAME, _T( "m_wndPartyInfo.Create - IN" ) );
	m_wndPartyInfo.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_PARTY_INFO );
	//FLINFO_LOG( PROGRAM_NAME, _T( "m_wndPartyInfo.Create - OUT" ) );

	//FLINFO_LOG( PROGRAM_NAME, _T( "m_wndPartySkill.Create - IN" ) );
	m_wndPartySkill.Create( WBS_CHILD | WBS_NOFRAME | WBS_NODRAWFRAME , rect, pWndTabCtrl, APP_PARTY_SKILL );
	//FLINFO_LOG( PROGRAM_NAME, _T( "m_wndPartySkill.Create - OUT" ) );

	WTCITEM tabTabItem;
	
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_INFOMATION);//"정보";
	tabTabItem.pWndBase = &m_wndPartyInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );
	
	tabTabItem.pszText = prj.GetText(TID_APP_SKILL);//"스킬";
	tabTabItem.pWndBase = &m_wndPartySkill;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	MoveParentCenter();

	FLINFO_LOG( PROGRAM_NAME, _T( "OUT" ) );
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndParty::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	FLINFO_LOG( PROGRAM_NAME, _T( "IN" ) );
	BOOL bFlag = CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY, 0, CPoint( 0, 0 ), pWndParent );
	FLINFO_LOG( PROGRAM_NAME, _T( "OUT" ) );

	// Daisy에서 설정한 리소스로 윈도를 연다.
	return bFlag;
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndParty::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndParty::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndParty::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndParty::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndParty::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndParty::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_LEAVE ) 
	{ // 탈퇴 눌렸다...
		if( g_pPlayer->m_nDuel == 2 )		// 극단듀얼중이면 탈퇴 안됨.
		{
			DWORD dwText = TID_GAME_PPVP_LEAVE;
			g_WndMng.PutString( prj.GetText( dwText ), NULL, prj.GetTextColor( dwText ) );
		} 
		else
		{
			if(m_wndPartyInfo.m_nSelected!=-1) 
			{
				u_long  nLeaveMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId; //탈퇴시킬놈 ID
				// 여기다가 탈퇴
				if( g_Party.m_nSizeofMember != 0 )
				{
					CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
					if( pWndPartyLeaveConfirm == NULL )
					{
						g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
						g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
						if( g_Party.m_aMember[0].m_uPlayerId == g_pPlayer->m_idPlayer )
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( nLeaveMember );
						else
							g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
					}
				}
				else
				{
					// 극단이 구성이 안되어있음
				}
			}
			else
			{
				CWndPartyLeaveConfirm* pWndPartyLeaveConfirm = (CWndPartyLeaveConfirm*)g_WndMng.GetWndBase( APP_PARTYLEAVE_CONFIRM );
				if( pWndPartyLeaveConfirm == NULL )
				{
					g_WndMng.m_pWndPartyLeaveConfirm = new CWndPartyLeaveConfirm;
					g_WndMng.m_pWndPartyLeaveConfirm->Initialize( NULL, APP_PARTYLEAVE_CONFIRM );
					if( g_Party.m_nSizeofMember != 0 )
						g_WndMng.m_pWndPartyLeaveConfirm->SetLeaveId( g_pPlayer->m_idPlayer );
				}
				// 내 자신을 극단에서 탈퇴
			}
			m_wndPartyInfo.m_nSelected=-1;
		}
	}
	// 극단장 인계.
	else if( nID==WIDC_BUTTON1 )
	{
		if( g_Party.m_nSizeofMember != 0 )
		{
			if( g_Party.IsLeader( g_pPlayer->m_idPlayer ) && m_wndPartyInfo.m_nSelected != -1 )
			{
				if( g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_bRemove == FALSE )
				{
					u_long  nLeadMember = g_Party.m_aMember[m_wndPartyInfo.m_nSelected].m_uPlayerId;
					if( g_Party.IsLeader( nLeadMember ) == FALSE )
					{
						if( g_pPlayer->m_nDuel == 2 )	// 극단 듀얼중일때는 극단장을 바꿀수 없음
							g_WndMng.PutString( prj.GetText( TID_PK_NOCHANGE_PARTYLEADER ), NULL, prj.GetTextColor( TID_PK_NOCHANGE_PARTYLEADER ) );
						else
							g_DPlay.SendPartyChangeLeader( g_pPlayer->m_idPlayer, nLeadMember );						
					}
					else
					{
						g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ALREADYMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ALREADYMASTER ) ); // "이미 단장을 하고 입니다"
					}
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_MASTER_AWAY ), NULL, prj.GetTextColor( TID_GAME_MASTER_AWAY ) ); // "단장이 아니거나 극단창에서 단원을 선택을 해야 합니다"					
				}
			}
			else
			{
				g_WndMng.PutString( prj.GetText( TID_GAME_PARTY_ISNOTMASTER ), NULL, prj.GetTextColor( TID_GAME_PARTY_ISNOTMASTER ) ); // "단장이 아니거나 극단창에서 단원을 선택을 해야 합니다"
			}
		}
	}
	else if( nID==WIDC_CHANGE )
	{
		if( g_Party.GetKind() == PARTY_KIND_BASE /* && g_Party.m_nLevel == 10 */ )
		{
			// 순회극단으로 바꾸기~
			SAFE_DELETE( m_WndPartyChangeTroup );
			m_WndPartyChangeTroup = new CWndPartyChangeTroup;
			m_WndPartyChangeTroup->Initialize( this );
		}
	}
	//sun: 8차 파티창 관련 적용 Neuz
	else if(nID == WIDC_BUTTON2)
	{
		if(g_Party.GetSizeofMember() >= 2)
		{
			m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.GetApplet( APP_PARTY_QUICK );
			if(m_pWndPartyQuick == NULL)
			{
				m_pWndPartyQuick = (CWndPartyQuick*)g_WndMng.CreateApplet( APP_PARTY_QUICK );
				m_pWndPartyQuick->SetActiveMember(g_Party.GetSizeofMember());
			}
			else
			{
				m_pWndPartyQuick->Destroy();
				m_pWndPartyQuick = NULL;
			}
		}
	}
	// 순회극단일때만 아이템 경험치 분배방식 갱신
	else
	if( g_Party.IsLeader(g_pPlayer->m_idPlayer) )
	{
		if( g_Party.GetKind() == PARTY_KIND_EXTEND )
		{
			// 경험치 분배 방식은 순회극단 임
			int nTroupsShareExp = g_Party.m_nTroupsShareExp;
			
			if(nID==WIDC_EXP_SHARE) nTroupsShareExp   = 0;
			else if(nID==WIDC_RADIO6) nTroupsShareExp = 1;
	//		else if(nID==WIDC_RADIO7) nTroupsShareExp = 2;
			
			if( nTroupsShareExp == 2 )
			{
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0004) ) );
				//			g_WndMng.OpenMessageBox( _T("경험치 분배의 동일분배는 아직 지원이 안됩니다.") );
			}
			else
			{
				if( nTroupsShareExp != g_Party.m_nTroupsShareExp )
				{
					// Send
					g_DPlay.SendChangeShareExp( nTroupsShareExp );
				}
			}
		}
		// 아이템 분배 방식은 단막극단 임
		int nTroupeShareItem = g_Party.m_nTroupeShareItem;

		if(nID==WIDC_ITEM_SHARE) nTroupeShareItem  = 0;
		else if(nID==WIDC_RADIO2) nTroupeShareItem = 1;
		else if(nID==WIDC_RADIO3) nTroupeShareItem = 2;
		else if(nID==WIDC_RADIO4) nTroupeShareItem = 3;		

		if( nTroupeShareItem != g_Party.m_nTroupeShareItem )
		{
			// Send
			g_DPlay.SendChangeShareItem( nTroupeShareItem );
			
		}
	}
	else
	{
		if( nID != WTBID_CLOSE && nID != WTBID_HELP )
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0008) ) );
//			g_WndMng.OpenMessageBox( "단장이 아니거나 순회극단이 아니므로 변경할수 없습니다." );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


#pragma region WndPartyInfo


/****************************************************
  WndId		: APP_PARTY_INFO - 개인분배
  CtrlId 	: WIDC_GROUPBOX1 - 극단정보
  CtrlId 	: WIDC_GROUPBOX3 - 경험치분배
  CtrlId 	: WIDC_GROUPBOX4 - 아이템분배
  CtrlId 	: WIDC_ITEM_SHARE - 개인분배
  CtrlId 	: WIDC_RADIO2 - 순서분배
  CtrlId 	: WIDC_RADIO3 - 수동분배
  CtrlId 	: WIDC_RADIO4 - 랜던분배
  CtrlId 	: WIDC_EXP_SHARE - 동일분배
  CtrlId 	: WIDC_RADIO6 - 기여도분배
  CtrlId 	: WIDC_RADIO7 - 피해분배
  CtrlId 	: WIDC_STATIC1 - 극단명칭 :
  CtrlId 	: WIDC_STATIC2 - 극단레벨 :
  CtrlId 	: WIDC_STATIC3 - 극단경험치 :
  CtrlId 	: WIDC_STATIC4 - 극단포인트 :
  CtrlId 	: WIDC_NAME - 마스커레이드
  CtrlId 	: WIDC_LEVEL - 1
  CtrlId 	: WIDC_EXP - 90
  CtrlId 	: WIDC_POINT - 0
****************************************************/


//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndPartyInfo::CWndPartyInfo() 
:	m_nSelected( -1 )
,	m_pVBGauge( NULL )
,	m_pGauEmptyNormal( NULL ) 
,	m_pGauFillNormal( NULL )
{ 
} 


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndPartyInfo::~CWndPartyInfo() 
{ 
	DeleteDeviceObjects();
} 

//---------------------------------------------------------------------------------------------
// 장치 오브젝트 회복하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HRESULT CWndPartyInfo::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();

	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 무효화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HRESULT CWndPartyInfo::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 삭제
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HRESULT CWndPartyInfo::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 윈도우 아이디
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndPartyInfo::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_INFO, 0, CPoint( 0, 0 ), pWndParent );
} 


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndPartyInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	RestoreDeviceObjects();

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_texGauEmptyNormal.LoadTexture(	m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("GauEmptyNormal.bmp") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_texGauFillNormal.LoadTexture(		m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("GauEmptyNormal.bmp") ), WNDCOLOR_DEFAULT_KEY, TRUE );
	}
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_pGauEmptyNormal	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("BarBg.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_pGauFillNormal	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("BarRed.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
	}

	CRect	rectRoot( m_pWndRoot->GetLayoutRect() );
	CRect	rectWindow( GetWindowRect() );
	CPoint	point( rectRoot.right - rectWindow.Width(), 110 );

	Move( point );
	MoveParentCenter();
} 


//---------------------------------------------------------------------------------------------
// 개요 체크
// param	: void
// return	: BOOL 
//---------------------------------------------------------------------------------------------
BOOL CWndPartyInfo::CheckSummary()
{
	BOOL bSummary( FALSE );
	int nLang = g_xFlyffConfig->GetMainLanguage();

	switch( nLang )
	{
		case LANG_GER:
		case LANG_FRE:
			bSummary = TRUE;
			break;
	}

	return bSummary;
}


//---------------------------------------------------------------------------------------------
// 상태에 따른 색상 얻기
// param	: 멤버, 멤버 인덱스
// return	: void 
//---------------------------------------------------------------------------------------------
DWORD CWndPartyInfo::GetFillColorofStatus( CMover* pMember, UINT nMemberIndex )
{
	PFLCOLOR_WNDPARTYINFO	pColorPartyInfo( g_FLColorManager->GetWndPartyInfo() ); 
	DWORD					dwFillColor( pColorPartyInfo->m_stStatusBoxFill_Normal.GetColor() );	// 기본

	if( IsValidObj( pMember ) )
	{
		if( pMember->GetHitPoint() == 0 ) 
			dwFillColor = pColorPartyInfo->m_stStatusBoxFill_Die.GetColor();						// 죽음
		else 
		{
			if( pMember->GetMaxHitPoint() > 0 && ( (FLOAT)pMember->GetHitPoint() ) / ( (FLOAT)pMember->GetMaxHitPoint() ) <.1f ) 
				dwFillColor = pColorPartyInfo->m_stStatusBoxFill_LowHP.GetColor();					// HP 낮음
		}
	}
	else
	{
		dwFillColor = pColorPartyInfo->m_stStatusBoxFill_NotFoundFromAround.GetColor();				// 주위에 없음

		if( g_Party.m_aMember[nMemberIndex].m_bRemove ) 
			dwFillColor = pColorPartyInfo->m_stStatusBoxFill_Disconnect.GetColor();
	}

	return dwFillColor;
}


//---------------------------------------------------------------------------------------------
// 상태 박스 그리기
// param	: 렌더러, 멤버, 멤버 인덱스, 높이 시작위치
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndPartyInfo::DrawStatusBox( C2DRender* p2DRender, CMover* pMember, UINT nMemberIndex, UINT nPosY )
{
	BYTE					byBoxPosX( 10 );
	BYTE					byBoxSideSize( 10 );

	CRect					rtBox( byBoxPosX, nPosY, byBoxPosX + byBoxSideSize, nPosY + byBoxSideSize );

	// 아웃 라인 그리기
	PFLCOLOR_WNDPARTYINFO	pColorPartyInfo( g_FLColorManager->GetWndPartyInfo() ); 
	p2DRender->RenderFillRect( rtBox, pColorPartyInfo->m_stStatusBoxOutLine.GetColor() );	

	rtBox.left		= rtBox.left	+ 1;
	rtBox.top		= rtBox.top		+ 1;
	rtBox.right		= rtBox.right	- 1;
	rtBox.bottom	= rtBox.bottom	- 1;

	// 상태 체크
	DWORD dwFillColor = GetFillColorofStatus( pMember, nMemberIndex );
	p2DRender->RenderFillRect( rtBox, dwFillColor );
}


//---------------------------------------------------------------------------------------------
// 멤버 정보 그리기
// param	: 렌더러, 멤버, 높이 시작위치
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndPartyInfo::DrawMemberInfo( C2DRender* p2DRender, CMover* pMember, UINT nMemberIndex, UINT nPosY )
{
	PFLCOLOR_WNDPARTYINFO		pColorPartyInfo( g_FLColorManager->GetWndPartyInfo() ); 
	PFONTCOLOR_WNDPARTYINFO		pFontColorWndPartyInfo = g_WndFontColorManager->GetWndPartyInfo();

	CString						strName( _T("") );
	CString						strTemp( _T("") );
	CString						strClass( _T("") );

	DWORD						dwTextColor( m_dwColor );	
	BOOL						bSummary( CheckSummary() );

	static const int			nString1st_PosX( 30 );				
	static const int			nString2nd_PosX( 80 );
	static const int			nString3rd_PosX( 120 );
	static const int			nString4th_PosX( 210 );

	static const BYTE			byPlayerNameMaxCount( 8 );

	// 리더 체크
	if( g_Party.IsLeader( g_Party.m_aMember[nMemberIndex].m_uPlayerId ) ) 
	{
		strClass = prj.GetText( TID_GAME_LEADER );	
		dwTextColor = pFontColorWndPartyInfo->m_stLeader.GetFontColor();
	}
	else
		strClass = prj.GetText( TID_GAME_MEMBER );	

	if( IsValidObj(pMember) )
	{
		if( pMember->GetHitPoint() == 0 ) 
			dwTextColor = pColorPartyInfo->m_stStatusBoxFill_Die.GetColor();						// 죽음
		else 
		{
			if( pMember->GetMaxHitPoint() > 0 && ( (FLOAT)pMember->GetHitPoint() ) / ( (FLOAT)pMember->GetMaxHitPoint() ) <.1f ) 
				dwTextColor = pColorPartyInfo->m_stStatusBoxFill_LowHP.GetColor();					// HP 낮음
		}

		if( MAX_PROFESSIONAL <= pMember->GetJob() &&
			pMember->GetJob() < MAX_MASTER )
			strTemp.Format( "%d%s", pMember->GetLevel(), prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
		else if( MAX_MASTER <= pMember->GetJob() )
			strTemp.Format( "%d%s", pMember->GetLevel(), prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
		else 
			strTemp.Format( "%d", pMember->GetLevel() );

		p2DRender->TextOut( nString1st_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );				// 레벨, 계급

		if( !bSummary )
		{
			strTemp.Format( "%s", strClass );
			p2DRender->TextOut( nString2nd_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );			// 단장, 멤버
			strTemp.Format( "%s", pMember->GetJobString() );
			p2DRender->TextOut( nString3rd_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );			// 직업
		}
		else
		{
			strTemp.Format( "%s", pMember->GetJobString() );
			p2DRender->TextOut( nString2nd_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );			// 직업
		}

		strName = pMember->GetName();
	}
	else
	{
		PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[nMemberIndex].m_uPlayerId );
		FLERROR_ASSERT_LOG_RETURN_VOID( pPlayerData, _T("[윈도우 파티 정보(극단)] 플레이어 정보를 찾을 수 없습니다 - ID : [ %u]"), g_Party.m_aMember[nMemberIndex].m_uPlayerId );

		dwTextColor = pColorPartyInfo->m_stStatusBoxFill_NotFoundFromAround.GetColor();				// 주위에 없음

		if( g_Party.m_aMember[nMemberIndex].m_bRemove ) 
			dwTextColor = pColorPartyInfo->m_stStatusBoxFill_Disconnect.GetColor();

		UINT nJob( pPlayerData->data.nJob );
		UINT nLevel( pPlayerData->data.nLevel );

		if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
			strTemp.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
		else if( MAX_MASTER <= nJob )
			strTemp.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
		else 
			strTemp.Format( "%d", nLevel );
		
		p2DRender->TextOut( nString1st_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );
		
		if( !bSummary )
		{
			strTemp.Format( "%s", strClass );
			p2DRender->TextOut( nString2nd_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor);
			strTemp.Format( "%s",prj.m_aJob[nJob].szName);
			p2DRender->TextOut( nString3rd_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor);
		}
		else
		{
			strTemp.Format( "%s", prj.m_aJob[nJob].szName );
			p2DRender->TextOut( nString2nd_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );
		}
		strName	= pPlayerData->szPlayer;
	}

	// 긴 이름 처리
	if( strName.GetLength() > byPlayerNameMaxCount ) 
	{
		int	nReduceCount( NULL );

		for( nReduceCount = 0; nReduceCount < byPlayerNameMaxCount; )
		{
			if( IsDBCSLeadByte( strName[ nReduceCount ] ) )
				nReduceCount+=2;
			else
				nReduceCount++;
		}

		strName = strName.Left( nReduceCount );		// 10바이트째에서 한글이 두동강날거같으면 9바이트에서 짜르기
		strName+="...";								// ... 추가
	}

	strTemp.Format("%s",strName);
	p2DRender->TextOut( nString4th_PosX, nPosY, 1.0f, 1.0f, strTemp, dwTextColor );
}


//---------------------------------------------------------------------------------------------
// 게이지 그리기
// param	: 렌더러, 멤버, 높이 시작위치
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndPartyInfo::DrawGauge(C2DRender* p2DRender, CMover* pMember, UINT nPosY)
{
	CRect					rtGauge( 0, 0, 0, 0 );
	CRect					rtGaugeBg( 0, 0, 0, 0 );
	BOOL					bSummary( CheckSummary() );
	static const WORD		wGaugeWidth( 95 );

	// HP 게이지
	if( !bSummary )
		rtGauge.SetRect( 310, nPosY - 2, 310 + wGaugeWidth, nPosY + 12 );
	else
		rtGauge.SetRect( 310, nPosY - 2, 310 + wGaugeWidth, nPosY + 12 );	// 오른쪽으로 30 이동

	rtGaugeBg = rtGauge; 

	if( IsValidObj( pMember ) )
	{
		int		nOffsetRight( NULL );
		int		nCurrentHitPoint( pMember->GetHitPoint() );
		int		nMaxHitPoint( pMember->GetMaxHitPoint() );

		if( nCurrentHitPoint > nMaxHitPoint )
			nCurrentHitPoint = nMaxHitPoint;

		nOffsetRight = nCurrentHitPoint * wGaugeWidth / nMaxHitPoint;
		rtGauge.right = rtGauge.left + nOffsetRight;
	}

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_pTheme->RenderGauge( p2DRender, &rtGaugeBg, WNDCOLOR_WHITE, m_pVBGauge, m_pGauEmptyNormal );
		m_pTheme->RenderGauge( p2DRender, &rtGauge, 0x64ff0000, m_pVBGauge, m_pGauFillNormal );
	}			

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		if( m_pGauEmptyNormal )
			m_pTheme->RenderGauge( p2DRender, &rtGaugeBg, WNDCOLOR_WHITE, m_pVBGauge, m_pGauEmptyNormal );

		if( IsValidObj(pMember) )
		{
			if( m_pGauFillNormal )
				m_pTheme->RenderGauge( p2DRender, &rtGauge, WNDCOLOR_WHITE, m_pVBGauge, m_pGauFillNormal );
		}
	}
}


//---------------------------------------------------------------------------------------------
// 파티 정보 그리기 ( 극단 )
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndPartyInfo::OnDraw( C2DRender* p2DRender ) 
{
	PFLCOLOR_WNDPARTYINFO	pColorPartyInfo( g_FLColorManager->GetWndPartyInfo() ); 

	// 현재 선택된 놈 표시
	if( m_nSelected != -1 ) 
	{
		CRect rect( 5, 8 + m_nSelected * 15, 410, 22 + m_nSelected * 15 );
		p2DRender->RenderFillRect( rect, pColorPartyInfo->m_stSelectMember.GetColor() );
	}

	// 파티원 정보 출력
	if( g_Party.GetSizeofMember() < m_nSelected + 1 ) 
		m_nSelected = -1;

	int nStartPosY( 10 );
	for( int i = 0; i < g_Party.m_nSizeofMember; i++ ) 
	{		
		CMover*	pObjMember( prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId ) );

		DrawStatusBox( p2DRender, pObjMember, i, nStartPosY );
		DrawMemberInfo( p2DRender, pObjMember, i, nStartPosY );	
		DrawGauge( p2DRender, pObjMember, nStartPosY );
	
		nStartPosY += 15; // 다음 줄
	}
} 




void CWndPartyInfo::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
//sun: 11, 타겟 표시 방법 변경
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if(((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;
	
	// 파티창에서 어떤놈 눌렀는지 색출
	CRect rect( 10, 10, 365, 10 + g_Party.GetSizeofMember() * 15 );
	if( rect.PtInRect( point ) ) 
		m_nSelected = ( point.y - 10 ) / 15;
	else 
		m_nSelected = -1;
	if( m_nSelected != -1 ) 
	{ // 선택된놈 있으면
		// 그놈을 타겟으로

		//sun: 8차 파티창 관련 적용 Neuz
		((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL; //마우스에 걸려 그려진놈을 지우고 타겟을 잡아야 안겹친다..

		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[m_nSelected].m_uPlayerId );
		if(g_pPlayer!=pObjMember) 
		{
			if(IsValidObj(pObjMember)) 
			{ // 화면에 없는놈은 패스...
				g_WorldMng()->SetObjFocus(pObjMember);
				CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
				if(pWndWorld)
					pWndWorld->m_pRenderTargetObj = NULL;
			}
		}
		else
			g_WorldMng()->SetObjFocus( NULL );	//sun: 8차 파티창 관련 적용 Neuz
	}
	else 
	{
		// 없으면 타겟 지운다
		g_WorldMng()->SetObjFocus(NULL);
	}
} 


#pragma endregion WndPartyInfo



/****************************************************
  WndId : APP_PARTY_SKILL - 극단스킬
****************************************************/

CWndPartySkill::CWndPartySkill() 
{ 
	ZeroMemory( m_atexSkill, sizeof( m_atexSkill ) );
	m_nSkillSelect = -1;
} 
CWndPartySkill::~CWndPartySkill() 
{ 
} 
void CWndPartySkill::OnDraw( C2DRender* p2DRender ) 
{ 
//	if( g_Party.m_nKindTroup )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		int nCount = 0;
		
		PFONTCOLOR_WNDPARTYSKILL pFontColorWndPartySkill = g_WndFontColorManager->GetWndPartySkill();
		
		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 3; j++, nCount++ )
			{
				SkillProp* pSkillProp =  prj.GetPartySkill( nCount + 1 );
				if( pSkillProp ) //&& g_Party.m_nSizeofMember >= 2 )
				{
					m_atexSkill[ i * 3 + j ] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, FALSE );
					CPoint point1 = CPoint( j * nWidth + 35 , i * nHeight + 6 );
					CPoint point2 = CPoint( j * nWidth + 3, i * nHeight + 3 );
					
					if( g_Party.GetKind() == PARTY_KIND_EXTEND && int(g_Party.GetLevel() - pSkillProp->dwReqDisLV) >= 0 )
					{
						if( (g_Party.GetPoint() - pSkillProp->dwReqPartyPoint) >= 0 )
						{
							// 사용가능한 기술
							p2DRender->TextOut( point1.x, point1.y, pSkillProp->szName, m_dwColor );
							//m_aSlotQueue[nIndex].m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, lpSkill->GetProp()->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff, FALSE );
							p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ] );
							
						}
						else
						{
							// 기술은 익혔으나 포인트가 모자라 사용을 못함
							p2DRender->TextOut( point1.x, point1.y, pSkillProp->szName, pFontColorWndPartySkill->m_stlessPoint.GetFontColor() );
							p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ] );
							
						}
					}
					else
					{
						// 기술을 못익혔음
						p2DRender->TextOut( point1.x, point1.y, pSkillProp->szName, pFontColorWndPartySkill->m_stNoLearnSkill.GetFontColor() );
						p2DRender->RenderTexture( point2, m_atexSkill[ i * 3 + j ], 54 );		
					}
				}
			}
		}
	}
} 
void CWndPartySkill::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	point.x /= nWidth; 
	point.y /= nHeight;
	int nSkill = point.y * 3 + point.x;
	
	SkillProp* pSkillProp =  prj.GetPartySkill( nSkill + 1 );
	if( pSkillProp )
	{
		m_nSkillSelect = nSkill;
		return;
	}
	m_nSkillSelect = -1;
} 
void CWndPartySkill::OnMouseWndSurface( CPoint point )
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 3;
	int nHeight = rect.Height() / 3;
	int nCount = 0;
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++, nCount++ )
		{
			SkillProp* pSkillProp =  prj.GetPartySkill( nCount + 1 );
			if( pSkillProp ) //&& g_Party.m_nSizeofMember >= 2 )
			{
				CRect rect( j * nWidth + 3, i * nHeight + 3, j * nWidth + 3 + nWidth, i * nHeight + 3 + nHeight);
				if( rect.PtInRect( point ) )
				{
					CPoint point2 = point;
					ClientToScreen( &point2 );
					ClientToScreen( &rect );
					
					g_WndMng.PutToolTip_Troupe( i * 3 + j + 1, point2, &rect );
				}
			}
		}
	}
}	
void CWndPartySkill::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_nSkillSelect = -1;
}

void CWndPartySkill::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_nSkillSelect == -1 || IsPush() == FALSE )
		return;
	//	FLTRACE_LOG( PROGRAM_NAME, _T( " hello " ) );
	CPoint pt( 3, 3 );
	CRect rect;
	//if( m_rect.PtInRect( point ) )
	//{
	//DWORD dwSkill = m_apJobSkill[ m_nCurSelect ].dwSkill;
	SkillProp* pSkillProp =  prj.GetPartySkill( m_nSkillSelect + 1 );
	if( pSkillProp ) //&& g_Party.m_nSizeofMember >= 2 )
	{
		if( int(g_Party.GetLevel() - pSkillProp->dwReqDisLV) >= 0 )
		{
			//FLItemElem* pItemElem = &m_pItemContainer->m_apItem[ m_pItemContainer->m_apIndex[ m_nCurSelect ] ] ;
			m_GlobalShortcut.m_dwShortcut  = SHORTCUT_SKILL;
			m_GlobalShortcut.m_dwType  = 2;//m_nJob;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwIndex = m_nSkillSelect + 1;//m_nSkillSelect;//m_nCurSelect;//(DWORD)pItemElem;//->m_dwItemId;
			m_GlobalShortcut.m_dwData  = 0;//pItemElem->m_dwObjId;//(DWORD)pItemElem;
			m_GlobalShortcut.m_dwId     = m_nSkillSelect + 1; // 컬런트 셀렉트가 곧 ID나 마찬가지임.
			m_GlobalShortcut.m_pTexture = m_atexSkill[ m_nSkillSelect ];//L;//pItemElem->m_pTexture;
			FLStrcpy( m_GlobalShortcut.m_szString, _countof( m_GlobalShortcut.m_szString ), pSkillProp->szName);
			//}
		}
	}
}

void CWndPartySkill::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPartySkill::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_SKILL, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndPartySkill::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndPartySkill::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPartySkill::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPartySkill::OnLButtonDblClk( UINT nFlags, CPoint point)
{
//	if( g_Party.m_nSizeofMember >= 2 && g_Party.m_nKindTroup == PARTY_KIND_EXTEND )
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 3;
		int nHeight = rect.Height() / 3;
		point.x /= nWidth; 
		point.y /= nHeight;
		int nSkill = point.y * 3 + point.x;
		
		SkillProp* pSkillProp =  prj.GetPartySkill( nSkill + 1 );
		if( pSkillProp )
		{
			g_WndMng.ObjectExecutor( SHORTCUT_SKILL, nSkill + 1, 2 );
		}
	}
}
BOOL CWndPartySkill::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
