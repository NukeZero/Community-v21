
#include "StdAfx.h"


DWORD	T_ITEM_SPEC::GetCoolTime() const
{
	return dwSkillReady;
}

BOOL	T_ITEM_SPEC::IsAccessory() const
{
	if( dwItemKind2 == IK2_JEWELRY )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsCollector() const
{
	if( dwItemKind3 == IK3_COLLECTER )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsWeaponParts() const
{
	if( dwParts == PARTS_RWEAPON )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsArmorParts() const
{
	if( dwParts == PARTS_SHIELD
		|| dwParts == PARTS_CAP
		|| dwParts == PARTS_UPPER_BODY
		|| dwParts == PARTS_HAND
		|| dwParts == PARTS_FOOT )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsCostumeParts() const
{
	if( dwParts == PARTS_CLOAK
		|| dwParts == PARTS_HAT
		|| dwParts == PARTS_CLOTH
		|| dwParts == PARTS_GLOVE
		|| dwParts == PARTS_BOOTS )
	{
		return TRUE;
	}

	return FALSE;
}

#ifdef COSTUME_UPGRADE_ENHANCEMENT_GEM
BOOL	T_ITEM_SPEC::IsCostumeEnhanceParts() const
{
	if( dwParts == PARTS_HAT
		|| dwParts == PARTS_CLOTH
		|| dwParts == PARTS_GLOVE
		|| dwParts == PARTS_BOOTS )
	{
		return TRUE;
	}

	return FALSE;
}
#endif

BOOL	T_ITEM_SPEC::IsSystemPet() const
{
	if( dwItemKind3 == IK3_EGG )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsEatPet() const
{
	if( dwItemKind3 == IK3_PET )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsUltimate() const
{
	if( dwReferStat1 == WEAPON_ULTIMATE )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsBaruna() const
{
	if( dwItemGrade == ITEM_GRADE_BARUNA )
	{
		return TRUE;
	}

	if( BARUNA_D <= dwReferStat1 && dwReferStat1 <= BARUNA_S )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsBarunaWeapon() const
{
	if( IsBaruna() && IsWeaponParts() )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsBarunaArmor() const
{
	if( IsBaruna() && IsArmorParts() )
	{
		return TRUE;
	}

	return FALSE;
}

#ifdef BARUNA_ULTIMATE_UPDATE
BOOL	T_ITEM_SPEC::IsBarunaUltimate() const
{
	if( dwItemGrade == ITEM_GRADE_BARUNAULTIMATE )
		return TRUE;
	
	if ( BARUNA_ULTIMATE == dwReferStat1 )
		return TRUE;
	return FALSE;
}
#endif

BOOL	T_ITEM_SPEC::IsVisPet() const
{
	if( dwItemKind3 == IK3_PET && dwReferStat1 == PET_VIS )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsVis() const
{
	if( dwItemKind3 == IK3_VIS )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsVisKey() const
{
	return ( dwID == ITEM_INDEX( 26855, II_SYS_SYS_VIS_KEY01 ) );
}

BOOL	T_ITEM_SPEC::IsPickupToBuff() const
{
	return ( dwID == ITEM_INDEX( 26910, II_SYS_SYS_SCR_PET_MAGIC ) );
}

BOOL	T_ITEM_SPEC::IsGold() const
{
	return ( dwID == ITEM_INDEX( 12, II_GOLD_SEED1 )
			|| dwID == ITEM_INDEX( 13, II_GOLD_SEED2 )
			|| dwID == ITEM_INDEX( 14, II_GOLD_SEED3 )
			|| dwID == ITEM_INDEX( 15, II_GOLD_SEED4 ) );
}

int		T_ITEM_SPEC::GetInventoryType() const
{
	int nInvenType = INVEN_TYPE_NONE;

	if( _GetContentState( CT_NEW_INVENTORY ) == CS_VER2 )
	{
		if( dwItemKind3 == IK3_PET || dwItemKind3 == IK3_EGG || dwItemKind3 == IK3_VIS )
		{
			nInvenType = INVEN_TYPE_PET;
		}
		else if( dwItemKind2 == IK2_CLOTHETC || dwItemKind3 == IK3_CLOTH )
		{
			nInvenType = INVEN_TYPE_COSTUME;
		}
		else if( dwItemKind3 == IK3_QUEST || dwItemKind3 == IK3_EVENTMAIN )
		{
			nInvenType = INVEN_TYPE_QUEST;
		}
		else
		{
			nInvenType = INVEN_TYPE_GENERAL;
		}
	}

	return nInvenType;
}

BOOL	T_ITEM_SPEC::IsUpgradeAble( int nUpgradeType ) const
{
	BOOL bUpgradeAble = FALSE;

	switch( nUpgradeType )
	{
	case IUTYPE_ENCHANT_GENERAL:
		{
			if( dwGeneralEnchantMode == 1 )
			{
				bUpgradeAble = TRUE;
			}
		}
		break;

	case IUTYPE_ENCHANT_ATTRIBUTE:
		{
			if( dwAttributeEnchantMode == 1 )
			{
				bUpgradeAble = TRUE;
			}
		}
		break;

	case IUTYPE_ENCHANT_GEM:
		{
			if( dwGemEnchantMode == 1 )
			{
				bUpgradeAble = TRUE;
			}
		}
		break;

	case IUTYPE_PIERCING:
		{
			if( dwPiercingMode == 1 )
			{
				bUpgradeAble = TRUE;
			}
		}
		break;

	case IUTYPE_RANDOM_OPTION_EXTENSION:
		{
			if( dwRandomOptionMode == 1 )
			{
				bUpgradeAble = TRUE;
			}
		}
		break;
#ifdef BARUNA_ULTIMATE_UPDATE
	case IUTYPE_ULTIMATE_TRANS:
		{
			if( dwReferTarget1 )
			{
				bUpgradeAble = TRUE;
			}
		}
		break;
#endif	
		
	default:
		break;
	}

	return bUpgradeAble;
}

BOOL	T_ITEM_SPEC::IsAbsoluteTime() const
{
	if( dwItemKind2 == IK2_BUFF2 || bAbsoluteTime == TRUE )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL	T_ITEM_SPEC::IsDebuff() const
{
	if( nEvildoing < 0 )
	{
		return TRUE;
	}

	return FALSE;
}
