#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_
#pragma once 
#define POINTER_64 __ptr64

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#define NTDDI_VERSION 0x05010000

#if	defined( _DLL ) && defined( _MT )
	#define	_AFXDLL
#endif

#define		__CLIENT
#define		_HAS_ITERATOR_DEBUGGING 0
   
// Windows or Standard        
#include <afxwin.h>      
#include <AFXTEMPL.H>        // MFC Automation classes  
#ifdef _xtrap
#include <xdebug>   
#endif
#include <winuser.h>    
#include <basetsd.h>  
#include <memory.h>  
#include <mmsystem.h>   
#include <string>   
#include <string.h> 
#include <stdio.h>  
#include <tchar.h>  
#include <time.h> 
#include <stdlib.h>
#include <process.h>

#include <afxdisp.h>        // MFC Automation classes

// STL 
#include <vector>
#include <list>
#include <map>
#include <string> 
#include <queue>

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

// exception handler관련 

#include "NeuzVersion.h"  
#include "VersionCommon.h"
#include "../all/Dekane.hpp"

#include "../_CommonDefine/FLCommonLength.h"
#include "../_CommonDefine/RefectoringDefine.h"
#include "../_CommonDefine/FLResourceDefines.h"
#include "../_CommonDefine/FLReturnStateCode.h"

#include "DefineCommon.h"
#include "Resource.h"    
#include "authorization.h" 

#pragma	warning( push )
#pragma warning( disable : 4819 )
// DirectX or 3D Core
#include <D3D9.h>
#include <dsound.h>
#include <dshow.h>

#include "dxutil.h"
#include "d3denumeration.h" 
#include "d3dsettings.h"
#include "d3dapp.h" 
#include "vutil.h"
#include "d3dfont.h"
#include "d3dutil.h"
#include "DSUtil.h"
#include "d3dx9math.h"
#pragma	warning( pop )

// _Network
#include "CmnHdr.h"
#include "ErrorCode.h"
#include "MsgHdr.h"
#include "tools.h"

#include "File.h"

#include "../_Interface/AppDefine.h"
#include "../_Network/Misc/Include/Misc.h"

// Resource
#include <define.h>
#include <defineAttribute.h>
#include <defineItemkind.h>
#include <defineEventArena.h>
#include <defineItemType.h>
#include <defineItemGrade.h>
#include <defineJob.h>
#include <defineWorld.h>
#include <defineText.h>
#include <defineSound.h>
#include <WndStyle.h>
#include <ResData.h>

#include "../_CommonDefine/FLTm.h"
#include "../_CommonDefine/FLFlyffConfig.h"
#include "../_CommonDefine/FLFileCompare.h"
#include "../_CommonDefine/FLCommonCount.h"
#include "../_CommonDefine/FLMakeFileData.h"
#include "../_CommonDefine/FLPacketStruct.h"
#include "../_CommonDefine/FlyffCommonDefine.h"

#include "../_CommonDefine/UTIL/FLKeyGenerator.h"

#include "../_CommonDefine/Packet/FLPacketAllDefine.h"
#include "../_CommonDefine/Packet/FLPacketItemUpgrade.h"

#include "../_UnhandledException/ExceptionHandler.h"
#include "FLContentSwitch.h"
// MultiMedia
#include <atlbase.h> 
// Common
#include "Data.h"
#include "Timer.h"
#include "Scanner.h"
#include "Script.h"
#include "2DRender.h"
#include "TextureManager.h"
#include "SkyBox.h"
#include "patch.h"
#include "Project.h"
#include "path.h"
#include "landscape.h"
#include "xUtil3D.h"
#include "Graphic3D.h"
#include "Music.h"
#include "HwOption.h"
#include "yUtil.h"


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

#include "../Neuz/FLApplyItemEffect.h"

#include "TexturePool.h"	//gmpbigsun

// Model
#include "Model.h"
#include "Billboard.h"
#include "Material.h"
#include "Bone.h"
#include "Object3D.h"
#include "ModelObject.h"
#include "ModelMng.h"
#include "Particle.h"

// object
#include "Obj.h"
#include "Ctrl.h"
#include "Item.h"
#include "sfxbase.h"
#include "Mover.h"   
#include "Region.h"
//#include "Weather.h"
#include "Camera.h"
#include "Light.h"
#include "World.h"
#include "WorldMng.h"

#include "CreateObj.h"
#include "authorization.h"

// Interface
#include "EditString.h"      // CString을 확장한 CEditString
#include "ToolTip.h"         // 툴팁 
#include "ITheme.h"          // 인터페이스의 색상, 스킨, 폰트 따위의 정의 클레스 
#include "WndBase.h"         // 윈도의 기본 클래스, 기타 각종 콘트롤 (버튼, 스크롤바 등등) 
#include "WndControl.h"      // 윈도 각종 콘트롤 (버튼, 스크롤바 등등)
#include "wndEditCtrl.h"     // 에디트 콘트롤, IME, 문자 입력
#include "WndItemCtrl.h"     // 윈도 각종 콘트롤 (버튼, 스크롤바 등등)
#include "WndTradeCtrl.h"     // 윈도 각종 콘트롤 (버튼, 스크롤바 등등)
#include "WndNeuz.h"         // Neuz 형태의 윈도 출력 기본 클래스 
#include "WndMessageBox.h"   // 매시지 박스, 경고, 종료 등등 
#include "WndGuideSystem.h"
#include "WndTitle.h"        // Title의 윈도 클래스 (Login, Character List 등등)
#include "WndTaskBar.h"      // 태스크 바 클래스 관련, 매뉴 등등 
#include "WndField.h"        // Field의 윈도 클래스 (상태, 인벤토리 등등)
#include "WndCommunication.h"// Field의 윈도 클래스 (상태, 인벤토리 등등)
#include "WndOption.h"       // Option 관련 
#include "WndOptionGame.h"	 // 게임 옵션
#include "WndDialog.h"       // 대화  
#include "WndShop.h"         // 상점 
#include "WndMessengerSearch.h" // 메신저 검색창
#include "WndManager.h"      // 윈도 매니저 
#include "WndWorld.h"        // 월드 윈도 
#include "WndGuild.h"			// 길드 윈도
#include "WndRankGuild.h"		// 길드 랭킹
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildVote.h"         // 길드투표 윈도
#include "WndUpgradeBase.h"         // 재련윈도 

#include "FLColorManager.h"
#include "WndFontColorManager.h"

#include "FLShortcutKeyManager.h"


#ifdef __YCLOTH
#include "cloth.h"
#endif

// Application
#include "DialogMsg.h"
#include "Neuz.h"
#include <common/FLTickCount.h>
#include <common/FLStdFile.h>
#include "flyffevent.h"

// console
#ifdef __BS_CONSOLE
#include "Console.h"
#endif

#ifdef __BS_EFFECT_LUA
//#include "BsCommon.h"
#include "../_CommonDefine/FLDeleter.h"
#endif

#include "GlobalTickManager.h"		//gmpbigsun( 20100713 )

#include "../Neuz/DPClient.h"

#define D3DDEVICE   g_Neuz.m_pd3dDevice


extern BYTE             g_bKeyTable[256]; // 키 입력 테이블 
extern BOOL             g_bSlotSwitchAboutEquipItem[ MAX_SLOT_ITEM ];
extern CNeuzApp         g_Neuz        ; // 어플리케이션 클래스, DX3D 프레임 워크 
extern CToolTip         g_toolTip     ; // 툴팁 출력 
extern CToolTip         g_toolTipSub1 ;
extern CToolTip         g_toolTipSub2 ;
extern CMover*          g_pPlayer     ; // 플레이어 객체 
extern CWorldMng	    g_WorldMng    ;
extern CProject         prj;
extern CModelObject*    g_pBipedMesh;
extern CD3DApplication* g_pD3dApp;
extern CGameTimer       g_GameTimer;
extern CDPClient		g_DPlay;


struct CULLINFO
{
    D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
    D3DXPLANE planeFrustum[6];    // planes of the view frustum
};
extern CULLINFO     g_cullinfo;
extern FLTickCount	g_TickCount;
extern CFlyffEvent	g_eLocal; 
extern ADDSMMODE	g_AddSMMode;
extern DWORD		g_tmCurrent;	// 전역으로 사용할 현재 시간.
extern int			g_nRenderCnt;	// 렌더화면때마다 하나씩 카운트 되는 변수
extern int			g_nProcessCnt;	// 프로세스마다 하나씩 카운트 되는 변수.

#include "GuildCombat1to1.h"						//sun: 11, 일대일 길드 대전
extern CGuildCombat1to1Mng g_GuildCombat1to1Mng;

#ifdef __BS_CONSOLE
extern DlgConsole g_Console;
#endif
extern std::vector< CString > g_vecEncryptedValidCertifierIP;

#include <crypto/FLRijndael.h>
#include <crypto/FLMD5.h>

inline int (WINAPIV* __snprintf)(char* buffer, size_t sizeOfBuffer, size_t count, const char* format ...) = _snprintf_s;
inline int (WINAPIV* __vsnprintf)(char*, size_t, const char*, va_list) = _vsnprintf;
inline int (WINAPIV* __vsnwprintf)(wchar_t*, size_t, const wchar_t*, va_list) = _vsnwprintf;


#endif 
