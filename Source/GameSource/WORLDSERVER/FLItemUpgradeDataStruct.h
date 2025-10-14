
#ifndef __FLITEMUPGRADEDATASTRUCT_H__
#define __FLITEMUPGRADEDATASTRUCT_H__


const DWORD		MAX_UPGRADE_PROB			= 10000;
const DWORD		MAX_RANDOMOPTION_PROB		= 1000000000;


//////////////////////////////////////////////////////////////////////////
struct T_ITEM_KIND
{
	DWORD	dwItemKind1;
	DWORD	dwItemKind2;
	DWORD	dwItemKind3;

	T_ITEM_KIND() : dwItemKind1( IK1_NONE ), dwItemKind2( IK2_NONE ), dwItemKind3( IK3_NONE )
	{
	}
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
struct T_UPGRADE_PROBABILITY
{
	DWORD	dwSuccessProb;
	DWORD	dwFailProb;
	DWORD	dwMinusProb;

	T_UPGRADE_PROBABILITY() : dwSuccessProb( 0 ), dwFailProb( 0 ), dwMinusProb( 0 )
	{
	}
};

typedef		std::map< DWORD /*Enchant Level*/, T_UPGRADE_PROBABILITY >			UpgradeProbDataMap;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct T_ITEM_ABILITY_PROPERTY
{
	DWORD	dwAddDamageRate;
	DWORD	dwAddDefenseRate;
	DWORD	dwAddAtkDmgRate;

	T_ITEM_ABILITY_PROPERTY() : dwAddDamageRate( 0 ), dwAddDefenseRate( 0 ), dwAddAtkDmgRate( 0 )
	{
	}
};

typedef		std::map< DWORD /*Enchant Level*/, T_ITEM_ABILITY_PROPERTY >			AbilityPropDataMap;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct	T_ENCHANT_DATA : T_ITEM_KIND
{
	int					nAttributeRemovePenya;
	T_ITEM_KIND			kMaterialKind;
	UpgradeProbDataMap	mapUpgradeProb;
	AbilityPropDataMap	mapAbilityPropData;

	T_ENCHANT_DATA() : nAttributeRemovePenya( 0 )
	{
		mapUpgradeProb.clear();
		mapAbilityPropData.clear();
	}
};

typedef		std::vector< T_ENCHANT_DATA >		EnchantDataVec;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct	T_PIERCING_DATA : T_ITEM_KIND
{
	int					nSizeIncreasePenya;
	int					nItemRemovePenya;
	T_ITEM_KIND			kMaterialKind;
	T_ITEM_KIND			kPiercedKind;
	UpgradeProbDataMap	mapSizeProb;

	T_PIERCING_DATA() : nSizeIncreasePenya( 0 ), nItemRemovePenya( 0 )
	{
		mapSizeProb.clear();
	}
};

typedef		std::vector< T_PIERCING_DATA >		PiercingDataVec;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef struct _T_ADJ_PROBABILITY
{
	short	shAdjValue;
	DWORD	dwProb;

	_T_ADJ_PROBABILITY() : shAdjValue( 0 ), dwProb( 0 )
	{
	}

} T_ADJ_PROBABILITY, *PT_ADJ_PROBABILITY;

typedef std::vector< T_ADJ_PROBABILITY >	ADJProbVec;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef struct _T_DST_PARAMETER_DATA
{
	WORD		wDstID;
	ADJProbVec	vecAdjProb;
	ADJProbVec	vecRetryAdjProb;
	ADJProbVec	vecDummyAdjProb;

	_T_DST_PARAMETER_DATA() : wDstID( 0 )
	{
		vecAdjProb.clear();
		vecRetryAdjProb.clear();
		vecDummyAdjProb.clear();
	}

} T_DST_PARAMETER_DATA, *PT_DST_PARAMETER_DATA;

typedef		std::map< WORD /*DST ID*/, T_DST_PARAMETER_DATA >		DstParameterDataMap;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef struct	_T_DST_PROBABILITY
{
	WORD	wDstID;
	DWORD	dwProb;

	_T_DST_PROBABILITY() : wDstID( 0 ), dwProb( 0 )
	{
	}

} T_DST_PROBABILITY, *PT_DST_PROBABILITY;

typedef std::vector< T_DST_PROBABILITY >	DSTProbVec;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef struct _T_RANDOMOPTION_DATA	: T_ITEM_KIND
{
	int						nRandomOptionGeneratePenya;
	DWORD					dwDummyRandomOptionSize;
	T_ITEM_KIND				kMaterialKind;
	UpgradeProbDataMap		mapSizeProb;
	DSTProbVec				vecDstProb;
	DSTProbVec				vecRetryDstProb;
	DSTProbVec				vecDummyDstProb;

	_T_RANDOMOPTION_DATA() : nRandomOptionGeneratePenya( 0 ), dwDummyRandomOptionSize( 0 )
	{
		mapSizeProb.clear();
		vecDstProb.clear();
		vecRetryDstProb.clear();
		vecDummyDstProb.clear();
	}

} T_RANDOMOPTION_DATA, *PT_RANDOMOPTION_DATA;

typedef		std::vector< T_RANDOMOPTION_DATA >		RandomOptionDataVec;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
typedef struct _T_COMBINE_DATA : T_ITEM_KIND
{
	DWORD	dwCombineOption;
	DWORD	dwSuccessProb;
	int		nInitializePenya;

	_T_COMBINE_DATA() : dwCombineOption( 0 ), dwSuccessProb( 0 ), nInitializePenya( 0 )
	{
	}

} T_COMBINE_DATA, *PT_COMBINE_DATA;

typedef		std::vector< T_COMBINE_DATA >			CombineDataVec;
//////////////////////////////////////////////////////////////////////////

#ifdef CARD_UPGRADE_SYSTEM
//////////////////////////////////////////////////////////////////////////
typedef struct _T_CARD_COMBINE_DATA : T_ITEM_KIND
{
	DWORD	dwItemIndex;
	DWORD	dwSuccessProb;

	_T_CARD_COMBINE_DATA() : dwItemIndex( 0 ), dwSuccessProb( 0 )
	{
	}

} T_CARD_COMBINE_DATA, *PT_CARD_COMBINE_DATA;

typedef		std::vector< T_CARD_COMBINE_DATA >			CardCombineDataVec;
//////////////////////////////////////////////////////////////////////////
#endif	// CARD_UPGRADE_SYSTEM

#ifdef BARUNA_ULTIMATE_UPDATE
//////////////////////////////////////////////////////////////////////////
struct	T_ULTIMATETRANS_DATA : T_ITEM_KIND
{
	
	T_ITEM_KIND			kMaterialKind1;
	T_ITEM_KIND			kMaterialKind2;
	int					nInitializePenya;
	T_ULTIMATETRANS_DATA() : nInitializePenya( 0 )
	{

	}
};

typedef		std::vector< T_ULTIMATETRANS_DATA >		UltimateTransDataVec;
//////////////////////////////////////////////////////////////////////////
#endif	// BARUNA_ULTIMATE_UPDATE
#endif // __FLITEMUPGRADEDATASTRUCT_H__
