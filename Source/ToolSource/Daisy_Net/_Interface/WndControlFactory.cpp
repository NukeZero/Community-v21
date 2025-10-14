
#include "StdAfx.h"
#include "WndControlFactory.h"

//---------------------------------------------------------------------------------------------
// 생성자 
//---------------------------------------------------------------------------------------------
CWndControlFactory::CWndControlFactory()
{
}


//---------------------------------------------------------------------------------------------
// 파괴자 
//---------------------------------------------------------------------------------------------
CWndControlFactory::~CWndControlFactory()
{
}


//---------------------------------------------------------------------------------------------
// Create Daisy Control ( 컨트롤 생성하기 )
// param	: dwControlType
// return	: bool 
//---------------------------------------------------------------------------------------------
WNDCTRL CWndControlFactory::CreateControl( DWORD dwControlType, CWndDialog* pWndDialog )
{
	WNDCTRL stResWndControl;

	LPWNDCTRL pWndControl( NULL );
	pWndControl = GetControlByWndType( dwControlType );

	FLERROR_ASSERT_LOG_RETURN( pWndControl, stResWndControl, _T("[컨트롤 팩토리]컨트롤을 생성 할 수 없습니다.") );			

	pWndControl->dwWndId = CWndBase::m_resMng.GetNewControlId( pWndDialog->GetWndId() );
	FLINFO_LOG( PROGRAM_NAME, _T("[컨트롤 팩토리]새로운 컨트롤 생성 - TYPE : [%u], ID : [%u]"), dwControlType, pWndControl->dwWndId );
	pWndControl->strDefine = CWndBase::m_resMng.GetNewControlDefine( pWndDialog->GetWndId(), pWndControl->strDefine ); 
	FLINFO_LOG( PROGRAM_NAME, _T("[컨트롤 팩토리]새로운 컨트롤 디파인 생성 - [%s]"), pWndControl->strDefine );

	stResWndControl = (*pWndControl);

	SAFE_DELETE( pWndControl );
	return stResWndControl;
}


//---------------------------------------------------------------------------------------------
// Render Preview Control ( 미리보기 컨트롤 그리기 )
// param	: dwControlType, p2DRender, pMousePoint
// return	: LPWNDCTRL 
//---------------------------------------------------------------------------------------------
bool CWndControlFactory::RenderPreviewControl( DWORD dwControlWndType, C2DRender* p2DRender, CPoint* pMousePoint )
{
	FLERROR_ASSERT_LOG_RETURN( p2DRender, false, _T("[컨트롤 팩토리]렌더러 포인터가 유효하지 않습니다.") );			
	FLERROR_ASSERT_LOG_RETURN( pMousePoint, false, _T("[컨트롤 팩토리]마우스 위치 포인터가 유효하지 않습니다.") );			

	if( NULL == dwControlWndType )
		return false;

	LPWNDCTRL pWndControl( NULL );
	pWndControl = GetControlByWndType( dwControlWndType );

	int nPreviewControlBoxLeft		= pMousePoint->x + pWndControl->m_stPreviewControlOffSet.left;
	int nPreviewControlBoxTop		= pMousePoint->y + pWndControl->m_stPreviewControlOffSet.top;
	int nPreviewControlBoxRight		= pMousePoint->x + pWndControl->m_stPreviewControlOffSet.right;
	int nPreviewControlBoxBottom	= pMousePoint->y + pWndControl->m_stPreviewControlOffSet.bottom;

	CRect stRect( nPreviewControlBoxLeft, nPreviewControlBoxTop, nPreviewControlBoxRight, nPreviewControlBoxBottom );
	p2DRender->RenderRect(stRect, pWndControl->m_dwPreviewControlBoxColor);
	
	int nTextPosX = pMousePoint->x - pWndControl->m_stPreviewControlTextPos.x;
	int nTextPosY = pMousePoint->y - pWndControl->m_stPreviewControlTextPos.y;

	p2DRender->TextOut( nTextPosX, nTextPosY, pWndControl->strTitle, pWndControl->m_dwPreviewControlTextColor );

	SAFE_DELETE( pWndControl );
	return false;
}


//---------------------------------------------------------------------------------------------
// Get Control By Window Type ( 윈도우 타입에 따른 컨트롤 생성, ※동적 할당해서 리턴합니다!! 꼭 해제 해 주세요! )
// param	: dwControlType
// return	: LPWNDCTRL 
//---------------------------------------------------------------------------------------------
LPWNDCTRL CWndControlFactory::GetControlByWndType( DWORD dwControlWndType )
{
	FLERROR_ASSERT_LOG_RETURN( dwControlWndType, NULL, _T("[컨트롤 팩토리]올바르지 않은 컨트롤 타입 입니다.") );			

	LPWNDCTRL pWndControl( NULL );

	switch( dwControlWndType )
	{
		case ID_WND_BUTTON:
			pWndControl = new WNDCTRL_BUTTON;
			break;				
		case ID_WND_CHECK:
			pWndControl = new WNDCTRL_CHECKBOX;
			break;
		case ID_WND_RADIO:
			pWndControl = new WNDCTRL_RADIOBUTTON;
			break;
		case ID_WND_STATIC:
			pWndControl = new WNDCTRL_STATICBOX;
			break;
		case ID_WND_GROUPBOX:
			pWndControl = new WNDCTRL_GROUPBOX;
			break;
		case ID_WND_PICTURE:
			pWndControl = new WNDCTRL_PICTUREBOX;
			break;
		case ID_WND_TEXT:
			pWndControl = new WNDCTRL_TEXTBOX;
			break;
		case ID_WND_EDIT:
			pWndControl = new WNDCTRL_EDITBOX;
			break;
		case ID_WND_LISTBOX:
			pWndControl = new WNDCTRL_LISTBOX;
			break;
		case ID_WND_LISTCTRL:
			pWndControl = new WNDCTRL_LISTCONTROL;
			break;
		case ID_WND_COMBOBOX:
			pWndControl = new WNDCTRL_COMBOBOX;
			break;
		case ID_WND_TABCTRL:
			pWndControl = new WNDCTRL_TABCONTROL;
			break;
		case ID_WND_TREECTRL:
			pWndControl = new WNDCTRL_TREECONTROL;
			break;
		case ID_WND_CUSTOM:
			pWndControl = new WNDCTRL_CUSTOMCONTROL;
			break;
		default:
			FLERROR_ASSERT_LOG_DO( NULL, break, _T("[컨트롤 팩토리]알수 없는 컨트롤 타입 입니다 - TYPE : [%u]"), dwControlWndType );
	}

	return pWndControl;
}


//---------------------------------------------------------------------------------------------
// Get Description ( 컨트롤의 기술명 얻기 ) - 구조를 바꾸기에는 너무 많은 부분을 손대야 기에 현실과 타협하였습니다... 미안합니다.
// param	: dwWndType, dwWndStyle
// return	: CString 
//---------------------------------------------------------------------------------------------
CString CWndControlFactory::GetDescription( DWORD dwWndType, DWORD dwWndStyle )
{
	LPWNDCTRL pWndControl( NULL );

	switch( dwWndType )
	{
		case WTYPE_BUTTON:
			{
				if( WBS_CHECK & dwWndStyle )			
					pWndControl = new WNDCTRL_CHECKBOX;
				else if( WBS_RADIO & dwWndStyle )
					pWndControl = new WNDCTRL_RADIOBUTTON;
				else
					pWndControl = new WNDCTRL_BUTTON;
			}
			break;		

		case WTYPE_STATIC:
			{
				if( WSS_GROUPBOX & dwWndStyle )			
					pWndControl = new WNDCTRL_GROUPBOX;
				else if( WSS_PICTURE & dwWndStyle )
					pWndControl = new WNDCTRL_PICTUREBOX;
				else
					pWndControl = new WNDCTRL_STATICBOX;
			}
			break;	

		case WTYPE_TEXT:
			pWndControl = new WNDCTRL_TEXTBOX;
			break;	

		case WTYPE_EDITCTRL:
			pWndControl = new WNDCTRL_EDITBOX;
			break;	
			
		case WTYPE_LISTBOX:
			pWndControl = new WNDCTRL_LISTBOX;
			break;	
			
		case WTYPE_LISTCTRL:
			pWndControl = new WNDCTRL_LISTCONTROL;
			break;	
			
		case WTYPE_COMBOBOX:
			pWndControl = new WNDCTRL_COMBOBOX;
			break;	

		case WTYPE_TABCTRL:
			pWndControl = new WNDCTRL_TABCONTROL;
			break;	

		case WTYPE_TREECTRL:
			pWndControl = new WNDCTRL_TREECONTROL;
			break;	

		case WTYPE_CUSTOM:
			pWndControl = new WNDCTRL_CUSTOMCONTROL;
			break;	

		default:
			FLERROR_ASSERT_LOG_DO( NULL, break, _T("[컨트롤 팩토리]알수 없는 컨트롤 타입 입니다 - TYPE : [%u]"), dwWndType );
	}

	CString strDescription( _T("") );

	FLERROR_ASSERT_LOG_RETURN( pWndControl, strDescription, _T("[컨트롤 팩토리]올바르지 않은 컨트롤 타입 입니다.") );		
	strDescription = pWndControl->strDescription;
	SAFE_DELETE(pWndControl)

	return strDescription;
}
