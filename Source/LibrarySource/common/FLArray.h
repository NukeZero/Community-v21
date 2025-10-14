#pragma once


#define RELEASE_ASSERT( x )		do{ if( x == false ) RELEASE_BREAK; }while(0)

template < class _Type, UINT _Size >
class FLStaticArray
{
public:
	_Type &		operator [] ( const size_t i )
	{
		RELEASE_ASSERT( i < _Size );
		return m_arr[ i ];
	}

	//void					Memset( const int c )
	//{
	//	::memset( m_arr, c, sizeof( m_arr ) );
	//}

	inline size_t			size() const
	{
		return _Size;
	}

private:
	_Type					m_arr[ _Size ];
};

//----------------------------------------------------------------------------//

template < class _Type >
class FLHeepArray
{
public:
	FLHeepArray( const size_t n ) : m_size( n ), m_parr( new _Type[ n ] )
	{
		RELEASE_ASSERT( n > 0 );
	}

	~FLHeepArray()
	{
		delete [] m_parr;
	}

	_Type &		operator [] ( const size_t i )
	{
		RELEASE_ASSERT( i < m_size );
		return m_parr[ i ];
	}

	//void					Memset( const int c )
	//{
	//	::memset( m_parr, c, sizeof( _Type ) * m_size );
	//}

	inline size_t			size() const
	{
		return m_size;
	}

private:
	_Type *					m_parr;
	const size_t			m_size;
};


//void *		memset_s( void * dst, const int c, const size_t dst_size )
//{
//	RELEASE_ASSERT
//}