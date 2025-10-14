#pragma once

class IItem_UseFunc;
class IItem_ApplyFunc;


struct FLItemCategoryKey
{
	FLItemCategoryKey( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3 ) : m_dwKind1( dwKind1 ), m_dwKind2( dwKind2 ), m_dwKind3( dwKind3 )
	{}

	bool					operator == ( const FLItemCategoryKey & r ) const  {		return ( m_dwKind1 == r.m_dwKind1 && m_dwKind2 == r.m_dwKind2 && m_dwKind3 == r.m_dwKind3 );	}
	bool					operator < ( const FLItemCategoryKey & r ) const  
	{	
		return ( m_dwKind1 != r.m_dwKind1 )	
			? ( m_dwKind1 < r.m_dwKind1 )
			: ( m_dwKind2 != r.m_dwKind2 )
				? ( m_dwKind2 < r.m_dwKind2 )
				: ( m_dwKind3 < r.m_dwKind3 );
	}


private:
	const DWORD				m_dwKind1;
	const DWORD				m_dwKind2;
	const DWORD				m_dwKind3;

//	FLItemCategoryKey( const DWORD dwMainCategory, const DWORD dwSubCategory ) : dwMain( dwMainCategory ), dwSub( dwSubCategory ) {}
//
//	bool operator < ( const FLItemCategoryKey & r ) const		{			return ( u64Value < r.u64Value );			}
//	bool operator == ( const FLItemCategoryKey & r ) const		{			return ( u64Value == r.u64Value );			}
//
//public:
//	union
//	{
//		UINT64				u64Value;
//
//		struct
//		{
//			DWORD				dwMain;
//			DWORD				dwSub;
//		};
//	};
};


class FLItemAction
{
public:
	FLItemAction();

	static FLItemAction &	GetInstance();

	bool					ItemUseHandler( FLWSUser & kUser, CAr & ar );			
	bool					ItemApplyHandler( FLWSUser & kUser, CAr & ar );	

private:
	bool					isItemValid_Common( FLWSUser & kUser, const DWORD dwInvenIndex );

	//IItem_UseFunc *			getItemUseFunc( const DWORD dwMainCategory, const DWORD dwSubCategory );
	//IItem_ApplyFunc *		getItemApplyFunc( const DWORD dwMainCategory, const DWORD dwSubCategory );
	//void					registUseFunc( const DWORD dwMainCategory, const DWORD dwSubCategory, IItem_UseFunc * p );
	//void					registApplyFunc( const DWORD dwMainCategory, const DWORD dwSubCategory, IItem_ApplyFunc * p );

	IItem_UseFunc *			getItemUseFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3 );
	IItem_ApplyFunc *		getItemApplyFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3 );
	void					registUseFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3, IItem_UseFunc * p );
	void					registApplyFunc( const DWORD dwKind1, const DWORD dwKind2, const DWORD dwKind3, IItem_ApplyFunc * p );

private:
	typedef					std::map< FLItemCategoryKey, IItem_UseFunc * >	UseFuncMap;
	typedef					std::map< FLItemCategoryKey, IItem_ApplyFunc * >	ApplyFuncMap;

	UseFuncMap				m_mapUseFunc;
	ApplyFuncMap			m_mapApplyFunc;
};


class FLItemCommonFunction
{
public:
	static bool				HasSameGroup( const int nItemGroup, const int nUseLimitGroup, CItemContainer< FLItemElem > & kInven, CPocketController & kPocket );


	//@@@@@@@@@@@@ Use에 일반화 시켜야함 : Item 객체를 받지 않고 InvenIndex를 받는 것을 생각해봄.
	//static void				DoPermanence( FLWSUser & kUser, FLItemElem & io_kUseItem, const BOOL bPermanence, const int nNum )
	//{
	//	if( bPermanence != TRUE )
	//	{
	//		g_DPSrvr.PutItemLog( &kUser, "*", "remove charge potion", &io_kUseItem, nNum );
	//		kUser.RemoveItem( io_kUseItem.m_dwObjId, nNum );
	//	}
	//}
};
