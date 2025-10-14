
#include "StdAfx.h"
#include "FLItemElem.h"
#include "definetext.h"

#ifndef __DBSERVER
#include "CreateObj.h"
#endif // __DBSERVER


FLItemElem::FLItemElem()
{
	constructor();
}

FLItemElem::FLItemElem( const FLItemElem & r )
{
	constructor();
	*this					= r;
}


FLItemElem::~FLItemElem()
{
	SAFE_DELETE( m_pPet );		//sun: 9, 9-10차 펫
}

void	FLItemElem::constructor()
{
	m_idGuild = 0;
	m_nItemNum = 1;
	m_nAbilityOption = 0;
	m_nRepairNumber	= 0;
	m_nHitPoint		= 0;
	m_nRepair	= 0;
	m_byFlag	= 0;
#ifdef __CLIENT
	m_bRepair	= FALSE;
#endif	// __CLIENT

	m_byItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;	
	m_nResistSMItemId = 0;

#ifdef __WORLDSERVER
	m_bQuery	= FALSE;
#endif	// __WORLDSERVER

	m_bCharged	= FALSE;
	m_dwKeepTime	= 0;

	m_pPet	= NULL;	//sun: 9, 9-10차 펫

	m_bTranformVisPet = FALSE;

	m_dwCouplePlayerId					= 0;
	m_nLevelDown						= 0;
	m_wRandomOptionOriginID				= 0;
	m_dwLooksChangeItemID				= 0;
	m_bCombined							= false;
	m_dwCombinedAddDamage				= 0;
	m_wCombinedRandomOptionOriginID		= 0;
	m_kPiercingOption.Clear();
	m_kRandomOption.Clear();
}

void	FLItemElem::Serialize( CAr & ar ) const
{
	FLItemBase::Serialize( ar );
	
	ar << m_nItemNum;
	ar << m_nRepairNumber << m_nHitPoint;
	ar << m_nRepair;
	ar << m_byFlag;
	ar << m_nAbilityOption;
	ar << m_idGuild;				// 길드번호도 저장.
	ar << m_byItemResist;			// 어느 속성 인가?
	ar << m_nResistAbilityOption ;	// 속성 추가 능력치 가변 옵션
	ar << m_nResistSMItemId;
	ar << m_bCharged;

	ar << m_dwKeepTime;
	if( m_dwKeepTime )
	{
		time_t t	= m_dwKeepTime - time_null();
		ar << t;
	}
	//sun: 9, 9-10차 펫
	if( m_pPet )
	{
		ar << (BYTE)0x01;
		m_pPet->Serialize( ar );
	}
	else
	{
		ar << (BYTE)0x00;
	}

	ar << m_bTranformVisPet;

	ar << m_dwCouplePlayerId;
	ar << m_nLevelDown;
	ar << m_wRandomOptionOriginID;
	ar << m_dwLooksChangeItemID;
	ar << m_bCombined;
	ar << m_dwCombinedAddDamage;
	ar << m_wCombinedRandomOptionOriginID;
	
	m_kPiercingOption.SerializeAllPiercingOption( ar );
	m_kRandomOption.SerializeAllRandomOptionExtension( ar );
}

void	FLItemElem::Deserialize( CAr & ar )
{
	FLItemBase::Deserialize( ar );
	
	ar >> m_nItemNum;
	ar >> m_nRepairNumber ;
	ar >> m_nHitPoint;
	ar >> m_nRepair;
	ar >> m_byFlag;
	ar >> m_nAbilityOption;
	ar >> m_idGuild;		
	ar >> m_byItemResist;	
	ar >> m_nResistAbilityOption ;	
	ar >> m_nResistSMItemId;
	ar >> m_bCharged;

	ar >> m_dwKeepTime;

	if( m_dwKeepTime ) 
	{
		time_t t;
		ar >> t;
#ifdef __CLIENT
		m_dwKeepTime	= time_null() + t;
#endif	// __CLIENT
	}
	//sun: 9, 9-10차 펫
	SAFE_DELETE( m_pPet );
	BYTE bPet;
	ar >> bPet;
	if( bPet )
	{
		m_pPet	= new CPet;
		m_pPet->Serialize( ar );
	}
	ar >> m_bTranformVisPet;

	ar >> m_dwCouplePlayerId;
	ar >> m_nLevelDown;
	ar >> m_wRandomOptionOriginID;
	ar >> m_dwLooksChangeItemID;
	ar >> m_bCombined;
	ar >> m_dwCombinedAddDamage;
	ar >> m_wCombinedRandomOptionOriginID;
	
	m_kPiercingOption.DeserializeAllPiercingOption( ar );
	m_kRandomOption.DeserializeAllRandomOptionExtension( ar );

#ifdef __CLIENT
	SetTexture();
#endif
}

void	FLItemElem::Empty( void )
{
	FLItemBase::Empty();

	SAFE_DELETE( m_pPet );		//sun: 9,	// __PET_0410

	constructor();
}

// 돈의 양을 얻는다. 
int	FLItemElem::GetGold()
{
	FLASSERT( GetProp()->dwItemKind1 == IK1_GOLD );
	return m_nHitPoint;		// int형으로 돈을 버리게 하기 위해서 이 변수를 사용 
}

void FLItemElem::UseItem()
{
	if( GetProp()->bPermanence != TRUE )
		m_nItemNum--;
}

FLItemElem& FLItemElem::operator =( const FLItemElem & ie )
{
	FLItemBase::operator =( ie );

	//SetTexture( ie.GetTexture() );
#ifdef __CLIENT
	m_pTexture				= ie.m_pTexture;
#endif// __CLIENT

	m_nItemNum             	= ie.m_nItemNum;
	m_nRepairNumber        	= ie.m_nRepairNumber;
	m_nHitPoint            	= ie.m_nHitPoint;
	m_nRepair				= ie.m_nRepair;
	m_byFlag               	= ie.m_byFlag;
	m_nAbilityOption       	= ie.m_nAbilityOption;
	m_idGuild              	= ie.m_idGuild;
	m_byItemResist          = ie.m_byItemResist;
	m_nResistAbilityOption 	= ie.m_nResistAbilityOption;
	m_nResistSMItemId      	= ie.m_nResistSMItemId;
	m_dwKeepTime			= ie.m_dwKeepTime;
	m_bCharged				= ie.m_bCharged;

//sun: 9, 9-10차 펫
	SAFE_DELETE( m_pPet );
	if( ie.m_pPet )
	{
		m_pPet	= new CPet;
		*m_pPet	= *ie.m_pPet;
	}

	m_bTranformVisPet					= ie.m_bTranformVisPet;

	m_dwCouplePlayerId					= ie.m_dwCouplePlayerId;
	m_nLevelDown						= ie.m_nLevelDown;
	m_wRandomOptionOriginID 			= ie.m_wRandomOptionOriginID;
	m_dwLooksChangeItemID				= ie.m_dwLooksChangeItemID;
	m_bCombined							= ie.m_bCombined;
	m_dwCombinedAddDamage				= ie.m_dwCombinedAddDamage;
	m_wCombinedRandomOptionOriginID		= ie.m_wCombinedRandomOptionOriginID;
	
	m_kPiercingOption					= ie.m_kPiercingOption;
	m_kRandomOption						= ie.m_kRandomOption;

	return *this;
}

// 유료아이템인가?
BOOL FLItemElem::IsCharged()
{
	if( m_bCharged == 1 || m_nResistSMItemId != 0 )
		return TRUE;

	return FALSE;
}

#if defined( __WORLDSERVER ) || defined (__CLIENT )
BOOL	FLItemElem::IsAccessory( void )
{	
	return CAccessoryProperty::GetInstance()->IsAccessory( m_dwItemId );
}

bool	FLItemElem::CanUseLevel( CMover* pMover )
{
	if( pMover == NULL )
	{
		return false;
	}

	PT_ITEM_SPEC pItemSpec	= GetProp();
	if( pItemSpec == NULL )
	{
		return false;
	}

	if( pItemSpec->dwParts != NULL_ID )
	{
		if( pMover->GetJobType() >= JTYPE_MASTER && pMover->GetJobType() > pMover->GetJobType( pItemSpec->dwItemJob ) )
		{
			return true;
		}

		if( GetMinLimitLevel() <= pMover->GetLevel() && pMover->GetLevel() <= pItemSpec->nMaxLimitLevel )
		{
			return true;
		}
	}
	else
	{
		if( pItemSpec->nMinLimitLevel <= pMover->GetLevel() && pMover->GetLevel() <= pItemSpec->nMaxLimitLevel )
		{
			return true;
		}
	}

	return false;
}

void FLItemElem::GetPiercingAvail( PPIERCINGAVAIL pPiercingAvail )
{
	for( size_t i = 0; i < GetGeneralPiercingSize(); i++ )
	{
		PPIERCINGAVAIL ptr	= CPiercingAvail::GetInstance()->GetPiercingAvail( GetGeneralPiercingItemID( i ) );
		if( ptr )
		{
			for( int j = 0; j < ptr->nSize; j++ )
			{
				int nFind	= -1;
				for( int k = 0; k < pPiercingAvail->nSize; k++ )
				{
					if(  pPiercingAvail->anDstParam[k] == ptr->anDstParam[j] )
					{
						nFind	= k;
						break;
					}
				}
				if( nFind < 0 )
					nFind	= pPiercingAvail->nSize++;
				pPiercingAvail->anDstParam[nFind]		= ptr->anDstParam[j];
				pPiercingAvail->anAdjParam[nFind]		+= ptr->anAdjParam[j];
			}
		}
	}
}

void FLItemElem::GetCombinedPiercingAvail( PPIERCINGAVAIL pPiercingAvail )
{
	for( size_t i = 0; i < GetCombinedGeneralPiercingSize(); i++ )
	{
		PPIERCINGAVAIL ptr	= CPiercingAvail::GetInstance()->GetPiercingAvail( GetCombinedGeneralPiercingItemID( i ) );
		if( ptr )
		{
			for( int j = 0; j < ptr->nSize; j++ )
			{
				int nFind	= -1;
				for( int k = 0; k < pPiercingAvail->nSize; k++ )
				{
					if(  pPiercingAvail->anDstParam[k] == ptr->anDstParam[j] )
					{
						nFind	= k;
						break;
					}
				}
				if( nFind < 0 )
					nFind	= pPiercingAvail->nSize++;
				pPiercingAvail->anDstParam[nFind]		= ptr->anDstParam[j];
				pPiercingAvail->anAdjParam[nFind]		+= ptr->anAdjParam[j];
			}
		}
	}
}
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
int FLItemElem::GetGemAvail( int nAblity )
{
	int nCount = -1;
	for( size_t i = 0; i < GetUltimatePiercingSize(); i++ )
	{
		if( GetUltimatePiercingItemID( i ) == nAblity )
			nCount++;
	}

	return CSetGem::GetInstance()->GetGemAvail( nAblity, nCount );
}
#endif

DWORD	FLItemElem::GetMinAddDamage()
{
	DWORD dwMinAddDamage = 0;
	
	const int nAbilityOption = GetAbilityOption();
	if( nAbilityOption > 0 )
	{
		float fFactor = 1.0f;
		const int nValue = prj.GetExpUpItem( this, nAbilityOption ) + 100;
		
		fFactor = fFactor * ( static_cast<float>( nValue ) / 100.0f );

		const DWORD dwMinDamage		= static_cast<DWORD>( static_cast<float>( GetProp()->dwAbilityMin * 2 ) * fFactor );

		dwMinAddDamage	=  dwMinDamage > ( GetProp()->dwAbilityMin * 2 ) ? dwMinDamage - ( GetProp()->dwAbilityMin * 2 ) : 0;
	}

	return dwMinAddDamage;
}

#endif // defined( __WORLDSERVER ) || defined (__CLIENT )

BOOL	FLItemElem::IsExpiring( void )
{	
	if( m_dwKeepTime )
	{
		if( time_null() > (time_t)m_dwKeepTime )
			return TRUE;
	}
	return FALSE;
}

BOOL	FLItemElem::IsVisPet()
{
	PT_ITEM_SPEC pItemProp = GetProp();
	if( pItemProp != NULL )
	{
		if( pItemProp->dwItemKind3 == IK3_PET && pItemProp->dwReferStat1 == PET_VIS )
		{
			return TRUE;
		}
		else
		{
			if( IsTransformVisPet() == TRUE )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL	FLItemElem::IsTransformVisPet()
{
	PT_ITEM_SPEC pItemProp = GetProp();
	if( pItemProp != NULL )
	{
		if( pItemProp->dwItemKind3 == IK3_PET && m_bTranformVisPet == TRUE )
		{
			return TRUE;
		}
	}
	return FALSE;
}

// 주사위로 제련 가능한 아이템 종류
BOOL FLItemElem::IsDiceRefineryAble( PT_ITEM_SPEC pProp )
{
	if( !pProp )
		return FALSE;

//#ifdef __ENCHANT_BARUNA16
//	if( pProp->IsBaruna() )
//		return FALSE;
//#endif //__ENCHANT_BARUNA16

	if( pProp->dwItemKind2 == IK2_ARMOR ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT ||
		pProp->dwItemKind2 == IK2_ARMORETC )
		return TRUE;
					
	return FALSE;
}

// 카드로 제련 가능한 아이템 종류
BOOL FLItemElem::IsEleRefineryAble( PT_ITEM_SPEC pProp )
{
	if( !pProp )
		return FALSE;

//#ifdef __ENCHANT_BARUNA16
// 	if( pProp->IsBaruna() )
// 		return FALSE;
//#endif //__ENCHANT_BARUNA16

	if( pProp->dwItemKind3 == IK3_SUIT ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT )
		return TRUE;
	
	return FALSE;
}

BOOL FLItemElem::IsElementalCard( const DWORD dwItemID )
{
	switch( dwItemID )
	{
		case ITEM_INDEX( 3206, II_GEN_MAT_ELE_FLAME ):
		case ITEM_INDEX( 3211, II_GEN_MAT_ELE_RIVER ):
		case ITEM_INDEX( 3216, II_GEN_MAT_ELE_GENERATOR ):
		case ITEM_INDEX( 3221, II_GEN_MAT_ELE_DESERT ):
		case ITEM_INDEX( 3226, II_GEN_MAT_ELE_CYCLON ):
			return TRUE;
		default:
			return FALSE;
	}
}

BOOL FLItemElem::IsOwnState( void )
{
	PT_ITEM_SPEC pItemProp = GetProp();
	FLASSERT( pItemProp );

	if( m_dwKeepTime && pItemProp->dwItemKind2 != IK2_WARP )
	{
		return TRUE;
	}

// 	if( (pItemProp->dwFlag & IP_FLAG_BINDS) == IP_FLAG_BINDS )
// 	{
// 		return TRUE;
// 	}

	if( IsBindState() == true )
	{
		return TRUE;
	}

// 	if( IsFlag( FLItemElem::binds ) == TRUE )
// 	{
// 		return TRUE;
// 	}
// 
// 	if( pItemProp->nBindCondition == BIND_CONDITION_GET && IsFlag( FLItemElem::reset_bind ) == FALSE )
// 	{
// 		return TRUE;
// 	}

	if( IsSetRandomOptionExtension() == true && ( pItemProp->IsCostumeParts() == TRUE || pItemProp->IsEatPet() == TRUE ) )
	{
		return TRUE;
	}

	if( GetLevelDown() > 0 )
	{
		return TRUE;
	}

	return FALSE;
}

bool	FLItemElem::IsBindState()
{
	PT_ITEM_SPEC pItemProp = GetProp();
	FLASSERT( pItemProp );

	if( IsFlag( binds ) == TRUE )
	{
		return true;
	}

	if( pItemProp->nBindCondition == BIND_CONDITION_GET && IsFlag( reset_bind ) == FALSE )
	{
		return true;
	}

	return false;
}

bool	FLItemElem::IsUseBindState()
{
	PT_ITEM_SPEC pItemProp = GetProp();
	FLASSERT( pItemProp );

	const int nBindCondition	= IsFlag( reset_bind ) == TRUE ? pItemProp->nResetBindCondition : pItemProp->nBindCondition;

	if( IsBindState() == false && nBindCondition == BIND_CONDITION_USE )
	{
		return true;
	}

	return false;
}

BOOL FLItemElem::IsUndestructable( void )
{
	PT_ITEM_SPEC pProperty = GetProp();

	if( pProperty )
	{
		if( (pProperty->dwFlag & IP_FLAG_UNDESTRUCTABLE ) == IP_FLAG_UNDESTRUCTABLE )
			return TRUE;
	}

	return FALSE;
}


BOOL FLItemElem::IsLogable( void )
{
	if( GetProp()->nLog != -1
		|| GetRandomOptionOriginID() != 0
		|| IsSetRandomOptionExtension() == true )
	{
		return TRUE;
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// mirchang_100820 기간제 아이템 설정..
BOOL FLItemElem::IsPeriod()
{
	PT_ITEM_SPEC pProperty = GetProp();
	if( pProperty != NULL )
	{
		if( ( pProperty->dwFlag & IP_FLAG_PERIOD ) == IP_FLAG_PERIOD )
		{
			return TRUE;
		}

		if( IsFlag( FLItemElem::period ) == TRUE )
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////


bool	FLItemElem::IsStackable( FLItemElem* pItemElem )
{
	PT_ITEM_SPEC pProp = GetProp();
	if( pProp != NULL && pItemElem != NULL )
	{
		if( IsEmpty() == FALSE
			&& m_dwItemId == pItemElem->m_dwItemId
			&& pProp->dwPackMax > 1
			&& m_nItemNum < (int)pProp->dwPackMax
			&& m_byFlag  == pItemElem->m_byFlag
			&& m_bCharged == pItemElem->m_bCharged
			&& IsPeriod() == FALSE
			&& pItemElem->IsPeriod() == FALSE
			&& m_dwKeepTime == 0
			&& pItemElem->m_dwKeepTime == 0 )
		{
			return true;
		}
	}

	return false;
}

CString FLItemElem::GetName( void )
{
	PT_ITEM_SPEC pProp	= GetProp();
	CString strName		= pProp->szName;
//sun: 9,
	if( pProp->dwItemKind3 == IK3_EGG && m_pPet /*&& m_pPet->GetLevel() > PL_EGG*/ )
	{
		MoverProp* pMoverProp	= prj.GetMoverProp( m_pPet->GetIndex() );
		if( pMoverProp )
		{
//sun: ?, __PET_1024
			if( m_pPet->HasName() )
				strName.Format( prj.GetText( TID_GAME_CAGE_STRING ), m_pPet->GetName() );
			else
				strName.Format( prj.GetText( TID_GAME_CAGE_STRING ), pMoverProp->szName );
		}
	}
/*
	else if( pProp->IsUltimate() )
	{
		int	nFind	= strName.Find( "@", 0 );
		if( nFind > -1 )
			strName.Delete( nFind - 1, 2 );
	}
*/
	return strName;
}

int	FLItemElem::GetMinLimitLevel( void )
{
	int nMinLimitLevel	= GetProp()->nMinLimitLevel - GetLevelDown();
	if( nMinLimitLevel <= 0 )
	{
		nMinLimitLevel	= 1;
	}

	return nMinLimitLevel;
}

//sun: 12, 무기 피어싱
// bSize는 피어싱 사이즈를 늘릴 수 있는지 검사할 때 TRUE값을 setting 한다.
// bSize를 TRUE로 할 경우 dwTagetItemKind3는 NULL_ID로 한다.
BOOL FLItemElem::IsPierceAble( DWORD dwTargetItemKind3, BOOL bSize )
{
	if( GetProp() == NULL )
	{
		return FALSE;
	}

	size_t nPiercedSize = GetGeneralPiercingSize();
	if( bSize ) // 피어싱 사이즈를 늘리는 경우
		nPiercedSize++;
	
	if( GetProp()->dwItemKind3 == IK3_SUIT )
	{
		if( nPiercedSize <= MAX_PIERCING_SUIT )
		{
			if( dwTargetItemKind3 == NULL_ID )
			{
				return TRUE;
			}
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
			else if( dwTargetItemKind3 == IK3_SOCKETCARD3 )
			{
				if( GetProp()->IsBarunaArmor() )
				{
					return TRUE;
				}
			}
#endif
			else if( dwTargetItemKind3 == IK3_SOCKETCARD )
			{
				return TRUE;
			}
			/*else if( dwTargetItemKind3 == IK3_BARUNAAMO_PIERCE_RUNE )
			{
				return TRUE;
			}*/
		}
	}
	
	else if( GetProp()->dwItemKind3 == IK3_SHIELD
		|| GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT
		|| GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC
		)
	{
		if( nPiercedSize <= MAX_PIERCING_WEAPON )
		{
			if( dwTargetItemKind3 == NULL_ID )
			{
				return TRUE;
			}
			else if( dwTargetItemKind3 == IK3_SOCKETCARD2 )
			{
				return TRUE;
			}
			/*else if( dwTargetItemKind3 == IK3_BARUNAWEA_PIERCE_RUNE )
			{
				return TRUE;
			}*/
		}
	}
	else if( IsVisPet() )
	{
		if( nPiercedSize <= MAX_VIS )
		{
			if( dwTargetItemKind3 == NULL_ID )
			{
				return TRUE;
			}
			else if( dwTargetItemKind3 == IK3_VIS )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//sun: 11,



BOOL FLItemElem::IsEgg()
{
	if( !IsPet() )
		return FALSE;
	return ( !m_pPet || m_pPet->GetLevel() == PL_EGG );
}

DWORD FLItemElem::GetVisPetSfxId()
{
	DWORD dwLevel = 0;
	for( size_t i=0; i<GetGeneralPiercingSize(); i++ )
	{
		PT_ITEM_SPEC pProp = g_xSpecManager->GetSpecItem( GetGeneralPiercingItemID( i ) );
		if( pProp && pProp->dwAbilityMax > dwLevel )
			dwLevel = pProp->dwAbilityMax;
	}

	switch( dwLevel )
	{
		case 1:	return XI_INDEX( 1914, XI_BUFFPET_GRADE1 );
		case 2: return XI_INDEX( 1915, XI_BUFFPET_GRADE2 );
		case 3: return XI_INDEX( 1916, XI_BUFFPET_GRADE3 );
	}

	return NULL_ID;
}

bool		FLItemElem::SelectRandomOption( BYTE bySelectFlag )
{
	bool	bRetValue	= true;

	if( bySelectFlag == _AWAKE_OLD_VALUE )
	{
		InitializeNewRandomOptionExtension();
	}
	else if( bySelectFlag == _AWAKE_NEW_VALUE )
	{
		OverwriteNewRandomOptionExtension();

		InitializeNewRandomOptionExtension();
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "bySelectFlag is invalid value. bySelectFlag : [%d]" ), (int)( bySelectFlag ) );

		bRetValue	= false;
	}

	return	bRetValue;
}

//	mulcom	END100405	각성 보호의 두루마리

//////////////////////////////////////////////////////////////////////////
void	FLItemElem::InitializeLooksChangeItemID()
{
	m_dwLooksChangeItemID = 0;
}

DWORD	FLItemElem::GetLooksChangeItemID() const
{
	return m_dwLooksChangeItemID;
}

void	FLItemElem::SetLooksChangeItemID( const DWORD dwItemID )
{
	PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( dwItemID );
	if( pItemSpec != NULL && pItemSpec->bIsLooksChangeMaterial == TRUE )
	{
		m_dwLooksChangeItemID = dwItemID;
	}
	else
	{
		InitializeLooksChangeItemID();
	}
}

bool	FLItemElem::IsMathingLooksChangeMaterialItem( const DWORD dwItemID ) const
{
	const T_ITEM_SPEC* pSrcItemProp		= g_xSpecManager->GetSpecItem( m_dwItemId );
	const T_ITEM_SPEC* pDestItemProp	= g_xSpecManager->GetSpecItem( dwItemID );
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return false;
	}

	if( pSrcItemProp->dwItemKind3 != pDestItemProp->dwItemKind3 )
	{
		return false;
	}

	if( pSrcItemProp->dwHanded != pDestItemProp->dwHanded )
	{
		return false;
	}

// 	if( pSrcItemProp->dwItemSex != pDestItemProp->dwItemSex )
// 	{
// 		return false;
// 	}

	const bool bSexSame = pSrcItemProp->dwItemSex != NULL_ID
		? ( pSrcItemProp->dwItemSex == pDestItemProp->dwItemSex || pDestItemProp->dwItemSex == NULL_ID )
		: ( pSrcItemProp->dwItemSex == pDestItemProp->dwItemSex );

	if( bSexSame == false )
	{
		return false;
	}

	return true;
}

bool	FLItemElem::IsSetCombinedOption() const
{
	return m_bCombined == true ? true : false;
}

void	FLItemElem::SetCombinedOption( bool bCombined )
{
	m_bCombined = bCombined;
}

void	FLItemElem::InitializeCombinedAddDamage()
{
	m_dwCombinedAddDamage = 0;
}

DWORD	FLItemElem::GetCombinedAddDamage() const
{
	return m_dwCombinedAddDamage;
}

void	FLItemElem::SetCombinedAddDamage( const DWORD dwCombinedAddDamage )
{
	if( IsSetCombinedOption() == true )
	{
		m_dwCombinedAddDamage = dwCombinedAddDamage;
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void	FLItemElem::InitializeCombinedPiercingItem()
{
	m_kPiercingOption.InitializeCombinedPiercingItem();
}

bool	FLItemElem::IsSetGeneralPiercingOption()
{
	return m_kPiercingOption.IsSetGeneralPiercingOption();
}

bool	FLItemElem::IsSetUltimatePiercingOption()
{
	return m_kPiercingOption.IsSetUltimatePiercingOption();
}

bool	FLItemElem::IsExistGeneralPiercedItemID( const DWORD dwItemID )
{
	return m_kPiercingOption.IsExistGeneralPiercedItemID( dwItemID );
}

bool	FLItemElem::IsSetCombinedGeneralPiercingOption()
{
	return m_kPiercingOption.IsSetCombinedGeneralPiercingOption();
}

size_t	FLItemElem::GetGeneralPiercingSize() const
{
	return m_kPiercingOption.GetGeneralPiercingSize();
}

size_t	FLItemElem::GetUltimatePiercingSize() const
{
	return m_kPiercingOption.GetUltimatePiercingSize();
}

size_t	FLItemElem::GetVisPetPiercingSize() const
{
	return m_kPiercingOption.GetVisPetPiercingSize();
}

size_t	FLItemElem::GetCombinedGeneralPiercingSize() const
{
	return m_kPiercingOption.GetCombinedGeneralPiercingSize();
}

DWORD	FLItemElem::GetGeneralPiercingItemID( const size_t Nth ) const
{
	return m_kPiercingOption.GetGeneralPiercingItemID( Nth );
}

DWORD	FLItemElem::GetUltimatePiercingItemID( const size_t Nth ) const
{
	return m_kPiercingOption.GetUltimatePiercingItemID( Nth );
}

time_t	FLItemElem::GetVisPetPiercingItemKeepTime( const size_t Nth ) const
{
	return m_kPiercingOption.GetVisPetPiercingItemKeepTime( Nth );
}

DWORD	FLItemElem::GetCombinedGeneralPiercingItemID( const size_t Nth ) const
{
	return m_kPiercingOption.GetCombinedGeneralPiercingItemID( Nth );
}

void	FLItemElem::SetGeneralPiercingSize( const size_t nSize )
{
	m_kPiercingOption.SetGeneralPiercingSize( nSize );
}

void	FLItemElem::SetUltimatePiercingSize( const size_t nSize )
{
	m_kPiercingOption.SetUltimatePiercingSize( nSize );
}

void	FLItemElem::SetVisPetPiercingSize( const size_t nSize )
{
	m_kPiercingOption.SetVisPetPiercingSize( nSize );
}

void	FLItemElem::SetCombinedGeneralPiercingSize( const size_t nSize )
{
	if( IsSetCombinedOption() == true )
	{
		m_kPiercingOption.SetCombinedGeneralPiercingSize( nSize );
	}
}

void	FLItemElem::SetGeneralPiercingItemID( const size_t Nth, const DWORD dwItemID )
{
	m_kPiercingOption.SetGeneralPiercingItemID( Nth, dwItemID );
}

void	FLItemElem::SetUltimatePiercingItemID( const size_t Nth, const DWORD dwItemID )
{
	m_kPiercingOption.SetUltimatePiercingItemID( Nth, dwItemID );
}

void	FLItemElem::SetVisPetPiercingItemKeepTime( const size_t Nth, const time_t tKeepTime )
{
	m_kPiercingOption.SetVisPetPiercingItemKeepTime( Nth, tKeepTime );
}

void	FLItemElem::SetCombinedGeneralPiercingItemID( const size_t Nth, const DWORD dwItemID )
{
	if( IsSetCombinedOption() == true )
	{
		m_kPiercingOption.SetCombinedGeneralPiercingItemID( Nth, dwItemID );
	}
}

void	FLItemElem::SwapVisItem( const size_t nSrc, const size_t nDest )
{
	m_kPiercingOption.SwapVisItem( nSrc, nDest );
}

void	FLItemElem::SerializePiercingOption( CAr & ar )
{
	m_kPiercingOption.SerializePiercingOption( ar );
}

void	FLItemElem::SerializeCombinedPiercingOption( CAr & ar )
{
	m_kPiercingOption.SerializeCombinedPiercingOption( ar );
}

void	FLItemElem::DeserializePiercingOption( CAr & ar )
{
	m_kPiercingOption.DeserializePiercingOption( ar );
}

void	FLItemElem::DeserializeCombinedPiercingOption( CAr & ar )
{
	m_kPiercingOption.DeserializeCombinedPiercingOption( ar );
}

void	FLItemElem::CopyPiercingOption( const FLPiercingOption & kPiercingOption )
{
	m_kPiercingOption = kPiercingOption;
}

void	FLItemElem::GetPiercingOption( FLPiercingOption & kPiercingOption )
{
	kPiercingOption = m_kPiercingOption;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void	FLItemElem::InitializeRandomOptionOriginID()
{
	m_wRandomOptionOriginID	= 0;
}

void	FLItemElem::InitializeCombinedRandomOptionOriginID()
{
	m_wCombinedRandomOptionOriginID	= 0;
}

WORD	FLItemElem::GetRandomOptionOriginID() const
{
	return m_wRandomOptionOriginID;
}

WORD	FLItemElem::GetCombinedRandomOptionOriginID() const
{
	return m_wCombinedRandomOptionOriginID;
}

void	FLItemElem::SetRandomOptionOriginID( const WORD wRandomOptionOriginID )
{
	m_wRandomOptionOriginID = wRandomOptionOriginID;
}

void	FLItemElem::SetCombinedRandomOptionOriginID( const WORD wCombinedRandomOptionOriginID )
{
	if( IsSetCombinedOption() == true )
	{
		m_wCombinedRandomOptionOriginID = wCombinedRandomOptionOriginID;
	}
}

void	FLItemElem::InitializeRandomOptionExtension()
{
	m_kRandomOption.InitializeRandomOptionExtension();
}

void	FLItemElem::InitializeNewRandomOptionExtension()
{
	m_kRandomOption.InitializeNewRandomOptionExtension();
}

void	FLItemElem::InitializeCombinedRandomOptionExtension()
{
	m_kRandomOption.InitializeCombinedRandomOptionExtension();
}

WORD	FLItemElem::GetRandomOptionExtensionFlag() const
{
	return m_kRandomOption.GetRandomOptionExtensionFlag();
}

void	FLItemElem::SetRandomOptionExtensionFlag( const WORD wRandomOptionExtensionFlag )
{
	m_kRandomOption.SetRandomOptionExtensionFlag( wRandomOptionExtensionFlag );
}

bool	FLItemElem::IsSetSafeGuardRandomOptionExtensionFlag() const
{
	return m_kRandomOption.IsSetSafeGuardRandomOptionExtensionFlag();
}

void	FLItemElem::SetSafeGuardRandomOptionExtensionFlag()
{
	m_kRandomOption.SetSafeGuardRandomOptionExtensionFlag();
}

void	FLItemElem::ResetSafeGuardRandomOptionExtensionFlag()
{
	m_kRandomOption.ResetSafeGuardRandomOptionExtensionFlag();
}

bool	FLItemElem::IsSetRandomOptionExtension() const
{
	return m_kRandomOption.IsSetRandomOptionExtension();
}

bool	FLItemElem::IsSetNewRandomOptionExtension() const
{
	return m_kRandomOption.IsSetNewRandomOptionExtension();
}

bool	FLItemElem::IsSetCombinedRandomOptionExtension() const
{
	return m_kRandomOption.IsSetCombinedRandomOptionExtension();
}

size_t	FLItemElem::GetRandomOptionExtensionSize() const
{
	return m_kRandomOption.GetRandomOptionExtensionSize();
}

size_t	FLItemElem::GetNewRandomOptionExtensionSize() const
{
	return m_kRandomOption.GetNewRandomOptionExtensionSize();
}

size_t	FLItemElem::GetCombinedRandomOptionExtensionSize() const
{
	return m_kRandomOption.GetCombinedRandomOptionExtensionSize();
}

WORD	FLItemElem::GetRandomOptionExtensionDstID( const size_t Nth ) const
{
	return m_kRandomOption.GetRandomOptionExtensionDstID( Nth );
}

WORD	FLItemElem::GetNewRandomOptionExtensionDstID( const size_t Nth ) const
{
	return m_kRandomOption.GetNewRandomOptionExtensionDstID( Nth );
}

WORD	FLItemElem::GetCombinedRandomOptionExtensionDstID( const size_t Nth ) const
{
	return m_kRandomOption.GetCombinedRandomOptionExtensionDstID( Nth );
}

short	FLItemElem::GetRandomOptionExtensionAdjValue( const size_t Nth ) const
{
	return m_kRandomOption.GetRandomOptionExtensionAdjValue( Nth );
}

short	FLItemElem::GetNewRandomOptionExtensionAdjValue( const size_t Nth ) const
{
	return m_kRandomOption.GetNewRandomOptionExtensionAdjValue( Nth );
}

short	FLItemElem::GetCombinedRandomOptionExtensionAdjValue( const size_t Nth ) const
{
	return m_kRandomOption.GetCombinedRandomOptionExtensionAdjValue( Nth );
}

bool	FLItemElem::SetRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension )
{
	return m_kRandomOption.SetRandomOptionExtension( kRandomOptionExtension );
}

bool	FLItemElem::SetNewRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension )
{
	return m_kRandomOption.SetNewRandomOptionExtension( kRandomOptionExtension );
}

bool	FLItemElem::SetCombinedRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension )
{
	if( IsSetCombinedOption() == true )
	{
		return m_kRandomOption.SetCombinedRandomOptionExtension( kRandomOptionExtension );
	}

	return false;
}

bool	FLItemElem::OverwriteNewRandomOptionExtension()
{
	return m_kRandomOption.OverwriteNewRandomOptionExtension();
}

void	FLItemElem::SerializeRandomOptionExtension( CAr & ar )
{
	m_kRandomOption.SerializeRandomOptionExtension( ar );
}

void	FLItemElem::SerializeNewRandomOptionExtension( CAr & ar )
{
	m_kRandomOption.SerializeNewRandomOptionExtension( ar );
}

void	FLItemElem::SerializeCombinedRandomOptionExtension( CAr & ar )
{
	m_kRandomOption.SerializeCombinedRandomOptionExtension( ar );
}

void	FLItemElem::DeserializeRandomOptionExtension( CAr & ar )
{
	m_kRandomOption.DeserializeRandomOptionExtension( ar );
}

void	FLItemElem::DeserializeNewRandomOptionExtension( CAr & ar )
{
	m_kRandomOption.DeserializeNewRandomOptionExtension( ar );
}

void	FLItemElem::DeserializeCombinedRandomOptionExtension( CAr & ar )
{
	m_kRandomOption.DeserializeCombinedRandomOptionExtension( ar );
}

void	FLItemElem::CopyRandomOption( const FLRandomOption & kRandomOption )
{
	m_kRandomOption	= kRandomOption;
}

void	FLItemElem::GetRandomOption( FLRandomOption & kRandomOption ) const
{
	kRandomOption = m_kRandomOption;
}

void	FLItemElem::RandomOptionDataConverting( __int64 n64RandomOptionId )
{
	if( n64RandomOptionId != 0 )
	{
		if( m_dwItemId == ITEM_INDEX( 3600, II_GEN_WARP_COUPLERING )
			|| m_dwItemId == ITEM_INDEX( 18186, II_GEN_WARP_WEDDING_BAND )
			|| m_dwItemId == ITEM_INDEX( 3601, II_GEN_WARP_COUPLERING01 )
			)
		{
			DWORD dwCouplePlayerId = static_cast<DWORD>( n64RandomOptionId );
			SetCoupleId( dwCouplePlayerId );
		}
		else
		{
			WORD wRandomOptionOriginId = static_cast<WORD>( n64RandomOptionId & 0x00000000000000FF );
			if( wRandomOptionOriginId > 0 )
			{
				SetRandomOptionOriginID( wRandomOptionOriginId );
			}

			const __int64  _AWAKE_SAFE_FLAG		= 0x2000000000000000;

			if( ( n64RandomOptionId & _AWAKE_SAFE_FLAG ) == _AWAKE_SAFE_FLAG )
			{
				SetSafeGuardRandomOptionExtensionFlag();
			}

			const __int64	LEVEL_DOWN_05_FLAG	= 0x4000000000000000;
			const __int64	LEVEL_DOWN_10_FLAG	= 0x8000000000000000;

			if( ( n64RandomOptionId & LEVEL_DOWN_05_FLAG ) == LEVEL_DOWN_05_FLAG )
			{
				SetLevelDown( 5 );
			}
			else if( ( n64RandomOptionId & LEVEL_DOWN_10_FLAG ) == LEVEL_DOWN_10_FLAG )
			{
				SetLevelDown( 10 );
			}

			int nRandomOptionSize	= 0;
			__int64 n64Location		= 0x3FFFF << 8;

			const DWORD OLD_MAX_RANDOMOPTION	= 3;

			for( int i = 0; i < OLD_MAX_RANDOMOPTION; ++i )
			{
				if( n64RandomOptionId & n64Location )
				{
					++nRandomOptionSize;
				}
				else
				{
					break;
				}

				n64Location	= n64Location << 18;
			}

			if( IsSetRandomOptionExtension() == false )
			{
				for( int Nth = 0; Nth < nRandomOptionSize; ++Nth )
				{
					T_RANDOMOPTION_EXT kRandomOption;

					int nRandomOption	= static_cast<int>( n64RandomOptionId >> ( 8 + Nth * 18 ) );	// 값을 가져오기 위하여 해당 위치로 시프트
					kRandomOption.shAdjValue	= static_cast<short>( nRandomOption & 0x000001FF );	// 하위 9비트 가져옴.
					if( nRandomOption & 0x00000200 )	// 최상위 10 비트가 1이면 음수
					{
						kRandomOption.shAdjValue	= -kRandomOption.shAdjValue;
					}

					nRandomOption	= nRandomOption >> 10;	// 10비트 밀고, 

					kRandomOption.wDstID	= static_cast<WORD>( nRandomOption & 0x0000007F );	// 8비트 가져옴

					SetRandomOptionExtension( kRandomOption );
				}
			}
			else
			{
				FLINFO_LOG( PROGRAM_NAME, _T( "Exist RandomOptionExtension. ItemId:[%d]" ), m_dwItemId );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
int	FLItemElem::GetLevelDown() const
{
	return m_nLevelDown;
}

void	FLItemElem::SetLevelDown( const int nLevelDown )
{
	m_nLevelDown = nLevelDown;
}

void	FLItemElem::InitializeLevelDown()
{
	m_nLevelDown = 0;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
DWORD	FLItemElem::GetCoupleId() const
{
	return m_dwCouplePlayerId;
}

void	FLItemElem::SetCoupleId( const DWORD dwPlayerId )
{
	m_dwCouplePlayerId = dwPlayerId;
}

void	FLItemElem::InitializeCoupleId()
{
	m_dwCouplePlayerId = 0;
}

void	FLItemElem::LogItemElem( TCHAR* pszLog, size_t nSize ) const
{
	FLSPrintf( pszLog, nSize, "m_dwItemId(%d)", m_dwItemId );
	FLSPrintf( pszLog, nSize, "%s, m_nItemNum(%d)", pszLog, m_nItemNum );
	FLSPrintf( pszLog, nSize, "%s, m_iSerialNumber(%d)", pszLog, GetSerialNumber() );
	FLSPrintf( pszLog, nSize, "%s, nAbilityOption(%d)", pszLog, m_nAbilityOption );
	FLSPrintf( pszLog, nSize, "%s, m_byItemResist(%d)", pszLog, m_byItemResist );
	FLSPrintf( pszLog, nSize, "%s, m_nResistAbilityOption(%d)", pszLog, m_nResistAbilityOption );
	FLSPrintf( pszLog, nSize, "%s, m_byFlag(%d)", pszLog, m_byFlag );
	FLSPrintf( pszLog, nSize, "%s, m_bCharged(%d)", pszLog, m_bCharged );
	FLSPrintf( pszLog, nSize, "%s, m_dwKeepTime(%d)", pszLog, m_dwKeepTime );
	FLSPrintf( pszLog, nSize, "%s, m_nHitPoint(%d)", pszLog, m_nHitPoint );
	FLSPrintf( pszLog, nSize, "%s, m_nRepairNumber(%d)", pszLog, m_nRepairNumber );
	FLSPrintf( pszLog, nSize, "%s, m_nRepair(%d)", pszLog, m_nRepair );
	FLSPrintf( pszLog, nSize, "%s, m_bTranformVisPet(%d)", pszLog, m_bTranformVisPet );
	
	FLSPrintf( pszLog, nSize, "%s, UltimatePiercingSize(%d)", pszLog, GetUltimatePiercingSize() );
	for( size_t Nth = 0; Nth < MAX_PIERCING_ULTIMATE; ++Nth )
	{
		FLSPrintf( pszLog, nSize, "%s, UltimatePiercingItemID_%d(%d)", pszLog, Nth, GetUltimatePiercingItemID( Nth ) );
	}
	
	FLSPrintf( pszLog, nSize, "%s, GeneralPiercingSize(%d)", pszLog, GetGeneralPiercingSize() );
	for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
	{
		FLSPrintf( pszLog, nSize, "%s, GeneralPiercingItemID_%d(%d)", pszLog, Nth, GetGeneralPiercingItemID( Nth ) );
	}

	for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
	{
		FLSPrintf( pszLog, nSize, "%s, GeneralPiercingKeepTime_%d(%d)", pszLog, Nth, GetVisPetPiercingItemKeepTime( Nth ) );
	}

	const BOOL bIsRarePet	= m_pPet != NULL ? TRUE : FALSE;

	FLSPrintf( pszLog, nSize, "%s, IsRarePet(%d)", pszLog, bIsRarePet );
	if( bIsRarePet == TRUE )
	{
		FLSPrintf( pszLog, nSize, "%s, PetKind(%d)", pszLog, m_pPet->GetKind() );
		FLSPrintf( pszLog, nSize, "%s, PetLevel(%d)", pszLog, m_pPet->GetLevel() );
		FLSPrintf( pszLog, nSize, "%s, PetExp(%d)", pszLog, m_pPet->GetExp() );
		FLSPrintf( pszLog, nSize, "%s, PetEnergy(%d)", pszLog, m_pPet->GetEnergy() );
		FLSPrintf( pszLog, nSize, "%s, PetLife(%d)", pszLog, m_pPet->GetLife() );
		
		for( BYTE byLevel = PL_D; byLevel < m_pPet->GetLevel(); ++byLevel )
		{
			FLSPrintf( pszLog, nSize, "%s, PetAvailLevel_%d(%d)", pszLog, byLevel, m_pPet->GetAvailLevel( byLevel ) );
		}
		
		FLSPrintf( pszLog, nSize, "%s, PetName = %s", pszLog, m_pPet->GetName() );
	}

	FLSPrintf( pszLog, nSize, "%s, m_dwCouplePlayerId(%d)", pszLog, m_dwCouplePlayerId );
	FLSPrintf( pszLog, nSize, "%s, m_nLevelDown(%d)", pszLog, m_nLevelDown );
	FLSPrintf( pszLog, nSize, "%s, m_wRandomOptionOriginID(%d)", pszLog, m_wRandomOptionOriginID );
	FLSPrintf( pszLog, nSize, "%s, RandomOptionExtensionFlag(%d)", pszLog, GetRandomOptionExtensionFlag() );
	
	FLSPrintf( pszLog, nSize, "%s, RandomOptionExtensionSize(%d)", pszLog, GetRandomOptionExtensionSize() );
	for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
	{
		FLSPrintf( pszLog, nSize, "%s, RandomOptionExtensionDstID_%d(%d)", pszLog, Nth, GetRandomOptionExtensionDstID( Nth*2, Nth*2+1 ) );
		FLSPrintf( pszLog, nSize, "%s, RandomOptionExtensionAdjValue_%d(%d)", pszLog, Nth, GetRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 ) );
	}
	
	if( _GetContentState( CT_DB_ITEM_EXTEND ) == CS_VER1 )
	{
		FLSPrintf( pszLog, nSize, "%s, m_idGuild(%d)", pszLog, m_idGuild );
		FLSPrintf( pszLog, nSize, "%s, m_nResistSMItemId(%d)", pszLog, m_nResistSMItemId );
		FLSPrintf( pszLog, nSize, "%s, m_dwLooksChangeItemID(%d)", pszLog, m_dwLooksChangeItemID );

		const bool bIsSetCombinedOption		= IsSetCombinedOption();
		FLSPrintf( pszLog, nSize, "%s, IsSetCombinedOption(%d)", pszLog, bIsSetCombinedOption );
		if( bIsSetCombinedOption == true )
		{
			FLSPrintf( pszLog, nSize, "%s, m_dwCombinedAddDamage(%d)", pszLog, m_dwCombinedAddDamage );
			FLSPrintf( pszLog, nSize, "%s, m_wCombinedRandomOptionOriginID(%d)", pszLog, m_wCombinedRandomOptionOriginID );
			
			FLSPrintf( pszLog, nSize, "%s, CombinedGeneralPiercingSize(%d)", pszLog, GetCombinedGeneralPiercingSize() );
			for( size_t Nth = 0; Nth < MAX_PIERCING; ++Nth )
			{
				FLSPrintf( pszLog, nSize, "%s, CombinedGeneralPiercingItemID_%d(%d)", pszLog, Nth, GetCombinedGeneralPiercingItemID( Nth ) );
			}

			FLSPrintf( pszLog, nSize, "%s, CombinedRandomOptionExtensionSize(%d)", pszLog, GetCombinedRandomOptionExtensionSize() );
			for( size_t Nth = 0; Nth < MAX_RANDOMOPTION_DB_COLUMN; ++Nth )
			{
				FLSPrintf( pszLog, nSize, "%s, CombinedRandomOptionExtensionDstID_%d(%d)", pszLog, Nth, GetCombinedRandomOptionExtensionDstID( Nth*2, Nth*2+1 ) );
				FLSPrintf( pszLog, nSize, "%s, CombinedRandomOptionExtensionAdjValue_%d(%d)", pszLog, Nth, GetCombinedRandomOptionExtensionAdjValue( Nth*2, Nth*2+1 ) );
			}
		}
	}
}

bool	FLItemElem::CanSellNPC() const
{
	if( IsQuest() == TRUE )
	{
		return false;
	}

	if( m_dwItemId == ITEM_INDEX( 26476, II_SYS_SYS_SCR_SEALCHARACTER ) )
	{
		return false;
	}

	if( m_dwItemId == ITEM_INDEX( 26456, II_SYS_SYS_SCR_PERIN ) )
	{
		return false;
	}

	const PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( m_dwItemId );
	if( pItemProp == NULL )
	{
		return false;
	}

	if( pItemProp->dwItemKind3 == IK3_EVENTMAIN )
	{
		return false;
	}

	if( pItemProp->dwParts == PARTS_RIDE && pItemProp->dwItemJob == JOB_VAGRANT )
	{
		return false;
	}

	if( GetCost() <= 0 )
	{
		return false;
	}

	return true;
}

void	FLItemElem::SetRandomOptionExtension( const DB_RANDOMOPTION_DST& DST, const DB_RANDOMOPTION_ADJ& ADJ )
{
	T_RANDOMOPTION_EXT lower_random_option;
	lower_random_option.wDstID		= static_cast<WORD>( DST.lower_DST );
	lower_random_option.shAdjValue	= static_cast<short>( ADJ.lower_ADJ );
	m_kRandomOption.SetRandomOptionExtension( lower_random_option );

	T_RANDOMOPTION_EXT upper_random_option;
	upper_random_option.wDstID		= static_cast<WORD>( DST.upper_DST );
	upper_random_option.shAdjValue	= static_cast<short>( ADJ.upper_ADJ );
	m_kRandomOption.SetRandomOptionExtension( upper_random_option );
}

void	FLItemElem::SetCombinedRandomOptionExtension( const DB_RANDOMOPTION_DST& DST, const DB_RANDOMOPTION_ADJ& ADJ )
{
	m_kRandomOption.SetCombinedRandomOptionExtension( T_RANDOMOPTION_EXT( static_cast<WORD>( DST.lower_DST ), static_cast<short>( ADJ.lower_ADJ ) ) );
	m_kRandomOption.SetCombinedRandomOptionExtension( T_RANDOMOPTION_EXT( static_cast<WORD>( DST.upper_DST ), static_cast<short>( ADJ.upper_ADJ ) ) );
}

int		FLItemElem::GetRandomOptionExtensionDstID( const size_t lower_nth, const size_t upper_nth ) const
{
	return DB_RANDOMOPTION_DST( m_kRandomOption.GetRandomOptionExtensionDstID( lower_nth ) , m_kRandomOption.GetRandomOptionExtensionDstID( upper_nth ) ).random_option_DST;
}

int		FLItemElem::GetCombinedRandomOptionExtensionDstID( const size_t lower_nth, const size_t upper_nth ) const
{
	return DB_RANDOMOPTION_DST( m_kRandomOption.GetCombinedRandomOptionExtensionDstID( lower_nth ) , m_kRandomOption.GetCombinedRandomOptionExtensionDstID( upper_nth ) ).random_option_DST;
}

int		FLItemElem::GetRandomOptionExtensionAdjValue( const size_t lower_nth, const size_t upper_nth ) const
{
	return DB_RANDOMOPTION_ADJ( m_kRandomOption.GetRandomOptionExtensionAdjValue( lower_nth ), m_kRandomOption.GetRandomOptionExtensionAdjValue( upper_nth ) ).random_option_ADJ;
}

int		FLItemElem::GetCombinedRandomOptionExtensionAdjValue( const size_t lower_nth, const size_t upper_nth ) const
{
	return DB_RANDOMOPTION_ADJ( m_kRandomOption.GetCombinedRandomOptionExtensionAdjValue( lower_nth ), m_kRandomOption.GetCombinedRandomOptionExtensionAdjValue( upper_nth ) ).random_option_ADJ;
}
