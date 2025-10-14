

#include "StdAfx.h"
#include "WndCommonItemExchange.h"

namespace WndCommonItemExchange
{
	static const BYTE		HEIGHT_OF_ONE_EXCHANGE_ITEM( 36 );

	static const BYTE		EXCHANGE_ITEM_ICON_DRAWOFFSET_X( 8 );
	static const BYTE		DRAWITEMCOUNT_OFFSET_X( 2 ); 

	static const char		EXCHANGE_ITEM_VIEWPORT_OFFSET_TOP( 4 );
	static const char		EXCHANGE_ITEM_VIEWPORT_OFFSET_LEFT( 1 );
	static const char		EXCHANGE_ITEM_VIEWPORT_OFFSET_RIGHT( 6 );
	static const char		EXCHANGE_ITEM_VIEWPORT_OFFSET_BOTTOM( 3 );

	static const BYTE		EXCHANGE_ARROW_COUNT( 1 );
};


//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndCommonItemExchange::CWndCommonItemExchange()
:	m_pWndDescription( NULL )
,	m_pWndExchangeList( NULL )
,	m_pTexExchangeArrow( NULL )
,	m_dwMMIDefineIndex( -1 )
,	m_dwCurrentDescPageIndex( NULL )
,	m_nTotalDescPageCount( NULL )
,	m_byConditionItemMaxCount( NULL )
,	m_byPaymentItemMaxCount( NULL )
,	m_bSendPacket( false )
,	m_byPaymentItemColorChangeCount( NULL )
,	m_pWndCommonItemExchangeConfirm( NULL )
,	m_dwPossibleMaxExchangeCount( NULL )
{
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndCommonItemExchange::~CWndCommonItemExchange()
{
	m_mapExchangeItem.clear();
	m_vecIconDrawRect.clear();
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonItemExchange::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( dwWndId,	FALSE, _T("[범용 아이템 교환 창] 윈도우 아이디가 유효하지 않습니다.") );
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchange::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pWndDescription = ( static_cast<CWndText*>( GetDlgItem( WIDC_TEXT1 ) ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndDescription, _T("[범용 아이템 교환 창] 텍스트 컨트롤 포인터가 유효하지 않습니다.") );

	m_pWndExchangeList = ( static_cast<CWndListBox*>( GetDlgItem( WIDC_LISTBOX1 ) ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndExchangeList, _T("[범용 아이템 교환 창] 리스트 포인터가 유효하지 않습니다.") );
	
	if( m_pWndExchangeList )
	{
		m_pWndExchangeList->m_bOnItem = TRUE;
		m_pWndExchangeList->m_nLineSpace = (int)( ( HEIGHT_OF_ONE_EXCHANGE_ITEM - m_pWndExchangeList->GetFontHeight( ) ) * 0.5f );
	}

	CString strFullPath( _T("") );

	strFullPath += DIR_THEME;
	strFullPath += DIR_DEFAULT;

	m_pTexExchangeArrow = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("ExchangeArrow.tga") ), 0, TRUE );

	m_PaymentItemColorChangeTimer.Set( SEC( 2 ) );

	MoveParentCenter();
}  


//---------------------------------------------------------------------------------------------
// 아이템 교환 정보 설정
// param	: MMI 정의 인덱스
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::SetItemExchangeInfo(const DWORD dwMMIDefineIndex)
{
	if( -1 == dwMMIDefineIndex )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 아이템 교환 창] MMI 인덱스 정보가 올바르지 않습니다.") );
	
	m_dwMMIDefineIndex = dwMMIDefineIndex;

	if( false == SetDescriptionString() )
		return false;
	
	if( false == CreateExchangeItemList() )
		return false;

	return true;
}


//---------------------------------------------------------------------------------------------
// 설명 문자열을을 설정한다.
// param	: void 
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::SetDescriptionString()
{
	FLERROR_ASSERT_LOG_RETURN( g_pExchangeEvent, false, _T("[범용 아이템 교환 창] 교환 스크립트 포인터가 유효하지 않습니다.") );

	TextIDVec vecDescTextID;

	if( false == g_pExchangeEvent->GetDescriptionTextID( m_dwMMIDefineIndex, &vecDescTextID ) )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 아이템 교환 창] 교환 설명 리스트 얻기를 실패하였습니다.  MMI ID - [ %u ]"), m_dwMMIDefineIndex );

	m_nTotalDescPageCount = vecDescTextID.size();
	
	// 리스트의 개수가 1개라면 다음 페이지 버튼을 비 활성화 한다.
	if( 1 == m_nTotalDescPageCount )
	{
		CWndButton* pNextPage( static_cast<CWndButton*>( GetDlgItem( WIDC_BUTTON1 ) ) );

		if( pNextPage )
		{
			pNextPage->SetVisible( FALSE );
			pNextPage->EnableWindow( FALSE );
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T("[범용 아이템 교환 창] 다음 설명 페이지 버튼 컨트롤 포인터가 유효하지 않습니다.") );		
			FLASSERT( NULL );
		}
	}

	m_pWndDescription->m_string.AddParsingString( prj.GetText( vecDescTextID[m_dwCurrentDescPageIndex] ) );
	m_pWndDescription->ResetString();	

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 리스트 생성하기
// param	: 아이템 리스트
// return	: void
// 설명		: 교환 아이템의 정보는 OnDraw에서 그릴것이고 이 클레스가 들고 있음으로 스크롤 바 때문에 ListBox에 
//			: 더미 데이터를 삽입한다.
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::CreateExchangeItemList()
{
	FLERROR_ASSERT_LOG_RETURN( m_pWndExchangeList, false, _T("[범용 아이템 교환 창] 교환 아이템 리스트 윈도우 포인터가 유효하지 않습니다.") );

	m_mapExchangeItem.clear();

	if( false == g_pExchangeEvent->GetExchangeItemList( m_dwMMIDefineIndex, m_mapExchangeItem ) )
		FLERROR_ASSERT_LOG_RETURN( NULL, false, _T("[범용 아이템 교환 창] 교환 아이템 리스트 정보를 얻어 올 수 없습니다.") );

	m_pWndExchangeList->ResetContent();
	
	int nCount( NULL );
	for( ExchangeItemListItr Iter = m_mapExchangeItem.begin(); Iter != m_mapExchangeItem.end(); ++Iter )
	{
		T_EXCHANGE_ITEM_LIST stData( Iter->second );

		if( stData.mapConditionItem.empty() )
			FLERROR_ASSERT_LOG_DO( NULL, continue, _T("[범용 아이템 교환 창] 필요 아이템 목록 비어 있습니다!! ID - [%u]."), Iter->first );
			
		if( stData.mapPaymentItem.empty() )
			FLERROR_ASSERT_LOG_DO( NULL, continue, _T("[범용 아이템 교환 창] 보상 아이템 목록 비어 있습니다!! ID - [%u]."), Iter->first );

		if( stData.mapConditionItem.size() > m_byConditionItemMaxCount )
			m_byConditionItemMaxCount = stData.mapConditionItem.size();

		if( stData.mapPaymentItem.size() > m_byPaymentItemMaxCount )
			m_byPaymentItemMaxCount = stData.mapPaymentItem.size();
		
		m_pWndExchangeList->AddString( _T("") );
		m_pWndExchangeList->SetItemData( nCount, NULL );

		nCount++;
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// 뷰 포트 설정
// param	: 렌더러
// return	: void
// 추가설명	: 리스트 박스 안에만 그림을 아이템을 그리기 위해서 뷰 포트와 클리핑 영역을 새로 설정한다.
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchange::SetViewPort( C2DRender* const p2DRender )
{
	CRect	rtViewport( m_pWndExchangeList->m_rectWindow );

	rtViewport.left		+= EXCHANGE_ITEM_VIEWPORT_OFFSET_LEFT;
	rtViewport.right	-= m_pWndExchangeList->GetScrollWidth() + EXCHANGE_ITEM_VIEWPORT_OFFSET_RIGHT;
	rtViewport.top		+= EXCHANGE_ITEM_VIEWPORT_OFFSET_TOP;
	rtViewport.bottom	-= EXCHANGE_ITEM_VIEWPORT_OFFSET_BOTTOM;

	ClientToScreen( &rtViewport );

	p2DRender->SetViewport( rtViewport );
}


//---------------------------------------------------------------------------------------------
// 보상 아이템 색상 바꾸기 갱신
// param	: void
// return	: void
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchange::UpdatePaymentItemColorChange()
{
	if( m_PaymentItemColorChangeTimer.TimeOut() )
	{
		++m_byPaymentItemColorChangeCount;
		m_PaymentItemColorChangeTimer.Reset();
		m_PaymentItemColorChangeTimer.Set( SEC(2) );

		if( m_byPaymentItemColorChangeCount >= m_byPaymentItemMaxCount )
			m_byPaymentItemColorChangeCount = NULL;
	}
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchange::OnDraw( C2DRender* p2DRender )
{
	if( NULL == p2DRender )
		return ;

	UpdatePaymentItemColorChange();
	DrawExchangeItemList( p2DRender );
}


//---------------------------------------------------------------------------------------------
// 교환 아이템 목록 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawExchangeItemList( C2DRender* const p2DRender )
{
	if( m_mapExchangeItem.empty() )
		return false;

	SetViewPort( p2DRender );

	m_vecIconDrawRect.clear();

	CRect	rtRect( m_pWndExchangeList->m_rectWindow );
	int		nSelected( m_pWndExchangeList->GetCurSel() );

	int nCount( NULL );
	for( ExchangeItemListItr Iter = m_mapExchangeItem.begin(); Iter != m_mapExchangeItem.end(); ++Iter )
	{
		T_EXCHANGE_ITEM_LIST stData( Iter->second );

		CRect	rtItem( m_pWndExchangeList->GetItemRect( nCount ) );
		CPoint	ptItem( rtItem.TopLeft() + m_pWndExchangeList->m_rectWindow.TopLeft() );

		DrawExchangeItem_ConditionItemIcon( p2DRender, stData.mapConditionItem, ptItem, rtItem );
		DrawExchangeItem_ExchangeArrow( p2DRender, ptItem, rtItem );
		DrawExchangeItem_PaymentItemIcon( p2DRender, stData.mapPaymentItem, ptItem, rtItem );
		DrawExchangeItem_WidthLine( p2DRender, nCount, rtItem );
		DrawExchangeItem_SelectBox( p2DRender, nCount, ptItem, rtItem );

		nCount++;
	}

	DrawToolTip();

	return true;
}


//---------------------------------------------------------------------------------------------
// 교환을 위해 필요한 아이템을 몇개나 가지고 있는지 체크한다
// param	: 교환 아이템 정보
// return	: 아이템이 부족합니다 ( true ) / 아이템이 부족하지 않습니다 ( false )
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::CheckExchangeConditionItemHaveCount( const T_EXCHANGE_ITEM& stData )
{
	bool bNotEnoughItemCount( false );

	if( NULL != stData.dwMinGeneralEnchant &&
		NULL != stData.dwMaxGeneralEnchant )
	{
		int		nItemCount( NULL );
		WORD	wStartCount( MAX_HUMAN_PARTS );
		WORD	wEndCount( MAX_INVENINDEX_QUEST );

		for( WORD wCount = wStartCount; wCount < wEndCount; wCount++ )
		{
			FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( wCount ) );
			if( NULL == pItemElem )
				continue;

			if( stData.dwItemID == pItemElem->m_dwItemId )	
			{
				DWORD dwGeneralEnchantLevel( pItemElem->GetAbilityOption() );
			
				if( dwGeneralEnchantLevel >= stData.dwMinGeneralEnchant &&		// 일반 제련 수치가 스크립트 수치 안에 존재한다면
					dwGeneralEnchantLevel <= stData.dwMaxGeneralEnchant )
				{
					nItemCount += pItemElem->m_nItemNum;
				}
			}
		}

		if( stData.nItemQuantity > nItemCount )
			bNotEnoughItemCount = true;
	}
	else
	{
		if( stData.nItemQuantity > g_pPlayer->m_Inventory.GetItemNumByItemId( stData.dwItemID ) )
			bNotEnoughItemCount = true;
	}

	return bNotEnoughItemCount;
}


//---------------------------------------------------------------------------------------------
// 아이템 그리기 (필요 아이템)
// param	: 렌더러, 아이템 리스트, 위치, 영역
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawExchangeItem_ConditionItemIcon( C2DRender* const p2DRender, const ExchangeItemMap& mapConditionItem, CPoint ptItem, const CRect rtItem )
{
	if( NULL == g_pPlayer )
		return false;

	if( mapConditionItem.empty() )
		return false;

	UINT nCount( NULL );

	for( ExchangeItemCItr pos = mapConditionItem.begin(); pos != mapConditionItem.end(); ++pos )
	{
		bool	bNotEnoughItemCount( false );
		const	T_EXCHANGE_ITEM stData( pos->second );

		const PT_ITEM_SPEC pCondItemProp = g_xSpecManager->GetSpecItem( stData.dwItemID );
		FLERROR_ASSERT_LOG_DO( pCondItemProp, continue, _T("[범용 아이템 교환 창] 교환 필요 아이템 속성을 얻어 올 수 없습니다. COUNT - [%u], INDEX - [%u]"), nCount, stData.dwItemID );

		CTexture* pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pCondItemProp->szIcon ), WNDCOLOR_DEFAULT_KEY );
		if( pTexture )
		{
			CPoint ptIcon( ptItem );
			ptIcon.x += EXCHANGE_ITEM_ICON_DRAWOFFSET_X;	// 처음에 OFFSET 만큼 띄고 그린다

			ptIcon.x += (nCount * EXCHANGE_ITEM_ICON_DRAWOFFSET_X) + (nCount * pTexture->m_size.cx);
			ptIcon.y += static_cast<long>( ( rtItem.Height() - pTexture->m_size.cy) * 0.5f );

			bNotEnoughItemCount = CheckExchangeConditionItemHaveCount( stData );

			if( bNotEnoughItemCount )
				p2DRender->RenderTexture( ptIcon, pTexture, 64 );
			else
				p2DRender->RenderTexture( ptIcon, pTexture );

			// 그리는 영역 정보 저장
			CRect rtIcon( ptIcon.x, ptIcon.y, ptIcon.x + pTexture->m_size.cx, ptIcon.y + pTexture->m_size.cy ); 
			
			ICONDRAWINFO stIconDrawRect;
			stIconDrawRect.m_dwItemID				= stData.dwItemID;
			stIconDrawRect.m_rtItemIconDraw			= rtIcon; 
			stIconDrawRect.m_dwMinGeneralEnchant	= stData.dwMinGeneralEnchant;
			stIconDrawRect.m_dwMaxGeneralEnchant	= stData.dwMaxGeneralEnchant;

			m_vecIconDrawRect.push_back( stIconDrawRect );
			++nCount;

			if( NULL == stData.nItemQuantity )
				continue;

			// 아이템 개수 그리기
			PFONTCOLOR_WNDCOMMONITEMEXCHANGE pFontColorWndItemExchange = g_WndFontColorManager->GetWndCommonItemExchange();

			TCHAR szCount[ 32 ] = { NULL, };
			FLSPrintf( szCount, _countof( szCount ), _T("%d"), stData.nItemQuantity );

			CPoint ptItemCount( rtIcon.BottomRight() );

			CSize StringSize( m_p2DRender->m_pFont->GetTextExtent( szCount ) );

			m_p2DRender->TextOut(	ptItemCount.x - StringSize.cx - DRAWITEMCOUNT_OFFSET_X,
									ptItemCount.y - StringSize.cy , 
									szCount, 
									pFontColorWndItemExchange->m_stItemCount.GetFontColor(),
									pFontColorWndItemExchange->m_stItemCountShadow.GetFontColor() );
		}
	}
	
	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 그리기 (화살표)
// param	: 렌더러, 위치 영역
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawExchangeItem_ExchangeArrow( C2DRender* const p2DRender, CPoint ptItem, const CRect rtItem )
{
	if( NULL == m_pTexExchangeArrow )
		return false;

	CPoint ptIcon( ptItem );
	ptIcon.x += EXCHANGE_ITEM_ICON_DRAWOFFSET_X;	// 처음에 OFFSET 만큼 띄고 그린다

	ptIcon.x += ( m_byConditionItemMaxCount * EXCHANGE_ITEM_ICON_DRAWOFFSET_X ) +
				( m_byConditionItemMaxCount * m_pTexExchangeArrow->m_size.cx );

	ptIcon.y += static_cast<long>( ( rtItem.Height() - m_pTexExchangeArrow->m_size.cy) * 0.5f );

	p2DRender->RenderTexture( ptIcon, m_pTexExchangeArrow );

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 그리기 (보상 아이템)
// param	: 렌더러, 아이템 리스트, 위치, 영역
// return	: void
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawExchangeItem_PaymentItemIcon( C2DRender* const p2DRender, const ExchangeItemMap& mapPaymentItem, CPoint ptItem, const CRect rtItem )
{
	if( mapPaymentItem.empty() )
		return false;

	UINT nDrawItemCount( mapPaymentItem.size() );
	UINT nCount( NULL );

	for( ExchangeItemCItr pos = mapPaymentItem.begin(); pos != mapPaymentItem.end(); ++pos )
	{
		const T_EXCHANGE_ITEM	stData( pos->second );
		const PT_ITEM_SPEC		pCondItemProp( g_xSpecManager->GetSpecItem( stData.dwItemID ) );

		FLERROR_ASSERT_LOG_DO( pCondItemProp, continue, _T("[범용 아이템 교환 창] 교환 필요 아이템 속성을 얻어 올 수 없습니다. COUNT - [%u], INDEX - [%u]"), nCount, stData.dwItemID );

		CTexture* pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pCondItemProp->szIcon ), WNDCOLOR_DEFAULT_KEY );
		if( pTexture )
		{
			CPoint ptIcon( ptItem );
			ptIcon.x += EXCHANGE_ITEM_ICON_DRAWOFFSET_X;	// 처음에 OFFSET 만큼 띄고 그린다

			ptIcon.x += ( ( nCount + m_byConditionItemMaxCount + EXCHANGE_ARROW_COUNT ) * EXCHANGE_ITEM_ICON_DRAWOFFSET_X ) +
						( ( nCount + m_byConditionItemMaxCount + EXCHANGE_ARROW_COUNT ) * pTexture->m_size.cx );
			
			ptIcon.y += static_cast<long>( ( rtItem.Height() - pTexture->m_size.cy) * 0.5f );

			// 보상 아이템이 여러개인 경우 그 중 하나만 교환해 줌 때문에 표현해주기
			
			if( nDrawItemCount > 1 )
			{
				BYTE byAlphaDrawIndex( NULL );

				if( m_byPaymentItemMaxCount > nDrawItemCount )
				{
					if( m_byPaymentItemColorChangeCount > nDrawItemCount )
						byAlphaDrawIndex = m_byPaymentItemColorChangeCount - nDrawItemCount;
					else
						byAlphaDrawIndex = m_byPaymentItemColorChangeCount;
				}
				else
					byAlphaDrawIndex = m_byPaymentItemColorChangeCount;
				
				if( byAlphaDrawIndex == nCount )
					p2DRender->RenderTexture( ptIcon, pTexture, 255 );
				else
					p2DRender->RenderTexture( ptIcon, pTexture, 64 );
			}
			else
				p2DRender->RenderTexture( ptIcon, pTexture );

			// 아이콘 그리는 영역 정보 저장
			CRect rtIcon( ptIcon.x, ptIcon.y, ptIcon.x + pTexture->m_size.cx, ptIcon.y + pTexture->m_size.cy ); 
			
			ICONDRAWINFO stIconDrawRect;
			stIconDrawRect.m_dwItemID		= stData.dwItemID;
			stIconDrawRect.m_rtItemIconDraw	= rtIcon; 
			stIconDrawRect.m_bind_flag		= stData.byItemFlag;

			m_vecIconDrawRect.push_back( stIconDrawRect );
			++nCount;

			if( NULL == stData.nItemQuantity )
				continue;

			// 아이템 개수 그리기
			PFONTCOLOR_WNDCOMMONITEMEXCHANGE pFontColorWndItemExchange = g_WndFontColorManager->GetWndCommonItemExchange();

			TCHAR szCount[ 32 ] = { NULL, };
			FLSPrintf( szCount, _countof( szCount ), _T("%d"), stData.nItemQuantity );

			CPoint ptItemCount( rtIcon.BottomRight() );

			CSize StringSize( m_p2DRender->m_pFont->GetTextExtent( szCount ) );
			m_p2DRender->TextOut(	ptItemCount.x - StringSize.cx - DRAWITEMCOUNT_OFFSET_X,
									ptItemCount.y - StringSize.cy , 
									szCount, 
									pFontColorWndItemExchange->m_stItemCount.GetFontColor(),
									pFontColorWndItemExchange->m_stItemCountShadow.GetFontColor() );
		}
	}
	
	return true;
}



//---------------------------------------------------------------------------------------------
// 아이템 그리기 (가로 줄)
// param	: 렌더러
// return	: 성공 / 실패
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawExchangeItem_WidthLine( C2DRender* p2DRender, const UINT& nCount, const CRect& rtItem )
{
	PFLCOLOR_WNDCOMMONITEMEXCHANGE pFLColorWndCommonItemExchange = g_FLColorManager->GetWndCommonItemExchange();
	DWORD dwLineColor = pFLColorWndCommonItemExchange->m_stListBoxLineColor.GetColor();

	CPoint ptItemLeftTop = m_pWndExchangeList->m_rectWindow.TopLeft(); 
	ptItemLeftTop.y += ( nCount * rtItem.Height() ) + WndListBox::TEXTOUT_OFFSET;

	p2DRender->RenderLine(	CPoint( ptItemLeftTop.x, ptItemLeftTop.y + rtItem.Height() ), 
							CPoint( ptItemLeftTop.x + rtItem.Width(), ptItemLeftTop.y + rtItem.Height() ),
							dwLineColor, dwLineColor );

	return true;
}


//---------------------------------------------------------------------------------------------
// 아이템 그리기 (선택박스)
// param	: 렌더러
// return	: 성공 / 실패
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawExchangeItem_SelectBox(C2DRender *p2DRender, const UINT &nCount, CPoint ptItem, const CRect &rtItem)
{
	if( m_mapExchangeItem.empty() )
		return false;

	int	nSelected( m_pWndExchangeList->GetCurSel() );

	if( nCount != nSelected )
		return false;

	PFLCOLOR_WNDCOMMONITEMEXCHANGE pFLColorWndCommonItemExchange = g_FLColorManager->GetWndCommonItemExchange();

	DWORD dwRectColor( pFLColorWndCommonItemExchange->m_stListBoxSelectRect.GetColor() );
	DWORD dwFillRectColor( pFLColorWndCommonItemExchange->m_stListBoxSelectFillRect.GetColor() );

	CRect rtRound( ptItem.x - WndListBox::TEXTOUT_OFFSET, ptItem.y, ptItem.x - WndListBox::TEXTOUT_OFFSET + rtItem.Width() - EXCHANGE_ITEM_VIEWPORT_OFFSET_RIGHT + 2, ptItem.y + rtItem.Height()+ 1 );	// 왜 1픽셀, 2픽셀이 모질르지 ㅡㅡ;;
	rtRound.DeflateRect( 2, 2 );

	p2DRender->RenderRect( rtRound, dwRectColor );

	rtRound.DeflateRect( 1, 1 );
	p2DRender->RenderFillRect( rtRound, dwFillRectColor );

	return true;
}


//---------------------------------------------------------------------------------------------
// 현재 마우스가 올려져 있는 아이템의 그리는 정보를 얻어온다.
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
ICONDRAWINFO CWndCommonItemExchange::GetCurrentOnMouseItemDrawInfo()
{
	return m_stCurrentOnMouseItemDrawInfo;
}


//---------------------------------------------------------------------------------------------
// 툴팁 그리기
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::DrawToolTip()
{
	if( m_vecIconDrawRect.empty() )
		return false;

	if( m_pWndOnMouseMove != m_pWndExchangeList )
		return false;

	CPoint	ptMouse( GetMousePoint() );
	CRect	rtListBox( m_pWndExchangeList->GetClientRect( TRUE ) );

	if( FALSE == rtListBox.PtInRect( ptMouse ) )
		return false;

	ZeroMemory( &m_stCurrentOnMouseItemDrawInfo, sizeof( ICONDRAWINFO ) );

	for( UINT nCount = 0; nCount < m_vecIconDrawRect.size(); nCount++ )
	{
		if( m_vecIconDrawRect[nCount].m_rtItemIconDraw.PtInRect( ptMouse ) ) 
		{
			m_stCurrentOnMouseItemDrawInfo = m_vecIconDrawRect[nCount];
			break;
		}
	}

	if( NULL == m_stCurrentOnMouseItemDrawInfo.m_dwItemID )
		return false;

	FLItemElem ItemElem;	
	ItemElem.m_dwItemId = m_stCurrentOnMouseItemDrawInfo.m_dwItemID;
	ItemElem.m_byFlag	= m_stCurrentOnMouseItemDrawInfo.m_bind_flag;

	CPoint	ptTemp = ptMouse;
	CRect	rtTemp = m_stCurrentOnMouseItemDrawInfo.m_rtItemIconDraw;

	ClientToScreen( &ptTemp );
	ClientToScreen( &rtTemp );

	g_WndMng.PutToolTip_Item( &ItemElem, ptTemp, &rtTemp, APP_DIALOG_EVENT );

	return true;
}


//---------------------------------------------------------------------------------------------
// 서버에게 패킷 전송
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchange::SetSendPacket()
{
	m_bSendPacket = true;
}


//---------------------------------------------------------------------------------------------
// 서버로 부터 패킷 전송 결과 받기
// param	: 메시지, 아이디, 결과
// return	: BOOL
// 설명		: 예전에는 결과값을 받아서 클라이언트에서 결과 메시지를 출력하였으나 지금은 
//			  서버에서 모든 메시지를 출력하고 있음.
//---------------------------------------------------------------------------------------------
void CWndCommonItemExchange::SetReceivedPacket()
{
	m_bSendPacket = false;
}


//---------------------------------------------------------------------------------------------
// 자식으로 부터 메시지 전달
// param	: 메시지, 아이디, 결과
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndCommonItemExchange::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( WIDC_OK == nID )		// 확인
	{
		if( m_bSendPacket )
			return FALSE;

		if( false == CheckPossibleMaxExchangeCount() )
		{
			SAFE_DELETE( m_pWndCommonItemExchangeConfirm );
			m_dwPossibleMaxExchangeCount = NULL;

			g_WndMng.OpenMessageBox( prj.GetText( TID_MMI_EXCHANGEAMOUNT_MESSAGE ) );
		}
	}

	if( WIDC_BUTTON1 == nID )	// 설명 다음 페이지 버튼
	{
		if( m_dwCurrentDescPageIndex < m_nTotalDescPageCount - 1 )
		{
			m_dwCurrentDescPageIndex += 1;
			
			SetDescriptionString();

			if( m_dwCurrentDescPageIndex == m_nTotalDescPageCount - 1 )
			{
				CWndButton* pNextPage( static_cast<CWndButton*>( GetDlgItem( WIDC_BUTTON1 ) ) );

				if( pNextPage )
				{
					pNextPage->SetVisible( FALSE );
					pNextPage->EnableWindow( FALSE );
				}
				else
				{
					FLERROR_LOG( PROGRAM_NAME, _T("[범용 아이템 교환 창] 다음 설명 페이지 버튼 컨트롤 포인터가 유효하지 않습니다.") );		
					FLASSERT( NULL );
				}
			}

		}
	}

	if( WIDC_CANCEL == nID )	// 취소
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}


//---------------------------------------------------------------------------------------------
// 교환 가능 한 최대 아이템 개수가 몇 개 인지 체크 해 보기
// param	: void
// return	: 성공 / 실패
// 설명		: 함수화 시켜야 되지만.. 지금 할 일이 태산이라.. 나중에 바꾸자
//---------------------------------------------------------------------------------------------
bool CWndCommonItemExchange::CheckPossibleMaxExchangeCount()
{
	m_dwPossibleMaxExchangeCount = NULL;

	if( m_mapExchangeItem.empty() )
		return false;

	if( NULL == m_pWndExchangeList )
		return false;

	int	nSelected( m_pWndExchangeList->GetCurSel() );
	if( -1 == nSelected )
		return false;

	if( (int)m_mapExchangeItem.size() <= nSelected )
		return false;

	const T_EXCHANGE_ITEM_LIST stDataList( m_mapExchangeItem[nSelected] );

	if( stDataList.mapConditionItem.empty() )
		return false;
	
	for( ExchangeItemCItr pos = stDataList.mapConditionItem.begin(); pos != stDataList.mapConditionItem.end(); ++pos )
	{
		const T_EXCHANGE_ITEM stData( pos->second );

		// 일반 제련 수치 교환 스크립트 일 때
		if( NULL != stData.dwMinGeneralEnchant &&
			NULL != stData.dwMaxGeneralEnchant )
		{
			int		nItemCount( NULL );
			WORD	wStartCount( MAX_HUMAN_PARTS );
			WORD	wEndCount( MAX_INVENINDEX_QUEST );

			for( WORD wCount = wStartCount; wCount < wEndCount; wCount++ )
			{
				FLItemElem* pItemElem( g_pPlayer->m_Inventory.GetAt( wCount ) );
				if( NULL == pItemElem )
					continue;

				if( stData.dwItemID == pItemElem->m_dwItemId )	
				{
					DWORD dwGeneralEnchantLevel( pItemElem->GetAbilityOption() );
				
					// 일반 제련 수치가 스크립트 수치 안에 존재한다면
					if( dwGeneralEnchantLevel >= stData.dwMinGeneralEnchant &&		
						dwGeneralEnchantLevel <= stData.dwMaxGeneralEnchant )
					{
						nItemCount += pItemElem->m_nItemNum;
					}
				}
			}

			if( stData.nItemQuantity > nItemCount )
				return false;
			else
			{
				// 소수점 오차를 피해기 위해서 처음부터 딱 나누어 지게 계산한다.
				int nRemainCount( nItemCount % stData.nItemQuantity );
				int nPossibleExchangeCount( ( nItemCount - nRemainCount ) / stData.nItemQuantity );	
		
				if( NULL == m_dwPossibleMaxExchangeCount )
					m_dwPossibleMaxExchangeCount = nPossibleExchangeCount;
				else
				{
					if( nPossibleExchangeCount < (int)m_dwPossibleMaxExchangeCount )
						m_dwPossibleMaxExchangeCount = nPossibleExchangeCount;
				}
			}
		}
		else	// 일반 교환 스크립트 일 때
		{
			int nItemCountFromInventory( g_pPlayer->m_Inventory.GetItemNumByItemId( stData.dwItemID ) );
			
			if( stData.nItemQuantity > nItemCountFromInventory )
				return false;
			else
			{
				// 소수점 오차를 피해기 위해서 처음부터 딱 나누어 지게 계산한다.
				int nRemainCount( nItemCountFromInventory % stData.nItemQuantity );
				int nPossibleExchangeCount( ( nItemCountFromInventory - nRemainCount ) / stData.nItemQuantity );	
		
				if( NULL == m_dwPossibleMaxExchangeCount )
					m_dwPossibleMaxExchangeCount = nPossibleExchangeCount;
				else
				{
					if( nPossibleExchangeCount < (int)m_dwPossibleMaxExchangeCount )
						m_dwPossibleMaxExchangeCount = nPossibleExchangeCount;
				}
			}
		}
	}

	if( 1 == m_dwPossibleMaxExchangeCount )
	{
		FLPacketItemExchangeReq	kPacket;

		kPacket.dwMenuID		= m_dwMMIDefineIndex;
		kPacket.dwListNth		= nSelected;
		kPacket.nExchangeCount	= m_dwPossibleMaxExchangeCount;

		g_DPlay.SendPacket( &kPacket );

		SetSendPacket();
	}
	else
	{
		SAFE_DELETE( m_pWndCommonItemExchangeConfirm );
		
		m_pWndCommonItemExchangeConfirm = new CWndCommonItemExchangeConfirm();
		if( m_pWndCommonItemExchangeConfirm )
		{
			m_pWndCommonItemExchangeConfirm->Initialize( this );
			m_pWndCommonItemExchangeConfirm->SetItemExchangeInfo( m_dwMMIDefineIndex, nSelected, m_dwPossibleMaxExchangeCount );
		}
	}
	
	return true;
}