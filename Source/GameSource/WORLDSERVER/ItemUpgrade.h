#pragma once

struct __ATTRIBUTE_ENCHANT
{
	int nProb;
	int nAddDamageRate;
	int nDefenseRate;
	int nAddAtkDmgRate;
	__ATTRIBUTE_ENCHANT()
	{
		nProb = 0;
		nAddDamageRate = 0;
		nDefenseRate = 0;
		nAddAtkDmgRate = 0;
	}
};

#ifdef BARUNA_UPGRADE_SMELT_SAFETY
struct __BARUNA_NPC_LEVEL
{
	DWORD dwExpRequired;
	DWORD dwAdditionalSuccessProp;
	
	__BARUNA_NPC_LEVEL()
	{
		dwExpRequired = 0;
		dwAdditionalSuccessProp = 0;
	}
};

struct __BARUNA_ENCHANT
{
	DWORD dwSuccessAdditionalExp;
	DWORD dwFailAddtionalExp;
	DWORD dwMinusAdditionalExp;
	DWORD dwSuccessProb;
	DWORD dwFailProb;
	DWORD dwMinusProb;
	
	__BARUNA_ENCHANT()
	{
		dwSuccessAdditionalExp = 0;
		dwFailAddtionalExp = 0;
		dwMinusAdditionalExp = 0;
		dwSuccessProb = 0;
		dwFailProb = 0;
	}
};

#endif

class CItemUpgrade
{
public:
	CItemUpgrade(void);
	~CItemUpgrade(void);
	static CItemUpgrade*	GetInstance( void );

	void	LoadScript();
	void	OnPiercingSize( FLWSUser* pUser, DWORD dwId1, DWORD dwId2, DWORD dwId3 );
	int		GetSizeProb( FLItemElem* pItemElem );
	void	OnPiercing( FLWSUser* pUser, DWORD dwItemId, DWORD dwSocketCard );
	void	OnPiercingRemove( FLWSUser* pUser, DWORD objId1, DWORD objId2 );

	BYTE	OnSmeltSafety( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr, FLItemElem* pItemSmeltScr );

	void	OnEnchant( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial );
	int		GetAttributeDamageFactor( int nAbilityOption );

	int		GetAttributeDefenseFactor( int nAbilityOption );
	int		GetAttributeAddAtkDmgFactor( int nAbilityOption );
#ifdef BARUNA_UPGRADE_SMELT_SAFETY
	__BARUNA_NPC_LEVEL GetBarunaNPCLevel( int nAbilityOption );
	__BARUNA_ENCHANT GetBarunaEnchant( int nAbilityOption, DWORD dwItemKind1 );
	int		GetMaxBarunaGeneralEnchantSize( DWORD dwItemKind1 );
#endif
	int		GetMaxGeneralEnchantSize() { return m_mapGeneralEnchant.size(); }
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	void	EnchantCostume( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial );
	int		GetMaxCostumeEnchantSize() { return m_mapCostumeEnchant.size(); }
	int		GetCostumeEnchantProb( int nAbilityOption );
#endif
	int		GetMaxAttributeEnchantSize() { return m_mapAttributeEnchant.size(); }
	void	ChangeAttribute( FLWSUser* pUser, DWORD dwTargetItem, DWORD dwUseItem , SAI79::ePropType nAttribute );

	void	OnItemTransy( FLWSUser* pUser, OBJID objidTarget, OBJID objidTransy, DWORD dwChangeId, BOOL bCash );


	//////////////////////////////////////////////////////////////////////////
	// item의 종류에 따른 제련 최대 수치
	int		GetMaxEnchantSize( FLItemElem* pItemElem );
	//////////////////////////////////////////////////////////////////////////
private:
	BOOL	RunItemTransy( FLWSUser* pUser, FLItemElem* pItemElemTarget, DWORD dwChangeId );

private:
	std::map<int, int>	m_mapSuitProb;
#ifdef BARUNA_UPGRADE_SUIT_PIERCING
	std::map<int, int>	m_mapBarunaSuitProb;
#endif
	std::map<int, int>	m_mapWeaponProb;
	
private:
	void	RefineAccessory( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial );
	void	RefineCollector( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial );
	BYTE	SmeltSafetyNormal( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr, FLItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyGeneral( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr, FLItemElem* pItemSmeltScr );
#ifdef BARUNA_UPGRADE_SMELT_SAFETY
	BYTE	SmeltSafetyBarunaGeneral( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr );
#endif
	BYTE	SmeltSafetyAccessory( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr );
	BYTE	SmeltSafetyPiercingSize( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr );

	BYTE	SmeltSafetyAttribute(FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr, FLItemElem* pItemSmeltScr );
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	BYTE	SmeltSafetyCostume(FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr );
#endif
public:

	void	EnchantGeneral( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial );
	int		GetGeneralEnchantProb( int nAbilityOption );
	void	EnchantAttribute( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial );
	int		GetAttributeEnchantProb( int nAbilityOption );
	
	DWORD	WhatEleCard( DWORD dwItemType );	// 곧 삭제

	std::map<int, int>	m_mapGeneralEnchant;
#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
	std::map<int, int>	m_mapCostumeEnchant;
#endif
	std::map<int, __ATTRIBUTE_ENCHANT> m_mapAttributeEnchant;

#ifdef BARUNA_UPGRADE_SMELT_SAFETY
	std::map<int, __BARUNA_NPC_LEVEL> m_mapBarunaNPCLevel;
	std::map<int, __BARUNA_ENCHANT> m_mapBarunaWeaponEnchant;
	std::map<int, __BARUNA_ENCHANT> m_mapBarunaArmorEnchant;
#endif

public:
	void	PetVisSize( FLWSUser* pUser, OBJID objIdMaterial );
	void	SetPetVisItem( FLWSUser* pUser, OBJID objIdVis );
	void	RemovePetVisItem( FLWSUser* pUser, int nPosition, BOOL bExpired = FALSE );
	void	PutPetVisItemLog( FLWSUser* pUser, const char* szAction, const char* szContext, FLItemElem* pItem, int nPosition );
	void	SwapVis( FLWSUser* pUser, int nPos1, int nPos2 );
	void	ChangeVisPetSfx( FLWSUser* pUser, FLItemElem* pItemElemPet );
	void	TransFormVisPet( FLWSUser* pUser, OBJID objIdMaterial );

};
