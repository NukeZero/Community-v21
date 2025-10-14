
#ifndef __BEASTHEADER_H_
#define __BEASTHEADER_H_

#define _MULTI_LANGUAGE_
#define __XAI1013
#define __X1014_PROP
#define __NEWITEM
#define __Y0202
#define __XFXP0521
#define __NEWPROCESS
#define __VMULTILANG
#define __VLANDATTR
#define __LOCATION

#define __N_WATERFRAME_050202
#define __N_TRIGGER_AREASET_050203

//#define __16BITLIGHT

#define __XVS
#define __X2LINK
#define __XMODEL8

#ifdef	__XMODEL8
#define		CBipedMesh		CModelObject
#define		CStaticMesh		CModelObject
#endif

/*
#include <basetsd.h>
#include <tchar.h>
#include <math.h>
#include <D3DX8.h>
#include "D3DApp.h"
#include "D3DUtil.h" 
#include "DXUtil.h"  
#include "D3DFile.h" 
*/
//#define __XUZHU 

#define	__USE_STR_TBL0407

#define __XTEXTURE_EX
#define __YDUNGEON_TRIGGER
#define __V050322_CACHEQUEST
#define	__V050324_AISCAN
#define __XEFFECT1006

#define __N_TRIGGER_AREASET_050203
#define __S_DONGEON_0504			// 클라/월드/Trans/Beast
#define	__V050524_QUEST				// 캐쉬 퀘스트 개선 
#define __EXPDROPLUCK0524
#define __V050714_BUGFIX_TOOLTIP
//#define		__NPP_050308		// 클라/월드/코어/디비 
#define __V050515_PROPLASTFULL
#define __YRENDER_FLAG
#define __YAIMONSTER_EX
#define __Y_PATROL
#define __Y_ROTXZ
#define	__LEGEND					//10차 전승시스템	Neuz, World, Trans
/*
#define __PIERCING_SM_TIME
#define __XDUALEQUIP
#define __J0608
*/

#define __CSC_WORLDOBJ_LIST			// World Object List
#define __CSC_TERRAIN_OPACITY		// Terrain Texture Opacity Control
#define __CSC_RENDER_AXIS			// Render Axis
#define __CSC_SCALE_CTRL			// Object Scale Control
#define __CSC_UNDO_REDO				// Undo / Redo
#define	__WATER_EXT					// 물 표현 확장 (물 텍스쳐의 종류 및 프레임속도 변경 추가)

#define __BS_INVISIBLE_COLLISION	// 게임 렌더링중 랜더링 제외 속성 ( 충돌처리만 ) 2009/7/30 
#define __BS_SAVE_2DMAP				// dds, bit맵 출력시 죽는문제 수정 
#define __BS_FIXED_LAYER_ALPHA		// fixed terrain layer alpha logic, 
									// 문제없으며 제 0번 Layer에 Blink(삭제된영역) 영역이 있으면 안되는 룰을 어겨서 색이 올라오는등의 문제가 발생했음.
#define __BS_SHADER_FROM_FILE		// hm............

#define	__CLOUD_ANIMATION_BUG		// 구름 애니메이션이 특정 영역에서만 수행되는 문제 수정
//#define __BS_AI_RESPAWN_ATTR		// 리스폰 구역의  AI설정		
//#define __BS_OVERLAP_LAYER		// 랜드스케이프 렌더링 및 중복레이어 허용

#define __BS_SUPPORT_LIGHTMAP		//20110307 : 라이트맵 너어볼까?

#endif //__BEASTHEADER_H_


