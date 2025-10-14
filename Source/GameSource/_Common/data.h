#ifndef __DATA_H
#define __DATA_H

#pragma once


template <class T> class CStack
{
protected:
	T*	m_pBlock;
	int	m_nSize;
	int	m_nGrowSize;
	int	m_nTop;

public:
	CStack( int nSize = 1024, int nGrowSize = 1024 );
	
	virtual	~CStack() { SAFE_DELETE_ARRAY( m_pBlock );	}

	void	SetSize( int nSize, int nGrowSize = 1024 );
	int		GetSize() { return m_nSize; }
	int		GetCount( void ) { return m_nTop; }
	void	Push( T data );
	T	    Pop( void );
	T	    Peek( void );
	void	Clear() { m_nTop = 0; }
	BOOL	IsFull( void )	{ return ( m_nTop >= m_nSize ); }
	BOOL	IsEmpty( void )	{ return ( m_nTop == 0 ); }
};
template <class T> inline CStack<T>::CStack( int nSize, int nGrowSize )
{
	m_nTop = 0;
	m_nGrowSize		= nGrowSize;
	m_pBlock	= new T[m_nSize = nSize];
}

template <class T> inline void CStack<T>::SetSize( int nSize, int nGrowSize )
{
	m_nTop	= 0;
	SAFE_DELETE_ARRAY( m_pBlock );
	m_nGrowSize		= nGrowSize;
	m_pBlock	= new T[m_nSize = nSize];
}

template <class T> inline void CStack<T>::Push( T data )
{
//	FLASSERT( m_nTop < m_nSize );
	if( m_nTop >= m_nSize )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
		return;
	}
	m_pBlock[m_nTop++]	= data;

	if( IsFull() && m_nGrowSize > 0 )
	{
		T* pOld	= m_pBlock;
		m_pBlock = new T[m_nSize + m_nGrowSize];
		memcpy( m_pBlock, pOld, m_nTop * sizeof(T) );
		SAFE_DELETE_ARRAY( pOld );
		m_nSize		+= m_nGrowSize;
	}
}

template <class T> inline T CStack<T>::Pop( void )
{
//	FLASSERT( m_nTop );
	if( m_nTop < 1 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
	}
	return m_pBlock[--m_nTop];
}

template <class T> inline T CStack<T>::Peek( void )
{
//	FLASSERT( m_nTop );
	if( m_nTop < 1 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
	}
	return m_pBlock[m_nTop - 1];
}


typedef CStack<DWORD>  CDWordStack; 
typedef CStack<WORD>   CWordStack; 
typedef CStack<CHAR>   CCharStack; 
typedef CStack<UINT>   CUIntStack; 
typedef CStack<LONG>   CLongStack; 
typedef CStack<short>  CShortStack; 
typedef CStack<int>    CIntStack; 

template <class T> class CQueue 
{
	int m_nSize;
	int m_nHead;
	int	m_nTail;

	T* m_pData;
public:
	CQueue(int nSize = 1024);
	~CQueue() { SAFE_DELETE_ARRAY( m_pData ); }
	void SetSize(int nSize);
	int  GetSize() { return m_nSize; }
	int GetCount(void)
	{ 
		if (m_nHead >= m_nTail)
			return (m_nHead - m_nTail);
		else
			return (m_nSize - 1 - m_nTail + m_nHead);
	}
	void Clear() { m_nHead = 0; m_nTail = 0; } 
	void Push(T data);
	T Pop();
	T GetPop();

private:
	void IncreaseIndex(int	&nIndex)
	{
		nIndex++;
		if (nIndex >= m_nSize)
			nIndex = 0;
	}
	BOOL IsFull(void)
	{
		if (GetCount() == m_nSize-1)
			return TRUE;
		return FALSE;
	}
	BOOL IsEmpty(void)
	{
		if (m_nHead == m_nTail)
			return TRUE;
		return FALSE;
	}
};

template <class T> inline CQueue<T>::CQueue(int nSize) 
{
	m_nHead = 0;
	m_nTail = 0;
	m_pData = new T[m_nSize = nSize];
}

template <class T> inline void CQueue<T>::SetSize(int nSize) 
{
	//m_nTop = 0;
	SAFE_DELETE_ARRAY( m_pData );
	m_pData = new T[m_nSize = nSize];
}

template <class T> inline void CQueue<T>::Push(T data) 
{
	if (IsFull())
	{
		T* pOld = m_pData;
		m_pData = new T[m_nSize + 1024];
		if (m_nHead >= m_nTail)
		{
			memcpy(m_pData, &pOld[m_nTail], (m_nHead-m_nTail)*sizeof(T));
			m_nHead = m_nHead-m_nTail;
			m_nTail = 0;
		}
		else
		{
			memcpy(m_pData, &pOld[m_nTail], (m_nSize-1-m_nTail)*sizeof(T));
			memcpy(&m_pData[m_nSize-1-m_nTail], pOld, m_nHead*sizeof(T));
			m_nHead = (m_nSize-1-m_nTail)+m_nHead;
			m_nTail = 0;
		}
		SAFE_DELETE_ARRAY( pOld );
		m_nSize += 1024;
	}
	m_pData[m_nHead] = data;
	IncreaseIndex(m_nHead);
}

template <class T> inline T CQueue<T>::Pop() 
{
	T *p;
//	FLASSERT(!IsEmpty());
	if( IsEmpty() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
	}
	p =  m_pData[m_nTail];
	IncreaseIndex(m_nTail);
	return p;
}

template <class T> inline T CQueue<T>::GetPop() 
{
//	FLASSERT(!IsEmpty());
	if( IsEmpty() )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "" ) );
	}
	return m_pData[m_nTail];
}

#include <list>

template <class T>
class CIndexArray
{
private:

	typedef	std::list< T >	DATAS;

	DATAS	m_listData;

	T**		m_pIndexData;
	size_t	m_nIndexCount;

	int		m_nEndIndex;

public:

	CIndexArray();
	virtual	~CIndexArray();

	int		GetSize()	const;

	void	SetAt( int nIndex, const T* pData );
	T*		GetAt( int nIndex )	const;

	void	Optimize();
	void	RemoveAll();

private:

	void	Grow( size_t nCount );

};

template <class T>
inline CIndexArray<T>::CIndexArray( )
{
	m_pIndexData = NULL;
	m_nIndexCount = 0;

	m_nEndIndex = -1;
}

template <class T>
inline CIndexArray<T>::~CIndexArray( )
{
	SAFE_DELETE_ARRAY( m_pIndexData );
}

template <class T>
inline int CIndexArray<T>::GetSize()	const
{
	// 호환을 위해 int로 리턴
	if( m_nIndexCount <= INT_MAX )
	{
		return static_cast< int >( m_nIndexCount );
	}

	FLASSERT( NULL );
	return 0;
}

template <class T>
void CIndexArray<T>::Grow( size_t nCount )
{
	FLASSERT( nCount <= INT_MAX );
	if( nCount <= 0 )
	{
		nCount = 100;
	}

	T** pTemp = new T*[nCount];
	memset( pTemp, 0, sizeof( T* ) * nCount );

	if( m_pIndexData != NULL )
	{
		memcpy( pTemp, m_pIndexData, sizeof( T* ) * (m_nEndIndex+1) );
	}

	SAFE_DELETE_ARRAY( m_pIndexData );
	m_pIndexData = pTemp;
	pTemp = NULL;
	m_nIndexCount = nCount;
}

template <class T>
inline T* CIndexArray<T>::GetAt( int nIndex )	const
{
	if( 0 <= nIndex && nIndex < GetSize() )
	{
		return m_pIndexData[nIndex];
	}

	return NULL;
}

template <class T>
void CIndexArray<T>::SetAt( int nIndex, const T* pData )
{
	FLASSERT( nIndex >= 0 );
	if( nIndex >= 0 )
	{
		if( GetSize() <= 0 || nIndex >= GetSize() )
		{
			Grow( nIndex*2 );	// 두배 확장
		}

		if( m_pIndexData[nIndex] == NULL )
		{
			m_listData.push_back( *pData );
			m_pIndexData[nIndex] = &(m_listData.back());
		}
		else
		{
			*m_pIndexData[nIndex] = *pData;
		}

		if( nIndex > m_nEndIndex )
		{
			m_nEndIndex = nIndex;
		}
	}
}

template <class T>
void CIndexArray<T>::Optimize()
{
	if( m_nEndIndex >= 0 && GetSize() > (m_nEndIndex+1) )
	{
		T** pTemp = new T*[m_nEndIndex+1];

		// 복사(m_nEndIndex를 0으로 초기화 하기 때문에 비어 있어도 1개는 복사된다.)
		memcpy( pTemp, m_pIndexData, sizeof( T* ) * (m_nEndIndex+1) );

		SAFE_DELETE_ARRAY( m_pIndexData );
		m_pIndexData = pTemp;
		m_nIndexCount = m_nEndIndex+1;
	}
}

template <class T>
inline void CIndexArray<T>::RemoveAll()
{
	SAFE_DELETE_ARRAY( m_pIndexData );
	m_listData.clear();

	m_nEndIndex = -1;
	m_nIndexCount = 0;
}

#if defined(__WORLDSERVER ) || defined(__CLIENT)
struct NaviPoint //	네비게이션에 클릭으로 위치를 표시하는 기능을 위한 구조체
{
	D3DXVECTOR3 Pos;		//	이 위치는 네이게이터상의 위치가 아닌 월드상의 위치임을 강조한다.
	short		Time;			//	포인트의 남아있는 시간
	DWORD		objid;		
	std::string      Name;
	NaviPoint()
	{
		Pos = D3DXVECTOR3(0.f, 0.f, 0.f);
		objid = 0xffffffff;
		Time = 0;
	}
	
	NaviPoint& operator=( const NaviPoint& src )
	{
		Pos   = src.Pos;
		objid = src.objid;
		Time  = src.Time;
		Name  = src.Name;

		return *this;
	}
}; 

typedef std::vector<NaviPoint> V_NaviPoint;

#endif // defined(__WORLDSERVER ) || defined(__CLIENT)

namespace SAI79
{
	// ************************************
	// * 타입 없음.	: 0
	// * 불   속성	: 1
	// * 물   속성	: 2
	// * 전기 속성	: 3
	// * 바람 속성	: 4
	// * 땅   속성	: 5
	// * 이   속성 타입을 ePropType로 설정
	// ************************************
	enum	ePropType 	{ NO_PROP = 0, FIRE, WATER, ELECTRICITY, WIND, EARTH, END_PROP };

	enum	eRelationType	{ RELATION_NONE = 0, RELATION_EQUAL, RELATION_ATK_DOMINANT, RELATION_DEF_DOMINANT, RELATION_MAX	};

	const BYTE RelationTable[ END_PROP ][ END_PROP ] = 
	{
		{ RELATION_NONE, RELATION_NONE, RELATION_NONE, RELATION_NONE, RELATION_NONE, RELATION_NONE },					// 속성 없음
		{ RELATION_NONE, RELATION_EQUAL, RELATION_DEF_DOMINANT, RELATION_NONE, RELATION_ATK_DOMINANT, RELATION_NONE },	// 불   속성
		{ RELATION_NONE, RELATION_ATK_DOMINANT, RELATION_EQUAL, RELATION_DEF_DOMINANT, RELATION_NONE, RELATION_NONE },	// 물   속성
		{ RELATION_NONE, RELATION_NONE, RELATION_ATK_DOMINANT, RELATION_EQUAL, RELATION_NONE, RELATION_DEF_DOMINANT },	// 전기 속성
		{ RELATION_NONE, RELATION_DEF_DOMINANT, RELATION_NONE, RELATION_NONE, RELATION_EQUAL, RELATION_ATK_DOMINANT },	// 바람 속성
		{ RELATION_NONE, RELATION_NONE, RELATION_NONE, RELATION_ATK_DOMINANT, RELATION_DEF_DOMINANT, RELATION_EQUAL }	// 땅   속성
	};

	const BYTE	GetRelationType( const BYTE byAtkPropType, const BYTE byDefPropType );

	inline const BYTE	GetRelationType( const BYTE byAtkPropType, const BYTE byDefPropType )
	{
		if( byAtkPropType >= END_PROP || byDefPropType >= END_PROP )
		{
			return RELATION_MAX;
		}

		const BYTE byRelationType	= RelationTable[ byAtkPropType ][ byDefPropType ];
		
		return byRelationType;
	}
} // namespace SAI79

#endif
