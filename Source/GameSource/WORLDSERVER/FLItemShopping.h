#ifndef __FLITEMSHOPPING_H__
#define __FLITEMSHOPPING_H__


class FLItemShopping
{
	public:
		static FLItemShopping* GetInstance();

		FLItemShopping();
		~FLItemShopping();

		int		BuyItemByBasket( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket );


	private:
		int		BuyItemFromNPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket );
		int		CheckBuyItemFromNPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket );
		int		CreateItemFromNPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket );


		int		BuyItemFromPC( FLWSUser* pUser, const PT_PACKET_SHOPPING_BASKET ptShoppingBasket );


	public:
		enum ITEM_SHOPPING_RESULT
		{
			ERR_GENERAL_ERROR = 0
			, ERR_SUCCESS						// 성공
			, ERR_USER_INVALID					// 이상한 유저
			, ERR_ITEM_INVALID					// 이상한 아이템
			, ERR_VENDOR_INVALID				// 거래자 이상
			, ERR_SHOPPING_INFO_INVALID			// 구매정보 이상
			, ERR_LACK_SPACE					// 공간 부족
			, ERR_LACK_MONEY					// 돈 부족
			, ERR_LACK_CHIP						// 칩 부족
			, ERR_BUY_ONLY_OCCUPATION			// 점령길드 전용 아이템
			, ERR_SUMCOST_OVERFLOW				// 총 구매 아이템의 가격이 유저가 가질 수 있는 페냐량을 초과
			, ERR_SUMCHIP_OVERFLOW				// 총 구매 아이템의 칩이 유저가 가질 수 있는 칩을 초과
		};

};

#define g_xItemShopping	FLItemShopping::GetInstance()


#endif // __FLITEMSHOPPING_H__