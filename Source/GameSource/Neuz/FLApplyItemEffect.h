#ifndef __FLAPPLYITEMEFFECT_H__
#define __FLAPPLYITEMEFFECT_H__


class FLApplyItemEffect
{
	public:
		static FLApplyItemEffect* GetInstance();

		FLApplyItemEffect();
		~FLApplyItemEffect();


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		BOOL	ApplyParameter( CMover* pSrc, CMover* pDest, PT_ITEM_SPEC pItemProp, BOOL bSend = TRUE, int nDamage = 0 );

		//////////////////////////////////////////////////////////////////////////
};


#define		g_xApplyItemEffect		FLApplyItemEffect::GetInstance()


#endif // __FLAPPLYITEMEFFECT_H__

