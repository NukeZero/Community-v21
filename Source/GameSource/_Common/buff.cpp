#include "stdafx.h"

#include "buff.h"

#ifdef __WORLDSERVER
#include "../WorldServer/user.h"
#include "FLSkillSystem.h"
#include "../_CommonDefine/Packet/FLPacketWDB_ConsignmentSale.h"

#include "../_Common/party.h"
extern CPartyMng	g_PartyMng;
#endif	// __WORLDSERVER

#ifdef	__CLIENT
#include "FLSkillSystem.h"
#endif

////////////////////////////////////////////////////////////////////////////////
IBuff::IBuff()
: m_wType( 0 )
, m_wId( 0 )
, m_dwLevel( 0 )
, m_tmTotal( 0 )
, m_tmInst( 0 )
, m_tmPain( 0 )
, m_tmPainTick( 0 )
, m_oiAttacker( NULL_ID )
#ifdef __CLIENT
, m_bSFX( FALSE )
, m_pSfx( NULL )
#endif	// __CLIENT
, m_bRemove( FALSE )
, m_nAdjValue( 0 )
, m_bExpire( FALSE )
{
}

IBuff::~IBuff( )
{
#ifdef __CLIENT
	TerminateWorking( );
#endif
}

#ifdef __CLIENT
//_SUN_SKILLSFX_TO_LUA
void IBuff::TerminateWorking( )
{
	//gmpbigsun : Mover에 지워져야할 인덱스를 유지하고 CSfx객체 내부에서 Index비교검사후 제거 했었음.
	// XI_*** 제거함 (IBuff가 생성소멸을 담당)
	//소멸자에서 호출되며, 즉각 생성했던 sfx를 제거한다.
	SAFE_DELETE( m_pSfx );
}
#endif

bool	IBuff::IsPainTime( DWORD dwNowTick )
{
	if( m_tmPain == 0 )
		return false;

	DWORD dwElapsedTime = dwNowTick - m_tmPainTick;
	if( dwElapsedTime >= m_tmPain )
	{
		m_tmPainTick = dwNowTick;
		return true;
	}

	return false;
}

void	IBuff::SetPain( DWORD dwPain, BYTE byBuffTickType, DWORD dwNowTick )
{
	if( dwPain == NULL_ID )
	{
		dwPain = 0;
	}

	m_tmPain = dwPain;
	m_tmPainTick = dwNowTick;
	if( byBuffTickType == BT_START )
	{
		m_tmPainTick -= m_tmPain;
	}
}

void IBuff::SetTotal( DWORD tmTotal )
{
//	if( tmTotal > GetRemain() )
//		m_tmTotal = tmTotal;
	m_tmTotal	= max( tmTotal, GetRemain() );
}

#ifndef __DBSERVER
BOOL IBuff::IsAddable( CMover* /*pMover*/ )
{
	// mirchang_?
// 	int nTotal	= static_cast<int>( GetTotal() );
// 	FLASSERT( nTotal >= 0 );
// 	if( nTotal < 0 )
// 	{
// 		return FALSE;
// 	}
	return TRUE;
}

void IBuff::AddTotal( DWORD tmTotal )
{
	SetTotal( tmTotal );
	SetInst( ::timeGetTime() );		// +
}

#endif	// __DBSERVER

DWORD IBuff::GetRemain()
{
	if( GetInst() == 0 )
	{
		return 0;
	}
	
	DWORD dwElapsedTime = ::timeGetTime() - GetInst();
	if( dwElapsedTime >= GetTotal() )	// 남은 시간은 총 버프 시간보다 클 수 없다.
	{
		return 0;						// 실제로 버프 제거는 시간 만료 후 다음 프로세스에서 하기 때문에 발생 가능하다.
	}

	return GetTotal() - dwElapsedTime;
}

int		IBuff::GetAdjValue()
{
	return m_nAdjValue;
}

void	IBuff::SetAdjValue( int nAdjValue )
{
	m_nAdjValue = nAdjValue;
}

bool	IBuff::IsAbsoluteTimeBuff()
{
	if( m_wType == BUFF_ITEM && GetSpecItem() != NULL && GetSpecItem()->IsAbsoluteTime() == TRUE )
	{
		return true;
	}

	return false;
}

PT_ITEM_SPEC	IBuff::GetSpecItem()
{
	return g_xSpecManager->GetSpecItem( GetId() );
}

SkillProp*		IBuff::GetPropSkill()
{
	return prj.GetSkillProp( GetId() );
}

IBuff& IBuff::operator =( const IBuff & buff )
{
	m_wType			= buff.m_wType;
	m_wId			= buff.m_wId;
	m_dwLevel		= buff.m_dwLevel;
	m_tmTotal		= buff.m_tmTotal;
	m_tmInst		= buff.m_tmInst;
	m_tmPain		= buff.m_tmPain;
	m_tmPainTick	= buff.m_tmPainTick;
	m_oiAttacker	= buff.m_oiAttacker;
#ifdef __CLIENT
	m_bSFX			= buff.m_bSFX;
#endif	// __CLIENT

	m_nAdjValue		= buff.m_nAdjValue;
	return *this;
}

void IBuff::SerializeLevel( CAr & ar )
{
	if( ar.IsStoring() )
	{
//#ifdef __DBSERVER
		if( IsAbsoluteTimeBuff() == true )
		{
			time_t t	= static_cast<time_t>( GetLevel() ) - time_null();
			ar << t;
		}
		else
		{
			ar << m_dwLevel;
		}
//#else	// __DBSERVER
//		ar << m_dwLevel;
//#endif	// __DBSERVER
	}
	else
	{
//#ifdef __DBSERVER
		if( IsAbsoluteTimeBuff() == true )
		{
			time_t t;
			ar >> t;
			SetLevel( time_null() + t );
		}
		else
		{
			ar >> m_dwLevel;
		}
//#else	// __DBSERVER
//		ar >> m_dwLevel;
//#endif	// __DBSERVER
	}
}

void IBuff::Serialize( CAr & ar, CMover* 
#ifdef __WORLDSERVER
					  pMover 
#endif 
					  )
{
	
	if( ar.IsStoring() )
	{
		ar << m_wType;
		ar << m_wId;
		SerializeLevel( ar );
#ifdef __WORLDSERVER
		DWORD tmTotal	= 0;
		if( GetTotal() > 0 )
		{
			if( GetInst() > 0 )
			{
				DWORD dwNowTick = ::timeGetTime();
				tmTotal = ( dwNowTick - GetInst() ) >= GetTotal() ? 0 : ( GetTotal() - ( dwNowTick - GetInst() ) );
				//tmTotal	= GetTotal() - ( g_tmCurrent - GetInst() );
			}
			else
			{
				tmTotal	= GetTotal();
			}
		}
//		if( static_cast<int>( tmTotal ) < 0 )
//			tmTotal	= 0;
		ar << tmTotal;
#else	// __WORLDSERVER
		ar << m_tmTotal;
#endif	// __WORLDSERVER

		ar << m_nAdjValue;
	}
	else
	{
		ar >> m_wId;
		SerializeLevel( ar );
		DWORD tmTotal;
		ar >> tmTotal;
#ifdef __WORLDSERVER
		if( IsCommon() && pMover && tmTotal >= MIN( 60 ) )
			tmTotal	= MIN( 60 );
#endif	// __WORLDSERVER
		SetTotal( tmTotal );

		ar >> m_nAdjValue;

		//////////////////////////////////////////////////////////////////////////
		DWORD dwPain = NULL_ID;
		BYTE byBuffTickType = UCHAR_MAX;// numeric_limits< BYTE >::max(); //static_cast< BYTE >( NULL_ID );
#ifdef __WORLDSERVER
		if( GetType() == BUFF_SKILL )
		{
			SkillProp* pSkillProp = prj.GetSkillProp( GetId() );
			if( pSkillProp != NULL )
			{
				dwPain = pSkillProp->tmContinuousPain;
				byBuffTickType = pSkillProp->byBuffTickType;

				AddSkillProp *pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, GetLevel() );
				if( pAddSkillProp != NULL )
				{
					dwPain = pAddSkillProp->dwPainTime;
				}
			}
		}
#endif
		SetPain( dwPain, byBuffTickType, ::GetTickCount() );
		//////////////////////////////////////////////////////////////////////////
	}
}

#ifndef __DBSERVER
DWORD IBuff::GetRemainSec()
{
	if( IsAbsoluteTimeBuff() == true )
	{
		time_t	t	= time_null();
		if( static_cast<DWORD>( t ) < GetLevel() )
		{
			return GetLevel() - static_cast<DWORD>( t );
		}
	}
	else
	{
		DWORD dwCurrentTick = ::timeGetTime();
		if( dwCurrentTick > GetInst() && dwCurrentTick - GetInst() < GetTotal() )
		{
			return ( GetTotal() - ( dwCurrentTick - GetInst() ) ) / 1000;
		}
	}

	return 0;
}

BOOL IBuff::Timeover( CMover* /*pMover*/, DWORD tmCurrent )
{
	if( IsAbsoluteTimeBuff() == true )
	{
		time_t	t	= time_null();
		if( (DWORD)( t ) >= GetLevel() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if( tmCurrent < GetInst() )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "IBuff::Timeover. GetInst() Overflow. tmCurrent:[%d], GetInst():[%d]" ), tmCurrent, GetInst() );
			return TRUE;
		}

		if( tmCurrent - GetInst() >= GetTotal() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL IBuff::Expire( CMover* pMover, DWORD tmCurrent )
{
	if( IsRemovable( pMover ) && Timeover( pMover, tmCurrent ) )
	{
#ifdef __WORLDSERVER
		OnExpire( pMover );
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

BOOL IBuff::Process( CMover* pMover, DWORD tmCurrent )
{
	if( GetRemove() )
	{
		return TRUE;
	}
#ifdef __WORLDSERVER
	if( IsValid( pMover ) == FALSE )
	{
		return TRUE;
	}
#endif // __WORLDSERVER
	if( pMover )
	{
#ifdef __CLIENT
		CreateSFX( pMover );

		if( m_pSfx )
		{
			m_pSfx->Process();
		}

#endif	// __CLIENT

		if( GetInst() == 0 )
		{
			Apply( pMover );
			SetInst( tmCurrent );
		}

		if( IsPainTime( tmCurrent ) == true )
		{
			ApplyPain( pMover );
		}
	}
	return Expire( pMover, tmCurrent );
}
#endif	// __DBSERVER

#ifdef __CLIENT
void IBuff::CreateSFX( CMover* pMover )
{
	//gmpbigsun: 이제는 sfx를 월드에 등록하지 않고 무버가 관리한다.
	//CreateSfx의 마지막 인자를 주목하라. 월드에 관리를 맞길것인가에 대한 BOOL값

	if( GetType() == BUFF_SKILL )
	{
		SkillProp* pSkillProp	= GetPropSkill();

		if( pSkillProp && !HasSFX() )
		{
			FLOAT fSkillTime	= (float)GetTotal() / 1000.0F;	// 초단위
			SetSFX();
			// 지속 효과: dwSfxObj4
			if( pSkillProp->dwSfxObj4 != NULL_ID )
			{
				if( GetTotal() == 0 )
					m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), 0, FALSE );
				else
				{
					//gmpbigsun( 20100908 ) : #10212
					//버프사용도중 같은버프를 사용했을때 sfx의 frame을 초기화 해줘야 하는데 해당 루틴도 없고, 방법도 전무하다.
					//고로 무한으로 설정해서 버프가 끝나지 않는한 sfx가 계속 유지되게 함
					m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), 
						D3DXVECTOR3( 0, 0, 0), pMover->GetId(), -1, FALSE ); //(int)( fSkillTime ) );
				}
			}
		}
	}
	else
	{
		PT_ITEM_SPEC pItemProp	= GetSpecItem();

		if( pItemProp && !HasSFX() )
		{
			FLOAT fSkillTime	= (float)GetTotal() / 1000.0F;	// 초단위
			SetSFX();
			// 지속 효과: dwSfxObj4
			if( pItemProp->dwSfxObj4 != NULL_ID )
			{
				if( GetTotal() == 0 )
					m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3( 0, 0, 0), pMover->GetId(), 0, FALSE );
				else
				{
					//gmpbigsun( 20100908 ) : #10212
					//버프사용도중 같은버프를 사용했을때 sfx의 frame을 초기화 해줘야 하는데 해당 루틴도 없고, 방법도 전무하다.
					//고로 무한으로 설정해서 버프가 끝나지 않는한 sfx가 계속 유지되게 함
					m_pSfx = CreateSfx( g_Neuz.m_pd3dDevice, pItemProp->dwSfxObj4, pMover->GetPos(), pMover->GetId(), 
						D3DXVECTOR3( 0, 0, 0), pMover->GetId(), -1, FALSE ); //(int)( fSkillTime ) );
				}
			}
		}
	}
}

void IBuff::RenderSfx( )
{
	if( m_pSfx )
	{
		m_pSfx->Render( D3DDEVICE );
	}
}
#endif	// __CLIENT

#ifndef __DBSERVER
////////////////////////////////////////////////////////////////////////////////

void IBuffItemBase::Release( CBuffMgr* pBuffMgr )
{
#ifdef __WORLDSERVER
	PT_ITEM_SPEC pProp	= GetSpecItem();
	if( pProp )
	{
		for( size_t Nth = 0; Nth < MAX_ITEM_PARAM; ++Nth )
		{
			if( pProp->dwDestParam[ Nth ] != NULL_ID )
			{
				pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam[ Nth ], pProp->nAdjParamVal[ Nth ], TRUE );
			}
		}
// 		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam1, pProp->nAdjParamVal1, TRUE );
// 		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam2, pProp->nAdjParamVal2, TRUE );
// 		pBuffMgr->GetMover()->ResetDestParam( pProp->dwDestParam3, pProp->nAdjParamVal3, TRUE );
		if( strlen( pProp->szTextFileName ) > 0 && IK3_ANGEL_BUFF != pProp->dwItemKind3 && IK2_PAPERING != pProp->dwItemKind2 )
		{
			if( NULL_ID != pProp->dwActiveSkill )
				pBuffMgr->RemoveBuff( BUFF_SKILL, (WORD)( pProp->dwActiveSkill ) );
			char szCommand[100]	= { 0,};
			CString strCommand	= pProp->szTextFileName;
			strCommand.Replace( '(', '\"' );
			strCommand.Replace( ')', '\"' ); 
			int n	= strCommand.Find( "/", 1 );
			if( n >= 0 )
			{
				FLStrncpy( szCommand, _countof( szCommand ), &pProp->szTextFileName[n], strCommand.GetLength() );
				ParsingCommand( szCommand, pBuffMgr->GetMover(), TRUE );
			}
		}
		if( pProp->dwItemKind2 == IK2_BUFF_TOGIFT )	// 버프 시간 만료되면 아이템 지급하는 시스템(IK2_BUFF_TOGIFT)
		{
			FLWSUser* pUser = static_cast<FLWSUser*>( pBuffMgr->GetMover() );
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pProp->dwReferTarget1 );
			if( IsValidObj( pUser ) && pItemProp )
			{
				FLItemElem itemElem;
				itemElem.m_dwItemId = pItemProp->dwID;
				itemElem.m_bCharged = pItemProp->bCharged;
				itemElem.m_nItemNum = 1;
				itemElem.SetSerialNumber();
				if( pUser->CreateItem( &itemElem ) )
				{
					g_DPSrvr.PutItemLog( pUser, "g", "BuffToGift", &itemElem, 1 );
				}
				else
				{
					g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, 0, itemElem, 0, pItemProp->szName, "" );
					g_DPSrvr.PutItemLog( pUser, "g", "BuffToGift_Post", &itemElem, 1 );
				}
			}
		}

		// TODO : 거래중계 추가등록 가능 개수 아이템 만료 통보 @@@@@@
		if( pProp->dwItemKind3 == IK3_TRADEREGISTERITEM && _GetContentState( CT_CONSIGNMENT_MARKET ) != CS_NOT_SUPPORTED )
		{
			using namespace nsConsignmentSale;

			FLPacketWDB_GetRegistedCountReq kPacket;

			kPacket.m_uPlayerID		= pBuffMgr->GetMover()->m_idPlayer;

			g_dpDBClient.SendPacket( &kPacket );
		}

		if( pProp->dwItemKind3 == IK3_PARTYPOINTFREE && pBuffMgr->GetMover()->m_idparty > 0 )
		{
			g_PartyMng.NotiSetPartyEffectModeToCS( pBuffMgr->GetMover()->m_idPlayer, pBuffMgr->GetMover()->m_idparty, PARTY_EFFECT_POINT_LIMITLESS, false );
		}
	}
#endif	// __WORLDSERVER
}

BOOL IBuffItemBase::IsCommon()
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	return !( pProp && pProp->bCharged );
}

BOOL IBuffItemBase::HasChrState( CMover*, DWORD dwChrState )
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	if( pProp )
	{
		for( int i = 0; i < MAX_ITEM_PARAM; i++ )
		{
			if( pProp->dwDestParam[i] == DST_CHRSTATE && pProp->nAdjParamVal[i] & dwChrState )
				return TRUE;
		}
	}
	return FALSE;
}

BOOL IBuffItemBase::IsDebuf( CMover* /*pMover*/ )
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	if( pProp != NULL && pProp->IsDebuff() == TRUE )
	{
		return TRUE;
	}

	return FALSE;
}

#ifdef __CLIENT
BOOL IBuffItemBase::IsCommercial()
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	return pProp && pProp->bCharged;	// ?	// pProp->dwID != ITEM_INDEX( 21029, II_PET_EGG )
}
#endif	// __CLIENT

////////////////////////////////////////////////////////////////////////////////
BOOL CBuffItem::IsIk1( DWORD dwIk1 )
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	return pProp && pProp->dwItemKind1 == dwIk1;
}

BOOL CBuffItem::IsIk3( DWORD dwIk3 )
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	return pProp && pProp->dwItemKind3 == dwIk3;
}


void CBuffItem::Apply( CMover* pMover )
{
	PT_ITEM_SPEC pProp	= GetSpecItem();
	if( pProp )
	{
		g_xApplyItemEffect->ApplyParameter( pMover, pMover, pProp );
	}
}

BOOL	CBuffItem::CanOverwrite()
{
	PT_ITEM_SPEC pItemSpec	= GetSpecItem();
	if( pItemSpec == NULL )
	{
		return FALSE;
	}

	if( pItemSpec->nMaxDuplication > 1 )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CBuffItem::IsRemovable( CMover* /*pMover*/ )
{
	PT_ITEM_SPEC pItemProp	= GetSpecItem();

	if( pItemProp != NULL )
	{
		if( pItemProp->dwItemKind2 != IK2_BUFF2 && pItemProp->dwSkillTime == 999999999 )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CBuffItem::AddTotal( DWORD tmTotal )
{
	switch( GetId() )
	{
		case ITEM_INDEX( 26531, II_SYS_SYS_SCR_PET_FEED_POCKET02 ):
		case ITEM_INDEX( 21032, II_SYS_SYS_SCR_PET_TONIC_A ):
		case ITEM_INDEX( 21033, II_SYS_SYS_SCR_PET_TONIC_B ):
			tmTotal	+= GetRemain();
			break;
		default:
			break;
	}
	IBuff::AddTotal( tmTotal );
}

DWORD CBuffItem::GetDisguise()
{
//	char szCommand[100]	= { 0,};
	PT_ITEM_SPEC pProp	= GetSpecItem();
	if( pProp && pProp->dwItemKind3 == IK3_TEXT_DISGUISE )
	{
		char* ptr	= ::strstr( pProp->szTextFileName, "/dis" );
		if( ptr )
		{
			int n	= atoi( ptr + 5 );	// "/dis "
			if( n > 0 )
			{
				MoverProp* pMoverProp	= prj.GetMoverProp( n );
				if( pMoverProp )
					return pMoverProp->dwID;
			}
		}
	}
	return NULL_ID;
}

#ifdef __WORLDSERVER
bool	CBuffItem::IsValidBuff( CMover* pMover )
{
	if( pMover != NULL )
	{
		PT_ITEM_SPEC pItemProp = GetSpecItem();
		if( pItemProp != NULL )
		{
			if( GetId() == ITEM_INDEX( 26531, II_SYS_SYS_SCR_PET_FEED_POCKET02 )
				|| GetId() == ITEM_INDEX( 21032, II_SYS_SYS_SCR_PET_TONIC_A )
				|| GetId() == ITEM_INDEX( 21033, II_SYS_SYS_SCR_PET_TONIC_B ) )
			{
				return true;
			}

			if( IsAbsoluteTimeBuff() == true )
			{
				return true;
			}
			else
			{
				if( pItemProp->dwSkillTime >= GetTotal() )
				{
					return true;
				}
			}
		}
	}

	return false;
}

BOOL CBuffSkill::IsValid( CMover* pMover )
{
	BOOL bIsValid = FALSE;

	SkillProp* pSkillProp = GetPropSkill();
	if( pSkillProp == NULL || pMover == NULL )
	{
		return FALSE;
	}

	if( pSkillProp->dwEquipItemKeepSkill == NULL_ID )
	{
		bIsValid = TRUE;
	}
	else
	{
		DWORD dwParts		= NULL_ID;
		DWORD dwWeaponType	= NULL_ID;

		switch( pSkillProp->dwEquipItemKeepSkill )
		{
		case KEEP_EQUIP_YOYO:
			dwParts			= PARTS_RWEAPON;
			dwWeaponType	= WT_MELEE_YOYO;
			break;

		case KEEP_EQUIP_BOW:
			dwParts			= PARTS_RWEAPON;
			dwWeaponType	= WT_RANGE_BOW;
			break;

		case KEEP_EQUIP_SWD:
			dwParts			= PARTS_RWEAPON;
			dwWeaponType	= WT_MELEE_SWD;
			break;

		case KEEP_EQUIP_AXE:
			dwParts			= PARTS_RWEAPON;
			dwWeaponType	= WT_MELEE_AXE;
			break;

		case KEEP_EQUIP_KNUCKLE:
			dwParts			= PARTS_RWEAPON;
			dwWeaponType	= WT_MELEE_KNUCKLE;
			break;

		case KEEP_EQUIP_SHILED:
			{
				PT_ITEM_SPEC pItemSpec = pMover->GetActiveHandItemProp( PARTS_SHIELD );
				if( pItemSpec != NULL && pItemSpec->dwItemKind3 == IK3_SHIELD )
				{
					bIsValid = TRUE;
				}
			}
			break;

		case KEEP_EQUIP_DUALWEAPON:
			if( pMover->IsDualWeapon() == TRUE )
			{
				bIsValid = TRUE;
			}
			break;

		case KEEP_EQUIP_TWOHANDEDWEAPON:
			{
				PT_ITEM_SPEC pItemSpec = pMover->GetActiveHandItemProp();
				if( pItemSpec != NULL && pItemSpec->dwItemKind3 != IK3_HAND && pItemSpec->dwHanded == HD_TWO )
				{
					bIsValid = TRUE;
				}
			}
			break;

		case KEEP_EQUIP_ELEMENTALWEAPON:
			{
				FLItemElem* pRightHandItem	= pMover->GetWeaponItem();
				FLItemElem* pLeftHandItem	= pMover->GetLWeaponItem();
				if( pRightHandItem != NULL )
				{
					if( pRightHandItem->GetItemResist() != SAI79::NO_PROP )
					{
						bIsValid = TRUE;
					}
				}

				if( pLeftHandItem != NULL )
				{
					if( pLeftHandItem->GetItemResist() != SAI79::NO_PROP )
					{
						bIsValid = TRUE;
					}
				}
			}
			break;

		default:
			break;
		}

		if( dwParts != NULL_ID && dwWeaponType != NULL_ID )
		{
			PT_ITEM_SPEC pItemSpec = pMover->GetActiveHandItemProp( dwParts );
			if( pItemSpec != NULL && pItemSpec->dwItemKind3 != IK3_HAND && pItemSpec->dwWeaponType == dwWeaponType )
			{
				bIsValid = TRUE;
			}
		}
	}

	return bIsValid;
}
#endif // __WORLDSERVER

void CBuffSkill::Apply( CMover* pMover )
{
#ifdef __CLIENT
	if( !pMover )	return;
#endif	// __CLIENT
	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	BOOL bResult	= pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.Apply" );
	if( bResult )
		g_cSkillSystem->ApplyParam( pMover, pMover, pSkillProp, pAddSkillProp, FALSE, 0 );
}

void	CBuffSkill::ApplyPain( CMover* pMover )
{
#ifdef	__WORLDSERVER
	SkillProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	if( pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.ApplyPain" ) == FALSE )
	{
		return;
	}

	g_cSkillSystem->ApplyActiveParam( pMover, pMover, pSkillProp, pAddSkillProp, TRUE, 0 );
#endif
}

void CBuffSkill::Release( CBuffMgr* pBuffMgr )
{
	CMover* pMover	= pBuffMgr->GetMover();
	if( pMover )
		g_cSkillSystem->OnEndSkillState( pMover, GetId(), GetLevel() );
}

BOOL CBuffSkill::IsRemovable( CMover* pMover )
{
#ifdef __CLIENT
	if( !pMover )	return TRUE;
#endif	// __CLIENT
	SkillProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsRemovable" );
	return pAddSkillProp && pAddSkillProp->dwSkillTime > 0;
}

BOOL CBuffSkill::IsAddable( CMover* pMover )
{
#ifdef __CLIENT
	SkillProp* pSkillProp	= GetPropSkill();
	return IBuff::IsAddable( pMover )
		&& !( pMover && pMover->IsPlayer() && pMover->m_nDead > 0 && pSkillProp && pSkillProp->IsDebuff() == TRUE );
#else	// __CLIENT
	SkillProp* pSkillProp	= GetPropSkill();
	return IBuff::IsAddable( pMover )
					&& !( pMover->IsPlayer() && pMover->m_nDead > 0 && pSkillProp && pSkillProp->IsDebuff() == TRUE );
#endif	// __CLIENT
}

BOOL CBuffSkill::IsGood( CMover* pMover )
{
	FLASSERT( pMover );
	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsGood" );
	if( pSkillProp != NULL && pSkillProp->IsDebuff() == FALSE )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBuffSkill::HasChrState( CMover* pMover, DWORD dwChrState )
{
	FLASSERT( pMover );
	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.HasChrState" );
	for( int i = 0; i < MAX_SKILL_PARAM; i++ )
		if( pAddSkillProp->dwDestParam[i] == DST_CHRSTATE && pAddSkillProp->dwAdjParamVal[i] & dwChrState )
			return TRUE;
	return FALSE;
}

BOOL CBuffSkill::HasDstParam( CMover* pMover, DWORD dwDstParam )
{
	FLASSERT( pMover );
	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.HasDstParam" );
	for( int i = 0; i < MAX_SKILL_PARAM; i++ )
	{
		if( pAddSkillProp->dwDestParam[i] == dwDstParam )	// 악행스킬인지 왜 체크를 하는가...
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBuffSkill::IsDebuf( CMover* pMover )
{
	FLASSERT( pMover );
	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsDebuf" );
	if( pSkillProp != NULL && pSkillProp->IsDebuff() == TRUE )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBuffSkill::IsAttacker( CMover* pMover, OBJID oiAttacker )
{
	FLASSERT( pMover );
	SkillProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill.IsAttacker" );
	if( pSkillProp && pSkillProp->IsDebuff() == TRUE && GetAttacker() == oiAttacker )
		return TRUE;
	return FALSE;
}

#ifdef __WORLDSERVER
bool	CBuffSkill::IsValidBuff( CMover* /*pMover*/ )
{
// 	if( pMover != NULL )
// 	{
// 		SkillProp* pSkillProp = NULL;
// 		AddSkillProp* pAddSkillProp = NULL;
// 		pMover->GetSkillProp( &pSkillProp, &pAddSkillProp, GetId(), GetLevel(), "CBuffSkill::IsValidBuff" );
// 		if( pSkillProp != NULL && pAddSkillProp != NULL )
// 		{
// 			DWORD dwAddSkillTime1, dwAddSkillTime2;
// 			dwAddSkillTime1 = dwAddSkillTime2 = 0;
// 			pMover->SubReferTime( &dwAddSkillTime1, &dwAddSkillTime2, pSkillProp, pAddSkillProp );
// 			if( (pAddSkillProp->dwSkillTime + dwAddSkillTime1 + dwAddSkillTime2) >= GetTotal() )
// 			{
// 				return true;
// 			}
// 		}
// 	}
// 
// 	return false;

	return true;
}

void CBuffSkill::OnExpire( CMover* pMover )
{
	if( GetId() == SKILL_INDEX( 308, SI_GEN_BURN ) )	// 화상 스킬
	{
		SkillProp* pSkillProp	= prj.GetSkillProp( GetId() );
		int nMin	= pSkillProp->dwAbilityMin;
		int nMax	= pSkillProp->dwAbilityMax;
		int nDamage	= xRandom( nMin, nMax );
		pMover->SendDamage( AF_FORCE, GetAttacker(), nDamage );
		g_xWSUserManager->AddCreateSfxObj( pMover, XI_INDEX( 421, XI_SKILL_ELE_FIRE_BURINGFIELD02 ) );
	}
}
#endif	// __WORLDSERVER

////////////////////////////////////////////////////////////////////////////////
void CBuffPet::Apply( CMover* pMover )
{
	pMover->SetDestParam( HIWORD( GetLevel() ), LOWORD( GetLevel() ), NULL_CHGPARAM, FALSE );
}

void CBuffPet::Release( CBuffMgr* pBuffMgr )
{
#ifdef __WORLDSERVER
	pBuffMgr->GetMover()->ResetDestParam( HIWORD( GetLevel() ), LOWORD( GetLevel() ), TRUE );
#endif	// __WORLDSERVER
}

////////////////////////////////////////////////////////////////////////////////
// BOOL CBuffItem2::Timeover( CMover* pMover, DWORD tmCurrent )
// {
// 	time_t	t	= time_null();
// 	if( (DWORD)( t ) >= GetLevel() )
// 	{
// #ifdef __WORLDSERVER
// 		PT_ITEM_SPEC pProp	= GetSpecItem();
// 		if( pProp && pMover->IsPlayer() )
// 			g_dpDBClient.SendLogSMItemUse( "2", static_cast<FLWSUser*>( pMover ), NULL, pProp );
// #endif	// __WORLDSERVER
// 		return TRUE;
// 	}
// 	return FALSE;
// }
// 
// void CBuffItem2::SerializeLevel( CAr & ar )
// {
// 	if( ar.IsStoring() )
// 	{
// 		time_t t	= static_cast<time_t>( GetLevel() ) - time_null();
// 		ar << t;
// 	}
// 	else
// 	{
// 		time_t t;
// 		ar >> t;
// 		SetLevel( time_null() + t );
// 	}
// }
#endif	// __DBSERVER

CBuffMgr::CBuffMgr()
{
	m_pMover = NULL;
}

CBuffMgr::~CBuffMgr()
{
	Clear();
}

void CBuffMgr::Clear()
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		SAFE_DELETE( *it );
	}
	m_listBuffs.clear();
}

IBuff* CBuffMgr::CreateBuff( WORD wType )
{
	IBuff* ptr	= NULL;
#ifdef __DBSERVER
	ptr	= new IBuff;
#else	// __DBSERVER
	switch( wType )
	{
		case BUFF_SKILL:	ptr		= new CBuffSkill;	break;
		case BUFF_ITEM:		ptr		= new CBuffItem;	break;
		case BUFF_PET:		ptr	= new CBuffPet;	break;
		case BUFF_EQUIP:	ptr	= new CBuffEquip;	break;
		default:	return NULL;
	}
#endif	// __DBSERVER
	ptr->SetType( wType );
	return ptr;
}

#ifndef __DBSERVER
BOOL CBuffMgr::Overwrite( IBuff* pNewBuff )
{
	if( pNewBuff == NULL )
	{
		return FALSE;
	}

	IBuff* pRemainBuff	= GetBuff( pNewBuff->GetType(), pNewBuff->GetId() );
	if( pRemainBuff == NULL )
	{
		return FALSE;
	}

	if( pRemainBuff->CanOverwrite() == FALSE )
	{
		return FALSE;
	}

	if( pRemainBuff->GetLevel() == pNewBuff->GetLevel() )
	{
		pRemainBuff->AddTotal( pNewBuff->GetTotal() );
		
		return TRUE;
	}
	else if( pRemainBuff->GetLevel() < pNewBuff->GetLevel() )
	{
		pNewBuff->SetAdjValue( pRemainBuff->GetAdjValue() );
		RemoveBuff( pRemainBuff, FALSE );
		
		return FALSE;
	}
	else	// if( pOld->GetLevel() > pBuff->GetLevel() )	// ignore
	{
		return TRUE;
	}


// 	IBuff* pOld	= GetBuff( pBuff->GetType(), pBuff->GetId() );
// 	if( pOld )
// 	{
// 		//////////////////////////////////////////////////////////////////////////
// 		// mirchang_101101
// 		if( pOld->GetType() == BUFF_ITEM )
// 		{
// 			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pOld->GetId() );
// 			if( pItemProp != NULL )
// 			{
// 				if( pItemProp->nMaxDuplication > 1 )
// 				{
// 					return FALSE;
// 				}
// 			}
// 		}
// 		//////////////////////////////////////////////////////////////////////////
// 
// 		if( pOld->GetLevel() == pBuff->GetLevel() )
// 		{
// 			pOld->AddTotal( pBuff->GetTotal() );
// 			return TRUE;
// 		}
// 		else if( pOld->GetLevel() < pBuff->GetLevel() )
// 		{
// 			pBuff->SetAdjValue( pOld->GetAdjValue() );
// 			RemoveBuff( pOld, FALSE );
// 		}
// 		else	// if( pOld->GetLevel() > pBuff->GetLevel() )	// ignore
// 			return TRUE;
// 	}
// 	return FALSE;
}

BOOL CBuffMgr::HasBuff( WORD wType, WORD wId )
{
	//return GetBuff( wType, wId ) != NULL;

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100806 삭제 예정 버프 검사 추가
	IBuff* pBuff = GetBuff( wType, wId );

	if( pBuff != NULL )
	{
		return TRUE;
	}

	return FALSE;
}

IBuff* CBuffMgr::GetBuff( WORD wType, WORD wId )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetId() == wId && pBuff->GetType() == wType && pBuff->GetRemove() == FALSE )
		{
			return pBuff;
		}
	}

	return NULL;
}

IBuff* CBuffMgr::GetExpiredBuff( WORD wType, WORD wId )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == wType && pBuff->GetId() == wId && pBuff->IsExpired() == TRUE )
		{
			return pBuff;
		}
	}

	return NULL;
}

IBuff* CBuffMgr::GetBuffByIk3( DWORD dwIk3 )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->IsIk3( dwIk3 ) == TRUE && pBuff->GetRemove() == FALSE )
		{
			return pBuff;
		}
	}

	return NULL;
}

BOOL CBuffMgr::HasBuffByIk3( DWORD dwIk3 )
{
	//return GetBuffByIk3( dwIk3 ) != NULL;

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100806 삭제 예정 버프 검사 추가
	IBuff* pBuff = GetBuffByIk3( dwIk3 );

	if( pBuff != NULL )
	{
		return TRUE;
	}

	return FALSE;
}

IBuff*	CBuffMgr::GetBuffByDestParam( DWORD dwDestParam )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL )
		{
			if( pBuff->HasDstParam( GetMover(), dwDestParam ) == TRUE )
			{
				return pBuff;
			}
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// mirchang_101101
bool	CBuffMgr::HasUseLimitGroupItemBuff( int nUseLimitGroup )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pItemProp != NULL && pItemProp->nItemGroup == nUseLimitGroup )
			{
				return true;
			}
		}
	}

	return false;
}

int		CBuffMgr::GetCountBuffByItemGroup( int nItemGroup )
{
	int nCount = 0;

	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pItemProp != NULL && pItemProp->nItemGroup == nItemGroup )
			{
				++nCount;
			}
		}
	}

	return nCount;
}

int		CBuffMgr::GetCountBuffByItemId( DWORD dwId )
{
	int nCount = 0;

	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pItemProp != NULL && pItemProp->dwID == dwId )
			{
				++nCount;
			}
		}
	}

	return nCount;
}

int		CBuffMgr::GetAdjValueDuplicationBuff( DWORD dwItemKind3 )
{
	int nValue = 0;

	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pItemProp != NULL && pItemProp->dwItemKind3 == dwItemKind3 )
			{
				nValue += pItemProp->nEffectValue;
			}
		}
	}

	return nValue;
}

int		CBuffMgr::GetRemainCountBuff( const DWORD dwItemKind3 )
{
	IBuff* pBuff	= GetBuffByIk3( dwItemKind3 );
	if( pBuff == NULL )
	{
		return 0;
	}

	const T_ITEM_SPEC* pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
	if( pItemProp == NULL )
	{
		return 0;
	}

	const int nUseCount = pBuff->GetAdjValue();
	if( nUseCount >= pItemProp->nEffectValue )
	{
		return 0;
	}

	return pItemProp->nEffectValue - nUseCount;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void	CBuffMgr::DoApplyHitActiveSkill( CMover* pDest, const DWORD dwProb )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
			
			if( pItemProp != NULL && pItemProp->dwItemKind3 == IK3_USESKILL
				&& pItemProp->dwHitActiveSkillId != NULL_ID && pItemProp->dwHitActiveSkillLv != NULL_ID && pItemProp->dwHitActiveSkillProb > dwProb )
			{
				if( pItemProp->dwHitActiveSkillTarget == IST_SELF )
				{
					g_cSkillSystem->DoActiveSkill( GetMover(), pItemProp->dwHitActiveSkillId, pItemProp->dwHitActiveSkillLv, GetMover(), true );
				}
				else if( pItemProp->dwHitActiveSkillTarget == IST_ANOTHER )
				{
					g_cSkillSystem->DoActiveSkill( GetMover(), pItemProp->dwHitActiveSkillId, pItemProp->dwHitActiveSkillLv, pDest, true );
				}
			}
		}
	}
}

void	CBuffMgr::DoApplyDamageActiveSkill( CMover* pDest, const DWORD dwProb )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );

			if( pItemProp != NULL && pItemProp->dwItemKind3 == IK3_USESKILL
				&& pItemProp->dwDamageActiveSkillId != NULL_ID && pItemProp->dwDamageActiveSkillLv != NULL_ID && pItemProp->dwDamageActiveSkillProb > dwProb )
			{
				if( pItemProp->dwDamageActiveSkillTarget == IST_SELF )
				{
					g_cSkillSystem->DoActiveSkill( GetMover(), pItemProp->dwDamageActiveSkillId, pItemProp->dwDamageActiveSkillLv, GetMover(), true );
				}
				else if( pItemProp->dwDamageActiveSkillTarget == IST_ANOTHER )
				{
					g_cSkillSystem->DoActiveSkill( GetMover(), pItemProp->dwDamageActiveSkillId, pItemProp->dwDamageActiveSkillLv, pDest, true );
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////


BOOL CBuffMgr::AddBuff( WORD wType, WORD wId, DWORD dwLevel, DWORD tmTotal, DWORD dwPain, BYTE byBuffTickType, OBJID oiAttacker )
{
	IBuff* pBuff	= CreateBuff( wType );

	DWORD dwNowTick = ::timeGetTime();
	pBuff->SetId( wId );
	pBuff->SetLevel( dwLevel );
	pBuff->SetTotal( tmTotal );
	pBuff->SetInst( dwNowTick );		// +
	pBuff->SetPain( dwPain, byBuffTickType, dwNowTick );
	pBuff->SetAttacker( oiAttacker );

	BOOL bNew	= AddBuff( pBuff );
	if( bNew == FALSE )
	{
		SAFE_DELETE( pBuff );
	}
	return bNew;
}

BOOL CBuffMgr::AddBuff( IBuff* pBuff )
{
	if( pBuff->IsAddable( GetMover() ) == FALSE )
	{
		return FALSE;
	}

	if( Overwrite( pBuff ) == TRUE )
	{
#ifdef __WORLDSERVER
		g_xWSUserManager->AddSetSkillState( GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
#endif	// __WORLDSERVER
		return FALSE;
	}
	PrepareBS( pBuff );
	bool bResult	= Add( pBuff );
#ifdef __WORLDSERVER
	if( bResult == true )
	{
		g_xWSUserManager->AddSetSkillState( GetMover(), GetMover(), pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal() );
	}
#endif	// __WORLDSERVER
	return static_cast<BOOL>( bResult );
}

void CBuffMgr::RemoveBuff( IBuff* pBuff, BOOL bFake )
{
	if( pBuff == NULL )
	{
		return;
	}

	if( pBuff->GetRemove() == FALSE )	// pBuff에 대하여 RemoveBuff의 최초 호출 시 Release 호출
	{
		pBuff->Release( this );

#ifdef __WORLDSERVER
		g_xWSUserManager->AddRemoveSkillInfluence( GetMover(), pBuff->GetType(), pBuff->GetId() );
#endif // __WORLDSERVER
	}

#ifdef __WORLDSERVER
	if( bFake == TRUE )
	{
		pBuff->SetRemove();
		return;
	}

//	g_xWSUserManager->AddRemoveSkillInfluence( GetMover(), pBuff->GetType(), pBuff->GetId() );

	if( pBuff->GetType() == BUFF_ITEM )
	{
		PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( pBuff->GetId() );
		if( pItemProp != NULL && pItemProp->bCharged == TRUE && GetMover()->IsPlayer() == TRUE )
		{
			g_dpDBClient.SendLogSMItemUse( "2", static_cast<FLWSUser*>( GetMover() ), NULL, pItemProp );
		}
	}
#endif	// __WORLDSERVER

	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pDelBuff = *it;
		if( pDelBuff != NULL && pDelBuff == pBuff )
		{
			m_listBuffs.erase( it );
			SAFE_DELETE( pBuff );
			return;
		}
	}
}

void CBuffMgr::RemoveBuff( WORD wType, WORD wId, BOOL bFake )
{
	IBuff* pBuff	= GetBuff( wType, wId );
	if( pBuff )
	{
		RemoveBuff( pBuff, bFake );
	}
}

void CBuffMgr::RemoveExpiredBuff( WORD wType, WORD wId, BOOL bFake )
{
	IBuff* pBuff	= GetExpiredBuff( wType, wId );
	if( pBuff != NULL )
	{
		RemoveBuff( pBuff, bFake );
	}
}

void CBuffMgr::PrepareBS( IBuff* pBuff )
{
	if( pBuff->GetType() == BUFF_SKILL && IsBSFull() )
	{
		IBuff* pFirst	= GetFirstBS();
		if( pFirst )
			RemoveBuff( pFirst, FALSE );
	}
}

int CBuffMgr::IsBSFull()
{
	int nNum	= 0;
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_SKILL )
		{
			nNum++;
		}
	}
	return nNum >= MAX_SKILLBUFF_COUNT;
}

IBuff* CBuffMgr::GetFirstBS()
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetType() == BUFF_SKILL )
		{
			SkillProp* pSkillProp = pBuff->GetPropSkill();
			if( pSkillProp && pSkillProp->IsDebuff() == FALSE )
			{
				return pBuff;
			}
		}
	}

	return NULL;
}

void CBuffMgr::Process()
{
	DWORD dwNowTick = ::timeGetTime();
	std::vector<LONG> vTemp;
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff == NULL )
		{
			continue;
		}
		BOOL bExpired	= pBuff->Process( GetMover(), dwNowTick );
		if( bExpired == TRUE )
		{
			pBuff->SetExpired();
			/************************************************************************/
			/*
			2012.09.04
			모든 버프의 삭제는 서버의 통보를 받고 삭제하도록 변경.
			*/
			/************************************************************************/
#ifdef __WORLDSERVER
			vTemp.push_back( MAKELONG( pBuff->GetId(), pBuff->GetType() ) );
#endif // __WORLDSERVER
		}
	}
	while( vTemp.empty() == false )
	{
		LONG l	= vTemp.back();
		RemoveExpiredBuff( HIWORD( l ), LOWORD( l ), FALSE );
		vTemp.pop_back();
	}
}

void CBuffMgr::RemoveBuffs( DWORD dwFlags, DWORD dwParam )
{
	std::vector<LONG> vTemp;
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff == NULL )
		{
			continue;
		}
		if(
			( ( dwFlags & RBF_UNCONDITIONAL )
//sun: 13, 하우징 시스템
				&& !pBuff->IsIk1( IK1_HOUSING )	// 하우징 버프는 "버프해제" 명령으로 삭제 안됨.
			)
			|| ( ( dwFlags & RBF_COMMON ) && GetMover() && pBuff->IsCommon() )
			|| ( ( dwFlags & RBF_IK3 ) && pBuff->IsIk3( dwParam ) )
			|| ( ( dwFlags & RBF_GOODSKILL ) && pBuff->IsGood( GetMover() ) )
			|| ( ( dwFlags & RBF_CHRSTATE ) && pBuff->HasChrState( GetMover(), dwParam ) )
			|| ( ( dwFlags & RBF_DSTPARAM ) && pBuff->HasDstParam( GetMover(), dwParam ) )
			|| ( ( dwFlags & RBF_DEBUF ) && pBuff->IsDebuf( GetMover() ) )
			|| ( ( dwFlags & RBF_ATTACKER ) && pBuff->IsAttacker( GetMover(), dwParam ) )
		)
		{
			vTemp.push_back( MAKELONG( pBuff->GetId(), pBuff->GetType() ) );
			if( dwFlags & RBF_ONCE )
			{
				break;
			}
		}
	}
	while( !vTemp.empty() )
	{
		LONG l	= vTemp.back();
		RemoveBuff( HIWORD( l ), LOWORD( l ) );
		vTemp.pop_back();
	}
}

DWORD CBuffMgr::GetDisguise()
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff = *it;
		if( pBuff == NULL )
		{
			continue;
		}
		DWORD dwIndex	= pBuff->GetDisguise();
		if( dwIndex != NULL_ID )
		{
			return dwIndex;
		}
	}
	return NULL_ID;
}
#endif	// __DBSERVER

#ifdef __WORLDSERVER
IBuff* CBuffMgr::GetBuffPet()
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff->GetType() == BUFF_PET )
		{
			return pBuff;
		}
	}
	return NULL;
}

void CBuffMgr::RemoveBuffPet( IBuff* pBuff )
{
	FLASSERT( pBuff );
	RemoveBuff( pBuff, FALSE );
	FLItemElem* pItem	= GetMover()->GetPetItem();
	if( pItem )
		GetMover()->ResetDestParamRandomOptExtension( pItem );
}

void CBuffMgr::RemoveBuffPet()
{
	IBuff* pBuff	= GetBuffPet();
	if( pBuff )
		RemoveBuffPet( pBuff );
}
#endif	// __WORLDSERVER

void CBuffMgr::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		// ar << m_mapBuffs.size();
		// chipi_090217
		ar << m_listBuffs.size() - GetRemoveBuffSize();
		for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
		{
			IBuff* pBuff	= *it;
			if( pBuff != NULL && pBuff->GetRemove() == FALSE )	// chipi_090217
			{
				pBuff->Serialize( ar, GetMover() );
			}
		}
	}
	else
	{
		Clear();
		size_t t;
		ar >> t;
		for( size_t i = 0; i < t; i++ )
		{
			WORD wType;
			ar >> wType;
			IBuff* pBuff	
				= CreateBuff( wType );

			if( pBuff != NULL )
			{
				pBuff->Serialize( ar, GetMover() );

				//////////////////////////////////////////////////////////////////////////
				// DB에 저장된 버프 남은 시간이 속성에 들어가 있는 시간보다 넘을 경우 날려버린다.
#ifdef __WORLDSERVER
				if( pBuff->IsValidBuff( GetMover() ) == true )
				{
					Add( pBuff );
				}
				else
				{
					CMover* pMover = GetMover();
					if( pMover != NULL )
					{
						FLINFO_LOG( PROGRAM_NAME, _T( "Invalid Buff. UserID:[%07d], BuffType:[%d], BuffID:[%d], RemainTime:[%d]" ),
						pMover->m_idPlayer, pBuff->GetType(), pBuff->GetId(), pBuff->GetTotal() );
					}
					SAFE_DELETE( pBuff );
				}
#else // __WORLDSERVER
				Add( pBuff );
#endif // __WORLDSERVER
				//////////////////////////////////////////////////////////////////////////
			}
			else
			{
				IBuff tmpBuff;
				tmpBuff.Serialize( ar, GetMover() );
				FLERROR_LOG( PROGRAM_NAME, _T( "Buff Type Invalid. Type : [%d]" ), wType );
			}
		}
#ifndef __DBSERVER
		Process();
#endif // __DBSERVER
	}
}

CBuffMgr& CBuffMgr::operator =( CBuffMgr & bm )
{
	Clear();
	for( BuffItr it = bm.m_listBuffs.begin(); it != bm.m_listBuffs.end(); ++it )
	{
		IBuff* pSource	= *it;
		if( pSource == NULL )
		{
			continue;
		}
		IBuff* pDest	= CreateBuff( pSource->GetType() );
		*pDest	= *pSource;
		bool bResult	= Add( pDest );
		FLASSERT( bResult );
	}
	return *this;
}


#ifdef __CLIENT
int CBuffMgr::GetCommercialCount()
{
	int nCount	= 0;
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff = *it;
		if( pBuff != NULL && pBuff->IsCommercial() )
		{
			nCount++;
		}
	}
	return nCount;
}

void CBuffMgr::ClearInst()
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff = *it;
		if( pBuff != NULL )
		{
			pBuff->SetInst( 0 );
		}
	}
}

void CBuffMgr::Render( )
{
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff = *it;
		if( pBuff != NULL )
		{
			pBuff->RenderSfx();
		}
	}
}
#endif	// __CLIENT

#ifdef __DBSERVER
void CBuffMgr::ToString( char* szString, size_t cchString )
{
	char szBuff[256]	= { 0,};
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff == NULL )
		{
			continue;
		}
		if( pBuff->GetType() == BUFF_EQUIP )
			continue;
//sun: 13, 하우징 시스템
		if( pBuff->GetType() == BUFF_ITEM )
		{
			PT_ITEM_SPEC pProp	= g_xSpecManager->GetSpecItem( pBuff->GetId() );
			if( pProp && pProp->dwItemKind1 == IK1_HOUSING )
				continue;
		}

		FLSPrintf( szBuff, _countof( szBuff ), "%d,%d,%d,%d,%d/", 
			pBuff->GetType(), pBuff->GetId(), pBuff->GetLevel(), pBuff->GetTotal(), pBuff->GetAdjValue() );
		FLStrcat( szString, cchString, szBuff );
	}
	FLStrcat( szString, cchString, NullStr );
}

#endif	// DBSERVER

// chipi_090217
size_t CBuffMgr::GetRemoveBuffSize()
{
	size_t nCount = 0;
	for( BuffItr it = m_listBuffs.begin(); it != m_listBuffs.end(); ++it )
	{
		IBuff* pBuff	= *it;
		if( pBuff != NULL && pBuff->GetRemove() == TRUE )
		{
			nCount++;
		}
	}

	return nCount;
}
