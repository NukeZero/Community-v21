#ifndef __ACTMSGQUE_H__
#define	__ACTMSGQUE_H__

#define	MAX_ACTMSG	0xff

typedef	struct	tagACTMSG
{
	OBJMSG	dwMsg;
	int	nParam1, nParam2, nParam3;
}
ACTMSG, *PACTMSG;

class CActMsgQue
{
	ACTMSG	m_aActMsg[MAX_ACTMSG];
	u_long	m_uHead;
	u_long	m_uTail;
public:
//	Constructions
	CActMsgQue()	{	Empty();	}
	~CActMsgQue()	{}
//	Operations
	VOID	AddTail( PACTMSG pActMsg );
	VOID	AddTail( OBJMSG dwMsg, int nParam1, nParam2, nParam3 );
	PACTMSG	RemoveHead( void );
	PACTMSG	GetHead( void );
	VOID	Empty( void )	{	m_uHead	= m_uTail	= 0;	}
//	Attributes
	BOOL	IsEmpty( void )	{	return( m_uHead == m_uTail );	}
}

inline VOID CActMsgQue::AddTail( PACTMSG pActMsg )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	chASSERT( uTail != m_uHead );
	memcpy( ( m_aActMsg + m_uTail ), pActMsg, sizeof(ACTMSG) );
	m_uTail		= uTail;
}

inline VOID CActMsgQue::AddTail( OBJMSG dwMsg, int nParam1, nParam2, nParam3 )
{
	u_long uTail	= ( m_uTail + 1 ) % MAX_ACTMSG;
	chASSERT( uTail != m_uHead );
	PACTMSG pActMsg		= m_aActMsg + m_uTail;
	pActMsg->dwMsg	= dwMsg;
	pActMsg->nParam1	= nParam1;
	pActMsg->nParam2	= nParam2;
	pActMsg->nParam3	= nParam3;
	m_uTail		= uTail;
}

inline PACTMSG CActMsgQue::RemoveHead( void )
{
	if( IsEmpty() )
		return NULL;
	PACTMSG pActMsg	= m_aActMsg + m_uHead;
	m_uHead		= ( m_uHead + 1 ) % MAX_ACTMSG;
	return pActMsg;
}

inline PACTMSG CActMsgQue::GetHead( void )
{
	if( IsEmpty() )
		return NULL;
	return( m_aActMsg + m_uHead );
}

#endif	// __ACTMSGQUE_H__