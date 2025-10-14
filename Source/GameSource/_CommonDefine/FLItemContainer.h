#ifndef __FLITEMCONTAINER_H__
#define __FLITEMCONTAINER_H__


typedef struct _T_ADDITEM_INFO
{
	DWORD	dwItemId;
	int		nItemNum;

	_T_ADDITEM_INFO() : dwItemId( 0 ), nItemNum( 0 )
	{
	}

} T_ADDITEM_INFO, *PT_ADDITEM_INFO;


template <class T> class CItemContainer  
{
public:
	CItemContainer();
	virtual ~CItemContainer();

	void	Clear();
	void	SetItemContainer( DWORD dwItemType, int nContainerType, DWORD dwItemMax, DWORD dwExtra = NULL_ID );
	void	Serialize( CAr & ar ) const;
	void	Deserialize( CAr & ar );

	//////////////////////////////////////////////////////////////////////////
	// Equip 관련 
	BOOL	DoEquip( DWORD dwSrcObjIndex, DWORD dwDestObjIndex );
	BOOL	UnEquip( DWORD dwObjIndex );
	BOOL	IsEquip( DWORD dwObjId );
	T*		GetEquip( DWORD dwObjIndex );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 장착을 포함한 인벤내에서..
	T*		GetAt( DWORD dwObjIndex )  const;
	T*		GetAtId( DWORD dwObjId ) const;
	DWORD	GetObjIdByObjIndex( DWORD dwObjIndex );
	DWORD	GetObjIndexByObjId( DWORD dwObjId );
	void	SetAtId( DWORD dwObjId, T* pItemElem );
	void	RemoveAt( DWORD dwObjIndex );
	void	RemoveAtId( DWORD dwObjId );
	BOOL	IsAlloced();
	BOOL	IsAllEmpty();
	BOOL	IsExistDoNotHaveItemServerTransform();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 장착을 제외한 인벤내에서..
	T*		GetAtByItemId( DWORD dwItemId );
	T*		GetAtByItemKind3( DWORD dwItemKind3 );
	int		GetItemNumByItemId( DWORD dwItemId );
	int		GetItemNumByItemKind3( DWORD dwItemKind3 );
	int		GetCountByItemId( DWORD dwItemId );
	bool	CheckItem( std::vector<T_ADDITEM_INFO> & vecAddItemInfo );
	bool	IsEnoughEmptySpace( std::vector<T_ADDITEM_INFO> & vecAddItemInfo );
	int		GetEmptyCountByInventoryType( int nInventoryType );
	int		GetEmptyCountByItemId( DWORD dwItemId );

	T*		GetItem( PT_ITEM_SPEC pItemProp, int nAbilityOption );
	T*		GetItem( PT_ITEM_SPEC pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption );

	DWORD	GetObjIdByItemId( DWORD dwItemId );
	//////////////////////////////////////////////////////////////////////////

#ifdef INVENTORY_ITEM_ALIGN
	void	Align(DWORD dwIndex);
#endif	// INVENTORY_ITEM_ALIGN
	//////////////////////////////////////////////////////////////////////////
	bool	Swap( DWORD dwSrcObjIndex, DWORD dwDestObjIndex );
	BOOL	IsFull( T* pElem, int nNum );
	BOOL	Add( T* pElem );
	BOOL	Add( T* pElem, DWORD* pItemObjID, int* pItemQuantity, DWORD* pCount );
	void	Copy( const CItemContainer<T> & rItemContainer );
	DWORD	GetFirstObjIndex( int nInventoryType );
	DWORD	GetSize();
	DWORD	GetMax();
	int		GetContainerType();
	BOOL	GetAllocedSerialNumber();
	void	SetAllocedSerialNumber( BOOL bAllocedSerialNumber );
	bool	GetObjIndexRange( const T_ITEM_SPEC* pItemProp, DWORD* pStart, DWORD* pEnd ) const;
	bool	GetObjIndexRange( int nInventoryType, DWORD* pStart, DWORD* pEnd ) const;
	//////////////////////////////////////////////////////////////////////////

#ifdef __DBSERVER
	void	ConvertInventoryIndex( DWORD dwOldInvenSize, DWORD dwOldPartsSize );
	void	RefreshIndex();
#endif // __DBSERVER

#ifdef __WORLDSERVER
	void	ReallocSerialNumber();
	void	InsertAllItem();
	void	RemoveAllItem();


	static void	InsertItem( SERIALNUMBER iSerialNumber, u_long /*idPlayerorGuild*/ )
	{
		CSerialNumber::GetInstance()->InsertSerialNumber( iSerialNumber );
	}
	static void	RemoveItem( SERIALNUMBER iSerialNumber, u_long /*idPlayerorGuild*/ )
	{
		CSerialNumber::GetInstance()->RemoveSerialNumber( iSerialNumber );
	}
#endif // __WORLDSERVER

	
private:
	void	Init();
	bool	AddItemToItem( DWORD dwSrcIndex, DWORD dwDstIndex );
	bool	ChangeItemToItem( DWORD dwSrcIndex, DWORD dwDstIndex );
	bool	MoveItemToEmpty( DWORD dwSrcIndex, DWORD dwDstIndex );


public:
	T*			m_apItem;					// dwObjId를 가지고 아이템을 찾는다.
	LPDWORD		m_apIndex;					// dwObjIndex를 가지고 dwObjId를 찾는다.


private:
	DWORD		m_dwIndexNum;				// equp을 제외한 순수 콘테이너 사이즈 
	DWORD		m_dwItemMax;				// equp을 포함한 전체 사이즈 
	
	BOOL		m_bAllocedSerialNumber;		// 시리얼넘버 변환을 위한 변수

	int			m_nContainerType;			// 타입
};

template <class T> CItemContainer<T>::CItemContainer()
{
	m_apItem				= NULL;
	m_apIndex				= NULL;
	m_dwItemMax				= 0;
	m_dwIndexNum			= 0;
	m_bAllocedSerialNumber	= FALSE;
	m_nContainerType		= CONTAINER_TYPE_NONE;
}

template <class T> CItemContainer<T>::~CItemContainer()
{
	SAFE_DELETE_ARRAY( m_apItem );
	SAFE_DELETE_ARRAY( m_apIndex );
}

template <class T> void CItemContainer<T>::Clear()
{
	for( DWORD i = 0; i < m_dwItemMax; ++i )
	{
		m_apItem[ i ].Empty();
		m_apIndex[ i ] = NULL_ID;
	}

	m_bAllocedSerialNumber = FALSE;
}

template <class T> void CItemContainer<T>::SetItemContainer( DWORD /*dwItemType*/, int nContainerType, DWORD dwItemMax, DWORD dwExtra )
{
	m_dwIndexNum		= dwItemMax;
	m_dwItemMax			= dwItemMax;
	m_nContainerType	= nContainerType;

	if( dwExtra != NULL_ID )
	{
		m_dwItemMax += dwExtra;
	}

	if( m_dwIndexNum > m_dwItemMax )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Container Size Invalid. m_nContainerType:[%d], m_dwItemMax:[%d], m_dwIndexNum:[%d]" )
			, m_nContainerType, m_dwItemMax, m_dwIndexNum );
	}

	SAFE_DELETE_ARRAY( m_apItem );
	SAFE_DELETE_ARRAY( m_apIndex );

	m_apItem	= new T[ m_dwItemMax ];
	m_apIndex	= new DWORD[ m_dwItemMax ];

	Clear();
}

template <class T> void CItemContainer<T>::Serialize( CAr & ar ) const
{
	DWORD dwSize = 0;

	if( ar.IsStoring() == TRUE )
	{
		ar.Write( m_apIndex, sizeof(DWORD) * m_dwItemMax );

		ptrdiff_t nSizeTag = ar.ReserveSpace( sizeof( dwSize ) );

		for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
		{
			if( m_apItem[ dwObjId ].IsEmpty() == FALSE )
			{
				ar << dwObjId;
				m_apItem[ dwObjId ].Serialize( ar );
				++dwSize;

				//FLASSERT( m_apItem[ch].GetSerialNumber() != 0 );
			}
		}

		ar.WriteReservedSpace( nSizeTag, &dwSize, sizeof( dwSize ) );

		ar << m_bAllocedSerialNumber;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR. IsStoring() FALSE ]" ) );
		return;
	}
}

template <class T> void CItemContainer<T>::Deserialize( CAr & ar )
{
	DWORD dwSize = 0;

	if( ar.IsLoading() == TRUE )
	{
		ar.Read( m_apIndex, sizeof(DWORD) * m_dwItemMax );
		// Clear
		for( DWORD i = 0; i < m_dwItemMax; ++i )
		{
			m_apItem[ i ].Empty();
		}

		ar >> dwSize;

		if( dwSize > m_dwItemMax )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR. CONTAINER SIZE OVERFLOW. Type(%d), Size(%u), Max(%u) ]" )
				, m_nContainerType, dwSize, m_dwItemMax );
			RELEASE_BREAK;
		}

		for( DWORD i = 0; i < dwSize; ++i )
		{
			DWORD dwObjId = 0;
			ar >> dwObjId;
			if( dwObjId < m_dwItemMax )
			{
				m_apItem[ dwObjId ].Deserialize( ar );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR. INVALID OBJID(%u), MAX(%u) ]" ), dwObjId, m_dwItemMax );
				FLItemElem kTmpItemElem;
				kTmpItemElem.Deserialize( ar );
			}
		}

		ar >> m_bAllocedSerialNumber;
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ CRITICAL ERROR. IsLoading() FALSE ]" ) );
		return;
	}
}

template <class T> BOOL CItemContainer<T>::DoEquip( DWORD dwSrcObjIndex, DWORD dwDestObjIndex )
{ 
	if( dwSrcObjIndex < MAX_HUMAN_PARTS || dwDestObjIndex >= MAX_HUMAN_PARTS )
	{
		return FALSE;
	}

	if( dwSrcObjIndex == dwDestObjIndex || dwSrcObjIndex >= m_dwItemMax || dwDestObjIndex >= m_dwItemMax )
	{
		return FALSE;
	}

	if( m_apIndex[ dwSrcObjIndex ] != NULL_ID && m_apIndex[ dwDestObjIndex ] == NULL_ID )
	{
		DWORD dwObjId = m_apIndex[ dwSrcObjIndex ];
		if( dwObjId < m_dwItemMax )
		{
			m_apItem[ dwObjId ].m_dwObjIndex = dwDestObjIndex;
			m_apIndex[ dwSrcObjIndex ] = NULL_ID;
			m_apIndex[ dwDestObjIndex ] = dwObjId;

			return TRUE;
		}
	}

	return FALSE;
}

template <class T> BOOL CItemContainer<T>::UnEquip( DWORD dwSrcObjIndex )
{
	if( dwSrcObjIndex >= MAX_HUMAN_PARTS )
	{
		return FALSE;
	}

	DWORD dwObjId = m_apIndex[ dwSrcObjIndex ];

	if( dwObjId == NULL_ID || dwObjId >= m_dwItemMax )
	{
		return FALSE;
	}

	DWORD dwStart = 0;
	DWORD dwEnd = 0;
	FLItemElem* pItemElem = &m_apItem[ dwObjId ];
	if( pItemElem != NULL && pItemElem->GetProp() != NULL )
	{
		GetObjIndexRange( pItemElem->GetProp(), &dwStart, &dwEnd );
	}

	for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
	{
		if( m_apIndex[ dwObjIndex ] == NULL_ID )
		{
			m_apIndex[ m_apItem[ dwObjId ].m_dwObjIndex ] = NULL_ID;
			m_apItem[ dwObjId ].m_dwObjIndex = dwObjIndex;
			m_apIndex[ dwObjIndex ] = dwObjId;

			return TRUE;
		}
	}

	return FALSE;
}

template <class T> BOOL CItemContainer<T>::IsEquip( DWORD dwObjId )
{
	if( !m_apItem )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "CONTAINER//0" ) );
		return FALSE;
	}

	if( dwObjId >= m_dwItemMax )
	{
		return FALSE;
	}

	if( m_apItem[ dwObjId ].m_dwObjIndex < MAX_HUMAN_PARTS )
	{
		return TRUE;
	}

	return FALSE;
}

template <class T> T* CItemContainer<T>::GetEquip( DWORD dwObjIndex )
{
	if( dwObjIndex < MAX_HUMAN_PARTS )
	{
		return GetAt( dwObjIndex );
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template <class T> T* CItemContainer<T>::GetAt( DWORD dwObjIndex ) const
{
	if( dwObjIndex < m_dwItemMax )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE ) 
			{
				return pItemElem;
			}
		}
	}

	return NULL;
}

template <class T> T* CItemContainer<T>::GetAtId( DWORD dwObjId ) const
{
	if( dwObjId < m_dwItemMax )
	{
		T* pItemElem = &m_apItem[ dwObjId ];
		if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE )
		{
			return pItemElem;
		}
	}

	return NULL;
}

template <class T> DWORD CItemContainer<T>::GetObjIdByObjIndex( DWORD dwObjIndex )
{
	DWORD dwObjId = NULL_ID;

	if( dwObjIndex < m_dwItemMax )
	{
		dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId >= m_dwItemMax )
		{
			dwObjId = NULL_ID;
		}
	}

	return dwObjId;
}

template <class T> DWORD CItemContainer<T>::GetObjIndexByObjId( DWORD dwObjId )
{
	DWORD dwObjIndex = NULL_ID;

	if( dwObjId < m_dwItemMax )
	{
		dwObjIndex = m_apItem[ dwObjId ].m_dwObjIndex;
		if( dwObjIndex >= m_dwItemMax )
		{
			dwObjIndex = NULL_ID;
		}
	}

	return dwObjIndex;
}

template <class T> void CItemContainer<T>::SetAtId( DWORD dwObjId, T* pItemElem )
{
	if( pItemElem == NULL )
	{
		return;
	}

	if( dwObjId >= m_dwItemMax || pItemElem->m_dwObjIndex >= m_dwItemMax )
	{
		return;
	}

	m_apItem[ dwObjId ]	= *pItemElem;
	m_apItem[ dwObjId ].m_dwObjId	= dwObjId;
	m_apItem[ dwObjId ].m_dwObjIndex = pItemElem->m_dwObjIndex;
	m_apIndex[ pItemElem->m_dwObjIndex ] = dwObjId;
}

template <class T> void CItemContainer<T>::RemoveAt( DWORD dwObjIndex )
{
	if( dwObjIndex >= m_dwItemMax )
	{
		return;
	}

	DWORD dwObjId = m_apIndex[ dwObjIndex ];
	if( dwObjId < m_dwItemMax )
	{
		m_apIndex[ dwObjIndex ] = NULL_ID;
		m_apItem[ dwObjId ].m_dwObjIndex = NULL_ID;
		m_apItem[ dwObjId ].Empty();
	}
}

template <class T> void CItemContainer<T>::RemoveAtId( DWORD dwObjId )
{
	if( dwObjId >= m_dwItemMax )
	{
		return;
	}

#ifdef	__WORLDSERVER
	RemoveItem( m_apItem[ dwObjId ].GetSerialNumber(), 0 );
#endif

	DWORD dwObjIndex = m_apItem[ dwObjId ].m_dwObjIndex;
	m_apItem[ dwObjId ].m_dwObjIndex = NULL_ID;
	m_apItem[ dwObjId ].Empty();
	if( dwObjIndex < m_dwItemMax )
	{
		m_apIndex[ dwObjIndex ] = NULL_ID;
	}
}

template <class T> BOOL CItemContainer<T>::IsAlloced()
{
	if( m_apItem == NULL || m_apIndex == NULL || m_nContainerType == CONTAINER_TYPE_NONE )
	{
		return FALSE;
	}

	return TRUE;
}

template <class T> BOOL CItemContainer<T>::IsAllEmpty()
{
	if( m_apItem == NULL || m_apIndex == NULL )
	{
		return TRUE;
	}
	else
	{
		for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
		{
			if( m_apItem[ dwObjId ].IsEmpty() == FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

template <class T> BOOL CItemContainer<T>::IsExistDoNotHaveItemServerTransform()
{
	for( DWORD dwObjIndex = 0; dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE )
			{
				PT_ITEM_SPEC pItemSpec	= pItemElem->GetProp();
				if( pItemSpec != NULL )
				{
					if( pItemSpec->bCanHaveServerTransform == FALSE )
					{
						return TRUE;
					}
				}
				else
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template <class T> T* CItemContainer<T>::GetAtByItemId( DWORD dwItemId )
{
	for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwItemId == dwItemId )
			{
				return pItemElem;
			}
		}
	}

	return NULL;
}

template <class T> T* CItemContainer<T>::GetAtByItemKind3( DWORD dwItemKind3 )
{
	for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->GetProp() != NULL && pItemElem->GetProp()->dwItemKind3 == dwItemKind3 )
			{
				return pItemElem;
			}
		}
	}

	return NULL;
}

template <class T> int CItemContainer<T>::GetItemNumByItemId( DWORD dwItemId )
{
	int nItemNum = 0;

	if( dwItemId == 0 || dwItemId == NULL_ID )
	{
		return nItemNum;
	}

	for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwItemId == dwItemId )
			{
				nItemNum += pItemElem->m_nItemNum;
			}
		}
	}

	return nItemNum;
}

template<class T> int CItemContainer<T>::GetItemNumByItemKind3( DWORD dwItemKind3 )
{
	int nItemNum = 0;

	for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE )
			{
				PT_ITEM_SPEC pItemProp = pItemElem->GetProp();
				if( pItemProp != NULL && pItemProp->dwItemKind3 == dwItemKind3 )
				{
					nItemNum += pItemElem->m_nItemNum;
				}
			}
		}
	}

	return nItemNum;
}

template<class T> int CItemContainer<T>::GetCountByItemId( DWORD dwItemId )
{
	int nCount = 0;

	if( dwItemId == 0 || dwItemId == NULL_ID )
	{
		return nCount;
	}

	for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
		{
			T* pItemElem = &m_apItem[ dwObjId ];
			if( pItemElem != NULL && pItemElem->IsEmpty() == FALSE && pItemElem->m_dwItemId == dwItemId )
			{
				++nCount;
			}
		}
	}

	return nCount;
}

template<class T> bool CItemContainer<T>::CheckItem( std::vector<T_ADDITEM_INFO> & vecAddItemInfo )
{
	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		std::map<int,int> mapItem;
		std::map<int,int>::iterator it;

		std::vector<T_ADDITEM_INFO> vecItem; 
		for( int i = 0; i < (int)( vecAddItemInfo.size() ); ++i )
		{
			T_ADDITEM_INFO tAddItemInfo;
			int nItemId = vecAddItemInfo[i].dwItemId;
			int nNum = vecAddItemInfo[i].nItemNum;
			
			int nItemNum = GetItemNumByItemId ( nItemId );
			
			it = mapItem.find(nItemId);
			if ( it == mapItem.end() )
			{
				mapItem[nItemId] = nItemNum;
				continue;
			}
			else
			{
				mapItem[nItemId] = it->second - nNum;
			}
			/*if( !mapItem.find(nItemId) )
				mapItem[nItemId] = nItemNum;
			else
			{
				it = mapItem.find(nItemId);
				mapItem[nItemId] = it->second - nItemNum;
			}
			it = mapItem.find(nItemId);*/

			
			if ( it->second < 0 )
				return FALSE;

		}
	}

	return TRUE;
}

template<class T> bool CItemContainer<T>::IsEnoughEmptySpace( std::vector<T_ADDITEM_INFO> & vecAddItemInfo )
{
	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		int nNoneEmptyCount		= GetEmptyCountByInventoryType( INVEN_TYPE_NONE );
		int nGeneralEmptyCount	= GetEmptyCountByInventoryType( INVEN_TYPE_GENERAL );
		int nPetEmptyCount		= GetEmptyCountByInventoryType( INVEN_TYPE_PET );
		int nCostumeEmptyCount	= GetEmptyCountByInventoryType( INVEN_TYPE_COSTUME );
		int nQuestEmptyCount	= GetEmptyCountByInventoryType( INVEN_TYPE_QUEST );

		for( int i = 0; i < (int)( vecAddItemInfo.size() ); ++i )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( vecAddItemInfo[i].dwItemId );
			if( pItemProp )
			{
				switch( pItemProp->GetInventoryType() )
				{
				case INVEN_TYPE_NONE:
					{
						nNoneEmptyCount -= (int)( vecAddItemInfo[i].nItemNum / pItemProp->dwPackMax );
						if( (int)( vecAddItemInfo[i].nItemNum % pItemProp->dwPackMax ) > 0 )
						{
							if( --nNoneEmptyCount < 0 )
							{
								return false;
							}
						}
					}
					break;

				case INVEN_TYPE_GENERAL:
					{
						nGeneralEmptyCount -= (int)( vecAddItemInfo[i].nItemNum / pItemProp->dwPackMax );
						if( (int)( vecAddItemInfo[i].nItemNum % pItemProp->dwPackMax ) > 0 )
						{
							if( --nGeneralEmptyCount < 0 )
							{
								return false;
							}
						}
					}
					break;

				case INVEN_TYPE_PET:
					{
						nPetEmptyCount -= (int)( vecAddItemInfo[i].nItemNum / pItemProp->dwPackMax );
						if( (int)( vecAddItemInfo[i].nItemNum % pItemProp->dwPackMax ) > 0 )
						{
							if( --nPetEmptyCount < 0 )
							{
								return false;
							}
						}
					}
					break;

				case INVEN_TYPE_COSTUME:
					{
						nCostumeEmptyCount -= (int)( vecAddItemInfo[i].nItemNum / pItemProp->dwPackMax );
						if( (int)( vecAddItemInfo[i].nItemNum % pItemProp->dwPackMax ) > 0 )
						{
							if( --nCostumeEmptyCount < 0 )
							{
								return false;
							}
						}
					}
					break;

				case INVEN_TYPE_QUEST:
					{
						nQuestEmptyCount -= (int)( vecAddItemInfo[i].nItemNum / pItemProp->dwPackMax );
						if( (int)( vecAddItemInfo[i].nItemNum % pItemProp->dwPackMax ) > 0 )
						{
							if( --nQuestEmptyCount < 0 )
							{
								return false;
							}
						}
					}
					break;

				default:
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if( nNoneEmptyCount >= 0 && nGeneralEmptyCount >= 0 && nPetEmptyCount >= 0 && nCostumeEmptyCount >= 0 && nQuestEmptyCount >= 0 )
		{
			return true;
		}
	}
	else
	{
		int nEmptyCount = GetEmptyCountByInventoryType( INVEN_TYPE_NONE );

		for( int i = 0; i < (int)( vecAddItemInfo.size() ); ++i )
		{
			PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( vecAddItemInfo[i].dwItemId );
			if( pItemProp )
			{
				nEmptyCount -= (int)( vecAddItemInfo[i].nItemNum / pItemProp->dwPackMax );
				if( (int)( vecAddItemInfo[i].nItemNum % pItemProp->dwPackMax ) > 0 )
				{
					if( --nEmptyCount < 0 )
					{
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		}

		if( nEmptyCount >= 0 )
		{
			return true;
		}
	}

	return false;
}

template<class T> int CItemContainer<T>::GetEmptyCountByInventoryType( int nInventoryType )
{
	int nCount = 0;

	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		DWORD dwStart = 0;
		DWORD dwEnd = 0;
		GetObjIndexRange( nInventoryType, &dwStart, &dwEnd );

		for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
		{
			if( m_apIndex[ dwObjIndex ] == NULL_ID )
			{
				++nCount;
			}
		}
	}
	else
	{
		for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
		{
			if( m_apIndex[ dwObjIndex ] == NULL_ID )
			{
				++nCount;
			}
		}
	}

	return nCount;
}

template<class T> int CItemContainer<T>::GetEmptyCountByItemId( DWORD dwItemId )
{
	int nCount = 0;

	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( dwItemId );
	if( pItemProp != NULL )
	{
		if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
		{
			DWORD dwStart = 0;
			DWORD dwEnd = 0;
			GetObjIndexRange( pItemProp, &dwStart, &dwEnd );

			for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
			{
				if( m_apIndex[ dwObjIndex ] == NULL_ID )
				{
					++nCount;
				}
			}
		}
		else
		{
			for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
			{
				if( m_apIndex[ dwObjIndex ] == NULL_ID )
				{
					++nCount;
				}
			}
		}
	}

	return nCount;
}

template <class T> T* CItemContainer<T>::GetItem( PT_ITEM_SPEC pItemProp, int nAbilityOption )
{
	if( pItemProp != NULL )
	{
		for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
		{
			DWORD dwObjId = m_apIndex[ dwObjIndex ];
			if( dwObjId < m_dwItemMax && m_apItem[ dwObjId ].IsEmpty() == FALSE )
			{
				T* pItemElem = &m_apItem[ dwObjId ];
				if( pItemElem != NULL && pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetAbilityOption() == nAbilityOption )
				{
					return pItemElem;
				}
			}
		}
	}

	return NULL;
}

template <class T> T* CItemContainer<T>::GetItem( PT_ITEM_SPEC pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption )
{
	if( pItemProp != NULL )
	{
		for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
		{
			DWORD dwObjId = m_apIndex[ dwObjIndex ];
			if( dwObjId < m_dwItemMax && m_apItem[ dwObjId ].IsEmpty() == FALSE )
			{
				T* pItemElem = &m_apItem[ dwObjId ];
				if( pItemElem != NULL && pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetAbilityOption() == nAbilityOption
					&& pItemElem->m_byItemResist == nItemResist && pItemElem->m_nResistAbilityOption == nResistAbilityOption )
				{
					return pItemElem;
				}
			}
		}
	}

	return NULL;
}

template <class T> DWORD CItemContainer<T>::GetObjIdByItemId( DWORD dwItemId )
{
	DWORD dwObjId = NULL_ID;

	if( dwItemId == 0 || dwItemId == NULL_ID )
	{
		return dwObjId;
	}

	for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
	{
		DWORD dwId	= m_apIndex[ dwObjIndex ];
		if( dwId != NULL_ID || dwId < m_dwItemMax )
		{
			if( m_apItem[ dwId ].m_dwItemId == dwItemId )
			{
				dwObjId = m_apItem[ dwId ].m_dwObjId;
				break;
			}
		}
	}

	return dwObjId;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template <class T> bool CItemContainer<T>::Swap( DWORD dwSrcObjIndex, DWORD dwDestObjIndex )
{
	if( dwSrcObjIndex == dwDestObjIndex || dwSrcObjIndex >= m_dwItemMax || dwDestObjIndex >= m_dwItemMax )
	{
		return false;
	}

// 	if( dwSrcObjIndex < MAX_HUMAN_PARTS || dwDestObjIndex < MAX_HUMAN_PARTS )
// 	{
// 		return false;
// 	}

	FLItemElem* pSrFLItemElem		= (FLItemElem*)GetAtId( m_apIndex[ dwSrcObjIndex ] );
	FLItemElem* pDestItemElem	= (FLItemElem*)GetAtId( m_apIndex[ dwDestObjIndex ] );

	if( pSrFLItemElem != NULL )
	{
		if( pDestItemElem != NULL )										// 아이템이 있는 곳으로..
		{
			if( pDestItemElem->IsStackable( pSrFLItemElem ) == true )	// 아이템이 합쳐진다..
			{
				return AddItemToItem( dwSrcObjIndex, dwDestObjIndex );
			}
			else														// 아이템끼리 자리 교환
			{
				return ChangeItemToItem( dwSrcObjIndex, dwDestObjIndex );
			}
		}
		else															// 아이템이 없는 곳으로..
		{
			return MoveItemToEmpty( dwSrcObjIndex, dwDestObjIndex );
		}
	}
	else
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "SrcItem is Invalid" ) );
		return false;
	}

//	return false;
}

template <class T> BOOL CItemContainer<T>::IsFull( T* pElem, int nNum )
{
	if( pElem == NULL || nNum <= 0 )
	{
		return TRUE;
	}

	PT_ITEM_SPEC pItemProp = pElem->GetProp();
	if( pItemProp == NULL )
	{
		return TRUE;
	}

	int nNumtmp = nNum;

	DWORD dwStart = 0;
	DWORD dwEnd = 0;

	GetObjIndexRange( pItemProp, &dwStart, &dwEnd );

	if( dwStart == 0 && dwEnd == 0 )
	{
		return TRUE;
	}

	for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
	{
		DWORD dwObjId = m_apIndex[ dwObjIndex ];
		if( dwObjId == NULL_ID )
		{
			if( nNumtmp > (int)pItemProp->dwPackMax )
			{
				nNumtmp		-= (int)pItemProp->dwPackMax;
			}
			else
			{
				return FALSE;
			}
		}
		else if( dwObjId < m_dwItemMax )
		{
			FLItemElem* pElemtmp = (FLItemElem*)&m_apItem[ dwObjId ];
			if( pElemtmp != NULL && pElemtmp->IsEmpty() == FALSE && pElemtmp->IsStackable( (FLItemElem*)pElem ) == true )
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (int)pItemProp->dwPackMax )
				{
					nNumtmp	-= ( (int)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
				}
				else
				{
					return FALSE;
				}
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "CItemContainer<T>::IsFull." ) );
			return TRUE;
		}
	}

	return TRUE;
}

template <class T> BOOL CItemContainer<T>::Add( T* pElem )
{
	if( pElem == NULL )
	{
		return FALSE;
	}

	PT_ITEM_SPEC pItemProp = pElem->GetProp();
	if( pItemProp == NULL )
	{
		return FALSE;
	}

	DWORD dwStart = 0;
	DWORD dwEnd = 0;
	GetObjIndexRange( pItemProp, &dwStart, &dwEnd );
	if( dwStart == 0 && dwEnd == 0 )
	{
		return FALSE;
	}

	int nNumtmp	= ( (FLItemElem*)pElem )->m_nItemNum;
	if( IsFull( pElem, nNumtmp ) == TRUE )
	{
		return FALSE;
	}

	BOOL bPackItem = FALSE;
	if( pItemProp->dwPackMax > 1 )
	{
		bPackItem = TRUE;
	}

	FLItemElem* pElemtmp = NULL;

	if( bPackItem )
	{
		for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
		{
			DWORD dwObjId = m_apIndex[ dwObjIndex ];
			if( dwObjId >= m_dwItemMax )
			{
				continue;
			}

			pElemtmp	= (FLItemElem*)&m_apItem[ dwObjId ];

			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->IsStackable( (FLItemElem*)pElem ) == true )
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (int)pItemProp->dwPackMax )
				{
					nNumtmp					-= ( (int)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
					pElemtmp->m_nItemNum	= (int)pItemProp->dwPackMax;
				}
				else
				{
					pElemtmp->m_nItemNum	+= nNumtmp;

					nNumtmp = 0;
					break;
				}
			}
		}
	}

	if( nNumtmp > 0 )
	{
		for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
		{
			pElemtmp = (FLItemElem*)&m_apItem[ dwObjId ];
			if( pElemtmp->IsEmpty() )	// 빈공간인지 검사
			{
				volatile DWORD dwObjIndex = NULL_ID;
				for( DWORD j = dwStart; j < dwEnd; ++j )
				{
					if( m_apIndex[ j ] == NULL_ID )
					{
						dwObjIndex = j;
						break;
					}
				}

				if( dwObjIndex < m_dwItemMax )
				{
					*pElemtmp				= *( (FLItemElem*)pElem );
					pElemtmp->m_dwObjId		= dwObjId;
					pElemtmp->m_dwObjIndex	= dwObjIndex;
					m_apIndex[ dwObjIndex ] = dwObjId;

#ifndef __DBSERVER
					pElemtmp->SetTexture();
#ifdef	__WORLDSERVER
					InsertItem( pElemtmp->GetSerialNumber(), 0 );
#endif // __WORLDSERVER
#endif // !__DBSERVER

					if( nNumtmp > (int)pItemProp->dwPackMax )
					{
						pElemtmp->m_nItemNum	= (int)pItemProp->dwPackMax;
						nNumtmp					-= (int)pItemProp->dwPackMax;

// #ifdef __WORLDSERVER
// 						pElem->SetSerialNumber();
// #endif // __WORLDSERVER
					}
					else
					{
						pElemtmp->m_nItemNum	= nNumtmp;
						nNumtmp	= 0;

						break;
					}
				}
			}
		}
	}

	if( nNumtmp > 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nNumtmp > 0. ItemId:[%d], Num:[%d]" ), pElem->m_dwItemId, pElem->m_nItemNum );
	}

	return TRUE;
}

template <class T> BOOL CItemContainer<T>::Add( T* pElem, DWORD* pItemObjID, int* pItemQuantity, DWORD* pCount )
{
	if( pElem == NULL || pItemObjID == NULL || pItemQuantity == NULL || pCount == NULL )
	{
		return FALSE;
	}

	PT_ITEM_SPEC pItemProp = pElem->GetProp();
	if( pItemProp == NULL )
	{
		return FALSE;
	}

	DWORD dwStart = 0;
	DWORD dwEnd = 0;
	GetObjIndexRange( pItemProp, &dwStart, &dwEnd );
	if( dwStart == 0 && dwEnd == 0 )
	{
		return FALSE;
	}

	int nNumtmp	= ( (FLItemElem*)pElem )->m_nItemNum;
	if( IsFull( pElem, nNumtmp ) == TRUE )
	{
		return FALSE;
	}

	BOOL bPackItem = FALSE;
	if( pItemProp->dwPackMax > 1 )
	{
		bPackItem = TRUE;
	}

	FLItemElem* pElemtmp = NULL;
	*pCount				= 0;

	if( bPackItem )
	{
		for( DWORD dwObjIndex = dwStart; dwObjIndex < dwEnd; ++dwObjIndex )
		{
			DWORD dwObjId = m_apIndex[ dwObjIndex ];
			if( dwObjId >= m_dwItemMax )
			{
				continue;
			}

			pElemtmp	= (FLItemElem*)&m_apItem[ dwObjId ];

			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->IsStackable( (FLItemElem*)pElem ) == true )	// 해당 인벤이 비어있지않고 겹칠 수 있을 때
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (int)pItemProp->dwPackMax )
				{
					nNumtmp					-= ( (int)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
					pElemtmp->m_nItemNum	= (int)pItemProp->dwPackMax;

					pItemObjID[*pCount]		= dwObjId;
					pItemQuantity[*pCount]	= pElemtmp->m_nItemNum;
					++( *pCount );
				}
				else
				{
					pElemtmp->m_nItemNum	+= nNumtmp;

					pItemObjID[*pCount]		= dwObjId;
					pItemQuantity[*pCount]	= pElemtmp->m_nItemNum;
					++( *pCount );
					
					nNumtmp = 0;
					break;
				}
			}
		}
	}

	if( nNumtmp > 0 )
	{
		for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
		{
			pElemtmp = (FLItemElem*)&m_apItem[ dwObjId ];
			if( pElemtmp->IsEmpty() )	// 빈공간인지 검사
			{
				volatile DWORD dwObjIndex = NULL_ID;
				for( DWORD j = dwStart; j < dwEnd; ++j )
				{
					if( m_apIndex[ j ] == NULL_ID )
					{
						dwObjIndex = j;
						break;
					}
				}
				
				if( dwObjIndex < m_dwItemMax )
				{
					*pElemtmp				= *( (FLItemElem*)pElem );
					pElemtmp->m_dwObjId		= dwObjId;
					pElemtmp->m_dwObjIndex	= dwObjIndex;
					m_apIndex[ dwObjIndex ] = dwObjId;

#ifndef __DBSERVER
					pElemtmp->SetTexture();
#ifdef	__WORLDSERVER
					InsertItem( pElemtmp->GetSerialNumber(), 0 );
#endif // __WORLDSERVER
#endif // !__DBSERVER

					if( nNumtmp > (int)pItemProp->dwPackMax )
					{
						pElemtmp->m_nItemNum	= (int)pItemProp->dwPackMax;
						nNumtmp					-= (int)pItemProp->dwPackMax;

						pItemObjID[*pCount]		= dwObjId;
						pItemQuantity[*pCount]	= pElemtmp->m_nItemNum;
						++( *pCount );

// #ifdef __WORLDSERVER
// 						pElem->SetSerialNumber();
// #endif // __WORLDSERVER
					}
					else
					{
						pElemtmp->m_nItemNum	= nNumtmp;
						nNumtmp	= 0;

						pItemObjID[*pCount]		= dwObjId;
						pItemQuantity[*pCount]	= pElemtmp->m_nItemNum;
						++( *pCount );

						break;
					}
				}
			}
		}
	}

	if( nNumtmp > 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "nNumtmp > 0. ItemId:[%d], Num:[%d]" ), pElem->m_dwItemId, pElem->m_nItemNum );
	}

	return TRUE;
}

template <class T> void CItemContainer<T>::Copy( const CItemContainer<T> & rItemContainer )
{
	if( m_dwItemMax != rItemContainer.m_dwItemMax || m_dwIndexNum != rItemContainer.m_dwIndexNum )
	{
		return;
	}
	
	memcpy( (void*)m_apIndex, (void*)rItemContainer.m_apIndex, sizeof(DWORD) * m_dwItemMax );
	
	for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
	{
		if( rItemContainer.m_apItem[ dwObjId ].IsEmpty() == FALSE )
		{
			m_apItem[ dwObjId ]		= rItemContainer.m_apItem[ dwObjId ];
		}
		else
		{
			m_apItem[ dwObjId ].Empty();
		}
	}

	m_bAllocedSerialNumber = rItemContainer.m_bAllocedSerialNumber;
}

template <class T> DWORD CItemContainer<T>::GetFirstObjIndex( int nInventoryType )
{
	DWORD dwObjIndex = NULL_ID;

	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		switch( nInventoryType )
		{
		case INVEN_TYPE_NONE:
			dwObjIndex = ( m_dwItemMax - m_dwIndexNum );
			break;

		case INVEN_TYPE_GENERAL:
			dwObjIndex = ( m_dwItemMax - m_dwIndexNum );
			break;

		case INVEN_TYPE_PET:
			dwObjIndex = ( m_dwItemMax - m_dwIndexNum + MAX_INVENTORY_GENERAL );
			break;

		case INVEN_TYPE_COSTUME:
			dwObjIndex = ( m_dwItemMax - m_dwIndexNum + MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET );
			break;

		case INVEN_TYPE_QUEST:
			dwObjIndex = ( m_dwItemMax - m_dwIndexNum + MAX_INVENTORY_GENERAL + MAX_INVENTORY_PET + MAX_INVENTORY_COSTUME );
			break;

		default:
			dwObjIndex = NULL_ID;
			break;
		}
	}
	else
	{
		dwObjIndex = ( m_dwItemMax - m_dwIndexNum );
	}

	return dwObjIndex;
}

template <class T> DWORD CItemContainer<T>::GetSize()
{
	return m_dwIndexNum;
}

template <class T> DWORD CItemContainer<T>::GetMax()
{
	return m_dwItemMax;
}

template <class T> int CItemContainer<T>::GetContainerType()
{
	return m_nContainerType;
}

template <class T> BOOL CItemContainer<T>::GetAllocedSerialNumber()
{
	return m_bAllocedSerialNumber;
}

template <class T> void CItemContainer<T>::SetAllocedSerialNumber( BOOL bAllocedSerialNumber )
{
	m_bAllocedSerialNumber = bAllocedSerialNumber;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef __DBSERVER
// 17차 인벤토리 확장
// 기존 인벤의 순서는 인벤 -> 장착 순이다.
// 인벤토리가 확장되면서 차후 확장을 용이하게 하기 위해 장착 -> 인벤 순으로 변경
template <class T> void CItemContainer<T>::ConvertInventoryIndex( DWORD dwOldInvenSize, DWORD dwOldPartsSize )
{
	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		Init();

		// 인벤 -> 장착 순에서 장착 -> 인벤 순으로 변경
		for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
		{
			if( m_apItem[ dwObjId ].IsEmpty() == FALSE && m_apItem[ dwObjId ].m_dwObjIndex != NULL_ID )
			{
				DWORD dwNewObjIndex = 0;
				
				// 장착중인 아이템인가..
				if( m_apItem[ dwObjId ].m_dwObjIndex >= dwOldInvenSize && m_apItem[ dwObjId ].m_dwObjIndex < ( dwOldInvenSize + dwOldPartsSize ) )
				{
					dwNewObjIndex = m_apItem[ dwObjId ].m_dwObjIndex - dwOldInvenSize;

					m_apItem[ dwObjId ].m_dwObjIndex = dwNewObjIndex;
					m_apIndex[ dwNewObjIndex ] = dwObjId;
				}
				// 인벤에 있는 아이템인가..
				else if( m_apItem[ dwObjId ].m_dwObjIndex < dwOldInvenSize )
				{
					dwNewObjIndex = m_apItem[ dwObjId ].m_dwObjIndex + MAX_HUMAN_PARTS;

					if( dwNewObjIndex < m_dwItemMax )
					{
						m_apItem[ dwObjId ].m_dwObjIndex = dwNewObjIndex;
						m_apIndex[ dwNewObjIndex ] = dwObjId;
					}
				}
			}
		}

		Init();
	}
}

template <class T> void CItemContainer<T>::RefreshIndex()
{
	Init();

	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		// 탭 분류에 맞게 dwObjIndex 변경..
		for( DWORD dwObjIndex = ( m_dwItemMax - m_dwIndexNum ); dwObjIndex < m_dwItemMax; ++dwObjIndex )
		{
			DWORD dwObjId = m_apIndex[ dwObjIndex ];
			if( dwObjId != NULL_ID && dwObjId < m_dwItemMax )
			{
				if( m_apItem[ dwObjId ].IsEmpty() == FALSE && m_apItem[ dwObjId ].m_dwObjIndex != NULL_ID )
				{
					DWORD dwStart = 0;
					DWORD dwEnd = 0;

					GetObjIndexRange( m_apItem[ dwObjId ].GetProp(), &dwStart, &dwEnd );

					if( m_apItem[ dwObjId ].m_dwObjIndex < dwStart || m_apItem[ dwObjId ].m_dwObjIndex >= dwEnd )
					{
						for( DWORD dwNewObjIndex = dwStart; dwNewObjIndex < m_dwItemMax; ++dwNewObjIndex )
						{
							if( m_apIndex[ dwNewObjIndex ] == NULL_ID )
							{
								m_apIndex[ m_apItem[ dwObjId ].m_dwObjIndex ] = NULL_ID;
								m_apItem[ dwObjId ].m_dwObjIndex = dwNewObjIndex;
								m_apIndex[ dwNewObjIndex ] = m_apItem[ dwObjId ].m_dwObjId;

								break;
							}
						}
					}
				}
			}
		}

		Init();
	}
}
#endif // __DBSERVER
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
template <class T> void CItemContainer<T>::ReallocSerialNumber()
{
	if( m_bAllocedSerialNumber == FALSE )
	{
		for( DWORD i = 0; i < m_dwItemMax; i++ )
		{
			if( m_apItem[i].IsEmpty() == FALSE )
			{
				m_apItem[i].SetSerialNumber();
			}
		}

		m_bAllocedSerialNumber = TRUE;
	}
}

template <class T> void	CItemContainer<T>::InsertAllItem()
{
	for( DWORD i = 0; i < m_dwItemMax; i++ )
	{
		if( m_apItem[i].IsEmpty() == FALSE )
		{
			InsertItem( m_apItem[i].GetSerialNumber(), 0 );
		}
	}
}

template <class T> void	CItemContainer<T>::RemoveAllItem()
{
	for( DWORD i = 0; i < m_dwItemMax; i++ )
	{
		if( m_apItem[i].IsEmpty() == FALSE )
		{
			RemoveItem( m_apItem[i].GetSerialNumber(), 0 );
		}
	}
}
#endif // WORLDSERVER
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template <class T> void CItemContainer<T>::Init()
{
	for( DWORD i = 0; i < m_dwItemMax; ++i )
	{
		if( m_apItem[ i ].IsEmpty() == TRUE )
		{
			m_apItem[ i ].m_dwObjId		= NULL_ID;
			m_apItem[ i ].m_dwObjIndex	= NULL_ID;
		}
		m_apIndex[ i ] = NULL_ID;
	}

	for( DWORD dwObjId = 0; dwObjId < m_dwItemMax; ++dwObjId )
	{
		DWORD dwObjIndex = m_apItem[ dwObjId ].m_dwObjIndex;
		if( dwObjIndex != NULL_ID && dwObjIndex < m_dwItemMax )
		{
			m_apIndex[ dwObjIndex ] = dwObjId;
		}
	}
}

template <class T> bool CItemContainer<T>::GetObjIndexRange( const T_ITEM_SPEC* pItemProp, DWORD* pStart, DWORD* pEnd ) const
{
	if( pItemProp == NULL )
	{
		*pStart = 0;
		*pEnd	= 0;
		return false;
	}
	else
	{
		if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
		{
			int nInventoryType = pItemProp->GetInventoryType();

			switch( nInventoryType )
			{
			case INVEN_TYPE_NONE:
				*pStart = m_dwItemMax - m_dwIndexNum;
				*pEnd	= m_dwItemMax;
				break;

			case INVEN_TYPE_GENERAL:
				*pStart	= MAX_HUMAN_PARTS;
				*pEnd	= MAX_INVENINDEX_GENERAL;
				break;

			case INVEN_TYPE_PET:
				*pStart = MAX_INVENINDEX_GENERAL;
				*pEnd	= MAX_INVENINDEX_PET;
				break;

			case INVEN_TYPE_COSTUME:
				*pStart = MAX_INVENINDEX_PET;
				*pEnd	= MAX_INVENINDEX_COSTUME;
				break;

			case INVEN_TYPE_QUEST:
				*pStart = MAX_INVENINDEX_COSTUME;
				*pEnd	= MAX_INVENINDEX_QUEST;
				break;

			default:
				*pStart = 0;
				*pEnd	= 0;
				break;
			}
		}
		else
		{
			*pStart = 0;
			*pEnd	= m_dwItemMax;
		}
	}

	if( *pStart >= m_dwItemMax || *pEnd > m_dwItemMax )
	{
		*pStart = 0;
		*pEnd	= 0;
	}

	if( *pStart == 0 && *pEnd == 0 )
	{
		return false;
	}

	return true;
}

template <class T> bool CItemContainer<T>::GetObjIndexRange( int nInventoryType, DWORD* pStart, DWORD* pEnd ) const
{
	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		switch( nInventoryType )
		{
		case INVEN_TYPE_NONE:
			*pStart = m_dwItemMax - m_dwIndexNum;
			*pEnd	= m_dwItemMax;
			break;

		case INVEN_TYPE_GENERAL:
			*pStart	= MAX_HUMAN_PARTS;
			*pEnd	= MAX_INVENINDEX_GENERAL;
			break;

		case INVEN_TYPE_PET:
			*pStart = MAX_INVENINDEX_GENERAL;
			*pEnd	= MAX_INVENINDEX_PET;
			break;

		case INVEN_TYPE_COSTUME:
			*pStart = MAX_INVENINDEX_PET;
			*pEnd	= MAX_INVENINDEX_COSTUME;
			break;

		case INVEN_TYPE_QUEST:
			*pStart = MAX_INVENINDEX_COSTUME;
			*pEnd	= MAX_INVENINDEX_QUEST;
			break;

		default:
			*pStart = 0;
			*pEnd	= 0;
			break;
		}
	}
	else
	{
		*pStart = 0;
		*pEnd	= m_dwItemMax;
	}

	if( *pStart >= m_dwItemMax || *pEnd > m_dwItemMax )
	{
		*pStart = 0;
		*pEnd	= 0;
	}

	if( *pStart == 0 && *pEnd == 0 )
	{
		return false;
	}

	return true;
}

// 아이템을 합치는 경우
template <class T> bool CItemContainer<T>::AddItemToItem( DWORD dwSrcObjIndex, DWORD dwDestObjIndex )
{
	FLItemElem* pSrFLItemElem		= (FLItemElem*)GetAtId( m_apIndex[ dwSrcObjIndex ] );
	FLItemElem* pDestItemElem	= (FLItemElem*)GetAtId( m_apIndex[ dwDestObjIndex ] );
	if( pSrFLItemElem == NULL || pDestItemElem == NULL )
	{
		return false;
	}

	PT_ITEM_SPEC pSrcItemProp	= pSrFLItemElem->GetProp();
	PT_ITEM_SPEC pDestItemProp	= pDestItemElem->GetProp();
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return false;
	}

	int nMoveNum = (int)pSrcItemProp->dwPackMax - pDestItemElem->m_nItemNum;

	if( nMoveNum > 0 )
	{
		if( nMoveNum >= pSrFLItemElem->m_nItemNum )		// 완전 합체
		{
			pDestItemElem->m_nItemNum += pSrFLItemElem->m_nItemNum;
			RemoveAtId( pSrFLItemElem->m_dwObjId );

			return true;
		}
		else											// 이동할 개수만..
		{
			pDestItemElem->m_nItemNum	+= nMoveNum;
			pSrFLItemElem->m_nItemNum	-= nMoveNum;

			return true;
		}
	}
	else
	{
		return false;
	}

//	return false;
}
#ifdef INVENTORY_ITEM_ALIGN
template <class T> void CItemContainer<T>::Align(DWORD dwIndex)
{
	FLItemElem* pSrFLItemElem;
	FLItemElem* pDestItemElem;

	DWORD dwStart, dwEnd;

	GetObjIndexRange ( dwIndex, &dwStart, &dwEnd );
	for( DWORD i = dwStart; i < dwEnd; i++ )
	{
		for( DWORD j = i+1; j < dwEnd; j++ )
		{
			pSrFLItemElem	= (FLItemElem*)GetAtId( m_apIndex[ i ] );
			pDestItemElem	= (FLItemElem*)GetAtId( m_apIndex[ j ] );
			if( pDestItemElem != NULL )
			{
				int SrcKind = pSrFLItemElem ? pSrFLItemElem->GetKind3() : MAX_ITEM_KIND3;
				int DestKind = pDestItemElem ? pDestItemElem->GetKind3() : MAX_ITEM_KIND3;
				if ( SrcKind > DestKind )
				{
					{
						if ( !pSrFLItemElem )
							MoveItemToEmpty( j, i );		// 아이템이 없는 곳으로..
						else	
							ChangeItemToItem( i, j );
					}
				}
				else if ( SrcKind == DestKind )
				{
					if ( pSrFLItemElem->GetItemIndex() > pDestItemElem->GetItemIndex() )
					{

						ChangeItemToItem( i, j );

					}
					else if ( pSrFLItemElem->GetItemIndex() == pDestItemElem->GetItemIndex() )
					{
						if( pDestItemElem->IsStackable( pSrFLItemElem ) == true )	// 아이템이 합쳐진다..
						{
							AddItemToItem( i, j );
						}
					}
				}
			}
		}
	}

	/*int count = 50;
	FLItemElem* pSrFLItemElem;
	FLItemElem* pDestItemElem;

	for ( int i = 91; i >= 50; i-- )
	{
		pSrFLItemElem	= (FLItemElem*)GetAtId( m_apIndex[ i ] );
		
		if ( pSrFLItemElem == 0 ) continue;

		int temp = count;

		for ( int j = temp; j <= i; j++ )
		{
			pDestItemElem	= (FLItemElem*)GetAtId( m_apIndex[ j ] );
			if ( pDestItemElem == 0 )
			{
				MoveItemToEmpty( i, j );
				break;
			}
			count++;
		}
	}*/

}
#endif	// INVENTORY_ITEM_ALIGN
// 아이템간의 자리 교환 - 인벤일 경우 아이템 종류에 따라 이동 불가
template <class T> bool CItemContainer<T>::ChangeItemToItem( DWORD dwSrcObjIndex, DWORD dwDestObjIndex )
{
	FLItemElem* pSrFLItemElem		= (FLItemElem*)GetAtId( m_apIndex[ dwSrcObjIndex ] );
	FLItemElem* pDestItemElem	= (FLItemElem*)GetAtId( m_apIndex[ dwDestObjIndex ] );

	if( pSrFLItemElem == NULL || pDestItemElem == NULL )
	{
		return false;
	}

	PT_ITEM_SPEC pSrcItemProp	= pSrFLItemElem->GetProp();
	PT_ITEM_SPEC pDestItemProp	= pDestItemElem->GetProp();
	if( pSrcItemProp == NULL || pDestItemProp == NULL )
	{
		return false;
	}

	DWORD dwSrcItemObjId	= m_apIndex[ dwSrcObjIndex ];
	DWORD dwDestItemObjId	= m_apIndex[ dwDestObjIndex ];

	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		if( pSrcItemProp->GetInventoryType() == pDestItemProp->GetInventoryType() )
		{
			m_apItem[ dwSrcItemObjId ].m_dwObjIndex		= dwDestObjIndex;
			m_apIndex[ dwDestObjIndex ]					= dwSrcItemObjId;

			m_apItem[ dwDestItemObjId ].m_dwObjIndex	= dwSrcObjIndex;
			m_apIndex[ dwSrcObjIndex ]					= dwDestItemObjId;

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_apItem[ dwSrcItemObjId ].m_dwObjIndex		= dwDestObjIndex;
		m_apIndex[ dwDestObjIndex ]					= dwSrcItemObjId;

		m_apItem[ dwDestItemObjId ].m_dwObjIndex	= dwSrcObjIndex;
		m_apIndex[ dwSrcObjIndex ]					= dwDestItemObjId;

		return true;
	}

//	return false;
}

// 아이템이 빈곳으로 옮겨지는 경우 - 인벤일 경우 아이템 종류에 따라 이동 불가
template <class T> bool CItemContainer<T>::MoveItemToEmpty( DWORD dwSrcObjIndex, DWORD dwDestObjIndex )
{
	FLItemElem* pSrFLItemElem		= (FLItemElem*)GetAtId( m_apIndex[ dwSrcObjIndex ] );
	if( pSrFLItemElem == NULL )
	{
		return false;
	}

	PT_ITEM_SPEC pSrcItemProp	= pSrFLItemElem->GetProp();
	if( pSrcItemProp == NULL )
	{
		return false;
	}

	DWORD dwSrcItemObjId	= m_apIndex[ dwSrcObjIndex ];

	if( m_nContainerType == CONTAINER_TYPE_INVENTORY )
	{
		if( dwSrcObjIndex < MAX_HUMAN_PARTS && dwDestObjIndex < MAX_HUMAN_PARTS )
		{
			m_apItem[ dwSrcItemObjId ].m_dwObjIndex		= dwDestObjIndex;
			m_apIndex[ dwDestObjIndex ] 				= dwSrcItemObjId;

			m_apIndex[ dwSrcObjIndex ]					= NULL_ID;

			return true;
		}
		else
		{
			DWORD dwStart = 0;
			DWORD dwEnd = 0;
			GetObjIndexRange( pSrcItemProp, &dwStart, &dwEnd );

			if( dwDestObjIndex >= dwStart && dwDestObjIndex < dwEnd )
			{
				m_apItem[ dwSrcItemObjId ].m_dwObjIndex		= dwDestObjIndex;
				m_apIndex[ dwDestObjIndex ] 				= dwSrcItemObjId;

				m_apIndex[ dwSrcObjIndex ]					= NULL_ID;

				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		m_apItem[ dwSrcItemObjId ].m_dwObjIndex		= dwDestObjIndex;
		m_apIndex[ dwDestObjIndex ]					= dwSrcItemObjId;

		m_apIndex[ dwSrcObjIndex ]					= NULL_ID;

		return true;
	}

//	return false;
}
//////////////////////////////////////////////////////////////////////////

#endif // __FLITEMCONTAINER_H__

