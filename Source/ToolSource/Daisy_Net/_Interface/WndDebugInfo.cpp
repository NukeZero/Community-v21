// WndArcane.cpp: implementation of the CWndNeuz class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "WndSample.h"
#include "WndDebugInfo.h"
#ifdef __ONLINE
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////////
// 캡춰 윈도 
/////////////////////////////////////////////////////////////////////////////////////

CWndCapture::CWndCapture()
{
}

CWndCapture::~CWndCapture()
{

}
void CWndCapture::OnDraw(C2DRender* p2DRender)
{

}
void CWndCapture::OnInitialUpdate()
{

	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect3_1( x - ( size.cx / 2) - size.cx - 10, y, (x - ( size.cx / 2) - size.cx - 10) + size.cx, y + size.cy );
	CRect rect3_2( x - ( size.cx / 2)               , y, (x - ( size.cx / 2)               ) + size.cx, y + size.cy );
	CRect rect3_3( x + ( size.cx / 2) + 10          , y, (x + ( size.cx / 2) + 10          ) + size.cx, y + size.cy );

	m_wndYes.Create( _T( "예" )   ,0, rect3_1,this,IDYES);//   ,m_pSprPack,9);
	m_wndNo.Create( _T( "아니오" ),0, rect3_2,this,IDNO);//    ,m_pSprPack,9);
	m_wndCancel.Create( _T( "취소" ),0, rect3_3,this,IDCANCEL);//,m_pSprPack,9);
	m_wndEdit.Create( g_Neuz.GetSafeHwnd(), 0, CRect( 5, 5, rect.Width() - 5, 35 ), this, 1000 );
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndCapture::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect( 250, 130 );
	SetTitle( _T( "Capture" ) );
	return CWndNeuz::Create( WBS_MOVE | WBS_MODAL| WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId );
}
BOOL CWndCapture::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndCapture::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndCapture::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCapture::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndCapture::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch(nID)
	{
		case IDYES:     
			_tcscpy( g_Neuz.m_szCaptureFileName, m_wndEdit.GetString() );
			g_Neuz.m_bCapture = TRUE;
			g_Neuz.m_nCaptureCnt = 0;
			CreateDirectory( _T( "Capture" ), NULL );
			Destroy(); 
			break;
		case IDNO:   
			g_Neuz.m_bCapture = FALSE;
			Destroy(); 
			break;
		case IDCANCEL:   
			Destroy(); 
			break;
	}
	//CWndDebugInfo* pWndDebug =  (CWndDebugInfo*)GetParentWnd();
	//pWndDebug->m_wndDebug[ 15 ].SetCheck( g_Neuz.m_bCapture );
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
/////////////////////////////////////////////////////////////////////////////////////
// 캡춰 윈도 
/////////////////////////////////////////////////////////////////////////////////////

CWndDebugInfo::CWndDebugInfo()
{
	SetPutRegInfo( FALSE );
}

CWndDebugInfo::~CWndDebugInfo()
{

}
void CWndDebugInfo::OnDraw(C2DRender* p2DRender)
{
    // Show frame rate
	p2DRender->TextOut( 2,  0 ,g_Neuz.m_strFrameStats , D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	p2DRender->TextOut( 2, 20 ,g_Neuz.m_strDeviceStats, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );

	TCHAR strDebug[90];

	if( g_pPlayer && g_pPlayer->GetWorld() )
	{
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		_stprintf( strDebug, _T("x:%6.3f y:%6.3f z:%6.3f %6.3f"), vPos.x, vPos.y, vPos.z,(float)g_pPlayer->GetAngle());
		p2DRender->TextOut( 2, 40, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		_stprintf( strDebug, _T("Objects:%d  Faces:%d   LandFace:%d %d" ), g_pPlayer->GetWorld()->m_nObjCullSize, g_nMaxTri,g_nRenderedLandTries, g_WndMng.m_pWndWorld->GetMouseMode() );
		p2DRender->TextOut( 2, 55, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}
	else
	{
		_stprintf( strDebug, _T( "x:0.0 y:0.0 z:%0.0 0.0" ) );
		p2DRender->TextOut( 2, 40, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		_stprintf( strDebug, _T("Objects:0  Faces:0" ) );
		p2DRender->TextOut( 2, 55, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}
}
void CWndDebugInfo::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요

	CWndButton* pWndButton;

	pWndButton = (CWndButton*)GetDlgItem( WIDC_GRID       ); pWndButton->SetCheck( g_WorldMng()->m_bViewGrid      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_TERRAIN    ); pWndButton->SetCheck( g_WorldMng()->m_bViewTerrain   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LOD_TERRAIN); pWndButton->SetCheck( g_WorldMng()->m_bViewLODTerrain);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LOD_OBJ    ); pWndButton->SetCheck( g_WorldMng()->m_bViewLODObj    );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_ANIMATION  ); pWndButton->SetCheck( CObj::IsAnimate()	 	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LIGHT      ); pWndButton->SetCheck( g_WorldMng()->m_bViewLight     );
	//pWndButton = (CWndButton*)GetDlgItem( WIDC_CAPTURE    ); pWndButton->SetCheck( 	 	);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HIDE       ); pWndButton->SetCheck( !g_pPlayer->IsVisible() );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WIREFRAME  ); pWndButton->SetCheck( g_WorldMng()->m_bViewWireframe  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_OBJECT     ); pWndButton->SetCheck( g_WorldMng()->m_bViewAllObjects );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_FRAME_SKIP ); pWndButton->SetCheck( GetFrameSkip()	               );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WEATHER    ); pWndButton->SetCheck( g_WorldMng()->m_bViewWeather	   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_BOUNDBOX   ); pWndButton->SetCheck( g_WorldMng()->m_bViewBoundBox   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_ATTRIBUTE  ); pWndButton->SetCheck( g_WorldMng()->m_bViewHeightAttribute	 	);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_SKYBOX     ); pWndButton->SetCheck( g_WorldMng()->m_bViewSkybox 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_NAME       ); pWndButton->SetCheck( g_WorldMng()->m_bViewName 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_CULLOBJ    ); pWndButton->SetCheck( g_WorldMng()->m_bCullObj	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_COLLISION  ); pWndButton->SetCheck( CObj::m_bCollision	 	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_FOG        ); pWndButton->SetCheck( g_WorldMng()->m_bViewFog 	  );	 
	pWndButton = (CWndButton*)GetDlgItem( WIDC_IDSTATE    ); pWndButton->SetCheck( g_WorldMng()->m_bViewIdState   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WATER      ); pWndButton->SetCheck( g_WorldMng()->m_bViewWater 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR       ); pWndButton->SetCheck( g_GameTimer.IsFixed()  );

	CString string;
	string.Format( "%d", g_GameTimer.m_nHour );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR ); pWndEdit->SetString( string );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR ); pWndButton->SetCheck(  g_GameTimer.IsFixed() );
	//g_World.m_bViewWater
	//////////////////////////////////////////////////////
	
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
} 
/*
void CWndDebugInfo::OnInitialUpdate()
{
	CRect rectClient = GetClientRect();
	rectClient.top = 80;

#define MAX_COMMAND 18

	static TCHAR aszCommand[MAX_COMMAND][30] = 
	{ 
		_T( "Grid" ), _T( "Wireframe" ), _T( "Skybox" ), _T( "Terrain" ), _T( "Object" ), _T( "Fog" ), _T( "LOD" ), 
		_T( "Water" ), _T( "Name" ), _T( "Animation" ), _T( "FrameSkip" ), _T( "CullObj" ), _T( "Light" ), 
		_T( "Weather" ), _T( "NudeSkin" ), _T( "Capture" ), _T( "BoundBox" ), _T( "Collision" )
	};

	for( int i = 0, i3 = 0; i < 6; i++ )
	{
		for( int i2 = 0; i2 < 3; i2++, i3++)
		{
			if( i3 < MAX_COMMAND )
			{
				m_wndDebug[ i3 ].Create( aszCommand[ i3], WBS_CHECK, CRect( rectClient.left + 2, rectClient.top, rectClient.left + 100, rectClient.top + 20 ), this, 100 + i3 ); 
				//m_wndDebug[ i3 ].SetPushColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
				//m_wndDebug[ i3 ].SetFontColor( D3DCOLOR_ARGB( 255, 000, 000, 255 ) );
			}
			rectClient.left += 100;
		}
		rectClient.left = 0;
		rectClient.top += 20;
	}
	m_wndEditExp.Create   ( g_Neuz.GetSafeHwnd(), 0, CRect( 5, 200, 100, 220 ), this, 1001 );
	m_wndButtonExp.Create ( _T( "Exp Up!" ), 0, CRect( 105, 200, 160, 220 ), this, 1002 ); 
	m_wndSetSkinSet.Create( _T( "SkinSet" ), 0, CRect(   5, 230,  60, 250 ), this, 1003 ); 
	m_wndSetHair.Create   ( _T( "Hair"    ), 0, CRect(  75, 230, 130, 250 ), this, 1004 ); 
	m_wndSetColor.Create  ( _T( "Color"   ), 0, CRect( 135, 230, 190, 250 ), this, 1005 ); 
	m_wndSetFace.Create   ( _T( "Face"    ), 0, CRect( 195, 230, 250, 250 ), this, 1006 ); 

//	m_wndDebug[ 0  ].SetCheck( g_World.m_bViewGrid      );
//	m_wndDebug[ 1  ].SetCheck( g_World.m_bViewWireframe );
//	m_wndDebug[ 2  ].SetCheck( g_World.m_bViewSkybox    );
//	m_wndDebug[ 3  ].SetCheck( g_World.m_bViewTerrain   );
//	m_wndDebug[ 4  ].SetCheck( g_World.m_bViewObject    );
//	m_wndDebug[ 5  ].SetCheck( g_World.m_bViewFog       );
//	m_wndDebug[ 6  ].SetCheck( g_World.m_bViewLOD       );
//	m_wndDebug[ 7  ].SetCheck( g_World.m_bViewWater     );
//	m_wndDebug[ 8  ].SetCheck( g_World.m_bViewName      );
	m_wndDebug[ 0  ].SetCheck( g_WorldMng()->m_bViewGrid      );
	m_wndDebug[ 1  ].SetCheck( g_WorldMng()->m_bViewWireframe );
	m_wndDebug[ 2  ].SetCheck( g_WorldMng()->m_bViewSkybox    );
	m_wndDebug[ 3  ].SetCheck( g_WorldMng()->m_bViewTerrain   );
	m_wndDebug[ 4  ].SetCheck( g_WorldMng()->m_bViewAllObjects   );
	m_wndDebug[ 5  ].SetCheck( g_WorldMng()->m_bViewFog       );
	m_wndDebug[ 6  ].SetCheck( g_WorldMng()->m_bViewLOD       );
	m_wndDebug[ 7  ].SetCheck( g_WorldMng()->m_bViewWater     );
	m_wndDebug[ 8  ].SetCheck( g_WorldMng()->m_bViewName      );

	m_wndDebug[ 9  ].SetCheck( CObj::IsAnimate()  );
	m_wndDebug[ 10 ].SetCheck( GetFrameSkip() );
//	m_wndDebug[ 11 ].SetCheck( g_World.m_bCullObj );
//	m_wndDebug[ 12 ].SetCheck( g_World.m_bViewLight );
//	m_wndDebug[ 13 ].SetCheck( g_World.m_bViewWeather );
	m_wndDebug[ 11 ].SetCheck( g_WorldMng()->m_bCullObj );
	m_wndDebug[ 12 ].SetCheck( g_WorldMng()->m_bViewLight );
	m_wndDebug[ 13 ].SetCheck( g_WorldMng()->m_bViewWeather );
	m_wndDebug[ 14 ].SetCheck( m_pTheme->m_bNudeSkin );
	m_wndDebug[ 15 ].SetCheck( g_Neuz.m_bCapture );
//	m_wndDebug[ 16 ].SetCheck( g_World.m_bViewBoundBox );
	m_wndDebug[ 16 ].SetCheck( g_WorldMng()->m_bViewBoundBox );
	m_wndDebug[ 17 ].SetCheck( CObj::m_bCollision );
	
	CWndNeuz::OnInitialUpdate();
}
*/
BOOL CWndDebugInfo::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DEBUGINFO, 0, CPoint( 0, 0 ), pWndParent );
/*	
//	CRect rect(0,0,280,190);
	CRect rect(0,0,280,270);
	SetTitle("디버그 정보");
	return CWndNeuz::Create( 0 | WBS_MOVE | WBS_SOUND | WBS_THICKFRAME | WBS_CAPTION, rect, &g_WndMng, dwWndId);
	*/
}
BOOL CWndDebugInfo::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CWndButton* pWndButton = (CWndButton*)*pLResult;
	//if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case WIDC_BUTTON1: // exp up
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			int nExp = _ttoi( pWndEdit->GetString() );
//#ifdef _DEBUG
			if( nExp >= 0 && nExp < 0x7fffffff )
//#else // _DEBUG
//			if( nExp >= 0 && nExp < 9999999 )
//#endif // _DEBUG
			{
	#ifdef __ONLINE
				g_DPlay.SendExp( nExp );
	#else
				g_pPlayer->AddExperience( nExp );
	#endif
			}
			break;
		}
			/*
		case 1003: // skinset
			g_pPlayer->SetSkinSet( _ttoi( m_wndEditExp.GetString() ) );
			break;
		case 1004: // hair
			g_pPlayer->SetHair( _ttoi( m_wndEditExp.GetString() ) );
			break;
		case 1005: // haircolor
			g_pPlayer->SetHairColor( _ttoi( m_wndEditExp.GetString() ) );
			break;
		case 1006: // face
			g_pPlayer->SetHead( _ttoi( m_wndEditExp.GetString() ) );
			break;
			
		case 1000:
			{
				CRect rect = GetWindowRect();
				m_wndExtend.SetCheck( !m_wndExtend.GetCheck() );
				if( m_wndExtend.GetCheck() )
					SetWndSize( rect.Width(), rect.Height() + 55 );
				else
					SetWndSize( rect.Width(), rect.Height() - 55 );
			}
			break;
			*/
//		case 100: g_World.m_bViewGrid      = !g_World.m_bViewGrid      ; 
		case WIDC_GRID: g_WorldMng()->m_bViewGrid      = !g_WorldMng()->m_bViewGrid      ; 
/*
			m_wndListBox.Create( WBS_MENU | WBS_NOFRAME, CRect(10,10,100,100), this, 10);
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
			m_wndListBox.AddString("test1");
*/		//	m_wndListBox.AddString("test1");m_wndListBox.AddString("test1");m_wndListBox.AddString("test1");
				
			break;
//		case 101: g_World.m_bViewWireframe = !g_World.m_bViewWireframe ; break;
//		case 102: g_World.m_bViewSkybox    = !g_World.m_bViewSkybox    ; break;
//		case 103: g_World.m_bViewTerrain   = !g_World.m_bViewTerrain   ; break;
//		case 104: g_World.m_bViewObject    = !g_World.m_bViewObject    ; break;
//		case 105: 
//			g_World.m_bViewFog = !g_World.m_bViewFog; 			    
//			g_World.SetFogEnable( g_Neuz.m_pd3dDevice, g_World.m_bViewFog ); 
//			break;
//		case 106: g_World.m_bViewLOD       = !g_World.m_bViewLOD       ; break;
//		case 107: g_World.m_bViewWater     = !g_World.m_bViewWater     ; break;
//		case 108: g_World.m_bViewName      = !g_World.m_bViewName      ; break;
		case WIDC_WIREFRAME: g_WorldMng()->m_bViewWireframe = !g_WorldMng()->m_bViewWireframe ; break;
		case WIDC_SKYBOX: g_WorldMng()->m_bViewSkybox    = !g_WorldMng()->m_bViewSkybox    ; break;
		case WIDC_TERRAIN: g_WorldMng()->m_bViewTerrain   = !g_WorldMng()->m_bViewTerrain   ; break;
		case WIDC_OBJECT: g_WorldMng()->m_bViewAllObjects   = !g_WorldMng()->m_bViewAllObjects   ; break;
		case 105: 
			g_WorldMng()->m_bViewFog = !g_WorldMng()->m_bViewFog; 			    
			g_WorldMng()->SetFogEnable( g_Neuz.m_pd3dDevice, g_WorldMng()->m_bViewFog ); 
			break;
		case WIDC_LOD_TERRAIN: g_WorldMng()->m_bViewLODTerrain       = !g_WorldMng()->m_bViewLODTerrain       ; break;
		case WIDC_LOD_OBJ: g_WorldMng()->m_bViewLODObj      = !g_WorldMng()->m_bViewLODObj       ; break;
		case WIDC_WATER: g_WorldMng()->m_bViewWater     = !g_WorldMng()->m_bViewWater     ; break;
		case WIDC_NAME: g_WorldMng()->m_bViewName      = !g_WorldMng()->m_bViewName      ; break;
		case WIDC_ANIMATION: CObj::AnimateEnable( !CObj::IsAnimate() ); break;
		case WIDC_FRAME_SKIP: SetFrameSkip( !GetFrameSkip() ); break;
		case WIDC_FOG: g_WorldMng()->m_bViewFog = !g_WorldMng()->m_bViewFog; break;
//		case 111: g_World.m_bCullObj = !g_World.m_bCullObj; break;
//		case 112: g_World.m_bViewLight = !g_World.m_bViewLight; break;
//		case 113: g_World.m_bViewWeather = !g_World.m_bViewWeather; break;
		case WIDC_CULLOBJ: g_WorldMng()->m_bCullObj = !g_WorldMng()->m_bCullObj; break;
		case WIDC_LIGHT: g_WorldMng()->m_bViewLight = !g_WorldMng()->m_bViewLight; break;
		case WIDC_WEATHER: g_WorldMng()->m_bViewWeather = !g_WorldMng()->m_bViewWeather; break;
		case WIDC_ATTRIBUTE: g_WorldMng()->m_bViewHeightAttribute = !g_WorldMng()->m_bViewHeightAttribute; break;
		case WIDC_HIDE: g_pPlayer->SetVisible( !g_pPlayer->IsVisible() ); break;
		//case WIDC_114: m_pTheme->m_bNudeSkin = !m_pTheme->m_bNudeSkin; break;
			/*
		case 115: 
			if( g_Neuz.m_bCapture == FALSE )
			{
				m_wndDebug[ 15 ].SetCheck( !m_wndDebug[ 15 ].GetCheck() );
				m_pWndCapture = new CWndCapture;
				m_pWndCapture->Initialize( this, 100000 );
			} 
			else
			{
				g_Neuz.m_bCapture = FALSE;
				m_wndDebug[ 15 ].SetCheck( g_Neuz.m_bCapture );
			}
			break;
			*/
		case WIDC_BOUNDBOX: 
//			g_World.m_bViewBoundBox = !g_World.m_bViewBoundBox;
			g_WorldMng()->m_bViewBoundBox = !g_WorldMng()->m_bViewBoundBox;
			break;
		case WIDC_COLLISION:
			CObj::m_bCollision = !CObj::m_bCollision;
			{
				CWndSample* pWndSample = new CWndSample;
				pWndSample->Initialize();
			}
		case WIDC_IDSTATE:
			g_WorldMng()->m_bViewIdState = !g_WorldMng()->m_bViewIdState;
			break;
		case WIDC_HOUR:
			 g_GameTimer.SetFixed( !g_GameTimer.IsFixed() );
			 pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR );
			 if( pWndButton->GetCheck() )
			 {
				 CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR );
				 int nHour = atoi( pWndEdit->GetString() );
				 if( nHour > 23 ) nHour = 23;
				 if( nHour < 0 ) nHour = 0;					 
				 g_GameTimer.SetTime( 0, nHour, 0, 0 );//Fixed( 	g_GameTimer.IsFixed() );//g_WorldMng()->m_nFixedHour = nHour;
			 }
			 
			break;
		case WIDC_EDIT_HOUR:
			{
				pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR );
				if( pWndButton->GetCheck() )
				{
					CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR );
					int nHour = atoi( pWndEdit->GetString() );
					if( nHour > 23 ) nHour = 23;
					if( nHour < 0 ) nHour = 0;					 
					g_GameTimer.SetTime( 0, nHour, 0, 0 );//Fixed( 	g_GameTimer.IsFixed() );//g_WorldMng()->m_nFixedHour = nHour;
				}
			}
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndDebugInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndDebugInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndDebugInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonUp(nFlags,point);
}
void CWndDebugInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonDown(nFlags,point);
}
void CWndDebugInfo::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if( m_pWndCapture == pWndChild )
		SAFE_DELETE( m_pWndCapture );
}	

/////////////////////////////////////////////////////////////////////////////////////
// 윈도 콘트롤 테스트 윈도
/////////////////////////////////////////////////////////////////////////////////////
CWndControlTest::CWndControlTest()
{
}

CWndControlTest::~CWndControlTest()
{

}
void CWndControlTest::OnDraw(C2DRender* p2DRender)
{

}
void CWndControlTest::OnInitialUpdate()
{
	m_wndMusic.Create( "Music", WBS_CHECK, CRect( 0,  0, 100, 20 ), this, 100 ); 
	m_wndSound.Create( "Sound", WBS_CHECK, CRect( 0, 25, 100, 50 ), this, 101 ); 
	//m_wndMusic.SetPushColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	//m_wndMusic.SetFontColor( D3DCOLOR_ARGB( 255, 000, 000, 255 ) );
	//m_wndSound.SetPushColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
	//m_wndSound.SetFontColor( D3DCOLOR_ARGB( 255, 000, 000, 255 ) );
	//m_wndMusic.SetCheck( g_Music.m_nMusicVolume );
	m_wndSound.SetCheck( g_SoundMng.m_nSoundVolume );
	m_wndComboBox.Create( 0, CRect( 1, 40, 200, 60 ), this, 10 );
	m_wndlistCtrl.Create( WLVS_ICON, CRect( 1, 65, 200, 200 ), this, 11 );
	m_wndButtonIcon.Create  ( "Icon"  , WBS_RADIO, CRect(   1, 210,  50, 230 ), this, 200 );
	m_wndButtonReport.Create( "Report", WBS_RADIO, CRect(  51, 210, 100, 230 ), this, 201 );
	m_wndButtonList.Create  ( "List"  , WBS_RADIO, CRect( 101, 210, 150, 230 ), this, 202 );
	m_wndButtonIcon.SetGroup( TRUE );

	TCHAR	szTitle[3][10] = {_T("File Name"), _T("Id"), _T("최대")};
	
	LV_COLUMN  pColumn;
	LV_ITEM	   lvitem;
	TCHAR      strNum[10];
	//
	// Report Column을 입력 
	//
	for(int i = 0;  i < 3; i++)
	{
		pColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		pColumn.fmt = LVCFMT_LEFT;
		pColumn.iSubItem = i;
		pColumn.cx = 60;  // SubItem is twice as large
		pColumn.pszText = szTitle[i];	
		m_wndlistCtrl.InsertColumn(i, &pColumn);  // assumes return value is OK.
	}
	//
	// 소스 아이템을 입력
	//
	for(i = 0; i <  256/*prj.m_terrainMng. GetItemPropSize()*/; i++) 
	{
		LPTERRAIN lpTerrain = prj.m_terrainMng.GetTerrain( i );
		if( lpTerrain->IsValid() )
		{
			lvitem.mask = LVIF_TEXT | LVIF_PARAM; // | (iSubItem == 0? LVIF_IMAGE : 0);
			lvitem.iItem = m_wndlistCtrl.GetItemCount(); //(iSubItem == 0)? iItem : iActualItem;
			lvitem.iSubItem = 0;
			lvitem.lParam = (LPARAM) lpTerrain;
			lvitem.pszText = lpTerrain->m_szTextureFileName; 
			m_wndlistCtrl.InsertItem(&lvitem);

			lvitem.mask = LVIF_TEXT;
			lvitem.iSubItem = 1; 
			_itoa(lpTerrain->m_dwId,strNum,10);
			lvitem.pszText = strNum;
			m_wndlistCtrl.SetItem(&lvitem);

			lvitem.iSubItem = 2; 
			//_itoa(pItemProp->m_nMax,strNum,10);
			lvitem.pszText = "b";
			m_wndlistCtrl.SetItem(&lvitem);
		}
	}

	CRect rect( 210,0,300,100);
	m_wndText.Create(WBS_CHILD,rect,this,10);
	rect = m_wndText.GetClientRect();
	m_wndText.m_string.Init( m_pFont, &rect );
	m_wndText.m_string.SetString( _T("나는\n 자랑스러운\n 태극기 앞에 조국과 민족의 무궁한 영광을 위하여 몸과 마음을 바쳐 충성을 다할 것을 굳게 다짐합니다.\n 할랠루야 ") );

	CWndNeuz::OnInitialUpdate();
}
BOOL CWndControlTest::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect(0,0,580,590);
	SetTitle("콘트롤 테스트");
	return CWndNeuz::Create( 0 | WBS_MOVE |/* WBS_DOCKABLE |*/ WBS_SOUND | WBS_THICKFRAME | WBS_CAPTION, rect, &g_WndMng, dwWndId);
}

BOOL CWndControlTest::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch(nID)
	{
		case 100: // wnd2
			/*
			g_Music.m_nMusicVolume = !g_Music.m_nMusicVolume;
			if ( g_Music.m_nMusicVolume == 0 )
				g_Music.Stop();
			else
				g_Music.Play();
				*/
			break;
		case 101: // wnd1 
			g_SoundMng.m_nSoundVolume = !g_SoundMng.m_nSoundVolume;
			break;
		case 200: // icon
			m_wndlistCtrl.AddWndStyle( WLVS_ICON );
			m_wndlistCtrl.DelWndStyle( WLVS_REPORT );
			m_wndlistCtrl.SetWndRect( m_wndlistCtrl.GetWindowRect( TRUE ) );
			break;
		case 201: // report
			m_wndlistCtrl.DelWndStyle( WLVS_ICON );
			m_wndlistCtrl.AddWndStyle( WLVS_REPORT );
			m_wndlistCtrl.SetWndRect( m_wndlistCtrl.GetWindowRect( TRUE ) );
			break;
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndControlTest::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndControlTest::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndControlTest::OnLButtonDown(UINT nFlags, CPoint point)
{
}