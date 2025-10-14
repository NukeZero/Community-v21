
#include "StdAfx.h"

#include "WndMadrigalGift.h"

// 18차 때문에 어쩔 수 없이 포함 시킨다.
#include "../Neuz/DPClient.h"
#include "../_CommonDefine/Packet/FLPacketMadrigalGift.h"
extern CDPClient				g_DPlay;

#define NEXT_INCREASELEVEL		10		//  다음 보상 레벨 (기획 의도 상 무조건 10증가다)

//---------------------------------------------------------------------------------------------
// 생성자
//---------------------------------------------------------------------------------------------
CWndMadrigalGift::CWndMadrigalGift()
:	m_pWndButtonGiftBox( NULL )
,	m_pGiftBox( NULL )
,	m_pRoundGauge( NULL )
,	m_pRoundArrow( NULL )
,	m_rtGiftBox( 0, 0, 0, 0 )
,	m_rtRoundGauge( 0, 0, 0, 0 )
,	m_rtRoundArrow( 0, 0, 0, 0 )
,	m_ptRoundGaugeCenter( 0, 0 )
,	m_wRoundGaugeRadius( 0 )
,	m_eStep( eStep_None )
,	m_byPointPercent( 0 )
,	m_wLevelForCompensation( 0 )
,	m_pTexRoundArrow( NULL )
,	m_dwRoundGaugeColor( WNDCOLOR_WHITE )
,	m_bSendPacket( false )

{
	ZeroMemory( m_pTexGiftBox, sizeof( CTexture* ) * eStep_Max );
}


//---------------------------------------------------------------------------------------------
// 파괴자
//---------------------------------------------------------------------------------------------
CWndMadrigalGift::~CWndMadrigalGift()
{
}


//---------------------------------------------------------------------------------------------
// 초기화
// param	: 부모 윈도우, 생성 윈도우 아이디
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndMadrigalGift::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	FLERROR_ASSERT_LOG_RETURN( pWndParent,	FALSE, _T("[마드리갈의 선물] 부모 윈도우 포인터가 유효하지 않습니다.") );
	FLERROR_ASSERT_LOG_RETURN( dwWndId,		FALSE, _T("[마드리갈의 선물] 생성 윈도우 아이디가 유효하지 않습니다.") );

	AddWndStyle( WBS_NODRAWFRAME );
	DelWndStyle( WBS_CAPTION );

	m_bNoCloseButton =  TRUE;
	m_bNoHelpButton =	TRUE;

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 0, 0 ), pWndParent );
}


//---------------------------------------------------------------------------------------------
// 업데이트 초기화
// param	: 없음
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMadrigalGift::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	m_pWndButtonGiftBox = static_cast<CWndButton*>( GetDlgItem( WIDC_BUTTON1 ) );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pWndButtonGiftBox, _T("[마드리갈의 선물] 선물 박스 버튼 포인터가 유효하지 않습니다.") );

	m_pGiftBox = GetWndCtrl( WIDC_BUTTON1 );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pGiftBox, _T("[마드리갈의 선물] 선물 박스 컨트롤 포인터가 유효하지 않습니다."))

	m_rtGiftBox = m_pGiftBox->rect;

	m_pRoundGauge = GetWndCtrl( WIDC_CUSTOM1 );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pRoundGauge, _T("[마드리갈의 선물] 원 게이지 컨트롤 포인터가 유효하지 않습니다."))
	
	m_rtRoundGauge = m_pRoundGauge->rect;

	m_pRoundArrow = GetWndCtrl( WIDC_CUSTOM2 );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pRoundArrow, _T("[마드리갈의 선물] 화살표 배경 컨트롤 포인터가 유효하지 않습니다.") );

	m_rtRoundArrow = m_pRoundArrow->rect;

	m_pTexRoundArrow = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), _T("MadrigalGift_Bg.tga") ), 0, TRUE );
	FLERROR_ASSERT_LOG_RETURN_VOID( m_pRoundArrow, _T("[마드리갈의 선물] 화살표 배경 텍스쳐를 불러올 수 없습니다.") );

	if( m_rtRoundGauge.Width() != m_rtRoundGauge.Height() )
		FLERROR_LOG( PROGRAM_NAME, _T("[마드리갈의 선물] 게이지 대상 영역이 정사각형이 아닙니다. 문제가 생길 수 있습니다. 데이지를 확인하세요") );

	m_ptRoundGaugeCenter.x = m_rtRoundGauge.left + m_rtRoundGauge.Width() / 2;
	m_ptRoundGaugeCenter.y = m_rtRoundGauge.top + m_rtRoundGauge.Height() / 2;

	/// +1은 테스트
	// 정사각형이라는 가정하예 반지름은 그냥 구한다.
	m_wRoundGaugeRadius = (m_rtRoundGauge.Width() / 2) + 1;

	for( BYTE byCount = 0; byCount < eStep_Max; byCount++ )
	{
		if( 0 == byCount )
			continue;
		
		TCHAR szTextureName[32] = { NULL, };
		_sntprintf_s( szTextureName, _countof( szTextureName ), _TRUNCATE, _T( "buttMadrigalgift%02d.tga" ), byCount );
		
		m_pTexGiftBox[byCount] = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, g_xFlyffConfig->GetMainLanguage(), szTextureName ), 0, TRUE );
	}

	if( eStep_None == m_eStep )
		m_pWndButtonGiftBox->EnableWindow( FALSE );
}  


//---------------------------------------------------------------------------------------------
// 현재 단계의 선물 박스 텍스쳐 설정하기
// param	: 단계
// return	: 색상
//---------------------------------------------------------------------------------------------
void CWndMadrigalGift::SetGiftBoxTexture()
{
	switch( m_eStep )
	{
		case eStep_None:
			{
				if( m_pTexGiftBox[eStep_1st] )
					m_pWndButtonGiftBox->m_pTexture = m_pTexGiftBox[eStep_1st];

				m_pWndButtonGiftBox->EnableWindow( FALSE );
			}
			break;
		case eStep_1st:
			{
				if( m_pTexGiftBox[eStep_1st] )
					m_pWndButtonGiftBox->m_pTexture = m_pTexGiftBox[eStep_1st];

				m_pWndButtonGiftBox->EnableWindow( TRUE );
			}
			break;
		case eStep_2nd:
			{
				if( m_pTexGiftBox[eStep_2nd] )
					m_pWndButtonGiftBox->m_pTexture = m_pTexGiftBox[eStep_2nd];

				m_pWndButtonGiftBox->EnableWindow( TRUE );
			}
			break;
		case eStep_3rd:
			{
				if( m_pTexGiftBox[eStep_3rd] )
					m_pWndButtonGiftBox->m_pTexture = m_pTexGiftBox[eStep_3rd];

				m_pWndButtonGiftBox->EnableWindow( TRUE );
			}
			break;
		default:
			{
				if( m_pTexGiftBox[eStep_1st] )
					m_pWndButtonGiftBox->m_pTexture = m_pTexGiftBox[eStep_1st];

				m_pWndButtonGiftBox->EnableWindow( FALSE );

				FLERROR_ASSERT_LOG_RETURN_VOID( NULL, _T("[마드리갈의 선물] 없는 선물 단계 입니다. 기본 텍스쳐로 대채합니다.") );	
			}
			break;
	}
}


//---------------------------------------------------------------------------------------------
// 현재 단계의 원 게이지 색상얻기
// param	: 단계
// return	: 색상
//---------------------------------------------------------------------------------------------
DWORD CWndMadrigalGift::GetRoundGaugeColor()
{
	PFLCOLOR_WNDMADRIGALGIFT pColorWndMadrigalGift = g_FLColorManager->GetWndMadrigalGift();

	switch( m_eStep )
	{
		case eStep_None:
			return pColorWndMadrigalGift->m_stRoundGuage_StepNone.GetColor();
		case eStep_1st:
			return pColorWndMadrigalGift->m_stRoundGuage_Step1.GetColor();
		case eStep_2nd:
			return pColorWndMadrigalGift->m_stRoundGuage_Step2.GetColor();
		case eStep_3rd:
			return pColorWndMadrigalGift->m_stRoundGuage_Step3.GetColor();
		default:
			{
				FLERROR_ASSERT_LOG_RETURN( NULL, WNDCOLOR_BLACK, _T("[마드리갈의 선물] 없는 선물 단계 입니다.") );	
			}
			break;	
	}

	FLERROR_ASSERT_LOG_RETURN( NULL, WNDCOLOR_BLACK, _T("[마드리갈의 선물] 로직 에러발생 들어올 수 없는 구문입니다.") );	
}


//---------------------------------------------------------------------------------------------
// 처리
// param	: void
// return	: 성공 / 실패
//---------------------------------------------------------------------------------------------
BOOL CWndMadrigalGift::Process()
{
	if( m_pGiftBox )
		m_rtGiftBox = m_pGiftBox->rect;

	if( m_pRoundGauge )
		m_rtRoundGauge = m_pRoundGauge->rect;

	if( m_pRoundArrow )
		m_rtRoundArrow = m_pRoundArrow->rect;

	SetGiftBoxTexture();
	m_dwRoundGaugeColor = GetRoundGaugeColor();

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 그리기
// param	: 렌더러
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMadrigalGift::OnDraw( C2DRender* p2DRender )
{
	if( NULL == p2DRender )
		return ;
	
	const BYTE byMaxPoint = 100;

	float fDegree = static_cast<float>( ( 360.0f * m_byPointPercent ) / byMaxPoint );

	D3DXVECTOR2		vCenterPos( static_cast<float>(m_ptRoundGaugeCenter.x), static_cast<float>(m_ptRoundGaugeCenter.y) );
	D3DXVECTOR2		vStandardPos( static_cast<float>(m_ptRoundGaugeCenter.x), static_cast<float>(m_ptRoundGaugeCenter.y - m_wRoundGaugeRadius) );

	float fRadian( D3DXToRadian( 3.6f ) );	// 360의 1%는 3.6 이므로 ( Degree를 Radian으로 변경 )

	for( BYTE byCount = 0; byCount < m_byPointPercent; byCount++ )
	{
		D3DXVECTOR2 vStandardAxis = vCenterPos - vStandardPos;

		D3DXVECTOR2 vRot( 0.0f, 0.0f );

		D3DXMATRIX mRotZ;
		D3DXMatrixIdentity( &mRotZ );
		D3DXMatrixRotationZ( &mRotZ, fRadian );
	
		D3DXVec2TransformCoord( &vRot, &vStandardAxis, &mRotZ );  
			
		vRot = vCenterPos - vRot; 

		p2DRender->RenderFillTriangle(	m_ptRoundGaugeCenter, 
										CPoint( (int)vStandardPos.x, (int)vStandardPos.y ), 
										CPoint( (int)vRot.x, (int)vRot.y ), 
										m_dwRoundGaugeColor, 
										m_dwRoundGaugeColor, 
										m_dwRoundGaugeColor
									 );
			
		vStandardPos = vRot;
	}

	DrawRoundArrow( p2DRender );
	DrawTooltip( p2DRender );
}


//---------------------------------------------------------------------------------------------
// 픽킹 공간인가?
// param	: 마우스 위치
// return	: 예 / 아니요
//---------------------------------------------------------------------------------------------
BOOL CWndMadrigalGift::IsPickupSpace( CPoint point )
{
	if( m_rtGiftBox.PtInRect( point ) )
		return TRUE;

	if( m_rtRoundGauge.PtInRect( point ) )
		return TRUE;

	return TRUE;
}


//---------------------------------------------------------------------------------------------
// 툴팁 텍스트 얻기
// param	: void
// return	: 문자열 포인터
//---------------------------------------------------------------------------------------------
CEditString CWndMadrigalGift::GetToolTipText()
{
	PFONTCOLOR_WNDMADRIGALGIFT pFontColorWndMadrigalGift = g_WndFontColorManager->GetWndMadrigalGift();

	CString			strTemp( _T("") );
	CEditString		strResult( _T("") );
	BYTE			byLegend( g_pPlayer->GetLegendChar() );


	switch( m_eStep )
	{
		case eStep_None:
			{
				strTemp.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT03 ), ( BYTE )m_eStep );
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT04 ), m_byPointPercent );
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp = GetCurrentLevelForCompensationToolTipText();
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT02 ), ( BYTE )(m_eStep + 1) );
				strResult.AddParsingString( strTemp );
				strTemp.Empty();

				return strResult;
			}
			break;
		case eStep_1st:
		case eStep_2nd:
			{
				strTemp = GetCurrentLevelForCompensationToolTipText();
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT04 ), m_byPointPercent );
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp = GetNextLevelForCompensationToolTipText();
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT08 ), ( BYTE )m_eStep );
				strResult.AddParsingString( strTemp );
				strTemp.Empty();

				return strResult;
			}
			break;
		case eStep_3rd:
			{
				strTemp = GetCurrentLevelForCompensationToolTipText();
				strResult.AddParsingString( strTemp, pFontColorWndMadrigalGift->m_stToolTip.GetFontColor() );
				strTemp.Empty();

				strTemp.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT08 ), ( BYTE )m_eStep );
				strResult.AddParsingString( strTemp );
				strTemp.Empty();

				return strResult;
			}
			break;
		default:
			{
				FLERROR_ASSERT_LOG_RETURN( NULL, strResult, _T("[마드리갈의 선물] 없는 선물 단계 입니다. 툴팁 문자열을 찾을 수 없습니다.") );	
			}
			break;	
	}

	FLERROR_ASSERT_LOG_RETURN( NULL, strResult, _T("[마드리갈의 선물] 로직 에러발생 들어올 수 없는 구문입니다.") );	
}


//---------------------------------------------------------------------------------------------
// 현재 단계의 레벨 보상 툴팁 텍스트 얻기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
CString CWndMadrigalGift::GetCurrentLevelForCompensationToolTipText()
{
	CString strToolTip( _T("") );

	switch( g_pPlayer->GetLegendChar() )
	{
		case LEGEND_CLASS_MASTER:
			strToolTip.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT06 ), ( BYTE )m_eStep, prj.GetText( TID_MMI_JOBGRADE_MASTER_TEXT ), m_wLevelForCompensation );
			break;
		case LEGEND_CLASS_HERO:
		case LEGEND_CLASS_LEGEND_HERO:		
			strToolTip.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT06 ), ( BYTE )m_eStep, prj.GetText( TID_MMI_JOBGRADE_HERO_TEXT ), m_wLevelForCompensation );
			break;

		default:
			strToolTip.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT06 ), ( BYTE )m_eStep, _T(""), m_wLevelForCompensation );
			break;
	}

	return strToolTip;
}


//---------------------------------------------------------------------------------------------
// 다음 단계의 레벨 보상 툴팁 텍스트 얻기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
CString CWndMadrigalGift::GetNextLevelForCompensationToolTipText()
{
	CString strToolTip( _T("") );

	switch( g_pPlayer->GetLegendChar() )
	{
		case LEGEND_CLASS_MASTER:
			strToolTip.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT07 ), ( BYTE )(m_eStep + 1), prj.GetText( TID_MMI_JOBGRADE_MASTER_TEXT ), m_wLevelForCompensation + NEXT_INCREASELEVEL );
			break;
		case LEGEND_CLASS_HERO:
		case LEGEND_CLASS_LEGEND_HERO:		
			strToolTip.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT07 ), ( BYTE )(m_eStep + 1), prj.GetText( TID_MMI_JOBGRADE_HERO_TEXT ), m_wLevelForCompensation + NEXT_INCREASELEVEL );
			break;
		default:
			strToolTip.Format( prj.GetText( TID_MMI_MADRIGALGIFT_TEXT07 ), ( BYTE )(m_eStep + 1), _T(""), m_wLevelForCompensation + NEXT_INCREASELEVEL );
			break;
	}

	return strToolTip;
}


//---------------------------------------------------------------------------------------------
// 아이템 툴팁 그리기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndMadrigalGift::DrawTooltip( C2DRender* p2DRender  )
{
	if( NULL == p2DRender )
		return false;

	if( m_pWndOnMouseMove != this &&
		m_pWndOnMouseMove != m_pWndButtonGiftBox )
		return false;

	CPoint	ptPoint( GetMousePoint() );
	CRect	rtClientRect( m_rtRoundArrow );		

	if( FALSE == rtClientRect.PtInRect( ptPoint ) )
		return false;

	if( m_bLButtonDown )
		return false;

	CPoint	ptTemp = ptPoint;
	CRect	rtTemp = rtClientRect;

	ClientToScreen( &ptTemp );
	ClientToScreen( &rtTemp );
		
	CEditString strToolTip( GetToolTipText() ); 
	g_toolTip.PutToolTip( (DWORD)this , strToolTip, rtTemp, ptTemp, 3 );

	return true;
}


//---------------------------------------------------------------------------------------------
// 원 모양의 화살표 배경 그리기
// param	: 렌더러
// return	: bool
//---------------------------------------------------------------------------------------------
bool CWndMadrigalGift::DrawRoundArrow( C2DRender* p2DRender  )
{
	if( NULL == p2DRender )
		return false;

	CPoint	ptPoint( GetMousePoint() );
	CRect	rtClientRect( GetClientRect() );		

	if( rtClientRect.PtInRect( ptPoint ) && m_pTexRoundArrow )
		p2DRender->RenderTexture( m_rtRoundArrow.TopLeft(), m_pTexRoundArrow );

	return true;
}


//---------------------------------------------------------------------------------------------
// 명령이 왔을 때의 이벤트 처리
// param	: 윈도우 아이디, 메시지, 기본 윈도우
// return	: BOOL
//---------------------------------------------------------------------------------------------
BOOL CWndMadrigalGift::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	if( WIDC_BUTTON1 == nID )			// 선물버튼 
	{
		if( false == m_bSendPacket &&	// 패킷을 보내지 않았고
			IsGiftBoxEnable() )			// 박스 버튼이 활성화 되어 있디마녀
		{
			FLPacketMadrigalGiftItemReceiveReq kPacket;
			g_DPlay.SendPacket( &kPacket );
			SetGiftBoxButtonEnable( false );
			m_bSendPacket = true;
		}
	}

	return CWndBase::OnCommand( nID, dwMessage, pWndBase );
}


//---------------------------------------------------------------------------------------------
// 선물 상자 버튼 활성화 / 비 활성화
// param	: 활성 / 비 활성
// return	: void
//---------------------------------------------------------------------------------------------
void CWndMadrigalGift::SetGiftBoxButtonEnable( bool bEnable )
{
	if( m_pWndButtonGiftBox )
		m_pWndButtonGiftBox->EnableWindow( bEnable );
}


//---------------------------------------------------------------------------------------------
// 선물 상자 버튼이 활성화 되어 있나? 
// param	: void
// return	: 활성 / 비 활성
//---------------------------------------------------------------------------------------------
bool CWndMadrigalGift::IsGiftBoxEnable()
{
	if( m_pWndButtonGiftBox )
		return m_pWndButtonGiftBox->IsWindowEnabled() ? true : false;

	return false;
}