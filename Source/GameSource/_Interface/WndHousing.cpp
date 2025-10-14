
#include "StdAfx.h"

#include "WndHousing.h"
#include "GuildHouse.h"
#include "WndGuildHouse.h"

/****************************************************
  WndId : APP_HOUSING - 하우징
  CtrlId : WIDC_BUTTON1 - 설치하기
  CtrlId : WIDC_BUTTON2 - 해체하기
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_STATIC1 - 설명
  CtrlId : WIDC_STATIC2 - 종류
  CtrlId : WIDC_STATIC3 - 아이템명
  CtrlId : WIDC_STATIC4 - 남은 시간
  CtrlId : WIDC_STATIC5 - 설치
  CtrlId : WIDC_LISTBOX1 - Listbox
****************************************************/

bool CompType_Greater( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	return first.m_nType > second.m_nType;
}
bool CompType_Smaller( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	return first.m_nType < second.m_nType;
}

bool CompDeploy_Greater( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	return first.m_bDeploy > second.m_bDeploy;
}
bool CompDeploy_Smaller( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	return first.m_bDeploy < second.m_bDeploy;
}

bool CompTime_Greater( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	return first.m_dwTime > second.m_dwTime;
}
bool CompTime_Smaller( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	return first.m_dwTime < second.m_dwTime;
}

bool CompName_Greater( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	if(first.m_strName.Compare(second.m_strName) < 0)
		return true;
	else
		return false;
}
bool CompName_Smaller( HOUSING_ITEM& first, HOUSING_ITEM& second )
{
	if(first.m_strName.Compare(second.m_strName) > 0)
		return true;
	else
		return false;
}

CWndHousing::CWndHousing() 
{ 
	m_vecItem.clear();
	m_mapItem.clear();
	m_nSortType = WIDC_STATIC2;
	m_bIsGreater = TRUE;
	m_nSelected = 0;
} 

CWndHousing::~CWndHousing() 
{ 
} 

void CWndHousing::SerializeRegInfo( CAr& ar, DWORD& dwVersion )
{
	CWndNeuz::SerializeRegInfo( ar, dwVersion );
}

void CWndHousing::OnDraw( C2DRender* p2DRender ) 
{ 
	if(m_vecItem.size())
	{
		std::vector<HOUSING_ITEM>::iterator iter;

		CWndListBox*			pWndListBox( (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 ) );
		FLERROR_ASSERT_LOG_RETURN_VOID( pWndListBox, _T("[윈도우 하우징] 리스트 박스 포인터가 유효하지 않습니다.") );

		LPWNDCTRL				pCustom( GetWndCtrl( WIDC_LISTBOX1 ) );
		FLERROR_ASSERT_LOG_RETURN_VOID( pCustom, _T("[윈도우 하우징] 커스텀 윈도우 포인터가 유효하지 않습니다.") );

		int						nListFontHeight( pWndListBox->GetFontHeight() + 1 );
		DWORD					dwColor( m_dwColor );
		int						nDrawCount( NULL );
		int						nIndex( NULL );
		int						nLine( NULL );

		PFONTCOLOR_WNDHOUSING	pFontColorWndHousing = g_WndFontColorManager->GetWndHousing();


		for(iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
		{
			if( m_nSelected != 0 && (m_nSelected - 1) == nLine )
				dwColor = pFontColorWndHousing->m_stSelect.GetFontColor();
			else
				dwColor = m_dwColor;

			if(nLine < pWndListBox->GetScrollPos()) 
			{
				nLine++;
				continue;	
			}
			else
				nLine++;

			if(nDrawCount >= 10) 
				continue;	// 10개 까지만 드로잉한다

			// 타입
			switch(iter->m_nType)
			{
				case IK3_BED:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_BED), dwColor);
					break;
				case IK3_SOFA:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_SOFA), dwColor);
					break;
				case IK3_WARDROBE:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_WARDROBE), dwColor);
					break;
				case IK3_CLOSET:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_CLOSET), dwColor);
					break;
				case IK3_TABLE:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_TABLE), dwColor);
					break;
				case IK3_CABINET:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_CABINET), dwColor);
					break;
				case IK3_PROPS:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_PROPS), dwColor);
					break;
				case IK3_WALLPAPER:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_WALLPAPER), dwColor);
					break;
				case IK3_CARPET:
					p2DRender->TextOut( pCustom->rect.left + 5, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, prj.GetText(TID_GAME_HOUSING_CARPET), dwColor);
					break;
			};
			// 이름
			p2DRender->TextOut( pCustom->rect.left + 85, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, iter->m_strName, dwColor);

			// 남은 시간
			time_t		t = (time_t)iter->m_dwTime - time_null();
			if(t < 0)	t = 0;
			CTimeSpan	ts( t );
			CString		strTime;

			// gmpbigsun : -_- 1시간 미만이면 시간만 표기하게 돼있다 -> 수정함 100105
			if( ts.GetDays() <= 0 && ts.GetHours() <= 0)
				strTime.Format( prj.GetText( TID_GAME_TIME_1HOUR ));
			else
				strTime.Format( prj.GetText( TID_GAME_REMAIN_TIME ), static_cast<int>(ts.GetDays()), ts.GetHours());

			p2DRender->TextOut( pCustom->rect.left + 250, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, strTime, dwColor);

			// 설치 여부
			if(iter->m_bDeploy)
				p2DRender->TextOut( pCustom->rect.left + 375, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, _T("O"), dwColor);
			else
				p2DRender->TextOut( pCustom->rect.left + 375, pCustom->rect.top + 8 + (nIndex)*nListFontHeight, _T("X"), dwColor);

			++nIndex;
			++nDrawCount;
		}
	}
} 

void CWndHousing::RefreshItemList()
{

	HOUSING_ITEM	tmpItem;
	int				nIndex = 1;

	// 아이템 리스트를 싹 새로 갱신한다
	m_vecItem.clear();
	m_mapItem.clear();
	
	CHousing::GetInstance()->GetHousingList( m_vecItem );

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	pWndListBox->ResetContent();
	
	for(std::vector<HOUSINGINFO>::iterator iter = m_vecItem.begin(); iter != m_vecItem.end(); ++iter)
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem(iter->dwItemId);
		
		pWndListBox->AddString(" ");
		tmpItem.m_nIndex  = nIndex;
		tmpItem.m_bDeploy = iter->bSetup;
		tmpItem.m_dwTime  = iter->tKeepTime; 
		tmpItem.m_strName = pItemProp->szName;
		tmpItem.m_strDesc = pItemProp->szCommand;
		tmpItem.m_nType   = pItemProp->dwItemKind3;
		tmpItem.dwItemId  = iter->dwItemId;

		m_mapItem.push_back(tmpItem);
		++nIndex;
	}
	
	Sort();
	m_nSelected = 0;
}

void CWndHousing::Sort()
{
	if(m_mapItem.size() > 1)
	{
		switch(m_nSortType)
		{
			case WIDC_STATIC2:		// 타입
				if(m_bIsGreater)
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompType_Greater);
				else
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompType_Smaller);
				break;
			case WIDC_STATIC3:		// 아이템명
				if(m_bIsGreater)
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompName_Greater);
				else
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompName_Smaller);
				break;
			case WIDC_STATIC4:		// 남은시간
				if(m_bIsGreater)
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompTime_Greater);
				else
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompTime_Smaller);
				break;
			case WIDC_STATIC5:		// 설치
				if(m_bIsGreater)
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompDeploy_Greater);
				else
					std::sort( m_mapItem.begin(), m_mapItem.end(), CompDeploy_Smaller);
				break;
		};
	}
}

void CWndHousing::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	RefreshItemList();

	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pWndButton1->EnableWindow(FALSE);
	pWndButton2->EnableWindow(FALSE);

	//ListBox
	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent( "123" );

	if(size.cy+2 > 16)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;

		CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
		lpWndCtrl = GetWndCtrl( WIDC_LISTBOX1 );
		rect = lpWndCtrl->rect;
		rect.bottom += ((size.cy+2)*10) - (rect.bottom-rect.top) + 4;
		//폰트 크기 차이 때문에 리스트박스 크기 늘임
		pWndList->SetWndRect(rect);

		//창 밀리는 경우 아래도 크기 조정
		CRect rectStatic, recText;
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
		lpWndCtrl = GetWndCtrl( WIDC_STATIC1 );
		rectStatic = lpWndCtrl->rect;
		pWndStatic->Move(rectStatic.left, rect.bottom + 10);

		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		lpWndCtrl = GetWndCtrl( WIDC_TEXT1 );
		recText = lpWndCtrl->rect;
		recText.top = rect.bottom + rectStatic.Height() + 18;
		pWndText->SetWndRect(recText, TRUE);
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndHousing::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HOUSING, 0, CPoint( 0, 0 ), pWndParent );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndHousing::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndHousing::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndHousing::OnSize( UINT nType, int cx, int cy ) 
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
}

BOOL CWndHousing::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	return TRUE;
}

void CWndHousing::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC2 );
	if( PtInRect(&pCustom->rect, point) )
	{
		//CtrlId : WIDC_STATIC2 - 종류
		if(m_nSortType == WIDC_STATIC2)
			m_bIsGreater = !m_bIsGreater;
		else
		{
			m_nSortType = WIDC_STATIC2;
			m_bIsGreater = TRUE;
		}
		RefreshItemList();
		CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		pWndButton1->EnableWindow(FALSE);
		pWndButton2->EnableWindow(FALSE);
	}
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	if( PtInRect(&pCustom->rect, point) )
	{
		//CtrlId : WIDC_STATIC3 - 아이템명
		if(m_nSortType == WIDC_STATIC3)
			m_bIsGreater = !m_bIsGreater;
		else
		{
			m_nSortType = WIDC_STATIC3;
			m_bIsGreater = TRUE;
		}
		RefreshItemList();
		CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		pWndButton1->EnableWindow(FALSE);
		pWndButton2->EnableWindow(FALSE);
	}
	pCustom = GetWndCtrl( WIDC_STATIC4 );
	if( PtInRect(&pCustom->rect, point) )
	{
		//CtrlId : WIDC_STATIC4 - 남은 시간
		if(m_nSortType == WIDC_STATIC4)
			m_bIsGreater = !m_bIsGreater;
		else
		{
			m_nSortType = WIDC_STATIC4;
			m_bIsGreater = TRUE;
		}
		RefreshItemList();
		CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		pWndButton1->EnableWindow(FALSE);
		pWndButton2->EnableWindow(FALSE);
	}
	pCustom = GetWndCtrl( WIDC_STATIC5 );
	if( PtInRect(&pCustom->rect, point) )
	{
		//CtrlId : WIDC_STATIC5 - 설치
		if(m_nSortType == WIDC_STATIC5)
			m_bIsGreater = !m_bIsGreater;
		else
		{
			m_nSortType = WIDC_STATIC5;
			m_bIsGreater = TRUE;
		}
		RefreshItemList();
		CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		pWndButton1->EnableWindow(FALSE);
		pWndButton2->EnableWindow(FALSE);
	}
	
} 
void CWndHousing::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndHousing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	int				nLoop		= 0;

	switch(nID)
	{
		case WIDC_LISTBOX1: // view ctrl
			{
				CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
				CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
				CWndText*		pWndText	= (CWndText*)GetDlgItem( WIDC_TEXT1 );
				
				m_nSelected					= pWndListBox->GetCurSel() + 1;

				for(std::vector<HOUSING_ITEM>::iterator iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
				{
					++nLoop;
					if(m_nSelected > nLoop)	continue;
					else if(m_nSelected == nLoop)
					{
						
						// 설명을 띄운다
						pWndText->SetString(iter->m_strDesc);
						
						//설치되었으면 해제버튼 활성/설치버튼 비활성, 안되었으면 반대로
						if(iter->m_bDeploy)
						{
							pWndButton1->EnableWindow(FALSE);
							pWndButton2->EnableWindow(FALSE);
							if(g_pPlayer)
							{
								CWorld* pWorld = g_pPlayer->GetWorld();
								if(pWorld)
								{
									if(pWorld->GetID() == WI_WORLD_MINIROOM && CDeployManager::GetInstance()->IsMyRoom())
										pWndButton2->EnableWindow(TRUE);
								} 
							}
						}
						else
						{
							pWndButton1->EnableWindow(FALSE);
							pWndButton2->EnableWindow(FALSE);
							if(g_pPlayer)
							{
								CWorld* pWorld = g_pPlayer->GetWorld();
								if(pWorld)
								{
									if(pWorld->GetID() == WI_WORLD_MINIROOM && CDeployManager::GetInstance()->IsMyRoom())
										pWndButton1->EnableWindow(TRUE);
								} 
							}
						}
					}
				}
			}
			break;

		case WIDC_BUTTON1:// 설치버튼
			{
				if(CDeployManager::GetInstance()->IsReady())
						CDeployManager::GetInstance()->EndDeploy();

				CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				for(std::vector<HOUSING_ITEM>::iterator iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
				{
					++nLoop;
					if(m_nSelected > nLoop)	continue;
					else if(m_nSelected == nLoop)
					{
						HOUSINGINFO		tmpData;
						PT_ITEM_SPEC	pItemProp = g_xSpecManager->GetSpecItem(iter->dwItemId);
						tmpData.bSetup    = TRUE;
						tmpData.dwItemId  = iter->dwItemId;
						tmpData.fAngle    = 0.0f;
						tmpData.tKeepTime = 0;

						if(iter->m_nType == IK3_WALLPAPER || iter->m_nType == IK3_CARPET)
						{
							g_DPlay.SendHousingReqSetupFurniture(tmpData);
						}
						else
						{
							CDeployManager::GetInstance()->LoadToDeploy(pItemProp->dwLinkKind, tmpData);
							CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
							if(pWndWorld) pWndWorld->SetFocus();
						}

						CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
						CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
						pWndButton1->EnableWindow(FALSE);
						pWndButton2->EnableWindow(FALSE);
					}
				}
			}
			break;

		case WIDC_BUTTON2:// 해제버튼
			if(!CDeployManager::GetInstance()->IsReady())
			{
				CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				for(std::vector<HOUSING_ITEM>::iterator iter = m_mapItem.begin(); iter != m_mapItem.end(); ++iter)
				{
					++nLoop;
					if(m_nSelected > nLoop)	continue;
					else if(m_nSelected == nLoop)
					{
						HOUSINGINFO		tmpData;
						PT_ITEM_SPEC	pItemProp = g_xSpecManager->GetSpecItem(iter->dwItemId);
						tmpData.bSetup    = FALSE;
						tmpData.dwItemId  = iter->dwItemId;
						tmpData.fAngle    = 0.0f;
						tmpData.tKeepTime = 0;
						g_DPlay.SendHousingReqSetupFurniture( tmpData );
						CWndButton*		pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
						CWndButton*		pWndButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
						pWndButton1->EnableWindow(FALSE);
						pWndButton2->EnableWindow(FALSE);
					}
				}	
			}
			break;
	
		
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndGuildHousing
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndGuildHousing::CWndGuildHousing( )
{
	m_eSection = GS_FURNITURE;		// 'R'
	m_bDeploying = FALSE;
	m_pGHShowOne = NULL;
	m_dwComboCurrIK3 = 0;
	m_nSelectedSort = 0;
}

CWndGuildHousing::~CWndGuildHousing( )
{
	SAFE_DELETE( m_pGHShowOne );
}

BOOL CWndGuildHousing::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GH_FURNITURE_STORAGE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndGuildHousing::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	if( pWndCombo )
	{
		pWndCombo->AddWndStyle( EBS_READONLY );
		pWndCombo->ResetContent( );
		int nIndex = -1;

		nIndex = pWndCombo->AddString( GETTEXT(TID_GAME_GUILDHOUSE_FURKIND) );
		pWndCombo->SetItemData( nIndex, 0 );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_BED ) );
		pWndCombo->SetItemData( nIndex, IK3_BED );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_TABLE ) );
		pWndCombo->SetItemData( nIndex, IK3_TABLE );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_SOFA ) );
		pWndCombo->SetItemData( nIndex, IK3_SOFA );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_DESK ) );
		pWndCombo->SetItemData( nIndex, IK3_DESK );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CHAIR ) );
		pWndCombo->SetItemData( nIndex, IK3_CHAIR );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CASE ) );
		pWndCombo->SetItemData( nIndex, IK3_CASE );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CLOSET ) );
		pWndCombo->SetItemData( nIndex, IK3_CLOSET );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CARPET ) );
		pWndCombo->SetItemData( nIndex, IK3_CARPET );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_WALLPAPER ) );
		pWndCombo->SetItemData( nIndex, IK3_WALLPAPER );

		//////////////////////////////////////////////////////////////////////////
		//	BEGIN110321	길드 가구 보관함 UI 카테고리 수정
// 		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_WARDROBE ) );
// 		pWndCombo->SetItemData( nIndex, IK3_WARDROBE );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CRYSTAL ) );
		pWndCombo->SetItemData( nIndex, IK3_REST );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_BATH ) );
		pWndCombo->SetItemData( nIndex, IK3_BATH );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_DRAWER ) );
		pWndCombo->SetItemData( nIndex, IK3_DRAWER );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CABINET ) );
		pWndCombo->SetItemData( nIndex, IK3_CABINET );
		//	END110321
		//////////////////////////////////////////////////////////////////////////
	}

	pWndCombo->SetCurSel( 0 );

	int sHeight = 30;

	CRect rect( 368, 10, 408, 40 );
	for( int i = 0; i < GH_MAX_VIEW_CAPACITY; ++i )
	{
		if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		{
			rect.top = 55 + ( i * sHeight ) + ( i + 1 ) * 10;
			rect.bottom = 65 + ( i * sHeight ) + sHeight + ( i + 1 ) * 10;
		}
		if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		{
			rect.top = 50 + ( i * sHeight ) + ( i + 1 ) * 10;
			rect.bottom = 60 + ( i * sHeight ) + sHeight + ( i + 1 ) * 10;
		}

		m_wndButton[i].Create( _T(""), WBS_CHILD, rect, this, i ); 
		m_wndButton[i].SetWndRect( rect );
		m_wndButton[i].SetVisible( FALSE );
	}

	//리스트박스에 들어갈 아이템의 높이 
	CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->m_nLineSpace = (int)( ( 40 - pWndListBox->GetFontHeight( ) ) * 0.5f );

	// 여기에 코딩하세요
	RefreshItemList();

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	CWndStatic* pStatic = NULL;
	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC4 );
	pStatic->SetVisible( FALSE );

	pStatic = (CWndStatic *)GetDlgItem( WIDC_STATIC5 );
	pStatic->SetVisible( FALSE );

	m_texUp.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME,g_xFlyffConfig->GetMainLanguage(), "LvUp.bmp" ), WNDCOLOR_DEFAULT_KEY );
	m_texDown.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"LvDn.bmp" ), WNDCOLOR_DEFAULT_KEY );
	
} 


void CWndGuildHousing::OnDraw( C2DRender* p2DRender ) 
{ 
	if( !m_cWndItems.empty() )
	{
		std::vector<HOUSING_ITEM>::iterator iter;

		CWndListBox*				pWndListBox( (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 ) );
		FLERROR_ASSERT_LOG_RETURN_VOID( pWndListBox, _T("[윈도우 하우징] 리스트 박스 포인터가 유효하지 않습니다.") );
	
		LPWNDCTRL					pCustom( GetWndCtrl( WIDC_LISTBOX1 ) );
		FLERROR_ASSERT_LOG_RETURN_VOID( pCustom, _T("[윈도우 하우징] 커스텀 윈도우 포인터가 유효하지 않습니다.") );
		
		PFONTCOLOR_WNDGUILDHOUSING	pFontColorWndGuildHousing( g_WndFontColorManager->GetWndGuildHousing() );
		PFLCOLOR_WNDGUILDHOUSING	pColorWndWndGuildHousing( g_FLColorManager->GetWndGuildHousing() );


		static const int			nListFontHeight = 40; 
		DWORD						dwColor( m_dwColor );
		int							nDrawCount( NULL );
		int							nIndex( NULL );
		int							nLine( NULL );
		bool						bSelected( false );

		UpdateIRButton();

		CRect renderBoxRect( pCustom->rect );
		renderBoxRect.left	+= 5;
		renderBoxRect.right -= 28;
		
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		FLERROR_ASSERT_LOG_RETURN_VOID( pWndText, _T("[윈도우 하우징] 커스텀 윈도우 포인터가 유효하지 않습니다.") );

		BOOL bNoSelecting( TRUE );

		for(iter = m_cWndItems.begin(); iter != m_cWndItems.end(); ++iter)
		{
			//선택 라인
			bSelected = ( m_nSelected != 0 && (m_nSelected - 1) == nLine );
	
			//위로 올라간 라인 패스 
			if(nLine < pWndListBox->GetScrollPos()) 
			{
				nLine++;
				continue;	
			}
			else
				nLine++;

			//드로잉 최대수치 
			if(nDrawCount >= GH_MAX_VIEW_CAPACITY )		
				continue;	

			const HOUSING_ITEM& kItem = *iter;

			if( kItem.m_bDeploy )
				dwColor = pFontColorWndGuildHousing->m_stDeploy.GetFontColor();
			else 
				dwColor = m_dwColor;

			//Render icon
			CPoint pt( 0, 0 ); 
			
			if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
			{
				pt.x = pCustom->rect.left + 16;
				pt.y = pCustom->rect.top + 10 + (nIndex)*nListFontHeight;
			}

			if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
			{
				pt.x = pCustom->rect.left + 16;
				pt.y = pCustom->rect.top + 8 + (nIndex)*nListFontHeight;
			}

			PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( kItem.m_nIndex );
			FLERROR_ASSERT_LOG_DO( pProp, continue, _T("[윈도우 길드 하우징] 아이템의 속성 포인터가 유효하지 않습니다.") );
			
			if( !pProp )
				continue;


			CTexture* pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), WNDCOLOR_DEFAULT_KEY );
			if( pTexture )
				p2DRender->RenderTexture( pt, pTexture );
		
			// 이름
			p2DRender->TextOut( pCustom->rect.left + 60, pCustom->rect.top + 20 + (nIndex)*nListFontHeight, kItem.m_strName, dwColor);

			// 남은 시간
			bool bGoTime = true;
			time_t		t = (time_t)iter->m_dwTime - time_null();
			if(t < 0)	
			{
				t = iter->m_dwTime;	// 만료된것은 자동으로 빠지기 때문에 이경우 아직 설치가 안됀 경우다.( Prop 시간 설정 )
				bGoTime = false;
			}
			CTimeSpan	ts( t );
			CString		strTime;

			if( ts.GetDays( ) )
				strTime.Format( prj.GetText( TID_PK_LIMIT_DAY ), ts.GetDays() );
			else if( ts.GetHours( ) )
				strTime.Format( prj.GetText(TID_PK_LIMIT_HOUR ), ts.GetHours() );
			else 
				strTime.Format( prj.GetText(TID_PK_LIMIT_MINUTE ), ts.GetMinutes() );

			//gmpbigsun(100326) : 진행중 특수기호 제거 및 기존(NEW)->외부로부터 읽어옴
			if( !bGoTime )
				strTime = strTime + CString( prj.GetText( TID_GAME_TOOLTIP_NEWFURNITURE ) ); 		//설치가 한번도 안댄( 시간이 안간다 )			

			p2DRender->TextOut( pCustom->rect.left + 280, pCustom->rect.top + 20 + (nIndex)*nListFontHeight, strTime, dwColor);

			//선택영역 박스
			if( bSelected )
			{
				bNoSelecting = FALSE;
				renderBoxRect.top = renderBoxRect.top + 4 + (nIndex)*nListFontHeight,
				renderBoxRect.bottom = renderBoxRect.top + 40;
				p2DRender->RenderFillRect( renderBoxRect, pColorWndWndGuildHousing->m_stSelectBoxFill.GetColor() );

				// 기능설명 
				pWndText->SetString( kItem.m_strDesc );
			}
		
			++nIndex;
			++nDrawCount;
		}

		if( bNoSelecting )
			pWndText->SetString( "" );
	}

	if( m_nSelectedSort != 0 )
	{
		//소트 마크 
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( m_nSelectedSort );
		if( pWndStatic )
		{
			CRect rect = pWndStatic->GetClientRect( TRUE );

			if( m_bIsGreater )	//오름차순
				p2DRender->RenderTexture( CPoint( rect.left-18, rect.top ), &m_texUp );
			else	//내림차순
				p2DRender->RenderTexture( CPoint( rect.left-18, rect.top ), &m_texDown );
		}
	}
} 

void CWndGuildHousing::RefreshItemList( )
{
	HOUSING_ITEM	tmpItem;
	int				nIndex = 1;

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	int currScrollPos = pWndListBox->GetScrollPos( );		// 현재 스크롤 위치를 보관해둔다.
	pWndListBox->ResetContent();

	// 벽지, 장판 해체시 복구flag
	BOOL bWallTex = FALSE;
	BOOL bTileTex = FALSE;

	// 아이템 리스트를 싹 새로 갱신한다
	m_cWndItems.clear();

	BOOL bShowWanted = FALSE;
	if( 0 != m_dwComboCurrIK3 )
		bShowWanted = TRUE;

	int iMaxSize = GuildHouse->GetFurnitureListSize( );
	for( int i = 0; i < iMaxSize; ++i )
	{
		GH_Fntr_Info* pInfo = GuildHouse->GetFurnitureInfoPtr( i );
		if( !pInfo )
			continue;

		PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( pInfo->dwItemId );
		if( !pProp )
		{
			FLASSERT( 0 );
			continue;
		}

		if( bShowWanted && m_dwComboCurrIK3 != pProp->dwItemKind3 )			//콤보박스의 가구종류를 선택한경우 해당가구류만 보여준다.
			continue;

		// 현재 섹션이 가구면 텔레포터 패스, 현재 섹션이 텔레포터면 가구 패스 
		if( GS_FURNITURE == m_eSection )
		{
			if( IK2_GUILDHOUSE_NPC == pProp->dwItemKind2 )
				continue;
		}
		else
		if( GS_TELEPORTER == m_eSection )
		{
			// 가구, 벽지/장판 
			if( IK2_GUILDHOUSE_FURNITURE == pProp->dwItemKind2 || IK2_GUILDHOUSE_PAPERING == pProp->dwItemKind2 )  
				continue;
		}

		pWndListBox->AddString( " " );

		tmpItem.m_nIndex = pInfo->dwItemId;
		tmpItem.m_bDeploy = pInfo->bSetup;
		tmpItem.m_dwTime = pInfo->tKeepTime;
		
		// 아직 설치가 한번도 안됀경우 남은시간을 받지(서버로부터)않는다. 고로 Prop data를 쓰자.
		if( 0 >= tmpItem.m_dwTime )
			tmpItem.m_dwTime = pProp->dwAbilityMin * 60;
		tmpItem.dwItemId = pInfo->objId;
		tmpItem.m_strName = pProp->szName;		//프로퍼티 이름 
		tmpItem.m_strDesc = pProp->szCommand;	//  "      설명 
		tmpItem.m_nSlotIndex = i;

		m_cWndItems.push_back( tmpItem );
		
		++nIndex;
	}

	GuildHouse->ApplyEFTexture( );
	
	Sort();

	// 타겟이 있다면 소팅후에 해당아이템 선택된상태로 ...
	if( !IsValidObjID( GuildHouse->m_dwSelectedObjID ) )
		m_nSelected = 0;
	else if( !SetSelectedByObjID( GuildHouse->m_dwSelectedObjID ) )
		m_nSelected = 0;

	pWndListBox->SetScrollPos( currScrollPos );

	UpdateIRButton( );
}

void CWndGuildHousing::UpdateIRButton( )
{
	FLERROR_ASSERT_LOG_RETURN_VOID( g_pPlayer, _T("[윈도우 길드 하우징] 플레이어의 포인터가 유효하지 않습니다.") );

	CWorld* pWorld = g_pPlayer->GetWorld();
	FLERROR_ASSERT_LOG_RETURN_VOID( pWorld, _T("[윈도우 길드 하우징] 월드의 포인터가 유효하지 않습니다.") );
	
	CGuild* pGuild = g_pPlayer->GetGuild();
	FLERROR_ASSERT_LOG_RETURN_VOID( pGuild, _T("[윈도우 길드 하우징] 길드의 포인터가 유효하지 않습니다.") );

	BOOL bResult( FALSE );
	
	bResult = pGuild->IsAuthority( g_pPlayer->m_idPlayer, PF_GUILDHOUSE_FURNITURE );
	
	if( bResult )
	{
		if( FALSE == pWorld->IsWorldGuildHouse() )			// 길드하우스 월드가 아니면 
			bResult = FALSE;
	}

	SetEnableInstallBtns( bResult );

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndListBox, _T("[윈도우 길드 하우징] 리스트 박스의 포인터가 유효하지 않습니다.") );

	int nScrollPos = pWndListBox->GetScrollPos();			// 가장 top인가?
	if( nScrollPos < 0 )
		pWndListBox->SetScrollPos( 0 );

	int iMaxSize = m_cWndItems.size( );
	for( int i = 0; i < GH_MAX_VIEW_CAPACITY; ++i )
	{
		CWndButton* pBtn = &m_wndButton[i];
		FLERROR_ASSERT_LOG_DO( pBtn, continue, _T("[윈도우 길드 하우징] 버튼의 포인터가 유효하지 않습니다.") );

		int top = i + nScrollPos;						//실제 데이터는 스크롤 포지션에 영향을 받고

		if( top < iMaxSize )
		{
			//데이터가 존재하는경우만 버튼을 보이게 하고 설치/해체 를 상황에 맞게 바꾸어준다.
			pBtn->SetVisible( TRUE );		//버튼은 무조건 0부터다.
			
			if( m_cWndItems[ top ].m_bDeploy )		
			{
				// 해체하는 이미지 ( 설치가 되어있으므로 )
				if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
					pBtn->SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Buttlockerinstall.BMP" ), TRUE );
				if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
					pBtn->SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Buttlockerinstall.tga" ), TRUE );

				pBtn->SetToolTip( GETTEXT(TID_TOOLTIP_GUILDHOUSE_BUTT_DISMANTLE) );		//가구가 해체됩니다.
			}
			else									
			{
				// 설치하는 이미지 ( 해체가 되어있으므로 )

				if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
					pBtn->SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Buttlockerdismantle.BMP" ), TRUE );
				if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
					pBtn->SetTexture( D3DDEVICE, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"Buttlockerdismantle.tga" ), TRUE );

				pBtn->SetToolTip( GETTEXT(TID_TOOLTIP_GUILDHOUSE_BUTT_INSTALL) );		//가구가 설치됩니다.
			}
		}
		else 
			pBtn->SetVisible( FALSE );
	}

	// 텔레포터 창일경우 설치된 녀석이 있다면 설치 안된녀석들은 disable 
	if( bResult && IsSection( GS_TELEPORTER ) )
	{
		BOOL bDeploy( FALSE );
		for( int i = 0; i < iMaxSize; ++i )
		{
			if( m_cWndItems[ i ].m_bDeploy )
			{
				bDeploy = TRUE;
				break;
			}
		}

		if( bDeploy )
		{
			for( int i = 0; i < GH_MAX_VIEW_CAPACITY; ++i )
			{
				CWndButton* pBtn = &m_wndButton[i];
				FLERROR_ASSERT_LOG_DO( pBtn, continue, _T("[윈도우 길드 하우징] 버튼의 포인터가 유효하지 않습니다.") );

				int top = i + nScrollPos;						//실제 데이터는 스크롤 포지션에 영향을 받고

				if( top < iMaxSize )
				{
					if( !m_cWndItems[ top ].m_bDeploy )		
						m_wndButton[i].EnableWindow( FALSE );
				}
			}
		}
	}
}

void CWndGuildHousing::Sort()
{
	if( !m_cWndItems.empty( ) )
	{
		switch(m_nSortType)
		{
			case WIDC_STATIC1:		// 아이템명
				if(m_bIsGreater)
					std::sort( m_cWndItems.begin(), m_cWndItems.end(), CompName_Greater);
				else
					std::sort( m_cWndItems.begin(), m_cWndItems.end(), CompName_Smaller);
				break;
			case WIDC_STATIC2:		// 남은시간
				if(m_bIsGreater)
					std::sort( m_cWndItems.begin(), m_cWndItems.end(), CompTime_Greater);
				else
					std::sort( m_cWndItems.begin(), m_cWndItems.end(), CompTime_Smaller);
				break;
			case WIDC_STATIC3:		// 설치
				if(m_bIsGreater)
					std::sort( m_cWndItems.begin(), m_cWndItems.end(), CompDeploy_Greater);
				else
					std::sort( m_cWndItems.begin(), m_cWndItems.end(), CompDeploy_Smaller);
				break;
		}

	}

}

BOOL CWndGuildHousing::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID >= 0 && nID < GH_MAX_VIEW_CAPACITY )
	{
		//리스트안에 아이템별 버튼이 눌렸다..

		// 설치, 재설치, 해체가 되지 않는 채널일경우 메세지 출력하고 Retrun
		if( !GuildHouse->IsSetFurnitureChannel( ) )
		{
			g_WndMng.PutString( GETTEXT( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_CHANNEL  ) );
			return CWndNeuz::OnChildNotify( message, nID, pLResult );
		}

		//스크롤이 된경우 nID는 탑인덱스를 더한 값이다.
		CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
		int scrollPos = pWndListBox->GetScrollPos( );

		//마우스 휠을 올릴경우 ScrollPos가 음수가 되는현상 보정 
		if( scrollPos < 0 )
			pWndListBox->SetScrollPos( 0 );					
		int resultIndex = nID + pWndListBox->GetScrollPos( );

		FLASSERT( resultIndex > -1 );

		HOUSING_ITEM& kItem = m_cWndItems[ resultIndex ];
		FLASSERT( (int)( m_cWndItems.size() ) > resultIndex );

		if( !kItem.m_bDeploy )		//설치되지 않았다면 
		{
			kItem.m_vPos = g_pPlayer->GetPos( );
			kItem.m_fAngle = 0.0f;

			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( kItem.m_nIndex );
			FLASSERT( pItemProp );

			// 텔레포터, 벽지, 장판인 경우 바로 설치 요청 , 가구인 경우 편집모드로 변경
			if( IK3_TELEPORTER == pItemProp->dwItemKind3 || IK3_WALLPAPER == pItemProp->dwItemKind3 || IK3_CARPET == pItemProp->dwItemKind3 )
			{
				GH_Fntr_Info* pInfo = GuildHouse->GetFurnitureInfoPtr( kItem.m_nSlotIndex );
				FLASSERT( pInfo );
				// 이미 설치되어 있는 텔레포터가 있는지 검사 
				GuildHouse->SendClientToWorld( GUILDHOUSE_PCKTTYPE_SETUP, *pInfo, kItem.m_nSlotIndex );
			}else
			{
				GuildDeploy()->LoadToDeploy(pItemProp->dwLinkKind, kItem );
				GuildHouse->m_iMode = GUILDHOUSE_PCKTTYPE_SETUP;
				
				CWndWorld* pWndWorld = (CWndWorld*)GetWndBase( APP_WORLD );
				if(pWndWorld) 
					pWndWorld->SetFocus();

				GuildHouse->m_nWndDeployingIndex = resultIndex;
			}
		}
		else
		{
			GH_Fntr_Info* pInfo = GuildHouse->GetFurnitureInfoPtr( kItem.m_nSlotIndex );
			FLASSERT( pInfo );
			GuildHouse->SendClientToWorld( GUILDHOUSE_PCKTTYPE_REMOVE, *pInfo, kItem.m_nSlotIndex );
		}
	}

	switch(nID)
	{
		case WIDC_LISTBOX1: // view ctrl
		{
			int	nLoop = 0;
			CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
			m_nSelected					= pWndListBox->GetCurSel() + 1;

			for(std::vector<HOUSING_ITEM>::iterator iter = m_cWndItems.begin(); iter != m_cWndItems.end(); ++iter)
			{
				HOUSING_ITEM& kItem = *iter;
				++nLoop;
				if(m_nSelected > nLoop)	continue;
				else if(m_nSelected == nLoop)
				{
					//현재 선택된 아이템이라면 
					GuildHouse->m_dwSelectedObjID = kItem.dwItemId;
				}
			}
		}
		break;

		case WIDC_COMBOBOX1:
			{
				if( message == WNM_SELCHANGE )
				{
					CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
					int curSel = pCombo->GetCurSel();
					m_dwComboCurrIK3 = pCombo->GetItemData( curSel );

					CWndListBox*	pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
					pWndListBox->SetCurSel( 0 );		// 콤보박스 아이템을 선택한경우 스크롤 위치는 최상으로 
					RefreshItemList( );
				}
			}
			break;
	}



	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndGuildHousing::SetSelectedByObjID( OBJID objID )
{
	// 오브젝트 아이디로 현재 유지하는 Data index를 뽑는다. 오브젝트를 선택했을때 윈도우에 자동선택기능을 위해 만들어짐 

	int totalSize = m_cWndItems.size();
	for( int i = 0; i < totalSize; ++i )
	{
		HOUSING_ITEM& kItem = m_cWndItems[ i ];
		if( kItem.dwItemId == objID )
		{
			m_nSelected = i+1;

			FixScrollBar( m_nSelected );
			
			return TRUE;
		}
	}

	return FALSE;
}

void CWndGuildHousing::FixScrollBar( const int nSelected )
{
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( !pWndListBox )
		return;

	int curScrollPos = pWndListBox->GetScrollPos( );
	int curSelected = pWndListBox->GetCurSel( );

	int nDis = 0;
	if( m_nSelected < ( curScrollPos + 1 ) )			//선택박스가 스크롤 위에 있는경우 
	{
		nDis = m_nSelected - 1;
	}
	else if ( m_nSelected > curScrollPos + GH_MAX_VIEW_CAPACITY )	// 선택박스가 스크롤 최대영역 아래 있는경우 
	{
		nDis = m_nSelected - GH_MAX_VIEW_CAPACITY;
	}
	else 
	{
		return;
	}

	pWndListBox->SetScrollPos( nDis );
}

int CWndGuildHousing::GetWndItemIndexByObjID( OBJID objID )
{
	int totalSize = m_cWndItems.size();
	for( int i = 0; i < totalSize; ++i )
	{
		HOUSING_ITEM& kItem = m_cWndItems[ i ];
		if( kItem.dwItemId == objID )
		{
			return i;
		}
	}

	return -1;
}

void CWndGuildHousing::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC1 );
	if( PtInRect(&pCustom->rect, point) )
	{
		if(m_nSortType == WIDC_STATIC1)
		{
			m_bIsGreater = !m_bIsGreater;
			UpdateSortTextColor( m_nSortType, WIDC_STATIC1 );
		}
		else
		{
			m_bIsGreater = TRUE;
			UpdateSortTextColor( m_nSortType, WIDC_STATIC1 );
			m_nSortType = WIDC_STATIC1;
			m_bIsGreater = TRUE;
		}
		RefreshItemList();
	}
	pCustom = GetWndCtrl( WIDC_STATIC2 );
	if( PtInRect(&pCustom->rect, point) )
	{
		//CtrlId : WIDC_STATIC2 - 시간 
		if(m_nSortType == WIDC_STATIC2)
		{
			m_bIsGreater = !m_bIsGreater;
			UpdateSortTextColor( m_nSortType, WIDC_STATIC2 );
		}
		else
		{
			m_bIsGreater = TRUE;
			UpdateSortTextColor( m_nSortType, WIDC_STATIC2 );
			m_nSortType = WIDC_STATIC2;
			m_bIsGreater = TRUE;
		}
		RefreshItemList();
	}
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	if( PtInRect(&pCustom->rect, point) )
	{
		//CtrlId : WIDC_STATIC3 - 설치상태
		if(m_nSortType == WIDC_STATIC3)
		{
			m_bIsGreater = !m_bIsGreater;
			UpdateSortTextColor( m_nSortType, WIDC_STATIC3 );
		}
		else
		{
			m_bIsGreater = TRUE;
			UpdateSortTextColor( m_nSortType, WIDC_STATIC3 );
			m_nSortType = WIDC_STATIC3;
			m_bIsGreater = TRUE;
		}
		
		RefreshItemList();
	}
} 

void CWndGuildHousing::UpdateSortTextColor( int oldType, int newType )
{
	// 2010_04_06 : 오름차순 내림차순 문자표시->아이콘 표시
	m_nSelectedSort = newType;

	//static const DWORD orgColor = 0xff2e70a9;		//음 ...
	//static const CString strUP = "^";
	//static const CString strDOWN = " ";
	//CWndStatic* pStatic = NULL;
	//pStatic = (CWndStatic*)GetDlgItem( oldType );
	//if( pStatic )
	//{
	//	pStatic->m_dwColor = orgColor;
	//	CString curTitle = pStatic->GetTitle( );

	//	if( -1 != curTitle.Find( strUP ) || -1 != curTitle.Find( strDOWN ) )
	//	{
	//		curTitle.Delete( curTitle.GetLength() - 1, 1 );
	//		pStatic->SetTitle( curTitle );
	//	}
	//}

	//pStatic = (CWndStatic*)GetDlgItem( newType );
	//if( pStatic )
	//{
	//	pStatic->m_dwColor = 0xffff1111;
	//	CString curTitle = pStatic->GetTitle( );
	//	if( m_bIsGreater )
	//		curTitle = curTitle + strUP;
	//	else curTitle = curTitle + strDOWN;

	//	pStatic->SetTitle( curTitle );
	//}
}

BOOL CWndGuildHousing::InitBySection( GH_SECTION eSection )
{
	SetSection( eSection );
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	if( !pWndCombo )
		return FALSE;
		
	pWndCombo->SetCurSel( 0 );
	
	m_dwComboCurrIK3 = 0;

	if( GS_FURNITURE == eSection )
	{
		SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_LOCKER ) ); //"Guild furniture storage" );
		pWndCombo->EnableWindow( TRUE );
	}
	else if( GS_TELEPORTER == eSection )
	{
		SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_TELEPORTER ) ); //"Teleporter storage" );
		pWndCombo->EnableWindow( FALSE );
	}
	
	AutoAddingComboItems( );
	RefreshItemList( );

	return TRUE;
}

void CWndGuildHousing::AutoAddingComboItems( )
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	if( !pWndCombo )
		return;

	pWndCombo->ResetContent( );

	if( GS_FURNITURE == m_eSection )
	{
		int nIndex = -1;

		nIndex = pWndCombo->AddString( GETTEXT(TID_GAME_GUILDHOUSE_FURKIND) );
		pWndCombo->SetItemData( nIndex, 0 );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_BED ) );
		pWndCombo->SetItemData( nIndex, IK3_BED );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_TABLE ) );
		pWndCombo->SetItemData( nIndex, IK3_TABLE );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_SOFA ) );
		pWndCombo->SetItemData( nIndex, IK3_SOFA );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_DESK ) );
		pWndCombo->SetItemData( nIndex, IK3_DESK );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CHAIR ) );
		pWndCombo->SetItemData( nIndex, IK3_CHAIR );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CASE ) );
		pWndCombo->SetItemData( nIndex, IK3_CASE );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CLOSET ) );
		pWndCombo->SetItemData( nIndex, IK3_CLOSET );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CARPET ) );
		pWndCombo->SetItemData( nIndex, IK3_CARPET );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_WALLPAPER ) );
		pWndCombo->SetItemData( nIndex, IK3_WALLPAPER );

		//////////////////////////////////////////////////////////////////////////
		//	BEGIN110321	길드 가구 보관함 UI 카테고리 수정
// 		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_WARDROBE ) );
// 		pWndCombo->SetItemData( nIndex, IK3_WARDROBE );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CRYSTAL ) );
		pWndCombo->SetItemData( nIndex, IK3_REST );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_BATH ) );
		pWndCombo->SetItemData( nIndex, IK3_BATH );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_DRAWER ) );
		pWndCombo->SetItemData( nIndex, IK3_DRAWER );

		nIndex = pWndCombo->AddString( GETTEXT( TID_MMI_GHOUSE_CABINET ) );
		pWndCombo->SetItemData( nIndex, IK3_CABINET );
		//	END110321
		//////////////////////////////////////////////////////////////////////////

	}
	else if( GS_TELEPORTER == m_eSection )
	{
		LPCTSTR pszString = prj.GetText( TID_MMI_GUILDHOUSE_TELEPORTER );
		int index = pWndCombo->AddString( pszString ); //"TELEPORTER" );
		pWndCombo->SetItemData( index, 0 );
	}

	pWndCombo->SetCurSel( 0 );
}

BOOL CWndGuildHousing::Process()
{
	static CRect sRect;
	static CRect sOldRect = GetWindowRect( );
	static int snBackScrollPos = 0;
	
	// 설치/재설치 모드라면, 현재 Visible상태에 따라 숨김과 복구를 실행한다.( 설치 편의 )
 	if( GUILDHOUSE_PCKTTYPE_SETUP == GuildHouse->m_iMode )
 	{
		sRect = m_rectCurrentWindow;

 		if( !m_bDeploying )
 		{
 			m_bDeploying = TRUE;
 			SetVisible( FALSE );
		
			SAFE_DELETE( m_pGHShowOne );
			m_pGHShowOne = new CWndGHouseShowOneUnit;
			m_pGHShowOne->Initialize( this );
			int wndItemIndex = GuildHouse->m_nWndDeployingIndex;
			
			FLASSERT( wndItemIndex > -1 && wndItemIndex < (int)( m_cWndItems.size() ) );

			if( wndItemIndex > -1 && wndItemIndex < (int)( m_cWndItems.size() ) )
				m_pGHShowOne->SetItem( m_cWndItems[ wndItemIndex ] );
		
		}
	}
	else
	{
		if( m_bDeploying )
		{
			SetVisible( TRUE );
			m_bDeploying = FALSE;
			GuildHouse->m_nWndDeployingIndex = -1;
			SAFE_DELETE( m_pGHShowOne );
		}
	}

	CWndStatic* pEndTime   = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	if( !pEndTime )
		return FALSE;

	time_t	t = (time_t)GuildHouse->GetUpkeepTime() - time_null();
	if(t < 0)	
	{
		pEndTime->SetTitle( GETTEXT( TID_GAME_GUILDHOUSE_EXPIRATION ) ); //길드하우스의 유지기간이 종료되었습니다.
		return CWndBase::Process();
	}
		
	CTimeSpan	ts( t );
	static CString	strDays, strHours, strMins, strSecs;
	CString strBind;
	const CString strSpace = " ";

	if( ts.GetDays( ) )
	{
		strDays.Format( prj.GetText( TID_PK_LIMIT_DAY ), ts.GetDays() );
		strBind += ( strSpace + strDays );
	}
	if( ts.GetHours( ) )
	{
		strHours.Format( prj.GetText(TID_PK_LIMIT_HOUR ), ts.GetHours() );
		strBind += ( strSpace + strHours );
	}
	if( ts.GetMinutes( ) )
	{
		strMins.Format( prj.GetText(TID_PK_LIMIT_MINUTE ), ts.GetMinutes() );
		strBind += ( strSpace + strMins );
	}
	if( ts.GetSeconds( ) )
	{
		strSecs.Format( prj.GetText(TID_PK_LIMIT_SECOND ), ts.GetSeconds() );
		strBind += ( strSpace + strSecs );
	}

	pEndTime->SetTitle( strBind );

	return CWndBase::Process();
}

void CWndGuildHousing::SetEnableInstallBtns( BOOL bEnable )
{
	for( int i = 0; i < GH_MAX_VIEW_CAPACITY; ++i )
	{
		m_wndButton[ i ].EnableWindow( bEnable );
	}
}

void CWndGuildHousing::CheckChannel( )
{
	if( GUILDHOUSE_PCKTTYPE_SETUP == GuildHouse->m_iMode || GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->m_iMode )
	{
		if( !GuildHouse->IsSetFurnitureChannel( ) )
			g_WndMng.PutString( GETTEXT( TID_GAME_GUILDHOUSE_INSIDE_INSTALL_CHANNEL  ) );	//현재 채널에서는 설치 안댄다..
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CWndGHouseShowOneUnit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndGHouseShowOneUnit::CWndGHouseShowOneUnit( )
{
	m_bNoCloseButton = TRUE;
}

CWndGHouseShowOneUnit::~CWndGHouseShowOneUnit( )
{
}

BOOL CWndGHouseShowOneUnit::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GH_SHOWONE, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndGHouseShowOneUnit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// 윈도를 중앙, 탑으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( (int)( rectRoot.right * 0.5f - rectWindow.Width() * 0.5f ), 0 );
	Move( point );
}

void CWndGHouseShowOneUnit::OnDraw( C2DRender* p2DRender )
{
	//Render icon
	LPWNDCTRL pCustom = GetWndCtrl( WIDC_LISTBOX1 );
	if( !pCustom )
		return;

 	CPoint pt( 0, 0 );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		pt.x = pCustom->rect.left + 10;
		pt.y = pCustom->rect.top;
	}
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		pt.x = pCustom->rect.left + 10;
		pt.y = pCustom->rect.top + 4;
	}


	PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( m_kItem.m_nIndex );
 	if( !pProp )
		return;
 
 	CTexture* pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pProp->szIcon), WNDCOLOR_DEFAULT_KEY );
 	if( pTexture )
 		p2DRender->RenderTexture( pt, pTexture );
 
 	// 이름
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->TextOut( pCustom->rect.left + 50, pCustom->rect.top + 12, m_kItem.m_strName, m_dwColor );
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->TextOut( pCustom->rect.left + 50, pCustom->rect.top + 14, m_kItem.m_strName, m_dwColor );
 
 	// 남은 시간
 	bool bGoTime = true;
 	time_t		t = (time_t)m_kItem.m_dwTime - time_null();
 	if(t < 0)	
 	{
 		t = m_kItem.m_dwTime;	// 만료된것은 자동으로 빠지기 때문에 이경우 아직 설치가 안됀 경우다.( Prop 시간 설정 )
 		bGoTime = false;
 	}
 	CTimeSpan	ts( t );
 	CString		strTime;

	if( ts.GetDays( ) )
		strTime.Format( prj.GetText( TID_PK_LIMIT_DAY ), ts.GetDays() );
	else if( ts.GetHours( ) )
		strTime.Format( prj.GetText(TID_PK_LIMIT_HOUR ), ts.GetHours() );
	else 
		strTime.Format( prj.GetText(TID_PK_LIMIT_MINUTE ), ts.GetMinutes() );

	if( !bGoTime )
		strTime = strTime + CString( " (NEW)" );		//설치가 한번도 안댄( 시간이 안간다 )
	else strTime = strTime + CString( " (▶)" );		//설치가 한번이상 되어서 시간이 흐르는중 

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->TextOut( pCustom->rect.left + 200, pCustom->rect.top + 12, strTime, m_dwColor );
	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
		p2DRender->TextOut( pCustom->rect.left + 200, pCustom->rect.top + 14, strTime, m_dwColor );

	static DWORD tmOld = timeGetTime( );
	static BOOL bRenderBox = TRUE;

	if( ( g_tmCurrent - tmOld ) > 600 )
	{
		tmOld = timeGetTime();
		bRenderBox = !bRenderBox; 
	}
	
	if( bRenderBox )
	{	
		PFLCOLOR_WNDGHOUSINGSHOWONEUNIT	pColorWndGHousingShowOneUnit( g_FLColorManager->GetWndGHousingShowOneUnit() );
		p2DRender->RenderFillRect( pCustom->rect, pColorWndGHousingShowOneUnit->m_stBoxFill.GetColor() );
	}
}






//CWndGuildHouseBid( 길드하우스 입찰 )
CWndGuildHouseBid::CWndGuildHouseBid( )
: m_bMaster( FALSE ), 
m_n64TenderPenya( 0 ),
m_bWaitResult( FALSE ),
m_dwWaitTime( 0 )
{
	_cBidDatas.reserve( 6 );
}

CWndGuildHouseBid::~CWndGuildHouseBid( )
{
	_cBidDatas.clear();
}

BOOL CWndGuildHouseBid::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GH_BID, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndGuildHouseBid::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	//check validity
	CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pEdit2 = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );

	if( !pEdit1 || !pEdit2 || !pWndListBox )
	{
		Destroy( );
		return;
	}

	// 윈도를 중앙, 탑으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( (int)( rectRoot.right * 0.5f - rectWindow.Width() * 0.5f ), 0 );
	Move( point );

	CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus() ;
	if( !pMoverTarget )
	{
		g_WndMng.OpenMessageBox( "Can not find selected NPC" );
		Destroy( );
		return;
	}

	pEdit1->AddWndStyle( EBS_NUMBER );
	pEdit2->AddWndStyle( EBS_NUMBER );

	CWndStatic* pWndPenya = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	pWndPenya->AddWndStyle(WSS_MONEY);
	pWndPenya->AddWndStyle( WSS_ALIGNHRIGHT );


	ResetInputMoneyWindows( );

	//마스터가 아닐경우(길드가 없을경우또한) 입찰버튼 비활성
    CGuild* pGuild = g_pPlayer->GetGuild();
	if( pGuild )	
		m_bMaster = pGuild->IsMaster( g_pPlayer->m_idPlayer );		//GetId()가 아님!!!!!!!!!!!!!!!!!!

	pWndListBox->AddString( prj.GetText( TID_MMI_GHOUSE_NOTBIDTURMNOTLIST ) );
	g_DPlay.SendReqGuildHouseTenderMainWnd( WI_GUILDHOUSE_MIDDLE, pMoverTarget->GetIndex() );
}

void CWndGuildHouseBid::OnDraw( C2DRender* p2DRender )
{
	if( m_bMaster && m_bWaitResult )
	{
		CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		if( !pWndBtnOK )
			return;

		DWORD dwDts = g_tmCurrent - m_dwWaitTime;

//		CRect rect = pWndBtnOK->GetWindowRect();
//		CString str;
//		str.Format( "%d Sec...", dwDts / 1000 );
//		p2DRender->TextOut( rect.left + 2, rect.bottom - 2, str, 0xffff0000 );

		if( dwDts > 5000 ) // 서버로부터 결과응답 대기중이고 , 5초가 지났으면 
		{
			m_dwWaitTime = 0;
			m_bWaitResult = FALSE;
		
			CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
			if( pWndBtnOK )
				pWndBtnOK->EnableWindow( TRUE );

		}
	}

}

BOOL CWndGuildHouseBid::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_LISTBOX1:	//하우스리스트중 하나가 선택됐다.
		RequestCurrHouseInfo( );		
		break;

	case WIDC_BUTTON1: // 입찰하기
		{
			CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
			CWndEdit* pEdit2 = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
			int nCurSel = pWndListBox->GetCurSel( );
			if( nCurSel < 0 || nCurSel >= (int)_cBidDatas.size() )
				break;

			GHBidData& kData = _cBidDatas[ nCurSel ];
	
			__int64 penya = 0, penrin = 0;
			CString str1 =  pEdit1->GetString( );
			CString str2 = pEdit2->GetString( );

			if( str1.GetLength() > 10 || str2.GetLength() > 10 )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_NOTMONEYCONDITION ) );
				ResetInputMoneyWindows( );
				break;
			}

			//	100805	CSTRING GETBUFFER
			penya = _atoi64( str1.GetString() );
			penrin = _atoi64( str2.GetString() );

		//	sscanf( str1.GetBuffer(0), "%d", &penya );
		//	sscanf( str2.GetBuffer(0), "%d", &penrin );

			if( penya < 0 || penrin < 0 )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_NOTMONEYCONDITION ) );
				ResetInputMoneyWindows( );
				break;
			}

			__int64 totalPenya = ( penrin * 100000000 ) + penya;

			//폐냐로만 21억을 초과할수없다. 단 펜린으로 초과가능 
			if( penya > 0x7d2b7500 )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_MONEYEXCESS ) );
				ResetInputMoneyWindows( );
				break;
			}

			if( totalPenya < (__int64)kData._nBidMinPenya )
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_GHOUSE_MINIMUMMONEYBID ) );
				ResetInputMoneyWindows( );
				break;
			}

//			if( penrin > 0 )
//			{
//				CString temp;
//				temp.Format( " (%d penrin, %d penya) Are you sure? ", penrin, penya );
//				if( AfxMessageBox( (LPCTSTR)temp, MB_OKCANCEL ) == IDCANCEL )
//				{
//					ResetInputMoneyWindows( );
//					break;
//				}
//			}
									
			g_DPlay.SendReqGuildHouseTenderJoin( kData._id, (int)penrin, (int)penya );

			// 중복 클릭 금지 .. 응답을 받아서 풀어주자.
			SetEnableWindow_Apply( FALSE, TRUE );
		}

		break;
		
	case WIDC_BUTTON2:	// 닫기
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndGuildHouseBid::SetEnableWindow_Apply( BOOL bEnable, BOOL bWait )
{
	if( !m_bMaster )
		return;

	CWndButton* pWndBtnOK = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if( pWndBtnOK )
		pWndBtnOK->EnableWindow( bEnable );

	if( bWait )
	{
		m_bWaitResult = TRUE;
		m_dwWaitTime = g_tmCurrent; 
	}
}

void CWndGuildHouseBid::ResetInputMoneyWindows( )
{
	CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pEdit2 = (CWndEdit*) GetDlgItem( WIDC_EDIT2 );

	pEdit1->SetString( "" );
	pEdit2->SetString( "" );
}

void CWndGuildHouseBid::RequestCurrHouseInfo( )
{
	//현재 선택되어진 하우스에대해서 세부항목 요청
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	int nCurSel = pWndListBox->GetCurSel( );
	if( nCurSel < 0 || nCurSel >= (int)_cBidDatas.size() )
		return;

	GHBidData& kData = _cBidDatas[ nCurSel ];
	
	g_DPlay.SendReqGuildHouseTenderInfoWnd( kData._id );

	CWndListBox* pWndListBox2 = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	if( pWndListBox2 )
	{
		pWndListBox2->ResetContent( );
		pWndListBox2->AddString( "Updating list..." );
	}
}

GHBidData* CWndGuildHouseBid::FindData( DWORD houseID )
{
	for( GHBidDataIter iter = _cBidDatas.begin(); iter != _cBidDatas.end(); ++iter )
	{
		GHBidData& kData = *iter;
		if( kData._id == houseID )
			return &kData;
	}

	return NULL;
}

void CWndGuildHouseBid::UpdateData_HouseList( DWORD houseID, const TCHAR* szName )
{
	//if cannot find, create new data
	GHBidData* pData = FindData( houseID );
	if( pData )
	{
		pData->_name = CString( szName );
	}
	else 
	{
		GHBidData kNewBid;
		kNewBid._id = houseID;
		kNewBid._name = CString( szName );

		_cBidDatas.push_back( kNewBid );
	}
}
	
void CWndGuildHouseBid::UpdateData_HouseInfo( DWORD houseID, const int nMinPenya, __int64 n64TenderPenya, std::vector< DWORD >& guildIDs )
{
	GHBidData* pData = FindData( houseID );
	if( pData )
	{
		pData->_nBidMinPenya = nMinPenya;
		pData->_cGuildList = guildIDs;
	}

	CGuild* pGuild = g_pPlayer->GetGuild( );
	if( pGuild )
	{
		std::vector<DWORD>::iterator finder = std::find( guildIDs.begin(), guildIDs.end(), pGuild->GetGuildId() );
		if( finder != guildIDs.end() )
		{
			m_n64TenderPenya = n64TenderPenya;
			FLASSERT( m_n64TenderPenya > 0 );
		}
	}
}

void CWndGuildHouseBid::RefreshWnd_HouseList( )
{
	//하우스 리스트 업뎃, 세부정보창은 초기화
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndList->ResetContent( );

	CString str;
	int seq = 1;
	for( GHBidDataIter iter = _cBidDatas.begin(); iter != _cBidDatas.end(); ++iter )
	{
		GHBidData& kData = *iter;
		//	100805	CSTRING GETBUFFER
		str.Format( "%d. %s", seq, kData._name.GetString() );

		pWndList->AddString( (LPCTSTR) str );

		++seq;
	}

	CWndListBox* pWndGuildList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	if( !pWndGuildList )
		return;

	pWndGuildList->ResetContent( );
	
	CWndStatic* pWndPenya = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	if( !pWndPenya )
		return;

	pWndPenya->SetTitle( "0" );

	//입력창 초기화
	CWndEdit* pWndEdit = (CWndEdit *)GetDlgItem( WIDC_EDIT1 );
	if( !pWndEdit )
		return;
	
	pWndEdit->Empty();
}

void CWndGuildHouseBid::RefreshWnd_HouseInfo( )
{
	//해당 하우스의 세부정보 업뎃
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	if( !pWndList )
		return;

	CWndListBox* pWndGuildList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX2 );
	if( !pWndGuildList )
		return;

	size_t nSelected = pWndList->GetCurSel();
	FLASSERT( nSelected < _cBidDatas.size() );

	GHBidData& kData = _cBidDatas[ nSelected ];

	//길드목록창 갱신
	pWndGuildList->ResetContent( );
	CString str;
	char buffer[ 64 ] = {0,};

	if( kData._cGuildList.empty() )
	{
		//현재 입찰길드 없음
		pWndGuildList->AddString( prj.GetText( TID_MMI_GHOUSE_NOTFOUNDBIDGUILD ) );
	}
	else
	{
		int seq = 1;
		char* szGuildName = NULL;
		for( std::vector<DWORD>::iterator iter = kData._cGuildList.begin(); iter != kData._cGuildList.end(); ++iter, ++seq )
		{
			DWORD id = *iter;
			CGuild* pGuild = g_GuildMng.GetGuild( id );
			if( !pGuild )
			{
				static char * unknown_guild = "unknown guild";
				szGuildName = unknown_guild;
			//	pWndGuildList->AddString( "unknown guild" );
			//	continue;
			}
			else
			{
				szGuildName = pGuild->m_szGuild;

				//입찰금 표시 ( 내길드고 입찰금 > 0 )
				if( g_pPlayer->GetGuild( ) == pGuild && 0 < m_n64TenderPenya )		
					FLInt64ToString( m_n64TenderPenya, buffer, _countof( buffer ), 10 );		//arg3 : 기수
				else FLStrcpy( buffer, _countof( buffer ), "?" );
			}

			//gmpbigsun( 20101215 ) : "페냐" 긁어옴
			str.Format( "%d. %s   %s %s", seq, szGuildName, buffer, prj.GetText( TID_GAME_SELLING_COST_0 ) );
			pWndGuildList->AddString( (LPCTSTR)str );
		}
	}

	//최소폐냐 갱신
	CWndStatic* pWndPenya = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	if( !pWndPenya )
		return;

	str.Empty();
	str.Format( "%d", kData._nBidMinPenya );
//	MakeMoneyStyle( str );
	pWndPenya->SetTitle( str );

	//입력창 초기화
	ResetInputMoneyWindows();
}

void CWndGuildHouseBid::MakeMoneyStyle( OUT CString& str )
{
	//천단위마다 ','추가해서 뱉어줌( 100000000 -> 100,000,000 )
	CString rst;
	int len = str.GetLength( );
	if( len < 4 )
		return;

	for( int i = 0; i < len; ++i )
	{
		if( i > 0 && i != (len-1) && i % 3 == 0 )		// i가 3의 배수이면
			rst.Insert( rst.GetLength(), ',' );
	
		rst.Insert( rst.GetLength(), str.GetAt( i ) );
	}

	str = rst;
}

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndGuildHouseNotice::CWndGuildHouseNotice( )
:	m_pEditComment( NULL )
{
	ZeroMemory( &m_kData, sizeof( T_PACKET_GUILDHOUSE_INFO ) );
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndGuildHouseNotice::~CWndGuildHouseNotice( )
{
	SAFE_DELETE( m_pEditComment );
}


//---------------------------------------------------------------------------------------------
// 업데이트 전 초기화
// param	: 부모 윈도우, 윈도우 아이디
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndGuildHouseNotice::Initialize( CWndBase* pWndParent, DWORD ) 
{ 
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDHOUSE_NOTICE, 0, CPoint( 0, 0 ), pWndParent );
} 


//---------------------------------------------------------------------------------------------
// 업데이트 전 초기화
// param	: void
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndGuildHouseNotice::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();

	FLERROR_ASSERT_LOG_RETURN_VOID( g_pPlayer, _T("[윈도우 중형 길드 하우스 공지] 플레이어 포인터가 유효하지 않습니다.") );
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndGuildHouseNotice::OnDraw( C2DRender* p2DRender )
{
}


//---------------------------------------------------------------------------------------------
// 자식에게 알림
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
BOOL CWndGuildHouseNotice::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		switch( nID )
		{
		case WIDC_BUTTON1:	//코멘트 작성
			{
				CWndStatic* pWndComment = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
				FLERROR_ASSERT_LOG_DO( pWndComment, break, _T("[윈도우 중형 길드 하우스 공지] 알림 윈도우 포인터가 유효하지 않습니다.") );

				CString strComment = pWndComment->GetTitle();

				SAFE_DELETE( m_pEditComment );
				m_pEditComment = new CWndGHNoticeEdit;
				FLERROR_ASSERT_LOG_DO( m_pEditComment, break, _T("[윈도우 중형 길드 하우스 공지] 알림 입력 윈도우 포인터가 유효하지 않습니다.") );
				
				m_pEditComment->Initialize( this );
				 m_pEditComment->SetEditString( _countof( m_kData.szGuildHouseComment ), strComment.GetString() );
			}
			break;

		case APP_GUILDHOUSE_NOTICE_EDIT: // 코멘트 에디트 윈도로부터 작성이 완료됨
			{
				FLStrcpy( m_kData.szGuildHouseComment, _countof( m_kData.szGuildHouseComment ), m_pEditComment->GetEditString() );
				g_DPlay.SendChangeGuildHouseComment( &m_kData );

				CWndStatic* pWndComment = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
				FLERROR_ASSERT_LOG_DO( pWndComment, break, _T("[윈도우 중형 길드 하우스 공지] 윈도우 알림 포인터가 유효하지 않습니다.") );
				pWndComment->SetTitle( m_kData.szGuildHouseComment );
			}
			break;
		}
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		switch( nID )
		{
		case WIDC_BUTTON1:	//코멘트 작성
			{
				CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
				FLERROR_ASSERT_LOG_DO( pWndText, break, _T("[윈도우 중형 길드 하우스 공지] 알림 텍스트 윈도우 포인터가 유효하지 않습니다.") );

				CString strComment = pWndText->m_string;

				SAFE_DELETE( m_pEditComment );
				m_pEditComment = new CWndGHNoticeEdit;
				FLERROR_ASSERT_LOG_DO( m_pEditComment, break, _T("[윈도우 중형 길드 하우스 공지] 알림 입력 윈도우 포인터가 유효하지 않습니다.") );
				
				m_pEditComment->Initialize( this );
				m_pEditComment->SetEditString( _countof( m_kData.szGuildHouseComment ), strComment.GetString() );
			}
			break;

		case APP_GUILDHOUSE_NOTICE_EDIT: // 코멘트 에디트 윈도로부터 작성이 완료됨
			{
				FLStrcpy( m_kData.szGuildHouseComment, _countof( m_kData.szGuildHouseComment ), m_pEditComment->GetEditString() );
				g_DPlay.SendChangeGuildHouseComment( &m_kData );

				CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
				FLERROR_ASSERT_LOG_DO( pWndText, break, _T("[윈도우 중형 길드 하우스 공지] 윈도우 알림 포인터가 유효하지 않습니다.") );
				pWndText->m_string = m_kData.szGuildHouseComment;
			}
			break;
		}
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


//---------------------------------------------------------------------------------------------
// 데이터 설정
// param	: 렌더러
// return	: void 
//---------------------------------------------------------------------------------------------
void CWndGuildHouseNotice::SetData( const T_PACKET_GUILDHOUSE_INFO& kData ) 
{
	CWndStatic* pWndTitle = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndTitle, _T("[윈도우 중형 길드 하우스 공지] 윈도우 타이틀 포인터가 유효하지 않습니다.") );

	pWndTitle->SetTitle( prj.GetText( kData.dwGuildHouseTitleIndex ) );

	CWndStatic* pWndGuildName = (CWndStatic*)GetDlgItem( WIDC_STATIC1 );
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndGuildName, _T("[윈도우 중형 길드 하우스 공지] 윈도우 길드 이름 포인터가 유효하지 않습니다.") );

	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	FLERROR_ASSERT_LOG_RETURN_VOID( pWndButton1, _T("[윈도우 중형 길드 하우스 공지] 공지 수정 버튼이 유효하지 않습니다.") );

	CGuild* pGuild = g_GuildMng.GetGuild( kData.dwGuildId );
	if( pGuild )
	{
		pWndGuildName->SetTitle( pGuild->m_szGuild );

		BOOL bMaster( pGuild->IsMaster( g_pPlayer->m_idPlayer ) );
		pWndButton1->EnableWindow( bMaster );
	}
	else
	{
		if( kData.dwGuildId == NULL_ID )
		{
			pWndGuildName->m_dwColor = 0xFFFF1111;
			pWndGuildName->SetTitle( prj.GetText( TID_MMI_GUILD_NOTGUILD ) ); // "주인없음"

			pWndButton1->EnableWindow( FALSE );
		}
		else 
			FLERROR_ASSERT_LOG_RETURN_VOID( NULL, _T("[윈도우 중형 길드 하우스 공지] 길드를 찾을 수 없음.") );
	}


	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		CWndStatic* pWndComment = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
		FLERROR_ASSERT_LOG_RETURN_VOID( pWndComment, _T("[윈도우 중형 길드 하우스 공지] 윈도우 알림 포인터가 유효하지 않습니다.") );
		pWndComment->SetTitle( kData.szGuildHouseComment );
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
		FLERROR_ASSERT_LOG_RETURN_VOID( pWndText, _T("[윈도우 중형 길드 하우스 공지] 윈도우 알림 포인터가 유효하지 않습니다.") );
		pWndText->m_string = kData.szGuildHouseComment;
	}

	m_kData = kData;
}


//----------------------------------------------------------------------------------------------------
//CWndGHNoticeEdit
CWndGHNoticeEdit::CWndGHNoticeEdit( )
{
}

CWndGHNoticeEdit::~CWndGHNoticeEdit( )
{
}

BOOL CWndGHNoticeEdit::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILDHOUSE_NOTICE_EDIT, 0, CPoint( 0, 0 ), pWndParent );
} 

void CWndGHNoticeEdit::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	//중앙
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

void CWndGHNoticeEdit::OnDraw( C2DRender* p2DRender )
{
}

BOOL CWndGHNoticeEdit::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_BUTTON1:	//OK
		//코멘트작성이 완료되면 부모에게 알리고 끝
		m_pParentWnd->OnChildNotify( MB_OK, this->GetWndId(), NULL );
		break;
	case WIDC_BUTTON2:	//Cancel
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

const char* CWndGHNoticeEdit::GetEditString( )
{
	CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	
	int n = strlen( pEdit1->m_string.GetString() );
	return pEdit1->m_string.GetString();
}

void CWndGHNoticeEdit::SetEditString( const int nMAX, const char* szComment )
{
	int len = _tcslen( szComment );

	if( CS_VER1 == _GetContentState( CT_NEWUI_19 ) )
	{
		CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
		pEdit1->SetMaxStringNumber( nMAX - 1 );	

		if( len <= nMAX )
			pEdit1->SetString( szComment, 0xff1111ff );
	}

	if( CS_VER2 == _GetContentState( CT_NEWUI_19 ) )
	{
		CWndEdit* pEdit1 = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
		pEdit1->SetMaxStringNumber( nMAX - 2 );	

		if( len <= nMAX )
			pEdit1->SetString( szComment );
	}
}

