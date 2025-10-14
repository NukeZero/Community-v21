#include "StdAfx.h"
#include "../Neuz/StdAfx.h"
#include "FLWndTeleporterMng.h"
#include "FLWndTeleporter_General.h"
#include "MapComboBoxData.h"
#include "party.h"
#include "playerdata.h"
#include "Continent.h"
#include "WndMapEx.h"
#include "defineMapComboBoxData.h"


//-------------------------------------------------------------------------------------------------
//				텔레포트 일반형 아이템 
//-------------------------------------------------------------------------------------------------
CFLWndTeleporter_General::CFLWndTeleporter_General(CTexture* pTexMap,DWORD dwItemObjID):
m_nTelID(-1),
m_pTeleportIconInfo(NULL)
{
	m_pTexTeleport_General = pTexMap;
	m_dwItemObjID = dwItemObjID;
}
//-------------------------------------------------------------------------------------------------
CFLWndTeleporter_General::~CFLWndTeleporter_General()
{
	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
	{
		SAFE_DELETE( *Iterator );
	}
	m_vecTeleportationPositionRect.clear();
}
//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_General::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	return TRUE;
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_General::OnInitialUpdate( )
{
	InitializeTeleportInformation( );
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporter_General::OnRButtonDown  ( UINT nFlags, CPoint point )
{

}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_General::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return FALSE;
}

//------------------------------------------------------------------------------------------------
void CFLWndTeleporter_General::InitializeTeleportInformation( )
{
	SpecTeleportMapPointMap mapTeleportMapPoint = g_xSpecManager->m_mapTeleportMapPoint;
	SpecTeleportMapPointItr itr_TelePoint;
	PT_TELEPORTMAP_POINT pTelPoint = NULL;

	for( itr_TelePoint = mapTeleportMapPoint.begin(); itr_TelePoint != mapTeleportMapPoint.end(); ++itr_TelePoint )
	{
		pTelPoint = &(itr_TelePoint->second);
		TeleportationIconInfo* pTeleportationIconInfo = new TeleportationIconInfo;
		pTeleportationIconInfo->SetIndex( pTelPoint->dwIndex );	// 인덱스
		pTeleportationIconInfo->SetIconPoint( CPoint( static_cast< LONG >( pTelPoint->tPosition.tPos.x ), static_cast< LONG >( pTelPoint->tPosition.tPos.z ) ) );//위치
		pTeleportationIconInfo->SetMapName( pTelPoint->szName );//설명 
		pTeleportationIconInfo->SetWorldID( pTelPoint->tPosition.dwWorldID );//월드 ID
		m_vecTeleportationPositionRect.push_back( pTeleportationIconInfo );
 	}	
}

//------------------------------------------------------------------------------------------------
void CFLWndTeleporter_General::RenderTeleportationPosition( C2DRender* p2DRender, 
														    BYTE BySelMapLocationID,
															CPoint pointMouse,
															CRect rectRevisedMapPosition,
															float fRevisedMapSizeRatio )
{
	//텔레포트 아이콘 그려줌 
	if( m_pTexTeleport_General == NULL )
	{
		return;
	}

	for( std::vector< TeleportationIconInfo* >::iterator Iterator = m_vecTeleportationPositionRect.begin(); Iterator != m_vecTeleportationPositionRect.end(); ++Iterator )
	{
		TeleportationIconInfo* pTeleportationIconInfo = ( TeleportationIconInfo* )( *Iterator );
		if( pTeleportationIconInfo == NULL )
		{
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
			m_pTexTeleport_General, 
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
				m_pTexTeleport_General, 
				EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO,
				fRevisedMapSizeRatio);
				m_pTexTeleport_General->RenderScal( 
				p2DRender, 
				pointExtendTeleportationPosition, 
				255, 
				fExtendTeleportationPositionTextureSizeRatio, 
				fExtendTeleportationPositionTextureSizeRatio );

				m_nTelID = pTeleportationIconInfo->GetIndex();
				m_pTeleportIconInfo = pTeleportationIconInfo;
				g_WndMng.m_pWndTeleporterMng->SetPointState(1);
		}
		else
		{
				m_pTexTeleport_General->RenderScal( 
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
void CFLWndTeleporter_General::OnMouseWndSurface( CPoint point )
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
void  CFLWndTeleporter_General::SendPacket()
{
	//텔레포트 이동 요청 
	m_kTeleporter.dwItemObjID		= m_dwItemObjID;
	m_kTeleporter.byType			= TELEPORT_MAP_TYPE_SPEC;
	m_kTeleporter.dwTeleportID		= m_nTelID;

	if(g_WndMng.m_pWndTeleporterMng)
	{
		g_WndMng.m_pWndTeleporterMng->SendPacket(&m_kTeleporter);
	}
}

//-----------------------------------------------------------------------------
void CFLWndTeleporter_General::OnLButtonDown  ( UINT nFlags, CPoint point )
{
	if(m_rectIconPT.PtInRect(point))
	{
		if(g_WndMng.m_pWndTeleporterMng->GetPointState() == 1)
		{
			SendPacket();
			PLAYSND(SND_PC_SKILLM_POWERINCREASE1_02 , &g_pPlayer->GetPos());
		}	
	}
}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporter_General::Process( void )
{
	return FALSE;
}
