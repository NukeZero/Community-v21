#include "stdafx.h"
#include "defineObj.h"
#include "guild.h"
#include "authorization.h"
#ifndef __BEAST
#include "AppDefine.h"
#endif
#include "sfx.h"
#include "party.h"
//#include "..\_UnhandledException\ExceptionHandler.h"
extern	CPartyMng	g_PartyMng;

#ifdef __SCHOOL0701
#include "eveschool.h"
#endif	// __SCHOOL0701

#ifdef __ACROBAT_0504
#include "defineskill.h"
#endif //__ACROBAT_0504
  
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
#ifdef __FINITEITEM0705
		DWORD dwItemID	= m_aEquipInfo[nParts].dwId;
#else	// __FINITEITEM0705
		DWORD dwItemID	= m_adwEquipment[ nParts ];
#endif	// __FINITEITEM0705
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
#ifdef __FINITEITEM0705
			nAttr	   = (m_aEquipInfo[nParts].nOption & 0x00FF0000) >> 16;
			nAttrLevel = (m_aEquipInfo[nParts].nOption & 0xFF000000) >> 24;
#else	// __FINITEITEM0705
			nAttr	   = (m_anEquipOption[nParts] & 0x00FF0000) >> 16;
			nAttrLevel = (m_anEquipOption[nParts] & 0xFF000000) >> 24;
#endif	// __FINITEITEM0705
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
#ifndef __BEAST
#ifdef _DEBUG
	if( g_pPlayer == this )
	{
		int a = 0;
	} else
	{
		int a = 0;
	}

#endif

#endif
	if( !IsVisible() || IsCull() )
		return;

#ifdef __CLIENT		
	if( IsActiveMover() && m_pActMover->IsFly() )
	{
//		if( g_Neuz.m_camera.m_fZoom < 1.0f )		// 가까이 확대됐을땐 안보이게 하자.
//			return;
	}
#endif
		
	// 사이즈가 스크린 좌표에서 20 픽셀보다 작으면 출력하지 않는다.
//if( IsShowSmallSize( pd3dDevice, 10 ) == FALSE )
//		return;

	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

#ifndef __BEAST
	// 사이즈가 스크린 좌표에서 20 픽셀보다 작으면 출력하지 않는다.
//	if( IsShowSmallSize( pd3dDevice, 20 ) == FALSE )
//		return;
#endif
	D3DXVECTOR3 vPos = GetPos();
	D3DXMATRIX  mWorld;

/*	
	mWorld = m_matWorld;		// 이후부터는 m_matWorld를 쓰지말고 matWorld를 쓰자 

	if( IsPlayer() )
		mWorld._42 += 0.07f;		// 구두의 힐 높이만큼 올려줌.
	// 좌표 이동 
*/
 #ifdef __CLIENT
	if( IsPlayer() )
	{
		D3DXVECTOR3 vPosHeel = vPos + D3DXVECTOR3( 0.0f, 0.07f, 0.0f ) ;
		D3DXMatrixTranslation( &m_matTrans, vPosHeel.x, vPosHeel.y, vPosHeel.z );
	}
//	if( IsUpdateMatrix() )
	{
		UpdateMatrix();		// 클라에선 매트릭스 갱신을 렌더에서 한다. 서버에선 Process에서 한다.
	}
 #endif
	mWorld = m_matWorld;		// 이후부터는 m_matWorld를 쓰지말고 matWorld를 쓰자 

#ifdef __CLIENT
	switch( GetIndex() )
	{
	case MI_SYLIACA1:
	case MI_SYLIACA2:
	case MI_SYLIACA3:
	case MI_SYLIACA4:
		if( m_pModel )
			mWorld._42 += (fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y) / 2.0);
		break;
	}
#endif
	
	// 티나 스케일 조정...
	if( GetIndex() == MI_MASA_TINA )
	{
		ResetScale();
		D3DXMatrixScaling(&m_matScale, GetScale().x, GetScale().y, GetScale().z );
	}

#ifndef __BEAST
	// 맞으면 흔들리는 처리.
	if( IsNPC() && m_nDmgCnt > 0 && pMoverProp->dwClass != RANK_SUPER )		// 보스몹은 흔들리지 않음.
	{
//		if( (m_nDmgCnt & 1) == 0 ) 
		{
//			float x = vPos.x;
			D3DXMATRIX matDmg;
//			int cx = -25 + (35 + xRandom(15));
//			int cz = -25 + (35 + xRandom(15));
			int cx = -25 + (xRandom(50));
			int cz = -25 + (xRandom(50));
			D3DXMatrixTranslation( &matDmg, (float)cx / 100.0f, 0, (float)cz / 100.0f );
			D3DXMatrixMultiply( &mWorld, &mWorld, &matDmg );
		}
	}
#endif	//	__BEAST
//	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
//	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);

	D3DXVECTOR3 vPos2 = GetPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = D3DXVec3Length( &vPos2 );
	if( nLength > CWorld::m_fFarPlane ) 
		nLength = CWorld::m_fFarPlane;
	if( nLength > CWorld::m_fFarPlane - ( CWorld::m_fFarPlane / 4 ) )
		return;
	//m_pModel->SetBlendFactor( m_nFadeoutCnt );//255 - nBlendFactor );
	//m_pModel->SetBlendFactor( 255 );//255 - nBlendFactor );

	CItemElem *pItemElem = NULL, pItemElemCloak;
	ItemProp *pItemProp = NULL;

	CModelObject* pModel = (CModelObject*)m_pModel;

/*	
#ifdef __YPARTS_EX
	if( IsPlayer() )
	{
		// 외투의상을 입었다면 주방어구 의상은 숨긴다...

		int nArryEquip1[5] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CLOAK };
		int nArryEquip2[5] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK2 };
		
		O3D_ELEMENT *pElem1 = NULL;
		O3D_ELEMENT *pElem2 = NULL;

		for( int i=0; i<5; i++ )
		{
			pElem1 = pModel->GetParts( nArryEquip1[i] );
			pElem2 = pModel->GetParts( nArryEquip2[i] );

			if( pElem2 )
			{
				if( pElem1 )
					pElem1->m_nEffect |= XE_HIDE;
			}
		}
	}
#endif //__YPARTS_EX	
*/	
#ifndef __BEAST	
	OverCoatItemRenderCheck(pModel);	
#endif //__BEAST	

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_RWEAPON );	// 오른쪽 무기에
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} 
	else
	{
#ifdef __FINITEITEM0705
		DWORD dwItemID = m_aEquipInfo[PARTS_RWEAPON].dwId;
#else	// __FINITEITEM0705
		DWORD dwItemID = m_adwEquipment[ PARTS_RWEAPON ];
#endif	// __FINITEITEM0705
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
#ifdef __FINITEITEM0705
		DWORD dwItemID	= m_aEquipInfo[PARTS_SHIELD].dwId;	// activemover 가 아닌놈은 가상아이템으로 꺼낸다.
#else	// __FINITEITEM0705
		DWORD dwItemID = m_adwEquipment[ PARTS_SHIELD ];	// activemover 가 아닌놈은 가상아이템으로 꺼낸다.
#endif	// __FINITEITEM0705
		if( dwItemID != NULL_ID )
			pItemProp = prj.GetItemProp( dwItemID );
	}
	if( pItemProp )
	{
		if( pItemProp->nReflect > 0 )	// 리플렉트가 걸려있으면
			pModel->SetEffect( PARTS_SHIELD, XE_REFLECT );	// 리플렉트 옵션으로 렌더.
	}
#ifndef __BEAST
	// 망토 텍스쳐 있으면 지정.
	pModel->SetExtTexture( m_pCloakTexture );
#endif	

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
	
#ifndef __BEAST
	if( GetActiveObj() == this ) 
		m_pModel->SetGroup( 0 );
	else
	{	
		if( m_pModel->m_nNoEffect == 0 )
		{
			int fDist = 50.0f;
			switch( g_Option.m_nObjectDetail )
			{
			case 0 :	fDist = 20.0f;	break;
			case 1 :	fDist = 10.0f;	break;
			case 2 :	fDist = 5.0f;	break;
			}
			int nLevel = m_fDistCamera / fDist;
			if( nLevel >= 2 )	nLevel = 2;
			if( nLevel < 0 )	
			{
				Message( "CMover::Render : %s, lod lv=%d %f", m_szName, nLevel, m_fDistCamera );
				nLevel = 0;
			}
			m_pModel->SetGroup( nLevel );
		} else
			m_pModel->SetGroup( 2 );		// 그림자 찍을땐 가장 낮은 단계로 찍자.
	}
#ifdef __ONLINE
 #ifdef __XTEXTURE_EX2
	if( pModel->m_pModelElem->m_nTextureEx > 0 )		// 확장텍스쳐 쓰는넘인가?
		pModel->SetTextureEx( pModel->m_pModelElem->m_nTextureEx );
	else
		pModel->SetTextureEx( 0 );
 #else
	if( pModel->m_pModelElem->m_bTextureEx )		// 확장텍스쳐 쓰는넘인가?
		pModel->SetTextureEx( TRUE );
	else
		pModel->SetTextureEx( FALSE );
 #endif
#endif

//	if( IsAuthHigher( AUTH_GAMEMASTER) )
//	{
#ifdef __YHAIR_HIDE
	if( IsActiveMover() )
	{
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 있는 경우 
		CItemElem* pItemElem	= GetEquipItem( PARTS_CAP );
		O3D_ELEMENT* pElement   = NULL;
		
		if( pItemElem )
		{
			ItemProp* pItemProp = pItemElem->GetProp();
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = ((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}
			
		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= GetEquipItem( PARTS_HAT );
						
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
						
						((CModelObject*)m_pModel)->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
		#endif //__YPARTS_EX							
		}
		else
		{
	#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
			CItemElem* pItemElemOvercoat	= GetEquipItem( PARTS_HAT );
			
			if( pItemElemOvercoat )
			{
				if( !pItemElemOvercoat->IsFlag( CItemElem::expired ) )
				{
					ItemProp* pItemPropOC = pItemElemOvercoat->GetProp();
					if( pItemPropOC && pItemPropOC->dwBasePartsIgnore != -1 )
					{
						if( pItemPropOC->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

						((CModelObject*)m_pModel)->SetEffect(pItemPropOC->dwBasePartsIgnore, XE_HIDE );
					}
				}
			}
	#endif //__YPARTS_EX							
		}		
	}
	else
	{
		// 헬멧이 머리카락 날려야하는것이냐?  // 인벤이 없는경우
		#ifdef __FINITEITEM0705
		DWORD dwId	= m_aEquipInfo[PARTS_CAP].dwId;
		#else	// __FINITEITEM0705
		DWORD dwId	= m_adwEquipment[PARTS_CAP];
		#endif	// __FINITEITEM0705
		O3D_ELEMENT*	pElement = NULL;
		
		if( dwId != NULL_ID )
		{
			ItemProp* pItemProp	= prj.GetItemProp( dwId );
			
			if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
			{
				pElement = pModel->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE);
			}
			
		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
		#ifdef __FINITEITEM0705
			dwId	= m_aEquipInfo[PARTS_HAT].dwId;
		#else	// __FINITEITEM0705
			dwId	= m_adwEquipment[PARTS_HAT];
		#endif	// __FINITEITEM0705
			if( dwId != NULL_ID )
			{
				if( !(m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
				{
					pItemProp	= prj.GetItemProp( dwId );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );

						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
		#endif //__YPARTS_EX									
		}
		else
		{
		#ifdef	__YPARTS_EX
			// 외투의상을 입었을경우 머리날릴것인가의 기준을 외투 모자를 기준으로 바꾼다
		#ifdef __FINITEITEM0705
			dwId	= m_aEquipInfo[PARTS_HAT].dwId;
		#else	// __FINITEITEM0705
			dwId	= m_adwEquipment[PARTS_HAT];
		#endif	// __FINITEITEM0705
			if( dwId != NULL_ID )
			{
				if( !(m_aEquipInfo[PARTS_HAT].byFlag & CItemElem::expired) )
				{					
					pItemProp	= prj.GetItemProp( dwId );
					if( pItemProp && pItemProp->dwBasePartsIgnore != -1 )
					{
						if( pItemProp->dwBasePartsIgnore == PARTS_HEAD )
							((CModelObject*)m_pModel)->SetEffect(PARTS_HAIR, XE_HIDE );
						
						((CModelObject*)m_pModel)->SetEffect(pItemProp->dwBasePartsIgnore, XE_HIDE );
					}
					else
					{
						if( pElement )
							pElement->m_nEffect &= ~XE_HIDE;
					}
				}
			}
		#endif //__YPARTS_EX														
		}
	}
#endif //__YHAIR_HIDE
	
		if( IsMode( TRANSPARENT_MODE ) )		// 투명상태가 아닐때만 렌더.
		{
#ifdef __SCHOOL0701
			if( g_eLocal.GetState( EVE_SCHOOL ) == 1 &&		// 학교 대항전 서버에 접속,
				g_eLocal.GetState( EVE_SCHOOL_BATTLE ) < SBS_START &&	// 대항전 시작 이전,
				!IsAuthHigher( AUTH_GAMEMASTER ) )	// 일반 사용자,
			{
				m_pModel->Render( pd3dDevice, &mWorld ); 
			}
			else
#endif
			{
				m_pModel->SetBlendFactor( 80 );
				if( IsActiveMover() ||		// 자기자신은 반투명으로 출력 ...혹은
					(IsActiveMover() == FALSE && g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER )) )		// 타인인데 플레이어가 겜마면.
					m_pModel->Render( pd3dDevice, &mWorld );		// 반투명으로 출력
				m_pModel->SetBlendFactor( 255 );
			}
		}
		else
		{

#ifdef __YSMOOTH_OBJ
			if( m_bSmooth && m_pModel->m_nNoEffect != 2 )
			{
				m_nSmoothCount += 5;

				if( m_nSmoothCount > 255 )
				{
					m_wBlendFactor = 255;
					m_nSmoothCount = 0;
					m_bSmooth      = FALSE;
				}
				else
					m_wBlendFactor = m_nSmoothCount;
			}
#endif //__YSMOOTH_OBJ
			
			m_pModel->SetBlendFactor(m_wBlendFactor);
			m_pModel->Render( pd3dDevice, &mWorld );	// 일반 상태 출력
		}
//	}
//	else
//	{
//		m_pModel->Render( pd3dDevice, &mWorld ); 		
//		m_pModel->m_dwColor = 0;
		

//	}
#else // __BEAST
		m_pModel->Render( pd3dDevice, &mWorld ); 
#endif	// __BEAST


	if( m_pObjHighlight == this )
	{
	//	m_pObjHighlight = NULL;
		if( m_pModel->m_nNoEffect == 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}
#ifndef __BEAST
		if( IsMode( TRANSPARENT_MODE ) == 0 )		// 투명상태가 아닐때만 렌더.
#endif
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
	
#ifdef __CLIENT
#ifdef __AMAKI
	if( g_pPlayer != this )
	{
		if( m_pRide && m_pActMover->IsFly() )
			m_pRide->Render( pd3dDevice, &mWorld );
	}
#else
	if( IsMode( TRANSPARENT_MODE ) == 0 )		// 투명상태가 아닐때만 렌더.
	{
		D3DXMATRIX mRide = m_matWorld;

		if( m_pRide && m_pActMover->IsFly() )
		{
			m_pRide->m_nNoEffect = m_pModel->m_nNoEffect;
			//D3DXMATRIX mRide = m_matWorld;
			ItemProp* pRideProp = prj.GetItemProp( m_dwRideItemIdx );		// 현재 타고있는 탈것의 프로퍼티.
#ifdef __X0907
			if( pRideProp && pRideProp->dwItemKind3 == IK3_STICK )
			{
				if( pModel->GetMatrixBone(0) )	// 빗자루는 루트 본에 연결시킴.
				{
					D3DXMATRIX mBoundY;
					D3DXMatrixTranslation( &mBoundY, 0, pModel->GetMatrixBone(0)->_42, 0 );
					D3DXMatrixMultiply( &mRide, &m_matWorld, &mBoundY );
				}
			}
 #endif // x0907
			m_pRide->Render( pd3dDevice, &mRide );
			m_pRide->m_nNoEffect = 0;

 #ifdef __XLADOLF
			// 일본판은 일회용이 아니다...나머지국가는 일회용~
			if( pRideProp && pRideProp->dwID == II_RID_RID_BOR_LADOLF || pRideProp->dwID == II_RID_RID_BOR_JLADOLF )
			{
				if( m_pLadolf == NULL )
				{
					m_pLadolf = new CModelObject;
					m_pLadolf->InitDeviceObjects( pd3dDevice );
					m_pLadolf->LoadBone( "mvr_Ladolf.chr" );
					m_pLadolf->LoadElement( "mvr_Ladolf.o3d", 0 );
					m_pLadolf->LoadMotion( "mvr_Ladolf_stand.ani" );
				}

				D3DXMATRIX mLadolf = mRide;
				D3DXVECTOR3 vLocal;

				FLOAT fAngXZ = GetAngle();
				FLOAT fAngH  = GetAngleX();
				AngleToVector( &vLocal, fAngXZ, -fAngH-10.0f, 1.4f );

				// 날기 시작 - 가속중
				if( m_pActMover->GetStateFlag() & OBJSTAF_ACC )
				{
					if( m_dwLadolfFlag == 0 )
					{
						m_pLadolf->LoadMotion( "mvr_Ladolf_walk.ani" );
						m_pLadolf->SetMotionBlending( TRUE );
						m_pLadolf->SetLoop( ANILOOP_LOOP );
						m_dwLadolfFlag = 1;
					}
				}
				else
				{
					if( D3DXVec3LengthSq( &m_pActMover->m_vDelta ) < 0.001f )
					{
						if( m_dwLadolfFlag == 1 )
						{
							m_pLadolf->LoadMotion( "mvr_Ladolf_stand.ani" );
							m_pLadolf->SetMotionBlending( TRUE );
							m_pLadolf->SetLoop( ANILOOP_LOOP );
							m_dwLadolfFlag = 0;
						}
					}
				}
			
				mLadolf._41 += vLocal.x;
				mLadolf._42 += vLocal.y;
				mLadolf._43 += vLocal.z;
				
				m_pLadolf->m_nNoEffect = m_pModel->m_nNoEffect;
				m_pLadolf->Render( pd3dDevice, &mLadolf );
				m_pLadolf->m_nNoEffect = 0;
			}
 #endif //__XLADOLF

		}
	}
#endif

#endif
#ifdef __BEAST
	if( m_respawnInfo.m_dwIndex )
		RenderRect( pd3dDevice, m_respawnInfo.m_rect, RECTCOLOR_MOVER );
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
#ifdef __CLIENT
	// 비행중 전방 xx각도 이내에 들어오는것은 TAB으로 자동 타겟팅.
	if( /*IsPlayer() &&*/ IsActiveMover() == FALSE && g_pPlayer->m_pActMover->IsFly() )
	{
		if( GetProp()->bKillable == 1 && GetProp()->dwFlying == 1 )		// 죽이는게 가능한넘 / 비행몹 만 타겟으로 잡힌다. 
		{
			D3DXVECTOR3	vCamera = g_Neuz.m_camera.m_vLookAt - g_Neuz.m_camera.GetPos();			// 카메라가 보는 정면벡터
			D3DXVECTOR3 vObj	= GetPos() - g_Neuz.m_camera.GetPos();	// 카메라에서 this오브젝트의 벡터.
			D3DXVec3Normalize( &vCamera, &vCamera );		// 노말라이즈.
			D3DXVec3Normalize( &vObj, &vObj );
			FLOAT fDot = D3DXVec3Dot( &vCamera, &vObj );
			if( fDot > cosf(D3DXToRadian(15.0f)) )
			{
				CWndWorld *pWndWorld = (CWndWorld *)g_WndMng.m_pWndWorld;
				if( pWndWorld )
				{
					pWndWorld->AddFlyTarget( GetId() );
				}
			}
		}
	}
	
	// 무버에 기본적으로 달려있는 이펙트. - 이건 죽을때까지 안없어진다.
	if( (m_dwFlag & MVRF_EFFECT1) == 0 )
	{
		CSfx *pSfx = NULL;
		switch( m_dwIndex )
		{
		case MI_RISEM1:
		case MI_RISEM2:
		case MI_RISEM3:
		case MI_RISEM4:
			pSfx = CreateSfx( D3DDEVICE, XI_NPCRISEMSIGN, GetPos(), GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );
			if( pSfx )
				pSfx->SetScale( GetScale() );
		}
		if( pSfx )
			m_dwFlag |= MVRF_EFFECT1;
	}
#ifdef __V050524_QUESTEMOTICON
	if( m_pModel->m_nNoEffect == 0 )
		RenderQuestEmoticon( pd3dDevice );
#endif // __V050524_QUESTEMOTICON


#endif
}

// 이것이 호출될때는 바로 이전에 CMover::Render()가 호출되어야 한다.
void CMover::RenderPartsEffect( LPDIRECT3DDEVICE9 pd3dDevice )
{
#ifndef __BEAST
#ifdef _DEBUG
	if( g_pPlayer == this )
	{
		int a = 0;
	} else
	{
		int a = 0;
	}
#endif

	if( IsMode( TRANSPARENT_MODE ) )		// 투명상태가 아닐때만 렌더.
		return;

	if( !IsVisible() || IsCull() )
		return;

	pd3dDevice->SetMaterial( g_TextureMng.GetMaterial( pd3dDevice, 0 ) );

	D3DXVECTOR3 vPos = GetPos();
	D3DXMATRIX  matWorld;
	
	matWorld = m_matWorld;

	D3DXVECTOR3 vPos2 = GetPos() - GetWorld()->m_pCamera->m_vPos;
	int nLength = D3DXVec3Length( &vPos2 );
	if( nLength > CWorld::m_fFarPlane ) 
		nLength = CWorld::m_fFarPlane;
	if( nLength > CWorld::m_fFarPlane - ( CWorld::m_fFarPlane / 4 ) )
		return;
	//m_pModel->SetBlendFactor( 255 );//255 - nBlendFactor );

	CItemElem *pItemElem = NULL;
	ItemProp *pItemProp = NULL;

	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_RWEAPON );	// 오른쪽 무기에
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} else
	{
#ifdef __FINITEITEM0705
		DWORD dwItemID = m_aEquipInfo[PARTS_RWEAPON].dwId;
#else	// __FINITEITEM0705
		DWORD dwItemID = m_adwEquipment[ PARTS_RWEAPON ];
#endif	// __FINITEITEM0705
		if( dwItemID != NULL_ID )
			pItemProp = prj.GetItemProp( dwItemID );
	}
	if( pItemProp )
	{
/*
		if( pItemProp->nReflect > 0 )	// 리플렉트가 걸려있으면
			((CModelObject*)m_pModel)->SetEffect( PARTS_RWEAPON, XE_REFLECT );	// 리플렉트 옵션으로 렌더.
		switch( pItemProp->dwSfxElemental )
		{
		case ELEMENTAL_FIRE:	
			((CModelObject*)m_pModel)->SetEffect( PARTS_RWEAPON, XE_ITEM_FIRE );	// 불 타는 옵션.
			break;
		case ELEMENTAL_ELEC:
			((CModelObject*)m_pModel)->SetEffect( PARTS_RWEAPON, XE_ITEM_ELEC );	// 전기 옵션.
			break;
#ifdef __XEFFECT1006
		case ELEMENTAL_WATER:
			((CModelObject*)m_pModel)->SetEffect( PARTS_RWEAPON, XE_ITEM_WATER );	// 물 옵션
			break;
		case ELEMENTAL_WIND:
			((CModelObject*)m_pModel)->SetEffect( PARTS_RWEAPON, XE_ITEM_WIND );	// 바람 옵션
			break;
		case ELEMENTAL_EARTH:
			((CModelObject*)m_pModel)->SetEffect( PARTS_RWEAPON, XE_ITEM_EARTH );	// 땅 옵션
			break;
#endif // xEffect1006
		}
*/
	}
	
	if( IsActiveMover() )
	{
		pItemElem = GetEquipItem( PARTS_SHIELD );	// 방패에
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	} else
	{
#ifdef __FINITEITEM0705
		DWORD dwItemID = m_aEquipInfo[PARTS_SHIELD].dwId;	// activemover 가 아닌놈은 가상아이템으로 꺼낸다.
#else	// __FINITEITEM0705
		DWORD dwItemID = m_adwEquipment[ PARTS_SHIELD ];	// activemover 가 아닌놈은 가상아이템으로 꺼낸다.
#endif	// __FINITEITEM0705
		if( dwItemID != NULL_ID )
			pItemProp = prj.GetItemProp( dwItemID );
	}
	if( pItemProp )
	{
		if( pItemProp->nReflect > 0 )	// 리플렉트가 걸려있으면
			((CModelObject*)m_pModel)->SetEffect( PARTS_SHIELD, XE_REFLECT );	// 리플렉트 옵션으로 렌더.
	}
	
	m_pModel->SetGroup( 0 );
	if( IsMode( TRANSPARENT_MODE ) == 0 )		// 투명상태가 아닐때만 렌더.
		m_pModel->RenderEffect( pd3dDevice, &matWorld ); 

	extern CPartsLaser g_Laser;
	D3DXMatrixIdentity( &matWorld );
	g_Laser.Render( pd3dDevice, &matWorld, g_ModelGlobal.m_vCameraPos, g_ModelGlobal.m_vCameraForward );
				

#endif // not beast
}


#ifdef __CLIENT

// nValue ( 1,2,3,4 )
void CMover::RenderGauge( LPDIRECT3DDEVICE9 pd3dDevice, int nValue )
{
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();

    pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

#ifdef __XGAU0520
	vOut.y -= 40;
#else	
	vOut.y -= 45;
#endif

	int nGaugeWidth = 80;
	CPoint point( vOut.x - nGaugeWidth / 2, vOut.y - 3 );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);

	CPoint p = CPoint( rect.left, rect.top );
	LONG   y = 200;

	MakeEven( p.x );

	g_Neuz.m_TextureGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);

	switch( nValue )
	{
		case 4:
			g_Neuz.m_TextureGauge[4].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		case 3:
			g_Neuz.m_TextureGauge[3].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		case 2:
			g_Neuz.m_TextureGauge[2].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
		case 1:
			g_Neuz.m_TextureGauge[1].RenderScal(&(g_Neuz.m_2DRender), p, y, 0.8f, 0.55f);
			break;
		case 0:		// 그리지 않는다.
			break;
		default:
			ASSERT( 0 );
			break;
	}
	
#ifdef __XUZHU
	g_Neuz.m_2DRender.TextOut( 2, 160, GetHitPoint(), D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
#endif
}


// 비행시 터보그래프
void CMover::RenderTurboGauge( LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue )
{
	if( IsMode( TRANSPARENT_MODE ) )
		return;
	if( !IsVisible() )
		return;
	//	pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
    D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
    pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
//	vOut.y -= 40;
	
	CPoint point( vOut.x - 80 / 2, vOut.y - 40 - 3 );		// 게이지 width 100
	CPoint p = CPoint( point.x - 2, point.y - 2 );

	MakeEven( p.x );
	
	g_Neuz.m_TextureTurboGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		// 게이지바 테두리(와꾸?)
	
	//int nHitWidth = nValue * nGaugeWidth / nMaxValue;
	
	int nWidth = g_Neuz.m_TextureTurboGauge[1].m_size.cx * nValue / nMaxValue;
	
	CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureTurboGauge[1].m_size.cy );
	
	if( m_pActMover->GetStateFlag() & OBJSTAF_TURBO )
	{
		if( g_nRenderCnt & 1 )
		{
			g_Neuz.m_TextureTurboGauge[1].Render( &g_Neuz.m_2DRender, p, p2, 200, 0.8f, 0.55f);
		}
		else
		{
			int nWidthScale = (96 * nValue / nMaxValue) * 0.8f;
			CRect rect( p.x + 1, p.y + 1, p.x + nWidthScale, p.y + 6 );
			g_Neuz.m_2DRender.RenderFillRect( rect, 0xffffffff );
		}
		
	} else
		g_Neuz.m_TextureTurboGauge[1].Render( &g_Neuz.m_2DRender, p, p2, 200, 0.8f, 0.55f);

#ifdef __XUZHU
	g_Neuz.m_2DRender.TextOut( p.x + 128, p.y, nValue, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
#endif

	// 속도 표시.

	extern int g_nFlySpeed;
	char szBuff[64];
	int		nLen, i, nIdx;
	sprintf( szBuff, "%d", g_nFlySpeed );
	nLen = strlen( szBuff );

//	vPosHeight = vPos;
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	vPosHeight.y = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	

	// 숫자폰트 찍기.
	p.x = vOut.x + 30;
	p.y = vOut.y + 0;

	MakeEven( p.x );

	for( i = 0; i < nLen; i ++ )
	{
		nIdx = szBuff[i] - '0';
		g_WndMng.m_pWndWorld->m_texFontDigital.GetAt(nIdx)->Render( &g_Neuz.m_2DRender, p );		// 수평 게이지.
		p.x += 16;
		
	}

	g_WndMng.m_pWndWorld->m_texFontDigital.GetAt(10)->Render( &g_Neuz.m_2DRender, p );		// 수평 게이지.
	
	
}
#endif // __CLIENT

#ifdef __V050322_CACHEQUEST
#ifdef __CLIENT
void CMover::RenderQuestEmoticon( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( IsPlayer() || m_bShowQuestEmoticon == FALSE )
		return;
	LPCHARACTER lpCharacter = GetCharacter();

	if( lpCharacter == NULL ) return;

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
	point.x = vOut.x;
	point.y = vOut.y;

	MakeEven( point.x );
	//point.x -= pFont->GetTextExtent( szName ).cx / 2;
	point.y -= 50.0f;

	if( m_nQuestEmoticonIndex  != -1 )	
	{
		
        pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
        pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

		point.x -= 16 * m_fQuestEmoticonScale;
		point.y -= 16 * m_fQuestEmoticonScale;
		g_Neuz.m_texQuestEmoticon.GetAt( m_nQuestEmoticonIndex )->RenderScal(&(g_Neuz.m_2DRender), point, 255, m_fQuestEmoticonScale, m_fQuestEmoticonScale );

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        TRUE );
	}
}
#endif // __CLIENT
#endif // __V050322_CACHEQUEST

void CMover::RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor )
{
	if( !IsVisible() )
		return;
	char szName[ 256 ];
	strcpy( szName, m_szName );
#ifndef __BEAST
	

#ifdef __CLIENT
#ifdef __GUILDCOMBAT
	BOOL  bOtherColor = FALSE;
	DWORD dwNewColor;

	if( IsPlayer() )
	{
		if( GetWorld()->GetID() == WI_WORLD_GUILDWAR )
		{
			if( g_pPlayer->GetGuild() && GetGuild() )
			{
				bOtherColor = TRUE;

				if( g_pPlayer->GetGuild()->GetGuildId() == GetGuild()->GetGuildId() )
				{
					dwNewColor = COLOR_GUILDCOMBAT;		
				}
				else
				{
					dwNewColor = 0xffffffff;							
				}
			}
		}
	}
#endif // __GUILDCOMBAT
#endif // __CLIENT
	

#ifdef __ACROBAT_0504
	/*
	// 다크 일루션 상태면 나 자신이외에는 이름을 출력하지 말자!!!
	if( !IsActiveMover() && m_SkillState.Find( SI_ACR_SUP_DARKILLUSION ) )
	{
		// 파티가 아니다
		if( g_Party.FindMember( m_idPlayer ) == -1 )
		{
			// 소속길드없음
			if( m_idGuild == 0 )
			{
				return;
			}
			// 소속길드있음
			else
			{
				// 같은 길드가 아니면 표시 불가
				if( m_idGuild != g_pPlayer->m_idGuild )
				{
					return;
				}
			}
		}
	}
	*/
#endif //__ACROBAT_0504

	// 명성에 따른 색표시.
	if( IsPlayer() )
	{
#ifdef __NPP_050308
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);

		if( pKarmaProp )
		{
			dwColor = pKarmaProp->dwColor;
			CString strFameName = GetFameName();
			if( strFameName.IsEmpty() == FALSE )
			{
				CString strName;
				strName = "[";
				strName += strFameName;
				strName += "] ";
				strName += m_szName;

				strcpy( szName, (LPCTSTR)strName );
			}		
		}
#else //__NPP_050308		
		if( m_nFame >= 10000 )	
		{
			if( (g_nRenderCnt / 2) & 1 )	// 10000 이상부턴 번쩍번쩍.
				dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );	// 
			else
				dwColor = D3DCOLOR_ARGB( 255, 0, 81, 182 );			// 남청?
		}
		else if( m_nFame >= 4000 )	dwColor = D3DCOLOR_ARGB( 255, 0, 114, 105 );	// 군청?
		else if( m_nFame >= 1000 )	dwColor = D3DCOLOR_ARGB( 255, 48, 48, 255 );	// 파랑
		else if( m_nFame >= 10 )	dwColor = D3DCOLOR_ARGB( 255, 148, 196, 255 );	// 하늘색
		
		CString strFameName = GetFameName();
		if( strFameName.IsEmpty() == FALSE )
		{
			CString strName;
			strName = "[";
			strName += strFameName;
			strName += "] ";
			strName += m_szName;
			strcpy( szName, (LPCTSTR)strName );
		}
#endif //__NPP_050308
	}
	if( g_eLocal.GetState( EVE_18 ) && g_eLocal.GetState( EVE_PK ) )		// 18세 서버면서 PK서버면
	{
#ifdef __NPP_050308

#else //__NPP_050308
		if( IsPlayer() )	// 플레이어 일때만 해당.
		{
			if( IsChaotic() )	// 슬로터 포인트가 있으면 킬수에 따라서 색깔 달리.
			{
				if( m_nNumKill >= 100 )	
				{
					if( (g_nRenderCnt / 2) & 1 )	// 100 이상부턴 번쩍번쩍.
						dwColor = D3DCOLOR_ARGB( 255, 255, 128, 128 );	// 빨강
					else
						dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );	// 빨강
				}
				else if( m_nNumKill >= 3 )	
					dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0 );	// 빨강
				else if( m_nNumKill == 2 )
					dwColor = D3DCOLOR_ARGB( 255, 255, 120, 0 );	// 주황
				else if( m_nNumKill == 1 )
					dwColor = D3DCOLOR_ARGB( 255, 244, 10, 244 );	// 보라

				CString strPKName = GetSlaughterName();
				if( strPKName.IsEmpty() == FALSE )
				{
					CString strName;
					strName = "[";
					strName += strPKName;
					strName += "] ";
					strName += m_szName;
					strcpy( szName, (LPCTSTR)strName );		// 명성치가 높더라도 슬로터면 슬로터의 상태가 우선시되어 출력 된다.
				}
			} else
			{
				if( m_tmCrime )
				{
					DWORD tmTimer = g_tmCurrent - m_tmCrime;
					if( (int)tmTimer < SEC(CRIME_TIME) )		// 범죄 저지른지 30초 이내면 이름 색 바꿈
					{
						int n = SEC(CRIME_TIME-5);
						if( (int)tmTimer > n && ((g_nRenderCnt / 16) & 1) )		// 5초남았을때는 깜빡깜빡.
							return;	
						dwColor = D3DCOLOR_ARGB( 255, 255, 128, 255 );	// 분홍색
					}
				}
			}
		} // player		
#endif //__NPP_050308
	} // 18세
#endif //__BEAST

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
	point.x = vOut.x;
	point.y = vOut.y;

	MakeEven( point.x );
	point.x -= pFont->GetTextExtent( szName ).cx / 2;
	point.y -= 20.0f;
	point.x -= 3;
	
#ifndef __BEAST
	// 특수 캐릭터 이름 출력 
	if( m_dwAuthorization >= AUTH_GAMEMASTER )
	{
		if( ( GetIndex() == MI_CROWNIBLIS || GetIndex() == MI_CROWNSHADE || GetIndex() == MI_CROWNBUBBLE ) )
		{
			strcpy( szName, GetProp()->szName ); 
			point.x = vOut.x;
			point.y = vOut.y;
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szName ).cx / 2;
			point.y -= 20.0f;
			dwColor = 0xffa0a0ff;
		}
		else
		if( GetIndex() == MI_DWARPET || GetIndex() == MI_DWARPETMAS )
		{
			TCHAR szStr[ 64 ];
			dwColor = 0xffa0a0ff;
			sprintf( szStr, "[%s]", GetProp()->szName );
			point.x = vOut.x; point.y = vOut.y;
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szStr ).cx / 2;
			point.y -= 35.0f;
			pFont->DrawText( point.x, point.y, dwColor, szStr );
			point.x = vOut.x; point.y = vOut.y;
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szName ).cx / 2;
			point.y -= 20.0f;
		}
	}
#endif
#ifdef __CLIENT
	if( IsPlayer() )
	{
		if( g_Party.FindMember( m_idPlayer ) != -1 )		// 파티멤버면 색깔 다르게 표시.
			dwColor = COLOR_PARTY;
		
	#ifdef __GUILDCOMBAT
		if( bOtherColor )
			dwColor = dwNewColor;
	#endif //__GUILDCOMBAT
		
		pFont->DrawText( point.x, point.y, dwColor, szName );

#ifdef __CHEER0615
		if( IsMode( EVENT_OLDBOY_MODE ) )
		{
			int nWidth = pFont->GetTextExtent( szName ).cx;
			pFont->DrawText( point.x+nWidth, point.y, D3DCOLOR_XRGB( 0, 200, 0 ), " ★" );
		}
#endif	// __CHEER0615		
	}
	else
#endif
	{
		LPCHARACTER lpCharacter = GetCharacter();
		if( lpCharacter && lpCharacter->m_nStructure != -1 ) 
		{
			LPSTRUCTURE lpStr = &prj.m_aStructure[ lpCharacter->m_nStructure ];
			TCHAR szStr[ 64 ];
			sprintf( szStr, "[%s]", lpStr->szName );
			point.x = vOut.x; point.y = vOut.y;
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szStr ).cx / 2;
			point.y -= 35.0f;
			pFont->DrawText( point.x, point.y, COLOR_NPC, szStr );
			point.x = vOut.x; point.y = vOut.y;
			MakeEven( point.x );
			point.x -= pFont->GetTextExtent( szName ).cx / 2;
			point.y -= 20.0f;
		}
		//sprintf( szNameTemp, "NPC %s", m_szName );
		//pFont->DrawText( vOut.x + 1, vOut.y + 1, 0xff000000, szName );
#		ifdef __BEAST
		pFont->DrawText( point.x, point.y, COLOR_NPC, szName );
#		else
		if( IsPeaceful() )
		{
			pFont->DrawText( point.x, point.y, COLOR_NPC, szName );
		}
		else
		{
			if( m_bActiveAttack )	// 선공 몬스터는 깜빡거린다.
			{
#ifdef __S0810_NOTMONNAME
				if( g_Option.m_nMonName )
				{
#endif // __S0810_NOTMONNAME

					pFont->DrawText( point.x, point.y, 0xffff0000, szName );
#ifdef __S0810_NOTMONNAME
				}
#endif // __S0810_NOTMONNAME
					
			} else
			{
				//DWORD dwColor = ( COLOR_MONSTER & 0x00ffffff ) | ( m_nAlphaName << 24 );
				//if( IsStill)
#ifdef __S0810_NOTMONNAME
				if( g_Option.m_nMonName )
#endif // __S0810_NOTMONNAME
				pFont->DrawText( point.x, point.y, COLOR_MONSTER, szName );
			}
		}
#		endif
	}
#ifndef __BEAST
	// 플레이어가 겜마면 디버깅 정보를 보여준다.
	if( GetWorld()->m_bViewIdState )
	{
		char szBuff[256];
		CString strName;
		CMover *pAttacker = prj.GetMover( m_idAttacker );
		if( pAttacker )
			strName = pAttacker->m_szName;
		sprintf( szBuff, "0x%x %08x, %d %d, %d(%s)", m_pActMover->GetStateFlag(), m_pActMover->GetState(), GetId(), m_pActMover->m_bGround, (int)m_idAttacker, strName );
		pFont->DrawText( vOut.x, vOut.y - 50, dwColor, szBuff );
		
	}
#endif // not beast
#ifdef __XUZHU
	if( GetAdjParam( DST_CHRSTATE ) & CHS_STUN )
		pFont->DrawText( point.x, point.y - 20, dwColor, "스턴" );
	if( GetAdjParam( DST_CHRSTATE ) & CHS_POISON )
		pFont->DrawText( point.x + 40, point.y - 20, dwColor, "독" );
	if( GetAdjParam( DST_CHRSTATE ) & CHS_DARK )
		pFont->DrawText( point.x + 80, point.y - 20, dwColor, "암흑" );
	if( GetAdjParam( DST_CHRSTATE ) & CHS_BLEEDING )
		pFont->DrawText( point.x + 100, point.y - 20, dwColor, "출혈" );
#endif // 
	

// Render Guild Name & Logo
	CGuild* pGuild = GetGuild();	
	if( pGuild )
	{
		point.x = vOut.x;
		point.y = vOut.y;
		MakeEven( point.x );	
		point.x -= pFont->GetTextExtent( pGuild->m_szGuild ).cx / 2;
		point.y -= 32.0f;
		point.x -= 3;
		
#ifdef __NPP_050308
		KarmaProp* pKarmaProp = prj.GetKarmaProp(m_nSlaughter);

		if( pKarmaProp )
		{
			dwColor = pKarmaProp->dwColor;
		}

	#ifdef __GUILDCOMBAT
		if( bOtherColor )
			dwColor = dwNewColor;
	#endif //__GUILDCOMBAT		

		pFont->DrawText( point.x, point.y, dwColor, pGuild->m_szGuild );	

#else //__NPP_050308	
		
		pFont->DrawText( point.x, point.y, 0xa876f3ff, pGuild->m_szGuild );

#endif //__NPP_050308
		
#ifndef __BEAST
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
		
		if( pGuild->m_dwLogo != 0 )
		{
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );

			int nLogoSizey = pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cy+2;
		
			point.x = vOut.x;
			point.y = vOut.y;
			
			MakeEven( point.x );			

			if( strlen(szName) > strlen(pGuild->m_szGuild) )
			{
				point.x -= pFont->GetTextExtent( szName ).cx / 2;
				point.y -= nLogoSizey;
			}
			else
			{
				point.x -= pFont->GetTextExtent( pGuild->m_szGuild ).cx / 2;
				point.y -= nLogoSizey;
			}
			
			point.x -= pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].m_size.cx+2;
			pWndWorld->m_pTextureLogo[pGuild->m_dwLogo-1].Render( &g_Neuz.m_2DRender, point, 255 );
			
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			
		}
#endif // __BEAST
		
#ifdef __NPP_050308
		if( IsPlayer() && !IsActiveMover() &&
			g_Neuz.m_NeuzEnemy.IsPKing( GetId() ) )
		{
			if( (m_dwFlag & MVRF_WARMARK) == 0 )	// 전쟁마크가 아직 안떠있는 상태면
			{
				RenderFlag( ENEMY_PK );		// 생성 전투 SFX
			}
		}
		else
#endif //__NPP_050308

#ifdef __CLIENT
		// 길드전시에 머리위에 뜨는 전투마크
		if( g_eLocal.GetState( EVE_GUILDWAR ) == 1 )		// 길드전 가능한 서버인가.
		{
			if( m_idWar && g_pPlayer->m_idWar == m_idWar )	// 나랑 같은 전쟁에 참가중인 놈인가.
			{
				if( m_idGuild != g_pPlayer->m_idGuild )		// 나랑 다른길드인가(동맹인경우는 이걸로 안된다.)
				{
					if( (m_dwFlag & MVRF_WARMARK) == 0 )	// 전쟁마크가 아직 안떠있는 상태면
					{
#ifdef __NPP_050308
						RenderFlag( ENEMY_WAR );
#else // __NPP_050308
						RenderFlag();
#endif // __NPP_050308
					}
				} 
			}
		}
#endif // CLIENT

		
	}




}
#ifndef __BEAST

// 전투교전중인 상태 표시
#ifdef __NPP_050308
void CMover::RenderFlag( int nType )
#else // __NPP_050308
void CMover::RenderFlag()
#endif // __NPP_050308
{
	// 결투상대라는 sfx를 머리위에 띄워줌.
	CSfx *pSfx = CreateSfx( D3DDEVICE, XI_GEN_PVP_FLAG01, GetPos(), GetId(), D3DXVECTOR3(0,0,0), NULL_ID, -1 );
	if( pSfx )
	{
		pSfx->SetScale( D3DXVECTOR3(1.5f, 1.5f, 1.5f) );
#ifdef __NPP_050308
		((CSfxDuelParty*)pSfx)->m_nType = nType;
#else // __NPP_050308
		((CSfxDuelParty*)pSfx)->m_nType = 1;
#endif // __NPP_050308
		m_dwFlag |= MVRF_WARMARK;
	}
}
	
void CMover::RenderHP(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( !IsVisible() )
		return;
	if( IsMode( TRANSPARENT_MODE ) )
		return;
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	vOut.y -= 34;
#ifdef __XGAU0520	
	int nGaugeWidth = 80;
#else
	int nGaugeWidth = 100;
#endif
	CPoint point( vOut.x - nGaugeWidth / 2, vOut.y - 3 );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
	int nHitPercent = GetHitPointPercent( 100 );

	MakeEven( p.x );

#ifdef __X1022
	if( nHitPercent < CRITICAL_BERSERK_HP )
	{
//		float w = p2.x * 0.8f;
//		float h = p2.y * 0.45f;
//		CRect rect( p.x, p.y, p.x + w, p.y + h );
//		g_Neuz.m_2DRender.RenderFillRect( rect, 0xffffffff );
		if( g_nRenderCnt & 1 )
			g_Neuz.m_TextureHPGauge[0].RenderScal2(&(g_Neuz.m_2DRender), p, 200, 0.7f, 0.45f, 0xffffffff );		
		else
			g_Neuz.m_TextureHPGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	}
	else
#endif // x1022
		g_Neuz.m_TextureHPGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	
	int nWidth = g_Neuz.m_TextureHPGauge[1].m_size.cx * GetHitPoint() / GetMaxHitPoint();
	
	CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy );
#ifdef __XGAU0520
#else
	p.x += 14;
#endif
	
#ifndef __X1022
	if( nHitPercent < CRITICAL_BERSERK_HP )
	{
		if( g_nRenderCnt & 1 )
			g_Neuz.m_TextureHPGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);		
		
	} else
#endif // not x1022
	{
		g_Neuz.m_TextureHPGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 200, 0.8f, 0.55f);		
	}
}

#ifdef __S0504_FLY
void CMover::RenderCasting(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if( g_pPlayer->IsStateMode( STATE_BASEMOTION_MODE ) == FALSE )
		return;
	if( g_pPlayer->m_nReadyTime == 0 )
		return;
	if( !IsVisible() )
		return;
	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	// 월드 좌표를 스크린 좌표로 프로젝션 한다.
	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	
	pd3dDevice->GetViewport( &vp );
	vp.X = 0;
	vp.Y = 0;
	
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matScale );
	D3DXMatrixMultiply( &matWorld, &matWorld, &m_matRotation );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
	
	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;
	
	D3DXVec3Project( &vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);
	
	vOut.y -= 48;
	int nGaugeWidth = 80;

	CPoint point( vOut.x - nGaugeWidth / 2, vOut.y - 3 );
	CRect rect( point.x - 2, point.y - 2, point.x + nGaugeWidth + 2, point.y + 5 + 2);
	
	CPoint p = CPoint( rect.left, rect.top );
		
	MakeEven( p.x );
	
//	if( (g_nRenderCnt / 16) & 1 )
//		g_Neuz.m_TextureHPGauge[0].RenderScal2(&(g_Neuz.m_2DRender), p, 200, 0.7f, 0.45f, 0xffffffff );		
//	else
		g_Neuz.m_TextureCastingGauge[0].RenderScal(&(g_Neuz.m_2DRender), p, 200, 0.8f, 0.55f);		
	
	ItemProp* pItemProp = prj.GetItemProp( g_pPlayer->m_dwUseItemId );
	if( pItemProp )
	{
		DWORD dwTime = timeGetTime() - g_pPlayer->m_nReadyTime;
		int nWidth = g_Neuz.m_TextureHPGauge[1].m_size.cx * ( (float)dwTime / (float)pItemProp->dwSkillReadyType );
		if( 112 < nWidth )
			nWidth = 112;
		if( nWidth < 0 )
			nWidth = 0;

		CPoint p2 = CPoint( nWidth, g_Neuz.m_TextureHPGauge[1].m_size.cy );
	
//		if( (g_nRenderCnt / 16) & 1 )
			g_Neuz.m_TextureCastingGauge[1].Render(&(g_Neuz.m_2DRender), p, p2, 255, 0.8f, 0.55f);		
	}
}
#endif // __S0504_FLY

#ifdef __NPP_050308
void CMover::RenderPVPCount(LPDIRECT3DDEVICE9 pd3dDevice)
{
	DWORD dwWidth = g_Neuz.m_d3dsdBackBuffer.Width / 2;
	DWORD dwHeight = g_Neuz.m_d3dsdBackBuffer.Height / 2;
	
	if( g_Neuz.m_dwPVPTime <= GetTickCount() && GetTickCount() <= g_Neuz.m_dwPVPTime + 1500 )
	{
		if( g_Neuz.m_nPVPCount == 0 )
			g_Neuz.m_nPVPCount = 10;
		
		float fScal = GetTickCount() - g_Neuz.m_dwPVPTime;
		fScal = fScal / 10.0f;
		int nAlpha = 180;
		if( 150 < nAlpha )
		if( fScal < 25 )
		{
			fScal = 1.0f;
		}
		else
		{
			fScal /= 10;
			fScal -= 1.5f;
			nAlpha -= fScal * 10;
		}

		if( 1.5f < fScal )
		{
			if( g_Neuz.m_nPVPCount == 0 )
				fScal = 1.5f;
			else if( 2.0f < fScal )
				fScal = 2.0f;
		}

		TRACE( "fScal : %f\n", fScal );

		CPoint p = CPoint( 
					dwWidth - ( g_Neuz.m_TexturePackPVP.m_ap2DTexture[g_Neuz.m_nPVPCount].m_size.cx * fScal / 2 ),
					dwHeight - ( g_Neuz.m_TexturePackPVP.m_ap2DTexture[g_Neuz.m_nPVPCount].m_size.cy * fScal / 2 ) - 50
					);
		 
		MakeEven( p.x );
		g_Neuz.m_TexturePackPVP.m_ap2DTexture[g_Neuz.m_nPVPCount].RenderScal(&(g_Neuz.m_2DRender), p, nAlpha, fScal, fScal);			
	}
	
	
}
#endif // __NPP_050308

#endif // __BEAST



#endif // not worldserver