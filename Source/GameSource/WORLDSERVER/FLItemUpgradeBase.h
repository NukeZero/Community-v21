
#ifndef __FLITEMUPGRADEBASE_H__
#define __FLITEMUPGRADEBASE_H__


#include "FLItemUpgradeDataStruct.h"


//////////////////////////////////////////////////////////////////////////
// TODO : RTTI을 이용. 아규먼트는 베이스 클래스로.. 사용 코드에서는 다운캐스팅 후 사용..?
class FLPacketItemUpgradeGeneralEnchantReq;
class FLPacketItemUpgradeAttributeEnchantReq;
class FLPacketItemUpgradeAttributeChangeReq;
class FLPacketItemUpgradeAttributeRemoveReq;
class FLPacketItemUpgradePiercingSizeIncreaseReq;
class FLPacketItemUpgradePiercingItemInsertReq;
class FLPacketItemUpgradePiercingItemRemoveReq;
class FLPacketItemUpgradeRandomOptionGenerateReq;
class FLPacketItemUpgradeRandomOptionGenerateRetryReq;
class FLPacketItemUpgradeRandomOptionInitializeReq;
class FLPacketItemUpgradeEquipLevelDecreaseReq;
class FLPacketItemUpgradeEquipLevelInitializeReq;
class FLPacketItemUpgradeCombineReq;
class FLPacketItemUpgradeCombineInitializeDataReq;
class FLPacketItemUpgradeCombineInitializeReq;
class FLPacketItemUpgradeLooksChangeReq;
class FLPacketItemUpgradeLooksInitializeReq;
#ifdef BARUNA_ULTIMATE_UPDATE
class FLPacketItemUpgradeUltimateTransReq;
#endif
class FLPacketItemUpgradeCardReq;

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
class FLPacketItemUpgradeSetGemReq;
class FLPacketItemUpgradeRemoveGemReq;
#endif

#ifdef COSTUME_UPGRADE_MIX
class FLPacketItemUpgradeMixReq;
#endif


//////////////////////////////////////////////////////////////////////////


class FLItemUpgradeBase
{
	public:
		FLItemUpgradeBase();
		virtual ~FLItemUpgradeBase();


		virtual DWORD	OnDoUpgradeItemGeneralEnchant( FLWSUser* , const FLPacketItemUpgradeGeneralEnchantReq*  ) const							{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemAttributeEnchant( FLWSUser* , const FLPacketItemUpgradeAttributeEnchantReq*  ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemAttributeChange( FLWSUser* , const FLPacketItemUpgradeAttributeChangeReq*  ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemAttributeRemove( FLWSUser* , const FLPacketItemUpgradeAttributeRemoveReq*  ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}

		virtual DWORD	OnDoUpgradeItemPiercingSizeIncrease( FLWSUser* , const FLPacketItemUpgradePiercingSizeIncreaseReq*  ) const				{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemPiercingItemInsert( FLWSUser* , const FLPacketItemUpgradePiercingItemInsertReq*  ) const					{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemPiercingItemRemove( FLWSUser* , const FLPacketItemUpgradePiercingItemRemoveReq*  ) const					{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		
		virtual DWORD	OnDoUpgradeItemRandomOptionGenerate( FLWSUser* , const FLPacketItemUpgradeRandomOptionGenerateReq*  ) const				{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemRandomOptionGenerateRetry( FLWSUser* , const FLPacketItemUpgradeRandomOptionGenerateRetryReq*  ) const	{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemRandomOptionInitialize( FLWSUser* , const FLPacketItemUpgradeRandomOptionInitializeReq*  ) const			{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		
		virtual DWORD	OnDoUpgradeItemEquipLevelDecrease( FLWSUser* , const FLPacketItemUpgradeEquipLevelDecreaseReq*  ) const					{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemEquipLevelInitialize( FLWSUser* , const FLPacketItemUpgradeEquipLevelInitializeReq*  ) const				{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}

		virtual DWORD	OnDoUpgradeItemCombine( FLWSUser* , const FLPacketItemUpgradeCombineReq*  ) const										{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual int		OnDoUpgradeItemCombineInitializeData( FLWSUser* , const FLPacketItemUpgradeCombineInitializeDataReq*  ) const			{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemCombineInitialize( FLWSUser* , const FLPacketItemUpgradeCombineInitializeReq*  ) const					{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}

		virtual DWORD	OnDoUpgradeItemLooksChange( FLWSUser* , const FLPacketItemUpgradeLooksChangeReq*  ) const								{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemLooksInitialize( FLWSUser* , const FLPacketItemUpgradeLooksInitializeReq*  ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}


		virtual DWORD	GetAttributeDamageFactor( const FLItemElem &  ) const																	{	return 0;	}
		virtual DWORD	GetAttributeDefenseFactor( const FLItemElem &  ) const																	{	return 0;	}
		virtual DWORD	GetAttributeAddAtkDmgFactor( const FLItemElem &  ) const																{	return 0;	}

		virtual bool	GetRandomOptionDummy( const FLItemElem & , const BYTE , RandomOptionExtensionVec & , BYTE &  ) const					{	return false;	}
		virtual bool	GetCombineInitializeData( const FLItemElem & , T_COMBINE_DATA &  ) const												{	return false;	}

		virtual size_t	GetMaxSizeGeneralEnchant( const DWORD  ) const																			{	return 0;	}
		virtual size_t	GetMaxSizeAttributeEnchant( const DWORD  ) const																		{	return 0;	}
		virtual size_t	GetMaxSizePiercing( const DWORD  ) const																				{	return 0;	}
		virtual size_t	GetMaxSizeRandomOption( const DWORD  ) const																			{	return 0;	}
#ifdef CARD_UPGRADE_SYSTEM
		virtual DWORD	OnDoUpgradeItemCard( FLWSUser* , const FLPacketItemUpgradeCardReq*  ) const										{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
#endif	// CARD_UPGRADE_SYSTEM
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
		virtual DWORD	OnDoUpgradeItemSetGem( FLWSUser* , const FLPacketItemUpgradeSetGemReq* ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
		virtual DWORD	OnDoUpgradeItemRemoveGem( FLWSUser* , const FLPacketItemUpgradeRemoveGemReq* ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
#endif
#ifdef COSTUME_UPGRADE_MIX
		virtual DWORD	OnDoUpgradeItemMix( FLWSUser* , const FLPacketItemUpgradeMixReq*, DWORD & ) const						{	return FSC_ITEMUPGRADE_NOT_SUPPORT;	}
#endif
	protected:
		bool	CanUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const;
		bool	CanUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const;


		DWORD	DoUpgradeItemLooksChange( FLWSUser* pUser, const FLPacketItemUpgradeLooksChangeReq* pMsg ) const;
		DWORD	DoUpgradeItemLooksInitialize( FLWSUser* pUser, const FLPacketItemUpgradeLooksInitializeReq* pMsg ) const;




		bool							LoadGeneralEnchantData( CLuaBase & kLua, EnchantDataVec & vecGeneralEnchantData ) const;
		bool							LoadAttributeEnchantData( CLuaBase & kLua, EnchantDataVec & vecAttributeEnchantData ) const;
		bool							LoadPiercingData( CLuaBase & kLua, PiercingDataVec & vecPiercingData ) const;
		bool							LoadRandomOptionExtensionData( CLuaBase & kLua, DstParameterDataMap & mapDstParameterData, RandomOptionDataVec & vecRandomOptionData ) const;
		bool							LoadCombineData( CLuaBase & kLua, CombineDataVec & vecCombineData ) const;
#ifdef CARD_UPGRADE_SYSTEM
		bool							LoadCardCombineData( CLuaBase & kLua, CardCombineDataVec & vecCombineData ) const;
#endif	// CARD_UPGRADE_SYSTEM

		const T_ENCHANT_DATA*			GetEnchantData( const T_ITEM_SPEC* pItemSpec, const EnchantDataVec & vecEnchantData ) const;
		const T_PIERCING_DATA*			GetPiercingData( const T_ITEM_SPEC* pItemSpec, const PiercingDataVec & vecPiercingData ) const;
		const T_RANDOMOPTION_DATA*		GetRandomOptionData( const T_ITEM_SPEC* pItemSpec, const RandomOptionDataVec & vecRandomOptionData ) const;
		const T_DST_PARAMETER_DATA*		GetDstParameterData( const WORD wDstID, const DstParameterDataMap & mapDstParameterData ) const;
		const T_COMBINE_DATA*			GetCombineData( const T_ITEM_SPEC* pItemSpec, const CombineDataVec & vecCombineData ) const;
#ifdef CARD_UPGRADE_SYSTEM
		const T_CARD_COMBINE_DATA*			GetCardCombineData( const T_ITEM_SPEC* pItemSpec, const CardCombineDataVec & vecCardCombineData ) const;
#endif	// CARD_UPGRADE_SYSTEM
		DWORD							DetermineRandomOptionSize( const UpgradeProbDataMap & mapSizeProb ) const;
		WORD							DetermineRandomOptionDst( const DSTProbVec & vecDstProb ) const;
		short							DetermineRandomOptionAdjValue( const ADJProbVec & vecAdjProb, const bool bDecreaseAdjValue = false ) const;

		bool							SetRandomOptionExtension( FLItemElem & kItemElem, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const;
		bool							SetNewRandomOptionExtension( FLItemElem & kItemElem, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const;
		bool							SetRetryRandomOptionExtension( FLItemElem & kItemElem, const BYTE byIndex, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const;
		bool							SetDummyRandomOptionExtension( RandomOptionExtensionVec & vecDummyRandomOption, const FLItemElem & kItemElem, const RandomOptionDataVec & vecRandomOptionData, const DstParameterDataMap & mapDstParameterData ) const;

		bool							IsValidStateUpgradeItem( FLWSUser* pUser, const DWORD dwMainItemObjID, const DWORD dwUpgradeType = NULL_ID ) const;
};

#endif // __FLITEMUPGRADEBASE_H__
