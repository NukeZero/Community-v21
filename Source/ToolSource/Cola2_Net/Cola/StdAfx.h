// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__62EF877B_D6C5_4541_B5DA_F06125586A92__INCLUDED_)
#define AFX_STDAFX_H__62EF877B_D6C5_4541_B5DA_F06125586A92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// STL 
//#pragma warning(disable:4786)
#include <vector>
#include <list>
#include <map>
#include <string> 
#include <queue>
#include <iostream>

using namespace std;


//#include <winsock2.h>

typedef	__int64	EXPINTEGER;
typedef	double	EXPFLOAT;

#include "defineCommon.h"
#include "Version.h"

typedef map< string, void* > CMapStrToPtr;
typedef CMapStrToPtr::value_type MapStrToPtrType;
typedef CMapStrToPtr::iterator MapStrToPtrItor;


#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <stdio.h>
#include <mmsystem.h>

#include "..\_UnhandledException\ExceptionHandler.h"

#include "..\_DirectX\dxutil.h"
#include "..\_DirectX\d3denumeration.h"
#include "..\_DirectX\d3dsettings.h"
#include "..\_DirectX\d3dutil.h"
#include "..\_DirectX\d3dapp.h"
#include "..\_DirectX\d3dfont.h"
#include "..\_DirectX\dsutil.h"

//#include "d3dutil.h"
#include "..\_Common\xuzhu.h"
#include "..\_Common\Project.h"
#include "..\_Common\Model.h"
#include "..\_Common\ModelGlobal.h"
#include "..\_Common\bone.h"
#include "..\_Common\Object3D.h"
#include "..\_Common\ModelObject.h"
#include "..\_Common\Material.h"
#include "..\_Common\xUtil.h"
#include "..\_Common\xUtil3D.h"
#include "..\_Common\Timer.h"
#include "..\_Common\vutil.h"
#include "..\_Common\data.h"
#include "..\_Common\path.h"
#include "..\_Common\script.h"
#include "..\_Common\file.h"

#include "..\_Common\file.h"

#include "..\Resource\define.h"
#include "ProjectCmn.h"

#include "Resource.h"
#include "Tool.h"
#include "Graphic2D.h"
#include "Graphic3D.h"

#include <assert.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__62EF877B_D6C5_4541_B5DA_F06125586A92__INCLUDED_)
