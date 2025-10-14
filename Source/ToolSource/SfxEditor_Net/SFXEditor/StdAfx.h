#if !defined(AFX_STDAFX_H__0DA107B0_C44E_4315_8DBB_ED200E7FD279__INCLUDED_)
#define AFX_STDAFX_H__0DA107B0_C44E_4315_8DBB_ED200E7FD279__INCLUDED_
#pragma once

//#define __VS6
#define __VS2003

// #pragma warning(disable:4786)
// 
// #ifdef __VS2003
// #pragma warning(disable:4244)
// #pragma warning(disable:4018)
// #endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define		__XVS
#define		__XCOLL
#define		__XCOLL2
#define		__XPICK
#define		__X2LINK
#define		__XSLIDE
#define     __AXYZ
#define     __AALLSELECT
#define     __ACENTERMODEL
#define     __AVALUEAPPLEY
#define     __ATEST
#define     __ADROPFILE
#define     __XTEXTURE_EX

// 종료시 저장묻기 기능 

#define NEW_FILE	1
#define LOAD_FILE	2

#ifdef DIR_SFX
	#define DIR_SFX     _T( "SFX\\"  )
#endif

#define __SFXTEX
#define __Y0202		//gmpbigsun(2011_0124)

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define __NONE_CUSTOM_CURSOR

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

// STL 
#include <vector>
#include <list>
#include <map>
#include <string> 
#include <queue>

using namespace std;

typedef map< string, void* > CMapStrToPtr;
typedef CMapStrToPtr::value_type MapStrToPtrType;
typedef CMapStrToPtr::iterator MapStrToPtrItor;

#include "Resource.h"    

#include <D3D9.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dshow.h>
#include "dxutil.h"
#include "d3denumeration.h" 
#include "d3dsettings.h"
#include "d3dapp.h" 
#include "d3dfont.h"
#include "d3dutil.h"
#include "file.h"
#include "vutil.h"
#include "DSUtil.h"


#include "..\resource\define.h" // 이걸 인크루드하는건 좋지 않다. --;
#include "..\resource\defineNeuz.h" // 이걸 인크루드하는건 좋지 않다. --;
#include "path.h"
#include "scanner.h"
#include "script.h"
#include "xUtil.h"
#include "xUtil3d.h"
#include "timer.h"
#include "2DRender.h"
#include "graphic3d.h"
#include "model.h"
#include "modelobject.h"
#include "object3d.h"

#include "sfxbase.h"
#include "SFXEditorDoc.h"
#include "SFXEditorView.h"
#include "ValueChange.h"
#include "ModelSettingDlg.h"

extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matInvView;

int (WINAPIV* __vsnprintf)(char*, size_t, const char*, va_list) = _vsnprintf;
int (WINAPIV* __vsnwprintf)(wchar_t*, size_t, const wchar_t*, va_list) = _vsnwprintf;

#endif 
