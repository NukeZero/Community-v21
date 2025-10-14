#ifndef __CREATEOBJ_H__
#define __CREATEOBJ_H__

#include "sfx.h"
//#include "d3dapp.h"

//CSfx* CreateGenSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxIdx, OBJID idSrc, D3DXVECTOR3& vPosSrc );

CMover *CreateMover( CWorld *pWorld, DWORD dwID, D3DXVECTOR3 vPos );
// SFX를 생성시키려면 이놈을 호출한다.
// 
// pd3dDevice d3d디바이스
// dwSfxObj SFX의 ID
// vPosSrc 쏜 위치
// idSrc 쏜놈 ID
// vPosDest 맞을 위치
// idDest 맞을놈 ID
CSfx* CreateSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );				
CSfx* CreateItemReadySfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );
CSfx* CreateItemSfx( LPDIRECT3DDEVICE9 pd3dDevice, CItemElem* pItemElem, OBJID idSrc, const D3DXVECTOR3& vPosSrc, OBJID idDest, const D3DXVECTOR3& vPosDest );
CSfx* CreateItemSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwItemId, OBJID idSrc, const D3DXVECTOR3& vPosSrc, OBJID idDest, const D3DXVECTOR3& vPosDest );
CSfx* CreateSkillSfx( LPDIRECT3DDEVICE9 pd3dDevice, LPSKILL pSkill, OBJID idSrc, D3DXVECTOR3& vPosSrc, OBJID idDest, D3DXVECTOR3& vPosDest );
CSfx* CreateSkillSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSkill, OBJID idSrc, D3DXVECTOR3& vPosSrc, OBJID idDest, D3DXVECTOR3& vPosDest );
CSfxShoot* CreateShootSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );
CSfx* CreateSfxYoYo( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, D3DXVECTOR3& vPosSrc, OBJID idSrc = NULL_ID, D3DXVECTOR3& vPosDest = D3DXVECTOR3( 0, 0, 0), OBJID idDest = NULL_ID, int nSec = 0 );


CObj* CreateObj( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwObjType, DWORD dwObjIndex, BOOL bInitProp = TRUE );
CObj* NewObj( DWORD dwObjType, DWORD dwObjIndex );
CAIInterface* CreateAIInterface( CObj* pObj, DWORD dwAIInterface );
CObj* ReadObj( CFileIO* pFile );

BOOL WriteObj( CFileIO* pFile, CObj* pObj );
BOOL CheckVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion );
BOOL WriteVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion );
BOOL CheckVersion( FILE* fp, DWORD dwVersion );
BOOL WriteVersion( FILE* fp, DWORD dwVersion );

#endif // __CREATEOBJ_H__