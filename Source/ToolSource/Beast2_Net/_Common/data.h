#ifndef __DATA_H
#define __DATA_H

#pragma once

const DWORD NULL_ID	= 0xffffffff;

#define MAX_CACHE_SERVER	8
#define MAX_USER	4096
#define INVALID_CACHE_SERVER	(UINT)0xffffffff

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
//	ASSERT( m_nTop < m_nSize );
	if( m_nTop >= m_nSize )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
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
//	ASSERT( m_nTop );
	if( m_nTop < 1 )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
	}
	return m_pBlock[--m_nTop];
}

template <class T> inline T CStack<T>::Peek( void )
{
//	ASSERT( m_nTop );
	if( m_nTop < 1 )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
	}
	return m_pBlock[m_nTop - 1];
}


/*
template <class T> class CStack 
{
	int m_nSize;
	int m_nTop;
	T* m_pData;
public:
	CStack(int nSize = 1024);
	~CStack() { delete m_pData; }
	void SetSize(int nSize);
	int  GetSize() { return m_nSize; }
	int GetTop() { return m_nTop; }
	int GetCount(void) { return m_nTop; }
	void Push(T data);
	void Clear() { m_nTop = 0; } 
	T Pop();
	T GetPop(); 
};

template <class T> inline CStack<T>::CStack(int nSize) 
{
	m_nTop = 0;
	m_pData = new T[m_nSize = nSize];
}
template <class T> inline void CStack<T>::SetSize(int nSize) 
{
	m_nTop = 0;
	delete m_pData;
	m_pData = new T[m_nSize = nSize];
}
template <class T> inline void CStack<T>::Push(T data) 
{
	ASSERT(m_nTop != m_nSize);
	m_pData[m_nTop++] = data;
	if(m_nTop >= m_nSize)
	{
		T* pOld = m_pData;
		m_pData = new T[m_nSize + 1024];
		memcpy(m_pData,pOld,m_nTop*sizeof(T));
		delete pOld;
		m_nSize += 1024;
	}
};
template <class T> inline T CStack<T>::Pop() 
{
	ASSERT(m_nTop);
	return m_pData[--m_nTop];
};
template <class T> inline T CStack<T>::GetPop() 
{
	ASSERT(m_nTop);
	return m_pData[m_nTop-1];
};
*/

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
	~CQueue() { delete m_pData; }
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
	m_nTop = 0;
	delete m_pData;
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
		delete pOld;
		m_nSize += 1024;
	}
	m_pData[m_nHead] = data;
	IncreaseIndex(m_nHead);
}

template <class T> inline T CQueue<T>::Pop() 
{
	T *p;
//	ASSERT(!IsEmpty());
	if( IsEmpty() )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
	}
	p =  m_pData[m_nTail];
	IncreaseIndex(m_nTail);
	return p;
}

template <class T> inline T CQueue<T>::GetPop() 
{
//	ASSERT(!IsEmpty());
	if( IsEmpty() )
	{
		WriteLog( "%s, %d", __FILE__, __LINE__ );
	}
	return m_pData[m_nTail];
}
/*
template <class T> class CMapCustom : public CObject
{
	T			*m_pData;
	int		m_nBlockSize;
	int		m_nSize;
protected:
	void	SetAtGrow(int idx, T obj);

public:   
	CMapCustom(int nSize = 10240, int nBlockSize = 10240);
	virtual ~CMapCustom();
	void	SetSize(int nSize);
	int		GetSize(void) { return m_nSize; }
	BOOL	RemoveKey(int	idx);
	BOOL	Lookup(int idx, T &obj)
		{
			if(idx == NULL_ID || (OBJID)idx >= (OBJID)m_nSize)
			{
				obj = NULL;
				return FALSE;
			}
			obj = m_pData[idx];
			if (obj) 
				return TRUE;
			return FALSE;
		}
	T operator [] (int idx)
		{
			if (idx == NULL_ID || (OBJID)idx >= (OBJID)m_nSize)
				return NULL;
 			ASSERT((OBJID)idx < (OBJID)m_nSize);
			return m_pData[idx]; 
		}
	void	SetAt(int idx, T obj)
		{	ASSERT(idx < m_nSize); m_pData[idx] = obj; }
	void	RemoveAll(void);
};

template <class T> inline CMapCustom<T>::CMapCustom(int nSize, int nBlockSize) 
{
	m_pData = new T[m_nSize = nSize];
	m_nBlockSize = nBlockSize;
	ZeroMemory(m_pData, sizeof(T)*m_nSize);
}

template <class T> inline void CMapCustom<T>::SetSize(int nSize)
{
	delete m_pData;
	m_pData = new T[m_nSize = nSize];
	ZeroMemory(m_pData, sizeof(T)*m_nSize);
}

template <class T> inline CMapCustom<T>::~CMapCustom()
{
	delete m_pData;
}

template <class T> inline BOOL CMapCustom<T>::RemoveKey(int	nIdx)
{
	if (nIdx < m_nSize && m_pData[nIdx])
	{
		m_pData[nIdx] = NULL;
		return TRUE;
	}
	return FALSE;
}

template <class T> inline void CMapCustom<T>::SetAtGrow(int nIdx, T obj)
{
	T			*pNewData;
	int		nNewSize;

	if (nIdx >= m_nSize)
	{
		nNewSize = (nIdx/m_nBlockSize+1)*m_nBlockSize;
		pNewData = new T[nNewSize];
		ZeroMemory(pNewData, sizeof(T)*nNewSize);
		CopyMemory(pNewData, m_pData, sizeof(T)*m_nSize);
		delete m_pData;
		m_nSize = nNewSize;
		m_pData = pNewData;
	}
	m_pData[nIdx] = obj;
}

template <class T> inline void CMapCustom<T>::RemoveAll(void)
{
	ZeroMemory(m_pData, sizeof(T)*m_nSize);
}
/*
#include "graphic.h"

class CMemStrea : public CMemFile
{
public:
	CMemStrea(UINT nGrowBytes = 1024 ) : CMemFile(nGrowBytes) { } 
	CMemStrea(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0 ) : CMemFile(lpBuffer,nBufferSize,nGrowBytes) { } 

//	CMemStrea(int nMode);
//	CMemStrea(int nSize = 1024,int nMode = 0);

	CMemStrea& operator >> (BYTE& nVal)
		{ Read(&nVal,sizeof(BYTE)); return *this; }
	CMemStrea& operator >> (WORD& nVal)
		{ Read(&nVal,sizeof(WORD)); return *this; }
	CMemStrea& operator >> (LONG& nVal)
		{ Read(&nVal,sizeof(LONG)); return *this; }
	CMemStrea& operator >> (UINT& nVal)
		{ Read(&nVal,sizeof(UINT)); return *this; }
	CMemStrea& operator >> (short& nVal)
		{ Read(&nVal,sizeof(short)); return *this; }
	//CMemStrea& operator >> (CString& str)
//		{ ReadString(str); return *this; }
	CMemStrea& operator >> (CPoint& point)
		{ Read(&point,sizeof(point)); return *this; }
	CMemStrea& operator >> (CSize& size)
		{ Read(&size,sizeof(size)); return *this; }
	CMemStrea& operator >> (CPtZ& ptz)
		{ Read(&ptz,sizeof(ptz)); return *this; }

	CMemStrea& operator << (BYTE nVal)
		{ Write(&nVal,sizeof(BYTE)); return *this; }
	CMemStrea& operator << (WORD nVal)
		{ Write(&nVal,sizeof(WORD)); return *this; }
	CMemStrea& operator << (LONG nVal)
		{ Write(&nVal,sizeof(LONG)); return *this; }
	CMemStrea& operator << (UINT nVal)
		{ Write(&nVal,sizeof(UINT)); return *this; }
	CMemStrea& operator << (short nVal)
		{ Write(&nVal,sizeof(short)); return *this; }
	//CMemStrea& operator << (LPCTSTR lpsz)
		//{ WriteString(lpsz); return *this; }
	CMemStrea& operator << (const CPoint& point)
		{ Write(&point,sizeof(point)); return *this; }
	CMemStrea& operator << (const CSize& size)
		{ Write(&size,sizeof(size)); return *this; }
	CMemStrea& operator << (const CPtZ& ptz)
		{ Write(&ptz,sizeof(ptz)); return *this; }

};

typedef CStack<WORD>  CWordStack; 
typedef CStack<CHAR>  CCharStack; 
typedef CStack<UINT>  CUIntStack; 
typedef CStack<LONG>  CLongStack; 
typedef CStack<short> CShortStack; 
typedef CStack<int>   CIntStack; 

#ifdef __DATA__

	Total data processing class

	class is

	1.Linked    : Linked list
	2.Intersect : Intersected clossing linked
	3.Iam       : Who I am? My family is parent, child, and brothers
	4.CirQueue  : Circular queue
	5.Keep	    :
	6.Stack     : ...
	7.Tree      : binary Tree

	made by nam, inhwan

#endif

class Linked
{
	public:
		int type;
		Linked *m_pPrior;
		Linked *m_pNext;
	public:
		Linked(int t = 0);
		~Linked();
		Linked *Elem(int idx);
		void Delete(void);
		void DelMem(void);
		Linked &operator - (Linked &i);
		Linked &operator + (Linked &i);
		Linked *operator [] (int idx);
		Linked *operator -- (void) { return m_pPrior; }
		Linked *operator ++ (void) { return m_pNext ; }
		Linked *GotoBegin(void);
		Linked *GotoEnd  (void);
	friend Linked *Linking(Linked *a,Linked *b);
};
class Intersect
{
	static
		Intersect *firstObj;

	public:
		int type;
		Intersect *left;
		Intersect *right;
		Intersect *up;
		Intersect *down;
		Intersect(int t=0);
		~Intersect();
		Intersect *Elem(int idx);
		void Delete(void);
		void DelH(void);
		void DelV(void);
		Intersect &operator - (Intersect &i);
		Intersect &operator + (Intersect &i);
		void operator -= (Intersect &i);
		void operator += (Intersect &i);
		Intersect *operator [] (int idx);
		Intersect *operator -- (void) { return left ; }
		Intersect *operator ++ (void) { return right; }
		Intersect *GotoLEnd(void);
		Intersect *GotoREnd(void);
		Intersect *GotoUEnd(void);
		Intersect *GotoDEnd(void);
};
class Iam
{
	static
		Iam *firstObj;

	public:
		int type;
		Iam *parent;
		Iam *child;
		Iam *older;
		Iam *younger;
		Iam(int t = 0);
		~Iam();
		Iam *Elem(int idx);
		void Kill(void);
		void KillParents(void);
		void KillBrothers(void);
		Iam &operator - (Iam &i);
		Iam &operator + (Iam &i);
		void operator -= (Iam &i);
		void operator += (Iam &i);
		Iam *operator [] (int idx);
		Iam *operator -- (void) { return older  ; }
		Iam *operator ++ (void) { return younger; }
		Iam *GotoOEnd(void);
		Iam *GotoYEnd(void);
		Iam *GotoPEnd(void);
		Iam *GotoCEnd(void);
};

typedef unsigned char BYTE; 
*/
inline void Swap(int &a,int &b)
{
	int c = a; a = b; b = c;
}
inline void Swap(BYTE &a,BYTE &b)
{
	BYTE c = a; a = b; b = c;
}

template <class T> class CFixedArray
{
protected:
	LPBYTE	m_pData;
	LPBYTE* m_apOffset;
	int m_nCurOffset;
	int m_nCurNum;
	int m_nMaxByte;
	int m_nMaxIndex;

	int m_nEndByte;
	int m_nEndIndex;

public:
	CFixedArray();
	
	virtual	~CFixedArray();
	int  GetSize() { return m_nMaxIndex; }
	void SetSize( int nNum, int nMaxIndex );
	void Add( T* pData, int nSize = 0 );
	void SetAtGrow( int nIndex, T* pData, int nSize = 0 );
	T* GetAt( DWORD dwIndex );
	void Optimize();
	void RemoveAll();
};
template <class T> inline CFixedArray<T>::CFixedArray( )
{
	m_pData = NULL;
	m_apOffset = NULL;
	m_nEndByte = 0;
	m_nEndIndex = -1;
	SetSize( 100, 100 );
}
template <class T> inline CFixedArray<T>::~CFixedArray( )
{
	SAFE_DELETE( m_apOffset );
	SAFE_DELETE( m_pData ); 
}
template <class T> inline void CFixedArray<T>::RemoveAll()
{
	SAFE_DELETE( m_apOffset );
	SAFE_DELETE( m_pData ); 
	m_pData = NULL;
	m_apOffset = NULL;
	m_nEndByte = 0;
	m_nEndIndex = -1;
	SetSize( 100, 100 );
}

// nNum = 총갯수 
// nMaxIndex = 색인 갯수 
// 0 = data,  5 = data일 경우 nMaxIndex는 0~5까지 총 6이 되며,
// nMaxNum는 0,2 총 2개가 된다. 
// 초기값은 100, 100으로 지정되어 있다.
template <class T> inline void CFixedArray<T>::SetSize( int nMaxNum, int nMaxIndex )
{
	m_pData = new BYTE[ sizeof( T ) * nMaxNum ];
	m_apOffset = new LPBYTE [ nMaxIndex ];
	ZeroMemory( m_apOffset, sizeof( LPBYTE ) * nMaxIndex );
	m_nMaxByte = sizeof( T ) * nMaxNum;
	m_nMaxIndex = nMaxIndex;
	m_nCurOffset = 0;
	m_nCurNum = 0;
}
template <class T> inline T* CFixedArray<T>::GetAt( DWORD dwIndex )
{
	if( dwIndex >= (DWORD)m_nMaxIndex )
		return NULL;
	return (T*) m_apOffset[ dwIndex ];
}
template <class T> inline void CFixedArray<T>::Add( T* pData, int nSize )
{
	SetAtGrow( m_nCurNum, pData, nSize );
	m_nCurNum++;
}
template <class T> void CFixedArray<T>::Optimize()
{
	if( m_nMaxIndex >= m_nEndIndex )
	{
		m_nMaxIndex = m_nEndIndex + 1;
		LPBYTE* apOffset = new LPBYTE [ m_nMaxIndex ]; 
		memcpy( apOffset, m_apOffset, sizeof( LPBYTE ) * m_nMaxIndex ); 
		SAFE_DELETE( m_apOffset );
		m_apOffset = apOffset;
	}
	if( m_nMaxByte > m_nCurOffset )
	{
		LPBYTE pData = new BYTE[ m_nCurOffset ]; 
		memcpy( pData, m_pData, m_nCurOffset ); 
		for( int i = 0; i <= m_nEndIndex; i++ )
			if( m_apOffset[ i ] )
				m_apOffset[ i ] = ( m_apOffset[ i ] - m_pData ) + pData; 
		SAFE_DELETE( m_pData );
		m_pData = pData;
		m_nMaxByte = m_nCurOffset ;
	}
}
/*
template <class T> void CFixedArray<T>::SetAt( int nIndex, T* pData, int nSize )
{
	if( nIndex < m_nMaxIndex )
		SetAtGrow( nIndex, pData, nSize );
	else
		ASSERT( 0 );
}*/
template <class T> void CFixedArray<T>::SetAtGrow( int nIndex, T* pData, int nSize )
{
	if( nIndex >= m_nMaxIndex )
	{
		LPBYTE* apOffset = new LPBYTE [ nIndex * 2 ]; // 두배 확장 
		ZeroMemory( apOffset, sizeof( LPBYTE ) * nIndex * 2 ); // 초기화 
		memcpy( apOffset, m_apOffset, sizeof( LPBYTE ) * ( m_nEndIndex + 1 ) ); // 복사 
		SAFE_DELETE( m_apOffset );
		m_apOffset = apOffset;
		m_nMaxIndex = nIndex * 2;
	}
	int nSizeTemp 
		= nSize ? nSize : sizeof( T );
	if( m_nCurOffset + nSizeTemp > m_nMaxByte )
	{
		m_nMaxByte = m_nCurOffset * 2 + nSizeTemp;
		LPBYTE pData2 = new BYTE[ m_nMaxByte ]; // 두배 확장 
		memcpy( pData2, m_pData, m_nCurOffset ); // 복사 
		for( int i = 0; i <= m_nEndIndex; i++ )
			if( m_apOffset[ i ] )
				m_apOffset[ i ] = ( m_apOffset[ i ] - m_pData ) + pData2; 
		SAFE_DELETE( m_pData );
		m_pData = pData2;
	}
	// 사이즈 자체에 변동이 없는 경우임( T 사이즈가 유지되는 경우 )
	if( nSize == 0 )
	{
		if( m_apOffset[ nIndex ] == NULL )
		{
			memcpy( &m_pData[ m_nCurOffset ], pData, sizeof( T ) );
			m_apOffset[ nIndex ] = (LPBYTE) m_pData + m_nCurOffset;
			m_nCurOffset += sizeof( T );
		}
		else
		{
			memcpy( m_apOffset[ nIndex ], pData, sizeof( T ) );
		}
	}
	// 각 구조체의 사이즈가 변경됨. ex) 스트링의 길이에 따라서.
	else
	{
		if( m_apOffset[ nIndex ] == NULL )
		{
			memcpy( &m_pData[ m_nCurOffset ], pData, nSize );
			m_apOffset[ nIndex ] = (LPBYTE) m_pData + m_nCurOffset;
			m_nCurOffset += nSize;
		}
		else
		{
			// 사이즈가 변경되는 구조에서는 인덱스 내용을 바꾸면 안됨.
//			ASSERT( 0 ); 
#if !defined( __SFXEDITOR ) && !defined( __COLA )
			WriteLog( "%s, %d", __FILE__, __LINE__ );
#endif
		}
	}
	if( nIndex > m_nEndIndex )
		m_nEndIndex = nIndex;
}

typedef	__int64	EXPINTEGER;
typedef	double	EXPFLOAT;

#if defined(__WORLDSERVER ) || defined(__CLIENT)
struct NaviPoint //	네비게이션에 클릭으로 위치를 표시하는 기능을 위한 구조체
{
	D3DXVECTOR3 Pos;		//	이 위치는 네이게이터상의 위치가 아닌 월드상의 위치임을 강조한다.
	short		Time;			//	포인트의 남아있는 시간
	DWORD		objid;		
	string      Name;
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

typedef vector<NaviPoint> V_NaviPoint;

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

	// ************************************
	// * 타입 없음.	: 0
	// * 일반 공격	: 1
	// * 매지션 스킬: 2
	// ************************************
	enum	eAttaType 	{ NO_ATTACK = 0, GENERAL, MAGICIAN_ATTACK, END_ATTA };
	
} // namespace SAI79

#endif
