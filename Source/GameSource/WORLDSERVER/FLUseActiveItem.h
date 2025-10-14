#ifndef __FLUSEACTIVEITEM_H__
#define __FLUSEACTIVEITEM_H__


class FLUseActiveItem
{
	public:
		static	FLUseActiveItem*	GetInstance();

		FLUseActiveItem();
		~FLUseActiveItem();

		int		OnDoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		OnDoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem );

	private:
		int		DoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		CheckActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		UseResetBind( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		UseBarunaGeneralEnchantProtection( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		UseBarunaAttributeEnchantProtection( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		UseBarunaAttributeEnchantProbability( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		UseBarunaPiercingProtection( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );
		int		UseBarunaPiercingProb( FLWSUser* pUser, FLItemElem* pSrcItem, FLItemElem* pDestItem );

		int		DoUseActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem );
		int		CheckActiveItem( FLWSUser* pUser, FLItemElem* pSrcItem );
		int		UseColosseumRetry( FLWSUser* pUser, FLItemElem* pSrcItem );
		int		UseColosseumBossRate( FLWSUser* pUser, FLItemElem* pSrcItem );


	public:
		enum USE_ACTIVEITEM_RESULT
		{
			ERR_GENERAL_ERROR = 0
			, ERR_SUCCESS_USE										// 성공
			, ERR_USER_INVALID										// 이상한 유저
			, ERR_ITEM_INVALID										// 이상한 아이템
			, ERR_DONOT_USE_STATE									// 적용 가능한 상태가 아님
			, ERR_DONOTUSE_ANOTHER									// 사용 제한된 그룹아이템과 같이 사용 못함
			, ERR_MISMATCH_USE_LEVEL								// 사용 레벨이 안됨
			, ERR_MISMATCH_ENCHANT_LEVEL							// 제련 레벨이 안맞음

			, ERR_SUCCESS_RESET_BIND								// 귀속 해제 성공
			, ERR_NOT_BIND_ITEM										// 귀속 상태 아이템이 아님
			, ERR_DONOT_RESET_BIND									// 귀속 해제 불가 아이템

			, ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT			// 바루나 일반 제련 파괴 보호
			, ERR_SUCCESS_BARUNA_GENERAL_ENCHANT_PROTECT_KEEP		// 바루나 일반 제련 파괴 보호 및 마이너스 보호
			, ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT			// 바루나 속성 제련 파괴 보호
			, ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROTECT_KEEP		// 바루나 속성 제련 파괴 보호 및 마이너스 보호
			, ERR_SUCCESS_BARUNA_ATTRIBUTE_ENCHANT_PROB				// 바루나 속성 제련 성공 확률 증가
			, ERR_SUCCESS_BARUNA_PIERCING_PROTECT					// 바루나 피어싱 문자석 파괴 보호
			, ERR_SUCCESS_BARUNA_PIERCING_PROB						// 바루나 피어싱 성공 확률 증가

			, ERR_SUCCESS_COLOSSEUM_RETRY							// 콜로세움 재시도 어쩌고..
			, ERR_FAIL_COLOSSEUM_RETRY								// 콜로세움 재시도 어쩌고..

			, ERR_SUCCESS_COLOSSEUM_BOSS_RATE						// 콜로세움 진보스 확률 어쩌고..
			, ERR_FAIL_COLOSSEUM_BOSS_RATE							// 콜로세움 진보스 확률 어쩌고..

		};

};


#define		g_xUseActiveItem	FLUseActiveItem::GetInstance()


#endif // __FLUSEACTIVEITEM_H__

