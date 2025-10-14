#ifndef __SERVER_DESC_H__
#define __SERVER_DESC_H__

#pragma once

#include "Ar.h"
#include "MyMap.h"

class CJurisdiction
{
public:
	DWORD	m_dwWorldID;
	CRect	m_rect;
	WORD	m_wLeft;
	WORD	m_wRight;
#ifdef __INSTANCE_DUNGEON
	BOOL	m_bSrc;
#endif	// __INSTANCE_DUNGEON

public:
	// Constructions
	CJurisdiction()
		{
			m_dwWorldID	= NULL_ID;
			m_rect.SetRect( 0, 0, 0, 0 );
			m_wLeft		= m_wRight	= 0;
#		ifdef __INSTANCE_DUNGEON
			m_bSrc	= FALSE;
#		endif	// __INSTANCE_DUNGEON
		}
	CJurisdiction( DWORD dwWorldID, const CRect & rect, WORD wLeft, WORD wRight )
		{
			m_dwWorldID	= dwWorldID;
			m_rect.CopyRect( rect );
			m_wLeft		= wLeft;
			m_wRight	= wRight;
#		ifdef __INSTANCE_DUNGEON
			m_bSrc	= FALSE;
#		endif	// __INSTANCE_DUNGEON
		}

#ifdef __INSTANCE_DUNGEON
	CJurisdiction( CJurisdiction* pSrc, DWORD dwWorldId )
		{
			m_dwWorldID	= dwWorldId;
			m_rect.CopyRect( pSrc->m_rect );
			m_wLeft	= m_wRight	= 0;
			m_bSrc	= FALSE;
		}
#endif	// __INSTANCE_DUNGEON

	virtual	~CJurisdiction()
		{
		}
};

#define NULL_POS_ATTR	(short)-32768

class CCtrlRegion
{
public:
	WORD	m_wLeft;
	WORD	m_wRight;

	CRect	m_rcComplete;
	CRect	m_rcLeftIncomplete;
	CRect	m_rcLeftNone;
	CRect	m_rcRightIncomplete;
	CRect	m_rcRightNone;
	CRect	m_rcJurisdiction;

public:
//	Constructors
	CCtrlRegion();
	CCtrlRegion( CJurisdiction* pJurisdiction );
//	 Desctructor
	~CCtrlRegion();
//	Operations
	short	GetPosAttr( const D3DXVECTOR3 & vPos );
	BOOL	GetRect( CPoint point, CRect & rRect );
};

#ifdef __INSTANCE_DUNGEON
#define	EQ_KEY	1000
#define	EQ_M_ID		10
#endif	// __INSTANCE_DUNGEON

class CServerDesc
{
#ifdef __MSVR
public:
	u_long	m_uIdofMulti;
private:
#else	// __MSVR
public:
#endif	// __MSVR
	u_long	m_uKey;
public:
	char	m_szAddr[16];
	list<CJurisdiction*>	m_lspJurisdiction;

public:
//	Constructions
	CServerDesc();
	virtual	~CServerDesc();

//	Operations
	BOOL	IsUnderJurisdiction( DWORD dwWorldID, const D3DVECTOR & vPos );
	BOOL	IsIntersected( DWORD dwWorldID );
#ifdef __MSVR
//	Attributes
#ifdef __INSTANCE_DUNGEON
	CMclCritSec		m_AddRemoveLock;
	void	Cleareq( void );
	void	SetKey( u_long uKey );
	CJurisdiction*	GetJurisdiction( DWORD dwWorldId );
#else	// __INSTANCE_DUNGEON
	void	SetKey( u_long uKey )	{	m_uKey	= uKey;		m_uIdofMulti	= uKey / 100;	}
#endif	// __INSTANCE_DUNGEON
	u_long	GetKey( void )	{	return m_uKey;	}
	u_long	GetIdofMulti( void )	{	return	m_uIdofMulti;	}	
#endif	// __MSVR
	void	Serialize( CAr & ar );
};

class CServerDescArray : public CMyMap<CServerDesc*>
{
public:

public:
	// Constructions
	CServerDescArray();
	virtual	~CServerDescArray();
	
	void Free( void );

	// Operations
	CServerDesc*	GetAt( ULONG uKey );
};

#endif	// __SERVER_DESC_H__