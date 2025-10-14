#include "stdafx.h"
#include "CommonCtrl.h"
#include "defineObj.h"
#include "CreateObj.h"
#include "defineSkill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMapStrToObjId	g_MapStrToObjId;
 
CCommonCtrl::CCommonCtrl()
{
	m_dwType = OT_CTRL;

	/////////////////////////////////////////////////////////////////////////////////////////
	ZeroMemory( &m_CtrlElem, sizeof( m_CtrlElem ) );
	/////////////////////////////////////////////////////////////////////////////////////////
	m_CtrlElem.m_dwMinItemNum			= 1;			//-최소 발생 아이템 수 
	m_CtrlElem.m_dwMaxiItemNum			= 3;			//-최대 발생 아이템 수 

	m_CtrlElem.m_dwInsideItemKind[0] = 0;//II_WEA_SWO_FLOWER;
	m_CtrlElem.m_dwInsideItemPer [0] = 1500000000;
	m_CtrlElem.m_dwInsideItemKind[1] = 0;//II_WEA_CHEE_ZANNE;
	m_CtrlElem.m_dwInsideItemPer [1] = 1500000000;
	m_CtrlElem.m_dwInsideItemKind[2] = 0;//II_WEA_WAN_BLUEBOW;
	m_CtrlElem.m_dwInsideItemPer [2] = 1500000000;
	m_CtrlElem.m_dwInsideItemKind[3] = 0;//II_ARM_ARM_SHI_FURY;
	m_CtrlElem.m_dwInsideItemPer [3] = 1500000000;
	
	m_CtrlElem.m_dwMonResKind  [0]      = MI_DEMIAN2;
	m_CtrlElem.m_dwMonResNum   [0]		= 0;
	m_CtrlElem.m_dwMonActAttack[0]		= 0;
	m_CtrlElem.m_dwMonResKind  [1]      = MI_BURUDENG2;
	m_CtrlElem.m_dwMonResNum   [1]		= 0;
	m_CtrlElem.m_dwMonActAttack[1]		= 0;
	m_CtrlElem.m_dwMonResKind  [2]      = MI_MIA2;
	m_CtrlElem.m_dwMonResNum   [2]		= 0;
	m_CtrlElem.m_dwMonActAttack[2]		= 0;

	m_CtrlElem.m_dwTrapOperType         = xRandom(2);
	m_CtrlElem.m_dwTrapRandomPer        = 2000000000;
	m_CtrlElem.m_dwTrapDelay            = 1000;
	
	m_CtrlElem.m_dwTrapKind [0] = SI_GEN_STUNSINGLE;    
	//m_CtrlElem.m_dwTrapKind [0] = SI_GEN_STUNGROUP;    
	m_CtrlElem.m_dwTrapLevel[0] = 3;

	m_CtrlElem.m_dwTrapKind [1] = SI_GEN_FLASH;
	//m_CtrlElem.m_dwTrapKind [0] = SI_GEN_STUNGROUP;    
	m_CtrlElem.m_dwTrapLevel[1] = 3;
	
	/////////////////////////////////////////////////////////////////////////////////////////
	SetAngle( (float)( xRandom(360) ) );
	
	Init();
}
CCommonCtrl::~CCommonCtrl()
{
#if !defined(__BEAST) && !defined(__ONLINE)
//{{AFX
	GetWorld()->m_respawner.Increment( GetRespawn(), FALSE );
//}}AFX
#endif
}

BOOL CCommonCtrl::Read( CFileIO* pFile )
{
	CObj::Read( pFile );

#ifdef __S_DONGEON_0504
	CCtrlElem* pCtrlElem;
 #ifdef __BEAST
	pCtrlElem = &m_respawnInfo.m_CtrlElem;
 #else
	pCtrlElem = &m_CtrlElem;
 #endif
	DWORD dwVersion;
	pFile->Read( &dwVersion, sizeof( DWORD ) );
	if( dwVersion == 0x80000000 )
	{
		pFile->Read( pCtrlElem, sizeof( CCtrlElem ) );
	}
	else
	if( dwVersion == 0x90000000 )
	{
#ifdef __LEGEND		
		// 예전 : 368
		// 현재 : 432
		pFile->Read( pCtrlElem, 88 );
		pFile->Read( ((BYTE*)pCtrlElem + 152), sizeof(CCtrlElem) - 152 );	// 432 - 64 = 368
#else //__LEGEND
		pFile->Read( pCtrlElem, sizeof( CCtrlElem ) );
#endif //__LEGEND
	}
	else
	{
		pCtrlElem->m_dwSet = dwVersion;
		pFile->Read( &pCtrlElem->m_dwSetItem, sizeof( CCtrlElem ) - ( sizeof( DWORD ) * 10 ) );
		// m_dwSet, m_dwSetItemCount, m_dwSetQuestNum1, m_dwSetFlagNum1
	}
#else // __S_DONGEON_0504
 #ifdef __BEAST
	pFile->Read( &m_respawnInfo.m_CtrlElem, sizeof( m_respawnInfo.m_CtrlElem ) );
 #else
	pFile->Read( &m_CtrlElem, sizeof( m_CtrlElem ) );
 #endif	
#endif // __S_DONGEON_0504

	return TRUE;
}
BOOL CCommonCtrl::Write( CFileIO* pFile )
{
	CObj::Write( pFile );

	// 비스트에서만 저장함.
#ifdef __S_DONGEON_0504
	CCtrlElem* pCtrlElem;
 #ifdef __BEAST
	pCtrlElem = &m_respawnInfo.m_CtrlElem;
 #else
	pCtrlElem = &m_CtrlElem;
 #endif
	DWORD dwVersion = 0x80000000;
	pFile->Write( &dwVersion, sizeof( DWORD ) );
	pFile->Write( pCtrlElem, sizeof( CCtrlElem ) );
#else // __S_DONGEON_0504
 #ifdef __BEAST
	pFile->Write( &m_respawnInfo.m_CtrlElem, sizeof( m_respawnInfo.m_CtrlElem ) );
 #else
	pFile->Write( &m_CtrlElem, sizeof( m_CtrlElem ) );
 #endif
#endif // __S_DONGEON_0504
	
	return TRUE;
}

void CCommonCtrl::DestroyWall( void )
{
}

// 싸이킥 월
void CCommonCtrl::_ProcessWall( void )
{
	if( m_nCount == 0 )
	{
	}

	D3DXVECTOR3	vPos = GetPos();

	int nRange = 4;
	// 일반적으로 fDepth가 가장 길기때문에 검사 영역은 fDepth로 했다. 
	float fDepth = 3;
	
	if( fDepth <= 4.0f )		nRange = 4;
	else if( fDepth <= 8.0f )	nRange = 8;
	else if( fDepth <= 16.0f )	nRange = 16;
	else						nRange = 32;


}

void CCommonCtrl::Process( FLOAT fElapsedTime )
{
	if( GetIndex() == CI_PSYCHICWALL )		// 싸이킥월일 경우 따로 예외처리
	{
		_ProcessWall();
		m_nCount ++;
		return;
	}

	if( m_pModel == NULL )
		return;
	
	CtrlProp* pProp = GetProp();

	if( pProp == NULL )
		return;

	
		m_pModel->FrameMove();		

		if( m_bAniPlay == FALSE )
			m_pModel->m_fFrameCurrent = m_fFrameCount;
		else
			m_fFrameCount = m_pModel->m_fFrameCurrent;

		// 사라지는 컨트롤
		if( pProp->dwCtrlKind2 == CK2_FADE )
		{			
			if( m_bAlpha )
			{
				m_nBlendFactorLocal-=5;
				
				if( m_nBlendFactorLocal < 0 )
				{
					m_nBlendFactorLocal = 0;
				}
			}
		}
		else
		// 사라지지 않는 컨트럴은 다시 작동가능하게 설정  (예)트리거
		if( pProp->dwCtrlKind2 == CK2_KEEP )
		{
			if( m_pModel->m_bEndFrame )
			{
				Init();
				m_pModel->m_bEndFrame     = FALSE;
				m_pModel->m_fFrameCurrent = 0.0f;
			}
		}
		
		if( pProp->dwCtrlKind3 == CK3_HALF )
		{
			// 풀에니메이션이 아니라면...현재 프레임이 전체의 50%일때 멈춘다.
			if( m_bAniPlay && m_pModel->m_fFrameCurrent == m_pModel->GetMaxFrame()/2 ) 
			{
				m_bAniPlay = FALSE;
				m_dwAniDelay = timeGetTime()+SEC(20);  //일정 시간뒤에 나머지 에니메이션 동작함
			}
			
			// 일정 시간이 지났으면 나머지 에니메이션 동작
			if( !m_bAniPlay && m_dwAniDelay < timeGetTime() )
			{	
				m_dwAniDelay = 0xffffffff;
				m_bAniPlay = TRUE;
			}
			
		}

}

void CCommonCtrl::_ProcessAction()
{
}

void CCommonCtrl::_ProcessTrap()
{
}


void CCommonCtrl::_CreateTrapSkill()
{

}


void CCommonCtrl::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( GetIndex() == CI_PSYCHICWALL )		// 싸이킥월일경우 따로 처리 - 일단 땜빵.
	{
		m_pSfxModel->m_vPos = GetPos();
		m_pSfxModel->m_vRotate.y = GetAngle();
		m_pSfxModel->m_vScale = GetScale();
		m_pSfxModel->m_matScale = m_matScale;
		m_pSfxModel->Render( pd3dDevice, NULL );
		return;
	}
		
	CtrlProp *pCtrlProp = GetProp();

	CObj::Render( pd3dDevice );

#ifdef __BEAST
	if( m_respawnInfo.m_dwIndex )
		RenderRect( pd3dDevice, m_respawnInfo.m_rect, RECTCOLOR_CTRL );
#endif
}

void CCommonCtrl::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{
#ifndef __WORLDSERVER
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
    pd3dDevice->GetViewport( &vp );
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	vOut.x -= pFont->GetTextExtent( GetProp()->szName ).cx / 2;
	pFont->DrawText( vOut.x + 1, vOut.y + 1, 0xff000000, GetProp()->szName );
	pFont->DrawText( vOut.x, vOut.y, dwColor, GetProp()->szName );
	return;
#endif	// __WORLDSERVER
}

void CCommonCtrl::SetActionPlay()
{
	m_pModel->SetLoop( ANILOOP_CONT );
	m_bAniPlay = TRUE;
}

void CCommonCtrl::DropItem()
{
	int nTotalNum = (xRandom(m_CtrlElem.m_dwMaxiItemNum - m_CtrlElem.m_dwMinItemNum) + m_CtrlElem.m_dwMinItemNum) + 1;
	DWORD	dwAdjRand;

	for( int i=0; i<MAX_CTRLDROPITEM; i++ )
	{
		ItemProp* pItemProp = prj.GetItemProp( m_CtrlElem.m_dwInsideItemKind[i] );
		
		if( pItemProp == NULL )
		{
	#ifdef __YDEBUG
			Error( "CtrlProp(%s)의 ItemKind(%d)값이 잘못되었다.", GetName(), m_CtrlElem.m_dwInsideItemKind[i] );
	#endif // __YDEBUG
			return;
		}

		dwAdjRand	= xRandom( 3000000000 );

		if( m_CtrlElem.m_dwInsideItemPer[i] >= dwAdjRand && nTotalNum )
		{
			CItemElem* pItemElem	= new CItemElem;
			pItemElem->m_dwItemId	= pItemProp->dwID;
			pItemElem->m_nItemNum	= 1;

			// 마지막 슬롯은 레어아이템용슬롯 레어아이템은 1개를 초과할수 없다.
			if( i == MAX_CTRLDROPITEM-1 )
			{
				if( pItemElem->m_nItemNum > 1 )
					pItemElem->m_nItemNum = 1;
			}

			nTotalNum -= pItemElem->m_nItemNum;

			pItemElem->m_nHitPoint	= pItemProp->dwEndurance;

			pItemElem->SetAbilityOption( 0 );
			pItemElem->m_dwSerialNumber		= xRand();
			CItem* pItem	= new CItem;

			pItem->m_pItemBase	= pItemElem;
			{
				pItem->m_idOwn			= m_nMoverID;
				pItem->m_dwDropTime		= timeGetTime();
			}
			pItem->m_bDropMob	= FALSE;
			if( pItem->m_pItemBase->m_dwItemId == 0 ) Error( "CCommonCtrl::DropItem SetIndex\n" );
			pItem->SetIndex( D3DDEVICE, pItem->m_pItemBase->m_dwItemId );
			
			D3DXVECTOR3	v3Pos = GetPos();

			FLOAT fRange = (float)xRandom(30)-15;

			fRange*=0.1f;

			v3Pos.x += fRange;
			v3Pos.z += fRange;
			pItem->SetPos( v3Pos );
			pItem->SetAngle( (float)( xRandom(360) ) );
			GetWorld()->AddObj( pItem, TRUE );
		}
	}

	m_CtrlElem.m_dwMaxiItemNum = 0;
}

void CCommonCtrl::DropNPC()
{
	for( int i=0; i<MAX_CTRLDROPMOB; i++ )
	{
		MoverProp* pMoverProp = prj.GetMoverProp( m_CtrlElem.m_dwMonResKind[i] );
		
		if( pMoverProp == NULL )
		{
#ifdef __YDEBUG
			Error( "CtrlProp(%s)의 m_dwMonResKind(%d)값이 잘못되었다.", GetName(), m_CtrlElem.m_dwMonResKind[i] );
#endif // __YDEBUG
			return;
		}

		// 일반 몹 생성
		DWORD dwBaseMob = m_CtrlElem.m_dwMonResNum[i]-m_CtrlElem.m_dwMonActAttack[i];

		for( int j=0; j<(int)( dwBaseMob ); j++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	
				return;

			D3DXVECTOR3	v3Pos = GetPos();
			v3Pos.y = GetWorld()->GetLandHeight( v3Pos );
			v3Pos.y+= 0.01f;
			
			FLOAT fRange = (float)xRandom(30)-15;
			
			fRange*=0.1f;
			
			v3Pos.x += fRange;
			v3Pos.z += fRange;
			
			pObj->SetPos( v3Pos );
			pObj->InitMotion( pObj->m_dwMotion );
			pObj->UpdateLocalMatrix();
			
			((CMover*)pObj)->m_bActiveAttack = FALSE;

			GetWorld()->AddObj( pObj, TRUE );
		}

		// 선공 몹 생성
		for( int j=0; j<(int)( m_CtrlElem.m_dwMonActAttack[i] ); j++ )
		{
			CObj* pObj	= CreateObj( D3DDEVICE, OT_MOVER, pMoverProp->dwID );
			if( NULL == pObj )	
				return;
			
			D3DXVECTOR3	v3Pos = GetPos();
			v3Pos.y = GetWorld()->GetLandHeight( v3Pos );
			v3Pos.y+= 0.01f;
			
			FLOAT fRange = (float)xRandom(30)-15;
			
			fRange*=0.1f;
			
			v3Pos.x += fRange;
			v3Pos.z += fRange;
			
			pObj->SetPos( v3Pos );
			pObj->InitMotion( pObj->m_dwMotion );
			pObj->UpdateLocalMatrix();
			
			((CMover*)pObj)->m_bActiveAttack = TRUE;
			
			GetWorld()->AddObj( pObj, TRUE );
		}
	}
}

void CCommonCtrl::Init()
{
	m_bTrap    = FALSE;
	m_bAniPlay = FALSE;
	m_dwTrapProcessTime = 0xffffffff;
	
	m_bAlpha = FALSE;
	m_nBlendFactorLocal = 255;
	m_nMoverID = NULL_ID;

	m_pSkillProp = NULL;
	m_pAddSkillProp = NULL;
	m_pSfxModel = m_pSfxModel2 = NULL;
	m_nCount = 0;
	m_nEvent = 0;
	m_idAttacker = NULL_ID;
	m_tmStart = timeGetTime();
	m_nLife = 0;
	
	m_fFrameCount = 0.0f;
	m_dwAniDelay = 0xffffffff;	
	m_bAniDelay  = FALSE;
}

