#include "StdAfx.h"
#include "../Neuz/StdAfx.h"
#include "FLWndTeleporter_General.h"
#include "FLWndTeleporter_Use.h"
#include "FLWndTeleporterMng.h"
#include "MapComboBoxData.h"
#include "party.h"
#include "playerdata.h"
#include "Continent.h"
#include "WndMapEx.h"
#include "defineMapComboBoxData.h"

const FLOAT CFLWndTeleporterMng::SOURCE_MAP_SIZE_X( 1280.0F );
const FLOAT CFLWndTeleporterMng::SOURCE_MAP_SIZE_Y( 960.0F );
const FLOAT CFLWndTeleporterMng::TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO( 1.4F );
const FLOAT CFLWndTeleporterMng::EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO( TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO + EXTEND_RATIO );
const FLOAT CFLWndTeleporterMng::EXTEND_RATIO( 0.4F );

CFLWndTeleporterMng::CFLWndTeleporterMng():
m_rectRevisedMapPosition( 0, 0, 0, 0 ),
m_byTransparentStateAlpha( NORMAL_STATE_ALPHA ),
m_bySelectedMapLocationID( CONT_NODATA ), 
m_fRevisedMapSizeRatio( 1.0F ),
m_pTelGeneral(NULL),
m_pTelUse(NULL),
m_pTexTeleport_General(NULL),
m_pTexTeleport_Use(NULL),
m_pMapTexture(NULL),
m_nPointState(-1)
{
	
}
CFLWndTeleporterMng::~CFLWndTeleporterMng()
{

}
//-------------------------------------------------------------------------------------------------
FLOAT CFLWndTeleporterMng::CalculateMapIconStartPosition( CPoint& pointDestinationPosition, 
															  FLOAT fIconPositionX, 
															  FLOAT fIconPositionY, 
															  const CTexture* const pIconTexture, 
															  FLOAT fIconSizeRatio ,
															  float fRevisedMapSizeRatio)
{
	FLOAT fRevisedIconSizeRatio = fIconSizeRatio * fRevisedMapSizeRatio;
	FLOAT fRevisedIconSizeX = static_cast< FLOAT >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconSizeY = static_cast< FLOAT >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio;
	pointDestinationPosition.SetPoint( 
		static_cast< int >( fIconPositionX - ( fRevisedIconSizeX * 0.5f ) ), 
		static_cast< int >( fIconPositionY - ( fRevisedIconSizeY * 0.5f ) ) );
	return fRevisedIconSizeRatio;
}
//-------------------------------------------------------------------------------------------------

FLOAT CFLWndTeleporterMng::CalculateMapIconRectFromPoint( CRect& rectDestinationIcon, 
															  FLOAT fIconPositionX, 
															  FLOAT fIconPositionY, 
															  const CTexture* const pIconTexture, 
															  FLOAT fIconSizeRatio,
															  float fRevisedMapSizeRatio)
{
	if( pIconTexture == NULL )
	{
		return 0.0F;
	}

	FLOAT fRevisedIconSizeRatio = fIconSizeRatio * fRevisedMapSizeRatio;
	FLOAT fRevisedIconSizeX = static_cast< FLOAT >( pIconTexture->m_size.cx ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconSizeY = static_cast< FLOAT >( pIconTexture->m_size.cy ) * fRevisedIconSizeRatio;
	FLOAT fRevisedIconStartPositionX = fIconPositionX - ( fRevisedIconSizeX * 0.5f );
	FLOAT fRevisedIconStartPositionY = fIconPositionY - ( fRevisedIconSizeY * 0.5f );
	rectDestinationIcon.SetRect( 
		static_cast< int >( fRevisedIconStartPositionX ), 
		static_cast< int >( fRevisedIconStartPositionY ), 
		static_cast< int >( fRevisedIconStartPositionX + fRevisedIconSizeX ), 
		static_cast< int >( fRevisedIconStartPositionY + fRevisedIconSizeY ) );
	return fRevisedIconSizeRatio;
}
//-------------------------------------------------------------------------------------------------
const D3DXVECTOR3& CFLWndTeleporterMng::ConvertPosition( D3DXVECTOR3& vDestination, 
															 const D3DXVECTOR3& vSource, 
															 BYTE byLocationID,CRect rectRevisedMapPosition)
{
	CRect rectLocationRealPosition = prj.m_MapInformationManager.FindRealPosition( byLocationID );
	FLOAT fLocationRealPositionX1 = static_cast< FLOAT >( rectLocationRealPosition.left );
	FLOAT fLocationRealPositionY1 = static_cast< FLOAT >( rectLocationRealPosition.top );
	FLOAT fLocationRealPositionX2 = static_cast< FLOAT >( rectLocationRealPosition.right );
	FLOAT fLocationRealPositionY2 = static_cast< FLOAT >( rectLocationRealPosition.bottom );
	FLOAT fRealMapWidth = fLocationRealPositionX2 - fLocationRealPositionX1;
	FLOAT fRealMapHeight = fLocationRealPositionY2 - fLocationRealPositionY1;
	FLOAT fRealPositionRatioX = ( vSource.x - fLocationRealPositionX1 ) / fRealMapWidth;
	FLOAT fRealPositionRatioY = ( fLocationRealPositionY2 - vSource.z ) / fRealMapHeight;
	vDestination.x = static_cast< FLOAT >( rectRevisedMapPosition.left ) + ( static_cast< FLOAT >( rectRevisedMapPosition.Width() ) * fRealPositionRatioX )-7.5f;
	vDestination.y = vSource.y;
	vDestination.z = static_cast< FLOAT >( rectRevisedMapPosition.top ) + ( static_cast< FLOAT >( rectRevisedMapPosition.Height() ) * fRealPositionRatioY )-12.0f;
	return vDestination;
}
//-------------------------------------------------------------------------------------------------
const D3DXVECTOR3& CFLWndTeleporterMng::ReconvertPosition( D3DXVECTOR3& vDestination, 
															   const D3DXVECTOR3& vSource, 
															   BYTE byLocationID ,
															   CRect rectRevisedMapPosition)
{
	CRect rectLocationRealPosition = prj.m_MapInformationManager.FindRealPosition( byLocationID );
	FLOAT fLocationRealPositionX1 = static_cast< FLOAT >( rectLocationRealPosition.left );
	FLOAT fLocationRealPositionY1 = static_cast< FLOAT >( rectLocationRealPosition.top );
	FLOAT fLocationRealPositionX2 = static_cast< FLOAT >( rectLocationRealPosition.right );
	FLOAT fLocationRealPositionY2 = static_cast< FLOAT >( rectLocationRealPosition.bottom );
	FLOAT fRealMapWidth = fLocationRealPositionX2 - fLocationRealPositionX1;
	FLOAT fRealMapHeight = fLocationRealPositionY2 - fLocationRealPositionY1;
	vDestination.x = fLocationRealPositionX1 + ( ( fRealMapWidth * ( vSource.x - static_cast< FLOAT >( rectRevisedMapPosition.left ) ) ) / static_cast< FLOAT >( rectRevisedMapPosition.Width() ) );
	vDestination.y = vSource.y;
	vDestination.z = fLocationRealPositionY2 - ( ( fRealMapHeight * ( vSource.z - static_cast< FLOAT >( rectRevisedMapPosition.top ) ) ) / static_cast< FLOAT >( rectRevisedMapPosition.Height() ) );
	return vDestination;
}
//-------------------------------------------------------------------------------------------------
const CRect& CFLWndTeleporterMng::ReviseScriptRectInformation( CRect& rectDestination, 
																   const CRect& rectSource,
																   float fRevisedMapSizeRatio,
																   CRect rectRevisedMapPosition)
{
	// 스크립트의 원본 위치 데이터는 모두 1280 * 960 해상도를 기준으로 작성되어 있음
	// 새로운 맵 시스템에서는 맵 크기를 마음대로 변경할 수 있으므로, 그에 맞게끔 보정 계산을 해서 보정된 위치 데이터를 사용해야 함
	FLOAT fRevisedPositionXRatio = static_cast< FLOAT >( rectSource.left ) / SOURCE_MAP_SIZE_X * static_cast< FLOAT >( rectRevisedMapPosition.Width() );
	FLOAT fRevisedPositionYRatio = static_cast< FLOAT >( rectSource.top ) / SOURCE_MAP_SIZE_Y * static_cast< FLOAT >( rectRevisedMapPosition.Height() );
	FLOAT fRevisedWidthRatio = static_cast< FLOAT >( rectSource.Width() ) * fRevisedMapSizeRatio;
	FLOAT fRevisedHeightRatio = static_cast< FLOAT >( rectSource.Height() ) * fRevisedMapSizeRatio;
	rectDestination.left = static_cast< int >( static_cast< FLOAT >( rectRevisedMapPosition.left ) + fRevisedPositionXRatio );
	rectDestination.top = static_cast< int >( static_cast< FLOAT >( rectRevisedMapPosition.top ) + fRevisedPositionYRatio );
	rectDestination.right = rectDestination.left + static_cast< int >( fRevisedWidthRatio );
	rectDestination.bottom = rectDestination.top + static_cast< int >( fRevisedHeightRatio );
	return rectDestination;
}
//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporterMng::IsPositionInSelectedMapArea( const D3DXVECTOR3& vPosition )
{
	BYTE byLocationID = GetMapArea( vPosition );
	if( m_bySelectedMapLocationID != CONT_NODATA && byLocationID != m_bySelectedMapLocationID )
	{
		return FALSE;
	}
	return TRUE;
}
void CFLWndTeleporterMng::OnMouseWndSurface( CPoint point )
{
	if(m_pTelGeneral)
	{
		m_pTelGeneral->OnMouseWndSurface(point);
	}

	if(m_pTelUse)
	{
		m_pTelUse->OnMouseWndSurface(point);
	}
}

//-------------------------------------------------------------------------------------------------
BYTE CFLWndTeleporterMng::GetMapArea( const D3DXVECTOR3& vPosition )
{
	//gmpbigsun(20101111) : 던젼 추가
	if( !g_pPlayer )
		return 0;

	CWorld* pWorld = g_pPlayer->GetWorld();
	if( pWorld )
	{
		switch( pWorld->GetID() )
		{
		case WI_INSTANCE_OMINOUS :			return DUNGEON_OMINOUS01;
		case WI_INSTANCE_OMINOUS_1 :		return DUNGEON_OMINOUS02;
		case WI_INSTANCE_DREADFULCAVE :		return DUNGEON_DREADFULCAVE;
		case WI_INSTANCE_BEHAMAH :			return DUNGEON_BEHEMOTH;
		case WI_WORLD_KEBARAS:				return CASHAREA_ASRIA;
		case WI_WORLD_CISLAND:				return CASHAREA_CORALICELAND;
		case WI_WORLD_RARTESIA:				return CASHAREA_RARTESIA;
		default: break;
		}
	}

	BYTE byLocationID = CContinent::GetInstance()->GetTown( vPosition );
	if( byLocationID == CONT_NODATA )
	{
		byLocationID = CContinent::GetInstance()->GetContinent( vPosition );
	}
	return byLocationID;
}
//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporterMng::Initialize		( CWndBase* pWndParent, DWORD nType )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), nType, 0 , CPoint( 0, 0 ), pWndParent );
}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporterMng::OnChildNotify	( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
	case WIDC_TEL_REG:
		if(m_pTelUse)
		{
			m_pTelUse->SendPacket_AddPoint();
		}
		break;
	case WIDC_TEL_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::OnDraw			( C2DRender* p2DRender )
{
    RenderMap( p2DRender );
	CPoint point = this->GetMousePoint();
	if(m_pTelGeneral)
	{
		m_pTelGeneral->RenderTeleportationPosition( p2DRender,
													m_bySelectedMapLocationID,
													point,
													m_rectRevisedMapPosition,
													m_fRevisedMapSizeRatio );
	}
	if(m_pTelUse)
	{
		m_pTelUse->RenderAddUserPoint( p2DRender, 
									   m_bySelectedMapLocationID,
									   point,
									   m_rectRevisedMapPosition,
									   m_fRevisedMapSizeRatio );
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::PaintFrame( C2DRender* p2DRender )
{
	if( prj.m_bMapTransparent == TRUE )
	{
		return;
	}

	if( m_pTexture == NULL )
	{
		return;
	}

	RenderWnd();
	if( IsWndStyle( WBS_CAPTION ) )
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle );
		p2DRender->TextOut( 10, 4, m_strTitle, ( ( m_dwColor & 0x00ffffff ) | ( m_nAlphaCount << 24 ) ) );
		p2DRender->SetFont( pOldFont );
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::SendPacket(const FLPacket* pkPacket)
{
	g_DPlay.SendPacket(pkPacket);
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::ReceivedPacket( FLSnapshotTeleportMapOpenNoti* pkOpenTeleport)
{
	g_WndMng.CloseFuncApp();	

	if(pkOpenTeleport->dwItemObjID != NULL_ID)
	{
		m_dwItemObjID = pkOpenTeleport->dwItemObjID;
		m_byItemType  =	pkOpenTeleport->byType;
		if(m_byItemType == TELEPORT_MAP_TYPE_USER)
		{
			m_vecUserPoints = pkOpenTeleport->vecUserPoints;
		}
		Initialize( &g_WndMng, APP_MAP_TELEPORT );
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::RecComplatePacket( FLSnapshotTeleportMapToPointAck* pkComplateTeleport)
{
	Destroy();
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::RecAddPointPacket( FLSnapshotTeleportMapAddMyPointAck* pkAddPointTeleport)
{
	if(m_pTelUse)
	{
		m_pTelUse->SetAddUserPoint(pkAddPointTeleport->tUserPoint);
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::RecDelPointPacket( FLSnapshotTeleportMapDelPointAck* pkDelPointTeleport)
{
	if(m_pTelUse)
	{
		m_pTelUse->SetDelUserPoint(pkDelPointTeleport->tUserPoint);
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::RecDelAllPointPacket( FLSnapShotTeleportMapDelAllPointAck* pkDelAllPointTeleport)
{
	if(m_pTelUse)
	{
		m_pTelUse->SetDelAllUserPoint();
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::RecChangePointNamePacket( FLSnapshotTeleportMapChgPointNameAck* pkChangePointNameTeleport)
{
	if(m_pTelUse)
	{
		m_pTelUse->SetChangeUserPointName(pkChangePointNameTeleport->tUserPoint);
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::OnInitialUpdate( )
{
	CWndNeuz::OnInitialUpdate(); 

	m_pTexTeleport_General = prj.m_MapInformationManager.GetTeleportTexture_General();
	m_pTexTeleport_Use = prj.m_MapInformationManager.GetTeleportTexture_User();
	ResetMapInfo();
	CWndButton* pBT_PositionInvert = ( CWndButton* )GetDlgItem( WIDC_TEL_REG );
	//아이템의 종류에 따라 버튼 활성/비활성

	SAFE_DELETE(m_pTelGeneral);
	m_pTelGeneral= new CFLWndTeleporter_General(m_pTexTeleport_General,m_dwItemObjID);
	m_pTelGeneral->OnInitialUpdate();

	switch(m_byItemType)
	{
	case TELEPORT_MAP_TYPE_SPEC:
		{
			if( pBT_PositionInvert )
				pBT_PositionInvert->EnableWindow( FALSE );	
		}
		break;

	case TELEPORT_MAP_TYPE_USER:
		{
			if( pBT_PositionInvert )
				pBT_PositionInvert->EnableWindow( TRUE );
			SAFE_DELETE(m_pTelUse);
			m_pTelUse = new CFLWndTeleporter_Use(m_pTexTeleport_Use, m_dwItemObjID,m_vecUserPoints);
			m_pTelUse->OnInitialUpdate();
		}
		break;
	}

	CWndBase*	pWndBase = g_WndMng.GetWndBase( APP_INVENTORY );
	if(pWndBase)
		pWndBase->Destroy();
}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporterMng::OnDropIcon		( LPSHORTCUT pShortcut, CPoint point )
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::OnRButtonDown  ( UINT nFlags, CPoint point )
{
	if(m_pTelGeneral)
	{
		m_pTelGeneral->OnRButtonDown( nFlags, point );
	}
	if( m_pTelUse )
	{
		m_pTelUse->OnRButtonDown( nFlags, point );
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( m_pTelUse )
	{
		m_pTelUse->OnLButtonDown( nFlags, point );
	}
	if(m_pTelGeneral)
	{
		m_pTelGeneral->OnLButtonDown( nFlags, point );
	}
}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporterMng::Process		( void )
{
	ProcessMapSizeInformation();
	if(m_pTelGeneral)
	{
		m_pTelGeneral->Process();
	}
	return CWndBase::Process();
}

//-------------------------------------------------------------------------------------------------
BOOL CFLWndTeleporterMng::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if(m_pTelGeneral)
	{
		m_pTelGeneral->OnCommand(nID,dwMessage,pWndBase);
	}
	if(m_pTelUse)
	{
		m_pTelUse->OnCommand(nID,dwMessage,pWndBase);
	}
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::RenderMap( C2DRender* p2DRender )
{
	if( prj.m_bMapTransparent == FALSE )
	{
		p2DRender->RenderFillRect( GetClientRect(), D3DCOLOR_ARGB( m_byTransparentStateAlpha, 0, 0, 0 ) );
	}

	if( m_pMapTexture == NULL )
	{
		return;
	}

	p2DRender->RenderTexture( 
		CPoint( m_rectRevisedMapPosition.left, m_rectRevisedMapPosition.top ), 
		m_pMapTexture, 
		m_byTransparentStateAlpha, 
		m_fRevisedMapSizeRatio, 
		m_fRevisedMapSizeRatio );
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::ResetMapInfo()
{
	MapComboBoxDataVector* pMapComboBoxDataVector = prj.m_MapInformationManager.GetMapNameVector();
	for( MapComboBoxDataVector::iterator Iterator = pMapComboBoxDataVector->begin(); Iterator != pMapComboBoxDataVector->end(); ++Iterator )
	{
		CMapComboBoxData* pMapComboBoxData = ( CMapComboBoxData* )( *Iterator );
		if( pMapComboBoxData == NULL )
		{
			continue;
		}

		DWORD dwID = pMapComboBoxData->GetID();
		//현재 선택된 맵아이디에 해당하는 CMapComboBoxData타입의 부모( 던젼, 유료.. )타입을 설정하고  , 열것인지 말것인지 결정
		if( MCD_MADRIGAL == dwID )
		{
			m_pMapTexture = pMapComboBoxData->GetMapTexture();
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CFLWndTeleporterMng::ProcessMapSizeInformation( void )
{
	if( m_pMapTexture == NULL )
	{
		return;
	}

	CSize sizeClient = GetClientRect().Size();
	CSize sizeTexture = m_pMapTexture->m_size;
	float fWindowXYRatio = static_cast< float >( sizeClient.cx ) / static_cast< float >( sizeClient.cy );
	float fTextureXYRatio = static_cast< float >( sizeTexture.cx ) / static_cast< float >( sizeTexture.cy );
	float fTextureYXRatio = static_cast< float >( sizeTexture.cy ) / static_cast< float >( sizeTexture.cx );
	FLOAT fRevisedMapSizeRatio = 1.0F;

	CSize sizeRevisedTexture( sizeClient );
	int nStartPositionX = 0;
	int nStartPositionY = 0;
	if( fWindowXYRatio > fTextureXYRatio )
	{
		sizeRevisedTexture.cx = static_cast< int >( static_cast< float >( sizeClient.cy ) * fTextureXYRatio );
		sizeRevisedTexture.cy = sizeClient.cy;
		nStartPositionX = ( sizeClient.cx - sizeRevisedTexture.cx ) / 2;
		m_fRevisedMapSizeRatio = fRevisedMapSizeRatio * ( ( static_cast< FLOAT >( sizeRevisedTexture.cy ) / static_cast< FLOAT >( sizeTexture.cy ) ) );
	}
	else if( fWindowXYRatio < fTextureXYRatio )
	{
		sizeRevisedTexture.cx = sizeClient.cx;
		sizeRevisedTexture.cy = static_cast< int >( static_cast< float >( sizeClient.cx ) * fTextureYXRatio );
		nStartPositionY = ( sizeClient.cy - sizeRevisedTexture.cy ) / 2;
		m_fRevisedMapSizeRatio = fRevisedMapSizeRatio * ( ( static_cast< FLOAT >( sizeRevisedTexture.cx ) / static_cast< FLOAT >( sizeTexture.cx ) ) );
	}

	m_rectRevisedMapPosition.SetRect( nStartPositionX, nStartPositionY, nStartPositionX + sizeRevisedTexture.cx, nStartPositionY + sizeRevisedTexture.cy );

}
