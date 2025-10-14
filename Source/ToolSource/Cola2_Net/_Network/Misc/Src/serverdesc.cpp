#include "stdafx.h"
#include "ServerDesc.h"
#include "Data.h"
#include "Landscape.h"

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CCtrlRegion::CCtrlRegion()
{
	m_wLeft		= m_wRight	= 0;
}

CCtrlRegion::CCtrlRegion( CJurisdiction* pJurisdiction )
{
	m_wLeft	= pJurisdiction->m_wLeft;
	m_wRight	= pJurisdiction->m_wRight;

	int nLeft	= (int)( pJurisdiction->m_rect.left * MPU * MAP_SIZE );	
	int nTop	= (int)( pJurisdiction->m_rect.top * MPU * MAP_SIZE );
	int nRight	= (int)( pJurisdiction->m_rect.right * MPU * MAP_SIZE );
	int nBottom		= (int)( pJurisdiction->m_rect.bottom * MPU * MAP_SIZE );

	m_rcComplete.top	= nTop;
	m_rcComplete.bottom	= nBottom;

	if( pJurisdiction->m_wLeft != 0 ) {
		
		m_rcLeftIncomplete.SetRect( nLeft, nTop, nLeft + 64, nBottom );
		m_rcLeftNone.SetRect( nLeft - 64, nTop, nLeft, nBottom );
		m_rcComplete.left	= nLeft + 64;
	}
	else {
		m_rcComplete.left	= nLeft;
	}

	if( pJurisdiction->m_wRight != 0 ) {
		m_rcRightIncomplete.SetRect( nRight - 64, nTop, nRight, nBottom );
		m_rcRightNone.SetRect( nRight, nTop, nRight + 64, nBottom );
		m_rcComplete.right		= nRight - 64;
	}
	else {
		m_rcComplete.right		= nRight;
	}



	m_rcJurisdiction.SetRect( pJurisdiction->m_rect.left * MPU * MAP_SIZE,
											pJurisdiction->m_rect.top * MPU * MAP_SIZE,
											pJurisdiction->m_rect.right * MPU * MAP_SIZE,
											pJurisdiction->m_rect.bottom * MPU * MAP_SIZE
											);
}

CCtrlRegion::~CCtrlRegion()
{

}

short CCtrlRegion::GetPosAttr( const D3DXVECTOR3 & vPos )
{
/*
	0? 1
	x > 0, 2
	NULL_POS_ATTR,	4
	x < 0, 3
*/

	LONG x	= (LONG)vPos.x;

	if( x >= m_rcComplete.left && x < m_rcComplete.right )
		return 0;

	if( m_wLeft != 0 )
	{
		if( x >= m_rcLeftIncomplete.left && x < m_rcLeftIncomplete.right )
			return (short)m_wLeft;
		if( x >= m_rcLeftNone.left && x < m_rcLeftNone.right )
			return (short)-m_wLeft;
	}

	if( m_wRight != 0 )
	{
		if( x >= m_rcRightIncomplete.left && x < m_rcRightIncomplete.right )
			return (short)m_wRight;
		if( x>= m_rcRightNone.left && x < m_rcRightNone.right )
			return (short)-m_wRight ;
	}

	return NULL_POS_ATTR;
}

BOOL CCtrlRegion::GetRect( CPoint point, CRect & rRect )
{
	if( m_rcJurisdiction.PtInRect( point ) )
	{
		rRect.SetRect( m_rcJurisdiction.TopLeft(), m_rcJurisdiction.BottomRight() );
		return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CServerDesc::CServerDesc()
{
#ifdef __MSVR
	m_uIdofMulti	= 0;
#endif	// __MSVR
	m_uKey	= 0;
	*m_szAddr	= '\0';
//	memset( m_szAddr, 0, sizeof(m_szAddr) );
}

CServerDesc::~CServerDesc()
{
#ifdef __INSTANCE_DUNGEON
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif	// __INSTANCE_DUNGEON

	CJurisdiction* pJurisdiction;
	while( m_lspJurisdiction.size() > 0 )
	{
		pJurisdiction	= m_lspJurisdiction.front();
		m_lspJurisdiction.pop_front();
		delete pJurisdiction;
	}
}

BOOL CServerDesc::IsUnderJurisdiction( DWORD dwWorldID, const D3DVECTOR & vPos )
{
	int x	= (int)( vPos.x / MPU / MAP_SIZE );
	int z	= (int)( vPos.z / MPU / MAP_SIZE );

#ifdef __INSTANCE_DUNGEON
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif	// __INSTANCE_DUNGEON

	for( list<CJurisdiction*>::iterator i = m_lspJurisdiction.begin(); i != m_lspJurisdiction.end(); ++i )
	{
		if( (*i)->m_dwWorldID == dwWorldID
#ifdef __INSTANCE_DUNGEON
			&& !( (*i)->m_bSrc )
#endif	// __INSTANCE_DUNGEON
		)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CServerDesc::IsIntersected( DWORD dwWorldID )
{
#ifdef __INSTANCE_DUNGEON
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif	// __INSTANCE_DUNGEON

	for( list<CJurisdiction*>::iterator i = m_lspJurisdiction.begin(); i != m_lspJurisdiction.end(); ++i )
	{
		if( (*i)->m_dwWorldID == dwWorldID )
			return TRUE;
	}
	return FALSE;
}

void CServerDesc::Serialize( CAr & ar )
{
#ifdef __INSTANCE_DUNGEON
	CMclAutoLock	Lock( m_AddRemoveLock );
#endif	// __INSTANCE_DUNGEON

	if( ar.IsStoring() )
	{
		ar << m_uKey;
		ar << (short)m_lspJurisdiction.size();
		for( list<CJurisdiction*>::iterator i = m_lspJurisdiction.begin(); i != m_lspJurisdiction.end(); ++i )
		{
			ar << (*i)->m_dwWorldID;
			ar << (*i)->m_rect;
			ar << (*i)->m_wLeft;
			ar << (*i)->m_wRight;
#ifdef __INSTANCE_DUNGEON
			ar << (*i)->m_bSrc;
#endif	// __INSTANCE_DUNGEON
		}
		ar.WriteString( m_szAddr );
	}
	else
	{
#	ifdef __MSVR
		u_long uKey;
		ar >> uKey;
		SetKey( uKey );
#	else	//__MSVR
		ar >> m_uKey;
#	endif	// __MSVR
		short nSize;
		ar >> nSize;
		for( int i =0; i < nSize; i++ )
		{
			CJurisdiction* pJurisdiction	= new CJurisdiction;
			ar >> pJurisdiction->m_dwWorldID;
			ar >> pJurisdiction->m_rect;
			ar >> pJurisdiction->m_wLeft;
			ar >> pJurisdiction->m_wRight;
#	ifdef __INSTANCE_DUNGEON
			ar << pJurisdiction->m_bSrc;
#	endif	// __INSTANCE_DUNGEON
			m_lspJurisdiction.push_back( pJurisdiction );
		}
		ar.ReadString( m_szAddr );
	}
}

#ifdef __INSTANCE_DUNGEON
void CServerDesc::Cleareq( void )
{
	if( m_uKey != EQ_KEY )
		return;
	
	CMclAutoLock	Lock( m_AddRemoveLock );

	while( !m_lspJurisdiction.empty() )
	{
		CJurisdiction* pJurisdiction	= m_lspJurisdiction.back();
		if( pJurisdiction->m_bSrc )
			return;
		delete pJurisdiction;
		m_lspJurisdiction.pop_back();
	}
}

void CServerDesc::SetKey( u_long uKey )
{
	m_uKey	= uKey;
	m_uIdofMulti	= uKey / 100;
	if( uKey == EQ_KEY )
	{
		CMclAutoLock	Lock( m_AddRemoveLock );
		for( list<CJurisdiction*>::iterator i = m_lspJurisdiction.begin(); i != m_lspJurisdiction.end(); ++i )
			(*i)->m_bSrc	= TRUE;
	}
}

CJurisdiction* CServerDesc::GetJurisdiction( DWORD dwWorldId )
{
	for( list<CJurisdiction*>::iterator i = m_lspJurisdiction.begin(); i != m_lspJurisdiction.end(); ++i )
	{
		if( (*i)->m_dwWorldID == dwWorldId )
			return (*i);
	}
	return NULL;
}
#endif	// __INSTANCE_DUNGEON

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CServerDescArray::CServerDescArray()
{
	SetSize( 16, 16, 8 );	
}

CServerDescArray::~CServerDescArray()
{
	Free();
}

void CServerDescArray::Free( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );

	CMyBucket<CServerDesc*>* pBucket	= GetFirstActive();
	while( pBucket )
	{
		delete pBucket->m_value;
		pBucket		= pBucket->pNext;
	}

	ClearActiveList();
}

CServerDesc* CServerDescArray::GetAt( ULONG uKey )
{
	CServerDesc* pDesc;

	if( Lookup( uKey, pDesc ) )
		return pDesc;

	return NULL;
}