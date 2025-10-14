#ifndef __FLRING_H__
#define __FLRING_H__


#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/noncopyable.hpp>

//neodeath comment:
// const_iterator 만들어야함.
// 단점 : 한 컨테이너에 동일한 객체를 한 객체만 담을 수 있다.
//		: static_cast의 남용 ( 기준 노드[m_stHeadNode]가 FLNode Type이여서 발생 )
// 장점 : O(1)의 복잡도로 Pop 가능.

template < class _Type >
class FLRing : boost::noncopyable			//컨테이너 복사 방지
{
	BOOST_STATIC_ASSERT( ( boost::is_pointer< _Type >::value == false ) );

public:
	struct FLNode : boost::noncopyable		//삽입된 노드의 복사 방지
	{
		//BOOST_STATIC_ASSERT( ( boost::is_base_and_derived< FLRing< _Type >::FLNode, _Type >::value == true ) );
		friend				FLRing< _Type >;
		friend				std::iterator;
	public:
		FLNode();
		virtual ~FLNode();

	public:
		bool				Unlink();
	
	//private:
	//	void *				operator new( size_t )			{}
	//	void				operator delete( void *, size_t ) {}

	private:
		FLNode *			m_pRPrev;
		FLNode *			m_pRNext;
	};

	//typedef					FLNode		_FLNode;

public:
	struct iterator			
	{
	public:
		iterator() : m_pNode( NULL )		{}
		iterator( FLNode * pNode ) : m_pNode( pNode )		{}

		_Type & operator *() const 		{		return * static_cast< _Type * >( m_pNode );		}
		_Type * operator->() const		{		return static_cast< _Type * >( m_pNode );		}

		iterator & operator++()			{
			m_pNode				= m_pNode->m_pRNext;
			return ( *this );
		}

		iterator operator++( int )
		{
			iterator tmp		= *this;
			++*this;
			return tmp;
		}

		iterator & operator--()
		{
			m_pNode				= m_pNode->m_pRPrev;
			return ( * this );
		}

		bool operator == ( const iterator & r ) const		{		return ( m_pNode == r.m_pNode );	}
		bool operator != ( const iterator & r ) const 		{		return ( m_pNode != r.m_pNode );	}

		// return node pointer
		FLNode * _MyNode() const							{		return m_pNode;						}
		// return node pointer	//@ 보완 해야함 : m_stHeadNode( == End() )로 호출했을 경우 낭패 
		_Type * _MyType() const								{		return static_cast< _Type * >( m_pNode );		}

	private:
		 FLNode *				m_pNode;
	};

public:
	FLRing();			
	virtual ~FLRing();

	bool					InsertBack( iterator it, _Type * pDest );
	bool					InsertForward( iterator it, _Type * pDest );

	bool					UnlinkNode( _Type * pObj );

	iterator				PopHead();				
	iterator				PopTail();

	iterator				Begin();
	iterator				Tail();	
	iterator				End();

	iterator				Erase( iterator it );

	void					Clear();

private:
	bool					insertBack( FLNode * pSrc, FLNode * pDest );
	bool					insertForward( FLNode * pSrc, FLNode * pDest );

private:
	FLNode					m_stHeadNode;
};

#endif // __FLRING_H__