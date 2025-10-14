#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define	_WIN32_WINNT	0x0501
#define	WINVER			0x0501

#if	defined( _DLL ) && defined( _MT )
	#define	_AFXDLL
#endif

#define	__LOGINSERVER



struct vector3 {
	float x; float y; float z;
	vector3(const float&& xx, const float&& yy, const float&&zz) { x = xx; y = yy; z = zz; }
};



#include <afxwin.h>

#include <afxdisp.h>        // MFC Automation classes

#include <mmsystem.h>

#include <map>
#include <list>
#include <vector>
#include <string>

//#define	FLASSERT_ENABLE
//#define	FLASSERT_HOOKER_ENABLE

#ifndef	_DEBUG
#define	__FLLOG_EXCLUDE_DEBUG_TRACE
#endif

extern	TCHAR	PROGRAM_NAME[128];
extern	TCHAR	NETWORK_LOG_NAME[128];

#include <common/FLStringFunction.h>
#include <thread/FLCriticalSection.h>
#include <thread/FLAutoLock.h>
#include <logging/FLLogFacade.h>
#include <network/FLNetworkCommon.h>

#include "VersionCommon.h"
#include "../all/Dekane.hpp"
#include <DefineCommon.h>
#include <CmnHdr.h>

#include "..\_CommonDefine\FLCommonLength.h"
#include "../_CommonDefine/RefectoringDefine.h"
#include "..\_CommonDefine\FLFlyffConfig.h"
#include "..\_CommonDefine\FlyffCommonDefine.h"
#include "..\_CommonDefine\FLPacketStruct.h"
#include "..\_CommonDefine\FLReturnStateCode.h"

#include <DXUtil.h>
#include <Data.h>
#include <file.h>
#include <Scanner.h>
#include <Timer.h>
#include <vutil.h>
#include <xutil.h>

#include <common/FLTickCount.h>


extern	FLTickCount	g_TickCount;
extern	char	g_szClientBindIP[16];
extern	char	g_lpCoreAddr[16];
extern	char	g_lpDBAddr[16];

//////////////////////////////////////////////////////////////////////////
//	BEGIN100616
//extern  BOOL	g_bNProtectAuth;
//	END100616
//////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
