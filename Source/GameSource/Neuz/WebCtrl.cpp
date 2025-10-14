// WebCtrl.cpp: implementation of the CWebCtrl class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CNTREVNT.H"
#include "WebCtrl.h"

#define WININC
#include <EXDISPID.H>
#include <ATLBASE.H> 
#include <ATLCONV.H>

//#include <afxinet.h>
#include <Mshtml.h>

#include "webbox.h"
#include "HelperWebBox.h"


#include "WndHelperWebBox.h"
#define THIS_PROP   "this"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWebContainer::CWebContainer( HWND hwnd )
{
   m_cRef = 0;
   m_hWnd = hwnd;
   m_punk = NULL;

   SetRectEmpty( &m_rect );

   m_pEvent = new CWebEventSink();

   m_pEvent->AddRef();
}

CWebContainer::~CWebContainer()
{
   m_pEvent->Release();

   m_punk->Release();
}

void CWebContainer::add()
{
   CLSID clsid;

   CLSIDFromString( L"Shell.Explorer", &clsid );

   CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (PVOID *) &m_punk );

   IOleObject *pioo;

   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   pioo->SetClientSite( this );

   pioo->Release();

   IPersistStreamInit *ppsi;

   m_punk->QueryInterface( IID_IPersistStreamInit, (PVOID *) &ppsi );

   ppsi->InitNew();
   ppsi->Release();

   ConnectEvents();

   setVisible( true );

   setFocus( true );
}

void CWebContainer::remove()
{
   IOleObject *pioo;
   
   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   pioo->Close( OLECLOSE_NOSAVE );

   pioo->SetClientSite( NULL );

   pioo->Release();

   IOleInPlaceObject *pipo;

   m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

   pipo->UIDeactivate();

   pipo->InPlaceDeactivate();

   pipo->Release();

   DisconnectEvents();
}

void CWebContainer::setLocation( int x, int y, int cx, int cy )
{
   m_rect.left   = x;
   m_rect.top    = y;
   m_rect.right  = cx;
   m_rect.bottom = cy;

   IOleInPlaceObject *pipo;

   m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

   pipo->SetObjectRects( &m_rect, &m_rect );

   pipo->Release();
}

void CWebContainer::setVisible( bool bVisible )
{
   IOleObject *pioo;

   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   if ( bVisible )
   {
      pioo->DoVerb( OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_hWnd, &m_rect );
      pioo->DoVerb( OLEIVERB_SHOW, NULL, this, 0, m_hWnd, &m_rect );
   }
   else
      pioo->DoVerb( OLEIVERB_HIDE, NULL, this, 0, m_hWnd, NULL );

   pioo->Release();
}

void CWebContainer::setFocus( bool bFocus )
{
   IOleObject *pioo;

   if ( bFocus )
   {
      m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

      pioo->DoVerb( OLEIVERB_UIACTIVATE, NULL, this, 0, m_hWnd, &m_rect );

      pioo->Release();
   }
}

void CWebContainer::ConnectEvents()
{
   IConnectionPoint *pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

   pcp->Advise( m_pEvent, &m_eventCookie );

   pcp->Release();
}

void CWebContainer::DisconnectEvents()
{
   IConnectionPoint *pcp;

   pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

   pcp->Unadvise( m_eventCookie );

   pcp->Release();
}

STDMETHODIMP CWebEventSink::Invoke(DISPID dispidMember, REFIID riid,
                                LCID lcid, WORD wFlags,
                                DISPPARAMS* pDispParams,
                                VARIANT* pvarResult,
                                EXCEPINFO*  pExcepInfo, UINT* puArgErr)
{
   USES_CONVERSION;
   
   if (!pDispParams)
      return E_INVALIDARG;

   switch (dispidMember)
   {
      // The parameters for this DISPID are as follows:
      // [0]: Cancel flag  - VT_BYREF|VT_BOOL
      // [1]: HTTP headers - VT_BYREF|VT_VARIANT
      // [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT 
      // [3]: Target frame name - VT_BYREF|VT_VARIANT 
      // [4]: Option flags - VT_BYREF|VT_VARIANT
      // [5]: URL to navigate to - VT_BYREF|VT_VARIANT
      // [6]: An object that evaluates to the top-level or frame
      //      WebBrowser object corresponding to the event. 
      //
      case DISPID_BEFORENAVIGATE2:
         if (pDispParams->cArgs >= 5 &&
             pDispParams->rgvarg[5].vt == (VT_BYREF|VT_VARIANT))
         {
         /*   CComVariant varURL(*pDispParams->rgvarg[5].pvarVal);
            varURL.ChangeType(VT_BSTR);
			
			//OLE2T(varURL.bstrVal);

			char TempChar[256];


			
			//strcpy(TempChar, (char*)(LPSTR)OLE2T(varURL.bstrVal) );

			char temp[120];
			mem_set(temp, 0, 120);
			memcpy(temp, varURL.bstrVal, 60);
			for(int  i = 1; i < 60; i++)
				temp[ i ] = temp[ i * 2];
			int tempsize = strlen(temp);
            mem_set(&temp[ tempsize], 0, 120 - tempsize);

			
			if( !strcmp(TempChar, "loading_start.htm") )
				//g_MainGame->m_GameData->m_bLoadingView = false;*/
         }
         break;

      // The parameters for this DISPID:
      // [0]: URL navigated to - VT_BYREF|VT_VARIANT
      // [1]: An object that evaluates to the top-level or frame
      //      WebBrowser object corresponding to the event. 
      //
		case DISPID_NAVIGATECOMPLETE2:
			{
				if(g_WndMng.GetWndBase(APP_WEBBOX))
					g_WndMng.GetWndBase(APP_WEBBOX)->SetVisible(TRUE);
				if(g_WndMng.GetWndBase(APP_WEBBOX2))
						g_WndMng.GetWndBase(APP_WEBBOX2)->SetVisible(TRUE);
			
				ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_SHOW );
				ShowWindow( CHelperWebBox::GetInstance()->GetDialogHandle(), SW_SHOW );

				//if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
				//{
				//	CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
				//	varURL.ChangeType(VT_BSTR);
				//	char TempChar[1024];
				//	strcpy(TempChar, (char*)(LPSTR)OLE2T(varURL.bstrVal) );
				//	if( !strcmp(TempChar, "http://119.46.99.241/play/games/ack_token.aspx#_=_") )
				//	{
				//		CWebCtrl *pthis = (CWebCtrl *) GetProp( CHelperWebBox::GetInstance()->GetDialogBoxHandle(), "this" );
				//		if( pthis )
				//		{
				//			//AfxMessageBox( "OpenURL pFile is NULL!!" );
				//			LPDISPATCH pdisp;
				//			pthis->m_pweb->get_Document(&pdisp);
				//			CComQIPtr<IHTMLDocument2> doc = (LPUNKNOWN)pdisp;
				//			this->Release();
				//			CComQIPtr<IHTMLElement> iElementtest;
				//			doc->get_body(&iElementtest);

				//			CComBSTR tmpstr = _T("");
				//			iElementtest->get_outerHTML(&tmpstr);
				//			USES_CONVERSION;
				//			char* htmlcode = OLE2A(tmpstr);
				//		}
				//		
				//	}
				//}
				
				
					
				/*if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
				{
					CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
					varURL.ChangeType(VT_BSTR);
					char TempChar[1024];
					strcpy(TempChar, (char*)(LPSTR)OLE2T(varURL.bstrVal) );
					if( !strcmp(TempChar, "http://119.46.99.241/play/games/ack_token.aspx#_=_") )
					{
						CInternetFile* pFile = NULL;
						CInternetSession InetSession( _T( "PhilippinesAuth" ) );

						try
						{
							pFile = (CInternetFile *)InetSession.OpenURL( TempChar );  
						}
						catch(CInternetException *pEx)
						{
							pFile = NULL;
							pEx = NULL;
						}

						

						if( pFile )
						{
							CString strTemp;
							pFile->SetReadBufferSize( 4096 );

							while( true )
							{
								if( pFile->ReadString( strTemp ) )
								{
									strTemp += "\r\n";
									strData += strTemp;
								}
								else
								{
									break;
								}
							}
						}
						else
						{
							AfxMessageBox( "OpenURL pFile is NULL!!" );
						}
					}
				}*/
				

				/*if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
				{
				CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
				varURL.ChangeType(VT_BSTR);

				char TempChar[512];
				strcpy(TempChar,OLE2A((const unsigned short*)pDispParams->rgvarg[0].pvarVal));

				if( !strcmp(TempChar, "loading_end.htm") )
				g_MainGame->m_GameData->m_bLoadingView = true;
				}*/
				break;
			}
		case DISPID_DOCUMENTCOMPLETE:

			if(g_WndMng.GetWndBase(APP_WEBBOX))
					g_WndMng.GetWndBase(APP_WEBBOX)->SetVisible(TRUE);
			if(g_WndMng.GetWndBase(APP_WEBBOX2))
					g_WndMng.GetWndBase(APP_WEBBOX2)->SetVisible(TRUE);
		
			ShowWindow( CWebBox::GetInstance()->m_hDlg, SW_SHOW );
			ShowWindow( CHelperWebBox::GetInstance()->GetDialogHandle(), SW_SHOW );

			if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
			{
				CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
				varURL.ChangeType(VT_BSTR);
				char TempChar[1024];
				strcpy(TempChar, (char*)(LPSTR)OLE2T(varURL.bstrVal) );
				if( !strcmp(TempChar, "http://119.46.99.241/play/games/ack_token.aspx#_=_") || !strcmp(TempChar, "http://119.46.99.241/play/games/ack_token.aspx#") 
					|| !strcmp(TempChar, "https://secure6.playpark.com/play/games/ack_token.aspx#_=_" ) || !strcmp(TempChar, "https://secure6.playpark.com/play/games/ack_token.aspx#" ))
				{
					CWebCtrl *pthis = (CWebCtrl *) GetProp( CHelperWebBox::GetInstance()->GetDialogBoxHandle(), "this" );
					if( pthis )
					{
						//AfxMessageBox( "OpenURL pFile is NULL!!" );
						/*LPDISPATCH pdisp;
						pthis->m_pweb->get_Document(&pdisp);
						CComQIPtr<IHTMLDocument2> doc = (LPUNKNOWN)pdisp;
						this->Release();
						CComQIPtr<IHTMLElement> iElementtest;
						doc->get_body(&iElementtest);

						CComBSTR tmpstr = _T("");
						iElementtest->get_outerHTML(&tmpstr);
						USES_CONVERSION;
						char* htmlcode = OLE2A(tmpstr);*/

						//LPDISPATCH pdisp;
						//pthis->m_pweb->get_Document(&pdisp);
						//CComQIPtr<IHTMLDocument2> doc = (LPUNKNOWN)pdisp;
						//this->Release();
						//CComQIPtr<IHTMLElement> iElementtest;

						///*BSTR bstr = SysAllocString(OLESTR("Written by IHTMLDocument2::write()."));
						//SAFEARRAY *psaStrings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
						//VARIANT *param;
						//HRESULT hr = SafeArrayAccessData(psaStrings, (LPVOID*)&param);
						//param->vt = VT_BSTR;
						//param->bstrVal = bstr;
						//hr = SafeArrayUnaccessData(psaStrings);
						//doc->write(psaStrings);*/

						//doc->get_body(&iElementtest);

						//CComBSTR tmpstr = _T("");
						//iElementtest->get_outerHTML(&tmpstr);
						///*iElementtest->get_outerText(&tmpstr);
						//iElementtest->get_title(&tmpstr);
						//iElementtest->get_innerHTML(&tmpstr);
						//iElementtest->get_innerText(&tmpstr);*/
						//USES_CONVERSION;
						//char* htmlcode = OLE2A(tmpstr);

						/*IHTMLDocument2 * pDoc = NULL;
						IDispatch * pDisp = NULL;
						IPersistStreamInit * pInit = NULL;
						IStream * pStream = NULL;
						HRESULT hr = pthis->m_pweb->get_Document(&pDisp);
						if ( SUCCEEDED(hr) && pDisp )
						{
							hr = pDisp->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pDoc);
							if ( SUCCEEDED(hr) && pDoc )
							{
								hr = pDoc->QueryInterface(IID_IPersistStreamInit, (void**)&pInit);
								if (SUCCEEDED(hr) && pInit )
								{
									hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
									if ( SUCCEEDED(hr) && pStream )
									{
										pInit->Save(pStream, FALSE);
									}
								}
							}
						}*/

						IHTMLDocument2 * pDoc = NULL;

						IStream * pStream = NULL;
						IPersistStreamInit * pPSI = NULL;

						HGLOBAL hHTMLContent;
						IDispatch * pDisp = NULL;
						HRESULT hr = pthis->m_pweb->get_Document(&pDisp);;
						bool bResult = false;
						char buff[100]; // 여기다 소스를 담는다

						 
						hr = pDisp->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pDoc);
						if (SUCCEEDED(hr))
						{
							hr = ::CreateStreamOnHGlobal(NULL, TRUE, &pStream);
							if (SUCCEEDED(hr))
							{
								// request the IPersistStreamInit interface
								hr = pDoc->QueryInterface(IID_IPersistStreamInit, (void **) &pPSI);
								if (SUCCEEDED(hr))
								{
									hr = pPSI->Save(pStream, FALSE);
									if (SUCCEEDED(hr))
									{
										hr = ::GetHGlobalFromStream(pStream, &hHTMLContent);
										if (SUCCEEDED(hr))
										{
											bResult = true;
											LPVOID data = ::GlobalLock(hHTMLContent);
											DWORD size = ::GlobalSize(hHTMLContent);
											if ( size >= 100 )
												break;
											::memcpy(buff, data, size);
											

											char* ptr = strtok ( buff, " " );
											if ( ptr == NULL )
												break;
											ptr = strtok ( NULL, " ");
											if ( ptr == NULL )
												break;
											char buff2[100];
											strcpy(buff2, ptr);
											char result[3][100];
											int nCount = 0;
											ptr = strtok ( buff2, "|" );
											while ( ptr != NULL )
											{
												if ( nCount >= 3 )
													break;
												strcpy(result[nCount], ptr);
												nCount++;
												ptr = strtok ( NULL, "|");
											}


											g_Neuz.SetAccountInfo( result[1], result[2], "", false );
											::GlobalUnlock(hHTMLContent);
										}
									}
									pPSI->Release();
								}
								// implicitly calls ::GlobalFree to free the global memory
								pStream->Release();
							}
						}




					}
					/*if(g_WndMng.GetWndBase(APP_WEBBOX2))
						g_WndMng.GetWndBase(APP_WEBBOX2)->SetVisible(FALSE);
					ShowWindow( CHelperWebBox::GetInstance()->GetDialogHandle(), SW_HIDE );*/

					CWndLogin* pWndLogin = ( CWndLogin* )g_WndMng.GetWndBase( APP_LOGIN );
					if(pWndLogin)
					{
						pWndLogin->Login();
					}

					CWndHelperWebBox* pWndHelperWebBox = ( CWndHelperWebBox* )g_WndMng.GetApplet( APP_WEBBOX2 );
					if( pWndHelperWebBox )
						pWndHelperWebBox->Destroy();
				}
			}
			/*if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))
			{
				CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);
				varURL.ChangeType(VT_BSTR);
				char TempChar[1024];
				strcpy(TempChar, (char*)(LPSTR)OLE2T(varURL.bstrVal) );
				if( !strcmp(TempChar, "http://119.46.99.241/play/games/ack_token.aspx#_=_") )
				{
					CInternetFile* pFile = NULL;
					CInternetSession InetSession( _T( "PhilippinesAuth" ) );

					try
					{
						pFile = (CInternetFile *)InetSession.OpenURL( TempChar );  
					}
					catch(CInternetException *pEx)
					{
						pFile = NULL;
						pEx = NULL;
					}

					

					if( pFile )
					{
						CString strTemp;
						pFile->SetReadBufferSize( 4096 );

						while( true )
						{
							if( pFile->ReadString( strTemp ) )
							{
								strTemp += "\r\n";
								strData += strTemp;
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						AfxMessageBox( "OpenURL pFile is NULL!!" );
					}
				}

				
			}*/
			break;
      default:
         break;
   }

    return S_OK;
}

IConnectionPoint* CWebContainer::GetConnectionPoint( REFIID riid )
{
   IConnectionPointContainer *pcpc;
   
   m_punk->QueryInterface( IID_IConnectionPointContainer, (PVOID *) &pcpc );

   IConnectionPoint *pcp;

   pcpc->FindConnectionPoint( riid, &pcp );

   pcpc->Release();

   return pcp;
}

HRESULT CWebContainer::getWeb( IWebBrowser2 **ppweb )
{
   return m_punk->QueryInterface( IID_IWebBrowser2, (PVOID *) ppweb );
}

CWebCtrl::CWebCtrl( HWND hwnd )
{
   m_hWnd = hwnd;
   m_cxScroll = GetSystemMetrics( SM_CXHSCROLL ) + 2;
   m_cyScroll = GetSystemMetrics( SM_CYVSCROLL ) + 2;
       
   m_cxWidth = 0;
   m_cxHeight = 0;
}



static LRESULT CALLBACK WebCtrlProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   CWebCtrl *pthis = (CWebCtrl *) GetProp( hwnd, THIS_PROP );

   if ( !pthis )
   {
      if ( uMsg != WM_CREATE )
         return DefWindowProc( hwnd, uMsg, wParam, lParam );

      pthis = new CWebCtrl( hwnd );

      SetProp( hwnd, THIS_PROP, (HANDLE) pthis );
   }

   LRESULT lResult = pthis->WebCtrlProc( uMsg, wParam, lParam );

   if ( uMsg == WM_DESTROY )
   {
      RemoveProp( hwnd, THIS_PROP );
	  SAFE_DELETE( pthis );
   }

   return lResult;
}

void CWebCtrl::Navigate( char* szURL, char* szPostData, char* szHeaders )
{
	VARIANT *pvarURL, *pvarPostData, *pvarHeaders;
	VARIANT varURL, varPostData, varHeaders;

	pvarURL		= NULL;
	pvarPostData	= NULL;
	pvarHeaders		= NULL;

	if( !szURL )
		return;

	if( !*szURL )
		return;

	int len = lstrlen( szURL ) + 1;

	WCHAR *pszW;

#ifndef UNICODE

	pszW	= new WCHAR[len];

	MultiByteToWideCharEx( CP_ACP, 0, szURL, -1, pszW, len );

#else	// UNICODE

	pszW = szURL;

#endif	// UNICODE

	VariantInit( &varURL );

	varURL.vt	= VT_BSTR;
	varURL.bstrVal	= SysAllocString( pszW );

#ifndef UNICODE

	SAFE_DELETE_ARRAY( pszW );

#endif	// UNICODE

	pvarURL	= &varURL;

	len	= lstrlen ( szHeaders );
	if( 0 < len )
	{
		len++;

#ifndef UNICODE

		pszW	= new WCHAR[len];

		MultiByteToWideCharEx( CP_ACP, 0, szHeaders, -1, pszW, len );

#else	// UNICODE

		pszW	= szURL;

#endif	// UNICODE

		VariantInit( &varHeaders );
		varHeaders.vt	= VT_BSTR;
		varHeaders.bstrVal	= SysAllocString( pszW );

#ifndef UNICODE

		SAFE_DELETE_ARRAY( pszW );

#endif	// UNICODE

		pvarHeaders		= &varHeaders;
	}

	len	= lstrlen ( szPostData );
	if( 0 < len )
	{
		VariantInit( &varPostData );

		SAFEARRAYBOUND rgsabound;
		rgsabound.cElements		= len;
		rgsabound.lLbound	= 0;

		varPostData.parray	= ::SafeArrayCreate( VT_UI1, 1, &rgsabound );
		varPostData.vt	= unsigned short( VT_UI1 | VT_ARRAY );

		void* pvDestData;
		::SafeArrayAccessData( varPostData.parray, &pvDestData );
		memcpy( pvDestData, szPostData, len );
		::SafeArrayUnaccessData( varPostData.parray );

		pvarPostData	= &varPostData;
	}

	m_pweb->Navigate2( pvarURL, NULL, NULL, pvarPostData, pvarHeaders );

	VariantClear( pvarURL );

	if( pvarPostData )
		VariantClear( pvarPostData );

	if (pvarHeaders)
		VariantClear( pvarHeaders );

//	m_pweb->Navigate2( &v, NULL, NULL, NULL, NULL );
}

/*
void CWebCtrl::Navigate( LPTSTR psz )
{
   if ( !psz )
      return;

   if ( !*psz )
      return;

   int len = lstrlen( psz ) + 1;

   WCHAR *pszW;

#ifndef UNICODE

   pszW = new WCHAR[len];

   MultiByteToWideChar( CP_ACP, 0, psz, -1, pszW, len );

#else

   pszW = psz;

#endif

   VARIANT v;

   VariantInit( &v );

   v.vt = VT_BSTR;
   v.bstrVal = SysAllocString( pszW );

#ifndef UNICODE

   delete []pszW;

#endif

   m_pweb->Navigate2( &v, NULL, NULL, NULL, NULL );

   VariantClear( &v );
}
*/
LRESULT CWebCtrl::WebCtrlProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = 0;

	switch ( uMsg )
	{
		case WEB_CTRL_ADDRESS:
			memset(g_address, 0, sizeof( g_address ));
			FLStrcpy(g_address, _countof( g_address ), (char*)lParam);
			break ; 
		case WEB_CTRL_POST:
			memset( g_postdata, 0, sizeof( g_postdata ) );
			FLStrcpy( g_postdata, _countof( g_postdata ), (char*)lParam );
			FLTRACE_LOG( PROGRAM_NAME, _T( "POST: %s" ), g_postdata );
//			{
//				FLTRACE_LOG( PROGRAM_NAME, _T( "POST: %s" ), g_postdata );
//			}
			break;
		case WEB_CTRL_HEADER:
			memset(g_header, 0, sizeof( g_header ));
			FLStrcpy( g_header, _countof( g_header ), (char*)lParam );
			FLTRACE_LOG( PROGRAM_NAME, _T( "HEADER: %s" ), g_header );
//			{
//				FLTRACE_LOG( PROGRAM_NAME, _T( "HEADER: %s" ), g_header );
//			}
			break;
		case WEB_CTRL_NAVIGATE:
			Navigate( g_address, g_postdata, g_header );
			break ; 
		case WEB_CTRL_GO_BACK:
			{
				m_pweb->GoBack();
				break;
			}
		case WEB_CTRL_GO_FORWARD:
			{
				m_pweb->GoForward();
				break;
			}
		case WEB_CTRL_STOP:
			{
				m_pweb->Stop();
				break;
			}
		case WEB_CTRL_REFRESH:
			{
				m_pweb->Refresh();
				break;
			}
		case WM_CREATE:
			m_pContainer = new CWebContainer( m_hWnd );
			m_pContainer->AddRef();
			m_pContainer->add();
			m_pContainer->getWeb( &m_pweb );

			CREATESTRUCT *pcs;
			pcs = (CREATESTRUCT *) lParam;
			Navigate( (LPTSTR) pcs->lpszName );
			break;
		case WM_SETTEXT:
			Navigate( (LPTSTR) lParam );
			break;

		case WM_SIZE:
			if( LOWORD( lParam ) > 0 && LOWORD( lParam ) > 0 )
			{
				m_cxWidth = LOWORD( lParam );
				m_cxHeight = HIWORD( lParam );
			}
			else 
			{
				//FLERROR_LOG( PROGRAM_NAME, _T( "WM_SIZE error, width %d height %d" ), LOWORD( lParam ), HIWORD( lParam ) );
			}

	        m_pContainer->setLocation( -2, -2, m_cxWidth + m_cxScroll, m_cxHeight + m_cyScroll );
//			m_pContainer->setLocation( -2, -2, LOWORD(lParam) + m_cxScroll, HIWORD(lParam) + m_cyScroll );
	        break;

	  case WEB_CTRL_SHOW:
		 m_pContainer->setVisible( true );
		 break;
	  case WEB_CTRL_HIDE:
		 m_pContainer->setVisible( false );
		 break;

	case WM_DESTROY:

		m_pweb->Release();

		m_pContainer->remove();

		m_pContainer->Release();

		break;

	default:

		lResult = DefWindowProc( m_hWnd, uMsg, wParam, lParam );

		break;
	}

	return lResult;
}

void CWebCtrl::ShowScroll( BOOL bShow )
{
	if( bShow )
	{
		m_cxScroll = 0;
		m_cyScroll = 0;
	}
	else 
	{
		m_cxScroll = GetSystemMetrics( SM_CXHSCROLL ) + 2;
		m_cyScroll = GetSystemMetrics( SM_CYVSCROLL ) + 2;
	}

	SendMessage( m_hWnd, WM_SIZE, 0, 0 );
}


bool WINAPI InitWebCtrl()
{
   CoInitialize( NULL );

 

   HINSTANCE hinst = GetModuleHandle( NULL );

   WNDCLASS wc = { 0 };
   if ( !GetClassInfo( hinst, WC_WEBCTRL, &wc ) )
   {
	   
      WNDCLASS wc2 = { 0 };

      wc2.style         = CS_DBLCLKS | CS_GLOBALCLASS | CS_NOCLOSE | CS_OWNDC ;
      wc2.lpfnWndProc   = ::WebCtrlProc;
      wc2.hInstance     = hinst;
      //wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
	  wc2.hCursor       = LoadCursor( hinst, /*MAKEINTRESOURCE(IDC_CURSOR1)*/NULL );
      wc2.lpszClassName = WC_WEBCTRL;

      return RegisterClass( &wc2 ) ? true : false;
   }
   
   return true;
}

bool WINAPI UninitWebCtrl()
{
   WNDCLASS wc;

   bool bResult = false;

   if ( GetClassInfo( GetModuleHandle( NULL ), WC_WEBCTRL, &wc ) )
      bResult = UnregisterClass( WC_WEBCTRL, wc.hInstance ) ? true : false;

   CoUninitialize();

   return bResult;
}

/*
HWND WINAPI CreateWebControl( LPCTSTR pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId )
{
   return CreateWindow( WC_WEBCTRL, pszText, dwStyles, x, y, cx, cy, hwndParent, (HMENU) uId, GetModuleHandle( NULL ), NULL );
}
*/

static class AutoInit
{
public:

   AutoInit()
   {
      InitWebCtrl();
   }

   ~AutoInit()
   {
      UninitWebCtrl();
   }

} a;