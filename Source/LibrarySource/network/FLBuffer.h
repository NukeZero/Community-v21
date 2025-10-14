#ifndef __FLBUFFER_H__
#define	__FLBUFFER_H__


#include "FLNetworkCommon.h"
#include "../memory/FLLowBufferPool.h"




#define	MAX_BUFFER		8192
#define	MAX_WSABUF		128



///////////////////////////////////////////////////////////////////////
// CBuffer
///////////////////////////////////////////////////////////////////////

class CBuffer
{
public:

	CBuffer*	pNext;

private:

	DPID		m_dpid;
	u_long		m_nBlockCount;

	LPBYTE		m_lpBufStart;
	LPBYTE		m_lpBufMax;
	LPBYTE		m_pHead;
	LPBYTE		m_pTail;

public:

	explicit CBuffer( u_long uBufSize );
	~CBuffer();

	u_long	GetSize()	const;
	LPBYTE	GetWritableBuffer( u_long* pnBufSize )	const;
	u_long	GetWritableBufferSize()	const;
	LPBYTE	GetReadableBuffer( u_long* pnBufSize )	const;
	u_long	GetReadableBufferSize()	const;

	u_long	GetBlockCount()	const;
	void	IncBlockCount();

	void	SetDPID( DPID dpid );
	DPID	GetDPID()	const;

	bool	Write( const void* pData, u_long nSize );

	void	CutTail( u_long nSize );
	void	UseTail( u_long nSize );

	void	CutHead( u_long nSize );
	void	DropHead( u_long nSize );

public:

	static	FLLowBufferPool< MAX_BUFFER, 512 >	m_cLowPool;

	MEMPOOLER_DECLARE( CBuffer );

};


///////////////////////////////////////////////////////////////////////
// CBufferQueue
///////////////////////////////////////////////////////////////////////

class CBufferQueue
{
private:

	CBuffer*	m_pHead;
	CBuffer*	m_pTail;
	u_long		m_uCount;

public:

	CBufferQueue();
	~CBufferQueue();

	CBuffer*	GetHead()	const;
	CBuffer*	GetTail()	const;

	BOOL		IsEmpty()	const;
	u_long		GetCount()	const;

	CBuffer*	PopHead();
	void		RemoveData( u_long uDataSize );

	void		AddTail( CBuffer* pBuffer );
	void		AddTail( CBufferQueue* pQueue );

	bool		Write( const void* pData, u_long nSize );

	void		Clear( BOOL bDelete );

	void		GetData( LPWSABUF lpBuffers, LPDWORD lpdwBufferCount )	const;
	void		SetDPID( DPID dpid );

private:

	void		RemoveHead();

};


///////////////////////////////////////////////////////////////////////
// CBufferFactory
///////////////////////////////////////////////////////////////////////

class CBufferFactory
{
public:

	static CBufferFactory&	GetInstance();

	CBuffer*	CreateBuffer( u_long uBufSize );

private:

	CBufferFactory();

};


#endif	// __BUFFER_H__