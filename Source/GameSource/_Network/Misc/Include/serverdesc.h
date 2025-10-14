#ifndef __SERVER_DESC_H__
#define __SERVER_DESC_H__

#pragma once

#include <network/FLAr.h>

class CJurisdiction
{
public:
	DWORD	m_dwWorldID;
	CRect	m_rect;
	WORD	m_wLeft;
	WORD	m_wRight;
public:
	// Constructions
	CJurisdiction()
		{
			m_dwWorldID	= NULL_ID;
			m_rect.SetRect( 0, 0, 0, 0 );
			m_wLeft		= m_wRight	= 0;
		}
	CJurisdiction( DWORD dwWorldID, const CRect & rect, WORD wLeft, WORD wRight )
		{
			m_dwWorldID	= dwWorldID;
			m_rect.CopyRect( rect );
			m_wLeft		= wLeft;
			m_wRight	= wRight;
		}
	virtual	~CJurisdiction()
		{
		}
};

#define NULL_POS_ATTR	(short)-32768



class CServerDesc
{
public:
	u_long	m_uIdofMulti;
private:
	u_long	m_uKey;
public:
	char	m_szAddr[16];
	std::list<CJurisdiction*>	m_lspJurisdiction;

public:
//	Constructions
	CServerDesc();
	virtual	~CServerDesc();

//	Operations
	BOOL	IsUnderJurisdiction( DWORD dwWorldID, const D3DVECTOR & vPos );
	BOOL	IsIntersected( DWORD dwWorldID );
//	Attributes
	void	SetKey( u_long uKey )	{	m_uKey	= uKey;		m_uIdofMulti	= uKey / 100;	}
	u_long	GetKey( void )	{	return m_uKey;	}
	u_long	GetIdofMulti( void )	{	return	m_uIdofMulti;	}	
	void	Serialize( CAr & ar );
};

class CServerDescArray : public std::map<u_long, CServerDesc*>
{
public:
	CMclCritSec		m_AddRemoveLock;

public:
	// Constructions
	CServerDescArray();
	virtual	~CServerDescArray();
	
	void Free( void );

	// Operations
	CServerDesc*	GetAt( ULONG uKey );
};

#endif	// __SERVER_DESC_H__