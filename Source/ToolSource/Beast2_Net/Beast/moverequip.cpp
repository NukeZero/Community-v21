#include "stdafx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "defineSound.h"
#include "defineItem.h"
//#include "defineAttribute.h"
#include "ModelObject.h"
#include "authorization.h"
#include "CreateObj.h"
#include "lang.h"

 
#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "guild.h"
#include "guildwar.h"
extern	CGuildMng	g_GuildMng;
extern	CGuildWarMng	g_GuildWarMng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 비스트일 때는 컴팔 안되게..
#ifdef __BEAST
#define GetJob()              Null()
#define UpdateParam()         Null()
#define IsInteriorityJob(x)   Null()
#define SetDestParam(x,y,z)   Null()
#define ResetDestParam(x,y,z) Null()

/*
#define GetMaxFatiguePoint() 0
#define GetMaxHitPoint()     0
#define GetMaxManaPoint()    0
*/
#endif

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
	{ 125,67,59 },
	{ 213,130,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 } 
};

#define TEX_PART_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair01.dds"  ) : _T( "Part_femaleHair01.dds"  ) )
//#define TEX_PART_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead01.dds"  ) : _T( "Part_femaleHead01.dds"  ) )
#define TEX_PART_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper01.dds" ) : _T( "Part_femaleUpper01.dds" ) )
#define TEX_PART_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower01.dds" ) : _T( "Part_femaleLower01.dds" ) )

#define PARTSMESH_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.o3d" ) : _T( "Part_femaleHair%02d.o3d" ) )
#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )
#define PARTSMESH_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper.o3d"    ) : _T( "Part_femaleUpper.o3d"    ) )
#define PARTSMESH_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower.o3d"    ) : _T( "Part_femaleLower.o3d"    ) )
#define PARTSMESH_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.o3d"     ) : _T( "Part_femaleHand.o3d"     ) )
#define PARTSMESH_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.o3d"     ) : _T( "Part_femaleFoot.o3d"     ) )

#define PARTSTEX_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.dds"  ) : _T( "Part_femaleHair%02d.dds"  ) )
//#define PARTSTEX_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.dds"  ) : _T( "Part_femaleHead%02d.dds"  ) )
#define PARTSTEX_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper%02d.dds" ) : _T( "Part_femaleUpper%02d.dds" ) )
#define PARTSTEX_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower%02d.dds" ) : _T( "Part_femaleLower%02d.dds" ) )
#define PARTSTEX_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.dds"      ) : _T( "Part_femaleHand.dds"      ) )
#define PARTSTEX_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.dds"      ) : _T( "Part_femaleFoot.dds"      ) )

// 머리카락 모양과 색 바꾸기  
void CMover::SetHair( int nHair )
{
	if( nHair < MAX_HAIR )
	{
		m_dwHairMesh = nHair;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ];
		// Hair
		_stprintf( lpszTemp, PARTSMESH_HAIR( GetSex() ), m_dwHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		_stprintf( lpszTemp, PARTSTEX_HAIR( GetSex() ), 0 /*nHairColor*/ + 1 );
		pModel->ChangeTexture( PARTS_HAIR, TEX_PART_HAIR( GetSex() ), lpszTemp );
	}
}

void  CMover::SetHairColor( FLOAT r, FLOAT g, FLOAT b )
{ 
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
	{
		m_fHairColorR = ( r < 0.3f ) ? 0.3f : r;
		m_fHairColorG = ( g < 0.3f ) ? 0.3f : g;
		m_fHairColorB = ( b < 0.3f ) ? 0.3f : b;
	}
	
	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}


void CMover::SetHairColor( DWORD dwHairColor )
{
	m_dwHairColor = dwHairColor;
	m_fHairColorR = ((dwHairColor>>16) & 0xff) / 255.f;
	m_fHairColorG = ((dwHairColor>>8) & 0xff)  / 255.f;
	m_fHairColorB = ((dwHairColor) & 0xff)     / 255.f;

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

	m_dwHairColor = D3DCOLOR_COLORVALUE( m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f );
}

// 표정 바꾸기 
void CMover::SetHead( int nHead )
{
	if( nHead < MAX_HEAD )
	{
		m_dwHeadMesh = nHead;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[ 64 ];
		// Hair
		_stprintf( lpszTemp, PARTSMESH_HAIR( GetSex() ), m_dwHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
}
void CMover::SetSkinSet( int nSkinSet )
{
	if( nSkinSet < 2 )
	{
		m_dwSkinSet = nSkinSet;
	}
}


// 이 함수는 전체 의상을 새로 완성하도록 한다.
// 보통 이 함수는 처음에 캐릭터를 생성할 때 한번 호출한다.
void CMover::UpdateParts( BOOL bFakeParts )
{
#ifdef __V050322_CACHEQUEST
	UpdateParts( GetSex(), m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHeadMesh, 
				 m_adwEquipment, (CModelObject*)m_pModel, bFakeParts ? NULL : &m_Inventory, GetProp()->bIfParts, this );
#else // __V050322_CACHEQUEST
//{{AFX
	UpdateParts( GetSex(), m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHeadMesh, 
				 m_adwEquipment, (CModelObject*)m_pModel, bFakeParts ? NULL : &m_Inventory, GetProp()->bIfParts );
//}}AFX
#endif // __V050322_CACHEQUEST
	((CModelObject*)m_pModel)->RestoreDeviceObjects();
	UpdateBoundBox();
	UpdateParam();
}

ItemProp* GetInventoryProp( LPDWORD padwEquipment, CItemContainer< CItemElem  >* pInventory, int nParts )
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;

	if( pInventory == NULL ) // fake only
	{
		if( padwEquipment[ nParts ] != NULL_ID ) 
			pItemProp = prj.GetItemProp( padwEquipment[ nParts ] );
	}
	else
	{
		pItemElem = pInventory->GetEquip( nParts );
		if( pItemElem )
			pItemProp = pItemElem->GetProp();
	}

	return pItemProp;
}
#ifdef __V050322_CACHEQUEST
void CMover::UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, LPDWORD padwEquipment, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts, CMover* pMover )
#else // __V050322_CACHEQUEST
//{{AFX
void CMover::UpdateParts( int nSex, int nSkinSet, int nFace, int nHairMesh, int nHeadMesh, LPDWORD padwEquipment, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts, )
//}}AFX
#endif // __V050322_CACHEQUEST
{
	if( pModel == NULL )
	{
		LPCTSTR szErr = Error( "CMover::UpdateParts : pModel is NULL" );
		ADDERRORMSG( szErr );
		return;
	}
	// 기본 부위(속살, 헤어, 등등)을 세팅한다.
	//LPSKINSET lpSkinset = prj.m_PartsMng.GetSkinSet( nSex, nSkinSet );
	//LPCTSTR lpHair = prj.m_PartsMng.GetHairMeshName( nSex, nHairMesh );
	TCHAR lpszTemp[ 64 ];
	if( bIfParts )
	{
		// Hair
		_stprintf( lpszTemp, PARTSMESH_HAIR( nSex ), nHairMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HAIR );
		// Head
		_stprintf( lpszTemp, PARTSMESH_HEAD( nSex ), nHeadMesh + 1 );
		pModel->LoadElement( lpszTemp, PARTS_HEAD );
	}
	// 먼저 익스크루시브 오브젝트를 TakeOff한다.
	BOOL abExclusiveParts[ MAX_HUMAN_PARTS ];
	ZeroMemory( abExclusiveParts, sizeof( abExclusiveParts ) );
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
		DWORD dwParts = -1;
		CItemElem* pItemElem = NULL;
		ItemProp* pItemProp = NULL;
		// 프로퍼티 꺼냄
		pItemProp = GetInventoryProp( padwEquipment, pInventory, i );

		// 익스클루시브가 있을때
		if( pItemProp && pItemProp->dwExclusive != NULL_ID )
		{
			pModel->TakeOffParts( pItemProp->dwExclusive );	// 익스클루시브를 제거
			abExclusiveParts[ pItemProp->dwExclusive ] = TRUE;		// 익스클루시브 된넘을 기억.
		}
	}
	// 장착부위를 세팅한다.
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		if( i == PARTS_HAIR || i == PARTS_HEAD  )//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;
		DWORD dwParts = -1;
		CItemElem* pItemElem = NULL;
		ItemProp* pItemProp = NULL;
		if( pInventory == NULL ) // fake only
		{
			if( padwEquipment[ i ] != NULL_ID ) 
				pItemProp = prj.GetItemProp( padwEquipment[ i ] );
		}
		else
		{
			pItemElem = pInventory->GetEquip( i );//AtId( padwEquipment[ i ] );
			if( pItemElem )
			{
				pItemProp = pItemElem->GetProp();
			}
		}
		TCHAR szPartsName[ 32 ];

		// 각 파츠의 o3d를 로딩.
		if( pItemProp )
		{
			if( pItemProp->bPartsFile == TRUE )
				prj.m_modelMng.MakePartsName( szPartsName, _T( "part" ), pItemProp->dwID, nSex );
			else
				prj.m_modelMng.MakeModelName( szPartsName, OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
			if( bIfParts || ( i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD ) )
			{
				pModel->LoadElement( szPartsName, i /*pItemProp->dwParts*/ );
			}
		}


		if( abExclusiveParts[ i ] == FALSE )
		{
			switch( i )
			{
			case PARTS_RWEAPON: 
#ifdef  __V050322_CACHEQUEST
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
#else
//{{AFX
				if( pItemProp )	
//}}AFX
#endif
				{
					switch( pItemProp->dwItemKind3 )
					{
					case IK3_KNUCKLEHAMMER:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
						break;
					case IK3_BOW:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
						break;
					case IK3_YOYO:
						((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx() );
						pModel->LoadElement( szPartsName, PARTS_LWEAPON );
						((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
						break;
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
#ifdef  __V050322_CACHEQUEST
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
#else
//{{AFX
				if( pItemProp )	
//}}AFX
#endif
				{
					((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				}
				else 
					pModel->TakeOffParts( i );
				break;
			case PARTS_SHIELD: 
#ifdef  __V050322_CACHEQUEST
				// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
				if( pItemProp && ( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) ) )
#else
//{{AFX
				if( pItemProp )	
//}}AFX
#endif
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
// 장비를 장착한다. 여기서 의상 교체 작업을 해준다.
// nOption은 Fake아이템 장착때만 쓰인다. - xuzhu -
BOOL CMover::DoEquip( int nSex, int nSkinSet, 
					  CItemElem* pItemElem, int nPart, DWORD dwItemIndex, int nOption, CItemContainer< CItemElem  >* pInventory, 
					  LPDWORD padwEquipment, int *paEquipOption, CModelObject* pModel, BOOL bEquip, CMover *pMover ) 
{
	ItemProp* pItemProp = pItemElem ? pItemElem->GetProp() : prj.GetItemProp( dwItemIndex );
	DWORD dwIndex = pItemElem ? pItemElem->m_dwObjIndex : 0;
	DWORD dwParts = pItemProp->dwParts;
	BOOL bIfParts = pMover ? pMover->GetProp()->bIfParts : 0;
	TCHAR lpszTemp[ 64 ];
	BOOL bFake = (pInventory == NULL) ? TRUE : FALSE;
	if( dwParts == NULL_ID )
		return FALSE;

	// 반지/귀걸이 벗을때 처리.
	if( !bEquip )
	{
		if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
		{
			CItemElem *pItemElemTemp;
			
			if( pInventory )
			{
				pItemElemTemp = (CItemElem*)pInventory->GetEquip( dwParts );
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
			ItemProp *pProp = pMover->GetEquipItemProp( pInventory, padwEquipment, PARTS_RWEAPON );
			if( pMover->GetJob() == JOB_BLADE )	// 쌍칼속성
			{
				if( pItemProp->dwID != II_WEA_KNU_ISHOFIST )
				{
					if( pProp && pProp->dwHanded == HD_ONE )	// 오른손에 무기가 있냐? 그 무기가 원핸드냐
					{
						if( pMover->GetEquipItemProp( pInventory, padwEquipment, PARTS_SHIELD ) == NULL )	//왼손에 방패없냐?
							dwParts = PARTS_LWEAPON;		// 장착위치를 왼손으로 바꿈
					} 
				}
			}

		} 
		else
		{
			// 해제시도.
			if( nPart >= 0  )		// 파츠를 직접 지정했을때만
				dwParts = nPart;	// 해제하라고 한 부위를 해제시킨다.
		}
	} // 쌍칼 처리.
	
#ifndef __WORLDSERVER	// <<<< 월드에서 처리안하면 메모리 리크 나지 않나? -xuzhu-
	((CModelObject*)pModel)->TakeOffParts( dwParts );	
#endif


	if( bEquip )	// 장착하려 할때만...
	{
		ItemProp *pHandItemProp	= NULL;
		ItemProp *pLHandItemProp = NULL;

		// 들고있는 무기 프로퍼티 꺼냄.
		pHandItemProp = pMover->GetEquipItemProp( pInventory, padwEquipment, PARTS_RWEAPON );		// 오른손 무기 프로퍼티.
		pLHandItemProp = pMover->GetEquipItemProp( pInventory, padwEquipment, PARTS_LWEAPON );		// 왼손 무기 프로퍼티.
		
		// 조건검사.
		if( pItemProp->dwItemKind3 == IK3_SHIELD )	// 방패류를 착용하려 했을때
		{
			if( pHandItemProp && pHandItemProp->dwHanded == HD_TWO )		// 손에 쥐고 있는게 투핸드면.
			{
				if( pInventory )
				{
					CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	
						return FALSE;
					if( padwEquipment ) padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				} else
				{
					if( padwEquipment )
						padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				}
				pModel->TakeOffParts( PARTS_RWEAPON );
			}
			else
			if( pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON )	// 왼손에 무기가 쥐어져있으면. 왼손에 드는 무기도 PARTS이름은 모두 RWEAPON이다.
			{
				if( pInventory )
				{
					CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_LWEAPON );
					if( pInventory->UnEquip( PARTS_LWEAPON ) == FALSE )	
						return FALSE;
					if( padwEquipment )
						padwEquipment[ PARTS_LWEAPON ] = NULL_ID;
				}
				else
				{
					if( padwEquipment )
						padwEquipment[ PARTS_LWEAPON ] = NULL_ID;
				}
				pModel->TakeOffParts( PARTS_LWEAPON );
			}
		} // IK3_SHIELD
		if( pItemProp->dwHanded == HD_TWO )		// 장착하려는 무기가 투핸드 였을때
		{
			if( pInventory == NULL )
			{
				if( padwEquipment[ PARTS_SHIELD ] != NULL_ID )		// 방패가 있었는가 검사해서 있으면 벗김.
				{
					pModel->TakeOffParts( PARTS_SHIELD );
					if( padwEquipment )
						padwEquipment[ PARTS_SHIELD ] = NULL_ID;
				}
				if( padwEquipment[ PARTS_RWEAPON ] != NULL_ID )		// 무기가 있었는가 검사해서 벗김.
				{
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( padwEquipment )
						padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				}
			}
			else
			{
				CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_SHIELD );
				if( pItemElemOld )
				{
					if( pInventory->UnEquip( PARTS_SHIELD ) == FALSE )	// 방패 벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
					pModel->TakeOffParts( PARTS_SHIELD );
					if( padwEquipment )
						padwEquipment[ PARTS_SHIELD ] = NULL_ID;
				}
				
				pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
				if( pInventory->GetEquip( PARTS_RWEAPON ) )
				{
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	// 무기벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( padwEquipment )
						padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
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
				if( padwEquipment )
					padwEquipment[ pItemProp->dwExclusive ] = NULL_ID;
			}
			// 페이크 장착 
			if( padwEquipment )
			{
				padwEquipment[ dwParts ] = dwItemIndex;
				paEquipOption[ dwParts ] = nOption;			// 페이크 아이템의 옵션을 넣어줌.
			}
		}
		else
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				if( pInventory->GetEquip( pItemProp->dwExclusive ) )
					if( pInventory->UnEquip( pItemProp->dwExclusive ) == FALSE )
						return FALSE;
				pModel->TakeOffParts( pItemProp->dwExclusive );
			}
			// 기존 장비 제거 
			if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
			{
				DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
				CItemElem* pItemElemTemp1 = (CItemElem*)pInventory->GetEquip( dwParts );
				CItemElem* pItemElemTemp2 = (CItemElem*)pInventory->GetEquip( dwParts + 1 );
				if( pItemElemTemp1 ) dwElem1 = pItemElemTemp1->m_dwItemId;
				if( pItemElemTemp2 ) dwElem2 = pItemElemTemp2->m_dwItemId;
				if( dwElem1 != NULL_ID && dwElem2 == NULL_ID )
					dwParts = pItemProp->dwPartsub;
			}

			CItemElem *pItemElemOld = pInventory->GetEquip( dwParts );
			
			if( pItemElemOld )
			{
				//if( pInventory->UnEquip(MAX_INVENTORY + dwParts ) == FALSE )
				if( pInventory->UnEquip( dwParts ) == FALSE )
					return FALSE;
			
			}
			// 새 장비 장착 
			if( TRUE == pInventory->DoEquip( dwIndex, /*MAX_INVENTORY +*/ dwParts ) )
			{
				if( padwEquipment )
					padwEquipment[ dwParts ] = pItemElem->m_dwObjId;

			}
			else
			{
				WriteError( "%s %d", __FILE__, __LINE__ );
				return FALSE;
			}
		} // not Fake

#ifndef __WORLDSERVER
		TCHAR szPartsName[ 32 ];
		if( pItemProp->bPartsFile == TRUE )
			prj.m_modelMng.MakePartsName( szPartsName, _T( "part" ), pItemProp->dwID, nSex );
		else
			prj.m_modelMng.MakeModelName( szPartsName, OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
		switch( dwParts )
		{
		case PARTS_RWEAPON: 
#ifdef  __V050322_CACHEQUEST
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
#endif
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				switch( pItemProp->dwItemKind3 )
				{
				case IK3_KNUCKLEHAMMER:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
					break;
				case IK3_BOW:
					((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
					break;
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
#ifdef  __V050322_CACHEQUEST
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
#endif
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
			}
			break;
		case PARTS_SHIELD: 
#ifdef  __V050322_CACHEQUEST
			// 변신 캐릭터는 장착 무기를 볼 수 없게 하자(파츠 장착만 못함. 수치 계산은 적용됨)
			if( pMover == NULL || ( pMover && pMover->IsDisguise() == FALSE ) )
#endif
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
			}
			break;
		case PARTS_UPPER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
			}
			break;
		case PARTS_LOWER_BODY:
			if( bIfParts )
			{
				{
					((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
					_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
				}
			}
			break;
		default:
			if( bIfParts )
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			break;
		}
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
					_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
					break;
				case PARTS_LOWER_BODY:
					{
						{
							pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
							_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
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
				return FALSE;
			}

		}

		if( padwEquipment ) 
		{
			padwEquipment[ dwParts ] = NULL_ID;
			paEquipOption[ dwParts ] = 0;
		}

#ifndef __WORLDSERVER
		if( bIfParts )
		{
			switch( dwParts )
			{
			case PARTS_UPPER_BODY:
				pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
				break;
			case PARTS_LOWER_BODY:
				pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
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
	} // 탈착


	// 벗길때
	if( bEquip == FALSE )
	{
		// 위에서 오른손 무기를 해제 했었다. 
		if( nPart == PARTS_RWEAPON )
		{
			if( pInventory )
			{
				if( pInventory->GetEquip( PARTS_LWEAPON ) )	// 왼손에 무기가 남아있다.
				{
					//오른손으로 옮겨줌.
				
					int nInvenSize = pInventory->GetSize();
					pInventory->Swap( nInvenSize + PARTS_LWEAPON, nInvenSize + PARTS_RWEAPON );	

				}
			}
			else
			{	// fake
				if( padwEquipment )
				{
					if( padwEquipment[ PARTS_LWEAPON ] != NULL_ID )
					{
						padwEquipment[ PARTS_RWEAPON ] = padwEquipment[ PARTS_LWEAPON ];	// 왼손걸 오른손으로 옮기고
						padwEquipment[ PARTS_LWEAPON ] = NULL_ID;	// 왼손은 지워주고
						paEquipOption[ PARTS_RWEAPON ] = paEquipOption[ PARTS_LWEAPON ];			// 페이크 아이템의 옵션을 넣어줌.
						paEquipOption[ PARTS_LWEAPON ] = 0;			// 페이크 아이템의 옵션을 넣어줌.
					}
				}
			}
		}
	}

#ifdef __YSOUND_EVENT	
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
#endif //__YSOUND_EVENT

	return TRUE;
}

#if 0
//{{AFX
// static DoEquip의 새버전.
BOOL CMover::DoEquip( CItemElem* pItemElem, DWORD dwItemIndex, int nOption, CItemContainer< CItemElem  >* pInventory, 
					  LPDWORD padwEquipment, CModelObject* pModel, BOOL bEquip, CMover *pMover ) 
{
	ItemProp* pItemProp = pItemElem ? pItemElem->GetProp() : prj.GetItemProp( dwItemIndex );
	DWORD dwIndex = pItemElem ? pItemElem->m_dwObjIndex : 0;
	DWORD dwParts = pItemProp->dwParts;
	BOOL bIfParts = pMover ? pMover->GetProp()->bIfParts : 0;
	TCHAR lpszTemp[ 64 ];
	BOOL bFake = (pInventory == NULL) ? TRUE : FALSE;
	if( dwParts == NULL_ID )
		return FALSE;

	// 반지/귀걸이 벗을때 처리.
	if( !bEquip )
	{
		if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
		{
			CItemElem *pItemElemTemp;
			
			if( pInventory )
			{
				pItemElemTemp = (CItemElem*)pInventory->GetEquip( dwParts );
				if( pItemElemTemp != pItemElem )
					dwParts = pItemProp->dwPartsub;
			}
		}
	}

	if( bEquip )
	{
		// 쌍칼(이도류) 처리.
		if( pItemProp && pItemProp->dwHanded == HD_ONE && pItemProp->dwParts == PARTS_RWEAPON )	// 원핸드 무기를 차려고 하는가?
		{
			// 디폴트는 장착위치 오른손
			if( 0 /*직업이 블레이드냐?*/ )	// 쌍칼속성
			{
				if( pMover->GetEquipItemProp( bFake, PARTS_RWEAPON ) )	// 오른손에 무기가 있냐?
				{
					if( pMover->GetEquipItemProp( bFake, PARTS_SHIELD ) == NULL )	//왼손에 방패없냐?
						dwParts = PARTS_LWEAPON;		// 장착위치를 왼손으로 바꿈
				} 
			}
		}
	} else
	{
		// 해제.
		CItemElem *pLHandElem = pInventory->Get&Equip( PARTS_LWEAPON );
		if( pItemElem == pLHandElem )	// 왼손에 장착한 것과 해제하려고 요청한 아이템이 같을때.
			dwParts = PARTS_LWEAPON;	// 파츠 이름을 이걸로 바꿔주자.
	}

	
#ifndef __WORLDSERVER
	((CModelObject*)pModel)->TakeOffParts( dwParts );
#endif

	if( bEquip )	// 장착하려 할때만...
	{
		ItemProp *pHandItemProp	= NULL;
		ItemProp *pLHandItemProp = NULL;

		// 들고있는 무기 프로퍼티 꺼냄.
		if( pInventory == NULL )
		{
			if( padwEquipment[PARTS_RWEAPON] != NULL_ID )
				pHandItemProp = prj.GetItemProp( padwEquipment[PARTS_RWEAPON] );
		} else
		{
			CItemElem *pItemElem = pInventory->GetEquip( PARTS_RWEAPON );		
			if( pItemElem )
				pHandItemProp = pItemElem->GetProp();
		}

		if( pInventory == NULL )
		{
			if( padwEquipment[PARTS_LWEAPON] != NULL_ID )
				pLHandItemProp = prj.GetItemProp( padwEquipment[PARTS_LWEAPON] );
		} else
		{
			CItemElem *pItemElem = pInventory->GetEquip( PARTS_LWEAPON );		
			if( pItemElem )
				pLHandItemProp = pItemElem->GetProp();
		}
		
		// 조건검사.
		if( pItemProp->dwItemKind3 == IK3_SHIELD )	// 방패류를 착용하려 했을때
		{
			if( pHandItemProp )
			{
				if( pHandItemProp->dwHanded == HD_TWO )		// 손에 쥐고 있는게 투핸드면.
				{
//					pModel->TakeOffParts( PARTS_RWEAPON );	// 실패할 수 있으므로 아래로 이동
					if( pInventory )
					{
						CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
						if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	
							return FALSE;
//							ASSERT( 0 );
						if( padwEquipment ) padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
					} else
					{
						if( padwEquipment )
							padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
					}
					pModel->TakeOffParts( PARTS_RWEAPON );
				}
				else
				if( pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON )	// 왼손에 무기가 쥐어져있으면. 왼손에 드는 무기도 PARTS이름은 모두 RWEAPON이다.
				{
					if( pInventory )
					{
						CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_LWEAPON );
						if( pInventory->UnEquip( PARTS_LWEAPON ) == FALSE )	
							return FALSE;
					
						if( padwEquipment ) padwEquipment[ PARTS_LWEAPON ] = NULL_ID;
					} else
					{
						if( padwEquipment )
							padwEquipment[ PARTS_LWEAPON ] = NULL_ID;
					}
					pModel->TakeOffParts( PARTS_LWEAPON );
				}
			}
		}
		if( pItemProp->dwHanded == HD_TWO )		// 장착하려는 무기가 투핸드 였을때
		{
			if( pInventory == NULL )
			{
				if( padwEquipment[ PARTS_SHIELD ] != NULL_ID )		// 방패가 있었는가 검사해서 있으면 벗김.
				{
					pModel->TakeOffParts( PARTS_SHIELD );
					if( padwEquipment )
						padwEquipment[ PARTS_SHIELD ] = NULL_ID;
				}
				if( padwEquipment[ PARTS_RWEAPON ] != NULL_ID )		// 무기가 있었는가 검사해서 벗김.
				{
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( padwEquipment )
						padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				}
			}
			else
			{
				CItemElem *pItemElemOld = pInventory->GetEquip( PARTS_SHIELD );
				if( pItemElemOld )
				{
					if( pInventory->UnEquip( PARTS_SHIELD ) == FALSE )	// 방패 벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
					pModel->TakeOffParts( PARTS_SHIELD );
					if( padwEquipment ) padwEquipment[ PARTS_SHIELD ] = NULL_ID;
				}
				
				pItemElemOld = pInventory->GetEquip( PARTS_RWEAPON );
				if( pInventory->GetEquip( PARTS_RWEAPON ) )
				{
					if( pInventory->UnEquip( PARTS_RWEAPON ) == FALSE )	// 무기벗김
						return FALSE;	// 인벤이 꽉차면 걍 취소
					pModel->TakeOffParts( PARTS_RWEAPON );
					if( padwEquipment ) padwEquipment[ PARTS_RWEAPON ] = NULL_ID;
				}
			}
		}
	}

	if( bEquip )
	{
		if( pInventory == NULL )
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				pModel->TakeOffParts( pItemProp->dwExclusive );
				if( padwEquipment )
					padwEquipment[ pItemProp->dwExclusive ] = NULL_ID;
			}
			// 페이크 장착 
			if( padwEquipment )
			{
				padwEquipment[ dwParts ] = dwItemIndex;
				paEquipOption[ dwParts ] = nOption;			// 페이크 아이템의 옵션을 넣어줌.
			}
		}
		else
		{
			// 익스크루시브 아이템 제거 
			if( pItemProp->dwExclusive != NULL_ID )
			{
				//if( pInventory->GetAt( MAX_INVENTORY + pItemProp->dwExclusive ) )
				if( pInventory->GetEquip( pItemProp->dwExclusive ) )
					if( pInventory->UnEquip( pItemProp->dwExclusive ) == FALSE )
						return FALSE;
					//if( pInventory->UnEquip( MAX_INVENTORY + pItemProp->dwExclusive ) == FALSE )
//						ASSERT( 0 );
				pModel->TakeOffParts( pItemProp->dwExclusive );
			}
			// 기존 장비 제거 
			//if( pInventory->GetAt( MAX_INVENTORY + dwParts ) )
			if( bEquip )
			{
				if( dwParts == PARTS_RING1 || dwParts == PARTS_EARRING1 )
				{
					DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
					CItemElem* pItemElemTemp1 = (CItemElem*)pInventory->GetEquip( dwParts );
					CItemElem* pItemElemTemp2 = (CItemElem*)pInventory->GetEquip( dwParts + 1 );
					if( pItemElemTemp1 ) dwElem1 = pItemElemTemp1->m_dwItemId;
					if( pItemElemTemp2 ) dwElem2 = pItemElemTemp2->m_dwItemId;
					if( dwElem1 != NULL_ID && dwElem2 == NULL_ID )
						dwParts = pItemProp->dwPartsub;
				}
			}

			CItemElem *pItemElemOld = pInventory->GetEquip( dwParts );
			
			if( pItemElemOld )
			{
				//if( pInventory->UnEquip(MAX_INVENTORY + dwParts ) == FALSE )
				if( pInventory->UnEquip( dwParts ) == FALSE )
					return FALSE;
				
			}
			// 새 장비 장착 
			if( TRUE == pInventory->DoEquip( dwIndex, /*MAX_INVENTORY +*/ dwParts ) )
			{
				if( padwEquipment )
					padwEquipment[ dwParts ] = pItemElem->m_dwObjId;
			}
			else
			{
				WriteError( "%s %d", __FILE__, __LINE__ );
				return FALSE;
			}
		}

#ifndef __WORLDSERVER
		TCHAR szPartsName[ 32 ];
		if( pItemProp->bPartsFile == TRUE )
			prj.m_modelMng.MakePartsName( szPartsName, _T( "part" ), pItemProp->dwID, nSex );
		else
			prj.m_modelMng.MakeModelName( szPartsName, OT_ITEM, pItemProp->dwID );
			//prj.m_modelMng.MakePartsName( szPartsName, _T( "item" ), pItemProp->dwID );
		switch( dwParts )
		{
		case PARTS_RWEAPON: 
			((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			switch( pItemProp->dwItemKind3 )
			{
			case IK3_KNUCKLEHAMMER:
				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx() );
				break;
			case IK3_BOW:
				((CModelObject*)pModel)->SetParent( PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
				break;
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
			break;
		case PARTS_LWEAPON: 
			((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			((CModelObject*)pModel)->SetParent( PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx() );
			break;
		case PARTS_SHIELD: 
			((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			((CModelObject*)pModel)->SetParent( PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx() );
			break;
		case PARTS_UPPER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
			}
			break;
		case PARTS_LOWER_BODY:
			if( bIfParts )
			{
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
				_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_LOWER_BODY, TEX_PART_LOWER( nSex ), lpszTemp );
			}
			break;
		default:
			if( bIfParts )
				((CModelObject*)pModel)->LoadElement( szPartsName, dwParts );
			break;
		}
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
					_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
					pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
					break;
				case PARTS_LOWER_BODY:
					pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
					_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
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
		}
		//if( pInventory && pInventory->GetAt( MAX_INVENTORY + dwParts ) )
		if( pInventory && pInventory->GetEquip( dwParts ) )
		{
			//if( pInventory->UnEquip( MAX_INVENTORY + dwParts ) == FALSE )
			if( pInventory->UnEquip( dwParts ) == FALSE )
			{
				return FALSE;
			}
		}
		
		if( padwEquipment ) 
		{
			padwEquipment[ dwParts ] = NULL_ID;
			paEquipOption[ dwParts ] = 0;
		}

#ifndef __WORLDSERVER
		if( bIfParts )
		{
			switch( dwParts )
			{
			case PARTS_UPPER_BODY:
				pModel->LoadElement( PARTSMESH_UPPER( nSex ), PARTS_UPPER_BODY );
				_stprintf( lpszTemp, PARTSTEX_UPPER( nSex ), nSkinSet + 1 );
				pModel->ChangeTexture( PARTS_UPPER_BODY, TEX_PART_UPPER( nSex ), lpszTemp );
				break;
			case PARTS_LOWER_BODY:
				pModel->LoadElement( PARTSMESH_LOWER( nSex ), PARTS_LOWER_BODY );
				_stprintf( lpszTemp, PARTSTEX_LOWER( nSex ), nSkinSet + 1 );
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
	} // 탈착
	return TRUE;
}
//}}AFX
#endif // 0


// 장착 가능한가를 검사.
BOOL CMover::IsEquipAble( ItemProp *pItemProp, OBJID dwObjid )
{
	// 빗자루류 검사.
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		int nLimitLv = pItemProp->dwFlightLimit;
		if( nLimitLv == NULL_ID )
			nLimitLv = 1;
		
		CWorld* pWorld = GetWorld();
		if( pWorld )
		{
			int nAttr	= pWorld->GetHeightAttribute( (int)( GetPos().x ), (int)( GetPos().z ) );
			
			if( GetFlightLevel() < nLimitLv )		// 비행레벨이 안되면 못탄다.
			{
				return FALSE;
			}
			
			if( !pWorld->m_bFly )
			{
				return FALSE;
			}

			// 장착 이전에 검사하여야 한다.
			if( nAttr == HATTR_NOFLY )		// 비행금지구역에선 못탄다.
				return FALSE;
		
		}
	}

	// 무기류가 아닌것(방어구)는 성별을 확인한다.
	{
		// 성별 확인( 무기는 빠짐 )
		if( pItemProp->dwItemSex != NULL_ID && pItemProp->dwItemSex != GetSex() )
		{
			return FALSE;
		} // 성별
	}// 무기
	
	// 방어구 입을때 직업확인.
	if( pItemProp->dwItemKind1 != IK1_WEAPON )
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) ) {
			return FALSE;
		}
	}

	// 아이템 필요직업검사.
	if( pItemProp->dwItemKind1 == IK1_WEAPON )		// 무기류를 장착하려할때
	{
		if( pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob( pItemProp->dwItemJob ) )	// 아이템에 필요직업이 지정되어 있고
		{
			return FALSE;
		}
	}

//	if( prj.m_nLanguage	!= LANG_CHI && prj.m_nLanguage != LANG_TWN )
//#ifdef __TAIWAN__
//	if( GetLevel() >= 60 )
//#endif	// __TAIWAN
		// 아이템 필요레벨 검사.
		if( pItemProp->dwLimitLevel1 != 0xffffffff )	// 필요레벨이 지정되어 있고
		{
			if( m_nLevel < (int)( pItemProp->dwLimitLevel1 ) )	// 아이템을 찰 레벨이 안되었으면 못참.
			{
#	ifdef __WORLDSERVER
				if( TRUE == IsPlayer() )
					( (CUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItemProp->dwLimitLevel1 );
#	endif	// __WORLDSERVER
				return FALSE;
			}
		}
		// 보석류를 장착
		if( pItemProp->dwItemKind2 == IK2_JEWELRY )
		{
			if( pItemProp->dwLimitLevel1 != 0xffffffff )	// 제한레벨이 걸려있고
			{
				if( m_nLevel < (int)( pItemProp->dwLimitLevel1 ) )	// 레벨이 안되면 못참.
				{
#	ifdef __WORLDSERVER
					if( TRUE == IsPlayer() )
						( (CUser*)this )->AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItemProp->dwLimitLevel1 );
#	endif	// __WORLDSERVER
					return FALSE;
				}
			}
		}



	ItemProp *pHandItemProp	= NULL;
	
	// 들고있는 무기 프로퍼티 꺼냄.
	CItemElem *pItemElem = m_Inventory.GetEquip( PARTS_RWEAPON );		
	if( pItemElem )
		pHandItemProp = pItemElem->GetProp();

	// 화살을 착용하려 했을때 보우일때만 착용돼야 함
	if( pItemProp->dwItemKind3 == IK3_ARROW && ( pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_BOW ) )
		return FALSE;			

	return TRUE;
}

//
//
// pItemElem이 벗는게 가능한가를 검사.
BOOL CMover::IsUnEquipAble( ItemProp *pItemProp )
{
	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( m_pActMover->IsSit() )	// 빗자루는 앉은 상태에서는 탈착 금지
			return FALSE;

		if( GetWorld() )	// RedoEquip에서 불렀을때 NULL인경우 있음. NULL이면 걍 벗김
		{
			int nAttr = GetWorld()->GetHeightAttribute( (int)( GetPos().x ), (int)( GetPos().z ) );		// 이동할 위치의 속성 읽음.
			// 비행금지 or 걷기금지 or 이동금지 지역에서 는 못내림.
			if( (nAttr == HATTR_NOFLY || nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE ) )		
				return FALSE;
		}
	}


	return TRUE;
}

#if !defined(__CLIENT) || defined(__ONLINE)		// 온라인(클라) || 비클라
//
//
// 정상적인 아이템 장착, 탈착 
BOOL CMover::DoEquip( CItemElem* pItemElem, BOOL bEquip, int nPart )
{
	ItemProp* pItemProp = pItemElem->GetProp();

	if( IsDie() )	// 죽은 상태에서는 장/탈착 금지
	{
		return FALSE;
	}

	if( bEquip )
	{
		if( pItemProp->dwParts == PARTS_RIDE && m_Inventory.GetEquip( PARTS_RIDE ) )
			return FALSE;
		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId ) == FALSE )
			return FALSE;
	}
	else
	{
		if( IsUnEquipAble( pItemProp ) == FALSE )
			return FALSE;
	}

		BOOL bResult = DoEquip( GetSex(), m_dwSkinSet, pItemElem, nPart, 0, pItemElem->GetAttrOption(), &m_Inventory, NULL, m_anEquipOption, (CModelObject*)m_pModel, bEquip, this );
		
	if( !bResult )	// 2004/04/27
		return FALSE;

#ifndef __ONLINE
	if( bEquip )
	{
		SetDestParamEquip( pItemProp, pItemElem );		// 장착한 장비의 DestParam설정
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
#endif	// __ONLINE

	if( pItemProp->dwParts == PARTS_RIDE )
	{
		if( bEquip )
		{
			SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );
		}
		else
		{
			SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );
		}
		ClearDest();	// 2004/04/30
		ClearDestAngle();
		memset( &m_CorrAction, 0, sizeof(CORR_ACTION) );
	}

	// 망토 입을때 처리.
	if( bEquip )
	{
		if( pItemProp->dwID == II_ARM_S_CLO_CLO_BLANK )		// 길드전용 민짜 망토.
		{
			m_idGuildCloak = pItemElem->m_idGuild;			// 망토에 박혀있던 길드아이디를 받음.
#ifdef __XCLOAK
			m_pCloakTexture = g_GuildTexture.LoadGuildTexture( pItemElem->m_idGuild );
#endif
		} else
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
#else // 온라인버전
//{{AFX
// 싱글 클라이언트 버젼 (디버깅 및 데모용)
// 정상적인 아이템 장착, 탈착 
BOOL CMover::DoEquip( CItemElem* pItemElem, BOOL bEquip, int nPart )
{
	ItemProp* pItemProp = pItemElem->GetProp();
 //tatic BOOL	   	   DoEquip( int nSex, int nSkinSe, pItemElem, p, i, int nOption, CItemContainer< CItemElem  >* pInventory, LPDWORD padwEquipment, int *paEquipOption, CModelObject* pModel, BOOL bEquip, CMover *pMover );
	BOOL bResult = DoEquip( GetSex(), m_dwSkinSet, pItemElem, 0, 0, 0, &m_Inventory, NULL, m_anEquipOption, (CModelObject*)m_pModel, bEquip, this );
	if( bEquip )
	{
		if( pItemProp->dwDestParam1 != -1 )
			SetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->dwChgParamVal1, 1 );
		if( pItemProp->dwDestParam2 != -1 )
			SetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->dwChgParamVal2, 1 );
		if( pItemProp->dwParts == PARTS_RIDE )
		{
			SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID );
			ClearDest();
		}
	}
	else
	{
		if( pItemProp->dwDestParam1 != -1 )
			ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, 1 );
		if( pItemProp->dwDestParam2 != -1 )
			ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, 1 );
		if( pItemProp->dwItemKind1 == IK1_WEAPON ) 
		{
			if( m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) )
				SendActMsg( OBJMSG_MODE_PEACE );
		}

		if( pItemProp->dwParts == PARTS_RIDE )
		{
			SendActMsg( OBJMSG_MODE_GROUND );
			ClearDest();
		}
	}
	UpdateBoundBox();
	UpdateParam();

	return bResult;
}
//}}AFX
// 싱글버전 끝
#endif  //  싱글버전 


void CMover::RedoEquip( BOOL fFakeParts )
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp	= NULL;
	BOOL bEquip;

	// 장착될 아이템의 선 처리 
	for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
	{
		bEquip	= fFakeParts? ( m_adwEquipment[i] != NULL_ID ): ( m_Inventory.GetEquip( i ) != NULL );
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
				pItemProp		= prj.GetItemProp( m_adwEquipment[i] );		// Fake 장비
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
//						WriteLog( "RedoEquip(): OBJSTAF_FLY" );
						SendActMsg( OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1 );	// 비행모드로 전환
					}
					if( m_dwRideItemIdx == 0 )	// 빗자루 ID가 없으면
					{
//						WriteLog( "RedoEquip(): m_dwRideItemIdx	= pItemProp->dwID" );
						m_dwRideItemIdx	= pItemProp->dwID;	// 값 보정.
					}
				} 
				if( pItemProp->dwID == II_ARM_S_CLO_CLO_BLANK )		// 길드전용 민짜 망토.
				{
					if( pItemElem )		// 서버 / 클라(액티브무버)
						m_idGuildCloak = pItemElem->m_idGuild;
 #ifdef __XCLOAK
					m_pCloakTexture = g_GuildTexture.LoadGuildTexture( m_idGuildCloak );
 #endif
				}
				if( pItemElem )
					m_anEquipOption[i] = pItemElem->GetAttrOption();
			
				// 장착한 아이템의 DestParam설정.
				SetDestParamEquip( pItemProp, pItemElem );
			}
			else
			{
				WriteLog( "RedoEquip(): Illegal dwParts, %s, %d", m_szName, i );
				m_adwEquipment[i]	= NULL_ID;
			}
		}
		else 
		{	// 장착될게 없음.
			if( i == PARTS_RIDE )	// 빗자루가 장착되어 있지 않은데
			{
				if( m_pActMover->IsFly() )	// 비행중이다.
				{
//					WriteLog( "RedoEquip(): OBJSTAF_GROUND" );
					SendActMsg( OBJMSG_MODE_GROUND, 0, 0, 1 );	// 그러면 지상모드로 변환 시켜야지.
				}
				if( m_dwRideItemIdx != 0 )		// 빗자루 인덱스도 0이어야 하는데 뭔가 있다.
				{
//					WriteLog( "RedoEquip(): m_dwRideItemIdx	= 0" );
					m_dwRideItemIdx		= 0;	// 그러면 0으로 클리어.
				}
				if( m_Inventory.GetEquip( i ) )
				{
//					WriteLog( "RedoEquip(): pRide is not null" );
					m_Inventory.UnEquip( i );
				}
			}
		}
	}


	pItemElem	= m_Inventory.GetEquip( PARTS_RWEAPON );		// 위에 루프에서 모든파츠를 이렇게 일괄적으로 넣어도 될듯 싶다.
	if( pItemElem )
	{
		m_adwEquipment[ PARTS_RWEAPON ] = pItemElem->GetAttrOption();
	}
	pItemElem	= m_Inventory.GetEquip( PARTS_LWEAPON );
	if( pItemElem )
	{
		m_adwEquipment[ PARTS_LWEAPON ] = pItemElem->GetAttrOption();
	}

	// 아이템이 실제 장착.
	UpdateParts( fFakeParts );



}

// 장착되어설 안될 장비가 장착되어 있다면 벗김
int	CMover::InvalidEquipOff( BOOL bFakeParts )
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp	= NULL;
	int		i;

	for( i = 0; i < MAX_HUMAN_PARTS; i ++ )
	{
		pItemElem = NULL;		
		pItemProp = NULL;

		// 장착된 아이템의 프로퍼티 꺼냄.
		if( bFakeParts )	// Fake 장비
		{
			if( m_adwEquipment[i] == NULL_ID )		continue;
			pItemProp		= prj.GetItemProp( m_adwEquipment[i] );		
		}
		else
		{
			pItemElem = m_Inventory.GetEquip( i );
			if( pItemElem )
				pItemProp  = pItemElem->GetProp();	
		}

		if( pItemProp == NULL )	
			continue;	// 프로퍼티 없으면 실패.

		if( IsEquipAble( pItemProp, pItemElem->m_dwObjId ) == FALSE )	// 장착할 수 없는 아이템이다.
		{
			if( pItemElem )
			{
				if( DoEquip( pItemElem, FALSE ) == FALSE )		// 벗김.
				{
				}
			} 
		}
	} // for
	
	return FALSE;
}

void CMover::SetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem )
{

}

void CMover::ResetDestParamEquip( ItemProp* pItemProp, CItemElem* pItemElem )
{

	if( pItemProp->dwDestParam1 != -1 )
		ResetDestParam( pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, 1 );
	if( pItemProp->dwDestParam2 != -1 )
		ResetDestParam( pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, 1 );
	
	if( pItemElem && pItemElem->m_nResistAbilityOption && pItemElem->GetProp()->dwItemKind1 == IK1_ARMOR )
	{
#ifdef __THAILAND_RESIST			
		float fResult = ((float)pItemElem->m_nResistAbilityOption+1.2f) * (0.7f+(float)(pItemElem->m_nResistAbilityOption*0.01f));
		int nResult;
		nResult = fResult * fResult;
#endif //__THAILAND_RESIST

		if( pItemElem->m_bItemResist == SAI79::FIRE )
		{
#ifdef __THAILAND_RESIST			
			ResetDestParam( DST_RESIST_FIRE, nResult, 1 );
			ResetDestParam( DST_RESIST_WATER, -nResult/2, 1 );
			ResetDestParam( DST_RESIST_WIND, nResult/2, 1 );
#else //__THAILAND_RESIST
			ResetDestParam( DST_RESIST_WATER, -pItemElem->m_nResistAbilityOption*2, 1 );
			ResetDestParam( DST_RESIST_WIND, pItemElem->m_nResistAbilityOption*2, 1 );
#endif //__THAILAND_RESIST
		}
		else
		if( pItemElem->m_bItemResist == SAI79::WATER )
		{
#ifdef __THAILAND_RESIST			
			ResetDestParam( DST_RESIST_WATER, nResult, 1 );
			ResetDestParam( DST_RESIST_ELECTRICITY, -nResult/2, 1 );
			ResetDestParam( DST_RESIST_FIRE, nResult/2, 1 );
#else //__THAILAND_RESIST
			ResetDestParam( DST_RESIST_ELECTRICITY, -pItemElem->m_nResistAbilityOption*2, 1 );
			ResetDestParam( DST_RESIST_FIRE, pItemElem->m_nResistAbilityOption*2, 1 );
#endif //__THAILAND_RESIST
		}
		else
		if( pItemElem->m_bItemResist == SAI79::ELECTRICITY )
		{
#ifdef __THAILAND_RESIST			
			ResetDestParam( DST_RESIST_ELECTRICITY, nResult, 1 );
			ResetDestParam( DST_RESIST_EARTH, -nResult/2, 1 );
			ResetDestParam( DST_RESIST_WATER, nResult/2, 1 );
#else //__THAILAND_RESIST
			ResetDestParam( DST_RESIST_EARTH, -pItemElem->m_nResistAbilityOption*2, 1 );
			ResetDestParam( DST_RESIST_WATER, pItemElem->m_nResistAbilityOption*2, 1 );
#endif //__THAILAND_RESIST
		}
		else
		if( pItemElem->m_bItemResist == SAI79::EARTH )
		{
#ifdef __THAILAND_RESIST			
			ResetDestParam( DST_RESIST_EARTH, nResult, 1 );
			ResetDestParam( DST_RESIST_WIND, -nResult/2, 1 );
			ResetDestParam( DST_RESIST_ELECTRICITY, nResult/2, 1 );
#else //__THAILAND_RESIST
			ResetDestParam( DST_RESIST_WIND, -pItemElem->m_nResistAbilityOption*2, 1 );
			ResetDestParam( DST_RESIST_ELECTRICITY, pItemElem->m_nResistAbilityOption*2, 1 );
#endif //__THAILAND_RESIST
		}
		else
		if( pItemElem->m_bItemResist == SAI79::WIND )
		{
#ifdef __THAILAND_RESIST			
			ResetDestParam( DST_RESIST_WIND, nResult, 1 );
			ResetDestParam( DST_RESIST_FIRE, -nResult/2, 1 );
			ResetDestParam( DST_RESIST_EARTH, nResult/2, 1 );
#else //__THAILAND_RESIST
			ResetDestParam( DST_RESIST_FIRE, -pItemElem->m_nResistAbilityOption*2, 1 );
			ResetDestParam( DST_RESIST_EARTH, pItemElem->m_nResistAbilityOption*2, 1 );
#endif //__THAILAND_RESIST
		}
	} 
	else
	{	
		if( pItemProp->nItemResistElecricity )
			ResetDestParam( DST_RESIST_ELECTRICITY, pItemProp->nItemResistElecricity, 1 );
		if( pItemProp->nItemResistFire )
			ResetDestParam( DST_RESIST_FIRE, pItemProp->nItemResistFire, 1 );
		if( pItemProp->nItemResistWind )
			ResetDestParam( DST_RESIST_WIND, pItemProp->nItemResistWind, 1 );
		if( pItemProp->nItemResistWater )
			ResetDestParam( DST_RESIST_WATER, pItemProp->nItemResistWater, 1 );
		if( pItemProp->nItemResistEarth )
			ResetDestParam( DST_RESIST_EARTH, pItemProp->nItemResistEarth, 1 );
	}

	if( pItemElem )
	{
	}
}

// 돈획득 	
void CMover::PickupGold( int nGold, BOOL bDropMob )
{
	ASSERT( nGold > 0 );
	if( nGold <= 0 )
	{
		Error( "%s PickupGold - %d", GetName(), nGold );
		return;
	}


	{
		// 파티 없을때
		PickupGoldCore( nGold );
	}

}


void CMover::PickupGoldCore( int nGold )
{
	if( AddGold( nGold ) == FALSE )
		return;

}

