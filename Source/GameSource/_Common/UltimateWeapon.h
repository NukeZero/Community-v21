// UltimateWeapon.h: interface for the CUltimateWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ULTIMATEWEAPON_H__637EE691_5DDC_4A45_8D63_A5870157585B__INCLUDED_)
#define AFX_ULTIMATEWEAPON_H__637EE691_5DDC_4A45_8D63_A5870157585B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_JEWEL 10

//sun: 9,10차 제련 __ULTIMATE
class FLItemElem;
class CUltimateWeapon  
{
public:
	enum { ULTIMATE_SUCCESS, ULTIMATE_FAILED, ULTIMATE_CANCEL, ULTIMATE_ISULTIMATE,
			ULTIMATE_INVENTORY, ULTIMATE_ISNOTULTIMATE };
	
	struct __MAKEGEMPROB
	{
		int		nEnchant;
		DWORD	dwGeneralProb;
		int		nGeneralNum;
		DWORD	dwUniqueProb;
		int		nUniqueNum;
		__MAKEGEMPROB()
		{
			nEnchant = 0;
			dwGeneralProb = 0;
			nGeneralNum = 0;
			dwUniqueProb = 0;
			nUniqueNum = 0;
		};
	};

	struct __MAKEGEMITEM
	{
		DWORD	dwItemID;
		int		nMinLevel;
		int		nMaxLevel;
		__MAKEGEMITEM()
		{
			dwItemID = 0;
			nMinLevel = 0;
			nMaxLevel = 0;
		};
	};

	struct __GEMABILITY
	{
		DWORD		dwGemItemId;
		DWORD		dwAbilityItemId;
		std::vector<int>	vecAbility;
	};
	
	struct __GEMABILITYKIND
	{
		int						nAbility;
		std::vector<__GEMABILITY>	vecAbilityKind;
	};
		
	BOOL Load_GemAbility();
	DWORD GetGemKind( DWORD dwItemLV );
#ifdef __WORLDSERVER
	BOOL Load_UltimateWeapon();
	DWORD GetGemAbilityKindRandom( DWORD dwGemItemid );
	int MakeGem( FLWSUser* pUser, OBJID objItemId, int & nNum );
	int SetGem( FLWSUser* pUser, OBJID objItemId, OBJID objGemItemId );
	int RemoveGem( FLWSUser* pUser, OBJID objItemId, OBJID objItemGem );
	int MakeItem( FLWSUser* pUser, OBJID* objItemId );
	int  TransWeapon( FLWSUser* pUser, OBJID objItem, OBJID objGem1, OBJID objGem2 );
	int  EnchantWeapon( FLWSUser* pUser, OBJID objItem, OBJID objItem2 );
	BYTE SmeltSafetyUltimate( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr );
#endif //__WORLDSERVER
	void SetDestParamUltimate( CMover* pMover, FLItemElem* pItemElem, BOOL bEquip, DWORD dwParts );
	std::map<int, int> GetDestParamUltimate( FLItemElem* pItemElem );
	int GetDST( DWORD dwItemId, int nItemNum, int nDSTInfo );
	CUltimateWeapon();
	virtual ~CUltimateWeapon();

	std::vector<__MAKEGEMITEM>	m_vecMakeGemItem;
	std::vector<__GEMABILITYKIND>	m_vecGemAbilityKind;
#ifdef __WORLDSERVER
	std::map<int, __MAKEGEMPROB> m_mapMakeGemProb;	
	std::map<int, int>			m_mapUltimateProb;
	int						m_nSetGemProb;		// 보석합성 확률
	int						m_nRemoveGemProb;	// 보석제거 확률
	int						m_nGen2UniProb;		// 일반->유니크
	int						m_nUni2UltiProb;	// 유니크->얼터멋
#endif // __WORLDSERVER
};

#endif // !defined(AFX_ULTIMATEWEAPON_H__637EE691_5DDC_4A45_8D63_A5870157585B__INCLUDED_)
