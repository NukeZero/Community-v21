#include "StdAfx.h"
#include "../Neuz/StdAfx.h"
#include "FLWndTeleporterMng.h"
#include "FLWndTeleporter_Use.h"
#include "MapComboBoxData.h"
#include "party.h"
#include "playerdata.h"
#include "Continent.h"
#include "WndMapEx.h"
#include "defineMapComboBoxData.h"
#include <string.h> 


//-------------------------------------------------------------------------------------------------
//				텔레포트 유저 등록형 아이템 
//-------------------------------------------------------------------------------------------------
CFLWndTeleporter_Use::CFLWndTeleporter_Use(CTexture* pTexMap, DWORD dwItemObjID,std::vector< T_TELEPORTMAP_POINT > vecUserPoint):
m_nTelID(-1),
m_bIsChangeName(FALSE),
m_pWndChangeName(NULL),
m_pTeleportIconInfo(NULL)
{
	m_pTexTeleport_User	= pTexMap;
	m_dwItemObjID = dwItemObjID;
	m_vecUserPoints = vecUserPoint;
}

//-------------------------------------------------------------------------------------------------
CFLWndTeleporter_Use::~CFLWndTeleporter_Use()
{
	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect_User.begin(); Iterator != m_vecTeleportationPositionRect_User.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_vecTeleportationPositionRect_User.clear();
	SAFE_DELETE(m_pWndChangeName);
}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_Use::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::OnDraw			( C2DRender* p2DRender,BYTE BySelMapLocationID,CPoint pointMouse  )
{

}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::PaintFrame( C2DRender* p2DRender )
{
}
//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::OnInitialUpdate( )
{
	InitializeAddPointInformation( );
	m_menuUsePosition.CreateMenu( g_WndMng.m_pWndTeleporterMng ); 
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::InitializeAddPointInformation( )
{
	std::vector<T_TELEPORTMAP_POINT>::iterator itr_UserPoint;
	T_TELEPORTMAP_POINT tTelPoint;

	for( itr_UserPoint = m_vecUserPoints.begin(); itr_UserPoint != m_vecUserPoints.end(); ++itr_UserPoint )
	{
		tTelPoint = (*itr_UserPoint);
		TeleportationIconInfo* pTeleportationIconInfo = new TeleportationIconInfo;
		pTeleportationIconInfo->SetIndex( tTelPoint.dwIndex );	// 인덱스
		pTeleportationIconInfo->SetIconPoint( CPoint( static_cast< LONG >( tTelPoint.tPosition.tPos.x ), static_cast< LONG >( tTelPoint.tPosition.tPos.z ) ) );//위치
		pTeleportationIconInfo->SetMapName( tTelPoint.szName );//설명 
		pTeleportationIconInfo->SetWorldID( tTelPoint.tPosition.dwWorldID );//월드 ID
		m_vecTeleportationPositionRect_User.push_back( pTeleportationIconInfo );
	}	
}


//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_Use::OnDropIcon		( LPSHORTCUT pShortcut, CPoint point )
{
	return FALSE;

}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::OnRButtonDown  ( UINT nFlags, CPoint point )
{
	if(m_rectIconPT.PtInRect(point))
	{
		memset(m_rectIconPT,0,sizeof(m_rectIconPT));
		m_nChangeId = m_nTelID;
		m_menuUsePosition.DeleteAllMenu();
		m_menuUsePosition.AppendMenu( 0,MMI_TELEPORTPOINTMENU01,prj.GetText(_T(TID_MMI_TELEPORTPOINTMENU01)));
		m_menuUsePosition.AppendMenu( 0,MMI_TELEPORTPOINTMENU02,prj.GetText(_T(TID_MMI_TELEPORTPOINTMENU02)));
		m_menuUsePosition.AppendMenu( 0,MMI_TELEPORTPOINTMENU03,prj.GetText(_T(TID_MMI_TELEPORTPOINTMENU03)));
		m_menuUsePosition.AppendMenu( 0,MMI_TELEPORTPOINTMENU04,prj.GetText(_T(TID_MMI_TELEPORTPOINTMENU04)));

		m_menuUsePosition.Move(CPoint( g_WndMng.m_pWndTeleporterMng->m_rectCurrentClient.left, g_WndMng.m_pWndTeleporterMng->m_rectCurrentClient.top ) + point);
		m_menuUsePosition.SetVisible(TRUE);
		m_menuUsePosition.SetFocus();

	}	
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::OnLButtonDown  ( UINT nFlags, CPoint point )
{
	if(m_rectIconPT.PtInRect(point))
	{

		memset(m_rectIconPT,0,sizeof(m_rectIconPT));
		if(g_WndMng.m_pWndTeleporterMng->GetPointState() == 2)
		{
			SendPacket();
			PLAYSND(SND_PC_SKILLM_POWERINCREASE1_02 , &g_pPlayer->GetPos());
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::OnMouseWndSurface( CPoint point )
{
	if(m_rectIconPT.PtInRect(point))
	{
		if(m_pTeleportIconInfo)
		{
			CPoint point2 = point;
			CRect  DrawRect = m_rectIconPT;
			g_WndMng.m_pWndTeleporterMng->ClientToScreen( &point2 );
			g_WndMng.m_pWndTeleporterMng->ClientToScreen( &DrawRect );
			g_toolTip.PutToolTip((DWORD)this,m_pTeleportIconInfo->GetMapName(),DrawRect,point2);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::SendPacket()
{
	//텔레포트 이동 요청 
	m_kTeleporter.dwItemObjID		= m_dwItemObjID;
	m_kTeleporter.byType			= TELEPORT_MAP_TYPE_USER;
	m_kTeleporter.dwTeleportID		= m_nTelID;

	if(g_WndMng.m_pWndTeleporterMng)
	{
		g_WndMng.m_pWndTeleporterMng->SendPacket(&m_kTeleporter);
	}
}
//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_Use::Process		( void )
{
	return FALSE;

}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_Use::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( pWndBase == &m_menuUsePosition )
	{
		memset(m_rectIconPT,0,sizeof(m_rectIconPT));
		switch(nID)
		{
		case MMI_TELEPORTPOINTMENU01:
			{
				Req_DelPoint();
			}
			break;
		case MMI_TELEPORTPOINTMENU02:
			{
				Req_DelAllPoint();
			}
			break;
		case MMI_TELEPORTPOINTMENU03:
			{
				OpenChangeName();
			}
			break;
		case MMI_TELEPORTPOINTMENU04:
            {
				Destroy();
			}
			break;
		}
	}
	return FALSE;
}
//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::Req_DelPoint()
{

	memset(m_rectIconPT,0,sizeof(m_rectIconPT));
	m_kDelPoint.dwItemObjID = m_dwItemObjID;
	m_kDelPoint.dwUserPointID = m_nTelID;
	if( g_WndMng.m_pWndTeleporterMng != NULL )
	{
		g_WndMng.m_pWndTeleporterMng->SendPacket( &m_kDelPoint );
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::Req_DelAllPoint()
{

	memset(m_rectIconPT,0,sizeof(m_rectIconPT));
	m_kDelAllPoint.dwItemObjID = m_dwItemObjID;
	if( g_WndMng.m_pWndTeleporterMng != NULL )
	{
		g_WndMng.m_pWndTeleporterMng->SendPacket( &m_kDelAllPoint );
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::Req_ChangePointName(CString strChangeName)
{
	m_kChangePointName.dwItemObjID = m_dwItemObjID;
	m_kChangePointName.dwUserPointID = m_nChangeId;
	FLStrcpy(m_kChangePointName.szPointName, _countof( m_kChangePointName.szPointName ), strChangeName);
	if( g_WndMng.m_pWndTeleporterMng != NULL )
	{
		g_WndMng.m_pWndTeleporterMng->SendPacket( &m_kChangePointName );
	}
	
}
//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::SetDelUserPoint(T_TELEPORTMAP_POINT tUsePoint)
{
	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect_User.begin(); Iterator != m_vecTeleportationPositionRect_User.end(); Iterator++)
	{

		if(m_vecTeleportationPositionRect_User.empty()) 
		{
			return;
		}
		if((*Iterator)->GetIndex() == tUsePoint.dwIndex)
		{
			SAFE_DELETE(*(Iterator));
			m_vecTeleportationPositionRect_User.erase(Iterator);
			m_pTeleportIconInfo = NULL;
			return;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::SetDelAllUserPoint()
{
	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect_User.begin(); Iterator != m_vecTeleportationPositionRect_User.end(); ++Iterator )
	{
		if(m_vecTeleportationPositionRect_User.empty()) 
		{
			return;
		}
		//SAFE_DELETE(*Iterator);
	}
	m_vecTeleportationPositionRect_User.clear();
	m_pTeleportIconInfo = NULL;
}
//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::OpenChangeName()
{
	m_pWndChangeName = new CFLWndChangeName;
	m_pWndChangeName->Initialize(g_WndMng.m_pParentWnd,APP_CHANGENAME);
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::SetChangeUserPointName(T_TELEPORTMAP_POINT tUsePoint)
{
	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect_User.begin(); Iterator != m_vecTeleportationPositionRect_User.end(); Iterator++)
	{

		if(m_vecTeleportationPositionRect_User.empty()) 
		{
			return;
		}
		if((*Iterator)->GetIndex() == tUsePoint.dwIndex)
		{
			CString strMapName;
			strMapName.Format("%s",tUsePoint.szName);
			(*Iterator)->SetMapName(strMapName);
			m_vecTeleportationPositionRect_User.insert(Iterator,(*Iterator));
			return;
		}
	}	
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::SendPacket_AddPoint()
{
	//위치 등록 요청
	m_kAddMyPoint.dwItemObjID		= m_dwItemObjID;
	if( g_WndMng.m_pWndTeleporterMng != NULL )
	{
		g_WndMng.m_pWndTeleporterMng->SendPacket( &m_kAddMyPoint );
	}
}

//------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::RenderAddUserPoint( C2DRender* p2DRender, 
												BYTE BySelMapLocationID,
												CPoint pointMouse,
												CRect rectRevisedMapPosition,
												float fRevisedMapSizeRatio )
{
	//텔레포트 아이콘 그려줌 
	if( m_pTexTeleport_User == NULL )
	{
		return;
	}

	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect_User.begin(); Iterator != m_vecTeleportationPositionRect_User.end(); ++Iterator )
	{
		TeleportationIconInfo* pTeleportationIconInfo = ( TeleportationIconInfo* )( *Iterator );
        
		if( pTeleportationIconInfo == NULL && pTeleportationIconInfo->GetIndex() == -1)
		{
			m_pTeleportIconInfo = NULL;
			continue;
		}

		const D3DXVECTOR3 vTeleportationIconPosition( static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().x ), 
			0.0F, 
			static_cast< FLOAT >( pTeleportationIconInfo->GetIconPoint().y) );

		if( IsPositionInSelectedMapArea( vTeleportationIconPosition ) == FALSE )
		{
			continue;
		}

		D3DXVECTOR3 vConvertedPosition( 0.0F, 0.0F, 0.0F );
		ConvertPosition( vConvertedPosition, vTeleportationIconPosition, BySelMapLocationID,rectRevisedMapPosition );

		CRect rectTeleportationPositionIcon( 0, 0, 0, 0 );
		FLOAT fRevisedTeleportationPositionTextureSizeRatio = CalculateMapIconRectFromPoint( 
			rectTeleportationPositionIcon, 
			vConvertedPosition.x, 
			vConvertedPosition.z, 
			m_pTexTeleport_User, 
			TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO, 
			fRevisedMapSizeRatio);
		//텔레 포트 아이콘을 그릴때 마우스 포인트반응으로 크게 그린다.
		if( rectTeleportationPositionIcon.PtInRect( pointMouse ) == TRUE )
		{
			m_rectIconPT = rectTeleportationPositionIcon;
			CPoint pointExtendTeleportationPosition( 0, 0 );
			FLOAT fExtendTeleportationPositionTextureSizeRatio = CalculateMapIconStartPosition( 
				pointExtendTeleportationPosition, 
				vConvertedPosition.x, 
				vConvertedPosition.z, 
				m_pTexTeleport_User, 
				EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO,
				fRevisedMapSizeRatio);
			m_pTexTeleport_User->RenderScal( 
				p2DRender, 
				pointExtendTeleportationPosition, 
				255, 
				fExtendTeleportationPositionTextureSizeRatio, 
				fExtendTeleportationPositionTextureSizeRatio );
				m_nTelID = pTeleportationIconInfo->GetIndex();
				m_pTeleportIconInfo = pTeleportationIconInfo;
				g_WndMng.m_pWndTeleporterMng->SetPointState(2);
		}
		else
		{
			m_pTexTeleport_User->RenderScal( 
				p2DRender, 
				CPoint( rectTeleportationPositionIcon.left, rectTeleportationPositionIcon.top ), 
				255, 
				fRevisedTeleportationPositionTextureSizeRatio, 
				fRevisedTeleportationPositionTextureSizeRatio );
		}
		//p2DRender->RenderRect(rectTeleportationPositionIcon,0xff000000);
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_Use::SetAddUserPoint(T_TELEPORTMAP_POINT tUsePoint)
{
	//위치 등록 받은 값 
	m_tUserPoint = tUsePoint;
	TeleportationIconInfo* pTeleportationIconInfo = new TeleportationIconInfo;
	pTeleportationIconInfo->SetIndex( m_tUserPoint.dwIndex );	// 인덱스	
	pTeleportationIconInfo->SetIconPoint( CPoint( static_cast< LONG >( m_tUserPoint.tPosition.tPos.x ), static_cast< LONG >( m_tUserPoint.tPosition.tPos.z ) ) );//위치
	pTeleportationIconInfo->SetMapName( m_tUserPoint.szName );//설명 
	pTeleportationIconInfo->SetWorldID( m_tUserPoint.tPosition.dwWorldID );//월드 ID
	m_vecTeleportationPositionRect_User.push_back( pTeleportationIconInfo );
}

//-------------------------------------------------------------------------------------------------
T_TELEPORTMAP_POINT CFLWndTeleporter_Use::GetetAddUserPoint()
{
	return m_tUserPoint;
}

CFLWndChangeName::CFLWndChangeName()
{

}
CFLWndChangeName::~CFLWndChangeName()
{

}

void CFLWndChangeName::OnInitialUpdate( )
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
	m_pWndEditChangeName = (CWndEdit*)GetDlgItem( WIDC_EDIT_CHANGENAME );
	m_pWndEditChangeName->SetTextSelectBlock(0,m_pWndEditChangeName->GetOffset());
	m_pWndEditChangeName->AddWndStyle( EBS_AUTOHSCROLL );
	m_pWndEditChangeName->SetFocus();
}

BOOL CFLWndChangeName::Initialize( CWndBase* pWndParent, DWORD nType)
{
	InitDialog( g_Neuz.GetSafeHwnd(), nType,  WBS_KEY| WBS_MODAL , CPoint( 0, 0 ), pWndParent );
	return TRUE;
}

BOOL CFLWndChangeName::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WIDC_OK:
		{
			g_WndMng.m_pWndTeleporterMng->GetTelUse()->Req_ChangePointName(m_pWndEditChangeName->GetString());
			Destroy();
		}
		break;
	case WIDC_CANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

