#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_
#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define	_WIN32_WINNT	0x0501
#define	WINVER			0x0501

#if	defined( _DLL ) && defined( _MT )
	#define	_AFXDLL
#endif

#define __WORLDSERVER

#include <afxwin.h>

#include <AFXTEMPL.H>
#include <process.h>
#include <basetsd.h> 

#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <limits>

//#define	FLASSERT_ENABLE
//#define	FLASSERT_HOOKER_ENABLE

#ifndef	_DEBUG
#define	__FLLOG_EXCLUDE_DEBUG_TRACE
#endif

extern	TCHAR	PROGRAM_NAME[128];
extern	TCHAR	NETWORK_LOG_NAME[128];

#include <afxdisp.h>        // MFC Automation classes


#pragma	warning( push )
#pragma warning( disable : 4819 )	
#include <d3d9.h>
#include <dxerr9.h>
#include <dshow.h>
#pragma	warning( pop )


#include <strmif.h>
#include <control.h>


#include <d3dx9math.h>


#include <common/FLStringFunction.h>
#include <thread/FLCriticalSection.h>
#include <thread/FLAutoLock.h>
#include <logging/FLLogFacade.h>
#include <network/FLNetworkCommon.h>


//////////////////////////////////////////////////////////////////////////
//	content
//////////////////////////////////////////////////////////////////////////
class	FLSpecManager;










//////////////////////////////////////////////////////////////////////////

#include "VersionCommon.h"
#include "../all/Dekane.hpp"
#include "..\_CommonDefine\FLCommonLength.h"
#include "../_CommonDefine/RefectoringDefine.h"
#include "..\_CommonDefine\FLResourceDefines.h"
#include "..\_CommonDefine\FLReturnStateCode.h"

#include <DefineCommon.h>
#include <CmnHdr.h>
#include <Authorization.h>
#include <File.h>
#include <MoverMsg.h>

#include "..\_CommonDefine\FLFlyffConfig.h"
#include "..\_CommonDefine\FlyffCommonDefine.h"
#include "..\_CommonDefine\FLPacketStruct.h"
#include "../_CommonDefine/Packet/FLPacketAllDefine.h"
#include "..\_Common\FLContentSwitch.h"
#include "../_Common/FLCommonCheck.h"

#pragma	warning( push )
#pragma warning( disable : 4819 )	
#include <d3dfile.h>
#include <d3dfont.h>
#include <d3dutil.h>
#include <dxutil.h>
#include <vutil.h>
#include <dsutil.h>
#pragma	warning( pop )

#include <Define.h>
#include <DefineAttribute.h>
#include <DefineItemKind.h>
#include <DefineJob.h>
#include <DefineWorld.h>
#include <defineText.h>
#include <defineSound.h>
#include <defineItemGrade.h>
#include <defineItemType.h>

#include <Data.h>
#include <Timer.h>
#include <patch.h>
#include <Path.h>
#include <Scanner.h>
#include <ScriptDialog.h>
#include <Script.h>
#include <Project.h>
#include <landscape.h>
#include <XUtil.h>
#include <XUtil3d.h>


//////////////////////////////////////////////////////////////////////////
// script file manager
#include "../_CommonDefine/FLScriptFileManager.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	spec
//////////////////////////////////////////////////////////////////////////
#include "../_CommonDefine/Spec/FLSpecStruct.h"
#include "../_CommonDefine/Spec/FLSpecManager.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// item container
#include "../_CommonDefine/FLItemContainer.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	item effect
//////////////////////////////////////////////////////////////////////////
#include "../worldserver/FLUseActiveItem.h"
#include "../worldserver/FLUseEffectItem.h"
#include "../worldserver/FLApplyItemEffect.h"
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Item Log
#include "../_CommonDefine/Log/FLItemLogStruct.h"
//////////////////////////////////////////////////////////////////////////


// Model
#include <Model.h>
#include <Bone.h>
#include <Object3d.h>
#include <ModelObject.h>
#include <ModelGlobal.h>
#include <ModelMng.h>

// object
#include <Obj.h>
#include <Ctrl.h>
#include <Item.h>
#include <Mover.h>
#include <Region.h>
//#include <Weather.h>
#include <World.h>
#include <GuildCombat1to1.h>
#include "User.h"

// ai
#include "..\_AIInterface\AIInterface.h"
#include "..\_AIInterface\AIMonster.h"
#include "..\_AIInterface\Action.h"
#include "..\_AIInterface\ActionMover.h"

#include <CreateObj.h>
#include <common/FLTickCount.h>
#include <flyffevent.h>
#include <buyinginfo.h>

#include "npchecker.h"
#include <Tax.h>



#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;
#include "DPCoreClient.h"
extern	CDPCoreClient	g_DPCoreClient;




//////////////////////////////////////////////////////////////////////////
//	contents
//////////////////////////////////////////////////////////////////////////

//	장바구니 (17차)
#include "FLItemShopping.h"


//////////////////////////////////////////////////////////////////////////
extern	CProject	prj;
extern	FLTickCount	g_TickCount;
extern	u_long		g_uIdofMulti;
extern	CGameTimer	g_GameTimer;
extern	CFlyffEvent	g_eLocal;
extern	ADDSMMODE	g_AddSMMode;
extern	ULONG		g_uKey;
extern	DWORD		g_tmCurrent;		// 전역으로 사용할 현재 시간.
extern	int			g_nProcessCnt;	// 프로세스마다 하나씩 카운트 되는 변수.
extern	int			g_nRenderCnt;	// 렌더화면때마다 하나씩 카운트 되는 변수

#include <GuildCombat1to1.h>
extern CGuildCombat1to1Mng	g_GuildCombat1to1Mng;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
