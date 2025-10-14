
#include "StdAfx.h"

#include "WndElldinsJar.h"

#define ITEMNUMBER_POS_OFFSET	11

// 세부 로그를 출력하려면 활성화 시키세요.
//#define WNDELLDINSJAR_SHOW_DETAIL_LOG

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndElldinsJar::CWndElldinsJar()
:	m_pFillTexture( NULL )
,	m_pUnSealedElldinsJar( NULL )
,	m_p1stPotion( NULL )
,	m_p2ndPotion( NULL )
,	m_dwUnSealElldinsJarID( NULL )
,	m_dw1stPostionID( NULL )
,	m_dw2ndPostionID( NULL )
,	m_pWndButtonOK( NULL )
,	m_pWndCurrentSavedHP( NULL )
,	m_pWndCanBeMaximumSaveHP( NULL )
{
	ZeroMemory( m_Rect, sizeof( CRect ) * _countof( m_Rect ) ); 

	OBJID dwID = g_pPlayer->GetRegisterElldinsJarID();
	if( NULL != dwID )
		FindRegisterUnSealElldinsJar( dwID );
	
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndElldinsJar::~CWndElldinsJar()
{
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndElldinsJar::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[엘딘의 항아리] 부모 윈도우 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN( dwWndId,		FALSE, _T("[엘딘의 항아리] 생성 윈도우 아이디가 유효하지 않습니다.") );

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndElldinsJar::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pFillTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ElldinImage01.tga") ), 0, TRUE );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pFillTexture, _T("[엘딘의 항아리] 항아리 채우는 텍스쳐 로딩 실패!!."))

	LPWNDCTRL pJarSlot( GetWndCtrl( WIDC_STATIC3 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( pJarSlot, _T("[엘딘의 항아리] 항아리 슬롯 컨트롤 포인터가 유효하지 않습니다."))

	LPWNDCTRL pJarImage( GetWndCtrl( WIDC_CUSTOM1 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( pJarImage, _T("[엘딘의 항아리] 항아리 슬롯 컨트롤 포인터가 유효하지 않습니다."))

	LPWNDCTRL p1stPotionSlot( GetWndCtrl( WIDC_STATIC4 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( p1stPotionSlot, _T("[엘딘의 항아리] 포션 1 슬롯 컨트롤 포인터가 유효하지 않습니다."))

	LPWNDCTRL p2ndPotionSlot( GetWndCtrl( WIDC_STATIC5 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( p2ndPotionSlot, _T("[엘딘의 항아리] 포션 2 슬롯 컨트롤 포인터가 유효하지 않습니다."))

	m_pWndButtonOK = static_cast<CWndButton*>( GetDlgItem( WIDC_BUTTON1 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndButtonOK, _T("[엘딘의 항아리] OK 버튼 컨트롤 포인터가 유효하지 않습니다."))

	m_pWndCurrentSavedHP = static_cast<CWndStatic*>( GetDlgItem( WIDC_STATIC7 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndCurrentSavedHP, _T("[엘딘의 항아리] 현재 저장 된 체력 컨트롤 포인터가 유효하지 않습니다."))

	m_pWndCanBeMaximumSaveHP = static_cast<CWndStatic*>( GetDlgItem( WIDC_STATIC8 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndCanBeMaximumSaveHP, _T("[엘딘의 항아리] 최대 저장 할 수 있는 체력 컨트롤 포인터가 유효하지 않습니다."))

	m_Rect[eSlot::eSlot_Jar]		= pJarSlot->rect;
	m_Rect[eSlot::eSlot_JarImage]	= pJarImage->rect;
	m_Rect[eSlot::eSlot_1stPotion]	= p1stPotionSlot->rect;
	m_Rect[eSlot::eSlot_2ndPotion]	= p2ndPotionSlot->rect;

	m_pWndButtonOK->EnableWindow( FALSE );

	MoveParentCenter();
} 


//---------------------------------------------------------------------------------------------
// 봉인이 해제 된 엘딘의 항아리 아이템 인가?
// param	: 아이템 아이디
// return	: 찾았다(true) / 못 찾았다(false)
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::IsUnSealElldinsJar( DWORD dwItemID )
{
	CItemContainer<FLItemElem>* pItemContainer = &g_pPlayer->m_Inventory;

	FLItemElem* pItemElem = pItemContainer->GetAtId( dwItemID );	
	FLERROR_ASSERT_LOG_RETURN( pItemElem, false, _T("[엘딘의 항아리] 아이템 포인터가 유효하지 않습니다.") );
	
	PT_ITEM_SPEC pItemSpec = pItemElem->GetProp();
	FLERROR_ASSERT_LOG_RETURN( pItemSpec, false, _T("[엘딘의 항아리] 아이템 스펙 포인터가 유효하지 않습니다.") );
	
	if( pItemSpec->dwItemKind2 != IK2_ELLDINPOTION ||
		pItemSpec->dwItemKind3 != IK3_SAVEPOTION )
		return false;

	if( pItemElem->m_dwKeepTime == NULL )			// 봉인되어 있음	
		return false;

	if( pItemElem->IsFlag( FLItemElem::expired ) )	// 기간 만료 됨
		return false;

	m_pUnSealedElldinsJar	= pItemElem;
	m_dwUnSealElldinsJarID	= pItemElem->m_dwObjId;

	g_pPlayer->SetRegisterElldinsJarID( m_dwUnSealElldinsJarID );

#ifdef WNDELLDINSJAR_SHOW_DETAIL_LOG	
	FLINFO_LOG( PROGRAM_NAME, _T("[엘딘의 항아리] 봉인 된 항아리 등록 : OBJECTID - [ %u ], 이름 - [ %s ]"), pItemElem->m_dwObjId, pItemSpec->szName );
#endif

	return true;
}


//---------------------------------------------------------------------------------------------
// 엘딘의 항아리에 사용 가능한 포션인가?
// param	: 아이템 아이디
// return	: 가능(true) / 불 가능(false)
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::IsPossibleRegisterElldinsJarPotion( DWORD dwItemID, eSlot eSlotIndex )
{
	if( NULL == m_pUnSealedElldinsJar )	// 항아리가 먼져 등록되어 있어야 함
	{
		g_WndMng.PutString( prj.GetText(TID_MMI_ELLDINPOTION_TEXT12), NULL, prj.GetTextColor(TID_MMI_ELLDINPOTION_TEXT12) );
		return true;
	}

	CItemContainer<FLItemElem>* pItemContainer = &g_pPlayer->m_Inventory;

	FLItemElem* pItemElem = pItemContainer->GetAtId( dwItemID );	
	FLERROR_ASSERT_LOG_RETURN( pItemElem, false, _T("[엘딘의 항아리] 아이템 포인터가 유효하지 않습니다.") );
	
	PT_ITEM_SPEC pItemSpec = pItemElem->GetProp();
	FLERROR_ASSERT_LOG_RETURN( pItemSpec, false, _T("[엘딘의 항아리] 아이템 스펙 포인터가 유효하지 않습니다.") );
	
	if( FALSE == pItemSpec->bCanSavePotion )
		return false;
	
	if( eSlotIndex == eSlot::eSlot_1stPotion )
	{
		if( m_p2ndPotion )
		{
			if( m_dw2ndPostionID == pItemElem->m_dwObjId )
			{
				g_WndMng.PutString( prj.GetText(TID_MMI_ELLDINPOTION_TEXT13), NULL, prj.GetTextColor(TID_MMI_ELLDINPOTION_TEXT13) );
				return true;
			}
		}
		
		m_p1stPotion = pItemElem;
		m_dw1stPostionID = pItemElem->m_dwObjId;
	}

	if( eSlotIndex == eSlot::eSlot_2ndPotion )
	{
		if( m_p1stPotion )
		{
			if( m_dw1stPostionID == pItemElem->m_dwObjId )
			{
				g_WndMng.PutString( prj.GetText(TID_MMI_ELLDINPOTION_TEXT13), NULL, prj.GetTextColor(TID_MMI_ELLDINPOTION_TEXT13) );
				return true;
			}
		}

		m_p2ndPotion = pItemElem;
		m_dw2ndPostionID = pItemElem->m_dwObjId;
	}

#ifdef WNDELLDINSJAR_SHOW_DETAIL_LOG	
	FLINFO_LOG( PROGRAM_NAME, _T("[엘딘의 항아리] 물약 : OBJECTID - [ %u ], 이름 - [ %s ]"), pItemElem->m_dwObjId, pItemSpec->szName );
#endif

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템이 인벤토리에 존재하는지 체크한다.
// param	: 슬롯 인덱스
// return	: void
//---------------------------------------------------------------------------------------------
void CWndElldinsJar::CheckItemFromInventory( eSlot eSlotIndex )
{
	CItemContainer<FLItemElem>* pItemContainer = &g_pPlayer->m_Inventory;

	if( eSlot::eSlot_Jar == eSlotIndex )
	{
		if( NULL == m_dwUnSealElldinsJarID )
			return ;
		
		FLItemElem* pItemElem = pItemContainer->GetAtId( m_dwUnSealElldinsJarID );	
		if( NULL == pItemElem )
		{
			m_pUnSealedElldinsJar	= NULL;
			m_dwUnSealElldinsJarID	= NULL;

			g_pPlayer->SetRegisterElldinsJarID( NULL );
		}		
	}

	if( eSlot::eSlot_1stPotion == eSlotIndex )
	{
		if( NULL == m_dw1stPostionID )
			return ;
		
		FLItemElem* pItemElem = pItemContainer->GetAtId( m_dw1stPostionID );	
		if( NULL == pItemElem )
		{
			m_p1stPotion		= NULL;
			m_dw1stPostionID	= NULL;
		}		
	}

	if( eSlot::eSlot_2ndPotion == eSlotIndex )
	{
		if( NULL == m_dw2ndPostionID )
			return ;
		
		FLItemElem* pItemElem = pItemContainer->GetAtId( m_dw2ndPostionID );	
		if( NULL == pItemElem )
		{
			m_p2ndPotion		= NULL;
			m_dw2ndPostionID	= NULL;
		}		
	}
}


//---------------------------------------------------------------------------------------------
// 등록 된 봉인이 해제 된 엘딘의 항아리를 찾는다
// param	: 아이템 아이디
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::FindRegisterUnSealElldinsJar( OBJID dwItemID )
{
	CItemContainer<FLItemElem>* pItemContainer = &g_pPlayer->m_Inventory;
	
	FLItemElem* pItemElem = pItemContainer->GetAtId( dwItemID );	
	FLERROR_ASSERT_LOG_RETURN( pItemElem, false, _T("[엘딘의 항아리] 등록되고 봉인이 해제 된 엘딘의 항아리를 찾을 수 않습니다.") );

	m_pUnSealedElldinsJar	= pItemElem;
	m_dwUnSealElldinsJarID	= pItemElem->m_dwObjId;

#ifdef WNDELLDINSJAR_SHOW_DETAIL_LOG	
	FLINFO_LOG( PROGRAM_NAME, _T("[엘딘의 항아리] 등록 된 봉인이 해제 된 항아리 찾음 : OBJECTID - [ %u ]"), pItemElem->m_dwObjId );
#endif
	return true;
}

//---------------------------------------------------------------------------------------------
// 등록 된 첫번째 물약을 찾는다
// param	: 아이템 아이디
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::FindRegister1stPostion( OBJID dwItemID )
{
	CItemContainer<FLItemElem>* pItemContainer = &g_pPlayer->m_Inventory;
	
	FLItemElem* pItemElem = pItemContainer->GetAtId( dwItemID );	
	FLERROR_ASSERT_LOG_RETURN( pItemElem, false, _T("[엘딘의 항아리] 등록되고 첫번째 물약를 찾을 수 않습니다.") );

	m_p1stPotion	= pItemElem;
	m_dw1stPostionID	= pItemElem->m_dwObjId;

#ifdef WNDELLDINSJAR_SHOW_DETAIL_LOG	
	FLINFO_LOG( PROGRAM_NAME, _T("[엘딘의 항아리] 등록 된 첫번째 물약 찾음 : OBJECTID - [ %u ]"), pItemElem->m_dwObjId );
#endif
	return true;
}

//---------------------------------------------------------------------------------------------
// 등록 된 두번째 물약을 찾는다
// param	: 아이템 아이디
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::FindRegister2ndPostion( OBJID dwItemID )
{
	CItemContainer<FLItemElem>* pItemContainer = &g_pPlayer->m_Inventory;
	
	FLItemElem* pItemElem = pItemContainer->GetAtId( dwItemID );	
	FLERROR_ASSERT_LOG_RETURN( pItemElem, false, _T("[엘딘의 항아리] 등록되고 두번째 물약를 찾을 수 않습니다.") );

	m_p2ndPotion	= pItemElem;
	m_dw2ndPostionID	= pItemElem->m_dwObjId;

#ifdef WNDELLDINSJAR_SHOW_DETAIL_LOG	
	FLINFO_LOG( PROGRAM_NAME, _T("[엘딘의 항아리] 등록 된 두번째 물약 찾음 : OBJECTID - [ %u ]"), pItemElem->m_dwObjId );
#endif
	return true;
}




//---------------------------------------------------------------------------------------------
// 슬롯에 아이콘 그리기
// param	: 렌더러, 아이템, 슬롯 인덱스
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::DrawIconInSlot( C2DRender* p2DRender, FLItemElem* pItem, eSlot eSlotIndex )
{
	if( NULL == pItem )
		return false;

	CTexture* pTexture = pItem->GetTexture();
	FLERROR_ASSERT_LOG_RETURN( pTexture, false, _T("[엘딘의 항아리] 아이템에 텍스쳐 정보가 없습니다.") );

	pTexture->Render( p2DRender, m_Rect[eSlotIndex].TopLeft() );

	if( pItem->m_nItemNum > 1 )
	{
		CString strNumber( _T("") );
		strNumber.Format( _T("%d"), pItem->m_nItemNum );

		
		CD3DFont* pFont = p2DRender->GetFont();
		FLERROR_ASSERT_LOG_RETURN( pFont, false, _T("[엘딘의 항아리] 폰트를 얻어올 수 없음.") );
		
		SIZE StringSize;

		StringSize.cx = 0;
		StringSize.cy = 0;

		PFONTCOLOR_WNDELLDINSJAR pFontColorWndElldinsJar = g_WndFontColorManager->GetWndElldinsJar();

		pFont->GetTextExtent( strNumber, &StringSize, g_xFlyffConfig->GetCodePage() );
		p2DRender->TextOut( (m_Rect[eSlotIndex].right - StringSize.cx) ,  m_Rect[eSlotIndex].bottom - StringSize.cy , strNumber, pFontColorWndElldinsJar->m_stItemCountShadow.GetFontColor() );	
		p2DRender->TextOut( (m_Rect[eSlotIndex].right - StringSize.cx) - 1,  ( m_Rect[eSlotIndex].bottom - StringSize.cy ) - 1 , strNumber, pFontColorWndElldinsJar->m_stItemCount.GetFontColor() );	
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 툴팁 그리기
// param	: 렌더러, 아이템, 슬롯 인덱스
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::DrawTooltip( C2DRender* p2DRender, FLItemElem* pItem, eSlot eSlotIndex )
{
	if( NULL == pItem )
		return false;

	CPoint ptPoint = GetMousePoint();
	if( m_Rect[eSlotIndex].PtInRect( ptPoint ) )
	{
		CPoint	ptTemp = ptPoint;
		CRect	rtTemp = m_Rect[eSlotIndex];

		ClientToScreen( &ptTemp );
		ClientToScreen( &rtTemp );
		
		g_WndMng.PutToolTip_Item( static_cast<FLItemBase*>( pItem ), ptTemp, &rtTemp );
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// HP 글씨 그리기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::DrawTextHP( C2DRender* p2DRender )
{
	FLERROR_ASSERT_LOG_RETURN( m_pWndCurrentSavedHP, false, _T("[엘딘의 항아리] 현재 저장 된 체력 윈도우 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN( m_pWndCanBeMaximumSaveHP, false, _T("[엘딘의 항아리] 최대 저장 할 수 있는 체력 윈도우 포인터가 유효하지 않습니다.") );

	if( NULL == m_pUnSealedElldinsJar )
		return false;

	CString strCurrentHP( _T("") );
	strCurrentHP.Format( _T("%d"), m_pUnSealedElldinsJar->m_nHitPoint );
	
	m_pWndCurrentSavedHP->SetTitle( strCurrentHP );

	PT_ITEM_SPEC pItemSpec = m_pUnSealedElldinsJar->GetProp();
	FLERROR_ASSERT_LOG_RETURN( pItemSpec, false, _T("[엘딘의 항아리] 아이템 속성 포인터가 유효하지 않습니다.") );

	CString strMaximumHP( _T("") );
	strMaximumHP.Format( _T("%d"), pItemSpec->nAdjParamVal[0] );

	m_pWndCanBeMaximumSaveHP->SetTitle( strMaximumHP );

	return true;
}


//---------------------------------------------------------------------------------------------
// 항아리 채우는 물 그리기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::DrawFillTexture( C2DRender* p2DRender )
{
	FLERROR_ASSERT_LOG_RETURN( m_pFillTexture, false, _T("[엘딘의 항아리] 현재 저장 된 체력 윈도우 포인터가 유효하지 않습니다.") );
	
	//-------------------------------------------------------------------------------
	// UV 계산 공식
	// 항아리 물 Texture에서 위와 아래 빈 공간을 제외한 순수한 공간을 계산하여 ( 계산값 : 63 Pixel )
	// 그 값을 100%로 하여 실제 항아리의 최대값에서 저장되어 있는 값으로 채워진 양을 계산하여 ( 예 : 10% )
	// 그만큼의 Texture만 UV를 조정하여 그린다. 단 항아리 밑 부분에 남는 공간이 있음으로 
	// 계산 시 밑에 빈 공간만큼의 UV를 더해서 그린다.
	// 간단하게 말해서 텍스쳐에서 빈 공간을 제외한 순수한 그림 높이를 계산하여 그린다.
	//-------------------------------------------------------------------------------

	#define FURE_JARIMAGE_HEIGHT_PIXEL				63.0f
	#define JARIMAGE_BOTTOM_EMPTYSPACE_HEIGHT_PIXEL	23.0f

	if( NULL == m_pUnSealedElldinsJar )
		return false;

	 if( NULL == m_pUnSealedElldinsJar->m_nHitPoint )
		 return false;

	PT_ITEM_SPEC pItemSpec = m_pUnSealedElldinsJar->GetProp();
	FLERROR_ASSERT_LOG_RETURN( pItemSpec, false, _T("[엘딘의 항아리] 아이템 속성 포인터가 유효하지 않습니다.") );

	FLERROR_ASSERT_LOG_RETURN( pItemSpec->nAdjParamVal[0], false, _T("[엘딘의 항아리] 최대 저장량이 0 입니다. 데이터를 확인하세요.") );

 	float fCurrentSavedHPPercent = static_cast<float>( ( m_pUnSealedElldinsJar->m_nHitPoint * 100.0f ) / pItemSpec->nAdjParamVal[0] );

	float fCalcFureJarImageHeightPixel = static_cast<float>( ( ( FURE_JARIMAGE_HEIGHT_PIXEL * fCurrentSavedHPPercent ) * 0.01f ) + JARIMAGE_BOTTOM_EMPTYSPACE_HEIGHT_PIXEL ); 

	float fTopUVCoord	= ( 1.0f - (fCalcFureJarImageHeightPixel / m_pFillTexture->m_size.cy) );
	float fTopCoord		= m_pFillTexture->m_size.cy - fCalcFureJarImageHeightPixel;

	p2DRender->RenderTextureUVEx(	CPoint( m_Rect[eSlot::eSlot_JarImage].left,	static_cast<int>( m_Rect[eSlot::eSlot_JarImage].top + fTopCoord ) ),
									CPoint( m_pFillTexture->m_size.cx,			static_cast<int>( fCalcFureJarImageHeightPixel ) ), 
									m_pFillTexture,
									0.0f,
									fTopUVCoord,
									1.0f,
									1.0f
								 );

	return true;
}



//---------------------------------------------------------------------------------------------
// OK 버튼을 활성화 할 것인가 체크한다.
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndElldinsJar::CheckEnableOKButton()
{
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndButtonOK, _T("[엘딘의 항아리] OK 버튼 윈도우 포인터가 유효하지 않습니다.") );

	if( NULL == m_pUnSealedElldinsJar )
	{
		m_pWndButtonOK->EnableWindow( FALSE );
		return ;
	}

	if( NULL == m_p1stPotion && NULL == m_p2ndPotion )
	{
		m_pWndButtonOK->EnableWindow( FALSE );
		return ;
	}
 
	m_pWndButtonOK->EnableWindow( TRUE );
}


//---------------------------------------------------------------------------------------------
// OK 버튼 클릭 시 이벤트 처리
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndElldinsJar::ButtonOK_Process()
{
	FLERROR_ASSERT_LOG_RETURN( m_pUnSealedElldinsJar, false, _T("[엘딘의 항아리] 정상적인 상황이 아닙니다 ( 항아리 없음 ).") );

	if( NULL == m_p1stPotion &&
		NULL == m_p2ndPotion )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[엘딘의 항아리] 정상적인 상황이 아닙니다 ( 물약이 둘 다 없음 ).") );

	if( m_p1stPotion )
		g_DPlay.SendFillElldinsJarWithPotion( m_p1stPotion->m_dwObjId, m_pUnSealedElldinsJar->m_dwObjId );	

	if( m_p2ndPotion )
		g_DPlay.SendFillElldinsJarWithPotion( m_p2ndPotion->m_dwObjId, m_pUnSealedElldinsJar->m_dwObjId );	

	return true;
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndElldinsJar::OnDraw(C2DRender* p2DRender)
{
	FLERROR_ASSERT_LOG_RETURN_VOID( p2DRender, _T("[엘딘의 항아리] 렌더러 포인터가 유효하지 않습니다.") );

	CheckEnableOKButton();
	
	if( NULL != m_dwUnSealElldinsJarID )
	{
		FindRegisterUnSealElldinsJar( m_dwUnSealElldinsJarID );
		g_pPlayer->SetRegisterElldinsJarID( m_dwUnSealElldinsJarID );
	}
	if( NULL != m_dw1stPostionID)
		FindRegister1stPostion( m_dw1stPostionID );
	if( NULL != m_dw2ndPostionID)
		FindRegister2ndPostion( m_dw2ndPostionID );

	CheckItemFromInventory( eSlot::eSlot_Jar );
	CheckItemFromInventory( eSlot::eSlot_1stPotion );
	CheckItemFromInventory( eSlot::eSlot_2ndPotion );

	DrawFillTexture( p2DRender );
	DrawTextHP( p2DRender ); 

	DrawIconInSlot( p2DRender, m_pUnSealedElldinsJar, eSlot::eSlot_Jar );
	DrawIconInSlot( p2DRender, m_p1stPotion,eSlot::eSlot_1stPotion );
	DrawIconInSlot( p2DRender, m_p2ndPotion, eSlot::eSlot_2ndPotion );

	DrawTooltip( p2DRender, m_pUnSealedElldinsJar, eSlot::eSlot_Jar );
	DrawTooltip( p2DRender, m_p1stPotion,eSlot::eSlot_1stPotion );
	DrawTooltip( p2DRender, m_p2ndPotion, eSlot::eSlot_2ndPotion );
}


//---------------------------------------------------------------------------------------------
// 명령이 왔을 때의 이벤트 처리
// param	: 윈도우 아이디, 메시지, 기본 윈도우
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndElldinsJar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( WIDC_BUTTON1 == nID )	// OK 버튼
		ButtonOK_Process();

	return CWndBase::OnCommand( nID, dwMessage, pWndBase );
}


//---------------------------------------------------------------------------------------------
// 아이콘이 드롭 됬을 때의 이벤트 처리
// param	: pShortcut, 마우스 위치
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndElldinsJar::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	FLERROR_ASSERT_LOG_RETURN( pShortcut, FALSE, _T("[엘딘의 항아리] 단축 아이콘의 포인터가 유효하지 않습니다."))

	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	FLERROR_ASSERT_LOG_RETURN( pWndFrame, FALSE, _T("[엘딘의 항아리] 윈도우 프레임의 포인터가 유효하지 않습니다."))

	if( pShortcut->m_dwShortcut != SHORTCUT_ITEM &&
		pWndFrame->GetWndId() != APP_INVENTORY )
		return FALSE;
		
	FLItemElem* pItemElem = static_cast<FLItemElem*>( g_pPlayer->GetItemId( pShortcut->m_dwId ) );
	FLERROR_ASSERT_LOG_RETURN( pItemElem, FALSE, _T("[엘딘의 항아리] 단축 아이콘 아이템의 포인터가 유효하지 않습니다."))

	bool bResult( true );

	if( PtInRect(&m_Rect[eSlot::eSlot_Jar], point) )
		bResult = IsUnSealElldinsJar( pShortcut->m_dwId );

	if( false == bResult )
	{
		g_WndMng.PutString( prj.GetText(TID_MMI_ELLDINPOTION_TEXT02), NULL, prj.GetTextColor(TID_MMI_ELLDINPOTION_TEXT02) );
		CheckEnableOKButton();
		return bResult;
	}

	if( PtInRect(&m_Rect[eSlot::eSlot_1stPotion], point) )
		bResult = IsPossibleRegisterElldinsJarPotion( pShortcut->m_dwId, eSlot::eSlot_1stPotion );

	if( PtInRect(&m_Rect[eSlot::eSlot_2ndPotion], point) )
		bResult = IsPossibleRegisterElldinsJarPotion( pShortcut->m_dwId, eSlot::eSlot_2ndPotion );

	if( false == bResult )
	{
		g_WndMng.PutString( prj.GetText(TID_MMI_ELLDINPOTION_TEXT08), NULL, prj.GetTextColor(TID_MMI_ELLDINPOTION_TEXT08) );
	}

	CheckEnableOKButton();
	return TRUE;
}


#pragma warning( default : 4482 )

