
#if !defined(AFX_STDAFX_H__F12B2C76_2E08_4923_852B_453F134B7DEF__INCLUDED_)
#define AFX_STDAFX_H__F12B2C76_2E08_4923_852B_453F134B7DEF__INCLUDED_

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

//define compiler
//#define __VS6
#define __VS2003

// #pragma warning(disable:4786)
// 
// #ifdef __VS2003
// #pragma warning(disable:4244)
// #pragma warning(disable:4018)
// #endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#define __NEWINTERFACE

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>		// MFC Views
#include <AFXTEMPL.H>        // MFC Automation classes  
#include <winuser.h>    
#include <basetsd.h>  
#include <memory.h>  
#include <mmsystem.h>   
#include <string.h>   
#include <stdio.h>  
#include <tchar.h>  
#include <time.h> 
#include <stdlib.h>

typedef CMap<DWORD, DWORD, void *, void *> CMapDWordToPtr;  

#define _MULTI_LANGUAGE_
#define __NEWTAB
#define __VMULTILANG
#define __VAPIFONT
#define __V041111_UPGRADECHAT
#define __VIMEMODIFY
#define __VMULTIIME
#define __V050322_CACHEQUEST
#define __V050913_TABWND
#define	__USE_STR_TBL0407
#define __NEW_UI19					//gmpbigsun( 201111008 ) : 

//#define __FREE_TYPE_FONT			//@@추가 KYT 프리타입 폰트 ( 2012 / 11 / 17 )
//#define  __TOON 



#include <xdebug>   
#include <string.h> 

// STL 
#include <vector>
#include <list>
#include <map>
#include <string> 
#include <queue>

// 라이브러리
#include <Common/FLAssert.h>
#include <Logging/FLLogFacade.h>

using namespace std;

typedef map< string, void* > CMapStrToPtr;
typedef CMapStrToPtr::value_type MapStrToPtrType;
typedef CMapStrToPtr::iterator MapStrToPtrItor;

#include "debug.h"

#include <D3D9.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dshow.h>

#ifdef __FREE_TYPE_FONT
#include <ft2build.h>
#include FT_FREETYPE_H

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftlist.h>
#include <freetype/ftoutln.h>
#include <freetype/fttypes.h>
#endif

#include "dxutil.h"
#include "d3denumeration.h" 
#include "d3dsettings.h"
#include "d3dapp.h" 
#include "d3dfont.h"
#include "d3dutil.h"
#include "DSUtil.h"
#include "vutil.h"

#include "define.h"
#include "defineSound.h"
#include "path.h"
#include "file.h"
#include "scanner.h"
#include "script.h"
#include "xUtil.h"
#include "xUtil3d.h"
#include "timer.h"
#include "2DRender.h"

#include "projectCmn.h"
#include "EditString.h"
#include "ITheme.h"
#include "ResManager.h"
#include "WndBase.h"
#include "WndControl.h"
#include "WndEditCtrl.h"
#include "WndNeuz.h"
#include "WndDialog.h"
#include "file.h"

#include "data.h"
#include "MainFrm.h"
#include "resource.h"

extern CMainFrame*	g_pMainFrame;
extern TCHAR		PROGRAM_NAME[128];
extern TCHAR		g_szWorkDirectory[MAX_PATH];

#define __NONE_CUSTOM_CURSOR

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F12B2C76_2E08_4923_852B_453F134B7DEF__INCLUDED_)
    
