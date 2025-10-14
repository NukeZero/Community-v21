#ifndef __FLSTDCONTAINER_H__
#define __FLSTDCONTAINER_H__


#include "../_CommonDefine/FLDeleter.h"	

//neodeath VC2003에선 TTP가 안먹음.. 되는게 뭐임?

//template < class _Ty, template < typename > class _Deleter, class _Ax = std::allocator< _Ty> >
//class FLVector : public std::vector< _Ty, _Ax >
//{
//public:
//	FLVector() {}
//	virtual ~FLVector() 
//	{ 
//		_Deleter< std::vector< _Ty, _Ax > >() ( *this ); 
//	}
//};


#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>


template < class _Ty, class _Deleter, class _Ax = std::allocator< _Ty > >
class FLVector: public std::vector< _Ty, _Ax >
{
public:
	FLVector() {}
	virtual ~FLVector() 	{ 
		_Deleter()( *this ); 
	}
};


template < class _Ptr, class _Ax = std::allocator< _Ptr > >
class FLVector_autoDelete : public FLVector< _Ptr, SeqDeleter< std::vector< _Ptr, _Ax > >, _Ax >
{
	BOOST_STATIC_ASSERT( ( boost::is_pointer< _Ptr >::value == true ) );
};

template < class _Kty, class _Ty, class _Deleter, class _Pr = std::less< _Ty >, class _Ax = std::allocator< std::pair< const _Kty, _Ty > > >
class FLMap : public std::map< _Kty, _Ty, _Pr, _Ax >
{
public:
	FLMap() {}
	virtual ~FLMap() 	{ 
		_Deleter()( *this ); 
	}
};


template < class _Kty, class _Ty, class _Pr = std::less< _Kty >, class _Ax = std::allocator< std::pair< const _Kty, _Ty > > >
class FLMap_DeleteSecond : public FLMap< _Kty, _Ty, AssDeleter< std::map< _Kty, _Ty, _Pr, _Ax > >, _Pr, _Ax >
{
	BOOST_STATIC_ASSERT( ( boost::is_pointer< _Ty >::value == true ) );
};


//// Less_(맴버 면수)   규칙으로 함수 생성  ex)  Less_nGold
//#define		Define_ScalarMemberValCompare(_class_type, _member_name)	bool Less_##_member_name( const _class_type & a, const _class_type & b )			\
//	{																				\
//		return ( a._member_name < b._member_name );										\
//	}																				\
//	bool Greater_##_member( const _class_type & a, const _class_type & b )			\
//	{																				\
//		return ( a._member_name > b._member_name );										\
//	}																		
//
//#define		Define_StringMemberValCompare(_class_type, _member_name)	bool Less_##_member_name( const _class_type & a, const _class_type & b )			\
//	{																				\
//		return ( 0 > ::strcmp( a._member_name, b._member_name ) );						\
//	}																				\
//	bool Greater_##_member_name( const _class_type & a, const _class_type & b )		\
//	{																				\
//		return ( 0 < ::strcmp( a._member_name, b._member_name ) );						\
//	}		

// Less_(맴버 면수)   규칙으로 함수 생성  ex)  Less_nGold
//#define		Define_ScalarMemberValCompare(ret, _parrent_type, _member_name)\
//ret Less_##_member_name( const _parrent_type & a, const _parrent_type & b )			\
//{																				\
//	return ( a._member_name < b._member_name );										\
//}																				\
//ret Greater_##_member_name( const _parrent_type & a, const _parrent_type & b )			\
//{																				\
//	return ( a._member_name > b._member_name );										\
//}																		
//
//
//#define		Define_StringMemberValCompare(ret, _parrent_type, _member_name)\
//ret Less_##_member_name( const _parrent_type & a, const _parrent_type & b )			\
//{																				\
//	return ( 0 > ::strcmp( a._member_name, b._member_name ) );						\
//}																				\
//ret Greater_##_member_name( const _parrent_type & a, const _parrent_type & b )		\
//{																				\
//	return ( 0 < ::strcmp( a._member_name, b._member_name ) );						\
//}		


//template <typename T>      // template paramter (데이터형 파라미터)
//inline bool less_fun( const T & a, const T & b )
//{
//	return a < b;
//}
//
//template <typename T>      // template paramter (데이터형 파라미터)
//inline bool greater_fun( const T & a, const T & b )
//{
//	return a > b;
//}


#endif //__FLSTDCONTAINER_H__