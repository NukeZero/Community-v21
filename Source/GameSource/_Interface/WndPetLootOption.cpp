#include "stdafx.h"
#include "resData.h"
#include "WndPetLootOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/****************************************************
  WndId : APP_PET_LOOT_OPTION - 줍기펫
  CtrlId : WIDC_BT_APPLY - 적용
  CtrlId : WIDC_BT_CANCEL - 취소
  CtrlId : WIDC_CHECK1 - 물약
  CtrlId : WIDC_CHECK2 - 장비
  CtrlId : WIDC_CHECK3 - 기타
  CtrlId : WIDC_STATIC1 - 줍기 옵션
****************************************************/

#ifdef PAT_LOOTOPTION
CWndPetLootOption::CWndPetLootOption() : m_nLootType ( g_Option.m_nLootType )
{
} 
CWndPetLootOption::~CWndPetLootOption() 
{ 
} 
void CWndPetLootOption::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndPetLootOption::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndButton* pWndOption1 = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndOption2 = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndOption3 = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndOption4 = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	CWndButton* pWndOption5 = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	CWndButton* pWndOption6 = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
	CWndButton* pWndOption7 = (CWndButton*)GetDlgItem( WIDC_CHECK7 );
	CWndButton* pWndOption8 = (CWndButton*)GetDlgItem( WIDC_CHECK8 );
	CWndButton* pWndOption9 = (CWndButton*)GetDlgItem( WIDC_CHECK9 );
	CWndButton* pWndOption10 = (CWndButton*)GetDlgItem( WIDC_CHECK10 );
	CWndButton* pWndOption11 = (CWndButton*)GetDlgItem( WIDC_CHECK11 );
	CWndButton* pWndOption12 = (CWndButton*)GetDlgItem( WIDC_CHECK12 );
	CWndButton* pWndOption13 = (CWndButton*)GetDlgItem( WIDC_CHECK13 );
	CWndButton* pWndOption14 = (CWndButton*)GetDlgItem( WIDC_CHECK14 );

	
	if (m_nLootType & 0x1) pWndOption1->SetCheck(TRUE);
	if (m_nLootType & 0x2) pWndOption2->SetCheck(TRUE);
	if (m_nLootType & 0x4) pWndOption3->SetCheck(TRUE);
	if (m_nLootType & 0x8) pWndOption4->SetCheck(TRUE);
	if (m_nLootType & 0x10) pWndOption5->SetCheck(TRUE);
	if (m_nLootType & 0x20) pWndOption6->SetCheck(TRUE);
	if (m_nLootType & 0x40) pWndOption7->SetCheck(TRUE);
	if (m_nLootType & 0x80) pWndOption8->SetCheck(TRUE);
	if (m_nLootType & 0x100) pWndOption9->SetCheck(TRUE);
	if (m_nLootType & 0x200) pWndOption10->SetCheck(TRUE);
	if (m_nLootType & 0x400) pWndOption11->SetCheck(TRUE);
	if (m_nLootType & 0x800) pWndOption12->SetCheck(TRUE);
	if (m_nLootType & 0x1000) pWndOption13->SetCheck(TRUE);
	if (m_nLootType & 0x2000) pWndOption14->SetCheck(TRUE);

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndPetLootOption::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PET_LOOT_OPTION, 0, CPoint( 0, 0 ), pWndParent );
}

  //직접 윈도를 열때 사용 
/*BOOL CWndPetLootOption::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
}*/
void CWndPetLootOption::ReceivedPacket()
{
	Initialize (&g_WndMng,APP_PET_LOOT_OPTION);
}


BOOL CWndPetLootOption::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPetLootOption::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndPetLootOption::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndPetLootOption::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndPetLootOption::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	CWndButton* pWndOption1 = (CWndButton*)GetDlgItem( WIDC_CHECK1 );
	CWndButton* pWndOption2 = (CWndButton*)GetDlgItem( WIDC_CHECK2 );
	CWndButton* pWndOption3 = (CWndButton*)GetDlgItem( WIDC_CHECK3 );
	CWndButton* pWndOption4 = (CWndButton*)GetDlgItem( WIDC_CHECK4 );
	CWndButton* pWndOption5 = (CWndButton*)GetDlgItem( WIDC_CHECK5 );
	CWndButton* pWndOption6 = (CWndButton*)GetDlgItem( WIDC_CHECK6 );
	CWndButton* pWndOption7 = (CWndButton*)GetDlgItem( WIDC_CHECK7 );
	CWndButton* pWndOption8 = (CWndButton*)GetDlgItem( WIDC_CHECK8 );
	CWndButton* pWndOption9 = (CWndButton*)GetDlgItem( WIDC_CHECK9 );
	CWndButton* pWndOption10 = (CWndButton*)GetDlgItem( WIDC_CHECK10 );
	CWndButton* pWndOption11 = (CWndButton*)GetDlgItem( WIDC_CHECK11 );
	CWndButton* pWndOption12 = (CWndButton*)GetDlgItem( WIDC_CHECK12 );
	CWndButton* pWndOption13 = (CWndButton*)GetDlgItem( WIDC_CHECK13 );
	CWndButton* pWndOption14 = (CWndButton*)GetDlgItem( WIDC_CHECK14 );

	switch ( nID )
	{
	case WIDC_CHECK1:
		if( pWndOption1->GetCheck() )		m_nLootType = m_nLootType | 0x1;
		else								m_nLootType = m_nLootType & 0xFFFE;
		break;
	case WIDC_CHECK2:
		if( pWndOption2->GetCheck() )		m_nLootType = m_nLootType | 0x2;
		else								m_nLootType = m_nLootType & 0xFFFD;
		break;
	case WIDC_CHECK3:
		if( pWndOption3->GetCheck() )		m_nLootType = m_nLootType | 0x4;
		else								m_nLootType = m_nLootType & 0xFFFB;
		break;
	case WIDC_CHECK4:
		if( pWndOption4->GetCheck() )		m_nLootType = m_nLootType | 0x8;
		else								m_nLootType = m_nLootType & 0xFFF7;
		break;
	case WIDC_CHECK5:
		if( pWndOption5->GetCheck() )		m_nLootType = m_nLootType | 0x10;
		else								m_nLootType = m_nLootType & 0xFFEF;
		break;
	case WIDC_CHECK6:
		if( pWndOption6->GetCheck() )		m_nLootType = m_nLootType | 0x20;
		else								m_nLootType = m_nLootType & 0xFFDF;
		break;
	case WIDC_CHECK7:
		if( pWndOption7->GetCheck() )		m_nLootType = m_nLootType | 0x40;
		else								m_nLootType = m_nLootType & 0xFFBF;
		break;
	case WIDC_CHECK8:
		if( pWndOption8->GetCheck() )		m_nLootType = m_nLootType | 0x80;
		else								m_nLootType = m_nLootType & 0xFF7F;
		break;
	case WIDC_CHECK9:
		if( pWndOption9->GetCheck() )		m_nLootType = m_nLootType | 0x100;
		else								m_nLootType = m_nLootType & 0xFEFF;
		break;
	case WIDC_CHECK10:
		if( pWndOption10->GetCheck() )		m_nLootType = m_nLootType | 0x200;
		else								m_nLootType = m_nLootType & 0xFDFF;
		break;
	case WIDC_CHECK11:
		if( pWndOption11->GetCheck() )		m_nLootType = m_nLootType | 0x400;
		else								m_nLootType = m_nLootType & 0xFBFF;
		break;
	case WIDC_CHECK12:
		if( pWndOption12->GetCheck() )		m_nLootType = m_nLootType | 0x800;
		else								m_nLootType = m_nLootType & 0xF7FF;
		break;
	case WIDC_CHECK13:
		if( pWndOption13->GetCheck() )		m_nLootType = m_nLootType | 0x1000;
		else								m_nLootType = m_nLootType & 0xEFFF;
		break;
	case WIDC_CHECK14:
		if( pWndOption14->GetCheck() )		m_nLootType = m_nLootType | 0x2000;
		else								m_nLootType = m_nLootType & 0xDFFF;
		break;
	case WIDC_APPLY:
		g_Option.m_nLootType = m_nLootType;
		g_DPlay.SendPetLootOption(m_nLootType);
		Destroy();
		break;
	case WIDC_CANCEL:
		Destroy();
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

#endif // PAT_LOOTOPTION