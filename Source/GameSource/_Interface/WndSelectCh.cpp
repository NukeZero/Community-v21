#include "stdafx.h"

#include "WndSelectCh.h"

/****************************************************
  WndId : APP_SELECT_CHANNEL - 채널 선택
  CtrlId : WIDC_LISTBOX1 - Listbox
  CtrlId : WIDC_STATIC1 - GroupBox
****************************************************/

CWndSelectCh::CWndSelectCh() 
{ 
} 

CWndSelectCh::~CWndSelectCh() 
{ 
}

void CWndSelectCh::OnDraw( C2DRender* p2DRender ) 
{ 
}

void CWndSelectCh::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	InitializeTicketInformation();

	m_pWndListBox_Channel = NULL;
	m_pWndListBox_World = NULL;

	// 여기에 코딩하세요
 	m_pWndListBox_Channel = (CWndListBox*)GetDlgItem( WIDC_LB_CHANNEL );
	m_pWndListBox_World = (CWndListBox*)GetDlgItem( WIDC_LB_WORLD );
 
	//하나라도 세팅이 안되있으면 에러로그!
	if( !( m_pWndListBox_World && 
		m_pWndListBox_Channel) )
	{	
		FLERROR_LOG( PROGRAM_NAME, _T( "CWndSelectCh::OnInitialUpdate()" ) );
	}

	m_pWndListBox_Channel->ResetContent();
	m_pWndListBox_World->ResetContent();

	PT_CHARGE_ZONE_WORLD ptChargeZoneInfo = NULL;
	for( m_itr_TicketInfo = m_mapTicket.begin(); m_itr_TicketInfo != m_mapTicket.end(); ++m_itr_TicketInfo )
	{
		ptChargeZoneInfo = &(m_itr_TicketInfo->second);
		m_pWndListBox_World->AddString(ptChargeZoneInfo->szWorldName);
		m_vecWorldID.push_back(ptChargeZoneInfo->dwWorldID);
	}

	m_pWndListBox_World->SetOnSelectColor(D3DCOLOR_ARGB( 255, 45, 107, 180 ));
	m_pWndListBox_World->SetOnMouseColor(D3DCOLOR_ARGB( 255, 45, 107, 180 ));
	m_pWndListBox_World->SetCurSel(0);
	m_pWndListBox_World->SetStyleState(TRUE);

	int nSelect = 0;
	PT_CHARGE_ZONE_WORLD pSpec = NULL;
	nSelect = m_pWndListBox_World->GetCurSel();
	pSpec = g_xSpecManager->GetSpecChargeZoneWorldByWorldID(m_vecWorldID[nSelect]);
	if(pSpec != NULL)
	{
		CString strTitle;
		for(int i=0; i < (int)pSpec->byChannelCount ; ++i)
		{
			strTitle.Format( "%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i+1);
			m_pWndListBox_Channel->AddString(strTitle);
		}

		m_pWndListBox_Channel->SetOnSelectColor(D3DCOLOR_ARGB( 255, 45, 107, 180 ));
		m_pWndListBox_Channel->SetOnMouseColor(D3DCOLOR_ARGB( 255, 45, 107, 180 ));
		m_pWndListBox_Channel->SetCurSel(0);
		m_pWndListBox_Channel->SetStyleState(TRUE);
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSelectCh::Initialize( CWndBase* pWndParent, DWORD nType ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), nType, 0, CPoint( 0, 0 ), pWndParent );
} 
void CWndSelectCh::ReceivedPacket( FLSnapshotChargeZoneTicketEnteranceNoti* pkOpenTicketEnterance)
{
	if(pkOpenTicketEnterance->dwTicketObjID != NULL_ID)
	{
		m_dwItemId = pkOpenTicketEnterance->dwTicketObjID;
		Initialize( &g_WndMng, APP_WORLD_FREETICKET );
	}
}

void CWndSelectCh::InitializeTicketInformation()
{
	FLItemElem* pItem;
	pItem = g_pPlayer->m_Inventory.GetAtId( m_dwItemId );

	if(g_xSpecManager->GetSpecChargeZoneWorldByItemID( pItem->m_dwItemId ,&m_mapTicket ) == false)
	{
		return;
	}
}

BOOL CWndSelectCh::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndSelectCh::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndSelectCh::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndSelectCh::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndSelectCh::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	PT_CHARGE_ZONE_WORLD pSpec = NULL;

	switch(nID)
	{
	case WIDC_BT_MOVE:
		{
			pSpec = g_xSpecManager->GetSpecChargeZoneWorldByWorldID(m_vecWorldID[m_pWndListBox_World->GetCurSel()]);
			if(pSpec != NULL)
			{
				FLPacketChargeZoneTicketEnteranceReq	pPacket;
				pPacket.dwTicketObjID = m_dwItemId;
				pPacket.dwWorldID = pSpec->dwWorldID;
				pPacket.byChannel = m_pWndListBox_Channel->GetCurSel();
				g_DPlay.SendPacket(&pPacket);
				Destroy();
			}
		}
		break;
	case WIDC_BT_CANCEL:
		Destroy();
		break;
	case WIDC_LB_WORLD: // view ctrl 
		{
			pSpec = g_xSpecManager->GetSpecChargeZoneWorldByWorldID(m_vecWorldID[m_pWndListBox_World->GetCurSel()]);
			
			if( pSpec == NULL ) return FALSE;

			CString strTitle;
			m_pWndListBox_Channel->ResetContent();
			for(int i=0; i < (int)pSpec->byChannelCount ; ++i)
			{
				strTitle.Format( "%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i+1);
				m_pWndListBox_Channel->AddString(strTitle);
			}
			m_pWndListBox_Channel->SetCurSel(0);
			m_pWndListBox_Channel->SetStyleState(TRUE);
		}
		break;
	case WIDC_LB_CHANNEL: // view ctrl 
		{
			pSpec = g_xSpecManager->GetSpecChargeZoneWorldByWorldID(m_vecWorldID[m_pWndListBox_World->GetCurSel()]);

		}
		break;
	}

	switch(message)
	{
	case WNM_DBLCLK:
		{
			if(nID == WIDC_LB_CHANNEL)
			{
				if(pSpec != NULL)
				{
					FLPacketChargeZoneTicketEnteranceReq	pPacket;
					pPacket.dwTicketObjID = m_dwItemId;
					pPacket.dwWorldID = pSpec->dwWorldID;
					pPacket.byChannel = m_pWndListBox_Channel->GetCurSel();
					g_DPlay.SendPacket(&pPacket);
					Destroy();
				}
			}		
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
