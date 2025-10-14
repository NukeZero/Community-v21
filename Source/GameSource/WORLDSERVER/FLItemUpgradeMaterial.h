#ifndef __FLITEMUPGRADEMATERIAL_H__
#define __FLITEMUPGRADEMATERIAL_H__

#include "FLItemUpgradeBase.h"

class FLItemUpgradeMaterial : public FLItemUpgradeBase
{
public:
	FLItemUpgradeMaterial(void);
	~FLItemUpgradeMaterial(void);

	static FLItemUpgradeMaterial*	GetInstance();

public:
#ifdef COSTUME_UPGRADE_MIX
		virtual DWORD	OnDoUpgradeItemMix( FLWSUser* pUser, const FLPacketItemUpgradeMixReq* pMsg, DWORD &dwItemIndex ) const;
#endif
};

#endif // __FLITEMUPGRADEMATERIAL_H__