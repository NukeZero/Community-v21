// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__70861DCB_BAB0_422F_907D_A15B2872A2C2__INCLUDED_)
#define AFX_STDAFX_H__70861DCB_BAB0_422F_907D_A15B2872A2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//define compiler
//#define __VS6
#define __VS2003

// #pragma warning(disable:4786)
// 
// #ifdef __VS2003
// #pragma warning(disable:4244)		//gmpbigsun : for VS2003
// #pragma warning(disable:4018)		//gmpbigsun : for VS2003
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

#ifdef DIR_SFX
	#define DIR_SFX     _T( "SFX\\"  )
#endif

#define __SFXTEX


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

// STL 
#include <vector>
#include <list>	
#include <map>
#include <string> 
#include <queue>
#include <set>

using namespace std;

typedef CMap<DWORD, DWORD, void *, void *> CMapDWordToPtr;  
typedef map< string, void* > CMapStrToPtr;
typedef CMapStrToPtr::value_type MapStrToPtrType;
typedef CMapStrToPtr::iterator MapStrToPtrItor;

//assert
#include <assert.h>

#include "Resource.h"    

#include <D3D9.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dshow.h>
#include "..\_DirectX\dxutil.h"
#include "..\_DirectX\d3denumeration.h" 
#include "..\_DirectX\d3dsettings.h"
#include "..\_DirectX\d3dapp.h" 
#include "..\_DirectX\d3dfont.h"
#include "..\_DirectX\d3dutil.h"
#include "..\_Common\file.h"
#include "..\_Common\vutil.h"
#include "..\_DirectX\DSUtil.h"
#include "..\_DirectX\d3dUtility.h"


#include "..\resource\define.h" // 이걸 인크루드하는건 좋지 않다. --;
#include "..\resource\defineNeuz.h" // 이걸 인크루드하는건 좋지 않다. --;
#include "..\_Common\path.h"
#include "..\_Common\scanner.h"
#include "..\_Common\script.h"
#include "..\_Common\xUtil.h"
#include "..\_Common\xUtil3d.h"
#include "..\_Common\timer.h"
#include "..\_Common\2DRender.h"
#include "..\_Common\graphic3d.h"
#include "..\_Common\model.h"
#include "..\_Common\modelobject.h"
#include "..\_Common\object3d.h"
#include "..\_Common\sfxbase.h"
#include "..\_Common\Light.h"
#include "../_Common/material.h"
//#include "weather.h"
#include "..\_Common\SkyBox.h"

// 오브젝트와 이펙트 객체의 매니져 
#include "SfxMgr.h"
#include "ObjMgr.h"
#include "ObjControl.h"
#include "..\_Common\terrain.h"


#include "MirandaDoc.h"
#include "MirandaView.h"
#include "MainFrm.h"
#include <afxdhtml.h>
//#include "ValueChange.h"
//#include "ModelSettingDlg.h"



extern D3DXMATRIX	g_matView;
extern D3DXMATRIX	g_matInvView;
extern CGameTimer	g_GameTimer;
extern int			g_bMultiMode;
extern BOOL			g_bWireframe;


#define MAIN_TIMER				1
#define RENDER_TIMER			2

// 플레이 모드 
#define		DEPLOY_MODE			0
#define		SIMULATION_MODE		1

// 카메라 모드 
#define		FOCUS_MODE			1
#define		EXPLORE_MODE		2

// 단축키 관련 
#define		HOTKEY_NONE			0

#define		TYPE_OBJECT			1
#define		TYPE_SFX			2

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__70861DCB_BAB0_422F_907D_A15B2872A2C2__INCLUDED_)
