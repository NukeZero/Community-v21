
#include "StdAfx.h"

#include "WndPartyQuick.h"
#include "Party.h"
#include "WndParty.h"
#include "WndManager.h"
#include "PlayerData.h"		

int StaticID[MAX_PARTY_MEMBER] = {	WIDC_STATIC_MEM1,
									WIDC_STATIC_MEM2,
									WIDC_STATIC_MEM3,
									WIDC_STATIC_MEM4,
									WIDC_STATIC_MEM5,
									WIDC_STATIC_MEM6,
									WIDC_STATIC_MEM7,
									WIDC_STATIC_MEM8	};

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndPartyQuick::CWndPartyQuick() 
:	m_pVBGauge( NULL )
,	m_MemberCount( NULL )
,	m_pWndParty( NULL )
,	m_pFocusMember( NULL )
,	m_pBtnParty( NULL )
,	m_pGauEmptyNormal( NULL )
,	m_pGauFillNormal( NULL )
{ 
	ZeroMemory( m_pWndMemberStatic, sizeof( CWndStatic* ) * MAX_PARTY_MEMBER );
} 


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndPartyQuick::~CWndPartyQuick() 
{ 
	DeleteDeviceObjects();
} 


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 회복하기
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HRESULT CWndPartyQuick::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 무효화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HRESULT CWndPartyQuick::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVBGauge );

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// 장치 오브젝트 삭제
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
HRESULT CWndPartyQuick::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );

	return S_OK;
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 윈도우 아이디
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndPartyQuick::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	if( g_Party.GetSizeofMember() < 2 ) // 시작과 함께 열릴 경우 Quick창은 열지 않도록 한다. Party정보가 없어 세팅 불가.
		return FALSE;

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_QUICK, 0, CPoint( 0, 0 ), pWndParent );
} 


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndPartyQuick::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pBtnParty = (CWndButton*)GetDlgItem( WIDC_BUTTON1 );

	for(int i=0; i<MAX_PARTY_MEMBER; i++)
		m_pWndMemberStatic[i] = (CWndStatic*)GetDlgItem( StaticID[i] );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_texGauEmptyNormal.LoadTexture(	m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("GauEmptySmall.bmp") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_texGauFillNormal.LoadTexture(		m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("GauFillSmall.bmp") ), WNDCOLOR_DEFAULT_KEY, TRUE );
	}
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		m_pGauEmptyNormal	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("BarBg.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
		m_pGauFillNormal	= m_textureMng.AddTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("BarRed.tga") ), WNDCOLOR_DEFAULT_KEY, TRUE );
	}
	
	SetActiveMember( g_Party.m_nSizeofMember );
	
	CRect	rectRoot( m_pWndRoot->GetLayoutRect() );
	CPoint	point( 30, (rectRoot.bottom - rectRoot.top) / 4);
	Move( point );
} 


void CWndPartyQuick::OnDraw( C2DRender* p2DRender ) 
{ 
	PFLCOLOR_WNDPARTYINFO		pColorPartyInfo( g_FLColorManager->GetWndPartyInfo() ); 
	PFONTCOLOR_WNDPARTYINFO		pFontColorWndPartyInfo = g_WndFontColorManager->GetWndPartyInfo();

	int							nMax( g_Party.m_nSizeofMember );
	CRect						rtMember( 0, 0, 0, 0 );
	DWORD						dwColor( NULL ); 		

	SetActiveMember(nMax);

	for( int nIndex = 0; nIndex < g_Party.m_nSizeofMember; nIndex++ )
	{
		dwColor = m_dwColor;
		
		LPWNDCTRL lpWndCtrl = GetWndCtrl( StaticID[nIndex] );
		FLERROR_ASSERT_LOG_DO( lpWndCtrl, continue, _T("[윈도우 파티 퀵] 윈도우 컨트롤 포인터가 유효하지 않습니다.") );
		
		rtMember = lpWndCtrl->rect;

		CString strMember( _T("") );
		CMover* pObjMember = prj.GetUserByID( g_Party.m_aMember[nIndex].m_uPlayerId );
		
		if( m_pFocusMember == pObjMember )
			p2DRender->RenderFillRect( rtMember, pColorPartyInfo->m_stSelectMember.GetColor() );

		u_long  nLeadMember = g_Party.m_aMember[nIndex].m_uPlayerId;
		if( IsValidObj(pObjMember) )
		{

			if(g_Party.IsLeader(nLeadMember)) 
			{
				dwColor = pFontColorWndPartyInfo->m_stLeader.GetFontColor();

				if( pObjMember->GetHitPoint() == 0 ) 
				{
					dwColor = pColorPartyInfo->m_stStatusBoxFill_Die.GetColor();
				}
				else if( ((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) <.1f )
				{
					dwColor = pColorPartyInfo->m_stStatusBoxFill_LowHP.GetColor();
				}

				if(pObjMember->IsMaster())
				{
					strMember.Format( prj.GetText( TID_GAME_QUICK_MARK_MASTER ), pObjMember->GetLevel(), pObjMember->GetName() );
				}
				else if(pObjMember->IsHero())
				{
					strMember.Format( prj.GetText( TID_GAME_QUICK_MARK_HERO ), pObjMember->GetLevel(), pObjMember->GetName() );
				}
				else if( pObjMember->IsLegendHero( ) )
				{
					strMember.Format( prj.GetText( TID_GAME_QUICK_MARK_HERO ), pObjMember->GetLevel(), pObjMember->GetName() );
				}
				else 
				{
					strMember.Format( "%d. %s", pObjMember->GetLevel(), pObjMember->GetName() );
				}
			}
			else
			{
				if( pObjMember->GetHitPoint() == 0 ) 
				{
					dwColor = pColorPartyInfo->m_stStatusBoxFill_Die.GetColor();
				}
				else if( ((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) <.1f ) 
				{
					dwColor = pColorPartyInfo->m_stStatusBoxFill_LowHP.GetColor();
				}
				
				if(pObjMember->IsMaster())
				{
					strMember.Format( prj.GetText( TID_GAME_QUICK_MARK_MASTER ), pObjMember->GetLevel(), pObjMember->GetName() );
				}
				else if(pObjMember->IsHero())
				{
					strMember.Format( prj.GetText( TID_GAME_QUICK_MARK_HERO ), pObjMember->GetLevel(), pObjMember->GetName() );
				}
				else if( pObjMember->IsLegendHero( ) )
				{
					strMember.Format( prj.GetText( TID_GAME_QUICK_MARK_HERO ), pObjMember->GetLevel(), pObjMember->GetName() );
				}
				else 
				{
					strMember.Format( "%d. %s", pObjMember->GetLevel(), pObjMember->GetName() );
				}
			}
		}
		else
		{
			dwColor = pColorPartyInfo->m_stStatusBoxFill_NotFoundFromAround.GetColor(); // 디폴트는 주위에 없는놈

			if( g_Party.m_aMember[nIndex].m_bRemove ) 
				dwColor = pColorPartyInfo->m_stStatusBoxFill_Disconnect.GetColor();

			const char* pszPlayer	= CPlayerDataCenter::GetInstance()->GetPlayerString( g_Party.m_aMember[nIndex].m_uPlayerId );

			CString	strTemp2( _T("") );
			PlayerData* pPlayerData	= CPlayerDataCenter::GetInstance()->GetPlayerData( g_Party.m_aMember[nIndex].m_uPlayerId );
			int nLevel	= pPlayerData->data.nLevel;
			int nJob	= pPlayerData->data.nJob;
		
			if( MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER )
				strTemp2.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_MASTER ) );
			else if( MAX_MASTER <= nJob )
				strTemp2.Format( "%d%s", nLevel, prj.GetText( TID_GAME_TOOLTIP_MARK_HERO ) );
			else 
				strTemp2.Format( "%d", nLevel );

			if(g_Party.IsLeader(nLeadMember)) //Leader Set 굵게 해야함...
				strMember.Format( "%s. %s", strTemp2,pszPlayer );
			else
				strMember.Format( "%s. %s", strTemp2,pszPlayer );

		}
		//Member - Level, Name Draw
		//긴 이름은 ... 으로.
		if( strMember.GetLength() > 13 ) 
		{
			int	nReduceCount = 0;

			for( nReduceCount=0; nReduceCount <13; )
			{
				if( IsDBCSLeadByte( strMember[ nReduceCount ] ) )
					nReduceCount+=2;
				else
					nReduceCount++;
			}

			strMember = strMember.Left( nReduceCount );
			strMember+="...";
		}

		p2DRender->TextOut( rtMember.TopLeft().x+5, rtMember.TopLeft().y+5, strMember, dwColor );

		//Member - Gauge Draw
		rtMember.TopLeft().y += 18;
		rtMember.TopLeft().x += 2;
		rtMember.BottomRight().x -= 2;
		int nWidth	= pObjMember ? pObjMember->GetHitPointPercent( rtMember.Width() ) : 0;
		CRect rectTemp = rtMember; 
		rectTemp.right = rectTemp.left + nWidth;
		if( rtMember.right < rectTemp.right )
			rectTemp.right = rtMember.right;

		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			m_pTheme->RenderGauge( p2DRender, &rtMember, WNDCOLOR_WHITE, m_pVBGauge, &m_texGauEmptyNormal );
			m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );

		}
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			if( m_pGauEmptyNormal )
				m_pTheme->RenderGauge( p2DRender, &rtMember, WNDCOLOR_WHITE, m_pVBGauge, m_pGauEmptyNormal );

			if( IsValidObj(pObjMember) )
			{
				if( m_pGauFillNormal )
					m_pTheme->RenderGauge( p2DRender, &rectTemp, WNDCOLOR_WHITE, m_pVBGauge, m_pGauFillNormal );
			}
		}
	}
} 



void CWndPartyQuick::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	int nMax = g_Party.m_nSizeofMember;

//sun: 11, 타겟 표시 방법 변경
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if(((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;
	
	for(int i=0; i<nMax; i++) 
	{
		lpWndCtrl = GetWndCtrl( StaticID[i] );
		rect = lpWndCtrl->rect;
		if( rect.PtInRect( point ) )
		{
			((CWndWorld*)g_WndMng.m_pWndWorld)->m_pSelectRenderObj = NULL; //마우스에 걸려 그려진놈을 지우고 타겟을 잡아야 안겹친다..
			m_pFocusMember = prj.GetUserByID( g_Party.m_aMember[ i ].m_uPlayerId );
			if( g_pPlayer != m_pFocusMember ) 
			{
				if( IsValidObj( m_pFocusMember ) ) 
				{
					g_WorldMng()->SetObjFocus( m_pFocusMember );
					CWndWorld* pWndWorld = g_WndMng.m_pWndWorld;
					if(pWndWorld)
						pWndWorld->m_pRenderTargetObj = NULL;
				}
			}
			else
				g_WorldMng()->SetObjFocus( NULL );
			return;
		}
	}
} 



BOOL CWndPartyQuick::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 ) 
	{
		m_pWndParty = (CWndParty*)g_WndMng.GetApplet( APP_PARTY );
		if(m_pWndParty == NULL)
			m_pWndParty = (CWndParty*)g_WndMng.CreateApplet( APP_PARTY );
		else
		{
			m_pWndParty->Destroy();
			m_pWndParty = NULL;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndPartyQuick::SetActiveMember(int MemberNum)
{
	int i, height;
	CRect rect;
	
	if(m_MemberCount != MemberNum)
	{
		rect = GetWindowRect(TRUE);
		height = 60;

		for(i=0; i<MemberNum; i++)
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( StaticID[i] );
			lpWndStatic->EnableWindow(TRUE);
			lpWndStatic->SetVisible(TRUE);
			height += 34;
		}
		for(i=MemberNum; i<MAX_PARTY_MEMBER; i++)
		{
			CWndStatic* lpWndStatic;
			lpWndStatic = (CWndStatic*) GetDlgItem( StaticID[i] );
			lpWndStatic->EnableWindow(FALSE);
			lpWndStatic->SetVisible(FALSE);
		}
		rect.bottom = rect.top + height;
		//Button Move
		CRect btnRect;
		btnRect.top = height - 50;
		btnRect.bottom = btnRect.top + 10;
		btnRect.left = 114;
		btnRect.right = btnRect.left + 10;
		m_pBtnParty->SetWndRect(btnRect);

		SetWndRect(rect);
		AdjustWndBase();
		m_MemberCount = MemberNum;
	}
}

