
#include "StdAfx.h"
#include "FLItemAction.h"
#include "FLItemAction_SavePotion.h"
#include "FLItemAction_PartyLevel.h"
#include "FLItemAction_PK_Penalty.h"



//class FLContextSwitch
//{
//public:
//	static FLContextSwitch &	GetInstance();
//
//	FLContextSwitch();				// map사이즈가 컨텐츠 사이즈랑 틀리다면
//
//	bool					Regist( const size_t uStartMainVer, const ContentType eCT, const size_t uCTVer );
//	bool					UnRegist( const size_t uMainVer, const ContentType eCT );
//
//	bool					VersionUp( const size_t uStartMainVer, const ContentType eCT, const size_t uCTVer );	//in 0이아니고 기존 버젼보다 높아야함
//	bool					VersionDown( const size_t uMainVer, const ContentType eCT, const size_t uCTVer );	//in 0이아니고 기존 버젼보다 낮아야함		
//
//	bool					IsActive( const ContentType eCT, const size_t uCTVer );
//
//private:
//	// 맵을 0~지금 버건까지 배열로 만들고 
//	// regist할때 지금 버젼부터 최종 버젼까지 넣는거지
//	// UnRegist할때 빼는거지
//		//eCT(key)  uCTVer(value)
//};





FLItemAction::FLItemAction()
{
	if( _GetContentState( CT_ELLDINS_JAR ) == CS_VER1 )
	{
		// Use
		//registUseFunc( IK1_CHARGED, IK2_POTION, IK3_SAVEPOTION, &FLItemAction_SavePotionStorage::GetInstance() );
		registUseFunc( IK1_ACTIVE, IK2_ELLDINPOTION, IK3_SAVEPOTION, &FLItemAction_SavePotionStorage::GetInstance() );

		//Apply
		registApplyFunc( IK1_ACTIVE, IK2_ELLDINPOTION, IK3_ELLDINKEY, &FLItemAction_SavePotionKey::GetInstance() );
		registApplyFunc( IK1_PASSIVE, IK2_ELLDINPOTION, IK3_FOODELLDIN, &FLItemAction_SavePotionCharge::GetInstance() );

		registApplyFunc( IK1_GENERAL, IK2_FOOD, IK3_INSTANT, &FLItemAction_SavePotionCharge::GetInstance() );
		registApplyFunc( IK1_GENERAL, IK2_FOOD, IK3_COOKING, &FLItemAction_SavePotionCharge::GetInstance() );
		registApplyFunc( IK1_GENERAL, IK2_FOOD, IK3_ICECEARM, &FLItemAction_SavePotionCharge::GetInstance() );
		registApplyFunc( IK1_GENERAL, IK2_FOOD, IK3_PILL, &FLItemAction_SavePotionCharge::GetInstance() );
		registApplyFunc( IK1_GENERAL, IK2_FOOD, IK3_DRINK, &FLItemAction_SavePotionCharge::GetInstance() );
	}

	registUseFunc( IK1_ACTIVE, IK2_SYSTEM, IK3_PARTYSPEEDLEVELUP, &FLItemAction_PartyLevelUp::GetInstance() );

	registUseFunc( IK1_ACTIVE, IK2_ONCE, IK3_PKPENALTYSCROLL_B, &FLItemAction_PK_Penalty::GetInstance() );
}

FLItemAction & FLItemAction::GetInstance()
{
	static FLItemAction inst; 
	return inst;
}

IItem_UseFunc * FLItemAction::getItemUseFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3 )
{
	//#define		SWITCH_CASE( key1, key2, val1, val2, ret )		if( key1 == ##val1 && key1 == ##val2 )		return ret;
	//SWITCH_CASE( TYPE1_POTION, TYPE2_POTION_SAVE, dwMainCategory, dwSubCategory, &FLItemAction_SavePotionStorage::GetInstance() );

	UseFuncMap::iterator it	= m_mapUseFunc.find( FLItemCategoryKey( dwKind1, dwKind2, dwKind3 ) );
	if( it == m_mapUseFunc.end() )
		return NULL;

	return it->second;
}

IItem_ApplyFunc *	FLItemAction::getItemApplyFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3 )
{
	ApplyFuncMap::iterator it	= m_mapApplyFunc.find( FLItemCategoryKey( dwKind1, dwKind2, dwKind3 ) );
	if( it == m_mapApplyFunc.end() )
		return NULL;

	return it->second;
}



//--------------------------------------------------------------------------------------------------------------------------------------------------//



bool FLItemAction::ItemUseHandler( FLWSUser & kUser, CAr & ar )
{
	DWORD dwData		= NULL_ID;
	ar >> dwData;

	const DWORD dwInvenIndex = static_cast< DWORD >( HIWORD( dwData ) );

	//1. 유효성 체크
	FLItemElem* pItemElem	= kUser.m_Inventory.GetAtId( dwInvenIndex );
	if( pItemElem == NULL )
		return true;

	const PT_ITEM_SPEC pSpec= pItemElem->GetProp();
	if( pSpec == NULL )
		return true;

	IItem_UseFunc * p		= getItemUseFunc( pSpec->dwItemKind1, pSpec->dwItemKind2, pSpec->dwItemKind3 );
	if( p == NULL )
		return false;

	//3. 실행
	if( isItemValid_Common( kUser, dwInvenIndex ) == false )
		return true;

	p->Use( kUser, *pItemElem, ar );

	//@@@@@@@@@@FLItemCommonFunction::DoPermanence( kUser, *pItemElem, )		IsValide()?????????????????

	return true;
}

bool FLItemAction::ItemApplyHandler( FLWSUser & kUser, CAr & ar )
{
	DWORD dwSrcObjID	= NULL_ID;
	DWORD dwDestObjID	= NULL_ID;
	ar >> dwSrcObjID >> dwDestObjID;

	FLItemElem* pSrcItem		= kUser.m_Inventory.GetAtId( dwSrcObjID );
	FLItemElem* pDestItem		= kUser.m_Inventory.GetAtId( dwDestObjID );
	if( pSrcItem == NULL || pDestItem == NULL )
		return true;

	const PT_ITEM_SPEC pSrcSpec	= pSrcItem->GetProp();
	const PT_ITEM_SPEC pDestSpec	= pDestItem->GetProp();
	if( pSrcSpec == NULL || pDestSpec == NULL )
		return true;

	IItem_ApplyFunc * p		= getItemApplyFunc( pSrcSpec->dwItemKind1, pSrcSpec->dwItemKind2, pSrcSpec->dwItemKind3 );
	if( p == NULL )
		return false;

	//3. apply
	if( isItemValid_Common( kUser, dwSrcObjID ) == false )
		return true;

	p->Apply( kUser, *pSrcItem, *pDestItem, ar );
	return true;
}

bool FLItemAction::isItemValid_Common( FLWSUser & kUser, const DWORD dwInvenIndex )
{
	// 1. 사용 가능
	if( IsValidObj( &kUser ) == FALSE )
		return false;

	CWorld* pWorld			= kUser.GetWorld();
	if( pWorld == NULL )
		return false;

	if( pWorld->GetID() == WI_WORLD_QUIZ )
		return false;


	// 2. 체크 아이템 
	if( kUser.IsUsableState( dwInvenIndex ) == FALSE )
		return false;

	FLItemElem* pItemElem	= kUser.m_Inventory.GetAtId( dwInvenIndex );
	if( IsUsableItem( pItemElem ) == FALSE )
		return false;

	if( IsUsableItem( pItemElem ) == FALSE )
		return false;

	if( pItemElem->m_nItemNum <= 0 )
		return false;

	// 3. 
	PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
	if( pItemSpec == NULL )
		return false;

	if( pItemElem->CanUseLevel( static_cast<CMover*>( &kUser ) ) == false )
	{
		if( pItemSpec->dwParts != NULL_ID )
		{
			kUser.AddDefinedText( TID_GAME_REQLEVEL, "\"%d\"", pItemElem->GetMinLimitLevel() );
		}
		else
		{
			TCHAR szMessage[ MAX_PATH ] = { 0 };
			FLSPrintf( szMessage, _countof( szMessage ), "%d - %d", pItemSpec->nMinLimitLevel, pItemSpec->nMaxLimitLevel );
			kUser.AddDefinedText( TID_GAME_EXP_NOTUSELEVEL, "\"%s\"", szMessage );
		}
		return false;
	}

	// 기간 만료
	if( pItemSpec && pItemSpec->dwParts == NULL_ID && pItemElem->IsFlag( FLItemElem::expired ) )
	{
		kUser.AddDefinedText( ( pItemSpec->dwItemKind3 == IK3_EGG ) ?		TID_GAME_PET_DEAD : TID_GAME_ITEM_EXPIRED );
		return false;
	}

	// 봉인 해제 타입 체크 추가..............

	// 길드대전맵에서 선수만 사용
	if( pWorld->GetID() == WI_WORLD_GUILDWAR && kUser.m_nGuildCombatState == 0 )
	{
		kUser.AddText( prj.GetText( TID_GAME_GUILDCOMBAT_STANDS_NOTUSEITEM ) );	//"수정해야함 : 관중석에서는 아이템을 사용할수 없습니다" );
		return false;
	}

	if( kUser.GetAdjParam( DST_CHRSTATE ) & CHS_SETSTONE )
		return false;

	return true;
}

void FLItemAction::registUseFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3, IItem_UseFunc * p )
{
	RELEASE_ASSERT( m_mapUseFunc.insert( make_pair( FLItemCategoryKey( dwKind1, dwKind2, dwKind3 ), p ) ).second == true );
}

void FLItemAction::registApplyFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3, IItem_ApplyFunc * p )
{
	RELEASE_ASSERT( m_mapApplyFunc.insert( make_pair( FLItemCategoryKey( dwKind1, dwKind2, dwKind3 ), p ) ).second == true );
}



bool FLItemCommonFunction::HasSameGroup( const int nItemGroup, const int nUseLimitGroup, CItemContainer< FLItemElem > & kInven, CPocketController & kPocket )
{
	if( nItemGroup <= 0 || nUseLimitGroup <= 0 )	 //그룹 업음.
		return false;

	for( DWORD dwObjID = 0; dwObjID < kInven.GetMax(); ++dwObjID )
	{
		FLItemElem* pItemElem	= kInven.GetAtId( dwObjID );

		if( IsUsableItem( pItemElem ) == FALSE )
			continue;

		PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
		if( pItemSpec == NULL )
			continue;

		if( pItemElem->IsExpiring() != FALSE || pItemElem->IsFlag( FLItemElem::expired ) != FALSE )
			continue;

		if( nUseLimitGroup == pItemSpec->nItemGroup && ( pItemElem->m_dwKeepTime != 0 ) )
			return true;
		//if( nItemGroup == pItemSpec->nItemGroup 

	}

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( kPocket.IsAvailable( nPocket, FALSE ) == FALSE )
			continue;

		for( DWORD dwObjID = 0; dwObjID < kPocket.m_kPocket[nPocket].GetMax(); ++dwObjID )
		{
			FLItemElem* pItemElem	= kPocket.m_kPocket[nPocket].GetAtId( dwObjID );

			if( IsUsableItem( pItemElem ) == FALSE )
				continue;

			PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
			if( pItemSpec == NULL )
				continue;

			if( pItemElem->IsExpiring() != FALSE || pItemElem->IsFlag( FLItemElem::expired ) != FALSE )
				continue;

			if( nUseLimitGroup == pItemSpec->nItemGroup && ( pItemElem->m_dwKeepTime != 0 ) )
				return true;
		}

	}

	return false;
}
