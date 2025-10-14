#ifndef __FLUSEEFFECTITEM_H__
#define __FLUSEEFFECTITEM_H__


class FLUseEffectItem
{
	public:
		static	FLUseEffectItem*	GetInstance();

		FLUseEffectItem();
		~FLUseEffectItem();

		int		OnDoUseEffectItem( FLWSUser* pUser, FLItemElem* pItemElem );

	private:
		int		DoUseEffectItem( FLWSUser* pUSer, FLItemElem* pItemElem );
		int		CheckEffectItem( FLWSUser* pUser, PT_ITEM_SPEC pItemProp );
		int		GetLimitCountUseMax( PT_ITEM_SPEC pItemProp );


	public:
		enum USE_EFFECTITEM_RESULT
		{
			ERR_GENERAL_ERROR = 0
			, ERR_SUCCESS_USE							// 성공
			, ERR_USER_INVALID							// 이상한 유저
			, ERR_ITEM_INVALID							// 이상한 아이템
			, ERR_DONOT_USE_STATE						// 적용 가능한 상태가 아님
			, ERR_ALEADY_USE							// 이미 동일종류 아이템 사용중
			, ERR_DONOTUSE_ANOTHER						// 사용 제한된 그룹아이템과 같이 사용 못함
			, ERR_MISMATCH_USE_LEVEL					// 사용 레벨이 안됨
			, ERR_MAX_OVER								// 최대 사용 개수 초과
			, ERR_NOT_PK_ENABLE_CHANNEL					// PK 가능한 채널이 아님
			, ERR_NOT_CHAO_PLAYER						// 카오 플레이어가 아님( PK 수치가 없음 )

		};

};


#define		g_xUseEffectItem	FLUseEffectItem::GetInstance()


#endif // __FLUSEEFFECTITEM_H__

