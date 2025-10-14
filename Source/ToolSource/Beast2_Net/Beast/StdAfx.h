#if !defined(AFX_STDAFX_H__411DF022_4A92_4491_AAAE_2DDD0ADAA60B__INCLUDED_)
#define AFX_STDAFX_H__411DF022_4A92_4491_AAAE_2DDD0ADAA60B__INCLUDED_
#pragma once

//#define __VS6
#define __VS2003
#define __BEAST
//#define	_WIN32_WINNT	0x0500 // Beast에서 이거 넣으면 CFileDialog에서 CString 파괴할 때 다운됨
#define WINVER 0x0501

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcview.h>		// MFC Views

#include <vector>
#include <list>
#include <map>
#include <string>
#include <queue>

using namespace std;

typedef map< DWORD, void* > CMapDWordToPtr;
typedef CMapDWordToPtr::value_type MapDWordToPtrType;
typedef CMapDWordToPtr::iterator MapDWordToPtrItor;
typedef map< string, void* > CMapStrToPtr;
typedef CMapStrToPtr::value_type MapStrToPtrType;
typedef CMapStrToPtr::iterator MapStrToPtrItor;
typedef vector< string > CStrArray;
typedef map< string, DWORD > CMapStrToObjId;
typedef CMapStrToObjId :: value_type MapStrToObjIdType;

#define chASSERT( x ) ASSERT( x )

//#define _AddErrorMsg(A)		/##/		// 디버그 모드일땐 무시하게 하자.
//#define __V041130_SKYLINE

//#include "..\neuz\versionCommon.h"

#include "resource.h"
 
#include <windows.h> 
#include <windowsx.h>
#include <basetsd.h> 
#include <memory.h> 
#include <mmsystem.h> 
#include <string.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include "ChkListBox.h"
#include "FileDlgWorld.h"

#include "debug.h"
#include <atlimage.h>


//	local file
#include "BeastHeader.h"

// DirectX3D
#include <dsound.h>
#include <dshow.h>
#include <D3D9.h>
#include "dxutil.h"
#include "d3denumeration.h" 
#include "d3dsettings.h"
#include "d3dapp.h"
//#include "d3dfile.h"
#include "d3dfont.h"
#include "d3dutil.h"
#include "dxutil.h"
#include "dsutil.h"
#include "vutil.h"
#include "xutil3D.h"

// 클라이언트 와 서버에서 공용으로 사용하는 값(constant), 형을 정의하는 파일
#include "definecommon.h"

// RESOURCE
#include "define.h"
#include "defineNeuz.h"
#include "defineAttribute.h"
#include "defineObj.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "defineJob.h"
#include "defineItemKind.h"
#include "lang.h"

#include "CmnHdr.h"
#include "file.h"

// BEAST
#include "Data.h"
#include "Scanner.h"
#include "ScriptDialog.h"
#include "Script.h"
#include "Project.h"
#include "Path.h"
#include "TerrainMng.h"
//#include "SkyBox.h"
#include "Patch.h"
#include "Landscape.h"
#include "2DRender.h"
#include "Graphic3D.h"
//#include "CreateObj.h"
#include "Timer.h"

// model
#include "Model.h"
#include "Billboard.h"
#include "Material.h"
#include "Bone.h"
#include "Object3D.h"
#include "ModelSfx.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "ModelMng.h"
//#include "..\_unhandledexception\ExceptionHandler.h"
/*
#include "Model.h"
#include "ModelMng.h"
#include "Object3D.h"
#include "ModelObject.h"
#include "ModelGlobal.h"
#include "Material.h"
#include "billboard.h"
*/
// object
#include "Obj.h"
#include "CommonCtrl.h"
#include "Ctrl.h"
#include "Item.h"
#include "SfxBase.h"
#include "Sfx.h"
#include "Mover.h" 
#include "Region.h"
#include "ship.h"
#include "Weather.h" 
#include "Camera.h" 
#include "Light.h" 
#include "World.h"
#include "WorldMng.h"
#include "MoverMsg.h"
#include "hwOption.h"
#include "SkillInfluence.h"
#include "beast.h"
#include "MainFrm.h"
#include "WorldView.h"

// AI

#include "..\_AIInterface\AIInterface.h"
//#include "..\_AIInterface\AIMover.h"
//#include "..\_AIInterface\AIMonster.h"

#include "xUtil.h"
#include "xUtil3d.h"
#include "sunUtil.h"

#include "CreateObj.h"
#include "authorization.h"
#include "DlgContinent.h"
#include "DlgLight24.h"
#include <afxdhtml.h>
#include "DlgCalcWorldmap.h"

extern CMainFrame* g_pMainFrame;
extern CWorldView* g_pWorldView;
extern C2DRender   g_2DRender;
extern CWorldMng   g_WorldMng;
extern CProject    prj;
extern COption     g_Option;

extern int		   g_MPU;
extern const int   OLD_MPU;

extern CDlgContinent g_kDlgContinent;
extern CDlgLight24 g_kDlg24Light;
extern CDlgCalcWorldmap g_kDlgCalcWorlmap;
extern const char* DEFAULT_FNAME;

struct CULLINFO
{
    D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
    D3DXPLANE planeFrustum[6];    // planes of the view frustum
};
extern CULLINFO       g_cullinfo;

int (WINAPIV* __vsnprintf)(char*, size_t, const char*, va_list) = _vsnprintf;
int (WINAPIV* __vsnwprintf)(wchar_t*, size_t, const wchar_t*, va_list) = _vsnwprintf;
int (WINAPIV* __snprintf)(char* buffer, size_t sizeOfBuffer, size_t count, const char* format ...) = _snprintf_s;


// override defeine 
#define D3DDEVICE   NULL
#define DLGMESSAGE( pMover, szText, dwColor ) ;

#define FOLDER 0x80000000
#define IS_FOLDER( x ) ( ( x & FOLDER ) ? TRUE : FALSE )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__411DF022_4A92_4491_AAAE_2DDD0ADAA60B__INCLUDED_)
