// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__15E804B5_D130_4465_9ADA_F892962B3919__INCLUDED_)
#define AFX_STDAFX_H__15E804B5_D130_4465_9ADA_F892962B3919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning(disable:4786)

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

#define __RESOURCE_TOOL
#define __EXPEXPAND1104
#define _MULTI_LANGUAGE_
//#define __SERVER
#define __CLIENT

#define	__SECURITY_0628

#ifndef D3DVECTOR_DEFINED
typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

//#include "data.h"
#include "file.h"
#include "vutil.h"
#include "scanner.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__15E804B5_D130_4465_9ADA_F892962B3919__INCLUDED_)
