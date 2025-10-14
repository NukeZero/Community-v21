#include "stdafx.h"
#include "defineObj.h"
#include "guild.h"
#include "authorization.h"

#include "sfx.h"
#include "party.h"
//#include "..\_UnhandledException\ExceptionHandler.h"
extern	CPartyMng	g_PartyMng;

#include "defineskill.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __WORLDSERVER

//////////////////////////////////////////////////////////////////////////////////////////////
// CMover
//////////////////////////////////////////////////////////////////////////////////////////////
void CMover::SetRenderPartsEffect( int nParts )
{
	CModelObject* pModel = (CModelObject*)m_pModel;
	CItemElem *pItemElem = NULL;
	ItemProp *pItemProp = NULL;

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( nParts );	// 오른쪽 무기에
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} 
	else
	{
		DWORD dwItemID	= m_adwEquipment[ nParts ];
		if( dwItemID != NULL_ID )
			pItemProp = prj.GetItemProp( dwItemID );
	}
	
	if( pItemProp )
	{
		if( pItemProp->nReflect > 0 )	// 리플렉트가 걸려있으면
			pModel->SetEffect( nParts, XE_REFLECT );	// 리플렉트 옵션으로 렌더.
		
		BOOL bExec = FALSE;
		
		switch( pItemProp->dwSfxElemental )
		{
#ifdef __XEFFECT0530
		case ELEMENTAL_FIRE:	pModel->SetEffect( nParts, XE_ITEM_FIRE | (5 << 24) );	bExec = TRUE; break; // 불 타는 옵션.
		case ELEMENTAL_ELEC:	pModel->SetEffect( nParts, XE_ITEM_ELEC | (0 << 24) );	bExec = TRUE; break; // 전기 옵션.
		case ELEMENTAL_WATER:	pModel->SetEffect( nParts, XE_ITEM_WATER | (5 << 24));	bExec = TRUE; break; // 물 옵션
		case ELEMENTAL_WIND:	pModel->SetEffect( nParts, XE_ITEM_WIND  | (5 << 24));	bExec = TRUE; break; // 바람 옵션
		case ELEMENTAL_EARTH:	pModel->SetEffect( nParts, XE_ITEM_EARTH | (5 << 24));	bExec = TRUE; break;// 땅 옵션
			break;
#else // xEffect0530
		case ELEMENTAL_FIRE:	pModel->SetEffect( nParts, XE_ITEM_FIRE );	bExec = TRUE; break; // 불 타는 옵션.
		case ELEMENTAL_ELEC:	pModel->SetEffect( nParts, XE_ITEM_ELEC );	bExec = TRUE; break; // 전기 옵션.
 #ifdef __XEFFECT1006
		case ELEMENTAL_WATER:	pModel->SetEffect( nParts, XE_ITEM_WATER );	bExec = TRUE; break; // 물 옵션
		case ELEMENTAL_WIND:	pModel->SetEffect( nParts, XE_ITEM_WIND  );	bExec = TRUE; break; // 바람 옵션
		case ELEMENTAL_EARTH:	pModel->SetEffect( nParts, XE_ITEM_EARTH );	bExec = TRUE; break;// 땅 옵션
			break;
 #endif // xEffect1006
#endif // not xEffect0530
		}
		
#ifdef __XEFFECT1006
		int nAttrLevel = 0;
		int	nAttr = 0;
		if( pItemElem )
		{
			nAttrLevel = pItemElem->m_nResistAbilityOption;
			nAttr = pItemElem->m_bItemResist;
		} else
		{
			nAttr	   = (m_anEquipOption[nParts] & 0x00FF0000) >> 16;
			nAttrLevel = (m_anEquipOption[nParts] & 0xFF000000) >> 24;
		}
		
		{
			int nEffLevel = 0;
			if( nAttr && (nAttrLevel > 10 || nAttrLevel < 0) )	// 속성은 있는데 속성레벨값이 이상할때.
			{
				LPCTSTR szErr = Message( "m_nResistAbilityOption=%d %s", nAttrLevel, GetName() );
				ADDERRORMSG( szErr );
				nAttrLevel = 10;
			}
			
			if( nAttrLevel == 10 )		nEffLevel = 5;
			else if( nAttrLevel == 9 )	nEffLevel = 4;
			else if( nAttrLevel == 8 )	nEffLevel = 3;
			else if( nAttrLevel == 7 )	nEffLevel = 2;
			else if( nAttrLevel >= 4 )	nEffLevel = 1;	// 4 ~ 6
			else 
				nEffLevel = 0;
			
			if( nEffLevel >= 1 )	// 속성제련레벨 1,2,3은 이펙트 없음.
			{
				nEffLevel <<= 24;
				
				switch( nAttr )
				{
				case SAI79::FIRE:			pModel->SetEffect( nParts, XE_ITEM_FIRE | nEffLevel );	break;	
				case SAI79::ELECTRICITY:	pModel->SetEffect( nParts, XE_ITEM_ELEC | nEffLevel );	break;
				case SAI79::WATER:			pModel->SetEffect( nParts, XE_ITEM_WATER | nEffLevel );	break;
				case SAI79::WIND:			pModel->SetEffect( nParts, XE_ITEM_WIND | nEffLevel );	break;
				case SAI79::EARTH:			pModel->SetEffect( nParts, XE_ITEM_EARTH | nEffLevel );	break;
				}
			}
		}
#endif // xEffect1006
		
	}
}
void CMover::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	MoverProp *pMoverProp = GetProp();
	if( pMoverProp == NULL )
	{
		LPCTSTR szErr = Message( "CMover::Render : 프로퍼티 못읽음 %s", GetName() );
		ADDERRORMSG( szErr );
	}

	if( !IsVisible() || IsCull() )
		return;

	
	// 사이즈가 스크린 좌표에서 20 픽셀보다 작으면 출력하지 않는다.
//if( IsShowSmallSize( pd3dDevice, 10 ) == FALSE )
//		return;

	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXVECTOR3 vPos = GetPos();
	D3DXMATRIX  mWorld;

/*	
	mWorld = m_matWorld;		// 이후부터는 m_matWorld를 쓰지말고 matWorld를 쓰자 

	if( IsPlayer() )
		mWorld._42 += 0.07f;		// 구두의 힐 높이만큼 올려줌.
	// 좌표 이동 
*/
	mWorld = m_matWorld;		// 이후부터는 m_matWorld를 쓰지말고 matWorld를 쓰자 

	
	// 티나 스케일 조정...
	if( GetIndex() == MI_MASA_TINA )
	{
		ResetScale();
		D3DXMatrixScaling(&m_matScale, GetScale().x, GetScale().y, GetScale().z );
	}


//	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);

	D3DXVECTOR3 vPos2 = GetPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = (int)( D3DXVec3Length( &vPos2 ) );
	if( nLength > CWorld::m_fFarPlane ) 
		nLength = (int)( CWorld::m_fFarPlane );
	if( nLength > CWorld::m_fFarPlane - ( CWorld::m_fFarPlane / 4 ) )
		return;
	//m_pModel->SetBlendFactor( m_nFadeoutCnt );//255 - nBlendFactor );
	//m_pModel->SetBlendFactor( 255 );//255 - nBlendFactor );

	CItemElem *pItemElem = NULL, pItemElemCloak;
	ItemProp *pItemProp = NULL;

	CModelObject* pModel = (CModelObject*)m_pModel;


	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_RWEAPON );	// 오른쪽 무기에
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} 
	else
	{
		DWORD dwItemID = m_adwEquipment[ PARTS_RWEAPON ];
		if( dwItemID != NULL_ID )
			pItemProp = prj.GetItemProp( dwItemID );
	}

	// 들고있는 무기의 이펙트 세팅.
	SetRenderPartsEffect( PARTS_RWEAPON );
	SetRenderPartsEffect( PARTS_LWEAPON );

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_SHIELD );	// 방패에
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} else
	{
		DWORD dwItemID = m_adwEquipment[ PARTS_SHIELD ];	// activemover 가 아닌놈은 가상아이템으로 꺼낸다.
		if( dwItemID != NULL_ID )
			pItemProp = prj.GetItemProp( dwItemID );
	}
	if( pItemProp )
	{
		if( pItemProp->nReflect > 0 )	// 리플렉트가 걸려있으면
			pModel->SetEffect( PARTS_SHIELD, XE_REFLECT );	// 리플렉트 옵션으로 렌더.
	}

	if( pModel )
	{
		O3D_ELEMENT *pElem = pModel->GetParts( PARTS_HAIR );

		if( pElem )
		{
			if( pElem->m_pObject3D )
			{
				pElem->m_pObject3D->m_fAmbient[0] = m_fHairColorR;
				pElem->m_pObject3D->m_fAmbient[1] = m_fHairColorG;
				pElem->m_pObject3D->m_fAmbient[2] = m_fHairColorB;
			}
		}
	}
	

	m_pModel->Render( pd3dDevice, &mWorld ); 


	if( m_pObjHighlight == this )
	{
	//	m_pObjHighlight = NULL;
		if( m_pModel->m_nNoEffect == 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}
		{
			if( m_pModel->m_nNoEffect == 0 )
			{
				m_pModel->Render( pd3dDevice, &mWorld ); 
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			}
		}

	}
	if( m_pActMover )
		m_pActMover->Render( pd3dDevice, &mWorld ); 
	
#ifdef __BEAST
	if( m_respawnInfo.m_dwIndex )
	{
		RenderRect( pd3dDevice, m_respawnInfo.m_rect, RECTCOLOR_MOVER );
	}
/*
	if( m_respawnInfo.m_dwIndex )
	{
		//GetWorld()->RenderRectTracker( CRect( vPos.x- 10, vPos.z - 10,  vPos.x + 10, vPos.z + 10 ), 0, 0x808080ff );
		CRect rect = m_respawnInfo.m_rect;
		CPoint point = CPoint( vPos.x, vPos.z );
		point.x = point.x / MPU * MPU;
		point.y = point.y / MPU * MPU;
		rect += point;
		// RenderGrids는 원점의 트랜스폼에서 시작하므로 트랜스폼을 리셋해야한다.
		D3DXMATRIX mWorld;
		D3DXMatrixIdentity( &mWorld ); 
		pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );
		GetWorld()->RenderGrids( rect, 0, 0x80ff0000 );
	//	RenderName( pd3dDevice );
	}
	*/
#endif

}

// 이것이 호출될때는 바로 이전에 CMover::Render()가 호출되어야 한다.
void CMover::RenderPartsEffect( LPDIRECT3DDEVICE9 pd3dDevice )
{

}



void CMover::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{
	if( !IsVisible() )
		return;
	char szName[ 256 ];
	strcpy( szName, m_szName );

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
	//vOutFLOAT fHeight = ( vOut[0].y - vOut[4].y ) - 10f;
	CPoint point;
	point.x = (int)( vOut.x );
	point.y = (int)( vOut.y );

	MakeEven( point.x );
	point.x -= pFont->GetTextExtent( szName ).cx / 2;
	point.y -= 20;
	point.x -= 3;
	
	{
		LPCHARACTER lpCharacter = GetCharacter();
		if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
		{
			LPSTRUCTURE lpStr = &prj.m_aStructure[ lpCharacter->m_nStructure ];
			TCHAR szStr[ 64 ];
			sprintf( szStr, "[%s]", lpStr->szName );
			point.x = (int)( vOut.x );
			point.y = (int)( vOut.y );
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szStr ).cx / 2;
			point.y -= 35;
			pFont->DrawText( (float)( point.x ), (float)( point.y ), COLOR_NPC, szStr );
			point.x = (int)( vOut.x );
			point.y = (int)( vOut.y );
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szName ).cx / 2;
			point.y -= 20;
		}
		//sprintf( szNameTemp, "NPC %s", m_szName );
		//pFont->DrawText( vOut.x + 1, vOut.y + 1, 0xff000000, szName );
		pFont->DrawText( (float)( point.x ), (float)( point.y ), COLOR_NPC, szName );
	}
	

// Render Guild Name & Logo
	CGuild* pGuild = GetGuild();	
	if( pGuild )
	{
		point.x = (int)( vOut.x );
		point.y = (int)( vOut.y );
		MakeEven( point.x );	
		point.x -= pFont->GetTextExtent( pGuild->m_szGuild ).cx / 2;
		point.y -= 32;
		point.x -= 3;
		
		pFont->DrawText( (float)( point.x ), (float)( point.y ), 0xa876f3ff, pGuild->m_szGuild );

	}




}


#endif // not worldserver