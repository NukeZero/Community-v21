
#ifndef __FLITEMUPGRADEACCESSORY_H__
#define __FLITEMUPGRADEACCESSORY_H__


#include "FLItemUpgradeBase.h"


class FLItemUpgradeAccessory : public FLItemUpgradeBase
{
	public:
		FLItemUpgradeAccessory();
		virtual ~FLItemUpgradeAccessory();

		static FLItemUpgradeAccessory*	GetInstance();

		bool			LoadScript( const TCHAR* pszFileName );

		virtual DWORD	OnDoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;
		
		virtual size_t	GetMaxSizeGeneralEnchant( const DWORD dwItemID ) const;


	private:
		void			Clear();

		bool			CanUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;

		DWORD			DoUpgradeItemGeneralEnchant( FLWSUser* pUser, const FLPacketItemUpgradeGeneralEnchantReq* pMsg ) const;


	private:
		EnchantDataVec				m_vecGeneralEnchantData;
};

#endif // __FLITEMUPGRADEACCESSORY_H__
