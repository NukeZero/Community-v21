#ifndef __SKILLINFLUENCE_H__
#define	__SKILLINFLUENCE_H__

#define	BUFF_ITEM	0
#define	BUFF_SKILL	1
#define	BUFF_PET	2
#define	BUFF_ITEM2	3	// 110217 더이상 사용하지 않음. item spec 의 bAbsoluteTime 사용.
#define	BUFF_EQUIP	4	// 장착류 아이템에 특정 DST(DST_GIFTBOX)에 대해 버프 아이콘만 출력
#define	BUFF_NULL_ID	(WORD)0xFFFF
#define	MAX_SKILLINFLUENCE	64

#ifdef SKILL_BUFF21
#define	MAX_SKILLBUFF_COUNT	21
#else
#define	MAX_SKILLBUFF_COUNT	14
#endif	// SKILL_BUFF21


#endif	// __SKILLINFLUENCE_H__