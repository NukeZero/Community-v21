#include "stdafx.h"
#include "defineText.h"
#include "ModelObject.h"
#include "authorization.h"
#include "CreateObj.h"
#include "lang.h"
#include "party.h"
#include "guild.h"
#include "guildwar.h"

#ifdef __CLIENT
	#include "../Neuz/DPClient.h"
	#include "DialogMsg.h"
	extern	CDPClient	g_DPlay;
	#include "GuildRender.h"
	#include "defineSound.h"
#endif	// __CLIENT

#ifdef	__WORLDSERVER
	#include "../WorldServer/User.h"
	#include "WorldMng.h"

	extern	CWorldMng			g_WorldMng;
#endif	// __WORLDSERVER

#include "accessory.h"		//sun: 11, 채집 시스템

extern	CPartyMng		g_PartyMng;
extern	CGuildMng		g_GuildMng;
extern	CGuildWarMng	g_GuildWarMng;

static const TCHAR	LOG_PARTY_ERROR[]	= _T( "PartyError" );


BYTE  nMaleHairColor[10][3] = { 
	{ 144,  99,  101 },
	{ 100, 107, 125 },
	{ 158,119,109 },
	{ 199,156,100 },
	{ 100,123,117 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

BYTE  nFeMaleHairColor[10][3] = { 
	{ 250,250,90 },
	{ 250,250,90 },
	{ 147,118,103 },
	{ 150, 100, 85 },
	{ 213,130,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

#define TEX_PART_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair01.dds"  ) : _T( "Part_femaleHair01.dds"  ) )
#define TEX_PART_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper01.dds" ) : _T( "Part_femaleUpper01.dds" ) )
#define TEX_PART_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower01.dds" ) : _T( "Part_femaleLower01.dds" ) )

#define PARTSMESH_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.o3d" ) : _T( "Part_femaleHair%02d.o3d" ) )
#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )
#define PARTSMESH_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper.o3d"    ) : _T( "Part_femaleUpper.o3d"    ) )
#define PARTSMESH_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower.o3d"    ) : _T( "Part_femaleLower.o3d"    ) )
#define PARTSMESH_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.o3d"     ) : _T( "Part_femaleHand.o3d"     ) )
#define PARTSMESH_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.o3d"     ) : _T( "Part_femaleFoot.o3d"     ) )

#define PARTSTEX_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.dds"  ) : _T( "Part_femaleHair%02d.dds"  ) )
#define PARTSTEX_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper%02d.dds" ) : _T( "Part_femaleUpper%02d.dds" ) )
#define PARTSTEX_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower%02d.dds" ) : _T( "Part_femaleLower%02d.dds" ) )
#define PARTSTEX_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.dds"      ) : _T( "Part_femaleHand.dds"      ) )
#define PARTSTEX_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.dds"      ) : _T( "Part_femaleFoot.dds"      ) )

int GetSelfDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_FIRE;
		case SAI79::WATER:
			return DST_RESIST_WATER;
		case SAI79::WIND:
			return DST_RESIST_WIND; 
		case SAI79::ELECTRICITY:
			return DST_RESIST_ELECTRICITY;
		case SAI79::EARTH:
			return DST_RESIST_EARTH;
	}
	return DST_RESIST_FIRE;
}

int GetStrongDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_WIND;
		case SAI79::WATER:
			return DST_RESIST_FIRE;
		case SAI79::WIND:
			return DST_RESIST_EARTH;
		case SAI79::ELECTRICITY:
			return DST_RESIST_WATER;
		case SAI79::EARTH:
			return DST_RESIST_ELECTRICITY;
	}
	return DST_RESIST_FIRE;
}

int GetWeakDST( BYTE type )
{
	switch( type )
	{
		case SAI79::FIRE:
			return DST_RESIST_WATER;
		case SAI79::WATER:
			return DST_RESIST_ELECTRICITY;
		case SAI79::WIND:
			return DST_RESIST_FIRE;
		case SAI79::ELECTRICITY:
			return DST_RESIST_EARTH;
		case SAI79::EARTH:
			return DST_RESIST_WIND;
	}
	return DST_RESIST_FIRE;
}

// 머리카락 모양과 색 바꾸기  
void CMover::SetHair( int nHair )
{
	if( nHair >= 0 && nHair < MAX_HAIR )
	{
		m_dwHairMesh = nHair;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ] = { 0, };
		// Hair
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HAIR( GetSex() ), m_dwHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_HAIR( GetSex() ), 0 /*nHairColor*/ + 1 );
		pModel->ChangeTexture( PARTS_HAIR, TEX_PART_HAIR( GetSex() ), lpszTemp );
	}
}

void  CMover::SetHairColor( FLOAT r, FLOAT g, FLOAT b )
{ 
#ifndef __Y_HAIR_BUG_FIX	
	if( ( r == 1.0f && g == 1.0f && b == 1.0f ) || 
		( fabs( r - 1.0f ) < 0.001f && fabs( g - 1.0f ) < 0.001f && fabs( b - 1.0f ) < 0.001f ) )
	{
		if( GetSex() == SEX_MALE )
		{
			m_fHairColorR = (nMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nMaleHairColor[m_dwHairMesh][2])/255.f;
		}
		else
		{
			m_fHairColorR = (nFeMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nFeMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nFeMaleHairColor[m_dwHairMesh][2])/255.f;
		}
	}
	else
#endif //__Y_HAIR_BUG_FIX
	{
#ifdef __Y_HAIR_BUG_FIX
		m_fHairColorR = ( r < 0.0f ) ? 0.0f : r;
		m_fHairColorG = ( g < 0.0f ) ? 0.0f : g;
		m_fHairColorB = ( b < 0.0f ) ? 0.0f : b;
#else //__Y_HAIR_BUG_FIX
		m_fHairColorR = ( r < 0.3f ) ? 0.3f : r;
		m_fHairColorG = ( g < 0.3f ) ? 0.3f : g;
		m_fHairColorB = ( b < 0.3f ) ? 0.3f : b;
#endif //__Y_HAIR_BUG_FIX
	}
	
	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}


void CMover::SetHairColor( DWORD dwHairColor )
{
	m_dwHairColor = dwHairColor;
	m_fHairColorR = ((dwHairColor>>16) & 0xff) / 255.f;
	m_fHairColorG = ((dwHairColor>>8) & 0xff)  / 255.f;
	m_fHairColorB = ((dwHairColor) & 0xff)     / 255.f;

#ifndef __Y_HAIR_BUG_FIX
	if( ( m_fHairColorR == 1.0f && m_fHairColorG == 1.0f && m_fHairColorB == 1.0f ) || 
		( fabs( m_fHairColorR - 1.0f ) < 0.001f && fabs( m_fHairColorG - 1.0f ) < 0.001f && fabs( m_fHairColorB - 1.0f ) < 0.001f ) )
	{
		if( GetSex() == SEX_MALE )
		{
			m_fHairColorR = (nMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nMaleHairColor[m_dwHairMesh][2])/255.f;
		}
		else
		{
			m_fHairColorR = (nFeMaleHairColor[m_dwHairMesh][0])/255.f;
			m_fHairColorG = (nFeMaleHairColor[m_dwHairMesh][1])/255.f;
			m_fHairColorB = (nFeMaleHairColor[m_dwHairMesh][2])/255.f;
		}

	}
#endif //__Y_HAIR_BUG_FIX

	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}

// 표정 바꾸기 
void CMover::SetHead( int nHead )
{
	if( nHead < MAX_HEAD )
	{
		m_dwHeadMesh = nHead;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ] = { 0, };
		// Hair
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HEAD( GetSex() ), m_dwHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
}
void CMover::SetSkinSet( int nSkinSet )
{
	if( nSkinSet < 2 )
	{
		m_dwSkinSet = nSkinSet;
#ifdef __CLIENT
		UpdateParts();
#endif
	}
}


// 이 함수는 전체 의상을 새로 완성하도록 한다.
// 보통 이 함수는 처음에 캐릭터를 생성할 때 한번 호출한다.
void CMover::UpdateParts( BOOL bFakeParts )
{
	UpdateParts( GetSex(), m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHeadMesh, 
		m_aEquipInfo, (CModelObject*)m_pModel, bFakeParts ? NULL : &m_Inventory, GetProp()->bIfParts, this );
	((CModelObject*)m_pModel)->RestoreDeviceObjects();
	UpdateBoundBox();
	UpdateParam();
}

PT_ITEM_SPEC GetInventoryProp( PEQUIP_INFO pEquipInfo, CItemContainer< FLItemElem  >* pInventory, int nParts )
{
	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp = NULL;

	if( pInventory == NULL ) // fake only
	{
		if( pEquipInfo[nParts].dwItemID != 0 ) 
			pItemProp = g_xSpecManager->GetSpecItem( pEquipInfo[nParts].dwItemID );
	}
	else
	{
		pItemElem = pInventory->GetEquip( nParts );
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}

	return pItemProp;
}

void CMover::UpdateParts( int nSex, int /*nSkinSet*/, int /*nFace*/, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< FLItemElem  >* pInventory, BOOL bIfParts, CMover* pMover )
{
	if( pModel == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "pModel is NULL" ) );
		return;
	}
	// 기본 부위(속살, 헤어, 등등)을 세팅한다.
	//LPSKINSET lpSkinset = prj.m_PartsMng.GetSkinSet( nSex, nSkinSet );
	//LPCTSTR lpHair = prj.m_PartsMng.GetHairMeshName( nSex, nHairMesh );
	TCHAR lpszTemp[ 64 ] = { 0, };
	if( bIfParts )
	{
		// Hair
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HAIR( nSex ), nHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		// Head
		FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSMESH_HEAD( nSex ), nHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
	// 먼저 익스크루시브 오브젝트를 TakeOff한다.
	BOOL abExclusiveParts[ MAX_HUMAN_PARTS ];
	ZeroMemory( abExclusiveParts, sizeof( abExclusiveParts ) );
#ifdef __CLIENT
		if(pMover)
			pMover->m_bExclusiveLowBody = false;
#endif
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
//		DWORD dwParts = -1;
//		FLItemElem* pItemElem = NULL;
		PT_ITEM_SPEC pItemProp = NULL;
		// 프로퍼티 꺼냄
		pItemProp = GetInventoryProp( pEquipInfo, pInventory, i );
		if( pItemProp )
		{
			if( pInventory )
			{
				FLItemElem* pItemElem	= pInventory->GetAt( i );
				if( pItemElem->IsFlag( FLItemElem::expired ) )
					pItemProp	= NULL;
			}
			else
			{
				if( pEquipInfo[i].byFlag & FLItemElem::expired )
					pItemProp	= NULL;
			}
		}

		if( pItemProp && pItemProp->dwExclusive != NULL_ID )
		{
			
#ifdef __CLIENT
		// 익스클루시브가 있을때
		if( i == PARTS_CLOTH && pMover)
			pMover->m_bExclusiveLowBody = true;
#endif
			pModel->TakeOffParts( pItemProp->dwExclusive );	// 익스클루시브를 제거
			abExclusiveParts[ pItemProp->dwExclusive ] = TRUE;		// 익스클루시브 된넘을 기억.
		}
	}
	// 장착부위를 세팅한다.
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
//		DWORD dwParts = -1;
		FLItemElem* pItemElem = NULL;
		PT_ITEM_SPEC pItemProp = NULL;
		if( pInventory == NULL ) // fake only
		{
			if( pEquipInfo[i].dwItemID != 0 ) 
			{
				if( ( pEquipInfo[i].byFlag & FLItemElem::expired ) != FLItemElem::expired )
				{
					//외형변경 처리
					DWORD dwIndex = pEquipInfo[i].dwItemID;
				
					if( 0 != pEquipInfo[i].dwLooksChangeItemID )
						dwIndex = pEquipInfo[i].dwLooksChangeItemID;

					pItemProp	= g_xSpecManager->GetSpecItem( dwIndex );
				}
			}
		}
		else
		{
			pItemElem = pInventory->GetEquip( i );//AtId( padwEquipment[ i ] );
			if( pItemElem )
			{
				if( !pItemElem->IsFlag( FLItemElem::expired ) )
				{
					//외형변경 처리
					if( pItemElem->GetLooksChangeItemID( ) != 0 )
						pItemProp	= g_xSpecManager->GetSpecItem( pItemElem->GetLooksChangeItemID( ) );
					else
						pItemProp = pItemElem->GetProp();
				}
			}
		}
		TCHAR szPartsName[ 32 ];

		// 각 파츠의 o3d를 로딩.
		if( pItemProp )
		{
			if( pItemProp->bPartsFile == TRUE )
				prj.m_modelMng.MakePartsName( szPartsName, _countof( szPartsName ), _T( "part" ), pItemProp->dwID, nSex );
			else
				prj.m_modelMng.MakeModelName( szPartsName, _countof( szPartsName ), OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
			if( bIfParts || ( i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD ) )
			{
				// 날개를 망토에 부착할려고 시도한 코드		2016.07.11
				//if ( pItemProp->dwItemKind3 != IK3_WING )			// 비행체 말고 다른 파트에 날개가 있으면 출력안시킴	05
					pModel->LoadElement( szPartsName, i /*pItemProp->dwParts*/ );
			}
		}


		if( abExclusiveParts[ i ] == FALSE )
		{
			switch( i )
			{
			case PARTS_RWEAPON: 
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
				{
					switch( pItemProp->dwItemKind3 )
					{
					case IK3_KNUCKLEHAMMER:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
						break;

					case IK3_BOW:
#ifdef __ADDWEAPON_CROSSBOW16
						if( pItemProp->nDestData1[0] == 100 )
							((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						else
							((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#else
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#endif

						break;
					case IK3_YOYO:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						pModel->LoadElement( szPartsName, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
						break;

#ifdef __ADDWEAPON_CROSSBOW16
					case IK3_CROSSBOW:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						break;
#endif //__ADDWEAPON_CROSSBOW16

					default:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					}
//					if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
//						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
//					else
//						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_LWEAPON: 
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
				{
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_SHIELD: 
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
					((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_UPPER_BODY:
				if( bIfParts )
					if( pItemProp == NULL )	
						pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				break;
			case PARTS_LOWER_BODY:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				break;
			case PARTS_HAND:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				if( bIfParts )
					if( pItemProp == NULL ) 
						pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			default:
				if( bIfParts )
				{
					if( pItemProp == NULL ) 
						pModel->TakeOffParts( i );
				}

			}
		}
	}
}

void CMover::SetDefaultLowerBody(CModelObject* pModel)
{
	if( pModel != NULL )
		pModel->LoadElement( PARTSMESH_LOWER( m_bySex ), PARTS_LOWER_BODY );
}
// 장비를 장착한다. 여기서 의상 교체 작업을 해준다.
// nOption은 Fake아이템 장착때만 쓰인다. - xuzhu -
BOOL CMover::DoEquip( int nSex, int nSkinSet, 
					  FLItemElem* pItemElem, int nPart, const EQUIP_INFO & rEquipInfo, CItemContainer< FLItemElem  >* pInventory, 
					  PEQUIP_INFO pEquipInfo, CModelObject* pModel, BOOL bEquip, CMover *pMover ) 
{
	PT_ITEM_SPEC pItemProp = pItemElem ? pItemElem->GetProp() : g_xSpecManager->GetSpecItem( rEquipInfo.dwItemID );

	DWORD dwIndex = pItemElem ? pItemElem->m_dwObjIndex : 0;
	DWORD dwParts = pItemProp->dwParts;
	BOOL bIfParts = pMover ? pMover->GetProp()->bIfParts : 0;
	TCHAR lpszTemp[ 64 ];
//	BOOL bFake = (pInventory == NULL) ? TRUE : FALSE;
	if( dwParts == NULL_ID )
		return FALSE;

	// 반지/귀걸이 벗을때 처리.
	if( !bEquip )
	{
		if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
		{
			FLItemElem *pItemElemTemp;
			
			if( pInventory )
			{
				pItemElemTemp = (FLItemElem*)pInventory->GetEquip( dwParts );
				if( pItemElemTemp != pItemElem )
					dwParts = pItemProp->dwPartsub;
			}
		}
	}
	// 쌍칼(이도류) 처리.
	if( pItemProp && pItemProp->dwHanded == HD_ONE && pItemProp->dwParts == PARTS_RWEAPON )	// 원핸드 무기를 차려고 하는가?
	{
		if( bEquip )
		{
			// 디폴트는 장착위치 오른손
			// 왼손에 무기가 가야하는 상황을 검사
			PT_ITEM_SPEC pProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_RWEAPON );
//sun: 10차 전승시스템	Neuz, World, Trans

			if( pMover->GetJob() == JOB_BLADE || pMover->GetJob() == JOB_BLADE_MASTER ||
				pMover->GetJob() == JOB_BLADE_HERO || pMover->GetJob() == JOB_STORMBLADE_HERO )	// 쌍칼속성
			{
				if( pItemProp->dwID != ITEM_INDEX( 6323, II_WEA_KNU_ISHOFIST ) )
				{
					if( pProp && pProp->dwHanded == HD_ONE )	// 오른손에 무기가 있냐? 그 무기가 원핸드냐
					{
						if( pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_SHIELD ) == NULL )	//왼손에 방패없냐?
							dwParts = PARTS_LWEAPON;		// 장착위치를 왼손으로 바꿈
					} 
				}
			}

			if( pProp && pProp->dwItemKind3 == IK3_YOYO )		// 오른손에 무기가 요요였으면
				pModel->TakeOffParts( PARTS_LWEAPON );			// 왼손에 요요모델도 삭제함. 오른손모델은 밑에서 자동으로 
			
		} 
		else
		{
			//////////////////////////////////////////////////////////////////////////
			if( pItemElem != NULL && pInventory != NULL )
			{
				if( pItemElem->m_dwObjIndex >= pInventory->GetSize() )
				{
					dwParts = pItemElem->m_dwObjIndex - pInventory->GetSize();
				}
				else
				{
					dwParts = pItemElem->m_dwObjIndex;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			// 해제시도.
//			if( nPart >= 0  )		// 파츠를 직접 지정했을때만
//				dwParts = nPart;	// 해제하라고 한 부위를 해제시킨다.
		}
	} // 쌍칼 처리.
	
#ifndef __WORLDSERVER	// <<<< 월드에서 처리안하면 메모리 리크 나지 않나? -xuzhu-
	((CModelObject*)pModel)->TakeOffParts( dwParts );	
#endif

	if( pItemProp && pItemProp->dwItemKind3 == IK3_YOYO )	// 벗으려던 무기가 요요였으면
		pModel->TakeOffParts( PARTS_LWEAPON );		// 왼손에 있던 요요모델도 없애준다.

	if( bEquip )	// 장착하려 할때만...
	{
		PT_ITEM_SPEC pHandItemProp	= NULL;
		PT_ITEM_SPEC pLHandItemProp = NULL;

		// 들고있는 무기 프로퍼티 꺼냄.
		pHandItemProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_RWEAPON );		// 오른손 무기 프로퍼티.
		pLHandItemProp = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_LWEAPON );		// 왼손 무기 프로퍼티.
		
		// 조건검사.
		if( pItemProp->dwItemKind3 == IK3_SHIELD || pItemProp->dwItemKind3 == IK3_ZEMBARUNA
			|| pItemProp->dwItemKind3 == IK3_MAGICBARUNA || pItemProp->dwItemKind3 == IK3_SHILDBARUNA )	// 방패류를 착용하려 했을때
		{
			if( pHandItemProp && pHandItemProp->dwHanded == HD_TWO )		// 손에 쥐고 있는게 투핸드면.
			{
				if( pInventory )
				{
					FLItemElem *pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	
						return FALSE;
#ifdef __WORLDSERVER
					// 투핸드 무기벗김
					if( pItemElemOld )
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
//sun: 9,10차 제련 __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, PARTS_RWEAPON );

#endif // worldserver
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				} else
				{
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				}
				pModel->TakeOffParts( PARTS_RWEAPON );
				if( pHandItemProp->dwItemKind3 == IK3_YOYO )	// 들고 있던 무기가 요요였으면
					pModel->TakeOffParts( PARTS_LWEAPON );		// 왼손에 있던 요요모델도 없애준다.
			}
			else
			if( pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON )	// 왼손에 무기가 쥐어져있으면. 왼손에 드는 무기도 PARTS이름은 모두 RWEAPON이다.
			{
				if( pInventory )
				{
					FLItemElem *pItemElemOld = pInventory->GetEquip( PARTS_LWEAPON );
					if( pInventory->UnEquip( PARTS_LWEAPON ) == FALSE )	
						return FALSE;
#ifdef __WORLDSERVER
					// 왼손 무기벗김
					if( pItemElemOld )
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, PARTS_LWEAPON, pItemElemOld, FALSE );
					}
#endif // worldserver
					if( pEquipInfo )
						pEquipInfo[PARTS_LWEAPON].dwItemID	= 0;
				}
				else
				{
					if( pEquipInfo )
						pEquipInfo[PARTS_LWEAPON].dwItemID	= 0;
				}
				pModel->TakeOffParts( PARTS_LWEAPON );
			}
		} // IK3_SHIELD
		if( pItemProp->dwHanded == HD_TWO )		// 장착하려는 무기가 투핸드 였을때
		{
			if( pHandItemProp && pHandItemProp->dwItemKind3 == IK3_YOYO )	// 들고 있던 무기가 요요였으면
				pModel->TakeOffParts( PARTS_LWEAPON );		// 왼손에 있던 요요모델도 없애준다.
			if( pInventory == NULL )
			{
				if( pEquipInfo[PARTS_SHIELD].dwItemID != 0 )		// 방패가 있었는가 검사해서 있으면 벗김.
				{
					pModel->TakeOffParts( PARTS_SHIELD );
					if( pEquipInfo )
						pEquipInfo[PARTS_SHIELD].dwItemID	= 0;
				}
				if( pEquipInfo[PARTS_RWEAPON].dwItemID != 0 )		// 무기가 있었는가 검사해서 벗김.
				{
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				}
			}
			else
			{
				FLItemElem *pItemElemOld = pInventory->GetEquip( PARTS_SHIELD );
				if( pItemElemOld )
				{
					if( pInventory->UnEquip( PARTS_SHIELD ) == FALSE )	// 방패 벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
#ifdef __WORLDSERVER
					if( pItemElemOld )	// 기존장비가 있었다면
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );
						
						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
#endif
					pModel->TakeOffParts( PARTS_SHIELD );
					if( pEquipInfo )
						pEquipInfo[PARTS_SHIELD].dwItemID	= 0;
				}
				
				pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
				if( pInventory->GetEquip( PARTS_RWEAPON ) )
				{
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	// 무기벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
#ifdef __WORLDSERVER
					if( pItemElemOld )	// 기존장비가 있었다면
					{
						PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
						pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

						g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

						g_xWSUserManager->AddDoEquip( pMover, -1, pItemElemOld, FALSE );
					}
//sun: 9,10차 제련 __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, PARTS_RWEAPON );

#endif // __WORLDSERVER
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( pEquipInfo )
						pEquipInfo[PARTS_RWEAPON].dwItemID	= 0;
				}
			}
		} // HD_TWO
	} // bEquip == TRUE

	if( bEquip )
	{
		if( pInventory == NULL )
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				pModel->TakeOffParts( pItemProp->dwExclusive );
				if( pEquipInfo )
					pEquipInfo[pItemProp->dwExclusive].dwItemID		= 0;
			}
			// 페이크 장착 
			if( pEquipInfo )
				memcpy( &pEquipInfo[dwParts], &rEquipInfo, sizeof(EQUIP_INFO) );
		}
		else
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				FLItemElem* pExclusive	= pInventory->GetEquip( pItemProp->dwExclusive );
				if( pExclusive )
				{
					if( pInventory->UnEquip( pItemProp->dwExclusive ) == FALSE )
						return FALSE;
#ifdef __WORLDSERVER
					PT_ITEM_SPEC pProp	= pExclusive->GetProp();
					pMover->ResetDestParamEquip( pProp, pExclusive );

					g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pExclusive );

					g_xWSUserManager->AddDoEquip( pMover, -1, pExclusive, FALSE );
#endif	// __WORLDSERVER

				}
				pModel->TakeOffParts( pItemProp->dwExclusive );

			}

			// 기존 장비 제거 
			if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
			{
				DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
				FLItemElem* pItemElemTemp1 = (FLItemElem*)pInventory->GetEquip( dwParts );
				FLItemElem* pItemElemTemp2 = (FLItemElem*)pInventory->GetEquip( dwParts + 1 );
				if( pItemElemTemp1 ) dwElem1 = pItemElemTemp1->m_dwItemId;
				if( pItemElemTemp2 ) dwElem2 = pItemElemTemp2->m_dwItemId;
				if( dwElem1 != NULL_ID && dwElem2 == NULL_ID )
					dwParts = pItemProp->dwPartsub;
			}
			FLItemElem *pItemElemOld = pInventory->GetEquip( dwParts );
			if( pItemElemOld )
			{
				if( pInventory->UnEquip( dwParts ) == FALSE )
					return FALSE;
#ifdef __WORLDSERVER
				PT_ITEM_SPEC pOldItemProp = pItemElemOld->GetProp();
				pMover->ResetDestParamEquip( pOldItemProp, pItemElemOld );

				g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElemOld );

				if( pItemElemOld->m_nResistSMItemId != 0 )	// 유료화 아이템이 셑팅되어 있다면
					((FLWSUser* )pMover)->DoSMItemUnEquip( pItemElemOld, dwParts );
//sun: 9,10차 제련 __ULTIMATE
				prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElemOld, FALSE, dwParts );

#endif // worldserver
				
			}
			
			// 새 장비 장착 
			if( TRUE == pInventory->DoEquip( dwIndex, /*MAX_INVENTORY +*/ dwParts ) )
			{
				if( pEquipInfo )
				{
//					pEquipInfo[dwParts].dwId	= pItemElem->m_dwObjId;		// TODO : 의도가 뭔지 알수가 없다.
					pEquipInfo[dwParts].dwItemID	= pItemElem->m_dwItemId;
					pEquipInfo[dwParts].nOption		= pItemElem->GetAttrOption();
				}

#ifdef __WORLDSERVER
				if( pItemElem && pItemElem->m_nResistSMItemId != 0 ) // 유료화 아이템이 셑팅되어 있다면
				{
					((FLWSUser* )pMover)->DoSMItemEquip( pItemElem, dwParts );
				}
//sun: 9,10차 제련 __ULTIMATE
				prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElem, TRUE, dwParts );

#endif // __WORLDSERVER
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
				return FALSE;
			}
		} // not Fake

#ifndef __WORLDSERVER
		TCHAR szPartsName[ 32 ];
		if( pItemProp->bPartsFile == TRUE )
			prj.m_modelMng.MakePartsName( szPartsName, _countof( szPartsName ), _T( "part" ), pItemProp->dwID, nSex );
		else
			prj.m_modelMng.MakeModelName( szPartsName, _countof( szPartsName ), OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
		switch( dwParts )
		{
		case PARTS_RWEAPON: 
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				switch( pItemProp->dwItemKind3 )
				{
				case IK3_KNUCKLEHAMMER:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
					break;
				case IK3_BOW:
#ifdef __ADDWEAPON_CROSSBOW16
					if( pItemProp->nDestData1[0] == 100 )
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					else
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#else
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
#endif
					break;

#ifdef __ADDWEAPON_CROSSBOW16
				case IK3_CROSSBOW:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					break;
#endif //__ADDWEAPON_CROSSBOW
				case IK3_YOYO:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					((CModelObject*)pModel)->LoadElement( szPartsName, PARTS_LWEAPON );
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
					break;
				default:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				}
	//			if( pItemProp->dwItemKind3 == IK3_KNUCKLEHAMMER )
	//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
	//			else
	//				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
			}
			break;
		case PARTS_LWEAPON: 
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
			}
			break;
		case PARTS_SHIELD: 
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
			}
			break;
		case PARTS_UPPER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
			}
			break;
		case PARTS_LOWER_BODY:
			if( bIfParts )
			{
				{
					((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
					FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				}
			}
			break;
		default:
			if( bIfParts )
			{
				// 날개를 망토에 부착할려고 시도한 코드		2016.07.11
				//if ( pItemProp->dwItemKind3 != IK3_WING )				// 비행체 말고 다른 파트에 날개가 있으면 출력안시킴	05
					((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			}
			break;

		}

#ifdef __CLIENT
		if( PARTS_LWEAPON == dwParts || PARTS_RWEAPON == dwParts || PARTS_SHIELD == dwParts )
			pMover->SetMotion( MTI_STAND, ANILOOP_1PLAY , MOP_NO_TRANS );

#endif //__CLIENT
		
#endif // not WorldServer
	} // 장착.
	else
	// 탈착.
	{
		if( bIfParts )
		{
			if( pItemProp->dwExclusive != NULL_ID )
			{
				switch( pItemProp->dwExclusive )
				{
				case PARTS_UPPER_BODY:
					pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
					FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
					break;
				case PARTS_LOWER_BODY:
					{

						PT_ITEM_SPEC pItemPropEquip = NULL;
						if( pItemProp->dwParts == PARTS_CLOTH )
						{
							pItemPropEquip = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_UPPER_BODY );
						}
						else
						{
							pItemPropEquip = pMover->GetEquipItemProp( pInventory, pEquipInfo, PARTS_CLOTH );
						}

						if( pItemPropEquip == NULL )
						{
							pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
							FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
							pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
						}					
					}					
					break;
				case PARTS_HAND:
					pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
					break;
				case PARTS_FOOT:
					pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
					break;
				}
			}
		}
		if( pInventory && pInventory->GetEquip( dwParts ) )
		{
			if( pInventory->UnEquip( dwParts ) == FALSE )
			{
#ifdef __WORLDSERVER
				if( TRUE == pMover->IsPlayer() )
					( (FLWSUser*)pMover )->AddDefinedText(TID_GAME_LACKSPACE, "" );
#endif	// __WORLDSERVER				
				return FALSE;
			}

#ifdef __WORLDSERVER
			g_xApplyItemEffect->RemoveEquipActiveSkill( pMover, pItemElem );

//sun: 9,10차 제련 __ULTIMATE
			prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemElem, FALSE, dwParts );
			if( pItemElem && pItemElem->m_nResistSMItemId != 0 )	// 유료화 아이템이 셑팅되어 있다면
			{
				((FLWSUser* )pMover)->DoSMItemUnEquip( pItemElem, dwParts );
			}
#endif // __WORLDSERVER
		}

		if( pEquipInfo )
		{
			pEquipInfo[dwParts].dwItemID			= 0;
			pEquipInfo[dwParts].nOption				= 0;
			pEquipInfo[dwParts].byFlag				= 0;
			pEquipInfo[dwParts].dwLooksChangeItemID	= 0;
			pEquipInfo[dwParts].bIsCombined			= false;
		}

#ifndef __WORLDSERVER
		if( bIfParts )
		{
			switch( dwParts )
			{
			case PARTS_UPPER_BODY:
				pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
				break;
			case PARTS_LOWER_BODY:
				pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				FLSPrintf( lpszTemp, _countof( lpszTemp ), PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				break;
			case PARTS_HAND:
				pModel->LoadElement( PARTSMESH_HAND( nSex ), PARTS_HAND );
				break;
			case PARTS_FOOT:
				pModel->LoadElement( PARTSMESH_FOOT( nSex ), PARTS_FOOT );
				break;
			}
		}
#endif

#ifdef __CLIENT
		if( PARTS_LWEAPON == dwParts || PARTS_RWEAPON == dwParts || PARTS_SHIELD == dwParts )
			pMover->SetMotion( MTI_STAND, ANILOOP_1PLAY , MOP_FIXED );
#endif //__CLIENT

	} // 탈착


	// 벗길때
	if( bEquip == FALSE )
	{
		// 위에서 오른손 무기를 해제 했었다. 
		if( nPart == PARTS_RWEAPON )
		{
			if( pInventory )
			{
				FLItemElem* pItemLeft = pInventory->GetEquip( PARTS_LWEAPON );
				if( pItemLeft )	// 왼손에 무기가 남아있다.
				{
					//오른손으로 옮겨줌.
					#ifdef __WORLDSERVER
					if( pMover->IsSMMode( SM_RESIST_ATTACK_LEFT ) )
					{
						pMover->m_nAttackResistRight = pMover->m_nAttackResistLeft;
						pMover->m_nAttackResistLeft = 0xff;
						DWORD dwSMTime = pMover->m_dwSMTime[SM_RESIST_ATTACK_LEFT];
						pMover->SetSMMode( SM_RESIST_ATTACK_LEFT, 1 );
						pMover->SetSMMode( SM_RESIST_ATTACK_RIGHT, dwSMTime );
						((FLWSUser*)pMover)->AddResistSMMode( pMover->m_nAttackResistLeft, pMover->m_nAttackResistRight, pMover->m_nDefenseResist );
					}
					#endif // __WORLDSERVER
					
					pInventory->Swap( PARTS_LWEAPON, PARTS_RWEAPON );	
#ifdef __WORLDSERVER			
//sun: 9,10차 제련 __ULTIMATE
					prj.m_UltimateWeapon.SetDestParamUltimate( pMover, pItemLeft, TRUE, PARTS_RWEAPON );
#endif // __WORLDSERVER

				#ifdef __CLIENT
					((CModelObject*)pModel)->MovePart( PARTS_RWEAPON, PARTS_LWEAPON );
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
				#endif //__CLIENT
				}
			}
			else
			{	// fake
				if( pEquipInfo )
				{
					if( pEquipInfo[PARTS_LWEAPON].dwItemID != 0 )
					{
						memcpy( &pEquipInfo[PARTS_RWEAPON], &pEquipInfo[PARTS_LWEAPON], sizeof(EQUIP_INFO) );
						pEquipInfo[PARTS_LWEAPON].dwItemID	= 0;
						pEquipInfo[PARTS_LWEAPON].nOption	= 0;
						pEquipInfo[PARTS_LWEAPON].byFlag	= 0;
					#ifdef __CLIENT
						((CModelObject*)pModel)->MovePart( PARTS_RWEAPON, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
					#endif	// __CLIENT
					}
				}
			}
		}
	}

#ifdef __CLIENT
	D3DXVECTOR3 vPos = pMover->GetPos();
				
	if( bEquip )
	{
		if( pItemProp )
		{
			if( pItemProp->dwItemKind1 == IK1_WEAPON )
			{
				PLAYSND( SND_INF_EQUIPWEAPON, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY )
			{
				PLAYSND( SND_INF_EQUIPACCESSORY, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_CLOTH )
			{
				PLAYSND( SND_INF_EQUIPCLOTH, &vPos );
			}
			else
			if( pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_ARMOR 
				|| pItemProp->dwItemKind2 == IK2_ARMORETC )
			{
				PLAYSND( SND_INF_EQUIPARMOR, &vPos );
			}	
		}
	}
	else
	{
		PLAYSND( SND_INF_INVENTORYDROP, &vPos );		
	}
#endif //__CLIENT

	return TRUE;
}

// 장착 가능한가를 검사.
//sun: 11, 각성, 축복
BOOL CMover::IsEquipAble( FLItemElem* pItem,BOOL bIgnoreLevel )
{
//sun: 11, 각성, 축복
	if( !pItem )
		return FALSE;
	PT_ITEM_SPEC pItemProp	= pItem->GetProp();
//	OBJID dwObjid	= pItem->m_dwObjId;

	// 빗자루류 검사.
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		int nLimitLv = pItemProp->dwFlightLimit;
		if( nLimitLv == NULL_ID )
			nLimitLv = 1;
//sun: all, __NOLIMIT_RIDE_ITEM
		if( pItemProp->dwID == ITEM_INDEX( 26514, II_RID_RID_STI_MAGIC01 ) )
			nLimitLv = 0;
		
		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			int nAttr	= pWorld->GetHeightAttribute( GetPos().x, GetPos().z );
			
			if( GetFlightLv() < nLimitLv )		// 비행레벨이 안되면 못탄다.
			{
#ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
					( (FLWSUser*)this )->AddDefinedText( TID_GAME_USEAIRCRAFT, "" );	// 잠시만 기다려주세요.
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_GAME_USEAIRCRAFT ), NULL, prj.GetTextColor( TID_GAME_USEAIRCRAFT ) );
#endif // __WORLDSERVER
				return FALSE;
			}
			
			if( !pWorld->m_bFly )
			{
#ifdef __WORLDSERVER
				( (FLWSUser*)this )->AddDefinedText( TID_ERROR_NOFLY, "" );	// 비행금지구역입니다.
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_ERROR_NOFLY ), NULL, prj.GetTextColor( TID_ERROR_NOFLY ) );
#endif // __WORLDSERVER
				return FALSE;
			}

			if( HasBuffByIk3( IK3_TEXT_DISGUISE ) )
			{
#ifdef __WORLDSERVER
				( (FLWSUser*)this )->AddDefinedText( TID_QUEST_DISQUISE_NOTFLY, "" );	// 변신중에는 비행을 할수 없습니다
#else // __WORLDSERVER
				g_WndMng.PutString( prj.GetText( TID_QUEST_DISQUISE_NOTFLY ), NULL, prj.GetTextColor( TID_QUEST_DISQUISE_NOTFLY ) );
#endif // __WORLDSERVER
				return FALSE;
			}
			
			// 장착 이전에 검사하여야 한다.
			if( nAttr == HATTR_NOFLY )		// 비행금지구역에선 못탄다.
				return FALSE;
			
		#ifdef __WORLDSERVER
			if( m_pActMover->IsState( OBJSTA_STAND ) == FALSE )
				return FALSE;
		#endif 
		}

//sun: 8, // __S8_PK
#ifdef __WORLDSERVER
		if( IsChaotic() )
		{
			CHAO_PROPENSITY Propensity = prj.GetPropensityPenalty( GetPKPropensity() );
			if( !Propensity.nFly )
			{
				((FLWSUser*)this)->AddDefinedText( TID_GAME_CHAOTIC_NOT_FLY );
				return FALSE;
			}
		}
#endif // __WORLDSERVER

//sun: 9, 9-10차 펫
#ifdef __WORLDSERVER
		if( HasActivatedEatPet() || HasActivatedSystemPet() )	// 펫이 소환된 상태라면 비행 불가
		{
			( (FLWSUser*)this )->AddDefinedText( TID_GAME_CANNOT_FLY_WITH_PET );
			return FALSE;
		}
#endif	// __WORLDSERVER
	}

	// 무기류가 아닌것(방어구)는 성별을 확인한다.
	{
		// 성별 확인( 무기는 빠짐 )
		if( pItemProp->dwItemSex != NULL_ID && pItemProp->dwItemSex != GetSex() )
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_WRONGSEX, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		} // 성별
	}// 무기
	
	// 방어구 입을때 직업확인.
	if( pItemProp->dwItemKind1 != IK1_WEAPON )
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) ) {
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100820 기간제 아이템 설정..
	if( pItem->IsFlag( FLItemElem::expired ) == TRUE )
	{
#ifdef __WORLDSERVER
		if( IsPlayer() == TRUE )
		{
			( (FLWSUser*)this )->AddDefinedText( TID_GAME_ITEM_EXPIRED );
		}
#endif	// __WORLDSERVER
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////

	// 아이템 필요직업검사.
	if( pItemProp->dwItemKind1 == IK1_WEAPON )		// 무기류를 장착하려할때
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) )	// 아이템에 필요직업이 지정되어 있고
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName );
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	if( !g_eLocal.GetState( EVE_SCHOOL ) && !bIgnoreLevel )
	{

		// 아이템 필요레벨 검사.
		if( pItem->CanUseLevel( this ) == false )	// 레벨이 안되면 못참.
		{
#ifdef __WORLDSERVER
			if( TRUE == IsPlayer() )
			{
				//sun: 11, 각성, 축복
				( (FLWSUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItem->GetMinLimitLevel() );
			}
#endif	// __WORLDSERVER
			return FALSE;
		}
		// 보석류를 장착
		if( pItemProp->dwItemKind2 == IK2_JEWELRY )
		{
			if( pItem->CanUseLevel( this ) == false )	// 레벨이 안되면 못참.
			{
#ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
				{
					//sun: 11, 각성, 축복
					( (FLWSUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItem->GetMinLimitLevel() );
				}
#endif	// __WORLDSERVER
				return FALSE;
			}
		}

	}

	PT_ITEM_SPEC pHandItemProp	= NULL;
	
	// 들고있는 무기 프로퍼티 꺼냄.
	FLItemElem *pItemElem = m_Inventory.GetEquip( PARTS_RWEAPON );		
	if( pItemElem )
	{
		pHandItemProp = pItemElem->GetProp();
	}

	// 화살을 착용하려 했을때 보우일때만 착용돼야 함
	if( pItemProp->dwItemKind3 == IK3_ARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_BOW ) )
		return FALSE;			

#ifdef __ADDWEAPON_CROSSBOW16
	if( pItemProp->dwItemKind3 == IK3_CROSSARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_CROSSBOW ) )
		return FALSE;
#endif //__ADDWEAPON_CROSSBOW16

	return TRUE;
}

//
//
// pItemElem이 벗는게 가능한가를 검사.
BOOL CMover::IsUnEquipAble( PT_ITEM_SPEC pItemProp )
{
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( m_pActMover->IsSit() )	// 빗자루는 앉은 상태에서는 탈착 금지
			return FALSE;

		if( GetWorld() )	// RedoEquip에서 불렀을때 NULL인경우 있음. NULL이면 걍 벗김
		{
			int nAttr = GetWorld()->GetHeightAttribute( GetPos().x, GetPos().z );		// 이동할 위치의 속성 읽음.
			// 비행금지 or 걷기금지 or 이동금지 지역에서 는 못내림.
			if( (nAttr == HATTR_NOFLY || nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE ) )		
				return FALSE;
		}
	}

	if( g_eLocal.GetState( EVE_SCHOOL ) && pItemProp->dwItemKind3 == IK3_CLOAK )
		return FALSE;
	
	return TRUE;
}

//
//
// 정상적인 아이템 장착, 탈착 
BOOL CMover::DoEquip( FLItemElem* pItemElem, BOOL bEquip, int nPart )
{
	PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
	
	if( IsDie() )	// 죽은 상태에서는 장/탈착 금지
	{
		return FALSE;
	}

	if( bEquip )
	{
		if( pItemProp->dwParts == PARTS_RIDE && m_Inventory.GetEquip( PARTS_RIDE ) )
			return FALSE;
		if( pItemProp->dwParts == PARTS_RIDE && pItemElem && pItemElem->IsFlag( FLItemElem::expired ) )
			return FALSE;
//sun: 11, 각성, 축복
		if( IsEquipAble( pItemElem ) == FALSE )
			return FALSE;
	}
	else
	{
		if( IsUnEquipAble( pItemProp ) == FALSE )
			return FALSE;
	}

	EQUIP_INFO equipInfo;
	equipInfo.dwItemID	= pItemElem->m_dwItemId;
	equipInfo.nOption	= pItemElem->GetAttrOption();
	equipInfo.byFlag	= pItemElem->m_byFlag;
	BOOL bResult = DoEquip( GetSex(), m_dwSkinSet, pItemElem, nPart, equipInfo, &m_Inventory, m_aEquipInfo, (CModelObject*)m_pModel, bEquip, this );
		
	if( !bResult )	// 2004/04/27
		return FALSE;

#ifndef __CLIENT
	if( bEquip )
	{
		SetDestParamEquip( pItemProp, pItemElem );		// 장착한 장비의 DestParam설정

		if( pItemProp->nBindCondition == BIND_CONDITION_USE
			&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
			&& pItemElem->IsFlag( FLItemElem::reset_bind ) == FALSE )
		{
			pItemElem->SetFlag( FLItemElem::binds );
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
		}

		else if( pItemProp->nResetBindCondition == BIND_CONDITION_USE
			&& pItemElem->IsFlag( FLItemElem::binds ) == FALSE
			&& pItemElem->IsFlag( FLItemElem::reset_bind ) == TRUE )
		{
			pItemElem->SetFlag( FLItemElem::binds );
			UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_FLAG, MAKELONG( pItemElem->m_dwObjIndex, pItemElem->m_byFlag ) );
		}
	}
	else
	if( bEquip == FALSE )
	{
		ResetDestParamEquip( pItemProp, pItemElem );	// 해제하는 장비의 DestParam리셋
		
		if( pItemProp->dwItemKind1 == IK1_WEAPON ) 
		{
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}

	}
#endif	// __CLIENT

	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
#ifdef __CLIENT	
			if( IsPlayer() )
			{
				if( g_WndMng.m_pWndWorld->m_bFirstFlying == FALSE )
				{
					PlayMusic(BGM_TH_FLYING);
					g_WndMng.m_pWndWorld->m_bFirstFlying = TRUE;
				}
				else
				{
					PlayMusic(BGM_EV_LIGHTWING);
				}
				g_WndMng.m_bAutoRun = FALSE;
			}
#endif//__CLIENT				
			SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
		}
		ClearDest();	// 2004/04/30
	}

	// 망토 입을때 처리.
	if( bEquip )
	{
		if( pItemProp->dwID == ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK ) )		// 길드전용 민짜 망토.
		{
			m_idGuildCloak = pItemElem->m_idGuild;			// 망토에 박혀있던 길드아이디를 받음.
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( pItemElem->m_idGuild );
#endif
		} 
		else
		{
#ifdef __XCLOAK
			m_pCloakTexture = NULL;
			m_idGuildCloak = 0;			// 망토에 박혀있던 길드아이디를 받음.
#endif
		}
	}

	UpdateBoundBox();
	UpdateParam();

	return bResult;
}


#ifdef __CLIENT
// 장착시 dwItemIndex만을 사용하는 이 Method는 Fake 아이템을 위해 사용 
// pModel에 값이 있는 경우는 좀 특별한 경우다. 그 경우는 장착/탈착 후에도 내부 장착에 관한 세팅에 변경이 없게 된다.
BOOL CMover::DoFakeEquip( const EQUIP_INFO & rEquipInfo, BOOL bEquip, int nPart, CModelObject* pModel )
{
	BOOL bResult;
	if( pModel )
		bResult	= CMover::DoEquip( GetSex(), m_dwSkinSet, NULL, nPart, rEquipInfo, NULL, m_aEquipInfo, (CModelObject*)pModel, bEquip, this );
	else
		bResult = CMover::DoEquip( GetSex(), m_dwSkinSet, NULL, nPart, rEquipInfo, NULL, m_aEquipInfo, (CModelObject*)m_pModel, bEquip, this );

	if( !bResult )	// 2004/04/27
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "return FALSE" ) );
		return FALSE;
	}

	UpdateBoundBox();
	UpdateParam();

	PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( rEquipInfo.dwItemID );
#ifndef __CLIENT
	if( pProp && pProp->dwItemKind1 >= IK2_WEAPON_DIRECT && pProp->dwItemKind1 <= IK2_WEAPON_MAGIC )
	{
		if( bEquip ) {
			if( !m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_COMBAT );
		}
		else {
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}
	}
#endif	// __CLIENT
	
	
	if( pProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
			SendActMsg( OBJMSG_MODE_FLY, pProp->dwID, 0, 1 );
			ClearDest();
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
			ClearDest();
		}
	}
	// 망토 입을때 처리.
	if( bEquip )
	{
		if( pProp->dwID == ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK ) )		// 길드전용 민짜 망토.
		{
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
#endif
		} else
		{
#ifdef __XCLOAK
			m_pCloakTexture = NULL;
#endif
		}
	}

	return bResult;
}
#endif // CLIENT

void CMover::RedoEquip( BOOL fFakeParts, BOOL bDestParam )
{
	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp	= NULL;
	BOOL bEquip;

	// 장착될 아이템의 선 처리 
	for( DWORD i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		bEquip	= fFakeParts? ( m_aEquipInfo[i].dwItemID != 0 ) : ( m_Inventory.GetEquip( i ) != NULL );
		if( bEquip )
		{
			pItemElem = NULL;			pItemProp = NULL;
			
			// 장착된 아이템의 프로퍼티 꺼냄.
			if( !fFakeParts )	// 진짜 장비냐.
			{
				pItemElem	= m_Inventory.GetEquip( i );
				if( NULL != pItemElem )
					pItemProp	= pItemElem->GetProp();
			}
			else
			{
				pItemProp	= g_xSpecManager->GetSpecItem( m_aEquipInfo[i].dwItemID );
			}

			
				
			if( ( pItemProp != NULL && pItemProp->dwID > 0 && 
				( pItemProp->dwParts == i || pItemProp->dwPartsub == i || 
				  (pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON) )		// RWEAPON은 LWEAPON과 같은것으로 친다.
				) &&
				( fFakeParts || m_Inventory.IsEquip( pItemElem->m_dwObjId ) ) )
			{
				// 빗자루를 장착할껀데
				if( i == PARTS_RIDE )
				{
					if( FALSE == m_pActMover->IsFly() )	// 비행중이 아니면
					{
//						FLERROR_LOG( PROGRAM_NAME, _T( "OBJSTAF_FLY" ) );
						SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );	// 비행모드로 전환
					}
					if( m_dwRideItemIdx == 0 )	// 빗자루 ID가 없으면
					{
//						FLERROR_LOG( PROGRAM_NAME, _T( "m_dwRideItemIdx	= pItemProp->dwID" ) );
						m_dwRideItemIdx	= pItemProp->dwID;	// 값 보정.
					}
				} 
				if( pItemProp->dwID == ITEM_INDEX( 4601, II_ARM_S_CLO_CLO_BLANK ) )		// 길드전용 민짜 망토.
				{
					if( pItemElem )		// 서버 / 클라(액티브무버)
						m_idGuildCloak = pItemElem->m_idGuild;
 #ifdef __XCLOAK
					m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
 #endif
				}
				if( pItemElem )
					m_aEquipInfo[i].nOption	= pItemElem->GetAttrOption();

				if( bDestParam )
				{
					SetDestParamEquip( pItemProp, pItemElem, TRUE );
//sun: 9,10차 제련 __ULTIMATE
					if( i == PARTS_RWEAPON )
						prj.m_UltimateWeapon.SetDestParamUltimate( this, pItemElem, TRUE, PARTS_RWEAPON );
				}
			}
			else
			{
				//gmpbigsun( 20120227 ) : log제거됨
				//FLERROR_LOG( PROGRAM_NAME, _T( "Illegal dwParts, %s, %d" ), m_szName, i );
				m_aEquipInfo[i].dwItemID	= 0;
			}
		}
		else 
		{	// 장착될게 없음.
			if( i == PARTS_RIDE )	// 빗자루가 장착되어 있지 않은데
			{
				if( m_pActMover->IsFly() )	// 비행중이다.
				{
//					FLERROR_LOG( PROGRAM_NAME, _T( "OBJSTAF_GROUND" ) );
					SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );	// 그러면 지상모드로 변환 시켜야지.
				}
				if( m_dwRideItemIdx != 0 )		// 빗자루 인덱스도 0이어야 하는데 뭔가 있다.
				{
//					FLERROR_LOG( PROGRAM_NAME, _T( "m_dwRideItemIdx	= 0" ) );
					m_dwRideItemIdx		= 0;	// 그러면 0으로 클리어.
				}
				if( m_Inventory.GetEquip( i ) )
				{
//					FLERROR_LOG( PROGRAM_NAME, _T( "pRide is not null" ) );
					m_Inventory.UnEquip( i );
				}
			}
		}
	}


	pItemElem	= m_Inventory.GetEquip( PARTS_RWEAPON );		// 위에 루프에서 모든파츠를 이렇게 일괄적으로 넣어도 될듯 싶다.
	if( pItemElem )
	{
		m_aEquipInfo[PARTS_RWEAPON].nOption	= pItemElem->GetAttrOption();
	}
	pItemElem	= m_Inventory.GetEquip( PARTS_LWEAPON );
	if( pItemElem )
	{
		m_aEquipInfo[PARTS_LWEAPON].nOption	= pItemElem->GetAttrOption();
	}

	// 아이템이 실제 장착.
	UpdateParts( fFakeParts );

#ifdef __WORLDSERVER
	InvalidEquipOff( fFakeParts );		// 장착되어설 안될 장비가 장착되어 있다면 벗김
#endif

	if( bDestParam )
	{
		int nAbilityOption	= GetSetItem();
		if( nAbilityOption > 0 )
			SetSetItemAvail( nAbilityOption );

		
		SetDestParamSetItem( NULL );
	}
}

// 장착되어설 안될 장비가 장착되어 있다면 벗김
int	CMover::InvalidEquipOff( BOOL bFakeParts )
{
	FLItemElem* pItemElem = NULL;
	PT_ITEM_SPEC pItemProp	= NULL;
	int		i;

	for( i = 0; i < MAX_HUMAN_PARTS; i ++ )
	{
		pItemElem = NULL;		
		pItemProp = NULL;

		// 장착된 아이템의 프로퍼티 꺼냄.
		if( bFakeParts )	// Fake 장비
		{
			if( m_aEquipInfo[i].dwItemID	== 0 )	continue;
			pItemProp	= g_xSpecManager->GetSpecItem( m_aEquipInfo[i].dwItemID );
		}
		else
		{
			pItemElem = m_Inventory.GetEquip( i );
			if( pItemElem )
				pItemProp  = pItemElem->GetProp();	
		}

		if( pItemProp == NULL )	
			continue;	// 프로퍼티 없으면 실패.

//sun: 8, // __S8_PK
//sun: 11, 각성, 축복
		if( IsEquipAble( pItemElem,TRUE ) == FALSE )	// 장착할 수 없는 아이템이다.
//!		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId, TRUE ) == FALSE )	// 장착할 수 없는 아이템이다.
//!		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId) == FALSE )	// 장착할 수 없는 아이템이다.
		{
			if( pItemElem )
			{
				if( DoEquip( pItemElem, FALSE ) == FALSE )		// 벗김.
				{
				#ifdef __WORLDSERVER
					// 벗기는데 실패함. 인벤이 꽉찼다거나 기타등등 이유
					#ifndef _DEBUG
						FLERROR_LOG( PROGRAM_NAME, _T( "아템 벗기는데 실패:%s" ), GetName() );
					#endif
				#endif
				}
			} 
		}
	} // for
	
	return FALSE;
}

void CMover::SetDestParamEquip( PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem, BOOL bIgnoreSetItem )
{
	if( pItemProp == NULL || pItemElem == NULL )
	{
		return;
	}

	if( pItemElem->IsFlag( FLItemElem::expired ) )
	{
		return;
	}

	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
		{
			SetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ], pItemProp->dwChgParamVal[ Nth ] );
		}
	}
// 	if( pItemProp->dwDestParam[0] != -1 )
// 	{
// 		SetDestParam( pItemProp->dwDestParam[0], pItemProp->nAdjParamVal[0], pItemProp->dwChgParamVal[0] );
// 	}
// 	if( pItemProp->dwDestParam[1] != -1 )
// 	{
// 		SetDestParam( pItemProp->dwDestParam[1], pItemProp->nAdjParamVal[1], pItemProp->dwChgParamVal[1] );
// 	}
// 	if( pItemProp->dwDestParam[2] != -1 )
// 	{
// 		SetDestParam( pItemProp->dwDestParam[2], pItemProp->nAdjParamVal[2], pItemProp->dwChgParamVal[2] );
// 	}
	
	// 원소별 속성이 붙은 아이템일경우 세팅함.
	
	// 방어구이고 속성옵션값이 있으면...
	if( pItemElem && pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( pItemElem->m_byItemResist != SAI79::NO_PROP )
		{
			int nStrong = GetStrongDST( pItemElem->m_byItemResist );
			int nWeak   = GetWeakDST( pItemElem->m_byItemResist );

			SetDestParam( nWeak,  -pItemElem->m_nResistAbilityOption*2, NULL_CHGPARAM );
			SetDestParam( nStrong, pItemElem->m_nResistAbilityOption*2, NULL_CHGPARAM );
		}
	}
	else
	{
		if( pItemProp->nItemResistElecricity )
			SetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity, NULL_CHGPARAM );
		if( pItemProp->nItemResistFire )
			SetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire, NULL_CHGPARAM );
		if( pItemProp->nItemResistWind )
			SetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind, NULL_CHGPARAM );
		if( pItemProp->nItemResistWater )
			SetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater, NULL_CHGPARAM );
		if( pItemProp->nItemResistEarth )
			SetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth, NULL_CHGPARAM );
	}

//sun: 11, 채집 시스템
	CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
	if( pItemElem && pItemElem->IsAccessory() )		// 액세서리
	{
		const std::vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
		if( pDst != NULL && pDst->empty() == false )
		{
			const size_t nSize	= pDst->size();
			for( size_t i = 0; i < nSize; i++ )
			{
				SetDestParam( (*pDst).at( i ).nDst, (*pDst).at( i ).nAdj, NULL_CHGPARAM );
			}
		}
	}

#ifdef __WORLDSERVER
	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] == DST_GIFTBOX )
		{
			AddBuff( BUFF_EQUIP, static_cast<WORD>( pItemProp->dwID ), 1, 999999999, 0, BT_TICK );	// TODO
			break;
		}
	}

// 	if( pItemProp->dwDestParam[0] == DST_GIFTBOX || pItemProp->dwDestParam[1] == DST_GIFTBOX || pItemProp->dwDestParam[2] == DST_GIFTBOX )
// 		AddBuff( BUFF_EQUIP, (WORD)( pItemProp->dwID ), 1, 999999999, 0, BT_TICK );	// TODO
#endif // __WORLDSERVER

	if( !bIgnoreSetItem )
	{
		if( pItemElem )
		{
			int nAbilityOption	= GetSetItem( pItemElem );
			if( nAbilityOption > 0 )
				SetSetItemAvail( nAbilityOption );
			SetDestParamSetItem( pItemElem );
		}
	}

	if( pItemElem && !pItemElem->IsFlag( FLItemElem::expired ) )
	{
		DestParamPiercingAvail( pItemElem, TRUE );
		SetDestParamRandomOpt( pItemElem );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		DestParamGemAvail( pItemElem, TRUE );
#endif
	}
}

void CMover::ResetDestParamEquip( PT_ITEM_SPEC pItemProp, FLItemElem* pItemElem )
{
	if( pItemProp == NULL || pItemElem == NULL )
	{
		return;
	}

	if( pItemElem->IsFlag( FLItemElem::expired ) )
	{
		return;
	}

	for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
	{
		if( pItemProp->dwDestParam[ Nth ] != NULL_ID )
		{
			ResetDestParam( pItemProp->dwDestParam[ Nth ], pItemProp->nAdjParamVal[ Nth ] );
		}
	}

// 	if( pItemProp->dwDestParam1 != -1 )
// 		ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1 );
// 	if( pItemProp->dwDestParam2 != -1 )
// 		ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2 );
// //sun: 10, __PROP_0827
// 	if( pItemProp->dwDestParam3 != -1 )
// 		ResetDestParam( pItemProp->dwDestParam3, pItemProp->nAdjParamVal3 );
	
	if( pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
		if( pItemElem->m_byItemResist != SAI79::NO_PROP )
		{
			int nSelf   = GetSelfDST( pItemElem->m_byItemResist );
			int nStrong = GetStrongDST( pItemElem->m_byItemResist );
			int nWeak   = GetWeakDST( pItemElem->m_byItemResist );

			if( g_xFlyffConfig->GetMainLanguage() == LANG_THA )			
			{
				float fResult = ((float)pItemElem->m_nResistAbilityOption+1.2f) * (0.7f+(float)(pItemElem->m_nResistAbilityOption*0.01f));
				int nResult = (int)( fResult * fResult );

				ResetDestParam( nSelf,   nResult );
				ResetDestParam( nWeak,  -nResult/2 );
				ResetDestParam( nStrong, nResult/2 );
			}
			else
			{
				ResetDestParam( nWeak,  -pItemElem->m_nResistAbilityOption*2 );
				ResetDestParam( nStrong, pItemElem->m_nResistAbilityOption*2 );
			}
		}
	} 
	else
	{	
		if( pItemProp->nItemResistElecricity )
			ResetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity);
		if( pItemProp->nItemResistFire )
			ResetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire );
		if( pItemProp->nItemResistWind )
			ResetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind );
		if( pItemProp->nItemResistWater )
			ResetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater );
		if( pItemProp->nItemResistEarth )
			ResetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth );
	}

	if( pItemElem )
	{
//sun: 11, 채집 시스템
		CAccessoryProperty* pProperty	= CAccessoryProperty::GetInstance();
		if( pItemElem->IsAccessory() )		// 액세서리
		{
			const std::vector<SINGLE_DST>* pDst	= pProperty->GetDst( pItemElem->m_dwItemId, pItemElem->GetAbilityOption() );
			if( pDst != NULL && pDst->empty() == false )
			{
				const size_t nSize	= pDst->size();
				for( size_t i = 0; i < nSize; i++ )
				{
					ResetDestParam( (*pDst).at( i ).nDst, (*pDst).at( i ).nAdj );
				}
			}
		}

#ifdef __WORLDSERVER
		if( pItemProp->dwDestParam[0] == DST_GIFTBOX || pItemProp->dwDestParam[1] == DST_GIFTBOX || pItemProp->dwDestParam[2] == DST_GIFTBOX )
		{
			((FLWSUser*)this)->RemoveBuff( BUFF_EQUIP, (WORD)( pItemProp->dwID ) );
		}
#endif // __WORLDSERVER

		int nAbilityOption	= GetSetItem( pItemElem );
		if( nAbilityOption > 0 )
			ResetSetItemAvail( nAbilityOption );
		DestParamPiercingAvail( pItemElem, FALSE );
		ResetDestParamSetItem( pItemElem );
		ResetDestParamRandomOpt( pItemElem );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		DestParamGemAvail( pItemElem, FALSE );
#endif
	}
}


// 돈획득 	
void CMover::PickupGold( int nGold, BOOL bDropMob )
{
	FLASSERT( nGold > 0 );
	if( nGold <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s PickupGold - %d" ), GetName(), nGold );
		return;
	}

#ifdef __WORLDSERVER
	CParty *pParty	= g_PartyMng.GetParty( m_idparty );
	if( pParty && pParty->IsMember( m_idPlayer ) && bDropMob )	// 파티가 있고 몹이 떨군 돈만 분배된다.
	{
		// 파티가 있는 상태에서 돈을 집으려면 돈이 분배된다.
		// 몬스터가 드롭한돈만 분배되어야 한다.
//		int nNumInsideMember = 1;		// 돈 줍는사람 중심으로 반경 xx미터 이내있는사람이 몇명이나 되는지 검사. 
		
		int	nMaxListMember = 0;
		FLWSUser* pMember	= NULL;
		int i;
//		float fDist;
		D3DXVECTOR3	vDist;

		// 돈줍는사람 반경안에 드는 멤버들만 추려낸다.
		//mem_set( pListMember, 0, sizeof(pListMember) );
		FLWSUser *pListMember[MAX_PTMEMBER_SIZE] = { 0 };
		for( i = 0; i < pParty->m_nSizeofMember; i++ )
		{
			pMember	= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
			/*
			if( IsValidObj( pMember ) )
			{
				vDist = pMember->GetPos() - GetPos();
				fDist = D3DXVec3LengthSq( &vDist );		// 돈 줍는사람과 멤버간의 거리.
				if( fDist < 32.0f * 32.0f )				// xx미터보다 작으면
				{
					pListMember[ nMaxListMember++ ] = pMember;
				}
			}
			*/ //2009.01.21 // 반경 검사에 레이어 추가
			if( IsValidArea( pMember, 32.0f ) )
				pListMember[ nMaxListMember++ ] = pMember;
		}

		if( nMaxListMember > 0 )
		{
			int nShare = nGold / nMaxListMember;		// 멤버 각각의 몫
			int nRest  = nGold % nMaxListMember;		// 나누고난 후 나머지.
			if( nShare )	// 몫이 있을때.
			{
				for( i = 0; i < nMaxListMember; i++ )
				{
					pMember = pListMember[i];
					pMember->PickupGoldCore( nShare );
				}
			}

			// 나머지는 랜덤으로 한사람이 가짐.
			if( nRest )
			{
				pMember = pListMember[ random(nMaxListMember) ];	
				pMember->PickupGoldCore( nRest );
			}
		}
		else
		{
			FLERROR_LOG( LOG_PARTY_ERROR, _T( "party pick money error:%s %d %d" ), GetName(), pParty->m_nSizeofMember, nMaxListMember );
			for( i = 0; i < pParty->m_nSizeofMember; i ++ )
			{
				pMember		= g_xWSUserManager->GetUserByPlayerID( pParty->m_aMember[i].m_uPlayerId );
				if( IsValidObj( pMember ) )
				{
					FLERROR_LOG( LOG_PARTY_ERROR, _T( "%d %s" ), i, pMember->GetName() );
				}
			}
		}
	} 
	else
#endif // WORLDSERVER	
	{
		// 파티 없을때
		PickupGoldCore( nGold );
	}
}


void CMover::PickupGoldCore( int nGold )
{
	if( AddGold( nGold ) == FALSE )
		return;

#ifdef __WORLDSERVER
	if( IsPlayer() )
	{
		((FLWSUser*)this)->AddGoldText( nGold );
		if( nGold >= 500 )
		{
			LogItemInfo aLogItem;
			//aLogItem.Action = "R";
			//aLogItem.SendName = "GROUND";
			//aLogItem.RecvName = GetName();
			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "R" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), "GROUND" );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), GetName() );

			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = GetGold() - nGold;
			aLogItem.Gold2 = GetGold();
			//aLogItem.ItemName = "SEED";
			FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
			aLogItem.kLogItem.nQuantity = nGold;
			g_DPSrvr.OnLogItem( aLogItem );
		}										
	}
#endif	// __WORLDSERVER
}


#ifdef __WORLDSERVER
// 돈을 땅에 떨군다.  bPK - PK당한 경우 
CItem* CMover::DropGold( DWORD dwGold, const D3DXVECTOR3& vPos, BOOL bPK )
{
	if( IsDropableState( bPK ) == FALSE )
		return NULL;

	CWorld *pWorld = GetWorld();

	int nGold	= (int)dwGold;
//	if( nGold > 0 && nGold <= (int)GetGold() )
	if( nGold > 0 && CheckUserGold( nGold, false ) == true )
	{
		if( nGold >= 500 )
		{
			LogItemInfo aLogItem;
			//aLogItem.Action = "D";
			//aLogItem.SendName = GetName();
			//aLogItem.RecvName = "GROUND";	// 땅에 버린것

			FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "D" );
			FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), GetName() );
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "GROUND" );

			aLogItem.WorldId = pWorld->GetID();
			aLogItem.Gold = GetGold();
			aLogItem.Gold2 = GetGold() - nGold;
			//aLogItem.ItemName = "SEED";
			FLSPrintf( aLogItem.kLogItem.szItemName, _countof( aLogItem.kLogItem.szItemName ), "%d", ITEM_INDEX( 12, II_GOLD_SEED1 ) );
			aLogItem.kLogItem.nQuantity = nGold;
			g_DPSrvr.OnLogItem( aLogItem );
		}

		AddGold( -nGold );

		FLItemElem* pItemElem	= new FLItemElem;
		
		if( nGold <= (int)g_xSpecManager->GetSpecItem( ITEM_INDEX( 12, II_GOLD_SEED1 ) )->dwAbilityMax )
			pItemElem->m_dwItemId	= ITEM_INDEX( 12, II_GOLD_SEED1 );
		else if( nGold <= (int)g_xSpecManager->GetSpecItem( ITEM_INDEX( 13, II_GOLD_SEED2 ) )->dwAbilityMax )
			pItemElem->m_dwItemId	= ITEM_INDEX( 13, II_GOLD_SEED2 );
		else if( nGold <= (int)g_xSpecManager->GetSpecItem( ITEM_INDEX( 14, II_GOLD_SEED3 ) )->dwAbilityMax )
			pItemElem->m_dwItemId	= ITEM_INDEX( 14, II_GOLD_SEED3 );
		else
			pItemElem->m_dwItemId	= ITEM_INDEX( 15, II_GOLD_SEED4 );

		pItemElem->m_nItemNum	= (int)nGold;				// __NPP_050308 이후에는 사용하지 않을 것
		pItemElem->m_nHitPoint  = nGold;					// int형으로 돈을 버리게 하기 위해서 이 변수를 사용 

		CItem* pItem	= new CItem;
		pItem->m_pItemBase	= pItemElem;
		pItem->SetIndex( D3DDEVICE, pItemElem->m_dwItemId );
		pItem->SetPos( vPos );
		pItem->SetAngle( (float)xRandom( 360 ) );
		pItem->m_dwDropTime		= timeGetTime();	
		if( pWorld->ADDOBJ( pItem, TRUE, GetLayer() ) == TRUE )
		{
			return pItem;
		}
		else
		{
			SAFE_DELETE( pItem );
		}
	}
	
	return NULL;
}
#endif // __WORLDSERVER



#ifdef __CLIENT
BOOL CMover::GetEquipFlag( int nParts, BYTE* pbyFlag )
{
	if( IsActiveMover() )
	{
		FLItemElem* pItemElem	= GetEquipItem( nParts );
		if( pItemElem )
		{
			*pbyFlag	= pItemElem->m_byFlag;
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		if( nParts < 0 || nParts >= MAX_HUMAN_PARTS )
			return FALSE;

		if( m_aEquipInfo[nParts].dwItemID != 0 )
		{
			*pbyFlag	= m_aEquipInfo[nParts].byFlag;
			return TRUE;
		}
		return FALSE;
	}
}
#endif	// __CLIENT


// 장비를 장/탈착한다.
// pItemElem - 장착/탈착 할 아이템
// bEquip - TRUE 장착
// nPart - 파츠번호 
void CMover::EquipItem( FLItemElem *pItemElem, BOOL bEquip, int nPart )
{
	if( m_pActMover && m_pActMover->IsActAttack() )
		return;

	if( DoEquip( pItemElem, bEquip, nPart ) )
	{
	#ifdef __WORLDSERVER
		
		g_xApplyItemEffect->RefreshAllEquipActiveSkill( this );

		g_xWSUserManager->AddDoEquip( this, nPart, pItemElem, static_cast< BYTE >( bEquip ) );
		if( pItemElem->GetProp()->dwParts == PARTS_RIDE && pItemElem->GetProp()->dwItemJob == JOB_VAGRANT && bEquip == FALSE )
//			( ( pItemElem->m_dwItemId == II_RID_RID_BOR_EVEINSHOVER || pItemElem->m_dwItemId == ITEM_INDEX( 5801, II_RID_RID_BOR_LADOLF ) ) && bEquip == FALSE )
			RemoveItem( pItemElem->m_dwObjId, pItemElem->m_nItemNum );
	#endif // __WORLDSERVER
	}
}

// 탈 것에서 내린다.
void CMover::UnequipRide()
{
	if( IsPlayer() )
	{
		FLItemElem* pItemElem = GetEquipItem( PARTS_RIDE );
		if( pItemElem )
			EquipItem( pItemElem, FALSE, -1 );	// -1 파츠번호 
	}
}

void CMover::ClearEquipInfo()
{
	memset( m_aEquipInfo, 0, sizeof(m_aEquipInfo) );
// 	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
// 		m_aEquipInfo[i].dwId = NULL_ID;
}
