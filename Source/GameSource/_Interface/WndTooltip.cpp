
#include "StdAfx.h"

#ifdef __HYPERLINK_ITEM16

#include "WndTooltip.h"

CWndTooltip::CWndTooltip( ) : 
m_pIconTexture( NULL ),
m_pUltimateTexture( NULL ),
m_pJewelBgTexture( NULL )
{
#ifdef __ENCHANT_BARUNA16
	m_pBarunaTexture = NULL;
#endif

	ResetData( );
}

CWndTooltip::~CWndTooltip( )
{

}

void CWndTooltip::OnDraw( C2DRender* p2DRender ) 
{ 
	//icon
	CPoint point = m_rectIcon.TopLeft();
	m_pIconTexture->Render( p2DRender, point );
	
	p2DRender->TextOut_EditString( m_rectRender.TopLeft().x + 3, m_rectRender.TopLeft().y + 45, m_strToolTip, 0, 0, 2 );
	
	if( 1 == m_nAdded )
	{
		if(m_pUltimateTexture != NULL)
			m_pUltimateTexture->Render( p2DRender, CPoint( 2, 43 ) );

		//Added Jewel Icon 
		CRect PlusRect = m_rectRender;
		CPoint point;
		point.x = PlusRect.left + ((PlusRect.Width() - 194) / 2);
		point.y = PlusRect.bottom - 70;


		CTexture* pTexture = NULL;
		for(int i=0; i<m_nSlot; i++)
		{
			if(m_pJewelBgTexture != NULL)
				m_pJewelBgTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );

			//Jewel Render
			if(m_nAddedJewel[i] != 0)
			{
				PT_ITEM_SPEC pItemProp;
				pItemProp = g_xSpecManager->GetSpecItem( m_nAddedJewel[i] );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );
				}
			}

			point.x += 38;
		}
	}
#ifdef __ENCHANT_BARUNA16
	else if( 2 == m_nAdded )
	{
		if( m_pBarunaTexture )
		{
			m_pBarunaTexture->Render( p2DRender, CPoint( 2, 43 ) );
		}
	}
#endif //__ENCHANT_BARUNA16
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	else if( 4 == m_nAdded )
	{
		//Added Jewel Icon 
		CRect PlusRect = m_rectRender;
		CPoint point;
		point.x = PlusRect.left + ((PlusRect.Width() - 194) / 2);
		point.y = PlusRect.bottom - 70;


		CTexture* pTexture = NULL;
		for(int i=0; i<m_nSlot; i++)
		{
			if(m_pJewelBgTexture != NULL)
				m_pJewelBgTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );

			//Jewel Render
			if(m_nAddedJewel[i] != 0)
			{
				PT_ITEM_SPEC pItemProp;
				pItemProp = g_xSpecManager->GetSpecItem( m_nAddedJewel[i] );
				if(pItemProp != NULL)
				{
					pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
					if(pTexture != NULL)
						pTexture->RenderScal( p2DRender, point, 255, 1.0f, 1.0f );
				}
			}

			point.x += 38;
		}
	}
#endif
} 

void CWndTooltip::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	if( pWndChat )
	{
		//최초 위치
		CRect rect = pWndChat->GetWndRect( );
		Move( rect.right + 10, rect.top );
	}

	InitTexture( );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndTooltip::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_TOOLTIP_LINKEDITEM, WBS_KEY, CPoint( 0, 0 ), pWndParent );
} 

BOOL CWndTooltip::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WTBID_CLOSE:	//툴팁은 종료메세지를 후킹해서 실제로 Destroy하지않고 감춘다.
		SetVisible( FALSE );
		nID = 0;
		break;
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndTooltip::Process()
{
	return TRUE;
}

CWndTooltip* CWndTooltip::_GetThis( )
{
	static CWndTooltip stWnd;
	static BOOL bInit = FALSE;
	if( bInit == FALSE )
	{
		stWnd.Initialize();
		bInit = TRUE;
	//	stWnd.SetVisible( FALSE );	//최초는 감춰둔다.
	}
	return &stWnd;
}

BOOL CWndTooltip::ShowTooltip( FLItemBase* pItemBase )
{
	SetVisible( TRUE );	//이렇게 해줘야 사이즈 변경이 제대로 먹는다.--;

	if( FALSE == UpdateData( pItemBase ) )
	{
		SetVisible( FALSE );
		return FALSE;
	}
	
	return TRUE;
}

BOOL CWndTooltip::UpdateData( FLItemBase* pItemBase )
{
	PT_ITEM_SPEC pItemProp = pItemBase->GetProp();
	if( !pItemProp )		
		return FALSE;

	//data init.
	ResetData( );

	m_pIconTexture = pItemBase->GetTexture( );

	CEditString strItem = _T( "" );
	g_WndMng.MakeToolTipText( pItemBase, strItem, APP_INVENTORY );

	m_strToolTip = strItem;
	m_strToolTip.Init( CWndBase::m_Theme.m_pFontText, &CRect( 0, 0, 200, 0 ) );
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString( m_strToolTip );
	
	m_rectRender = CRect( 0, 0, size.cx + 20, size.cy + 80 + ( 2 * ( size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight() ) ) );

	//적은 설명으로 인해 윈도가 너무 작아지는걸 방지
	if( m_rectRender.Width() < 160 )
		m_rectRender.right = m_rectRender.left + 160;
	if( m_rectRender.Height() < 100 )
		m_rectRender.bottom = m_rectRender.top + 100;
	this->SetWndSize( m_rectRender.Width(), m_rectRender.Height() );

	//채팅윈도 Bottom을 기준으로 본윈도 높이만큼 y조정
	CWndChat* pWndChat = (CWndChat*) g_WndMng.GetApplet( APP_COMMUNICATION_CHAT );
	if( pWndChat )
	{
		CRect rect = pWndChat->GetWndRect( );
		this->Move( rect.right + 10, rect.bottom - m_rectRender.Height() );
	}

	m_rectIcon = CRect( m_rectRender.Width() / 2 - 22, 5, m_rectRender.Width() / 2 + 10, 37 );

	//Check extra
	CheckExtraInfo( pItemBase );

	return TRUE;
}

BOOL CWndTooltip::InitTexture( )
{
	CString strPath;
	
	if(g_xFlyffConfig->GetMainLanguage() == LANG_FRE)
		strPath = MakePath( DIR_THEME , g_xFlyffConfig->GetMainLanguage(), "Icon_Ultimate.dds");
	else
		strPath = MakePath( DIR_ICON, "Icon_Ultimate.dds");

	m_pUltimateTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strPath, 0xffff00ff );
	m_pJewelBgTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(),"WndChgElemItem.bmp"), 0xffff00ff );

#ifdef __ENCHANT_BARUNA16
	m_pBarunaTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "Icon_Baruna01.dds" ), 0xffff00ff );

	if( m_pUltimateTexture && m_pJewelBgTexture && m_pBarunaTexture )
		return TRUE;
#endif //__ENCHANT_BARUNA16

	if( m_pUltimateTexture && m_pJewelBgTexture )
		return TRUE;

	return FALSE;
}

void CWndTooltip::CheckExtraInfo( FLItemBase* pItemBase )
{
	PT_ITEM_SPEC pProp = pItemBase->GetProp( );
	if( !pProp )
		return;

	if( pProp->IsUltimate() 
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		|| pProp->IsCostumeEnhanceParts()
#endif
		)
	{

		m_nAdded = 1;

		//무기 피어싱
		m_nSlot = ((FLItemElem*)pItemBase)->GetUltimatePiercingSize();
		for(int i=0; i<m_nSlot; i++)
		{
			m_nAddedJewel[i] = ((FLItemElem*)pItemBase)->GetUltimatePiercingItemID(i);
			int a = m_nAddedJewel[ i ];
			a = a;
		}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		if( pProp->IsCostumeEnhanceParts() )
		{
			m_nAdded = 4;

			int nJewel = ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + ((FLItemElem*)pItemBase)->GetProp()->dwParts - PARTS_HAT;
			for(int i=0; i<m_nSlot; i++)
			{
				if( m_nAddedJewel[i] >= DST_STR && m_nAddedJewel[i] < MAX_ADJPARAMARY )
					m_nAddedJewel[i] = nJewel;
			}
		}
#endif
	}

#ifdef __ENCHANT_BARUNA16
	if( pProp->IsBaruna( ) )
	{
		m_nAdded = 2;
	}
#endif //__ENCHANT_BARUNA16

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	if( pProp->IsCostumeEnhanceParts() )
	{
		m_nAdded = 4;

		m_nSlot = ((FLItemElem*)pItemBase)->GetUltimatePiercingSize();
		for(int i=0; i<m_nSlot; i++)
		{
			m_nAddedJewel[i] = ((FLItemElem*)pItemBase)->GetUltimatePiercingItemID(i);
		}

		int nJewel = ITEM_INDEX( 25338, II_GEN_MAT_GARNET ) + ((FLItemElem*)pItemBase)->GetProp()->dwParts - PARTS_HAT;
		for(int i=0; i<m_nSlot; i++)
		{
			if( m_nAddedJewel[i] >= DST_STR && m_nAddedJewel[i] < MAX_ADJPARAMARY )
				m_nAddedJewel[i] = nJewel;
		}
	}
#endif

	if( m_nSlot > 0  )
	{
		m_rectRender.bottom += 40; // 보석슬롯만큼 늘려준다.
		this->SetWndSize( m_rectRender.Width(), m_rectRender.Height() );
	}
}

void CWndTooltip::ResetData( )
{
	m_nSlot = 0;
	m_nAdded = 0;
	memset( m_nAddedJewel, 0, sizeof( m_nAddedJewel ) );
}

#endif //__HYPERLINK_ITEM16