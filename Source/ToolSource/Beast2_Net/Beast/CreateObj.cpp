#include "stdafx.h"
#include "defineSound.h"
#include "DefineObj.h"
#include "..\_AIInterface\AIClockWorks.h"
#include "..\_AIInterface\AIMonster.h"
#include "..\_AIInterface\AIPet.h"
#include "CommonCtrl.h"
#include "Sfx.h"

#include "Ship.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMover *CreateMover( CWorld *pWorld, DWORD dwID, D3DXVECTOR3 vPos )
{
	CMover *pMover;

	pMover = (CMover *)CreateObj( D3DDEVICE, OT_MOVER, dwID );
	if( pMover == NULL )	
		return NULL;
	pMover->SetPos( vPos );
	pMover->InitMotion( MTI_STAND );
	pMover->UpdateLocalMatrix();
	
	pMover->AddItToGlobalId();
	pWorld->AddObj( pMover, FALSE );

	return pMover;
}



/*
	XI_로 시작하는건 하나의 빌보드 텍스춰일 수도 있고, 특수효과 툴에 의해 생성된 데이타일 수도 있다.
	
	CreateGenSfx - 일반 효과 인덱스에 해당하는 효과 인덱스로 생성 
			dwSfxIdx( EI_EXPLOSION ) -> Create( XI_EXPLOSION )

	CreateItemSfx - 아이템 인덱스에 해당하는 효과 인덱스로 생성 
			pItemElem( II_FOOD ) -> Create( XI_FOOD )
			
	CreateSkillSfx - 스킬 인덱스에 해당하는 효과 인덱스로 생성 
			pItemElem( SI_FIREBALL ) -> Create( XI_FIREBALL )
		
*/



// 일반 효과 생성 
// SFX 새로 만들고도 여기다가 추가 안해주면 생성 안된다.
CSfx* CreateSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;
#ifdef _DEBUG
	if( dwSfxObj == 575 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif

	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
	return pObj;
}

// 요요무기는 무조건 여기만 호출
CSfx* CreateSfxYoYo( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;

#ifdef _DEBUG
	if( dwSfxObj == 116 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
	return pObj;
}

CSfxShoot* CreateShootSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
						D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfxShoot* pObj = NULL;
#ifdef _DEBUG
	if( dwSfxObj == XI_SKILL_MAG_FIRE_FIRESTRIKE01 )	// 디버깅 하려면 이걸 바꿔 쓰세요.
	{
		int a = 0;
	}
#endif
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );
	
	return pObj;
}

CSfx* CreateItemReadySfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSfxObj, 
				 D3DXVECTOR3& vPosSrc, OBJID idSrc, D3DXVECTOR3& vPosDest, OBJID idDest, int nSec )
{
	CSfx* pObj = NULL;
	CMover *pSrc = prj.GetMover( idSrc );
	CMover *pDst = prj.GetMover( idDest );

	return pObj;
}

// 아이템을 사용시 발생할 효과 생성 
CSfx* CreateItemSfx( LPDIRECT3DDEVICE9 pd3dDevice, CItemElem* pItemElem, OBJID idSrc, const D3DXVECTOR3& vPosSrc, OBJID idDest, const D3DXVECTOR3& vPosDest )
{
	CSfx* pObj = NULL;
	ItemProp* pProp = prj.GetItemProp( pItemElem->m_dwItemId );
	
//	switch( pProp->dwSfxObj )
//	{
//	case XI_ITEM_WAND_ATK: 
//		pObj = new CSfxItemWandAtk; pObj->SetSfx( -1, idSrc, vPosSrc, idDest, vPosDest ); break;
		
//	}
	return pObj;
}

CSfx* CreateItemSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwItemId, OBJID idSrc, const D3DXVECTOR3& vPosSrc, OBJID idDest, const D3DXVECTOR3& vPosDest )
{
	CSfx* pObj = NULL;
	ItemProp* pProp = prj.GetItemProp( dwItemId );
	
//	switch( pProp->dwSfxObj )
//	{
//	case XI_ITEM_WAND_ATK: pObj = new CSfxItemWandAtk; pObj->SetSfx( -1, idSrc, vPosSrc, idDest, vPosDest ); break;
//	}
	return pObj;
}
// 스킬 사용시 발생할 효과 생성 
CSfx* CreateSkillSfx( LPDIRECT3DDEVICE9 pd3dDevice, LPSKILL pSkill, OBJID idSrc, D3DXVECTOR3& vPosSrc, OBJID idDest, D3DXVECTOR3& vPosDest )
{
	return CreateSkillSfx( pd3dDevice, pSkill->dwSkill, idSrc, vPosSrc, idDest, vPosDest );
}
CSfx* CreateSkillSfx( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwSkill, OBJID idSrc, D3DXVECTOR3& vPosSrc, OBJID idDest, D3DXVECTOR3& vPosDest )
{
	CSfx* pObj = NULL; 
	ItemProp* pProp = prj.GetSkillProp( dwSkill );
	//pObj = new CSfxGeneral; pObj->SetSfx( pProp->dwSfxObj, idSrc, vPosSrc, idDest, vPosDest );
//	switch( pProp->dwSfxObj )
//	{
//	case XI_SKILL_MER_YATTACK      : pObj = new CSfxSkillMerYAttack     ; pObj->SetSfx( pProp->dwSfxObj, vPosSrc, idSrc, vPosSrc, idSrc ); break;
//	case XI_SKILL_MER_SHINING_SWING: pObj = new CSfxSkillMerShiningSwing; pObj->SetSfx( pProp->dwSfxObj, vPosSrc, idSrc, vPosSrc, idSrc ); break;
//	case XI_SKILL_MER_OVER_CUTTER  : pObj = new CSfxSkillMerOverCutter  ; pObj->SetSfx( pProp->dwSfxObj, vPosSrc, idSrc, vPosSrc, idSrc ); break;
//	case XI_SKILL_MAG_TRIPLE_BALL  : pObj = new CSfxSkillMagTripleBall  ; pObj->SetSfx( pProp->dwSfxObj, vPosSrc, idSrc, vPosSrc, idSrc ); break;
//	case XI_SKILL_MAG_SPARKLE_RAGE : pObj = new CSfxSkillMagSparkleRage ; pObj->SetSfx( pProp->dwSfxObj, vPosSrc, idSrc, vPosSrc, idSrc ); break;
//	case XI_SKILL_MAG_FLASH_TORNADO: pObj = new CSfxSkillMagFlashTornado; pObj->SetSfx( pProp->dwSfxObj, vPosSrc, idSrc, vPosSrc, idSrc ); break;
//	}
	//XI_PARTICLE // 파티클 모음집 
	//XI_HITEFFECT // 타격 효과 모음집 
	//XI_ETC // 잡다구리 모음집 
	/*
	*/
	return pObj;
}


CObj* NewObj( DWORD dwObjType, DWORD dwObjIndex )
{
	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj; break;
		case OT_CTRL : pObj = new CCommonCtrl; break;
		//case OT_LIGHT: pObj = new CItem; break;
		//case OT_CAMERA: pObj = new CItem; break;
		case OT_SFX  : pObj = new CSfx; break;  
		case OT_ITEM : pObj = new CItem; break;
		case OT_MOVER: pObj = new CMover; break;
		case OT_REGION: pObj = new CRegion; break;

	}
	pObj->m_dwType = dwObjType;
	pObj->m_dwIndex = dwObjIndex;
	return pObj;
}

CObj* CreateObj( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwObjType, DWORD dwObjIndex, BOOL bInitProp )
{
	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj; break;
		case OT_CTRL : pObj = new CCommonCtrl; break;
		//case OT_LIGHT: pObj = new CItem; break;
		//case OT_CAMERA: pObj = new CItem; break;
		case OT_SFX  : pObj = new CSfx; break;  
		case OT_ITEM : pObj = new CItem; break;
		case OT_MOVER: pObj = new CMover; break;
		case OT_REGION: pObj = new CRegion; break;
#ifdef __Y_PATROL
		case OT_PATH: pObj = new CPathObj; break;
#endif //__Y_PATROL			

	}

	if(pObj)
	{
		pObj->m_dwType = dwObjType;

		if( pObj->GetType() == OT_SFX )
		{
			((CSfx*)pObj)->SetSfx( pd3dDevice, dwObjIndex, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), NULL_ID ); 
			return pObj;
		}
//		else
			
		/*
		if( dwObjType == OT_REGION )
		{
			pObj->m_dwIndex = dwObjIndex;
			((CRegion*)pObj)->InitDeviceObjects( pd3dDevice );
			((CRegion*)pObj)->RestoreDeviceObjects( pd3dDevice );
			return pObj;
		}
		*/
		if( pObj->SetIndex( pd3dDevice, dwObjIndex, bInitProp ) == TRUE )
		{
			//pObj->InitProp();
			pObj->SetMotion(0);
			// 스피드를 설정하려면 현재로선 이수밖에....좆치안타.
			if( dwObjType == OT_MOVER )
			{
				CMover *pMover = (CMover *)pObj;
				pMover->m_pActMover->m_fSpeed = pMover->GetProp()->fSpeed;
			}
			return pObj;
		}

		Message( "CreateObj : type=%d, index=%d, bInitProp=%d", dwObjType, dwObjIndex, bInitProp );
		SAFE_DELETE( pObj );
	}
	return NULL;
}
CAIInterface* CreateAIInterface( CObj* pObj, DWORD dwAIInterface )
{
	CAIInterface* pAI = NULL;

	return pAI;
}

CObj* ReadObj( CFileIO* pFile )
{
	DWORD dwObjType;
	pFile->Read( &dwObjType, sizeof( DWORD ), 1 );

	CObj* pObj = NULL;
	switch( dwObjType )
	{
		case OT_OBJ  : pObj = new CObj; break;
		case OT_CTRL : pObj = new CCommonCtrl; break;
		case OT_SFX  : pObj = new CSfx; break;  
		case OT_ITEM : 
			//ObjType = OT_MOVER;
			pObj = new CItem; 
//			pObj = new CItem;		TODO_CHECK
			break;
		case OT_MOVER:				// 파일에서 읽어들이는 것은 모두 NPC
			pObj = new CMover; 
			((CMover *)pObj)->InitNPCProperty();
			break;
		case OT_REGION: 
			pObj = new CRegion; 
			break;
	}

	if( pObj ) 
	{
		pObj->Read( pFile );
		return pObj;
	}
	return NULL;
}

BOOL WriteObj( CFileIO* pFile, CObj* pObj )
{
	pFile->Write( &pObj->m_dwType, sizeof( DWORD ), 1 );
	pObj->Write( pFile );
	return TRUE;
}
BOOL CheckVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion )
{
	CHAR szName[32];
	CHAR szVersion[32];

	fread( szName, sizeof( szName ), 1, fp );
	fread( szVersion, sizeof( szVersion ), 1, fp );

	if( !strcmp( szName, lpszName ) && !strcmp( szVersion, lpszVersion) )
		return TRUE;

	return FALSE;
}
BOOL WriteVersion( FILE* fp, LPCTSTR lpszName, LPCTSTR lpszVersion )
{
	if( strlen( lpszName ) >= 32 ||  strlen( lpszVersion ) >= 32 )
		return FALSE;
	fwrite( lpszName, sizeof( 32 ), 1, fp );
	fwrite( lpszVersion, sizeof( 32 ), 1, fp );
	return TRUE;
}
BOOL CheckVersion( FILE* fp, DWORD dwVersion )
{
	DWORD dwVer;
	fread( &dwVer, sizeof( DWORD ), 1, fp );

	if( dwVer == dwVersion )
		return TRUE;

	return FALSE;
}
BOOL WriteVersion( FILE* fp, DWORD dwVersion  )
{
	fwrite( &dwVersion, sizeof( DWORD ), 1, fp );
	return TRUE;
}




















