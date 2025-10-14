#include "stdafx.h"

//sun: 11, 주머니

#include "pocket.h"

#ifdef __WORLDSERVER
#include "../WorldServer/user.h"
#endif	// __WORLDSERVER


CPocketController::CPocketController()
{
#ifdef __WORLDSERVER
	m_pMover	= NULL;
#endif	// __WORLDSERVER

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		m_kPocketProperty[nPocket].bExpired			= TRUE;
		m_kPocketProperty[nPocket].tExpirationDate	= 0;
	}
}

CPocketController::~CPocketController()
{
	Clear();
}

void CPocketController::Clear( void )
{
}

BOOL CPocketController::IsAvailable( int nPocket, BOOL bExpiration )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return FALSE;
	}

	if( m_kPocket[nPocket].IsAlloced() == TRUE
		&& ( bExpiration == FALSE || IsExpired( nPocket ) == FALSE ) )
	{
		return TRUE;
	}

	return FALSE;
//	return( m_apPocket[nPocket] && ( !bExpiration || !m_apPocket[nPocket]->IsExpired() ) );
}

void CPocketController::Avail( int nPocket, time_t t )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return;
	}

	time_t tCurrent		= time_null();
	DWORD dwPocketSize	= MAX_DEFAULT_POCKET;
	
	if( nPocket > 0 )
	{
		dwPocketSize = MAX_EXTENSION_POCKET;
	}

	if( m_kPocket[nPocket].IsAlloced() == FALSE )
	{
		m_kPocket[nPocket].SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_POCKET, dwPocketSize );
		SetExpirationDate( nPocket, tCurrent + t );
	}
	else
	{
		if( IsExpired( nPocket ) == TRUE )
		{
			SetExpirationDate( nPocket, tCurrent + t );
		}
		else
		{
			SetExpirationDate( nPocket, GetExpirationDate( nPocket ) + t );
		}
	}

	SetExpired( nPocket, FALSE );
	if( t == 0 )
	{
		SetExpirationDate( nPocket, 0 );
	}
	
// 	if( m_apPocket[nPocket] )
// 	{
// 		if( m_apPocket[nPocket]->IsExpired() )
// 			m_apPocket[nPocket]->SetExpirationDate( tCurrent + t );
// 		else
// 			m_apPocket[nPocket]->SetExpirationDate( m_apPocket[nPocket]->GetExpirationDate() + t );
// 	}
// 	else
// 	{
// 		m_apPocket[nPocket]		= new CPocket;
// 		m_apPocket[nPocket].SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_POCKET, nPocket == 0? MAX_DEFAULT_POCKET: MAX_EXTENSION_POCKET );
// 		m_apPocket[nPocket]->SetExpirationDate( tCurrent + t );
// 	}
// 	m_apPocket[nPocket]->SetExpired( FALSE );
// 
// 	if( t == 0 )
// 		m_apPocket[nPocket]->SetExpirationDate( 0 );
}

#if defined(__WORLDSERVER) || defined(__CLIENT)
FLItemElem*	CPocketController::GetAtId( int nPocket, DWORD dwItemObjID, BOOL bExpiration )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return NULL;
	}

	if( IsAvailable( nPocket, bExpiration ) == FALSE )
	{
		return NULL;
	}

	return m_kPocket[nPocket].GetAtId( dwItemObjID );
}

void	CPocketController::SetAtId( int nPocket, OBJID dwObjId, FLItemElem* pItemElem )
{
	if( IsAvailable( nPocket, FALSE ) == FALSE )
	{
		return;
	}

	m_kPocket[nPocket].SetAtId( dwObjId, pItemElem );
}

#ifdef __CLIENT
FLItemElem*	CPocketController::GetAt( int nPocket, int nIndex )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return NULL;
	}

	if( IsAvailable( nPocket, FALSE ) == FALSE )
	{
		return NULL;
	}

	return m_kPocket[nPocket].GetAt( nIndex );
}
#endif	// __CLIENT

BOOL	CPocketController::Add( int nPocket, FLItemElem*pItemElem )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return FALSE;
	}

	DWORD dwItemObjID[MAX_EXTENSION_POCKET] = { NULL_ID, };
	int	anNum[MAX_EXTENSION_POCKET] = { 0, };

	DWORD dwCount	= 0;

	if( IsAvailable( nPocket ) && m_kPocket[nPocket].Add( pItemElem, dwItemObjID, anNum, &dwCount ) )
	{
#ifdef __WORLDSERVER
		for( DWORD i = 0; i < dwCount; ++i )
		{
			FLItemElem * pCreateItem = m_kPocket[nPocket].GetAtId( dwItemObjID[i] );
			if( pCreateItem != NULL )
			{
				( (FLWSUser*)m_pMover )->AddPocketAddItem( nPocket, pCreateItem );
			}
		}
#endif	// __WORLDSERVER
		return TRUE;
	}
	return FALSE;
}

BOOL	CPocketController::RemoveAtId( int nPocket, DWORD dwItemObjID, int nNum,  BOOL bExpiration )	// if nNum < 0, remove all.
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return FALSE;
	}

	if( bExpiration && IsAvailable( nPocket ) == FALSE )
	{
		return FALSE;
	}

	FLItemElem* pItemElem	= m_kPocket[nPocket].GetAtId( dwItemObjID );
	if( pItemElem == NULL )
	{
		return FALSE;
	}

	if( pItemElem->m_nItemNum > nNum )
	{
		pItemElem->m_nItemNum	-= nNum;
	}
	else
	{
		m_kPocket[nPocket].RemoveAtId( dwItemObjID );
	}

#ifdef __WORLDSERVER
	( static_cast<FLWSUser*>( m_pMover ) )->AddPocketRemoveItem( nPocket, dwItemObjID, nNum, bExpiration );
#endif	// __WORLDSERVER

	return TRUE;
}

int	CPocketController::RemoveItem( DWORD dwItemId, int nNum )
{
	int nRemnant	= nNum;
	FLItemElem* pItemElem	= NULL;

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( IsAvailable( nPocket ) )
		{
			for( DWORD dwObjId = 0; dwObjId < m_kPocket[nPocket].GetMax() && nRemnant > 0; ++dwObjId )
			{
				pItemElem	= m_kPocket[nPocket].GetAtId( dwObjId ) ;
				if( pItemElem && pItemElem->m_dwItemId == dwItemId )
				{
					if( nRemnant > pItemElem->m_nItemNum )
					{
						nRemnant	-= pItemElem->m_nItemNum;
						m_kPocket[nPocket].RemoveAtId( dwObjId );
					}
					else
					{
						pItemElem->m_nItemNum	-= nRemnant;
						nRemnant	= 0;
						return nNum;
					}
				}
			}
		}
	}

	return( nNum - nRemnant );
}

void	CPocketController::RemoveAllItem()
{
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( IsAvailable( nPocket ) )
		{
			for( DWORD dwObjId = 0; dwObjId < m_kPocket[nPocket].GetMax(); ++dwObjId )
			{
				FLItemElem* pItemElem	= m_kPocket[nPocket].GetAtId( dwObjId );
				if( pItemElem )
				{
					RemoveAtId( nPocket, pItemElem->m_dwObjId, pItemElem->m_nItemNum );
				}
			}
		}
	}
}

int		CPocketController::GetEmptyCount( void )
{
	int nCount	= 0;
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( IsAvailable( nPocket ) )
		{
			nCount	+= m_kPocket[nPocket].GetEmptyCountByInventoryType( INVEN_TYPE_NONE );
		}
	}

	return nCount;
}

//	포켓이 완전히 비어있는지 검사
BOOL	CPocketController::IsAllClean( void )
{
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( m_kPocket[nPocket].IsAllEmpty() == FALSE )
		{
			return FALSE;
		}
	}

	return TRUE;

// 	int nCount	= 0,nCountMax = 0;
// 	for( int i = 0; i < MAX_CHARACTER_POCKET; i++ )
// 	{
// 		if( m_apPocket[i] != NULL )
// 		{
// 			nCount	+= m_apPocket[i]->GetEmptyCountByInventoryType( INVEN_TYPE_NONE );
// 			nCountMax	+= m_apPocket[i]->GetMax();
// 		}
// 	}
// 	if( nCount == nCountMax)
// 		return TRUE;
// 	else
// 		return FALSE;
}

int		CPocketController::GetCount( DWORD dwItemId )
{
	int nCount	= 0;
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( IsAvailable( nPocket ) )
		{
			nCount	+= m_kPocket[nPocket].GetCountByItemId( dwItemId );
		}
	}

	return nCount;
}

time_t	CPocketController::GetAvailable( int nPocket )
{
	time_t t = 0;

	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return t;
	}

	if( IsAvailable( nPocket ) )
	{
		t = GetExpirationDate( nPocket ) - time_null();
		if( t < 0 )
		{
			t = 0;
		}
	}

	return t;
}

void	CPocketController::SetAttribute( int nAttribute, int nPocket, int nData )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return;
	}

	switch( nAttribute )
	{
		case expired:
			{
				if( IsAvailable( nPocket, FALSE ) )
				{
					SetExpired( nPocket, static_cast<BOOL>( nData ) );
				}
				break;
			}
		case avail:
			{
				Avail( nPocket, static_cast<time_t>( nData ) );
				break;
			}
		default:
			break;
	}
#ifdef __WORLDSERVER
	( (FLWSUser*)m_pMover )->AddPocketAttribute( nAttribute, nPocket, nData );
#endif	// __WORLDSERVER
}
#endif	// __WORLDSERVER || __CLIENT

void	CPocketController::Copy( CPocketController & rPocketController )
{
	Clear();

	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; nPocket++ )
	{
		if( rPocketController.IsAvailable( nPocket, FALSE ) == TRUE )
		{
			Avail( nPocket );
			m_kPocket[nPocket].Copy( rPocketController.m_kPocket[nPocket] );
		}
		else
		{
			SAFE_DELETE_ARRAY( m_kPocket[nPocket].m_apItem );
			SAFE_DELETE_ARRAY( m_kPocket[nPocket].m_apIndex );
		}

		m_kPocketProperty[nPocket].bExpired			= rPocketController.IsExpired( nPocket );
		m_kPocketProperty[nPocket].tExpirationDate	= rPocketController.GetExpirationDate( nPocket );
	}
}

void	CPocketController::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
		{
			if( IsAvailable( nPocket, FALSE ) == TRUE )
			{
				ar << (BYTE)1;
				ar << m_kPocketProperty[nPocket].bExpired;
				ar << m_kPocketProperty[nPocket].tExpirationDate;
				if( m_kPocketProperty[nPocket].tExpirationDate > 0 )
				{
					time_t	t	= m_kPocketProperty[nPocket].tExpirationDate - time_null();
					ar << t;
				}
				m_kPocket[nPocket].Serialize( ar );
			}
			else
			{
				ar << (BYTE)0;
			}
		}
	}
	else
	{
		Clear();

		for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
		{
			BYTE bExists;
			ar >> bExists;
			if( bExists == 0 )
			{
				continue;
			}

			Avail( nPocket );
			ar >> m_kPocketProperty[nPocket].bExpired;
			ar >> m_kPocketProperty[nPocket].tExpirationDate;
			if( m_kPocketProperty[nPocket].tExpirationDate )
			{
				time_t	t;
				ar >> t;
#ifdef __CLIENT
				m_kPocketProperty[nPocket].tExpirationDate	= time_null() + t;
#endif	// __CLIENT
			}
			m_kPocket[nPocket].Deserialize( ar );
		}
	}
}

void	CPocketController::SetExpired( int nPocket, BOOL bExpired )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return;
	}

	m_kPocketProperty[nPocket].bExpired = bExpired;
}

BOOL	CPocketController::IsExpired( int nPocket )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return TRUE;
	}

	return m_kPocketProperty[nPocket].bExpired;
}
void	CPocketController::SetExpirationDate( int nPocket, time_t tExpirationDate )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return;
	}

	m_kPocketProperty[nPocket].tExpirationDate = tExpirationDate;
}

time_t	CPocketController::GetExpirationDate( int nPocket )
{
	if( nPocket < 0 || nPocket >= MAX_CHARACTER_POCKET )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nPocket is Invalid. nPocket:[%d]" ), nPocket );
		return 0;
	}

	return m_kPocketProperty[nPocket].tExpirationDate;
}

BOOL	CPocketController::IsExistDoNotHaveItemServerTransform()
{
	for( int nPocket = 0; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( IsAvailable( nPocket, FALSE ) )
		{
			if( m_kPocket[nPocket].IsExistDoNotHaveItemServerTransform() == TRUE )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// CPocket*	CPocketController::GetPocket( int nPocket )
// {
// 	if( IsAvailable( nPocket, FALSE ) )
// 		return m_apPocket[nPocket];
// 	return NULL;
// }

#ifdef __WORLDSERVER
void	CPocketController::ProcessExpiring( void )
{
	if( m_pMover->IsNPC() )
		return;
	FLWSUser* pUser	= (FLWSUser*)m_pMover;

	for( int nPocket = 1; nPocket < MAX_CHARACTER_POCKET; ++nPocket )
	{
		if( IsAvailable( nPocket ) )
		{
			if( time_null() > GetExpirationDate( nPocket ) )
			{
				SetAttribute( expired, nPocket, TRUE );
#ifdef __INTERNALSERVER
				pUser->AddPocketView();
#endif	// __INTERNAL_SERVER
			}
		}
	}
}
#endif	// __WORLDSERVER
