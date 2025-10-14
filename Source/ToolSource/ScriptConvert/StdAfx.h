// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__76259A06_27AB_48D8_8B34_6E820921B0D5__INCLUDED_)
#define AFX_STDAFX_H__76259A06_27AB_48D8_8B34_6E820921B0D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LANG_MAX 6

#define __DAISY
#define __VMULTILANG
#define _MULTI_LANGUAGE_
#define __CHARNEXT
#define __SCRIPT_TOOL







	
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afxpriv.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

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

#import "c:\program files\common files\system\ado\msado27.tlb" no_namespace rename( "EOF", "A_EOF" ) 

// STL 
#pragma warning(disable:4786)
#include <vector>
#include <list>
#include <map>
#include <string> 
#include <queue>

//#include "Scanner.h"
#include "WndStyle.h"
#include "vutil.h"
#include "xutil.h"
#include "file.h"
#include "Script.h"
#include "ExcelCtrl.h"
#include "Debug.h"

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define __YDEBUG

using namespace std;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__76259A06_27AB_48D8_8B34_6E820921B0D5__INCLUDED_)
